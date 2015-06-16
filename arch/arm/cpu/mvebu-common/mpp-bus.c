/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt  on the worldwide
 * web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <config.h>
#include <common.h>
#include <asm/system.h>
#include <fdtdec.h>
#include <libfdt.h>
#include <errno.h>
#include <asm/io.h>
#include <malloc.h>
#include <asm/arch-mvebu/mpp.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/pinctl.h>

#define MAX_MPP_BUSES	10

struct mpp_pin {
	u32 id;
	u32 func;
};

struct mpp_bus {
	const char *name;
	int pin_cnt;
	int bank_id;
	struct mpp_pin *pins;
};

struct mpp_bus buses[MAX_MPP_BUSES];

#define DEBUG
int mpp_get_bus_id(char *name)
{
	int id = -1, i;
	for (i = 0; i < MAX_MPP_BUSES; i++) {
		if (strcmp(name, buses[i].name) == 0) {
			id = i;
			break;
		}
	}
	return id;
}

int mpp_is_bus_enabled(char *name)
{
	int id;
	int pin_id;
	struct mpp_pin *pin;
	struct mpp_bus *bus;
	int bus_active = 1;

	id = mpp_get_bus_id(name);
	if (id < 0) {
		error("mpp bus %s not found\n", name);
		return -ENODEV;
	}
	bus = &buses[id];

	for (pin_id = 0; pin_id < bus->pin_cnt; pin_id++) {
		pin = &bus->pins[pin_id];
		if (pinctl_get_pin_func(bus->bank_id, pin->id) != pin->func) {
			bus_active = 0;
			break;
		}
	}

	return bus_active;
}

int mpp_enable_bus(char *name)
{
	int i, id;
	struct mpp_pin *pin;
	struct mpp_bus *bus;


	/* Find bus by name */
	id = mpp_get_bus_id(name);
	if (id < 0) {
		error("mpp bus %s not found\n", name);
		return -ENODEV;
	}
	bus = &buses[id];

	printf("Enabling MPP bus %s\n", name);
	/* Check if someone already modified one of the pins */
	for (i = 0; i < bus->pin_cnt; i++) {
		pin = &bus->pins[i];

		printf("Setting (bank, pin, func) = (%d, %d, %d)\n", bus->bank_id, pin->id, pin->func);
		pinctl_set_pin_func(bus->bank_id, pin->id, pin->func);
	}

	return 0;
}

int mpp_bus_probe(void)
{
	int node_list[MAX_MPP_BUSES];
	int count, i, node, err;
	const void *blob = gd->fdt_blob;
	struct mpp_bus *bus;
	const char *bank_name;

	/* Initialize the mpp registers */
	count = fdtdec_find_aliases_for_id(blob, "mpp-bus",
			COMPAT_MVEBU_MPP_BUS, node_list, MAX_MPP_BUSES);

	if (count <= 0)
		return -ENODEV;

	if (count > MAX_MPP_BUSES) {
		error("mpp-bus: too many buses. using first %d\n", MAX_MPP_BUSES);
		count = MAX_MPP_BUSES;
	}

	for (i = 0; i < count ; i++) {
		node = node_list[i];

		if (node <= 0)
			continue;

		bus = &buses[i];
		bus->name = fdt_getprop(blob, node, "bus-name", NULL);
		if (bus->name == 0) {
			error("Failed reading mpp bus name\n");
			continue;
		}

		bus->pin_cnt = fdtdec_get_int(blob, node, "pin-count", 0);
		if (bus->pin_cnt == 0) {
			error("Failed reading pin count for bus %s\n", bus->name);
			continue;
		}
		bus->pins = malloc(bus->pin_cnt * sizeof(struct mpp_pin));
		if (bus->pins == 0) {
			error("Failed allocating memory bus pins %s\n", bus->name);
			continue;
		}

		err = fdtdec_get_int_array(blob, node, "pins", (u32 *)bus->pins, 2 * bus->pin_cnt);
		if (err) {
			error("Failed reading pins for bus %s\n", bus->name);
			continue;
		}

		bank_name = fdt_getprop(blob, node, "bank-name", NULL);
		bus->bank_id = pinctl_get_bank_id(bank_name);
		if (bus->bank_id < 0) {
			error("Failed getting bank id for bus %s\n", bus->name);
			continue;
		}
	}

	return 0;
}
