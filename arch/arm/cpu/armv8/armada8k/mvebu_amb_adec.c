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

/* #define DEBUG */
/* #define DEBUG_REG */

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/amb_adec.h>

/* common defines */
#define WIN_ENABLE_BIT			(0x1)
/* Physical address of the base of the window = {AddrLow[19:0],20`h0} */
#define ADDRESS_SHIFT			(20 - 4)

/* AMB registers */
#define AMB_MAX_WIN_ID			(7)

#define AMB_WIN_CR_OFFSET(base, win)	(base + 0x0 + (0x8 * win))
#define AMB_ATTR_OFFSET			(8)
#define AMB_ATTR_MASK			(0xFF)
#define AMB_SIZE_OFFSET			16
#define AMB_SIZE_MASK			0xFF

#define AMB_WIN_BASE_OFFSET(base, win)	(base + 0x4 + (0x8 * win))
#define AMB_BASE_OFFSET			(16)

#define AMB_WIN_ALIGNMENT_64K		(0x10000)

DECLARE_GLOBAL_DATA_PTR;

enum amb_attribute_ids {
	AMB_SPI0_CS0_ID = 0x1E,
	AMB_SPI0_CS1_ID = 0x5E,
	AMB_SPI0_CS2_ID = 0x9E,
	AMB_SPI0_CS3_ID = 0xDE,
	AMB_SPI1_CS0_ID = 0x1A,
	AMB_SPI1_CS1_ID = 0x5A,
	AMB_SPI1_CS2_ID = 0x9A,
	AMB_SPI1_CS3_ID = 0xDA,
	AMB_DEV_CS0_ID  = 0x3E,
	AMB_DEV_CS1_ID  = 0x3D,
	AMB_DEV_CS2_ID  = 0x3B,
	AMB_DEV_CS3_ID  = 0x37,
	AMB_BOOT_CS_ID  = 0x2f,
	AMB_BOOT_ROM_ID = 0x1D,
	AMB_MAX_TID,
	INVALID_TID     = 0xFF
};

struct amb_attribute_name_map {
	enum amb_attribute_ids attr_id;
	char name[10];
};

struct amb_attribute_name_map amb_attribute_name_table[] = {
	{AMB_SPI0_CS0_ID,   "SPI0_CS0"},
	{AMB_SPI0_CS1_ID,   "SPI0_CS1"},
	{AMB_SPI0_CS2_ID,   "SPI0_CS2"},
	{AMB_SPI0_CS3_ID,   "SPI0_CS3"},
	{AMB_SPI1_CS0_ID,   "SPI1_CS0"},
	{AMB_SPI1_CS1_ID,   "SPI1_CS1"},
	{AMB_SPI1_CS2_ID,   "SPI1_CS2"},
	{AMB_SPI1_CS3_ID,   "SPI1_CS3"},
	{AMB_DEV_CS0_ID,    "DEV_CS0 "},
	{AMB_DEV_CS1_ID,    "DEV_CS1 "},
	{AMB_DEV_CS2_ID,    "DEV_CS2 "},
	{AMB_DEV_CS3_ID,    "DEV_CS3 "},
	{AMB_BOOT_CS_ID,    "BOOT_CS "},
	{AMB_BOOT_ROM_ID,   "BOOT_ROM"},
	{INVALID_TID,       "INVALID "},
};

static char *amb_attribute_name_get(enum amb_attribute_ids attr_id)
{
	int i;

	for (i = 0; i < AMB_MAX_TID; i++)
		if (amb_attribute_name_table[i].attr_id == attr_id)
			return amb_attribute_name_table[i].name;
	return amb_attribute_name_get(INVALID_TID);
}

void dump_amb_adec(void)
{
	const void *blob = gd->fdt_blob;
	void __iomem *amb_base;
	u32 ctrl, base, win_id, attr, node;
	uint32_t size, size_count;

	/* Get address decoding node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_AMB_ADEC));
	if (node < 0) {
		error("No AMB address decoding node found in FDT blob\n");
		return;
	}
	/* Get the base address of the address decoding MBUS */
	amb_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	/* Dump all AMB windows */
	printf("bank  attribute   base          size\n");
	printf("--------------------------------------------\n");
	for (win_id = 0; win_id < AMB_MAX_WIN_ID; win_id++) {
		ctrl = readl(AMB_WIN_CR_OFFSET(amb_base, win_id));
		if (ctrl & WIN_ENABLE_BIT) {
			base = readl(AMB_WIN_BASE_OFFSET(amb_base, win_id));
			attr = (ctrl >> AMB_ATTR_OFFSET) & AMB_ATTR_MASK;
			size_count = (ctrl >> AMB_SIZE_OFFSET) & AMB_SIZE_MASK;
			size = (size_count + 1) * AMB_WIN_ALIGNMENT_64K;
			printf("amb   %s    0x%08x    0x%08x\n", amb_attribute_name_get(attr), base, size);
		}
	}

	return;
}
