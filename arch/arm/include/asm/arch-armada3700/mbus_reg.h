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

#ifndef _MBUS_A3700_H_
#define _MBUS_A3700_H_

#define MBUS_WIN_CTRL_REG(win_num)		(win_num * 0x10)
#define MBUS_CR_WIN_ENABLE				0x1
#define MBUS_CR_WIN_TARGET_OFFS			4
#define MBUS_CR_WIN_TARGET_MASK			(0xf << MBUS_CR_WIN_TARGET_OFFS)
#define MBUS_CR_WIN_ATTR_OFFS			8
#define MBUS_CR_WIN_ATTR_MASK			(0xff << MBUS_CR_WIN_ATTR_OFFS)

#define MBUS_WIN_SIZE_REG(win_num)		(0x4 + win_num * 0x10)
#define MBUS_CR_WIN_SIZE_OFFS			0
#define MBUS_CR_WIN_SIZE_MASK			(0xffffff << MBUS_CR_WIN_SIZE_OFFS)
#define MBUS_CR_WIN_SIZE_ALIGNMENT		0x10000


#define MBUS_WIN_BASE_REG(win_num)		(0x8 + win_num * 0x10)
#define MBUS_BR_BASE_OFFS				0
#define MBUS_BR_BASE_MASK				(0xffffff <<  MBUS_BR_BASE_OFFS)

#define MBUS_WIN_REMAP_LOW_REG(win_num)	(0xc + win_num * 0x10)
#define MBUS_RLR_REMAP_LOW_OFFS			0
#define MBUS_RLR_REMAP_LOW_MASK			(0xffffff << MBUS_RLR_REMAP_LOW_OFFS)

/* there is no remap_high reg in Armada3700, just define here to pass compilation */
#define MBUS_WIN_REMAP_HIGH_REG(win_num)	(0xc + win_num * 0x10)

#define MBUS_WIN_INTEREG_REG			(0xF0)

#endif	/* _MBUS_A3700_H_ */

