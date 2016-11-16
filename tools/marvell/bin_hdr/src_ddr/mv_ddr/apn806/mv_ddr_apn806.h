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

#ifndef _MV_DDR_AP806_H
#define _MV_DDR_AP806_H

#include "mv_ddr_mc6_drv.h"

#define INTER_REGS_BASE	0xf0000000
/* revision id register address */
#define MVEBU_CSS_GWD_CTRL_IIDR2_REG	(INTER_REGS_BASE + 0x610fcc)
#define GWD_IIDR2_REV_ID_OFFSET		12
#define GWD_IIDR2_REV_ID_MASK		0xf
#define APN806_REV_ID_A0		0
#define APN806_REV_ID_A1		1

#define NO_EFUSE
#define IF_ID_0				0

#define MAX_INTERFACE_NUM		1
#define MAX_BUS_NUM			9
#define DDR_IF_CTRL_SUBPHYS_NUM		3

#define MEM_TO_MC6_FREQ_RATIO		2

#define NON_DRAM_MEM_RGN_START_ADDR	0xf0000000ULL
#define NON_DRAM_MEM_RGN_END_ADDR	0x100000000ULL

#define DSS_CR0_REG_ADDR		0x6f0100	/* TODO: put this register in ATF h file */
#define IOMUX_SEL_OFFS			17
#define IOMUX_SEL_MASK			0x7
#define CONF_DCLK_CG_BYPASS_OFFS	1
#define CONF_DCLK_CG_BYPASS_MASK	0x1
#define CONF_DCLK_REG_CG_BYPASS_OFFS	0x2
#define CONF_DCLK_REG_CG_BYPASS_MASK	0x1
enum{
	DSS_LPDDR4_64_DIMM = 0x0,
	DSS_LPDDR4_32_BRD = 0x1,
	DSS_DDR3_DIMM = 0x3,
	DSS_DDR4_DIMM = 0x4,
	DSS_DDR3_ON_BOARD = 0x5,
	DSS_DDR4_ON_BOARD = 0x6
};

#define DUNIT_BASE_ADDR		0x10000

#define SAR_REG_ADDR			0x6f4400
#define RST2_CLOCK_FREQ_MODE_OFFS	0
#define RST2_CLOCK_FREQ_MODE_MASK	0x1f

#define DFX_SRVR_BASE_ADDR		0x6f8000
#define DEV_GEN_CTRL1_REG_ADDR		(DFX_SRVR_BASE_ADDR + 0x254)
#define MISC_CLKDIV_RATIO_1_OFFS	6		/* ddr divider */
#define MISC_CLKDIV_RATIO_1_MASK	0x3f
#define MISC_CLKDIV_RATIO_2_OFFS	12		/* mc divider */
#define MISC_CLKDIV_RATIO_2_MASK	0x3f

#define DEV_GEN_CTRL3_REG_ADDR		(DFX_SRVR_BASE_ADDR + 0x25c)
#define	MISC_CLKDIV_ALIGN_EN_OFFS	0
#define MISC_CLKDIV_ALIGN_EN_MASK	0xff
#define MISC_CLKDIV_RELAX_EN_OFFS	16
#define MISC_CLKDIV_RELAX_EN_MASK	0xff
#define	MISC_CLKDIV_RELOAD_FORCE_OFFS	24
#define	MISC_CLKDIV_RELOAD_FORCE_MASK	0xff
#define RELOAD_FORCE_VAL		0
#define RELAX_EN_VAL			0x3
#define ALIGN_EN_VAL			0x3

#define DEV_GEN_CTRL4_REG_ADDR		(DFX_SRVR_BASE_ADDR + 0x260)
#define MISC_CLKDIV_RELOAD_SMOOTH_OFFS	0
#define MISC_CLKDIV_RELOAD_SMOOTH_MASK	0xff
#define MISC_CLKDIV_RELOAD_RATIO_OFFS	16
#define MISC_CLKDIV_RELOAD_RATIO_MASK	0x1
#define RELOAD_SMOOTH_VAL		0x3
#define RELOAD_RATIO_VAL		0x1

