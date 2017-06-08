/*
 * Copyright (C) 2017 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
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
	const char *unit;
	enum mci_unit phy_unit;
	u32 reg_ofs;
	u32 value;
	int ret;
	int unit_id = simple_strtoul(argv[3], NULL, 10);

	/* Verify enough arguments entered by user */
	if (argc < 5)
		goto input_error;

	unit = argv[2];
	phy_unit = parse_unit(unit);
	if (phy_unit == MCI_MAX) {
		error("Error: unit %s is not supported\n", unit);
		return CMD_RET_USAGE;
	}


	/* Get Offset */
	reg_ofs = simple_strtoul(argv[4], NULL, 16);

	/* read commnad */
	if (strcmp(argv[1], "read") == 0) {
		/* Verify required arguments entered by user */
		if (argc != 5)
			goto input_error;

		if (phy_unit == AP_PHY || phy_unit == CP_PHY) {
			ret = mvebu_mci_phy_read(MCI_PHY_REG_REGION, phy_unit,
					       unit_id, reg_ofs, &value);
			if (ret)
				return ret;

			printf("0x%x: 0x%x\n", reg_ofs, value);
		} else if (phy_unit == AP_CTRL || phy_unit == CP_CTRL) {
			ret = mvebu_mci_phy_read(MCI_CTRL_REGION, phy_unit,
					       unit_id, reg_ofs, &value);
			if (ret)
				return ret;

			printf("0x%x: 0x%x\n", reg_ofs, value);
		} else {
			error("unknown unit\n");
			return -EINVAL;
		}
	/* write commnad */
	} else if (strcmp(argv[1], "write") == 0) {
		/* Verify required arguments entered by user */
		if (argc != 6)
			goto input_error;

		if (phy_unit == AP_PHY || phy_unit == CP_PHY) {
			value = simple_strtoul(argv[5], NULL, 16);
			ret = mvebu_mci_phy_write(MCI_PHY_REG_REGION, phy_unit,
						unit_id, reg_ofs, value);
			if (ret)
				return ret;

		} else if (phy_unit == AP_CTRL || phy_unit == CP_CTRL) {
			value = simple_strtoul(argv[5], NULL, 16);
			ret = mvebu_mci_phy_write(MCI_CTRL_REGION, phy_unit,
						unit_id, reg_ofs, value);
			if (ret)
				return ret;

		} else {
			error("unknown unit\n");
			return -EINVAL;
		}
	} else {
		error("unknown command: %s\n", argv[1]);
		return -EINVAL;
	}
	return 0;

input_error:
	printf("\nInput error: Please go over command help:\n");
	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	mci,      6,     0,      do_mci_cmd,
	"\nAccess to MCI indirect registers\n",
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
