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
	int nodeoff, node, bdk_node, ret;
	const char *board_model, *board_rev;
	const char *board_mac_addr, *board_serial;
	char temp_brd_mdl[32], temp_brd_mac[32];
	char temp_brd_srl[32], temp_brd_rev[32];

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

		/* Collect all relevant info for the board that is needed */
		board_model = fdt_getprop(blob, nodeoff, "BOARD-MODEL", NULL);
		if (!board_model) {
			printf("Can't find BOARD-MODEL\n");
			return -ENOENT;
		}

		board_serial = fdt_getprop(blob, nodeoff, "BOARD-SERIAL", NULL);
		if (!board_serial) {
			printf("Can't find BOARD-SERIAL\n");
			return -ENOENT;
		}

		board_mac_addr = fdt_getprop(blob, nodeoff,
					     "BOARD-MAC-ADDRESS", NULL);
		if (!board_mac_addr) {
			printf("Can't find BOARD-MAC-ADDRESS\n");
			return -ENOENT;
		}

		board_rev = fdt_getprop(blob, nodeoff, "BOARD-REVISION", NULL);
		if (!board_rev) {
			printf("Can't find BOARD-REVISION\n");
			return -ENOENT;
		}

		/* Hold in temprary storage */
		strncpy(temp_brd_mdl, board_model, sizeof(temp_brd_mdl));
		strncpy(temp_brd_srl, board_serial, sizeof(temp_brd_srl));
		strncpy(temp_brd_mac, board_mac_addr, sizeof(temp_brd_mac));
		strncpy(temp_brd_rev, board_rev, sizeof(temp_brd_rev));

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
		if (fdt_setprop_string(blob, node, "BOARD-SERIAL",
				       temp_brd_srl)) {
			puts("Can't set BOARD-SERIAL\n");
			return -EIO;
		}
		if (fdt_setprop_string(blob, node,
				       "BOARD-MAC-ADDRESS", temp_brd_mac)) {
			puts("Can't set BOARD-MAC-ADDRESS\n");
			return -EIO;
		}
		if (fdt_setprop_string(blob, node,
				       "BOARD-REVISION", temp_brd_rev)) {
			puts("Can't set BOARD-REVISION\n");
			return -EIO;
		}
		if (fdt_setprop_string(blob, node,
				       "BOARD-MODEL", temp_brd_mdl)) {
			puts("Cannot set BOARD-MODEL\n");
			return -EIO;
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
