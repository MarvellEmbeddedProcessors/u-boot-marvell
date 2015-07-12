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

struct mvebu_soc_info armadalp_soc_info[] = {
	/* Armada LP A */
	{
		.name = "MV-ARMADALP_A", /* temp SOC name and SOC ID */
		.id = 0x9991,
	},
	/* Armada LP B */
	{
		.name = "MV-ARMADALP_B", /* temp SOC name and SOC ID */
		.id = 0x9992,
	},
	/* Delimiter */
	{.id = 0}
};

struct mvebu_soc_family armadalp_family_info = {
	.name = "MV-99xx", /* temp SOC name and SOC ID */
	.id   = 9990,
	.rev_name = {"Z1", "A0"},
	.soc_table = armadalp_soc_info,
};
