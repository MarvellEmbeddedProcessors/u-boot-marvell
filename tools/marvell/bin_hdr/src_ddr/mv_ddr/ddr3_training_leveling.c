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

#include "ddr3_init.h"

#define WL_ITERATION_NUM		10

static u32 pup_mask_table[] = {
	0x000000ff,
	0x0000ff00,
	0x00ff0000,
	0xff000000
};

static struct write_supp_result wr_supp_res[MAX_INTERFACE_NUM][MAX_BUS_NUM];

static int ddr3_tip_dynamic_write_leveling_seq(u32 dev_num);
static int ddr3_tip_dynamic_read_leveling_seq(u32 dev_num);
static int ddr3_tip_dynamic_per_bit_read_leveling_seq(u32 dev_num);
static int ddr3_tip_wl_supp_align_phase_shift(u32 dev_num, u32 if_id,
					      u32 bus_id);
static int ddr3_tip_xsb_compare_test(u32 dev_num, u32 if_id, u32 bus_id,
				     u32 edge_offset);

u32 ddr3_tip_max_cs_get(u32 dev_num)
{
	u32 c_cs, if_id, bus_id;
	static u32 max_cs;
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);

	if (!max_cs) {
		CHECK_STATUS(ddr3_tip_get_first_active_if((u8)dev_num,
							  tm->if_act_mask,
							  &if_id));
		for (bus_id = 0; bus_id < octets_per_if_num; bus_id++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_id);
			break;
		}

		for (c_cs = 0; c_cs < NUM_OF_CS; c_cs++) {
			VALIDATE_ACTIVE(tm->
					interface_params[if_id].as_bus_params[bus_id].
					cs_bitmask, c_cs);
			max_cs++;
		}
	}

	return max_cs;
}

/*****************************************************************************
Dynamic read leveling
******************************************************************************/
int ddr3_tip_dynamic_read_leveling(u32 dev_num, u32 freq)
{
	u32 data, mask;
	u32 max_cs = ddr3_tip_max_cs_get(dev_num);
	u32 bus_num, if_id, cl_val;
	enum hws_speed_bin speed_bin_index;
	/* save current CS value */
	u32 cs_enable_reg_val[MAX_INTERFACE_NUM] = { 0 };
	int is_any_pup_fail = 0;
	u32 data_read[MAX_INTERFACE_NUM + 1] = { 0 };
	u8 rl_values[NUM_OF_CS][MAX_BUS_NUM][MAX_INTERFACE_NUM];
	struct pattern_info *pattern_table = ddr3_tip_get_pattern_table();
	u16 *mask_results_pup_reg_map = ddr3_tip_get_mask_results_pup_reg_map();
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	for (effective_cs = 0; effective_cs < NUM_OF_CS; effective_cs++)
		for (bus_num = 0; bus_num < MAX_BUS_NUM; bus_num++)
			for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++)
				rl_values[effective_cs][bus_num][if_id] = 0;

	for (effective_cs = 0; effective_cs < max_cs; effective_cs++) {
		for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			training_result[training_stage][if_id] = TEST_SUCCESS;

			/* save current cs enable reg val */
			CHECK_STATUS(ddr3_tip_if_read
				     (dev_num, ACCESS_TYPE_UNICAST, if_id,
				      CS_ENABLE_REG, cs_enable_reg_val,
				      MASK_ALL_BITS));
			/* enable single cs */
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_UNICAST, if_id,
				      CS_ENABLE_REG, (1 << 3), (1 << 3)));
		}

		ddr3_tip_reset_fifo_ptr(dev_num);

		/*
		 *     Phase 1: Load pattern (using ODPG)
		 *
		 * enter Read Leveling mode
		 * only 27 bits are masked
		 * assuming non multi-CS configuration
		 * write to CS = 0 for the non multi CS configuration, note
		 * that the results shall be read back to the required CS !!!
		 */

		/* BUS count is 0 shifted 26 */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      ODPG_DATA_CONTROL_REG, 0x3, 0x3));
		CHECK_STATUS(ddr3_tip_configure_odpg
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0,
			      pattern_table[PATTERN_RL].num_of_phases_tx, 0,
			      pattern_table[PATTERN_RL].num_of_phases_rx, 0, 0,
			      effective_cs, STRESS_NONE, DURATION_SINGLE));

		/* load pattern to ODPG */
		ddr3_tip_load_pattern_to_odpg(dev_num, ACCESS_TYPE_MULTICAST,
					      PARAM_NOT_CARE, PATTERN_RL,
					      pattern_table[PATTERN_RL].
					      start_addr);

		/*
		 *     Phase 2: ODPG to Read Leveling mode
		 */

		/* General Training Opcode register */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      ODPG_WRITE_READ_MODE_ENABLE_REG, 0,
			      MASK_ALL_BITS));

		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      ODPG_TRAINING_CONTROL_REG,
			      (0x301b01 | effective_cs << 2), 0x3c3fef));

		/* Object1 opcode register 0 & 1 */
		for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			speed_bin_index =
				tm->interface_params[if_id].speed_bin_index;
			cl_val =
				cas_latency_table[speed_bin_index].cl_val[freq];
			data = (cl_val << 17) | (0x3 << 25);
			mask = (0xff << 9) | (0x1f << 17) | (0x3 << 25);
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_UNICAST, if_id,
				      ODPG_OBJ1_OPCODE_REG, data, mask));
		}

		/* Set iteration count to max value */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_OPCODE_1_REG, 0xd00, 0xd00));

		/*
		 *     Phase 2: Mask config
		 */

		ddr3_tip_dynamic_read_leveling_seq(dev_num);

		/*
		 *     Phase 3: Read Leveling execution
		 */

		/* temporary jira dunit=14751 */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_DBG_1_REG, 0, (u32)(1 << 31)));
		/* configure phy reset value */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_DBG_3_REG, (0x7f << 24),
			      (u32)(0xff << 24)));
		/* data pup rd reset enable  */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      SDRAM_CONFIGURATION_REG, 0, (1 << 30)));
		/* data pup rd reset disable */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      SDRAM_CONFIGURATION_REG, (1 << 30), (1 << 30)));
		/* training SW override & training RL mode */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_SW_2_REG, 0x1, 0x9));
		/* training enable */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_REG, (1 << 24) | (1 << 20),
			      (1 << 24) | (1 << 20)));
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_REG, (u32)(1 << 31), (u32)(1 << 31)));

		/********* trigger training *******************/
		if (ddr3_tip_dev_attr_get(dev_num, MV_ATTR_TIP_REV) >= MV_TIP_REV_3) {
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
				      ODPG_TRAINING_TRIGGER_REG, 0x1, 0x1));

			/* check for training done + results pass */
			if (ddr3_tip_if_polling
			    (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0x2, 0x2,
			     ODPG_TRAINING_STATUS_REG,
			     MAX_POLLING_ITERATIONS) != MV_OK) {
				DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
					       ("Training Done Failed\n"));
				return MV_FAIL;
			}

			for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
				VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
				CHECK_STATUS(ddr3_tip_if_read
					     (dev_num, ACCESS_TYPE_UNICAST,
					      if_id,
					      ODPG_TRAINING_TRIGGER_REG, data_read,
					      0x4));
				data = data_read[if_id];
				if (data != 0x0) {
					DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
						       ("Training Result Failed\n"));
				}
			}

			/* disable ODPG - Back to functional mode */
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
				      ODPG_ENABLE_REG, 0x1 << ODPG_DISABLE_OFFS,
				      (0x1 << ODPG_DISABLE_OFFS)));

			if (ddr3_tip_if_polling
			    (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0x0,
			     0x1 << ODPG_ENABLE_OFFS, ODPG_ENABLE_REG,
			     MAX_POLLING_ITERATIONS) != MV_OK) {
				DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
					       ("ODPG disable failed "));
				return MV_FAIL;
			}

			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
				      ODPG_DATA_CONTROL_REG, 0, MASK_ALL_BITS));
		} else {
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
				      ODPG_TRAINING_STATUS_REG, 0x1, 0x1));
		}

		/* double loop on bus, pup */
		for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			/* check training done */
			is_any_pup_fail = 0;
			for (bus_num = 0;
			     bus_num < octets_per_if_num;
			     bus_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_num);
				if (ddr3_tip_if_polling
				    (dev_num, ACCESS_TYPE_UNICAST,
				     if_id, (1 << 25), (1 << 25),
				     mask_results_pup_reg_map[bus_num],
				     MAX_POLLING_ITERATIONS) != MV_OK) {
					DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
						       ("\n_r_l: DDR3 poll failed(2) for IF %d CS %d bus %d",
							if_id, effective_cs, bus_num));
					is_any_pup_fail = 1;
				} else {
					/* read result per pup */
					CHECK_STATUS(ddr3_tip_if_read
						     (dev_num,
						      ACCESS_TYPE_UNICAST,
						      if_id,
						      mask_results_pup_reg_map
						      [bus_num], data_read,
						      0xff));
					rl_values[effective_cs][bus_num]
						[if_id] = (u8)data_read[if_id];
				}
			}

			if (is_any_pup_fail == 1) {
				training_result[training_stage][if_id] =
					TEST_FAILED;
				if (debug_mode == 0)
					return MV_FAIL;
			}
		}

		DEBUG_LEVELING(DEBUG_LEVEL_INFO, ("RL exit read leveling\n"));

		/*
		 *     Phase 3: Exit Read Leveling
		 */

		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_SW_2_REG, (1 << 3), (1 << 3)));
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_SW_1_REG, (1 << 16), (1 << 16)));
		/* set ODPG to functional */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      ODPG_DATA_CONTROL_REG, 0x0, MASK_ALL_BITS));

		/*
		 * Copy the result from the effective CS search to the
		 * real Functional CS
		 */
		/*ddr3_tip_write_cs_result(dev_num, RL_PHY_REG); */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      ODPG_DATA_CONTROL_REG, 0x0, MASK_ALL_BITS));
	}

	for (effective_cs = 0; effective_cs < max_cs; effective_cs++) {
		/* double loop on bus, pup */
		for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (bus_num = 0;
			     bus_num < octets_per_if_num;
			     bus_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_num);
				/* read result per pup from arry */
				data = rl_values[effective_cs][bus_num][if_id];
				data = (data & 0x1f) |
					(((data & 0xe0) >> 5) << 6);
				ddr3_tip_bus_write(dev_num,
						   ACCESS_TYPE_UNICAST,
						   if_id,
						   ACCESS_TYPE_UNICAST,
						   bus_num, DDR_PHY_DATA,
						   RL_PHY_REG +
						   ((effective_cs ==
						     0) ? 0x0 : 0x4), data);
			}
		}
	}
	/* Set to 0 after each loop to avoid illegal value may be used */
	effective_cs = 0;

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		/* restore cs enable value */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      CS_ENABLE_REG, cs_enable_reg_val[if_id],
			      MASK_ALL_BITS));
		if (odt_config != 0) {
			CHECK_STATUS(ddr3_tip_write_additional_odt_setting
				     (dev_num, if_id));
		}
	}

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		if (training_result[training_stage][if_id] == TEST_FAILED)
			return MV_FAIL;
	}

	return MV_OK;
}

