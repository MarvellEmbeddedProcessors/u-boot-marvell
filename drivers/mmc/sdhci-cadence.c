// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 */

#include <common.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/bug.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/sizes.h>
#include <linux/libfdt.h>
#include <mmc.h>
#include <sdhci.h>
#include <div64.h>

#define DEBUG_DRV(fmt, ...)	\
	if (0) \
		printf(fmt, ##__VA_ARGS__)

#define SDR_10MHZ			10000000

#define SDHCI_CDNS_SD6_MAXCLK		200000000

#define SDHCI_CDNS_HRS00			0x00
#define SDHCI_CDNS_HRS00_SWR			BIT(0)

#define SDHCI_CDNS_HRS02			0x08		/* PHY access port */
#define SDHCI_CDNS_HRS04			0x10		/* PHY access port */
/* SD 4.0 Controller HRS - Host Register Set (specific to Cadence) */
#define SDHCI_CDNS_SD4_HRS04_ACK		BIT(26)
#define SDHCI_CDNS_SD4_HRS04_RD			BIT(25)
#define SDHCI_CDNS_SD4_HRS04_WR			BIT(24)
#define SDHCI_CDNS_SD4_HRS04_RDATA		GENMASK(23, 16)
#define SDHCI_CDNS_SD4_HRS04_WDATA		GENMASK(15, 8)
#define SDHCI_CDNS_SD4_HRS04_ADDR		GENMASK(5, 0)

#define SDHCI_CDNS_HRS06			0x18		/* eMMC control */
#define SDHCI_CDNS_HRS06_TUNE_UP		BIT(15)
#define SDHCI_CDNS_HRS06_TUNE			GENMASK(13, 8)
#define SDHCI_CDNS_HRS06_MODE			GENMASK(2, 0)
#define SDHCI_CDNS_HRS06_MODE_SD		0x0
#define SDHCI_CDNS_HRS06_MODE_MMC_SDR		0x2
#define SDHCI_CDNS_HRS06_MODE_MMC_DDR		0x3
#define SDHCI_CDNS_HRS06_MODE_MMC_HS200		0x4
#define SDHCI_CDNS_HRS06_MODE_MMC_HS400		0x5
#define SDHCI_CDNS_HRS06_MODE_MMC_HS400ES	0x6

/* SD 6.0 Controller HRS - Host Register Set (Specific to Cadence) */
#define SDHCI_CDNS_SD6_HRS04_ADDR		GENMASK(15, 0)

#define SDHCI_CDNS_HRS05			0x14

#define SDHCI_CDNS_HRS07			0x1C
#define	SDHCI_CDNS_HRS07_RW_COMPENSATE		GENMASK(20, 16)
#define	SDHCI_CDNS_HRS07_IDELAY_VAL		GENMASK(4, 0)

#define SDHCI_CDNS_HRS09			0x24
#define	SDHCI_CDNS_HRS09_RDDATA_EN		BIT(16)
#define	SDHCI_CDNS_HRS09_RDCMD_EN		BIT(15)
#define	SDHCI_CDNS_HRS09_EXTENDED_WR_MODE	BIT(3)
#define	SDHCI_CDNS_HRS09_EXTENDED_RD_MODE	BIT(2)
#define	SDHCI_CDNS_HRS09_PHY_INIT_COMPLETE	BIT(1)
#define	SDHCI_CDNS_HRS09_PHY_SW_RESET		BIT(0)

#define SDHCI_CDNS_HRS10			0x28
#define	SDHCI_CDNS_HRS10_HCSDCLKADJ		GENMASK(19, 16)

#define SDHCI_CDNS_SRS11			0x2c
#define SDHCI_CDNS_SRS11_SW_RESET_ALL 		BIT(24)
#define SDHCI_CDNS_SRS11_SW_RESET_CMD 		BIT(25)
#define SDHCI_CDNS_SRS11_SW_RESET_DAT		BIT(26)

#define SDHCI_CDNS_HRS16			0x40
#define SDHCI_CDNS_HRS16_WRDATA1_SDCLK_DLY	GENMASK(31, 28)
#define SDHCI_CDNS_HRS16_WRDATA0_SDCLK_DLY	GENMASK(27, 24)
#define SDHCI_CDNS_HRS16_WRCMD1_SDCLK_DLY	GENMASK(23, 20)
#define SDHCI_CDNS_HRS16_WRCMD0_SDCLK_DLY	GENMASK(19, 16)
#define SDHCI_CDNS_HRS16_WRDATA1_DLY		GENMASK(15, 12)
#define SDHCI_CDNS_HRS16_WRDATA0_DLY		GENMASK(11, 8)
#define SDHCI_CDNS_HRS16_WRCMD1_DLY		GENMASK(7, 4)
#define SDHCI_CDNS_HRS16_WRCMD0_DLY		GENMASK(3, 0)

/* PHY registers for SD6 controller */
#define SDHCI_CDNS_SD6_PHY_DQ_TIMING				0x2000
#define	SDHCI_CDNS_SD6_PHY_DQ_TIMING_IO_MASK_ALWAYS_ON		BIT(31)
#define	SDHCI_CDNS_SD6_PHY_DQ_TIMING_IO_MASK_END		GENMASK(29, 27)
#define	SDHCI_CDNS_SD6_PHY_DQ_TIMING_IO_MASK_START		GENMASK(26, 24)
#define	SDHCI_CDNS_SD6_PHY_DQ_TIMING_DATA_SELECT_OE_END		GENMASK(2, 0)

#define SDHCI_CDNS_SD6_PHY_DQS_TIMING				0x2004
#define	SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_EXT_LPBK_DQS		BIT(22)
#define	SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_LPBK_DQS		BIT(21)
#define	SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_PHONY_DQS		BIT(20)
#define	SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_PHONY_DQS_CMD		BIT(19)

#define SDHCI_CDNS_SD6_PHY_GATE_LPBK				0x2008
#define	SDHCI_CDNS_SD6_PHY_GATE_LPBK_SYNC_METHOD		BIT(31)
#define	SDHCI_CDNS_SD6_PHY_GATE_LPBK_SW_HALF_CYCLE_SHIFT	BIT(28)
#define	SDHCI_CDNS_SD6_PHY_GATE_LPBK_RD_DEL_SEL			GENMASK(24, 19)
#define SDHCI_CDNS_SD6_PHY_GATE_LPBK_UNDERRUN_SUPPRESS		BIT(18)
#define	SDHCI_CDNS_SD6_PHY_GATE_LPBK_GATE_CFG_ALWAYS_ON		BIT(6)

#define SDHCI_CDNS_SD6_PHY_DLL_MASTER				0x200C
#define	SDHCI_CDNS_SD6_PHY_DLL_MASTER_BYPASS_MODE		BIT(23)
#define	SDHCI_CDNS_SD6_PHY_DLL_MASTER_PHASE_DETECT_SEL		GENMASK(22, 20)
#define	SDHCI_CDNS_SD6_PHY_DLL_MASTER_DLL_LOCK_NUM		GENMASK(18, 16)
#define	SDHCI_CDNS_SD6_PHY_DLL_MASTER_DLL_START_POINT		GENMASK(7, 0)

#define SDHCI_CDNS_SD6_PHY_DLL_SLAVE				0x2010
#define	SDHCI_CDNS_SD6_PHY_DLL_SLAVE_READ_DQS_CMD_DELAY		GENMASK(31, 24)
#define	SDHCI_CDNS_SD6_PHY_DLL_SLAVE_CLK_WRDQS_DELAY		GENMASK(23, 16)
#define	SDHCI_CDNS_SD6_PHY_DLL_SLAVE_CLK_WR_DELAY		GENMASK(15, 8)
#define	SDHCI_CDNS_SD6_PHY_DLL_SLAVE_READ_DQS_DELAY		GENMASK(7, 0)

#define SDHCI_CDNS_SD6_PHY_DLL_OBS_REG0				0x201C
#define SDHCI_CDNS_SD6_PHY_DLL_OBS_REG0_DLL_LOCK_MODE		GENMASK(2, 1)

#define SDHCI_CDNS_SD6_PHY_CTRL					0x2080
#define	SDHCI_CDNS_SD6_PHY_CTRL_PHONY_DQS_TIMING		GENMASK(9, 4)


/* SRS - Slot Register Set (SDHCI-compatible) */
#define SDHCI_CDNS_SRS_BASE		0x200

/* PHY registers for SD4 controller */
#define SDHCI_CDNS_PHY_DLY_SD_HS	0x00
#define SDHCI_CDNS_PHY_DLY_SD_DEFAULT	0x01
#define SDHCI_CDNS_PHY_DLY_UHS_SDR12	0x02
#define SDHCI_CDNS_PHY_DLY_UHS_SDR25	0x03
#define SDHCI_CDNS_PHY_DLY_UHS_SDR50	0x04
#define SDHCI_CDNS_PHY_DLY_UHS_DDR50	0x05
#define SDHCI_CDNS_PHY_DLY_EMMC_LEGACY	0x06
#define SDHCI_CDNS_PHY_DLY_EMMC_SDR	0x07
#define SDHCI_CDNS_PHY_DLY_EMMC_DDR	0x08
#define SDHCI_CDNS_PHY_DLY_SDCLK	0x0b
#define SDHCI_CDNS_PHY_DLY_HSMMC	0x0c
#define SDHCI_CDNS_PHY_DLY_STROBE	0x0d

/*
 * The tuned val register is 6 bit-wide, but not the whole of the range is
 * available.  The range 0-42 seems to be available (then 43 wraps around to 0)
 * but I am not quite sure if it is official.  Use only 0 to 39 for safety.
 */
#define SDHCI_CDNS_MAX_TUNING_LOOP	40

struct sdhci_cdns_phy_cfg {
	const char *property;
	u8 addr;
};

enum sdhci_cdns_sd6_phy_lock_mode {
	SDHCI_CDNS_SD6_PHY_LOCK_MODE_FULL_CLK = 0,
	SDHCI_CDNS_SD6_PHY_LOCK_MODE_HALF_CLK = 2,
	SDHCI_CDNS_SD6_PHY_LOCK_MODE_SATURATION = 3,
};

struct sdhci_cdns_sd6_phy_timings {
	u32 t_cmd_output_min;
	u32 t_cmd_output_max;
	u32 t_dat_output_min;
	u32 t_dat_output_max;
	u32 t_cmd_input_min;
	u32 t_cmd_input_max;
	u32 t_dat_input_min;
	u32 t_dat_input_max;
	u32 t_sdclk_min;
	u32 t_sdclk_max;
};

struct sdhci_cdns_sd6_phy_delays {
	u32 phy_sdclk_delay;
	u32 phy_cmd_o_delay;
	u32 phy_dat_o_delay;
	u32 iocell_input_delay;
	u32 iocell_output_delay;
	u32 delay_element_org;
	u32 delay_element;
};

struct sdhci_cdns_sd6_phy_settings {
	/* SDHCI_CDNS_SD6_PHY_DLL_SLAVE */
	u32 cp_read_dqs_cmd_delay;
	u32 cp_read_dqs_delay;
	u32 cp_clk_wr_delay;
	u32 cp_clk_wrdqs_delay;

	/* SDHCI_CDNS_SD6_PHY_DLL_MASTER */
	u32 cp_dll_bypass_mode;
	u32 cp_dll_start_point;

	/* SDHCI_CDNS_SD6_PHY_DLL_OBS_REG0 */
	u32 cp_dll_locked_mode;

	/* SDHCI_CDNS_SD6_PHY_GATE_LPBK */
	u32 cp_gate_cfg_always_on;
	u32 cp_sync_method;
	u32 cp_rd_del_sel;
	u32 cp_sw_half_cycle_shift;
	u32 cp_underrun_suppress;

	/* SDHCI_CDNS_SD6_PHY_DQ_TIMING */
	u32 cp_io_mask_always_on;
	u32 cp_io_mask_end;
	u32 cp_io_mask_start;
	u32 cp_data_select_oe_end;

	/* SDHCI_CDNS_SD6_PHY_DQS_TIMING */
	u32 cp_use_ext_lpbk_dqs;
	u32 cp_use_lpbk_dqs;
	u8 cp_use_phony_dqs;
	u8 cp_use_phony_dqs_cmd;

	/* HRS 09 */
	u8 sdhc_extended_rd_mode;
	u8 sdhc_extended_wr_mode;
	u32 sdhc_rdcmd_en;
	u32 sdhc_rddata_en;

	/* HRS10 */
	u32 sdhc_hcsdclkadj;

	/* HRS 07 */
	u32 sdhc_idelay_val;
	u32 sdhc_rw_compensate;

	/* SRS 11 */
	u32 sdhc_sdcfsh;
	u32 sdhc_sdcfsl;

	/* HRS 16 */
	u32 sdhc_wrcmd0_dly;
	u32 sdhc_wrcmd0_sdclk_dly;
	u32 sdhc_wrcmd1_dly;
	u32 sdhc_wrcmd1_sdclk_dly;
	u32 sdhc_wrdata0_dly;
	u32 sdhc_wrdata0_sdclk_dly;
	u32 sdhc_wrdata1_dly;
	u32 sdhc_wrdata1_sdclk_dly;

	u32 hs200_tune_val;
};

struct sdhci_cdns_sd6_phy_intermediate_results {
	/* TODO consider to move the following variables to out calculations */
	u32 t_sdmclk_calc;
	u32 dll_max_value;
};

struct sdhci_cdns_sd6_phy {
	struct sdhci_cdns_sd6_phy_timings t;
	struct sdhci_cdns_sd6_phy_delays d;
	u32 t_sdmclk;
	struct sdhci_cdns_sd6_phy_settings settings;
	struct sdhci_cdns_sd6_phy_intermediate_results vars;
	bool ddr;
	bool tune_cmd;
	bool tune_dat;
	bool strobe_cmd;
	bool strobe_dat;
	int mode;
	int t_sdclk;
};

struct sdhci_cdns_plat {
	struct mmc_config cfg;
	struct mmc mmc;
	void __iomem *hrs_addr;
	void *priv;
};

static struct sdhci_cdns_sd6_phy sd6_phy_config;

static void init_hs(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 2000, .t_cmd_output_max = t_sdclk - 6000,
		.t_dat_output_min = 2000, .t_dat_output_max = t_sdclk - 6000,
		.t_cmd_input_min = 14000, .t_cmd_input_max = t_sdclk + 2500,
		.t_dat_input_min = 14000, .t_dat_input_max = t_sdclk + 2500,
		.t_sdclk_min = 1000000 / 50, .t_sdclk_max = 1000000 / 0.4
	};
}

