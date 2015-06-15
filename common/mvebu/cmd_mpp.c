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
#include <command.h>
#include <vsprintf.h>
#include <errno.h>
#include "asm/arch-mvebu/mpp.h"

static int set_range(int *start, int *end, int max_pin)
{
	/* Print full range */
	if (*start < 0) {
		*start = 0;
		*end = max_pin;
	}
	/* Print single MPP */
	if ((*start >= 0) && (*end < 0))
		*end = *start;

	/* clip end to MAX_MPP_ID */
	*end = min(*end, max_pin);

	/* Error checking on range */
	if (*start > *end) {
		printf("Error: Start MPP (%d) higher then end MPP (%d)\n", *start, *end);
		return -EINVAL;
	}

	return 0;
}

static int cmd_mpp_list(void)
{
	int id, pins;
	const char *name;

	printf("\n id  name           pins\n");
	printf("------------------------\n");

	for (id = 0; id <= 10; id++) {
		name = mpp_get_bank_name(id);
		if (name == NULL)
			continue;

		pins = mpp_get_bank_pins(id);
		printf(" %d   %-15s %d\n", id, name, pins);
	}

	printf("\n");
	return 0;
}

static int cmd_mpp_read(int bank_id, int start, int end)
{
	int pin, ret;

	ret = set_range(&start, &end, mpp_get_bank_pins(bank_id) - 1);
	if (ret)
		return ret;

	printf("\n mpp bank: %s\n", mpp_get_bank_name(bank_id));

	printf("\n id  function\n");
	printf("--------------\n");

	for (pin = start; pin <= end; pin++)
		printf("  %-2d     %-2d\n", pin, mpp_get_pin_func(bank_id, pin));

	return 0;
}

static int cmd_mpp_write(int bank_id, int pin, int func)
{
	int ret;
	int max_pin_id;

	max_pin_id = mpp_get_bank_pins(bank_id);
	if (max_pin_id <= 0) {
		printf("Error: bad bank id %d\n", bank_id);
		return -EINVAL;
	}

	if ((pin < 0) || (pin > max_pin_id)) {
		printf("Error: pin %d out of range [%d, %d]\n", pin, 0, max_pin_id);
		return -EINVAL;
	}

	ret = mpp_set_pin_func(bank_id, pin, func);
	if (ret)
		return ret;

	/* Readback to verify */
	printf("Readback: %02d  0x%x\n", pin, mpp_get_pin_func(bank_id, pin));

	return 0;
}


int do_mpp_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	const char *cmd = argv[1];
	int func = 0;
	int start_pin = -1;
	int end_pin = -1;
	int bank_id = -1;


	if (argc < 2) {
		printf("Error: Please specify command type list|read|write\n");
		return 1;
	}

	if ((strcmp(cmd, "read") == 0) && (argc < 3)) {
		printf("Error: read requires a bank id\n");
		return 1;
	}

	if ((strcmp(cmd, "write") == 0) && (argc < 5)) {
		printf("Error: Please specify bank, pin, and function\n");
		return 1;
	}


	if (argc > 2)
		bank_id = (int)simple_strtoul(argv[2], NULL, 10);

	if (argc > 3)
		start_pin = (int)simple_strtoul(argv[3], NULL, 10);

	if (argc > 4) {
		func = (int)simple_strtoul(argv[4], NULL, 16);
		end_pin = (int)simple_strtoul(argv[4], NULL, 10);
	}

	if (strcmp(cmd, "list") == 0) {
		if (cmd_mpp_list())
			return -EINVAL;
	} else if (strcmp(cmd, "read") == 0) {
		if (cmd_mpp_read(bank_id, start_pin, end_pin))
			return -EINVAL;
	} else if (strcmp(cmd, "write") == 0) {
		if (cmd_mpp_write(bank_id, start_pin, func))
			return -EINVAL;
	} else {
		printf("ERROR: unknown command to mpp: \"%s\"\n", cmd);
		return -EINVAL;
	}

	return 0;
}

U_BOOT_CMD(
	mpp,      6,     0,      do_mpp_cmd,
	"mpp - Display or modify MPP values\n",
	"\n"
	"Display or modify MPP values\n"
	"\tlist			- List all MPP banks\n"
	"\tread <bank>		- Read all MPP values on bank\n"
	"\tread <bank> <pin>	- Read MPP function\n"
	"\tread <bank> <x> <y>  - Read MPP values of pin x to y\n"
	"\twrite <bank> <pin> <func> - Modify the function of an MPP\n"
);


static int cmd_mppbus_enable(int bus_id)
{
	struct mpp_bus *bus = mpp_get_bus(bus_id);

	printf("Enabling MPP bus %s\n", bus->name);
	mpp_enable_bus(bus->name);

	return 0;
}

static int cmd_mppbus_list(int bus_id)
{
	int id, start, end, i;
	struct mpp_bus *bus;
	struct mpp_pin *pin;
	u32 curr;

	start = bus_id;
	end = bus_id + 1;

	if (bus_id < 0) {
		start = 0;
		end = MAX_MPP_BUSES;
	}

	printf("\nId  Name            Pins  Status\n");
	printf("----------------------------------\n");

	for (id = start; id < end; id++) {
		bus = mpp_get_bus(id);
		if (bus == NULL)
			continue;

		printf("%02d  %-15s %-2d     %s\n", id, bus->name, bus->pin_cnt,
		       mpp_is_bus_enabled(bus->name) ? "Enabled" : "Disabled");
	}

	/* For single bus call, list the bus pins */
	if (bus_id >= 0) {
		printf("\npin  func  curr  status\n");
		printf("-----------------------\n");
		bus = mpp_get_bus(bus_id);
		for (i = 0; i < bus->pin_cnt; i++) {
			pin = &bus->pins[i];
			curr = mpp_get_pin_func(bus->bank_id, pin->id);
			printf(" %d    %d    %d     %s\n", pin->id, pin->func, curr,
				pin->func == curr ? "set" : "unset");
		}
	}
	return 0;
}

int do_mppbus_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd = argv[1];
	int bus_id = -1;

	if ((strcmp(cmd, "enable") == 0) && (argc < 3)) {
		printf("Error: Please specify MPP bus id and alternative no\n");
		return 1;
	}

	if (argc > 2)
		bus_id = (int)simple_strtoul(argv[2], NULL, 10);

	if (bus_id > MAX_MPP_BUSES) {
		error("MPP bus id exceeds maximum of %d\n", MAX_MPP_BUSES);
		return -EINVAL;
	}

	if ((strcmp(cmd, "list") == 0) || (argc < 2)) {
		if (cmd_mppbus_list(bus_id))
			return -EINVAL;
	} else if (strcmp(cmd, "enable") == 0) {
		if (cmd_mppbus_enable(bus_id))
			return -EINVAL;
	} else {
		error("unknown command to mppbus: \"%s\"\n", cmd);
		return -EINVAL;
	}

	return 0;
}

U_BOOT_CMD(
	mppbus,      6,     0,      do_mppbus_cmd,
	"mppbus - Display or modify MPP bus info\n",
	"\n"
	"Display or modify MPP bus info\n"
	"\tlist		- Display available MPP buses\n"
	"\tlist <x>	- Display MPP bus x info\n"
	"\tenable <x> <y> - Enable MPP bus x alternative y\n"
);
