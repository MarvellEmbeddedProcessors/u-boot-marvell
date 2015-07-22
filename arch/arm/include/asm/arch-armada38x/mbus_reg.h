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

#ifndef _MBUS_A38X_H_
#define _MBUS_A38X_H_

/* to config MBUS, usually there are severl regs:
  * size_reg: window size
  * ctrl_reg: enable/disable, target, attribute
  * base_reg: window base
  * remap_low_reg & remap_high_reg
  *
  * But not all Soc have the same reg define, some
  * might put size_reg as part of ctrl_reg, some might
  * not have remap_high_reg.
  * To have a unified driver,
  *   - we define all the regs,
  *   - some regs might have the same address, like size_reg
  *      and ctrl_reg, when size information is put into ctrl_reg.
  *   - if some Soc does not have it, we use a Dummy
  *      and make sure it will not be configured.
  */

#define MBUS_WIN_CTRL_REG(win_num)		((win_num < 8) ? \
		(win_num * 0x10) : (0x90 + (win_num-8)*0x08))
#define MBUS_CR_WIN_ENABLE			0x1
#define MBUS_CR_WIN_TARGET_OFFS			4
#define MBUS_CR_WIN_TARGET_MASK			(0xf << MBUS_CR_WIN_TARGET_OFFS)
#define MBUS_CR_WIN_ATTR_OFFS			8
#define MBUS_CR_WIN_ATTR_MASK			(0xff << MBUS_CR_WIN_ATTR_OFFS)

#define MBUS_WIN_SIZE_REG(win_num)		((win_num < 8) ? \
		(win_num * 0x10) : (0x90 + (win_num-8)*0x08))
#define MBUS_CR_WIN_SIZE_OFFS			16
#define MBUS_CR_WIN_SIZE_MASK			(0xffff << MBUS_CR_WIN_SIZE_OFFS)
#define MBUS_CR_WIN_SIZE_ALIGNMENT		0x10000

#define MBUS_WIN_BASE_REG(win_num)		((win_num < 8) ? \
		(0x4 + win_num*0x10) :	(0x94 + (win_num-8)*0x08))
#define MBUS_BR_BASE_OFFS			16
#define MBUS_BR_BASE_MASK			(0xffff <<  MBUS_BR_BASE_OFFS)

#define MBUS_WIN_REMAP_LOW_REG(win_num)		((win_num < 8) ? \
		(0x8 + win_num*0x10) : (0))
#define MBUS_RLR_REMAP_LOW_OFFS			16
#define MBUS_RLR_REMAP_LOW_MASK			(0xffff << MBUS_RLR_REMAP_LOW_OFFS)

#define MBUS_WIN_REMAP_HIGH_REG(win_num)	((win_num < 8) ? \
		(0xC + win_num*0x10) : (0))
#define MBUS_RHR_REMAP_HIGH_OFFS		0
#define MBUS_RHR_REMAP_HIGH_MASK		(0xffffffff << MBUS_RHR_REMAP_HIGH_OFFS)

#define MBUS_WIN_INTEREG_REG			(0x80)

#endif	/* _MBUS_A38X_H_ */