static void init_uhs_sdr12(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 800, .t_cmd_output_max = t_sdclk - 3000,
		.t_dat_output_min = 800, .t_dat_output_max = t_sdclk - 3000,
		.t_cmd_input_min = 14000, .t_cmd_input_max = t_sdclk + 1500,
		.t_dat_input_min = 14000, .t_dat_input_max = t_sdclk + 1500,
		.t_sdclk_min = 1000000 / 25, .t_sdclk_max = 1000000 / 0.4
	};
}

static void init_uhs_sdr25(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 800, .t_cmd_output_max = t_sdclk - 3000,
		.t_dat_output_min = 800, .t_dat_output_max = t_sdclk - 3000,
		.t_cmd_input_min = 14000, .t_cmd_input_max = t_sdclk + 1500,
		.t_dat_input_min = 14000, .t_dat_input_max = t_sdclk + 1500,
		.t_sdclk_min = 1000000 / 50, .t_sdclk_max = 1000000 / 0.4
	};
}

static void init_uhs_sdr50(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 800, .t_cmd_output_max = t_sdclk - 3000,
		.t_dat_output_min = 800, .t_dat_output_max = t_sdclk - 3000,
		.t_cmd_input_min = 7500, .t_cmd_input_max = t_sdclk + 1500,
		.t_dat_input_min = 7500, .t_dat_input_max = t_sdclk + 1500,
		.t_sdclk_min = 1000000 / 100, .t_sdclk_max = 1000000 / 0.4
	};
}

static void init_uhs_sdr104(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 800, .t_cmd_output_max = t_sdclk - 1400,
		.t_dat_output_min = 800, .t_dat_output_max = t_sdclk - 1400,
		.t_cmd_input_min = 1000, .t_cmd_input_max = t_sdclk + 1000,
		.t_dat_input_min = 1000, .t_dat_input_max = t_sdclk + 1000,
		.t_sdclk_min = 1000000 / 200, .t_sdclk_max = 1000000 / 100
	};
}

static void init_uhs_ddr50(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 800, .t_cmd_output_max = t_sdclk - 3000,
		.t_dat_output_min = 800, .t_dat_output_max = t_sdclk - 3000,
		.t_cmd_input_min = 13700, .t_cmd_input_max = t_sdclk + 1500,
		.t_dat_input_min = 7000, .t_dat_input_max = t_sdclk + 1500,
		.t_sdclk_min = 1000000 / 50, .t_sdclk_max = 1000000 / 0.4
	};
}

static void init_emmc_legacy(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 3000, .t_cmd_output_max = t_sdclk - 3000,
		.t_dat_output_min = 3000, .t_dat_output_max = t_sdclk - 3000,
		.t_cmd_input_min = 11700, .t_cmd_input_max = t_sdclk + 8300,
		.t_dat_input_min = 11700, .t_dat_input_max = t_sdclk + 8300,
		.t_sdclk_min = 1000000 / 25, .t_sdclk_max = 1000000 / 0.4
	};
}

static void init_emmc_sdr(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 3000, .t_cmd_output_max = t_sdclk - 3000,
		.t_dat_output_min = 3000, .t_dat_output_max = t_sdclk - 3000,
		.t_cmd_input_min = 13700, .t_cmd_input_max = t_sdclk + 2500,
		.t_dat_input_min = 13700, .t_dat_input_max = t_sdclk + 2500,
		.t_sdclk_min = 1000000 / 50, .t_sdclk_max = 1000000 / 0.4
	};
}

static void init_emmc_ddr(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 3000, .t_cmd_output_max = t_sdclk - 3000,
		.t_dat_output_min = 2500, .t_dat_output_max = t_sdclk - 2500,
		.t_cmd_input_min = 13700, .t_cmd_input_max = t_sdclk + 2500,
		.t_dat_input_min = 7000, .t_dat_input_max = t_sdclk + 1500,
		.t_sdclk_min = 1000000 / 50, .t_sdclk_max = 1000000 / 0.4
	};
}

static void init_emmc_hs200(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 800, .t_cmd_output_max = t_sdclk - 1400,
		.t_dat_output_min = 800, .t_dat_output_max = t_sdclk - 1400,
		.t_cmd_input_min = 1000, .t_cmd_input_max = t_sdclk + 1000,
		.t_dat_input_min = 1000, .t_dat_input_max = t_sdclk + 1000,
		.t_sdclk_min = 1000000 / 200, .t_sdclk_max = 1000000 / 100
	};
}