/*
 * Legacy Dynamic write leveling
 */
int ddr3_tip_legacy_dynamic_write_leveling(u32 dev_num)
{
	u32 c_cs, if_id, cs_mask = 0;
	u32 max_cs = ddr3_tip_max_cs_get(dev_num);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	/*
	 * In TRAINIUNG reg (0x15b0) write 0x80000008 | cs_mask:
	 * Trn_start
	 * cs_mask = 0x1 <<20 Trn_CS0 - CS0 is included in the DDR3 training
	 * cs_mask = 0x1 <<21 Trn_CS1 - CS1 is included in the DDR3 training
	 * cs_mask = 0x1 <<22 Trn_CS2 - CS2 is included in the DDR3 training
	 * cs_mask = 0x1 <<23 Trn_CS3 - CS3 is included in the DDR3 training
	 * Trn_auto_seq =  write leveling
	 */
	for (c_cs = 0; c_cs < max_cs; c_cs++)
		cs_mask = cs_mask | 1 << (20 + c_cs);

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, 0,
			      TRAINING_REG, (0x80000008 | cs_mask),
			      0xffffffff));
		mdelay(20);
		if (ddr3_tip_if_polling
		    (dev_num, ACCESS_TYPE_UNICAST, if_id, 0,
		     (u32)0x80000000, TRAINING_REG,
		     MAX_POLLING_ITERATIONS) != MV_OK) {
			DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
				       ("polling failed for Old WL result\n"));
			return MV_FAIL;
		}
	}

	return MV_OK;
}

/*
 * Legacy Dynamic read leveling
 */
int ddr3_tip_legacy_dynamic_read_leveling(u32 dev_num)
{
	u32 c_cs, if_id, cs_mask = 0;
	u32 max_cs = ddr3_tip_max_cs_get(dev_num);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	/*
	 * In TRAINIUNG reg (0x15b0) write 0x80000040 | cs_mask:
	 * Trn_start
	 * cs_mask = 0x1 <<20 Trn_CS0 - CS0 is included in the DDR3 training
	 * cs_mask = 0x1 <<21 Trn_CS1 - CS1 is included in the DDR3 training
	 * cs_mask = 0x1 <<22 Trn_CS2 - CS2 is included in the DDR3 training
	 * cs_mask = 0x1 <<23 Trn_CS3 - CS3 is included in the DDR3 training
	 * Trn_auto_seq =  Read Leveling using training pattern
	 */
	for (c_cs = 0; c_cs < max_cs; c_cs++)
		cs_mask = cs_mask | 1 << (20 + c_cs);

	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, 0, TRAINING_REG,
		      (0x80000040 | cs_mask), 0xffffffff));
	mdelay(100);

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		if (ddr3_tip_if_polling
		    (dev_num, ACCESS_TYPE_UNICAST, if_id, 0,
		     (u32)0x80000000, TRAINING_REG,
		     MAX_POLLING_ITERATIONS) != MV_OK) {
			DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
				       ("polling failed for Old RL result\n"));
			return MV_FAIL;
		}
	}

	return MV_OK;
}

/*
 * Dynamic per bit read leveling
 */
