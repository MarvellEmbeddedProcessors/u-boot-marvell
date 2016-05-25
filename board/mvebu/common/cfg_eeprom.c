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
#include <fdt_support.h>
#include "cfg_eeprom.h"

#define AUTO_RECOVERY_RETRY_TIMES 10

u8 mapping_default_fdt[] = DEFAULT_FDT_PER_BOARD;
struct eeprom_struct board_config_val = CFG_DEFAULT_VALUE;
struct board_config_struct *board_cfg = &(board_config_val.board_config);
struct config_types_info config_types_info[] = MV_EEPROM_CONFIG_INFO;
/* this array is used as temporary fdt, in order to enable to make changes
 on the fdt_blob without changing the fdt in the local struct */
uint8_t fdt_blob_temp[CONFIG_FDT_SIZE];
int eeprom_initialized = -1;
int g_board_id = -1;
#ifdef CONFIG_TARGET_ARMADA_8K
/* this array is used a buffer to zip and unzip the fdt blob */
uint8_t fdt_zip_buffer[CONFIG_FDT_SIZE];
#endif

static char hw_info_param_list[][HW_INFO_MAX_NAME_LEN] = {
	"board_id",
	"pcb_slm",
	"pcb_rev",
	"eco_rev",
	"pcb_sn",
	"ethaddr",
	"eth1addr",
	"eth2addr",
	"eth3addr"
};
static int hw_info_param_num = (sizeof(hw_info_param_list)/sizeof(hw_info_param_list[0]));

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
#ifdef CONFIG_TARGET_ARMADA_8K
int cfg_eeprom_zip_and_unzip(unsigned long size, void *source_fdt, bool zip_flag)
{
	unsigned long new_size = ~0UL;

	if (zip_flag) {
		/* compress fdt */
		if (gzip((void *)fdt_zip_buffer, &new_size, source_fdt, size) != 0) {
			error("Could not compress device tree file\n");
			return -1;
		}
	} else {
		/* decompress fdt */
		if (gunzip((void *)fdt_zip_buffer, size, source_fdt, &new_size) != 0) {
			error("Could not decompress device tree file\n");
			return -1;
		}
	}
	if (new_size > CONFIG_FDT_SIZE) {
		error("Unexpected size of zip file = %lu\n", new_size);
		return -1;
	}
	debug("The new size of the fdt = %lu\n", new_size);
	/* copy the compressed/decompressed file back to the source fdt */
	memcpy(source_fdt, (void *)fdt_zip_buffer, new_size);

	return new_size;
}

int cfg_eeprom_zip_fdt(unsigned long size, void *source_fdt)
{
	return cfg_eeprom_zip_and_unzip(size, source_fdt, true);
}

int cfg_eeprom_unzip_fdt(unsigned long size, void *source_fdt)
{
	return cfg_eeprom_zip_and_unzip(size, source_fdt, false);
}

#else
int cfg_eeprom_zip_fdt(unsigned long size, void *source_fdt)
{
	/* return the orignal size of the fdt blob */
	return CONFIG_FDT_SIZE;
}

int cfg_eeprom_unzip_fdt(unsigned long size, void *source_fdt)
{
	/* return zero because in armada-3700 there is no compressed fdt,
	   therefore the unzip function and its return value has no effect the  */
	return 0;
}
#endif

/* cfg_eeprom_upload_fdt_from_flash
 * write the required FDT to local struct,
 * if found fdt config id in flash, else return false
 */
bool cfg_eeprom_upload_fdt_from_flash(u8 fdt_config_id)
{
	int i;
	void *fdt_blob = __dtb_dt_begin;

	debug("FDT config id = %x\n", fdt_config_id);
	for (i = 0; fdt_check_header(fdt_blob) == 0; i++) {
		if ((u8)fdtdec_get_int(fdt_blob, 0, "fdt_config_id", -1) == fdt_config_id &&
		    (u8)fdtdec_get_int(fdt_blob, 0, "board_id", -1) == cfg_eeprom_get_board_id()) {
			memcpy((void *)board_config_val.fdt_blob, fdt_blob, CONFIG_FDT_SIZE);
			return true;
		}
		fdt_blob += CONFIG_FDT_SIZE;
	}
	return false;
}

