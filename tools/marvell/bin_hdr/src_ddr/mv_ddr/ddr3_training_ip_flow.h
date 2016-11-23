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

#ifndef _DDR3_TRAINING_IP_FLOW_H_
#define _DDR3_TRAINING_IP_FLOW_H_

#include "ddr3_training_ip.h"
#include "ddr3_training_ip_pbs.h"

#define MRS0_CMD			0x3
#define MRS1_CMD			0x4
#define MRS2_CMD			0x8
#define MRS3_CMD			0x9

/*
 * Definitions of INTERFACE registers
 */

#define READ_BUFFER_SELECT		0x14a4

/*
 * Definitions of PHY registers
 */

#define KILLER_PATTERN_LENGTH		32
#define EXT_ACCESS_BURST_LENGTH		8

#define IS_ACTIVE(mask, id) \
	((mask) & (1 << (id)))

#define VALIDATE_ACTIVE(mask, id)		\
	{					\
	if (IS_ACTIVE(mask, id) == 0)		\
		continue;			\
	}

#define IS_IF_ACTIVE(if_mask, if_id) \
	((if_mask) & (1 << (if_id)))

#define VALIDATE_IF_ACTIVE(mask, id)		\
	{					\
	if (IS_IF_ACTIVE(mask, id) == 0)	\
		continue;			\
	}

#define IS_BUS_ACTIVE(if_mask , if_id) \
	(((if_mask) >> (if_id)) & 1)

#define VALIDATE_BUS_ACTIVE(mask, id)		\
	{					\
	if (IS_BUS_ACTIVE(mask, id) == 0)	\
		continue;			\
	}

#define DDR3_IS_ECC_PUP3_MODE(if_mask) \
	(((if_mask) == BUS_MASK_16BIT_ECC_PUP3) ? 1 : 0)

#define DDR3_IS_ECC_PUP4_MODE(if_mask) \
	((if_mask == BUS_MASK_32BIT_ECC || if_mask == BUS_MASK_16BIT_ECC) ? 1 : 0)

#define DDR3_IS_16BIT_DRAM_MODE(mask) \
	((mask == BUS_MASK_16BIT || mask == BUS_MASK_16BIT_ECC || mask == BUS_MASK_16BIT_ECC_PUP3) ? 1 : 0)

#define DDR3_IS_ECC_PUP8_MODE(if_mask) \
	((if_mask == MV_DDR_32BIT_ECC_PUP8_BUS_MASK || if_mask == MV_DDR_64BIT_ECC_PUP8_BUS_MASK) ? 1 : 0)

#define MV_DDR_IS_64BIT_DRAM_MODE(mask) \
	((((mask) & MV_DDR_64BIT_BUS_MASK) == MV_DDR_64BIT_BUS_MASK) || \
	(((mask) & MV_DDR_64BIT_ECC_PUP8_BUS_MASK) == MV_DDR_64BIT_ECC_PUP8_BUS_MASK) ? 1 : 0)

#define MV_DDR_IS_32BIT_IN_64BIT_DRAM_MODE(mask, octets_per_if_num/* FIXME: get from ATF */) \
	((octets_per_if_num == 9/* FIXME: get from ATF */) && \
	((mask == BUS_MASK_32BIT) || \
	(mask == MV_DDR_32BIT_ECC_PUP8_BUS_MASK)) ? 1 : 0)

#define MV_DDR_IS_HALF_BUS_DRAM_MODE(mask, octets_per_if_num/* FIXME: get from ATF */) \
	(MV_DDR_IS_32BIT_IN_64BIT_DRAM_MODE(mask, octets_per_if_num) || DDR3_IS_16BIT_DRAM_MODE(mask))

#define ECC_READ_BUS_0			0
#define ECC_PHY_ACCESS_3		3
#define ECC_PHY_ACCESS_4		4
#define ECC_PHY_ACCESS_8		8
#define MEGA				1000000
#define BUS_WIDTH_IN_BITS		8