int ddr3_tip_dynamic_per_bit_read_leveling(u32 dev_num, u32 freq)
{
	u32 data, mask;
	u32 bus_num, if_id, cl_val, bit_num;
	u32 curr_numb, curr_min_delay;
	int adll_array[3] = { 0, -0xa, 0x14 };
	u32 phyreg3_arr[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	enum hws_speed_bin speed_bin_index;
	int is_any_pup_fail = 0;
	int break_loop = 0;
	u32 cs_enable_reg_val[MAX_INTERFACE_NUM]; /* save current CS value */
	u32 data_read[MAX_INTERFACE_NUM];
	int per_bit_rl_pup_status[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	u32 data2_write[MAX_INTERFACE_NUM][MAX_BUS_NUM];
	struct pattern_info *pattern_table = ddr3_tip_get_pattern_table();
	u16 *mask_results_dq_reg_map = ddr3_tip_get_mask_results_dq_reg();
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (bus_num = 0;
		     bus_num <= octets_per_if_num; bus_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_num);
			per_bit_rl_pup_status[if_id][bus_num] = 0;
			data2_write[if_id][bus_num] = 0;
			/* read current value of phy register 0x3 */
			CHECK_STATUS(ddr3_tip_bus_read
				     (dev_num, if_id, ACCESS_TYPE_UNICAST,
				      bus_num, DDR_PHY_DATA,
				      READ_CENTRALIZATION_PHY_REG,
				      &phyreg3_arr[if_id][bus_num]));
		}
	}

	/* NEW RL machine */
	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		training_result[training_stage][if_id] = TEST_SUCCESS;

		/* save current cs enable reg val */
		CHECK_STATUS(ddr3_tip_if_read
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      CS_ENABLE_REG, &cs_enable_reg_val[if_id],
			      MASK_ALL_BITS));
		/* enable single cs */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      CS_ENABLE_REG, (1 << 3), (1 << 3)));
	}

	ddr3_tip_reset_fifo_ptr(dev_num);
	for (curr_numb = 0; curr_numb < 3; curr_numb++) {
		/*
		 *     Phase 1: Load pattern (using ODPG)
		 *
		 * enter Read Leveling mode
		 * only 27 bits are masked
		 * assuming non multi-CS configuration
		 * write to CS = 0 for the non multi CS configuration, note that
		 * the results shall be read back to the required CS !!!
		 */

		/* BUS count is 0 shifted 26 */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      ODPG_DATA_CONTROL_REG, 0x3, 0x3));
		CHECK_STATUS(ddr3_tip_configure_odpg
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0,
			      pattern_table[PATTERN_TEST].num_of_phases_tx, 0,
			      pattern_table[PATTERN_TEST].num_of_phases_rx, 0,
			      0, 0, STRESS_NONE, DURATION_SINGLE));

		/* load pattern to ODPG */
		ddr3_tip_load_pattern_to_odpg(dev_num, ACCESS_TYPE_MULTICAST,
					      PARAM_NOT_CARE, PATTERN_TEST,
					      pattern_table[PATTERN_TEST].
					      start_addr);

		/*
		 *     Phase 2: ODPG to Read Leveling mode
		 */

		/* General Training Opcode register */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      ODPG_WRITE_READ_MODE_ENABLE_REG, 0,
			      MASK_ALL_BITS));
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      ODPG_TRAINING_CONTROL_REG, 0x301b01, 0x3c3fef));

		/* Object1 opcode register 0 & 1 */
		for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			speed_bin_index =
				tm->interface_params[if_id].speed_bin_index;
			cl_val =
				cas_latency_table[speed_bin_index].cl_val[freq];
			data = (cl_val << 17) | (0x3 << 25);
			mask = (0xff << 9) | (0x1f << 17) | (0x3 << 25);
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_UNICAST, if_id,
				      ODPG_OBJ1_OPCODE_REG, data, mask));
		}

		/* Set iteration count to max value */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_OPCODE_1_REG, 0xd00, 0xd00));

		/*
		 *     Phase 2: Mask config
		 */

		ddr3_tip_dynamic_per_bit_read_leveling_seq(dev_num);

		/*
		 *     Phase 3: Read Leveling execution
		 */

		/* temporary jira dunit=14751 */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_DBG_1_REG, 0, (u32)(1 << 31)));
		/* configure phy reset value */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_DBG_3_REG, (0x7f << 24),
			      (u32)(0xff << 24)));
		/* data pup rd reset enable  */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      SDRAM_CONFIGURATION_REG, 0, (1 << 30)));
		/* data pup rd reset disable */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      SDRAM_CONFIGURATION_REG, (1 << 30), (1 << 30)));
		/* training SW override & training RL mode */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_SW_2_REG, 0x1, 0x9));
		/* training enable */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_REG, (1 << 24) | (1 << 20),
			      (1 << 24) | (1 << 20)));
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_REG, (u32)(1 << 31), (u32)(1 << 31)));

		/********* trigger training *******************/
		if (ddr3_tip_dev_attr_get(dev_num, MV_ATTR_TIP_REV) >= MV_TIP_REV_3) {
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
				      ODPG_TRAINING_TRIGGER_REG, 0x1, 0x1));

			/* check for training done + results pass */
			if (ddr3_tip_if_polling
			    (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0x2, 0x2,
			     ODPG_TRAINING_STATUS_REG,
			     MAX_POLLING_ITERATIONS) != MV_OK) {
				DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
					       ("Training Done Failed\n"));
				return MV_FAIL;
			}

			for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
				VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
				CHECK_STATUS(ddr3_tip_if_read
					     (dev_num, ACCESS_TYPE_UNICAST,
					      if_id,
					      ODPG_TRAINING_TRIGGER_REG, data_read,
					      0x4));
				data = data_read[if_id];
				if (data != 0x0) {
					DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
						       ("Training Result Failed\n"));
				}
			}

			/*disable ODPG - Back to functional mode */
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
				      ODPG_ENABLE_REG, 0x1 << ODPG_DISABLE_OFFS,
				      (0x1 << ODPG_DISABLE_OFFS)));
			if (ddr3_tip_if_polling
			    (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, 0x0, 0x1,
			     ODPG_ENABLE_REG, MAX_POLLING_ITERATIONS) != MV_OK) {
				DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
					       ("ODPG disable failed "));
				return MV_FAIL;
			}
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
				      ODPG_DATA_CONTROL_REG, 0, MASK_ALL_BITS));
		} else {
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
				      ODPG_TRAINING_STATUS_REG, 0x1, 0x1));
		}

		/* double loop on bus, pup */
		for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			/* check training done */
			for (bus_num = 0;
			     bus_num < octets_per_if_num;
			     bus_num++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_num);

				if (per_bit_rl_pup_status[if_id][bus_num]
				    == 0) {
					curr_min_delay = 0;
					for (bit_num = 0; bit_num < 8;
					     bit_num++) {
						if (ddr3_tip_if_polling
						    (dev_num,
						     ACCESS_TYPE_UNICAST,
						     if_id, (1 << 25),
						     (1 << 25),
						     mask_results_dq_reg_map
						     [bus_num * 8 + bit_num],
						     MAX_POLLING_ITERATIONS) !=
						    MV_OK) {
							DEBUG_LEVELING
								(DEBUG_LEVEL_ERROR,
								 ("\n_r_l: DDR3 poll failed(2) for bus %d bit %d\n",
								  bus_num,
								  bit_num));
						} else {
							/* read result per pup */
							CHECK_STATUS
								(ddr3_tip_if_read
								 (dev_num,
								  ACCESS_TYPE_UNICAST,
								  if_id,
								  mask_results_dq_reg_map
								  [bus_num * 8 +
								   bit_num],
								  data_read,
								  MASK_ALL_BITS));
							data =
								(data_read
								 [if_id] &
								 0x1f) |
								((data_read
								  [if_id] &
								  0xe0) << 1);
							if (curr_min_delay == 0)
								curr_min_delay =
									data;
							else if (data <
								 curr_min_delay)
								curr_min_delay =
									data;
							if (data > data2_write[if_id][bus_num])
								data2_write
									[if_id]
									[bus_num] =
									data;
						}
					}

					if (data2_write[if_id][bus_num] <=
					    (curr_min_delay +
					     MAX_DQ_READ_LEVELING_DELAY)) {
						per_bit_rl_pup_status[if_id]
							[bus_num] = 1;
					}
				}
			}
		}

		/* check if there is need to search new phyreg3 value */
		if (curr_numb < 2) {
			/* if there is DLL that is not checked yet */
			for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1;
			     if_id++) {
				VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
				for (bus_num = 0;
				     bus_num < octets_per_if_num;
				     bus_num++) {
					VALIDATE_BUS_ACTIVE(tm->bus_act_mask,
							bus_num);
					if (per_bit_rl_pup_status[if_id]
					    [bus_num] != 1) {
						/* go to next ADLL value */
						CHECK_STATUS
							(ddr3_tip_bus_write
							 (dev_num,
							  ACCESS_TYPE_UNICAST,
							  if_id,
							  ACCESS_TYPE_UNICAST,
							  bus_num, DDR_PHY_DATA,
							  READ_CENTRALIZATION_PHY_REG,
							  (phyreg3_arr[if_id]
							   [bus_num] +
							   adll_array[curr_numb])));
						break_loop = 1;
						break;
					}
				}
				if (break_loop)
					break;
			}
		}		/* if (curr_numb < 2) */
		if (!break_loop)
			break;
	}		/* for ( curr_numb = 0; curr_numb <3; curr_numb++) */

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (bus_num = 0; bus_num < octets_per_if_num;
		     bus_num++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_num);
			if (per_bit_rl_pup_status[if_id][bus_num] == 1)
				ddr3_tip_bus_write(dev_num,
						   ACCESS_TYPE_UNICAST,
						   if_id,
						   ACCESS_TYPE_UNICAST,
						   bus_num, DDR_PHY_DATA,
						   RL_PHY_REG +
						   CS_BYTE_GAP(effective_cs),
						   data2_write[if_id]
						   [bus_num]);
			else
				is_any_pup_fail = 1;
		}

		/* TBD flow does not support multi CS */
		/*
		 * cs_bitmask = tm->interface_params[if_id].
		 * as_bus_params[bus_num].cs_bitmask;
		 */
		/* divide by 4 is used for retrieving the CS number */
		/*
		 * TBD BC2 - what is the PHY address for other
		 * CS ddr3_tip_write_cs_result() ???
		 */
		/*
		 * find what should be written to PHY
		 * - max delay that is less than threshold
		 */
		if (is_any_pup_fail == 1) {
			training_result[training_stage][if_id] = TEST_FAILED;
			if (debug_mode == 0)
				return MV_FAIL;
		}
	}
	DEBUG_LEVELING(DEBUG_LEVEL_INFO, ("RL exit read leveling\n"));

	/*
	 *     Phase 3: Exit Read Leveling
	 */

	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      TRAINING_SW_2_REG, (1 << 3), (1 << 3)));
	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      TRAINING_SW_1_REG, (1 << 16), (1 << 16)));
	/* set ODPG to functional */
	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      ODPG_DATA_CONTROL_REG, 0x0, MASK_ALL_BITS));
	/*
	 * Copy the result from the effective CS search to the real
	 * Functional CS
	 */
	ddr3_tip_write_cs_result(dev_num, RL_PHY_REG);
	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      ODPG_DATA_CONTROL_REG, 0x0, MASK_ALL_BITS));

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		/* restore cs enable value */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      CS_ENABLE_REG, cs_enable_reg_val[if_id],
			      MASK_ALL_BITS));
		if (odt_config != 0) {
			CHECK_STATUS(ddr3_tip_write_additional_odt_setting
				     (dev_num, if_id));
		}
	}

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		if (training_result[training_stage][if_id] == TEST_FAILED)
			return MV_FAIL;
	}

	return MV_OK;
}

