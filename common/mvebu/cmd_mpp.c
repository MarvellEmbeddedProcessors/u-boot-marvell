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

#include <config.h>
#include <common.h>
#include <command.h>
#include <vsprintf.h>
#include <errno.h>
#include "asm/arch-mvebu/mpp.h"
#include "asm/arch/soc-info.h"

static int set_range(int *start, int *end)
{
	/* Print full range */
	if (*start < 0) {
		*start = 0;
		*end = MAX_MPP_ID;
	}
	/* Print single MPP */
	if ((*start >= 0) && (*end < 0))
		*end = *start;

	/* clip end to MAX_MPP_ID */
	*end = min(*end, MAX_MPP_ID);

	/* Error checking on range */
	if (*start > *end) {
		printf("Error: Start MPP (%d) higher then end MPP (%d)\n", *start, *end);
		return -EINVAL;
	}

	return 0;
}

static int cmd_mpp_list(char **mpp_desc, int start, int end)
{
	int i, pin, ret;
	char *mpp_opt;

	ret = set_range(&start, &end);
	if (ret)
		return ret;

	/* Print table head */
	printf("No  ");
	for (i = 0; i < MAX_MPP_OPTS; i++)
		printf("0x%-12x", i);
	printf("\n-----------------------------------------------------------------------------\n");

	for (pin = start; pin <= end; pin++) {
		printf("%02d  ", pin);
		for (i = 0; i < MAX_MPP_OPTS; i++) {
			mpp_opt = *(mpp_desc + (pin * MAX_MPP_OPTS) + i);
			printf("%-14s", mpp_opt);
		}
		printf("\n");
	}

	printf("\n");
	return 0;
}

static int cmd_mpp_read(char **mpp_desc, int start, int end)
{
	u8 value;
	int pin, ret;
	char *mpp_opt;

	ret = set_range(&start, &end);
	if (ret)
		return ret;

	printf("No  Value  Name\n");
	printf("--------------------------\n");

	for (pin = start; pin <= end; pin++) {
		value = min(mpp_get_pin(pin), (u8)MAX_MPP_OPTS);
		mpp_opt = *(mpp_desc + (pin * MAX_MPP_OPTS) + value);

		printf("%02d  0x%x   %s\n", pin, value, mpp_opt);
	}

	return 0;
}

static int cmd_mpp_write(char **mpp_desc, int pin, int value)
{
	char *mpp_opt;

	if ((pin < 0) || (pin > MAX_MPP_ID)) {
		printf("Error: Pin No %d out of range [%d, %d]\n", pin, 0, MAX_MPP_ID);
		return -EINVAL;
	}

	mpp_set_pin(pin, value);

	/* Readback to verify */
	value = min(mpp_get_pin(pin), (u8)MAX_MPP_OPTS);
	mpp_opt = *(mpp_desc + (pin * MAX_MPP_OPTS) + value);
	printf("%02d  0x%x   %s\n", pin, value, mpp_opt);

	return 0;
}


int do_mpp_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd = argv[1];
	char  **mpp_desc;
	int value = 0;
	int start_pin = -1;
	int end_pin = -1;

	if ((strcmp(cmd, "write") == 0) && (argc < 4)) {
		printf("Error: Please specify MPP number and value\n");
		return 1;
	}

	mpp_desc = mpp_get_desc_table();

	if (argc > 2)
		start_pin = (int)simple_strtoul(argv[2], NULL, 10);

	if (argc > 3) {
		value = (int)simple_strtoul(argv[3], NULL, 16);
		end_pin = (int)simple_strtoul(argv[3], NULL, 10);
	}

	if ((strcmp(cmd, "list") == 0) || (argc < 2)) {
		if (cmd_mpp_list(mpp_desc, start_pin, end_pin))
			return -EINVAL;
	} else if (strcmp(cmd, "read") == 0) {
		if (cmd_mpp_read(mpp_desc, start_pin, end_pin))
			return -EINVAL;
	} else if (strcmp(cmd, "write") == 0) {
		if (cmd_mpp_write(mpp_desc, start_pin, value))
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
	"\tlist		- Display MPP pins and their options\n"
	"\tlist <x>	- Display available option of MPP x\n"
	"\tlist <x> <y>	- Display available option of MPP x to y\n"
	"\tread		- Read all MPP values\n"
	"\tread <x>	- Read MPP x value\n"
	"\tread <x> <y>	- Read MPP x to y values\n"
	"\twrite x y	- Write y to MPP x\n"
);



static int cmd_mppbus_enable(char **mpp_desc, int bus_id)
{
	struct mpp_bus *bus = soc_get_mpp_bus(bus_id);
	int pin;
	int bus_alt = 0;

	if (bus_alt > (bus->bus_cnt - 1)) {
		error("Bus alternative %d not exist on bus %s", bus_alt, bus->name);
		return -EINVAL;
	}

	printf("Enabling bus %s Alternative %d\n", bus->name, bus_alt);
	for (pin = 0; pin < bus->pin_cnt; pin++) {
		u8 id = bus->pin_data[bus_alt][pin].id;
		u8 val = bus->pin_data[bus_alt][pin].val;
		mpp_set_pin(id, val);
	}
	return 0;
}

static int cmd_mppbus_list(char **mpp_desc, int bus_id)
{
	int id, opt, start, end, pin;
	struct mpp_bus *bus;

	start = bus_id;
	end = bus_id + 1;

	if (bus_id < 0) {
		start = 0;
		end = MAX_MPP_BUS;
	}

	printf("Id  Name            Pins  Status\n");
	printf("----------------------------------\n");
	for (id = start; id < end; id++) {
		bus = soc_get_mpp_bus(id);
		if (!mpp_is_bus_valid(bus))
			continue;
		if (mpp_is_bus_enabled(bus))
			printf("%02d  %-15s %d     %s\n", id, bus->name, bus->pin_cnt, "Enabled");
		else
			printf("%02d  %-15s %d     %s\n", id, bus->name, bus->pin_cnt, "Disabled");
	}

	if (bus_id >= 0) {
		bus = soc_get_mpp_bus(bus_id);
		for (opt = 0; opt < bus->bus_cnt; opt++) {
			printf("\nAlternative %d\n", opt);
			for (pin = 0; pin < bus->pin_cnt; pin++) {
				u8 id = bus->pin_data[opt][pin].id;
				u8 val = bus->pin_data[opt][pin].val;
				char *pin_name = *(mpp_desc + (id * MAX_MPP_OPTS) + val);

				printf("MPP %d  0x%x %s\n", id, val, pin_name);
			}
		}
	}
	return 0;
}

int do_mppbus_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd = argv[1];
	char  **mpp_desc;
	int bus_id = -1;

	if ((strcmp(cmd, "enable") == 0) && (argc < 3)) {
		printf("Error: Please specify MPP bus id and alternative no\n");
		return 1;
	}

	mpp_desc = mpp_get_desc_table();

	if (argc > 2)
		bus_id = (int)simple_strtoul(argv[2], NULL, 10);

	if (bus_id > MAX_MPP_BUS) {
		error("MPP bus id exceeds maximum of %d\n", MAX_MPP_BUS);
		return -EINVAL;
	}

	if ((strcmp(cmd, "list") == 0) || (argc < 2)) {
		if (cmd_mppbus_list(mpp_desc, bus_id))
			return -EINVAL;
	} else if (strcmp(cmd, "enable") == 0) {
		if (cmd_mppbus_enable(mpp_desc, bus_id))
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
