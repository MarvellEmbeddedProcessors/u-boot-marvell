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
#include "cfg_eeprom.h"

int fdt_cfg_load(const char *selected_index);
void fdt_cfg_print_select(void);
int fdt_cfg_set_select(const char *selected_index);
int fdt_cfg_list(void);
int fdt_cfg_save(uint8_t *fdt_blob);

#endif
