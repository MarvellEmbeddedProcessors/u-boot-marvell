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

	*	Redistributions of source code must retain the above copyright notice,
		this list of conditions and the following disclaimer.

	*	Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

	*	Neither the name of Marvell nor the names of its contributors may be
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

#ifndef __INCmvDramIfRegsh
#define __INCmvDramIfRegsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysDdrConfig.h"

#define SDRAM_WIN_BASE_REG(winNum)		(MV_DDR_WIN_REGS_BASE + 0x180 + winNum * 0x8)
#define SDRAMWBR_BASE_OFFS			24
#define SDRAMWBR_BASE_MASK			(0xFF << SDRAMWBR_BASE_OFFS)
#define SDRAMWBR_BASE_EXT_MASK			(0xF)

#define SDRAMWBR_BASE_ALIGNMENT			0x10000

#define SDRAM_WIN_CTRL_REG(winNum)		(MV_DDR_WIN_REGS_BASE + 0x184 + winNum * 0x8)
#define SDRAMWCR_ENABLE				BIT0
#define SDRAMWCR_CS_OFFS			2
#define SDRAMWCR_CS_MASK			(0x7 << SDRAMWCR_CS_OFFS)
#define SDRAMWCR_SIZE_OFFS			24
#define SDRAMWCR_SIZE_MASK			(0xFF << SDRAMWCR_SIZE_OFFS)
#define SDRAMWCR_SIZE_ALLIGNMENT		0x1000000

#define SDRAM_WIN_CTRL_WIN_ENA_OFFS		0
#define SDRAM_MAX_ADDR					0xC0000000
#define SDRAM_ADDR_MASK					0xFF000000
#define SDRAM_MAX_CS					4
#define SRAM_WIN_CTRL_DEFAULT_VAL		0x100

/* DDR SDRAM Initialization Control Register (DSICR) */
#define DDR_SDRAM_INIT_CTRL_REG	    (MV_DDR_REGS_BASE + 0x1480)
#define DSICR_INIT_EN		    	(1 << 0)
#define DSICR_T200_SET		    	(1 << 8)

/* sdram extended mode2 register (SEM2R) */
#define SDRAM_PHY_REGISTER_FILE_ACCESS		(MV_DDR_CTRL_REGS_BASE + 0x16A0)

/* sdram error registers */
#define SDRAM_ERROR_CAUSE_REG               (MV_DDR_REGS_BASE + 0x14d0)
#define SDRAM_ERROR_MASK_REG                (MV_DDR_REGS_BASE + 0x14d4)
#define SDRAM_ERROR_DATA_LOW_REG            (MV_DDR_REGS_BASE + 0x1444)
#define SDRAM_ERROR_DATA_HIGH_REG           (MV_DDR_REGS_BASE + 0x1440)
#define SDRAM_ERROR_ADDR_REG                (MV_DDR_REGS_BASE + 0x1450)
#define SDRAM_ERROR_ECC_REG                 (MV_DDR_REGS_BASE + 0x1448)
#define SDRAM_CALC_ECC_REG                  (MV_DDR_REGS_BASE + 0x144c)
#define SDRAM_ECC_CONTROL_REG               (MV_DDR_REGS_BASE + 0x1454)
#define SDRAM_SINGLE_BIT_ERR_CNTR_REG 		(MV_DDR_REGS_BASE + 0x1458)
#define SDRAM_DOUBLE_BIT_ERR_CNTR_REG		(MV_DDR_REGS_BASE + 0x145c)