/* cfg_eeprom_upload_fdt_from_eeprom - write FDT from EEPROM to local struct */
bool cfg_eeprom_upload_fdt_from_eeprom(uint8_t *fdt_blob)
{
	struct config_types_info config_info;
	unsigned long decompressed_size;
	u32 fdt_blob_size = board_config_val.length - EEPROM_STRUCT_SIZE;

	/* read the compressed file from EEPROM to buffer */
	if (fdt_blob_size) {
		if (!cfg_eeprom_get_config_type(MV_CONFIG_FDT_FILE, &config_info))
			error("Could not find MV_CONFIG_FDT_FILE\n");
		i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, config_info.byte_num, MULTI_FDT_EEPROM_ADDR_LEN,
			 fdt_blob_temp, fdt_blob_size);

		/* decompress fdt */
		decompressed_size = cfg_eeprom_unzip_fdt(CONFIG_FDT_SIZE, (void *)fdt_blob_temp);

		if (decompressed_size == -1)
			return false;

		/* if didn't find FDT in EEPROM */
		if (fdt_check_header((void *)fdt_blob_temp) != 0) {
			debug("FDT in EEPROM is invalid and didn't loaded to RAM\n");
			return false;
		}
		memcpy((void *)fdt_blob, fdt_blob_temp, CONFIG_FDT_SIZE);
	} else {
		debug("FDT in EEPROM is invalid and didn't loaded to RAM\n");
		return false;
	}
	return true;
}

/* cfg_eeprom_write_to_eeprom - write the local struct to EEPROM */
void cfg_eeprom_write_to_eeprom(int length, bool write_fdt)
{
	int reserve_length, size_of_loop, i;

	/* calculate checksum and save it in struct */
	if (write_fdt)
		/* if fdt from EEPROM is enable, the checksum calculation includes fdt blob */
		board_config_val.checksum = cfg_eeprom_checksum8((uint8_t *)&board_config_val.pattern,
								 (uint32_t)board_config_val.length - 4);
	else
		/* if fdt from EEPROM is disable, the calculation of the checksum will be without the fdt blob */
		board_config_val.checksum = cfg_eeprom_checksum8((uint8_t *)&board_config_val.pattern,
								 (uint32_t)EEPROM_STRUCT_SIZE - 4);


	/* write fdt struct to EEPROM */
	size_of_loop = length / I2C_PAGE_WRITE_SIZE;
	reserve_length = length % I2C_PAGE_WRITE_SIZE;

	/* i2c support on page write with size 32-byets */
	for (i = 0; i < size_of_loop; i++) {
		i2c_write(BOARD_DEV_TWSI_INIT_EEPROM, i*I2C_PAGE_WRITE_SIZE, MULTI_FDT_EEPROM_ADDR_LEN,
			  (uint8_t *)&(board_config_val) + i*I2C_PAGE_WRITE_SIZE, I2C_PAGE_WRITE_SIZE);
	}
	/* write the reserve data from 32-bytes */
	if (reserve_length)
		i2c_write(BOARD_DEV_TWSI_INIT_EEPROM, i*I2C_PAGE_WRITE_SIZE, MULTI_FDT_EEPROM_ADDR_LEN,
			  (uint8_t *)&(board_config_val) + i*I2C_PAGE_WRITE_SIZE, reserve_length);
}

