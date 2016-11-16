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

#include "mv_ddr_spd.h"

#define MV_DDR_SPD_DATA_MTB		125	/* medium timebase, ps */
#define MV_DDR_SPD_DATA_FTB		1	/* fine timebase, ps */
#define MV_DDR_SPD_MSB_OFFS		8	/* most significant byte offset, bits */

#define MV_DDR_SPD_SUPPORTED_CLS_NUM	30

static unsigned int mv_ddr_spd_supported_cls[MV_DDR_SPD_SUPPORTED_CLS_NUM];

int mv_ddr_spd_supported_cls_calc(union mv_ddr_spd_data *spd_data)
{
	unsigned int byte, bit, start_cl;

	start_cl = (spd_data->all_bytes[23] & 0x8) ? 23 : 7;

	for (byte = 20; byte < 23; byte++) {
		for (bit = 0; bit < 8; bit++) {
			if (spd_data->all_bytes[byte] & (1 << bit))
				mv_ddr_spd_supported_cls[(byte - 20) * 8 + bit] = start_cl + (byte - 20) * 8 + bit;
			else
				mv_ddr_spd_supported_cls[(byte - 20) * 8 + bit] = 0;
		}
	}

	for (byte = 23, bit = 0; bit < 6; bit++) {
		if (spd_data->all_bytes[byte] & (1 << bit))
			mv_ddr_spd_supported_cls[(byte - 20) * 8 + bit] = start_cl + (byte - 20) * 8 + bit;
		else
			mv_ddr_spd_supported_cls[(byte - 20) * 8 + bit] = 0;
	}

	return 0;
}

unsigned int mv_ddr_spd_supported_cl_get(unsigned int cl)
{
	unsigned int supported_cl;
	int i = 0;

	while (i < MV_DDR_SPD_SUPPORTED_CLS_NUM &&
		mv_ddr_spd_supported_cls[i] < cl)
		i++;

	if (i < MV_DDR_SPD_SUPPORTED_CLS_NUM)
		supported_cl = mv_ddr_spd_supported_cls[i];
	else
		supported_cl = 0;

	return supported_cl;
}

