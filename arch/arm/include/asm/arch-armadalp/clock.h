/*
 * ***************************************************************************
 * Copyright (C) 2015 Marvell International Ltd.
 * ***************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************
 */

#ifndef _MVEBU_CLOCK_H_
#define _MVEBU_CLOCK_H_

/* Spread Spectrum Clock */
#undef MVEBU_A3700_ENABLE_SSC

#ifdef MVEBU_A3700_ENABLE_SSC
/* In order to use the SSC function, the KVCO must be operated above 2 GHz */
#define MVEBU_KVCO_MIN		2000
#else
#define MVEBU_KVCO_MIN		1200
#endif /* MVEBU_A3700_ENABLE_SSC */

#define MVEBU_KVCO_MAX		3000

/****************/
/* REF Clock    */
/****************/
#define MVEBU_TEST_PIN_LATCH_N		(MVEBU_TESTPIN_NORTH_REG_BASE + 0x8)
#define MVEBU_XTAL_MODE_MASK		BIT9
#define MVEBU_XTAL_MODE_OFFS		9
#define MVEBU_XTAL_CLOCK_25MHZ		(0x0)
#define MVEBU_XTAL_CLOCK_40MHZ		(0x1)

/****************/
/* North Bridge */
/****************/
#define MVEBU_NORTH_BRG_PLL_BASE		(MVEBU_NORTH_CLOCK_REGS_BASE + 0x200)
#define MVEBU_NORTH_BRG_TBG_CFG			(MVEBU_NORTH_BRG_PLL_BASE + 0x0)
#define MVEBU_NORTH_BRG_TBG_CTRL0		(MVEBU_NORTH_BRG_PLL_BASE + 0x4)
#define MVEBU_NORTH_BRG_TBG_CTRL1		(MVEBU_NORTH_BRG_PLL_BASE + 0x8)
#define MVEBU_NORTH_BRG_TBG_CTRL2		(MVEBU_NORTH_BRG_PLL_BASE + 0xC)
#define MVEBU_NORTH_BRG_TBG_CTRL3		(MVEBU_NORTH_BRG_PLL_BASE + 0x10)
#define MVEBU_NORTH_BRG_TBG_CTRL4		(MVEBU_NORTH_BRG_PLL_BASE + 0x14)
#define MVEBU_NORTH_BRG_TBG_CTRL5		(MVEBU_NORTH_BRG_PLL_BASE + 0x18)
#define MVEBU_NORTH_BRG_TBG_CTRL6		(MVEBU_NORTH_BRG_PLL_BASE + 0x1C)
#define MVEBU_NORTH_BRG_TBG_CTRL7		(MVEBU_NORTH_BRG_PLL_BASE + 0x20)
#define MVEBU_NORTH_BRG_TBG_CTRL8		(MVEBU_NORTH_BRG_PLL_BASE + 0x30)

#define MVEBU_NORTH_CLOCK_TBG_SELECT_REG	(MVEBU_NORTH_CLOCK_REGS_BASE + 0x0)
#define NB_CLK_TBG_SEL_MASK_ALL			(0x3F << 26)

/* north bridge clock divider select registers */
#define MVEBU_NORTH_CLOCK_DIVIDER_SELECT0_REG	(MVEBU_NORTH_CLOCK_REGS_BASE + 0x4)
#define NB_CLK_DIV0_MASK_ALL			(BIT31 | (0x3 << 26) | BIT19 | (0x3F << 6))

#define MVEBU_NORTH_CLOCK_DIVIDER_SELECT1_REG	(MVEBU_NORTH_CLOCK_REGS_BASE + 0x8)
#define NB_CLK_DIV1_MASK_ALL			((0x3 << 30) | (0x7 << 21))

#define MVEBU_NORTH_CLOCK_DIVIDER_SELECT2_REG	(MVEBU_NORTH_CLOCK_REGS_BASE + 0xC)
#define NB_CLK_DIV2_MASK_ALL			(BIT31 | (0xF << 3))
#define CPU_CNT_CLK_DIV_SEL_OFFS		(27)

/* north bridge clock source register */
#define MVEBU_NORTH_CLOCK_SELECT_REG		(MVEBU_NORTH_CLOCK_REGS_BASE + 0x10)
#define NB_CLK_SEL_MASK_ALL			((0xFFFF << 16) | BIT14)

