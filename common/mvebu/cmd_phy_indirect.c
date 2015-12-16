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
#include <asm/arch-mvebu/mvebu_phy_indirect.h>

static enum phy_indirect_unit parse_unit(const char *unit)
{
	if ((strcmp(unit, "ihb") == 0))
		return INDIRECT_IHB;
	else
		return INDIRECT_MAX;
}

int do_indirect_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *unit = argv[1];
	const char *cmd = argv[2];
	enum phy_indirect_unit phy_unit;
	u32 reg_ofs;
	u32 value;

	phy_unit = parse_unit(unit);
	if (phy_unit == INDIRECT_MAX) {
		error("Error: unit %s is not supported\n", unit);
		return 1;
	}

	if ((strcmp(cmd, "write") == 0) && (argc < 5)) {
		error("missing parameters for 'write' command\n");
		printf("make sure you specify both offset and value\n");
		return 1;
	}
	if ((strcmp(cmd, "read") == 0) && (argc < 4)) {
		error("missing parameters for 'read' command\n");
		printf("make sure you specify register offset\n");
		return 1;
	}

	/* Get Offset */
	reg_ofs = simple_strtoul(argv[3], NULL, 16);

	/* read commnad */
	if (strcmp(cmd, "read") == 0) {
		if (mvebu_phy_indirect_read(phy_unit, reg_ofs, &value))
			return 1;
		printf("0x%x: 0x%x\n", reg_ofs, value);
	} else if (strcmp(cmd, "write") == 0) {
		value = simple_strtoul(argv[4], NULL, 16);

		if (mvebu_phy_indirect_write(phy_unit, reg_ofs, value))
			return 1;
	} else {
		error("unknown command \"%s\"\n", cmd);
		return 1;
	}

	return 0;
}

U_BOOT_CMD(
	indirect,      5,     0,      do_indirect_cmd,
	"Access to PHY indirect registers\n",
	"<unit> <command> <offset> <value>\n"
	"	- Read/Write from/to indirect registers\n"
	"\n"
	"Parameters:\n"
	"\tunit		ihb\n"
	"\tcommand	read/write\n"
	"\toffset		register address\n"
	"\tvalue		register data to write\n"
	"Example: indirect ihb read 0x20\n"
);
