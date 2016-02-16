/*
* ***************************************************************************
* Copyright (C) 2015 Marvell International Ltd.
* ***************************************************************************
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 2 of the License, or any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ***************************************************************************
*/
/* pcie-core.h
 *
 * Definition for PCIe core unit
 *
 * Author: Victor Gu <xigu@marvell.com>
 *
 * March 6, 2015
 *
 */

#ifndef _PCIE_CORE_H_
#define _PCIE_CORE_H_

#include <asm/arch/memory-map.h>
#include <asm/arch-mvebu/mvebu.h>
#include <pci.h>

/* ArLP RC has only 1 port */
#define PCIE_ADVK_MAX_PORT	(1)

/*
 * Make it 1 because we fake up internal bus 0.
 * We fake up internal bus 0 because we assume root port 0
 * is on bus 0. In that case, if we send a config TLP to
 * Endpoint device on bus 1, it must be a Type 1 TLP.
 * However, it's not. Because we only have one link, without
 * any internal bus.
 * If we don't fake internal bus, then just leave it 0.
 */
#define PCIE_ADVK_ACTUAL_FIRST_BUSNO	1

#define SIZE_8BIT	1
#define SIZE_16BIT	2
#define SIZE_32BIT	4

#define LOCAL_VENDOR_ID		0x11AB
#define LOCAL_DEVICE_ID		0x0
#define LOCAL_SUBVENDOR_ID	0x11AB
#define LOCAL_SUBDEVICE_ID	0x0
#define LOCAL_CLASS_CODE	0x2
#define LOCAL_SUBCLASS_CODE	0x0
#define LOCAL_REVISION_ID	0x0
#define LOCAL_PROGIF_CODE	0x0

#define BAR_PCIE_MEM		0x0
#define BAR_PCIE_REG		0x4

/*
 * Used in PIO read/write
 * Should choose this value more carefully
 */
#define PCIE_CORE_PIO_TIMEOUT_NUM	1000
#define PCIE_CORE_PIO_ADDR_MASK	0xfffffffc
/* This defines the size of VPD RAM */
#define PCIE_CORE_VPD_RAM_SIZE	0x400 /*1K*/

/* Time out number to get PCIe link up */
#define PCIE_LINK_TIMEOUT_NUM	1000

/* Transaction types */
#define PCIE_MEM_RD		0x0
#define PCIE_MEM_WR		0x2
#define PCIE_IO_RD		0x4
#define PCIE_IO_WR		0x6
#define PCIE_CONFIG_RD_TYPE0	0x8
#define PCIE_CONFIG_RD_TYPE1	0x9
#define PCIE_CONFIG_WR_TYPE0	0xa
#define PCIE_CONFIG_WR_TYPE1	0xb
#define PCIE_MSG_REQ		0xc

/* Message codes
 * virtual legacy interrupt messages
 */
#define MSG_ASSERT_INTA		0x20
#define MSG_ASSERT_INTB		0x21
#define MSG_ASSERT_INTC		0x22
#define MSG_ASSERT_INTD		0x23
#define MSG_DEASSERT_INTA	0x24
#define MSG_DEASSERT_INTB	0x25
#define MSG_DEASSERT_INTC	0x26
#define MSG_DEASSERT_INTD	0x27

/* Power related messages */
#define MSG_PM_ACTIVE_STATE_NAK	0x14
#define MSG_PM_PME		0x18
#define MSG_PM_TURN_OFF		0x19
#define MSG_PM_TO_ACK		0x1b

/* Error messages */
#define MSG_ERR_COR		0x30
#define MSG_ERR_NONFATAL	0x31
#define MSG_ERR_FATAL		0x33

/* Unlock message */
#define MSG_UNLOCK		0x00

/* Slot power limit set message */
#define MSG_SET_SLOT_POWER_LIMIT	0x50

/* Hot-Plug related messages */
#define MSG_ATT_INDICATOR_ON		0x41
#define MSG_ATT_INDICATOR_BLINK		0x43
#define MSG_ATT_INDICATOR_OFF		0x40
#define MSG_POWER_INDICATOR_ON		0x45
#define MSG_POWER_INDICATOR_BLINK	0x47
#define MSG_POWER_INDICATOR_OFF		0x44
#define MSG_ATT_BUTTON_PRESSED		0x48

