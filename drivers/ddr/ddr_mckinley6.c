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

	writel(0x7fbfdfef, base_addr + 0x340);
	writel(0x020000aa, base_addr + 0x344);
	writel(0x00200000, base_addr + 0x310);
	writel(0x00200000, base_addr + 0x314);
	writel(0x00200000, base_addr + 0x318);
	writel(0x00200000, base_addr + 0x31c);
#ifdef CONFIG_PALLADIUM
	writel(0x000d0001, base_addr + 0x200); /* use 512MB CS */
#else
	writel(0x000e0001, base_addr + 0x200);
#endif
	writel(0x00000000, base_addr + 0x204);
	writel(0x400e0001, base_addr + 0x208);
	writel(0x00000000, base_addr + 0x20c);
	writel(0x800e0001, base_addr + 0x210);
	writel(0x00000000, base_addr + 0x214);
	writel(0xc00e0001, base_addr + 0x218);
	writel(0x00000000, base_addr + 0x21c);
#ifdef CONFIG_PALLADIUM
	writel(0x13010332, base_addr + 0x220); /* use 64 bit ls*/
#else
	writel(0x05000632, base_addr + 0x220);
#endif
	writel(0x05000632, base_addr + 0x224);
	writel(0x05000632, base_addr + 0x228);
	writel(0x05000632, base_addr + 0x22c);
	writel(0x00030400, base_addr + 0x044);
	writel(0x00006000, base_addr + 0x2c0);
	writel(0x00000020, base_addr + 0x2c4);
	writel(0x3fd5b595, base_addr + 0x058);
	writel(0x00000001, base_addr + 0x048);
	writel(0x00010200, base_addr + 0x180);
	writel(0x000000ff, base_addr + 0x050);
#ifdef CONFIG_PALLADIUM
	writel(0x00000000, base_addr + 0x04c); /* Disable ECC */
#else
	writel(0x00000002, base_addr + 0x04c);
