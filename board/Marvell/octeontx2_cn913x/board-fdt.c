// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <env.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <linux/libfdt.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <ddr_spd.h>
#include <mvebu/smbios_ddr_info.h>

DECLARE_GLOBAL_DATA_PTR;

#define MAX_SCKT	1
#define MAX_CTRL	1
#define MAX_DIMM	1

static u8 num_sckt;
static u8 dimm_present[MAX_SCKT][MAX_CTRL][MAX_DIMM];
static const u8 *spd_cache[MAX_SCKT][MAX_CTRL][MAX_DIMM];

extern unsigned long fdt_base_addr;

/*
 * This structure provides the address mapping for a Physical
 * Memory Array
 */
static void smbios_update_type19(void)
{
	u32 i;
	char tmp_str[40];
	int node_offset;
	struct fdt_resource res;
	void *fdt = (void *)gd->fdt_blob;

	i = 0;
	do {
		sprintf(tmp_str, "/memory@%d", i);
		node_offset = fdt_path_offset(fdt, tmp_str);
		if (node_offset < 0)
			continue;

		if (fdt_get_resource(fdt, node_offset, "reg", 0, &res))
			continue;

		sprintf(tmp_str, "/uboot-smbios/type19@%d", i);
		node_offset = fdt_path_offset(fdt, tmp_str);
		if (node_offset < 0)
			continue;

		if (res.start < 0x100000000 && res.end < 0x100000000) {
			fdt_setprop_inplace_u32(fdt, node_offset, "start-addr", res.start);
			fdt_setprop_inplace_u32(fdt, node_offset, "end-addr", res.end);
		} else {
			fdt_setprop_inplace_u64(fdt, node_offset, "ext-start-addrs", res.start);
			fdt_setprop_inplace_u64(fdt, node_offset, "ext-end-addrs", res.end);
		}
		i++;

	} while (node_offset > 0);
}

static void smbios_add_ddr_info(u8 dimm)
{
	u32 tmp_val32;
	char tmp_str[40];
	int node_offset;
	void *fdt = (void *)gd->fdt_blob;

	sprintf(tmp_str, "/uboot-smbios/type17@%d", dimm);
	node_offset = fdt_path_offset(fdt, tmp_str);
	if (node_offset < 0)
		return;

	tmp_val32 = mv_ddr_spd_die_capacity_get();
	if (tmp_val32 < 0x7FFF) {
		fdt_setprop_inplace_u32(fdt, node_offset, "size", tmp_val32);
	} else {
		fdt_setprop_inplace_u32(fdt, node_offset, "size", 0x7FFF);
		fdt_setprop_inplace_u32(fdt, node_offset, "ext-size", tmp_val32);
	}
	fdt_setprop_inplace_u64(fdt, node_offset, "vol-sizes", tmp_val32 * 1024 * 1024);
	fdt_setprop_inplace_u64(fdt, node_offset, "log-sizes", tmp_val32 * 1024 * 1024);

	fdt_setprop_inplace_u32(fdt, node_offset, "total-width", bus_total_width());
	fdt_setprop_inplace_u32(fdt, node_offset, "data-width", bus_data_width());
	fdt_setprop_inplace_u32(fdt, node_offset, "form-factor", spd_module_type_to_dtmf_type());
	fdt_setprop_inplace_u32(fdt, node_offset, "mem-type", spd_dramdev_type_to_dtmf_type());

	fdt_setprop_inplace_u32(fdt, node_offset, "speed", get_dram_speed());
	fdt_setprop_inplace_u32(fdt, node_offset, "configured-memory-speed", get_dram_speed());
	fdt_setprop_inplace_u32(fdt, node_offset, "attributes", mv_ddr_spd_pkg_rank_get());
	fdt_setprop_inplace_u32(fdt, node_offset, "module-product-id", get_product_id());
	fdt_setprop_inplace_u32(fdt, node_offset, "minimum_voltage", get_dram_min_volt());
	fdt_setprop_inplace_u32(fdt, node_offset, "maximum_voltage", get_dram_max_volt());
	fdt_setprop_inplace_u32(fdt, node_offset, "configured_voltage", get_dram_configured_volt());
}

/*
 * This structure describes a single memory device that is
 * part of a larger Physical Memory Array  structure.
 */
void smbios_update_type17(void)
{
	u8 i, j, k;
	u8 dimm_num;

	dimm_num = 0;
	for (i = 0; i < MAX_SCKT; i++) {
		for (j = 0; j < MAX_CTRL; j++) {
			for (k = 0; k < MAX_DIMM; k++) {
				if (!dimm_present[i][j][k] || !spd_cache[i][j][k]) {
					dimm_num++;
					continue;
				}
				get_dram_info_init((u8 *)spd_cache[i][j][k]);
				smbios_add_ddr_info(dimm_num);
				dimm_num++;
			}
		}
	}
}

u64 fdt_get_board_info(void)
{
	int node, ret, i;
	char tmp_str[80];
	const void *fdt = gd->fdt_blob;
	int sckt_index, ctrl_index, dimm_index;

	if (!fdt) {
		printf("ERROR: %s: no valid device tree found\n", __func__);
		return 0;
	}

	ret = fdt_check_header(fdt);
	if (ret < 0) {
		printf("fdt: %s\n", fdt_strerror(ret));
		return 0;
	}

	/* Figure out number of sockets on this board */
	i = 0;
	do {
		sprintf(tmp_str, "/memory@%d", i);
		node = fdt_path_offset(fdt, tmp_str);
		if (node > 0)
			num_sckt++;
		i++;
	} while (node > 0);

	memset(spd_cache, 0, sizeof(spd_cache));

	for (sckt_index = 0; sckt_index < MAX_SCKT; sckt_index++) {
		for (ctrl_index = 0; ctrl_index < MAX_CTRL; ctrl_index++) {
			for (dimm_index = 0; dimm_index < MAX_DIMM; dimm_index++) {
				sprintf(tmp_str,
					"DDR-CONFIG-SPD-DATA.DIMM%d.LMC%d.N%d", dimm_index, ctrl_index, sckt_index);

				spd_cache[sckt_index][ctrl_index][dimm_index] = (u8 *)SHARED_DDR_BTW_BLE_UBOOT;

				if (spd_cache[sckt_index][ctrl_index][dimm_index])
					dimm_present[sckt_index][ctrl_index][dimm_index] = 1;
				else
					dimm_present[sckt_index][ctrl_index][dimm_index] = 0;
			}
		}
	}

	smbios_update_type17();
	smbios_update_type19();

	return 0;
}
