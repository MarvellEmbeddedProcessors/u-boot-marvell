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

#ifndef _FDT_EEPROM_H_
#define _FDT_EEPROM_H_
#include <common.h>
#include <i2c.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/board-info.h>
#include <asm/arch-mvebu/mvebu.h>

enum mv_config_type_id {
	MV_CONFIG_VERIFICATION_PATTERN,
	MV_CONFIG_LENGTH,
	MV_CONFIG_BOARDID,
	MV_CONFIG_FDTCFG_EN,
	MV_CONFIG_ACTIVE_FDT_SELECTION,
	MV_CONFIG_FDT_FILE,
	MV_CONFIG_FDTCFG_VALID,
	MV_CONFIG_TYPE_MAX_OPTION,  /* limit for user read/write routines */
};

/* #pragma pack(1) */
struct manufacturing_information_struct_info {
	u8 boardid;
	u8 reserve_manufacturing_information[23];
};

/* #pragma pack(1) */
struct board_config_struct_info {
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
	struct manufacturing_information_struct_info man_info;
	struct board_config_struct_info board_config;
	u8 fdt_blob[MVEBU_FDT_SIZE];
};

struct  fdt_config_types_info {
	enum mv_config_type_id config_id;
	char name[30];
	u8 byte_num;
	u32 byte_cnt;
};

#define BOARD_ID_INDEX_MASK		0x10
#define I2C_PAGE_WRITE_SIZE		32

#define offset_in_eeprom(a)		((u8)(offsetof(struct eeprom_struct, a)))
#define get_default_fdt_config_id(boardid)	mapping_default_fdt[boardid & (BOARD_ID_INDEX_MASK - 1)]
#define boardid_is_valid(boardid)      (((boardid) >= MARVELL_BOARD_ID_BASE) && ((boardid) < MV_MAX_MARVELL_BOARD_ID))\
					   ? true : false

/* {{MV_CONFIG_TYPE_ID configId,	name,			byte_num,				byte_cnt}} */
#define MV_EEPROM_CONFIG_INFO { \
{ MV_CONFIG_VERIFICATION_PATTERN,	"EEPROM Pattern",	offset_in_eeprom(pattern),			  \
									sizeof(fdt_config_val.pattern)},	  \
{ MV_CONFIG_LENGTH,			"Data length",		offset_in_eeprom(length),			  \
									sizeof(fdt_config_val.length)},		  \
{ MV_CONFIG_BOARDID,			"Board ID",		offset_in_eeprom(man_info.boardid),		  \
									sizeof(fdt_config_val.man_info.boardid)}, \
{ MV_CONFIG_FDTCFG_EN,			"EEPROM enable",	offset_in_eeprom(board_config.fdt_cfg_en),	  \
									sizeof(board_cfg->fdt_cfg_en)},		  \
{ MV_CONFIG_ACTIVE_FDT_SELECTION,	"Active FDT selection", offset_in_eeprom(board_config.active_fdt_selection),\
								sizeof(board_cfg->active_fdt_selection)},	  \
{ MV_CONFIG_FDTCFG_VALID,		"Validation counter",	offset_in_eeprom(board_config.validation_counter),\
								sizeof(board_cfg->validation_counter)},		  \
{ MV_CONFIG_FDT_FILE,			"FDT file",		offset_in_eeprom(fdt_blob),			  \
								sizeof(fdt_config_val.fdt_blob)}		  \
}

#define FDT_DEFAULT_VALUE  {											  \
				0x00000000,				     /* checksum */			  \
				0xfecadefa,				     /* EEPROM pattern */		  \
				0x002c,					     /* length = 44 bytes */		  \
				{MV_DEFAULT_BOARD_ID,		/* board ID */			  \
				{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,					  \
				 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,				  \
				 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },/* reserve_man_information */  \
				{0x00,					     /* fdt config disable */		  \
				 0x03,					     /* active fdt selection = default */ \
				 0x00,					     /* validation counter = 0 */	  \
				{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },/* reserve_board_cgf */		  \
				{ [0 ... 7167] = 1 }			     /* fdt file */			  \
}

	#define MV_MAX_FDT_CONFIGURATION	MV_MARVELL_BOARD_NUM * 8
	extern struct eeprom_struct fdt_config_val;
	uint32_t mvebu_checksum8(uint8_t *start, uint32_t len);
	bool fdt_config_is_enable(void);
	bool upload_fdt_from_flash(u8 fdt_config_id);
	bool upload_fdt_from_eeprom(void);
	void write_fdt_struct_to_eeprom(void);
	u8 *mvebu_fdt_config_init(void);

#endif
