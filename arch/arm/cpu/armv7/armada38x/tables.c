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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/regs-base.h>
#include <asm/arch-mvebu/soc.h>

struct mvebu_soc_info a38x_soc_info[] = {
	/* Armda 380 */
	{
		.name = "MV-6280",
		.id = 0x6820,
	},
	/* Armda 385 */
	{
		.name = "MV-6285",
		.id = 0x6825,
	},
	/* Armda 388 */
	{
		.name = "MV-6288",
		.id = 0x6828,
	},
	/* Delimiter */
	{.id = 0}
};

struct mvebu_soc_family a38x_family_info = {
	.name = "MV-80xx",
	.id   = 8000,
	.rev_name = {"Z1", "A0"},
	.soc_table = a38x_soc_info,
};
