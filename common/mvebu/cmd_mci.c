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
#include <asm/arch-mvebu/mvebu_mci.h>

static enum mci_unit parse_unit(const char *unit)
{
	if ((strcmp(unit, "ap_phy") == 0))
		return AP_PHY;
	else if ((strcmp(unit, "ap_ctrl") == 0))
		return AP_CTRL;
	else if ((strcmp(unit, "cp_phy") == 0))
		return CP_PHY;
	else if ((strcmp(unit, "cp_ctrl") == 0))
		return CP_CTRL;
	else
		return MCI_MAX;
}

int do_mci_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *unit = argv[2];
	enum mci_unit phy_unit;
	u32 reg_ofs;
	u32 value;
	int unit_id = simple_strtoul(argv[3], NULL, 10);
	phy_unit = parse_unit(unit);
	if (phy_unit == MCI_MAX) {
		error("Error: unit %s is not supported\n", unit);
		return 1;
	}


	/* Get Offset */
	reg_ofs = simple_strtoul(argv[4], NULL, 16);

	/* read commnad */
	if (strcmp(argv[1], "read") == 0) {
		if (phy_unit == AP_PHY || phy_unit == CP_PHY) {
			if (mvebu_mci_phy_read(MCI_PHY_REG_REGION, phy_unit, unit_id, reg_ofs, &value))
				return 1;
			printf("0x%x: 0x%x\n", reg_ofs, value);
		} else if (phy_unit == AP_CTRL || phy_unit == CP_CTRL) {
			if (mvebu_mci_phy_read(MCI_CTRL_REGION, phy_unit, unit_id, reg_ofs, &value))
				return 1;
			printf("0x%x: 0x%x\n", reg_ofs, value);
		} else {
			error("unknown unit\n");
			return 1;
		}
	/* write commnad */
	} else if (strcmp(argv[1], "write") == 0) {
		if (phy_unit == AP_PHY || phy_unit == CP_PHY) {
			value = simple_strtoul(argv[5], NULL, 16);

			if (mvebu_mci_phy_write(MCI_PHY_REG_REGION, phy_unit, unit_id, reg_ofs, value))
				return 1;
		} else if (phy_unit == AP_CTRL || phy_unit == CP_CTRL) {
			value = simple_strtoul(argv[5], NULL, 16);

			if (mvebu_mci_phy_write(MCI_CTRL_REGION, phy_unit, unit_id, reg_ofs, value))
				return 1;
		} else {
			error("unknown unit\n");
			return 1;
		}
	} else {
		error("unknown command: %s\n", argv[1]);
		return 1;
	}
	return 0;
}

U_BOOT_CMD(
	mci,      6,     0,      do_mci_cmd,
	"Access to MCI indirect registers\n",
	"<cmd> <mci_type> <mci_num> <offset> <value>\n"
	"	- read/write from/to mci registers\n"
	"\n"
	"Parameters:\n"
	"\tcmd			read/write\n"
	"\tmci type		ap_phy,ap_ctrl,cp_phy,cp_ctrl\n"
	"\tmci num		0/1\n"
	"\toffset		register address\n"
	"\tvalue                register data to write\n"
	"Examples:\n"
	"\t-mci read ap_phy 0 0xa\n"
	"\t-mci read ap_ctrl 0 0x5\n"
	"\t-mci read ap_phy 1 0x7\n"
	"\t-mci read ap_ctrl 1 0x3\n"
	"\t-mci read cp_phy 0 0x8\n"
	"\t-mci read cp_ctrl 0 0x4\n"
	"\t-mci read cp_phy 1 0x0\n"
	"\t-mci read cp_ctrl 1 0x12\n"
	"\t-mci write ap_phy 0 0xa 0\n"
	"\t-mci write ap_ctrl 0 0x5 1\n"
	"\t-mci write ap_phy 1 0x7 2\n"
	"\t-mci write ap_ctrl 1 0x3 3\n"
	"\t-mci write cp_phy 0 0x8 4\n"
	"\t-mci write cp_ctrl 0 0x4 5\n"
	"\t-mci write cp_phy 1 0x0 6\n"
	"\t-mci write cp_ctrl 1 0x12 7\n"
);