/* north bridge clock enable register */
#define MVEBU_NORTH_CLOCK_ENABLE_REG		(MVEBU_NORTH_CLOCK_REGS_BASE + 0x14)
#define NB_ALL_CLK_ENABLE			(0x0)

/****************/
/* South Bridge */
/****************/

/* south bridge clock enable register */
#define MVEBU_SOUTH_CLOCK_TBG_SELECT_REG	(MVEBU_SOUTH_CLOCK_REGS_BASE + 0x0)
#define SB_CLK_TBG_SEL_MASK_ALL			((0x3FF << 22) | (0x3 << 12) | 0x3F)
#define GBE_125_TBG_SEL_OFFS			(10)

#define MVEBU_SOUTH_CLOCK_DIVIDER_SELECT0_REG	(MVEBU_SOUTH_CLOCK_REGS_BASE + 0x4)
#define SB_CLK_DIV0_MASK_ALL			((0x1F << 27) | 0x7)
#define AXI_CLK_PRSCL1_OFFS			(24)
#define AXI_CLK_PRSCL2_OFFS			(21)
#define USB32_SS_SYS_CLK_PRSCL1_OFFS		(18)
#define USB32_SS_SYS_CLK_PRSCL2_OFFS		(15)
#define USB32_USB2_SYS_CLK_PRSCL1_OFFS		(12)
#define USB32_USB2_SYS_CLK_PRSCL2_OFFS		(9)

#define MVEBU_SOUTH_CLOCK_DIVIDER_SELECT1_REG	(MVEBU_SOUTH_CLOCK_REGS_BASE + 0x8)
#define SB_CLK_DIV1_MASK_ALL			((0xFF << 24) | (0x7 << 15) | 0x3F)
#define GBE_125_CLK_PRSCL_MASK			(0x7)
#define GBE_125_CLK_PRSCL1_OFF			(9)
#define GBE_125_CLK_PRSCL2_OFF			(6)

#define MVEBU_SOUTH_CLOCK_DIVIDER_SELECT2_REG	(MVEBU_SOUTH_CLOCK_REGS_BASE + 0xC)
#define SB_CLK_DIV2_MASK_ALL			((0xFFFFF << 12) | 0x3F)

/* south bridge clock source register */
#define MVEBU_SOUTH_CLOCK_SELECT_REG		(MVEBU_SOUTH_CLOCK_REGS_BASE + 0x10)
#define SB_AXI_CLK_SEL_OFFS			(10)
#define USB32_SS_SYS_CLK_SEL_OFFS		(9)
#define USB32_USB2_SYS_CLK_SEL_OFFS		(8)
#define SDIO_CLK_SEL_OFFS			(7)
#define GBE_CORE_CLK_SEL_OFFS			(5)
#define GBE_125_CLK_SEL_OFFS			(3)
#define GBE_50_CLK_SEL_OFFS			(1)


/* south bridge clock enable register */
#define MVEBU_SOUTH_CLOCK_ENABLE_REG		(MVEBU_SOUTH_CLOCK_REGS_BASE + 0x14)
/* (GBE0 and GBE1 have reversed logic) */
#define SB_ALL_CLK_ENABLE			(BIT19 | BIT20)


enum a3700_clock_src {
	TBG_A,
	TBG_B
};

enum a3700_clock_line {
	TBG_A_P = 0,
	TBG_B_P = 1,
	TBG_A_S = 2,
	TBG_B_S = 3
};

/*
 * North Bridge Clocks
 */
/* North Bridge clock line selectors (0x13000) */
struct a3700_nb_clock_sel {
	enum a3700_clock_line mmc_pclk_sel;		/* [1:0] */
	enum a3700_clock_line sata_host_pclk_sel;	/* [3:2] */
	enum a3700_clock_line sec_at_pclk_sel;		/* [5:4] */
	enum a3700_clock_line sec_dap_pclk_sel;		/* [7:6] */
	enum a3700_clock_line tsecm_pclk_sel;		/* [9:8] */
	enum a3700_clock_line setm_tmx_pclk_sel;	/* [11:10] */
	enum a3700_clock_line sqf_pclk_sel;		/* [13:12] */
	enum a3700_clock_line pwm_pclk_sel;		/* [15:14] */
	enum a3700_clock_line ddr_fclk_pclk_sel;	/* [17:16] */
	enum a3700_clock_line trace_pclk_sel;		/* [19:18] */
	enum a3700_clock_line counter_pclk_sel;		/* [21:20] */
	enum a3700_clock_line a53_cpu_pclk_sel;		/* [23:22] */
	enum a3700_clock_line eip97_pclk_sel;		/* [25:24] */
};

