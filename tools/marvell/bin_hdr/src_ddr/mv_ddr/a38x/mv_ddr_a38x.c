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

#include "sys_env_lib.h"

#if defined(CONFIG_PHY_STATIC)
#include "mv_ddr_a38x_phy_static.h"
#endif
#if defined(CONFIG_MC_STATIC)
#include "mv_ddr_a38x_mc_static.h"
#endif

#define DDR_INTERFACES_NUM		1
#define DDR_INTERFACE_OCTETS_NUM	5

#define SAR_DEV_ID_OFFS			27
#define SAR_DEV_ID_MASK			0x7

/* Termal Sensor Registers */
#define TSEN_CONTROL_LSB_REG		0xE4070
#define TSEN_CONTROL_LSB_TC_TRIM_OFFSET	0
#define TSEN_CONTROL_LSB_TC_TRIM_MASK	(0x7 << TSEN_CONTROL_LSB_TC_TRIM_OFFSET)
#define TSEN_CONTROL_MSB_REG		0xE4074
#define TSEN_CONTROL_MSB_RST_OFFSET	8
#define TSEN_CONTROL_MSB_RST_MASK	(0x1 << TSEN_CONTROL_MSB_RST_OFFSET)
#define TSEN_STATUS_REG			0xe4078
#define TSEN_STATUS_READOUT_VALID_OFFSET	10
#define TSEN_STATUS_READOUT_VALID_MASK	(0x1 <<				\
					 TSEN_STATUS_READOUT_VALID_OFFSET)
#define TSEN_STATUS_TEMP_OUT_OFFSET	0
#define TSEN_STATUS_TEMP_OUT_MASK	(0x3ff << TSEN_STATUS_TEMP_OUT_OFFSET)

#if defined(CONFIG_DDR4)
struct dlb_config ddr3_dlb_config_table[] = {
	{REG_STATIC_DRAM_DLB_CONTROL, 0x2000005f},
	{DLB_BUS_OPTIMIZATION_WEIGHTS_REG, 0x00880000},
	{DLB_AGING_REGISTER, 0x3f7f007f},
	{DLB_EVICTION_CONTROL_REG, 0x0000129f},
	{DLB_EVICTION_TIMERS_REGISTER_REG, 0x00ff0000},
	{DLB_BUS_WEIGHTS_DIFF_CS, 0x04030803},
	{DLB_BUS_WEIGHTS_DIFF_BG, 0x00000A02},
	{DLB_BUS_WEIGHTS_SAME_BG, 0x08000901},
	{DLB_BUS_WEIGHTS_RD_WR,  0x00020005},
	{DLB_BUS_WEIGHTS_ATTR_SYS_PRIO, 0x00060f10},
	{DLB_MAIN_QUEUE_MAP, 0x00000543},
	{DLB_LINE_SPLIT, 0x0000000f},
	{DLB_USER_COMMAND_REG, 0x00000000},
	{0x0, 0x0}
};
#else /* CONFIG_DDR4 */
struct dlb_config ddr3_dlb_config_table[] = {
	{REG_STATIC_DRAM_DLB_CONTROL, 0x2000005c},
	{DLB_BUS_OPTIMIZATION_WEIGHTS_REG, 0x00880000},
	{DLB_AGING_REGISTER, 0x0f7f007f},
	{DLB_EVICTION_CONTROL_REG, 0x0000129f},
	{DLB_EVICTION_TIMERS_REGISTER_REG, 0x00ff0000},
	{DLB_BUS_WEIGHTS_DIFF_CS, 0x04030802},
	{DLB_BUS_WEIGHTS_DIFF_BG, 0x00000a02},
	{DLB_BUS_WEIGHTS_SAME_BG, 0x09000a01},
	{DLB_BUS_WEIGHTS_RD_WR, 0x00020005},
	{DLB_BUS_WEIGHTS_ATTR_SYS_PRIO, 0x00060f10},
	{DLB_MAIN_QUEUE_MAP, 0x00000543},
	{DLB_LINE_SPLIT, 0x00000000},
	{DLB_USER_COMMAND_REG, 0x00000000},
	{0x0, 0x0}
};
#endif /* CONFIG_DDR4 */

#if defined(CONFIG_PHY_STATIC) || defined(CONFIG_MC_STATIC)
struct dram_modes {
	char *mode_name;
	u8 cpu_freq;
	struct reg_data *mc_regs;
	struct mv_ddr_subphys_reg_config *ctrl_phy_regs;
	struct mv_ddr_subphys_reg_config *data_phy_regs;
};

static struct dram_modes ddr_modes[] = {
	/* Conf name, DDR Frequency, MC regs, PHY cntrl, PHY data */
#if !defined(CONFIG_DDR4)
	{"a38x_600", DDR_FREQ_600, a38x_mc_600, a38x_ctrl_phy_600, a38x_data_phy_600},
#endif
	{"a38x_800", DDR_FREQ_800, a38x_mc_800, a38x_ctrl_phy_800, a38x_data_phy_800},
	{"", DDR_FREQ_LAST, NULL, NULL, NULL}
};
#endif /* SUPPORT_STATIC_DUNIT_CONFIG */

#ifdef STATIC_ALGO_SUPPORT
/* This array hold the board round trip delay (DQ and CK) per <interface,bus> */
struct trip_delay_element a38x_board_round_trip_delay_array[] = {
	/* 1st board */
	/* Interface bus DQS-delay CK-delay */
	{ 3952, 5060 },
	{ 3192, 4493 },
	{ 4785, 6677 },
	{ 3413, 7267 },
	{ 4282, 6086 },	/* ECC PUP */
	{ 3952, 5134 },
	{ 3192, 4567 },
	{ 4785, 6751 },
	{ 3413, 7341 },
	{ 4282, 6160 },	/* ECC PUP */

	/* 2nd board */
	/* Interface bus DQS-delay CK-delay */
	{ 3952, 5060 },
	{ 3192, 4493 },
	{ 4785, 6677 },
	{ 3413, 7267 },
	{ 4282, 6086 },	/* ECC PUP */
	{ 3952, 5134 },
	{ 3192, 4567 },
	{ 4785, 6751 },
	{ 3413, 7341 },
	{ 4282, 6160 }	/* ECC PUP */
};

/* package trace */
static struct trip_delay_element a38x_package_round_trip_delay_array[] = {
	/* IF BUS DQ_DELAY CK_DELAY */
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 }
};

static int a38x_silicon_delay_offset[] = {
	/* board 0 */
	0,
	/* board 1 */
	0,
	/* board 2 */
	0
};
#endif /* STATIC_ALGO_SUPPORT */

static u8 a38x_bw_per_freq[DDR_FREQ_LAST] = {
	0x3,			/* DDR_FREQ_100 */
#if !defined(CONFIG_DDR4)
	0x4,			/* DDR_FREQ_400 */
	0x4,			/* DDR_FREQ_533 */
#endif /* CONFIG_DDR4 */
	0x5,			/* DDR_FREQ_667 */
	0x5,			/* DDR_FREQ_800 */
	0x5,			/* DDR_FREQ_933 */
	0x5,			/* DDR_FREQ_1066 */
#if defined(CONFIG_DDR4)
	0x5,			/*DDR_FREQ_900*/
	0x5,			/*DDR_FREQ_1000*/
#else /* CONFIG_DDR4 */
	0x3,			/* DDR_FREQ_311 */
	0x3,			/* DDR_FREQ_333 */
	0x4,			/* DDR_FREQ_467 */
	0x5,			/* DDR_FREQ_850 */
	0x5,			/* DDR_FREQ_600 */
	0x3,			/* DDR_FREQ_300 */
	0x5,			/* DDR_FREQ_900 */
	0x3,			/* DDR_FREQ_360 */
	0x5			/* DDR_FREQ_1000 */
#endif /* CONFIG_DDR4 */
};

