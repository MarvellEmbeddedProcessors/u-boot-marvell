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

#include <config.h>
#include <common.h>
#include <errno.h>
#include <asm/system.h>
#include <asm/bitops.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/adec.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/tables.h>


/* Weak function for boards who need specific init seqeunce */
int __soc_late_init(void)
{
	return 0;
}
int soc_late_init(void) __attribute__((weak, alias("__soc_late_init")));

u16 *soc_get_unit_mask_table(void)
{
	struct mvebu_soc_family *soc_family = get_soc_family();
	return soc_family->base_unit_info;
}

char **soc_get_mpp_desc_table(void)
{
	struct mvebu_soc_family *soc_family = get_soc_family();
	return soc_family->mpp_desc;
}

u32 *soc_get_mpp_update_mask(void)
{
	struct mvebu_soc_family *soc_family = get_soc_family();
	return soc_family->mpp_mask;
}
u32 *soc_get_mpp_update_val(void)
{
	struct mvebu_soc_family *soc_family = get_soc_family();
	return soc_family->mpp_update;
}
u32 *soc_get_mpp_protect_mask(void)
{
	struct mvebu_soc_family *soc_family = get_soc_family();
	return soc_family->mpp_protect;
}

struct mpp_bus *soc_get_mpp_bus(int bus_id)
{
	struct mvebu_soc_family *soc_family = get_soc_family();
	return soc_family->mpp_buses + bus_id;
}


static int update_soc_units(struct mvebu_soc_info *soc)
{
	u16 *unit_mask = soc_get_unit_mask_table();

	if (soc->unit_disable)
		update_unit_info(unit_mask, soc->unit_disable, UNIT_INFO_DISABLE);

	return 0;
}

int mvebu_soc_init()
{
	struct mvebu_soc_info *soc;
	struct mvebu_soc_family *soc_family;
	int soc_id, soc_rev;
	int ret;

	set_soc_family(soc_init());
	soc_family = get_soc_family();
	if (!soc_family)
		error("Failed to get SOC Family info\n");

	soc_id  = soc_get_id();
	soc_rev = soc_get_rev();
	debug("Current device ID  = %x\n", soc_id);
	debug("Current device Rev = %x\n", soc_rev);

	/* Find the exact SOC out of the family */
	soc = soc_family->soc_table;
	while (soc->id != 0) {
		if (soc->id == soc_id) {
			soc_family->curr_soc = soc;
			break;
		}
		soc++;
	}

	if (soc_family->curr_soc == NULL) {
		error("Can't find soc info %d", soc_id);
		return -ENODEV;
	}

	/* Store global variable to SOC */
	debug("Current device name = %s %s\n", soc->name, soc_family->rev_name[soc_rev]);

	/* Update SOC info according to family */
	update_soc_units(soc);

	/* Initialize physical memory map */
#ifndef CONFIG_PALLADIUM
	adec_init(soc->memory_map);
#endif

	/* Soc specific init */
	ret = soc_late_init();
	if (ret)
		error("SOC late init failed");

#ifdef CPU_RELEASE_ADDR
	*(unsigned long *)CPU_RELEASE_ADDR = 0;
#endif

	return 0;
}