/*
 * DFX address Space
 * Table 2: DFX address space
 * Address Bits   Value   Description
 * [31 : 20]   0x? DFX base address bases PCIe mapping
 * [19 : 15]   0...Number_of_client-1   Client Index inside pipe.
 *             See also Table 1 Multi_cast = 29 Broadcast = 28
 * [14 : 13]   2'b01   Access to Client Internal Register
 * [12 : 0]   Client Internal Register offset   See related Client Registers
 * [14 : 13]   2'b00   Access to Ram Wrappers Internal Register
 * [12 : 6]   0 Number_of_rams-1   Ram Index inside Client
 * [5 : 0]   Ram Wrapper Internal Register offset   See related Ram Wrappers
 * Registers
 */

/* nsec */
#define  TREFI_LOW				7800
#define  TREFI_HIGH				3900

#define  TR2R_VALUE_REG				0x180
#define  TR2R_MASK_REG				0x180
#define  TRFC_MASK_REG				0x7f
#define  TR2W_MASK_REG				0x600
#define  TW2W_HIGH_VALUE_REG			0x1800
#define  TW2W_HIGH_MASK_REG			0xf800
#define  TRFC_HIGH_VALUE_REG			0x20000
#define  TRFC_HIGH_MASK_REG			0x70000
#define  TR2R_HIGH_VALUE_REG			0x0
#define  TR2R_HIGH_MASK_REG			0x380000
#define  TMOD_VALUE_REG				0x16000000
#define  TMOD_MASK_REG				0x1e000000
#define  T_VALUE_REG				0x40000000
#define  T_MASK_REG				0xc0000000
#define  AUTO_ZQC_TIMING			15384
#define  WRITE_XBAR_PORT1			0xc03f8077
#define  READ_XBAR_PORT1			0xc03f8073
#define  DISABLE_DDR_TUNING_DATA		0x02294285
#define  ENABLE_DDR_TUNING_DATA			0x12294285

#define ODPG_TRAINING_TRIGGER_REG		0x1030
#define ODPG_STATUS_DONE_REG			0x16fc

#define ODPG_TRAINING_CONTROL_REG		0x1034
#define ODPG_OBJ1_OPCODE_REG			0x103c
#define ODPG_OBJ1_ITER_CNT_REG			0x10b4
#define CALIB_OBJ_PRFA_REG			0x10c4
#define ODPG_WRITE_LEVELING_DONE_CNTR_REG	0x10f8
#define ODPG_WRITE_READ_MODE_ENABLE_REG		0x10fc
#define TRAINING_OPCODE_1_REG			0x10b4