int ddr3_tip_calc_cs_mask(u32 dev_num, u32 if_id, u32 effective_cs,
			  u32 *cs_mask)
{
	u32 all_bus_cs = 0, same_bus_cs;
	u32 bus_cnt;
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	*cs_mask = same_bus_cs = CS_BIT_MASK;

	/*
	 * In some of the devices (such as BC2), the CS is per pup and there
	 * for mixed mode is valid on like other devices where CS configuration
	 * is per interface.
	 * In order to know that, we do 'Or' and 'And' operation between all
	 * CS (of the pups).
	 * If they are they are not the same then it's mixed mode so all CS
	 * should be configured (when configuring the MRS)
	 */
	for (bus_cnt = 0; bus_cnt < octets_per_if_num; bus_cnt++) {
		VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_cnt);

		all_bus_cs |= tm->interface_params[if_id].
			as_bus_params[bus_cnt].cs_bitmask;
		same_bus_cs &= tm->interface_params[if_id].
			as_bus_params[bus_cnt].cs_bitmask;

		/* cs enable is active low */
		*cs_mask &= ~tm->interface_params[if_id].
			as_bus_params[bus_cnt].cs_bitmask;
	}

	if (all_bus_cs == same_bus_cs)
		*cs_mask = (*cs_mask | (~(1 << effective_cs))) & CS_BIT_MASK;

	return MV_OK;
}

/*
 * Dynamic write leveling
 */