/* HS400 and HS400ES */
static void init_emmc_hs400(struct sdhci_cdns_sd6_phy_timings *t, int t_sdclk)
{
	DEBUG_DRV("%s\n", __func__);
	*t = (struct sdhci_cdns_sd6_phy_timings){
		.t_cmd_output_min = 800, .t_cmd_output_max = t_sdclk - 1400,
		.t_dat_output_min = 400, .t_dat_output_max = t_sdclk - 400,
		.t_cmd_input_min = 1000, .t_cmd_input_max = t_sdclk + 1000,
		.t_dat_input_min = 1000, .t_dat_input_max = t_sdclk + 1000,
		.t_sdclk_min = 1000000 / 200, .t_sdclk_max = 1000000 / 100
	};
}

static void (*init_timings[])(struct sdhci_cdns_sd6_phy_timings*, int) = {
	init_emmc_legacy, init_emmc_sdr, init_hs, init_uhs_sdr12,
	init_uhs_sdr25, init_uhs_sdr50, init_uhs_sdr104, init_uhs_ddr50,
	init_emmc_ddr, init_emmc_hs200, init_emmc_hs400
};

#ifdef SD4_ENABLE
static const struct sdhci_cdns_phy_cfg sdhci_cdns_sd4_phy_cfgs[] = {
	{ "cdns,phy-input-delay-sd-highspeed", SDHCI_CDNS_PHY_DLY_SD_HS, },
	{ "cdns,phy-input-delay-legacy", SDHCI_CDNS_PHY_DLY_SD_DEFAULT, },
	{ "cdns,phy-input-delay-sd-uhs-sdr12", SDHCI_CDNS_PHY_DLY_UHS_SDR12, },
	{ "cdns,phy-input-delay-sd-uhs-sdr25", SDHCI_CDNS_PHY_DLY_UHS_SDR25, },
	{ "cdns,phy-input-delay-sd-uhs-sdr50", SDHCI_CDNS_PHY_DLY_UHS_SDR50, },
	{ "cdns,phy-input-delay-sd-uhs-ddr50", SDHCI_CDNS_PHY_DLY_UHS_DDR50, },
	{ "cdns,phy-input-delay-mmc-highspeed", SDHCI_CDNS_PHY_DLY_EMMC_SDR, },
	{ "cdns,phy-input-delay-mmc-ddr", SDHCI_CDNS_PHY_DLY_EMMC_DDR, },
	{ "cdns,phy-dll-delay-sdclk", SDHCI_CDNS_PHY_DLY_SDCLK, },
	{ "cdns,phy-dll-delay-sdclk-hsmmc", SDHCI_CDNS_PHY_DLY_HSMMC, },
	{ "cdns,phy-dll-delay-strobe", SDHCI_CDNS_PHY_DLY_STROBE, },
};

static int sdhci_cdns_sd4_write_phy_reg(struct sdhci_cdns_plat *plat,
					u8 addr, u8 data)
{
	void __iomem *reg = plat->hrs_addr + SDHCI_CDNS_HRS04;
	u32 tmp;
	int ret;

	tmp = FIELD_PREP(SDHCI_CDNS_SD4_HRS04_WDATA, data) |
		FIELD_PREP(SDHCI_CDNS_SD4_HRS04_ADDR, addr);
	writel(tmp, reg);

	tmp |= SDHCI_CDNS_SD4_HRS04_WR;
	writel(tmp, reg);

	ret = readl_poll_timeout(reg, tmp, tmp & SDHCI_CDNS_SD4_HRS04_ACK, 10);
	if (ret)
		return ret;

	tmp &= ~SDHCI_CDNS_SD4_HRS04_WR;
	writel(tmp, reg);

	return 0;
}

static int sdhci_cdns_sd4_phy_init(struct sdhci_cdns_plat *plat,
				   const void *fdt, int nodeoffset)
{
	const fdt32_t *prop;
	int ret, i;

	for (i = 0; i < ARRAY_SIZE(sdhci_cdns_sd4_phy_cfgs); i++) {
		prop = fdt_getprop(fdt, nodeoffset,
				sdhci_cdns_sd4_phy_cfgs[i].property, NULL);
		if (!prop)
			continue;

		ret = sdhci_cdns_sd4_write_phy_reg(plat,
					sdhci_cdns_sd4_phy_cfgs[i].addr,
					fdt32_to_cpu(*prop));
		if (ret)
			return ret;
	}

	return 0;
}
#endif

#ifdef PHY_DEBUG
static void sdhci_cdns_sd6_phy_dump(struct sdhci_cdns_sd6_phy *phy)
{
	printf("sdhci_cdns_sd6_phy_init mode %d t_sdclk %d\n", phy->mode, phy->t_sdclk);

	printf("cp_clk_wr_delay %d\n", phy->settings.cp_clk_wr_delay);
	printf("cp_clk_wrdqs_delay %d \n", phy->settings.cp_clk_wrdqs_delay);
	printf("cp_data_select_oe_end %d\n", phy->settings.cp_data_select_oe_end);
	printf("cp_dll_bypass_mode %d\n", phy->settings.cp_dll_bypass_mode);
	printf("cp_dll_locked_mode %d\n", phy->settings.cp_dll_locked_mode);
	printf("cp_dll_start_point %d\n", phy->settings.cp_dll_start_point);
	printf("cp_io_mask_always_on %d\n", phy->settings.cp_io_mask_always_on);
	printf("cp_io_mask_end %d\n", phy->settings.cp_io_mask_end);
	printf("cp_io_mask_start %d\n", phy->settings.cp_io_mask_start);
	printf("cp_rd_del_sel %d\n", phy->settings.cp_rd_del_sel);
	printf("cp_read_dqs_cmd_delay %d\n", phy->settings.cp_read_dqs_cmd_delay);
	printf("cp_read_dqs_delay %d\n", phy->settings.cp_read_dqs_delay);
	printf("cp_sw_half_cycle_shift %d\n", phy->settings.cp_sw_half_cycle_shift);
	printf("cp_sync_method %d\n", phy->settings.cp_sync_method);
	printf("cp_use_ext_lpbk_dqs %d\n", phy->settings.cp_use_ext_lpbk_dqs);
	printf("cp_use_lpbk_dqs %d\n", phy->settings.cp_use_lpbk_dqs);
	printf("cp_use_phony_dqs %d\n", phy->settings.cp_use_phony_dqs);
	printf("cp_use_phony_dqs_cmd %d\n", phy->settings.cp_use_phony_dqs_cmd);
	printf("sdhc_extended_rd_mode %d\n", phy->settings.sdhc_extended_rd_mode);
	printf("sdhc_extended_wr_mode %d\n", phy->settings.sdhc_extended_wr_mode);

	printf("sdhc_hcsdclkadj %d\n", phy->settings.sdhc_hcsdclkadj);
	printf("sdhc_idelay_val %d\n", phy->settings.sdhc_idelay_val);
	printf("sdhc_rdcmd_en %d\n", phy->settings.sdhc_rdcmd_en);
	printf("sdhc_rddata_en %d\n", phy->settings.sdhc_rddata_en);
	printf("sdhc_rw_compensate %d\n", phy->settings.sdhc_rw_compensate);
	printf("sdhc_sdcfsh %d\n", phy->settings.sdhc_sdcfsh);
	printf("sdhc_sdcfsl %d\n", phy->settings.sdhc_sdcfsl);
	printf("sdhc_wrcmd0_dly %d %d\n", phy->settings.sdhc_wrcmd0_dly, phy->settings.sdhc_wrcmd0_sdclk_dly);
	printf("sdhc_wrcmd1_dly %d %d\n", phy->settings.sdhc_wrcmd1_dly, phy->settings.sdhc_wrcmd1_sdclk_dly);
	printf("sdhc_wrdata0_dly %d %d \n", phy->settings.sdhc_wrdata0_dly, phy->settings.sdhc_wrdata0_sdclk_dly);

	printf("sdhc_wrdata1_dly %d %d \n", phy->settings.sdhc_wrdata1_dly, phy->settings.sdhc_wrdata1_sdclk_dly);
	printf("hs200_tune_val %d\n", phy->settings.hs200_tune_val);
}
#endif

