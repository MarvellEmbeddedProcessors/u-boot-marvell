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

void mvebu_dram_mac_init(struct mvebu_dram_config *dram_config)
{
	void __iomem *base_addr = dram_config->mac_base;

	writel(0x7B004a28, base_addr + 0x0);
	udelay(2000);
	writel(0x36301820, base_addr + 0x4);
	udelay(2000);
	writel(0x43149997, base_addr + 0x8);
	udelay(2000);
	writel(0x38411bc7, base_addr + 0xC);
	udelay(2000);
	writel(0x14300000, base_addr + 0x10);
	udelay(2000);
	writel(0x00000700, base_addr + 0x14);
	udelay(2000);
	writel(0x0060f3ff, base_addr + 0x24);
	udelay(2000);
	writel(0x000F8830, base_addr + 0x28);
	udelay(2000);
	writel(0x28c50f8 , base_addr + 0x2C);
	udelay(2000);
	writel(0x0000c671, base_addr + 0x7C);
	udelay(2000);
	writel(0x00030000, base_addr + 0x94);
	udelay(2000);
	writel(0x00000300, base_addr + 0x9C);
	udelay(2000);
	writel(0x00000000, base_addr + 0xa8);
	udelay(2000);
	writel(0xbd09000d, base_addr + 0xcc);
	udelay(2000);
	writel(0x00000000, base_addr + 0x74);
	udelay(2000);
	writel(0x00000009, base_addr + 0x138);
	udelay(2000);
	writel(0x0000000c, base_addr + 0x13C);
	udelay(2000);
	writel(0xFFFFFFF1, base_addr + 0x104);
	udelay(2000);
	writel(0xFFFFFFE5, base_addr + 0x10c);
	udelay(2000);
	writel(0x00000000, base_addr + 0x114);
	udelay(2000);
	writel(0x0       , base_addr + 0x11c);
	udelay(2000);
	writel(0x00000650, base_addr + 0x1D0);
	udelay(2000);
	writel(0x00000046, base_addr + 0x1D4);
	udelay(2000);
	writel(0x00000010, base_addr + 0x1D8);
	udelay(2000);
	writel(0x00000000, base_addr + 0x1DC);
	udelay(2000);
	writel(0x23      , base_addr + 0x1E0);
	udelay(2000);
	writel(0x00203c18, base_addr + 0x1E4);
	udelay(2000);
	writel(0xf8000019, base_addr + 0x1EC);
	udelay(2000);
}

void mvebu_dram_phy_init(struct mvebu_dram_config *dram_config)
{
	void __iomem *base_addr = dram_config->phy_base;
	u32 reg;

	writel(0xe8243dfe, base_addr + 0xA0);
	udelay(2000);
	writel(0xe8280434, base_addr + 0xA0);
	udelay(2000);

	writel(0x281020da, base_addr + 0xA0);
	udelay(2000);
	writel(0xe8260cb2, base_addr + 0xA0);
	udelay(2000);
	writel(0xe8290000, base_addr + 0xA0);
	udelay(2000);
	writel(0xf810001f, base_addr + 0xA0);
	udelay(2000);

	writel(0xC0005847, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC0406049, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC080704d, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC0C0a85b, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC002008d, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC0420084, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC082009f, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC0c20099, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC003000F, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC043000f, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC083000f, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0xC0C3000f, base_addr + 0xA0);
	udelay(2000);
	reg = readl(base_addr + 0xA0);
	while ((reg & (1<<31)) > 0)
		reg = readl(base_addr + 0xA0);

	writel(0x00000001, 0xD0001480);
	udelay(2000000);

	writel(0x80100008, 0xD00015B0);
	udelay(2000);
	writel(0x80100002, 0xD00015B0);
	udelay(2000);
	writel(0x80100040, 0xD00015B0);
	udelay(2000);
	writel(0x11,       0xD0018000);
	udelay(2000);
	writel(0x0,        0xD00200e8);
	udelay(2000);
	writel(0x0FFFFFE1, 0xD0020184);
	udelay(2000);
	writel(0xfffdffff, 0xD00182e4);
	udelay(2000);
}
