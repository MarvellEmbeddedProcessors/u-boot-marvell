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

#ifndef _COMPHY_HPIPE_H_
#define _COMPHY_HPIPE_H_

#define HPIPE_POWER_AND_PLL_CTRL_REG            0x004
#define HPIPE_SQUELCH_FFE_SETTING_REG           0x018
#define HPIPE_DFE_REG0                          0x01C
#define HPIPE_G1_SETTINGS_0_REG                 0x034
#define HPIPE_G1_SETTINGS_1_REG                 0x038
#define HPIPE_G2_SETTINGS_1_REG                 0x040
#define HPIPE_SYNC_PATTERN_REG                  0x090
#define HPIPE_INTERFACE_REG                     0x094
#define HPIPE_ISOLATE_REG                       0x098
#define HPIPE_VTHIMPCAL_CTRL_REG                0x104
#define HPIPE_PCIE_REG0                         0x120
#define HPIPE_LANE_ALIGN_REG0                   0x124
#define HPIPE_MISC_REG                          0x13C
#define HPIPE_GLUE_REG                          0x140
#define HPIPE_GENERATION_DIVIDER_FORCE_REG      0x144
#define HPIPE_RESET_DFE_REG                     0x148
#define HPIPE_PLLINTP_REG1                      0x150
#define HPIPE_RX_REG3                           0x188
#define HPIPE_PCIE_REG1                         0x288
#define HPIPE_PCIE_REG3                         0x290
#define HPIPE_G1_SETTINGS_3_REG                 0x440
#define HPIPE_G1_SETTINGS_4_REG                 0x444
#define HPIPE_G2_SETTINGS_3_REG                 0x448
#define HPIPE_G2_SETTINGS_4_REG                 0x44C
#define HPIPE_LANE_STATUS0_REG			0x60C
#define HPIPE_LANE_CFG4_REG                     0x620
#define HPIPE_GLOBAL_CLK_CTRL                   0x704
#define HPIPE_GLOBAL_MISC_CTRL                  0x718
#define HPIPE_GLOBAL_PM_CTRL                    0x740

#endif /* _COMPHY_HPIPE_H_ */

