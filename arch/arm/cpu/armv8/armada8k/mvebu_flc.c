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

/*#define DEBUG*/
/*#define DEBUG_REG*/

#include <common.h>
#include <asm/system.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/flc.h>

DECLARE_GLOBAL_DATA_PTR;

/* FLC defines */
#define MMAP_FLC_OFFSET				(flc_base + 0x0)
#define MMAP_FLC_HIGH_OFFSET		(flc_base + 0x4)
#define MMAP_NC_FLC_OFFSET			(flc_base + 0x8)
#define MMAP_NC_FLC_HIGH_OFFSET		(flc_base + 0xc)

#define FLC_ENABLE_BIT			(0x1)

/* low base address [31:23] */
#define FLC_ADDRESS_MASK	(0xFF800000)

#define FLC_SIZE_OFFSET		(16)
#define FLC_SIZE_MASK		(0x1F)

struct flc_map {
	u32 base_addr_high;
	u32 base_addr_low;
	u32 flc_size_high;
	u32 flc_size_low;
};

char *mapping_flc_size[] = {
	"Reserved", /* 0x0 */
	"Reserved", /* 0x1 */
	"Reserved", /* 0x2 */
	"Reserved", /* 0x3 */
	"Reserved", /* 0x4 */
	"Reserved", /* 0x5 */
	"Reserved", /* 0x6 */
	"8 MB",     /* 0x7 */
	"16 MB",    /* 0x8 */
	"32 MB",    /* 0x9 */
	"64 MB",    /* 0xA */
	"128 MB",   /* 0xB */
	"256 MB",   /* 0xC */
	"512 MB",   /* 0xD */
	"1 GB",     /* 0xE */
	"2 GB",     /* 0xF */
	"4 GB",     /* 0x10 */
	"8 GB",     /* 0x11 */
	"16 GB",    /* 0x12 */
	"32 GB",    /* 0x13 */
	"64 GB",    /* 0x14 */
	"128 GB",   /* 0x15 */
	"256 GB",   /* 0x16 */
	"512 GB",   /* 0x17 */
	"1 TB",     /* 0x18 */
	"Reserved", /* 0x19 */
	"Reserved", /* 0x1A */
	"Reserved", /* 0x1B */
	"Reserved", /* 0x1C */
	"Reserved", /* 0x1D */
	"Reserved", /* 0x1E */
	"Reserved", /* 0x1F */
};

static int get_flc_size(u64 size)
{
	switch (size) {
	case 0x800000:
		return 7;
	case 0x1000000:
		return 8;
	case 0x2000000:
		return 9;
	case 0x4000000:
		return 0xa;
	case 0x8000000:
		return 0xb;
	case 0x10000000:
		return 0xc;
	case 0x20000000:
		return 0xd;
	case 0x40000000:
		return 0xe;
	case 0x80000000:
		return 0xf;
	case 0x100000000:
		return 0x10;
	case 0x200000000:
		return 0x11;
	case 0x400000000:
		return 0x12;
	case 0x800000000:
		return 0x13;
	case 0x1000000000:
		return 0x14;
	case 0x2000000000:
		return 0x15;
	case 0x4000000000:
		return 0x16;
	case 0x8000000000:
		return 0x17;
	case 0x10000000000:
		return 0x18;
	default:
		error("No valid size, size must be a power of 2 between 8 MB to 1 TB");
	}

	return -1;
}

void __iomem *flc_base;

static void print_flc_mmap_reg_val(u32 reg_val, u32 reg_val_high)
{
	u64 start_addr;
	u32 alr;

	if (!(reg_val & FLC_ENABLE_BIT)) {
		printf("\nEntry disable\n");
		return;
	}

	alr = reg_val & FLC_ADDRESS_MASK;
	start_addr = alr | ((u64)reg_val_high << 32);

	printf("0x%016llx   %s\n", start_addr,
	       mapping_flc_size[(reg_val >> FLC_SIZE_OFFSET) & FLC_SIZE_MASK]);
}

static u32 get_flc_mmap_reg_val(struct flc_map *memory_map)
{
	u32 reg_val;
	int     size;

	size = get_flc_size(memory_map->flc_size_low |
						((u64)memory_map->flc_size_high  << 32));

	reg_val = FLC_ENABLE_BIT;
	reg_val |= (memory_map->base_addr_low & FLC_ADDRESS_MASK);
	reg_val |= size << FLC_SIZE_OFFSET;

	return reg_val;
}


void dump_flc(void)
{
	/* Dump all FLC mapping */
	printf("FLC MMAP     start                 size\n");
	printf("-----------------------------------------\n");

	printf("FLC        ");
	print_flc_mmap_reg_val(readl(MMAP_FLC_OFFSET), readl(MMAP_FLC_HIGH_OFFSET));
	printf("\nNC FLC     ");
	print_flc_mmap_reg_val(readl(MMAP_NC_FLC_OFFSET), readl(MMAP_NC_FLC_HIGH_OFFSET));

	return;
}

int init_flc(void)
{
	struct flc_map memory_map;
	const void *blob = gd->fdt_blob;
	u32 node, array_count, size;

	debug_enter();
	debug("Initializing FLC\n");

	/* Get FLC node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_FLC));
	if (node < 0) {
		debug("No FLC node found in FDT blob\n");
		return -1;
	}
	/* Get the base address of the FLC */
	flc_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	size = sizeof(struct flc_map) / sizeof(u32);

	/* Get the FLC non-cacheable parameters */
	array_count = fdtdec_get_int_array_count(blob, node, "flc_nc_map", (u32 *)&memory_map, size);
	if (array_count != -FDT_ERR_NOTFOUND) {
		/* FLC MMAP NC is Optional */
		writel(memory_map.base_addr_high, MMAP_NC_FLC_HIGH_OFFSET);
		writel(get_flc_mmap_reg_val(&memory_map), MMAP_NC_FLC_OFFSET);
	}

	/* Get the FLC mapping parameters */
	array_count = fdtdec_get_int_array_count(blob, node, "flc_ext_dev_map", (u32 *)&memory_map, size);
	if (array_count == -FDT_ERR_NOTFOUND) {
		error("no flc_ext_dev_map found\n");
		return 0;
	}

	/* set the flc map */
	writel(memory_map.base_addr_high, MMAP_FLC_HIGH_OFFSET);
	writel(get_flc_mmap_reg_val(&memory_map), MMAP_FLC_OFFSET);

	debug("Done FLC Initializing\n");
	debug_exit();

	return 0;
}
