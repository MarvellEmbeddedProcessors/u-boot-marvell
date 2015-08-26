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

#include "command.h"
#include "printf.h"
#include "mvUart.h"
#include "lib_utils.h"
#include "mv_os.h"
#include "soc_spec.h"
#include "ddr3_hws_hw_training_def.h"
#include "stdint.h"
#include "gtOs/mvXor.h"
#include "gtOs/mvXorRegs.h"
#include "stdlib.h"
#include "util.h"
#include "xor_memtest.h"

extern GT_U32 mvHwsDdr3TipMaxCSGet(GT_U32 devNum);

/*******************************************************************************
* fillKillerPattern
*
* DESCRIPTION:
* Writes the killer pattern in the specified source address in DRAM
*
* INPUT:
*	nBase		- base address to start writing to
*	nSize		- size of the block in bytes
*	nbusWidth	- bus width, must be multiply of 32
*
*******************************************************************************/
void fillKillerPattern(MV_U32 nBase, MV_U32 nSize, MV_U32 nbusWidth)
{
	MV_U8 killerPatternBusState[ST_KP_LEN] = KILLER_PATTERN_BUS_STATE;
	MV_U8 killerPatternBitState[ST_KP_LEN] = KILLER_PATTERN_BIT_STATE;
	MV_U32 nOffset = 0;
	MV_U32 nNumOfWord = nbusWidth / 32;
	MV_U32 nPattern;
	MV_U32 nIndex   = 0;
	MV_U32 nVicBit  = BIT0;
	MV_U32 nVicWord = 0;
	MV_U32 nWord;

	while (nOffset < nSize) {
		if (nIndex == ST_KP_LEN) {
			nIndex = 0;
			nVicBit <<= 1;
			if (nVicBit == 0) {
				nVicBit = BIT0;
				nVicWord++;
				if (nVicWord == nNumOfWord)
					nVicWord = 0;
			}
		}
		for (nWord = 0; ((nWord < nNumOfWord) && (nOffset < nSize)); nWord++, nOffset += 0x4) {
			nPattern = BUS_STATUS(killerPatternBusState[nIndex]);
			if (nWord == nVicWord)
				nPattern = ((killerPatternBitState[nIndex]) ?
						(nPattern | nVicBit) : (nPattern & ~nVicBit));

			MV_MEMIO_LE32_WRITE(nBase + nOffset, nPattern);
		}
		nIndex++;
	}
}

/*******************************************************************************
* configureMemoryMapping
*
* DESCRIPTION:
* Opens memory windows in the XOR Engine to the DRAM and SRAM
*
*******************************************************************************/
void configureMemoryMapping()
{
	MV_U32 max_cs;
	MV_U32 uiCsEna = 0, cs_c;
#ifndef MV88F672X
	max_cs = mvHwsDdr3TipMaxCSGet(0);
#else
	max_cs = 2;
#endif
	for (cs_c = 0; cs_c < max_cs; cs_c++)
		uiCsEna |= 1 << cs_c;

	/* enable SRAM window */
	uiCsEna |= 1 << SRAM_WIN_ID;
	max_cs++;
	/* configure XOR engine windows */
	mvSysXorInit(max_cs, uiCsEna, 0x80000000, 0);

	/* shrink first DRAM(CS0) window to 0x40000000 (bytes)
	 * to avoid memory mapping conflict:
	 * First DRAM(CS0) window starts in 0x0 and ends in 0x80000000
	 * SRAM window starts in 0x40000000
	 * Hence, there is a conflict */
	MV_REG_WRITE(XOR_SIZE_MASK_REG(0, 0), 0x3FFF0000);
}

