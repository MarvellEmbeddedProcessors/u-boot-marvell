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

#ifndef _CFG_EEPROM_H_
#define _CFG_EEPROM_H_
#include <common.h>
#include <i2c.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/board-info.h>
#include <asm/arch-mvebu/mvebu.h>

enum mv_config_type_id {
	MV_CONFIG_VERIFICATION_PATTERN,
	MV_CONFIG_LENGTH,
	MV_CONFIG_HW_INFO,
	MV_CONFIG_FDTCFG_EN,
	MV_CONFIG_ACTIVE_FDT_SELECTION,
	MV_CONFIG_FDT_FILE,
	MV_CONFIG_FDTCFG_VALID,
	MV_CONFIG_TYPE_MAX_OPTION,  /* limit for user read/write routines */
};

/* #pragma pack(1) */
#define MVEBU_HW_INFO_LEN	256
struct manufacturing_information_struct {
	u8 hw_info[MVEBU_HW_INFO_LEN];
};

/* #pragma pack(1) */
struct board_config_struct {
	u8 fdt_cfg_en;
	u8 active_fdt_selection;
	u8 validation_counter;
	u8 reserve_board_cgf[7];
};

/* #pragma pack(1) */
struct eeprom_struct {
	u32 checksum;
	u32 pattern;
	u16 length;
	struct manufacturing_information_struct man_info;
	struct board_config_struct board_config;
	u8 fdt_blob[CONFIG_FDT_SIZE];
};

struct config_types_info {
	enum mv_config_type_id config_id;
	char name[30];
	u32 byte_num;
	u32 byte_cnt;
};

#define BOARD_ID_INDEX_MASK		0x10
#define I2C_PAGE_WRITE_SIZE		32
#define EEPROM_STRUCT_SIZE		(sizeof(struct eeprom_struct) - sizeof(board_config_val.fdt_blob))
#define HW_INFO_MAX_PARAM_NUM		32
struct hw_info_point_struct {
	char *name;
	char *value;
};

#define HW_INFO_MAX_NAME_LEN		32
#define HW_INFO_MAX_VALUE_LEN		32
struct hw_info_data_struct {
	char name[HW_INFO_MAX_NAME_LEN];
	char value[HW_INFO_MAX_VALUE_LEN];
};

#define offset_in_eeprom(a)		((u32)(offsetof(struct eeprom_struct, a)))
#define get_default_fdt_config_id(boardid)	mapping_default_fdt[boardid & (BOARD_ID_INDEX_MASK - 1)]
#define boardid_is_valid(boardid)      (((boardid) >= MARVELL_BOARD_ID_BASE) && ((boardid) < MV_MAX_MARVELL_BOARD_ID))\
					   ? true : false

/* {{MV_CONFIG_TYPE_ID configId,	name,			byte_num,				byte_cnt}} */
#define MV_EEPROM_CONFIG_INFO { \
{ MV_CONFIG_VERIFICATION_PATTERN,	"EEPROM Pattern",	offset_in_eeprom(pattern),			  \
									sizeof(board_config_val.pattern)},	  \
{ MV_CONFIG_LENGTH,			"Data length",		offset_in_eeprom(length),			  \
									sizeof(board_config_val.length)},	  \
{ MV_CONFIG_HW_INFO,			"Box Information",	offset_in_eeprom(man_info.hw_info),		  \
									sizeof(board_config_val.man_info.hw_info)}, \
{ MV_CONFIG_FDTCFG_EN,			"EEPROM enable",	offset_in_eeprom(board_config.fdt_cfg_en),	  \
									sizeof(board_cfg->fdt_cfg_en)},		  \
{ MV_CONFIG_ACTIVE_FDT_SELECTION,	"Active FDT selection", offset_in_eeprom(board_config.active_fdt_selection),\
								sizeof(board_cfg->active_fdt_selection)},	  \
{ MV_CONFIG_FDTCFG_VALID,		"Validation counter",	offset_in_eeprom(board_config.validation_counter),\
								sizeof(board_cfg->validation_counter)},		  \
{ MV_CONFIG_FDT_FILE,			"FDT file",		offset_in_eeprom(fdt_blob),			  \
								sizeof(board_config_val.fdt_blob)}		  \
}

#define CFG_DEFAULT_VALUE  {											  \
				0x00000000,				     /* checksum */			  \
				0xfecadefa,				     /* EEPROM pattern */		  \
				EEPROM_STRUCT_SIZE,			     /* length = 0x114 bytes */		  \
				{{[0 ... (MVEBU_HW_INFO_LEN - 1)] = 0x00} },   /* manufacturing_information */	  \
				{0x00,					     /* fdt config disable */		  \
				 0x03,					     /* active fdt selection = default */ \
				 0x00,					     /* validation counter = 0 */	  \
				{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },/* reserve_board_cgf */		  \
				{[0 ... (CONFIG_FDT_SIZE - 1)] = 1}	     /* fdt file */			  \
}

#define MV_MAX_FDT_CONFIGURATION	MV_MARVELL_BOARD_NUM * 8

bool cfg_eeprom_fdt_config_is_enable(void);
bool cfg_eeprom_upload_fdt_from_flash(u8 fdt_config_id);
bool cfg_eeprom_upload_fdt_from_eeprom(uint8_t *fdt_blob);
void cfg_eeprom_save(uint8_t *fdt_blob, int with_fdt_blob);
struct eeprom_struct *cfg_eeprom_get_board_config(void);
void cfg_eeprom_get_hw_info_str(uchar *hw_info_str);
void cfg_eeprom_set_hw_info_str(uchar *hw_info_str);
int cfg_eeprom_parse_hw_info(uchar *hw_info_str, struct hw_info_point_struct *hw_info_point_array);
int cfg_eeprom_parse_env(struct hw_info_data_struct *hw_info_data_array, int size);
int cfg_eeprom_get_board_id(void);

#endif /* _CFG_EEPROM_H_ */
