/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * SPDX-License-Identifier:    GPL-2.0
 * https://spdx.org/licenses
 */

#include <command.h>
#include <dm.h>
#include <asm/arch/smc.h>

static int do_efi_cmd(struct cmd_tbl *cmdtp, int flag, int argc,
		      char *const argv[])
{
	char filesize[64];
	char *endp;
	u64 addr = 0, size = 0;
	int ret = CMD_RET_USAGE, image_id = -1;

	if (argc < 3)
		return ret;

	image_id = simple_strtol(argv[1], &endp, 0);
	if (image_id < 1 || image_id > 2)
		return ret;
	addr = simple_strtoull(argv[2], &endp, 16);
	if (!addr)
		return ret;

	ret = smc_load_efi_img(image_id, addr, &size);
	if (ret)
		return CMD_RET_FAILURE;

	snprintf(filesize, sizeof(filesize), "%llx", size);
	env_set("filesize", filesize);
	printf("Loaded EFI App image at 0x%llx with 0x%llx bytes\n", addr,
	       size);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	efiload, 3, 1, do_efi_cmd, "Load EFI App",
	"Load EFI Application image from secure SPI flash to memory\n"
	"efiload [Image ID] [Load address]\n"
	"    - Image ID can be either 1 or 2 only.\n"
	"    - ID=1 will load EFI_IMAGE1[efi_app1.efi]\n"
	"    - ID=2 will load EFI_IMAGE2[efi_app2.efi]\n"
	"\n"
	"    - Loaded image size will be printed out\n"
	"\n"
	"Example - efiload 1 $loadaddr\n"
);