static void sdhci_cdns_sd6_calc_phy(struct sdhci_cdns_sd6_phy *phy)
{
	int mode = phy->mode;

	if (mode == SDHCI_CDNS_HRS06_MODE_MMC_SDR) {
#if defined(SDR_25MHZ)
		printf("SDR_25MHZ SDHCI_CDNS_HRS06_MODE_MMC_SDR PHY configuration\n");
		phy->settings.cp_clk_wr_delay = 0;
		phy->settings.cp_clk_wrdqs_delay = 0;
		phy->settings.cp_data_select_oe_end = 1;
		phy->settings.cp_dll_bypass_mode = 1;
		phy->settings.cp_dll_locked_mode = 3;
		phy->settings.cp_dll_start_point = 4;
		phy->settings.cp_gate_cfg_always_on = 1;
		phy->settings.cp_io_mask_always_on = 0;
		phy->settings.cp_io_mask_end = 7;
		phy->settings.cp_io_mask_start = 0;
		phy->settings.cp_rd_del_sel = 52;
		phy->settings.cp_read_dqs_cmd_delay = 0;
		phy->settings.cp_read_dqs_delay = 0;
		phy->settings.cp_sw_half_cycle_shift = 0;
		phy->settings.cp_sync_method = 1;
		phy->settings.cp_underrun_suppress = 1;
		phy->settings.cp_use_ext_lpbk_dqs = 1;
		phy->settings.cp_use_lpbk_dqs = 1;
		phy->settings.cp_use_phony_dqs = 1;
		phy->settings.cp_use_phony_dqs_cmd = 1;
		phy->settings.sdhc_extended_rd_mode = 1;
		phy->settings.sdhc_extended_wr_mode = 1;
		phy->settings.sdhc_hcsdclkadj = 3;
		phy->settings.sdhc_idelay_val = 1;
		phy->settings.sdhc_rdcmd_en = 1;
		phy->settings.sdhc_rddata_en = 1;
		phy->settings.sdhc_rw_compensate = 14;
		phy->settings.sdhc_sdcfsh = 0;
		phy->settings.sdhc_sdcfsl = 4;
		phy->settings.sdhc_wrcmd0_dly = 1;
		phy->settings.sdhc_wrcmd0_sdclk_dly = 0;
		phy->settings.sdhc_wrcmd1_dly = 0;
		phy->settings.sdhc_wrcmd1_sdclk_dly = 0;
		phy->settings.sdhc_wrdata0_dly = 1;
		phy->settings.sdhc_wrdata0_sdclk_dly = 0;
		phy->settings.sdhc_wrdata1_dly = 0;
		phy->settings.sdhc_wrdata1_sdclk_dly = 0;
#else
		DEBUG_DRV("SDR_10MHZ SDHCI_CDNS_HRS06_MODE_MMC_SDR PHY configuration\n");
		phy->settings.cp_clk_wr_delay = 0;
		phy->settings.cp_clk_wrdqs_delay = 0;
		phy->settings.cp_data_select_oe_end = 1;
		phy->settings.cp_dll_bypass_mode = 1;
		phy->settings.cp_dll_locked_mode = 3;
		phy->settings.cp_dll_start_point = 4;
		phy->settings.cp_gate_cfg_always_on = 1;
		phy->settings.cp_io_mask_always_on = 0;
		phy->settings.cp_io_mask_end = 0;
		phy->settings.cp_io_mask_start = 0;
		phy->settings.cp_rd_del_sel = 52;
		phy->settings.cp_read_dqs_cmd_delay = 0;
		phy->settings.cp_read_dqs_delay = 0;
		phy->settings.cp_sw_half_cycle_shift = 0;
		phy->settings.cp_sync_method = 1;
		phy->settings.cp_underrun_suppress = 1;
		phy->settings.cp_use_ext_lpbk_dqs = 1;
		phy->settings.cp_use_lpbk_dqs = 1;
		phy->settings.cp_use_phony_dqs = 1;
		phy->settings.cp_use_phony_dqs_cmd = 1;
		phy->settings.sdhc_extended_rd_mode = 1;
		phy->settings.sdhc_extended_wr_mode = 1;
		phy->settings.sdhc_hcsdclkadj = 1;
		phy->settings.sdhc_idelay_val = 0;
		phy->settings.sdhc_rdcmd_en = 1;
		phy->settings.sdhc_rddata_en = 1;
		phy->settings.sdhc_rw_compensate = 9;
		phy->settings.sdhc_sdcfsh = 0;
		phy->settings.sdhc_sdcfsl = 10;
		phy->settings.sdhc_wrcmd0_dly = 1;
		phy->settings.sdhc_wrcmd0_sdclk_dly = 0;
		phy->settings.sdhc_wrcmd1_dly = 0;
		phy->settings.sdhc_wrcmd1_sdclk_dly = 0;
		phy->settings.sdhc_wrdata0_dly = 1;
		phy->settings.sdhc_wrdata0_sdclk_dly = 0;
		phy->settings.sdhc_wrdata1_dly = 0;
		phy->settings.sdhc_wrdata1_sdclk_dly = 0;
#endif
	}
	else {
		DEBUG_DRV("Error: PHY configuration is not updated, mode %d\n", mode);
	}
}

static u32 sdhci_cdns_sd6_readl(struct sdhci_host *host, int reg)
{
	return readl(host->ioaddr + reg);
}

static void sdhci_cdns_sd6_writel(struct sdhci_host *host, u32 val, int reg)
{
	writel(val, host->ioaddr + reg);
}

static u16 sdhci_cdns_sd6_readw(struct sdhci_host *host, int reg)
{
	u32 val, regoff;

	regoff = reg & ~3;

	val = readl(host->ioaddr + regoff);
	if ((reg & 0x3) == 0)
		return (val & 0xFFFF);
	else
		return ((val >> 16) & 0xFFFF);
}

static void sdhci_cdns_sd6_writew(struct sdhci_host *host, u16 val, int reg)
{
	writew(val, host->ioaddr + reg);
}

static u8 sdhci_cdns_sd6_readb(struct sdhci_host *host, int reg)
{
	u32 val, regoff;

	regoff = reg & ~3;

	val = readl(host->ioaddr + regoff);
	switch (reg & 3) {
	case 0:
		return (val & 0xFF);
	case 1:
		return ((val >> 8) & 0xFF);
	case 2:
		return ((val >> 16) & 0xFF);
	case 3:
		return ((val >> 24) & 0xFF);
	}
	return 0;
}

static void sdhci_cdns_sd6_writeb(struct sdhci_host *host, u8 val, int reg)
{
	writeb(val, host->ioaddr + reg);
}

void sdhci_cdns_sd6_fullsw_reset(struct sdhci_cdns_plat *plat)
{
	u32 regval;

	regval = readl(plat->hrs_addr + SDHCI_CDNS_HRS00);
	regval |= SDHCI_CDNS_HRS00_SWR;
	writel(regval, plat->hrs_addr + SDHCI_CDNS_HRS00);

	do {
		regval = readl(plat->hrs_addr + SDHCI_CDNS_HRS00);
	} while (regval & SDHCI_CDNS_HRS00_SWR);

	DEBUG_DRV("Success in reset of eMMC controller 0x%x\n", regval);
}

void sdhci_cdns_sd6_stop_clock(struct sdhci_cdns_plat *plat)
{
	u32 reg_srs11 = 0;

	DEBUG_DRV("%s\n", __func__);
	reg_srs11 = readl(plat->hrs_addr + SDHCI_CDNS_SRS_BASE + SDHCI_CDNS_SRS11);
	reg_srs11 &= ~5;
	writel(reg_srs11, plat->hrs_addr + SDHCI_CDNS_SRS_BASE + SDHCI_CDNS_SRS11);
}

void sdhci_cdns_sd6_set_volt(struct sdhci_cdns_plat *plat)
{
	u32 controller_setting = 0;

	controller_setting = readl(plat->hrs_addr + SDHCI_CDNS_SRS_BASE + SDHCI_CDNS_HRS10);
	controller_setting &= ~(7 << 9);

	controller_setting |= (7 << 9);
	controller_setting |= (1 << 8);

	writel(controller_setting, plat->hrs_addr + SDHCI_CDNS_SRS_BASE + SDHCI_CDNS_HRS10);
	controller_setting = readl(plat->hrs_addr + SDHCI_CDNS_SRS_BASE + SDHCI_CDNS_HRS10);
	DEBUG_DRV("controller_setting 0x%x\n", controller_setting);
}

static int sdhci_cdns_sd6_get_fdt_params(struct udevice *dev, struct sdhci_cdns_plat *plat)
{
	struct sdhci_cdns_sd6_phy *phy = plat->priv;
	const char *mode_name;

#ifdef CN10K_SILICON
	dev_read_u32(dev, "cdns,iocell_input_delay", &phy->d.iocell_input_delay);

	dev_read_u32(dev, "cdns,iocell_output_delay", &phy->d.iocell_output_delay);

	dev_read_u32(dev, "cdns,delay_element", &phy->d.delay_element);
#else
	phy->d.iocell_input_delay = 0;
	phy->d.iocell_output_delay = 0;
	phy->d.delay_element = 0;
#endif

	mode_name = dev_read_string(dev, "cdns, mode");

	if (mode_name != NULL) {
		if (!strcmp("emmc_sdr", mode_name))
			phy->mode = MMC_HS_52;
		else if (!strcmp("emmc_ddr", mode_name))
			phy->mode = MMC_DDR_52;
		else if (!strcmp("emmc_hs200", mode_name))
			phy->mode = MMC_HS_200;
		else if (!strcmp("emmc_hs400", mode_name))
			phy->mode = MMC_HS_400;
		else if (!strcmp("sd_hs", mode_name))
			phy->mode = SD_HS;
		else
			phy->mode = MMC_HS;
	} else
		phy->mode = MMC_HS;

#ifdef SDR_10MHZ
	phy->mode = SDHCI_CDNS_HRS06_MODE_MMC_SDR;
#endif

	return 0;
}

static u32 sdhci_cdns_sd6_read_phy_reg(struct sdhci_cdns_plat *plat,
					u32 addr)
{
	writel(addr, plat->hrs_addr + SDHCI_CDNS_HRS04);
	return readl(plat->hrs_addr + SDHCI_CDNS_HRS05);
}

static void sdhci_cdns_sd6_write_phy_reg(struct sdhci_cdns_plat *plat,
					 u32 addr, u32 data)
{
	u32 data_read;

	writel(addr, plat->hrs_addr + SDHCI_CDNS_HRS04);
	writel(data, plat->hrs_addr + SDHCI_CDNS_HRS05);

	//TODO remove it
	writel(addr, plat->hrs_addr + SDHCI_CDNS_HRS04);
	data_read = readl(plat->hrs_addr + SDHCI_CDNS_HRS05);

#ifdef PHY_DEBUG
	if (data != data_read)
		printf("Error written and read PHY data are different\n");
#endif
}

static int sdhci_cdns_sd6_dll_reset(struct sdhci_cdns_plat *plat, bool doreset)
{
	u32 reg;
	int ret = 0;

	reg = readl(plat->hrs_addr + SDHCI_CDNS_HRS09);
	if (doreset)
		reg &= ~SDHCI_CDNS_HRS09_PHY_SW_RESET;
	else
		reg |= SDHCI_CDNS_HRS09_PHY_SW_RESET;
	writel(reg, plat->hrs_addr + SDHCI_CDNS_HRS09);

	if (!doreset) {
		do {
			reg = readl(plat->hrs_addr + SDHCI_CDNS_HRS09);
		} while ((reg & SDHCI_CDNS_HRS09_PHY_INIT_COMPLETE) == 0);
	}

	return ret;
}

