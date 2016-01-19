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

/*
** ecc_enabled - when true, DRAM ecc is enabled.
** cs_count - Number of DRAM chip-selects.
** bus_width - 16, 32 or 64 bit DDR.
** size_mb - when != 0, override DDR size according to this value (in MB)
*/
struct mvebu_dram_config {
	void __iomem *mac_base;
	void __iomem *phy_base;

	/* DRAM configuration options. */
	bool ecc_enabled;
	u32 cs_count;
	u32 bus_width;
	u32 size_mb;
	u32 freq_mhz;
};
void mvebu_dram_init(const void *blob);
void mvebu_dram_mac_init(struct mvebu_dram_config *dram_config);
void mvebu_dram_phy_init(struct mvebu_dram_config *dram_config);
void mvebu_dram_dll_tune(struct mvebu_dram_config *dram_config);
#endif /* _MVEBU_DDR_H_ */
