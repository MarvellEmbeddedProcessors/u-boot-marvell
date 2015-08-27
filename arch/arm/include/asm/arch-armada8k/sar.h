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

#define SAMPLE_AT_RESET_REG_0		(MVEBU_REGS_BASE + 0x6F8200)

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

#define SAMPLE_AT_RESET_REG_1		(MVEBU_REGS_BASE + 0x6F8204)

#define SAR1_PLL2_OFFSET		(15)
#define SAR1_PLL2_MASK			(0x7 << SAR1_PLL2_OFFSET)
#define SAR1_PLL1_OFFSET		(18)
#define SAR1_PLL1_MASK			(0x7 << SAR1_PLL1_OFFSET)
#define SAR1_PLL0_OFFSET		(21)
#define SAR1_PLL0_MASK			(0x7 << SAR1_PLL0_OFFSET)
#define SAR1_STANDALONE_OFFSET		(24)
#define SAR1_STANDALONE_MASK		(1 << SAR1_STANDALONE_OFFSET)


struct sar_info {
	char *name;
	u32 offset;
	u32 mask;
};

#endif	/* _SAR_H_ */
