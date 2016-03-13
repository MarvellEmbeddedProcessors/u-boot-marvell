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
#include <errno.h>
#include <asm/arch-mvebu/fdt.h>
#include "../../board/mvebu/common/cfg_eeprom.h"

/* load the HW configuration from cfg_eeprom module and dump them */
static int cmd_hw_info_dump(char *name)
{
	int idx;
	int hw_param_num;
	uchar hw_info_str[MVEBU_HW_INFO_LEN];
	struct hw_info_point_struct hw_info_point_array[HW_INFO_MAX_PARAM_NUM];

	/* need to set all to 0 for later string operation */
	memset(hw_info_str, 0, sizeof(hw_info_str));
	cfg_eeprom_get_hw_info_str(hw_info_str);

	hw_param_num = cfg_eeprom_parse_hw_info(hw_info_str, hw_info_point_array);

	/* it is possible the HW configuration is empty */
	if (hw_param_num == 0)
		return 0;

	if (hw_param_num > MVEBU_HW_INFO_LEN) {
		error("hw_info internal error, counter should not exceed %d\n", MVEBU_HW_INFO_LEN);
		return -EINVAL;
	}

	printf("\nname               value\n");
	printf("------------------------------------\n");
	for (idx = 0; idx < hw_param_num; idx++) {
		if (name) {
			if (strcmp(name, hw_info_point_array[idx].name) == 0) {
				printf("%-16s   %-s\n", hw_info_point_array[idx].name, hw_info_point_array[idx].value);
				break;
			}
		} else {
			printf("%-16s   %-s\n", hw_info_point_array[idx].name, hw_info_point_array[idx].value);
		}
	}

	return 0;
}

int cmd_hw_info_load(char *name, int silence)
{
	int idx;
	int hw_param_num;
	uchar hw_info_str[MVEBU_HW_INFO_LEN];
	struct hw_info_point_struct hw_info_point_array[HW_INFO_MAX_PARAM_NUM];

	/* get hw_info from system
	 * need to memset the hw_info to 0 for later string operation
	 */
	memset(hw_info_str, 0, sizeof(hw_info_str));
	cfg_eeprom_get_hw_info_str(hw_info_str);

	hw_param_num = cfg_eeprom_parse_hw_info(hw_info_str, hw_info_point_array);

	/* it is possible the HW configuration is empty */
	if (hw_param_num == 0)
		return 0;

	if (hw_param_num > MVEBU_HW_INFO_LEN) {
		error("Factory HW information: variables number from EEPROM should not exceed %d\n", MVEBU_HW_INFO_LEN);
		return -EINVAL;
	}

	/* save the HW parameter to env varibles one by one */
	for (idx = 0; idx < hw_param_num; idx++) {
		/* if customer input a specific and valid HW parameter name, only save this HW parameter
		 * from EEPROM to env variables.
		 * otherwise save all the HW parameters from EEPROM to env variables.
		 */
		if (name) {
			if (strcmp(name, hw_info_point_array[idx].name) == 0) {
				setenv(hw_info_point_array[idx].name, hw_info_point_array[idx].value);
				break;
			}
		} else {
			setenv(hw_info_point_array[idx].name, hw_info_point_array[idx].value);
		}
	}

	printf("HW information is loaded to enviroment variables\n");
	cmd_hw_info_dump(name);

	/* just print indication to ask user to perform saveenv manually in silence mode,
	 * which is used when restore the HW configuration to env variables with env reset.
	 * to ask confirmation that if need to save env in non-silence mode,
	 * which is used by hw_info cmd.
	 */
	if (silence) {
		printf("To save the changes, please run the command saveenv\n");
	} else {
		printf("Do you want to save enviroment variables? <y/N> ");
		if (confirm_yesno())
			saveenv();
		else
			printf("To save the changes, please run the command saveenv\n");
	}

	return 0;
}

