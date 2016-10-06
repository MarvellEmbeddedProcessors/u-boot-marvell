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

enum mci_unit {
	AP_PHY,
	AP_CTRL,
	CP_PHY,
	CP_CTRL,
	MCI_MAX
};

enum mci_region {
	MCI_CTRL_REGION     = 0,
	MCI_PHY_REG_REGION  = 3,
};
/*
 * Functions prototypes
 */

int mvebu_mci_phy_init(void);
int mvebu_mci_phy_read(enum mci_region region, enum mci_unit unit, int unit_id, int reg_ofs, u32 *val);
int mvebu_mci_phy_write(enum mci_region region, enum mci_unit unit, int unit_id, int reg_ofs, u32 val);


#endif /* __MVEBU_MMC_H__ */
