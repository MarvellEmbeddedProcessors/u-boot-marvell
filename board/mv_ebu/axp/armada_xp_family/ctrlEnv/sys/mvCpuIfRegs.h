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


#ifndef __INCmvCpuIfRegsh
#define __INCmvCpuIfRegsh

/****************************************/
/* ARM Control and Status Registers Map */
/****************************************/
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"

#define MV_CPUIF_REGS_BASE(cpu)			(MV_CPUIF_REGS_OFFSET(cpu))
#define MV_MISC_REGS_BASE			(MV_MISC_REGS_OFFSET)
#define MV_CLK_CMPLX_REGS_BASE		(MV_CLK_CMPLX_REGS_OFFSET)
#define MV_L2C_REGS_BASE			(MV_AURORA_L2_REGS_OFFSET)
#define MV_CPUIF_SHARED_REGS_BASE		(MV_MBUS_REGS_OFFSET)
#define MV_COHERENCY_FABRIC_REGS_BASE		(MV_COHERENCY_FABRIC_OFFSET)

#define CPU_CONFIG_REG(cpu)			(MV_CPUIF_REGS_BASE(cpu))
#define CPU_CTRL_STAT_REG(cpu)			(MV_CPUIF_REGS_BASE(cpu) + 0x8)
#define CPU_RESUME_ADDR_REG(cpu)		(MV_CPUIF_SHARED_REGS_BASE + (0x2124) + (cpu)*0x100)
#define CPU_RESET_REG(cpu)			(MV_CPUIF_SHARED_REGS_BASE + (0x800+(cpu)*8))
#define CPU_RESUME_CTRL_REG			(MV_CPUIF_SHARED_REGS_BASE + (0x988))
#define CPU_RSTOUTN_MASK_REG			(MV_MISC_REGS_BASE + 0x60)
#define CPU_SYS_SOFT_RST_REG			(MV_MISC_REGS_BASE + 0x64)
#define CPU_L2_CTRL_REG				(MV_L2C_REGS_BASE + 0x100)
#define CPU_L2_AUX_CTRL_REG			(MV_L2C_REGS_BASE + 0x104)
#define CPU_L2_BLOCK_ALLOCATION_REG             (MV_L2C_REGS_BASE + 0x78c)
#define CPU_L2_DATA_LOCKDOWN_REG(cpu)           (MV_L2C_REGS_BASE + 0x900 + (cpu)*0x8)
#define CPU_L2_INSTRUCTION_LOCKDOWN_REG(cpu)    (MV_L2C_REGS_BASE + 0x904 + (cpu)*0x8)
#define CPU_IO_BRIDGE_LOCKDOWN_REG              (MV_L2C_REGS_BASE + 0x984)
#define SOC_CTRL_REG				(MV_MISC_REGS_BASE + 0x4)
#define LVDS_PADS_CTRL_REG			(MV_MISC_REGS_BASE + 0xF0)
#define SOC_COHERENCY_FABRIC_CTRL_REG		(MV_COHERENCY_FABRIC_REGS_BASE)
#define SOC_COHERENCY_FABRIC_CFG_REG		(MV_COHERENCY_FABRIC_REGS_BASE + 0x4)
#define SOC_CFU_CFG_REG                                (MV_COHERENCY_FABRIC_REGS_BASE + 0x28)
#define SOC_CIB_CTRL_CFG_REG			(MV_COHERENCY_FABRIC_REGS_BASE + 0x80)
#define CFU_CFG_REG				(MV_COHERENCY_FABRIC_REGS_BASE + 0x28)

#define Fabric_Units_Priority_Control_REG	(MV_MBUS_REGS_OFFSET + 0x424)
#define Fabric_Units_Prefetch_Control_REG	(MV_MBUS_REGS_OFFSET + 0x42c)
#define CPUs_Data_PFen (0xf << 8)
#define CPUs_Data_PFen_MASK (0xf << 8)
#define CPU_PRIO_HIGH  0x2
#define CPUs_PRIO_MASK 0xff
#define CPU0_PRIO_HIGH (CPU_PRIO_HIGH << 0)
#define CPU1_PRIO_HIGH (CPU_PRIO_HIGH << 2)
#define CPU2_PRIO_HIGH (CPU_PRIO_HIGH << 4)
#define CPU3_PRIO_HIGH (CPU_PRIO_HIGH << 6)
/*   SoC Control Register bits  */
#define PCIE0_QUADX1_EN				(1<<7)
#define PCIE1_QUADX1_EN				(1<<8)

