// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <dm.h>
#include <asm/io.h>
#include <errno.h>
#include <mvebu/fuse-mvebu.h>

#define GET_LEN(width)  DIV_ROUND_UP(width, 32)
#define ROW_WORDS_LEN	3
#define ECC_BITS_MASK	0xfe000000

static int valid_prog_words;
static u32 prog_val[ROW_WORDS_LEN];

int mvebu_efuse_hd_read(struct udevice *dev, int row_id, u32 *val)
{
	void __iomem *otp_mem;
	int row_base, i, row_widths;
	struct mvebu_fuse_block_data *priv = dev_get_priv(dev);

	row_base = priv->row_base;
	otp_mem = priv->target_otp_mem + (row_id - row_base) *
		   priv->pdata->row_step;
	row_widths = priv->pdata->row_bit_width;

	for (i = 0; i < GET_LEN(row_widths); i++)
		*(val + i) = readl(otp_mem + 4 * i);

	return 0;
}

static int do_mvebu_efuse_hd_prog(struct udevice *dev, int row_id, u32 *new_val)
{
	void __iomem *otp_mem, *ctrl_reg;
	int row_base, i, row_widths;
	struct mvebu_fuse_block_data *priv = dev_get_priv(dev);
	u32 fuse_read_value[ROW_WORDS_LEN];

	row_base = priv->row_base;
	otp_mem = priv->target_otp_mem + (row_id - row_base) *
		   priv->pdata->row_step;
	ctrl_reg = priv->control_reg;
	row_widths = priv->pdata->row_bit_width;

	/* only write a fuse line with lock bit */
	if (!(*(new_val + 2) & 0x1))
		return -EINVAL;
	/* according to specs ECC protection bits must be 0 on write */
	if (*(new_val + 1) & ECC_BITS_MASK)
		return -EINVAL;

	/* enable fuse prog */
#ifndef DRY_RUN
	setbits_le32(ctrl_reg, MVEBU_EFUSE_CTRL_PROGRAM_ENABLE);
#else
	/* avoid build warning for enable DRY_RUN */
	ctrl_reg = ctrl_reg;
#endif
	/* read fuse row value before burn fuse */
	for (i = 0; i < GET_LEN(row_widths); i++)
		fuse_read_value[i] = readl(otp_mem + 4 * i);
	/* fuse row value burn */
	for (i = 0; i < GET_LEN(row_widths); i++) {
		fuse_read_value[i] |= *(new_val + i);
		writel(fuse_read_value[i], otp_mem + 4 * i);
	}

	/* wait 1 ms for burn efuse */
	mdelay(1);

	/* Disable efuse write */
#ifndef DRY_RUN
	clrbits_le32(ctrl_reg, MVEBU_EFUSE_CTRL_PROGRAM_ENABLE);
#endif
	return 0;
}

int mvebu_efuse_hd_prog(struct udevice *dev, int word, int row_id, u32 new_val)
{
	int res = 0;

	if (word < ROW_WORDS_LEN - 1) {
		prog_val[word] = new_val;
		valid_prog_words |= (1 << word);
	}

	if (word == ROW_WORDS_LEN - 1) {
		if ((valid_prog_words & 3) == 0 && new_val) {
			prog_val[0] = 0;
			prog_val[1] = 0;
			/* lock bit is set to 1*/
			prog_val[2] = 1;
			res = do_mvebu_efuse_hd_prog(dev, row_id, prog_val);
			valid_prog_words = 0;
		} else if ((valid_prog_words & 3) == 3 && new_val) {
			prog_val[ROW_WORDS_LEN - 1] = new_val;
			res = do_mvebu_efuse_hd_prog(dev, row_id, prog_val);
			valid_prog_words = 0;
		} else {
			res = -EINVAL;
			valid_prog_words = 0;
		}
	}

	return res;
}
