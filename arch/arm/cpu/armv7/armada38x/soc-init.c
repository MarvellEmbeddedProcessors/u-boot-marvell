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

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/unit-info.h>
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