int mv_ddr_spd_timing_calc(union mv_ddr_spd_data *spd_data, unsigned int timing_data[])
{
	int calc_val;

	/* t ck avg min, ps */
	calc_val = spd_data->byte_fields.byte_18 * MV_DDR_SPD_DATA_MTB +
		(signed char)spd_data->byte_fields.byte_125 * MV_DDR_SPD_DATA_FTB;
	if (calc_val < 0)
		return 1;
	timing_data[MV_DDR_TCK_AVG_MIN] = calc_val;

	/* t aa min, ps */
	calc_val = spd_data->byte_fields.byte_24 * MV_DDR_SPD_DATA_MTB +
		(signed char)spd_data->byte_fields.byte_123 * MV_DDR_SPD_DATA_FTB;
	if (calc_val < 0)
		return 1;
	timing_data[MV_DDR_TAA_MIN] = calc_val;

	/* t rfc1 min, ps */
	timing_data[MV_DDR_TRFC1_MIN] = (spd_data->byte_fields.byte_30 +
		(spd_data->byte_fields.byte_31 << MV_DDR_SPD_MSB_OFFS)) * MV_DDR_SPD_DATA_MTB;

	/* t wr min, ps */
	timing_data[MV_DDR_TWR_MIN] = (spd_data->byte_fields.byte_42 +
		(spd_data->byte_fields.byte_41.bit_fields.t_wr_min_msn << MV_DDR_SPD_MSB_OFFS)) *
		MV_DDR_SPD_DATA_MTB;
	/* FIXME: wa: set twr to a default value, if it's unset on spd */
	if (timing_data[MV_DDR_TWR_MIN] == 0)
		timing_data[MV_DDR_TWR_MIN] = 15000;

	/* t rcd min, ps */
	calc_val = spd_data->byte_fields.byte_25 * MV_DDR_SPD_DATA_MTB +
		(signed char)spd_data->byte_fields.byte_122 * MV_DDR_SPD_DATA_FTB;
	if (calc_val < 0)
		return 1;
	timing_data[MV_DDR_TRCD_MIN] = calc_val;

	/* t rp min, ps */
	calc_val = spd_data->byte_fields.byte_26 * MV_DDR_SPD_DATA_MTB +
		(signed char)spd_data->byte_fields.byte_121 * MV_DDR_SPD_DATA_FTB;
	if (calc_val < 0)
		return 1;
	timing_data[MV_DDR_TRP_MIN] = calc_val;

	/* t rc min, ps */
	calc_val = (spd_data->byte_fields.byte_29 +
		(spd_data->byte_fields.byte_27.bit_fields.t_rc_min_msn << MV_DDR_SPD_MSB_OFFS)) *
		MV_DDR_SPD_DATA_MTB +
		(signed char)spd_data->byte_fields.byte_120 * MV_DDR_SPD_DATA_FTB;
	if (calc_val < 0)
		return 1;
	timing_data[MV_DDR_TRC_MIN] = calc_val;

	/* t ras min, ps */
	timing_data[MV_DDR_TRAS_MIN] = (spd_data->byte_fields.byte_28 +
		(spd_data->byte_fields.byte_27.bit_fields.t_ras_min_msn << MV_DDR_SPD_MSB_OFFS)) *
		MV_DDR_SPD_DATA_MTB;

	/* t rrd s min, ps */
	calc_val = spd_data->byte_fields.byte_38 * MV_DDR_SPD_DATA_MTB +
		(signed char)spd_data->byte_fields.byte_119 * MV_DDR_SPD_DATA_FTB;
	if (calc_val < 0)
		return 1;
	timing_data[MV_DDR_TRRD_S_MIN] = calc_val;

	/* t rrd l min, ps */
	calc_val = spd_data->byte_fields.byte_39 * MV_DDR_SPD_DATA_MTB +
		(signed char)spd_data->byte_fields.byte_118 * MV_DDR_SPD_DATA_FTB;
	if (calc_val < 0)
		return 1;
	timing_data[MV_DDR_TRRD_L_MIN] = calc_val;

	/* t faw min, ps */
	timing_data[MV_DDR_TFAW_MIN] = (spd_data->byte_fields.byte_37 +
		(spd_data->byte_fields.byte_36.bit_fields.t_faw_min_msn << MV_DDR_SPD_MSB_OFFS)) *
		MV_DDR_SPD_DATA_MTB;

	/* t wtr s min, ps */
	timing_data[MV_DDR_TWTR_S_MIN] = (spd_data->byte_fields.byte_44 +
		(spd_data->byte_fields.byte_43.bit_fields.t_wtr_s_min_msn << MV_DDR_SPD_MSB_OFFS)) *
		MV_DDR_SPD_DATA_MTB;
	/* FIXME: wa: set twtr_s to a default value, if it's unset on spd */
	if (timing_data[MV_DDR_TWTR_S_MIN] == 0)
		timing_data[MV_DDR_TWTR_S_MIN] = 2500;

	/* t wtr l min, ps */
	timing_data[MV_DDR_TWTR_L_MIN] = (spd_data->byte_fields.byte_45 +
		(spd_data->byte_fields.byte_43.bit_fields.t_wtr_l_min_msn << MV_DDR_SPD_MSB_OFFS)) *
		MV_DDR_SPD_DATA_MTB;
	/* FIXME: wa: set twtr_l to a default value, if it's unset on spd */
	if (timing_data[MV_DDR_TWTR_L_MIN] == 0)
		timing_data[MV_DDR_TWTR_L_MIN] = 7500;

	return 0;
}

