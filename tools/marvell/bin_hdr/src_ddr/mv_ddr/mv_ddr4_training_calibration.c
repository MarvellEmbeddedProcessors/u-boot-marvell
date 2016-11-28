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

#if defined(CONFIG_DDR4)

/* DESCRIPTION: DDR4 Receiver and DQVref Calibration */

#include "ddr3_init.h"
#include "mv_ddr4_training_calibration.h"
#include "mv_ddr4_training.h"
#include "mv_ddr4_mpr_pda_if.h"

#define RX_DIR			0
#define TX_DIR			1
#define MAX_DIR_TYPES		2

#define RECEIVER_DC_STEP_SIZE	3
#define RECEIVER_DC_MIN_RANGE	0
#define RECEIVER_DC_MAX_RANGE	63
#define RECEIVER_DC_MAX_COUNT	(((RECEIVER_DC_MAX_RANGE - RECEIVER_DC_MIN_RANGE) / RECEIVER_DC_STEP_SIZE) + 1)

#define PBS_VAL_FACTOR		1000
#define MV_DDR_VW_TX_NOISE_FILTER	8	/* adlls */

static u8 pbs_max = 31;
static u8 vdq_tv; /* vref value for dq vref calibration */
static u8 duty_cycle; /* duty cycle value for receiver calibration */
static u8 rx_vw_pos[MAX_INTERFACE_NUM][MAX_BUS_NUM];
static u8 patterns_byte_status[MAX_INTERFACE_NUM][MAX_BUS_NUM];
static const char *str_dir[MAX_DIR_TYPES] = {"read", "write"};

static u8 center_low_element_get(u8 dir, u8 pbs_element, u16 lambda, u8 pbs_max_val)
{
	u8 result;

	if (dir == RX_DIR)
		result = pbs_element * lambda / PBS_VAL_FACTOR;
	else
		result = (pbs_max_val - pbs_element) * lambda / PBS_VAL_FACTOR;

	return result;
}

static u8 center_high_element_get(u8 dir, u8 pbs_element, u16 lambda, u8 pbs_max_val)
{
	u8 result;

	if (dir == RX_DIR)
		result = (pbs_max_val - pbs_element) * lambda / PBS_VAL_FACTOR;
	else
		result = pbs_element * lambda / PBS_VAL_FACTOR;

	return result;
}

static int mv_ddr4_centralization(u8 dev_num, u16 (*lambda)[MAX_BUS_NUM][BUS_WIDTH_IN_BITS], u8 (*copt)[MAX_BUS_NUM],
				  u8 (*pbs_result)[MAX_BUS_NUM][BUS_WIDTH_IN_BITS], u8 (*vw_size)[MAX_BUS_NUM],
				  u8 mode, u16 param0, u8 param1);
static int mv_ddr4_dqs_reposition(u8 dir, u16 *lambda, u8 *pbs_result, char delta, u8 *copt, u8 *dqs_pbs);
static int mv_ddr4_copt_get(u8 dir, u16 *lambda, u8 *vw_l, u8 *vw_h, u8 *pbs_result, u8 *copt);
static int mv_ddr4_center_of_mass_calc(u8 dev_num, u8 if_id, u8 subphy_num, u8 mode, u8 *vw_l, u8 *vw_h, u8 *vw_v,
				       u8 vw_num, u8 *v_opt, u8 *t_opt);
static int mv_ddr4_tap_tuning(u8 dev_num, u16 (*pbs_tap_factor)[MAX_BUS_NUM][BUS_WIDTH_IN_BITS], u8 mode);

/* dq vref calibration flow */
int mv_ddr4_dq_vref_calibration(u8 dev_num)
{
	u32 if_id, subphy_num;
	u32 vref_idx, dq_idx, pad_num = 0;
	u8 dq_vref_start_win[MAX_INTERFACE_NUM][MAX_BUS_NUM][MV_DDR4_VREF_MAX_COUNT];
	u8 dq_vref_end_win[MAX_INTERFACE_NUM][MAX_BUS_NUM][MV_DDR4_VREF_MAX_COUNT];
	u8 valid_win_size[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 c_opt_per_bus[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 valid_vref_cnt[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 valid_vref_ptr[MAX_INTERFACE_NUM][MAX_BUS_NUM][MV_DDR4_VREF_MAX_COUNT];
	u8 center_adll[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 center_vref[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 pbs_res_per_bus[MAX_INTERFACE_NUM][MAX_BUS_NUM][BUS_WIDTH_IN_BITS];
	u16 lambda_per_dq[MAX_INTERFACE_NUM][MAX_BUS_NUM][BUS_WIDTH_IN_BITS];
	u16 vref_avg, vref_subphy_num;
	int vref_tap_idx;
	int vref_range_min;
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	enum mv_ddr4_vref_subphy_cal_state all_subphys_state = MV_DDR4_VREF_SUBPHY_CAL_ABOVE;
	int tap_tune_passed = MV_FALSE;
	enum mv_ddr4_vref_tap_state vref_tap_set_state = MV_DDR4_VREF_TAP_START;
	enum hws_result *flow_result = ddr3_tip_get_result_ptr(training_stage);
	u8 subphy_max = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	enum mv_ddr4_vref_subphy_cal_state vref_state_per_subphy[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	int status;
	static u8 vref_byte_status[MAX_INTERFACE_NUM][MAX_BUS_NUM][MV_DDR4_VREF_MAX_RANGE];

	DEBUG_CALIBRATION(DEBUG_LEVEL_INFO, ("Starting ddr4 dq vref calibration training stage\n"));

	vdq_tv = 0;
	duty_cycle = 0;

	/* reset valid vref counter per if and subphy */
	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		for (subphy_num = 0; subphy_num < MAX_BUS_NUM; subphy_num++) {
			valid_vref_cnt[if_id][subphy_num] = 0;
			vref_state_per_subphy[if_id][subphy_num] = MV_DDR4_VREF_SUBPHY_CAL_ABOVE;
		}
	}

	if (mv_ddr4_tap_tuning(dev_num, lambda_per_dq, TX_DIR) == MV_OK)
		tap_tune_passed = MV_TRUE;

	/* place dram to vref training mode */
	mv_ddr4_vref_training_mode_ctrl(dev_num, 0, ACCESS_TYPE_MULTICAST, MV_TRUE);

	/* main loop for 2d scan (low_to_high voltage scan) */
	vref_tap_idx = MV_DDR4_VREF_MAX_RANGE;
	vref_range_min = MV_DDR4_VREF_MIN_RANGE;

	if (vref_range_min < MV_DDR4_VREF_STEP_SIZE)
		vref_range_min = MV_DDR4_VREF_STEP_SIZE;

	/* clean vref status array */
	memset(vref_byte_status, BYTE_NOT_DEFINED, sizeof(vref_byte_status));

	for (vref_tap_idx = MV_DDR4_VREF_MAX_RANGE; (vref_tap_idx >= vref_range_min) &&
	     (all_subphys_state != MV_DDR4_VREF_SUBPHY_CAL_UNDER);
	     vref_tap_idx -= MV_DDR4_VREF_STEP_SIZE) {
		/* set new vref training value in dram */
		mv_ddr4_vref_tap_set(dev_num, 0, ACCESS_TYPE_MULTICAST, vref_tap_idx, vref_tap_set_state);

		if (tap_tune_passed == MV_FALSE) {
			if (mv_ddr4_tap_tuning(dev_num, lambda_per_dq, TX_DIR) == MV_OK)
				tap_tune_passed = MV_TRUE;
			else
				continue;
		}

		if (mv_ddr4_centralization(dev_num, lambda_per_dq, c_opt_per_bus, pbs_res_per_bus,
					   valid_win_size, TX_DIR, vref_tap_idx, 0) != MV_OK) {
			DEBUG_CALIBRATION(DEBUG_LEVEL_ERROR,
					  ("error: %s: ddr4 centralization failed (dq vref tap index %d)!!!\n",
					   __func__, vref_tap_idx));
			continue;
		}

		/* go over all results and find out the vref start and end window */
		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
				if (valid_win_size[if_id][subphy_num] > MV_DDR_VW_TX_NOISE_FILTER) {
					if (vref_state_per_subphy[if_id][subphy_num] == MV_DDR4_VREF_SUBPHY_CAL_UNDER)
						DEBUG_CALIBRATION(DEBUG_LEVEL_ERROR,
								  ("warning: %s: ddr4 vref voltage noise\n",
								   __func__));
					/* window is valid; keep current vref_tap_idx value and increment counter */
					vref_idx = valid_vref_cnt[if_id][subphy_num];
					valid_vref_ptr[if_id][subphy_num][vref_idx] = vref_tap_idx;
					valid_vref_cnt[if_id][subphy_num]++;

					/* set 0 for possible negative values */
					vref_byte_status[if_id][subphy_num][vref_idx] |=
						patterns_byte_status[if_id][subphy_num];
					dq_vref_start_win[if_id][subphy_num][vref_idx] =
						c_opt_per_bus[if_id][subphy_num] + 1 -
						valid_win_size[if_id][subphy_num] / 2;
					dq_vref_start_win[if_id][subphy_num][vref_idx] =
						(valid_win_size[if_id][subphy_num] % 2 == 0) ?
						dq_vref_start_win[if_id][subphy_num][vref_idx] :
						dq_vref_start_win[if_id][subphy_num][vref_idx] - 1;
					dq_vref_end_win[if_id][subphy_num][vref_idx] =
						c_opt_per_bus[if_id][subphy_num] +
						valid_win_size[if_id][subphy_num] / 2;
					vref_state_per_subphy[if_id][subphy_num] = MV_DDR4_VREF_SUBPHY_CAL_INSIDE;
				} else if (vref_state_per_subphy[if_id][subphy_num] == MV_DDR4_VREF_SUBPHY_CAL_INSIDE) {
					vref_state_per_subphy[if_id][subphy_num] = MV_DDR4_VREF_SUBPHY_CAL_UNDER;
				}
			} /* subphy */
		} /* if */

		/* check all subphys are in under state */
		all_subphys_state = MV_DDR4_VREF_SUBPHY_CAL_UNDER;
		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
				if (vref_state_per_subphy[if_id][subphy_num] != MV_DDR4_VREF_SUBPHY_CAL_UNDER)
					all_subphys_state = MV_DDR4_VREF_SUBPHY_CAL_INSIDE;
			}
		}
	}

	if (tap_tune_passed == MV_FALSE) {
		DEBUG_CALIBRATION(DEBUG_LEVEL_INFO,
				  ("%s: tap tune not passed on any dq_vref value\n", __func__));
		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			/* report fail for all active interfaces; multi-interface support - tbd */
			flow_result[if_id] = TEST_FAILED;
		}

		return MV_FAIL;
	}

	/* close vref range */
	mv_ddr4_vref_tap_set(dev_num, 0, ACCESS_TYPE_MULTICAST, vref_tap_idx, MV_DDR4_VREF_TAP_END);

	/* find out the results with the mixed and low states and move the low state 64 adlls */
	for (vref_idx = 0; vref_idx < MV_DDR4_VREF_MAX_RANGE; vref_idx++) {
		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
				if ((vref_byte_status[if_id][subphy_num][vref_idx]) &
				    (BYTE_HOMOGENEOUS_LOW | BYTE_SPLIT_OUT_MIX)) {
					if (dq_vref_start_win[if_id][subphy_num][vref_idx] <= 31 &&
					    dq_vref_end_win[if_id][subphy_num][vref_idx] <= 31) {
						dq_vref_start_win[if_id][subphy_num][vref_idx] += 64;
						dq_vref_end_win[if_id][subphy_num][vref_idx] += 64;
						DEBUG_CALIBRATION
							(DEBUG_LEVEL_TRACE,
							 ("%s vref_idx %d if %d subphy %d added 64 adlls to window\n",
							  __func__, valid_vref_ptr[if_id][subphy_num][vref_idx],
							  if_id, subphy_num));
					}
				}
			}
		}
	}

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			DEBUG_CALIBRATION(DEBUG_LEVEL_INFO,
					  ("calculating center of mass for subphy %d, valid windonw size %d\n",
					   subphy_num, valid_win_size[if_id][subphy_num]));
			if (valid_vref_cnt[if_id][subphy_num] > 0) {
				/* calculate center of mass sampling point (t, v) for each subphy */
				status = mv_ddr4_center_of_mass_calc(dev_num, if_id, subphy_num, TX_DIR,
								     dq_vref_start_win[if_id][subphy_num],
								     dq_vref_end_win[if_id][subphy_num],
								     valid_vref_ptr[if_id][subphy_num],
								     valid_vref_cnt[if_id][subphy_num],
								     &center_vref[if_id][subphy_num],
								     &center_adll[if_id][subphy_num]);
				if (status != MV_OK)
					return status;

				DEBUG_CALIBRATION(DEBUG_LEVEL_INFO,
						  ("center of mass results: vref %d, adll %d\n",
						   center_vref[if_id][subphy_num], center_adll[if_id][subphy_num]));
			}
		}
	}

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		vref_avg = 0;
		vref_subphy_num = 0;
		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			vref_avg += center_vref[if_id][subphy_num];
			vref_subphy_num++;
		}

		mv_ddr4_vref_tap_set(dev_num, if_id, ACCESS_TYPE_UNICAST,
				     vref_avg / vref_subphy_num, MV_DDR4_VREF_TAP_START);
		mv_ddr4_vref_tap_set(dev_num, if_id, ACCESS_TYPE_UNICAST,
				     vref_avg / vref_subphy_num, MV_DDR4_VREF_TAP_END);
		DEBUG_CALIBRATION(DEBUG_LEVEL_INFO, ("final vref average %d\n", vref_avg / vref_subphy_num));
		/* run centralization again with optimal vref to update global structures */
		mv_ddr4_centralization(dev_num, lambda_per_dq, c_opt_per_bus, pbs_res_per_bus, valid_win_size,
				       TX_DIR, vref_avg / vref_subphy_num, duty_cycle);
	}

	/* return dram from vref DRAM from vref training mode */
	mv_ddr4_vref_training_mode_ctrl(dev_num, 0, ACCESS_TYPE_MULTICAST, MV_FALSE);

	/* dqs tx reposition calculation */
	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			for (dq_idx = 0; dq_idx < 8; dq_idx++) {
				pad_num = dq_map_table[dq_idx +
						       subphy_num * BUS_WIDTH_IN_BITS +
						       if_id * BUS_WIDTH_IN_BITS * subphy_max];
				status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_UNICAST,
							    subphy_num, DDR_PHY_DATA,
							    0x10 + pad_num + effective_cs * 0x10,
							    pbs_res_per_bus[if_id][subphy_num][dq_idx]);
				if (status != MV_OK)
					return status;
			}

			status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_UNICAST,
						    subphy_num, DDR_PHY_DATA,
						    WRITE_CENTRALIZATION_PHY_REG + effective_cs * 4,
						    center_adll[if_id][subphy_num] % 64);
			if (status != MV_OK)
				return status;
		}
	}

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		/* report pass for all active interfaces; multi-interface support - tbd */
		flow_result[if_id] = TEST_SUCCESS;
	}

	return MV_OK;
}

