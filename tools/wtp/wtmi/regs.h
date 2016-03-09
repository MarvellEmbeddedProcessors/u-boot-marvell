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

#ifndef __REGS_H
#define __REGS_H

#define MVEBU_REGS_BASE			(0xC0000000)

/* Clocks */
#define MVEBU_NORTH_CLOCK_REGS_BASE	(MVEBU_REGS_BASE + 0x13000)
#define MVEBU_SOUTH_CLOCK_REGS_BASE	(MVEBU_REGS_BASE + 0x18000)
#define MVEBU_TESTPIN_NORTH_REG_BASE	(MVEBU_NORTH_CLOCK_REGS_BASE + 0x800)


/****************/
/* North Bridge */
/****************/
#define MVEBU_NORTH_BRG_PLL_BASE		(MVEBU_NORTH_CLOCK_REGS_BASE + 0x200)
#define MVEBU_NORTH_BRG_TBG_CFG			(MVEBU_NORTH_BRG_PLL_BASE + 0x0)
#define MVEBU_NORTH_BRG_TBG_CTRL0		(MVEBU_NORTH_BRG_PLL_BASE + 0x4)
#define MVEBU_NORTH_BRG_TBG_CTRL1		(MVEBU_NORTH_BRG_PLL_BASE + 0x8)
#define MVEBU_NORTH_BRG_TBG_CTRL2		(MVEBU_NORTH_BRG_PLL_BASE + 0xC)
#define MVEBU_NORTH_BRG_TBG_CTRL3		(MVEBU_NORTH_BRG_PLL_BASE + 0x10)
#define MVEBU_NORTH_BRG_TBG_CTRL4		(MVEBU_NORTH_BRG_PLL_BASE + 0x14)
#define MVEBU_NORTH_BRG_TBG_CTRL5		(MVEBU_NORTH_BRG_PLL_BASE + 0x18)
#define MVEBU_NORTH_BRG_TBG_CTRL6		(MVEBU_NORTH_BRG_PLL_BASE + 0x1C)
#define MVEBU_NORTH_BRG_TBG_CTRL7		(MVEBU_NORTH_BRG_PLL_BASE + 0x20)
#define MVEBU_NORTH_BRG_TBG_CTRL8		(MVEBU_NORTH_BRG_PLL_BASE + 0x30)

#define MVEBU_NORTH_CLOCK_TBG_SELECT_REG	(MVEBU_NORTH_CLOCK_REGS_BASE + 0x0)
#define MVEBU_NORTH_CLOCK_DIVIDER_SELECT0_REG	(MVEBU_NORTH_CLOCK_REGS_BASE + 0x4)
#define MVEBU_NORTH_CLOCK_DIVIDER_SELECT1_REG	(MVEBU_NORTH_CLOCK_REGS_BASE + 0x8)
#define MVEBU_NORTH_CLOCK_DIVIDER_SELECT2_REG	(MVEBU_NORTH_CLOCK_REGS_BASE + 0xC)
#define MVEBU_NORTH_CLOCK_SELECT_REG		(MVEBU_NORTH_CLOCK_REGS_BASE + 0x10)


/* Reset sample */
#define MVEBU_TEST_PIN_LATCH_N		(MVEBU_TESTPIN_NORTH_REG_BASE + 0x8)

/* Mailbox */
#define MAILBOX_START_ADDR		0x40000000

/* Mailbox offsets 0x0 to 0x40 are WO by aplication CPU (0x44 - 0x7F are undefined)
   and offsets 0x80 to 0xE0 are RO by application CPU (0xE4 - 0xFF are undefined) */
#define MAILBOX_IN_ARG(n)		(MAILBOX_START_ADDR + n * 4)
#define MAILBOX_IN_CMD			(MAILBOX_START_ADDR + 0x40)
#define MAILBOX_OUT_STATUS		(MAILBOX_START_ADDR + 0x80)
#define MAILBOX_OUT_ARG(n)		(MAILBOX_START_ADDR + 0x84 + n * 4)

#define HOST_INT_SET			(MAILBOX_START_ADDR + 0x214)
#define SP_INT_RESET			(MAILBOX_START_ADDR + 0x218)
#define SP_CONTROL			(MAILBOX_START_ADDR + 0x220)


#endif /* __REGS_H */