#define PCIE_CORE_DEV_CTRL_STATS_REG	0xC8
#define PCIE_CORE_LINK_CTRL_STAT_REG	0xD0
#define PCIE_CORE_LINK_TRAINING_SHIFT	5
#define PCIE_CORE_LINK_SPEED_SHIFT	16
#define PCIE_CORE_LINK_SPEED_MASK	0xF
#define PCIE_CORE_LINK_WIDTH_SHIFT	20
#define PCIE_CORE_LINK_WIDTH_MASK	0x3F
#define PCIE_CORE_LINK_DLL_STATE_SHIFT	29
#define PCIE_CORE_LINK_DLL_STATE_MASK	0x1

#define PCIE_DLL_STATE_ACTIVE	1
#define PCIE_DLL_STATE_INACTIVE	0

#define PCIE_CORE_ERR_CAP_CTRL_REG	0x118

/* Shift */
#define PCIE_CORE_PIO_CTRL_BYTE_COUNT_SHIFT	16

/* Aardvark PIO registers */
#define PIO_BASE_ADDR			0x4000
#define PCIE_CORE_PIO_CTRL		0x0
#define PCIE_CORE_PIO_STAT		0x4
#define PCIE_CORE_PIO_ADDR_LS		0x8
#define PCIE_CORE_PIO_ADDR_MS		0xc
#define PCIE_CORE_PIO_WR_DATA		0x10
#define PCIE_CORE_PIO_WR_DATA_STRB	0x14
#define PCIE_CORE_PIO_RD_DATA		0x18
#define PCIE_CORE_PIO_START		0x1c
#define PCIE_CORE_PIO_ISR		0x20
#define PCIE_CORE_PIO_ISRM		0x24

/* Aardvark PCIe DPMU registers */
#define DPMU_BASE_ADDR			0x4400
#define PCIE_CORE_DPMU_CTRL0		0x0

