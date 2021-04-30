// SPDX-License-Identifier:    GPL-2.0
// https://spdx.org/licenses
// Copyright (C) 2020 Marvell International Ltd.

#include <command.h>
#include <dm.h>
#include <asm/arch/smc.h>
#include <asm/arch-cn10k/switch.h>

static int process_prbs_get_request(struct cmd_tbl *cmdtp, int flag, int argc,
				    char *const argv[])
{
	unsigned long value;
	u32 dev_num, intf_num, flag_val, lane_num;
	int ret = CMD_RET_USAGE;

	if (argc < 7) {
		printf("USAGE: swcfg get prbs <dev num> <port num> <lane num> <accumulate>\n");
		return CMD_RET_SUCCESS;
	}

	value = 0;
	ret = strict_strtoul(argv[3], 10, &value);
	if (ret) {
		printf("Invalid device number\n");
		printf("USAGE: swcfg get prbs <dev num> <port num> <lane num> <accumulate>\n");
		return CMD_RET_SUCCESS;
	}
	dev_num = value;

	value = 0;
	ret = strict_strtoul(argv[4], 10, &value);
	if (ret) {
		printf("Invalid interface number\n");
		printf("USAGE: swcfg get prbs <dev num> <port num> <lane num> <accumulate>\n");
		return CMD_RET_SUCCESS;
	}
	intf_num = value;

	value = 0;
	ret = strict_strtoul(argv[5], 10, &value);
	if (ret) {
		printf("Invalid lane number\n");
		printf("USAGE: swcfg get prbs <dev num> <port num> <lane num> <accumulate>\n");
		return CMD_RET_SUCCESS;
	}
	lane_num = value;

	value = 0;
	ret = strict_strtoul(argv[6], 10, &value);
	if (ret) {
		printf("Invalid <accumulate>: 0 - Reset counters / 1 - Accumulate\n");
		printf("USAGE: swcfg get prbs <dev num> <port num> <lane num> <accumulate>\n");
		return CMD_RET_SUCCESS;
	}
	flag_val = value;

	u32 params[3];

	memset(params, 0, sizeof(params));
	ret = switch_cmd_opcode12(dev_num, intf_num, lane_num, flag_val, params);
	if (ret) {
		printf("CMD Error\n");
	} else {
		printf("PRBS Count: 0x%x\n", params[0]);
		printf("Pattern Count: 0x%x\n", params[1]);
		printf("Lock Status: 0x%x\n", params[2]);
	}
	return CMD_RET_SUCCESS;
}

static int process_prbs_set_request(struct cmd_tbl *cmdtp, int flag, int argc,
				    char *const argv[])
{
	unsigned long value;
	u32 dev_num, intf_num, flag_val, lane_num, reg_val;
	int ret = CMD_RET_USAGE;

	if (argc < 8) {
		printf("USAGE: swcfg set prbs <dev num> <port num> <lane num> ");
		printf("<prbs mode> <enable>\n");
		printf("<enable> : 1 - Enable PRBS mode, 0 - Disable PRBS mode\n");
		return CMD_RET_SUCCESS;
	}

	value = 0;
	ret = strict_strtoul(argv[3], 10, &value);
	if (ret) {
		printf("Invalid device number\n");
		printf("USAGE: swcfg set prbs <dev num> <port num> <lane num> ");
		printf("<prbs mode> <enable>\n");
		printf("<enable> : 1 - Enable PRBS mode, 0 - Disable PRBS mode\n");
		return CMD_RET_SUCCESS;
	}
	dev_num = value;

	value = 0;
	ret = strict_strtoul(argv[4], 10, &value);
	if (ret) {
		printf("Invalid interface number\n");
		printf("USAGE: swcfg set prbs <dev num> <port num> <lane num> ");
		printf("<prbs mode> <enable>\n");
		printf("<enable> : 1 - Enable PRBS mode, 0 - Disable PRBS mode\n");
		return CMD_RET_SUCCESS;
	}
	intf_num = value;

	value = 0;
	ret = strict_strtoul(argv[5], 10, &value);
	if (ret) {
		printf("Invalid lane number\n");
		printf("USAGE: swcfg set prbs <dev num> <port num> <lane num> ");
		printf("<prbs mode> <enable>\n");
		printf("<enable> : 1 - Enable PRBS mode, 0 - Disable PRBS mode\n");
		return CMD_RET_SUCCESS;
	}
	lane_num = value;

	value = 0;
	ret = strict_strtoul(argv[6], 10, &value);
	if (ret) {
		printf("Invalid PRBS mode\n");
		printf("USAGE: swcfg set prbs <dev num> <port num> <lane num> ");
		printf("<prbs mode> <enable>\n");
		printf("<enable> : 1 - Enable PRBS mode, 0 - Disable PRBS mode\n");
		return CMD_RET_SUCCESS;
	}
	reg_val = value;

	value = 0;
	ret = strict_strtoul(argv[7], 10, &value);
	if (ret) {
		printf("Invalid flag, 0 or 1\n");
		printf("USAGE: swcfg set prbs <dev num> <port num> <lane num> ");
		printf("<prbs mode> <enable>\n");
		printf("<enable> : 1 - Enable PRBS mode, 0 - Disable PRBS mode\n");
		return CMD_RET_SUCCESS;
	}
	flag_val = value;

	ret = switch_cmd_opcode11(dev_num, intf_num,
				  lane_num, reg_val, flag_val);
	if (ret)
		printf("CMD Error\n");
	return CMD_RET_SUCCESS;
}

