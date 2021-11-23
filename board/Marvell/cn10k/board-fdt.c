// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020-2021 Marvell
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <env.h>
#include <log.h>
#include <linux/io.h>
#include <linux/compiler.h>
#include <linux/libfdt.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <asm/arch/smc.h>
#include <asm/arch/board.h>

DECLARE_GLOBAL_DATA_PTR;

const char *fdt_get_run_platform(void)
{
	int node, ret, len = 32;
	const void *fdt = gd->fdt_blob;
	const char *str = NULL;

	if (!fdt) {
		printf("ERROR: %s: no valid device tree found\n", __func__);
		return str;
	}

	ret = fdt_check_header(fdt);
	if (ret < 0) {
		printf("fdt: %s\n", fdt_strerror(ret));
		return str;
	}

	node = fdt_path_offset(fdt, "/soc@0");
	if (node < 0) {
		printf("%s: /soc is missing from device tree: %s\n",
		       __func__, fdt_strerror(node));
		return str;
	}

	str = fdt_getprop(fdt, node, "runplatform", &len);
	if (!str)
		printf("Error: cannot retrieve platform from fdt\n");
	return str;
}

static int fdt_get_bdk_node(void)
{
	int node, ret;
	const void *fdt = gd->fdt_blob;

	if (!fdt) {
		printf("ERROR: %s: no valid device tree found\n", __func__);
		return 0;
	}

	ret = fdt_check_header(fdt);
	if (ret < 0) {
		printf("fdt: %s\n", fdt_strerror(ret));
		return 0;
	}

	node = fdt_path_offset(fdt, "/cavium,bdk");
	if (node < 0) {
		printf("%s: /cavium,bdk is missing from device tree: %s\n",
		       __func__, fdt_strerror(node));
		return 0;
	}
	return node;
}

#if CONFIG_IS_ENABLED(GENERATE_SMBIOS_TABLE)

#include <ddr_spd.h>
#include <mvebu/smbios_ddr_info.h>

#define MAX_DSS	3

static u8 dimm_present[MAX_DSS] = {0};
static const u8 *spd_cache[MAX_DSS] = {NULL};
static const u16 spd_cache_len[MAX_DSS] = {0};
int cavium_bdk;

