/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#ifndef _REGS_BASE_H_
#define _REGS_BASE_H_

#include <asm/arch/memory-map.h>

#define MPP_REGS_BASE		(MVEBU_REGS_BASE + 0x18000)
#define MVEBU_ADEC_BASE		(MVEBU_REGS_BASE + 0x20000)

#define MVEBU_UART_BASE(x)	(MVEBU_REGS_BASE + 0x12000 + (0x100 * x))
#define MVEBU_GLOBAL_TIMER_BASE	(MVEBU_REGS_BASE + 0x20300)
#define MVEBU_SPI_REGS_BASE(x)	(MVEBU_REGS_BASE + 0x10600 + (x * 0x80))
#define MVEBU_PCIE_BASE(x)	((x == 0) ? (MVEBU_REGS_BASE + 0x80000) : \
				(MVEBU_REGS_BASE + 0x40000 + (0x4000 * (x - 1))))
#define MVEBU_I2C_BASE(x)	(MVEBU_REGS_BASE + 0x11000 + (0x100 * x))
#define MVEBU_NAND_REGS_BASE	(MVEBU_REGS_BASE + 0xD0000)
#define MVEBU_MISC_REGS_BASE	(MVEBU_REGS_BASE + 0x18200)
#define MVEBU_DEVICE_ID_REG	(MVEBU_MISC_REGS_BASE + 0x38)
#define MVEBU_DEVICE_REV_REG	(MVEBU_MISC_REGS_BASE + 0x3C)
#define MVEBU_RESET_MASK_REG	(MVEBU_MISC_REGS_BASE + 0x60)
#define MVEBU_SOFT_RESET_REG	(MVEBU_MISC_REGS_BASE + 0x64)

#endif	/* _REGS_BASE_H_ */