static int process_serdes_get_request(struct cmd_tbl *cmdtp, int flag, int argc,
				      char *const argv[])
{
	unsigned long value;
	u32 dev_num, intf_num, lane_num;
	int ret = CMD_RET_USAGE;

	if (argc < 4)
		return CMD_RET_USAGE;

	if (!strcmp(argv[3], "rx")) {
		if (argc < 7) {
			printf("USAGE: swcfg get serdes rx  <dev num> <port num> <lane num>\n");
			return CMD_RET_SUCCESS;
		}

		value = 0;
		ret = strict_strtoul(argv[4], 10, &value);
		if (ret) {
			printf("Invalid device number\n");
			printf("USAGE: swcfg get serdes rx  <dev num> <port num> <lane num>\n");
			return CMD_RET_SUCCESS;
		}
		dev_num = value;

		value = 0;
		ret = strict_strtoul(argv[5], 10, &value);
		if (ret) {
			printf("Invalid interface number\n");
			printf("USAGE: swcfg get serdes rx  <dev num> <port num> <lane num>\n");
			return CMD_RET_SUCCESS;
		}
		intf_num = value;

		value = 0;
		ret = strict_strtoul(argv[6], 10, &value);
		if (ret) {
			printf("Invalid lane number\n");
			printf("USAGE: swcfg get serdes rx  <dev num> <port num> <lane num>\n");
			return CMD_RET_SUCCESS;
		}
		lane_num = value;

		u32 params[6];

		memset(params, 0, sizeof(params));
		ret = switch_cmd_opcode22(dev_num, intf_num, lane_num, params);
		if (ret) {
			printf("CMD Error\n");
		} else {
			printf("DC Gain : 0x%x\n", params[0]);
			printf("Low frequency gain : 0x%x\n", params[1]);
			printf("High frequency gain : 0x%x\n", params[2]);
			printf("CTLE bandwidth : 0x%x\n", params[3]);
			printf("CTLE loop bandwidth : 0x%x\n", params[4]);
			printf("Squelch detector threshold : 0x%x\n", params[5]);
		}
		return CMD_RET_SUCCESS;

	} else if (!strcmp(argv[3], "tx")) {
		if (argc < 7) {
			printf("USAGE: swcfg get serdes tx  <dev num> <port num> <lane num>\n");
			return CMD_RET_SUCCESS;
		}

		value = 0;
		ret = strict_strtoul(argv[4], 10, &value);
		if (ret) {
			printf("Invalid device number\n");
			printf("USAGE: swcfg get serdes tx  <dev num> <port num> <lane num>\n");
			return CMD_RET_SUCCESS;
		}
		dev_num = value;

		value = 0;
		ret = strict_strtoul(argv[5], 10, &value);
		if (ret) {
			printf("Invalid interface number\n");
			printf("USAGE: swcfg get serdes tx  <dev num> <port num> <lane num>\n");
			return CMD_RET_SUCCESS;
		}
		intf_num = value;

		value = 0;
		ret = strict_strtoul(argv[6], 10, &value);
		if (ret) {
			printf("Invalid lane number\n");
			printf("USAGE: swcfg get serdes tx  <dev num> <port num> <lane num>\n");
			return CMD_RET_SUCCESS;
		}
		lane_num = value;

		u32 params[5];

		memset(params, 0, sizeof(params));
		ret = switch_cmd_opcode21(dev_num, intf_num, lane_num, params);
		if (ret) {
			printf("CMD Error\n");
		} else {
			printf("Tx Driver output amplitude : 0x%x\n", params[0]);
			printf("Tx amplitude adjust : 0x%x\n", params[1]);
			printf("Emphasis amplitude for Gen0 : 0x%x\n", params[2]);
			printf("Emphasis amplitude for Gen1 : 0x%x\n", params[3]);
			printf("Tx amplitude shift : 0x%x\n", params[4]);
		}
		return CMD_RET_SUCCESS;

	} else {
		return CMD_RET_USAGE;
	}

	return CMD_RET_SUCCESS;
}

