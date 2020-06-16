/* SPDX-License-Identifier:    GPL-2.0
 *
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#ifndef __SOC_H__
#define __SOC_H__

/* Product PARTNUM */
#define CN106XX	0xB9

/* Register defines */

#define otx_is_soc(soc)	(read_partnum() == (soc))
#define otx_is_board(model) (!strcmp(read_board_name(), model))
#define otx_is_platform(platform) (read_platform() == (platform))

typedef enum {
	PLATFORM_HW = 0,
	PLATFORM_EMULATOR = 1,
	PLATFORM_ASIM = 3,
} platform_t;

platform_t read_platform(void);
u8 read_partnum(void);
u8 read_partvar(void);
const char *read_board_name(void);

#endif /* __SOC_H */
