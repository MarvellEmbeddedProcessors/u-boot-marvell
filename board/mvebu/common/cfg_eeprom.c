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
#include <asm/arch-mvebu/soc.h>
#include <fdtdec.h>
#include <libfdt.h>
#include "cfg_eeprom.h"

u8 mapping_default_fdt[] = DEFAULT_FDT_PER_BOARD;
struct eeprom_struct board_config_val = CFG_DEFAULT_VALUE;
struct board_config_struct *board_cfg = &(board_config_val.board_config);
struct config_types_info config_types_info[] = MV_EEPROM_CONFIG_INFO;
int eeprom_initialized = -1;

static uint32_t cfg_eeprom_checksum8(uint8_t *start, uint32_t len)
{
	uint32_t sum = 0;
	uint8_t *startp = start;
	do {
		sum += *startp;
		startp++;
		len--;
	} while (len > 0);
	return sum;
}

/* cfg_eeprom_fdt_config_is_enable - checking if need to load FDT from EEPROM at boot-time */
bool cfg_eeprom_fdt_config_is_enable(void)
{
	if (board_cfg->fdt_cfg_en == 1)
		return true;
	return false;
}

/* cfg_eeprom_get_config_type
 * config_info input pointer receive the mapping of the
 * required field in the local struct
 */
static bool cfg_eeprom_get_config_type(enum mv_config_type_id config_class, struct config_types_info *config_info)
{
	int i;

	/* verify existence of requested config type, pull its data */
	for (i = 0; i < MV_CONFIG_TYPE_MAX_OPTION ; i++)
		if (config_types_info[i].config_id == config_class) {
			*config_info = config_types_info[i];
			return true;
		}
	error("requested MV_CONFIG_TYPE_ID was not found (%d)\n", config_class);

	return false;
}

/* cfg_eeprom_upload_fdt_from_flash
 * write the required FDT to local struct,
 * if found fdt config id in flash, else return false
 */
bool cfg_eeprom_upload_fdt_from_flash(u8 fdt_config_id)
{
	int i;
	void *fdt_blob_temp = __dtb_dt_begin;

	debug("FDT config id = %x\n", fdt_config_id);
	for (i = 0; fdt_check_header(fdt_blob_temp) == 0; i++) {
		if ((u8)fdtdec_get_int(fdt_blob_temp, 0, "fdt_config_id", -1) == fdt_config_id) {
			memcpy((void *)board_config_val.fdt_blob, fdt_blob_temp, MVEBU_FDT_SIZE);
			return true;
		}
		fdt_blob_temp += MVEBU_FDT_SIZE;
	}
	return false;
}

/* cfg_eeprom_upload_fdt_from_eeprom - write FDT from EEPROM to local struct */
bool cfg_eeprom_upload_fdt_from_eeprom(void)
{
	struct config_types_info config_info;

	/* read fdt from EEPROM */
	if (!cfg_eeprom_get_config_type(MV_CONFIG_FDT_FILE, &config_info))
		debug("ERROR: Could not find MV_CONFIG_FDT_FILE\n");

	i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, config_info.byte_num, MULTI_FDT_EEPROM_ADDR_LEN,
		 (uint8_t *)&board_config_val.fdt_blob, config_info.byte_cnt);
	return true;
}

/* cfg_eeprom_save - write the local struct to EEPROM */
void cfg_eeprom_save(void)
{
	int reserve_length, size_of_loop, i;

	/* calculate checksum and save it in struct */
	board_config_val.checksum = cfg_eeprom_checksum8((uint8_t *)&board_config_val.pattern,
							 (uint32_t) board_config_val.length - 4);

	/* write fdt struct to EEPROM */
	size_of_loop = board_config_val.length / I2C_PAGE_WRITE_SIZE;
	reserve_length = board_config_val.length % I2C_PAGE_WRITE_SIZE;

	/* i2c support on page write with size 32-byets */
	for (i = 0; i < size_of_loop; i++) {
		i2c_write(BOARD_DEV_TWSI_INIT_EEPROM, i*I2C_PAGE_WRITE_SIZE, MULTI_FDT_EEPROM_ADDR_LEN,
			  (uint8_t *)&(board_config_val) + i*I2C_PAGE_WRITE_SIZE, I2C_PAGE_WRITE_SIZE);
	}

	i2c_write(BOARD_DEV_TWSI_INIT_EEPROM, i*I2C_PAGE_WRITE_SIZE, MULTI_FDT_EEPROM_ADDR_LEN,
		  (uint8_t *)&(board_config_val) + i*I2C_PAGE_WRITE_SIZE, reserve_length);
}

