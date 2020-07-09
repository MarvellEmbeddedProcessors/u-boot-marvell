// SPDX-License-Identifier:    GPL-2.0
/*
 * https://spdx.org/licenses
 *
 * Copyright (C) 2019 Marvell International Ltd.
 */
#include <asm/psci.h>
#include <common.h>
#include <command.h>
#include <linux/arm-smccc.h>
#include <linux/compiler.h>
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

static int do_prbs(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
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

static int do_eye(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
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

static int do_serdes(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
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

U_BOOT_CMD(serdes,    3,    1,     do_serdes,
	   "command to display serdes state of a lane in a given QLM",
	   "<qlm> <lane>\n"
	   "    - display serdes state of 'lane' in 'qlm'\n"
);
