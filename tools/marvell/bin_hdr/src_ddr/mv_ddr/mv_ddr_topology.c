/*******************************************************************************
Copyright (C) 2016 Marvell International Ltd.

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the three
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 2 of the License, or any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************************
Marvell GNU General Public License FreeRTOS Exception

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the Lesser
General Public License Version 2.1 plus the following FreeRTOS exception.
An independent module is a module which is not derived from or based on
FreeRTOS.
Clause 1:
Linking FreeRTOS statically or dynamically with other modules is making a
combined work based on FreeRTOS. Thus, the terms and conditions of the GNU
General Public License cover the whole combination.
As a special exception, the copyright holder of FreeRTOS gives you permission
to link FreeRTOS with independent modules that communicate with FreeRTOS solely
through the FreeRTOS API interface, regardless of the license terms of these
independent modules, and to copy and distribute the resulting combined work
under terms of your choice, provided that:
1. Every copy of the combined work is accompanied by a written statement that
details to the recipient the version of FreeRTOS used and an offer by yourself
to provide the FreeRTOS source code (including any modifications you may have
made) should the recipient request it.
2. The combined work is not itself an RTOS, scheduler, kernel or related
product.
3. The independent modules add significant and primary functionality to
FreeRTOS and do not merely extend the existing functionality already present in
FreeRTOS.
Clause 2:
FreeRTOS may not be used for any competitive or comparative purpose, including
the publication of any form of run time or compile time metric, without the
express permission of Real Time Engineers Ltd. (this is the norm within the
industry and is intended to ensure information accuracy).

********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

	* Neither the name of Marvell nor the names of its contributors may be
	  used to endorse or promote products derived from this software without
	  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "mv_ddr_topology.h"
#include "mv_ddr_common.h"
#include "mv_ddr_spd.h"
#include "ddr3_init.h"
#include "ddr_topology_def.h"
#include "ddr3_training_ip_db.h"
#include "ddr3_training_ip.h"

#ifdef MV_DDR_ATF
#include "dram_if.h"
struct dram_config *mv_ddr_dram_config_update(void)
{
	struct dram_config *dc = mv_ddr_dram_config_get();
	uint64_t size = mv_ddr_get_total_memory_size_in_bits();

	/* convert total memory size from bits to megabytes */
	size /= (8 * 1024 * 1024);

	/* set total memory size in megabytes in dram configuration */
	dc->iface[0].size_mbytes = size;

	/* set bus width in dram configuration */
	dc->iface[0].bus_width = mv_ddr_if_bus_width_get();

	return dc;
}
#endif /* MV_DDR_ATF */

unsigned int mv_ddr_cl_calc(unsigned int taa_min, unsigned int tclk)
{
	unsigned int cl = ceil_div(taa_min, tclk);

	return mv_ddr_spd_supported_cl_get(cl);

}

unsigned int mv_ddr_cwl_calc(unsigned int tclk)
{
	unsigned int cwl;

	if (tclk >= 1250)
		cwl = 9;
	else if (tclk >= 1071)
		cwl = 10;
	else if (tclk >= 938)
		cwl = 11;
	else if (tclk >= 833)
		cwl = 12;
	else
		cwl = 0;

	return cwl;
}

struct mv_ddr_topology_map *mv_ddr_topology_map_update(void)
{
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	unsigned int octets_per_if_num = ddr3_tip_dev_attr_get(0, MV_ATTR_OCTET_PER_INTERFACE);
	enum hws_speed_bin speed_bin_index;
	enum hws_ddr_freq freq = DDR_FREQ_LAST;
	unsigned int tclk;
	unsigned char val = 0;
	int i;

	if (tm->interface_params[0].memory_freq == DDR_FREQ_SAR)
		tm->interface_params[0].memory_freq = mv_ddr_init_freq_get();

