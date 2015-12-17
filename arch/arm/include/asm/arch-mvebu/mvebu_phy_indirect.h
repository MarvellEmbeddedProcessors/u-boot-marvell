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

#ifndef __MVEBU_PHY_INDIRECT_H__
#define __MVEBU_PHY_INDIRECT_H__

enum phy_indirect_unit {
	INDIRECT_IHB,
	INDIRECT_MAX
};

/*
 * Functions prototypes
 */

int mvebu_phy_indirect_init(void);
int mvebu_phy_indirect_read(enum phy_indirect_unit unit, int reg_ofs, u32 *val);
int mvebu_phy_indirect_write(enum phy_indirect_unit unit, int reg_ofs, u32 val);


#endif /* __MVEBU_MMC_H__ */
