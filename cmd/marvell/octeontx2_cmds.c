/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * SPDX-License-Identifier:    GPL-2.0
 * https://spdx.org/licenses
 */

#include <common.h>
#include <command.h>
#include <asm/arch/smc.h>

static int mrvl_otx2_fsafe_clr_sec_boot(
	cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	return smc_flsf_clr_force_2ndry();
}

U_BOOT_CMD(
	mrvl_otx2_fsafe_clr_sec_boot, 1, 1, mrvl_otx2_fsafe_clr_sec_boot,
	"Marvell OcteonTX2 Fail Safe: clear secondary boot", ""
);