/* cfg_eeprom_get_board_config - return the whole board config
 * It is assumed the cfg_eeprom_init must be called prior to this routine,
 * otherwise static default configuration will be used.
 */
struct eeprom_struct *cfg_eeprom_get_board_config(void)
{
	return &board_config_val;
}

/* cfg_eeprom_get_fdt - return the FDT block
 * It is assumed the cfg_eeprom_init must be called prior to this routine,
 * otherwise static default configuration will be used.
 */
uint8_t *cfg_eeprom_get_fdt(void)
{
	return (uint8_t *)&board_config_val.fdt_blob;
}

/* cfg_eeprom_init - initialize FDT configuration struct */
int cfg_eeprom_init(void)
{
	struct eeprom_struct eeprom_buffer;
	struct config_types_info config_info;
	uint32_t calculate_checksum;

	/* It is possible that this init will be called by several modules during init,
	 * however only need to initialize it for one time
	 */
	if (eeprom_initialized > 0)
		return 0;

	init_func_i2c();

	/* update default active_fdt_selection, just in case there is no valid data in eeprom,
	 * and need to write default active_fdt_selection per SoC.
	 */
	board_cfg->active_fdt_selection = get_default_fdt_config_id(MV_DEFAULT_BOARD_ID);

	/* read pattern from EEPROM */
	if (!cfg_eeprom_get_config_type(MV_CONFIG_VERIFICATION_PATTERN, &config_info)) {
		error("Could not find MV_CONFIG_VERIFICATION_PATTERN\n");
		return -1;
	}

	i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, config_info.byte_num, MULTI_FDT_EEPROM_ADDR_LEN,
		 (uint8_t *)&eeprom_buffer.pattern, config_info.byte_cnt);

	/* check if pattern in EEPROM is invalid */
	if (eeprom_buffer.pattern != board_config_val.pattern) {
		debug("Could not find pattern. Loading default FDT\n");
		cfg_eeprom_upload_fdt_from_flash(get_default_fdt_config_id(MV_DEFAULT_BOARD_ID));
		goto init_done;
	}

	/* read length from EEPROM */
	if (!cfg_eeprom_get_config_type(MV_CONFIG_LENGTH, &config_info)) {
		error("Could not find MV_CONFIG_LENGTH\n");
		return -1;
	}

	i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, config_info.byte_num, MULTI_FDT_EEPROM_ADDR_LEN,
		 (uint8_t *)&eeprom_buffer.length, config_info.byte_cnt);

	/* read all the struct from EEPROM according to length field */
	i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, 0, MULTI_FDT_EEPROM_ADDR_LEN,
		 (uint8_t *)&eeprom_buffer, eeprom_buffer.length);
	/* calculate checksum and compare with the checksum that we read */
	calculate_checksum = cfg_eeprom_checksum8((uint8_t *)&eeprom_buffer.pattern,
				(uint32_t) eeprom_buffer.length - 4);

	/* if checksum is valid */
	if (calculate_checksum == eeprom_buffer.checksum) {
		/* update board_config_val struct with the read values from EEPROM */
		board_config_val = eeprom_buffer;
		/* if fdt_config is enabled, return - FDT already read in the struct from EEPROM */
		if (cfg_eeprom_fdt_config_is_enable()) {
			debug("read FDT from EEPROM\n");
			goto init_done;
		}

		/* read FDT from flash according to select active fdt */
		if (cfg_eeprom_upload_fdt_from_flash(board_cfg->active_fdt_selection)) {
			debug("read selected FDT by USER\n");
			goto init_done;
		}
	}

	/* if checksum is invalid or if select active fdt is invalid */
	/* need to load default FDT */
	if (boardid_is_valid(eeprom_buffer.man_info.boardid)) {
		cfg_eeprom_upload_fdt_from_flash(get_default_fdt_config_id(eeprom_buffer.man_info.boardid));
		debug("read board default FDT\n");
	} else {
		cfg_eeprom_upload_fdt_from_flash(get_default_fdt_config_id(MV_DEFAULT_BOARD_ID));
		debug("read SoC default FDT\n");
	}

init_done:
	eeprom_initialized = 1;
	return 0;
}
