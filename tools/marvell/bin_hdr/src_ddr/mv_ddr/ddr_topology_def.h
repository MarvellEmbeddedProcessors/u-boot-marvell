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

#ifndef _DDR_TOPOLOGY_DEF_H
#define _DDR_TOPOLOGY_DEF_H

#include "ddr3_training_ip_def.h"
#include "ddr3_topology_def.h"

#if defined(CONFIG_ARMADA_38X) || defined(CONFIG_ARMADA_39X)
#include "mv_ddr_a38x.h"
#elif defined(CONFIG_APN806)
#include "mv_ddr_apn806.h"
#endif

#include "mv_ddr_topology.h"
#include "mv_ddr_spd.h"

struct bus_params {
	/* Chip Select (CS) bitmask (bits 0-CS0, bit 1- CS1 ...) */
	u8 cs_bitmask;

	/*
	 * mirror enable/disable
	 * (bits 0-CS0 mirroring, bit 1- CS1 mirroring ...)
	 */
	int mirror_enable_bitmask;

	/* DQS Swap (polarity) - true if enable */
	int is_dqs_swap;

	/* CK swap (polarity) - true if enable */
	int is_ck_swap;
};

struct if_params {
	/* bus configuration */
	struct bus_params as_bus_params[MAX_BUS_NUM];

	/* Speed Bin Table */
	enum hws_speed_bin speed_bin_index;

	/* sdram device width */
	enum mv_ddr_dev_width bus_width;

	/* total sdram capacity per die, megabits */
	enum mv_ddr_die_capacity memory_size;

	/* The DDR frequency for each interfaces */
	enum hws_ddr_freq memory_freq;

	/*
	 * delay CAS Write Latency
	 * - 0 for using default value (jedec suggested)
	 */
	u8 cas_wl;

	/*
	 * delay CAS Latency
	 * - 0 for using default value (jedec suggested)
	 */
	u8 cas_l;

	/* operation temperature */
	enum mv_ddr_temperature interface_temp;
};

struct mv_ddr_topology_map {
	/* Number of interfaces (default is 12) */
	u8 if_act_mask;

	/* Controller configuration per interface */
	struct if_params interface_params[MAX_INTERFACE_NUM];

	/* Bit mask for active buses */
	u16 bus_act_mask;

	/* source of ddr configuration data */
	enum mv_ddr_cfg_src cfg_src;

	/* raw spd data */
	union mv_ddr_spd_data spd_data;

	/* timing parameters */
	unsigned int timing_data[MV_DDR_TDATA_LAST];
};

/* DDR3 training global configuration parameters */
struct tune_train_params {
	u32 ck_delay;
	u32 phy_reg3_val;
	u32 g_zpri_data;
	u32 g_znri_data;
	u32 g_zpri_ctrl;
	u32 g_znri_ctrl;
	u32 g_zpodt_data;
	u32 g_znodt_data;
	u32 g_zpodt_ctrl;
	u32 g_znodt_ctrl;
	u32 g_dic;
	u32 g_odt_config;
	u32 g_rtt_nom;
	u32 g_rtt_wr;
	u32 g_rtt_park;
};

#endif /* _DDR_TOPOLOGY_DEF_H */
