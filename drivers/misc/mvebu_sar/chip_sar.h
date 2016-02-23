/*
 * ***************************************************************************
 * Copyright (C) 2015 Marvell International Ltd.
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
#include <asm/arch-mvebu/fdt.h>
#include <mvebu_chip_sar.h>

struct sar_chip_info {
	enum fdt_compat_id compat;
	int (*sar_init_func)(const void *blob, int node);
	int (*sar_dump_func)(void);
	int (*sar_value_get_func)(enum mvebu_sar_opts sar, struct sar_val *val);
	int (*sar_bootsrc_get)(u32 *idx);
};

int mvebu_sar_chip_register(enum fdt_compat_id compat, struct sar_chip_info *info, uintptr_t *chip_id);
int mvebu_sar_id_register(uintptr_t chip_id, u32 sar_id);

/* AP806 SAR functions. */
int ap806_sar_init(const void *blob, int node);

/* CP110 SAR functions. */
int cp110_sar_init(const void *blob, int node);
