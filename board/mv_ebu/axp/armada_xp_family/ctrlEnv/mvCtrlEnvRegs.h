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

#ifndef __INCmvCtrlEnvRegsh
#define __INCmvCtrlEnvRegsh

#include "mvCtrlEnvSpec.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* CV Support */
#define PEX0_MEM0 	PEX0_MEM
#define PCI0_MEM0	PEX0_MEM

/* Controller revision info */
#define PCI_CLASS_CODE_AND_REVISION_ID		0x008
#define PCCRIR_REVID_OFFS			0		/* Revision ID */
#define PCCRIR_REVID_MASK			(0xff << PCCRIR_REVID_OFFS)

/* Controler environment registers offsets */
#define MV_TDM_IRQ_NUM				56


/* Coherent Fabric Control and Status */
#define MV_COHERENCY_FABRIC_CTRL_REG		(MV_COHERENCY_FABRIC_OFFSET + 0x0)
#define MV_COHERENCY_FABRIC_CFG_REG		(MV_COHERENCY_FABRIC_OFFSET + 0x4)

/* CIB registers offsets */
#define MV_CIB_CTRL_CFG_REG			(MV_COHERENCY_FABRIC_OFFSET + 0x80)

/* PMU_NFABRIC PMU_NFABRIC PMU_UNIT_SERVICE Units */
#define MV_L2C_NFABRIC_PM_CTRL_CFG_REG		(MV_PMU_NFABRIC_UNIT_SERV_OFFSET + 0x4)
#define MV_L2C_NFABRIC_PM_CTRL_CFG_PWR_DOWN	(1 << 20)

#define MV_L2C_NFABRIC_PWR_DOWN_FLOW_CTRL_REG	(MV_PMU_NFABRIC_UNIT_SERV_OFFSET + 0x8)

#define PM_CONTROL_AND_CONFIG_REG(cpu)		(MV_CPU_PMU_UNIT_SERV_OFFSET(cpu) + 0x4)
#define PM_CONTROL_AND_CONFIG_DFS_REQ		(1 << 18)
#define PM_CONTROL_AND_CONFIG_PWDDN_REQ		(1 << 16)
#define PM_CONTROL_AND_CONFIG_L2_PWDDN		(1 << 20)

#define PM_STATUS_AND_MASK_REG(cpu)		(MV_CPU_PMU_UNIT_SERV_OFFSET(cpu) + 0xc)
#define PM_STATUS_AND_MASK_CPU_IDLE_WAIT	(1 << 16)
#define PM_STATUS_AND_MASK_SNP_Q_EMPTY_WAIT	(1 << 17)
#define PM_STATUS_AND_MASK_IRQ_WAKEUP		(1 << 20)
#define PM_STATUS_AND_MASK_FIQ_WAKEUP		(1 << 21)
#define PM_STATUS_AND_MASK_DBG_WAKEUP		(1 << 22)
#define PM_STATUS_AND_MASK_IRQ_MASK		(1 << 24)
#define PM_STATUS_AND_MASK_FIQ_MASK		(1 << 25)

#define PM_EVENT_STATUS_AND_MASK_REG(cpu)		(MV_CPU_PMU_UNIT_SERV_OFFSET(cpu) + 0x20)
#define PM_EVENT_STATUS_AND_MASK_DFS_DONE_OFFS			1
#define PM_EVENT_STATUS_AND_MASK_DFS_DONE_MASK_OFFS		17

#define PM_CPU_BOOT_ADDR_REDIRECT(cpu)		(MV_CPU_PMU_UNIT_SERV_OFFSET(cpu) + 0x24)

/* Power Management Memory Power Down Registers 1 - 6 */
#define POWER_MNG_MEM_CTRL_REG(num)		((num) < 6 ? 0x1820C + (num) * 4 : 0x18228)
#define PMC_MCR_NUM_COMM			6
#define PMC_MCR_NUM_PEX				2
#define PMC_MCR_NUM_USB				4
#define PMC_MCR_NUM_DUNIT			3
#define PMC_MCR_NUM_DEVB			4
#define PMC_MCR_NUM_NF				4
#define PMC_MCR_NUM_XOR				4
#define PMC_MCR_NUM_SATA			5
#define PMC_MCR_NUM_CESA			4
#define PMC_MCR_NUM_GE				5
#define PMC_MCR_NUM_PNC				5
#define PMC_MCR_NUM_BM				5
#define PMC_MCR_NUM_PDMA			1
#define PMC_MCR_NUM_NCS				3
#define PMC_MCR_NUM_CFU				3
#define PMC_MCR_NUM_L2				3
#define PMC_MCR_NUM_CIB				3
#define PMC_MCR_NUM_CPU				3
#define PMC_MCR_NUM_IDMA			4
#define PMC_MCR_NUM_LCD				4
#define PMC_MCR_NUM_PMU				6

