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
#include <command.h>

#include <asm/arch-mvebu/adec.h>
#include <asm/arch-mvebu/unit-info.h>

int do_map_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	adec_dump();
	return 0;
}

U_BOOT_CMD(
	map,      1,     1,      do_map_cmd,
	"map	- Display address decode windows\n",
	"\n"
	"\tDisplay address decode windows\n"
);


int do_units_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd = argv[1];

	if (strcmp(cmd, "list") == 0)
		dump_unit_info();
	else
		printf("ERROR: unknown command to units: \"%s\"\n", cmd);

	return 0;
}

U_BOOT_CMD(
	units,      2,     1,      do_units_cmd,
	"units	- Display and modify active units\n",
	"\n"
	"Display and modufy units in current setup\n"
	"\tlist - Display current active units\n"
);