static u8 a38x_rate_per_freq[DDR_FREQ_LAST] = {
	0x1,			/* DDR_FREQ_100 */
#if !defined(CONFIG_DDR4)
	0x2,			/* DDR_FREQ_400 */
	0x2,			/* DDR_FREQ_533 */
#endif /* CONFIG_DDR4 */
	0x2,			/* DDR_FREQ_667 */
	0x2,			/* DDR_FREQ_800 */
	0x3,			/* DDR_FREQ_933 */
	0x3,			/* DDR_FREQ_1066 */
#ifdef CONFIG_DDR4
	0x2,			/*DDR_FREQ_900*/
	0x2,			/*DDR_FREQ_1000*/
#else /* CONFIG_DDR4 */
	0x1,			/* DDR_FREQ_311 */
	0x1,			/* DDR_FREQ_333 */
	0x2,			/* DDR_FREQ_467 */
	0x2,			/* DDR_FREQ_850 */
	0x2,			/* DDR_FREQ_600 */
	0x1,			/* DDR_FREQ_300 */
	0x2,			/* DDR_FREQ_900 */
	0x1,			/* DDR_FREQ_360 */
	0x2			/* DDR_FREQ_1000 */
#endif /* CONFIG_DDR4 */
};

static u16 a38x_vco_freq_per_sar_ref_clk_25_mhz[] = {
	666,			/* 0 */
	1332,
	800,
	1600,
	1066,
	2132,
	1200,
	2400,
	1332,
	1332,
	1500,
	1500,
	1600,			/* 12 */
	1600,
	1700,
	1700,
	1866,
	1866,
	1800,			/* 18 */
	2000,
	2000,
	4000,
	2132,
	2132,
	2300,
	2300,
	2400,
	2400,
	2500,
	2500,
	800
};

static u16 a38x_vco_freq_per_sar_ref_clk_40_mhz[] = {
	666,			/* 0 */
	1332,
	800,
	800,			/* 0x3 */
	1066,
	1066,			/* 0x5 */
	1200,
	2400,
	1332,
	1332,
	1500,			/* 10 */
	1600,			/* 0xB */
	1600,
	1600,
	1700,
	1560,			/* 0xF */
	1866,
	1866,
	1800,
	2000,
	2000,			/* 20 */
	4000,
	2132,
	2132,
	2300,
	2300,
	2400,
	2400,
	2500,
	2500,
	1800			/* 30 - 0x1E */
};

#if defined(CONFIG_DDR4)
u16 odt_slope[] = {
	21443,
	1452,
	482,
	240,
	141,
	90,
	67,
	52
};

u16 odt_intercept[] = {
	1517,
	328,
	186,
	131,
	100,
	80,
	69,
	61
};

/* Map of scratch PHY registers used to store stability value */
u32 dmin_phy_reg_table[MAX_BUS_NUM * MAX_CS_NUM][2] = {
	/* subphy, addr */
	{0, 0xc0},	/* cs 0, subphy 0 */
	{0, 0xc1},	/* cs 0, subphy 1 */
	{0, 0xc2},	/* cs 0, subphy 2 */
	{0, 0xc3},	/* cs 0, subphy 3 */
	{0, 0xc4},	/* cs 0, subphy 4 */
	{1, 0xc0},	/* cs 1, subphy 0 */
	{1, 0xc1},	/* cs 1, subphy 1 */
	{1, 0xc2},	/* cs 1, subphy 2 */
	{1, 0xc3},	/* cs 1, subphy 3 */
	{1, 0xc4},	/* cs 1, subphy 4 */
	{2, 0xc0},	/* cs 2, subphy 0 */
	{2, 0xc1},	/* cs 2, subphy 1 */
	{2, 0xc2},	/* cs 2, subphy 2 */
	{2, 0xc3},	/* cs 2, subphy 3 */
	{2, 0xc4},	/* cs 2, subphy 4 */
	{0, 0xc5},	/* cs 3, subphy 0 */
	{1, 0xc5},	/* cs 3, subphy 1 */
	{2, 0xc5},	/* cs 3, subphy 2 */
	{0, 0xc6},	/* cs 3, subphy 3 */
	{1, 0xc6}	/* cs 3, subphy 4 */
};
#endif /* CONFIG_DDR4 */

static u32 async_mode_at_tf;

static u32 dq_bit_map_2_phy_pin[] = {
	1, 0, 2, 6, 9, 8, 3, 7,	/* 0 */
	8, 9, 1, 7, 2, 6, 3, 0,	/* 1 */
	3, 9, 7, 8, 1, 0, 2, 6,	/* 2 */
	1, 0, 6, 2, 8, 3, 7, 9,	/* 3 */
	0, 1, 2, 9, 7, 8, 3, 6,	/* 4 */
};

void mv_ddr_mem_scrubbing(void)
{
}

static int ddr3_tip_a38x_set_divider(u8 dev_num, u32 if_id,
				     enum hws_ddr_freq freq);

/*
 * Read temperature TJ value
 */
static u32 ddr3_ctrl_get_junc_temp(u8 dev_num)
{
	int reg = 0;

	/* Initiates TSEN hardware reset once */
	if ((reg_read(TSEN_CONTROL_MSB_REG) & TSEN_CONTROL_MSB_RST_MASK) == 0) {
		reg_bit_set(TSEN_CONTROL_MSB_REG, TSEN_CONTROL_MSB_RST_MASK);
		/* set Tsen Tc Trim to correct default value (errata #132698) */
		reg = reg_read(TSEN_CONTROL_LSB_REG);
		reg &= ~TSEN_CONTROL_LSB_TC_TRIM_MASK;
		reg |= 0x3 << TSEN_CONTROL_LSB_TC_TRIM_OFFSET;
		reg_write(TSEN_CONTROL_LSB_REG, reg);
	}
	mdelay(10);

	/* Check if the readout field is valid */
	if ((reg_read(TSEN_STATUS_REG) & TSEN_STATUS_READOUT_VALID_MASK) == 0) {
		printf("%s: TSEN not ready\n", __func__);
		return 0;
	}

	reg = reg_read(TSEN_STATUS_REG);
	reg = (reg & TSEN_STATUS_TEMP_OUT_MASK) >> TSEN_STATUS_TEMP_OUT_OFFSET;

	return ((((10000 * reg) / 21445) * 1000) - 272674) / 1000;
}

/*
 * Name:     ddr3_tip_a38x_get_freq_config.
 * Desc:
 * Args:
 * Notes:
 * Returns:  MV_OK if success, other error code if fail.
 */
static int ddr3_tip_a38x_get_freq_config(u8 dev_num, enum hws_ddr_freq freq,
				  struct hws_tip_freq_config_info
				  *freq_config_info)
{
	if (a38x_bw_per_freq[freq] == 0xff)
		return MV_NOT_SUPPORTED;

	if (freq_config_info == NULL)
		return MV_BAD_PARAM;

	freq_config_info->bw_per_freq = a38x_bw_per_freq[freq];
	freq_config_info->rate_per_freq = a38x_rate_per_freq[freq];
	freq_config_info->is_supported = 1;

	return MV_OK;
}

/*
 * Name:     ddr3_tip_a38x_if_read.
 * Desc:
 * Args:
 * Notes:
 * Returns:  MV_OK if success, other error code if fail.
 */
static int ddr3_tip_a38x_if_read(u8 dev_num, enum hws_access_type interface_access,
			  u32 if_id, u32 reg_addr, u32 *data, u32 mask)
{
	*data = reg_read(reg_addr) & mask;

	return MV_OK;
}

/*
 * Name:     ddr3_tip_a38x_if_write.
 * Desc:
 * Args:
 * Notes:
 * Returns:  MV_OK if success, other error code if fail.
 */
static int ddr3_tip_a38x_if_write(u8 dev_num, enum hws_access_type interface_access,
			   u32 if_id, u32 reg_addr, u32 data_value,
			   u32 mask)
{
	u32 ui_data_read;

	if (mask != MASK_ALL_BITS) {
		CHECK_STATUS(ddr3_tip_a38x_if_read
			     (dev_num, ACCESS_TYPE_UNICAST, if_id, reg_addr,
			      &ui_data_read, MASK_ALL_BITS));
		data_value = (ui_data_read & (~mask)) | (data_value & mask);
	}

	reg_write(reg_addr, data_value);

	return MV_OK;
}

/*
 * Name:     ddr3_tip_a38x_select_ddr_controller.
 * Desc:     Enable/Disable access to Marvell's server.
 * Args:     dev_num     - device number
 *           enable        - whether to enable or disable the server
 * Notes:
 * Returns:  MV_OK if success, other error code if fail.
 */
