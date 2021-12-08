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

static union mv_spd_data *spd_data;

u8 mv_ddr_spd_dev_type_get(void)
{
	// Regardless if device is DDR5 or DDR4, device type is always within byte 2:
	u8 dev_type = spd_data->ddr4_spd_data.byte_fields.byte_2;
	return dev_type;
}

bool is_ddr5(void)
{
	return (mv_ddr_spd_dev_type_get() == SPD_DRAM_TYPE_DDR5_SDRAM);
}

u8 mv_ddr_spd_module_type_detail_get(void)
{
	u8 type_detail = TYPE_SYNCHRONOUS;
	// Regardless if DDR5 or DDR4, module type is always within byte 3:
	u8 module_type = spd_data->ddr4_spd_data.byte_fields.byte_3.bit_fields.module_type;

	if (is_ddr5()) {
		// JESD400-5 DDR5 SPD Contents 0.96 -> RDIMM Overlay Schema
		if (module_type == SPD_RDIMM)
			type_detail |= TYPE_REGISTERED_BUFFERED;
		// JESD400-5 DDR5 SPD Contents 0.96 -> LRDIMM Overlay Schema
		if (module_type == SPD_LRDIMM)
			type_detail |= TYPE_LRDIMM;
		// JESD400-5 DDR5 SPD Contents 0.96 -> UDIMM Overlay Schema
		if (module_type == SPD_UDIMM || module_type == SPD_SO_DIMM)
			type_detail |= TYPE_UNBUFFERED_UNREGISTERED;
	} else {
		switch (module_type) {
		// DDR4 SPD spec -> RDIMM Overlay Schema
		case SPD_RDIMM:
		case SPD_MINI_RDIMM:
		case SPD_72B_SO_RDIMM:
			type_detail |= TYPE_REGISTERED_BUFFERED;
			break;
		// DDR4 SPD spec -> LRDIMM Overlay Schema
		case SPD_LRDIMM:
			type_detail |= TYPE_LRDIMM;
			break;
		// DDR4 SPD spec -> UDIMM Overlay Schema
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
	}

	return type_detail;
}

// Returns total bus width extension per DIMM
enum mv_ddr_bus_width_ext mv_ddr_spd_bus_width_ext_get(void)
{
	u8 bus_width_ext;
	enum mv_ddr_bus_width_ext ret_val;

	if (is_ddr5()) {	//DDR5
		bus_width_ext = spd_data->ddr5_spd_data.byte_fields.byte_235.bit_fields.bus_width_ext_per_channel;

		switch (bus_width_ext) {
		case 0x00:
			ret_val = MV_DDR_BUS_WIDTH_EXT_0;
			break;
		case 0x01:
			ret_val = MV_DDR_BUS_WIDTH_EXT_4;
			break;
		case 0x02:
			ret_val = MV_DDR_BUS_WIDTH_EXT_8;
			break;
		default:
			ret_val = MV_DDR_BUS_WIDTH_EXT_LAST;
			break;
		}
	} else {	//DDR4
		bus_width_ext = spd_data->ddr4_spd_data.byte_fields.byte_13.bit_fields.bus_width_ext;

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
	}

	if (is_ddr5()) {	// DDR5: width ext per channel * channels per DIMM = total width
		ret_val *= spd_data->ddr5_spd_data.byte_fields.byte_235.bit_fields.channel_per_dimm + 1;
	}

	return ret_val;
}

// Returns total primary bus width per DIMM
enum mv_ddr_pri_bus_width mv_ddr_spd_pri_bus_width_get(void)
{
	u8 pri_bus_width;
	enum mv_ddr_pri_bus_width ret_val;

	if (is_ddr5())
		pri_bus_width = spd_data->ddr5_spd_data.byte_fields.byte_235.bit_fields.primary_bus_width_per_channel;
	else
		pri_bus_width = spd_data->ddr4_spd_data.byte_fields.byte_13.bit_fields.primary_bus_width;

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

	if (is_ddr5()) { // DDR5: width per channel * channels per DIMM = total width
		ret_val *= spd_data->ddr5_spd_data.byte_fields.byte_235.bit_fields.channel_per_dimm + 1;
	}

	return ret_val;
}

u32 spd_module_type_to_dtmf_type(void)
{
	// Regardless if DDR5 or DDR4, module type is always within byte 3:
	u8 mod_type = spd_data->ddr4_spd_data.byte_fields.byte_3.bit_fields.module_type;

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

	case SPD_SOLDER_DOWN:
		return DTMF_TYPE17_FORM_FACTOR_ROW_OF_CHIPS;
	default:
		return 0;
	}
}