/* North Bridge clock divider 0 (0x13004) */
struct a3700_nb_clock_div_0 {
	u8 pwm_clk_prscl2;	/* [2:0] */
	u8 pwm_clk_prscl1;	/* [5:3] */
	u8 ddr_fclk_clk_prscl2;	/* [14:12] */
	u8 ddr_fclk_clk_prscl1;	/* [17:15] */
	u8 ddr_phy_mck_div_sel;	/* [18] */
	u8 trace_div_sel;	/* [22:20] */
	u8 counter_clk_prscl;	/* [25:23] */
	u8 a53_cpu_clk_prscl;	/* [30:28] */
};

/* North Bridge clock divider 1 (0x13008) */
struct a3700_nb_clock_div_1 {
	u8 sec_at_clk_prscl2;	/* [2:0] */
	u8 sec_at_clk_prscl1;	/* [5:3] */
	u8 sec_dap_clk_prscl2;	/* [8:6] */
	u8 sec_dap_clk_prscl1;	/* [11:9] */
	u8 tsecm_clk_prscl2;	/* [14:12] */
	u8 tsecm_clk_prscl1;	/* [17:15] */
	u8 setm_tmx_clk_prscl;	/* [20:18] */
	u8 sqf_clk_prscl2;	/* [26:24] */
	u8 sqf_clk_prscl1;	/* [29:27] */
};

/* North Bridge clock divider 2 (0x1300C) */
struct a3700_nb_clock_div_2 {
	u8 wc_ahb_div_sel;	/* [2:0] */
	u8 sata_h_clk_prscl2;	/* [9:7] */
	u8 sata_h_clk_prscl1;	/* [12:10] */
	u8 mmc_clk_prscl2;	/* [15:13] */
	u8 mmc_clk_prscl1;	/* [18:16] */
	u8 eip97_clk_prscl2;	/* [21:19] */
	u8 eip97_clk_prscl1;	/* [24:22] */
	u8 atb_clk_div_sel;	/* [26:25] */
	u8 cpu_cnt_clk_div_sel;	/* [28:27] */
	u8 plkdbg_clk_div_sel;	/* [30:29] */
};

/* North Bridge clock configuration */
struct a3700_nb_clock_cfg {
	struct a3700_nb_clock_sel	clock_sel;
	struct a3700_nb_clock_div_0	div0;
	struct a3700_nb_clock_div_1	div1;
	struct a3700_nb_clock_div_2	div2;
};

/*
 * South Bridge Clocks
 */
 /* South Bridge clock line selectors (0x18000) */
struct a3700_sb_clock_sel {
	enum a3700_clock_line gbe_50_pclk_sel;		/* [7:6] */
	enum a3700_clock_line gbe_core_pclk_sel;	/* [9:8] */
	enum a3700_clock_line gbe_125_pclk_sel;		/* [11:10] */
	enum a3700_clock_line sdio_pclk_sel;		/* [15:14] */
	enum a3700_clock_line usb32_usb2_sys_pclk_sel;	/* [17:16] */
	enum a3700_clock_line usb32_ss_sys_pclk_sel;	/* [19:18] */
	enum a3700_clock_line sb_axi_pclk_sel;		/* [21:20] */
};

/* South Bridge clock divider 0 (0x18004) */
struct a3700_sb_clock_div_0 {
	u8 sdio_clk_prscl2;		/* [5:3] */
	u8 sdio_clk_prscl1;		/* [8:6] */
	u8 usb32_usb2_sys_clk_prscl2;	/* [11:9] */
	u8 usb32_usb2_sys_clk_prscl1;	/* [14:12] */
	u8 usb32_ss_sys_clk_prscl2;	/* [17:15] */
	u8 usb32_ss_sys_clk_prscl1;	/* [20:18] */
	u8 sb_axi_clk_prscl2;		/* [23:21] */
	u8 sb_axi_clk_prscl1;		/* [24:24] */
};

