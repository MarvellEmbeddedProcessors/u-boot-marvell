/* SPDX-License-Identifier: GPL-2.0+
 * Copyright (C) 2018 Marvell International Ltd.
 * Copyright (C) 2015-2016 Reinhard Pfau <reinhard.pfau@gdsys.cc>
 *
 * https://spdx.org/licenses
 */

#ifndef _MVEBU_EFUSE_H
#define _MVEBU_EFUSE_H

#include <common.h>

struct efuse_val {
	union {
		struct {
			u8 d[8];
		} bytes;
		struct {
			u16 d[4];
		} words;
		struct {
			u32 d[2];
		} dwords;
	};
	u32 lock;
};

int mvebu_efuse_init_hw(unsigned long efuse_base);

int mvebu_read_efuse(int nr, struct efuse_val *val);

int mvebu_write_efuse(int nr, struct efuse_val *val);

int mvebu_lock_efuse(int nr);

#endif
