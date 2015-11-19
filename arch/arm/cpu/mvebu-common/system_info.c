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

#include <malloc.h>
#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/system_info.h>

struct sys_info *sys_info_in_ptr = (struct sys_info *)SYSTEM_INFO_ADDRESS;
#ifdef CONFIG_SPL_BUILD
/* skip index 0 because this index in dedicated to array system-info size*/
int sys_info_size = 1;

void set_info(enum sys_info_type field, unsigned int value)
{
	if (sys_info_size == 1)
		sys_info_in_ptr[ARRAY_SIZE].field_id = ARRAY_SIZE;
	sys_info_in_ptr[sys_info_size].field_id = field;
	sys_info_in_ptr[sys_info_size].value = value;
	sys_info_in_ptr[ARRAY_SIZE].value = ++sys_info_size;
}

#else /*for U-Boot */
DECLARE_GLOBAL_DATA_PTR;

int get_info(enum sys_info_type field)
{
	int i;
	for (i = 1; i < sys_info_in_ptr[ARRAY_SIZE].value; i++)
		if (gd->arch.local_sys_info[i].field_id == field)
			return gd->arch.local_sys_info[i].value;
	return -1;
}

void sys_info_init(void)
{
	int i;

	if (sys_info_in_ptr[ARRAY_SIZE].field_id != ARRAY_SIZE)
		sys_info_in_ptr[ARRAY_SIZE].value = 1;

	for (i = 1; i < sys_info_in_ptr[ARRAY_SIZE].value ; i++) {
		gd->arch.local_sys_info[i].field_id = sys_info_in_ptr[i].field_id;
		gd->arch.local_sys_info[i].value = sys_info_in_ptr[i].value;
	}
}
#endif