int ddr3_tip_dynamic_write_leveling(u32 dev_num, int phase_remove)
{
	u32 reg_data = 0, iter, if_id, bus_cnt, trigger_reg_addr;
	u32 cs_enable_reg_val[MAX_INTERFACE_NUM] = { 0 };
	u32 cs_mask[MAX_INTERFACE_NUM];
	u32 read_data_sample_delay_vals[MAX_INTERFACE_NUM] = { 0 };
	u32 read_data_ready_delay_vals[MAX_INTERFACE_NUM] = { 0 };
	/* 0 for failure */
	u32 res_values[MAX_INTERFACE_NUM * MAX_BUS_NUM] = { 0 };
	u32 test_res = 0;	/* 0 - success for all pup */
	u32 data_read[MAX_INTERFACE_NUM];
	u8 wl_values[NUM_OF_CS][MAX_BUS_NUM][MAX_INTERFACE_NUM];
	u16 *mask_results_pup_reg_map = ddr3_tip_get_mask_results_pup_reg_map();
	u32 cs_mask0[MAX_INTERFACE_NUM] = { 0 };
	u32 max_cs = ddr3_tip_max_cs_get(dev_num);
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);

		training_result[training_stage][if_id] = TEST_SUCCESS;

		/* save Read Data Sample Delay */
		CHECK_STATUS(ddr3_tip_if_read
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      READ_DATA_SAMPLE_DELAY,
			      read_data_sample_delay_vals, MASK_ALL_BITS));
		/* save Read Data Ready Delay */
		CHECK_STATUS(ddr3_tip_if_read
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      READ_DATA_READY_DELAY, read_data_ready_delay_vals,
			      MASK_ALL_BITS));
		/* save current cs reg val */
		CHECK_STATUS(ddr3_tip_if_read
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      CS_ENABLE_REG, cs_enable_reg_val, MASK_ALL_BITS));
	}

	if (ddr3_tip_dev_attr_get(dev_num, MV_ATTR_TIP_REV) < MV_TIP_REV_3) {
		/* Enable multi-CS */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			     CS_ENABLE_REG, 0, (1 << 3)));
	}

	/*
	 *     Phase 1: DRAM 2 Write Leveling mode
	 */

	/*Assert 10 refresh commands to DRAM to all CS */
	for (iter = 0; iter < WL_ITERATION_NUM; iter++) {
		for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_UNICAST,
				      if_id, SDRAM_OPERATION_REG,
				      (u32)((~(0xf) << 8) | 0x2), 0xf1f));
		}
	}
	/* check controller back to normal */
	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		if (ddr3_tip_if_polling
		    (dev_num, ACCESS_TYPE_UNICAST, if_id, 0, 0x1f,
		     SDRAM_OPERATION_REG, MAX_POLLING_ITERATIONS) != MV_OK) {
			DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
				       ("WL: DDR3 poll failed(3)"));
		}
	}

	for (effective_cs = 0; effective_cs < max_cs; effective_cs++) {
		/*enable write leveling to all cs  - Q off , WL n */
		/* calculate interface cs mask */
		CHECK_STATUS(ddr3_tip_write_mrs_cmd(dev_num, cs_mask0, MR_CMD1,
						    0x1000, 0x1080));

		for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			/* cs enable is active low */
			ddr3_tip_calc_cs_mask(dev_num, if_id, effective_cs,
					      &cs_mask[if_id]);
		}

		if (ddr3_tip_dev_attr_get(dev_num, MV_ATTR_TIP_REV) >= MV_TIP_REV_3) {
			/* Enable Output buffer to relevant CS - Q on , WL on */
			CHECK_STATUS(ddr3_tip_write_mrs_cmd
				     (dev_num, cs_mask, MR_CMD1, 0x80, 0x1080));

			/*enable odt for relevant CS */
			CHECK_STATUS(ddr3_tip_if_write
				     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
				      0x1498, (0x3 << (effective_cs * 2)), 0xf));
		} else {
			/* FIXME: should be the same as _CPU case */
			CHECK_STATUS(ddr3_tip_write_mrs_cmd
				     (dev_num, cs_mask, MR_CMD1, 0xc0, 0x12c4));
		}

		/*
		 *     Phase 2: Set training IP to write leveling mode
		 */

		CHECK_STATUS(ddr3_tip_dynamic_write_leveling_seq(dev_num));

		/*
		 *     Phase 3: Trigger training
		 */
		if (ddr3_tip_dev_attr_get(dev_num, MV_ATTR_TIP_REV) < MV_TIP_REV_3)
			trigger_reg_addr = ODPG_TRAINING_STATUS_REG;
		else
			trigger_reg_addr = ODPG_TRAINING_TRIGGER_REG;

		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      trigger_reg_addr, 0x1, 0x1));

		if (ddr3_tip_dev_attr_get(dev_num, MV_ATTR_TIP_REV) >= MV_TIP_REV_3) {
			for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
				VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);

				/* training done */
				if (ddr3_tip_if_polling
				    (dev_num, ACCESS_TYPE_UNICAST, if_id,
				     (1 << 1), (1 << 1), ODPG_TRAINING_STATUS_REG,
				     MAX_POLLING_ITERATIONS) != MV_OK) {
					DEBUG_LEVELING(
						DEBUG_LEVEL_ERROR,
						("WL: DDR3 poll (4) failed (Data: 0x%x)\n",
						 reg_data));
				} else {
#if defined(CONFIG_ARMADA_38X) /* JIRA #1498 for 16 bit with ECC */
					if (tm->bus_act_mask == 0xb)
						break;
#endif
					CHECK_STATUS(ddr3_tip_if_read
						     (dev_num, ACCESS_TYPE_UNICAST,
						      if_id,
						      ODPG_TRAINING_TRIGGER_REG,
						      data_read, (1 << 2)));
					if (data_read[if_id] != 0) {
						DEBUG_LEVELING(
							DEBUG_LEVEL_ERROR,
							("WL 1: WL failed IF %d reg_data=0x%x\n",
							 if_id, data_read[if_id]));
					}
				}
			}
		}

		for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			/* training done */
			if (ddr3_tip_if_polling
			    (dev_num, ACCESS_TYPE_UNICAST, if_id,
			     (1 << 1), (1 << 1), ODPG_TRAINING_STATUS_REG,
			     MAX_POLLING_ITERATIONS) != MV_OK) {
				DEBUG_LEVELING(
					DEBUG_LEVEL_ERROR,
					("WL: DDR3 poll (4) failed (Data: 0x%x)\n",
					 reg_data));
			} else {
				CHECK_STATUS(ddr3_tip_if_read
					     (dev_num, ACCESS_TYPE_UNICAST,
					      if_id,
					      ODPG_TRAINING_STATUS_REG,
					      data_read, (1 << 2)));
				reg_data = data_read[if_id];
#if defined(CONFIG_ARMADA_38X) /* JIRA #1498 for 16 bit with ECC */
				if (tm->bus_act_mask == 0xb) {
					/* set to data to 0 to skip the check */
					reg_data = 0;
				}
#endif
				if (reg_data != 0) {
					DEBUG_LEVELING(
						DEBUG_LEVEL_ERROR,
						("WL 2: WL failed IF %d reg_data=0x%x\n",
						 if_id, reg_data));
				}

				/* check for training completion per bus */
				for (bus_cnt = 0;
				     bus_cnt < octets_per_if_num;
				     bus_cnt++) {
					VALIDATE_BUS_ACTIVE(tm->bus_act_mask,
							bus_cnt);
					/* training status */
					CHECK_STATUS(ddr3_tip_if_read
						     (dev_num,
						      ACCESS_TYPE_UNICAST,
						      if_id,
						      mask_results_pup_reg_map
						      [bus_cnt], data_read,
						      MASK_ALL_BITS));
					reg_data = data_read[if_id];
					DEBUG_LEVELING(
						DEBUG_LEVEL_TRACE,
						("WL: IF %d BUS %d reg 0x%x\n",
						 if_id, bus_cnt, reg_data));
					if ((reg_data & (1 << 25)) == 0) {
						res_values[
							(if_id *
							 octets_per_if_num)
							+ bus_cnt] = 1;
					}
					CHECK_STATUS(ddr3_tip_if_read
						     (dev_num,
						      ACCESS_TYPE_UNICAST,
						      if_id,
						      mask_results_pup_reg_map
						      [bus_cnt], data_read,
						      0xff));
					/*
					 * Save the read value that should be
					 * write to PHY register
					 */
					wl_values[effective_cs]
						[bus_cnt][if_id] =
						(u8)data_read[if_id];
				}
			}
		}

		/*
		 *     Phase 3.5: Validate result
		 */
		for (if_id = 0; if_id < MAX_INTERFACE_NUM; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			for (bus_cnt = 0; bus_cnt < octets_per_if_num; bus_cnt++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_cnt);
				/*
				 * Read result control register according to subphy
				 * "16" below is for a half-phase
				 */
				reg_data = wl_values[effective_cs][bus_cnt][if_id] + 16;
				/*
				 * Write to WL register: ADLL [4:0], Phase [8:6],
				 * Centralization ADLL [15:10] + 0x10
				 */
				reg_data = (reg_data & 0x1f) |
					   (((reg_data & 0xe0) >> 5) << 6) |
					   (((reg_data & 0x1f) + phy_reg1_val) << 10);
				/* Search with WL CS0 subphy reg */
				ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id,
						   ACCESS_TYPE_UNICAST, bus_cnt,
						   DDR_PHY_DATA, WL_PHY_REG, reg_data);
				/*
				 * Check for change in data read from DRAM.
				 * If changed, fix the result
				 */
				CHECK_STATUS(ddr3_tip_if_read
					     (dev_num,
					      ACCESS_TYPE_UNICAST,
					      if_id,
					      TRAINING_WRITE_LEVELING_REG,
					      data_read, MASK_ALL_BITS));
				if (((data_read[if_id] & (1 << (bus_cnt + 20))) >>
				     (bus_cnt + 20)) == 0) {
					DEBUG_LEVELING(
						DEBUG_LEVEL_ERROR,
						("WLValues was changed from 0x%X",
						 wl_values[effective_cs]
						 [bus_cnt][if_id]));
					wl_values[effective_cs]
					[bus_cnt][if_id] += 32;
					DEBUG_LEVELING(
						DEBUG_LEVEL_ERROR,
						("to 0x%X",
						 wl_values[effective_cs]
						 [bus_cnt][if_id]));
				}
			}
		}

		/*
		 *     Phase 4: Exit write leveling mode
		 */

		/* disable DQs toggling */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      WR_LEVELING_DQS_PATTERN_REG, 0x0, 0x1));

		/* Update MRS 1 (WL off) */
		if (ddr3_tip_dev_attr_get(dev_num, MV_ATTR_TIP_REV) >= MV_TIP_REV_3) {
			CHECK_STATUS(ddr3_tip_write_mrs_cmd(dev_num, cs_mask0, MR_CMD1,
							    0x1000, 0x1080));
		} else {
			/* FIXME: should be same as _CPU case */
			CHECK_STATUS(ddr3_tip_write_mrs_cmd(dev_num, cs_mask0, MR_CMD1,
							    0x1000, 0x12c4));
		}

		/* Update MRS 1 (return to functional mode - Q on , WL off) */
		CHECK_STATUS(ddr3_tip_write_mrs_cmd
			     (dev_num, cs_mask0, MR_CMD1, 0x0, 0x1080));

		/* set phy to normal mode */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_SW_2_REG, 0x5, 0x7));

		/* exit sw override mode  */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      TRAINING_SW_2_REG, 0x4, 0x7));
	}

	/*
	 *     Phase 5: Load WL values to each PHY
	 */

	for (effective_cs = 0; effective_cs < max_cs; effective_cs++) {
		for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
			VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
			test_res = 0;
			for (bus_cnt = 0;
			     bus_cnt < octets_per_if_num;
			     bus_cnt++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_cnt);
				/* check if result == pass */
				if (res_values
				    [(if_id *
				      octets_per_if_num) +
				     bus_cnt] == 0) {
					/*
					 * read result control register
					 * according to pup
					 */
					reg_data =
						wl_values[effective_cs][bus_cnt]
						[if_id];
					/*
					 * Write into write leveling register
					 * ([4:0] ADLL, [8:6] Phase, [15:10]
					 * (centralization) ADLL + 0x10)
					 */
					reg_data =
						(reg_data & 0x1f) |
						(((reg_data & 0xe0) >> 5) << 6) |
						(((reg_data & 0x1f) +
						  phy_reg1_val) << 10);
					/*
					 * in case phase remove should be executed
					 * need to remove more than one phase.
					 * this will take place only in low frequency,
					 * where there could be more than one phase between sub-phys
					 */
					if (phase_remove == 1)
						reg_data |= ((reg_data >> WR_LVL_PH_SEL_OFFS &
							     WR_LVL_PH_SEL_MASK) &
							     FIRST_PHASE) << WR_LVL_PH_SEL_OFFS;

					ddr3_tip_bus_write(
						dev_num,
						ACCESS_TYPE_UNICAST,
						if_id,
						ACCESS_TYPE_UNICAST,
						bus_cnt,
						DDR_PHY_DATA,
						WL_PHY_REG +
						effective_cs *
						CS_REGISTER_ADDR_OFFSET,
						reg_data);
				} else {
					test_res = 1;
					/*
					 * read result control register
					 * according to pup
					 */
					CHECK_STATUS(ddr3_tip_if_read
						     (dev_num,
						      ACCESS_TYPE_UNICAST,
						      if_id,
						      mask_results_pup_reg_map
						      [bus_cnt], data_read,
						      0xff));
					reg_data = data_read[if_id];
					DEBUG_LEVELING(
						DEBUG_LEVEL_ERROR,
						("WL: IF %d BUS %d failed, reg 0x%x\n",
						 if_id, bus_cnt, reg_data));
				}
			}

			if (test_res != 0) {
				training_result[training_stage][if_id] =
					TEST_FAILED;
			}
		}
	}
	/* Set to 0 after each loop to avoid illegal value may be used */
	effective_cs = 0;

	/*
	 * Copy the result from the effective CS search to the real
	 * Functional CS
	 */
	/* ddr3_tip_write_cs_result(dev_num, WL_PHY_REG); */
	/* restore saved values */
	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		/* restore Read Data Sample Delay */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      READ_DATA_SAMPLE_DELAY,
			      read_data_sample_delay_vals[if_id],
			      MASK_ALL_BITS));

		/* restore Read Data Ready Delay */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      READ_DATA_READY_DELAY,
			      read_data_ready_delay_vals[if_id],
			      MASK_ALL_BITS));

		/* enable multi cs */
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      CS_ENABLE_REG, cs_enable_reg_val[if_id],
			      MASK_ALL_BITS));
	}

	if (ddr3_tip_dev_attr_get(dev_num, MV_ATTR_TIP_REV) >= MV_TIP_REV_3) {
		/* Disable modt0 for CS0 training - need to adjust for multi-CS
		 * in case of ddr4 set 0xf else 0
		 */
		if (odt_config != 0) {
			CHECK_STATUS(ddr3_tip_if_write(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
						       SDRAM_ODT_CONTROL_HIGH_REG, 0x0, 0xf));
		}
		else {
			CHECK_STATUS(ddr3_tip_if_write(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
						       SDRAM_ODT_CONTROL_HIGH_REG, 0xf, 0xf));
		}

	}

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		if (training_result[training_stage][if_id] == TEST_FAILED)
			return MV_FAIL;
	}

	return MV_OK;
}