#define SDRAM_CONFIGURATION_REG			0x1400
#define MV_DDR_REFRESH_OFFS			0
#define MV_DDR_REFRESH_MASK			0x3fff
#define MV_DDR_DRAM_TYPE_OFFS			14
#define MV_DDR_DRAM_TYPE_MASK			0x1
#define MV_DDR_BUS_IN_USE_OFFS			15
#define MV_DDR_BUS_IN_USE_MASK			0x1
#define MV_DDR_CPU_2DRAM_WR_BUFF_CUT_TH_OFFS	16
#define MV_DDR_CPU_2DRAM_WR_BUFF_CUT_TH_MASK	0x1
#define MV_DDR_REG_DIMM_OFFS			17
#define MV_DDR_REG_DIMM_MASK			0x1
#define MV_DDR_ECC_OFFS				18
#define MV_DDR_ECC_MASK				0x1
#define MV_DDR_IGNORE_ERRORS_OFFS		19
#define MV_DDR_IGNORE_ERRORS_MASK		0x1
#define MV_DDR_DRAM_TYPE_HIGH_OFFS		20
#define MV_DDR_DRAM_TYPE_HIGH_MASK		0x1
#define MV_DDR_SELF_REFRESH_MODE_OFFS		24
#define MV_DDR_SELF_REFRESH_MODE_MASK		0x1
#define MV_DDR_CPU_RD_PER_PROP_OFFS		25
#define MV_DDR_CPU_RD_PER_PROP_MASK		0x1
#define MV_DDR_DDR4_EMULATION_OFFS		26
#define MV_DDR_DDR4_EMULATION_MASK		0x1
#define MV_DDR_PHY_RF_RST_OFFS			27
#define MV_DDR_PHY_RF_RST_MASK			0x1
#define MV_DDR_PUP_RST_DIVIDER_OFFS		28
#define MV_DDR_PUP_RST_DIVIDER_MASK		0x1
#define MV_DDR_DATA_PUP_WR_RESET_OFFS		29
#define MV_DDR_DATA_PUP_WR_RESET_MASK		0x1
#define MV_DDR_DATA_PUP_RD_RESET_OFFS		30
#define MV_DDR_DATA_PUP_RD_RESET_MASK		0x1
#define MV_DDR_DATA_PUP_RD_RESET_ENA		0x0
#define MV_DDR_DATA_PUP_RD_RESET_DIS		0x1
#define MV_DDR_IO_BIST_OFFS			31
#define MV_DDR_DATA_PUP_RD_RESET_MASK		0x1

#define DDR_CONTROL_LOW_REG			0x1404

#define SDRAM_TIMING_LOW_REG			0x1408
#define SDRAM_TIMING_LOW_TRAS_OFFS		0
#define SDRAM_TIMING_LOW_TRAS_MASK		0xf
#define SDRAM_TIMING_LOW_TRCD_OFFS		4
#define SDRAM_TIMING_LOW_TRCD_MASK		0xf
#define SDRAM_TIMING_HIGH_TRCD_OFFS		22
#define SDRAM_TIMING_HIGH_TRCD_MASK		0x1
#define SDRAM_TIMING_LOW_TRP_OFFS		8
#define SDRAM_TIMING_LOW_TRP_MASK		0xf
#define SDRAM_TIMING_HIGH_TRP_OFFS		23
#define SDRAM_TIMING_HIGH_TRP_MASK		0x1
#define SDRAM_TIMING_LOW_TWR_OFFS		12
#define SDRAM_TIMING_LOW_TWR_MASK		0xf
#define SDRAM_TIMING_LOW_TWTR_OFFS		16
#define SDRAM_TIMING_LOW_TWTR_MASK		0xf
#define SDRAM_TIMING_LOW_TRAS_HIGH_OFFS		20
#define SDRAM_TIMING_LOW_TRAS_HIGH_MASK		0x3
#define SDRAM_TIMING_LOW_TRRD_OFFS		24
#define SDRAM_TIMING_LOW_TRRD_MASK		0xf
#define SDRAM_TIMING_LOW_TRTP_OFFS		28
#define SDRAM_TIMING_LOW_TRTP_MASK		0xf

#define SDRAM_TIMING_HIGH_REG			0x140c
#define SDRAM_TIMING_HIGH_TRFC_OFFS		0
#define SDRAM_TIMING_HIGH_TRFC_MASK		0x7f
#define SDRAM_TIMING_HIGH_TR2R_OFFS		7
#define SDRAM_TIMING_HIGH_TR2R_MASK		0x3
#define SDRAM_TIMING_HIGH_TR2W_W2R_OFFS		9
#define SDRAM_TIMING_HIGH_TR2W_W2R_MASK		0x3
#define SDRAM_TIMING_HIGH_TW2W_OFFS		11
#define SDRAM_TIMING_HIGH_TW2W_MASK		0x1f
#define SDRAM_TIMING_HIGH_TRFC_HIGH_OFFS	16
#define SDRAM_TIMING_HIGH_TRFC_HIGH_MASK	0x7
#define SDRAM_TIMING_HIGH_TR2R_HIGH_OFFS	19
#define SDRAM_TIMING_HIGH_TR2R_HIGH_MASK	0x7
#define SDRAM_TIMING_HIGH_TR2W_W2R_HIGH_OFFS	22
#define SDRAM_TIMING_HIGH_TR2W_W2R_HIGH_MASK	0x7
#define SDRAM_TIMING_HIGH_TMOD_OFFS		25
#define SDRAM_TIMING_HIGH_TMOD_MASK		0xf
#define SDRAM_TIMING_HIGH_TMOD_HIGH_OFFS	30
#define SDRAM_TIMING_HIGH_TMOD_HIGH_MASK	0x3

