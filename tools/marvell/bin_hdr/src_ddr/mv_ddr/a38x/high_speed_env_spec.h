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

#ifndef _HIGH_SPEED_ENV_SPEC_H
#define _HIGH_SPEED_ENV_SPEC_H

#include "seq_exec.h"

/*
 * For setting or clearing a certain bit (bit is a number between 0 and 31)
 * in the data
 */
#define SET_BIT(data, bit)		((data) | (0x1 << (bit)))
#define CLEAR_BIT(data, bit)		((data) & (~(0x1 << (bit))))

#define MAX_SERDES_LANES		7	/* as in a39x */

/* Serdes revision */
/* Serdes revision 1.2 (for A38x-Z1) */
#define MV_SERDES_REV_1_2		0x0
/* Serdes revision 2.1 (for A39x-Z1, A38x-A0) */
#define MV_SERDES_REV_2_1		0x1
#define MV_SERDES_REV_NA		0xff

#define	SERDES_REGS_LANE_BASE_OFFSET(lane)	(0x800 * (lane))

#define PEX_X4_ENABLE_OFFS						\
	(hws_ctrl_serdes_rev_get() == MV_SERDES_REV_1_2 ? 18 : 31)

/* Serdes lane types */
enum serdes_type {
	PEX0,
	PEX1,
	PEX2,
	PEX3,
	SATA0,
	SATA1,
	SATA2,
	SATA3,
	SGMII0,
	SGMII1,
	SGMII2,
	QSGMII,
	USB3_HOST0,
	USB3_HOST1,
	USB3_DEVICE,
	SGMII3,
	XAUI,
	RXAUI,
	DEFAULT_SERDES,
	LAST_SERDES_TYPE
};

/* Serdes baud rates */
enum serdes_speed {
	SERDES_SPEED_1_25_GBPS,
	SERDES_SPEED_1_5_GBPS,
	SERDES_SPEED_2_5_GBPS,
	SERDES_SPEED_3_GBPS,
	SERDES_SPEED_3_125_GBPS,
	SERDES_SPEED_5_GBPS,
	SERDES_SPEED_6_GBPS,
	SERDES_SPEED_6_25_GBPS,
	LAST_SERDES_SPEED
};

/* Serdes modes */
enum serdes_mode {
	PEX_ROOT_COMPLEX_X1,
	PEX_ROOT_COMPLEX_X4,
	PEX_END_POINT_X1,
	PEX_END_POINT_X4,

	SERDES_DEFAULT_MODE, /* not pex */

	SERDES_LAST_MODE
};

struct serdes_map {
	enum serdes_type	serdes_type;
	enum serdes_speed	serdes_speed;
	enum serdes_mode	serdes_mode;
	int			swap_rx;
	int			swap_tx;
};

/* Serdes ref clock options */
enum ref_clock {
	REF_CLOCK_25MHZ,
	REF_CLOCK_100MHZ,
	REF_CLOCK_40MHZ,
	REF_CLOCK_UNSUPPORTED
};

/* Serdes sequences */
enum serdes_seq {
	SATA_PORT_0_ONLY_POWER_UP_SEQ,
	SATA_PORT_1_ONLY_POWER_UP_SEQ,
	SATA_POWER_UP_SEQ,
	SATA_1_5_SPEED_CONFIG_SEQ,
	SATA_3_SPEED_CONFIG_SEQ,
	SATA_6_SPEED_CONFIG_SEQ,
	SATA_ELECTRICAL_CONFIG_SEQ,
	SATA_TX_CONFIG_SEQ1,
	SATA_PORT_0_ONLY_TX_CONFIG_SEQ,
	SATA_PORT_1_ONLY_TX_CONFIG_SEQ,
	SATA_TX_CONFIG_SEQ2,

	SGMII_POWER_UP_SEQ,
	SGMII_1_25_SPEED_CONFIG_SEQ,
	SGMII_3_125_SPEED_CONFIG_SEQ,
	SGMII_ELECTRICAL_CONFIG_SEQ,
	SGMII_TX_CONFIG_SEQ1,
	SGMII_TX_CONFIG_SEQ2,

	PEX_POWER_UP_SEQ,
	PEX_2_5_SPEED_CONFIG_SEQ,
	PEX_5_SPEED_CONFIG_SEQ,
	PEX_ELECTRICAL_CONFIG_SEQ,
	PEX_TX_CONFIG_SEQ1,
	PEX_TX_CONFIG_SEQ2,
	PEX_TX_CONFIG_SEQ3,
	PEX_BY_4_CONFIG_SEQ,
	PEX_CONFIG_REF_CLOCK_25MHZ_SEQ,
	PEX_CONFIG_REF_CLOCK_100MHZ_SEQ,
	PEX_CONFIG_REF_CLOCK_40MHZ_SEQ,

	USB3_POWER_UP_SEQ,
	USB3_HOST_SPEED_CONFIG_SEQ,
	USB3_DEVICE_SPEED_CONFIG_SEQ,
	USB3_ELECTRICAL_CONFIG_SEQ,
	USB3_TX_CONFIG_SEQ1,
	USB3_TX_CONFIG_SEQ2,
	USB3_TX_CONFIG_SEQ3,
	USB3_DEVICE_CONFIG_SEQ,

