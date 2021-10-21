/*
 * Copyright (C) 2021 Marvell International Ltd.
 *
 * SPDX-License-Identifier:    GPL-2.0
 * https://spdx.org/licenses
 */

#include <common.h>
#include <command.h>
#include <asm/arch/smc.h>

static int set_avs_status(
	struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint8_t avs_status = 0;
	if(argc != 2)
		return CMD_RET_USAGE;
	else {
		if (strcmp(argv[1], "start") == 0)
			avs_status = 1;
		else if (strcmp(argv[1], "stop") == 0)
			avs_status = 0;
		else
			return CMD_RET_USAGE;
	}
	(void)smc_set_avsstatus(avs_status);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	avs, 2, 0, set_avs_status,
	"Marvell OcteonTX2 Set AVS : start / stop", "\navs start\n"
	"avs stop\n"
);
