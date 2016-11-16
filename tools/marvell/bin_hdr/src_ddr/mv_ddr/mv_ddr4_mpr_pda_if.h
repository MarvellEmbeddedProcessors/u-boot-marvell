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

#ifndef _MV_DDR4_MPR_PDA_IF_H
#define _MV_DDR4_MPR_PDA_IF_H

#include "ddr3_init.h"
#include "mv_ddr_common.h"

#define MV_DDR4_VREF_STEP_SIZE	3
#define MV_DDR4_VREF_MIN_RANGE	1
#define MV_DDR4_VREF_MAX_RANGE	73
#define MV_DDR4_VREF_MAX_COUNT	(((MV_DDR4_VREF_MAX_RANGE - MV_DDR4_VREF_MIN_RANGE) / MV_DDR4_VREF_STEP_SIZE) + 2)

#define MV_DDR4_MPR_READ_PATTERN_NUM	3

enum mv_ddr4_mpr_read_format {
	MV_DDR4_MPR_READ_SERIAL,
	MV_DDR4_MPR_READ_PARALLEL,
	MV_DDR4_MPR_READ_STAGGERED,
	MV_DDR4_MPR_READ_RSVD_TEMP
};

enum mv_ddr4_mpr_read_type {
	MV_DDR4_MPR_READ_RAW,
	MV_DDR4_MPR_READ_DECODED
};

enum mv_ddr4_vref_tap_state {
	MV_DDR4_VREF_TAP_START,
	MV_DDR4_VREF_TAP_BUSY,
	MV_DDR4_VREF_TAP_FLIP,
	MV_DDR4_VREF_TAP_END
};

int mv_ddr4_mode_regs_init(u8 dev_num);
int mv_ddr4_mpr_read(u8 dev_num, u32 mpr_num, u32 page_num,
		     enum mv_ddr4_mpr_read_format read_format,
		     enum mv_ddr4_mpr_read_type read_type,
		     u32 *data);
int mv_ddr4_mpr_write(u8 dev_num, u32 mpr_location, u32 mpr_num,
		      u32 page_num, u32 data);
int mv_ddr4_dq_pins_mapping(u8 dev_num);
int mv_ddr4_vref_training_mode_ctrl(u8 dev_num, u8 if_id,
				 enum hws_access_type access_type,
				 int enable);
int mv_ddr4_vref_tap_set(u8 dev_num, u8 if_id,
			 enum hws_access_type access_type,
			 u32 taps_num,
			 enum mv_ddr4_vref_tap_state state);
int mv_ddr4_vref_set(u8 dev_num, u8 if_id, enum hws_access_type access_type,
		     u32 range, u32 vdq_tv, u8 vdq_training_ena);
int mv_ddr4_pda_pattern_odpg_load(u32 dev_num, enum hws_access_type access_type,
				  u32 if_id, u32 subphy_mask, u32 cs_num);
int mv_ddr4_pda_ctrl(u8 dev_num, u8 if_id, u8 cs_num, int enable);

#endif /* _MV_DDR4_MPR_PDA_IF_H */