	USB2_POWER_UP_SEQ,

	SERDES_POWER_DOWN_SEQ,

	SGMII3_POWER_UP_SEQ,
	SGMII3_1_25_SPEED_CONFIG_SEQ,
	SGMII3_TX_CONFIG_SEQ1,
	SGMII3_TX_CONFIG_SEQ2,

	QSGMII_POWER_UP_SEQ,
	QSGMII_5_SPEED_CONFIG_SEQ,
	QSGMII_ELECTRICAL_CONFIG_SEQ,
	QSGMII_TX_CONFIG_SEQ1,
	QSGMII_TX_CONFIG_SEQ2,

	XAUI_POWER_UP_SEQ,
	XAUI_3_125_SPEED_CONFIG_SEQ,
	XAUI_ELECTRICAL_CONFIG_SEQ,
	XAUI_TX_CONFIG_SEQ1,
	XAUI_TX_CONFIG_SEQ2,

	RXAUI_POWER_UP_SEQ,
	RXAUI_6_25_SPEED_CONFIG_SEQ,
	RXAUI_ELECTRICAL_CONFIG_SEQ,
	RXAUI_TX_CONFIG_SEQ1,
	RXAUI_TX_CONFIG_SEQ2,

	SERDES_LAST_SEQ
};

/* The different sequence types for PEX and USB3 */
enum {
	PEX,
	USB3,
	LAST_PEX_USB_SEQ_TYPE
};

enum {
	PEXSERDES_SPEED_2_5_GBPS,
	PEXSERDES_SPEED_5_GBPS,
	USB3SERDES_SPEED_5_GBPS_HOST,
	USB3SERDES_SPEED_5_GBPS_DEVICE,
	LAST_PEX_USB_SPEED_SEQ_TYPE
};

/* The different sequence types for SATA and SGMII */
enum {
	SATA,
	SGMII,
	SGMII_3_125,
	LAST_SATA_SGMII_SEQ_TYPE
};

enum {
	QSGMII_SEQ_IDX,
	LAST_QSGMII_SEQ_TYPE
};

enum {
	XAUI_SEQ_IDX,
	RXAUI_SEQ_IDX,
	LAST_XAUI_RXAUI_SEQ_TYPE
};

enum {
	SATASERDES_SPEED_1_5_GBPS,
	SATASERDES_SPEED_3_GBPS,
	SATASERDES_SPEED_6_GBPS,
	SGMIISERDES_SPEED_1_25_GBPS,
	SGMIISERDES_SPEED_3_125_GBPS,
	LAST_SATA_SGMII_SPEED_SEQ_TYPE
};

extern u8 selectors_serdes_rev1_map[LAST_SERDES_TYPE][MAX_SERDES_LANES];
extern u8 selectors_serdes_rev2_map[LAST_SERDES_TYPE][MAX_SERDES_LANES];

u8 hws_ctrl_serdes_rev_get(void);
int mv_update_serdes_select_phy_mode_seq(void);
int hws_board_topology_load(struct serdes_map **serdes_map, u8 *count);
enum serdes_seq serdes_type_and_speed_to_speed_seq(enum serdes_type serdes_type,
						   enum serdes_speed baud_rate);
int hws_serdes_seq_init(void);
int hws_serdes_seq_db_init(void);
int hws_power_up_serdes_lanes(struct serdes_map *serdes_map, u8 count);
int hws_ctrl_high_speed_serdes_phy_config(void);
int serdes_power_up_ctrl(u32 serdes_num, int serdes_power_up,
			 enum serdes_type serdes_type,
			 enum serdes_speed baud_rate,
			 enum serdes_mode serdes_mode,
			 enum ref_clock ref_clock);
int serdes_power_up_ctrl_ext(u32 serdes_num, int serdes_power_up,
			     enum serdes_type serdes_type,
			     enum serdes_speed baud_rate,
			     enum serdes_mode serdes_mode,
			     enum ref_clock ref_clock);
u32 hws_serdes_silicon_ref_clock_get(void);
int hws_serdes_pex_ref_clock_get(enum serdes_type serdes_type,
				 enum ref_clock *ref_clock);
int hws_ref_clock_set(u32 serdes_num, enum serdes_type serdes_type,
		      enum ref_clock ref_clock);
int hws_update_serdes_phy_selectors(struct serdes_map *serdes_map, u8 count);
u32 hws_serdes_get_phy_selector_val(int serdes_num,
				    enum serdes_type serdes_type);
u32 hws_serdes_get_ref_clock_val(enum serdes_type serdes_type);
u32 hws_serdes_get_max_lane(void);
int hws_get_ext_base_addr(u32 serdes_num, u32 base_addr, u32 unit_base_offset,
			  u32 *unit_base_reg, u32 *unit_offset);
int hws_pex_tx_config_seq(const struct serdes_map *serdes_map, u8 count);
u32 hws_get_physical_serdes_num(u32 serdes_num);
int hws_is_serdes_active(u8 lane_num);

#endif /* _HIGH_SPEED_ENV_SPEC_H */