	if (tm->cfg_src == MV_DDR_CFG_SPD) {
		/* check dram device type */
		val = mv_ddr_spd_dev_type_get(&tm->spd_data);
		if (val != MV_DDR_SPD_DEV_TYPE_DDR4) {
			printf("mv_ddr: unsupported dram device type found\n");
			return NULL;
		}

		/* update topology map with timing data */
		if (mv_ddr_spd_timing_calc(&tm->spd_data, tm->timing_data) > 0) {
			printf("mv_ddr: negative timing data found\n");
			return NULL;
		}

		/* update device width in topology map */
		tm->interface_params[0].bus_width = mv_ddr_spd_dev_width_get(&tm->spd_data);

		/* update die capacity in topology map */
		tm->interface_params[0].memory_size = mv_ddr_spd_die_capacity_get(&tm->spd_data);

		/* update bus bit mask in topology map */
		tm->bus_act_mask = mv_ddr_bus_bit_mask_get();

		/* update cs bit mask in topology map */
		val = mv_ddr_spd_cs_bit_mask_get(&tm->spd_data);
#ifdef CONFIG_APN806
		int rev_id = apn806_rev_id_get();
#endif
		for (i = 0; i < octets_per_if_num; i++) {
#ifdef CONFIG_APN806
			if (rev_id == APN806_REV_ID_A0)
				tm->interface_params[0].as_bus_params[i].cs_bitmask = 0x1;
			else
#endif
				tm->interface_params[0].as_bus_params[i].cs_bitmask = val;
		}

		/* check dram module type */
		val = mv_ddr_spd_module_type_get(&tm->spd_data);
		switch (val) {
		case MV_DDR_SPD_MODULE_TYPE_UDIMM:
		case MV_DDR_SPD_MODULE_TYPE_SO_DIMM:
		case MV_DDR_SPD_MODULE_TYPE_MINI_UDIMM:
		case MV_DDR_SPD_MODULE_TYPE_72BIT_SO_UDIMM:
		case MV_DDR_SPD_MODULE_TYPE_16BIT_SO_DIMM:
		case MV_DDR_SPD_MODULE_TYPE_32BIT_SO_DIMM:
			break;
		default:
			printf("mv_ddr: unsupported dram module type found\n");
			return NULL;
		}

		/* update mirror bit mask in topology map */
		val = mv_ddr_spd_mem_mirror_get(&tm->spd_data);
		for (i = 0; i < octets_per_if_num; i++) {
#ifdef CONFIG_APN806
			if (rev_id == APN806_REV_ID_A0)
				tm->interface_params[0].as_bus_params[i].mirror_enable_bitmask = 0;
			else
#endif
				tm->interface_params[0].as_bus_params[i].mirror_enable_bitmask = val << 1;
		}

		tclk = 1000000 / freq_val[tm->interface_params[0].memory_freq];
		/* update cas write latency (cwl) */
		val = mv_ddr_cwl_calc(tclk);
		if (val == 0) {
			printf("mv_ddr: unsupported cas write latency value found\n");
			return NULL;
		}
		tm->interface_params[0].cas_wl = val;

		/* update cas latency (cl) */
		mv_ddr_spd_supported_cls_calc(&tm->spd_data);
		val = mv_ddr_cl_calc(tm->timing_data[MV_DDR_TAA_MIN], tclk);
		if (val == 0) {
			printf("mv_ddr: unsupported cas latency value found\n");
			return NULL;
		}
		tm->interface_params[0].cas_l = val;
	} else if (tm->cfg_src == MV_DDR_CFG_DEFAULT) {
		/* set cas and cas-write latencies per speed bin, if they unset */
		speed_bin_index = tm->interface_params[0].speed_bin_index;
		freq = tm->interface_params[0].memory_freq;

		if (tm->interface_params[0].cas_l == 0)
			tm->interface_params[0].cas_l =
				cas_latency_table[speed_bin_index].cl_val[freq];

		if (tm->interface_params[0].cas_wl == 0)
			tm->interface_params[0].cas_wl =
				cas_write_latency_table[speed_bin_index].cl_val[freq];
#ifdef CONFIG_APN806
		int rev_id = apn806_rev_id_get();
		/*
		 * in case of a0 using 32 bit configuration with ecc
		 * change the configuration to 32 bit without ecc
		 * due to a0 bug in the patterns fifo using ecc.
		 */
		if (rev_id == APN806_REV_ID_A0) {
			if (tm->bus_act_mask == MV_DDR_32BIT_ECC_PUP8_BUS_MASK)
				tm->bus_act_mask = BUS_MASK_32BIT;
		}
#endif
	}

	return tm;
}

unsigned short mv_ddr_bus_bit_mask_get(void)
{
	unsigned short pri_and_ext_bus_width = 0x0;
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	unsigned int octets_per_if_num = ddr3_tip_dev_attr_get(0, MV_ATTR_OCTET_PER_INTERFACE);

	if (tm->cfg_src == MV_DDR_CFG_SPD) {
		enum mv_ddr_pri_bus_width pri_bus_width = mv_ddr_spd_pri_bus_width_get(&tm->spd_data);
		enum mv_ddr_bus_width_ext bus_width_ext = mv_ddr_spd_bus_width_ext_get(&tm->spd_data);

		switch (pri_bus_width) {
		case MV_DDR_PRI_BUS_WIDTH_16:
			pri_and_ext_bus_width = BUS_MASK_16BIT;
			break;
		case MV_DDR_PRI_BUS_WIDTH_32:
			pri_and_ext_bus_width = BUS_MASK_32BIT;
			break;
		case MV_DDR_PRI_BUS_WIDTH_64:
			pri_and_ext_bus_width = MV_DDR_64BIT_BUS_MASK;
			break;
		default:
			pri_and_ext_bus_width = 0x0;
		}

		if (bus_width_ext == MV_DDR_BUS_WIDTH_EXT_8)
			pri_and_ext_bus_width |= 1 << (octets_per_if_num - 1);
	}

	return pri_and_ext_bus_width;
}

unsigned int mv_ddr_if_bus_width_get(void)
{
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	unsigned int bus_width;

	switch (tm->bus_act_mask) {
	case BUS_MASK_16BIT:
	case BUS_MASK_16BIT_ECC:
	case BUS_MASK_16BIT_ECC_PUP3:
		bus_width = 16;
		break;
	case BUS_MASK_32BIT:
	case BUS_MASK_32BIT_ECC:
	case MV_DDR_32BIT_ECC_PUP8_BUS_MASK:
		bus_width = 32;
		break;
	case MV_DDR_64BIT_BUS_MASK:
	case MV_DDR_64BIT_ECC_PUP8_BUS_MASK:
		bus_width = 64;
		break;
	default:
		printf("mv_ddr: unsupported bus active mask parameter found\n");
		bus_width = 0;
	}

	return bus_width;
}