/* SDRAM Error Cause Register (SECR) */
#define SECR_SINGLE_BIT_ERR			BIT0
#define SECR_DOUBLE_BIT_ERR			BIT1
#define SECR_DATA_PATH_PARITY_ERR	BIT2
#define SECR_TRAINING_ERR_DETECTED	BIT3
/* SDRAM Error Address Register (SEAR) */
#define SEAR_ERR_TYPE_OFFS			0
#define SEAR_ERR_TYPE_MASK      	(1 << SEAR_ERR_TYPE_OFFS)
#define SEAR_ERR_TYPE_SINGLE    	0
#define SEAR_ERR_TYPE_DOUBLE    	(1 << SEAR_ERR_TYPE_OFFS)
#define SEAR_ERR_CS_OFFS			1
#define SEAR_ERR_CS_MASK			(3 << SEAR_ERR_CS_OFFS)
#define SEAR_ERR_CS(csNum)			(csNum << SEAR_ERR_CS_OFFS)
#define SEAR_ERR_ADDR_OFFS      	3
#define SEAR_ERR_ADDR_MASK      	(0x1FFFFFFF << SEAR_ERR_ADDR_OFFS)

/* SDRAM ECC Control Register (SECR) */
#define SECR_FORCEECC_OFFS          0
#define SECR_FORCEECC_MASK          (0xFF << SECR_FORCEECC_OFFS)
#define SECR_FORCEEN_OFFS           8
#define SECR_FORCEEN_MASK           (1 << SECR_FORCEEN_OFFS)
#define SECR_ECC_CALC_MASK          (0 << SECR_FORCEEN_OFFS)
#define SECR_ECC_USER_MASK          (1 << SECR_FORCEEN_OFFS)
#define SECR_PERRPROP_EN            BIT9
#define SECR_CNTMODE_OFFS           10
#define SECR_CNTMODE_MASK           (1 << SECR_CNTMODE_OFFS)
#define SECR_ALL_IN_CS0             (0 << SECR_CNTMODE_OFFS)
#define SECR_NORMAL_COUNTER         (1 << SECR_CNTMODE_OFFS)
#define SECR_THRECC_OFFS            16
#define SECR_THRECC_MAX             0xFF
#define SECR_THRECC_MASK            (SECR_THRECC_MAX << SECR_THRECC_OFFS)
#define SECR_THRECC(threshold)      (threshold << SECR_THRECC_OFFS)

/* FPGA - Delay Phase */
#define FPGA_DDR3_DELAY_PHASE		0x00000004

 /* SDRAM CSn Size Register (SCSR) */
#define SDRAM_SIZE_REG(csNum)	(0x1504 + ((csNum) * 8))
#define SCSR_SIZE_OFFS			24
#define SCSR_SIZE_MASK			(0xff << SCSR_SIZE_OFFS)
#define SCSR_SIZE_ALIGNMENT		0x1000000
#define SCSR_WIN_EN			BIT0

#define ECC_SUPPORT
#define REG_SDRAM_CONFIG_ADDR                                   0x1400
#define REG_SDRAM_CONFIG_ECC_OFFS                               18
#define REG_SDRAM_CONFIG_IERR									19
#define REG_STATIC_DRAM_DLB_CONTROL                             0x1700
#define DLB_ENABLE 0x1
#define DLB_WRITE_COALESING (0x1 << 2)
#define DLB_AXI_PREFETCH_EN (0x1 << 3)
#define DLB_MBUS_PREFETCH_EN (0x1 << 4)
#define PreFetchNLnSzTr (0x1 << 6)
#define PF_RegretEn (0x1<<9)
#define DLB_BUS_Optimization_Weights_REG     0x1704
#define DLB_Eviction_Timers_Register_REG     0x1710
#define SameBankOPageWeight (0x1 << 0)
#define DiffBankOPageWeight (0x1 << 2)
#define SameCmdWeight (0x1 << 6)
#define PfRspWeight (0x1 << 16)
#define PfPrioSrc (0x1 << 19)
#define EvictBlockWin (0x18B << 0)
#define EvictBlockWin_MASK (0x3FF << 0)
#define EvictBlockWinLow ( 0x1f << 10)
#define EvictBlockWinLow_MASK ( 0x1F << 10)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCmvDramIfRegsh */
