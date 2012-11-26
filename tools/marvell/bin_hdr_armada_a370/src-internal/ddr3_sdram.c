/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
		used to endorse or promote products derived from this software without
		specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
#include "mv_os.h"
#include "ddr3_hw_training.h"
#include "mvXor.h"
#include "mvXorRegs.h"
#include "config_marvell.h"  	/* Required to identify SOC and Board */

#define DEBUG_SDRAM_S(s)				putstring(s)
#define DEBUG_SDRAM_D(d, l)				putdata(d, l)

static MV_STATUS  ddr3DramSramBurst(MV_U32 src, MV_U32 dst, MV_U32 len);
static MV_VOID ddr3FlushL1Line(MV_U32 uiLine);

extern MV_U32 g_auiPbsPattern[2][LEN_PBS_PATTERN];

/* PBS locked dq (per pup) */
MV_U32 s_auiPbsLockedDq[MAX_PUP_NUM][DQ_NUM] = {{0}};
MV_U32 s_auiPbsLockedValue[MAX_PUP_NUM][DQ_NUM] = {{0}};
MV_BOOL bPerBitData[MAX_PUP_NUM][DQ_NUM];

static MV_U32 auiSdramData[LEN_KILLER_PATTERN] __attribute__((aligned(32))) = {0};
static MV_CRC_DMA_DESC Virt __attribute__((aligned(32))) = {0};

#define XOR_TIMEOUT 0x8000000

struct xor_channel_t {
	MV_CRC_DMA_DESC *pDescriptor;
	MV_ULONG	descPhyAddr;
};

#define XOR_CAUSE_DONE_MASK(chan) ((BIT0|BIT1) << (chan * 16))
void xor_waiton_eng(int chan)
{
	int timeout = 0;

	while (!(MV_REG_READ(XOR_CAUSE_REG(XOR_UNIT(chan))) & XOR_CAUSE_DONE_MASK(XOR_CHAN(chan))))	{
		if (timeout > XOR_TIMEOUT)
			goto timeout;
		timeout++;
	}

	timeout = 0;
	while (mvXorStateGet(chan) != MV_IDLE) {
		if (timeout > XOR_TIMEOUT)
			goto timeout;
		timeout++;
	}
	/* Clear int */
	MV_REG_WRITE(XOR_CAUSE_REG(XOR_UNIT(chan)), ~(XOR_CAUSE_DONE_MASK(XOR_CHAN(chan))));

timeout:
		/*
		if (timeout > XOR_TIMEOUT)
			printf("ERR: XOR eng got timedout!!\n"); */
		return;
}

/******************************************************************************
* Name:     ddr3SdramCompare.
* Desc:     Execute compare per PUP
* Args:     uiUnlockPup       Bit array of the unlock pups
*           puiNewLockedPup   Output  bit array of the pups with failed compare
*           auiPattern        Pattern to compare
*           uiPatternLen      Length of pattern (in bytes)
*           uiSdramOffset     offset address to the SDRAM
*           bWrite            write to the SDRAM before read
*           bMask             compare pattern with mask;
*           auiMaskPatter     Mask to compare pattern
*
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3SdramCompare(MV_DRAM_INFO *pDramInfo, MV_U32 uiUnlockPup, MV_U32 *puiNewLockedPup, MV_U32 *auiPattern,
				MV_U32 uiPatternLen, MV_U32 uiSdramOffset, MV_BOOL bWrite, MV_BOOL bMask, MV_U32 *auiMaskPattern, MV_BOOL bSpecialCompare)
{
	MV_U32 uj, uk, uiVar1, uiVar2, uiPupGroups;

	if (pDramInfo->uiNumOfStdPups == PUP_NUM_64BIT)
		uiPupGroups = 2;
	else
		uiPupGroups = 1;

	/* check if need to write to sdram before read */
	if (bWrite == TRUE)
		ddr3DramSramBurst((MV_U32)auiPattern, uiSdramOffset, uiPatternLen);

	ddr3DramSramBurst(uiSdramOffset, (MV_U32)auiSdramData, uiPatternLen);

	/* compare read result to write */
	for (uj = 0; uj < uiPatternLen; uj++)	{
		if(bSpecialCompare)
			if ((uj == 30) || (uj == 31) || (uj == 61) || (uj == 62) || (uj == 93) || (uj == 94) ||
						 (uj == 126) || (uj == 127))
				continue;

#ifdef MV88F78X60
		if (((auiSdramData[uj]) != (auiPattern[uj])) && (*puiNewLockedPup != 0xFF)) {
			for (uk = 0; uk < PUP_NUM_32BIT; uk++) {
				uiVar1 = ((auiSdramData[uj] >> (CMP_BYTE_SHIFT*uk)) & CMP_BYTE_MASK);
				uiVar2 = ((auiPattern[uj] >> (CMP_BYTE_SHIFT*uk)) & CMP_BYTE_MASK);
				if (uiVar1 != uiVar2)
					*puiNewLockedPup |= (1 << (uk + (PUP_NUM_32BIT * (uj % uiPupGroups))));
			}
		}
#elif defined(MV88F67XX)
		if (((auiSdramData[uj]) != (auiPattern[uj])) && (*puiNewLockedPup != 0x3)) {
			/* found error */
			for (uk = 0; uk < PUP_NUM_32BIT; uk++) {
				uiVar1 = ((auiSdramData[uj] >> (CMP_BYTE_SHIFT*uk)) & CMP_BYTE_MASK);
				uiVar2 = ((auiPattern[uj] >> (CMP_BYTE_SHIFT*uk)) & CMP_BYTE_MASK);
				if (uiVar1 != uiVar2)
					*puiNewLockedPup |= (1 << (uk % PUP_NUM_16BIT));
			}
		}
#endif

	}

	return MV_OK;

}

