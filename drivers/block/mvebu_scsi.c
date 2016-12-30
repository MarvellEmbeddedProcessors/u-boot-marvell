/*
 * Copyright (C) 2016 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#include <common.h>
#include <scsi.h>
#include <dm.h>

DECLARE_GLOBAL_DATA_PTR;

static int mvebu_scsi_probe(struct udevice *bus)
{
	/* Do nothing */
	return 0;
}

static const struct udevice_id mvebu_scsi_ids[] = {
	{ .compatible = "marvell,mvebu-scsi" },
	{ }
};

U_BOOT_DRIVER(scsi_mvebu_drv) = {
	.name		= "scsi_mvebu",
	.id		= UCLASS_SCSI,
	.of_match	= mvebu_scsi_ids,
	.probe		= mvebu_scsi_probe,
};

