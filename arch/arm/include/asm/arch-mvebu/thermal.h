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

#ifndef _THERMAL_H_
#define _THERMAL_H_

#include <dt-bindings/comphy/comphy_data.h>
#include <fdtdec.h>

#define THERMAL_MAX_UNIT 3

struct thermal_unit_config {
	/* thermal temperature parameters */
	s32 tsen_offset;
	s32 tsen_gain;
	s32 tsen_divisor;
	/* thermal data */
	s32 tsen_ready;
	void __iomem *regs_base;
	u32 thermal_unit_index;
	enum fdt_compat_id compat;
	/* thermal functionality */
	u32 (*ptr_thermal_sensor_probe)(struct thermal_unit_config *);
	s32 (*ptr_thermal_sensor_read)(struct thermal_unit_config *);
};

/* Thermal driver APIs */
u32 mvebu_thermal_init(const void *blob);
s32 mvebu_thermal_read(u32 thermal_id, s32 *temp);
uintptr_t mvebu_get_thermal_reg_base(u32 thermal_id);
u32 mvebu_get_thermal_count(void);

/* Thermal sensors APIs */
s32 mvebu_thermal_sensor_read(struct thermal_unit_config *thermal_config);
u32 mvebu_thermal_sensor_probe(struct thermal_unit_config *thermal_config);

/* External Thermal sensors APIs */
s32 mvebu_thermal_ext_sensor_read(struct thermal_unit_config *thermal_config);
u32 mvebu_thermal_ext_sensor_probe(struct thermal_unit_config *thermal_config);

#endif /* _THERMAL_H_ */