#define CLKS_CTRL_REG_ADDR			0x6F4340
#define RING_CLK_TO_ALL_CLK_PHI_RST_OFFS	0
#define RING_CLK_TO_ALL_CLK_PHI_RST_MASK	0x1
#define BLOCK_PHI_RST_TO_RING_TO_MC_CLK_OFFS	4
#define BLOCK_PHI_RST_TO_RING_TO_MC_CLK_MASK	0x1
#define BLOCK_PHI_RST_VAL			0
#define RING_CLK_VAL				0

/* FIXME: locate the definition in mc6 driver file when generated */
#define REG_CH0_PHY_RL_CTRL_OFFS	0x1180
#define REG_CH0_PHY_RL_CTRL_ADDR(cs)	(MC6_BASE_ADDR + REG_CH0_PHY_RL_CTRL_OFFS + (cs * 36))
#define PHY_RL_CYCLE_DLY_MC6_OFFS	8
#define PHY_RL_CYCLE_DLY_MC6_MASK	0xf

#define REG_PHY_CONTROL_1_ADDR		(MC6_BASE_ADDR + 0x1000)
#define PHY_RFIFO_RPTR_DLY_VAL_OFFS	4
#define PHY_RFIFO_RPTR_DLY_VAL_MASK	0xf

#define REG_RDP_CONTROL_ADDR		(MC6_BASE_ADDR + 0x64)
#define MB_READ_DATA_LATENCY_CH0_OFFS	0
#define MB_READ_DATA_LATENCY_CH0_MASK	0x3f

#define MC6_REG_USER_CMD0		(MC6_BASE_ADDR + 0x20)
#define MC6_USER_CS_OFFS		24
#define MC6_USER_CS_MASK		0xf

#ifdef NO_EFUSE
#define	SAR_CPU1800_0X2	0x2
#define	SAR_CPU1800_0X3	0x3
#define	SAR_CPU1600_0X4	0x4
#define	SAR_CPU1600_0X5	0x5
#define	SAR_CPU1300_0X6	0x6
#define	SAR_CPU1300_0X7	0x7
#define	SAR_CPU1200_TBD	0xff /* TODO: find correct value */
#else/*EFUSE*/
#define	SAR_CPU1600_0X0	0x0
#define	SAR_CPU1600_0X1	0x1
#define	SAR_CPU1000_0X2	0x2
#define	SAR_CPU1200_0X3	0x3
#define	SAR_CPU1400_0X4	0x4
#define	SAR_CPU600_0X5	0x5
#define	SAR_CPU800_0X6	0x6
#define	SAR_CPU1000_0X7	0x7
#endif

#define DFS_LOW_FREQ_VALUE		100
#define SDRAM_CS_SIZE			0x7fffffffUL	/* FIXME: implement a function for cs size for each platform */

/* right now, we're not supporting this in mainline */
#undef SUPPORT_STATIC_DUNIT_CONFIG

/* Controler bus divider 1 for 32 bit, 2 for 64 bit */
#define DDR_CONTROLLER_BUS_WIDTH_MULTIPLIER	1

/* Tune internal training params values */
#define TUNE_TRAINING_PARAMS_CK_DELAY		160

#define TUNE_TRAINING_PARAMS_PHYREG3VAL		0xA
#define TUNE_TRAINING_PARAMS_PRI_DATA		123
#define TUNE_TRAINING_PARAMS_NRI_DATA		123
#define TUNE_TRAINING_PARAMS_PRI_CTRL		74
#define TUNE_TRAINING_PARAMS_NRI_CTRL		74
#define TUNE_TRAINING_PARAMS_P_ODT_DATA		45
#define TUNE_TRAINING_PARAMS_N_ODT_DATA		45
#define TUNE_TRAINING_PARAMS_P_ODT_CTRL		45
#define TUNE_TRAINING_PARAMS_N_ODT_CTRL		45
#define TUNE_TRAINING_PARAMS_DIC		0x2
#define TUNE_TRAINING_PARAMS_ODT_CONFIG_2CS	0x120012
#define TUNE_TRAINING_PARAMS_ODT_CONFIG_1CS	0x10000
#define TUNE_TRAINING_PARAMS_RTT_NOM		0x44
#define TUNE_TRAINING_PARAMS_RTT_WR_1CS		0x0
#define TUNE_TRAINING_PARAMS_RTT_WR_2CS		0x0