/* cfg_eeprom_save - write the local struct to EEPROM */
void cfg_eeprom_save(uint8_t *fdt_blob, int write_forced_fdt)
{
	unsigned long compressed_size;
	unsigned long decompressed_size;

	/* if fdt_config is enable write also fdt to EEPROM
	   or if fdt_config in disable but we want to force write of FDT without selecting the customized FDT */
	if (board_config_val.board_config.fdt_cfg_en == 1 || write_forced_fdt) {
		/* back up the fdt that is in the local struct, and restore it at the end of this function */
		if (fdt_blob != board_config_val.fdt_blob) {
			memcpy((void *)fdt_blob_temp, (void *)board_config_val.fdt_blob, CONFIG_FDT_SIZE);
			memcpy((void *)board_config_val.fdt_blob, (void *)fdt_blob, CONFIG_FDT_SIZE);
		}

		/* compress fdt */
		compressed_size = cfg_eeprom_zip_fdt(CONFIG_FDT_SIZE, (void *)board_config_val.fdt_blob);

		if (compressed_size == -1)
			return;

		/* update length  */
		board_config_val.length = compressed_size + EEPROM_STRUCT_SIZE;
		debug("size of struct + fdt compressed = %d\n", board_config_val.length);

		/* write local struct with fdt blob to EEPROM */
		if (board_config_val.board_config.fdt_cfg_en == 1)
			cfg_eeprom_write_to_eeprom(board_config_val.length, true);
		else
			cfg_eeprom_write_to_eeprom(board_config_val.length, false);

		/* decompress fdt - After saving the fdt, the compressed file is in the struct,
		   therefore need to return to the state before saving the FDT. to let the user
		   continue work without resetting his board. */
		decompressed_size = cfg_eeprom_unzip_fdt(CONFIG_FDT_SIZE, (void *)board_config_val.fdt_blob);

		if (decompressed_size == -1)
			return;

		/* restore the fdt from local struct after it was written*/
		if (fdt_blob != board_config_val.fdt_blob) {
			memcpy((void *)fdt_blob, (void *)board_config_val.fdt_blob, CONFIG_FDT_SIZE);
			memcpy((void *)board_config_val.fdt_blob, (void *)fdt_blob_temp, CONFIG_FDT_SIZE);
		}

	} else {
		/* write local struct with fdt blob to EEPROM */
		cfg_eeprom_write_to_eeprom(EEPROM_STRUCT_SIZE, false);
	}
	/* reset g_board_id so it will get board ID from EEPROM again */
	g_board_id = -1;
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

/* cfg_eeprom_get_hw_info_str - copy hw_info string from cfg_eeprom module to destination */
void cfg_eeprom_get_hw_info_str(uchar *hw_info_str)
{
	int len;
	struct config_types_info config_info;

	/* if board_id isn't initialized, need to read hw_info and board_id from EEPROM */
	if (g_board_id == -1) {
		/* read hw_info config from EEPROM */
		if (!cfg_eeprom_get_config_type(MV_CONFIG_HW_INFO, &config_info)) {
			error("Could not find MV_CONFIG_hw_info\n");
			return;
		}
		i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, config_info.byte_num, MULTI_FDT_EEPROM_ADDR_LEN,
			 (uint8_t *)board_config_val.man_info.hw_info, config_info.byte_cnt);
	}
	len = strlen((const char *)board_config_val.man_info.hw_info);
	if (len >= MVEBU_HW_INFO_LEN)
		len = MVEBU_HW_INFO_LEN - 1;

	memcpy(hw_info_str, board_config_val.man_info.hw_info, len);
}

/* cfg_eeprom_set_hw_info_str - copy hw_info sting to cfg_eeprom module
 * It is assumed the cfg_eeprom_init must be called prior to this routine,
 * otherwise static default configuration will be used.
 */
void cfg_eeprom_set_hw_info_str(uchar *hw_info_str)
{
	int len;
	struct config_types_info config_info;

	/* read hw_info config from EEPROM */
	if (!cfg_eeprom_get_config_type(MV_CONFIG_HW_INFO, &config_info)) {
		error("Could not find MV_CONFIG_hw_info\n");
		return;
	}

	len = strlen((const char *)hw_info_str);
	if (len >= config_info.byte_cnt)
		len = config_info.byte_cnt - 1;

	/* need to set all value to 0 at first for later string operation */
	memset(board_config_val.man_info.hw_info, 0, config_info.byte_cnt);
	memcpy(board_config_val.man_info.hw_info, hw_info_str, len);
}

