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

#include "chip_sar.h"

#define DDR_PLL_ID	0
#define RING_PLL_ID	1
#define CPU_PLL_ID	2

/* SAR AP806 registers */
#define SAR_DUAL_AP_MODE_OFFSET		0
#define SAR_DUAL_AP_MODE_MASK		(0x1 << SAR_DUAL_AP_MODE_OFFSET)
#define SAR_I2C_INIT_EN_OFFSET		1
#define SAR_I2C_INIT_EN_MASK		(0x1 << SAR_I2C_INIT_EN_OFFSET)
#define SAR_BOOT_SRC_OFFSET		2
#define SAR_BOOT_SRC_MASK		(0x3 << SAR_BOOT_SRC_OFFSET)
#define SAR_C2C_IHB_OFFSET		4
#define SAR_C2C_IHB_MASK		(0x1 << SAR_C2C_IHB_OFFSET)
#define SAR_EFUSE_BYPASS_OFFSET		7
#define SAR_EFUSE_BYPASS_MASK		(0x1 << SAR_EFUSE_BYPASS_OFFSET)
#define SAR_STAND_ALONE_AP_MODE_OFFSET	8
#define SAR_STAND_ALONE_AP_MODE_MASK	(0x1 << SAR_STAND_ALONE_AP_MODE_OFFSET)
#define SAR_SLAVE_AP_MODE_OFFSET	9
#define SAR_SLAVE_AP_MODE_MASK		(0x1 << SAR_SLAVE_AP_MODE_OFFSET)
#define SAR_AVS_MASTER_OFFSET		11
#define SAR_AVS_MASTER_MASK		(0x1 << SAR_AVS_MASTER_OFFSET)

#define AP806_SAR_1_REG			4
#define SAR1_PLL2_OFFSET		(15)
#define SAR1_PLL2_MASK			(0x7 << SAR1_PLL2_OFFSET)
#define SAR1_PLL1_OFFSET		(18)
#define SAR1_PLL1_MASK			(0x7 << SAR1_PLL1_OFFSET)
#define SAR1_PLL0_OFFSET		(21)
#define SAR1_PLL0_MASK			(0x7 << SAR1_PLL0_OFFSET)
#define SAR1_STANDALONE_OFFSET		(24)
#define SAR1_STANDALONE_MASK		(1 << SAR1_STANDALONE_OFFSET)

struct sar_info {
	char *name;
	u32 offset;
	u32 mask;
};

struct sar_info ap806_sar_0[] = {
	{"Dual AP mode       ", SAR_DUAL_AP_MODE_OFFSET, SAR_DUAL_AP_MODE_MASK },
	{"I2C init Enable    ", SAR_I2C_INIT_EN_OFFSET, SAR_I2C_INIT_EN_MASK },
	{"Boot Source        ", SAR_BOOT_SRC_OFFSET, SAR_BOOT_SRC_MASK },
	{"C2C IHB select     ", SAR_C2C_IHB_OFFSET, SAR_C2C_IHB_MASK },
	{"EFUSE ByPass       ", SAR_EFUSE_BYPASS_OFFSET, SAR_EFUSE_BYPASS_MASK },
	{"Stand Alone AP mode", SAR_STAND_ALONE_AP_MODE_OFFSET, SAR_STAND_ALONE_AP_MODE_MASK},
	{"Slave AP mode      ", SAR_SLAVE_AP_MODE_OFFSET, SAR_SLAVE_AP_MODE_MASK },
	{"AVS master         ", SAR_AVS_MASTER_OFFSET, SAR_AVS_MASTER_MASK },
	{"",			-1,			-1},
};

struct sar_info ap806_sar_1[] = {
	{"Standalone AP      ", SAR1_STANDALONE_OFFSET, SAR1_STANDALONE_MASK},
	{"PLL0 Config        ", SAR1_PLL0_OFFSET, SAR1_PLL0_MASK },
	{"PLL1 Config        ", SAR1_PLL1_OFFSET, SAR1_PLL1_MASK },
	{"PLL2 Config        ", SAR1_PLL2_OFFSET, SAR1_PLL2_MASK },
	{"",			-1,			-1},
};

static void __iomem *__attribute__((section(".data")))sar_base;

