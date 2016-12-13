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

#ifndef _DDR3_TRAINING_IP_H_
#define _DDR3_TRAINING_IP_H_

#include "ddr3_training_ip_def.h"
#include "ddr_topology_def.h"
#include "ddr_training_ip_db.h"

#define MV_DDR_VERSION "mv_ddr: version 16.12.1"

#define MAX_CS_NUM		4
#define MAX_TOTAL_BUS_NUM	(MAX_INTERFACE_NUM * MAX_BUS_NUM)

#define GET_MIN(arg1, arg2)	((arg1) < (arg2)) ? (arg1) : (arg2)
#define GET_MAX(arg1, arg2)	((arg1) < (arg2)) ? (arg2) : (arg1)

#define INIT_CONTROLLER_MASK_BIT	0x00000001
#define STATIC_LEVELING_MASK_BIT	0x00000002
#define SET_LOW_FREQ_MASK_BIT		0x00000004
#define LOAD_PATTERN_MASK_BIT		0x00000008
#define SET_MEDIUM_FREQ_MASK_BIT	0x00000010
#define WRITE_LEVELING_MASK_BIT		0x00000020
#define LOAD_PATTERN_2_MASK_BIT		0x00000040
#define READ_LEVELING_MASK_BIT		0x00000080
#define SW_READ_LEVELING_MASK_BIT	0x00000100
#define WRITE_LEVELING_SUPP_MASK_BIT	0x00000200
#define PBS_RX_MASK_BIT			0x00000400
#define PBS_TX_MASK_BIT			0x00000800
#define SET_TARGET_FREQ_MASK_BIT	0x00001000
#define ADJUST_DQS_MASK_BIT		0x00002000
#define WRITE_LEVELING_TF_MASK_BIT	0x00004000
#define LOAD_PATTERN_HIGH_MASK_BIT	0x00008000
#define READ_LEVELING_TF_MASK_BIT	0x00010000
#define WRITE_LEVELING_SUPP_TF_MASK_BIT	0x00020000
#define DM_PBS_TX_MASK_BIT		0x00040000
#define CENTRALIZATION_RX_MASK_BIT	0x00100000
#define CENTRALIZATION_TX_MASK_BIT	0x00200000
#define TX_EMPHASIS_MASK_BIT		0x00400000
#define PER_BIT_READ_LEVELING_TF_MASK_BIT	0x00800000
#define VREF_CALIBRATION_MASK_BIT	0x01000000
#define WRITE_LEVELING_LF_MASK_BIT	0x02000000

/* DDR4 Specific Training Mask bits */
#if defined (CONFIG_DDR4)
#define RECEIVER_CALIBRATION_MASK_BIT     0x04000000
#define WL_PHASE_CORRECTION_MASK_BIT      0x08000000
#define DQ_VREF_CALIBRATION_MASK_BIT      0x10000000
#define DQ_MAPPING_MASK_BIT               0x20000000
#endif /* CONFIG_DDR4 */

enum hws_result {
	TEST_FAILED = 0,
	TEST_SUCCESS = 1,
	NO_TEST_DONE = 2
};

enum hws_training_result {
	RESULT_PER_BIT,
	RESULT_PER_BYTE
};

enum auto_tune_stage {
	INIT_CONTROLLER,
	STATIC_LEVELING,
	SET_LOW_FREQ,
	LOAD_PATTERN,
	SET_MEDIUM_FREQ,
	WRITE_LEVELING,
	LOAD_PATTERN_2,
	READ_LEVELING,
#if defined(CONFIG_DDR4)
	SW_READ_LEVELING,
#endif /* CONFIG_DDR4 */
	WRITE_LEVELING_SUPP,
	PBS_RX,
	PBS_TX,
	SET_TARGET_FREQ,
	ADJUST_DQS,
	WRITE_LEVELING_TF,
	READ_LEVELING_TF,
	WRITE_LEVELING_SUPP_TF,
	DM_PBS_TX,
	VREF_CALIBRATION,
	CENTRALIZATION_RX,
	CENTRALIZATION_TX,
	TX_EMPHASIS,
	LOAD_PATTERN_HIGH,
	PER_BIT_READ_LEVELING_TF,
#if defined(CONFIG_DDR4)
	RECEIVER_CALIBRATION,
	WL_PHASE_CORRECTION,
	DQ_VREF_CALIBRATION,
	DQ_MAPPING,
#endif /* CONFIG_DDR4 */
	WRITE_LEVELING_LF,
	MAX_STAGE_LIMIT
};

