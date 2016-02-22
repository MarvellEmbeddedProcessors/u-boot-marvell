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

#ifndef _SAR_H_
#define _SAR_H_

#include <asm/types.h>
/*
** List of boot source options.
** Return value for each of the options:
**  - SAR_CPU_FREQ: Frequency (Hz)
**  - SAR_DDR_FREQ: Frequency (Hz)
**  - SAR_AP_FABRIC_FREQ: Frequency (Hz)
**  - SAR_CP_FABRIC_FREQ: Frequency (Hz)
**  - SAR_BOOT_SRC: Boot source type (see mvebu_bootsrc_type)
*/
enum mvebu_sar_opts {
	SAR_CPU_FREQ = 0,
	SAR_DDR_FREQ,
	SAR_AP_FABRIC_FREQ,
	SAR_CP_FABRIC_FREQ,
	SAR_BOOT_SRC,
	SAR_MAX_IDX
};

enum mvebu_bootsrc_type {
	BOOTSRC_NAND,
	BOOTSRC_SPI,
	BOOTSRC_SD_EMMC,
	BOOTSRC_MAX_IDX
};


int mvebu_sar_init(const void *blob);
int mvebu_sar_value_get(enum mvebu_sar_opts opt, u32 *val);

#endif	/* _SAR_H_ */
