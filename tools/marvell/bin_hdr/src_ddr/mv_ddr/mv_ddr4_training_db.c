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

/* DDR4 Training Database */

#include "ddr3_init.h"

/* list of allowed frequencies listed in order of enum hws_ddr_freq */
u32 freq_val[DDR_FREQ_LAST] = {
	130,	/* DDR_FREQ_LOW_FREQ */
	650,	/* DDR_FREQ_650 */
	666,	/* DDR_FREQ_667 */
	800,	/* DDR_FREQ_800 */
	933,	/* DDR_FREQ_933 */
	1066,	/* DDR_FREQ_1066 */
	900,	/* DDR_FREQ_900 */
	1000,	/* DDR_FREQ_1000 */
	1050,	/* DDR_FREQ_1050 */
	1200	/* DDR_FREQ_1200 */
};

/* non-dbi mode - table for cl values per frequency for each speed bin index */
struct cl_val_per_freq cas_latency_table[] = {
	/* TODO: review 1600 and 1866 speed grades */
	/* 130M	650M	667M	800M	933M	1067M	900	1000	1050	1200 */
	{ {10,	10,	10,	0,	0,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1600J */
	{ {10,	11,	11,	0,	0,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1600K */
	{ {10,	12,	12,	0,	0,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1600L */
	{ {10,	12,	12,	12,	0,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1866L */
	{ {10,	12,	12,	13,	0,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1866M */
	{ {10,	12,	12,	14,	0,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1866N */
	{ {10,	10,	10,	12,	14,	14,	14,	14,	14,	0} },	/* SPEED_BIN_DDR_2133N */
	{ {10,	9,	9,	12,	14,	15,	14,	15,	15,	0} },	/* SPEED_BIN_DDR_2133P */
	{ {10,	10,	10,	12,	14,	16,	14,	16,	16,	0} },	/* SPEED_BIN_DDR_2133R */
	{ {10,	10,	10,	12,	14,	16,	14,	16,	16,	18} },	/* SPEED_BIN_DDR_2400P */
	{ {10,	9,	9,	11,	13,	15,	13,	15,	15,	18} },	/* SPEED_BIN_DDR_2400R */
	{ {10,	9,	9,	11,	13,	15,	13,	15,	15,	17} },	/* SPEED_BIN_DDR_2400S */
	{ {10,	10,	10,	12,	14,	16,	14,	16,	16,	18} }	/* SPEED_BIN_DDR_2400U */
};

/* dbi mode - table for cl values per frequency for each speed bin index */
struct cl_val_per_freq cas_latency_table_dbi[] = {
	/* 130M	650M	667M	800M	933M	1067M	900	1000 */
	{ {0,	12,	12,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1600J */
	{ {0,	13,	13,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1600K */
	{ {0,	14,	14,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1600L */
	{ {0,	14,	14,	14,	0,	0,	14,	0} },	/* SPEED_BIN_DDR_1866L */
	{ {0,	14,	14,	15,	0,	0,	15,	0} },	/* SPEED_BIN_DDR_1866M */
	{ {0,	14,	14,	16,	0,	0,	16,	0} },	/* SPEED_BIN_DDR_1866N */
	{ {0,	12,	12,	14,	16,	17,	14,	17} },	/* SPEED_BIN_DDR_2133N */
	{ {0,	11,	11,	14,	16,	18,	14,	18} },	/* SPEED_BIN_DDR_2133P */
	{ {0,	12,	12,	14,	16,	19,	14,	19} },	/* SPEED_BIN_DDR_2133R */
	{ {0,	12,	12,	14,	16,	19,	14,	19} },	/* SPEED_BIN_DDR_2400P */
	{ {0,	11,	11,	13,	15,	18,	13,	18} },	/* SPEED_BIN_DDR_2400R */
	{ {0,	11,	11,	13,	15,	18,	13,	18} },	/* SPEED_BIN_DDR_2400S */
	{ {0,	12,	12,	14,	16,	19,	14,	19} }	/* SPEED_BIN_DDR_2400U */
};

/* table for cwl values per speed bin index */
struct cl_val_per_freq cas_write_latency_table[] = {
	/* 130M	650M	667M	800M	933M	1067M	900	1000	1050	1200 */
	{ {9,	9,	9,	0,	0,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1600J */
	{ {9,	9,	9,	0,	0,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1600K */
	{ {9,	9,	9,	0,	0,	0,	0,	0,	0,	0} },	/* SPEED_BIN_DDR_1600L */
	{ {9,	9,	9,	10,	0,	0,	10,	0,	0,	0} },	/* SPEED_BIN_DDR_1866L */
	{ {9,	9,	9,	10,	0,	0,	10,	0,	0,	0} },	/* SPEED_BIN_DDR_1866M */
	{ {9,	9,	9,	10,	0,	0,	10,	0,	0,	0} },	/* SPEED_BIN_DDR_1866N */
	{ {9,	9,	9,	9,	10,	11,	10,	11,	10,	11} },	/* SPEED_BIN_DDR_2133N */
	{ {9,	9,	9,	9,	10,	11,	10,	11,	10,	11} },	/* SPEED_BIN_DDR_2133P */
	{ {9,	9,	9,	10,	10,	11,	10,	11,	10,	11} },	/* SPEED_BIN_DDR_2133R */
	{ {9,	9,	9,	9,	10,	11,	10,	11,	10,	12} },	/* SPEED_BIN_DDR_2400P */
	{ {9,	9,	9,	9,	10,	11,	10,	11,	10,	12} },	/* SPEED_BIN_DDR_2400R */
	{ {9,	9,	9,	9,	10,	11,	10,	11,	10,	12} },	/* SPEED_BIN_DDR_2400S */
	{ {9,	9,	9,	9,	10,	11,	10,	11,	10,	12} }	/* SPEED_BIN_DDR_2400U */
};

/*
 * rfc values, ns
 * note: values per JEDEC speed bin 1866; TODO: check it
 */
u16 rfc_table[] = {
	0,	/* placholder */
	0,	/* placholder */
	160,	/* 2G */
	260,	/* 4G */
	350,	/* 8G */
	0,	/* TODO: placeholder for 16-Mbit die capacity */
	0,	/* TODO: placeholder for 32-Mbit die capacity*/
	0,	/* TODO: placeholder for 12-Mbit die capacity */
	0	/* TODO: placeholder for 24-Mbit die capacity */
};

u16 rtt_table[] = {
	0xffff,
	60,
	120,
	40,
	240,
	48,
	80,
	34
};

u8 twr_mask_table[] = {
	0xa,
	0xa,
	0xa,
	0xa,
	0xa,
	0xa,
	0xa,
	0xa,
	0xa,
	0xa,
	0x0,	/* 10 */
	0xa,
	0x1,	/* 12 */
	0xa,
	0x2,	/* 14 */
	0xa,
	0x3,	/* 16 */
	0xa,
	0x4,	/* 18 */
	0xa,
	0x5,	/* 20 */
	0xa,
	0xa,	/* 22 */
	0xa,
	0x6	/* 24 */
};

u8 cl_mask_table[] = {
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x1,	/* 10 */
	0x2,
	0x3,	/* 12 */
	0x4,
	0x5,	/* 14 */
	0x6,
	0x7,	/* 16 */
	0xd,
	0x8,	/* 18 */
	0x0,
	0x9,	/* 20 */
	0x0,
	0xa,	/* 22 */
	0x0,
	0xb	/* 24 */
};

u8 cwl_mask_table[] = {
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x1,	/* 10 */
	0x2,
	0x3,	/* 12 */
	0x0,
	0x4,	/* 14 */
	0x0,
	0x5,	/* 16 */
	0x0,
	0x6	/* 18 */
};

u32 speed_bin_table_t_rcd_t_rp[] = {
	12500,
	13750,
	15000,
	12850,
	13920,
	15000,
	13130,
	14060,
	15000,
	12500,
	13320,
	14160,
	15000
};

u32 speed_bin_table_t_rc[] = {
	47500,
	48750,
	50000,
	46850,
	47920,
	49000,
	46130,
	47060,
	48000,
	44500,
	45320,
	46160,
	47000
};

/* return speed bin value for selected index and element */
u32 speed_bin_table(u8 index, enum speed_bin_table_elements element)
{
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	u32 result = 0;

	switch (element) {
	case SPEED_BIN_TRCD:
	case SPEED_BIN_TRP:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TRCD_MIN];
		else
			result = speed_bin_table_t_rcd_t_rp[index];
		break;
	case SPEED_BIN_TRAS:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TRAS_MIN];
		else {
			if (index <= SPEED_BIN_DDR_1600L) result = 35000;
			else if (index <= SPEED_BIN_DDR_1866N) result = 34000;
			else if (index <= SPEED_BIN_DDR_2133R) result = 33000;
			else result = 32000;
		}
		break;
	case SPEED_BIN_TRC:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TRC_MIN];
		else
			result = speed_bin_table_t_rc[index];
		break;
	case SPEED_BIN_TRRD0_5K:
	case SPEED_BIN_TRRD1K:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TRRD_S_MIN];
		else {
			if (index <= SPEED_BIN_DDR_1600L) result = 5000;
			else if (index <= SPEED_BIN_DDR_1866N) result = 4200;
			else if (index <= SPEED_BIN_DDR_2133R) result = 3700;
			else result = 3300;
		}
	        break;
	case SPEED_BIN_TRRD2K:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TRRD_S_MIN];
		else {
			if (index <= SPEED_BIN_DDR_1600L) result = 6000;
			else result = 5300;
		}

		break;
	case SPEED_BIN_TRRDL0_5K:
	case SPEED_BIN_TRRDL1K:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TRRD_L_MIN];
		else {
			if (index <= SPEED_BIN_DDR_1600L) result = 6000;
			else if (index <= SPEED_BIN_DDR_2133R) result = 5300;
			else result = 4900;
		}
		break;
	case SPEED_BIN_TRRDL2K:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TRRD_L_MIN];
		else {
			if (index <= SPEED_BIN_DDR_1600L) result = 7500;
			else result = 6400;
		}
	        break;
	case SPEED_BIN_TPD:
		result = 5000;
		break;
	case SPEED_BIN_TFAW0_5K:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TFAW_MIN];
		else {
			if (index <= SPEED_BIN_DDR_1600L) result = 20000;
			else if (index <= SPEED_BIN_DDR_1866N) result = 17000;
			else if (index <= SPEED_BIN_DDR_2133R) result = 15000;
			else result = 13000;
		}
	        break;
	case SPEED_BIN_TFAW1K:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TFAW_MIN];
		else {
			if (index <= SPEED_BIN_DDR_1600L) result = 25000;
			else if (index <= SPEED_BIN_DDR_1866N) result = 23000;
			else result = 21000;
		}
	        break;
	case SPEED_BIN_TFAW2K:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TFAW_MIN];
		else {
			if (index <= SPEED_BIN_DDR_1600L) result = 35000;
			else result = 30000;
		}
		break;
	case SPEED_BIN_TWTR:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TWTR_S_MIN];
		else
			result = 2500;
		break;
	case SPEED_BIN_TWTRL:
	case SPEED_BIN_TRTP:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TWTR_L_MIN];
		else
			result = 7500;
		break;
	case SPEED_BIN_TWR:
	case SPEED_BIN_TMOD:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = tm->timing_data[MV_DDR_TWR_MIN];
		else
			result = 15000;
		break;
	case SPEED_BIN_TXPDLL:
		result = 24000;
		break;
	case SPEED_BIN_TXSDLL:
		if (tm->cfg_src == MV_DDR_CFG_SPD)
			result = 768;	/* in number of clocks */
		else {
			if (index <= SPEED_BIN_DDR_1866N)
				result = 597;	/* in number of clocks */
			else
				result = 768;	/* in number of clocks */
		}
		break;
	default:
		DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR,
				  ("speed_bin_table: error, element id not valid (%d)\n",
				   (int)element));
		break;
	}

	return result;
}