#define SDRAM_ACCESS_CONTROL_REG		0x1410
#define MV_DDR_T_FAW_OFFS			24
#define MV_DDR_T_FAW_MASK			0x7f

#define SDRAM_OPEN_PAGE_CONTROL_REG		0x1414
#define SDRAM_OPERATION_REG			0x1418

#define DUNIT_CONTROL_HIGH_REG			0x1424
#define DUNIT_CTRL_HIGH_CPU_INTERJECTION_OFFS	3
#define DUNIT_CTRL_HIGH_CPU_INTERJECTION_MASK	0x1
enum {
	CPU_INTERJECTION_ENABLE_SPLIT,
	CPU_INTERJECTION_DISABLE_SPLIT
};
#define ODT_TIMING_LOW				0x1428

#define DDR_TIMING_REG				0x142c
#define DDR_TIMING_TCCD_OFFS			18
#define DDR_TIMING_TCCD_MASK			0x7
#define DDR_TIMING_TPD_OFFS			0
#define DDR_TIMING_TPD_MASK			0xf
#define DDR_TIMING_TXPDLL_OFFS			4
#define DDR_TIMING_TXPDLL_MASK			0x1f

#define ODT_TIMING_HI_REG			0x147c
#define SDRAM_INIT_CONTROL_REG			0x1480
#define SDRAM_ODT_CONTROL_HIGH_REG		0x1498
#define DUNIT_ODT_CONTROL_REG			0x149c
#define READ_BUFFER_SELECT_REG			0x14a4
#define DUNIT_MMASK_REG				0x14b0

#define CALIB_MACHINE_CTRL_REG			0x14cc
#define MV_DDR_DYN_PADS_CALIB_EN_OFFS		0
#define MV_DDR_DYN_PADS_CALIB_EN_MASK		0x1
enum {
	DYN_PAD_CALIB_DISABLE,
	DYN_PAD_CALIB_ENABLE
};
#define MV_DDR_RECALIBRATE_OFFS			1
#define MV_DDR_RECALIBRATE_MASK			0x1
#define MV_DDR_DYN_PADS_CALIB_BLOCK_OFFS	2
#define MV_DDR_DYN_PADS_CALIB_BLOCK_MASK	0x1
#define MV_DDR_CALIB_UPDATE_CTRL_OFFS		3
#define MV_DDR_CALIB_UPDATE_CTRL_MASK		0x3
enum {
	CALIB_MACHINE_INT_CTRL = 1,
	CALIB_MACHINE_EXT_CTRL = 2
};
#define MV_DDR_DYN_PADS_CALIB_COUNTER_OFFS	13
#define MV_DDR_DYN_PADS_CALIB_COUNTER_MASK	0x3ffff
#define MV_DDR_CALIB_MACHINE_STATUS_OFFS	31
#define MV_DDR_CALIB_MACHINE_STATUS_MASK	0x1
enum {
	CALIB_MACHINE_STATUS_BUSY,
	CALIB_MACHINE_STATUS_READY
};

