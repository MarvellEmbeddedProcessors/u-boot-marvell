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
#include <asm/io.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/thermal.h>

DECLARE_GLOBAL_DATA_PTR;

void __iomem __attribute__((section(".data")))*thermal_base;
u32 __attribute__((section(".data")))tsen_ready;

#define THERMAL_SEN_CTRL_MSB				0x4
#define THERMAL_SEN_CTRL_MSB_RST_OFFSET			8
#define THERMAL_SEN_CTRL_MSB_RST_MASK			(0x1 << THERMAL_SEN_CTRL_MSB_RST_OFFSET)

#define THERMAL_SEN_CTRL_STATS				0x8
#define THERMAL_SEN_CTRL_STATS_VALID_OFFSET		10
#define THERMAL_SEN_CTRL_STATS_VALID_MASK		(0x1 << THERMAL_SEN_CTRL_STATS_VALID_OFFSET)
#define THERMAL_SEN_CTRL_STATS_TEMP_OUT_OFFSET		0
#define THERMAL_SEN_CTRL_STATS_TEMP_OUT_MASK		(0x3FF << THERMAL_SEN_CTRL_STATS_TEMP_OUT_OFFSET)

u32 mvebu_thermal_sensor_read(void)
{
	u32 reg;

	if (!tsen_ready)
		return 0;

	reg = readl(thermal_base + THERMAL_SEN_CTRL_STATS);
	reg = ((reg & THERMAL_SEN_CTRL_STATS_TEMP_OUT_MASK) >> THERMAL_SEN_CTRL_STATS_TEMP_OUT_OFFSET);

	return ((4761 * reg) - 2791000) / 10000;
}

u32 mvebu_thermal_sensor_probe(void)
{
	const void *blob = gd->fdt_blob;
	u32 node, reg, timeout = 0;

	debug_enter();
	debug("Initializing thermal sensor unit\n");
	/* flag to indicate that TSEN is not ready */
	tsen_ready = 0;

	/* Get thermal sensor node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_THERMAL_SENSOR));
	if (node == -1) {
		debug("No thermal sensor node found in FDT blob\n");
		return -1;
	}
	/* Get the base address */
	thermal_base = (void *)fdt_get_regs_offs(blob, node, "reg");

	/* Initialize thermal sensor hardware reset once */
	reg = readl(thermal_base + THERMAL_SEN_CTRL_MSB);
	reg |= THERMAL_SEN_CTRL_MSB_RST_MASK;
	writel(reg, thermal_base + THERMAL_SEN_CTRL_MSB);

	reg = readl(thermal_base + THERMAL_SEN_CTRL_STATS);
	while ((reg & THERMAL_SEN_CTRL_STATS_VALID_MASK) == 0 && timeout < 300) {
		udelay(1);
		reg = readl(thermal_base + THERMAL_SEN_CTRL_STATS);
		timeout++;
	}

	if ((reg & THERMAL_SEN_CTRL_STATS_VALID_MASK) == 0) {
		error("%s: thermal sensor is not ready\n", __func__);
		return -1;
	}

	/* TSEN is ready to use */
	tsen_ready = 1;


	debug("Done thermal sensor initializing unit\n");
	debug_exit();

	return 0;
}