/******************************************************************************
* Name: 	ddr3SdramPbsCompare.
* Desc:		Execute SRAM compare per PUP and DQ.
* Args:		uiPupLocked     		bit array of locked pups
*			bIsTx 					Indicate whether Rx or Tx
*			uiPbsPatternIdx     	Index of PBS pattern    
*			uiPbsCurrVal        	The PBS value
*			uiPbsLockVal        	The value to set to locked PBS
*			auiSkewArray        	Global array to update with the compare results 
*			aiUnlockPupDqArray  	bit array of the locked / unlocked pups per dq.
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3SdramPbsCompare(MV_DRAM_INFO *pDramInfo, MV_U32 uiPupLocked, MV_BOOL bIsTx, 
		MV_U32 uiPbsPatternIdx, MV_U32 uiPbsCurrVal, MV_U32 uiPbsLockVal,MV_U32 *auiSkewArray,
		 MV_U8 *auiUnlockPupDqArray )
{
	MV_U32 auiPbsWritePup[DQ_NUM] = {0}; 		/* bit array failed dq per pup for current compare */
	MV_U32 uiUpdatePup;                     	/* pup as HW convention */ 
	MV_U32 uiMaxPup;							/* maximal pup index */
	MV_U32 uiPupAddr;
	MV_U32 ui, uiDq, uiPup, uiReg;
	MV_BOOL uiVar1,uiVar2;
	MV_U32 uiSdramOffset, uiPupGroups, uiTempPup;

	uiMaxPup = pDramInfo->uiNumOfStdPups;
	
	uiSdramOffset = (SDRAM_PBS_I_OFFS + uiPbsPatternIdx*SDRAM_PBS_NEXT_OFFS);
	
	if (pDramInfo->uiNumOfStdPups == PUP_NUM_64BIT)
		uiPupGroups = 2;
	else
		uiPupGroups = 1;

	/* check if need to write to sdram before read */
	if (bIsTx == TRUE) {
		ddr3DramSramBurst((MV_U32)&g_auiPbsPattern[uiPbsPatternIdx], uiSdramOffset, LEN_PBS_PATTERN);
	}

	ddr3DramSramBurst(uiSdramOffset, (MV_U32)auiSdramData, LEN_PBS_PATTERN);
	
	/* compare read result to write */
	for (ui = 0; ui < LEN_PBS_PATTERN; ui++) {
		if (( auiSdramData[ui] ) != ( g_auiPbsPattern[uiPbsPatternIdx][ui] )) {
			/* found error */
			/* error in low pup group  */
			for (uiPup=0;uiPup<PUP_NUM_32BIT;uiPup++) {
				uiVar1 = ((auiSdramData[ui] >> CMP_BYTE_SHIFT*uiPup) & CMP_BYTE_MASK);
				uiVar2 = ((g_auiPbsPattern[uiPbsPatternIdx][ui] >> CMP_BYTE_SHIFT*uiPup) & CMP_BYTE_MASK);
		
				if (uiVar1 != uiVar2) {
#ifdef MV88F78X60
					uiTempPup = (uiPup + PUP_NUM_32BIT * (ui % uiPupGroups));
#elif defined(MV88F67XX)
					uiTempPup = (uiPup % PUP_NUM_16BIT);
#endif
					uiUpdatePup = (1 << uiTempPup);
					/* Pup is failed - Go over all DQs and look for failures */
					for (uiDq = 0; uiDq < DQ_NUM; uiDq++) {
						if (((uiVar1 >> uiDq) & 0x1) != ((uiVar2 >> uiDq) & 0x1)) {
							if (s_auiPbsLockedDq[uiTempPup][uiDq] == TRUE && 
								s_auiPbsLockedValue[uiTempPup][uiDq] != uiPbsCurrVal) {
								continue;
							}
							
							/* activate write to update PBS to pbsLockVal */
							auiPbsWritePup[uiDq] |= uiUpdatePup;

							/* update the unlockPupDqArray */
							auiUnlockPupDqArray[uiDq] &= ~uiUpdatePup;

							/* Lock PBS value for failed bits in compare operation*/
							auiSkewArray[uiTempPup * DQ_NUM + uiDq] = uiPbsCurrVal;
						}
					}
				}
			}
		}
	}

	uiPupAddr = (bIsTx == TRUE) ? PUP_PBS_TX : PUP_PBS_RX;

	/* Set last failed bits PBS to min / max pbs value */
	for (uiDq = 0; uiDq < DQ_NUM; uiDq++ ) {
		for (uiPup = 0; uiPup < uiMaxPup; uiPup++ ) {
			if (auiPbsWritePup[uiDq] & (1 << uiPup)) {
				if (s_auiPbsLockedDq[uiPup][uiDq] == TRUE && s_auiPbsLockedValue[uiPup][uiDq] != uiPbsCurrVal) {
					continue;
				}
				/* mark the dq as locked */
				s_auiPbsLockedDq[uiPup][uiDq] = TRUE;
				s_auiPbsLockedValue[uiPup][uiDq] = uiPbsCurrVal;
				ddr3WritePupReg(uiPupAddr+uiDq,CS0, uiPup, 0, uiPbsLockVal);
			}
		}
	}
	return MV_OK;
}