static int sdhci_cdns_sd6_phy_init(struct udevice *dev, struct sdhci_cdns_plat *plat)
{
	int ret;
	u32 reg;
	struct sdhci_cdns_sd6_phy *phy = plat->priv;

#ifdef PHY_DEBUG
	if ((phy->mode == -1) || (phy->t_sdclk == -1))
		return 0;
#endif

	DEBUG_DRV("%s mode %d sdck %u\n", __func__, phy->mode, phy->t_sdclk);

	sdhci_cdns_sd6_calc_phy(phy);
#ifdef PHY_DEBUG
	sdhci_cdns_sd6_phy_dump(phy);
#endif

	sdhci_cdns_sd6_dll_reset(plat, true);

	/* cp_use_phony_dqs SDHCI_CDNS_SD6_PHY_DQS_TIMING */
	reg = sdhci_cdns_sd6_read_phy_reg(plat, SDHCI_CDNS_SD6_PHY_DQS_TIMING);
	reg &= ~SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_EXT_LPBK_DQS;
	reg &= ~SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_LPBK_DQS;
	reg &= ~SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_PHONY_DQS;
	reg &= ~SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_PHONY_DQS_CMD;
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_EXT_LPBK_DQS,
			phy->settings.cp_use_ext_lpbk_dqs);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_LPBK_DQS,
			phy->settings.cp_use_lpbk_dqs);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_PHONY_DQS,
			phy->settings.cp_use_phony_dqs);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DQS_TIMING_USE_PHONY_DQS_CMD,
			phy->settings.cp_use_phony_dqs_cmd);

	DEBUG_DRV("SDHCI_CDNS_SD6_PHY_DQS_TIMING 0x%x\n", reg);
	sdhci_cdns_sd6_write_phy_reg(plat, SDHCI_CDNS_SD6_PHY_DQS_TIMING, reg);

	/* SDHCI_CDNS_SD6_PHY_GATE_LPBK */
	reg = sdhci_cdns_sd6_read_phy_reg(plat, SDHCI_CDNS_SD6_PHY_GATE_LPBK);
	reg &= ~SDHCI_CDNS_SD6_PHY_GATE_LPBK_SYNC_METHOD;
	reg &= ~SDHCI_CDNS_SD6_PHY_GATE_LPBK_SW_HALF_CYCLE_SHIFT;
	reg &= ~SDHCI_CDNS_SD6_PHY_GATE_LPBK_RD_DEL_SEL;
	reg &= ~SDHCI_CDNS_SD6_PHY_GATE_LPBK_GATE_CFG_ALWAYS_ON;
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_GATE_LPBK_SYNC_METHOD,
			phy->settings.cp_sync_method);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_GATE_LPBK_SW_HALF_CYCLE_SHIFT,
			phy->settings.cp_sw_half_cycle_shift);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_GATE_LPBK_RD_DEL_SEL,
			phy->settings.cp_rd_del_sel);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_GATE_LPBK_GATE_CFG_ALWAYS_ON,
			phy->settings.cp_gate_cfg_always_on);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_GATE_LPBK_UNDERRUN_SUPPRESS,
			phy->settings.cp_underrun_suppress);

	DEBUG_DRV("SDHCI_CDNS_SD6_PHY_GATE_LPBK 0x%x\n", reg);
	sdhci_cdns_sd6_write_phy_reg(plat, SDHCI_CDNS_SD6_PHY_GATE_LPBK, reg);

	/* SDHCI_CDNS_SD6_PHY_DLL_MASTER */
	reg = 0x0;
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DLL_MASTER_BYPASS_MODE,
			 phy->settings.cp_dll_bypass_mode);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DLL_MASTER_PHASE_DETECT_SEL, 2);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DLL_MASTER_DLL_START_POINT,
			phy->settings.cp_dll_start_point);

	DEBUG_DRV("SDHCI_CDNS_SD6_PHY_DLL_MASTER 0x%x\n", reg);
	sdhci_cdns_sd6_write_phy_reg(plat, SDHCI_CDNS_SD6_PHY_DLL_MASTER, reg);

	/* SDHCI_CDNS_SD6_PHY_DLL_SLAVE */
	reg = 0x0;
	reg = FIELD_PREP(SDHCI_CDNS_SD6_PHY_DLL_SLAVE_READ_DQS_CMD_DELAY,
			 phy->settings.cp_read_dqs_cmd_delay);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DLL_SLAVE_CLK_WRDQS_DELAY,
			phy->settings.cp_clk_wrdqs_delay);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DLL_SLAVE_CLK_WR_DELAY,
			phy->settings.cp_clk_wr_delay);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DLL_SLAVE_READ_DQS_DELAY,
			phy->settings.cp_read_dqs_delay);

	reg = 0;
	DEBUG_DRV("SDHCI_CDNS_SD6_PHY_DLL_SLAVE 0x%x\n", reg);
	sdhci_cdns_sd6_write_phy_reg(plat, SDHCI_CDNS_SD6_PHY_DLL_SLAVE, reg);

	/* SDHCI_CDNS_SD6_PHY_CTRL */
	reg = sdhci_cdns_sd6_read_phy_reg(plat, SDHCI_CDNS_SD6_PHY_CTRL);
	reg &= ~SDHCI_CDNS_SD6_PHY_CTRL_PHONY_DQS_TIMING;
	DEBUG_DRV("SDHCI_CDNS_SD6_PHY_CTRL 0x%x \n", reg);
	sdhci_cdns_sd6_write_phy_reg(plat, SDHCI_CDNS_SD6_PHY_CTRL, reg);

	ret = sdhci_cdns_sd6_dll_reset(plat, false);
	if (ret)
		return ret;

	reg = sdhci_cdns_sd6_read_phy_reg(plat, SDHCI_CDNS_SD6_PHY_DQ_TIMING);
	reg &= ~SDHCI_CDNS_SD6_PHY_DQ_TIMING_IO_MASK_ALWAYS_ON;
	reg &= ~SDHCI_CDNS_SD6_PHY_DQ_TIMING_IO_MASK_END;
	reg &= ~SDHCI_CDNS_SD6_PHY_DQ_TIMING_IO_MASK_START;
	reg &= ~SDHCI_CDNS_SD6_PHY_DQ_TIMING_DATA_SELECT_OE_END;
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DQ_TIMING_IO_MASK_ALWAYS_ON,
			phy->settings.cp_io_mask_always_on);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DQ_TIMING_IO_MASK_END,
			phy->settings.cp_io_mask_end);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DQ_TIMING_IO_MASK_START,
			phy->settings.cp_io_mask_start);
	reg |= FIELD_PREP(SDHCI_CDNS_SD6_PHY_DQ_TIMING_DATA_SELECT_OE_END,
			phy->settings.cp_data_select_oe_end);

	DEBUG_DRV("SDHCI_CDNS_SD6_PHY_DQ_TIMING 0x%x\n", reg);
	sdhci_cdns_sd6_write_phy_reg(plat, SDHCI_CDNS_SD6_PHY_DQ_TIMING, reg);

	reg = readl(plat->hrs_addr + SDHCI_CDNS_HRS09);
	if (phy->settings.sdhc_extended_wr_mode)
		reg |= SDHCI_CDNS_HRS09_EXTENDED_WR_MODE;
	else
		reg &= ~SDHCI_CDNS_HRS09_EXTENDED_WR_MODE;

	if (phy->settings.sdhc_extended_rd_mode)
		reg |= SDHCI_CDNS_HRS09_EXTENDED_RD_MODE;
	else
		reg &= ~SDHCI_CDNS_HRS09_EXTENDED_RD_MODE;

	if (phy->settings.sdhc_rddata_en)
		reg |= SDHCI_CDNS_HRS09_RDDATA_EN;
	else
		reg &= ~SDHCI_CDNS_HRS09_RDDATA_EN;

	if (phy->settings.sdhc_rdcmd_en)
		reg |= SDHCI_CDNS_HRS09_RDCMD_EN;
	else
		reg &= ~SDHCI_CDNS_HRS09_RDCMD_EN;

	DEBUG_DRV("HRS09 0x%x\n", reg);
	writel(reg, plat->hrs_addr + SDHCI_CDNS_HRS09);
	//writel(0x1800c, plat->hrs_addr + SDHCI_CDNS_HRS09);

	DEBUG_DRV("HRS02 0x30004\n");
	writel(0x30004, plat->hrs_addr + SDHCI_CDNS_HRS02);

	reg = 0x0;
	reg = FIELD_PREP(SDHCI_CDNS_HRS10_HCSDCLKADJ, phy->settings.sdhc_hcsdclkadj);
	DEBUG_DRV("HRS10 0x%x\n", reg);
	writel(reg, plat->hrs_addr + SDHCI_CDNS_HRS10);

	reg = 0x0;
	reg = FIELD_PREP(SDHCI_CDNS_HRS16_WRDATA1_SDCLK_DLY,
			 phy->settings.sdhc_wrdata1_sdclk_dly);
	reg |= FIELD_PREP(SDHCI_CDNS_HRS16_WRDATA0_SDCLK_DLY,
			 phy->settings.sdhc_wrdata0_sdclk_dly);
	reg |= FIELD_PREP(SDHCI_CDNS_HRS16_WRCMD1_SDCLK_DLY,
			 phy->settings.sdhc_wrcmd1_sdclk_dly);
	reg |= FIELD_PREP(SDHCI_CDNS_HRS16_WRCMD0_SDCLK_DLY,
			 phy->settings.sdhc_wrcmd0_sdclk_dly);
	reg |= FIELD_PREP(SDHCI_CDNS_HRS16_WRDATA1_DLY,
			 phy->settings.sdhc_wrdata1_dly);
	reg |= FIELD_PREP(SDHCI_CDNS_HRS16_WRDATA0_DLY,
			 phy->settings.sdhc_wrdata0_dly);
	reg |= FIELD_PREP(SDHCI_CDNS_HRS16_WRCMD1_DLY,
			 phy->settings.sdhc_wrcmd1_dly);
	reg |= FIELD_PREP(SDHCI_CDNS_HRS16_WRCMD0_DLY,
			 phy->settings.sdhc_wrcmd0_dly);

	DEBUG_DRV("HRS16 0x%x\n", reg);
	writel(reg, plat->hrs_addr + SDHCI_CDNS_HRS16);

	reg = 0x0;
	reg = FIELD_PREP(SDHCI_CDNS_HRS07_RW_COMPENSATE,
			 phy->settings.sdhc_rw_compensate);
	reg |= FIELD_PREP(SDHCI_CDNS_HRS07_IDELAY_VAL,
			 phy->settings.sdhc_idelay_val);
	DEBUG_DRV("HRS07 0x%x\n", reg);
	writel(reg, plat->hrs_addr + SDHCI_CDNS_HRS07);
	return 0;
}