#define PMC_COMMSTOPMEM_OFFS			4
#define PMC_COMMSTOPMEM_MASK			(7 << PMC_COMMSTOPMEM_OFFS)
#define PMC_COMMSTOPMEM_EN			(0 << PMC_COMMSTOPMEM_OFFS)
#define PMC_COMMSTOPMEM_STOP			(1 << PMC_COMMSTOPMEM_OFFS)

#define PMC_PEXSTOPMEM_OFFS(port)		((port) < 10 ? ((port) * 3) : 0)
#define PMC_PEXSTOPMEM_MASK(port)		(7 << PMC_PEXSTOPMEM_OFFS(port))
#define PMC_PEXSTOPMEM_EN(port)			(0 << PMC_PEXSTOPMEM_OFFS(port))
#define PMC_PEXSTOPMEM_STOP(port)		(1 << PMC_PEXSTOPMEM_OFFS(port))

#define PMC_USBSTOPMEM_OFFS(port)		((port) < 3 ? (3 + (port) * 3) : 0)
#define PMC_USBSTOPMEM_MASK(port)		(7 << PMC_USBSTOPMEM_OFFS(port))
#define PMC_USBSTOPMEM_EN(port)			(0 << PMC_USBSTOPMEM_OFFS(port))
#define PMC_USBSTOPMEM_STOP(port)		(1 << PMC_USBSTOPMEM_OFFS(port))

#define PMC_DUNITSTOPMEM_OFFS			12
#define PMC_DUNITSTOPMEM_MASK			(7 << PMC_DUNITSTOPMEM_OFFS)
#define PMC_DUNITSTOPMEM_EN			(0 << PMC_DUNITSTOPMEM_OFFS)
#define PMC_DUNITSTOPMEM_STOP			(1 << PMC_DUNITSTOPMEM_OFFS)

#define PMC_NFSTOPMEM_OFFS			27
#define PMC_NFSTOPMEM_MASK			(7 << PMC_NFSTOPMEM_OFFS)
#define PMC_NFSTOPMEM_EN			(0 << PMC_NFSTOPMEM_OFFS)
#define PMC_NFSTOPMEM_STOP			(1 << PMC_NFSTOPMEM_OFFS)

#define PMC_DEVBSTOPMEM_OFFS			21
#define PMC_DEVBSTOPMEM_MASK			(7 << PMC_DEVBSTOPMEM_OFFS)
#define PMC_DEVBSTOPMEM_EN			(0 << PMC_DEVBSTOPMEM_OFFS)
#define PMC_DEVBSTOPMEM_STOP			(1 << PMC_DEVBSTOPMEM_OFFS)

#define PMC_XORSTOPMEM_OFFS(port)		((port) == 0 ? 15 : 24)
#define PMC_XORSTOPMEM_MASK(port)		(7 << PMC_XORSTOPMEM_OFFS(port))
#define PMC_XORSTOPMEM_EN(port)			(0 << PMC_XORSTOPMEM_OFFS(port))
#define PMC_XORSTOPMEM_STOP(port)		(1 << PMC_XORSTOPMEM_OFFS(port))

#define PMC_SATASTOPMEM_OFFS(port)		((port) == 0 ? 18 : 24)
#define PMC_SATASTOPMEM_MASK(port)		(0x3F << PMC_SATASTOPMEM_OFFS(port))
#define PMC_SATASTOPMEM_EN(port)		(0 << PMC_SATASTOPMEM_OFFS(port))
#define PMC_SATASTOPMEM_STOP(port)		(9 << PMC_SATASTOPMEM_OFFS(port))