#if defined(CONFIG_DDR4)
#define TUNE_TRAINING_PARAMS_P_ODT_DATA_DDR4	0xD
#define TUNE_TRAINING_PARAMS_DIC_DDR4		0x0
#define TUNE_TRAINING_PARAMS_ODT_CONFIG_DDR4	0x330012
#define TUNE_TRAINING_PARAMS_RTT_NOM_DDR4	0x400 /*RZQ/3 = 0x600*/
#define TUNE_TRAINING_PARAMS_RTT_WR		0x200 /*RZQ/1 = 0x400*/
#define TUNE_TRAINING_PARAMS_RTT_PARK		0x40	/* rtt park = 0x1 RZQ_div4 */
#else /* CONFIG_DDR4 */
#define TUNE_TRAINING_PARAMS_RTT_WR		0x0   /*off*/
#endif /* CONFIG_DDR4 */

#define MARVELL_BOARD				MARVELL_BOARD_ID_BASE

#define MAX_DQ_NUM				72

/* Subphy result control per byte registers */
#define RESULT_CONTROL_BYTE_PUP_0_REG	0x1830
#define RESULT_CONTROL_BYTE_PUP_1_REG	0x1834
#define RESULT_CONTROL_BYTE_PUP_2_REG	0x1838
#define RESULT_CONTROL_BYTE_PUP_3_REG	0x183C
#define RESULT_CONTROL_BYTE_PUP_4_REG	0x18B0
#define RESULT_CONTROL_BYTE_PUP_5_REG	0x18B4
#define RESULT_CONTROL_BYTE_PUP_6_REG	0x18B8
#define RESULT_CONTROL_BYTE_PUP_7_REG	0x18BC
#define RESULT_CONTROL_BYTE_PUP_8_REG	0x18C0

/* Subphy result control per bit registers */
#define RESULT_CONTROL_PUP_0_BIT_0_REG	0x18C4
#define RESULT_CONTROL_PUP_0_BIT_1_REG	0x18C8
#define RESULT_CONTROL_PUP_0_BIT_2_REG	0x18CC
#define RESULT_CONTROL_PUP_0_BIT_3_REG	0x18F0
#define RESULT_CONTROL_PUP_0_BIT_4_REG	0x18F4
#define RESULT_CONTROL_PUP_0_BIT_5_REG	0x18F8
#define RESULT_CONTROL_PUP_0_BIT_6_REG	0x18FC
#define RESULT_CONTROL_PUP_0_BIT_7_REG	0x1930

#define RESULT_CONTROL_PUP_1_BIT_0_REG	0x1934
#define RESULT_CONTROL_PUP_1_BIT_1_REG	0x1938
#define RESULT_CONTROL_PUP_1_BIT_2_REG	0x193C
#define RESULT_CONTROL_PUP_1_BIT_3_REG	0x19B0
#define RESULT_CONTROL_PUP_1_BIT_4_REG	0x19B4
#define RESULT_CONTROL_PUP_1_BIT_5_REG	0x19B8
#define RESULT_CONTROL_PUP_1_BIT_6_REG	0x19BC
#define RESULT_CONTROL_PUP_1_BIT_7_REG	0x19C0

#define RESULT_CONTROL_PUP_2_BIT_0_REG	0x19C4
#define RESULT_CONTROL_PUP_2_BIT_1_REG	0x19C8
#define RESULT_CONTROL_PUP_2_BIT_2_REG	0x19CC
#define RESULT_CONTROL_PUP_2_BIT_3_REG	0x19F0
#define RESULT_CONTROL_PUP_2_BIT_4_REG	0x19F4
#define RESULT_CONTROL_PUP_2_BIT_5_REG	0x19F8
#define RESULT_CONTROL_PUP_2_BIT_6_REG	0x19FC
#define RESULT_CONTROL_PUP_2_BIT_7_REG	0x1A30

