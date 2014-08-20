/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#define DEBUG

#include <config.h>
#include <common.h>
#include <asm/system.h>
#include <asm/bitops.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/tables.h>
#include <asm/arch-mvebu/soc.h>

const char *mvebu_unit_names[MAX_UNIT_ID + 1] = {
	[DRAM_UNIT_ID] =	"dram",
	[CPU_UNIT_ID] =		"cpu",
	[PCIE_UNIT_ID] =	"pcie",
	[ETH_GIG_UNIT_ID] =	"eth",
	[USB_UNIT_ID] =		"usb2",
	[USB3_UNIT_ID] =	"usb3",
	[IDMA_UNIT_ID] =	"idma",
	[IDMA_CHAN_UNIT_ID] =	"idma channel",
	[XOR_UNIT_ID] =		"xor",
	[XOR_CHAN_UNIT_ID] =	"xor channel",
	[SATA_UNIT_ID] =	"sata",
	[TDM_32CH_UNIT_ID] =	"tdm32",
	[TDM_PORTS_UNIT_ID] =	"tdm",
	[UART_UNIT_ID] =	"uart",
	[CESA_UNIT_ID] =	"cesa",
	[SPI_UNIT_ID] =		"spi",
	[SDIO_UNIT_ID] =	"sdio",
	[BM_UNIT_ID] =		"bm",
	[PNC_UNIT_ID] =		"pnc",
	[NAND_UNIT_ID] =	"nand",
	[SERDES_LANES_UNIT_ID] =	"serdes-lane",
	[MAX_UNIT_ID] =		"NA"
};

const char *unit_info_get_name(enum mvebu_unit_id id)
{
	if ((id < 0) || (id > MAX_UNIT_ID)) {
		printf("Error: %s: Illegal unit id %d\n", __func__, id);
		return NULL;
	}

	return mvebu_unit_names[id];
}

static u16 *unit_info_get_mask_ref(enum mvebu_unit_id id)
{
	u16 *unit_mask;

	if ((id < 0) || (id > MAX_UNIT_ID)) {
		printf("Error: %s: Illegal unit id %d\n", __func__, id);
		return NULL;
	}

	unit_mask = soc_get_unit_mask_table();
	unit_mask += id;

	return unit_mask;
}

u16 unit_info_get_mask(enum mvebu_unit_id id)
{
	return *unit_info_get_mask_ref(id);
}

void unit_info_disable_units(enum mvebu_unit_id id, u16 mask)
{
	u16 *unit_mask;

	unit_mask = unit_info_get_mask_ref(id);
	if (!unit_mask)
		return;

	disable_unit(unit_mask, mask);
}

void unit_info_disable_unit_instance(enum mvebu_unit_id id, int index)
{
	unit_info_disable_units(id, (1 << index));
}

bool unit_info_is_active(enum mvebu_unit_id id, int index)
{
	u16 *unit_mask;
	bool active;

	unit_mask = unit_info_get_mask_ref(id);
	if (!unit_mask)
		return -1;

	/* TODO - is this BE safe, should i use test_bit ??*/
	active = (bool)(((*unit_mask) >> index) & 0x1);
	return active;
}

int unit_info_get_count(enum mvebu_unit_id id)
{
	u16 *unit_mask;

	unit_mask = unit_info_get_mask_ref(id);
	if (!unit_mask)
		return -1;

	return hweight32(*unit_mask);
}

static void unit_mask_to_list(u16 mask, char *list)
{
	int i;
	char str[8];

	sprintf(list, " ");

	for (i = 0; i < 16; i++) {
		if ((mask >> i) & 0x1) {
			sprintf(str, "%d ", i);
			strcat(list, str);
		}
	}
}

void dump_unit_info(void)
{
	u16 *unit_mask;
	char active_list[64];
	int unit_id;

	unit_mask = soc_get_unit_mask_table();

	printf("name      active\n");
	printf("-----------------------\n");

	for (unit_id = 0; unit_id < MAX_UNIT_ID; unit_id++) {
		if (*unit_mask) {
			unit_mask_to_list(*unit_mask, active_list);
			printf("%-8s %s\n", unit_info_get_name(unit_id), active_list);
		}
		unit_mask++;
	}
}

void update_unit_info(u16 *unit_mask, u16 *new_mask,
		      enum unit_update_mode update_mode)
{
	int unit_id;

	debug("Updating unit info\n");

	for (unit_id = 0; unit_id < MAX_UNIT_ID; unit_id++) {
		if (update_mode == UNIT_INFO_OVERRIDE)
			override_unit(unit_mask, (*new_mask));
		else if (update_mode == UNIT_INFO_DISABLE)
			disable_unit(unit_mask, (*new_mask));
		else if (update_mode == UNIT_INFO_ENABLE)
			enable_unit(unit_mask, (*new_mask));

		unit_mask++;
		new_mask++;
	}
}