enum hws_access_type {
	ACCESS_TYPE_UNICAST = 0,
	ACCESS_TYPE_MULTICAST = 1
};

enum hws_algo_type {
	ALGO_TYPE_DYNAMIC,
	ALGO_TYPE_STATIC
};

struct init_cntr_param {
	int is_ctrl64_bit;
	int do_mrs_phy;
	int init_phy;
	int msys_init;
};

struct pattern_info {
	u8 num_of_phases_tx;
	u8 tx_burst_size;
	u8 delay_between_bursts;
	u8 num_of_phases_rx;
	u32 start_addr;
	u8 pattern_len;
};

/* CL value for each frequency */
struct cl_val_per_freq {
	u8 cl_val[DDR_FREQ_LAST];
};

struct cs_element {
	u8 cs_num;
	u8 num_of_cs;
};

struct mode_info {
	/* 32 bits representing MRS bits */
	u32 reg_mr0[MAX_INTERFACE_NUM];
	u32 reg_mr1[MAX_INTERFACE_NUM];
	u32 reg_mr2[MAX_INTERFACE_NUM];
	u32 reg_m_r3[MAX_INTERFACE_NUM];
	/*
	 * Each element in array represent read_data_sample register delay for
	 * a specific interface.
	 * Each register, 4 bits[0+CS*8 to 4+CS*8] represent Number of DDR
	 * cycles from read command until data is ready to be fetched from
	 * the PHY, when accessing CS.
	 */
	u32 read_data_sample[MAX_INTERFACE_NUM];
	/*
	 * Each element in array represent read_data_sample register delay for
	 * a specific interface.
	 * Each register, 4 bits[0+CS*8 to 4+CS*8] represent the total delay
	 * from read command until opening the read mask, when accessing CS.
	 * This field defines the delay in DDR cycles granularity.
	 */
	u32 read_data_ready[MAX_INTERFACE_NUM];
};

struct hws_tip_freq_config_info {
	u8 is_supported;
	u8 bw_per_freq;
	u8 rate_per_freq;
};

struct hws_cs_config_info {
	u32 cs_reg_value;
	u32 cs_cbe_value;
};

struct dfx_access {
	u8 pipe;
	u8 client;
};

struct hws_xsb_info {
	struct dfx_access *dfx_table;
};

int ddr3_tip_register_dq_table(u32 dev_num, u32 *table);
int hws_ddr3_tip_select_ddr_controller(u32 dev_num, int enable);
int hws_ddr3_tip_init_controller(u32 dev_num,
				 struct init_cntr_param *init_cntr_prm);
int hws_ddr3_tip_load_topology_map(u32 dev_num,
				   struct mv_ddr_topology_map *topology);
int hws_ddr3_tip_run_alg(u32 dev_num, enum hws_algo_type algo_type);
int hws_ddr3_tip_mode_read(u32 dev_num, struct mode_info *mode_info);
int hws_ddr3_tip_read_training_result(u32 dev_num,
		enum hws_result result[MAX_STAGE_LIMIT][MAX_INTERFACE_NUM]);
int ddr3_tip_is_pup_lock(u32 *pup_buf, enum hws_training_result read_mode);
u8 ddr3_tip_get_buf_min(u8 *buf_ptr);
u8 ddr3_tip_get_buf_max(u8 *buf_ptr);
uint64_t mv_ddr_get_memory_size_per_cs_in_bits(void);
uint64_t mv_ddr_get_total_memory_size_in_bits(void);
#endif /* _DDR3_TRAINING_IP_H_ */
