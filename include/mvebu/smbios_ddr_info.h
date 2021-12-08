/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2020, Sujeet Baranwal <sbaranwal@marvell.com>
 *
 */

#ifndef _SMBIOS_DDR_INFO_H_
#define _SMBIOS_DDR_INFO_H_

#include <linux/bitops.h>

#define _256M	0x10000000
#define _512M	0x20000000

#define _1G		0x40000000
#define _2G		0x80000000
#define _4G		0x100000000
#define _8G		0x200000000
#define _12G    0x240000000
#define _16G	0x400000000
#define _24G    0x600000000
#define _32G	0x800000000
#define _48G    0xc00000000
#define _64G	0x1000000000
#define _128G	0x2000000000

// DDR4 specific register offsets:
#define MODULE_MANUFACTURER_ID_LSB_INDEX_DDR4	320
#define MODULE_MANUFACTURER_ID_MSB_INDEX_DDR4	321
#define MODULE_SERIAL_NO_INDEX_DDR4				325 // 325~328
#define MODOULE_PART_NO_DDR4					329	// 329~348
#define MODOULE_REVISION_CODE_DDR4				349
#define DRAM_MANUFACTURER_ID_LSB_INDEX_DDR4		350
#define DRAM_MANUFACTURER_ID_MSB_INDEX_DDR4		351

#define MODULE_MANUFACTURER_ID_LSB_INDEX_DDR5	512
#define MODULE_MANUFACTURER_ID_MSB_INDEX_DDR5	513
#define MODULE_SERIAL_NO_INDEX_DDR5				517 // 517~520
#define MODOULE_PART_NO_DDR5					521 // 521~550
#define MODOULE_REVISION_CODE_DDR5				551
#define DRAM_MANUFACTURER_ID_LSB_INDEX_DDR5		552
#define DRAM_MANUFACTURER_ID_MSB_INDEX_DDR5		553

/* Memory type */
#define SPD_RDIMM							0x1
#define SPD_UDIMM							0x2
#define SPD_SO_DIMM							0x3
#define SPD_LRDIMM							0x4
#define SPD_MINI_RDIMM						0x5
#define SPD_MINI_UDIMM						0x6
#define SPD_72B_SO_RDIMM					0x8
#define SPD_72B_SO_UDIMM					0x9
#define SPD_DDIMM							0xA
#define SPD_SOLDER_DOWN						0xB
#define SPD_16B_SO_DIMM						0xC
#define SPD_32B_SO_DIMM						0xD

#define DTMF_TYPE17_FORM_FACTOR_DIMM			0x09
#define DTMF_TYPE17_FORM_FACTOR_ROW_OF_CHIPS	0x0B
#define DTMF_TYPE17_FORM_FACTOR_SODIMM			0x0D



/* DRAM type */
#define SPD_DRAM_TYPE_FAST_PAGE_MODE			0x01
#define SPD_DRAM_TYPE_EDO						0x02
#define SPD_DRAM_TYPE_PIPELINED_NIBBLE			0x03
#define SPD_DRAM_TYPE_SDRAM						0x04
#define SPD_DRAM_TYPE_ROM						0x05
#define SPD_DRAM_TYPE_DDR_SGRAM					0x06
#define SPD_DRAM_TYPE_DDR_SDRAM					0x07
#define SPD_DRAM_TYPE_DDR2_SDRAM				0x08
#define SPD_DRAM_TYPE_DDR2_SDRAM_FB_DIMM		0x09
#define SPD_DRAM_TYPE_DDR2_SDRAM_FB_DIMM_PROBE	0x0A
#define SPD_DRAM_TYPE_DDR3_SDRAM				0x0B
#define SPD_DRAM_TYPE_DDR4_SDRAM				0x0C
#define SPD_DRAM_TYPE_DDR5_SDRAM				0x12