#define PMC_CESASTOPMEM_OFFS			18
#define PMC_CESASTOPMEM_MASK			(7 << PMC_CESASTOPMEM_OFFS)
#define PMC_CESASTOPMEM_EN			(0 << PMC_CESASTOPMEM_OFFS)
#define PMC_CESASTOPMEM_STOP			(1 << PMC_CESASTOPMEM_OFFS)

#define PMC_GESTOPMEM_OFFS(port)		((port) < 4 ? (9 - (port) * 3) : 0)
#define PMC_GESTOPMEM_MASK(port)		(7 << PMC_GESTOPMEM_OFFS(port))
#define PMC_GESTOPMEM_EN(port)			(0 << PMC_GESTOPMEM_OFFS(port))
#define PMC_GESTOPMEM_STOP(port)		(1 << PMC_GESTOPMEM_OFFS(port))

#define PMC_PNCSTOPMEM_OFFS			12
#define PMC_PNCSTOPMEM_MASK			(7 << PMC_PNCSTOPMEM_OFFS)
#define PMC_PNCSTOPMEM_EN			(0 << PMC_PNCSTOPMEM_OFFS)
#define PMC_PNCSTOPMEM_STOP			(1 << PMC_PNCSTOPMEM_OFFS)

#define PMC_BMSTOPMEM_OFFS			15
#define PMC_BMSTOPMEM_MASK			(7 << PMC_BMSTOPMEM_OFFS)
#define PMC_BMSTOPMEM_EN			(0 << PMC_BMSTOPMEM_OFFS)
#define PMC_BMSTOPMEM_STOP			(1 << PMC_BMSTOPMEM_OFFS)

#define PMC_PDMASTOPMEM_OFFS			0
#define PMC_PDMATOPMEM_MASK			(7 << PMC_PDMASTOPMEM_OFFS)
#define PMC_PDMASTOPMEM_EN			(0 << PMC_PDMASTOPMEM_OFFS)
#define PMC_PDMASTOPMEM_STOP			(1 << PMC_PDMASTOPMEM_OFFS)

#define PMC_NCSSTOPMEM_OFFS			24
#define PMC_NCSSTOPMEM_MASK			(7 << PMC_NCSSTOPMEM_OFFS)
#define PMC_NCSSTOPMEM_EN			(0 << PMC_NCSSTOPMEM_OFFS)
#define PMC_NCSSTOPMEM_STOP			(1 << PMC_NCSSTOPMEM_OFFS)

#define PMC_CFUSTOPMEM_OFFS			21
#define PMC_CFUSTOPMEM_MASK			(7 << PMC_CFUSTOPMEM_OFFS)
#define PMC_CFUSTOPMEM_EN			(0 << PMC_CFUSTOPMEM_OFFS)
#define PMC_CFUSTOPMEM_STOP			(1 << PMC_CFUSTOPMEM_OFFS)

#define PMC_L2STOPMEM_OFFS			18
#define PMC_L2STOPMEM_MASK			(7 << PMC_L2STOPMEM_OFFS)
#define PMC_L2STOPMEM_EN			(0 << PMC_L2STOPMEM_OFFS)
#define PMC_L2STOPMEM_STOP			(1 << PMC_L2STOPMEM_OFFS)

#define PMC_CIBSTOPMEM_OFFS			15
#define PMC_CIBSTOPMEM_MASK			(7 << PMC_CIBSTOPMEM_OFFS)
#define PMC_CIBSTOPMEM_EN			(0 << PMC_CIBSTOPMEM_OFFS)
#define PMC_CIBSTOPMEM_STOP			(1 << PMC_CIBSTOPMEM_OFFS)

/* TODO - verify, the manual has no description */
#define PMC_CPUSTOPMEM_OFFS(id)			((id) < 4 ? (id) * 3 : 0)
#define PMC_CPUSTOPMEM_MASK(id)			(7 << PMC_CPUSTOPMEM_OFFS(id))
#define PMC_CPUSTOPMEM_EN(id)			(0 << PMC_CPUSTOPMEM_OFFS(id))
#define PMC_CPUSTOPMEM_STOP(id)			(1 << PMC_CPUSTOPMEM_OFFS(id))

