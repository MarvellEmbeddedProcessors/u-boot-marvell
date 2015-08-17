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

#define SAMPLE_AT_RESET_REG_0		(MVEBU_REGS_BASE + 0x7F0200)

#define SAR_DUAL_AP_MODE_OFFSET		0
#define SAR_DUAL_AP_MODE_MASK		(0x1 << SAR_DUAL_AP_MODE_OFFSET)
#define SAR_I2C_INIT_EN_OFFSET		1
#define SAR_I2C_INIT_EN_MASK		(0x1 << SAR_I2C_INIT_EN_OFFSET)
#define SAR_BOOT_SRC_OFFSET		2
#define SAR_BOOT_SRC_MASK		(0x3 << SAR_BOOT_SRC_OFFSET)
#define SAR_C2C_IHB_OFFSET		4
#define SAR_C2C_IHB_MASK		(0x1 << SAR_C2C_IHB_OFFSET)
#define SAR_EFUSE_BYPASS_OFFSET		7
#define SAR_EFUSE_BYPASS_MASK		(0x1 << SAR_EFUSE_BYPASS_OFFSET)
#define SAR_STAND_ALONE_AP_MODE_OFFSET	8
#define SAR_STAND_ALONE_AP_MODE_MASK	(0x1 << SAR_STAND_ALONE_AP_MODE_OFFSET)
#define SAR_SLAVE_AP_MODE_OFFSET	9
#define SAR_SLAVE_AP_MODE_MASK		(0x1 << SAR_SLAVE_AP_MODE_OFFSET)
#define SAR_AVS_MASTER_OFFSET		11
#define SAR_AVS_MASTER_MASK		(0x1 << SAR_AVS_MASTER_OFFSET)

#define SAMPLE_AT_RESET_REG_1		(MVEBU_REGS_BASE + 0x7F0204)

struct sar_info {
	char *name;
	u32 offset;
	u32 mask;
};

struct sar_info sar_0[] = {
	{"Dual AP mode       ", SAR_DUAL_AP_MODE_OFFSET, SAR_DUAL_AP_MODE_MASK },
	{"I2C init Enable    ", SAR_I2C_INIT_EN_OFFSET, SAR_I2C_INIT_EN_MASK },
	{"Boot Source        ", SAR_BOOT_SRC_OFFSET, SAR_BOOT_SRC_MASK },
	{"C2C IHB select     ", SAR_C2C_IHB_OFFSET, SAR_C2C_IHB_MASK },
	{"EFUSE ByPass       ", SAR_EFUSE_BYPASS_OFFSET, SAR_EFUSE_BYPASS_MASK },
	{"Stand Alone AP mode", SAR_STAND_ALONE_AP_MODE_OFFSET, SAR_STAND_ALONE_AP_MODE_MASK},
	{"Slave AP mode      ", SAR_SLAVE_AP_MODE_OFFSET, SAR_SLAVE_AP_MODE_MASK },
	{"AVS master         ", SAR_AVS_MASTER_OFFSET, SAR_AVS_MASTER_MASK },
	{"",			-1,			-1},
};

struct sar_info sar_1[] = {
	{"",			-1,			-1},
};


#endif	/* _SAR_H_ */
