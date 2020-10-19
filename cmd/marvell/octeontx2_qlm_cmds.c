// SPDX-License-Identifier:    GPL-2.0
/*
 * https://spdx.org/licenses
 *
 * Copyright (C) 2019 Marvell International Ltd.
 */
#include <asm/psci.h>
#include <asm/system.h>
#include <common.h>
#include <command.h>
#include <linux/arm-smccc.h>
#include <linux/compiler.h>
#include <linux/delay.h>
#include <linux/psci.h>
#include <asm/arch/smc.h>

enum cgx_prbs_cmd {
        CGX_PRBS_START_CMD = 1,
        CGX_PRBS_STOP_CMD,
        CGX_PRBS_GET_DATA_CMD
};

#define SMCCC_RET_PENDING 1

static void waitforresult(struct arm_smccc_res *res)
{
	int fdelay = 1000;
	while (fdelay && (res->a0 == SMCCC_RET_PENDING)) {
		mdelay(100);
		fdelay = fdelay - 100;
	}
}

static int do_prbs(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	struct arm_smccc_res res;
	long time;
	ulong mode;
	ulong qlm;
	ulong lane;
	ulong delay;

	if (argc == 5) {
		qlm = simple_strtoul(argv[1], NULL, 10);
		lane = simple_strtoul(argv[2], NULL, 10);
		mode = simple_strtoul(argv[3], NULL, 10);
		time = simple_strtoul(argv[4], NULL, 10);
	} else {
		return CMD_RET_USAGE;
	}

	arm_smccc_smc(OCTEONTX_SERDES_DBG_PRBS, CGX_PRBS_START_CMD, qlm, mode, lane, 0, 0, 0, &res);

	waitforresult(&res);
	if (res.a0 != SMCCC_RET_SUCCESS) {
		return CMD_RET_FAILURE;
	}

	while(time > 0)
	{
		if(time > 5) {
			delay = 5;
			time = time - 5;
		}
		else {
			delay = time;
			time  = 0;
		}
		mdelay(delay * 1000);
		arm_smccc_smc(OCTEONTX_SERDES_DBG_PRBS, CGX_PRBS_GET_DATA_CMD, qlm, 1, lane, 0, 0, 0, &res);
		waitforresult(&res);

		if (res.a0 != SMCCC_RET_SUCCESS) {
			return CMD_RET_FAILURE;
		}
	}

	arm_smccc_smc(OCTEONTX_SERDES_DBG_PRBS, CGX_PRBS_STOP_CMD, qlm, mode, lane, 0, 0, 0, &res);
	waitforresult(&res);

	if (res.a0 != SMCCC_RET_SUCCESS) {
		return CMD_RET_FAILURE;
	}
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(prbs,    5,    1,     do_prbs,
	   "command to run PRBS on slected QLM",
	   "<qlm> <lane> <mode> <time> \n"
	   "    - run PRBS with pattern indicated by 'mode' on selected 'qlm'\n"
	   "      PRBS will be enabled by 'time' seconds\n"
	   "      PRBS is performed on a particular 'lane'"
);

static int do_eye(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	struct arm_smccc_res res;
	ulong lane;
	ulong qlm;

	if (argc != 3)
		return CMD_RET_USAGE;

	qlm = simple_strtoul(argv[1], NULL, 10);
	lane = simple_strtoul(argv[2], NULL, 10);

	arm_smccc_smc(OCTEONTX_SERDES_DBG_GET_EYE, qlm, lane, 1, 0, 0, 0, 0, &res);
	waitforresult(&res);

        if (res.a0 != SMCCC_RET_SUCCESS) {
                return CMD_RET_FAILURE;
        }
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(eye,    3,    1,     do_eye,
	   "command to display eye on slected QLM",
	   "<qlm> <lane>\n"
	   "    - run eye by 'lane'  'qlm'\n"
);

static int do_serdes(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	struct arm_smccc_res res;
	ulong lane;
	ulong qlm;

	if (argc != 3)
		return CMD_RET_USAGE;

	qlm = simple_strtoul(argv[1], NULL, 10);
	lane = simple_strtoul(argv[2], NULL, 10);

	arm_smccc_smc(OCTEONTX_SERDES_DBG_GET_CONF, qlm, lane, 1, 0, 0, 0, 0, &res);
	waitforresult(&res);

        if (res.a0 != SMCCC_RET_SUCCESS) {
                return CMD_RET_FAILURE;
        }
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(serdes,    3,    1,     do_serdes,
	   "command to display serdes state of a lane in a given QLM",
	   "<qlm> <lane>\n"
	   "    - display serdes state of 'lane' in 'qlm'\n"
);

static int do_loop(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	struct arm_smccc_res res;
	ulong lane;
	ulong qlm;
	ulong type;

	if (argc != 4)
		return CMD_RET_USAGE;

	qlm = simple_strtoul(argv[1], NULL, 10);
	lane = simple_strtoul(argv[2], NULL, 10);
	type = simple_strtoul(argv[3], NULL, 10);

	arm_smccc_smc(OCTEONTX_SERDES_DBG_SET_LOOP, qlm, lane, type, 0, 0, 0,
			0, &res);
	waitforresult(&res);

	if (res.a0 != SMCCC_RET_SUCCESS)
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(serdes_loop,    4,    1,     do_loop,
	   "command to display serdes state of a lane in a given QLM",
	   "<qlm> <lane> <flag>\n"
	   "    - configure serdes in FEA/NED/NEA/FED loopback mode on 'lane' in 'qlm'\n"
	   "	- if pass 'flag' as 1 = FEA/2 = NED/3 = NEA/4 = FED and 0 to disable\n"
);

static int do_tunetx(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	struct arm_smccc_res res;
	ulong lane;
	ulong qlm;
	ulong swing, pre, post;

	if (argc != 6)
		return CMD_RET_USAGE;

	qlm = simple_strtoul(argv[1], NULL, 10);
	lane = simple_strtoul(argv[2], NULL, 10);
	swing = simple_strtoul(argv[3], NULL, 10);
	pre = simple_strtoul(argv[4], NULL, 10);
	post = simple_strtoul(argv[5], NULL, 10);

	arm_smccc_smc(OCTEONTX_SERDES_DBG_SET_CONF, qlm, lane, swing,
			(pre << 8) | (post & 0xff), 0, 0, 0, &res);
	waitforresult(&res);

	if (res.a0 != SMCCC_RET_SUCCESS)
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(serdes_tune,    6,    1,     do_tunetx,
	   "command to configure serdes tuning parameters (swing, pre, post) on a lane in a given QLM",
	   "<qlm> <lane> <swing> <pre> <post>\n"
	   "    - configure serdes tuning on 'lane' in 'qlm'\n"
);