/* centralization flow */
static int mv_ddr4_centralization(u8 dev_num, u16 (*lambda)[MAX_BUS_NUM][BUS_WIDTH_IN_BITS], u8 (*copt)[MAX_BUS_NUM],
				  u8 (*pbs_result)[MAX_BUS_NUM][BUS_WIDTH_IN_BITS], u8 (*vw_size)[MAX_BUS_NUM],
				  u8 mode, u16 param0, u8 param1)
{
/* FIXME:  remove the dependency in 64bit */
#if defined(CONFIG_64BIT)
#define MV_DDR_NUM_OF_CENTRAL_PATTERNS	(PATTERN_KILLER_DQ7_64 - PATTERN_KILLER_DQ0 + 1)
#else
#define MV_DDR_NUM_OF_CENTRAL_PATTERNS	(PATTERN_KILLER_DQ7 - PATTERN_KILLER_DQ0 + 1)
#endif
	static u8 subphy_end_win[MAX_DIR_TYPES][MAX_INTERFACE_NUM][MAX_BUS_NUM];
	static u8 subphy_start_win[MAX_DIR_TYPES][MAX_INTERFACE_NUM][MAX_BUS_NUM];
	static u8 final_start_win[MAX_INTERFACE_NUM][MAX_BUS_NUM][BUS_WIDTH_IN_BITS];
	static u8 final_end_win[MAX_INTERFACE_NUM][MAX_BUS_NUM][BUS_WIDTH_IN_BITS];
	enum hws_training_ip_stat training_result[MAX_INTERFACE_NUM];
	u32 if_id, subphy_num, pattern_id, pattern_loop_idx, bit_num;
	u8  curr_start_win[BUS_WIDTH_IN_BITS];
	u8  curr_end_win[BUS_WIDTH_IN_BITS];
	static u8 start_win_db[MV_DDR_NUM_OF_CENTRAL_PATTERNS][MAX_INTERFACE_NUM][MAX_BUS_NUM][BUS_WIDTH_IN_BITS];
	static u8 end_win_db[MV_DDR_NUM_OF_CENTRAL_PATTERNS][MAX_INTERFACE_NUM][MAX_BUS_NUM][BUS_WIDTH_IN_BITS];
	u8  curr_win[BUS_WIDTH_IN_BITS];
	u8  opt_win, waste_win, start_win_skew, end_win_skew;
	u8  final_subphy_win[MAX_INTERFACE_NUM][BUS_WIDTH_IN_BITS];
	enum hws_training_result result_type = RESULT_PER_BIT;
	enum hws_dir direction;
	enum hws_search_dir search_dir;
	u32 *result[HWS_SEARCH_DIR_LIMIT];
	u32 max_win_size;
	u8 curr_end_win_min, curr_start_win_max;
	u32 cs_ena_reg_val[MAX_INTERFACE_NUM];
	u8 current_byte_status;
	int status;
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	u8 subphy_max = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		/* save current cs enable reg val */
		status = ddr3_tip_if_read(dev_num, ACCESS_TYPE_UNICAST, if_id, CS_ENABLE_REG,
					  cs_ena_reg_val, MASK_ALL_BITS);
		if (status != MV_OK)
			return status;

		/* enable single cs */
		status = ddr3_tip_if_write(dev_num, ACCESS_TYPE_UNICAST, if_id, CS_ENABLE_REG,
					   (0x1 << 3), (0x1 << 3));
		if (status != MV_OK)
			return status;
	}

	if (mode == TX_DIR) {
		max_win_size = MAX_WINDOW_SIZE_TX;
		direction = OPER_WRITE;
	} else {
		max_win_size = MAX_WINDOW_SIZE_RX;
		direction = OPER_READ;
	}

	/* database initialization */
	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			patterns_byte_status[if_id][subphy_num] = BYTE_NOT_DEFINED;
			subphy_end_win[mode][if_id][subphy_num] = (max_win_size - 1);
			subphy_start_win[mode][if_id][subphy_num] = 0;
			vw_size[if_id][subphy_num] = (max_win_size - 1);
			for (bit_num = 0; bit_num < BUS_WIDTH_IN_BITS; bit_num++) {
				final_start_win[if_id][subphy_num][bit_num] = 0;
				final_end_win[if_id][subphy_num][bit_num] = (max_win_size - 1);
				if (mode == TX_DIR)
					final_end_win[if_id][subphy_num][bit_num] = (2 * max_win_size - 1);
			}
			if (mode == TX_DIR) {
				subphy_end_win[mode][if_id][subphy_num] = (2 * max_win_size - 1);
				vw_size[if_id][subphy_num] = (2 * max_win_size - 1);
			}
		}
	}

	/* main flow */
	/* FIXME: hard-coded "22" below for PATTERN_KILLER_DQ7_64 enum hws_pattern */
	for (pattern_id = PATTERN_KILLER_DQ0, pattern_loop_idx = 0;
	     pattern_id <= (MV_DDR_IS_64BIT_DRAM_MODE(tm->bus_act_mask) ? 22 : PATTERN_KILLER_DQ7);
	     pattern_id++, pattern_loop_idx++) {
		ddr3_tip_ip_training_wrapper(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST,
					     PARAM_NOT_CARE, result_type, HWS_CONTROL_ELEMENT_ADLL,
					     PARAM_NOT_CARE, direction, tm->if_act_mask,
					     0x0, max_win_size - 1, max_win_size - 1, pattern_id,
					     EDGE_FPF, CS_SINGLE, PARAM_NOT_CARE, training_result);

		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
				/*
				 * in case the previous patterns found the current subphy as BYTE_NOT_DEFINED,
				 * continue to next subphy
				 */
				if ((patterns_byte_status[if_id][subphy_num] == BYTE_NOT_DEFINED) &&
				    (pattern_id != PATTERN_KILLER_DQ0))
					continue;
				/*
				 * in case the result of the current subphy is BYTE_NOT_DEFINED mark the
				 * pattern byte status as BYTE_NOT_DEFINED
				 */
				current_byte_status = mv_ddr_tip_sub_phy_byte_status_get(if_id, subphy_num);
				if (current_byte_status == BYTE_NOT_DEFINED) {
					DEBUG_DDR4_CENTRALIZATION
						(DEBUG_LEVEL_INFO,
						 ("%s:%s: failed to lock subphy, pat %d if %d subphy %d\n",
						 __func__, str_dir[mode], pattern_id, if_id, subphy_num));
					patterns_byte_status[if_id][subphy_num] = BYTE_NOT_DEFINED;
					/* update the valid window size which is return value from this function */
					vw_size[if_id][subphy_num] = 0;
					/* continue to next subphy */
					continue;
				}

				/* set the status of this byte */
				patterns_byte_status[if_id][subphy_num] |= current_byte_status;
				for (search_dir = HWS_LOW2HIGH; search_dir <= HWS_HIGH2LOW; search_dir++) {
					status = ddr3_tip_read_training_result(dev_num, if_id, ACCESS_TYPE_UNICAST,
									       subphy_num, ALL_BITS_PER_PUP,
									       search_dir, direction, result_type,
									       TRAINING_LOAD_OPERATION_UNLOAD,
									       CS_SINGLE, &result[search_dir],
									       MV_TRUE, 0, MV_FALSE);
					if (status != MV_OK)
						return status;

					DEBUG_DDR4_CENTRALIZATION
					(DEBUG_LEVEL_INFO,
					 ("param0 %d param1 %d pat %d if %d subphy %d "
					 "regs: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
					 param0, param1, pattern_id, if_id, subphy_num,
					 result[search_dir][0], result[search_dir][1],
					 result[search_dir][2], result[search_dir][3],
					 result[search_dir][4], result[search_dir][5],
					 result[search_dir][6], result[search_dir][7]));
				}

				for (bit_num = 0; bit_num < BUS_WIDTH_IN_BITS; bit_num++) {
					/* read result success */
					DEBUG_DDR4_CENTRALIZATION(
								  DEBUG_LEVEL_INFO,
								  ("%s %s subphy locked, pat %d if %d subphy %d\n",
								  __func__, str_dir[mode], pattern_id,
								  if_id, subphy_num));
					start_win_db[pattern_loop_idx][if_id][subphy_num][bit_num] =
						GET_TAP_RESULT(result[HWS_LOW2HIGH][bit_num], EDGE_1);
					end_win_db[pattern_loop_idx][if_id][subphy_num][bit_num] =
						GET_TAP_RESULT(result[HWS_HIGH2LOW][bit_num], EDGE_1);
				}
			} /* subphy */
		} /* interface */
	} /* pattern */

	/*
	 * check if the current patterns subphys in all interfaces has mixed and low byte states
	 * in that case add 64 adlls to the low byte
	 */
	for (pattern_id = PATTERN_KILLER_DQ0, pattern_loop_idx = 0;
		pattern_id <= (MV_DDR_IS_64BIT_DRAM_MODE(tm->bus_act_mask) ? 22 : PATTERN_KILLER_DQ7);
		pattern_id++, pattern_loop_idx++) {
		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
				if (patterns_byte_status[if_id][subphy_num] == BYTE_NOT_DEFINED)
					continue;
				opt_win = 2 * max_win_size;	/* initialize opt_win */
				/* in case this byte in the pattern is homogeneous low add 64 adlls to the byte */
				if (((patterns_byte_status[if_id][subphy_num]) &
				    (BYTE_HOMOGENEOUS_LOW | BYTE_SPLIT_OUT_MIX)) ==
				     (BYTE_HOMOGENEOUS_LOW | BYTE_SPLIT_OUT_MIX)) {
					for (bit_num = 0; bit_num < BUS_WIDTH_IN_BITS; bit_num++) {
						if (start_win_db[pattern_loop_idx][if_id][subphy_num][bit_num] <= 31 &&
						    end_win_db[pattern_loop_idx][if_id][subphy_num][bit_num] <= 31) {
							start_win_db[pattern_loop_idx][if_id][subphy_num][bit_num] +=
								64;
							end_win_db[pattern_loop_idx][if_id][subphy_num][bit_num] += 64;
							DEBUG_DDR4_CENTRALIZATION
								(DEBUG_LEVEL_TRACE,
								 ("%s %s pattern %d if %d subphy %d bit %d added 64 "
								 "adll\n",
								 __func__, str_dir[mode], pattern_id, if_id,
								 subphy_num, bit_num));
						}
					}
				}

				/* calculations for the current pattern per subphy */
				for (bit_num = 0; bit_num < BUS_WIDTH_IN_BITS; bit_num++) {
					curr_win[bit_num] = end_win_db[pattern_loop_idx][if_id][subphy_num][bit_num] -
						start_win_db[pattern_loop_idx][if_id][subphy_num][bit_num] + 1;
					curr_start_win[bit_num] =
						start_win_db[pattern_loop_idx][if_id][subphy_num][bit_num];
					curr_end_win[bit_num] =
						end_win_db[pattern_loop_idx][if_id][subphy_num][bit_num];
				}

				opt_win = GET_MIN(opt_win, ddr3_tip_get_buf_min(curr_win));
				vw_size[if_id][subphy_num] =
					GET_MIN(vw_size[if_id][subphy_num], ddr3_tip_get_buf_min(curr_win));

				/* final subphy window length */
				final_subphy_win[if_id][subphy_num] = ddr3_tip_get_buf_min(curr_end_win) -
					ddr3_tip_get_buf_max(curr_start_win) + 1;
				waste_win = opt_win - final_subphy_win[if_id][subphy_num];
				start_win_skew = ddr3_tip_get_buf_max(curr_start_win) -
					ddr3_tip_get_buf_min(curr_start_win);
				end_win_skew = ddr3_tip_get_buf_max(curr_end_win) -
					ddr3_tip_get_buf_min(curr_end_win);

				/* min/max updated with pattern change */
				curr_end_win_min = ddr3_tip_get_buf_min(curr_end_win);
				curr_start_win_max = ddr3_tip_get_buf_max(curr_start_win);
				subphy_end_win[mode][if_id][subphy_num] =
					GET_MIN(subphy_end_win[mode][if_id][subphy_num], curr_end_win_min);
				subphy_start_win[mode][if_id][subphy_num] =
					GET_MAX(subphy_start_win[mode][if_id][subphy_num], curr_start_win_max);
				DEBUG_DDR4_CENTRALIZATION
					(DEBUG_LEVEL_INFO,
					 ("%s, %s pat %d if %d subphy %d opt_win %d ",
					 __func__, str_dir[mode], pattern_id, if_id, subphy_num, opt_win));
				DEBUG_DDR4_CENTRALIZATION
					(DEBUG_LEVEL_INFO,
					 ("final_subphy_win %d waste_win %d "
					 "start_win_skew %d end_win_skew %d ",
					 final_subphy_win[if_id][subphy_num],
					 waste_win, start_win_skew, end_win_skew));
				DEBUG_DDR4_CENTRALIZATION(DEBUG_LEVEL_INFO,
					("curr_start_win_max %d curr_end_win_min %d "
					"subphy_start_win %d subphy_end_win %d\n",
					curr_start_win_max, curr_end_win_min,
					subphy_start_win[mode][if_id][subphy_num],
					subphy_end_win[mode][if_id][subphy_num]));

				/* valid window */
				DEBUG_DDR4_CENTRALIZATION(DEBUG_LEVEL_INFO,
					("valid window, pat %d if %d subphy %d\n",
					pattern_id, if_id, subphy_num));
				for (bit_num = 0; bit_num < BUS_WIDTH_IN_BITS; bit_num++) {
					final_start_win[if_id][subphy_num][bit_num] =
						GET_MAX(final_start_win[if_id][subphy_num][bit_num],
							curr_start_win[bit_num]);
					final_end_win[if_id][subphy_num][bit_num] =
						GET_MIN(final_end_win[if_id][subphy_num][bit_num],
							curr_end_win[bit_num]);
				} /* bit */
			} /* subphy */
		} /* if_id */
	} /* pattern */

	/* calculate valid window for each subphy */
	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			if (patterns_byte_status[if_id][subphy_num] != BYTE_NOT_DEFINED) {
				/*
				 * in case of bytes status which were found as mixed and low
				 * change the their status to be mixed only, due to the fact
				 * that we have already dealt with this bytes by adding 64 adlls
				 * to the low bytes
				 */
				if (patterns_byte_status[if_id][subphy_num] &
				    (BYTE_HOMOGENEOUS_LOW | BYTE_SPLIT_OUT_MIX))
					patterns_byte_status[if_id][subphy_num] = BYTE_SPLIT_OUT_MIX;
				if (rx_vw_pos[if_id][subphy_num] == 0)	/* rx_vw_pos is initialized during tap tune */
					pbs_max = 31 - 0xa;
				else
					pbs_max = 31;

				/* continue if locked */
				/*if (centralization_state[if_id][subphy_num] == 0) {*/
				status = mv_ddr4_copt_get(mode, lambda[if_id][subphy_num],
							  final_start_win[if_id][subphy_num],
							  final_end_win[if_id][subphy_num],
							  pbs_result[if_id][subphy_num],
							  &copt[if_id][subphy_num]);
				DEBUG_DDR4_CENTRALIZATION
					(DEBUG_LEVEL_INFO,
					 ("%s %s if %d subphy %d copt %d\n",
					 __func__, str_dir[mode], if_id, subphy_num, copt[if_id][subphy_num]));

				if (status != MV_OK) {
					/*
					 * TODO: print out error message(s) only when all points fail
					 * as temporary solution, replaced ERROR to TRACE debug level
					 */
					DEBUG_DDR4_CENTRALIZATION
						(DEBUG_LEVEL_TRACE,
						 ("%s %s copt calculation failed, "
						 "no valid window for subphy %d\n",
						 __func__, str_dir[mode], subphy_num));
					/* set the byte to 0 (fail) and clean the status (continue with algorithm) */
					vw_size[if_id][subphy_num] = 0;
					status = MV_OK;

					if (debug_mode == MV_FALSE) {
						/*
						 * TODO: print out error message(s) only when all points fail
						 * as temporary solution, commented out debug level set to TRACE
						*/
						/*
						 * ddr3_hws_set_log_level(DEBUG_BLOCK_CALIBRATION, DEBUG_LEVEL_TRACE);
						 */
						/* open relevant log and run function again for debug */
						mv_ddr4_copt_get(mode, lambda[if_id][subphy_num],
									final_start_win[if_id][subphy_num],
									final_end_win[if_id][subphy_num],
									pbs_result[if_id][subphy_num],
									&copt[if_id][subphy_num]);
						/*
						 * ddr3_hws_set_log_level(DEBUG_BLOCK_CALIBRATION, DEBUG_LEVEL_ERROR);
						 */
					} /* debug mode */
				} /* status */
			} /* byte not defined */
		} /* subphy */
	} /* if_id */

	/* restore cs enable value*/
	for (if_id = 0; if_id < MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		status = ddr3_tip_if_write(dev_num, ACCESS_TYPE_UNICAST, if_id, CS_ENABLE_REG,
					   cs_ena_reg_val[if_id], MASK_ALL_BITS);
		if (status != MV_OK)
			return status;
	}

	return status;
}

