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

struct tsen_info {
	void __iomem *regs_base;
	u32 tsen_offset;
	u32 tsen_gain;
	u32 tsen_ready;
	u32 tsen_divisor;
};

struct tsen_info __attribute__((section(".data")))tsen_config;
struct tsen_info __attribute__((section(".data")))*tsen = &tsen_config;

#define THERMAL_TIMEOUT		1200

#define THERMAL_SEN_CTRL_LSB				0x0
#define THERMAL_SEN_CTRL_LSB_STRT_OFFSET		0
#define THERMAL_SEN_CTRL_LSB_STRT_MASK			(0x1 << THERMAL_SEN_CTRL_LSB_STRT_OFFSET)
#define THERMAL_SEN_CTRL_LSB_RST_OFFSET			1
#define THERMAL_SEN_CTRL_LSB_RST_MASK			(0x1 << THERMAL_SEN_CTRL_LSB_RST_OFFSET)
#define THERMAL_SEN_CTRL_LSB_EN_OFFSET			2
#define THERMAL_SEN_CTRL_LSB_EN_MASK			(0x1 << THERMAL_SEN_CTRL_LSB_EN_OFFSET)

#define THERMAL_SEN_CTRL_STATS				0x8
#define THERMAL_SEN_CTRL_STATS_VALID_OFFSET		16
#define THERMAL_SEN_CTRL_STATS_VALID_MASK		(0x1 << THERMAL_SEN_CTRL_STATS_VALID_OFFSET)
#define THERMAL_SEN_CTRL_STATS_TEMP_OUT_OFFSET		0
#define THERMAL_SEN_CTRL_STATS_TEMP_OUT_MASK		(0x3FF << THERMAL_SEN_CTRL_STATS_TEMP_OUT_OFFSET)

#define THERMAL_SEN_OUTPUT_MSB				512
#define THERMAL_SEN_OUTPUT_COMP				1024

u32 mvebu_thermal_sensor_read(void)
{
	u32 reg;

	if (!tsen->tsen_ready)
		return 0;

	reg = readl(tsen->regs_base + THERMAL_SEN_CTRL_STATS);
	reg = ((reg & THERMAL_SEN_CTRL_STATS_TEMP_OUT_MASK) >> THERMAL_SEN_CTRL_STATS_TEMP_OUT_OFFSET);

	/* TSEN output format is signed as a 2s complement number
	   ranging from-512 to +511. when MSB is set, need to
	   calculate the complement number */
	if (reg >= THERMAL_SEN_OUTPUT_MSB)
		reg -= THERMAL_SEN_OUTPUT_COMP;

	return ((tsen->tsen_gain * reg) + tsen->tsen_offset) / tsen->tsen_divisor;
}

u32 mvebu_thermal_sensor_probe(void)
{
	const void *blob = gd->fdt_blob;
	u32 node, reg, timeout = 0;

	debug_enter();
	debug("Initializing thermal sensor V2 unit\n");
	/* set flag to indicate that TSEN is not ready */
	tsen->tsen_ready = 0;

	/* Get thermal sensor node from the FDT blob */
	node = fdt_node_offset_by_compatible(blob, -1, fdtdec_get_compatible(COMPAT_MVEBU_THERMAL_SENSOR_V2));
	if (node == -1) {
		debug("No thermal sensor node found in FDT blob\n");
		return -1;
	}
	/* Get the base address */
	tsen->regs_base = (void *)fdt_get_regs_offs(blob, node, "reg");
	if (tsen->regs_base == NULL) {
		debug("%s: missing reg field for thermal sensor node", __func__);
		return -1;
	}
	tsen->tsen_gain = fdtdec_get_int(blob, node, "gain", -1);
	if (tsen->tsen_gain == -1) {
		debug("%s: missing gain field for thermal sensor node", __func__);
		return -1;
	}
	tsen->tsen_offset = fdtdec_get_int(blob, node, "offset", -1);
	if (tsen->tsen_offset == -1) {
		debug("%s: missing offset field for thermal sensor node", __func__);
		return -1;
	}
	tsen->tsen_divisor = fdtdec_get_int(blob, node, "divisor", -1);
	if (tsen->tsen_divisor == -1) {
		debug("%s: divisor offset field for thermal sensor node", __func__);
		return -1;
	}

	/* Initialize thermal sensor hardware reset once */
	reg = readl(tsen->regs_base + THERMAL_SEN_CTRL_LSB);
	reg &= ~THERMAL_SEN_CTRL_LSB_RST_OFFSET; /* De-assert TSEN_RESET */
	reg |= THERMAL_SEN_CTRL_LSB_EN_MASK; /* Set TSEN_EN to 1 */
	reg |= THERMAL_SEN_CTRL_LSB_STRT_MASK; /* Set TSEN_START to 1 */
	writel(reg, tsen->regs_base + THERMAL_SEN_CTRL_LSB);

	reg = readl(tsen->regs_base + THERMAL_SEN_CTRL_STATS);
	while ((reg & THERMAL_SEN_CTRL_STATS_VALID_MASK) == 0 && timeout < THERMAL_TIMEOUT) {
		udelay(10);
		reg = readl(tsen->regs_base + THERMAL_SEN_CTRL_STATS);
		timeout++;
	}
	if ((reg & THERMAL_SEN_CTRL_STATS_VALID_MASK) == 0) {
		error("%s: thermal sensor is not ready\n", __func__);
		return -1;
	}

	/* TSEN is ready to use */
	tsen->tsen_ready = 1;

	debug("Done thermal sensor V2 initializing unit\n");
	debug_exit();

	return 0;
}