static int ddr3_tip_a38x_select_ddr_controller(u8 dev_num, int enable)
{
	u32 reg;

	reg = reg_read(CS_ENABLE_REG);

	if (enable)
		reg |= (1 << 6);
	else
		reg &= ~(1 << 6);

	reg_write(CS_ENABLE_REG, reg);

	return MV_OK;
}

static u8 ddr3_tip_clock_mode(u32 frequency)
{
	if ((frequency == DDR_FREQ_LOW_FREQ) || (freq_val[frequency] <= 400))
		return 1;

	return 2;
}

static int mv_ddr_sar_freq_get(int dev_num, enum hws_ddr_freq *freq)
{
	u32 reg, ref_clk_satr;

	/* Read sample at reset setting */
	reg = (reg_read(REG_DEVICE_SAR1_ADDR) >>
	       RST2_CPU_DDR_CLOCK_SELECT_IN_OFFSET) &
		RST2_CPU_DDR_CLOCK_SELECT_IN_MASK;

	ref_clk_satr = reg_read(DEVICE_SAMPLE_AT_RESET2_REG);
	if (((ref_clk_satr >> DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_OFFSET) & 0x1) ==
	    DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_25MHZ) {
		switch (reg) {
#if !defined(CONFIG_DDR4)
		case 0x1:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR,
					      ("Warning: Unsupported freq mode for 333Mhz configured(%d)\n",
					      reg));
		case 0x0:
			*freq = DDR_FREQ_333;
			break;
		case 0x3:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR,
					      ("Warning: Unsupported freq mode for 400Mhz configured(%d)\n",
					      reg));
		case 0x2:
			*freq = DDR_FREQ_400;
			break;
		case 0xd:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR,
					      ("Warning: Unsupported freq mode for 533Mhz configured(%d)\n",
					      reg));
		case 0x4:
			*freq = DDR_FREQ_533;
			break;
		case 0x6:
			*freq = DDR_FREQ_600;
			break;
#endif /* CONFIG_DDR4 */
		case 0x11:
		case 0x14:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR,
					      ("Warning: Unsupported freq mode for 667Mhz configured(%d)\n",
					      reg));
		case 0x8:
			*freq = DDR_FREQ_667;
			break;
		case 0x15:
		case 0x1b:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR,
					      ("Warning: Unsupported freq mode for 800Mhz configured(%d)\n",
					      reg));
		case 0xc:
			*freq = DDR_FREQ_800;
			break;
		case 0x10:
			*freq = DDR_FREQ_933;
			break;
		case 0x12:
			*freq = DDR_FREQ_900;
			break;
#if defined(CONFIG_DDR4)
		case 0x13:
			*freq = DDR_FREQ_1000;
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR,
					      ("Warning: Unsupported freq mode for 1000Mhz configured(%d)\n",
					      reg));
			break;
#else /* CONFIG_DDR4 */
		case 0x13:
			*freq = DDR_FREQ_933;
			break;
#endif /* CONFIG_DDR4 */
		default:
			*freq = 0;
			return MV_NOT_SUPPORTED;
		}
	} else { /* REFCLK 40MHz case */
		switch (reg) {
#if !defined(CONFIG_DDR4)
		case 0x3:
			*freq = DDR_FREQ_400;
			break;
		case 0x5:
			*freq = DDR_FREQ_533;
			break;
#endif /* CONFIG_DDR4 */
		case 0xb:
			*freq = DDR_FREQ_800;
			break;
		case 0x1e:
			*freq = DDR_FREQ_900;
			break;
		default:
			*freq = 0;
			return MV_NOT_SUPPORTED;
		}
	}

	return MV_OK;
}

#if !defined(CONFIG_DDR4)
static int ddr3_tip_a38x_get_medium_freq(int dev_num, enum hws_ddr_freq *freq)
{
	u32 reg, ref_clk_satr;

	/* Read sample at reset setting */
	reg = (reg_read(REG_DEVICE_SAR1_ADDR) >>
	RST2_CPU_DDR_CLOCK_SELECT_IN_OFFSET) &
	RST2_CPU_DDR_CLOCK_SELECT_IN_MASK;

	ref_clk_satr = reg_read(DEVICE_SAMPLE_AT_RESET2_REG);
	if (((ref_clk_satr >> DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_OFFSET) & 0x1) ==
	    DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_25MHZ) {
		switch (reg) {
		case 0x0:
		case 0x1:
			/* Medium is same as TF to run PBS in this freq */
			*freq = DDR_FREQ_333;
			break;
		case 0x2:
		case 0x3:
			/* Medium is same as TF to run PBS in this freq */
			*freq = DDR_FREQ_400;
			break;
		case 0x4:
		case 0xd:
			/* Medium is same as TF to run PBS in this freq */
			*freq = DDR_FREQ_533;
			break;
		case 0x8:
		case 0x10:
		case 0x11:
		case 0x14:
			*freq = DDR_FREQ_333;
			break;
		case 0xc:
		case 0x15:
		case 0x1b:
			*freq = DDR_FREQ_400;
			break;
		case 0x6:
			*freq = DDR_FREQ_300;
			break;
		case 0x12:
			*freq = DDR_FREQ_360;
			break;
		case 0x13:
			*freq = DDR_FREQ_400;
			break;
		default:
			*freq = 0;
			return MV_NOT_SUPPORTED;
		}
	} else { /* REFCLK 40MHz case */
		switch (reg) {
		case 0x3:
			/* Medium is same as TF to run PBS in this freq */
			*freq = DDR_FREQ_400;
			break;
		case 0x5:
			/* Medium is same as TF to run PBS in this freq */
			*freq = DDR_FREQ_533;
			break;
		case 0xb:
			*freq = DDR_FREQ_400;
			break;
		case 0x1e:
			*freq = DDR_FREQ_360;
			break;
		default:
			*freq = 0;
			return MV_NOT_SUPPORTED;
		}
	}

	return MV_OK;
}
#endif /* CONFIG_DDR4 */

static int ddr3_tip_a38x_get_device_info(u8 dev_num, struct ddr3_device_info *info_ptr)
{
#if defined(CONFIG_ARMADA_39X)
	info_ptr->device_id = 0x6900;
#else
	info_ptr->device_id = 0x6800;
#endif
	info_ptr->ck_delay = ck_delay;

	return MV_OK;
}

static int mv_ddr_sw_db_init(u32 dev_num, u32 board_id)
{
	struct hws_tip_config_func_db config_func;

	/* new read leveling version */
	config_func.tip_dunit_read_func = ddr3_tip_a38x_if_read;
	config_func.tip_dunit_write_func = ddr3_tip_a38x_if_write;
	config_func.tip_dunit_mux_select_func =
		ddr3_tip_a38x_select_ddr_controller;
	config_func.tip_get_freq_config_info_func =
		ddr3_tip_a38x_get_freq_config;
	config_func.tip_set_freq_divider_func = ddr3_tip_a38x_set_divider;
	config_func.tip_get_device_info_func = ddr3_tip_a38x_get_device_info;
	config_func.tip_get_temperature = ddr3_ctrl_get_junc_temp;
	config_func.tip_get_clock_ratio = ddr3_tip_clock_mode;
	config_func.tip_external_read = ddr3_tip_ext_read;
	config_func.tip_external_write = ddr3_tip_ext_write;

	ddr3_tip_init_config_func(dev_num, &config_func);

	ddr3_tip_register_dq_table(dev_num, dq_bit_map_2_phy_pin);

	/* set device attributes*/
	ddr3_tip_dev_attr_init(dev_num);
	ddr3_tip_dev_attr_set(dev_num, MV_ATTR_TIP_REV, MV_TIP_REV_4);
	ddr3_tip_dev_attr_set(dev_num, MV_ATTR_PHY_EDGE, MV_DDR_PHY_EDGE_POSITIVE);
	ddr3_tip_dev_attr_set(dev_num, MV_ATTR_OCTET_PER_INTERFACE, DDR_INTERFACE_OCTETS_NUM);
#ifdef CONFIG_ARMADA_39X
	ddr3_tip_dev_attr_set(dev_num, MV_ATTR_INTERLEAVE_WA, 1);
#else
	ddr3_tip_dev_attr_set(dev_num, MV_ATTR_INTERLEAVE_WA, 0);
#endif

#ifdef STATIC_ALGO_SUPPORT
	{
		struct hws_tip_static_config_info static_config;
		u32 board_offset =
		    board_id * DDR_INTERFACES_NUM *
		    DDR_INTERFACE_OCTETS_NUM;

		static_config.silicon_delay =
			a38x_silicon_delay_offset[board_id];
		static_config.package_trace_arr =
			a38x_package_round_trip_delay_array;
		static_config.board_trace_arr =
			&a38x_board_round_trip_delay_array[board_offset];
		ddr3_tip_init_static_config_db(dev_num, &static_config);
	}
#endif

	ca_delay = 0;
	delay_enable = 1;
	dfs_low_freq = DFS_LOW_FREQ_VALUE;
	calibration_update_control = 1;

#ifdef CONFIG_ARMADA_38X
	/* For a38x only, change to 2T mode to resolve low freq instability */
	mode_2t = 1;
#endif

#if !defined(CONFIG_DDR4)
	ddr3_tip_a38x_get_medium_freq(dev_num, &medium_freq);
#endif /* CONFIG_DDR4 */

	return MV_OK;
}

