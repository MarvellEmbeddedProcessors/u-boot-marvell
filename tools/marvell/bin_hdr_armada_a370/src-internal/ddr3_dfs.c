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

#include "ddr3_hw_training.h"

/*
 * Debug
*/
#define DEBUG_DFS_C(s, d, l)     	DEBUG_DFS_S(s); DEBUG_DFS_D(d, l); DEBUG_DFS_S("\n")
#define DEBUG_DFS_FULL_C(s, d, l)	DEBUG_DFS_FULL_S(s); DEBUG_DFS_FULL_D(d, l); DEBUG_DFS_FULL_S("\n")

#ifdef	MV_DEBUG_DFS
#define DEBUG_DFS_S(s)				putstring(s)
#define DEBUG_DFS_D(d, l)			putdata(d, l)

#else
#define DEBUG_DFS_S(s)
#define DEBUG_DFS_D(d, l)
#endif

#ifdef	MV_DEBUG_DFS_FULL
#define DEBUG_DFS_FULL_S(s)			putstring(s)
#define DEBUG_DFS_FULL_D(d, l)		putdata(d, l)
#else
#define DEBUG_DFS_FULL_S(s)
#define DEBUG_DFS_FULL_D(d, l)
#endif

extern MV_U16 auiODTStatic[ODT_OPT][MAX_CS];
extern MV_U16 auiODTDynamic[ODT_OPT][MAX_CS];

extern MV_U8 s_auiDivRatio1to1[CLK_CPU][CLK_DDR];
extern MV_U8 s_auiDivRatio2to1[CLK_CPU][CLK_DDR];

MV_U32	ddr3GetFreqParameter(MV_U32 uiTargetFreq, MV_BOOL b2to1ratio);

/******************************************************************************
* Name:     ddr3GetFreqParameter.
* Desc:     Finds CPU/DDR uiFrequency ratio according to Sample@reset and table.
* Args:     uiTargetFreq - target uiFrequency
* Notes:
* Returns:  uiFreqPar - the ratio parameter
*/

MV_U32 ddr3GetFreqParameter(MV_U32 uiTargetFreq, MV_BOOL b2to1ratio)
{
	MV_U32 uiVCOFreq, uiFreqPar;

	uiVCOFreq = ddr3GetVCOFreq();

	/* Find the ratio between PLL uiFrequency and ddr-clk.  */
	if (b2to1ratio)
		uiFreqPar = s_auiDivRatio2to1[uiVCOFreq][uiTargetFreq];
	else
		uiFreqPar = s_auiDivRatio1to1[uiVCOFreq][uiTargetFreq];

	return uiFreqPar;
}


/******************************************************************************
* Name:     ddr3DfsHigh2Low
* Desc:		
* Args:     uiFreq - target uiFrequency
* Notes:
* Returns:  MV_OK - success, MV_FAIL - fail
*/