/*
 * Dynamic write leveling supplementary
 */
int ddr3_tip_dynamic_write_leveling_supp(u32 dev_num)
{
	int adll_offset;
	u32 if_id, bus_id, data, data_tmp;
	int is_if_fail = 0;
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		is_if_fail = 0;

		for (bus_id = 0; bus_id < octets_per_if_num; bus_id++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_id);
			wr_supp_res[if_id][bus_id].is_pup_fail = 1;
			CHECK_STATUS(ddr3_tip_bus_read
				     (dev_num, if_id, ACCESS_TYPE_UNICAST,
				      bus_id, DDR_PHY_DATA,
				      WRITE_CENTRALIZATION_PHY_REG +
				      effective_cs * CS_REGISTER_ADDR_OFFSET,
				      &data));
			DEBUG_LEVELING(
				DEBUG_LEVEL_TRACE,
				("WL Supp: adll_offset=0 data delay = %d\n",
				 data));
			if (ddr3_tip_wl_supp_align_phase_shift
			    (dev_num, if_id, bus_id) == MV_OK) {
				DEBUG_LEVELING(
					DEBUG_LEVEL_TRACE,
					("WL Supp: IF %d bus_id %d adll_offset=0 Success !\n",
					 if_id, bus_id));
				continue;
			}

			/* change adll */
			adll_offset = 5;
			CHECK_STATUS(ddr3_tip_bus_write
				     (dev_num, ACCESS_TYPE_UNICAST, if_id,
				      ACCESS_TYPE_UNICAST, bus_id, DDR_PHY_DATA,
				      WRITE_CENTRALIZATION_PHY_REG +
				      effective_cs * CS_REGISTER_ADDR_OFFSET,
				      data + adll_offset));
			CHECK_STATUS(ddr3_tip_bus_read
				     (dev_num, if_id, ACCESS_TYPE_UNICAST,
				      bus_id, DDR_PHY_DATA,
				      WRITE_CENTRALIZATION_PHY_REG +
				      effective_cs * CS_REGISTER_ADDR_OFFSET,
				      &data_tmp));
			DEBUG_LEVELING(
				DEBUG_LEVEL_TRACE,
				("WL Supp: adll_offset= %d data delay = %d\n",
				 adll_offset, data_tmp));

			if (ddr3_tip_wl_supp_align_phase_shift
			    (dev_num, if_id, bus_id) == MV_OK) {
				DEBUG_LEVELING(
					DEBUG_LEVEL_TRACE,
					("WL Supp: IF %d bus_id %d adll_offset= %d Success !\n",
					 if_id, bus_id, adll_offset));
				continue;
			}

			/* change adll */
			adll_offset = -5;
			CHECK_STATUS(ddr3_tip_bus_write
				     (dev_num, ACCESS_TYPE_UNICAST, if_id,
				      ACCESS_TYPE_UNICAST, bus_id, DDR_PHY_DATA,
				      WRITE_CENTRALIZATION_PHY_REG +
				      effective_cs * CS_REGISTER_ADDR_OFFSET,
				      data + adll_offset));
			CHECK_STATUS(ddr3_tip_bus_read
				     (dev_num, if_id, ACCESS_TYPE_UNICAST,
				      bus_id, DDR_PHY_DATA,
				      WRITE_CENTRALIZATION_PHY_REG +
				      effective_cs * CS_REGISTER_ADDR_OFFSET,
				      &data_tmp));
			DEBUG_LEVELING(
				DEBUG_LEVEL_TRACE,
				("WL Supp: adll_offset= %d data delay = %d\n",
				 adll_offset, data_tmp));
			if (ddr3_tip_wl_supp_align_phase_shift
			    (dev_num, if_id, bus_id) == MV_OK) {
				DEBUG_LEVELING(
					DEBUG_LEVEL_TRACE,
					("WL Supp: IF %d bus_id %d adll_offset= %d Success !\n",
					 if_id, bus_id, adll_offset));
				continue;
			} else {
				DEBUG_LEVELING(
					DEBUG_LEVEL_ERROR,
					("WL Supp: IF %d bus_id %d Failed !\n",
					 if_id, bus_id));
				is_if_fail = 1;
			}
		}

		if (is_if_fail == 1) {
			DEBUG_LEVELING(DEBUG_LEVEL_ERROR,
				       ("WL Supp: CS# %d: IF %d failed\n",
					effective_cs, if_id));
			training_result[training_stage][if_id] = TEST_FAILED;
		} else {
			training_result[training_stage][if_id] = TEST_SUCCESS;
		}
	}

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		if (training_result[training_stage][if_id] == TEST_FAILED)
			return MV_FAIL;
	}

	return MV_OK;
}