static int mv_ddr_training_mask_set(void)
{
#if defined(CONFIG_DDR4)
	mask_tune_func = (SET_LOW_FREQ_MASK_BIT |
			  LOAD_PATTERN_MASK_BIT |
			  SET_TARGET_FREQ_MASK_BIT |
			  WRITE_LEVELING_TF_MASK_BIT |
			  READ_LEVELING_TF_MASK_BIT |
			  RECEIVER_CALIBRATION_MASK_BIT |
			  WL_PHASE_CORRECTION_MASK_BIT |
			  DQ_VREF_CALIBRATION_MASK_BIT);
	/* Temporarily disable the DQ_MAPPING stage */
	/*		  DQ_MAPPING_MASK_BIT */
	rl_mid_freq_wa = 0;

	/* In case A382, Vref calibration workaround isn't required */
	if (((reg_read(DEV_ID_REG) & 0xFFFF0000) >> 16) == 0x6811) {
		printf("vref_calibration_wa is disabled\n");
		vref_calibration_wa = 0;
	}
#else /* CONFIG_DDR4 */
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	enum hws_ddr_freq ddr_freq = tm->interface_params[0].memory_freq;

	mask_tune_func = (SET_LOW_FREQ_MASK_BIT |
			  LOAD_PATTERN_MASK_BIT |
			  SET_MEDIUM_FREQ_MASK_BIT | WRITE_LEVELING_MASK_BIT |
			  WRITE_LEVELING_SUPP_MASK_BIT |
			  READ_LEVELING_MASK_BIT |
			  PBS_RX_MASK_BIT |
			  PBS_TX_MASK_BIT |
			  SET_TARGET_FREQ_MASK_BIT |
			  WRITE_LEVELING_TF_MASK_BIT |
			  WRITE_LEVELING_SUPP_TF_MASK_BIT |
			  READ_LEVELING_TF_MASK_BIT |
			  CENTRALIZATION_RX_MASK_BIT |
			  CENTRALIZATION_TX_MASK_BIT);
	rl_mid_freq_wa = 1;

	if ((ddr_freq == DDR_FREQ_333) || (ddr_freq == DDR_FREQ_400)) {
		mask_tune_func = (WRITE_LEVELING_MASK_BIT |
				  LOAD_PATTERN_2_MASK_BIT |
				  WRITE_LEVELING_SUPP_MASK_BIT |
				  READ_LEVELING_MASK_BIT |
				  PBS_RX_MASK_BIT |
				  PBS_TX_MASK_BIT |
				  CENTRALIZATION_RX_MASK_BIT |
				  CENTRALIZATION_TX_MASK_BIT);
		rl_mid_freq_wa = 0; /* WA not needed if 333/400 is TF */
	}

	/* Supplementary not supported for ECC modes */
	if (1 == ddr3_if_ecc_enabled()) {
		mask_tune_func &= ~WRITE_LEVELING_SUPP_TF_MASK_BIT;
		mask_tune_func &= ~WRITE_LEVELING_SUPP_MASK_BIT;
		mask_tune_func &= ~PBS_TX_MASK_BIT;
		mask_tune_func &= ~PBS_RX_MASK_BIT;
	}
#endif /* CONFIG_DDR4 */

	return MV_OK;
}

/* function: mv_ddr_set_calib_controller
 * this function sets the controller which will control
 * the calibration cycle in the end of the training.
 * 1 - internal controller
 * 2 - external controller
 */
void mv_ddr_set_calib_controller(void)
{
	calibration_update_control = CALIB_MACHINE_INT_CTRL;
}

static int ddr3_tip_a38x_set_divider(u8 dev_num, u32 if_id,
				     enum hws_ddr_freq frequency)
{
	u32 divider = 0;
	u32 sar_val, ref_clk_satr;
	u32 async_val;

	if (if_id != 0) {
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR,
				      ("A38x does not support interface 0x%x\n",
				       if_id));
		return MV_BAD_PARAM;
	}

	/* get VCO freq index */
	sar_val = (reg_read(REG_DEVICE_SAR1_ADDR) >>
		   RST2_CPU_DDR_CLOCK_SELECT_IN_OFFSET) &
		RST2_CPU_DDR_CLOCK_SELECT_IN_MASK;

	ref_clk_satr = reg_read(DEVICE_SAMPLE_AT_RESET2_REG);
	if (((ref_clk_satr >> DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_OFFSET) & 0x1) ==
	    DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_25MHZ)
		divider = a38x_vco_freq_per_sar_ref_clk_25_mhz[sar_val] / freq_val[frequency];
	else
		divider = a38x_vco_freq_per_sar_ref_clk_40_mhz[sar_val] / freq_val[frequency];

	if ((async_mode_at_tf == 1) && (freq_val[frequency] > 400)) {
		/* Set async mode */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0x20220, 0x1000, 0x1000));
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe42f4, 0x200, 0x200));

		/* Wait for async mode setup */
		mdelay(5);

		/* Set KNL values */
		switch (frequency) {
#ifdef CONFIG_DDR3
		case DDR_FREQ_467:
			async_val = 0x806f012;
			break;
		case DDR_FREQ_533:
			async_val = 0x807f012;
			break;
		case DDR_FREQ_600:
			async_val = 0x805f00a;
			break;
#endif
		case DDR_FREQ_667:
			async_val = 0x809f012;
			break;
		case DDR_FREQ_800:
			async_val = 0x807f00a;
			break;
#ifdef CONFIG_DDR3
		case DDR_FREQ_850:
			async_val = 0x80cb012;
			break;
#endif
		case DDR_FREQ_900:
			async_val = 0x80d7012;
			break;
		case DDR_FREQ_933:
			async_val = 0x80df012;
			break;
		case DDR_FREQ_1000:
			async_val = 0x80ef012;
			break;
		case DDR_FREQ_1066:
			async_val = 0x80ff012;
			break;
		default:
			/* set DDR_FREQ_667 as default */
			async_val = 0x809f012;
		}
		ddr3_tip_a38x_if_write(dev_num, ACCESS_TYPE_UNICAST, if_id, 0xe42f0, async_val, 0xffffffff);
	} else {
		/* Set sync mode */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0x20220, 0x0, 0x1000));
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe42f4, 0x0, 0x200));

		/* cpupll_clkdiv_reset_mask */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe4264, 0x1f, 0xff));

		/* cpupll_clkdiv_reload_smooth */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe4260, (0x2 << 8), (0xff << 8)));

		/* cpupll_clkdiv_relax_en */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe4260, (0x2 << 24), (0xff << 24)));

		/* write the divider */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe4268, (divider << 8), (0x3f << 8)));

		/* set cpupll_clkdiv_reload_ratio */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe4264, (1 << 8), (1 << 8)));

		/* undet cpupll_clkdiv_reload_ratio */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe4264, 0, (1 << 8)));

		/* clear cpupll_clkdiv_reload_force */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe4260, 0, (0xff << 8)));

		/* clear cpupll_clkdiv_relax_en */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe4260, 0, (0xff << 24)));

		/* clear cpupll_clkdiv_reset_mask */
		CHECK_STATUS(ddr3_tip_a38x_if_write
			     (dev_num, ACCESS_TYPE_UNICAST, if_id,
			      0xe4264, 0, 0xff));
	}

	/* Dunit training clock + 1:1/2:1 mode */
	CHECK_STATUS(ddr3_tip_a38x_if_write
		     (dev_num, ACCESS_TYPE_UNICAST, if_id, 0x18488,
		      ((ddr3_tip_clock_mode(frequency) & 0x1) << 16),
		      (1 << 16)));
	CHECK_STATUS(ddr3_tip_a38x_if_write
		     (dev_num, ACCESS_TYPE_UNICAST, if_id, 0x1524,
		      ((ddr3_tip_clock_mode(frequency) - 1) << 15),
		      (1 << 15)));

	return MV_OK;
}