/*
 * mv_ddr4_copt_get function
 * inputs:
 *	dir - direction; 0 is for rx, 1 for tx
 *	lambda - a pointer to adll to pbs ration multiplied by PBS_VAL_FACTOR
 *	vw_l - a pointer to valid window low limit in adll taps
 *	vw_h - a pointer to valid window high limit in adll taps
 * outputs:
 *	pbs_result - a pointer to pbs new delay value; the function's output
 *	copt - optimal center of subphy in adll taps
 * The function assumes initial pbs tap value is zero. Otherwise, it requires logic
 * getting pbs value per dq and setting pbs_taps_per_dq array.
 * It provides with a solution for a single subphy (8 bits).
 * The calling function is responsible for any additional pbs taps for dqs
 */
static int mv_ddr4_copt_get(u8 dir, u16 *lambda, u8 *vw_l, u8 *vw_h, u8 *pbs_result, u8 *copt)
{
	u8 center_per_dq[8];
	u8 center_zone_low[8] = {0};
	u8 center_zone_high[8] = {0};
	u8 ext_center_zone_low[8] = {0};
	u8 ext_center_zone_high[8] = {0};
	u8 pbs_taps_per_dq[8] = {0};
	u8 vw_per_dq[8];
	u8 vw_zone_low[8] = {0};
	u8 vw_zone_high[8] = {0};
	u8 margin_vw[8] = {0};
	u8 copt_val;
	u8 dq_idx;
	u8 center_zone_max_low = 0;
	u8 center_zone_min_high = 128;
	u8 vw_zone_max_low = 0;
	u8 vw_zone_min_high = 128;
	u8 min_vw = 63; /* minimum valid window between all bits */
	u8 center_low_el;
	u8 center_high_el;

	/* lambda calculated as D * PBS_VALUE_FACTOR / d */
	//printf("Copt::Debug::\t");
	for (dq_idx = 0; dq_idx < 8; dq_idx++) {
		center_per_dq[dq_idx] = 0.5 * (vw_h[dq_idx] + vw_l[dq_idx]);
		vw_per_dq[dq_idx] = 1 + (vw_h[dq_idx] - vw_l[dq_idx]);
		if (min_vw > vw_per_dq[dq_idx])
			min_vw = vw_per_dq[dq_idx];
	}

	/* calculate center zone */
	for (dq_idx = 0; dq_idx < 8; dq_idx++) {
		center_low_el = center_low_element_get(dir, pbs_taps_per_dq[dq_idx], lambda[dq_idx], pbs_max);
		if (center_per_dq[dq_idx] > center_low_el)
			center_zone_low[dq_idx] = center_per_dq[dq_idx] - center_low_el;
		center_high_el = center_high_element_get(dir, pbs_taps_per_dq[dq_idx], lambda[dq_idx], pbs_max);
		center_zone_high[dq_idx] = center_per_dq[dq_idx] + center_high_el;
		if (center_zone_max_low < center_zone_low[dq_idx])
			center_zone_max_low = center_zone_low[dq_idx];
		if (center_zone_min_high > center_zone_high[dq_idx])
			center_zone_min_high = center_zone_high[dq_idx];
		DEBUG_CALIBRATION(DEBUG_LEVEL_TRACE,
				  ("center: low %d, high %d, max_low %d, min_high %d\n",
				   center_zone_low[dq_idx], center_zone_high[dq_idx],
				   center_zone_max_low, center_zone_min_high));
	}

	if (center_zone_min_high >= center_zone_max_low) { /* center zone visib */
		/* set copt_val to high zone for rx */
		copt_val = (dir == RX_DIR) ? center_zone_max_low : center_zone_min_high;
		*copt = copt_val;

		/* calculate additional pbs taps */
		for (dq_idx = 0; dq_idx < 8; dq_idx++) {
			if (dir == RX_DIR)
				pbs_result[dq_idx] = (copt_val - center_per_dq[dq_idx]) *
						     PBS_VAL_FACTOR / lambda[dq_idx];
			else
				pbs_result[dq_idx] = (center_per_dq[dq_idx] - copt_val) *
						     PBS_VAL_FACTOR / lambda[dq_idx];
		}
		return MV_OK;
	} else { /* not center zone visib */
		for (dq_idx = 0; dq_idx < 8; dq_idx++) {
			if ((center_zone_low[dq_idx] + 1) > (0.5 * vw_per_dq[dq_idx] + vw_per_dq[dq_idx] % 2)) {
				vw_zone_low[dq_idx] = (center_zone_low[dq_idx] + 1) -
						      (0.5 * vw_per_dq[dq_idx] + vw_per_dq[dq_idx] % 2);
			} else {
				vw_zone_low[dq_idx] = 0;
				DEBUG_CALIBRATION(DEBUG_LEVEL_TRACE,
						  ("dq_idx %d, center zone low %d, vw_l %d, vw_l %d\n",
						   dq_idx, center_zone_low[dq_idx], vw_l[dq_idx], vw_h[dq_idx]));
			}

			vw_zone_high[dq_idx] = center_zone_high[dq_idx] + 0.5 * vw_per_dq[dq_idx];

			if (vw_zone_max_low < vw_zone_low[dq_idx])
				vw_zone_max_low = vw_zone_low[dq_idx];

			if (vw_zone_min_high > vw_zone_high[dq_idx])
				vw_zone_min_high = vw_zone_high[dq_idx];

			DEBUG_CALIBRATION(DEBUG_LEVEL_TRACE,
					  ("valid_window: low %d, high %d, max_low %d, min_high %d\n",
					   vw_zone_low[dq_idx], vw_zone_high[dq_idx],
					   vw_zone_max_low, vw_zone_min_high));
		}

		/* try to extend center zone */
		if (vw_zone_min_high >= vw_zone_max_low) { /* vw zone visib */
			center_zone_max_low = 0;
			center_zone_min_high = 128;

			for (dq_idx = 0; dq_idx < 8; dq_idx++) {
				margin_vw[dq_idx] =  vw_per_dq[dq_idx] - min_vw;

				if (center_zone_low[dq_idx] > margin_vw[dq_idx])
					ext_center_zone_low[dq_idx] = center_zone_low[dq_idx] - margin_vw[dq_idx];
				else
					ext_center_zone_low[dq_idx] = 0;

				ext_center_zone_high[dq_idx] = center_zone_high[dq_idx] + margin_vw[dq_idx];

				if (center_zone_max_low < ext_center_zone_low[dq_idx])
					center_zone_max_low = ext_center_zone_low[dq_idx];

				if (center_zone_min_high > ext_center_zone_high[dq_idx])
					center_zone_min_high = ext_center_zone_high[dq_idx];

				DEBUG_CALIBRATION(DEBUG_LEVEL_TRACE,
						  ("ext_center: low %d, high %d, max_low %d, min_high %d\n",
						   ext_center_zone_low[dq_idx], ext_center_zone_high[dq_idx],
						   center_zone_max_low, center_zone_min_high));
			}

			if (center_zone_min_high >= center_zone_max_low) { /* center zone visib */
				/* get optimal center position */
				copt_val = (dir == RX_DIR) ? center_zone_max_low : center_zone_min_high;
				*copt = copt_val;

				/* calculate additional pbs taps */
				for (dq_idx = 0; dq_idx < 8; dq_idx++) {
					if (dir == 0) {
						if (copt_val > center_per_dq[dq_idx])
							pbs_result[dq_idx] = (copt_val - center_per_dq[dq_idx]) *
									     PBS_VAL_FACTOR / lambda[dq_idx];
						else
							pbs_result[dq_idx] = 0;
					} else {
						if (center_per_dq[dq_idx] > copt_val)
							pbs_result[dq_idx] = (center_per_dq[dq_idx] - copt_val) *
									     PBS_VAL_FACTOR / lambda[dq_idx];
						else
							pbs_result[dq_idx] = 0;
					}

					if (pbs_result[dq_idx] > pbs_max)
						pbs_result[dq_idx] = pbs_max;
				}

				return MV_OK;
			} else { /* not center zone visib */
				/*
				 * TODO: print out error message(s) only when all points fail
				 * as temporary solution, replaced ERROR to TRACE debug level
				*/
				DEBUG_DDR4_CENTRALIZATION(DEBUG_LEVEL_TRACE,
							  ("lambda: %d, %d, %d, %d, %d, %d, %d, %d\n",
							   lambda[0], lambda[1], lambda[2], lambda[3],
							   lambda[4], lambda[5], lambda[6], lambda[7]));

				DEBUG_DDR4_CENTRALIZATION(DEBUG_LEVEL_TRACE,
							  ("vw_h: %d, %d, %d, %d, %d, %d, %d, %d\n",
							   vw_h[0], vw_h[1], vw_h[2], vw_h[3],
							   vw_h[4], vw_h[5], vw_h[6], vw_h[7]));

				DEBUG_DDR4_CENTRALIZATION(DEBUG_LEVEL_TRACE,
							  ("vw_l: %d, %d, %d, %d, %d, %d, %d, %d\n",
							   vw_l[0], vw_l[1], vw_l[2], vw_l[3],
							   vw_l[4], vw_l[5], vw_l[6], vw_l[7]));

				for (dq_idx = 0; dq_idx < 8; dq_idx++) {
					DEBUG_DDR4_CENTRALIZATION(DEBUG_LEVEL_TRACE,
								  ("center: low %d, high %d, "
								   "max_low %d, min_high %d\n",
								   center_zone_low[dq_idx], center_zone_high[dq_idx],
								   center_zone_max_low, center_zone_min_high));

					DEBUG_DDR4_CENTRALIZATION(DEBUG_LEVEL_TRACE,
								  ("valid_window: low %d, high %d, "
								   "max_low %d, min_high %d\n",
								   vw_zone_low[dq_idx], vw_zone_high[dq_idx],
								   vw_zone_max_low, vw_zone_min_high));

					DEBUG_DDR4_CENTRALIZATION(DEBUG_LEVEL_TRACE,
								  ("ext_center: low %d, high %d, "
								   "max_low %d, min_high %d\n",
								   ext_center_zone_low[dq_idx],
								   ext_center_zone_high[dq_idx],
								   center_zone_max_low, center_zone_min_high));
				}

				return MV_FAIL;
			}
		} else { /* not vw zone visib; failed to find a single sample point */
			return MV_FAIL;
		}
	}

	return MV_OK;
}