/* ARM Configuration register */
/* CPU_CONFIG_REG (CCR) */

/* Reset vector location */
#define CCR_VEC_INIT_LOC_OFFS			1
#define CCR_VEC_INIT_LOC_MASK			(1 << CCR_VEC_INIT_LOC_OFFS)
/* reset at 0x00000000 */
#define CCR_VEC_INIT_LOC_0000			(0 << CCR_VEC_INIT_LOC_OFFS)
/* reset at 0xFFFF0000 */
#define CCR_VEC_INIT_LOC_FF00			(1 << CCR_VEC_INIT_LOC_OFFS)

#define CCR_ENDIAN_INIT_OFFS			3
#define CCR_ENDIAN_INIT_MASK			(1 << CCR_ENDIAN_INIT_OFFS)
#define CCR_ENDIAN_INIT_LITTLE			(0 << CCR_ENDIAN_INIT_OFFS)
#define CCR_ENDIAN_INIT_BIG			(1 << CCR_ENDIAN_INIT_OFFS)

#define CCR_ARM_ID_SEL_OFFS			4
#define CCR_CPU_ID_SEL_MASK			(1 << CCR_ARM_ID_SEL_OFFS)
#define CCR_CPU_ID_SEL_ARM			(0 << CCR_ARM_ID_SEL_OFFS)
#define CCR_CPU_ID_SEL_MRVL			(1 << CCR_ARM_ID_SEL_OFFS)

#define CCR_TE_INIT_OFFS			5
#define CCR_TE_INIT_MASK			(1 << CCR_NCB_BLOCKING_OFFS)
#define CCR_TE_INIT_ARM				(0 << CCR_NCB_BLOCKING_OFFS)
#define CCR_TE_INIT_THUMB			(1 << CCR_NCB_BLOCKING_OFFS)

#define CCR_NFMI_EN_OFFS			6
#define CCR_NFMI_EN_MASK			(1 << CCR_NFMI_EN_OFFS)
#define CCR_NFMI_EN_DIS				(0 << CCR_NFMI_EN_OFFS)
#define CCR_NFMI_EN_EN				(1 << CCR_NFMI_EN_OFFS)

#define CCR_CORE_MODE_OFFS			9
#define CCR_CORE_MODE_MASK			(3 << CCR_CORE_MODE_OFFS)
#define CCR_CORE_MODE_ARM1176			(0 << CCR_CORE_MODE_OFFS)
#define CCR_CORE_MODE_CORTEX_A8			(1 << CCR_CORE_MODE_OFFS)
#define CCR_CORE_MODE_ARM11_MPC			(2 << CCR_CORE_MODE_OFFS)

#define CCR_UBIT_INIT_OFFS			11
#define CCR_UBIT_INIT_MASK			(1 << CCR_UBIT_INIT_OFFS)
#define CCR_UBIT_INIT_DIS			(0 << CCR_UBIT_INIT_OFFS)
#define CCR_UBIT_INIT_EN			(1 << CCR_UBIT_INIT_OFFS)

#define CCR_PCLK_WFI_OFFS			15
#define CCR_PCLK_WFI_MASK			(1 << CCR_PCLK_WFI_OFFS)
#define CCR_PCLK_WFI_DIS			(0 << CCR_PCLK_WFI_OFFS)
#define CCR_PCLK_WFI_EN				(1 << CCR_PCLK_WFI_OFFS)

#define CCR_SHARED_L2_OFFS			16
#define CCR_SHARED_L2_MASK			(1 << CCR_SHARED_L2_OFFS)
#define CCR_SHARED_L2_DIS			(0 << CCR_SHARED_L2_OFFS)
#define CCR_SHARED_L2_EN			(1 << CCR_SHARED_L2_OFFS)

#define CCR_SP_IN_MP_OFFS			17
#define CCR_SP_IN_MP_MASK			(1 << CCR_SP_IN_MP_OFFS)
#define CCR_SP_IN_MP_DIS			(0 << CCR_SP_IN_MP_OFFS)
#define CCR_SP_IN_MP_EN				(1 << CCR_SP_IN_MP_OFFS)

#define CCR_SRAM_LOW_LEAK_OFFS			19
#define CCR_SRAM_LOW_LEAK_MASK			(1 << CCR_SRAM_LOW_LEAK_OFFS)
#define CCR_SRAM_LOW_LEAK_EN			(0 << CCR_SRAM_LOW_LEAK_OFFS)
#define CCR_SRAM_LOW_LEAK_DIS			(1 << CCR_SRAM_LOW_LEAK_OFFS)