/*
 * Phase Shift
 */
static int ddr3_tip_wl_supp_align_phase_shift(u32 dev_num, u32 if_id,
					      u32 bus_id)
{
	u32 original_phase;
	u32 data, write_data;

	wr_supp_res[if_id][bus_id].stage = PHASE_SHIFT;
	if (ddr3_tip_xsb_compare_test
	    (dev_num, if_id, bus_id, 0) == MV_OK)
		return MV_OK;

	/* Read current phase */
	CHECK_STATUS(ddr3_tip_bus_read
		     (dev_num, if_id, ACCESS_TYPE_UNICAST, bus_id,
		      DDR_PHY_DATA, WL_PHY_REG + effective_cs *
		      CS_REGISTER_ADDR_OFFSET, &data));
	original_phase = (data >> 6) & 0x7;

	/* Set phase (0x0[6-8]) -2 */
	if (original_phase >= 1) {
		if (original_phase == 1)
			write_data = data & ~0x1df;
		else
			write_data = (data & ~0x1c0) |
				     ((original_phase - 2) << 6);
		ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id,
				   ACCESS_TYPE_UNICAST, bus_id, DDR_PHY_DATA,
				   WL_PHY_REG + effective_cs *
				   CS_REGISTER_ADDR_OFFSET, write_data);
		if (ddr3_tip_xsb_compare_test
		    (dev_num, if_id, bus_id, -2) == MV_OK)
			return MV_OK;
	}

	/* Set phase (0x0[6-8]) +2 */
	if (original_phase <= 5) {
		write_data = (data & ~0x1c0) |
			     ((original_phase + 2) << 6);
		ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id,
				   ACCESS_TYPE_UNICAST, bus_id, DDR_PHY_DATA,
				   WL_PHY_REG + effective_cs *
				   CS_REGISTER_ADDR_OFFSET, write_data);
		if (ddr3_tip_xsb_compare_test
		    (dev_num, if_id, bus_id, 2) == MV_OK)
			return MV_OK;
	}

	/* Set phase (0x0[6-8]) +4 */
	if (original_phase <= 3) {
		write_data = (data & ~0x1c0) |
			     ((original_phase + 4) << 6);
		ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id,
				   ACCESS_TYPE_UNICAST, bus_id, DDR_PHY_DATA,
				   WL_PHY_REG + effective_cs *
				   CS_REGISTER_ADDR_OFFSET, write_data);
		if (ddr3_tip_xsb_compare_test
		    (dev_num, if_id, bus_id, 4) == MV_OK)
			return MV_OK;
	}

	/* Set phase (0x0[6-8]) +6 */
	if (original_phase <= 1) {
		write_data = (data & ~0x1c0) |
			     ((original_phase + 6) << 6);
		ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id,
				   ACCESS_TYPE_UNICAST, bus_id, DDR_PHY_DATA,
				   WL_PHY_REG + effective_cs *
				   CS_REGISTER_ADDR_OFFSET, write_data);
		if (ddr3_tip_xsb_compare_test
		    (dev_num, if_id, bus_id, 6) == MV_OK)
			return MV_OK;
	}

	/* Write original WL result back */
	ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id,
			   ACCESS_TYPE_UNICAST, bus_id, DDR_PHY_DATA,
			   WL_PHY_REG + effective_cs *
			   CS_REGISTER_ADDR_OFFSET, data);
	wr_supp_res[if_id][bus_id].is_pup_fail = 1;

	return MV_FAIL;
}

/*
 * Compare Test
 */