#define DRAM_DLL_TIMING_REG			0x14e0
#define DRAM_ZQ_INIT_TIMIMG_REG			0x14e4
#define DRAM_ZQ_TIMING_REG			0x14e8
#define DRAM_LONG_TIMING_REG			0x14ec
#define DRAM_LONG_TIMING_DDR4_TRRD_L_OFFS	0
#define DRAM_LONG_TIMING_DDR4_TRRD_L_MASK	0xf
#define DRAM_LONG_TIMING_DDR4_TWTR_L_OFFS	4
#define DRAM_LONG_TIMING_DDR4_TWTR_L_MASK	0xf
#define DFS_REG					0x1528
#define READ_DATA_SAMPLE_DELAY			0x1538
#define READ_DATA_READY_DELAY			0x153c
#define TRAINING_REG				0x15b0
#define TRAINING_TRN_START_OFFS			31
#define TRAINING_TRN_START_MASK			0x1
#define TRAINING_TRN_START_ENA			0x1
#define TRAINING_TRN_START_DIS			0x0
#define TRAINING_SW_1_REG			0x15b4
#define TRAINING_SW_2_REG			0x15b8
#define TRAINING_SW_2_TRN_ECC_MUX_OFFS		1
#define TRAINING_SW_2_TRN_ECC_MUX_MASK		0x1
#define TRAINING_SW_2_TRN_ECC_MUX_ENA		0x1
#define TRAINING_SW_2_TRN_ECC_MUX_DIS		0x0
#define TRAINING_SW_2_TRN_SW_OVRD_OFFS		0
#define TRAINING_SW_2_TRN_SW_OVRD_MASK		0x1
#define TRAINING_SW_2_TRN_SW_OVRD_ENA		0x1
#define TRAINING_SW_2_TRN_SW_OVRD_DIS		0x0
#define TRAINING_PATTERN_BASE_ADDRESS_REG	0x15bc
#define TRAINING_DBG_1_REG			0x15c0
#define TRAINING_DBG_2_REG			0x15c4
#define TRAINING_DBG_3_REG			0x15c8
#define RANK_CTRL_REG				0x15e0
#define TIMING_REG				0x15e4
#define DRAM_PHY_CONFIGURATION			0x15ec
#define MR0_REG					0x15d0
#define MR1_REG					0x15d4
#define MR2_REG					0x15d8
#define MR3_REG					0x15dc
#define TIMING_REG				0x15e4
#define ODPG_CTRL_CONTROL_REG			0x1600

#define ODPG_DATA_CONTROL_REG			0x1630
#define ODPG_DATA_CS_OFFS			26
#define ODPG_DATA_CS_MASK			0x3

#define ODPG_PATTERN_ADDR_OFFSET_REG		0x1638
#define ODPG_DATA_BUF_SIZE_REG			0x163c
#define PHY_LOCK_STATUS_REG			0x1674
#define PHY_REG_FILE_ACCESS			0x16a0
#define TRAINING_WRITE_LEVELING_REG		0x16ac
#define ODPG_PATTERN_ADDR_REG			0x16b0
#define ODPG_PATTERN_DATA_HI_REG		0x16b4
#define ODPG_PATTERN_DATA_LOW_REG		0x16b8
#define ODPG_BIST_LAST_FAIL_ADDR_REG		0x16bc
#define ODPG_BIST_DATA_ERROR_COUNTER_REG	0x16c0
#define ODPG_BIST_FAILED_DATA_HI_REG		0x16c4
#define ODPG_BIST_FAILED_DATA_LOW_REG		0x16c8
#define ODPG_WRITE_DATA_ERROR_REG		0x16cc

#define CS_ENABLE_REG				0x16d8
#define TUNING_ACTIVE_SEL_OFFS			0x6
#define TUNING_ACTIVE_SEL_MASK			0x1
#define FC_SAMPLE_STAGES_OFFS			0
#define FC_SAMPLE_STAGES_MASK			0x7
#define SINGLE_CS_PIN_OFFS			3
#define SINGLE_CS_PIN_MASK			0x1
#define SINGLE_CS_ENA				0x1
enum {
	ENABLE_TIP = 1,
	ENABLE_MCKINLEY = 0,
	USES_SEPARATE = 0,
	USES_SINGLE = 0x1
};