#define CCR_CLUSTER_ID_OFFS			24
#define CCR_CLUSTER_ID_MASK			(0xF << CCR_SRAM_LOW_LEAK_OFFS)


/* ARM Control and Status register */
/* CPU_CTRL_STAT_REG (CCSR) */

#define CCSR_SMP_N_AMP_OFFS			0
#define CCSR_SMP_N_AMP_MASK			(1 << CCSR_SMP_N_AMP_OFFS)

#define CCSR_ENDIAN_STATUS_OFFS			15
#define CCSR_ENDIAN_STATUS_MASK			(1 << CCSR_ENDIAN_STATUS_OFFS)
#define CCSR_ENDIAN_STATUS_LITTLE		(0 << CCSR_ENDIAN_STATUS_OFFS)
#define CCSR_ENDIAN_STATUS_BIG			(1 << CCSR_ENDIAN_STATUS_OFFS)


/* RSTOUTn Mask Register */
/* CPU_RSTOUTN_MASK_REG (CRMR) */

#define CRMR_SOFT_RST_OUT_OFFS			0
#define CRMR_SOFT_RST_OUT_MASK			(1 << CRMR_SOFT_RST_OUT_OFFS)
#define CRMR_SOFT_RST_OUT_ENABLE		(1 << CRMR_SOFT_RST_OUT_OFFS)
#define CRMR_SOFT_RST_OUT_DISABLE		(0 << CRMR_SOFT_RST_OUT_OFFS)

#define CRMR_PEX_SYSRST_OUT_OFFS(bus)		(1 + ((bus) & 0x3))
#define CRMR_PEX_SYSRST_OUT_MASK(bus)		(1 << CRMR_PEX_SYSRST_OUT_OFFS(bus))
#define CRMR_PEX_SYSRST_OUT_ENABLE(bus)		(1 << CRMR_PEX_SYSRST_OUT_OFFS(bus))
#define CRMR_PEX_SYSRST_OUT_DISABLE(bus)	(0 << CRMR_PEX_SYSRST_OUT_OFFS(bus))

#define CRMR_PEX_TRST_OUT_OFFS(bus)		(5 + ((bus) & 0x3))
#define CRMR_PEX_TRST_OUT_MASK(bus)		(1 << CRMR_PEX_TRST_OUT_OFFS(bus))
#define CRMR_PEX_TRST_OUT_ENABLE(bus)		(1 << CRMR_PEX_TRST_OUT_OFFS(bus))
#define CRMR_PEX_TRST_OUT_DISABLE(bus)		(0 << CRMR_PEX_TRST_OUT_OFFS(bus))


/* System Software Reset Register */
/* CPU_SYS_SOFT_RST_REG (CSSRR) */

#define CSSRR_SYSTEM_SOFT_RST			BIT0


/* CPU_L2_CTRL_REG fields */

#define CL2CR_L2_EN_OFFS			0
#define CL2CR_L2_EN_MASK			(1 << CL2CR_L2_EN_OFFS)

/* CPU_L2_AUX_CTRL_REG fields */

#define CL2ACR_WB_WT_ATTR_OFFS			0
#define CL2ACR_WB_WT_ATTR_MASK			(3 << CL2ACR_WB_WT_ATTR_OFFS)
#define CL2ACR_WB_WT_ATTR_PAGE			(0 << CL2ACR_WB_WT_ATTR_OFFS)
#define CL2ACR_WB_WT_ATTR_WB			(1 << CL2ACR_WB_WT_ATTR_OFFS)
#define CL2ACR_WB_WT_ATTR_WT			(2 << CL2ACR_WB_WT_ATTR_OFFS)

#define CL2ACR_PFU_OFFS				2
#define CL2ACR_PFU_MASK				(1 << CL2ACR_PFU_OFFS)
#define CL2ACR_PFU_EN				(1 << CL2ACR_PFU_OFFS)
#define CL2ACR_PFU_DIS				(0 << CL2ACR_PFU_OFFS)

#define CL2ACR_L2_SIZE_OFFS			10
#define CL2ACR_L2_SIZE_MASK			(3 << CL2ACR_L2_SIZE_OFFS)
#define CL2ACR_L2_SIZE_KB(reg)			((((((reg) & 0x3) & CL2ACR_L2_SIZE_MASK) \
						>> CL2ACR_PFU_OFFS) + 1) * _512K)

#define CL2ACR_ASSOC_OFFS			13
#define CL2ACR_ASSOC_MASK			(0xF << CL2ACR_ASSOC_OFFS)