/*
 * external read from memory
 */
int ddr3_tip_ext_read(u32 dev_num, u32 if_id, u32 reg_addr,
		      u32 num_of_bursts, u32 *data)
{
	u32 burst_num;

	for (burst_num = 0; burst_num < num_of_bursts * 8; burst_num++)
		data[burst_num] = readl(reg_addr + 4 * burst_num);

	return MV_OK;
}

/*
 * external write to memory
 */
int ddr3_tip_ext_write(u32 dev_num, u32 if_id, u32 reg_addr,
		       u32 num_of_bursts, u32 *data) {
	u32 burst_num;

	for (burst_num = 0; burst_num < num_of_bursts * 8; burst_num++)
		writel(data[burst_num], reg_addr + 4 * burst_num);

	return MV_OK;
}

int mv_ddr_early_init(void)
{
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	/* FIXME: change this configuration per ddr type
	 * configure a380 and a390 to work with receiver odt timing
	 * the odt_config is defined:
	 * '1' in ddr4
	 * '0' in ddr3
	 * here the parameter is run over in ddr4 and ddr3 to '1' (in ddr4 the default is '1')
	 * to configure the odt to work with timing restrictions
	 */
	odt_config = 1;

	mv_ddr_sw_db_init(0, 0);

	if (tm->interface_params[0].memory_freq != DDR_FREQ_SAR)
		async_mode_at_tf = 1;

	return MV_OK;
}

int mv_ddr_early_init2(void)
{
	mv_ddr_training_mask_set();

	return MV_OK;
}

int ddr3_post_run_alg(void)
{
	return MV_OK;
}

int ddr3_silicon_post_init(void)
{
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	/* Set half bus width */
	if (DDR3_IS_16BIT_DRAM_MODE(tm->bus_act_mask)) {
		CHECK_STATUS(ddr3_tip_if_write
			     (0, ACCESS_TYPE_UNICAST, PARAM_NOT_CARE,
			      SDRAM_CONFIGURATION_REG, 0x0, 0x8000));
	}

	return MV_OK;
}

u32 mv_ddr_init_freq_get(void)
{
	enum hws_ddr_freq freq;

	mv_ddr_sar_freq_get(0, &freq);

	return freq;
}

#if defined(CONFIG_PHY_STATIC) || defined(CONFIG_MC_STATIC)
/*
 * Name:     ddr3_get_static_ddr_mode - Init Memory controller with
 *           static parameters
 * Desc:     Use this routine to init the controller without the HW training
 *           procedure.
 *           User must provide compatible header file with registers data.
 * Args:     None.
 * Notes:
 * Returns:  None.
 */

u32 ddr3_get_static_ddr_mode(void)
{
	u32 i;

	for (i = 0; ddr_modes[i].mc_regs != NULL; i++) {
		if (mv_ddr_init_freq_get() == ddr_modes[i].cpu_freq)
			return i;
	}

	DEBUG_INIT_S("\n*** Error: ddr3_get_static_ddr_mode: No match for requested DDR mode. ***\n\n");

	return 0;
}
#endif

static u32 ddr3_get_bus_width(void)
{
	u32 bus_width;

	bus_width = (reg_read(SDRAM_CONFIGURATION_REG) & 0x8000) >>
		REG_SDRAM_CONFIG_WIDTH_OFFS;

	return (bus_width == 0) ? 16 : 32;
}

static u32 ddr3_get_device_width(u32 cs)
{
	u32 device_width;

	device_width = (reg_read(REG_SDRAM_ADDRESS_CTRL_ADDR) &
			(0x3 << (REG_SDRAM_ADDRESS_CTRL_STRUCT_OFFS * cs))) >>
		(REG_SDRAM_ADDRESS_CTRL_STRUCT_OFFS * cs);

	return (device_width == 0) ? 8 : 16;
}

static float ddr3_get_device_size(u32 cs)
{
	u32 device_size_low, device_size_high, device_size;
	u32 data, cs_low_offset, cs_high_offset;

	cs_low_offset = REG_SDRAM_ADDRESS_SIZE_OFFS + cs * 4;
	cs_high_offset = REG_SDRAM_ADDRESS_SIZE_OFFS +
		REG_SDRAM_ADDRESS_SIZE_HIGH_OFFS + cs;

	data = reg_read(REG_SDRAM_ADDRESS_CTRL_ADDR);
	device_size_low = (data >> cs_low_offset) & 0x3;
	device_size_high = (data >> cs_high_offset) & 0x1;

	device_size = device_size_low | (device_size_high << 2);

	switch (device_size) {
	case 0:
		return 2;
	case 2:
		return 0.5;
	case 3:
		return 1;
	case 4:
		return 4;
	case 5:
		return 8;
	case 1:
	default:
		DEBUG_INIT_C("Error: Wrong device size of Cs: ", cs, 1);
		/*
		 * Small value will give wrong emem size in
		 * ddr3_calc_mem_cs_size
		 */
		return 0.01;
	}
}

static int ddr3_calc_mem_cs_size(u32 cs, u32 *cs_size)
{
	float cs_mem_size;

	/* Calculate in GiB */
	cs_mem_size = ((ddr3_get_bus_width() / ddr3_get_device_width(cs)) *
		       ddr3_get_device_size(cs)) / 8;

	/*
	 * Multiple controller bus width, 2x for 64 bit
	 * (SoC controller may be 32 or 64 bit,
	 * so bit 15 in 0x1400, that means if whole bus used or only half,
	 * have a differnt meaning
	 */
	cs_mem_size *= DDR_CONTROLLER_BUS_WIDTH_MULTIPLIER;

	if (cs_mem_size == 0.125) {
		*cs_size = 128 << 20;
	} else if (cs_mem_size == 0.25) {
		*cs_size = 256 << 20;
	} else if (cs_mem_size == 0.5) {
		*cs_size = 512 << 20;
	} else if (cs_mem_size == 1) {
		*cs_size = 1 << 30;
	} else if (cs_mem_size == 2) {
		*cs_size = 2 << 30;
	} else {
		DEBUG_INIT_C("Error: Wrong Memory size of Cs: ", cs, 1);
		return MV_BAD_VALUE;
	}

	return MV_OK;
}

static int ddr3_fast_path_dynamic_cs_size_config(u32 cs_ena)
{
	u32 reg, cs;
	u32 mem_total_size = 0;
	u32 cs_mem_size = 0;
	u32 mem_total_size_c, cs_mem_size_c;

#ifdef DEVICE_MAX_DRAM_ADDRESS_SIZE
	u32 physical_mem_size;
	u32 max_mem_size = DEVICE_MAX_DRAM_ADDRESS_SIZE;
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
#endif

	/* Open fast path windows */
	for (cs = 0; cs < MAX_CS; cs++) {
		if (cs_ena & (1 << cs)) {
			/* get CS size */
			if (ddr3_calc_mem_cs_size(cs, &cs_mem_size) != MV_OK)
				return MV_FAIL;

#ifdef DEVICE_MAX_DRAM_ADDRESS_SIZE
			/*
			 * if number of address pins doesn't allow to use max
			 * mem size that is defined in topology
			 * mem size is defined by DEVICE_MAX_DRAM_ADDRESS_SIZE
			 */
			physical_mem_size = mem_size
				[tm->interface_params[0].memory_size];

			if (ddr3_get_device_width(cs) == 16) {
				/*
				 * 16bit mem device can be twice more - no need
				 * in less significant pin
				 */
				max_mem_size = DEVICE_MAX_DRAM_ADDRESS_SIZE * 2;
			}

			if (physical_mem_size > max_mem_size) {
				cs_mem_size = max_mem_size *
					(ddr3_get_bus_width() /
					 ddr3_get_device_width(cs));
				printf("Updated Physical Mem size is from 0x%x to %x\n",
				       physical_mem_size,
				       DEVICE_MAX_DRAM_ADDRESS_SIZE);
			}
#endif

			/* set fast path window control for the cs */
			reg = 0xffffe1;
			reg |= (cs << 2);
			reg |= (cs_mem_size - 1) & 0xffff0000;
			/*Open fast path Window */
			reg_write(REG_FASTPATH_WIN_CTRL_ADDR(cs), reg);

			/* Set fast path window base address for the cs */
			reg = ((cs_mem_size) * cs) & 0xffff0000;
			/* Set base address */
			reg_write(REG_FASTPATH_WIN_BASE_ADDR(cs), reg);

			/*
			 * Since memory size may be bigger than 4G the summ may
			 * be more than 32 bit word,
			 * so to estimate the result divide mem_total_size and
			 * cs_mem_size by 0x10000 (it is equal to >> 16)
			 */
			mem_total_size_c = mem_total_size >> 16;
			cs_mem_size_c = cs_mem_size >> 16;
			/* if the sum less than 2 G - calculate the value */
			if (mem_total_size_c + cs_mem_size_c < 0x10000)
				mem_total_size += cs_mem_size;
			else	/* put max possible size */
				mem_total_size = L2_FILTER_FOR_MAX_MEMORY_SIZE;
		}
	}

	/* Set L2 filtering to Max Memory size */
	reg_write(ADDRESS_FILTERING_END_REGISTER, mem_total_size);

	return MV_OK;
}

