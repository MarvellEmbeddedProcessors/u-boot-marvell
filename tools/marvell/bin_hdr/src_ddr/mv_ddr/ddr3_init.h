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

#ifndef _DDR3_INIT_H
#define _DDR3_INIT_H

#if defined(MV_DDR) /* U-BOOT MARVELL 2013.01 */
#include "ddr_mv_wrapper.h"
#elif defined(MV_DDR_ATF) /* MARVELL ATF */
#include "mv_ddr_atf_wrapper.h"
#else /* U-BOOT SPL */
#include "ddr_ml_wrapper.h"
#endif

#if defined(CONFIG_ARMADA_38X) || defined(CONFIG_ARMADA_39X)
#include "mv_ddr_a38x_init.h"
#elif defined(CONFIG_APN806)
#include "mv_ddr_apn806_init.h"
#endif

#include "seq_exec.h"
#include "ddr3_logging_def.h"
#include "ddr3_training_hw_algo.h"
#include "ddr3_training_ip.h"
#include "ddr3_training_ip_centralization.h"
#include "ddr3_training_ip_engine.h"
#include "ddr3_training_ip_flow.h"
#include "ddr3_training_ip_pbs.h"
#include "ddr3_training_ip_prv_if.h"
#include "ddr3_training_ip_static.h"
#include "ddr3_training_leveling.h"
#include "xor.h"

/* For checking function return values */
#define CHECK_STATUS(orig_func)		\
	{				\
		int status;		\
		status = orig_func;	\
		if (MV_OK != status)	\
			return status;	\
	}

#define GET_MAX_VALUE(x, y)			\
	((x) > (y)) ? (x) : (y)

#define SUB_VERSION	0

/* max number of devices supported by driver */
#define MAX_DEVICE_NUM	1

enum log_level  {
	MV_LOG_LEVEL_0,
	MV_LOG_LEVEL_1,
	MV_LOG_LEVEL_2,
	MV_LOG_LEVEL_3
};

/* Globals */
extern u8 debug_training, debug_calibration, debug_ddr4_centralization,
	debug_tap_tuning;
extern u8 is_reg_dump;
extern u8 generic_init_controller;
/* list of allowed frequency listed in order of enum hws_ddr_freq */
extern u32 freq_val[DDR_FREQ_LAST];
extern u32 is_pll_old;
extern struct cl_val_per_freq cas_latency_table[];
extern struct pattern_info pattern_table[];
extern struct cl_val_per_freq cas_write_latency_table[];
extern u8 debug_centralization, debug_training_ip, debug_training_bist,
	debug_pbs, debug_training_static, debug_leveling;
extern struct hws_tip_config_func_db config_func_info[];
extern u8 twr_mask_table[];
extern u8 cl_mask_table[];
extern u8 cwl_mask_table[];
extern u16 rfc_table[];
extern u32 speed_bin_table_t_rc[];
extern u32 speed_bin_table_t_rcd_t_rp[];

extern u32 vref_init_val;
extern u32 g_zpri_data;
extern u32 g_znri_data;
extern u32 g_zpri_ctrl;
extern u32 g_znri_ctrl;
extern u32 g_zpodt_data;
extern u32 g_znodt_data;
extern u32 g_zpodt_ctrl;
extern u32 g_znodt_ctrl;
extern u32 g_dic;
extern u32 g_odt_config;
extern u32 g_rtt_nom;
extern u32 g_rtt_wr;
extern u32 g_rtt_park;

extern u8 debug_training_access;
extern u32 first_active_if;
extern u32 delay_enable, ck_delay, ca_delay;
extern u32 mask_tune_func;
extern u32 rl_version;
extern int rl_mid_freq_wa;
extern u8 calibration_update_control; /* 2 external only, 1 is internal only */
extern enum hws_ddr_freq medium_freq;

extern enum hws_result training_result[MAX_STAGE_LIMIT][MAX_INTERFACE_NUM];
extern enum hws_ddr_freq low_freq;
extern enum auto_tune_stage training_stage;
extern u32 is_pll_before_init;
extern u32 is_adll_calib_before_init;
extern u32 is_dfs_in_init;
extern int wl_debug_delay;
extern u32 silicon_delay[MAX_DEVICE_NUM];
extern u32 start_pattern, end_pattern;
extern u32 phy_reg0_val;
extern u32 phy_reg1_val;
extern u32 phy_reg2_val;
extern u32 phy_reg3_val;
extern enum hws_pattern sweep_pattern;
extern enum hws_pattern pbs_pattern;
extern u32 g_znri_data;
extern u32 g_zpri_data;
extern u32 g_znri_ctrl;
extern u32 g_zpri_ctrl;
extern u32 finger_test, p_finger_start, p_finger_end, n_finger_start,
	n_finger_end, p_finger_step, n_finger_step;
