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

#define DEBUG

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-armadalp/armadalp.h>

int soc_early_init_f(void)
{
	return 0;
}

int soc_get_rev(void)
{
	/* This should read the soc rev from some register*/
	return 0;
}

int soc_get_id(void)
{
	/* TO-DO, get soc ID from PCIe register */
	/* in ArmadaLP, there is no device ID register, like A38x,
	    it needs to be got from PCIe register, like A370 and AXP */
	u32 id = 0x9991;
	return id;
}

void soc_init(void)
{
	/* Do early SOC specific init here */

	return;
}

int dram_init(void)
{
#ifdef CONFIG_PALLADIUM
	/* NO DRAM init sequence in Pallaidum, so set static DRAM size of 256MB */
	gd->ram_size = 0x20000000;
#else
	int cs;
	u32 ctrl, size, base;

	gd->ram_size = 0;
	/*
	* We assume the DDR training code has configured
	* The SDRAM adec windows so we pull our info from there
	*/

	for (cs = 0; cs < CONFIG_NR_DRAM_BANKS; cs++) {
		ctrl = readl(MBUS_SDRAM_CTRL_REG(cs));
		if (ctrl & MBUS_SDRAM_WIN_ENABLE) {
			base = readl(MBUS_SDRAM_BASE_REG(cs));
			size = (ctrl & MBUS_SDRAM_SIZE_MASK) + MBUS_SDRAM_SIZE_ALIGN;
			gd->bd->bi_dram[cs].start = base;
			gd->bd->bi_dram[cs].size = size;

			gd->ram_size += size;

			debug("DRAM bank %d base 0x%08x size 0x%x ", cs, base, size);
		}
	}

	if (gd->ram_size == 0) {
		error("No DRAM banks detected");
		return 1;
	}
#endif

	return 0;
}
