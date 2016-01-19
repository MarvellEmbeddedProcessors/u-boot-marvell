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

#ifndef __DDR_MCKINLEY6_H
#define __DDR_MCKINLEY6_H

#define MCK6_DRAM_STATUS_REG		(0x8)
#define INIT_DONE0_CH0			(0x1)

#define MCK6_USER_COMMAND_0_REG		(0x20)
#define SDRAM_INIT_REQ_MASK		(0x1)
#define CMD_CH_ENABLE(c)		(1 << (28 + c))
#define CMD_CS_MASK(m)			((m) << 24)

#define MCK6_CTRL_0_REG			(0x44)
#define CTRL_DATA_WIDTH_OFFSET		8
#define CTRL_DATA_WIDTH_MASK		(0xF << 8)
#define BUS_WIDTH_2_IDX(w)		(((w) <= 16) ? ((w) / 8) : (((w) / 32) + 2))
#define CTRL_DATA_WIDTH(w)		(BUS_WIDTH_2_IDX(w) << 8)
#define CTRL_DATA_WIDTH_CALC(v)		(1 << ((v) + 2))

#define MCK6_MMAP0_LOW_CH(i)		(0x200 + 8*(i))
#define MMAP_AREA_LEN_OFFSET		16
#define MMAP_AREA_LEN_MASK		(0x1F << 16)
#define MMAP_AREA_LEN(x)		((x) << 16)

#define DRAM_INIT_TIMEOUT		1000

/* DLL Tune definitions */
#define DLL_PHSEL_START			0x00
#define DLL_PHSEL_END			0x3F
#define DLL_PHSEL_STEP			0x1
#define DLL_RST_TIMER_VAL		0x10
#define DLL_RESET_WAIT_US		100
#define DLL_UPDATE_WAIT_US		50

#define MC6_CH0_PHY_CONTROL_8		(0x1C)
#define DLL_AUTO_UPDATE_EN		(BIT2)
#define DLL_AUTO_MANUAL_UPDATE		(BIT3)
#define DLL_RESET_TIMER(m)		((m & 0xFF) << 24)

#define MC6_CH0_PHY_CONTROL_9		(0x20)
#define PHY_DLL_RESET			(BIT29)
#define DLL_UPDATE_EN_PULSE		(BIT30)

#define MC6_CH0_PHY_DLL_CONTROL_B0	(0x50)
#define DLL_PHASE_POS_SHIFT		(16)
#define DLL_PHASE_NEG_SHIFT		(24)
#define DLL_PHASE_SZ_MASK		(0x3F)

#define DRAM_DIRECT_START		(0x6000000)
#define DRAM_DIRECT_SIZE		(2048)

#define mck6_writel(v, c)		\
do {							\
	debug("0x%p - 0x08%x\n", c, v);	\
	writel(v, c);					\
} while (0)

#endif
