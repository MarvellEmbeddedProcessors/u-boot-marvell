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
#include <common.h>
#include <asm/io.h>
#include <errno.h>
#include <libfdt.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/clock.h>
#include <mvebu_chip_sar.h>
#include <asm/arch-mvebu/mvebu.h>

#include "chip_sar.h"

/* SAR CP110 registers */
#define SAR1_RST_PCIE0_CLOCK_CONFIG_CP0_OFFSET	(2)
#define SAR1_RST_PCIE0_CLOCK_CONFIG_CP0_MASK	(0x1 << SAR1_RST_PCIE0_CLOCK_CONFIG_CP0_OFFSET)
#define SAR1_RST_PCIE1_CLOCK_CONFIG_CP0_OFFSET	(3)
#define SAR1_RST_PCIE1_CLOCK_CONFIG_CP0_MASK	(0x1 << SAR1_RST_PCIE1_CLOCK_CONFIG_CP0_OFFSET)

#define SAR1_RST_BOOT_MODE_AP_CP0_OFFSET	(4)
#define SAR1_RST_BOOT_MODE_AP_CP0_MASK		(0x3f << SAR1_RST_BOOT_MODE_AP_CP0_OFFSET)

static void __iomem *__attribute__((section(".data")))sar_base;

struct sar_info {
	char *name;
	u32 offset;
	u32 mask;
};

struct sar_info cp110_sar_0[] = {
	{"PCIE0 clock config   ", SAR1_RST_PCIE0_CLOCK_CONFIG_CP0_OFFSET, SAR1_RST_PCIE0_CLOCK_CONFIG_CP0_MASK},
	{"PCIE1 clock config   ", SAR1_RST_PCIE1_CLOCK_CONFIG_CP0_OFFSET, SAR1_RST_PCIE1_CLOCK_CONFIG_CP0_MASK},
	{"Reset Boot Mode     ", SAR1_RST_BOOT_MODE_AP_CP0_OFFSET, SAR1_RST_BOOT_MODE_AP_CP0_MASK },
	{"",			-1,			-1},
};

struct bootsrc_idx_info {
	int start;
	int end;
	enum mvebu_bootsrc_type src;
	int index;
};

static struct bootsrc_idx_info bootsrc_list[] = {
	{0x0,	0x5,	BOOTSRC_NOR,		0},
	{0xA,	0x25,	BOOTSRC_NAND,		0},
	{0x28,	0x28,	BOOTSRC_AP_SD_EMMC,	0},
	{0x29,	0x29,	BOOTSRC_SD_EMMC,	0},
	{0x2A,	0x2A,	BOOTSRC_AP_SD_EMMC,	0},
	{0x2B,	0x2B,	BOOTSRC_SD_EMMC,	0},
	{0x30,	0x30,	BOOTSRC_AP_SPI,		0},
	{0x31,	0x31,	BOOTSRC_AP_SPI,		0}, /* BootRom disabled */
	{0x32,	0x33,	BOOTSRC_SPI,		1},
	{0x34,	0x35,	BOOTSRC_SPI,		0},
	{0x36,	0x37,	BOOTSRC_SPI,		1}, /* BootRom disabled */
	{-1,	-1,	-1}
};

int cp110_sar_bootsrc_get(enum mvebu_sar_opts sar_opt, struct sar_val *val)
{
	u32 reg, mode;
	int i;
	debug_enter();
	reg = readl(sar_base);
	mode = (reg & SAR1_RST_BOOT_MODE_AP_CP0_MASK) >> SAR1_RST_BOOT_MODE_AP_CP0_OFFSET;

	val->raw_sar_val = mode;

	i = 0;
	while (bootsrc_list[i].start != -1) {
		if ((mode >= bootsrc_list[i].start) && (mode <= bootsrc_list[i].end)) {
			val->bootsrc.type = bootsrc_list[i].src;
			val->bootsrc.index = bootsrc_list[i].index;
			break;
		}
		i++;
	}

	if (bootsrc_list[i].start == -1) {
		error("Bad CP110 sample at reset mode (%d).\n", mode);
		return -EINVAL;
	}
	debug_exit();
	return 0;
}

int cp110_sar_value_get(enum mvebu_sar_opts sar_opt, struct sar_val *val)
{
	u32 reg, mode;

	debug_enter();
	reg = readl(sar_base);

	switch (sar_opt) {
	case SAR_BOOT_SRC:
		return cp110_sar_bootsrc_get(sar_opt, val);
	case SAR_CP_PCIE0_CLK:
		mode = (reg & SAR1_RST_PCIE0_CLOCK_CONFIG_CP0_MASK) >> SAR1_RST_PCIE0_CLOCK_CONFIG_CP0_OFFSET;
		val->raw_sar_val = mode;
		val->clk_direction = mode;
		break;
	case SAR_CP_PCIE1_CLK:
		mode = (reg & SAR1_RST_PCIE1_CLOCK_CONFIG_CP0_MASK) >> SAR1_RST_PCIE1_CLOCK_CONFIG_CP0_OFFSET;
		val->raw_sar_val = mode;
		val->clk_direction = mode;
		break;
	default:
		error("AP806-SAR: Unsupported SAR option %d.\n", sar_opt);
		return -EINVAL;
	}
	debug_exit();
	return 0;
}

static int cp110_sar_dump(void)
{
	u32 reg, val;
	struct sar_info *sar;

	reg = readl(sar_base);
	printf("\nCP110 SAR register 0 [0x%08x]:\n", reg);
	printf("----------------------------------\n");
	sar = cp110_sar_0;
	while (sar->offset != -1) {
		val = (reg & sar->mask) >> sar->offset;
		printf("%s  0x%x\n", sar->name, val);
		sar++;
	}
	return 0;
}

int cp110_sar_init(const void *blob, int node)
{
	uintptr_t chip_id;
	int ret, i;
	struct sar_chip_info info;

	debug_enter();

	u32 sar_list[] = {
		SAR_CP_PCIE0_CLK,
		SAR_CP_PCIE1_CLK,
		SAR_BOOT_SRC
	};

	sar_base = fdt_get_regs_offs(blob, node, "reg");

	info.sar_dump_func = cp110_sar_dump;
	info.sar_value_get_func = cp110_sar_value_get;

	ret = mvebu_sar_chip_register(COMPAT_MVEBU_SAR_REG_CP110, &info, &chip_id);
	if (ret) {
		error("Failed to register AP806 SAR functions.\n");
		return ret;
	}

	for (i = 0; i < ARRAY_SIZE(sar_list); i++) {
		ret = mvebu_sar_id_register(chip_id, sar_list[i]);
		if (ret) {
			error("Failed to register SAR %d, for CP110.\n", sar_list[i]);
			return ret;
		}
	}

	debug_exit();

	return 0;
}

