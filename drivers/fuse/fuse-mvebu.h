/* SPDX-License-Identifier: GPL-2.0+
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#ifndef _MVEBU_EFUSE_H
#define _MVEBU_EFUSE_H

#include <common.h>

#if defined(CONFIG_MVEBU_EFUSE_FAKE)
#define DRY_RUN
#else
#undef DRY_RUN
#endif

#define MVEBU_EFUSE_SRV_CTRL_LD_SEL_USER	BIT(6)
#define MVEBU_EFUSE_CTRL_LD_SEC_EN_MASK		BIT(7)
#define MVEBU_EFUSE_CTRL_PROGRAM_ENABLE		BIT(31)

struct mvebu_fuse_platform_data {
	unsigned int row_bit_width;
	unsigned int row_step;
};

struct mvebu_fuse_block_data {
	struct mvebu_fuse_platform_data	*pdata;
	unsigned int	row_base;
	unsigned int	row_num;
	void	*control_reg;
	void	*target_otp_mem;
	char	block_name[64];
};

struct fuse_ops {
	int (*fuse_init)(struct udevice *dev);
	int (*fuse_hd_read)(struct udevice *dev, int row_id, u32 *val);
	int (*fuse_hd_prog)(struct udevice *dev, int word, int row_id,
			    u32 new_val);
};

int mvebu_efuse_init_hw(struct udevice *dev);
int mvebu_efuse_hd_read(struct udevice *dev, int row_id, u32 *val);
int mvebu_efuse_hd_prog(struct udevice *dev, int word, int row_id, u32 new_val);

#endif

