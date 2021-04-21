/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * SPDX-License-Identifier:    GPL-2.0
 * https://spdx.org/licenses
 */

#include <common.h>
#include <command.h>
#include <asm/arch/smc.h>

extern ssize_t smc_flsf_clr_force_2ndry(void);

static int do_fsafe_clr(
	struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	return smc_flsf_clr_force_2ndry();
}

U_BOOT_CMD(
	fsafe_clr, 1, 0, do_fsafe_clr,
	"Marvell OcteonTX2/CN10K Fail Safe: clear secondary boot", ""
);
