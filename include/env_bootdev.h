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

#ifndef _ENV_BOOTDEV_H_
#define _ENV_BOOTDEV_H_


typedef int (*init_env_func)(void);
typedef int (*save_env_func)(void);
typedef void (*relocate_spec_func)(void);

struct env_func_ptr {
	init_env_func init_env;
	save_env_func save_env;
	relocate_spec_func reloc_env;
};

void sf_env_init(void);
void nand_env_init(void);
void mmc_boot_env_init(void);

#endif /* _ENV_BOOTDEV_H_ */