/* cfg_eeprom_skip_space - skip the space character */
static char *cfg_eeprom_skip_space(char *buf)
{
	while ((buf[0] == ' ' || buf[0] == '\t'))
		++buf;
	return buf;
}

/* cfg_eeprom_char_to_hex - convert char to hex */
static int cfg_eeprom_char_to_hex(int a)
{
	if (a >= '0' && a <= '9')
		return a - '0';
	else if (a >= 'a' && a <= 'f')
		return a - 'a' + 10;
	else if (a >= 'A' && a <= 'F')
		return a - 'A' + 10;
	else
		return 0;
}

/* cfg_eeprom_string_to_hex - convert string to hex */
static u32 cfg_eeprom_string_to_hex(char *str)
{
	int len;
	int idx;
	u32 value  = 0;

	len = strlen(str);
	for (idx = 0; idx < len; idx++)
		value = (value * 0x10 + cfg_eeprom_char_to_hex(str[idx]));

	return value;
}

/* cfg_eeprom_parse_hw_info - parse the hw_info from string to name/value pairs */
int cfg_eeprom_parse_hw_info(uchar *hw_info_str, struct hw_info_point_struct *hw_info_point_array)
{
	int count;
	char *name;
	char *value;

	cfg_eeprom_get_hw_info_str(hw_info_str);
	name = (char *)hw_info_str;
	name = cfg_eeprom_skip_space(name);
	/* return 0 in case the string is empty */
	if (NULL == name)
		return 0;

	for (count = 0; name != NULL; count++) {
		hw_info_point_array[count].name = name;
		value = strchr(name, '=');

		if (value == NULL)
			return count;

		*value = '\0';
		value++;
		hw_info_point_array[count].value = value;

		name = strchr(value, ' ');
		if (name == NULL)
			return ++count;

		*name = '\0';
		name = cfg_eeprom_skip_space(name + 1);
	}
	count++;

	return count;
}

/* cfg_eeprom_parse_env - parse the env from env to name/value pairs */
int cfg_eeprom_parse_env(struct hw_info_data_struct *hw_info_data_array, int size)
{
	int param_num = 0;
	int idx;
	int len;
	char *name;
	char *value;

	/* need to memset to 0 for later string operation */
	memset(hw_info_data_array, 0, size);
	for (idx = 0; idx < hw_info_param_num; idx++) {
		name = hw_info_param_list[idx];
		value = getenv(name);

		/* print indication if the expected HW info parameter does not exist in env */
		if (NULL == value) {
			printf("miss %s in env, please set it at first\n", hw_info_param_list[idx]);
			continue;
		}

		len = strlen(name);
		if (len > HW_INFO_MAX_NAME_LEN)
			len  = HW_INFO_MAX_NAME_LEN;
		memcpy(hw_info_data_array[param_num].name, name, len);

		len = strlen(value);
		if (len > HW_INFO_MAX_NAME_LEN)
			len  = HW_INFO_MAX_NAME_LEN;
		memcpy(hw_info_data_array[param_num].value, value, len);

		param_num++;
	}

	return param_num;
}

/* cfg_eeprom_parse_hw_info - return board ID
 * this routine will return the board ID from EEPROM in case there is valid
 * board ID in EEPROM, otherwise it return default board ID per SoC.
 * It is assumed the cfg_eeprom_init must be called prior to this routine,
 * otherwise static default configuration will be used.
 */
