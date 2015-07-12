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
#include <asm/arch-armada38x/armada38x.h>

#define MBUS_SDRAM_WIN_ENABLE			0x1
#define MBUS_SDRAM_BASE_REG(win)		(MVEBU_ADEC_BASE + 0x180 + (win * 0x8))
#define MBUS_SDRAM_CTRL_REG(win)		(MVEBU_ADEC_BASE + 0x184 + (win * 0x8))
#define MBUS_SDRAM_SIZE_MASK			(0xFF << 24)
#define MBUS_SDRAM_SIZE_ALIGN			(1 << 24)

int soc_early_init_f(void)
{
	return 0;
}

struct mvebu_soc_family *soc_init(void)
{
	/* Do early SOC specific init here */

	/* Disable I/O error propogation. Error propogation
	 * causes exception when scanning a PCIe port with
	 * no device attached */
	writel(0, MVEBU_IO_ERR_CTL_REG);

	return &a38x_family_info;
}

int dram_init(void)
{
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

	return 0;
}

