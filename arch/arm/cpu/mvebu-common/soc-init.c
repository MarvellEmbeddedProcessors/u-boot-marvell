/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#define DEBUG

#include <config.h>
#include <common.h>
#include <errno.h>
#include <asm/system.h>
#include <asm/bitops.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/adec.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/tables.h>

struct mvebu_soc_family *soc_family;

static struct mvebu_soc_info *get_soc_info(int soc_id)
{
	struct mvebu_soc_info *soc = soc_family->soc_table;

	/* Avoid searching on each access */
	if (soc_family->curr_soc)
		return soc_family->curr_soc;

	while (soc->id != 0) {
		if (soc->id == soc_id) {
			soc_family->curr_soc = soc;
			break;
		}
		soc++;
	}

	if (soc_family->curr_soc == NULL)
		printf("Error: %s: Can't find soc info %d\n", __func__, soc_id);

	return soc_family->curr_soc;
}

u16 *soc_get_unit_mask_table(void)
{
	return soc_family->base_unit_info;
}

char *soc_get_mpp_desc_table(void)
{
	return soc_family->mpp_desc;
}


static int update_soc_units(int soc_id)
{
	struct mvebu_soc_info *soc = get_soc_info(soc_id);
	u16 *unit_mask = soc_get_unit_mask_table();

	if (soc->unit_disable)
		update_unit_info(unit_mask, soc->unit_disable, UNIT_INFO_DISABLE);

	return 0;
}

static int soc_init_memory_map(int soc_id)
{
	struct mvebu_soc_info *soc_info = get_soc_info(soc_id);
	struct adec_win *memory_map = soc_info->memory_map;

	if (soc_family->adec_type == 0) {
		adec_ap_init(memory_map);
	} else if (soc_family->adec_type == 1) {
		printf(" Error: No MBUS support yet\n");
		return -EINVAL;
	}

	return 0;
}

int common_soc_init(struct mvebu_soc_family *soc_family_info)
{
	int soc_id = soc_get_id();
	int ret;

	soc_family = soc_family_info;

	update_soc_units(soc_id);

	ret = soc_init_memory_map(soc_id);
	if (ret)
		return ret;

	return 0;
}