static void sdhci_cdns_set_control_reg(struct sdhci_host *host)
{
	struct mmc *mmc = host->mmc;
	struct sdhci_cdns_plat *plat = dev_get_platdata(mmc->dev);
	unsigned int clock = mmc->clock;
	u32 mode, tmp;

#ifdef USE_MMC_TIMING
	/*
	 * REVISIT:
	 * The mode should be decided by MMC_TIMING_* like Linux, but
	 * U-Boot does not support timing.  Use the clock frequency instead.
	 */
	if (clock <= 26000000) {
		mode = SDHCI_CDNS_HRS06_MODE_SD; /* use this for Legacy */
	} else if (clock <= 52000000) {
		if (mmc->ddr_mode)
			mode = SDHCI_CDNS_HRS06_MODE_MMC_DDR;
		else
			mode = SDHCI_CDNS_HRS06_MODE_MMC_SDR;
	} else {
		if (mmc->ddr_mode)
			mode = SDHCI_CDNS_HRS06_MODE_MMC_HS400;
		else
			mode = SDHCI_CDNS_HRS06_MODE_MMC_HS200;
	}

#endif
#ifdef SDR_10MHZ
	mode = SDHCI_CDNS_HRS06_MODE_MMC_SDR;
#endif
	DEBUG_DRV("%s: mode %d clock %d \n", __func__, mode, clock);
	tmp = readl(plat->hrs_addr + SDHCI_CDNS_HRS06);
	tmp &= ~SDHCI_CDNS_HRS06_MODE;
	tmp |= FIELD_PREP(SDHCI_CDNS_HRS06_MODE, mode);
	writel(tmp, plat->hrs_addr + SDHCI_CDNS_HRS06);
}

static int sdhci_cdns_set_tune_val(struct sdhci_cdns_plat *plat,
				unsigned int val)
{
	void __iomem *reg = plat->hrs_addr + SDHCI_CDNS_HRS06;
	u32 tmp;
	int i, ret;

	if (WARN_ON(!FIELD_FIT(SDHCI_CDNS_HRS06_TUNE, val)))
		return -EINVAL;

	tmp = readl(reg);
	tmp &= ~SDHCI_CDNS_HRS06_TUNE;
	tmp |= FIELD_PREP(SDHCI_CDNS_HRS06_TUNE, val);

	/*
	 * Workaround for IP errata:
	 * The IP6116 SD/eMMC PHY design has a timing issue on receive data
	 * path. Send tune request twice.
	 */
	for (i = 0; i < 2; i++) {
		tmp |= SDHCI_CDNS_HRS06_TUNE_UP;
		writel(tmp, reg);

		ret = readl_poll_timeout(reg, tmp,
					 !(tmp & SDHCI_CDNS_HRS06_TUNE_UP), 1);
		if (ret)
			return ret;
	}

	return 0;
}

static int sdhci_cdns_sd6_phy_lock_dll(struct sdhci_cdns_sd6_phy *phy)
{
	u32 delay_element = phy->d.delay_element_org;
	u32 delay_elements_in_sdmclk;
	enum sdhci_cdns_sd6_phy_lock_mode mode;

	delay_elements_in_sdmclk = DIV_ROUND_UP(phy->t_sdmclk, delay_element);
	if (delay_elements_in_sdmclk > 256) {
		delay_element *= 2;
		delay_elements_in_sdmclk = DIV_ROUND_UP(phy->t_sdmclk,
							delay_element);

		if (delay_elements_in_sdmclk > 256) {
			return -1;
		} else {
			mode = SDHCI_CDNS_SD6_PHY_LOCK_MODE_HALF_CLK;
			phy->vars.dll_max_value = 127;
		}
	} else {
		mode = SDHCI_CDNS_SD6_PHY_LOCK_MODE_FULL_CLK;
		phy->vars.dll_max_value = 255;
	}

	phy->vars.t_sdmclk_calc = delay_element * delay_elements_in_sdmclk;
	phy->d.delay_element = delay_element;
	phy->settings.cp_dll_locked_mode = mode;
	phy->settings.cp_dll_bypass_mode = 0;

	return 0;
}

static void sdhci_cdns_sd6_phy_dll_bypass(struct sdhci_cdns_sd6_phy *phy)
{
	phy->vars.dll_max_value = 256;
	phy->settings.cp_dll_bypass_mode = 1;
	phy->settings.cp_dll_locked_mode =
		SDHCI_CDNS_SD6_PHY_LOCK_MODE_SATURATION;
}

static void sdhci_cdns_sd6_phy_configure_dll(struct sdhci_cdns_sd6_phy *phy)
{
	if (phy->settings.sdhc_extended_wr_mode == 0) {
		if (sdhci_cdns_sd6_phy_lock_dll(phy) == 0)
			return;
	}
	sdhci_cdns_sd6_phy_dll_bypass(phy);
}

static void sdhci_cdns_sd6_phy_calc_out(struct sdhci_cdns_sd6_phy *phy,
					bool cmd_not_dat)
{
	u32 wr0_dly = 0, wr1_dly = 0, output_min, output_max, phy_o_delay;
	u32 clk_wr_delay = 0, wr0_sdclk_dly = 0, wr1_sdclk_dly = 0;
	bool data_ddr = phy->ddr && !cmd_not_dat;
	int t;

	if (cmd_not_dat) {
		output_min = phy->t.t_cmd_output_min;
		output_max = phy->t.t_cmd_output_max;
		phy_o_delay = phy->d.phy_cmd_o_delay;
	} else {
		output_min = phy->t.t_dat_output_min;
		output_max = phy->t.t_dat_output_max;
		phy_o_delay = phy->d.phy_dat_o_delay;
	}

	clk_wr_delay = 0;
	if (data_ddr)
		wr0_sdclk_dly = wr1_sdclk_dly = 1;

	t = phy_o_delay - phy->d.phy_sdclk_delay - output_min;
	if ((t < 0) && (phy->settings.sdhc_extended_wr_mode == 1)) {
		u32 n_half_cycle = DIV_ROUND_UP(-t * 2, phy->t_sdmclk);

		wr0_dly = (n_half_cycle + 1) / 2;
		if (data_ddr)
			wr1_dly = (n_half_cycle + 1) / 2;
		else
			wr1_dly = (n_half_cycle + 1) % 2 + wr0_dly - 1;
	}

	if (phy->settings.sdhc_extended_wr_mode == 0) {
		u32 out_hold, out_setup, out_hold_margin;
		u32 n;

		if (!data_ddr)
			wr0_dly = 1;

		out_setup = output_max;
		out_hold = output_min;
		out_hold_margin = DIV_ROUND_UP(out_setup - out_hold, 4);
		out_hold += out_hold_margin;

		if (phy->settings.cp_dll_bypass_mode == 0) {
			n = DIV_ROUND_UP(256 * out_hold, phy->vars.t_sdmclk_calc);
			// TODO remove the following assert
			BUG_ON((n * phy->vars.t_sdmclk_calc / 256) >= out_hold);
		} else {
			n = DIV_ROUND_UP(out_hold, phy->d.delay_element) - 1;
			// TODO remove the following assert
			BUG_ON(((n + 1) * phy->d.delay_element) > out_hold);
		}

		if (n <= phy->vars.dll_max_value) {
			clk_wr_delay = n;
		} else {
			DEBUG_DRV("Warning Expected DLL value setting exceeds DLL capabilities. Applied maximum DLL setting.\n");
			clk_wr_delay = 255;
			/* no dll setting*/
		}
	} else {
		/*  sdhc_extended_wr_mode = 1 - PHY IO cell work in SDR mode */
		clk_wr_delay = 0;
	}

	if (cmd_not_dat) {
		phy->settings.sdhc_wrcmd0_dly = wr0_dly;
		phy->settings.sdhc_wrcmd1_dly = wr1_dly;
		phy->settings.cp_clk_wrdqs_delay = clk_wr_delay;
		phy->settings.sdhc_wrcmd0_sdclk_dly = wr0_sdclk_dly;
		phy->settings.sdhc_wrcmd1_sdclk_dly = wr1_sdclk_dly;
	} else {
		phy->settings.sdhc_wrdata0_dly = wr0_dly;
		phy->settings.sdhc_wrdata1_dly = wr1_dly;
		phy->settings.cp_clk_wr_delay = clk_wr_delay;
		phy->settings.sdhc_wrdata0_sdclk_dly = wr0_sdclk_dly;
		phy->settings.sdhc_wrdata1_sdclk_dly = wr1_sdclk_dly;
	}
}