/* Aardvark Control registers */
#define CONTROL_BASE_ADDR	0x4800
#define PCIE_CORE_CTRL0_REG	0x0
#define PCIE_GEN_SEL_MSK	0x3
#define PCIE_GEN_SEL_SHIFT	0x0
#define SPEED_GEN_1		0
#define SPEED_GEN_2		1
#define SPEED_GEN_3		2
#define IS_RC_MSK		0x4
#define IS_RC_SHIFT		2
#define LANE_CNT_MSK		0x18
#define LANE_CNT_SHIFT		0x3
#define LANE_COUNT_1		(0 << LANE_CNT_SHIFT)
#define LANE_COUNT_2		(1 << LANE_CNT_SHIFT)
#define LANE_COUNT_4		(2 << LANE_CNT_SHIFT)
#define LANE_COUNT_8		(3 << LANE_CNT_SHIFT)
#define LINK_TRAINNING_EN	BIT6
#define CORE_RESET_MSK		BIT7
#define MGMT_RESET_MSK		BIT8
#define MGMT_STICKY_RESET_MSK	BIT9
#define APP_RESET_MSK		BIT16
#define LEGACY_INTA		BIT28
#define LEGACY_INTB		BIT29
#define LEGACY_INTC		BIT30
#define LEGACY_INTD		BIT31
#define PCIE_CORE_CTRL1_REG	0x4
#define HOT_RESET_GEN		BIT0
#define PCIE_CORE_CTRL2_REG	0x8
#define STRICT_ORDER_ENABLE	BIT5
#define OB_WIN_ENABLE		BIT6
#define MSI_ENABLE		BIT10
#define PCIE_CORE_FLUSH_REG	0xC
#define PCIE_CORE_FLUSH_TIMER_MASK	0xFFFF
#define PCIE_CORE_FORCE_FLUSH	BIT16
#define PCIE_CORE_FLUSH_ACTIVE	BIT17
#define PCIE_CORE_FLUSH_CLEAR	BIT18
#define PCIE_CORE_PWR_CTRL_REG	0x10
#define REQ_PM_TRANSITION_L23	(1 << 0)
#define POWER_STATE_CHANGE_ACK	(1 << 1)
#define PCIE_CORE_PHY_REF_CLK_REG	0x14
#define PCIE_CORE_EN_TX		(1 << 1)
#define PCIE_CORE_EN_RX		(1 << 2)
#define PCIE_CORE_SEL_AMP_MASK	0x7
#define PCIE_CORE_SEL_AMP_SHIFT	0x8
#define PCIE_CORE_EN_PU		(1 << 12)
#define PCIE_CORE_MSG_LOG0		0x30
#define PCIE_CORE_MSG_LOG0_MSK		0xFF
#define PCIE_CORE_ISR0			0x40
#define PCIE_CORE_ISR0_HOT_RESET		BIT0
#define PCIE_CORE_ISR0_LINK_DOWN		BIT1
#define PCIE_CORE_ISR0_INB_MSG			BIT2
#define PCIE_CORE_ISR0_DP_PERR			BIT3
#define PCIE_CORE_ISR0_DP_FERR			BIT4
#define PCIE_CORE_ISR0_MSG_PME2ACK		BIT5
#define PCIE_CORE_ISR0_MSG_PM_TURN_OFF		BIT6
#define PCIE_CORE_ISR0_MSG_PM_PME		BIT7
#define PCIE_CORE_ISR0_MSG_PM_ACTIVE_NAK	BIT8
#define PCIE_CORE_ISR0_LEGACY_INT_SENT		BIT9
#define PCIE_CORE_ISR0_LEGACY_INTA_ASSERT	BIT16
#define PCIE_CORE_ISR0_LEGACY_INTB_ASSERT	BIT17
#define PCIE_CORE_ISR0_LEGACY_INTC_ASSERT	BIT18
#define PCIE_CORE_ISR0_LEGACY_INTD_ASSERT	BIT19
#define PCIE_CORE_ISR0_LEGACY_INTA_DEASSERT	BIT20
#define PCIE_CORE_ISR0_LEGACY_INTB_DEASSERT	BIT21
#define PCIE_CORE_ISR0_LEGACY_INTC_DEASSERT	BIT22
#define PCIE_CORE_ISR0_LEGACY_INTD_DEASSERT	BIT23
#define PCIE_CORE_ISRM0				0x44
#define PCIE_CORE_ISR1				0x48
#define PCIE_CORE_ISR1_PWR_CHANGE_STATE		BIT4
#define PCIE_CORE_ISR1_FLUSH_OFF		BIT5
#define PCIE_CORE_ISRM1				0x4c
#define PCIE_CORE_MSI_MSG_LOW_ADDR		0x50
#define PCIE_CORE_MSI_MSG_HIGH_ADDR		0x54
#define PCIE_CORE_MSI_MSG_ISR_STAT		0x58
#define PCIE_CORE_MSI_MSG_ISR_MASK		0x5c
#define PCIE_CORE_DP_PAR_CNTL	0x60
#define PCIE_CORE_DP_PAR_STAT0	0x64
#define PCIE_CORE_DP_PAR_STAT1	0x68

/* Victor: below PF/VF register do not exist in ArLP */
#define PCIE_CORE_PF_FLR_CTRL	0x80
#define PCIE_CORE_PF_FLR_STAT	0x84
#define PCIE_CORE_VF_FLR_CTRL	0x88
#define PCIE_CORE_VF_FLR_TRL	0x8c

#define PCIE_CORE_MSI_PAYLOAD	0x9c

#define OB_WIN_BASE_ADDR	0x4c00
#define OB_WIN_MATCH_LS		0x00
#define OB_WIN_MATCH_MS		0x04
#define OB_WIN_REMAP_LS		0x08
#define OB_WIN_REMAP_MS		0x0c
#define OB_WIN_MASK_LS		0x10
#define OB_WIN_MASK_MS		0x14
#define OB_WIN_ACTIONS		0x18

#define OB_WIN_BLOCK_SIZE	0x20

#define OB_Default_ACTIONS	0xfc

#define OB_PCIE_MEM		0x0
#define OB_PCIE_IO		0x4
#define OB_PCIE_CONFIG0		0x8
#define OB_PCIE_CONFIG1		0x9
#define OB_PCIE_MSG		0xc
#define OB_PCIE_MSG_VENDOR	0xd

