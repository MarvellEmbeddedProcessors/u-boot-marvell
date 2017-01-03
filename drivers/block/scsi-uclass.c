/*
 * Copyright (c) 2015 Google, Inc
 * Written by Simon Glass <sjg@chromium.org>
 * Copyright (c) 2016 Xilinx, Inc
 * Written by Michal Simek
 *
 * Based on ahci-uclass.c
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <scsi.h>

DECLARE_GLOBAL_DATA_PTR;

static int scsi_post_probe(struct udevice *dev)
{
	debug("%s: device %p\n", __func__, dev);
	scsi_low_level_init(0, dev);
	return 0;
}

static void scsi_ofdata_to_uclass_platdata(struct udevice *dev)
{
	struct scsi_platdata *plat = dev_get_uclass_platdata(dev);
	const void *blob = gd->fdt_blob;
	int node = dev->of_offset;

	plat->base = (unsigned long)dev_get_addr_ptr(dev);
	plat->max_id = fdtdec_get_uint(blob,
				       node,
				       "max-id",
				       CONFIG_SYS_SCSI_MAX_SCSI_ID);
	plat->max_lun = fdtdec_get_uint(blob,
					node,
					"max-lun",
					CONFIG_SYS_SCSI_MAX_LUN);
	return;
}

static int scsi_post_bind(struct udevice *dev)
{
	/* Get uclass plat data from fdt */
	scsi_ofdata_to_uclass_platdata(dev);
}

UCLASS_DRIVER(scsi) = {
	.id		= UCLASS_SCSI,
	.name		= "scsi",
	.post_bind	= scsi_post_bind,
	.post_probe	 = scsi_post_probe,
	.per_device_platdata_auto_alloc_size = sizeof(struct scsi_platdata),
};