#define PMC_IDMASTOPMEM_OFFS			12
#define PMC_IDMASTOPMEM_MASK			(7 << PMC_IDMASTOPMEM_OFFS)
#define PMC_IDMASTOPMEM_EN			(0 << PMC_IDMASTOPMEM_OFFS)
#define PMC_IDMASTOPMEM_STOP			(1 << PMC_IDMASTOPMEM_OFFS)

#define PMC_LCDSTOPMEM_OFFS			0
#define PMC_LCDSTOPMEM_MASK			(7 << PMC_LCDSTOPMEM_OFFS)
#define PMC_LCDSTOPMEM_EN			(0 << PMC_LCDSTOPMEM_OFFS)
#define PMC_LCDSTOPMEM_STOP			(1 << PMC_LCDSTOPMEM_OFFS)

#define PMC_PMUSTOPMEM_OFFS			0
#define PMC_PMUSTOPMEM_MASK			(7 << PMC_PMUSTOPMEM_OFFS)
#define PMC_PMUSTOPMEM_EN			(0 << PMC_PMUSTOPMEM_OFFS)
#define PMC_PMUSTOPMEM_STOP			(1 << PMC_PMUSTOPMEM_OFFS)


/*  Power Management Clock Gating Control Register	*/
#define POWER_MNG_CTRL_REG			0x18220
#define L2C_MTCMOS_CONTROL_0_REG    0x22F00
#define L2C_MTCMOS_CONTROL_1_REG    0x22F04

#define PMU_DFS_CTRL_REG(cpu)			(MV_RUNIT_PMU_REGS_OFFSET + 0x54 + ((cpu) * 0x4))
#define PMU_DFS_CTRL_INIT_RATIO_OFFS	24
#define PMU_DFS_CTRL_INIT_RATIO_MASK	0x3F
#define PMU_DFS_CTRL_RATIO_OFFS			16
#define PMU_DFS_CTRL_RATIO_MASK			0x3F

#define PMC_TDMSTOPCLOCK_OFFS			25
#define PMC_TDMSTOPCLOCK_MASK			(1 << PMC_TDMSTOPCLOCK_OFFS)
#define PMC_TDMSTOPCLOCK_EN			(1 << PMC_TDMSTOPCLOCK_OFFS)
#define PMC_TDMSTOPCLOCK_STOP			(0 << PMC_TDMSTOPCLOCK_OFFS)

#define PMC_PEXSTOPCLOCK_OFFS(port)		((port) < 8 ? (5 + (port)) : (18 + (port)))
#define PMC_PEXSTOPCLOCK_MASK(port)		(1 << PMC_PEXSTOPCLOCK_OFFS(port))
#define PMC_PEXSTOPCLOCK_EN(port)		(1 << PMC_PEXSTOPCLOCK_OFFS(port))
#define PMC_PEXSTOPCLOCK_STOP(port)		(0 << PMC_PEXSTOPCLOCK_OFFS(port))

#define PMC_USBSTOPCLOCK_OFFS(port)		((port) < 3 ? (18 + (port)) : 0)
#define PMC_USBSTOPCLOCK_MASK(port)		(1 << PMC_USBSTOPCLOCK_OFFS(port))
#define PMC_USBSTOPCLOCK_EN(port)		(1 << PMC_USBSTOPCLOCK_OFFS(port))
#define PMC_USBSTOPCLOCK_STOP(port)		(0 << PMC_USBSTOPCLOCK_OFFS(port))

#define PMC_SDIOSTOPCLOCK_OFFS			17
#define PMC_SDIOSTOPCLOCK_MASK			(1 << PMC_SDIOSTOPCLOCK_OFFS)
#define PMC_SDIOSTOPCLOCK_EN			(1 << PMC_SDIOSTOPCLOCK_OFFS)
#define PMC_SDIOSTOPCLOCK_STOP			(0 << PMC_SDIOSTOPCLOCK_OFFS)

#define PMC_RUNITSTOPCLOCK_OFFS			24
#define PMC_RUNITSTOPCLOCK_MASK			(1 << PMC_RUNITSTOPCLOCK_OFFS)
#define PMC_RUNITSTOPCLOCK_EN			(1 << PMC_RUNITSTOPCLOCK_OFFS)
#define PMC_RUNITSTOPCLOCK_STOP			(0 << PMC_RUNITSTOPCLOCK_OFFS)

