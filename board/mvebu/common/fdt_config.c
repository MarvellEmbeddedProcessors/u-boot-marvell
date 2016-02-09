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

#include <fdtdec.h>
#include <libfdt.h>
#include <asm/u-boot.h>
#include <fdt_support.h>
#include "devel-board.h"
#include "fdt_config.h"

DECLARE_GLOBAL_DATA_PTR;

static int fdt_list_size = -1;
static int fdt_list_of_configs[MV_MAX_FDT_CONFIGURATION] = {-1};

/* fdt_select_set - if the selected_index is exist, update active_fdt_selection field. else return without changing */
int fdt_select_set(const char *selected_index)
{
	int i, index_int;
	struct eeprom_struct *p_board_config;

	if (fdt_list_size == -1) {
		printf("FDT configuration list is not available.\n");
		printf("need to run \"fdt_config list\" command before. and choose from this list.\n");
		return 1;
	}

	index_int = (int)simple_strtoul(selected_index, NULL, 16);
	p_board_config = cfg_eeprom_get_board_config();
	for (i = 0; i < fdt_list_size ; i++) {
		if (index_int == fdt_list_of_configs[i]) {
			p_board_config->board_config.active_fdt_selection = fdt_list_of_configs[i];
			printf("To save the changes, please run the command fdt_config save.\n");
			return 0;
		}
	}

	printf("Index is not exist\n");
	return 1;
}

/* fdt_select_list - print list of all fdt_config_id */
int fdt_select_list(void)
{
	int i, fdt_config_id;
	char *fdt_model;
	void *fdt_blob_temp = __dtb_dt_begin;

	fdt_list_size = 0;
	printf("FDT config list:\n");
	for (i = 0; fdt_check_header(fdt_blob_temp) == 0; i++) {
		if ((u8)fdtdec_get_int(fdt_blob_temp, 0, "board_id", -1) == cfg_eeprom_get_board_id()) {
			fdt_list_size++;
			fdt_model = (char *)fdt_getprop(fdt_blob_temp, 0, "model", NULL);
			fdt_config_id = fdtdec_get_int(fdt_blob_temp, 0, "fdt_config_id", -1);
			printf("\t%d - %s\n", fdt_config_id, fdt_model);
			fdt_list_of_configs[fdt_list_size - 1] = fdt_config_id;
		}
		fdt_blob_temp += MVEBU_FDT_SIZE;
	}
	return 0;
}

/* fdt_cfg_read_eeprom - write FDT from EEPROM to local struct and set 'fdt addr' environment variable */
void fdt_cfg_read_eeprom(void)
{
	struct eeprom_struct *p_board_config;

	p_board_config = cfg_eeprom_get_board_config();
	if (cfg_eeprom_upload_fdt_from_eeprom())
		set_working_fdt_addr(p_board_config->fdt_blob);
}

/* fdt_cfg_read_flash - write the required FDT to local struct, if found fdt config id in the list.
   else return without writing anything*/
int fdt_cfg_read_flash(const char *selected_index)
{
	int i, index_int;
	struct eeprom_struct *p_board_config;

	if (fdt_list_size == -1) {
		printf("FDT configuration list is not available.\n");
		printf("need to run \"fdt_config list\" command before. and choose from this list.\n");
		return 1;
	}

	index_int = (int)simple_strtoul(selected_index, NULL, 16);
	p_board_config = cfg_eeprom_get_board_config();
	for (i = 0; i < fdt_list_size ; i++) {
		if (index_int == fdt_list_of_configs[i]) {
			cfg_eeprom_upload_fdt_from_flash(fdt_list_of_configs[i]);
			set_working_fdt_addr(p_board_config->fdt_blob);
			printf("To save the changes, please run the command fdt_config save.\n");
			return 0;
		}
	}
	printf("Index is not exist\n");
	return 1;
}

/* fdt_cfg_on - enable read FDT from EEPROM */
int fdt_cfg_on(void)
{
	struct eeprom_struct *p_board_config;

	p_board_config = cfg_eeprom_get_board_config();
	if (cfg_eeprom_fdt_config_is_enable() == false) {
		p_board_config->length += MVEBU_FDT_SIZE;
		p_board_config->board_config.fdt_cfg_en = 1;
	}
	printf("Do you want to read FDT from EEPROM? <y/N> ");
	if (confirm_yesno()) {
		if (!cfg_eeprom_upload_fdt_from_eeprom())
			return 1;
	} else
		printf("To save the changes, please run the command fdt_config save.\n");
	return 0;
}

/* fdt_cfg_on - disable read FDT from EEPROM */
int fdt_cfg_off(void)
{
	struct eeprom_struct *p_board_config;

	p_board_config = cfg_eeprom_get_board_config();
	if (cfg_eeprom_fdt_config_is_enable()) {
		p_board_config->length -= MVEBU_FDT_SIZE;
		p_board_config->board_config.fdt_cfg_en = 0;
	}
	printf("config fdt is now disable\n");
	printf("To save the changes, please run the command fdt_config save.\n");
	return 0;
}