/******************************************************************************
* Name:     ddr3SdramDirectCompare.
* Desc:     Execute compare per PUP without DMA (no burst mode)
* Args:     uiUnlockPup       Bit array of the unlock pups
*           puiNewLockedPup   Output  bit array of the pups with failed compare
*           auiPattern        Pattern to compare
*           uiPatternLen      Length of pattern (in bytes)
*           uiSdramOffset     offset address to the SDRAM
*           bWrite            write to the SDRAM before read
*           bMask             compare pattern with mask;
*           auiMaskPatter     Mask to compare pattern
*
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

MV_STATUS ddr3SdramDirectCompare(MV_DRAM_INFO *pDramInfo, MV_U32 uiUnlockPup, MV_U32 *puiNewLockedPup,
			MV_U32 *auiPattern, MV_U32 uiPatternLen, MV_U32 uiSdramOffset, MV_BOOL bWrite,
			MV_BOOL bMask,  MV_U32 *auiMaskPattern)
{
	MV_U32 uj, uk, uiVar1, uiVar2, uiPupGroups;
	MV_U32 *auiSdramAddr; 	/* used to read from SDRAM */

	auiSdramAddr = (MV_U32 *)uiSdramOffset;

	if (pDramInfo->uiNumOfStdPups == PUP_NUM_64BIT)
		uiPupGroups = 2;
	else
		uiPupGroups = 1;

	/* check if need to write before read */
	if (bWrite == TRUE) {
		for (uk = 0; uk < uiPatternLen; uk++) {
			*auiSdramAddr = auiPattern[uk];
			auiSdramAddr++;
		}
	}

	auiSdramAddr = (MV_U32 *)uiSdramOffset;

	for (uk = 0; uk < uiPatternLen; uk++) {
		auiSdramData[uk] = *auiSdramAddr;
		auiSdramAddr++;
	}

	/* compare read result to write */
	for (uj = 0; uj < uiPatternLen; uj++)	{
#ifdef MV88F78X60
		if (((auiSdramData[uj]) != (auiPattern[uj])) && (*puiNewLockedPup != 0xFF)) {
			for (uk = 0; uk < PUP_NUM_32BIT; uk++) {
				uiVar1 = ((auiSdramData[uj] >> (CMP_BYTE_SHIFT*uk)) & CMP_BYTE_MASK);
				uiVar2 = ((auiPattern[uj] >> (CMP_BYTE_SHIFT*uk)) & CMP_BYTE_MASK);
				if (uiVar1 != uiVar2)
					*puiNewLockedPup |= (1 << (uk + (PUP_NUM_32BIT * (uj % uiPupGroups))));
			}
		}

#elif defined(MV88F67XX)
		if (((auiSdramData[uj]) != (auiPattern[uj])) && (*puiNewLockedPup != 0x3)) {
			/* found error */
			for (uk = 0; uk < PUP_NUM_32BIT; uk++) {
				uiVar1 = ((auiSdramData[uj] >> (CMP_BYTE_SHIFT*uk)) & CMP_BYTE_MASK);
				uiVar2 = ((auiPattern[uj] >> (CMP_BYTE_SHIFT*uk)) & CMP_BYTE_MASK);
				if (uiVar1 != uiVar2)
					*puiNewLockedPup |= (1 << (uk % PUP_NUM_16BIT));
			}
		}
#endif
	}
	return MV_OK;
}