static void smbios_update_type7(void)
{
	u32 val;
	int node_offset;
	void *fdt = (void *)gd->fdt_blob;
	const u32 *reg;
	u8 cache_index;
	char node_name[32] = "";
	u32 cache_size[4] = {0};

	debug("SMBIOS: %s() entry\n", __func__);

	node_offset = fdt_path_offset(fdt, "/l3-cache");
	if (node_offset < 0)
		return;

	debug("SMBIOS: /l3-cache node found in FDT\n");

	reg = fdt_getprop(fdt, node_offset, "cache-size", NULL);
	if (!reg)
		return;
	val = fdt32_to_cpu(*reg);

	debug("SMBIOS: L3 cache size = %d KB\n", val/1024);

	node_offset = fdt_path_offset(fdt, "/uboot-smbios/type7@3");
	if (node_offset < 0)
		return;

	debug("SMBIOS: type 7 node for L3 cache found in FDT\n");

	// Node provides size in bytes. Recalculate for 64K granularity (see SMBIOS spec):
	val = (val / 0x10000) | BIT(15);
	fdt_setprop_inplace_u32(fdt, node_offset, "maxsize", val);
	fdt_setprop_inplace_u32(fdt, node_offset, "installed-size", val);

	debug("SMBIOS: updated L3 cache maxsize and installed-size to 0x%x\n", val);

	// Getting L1 and L2 caches sizes from core 0
	node_offset = fdt_path_offset(fdt, "/cpus/cpu@0");
	if (node_offset > 0) {
		debug("SMBIOS: /cpus/cpu@0 node found in FDT\n");

		// Get L1 Instruction Cache size
		reg = fdt_getprop(fdt, node_offset, "i-cache-size", NULL);
		if (reg) {
			// Node provides size in bytes for given core. Increase total size in KB:
			cache_size[0] = fdt32_to_cpu(*reg) / 1024;
			debug("SMBIOS: core L1 instruction cache size = %d KB\n", cache_size[0]);
		}

		// Get L1 Data Cache size
		reg = fdt_getprop(fdt, node_offset, "d-cache-size", NULL);
		if (reg) {
			// Node provides size in bytes for given core. Increase total size in KB:
			cache_size[1] = fdt32_to_cpu(*reg) / 1024;
			debug("SMBIOS: core L1 data cache size        = %d KB\n", cache_size[1]);
		}

		node_offset = fdt_path_offset(fdt, "/cpus/cpu@0/l2-cache");
		if (node_offset > 0) {
			debug("SMBIOS: /cpus/cpu@0/l2-cache node found in FDT\n");

			// Get L2 Unified Cache size
			reg = fdt_getprop(fdt, node_offset, "cache-size", NULL);
			if (reg) {
				// Node provides size in bytes for given core. Increase total size in KB:
				cache_size[2] = fdt32_to_cpu(*reg) / 1024;
				debug("SMBIOS: core L2 unified cache size     = %d KB\n", cache_size[2]);
			}
		}
	}

	// L1I + L1D + L2 Unified -> Max 3 nodes to be updated
	for (cache_index = 0; cache_index < 3; cache_index++) {
		if (cache_size[cache_index]) { // Update only when got valid value
			sprintf(node_name, "/uboot-smbios/type7@%d", cache_index);
			debug("SMBIOS: finding %s node in FDT\n", node_name);
			// Internal L1I Cache
			node_offset = fdt_path_offset(fdt, node_name);
			if (node_offset < 0)
				continue;

			fdt_setprop_inplace_u32(fdt, node_offset, "maxsize", cache_size[cache_index]);
			fdt_setprop_inplace_u32(fdt, node_offset, "installed-size", cache_size[cache_index]);
			debug("SMBIOS: updated cache maxsize and installed-size to %d KB\n", cache_size[cache_index]);
		}
	}
}