#define DTMF_TYPE17_TYPE_SGRAM					0x10
#define DTMF_TYPE17_TYPE_ROM					0x08
#define DTMF_TYPE17_TYPE_SDRAM					0x0F
#define DTMF_TYPE17_TYPE_DDR					0x12
#define DTMF_TYPE17_TYPE_DDR2					0x13
#define DTMF_TYPE17_TYPE_DDR2_FB_DIMM			0x14
#define DTMF_TYPE17_TYPE_DDR3					0x18
#define DTMF_TYPE17_TYPE_DDR4					0x1A
#define DTMF_TYPE17_TYPE_DDR5					0x22

/* Bus width */
enum mv_ddr_bus_width_ext { /* number of extension bus width bits */
	MV_DDR_BUS_WIDTH_EXT_0		= 0,
	MV_DDR_BUS_WIDTH_EXT_4		= 4,
	MV_DDR_BUS_WIDTH_EXT_8		= 8,
	MV_DDR_BUS_WIDTH_EXT_LAST	= 9
};

enum mv_ddr_pri_bus_width { /* number of primary bus width bits */
	MV_DDR_PRI_BUS_WIDTH_8		= 8,
	MV_DDR_PRI_BUS_WIDTH_16		= 16,
	MV_DDR_PRI_BUS_WIDTH_32		= 32,
	MV_DDR_PRI_BUS_WIDTH_64		= 64,
	MV_DDR_PRI_BUS_WIDTH_LAST	= 65
};

/* Memory Size */
#define MV_DDR_DIE_CAP_32MB		0x20
#define MV_DDR_DIE_CAP_64MB		0x40
#define	MV_DDR_DIE_CAP_128MB	0x80
#define MV_DDR_DIE_CAP_256MB	0x100
#define MV_DDR_DIE_CAP_512MB	0x200
#define MV_DDR_DIE_CAP_1GB		0x400
#define MV_DDR_DIE_CAP_2GB		0x800
#define MV_DDR_DIE_CAP_4GB		0x1000
#define MV_DDR_DIE_CAP_1_5GB	0x600
#define MV_DDR_DIE_CAP_3GB		0xC00

/* Rank */
enum mv_ddr_pkg_rank { /* number of package ranks per dimm */
	MV_DDR_PKG_RANK_1 = 1,
	MV_DDR_PKG_RANK_2,
	MV_DDR_PKG_RANK_3,
	MV_DDR_PKG_RANK_4,
	MV_DDR_PKG_RANK_5,
	MV_DDR_PKG_RANK_6,
	MV_DDR_PKG_RANK_7,
	MV_DDR_PKG_RANK_8,
	MV_DDR_PKG_RANK_LAST
};

#define	TYPE_SYNCHRONOUS				BIT(7)
#define	TYPE_REGISTERED_BUFFERED		BIT(13)
#define	TYPE_UNBUFFERED_UNREGISTERED	BIT(14)
#define	TYPE_LRDIMM						BIT(15)

/* Misc */
#define SET_1					1
#define MODULE_PART_LENGTH_DDR4	20
#define MODULE_PART_LENGTH_DDR5	30
#define SINGLE_LOAD_STACK_3DS	0x2
#define DDR_SPEED_INFO_INDEX	MV_DDR_SPD_DATA_BLOCK0_SIZE

u8 mv_ddr_spd_dev_type_get(void);
u32 bus_data_width(void);
u32 get_dram_speed(void);
u8	get_product_id(void);
u32 bus_total_width(void);
u32 get_dram_serial(void);
u16 get_dram_max_volt(void);
u16 get_dram_min_volt(void);
void get_dram_info_init(void *spd);
u16 get_dram_manufacturer_id(void);
u16 get_dram_configured_volt(void);
u16 get_module_manufacturer_id(void);
void get_dram_module_part_no(char *str);
u32 mv_ddr_spd_die_capacity_get(void);
u32 spd_module_type_to_dtmf_type(void);
u32 spd_dramdev_type_to_dtmf_type(void);
enum mv_ddr_pkg_rank mv_ddr_spd_pkg_rank_get(void);

#endif /* _SMBIOS_DDR_INFO_H_ */
