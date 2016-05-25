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
struct fdt_configs_details {
	int fdt_config_id;
	char fdt_model[30];
};

static struct fdt_configs_details fdt_list_of_configs[MV_MAX_FDT_CONFIGURATION];

/* fdt_create_list - create the list of fdt configs */
int fdt_create_list(void)
{
	int fdt_config_id;
	char *fdt_model;
	void *fdt_blob_temp = __dtb_dt_begin;
	uint8_t fdt_blob[CONFIG_FDT_SIZE];

	fdt_list_size = 1;

	/* first index (0) will always be dedicated for the customized FDT from EEPROM.
	   if there is a valid FDT in EEPROM the index will stay zero, but if not the index will equal to -1 */
	if (cfg_eeprom_upload_fdt_from_eeprom(fdt_blob)) {
		strcpy(fdt_list_of_configs[fdt_list_size - 1].fdt_model, "(EEPROM) custom FDT available");
		fdt_list_of_configs[fdt_list_size - 1].fdt_config_id = 0;
	} else {
		strcpy(fdt_list_of_configs[fdt_list_size - 1].fdt_model, "(EPPROM) not available");
		fdt_list_of_configs[fdt_list_size - 1].fdt_config_id = -1;
	}
	while (fdt_check_header(fdt_blob_temp) == 0) {
		if ((u8)fdtdec_get_int(fdt_blob_temp, 0, "board_id", -1) == cfg_eeprom_get_board_id()) {
			fdt_model = (char *)fdt_getprop(fdt_blob_temp, 0, "model", NULL);
			fdt_config_id = fdtdec_get_int(fdt_blob_temp, 0, "fdt_config_id", -1);
			strcpy(fdt_list_of_configs[fdt_list_size].fdt_model, fdt_model);
			fdt_list_of_configs[fdt_list_size].fdt_config_id = fdt_config_id;
			fdt_list_size++;
		}
		fdt_blob_temp += CONFIG_FDT_SIZE;
	}
	return 0;
}

/* fdt_cfg_print_select - print active FDT selection */
void fdt_cfg_print_select(void)
{
	int i;
	struct eeprom_struct *p_board_config;

	if (fdt_list_size == -1)
		fdt_create_list();

	p_board_config = cfg_eeprom_get_board_config();
	for (i = 0; i < fdt_list_size; i++) {
		if (p_board_config->board_config.active_fdt_selection == fdt_list_of_configs[i].fdt_config_id) {
			printf("\t%d - %s\n", fdt_list_of_configs[i].fdt_config_id, fdt_list_of_configs[i].fdt_model);
			return;
		}
	}
	return;
}

/* fdt_cfg_set_select - update active_fdt_selection field */
int fdt_cfg_set_select(const char *selected_index)
{
	int i, index_int, index_exist = 0;
	struct eeprom_struct *p_board_config;
	uint8_t fdt_blob[CONFIG_FDT_SIZE];

	if (fdt_list_size == -1) {
		fdt_create_list();
	}

	index_int = (int)simple_strtoul(selected_index, NULL, 16);
	p_board_config = cfg_eeprom_get_board_config();

	/* search selected index in the list, and if index found set it as the active FDT selection */
	for (i = 0; i < fdt_list_size ; i++) {
		if (index_int == fdt_list_of_configs[i].fdt_config_id) {
			p_board_config->board_config.active_fdt_selection = fdt_list_of_configs[i].fdt_config_id;
			index_exist = 1;
		}
	}
	if (index_exist) {
		if (index_int == 0) {
			/* user select to boot FDT from EEPROM */
			p_board_config->board_config.fdt_cfg_en = 1;
			printf("Read FDT from EEPROM, please wait\n");
			/* load FDT from EEPROM to a temporary file and not to the local struct is done in order to
			   calculate checksum, without an influence on the current fdt that exists in local struct */
			cfg_eeprom_upload_fdt_from_eeprom(fdt_blob);
			cfg_eeprom_save(fdt_blob, 0);
		} else {
			/* user select to boot from preset FDT */
			p_board_config->board_config.fdt_cfg_en = 0;
			cfg_eeprom_save(p_board_config->fdt_blob, 0);
		}
		return 0;
	/* index does not exist at the list */
	} else if (index_int == 0) {
			/* user select customized FDT from EEPROM although it is not available */
			printf("(EPPROM) custom FDT is not available\n");
	} else {
			printf("Index is not exist\n");
	}
		return 1;
}

