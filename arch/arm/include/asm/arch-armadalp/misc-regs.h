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
#include <asm/arch/regs-base.h>

#define MVEBU_NB_PM_PWRDWN_OPT_REG     (MVEBU_NORTH_PM_REGS_BASE + 0x8)
#define L2_SRAM_LKG_PD_EN              (BIT8)
#define CPU_ROM_PD_EN                  (BIT10)
#define AVS_DISABLE_MODE               (BIT14)
#define DDRPHY_PAD_PWRDWN_EN           (BIT22)

#define MVEBU_AXI_DCTRL_CTRL_RST_REG   (MVEBU_AXI_DCTRL_REGS_BASE)
#define DDRPHY_MODE_SELECT             (BIT16)         /* 0: 2 to 1, 1: 4 to 1 */

#define MVEBU_AXI_DCTRL_MCM_SEL_REG    (MVEBU_AXI_DCTRL_REGS_BASE + 0x4)


#endif /* _MISC_REGS_H_ */
