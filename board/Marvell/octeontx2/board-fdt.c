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

u64 fdt_get_preserved_mem_size(unsigned int node)
{
	const void *fdt = gd->fdt_blob;
	int bdk_node, len = 64;
	const char *str = NULL;
	char prop_name[34] = {};
	u64 preserved_mem_size = 0;

	bdk_node = fdt_get_bdk_node();
	if (!bdk_node)
		return 0;

	snprintf(prop_name, 34, "DDR-CONFIG-PRESERVE-NON-SECURE.N%d", node);
	str = fdt_getprop(fdt, bdk_node, prop_name, &len);
	if (str)
		preserved_mem_size += (strtoul(str, NULL, 16) << 20);

	snprintf(prop_name, 34, "DDR-CONFIG-PRESERVE-SECURE.N%d", node);
	str = fdt_getprop(fdt, bdk_node, prop_name, &len);
	if (str)
		preserved_mem_size += (strtoul(str, NULL, 16) << 20);

	if (preserved_mem_size)
		printf("Total memory preserved region: 0x%llx bytes\n", preserved_mem_size);
	return preserved_mem_size;
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
