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

#ifndef _MVEBU_DDR_H_
#define _MVEBU_DDR_H_

struct mvebu_dram_config {
	void __iomem *mac_base;
	void __iomem *phy_base;
	u32 ecc_enabled;
	u32 cs_count;
};

void static_dram_init(const void *blob);
void mvebu_dram_mac_init(struct mvebu_dram_config *dram_config);
void mvebu_dram_phy_init(struct mvebu_dram_config *dram_config);

#endif /* _MVEBU_DDR_H_ */