static int process_serdes_set_request(struct cmd_tbl *cmdtp, int flag, int argc,
				      char *const argv[])
{
	unsigned long value;
	long svalue;
	u32 dev_num, intf_num, lane_num;
	int ret = CMD_RET_USAGE;

	if (argc < 4)
		return CMD_RET_USAGE;

	if (!strcmp(argv[3], "rx")) {
		if (argc < 13) {
			printf("USAGE: swcfg set serdes rx <dev num> <port num> ");
			printf("<lane num> <dc gain> <lf gain> <hf gain> ");
			printf("<ctle bw> <ctle loop bw> <sq thresh>\n");
			return CMD_RET_SUCCESS;
		}

		value = 0;
		ret = strict_strtoul(argv[4], 10, &value);
		if (ret) {
			printf("Invalid device number\n");
			printf("USAGE: swcfg set serdes rx <dev num> <port num> ");
			printf("<lane num> <dc gain> <lf gain> <hf gain> ");
			printf("<ctle bw> <ctle loop bw> <sq thresh>\n");
			return CMD_RET_SUCCESS;
		}
		dev_num = value;

		value = 0;
		ret = strict_strtoul(argv[5], 10, &value);
		if (ret) {
			printf("Invalid interface number\n");
			printf("USAGE: swcfg set serdes rx <dev num> <port num> ");
			printf("<lane num> <dc gain> <lf gain> <hf gain> ");
			printf("<ctle bw> <ctle loop bw> <sq thresh>\n");
			return CMD_RET_SUCCESS;
		}
		intf_num = value;

		value = 0;
		ret = strict_strtoul(argv[6], 10, &value);
		if (ret) {
			printf("Invalid lane number\n");
			printf("USAGE: swcfg set serdes rx <dev num> <port num> ");
			printf("<lane num> <dc gain> <lf gain> <hf gain> ");
			printf("<ctle bw> <ctle loop bw> <sq thresh>\n");
			return CMD_RET_SUCCESS;
		}
		lane_num = value;

		u32 dc_gain = 0;

		value = 0;
		ret = strict_strtoul(argv[7], 10, &value);
		if (ret || value > 15) {
			printf("Invalid dc gain\n");
			printf("USAGE: swcfg set serdes rx <dev num> <port num> ");
			printf("<lane num> <dc gain> <lf gain> <hf gain> ");
			printf("<ctle bw> <ctle loop bw> <sq thresh>\n");
			return CMD_RET_SUCCESS;
		}
		dc_gain = value;

		u32 lf_gain = 0;

		value = 0;
		ret = strict_strtoul(argv[8], 10, &value);
		if (ret || value > 15) {
			printf("Invalid low frequency gain\n");
			printf("USAGE: swcfg set serdes rx <dev num> <port num> ");
			printf("<lane num> <dc gain> <lf gain> <hf gain> ");
			printf("<ctle bw> <ctle loop bw> <sq thresh>\n");
			return CMD_RET_SUCCESS;
		}
		lf_gain = value;

		u32 hf_gain = 0;

		value = 0;
		ret = strict_strtoul(argv[9], 10, &value);
		if (ret || value > 15) {
			printf("Invalid high frequency gain\n");
			printf("USAGE: swcfg set serdes rx <dev num> <port num> ");
			printf("<lane num> <dc gain> <lf gain> <hf gain> ");
			printf("<ctle bw> <ctle loop bw> <sq thresh>\n");
			return CMD_RET_SUCCESS;
		}
		hf_gain = value;

		u32 ctle_bw = 0;

		value = 0;
		ret = strict_strtoul(argv[10], 10, &value);
		if (ret || value > 15) {
			printf("Invalid CTLE bandwidth\n");
			printf("USAGE: swcfg set serdes rx <dev num> <port num> ");
			printf("<lane num> <dc gain> <lf gain> <hf gain> ");
			printf("<ctle bw> <ctle loop bw> <sq thresh>\n");
			return CMD_RET_SUCCESS;
		}
		ctle_bw = value;

		u32 ctle_loop_bw = 0;

		value = 0;
		ret = strict_strtoul(argv[11], 10, &value);
		if (ret || value > 15) {
			printf("Invalid CTLE loop bandwidth\n");
			printf("USAGE: swcfg set serdes rx <dev num> <port num> ");
			printf("<lane num> <dc gain> <lf gain> <hf gain> ");
			printf("<ctle bw> <ctle loop bw> <sq thresh>\n");
			return CMD_RET_SUCCESS;
		}
		ctle_loop_bw = value;

		u32 squelch_thresh = 0;

		value = 0;
		ret = strict_strtoul(argv[12], 10, &value);
		if (ret || value > 310) {
			printf("Invalid squelch detector threshold\n");
			printf("USAGE: swcfg set serdes rx <dev num> <port num> ");
			printf("<lane num> <dc gain> <lf gain> <hf gain> ");
			printf("<ctle bw> <ctle loop bw> <sq thresh>\n");
			return CMD_RET_SUCCESS;
		}
		squelch_thresh = value;

		ret = switch_cmd_opcode14(dev_num, intf_num, lane_num, dc_gain, lf_gain,
					  hf_gain, ctle_bw, ctle_loop_bw, squelch_thresh);
		if (ret)
			printf("CMD Error\n");
		return CMD_RET_SUCCESS;

	} else if (!strcmp(argv[3], "tx")) {
		if (argc < 11) {
			printf("USAGE: swcfg set serdes tx <dev num> <port num> ");
			printf("<amplitude> <amp adjust> <emphasis 0> ");
			printf("<emphasis 1> <amp shift>\n");
			return CMD_RET_SUCCESS;
		}

		value = 0;
		ret = strict_strtoul(argv[4], 10, &value);
		if (ret) {
			printf("Invalid device number\n");
			printf("USAGE: swcfg set serdes tx <dev num> <port num> ");
			printf("<amplitude> <amp adjust> <emphasis 0> ");
			printf("<emphasis 1> <amp shift>\n");
			return CMD_RET_SUCCESS;
		}
		dev_num = value;

		value = 0;
		ret = strict_strtoul(argv[5], 10, &value);
		if (ret) {
			printf("Invalid port number\n");
			printf("USAGE: swcfg set serdes tx <dev num> <port num> ");
			printf("<amplitude> <amp adjust> <emphasis 0> ");
			printf("<emphasis 1> <amp shift>\n");
			return CMD_RET_SUCCESS;
		}
		intf_num = value;

		u32 amplitude = 0;

		value = 0;
		ret = strict_strtoul(argv[6], 10, &value);
		if (ret) {
			printf("Invalid TX driver amplitude\n");
			printf("USAGE: swcfg set serdes tx <dev num> <port num> ");
			printf("<amplitude> <amp adjust> <emphasis 0> ");
			printf("<emphasis 1> <amp shift>\n");
			return CMD_RET_SUCCESS;
		}
		amplitude = value;

		u32 amplitude_adj = 0;

		value = 0;
		ret = strict_strtoul(argv[7], 10, &value);
		if (ret || value > 31) {
			printf("Invalid amplitude adjust enable\n");
			printf("USAGE: swcfg set serdes tx <dev num> <port num> ");
			printf("<amplitude> <amp adjust> <emphasis 0> ");
			printf("<emphasis 1> <amp shift>\n");
			return CMD_RET_SUCCESS;
		}
		amplitude_adj = value;

		s32 emphasis0 = 0;

		svalue = simple_strtol(argv[8], NULL, 10);
		if (svalue < -7 || svalue > 15) {
			printf("Invalid emphasis amplitude for Gen0\n");
			printf("USAGE: swcfg set serdes tx <dev num> <port num> ");
			printf("<amplitude> <amp adjust> <emphasis 0> ");
			printf("<emphasis 1> <amp shift>\n");
			return CMD_RET_SUCCESS;
		}
		emphasis0 = svalue;

		s32 emphasis1 = 0;

		svalue = simple_strtol(argv[9], NULL, 10);
		if (svalue < -31 || svalue > 31) {
			printf("Invalid emphasis amplitude for Gen1\n");
			printf("USAGE: swcfg set serdes tx <dev num> <port num> ");
			printf("<amplitude> <amp adjust> <emphasis 0> ");
			printf("<emphasis 1> <amp shift>\n");
			return CMD_RET_SUCCESS;
		}
		emphasis1 = svalue;

		u32 amp_shift = 0;

		value = 0;
		ret = strict_strtoul(argv[10], 10, &value);
		if (ret) {
			printf("Invalid TX amplitude shift\n");
			printf("USAGE: swcfg set serdes tx <dev num> <port num> ");
			printf("<amplitude> <amp adjust> <emphasis 0> ");
			printf("<emphasis 1> <amp shift>\n");
			return CMD_RET_SUCCESS;
		}
		amp_shift = value;

		ret = switch_cmd_opcode13(dev_num, intf_num, amplitude,
					  amplitude_adj, emphasis0, emphasis1, amp_shift);
		if (ret)
			printf("CMD Error\n");
		return CMD_RET_SUCCESS;
	} else {
		return CMD_RET_USAGE;
	}

	return CMD_RET_SUCCESS;
}

