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
#include <command.h>
#include <vsprintf.h>
#include <errno.h>
#include <asm/arch-mvebu/fdt.h>
#include "../../board/mvebu/common/cfg_eeprom.h"
#include "../../board/mvebu/common/fdt_config.h"

int do_fdt_config_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd = argv[1];
	const char *fdt_option = NULL;
	uint8_t *fdt_blob;

	if (argc > 2)
		fdt_option = argv[2];

	if (strcmp(cmd, "load") == 0) {
		if (argc == 3) {
			if (fdt_cfg_load(fdt_option))
				return 1;
		} else {
			return CMD_RET_USAGE;
		}
	} else if (strcmp(cmd, "save") == 0) {
		fdt_blob = cfg_eeprom_get_fdt();
		fdt_cfg_save(fdt_blob);
	} else if (strcmp(cmd, "select") == 0) {
		if (argc < 3) {
			fdt_cfg_print_select();
		} else if (fdt_cfg_set_select(fdt_option)) {
			return 1;
		}
	} else if (strcmp(cmd, "list") == 0) {
		if (fdt_cfg_list())
			return 1;
	} else {
		return CMD_RET_USAGE;
	}

	return 0;
}

U_BOOT_CMD(
	fdt_config,    6,     1,      do_fdt_config_cmd,
	"Modify SOC and board FDT configuration\n",
	"\n"
	"Modify SOC and board configuration\n"
	"\tlist		  - List the available FDT: preset and customized FDT on EEPROM (if exist)\n"
	"\tselect [x]	  - Select active FDT to boot from\n\n"

	"To create modified FDT on EEPROM, use the following\n"
	"\tload <x>	  - Load FDT <x> to DRAM (loaded to fdt_addr env. variable)\n"
	"\tsave		  - Save FDT in EEPROM\n"
);
