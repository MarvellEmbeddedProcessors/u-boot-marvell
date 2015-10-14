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

#ifndef _MBUS_H_
#define _MBUS_H_

#define MBUS_WIN_MAP_NUM_MAX	8

struct mbus_win {
	u32 target;
	uintptr_t base_addr;
	uintptr_t win_size;
	u32 attribute;
	u32 remapped;
	u32 enabled;
};

struct  mbus_win_map {
	struct mbus_win mbus_windows[MBUS_WIN_MAP_NUM_MAX];
	int mbus_win_num;
};

void dump_mbus(void);
int init_mbus(void);
int remap_mbus(phys_addr_t input, phys_addr_t output);
void mbus_win_map_build(struct mbus_win_map *win_map);

#endif /* _MBUS_H_ */

