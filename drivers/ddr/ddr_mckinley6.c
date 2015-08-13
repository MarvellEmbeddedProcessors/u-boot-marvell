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

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/ddr.h>

#define MAX_REG_OFFSET 39

enum mvebu_mck_freq_support {
	FREQ_667_HZ = 0,
	FREQ_800_HZ,
	FREQ_933_HZ,
	MAX_HZ_SUPPORTED,
};

struct mvebu_mckinley_config {
	u32 reg_offset;
	u32 values[MAX_HZ_SUPPORTED];
};

struct mvebu_mckinley_config mckinley_mac_config[MAX_REG_OFFSET] = {
/*      reg_offset  667_HZ      800_HZ      933_HZ     */
	{ 0x200, { 0x000D0001, 0x000D0001, 0x000C0001} },
	{ 0x200, { 0x000D0001, 0x000D0001, 0x000C0001} },
	{ 0x204, { 0x00000000, 0x00000000, 0x00000000} },
	{ 0x208, { 0xA00E0000, 0xA00E0000, 0xA00E0001} },
	{ 0x20C, { 0x00000010, 0x00000010, 0x00000010} },
	{ 0x210, { 0x00180000, 0x00180000, 0x00180000} },
	{ 0x214, { 0x00000080, 0x00000080, 0x00000080} },
	{ 0x218, { 0x000F0000, 0x000F0000, 0x000F0000} },
	{ 0x21C, { 0x00000090, 0x00000090, 0x00000090} },
	{ 0x220, { 0x02000539, 0x02000539, 0x02000332} },
	{ 0x224, { 0x02001555, 0x02001555, 0x02001555} },
	{ 0x228, { 0x02002228, 0x02002228, 0x02002228} },
	{ 0x22C, { 0x02003116, 0x02003116, 0x02003116} },
	{ 0x044, { 0x00030400, 0x00030400, 0x00030300} },
	{ 0x2C4, { 0x00000030, 0x00000030, 0x00000030} },
	{ 0x300, { 0x00000909, 0x0000090C, 0x00000A0D} },
	{ 0x304, { 0x00000000, 0x00000000, 0x00000000} },
	{ 0x380, { 0x000001F4, 0x00000258, 0x000002BC} },
	{ 0x384, { 0x000003E8, 0x000004B0, 0x0002D8E8} },
	{ 0x388, { 0x00000043, 0x00000050, 0x0000005E} },
	{ 0x38C, { 0x00000400, 0x00000400, 0x00000400} },
	{ 0x390, { 0x00800200, 0x00800200, 0x00800200} },
	{ 0x394, { 0x006B03CF, 0x008003CF, 0x009603CF} },
	{ 0x398, { 0x00B40300, 0x00D80300, 0x009F0255} },
	{ 0x39C, { 0x00000707, 0x00000808, 0x00000A0A} },
	{ 0x3A0, { 0x00040400, 0x00050500, 0x00050600} },
	{ 0x3A4, { 0x00000002, 0x00000002, 0x00000001} },
	{ 0x3A8, { 0x00001808, 0x00001808, 0x00001808} },
	{ 0x3AC, { 0x14200A16, 0x14260C1B, 0x162D0D20} },
	{ 0x3B0, { 0x0A0A050A, 0x0C0C060C, 0x0D0E070D} },
	{ 0x3B4, { 0x04040502, 0x05040602, 0x05040703} },
	{ 0x3B8, { 0x00000400, 0x00000500, 0x00000500} },
	{ 0x3BC, { 0x00000000, 0x00000000, 0x00000000} },
	{ 0x3C0, { 0x00000000, 0x00000000, 0x00000000} },
	{ 0x3C4, { 0x00000000, 0x00000000, 0x00000000} },
	{ 0x3C8, { 0x00000000, 0x00000000, 0x00000000} },
	{ 0x3CC, { 0x00000000, 0x00000000, 0x00000000} },
	{ 0x3D0, { 0x00000000, 0x00000000, 0x00000000} },
	{ 0x048, { 0x000FF1FC, 0x000FF1FC, 0x000FF1FC} },
};

void mvebu_dram_mac_init(struct mvebu_dram_config *dram_config)
{
	void __iomem *base_addr = dram_config->mac_base;
	struct mvebu_mckinley_config *mac_config = &mckinley_mac_config[0];
	u32 i, freq_indx;

	/* for now set the frequency to 667 (index 0) */
	freq_indx = 0;
	for (i = 0 ; i < MAX_REG_OFFSET; i++)
		writel(mac_config->values[freq_indx], base_addr + mac_config->reg_offset);

}

void mvebu_dram_phy_init(struct mvebu_dram_config *dram_config)
{
	void __iomem *base_addr = dram_config->phy_base;
	u32 reg;

	reg = readl(base_addr + 0x8);
	while (reg != 0x00001111)
		reg = readl(base_addr + 0x8);
}
