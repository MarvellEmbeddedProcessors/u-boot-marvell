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
#include "../../board/mvebu/common/cfg_eeprom.h"
#include "../../board/mvebu/common/fdt_config.h"

int do_fdt_config_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd = argv[1];
	const char *fdt_option = NULL;
	const char *fdt_model = NULL;

	if ((strcmp(cmd, "select") == 0) && (argc < 3))
		return CMD_RET_USAGE;

	if (argc > 2)
		fdt_option = argv[2];

	if (argc > 3)
		fdt_model = argv[3];

	if (strcmp(cmd, "read") == 0) {
		if (argc < 4 && (strcmp(fdt_option, "eeprom") == 0)) {
			fdt_cfg_read_eeprom();
		} else if (argc > 3 && (strcmp(fdt_option, "flash") == 0)) {
			if (fdt_cfg_read_flash(fdt_model))
				return 1;
		} else
			return CMD_RET_USAGE;
	} else if (strcmp(cmd, "save") == 0) {
		cfg_eeprom_save();
	} else if (strcmp(cmd, "on") == 0) {
		if (fdt_cfg_on())
			return 1;
	} else if (strcmp(cmd, "off") == 0) {
		if (fdt_cfg_off())
			return 1;
	} else if (strcmp(cmd, "select") == 0) {
		if (fdt_select_set(fdt_option))
			return 1;
	} else if (strcmp(cmd, "list") == 0) {
		if (fdt_select_list())
			return 1;
	} else {
		return CMD_RET_USAGE;
	}

	return 0;
}

U_BOOT_CMD(
	fdt_config,    6,     1,      do_fdt_config_cmd,
	"fdt_config - Modify SOC and board configuration\n",
	"\n"
	"Modify SOC and board configuration\n"
	"\tread	eeprom	  - Read FDT from EEPROM and save to DRAM\n"
	"\tread flash <x> - Read x FDT from U-Boot and save to DRAM\n"
	"\tsave		  - Save FDT in EEPROM\n"
	"\toff		  - Disable the feature of loading the FDT that saved in EEPROM\n"
	"\ton		  - Enable the feature of loading the FDT that saved in EEPROM\n"
	"\tlist		  - Show the options of the board\n"
	"\tselect <x>	  - Update active FDT selection\n"
);
