// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020, Sujeet Baranwal <sbaranwal@marvell.com>
 *
 */

#include <stdint.h>
#include <common.h>
#include <env.h>
#include <mapmem.h>
#include <smbios.h>
#include <tables_csum.h>
#include <version.h>
#include <mapmem.h>
#include <mvebu/mv_ddr_spd.h>
#include <mvebu/smbios_ddr_info.h>

static union mv_ddr_spd_data *spd_data;

u8 mv_ddr_spd_dev_type_get(void)
{
	u8 dev_type = spd_data->byte_fields.byte_2;
	return dev_type;
}

u8 mv_ddr_spd_module_type_get(void)
{
	u8 type_detail = TYPE_SYNCHRONOUS;
	u8 module_type = spd_data->byte_fields.byte_3.bit_fields.module_type;

	switch (module_type) {
	case SPD_RDIMM:
	case SPD_MINI_RDIMM:
	case SPD_72B_SO_RDIMM:
		type_detail |= TYPE_REGISTERED_BUFFERED;
		break;
	default:
		break;
	}

	switch (module_type) {
	case SPD_UDIMM:
	case SPD_SO_DIMM:
	case SPD_MINI_UDIMM:
	case SPD_72B_SO_UDIMM:
	case SPD_16B_SO_DIMM:
	case SPD_32B_SO_DIMM:
		type_detail |= TYPE_UNBUFFERED_UNREGISTERED;
		break;
	default:
		break;
	}

	return type_detail;
}

enum mv_ddr_pkg_rank mv_ddr_spd_bus_width_ext_get(void)
{
	u8 bus_width_ext = spd_data->byte_fields.byte_13.bit_fields.bus_width_ext;
	enum mv_ddr_bus_width_ext ret_val;

	switch (bus_width_ext) {
	case 0x00:
		ret_val = MV_DDR_BUS_WIDTH_EXT_0;
		break;
	case 0x01:
		ret_val = MV_DDR_BUS_WIDTH_EXT_8;
		break;
	default:
		ret_val = MV_DDR_BUS_WIDTH_EXT_LAST;
		break;
	}
	return ret_val;
}

u32 mv_ddr_spd_pri_bus_width_get(void)
{
	u8 pri_bus_width = spd_data->byte_fields.byte_13.bit_fields.primary_bus_width;
	enum mv_ddr_pri_bus_width ret_val;

	switch (pri_bus_width) {
	case 0x00:
		ret_val = MV_DDR_PRI_BUS_WIDTH_8;
		break;
	case 0x01:
		ret_val = MV_DDR_PRI_BUS_WIDTH_16;
		break;
	case 0x02:
		ret_val = MV_DDR_PRI_BUS_WIDTH_32;
		break;
	case 0x03:
		ret_val = MV_DDR_PRI_BUS_WIDTH_64;
		break;
	default:
		ret_val = MV_DDR_PRI_BUS_WIDTH_LAST;
	}
	return ret_val;
}

u32 spd_module_type_to_dtmf_type(void)
{
	u8 mod_type = mv_ddr_spd_module_type_get();

	switch (mod_type) {
	case SPD_RDIMM:
	case SPD_UDIMM:
	case SPD_LRDIMM:
	case SPD_MINI_RDIMM:
	case SPD_MINI_UDIMM:
		return DTMF_TYPE17_FORM_FACTOR_DIMM;

	case SPD_SO_DIMM:
	case SPD_72B_SO_RDIMM:
	case SPD_72B_SO_UDIMM:
	case SPD_16B_SO_DIMM:
	case SPD_32B_SO_DIMM:
		return DTMF_TYPE17_FORM_FACTOR_SODIMM;

	default:
		return 0;
	}
}

u32 spd_dramdev_type_to_dtmf_type(void)
{
	u8 mod_type = mv_ddr_spd_dev_type_get();

	switch (mod_type) {
	case	SPD_DRAM_TYPE_SDRAM:
		return DTMF_TYPE17_TYPE_SDRAM;
	case    SPD_DRAM_TYPE_ROM:
		return DTMF_TYPE17_TYPE_ROM;
	case    SPD_DRAM_TYPE_DDR_SGRAM:
		return DTMF_TYPE17_TYPE_SGRAM;
	case    SPD_DRAM_TYPE_DDR_SDRAM:
		return DTMF_TYPE17_TYPE_SDRAM;

	case    SPD_DRAM_TYPE_DDR2_SDRAM:
		return DTMF_TYPE17_TYPE_DDR2;

	case    SPD_DRAM_TYPE_DDR2_SDRAM_FB_DIMM:
		return DTMF_TYPE17_TYPE_DDR2_FB_DIMM;

	case    SPD_DRAM_TYPE_DDR2_SDRAM_FB_DIMM_PROBE:
		return DTMF_TYPE17_TYPE_DDR2_FB_DIMM;

	case    SPD_DRAM_TYPE_DDR3_SDRAM:
		return DTMF_TYPE17_TYPE_DDR3;
	case    SPD_DRAM_TYPE_DDR4_SDRAM:
		return DTMF_TYPE17_TYPE_DDR4;
	default:
		return	0;
	}
}

