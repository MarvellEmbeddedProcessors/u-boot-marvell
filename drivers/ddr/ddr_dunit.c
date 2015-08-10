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

void mvebu_dram_mac_init(void)
{
	writel(0x7B004a28, 0xD0001400);
	udelay(2000);
	writel(0x36301820, 0xD0001404);
	udelay(2000);
	writel(0x43149997, 0xD0001408);
	udelay(2000);
	writel(0x38411bc7, 0xD000140C);
	udelay(2000);
	writel(0x14300000, 0xD0001410);
	udelay(2000);
	writel(0x00000700, 0xD0001414);
	udelay(2000);
	writel(0x0060f3ff, 0xD0001424);
	udelay(2000);
	writel(0x000F8830, 0xD0001428);
	udelay(2000);
	writel(0x28c50f8 , 0xD000142C);
	udelay(2000);
	writel(0x0000c671, 0xD000147C);
	udelay(2000);
	writel(0x00030000, 0xD0001494);
	udelay(2000);
	writel(0x00000300, 0xD000149C);
	udelay(2000);
	writel(0x00000000, 0xD00014a8);
	udelay(2000);
	writel(0xbd09000d, 0xD00014cc);
	udelay(2000);
	writel(0x00000000, 0xD0001474);
	udelay(2000);
	writel(0x00000009, 0xD0001538);
	udelay(2000);
	writel(0x0000000c, 0xD000153C);
	udelay(2000);
	writel(0xFFFFFFF1, 0xD0001504);
	udelay(2000);
	writel(0xFFFFFFE5, 0xD000150c);
	udelay(2000);
	writel(0x00000000, 0xD0001514);
	udelay(2000);
	writel(0x0       , 0xD000151c);
	udelay(2000);
	writel(0x00000650, 0xD00015D0);
	udelay(2000);
	writel(0x00000046, 0xD00015D4);
	udelay(2000);
	writel(0x00000010, 0xD00015D8);
	udelay(2000);
	writel(0x00000000, 0xD00015DC);
	udelay(2000);
	writel(0x23      , 0xD00015E0);
	udelay(2000);
	writel(0x00203c18, 0xD00015E4);
	udelay(2000);
	writel(0xf8000019, 0xD00015EC);
	udelay(2000);

	writel(0xe8243dfe, 0xD00016A0);
	udelay(2000);
	writel(0xe8280434, 0xD00016A0);
	udelay(2000);

	writel(0x281020da, 0xD00016A0);
	udelay(2000);
	writel(0xe8260cb2, 0xD00016A0);
	udelay(2000);
	writel(0xe8290000, 0xD00016A0);
	udelay(2000);
	writel(0xf810001f, 0xD00016A0);
	udelay(2000);
}

void mvebu_dram_phy_init(void)
{
	u32 reg;
	writel(0xC0005847, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC0406049, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC080704d, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC0C0a85b, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC002008d, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC0420084, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC082009f, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC0c20099, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC003000F, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC043000f, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC083000f, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

	writel(0xC0C3000f, 0xD00016A0);
	udelay(2000);
	reg = readl(0xD00016A0);
	while ((reg & (1<<31)) > 0)
		reg = readl(0xD00016A0);

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
