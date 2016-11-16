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

#ifndef _DDR3_TRAINING_IP_PRV_IF_H
#define _DDR3_TRAINING_IP_PRV_IF_H

#include "ddr3_training_ip.h"
#include "ddr3_training_ip_flow.h"
#include "ddr3_training_ip_bist.h"

enum hws_static_config_type {
	WRITE_LEVELING_STATIC,
	READ_LEVELING_STATIC
};

struct ddr3_device_info {
	u32 device_id;
	u32 ck_delay;
};

typedef int (*HWS_TIP_DUNIT_MUX_SELECT_FUNC_PTR)(u8 dev_num, int enable);
typedef int (*HWS_TIP_DUNIT_REG_READ_FUNC_PTR)(
	u8 dev_num, enum hws_access_type interface_access, u32 if_id,
	u32 offset, u32 *data, u32 mask);
typedef int (*HWS_TIP_DUNIT_REG_WRITE_FUNC_PTR)(
	u8 dev_num, enum hws_access_type interface_access, u32 if_id,
	u32 offset, u32 data, u32 mask);
typedef int (*HWS_TIP_GET_FREQ_CONFIG_INFO)(
	u8 dev_num, enum hws_ddr_freq freq,
	struct hws_tip_freq_config_info *freq_config_info);
typedef int (*HWS_TIP_GET_DEVICE_INFO)(
	u8 dev_num, struct ddr3_device_info *info_ptr);
typedef int (*HWS_GET_CS_CONFIG_FUNC_PTR)(
	u8 dev_num, u32 cs_mask, struct hws_cs_config_info *cs_info);
typedef int (*HWS_SET_FREQ_DIVIDER_FUNC_PTR)(
	u8 dev_num, u32 if_id, enum hws_ddr_freq freq);
typedef int (*HWS_GET_INIT_FREQ)(u8 dev_num, enum hws_ddr_freq *freq);
typedef int (*HWS_TRAINING_IP_IF_WRITE_FUNC_PTR)(
	u32 dev_num, enum hws_access_type access_type, u32 dunit_id,
	u32 reg_addr, u32 data, u32 mask);
typedef int (*HWS_TRAINING_IP_IF_READ_FUNC_PTR)(
	u32 dev_num, enum hws_access_type access_type, u32 dunit_id,
	u32 reg_addr, u32 *data, u32 mask);
typedef int (*HWS_TRAINING_IP_BUS_WRITE_FUNC_PTR)(
	u32 dev_num, enum hws_access_type dunit_access_type, u32 if_id,
	enum hws_access_type phy_access_type, u32 phy_id,
	enum hws_ddr_phy phy_type, u32 reg_addr, u32 data);
typedef int (*HWS_TRAINING_IP_BUS_READ_FUNC_PTR)(
	u32 dev_num, u32 if_id, enum hws_access_type phy_access_type,
	u32 phy_id, enum hws_ddr_phy phy_type, u32 reg_addr, u32 *data);
typedef int (*HWS_TRAINING_IP_ALGO_RUN_FUNC_PTR)(
	u32 dev_num, enum hws_algo_type algo_type);
typedef int (*HWS_TRAINING_IP_SET_FREQ_FUNC_PTR)(
	u32 dev_num, enum hws_access_type access_type, u32 if_id,
	enum hws_ddr_freq frequency);
typedef int (*HWS_TRAINING_IP_INIT_CONTROLLER_FUNC_PTR)(
	u32 dev_num, struct init_cntr_param *init_cntr_prm);
typedef int (*HWS_TRAINING_IP_PBS_RX_FUNC_PTR)(u32 dev_num);
typedef int (*HWS_TRAINING_IP_PBS_TX_FUNC_PTR)(u32 dev_num);
typedef int (*HWS_TRAINING_IP_SELECT_CONTROLLER_FUNC_PTR)(
	u32 dev_num, int enable);
typedef int (*HWS_TRAINING_IP_TOPOLOGY_MAP_LOAD_FUNC_PTR)(
	u32 dev_num, struct mv_ddr_topology_map *tm);
typedef int (*HWS_TRAINING_IP_STATIC_CONFIG_FUNC_PTR)(
	u32 dev_num, enum hws_ddr_freq frequency,
	enum hws_static_config_type static_config_type, u32 if_id);
typedef int (*HWS_TRAINING_IP_EXTERNAL_READ_PTR)(
	u32 dev_num, u32 if_id, u32 ddr_addr, u32 num_bursts, u32 *data);
typedef int (*HWS_TRAINING_IP_EXTERNAL_WRITE_PTR)(
	u32 dev_num, u32 if_id, u32 ddr_addr, u32 num_bursts, u32 *data);
typedef int (*HWS_TRAINING_IP_BIST_ACTIVATE)(
	u32 dev_num, enum hws_pattern pattern, enum hws_access_type access_type,
	u32 if_num, enum hws_dir direction,
	enum hws_stress_jump addr_stress_jump,
	enum hws_pattern_duration duration,
	enum hws_bist_operation oper_type, u32 offset, u32 cs_num,
	u32 pattern_addr_length);
typedef int (*HWS_TRAINING_IP_BIST_READ_RESULT)(
	u32 dev_num, u32 if_id, struct bist_result *pst_bist_result);
typedef int (*HWS_TRAINING_IP_LOAD_TOPOLOGY)(u32 dev_num, u32 config_num);
typedef int (*HWS_TRAINING_IP_READ_LEVELING)(u32 dev_num, u32 config_num);
typedef int (*HWS_TRAINING_IP_WRITE_LEVELING)(u32 dev_num, u32 config_num);
typedef u32 (*HWS_TRAINING_IP_GET_TEMP)(u8 dev_num);
typedef u8 (*HWS_TRAINING_IP_GET_RATIO)(u32 freq);

struct hws_tip_config_func_db {
	HWS_TIP_DUNIT_MUX_SELECT_FUNC_PTR tip_dunit_mux_select_func;
	HWS_TIP_DUNIT_REG_READ_FUNC_PTR tip_dunit_read_func;
	HWS_TIP_DUNIT_REG_WRITE_FUNC_PTR tip_dunit_write_func;
	HWS_TIP_GET_FREQ_CONFIG_INFO tip_get_freq_config_info_func;
	HWS_TIP_GET_DEVICE_INFO tip_get_device_info_func;
	HWS_SET_FREQ_DIVIDER_FUNC_PTR tip_set_freq_divider_func;
	HWS_GET_CS_CONFIG_FUNC_PTR tip_get_cs_config_info;
	HWS_TRAINING_IP_GET_TEMP tip_get_temperature;
	HWS_TRAINING_IP_GET_RATIO tip_get_clock_ratio;
	HWS_TRAINING_IP_EXTERNAL_READ_PTR tip_external_read;
	HWS_TRAINING_IP_EXTERNAL_WRITE_PTR tip_external_write;
};

int ddr3_tip_init_config_func(u32 dev_num,
			      struct hws_tip_config_func_db *config_func);
int ddr3_tip_register_xsb_info(u32 dev_num,
			       struct hws_xsb_info *xsb_info_table);
enum hws_result *ddr3_tip_get_result_ptr(u32 stage);
int ddr3_set_freq_config_info(struct hws_tip_freq_config_info *table);
int print_device_info(u8 dev_num);

#endif /* _DDR3_TRAINING_IP_PRV_IF_H */
