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
#include "chip_sar.h"
#include <asm/arch-mvebu/mvebu.h>

DECLARE_GLOBAL_DATA_PTR;

#define SAR_MAX_CHIP	4

#define INDEX_TO_CHIP_ID(idx)	((idx) | 0x80)
#define CHIP_ID_TO_INDEX(cid)	((cid) & ~0x80)

struct sar_chip_info sar_chip_info[] = {
#ifdef CONFIG_TARGET_ARMADA_8K
	{.compat = COMPAT_MVEBU_SAR_REG_AP806,
	 .sar_init_func = ap806_sar_init,
	},
	{.compat = COMPAT_MVEBU_SAR_REG_CP110,
	 .sar_init_func = cp110_sar_init,
	},
#endif
	{.compat = COMPAT_UNKNOWN}
};

uintptr_t __attribute__((section(".data")))soc_sar_info[SAR_MAX_IDX];

static struct sar_chip_info *get_chip_config(enum fdt_compat_id compat)
{
	struct sar_chip_info *chip_config_ptr = sar_chip_info;

	while (chip_config_ptr->compat != COMPAT_UNKNOWN) {
		if (chip_config_ptr->compat == compat)
			return chip_config_ptr;
		chip_config_ptr++;
	}

	return NULL;
}


int mvebu_sar_chip_register(enum fdt_compat_id compat, struct sar_chip_info *info, uintptr_t *chip_id)
{
	struct sar_chip_info *chip_cfg_ptr;

	chip_cfg_ptr = get_chip_config(compat);
	if (chip_cfg_ptr == NULL) {
		error("Cannot find compat %s in sar chips list.\n", fdtdec_get_compatible(compat));
		return -ENXIO;
	}

	chip_cfg_ptr->sar_dump_func = info->sar_dump_func;
	chip_cfg_ptr->sar_value_get_func = info->sar_value_get_func;
	chip_cfg_ptr->sar_bootsrc_get = info->sar_bootsrc_get;

	if (chip_id)
		*chip_id = INDEX_TO_CHIP_ID(((uintptr_t)(chip_cfg_ptr - sar_chip_info)));

	return 0;
}


int mvebu_sar_id_register(uintptr_t chip_id, u32 sar_id)
{
	if (soc_sar_info[sar_id]) {
		error("sar %d was already registered.\n", sar_id);
		return -EBUSY;
	}

	soc_sar_info[sar_id] = chip_id;

	return 0;
}


int mvebu_sar_init(const void *blob)
{
	int node, sar_list[SAR_MAX_CHIP];
	int ret;
	u32 chip_count, i;
	struct sar_chip_info *chip_cfg_ptr;

	memset(soc_sar_info, 0, sizeof(soc_sar_info));
	chip_count = fdtdec_find_aliases_for_id(blob, "sar-reg",
			COMPAT_MVEBU_SAR_REG_COMMON, sar_list, SAR_MAX_CHIP);

	if (chip_count <= 0) {
		error("Cannot find sample-at-reset dt entry (%d).\n", chip_count);
		return -ENODEV;
	}

	for (i = 0; i < chip_count ; i++) {

		node = sar_list[i];
		if (node <= 0)
			continue;

		/* Skip if Node is disabled */
		if (!fdtdec_get_is_enabled(blob, node))
			continue;

		chip_cfg_ptr = get_chip_config(fdtdec_next_lookup(blob, node, COMPAT_MVEBU_SAR_REG_COMMON));
		if (chip_cfg_ptr == NULL) {
			error("Bad compatible for sar-reg.\n");
			continue;
		}

		ret = chip_cfg_ptr->sar_init_func(blob, node);
		if (ret) {
			error("sar_init failed (%d).\n", ret);
			return ret;
		}
	}
	return 0;
}

int mvebu_sar_value_get(enum mvebu_sar_opts opt, struct sar_val *val)
{
	u32 idx;
	struct sar_chip_info *chip_ptr;
	debug_enter();
	if (soc_sar_info[opt]) {
		idx = CHIP_ID_TO_INDEX(soc_sar_info[opt]);
		chip_ptr = &sar_chip_info[idx];
		return chip_ptr->sar_value_get_func(opt, val);
	}

	error("SAR - No chip registered on sar %d.\n", opt);
	debug_exit();
	return -ENODEV;
}

char *mvebu_sar_bootsrc_to_name(enum mvebu_bootsrc_type src)
{
	switch (src) {
	case(BOOTSRC_NAND):
		return "nand";
	case(BOOTSRC_SPI):
	case(BOOTSRC_AP_SPI):
		return "spi";
	case(BOOTSRC_SD_EMMC):
	case(BOOTSRC_AP_SD_EMMC):
		return "mmc";
	case(BOOTSRC_NOR):
		return "nor";
	default:
		return "unknown";
	}
}

void mvebu_sar_dump(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(sar_chip_info)-1; i++)
		sar_chip_info[i].sar_dump_func();

	return;
}


