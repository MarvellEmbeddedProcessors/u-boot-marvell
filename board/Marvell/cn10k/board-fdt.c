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
#include <log.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <linux/libfdt.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <asm/arch/smc.h>
#include <asm/arch/board.h>

DECLARE_GLOBAL_DATA_PTR;

extern unsigned long fdt_base_addr;

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
		mac_addr = simple_strtol(str, NULL, 16);
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
					"BOARD-MAC-ADDRESS",
					"BOARD-REVISION",
					"BOARD-MAC-ADDRESS-NUM"
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

/**
 * Return the FDT base address that was passed by ATF
 *
 * @return	FDT base address received from ATF in x1 register
 */
void *board_fdt_blob_setup(void)
{
	return (void *)fdt_base_addr;
}