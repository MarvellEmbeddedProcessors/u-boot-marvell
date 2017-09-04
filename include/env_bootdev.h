/*
 * Copyright (C) 2017 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
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

#endif /* _ENV_BOOTDEV_H_ */
