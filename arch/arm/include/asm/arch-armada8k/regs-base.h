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

/* Base registers for AP */
#define MPP_REGS_BASE	(MVEBU_REGS_BASE + 0x6F008C)

/* List of register base for all units */
#define MVEBU_ADEC_AP_BASE 	(MVEBU_REGS_BASE + 0x0)
#define MVEBU_ADEC_RFU_BASE 	(MVEBU_REGS_BASE + 0x680000)
#define MVEBU_ADEC_IOB_BASE 	(MVEBU_REGS_BASE + 0x2810000)

#define MVEBU_RFU_BASE		(MVEBU_REGS_BASE + 0x6F0000)
#define MVEBU_UART_BASE(x)	(MVEBU_REGS_BASE + 0x512000 + (0x100 * x))
#define MVEBU_MISC_REGS_BASE	(MVEBU_REGS_BASE + 0x18200)
#define MVEBU_DEVICE_ID_REG	(MVEBU_MISC_REGS_BASE + 0x38)
#define MVEBU_DEVICE_REV_REG	(MVEBU_MISC_REGS_BASE + 0x3C)
#define MVEBU_RESET_MASK_REG	(MVEBU_MISC_REGS_BASE + 0x60)
#define MVEBU_SOFT_RESET_REG	(MVEBU_MISC_REGS_BASE + 0x64)

#define MVEBU_GICD_BASE		(MVEBU_REGS_BASE + 0x210000)
#define MVEBU_GICC_BASE		(MVEBU_REGS_BASE + 0x220000)

#define MVEBU_GENERIC_TIMER_BASE	(MVEBU_REGS_BASE + 0x581000)

#define MVEBU_LLC_BASE			(MVEBU_REGS_BASE + 0x8000)

#define MVEBU_IP_CONFIG_REG		(MVEBU_REGS_BASE + 0x6F4100)

#endif	/* _REGS_BASE_H_ */
