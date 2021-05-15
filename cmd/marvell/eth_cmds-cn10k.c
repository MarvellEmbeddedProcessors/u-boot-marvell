// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2021 Marvell
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <command.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <net.h>

extern int eth_intf_set_fec(struct udevice *ethdev, int type);
extern int eth_intf_get_fec(struct udevice *ethdev);
extern void nix_print_mac_info(struct udevice *dev);

static int do_ethlist(struct cmd_tbl *cmdtp, int flag, int argc,
		      char *const argv[])
{
	struct udevice *dev;

	for (uclass_find_first_device(UCLASS_ETH, &dev); dev;
	     uclass_find_next_device(&dev)) {
		printf("eth%d [%s]", dev->seq, dev->name);
		if (!strncmp(dev->name, "rvu_", 4))
			nix_print_mac_info(dev);
		printf("\n");
	}
	return 0;
}

U_BOOT_CMD(
	ethlist, 1, 1, do_ethlist, "Display ethernet interface list",
	"Prints all detected ethernet interfaces with below format\n"
	"ethX [device name] [LMAC info for RVU PF devices]\n"
);

static int do_ethparam_common(struct cmd_tbl *cmdtp, int flag, int argc,
			      char *const argv[])
{
	const char *cmd;
	char *endp;
	const char *devname;
	int ret = CMD_RET_USAGE, arg;
	struct udevice *dev = NULL;

	if (argc < 2)
		return ret;

	cmd = argv[0];

	devname = argv[1];
	dev = eth_get_dev_by_name(devname);
	if (!dev) {
		printf("device interface %s not found\n", devname);
		return CMD_RET_FAILURE;
	}
	if (strncmp(dev->name, "rvu_", 4)) {
		printf("Invalid eth interface choose RVU PF device\n");
		return CMD_RET_FAILURE;
	}

	if (strcmp(cmd, "set_fec") == 0) {
		if (argc < 3)
			return CMD_RET_FAILURE;
		arg = simple_strtol(argv[2], &endp, 0);
		if (arg < 0 || arg > 2)
			return ret;
		ret = eth_intf_set_fec(dev, arg);
	} else if (strcmp(cmd, "get_fec") == 0) {
		ret = eth_intf_get_fec(dev);
	}
	return (ret == 0) ? CMD_RET_SUCCESS : CMD_RET_FAILURE;
}

U_BOOT_CMD(
	set_fec, 3, 1, do_ethparam_common,
	"Modify fec type for selected ethernet interface",
	"Example - set_fec <ethX> [type]\n"
	"Set FEC type for any of RVU PF based network interfaces\n"
	"- where type - 0 [NO FEC] 1 [BASER_FEC] 2 [RS_FEC]\n"
	"Use 'ethlist' command to display network interface names\n"
);

U_BOOT_CMD(
	get_fec, 2, 1, do_ethparam_common,
	"Display fec type for selected ethernet interface",
	"Example - get_fec <ethX>\n"
	"Get FEC type for any of RVU PF based network interfaces\n"
	"Use 'ethlist' command to display network interface names\n"
);