#define PMC_XORSTOPCLOCK_OFFS			22
#define PMC_XORSTOPCLOCK_MASK			(1 << PMC_XORSTOPCLOCK_OFFS)
#define PMC_XORSTOPCLOCK_EN			(1 << PMC_XORSTOPCLOCK_OFFS)
#define PMC_XORSTOPCLOCK_STOP			(0 << PMC_XORSTOPCLOCK_OFFS)

#define PMC_SATASTOPCLOCK_OFFS(ch)		(ch == 0 ? 14 : 29)
#define PMC_SATASTOPCLOCK_MASK(ch)		(3 << PMC_SATASTOPCLOCK_OFFS(ch))
#define PMC_SATASTOPCLOCK_EN(ch)		(3 << PMC_SATASTOPCLOCK_OFFS(ch))
#define PMC_SATASTOPCLOCK_STOP(ch)		(0 << PMC_SATASTOPCLOCK_OFFS(ch))

#define PMC_CESASTOPCLOCK_OFFS			23
#define PMC_CESASTOPCLOCK_MASK			(1 << PMC_CESASTOPCLOCK_OFFS)
#define PMC_CESASTOPCLOCK_EN			(1 << PMC_CESASTOPCLOCK_OFFS)
#define PMC_CESASTOPCLOCK_STOP			(0 << PMC_CESASTOPCLOCK_OFFS)

#define PMC_GESTOPCLOCK_OFFS(port)		((port) < 4 ? (4 - (port)) : 0)
#define PMC_GESTOPCLOCK_MASK(port)		(1 << PMC_GESTOPCLOCK_OFFS(port))
#define PMC_GESTOPCLOCK_EN(port)		(1 << PMC_GESTOPCLOCK_OFFS(port))
#define PMC_GESTOPCLOCK_STOP(port)		(0 << PMC_GESTOPCLOCK_OFFS(port))

#define PMC_NETASTOPCLOCK_OFFS			13
#define PMC_NETASTOPCLOCK_MASK			(1 << PMC_NETASTOPCLOCK_OFFS)
#define PMC_NETASTOPCLOCK_EN			(1 << PMC_NETASTOPCLOCK_OFFS)
#define PMC_NETASTOPCLOCK_STOP			(0 << PMC_NETASTOPCLOCK_OFFS)

#define PMC_LCDSTOPCLOCK_OFFS			16
#define PMC_LCDSTOPCLOCK_MASK			(1 << PMC_LCDSTOPCLOCK_OFFS)
#define PMC_LCDSTOPCLOCK_EN			(1 << PMC_LCDSTOPCLOCK_OFFS)
#define PMC_LCDSTOPCLOCK_STOP			(0 << PMC_LCDSTOPCLOCK_OFFS)

#define PMC_IDMASTOPCLOCK_OFFS			21
#define PMC_IDMASTOPCLOCK_MASK			(1 << PMC_IDMASTOPCLOCK_OFFS)
#define PMC_IDMASTOPCLOCK_EN			(1 << PMC_IDMASTOPCLOCK_OFFS)
#define PMC_IDMASTOPCLOCK_STOP			(0 << PMC_IDMASTOPCLOCK_OFFS)

#define PMC_DDRSTOPCLOCK_OFFS			28
#define PMC_DDRSTOPCLOCK_MASK			(1 << PMC_DDRSTOPCLOCK_OFFS)
#define PMC_DDRSTOPCLOCK_EN			(1 << PMC_DDRSTOPCLOCK_OFFS)
#define PMC_DDRSTOPCLOCK_STOP			(0 << PMC_DDRSTOPCLOCK_OFFS)

#define SATA_IMP_TX_SSC_CTRL_REG(port)		(0xA2810 + (port)*0x2000)
#define SATA_GEN_1_SET_0_REG(port)			(0xA2834 + (port)*0x2000)
#define SATA_GEN_1_SET_1_REG(port)			(0xA2838 + (port)*0x2000)
#define SATA_GEN_2_SET_0_REG(port)			(0xA283C + (port)*0x2000)
#define SATA_GEN_2_SET_1_REG(port)			(0xA2840 + (port)*0x2000)