static u32 sar_get_pll_val(u32 index)
{
	void __iomem *reg = sar_base + AP806_SAR_1_REG;

	switch (index) {
	case 0:
		return (readl(reg) & SAR1_PLL0_MASK) >> SAR1_PLL0_OFFSET;
	case 1:
		return (readl(reg) & SAR1_PLL1_MASK) >> SAR1_PLL1_OFFSET;
	case 2:
		return (readl(reg) & SAR1_PLL2_MASK) >> SAR1_PLL2_OFFSET;
	default:
		error("sar regs: unsuported PLL index %d", index);
	}

	return -1;
}


static const u32 pll_freq_tbl[7][5] = {
	/* DDR */   /* Ring */ /* CPU */ /* PIDI-AP */  /* PIDI-CP */
	{1.2  * GHz, 2.0 * GHz, 2.5 * GHz, 2 * GHz,	2 * GHz},
	{1.05 * GHz, 1.8 * GHz, 2.2 * GHz, 1 * GHz,	1 * GHz},
	{900  * MHz, 1.6 * GHz, 2.0 * GHz, 1 * GHz,	1 * GHz},
	{800  * MHz, 1.4 * GHz, 1.7 * GHz,    0,	   0},
	{650  * MHz, 1.2 * GHz, 1.6 * GHz,    0,	   0},
	{650  * MHz, 1.2 * GHz, 1.2 * GHz,    0,	   0},
	{650  * MHz, 1.2 * GHz, 1.2 * GHz,    0,	   0}
};


int ap806_sar_value_get(enum mvebu_sar_opts sar_opt, struct sar_val *val)
{
	u32 pll_idx, pll_val;

	switch (sar_opt) {
	case(SAR_CPU_FREQ):
		pll_idx = CPU_PLL_ID;
		break;
	case(SAR_DDR_FREQ):
		pll_idx = DDR_PLL_ID;
		break;
	case(SAR_AP_FABRIC_FREQ):
		pll_idx = RING_PLL_ID;
		break;
	default:
		error("AP806-SAR: Unsupported SAR option %d.\n", sar_opt);
		return -EINVAL;
	}
	pll_val = sar_get_pll_val(pll_idx);
	val->raw_sar_val = pll_val;
	val->freq = pll_freq_tbl[pll_val][pll_idx];
	return 0;
}

static int ap806_sar_dump(void)
{
	u32 reg, val;
	struct sar_info *sar;

	reg = readl(sar_base);
	printf("AP806 SAR register 0 [0x%08x]:\n", reg);
	printf("----------------------------------\n");
	sar = ap806_sar_0;
	while (sar->offset != -1) {
		val = (reg & sar->mask) >> sar->offset;
		printf("%s  0x%x\n", sar->name, val);
		sar++;
	}

	reg = readl(sar_base + AP806_SAR_1_REG);
	printf("\nAP806 SAR register 1 [0x%08x]:\n", reg);
	printf("----------------------------------\n");
	sar = ap806_sar_1;
	while (sar->offset != -1) {
		val = (reg & sar->mask) >> sar->offset;
		printf("%s  0x%x\n", sar->name, val);
		sar++;
	}
	return 0;
}



int ap806_sar_init(const void *blob, int node)
{
	uintptr_t chip_id;
	int ret, i;
	struct sar_chip_info info;

	u32 sar_list[] = {
		SAR_CPU_FREQ,
		SAR_DDR_FREQ,
		SAR_AP_FABRIC_FREQ
	};

	sar_base = fdt_get_regs_offs(blob, node, "reg");

	info.sar_dump_func = ap806_sar_dump;
	info.sar_value_get_func = ap806_sar_value_get;

	ret = mvebu_sar_chip_register(COMPAT_MVEBU_SAR_REG_AP806, &info, &chip_id);
	if (ret) {
		error("Failed to register AP806 SAR functions.\n");
		return ret;
	}

	for (i = 0; i < ARRAY_SIZE(sar_list); i++) {
		ret = mvebu_sar_id_register(chip_id, sar_list[i]);
		if (ret) {
			error("Failed to register SAR %d, for AP806.\n", sar_list[i]);
			return ret;
		}
	}

	return 0;
}