static int ddr3_tip_xsb_compare_test(u32 dev_num, u32 if_id, u32 bus_id,
				     u32 edge_offset)
{
	u32 num_of_succ_byte_compare, word_in_pattern;
	u32 word_offset, i, num_of_word_mult;
	u32 read_pattern[TEST_PATTERN_LENGTH * 2];
	struct pattern_info *pattern_table = ddr3_tip_get_pattern_table();
	u32 pattern_test_pattern_table[8];
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	/* 3 below for INTERFACE_BUS_MASK_16BIT */
	num_of_word_mult = (tm->bus_act_mask == 3) ? 1 : 2;

	for (i = 0; i < 8; i++) {
		pattern_test_pattern_table[i] =
			pattern_table_get_word(dev_num, PATTERN_TEST, (u8)i);
	}

	/* External write, read and compare */
	CHECK_STATUS(ddr3_tip_load_pattern_to_mem(dev_num, PATTERN_TEST));

	CHECK_STATUS(ddr3_tip_reset_fifo_ptr(dev_num));

	CHECK_STATUS(ddr3_tip_ext_read
		     (dev_num, if_id,
		      ((pattern_table[PATTERN_TEST].start_addr << 3) +
		       ((SDRAM_CS_SIZE + 1) * effective_cs)), 1, read_pattern));

	DEBUG_LEVELING(
		DEBUG_LEVEL_TRACE,
		("XSB-compt CS#%d: IF %d bus_id %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		 effective_cs, if_id, bus_id,
		 read_pattern[0], read_pattern[1],
		 read_pattern[2], read_pattern[3],
		 read_pattern[4], read_pattern[5],
		 read_pattern[6], read_pattern[7]));

	/* compare byte per pup */
	num_of_succ_byte_compare = 0;
	for (word_in_pattern = start_xsb_offset;
	     word_in_pattern < (TEST_PATTERN_LENGTH * num_of_word_mult);
	     word_in_pattern++) {
		word_offset = word_in_pattern;
		if ((word_offset > (TEST_PATTERN_LENGTH * 2 - 1)))
			continue;

		if ((read_pattern[word_in_pattern] & pup_mask_table[bus_id]) ==
		    (pattern_test_pattern_table[word_offset] &
		     pup_mask_table[bus_id]))
			num_of_succ_byte_compare++;
	}

	if ((TEST_PATTERN_LENGTH * num_of_word_mult - start_xsb_offset) ==
	    num_of_succ_byte_compare) {
		wr_supp_res[if_id][bus_id].stage = edge_offset;
		DEBUG_LEVELING(DEBUG_LEVEL_TRACE,
			       ("supplementary: shift to %d for if %d pup %d success\n",
				edge_offset, if_id, bus_id));
		wr_supp_res[if_id][bus_id].is_pup_fail = 0;

		return MV_OK;
	} else {
		DEBUG_LEVELING(
			DEBUG_LEVEL_TRACE,
			("XSB-compt CS#%d: IF %d bus_id %d num_of_succ_byte_compare %d - Fail!\n",
			 effective_cs, if_id, bus_id, num_of_succ_byte_compare));

		DEBUG_LEVELING(
			DEBUG_LEVEL_TRACE,
			("XSB-compt: expected 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
			 pattern_test_pattern_table[0],
			 pattern_test_pattern_table[1],
			 pattern_test_pattern_table[2],
			 pattern_test_pattern_table[3],
			 pattern_test_pattern_table[4],
			 pattern_test_pattern_table[5],
			 pattern_test_pattern_table[6],
			 pattern_test_pattern_table[7]));
		DEBUG_LEVELING(
			DEBUG_LEVEL_TRACE,
			("XSB-compt: recieved 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
			 read_pattern[0], read_pattern[1],
			 read_pattern[2], read_pattern[3],
			 read_pattern[4], read_pattern[5],
			 read_pattern[6], read_pattern[7]));

		return MV_FAIL;
	}
}

/*
 * Dynamic write leveling sequence
 */
static int ddr3_tip_dynamic_write_leveling_seq(u32 dev_num)
{
	u32 bus_id, dq_id;
	u16 *mask_results_pup_reg_map = ddr3_tip_get_mask_results_pup_reg_map();
	u16 *mask_results_dq_reg_map = ddr3_tip_get_mask_results_dq_reg();
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      TRAINING_SW_2_REG, 0x1, 0x5));
	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      TRAINING_WRITE_LEVELING_REG, 0x50, 0xff));
	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      TRAINING_WRITE_LEVELING_REG, 0x5c, 0xff));
	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      ODPG_TRAINING_CONTROL_REG, 0x381b82, 0x3c3faf));
	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      ODPG_OBJ1_OPCODE_REG, (0x3 << 25), (0x3ffff << 9)));
	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      ODPG_OBJ1_ITER_CNT_REG, 0x80, 0xffff));
	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      ODPG_WRITE_LEVELING_DONE_CNTR_REG, 0x14, 0xff));
	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      TRAINING_WRITE_LEVELING_REG, 0xff5c, 0xffff));

	/* mask PBS */
	for (dq_id = 0; dq_id < MAX_DQ_NUM; dq_id++) {
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      mask_results_dq_reg_map[dq_id], 0x1 << 24,
			      0x1 << 24));
	}

	/* Mask all results */
	for (bus_id = 0; bus_id < octets_per_if_num; bus_id++) {
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      mask_results_pup_reg_map[bus_id], 0x1 << 24,
			      0x1 << 24));
	}

	/* Unmask only wanted */
	for (bus_id = 0; bus_id < octets_per_if_num; bus_id++) {
		VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_id);
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      mask_results_pup_reg_map[bus_id], 0, 0x1 << 24));
	}

	CHECK_STATUS(ddr3_tip_if_write
		     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		      WR_LEVELING_DQS_PATTERN_REG, 0x1, 0x1));

	return MV_OK;
}

/*
 * Dynamic read leveling sequence
 */
static int ddr3_tip_dynamic_read_leveling_seq(u32 dev_num)
{
	u32 bus_id, dq_id;
	u16 *mask_results_pup_reg_map = ddr3_tip_get_mask_results_pup_reg_map();
	u16 *mask_results_dq_reg_map = ddr3_tip_get_mask_results_dq_reg();
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	/* mask PBS */
	for (dq_id = 0; dq_id < MAX_DQ_NUM; dq_id++) {
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      mask_results_dq_reg_map[dq_id], 0x1 << 24,
			      0x1 << 24));
	}

	/* Mask all results */
	for (bus_id = 0; bus_id < octets_per_if_num; bus_id++) {
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      mask_results_pup_reg_map[bus_id], 0x1 << 24,
			      0x1 << 24));
	}

	/* Unmask only wanted */
	for (bus_id = 0; bus_id < octets_per_if_num; bus_id++) {
		VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_id);
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      mask_results_pup_reg_map[bus_id], 0, 0x1 << 24));
	}

	return MV_OK;
}

/*
 * Dynamic read leveling sequence
 */
static int ddr3_tip_dynamic_per_bit_read_leveling_seq(u32 dev_num)
{
	u32 bus_id, dq_id;
	u16 *mask_results_pup_reg_map = ddr3_tip_get_mask_results_pup_reg_map();
	u16 *mask_results_dq_reg_map = ddr3_tip_get_mask_results_dq_reg();
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	/* mask PBS */
	for (dq_id = 0; dq_id < MAX_DQ_NUM; dq_id++) {
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      mask_results_dq_reg_map[dq_id], 0x1 << 24,
			      0x1 << 24));
	}

	/* Mask all results */
	for (bus_id = 0; bus_id < octets_per_if_num; bus_id++) {
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      mask_results_pup_reg_map[bus_id], 0x1 << 24,
			      0x1 << 24));
	}

	/* Unmask only wanted */
	for (dq_id = 0; dq_id < MAX_DQ_NUM; dq_id++) {
		VALIDATE_BUS_ACTIVE(tm->bus_act_mask, dq_id / 8);
		CHECK_STATUS(ddr3_tip_if_write
			     (dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
			      mask_results_dq_reg_map[dq_id], 0x0 << 24,
			      0x1 << 24));
	}

	return MV_OK;
}

/*
 * Print write leveling supplementary results
 */
int ddr3_tip_print_wl_supp_result(u32 dev_num)
{
	u32 bus_id = 0, if_id = 0;
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	DEBUG_LEVELING(DEBUG_LEVEL_INFO,
		       ("I/F0 PUP0 Result[0 - success, 1-fail] ...\n"));

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (bus_id = 0; bus_id < octets_per_if_num;
		     bus_id++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_id);
			DEBUG_LEVELING(DEBUG_LEVEL_INFO,
				       ("%d ,", wr_supp_res[if_id]
					[bus_id].is_pup_fail));
		}
	}
	DEBUG_LEVELING(
		DEBUG_LEVEL_INFO,
		("I/F0 PUP0 Stage[0-phase_shift, 1-clock_shift, 2-align_shift] ...\n"));

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);
		for (bus_id = 0; bus_id < octets_per_if_num;
		     bus_id++) {
			VALIDATE_BUS_ACTIVE(tm->bus_act_mask, bus_id);
			DEBUG_LEVELING(DEBUG_LEVEL_INFO,
				       ("%d ,", wr_supp_res[if_id]
					[bus_id].stage));
		}
	}

	return MV_OK;
}
