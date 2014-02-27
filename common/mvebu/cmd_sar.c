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
#include <vsprintf.h>
#include <errno.h>

#include "../../board/mvebu/common/sar.h"

int do_sar_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd = argv[1];
	const char *key = NULL;
	int value = 0;

	if (sar_is_available() == 0) {
		printf("Error: SAR variables not available for this board\n");
		return 1;
	}

	if ((strcmp(cmd, "read") == 0) && (argc < 3)) {
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
		if (sar_print_key(key))
			return -EINVAL;
	} else if (strcmp(cmd, "write") == 0) {
		if (sar_write_key(key, value))
			return -EINVAL;
	} else {
		printf("ERROR: unknown command to SatR: \"%s\"\n", cmd);
		return -EINVAL;
	}

	return 0;
}

U_BOOT_CMD(
	SatR,      6,     1,      do_sar_cmd,
	"SatR - Modify SOC's sample at reset (SAR) values\n",
	"\n"
	"Modify SOC's sample at reset values\n"
	"\tlist		- Display all availble SAR variables\n"
	"\tlist <x>	- Display options for SAR variable x\n"
	"\tdefault	- Set all SAR variable to default value\n"
	"\tdefault <x>	- Set SAR variable x default value\n"
	"\twrite x y	- Write y to SAR variable x\n"
	"\tread x	- Read SAR variable x\n"
);