static int ddr3_restore_and_set_final_windows(u32 *win, const char *ddr_type)
{
	u32 win_ctrl_reg, num_of_win_regs;
	u32 cs_ena = sys_env_get_cs_ena_from_reg();
	u32 ui;

	win_ctrl_reg = REG_XBAR_WIN_4_CTRL_ADDR;
	num_of_win_regs = 16;

	/* Return XBAR windows 4-7 or 16-19 init configuration */
	for (ui = 0; ui < num_of_win_regs; ui++)
		reg_write((win_ctrl_reg + 0x4 * ui), win[ui]);

	printf("%s Training Sequence - Switching XBAR Window to FastPath Window\n",
	       ddr_type);

#if defined DYNAMIC_CS_SIZE_CONFIG
	if (ddr3_fast_path_dynamic_cs_size_config(cs_ena) != MV_OK)
		printf("ddr3_fast_path_dynamic_cs_size_config FAILED\n");
#else
	u32 reg, cs;
	reg = 0x1fffffe1;
	for (cs = 0; cs < MAX_CS; cs++) {
		if (cs_ena & (1 << cs)) {
			reg |= (cs << 2);
			break;
		}
	}
	/* Open fast path Window to - 0.5G */
	reg_write(REG_FASTPATH_WIN_0_CTRL_ADDR, reg);
#endif

	return MV_OK;
}

static int ddr3_save_and_set_training_windows(u32 *win)
{
	u32 cs_ena;
	u32 reg, tmp_count, cs, ui;
	u32 win_ctrl_reg, win_base_reg, win_remap_reg;
	u32 num_of_win_regs, win_jump_index;
	win_ctrl_reg = REG_XBAR_WIN_4_CTRL_ADDR;
	win_base_reg = REG_XBAR_WIN_4_BASE_ADDR;
	win_remap_reg = REG_XBAR_WIN_4_REMAP_ADDR;
	win_jump_index = 0x10;
	num_of_win_regs = 16;
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

#ifdef DISABLE_L2_FILTERING_DURING_DDR_TRAINING
	/*
	 * Disable L2 filtering during DDR training
	 * (when Cross Bar window is open)
	 */
	reg_write(ADDRESS_FILTERING_END_REGISTER, 0);
#endif

	cs_ena = tm->interface_params[0].as_bus_params[0].cs_bitmask;

	/* Close XBAR Window 19 - Not needed */
	/* {0x000200e8}  -   Open Mbus Window - 2G */
	reg_write(REG_XBAR_WIN_19_CTRL_ADDR, 0);

	/* Save XBAR Windows 4-19 init configurations */
	for (ui = 0; ui < num_of_win_regs; ui++)
		win[ui] = reg_read(win_ctrl_reg + 0x4 * ui);

	/* Open XBAR Windows 4-7 or 16-19 for other CS */
	reg = 0;
	tmp_count = 0;
	for (cs = 0; cs < MAX_CS; cs++) {
		if (cs_ena & (1 << cs)) {
			switch (cs) {
			case 0:
				reg = 0x0e00;
				break;
			case 1:
				reg = 0x0d00;
				break;
			case 2:
				reg = 0x0b00;
				break;
			case 3:
				reg = 0x0700;
				break;
			}
			reg |= (1 << 0);
			reg |= (SDRAM_CS_SIZE & 0xffff0000);

			reg_write(win_ctrl_reg + win_jump_index * tmp_count,
				  reg);
			reg = (((SDRAM_CS_SIZE + 1) * (tmp_count)) &
			       0xffff0000);
			reg_write(win_base_reg + win_jump_index * tmp_count,
				  reg);

			if (win_remap_reg <= REG_XBAR_WIN_7_REMAP_ADDR)
				reg_write(win_remap_reg +
					  win_jump_index * tmp_count, 0);

			tmp_count++;
		}
	}

	return MV_OK;
}

static u32 win[16];

int mv_ddr_pre_training_soc_config(const char *ddr_type)
{
	u32 soc_num;
	u32 reg_val;

	/* Switching CPU to MRVL ID */
	soc_num = (reg_read(REG_SAMPLE_RESET_HIGH_ADDR) & SAR1_CPU_CORE_MASK) >>
		SAR1_CPU_CORE_OFFSET;
	switch (soc_num) {
	case 0x3:
		reg_bit_set(CPU_CONFIGURATION_REG(3), CPU_MRVL_ID_OFFSET);
		reg_bit_set(CPU_CONFIGURATION_REG(2), CPU_MRVL_ID_OFFSET);
	case 0x1:
		reg_bit_set(CPU_CONFIGURATION_REG(1), CPU_MRVL_ID_OFFSET);
	case 0x0:
		reg_bit_set(CPU_CONFIGURATION_REG(0), CPU_MRVL_ID_OFFSET);
	default:
		break;
	}

	/*
	 * Set DRAM Reset Mask in case detected GPIO indication of wakeup from
	 * suspend i.e the DRAM values will not be overwritten / reset when
	 * waking from suspend
	 */
	if (sys_env_suspend_wakeup_check() ==
	    SUSPEND_WAKEUP_ENABLED_GPIO_DETECTED) {
		reg_bit_set(REG_SDRAM_INIT_CTRL_ADDR,
			    1 << REG_SDRAM_INIT_RESET_MASK_OFFS);
	}

	/* Check if DRAM is already initialized  */
	if (reg_read(REG_BOOTROM_ROUTINE_ADDR) &
	    (1 << REG_BOOTROM_ROUTINE_DRAM_INIT_OFFS)) {
		printf("%s Training Sequence - 2nd boot - Skip\n", ddr_type);
		return MV_OK;
	}

	/* Fix read ready phases for all SOC in reg 0x15c8 */
	reg_val = reg_read(REG_TRAINING_DEBUG_3_ADDR);
	reg_val &= ~(REG_TRAINING_DEBUG_3_MASK);
	reg_val |= 0x4;		/* Phase 0 */
	reg_val &= ~(REG_TRAINING_DEBUG_3_MASK << REG_TRAINING_DEBUG_3_OFFS);
	reg_val |= (0x4 << (1 * REG_TRAINING_DEBUG_3_OFFS));	/* Phase 1 */
	reg_val &= ~(REG_TRAINING_DEBUG_3_MASK << (3 * REG_TRAINING_DEBUG_3_OFFS));
	reg_val |= (0x6 << (3 * REG_TRAINING_DEBUG_3_OFFS));	/* Phase 3 */
	reg_val &= ~(REG_TRAINING_DEBUG_3_MASK << (4 * REG_TRAINING_DEBUG_3_OFFS));
	reg_val |= (0x6 << (4 * REG_TRAINING_DEBUG_3_OFFS));
	reg_val &= ~(REG_TRAINING_DEBUG_3_MASK << (5 * REG_TRAINING_DEBUG_3_OFFS));
	reg_val |= (0x6 << (5 * REG_TRAINING_DEBUG_3_OFFS));
	reg_write(REG_TRAINING_DEBUG_3_ADDR, reg_val);

	/*
	 * Axi_bresp_mode[8] = Compliant,
	 * Axi_addr_decode_cntrl[11] = Internal,
	 * Axi_data_bus_width[0] = 128bit
	 * */
	/* 0x14a8 - AXI Control Register */
	reg_write(REG_DRAM_AXI_CTRL_ADDR, 0);

	/*
	 * Stage 2 - Training Values Setup
	 */
	/* Set X-BAR windows for the training sequence */
	ddr3_save_and_set_training_windows(win);

#ifdef SUPPORT_STATIC_DUNIT_CONFIG
	/*
	 * Load static controller configuration (in case dynamic/generic init
	 * is not enabled
	 */
	if (generic_init_controller == 0) {
		ddr3_tip_init_specific_reg_config(0,
						  ddr_modes
						  [ddr3_get_static_ddr_mode
						   ()].regs);
	}
#endif

	return MV_OK;
}