#define OB_ATTR_SEL		BIT11
#define OB_FUNC_SEL		BIT23

/* PCIe address translation registers */
#define ADDR_TRANSLATION_ADDR	0x5800
#define AT_PF0_BAR0		0x0
#define BAR_TYPE_32b		0
#define BAR_TYPE_64b		0x4
#define BAR_ADDR_MASK		(~0x7F)
#define AT_PF0_BAR1		0x4
#define AT_PF0_BAR2		0x8
#define AT_PF0_BAR3		0xc
#define AT_PF0_BAR4		0x10
#define AT_PF0_BAR5		0x14
#define MSG_BAR			0x100
#define EXT_ROM_BAR		0x120
#define AT_ISEL_PF0_BAR0	0x200
#define AT_ISEL_PF0_BAR1	0x204
#define AT_ISEL_PF0_BAR2	0x208
#define AT_ISEL_PF0_BAR3	0x20c
#define AT_ISEL_PF0_BAR4	0x210
#define AT_ISEL_PF0_BAR5	0x214

#define LMI_BASE_ADDR		0x6000
#define PHY_CONF_REG0		0x0
#define PHY_ERR_REPORT		BIT6
#define LTSSM_STATE_SHIFT	24
#define LTSSM_STATE_MASK	0x3f000000
#define LTSSM_STATE_L0		0x10
#define PHY_CONF_REG1		0x4
#define DLL_TIMER_CONF		0x8
#define REPLAY_TIMEOUT_MASK	0xffff
#define REPLAY_TIMEOUT_SHIFT	0
#define ACK_TIMEOUT_MASK	0xffff0000
#define ACK_TIMEOUT_SHIFT	16
#define RCV_CREDIT_LT_REG0	0xc
#define RCV_CREDIT_LT_REG1	0x10
#define TRANSMIT_CREDIT_LT_REG0	0x14
#define TRANSMIT_CREDIT_LT_REG1	0x18
#define TRANSMIT_CREDIT_UPDATE_REG0 0x1c
#define TRANSMIT_CREDIT_UPDATE_REG1 0x20
#define L0S_TIMEOUT_REG		0x24
#define L0S_TIMEOUT_SHIFT	0
#define L0S_TIMEOUT_MASK	0xffff
#define L2_ENTRY_ENABLE		BIT16
#define L1_REENTRY_DELAY	0x40
#define LOCAL_VENDOR_ID_REG	0x44
#define ASPM_L1_TIMEOUT_REG	0x48
#define ASPM_L1_TIMEOUT_MASK	0xfffff
#define LOCAL_ERR_STAT_REG	0x20c
#define PNP_RX_FIFO_PAR		BIT0
#define CPL_RX_FIFO_PAR		BIT1
#define REPLAY_BUF_PAR		BIT2
#define PNP_RX_FIFO_OVERFLOW	BIT3
#define CPL_RX_FIFO_OVERFLOW	BIT4
#define REPLAY_TIMEOUT		BIT5
#define REPLAY_TIMER_ROLLOVER	BIT6
#define PHY_ERR			BIT7
#define MALFORMED_TLP		BIT8
#define UNEXPECTED_CPL		BIT9
#define FLOW_CONTROL_ERR	BIT10
#define CPL_TIMEOUT		BIT11
#define LOCAL_ERR_INT_MASK_REG	0x210
#define LCRC_ERR_COUNT_REG	0x214
#define LCRC_COUNT_MASK		0xffff
#define ECC_COR_ERR_COUNT_REG	0x218
#define PNP_RX_FIFO_MASK	0xff
#define PNP_RX_FIFO_SHIFT	0
#define CPL_RX_FIFO_MASK	0xff00
#define CPL_RX_FIFO_SHIFT	8
#define REPLAY_RAM_MASK		0xff0000
#define REPLAY_RAM_SHIFT	16
#define PHY_FUNC_0_BAR_CONF_REG	0x240
#define PHY_FUNC_1_BAR_CONF_REG	0x244
#define BAR_0_1_APERTURE_SHIFT	0
#define BAR_2_3_APERTURE_SHIFT	7
#define BAR_4_5_APERTURE_SHIFT	19
#define BAR_ROM_APERTURE_SHIFT	26
#define BAR_0_1_ENABLE		(1 << 5)
#define BAR_2_3_ENABLE		(1 << 12)
#define BAR_4_5_ENABLE		(1 << 24)
#define BAR_ROM_ENABLE		(1 << 31)
#define BAR_0_1_PREFETCHABLE	(1 << 6)
#define BAR_2_3_PREFETCHABLE	(1 << 13)
#define BAR_4_5_PREFETCHABLE	(1 << 25)