/*
 * mv_ddr4_dqs_reposition function gets copt to align to and returns pbs value per bit
 * parameters:
 *	dir - direction; 0 is for rx, 1 for tx
 *	lambda - a pointer to adll to pbs ration multiplied by PBS_VAL_FACTOR
 *	pbs_result - a pointer to pbs new delay value; the function's output
 *	delta - signed; possilbe values: +0xa, 0x0, -0xa; for rx can be only negative
 *	copt - optimal center of subphy in adll taps
 *	dqs_pbs - optimal pbs
 * The function assumes initial pbs tap value is zero. Otherwise, it requires logic
 * getting pbs value per dq and setting pbs_taps_per_dq array.
 * It provides with a solution for a single subphy (8 bits).
 * The calling function is responsible for any additional pbs taps for dqs
 */
static int mv_ddr4_dqs_reposition(u8 dir, u16 *lambda, u8 *pbs_result, char delta, u8 *copt, u8 *dqs_pbs)
{
	u8 dq_idx;
	u32 pbs_max_val = 0;
	u32 lambda_avg = 0;

	/* lambda calculated as D * X / d */
	for (dq_idx = 0; dq_idx < 8; dq_idx++) {
		if (pbs_max_val < pbs_result[dq_idx])
			pbs_max_val = pbs_result[dq_idx];
		lambda_avg += lambda[dq_idx];
	}

	if (delta >= 0)
		*dqs_pbs = (pbs_max_val + delta) / 2;
	else /* dqs already 0xa */
		*dqs_pbs = pbs_max_val / 2;

	lambda_avg /= 8;

	/* change in dqs pbs value requires change in final copt position from mass center solution */
	if (dir == TX_DIR) {
		/* for tx, additional pbs on dqs in opposite direction of adll */
		*copt = *copt + ((*dqs_pbs) * lambda_avg) / PBS_VAL_FACTOR;
	} else {
		/* for rx, additional pbs on dqs in same direction of adll */
		if (delta < 0)
			*copt = *copt - ((*dqs_pbs + delta) * lambda_avg) / PBS_VAL_FACTOR;
		else
			*copt = *copt - (*dqs_pbs * lambda_avg) / PBS_VAL_FACTOR;
	}

	return MV_OK;
}