/* South Bridge clock divider 1 (0x18008) */
struct a3700_sb_clock_div_1 {
	u8 gbe_125_clk_prscl2;		/* [8:6] */
	u8 gbe_125_clk_prscl1;		/* [11:9] */
	u8 gbe_bm_core_clk_div2_sel;	/* [12] */
	u8 gbe1_core_clk_div2_sel;	/* [13] */
	u8 gbe0_core_clk_div2_sel;	/* [14] */
	u8 gbe_core_clk_prscl2;		/* [20:18] */
	u8 gbe_core_clk_prscl1;		/* [23:21] */
};

/* South Bridge clock divider 2 (0x1800C) */
struct a3700_sb_clock_div_2 {
	u8 gbe_50_clk_prscl2;		/* [8:6] */
	u8 gbe_50_clk_prscl1;		/* [11:9] */
};

/* South Bridge clock configuration */
struct a3700_sb_clock_cfg {
	struct a3700_sb_clock_sel	clock_sel;
	struct a3700_sb_clock_div_0	div0;
	struct a3700_sb_clock_div_1	div1;
	struct a3700_sb_clock_div_2	div2;
};

struct a3700_tbg_cfg {
	u32	kvco_mhz;
	u32	se_vcodiv;
	u32	diff_vcodiv;
};

/* System-wide clock configuration */
struct a3700_clock_cfg {
	u32				cpu_freq_mhz;
	u32				ddr_freq_mhz;
	struct a3700_tbg_cfg		tbg_a;
	struct a3700_tbg_cfg		tbg_b;
	struct a3700_nb_clock_cfg	nb_clk_cfg;
	struct a3700_sb_clock_cfg	sb_clk_cfg;
};

/* This index points to configuration selected in MVEBU_A3700_CLOCK_CFGS
   when DT entry is missing or invalid */
#define MVEBU_A3700_DEF_CLOCK_PRESET_IDX	1