static void smbios_update_type19(void)
{
	u32 i;
	char tmp_str[40];
	int node_offset;
	struct fdt_resource res;
	void *fdt = (void *)gd->fdt_blob;

	debug("SMBIOS: %s() entry\n", __func__);

	i = 0;
	do {
		sprintf(tmp_str, "/memory@%d", i);
		node_offset = fdt_path_offset(fdt, tmp_str);
		if (node_offset < 0)
			continue;

		debug("SMBIOS: found %s node in FDT\n", tmp_str);

		if (fdt_get_resource(fdt, node_offset, "reg", 0, &res)) {
			i++;
			continue;
		}

		debug("SMBIOS: res.start = 0x%lx, res.end = 0x%lx\n", (unsigned long) res.start, (unsigned long) res.end);

		sprintf(tmp_str, "/uboot-smbios/type19@%d", i);
		node_offset = fdt_path_offset(fdt, tmp_str);
		if (node_offset < 0)
			continue;

		debug("SMBIOS: found %s node in FDT\n", tmp_str);

		if (res.start < 0x100000000 && res.end < 0x100000000) {
			fdt_setprop_inplace_u32(fdt, node_offset, "start-addr", res.start);
			fdt_setprop_inplace_u32(fdt, node_offset, "end-addr", res.end);
			debug("SMBIOS: updated start-addr to 0x%lx and end-addr to 0x%lx\n", (unsigned long) res.start, (unsigned long) res.end);
		} else {
			fdt_setprop_inplace_u64(fdt, node_offset, "ext-start-addrs", res.start);
			fdt_setprop_inplace_u64(fdt, node_offset, "ext-end-addrs", res.end);
			debug("SMBIOS: updated ext-start-addr to 0x%lx and ext-end-addr to 0x%lx\n", (unsigned long) res.start, (unsigned long) res.end);
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
	u8 module_type;
	bool skip_module_supplier_data = false;

	debug("SMBIOS: %s() entry\n", __func__);

	module_type = mv_ddr_spd_dev_type_get();

	/*	NOTE: DDR5 SPD data shall basically have at least 640 bytes.
	 *	However when Virtual SPD data are defined in FDT for DDR5, they might have just 512 bytes.
	 *	Considering above, code allows 512 bytes of DDR5 SPD data,
	 *	but in this special case prevents from gathering Module Supplier’s Data
	 *	from bytes 513-640 (e.g. serial number).

	 *	Similarly with DDR4: 320 SPD bytes are enough, but Module Supplier’s Data are gathered
	 *	only when SPD data length is at least 384 bytes.
	 *	No prevention causes exception as smbios_ddr_info.c code does not take SPD length into consideration.
	 */
	if (module_type != SPD_DRAM_TYPE_DDR4_SDRAM && module_type != SPD_DRAM_TYPE_DDR5_SDRAM) {
		debug("SMBIOS: unsupported memory type (%x)!\n", module_type);
		return;
	} else if (module_type == SPD_DRAM_TYPE_DDR5_SDRAM && spd_cache_len[dimm] < 512) {
		debug("SMBIOS: error -> DDR5 SPD data length (%d) < %d bytes!\n", spd_cache_len[dimm], 512);
		return;
	} else if (module_type == SPD_DRAM_TYPE_DDR4_SDRAM && spd_cache_len[dimm] < 320) {
		debug("SMBIOS: error -> DDR4 SPD data length (%d) < %d bytes!\n", spd_cache_len[dimm], 320);
		return;
	}

	if ((module_type == SPD_DRAM_TYPE_DDR5_SDRAM && spd_cache_len[dimm] < 640) ||
		(module_type == SPD_DRAM_TYPE_DDR4_SDRAM && spd_cache_len[dimm] < 384)) {
		// DDR4: Module Supplier’s Data: Bytes 320~383
		// DDR5: Module Supplier’s Data: Bytes 512~639
		debug("SMBIOS: skip checking Module Supplier’s Data as SPD data length = %d for memory type = 0x%X\n", spd_cache_len[dimm], module_type);
		skip_module_supplier_data = true;
	}

	sprintf(tmp_str, "/uboot-smbios/type17@%d", dimm);
	node_offset = fdt_path_offset(fdt, tmp_str);
	if (node_offset < 0)
		return;

	debug("SMBIOS: found %s node in FDT\n", tmp_str);

	tmp_val32 = mv_ddr_spd_die_capacity_get();
	debug("SMBIOS: size of memory device = %u (MB)\n", tmp_val32);
	if (tmp_val32 < 0x7FFF)
		fdt_setprop_inplace_u32(fdt, node_offset, "size", tmp_val32);
	else
		fdt_setprop_inplace_u32(fdt, node_offset, "size", 0x7FFF); // If the size is (32 GB-1 MB) or greater, the field value is 7FFFh

	fdt_setprop_inplace_u32(fdt, node_offset, "ext-size", tmp_val32); // Actual size is stored in the Extended Size field.
	fdt_setprop_inplace_u64(fdt, node_offset, "vol-sizes", tmp_val32 * 1024 * 1024); // Size of the Volatile portion of the memory device in bytes
	fdt_setprop_inplace_u64(fdt, node_offset, "log-sizes", tmp_val32 * 1024 * 1024); // Size of the Logical memory device in Bytes

	tmp_val32 = bus_total_width();
	fdt_setprop_inplace_u32(fdt, node_offset, "total-width", tmp_val32);
	debug("SMBIOS: updated total-width to %d bits\n", tmp_val32);

	tmp_val32 = bus_data_width();
	fdt_setprop_inplace_u32(fdt, node_offset, "data-width", tmp_val32);
	debug("SMBIOS: updated data-width  to %d bits\n", tmp_val32);

	tmp_val32 = spd_module_type_to_dtmf_type();
	fdt_setprop_inplace_u32(fdt, node_offset, "form-factor", tmp_val32);
	debug("SMBIOS: updated form-factor to 0x%X\n", tmp_val32);

	tmp_val32 = spd_dramdev_type_to_dtmf_type();
	fdt_setprop_inplace_u32(fdt, node_offset, "mem-type", tmp_val32);
	debug("SMBIOS: updated mem-type to 0x%X\n", tmp_val32);

	tmp_val32 = get_dram_speed(); // DDR speed in MT/s.
	fdt_setprop_inplace_u32(fdt, node_offset, "speed", tmp_val32);
	debug("SMBIOS: updated speed to %u (MT/s)\n", tmp_val32);

	// Gather currently configured DDR speed out of FDT table:
	{
		const char *speed_str = fdt_getprop(fdt, cavium_bdk, "DDR-SPEED", NULL);

		if (speed_str) {
			tmp_val32 = (u32) simple_strtoul(speed_str, NULL, 0);
			fdt_setprop_inplace_u32(fdt, node_offset, "configured-memory-speed", tmp_val32);
			debug("SMBIOS: updated configured-memory-speed to %u (MT/s)\n", tmp_val32);
		}
	}

	tmp_val32 = mv_ddr_spd_pkg_rank_get(); // Bits 3-0: rank
	fdt_setprop_inplace_u32(fdt, node_offset, "attributes", tmp_val32);
	debug("SMBIOS: updated attributes (rank) to %d\n", tmp_val32);

	tmp_val32 = get_dram_min_volt();
	fdt_setprop_inplace_u32(fdt, node_offset, "minimum_voltage", tmp_val32);
	debug("SMBIOS: updated minimum_voltage to %u mV\n", tmp_val32);

	tmp_val32 = get_dram_max_volt();
	fdt_setprop_inplace_u32(fdt, node_offset, "maximum_voltage", tmp_val32);
	debug("SMBIOS: updated maximum_voltage to %u mV\n", tmp_val32);

	tmp_val32 = get_dram_configured_volt();
	fdt_setprop_inplace_u32(fdt, node_offset, "configured_voltage", tmp_val32);
	debug("SMBIOS: updated configured_voltage to %u mV\n", tmp_val32);

	// Gather Module Supplier’s Data only when corresponding SPD data are available:
	if (!skip_module_supplier_data) {
		tmp_val32 = get_dram_serial();
		sprintf(tmp_str, "%X", tmp_val32);
		fdt_setprop_string(fdt, node_offset, "serial-number", tmp_str);
		debug("SMBIOS: updated serial-number to %s\n", tmp_str);

		tmp_val32 = get_module_manufacturer_id();
		fdt_setprop_inplace_u32(fdt, node_offset, "module-manufacturer-id", tmp_val32);
		debug("SMBIOS: updated module-manufacturer-id to 0x%x\n", tmp_val32);

		// Updating "manufacturer" string node would require transalting table of
		// module-manufacturer-id into corresponding string. Thus skipped "manufacturer" update.

		get_dram_module_part_no(tmp_str);
		fdt_setprop_string(fdt, node_offset, "part-number", tmp_str);
		debug("SMBIOS: updated part-number to %s\n", tmp_str);

		tmp_val32 = get_product_id();
		fdt_setprop_inplace_u32(fdt, node_offset, "module-product-id", tmp_val32);
		debug("SMBIOS: updated module-product-id to 0x%x\n", tmp_val32);
	}
}

void smbios_update_type17(void)
{
	u8 i;

	debug("SMBIOS: %s() entry\n", __func__);

	for (i = 0; i < MAX_DSS; i++) {
		if (!dimm_present[i] || !spd_cache[i])
			continue;

		debug("SMBIOS: getting SPD details for DIMM%d\n", i);
		get_dram_info_init((u8 *)spd_cache[i]);
		smbios_add_ddr_info(i);
	}
}

void smbios_update_type4(void)
{
	void *fdt = (void *)gd->fdt_blob;
	int node_offset;
	int t4_node_offset;
	u32 val = 0;
	const char *speed;
	char tmp_str[40];
	u8  core_no = 0;
	char node_name[32] = "";

	t4_node_offset = fdt_path_offset(fdt, "/uboot-smbios/type4@0");
	if (t4_node_offset < 0)
		return;

	debug("SMBIOS: type 4 node found in FDT\n");

	val = 0; // To be filled by O.E.M if needed.
	fdt_setprop_inplace_u32(fdt, t4_node_offset, "processor-id", val);
	debug("SMBIOS: updated processor-id to 0x%X\n", val);

	node_offset = fdt_path_offset(fdt, "/soc@0");
	if (node_offset > 0) {
		const char *rev_str = fdt_getprop(fdt, node_offset, "chiprevision", NULL);

		debug("SMBIOS: /soc@0 node found in FDT\n");

		if (rev_str) {
			fdt_setprop_string(fdt, t4_node_offset, "processor-version", rev_str);
			debug("SMBIOS: updated processor-version to %s\n", rev_str);
		}
	}

	sprintf(tmp_str, "Not Specified");	// Might be adjusted by OEM
	fdt_setprop_string(fdt, t4_node_offset, "serial-number", tmp_str);
	debug("SMBIOS: updated serial-number to %s\n", tmp_str);

	sprintf(tmp_str, "Not Specified");	// Might be adjusted by OEM
	fdt_setprop_string(fdt, t4_node_offset, "asset-tag", tmp_str);
	debug("SMBIOS: updated asset-tag to %s\n", tmp_str);

	switch (read_partnum()) {
	case CN10KA:
		sprintf(tmp_str, "CN10KA");
		break;
	case CNF10KA:
		sprintf(tmp_str, "CNF10KA");
		break;
	case CNF10KB:
		sprintf(tmp_str, "CNF10KB");
		break;
	default:
		sprintf(tmp_str, "Unknown");
		break;
	}

	fdt_setprop_string(fdt, t4_node_offset, "part-number", tmp_str);
	debug("SMBIOS: updated part-number to %s\n", tmp_str);

	speed = fdt_getprop(fdt, cavium_bdk, "CORECLOCK-FREQ-MAX", NULL);
	if (speed) {
		val = (u32) simple_strtoul(speed, NULL, 0);
		fdt_setprop_inplace_u32(fdt, t4_node_offset, "curspeed", val);
		debug("SMBIOS: updated curspeed to %u MHz\n", val);
	}

	// Get number of cores
	do {
		sprintf(node_name, "/cpus/cpu@%X", core_no*0x10000); // cpu@0, cpu@10000, cpu@20000, etc.
		node_offset = fdt_path_offset(fdt, node_name);
		if (node_offset < 0)
			continue;

		debug("SMBIOS: %s node found in FDT\n", node_name);
		core_no++;
	} while (node_offset > 0);

	// Core number
	fdt_setprop_inplace_u32(fdt, t4_node_offset, "core-count", core_no);
	debug("SMBIOS: updated core-count   to %u\n", core_no);

	// Assume all cores are enabled
	fdt_setprop_inplace_u32(fdt, t4_node_offset, "core-enabled", core_no);
	debug("SMBIOS: updated core-enabled to %u\n", core_no);

	// Single threaded ARM Neoverse N2 core -> thread-count = core_no
	fdt_setprop_inplace_u32(fdt, t4_node_offset, "thread-count", core_no);
	debug("SMBIOS: updated thread-count to %u\n", core_no);
}

u64 fdt_get_smbios_info(void)
{
	int node, ret, i;
	char tmp_str[80];
	const void *fdt = gd->fdt_blob;
	u8 num_sckt;
	u32 dmc_mask = (u32)(~((u32) 0)); //Assume all memory controllers are enabled.
	const char *str = NULL;

	debug("SMBIOS: %s() entry\n", __func__);

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

	if (!num_sckt)
		return num_sckt;

	debug("SMBIOS: system has %d %s\n", num_sckt, (num_sckt != 1) ? "sockets" : "socket");

	/* Determine location of SPD cache */
	cavium_bdk = fdt_path_offset(fdt, "/cavium,bdk");
	if (cavium_bdk < 0) {
		printf("%s: /cavium,bdk is missing from device tree: %s\n",
		       __func__, fdt_strerror(cavium_bdk));
		return 0;
	}
	debug("SMBIOS: found /cavium,bdk node in FDT\n");

	memset(spd_cache, 0, sizeof(spd_cache));

	str = fdt_getprop(fdt, cavium_bdk, "DDR-DMC-MASK", (int *) &spd_cache_len[i]);
	if (str) {
		dmc_mask = simple_strtoul(str, NULL, 0);
	}
	debug("SMBIOS: DMC Mask = 0x%x\n", dmc_mask);

	for (i = 0; i < MAX_DSS; i++) {
		// According to EBF code it is enough to disable only even numbered controllers, to have whole DSS "disabled".
		if (!(dmc_mask & (1 << (2*i)))) {
			debug("SMBIOS: DDS%d disabled by DMC Mask.\n", i);
			continue;
		}

		sprintf(tmp_str, "DDR-SPD-DATA.DSS%d", i);
		spd_cache[i] = fdt_getprop(fdt, cavium_bdk, tmp_str, (int *) &spd_cache_len[i]);
		debug("SMBIOS: spd_cache[%d] = %p, %s\n", i, spd_cache[i], tmp_str);

		if (spd_cache[i])
			dimm_present[i] = 1;
		else {
			// Couldn't locate DDR-SPD-DATA.DSSx node,
			// Either due to DIMM absence in slot or due to soldered RAM existence.
			// In the latter case, there might be virtual SPD data available. Let's check it.
			sprintf(tmp_str, "DDR-SPD-DATA"); //Always the same regardless of DSS
			spd_cache[i] = fdt_getprop(fdt, cavium_bdk, tmp_str, (int *) &spd_cache_len[i]);

			if (spd_cache[i])
				dimm_present[i] = 1;
		}
		debug("SMBIOS: DIMM slot %d %s. SPD data length = %u\n", i, dimm_present[i] ? "populated" : "empty", spd_cache_len[i]);
	}

	smbios_update_type17(); // Memory Device (Type 17)
	smbios_update_type7();	// Cache Information (Type 7)
	smbios_update_type19();	// Memory Array Mapped Address (Type 19)
	smbios_update_type4();	// Processor Information (Type 17)

	return 0;
}
#endif //CONFIG_IS_ENABLED(GENERATE_SMBIOS_TABLE)

u64 fdt_get_board_mac_addr(bool use_id, u8 id)
{
	int node, len = 16;
	const char *str = NULL;
	const void *fdt = gd->fdt_blob;
	u64 mac_addr = 0;
	char name[32];

	node = fdt_get_bdk_node();
	if (!node)
		return mac_addr;

	debug("fdt: %d %d\n", use_id, id);
	if (use_id)
		snprintf(name, sizeof(name), "BOARD-MAC-ADDRESS-ID%d", id);
	else
		snprintf(name, sizeof(name), "BOARD-MAC-ADDRESS");

	debug("fdt: %s\n", name);
	str = fdt_getprop(fdt, node, name, &len);
	if (str)
		mac_addr = simple_strtoul(str, NULL, 16);
	debug("fdt: %llx\n", mac_addr);
	return mac_addr;
}

int fdt_get_board_mac_cnt(bool *use_id)
{
	int node, len = 16;
	const char *str = NULL;
	const void *fdt = gd->fdt_blob;
	int mac_count = 0, mac_id_count = 0;

	node = fdt_get_bdk_node();
	if (!node)
		return mac_count;
	str = fdt_getprop(fdt, node, "BOARD-MAC-ADDRESS-NUM", &len);
	if (str) {
		mac_count = simple_strtol(str, NULL, 10);
		if (!mac_count)
			mac_count = simple_strtol(str, NULL, 16);
		debug("fdt: MAC_NUM %d\n", mac_count);
	} else {
		printf("Error: cannot retrieve mac count prop from fdt\n");
	}
	str = fdt_getprop(gd->fdt_blob, node, "BOARD-MAC-ADDRESS-NUM-OVERRIDE",
			  &len);
	if (str) {
		if (simple_strtol(str, NULL, 10) >= 0)
			mac_count = simple_strtol(str, NULL, 10);
		debug("fdt: MAC_NUM %d\n", mac_count);
	} else {
		printf("Error: cannot retrieve mac num override prop\n");
	}
	str = fdt_getprop(fdt, node, "BOARD-MAC-ADDRESS-ID-NUM", &len);
	if (str) {
		mac_id_count = simple_strtol(str, NULL, 10);
		if (!mac_id_count)
			mac_id_count = simple_strtol(str, NULL, 16);
		debug("fdt: MAC_ID_NUM %d\n", mac_id_count);
		if (mac_id_count)
			mac_count = mac_id_count;
	} else {
		printf("Error: cannot retrieve mac count prop from fdt\n");
	}
	*use_id = mac_id_count ? true : false;
	return mac_count;
}

const char *fdt_get_board_serial(void)
{
	const void *fdt = gd->fdt_blob;
	int node, len = 64;
	const char *str = NULL;

	node = fdt_get_bdk_node();
	if (!node)
		return NULL;

	str = fdt_getprop(fdt, node, "BOARD-SERIAL", &len);
	if (!str)
		printf("Error: cannot retrieve board serial from fdt\n");
	return str;
}

const char *fdt_get_board_revision(void)
{
	const void *fdt = gd->fdt_blob;
	int node, len = 64;
	const char *str = NULL;

	node = fdt_get_bdk_node();
	if (!node)
		return NULL;

	str = fdt_getprop(fdt, node, "BOARD-REVISION", &len);
	if (!str)
		printf("Error: cannot retrieve board revision from fdt\n");
	return str;
}

const char *fdt_get_board_model(void)
{
	int node, len = 16;
	const char *str = NULL;
	const void *fdt = gd->fdt_blob;

	node = fdt_get_bdk_node();
	if (!node)
		return NULL;
	str = fdt_getprop(fdt, node, "BOARD-MODEL", &len);
	if (!str)
		printf("Error: cannot retrieve board model from fdt\n");
	return str;
}

/*
 * Get config option SWITCH-MICROINIT value if defined,
 * otherwise default to 1.
 */
int fdt_get_switch_config(void)
{
	int node, config_switch = 1;
	const char *str = NULL;
	const void *fdt = gd->fdt_blob;

	node = fdt_get_bdk_node();
	if (!node)
		return config_switch;

	str = fdt_getprop(fdt, node, "SWITCH-MICROINIT", NULL);
	if (str)
		config_switch = (str[0] - '0');

	return config_switch;
}

int arch_fixup_memory_node(void *blob)
{
	return 0;
}

/**
 * Remove all properties other than the ones we want and rename the node
 */
static int ft_board_clean_props(void *blob, int node)
{
	static const char * const
		octeontx_brd_nodes[] = {"BOARD-MODEL",
					"BOARD-SERIAL",
					"BOARD-REVISION",
					"BOARD-MAC-ADDRESS",
					"BOARD-MAC-ADDRESS-NUM",
					"BOARD-MAC-ADDRESS-ID-NUM",
					"BOARD-MAC-ADDRESS-ID0",
					"BOARD-MAC-ADDRESS-ID1",
					"BOARD-MAC-ADDRESS-ID2",
					"BOARD-MAC-ADDRESS-ID3",
					"BOARD-MAC-ADDRESS-ID4",
					"BOARD-MAC-ADDRESS-ID5",
					"BOARD-MAC-ADDRESS-ID6",
					"BOARD-MAC-ADDRESS-ID7",
					"BOARD-MAC-ADDRESS-ID8",
					"BOARD-MAC-ADDRESS-ID9",
					"BOARD-MAC-ADDRESS-ID10",
					"BOARD-MAC-ADDRESS-ID11",
					"BOARD-MAC-ADDRESS-ID12",
					"BOARD-MAC-ADDRESS-ID13",
					"BOARD-MAC-ADDRESS-ID14",
					"BOARD-MAC-ADDRESS-ID15",
					"BOARD-MAC-ADDRESS-ID16",
					"BOARD-MAC-ADDRESS-ID17",
					"BOARD-MAC-ADDRESS-ID18",
					"BOARD-MAC-ADDRESS-ID19",
					"BOARD-MAC-ADDRESS-ID20",
					"BOARD-MAC-ADDRESS-ID21",
					"BOARD-MAC-ADDRESS-ID22",
					"BOARD-MAC-ADDRESS-ID23",
					"BOARD-MAC-ADDRESS-ID24",
					"BOARD-MAC-ADDRESS-ID25",
					"BOARD-MAC-ADDRESS-ID26",
					"BOARD-MAC-ADDRESS-ID27",
					"BOARD-MAC-ADDRESS-ID28",
					"BOARD-MAC-ADDRESS-ID29",
					"BOARD-MAC-ADDRESS-ID30",
					"BOARD-MAC-ADDRESS-ID31",
					"RESET-COUNT-WARM",
					"RESET-COUNT-COLD",
					"RESET-COUNT-CORE-WDOG",
					"RESET-COUNT-SCP-WDOG",
					"RESET-COUNT-MCP-WDOG",
					"RESET-COUNT-ECP-WDOG"
					};
	int offset;
	const char *name;
	const char *data;
	int len;
	int i;
	int ret;
	bool found;
	int off_idx = 0;
	int prop_offsets[1000];

	fdt_for_each_property_offset(offset, blob, node) {
		if (off_idx == ARRAY_SIZE(prop_offsets)) {
			printf("Too many properties!\n");
			return -1;
		}
		prop_offsets[off_idx++] = offset;
	}

	while (--off_idx >= 0) {
		offset = prop_offsets[off_idx];
		data = fdt_getprop_by_offset(blob, offset, &name, &len);
		if (!data || !name) {
			printf("Error: could not obtain property data or name at offset 0x%x\n",
			       offset);
			return -1;
		}
		debug("Found property %s at offset 0x%x\n", name, offset);

		/* Delete all properties that are not in our list */
		found = false;
		for (i = 0; i < ARRAY_SIZE(octeontx_brd_nodes); i++) {
			if (!strcmp(name, octeontx_brd_nodes[i])) {
				found = true;
				break;
			}
		}
		if (!found) {
			debug("Deleting cavium,bdk/%s\n", name);
			ret = fdt_delprop(blob, node, name);
			if (ret) {
				printf("Error: could not delete property %s: %s\n",
				       name, fdt_strerror(ret));
				return -1;
			}
		} else {
			debug("Keeping cavium,bdk/%s\n", name);
		}
	}

	ret = fdt_set_name(blob, node, "octeontx_brd");
	if (ret) {
		printf("Error: could not rename cavium,bdk to octeontx_brd: %s\n",
		       fdt_strerror(ret));
		return -1;
	}
	debug("Changed name to octeontx_brd, last offset: 0x%x\n", offset);
	return 0;
}

int ft_board_setup(void *blob, struct bd_info *bd)
{
	int nodeoff, ret;

	if (!blob) {
		printf("ERROR: NULL FDT pointer\n");
		return -1;
	}
	debug("%s: FDT pointer: %p\n", __func__, blob);
	ret = fdt_check_header(blob);
	if (ret < 0) {
		printf("ERROR: %s\n", fdt_strerror(ret));
		return ret;
	}

	nodeoff = fdt_path_offset(blob, "/cavium,bdk");
	if (nodeoff < 0) {
		/*
		 * It is possible that the FDT has already been processed if
		 * the fdt boardsetup command is entered.
		 */
		if (fdt_path_offset(blob, "/octeontx_brd") >= 0)
			return 0;

		printf("ERROR: FDT BDK node not found\n");
		return nodeoff;
	}
	debug("/cavium,bdk node: 0x%x\n", nodeoff);
	ret = ft_board_clean_props(blob, nodeoff);
	return ret;
}