#define RESULT_CONTROL_PUP_3_BIT_0_REG	0x1A34
#define RESULT_CONTROL_PUP_3_BIT_1_REG	0x1A38
#define RESULT_CONTROL_PUP_3_BIT_2_REG	0x1A3C
#define RESULT_CONTROL_PUP_3_BIT_3_REG	0x1AB0
#define RESULT_CONTROL_PUP_3_BIT_4_REG	0x1AB4
#define RESULT_CONTROL_PUP_3_BIT_5_REG	0x1AB8
#define RESULT_CONTROL_PUP_3_BIT_6_REG	0x1ABC
#define RESULT_CONTROL_PUP_3_BIT_7_REG	0x1AC0

#define RESULT_CONTROL_PUP_4_BIT_0_REG	0x1AC4
#define RESULT_CONTROL_PUP_4_BIT_1_REG	0x1AC8
#define RESULT_CONTROL_PUP_4_BIT_2_REG	0x1ACC
#define RESULT_CONTROL_PUP_4_BIT_3_REG	0x1AF0
#define RESULT_CONTROL_PUP_4_BIT_4_REG	0x1AF4
#define RESULT_CONTROL_PUP_4_BIT_5_REG	0x1AF8
#define RESULT_CONTROL_PUP_4_BIT_6_REG	0x1AFC
#define RESULT_CONTROL_PUP_4_BIT_7_REG	0x1B30

#define RESULT_CONTROL_PUP_5_BIT_0_REG	0x1B34
#define RESULT_CONTROL_PUP_5_BIT_1_REG	0x1B38
#define RESULT_CONTROL_PUP_5_BIT_2_REG	0x1B3C
#define RESULT_CONTROL_PUP_5_BIT_3_REG	0x1BB0
#define RESULT_CONTROL_PUP_5_BIT_4_REG	0x1BB4
#define RESULT_CONTROL_PUP_5_BIT_5_REG	0x1BB8
#define RESULT_CONTROL_PUP_5_BIT_6_REG	0x1BBC
#define RESULT_CONTROL_PUP_5_BIT_7_REG	0x1BC0

#define RESULT_CONTROL_PUP_6_BIT_0_REG	0x1BC4
#define RESULT_CONTROL_PUP_6_BIT_1_REG	0x1BC8
#define RESULT_CONTROL_PUP_6_BIT_2_REG	0x1BCC
#define RESULT_CONTROL_PUP_6_BIT_3_REG	0x1BF0
#define RESULT_CONTROL_PUP_6_BIT_4_REG	0x1BF4
#define RESULT_CONTROL_PUP_6_BIT_5_REG	0x1BF8
#define RESULT_CONTROL_PUP_6_BIT_6_REG	0x1BFC
#define RESULT_CONTROL_PUP_6_BIT_7_REG	0x1C30

#define RESULT_CONTROL_PUP_7_BIT_0_REG	0x1C34
#define RESULT_CONTROL_PUP_7_BIT_1_REG	0x1C38
#define RESULT_CONTROL_PUP_7_BIT_2_REG	0x1C3C
#define RESULT_CONTROL_PUP_7_BIT_3_REG	0x1CB0
#define RESULT_CONTROL_PUP_7_BIT_4_REG	0x1CB4
#define RESULT_CONTROL_PUP_7_BIT_5_REG	0x1CB8
#define RESULT_CONTROL_PUP_7_BIT_6_REG	0x1CBC
#define RESULT_CONTROL_PUP_7_BIT_7_REG	0x1CC0