#define CL2ACR_L2_WAY_SZ_OFFS			17
#define CL2ACR_L2_WAY_SZ_MASK			(7 << CL2ACR_L2_WAY_SZ_OFFS)
#define CL2ACR_L2_WAY_SZ_KB(reg)		(_16K << (((((reg) & 0x7) & CL2ACR_L2_SIZE_MASK) \
						>> CL2ACR_L2_WAY_SZ_OFFS)))

#define CL2ACR_ECC_OFFS				20
#define CL2ACR_ECC_MASK				(1 << CL2ACR_ECC_OFFS)
#define CL2ACR_ECC_EN				(1 << CL2ACR_ECC_OFFS)
#define CL2ACR_ECC_DIS				(0 << CL2ACR_ECC_OFFS)

#define CL2ACR_PARITY_OFFS			21
#define CL2ACR_PARITY_MASK			(1 << CL2ACR_PARITY_OFFS)
#define CL2ACR_PARITY_EN			(1 << CL2ACR_PARITY_OFFS)
#define CL2ACR_PARITY_DIS			(0 << CL2ACR_PARITY_OFFS)

#define CL2ACR_INVAL_UCE_OFFS			22
#define CL2ACR_INVAL_UCE_MASK			(1 << CL2ACR_INVAL_UCE_OFFS)
#define CL2ACR_INVAL_UCE_EN			(1 << CL2ACR_INVAL_UCE_OFFS)
#define CL2ACR_INVAL_UCE_DIS			(0 << CL2ACR_INVAL_UCE_OFFS)

#define CL2ACR_FORCE_WA_OFFS			23
#define CL2ACR_FORCE_WA_MASK			(3 << CL2ACR_FORCE_WA_OFFS)
#define CL2ACR_FORCE_WA_DISABLE			(0 << CL2ACR_FORCE_WA_OFFS)
#define CL2ACR_FORCE_NO_WA			(1 << CL2ACR_FORCE_WA_OFFS)
#define CL2ACR_FORCE_WA				(2 << CL2ACR_FORCE_WA_OFFS)

#define CL2ACR_REP_STRGY_OFFS			27
#define CL2ACR_REP_STRGY_MASK			(3 << CL2ACR_REP_STRGY_OFFS)

#define CL2ACR_REP_STRGY_LFSR_MASK              (0x1 << CL2ACR_REP_STRGY_OFFS)
#define CL2ACR_REP_STRGY_semiPLRU_MASK		(0x2 << CL2ACR_REP_STRGY_OFFS)
#define CL2ACR_REP_STRGY_semiPLRU_WA_MASK          (0x3 << CL2ACR_REP_STRGY_OFFS)
#define CL2_DUAL_EVICTION		(0x1 << 4)
#define CL2_PARITY_ENABLE		(0x1 << 21)
#define CL2_InvalEvicLineUCErr          (0x1 << 22)

/* SOC_CTRL_REG fields */
#define SCR_PEX_ENA_OFFS(pex)			((pex) & 0x3)
#define SCR_PEX_ENA_MASK(pex)			(1 << pex)

#define SCR_PEX_4BY1_OFFS(pex)			((pex) + 7)
#define SCR_PEX_4BY1_MASK(pex)			(1 << SCR_PEX_4BY1_OFFS(pex))

#define SCR_PEX0_4BY1_OFFS			7
#define SCR_PEX0_4BY1_MASK			(1 << SCR_PEX0_4BY1_OFFS)

#define SCR_PEX1_4BY1_OFFS			8
#define SCR_PEX1_4BY1_MASK			(1 << SCR_PEX1_4BY1_OFFS)

#define PCIE1_CLK_OUT_EN_OFF                   5
#define PCIE1_CLK_OUT_EN_MASK                  (1 << PCIE1_CLK_OUT_EN_OFF)

#define PCIE0_CLK_OUT_EN_OFF                   4
#define PCIE0_CLK_OUT_EN_MASK                  (1 << PCIE0_CLK_OUT_EN_OFF)

/* LVDS_PADS_CTRL_REG fields */
#define LVDS_PADS_CONF_PD_OFFS(idx)		(16 + idx)
#define LVDS_PADS_CONF_PD_MASK(idx)		(1 << (16 + idx))
#define LVDS_PADS_CONF_PD_EN(idx, en)		((en ? 0 : 1) << LVDS_PADS_CONF_PD_OFFS(idx))


/*******************************************/
/* Main Interrupt Controller Registers Map */
/*******************************************/