/******************************************************************************
* Name:     ddr3DramSramBurst
* Desc:     Read from the SDRAM in burst of 64 bytes
* Args:		src
			dst
* Notes:	Using the XOR mechanism
* Returns:  MV_OK if success, other error code if fail.
*/

static MV_STATUS ddr3DramSramBurst(MV_U32 src, MV_U32 dst, MV_U32 len)
{
	MV_U32 chan, byteCount, uiCsNum, uiByte;
	struct xor_channel_t channel;

	chan = 0;
	
	/* byte count */
	byteCount = (len * 4);

	/* wait for previous transfer completion */
	while (mvXorStateGet(chan) != MV_IDLE);

	/* build the channel descriptor */
	channel.pDescriptor = &Virt;

	/* Enable Address Override and set correct Src and Dest */
	if (src < SRAM_BASE) { 		/* Src is DRAM CS, Dst is SRAM */
		uiCsNum = (src / (1 + SDRAM_CS_SIZE));
		MV_REG_WRITE(XOR_ADDR_OVRD_REG(0, 0), ((uiCsNum << 1) | (1 << 0)));
		channel.pDescriptor->srcAdd0 = (src % (1 + SDRAM_CS_SIZE)) ;
		channel.pDescriptor->destAdd = dst;
	} else {					/* Src is SRAM, Dst is DRAM CS */
		uiCsNum = (dst / (1 + SDRAM_CS_SIZE));
		MV_REG_WRITE(XOR_ADDR_OVRD_REG(0, 0), ((uiCsNum << 25) | (1 << 24)));
		channel.pDescriptor->srcAdd0 = src;
		channel.pDescriptor->destAdd = (dst % (1 + SDRAM_CS_SIZE));
	}

	channel.pDescriptor->srcAdd1 = 0;
	channel.pDescriptor->byteCnt = byteCount;
	channel.pDescriptor->nextDescPtr = 0;
	channel.pDescriptor->status = BIT31;
	channel.pDescriptor->descCommand = 0x0;
	channel.descPhyAddr = (MV_ULONG)&Virt;

	ddr3FlushL1Line((MV_U32)&Virt);

	/* issue the transfer */
	if (mvXorTransfer(chan, MV_DMA, channel.descPhyAddr) != MV_OK)
		return MV_FAIL;


	/* wait for completion */
	xor_waiton_eng(chan);

	if (dst > SRAM_BASE) {
		for (uiByte = 0; uiByte < byteCount; uiByte += 0x20)
			cache_inv(dst+uiByte);
	}
	return MV_OK;
}

/******************************************************************************
* Name:     ddr3FlushL1Line
* Desc:
* Args:
* Notes:
* Returns:  MV_OK if success, other error code if fail.
*/

static MV_VOID ddr3FlushL1Line(MV_U32 uiLine) {
	
	MV_U32 uiReg;

	uiReg = MV_REG_READ(REG_SAMPLE_RESET_LOW_ADDR) & (1<<REG_SAMPLE_RESET_CPU_ARCH_OFFS);
#ifdef MV88F67XX
	uiReg = ~uiReg & (1<<REG_SAMPLE_RESET_CPU_ARCH_OFFS);
#endif
	if (uiReg) {
		/* V7 Arch mode */
		flush_l1_v7(uiLine);
		flush_l1_v7(uiLine+CACHE_LINE_SIZE);
	} else {
		/* V6 Arch mode */
		flush_l1_v6(uiLine);
		flush_l1_v6(uiLine+CACHE_LINE_SIZE);
	}
}

