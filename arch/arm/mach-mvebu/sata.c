/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <ahci.h>
#include <dm.h>

DECLARE_GLOBAL_DATA_PTR;

/* pointer of sata base for future initialization */
void __iomem *sata_ptr;

/*
 * Dummy implementation that can be overwritten by a board
 * specific function
 */
__weak int board_ahci_enable(struct udevice *dev)
{
	return 0;
}

static int mvebu_ahci_probe(struct udevice *dev)
{
	/*
	 * Board specific SATA / AHCI enable code, e.g. enable the
	 * AHCI power or deassert reset
	 */
	board_ahci_enable(dev);

	/*
	 * save current sata ptr for future initialization,
	 * assuming there is only one controller.
	 */
	sata_ptr = dev_get_addr_ptr(dev);
	ahci_init(sata_ptr);

	return 0;
}

void scsi_bus_reset(void)
{
	ahci_init(sata_ptr);
}

static const struct udevice_id mvebu_ahci_ids[] = {
	{ .compatible = "marvell,armada-3700-ahci" },
	{ .compatible = "marvell,armada-8k-ahci" },
	{ }
};

U_BOOT_DRIVER(ahci_mvebu_drv) = {
	.name		= "ahci_mvebu",
	.id		= UCLASS_AHCI,
	.of_match	= mvebu_ahci_ids,
	.probe		= mvebu_ahci_probe,
};
