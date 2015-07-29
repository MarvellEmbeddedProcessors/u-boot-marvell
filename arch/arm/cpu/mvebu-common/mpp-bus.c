/*
 * ***************************************************************************
 * Copyright (C) 2015 Marvell International Ltd.
 * ***************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

struct mpp_bus __attribute__((section(".data"))) buses[MAX_MPP_BUSES];

#define DEBUG
int mpp_get_bus_id(const char *name)
{
	int id = -1, i;
	for (i = 0; i < MAX_MPP_BUSES; i++) {
		if (strcmp(name, buses[i].name) == 0) {
			id = i;
			break;
		}
	}

	if (buses[id].valid == 0)
		return -1;

	return id;
}

struct mpp_bus *mpp_get_bus(int id)
{
	if (buses[id].valid == 0)
		return NULL;

	return &buses[id];
}

u32 mpp_get_pin_func(int bank_id, u32 pin_id)
{
	return pinctl_get_pin_func(bank_id, pin_id);
}

int mpp_set_pin_func(int bank_id, u32 pin_id, u32 func)
{
	return pinctl_set_pin_func(bank_id, pin_id, func);
}

const char *mpp_get_bank_name(int bank_id)
{
	return pinctl_get_name(bank_id);
}

int mpp_get_bank_pins(int bank_id)
{
	return pinctl_get_pin_cnt(bank_id);
}

int mpp_is_bus_enabled(const char *name)
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

int mpp_enable_bus(const char *name)
{
	int i, id, ret;
	struct mpp_pin *pin;
	struct mpp_bus *bus;


	/* Find bus by name */
	id = mpp_get_bus_id(name);
	if (id < 0) {
		error("mpp bus %s not found\n", name);
		return -ENODEV;
	}
	bus = &buses[id];

	for (i = 0; i < bus->pin_cnt; i++) {
		pin = &bus->pins[i];
		ret = pinctl_set_pin_func(bus->bank_id, pin->id, pin->func);
		if (ret)
			printf("Warning: pin %d not set for bus %s\n", pin->id, bus->name);
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
		bus->valid = 1;
	}

	return 0;
}
