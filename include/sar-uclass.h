/*
 * ***************************************************************************
 * Copyright (C) 2017 Marvell International Ltd.
 * ***************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************
 */

#include <common.h>
#include <libfdt.h>
#include <mvebu/mvebu_chip_sar.h>
#include <fdtdec.h>
#include <dm.h>
#include <dm/device-internal.h>

struct sar_ops {
	int (*sar_init_func)(struct udevice *dev);
	int (*sar_dump_func)(struct udevice *dev);
	int (*sar_value_get_func)(struct udevice *dev, enum mvebu_sar_opts sar,
			struct sar_val *val);
	int (*sar_bootsrc_get)(struct udevice *dev, u32 *idx);
};

struct dm_sar_pdata {
	void __iomem *sar_base;
	const char *sar_name;
};

int mvebu_sar_id_register(struct udevice *dev, u32 sar_id);
