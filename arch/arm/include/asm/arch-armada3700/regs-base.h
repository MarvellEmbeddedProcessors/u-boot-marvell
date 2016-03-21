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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ***************************************************************************
*/

#ifndef _REGS_BASE_H_
#define _REGS_BASE_H_

#include <asm/arch/memory-map.h>

#define MPP_REGS_BASE		(MVEBU_REGS_BASE + 0x18000)
#define MVEBU_ADEC_BASE		(MVEBU_REGS_BASE + 0xCF00)

#define MVEBU_MISC_REGS_BASE	(MVEBU_REGS_BASE + 0x18200)
#define MVEBU_DEVICE_ID_REG	(MVEBU_MISC_REGS_BASE + 0x38)
#define MVEBU_DEVICE_REV_REG	(MVEBU_MISC_REGS_BASE + 0x3C)
#define MVEBU_RESET_MASK_REG	(MVEBU_MISC_REGS_BASE + 0x60)
#define MVEBU_SOFT_RESET_REG	(MVEBU_MISC_REGS_BASE + 0x64)

#define MVEBU_GIC_BASE (MVEBU_REGS_BASE + 0x1d00000)
#define MVEBU_GICD_BASE		(MVEBU_GIC_BASE)
#define MVEBU_GICR_BASE		(MVEBU_GIC_BASE + 0x40000)
#define MVEBU_GICC_BASE		(MVEBU_GIC_BASE + 0x80000)

/* GBE register base address */
#define MVEBU_A3700_GBE0_INTERNAL_REG_BASE	MVEBU_REGS_BASE + 0xc364
#define MVEBU_A3700_GBE1_INTERNAL_REG_BASE	MVEBU_REGS_BASE + 0xc464

/* CCI-400 */
#define MVEBU_CCI_BASE			(MVEBU_REGS_BASE + 0x8000000)
#define MVEBU_CCI_S3_SNOOP_CTRL_REG	(MVEBU_CCI_BASE + 0x4000)

/* GPIO register base address */
#define MVEBU_GPIO_NB_REG_BASE	(MVEBU_REGS_BASE + 0x13800)
#define MVEBU_GPIO_SB_REG_BASE	(MVEBU_REGS_BASE + 0x18800)

#define MVEBU_A3700_NB_TEST_PIN_OUTPUT_EN (MVEBU_REGS_BASE + 0x13804)
#define MVEBU_A3700_NB_TEST_PIN_OUTPUT_SPI_EN_OFF (28)

/* Clocks */
#define MVEBU_NORTH_CLOCK_REGS_BASE	(MVEBU_REGS_BASE + 0x13000)
#define MVEBU_SOUTH_CLOCK_REGS_BASE	(MVEBU_REGS_BASE + 0x18000)
#define MVEBU_TESTPIN_NORTH_REG_BASE	(MVEBU_NORTH_CLOCK_REGS_BASE + 0x800)

/* Reset sample */
#define MVEBU_TEST_PIN_LATCH_N		(MVEBU_TESTPIN_NORTH_REG_BASE + 0x8)

/* PM */
#define MVEBU_NORTH_PM_REGS_BASE	(MVEBU_REGS_BASE + 0x14000)
#define MVEBU_SOUTH_PM_REGS_BASE	(MVEBU_REGS_BASE + 0x19000)

/* AXI DCTRL */
#define MVEBU_AXI_DCTRL_REGS_BASE	(MVEBU_REGS_BASE + 0x2000)

/* AVS */
#define MVEBU_AVS_REGS_BASE		(MVEBU_REGS_BASE + 0x11500)

/* Mailbox */
#define MVEBU_MBOX_REGS_BASE		(MVEBU_REGS_BASE + 0xB0000)

#endif	/* _REGS_BASE_H_ */