#define WR_LEVELING_DQS_PATTERN_REG			0x16dc

#define ODPG_BIST_DONE				0x186d4
#define ODPG_BIST_DONE_BIT_OFFS			0
#define ODPG_BIST_DONE_BIT_VALUE_REV2		1
#define ODPG_BIST_DONE_BIT_VALUE_REV3		0

#define WL_PHY_BASE				0x0
#define WL_PHY_REG(cs)				(WL_PHY_BASE + (cs) * 0x4)
#define WR_LVL_PH_SEL_OFFS			6
#define WR_LVL_PH_SEL_MASK			0x7
#define WR_LVL_REF_DLY_OFFS			0
#define WR_LVL_REF_DLY_MASK			0x1f
#define CTRL_CENTER_DLY_OFFS			10
#define CTRL_CENTER_DLY_MASK			0x1f
#define CTRL_CENTER_DLY_INV_OFFS		15
#define CTRL_CENTER_DLY_INV_MASK		0x1
enum {
	FIRST_PHASE = 0x1,
	SECOND_PHASE,
	THIRD_PHASE,
	FOURTH_PHASE,
	FIFTH_PHASE,
	SIXTH_PHASE,
	SEVENTH_PHASE
};

#define WRITE_CENTRALIZATION_PHY_REG		0x1
#define RL_PHY_BASE				0x2
#define RL_PHY_REG(cs)				(RL_PHY_BASE + (cs) * 0x4)
#define RL_REF_DLY_OFFS				0
#define RL_REF_DLY_MASK				0x1f
#define RL_PH_SEL_OFFS				6
#define RL_PH_SEL_MASK				0x7
#define READ_CENTRALIZATION_PHY_REG		0x3
#define PBS_RX_PHY_REG				0x50
#define PBS_TX_PHY_REG				0x10
#define PHY_CONTROL_PHY_REG			0x90
#define BW_PHY_REG				0x92
#define RATE_PHY_REG				0x94
#define CMOS_CONFIG_PHY_REG			0xa2
#define PAD_ZRI_CALIB_PHY_REG			0xa4
#define PAD_ODT_CALIB_PHY_REG			0xa6
#define PAD_CONFIG_PHY_REG			0xa8
#define PAD_PRE_DISABLE_PHY_REG			0xa9
#define TEST_ADLL_REG				0xbf
#define CSN_IOB_VREF_REG(cs)			(0xdb + (cs * 12))
#define CSN_IO_BASE_VREF_REG(cs)		(0xd0 + (cs * 12))

enum {
	DQSP_PAD = 4,
	DQSN_PAD
};
#define WR_DESKEW_PHY_BASE			0x10
#define WR_DESKEW_PHY_REG(cs, bit)		(WR_DESKEW_PHY_BASE + (cs) * 0x10 + (bit))
#define WR_DESKEW_BRDCAST_PHY_BASE		0x1f
#define WR_DESKEW_BRDCAST_PHY_REG(cs)		(WR_DESKEW_BRDCAST_PHY_BASE + (cs) * 0x10)
#define RD_DESKEW_PHY_BASE			0x50
#define RD_DESKEW_PHY_REG(cs, bit)		(RD_DESKEW_PHY_BASE + (cs) * 0x10 + (bit))
#define RD_DESKEW_BRDCAST_PHY_BASE		0x5f
#define RD_DESKEW_BRDCAST_PHY_REG(cs)		(RD_DESKEW_BRDCAST_PHY_BASE + (cs) * 0x10)

#define RESULT_DB_PHY_REG_ADDR			0xc0
#define RESULT_DB_PHY_REG_RX_OFFSET		5
#define RESULT_DB_PHY_REG_TX_OFFSET		0