enum mv_ddr_dev_width mv_ddr_spd_dev_width_get(union mv_ddr_spd_data *spd_data)
{
	unsigned char dev_width = spd_data->byte_fields.byte_12.bit_fields.device_width;
	enum mv_ddr_dev_width ret_val;

	switch (dev_width) {
	case 0x00:
		ret_val = MV_DDR_DEV_WIDTH_4BIT;
		break;
	case 0x01:
		ret_val = MV_DDR_DEV_WIDTH_8BIT;
		break;
	case 0x02:
		ret_val = MV_DDR_DEV_WIDTH_16BIT;
		break;
	case 0x03:
		ret_val = MV_DDR_DEV_WIDTH_32BIT;
		break;
	default:
		ret_val = MV_DDR_DEV_WIDTH_LAST;
	}

	return ret_val;
}

enum mv_ddr_die_capacity mv_ddr_spd_die_capacity_get(union mv_ddr_spd_data *spd_data)
{
	unsigned char die_cap = spd_data->byte_fields.byte_4.bit_fields.die_capacity;
	enum mv_ddr_die_capacity ret_val;

	switch (die_cap) {
	case 0x00:
		ret_val = MV_DDR_DIE_CAP_256MBIT;
		break;
	case 0x01:
		ret_val = MV_DDR_DIE_CAP_512MBIT;
		break;
	case 0x02:
		ret_val = MV_DDR_DIE_CAP_1GBIT;
		break;
	case 0x03:
		ret_val = MV_DDR_DIE_CAP_2GBIT;
		break;
	case 0x04:
		ret_val = MV_DDR_DIE_CAP_4GBIT;
		break;
	case 0x05:
		ret_val = MV_DDR_DIE_CAP_8GBIT;
		break;
	case 0x06:
		ret_val = MV_DDR_DIE_CAP_16GBIT;
		break;
	case 0x07:
		ret_val = MV_DDR_DIE_CAP_32GBIT;
		break;
	case 0x08:
		ret_val = MV_DDR_DIE_CAP_12GBIT;
		break;
	case 0x09:
		ret_val = MV_DDR_DIE_CAP_24GBIT;
		break;
	default:
		ret_val = MV_DDR_DIE_CAP_LAST;
	}

	return ret_val;
}

unsigned char mv_ddr_spd_mem_mirror_get(union mv_ddr_spd_data *spd_data)
{
	unsigned char mem_mirror = spd_data->byte_fields.byte_131.bit_fields.rank_1_mapping;

	return mem_mirror;
}

enum mv_ddr_pkg_rank mv_ddr_spd_pri_bus_width_get(union mv_ddr_spd_data *spd_data)
{
	unsigned char pri_bus_width = spd_data->byte_fields.byte_13.bit_fields.primary_bus_width;
	enum mv_ddr_pri_bus_width ret_val;

	switch (pri_bus_width) {
	case 0x00:
		ret_val = MV_DDR_PRI_BUS_WIDTH_8;
		break;
	case 0x01:
		ret_val = MV_DDR_PRI_BUS_WIDTH_16;
		break;
	case 0x02:
		ret_val = MV_DDR_PRI_BUS_WIDTH_32;
		break;
	case 0x03:
		ret_val = MV_DDR_PRI_BUS_WIDTH_64;
		break;
	default:
		ret_val = MV_DDR_PRI_BUS_WIDTH_LAST;
	}

	return ret_val;
}

enum mv_ddr_pkg_rank mv_ddr_spd_bus_width_ext_get(union mv_ddr_spd_data *spd_data)
{
	unsigned char bus_width_ext = spd_data->byte_fields.byte_13.bit_fields.bus_width_ext;
	enum mv_ddr_bus_width_ext ret_val;

	switch (bus_width_ext) {
	case 0x00:
		ret_val = MV_DDR_BUS_WIDTH_EXT_0;
		break;
	case 0x01:
		ret_val = MV_DDR_BUS_WIDTH_EXT_8;
		break;
	default:
		ret_val = MV_DDR_BUS_WIDTH_EXT_LAST;
	}

