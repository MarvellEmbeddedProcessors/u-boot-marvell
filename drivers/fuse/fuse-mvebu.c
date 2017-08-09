// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Marvell International Ltd.
 * Copyright (C) 2015-2016 Reinhard Pfau <reinhard.pfau@gdsys.cc>
 *
 * https://spdx.org/licenses
 */

#include <config.h>
#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/soc.h>
#include <linux/mbus.h>
#include <dm/device.h>
#include <dm.h>
#include <fdt_support.h>
#include "fuse-mvebu.h"

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_MVEBU_EFUSE_FAKE)
#define DRY_RUN
#else
#undef DRY_RUN
#endif

#define MBUS_EFUSE_SIZE		BIT(20)
#define EFUSE_ROW_REG_WIDTH	16

enum {
	MVEBU_EFUSE_CTRL_PROGRAM_ENABLE = (1 << 31),
};

struct mvebu_fuse_block_data {
	unsigned int	row_num;
	void	*control_reg;
	void	*target_otp_mem;
};

static struct mvebu_fuse_block_data efuse_data;

static void enable_efuse_program(void)
{
#ifndef DRY_RUN
	setbits_le32(efuse_data.control_reg, MVEBU_EFUSE_CTRL_PROGRAM_ENABLE);
#endif
}

static void disable_efuse_program(void)
{
#ifndef DRY_RUN
	clrbits_le32(efuse_data.control_reg, MVEBU_EFUSE_CTRL_PROGRAM_ENABLE);
#endif
}

static int do_prog_efuse(int nr, struct efuse_val *new_val,
			 u32 mask0, u32 mask1)
{
	struct efuse_val val;
	void __iomem *otp_mem;

	otp_mem = efuse_data.target_otp_mem + EFUSE_ROW_REG_WIDTH * nr;

	/* read efuse row value before burn */
	val.dwords.d[0] = readl(otp_mem);
	val.dwords.d[1] = readl(otp_mem + 4);
	val.lock = readl(otp_mem + 8);

	if (val.lock & 1)
		return -EPERM;

	val.dwords.d[0] |= (new_val->dwords.d[0] & mask0);
	val.dwords.d[1] |= (new_val->dwords.d[1] & mask1);
	val.lock |= new_val->lock;

	writel(val.dwords.d[0], otp_mem);
	mdelay(1);
	writel(val.dwords.d[1], otp_mem + 4);
	mdelay(1);
	writel(val.lock, otp_mem + 8);
	mdelay(5);

	return 0;
}

static int prog_efuse(int nr, struct efuse_val *new_val, u32 mask0, u32 mask1)
{
	int res = 0;

	if (!new_val)
		return -EINVAL;

	/* only write a fuse line with lock bit */
	if (!new_val->lock)
		return -EINVAL;

	/* according to specs ECC protection bits must be 0 on write */
	if (new_val->bytes.d[7] & 0xFE)
		return -EINVAL;

	if (!new_val->dwords.d[0] && !new_val->dwords.d[1] && (mask0 | mask1))
		return 0;

	enable_efuse_program();

	res = do_prog_efuse(nr, new_val, mask0, mask1);

	disable_efuse_program();

	return res;
}

int mvebu_efuse_init_hw(unsigned long efuse_base)
{
	int ret;

	ret = mvebu_mbus_add_window_by_id(CPU_TARGET_SATA23_DFX,
					  0xA, efuse_base, MBUS_EFUSE_SIZE);

	if (ret)
		return ret;

	return 0;
}

int mvebu_read_efuse(int nr, struct efuse_val *val)
{
	void __iomem *otp_mem;

	otp_mem = efuse_data.target_otp_mem + EFUSE_ROW_REG_WIDTH * nr;

	val->dwords.d[0] = readl(otp_mem);
	val->dwords.d[1] = readl(otp_mem + 4);
	val->lock = readl(otp_mem + 8);

	return 0;
}

