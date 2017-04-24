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

#include <config.h>
#include <common.h>
#include <command.h>
#include <vsprintf.h>
#include <errno.h>
#include <i2c.h>
#include <mvebu/sar.h>

static int sar_initialized = 0;

int do_sar_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd = argv[1];
	const char *key = NULL;
	int value = 0;

	/* initialize sar driver variables from the device tree */
	if (!sar_initialized) {
		sar_init();
		sar_initialized = 1;
	}

	if (sar_is_available() == 0) {
		printf("Error: SAR variables not available for this board\n");
		return 1;
	}

	if ((strcmp(cmd, "read") == 0) && (argc < 2)) {
		printf("Error: Please specify SAR key\n");
		return 1;
	}

	if ((strcmp(cmd, "write") == 0) && (argc < 4)) {
		printf("Error: Please specify SAR key and value\n");
		return 1;
	}

	if (argc > 2)
		key = argv[2];
	if (argc > 3)
		value = (int)simple_strtoul(argv[3], NULL, 16);

	if (strcmp(cmd, "list") == 0) {
		if (argc < 3) {
			sar_list_keys();
		} else {
			if (sar_list_key_opts(key))
				return -EINVAL;
		}
	} else if (strcmp(cmd, "default") == 0) {
		if (argc < 3) {
			sar_defualt_all();
		} else {
			if (sar_default_key(key))
				return -EINVAL;
		}
	} else if (strcmp(cmd, "read") == 0) {
		if (!key)
			sar_read_all();
		else
			if (sar_print_key(key))
				return -EINVAL;
	} else if (strcmp(cmd, "write") == 0) {
		if (sar_write_key(key, value))
			return -EINVAL;
	} else {
		printf("ERROR: unknown command to sar: \"%s\"\n", cmd);
		return CMD_RET_USAGE;
	}

	return 0;
}

U_BOOT_CMD(
	sar,      6,     1,      do_sar_cmd,
	"sar - Modify SOC's sample at reset (SAR) values",
	"\n"
	"Modify SOC's sample at reset values\n"
	"\tlist		- Display all availble SAR variables\n"
	"\tlist <x>	- Display options for SAR variable x\n"
	"\tdefault	- Set all SAR variable to default value\n"
	"\tdefault <x>	- Set SAR variable x default value\n"
	"\twrite x y	- Write y to SAR variable x\n"
	"\tread		- Read all SAR variables\n"
	"\tread <x>	- Read SAR variable x\n"
);

U_BOOT_CMD(
	SatR,      6,     1,      do_sar_cmd,
	"SatR - Modify SOC's sample at reset (SAR) values",
	"\n"
	"Modify SOC's sample at reset values\n"
	"\tlist		- Display all availble SAR variables\n"
	"\tlist <x>	- Display options for SAR variable x\n"
	"\tdefault	- Set all SAR variable to default value\n"
	"\tdefault <x>	- Set SAR variable x default value\n"
	"\twrite x y	- Write y to SAR variable x\n"
	"\tread		- Read all SAR variables\n"
	"\tread <x>	- Read SAR variable x\n"
);