	return ret_val;
}

static enum mv_ddr_pkg_rank mv_ddr_spd_pkg_rank_get(union mv_ddr_spd_data *spd_data)
{
	unsigned char pkg_rank = spd_data->byte_fields.byte_12.bit_fields.dimm_pkg_ranks_num;
	enum mv_ddr_pkg_rank ret_val;

	switch (pkg_rank) {
	case 0x00:
		ret_val = MV_DDR_PKG_RANK_1;
		break;
	case 0x01:
		ret_val = MV_DDR_PKG_RANK_2;
		break;
	case 0x02:
		ret_val = MV_DDR_PKG_RANK_3;
		break;
	case 0x03:
		ret_val = MV_DDR_PKG_RANK_4;
		break;
	case 0x04:
		ret_val = MV_DDR_PKG_RANK_5;
		break;
	case 0x05:
		ret_val = MV_DDR_PKG_RANK_6;
		break;
	case 0x06:
		ret_val = MV_DDR_PKG_RANK_7;
		break;
	case 0x07:
		ret_val = MV_DDR_PKG_RANK_8;
		break;
	default:
		ret_val = MV_DDR_PKG_RANK_LAST;
	}

	return ret_val;
}

static enum mv_ddr_die_count mv_ddr_spd_die_count_get(union mv_ddr_spd_data *spd_data)
{
	unsigned char die_count = spd_data->byte_fields.byte_6.bit_fields.die_count;
	enum mv_ddr_die_count ret_val;

	switch (die_count) {
	case 0x00:
		ret_val = MV_DDR_DIE_CNT_1;
		break;
	case 0x01:
		ret_val = MV_DDR_DIE_CNT_2;
		break;
	case 0x02:
		ret_val = MV_DDR_DIE_CNT_3;
		break;
	case 0x03:
		ret_val = MV_DDR_DIE_CNT_4;
		break;
	case 0x04:
		ret_val = MV_DDR_DIE_CNT_5;
		break;
	case 0x05:
		ret_val = MV_DDR_DIE_CNT_6;
		break;
	case 0x06:
		ret_val = MV_DDR_DIE_CNT_7;
		break;
	case 0x07:
		ret_val = MV_DDR_DIE_CNT_8;
		break;
	default:
		ret_val = MV_DDR_DIE_CNT_LAST;
	}

	return ret_val;
}

unsigned char mv_ddr_spd_cs_bit_mask_get(union mv_ddr_spd_data *spd_data)
{
	unsigned char cs_bit_mask = 0x0;
	enum mv_ddr_pkg_rank pkg_rank = mv_ddr_spd_pkg_rank_get(spd_data);
	enum mv_ddr_die_count die_cnt = mv_ddr_spd_die_count_get(spd_data);

	if (pkg_rank == MV_DDR_PKG_RANK_1 && die_cnt == MV_DDR_DIE_CNT_1)
		cs_bit_mask = 0x1;
	else if (pkg_rank == MV_DDR_PKG_RANK_1 && die_cnt == MV_DDR_DIE_CNT_2)
		cs_bit_mask = 0x3;
	else if (pkg_rank == MV_DDR_PKG_RANK_2 && die_cnt == MV_DDR_DIE_CNT_1)
		cs_bit_mask = 0x3;
	else if (pkg_rank == MV_DDR_PKG_RANK_2 && die_cnt == MV_DDR_DIE_CNT_2)
		cs_bit_mask = 0xf;

	return cs_bit_mask;
}

unsigned char mv_ddr_spd_dev_type_get(union mv_ddr_spd_data *spd_data)
{
	unsigned char dev_type = spd_data->byte_fields.byte_2;

	return dev_type;
}

unsigned char mv_ddr_spd_module_type_get(union mv_ddr_spd_data *spd_data)
{
	unsigned char module_type = spd_data->byte_fields.byte_3.bit_fields.module_type;

	return module_type;
}