#define PHY_BASE_ADDR		0x8000		/* update for Sycamore */

/* For new PIO */
#define PCIE_BDF(dev)            (dev << 4)	/* PCI_BDF shifts 8bit, so we need extra 4bit shift */
#define PCIE_BUS(bus)            ((bus & 0xFF) << 20)
#define PCIE_DEV(dev)            ((dev & 0x1F) << 15)
#define PCIE_FUNC(fun)           ((fun & 0x7)  << 12)
#define PCIE_REG_ADDR(reg)       ((reg & 0xFFC) << 0)

/* PCIe core controller registers */
#define PCIE_CTRL_CORE_BASE_ADDR	0x18000
#define PCIE_CTRL_CONFIG_REG		0x0
#define PCIE_CTRL_MODE_SHIFT		0x0
#define PCIE_CTRL_MODE_MASK		0x1

#define PCIE_CORE_MODE_EP		0x0
#define PCIE_CORE_MODE_RC		0x1

/* PCIe interrupt registers */
#define PCIE_CTRL_INT_BASE_ADDR		0x1B000
#define RC_INT_STATS_REG		0x0
#define RC_INT_MASK_REG			0x4

/*
 * Get the register address of a given function's configuration space(each funciton has an address space of 0x1000)
 * baseaddr is the base address of a PCIe device in Aardvark
 */
#define PCIE_CORE_CONFIG_REG_ADDR(baseaddr, offset) ((u64)baseaddr + offset)

/* Get the PIO registers addresses of a PCIE device, 0x4000 is the offset of PIO register block */
#define PCIE_CORE_PIO_REG_ADDR(baseaddr, offset) ((u64)baseaddr + PIO_BASE_ADDR + offset)

/* Get the Control registers addresses of a PCIE device, 0x4800 is the offset of control register block*/
#define PCIE_CORE_CTRL_REG_ADDR(baseaddr, offset) ((u64)baseaddr + CONTROL_BASE_ADDR + offset)

/* Get the Outbound Address Translation window register address, 0x4c00 is the offset of ob window register block */
#define PCIE_CORE_OB_REG_ADDR(baseaddr, offset, win) ((u64)baseaddr + OB_WIN_BASE_ADDR + win*OB_WIN_BLOCK_SIZE + offset)

/* Get the DPMU registers addresses of a PCIE device, 0x4400 is the offset of control register block */
#define PCIE_CORE_DPMU_REG_ADDR(baseaddr, offset) ((u64)baseaddr + DPMU_BASE_ADDR + offset)

/* Get the Address Translation registers address of a PCIE device's , 0x5800 is the offset of control register block */
#define PCIE_CORE_AT_REG_ADDR(baseaddr, offset) ((u64)baseaddr + ADDR_TRANSLATION_ADDR + offset)

/* Get the VPD addresses of a PCIe device, 0x5000 is the offset of VPD register block */
#define PCIE_CORE_VPD_RAM_ADDR(baseaddr, vfnum, offset) ((u64)baseaddr + VPD_RAM_BASE_ADDR + offset)

/* Get the LMI register address of a PCIE device, 0x6000 is the offset of control register block */
#define PCIE_CORE_LMI_REG_ADDR(baseaddr, offset) ((u64)baseaddr + LMI_BASE_ADDR + offset)

/* Get PCIe controller core configuration addresses of a PCIe device, 0x180000 is the offset of controller core block */
#define PCIE_CTRL_CORE_REG_ADDR(baseaddr, offset) ((u64)baseaddr + PCIE_CTRL_CORE_BASE_ADDR + offset)

#endif /* _PCIE_CORE_H_ */

