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

#include <asm/arch/memory-map.h>

#define SAMPLE_AT_RESET_REG_0		(MVEBU_REGS_BASE + 0xE4200)
#define SAR_CPU_ENABLE_OFFSET		16
#define SAR_CPU_ENABLE_MASK		(0x1 << SAR_CPU_ENABLE_OFFSET)
#define SAR_BOOT_SOURCE_OFFSET		3
#define SAR_BOOT_SOURCE_MASK		(0x3F << SAR_BOOT_SOURCE_OFFSET)

#define SAMPLE_AT_RESET_REG_1		(MVEBU_REGS_BASE + 0xE4204)
#define SAR_CPU_DDR_FREQ_OFFSET		17
#define SAR_CPU_DDR_FREQ_MASK		(0x1F << SAR_CPU_DDR_FREQ_OFFSET)

struct sar_info {
	char *name;
	u32 offset;
	u32 mask;
};

struct sar_info sar_0[] = {
	{"CPU Enable ",		SAR_CPU_ENABLE_OFFSET,	SAR_CPU_ENABLE_MASK},
	{"Boot src   ",		SAR_BOOT_SOURCE_OFFSET,	SAR_BOOT_SOURCE_MASK},
	{"",			-1,			-1},
};

struct sar_info sar_1[] = {
	{"CPU DDR frq",	SAR_CPU_DDR_FREQ_OFFSET, SAR_CPU_DDR_FREQ_MASK},
	{"",			-1,			-1},
};

#endif	/* _SAR_H_ */