#define DDR0_ADDR_1				0xf8258
#define DDR0_ADDR_2				0xf8254
#define DDR1_ADDR_1				0xf8270
#define DDR1_ADDR_2				0xf8270
#define DDR2_ADDR_1				0xf825c
#define DDR2_ADDR_2				0xf825c
#define DDR3_ADDR_1				0xf8264
#define DDR3_ADDR_2				0xf8260
#define DDR4_ADDR_1				0xf8274
#define DDR4_ADDR_2				0xf8274

#define GENERAL_PURPOSE_RESERVED0_REG		0x182e0

#define GET_BLOCK_ID_MAX_FREQ(dev_num, block_id)	800000
#define CS0_RD_LVL_REF_DLY_OFFS			0
#define CS0_RD_LVL_REF_DLY_LEN			0
#define CS0_RD_LVL_PH_SEL_OFFS			0
#define CS0_RD_LVL_PH_SEL_LEN			0

#define CS_REGISTER_ADDR_OFFSET			4
#define CALIBRATED_OBJECTS_REG_ADDR_OFFSET	0x10

#define MAX_POLLING_ITERATIONS			1000000

#define PHASE_REG_OFFSET			32
#define NUM_BYTES_IN_BURST			31
#define NUM_OF_CS				4
#define CS_BYTE_GAP(cs_num)			((cs_num) * 0x4)
#define CS_PBS_GAP(cs_num)			((cs_num) * 0x10)
#define ADLL_LENGTH				32
#define BITS_IN_BYTE				8

#if defined(CONFIG_DDR4)
/* DDR4 MRS */
#define MRS4_CMD				0x10
#define MRS5_CMD				0x11
#define MRS6_CMD				0x12

/* DDR4 Registers */
#define DDR4_MR0_REG				0x1900
#define DDR4_MR1_REG				0x1904
#define DDR4_MR2_REG				0x1908
#define DDR4_MR3_REG				0x190C
#define DDR4_MR4_REG				0x1910
#define DDR4_MR5_REG				0x1914
#define DDR4_MR6_REG				0x1918
#define DDR4_MPR_WR_REG				0x19D0
#define DRAM_PINS_MUX_REG			0x19D4
#endif /* CONFIG_DDR4 */

enum mr_number {
	MR_CMD0,
	MR_CMD1,
	MR_CMD2,
	MR_CMD3,
#if defined(CONFIG_DDR4)
	MR_CMD4,
	MR_CMD5,
	MR_CMD6,
#endif
	MR_LAST
};

struct mv_ddr_mr_data {
	u32 cmd;
	u32 reg_addr;
};

struct write_supp_result {
	enum hws_wl_supp stage;
	int is_pup_fail;
};

struct page_element {
	enum hws_page_size page_size_8bit;
	/* page size in 8 bits bus width */
	enum hws_page_size page_size_16bit;
	/* page size in 16 bits bus width */
	u32 ui_page_mask;
	/* Mask used in register */
};

int ddr3_tip_write_leveling_static_config(u32 dev_num, u32 if_id,
					  enum hws_ddr_freq frequency,
					  u32 *round_trip_delay_arr);
int ddr3_tip_read_leveling_static_config(u32 dev_num, u32 if_id,
					 enum hws_ddr_freq frequency,
					 u32 *total_round_trip_delay_arr);
int ddr3_tip_if_write(u32 dev_num, enum hws_access_type interface_access,
		      u32 if_id, u32 reg_addr, u32 data_value, u32 mask);
int ddr3_tip_if_polling(u32 dev_num, enum hws_access_type access_type,
			u32 if_id, u32 exp_value, u32 mask, u32 offset,
			u32 poll_tries);
int ddr3_tip_if_read(u32 dev_num, enum hws_access_type interface_access,
		     u32 if_id, u32 reg_addr, u32 *data, u32 mask);
int ddr3_tip_bus_read_modify_write(u32 dev_num,
				   enum hws_access_type access_type,
				   u32 if_id, u32 phy_id,
				   enum hws_ddr_phy phy_type,
				   u32 reg_addr, u32 data_value, u32 reg_mask);