/*******************************************************************************
* XORCalculateCRC
*
* DESCRIPTION:
* Calculates crc32c using the XOR engine
*
* INPUT:
*	nChannel	- channel number (0..3)
*	nSrcAddr	- source address
*	nByteCnt	- size of the block in bytes
*
* OUTPUT:
*	res		- crc32c result
*
* RETURN:
*	MV_ERROR, in case of any error in calculation
*	MV_OK, otherwise
*
*******************************************************************************/
MV_STATUS XORCalculateCRC(MV_U32 nChannel, MV_U32 nSrcAddr, MV_U32 nByteCnt, MV_U32 *res)
{

	MV_U32 nTimeout = XOR_ENGINE_TIMEOUT;
	MV_XOR_DESC XorDesc;
	MV_XOR_DESC *pXorDesc = &XorDesc;
	MV_U32 nXorChannel = XOR_CHAN(nChannel);

	/* align pointer address to be multiple of 32 */
	pXorDesc = (MV_XOR_DESC *)(((MV_U32)((void *)pXorDesc + 32)) & ~(32 - 1));

	memset((void *)pXorDesc, 0, sizeof(MV_CRC_DMA_DESC));

	/* clean previous indication from engine */
	MV_REG_WRITE(XOR_CAUSE_REG(XOR_UNIT(nChannel)), 0);

	memset((void *)pXorDesc, 0, sizeof(MV_CRC_DMA_DESC));
	pXorDesc->srcAdd0		= nSrcAddr;
	pXorDesc->byteCnt		= nByteCnt;
	pXorDesc->descCommand		= BIT30;
	pXorDesc->phyNextDescPtr	= 0;
	pXorDesc->status		= BIT31;
	pXorDesc->crc32Result		= 0;

	/* wait for previous transfer to finnish */
	while (mvXorStateGet(nChannel) != MV_IDLE)
		;

	/* disable address override */
	MV_REG_WRITE(XOR_OVERRIDE_CTRL_REG(nChannel), 0x0);

	if (mvXorTransfer(nChannel, MV_CRC32, (MV_U32)pXorDesc) != MV_OK) {
		mvPrintf("%s: Error during XOR CRC...!\n", __func__);
		return MV_ERROR;
	}

	/* wait for the engine to finish */
	while ((MV_REG_READ(XOR_CAUSE_REG(XOR_UNIT(nChannel))) & ((BIT1|BIT0) << (16*nXorChannel))) == 0) {
		if (nTimeout-- == 0) {
			mvPrintf("%s: XOR CRC timeout!!\n", __func__);
			return MV_ERROR;
		}
	}

	nTimeout = XOR_ENGINE_TIMEOUT;
	while (mvXorStateGet(nChannel) != MV_IDLE) {
		if (nTimeout-- == 0) {
			mvPrintf("%s: XOR CRC timeout 0!!\n", __func__);
			return MV_ERROR;
		}
	}

	*res = pXorDesc->crc32Result;
	return MV_OK;
}

/*******************************************************************************
* do_xorMemTest
*
* DESCRIPTION:
* The command run memory test that is based on the XOR engine
*
*******************************************************************************/
int do_xorMemTest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_STATUS status;
	MV_U32 nBlockCRC, nOffsetInDest, nOffset, nCRC, i, nSourceAddr, nDestAddr;
	MV_U32 sourceBase = SOURCE_ADDRESS_BASE;
	MV_U32 destBase = DESTANTION_ADDRESS_BASE;
	MV_U32 destSize = DESTANTION_SIZE;
	MV_U32 iterations = 1;

	if (argc > 1)
		nSourceAddr = simple_strtoul(argv[1], NULL, 16);

	if (argc > 2)
		nDestAddr = simple_strtoul(argv[2], NULL, 16);

	if (argc > 3)
		destSize = simple_strtoul(argv[3], NULL, 16);

	if (argc > 4)
		iterations = simple_strtoul(argv[4], NULL, 16);

	/* check destSize, must be multiply of 1MB */
	if (destSize & (_1M - 1)) {
		mvPrintf("Error: Destination size must be multiply of 0x%X(1MB)\n", _1M);
		mvSysXorFinish();
		return -1;
	}

	configureMemoryMapping();

	/* write the killer pattern in the source block */
	fillKillerPattern(sourceBase, BLOCK_SIZE, 64);
	fillKillerPattern(sourceBase + BLOCK_SIZE, BLOCK_SIZE, 128);

	status = XORCalculateCRC(2, sourceBase, TOTAL_BLOCK_SIZE, &nBlockCRC);
	CHECK_CRC_STATUS(status);
	mvPrintf("nBlockCRC %x\n", nBlockCRC);

	/* Start copying blocks */
	for (i = 0; i < iterations; i++) {
		for (nOffsetInDest = 0; nOffsetInDest < destSize; nOffsetInDest += TOTAL_BLOCK_SIZE) {

			/* Copy block to the current destination address*/
			for (nOffset = 0; nOffset < TOTAL_BLOCK_SIZE; nOffset += CPU_BURST_SIZE) {
				nSourceAddr = sourceBase + nOffset;
				nDestAddr = destBase + nOffsetInDest + nOffset;
				MV_PLD_RANGE(nDestAddr, nDestAddr + CPU_BURST_SIZE);
				memcpy((void *)nDestAddr, (void *)nSourceAddr, CPU_BURST_SIZE);
				MV_FLUSH_CACHE_L1(nDestAddr, nDestAddr + CPU_BURST_SIZE);
			}
		}

		/* Check the copied blocks in destination */
		for (nOffsetInDest = 0; nOffsetInDest < destSize; nOffsetInDest += TOTAL_BLOCK_SIZE) {
			nDestAddr = destBase + nOffsetInDest;
			status = XORCalculateCRC(2, nDestAddr, TOTAL_BLOCK_SIZE, &nCRC);
			CHECK_CRC_STATUS(status);
			if (nCRC != nBlockCRC) {
				mvPrintf("Error at Block start address %X\n", nDestAddr);
				mvPrintf("CRC was %X expected %X\n", nCRC, nBlockCRC);
				mvSysXorFinish();
				return -1;
			}
		}
		mvPrintf("\b\b\b\b%4d", i);
	}
	mvPrintf("\b\b\b\b");
	mvPrintf("MemTest end successfully\n");
	mvSysXorFinish();

	return 0;
}