u32 bus_total_width(void)
{
	u32 data_width = mv_ddr_spd_pri_bus_width_get();
	u32 ext_width = mv_ddr_spd_bus_width_ext_get();

	return data_width + ext_width;
}

u32 bus_data_width(void)
{
	u32 data_width = mv_ddr_spd_pri_bus_width_get();

	return data_width;
}

u32 mv_ddr_spd_die_capacity_get(void)
{
	u32 sdram_capacity, primary_bus_width, sdram_width;
	u32 logical_ranks_per_dimm, total;

	/* die_capacity = 0 => 256Mbits, from here, it doubles */
	sdram_capacity = 256 << spd_data->byte_fields.byte_4.bit_fields.die_capacity;
	primary_bus_width = 8 << spd_data->byte_fields.byte_13.bit_fields.primary_bus_width;
	sdram_width = 4 << spd_data->byte_fields.byte_12.bit_fields.device_width;

	/* for SDP, DDP, QDP */
	logical_ranks_per_dimm =  spd_data->byte_fields.byte_12.bit_fields.dimm_pkg_ranks_num + 1;

	/* for 3DS */
	if (spd_data->byte_fields.byte_6.bit_fields.signal_loading == SINGLE_LOAD_STACK_3DS)
		if (spd_data->byte_fields.byte_6.bit_fields.die_count)
			logical_ranks_per_dimm *= (spd_data->byte_fields.byte_6.bit_fields.die_count + 1);

	total = (sdram_capacity /  8) * (primary_bus_width / sdram_width) * logical_ranks_per_dimm;
	return total;
}

u32 get_dram_speed(void)
{
	u32 speed = *(u32 *)&spd_data->all_bytes[DDR_SPEED_INFO_INDEX];

	/* Speed would be doubled because of DDR */
	return speed * 2;
}

u32 get_module_manufacturer_id(void)
{
	u32 manufacturer_code = ((u32)spd_data->all_bytes[MODULE_MANUFACTURER_ID_MSB_INDEX] << 8) |
		(spd_data->all_bytes[MODULE_MANUFACTURER_ID_LSB_INDEX]);
	return manufacturer_code;
}

u32 get_dram_manufacturer_id(void)
{
	u32 manufacturer_code = ((u32)spd_data->all_bytes[DRAM_MANUFACTURER_ID_MSB_INDEX] << 8) |
		(spd_data->all_bytes[DRAM_MANUFACTURER_ID_LSB_INDEX]);
	return manufacturer_code;
}

u32 get_dram_serial(void)
{
	u32 serial = *(u32 *)&spd_data->all_bytes[MODULE_SERIAL_NO_INDEX];
	return serial;
}

void get_dram_module_part_no(char *str)
{
	char *module_part_no = (char *)&spd_data->all_bytes[MODOULE_PART_NO];

	memcpy(str,  module_part_no, MODULE_PART_LENGTH);
	str[MODULE_PART_LENGTH + 1] = 0;
}

u32 get_product_id(void)
{
	u16 prod_id =  spd_data->all_bytes[MODOULE_REVISION_CODE];
	return prod_id;
}

enum mv_ddr_pkg_rank mv_ddr_spd_pkg_rank_get(void)
{
	u8 pkg_rank = spd_data->byte_fields.byte_12.bit_fields.dimm_pkg_ranks_num;
	enum mv_ddr_pkg_rank ret_val;

	switch (pkg_rank) {
	case MV_DDR_PKG_RANK_1:
	case MV_DDR_PKG_RANK_2:
	case MV_DDR_PKG_RANK_3:
	case MV_DDR_PKG_RANK_4:
		ret_val = pkg_rank + 1;
		break;
	default:
		ret_val = MV_DDR_PKG_RANK_LAST;
	}
	return ret_val;
}

u16 get_dram_min_volt(void)
{
	if (spd_data->byte_fields.byte_11.bit_fields.operable_1V)
		return 1000;

	if (spd_data->byte_fields.byte_11.bit_fields.operable_1_2V)
		return 1200;

	if (spd_data->byte_fields.byte_11.bit_fields.operable_2V)
		return 2000;

	return 0;
}

u16 get_dram_max_volt(void)
{
	if (spd_data->byte_fields.byte_11.bit_fields.operable_2V)
		return 2000;

	if (spd_data->byte_fields.byte_11.bit_fields.operable_1_2V)
		return 1200;

	if (spd_data->byte_fields.byte_11.bit_fields.operable_1V)
		return 1000;

	return 0;
}

u16 get_dram_configured_volt(void)
{
	/* TBD by OEM, the voltage they run */
	return 1200;
}

void get_dram_info_init(void *spd)
{
	spd_data = map_sysmem(SHARED_DDR_BTW_BLE_UBOOT, 384);
}
