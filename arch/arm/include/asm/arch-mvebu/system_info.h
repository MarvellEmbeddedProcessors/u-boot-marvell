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

#ifndef _SYSTEM_INFO_H_
#define _SYSTEM_INFO_H_

#define SYSTEM_INFO_ADDRESS	0x4000000

enum sys_info_type {
	ARRAY_SIZE,
	DRAM_CS0_SIZE,
	DRAM_CS1_SIZE,
	DRAM_CS2_SIZE,
	DRAM_CS3_SIZE,
	DRAM_BUS_WIDTH,
	DRAM_ECC,
	DRAM_CS0,
	DRAM_CS1,
	DRAM_CS2,
	DRAM_CS3,
	MAX_OPTION,
};

struct sys_info {
	enum sys_info_type field_id;
	unsigned int value;
};

int get_info(enum sys_info_type field);
void set_info(enum sys_info_type field, unsigned int value);
void sys_info_init(void);

#endif /* _SYSTEM_INFO_H_ */
