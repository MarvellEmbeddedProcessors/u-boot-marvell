// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <errno.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <log.h>

#include <linux/compiler.h>
#include <linux/libfdt.h>

#include <asm/arch/board.h>
#include <asm/arch/smc.h>
#include <asm/global_data.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

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

#define MAX_SCKT	1
#define MAX_CTRL	3
#define MAX_DIMM	2

static u8 num_sckt;
static u8 dimm_present[MAX_SCKT][MAX_CTRL][MAX_DIMM];
static const u8 *spd_cache[MAX_SCKT][MAX_CTRL][MAX_DIMM];

static void smbios_update_type7(void)
{
	u32 val;
	int node_offset;
	void *fdt = (void *)gd->fdt_blob;
	const u32 *reg;

	node_offset = fdt_path_offset(fdt, "/l3-cache");
	if (node_offset < 0)
		return;

	reg = fdt_getprop(fdt, node_offset, "cache-size", NULL);
	if (!reg)
		return;
	val = fdt32_to_cpu(*reg);

	node_offset = fdt_path_offset(fdt, "/uboot-smbios/type7@3");
	if (node_offset < 0)
		return;

	fdt_setprop_inplace_u32(fdt, node_offset, "maxsize", val);
	fdt_setprop_inplace_u32(fdt, node_offset, "installed-size", val);
}

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
}

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

u64 fdt_get_smbios_info(void)
{
	int node, ret, len, i, j, k;
	char tmp_str[80];
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

	/* Determine location of SPD cache */
	node = fdt_path_offset(fdt, "/cavium,bdk");
	if (node < 0) {
		printf("%s: /cavium,bdk is missing from device tree: %s\n",
		       __func__, fdt_strerror(node));
		return 0;
	}

	memset(spd_cache, 0, sizeof(spd_cache));

	len = 512;
	for (i = 0; i < MAX_SCKT; i++) {
		for (j = 0; j < MAX_CTRL; j++) {
			for (k = 0; k < MAX_DIMM; k++) {
				sprintf(tmp_str,
					"DDR-CONFIG-SPD-DATA.DIMM%d.LMC%d.N%d", k, j, i);
				spd_cache[i][j][k] = fdt_getprop(fdt, node, tmp_str, &len);
				if (spd_cache[i][j][k])
					dimm_present[i][j][k] = 1;
				else
					dimm_present[i][j][k] = 0;
			}
		}
	}

	smbios_update_type7();
	smbios_update_type17();
	smbios_update_type19();

	return 0;
}
#endif //CONFIG_IS_ENABLED(GENERATE_SMBIOS_TABLE)

u64 fdt_get_board_mac_addr(void)
{
	int node, len = 16;
	const char *str = NULL;
	const void *fdt = gd->fdt_blob;
	u64 mac_addr = 0;

	node = fdt_get_bdk_node();
	if (!node)
		return mac_addr;
	str = fdt_getprop(fdt, node, "BOARD-MAC-ADDRESS", &len);
	if (str)
		mac_addr = simple_strtol(str, NULL, 16);
	return mac_addr;
}

int fdt_get_board_mac_cnt(void)
{
	int node, len = 16;
	const char *str = NULL;
	const void *fdt = gd->fdt_blob;
	int mac_count = 0;

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

int arch_fixup_memory_node(void *blob)
{
	return 0;
}

int ft_board_setup(void *blob, struct bd_info *bd)
{
	int nodeoff, node, ret, i;
	const char *temp;

	static const char * const
		octeontx_brd_nodes[] = {"BOARD-MODEL",
					"BOARD-SERIAL",
					"BOARD-MAC-ADDRESS",
					"BOARD-REVISION",
					"BOARD-MAC-ADDRESS-NUM"
					};
	char nodes[ARRAY_SIZE(octeontx_brd_nodes)][32];

	ret = fdt_check_header(blob);
	if (ret < 0) {
		printf("ERROR: %s\n", fdt_strerror(ret));
		return ret;
	}

	if (blob) {
		nodeoff = fdt_path_offset(blob, "/cavium,bdk");
		if (nodeoff < 0) {
			printf("ERROR: FDT BDK node not found\n");
			return nodeoff;
		}

		/* Read properties in temporary variables */
		for (i = 0; i < ARRAY_SIZE(octeontx_brd_nodes); i++) {
			temp = fdt_getprop(blob, nodeoff,
					   octeontx_brd_nodes[i], NULL);
			strncpy(nodes[i], temp, sizeof(nodes[i]));
		}

		/* Delete cavium,bdk node */
		ret = fdt_del_node(blob, nodeoff);
		if (ret < 0) {
			printf("WARNING : could not remove cavium, bdk node\n");
			return ret;
		}
		debug("%s deleted 'cavium,bdk' node\n", __func__);
		/*
		 * Add a new node at root level which would have
		 * necessary info
		 */
		node = fdt_add_subnode(blob, 0, "octeontx_brd");
		if (node < 0) {
			printf("Cannot create node octeontx_brd: %s\n",
			       fdt_strerror(node));
			return -EIO;
		}

		/* Populate properties in node */
		for (i = 0; i < ARRAY_SIZE(octeontx_brd_nodes); i++) {
			if (fdt_setprop_string(blob, node,
					       octeontx_brd_nodes[i],
					       nodes[i])) {
				printf("Can't set %s\n", nodes[i]);
				return -EIO;
			}
		}
	}

	return 0;
}

/**
 * Return the FDT base address that was passed by ATF
 *
 * @return	FDT base address received from ATF in x1 register
 */
void *board_fdt_blob_setup(void)
{
	return (void *)fdt_base_addr;
}