extern u32 mode_2t;
extern u32 xsb_validate_type;
extern u32 xsb_validation_base_address;
extern u32 odt_additional;
extern u32 debug_mode;
extern u32 debug_dunit;
extern u32 clamp_tbl[];
extern u32 freq_mask[MAX_DEVICE_NUM][DDR_FREQ_LAST];

extern u32 maxt_poll_tries;
extern u32 is_bist_reset_bit;

extern u8 vref_window_size[MAX_INTERFACE_NUM][MAX_BUS_NUM];
extern u32 effective_cs;
extern int ddr3_tip_centr_skip_min_win_check;
extern u32 *dq_map_table;

extern u8 debug_training_hw_alg;

extern u32 start_xsb_offset;
extern u32 odt_config;

extern u16 mask_results_dq_reg_map[];

extern u32 target_freq;
extern u32 dfs_low_freq;
extern u32 mem_size[];

extern u32 nominal_avs;
extern u32 extension_avs;

extern struct dlb_config ddr3_dlb_config_table[];

#if defined(CONFIG_DDR4)
/* if 1, SSTL & POD have same Vref and workaround is required */
extern u8 vref_calibration_wa;
#endif /* CONFIG_DDR4 */

/* Prototypes */
int ddr3_init(void);
int ddr3_tip_enable_init_sequence(u32 dev_num);

int ddr3_hws_hw_training(enum hws_algo_type algo_mode);
int ddr3_silicon_pre_init(void);
int ddr3_silicon_post_init(void);
int ddr3_post_run_alg(void);
int ddr3_if_ecc_enabled(void);
void ddr3_new_tip_ecc_scrub(void);

void mv_ddr_ver_print(void);
struct mv_ddr_topology_map *mv_ddr_topology_map_get(void);

int ddr3_if_ecc_enabled(void);
int ddr3_tip_reg_write(u32 dev_num, u32 reg_addr, u32 data);
int ddr3_tip_reg_read(u32 dev_num, u32 reg_addr, u32 *data, u32 reg_mask);
int ddr3_silicon_get_ddr_target_freq(u32 *ddr_freq);
#if defined(CONFIG_DDR4)
int mv_ddr4_mode_regs_init(u8 dev_num);
int mv_ddr4_sdram_config(u32 dev_num);
int mv_ddr4_phy_config(u32 dev_num);
int mv_ddr4_calibration_adjust(u32 dev_num, u8 vref_en, u8 pod_only);
int mv_ddr4_training_main_flow(u32 dev_num);
#endif /* CONFIG_DDR4 */

int print_adll(u32 dev_num, u32 adll[MAX_INTERFACE_NUM * MAX_BUS_NUM]);
int print_ph(u32 dev_num, u32 adll[MAX_INTERFACE_NUM * MAX_BUS_NUM]);
int read_phase_value(u32 dev_num, u32 pup_values[MAX_INTERFACE_NUM * MAX_BUS_NUM],
		     int reg_addr, u32 mask);
int write_leveling_value(u32 dev_num, u32 pup_values[MAX_INTERFACE_NUM * MAX_BUS_NUM],
			 u32 pup_ph_values[MAX_INTERFACE_NUM * MAX_BUS_NUM], int reg_addr);
int ddr3_tip_restore_dunit_regs(u32 dev_num);
void print_topology(struct mv_ddr_topology_map *tm);

u32 mv_board_id_get(void);

int ddr3_load_topology_map(void);
int ddr3_tip_init_specific_reg_config(u32 dev_num,
				      struct reg_data *reg_config_arr);
void ddr3_hws_set_log_level(enum ddr_lib_debug_block block, u8 level);
int ddr3_tip_tune_training_params(u32 dev_num,
				  struct tune_train_params *params);
void get_target_freq(u32 freq_mode, u32 *ddr_freq, u32 *hclk_ps);
void ddr3_fast_path_static_cs_size_config(u32 cs_ena);
u32 mv_board_id_index_get(u32 board_id);
void ddr3_set_log_level(u32 n_log_level);
int calc_cs_num(u32 dev_num, u32 if_id, u32 *cs_num);

int hws_ddr3_cs_base_adr_calc(u32 if_id, u32 cs, u32 *cs_base_addr);

int ddr3_tip_print_pbs_result(u32 dev_num, u32 cs_num, enum pbs_dir pbs_mode);
int ddr3_tip_clean_pbs_result(u32 dev_num, enum pbs_dir pbs_mode);

int ddr3_tip_static_round_trip_arr_build(u32 dev_num,
					 struct trip_delay_element *table_ptr,
					 int is_wl, u32 *round_trip_delay_arr);

u32 mv_ddr_init_freq_get(void);
void mv_ddr_mc_config(void);
int mv_ddr_mc_init(void);
void mv_ddr_set_calib_controller(void);
#endif /* _DDR3_INIT_H */