static void sdhci_cdns_sd6_phy_calc_cmd_out(struct sdhci_cdns_sd6_phy *phy)
{
	sdhci_cdns_sd6_phy_calc_out(phy, true);
}

static void sdhci_cdns_sd6_phy_calc_cmd_in(struct sdhci_cdns_sd6_phy *phy)
{
	//phy->settings.cp_gate_cfg_always_on = 1;
//	phy->settings.cp_io_mask_always_on = 0;
	phy->settings.cp_io_mask_end =
		((phy->d.iocell_output_delay + phy->d.iocell_input_delay) * 2)
		/ phy->t_sdmclk;

	if (phy->settings.cp_io_mask_end >= 8)
		phy->settings.cp_io_mask_end = 7;

	if (phy->strobe_cmd && (phy->settings.cp_io_mask_end > 0))
		phy->settings.cp_io_mask_end--;

	//XXX
	//phy->settings.cp_sync_method = 1;
	//phy->settings.cp_rd_del_sel = 52;
	//phy->settings.cp_use_ext_lpbk_dqs = 1;
	//phy->settings.cp_use_lpbk_dqs = 1;

	if (phy->strobe_cmd) {
		phy->settings.cp_use_phony_dqs_cmd = 0;
		phy->settings.cp_read_dqs_cmd_delay = 64;
	} else {
		phy->settings.cp_use_phony_dqs_cmd = 1;
		phy->settings.cp_read_dqs_cmd_delay = 0;
	}

	if ((phy->mode == MMC_HS_400 && !phy->strobe_cmd)
	     || phy->mode == MMC_HS_200)
		phy->settings.cp_read_dqs_cmd_delay =
			phy->settings.hs200_tune_val;
}

static void sdhci_cdns_sd6_phy_calc_dat_in(struct sdhci_cdns_sd6_phy *phy)
{
	u32 hcsdclkadj = 0;

	if (phy->strobe_dat) {
		phy->settings.cp_use_phony_dqs = 0;
		phy->settings.cp_read_dqs_delay = 64;
	} else {
		phy->settings.cp_use_phony_dqs = 1;
		phy->settings.cp_read_dqs_delay = 0;
	}

	if (phy->mode == MMC_HS_200)
		phy->settings.cp_read_dqs_delay =
			phy->settings.hs200_tune_val;

	if (phy->strobe_dat) {
		/* dqs loopback input via IO cell */
		hcsdclkadj += phy->d.iocell_input_delay;
		/* dfi_dqs_in: mem_dqs -> clean_dqs_mod; delay of hic_dll_dqs_nand2 */
		hcsdclkadj += phy->d.delay_element / 2;
		/* delay line */
		hcsdclkadj += phy->t_sdclk / 2;
		/* PHY FIFO write pointer */
		hcsdclkadj += phy->t_sdclk / 2 + phy->d.delay_element;
		/* 1st synchronizer */
		hcsdclkadj += DIV_ROUND_UP(hcsdclkadj, phy->t_sdmclk)
			* phy->t_sdmclk - hcsdclkadj;
		/*
		 * 2nd synchronizer + PHY FIFO read pointer + PHY rddata
		 * + PHY rddata registered, + FIFO 1st ciu_en
		 */
		hcsdclkadj += 5 * phy->t_sdmclk;
		/* FIFO 2st ciu_en */
		hcsdclkadj += phy->t_sdclk;

		hcsdclkadj /= phy->t_sdclk;
	} else {
		u32 n;

		/* rebar PHY delay */
		hcsdclkadj += 2 * phy->t_sdmclk;
		/* rebar output via IO cell */
		hcsdclkadj += phy->d.iocell_output_delay;
		/* dqs loopback input via IO cell */
		hcsdclkadj += phy->d.iocell_input_delay;
		/* dfi_dqs_in: mem_dqs -> clean_dqs_mod delay of hic_dll_dqs_nand2 */
		hcsdclkadj += phy->d.delay_element / 2;
		/* dll: one delay element between SIGI_0 and SIGO_0 */
		hcsdclkadj += phy->d.delay_element;
		/* dfi_dqs_in: mem_dqs_delayed -> clk_dqs delay of hic_dll_dqs_nand2 */
		hcsdclkadj += phy->d.delay_element / 2;
		/* deskew DLL: clk_dqs -> clk_dqN: one delay element */
		hcsdclkadj += phy->d.delay_element;

		if (phy->t_sdclk == phy->t_sdmclk)
			n = (hcsdclkadj - 2 * phy->t_sdmclk) / phy->t_sdclk;
		else
			n = hcsdclkadj / phy->t_sdclk;

		/* phase shift within one t_sdclk clock cycle caused by rebar - lbk dqs delay */
		hcsdclkadj = hcsdclkadj % phy->t_sdclk;
		/* PHY FIFO write pointer */
		hcsdclkadj += phy->t_sdclk / 2;
		/* 1st synchronizer */
		hcsdclkadj += DIV_ROUND_UP(hcsdclkadj, phy->t_sdmclk)
			* phy->t_sdmclk - hcsdclkadj;
		/*
		 * 2nd synchronizer + PHY FIFO read pointer + PHY rddata
		 * + PHY rddata registered
		 */
		hcsdclkadj += 4 * phy->t_sdmclk;

		if ((phy->t_sdclk / phy->t_sdmclk) > 1) {
			u32 tmp1, tmp2;

			tmp1 = hcsdclkadj;
			tmp2 = (hcsdclkadj / phy->t_sdclk) * phy->t_sdclk
				+ phy->t_sdclk - phy->t_sdmclk;
			if (tmp1 == tmp2)
				tmp2 += phy->t_sdclk;

			/* FIFO aligns to clock cycle before ciu_en */
			hcsdclkadj += tmp2 - tmp1;
		}

		/* FIFO 1st ciu_en */
		hcsdclkadj += phy->t_sdmclk;
		/* FIFO 2nd ciu_en */
		hcsdclkadj += phy->t_sdclk;

		hcsdclkadj /= phy->t_sdclk;

		hcsdclkadj += n;

		if ((phy->t_sdclk / phy->t_sdmclk) >= 2) {
			if ((phy->mode == UHS_DDR50)
				|| (phy->mode == MMC_DDR_52))
				hcsdclkadj -= 2;
			else
				hcsdclkadj -= 1;
		} else if ((phy->t_sdclk / phy->t_sdmclk) == 1) {
			hcsdclkadj += 2;
		}

		if (phy->tune_dat)
			hcsdclkadj -= 1;
	}

	if (hcsdclkadj > 15)
		hcsdclkadj = 15;

	phy->settings.sdhc_hcsdclkadj = hcsdclkadj;
}

static void sdhci_cdns_sd6_phy_calc_dat_out(struct sdhci_cdns_sd6_phy *phy)
{
	sdhci_cdns_sd6_phy_calc_out(phy, false);
}

static void sdhci_cdns_sd6_phy_calc_io(struct sdhci_cdns_sd6_phy *phy)
{
	u32 rw_compensate;

	rw_compensate = (phy->d.iocell_input_delay + phy->d.iocell_output_delay)
		/ phy->t_sdmclk + phy->settings.sdhc_wrdata0_dly + 5 + 3;

	phy->settings.sdhc_idelay_val = (2 * phy->d.iocell_input_delay)
		/ phy->t_sdmclk;

	phy->settings.cp_io_mask_start = 0;
	if ((phy->t_sdclk == phy->t_sdmclk) && (rw_compensate > 10))
		phy->settings.cp_io_mask_start = 2 * (rw_compensate - 10);

	if (phy->mode == UHS_SDR104)
		phy->settings.cp_io_mask_start++;

	if ((phy->t_sdclk == phy->t_sdmclk) && (phy->mode == UHS_SDR50))
		phy->settings.cp_io_mask_start++;

	phy->settings.sdhc_rw_compensate = rw_compensate;
}

static void sdhci_cdns_sd6_phy_calc_settings(struct sdhci_cdns_sd6_phy *phy)
{
	// XXX
	phy->settings.cp_data_select_oe_end = 1;
	phy->settings.cp_dll_start_point = 4;

	sdhci_cdns_sd6_phy_calc_cmd_out(phy);
	sdhci_cdns_sd6_phy_calc_cmd_in(phy);
	sdhci_cdns_sd6_phy_calc_dat_out(phy);
	sdhci_cdns_sd6_phy_calc_dat_in(phy);
	sdhci_cdns_sd6_phy_calc_io(phy);
}