#define SATA_PWR_PLL_CTRL_REG(port)			(0xA2804 + (port)*0x2000)
#define SATA_DIG_LP_ENA_REG(port)			(0xA288C + (port)*0x2000)
#define SATA_REF_CLK_SEL_REG(port)			(0xA2918 + (port)*0x2000)
#define SATA_COMPHY_CTRL_REG(port)			(0xA2920 + (port)*0x2000)
#define SATA_LP_PHY_EXT_CTRL_REG(port)		(0xA2058 + (port)*0x2000)
#define SATA_LP_PHY_EXT_STAT_REG(port)		(0xA205C + (port)*0x2000)

#define SGMII_PWR_PLL_CTRL_REG(port)		(0x72E04 + ((port)%2)*0x4000 - ((port)/2)*0x40000)
#define SGMII_DIG_LP_ENA_REG(port)		(0x72E8C + ((port)%2)*0x4000 - ((port)/2)*0x40000)
#define SGMII_REF_CLK_SEL_REG(port)		(0x72F18 + ((port)%2)*0x4000 - ((port)/2)*0x40000)
#define SGMII_SERDES_CFG_REG(port)		(0x724A0 + ((port)%2)*0x4000 - ((port)/2)*0x40000)
#define SGMII_SERDES_STAT_REG(port)		(0x724A4 + ((port)%2)*0x4000 - ((port)/2)*0x40000)
#define SGMII_COMPHY_CTRL_REG(port)		(0x72F20 + ((port)%2)*0x4000 - ((port)/2)*0x40000)
#define QSGMII_GEN_1_SETTING_REG(port)		(0x72E38 + ((port)%2)*0x4000 - ((port)/2)*0x40000)

#define SERDES_LINE_MUX_REG_0_7			0x18270
#define SERDES_LINE_MUX_REG_8_15		0x18274
#define QSGMII_CONTROL_1_REG                    0x18404
/* Controler environment registers offsets */
#define GEN_PURP_RES_1_REG			0x182F4
#define GEN_PURP_RES_2_REG			0x182F8

#define MPP_CONTROL_REG(id)			(0x18000 + (id * 4))

/* Sample at Reset */
#define MPP_SAMPLE_AT_RESET(id)			(0x18230 + (id * 4))

/* SYSRSTn Length Counter */
#define SYSRST_LENGTH_COUNTER_REG		0x18250
#define SLCR_COUNT_OFFS				0
#define SLCR_COUNT_MASK				(0x1FFFFFFF << SLCR_COUNT_OFFS)
#define SLCR_CLR_OFFS				31
#define SLCR_CLR_MASK				(1 << SLCR_CLR_OFFS)

/* Device ID */
#define CHIP_BOND_REG				0x18238
#define PCKG_OPT_MASK				0x3

#define MPP_OUTPUT_DRIVE_REG			0x184E4
#define MPP_GE_A_OUTPUT_DRIVE_OFFS		6
#define MPP_GE_A_1_8_OUTPUT_DRIVE		(0x1 << MPP_GE_A_OUTPUT_DRIVE_OFFS)
#define MPP_GE_A_2_5_OUTPUT_DRIVE		(0x2 << MPP_GE_A_OUTPUT_DRIVE_OFFS)
#define MPP_GE_B_OUTPUT_DRIVE_OFFS		14
#define MPP_GE_B_1_8_OUTPUT_DRIVE		(0x1 << MPP_GE_B_OUTPUT_DRIVE_OFFS)
#define MPP_GE_B_2_5_OUTPUT_DRIVE		(0x2 << MPP_GE_B_OUTPUT_DRIVE_OFFS)

#define MSAR_BOOT_MODE_OFFS			5
#define MSAR_BOOT_MODE_MASK			(0xF << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NOR				(0x0 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_SPI				(0x3 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_DOVE_NAND			(0x1 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_LEGACY_NAND			(0x6 << MSAR_BOOT_MODE_OFFS)

#define MSAR_TCLK_OFFS				28
#define MSAR_TCLK_MASK				(0x1 << MSAR_TCLK_OFFS)

/*****************/
/* PUP registers */
/*****************/
#define PUP_EN_REG				0x1864C

