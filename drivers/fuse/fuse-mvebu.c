// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <config.h>
#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/soc.h>
#include <dm.h>
#include <fdt_support.h>
#include "fuse-mvebu.h"

DECLARE_GLOBAL_DATA_PTR;

#define GET_LEN(width)  DIV_ROUND_UP(width, 32)

static int row_index;

static int get_fuse_device(u32 bank, struct udevice **devp)
{
	struct mvebu_fuse_block_data *priv;
	struct udevice *dev;

	for (uclass_first_device(UCLASS_MISC, &dev); dev;
	     uclass_next_device(&dev)) {
		if ((device_is_compatible(dev, "marvell,mvebu-fuse-hd"))) {
			priv = dev_get_priv(dev);
			if (bank >= priv->row_base &&
			    bank < priv->row_base + priv->row_num) {
				*devp = dev;
				break;
			}
		}
	}

	return 0;
}

int fuse_read(u32 bank, u32 word, u32 *val)
{
	int res = 0;
	u32 *fuse_data;
	struct udevice *dev = NULL;
	struct mvebu_fuse_block_data *priv;
	const struct fuse_ops *ops;
	int row_widths;

	if (bank < 0 || bank >= row_index)
		return -EINVAL;

	res = get_fuse_device(bank, &dev);
	if (res)
		return res;

	priv = dev_get_priv(dev);
	ops = device_get_ops(dev);
	row_widths = priv->pdata->row_bit_width;

	if (word >= GET_LEN(row_widths))
		return -EINVAL;

	fuse_data = kmalloc_array(GET_LEN(row_widths), sizeof(u32),
				  GFP_KERNEL);

	res = ops->fuse_hd_read(dev, bank, fuse_data);

	if (res)
		goto read_end;

	*val = fuse_data[word];

read_end:
	kfree(fuse_data);
	return res;
}

int fuse_sense(u32 bank, u32 word, u32 *val)
{
	/* not supported */
	return -EPERM;
}

int fuse_prog(u32 bank, u32 word, u32 val)
{
	int res = 0;
	struct udevice *dev = NULL;
	struct mvebu_fuse_block_data *priv;
	const struct fuse_ops *ops;
	int row_widths;

	if (bank < 0 || bank >= row_index)
		return -EINVAL;

	res = get_fuse_device(bank, &dev);
	if (res)
		return res;

	priv = dev_get_priv(dev);
	ops = device_get_ops(dev);
	row_widths = priv->pdata->row_bit_width;

	if (word >= GET_LEN(row_widths))
		return -EINVAL;

	res = ops->fuse_hd_prog(dev, word, bank, val);

	return res;
}

int fuse_override(u32 bank, u32 word, u32 val)
{
	/* not supported */
	return -EPERM;
}

static int fuse_probe(struct udevice *dev)
{
	const void *blob = gd->fdt_blob;
	int node = dev_of_offset(dev);
	struct mvebu_fuse_block_data *priv = dev_get_priv(dev);
	const struct fuse_ops *ops = NULL;
	int res = 0;

	priv->control_reg = dev_read_addr_ptr(dev);
	if (IS_ERR(priv->control_reg))
		return -EINVAL;

	priv->pdata =
		(struct mvebu_fuse_platform_data *)dev_get_driver_data(dev);
#ifndef DRY_RUN
	priv->target_otp_mem =
		(void *)(((unsigned long)fdtdec_get_int(blob, node,
			"otp-mem", 0)) & 0xffffffff);

#else
	priv->target_otp_mem = NULL;
#endif

	ops = device_get_ops(dev);
	res = ops->fuse_init(dev);
	if (res)
		return res;

	priv->row_num = fdtdec_get_int(blob, node, "rows-count", 1);

	priv->row_base = row_index;
	row_index = priv->row_num + row_index;

	return res;
}

static struct fuse_ops fuse_ops = {
	.fuse_init = mvebu_efuse_init_hw,
	.fuse_hd_read = mvebu_efuse_hd_read,
	.fuse_hd_prog = mvebu_efuse_hd_prog,
};

static struct mvebu_fuse_platform_data fuse_hd_pdata = {
	.row_bit_width = 65,
	.row_step = 16,
};

static const struct udevice_id mvebu_fuse_ids[] = {
	{ .compatible = "marvell,mvebu-fuse-hd",
		.data = (ulong)&fuse_hd_pdata },
	{}
};

int fuse_bind(struct udevice *dev)
{
	struct udevice *bank;
	struct uclass *uc;
	const void *blob = gd->fdt_blob;
	int node = dev_of_offset(dev);
	int ret = 0;

	fdtdec_get_alias_seq(blob, "fuse", node, &dev->req_seq);

	/* Get MISC uclass */
	ret = uclass_get(UCLASS_MISC, &uc);
	if (ret)
		return ret;

	/*
	 * Reorder the efuse bank device in devices
	 * list by its request sequence number.
	 */
	uclass_foreach_dev(bank, uc) {
		node = dev_of_offset(bank);
		fdtdec_get_alias_seq(blob, "fuse", node, &bank->req_seq);
		if ((device_is_compatible(bank, "marvell,mvebu-fuse-hd"))) {
			if (bank->req_seq > dev->req_seq) {
				list_move(&dev->uclass_node,
					  bank->uclass_node.prev);
				break;
			}
		}
	}

	return 0;
}

U_BOOT_DRIVER(mvebu_efuse) = {
	.name	= "mvebu_efuse",
	.id	= UCLASS_MISC,
	.of_match = mvebu_fuse_ids,
	.probe	= fuse_probe,
	.priv_auto_alloc_size = sizeof(struct mvebu_fuse_block_data),
	.ops	= &fuse_ops,
	.bind	= fuse_bind,
};