static int sdhci_cdns_sd6_phy_update_timings(struct sdhci_cdns_plat *plat)
{
	struct sdhci_cdns_sd6_phy *phy = plat->priv;
	int t_sdmclk = phy->t_sdmclk;

	DEBUG_DRV("%s\n", __func__);
	/* initialize input */
	init_timings[phy->mode](&phy->t, phy->t_sdclk);

	switch (phy->mode) {
	case UHS_SDR104:
		phy->tune_cmd = true;
		phy->tune_dat = true;
		break;
	case UHS_DDR50:
		phy->ddr = true;
		break;
	case MMC_DDR_52:
		phy->ddr = true;
		break;
	case MMC_HS_200:
		phy->tune_dat = true;
		phy->tune_cmd = true;
		break;
	case MMC_HS_400:
		phy->tune_cmd = true;
		phy->ddr = true;
		phy->strobe_dat = true;
		break;
	}

	//if(phy->enhanced_strobe)
	//	phy->strobe_cmd = true;

	phy->d.phy_sdclk_delay = 2 * t_sdmclk;
	phy->d.phy_cmd_o_delay = 2 * t_sdmclk + t_sdmclk / 2;
	phy->d.phy_dat_o_delay = 2 * t_sdmclk + t_sdmclk / 2;

	if (phy->t_sdclk == phy->t_sdmclk) {
		phy->settings.sdhc_extended_wr_mode = 0;
		phy->settings.sdhc_extended_rd_mode = 0;
	} else {
		phy->settings.sdhc_extended_wr_mode = 1;
		phy->settings.sdhc_extended_rd_mode = 1;
	}

	phy->settings.sdhc_rdcmd_en = 1;
	phy->settings.sdhc_rddata_en = 1;

	sdhci_cdns_sd6_phy_configure_dll(phy);

	sdhci_cdns_sd6_phy_calc_settings(phy);

	return 0;
}

static void sdhci_cdns_sd6_set_clock(struct sdhci_host *host,
				unsigned int clock)
{
	struct udevice *dev = host->mmc->dev;
	struct sdhci_cdns_plat *plat = dev_get_platdata(dev);
	struct sdhci_cdns_sd6_phy *phy = plat->priv;
	unsigned int regval;

	DEBUG_DRV("sdhci_cdns_sd6_set_clock %d\n", clock);
	phy->t_sdclk = 5000; //DIV_ROUND_DOWN_ULL(1e12, clock);

	if (sdhci_cdns_sd6_phy_update_timings(plat))
		printf("%s: update timings failed\n", __func__);
	else
		host->clock = clock;

        if (sdhci_cdns_sd6_phy_init(dev, plat))
                dev_info(mmc_dev(host->mmc), "%s: phy init failed\n", __func__);

	regval = sdhci_cdns_sd6_readw(host, SDHCI_CDNS_SRS11);
	regval &= ~(0x3FF << 6);
	regval |= (phy->settings.sdhc_sdcfsh << 6) | (phy->settings.sdhc_sdcfsl << 8);
	regval |= SDHCI_CLOCK_INT_EN;
	sdhci_cdns_sd6_writew(host, regval, SDHCI_CDNS_SRS11);
}

#ifdef PHY_DEBUG
void sdhci_cdns_sd6_dump(struct sdhci_cdns_plat *plat)
{
	int i;

	for (i = 0; i < 14; i++)
		printf("HRS%d 0x%x\n", i, readl(plat->hrs_addr + (i * 4)));

	for (i = 0; i < 27; i++)
		printf("SRS%d 0x%x\n", i, readl(plat->hrs_addr + 0x200 + (i * 4)));
}
#endif

static int sdhci_cdns_sd6_plat_init(struct udevice *dev, struct sdhci_cdns_plat *plat)
{
	struct sdhci_cdns_sd6_phy *phy = plat->priv;

	sdhci_cdns_sd6_fullsw_reset(plat);
	sdhci_cdns_sd6_set_volt(plat);

	sdhci_cdns_sd6_get_fdt_params(dev, plat);

#if defined(SDR_25MHZ)
	phy->t_sdclk = 5000;
	phy->t_sdmclk = 5000;
#else
	phy->t_sdmclk = 5000;
	phy->t_sdclk = 100000;
#endif
	phy->settings.cp_use_phony_dqs = 1;
	phy->settings.cp_use_phony_dqs_cmd = 1;
	phy->settings.cp_dll_bypass_mode = 1;

	phy->settings.cp_read_dqs_cmd_delay = 0;
	phy->settings.cp_clk_wrdqs_delay = 0;
	phy->settings.cp_clk_wr_delay = 0;
	phy->settings.cp_read_dqs_delay = 0;
	phy->settings.cp_io_mask_end = 5;
	phy->settings.cp_io_mask_start = 0;
	phy->settings.sdhc_extended_rd_mode = 1;
	phy->settings.sdhc_extended_wr_mode = 1;
	phy->settings.sdhc_hcsdclkadj = 6;
	phy->settings.sdhc_rw_compensate = 10;
	phy->settings.sdhc_idelay_val = 0;

	sdhci_cdns_sd6_phy_update_timings(plat);
	return 0;
}

static int __maybe_unused sdhci_cdns_execute_tuning(struct udevice *dev,
							unsigned int opcode)
{
	struct sdhci_cdns_plat *plat = dev_get_platdata(dev);
	struct mmc *mmc = &plat->mmc;
	int cur_streak = 0;
	int max_streak = 0;
	int end_of_streak = 0;
	int i;

	/*
	 * This handler only implements the eMMC tuning that is specific to
	 * this controller.  The tuning for SD timing should be handled by the
	 * SDHCI core.
	 */
	if (!IS_MMC(mmc))
		return -ENOTSUPP;

	if (WARN_ON(opcode != MMC_CMD_SEND_TUNING_BLOCK_HS200))
		return -EINVAL;

	for (i = 0; i < SDHCI_CDNS_MAX_TUNING_LOOP; i++) {
		if (sdhci_cdns_set_tune_val(plat, i) ||
			mmc_send_tuning(mmc, opcode, NULL)) { /* bad */
			cur_streak = 0;
		} else { /* good */
			cur_streak++;
			if (cur_streak > max_streak) {
				max_streak = cur_streak;
				end_of_streak = i;
			}
		}
	}

	if (!max_streak) {
		dev_err(dev, "no tuning point found\n");
		return -EIO;
	}

	return sdhci_cdns_set_tune_val(plat, end_of_streak - max_streak / 2);
}

static struct dm_mmc_ops sdhci_cdns_mmc_ops;

static const struct sdhci_ops sdhci_cdns_ops = {
	.set_control_reg = sdhci_cdns_set_control_reg,
	.write_l = sdhci_cdns_sd6_writel,
	.read_l = sdhci_cdns_sd6_readl,
	.write_w = sdhci_cdns_sd6_writew,
	.read_w = sdhci_cdns_sd6_readw,
	.write_b = sdhci_cdns_sd6_writeb,
	.read_b = sdhci_cdns_sd6_readb,
	.set_clock = sdhci_cdns_sd6_set_clock,
};

static int sdhci_cdns_bind(struct udevice *dev)
{
	struct sdhci_cdns_plat *plat = dev_get_platdata(dev);

	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

static int sdhci_cdns_probe(struct udevice *dev)
{
#ifdef SD4_ENABLE
	DECLARE_GLOBAL_DATA_PTR;
#endif
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct sdhci_cdns_plat *plat = dev_get_platdata(dev);
	struct sdhci_host *host = dev_get_priv(dev);
	fdt_addr_t base;
	int ret;
	struct sdhci_cdns_sd6_phy *phy;

	base = dev_read_addr(dev);
	if (base == FDT_ADDR_T_NONE)
		return -EINVAL;

	plat->hrs_addr = devm_ioremap(dev, base, SZ_1K);
	if (!plat->hrs_addr)
		return -ENOMEM;

	plat->priv = &sd6_phy_config;

	host->name = dev->name;
	host->ioaddr = plat->hrs_addr + SDHCI_CDNS_SRS_BASE;
	host->ops = &sdhci_cdns_ops;
	host->quirks |= SDHCI_QUIRK_WAIT_SEND_CMD;
	sdhci_cdns_mmc_ops = sdhci_ops;
#ifdef MMC_SUPPORTS_TUNING
	sdhci_cdns_mmc_ops.execute_tuning = sdhci_cdns_execute_tuning;
#endif

	ret = mmc_of_parse(dev, &plat->cfg);
	if (ret)
		return ret;

	phy = plat->priv;
#ifdef SD4_ENABLE
	ret = sdhci_cdns_sd4_phy_init(plat, gd->fdt_blob, dev_of_offset(dev));
	if (ret)
		return ret;
#else
	sdhci_cdns_sd6_plat_init(dev, plat);

#if defined(SDR_25MHZ)
        phy->t_sdclk = 5000;
        phy->t_sdmclk = 5000;
#else
        phy->t_sdmclk = 5000;
        phy->t_sdclk = 100000;
#endif

	ret = sdhci_cdns_sd6_phy_init(dev, plat);
	if (ret)
		return ret;
#endif

#ifdef SDR_10MHZ
	host->max_clk = SDR_10MHZ;
#else
	host->max_clk = SDHCI_CDNS_SD6_MAXCLK;
#endif
	host->mmc = &plat->mmc;
	host->mmc->dev = dev;

	DEBUG_DRV("sdmclk %d sdclk %d\n", phy->t_sdmclk, phy->t_sdclk);
	ret = sdhci_setup_cfg(&plat->cfg, host, phy->t.t_sdclk_min, phy->t.t_sdclk_max);
	if (ret)
		return ret;

	upriv->mmc = &plat->mmc;
	host->mmc->priv = host;

#ifdef PHY_DEBUG
	sdhci_cdns_sd6_dump(plat);
#endif
	return sdhci_probe(dev);
}

static const struct udevice_id sdhci_cdns_match[] = {
	{ .compatible = "socionext,uniphier-sd4hc" },
	{ .compatible = "cdns,sd4hc" },
	{ .compatible = "cdns,sd6hc" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(sdhci_cdns) = {
	.name = "sdhci-cdns",
	.id = UCLASS_MMC,
	.of_match = sdhci_cdns_match,
	.bind = sdhci_cdns_bind,
	.probe = sdhci_cdns_probe,
	.priv_auto_alloc_size = sizeof(struct sdhci_host),
	.platdata_auto_alloc_size = sizeof(struct sdhci_cdns_plat),
	.ops = &sdhci_cdns_mmc_ops,
};