/*
 * mv_ddr4_center_of_mass_calc function
 * parameters:
 *	vw_l - a pointer to valid window low limit in adll taps
 *	vw_h - a pointer to valid window high limit in adll taps
 *	vw_v - a pointer to vref value matching vw_l/h arrays
 *	vw_num - number of valid windows (lenght vw_v vector)
 *	v_opt - optimal voltage value in vref taps
 *	t_opt - optimal adll value in adll taps
 * This function solves 2D centroid equation (e.g., adll and vref axes)
 * The function doesn't differentiate between byte and bit eyes
 */
static int mv_ddr4_center_of_mass_calc(u8 dev_num, u8 if_id, u8 subphy_num, u8 mode, u8 *vw_l,
				       u8 *vw_h, u8 *vw_v, u8 vw_num, u8 *v_opt, u8 *t_opt)
{
	u8 idx;
	u8 edge_t[128], edge_v[128];
	u8 min_edge_t = 127, min_edge_v = 127;
	int polygon_area = 0;
	int t_opt_temp = 0, v_opt_temp = 0;
	int vw_avg = 0, v_avg = 0;
	int s0 = 0, s1 = 0, s2 = 0, slope = 1, r_sq = 0;
	u32 d_min = 10000, reg_val = 0;
	int status;

	/*
	 * reorder all polygon points counterclockwise
	 * get min value of each axis to shift to smaller calc value
	 */
	 for (idx = 0; idx < vw_num; idx++) {
		edge_t[idx] = vw_l[idx];
		edge_v[idx] = vw_v[idx];
		if (min_edge_v > vw_v[idx])
			min_edge_v = vw_v[idx];
		if (min_edge_t > vw_l[idx])
			min_edge_t = vw_l[idx];
		edge_t[vw_num * 2 - 1 - idx] = vw_h[idx];
		edge_v[vw_num * 2 - 1 - idx] = vw_v[idx];
		vw_avg += vw_h[idx] - vw_l[idx];
		v_avg += vw_v[idx];
		DEBUG_CALIBRATION(DEBUG_LEVEL_TRACE,
				  ("%s: if %d, byte %d, direction %d, vw_v %d, vw_l %d, vw_h %d\n",
				   __func__, if_id, subphy_num, mode, vw_v[idx], vw_l[idx], vw_h[idx]));
	}

	vw_avg *= 1000 / vw_num;
	v_avg /= vw_num;
	for (idx = 0; idx < vw_num; idx++) {
		s0 += (1000 * (vw_h[idx] - vw_l[idx]) - vw_avg) * (vw_v[idx] - v_avg);
		s1 += (vw_v[idx] - v_avg) * (vw_v[idx] - v_avg);
		s2 += (1000 * (vw_h[idx] - vw_l[idx]) - vw_avg) * (1000 * (vw_h[idx] - vw_l[idx]) - vw_avg);
	}
	r_sq = s0 * (s0 / s1);
	r_sq /= (s2 / 1000);
	slope = s0 / s1;

	/* idx n is equal to idx 0 */
	edge_t[vw_num * 2] = vw_l[0];
	edge_v[vw_num * 2] = vw_v[0];

	/* calculate polygon area, a (may be negative) */
	for (idx = 0; idx < vw_num * 2; idx++)
		polygon_area = polygon_area +
			       ((edge_t[idx] - min_edge_t)*(edge_v[idx + 1] - min_edge_v) -
			       (edge_t[idx + 1] - min_edge_t)*(edge_v[idx] - min_edge_v));

	/* calculate optimal point */
	for (idx = 0; idx < vw_num * 2; idx++) {
		t_opt_temp = t_opt_temp +
			     (edge_t[idx] + edge_t[idx + 1] - 2 * min_edge_t) *
			     ((edge_t[idx] - min_edge_t)*(edge_v[idx + 1] - min_edge_v) -
			      (edge_t[idx + 1] - min_edge_t)*(edge_v[idx] - min_edge_v));
		v_opt_temp = v_opt_temp +
			     (edge_v[idx] + edge_v[idx + 1] - 2 * min_edge_v) *
			     ((edge_t[idx] - min_edge_t)*(edge_v[idx + 1] - min_edge_v) -
			      (edge_t[idx + 1] - min_edge_t)*(edge_v[idx] - min_edge_v));
	}

	*t_opt = t_opt_temp / (3 * polygon_area);
	*v_opt = v_opt_temp / (3 * polygon_area);

	/* re-shift */
	*t_opt += min_edge_t;
	*v_opt += min_edge_v;

	/* calculate d_min */
	for (idx = 0; idx < 2 * vw_num; idx++) {
		s0 = (*t_opt - edge_t[idx]) * (*t_opt - edge_t[idx]) +
		     (*v_opt - edge_v[idx]) * (*v_opt - edge_v[idx]);
		d_min = (d_min > s0) ? s0 : d_min;
	}
	DEBUG_CALIBRATION(DEBUG_LEVEL_TRACE,
			  ("%s: r_sq %d, slope %d, area = %d, , d_min = %d\n",
			   __func__, r_sq, slope, polygon_area, d_min));

	/* insert vw eye to register database for validation */
	if (d_min < 0)
		d_min = -d_min;
	if (polygon_area < 0)
		polygon_area = -polygon_area;

	status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_UNICAST, subphy_num,
				    DDR_PHY_DATA, RESULT_DB_PHY_REG_ADDR + effective_cs + 4 * (1 - mode),
				    polygon_area);
	if (status != MV_OK)
		return status;

	status = ddr3_tip_bus_read(dev_num, if_id, ACCESS_TYPE_UNICAST,
				   dmin_phy_reg_table[effective_cs * 5 + subphy_num][0], DDR_PHY_CONTROL,
				   dmin_phy_reg_table[effective_cs * 5 + subphy_num][1], &reg_val);
	if (status != MV_OK)
		return status;

	reg_val &= 0xff << (8 * mode); /* rx clean bits 0..8, tx bits 9..16 */
	reg_val |= d_min / 2 << (8 * (1 - mode)); /* rX write bits 0..8, tx bits 9..16 */

	status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_UNICAST,
				    dmin_phy_reg_table[effective_cs * 5 + subphy_num][0], DDR_PHY_CONTROL,
				    dmin_phy_reg_table[effective_cs * 5 + subphy_num][1], reg_val);
	if (status != MV_OK)
		return status;

	if (polygon_area < 400) {
		DEBUG_CALIBRATION(DEBUG_LEVEL_ERROR,
				  ("%s: if %d, subphy %d: poligon area too small %d (dmin %d)\n",
				   __func__, if_id, subphy_num, polygon_area, d_min));
		if (debug_mode == MV_FALSE)
			return MV_FAIL;
	}

	return MV_OK;
}