u32 spd_dramdev_type_to_dtmf_type(void)
{
	u8 dev_type = mv_ddr_spd_dev_type_get();

	switch (dev_type) {
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
	case    SPD_DRAM_TYPE_DDR5_SDRAM:
		return DTMF_TYPE17_TYPE_DDR5;
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

// Returns number of dies per package
u8 mv_ddr_spd_die_count_get(void)
{
	u8 die_cnt, val;

	if (is_ddr5()) {
		val = spd_data->ddr5_spd_data.byte_fields.byte_4.bit_fields.die_per_package;
		switch (val) {
		case 0x00:
			die_cnt = 1;
			break;
		case 0x02:
			die_cnt = 2;
			break;
		case 0x03:
			die_cnt = 4;
			break;
		case 0x04:
			die_cnt = 8;
			break;
		case 0x05:
			die_cnt = 16;
			break;
		default:
			die_cnt = 0;
		}
	} else {
		val = spd_data->ddr4_spd_data.byte_fields.byte_6.bit_fields.die_count;
		if (val >= 0 && val <= 7)
			die_cnt = val + 1;
		else
			die_cnt = 0;
	}
	return die_cnt;
}

// Returns SDRAM density per die in bits
u64 mv_ddr_spd_die_density_get(void)
{
	u64 die_density;
	u8 val;

	if (is_ddr5()) {
		val = spd_data->ddr5_spd_data.byte_fields.byte_4.bit_fields.sdram_density_per_die;
		switch (val) {
		case 0x01:
			die_density = _4G;
			break;
		case 0x02:
			die_density = _8G;
			break;
		case 0x03:
			die_density = _12G;
			break;
		case 0x04:
			die_density = _16G;
			break;
		case 0x05:
			die_density = _24G;
			break;
		case 0x06:
			die_density = _32G;
			break;
		case 0x07:
			die_density = _48G;
			break;
		case 0x08:
			die_density = _64G;
			break;
		default:
			die_density = 0;
		}
	} else {
		val = spd_data->ddr4_spd_data.byte_fields.byte_4.bit_fields.die_capacity;

		switch (val) {
		case 0x00:
			die_density = _256M;
			break;
		case 0x01:
			die_density = _512M;
			break;
		case 0x02:
			die_density = _1G;
			break;
		case 0x03:
			die_density = _2G;
			break;
		case 0x04:
			die_density = _4G;
			break;
		case 0x05:
			die_density = _8G;
			break;
		case 0x06:
			die_density = _16G;
			break;
		case 0x07:
			die_density = _32G;
			break;
		case 0x08:
			die_density = _12G;
			break;
		case 0x09:
			die_density = _24G;
			break;
		default:
			die_density = 0;
		}
	}
	return die_density;
}

u32 mv_ddr_spd_die_capacity_get(void)
{
	u64 total;	// Capacity in bytes;
	u8	ranks				= mv_ddr_spd_pkg_rank_get();		// Single Die Package, Dual Die Package, Quad Die Package
	u64 sdram_capacity		= mv_ddr_spd_die_density_get();	// In bits.
	u8  primary_bus_width	= mv_ddr_spd_pri_bus_width_get();	// In bits
	u8  die_cnt				= mv_ddr_spd_die_count_get();

	if (is_ddr5()) {
		// For DDR5 ranks represents Package Ranks per Channel
		u8 package_ranks_per_channel = ranks;
		u8 sdram_io_width = 4 << spd_data->ddr5_spd_data.byte_fields.byte_6.bit_fields.sdram_io_width;

		/* To calculate the total capacity in bytes for a symmetric module, the following math applies:
		 *		Capacity in bytes =
		 *		Number of channels per DIMM *
		 *		Primary bus width per channel / SDRAM I/O Width *
		 *		Die per package *
		 *		SDRAM density per die / 8 *
		 *		Package ranks per channel
		 */
		total = (primary_bus_width / sdram_io_width) *
				die_cnt *
				(sdram_capacity / 8) *
				package_ranks_per_channel;

		/* // Uncomment for debugging purposes
		 * printf("SPD: package_ranks_per_channel = %d\n", package_ranks_per_channel);
		 * printf("SPD: sdram_io_width = %d\n", sdram_io_width);
		 * printf("SPD: die_cnt = %d\n", die_cnt);
		 * printf("SPD: primary_bus_width = %d\n", primary_bus_width);
		 * printf("SPD: sdram_capacity = 0x%lx\n", sdram_capacity);
		 * printf("SPD: total = %lu\n", total);
		 */
	} else {
		// For DDR4 ranks represents Package Ranks per DIMM
		u8 logical_ranks_per_dimm = ranks;
		u8 sdram_width = 4 << spd_data->ddr4_spd_data.byte_fields.byte_12.bit_fields.device_width;

		// For Single load stack (3DS) modules recalculate
		if (die_cnt && spd_data->ddr4_spd_data.byte_fields.byte_6.bit_fields.signal_loading == SINGLE_LOAD_STACK_3DS)
			logical_ranks_per_dimm *= die_cnt;

		// DDR4: Calculating Module DRAM Capacity
		// Total = SDRAM Capacity / 8 * Primary Bus Width / SDRAM Width * Logical Ranks per DIMM
		total = (sdram_capacity /  8) * (primary_bus_width / sdram_width) * logical_ranks_per_dimm;
	}

	return ((u32) (total / 0x100000)); // Recalculate to MB
}

u32 get_dram_speed(void)
{
	u32 speed;

	if (is_ddr5()) {
		u16 tckavg_min = ((u16)	spd_data->ddr5_spd_data.byte_fields.byte_21 << 8) |
								spd_data->ddr5_spd_data.byte_fields.byte_20; // In picoseconds

		// See DDR5 SPD spec -> 1.0 Example SPD Codes tCKmin (Grade vs tCK table)
		switch (tckavg_min) {
		case 625:
			speed = 3200;
			break;
		case 555:
			speed = 3600;
			break;
		case 500:
			speed = 4000;
			break;
		case 454:
			speed = 4400;
			break;
		case 416:
			speed = 4800;
			break;
		case 384:
			speed = 5200;
			break;
		case 357:
			speed = 5600;
			break;
		case 333:
			speed = 6000;
			break;
		case 312:
			speed = 6400;
			break;
		case 294:
			speed = 6800;
			break;
		case 277:
			speed = 7200;
			break;
		case 263:
			speed = 7600;
			break;
		case 250:
			speed = 8000;
			break;
		case 238:
			speed = 8400;
			break;
		default:
			speed = 0;
		}
	} else {
		u8 mtb_units = spd_data->ddr4_spd_data.byte_fields.byte_18;
		// Refer to DDR4 SPD spec -> Byte 18 (0x012): SDRAM Minimum Cycle Time (tCKAVGmin)
		switch (mtb_units) {
		case 10:
			speed = 1600;
			break;
		case 9:
			speed = 1866;
			break;
		case 8:
			speed = 2133;
			break;
		case 7:
			speed = 2400;
			break;
		case 6:
			speed = 2666;
			break;
		case 5:
			speed = 3200;
			break;
		default:
			speed = 0;
		}
	}

	return speed;
}

u16 get_module_manufacturer_id(void)
{
	u16 manufacturer_code;

	if (is_ddr5()) {
		manufacturer_code = ((u16)spd_data->ddr5_spd_data.all_bytes[MODULE_MANUFACTURER_ID_MSB_INDEX_DDR5] << 8) |
				(spd_data->ddr5_spd_data.all_bytes[MODULE_MANUFACTURER_ID_LSB_INDEX_DDR5]);
	} else {
		manufacturer_code = ((u16)spd_data->ddr4_spd_data.all_bytes[MODULE_MANUFACTURER_ID_MSB_INDEX_DDR4] << 8) |
				(spd_data->ddr4_spd_data.all_bytes[MODULE_MANUFACTURER_ID_LSB_INDEX_DDR4]);
	}

	return manufacturer_code;
}

u16 get_dram_manufacturer_id(void)
{
	u16 manufacturer_code;

	if (is_ddr5()) {
		manufacturer_code = ((u16)spd_data->ddr5_spd_data.all_bytes[DRAM_MANUFACTURER_ID_MSB_INDEX_DDR5] << 8) |
				(spd_data->ddr5_spd_data.all_bytes[DRAM_MANUFACTURER_ID_LSB_INDEX_DDR5]);
	} else {
		manufacturer_code = ((u16)spd_data->ddr4_spd_data.all_bytes[DRAM_MANUFACTURER_ID_MSB_INDEX_DDR4] << 8) |
				(spd_data->ddr4_spd_data.all_bytes[DRAM_MANUFACTURER_ID_LSB_INDEX_DDR4]);
	}

	return manufacturer_code;
}

u32 get_dram_serial(void)
{
	u32 serial;

	if (is_ddr5())
		serial = *(u32 *)&spd_data->ddr5_spd_data.all_bytes[MODULE_SERIAL_NO_INDEX_DDR5];
	else
		serial = *(u32 *)&spd_data->ddr4_spd_data.all_bytes[MODULE_SERIAL_NO_INDEX_DDR4];

	return serial;
}

void get_dram_module_part_no(char *str)
{
	char *module_part_no;

	if (is_ddr5()) {
		module_part_no = (char *)&spd_data->ddr5_spd_data.all_bytes[MODOULE_PART_NO_DDR5];
		memcpy(str,  module_part_no, MODULE_PART_LENGTH_DDR5);
		str[MODULE_PART_LENGTH_DDR5 + 1] = 0;
	} else {
		module_part_no = (char *)&spd_data->ddr4_spd_data.all_bytes[MODOULE_PART_NO_DDR4];
		memcpy(str,  module_part_no, MODULE_PART_LENGTH_DDR4);
		str[MODULE_PART_LENGTH_DDR4 + 1] = 0;
	}
}

u8 get_product_id(void)
{
	if (is_ddr5())
		return spd_data->ddr5_spd_data.all_bytes[MODOULE_REVISION_CODE_DDR5];
	else
		return spd_data->ddr4_spd_data.all_bytes[MODOULE_REVISION_CODE_DDR4];
}

// For DDR5: returns ranks per channel
// For DDR4: returns ranks per DIMM
enum mv_ddr_pkg_rank mv_ddr_spd_pkg_rank_get(void)
{
	u8 pkg_rank;

	if (is_ddr5()) {	// DDR5: Ranks per channel
		pkg_rank = spd_data->ddr5_spd_data.byte_fields.byte_234.bit_fields.package_rank_per_channel;
	} else {	// DDR4: Ranks per DIMM
		pkg_rank = spd_data->ddr4_spd_data.byte_fields.byte_12.bit_fields.dimm_pkg_ranks_num;
	}

	// Valid rank range according to SPD spec for DDR4/DDR5: <0-7>
	if (pkg_rank >= 0 && pkg_rank <= 7)
		return (pkg_rank+1);
	else
		return MV_DDR_PKG_RANK_LAST;
}

u16 get_dram_min_volt(void)
{
	if (is_ddr5()) {
		if (spd_data->ddr5_spd_data.byte_fields.byte_16.bit_fields.nominal == 0)
			return 1100; // 1.1V
	} else {
		if (spd_data->ddr4_spd_data.byte_fields.byte_11.bit_fields.operable)
			return 1200; // 1.2V
	}

	return 0;
}

u16 get_dram_max_volt(void)
{
	return get_dram_min_volt(); // DDR4:1.2V, DDR5:1.1V -> max voltage is the same as min
}

u16 get_dram_configured_volt(void)
{
	/* TBD by OEM, the voltage they run */
	return get_dram_min_volt();
}

void get_dram_info_init(void *spd)
{
	spd_data = map_sysmem((phys_addr_t)spd, 0); // Last parameter doesn't matter, so set to 0.
}
