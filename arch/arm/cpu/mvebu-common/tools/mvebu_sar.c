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
