/*
 * ***************************************************************************
 * Copyright (C) 2017 Marvell International Ltd.
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
#include <asm/arch-mvebu/soc.h>
#include <mvebu/mvebu_chip_sar.h>

#define RFU_GLOBAL_SW_RST		(MVEBU_RFU_BASE + 0x84)
#define RFU_SW_RESET_OFFSET		0

int soc_early_init_f(void)
{
#ifdef CONFIG_MVEBU_SAR
	/* Sample at reset register init */
	mvebu_sar_init(gd->fdt_blob);
#endif

	return 0;
}