/* tap tuning flow */
static int mv_ddr4_tap_tuning(u8 dev_num, u16 (*pbs_tap_factor)[MAX_BUS_NUM][BUS_WIDTH_IN_BITS], u8 mode)
{
	enum hws_training_ip_stat training_result[MAX_INTERFACE_NUM];
	u32 if_id, subphy_num, bit_num, pattern_id;
	u32 new_pbs_val = 15;
	u32 limit_div;
	u8 curr_start_win, curr_end_win;
	u8 upd_curr_start_win, upd_curr_end_win;
	u8 start_win_diff, end_win_diff;
	u32 max_win_size, a, b;
	u32 cs_ena_reg_val[MAX_INTERFACE_NUM];
	u32 reg_addr;
	enum hws_search_dir search_dir_id;
	enum hws_dir direction;
	u32 *result[MAX_BUS_NUM][HWS_SEARCH_DIR_LIMIT];
	u32 result1[MAX_BUS_NUM][HWS_SEARCH_DIR_LIMIT][BUS_WIDTH_IN_BITS];
	u8 subphy_max = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	enum hws_training_result result_type = RESULT_PER_BIT;
	int status;

	if (mode == TX_DIR) {
		max_win_size = MAX_WINDOW_SIZE_TX;
		direction = OPER_WRITE;
	} else {
		max_win_size = MAX_WINDOW_SIZE_RX;
		direction = OPER_READ;
	}

	/* clean all pbs registers to allow multiple run of this flow */
	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		reg_addr = (mode == RX_DIR) ? 0x5f : 0x1f;
		/* change pbs (0 at this stage) */
		status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_MULTICAST,
					    PARAM_NOT_CARE, DDR_PHY_DATA, reg_addr + effective_cs * 0x10, 0);
		if (status != MV_OK)
			return status;

		reg_addr = (mode == RX_DIR) ? 0x54 : 0x14;
		/* change pbs (0 at this stage) */
		status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_MULTICAST,
					    PARAM_NOT_CARE, DDR_PHY_DATA, reg_addr + effective_cs * 0x10, 0);
		if (status != MV_OK)
			return status;

		status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_MULTICAST,
					    PARAM_NOT_CARE, DDR_PHY_DATA, reg_addr + effective_cs * 0x10 + 1, 0);
		if (status != MV_OK)
			return status;
	}

	/* change pbs, find new window, and read current pbs value */
	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		/* save current cs enable reg val */
		status = ddr3_tip_if_read(dev_num, ACCESS_TYPE_UNICAST, if_id, CS_ENABLE_REG,
					  cs_ena_reg_val, MASK_ALL_BITS);
		if (status != MV_OK)
			return status;

		/* enable single cs */
		status = ddr3_tip_if_write(dev_num, ACCESS_TYPE_UNICAST, if_id, CS_ENABLE_REG,
					   (1 << 3), (1 << 3));
		if (status != MV_OK)
			return status;
	}

	/* find window; run training */
	/* FIXME: fix this hard-coded parameters due to compilation issue with patterns definitions */
	pattern_id = MV_DDR_IS_64BIT_DRAM_MODE(tm->bus_act_mask) ? 73 : 23;
	ddr3_tip_ip_training_wrapper(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST,
				     PARAM_NOT_CARE, result_type, HWS_CONTROL_ELEMENT_ADLL,
				     PARAM_NOT_CARE, direction, tm->if_act_mask,
				     0x0, max_win_size - 1, max_win_size - 1, pattern_id, EDGE_FPF, CS_SINGLE,
				     PARAM_NOT_CARE, training_result);

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			/* 0 - align to left, 1 - to center, 2 - to right */
			rx_vw_pos[if_id][subphy_num] = 1;
			for (search_dir_id = HWS_LOW2HIGH; search_dir_id <= HWS_HIGH2LOW; search_dir_id++) {
				status = ddr3_tip_read_training_result(dev_num, if_id, ACCESS_TYPE_UNICAST,
								       subphy_num, ALL_BITS_PER_PUP,
								       search_dir_id, direction,
								       result_type, TRAINING_LOAD_OPERATION_UNLOAD,
								       CS_SINGLE,
								       &(result[subphy_num][search_dir_id]),
								       MV_TRUE, 0, MV_FALSE);
				if (status != MV_OK)
					return status;

				DEBUG_TAP_TUNING_ENGINE(DEBUG_LEVEL_INFO,
							("cs %d if %d subphy %d mode %d result: "
							 "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
							 effective_cs, if_id, subphy_num, mode,
							 result[subphy_num][search_dir_id][0],
							 result[subphy_num][search_dir_id][1],
							 result[subphy_num][search_dir_id][2],
							 result[subphy_num][search_dir_id][3],
							 result[subphy_num][search_dir_id][4],
							 result[subphy_num][search_dir_id][5],
							 result[subphy_num][search_dir_id][6],
							 result[subphy_num][search_dir_id][7]));
			}

			for (bit_num = 0; bit_num < BUS_WIDTH_IN_BITS; bit_num++) {
				a = result[subphy_num][HWS_LOW2HIGH][bit_num];
				b = result[subphy_num][HWS_HIGH2LOW][bit_num];
				result1[subphy_num][HWS_LOW2HIGH][bit_num] = a;
				result1[subphy_num][HWS_HIGH2LOW][bit_num] = b;
			}
		}
	}

	/* change pbs, find new window, and read current pbs value */
	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		reg_addr = (mode == RX_DIR) ? 0x5f : 0x1f;
		/* change pbs (0 at this stage) */
		status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_MULTICAST,
					    PARAM_NOT_CARE, DDR_PHY_DATA, reg_addr + effective_cs * 0x10,
					    new_pbs_val);
		if (status != MV_OK)
			return status;
	 }

	/* FIXME: fix this hard-coded parameters due to compilation issue with patterns definitions */
	pattern_id = MV_DDR_IS_64BIT_DRAM_MODE(tm->bus_act_mask) ? 73 : 23;
	ddr3_tip_ip_training_wrapper(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ACCESS_TYPE_MULTICAST,
				     PARAM_NOT_CARE, result_type, HWS_CONTROL_ELEMENT_ADLL, PARAM_NOT_CARE,
				     direction, tm->if_act_mask, 0x0, max_win_size - 1, max_win_size - 1,
				     pattern_id, EDGE_FPF, CS_SINGLE, PARAM_NOT_CARE, training_result);

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			for (search_dir_id = HWS_LOW2HIGH; search_dir_id <= HWS_HIGH2LOW; search_dir_id++) {
				status = ddr3_tip_read_training_result(dev_num, if_id, ACCESS_TYPE_UNICAST,
								       subphy_num, ALL_BITS_PER_PUP, search_dir_id,
								       direction, result_type,
								       TRAINING_LOAD_OPERATION_UNLOAD, CS_SINGLE,
								       &(result[subphy_num][search_dir_id]),
								       MV_TRUE, 0, MV_FALSE);
				if (status != MV_OK)
					return status;

				DEBUG_TAP_TUNING_ENGINE(DEBUG_LEVEL_INFO,
							("cs %d if %d subphy %d mode %d result: "
							 "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
							 effective_cs, if_id, subphy_num, mode,
							 result[subphy_num][search_dir_id][0],
							 result[subphy_num][search_dir_id][1],
							 result[subphy_num][search_dir_id][2],
							 result[subphy_num][search_dir_id][3],
							 result[subphy_num][search_dir_id][4],
							 result[subphy_num][search_dir_id][5],
							 result[subphy_num][search_dir_id][6],
							 result[subphy_num][search_dir_id][7]));
			}

			for (bit_num = 0; bit_num < BUS_WIDTH_IN_BITS; bit_num++) {
				start_win_diff = 0;
				end_win_diff = 0;
				limit_div = 0;
				if ((GET_LOCK_RESULT(result1[subphy_num][HWS_LOW2HIGH][bit_num]) == MV_TRUE) &&
				    (GET_LOCK_RESULT(result1[subphy_num][HWS_HIGH2LOW][bit_num]) == MV_TRUE) &&
				    (GET_LOCK_RESULT(result[subphy_num][HWS_LOW2HIGH][bit_num]) == MV_TRUE) &&
				    (GET_LOCK_RESULT(result[subphy_num][HWS_HIGH2LOW][bit_num]) == MV_TRUE)) {
					curr_start_win = GET_TAP_RESULT(result1[subphy_num][HWS_LOW2HIGH][bit_num],
									EDGE_1);
					curr_end_win = GET_TAP_RESULT(result1[subphy_num][HWS_HIGH2LOW][bit_num],
								      EDGE_1);
					upd_curr_start_win = GET_TAP_RESULT(result[subphy_num][HWS_LOW2HIGH][bit_num],
									    EDGE_1);
					upd_curr_end_win = GET_TAP_RESULT(result[subphy_num][HWS_HIGH2LOW][bit_num],
									  EDGE_1);

					if (upd_curr_start_win != 0 && curr_start_win != 0) {
						if (upd_curr_start_win > curr_start_win)
							start_win_diff = upd_curr_start_win - curr_start_win;
						else
							start_win_diff = curr_start_win - upd_curr_start_win;
						limit_div++;
					} else {
						rx_vw_pos[if_id][subphy_num] = 0;
					}

					if (upd_curr_end_win != max_win_size && curr_end_win != max_win_size) {
						if (upd_curr_end_win  > curr_end_win)
							end_win_diff = upd_curr_end_win - curr_end_win;
						else
							end_win_diff = curr_end_win - upd_curr_end_win;
						limit_div++;
					} else {
						rx_vw_pos[if_id][subphy_num] = 2;
					}

					if (limit_div != 0) {
						pbs_tap_factor[if_id][subphy_num][bit_num] =
							PBS_VAL_FACTOR * (start_win_diff + end_win_diff) /
							(new_pbs_val * limit_div);
					}

					DEBUG_TAP_TUNING_ENGINE(DEBUG_LEVEL_INFO,
								("cs %d if %d subphy %d bit %d sw1 %d sw2 %d "
								 "ew1 %d ew2 %d sum delta %d, align %d\n",
								 effective_cs, if_id, subphy_num, bit_num,
								 curr_start_win, upd_curr_start_win,
								 curr_end_win, upd_curr_end_win,
								 pbs_tap_factor[if_id][subphy_num][bit_num],
								 rx_vw_pos[if_id][subphy_num]));
				} else {
					DEBUG_TAP_TUNING_ENGINE(DEBUG_LEVEL_ERROR,
								("tap tuning fail %s cs %d if %d subphy %d bit %d\n",
								 (mode == RX_DIR) ? "RX" : "TX", effective_cs, if_id,
								 subphy_num, bit_num));
					DEBUG_TAP_TUNING_ENGINE(DEBUG_LEVEL_ERROR,
								("cs %d, if %d subphy %d mode %d result: "
								 "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
								 effective_cs, if_id, subphy_num, mode,
								 result[subphy_num][HWS_LOW2HIGH][0],
								 result[subphy_num][HWS_LOW2HIGH][1],
								 result[subphy_num][HWS_LOW2HIGH][2],
								 result[subphy_num][HWS_LOW2HIGH][3],
								 result[subphy_num][HWS_LOW2HIGH][4],
								 result[subphy_num][HWS_LOW2HIGH][5],
								 result[subphy_num][HWS_LOW2HIGH][6],
								 result[subphy_num][HWS_LOW2HIGH][7]));
					DEBUG_TAP_TUNING_ENGINE(DEBUG_LEVEL_ERROR,
								("cs %d if %d subphy %d mode %d result: "
								 "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
								 effective_cs, if_id, subphy_num, mode,
								 result[subphy_num][HWS_HIGH2LOW][0],
								 result[subphy_num][HWS_HIGH2LOW][1],
								 result[subphy_num][HWS_HIGH2LOW][2],
								 result[subphy_num][HWS_HIGH2LOW][3],
								 result[subphy_num][HWS_HIGH2LOW][4],
								 result[subphy_num][HWS_HIGH2LOW][5],
								 result[subphy_num][HWS_HIGH2LOW][6],
								 result[subphy_num][HWS_HIGH2LOW][7]));
					DEBUG_TAP_TUNING_ENGINE(DEBUG_LEVEL_ERROR,
								("cs %d if %d subphy %d mode %d result: "
								 "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
								 effective_cs, if_id, subphy_num, mode,
								 result1[subphy_num][HWS_LOW2HIGH][0],
								 result1[subphy_num][HWS_LOW2HIGH][1],
								 result1[subphy_num][HWS_LOW2HIGH][2],
								 result1[subphy_num][HWS_LOW2HIGH][3],
								 result1[subphy_num][HWS_LOW2HIGH][4],
								 result1[subphy_num][HWS_LOW2HIGH][5],
								 result1[subphy_num][HWS_LOW2HIGH][6],
								 result1[subphy_num][HWS_LOW2HIGH][7]));
					DEBUG_TAP_TUNING_ENGINE(DEBUG_LEVEL_ERROR,
								("cs %d if %d subphy %d mode %d result: "
								 "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
								 effective_cs, if_id, subphy_num, mode,
								 result1[subphy_num][HWS_HIGH2LOW][0],
								 result1[subphy_num][HWS_HIGH2LOW][1],
								 result1[subphy_num][HWS_HIGH2LOW][2],
								 result1[subphy_num][HWS_HIGH2LOW][3],
								 result1[subphy_num][HWS_HIGH2LOW][4],
								 result1[subphy_num][HWS_HIGH2LOW][5],
								 result1[subphy_num][HWS_HIGH2LOW][6],
								 result1[subphy_num][HWS_HIGH2LOW][7]));
				}
			}
		}
	}

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id); /* restore cs enable value */
		status = ddr3_tip_if_write(dev_num, ACCESS_TYPE_UNICAST, if_id, CS_ENABLE_REG,
					   cs_ena_reg_val[if_id], MASK_ALL_BITS);
		if (status != MV_OK)
			return status;
	}

	/* restore pbs (set to 0) */
	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			reg_addr = (mode == RX_DIR) ? 0x5f : 0x1f;
			status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_UNICAST,
						    subphy_num, DDR_PHY_DATA, reg_addr + effective_cs * 0x10, 0);
			if (status != MV_OK)
				return status;
		}
	}

	if (mode == RX_DIR) {
		/*
		 * pattern special for rx
		 * check for rx_vw_pos stat
		 * - add n pbs taps to every dq to align to left (pbs_max set to (31 - n)
		 * - add pbs taps to dqs to align to right
		 */
		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
				if (rx_vw_pos[if_id][subphy_num] == 0) {
					status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0,
								    ACCESS_TYPE_UNICAST, subphy_num, DDR_PHY_DATA,
								    0x5f + effective_cs * 0x10, 0xa);
					if (status != MV_OK)
						return status;

					DEBUG_CALIBRATION(DEBUG_LEVEL_INFO,
							  ("%s: if %d, subphy %d aligned to left\n",
							   __func__, if_id, subphy_num));
				} else if (rx_vw_pos[if_id][subphy_num] == 2) {
					status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0,
								    ACCESS_TYPE_UNICAST, subphy_num, DDR_PHY_DATA,
								    0x54 + effective_cs * 0x10, 0xa);
					if (status != MV_OK)
						return status;

					status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0,
								    ACCESS_TYPE_UNICAST, subphy_num, DDR_PHY_DATA,
								    0x55 + effective_cs * 0x10, 0xa);
					if (status != MV_OK)
						return status;

					DEBUG_CALIBRATION(DEBUG_LEVEL_INFO,
							  ("%s: if %d , subphy %d aligned to right\n",
							   __func__, if_id, subphy_num));
				}
			} /* subphy */
		} /* if */
	} /* rx */

	return status;
}