int cfg_eeprom_get_board_id(void)
{
	int idx;
	int param_num;
	uchar hw_info_str[MVEBU_HW_INFO_LEN];
	struct hw_info_point_struct hw_info_point_array[HW_INFO_MAX_PARAM_NUM];
	u32 board_id;

	/* return g_board_id if it is already initialized */
	if (g_board_id != -1)
		return g_board_id;

	memset(hw_info_str, 0, sizeof(hw_info_str));
	cfg_eeprom_get_hw_info_str(hw_info_str);

	param_num = cfg_eeprom_parse_hw_info(hw_info_str, hw_info_point_array);
	if (param_num == 0)
		goto default_id;

	if (param_num > HW_INFO_MAX_NAME_LEN) {
		printf("parameter number should not exceed %d\n", HW_INFO_MAX_NAME_LEN);
		goto default_id;
	}

	for (idx = 0; idx < param_num; idx++) {
		if ((strcmp("board_id", hw_info_point_array[idx].name) == 0) ||
		    (strcmp("boardid", hw_info_point_array[idx].name) == 0)) {
			board_id = cfg_eeprom_string_to_hex(hw_info_point_array[idx].value);
			g_board_id = board_id;
			return board_id;
		}
	}

default_id:
	g_board_id = MV_DEFAULT_BOARD_ID;
	return MV_DEFAULT_BOARD_ID;
}

#ifdef CONFIG_TARGET_ARMADA_3700
static u32 cfg_eeprom_check_validation_counter(void)
{
	u32 load_default = 0;

	/* Only increase the validation_counter if we are not using the default FDT  */
	if (board_cfg->fdt_cfg_en ||
	    board_cfg->active_fdt_selection != get_default_fdt_config_id(cfg_eeprom_get_board_id())) {
		board_cfg->validation_counter++;
		cfg_eeprom_save(board_config_val.fdt_blob, 0);
	}

	/* Add hints here if validation_counter != 0 when the system starts */
	if (board_cfg->validation_counter > 1)
		error("Detect system crash %d times, will use defalut FDT if detect %d crashes\n",
		      board_cfg->validation_counter, AUTO_RECOVERY_RETRY_TIMES);

	/* Load the default FDT when validation_counter >= AUTO_RECOVERY_RETRY_TIMES */
	if (board_cfg->validation_counter >= AUTO_RECOVERY_RETRY_TIMES) {
		printf("Exceed maximum retry counter. Use default FDT\n");
		load_default = 1;
		/* reset the validation_counter here, not in cfg_eeprom_finish,
		 * otherwise the eeprom will be overrided by the default FDT
		 */
		board_cfg->validation_counter = 0;
		cfg_eeprom_save(board_config_val.fdt_blob, 0);
	}

	return load_default;
}
#endif


/* cfg_eeprom_init - initialize FDT configuration struct
   The EEPROM FDT is used if 1) the checksum is valid, 2) the system
   is not in recovery mode, 3) validation_counter < AUTO_RECOVERY_RETRY_TIMES
   Otherwise the default FDT is used.
 */