int ddr3_tip_bus_read(u32 dev_num, u32 if_id, enum hws_access_type phy_access,
		      u32 phy_id, enum hws_ddr_phy phy_type, u32 reg_addr,
		      u32 *data);
int ddr3_tip_bus_write(u32 dev_num, enum hws_access_type e_interface_access,
		       u32 if_id, enum hws_access_type e_phy_access, u32 phy_id,
		       enum hws_ddr_phy e_phy_type, u32 reg_addr,
		       u32 data_value);
int ddr3_tip_freq_set(u32 dev_num, enum hws_access_type e_access, u32 if_id,
		      enum hws_ddr_freq memory_freq);
int ddr3_tip_adjust_dqs(u32 dev_num);
int ddr3_tip_init_controller(u32 dev_num);
int ddr3_tip_ext_read(u32 dev_num, u32 if_id, u32 reg_addr,
		      u32 num_of_bursts, u32 *addr);
int ddr3_tip_ext_write(u32 dev_num, u32 if_id, u32 reg_addr,
		       u32 num_of_bursts, u32 *addr);
int ddr3_tip_dynamic_read_leveling(u32 dev_num, u32 ui_freq);
int mv_ddr_rl_dqs_burst(u32 dev_num, u32 if_id, u32 freq);
int ddr3_tip_legacy_dynamic_read_leveling(u32 dev_num);
int ddr3_tip_dynamic_per_bit_read_leveling(u32 dev_num, u32 ui_freq);
int ddr3_tip_legacy_dynamic_write_leveling(u32 dev_num);
int ddr3_tip_dynamic_write_leveling(u32 dev_num, int phase_remove);
int ddr3_tip_dynamic_write_leveling_supp(u32 dev_num);
int ddr3_tip_static_init_controller(u32 dev_num);
int ddr3_tip_configure_phy(u32 dev_num);
int ddr3_tip_load_pattern_to_odpg(u32 dev_num, enum hws_access_type access_type,
				  u32 if_id, enum hws_pattern pattern,
				  u32 load_addr);
int ddr3_tip_load_pattern_to_mem(u32 dev_num, enum hws_pattern e_pattern);
int ddr3_tip_configure_odpg(u32 dev_num, enum hws_access_type access_type,
			    u32 if_id, enum hws_dir direction, u32 tx_phases,
			    u32 tx_burst_size, u32 rx_phases,
			    u32 delay_between_burst, u32 rd_mode, u32 cs_num,
			    u32 addr_stress_jump, u32 single_pattern);
int ddr3_tip_set_atr(u32 dev_num, u32 flag_id, u32 value);
int ddr3_tip_write_mrs_cmd(u32 dev_num, u32 *cs_mask_arr, enum mr_number mr_num, u32 data, u32 mask);
int ddr3_tip_write_cs_result(u32 dev_num, u32 offset);
int ddr3_tip_get_first_active_if(u8 dev_num, u32 interface_mask, u32 *if_id);
int ddr3_tip_reset_fifo_ptr(u32 dev_num);
int ddr3_tip_read_pup_value(u32 dev_num,
			    u32 pup_values[MAX_INTERFACE_NUM * MAX_BUS_NUM],
			    int reg_addr, u32 mask);
int ddr3_tip_read_adll_value(u32 dev_num,
			     u32 pup_values[MAX_INTERFACE_NUM * MAX_BUS_NUM],
			     int reg_addr, u32 mask);
int ddr3_tip_write_adll_value(u32 dev_num,
			      u32 pup_values[MAX_INTERFACE_NUM * MAX_BUS_NUM],
			      int reg_addr);
int ddr3_tip_tune_training_params(u32 dev_num,
				  struct tune_train_params *params);

#endif /* _DDR3_TRAINING_IP_FLOW_H_ */