#define RESULT_CONTROL_PUP_8_BIT_0_REG	0x1CC4
#define RESULT_CONTROL_PUP_8_BIT_1_REG	0x1CC8
#define RESULT_CONTROL_PUP_8_BIT_2_REG	0x1CCC
#define RESULT_CONTROL_PUP_8_BIT_3_REG	0x1CF0
#define RESULT_CONTROL_PUP_8_BIT_4_REG	0x1CF4
#define RESULT_CONTROL_PUP_8_BIT_5_REG	0x1CF8
#define RESULT_CONTROL_PUP_8_BIT_6_REG	0x1CFC
#define RESULT_CONTROL_PUP_8_BIT_7_REG	0x1D30

#define ODPG_TRAINING_STATUS_REG	0x1030
#define ODPG_ENABLE_REG			0x1630
#define ODPG_ENABLE_OFFS		31
#define ODPG_DISABLE_OFFS		30

/* Matrix enables DRAM modes (bus width/ECC) per boardId */
#define TOPOLOGY_UPDATE_32BIT			0
#define TOPOLOGY_UPDATE_32BIT_ECC		1
#define TOPOLOGY_UPDATE_16BIT			2
#define TOPOLOGY_UPDATE_16BIT_ECC		3
#define TOPOLOGY_UPDATE_16BIT_ECC_PUP3		4
#define TOPOLOGY_UPDATE { \
		/* 32Bit, 32bit ECC, 16bit, 16bit ECC PUP4, 16bit ECC PUP3 */ \
		{1, 1, 1, 1, 1},	/* RD_NAS_68XX_ID */ \
		{1, 1, 1, 1, 1},	/* DB_68XX_ID	  */ \
		{1, 0, 1, 0, 1},	/* RD_AP_68XX_ID  */ \
		{1, 0, 1, 0, 1},	/* DB_AP_68XX_ID  */ \
		{1, 0, 1, 0, 1},	/* DB_GP_68XX_ID  */ \
		{0, 0, 1, 1, 0},	/* DB_BP_6821_ID  */ \
		{1, 1, 1, 1, 1}		/* DB_AMC_6820_ID */ \
	};

enum {
	CPU_1066MHZ_DDR_400MHZ,
	CPU_RESERVED_DDR_RESERVED0,
	CPU_667MHZ_DDR_667MHZ,
	CPU_800MHZ_DDR_800MHZ,
	CPU_RESERVED_DDR_RESERVED1,
	CPU_RESERVED_DDR_RESERVED2,
	CPU_RESERVED_DDR_RESERVED3,
	LAST_FREQ
};

enum {
	LPDDR4_DIMM,
	LPDDR4_BRD,
	DDR4_DIMM,
	DDR4_BRD,
	DDR3_DIMM,
	DDR3_BRD,
	LAST_DDR_CFG_OPT
};

/* struct used for DLB configuration array */
struct dlb_config {
	u32 reg_addr;
	u32 reg_data;
};

#define ACTIVE_INTERFACE_MASK			0x1

extern u32 dmin_phy_reg_table[][2];
extern u16 odt_slope[];
extern u16 odt_intercept[];

int mv_ddr_pre_training_soc_config(const char *ddr_type);
int mv_ddr_post_training_soc_config(const char *ddr_type);
int mv_ddr_mc6_init_controller(void);	/* FIXME: remove to mc6 driver when implemented */

#ifdef CONFIG_MC_STATIC
int mv_ddr_mc_static_config(void);
#endif /* CONFIG_MC_STATIC */

#ifdef CONFIG_PHY_STATIC
void mv_ddr_phy_static_config(void);
#endif /* CONFIG_PHY_STATIC */
/*
 * TODO: dq to pad mapping detection code to be relocated
 * to the generic part of mv_ddr code.
 */
#if 0 /* unmask to enable dq to pad mapping detection */
#define MV_DDR_DQ_MAPPING_DETECT
#endif

#if defined(MV_DDR_DQ_MAPPING_DETECT)
#define MV_DDR_DQ_MAPPING_DETECT_VERBOSE 0
int mv_ddr_dq_mapping_detect(u32 dev_num);
#endif
void mv_ddr_mem_scrubbing(void);
int apn806_rev_id_get(void);
#endif /* _MV_DDR_AP806_H */