int mvebu_write_efuse(int nr, struct efuse_val *val)
{
	return prog_efuse(nr, val, ~0, ~0);
}

int mvebu_lock_efuse(int nr)
{
	struct efuse_val val = {
		.lock = 1,
	};

	return prog_efuse(nr, &val, 0, 0);
}

/*
 * wrapper funcs providing the fuse API
 *
 * we use the following mapping:
 *   "bank" ->	eFuse line
 *   "word" ->	0: bits 0-31
 *		1: bits 32-63
 *		2: bit 64 (lock)
 */

static struct efuse_val prog_val;
static int valid_prog_words;

int fuse_read(u32 bank, u32 word, u32 *val)
{
	struct efuse_val fuse_line;
	int res;

	if (bank < 0 || bank >= efuse_data.row_num ||
	    word > 2)
		return -EINVAL;

	res = mvebu_read_efuse(bank, &fuse_line);
	if (res)
		return res;

	if (word < 2)
		*val = fuse_line.dwords.d[word];
	else
		*val = fuse_line.lock;

	return res;
}

int fuse_sense(u32 bank, u32 word, u32 *val)
{
	/* not supported */
	return -ESRCH;
}

int fuse_prog(u32 bank, u32 word, u32 val)
{
	int res = 0;

	/*
	 * For now: remember values for word == 0 and word == 1 and write the
	 * whole line when word == 2.
	 * This implies that we always require all 3 fuse prog cmds (one for
	 * for each word) to write a single fuse line.
	 * Exception is a single write to word 2 which will lock the fuse line.
	 *
	 * Hope that will be OK.
	 */

	if (bank < 0 || bank >= efuse_data.row_num ||
	    word > 2)
		return -EINVAL;

	if (word < 2) {
		prog_val.dwords.d[word] = val;
		valid_prog_words |= (1 << word);
	} else if ((valid_prog_words & 3) == 0 && val) {
		res = mvebu_lock_efuse(bank);
		valid_prog_words = 0;
	} else if ((valid_prog_words & 3) != 3 || !val) {
		res = -EINVAL;
	} else {
		prog_val.lock = val != 0;
		res = mvebu_write_efuse(bank, &prog_val);
		valid_prog_words = 0;
	}
	return res;
}

int fuse_override(u32 bank, u32 word, u32 val)
{
	/* not supported */
	return -ESRCH;
}

static int fuse_probe(struct udevice *dev)
{
	const void *blob = gd->fdt_blob;
	int node = dev_of_offset(dev);
	struct mvebu_fuse_block_data *priv = dev_get_priv(dev);
	int res = 0;
	unsigned long efuse_base;

	priv->control_reg = dev_read_addr_ptr(dev);
	if (IS_ERR(priv->control_reg))
		return -EINVAL;
	efuse_data.control_reg = priv->control_reg;

#ifndef DRY_RUN
	priv->target_otp_mem =
		(void *)(((unsigned long)fdtdec_get_int(blob, node,
			"otp-mem", 0)) & 0xffffffff);

#else
	priv->target_otp_mem = NULL;
#endif
	efuse_data.target_otp_mem = priv->target_otp_mem;

	/* efuse mbus window initialization */
	efuse_base = (unsigned long)priv->target_otp_mem & 0xFF000000;
	res = mvebu_efuse_init_hw(efuse_base);
	if (res)
		return res;

	priv->row_num = fdtdec_get_int(blob, node, "row-count", 0);
	efuse_data.row_num = priv->row_num;

	return 0;
}

static const struct udevice_id efuse_ids[] = {
	{ .compatible = "marvell,a38x-efuse" },
	{ }
};

U_BOOT_DRIVER(mvebu_efuse) = {
	.name	= "mvebu_efuse",
	.id	= UCLASS_MISC,
	.of_match = efuse_ids,
	.probe	= fuse_probe,
	.priv_auto_alloc_size = sizeof(struct mvebu_fuse_block_data),
};

