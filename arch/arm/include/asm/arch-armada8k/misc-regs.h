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

#ifndef _MISC_REGS_H_
#define _MISC_REGS_H_
#include <asm/arch-armada8k/regs-base.h>

#define MVEBU_AMB_IP_BRIDGE_WIN_REG(win)	(MVEBU_AMB_IP_BASE + (win * 0x8))
#define MVEBU_AMB_IP_BRIDGE_WIN_EN_OFFSET	0
#define MVEBU_AMB_IP_BRIDGE_WIN_EN_MASK		(0x1 << MVEBU_AMB_IP_BRIDGE_WIN_EN_OFFSET)
#define MVEBU_AMB_IP_BRIDGE_WIN_SIZE_OFFSET	16
#define MVEBU_AMB_IP_BRIDGE_WIN_SIZE_MASK	(0xffff << MVEBU_AMB_IP_BRIDGE_WIN_SIZE_OFFSET)

#endif /* _MISC_REGS_H_ */
