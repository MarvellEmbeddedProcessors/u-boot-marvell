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

#include <common.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/arch/sar.h>

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
	{"Standalone AP      ", SAR1_STANDALONE_OFFSET, SAR1_STANDALONE_MASK},
	{"PLL0 Config        ", SAR1_PLL0_OFFSET, SAR1_PLL0_MASK },
	{"PLL1 Config        ", SAR1_PLL1_OFFSET, SAR1_PLL1_MASK },
	{"PLL2 Config        ", SAR1_PLL2_OFFSET, SAR1_PLL2_MASK },
	{"",			-1,			-1},
};

void mvebu_sar_dump_reg(void)
{
	u32 reg, val;
	struct sar_info *sar;

	printf("| SAR Name            | Mask       | Offset     | Value      |\n");
	reg = readl(SAMPLE_AT_RESET_REG_0);
	printf("Sample at reset register 0 [0x%08x]:\n", reg);
	sar = sar_0;
	while (sar->offset != -1) {
		val = (reg & sar->mask) >> sar->offset;
		printf("| %s | 0x%08x | 0x%08x | 0x%08x |\n",
		       sar->name, sar->offset, sar->mask, val);
		sar++;
	}

	reg = readl(SAMPLE_AT_RESET_REG_1);
	printf("Sample at reset register 1 [0x%08x]:\n", reg);
	sar = sar_1;
	while (sar->offset != -1) {
		val = (reg & sar->mask) >> sar->offset;
		printf("| %s | 0x%08x | 0x%08x | 0x%08x |\n",
		       sar->name, sar->offset, sar->mask, val);
		sar++;
	}
	printf("\n");
}