/* receiver duty cycle flow */
#define DDR_PHY_JIRA_ENABLE
int mv_ddr4_receiver_calibration(u8 dev_num)
{
	u32  if_id, subphy_num;
	u32 vref_idx, dq_idx, pad_num = 0;
	u8 dq_vref_start_win[MAX_INTERFACE_NUM][MAX_BUS_NUM][RECEIVER_DC_MAX_COUNT];
	u8 dq_vref_end_win[MAX_INTERFACE_NUM][MAX_BUS_NUM][RECEIVER_DC_MAX_COUNT];
	u8 c_vref[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 valid_win_size[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 c_opt_per_bus[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 valid_vref_cnt[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 valid_vref_ptr[MAX_INTERFACE_NUM][MAX_BUS_NUM][RECEIVER_DC_MAX_COUNT];
	u8 center_adll[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 center_vref[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 pbs_res_per_bus[MAX_INTERFACE_NUM][MAX_BUS_NUM][BUS_WIDTH_IN_BITS];
	u16 lambda_per_dq[MAX_INTERFACE_NUM][MAX_BUS_NUM][BUS_WIDTH_IN_BITS];
	u8 dqs_pbs = 0, const_pbs;
	int tap_tune_passed = MV_FALSE;
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	enum hws_result *flow_result = ddr3_tip_get_result_ptr(training_stage);
	u8 subphy_max = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
#ifdef DDR_PHY_JIRA_ENABLE
	u32  dqs_pbs_jira56[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u8 delta = 0;
#endif
	u32 max_cs = ddr3_tip_max_cs_get(dev_num);
	u32 ctr_x[4], pbs_temp[4];
	u16 cs_index = 0, pbs_rx_avg, lambda_avg;
	int status;

	DEBUG_CALIBRATION(DEBUG_LEVEL_INFO, ("Starting ddr4 dc calibration training stage\n"));

	vdq_tv = 0;
	duty_cycle = 0;

	/* reset valid vref counter per if and subphy */
	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++)
		for (subphy_num = 0; subphy_num < MAX_BUS_NUM; subphy_num++)
			valid_vref_cnt[if_id][subphy_num] = 0;

	/* calculate pbs-adll tap tuning */
	/* reset special pattern configuration to re-run this stage */
	status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			   DDR_PHY_DATA, 0x5f + effective_cs * 0x10, 0x0);
	if (status != MV_OK)
		return status;

	status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			   DDR_PHY_DATA, 0x54 + effective_cs * 0x10, 0x0);
	if (status != MV_OK)
		return status;

	status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			   DDR_PHY_DATA, 0x55 + effective_cs * 0x10, 0x0);
	if (status != MV_OK)
		return status;

#ifdef DDR_PHY_JIRA_ENABLE
	if (effective_cs != 0) {
		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
				status = ddr3_tip_bus_read(dev_num, if_id, ACCESS_TYPE_UNICAST, subphy_num,
							   DDR_PHY_DATA, 0x54 + 0 * 0x10,
							   &dqs_pbs_jira56[if_id][subphy_num]);
				if (status != MV_OK)
					return status;

				status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0, ACCESS_TYPE_UNICAST,
							    subphy_num, DDR_PHY_DATA, 0x54 + 0 * 0x10, 0x0);
				if (status != MV_OK)
					return status;

				status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0, ACCESS_TYPE_UNICAST,
							    subphy_num, DDR_PHY_DATA, 0x55 + 0 * 0x10, 0x0);
				if (status != MV_OK)
					return status;
			}
		}
	}