int cfg_eeprom_init(void)
{
	struct eeprom_struct eeprom_buffer;
	struct config_types_info config_info;
	uint32_t calculate_checksum;
	unsigned long decompressed_size;
	u32 load_default = 0;

	/* It is possible that this init will be called by several modules during init,
	 * however only need to initialize it for one time
	 */
	if (eeprom_initialized == 1)
		return 0;

	init_func_i2c();

	/* update default active_fdt_selection, just in case there is no valid data in eeprom,
	 * and need to write default active_fdt_selection per SoC.
	 */
	board_cfg->active_fdt_selection = get_default_fdt_config_id(cfg_eeprom_get_board_id());

	/* read pattern from EEPROM */
	if (!cfg_eeprom_get_config_type(MV_CONFIG_VERIFICATION_PATTERN, &config_info)) {
		error("Could not find MV_CONFIG_VERIFICATION_PATTERN\n");
		return -1;
	}
	i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, config_info.byte_num, MULTI_FDT_EEPROM_ADDR_LEN,
		 (uint8_t *)&eeprom_buffer.pattern, config_info.byte_cnt);

	/* check if pattern in EEPROM is invalid */
	if (eeprom_buffer.pattern != board_config_val.pattern) {
		printf("EEPROM configuration pattern not detected. Loading default FDT\n");
		cfg_eeprom_upload_fdt_from_flash(get_default_fdt_config_id(cfg_eeprom_get_board_id()));
		goto init_done;
	}

	/* read struct (without fdt blob) from EEPROM */
	i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, 0, MULTI_FDT_EEPROM_ADDR_LEN,
		 (uint8_t *)&eeprom_buffer, EEPROM_STRUCT_SIZE);

	/* examination if it's necessary to read fdt from EEPROM */
	if (eeprom_buffer.board_config.fdt_cfg_en == 1) {
		/* read the compressed file from EEPROM to buffer */
		if (!cfg_eeprom_get_config_type(MV_CONFIG_FDT_FILE, &config_info))
			error("Could not find MV_CONFIG_FDT_FILE\n");

		i2c_read(BOARD_DEV_TWSI_INIT_EEPROM, config_info.byte_num, MULTI_FDT_EEPROM_ADDR_LEN,
			 (uint8_t *)eeprom_buffer.fdt_blob, eeprom_buffer.length - EEPROM_STRUCT_SIZE);

		/* calculate checksum */
		calculate_checksum = cfg_eeprom_checksum8((uint8_t *)&eeprom_buffer.pattern,
							   (uint32_t)eeprom_buffer.length - 4);

		/* decompress fdt */
		decompressed_size = cfg_eeprom_unzip_fdt(CONFIG_FDT_SIZE, (void *)eeprom_buffer.fdt_blob);

		if (decompressed_size == -1)
			return -1;

	} else {
		/* calculate checksum */
		calculate_checksum = cfg_eeprom_checksum8((uint8_t *)&eeprom_buffer.pattern,
							(uint32_t)EEPROM_STRUCT_SIZE - 4);
	}

	/* if checksum is valid and not in recovery boot mode */
	/* the recovery mode works only on armada-3700 for now */
#ifdef CONFIG_TARGET_ARMADA_3700
	if (calculate_checksum == eeprom_buffer.checksum && !mvebu_is_in_recovery_mode()) {
#else
	if (calculate_checksum == eeprom_buffer.checksum) {
#endif
		/* update board_config_val struct with the read values from EEPROM */
		board_config_val = eeprom_buffer;

#ifdef CONFIG_TARGET_ARMADA_3700
		/* load default FDT if validation_counter >= AUTO_RECOVERY_RETRY_TIMES */
		load_default = cfg_eeprom_check_validation_counter();
#endif
	}

	if (!load_default) {
		/* if fdt_config is enabled, return - FDT already read in the struct from EEPROM */
		if (cfg_eeprom_fdt_config_is_enable()) {
			printf("read FDT from EEPROM\n");
			goto init_done;
		}

		/* read FDT from flash according to select active fdt */
		if (cfg_eeprom_upload_fdt_from_flash(board_cfg->active_fdt_selection)) {
			printf("read selected FDT by USER\n");
			goto init_done;
		}
	}

	/* if checksum is invalid or if select active fdt is invalid or
	 * validation_counter >= AUTO_RECOVERY_RETRY_TIMES, need to load default FDT
	 */
	if (boardid_is_valid(cfg_eeprom_get_board_id())) {
		cfg_eeprom_upload_fdt_from_flash(get_default_fdt_config_id(cfg_eeprom_get_board_id()));
		printf("read board default FDT\n");
	} else {
		cfg_eeprom_upload_fdt_from_flash(get_default_fdt_config_id(MV_DEFAULT_BOARD_ID));
		printf("read SoC default FDT\n");
	}

init_done:
	eeprom_initialized = 1;
	return 0;
}

/* cfg_eeprom_finish - reset the validation_counter if boots normally */
int cfg_eeprom_finish(void)
{
	if (board_cfg->validation_counter != 0) {
		board_cfg->validation_counter = 0;
		cfg_eeprom_save(board_config_val.fdt_blob, 0);
	}

	return 0;
}

