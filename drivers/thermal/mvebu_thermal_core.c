/*
* ***************************************************************************
* Copyright (C) 2016 Marvell International Ltd.
* ***************************************************************************
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* Neither the name of Marvell nor the names of its contributors may be used
* to endorse or promote products derived from this software without specific
* prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************
*/

/* #define DEBUG */
#include <common.h>
#include <malloc.h>
#include <fdtdec.h>
#include <asm/io.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/thermal.h>

static struct thermal_unit_config __attribute__((section(".data")))*thermal_units;
static u32 __attribute__((section(".data")))thermal_count;

static struct thermal_unit_config *init_thermal_config(u32 thermal_id, const void *blob, s32 node)
{
	struct thermal_unit_config *thermal_cfg;
	enum fdt_compat_id compat = fdtdec_next_lookup(blob, node, COMPAT_MVEBU_THERMAL);

	if (thermal_id >= thermal_count) {
		error("Thermal unit %d exceeds max unit (%d)\n", thermal_id, thermal_count);
		return NULL;
	}

	thermal_cfg = &thermal_units[thermal_id];

	/* Read register base */
	thermal_cfg->regs_base = fdt_get_regs_offs(blob, node, "reg");
	if (thermal_cfg->regs_base == 0) {
		error("thermal #%d: base address is missing in device-tree\n", thermal_id);
		return NULL;
	}

	/* Register functions (according to compatible) */
	switch (compat) {
	case COMPAT_MVEBU_THERMAL_SENSOR:
		thermal_cfg->ptr_thermal_sensor_probe = mvebu_thermal_sensor_probe;
		thermal_cfg->ptr_thermal_sensor_read = mvebu_thermal_sensor_read;
		break;
	case COMPAT_MVEBU_THERMAL_EXT_SENSOR:
		thermal_cfg->ptr_thermal_sensor_probe = mvebu_thermal_ext_sensor_probe;
		thermal_cfg->ptr_thermal_sensor_read = mvebu_thermal_ext_sensor_read;
		break;
	default:
		error("thermal.%lx: compatible %d is not supported\n", (uintptr_t)thermal_cfg->regs_base, compat);
		return NULL;
	}

	thermal_cfg->compat = compat;

	/* Read temperature calculation parameters */
	thermal_cfg->tsen_gain = fdtdec_get_int(blob, node, "gain", 0);
	if (thermal_cfg->tsen_gain <= 0) {
		error("thermal%lx: gain is missing in device-tree\n", (uintptr_t)thermal_cfg->regs_base);
		return NULL;
	}
	thermal_cfg->tsen_offset = fdtdec_get_int(blob, node, "offset", 0);
	if (thermal_cfg->tsen_offset <= 0) {
		error("thermal%lx: offset is missing in device-tree\n", (uintptr_t)thermal_cfg->regs_base);
		return NULL;
	}
	thermal_cfg->tsen_divisor = fdtdec_get_int(blob, node, "divisor", 0);
	if (thermal_cfg->tsen_divisor <= 0) {
		error("thermal%lx: divisor is missing in device-tree\n", (uintptr_t)thermal_cfg->regs_base);
		return NULL;
	}

	/* Save thermal index for MultiCP devices (A8K) */
	thermal_cfg->thermal_unit_index = thermal_id;

	return thermal_cfg;
}

u32 mvebu_thermal_init(const void *blob)
{
	s32 node, thermal_list[THERMAL_MAX_UNIT];
	struct thermal_unit_config *thermal_cfg;
	u32 i;
	static s32 init_done;

	debug_enter();

	if (init_done) {
		debug("Thermal units are already initialized\n");
		goto thermal_init_ok;
	}
	thermal_count = fdtdec_find_aliases_for_id(blob, "thermal",
					COMPAT_MVEBU_THERMAL, thermal_list, THERMAL_MAX_UNIT);

	/*  Exit in case no thermal unit was set in Device tree */
	if (thermal_count <= 0) {
		debug("Thermal: no unit was set in Device tree\n");
		goto thermal_init_ok;
	}

	/* allocate thermal config structures */
	thermal_units = malloc(sizeof(struct thermal_unit_config) * thermal_count);
	if (!thermal_units) {
		error("Thermal: failed to allocate memory for %d units\n", thermal_count);
		return -1;
	}

	for (i = 0; i < thermal_count ; i++) {
		node = thermal_list[i];
		if (node <= 0)
			continue;

		/* Init Sensor data structure */
		thermal_cfg = init_thermal_config(i, blob, node);
		if (thermal_cfg == NULL) {
			error("Thermal #%d: failed to initialize thermal data structure\n", i);
			continue;
		}

		/* set flag to indicate that Thermal Sensor is not ready */
		thermal_cfg->tsen_ready = 0;

		/* Sensor init */
		if (thermal_cfg->ptr_thermal_sensor_probe(thermal_cfg)) {
			error("thermal.%lx: failed to initialize thermal info\n", (uintptr_t)thermal_cfg->regs_base);
			continue; /* initialization failed */
		}

		/* Thermal Sensor is ready to use */
		thermal_cfg->tsen_ready = 1;

		debug("thermal.%lx: Initialized\n", (uintptr_t)thermal_cfg->regs_base);
	}

	/* Initialization completed */
	init_done = 1;

thermal_init_ok:
	debug_exit();

	return 0;
}

s32 mvebu_thermal_read(u32 thermal_id, s32 *temp)
{
	struct thermal_unit_config *thermal_cfg;

	if (thermal_id >= thermal_count) {
		error("Thermal unit %d exceeds max unit (%d)\n", thermal_id, thermal_count);
		return -1;
	}

	if (temp == NULL) {
		error("NULL pointer for temperature read\n");
		return -1;
	}

	thermal_cfg = &thermal_units[thermal_id];

	if (thermal_cfg->ptr_thermal_sensor_read == 0 ||
	    thermal_cfg->tsen_ready == 0) {
		debug("Thermal unit #%d was not initialized\n", thermal_id);
		return -1;
	}

	*temp = thermal_cfg->ptr_thermal_sensor_read(thermal_cfg);
	return 0;
}

uintptr_t mvebu_get_thermal_reg_base(u32 thermal_id)
{
	if (thermal_id >= thermal_count) {
		error("Thermal unit %d exceeds max unit (%d)\n", thermal_id, thermal_count);
		return 0; /* return base as 0 */
	}

	return (uintptr_t)thermal_units[thermal_id].regs_base;
}

u32 mvebu_get_thermal_count(void)
{
	return thermal_count;
}