static int cmd_hw_info_store(char *name)
{
	int idx;
	int str_len = 0;
	int total_str_len = 0;
	int hw_param_num;
	uchar hw_info_str[MVEBU_HW_INFO_LEN];
	struct hw_info_data_struct hw_info_data_arry[HW_INFO_MAX_PARAM_NUM];
	uint8_t *fdt_blob;

	printf("Are you sure you want to override factory settings in EEPROM? <y/N>");
	if (!confirm_yesno())
		return 0;

	/* get hw_info from env */
	hw_param_num = cfg_eeprom_parse_env(&hw_info_data_arry[0], sizeof(hw_info_data_arry));

	/* return in case no valid env variables */
	if (0 == hw_param_num) {
		printf("There is no supported HW configuration parameter in env variables\n");
		return 0;
	}

	 /* need to memset the hw_info to 0 for later string operation */
	memset(hw_info_str, 0, sizeof(hw_info_str));
	for (idx = 0; (idx < hw_param_num) && (total_str_len < MVEBU_HW_INFO_LEN); idx++) {
		/* check name in case only want to store specific HW parameter */
		if (name && strcmp(name, hw_info_data_arry[idx].name))
			continue;

		str_len = strlen(hw_info_data_arry[idx].name);
		if (str_len > HW_INFO_MAX_NAME_LEN)
			str_len = HW_INFO_MAX_NAME_LEN;

		if ((total_str_len + str_len) > MVEBU_HW_INFO_LEN) {
			error("HW information string from env is too long, exceed %d\n", MVEBU_HW_INFO_LEN);
			break;
		}

		memcpy(hw_info_str + total_str_len, hw_info_data_arry[idx].name, str_len);
		total_str_len += str_len;

		if ((total_str_len + 1) > MVEBU_HW_INFO_LEN) {
			error("HW information string from env is too long, exceed %d\n", MVEBU_HW_INFO_LEN);
			break;
		}

		hw_info_str[total_str_len++] = '=';

		str_len = strlen(hw_info_data_arry[idx].value);
		if (str_len > HW_INFO_MAX_VALUE_LEN)
			str_len = HW_INFO_MAX_VALUE_LEN;

		if ((total_str_len + str_len) > MVEBU_HW_INFO_LEN) {
			error("HW information string from env is too long, exceed %d\n", MVEBU_HW_INFO_LEN);
			break;
		}

		memcpy(hw_info_str + total_str_len, hw_info_data_arry[idx].value, str_len);
		total_str_len += str_len;

		if ((total_str_len + 1) > MVEBU_HW_INFO_LEN) {
			error("HW information string from env is too long, exceed %d\n", MVEBU_HW_INFO_LEN);
			break;
		}
		hw_info_str[total_str_len++] = ' ';
	}

	cfg_eeprom_set_hw_info_str(hw_info_str);
	/* save hw_info to EEPROM, and also the rest of the eeprom struct without changing it */
	fdt_blob = cfg_eeprom_get_fdt();
	cfg_eeprom_save(fdt_blob, 0);

	printf("hw_info is saved to EEPROM\n");
	cmd_hw_info_dump(name);

	return 0;
}

int do_hw_info(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	const char *cmd = argv[1];

	if (argc < 2)
		return CMD_RET_USAGE;

	if (!strcmp(cmd, "dump")) {
		if (cmd_hw_info_dump(argv[2]))
			return -EINVAL;
	} else if (!strcmp(cmd, "load")) {
		if (cmd_hw_info_load(argv[2], 0))
			return -EINVAL;
	} else if (!strcmp(cmd, "store")) {
		if (cmd_hw_info_store(argv[2]))
			return -EINVAL;
	} else {
		return CMD_RET_USAGE;
	}

	return 0;
}

U_BOOT_CMD(
	hw_info,      3,     0,      do_hw_info,
	"hw_info\n",
	"\n"
	"Load/Store HW information environment variables from/to EEPROM\n"
	"HW information includes predefined list of Env. variables (such as board ID, PCB SLM number, MAC addresses, etc).\n"
	"\tdump  <evn_name>            - Dump all (or specific <env_name>) HW parameter from EEPROM\n"
	"\tload  <env_name>            - Load all (or specific <env_name>) HW parameter from EEPROM to env variables\n"
	"\tstore <env_name>            - Store all or specific HW parameter from env variables to EEPROM\n"
	"\t				 usage of 'hw_info store' requires proper values to be set for the variables listed below\n"
	"Supported HW information parameters\n"
	"\tboard_id      board ID\n"
	"\tpcb_slm       PCB SLM number\n"
	"\tpcb_rev       PCB revision number\n"
	"\teco_rev       ECO revision number\n"
	"\tpcb_sn        PCB SN\n"
	"\tethaddr       first MAC address\n"
	"\teth1addr      second MAC address\n"
	"\teth2addr      third MAC address\n"
	"\teth3addr      fourth MAC address\n"
);
