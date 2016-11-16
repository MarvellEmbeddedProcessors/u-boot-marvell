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

#ifndef _DDR3_TRAINING_IP_ENGINE_H_
#define _DDR3_TRAINING_IP_ENGINE_H_

#include "ddr3_training_ip_def.h"
#include "ddr3_training_ip_flow.h"

#define EDGE_1				0
#define EDGE_2				1
#define ALL_PUP_TRAINING		0xe
#define PUP_RESULT_EDGE_1_MASK		0xff
#define PUP_RESULT_EDGE_2_MASK		(0xff << 8)
#define PUP_LOCK_RESULT_BIT		25

#define GET_TAP_RESULT(reg, edge)				 \
	(((edge) == EDGE_1) ? ((reg) & PUP_RESULT_EDGE_1_MASK) : \
	 (((reg) & PUP_RESULT_EDGE_2_MASK) >> 8));
#define GET_LOCK_RESULT(reg)						\
	(((reg) & (1<<PUP_LOCK_RESULT_BIT)) >> PUP_LOCK_RESULT_BIT)

#define EDGE_FAILURE			128
#define ALL_BITS_PER_PUP		128

#define MIN_WINDOW_SIZE			6
#define MAX_WINDOW_SIZE_RX		32
#define MAX_WINDOW_SIZE_TX		64

int ddr3_tip_training_ip_test(u32 dev_num, enum hws_training_result result_type,
			      enum hws_search_dir search_dir,
			      enum hws_dir direction,
			      enum hws_edge_compare edge,
			      u32 init_val1, u32 init_val2,
			      u32 num_of_iterations, u32 start_pattern,
			      u32 end_pattern);
int ddr3_tip_load_pattern_to_mem(u32 dev_num, enum hws_pattern pattern);
int ddr3_tip_load_all_pattern_to_mem(u32 dev_num);
int ddr3_tip_read_training_result(u32 dev_num, u32 if_id,
				  enum hws_access_type pup_access_type,
				  u32 pup_num, u32 bit_num,
				  enum hws_search_dir search,
				  enum hws_dir direction,
				  enum hws_training_result result_type,
				  enum hws_training_load_op operation,
				  u32 cs_num_type, u32 **load_res,
				  int is_read_from_db, u8 cons_tap,
				  int is_check_result_validity);
int ddr3_tip_ip_training(u32 dev_num, enum hws_access_type access_type,
			 u32 interface_num,
			 enum hws_access_type pup_access_type,
			 u32 pup_num, enum hws_training_result result_type,
			 enum hws_control_element control_element,
			 enum hws_search_dir search_dir, enum hws_dir direction,
			 u32 interface_mask, u32 init_value, u32 num_iter,
			 enum hws_pattern pattern,
			 enum hws_edge_compare edge_comp,
			 enum hws_ddr_cs cs_type, u32 cs_num,
			 enum hws_training_ip_stat *train_status);
int ddr3_tip_ip_training_wrapper(u32 dev_num, enum hws_access_type access_type,
				 u32 if_id,
				 enum hws_access_type pup_access_type,
				 u32 pup_num,
				 enum hws_training_result result_type,
				 enum hws_control_element control_element,
				 enum hws_search_dir search_dir,
				 enum hws_dir direction,
				 u32 interface_mask, u32 init_value1,
				 u32 init_value2, u32 num_iter,
				 enum hws_pattern pattern,
				 enum hws_edge_compare edge_comp,
				 enum hws_ddr_cs train_cs_type, u32 cs_num,
				 enum hws_training_ip_stat *train_status);
int is_odpg_access_done(u32 dev_num, u32 if_id);
void ddr3_tip_print_bist_res(void);
struct pattern_info *ddr3_tip_get_pattern_table(void);
u16 *ddr3_tip_get_mask_results_dq_reg(void);
u16 *ddr3_tip_get_mask_results_pup_reg_map(void);

#endif /* _DDR3_TRAINING_IP_ENGINE_H_ */
