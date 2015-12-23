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
#include "fdt_eeprom.h"

u8 mapping_default_fdt[] = DEFAULT_FDT_PER_BOARD;
struct eeprom_struct fdt_config_val = FDT_DEFAULT_VALUE;
struct board_config_struct_info *board_cfg = &(fdt_config_val.board_config);
struct fdt_config_types_info fdt_config_types_info[] = MV_EEPROM_CONFIG_INFO;

#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SYS_I2C)
static int init_func_i2c(void)
{
#ifdef CONFIG_SYS_I2C
	i2c_init_all();
#else
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#endif
	return 0;
}
#endif

uint32_t mvebu_checksum8(uint8_t *start, uint32_t len)
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
/* fdt_config_is_enable - checking if need to load FDT from EEPROM at boot-time */
bool fdt_config_is_enable(void)
{
	if (board_cfg->fdt_cfg_en == 1)
		return true;
	return false;
}

/* config_type_get - config_info input pointer receive the mapping of the required field in the local struct */
bool config_type_get(enum mv_config_type_id config_class, struct fdt_config_types_info *config_info)
{
	int i;

	/* verify existence of requested config type, pull its data */
	for (i = 0; i < MV_CONFIG_TYPE_MAX_OPTION ; i++)
		if (fdt_config_types_info[i].config_id == config_class) {
			*config_info = fdt_config_types_info[i];
			return true;
		}
	error("requested MV_CONFIG_TYPE_ID was not found (%d)\n", config_class);

	return false;
}

/* upload_fdt_from_flash - write the required FDT to local struct, if found fdt config id in flash, else return false*/
bool upload_fdt_from_flash(u8 fdt_config_id)
{
	int i;
	void *fdt_blob_temp = __dtb_dt_begin;

	debug("FDT config id = %x\n", fdt_config_id);
	for (i = 0; fdt_check_header(fdt_blob_temp) == 0; i++) {
		if ((u8)fdtdec_get_int(fdt_blob_temp, 0, "fdt_config_id", -1) == fdt_config_id) {
			memcpy((void *)fdt_config_val.fdt_blob, fdt_blob_temp, MVEBU_FDT_SIZE);
			return true;
		}
		fdt_blob_temp += MVEBU_FDT_SIZE;
	}
	return false;
}

/* upload_fdt_from_eeprom - write FDT from EEPROM to local struct */
bool upload_fdt_from_eeprom(void)
{
	struct fdt_config_types_info config_info;

	/* read fdt from EEPROM */
	if (!config_type_get(MV_CONFIG_FDT_FILE, &config_info))
		debug("ERROR: Could not find MV_CONFIG_FDT_FILE\n");

	i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, config_info.byte_num, 1,
		 (uint8_t *)&fdt_config_val.fdt_blob, config_info.byte_cnt);
	return true;
}

/* write_fdt_struct_to_eeprom - write FDT from local struct to EEPROM */
void write_fdt_struct_to_eeprom(void)
{
	int reserve_length, size_of_loop, i;

	size_of_loop = fdt_config_val.length / I2C_PAGE_WRITE_SIZE;
	reserve_length = fdt_config_val.length % I2C_PAGE_WRITE_SIZE;

	/* i2c support on page write with size 32-byets */
	for (i = 0; i < size_of_loop; i++)
		i2c_write(BOARD_DEV_TWSI_INIT_EEPROM, i*I2C_PAGE_WRITE_SIZE, 1,
			  (uint8_t *)&(fdt_config_val) + i*I2C_PAGE_WRITE_SIZE, I2C_PAGE_WRITE_SIZE);

	i2c_write(BOARD_DEV_TWSI_INIT_EEPROM, i*I2C_PAGE_WRITE_SIZE, 1,
		  (uint8_t *)&(fdt_config_val) + i*I2C_PAGE_WRITE_SIZE, reserve_length);
}

/* mvebu_fdt_config_init - initialize FDT configuration struct,
   and return the selected FTD that should be loaded at boot time */
u8 *mvebu_fdt_config_init(void)
{
	struct eeprom_struct eeprom_buffer;
	struct fdt_config_types_info config_info;
	uint32_t calculate_checksum;

	init_func_i2c();
	/* read pattern from EEPROM */
	if (!config_type_get(MV_CONFIG_VERIFICATION_PATTERN, &config_info))
		error("Could not find MV_CONFIG_VERIFICATION_PATTERN\n");

	i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, config_info.byte_num, 1,
		 (uint8_t *)&eeprom_buffer.pattern, config_info.byte_cnt);

	/* check if pattern in EEPROM is valid */
	if (eeprom_buffer.pattern != fdt_config_val.pattern) {
		debug("Could not find pattern. Loading default FDT\n");
		upload_fdt_from_flash(get_default_fdt_config_id(MV_DEFAULT_BOARD_ID));
		return fdt_config_val.fdt_blob;
	}

	/* read length from EEPROM */
	if (!config_type_get(MV_CONFIG_LENGTH, &config_info))
		error("Could not find MV_CONFIG_LENGTH\n");

	i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, config_info.byte_num, 1,
		 (uint8_t *)&eeprom_buffer.length, config_info.byte_cnt);

	/* read all the struct from EEPROM according to length field */
	i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, 0, 1, (uint8_t *)&eeprom_buffer, eeprom_buffer.length);
	/* calculate checksum and compare with the checksum that we read */
	calculate_checksum = mvebu_checksum8((uint8_t *)&eeprom_buffer.pattern, (uint32_t) eeprom_buffer.length - 4);

	/* if checksum is valid */
	if (calculate_checksum == eeprom_buffer.checksum) {
		/* update fdt_config_val struct with the read values from EEPROM */
		fdt_config_val = eeprom_buffer;
		/* if fdt_config is enabled, return - FDT already read in the struct from EEPROM */
		if (fdt_config_is_enable()) {
			debug("read FDT from EEPROM\n");
			return fdt_config_val.fdt_blob;
		}
		/* read FDT from flash according to select active fdt */
		if (upload_fdt_from_flash(board_cfg->active_fdt_selection)) {
			debug("read selected FDT by USER\n");
			return fdt_config_val.fdt_blob;
		}
	}
	/* if checksum is invalid or if select active fdt is invalid */
	/* need to load default FDT */
	if (boardid_is_valid(eeprom_buffer.man_info.boardid)) {
		upload_fdt_from_flash(get_default_fdt_config_id(eeprom_buffer.man_info.boardid));
		debug("read board defualt FDT\n");
	} else {
		upload_fdt_from_flash(get_default_fdt_config_id(MV_DEFAULT_BOARD_ID));
		debug("read SoC defualt FDT\n");
	}

	return fdt_config_val.fdt_blob;
}