static int ddr3_new_tip_dlb_config(void)
{
	u32 reg, i = 0;
	struct dlb_config *config_table_ptr = sys_env_dlb_config_ptr_get();

	/* Write the configuration */
	while (config_table_ptr[i].reg_addr != 0) {
		reg_write(config_table_ptr[i].reg_addr,
			  config_table_ptr[i].reg_data);
		i++;
	}

#if defined(CONFIG_DDR4)
	reg = reg_read(REG_DDR_CONT_HIGH_ADDR);
	reg |= DLB_INTERJECTION_ENABLE;
	reg_write(REG_DDR_CONT_HIGH_ADDR, reg);
#endif /* CONFIG_DDR4 */

	/* Enable DLB */
	reg = reg_read(REG_STATIC_DRAM_DLB_CONTROL);
	reg |= DLB_ENABLE | DLB_WRITE_COALESING | DLB_AXI_PREFETCH_EN |
		DLB_MBUS_PREFETCH_EN | PREFETCH_N_LN_SZ_TR;
	reg_write(REG_STATIC_DRAM_DLB_CONTROL, reg);

	return MV_OK;
}

int mv_ddr_post_training_soc_config(const char *ddr_type)
{
	u32 reg_val;

	/* Restore and set windows */
	ddr3_restore_and_set_final_windows(win, ddr_type);

	/* Update DRAM init indication in bootROM register */
	reg_val = reg_read(REG_BOOTROM_ROUTINE_ADDR);
	reg_write(REG_BOOTROM_ROUTINE_ADDR,
		  reg_val | (1 << REG_BOOTROM_ROUTINE_DRAM_INIT_OFFS));

	/* DLB config */
	ddr3_new_tip_dlb_config();

	return MV_OK;
}

void mv_ddr_mc_config(void)
{
	/* Memory controller initializations */
	struct init_cntr_param init_param;
	int status;

	init_param.do_mrs_phy = 1;
	init_param.is_ctrl64_bit = 0;
	init_param.init_phy = 1;
	init_param.msys_init = 1;
	status = hws_ddr3_tip_init_controller(0, &init_param);
	if (MV_OK != status) {
		printf("DDR3 init controller - FAILED 0x%x\n", status);
	}

	status = mv_ddr_mc_init();
	if (MV_OK != status) {
		printf("DDR3 init_sequence - FAILED 0x%x\n", status);
	}
}
/* function: mv_ddr_mc_init
 * this function enables the dunit after init controller configuration
 */
int mv_ddr_mc_init(void)
{
	CHECK_STATUS(ddr3_tip_enable_init_sequence(0));

	return MV_OK;
}

/* function: ddr3_tip_configure_phy
 * configures phy and electrical parameters
 */
int ddr3_tip_configure_phy(u32 dev_num)
{
	u32 if_id, phy_id;
	u32 octets_per_if_num = ddr3_tip_dev_attr_get(dev_num, MV_ATTR_OCTET_PER_INTERFACE);
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	CHECK_STATUS(ddr3_tip_bus_write
		(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA,
		PAD_ZRI_CALIB_PHY_REG,
		((0x7f & g_zpri_data) << 7 | (0x7f & g_znri_data))));
	CHECK_STATUS(ddr3_tip_bus_write
		(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_CONTROL,
		PAD_ZRI_CALIB_PHY_REG,
		((0x7f & g_zpri_ctrl) << 7 | (0x7f & g_znri_ctrl))));
	CHECK_STATUS(ddr3_tip_bus_write
		(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA,
		PAD_ODT_CALIB_PHY_REG,
		((0x3f & g_zpodt_data) << 6 | (0x3f & g_znodt_data))));
	CHECK_STATUS(ddr3_tip_bus_write
		(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_CONTROL,
		PAD_ODT_CALIB_PHY_REG,
		((0x3f & g_zpodt_ctrl) << 6 | (0x3f & g_znodt_ctrl))));

	CHECK_STATUS(ddr3_tip_bus_write
		(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA,
		PAD_PRE_DISABLE_PHY_REG, 0));
	CHECK_STATUS(ddr3_tip_bus_write
		(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_DATA,
		CMOS_CONFIG_PHY_REG, 0));
	CHECK_STATUS(ddr3_tip_bus_write
		(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, DDR_PHY_CONTROL,
		CMOS_CONFIG_PHY_REG, 0));

	for (if_id = 0; if_id <= MAX_INTERFACE_NUM - 1; if_id++) {
		/* check if the interface is enabled */
		VALIDATE_IF_ACTIVE(tm->if_act_mask, if_id);

		for (phy_id = 0;
			phy_id < octets_per_if_num;
			phy_id++) {
				VALIDATE_BUS_ACTIVE(tm->bus_act_mask, phy_id);
				/* Vref & clamp */
				CHECK_STATUS(ddr3_tip_bus_read_modify_write
					(dev_num, ACCESS_TYPE_UNICAST,
					if_id, phy_id, DDR_PHY_DATA,
					PAD_CONFIG_PHY_REG,
					((clamp_tbl[if_id] << 4) | vref_init_val),
					((0x7 << 4) | 0x7)));
				/* clamp not relevant for control */
				CHECK_STATUS(ddr3_tip_bus_read_modify_write
					(dev_num, ACCESS_TYPE_UNICAST,
					if_id, phy_id, DDR_PHY_CONTROL,
					PAD_CONFIG_PHY_REG, 0x4, 0x7));
		}
	}

	if (ddr3_tip_dev_attr_get(dev_num, MV_ATTR_PHY_EDGE) ==
		MV_DDR_PHY_EDGE_POSITIVE)
		CHECK_STATUS(ddr3_tip_bus_write
		(dev_num, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
		DDR_PHY_DATA, 0x90, 0x6002));

#if defined(CONFIG_DDR4)
	mv_ddr4_phy_config(dev_num);
#endif /* CONFIG_DDR4 */

	return MV_OK;
}

#if defined(CONFIG_DDR4)
/* function: ddr4TipCalibrationValidate
 * this function validates the calibration values
 * the function is per soc due to the different processes the calibration values are different
 */