/* Extract CPU, L2, DDR clocks SAR value from
** SAR bits 24-27
*/
#define MSAR_CPU_CLK_IDX(sar0, sar1)		((((sar0) >> 21) & 0x7) + ((((sar1) >> 20) & 1) << 3))
#define MSAR_CPU_CLK_TWSI(sar0, sar1)		((((sar0) >> 2)  & 0x7) + (((sar1) & 1) << 3))
#define MSAR_DDR_L2_CLK_RATIO_IDX(sar0, sar1)	((((sar0) >> 24) & 0xF) + ((((sar1) >> 19) & 1) << 4))
#define MSAR_DDR_L2_CLK_RATIO_TWSI(sar0)	(((sar0) >> 1)  & 0xF)

#ifndef MV_ASMLANGUAGE

#define MV_CPU_CLK_TBL { 1000, 1066, 1200, 1333, 1500, 1666, 1800, 2000,\
			  600,  667,  800, 1600, 2133, 2200, 2400, 0 }

/*		cpu	l2c	hclk	ddr	*/
#define MV_DDR_L2_CLK_RATIO_TBL    { \
/*00*/	{	1,	1,	4,	2	},\
/*01*/	{	1,	2,	2,	2	},\
/*02*/	{	2,	2,	6,	3	},\
/*03*/	{	2,	2,	3,	3	},\
/*04*/	{	1,	2,	3,	3	},\
/*05*/	{	1,	2,	4,	2	},\
/*06*/	{	1,	1,	2,	2	},\
/*07*/	{	2,	3,	6,	6	},\
/*08*/	{	2,	3,	5,	5	},\
/*09*/	{	1,	2,	6,	3	},\
/*10*/	{	2,	4,	10,	5	},\
/*11*/	{	1,	3,	6,	6	},\
/*12*/	{	1,	2,	4,	4	},\
/*13*/	{	1,	3,	6,	3	},\
/*14*/	{	1,	2,	5,	5	},\
/*15*/	{	2,	2,	5,	5	},\
/*16*/	{	1,	1,	3,	3	},\
/*17*/	{	2,	5,	10,	10	},\
/*18*/	{	1,	3,	8,	4	},\
/*19*/	{	1,	1,	2,	1	},\
/*20*/	{	2,	3,	6,	3	},\
/*21*/	{	1,	2,	8,	4	},\
/*22*/	{	2,	5,	10,	5	} \
}

/* These macros help units to identify a target Mport Arbiter group */
#define MV_TARGET_IS_DRAM(target)   \
		((target >= SDRAM_CS0) && (target <= SDRAM_CS3))

#define MV_TARGET_IS_PEX0(target)   \
		((target >= PEX0_MEM) && (target <= PEX0_IO))
#define MV_TARGET_IS_PEX1(target)   \
		((target >= PEX1_MEM) && (target <= PEX1_IO))
#define MV_TARGET_IS_PEX2(target)   \
		((target >= PEX2_MEM) && (target <= PEX2_IO))
#define MV_TARGET_IS_PEX3(target)   \
		((target >= PEX3_MEM) && (target <= PEX3_IO))
#define MV_TARGET_IS_PEX4(target)   \
		((target >= PEX4_MEM) && (target <= PEX4_IO))
#define MV_TARGET_IS_PEX5(target)   \
		((target >= PEX5_MEM) && (target <= PEX5_IO))
#define MV_TARGET_IS_PEX6(target)   \
		((target >= PEX6_MEM) && (target <= PEX6_IO))
#define MV_TARGET_IS_PEX7(target)   \
		((target >= PEX7_MEM) && (target <= PEX7_IO))
#define MV_TARGET_IS_PEX8(target)   \
		((target >= PEX8_MEM) && (target <= PEX8_IO))
#define MV_TARGET_IS_PEX9(target)   \
		((target >= PEX9_MEM) && (target <= PEX9_IO))

#define MV_TARGET_IS_PEX(target)	((target >= PEX0_MEM) && (target <= PEX9_IO))

#define MV_TARGET_IS_DEVICE(target)	((target >= DEVICE_CS0) && (target <= DEVICE_CS3))

#define MV_PCI_DRAM_BAR_TO_DRAM_TARGET(bar)   0

#define MV_CHANGE_BOOT_CS(target) target

#define TCLK_TO_COUNTER_RATIO   1   /* counters running in Tclk */


