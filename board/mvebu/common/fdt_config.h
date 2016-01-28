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
#ifndef _FDT_CONFIG_H_
#define _FDT_CONFIG_H_
#include "fdt_eeprom.h"

void fdt_cfg_read_eeprom(void);
int fdt_cfg_read_flash(const char *selected_index);
int fdt_cfg_on(void);
int fdt_cfg_off(void);
int fdt_select_set(const char *selected_index);
int fdt_select_list(void);

#endif
