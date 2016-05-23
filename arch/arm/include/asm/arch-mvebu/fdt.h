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

#ifndef _MVEBU_FDT_H_
#define _MVEBU_FDT_H_
#include <fdtdec.h>

struct fdt_range {
	u32 child_bus_address;
	u32 parent_bus_address;
	u32 size;
};

int setup_fdt(void);

#ifdef CONFIG_MULTI_DT_FILE
int mvebu_setup_fdt(void);
#endif

#ifdef CONFIG_BOARD_CONFIG_EEPROM
int cfg_eeprom_init(void);
int cfg_eeprom_finish(void);
uint8_t *cfg_eeprom_get_fdt(void);
#endif

void *fdt_get_regs_offs(const void *blob, int node, const char *prop_name);
void *fdt_get_regs_base(const void *blob, int node, uintptr_t reg);
void *fdt_get_reg_offs_by_compat(enum fdt_compat_id compat_id);

#endif /* _MVEBU_FDT_H_ */

