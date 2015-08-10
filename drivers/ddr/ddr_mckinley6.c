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
	writel(0x7fbfdfef, 0xf0020340);
	writel(0x020000aa, 0xf0020344);
	writel(0x00200000, 0xf0020310);
	writel(0x00200000, 0xf0020314);
	writel(0x00200000, 0xf0020318);
	writel(0x00200000, 0xf002031c);
#ifdef CONFIG_PALLADIUM
	writel(0x000d0001, 0xf0020200); /* use 512MB CS */
#else
	writel(0x000e0001, 0xf0020200);
#endif
	writel(0x00000000, 0xf0020204);
	writel(0x400e0001, 0xf0020208);
	writel(0x00000000, 0xf002020c);
	writel(0x800e0001, 0xf0020210);
	writel(0x00000000, 0xf0020214);
	writel(0xc00e0001, 0xf0020218);
	writel(0x00000000, 0xf002021c);
#ifdef CONFIG_PALLADIUM
	writel(0x13010332, 0xf0020220); /* use 64 bit ls*/
#else
	writel(0x05000632, 0xf0020220);
#endif
	writel(0x05000632, 0xf0020224);
	writel(0x05000632, 0xf0020228);
	writel(0x05000632, 0xf002022c);
	writel(0x00030400, 0xf0020044);
	writel(0x00006000, 0xf00202c0);
	writel(0x00000020, 0xf00202c4);
	writel(0x3fd5b595, 0xf0020058);
	writel(0x00000001, 0xf0020048);
	writel(0x00010200, 0xf0020180);
	writel(0x000000ff, 0xf0020050);
#ifdef CONFIG_PALLADIUM
	writel(0x00000000, 0xf002004c); /* Disable ECC */
#else
	writel(0x00000002, 0xf002004c);