#define CPU_MAIN_INT_CAUSE_REG(vec, cpu)	(MV_CPUIF_REGS_BASE(cpu) + 0x80 + (vec * 0x4))
#define CPU_MAIN_INT_TWSI_OFFS(i)			(2 + i)
#define CPU_MAIN_INT_CAUSE_TWSI(i)			(31 + i)

#define CPU_CF_LOCAL_MASK_REG(cpu)			(MV_CPUIF_REGS_BASE(cpu) + 0xc4)
#define CPU_CF_LOCAL_MASK_PMU_MASK_OFFS		18
#define CPU_INT_SOURCE_CONTROL_REG(i)		(MV_CPUIF_SHARED_REGS_BASE + 0xB00 + (i * 0x4))

#define CPU_INT_SOURCE_CONTROL_IRQ_OFFS		28 
#define CPU_INT_SOURCE_CONTROL_IRQ_MASK		(1 << CPU_INT_SOURCE_CONTROL_IRQ_OFFS )

#define CPU_INT_SET_ENABLE_REG			(MV_CPUIF_SHARED_REGS_BASE + 0xA30)
#define CPU_INT_CLEAR_ENABLE_REG		(MV_CPUIF_SHARED_REGS_BASE + 0xA34)

#define CPU_INT_SET_MASK_OFFS			(0xB8)
#define CPU_INT_CLEAR_MASK_OFFS			(0xBC)

#define CPU_INT_SET_MASK_REG(cpu)		(MV_CPUIF_REGS_BASE(cpu) + CPU_INT_SET_MASK_OFFS)
#define CPU_INT_CLEAR_MASK_REG(cpu)		(MV_CPUIF_REGS_BASE(cpu) + CPU_INT_CLEAR_MASK_OFFS)

#define CPU_INT_SET_MASK_LOCAL_REG		(MV_CPUIF_LOCAL_REGS_OFFSET + CPU_INT_SET_MASK_OFFS)
#define CPU_INT_CLEAR_MASK_LOCAL_REG	(MV_CPUIF_LOCAL_REGS_OFFSET + CPU_INT_CLEAR_MASK_OFFS)

#define CPU_SNOOP_FILTER_CTRL_REG		(MV_CPUIF_LOCAL_REGS_OFFSET + 0x20)

#define MV_IRQ_NR							116


/*******************************************/
/* ARM Doorbell Registers Map		   */
/*******************************************/
#define CPU_SW_TRIG_IRQ						(MV_MBUS_REGS_OFFSET + 0xA04)
#define CPU_DOORBELL_IN_REG					(MV_CPUIF_LOCAL_REGS_OFFSET + 0x78)
#define CPU_DOORBELL_IN_MASK_REG			(MV_CPUIF_LOCAL_REGS_OFFSET + 0x7C)
#define CPU_HOST_TO_ARM_DRBL_REG(cpu)		(MV_CPUIF_REGS_BASE(cpu) + 0x78)
#define CPU_HOST_TO_ARM_MASK_REG(cpu)		(MV_CPUIF_REGS_BASE(cpu) + 0x7C)
#define CPU_ARM_TO_HOST_DRBL_REG(cpu)		(MV_CPUIF_REGS_BASE(cpu) + 0x70)
#define CPU_ARM_TO_HOST_MASK_REG(cpu)		(MV_CPUIF_REGS_BASE(cpu) + 0x74)

/*******************************************/
/* CLOCK Complex Registers Map			   */
/*******************************************/

#define CPU_DIV_CLK_CTRL0_REG				(MV_CLK_CMPLX_REGS_OFFSET)
#define CPU_DIV_CLK_CTRL0_RESET_MASK_OFFS	8
#define CPU_DIV_CLK_CTRL2_RATIO_FULL0_REG	(MV_CLK_CMPLX_REGS_OFFSET + 0x8)
#define CPU_DIV_CLK_CTRL2_NB_RATIO_OFFS		16
#define CPU_DIV_CLK_CTRL3_RATIO_FULL1_REG	(MV_CLK_CMPLX_REGS_OFFSET + 0xC)
#define CPU_DIV_CLK_CTRL3_CPU_RATIO_OFFS	8

/* CPU control register map */
/* Set bits means value is about to change according to new value */
#define CPU_CONFIG_DEFAULT_MASK         	(CCR_VEC_INIT_LOC_MASK)
#define CPU_CONFIG_DEFAULT                      (CCR_VEC_INIT_LOC_FF00)


#endif /* __INCmvCpuIfRegsh */