/* Init values for the static clock configurations array */
/*
*************************************************************************************************************************************
Configuration 0 – CPU 600, DDR 600

		SE DIV	DIFF DIV
KVCO-A	2400	1	1	← 2^n
KVCO-B	2000	1	1	← 2^n						NB				SB
=====================================================================================================================================
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-A-S	1200	3	1	sdio_pclk_sel		400	SB					DIV0[8:6]	DIV0[5:3]
	2^n →	1	NA	ddr_phy_mck_div_sel	600	NB	DIV0[18]	NA
		3	1	mmc_pclk_sel		400	NB	DIV2[15:13]	DIV2[18:16]
-------------------------------------------------------------------------------------------------------------------------------------
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-A-P	1200	2	NA	a53_cpu_pclk_sel	600	NB	DIV0[30:28]	NA
		2	NA	wc_ahb_div_sel		300	NB	DIV2[2:0]	NA
	2^n →	0	NA	atb_clk_div_sel		600	NB	DIV2[26:5]	NA
	2^n →	1	NA	plkdbg_clk_div_sel	300	NB	DIV2[30:29]	NA
		2	NA	setm_tmx_pclk_sel	600	NB	DIV1[20:18]	NA
		2	NA	trace_pclk_sel		600	NB	DIV0[22:20]	NA
		3	2	tsecm_pclk_sel		200	NB	DIV1[14:12]	DIV1[17:15]
		3	2	sqf_pclk_sel		200	NB	DIV1[26:24]	DIV1[29:27]
-------------------------------------------------------------------------------------------------------------------------------------
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-B_S	1000	2	1	eip97_pclk_sel		500	NB	DIV2[24:22]	DIV2[21:19]
		5	2	pwm_pclk_sel		100	NB	DIV0[5:3]	DIV0[2:0]
		5	1	sec_at_pclk_sel		200	NB	DIV1[5:3]	DIV1[2:0]
		5	2	sec_dap_pclk_sel	100	NB	DIV1[11:9]	DIV1[8:6]
		5	1	sata_host_pclk_sel	200	NB	DIV2[9:7]	DIV2[12:10]
		4	2	ddr_fclk_pclk_sel	125	NB	DIV0[17:15]	DIV0[14:12]
		6	NA	counter_pclk_sel	167	NB	DIV0[25:23]	NA
	2^n →	1	NA	cpu_cnt_clk_div_sel	83	NB	DIV2[28:27]	NA
		4	1	gbe_core_pclk_sel	250	SB					DIV1[20:18]	DIV1[23:21]
		1	NA	gbe_bm_core_clk_div2_sel250	SB					DIV1[12]	NA
		1	NA	gbe1_core_clk_div2_sel	250	SB					DIV1[13]	NA
		1	NA	gbe0_core_clk_div2_sel	250	SB					DIV1[14]	NA
		4	2	gbe_125_pclk_sel	125	SB					DIV1[11:9]	DIV1[8:6]
		5	1	sb_axi_pclk_sel		200	SB					DIV0[20:18]	DIV0[23:21]
		4	2	usb32_ss_sys_pclk_sel	125	SB					DIV0[17:15]	DIV0[20:18]
		5	2	usb32_usb2_sys_pclk_sel	100	SB					DIV0[14:12]	DIV0[11:9]
-------------------------------------------------------------------------------------------------------------------------------------
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-B-P	1000	4	5	gbe_50_pclk_sel		50	SB					DIV2[11:9]	DIV2[8:6]
*************************************************************************************************************************************
*************************************************************************************************************************************
Configuration 1 – CPU 400, DDR 600

		SE DIV	DIFF DIV
KVCO-A	2400	1	1	← 2^n
KVCO-B	1600	1	1	← 2^n						NB				SB
=====================================================================================================================================
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-A-S	1200	5	2	sec_at_pclk_sel		120	NB	DIV1[5:3]	DIV1[2:0]
		5	4	sec_dap_pclk_sel	60	NB	DIV1[11:9]	DIV1[8:6]
		5	4	pwm_pclk_sel		60	NB	DIV0[5:3]	DIV0[2:0]
		2	6	ddr_fclk_pclk_sel	100	NB	DIV0[17:15]	DIV0[14:12]
		1	4	eip97_pclk_sel		300	NB	DIV2[24:22]	DIV2[21:19]
		2	2	gbe_core_pclk_sel	300	SB					DIV1[20:18]	DIV1[23:21]
		1	NA	gbe_bm_core_clk_div2_sel300	SB					DIV1[12]	NA
		1	NA	gbe1_core_clk_div2_sel	300	SB					DIV1[13]	NA
		1	NA	gbe0_core_clk_div2_sel	300	SB					DIV1[14]	NA
		5	1	sdio_pclk_sel		240	SB					DIV0[8:6]	DIV0[5:3]
		4	5	usb32_usb2_sys_pclk_sel	60	SB					DIV0[14:12]	DIV0[11:9]
		3	4	usb32_ss_sys_pclk_sel	100	SB					DIV0[17:15]	DIV0[20:18]
	2^n →	1	NA	ddr_phy_mck_div_sel	600	NB	DIV0[18]	NA
-------------------------------------------------------------------------------------------------------------------------------------
		PRE-1	PRE-2	TARGET	FREQ		PRE-1	BR	PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-A-P	1200	4	5	gbe_50_pclk_sel		60	SB					DIV2[11:9]	DIV2[8:6]
-------------------------------------------------------------------------------------------------------------------------------------
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-B_S	800	4	1	mmc_pclk_sel		200	NB	DIV2[15:13]	DIV2[18:16]
		5	1	sata_host_pclk_sel	160	NB	DIV2[9:7]	DIV2[12:10]
		5	1	tsecm_pclk_sel		160	NB	DIV1[14:12]	DIV1[17:15]
		2	NA	setm_tmx_pclk_sel	400	NB	DIV1[20:18]	NA
		5	1	sqf_pclk_sel		160	NB	DIV1[26:24]	DIV1[29:27]
		2	NA	trace_pclk_sel		400	NB	DIV0[22:20]	NA
		5	NA	counter_pclk_sel	160	NB	DIV0[25:23]	NA
	2^n →	1	NA	cpu_cnt_clk_div_sel	80	NB	DIV2[28:27]	NA
		2	NA	a53_cpu_pclk_sel	400	NB	DIV0[30:28]	NA
		2	NA	wc_ahb_div_sel		200	NB	DIV2[2:0]	NA
	2^n →	0	NA	atb_clk_div_sel		400	NB	DIV2[26:5]	NA
	2^n →	1	NA	plkdbg_clk_div_sel	200	NB	DIV2[30:29]	NA
		3	2	gbe_125_pclk_sel	133	SB					DIV1[11:9]	DIV1[8:6]
		2	2	sb_axi_pclk_sel		200	SB					DIV0[20:18]	DIV0[23:21]
-------------------------------------------------------------------------------------------------------------------------------------
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-B-P	800
*************************************************************************************************************************************
*************************************************************************************************************************************
Configuration 2 – CPU 1000, DDR 800

		SE DIV	DIFF DIV
KVCO-A	1600	0	1	← 2^n
KVCO-B	2000	1	2	← 2^n						NB				SB
=====================================================================================================================================
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-A-S	1600	4	1	mmc_pclk_sel		400	NB	DIV2[15:13]	DIV2[18:16]
	2^n →	1	NA	ddr_phy_mck_div_sel	800	NB	DIV0[18]	NA
		4	1	sdio_pclk_sel		400	SB					DIV0[8:6]	DIV0[5:3]
-------------------------------------------------------------------------------------------------------------------------------------
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-A-P	800	5	NA	counter_pclk_sel	160	NB	DIV0[25:23]	NA
	2^n →	1	NA	cpu_cnt_clk_div_sel	80	NB	DIV2[28:27]	NA
		4	1	sqf_pclk_sel		200	NB	DIV1[26:24]	DIV1[29:27]
		4	1	sata_host_pclk_sel	200	NB	DIV2[9:7]	DIV2[12:10]
		4	1	tsecm_pclk_sel		200	NB	DIV1[14:12]	DIV1[17:15]
-------------------------------------------------------------------------------------------------------------------------------------
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-B_S	1000	5	1	sec_at_pclk_sel		200	NB	DIV1[5:3]	DIV1[2:0]
		2	1	eip97_pclk_sel		500	NB	DIV2[24:22]	DIV2[21:19]
		1	NA	setm_tmx_pclk_sel	1000	NB	DIV1[20:18]	NA
		1	NA	trace_pclk_sel		1000	NB	DIV0[22:20]	NA
		1	NA	a53_cpu_pclk_sel	1000	NB	DIV0[30:28]	NA
		4	NA	wc_ahb_div_sel		250	NB	DIV2[2:0]	NA
	2^n →	0	NA	atb_clk_div_sel		1000	NB	DIV2[26:5]	NA
	2^n →	1	NA	plkdbg_clk_div_sel	500	NB	DIV2[30:29]	NA
		4	2	ddr_fclk_pclk_sel	125	NB	DIV0[17:15]	DIV0[14:12]
		5	2	sec_dap_pclk_sel	100	NB	DIV1[11:9]	DIV1[8:6]
		5	2	pwm_pclk_sel		100	NB	DIV0[5:3]	DIV0[2:0]
		4	1	gbe_core_pclk_sel	250	SB					DIV1[20:18]	DIV1[23:21]
		1	NA	gbe_bm_core_clk_div2_sel250	SB					DIV1[12]	NA
		1	NA	gbe1_core_clk_div2_sel	250	SB					DIV1[13]	NA
		1	NA	gbe0_core_clk_div2_sel	250	SB					DIV1[14]	NA
		4	2	gbe_125_pclk_sel	125	SB					DIV1[11:9]	DIV1[8:6]
		2	2	sb_axi_pclk_sel		250	SB					DIV0[20:18]	DIV0[23:21]
		4	2	usb32_ss_sys_pclk_sel	125	SB					DIV0[17:15]	DIV0[20:18]
		5	2	usb32_usb2_sys_pclk_sel	100	SB					DIV0[14:12]	DIV0[11:9]
-------------------------------------------------------------------------------------------------------------------------------------
		PRE-1	PRE-2	TARGET			FREQ	BR	PRE-1		PRE-2		PRE-1		PRE-2
-------------------------------------------------------------------------------------------------------------------------------------
TBG-B-P	500	2	5	gbe_50_pclk_sel		50	SB					DIV2[11:9]	DIV2[8:6]
*************************************************************************************************************************************
*/
/* Init values for the static clock configurations array */
#define MVEBU_A3700_CLOCK_CFGS	{\
	 /* Clock tree configuration entry 0 (testing, not yet fully functional) */\
	{ 600, 600, /* CPU, DDR */\
		{2400, 1, 1}, /* TBG-A: KVCO Freq, SE_VCODIV, DIFF_VCODIV */\
		{2000, 1, 1}, /* TBG-B: KVCO Freq, SE_VCODIV, DIFF_VCODIV */\
		/* NorthBridge */\
		{{TBG_A_S, TBG_B_S, TBG_B_S, TBG_B_S, TBG_A_P, TBG_A_P, TBG_A_P,\
		  TBG_B_S, TBG_B_S, TBG_A_P, TBG_B_S, TBG_A_P, TBG_B_S},\
		 {2, 5, 2, 4, 1, 2, 6, 2},		/* DIV0 */\
		 {1, 5, 2, 5, 2, 3, 2, 2, 3},		/* DIV1 */\
		 {2, 1, 5, 1, 3, 1, 2, 0, 1, 1} },	/* DIV2 */\
		/* SouthBridge */\
		{{TBG_B_P, TBG_B_S, TBG_B_S, TBG_A_S, TBG_B_S, TBG_B_S, TBG_B_S},\
		 {1, 3, 2, 5, 2, 4, 1, 5},	/* DIV0 */\
		 {2, 4, 1, 1, 1, 4, 1},		/* DIV1 */\
		 {5, 4} }			/* DIV2 */\
	},\
	/* Clock tree configuration entry 1 (default working setup) */\
	{ 400, 600, /* CPU, DDR */\
		{2400, 1, 1}, /* TBG-A: KVCO Freq, SE_VCODIV, DIFF_VCODIV */\
		{1600, 1, 1}, /* TBG-B: KVCO Freq, SE_VCODIV, DIFF_VCODIV */\
		/* NorthBridge */\
		{{TBG_B_S, TBG_B_S, TBG_A_S, TBG_A_S, TBG_B_S, TBG_B_S, TBG_B_S,\
		 TBG_A_S, TBG_A_S, TBG_B_S, TBG_B_S, TBG_B_S, TBG_A_S},\
		{4, 5, 6, 2, 1, 2, 5, 2},		/* DIV0 */\
		{2, 5, 4, 5, 1, 5, 2, 1, 5},		/* DIV1 */\
		{2, 1, 5, 1, 4, 1, 4, 0, 1, 1} },	/* DIV2 */\
		/* SouthBridge */\
		{{TBG_A_P, TBG_A_S, TBG_B_S, TBG_A_S, TBG_A_S, TBG_A_S, TBG_B_S},\
		{1, 5, 5, 4, 3, 4, 2, 2},	/* DIV0 */\
		{2, 3, 1, 1, 1, 2, 4},		/* DIV1 */\
		{5, 4} }			/* DIV2 */\
	},\
	/* Clock tree configuration entry 2 */\
	{ 1000, 800, /* CPU, DDR */\
		{1600, 0, 1}, /* TBG-A: KVCO Freq, SE_VCODIV, DIFF_VCODIV */\
		{2000, 1, 2}, /* TBG-B: KVCO Freq, SE_VCODIV, DIFF_VCODIV */\
		/* NorthBridge */\
		{{TBG_A_S, TBG_A_P, TBG_B_S, TBG_B_S, TBG_A_P, TBG_B_S, TBG_A_P,\
		  TBG_B_S, TBG_B_S, TBG_B_S, TBG_A_P, TBG_B_S, TBG_B_S},\
		{4, 5, 2, 4, 1, 1, 5, 1},		/* DIV0 */\
		{1, 5, 2, 5, 1, 4, 1, 1, 4},		/* DIV1 */\
		{2, 1, 4, 1, 4, 2, 1, 0, 1, 1} },	/* DIV2 */\
		/* SouthBridge */\
		{{TBG_B_P, TBG_B_S, TBG_B_S, TBG_A_S, TBG_B_S, TBG_B_S, TBG_B_S},\
		{1, 4, 2, 5, 2, 4, 2, 2},	/* DIV0 */\
		{2, 4, 1, 1, 1, 4, 1},		/* DIV1 */\
		{5, 2} }			/* DIV2 */\
	},\
}

/*****************************************************************************
* get_ref_clk
*
* return: reference clock in MHz (25 or 40)
******************************************************************************/
u32 get_ref_clk(void);

/******************************************************************************
* Name: set_clocks
*
* Description: Configure entire clock tree according to CPU and DDR frequency
*
* Input:	cpu_clk_mhz: required CPU clock
*		ddr_clk_mhz: required DDR clock
*		tbg_a_kvco_mhz: required TBG-A KVCO frequency or 0 for any available
*		tbg_b_kvco_mhz: required TBG-B KVCO frequency or 0 for any available
* Output:	None
* Return:	Non-zero if the requested settings are not supported
******************************************************************************/
u32 set_clocks(u32 cpu_clk_mhz, u32 ddr_clk_mhz, u32 tbg_a_kvco_mhz, u32 tbg_b_kvco_mhz);

int init_clock(void);

#endif /* _MVEBU_CLOCK_H_ */