#endif

	if (mv_ddr4_tap_tuning(dev_num, lambda_per_dq, RX_DIR) == MV_OK)
		tap_tune_passed = MV_TRUE;

	/* main loop for 2d scan (low_to_high voltage scan) */
	for (duty_cycle = RECEIVER_DC_MIN_RANGE;
	     duty_cycle <= RECEIVER_DC_MAX_RANGE;
	     duty_cycle += RECEIVER_DC_STEP_SIZE) {
		/* set new receiver dc training value in dram */
		status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
					    ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA,
					    CSN_IOB_VREF_REG(effective_cs), duty_cycle);
		if (status != MV_OK)
			return status;

		status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
					    ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA,
					    CSN_IO_BASE_VREF_REG(effective_cs) + 4, duty_cycle);
		if (status != MV_OK)
			return status;

		status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
					    ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA,
					    CSN_IO_BASE_VREF_REG(effective_cs) + 5, duty_cycle);
		if (status != MV_OK)
			return status;

		if (tap_tune_passed == MV_FALSE) {
			if (mv_ddr4_tap_tuning(dev_num, lambda_per_dq, RX_DIR) == MV_OK) {
				tap_tune_passed = MV_TRUE;
			} else {
				DEBUG_CALIBRATION(DEBUG_LEVEL_ERROR,
						  ("rc, tap tune failed inside calibration\n"));
				continue;
			}
		}

		if (mv_ddr4_centralization(dev_num, lambda_per_dq, c_opt_per_bus, pbs_res_per_bus,
					   valid_win_size, RX_DIR, vdq_tv, duty_cycle) != MV_OK) {
			DEBUG_CALIBRATION(DEBUG_LEVEL_ERROR,
					  ("error: ddr4 centralization failed (duty_cycle %d)!!!\n", duty_cycle));
			if (debug_mode == MV_FALSE)
				break;
		}

		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
				if (valid_win_size[if_id][subphy_num] > 8) {
					/* window is valid; keep current duty_cycle value and increment counter */
					vref_idx = valid_vref_cnt[if_id][subphy_num];
					valid_vref_ptr[if_id][subphy_num][vref_idx] = duty_cycle;
					valid_vref_cnt[if_id][subphy_num]++;
					c_vref[if_id][subphy_num] = c_opt_per_bus[if_id][subphy_num];
					/* set 0 for possible negative values */
					dq_vref_start_win[if_id][subphy_num][vref_idx] =
						c_vref[if_id][subphy_num] + 1 - valid_win_size[if_id][subphy_num] / 2;
					dq_vref_start_win[if_id][subphy_num][vref_idx] =
						(valid_win_size[if_id][subphy_num] % 2 == 0) ?
						dq_vref_start_win[if_id][subphy_num][vref_idx] :
						dq_vref_start_win[if_id][subphy_num][vref_idx] - 1;
					dq_vref_end_win[if_id][subphy_num][vref_idx] =
						c_vref[if_id][subphy_num] + valid_win_size[if_id][subphy_num] / 2;
				}
			} /* subphy */
		} /* if */
	} /* duty_cycle */

	if (tap_tune_passed == MV_FALSE) {
		DEBUG_CALIBRATION(DEBUG_LEVEL_INFO,
				  ("%s: tap tune not passed on any duty_cycle value\n", __func__));
		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			/* report fail for all active interfaces; multi-interface support - tbd */
			flow_result[if_id] = TEST_FAILED;
		}

		return MV_FAIL;
	}

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			DEBUG_CALIBRATION(DEBUG_LEVEL_INFO,
					  ("calculating center of mass for subphy %d, valid window size %d\n",
					   subphy_num, valid_win_size[if_id][subphy_num]));
			if (valid_vref_cnt[if_id][subphy_num] > 0) {
				/* calculate center of mass sampling point (t, v) for each subphy */
				status = mv_ddr4_center_of_mass_calc(dev_num, if_id, subphy_num, RX_DIR,
								     dq_vref_start_win[if_id][subphy_num],
								     dq_vref_end_win[if_id][subphy_num],
								     valid_vref_ptr[if_id][subphy_num],
								     valid_vref_cnt[if_id][subphy_num],
								     &center_vref[if_id][subphy_num],
								     &center_adll[if_id][subphy_num]);
				if (status != MV_OK)
					return status;

				DEBUG_CALIBRATION(DEBUG_LEVEL_INFO,
						  ("center of mass results: vref %d, adll %d\n",
						   center_vref[if_id][subphy_num], center_adll[if_id][subphy_num]));
			} else {
				DEBUG_CALIBRATION(DEBUG_LEVEL_ERROR,
						  ("%s: no valid window found for cs %d, subphy %d\n",
						   __func__, effective_cs, subphy_num));
				return MV_FAIL;
			}
		}
	}

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
						    ACCESS_TYPE_UNICAST, subphy_num, DDR_PHY_DATA,
						    CSN_IOB_VREF_REG(effective_cs),
						    center_vref[if_id][subphy_num]);
			if (status != MV_OK)
				return status;

			status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
						    ACCESS_TYPE_UNICAST, subphy_num, DDR_PHY_DATA,
						    CSN_IO_BASE_VREF_REG(effective_cs) + 4,
						    center_vref[if_id][subphy_num]);
			if (status != MV_OK)
				return status;

			status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
						    ACCESS_TYPE_UNICAST, subphy_num, DDR_PHY_DATA,
						    CSN_IO_BASE_VREF_REG(effective_cs) + 5,
						    center_vref[if_id][subphy_num]);
			if (status != MV_OK)
				return status;

			DEBUG_CALIBRATION(DEBUG_LEVEL_INFO, ("final dc %d\n", center_vref[if_id][subphy_num]));
		}

		/* run centralization again with optimal vref to update global structures */
		mv_ddr4_centralization(dev_num, lambda_per_dq, c_opt_per_bus, pbs_res_per_bus, valid_win_size,
				       RX_DIR, 0, center_vref[if_id][0]);

		for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
			const_pbs = 0;
			if (rx_vw_pos[if_id][subphy_num] == 0) {
				const_pbs = 0xa;
				mv_ddr4_dqs_reposition(RX_DIR, lambda_per_dq[if_id][subphy_num],
						       pbs_res_per_bus[if_id][subphy_num], 0xa,
						       &center_adll[if_id][subphy_num], &dqs_pbs);
			}

			if (rx_vw_pos[if_id][subphy_num] == 1) {
				mv_ddr4_dqs_reposition(RX_DIR, lambda_per_dq[if_id][subphy_num],
						       pbs_res_per_bus[if_id][subphy_num], 0x0,
						       &center_adll[if_id][subphy_num], &dqs_pbs);
			}

			if (rx_vw_pos[if_id][subphy_num] == 2) {
				mv_ddr4_dqs_reposition(RX_DIR, lambda_per_dq[if_id][subphy_num],
						       pbs_res_per_bus[if_id][subphy_num], -0xa,
						       &center_adll[if_id][subphy_num], &dqs_pbs);
			}

			/* dq pbs update */
			for (dq_idx = 0; dq_idx < 8 ; dq_idx++) {
				pad_num = dq_map_table[dq_idx +
						       subphy_num * BUS_WIDTH_IN_BITS +
						       if_id * BUS_WIDTH_IN_BITS * subphy_max];
				status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_UNICAST,
							    subphy_num, DDR_PHY_DATA,
							    0x50 + pad_num + effective_cs * 0x10,
							    const_pbs + pbs_res_per_bus[if_id][subphy_num][dq_idx]);
				if (status != MV_OK)
					return status;
			}

			/* dqs pbs update */
			status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0, ACCESS_TYPE_UNICAST, subphy_num,
						    DDR_PHY_DATA, 0x54 + effective_cs * 0x10, dqs_pbs);
			if (status != MV_OK)
				return status;

			status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0, ACCESS_TYPE_UNICAST, subphy_num,
						    DDR_PHY_DATA, 0x55 + effective_cs * 0x10, dqs_pbs);
			if (status != MV_OK)
				return status;

			status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_UNICAST,
						    subphy_num, DDR_PHY_DATA,
						    READ_CENTRALIZATION_PHY_REG + effective_cs * 4,
						    center_adll[if_id][subphy_num]);
			if (status != MV_OK)
				return status;

#ifdef DDR_PHY_JIRA_ENABLE
			if (effective_cs != 0) {
				status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0, ACCESS_TYPE_UNICAST,
							    subphy_num, DDR_PHY_DATA, 0x54 + 0 * 0x10,
							    dqs_pbs_jira56[if_id][subphy_num]);
				if (status != MV_OK)
					return status;

				status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, 0, ACCESS_TYPE_UNICAST,
							    subphy_num, DDR_PHY_DATA, 0x55 + 0 * 0x10,
							    dqs_pbs_jira56[if_id][subphy_num]);
				if (status != MV_OK)
					return status;
			}
#endif
		}
	}

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		/* report pass for all active interfaces; multi-interface support - tbd */
		flow_result[if_id] = TEST_SUCCESS;
	}

#ifdef DDR_PHY_JIRA_ENABLE
	if (effective_cs == (max_cs - 1)) {
		/* adjust dqs to be as cs0 */
		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (subphy_num = 0; subphy_num < subphy_max; subphy_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, subphy_num);
				pbs_rx_avg = 0;
				/* find average of all pbs of dqs and read ctr_x */
				for (cs_index = 0; cs_index < max_cs; cs_index++) {
					status = ddr3_tip_bus_read(dev_num, if_id, ACCESS_TYPE_UNICAST,
								   subphy_num, DDR_PHY_DATA,
								   0x54 + cs_index * 0x10,
								   &pbs_temp[cs_index]);
					if (status != MV_OK)
						return status;

					status = ddr3_tip_bus_read(dev_num, if_id, ACCESS_TYPE_UNICAST,
								   subphy_num, DDR_PHY_DATA,
								   0x3 + cs_index * 0x4,
								   &ctr_x[cs_index]);
					if (status != MV_OK)
						return status;

					pbs_rx_avg = pbs_rx_avg + pbs_temp[cs_index];
				}

				pbs_rx_avg = pbs_rx_avg / max_cs;

				/* update pbs and ctr_x */
				lambda_avg = (lambda_per_dq[if_id][subphy_num][0] +
					      lambda_per_dq[if_id][subphy_num][1] +
					      lambda_per_dq[if_id][subphy_num][2] +
					      lambda_per_dq[if_id][subphy_num][3] +
					      lambda_per_dq[if_id][subphy_num][4] +
					      lambda_per_dq[if_id][subphy_num][5] +
					      lambda_per_dq[if_id][subphy_num][6] +
					      lambda_per_dq[if_id][subphy_num][7]) / 8;

				for (cs_index = 0; cs_index < max_cs; cs_index++) {
					status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST,
								    0, ACCESS_TYPE_UNICAST,
								    subphy_num, DDR_PHY_DATA,
								    0x54 + cs_index * 0x10, pbs_rx_avg);
					if (status != MV_OK)
						return status;

					status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST,
								    0, ACCESS_TYPE_UNICAST,
								    subphy_num, DDR_PHY_DATA,
								    0x55 + cs_index * 0x10, pbs_rx_avg);
					if (status != MV_OK)
						return status;

					/* update */
					if (pbs_rx_avg >= pbs_temp[cs_index]) {
						delta = ((pbs_rx_avg - pbs_temp[cs_index]) * lambda_avg) /
							PBS_VAL_FACTOR;
						if (ctr_x[cs_index] >= delta) {
							ctr_x[cs_index] = ctr_x[cs_index] - delta;
						} else {
							ctr_x[cs_index] = 0;
							DEBUG_CALIBRATION(DEBUG_LEVEL_INFO,
									  ("jira ddrphy56 extend fix(-) required %d\n",
									   delta));
						}
					} else {
						delta = ((pbs_temp[cs_index] - pbs_rx_avg) * lambda_avg) /
							PBS_VAL_FACTOR;
						if ((ctr_x[cs_index] + delta) > 32) {
							ctr_x[cs_index] = 32;
							DEBUG_CALIBRATION(DEBUG_LEVEL_INFO,
									  ("jira ddrphy56 extend fix(+) required %d\n",
									   delta));
						} else {
							ctr_x[cs_index] = (ctr_x[cs_index] + delta);
						}
					}
					status = ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id,
								    ACCESS_TYPE_UNICAST, subphy_num, DDR_PHY_DATA,
								    READ_CENTRALIZATION_PHY_REG + effective_cs * 4,
								    ctr_x[cs_index]);
					if (status != MV_OK)
						return status;
				}
			}
		}
	}
#endif

    return MV_OK;
}
#endif /* CONFIG_DDR4 */
