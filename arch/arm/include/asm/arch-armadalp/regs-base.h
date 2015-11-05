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
#define MVEBU_ARLP_GBE0_INTERNAL_REG_BASE	MVEBU_REGS_BASE + 0xc364
#define MVEBU_ARLP_GBE1_INTERNAL_REG_BASE	MVEBU_REGS_BASE + 0xc464

/* CCI-400 */
#define MVEBU_CCI_BASE			(MVEBU_REGS_BASE + 0x8000000)
#define MVEBU_CCI_S3_SNOOP_CTRL_REG	(MVEBU_CCI_BASE + 0x4000)

/* GPIO register base address */
#define MVEBU_A3700_GPIO_SB_SEL (MVEBU_REGS_BASE + 0x18830)
#define MVEBU_A3700_GPIO_RGMII_GPIO_EN_OFF (3)

#define MVEBU_A3700_GPIO_NB_SEL (MVEBU_REGS_BASE + 0x13830)
#define MVEBU_A3700_GPIO_TW1_GPIO_EN_OFF (10)

#endif	/* _REGS_BASE_H_ */