#endif
	writel(0x00000480, base_addr + 0x054);
	writel(0x0000080a, base_addr + 0x300);
	writel(0x00000032, base_addr + 0x380);
	writel(0x00000032, base_addr + 0x384);
	writel(0x09600050, base_addr + 0x388);
	writel(0x00000200, base_addr + 0x38c);
	writel(0x00400100, base_addr + 0x390);
	writel(0x00f00618, base_addr + 0x394);
	writel(0x00f80200, base_addr + 0x398);
	writel(0x00120808, base_addr + 0x39c);
	writel(0x00050514, base_addr + 0x3a0);
	writel(0x00000001, base_addr + 0x3a4);
	writel(0x00000c04, base_addr + 0x3a8);
	writel(0x18260a1c, base_addr + 0x3ac);
	writel(0x0a0c060a, base_addr + 0x3b0);
	writel(0x04040606, base_addr + 0x3b4);
	writel(0x00000606, base_addr + 0x3b8);
	writel(0x02020404, base_addr + 0x3bc);
	writel(0x00000000, base_addr + 0x3c0);
	writel(0x00000000, base_addr + 0x3c4);
	writel(0x00081239, base_addr + 0x3dc);
	writel(0x000e000e, base_addr + 0x064);
	writel(0x10077779, base_addr + 0x804);
	writel(0x1ff00770, base_addr + 0x808);
	writel(0x3f03fc77, base_addr + 0x80c);
	writel(0x00000000, base_addr + 0x828);
	writel(0x03800000, base_addr + 0x830);
	writel(0x00300000, base_addr + 0x834);
	writel(0x00100100, base_addr + 0x810);
	writel(0x00000400, base_addr + 0x840);
	writel(0x81000001, base_addr + 0x8c0);
	writel(0x00010000, base_addr + 0x8e0);
	writel(0x00000000, base_addr + 0x8d0);
	writel(0x00020000, base_addr + 0x8e0);
	writel(0x00000000, base_addr + 0x8d0);
	writel(0x00040000, base_addr + 0x8e0);
	writel(0x00000000, base_addr + 0x8d0);
	writel(0x00080000, base_addr + 0x8e0);
	writel(0x00000000, base_addr + 0x8d0);
	writel(0x00000000, base_addr + 0x890);
	writel(0x00000000, base_addr + 0x894);
	writel(0x00000000, base_addr + 0x898);
	writel(0x00000000, base_addr + 0x89c);
	writel(0x00000000, base_addr + 0x8a0);
	writel(0x00000000, base_addr + 0x8a4);
	writel(0x00000000, base_addr + 0x8a8);
	writel(0x00000000, base_addr + 0x8ac);
	writel(0x00000000, base_addr + 0x8b0);
	writel(0x00004040, base_addr + 0x800);
	writel(0x00080200, base_addr + 0x814);
	writel(0x00000003, base_addr + 0x838);
	writel(0x00000020, base_addr + 0x83c);
	writel(0x0000010a, base_addr + 0x980);
	writel(0x0000010a, base_addr + 0xd80);
	writel(0x0000010a, base_addr + 0x984);
	writel(0x0000010a, base_addr + 0xd84);
	writel(0x0000010a, base_addr + 0x988);
	writel(0x0000010a, base_addr + 0xd88);
	writel(0x0000010a, base_addr + 0x98c);
	writel(0x0000010a, base_addr + 0xd8c);
	writel(0x0000010a, base_addr + 0x990);
	writel(0x0000010a, base_addr + 0xd90);
	writel(0x0000010a, base_addr + 0x994);
	writel(0x0000010a, base_addr + 0xd94);
	writel(0x0000010a, base_addr + 0x998);
	writel(0x0000010a, base_addr + 0xd98);
	writel(0x0000010a, base_addr + 0x99c);
	writel(0x0000010a, base_addr + 0xd9c);
	writel(0x0000010a, base_addr + 0x9a0);
	writel(0x0000010a, base_addr + 0xda0);
	writel(0x0000010a, base_addr + 0x9a4);
	writel(0x0000010a, base_addr + 0xda4);
	writel(0x0000010a, base_addr + 0x9a8);
	writel(0x0000010a, base_addr + 0xda8);
	writel(0x0000010a, base_addr + 0x9ac);
	writel(0x0000010a, base_addr + 0xdac);
	writel(0x0000010a, base_addr + 0x9b0);
	writel(0x0000010a, base_addr + 0xdb0);
	writel(0x0000010a, base_addr + 0x9b4);
	writel(0x0000010a, base_addr + 0xdb4);
	writel(0x0000010a, base_addr + 0x9b8);
	writel(0x0000010a, base_addr + 0xdb8);
	writel(0x0000010a, base_addr + 0x9bc);
	writel(0x0000010a, base_addr + 0xdbc);
	writel(0x0000010a, base_addr + 0x9c0);
	writel(0x0000010a, base_addr + 0xdc0);
	writel(0x0000010a, base_addr + 0x9c4);
	writel(0x0000010a, base_addr + 0xdc4);
	writel(0x0000010a, base_addr + 0x9c8);
	writel(0x0000010a, base_addr + 0xdc8);
	writel(0x0000010a, base_addr + 0x9cc);
	writel(0x0000010a, base_addr + 0xdcc);
	writel(0x0000010a, base_addr + 0x9d0);
	writel(0x0000010a, base_addr + 0xdd0);
	writel(0x0000010a, base_addr + 0x9d4);
	writel(0x0000010a, base_addr + 0xdd4);
	writel(0x0000010a, base_addr + 0x9d8);
	writel(0x0000010a, base_addr + 0xdd8);
	writel(0x0000010a, base_addr + 0x9dc);
	writel(0x0000010a, base_addr + 0xddc);
	writel(0x0000010a, base_addr + 0x9e0);
	writel(0x0000010a, base_addr + 0xde0);
	writel(0x0000010a, base_addr + 0x9e4);
	writel(0x0000010a, base_addr + 0xde4);
	writel(0x0000010a, base_addr + 0x9e8);
	writel(0x0000010a, base_addr + 0xde8);
	writel(0x0000010a, base_addr + 0x9ec);
	writel(0x0000010a, base_addr + 0xdec);
	writel(0x0000010a, base_addr + 0x9f0);
	writel(0x0000010a, base_addr + 0xdf0);
	writel(0x0000010a, base_addr + 0x9f4);
	writel(0x0000010a, base_addr + 0xdf4);
	writel(0x0000010a, base_addr + 0x9f8);
	writel(0x0000010a, base_addr + 0xdf8);
	writel(0x0000010a, base_addr + 0x9fc);
	writel(0x0000010a, base_addr + 0xdfc);
	writel(0x0000010a, base_addr + 0xa00);
	writel(0x0000010a, base_addr + 0xe00);
	writel(0x0000010a, base_addr + 0xa04);
	writel(0x0000010a, base_addr + 0xe04);
	writel(0x0000010a, base_addr + 0xa08);
	writel(0x0000010a, base_addr + 0xe08);
	writel(0x0000010a, base_addr + 0xa0c);
	writel(0x0000010a, base_addr + 0xe0c);
	writel(0x08080000, base_addr + 0x850);
	writel(0x08080000, base_addr + 0x854);
	writel(0x08080000, base_addr + 0x858);
	writel(0x08080000, base_addr + 0x85c);
	writel(0x08080000, base_addr + 0x860);
	writel(0x08080000, base_addr + 0x864);
	writel(0x08080000, base_addr + 0x868);
	writel(0x08080000, base_addr + 0x86c);
	writel(0x08080000, base_addr + 0x870);
	writel(0x20000000, base_addr + 0x820);
	writel(0x40000000, base_addr + 0x820);
	writel(0x80000000, base_addr + 0x820);
	writel(0x1f000001, base_addr + 0x020);
}

void mvebu_dram_phy_init(struct mvebu_dram_config *dram_config)
{
	void __iomem *base_addr = dram_config->phy_base;
	u32 reg;

	reg = readl(base_addr + 0x8);
	while (reg != 0x00001111)
		reg = readl(base_addr + 0x8);
}