/* fdt_cfg_list - print list of all fdt_config_id that compatible to the boardID */
int fdt_cfg_list(void)
{
	struct eeprom_struct *p_board_config;
	int i;

	if (fdt_list_size == -1)
		fdt_create_list();

	printf("FDT config list:\n");
	p_board_config = cfg_eeprom_get_board_config();
	for (i = 0; i < fdt_list_size; i++) {
		if (i == 0)
			printf("\t0 - %s  ", fdt_list_of_configs[0].fdt_model);
		else
			printf("\t%d - %s  ", fdt_list_of_configs[i].fdt_config_id, fdt_list_of_configs[i].fdt_model);
		if (p_board_config->board_config.active_fdt_selection == fdt_list_of_configs[i].fdt_config_id)
			printf("[*]\n");
		else
			printf("\n");
	}

	return 0;
}

/* fdt_cfg_read_eeprom - write FDT from EEPROM to local struct and set 'fdt addr' environment variable */
static void fdt_cfg_read_eeprom(void)
{
	struct eeprom_struct *p_board_config;

	p_board_config = cfg_eeprom_get_board_config();
	printf("Read FDT from EEPROM, please wait\n");
	if (cfg_eeprom_upload_fdt_from_eeprom(p_board_config->fdt_blob)) {
		set_working_fdt_addr(p_board_config->fdt_blob);
		printf("Loaded FDT from EEPROM successfully\n");
		printf("To save the changes, please run the command fdt_config save.\n");
	}
}

/* fdt_cfg_read_flash - write the required FDT to local struct, if found fdt config id in the list.
   else return without writing anything*/
static int fdt_cfg_read_flash(int selected_index)
{
	int i;
	struct eeprom_struct *p_board_config;

	p_board_config = cfg_eeprom_get_board_config();
	for (i = 0; i < fdt_list_size ; i++) {
		if (selected_index == fdt_list_of_configs[i].fdt_config_id) {
			cfg_eeprom_upload_fdt_from_flash(fdt_list_of_configs[i].fdt_config_id);
			set_working_fdt_addr(p_board_config->fdt_blob);
			printf("To save the changes, please run the command fdt_config save.\n");
			return 0;
		}
	}
	printf("Index is not exist\n");
	return 1;
}

/* fdt_cfg_load - load FDT to local struct */
int fdt_cfg_load(const char *selected_index)
{
	int index_int;

	if (fdt_list_size == -1)
		fdt_create_list();

	index_int = (int)simple_strtoul(selected_index, NULL, 16);

	/* load fdt from EEPROM */
	if (index_int == 0) {
		/* if FDT in EEPROM is available */
		if (fdt_list_of_configs[0].fdt_config_id == 0) {
			fdt_cfg_read_eeprom();
			return 0;
		} else {
			printf("(EPPROM) custom FDT is not available\n");
			return 1;
		}
	}
	/* load FDT from flash */
	if (fdt_cfg_read_flash(index_int))
		return 1;
	return 0;
}

/* fdt_cfg_save - save the current working FDT to EEPROM */
int fdt_cfg_save(uint8_t *fdt_blob)
{
	cfg_eeprom_save(fdt_blob, 1);
	fdt_list_size = -1;
	printf("FDT is saved in EEPROM, to boot from customized FDT please select it from list\n");
	return 0;
}