#endif
	writel(0x00000480, 0xf0020054);
	writel(0x0000080a, 0xf0020300);
	writel(0x00000032, 0xf0020380);
	writel(0x00000032, 0xf0020384);
	writel(0x09600050, 0xf0020388);
	writel(0x00000200, 0xf002038c);
	writel(0x00400100, 0xf0020390);
	writel(0x00f00618, 0xf0020394);
	writel(0x00f80200, 0xf0020398);
	writel(0x00120808, 0xf002039c);
	writel(0x00050514, 0xf00203a0);
	writel(0x00000001, 0xf00203a4);
	writel(0x00000c04, 0xf00203a8);
	writel(0x18260a1c, 0xf00203ac);
	writel(0x0a0c060a, 0xf00203b0);
	writel(0x04040606, 0xf00203b4);
	writel(0x00000606, 0xf00203b8);
	writel(0x02020404, 0xf00203bc);
	writel(0x00000000, 0xf00203c0);
	writel(0x00000000, 0xf00203c4);
	writel(0x00081239, 0xf00203dc);
	writel(0x000e000e, 0xf0020064);
	writel(0x10077779, 0xf0020804);
	writel(0x1ff00770, 0xf0020808);
	writel(0x3f03fc77, 0xf002080c);
	writel(0x00000000, 0xf0020828);
	writel(0x03800000, 0xf0020830);
	writel(0x00300000, 0xf0020834);
	writel(0x00100100, 0xf0020810);
	writel(0x00000400, 0xf0020840);
	writel(0x81000001, 0xf00208c0);
	writel(0x00010000, 0xf00208e0);
	writel(0x00000000, 0xf00208d0);
	writel(0x00020000, 0xf00208e0);
	writel(0x00000000, 0xf00208d0);
	writel(0x00040000, 0xf00208e0);
	writel(0x00000000, 0xf00208d0);
	writel(0x00080000, 0xf00208e0);
	writel(0x00000000, 0xf00208d0);
	writel(0x00000000, 0xf0020890);
	writel(0x00000000, 0xf0020894);
	writel(0x00000000, 0xf0020898);
	writel(0x00000000, 0xf002089c);
	writel(0x00000000, 0xf00208a0);
	writel(0x00000000, 0xf00208a4);
	writel(0x00000000, 0xf00208a8);
	writel(0x00000000, 0xf00208ac);
	writel(0x00000000, 0xf00208b0);
	writel(0x00004040, 0xf0020800);
	writel(0x00080200, 0xf0020814);
	writel(0x00000003, 0xf0020838);
	writel(0x00000020, 0xf002083c);
	writel(0x0000010a, 0xf0020980);
	writel(0x0000010a, 0xf0020d80);
	writel(0x0000010a, 0xf0020984);
	writel(0x0000010a, 0xf0020d84);
	writel(0x0000010a, 0xf0020988);
	writel(0x0000010a, 0xf0020d88);
	writel(0x0000010a, 0xf002098c);
	writel(0x0000010a, 0xf0020d8c);
	writel(0x0000010a, 0xf0020990);
	writel(0x0000010a, 0xf0020d90);
	writel(0x0000010a, 0xf0020994);
	writel(0x0000010a, 0xf0020d94);
	writel(0x0000010a, 0xf0020998);
	writel(0x0000010a, 0xf0020d98);
	writel(0x0000010a, 0xf002099c);
	writel(0x0000010a, 0xf0020d9c);
	writel(0x0000010a, 0xf00209a0);
	writel(0x0000010a, 0xf0020da0);
	writel(0x0000010a, 0xf00209a4);
	writel(0x0000010a, 0xf0020da4);
	writel(0x0000010a, 0xf00209a8);
	writel(0x0000010a, 0xf0020da8);
	writel(0x0000010a, 0xf00209ac);
	writel(0x0000010a, 0xf0020dac);
	writel(0x0000010a, 0xf00209b0);
	writel(0x0000010a, 0xf0020db0);
	writel(0x0000010a, 0xf00209b4);
	writel(0x0000010a, 0xf0020db4);
	writel(0x0000010a, 0xf00209b8);
	writel(0x0000010a, 0xf0020db8);
	writel(0x0000010a, 0xf00209bc);
	writel(0x0000010a, 0xf0020dbc);
	writel(0x0000010a, 0xf00209c0);
	writel(0x0000010a, 0xf0020dc0);
	writel(0x0000010a, 0xf00209c4);
	writel(0x0000010a, 0xf0020dc4);
	writel(0x0000010a, 0xf00209c8);
	writel(0x0000010a, 0xf0020dc8);
	writel(0x0000010a, 0xf00209cc);
	writel(0x0000010a, 0xf0020dcc);
	writel(0x0000010a, 0xf00209d0);
	writel(0x0000010a, 0xf0020dd0);
	writel(0x0000010a, 0xf00209d4);
	writel(0x0000010a, 0xf0020dd4);
	writel(0x0000010a, 0xf00209d8);
	writel(0x0000010a, 0xf0020dd8);
	writel(0x0000010a, 0xf00209dc);
	writel(0x0000010a, 0xf0020ddc);
	writel(0x0000010a, 0xf00209e0);
	writel(0x0000010a, 0xf0020de0);
	writel(0x0000010a, 0xf00209e4);
	writel(0x0000010a, 0xf0020de4);
	writel(0x0000010a, 0xf00209e8);
	writel(0x0000010a, 0xf0020de8);
	writel(0x0000010a, 0xf00209ec);
	writel(0x0000010a, 0xf0020dec);
	writel(0x0000010a, 0xf00209f0);
	writel(0x0000010a, 0xf0020df0);
	writel(0x0000010a, 0xf00209f4);
	writel(0x0000010a, 0xf0020df4);
	writel(0x0000010a, 0xf00209f8);
	writel(0x0000010a, 0xf0020df8);
	writel(0x0000010a, 0xf00209fc);
	writel(0x0000010a, 0xf0020dfc);
	writel(0x0000010a, 0xf0020a00);
	writel(0x0000010a, 0xf0020e00);
	writel(0x0000010a, 0xf0020a04);
	writel(0x0000010a, 0xf0020e04);
	writel(0x0000010a, 0xf0020a08);
	writel(0x0000010a, 0xf0020e08);
	writel(0x0000010a, 0xf0020a0c);
	writel(0x0000010a, 0xf0020e0c);
	writel(0x08080000, 0xf0020850);
	writel(0x08080000, 0xf0020854);
	writel(0x08080000, 0xf0020858);
	writel(0x08080000, 0xf002085c);
	writel(0x08080000, 0xf0020860);
	writel(0x08080000, 0xf0020864);
	writel(0x08080000, 0xf0020868);
	writel(0x08080000, 0xf002086c);
	writel(0x08080000, 0xf0020870);
	writel(0x20000000, 0xf0020820);
	writel(0x40000000, 0xf0020820);
	writel(0x80000000, 0xf0020820);
	writel(0x1f000001, 0xf0020020);
}

void mvebu_dram_phy_init(void)
{
	u32 reg;
	reg = readl(0xf0020008);
	while (reg != 0x00001111)
		reg = readl(0xf0020008);
}
