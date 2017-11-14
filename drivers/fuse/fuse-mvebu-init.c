/*
 * Copyright (C) 2017 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#include <config.h>
#include <common.h>
#include <asm/arch/cpu.h>
#include <linux/mbus.h>
#include <dm.h>
#include <asm/io.h>
#include <errno.h>
#include "fuse-mvebu.h"

#define MBUS_EFUSE_SIZE BIT(20)
#define EFUSE_ADDR_MASK	0xFF000000

int mvebu_efuse_init_hw(struct udevice *dev)
{
	struct mvebu_fuse_block_data *priv = dev_get_priv(dev);
	int ret = 0;
	phys_addr_t efuse_base;

	efuse_base = (phys_addr_t)priv->target_otp_mem & EFUSE_ADDR_MASK;
	ret = mvebu_mbus_add_window_by_id(
		CPU_TARGET_SATA23_DFX, 0xA, efuse_base, MBUS_EFUSE_SIZE);

	return ret;
}