MV_STATUS mv_ddr4_calibration_validate(MV_U32 dev_num)
{
	MV_STATUS status = MV_OK;
	MV_U8 if_id = 0;
	MV_U32 read_data[MAX_INTERFACE_NUM];
	MV_U32 cal_n = 0, cal_p = 0;

	/*
	 * Pad calibration control enable: during training set the calibration to be internal
	 * at the end of the training it should be fixed to external to be configured by the mc6
	 * FIXME: set the calibration to external in the end of the training
	 */

	/* pad calibration control enable */
	CHECK_STATUS(ddr3_tip_if_write
			(DEV_NUM_0, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, CALIB_MACHINE_CTRL_REG,
			DYN_PAD_CALIB_ENABLE << MV_DDR_DYN_PADS_CALIB_EN_OFFS |
			CALIB_MACHINE_INT_CTRL << MV_DDR_CALIB_UPDATE_CTRL_OFFS,
			MV_DDR_RECALIBRATE_MASK << MV_DDR_DYN_PADS_CALIB_EN_OFFS |
			MV_DDR_CALIB_UPDATE_CTRL_MASK << MV_DDR_CALIB_UPDATE_CTRL_OFFS));

	/* Polling initial calibration is done*/
	if (ddr3_tip_if_polling(dev_num, ACCESS_TYPE_UNICAST, if_id,
				CALIB_MACHINE_STATUS_READY << MV_DDR_CALIB_MACHINE_STATUS_OFFS,
				MV_DDR_CALIB_MACHINE_STATUS_MASK << MV_DDR_CALIB_MACHINE_STATUS_OFFS,
				CALIB_MACHINE_CTRL_REG, MAX_POLLING_ITERATIONS) != MV_OK)
		DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("ddr4TipCalibrationAdjust: DDR4 calibration poll failed(0)\n"));

	/* Polling that calibration propagate to io */
	if (ddr3_tip_if_polling(dev_num, ACCESS_TYPE_UNICAST, if_id, 0x3FFFFFF, 0x3FFFFFF, REG_PHY_LOCK_STATUS_ADDR,
				MAX_POLLING_ITERATIONS) != MV_OK)
		DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("ddr4TipCalibrationAdjust: DDR4 calibration poll failed(1)\n"));

	/* TODO - debug why polling not enough*/
	mdelay(10);

	/* pad calibration control disable */
	CHECK_STATUS(ddr3_tip_if_write
			(DEV_NUM_0, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, CALIB_MACHINE_CTRL_REG,
			DYN_PAD_CALIB_DISABLE << MV_DDR_DYN_PADS_CALIB_EN_OFFS |
			CALIB_MACHINE_INT_CTRL << MV_DDR_CALIB_UPDATE_CTRL_OFFS,
			MV_DDR_RECALIBRATE_MASK << MV_DDR_DYN_PADS_CALIB_EN_OFFS |
			MV_DDR_CALIB_UPDATE_CTRL_MASK << MV_DDR_CALIB_UPDATE_CTRL_OFFS));

	/* Polling initial calibration is done */
	if (ddr3_tip_if_polling(dev_num, ACCESS_TYPE_UNICAST, if_id,
				CALIB_MACHINE_STATUS_READY << MV_DDR_CALIB_MACHINE_STATUS_OFFS,
				MV_DDR_CALIB_MACHINE_STATUS_MASK << MV_DDR_CALIB_MACHINE_STATUS_OFFS,
				CALIB_MACHINE_CTRL_REG, MAX_POLLING_ITERATIONS) != MV_OK)
		DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("ddr4TipCalibrationAdjust: DDR4 calibration poll failed(0)\n"));

	/* Polling that calibration propagate to io */
	if (ddr3_tip_if_polling(dev_num, ACCESS_TYPE_UNICAST, if_id, 0x3FFFFFF, 0x3FFFFFF, REG_PHY_LOCK_STATUS_ADDR,
				MAX_POLLING_ITERATIONS) != MV_OK)
		DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("ddr4TipCalibrationAdjust: DDR4 calibration poll failed(1)\n"));

	/* TODO - debug why polling not enough */
	mdelay(10);

	/* Read Cal value and set to manual val */
	CHECK_STATUS(ddr3_tip_if_read(dev_num, ACCESS_TYPE_UNICAST, if_id, 0x1DC8, read_data, MASK_ALL_BITS));
	cal_n = (read_data[if_id] & ((0x3F) << 10)) >> 10;
	cal_p = (read_data[if_id] & ((0x3F) << 4)) >> 4;
	DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO,
			  ("ddr4TipCalibrationValidate::DDR4 SSTL calib val - Pcal = 0x%x , Ncal = 0x%x\n",
			   cal_p, cal_n));
	if ((cal_n >= 56) || (cal_n <= 6) || (cal_p >= 59) || (cal_p <= 7)) {
		DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR,
				  ("ddr4TipCalibrationValidate: Error:DDR4 SSTL calib val - Pcal = 0x%x,\
				   Ncal = 0x%x are out of range\n", cal_p, cal_n));
		status = MV_FAIL;
	}

	/* 14C8 - Vertical */
	CHECK_STATUS(ddr3_tip_if_read(dev_num, ACCESS_TYPE_UNICAST, if_id, 0x14C8, read_data, MASK_ALL_BITS));
	cal_n = (read_data[if_id] & ((0x3F) << 10)) >> 10;
	cal_p = (read_data[if_id] & ((0x3F) << 4)) >> 4;
	DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO,
			  ("ddr4TipCalibrationValidate::DDR4 POD-V calib val - Pcal = 0x%x , Ncal = 0x%x\n",
			  cal_p, cal_n));
	if ((cal_n >= 56) || (cal_n <= 6) || (cal_p >= 59) || (cal_p <= 7)) {
		DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR,
				  ("ddr4TipCalibrationValidate: Error:DDR4 POD-V calib val - Pcal = 0x%x , Ncal= 0x%x \
				   are out of range\n", cal_p, cal_n));
		status = MV_FAIL;
	}

	/* 17C8 - Horizontal */
	CHECK_STATUS(ddr3_tip_if_read(dev_num, ACCESS_TYPE_UNICAST, if_id, 0x17C8, read_data, MASK_ALL_BITS));
	cal_n = (read_data[if_id] & ((0x3F) << 10)) >> 10;
	cal_p = (read_data[if_id] & ((0x3F) << 4)) >> 4;
	DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO,
			  ("ddr4TipCalibrationValidate::DDR4 POD-H calib val - Pcal = 0x%x , Ncal = 0x%x \n",
			  cal_p, cal_n));
	if ((cal_n >= 56) || (cal_n <= 6) || (cal_p >= 59) || (cal_p <= 7)) {
		DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR,
				  ("ddr4TipCalibrationValidate: Error:DDR4 POD-H calib val - Pcal = 0x%x, \
				   Ncal = 0x%x are out of range\n", cal_p, cal_n));
		status = MV_FAIL;
	}

	return status;
}
#endif /* CONFIG_DDR4 */

#ifdef CONFIG_MC_STATIC
int mv_ddr_mc_static_config(void)
{
	u32 mode, i = 0;

	mode = ddr3_get_static_ddr_mode();
	while (ddr_modes[mode].mc_regs[i].reg_addr != 0xffffffff) {
		ddr3_tip_if_write(0, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE,
				  ddr_modes[mode].mc_regs[i].reg_addr,
				  ddr_modes[mode].mc_regs[i].reg_data,
				  ddr_modes[mode].mc_regs[i].reg_mask);
		i++;
	}

	CHECK_STATUS(ddr3_tip_enable_init_sequence(0));

	return MV_OK;
}
#endif /* CONFIG_MV_DDR_STATIC_MC */

#ifdef CONFIG_PHY_STATIC
static int mv_ddr_a38x_phy_static_config(u32 if_id, u32 subphys_num, enum hws_ddr_phy subphy_type)
{
	u32 i, mode, subphy_id, dev_num = 0;

	mode = ddr3_get_static_ddr_mode();
	if (subphy_type == DDR_PHY_DATA) {
		for (subphy_id = 0; subphy_id < subphys_num; subphy_id++) {
			i = 0;
			while (ddr_modes[mode].data_phy_regs[i].reg_addr != 0xffffffff) {
				ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_UNICAST,
							subphy_id, subphy_type, ddr_modes[mode].data_phy_regs[i].reg_addr,
							ddr_modes[mode].data_phy_regs[i].reg_data[subphy_id]);
				i++;
			}
		}
	} else {
		for (subphy_id = 0; subphy_id < subphys_num; subphy_id++) {
			i = 0;
			while (ddr_modes[mode].ctrl_phy_regs[i].reg_addr != 0xffffffff) {
				ddr3_tip_bus_write(dev_num, ACCESS_TYPE_UNICAST, if_id, ACCESS_TYPE_UNICAST,
							subphy_id, subphy_type, ddr_modes[mode].ctrl_phy_regs[i].reg_addr,
							ddr_modes[mode].ctrl_phy_regs[i].reg_data[subphy_id]);
				i++;
			}
		}
	}

	return MV_OK;
}

void mv_ddr_phy_static_config(void)
{
/* TODO: Need to use variable for subphys number */
	mv_ddr_a38x_phy_static_config(0, 4, DDR_PHY_DATA);
	mv_ddr_a38x_phy_static_config(0, 3, DDR_PHY_CONTROL);
}
#endif