#define BOOT_TARGETS_NAME_ARRAY {	\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	TBL_TERM, 			\
	TBL_TERM, 			\
	TBL_TERM,			\
	TBL_TERM,			\
	TBL_TERM,			\
	TBL_TERM,			\
	TBL_TERM,			\
	BOOT_ROM_CS			\
}

#define START_DEV_CS   		DEV_CS0
#define DEV_TO_TARGET(dev)	((dev) + START_DEV_CS)

#define PCI_IF0_MEM0		PEX0_MEM
#define PCI_IF0_IO		PEX0_IO

/* This enumerator defines the Marvell controller target ID  (see Address map) */
typedef enum _mvTargetId {
    DRAM_TARGET_ID	= 0,	/* Port 0 -> DRAM interface		*/
    DEV_TARGET_ID	= 1,	/* Port 1 -> Device port, BootROM, SPI	*/
    PEX0_2_TARGET_ID	= 4,	/* Port 4 -> PCI Express 0 and 2	*/
    PEX1_3_TARGET_ID	= 8,	/* Port 4 -> PCI Express 1 and 3	*/
    CRYPT_TARGET_ID	= 9,	/* Port 9 --> Crypto Engine SRAM	*/
    PNC_BM_TARGET_ID	= 12,  	/* Port 12 -> PNC + BM Unit		*/
    MAX_TARGETS_ID
} MV_TARGET_ID;

/*
typedef enum {
	SERDES_UNIT_UNCONNECTED	= 0x0,
	SERDES_UNIT_PEX		= 0x1,
	SERDES_UNIT_SATA	= 0x2,
	SERDES_UNIT_SGMII0	= 0x3,
	SERDES_UNIT_SGMII1	= 0x4,
	SERDES_UNIT_SGMII2	= 0x5,
	SERDES_UNIT_SGMII3	= 0x6,
	SERDES_UNIT_QSGMII	= 0x7
} MV_SERDES_UNIT_INDX;
*/

/*
	This structure refrect registers:
	Serdes 0-7 selectors		0x18270
	and Serdes 8-15 selectors  	0x18274
*/

#define SERDES_CFG {	\
	{0, 1, -1 , -1, -1, -1, -1, -1}, /* Lane 0 */	\
	{0, 1, -1 , -1, -1, -1, -1, -1}, /* Lane 1 */	\
	{0, 1, -1 ,  2, -1, -1, -1, -1}, /* Lane 2 */	\
	{0, 1, -1 , -1,  2, -1, -1,  3}, /* Lane 3 */	\
	{0, 1,  2 , -1, -1,  3, -1, -1}, /* Lane 4 */	\
	{0, 1,  2 , -1,  3, -1, -1,  4}, /* Lane 5 */	\
	{0, 1,  2 ,  4, -1,  3, -1, -1}, /* Lane 6 */	\
	{0, 1, -1 ,  2, -1, -1,  3, -1}, /* Lane 7*/	\
	{0, 1, -1 , -1, -1, -1, -1, -1}, /* Lane 8 */	\
	{0, 1, -1 , -1, -1, -1, -1, -1}, /* Lane 9 */	\
	{0, 1, -1 , -1, -1, -1, -1, -1}, /* Lane 10 */	\
	{0, 1, -1 , -1, -1, -1, -1, -1}, /* Lane 11 */	\
	{0, 1, -1 , -1, -1, -1, -1, -1}, /* Lane 12 */	\
	{0, 1, -1 , -1, -1, -1, -1, -1}, /* Lane 13 */	\
	{0, 1, -1 , -1, -1, -1, -1, -1}, /* Lane 14 */	\
	{0, 1, -1 , -1, -1, -1, -1, -1}	 /* Lane 15 */	\
}


/*
	This enum should reflect the units numbers in register
	space which we will need when accessing the HW
*/

typedef enum {
	PEX0_0x4	= 0,
	PEX0_1x4	= 1,
	PEX0_2x4	= 2,
	PEX0_3x4	= 3,
	PEX1_0x4	= 4,
	PEX1_1x4	= 5,
	PEX1_2x4	= 6,
	PEX1_3x4	= 7,
	PEX2_0x4	= 8,
	PEX3_0x4	= 9,
	PEXIF_MAX	= 10
} MV_PEXIF_INDX;

#endif /* MV_ASMLANGUAGE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