MV_STATUS ddr3DfsHigh2Low(MV_U32 uiFreq, MV_DRAM_INFO *pDramInfo)
{
	if (pDramInfo->bIsA0) {
#ifdef MV88F78X60
		/* This Flow is relevant for ArmadaXP A0 */
		MV_U32 uiReg, uiFreqPar, uiTemp;
		MV_U32 uiCs = 0;

		DEBUG_DFS_C("DDR3 - DFS - High To Low - Starting DFS procedure to Frequency - ", uiFreq, 1);

		uiFreqPar = ddr3GetFreqParameter(uiFreq, FALSE);		/* target uiFrequency - 100MHz */
	
		/* Configure - DRAM DLL final state after DFS is complete - Enable */
		uiReg = MV_REG_READ(REG_DFS_ADDR);
		uiReg |= (1 << REG_DFS_DLLNEXTSTATE_OFFS);				/* [0] - DfsDllNextState - Disable */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */

		/* Configure -  XBAR Retry response during Block to enable internal access - Disable */
		uiReg = MV_REG_READ(REG_METAL_MASK_ADDR);
		uiReg &= ~(1 << REG_METAL_MASK_RETRY_OFFS);				/* [0] - RetryMask - Disable */
		MV_REG_WRITE(REG_METAL_MASK_ADDR, uiReg);  				/* 0x14B0 - Dunit MMask Register */

		/* Configure - Block new external transactions - Enable*/
		uiReg = MV_REG_READ(REG_DFS_ADDR);
		uiReg |= (1 << REG_DFS_BLOCK_OFFS);						/* [1] - DfsBlock - Enable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */

		/* Registered DIMM support */
		if (pDramInfo->bRegDimm) {
			/* Configure - Disable Register DIMM CKE Power Down mode - CWA_RC */
			uiReg = ((0x9 & REG_SDRAM_OPERATION_CWA_RC_MASK) << REG_SDRAM_OPERATION_CWA_RC_OFFS);
			/* Configure - Disable Register DIMM CKE Power Down mode - CWA_DATA */
			uiReg |= ((0 & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
			/* Configure - Disable Register DIMM CKE Power Down mode - Set Delay - tMRD */
			uiReg |= (0 << REG_SDRAM_OPERATION_CWA_DELAY_SEL_OFFS);
			/* Configure - Issue CWA command with the above parameters */
			uiReg |= (REG_SDRAM_OPERATION_CMD_CWA & ~(pDramInfo->uiCsEna << REG_SDRAM_OPERATION_CS_OFFS));
			
			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */

			/* Poll - Wait for CWA operation completion */
			do {
				uiReg = (MV_REG_READ(REG_SDRAM_OPERATION_ADDR) & (REG_SDRAM_OPERATION_CMD_MASK));
			} while (uiReg);
			
			/* Configure - Disable outputs floating during Self Refresh */
			uiReg = MV_REG_READ(REG_REGISTERED_DRAM_CTRL_ADDR);
			uiReg &= ~(1 << REG_REGISTERED_DRAM_CTRL_SR_FLOAT_OFFS);	/* [15] - SRFloatEn - Disable */
			MV_REG_WRITE(REG_REGISTERED_DRAM_CTRL_ADDR, uiReg);			/* 0x16D0 - DDR3 Registered DRAM Control */
		}

		/* Optional - Configure - DDR3_Rtt_nom_CS# */
		for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
			if (pDramInfo->uiCsEna & (1<<uiCs)) {
				uiReg = MV_REG_READ(REG_DDR3_MR1_CS_ADDR + (uiCs << MR_CS_ADDR_OFFS));
				uiReg &= REG_DDR3_MR1_RTT_MASK;
				MV_REG_WRITE(REG_DDR3_MR1_CS_ADDR + (uiCs << MR_CS_ADDR_OFFS), uiReg);
			}
		}
	
		/* Configure - Move DRAM into Self Refresh */
		uiReg = MV_REG_READ(REG_DFS_ADDR);
		uiReg |= (1 << REG_DFS_SR_OFFS);						/* [2] - DfsSR - Enable */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */
	
		/* Poll - Wait for Self Refresh indication */
		do {
			uiReg = ((MV_REG_READ(REG_DFS_ADDR)) & (1 << REG_DFS_ATSR_OFFS));
		} while (uiReg == 0x0);									/* 0x1528 [3] - DfsAtSR - Wait for '1' */
	
		/* Start of clock change procedure (PLL) */
	
		uiReg = 0x0000FDFF;	 /* Initial Setup - assure that teh "load new ratio" is clear (bit 24)
		and in the same chance, block reassertions of reset [15:8] and force reserved bits[7:0]. */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);		/* 0x18700 - CPU Div CLK control 0 */

		uiReg = 0x0000FF00;	/* RelaX whenever reset is asserted to that channel (good for any case) */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_1_ADDR, uiReg);		/* 0x18704 - CPU Div CLK control 0 */

		uiReg = (MV_REG_READ(REG_CPU_DIV_CLK_CTRL_2_ADDR) & REG_CPU_DIV_CLK_CTRL_3_FREQ_MASK);

		uiReg |= (uiFreqPar << REG_CPU_DIV_CLK_CTRL_3_FREQ_OFFS); /* full Integer ratio from PLL-out to ddr-clk */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_2_ADDR, uiReg);			/* 0x1870C - CPU Div CLK control 3 uiRegister*/

		/* Shut off clock enable to the DDRPHY clock channel (this is the "D"). All the rest
		are kept as is (forced, but could be read-modify-write).
		This is done now by RMW above. */

		uiReg = 0x000FFF02; /* Clock is not shut off gracefully - keep it running */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_4_ADDR, uiReg);

		uDelay(1); /* wait before replacing the clock on the DDR Phy Channel. */

		uiReg = 0x0102FDFF; /* This for triggering the frequency update. Bit[24] is the central control
		bits [23:16] == which channels to change ==2 ==> only DDR Phy (smooth transition)
		bits [15:8] == mask reset reassertion due to clock modification to these channels.
		bits [7:0] == not in use */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);		/* 0x18700 - CPU Div CLK control 0 uiRegister*/

		uDelay(1);	/* Wait 1uSec */

		/* Poll Div CLK status 0 uiRegister - indication that the clocks are active - 0x18718 [8] */
		do {
			uiReg = ((MV_REG_READ(REG_CPU_DIV_CLK_STATUS_0_ADDR)) & (1 << REG_CPU_DIV_CLK_ALL_STABLE_OFFS));
		} while (uiReg == 0);

		uiReg = 0x000000FF; /* Clean the CTRL0, to be ready for next resets and next requests of ratio modifications. */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);		/* 0x18700 - CPU Div CLK control 0 uiRegister*/

		uDelay(5);

		/* End of clock change procedure (PLL) */
	
		/* Configure - Select normal clock for the DDR PHY - Enable */
		uiReg = MV_REG_READ(REG_DRAM_INIT_CTRL_STATUS_ADDR);
		uiReg |= (1 << REG_DRAM_INIT_CTRL_TRN_CLK_OFFS);				/* [16] - ddr_phy_trn_clk_sel - Enable  */
		MV_REG_WRITE(REG_DRAM_INIT_CTRL_STATUS_ADDR, uiReg);			/* 0x18488 - DRAM Init control status uiRegister */
	
		/* Configure - Set Correct Ratio - 1:1 */
		/* [15] - Phy2UnitClkRatio = 0 - Set 1:1 Ratio between Dunit and Phy */
		uiReg = (MV_REG_READ(REG_DDR_IO_ADDR) & ~(1 << REG_DDR_IO_CLK_RATIO_OFFS));
		MV_REG_WRITE(REG_DDR_IO_ADDR, uiReg);				/* 0x1524 - DDR IO Register */
	
		/* Configure - 2T Mode - Restore original configuration */
		uiReg = MV_REG_READ(REG_DUNIT_CTRL_LOW_ADDR);
		uiReg &= ~(REG_DUNIT_CTRL_LOW_2T_MASK << REG_DUNIT_CTRL_LOW_2T_OFFS);	/* [3:4] 2T - 1T Mode - low freq */
		MV_REG_WRITE(REG_DUNIT_CTRL_LOW_ADDR, uiReg);			/* 0x1404 - DDR Controller Control Low Register */
	
		/* Configure - Restore CL and CWL - MRS Commands */
		uiReg = MV_REG_READ(REG_DFS_ADDR);
		uiReg &= ~(REG_DFS_CL_NEXT_STATE_MASK << REG_DFS_CL_NEXT_STATE_OFFS);
		uiReg &= ~(REG_DFS_CWL_NEXT_STATE_MASK << REG_DFS_CWL_NEXT_STATE_OFFS);
		uiReg |= (0x4 << REG_DFS_CL_NEXT_STATE_OFFS);			/* [8] - DfsCLNextState - MRS CL=6 after DFS (due to DLL-off mode) */
		uiReg |= (0x1 << REG_DFS_CWL_NEXT_STATE_OFFS);			/* [12] - DfsCWLNextState - MRS CWL=6 after DFS (due to DLL-off mode) */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */
	
		/* Poll - Wait for APLL + ADLLs lock on new frequency */
		do {
			uiReg = ((MV_REG_READ(REG_PHY_LOCK_STATUS_ADDR)) & REG_PHY_LOCK_APLL_ADLL_STATUS_MASK);
		} while (uiReg != REG_PHY_LOCK_APLL_ADLL_STATUS_MASK); /* 0x1674 [10:0] - Phy lock status Register */
	
		/* Configure - Reset the PHY Read FIFO and Write channels - Set Reset */
		uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & REG_SDRAM_CONFIG_MASK);
		/* [30:29] = 0 - Data Pup R/W path reset */
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);			/* 0x1400 - SDRAM Configuration uiRegister */

		/* Configure - DRAM Data PHY Read [30], Write [29] path reset - Release Reset */
		uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) | ~REG_SDRAM_CONFIG_MASK);
		/* [30:29] = '11' - Data Pup R/W path reset */
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);			/* 0x1400 - SDRAM Configuration uiRegister */

		/* Registered DIMM support */
		if (pDramInfo->bRegDimm) {

			/* Configure - Change register DRAM frequency band (band 2) - CWA_RC */
			uiReg = ((0x2 & REG_SDRAM_OPERATION_CWA_RC_MASK) << REG_SDRAM_OPERATION_CWA_RC_OFFS);
			/* Configure - Change register DRAM frequency band (band 2) - CWA_DATA */
			uiReg |= ((0x8 & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
			/* Configure - Set Delay - tMRD */
			uiReg |= (0 << REG_SDRAM_OPERATION_CWA_DELAY_SEL_OFFS);
			/* Configure - Issue CWA command with the above parameters */
			uiReg |= (REG_SDRAM_OPERATION_CMD_CWA & ~(pDramInfo->uiCsEna << REG_SDRAM_OPERATION_CS_OFFS));
			
			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */

			/* Poll - Wait for CWA operation completion */
			do {
				uiReg = (MV_REG_READ(REG_SDRAM_OPERATION_ADDR) & (REG_SDRAM_OPERATION_CMD_MASK));
			} while (uiReg);
	
			/* Configure - Change register DRAM operating speed (below 400MHz) - CWA_RC */
			uiReg = ((0xA & REG_SDRAM_OPERATION_CWA_RC_MASK) << REG_SDRAM_OPERATION_CWA_RC_OFFS);
			/* Configure - Change register DRAM operating speed (below 400MHz) - CWA_DATA */
			uiReg |= ((0x0 & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
			/* Configure - Set Delay - tSTAB */
			uiReg |= (0x1 << REG_SDRAM_OPERATION_CWA_DELAY_SEL_OFFS);
			/* Configure - Issue CWA command with the above parameters */
			uiReg |= (REG_SDRAM_OPERATION_CMD_CWA & ~(pDramInfo->uiCsEna << REG_SDRAM_OPERATION_CS_OFFS));
			
			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */

			/* Poll - Wait for CWA operation completion */
			do {
				uiReg = (MV_REG_READ(REG_SDRAM_OPERATION_ADDR) & (REG_SDRAM_OPERATION_CMD_MASK));
			} while (uiReg);
		}

		/* Configure - Exit Self Refresh */
		uiReg = (MV_REG_READ(REG_DFS_ADDR) & ~(1 << REG_DFS_SR_OFFS));	/* [2] - DfsSR  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);				/* 0x1528 - DFS uiRegister */

		/* Poll - DFS Register - 0x1528 [3] - DfsAtSR - All DRAM devices on all ranks are NOT in self refresh mode */
		do {
			uiReg = ((MV_REG_READ(REG_DFS_ADDR)) & (1 << REG_DFS_ATSR_OFFS));
		} while (uiReg);		/* Wait for '0' */
	
		/* Configure - Issue Refresh command */
		/* [3-0] = 0x2 - Refresh Command, [11-8] - enabled Cs */
		uiReg = REG_SDRAM_OPERATION_CMD_RFRS;
		for (uiCs = 0; uiCs < MAX_CS; uiCs++)
			if (pDramInfo->uiCsEna & (1 << uiCs))
				uiReg &= ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs));
	
		MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
	
		/* Poll - Wait for Refresh operation completion */
		do {
			uiReg = ((MV_REG_READ(REG_SDRAM_OPERATION_ADDR)) & REG_SDRAM_OPERATION_CMD_RFRS_DONE);
		} while (uiReg);				/* Wait for '0' */

		/* Configure - Block new external transactions - Disable*/
		uiReg = MV_REG_READ(REG_DFS_ADDR);
		uiReg &= ~(1 << REG_DFS_BLOCK_OFFS);					/* [1] - DfsBlock - Disable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */

		/* Configure -  XBAR Retry response during Block to enable internal access - Disable */
		uiReg = MV_REG_READ(REG_METAL_MASK_ADDR);
		uiReg |= (1 << REG_METAL_MASK_RETRY_OFFS);				/* [0] - RetryMask - Enable */
		MV_REG_WRITE(REG_METAL_MASK_ADDR, uiReg);  				/* 0x14B0 - Dunit MMask Register */
	
		/* Configure - Set CL */
		uiReg = MV_REG_READ(REG_DDR3_MR0_ADDR) & ~REG_DDR3_MR0_CL_MASK;
		uiTemp = 0x4;								/* CL=6 - 0x4 */
		uiReg |= ((uiTemp & 0x1) << REG_DDR3_MR0_CL_OFFS);
		uiReg |= ((uiTemp & 0xE) << REG_DDR3_MR0_CL_HIGH_OFFS);
		MV_REG_WRITE(REG_DDR3_MR0_ADDR, uiReg);		/* 0x15D0 - DDR3 MR0 Register */
	
		/* Configure - Set CWL */
		uiReg = MV_REG_READ(REG_DDR3_MR2_ADDR) & ~(REG_DDR3_MR2_CWL_MASK << REG_DDR3_MR2_CWL_OFFS);
		uiReg |= ((0x1) << REG_DDR3_MR2_CWL_OFFS);  /* CWL=6 - 0x1 */
		
		MV_REG_WRITE(REG_DDR3_MR2_ADDR, uiReg);		/* 0x15D8 - DDR3 MR2 Register */
	
		DEBUG_DFS_C("DDR3 - DFS - High To Low - Ended successfuly - new Frequency - ", uiFreq, 1);
#endif
		return MV_OK;
	} else {
		/* This Flow is relevant for Armada370 A0 and ArmadaXP Z1 */
		
		MV_U32 uiReg, uiFreqPar;
		MV_U32 uiCs = 0;
	
		DEBUG_DFS_C("DDR3 - DFS - High To Low - Starting DFS procedure to Frequency - ", uiFreq, 1);
	
		uiFreqPar = ddr3GetFreqParameter(uiFreq, FALSE);		/* target uiFrequency - 100MHz */
	
		uiReg = 0x0000FF00;
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_1_ADDR, uiReg);		/* 0x18700 - CPU Div CLK control 0 */
	
		uiReg = MV_REG_READ(REG_ODPG_CNTRL_ADDR);			/* 0x1600 - ODPG_CNTRL_Control */
		uiReg |= (1<<REG_ODPG_CNTRL_OFFS);				/* [21] = 1 - auto refresh disable */
		MV_REG_WRITE(REG_ODPG_CNTRL_ADDR, uiReg);
	
		uiReg = MV_REG_READ(REG_PHY_LOCK_MASK_ADDR);	/* 0x1670 - PHY lock mask uiRegister */
		uiReg &= REG_PHY_LOCK_MASK_MASK;				/* [11:0] = 0 */
		MV_REG_WRITE(REG_PHY_LOCK_MASK_ADDR, uiReg);
	
		uiReg = MV_REG_READ(REG_DFS_ADDR);			/* 0x1528 - DFS uiRegister */
	
		/*	disable reconfig */
		uiReg &= ~0x10;								/* [4] - Enable reconfig MR uiRegisters after DFS_ERG */
		uiReg |= 0x1;								/* [0] - DRAM DLL disabled after DFS */
	
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);			/* 0x1528 - DFS uiRegister */
	
		uiReg = (MV_REG_READ(REG_METAL_MASK_ADDR) & ~(1 << 0));   	/* [0] - disable */
		MV_REG_WRITE(REG_METAL_MASK_ADDR, uiReg);  					/* 0x14B0 - Dunit MMask Register */
	
		uiReg = (MV_REG_READ(REG_DFS_ADDR) | (1<<REG_DFS_BLOCK_OFFS));	/*  [1] - DFS Block enable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);							/* 0x1528 - DFS uiRegister */
	
		uiReg = (MV_REG_READ(REG_DFS_ADDR) | (1<<REG_DFS_SR_OFFS));	/*  [2] - DFS Self refresh enable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);							/* 0x1528 - DFS uiRegister */
	
		/* Poll DFS Register - 0x1528 [3] - DfsAtSR -
		All DRAM devices on all ranks are in self refresh mode - DFS can be executed afterwards */
		do {
			uiReg = ((MV_REG_READ(REG_DFS_ADDR)) & (1 << REG_DFS_ATSR_OFFS));
		} while (uiReg == 0x0);						/* Wait for '1' */
	
	
		/* disable ODT on DLL-off mode */
		MV_REG_WRITE(REG_SDRAM_ODT_CTRL_HIGH_ADDR, REG_SDRAM_ODT_CTRL_HIGH_OVRD_MASK);
	
		uiReg = (MV_REG_READ(REG_PHY_LOCK_MASK_ADDR) & REG_PHY_LOCK_MASK_MASK); /* [11:0] = 0 */
		MV_REG_WRITE(REG_PHY_LOCK_MASK_ADDR, uiReg);	/* 0x1670 - PHY lock mask uiRegister */
	
		uDelay(1);	/* Add delay between entering SR and start ratio modification */
	
		uiReg = 0x0000FDFF;	 /* Initial Setup - assure that teh "load new ratio" is clear (bit 24)
		and in the same chance, block reassertions of reset [15:8] and force reserved bits[7:0]. */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);		/* 0x18700 - CPU Div CLK control 0 */
	
		uiReg = 0x0000FF00;	/* RelaX whenever reset is asserted to that channel (good for any case) */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_1_ADDR, uiReg);		/* 0x18700 - CPU Div CLK control 0 */
	
		uiReg = (MV_REG_READ(REG_CPU_DIV_CLK_CTRL_3_ADDR) & REG_CPU_DIV_CLK_CTRL_3_FREQ_MASK);
	
		uiReg |= (uiFreqPar << REG_CPU_DIV_CLK_CTRL_3_FREQ_OFFS); /* full Integer ratio from PLL-out to ddr-clk */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_3_ADDR, uiReg);			/* 0x1870C - CPU Div CLK control 3 uiRegister*/
	
		/* Shut off clock enable to the DDRPHY clock channel (this is the "D"). All the rest
		are kept as is (forced, but could be read-modify-write).
		This is done now by RMW above. */
	
		uiReg = 0x000FFF02; /* Clock is not shut off gracefully - keep it running */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_4_ADDR, uiReg);
	
		uDelay(1); /* wait before replacing the clock on the DDR Phy Channel. */
	
		uiReg = 0x0102FDFF; /* This for triggering the frequency update. Bit[24] is the central control
		bits [23:16] == which channels to change ==2 ==> only DDR Phy (smooth transition)
		bits [15:8] == mask reset reassertion due to clock modification to these channels.
		bits [7:0] == not in use */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);		/* 0x18700 - CPU Div CLK control 0 uiRegister*/
	
		uDelay(1);	/* Wait 1uSec */
	
		/* Poll Div CLK status 0 uiRegister - indication that the clocks are active - 0x18718 [8] */
		do {
			uiReg = ((MV_REG_READ(REG_CPU_DIV_CLK_STATUS_0_ADDR)) & (1 << REG_CPU_DIV_CLK_ALL_STABLE_OFFS));
		} while (uiReg == 0);
	
		uiReg = 0x000000FF; /* Clean the CTRL0, to be ready for next resets and next requests of ratio modifications. */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);		/* 0x18700 - CPU Div CLK control 0 uiRegister*/
	
		uDelay(5);
	
		/* Switch HCLK Mux to training clk (100Mhz), keep DFS request bit */
		uiReg = 0x20050000;
		MV_REG_WRITE(REG_DRAM_INIT_CTRL_STATUS_ADDR, uiReg);    /* 0x18488 - DRAM Init control status uiRegister */
	
		uiReg = (MV_REG_READ(REG_DDR_IO_ADDR) & ~(1 << REG_DDR_IO_CLK_RATIO_OFFS));
		/* [15] = 0 - Set 1:1 Ratio between Dunit and Phy */
		MV_REG_WRITE(REG_DDR_IO_ADDR, uiReg);		/* 0x1524 - DDR IO Regist */

		uiReg = (MV_REG_READ(REG_DRAM_PHY_CONFIG_ADDR) & REG_DRAM_PHY_CONFIG_MASK);
		/* [31:30]] - reset pup data ctrl ADLL */
		MV_REG_WRITE(REG_DRAM_PHY_CONFIG_ADDR, uiReg);	/* 0x15EC - DRAM PHY Config uiRegister */
	
		uiReg = (MV_REG_READ(REG_DRAM_PHY_CONFIG_ADDR) | ~REG_DRAM_PHY_CONFIG_MASK);
		/* [31:30] - normal pup data ctrl ADLL */
		MV_REG_WRITE(REG_DRAM_PHY_CONFIG_ADDR, uiReg);	/* 0x15EC - DRAM PHY Config uiRegister */

		uDelay(1);	/* Wait 1uSec */
	
		/* 0x1404 */
		uiReg = (MV_REG_READ(REG_DUNIT_CTRL_LOW_ADDR) & 0xFFFFFFE7);
		MV_REG_WRITE(REG_DUNIT_CTRL_LOW_ADDR, uiReg);
	
		/* Poll Phy lock status uiRegister - APLL lock indication - 0x1674  */
		do {
			uiReg = ((MV_REG_READ(REG_PHY_LOCK_STATUS_ADDR)) & REG_PHY_LOCK_STATUS_LOCK_MASK);
		} while (uiReg != REG_PHY_LOCK_STATUS_LOCK_MASK);		/* Wait for '0xFFF' */
	
		uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & REG_SDRAM_CONFIG_MASK);
		/* [30:29] = 0 - Data Pup R/W path reset */
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);			/* 0x1400 - SDRAM Configuration uiRegister */
	
		uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) | ~REG_SDRAM_CONFIG_MASK);
		/* [30:29] = '11' - Data Pup R/W path reset */
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);			/* 0x1400 - SDRAM Configuration uiRegister */
	
		uDelay(1000);	/* Wait 1mSec */
	
		for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
			if (pDramInfo->uiCsEna & (1<<uiCs)) {
				uiReg = 0x00000047;
				uiReg &= REG_DDR3_MR1_ODT_MASK;
				uiReg |= auiODTStatic[pDramInfo->uiCsEna][uiCs];
	
				MV_REG_WRITE(REG_DDR3_MR1_ADDR, uiReg);
	
				/* Issue MRS Command to current uiCs */
				uiReg = (REG_SDRAM_OPERATION_CMD_MR1 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
				/* [3-0] = 0x4 - MR1 Command, [11-8] - enable current uiCs */
				MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
	
				uDelay(MRS_DELAY);
	
				uiReg = 0x00000610;
	
				MV_REG_WRITE(REG_DDR3_MR0_ADDR, uiReg);
	
				/* Issue MRS Command to current uiCs */
				uiReg = (REG_SDRAM_OPERATION_CMD_MR0 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
				/* [3-0] = 0x4 - MR1 Command, [11-8] - enable current uiCs */
				MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
	
				uDelay(MRS_DELAY);
	
				/* Configure MR2 in uiCs[uiCs] */
				uiReg = 0x00010208;
				uiReg &= REG_DDR3_MR2_ODT_MASK;
				uiReg |= auiODTDynamic[pDramInfo->uiCsEna][uiCs];
	
				MV_REG_WRITE(REG_DDR3_MR2_ADDR, uiReg);
	
				/* Issue MRS Command to current uiCs */
				uiReg = (REG_SDRAM_OPERATION_CMD_MR2 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
				/* [3-0] = 0x4 - MR2 Command, [11-8] - enable current uiCs */
				MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
	
				uDelay(MRS_DELAY);
	
				/* Set current uiRdSampleDelay  */
				uiReg = MV_REG_READ(REG_READ_DATA_SAMPLE_DELAYS_ADDR);
				uiReg &= ~(REG_READ_DATA_SAMPLE_DELAYS_MASK << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
				uiReg |= (5 << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
				MV_REG_WRITE(REG_READ_DATA_SAMPLE_DELAYS_ADDR, uiReg);
	
				/* Set current uiRdReadyDelay  */
				uiReg = MV_REG_READ(REG_READ_DATA_READY_DELAYS_ADDR);
				uiReg &= ~(REG_READ_DATA_READY_DELAYS_MASK << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
				uiReg |= ((6) << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
				MV_REG_WRITE(REG_READ_DATA_READY_DELAYS_ADDR, uiReg);
			}
		}
	
		uiReg = (MV_REG_READ(REG_DFS_ADDR) & ~(1<<REG_DFS_SR_OFFS));	/*  [2] - DFS Self refresh disable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);								/* 0x1528 - DFS uiRegister */
	
		uiReg = (MV_REG_READ(REG_DFS_ADDR) & ~(1<<REG_DFS_BLOCK_OFFS));	/*  [1] - DFS Block enable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);								/* 0x1528 - DFS uiRegister */
	
		/* Poll DFS Register - 0x1528 [3] - DfsAtSR -
		All DRAM devices on all ranks are in self refresh mode - DFS can be executed afterwards */
		do {
			uiReg = ((MV_REG_READ(REG_DFS_ADDR)) & (1 << REG_DFS_ATSR_OFFS));
		} while (uiReg);		/* Wait for '1' */
	
		uiReg = (MV_REG_READ(REG_METAL_MASK_ADDR) | (1 << 0));
		/* [0] - Enable Dunit to crossbar retry */
		MV_REG_WRITE(REG_METAL_MASK_ADDR, uiReg);  		/* 0x14B0 - Dunit MMask Register */
	
		uiReg = MV_REG_READ(REG_ODPG_CNTRL_ADDR);		/* 0x1600 - PHY lock mask uiRegister */
		uiReg &= ~(1<<REG_ODPG_CNTRL_OFFS);				/* [21] = 0 */
		MV_REG_WRITE(REG_ODPG_CNTRL_ADDR, uiReg);
	
		uiReg = MV_REG_READ(REG_PHY_LOCK_MASK_ADDR);	/* 0x1670 - PHY lock mask uiRegister */
		uiReg |= ~REG_PHY_LOCK_MASK_MASK;				/* [11:0] = FFF */
		MV_REG_WRITE(REG_PHY_LOCK_MASK_ADDR, uiReg);
	
		DEBUG_DFS_C("DDR3 - DFS - High To Low - Ended successfuly - new Frequency - ", uiFreq, 1);
		return MV_OK;
	
	}
}

/******************************************************************************
* Name:     ddr3DfsLow2High.
* Desc:
* Args:		uiFreq - target uiFrequency
* Notes:
* Returns:  MV_OK - success, MV_FAIL - fail
*/

MV_STATUS ddr3DfsLow2High(MV_U32 uiFreq, MV_BOOL b2to1ratio, MV_DRAM_INFO *pDramInfo)
{
	if (pDramInfo->bIsA0) {
#ifdef MV88F78X60
		/* This Flow is relevant for ArmadaXP A0 */
		MV_U32 uiReg, uiFreqPar, uiTemp;
		MV_U32 uiCs = 0;

		DEBUG_DFS_C("DDR3 - DFS - Low To High - Starting DFS procedure to Frequency - ", uiFreq, 1);

		uiFreqPar = ddr3GetFreqParameter(uiFreq, b2to1ratio);		/* target uiFrequency - uiFreq */

		/* Configure - DRAM DLL final state after DFS is complete - Enable */
		uiReg = MV_REG_READ(REG_DFS_ADDR);
		uiReg &= ~(1 << REG_DFS_DLLNEXTSTATE_OFFS);				/* [0] - DfsDllNextState - Enable */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */

		/* Configure -  XBAR Retry response during Block to enable internal access - Disable */
		uiReg = MV_REG_READ(REG_METAL_MASK_ADDR);
		uiReg &= ~(1 << REG_METAL_MASK_RETRY_OFFS);				/* [0] - RetryMask - Disable */
		MV_REG_WRITE(REG_METAL_MASK_ADDR, uiReg);  				/* 0x14B0 - Dunit MMask Register */

		/* Configure - Block new external transactions - Enable*/
		uiReg = MV_REG_READ(REG_DFS_ADDR);
		uiReg |= (1 << REG_DFS_BLOCK_OFFS);						/* [1] - DfsBlock - Enable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */
	
		/* Configure - Move DRAM into Self Refresh */
		uiReg = MV_REG_READ(REG_DFS_ADDR);
		uiReg |= (1 << REG_DFS_SR_OFFS);						/* [2] - DfsSR - Enable */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */
	
		/* Poll - Wait for Self Refresh indication */
		do {
			uiReg = ((MV_REG_READ(REG_DFS_ADDR)) & (1<<REG_DFS_ATSR_OFFS));
		} while (uiReg == 0x0);									/* 0x1528 [3] - DfsAtSR - Wait for '1' */
	
		/* Start of clock change procedure (PLL) */
	
		uiReg = 0x0000FFFF;	/*  Initial Setup - assure that teh "load new ratio" is clear (bit 24)
		and in the same chance, block reassertions of reset [15:8] and force reserved bits[7:0]. */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);		/* 0x18700 - CPU Div CLK control 0 */

		uiReg = 0x0000FF00;	/* RelaX whenever reset is asserted to that channel (good for any case) */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_1_ADDR, uiReg);		/* 0x18704 - CPU Div CLK control 0 */

		uiReg = (MV_REG_READ(REG_CPU_DIV_CLK_CTRL_2_ADDR) & REG_CPU_DIV_CLK_CTRL_3_FREQ_MASK);
		uiReg |= (uiFreqPar << REG_CPU_DIV_CLK_CTRL_3_FREQ_OFFS);
		/* full Integer ratio from PLL-out to ddr-clk */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_2_ADDR, uiReg);	/* 0x1870C - CPU Div CLK control 3 uiRegister*/

	/* 	Shut off clock enable to the DDRPHY clock channel (this is the "D"). All the rest
		are kept as is (forced, but could be read-modify-write).
		This is done now by RMW above.  */
		uiReg = 0x000FFF02;
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_4_ADDR, uiReg);

		uDelay(1); /* wait before replacing the clock on the DDR Phy Channel. */

		uiReg = 0x0102FDFF;
	/* 	This for triggering the frequency update. Bit[24] is the central control
		bits [23:16] == which channels to change ==2 ==> only DDR Phy (smooth transition)
		bits [15:8] == mask reset reassertion due to clock modification to these channels.
		bits [7:0] == not in use */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);	/* 0x18700 - CPU Div CLK control 0 uiRegister*/

		uDelay(1);
	
		/* Poll Div CLK status 0 uiRegister - indication that the clocks are active - 0x18718 [8] */
		do {
			uiReg = ((MV_REG_READ(REG_CPU_DIV_CLK_STATUS_0_ADDR)) & (1 << REG_CPU_DIV_CLK_ALL_STABLE_OFFS));
		} while (uiReg == 0);

		uiReg = 0x000000FF;
		/* Clean the CTRL0, to be ready for next resets and next requests of ratio modifications. */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);	/* 0x18700 - CPU Div CLK control 0 uiRegister*/
	
		/* End of clock change procedure (PLL) */
	
		/* Configure - Select normal clock for the DDR PHY - Disable */
		uiReg = MV_REG_READ(REG_DRAM_INIT_CTRL_STATUS_ADDR);
		uiReg &= ~(1 << REG_DRAM_INIT_CTRL_TRN_CLK_OFFS);				/* [16] - ddr_phy_trn_clk_sel - Disable  */
		MV_REG_WRITE(REG_DRAM_INIT_CTRL_STATUS_ADDR, uiReg);			/* 0x18488 - DRAM Init control status uiRegister */
	
		/* Configure - Set Correct Ratio - according to target ratio parameter - 2:1/1:1 */
		if (b2to1ratio) {
			/* [15] - Phy2UnitClkRatio = 1 - Set 2:1 Ratio between Dunit and Phy */
			uiReg = (MV_REG_READ(REG_DDR_IO_ADDR) | (1 << REG_DDR_IO_CLK_RATIO_OFFS));
		} else {
			/* [15] - Phy2UnitClkRatio = 0 - Set 1:1 Ratio between Dunit and Phy */
			uiReg = (MV_REG_READ(REG_DDR_IO_ADDR) & ~(1 << REG_DDR_IO_CLK_RATIO_OFFS));
		}
		MV_REG_WRITE(REG_DDR_IO_ADDR, uiReg);				/* 0x1524 - DDR IO Register */

		/* Configure - 2T Mode - Restore original configuration */
		uiReg = MV_REG_READ(REG_DUNIT_CTRL_LOW_ADDR);
		uiReg &= ~(REG_DUNIT_CTRL_LOW_2T_MASK << REG_DUNIT_CTRL_LOW_2T_OFFS);	/* [3:4] 2T - Restore value */
		uiReg |= ((pDramInfo->ui2TMode & REG_DUNIT_CTRL_LOW_2T_MASK) << REG_DUNIT_CTRL_LOW_2T_OFFS);
		MV_REG_WRITE(REG_DUNIT_CTRL_LOW_ADDR, uiReg);			/* 0x1404 - DDR Controller Control Low Register */
	
		/* Configure - Restore CL and CWL - MRS Commands */
		uiReg = MV_REG_READ(REG_DFS_ADDR);
		uiReg &= ~(REG_DFS_CL_NEXT_STATE_MASK << REG_DFS_CL_NEXT_STATE_OFFS);
		uiReg &= ~(REG_DFS_CWL_NEXT_STATE_MASK << REG_DFS_CWL_NEXT_STATE_OFFS);
		if (uiFreq == DDR_400)
			uiTemp = ddr3CLtoValidCL(6);
		else		
			uiTemp = ddr3CLtoValidCL(pDramInfo->uiCL);
		uiReg |= ((uiTemp & REG_DFS_CL_NEXT_STATE_MASK) << REG_DFS_CL_NEXT_STATE_OFFS);						/* [8] - DfsCLNextState */
		if (uiFreq == DDR_400)
			uiReg |= (((0) & REG_DFS_CWL_NEXT_STATE_MASK) << REG_DFS_CWL_NEXT_STATE_OFFS);	/* [12] - DfsCWLNextState */
		else		
			uiReg |= (((pDramInfo->uiCWL) & REG_DFS_CWL_NEXT_STATE_MASK) << REG_DFS_CWL_NEXT_STATE_OFFS);	/* [12] - DfsCWLNextState */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */
	
		/* Optional - Configure - DDR3_Rtt_nom_CS# */
		for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
			if (pDramInfo->uiCsEna & (1<<uiCs)) {
				uiReg = MV_REG_READ(REG_DDR3_MR1_CS_ADDR + (uiCs << MR_CS_ADDR_OFFS));
				uiReg &= REG_DDR3_MR1_RTT_MASK;
				uiReg |= auiODTStatic[pDramInfo->uiCsEna][uiCs];
				MV_REG_WRITE(REG_DDR3_MR1_CS_ADDR + (uiCs << MR_CS_ADDR_OFFS), uiReg);
			}
		}

		/* Configure - Reset ADLLs - Set Reset */
		uiReg = (MV_REG_READ(REG_DRAM_PHY_CONFIG_ADDR) & REG_DRAM_PHY_CONFIG_MASK);
		/* [31:30]] - reset pup data ctrl ADLL */
		MV_REG_WRITE(REG_DRAM_PHY_CONFIG_ADDR, uiReg);	/* 0x15EC - DRAM PHY Config Register */
	
		/* Configure - Reset ADLLs - Release Reset */
		uiReg = (MV_REG_READ(REG_DRAM_PHY_CONFIG_ADDR) | ~REG_DRAM_PHY_CONFIG_MASK);
		/* [31:30] - normal pup data ctrl ADLL */
		MV_REG_WRITE(REG_DRAM_PHY_CONFIG_ADDR, uiReg);	/* 0x15EC - DRAM PHY Config uiRegister */
	
		/* Poll - Wait for APLL + ADLLs lock on new frequency */
		do {
			uiReg = ((MV_REG_READ(REG_PHY_LOCK_STATUS_ADDR)) & REG_PHY_LOCK_APLL_ADLL_STATUS_MASK);
		} while (uiReg != REG_PHY_LOCK_APLL_ADLL_STATUS_MASK); /* 0x1674 [10:0] - Phy lock status Register */
		
		/* Configure - Reset the PHY SDR clock divider */
		if (b2to1ratio) {
			/* Pup Reset Divider B - Set Reset */
			/* [28] - DataPupRdRST = 0 */
			uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & ~(1 << REG_SDRAM_CONFIG_PUPRSTDIV_OFFS));
			/* [28] - DataPupRdRST = 1 */
			uiTemp = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) | (1 << REG_SDRAM_CONFIG_PUPRSTDIV_OFFS));
			MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);		/* 0x1400 - SDRAM Configuration uiRegister */

			/* Pup Reset Divider B - Release Reset */
			MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiTemp);	/* 0x1400 - SDRAM Configuration uiRegister */
		}

		/* Configure - Reset the PHY Read FIFO and Write channels - Set Reset */
		uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & REG_SDRAM_CONFIG_MASK);
		/* [30:29] = 0 - Data Pup R/W path reset */
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);			/* 0x1400 - SDRAM Configuration uiRegister */

		/* Configure - DRAM Data PHY Read [30], Write [29] path reset - Release Reset */
		uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) | ~REG_SDRAM_CONFIG_MASK);
		/* [30:29] = '11' - Data Pup R/W path reset */
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);			/* 0x1400 - SDRAM Configuration uiRegister */

		/* Registered DIMM support */
		if (pDramInfo->bRegDimm) {
			/* Configure - Change register DRAM frequency band (band 1) - CWA_RC */
			uiReg = ((0x2 & REG_SDRAM_OPERATION_CWA_RC_MASK) << REG_SDRAM_OPERATION_CWA_RC_OFFS);
			/* Configure - Change register DRAM frequency band (band 1) - CWA_DATA */
			uiReg |= ((0 & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
			/* Configure - Set Delay - tMRD */
			uiReg |= (0 << REG_SDRAM_OPERATION_CWA_DELAY_SEL_OFFS);
			/* Configure - Issue CWA command with the above parameters */
			uiReg |= (REG_SDRAM_OPERATION_CMD_CWA & ~(pDramInfo->uiCsEna << REG_SDRAM_OPERATION_CS_OFFS));
			
			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */

			/* Poll - Wait for CWA operation completion */
			do {
				uiReg = (MV_REG_READ(REG_SDRAM_OPERATION_ADDR) & (REG_SDRAM_OPERATION_CMD_MASK));
			} while (uiReg);
			
			/* Configure - Change register DRAM operating speed (DDR3-1333 / DDR3-1600) - CWA_RC */
			uiReg = ((0xA & REG_SDRAM_OPERATION_CWA_RC_MASK) << REG_SDRAM_OPERATION_CWA_RC_OFFS);
			if (uiFreq <= DDR_400) {
				/* Configure - Change register DRAM operating speed (DDR3-800) - CWA_DATA */
				uiReg |= ((0x0 & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
			} else if ((uiFreq > DDR_400) && (uiFreq < DDR_533)) {
				/* Configure - Change register DRAM operating speed (DDR3-1066) - CWA_DATA */
				uiReg |= ((0x1 & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
//				uiReg |= ((0x3 & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
			} else if ((uiFreq >= DDR_533) && (uiFreq <= DDR_666)) {
				/* Configure - Change register DRAM operating speed (DDR3-1333) - CWA_DATA */
				uiReg |= ((0x2 & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
//				uiReg |= ((0x3 & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
			} else {
				/* Configure - Change register DRAM operating speed (DDR3-1600) - CWA_DATA */
				uiReg |= ((0x3 & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
			}
			/* Configure - Set Delay - tSTAB */
			uiReg |= (0x1 << REG_SDRAM_OPERATION_CWA_DELAY_SEL_OFFS);
			/* Configure - Issue CWA command with the above parameters */
			uiReg |= (REG_SDRAM_OPERATION_CMD_CWA & ~(pDramInfo->uiCsEna << REG_SDRAM_OPERATION_CS_OFFS));

			MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */

			/* Poll - Wait for CWA operation completion */
			do {
				uiReg = (MV_REG_READ(REG_SDRAM_OPERATION_ADDR) & (REG_SDRAM_OPERATION_CMD_MASK));
			} while (uiReg);
		}

		/* Configure - Exit Self Refresh */
		uiReg = (MV_REG_READ(REG_DFS_ADDR) & ~(1 << REG_DFS_SR_OFFS));	/* [2] - DfsSR  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);				/* 0x1528 - DFS uiRegister */

		/* Poll - DFS Register - 0x1528 [3] - DfsAtSR - All DRAM devices on all ranks are NOT in self refresh mode */
		do {
			uiReg = ((MV_REG_READ(REG_DFS_ADDR)) & (1 << REG_DFS_ATSR_OFFS));
		} while (uiReg);		/* Wait for '0' */

		/* Configure - Issue Refresh command */
		/* [3-0] = 0x2 - Refresh Command, [11-8] - enabled Cs */
		uiReg = REG_SDRAM_OPERATION_CMD_RFRS;
		for (uiCs = 0; uiCs < MAX_CS; uiCs++)
			if (pDramInfo->uiCsEna & (1 << uiCs))
				uiReg &= ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs));
	
		MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
	
		/* Poll - Wait for Refresh operation completion */
		do {
			uiReg = ((MV_REG_READ(REG_SDRAM_OPERATION_ADDR)) & REG_SDRAM_OPERATION_CMD_RFRS_DONE);
		} while (uiReg);				/* Wait for '0' */

		/* Configure - Block new external transactions - Disable*/
		uiReg = MV_REG_READ(REG_DFS_ADDR);
		uiReg &= ~(1 << REG_DFS_BLOCK_OFFS);					/* [1] - DfsBlock - Disable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */

		/* Configure -  XBAR Retry response during Block to enable internal access - Disable */
		uiReg = MV_REG_READ(REG_METAL_MASK_ADDR);
		uiReg |= (1 << REG_METAL_MASK_RETRY_OFFS);				/* [0] - RetryMask - Enable */
		MV_REG_WRITE(REG_METAL_MASK_ADDR, uiReg);  				/* 0x14B0 - Dunit MMask Register */
	
		/* Configure - restore CL */
		uiReg = MV_REG_READ(REG_DDR3_MR0_ADDR) & ~REG_DDR3_MR0_CL_MASK;
		if (uiFreq == DDR_400)
			uiTemp = ddr3CLtoValidCL(6);
		else
			uiTemp = ddr3CLtoValidCL(pDramInfo->uiCL);
		uiReg |= ((uiTemp & 0x1) << REG_DDR3_MR0_CL_OFFS);
		uiReg |= ((uiTemp & 0xE) << REG_DDR3_MR0_CL_HIGH_OFFS);
		MV_REG_WRITE(REG_DDR3_MR0_ADDR, uiReg);		/* 0x15D0 - DDR3 MR0 Register */
	
		/* Configure - restore CWL */
		uiReg = MV_REG_READ(REG_DDR3_MR2_ADDR) & ~(REG_DDR3_MR2_CWL_MASK << REG_DDR3_MR2_CWL_OFFS);
		if (uiFreq == DDR_400)
			uiReg |= ((0) << REG_DDR3_MR2_CWL_OFFS);
		else
			uiReg |= ((pDramInfo->uiCWL) << REG_DDR3_MR2_CWL_OFFS);
		MV_REG_WRITE(REG_DDR3_MR2_ADDR, uiReg);		/* 0x15D8 - DDR3 MR2 Register */
	
		DEBUG_DFS_C("DDR3 - DFS - Low To High - Ended successfuly - new Frequency - ", uiFreq, 1);
#endif		
		return MV_OK;

	} else {
		/* This Flow is relevant for Armada370 A0 and ArmadaXP Z1 */
		
		MV_U32 uiReg, uiFreqPar, uiTemp;
		MV_U32 uiCs = 0;
	
		DEBUG_DFS_C("DDR3 - DFS - Low To High - Starting DFS procedure to Frequency - ", uiFreq, 1);
	
		uiFreqPar = ddr3GetFreqParameter(uiFreq, b2to1ratio);		/* target uiFrequency - uiFreq */
	
		uiReg = 0x0000FF00;
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_1_ADDR, uiReg);
	
		uiReg = MV_REG_READ(REG_ODPG_CNTRL_ADDR);				/* 0x1600 - PHY lock mask uiRegister */
		uiReg |= (1<<REG_ODPG_CNTRL_OFFS);						/* [21] = 1 */
		MV_REG_WRITE(REG_ODPG_CNTRL_ADDR, uiReg);
	
		uiReg = MV_REG_READ(REG_PHY_LOCK_MASK_ADDR);			/* 0x1670 - PHY lock mask uiRegister */
		uiReg &= REG_PHY_LOCK_MASK_MASK;						/* [11:0] = 0 */
		MV_REG_WRITE(REG_PHY_LOCK_MASK_ADDR, uiReg);
	
		/* Enable reconfig MR Registers after DFS */
		uiReg = MV_REG_READ(REG_DFS_ADDR);						/* 0x1528 - DFS uiRegister */
		uiReg &= ~0x11;											/* [4] - Disable - reconfig MR uiRegisters after DFS_ERG */
																/* [0] - Enable - DRAM DLL after DFS */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);						/* 0x1528 - DFS uiRegister */
	
		/* Disable DRAM Controller to crossbar retry */
		uiReg = (MV_REG_READ(REG_METAL_MASK_ADDR) & ~(1 << 0));   		/* [0] - disable */
		MV_REG_WRITE(REG_METAL_MASK_ADDR, uiReg);  						/* 0x14B0 - Dunit MMask Register */
	
		/* Enable DRAM Blocking */
		uiReg = (MV_REG_READ(REG_DFS_ADDR) | (1<<REG_DFS_BLOCK_OFFS));	/*  [1] - DFS Block enable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);								/* 0x1528 - DFS uiRegister */
	
		/* Enable Self refresh */
		uiReg = (MV_REG_READ(REG_DFS_ADDR) | (1<<REG_DFS_SR_OFFS));		/*  [2] - DFS Self refresh enable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);								/* 0x1528 - DFS uiRegister */
	
		/* Poll DFS Register - All DRAM devices on all ranks are in self refresh mode -
		DFS can be executed afterwards */
		/* 0x1528 [3] - DfsAtSR  */
		do {
			uiReg = ((MV_REG_READ(REG_DFS_ADDR)) & (1<<REG_DFS_ATSR_OFFS));
		} while (uiReg == 0x0);											/* Wait for '1' */
	
		/* Set Correct Ratio - if uiFreq>MARGIN_FREQ use 2:1 ratio else use 1:1 ratio  */
		if (b2to1ratio) {
			/* [15] = 1 - Set 2:1 Ratio between Dunit and Phy */
			uiReg = (MV_REG_READ(REG_DDR_IO_ADDR) | (1<<REG_DDR_IO_CLK_RATIO_OFFS));
		} else {
			/* [15] = 0 - Set 1:1 Ratio between Dunit and Phy */
			uiReg = (MV_REG_READ(REG_DDR_IO_ADDR) & ~(1<<REG_DDR_IO_CLK_RATIO_OFFS));
		}
		MV_REG_WRITE(REG_DDR_IO_ADDR, uiReg);	/* 0x1524 - DDR IO Register */
	
		/* Switch HCLK Mux from (100Mhz) [16]=0, keep DFS request bit */
		uiReg = 0x20040000;
		/* [29] - training logic request DFS, [28:27] - preload patterns uiFrequency [18]  */
	
		/* 0x18488 - DRAM Init control status uiRegister */
		MV_REG_WRITE(REG_DRAM_INIT_CTRL_STATUS_ADDR, uiReg);
	
		uDelay(1);	/* Add delay between entering SR and start ratio modification */
	
		uiReg = 0x0000FFFF;	/*  Initial Setup - assure that teh "load new ratio" is clear (bit 24)
		and in the same chance, block reassertions of reset [15:8] and force reserved bits[7:0]. */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);		/* 0x18700 - CPU Div CLK control 0 */
	
		uiReg = 0x0000FF00;	/* RelaX whenever reset is asserted to that channel (good for any case) */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_1_ADDR, uiReg);		/* 0x18704 - CPU Div CLK control 0 */
	
		uiReg = (MV_REG_READ(REG_CPU_DIV_CLK_CTRL_3_ADDR) & REG_CPU_DIV_CLK_CTRL_3_FREQ_MASK);
		uiReg |= (uiFreqPar << REG_CPU_DIV_CLK_CTRL_3_FREQ_OFFS);
		/* full Integer ratio from PLL-out to ddr-clk */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_3_ADDR, uiReg);	/* 0x1870C - CPU Div CLK control 3 uiRegister*/
	
		/* 	Shut off clock enable to the DDRPHY clock channel (this is the "D"). All the rest
			are kept as is (forced, but could be read-modify-write).
			This is done now by RMW above.  */
	
		uiReg = 0x000FFF02;
	
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_4_ADDR, uiReg);
	
		uDelay(1); /* wait before replacing the clock on the DDR Phy Channel. */
	
		uiReg = 0x0102FDFF;
		/* 	This for triggering the frequency update. Bit[24] is the central control
			bits [23:16] == which channels to change ==2 ==> only DDR Phy (smooth transition)
			bits [15:8] == mask reset reassertion due to clock modification to these channels.
			bits [7:0] == not in use */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);	/* 0x18700 - CPU Div CLK control 0 uiRegister*/
	
		uDelay(1);
	
		/* Poll Div CLK status 0 uiRegister - indication that the clocks are active - 0x18718 [8] */
		do {
			uiReg = ((MV_REG_READ(REG_CPU_DIV_CLK_STATUS_0_ADDR)) & (1 << REG_CPU_DIV_CLK_ALL_STABLE_OFFS));
		} while (uiReg == 0);
	
		uiReg = 0x000000FF;
		/* Clean the CTRL0, to be ready for next resets and next requests of ratio modifications. */
		MV_REG_WRITE(REG_CPU_DIV_CLK_CTRL_0_ADDR, uiReg);	/* 0x18700 - CPU Div CLK control 0 uiRegister*/
	
		uDelay(5);
	
		if (b2to1ratio) {
			/* Pup Reset Divider B - Set Reset */
			uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & ~(1<<28)); 	/* [28] = 0 - Pup Reset Divider B */
			uiTemp = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) | (1<<28)); 	/* [28] = 1 - Pup Reset Divider B */
			MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);		/* 0x1400 - SDRAM Configuration uiRegister */
	
			/* Pup Reset Divider B - Release Reset */
			MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiTemp);	/* 0x1400 - SDRAM Configuration uiRegister */
		}
	
		/* DRAM Data PHYs ADLL Reset - Set Reset */
		uiReg = (MV_REG_READ(REG_DRAM_PHY_CONFIG_ADDR) & REG_DRAM_PHY_CONFIG_MASK);
		/* [31:30]] - reset pup data ctrl ADLL */
		MV_REG_WRITE(REG_DRAM_PHY_CONFIG_ADDR, uiReg);	/* 0x15EC - DRAM PHY Config Register */
	
		uDelay(25);
	
		/* APLL lock indication - Poll Phy lock status Register - 0x1674 [9] */
		do {
			uiReg = ((MV_REG_READ(REG_PHY_LOCK_STATUS_ADDR)) & (1 << REG_PHY_LOCK_STATUS_LOCK_OFFS));
		} while (uiReg == 0);
		
		/* DRAM Data PHYs ADLL Reset - Release Reset */
		uiReg = (MV_REG_READ(REG_DRAM_PHY_CONFIG_ADDR) | ~REG_DRAM_PHY_CONFIG_MASK);
		/* [31:30] - normal pup data ctrl ADLL */
		MV_REG_WRITE(REG_DRAM_PHY_CONFIG_ADDR, uiReg);	/* 0x15EC - DRAM PHY Config uiRegister */
	
		uDelay(10000);	/* Wait 10mSec */
	
		/* APLL lock indication - Poll Phy lock status Register - 0x1674 [11:0] */
		do {
			uiReg = ((MV_REG_READ(REG_PHY_LOCK_STATUS_ADDR)) & REG_PHY_LOCK_STATUS_LOCK_MASK);
		} while (uiReg != REG_PHY_LOCK_STATUS_LOCK_MASK);
	
		/* DRAM Data PHY Read [30], Write [29] path reset - Set Reset */
		uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & REG_SDRAM_CONFIG_MASK);
		/* [30:29] = 0 - Data Pup R/W path reset */
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);			/* 0x1400 - SDRAM Configuration uiRegister */
	
		/* DRAM Data PHY Read [30], Write [29] path reset - Release Reset */
		uiReg = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) | ~REG_SDRAM_CONFIG_MASK);
		/* [30:29] = '11' - Data Pup R/W path reset */
		MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);			/* 0x1400 - SDRAM Configuration uiRegister */
	
		/* Disable DFS Reconfig */
		uiReg = (MV_REG_READ(REG_DFS_ADDR) & ~(1<<4));
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);				/* 0x1528 - DFS uiRegister */
	
		uiReg = (MV_REG_READ(REG_DFS_ADDR) & ~(1<<REG_DFS_SR_OFFS));	/* [2] - DFS Self refresh disable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);				/* 0x1528 - DFS uiRegister */
	
		/* Poll DFS Register - 0x1528 [3] - DfsAtSR - All DRAM devices on all ranks are NOT in self refresh mode */
		do {
			uiReg = ((MV_REG_READ(REG_DFS_ADDR)) & (1<<REG_DFS_ATSR_OFFS));
		} while (uiReg);		/* Wait for '0' */
	
		/* 0x1404 */
		uiReg = ((MV_REG_READ(REG_DUNIT_CTRL_LOW_ADDR) & 0xFFFFFFE7)  | 0x2);
		
		/* Configure - 2T Mode - Restore original configuration */
		uiReg &= ~(REG_DUNIT_CTRL_LOW_2T_MASK << REG_DUNIT_CTRL_LOW_2T_OFFS);	/* [3:4] 2T - Restore value */
		uiReg |= ((pDramInfo->ui2TMode & REG_DUNIT_CTRL_LOW_2T_MASK) << REG_DUNIT_CTRL_LOW_2T_OFFS);
		MV_REG_WRITE(REG_DUNIT_CTRL_LOW_ADDR, uiReg);

		uDelay(1);	/* Wait 1us */
	
		for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
			if (pDramInfo->uiCsEna & (1<<uiCs)) {
	
				uiReg = (MV_REG_READ(REG_DDR3_MR1_ADDR));
				uiReg &= ~(1 << REG_DDR3_MR1_DLL_ENA_OFFS); /* DLL Enable */
				uiReg &= REG_DDR3_MR1_ODT_MASK;
				uiReg |= auiODTStatic[pDramInfo->uiCsEna][uiCs];
				MV_REG_WRITE(REG_DDR3_MR1_ADDR, uiReg);
	
				/* Issue MRS Command to current uiCs */
				uiReg = (REG_SDRAM_OPERATION_CMD_MR1 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
				/* [3-0] = 0x4 - MR1 Command, [11-8] - enable current uiCs */
				MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
	
				uDelay(MRS_DELAY);
	
				/* DLL Reset - MR0 */
				uiReg = (MV_REG_READ(REG_DDR3_MR0_ADDR));
				MV_REG_WRITE(REG_DDR3_MR0_ADDR, uiReg);
	
				/* Issue MRS Command to current uiCs */
				uiReg = (REG_SDRAM_OPERATION_CMD_MR0 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
				/* [3-0] = 0x4 - MR1 Command, [11-8] - enable current uiCs */
				MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
	
				uDelay(MRS_DELAY);
	
				/* MRS Command patch */
				/* Configure MR0 in uiCs[uiCs] */
				uiReg = 0x00000600;
				uiTemp = ddr3CLtoValidCL(pDramInfo->uiCL);
			
				uiReg |= ((uiTemp & 0x1) << 2);
				uiReg |= ((uiTemp & 0xE) << 3); /* to bit 4 */
				
				MV_REG_WRITE(REG_DDR3_MR0_ADDR, uiReg);
	
				/* Issue MRS Command to current uiCs */
				uiReg = (REG_SDRAM_OPERATION_CMD_MR0 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
				/* [3-0] = 0x4 - MR0 Command, [11-8] - enable current uiCs */
				MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
	
				uDelay(MRS_DELAY);
	
				/* Configure MR2 in uiCs[uiCs] */
				if (b2to1ratio)
					uiReg = (0x00000010 & REG_DDR3_MR2_ODT_MASK);
				else
					uiReg = (0x00000008 & REG_DDR3_MR2_ODT_MASK);	/* CWL=6 */
	
				uiReg |= auiODTDynamic[pDramInfo->uiCsEna][uiCs];
				MV_REG_WRITE(REG_DDR3_MR2_ADDR, uiReg);
	
				/* Issue MRS Command to current uiCs */
				uiReg = (REG_SDRAM_OPERATION_CMD_MR2 & ~(1 << (REG_SDRAM_OPERATION_CS_OFFS + uiCs)));
				/* [3-0] = 0x4 - MR2 Command, [11-8] - enable current uiCs */
				MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
	
				uDelay(MRS_DELAY);
	
				/* Set current uiRdSampleDelay  */
				uiReg = MV_REG_READ(REG_READ_DATA_SAMPLE_DELAYS_ADDR);
				uiReg &= ~(REG_READ_DATA_SAMPLE_DELAYS_MASK << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
				uiReg |= (pDramInfo->uiCL << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
				MV_REG_WRITE(REG_READ_DATA_SAMPLE_DELAYS_ADDR, uiReg);
	
				/* Set current uiRdReadyDelay  */
				uiReg = MV_REG_READ(REG_READ_DATA_READY_DELAYS_ADDR);
				uiReg &= ~(REG_READ_DATA_READY_DELAYS_MASK << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
				uiReg |= ((pDramInfo->uiCL+1) << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
				MV_REG_WRITE(REG_READ_DATA_READY_DELAYS_ADDR, uiReg);
			}
		}
	
		/* Enable ODT on DLL-on mode */
		MV_REG_WRITE(REG_SDRAM_ODT_CTRL_HIGH_ADDR, 0);
	
		uiReg = (MV_REG_READ(REG_DFS_ADDR) & ~(1<<REG_DFS_BLOCK_OFFS));	/*  [1] - DFS Block disable  */
		MV_REG_WRITE(REG_DFS_ADDR, uiReg);	/* 0x1528 - DFS uiRegister */
	
		/* Change DDR uiFrequency to 100MHz procedure: */
		uiReg = MV_REG_READ(REG_ODPG_CNTRL_ADDR);			/* 0x1600 - PHY lock mask uiRegister */
		uiReg &= ~(1<<REG_ODPG_CNTRL_OFFS);				/* [21] = 0 */
		MV_REG_WRITE(REG_ODPG_CNTRL_ADDR, uiReg);
	
		/* Change DDR uiFrequency to 100MHz procedure: */
		uiReg = MV_REG_READ(REG_PHY_LOCK_MASK_ADDR);	/* 0x1670 - PHY lock mask uiRegister */
		uiReg |= ~REG_PHY_LOCK_MASK_MASK;				/* [11:0] = FFF */
		MV_REG_WRITE(REG_PHY_LOCK_MASK_ADDR, uiReg);
	
		uiReg = (MV_REG_READ(REG_METAL_MASK_ADDR) | (1 << 0));   	/* [0] - disable */
		MV_REG_WRITE(REG_METAL_MASK_ADDR, uiReg);  					/* 0x14B0 - Dunit MMask Register */
	
		DEBUG_DFS_C("DDR3 - DFS - Low To High - Ended successfuly - new Frequency - ", uiFreq, 1);
		return MV_OK;
	}
}
