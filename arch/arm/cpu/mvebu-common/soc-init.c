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

#include <config.h>
#include <common.h>
#include <errno.h>
#include <asm/system.h>
#include <asm/bitops.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/tables.h>

#ifdef CONFIG_MVEBU_CCU
#include <asm/arch-mvebu/ccu.h>
#endif
#ifdef CONFIG_MVEBU_RFU
#include <asm/arch-mvebu/rfu.h>
#endif
#ifdef CONFIG_MVEBU_IOB
#include <asm/arch-mvebu/iob.h>
#endif
#ifdef CONFIG_MVEBU_MBUS
#include <asm/arch-mvebu/mbus.h>
#endif

/* Weak function for boards who need specific init seqeunce */
int __soc_late_init(void)
{
	return 0;
}
int soc_late_init(void) __attribute__((weak, alias("__soc_late_init")));

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

	/* Initialize physical memory map */
#ifdef CONFIG_MVEBU_CCU
	init_ccu();
#endif
#ifdef CONFIG_MVEBU_RFU
	init_rfu();
#endif
#ifdef CONFIG_MVEBU_IOB
	init_iob();
#endif
#ifdef CONFIG_MVEBU_MBUS
	init_mbus();
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

