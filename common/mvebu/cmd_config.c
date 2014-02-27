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

#include "../../board/mvebu/common/config.h"


struct var_desc cfg_desc_table[MAX_CFG] = {
	[BOARD_ID_CFG] =	{ "board_id", "Board ID of current board"},
	[SERDES_0_CFG] =	{ "serdes_0", "Configurations on SERDES lane 0"},
	[SERDES_1_CFG] =	{ "serdes_1", "Configurations on SERDES lane 1"},
};

static int cmd_cfg_key_to_id(const char *key)
{
	int id = -1;

	for (id = 0; id < MAX_CFG; id++) {
		if (strcmp(key, cfg_desc_table[id].key) == 0)
			return id;
	}

	if (id  == -1) {
		printf("Satr: Error: Unknown key \"%s\"\n", key);
		return -1;
	}
	if (cfg_is_var_active(id) == 0) {
		printf("Satr: Error: Key \"%s\" is inactive on this board\n", key);
		return -1;
	}
	return id;
}

static void cmd_cfg_list_var(int id, bool print_opts)
{
	int cnt;
	struct var_opts *opts;

	printf("%-10s %s\n", cfg_desc_table[id].key,
	       cfg_desc_table[id].description);

	if (print_opts) {
		opts = cfg_get_var_opts(id, &cnt);
		while (cnt--) {
			printf("\t0x%-2x %s ", opts->value, opts->desc);
			if (opts->flags & VAR_IS_DEFUALT)
				printf("[Default]");
			printf("\n");
			opts++;
		}
	}
}

static int cmd_cfg_print_key(const char *key)
{
	int id = cmd_cfg_key_to_id(key);
	struct var_opts *opts;
	char *desc = NULL;
	int val, ret, cnt;

	if (id == -1)
		return -EINVAL;

	ret = cfg_read_var(id, &val);
	if (ret)
		return ret;

	opts = cfg_get_var_opts(id, &cnt);
	while (cnt--) {
		if (opts->value == val)
			desc = opts->desc;
		opts++;
	}

	if (desc == NULL)
		printf("%s = 0x%x  ERROR: UNKNOWN OPTION\n", key, val);
	else
		printf("%s = 0x%x  %s\n", key, val, desc);

	return 0;
}

static void cmd_cfg_print_all(void)
{
	int id;

	printf("\n");
	for (id = 0; id < MAX_CFG; id++) {
		if (cfg_is_var_active(id))
			cmd_cfg_print_key(cfg_desc_table[id].key);
	}
	printf("\n");
}

static int cmd_cfg_write_key(const char *key, int val)
{
	int id = cmd_cfg_key_to_id(key);
	struct var_opts *opts;
	char *desc = NULL;
	int cnt;

	if (id == -1)
		return -EINVAL;

	opts = cfg_get_var_opts(id, &cnt);
	while (cnt--) {
		if (opts->value == val)
			desc = opts->desc;
		opts++;
	}

	if (desc == NULL) {
		printf("ERROR: value 0x%x not supported for key %s\n", val, key);
		printf("use \"SatR list %s\" to print supported values\n", key);
	}

	if (cfg_write_var(id, val))
		return -1;

	/* Display the updated variable */
	cmd_cfg_print_key(key);

	return 0;
}

static int cmd_cfg_default_all(void)
{
	int id;
	int ret = 0;

	cfg_default_all();

	for (id = 0; id < MAX_CFG; id++) {
		if (cfg_is_var_active(id))
			cmd_cfg_print_key(cfg_desc_table[id].key);
	}

	return ret;
}

static int cmd_cfg_default_key(const char *key)
{
	int id = cmd_cfg_key_to_id(key);
	int ret;

	if (id == -1)
		return -EINVAL;

	ret = cfg_default_var(id);
	if (ret)
		return ret;

	/* Display the updated variable */
	cmd_cfg_print_key(key);

	return 0;
}

static void cmd_cfg_list_all(void)
{
	int id;

	printf("\n");
	for (id = 0; id < MAX_CFG; id++) {
		if (cfg_is_var_active(id))
			cmd_cfg_list_var(id, 0);
	}
	printf("\n");
}

static int cmd_cfg_list_key(const char *key)
{
	int id = cmd_cfg_key_to_id(key);

	if (id == -1)
		return -EINVAL;

	printf("\n");
	cmd_cfg_list_var(id, 1);
	printf("\n");

	return 0;
}

int do_config_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd = argv[1];
	const char *key = NULL;
	int value = 0;

	if (cfg_is_available() == 0) {
		printf("Error: Config variables not available for this board\n");
		return 1;
	}

	if ((strcmp(cmd, "write") == 0) && (argc < 4)) {
		printf("Error: Please specify config key and value\n");
		return 1;
	}

	if (argc > 2)
		key = argv[2];
	if (argc > 3)
		value = (int)simple_strtoul(argv[3], NULL, 16);

	if (strcmp(cmd, "list") == 0) {
		if (argc < 3) {
			cmd_cfg_list_all();
		} else {
			if (cmd_cfg_list_key(key))
				return -EINVAL;
		}
	} else if (strcmp(cmd, "default") == 0) {
		if (argc < 3) {
			cmd_cfg_default_all();
		} else {
			if (cmd_cfg_default_key(key))
				return -EINVAL;
		}
	} else if (strcmp(cmd, "read") == 0) {
		if (argc < 3) {
			cmd_cfg_print_all();
		} else {
			if (cmd_cfg_print_key(key))
				return -EINVAL;
		}
	} else if (strcmp(cmd, "write") == 0) {
		if (cmd_cfg_write_key(key, value))
			return -EINVAL;
	} else {
		printf("ERROR: unknown command to config: \"%s\"\n", cmd);
		return -EINVAL;
	}

	return 0;
}

U_BOOT_CMD(
	config,    6,     1,      do_config_cmd,
	"config - Modify SOC and board configuration\n",
	"\n"
	"Modify SOC and board configuration\n"
	"\tlist		- Display all availble config variables\n"
	"\tlist <x>	- Display options for config variable x\n"
	"\tdefault	- Set all config variable to default value\n"
	"\tdefault <x>	- Set config variable x default value\n"
	"\tread		- Read all config variables\n"
	"\tread <x>	- Read config variable x\n"
	"\twrite <x> <y>- Write y to config variable x\n"
);