static int process_port_ap_configuration(struct cmd_tbl *cmdtp, int flag, int argc,
					 char *const argv[])
{
	unsigned long value;
	long svalue;
	u32 dev_num, intf_num, lane_num;
	int ret = CMD_RET_USAGE;

	if (!strcmp(argv[4], "rx")) {
		if (argc < 19) {
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}

		value = 0;
		ret = strict_strtoul(argv[5], 10, &value);
		if (ret) {
			printf("Invalid device number\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		dev_num = value;

		value = 0;
		ret = strict_strtoul(argv[6], 10, &value);
		if (ret) {
			printf("Invalid interface number\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		intf_num = value;

		value = 0;
		ret = strict_strtoul(argv[7], 10, &value);
		if (ret) {
			printf("Invalid lane number\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		lane_num = value;

		u32 speed;

		value = 0;
		ret = strict_strtoul(argv[8], 10, &value);
		if (ret) {
			printf("Invalid port speed value\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		speed = value;

		u32 squelch_thresh = 0;

		value = 0;
		ret = strict_strtoul(argv[9], 10, &value);
		if (ret || value > 310) {
			printf("Invalid squelch detector threshold\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		squelch_thresh = value;

		u32 lf_gain = 0;

		value = 0;
		ret = strict_strtoul(argv[10], 10, &value);
		if (ret || value > 15) {
			printf("Invalid low frequency gain\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		lf_gain = value;

		u32 hf_gain = 0;

		value = 0;
		ret = strict_strtoul(argv[11], 10, &value);
		if (ret || value > 15) {
			printf("Invalid high frequency gain\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		hf_gain = value;

		u32 dc_gain = 0;

		value = 0;
		ret = strict_strtoul(argv[12], 10, &value);
		if (ret || value > 15) {
			printf("Invalid DC gain\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		hf_gain = value;

		u32 ctle_bw = 0;

		value = 0;
		ret = strict_strtoul(argv[13], 10, &value);
		if (ret || value > 15) {
			printf("Invalid CTLE bandwidth\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		ctle_bw = value;

		u32 ctle_loop_bw = 0;

		value = 0;
		ret = strict_strtoul(argv[14], 10, &value);
		if (ret || value > 15) {
			printf("Invalid CTLE loop bandwidth\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		ctle_loop_bw = value;

		u32 etl_min_delay = 0;

		value = 0;
		ret = strict_strtoul(argv[15], 10, &value);
		if (ret || value > 31) {
			printf("Invalid ETL min delay\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		etl_min_delay = value;

		u32 etl_max_delay = 0
			;
		value = 0;
		ret = strict_strtoul(argv[16], 10, &value);
		if (ret || value > 31) {
			printf("Invalid ETL max delay\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		etl_max_delay = value;

		u32 etl_enable = 0;

		value = 0;
		ret = strict_strtoul(argv[17], 10, &value);
		if (ret) {
			printf("Invalid ETL enable\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		etl_enable = value;

		u32 override_bitmap = 0;

		value = 0;
		ret = strict_strtoul(argv[18], 10, &value);
		if (ret) {
			printf("Invalid field override bitmap\n");
			printf("USAGE: swcfg set cfg ap rx <dev num> <port num> <lane num>");
			printf(" <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw>");
			printf(" <ctle loop bw> <etl min delay> <etl max delay> <etl enable>");
			printf(" <override bitmap>\n");
			return CMD_RET_SUCCESS;
		}
		override_bitmap = value;

		ret = switch_cmd_opcode17(dev_num, intf_num, lane_num, speed, squelch_thresh,
					  lf_gain, hf_gain, dc_gain, ctle_bw, ctle_loop_bw,
					  etl_min_delay, etl_max_delay, etl_enable,
					  override_bitmap);
		if (ret)
			printf("CMD Error\n");
		return CMD_RET_SUCCESS;

	} else if (!strcmp(argv[4], "tx")) {
		if (argc < 12) {
			printf("USAGE: swcfg set cfg ap tx <dev num> <port num> <lane num>");
			printf(" <speed> <tx amp offset> <tx emph0 offset> <tx emph1 offset>\n");
			return CMD_RET_SUCCESS;
		}

		value = 0;
		ret = strict_strtoul(argv[5], 10, &value);
		if (ret) {
			printf("Invalid device number\n");
			printf("USAGE: swcfg set cfg ap tx <dev num> <port num> <lane num>");
			printf(" <speed> <tx amp offset> <tx emph0 offset> <tx emph1 offset>\n");
			return CMD_RET_SUCCESS;
		}
		dev_num = value;

		value = 0;
		ret = strict_strtoul(argv[6], 10, &value);
		if (ret) {
			printf("Invalid interface number\n");
			printf("USAGE: swcfg set cfg ap tx <dev num> <port num> <lane num>");
			printf(" <speed> <tx amp offset> <tx emph0 offset> <tx emph1 offset>\n");
			return CMD_RET_SUCCESS;
		}
		intf_num = value;

		value = 0;
		ret = strict_strtoul(argv[7], 10, &value);
		if (ret) {
			printf("Invalid lane number\n");
			printf("USAGE: swcfg set cfg ap tx <dev num> <port num> <lane num>");
			printf(" <speed> <tx amp offset> <tx emph0 offset> <tx emph1 offset>\n");
			return CMD_RET_SUCCESS;
		}
		lane_num = value;

		u32 speed;

		value = 0;
		ret = strict_strtoul(argv[8], 10, &value);
		if (ret) {
			printf("Invalid port speed value\n");
			printf("USAGE: swcfg set cfg ap tx <dev num> <port num> <lane num>");
			printf(" <speed> <tx amp offset> <tx emph0 offset> <tx emph1 offset>\n");
			return CMD_RET_SUCCESS;
		}
		speed = value;

		s32 tx_amp_offset;

		svalue = 0;
		svalue = simple_strtol(argv[9], NULL, 10);
		if (svalue < -15 || svalue > 15) {
			printf("Invalid TX amplitude offset\n");
			printf("USAGE: swcfg set cfg ap tx <dev num> <port num> <lane num>");
			printf(" <speed> <tx amp offset> <tx emph0 offset> <tx emph1 offset>\n");
			return CMD_RET_SUCCESS;
		}
		tx_amp_offset = svalue;

		s32 tx_emph0_offset;

		svalue = 0;
		svalue = simple_strtol(argv[10], NULL, 10);
		if (svalue < -15 || svalue > 15) {
			printf("Invalid TX emphasis 0 offset\n");
			printf("USAGE: swcfg set cfg ap tx <dev num> <port num> <lane num>");
			printf(" <speed> <tx amp offset> <tx emph0 offset> <tx emph1 offset>\n");
			return CMD_RET_SUCCESS;
		}
		tx_emph0_offset = svalue;

		s32 tx_emph1_offset;

		svalue = 0;
		svalue = simple_strtol(argv[11], NULL, 10);
		if (svalue < -15 || svalue > 15) {
			printf("Invalid TX emphasis 1 offset\n");
			printf("USAGE: swcfg set cfg ap tx <dev num> <port num> <lane num>");
			printf(" <speed> <tx amp offset> <tx emph0 offset> <tx emph1 offset>\n");
			return CMD_RET_SUCCESS;
		}
		tx_emph1_offset = svalue;

		ret = switch_cmd_opcode18(dev_num, intf_num, lane_num, speed, tx_amp_offset,
					  tx_emph0_offset, tx_emph1_offset);
		if (ret)
			printf("CMD Error\n");
	} else {
		if (argc < 13) {
			printf("USAGE: swcfg set cfg ap <dev num> <port num> <lane num>");
			printf(" <fc pause> <fc am dir> <fec supported> <fec required> <mode> <speed>\n");
			return CMD_RET_SUCCESS;
		}

		value = 0;
		ret = strict_strtoul(argv[4], 10, &value);
		if (ret) {
			printf("Invalid device number\n");
			printf("USAGE: swcfg set cfg ap <dev num> <port num> <lane num>");
			printf(" <fc pause> <fc am dir> <fec supported> <fec required> <mode> <speed>\n");
			return CMD_RET_SUCCESS;
		}
		dev_num = value;

		value = 0;
		ret = strict_strtoul(argv[5], 10, &value);
		if (ret) {
			printf("Invalid interface number\n");
			printf("USAGE: swcfg set cfg ap <dev num> <port num> <lane num>");
			printf(" <fc pause> <fc am dir> <fec supported> <fec required> <mode> <speed>\n");
			return CMD_RET_SUCCESS;
		}
		intf_num = value;

		value = 0;
		ret = strict_strtoul(argv[6], 10, &value);
		if (ret) {
			printf("Invalid lane number\n");
			printf("USAGE: swcfg set cfg ap <dev num> <port num> <lane num>");
			printf(" <fc pause> <fc am dir> <fec supported> <fec required> <mode> <speed>\n");
			return CMD_RET_SUCCESS;
		}
		lane_num = value;

		u32 fc_pause;

		value = 0;
		ret = strict_strtoul(argv[7], 10, &value);
		if (ret) {
			printf("Invalid FC pause\n");
			printf("USAGE: swcfg set cfg ap <dev num> <port num> <lane num>");
			printf(" <fc pause> <fc am dir> <fec supported> <fec required> <mode> <speed>\n");
			return CMD_RET_SUCCESS;
		}
		fc_pause = value;

		u32 fc_am_dir;

		value = 0;
		ret = strict_strtoul(argv[8], 10, &value);
		if (ret) {
			printf("Invalid FC assymetric direction\n");
			printf("USAGE: swcfg set cfg ap <dev num> <port num> <lane num>");
			printf(" <fc pause> <fc am dir> <fec supported> <fec required> <mode> <speed>\n");
			return CMD_RET_SUCCESS;
		}
		fc_am_dir = value;

		u32 fec_supported;

		value = 0;
		ret = strict_strtoul(argv[9], 10, &value);
		if (ret) {
			printf("Invalid FEC supported value\n");
			printf("USAGE: swcfg set cfg ap <dev num> <port num> <lane num>");
			printf(" <fc pause> <fc am dir> <fec supported> <fec required> <mode> <speed>\n");
			return CMD_RET_SUCCESS;
		}
		fec_supported = value;

		u32 fec_required;

		value = 0;
		ret = strict_strtoul(argv[10], 10, &value);
		if (ret) {
			printf("Invalid FEC required value\n");
			printf("USAGE: swcfg set cfg ap <dev num> <port num> <lane num>");
			printf(" <fc pause> <fc am dir> <fec supported> <fec required> <mode> <speed>\n");
			return CMD_RET_SUCCESS;
		}
		fec_required = value;

		u32 mode;

		value = 0;
		ret = strict_strtoul(argv[11], 10, &value);
		if (ret) {
			printf("Invalid port mode value\n");
			printf("USAGE: swcfg set cfg ap <dev num> <port num> <lane num>");
			printf(" <fc pause> <fc am dir> <fec supported> <fec required> <mode> <speed>\n");
			return CMD_RET_SUCCESS;
		}
		mode = value;

		u32 speed;

		value = 0;
		ret = strict_strtoul(argv[12], 10, &value);
		if (ret) {
			printf("Invalid port speed value\n");
			printf("USAGE: swcfg set cfg ap <dev num> <port num> <lane num>");
			printf(" <fc pause> <fc am dir> <fec supported> <fec required> <mode> <speed>\n");
			return CMD_RET_SUCCESS;
		}
		speed = value;

		ret = switch_cmd_opcode15(dev_num, intf_num, lane_num, fc_pause, fc_am_dir,
					  fec_supported, fec_required, mode, speed);
		if (ret)
			printf("CMD Error\n");
		return CMD_RET_SUCCESS;
	}
	return CMD_RET_SUCCESS;
}

static int process_port_configuration(struct cmd_tbl *cmdtp, int flag, int argc,
				      char *const argv[])
{
	unsigned long value;
	u32 dev_num, intf_num;
	int ret = CMD_RET_USAGE;

	if (argc < 8) {
		printf("USAGE: swcfg set cfg <dev num> <port num> <speed> <mode> <fec>\n");
		return CMD_RET_SUCCESS;
	}

	value = 0;
	ret = strict_strtoul(argv[3], 10, &value);
	if (ret) {
		printf("Invalid device number\n");
		printf("USAGE: swcfg set cfg <dev num> <port num> <speed> <mode> <fec>\n");
		return CMD_RET_SUCCESS;
	}
	dev_num = value;

	value = 0;
	ret = strict_strtoul(argv[4], 10, &value);
	if (ret) {
		printf("Invalid interface number\n");
		printf("USAGE: swcfg set cfg <dev num> <port num> <speed> <mode> <fec>\n");
		return CMD_RET_SUCCESS;
	}
	intf_num = value;

	u32 speed;

	value = 0;
	ret = strict_strtoul(argv[5], 10, &value);
	if (ret) {
		printf("Invalid speed\n");
		printf("USAGE: swcfg set cfg <dev num> <port num> <speed> <mode> <fec>\n");
		return CMD_RET_SUCCESS;
	}
	speed = value;

	u32 intf_mode;

	value = 0;
	ret = strict_strtoul(argv[6], 10, &value);
	if (ret) {
		printf("Invalid interface mode\n");
		printf("USAGE: swcfg set cfg <dev num> <port num> <speed> <mode> <fec>\n");
		return CMD_RET_SUCCESS;
	}
	intf_mode = value;

	u32 fec;

	value = 0;
	ret = strict_strtoul(argv[7], 10, &value);
	if (ret) {
		printf("Invalid FEC\n");
		printf("USAGE: swcfg set cfg <dev num> <port num> <speed> <mode> <fec>\n");
		return CMD_RET_SUCCESS;
	}
	fec = value;

	ret = switch_cmd_opcode10(dev_num, intf_num, speed, intf_mode, fec);
	if (ret)
		printf("CMD Error\n");
	return CMD_RET_SUCCESS;
}

static int do_switch_cmd(struct cmd_tbl *cmdtp, int flag, int argc,
			 char *const argv[])
{
	unsigned long value;
	u8 result;
	u32 dev_num, intf_num, reg_val, flag_val, lane_num;
	int ret = CMD_RET_USAGE;

	if (argc < 2)
		return ret;

	if (!strcmp(argv[1], "profile")) {
	/* Opcode 4 command */
		if (argc < 3) {
			printf("Missing profile number\n");
			printf("USAGE: swcfg profile <profile num>\n");
			return CMD_RET_SUCCESS;
		}

		ret = strict_strtoul(argv[2], 10, &value);
		if (ret) {
			printf("USAGE: swcfg profile <profile num>\n");
			return CMD_RET_SUCCESS;
		}

		result = switch_cmd_opcode4((u8)value);
		switch (result) {
		case 0:
			ret = CMD_RET_SUCCESS;
			break;
		case 16:
			printf("Invalid profile number\n");
			ret = CMD_RET_FAILURE;
			break;
		case 17:
			printf("Cannot find configuration file\n");
			ret = CMD_RET_FAILURE;
			break;
		default:
			printf("Invalid return code [0x%x]\n", result);
			ret = CMD_RET_FAILURE;
			break;
		}
	} else if (!strcmp(argv[1], "version")) {
		char buffer[128];

		memset(buffer, 0, sizeof(buffer));
		ret = switch_cmd_opcode5(buffer);
		if (ret) {
			pr_debug("%s %d : version cmd error [0x%x]\n", __func__, __LINE__, ret);
			return CMD_RET_FAILURE;
		}

		printf("%s\n", buffer);
		printf("%s\n", &buffer[32]);
		printf("%s\n", &buffer[64]);
		printf("%s\n", &buffer[96]);
		ret = CMD_RET_SUCCESS;
	} else if (!strcmp(argv[1], "name")) {
		char buffer[252];

		memset(buffer, 0, sizeof(buffer));

		if (argc < 4) {
			printf("USAGE: swcfg name <id> <group>\n");
			return CMD_RET_SUCCESS;
		}

		value = 0;
		ret = strict_strtoul(argv[2], 10, &value);
		if (ret) {
			printf("Invalid Id\n");
			printf("USAGE: swcfg name <id> <group>\n");
			return CMD_RET_SUCCESS;
		}
		dev_num = value;

		value = 0;
		ret = strict_strtoul(argv[3], 10, &value);
		if (ret) {
			printf("Invalid group\n");
			printf("USAGE: swcfg name <id> <group>\n");
			return CMD_RET_SUCCESS;
		}
		lane_num = value;

		ret = switch_cmd_opcode7(dev_num, lane_num, buffer);
		if (ret) {
			pr_debug("%s %d : names cmd error [0x%x]\n", __func__, __LINE__, ret);
			return CMD_RET_FAILURE;
		}

		printf("%s\n", buffer);
		ret = CMD_RET_SUCCESS;
	} else if (!strcmp(argv[1], "get")) {
		if (argc < 3)
			return CMD_RET_USAGE;

		if (!strcmp(argv[2], "status")) {
			if (argc < 5) {
				printf("USAGE: swcfg get status <dev num> <port num>\n");
				return CMD_RET_SUCCESS;
			}

			value = 0;
			ret = strict_strtoul(argv[3], 10, &value);
			if (ret) {
				printf("Invalid device number\n");
				printf("USAGE: swcfg get status <dev num> <port num>\n");
				return CMD_RET_SUCCESS;
			}
			dev_num = value;

			value = 0;
			ret = strict_strtoul(argv[4], 10, &value);
			if (ret) {
				printf("Invalid port number\n");
				printf("USAGE: swcfg get status <dev num> <port num>\n");
				return CMD_RET_SUCCESS;
			}
			intf_num = value;

			reg_val = 0;
			ret = switch_cmd_opcode6(dev_num, intf_num, &reg_val);
			if (ret)
				printf("CMD Error\n");
			else
				printf("STATUS : 0x%04x\n", reg_val);
			return CMD_RET_SUCCESS;

		} else if (!strcmp(argv[2], "prbs")) {
			return process_prbs_get_request(cmdtp, flag, argc, argv);
		} else if (!strcmp(argv[2], "serdes")) {
			return process_serdes_get_request(cmdtp, flag, argc, argv);
		} else {
			return CMD_RET_USAGE;
		}
	} else if (!strcmp(argv[1], "set")) {
		if (argc < 3)
			return CMD_RET_USAGE;

		if (!strcmp(argv[2], "lpbk")) {
			if (argc < 6) {
				printf("USAGE: swcfg set lpbk <dev num> <port num> <lpbk mode>\n");
				printf("0 no lpbk, 1 serdes tx2rx, 2 serdes rx2tx, 3 mac tx2rx\n");
				return CMD_RET_SUCCESS;
			}

			value = 0;
			ret = strict_strtoul(argv[3], 10, &value);
			if (ret) {
				printf("Invalid device number\n");
				printf("USAGE: swcfg set lpbk <dev num> <port num> <lpbk mode>\n");
				printf("0 no lpbk, 1 serdes tx2rx, 2 serdes rx2tx, 3 mac tx2rx\n");
				return CMD_RET_SUCCESS;
			}
			dev_num = value;

			value = 0;
			ret = strict_strtoul(argv[4], 10, &value);
			if (ret) {
				printf("Invalid interface number\n");
				printf("USAGE: swcfg set lpbk <dev num> <port num> <lpbk mode>\n");
				printf("0 no lpbk, 1 serdes tx2rx, 2 serdes rx2tx, 3 mac tx2rx\n");
				return CMD_RET_SUCCESS;
			}
			intf_num = value;

			value = 0;
			ret = strict_strtoul(argv[5], 10, &value);
			if (ret || value > 3) {
				printf("Invalid loopback mode\n");
				printf("USAGE: swcfg set lpbk <dev num> <port num> <lpbk mode>\n");
				printf("0 no lpbk, 1 serdes tx2rx, 2 serdes rx2tx, 3 mac tx2rx\n");
				return CMD_RET_SUCCESS;
			}
			flag_val = value;

			reg_val = 0;
			ret = switch_cmd_opcode19(dev_num, intf_num, flag_val);
			if (ret)
				printf("CMD Error\n");
			return CMD_RET_SUCCESS;

		} else if (!strcmp(argv[2], "prbs")) {
			return process_prbs_set_request(cmdtp, flag, argc, argv);
		} else if (!strcmp(argv[2], "nego")) {
			if (argc < 6) {
				printf("USAGE: swcfg set nego <dev num> <port num> <negotiation mode>\n");
				printf("0 - 10Gbps full, 1 - 1Gbps full, 2 - 100Mbps full, 3 - 100Mbps");
				printf(" half 4 - 10Mbps full, 5 - 10Mbps half\n");
				return CMD_RET_SUCCESS;
			}

			value = 0;
			ret = strict_strtoul(argv[3], 10, &value);
			if (ret) {
				printf("Invalid device number\n");
				printf("USAGE: swcfg set nego <dev num> <port num> <negotiation mode>\n");
				printf("0 - 10Gbps full, 1 - 1Gbps full, 2 - 100Mbps full, 3 - 100Mbps half 4 - 10Mbps full, 5 - 10Mbps half\n");
				return CMD_RET_SUCCESS;
			}
			dev_num = value;

			value = 0;
			ret = strict_strtoul(argv[4], 10, &value);
			if (ret) {
				printf("Invalid interface number\n");
				printf("USAGE: swcfg set nego <dev num> <port num> <negotiation mode>\n");
				printf("0 - 10Gbps full, 1 - 1Gbps full, 2 - 100Mbps full, 3 - 100Mbps half 4 - 10Mbps full, 5 - 10Mbps half\n");
				return CMD_RET_SUCCESS;
			}
			intf_num = value;

			value = 0;
			ret = strict_strtoul(argv[5], 10, &value);
			if (ret || value > 3) {
				printf("Invalid negotiation mode\n");
				printf("USAGE: swcfg set nego <dev num> <port num> <negotiation mode>\n");
				printf("0 - 10Gbps full, 1 - 1Gbps full, 2 - 100Mbps full, 3 - 100Mbps half 4 - 10Mbps full, 5 - 10Mbps half\n");
				return CMD_RET_SUCCESS;
			}
			flag_val = value;

			reg_val = 0;
			ret = switch_cmd_opcode20(dev_num, intf_num, flag_val);
			if (ret)
				printf("CMD Error\n");
			return CMD_RET_SUCCESS;

		} else if (!strcmp(argv[2], "cfg")) {
			if (argc < 5)
				return CMD_RET_USAGE;

			if (!strcmp(argv[3], "ap"))
				return process_port_ap_configuration(cmdtp, flag, argc, argv);
			else
				return process_port_configuration(cmdtp, flag, argc, argv);

		} else if (!strcmp(argv[2], "serdes")) {
			return process_serdes_set_request(cmdtp, flag, argc, argv);
		} else {
			return CMD_RET_USAGE;
		}

	} else {
		return CMD_RET_USAGE;
	}

	return ret;
}

U_BOOT_CMD(swcfg, 19, 1, do_switch_cmd, "Switch Configuration Command",
	   "Configure network switch\n"
	   "	- <dev_num> = 0 for single switch systems\n"
	   "\n"
	   "swcfg profile <num>\n"
	   "    - configure switch with given profile number\n"
	   "swcfg version\n"
	   "    - versions of switch firmware files\n"
	   "swcfg name <profile id> <group>\n"
	   "    - names of switch firmware file\n"
	   "	- <group> = 0 if profiles less than 10\n"
	   "swcfg get status <dev num> <port num>\n"
	   "    - get port status\n"
#if 0
	   "swcfg set cfg <dev num> <port num> <speed> <mode> <fec>\n"
	   "    - set port configuration\n"
	   "swcfg set prbs <dev num> <port num> <lane num> <prbs mode> <enable>\n"
	   "    - set port PRBS format\n"
	   "swcfg get prbs <dev num> <port num> <lane num> <accumulate>\n"
	   "    - get port PRBS counts\n"
	   "swcfg set serdes tx <dev num> <port num> <amplitude> <amp adjust> <emph0> <emph1> <amp shift>\n"
	   "    - set SERDES TX parameters\n"
	   "swcfg set serdes rx <dev num> <port num> <lane num> <dc gain> <lf gain> <hf gain> <ctle bw> <ctle loop bw> <sq thresh>\n"
	   "    - set SERDES RX parameters\n"
	   "swcfg set cfg ap <dev num > <port num> <lane num> <fc pause> <fc dir> <fec supp> <fec req> <mode> <speed>\n"
	   "    - set port AP configuration\n"
	   "swcfg set cfg ap rx <dev num> <port num> <lane num> <speed> <sq thresh> <lf gain> <hf gain> <dc gain> <ctle bw> <ctle loop bw> <etl min delay> <etl max delay> <etl enable> <field override bitmap>\n"
	   "    - set port AP configuration advanced rx parameters\n"
	   "swcfg set cfg ap tx <dev num> <port num> <lane num> <speed> <tx amp offset> <tx emph0 offset> <tx emph1 offset>\n"
	   "    - set port AP configuration advanced tx parameters\n"
	   "swcfg set lpbk <dev num> <port num> <lpbk mode>\n"
	   "    - set interface loopback\n"
	   "swcfg set nego <dev num> <port num> <mode>\n"
	   "    - set interface negotiation\n"
	   "swcfg get serdes tx <dev num> <port num> <lane num>\n"
	   "    - get SERDES TX parameters\n"
	   "swcfg get serdes rx <dev num> <port num> <lane num>\n"
	   "    - get SERDES RX parameters\n"
	   "\n"
#endif
	   "Example - swcfg profile 3\n"
	   "Example - swcfg version\n"
	   "Example - swcfg name 3 0\n"
	   "Example - swcfg get status 0 6\n"
);
