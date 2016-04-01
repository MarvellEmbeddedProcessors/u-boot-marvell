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

#ifndef _MULTI_FDT_H_
#define _MULTI_FDT_H_

#define A3700_DB_ID			0x20
/*
 * There are two main presets for Armada3700 DB, between
 * these two presets, most of the configurations are the same:
   - CPU 600MHz and DDR 600MHz
   - SATA
   - neta0 - RGMII
   - PCIe(COMPHY-0)
 *
 * The only difference is COMPHY-1
 * preset 1: COMPHY-1 is working as USB3.
 * preset 2. COMPHY-1 is working as SGMII-1
 */
#define A3700_DB_CONFIG_ID_USB3		1
#define A3700_DB_CONFIG_ID_SGMII1	2
#define A3700_DB_CONFIG_ID_SFP_2_5G	3

#define A3700_DB_CONFIG_ID_DEFAULT	A3700_DB_CONFIG_ID_USB3

#endif /* _MULTI_FDT_H_ */

