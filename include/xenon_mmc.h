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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ***************************************************************************
*/
/* xenon_emmc.h
 *
 * Definition for Marvell XENON SDIO/eMMC
 *
 * Author: Victor Gu <xigu@marvell.com>
 *
 * March 6, 2015
 *
 */
#ifndef _XENON_MMC_H_
#define _XENON_MMC_H_

#include <asm/io.h>
#include <mmc.h>
#include <sdhci.h>
#include <fdtdec.h>

/* Register Offset of SD Host Controller SOCP self-defined register */
#define SDHC_IPID				0x0100
#define SDHC_SYS_CFG_INFO			0x0104
#define SLOT_TYPE_SDIO_SHIFT			24
#define SLOT_TYPE_EMMC_MASK			0xff
#define SLOT_TYPE_EMMC_SHIFT			16
#define SLOT_TYPE_SD_SDIO_MMC_MASK		0xff
#define SLOT_TYPE_SD_SDIO_MMC_SHIFT		8

#define SDHC_SYS_OP_CTRL			0x0108
#define AUTO_CLKGATE_DISABLE_MASK		(0x1<<20)
#define SDCLK_IDLEOFF_ENABLE_SHIFT		8
#define SLOT_ENABLE_SHIFT			0

#define SDHC_SYS_EXT_OP_CTRL			0x010c
#define SDHC_TEST_OUT				0x0110
#define SDHC_TESTOUT_MUXSEL			0x0114

#define SDHC_SLOT_EXT_INT_STATUS		0x0120
#define SDHC_SLOT_EXT_ERR_STATUS		0x0122
#define SDHC_SLOT_EXT_INT_STATUS_EN		0x0124
#define SDHC_SLOT_EXT_ERR_STATUS_EN		0x0126

#define SDHC_SLOT_OP_STATUS_CTRL		0x0128
#define TUNING_PROG_FIXED_DELAY_MASK		0x7ff	/* MAX 0x7ff */
#define FORCE_SEL_INVERSE_CLK_SHIFT		11

#define SDHC_SLOT_FIFO_CTRL			0x012c

#define SDHC_SLOT_eMMC_CTRL			0x0130
#define ENABLE_DATA_STROBE_SHIFT		24
#define SET_EMMC_RSTN_SHIFT			16
#define eMMC_VCCQ_MASK				0x3
#define eMMC_VCCQ_1_8V				0x1
#define eMMC_VCCQ_1_2V				0x2
#define	eMMC_VCCQ_3_3V				0x3

#define SDHC_SLOT_OUTPUT_DLY_CTRL		0x0134
#define SDHC_SLOT_DCM_CTRL			0x0137

#define SDHC_SLOT_DLL_CTRL			0x0138
#define SELECT_DEF_DLL				0x1

#define SDHC_SLOT_DLL_PHASE_SEL			0x013c
#define DLL_UPDATE_STROBE			7

#define SDHC_SLOT_STROBE_DLY_CTRL		0x0140
#define STROBE_DELAY_FIXED_MASK			0xffff

#define SDHC_SLOT_RETUNING_REQ_CTRL		0x0144
/* retuning compatible */
#define RETUNING_COMPATIBLE			0x1

#define SDHC_SLOT_AUTO_RETUNING_CTRL		0x0148
#define ENABLE_AUTO_RETUNING			0x1

#define SDHC_SLOT_EXT_PRESENT_STATE		0x014c
#define SDHC_SLOT_DLL_CUR_DLY_VAL		0x0150
#define SDHC_SLOT_TUNING_CUR_DLY_VAL		0x0154
#define SDHC_SLOT_STROBE_CUR_DLY_VAL		0x0158
#define SDHC_SLOT_SUB_CMD_STRL			0x015c

#define SDHC_SLOT_CQ_TASK_INFO			0x0160

#define SDHC_SLOT_TUNING_DEBUG_INFO		0x01f0
#define SDHC_SLOT_DATAIN_DEBUG_INFO		0x01f4

#define XENON_MMC_MAX_CLK			(400000000)

#define XENON_MMC_CMD_MAX_TIMEOUT		3200
#define XENON_MMC_CMD_DEFAULT_TIMEOUT		100

/* Tuning Parameter */
#define TMR_RETUN_NO_PRESENT			0xf
#define XENON_MAX_TUN_COUNT			0xb

#define EMMC_PHY_REG_BASE			0x170
#define EMMC_PHY_TIMING_ADJUST			EMMC_PHY_REG_BASE
#define OUTPUT_QSN_PHASE_SELECT			(1 << 17)
#define SAMPL_INV_QSP_PHASE_SELECT		(1 << 18)
#define SAMPL_INV_QSP_PHASE_SELECT_SHIFT	18
#define PHY_INITIALIZAION			(1 << 31)
#define WAIT_CYCLE_BEFORE_USING_MASK		0xf
#define WAIT_CYCLE_BEFORE_USING_SHIFT		12
#define FC_SYNC_EN_DURATION_MASK		0xf
#define FC_SYNC_EN_DURATION_SHIFT		8
#define FC_SYNC_RST_EN_DURATION_MASK		0xf
#define FC_SYNC_RST_EN_DURATION_SHIFT		4
#define FC_SYNC_RST_DURATION_MASK		0xf
#define FC_SYNC_RST_DURATION_SHIFT		0

#define EMMC_PHY_FUNC_CONTROL			(EMMC_PHY_REG_BASE + 0x4)
#define DQ_ASYNC_MODE				(1 << 4)
#define DQ_DDR_MODE_SHIFT			8
#define DQ_DDR_MODE_MASK			0xff
#define CMD_DDR_MODE				(1 << 16)

#define EMMC_PHY_PAD_CONTROL			(EMMC_PHY_REG_BASE + 0x8)
#define REC_EN_SHIFT				24
#define REC_EN_MASK				0xf
#define FC_DQ_RECEN				(1 << 24)
#define FC_CMD_RECEN				(1 << 25)
#define FC_QSP_RECEN				(1 << 26)
#define FC_QSN_RECEN				(1 << 27)
#define OEN_QSN					(1 << 28)
#define AUTO_RECEN_CTRL				(1 << 30)

#define EMMC_PHY_PAD_CONTROL1			(EMMC_PHY_REG_BASE + 0xc)
#define EMMC_PHY_PAD_CONTROL2			(EMMC_PHY_REG_BASE + 0x10)
#define EMMC_PHY_DLL_CONTROL			(EMMC_PHY_REG_BASE + 0x14)
#define DLL_DELAY_TEST_LOWER_SHIFT		8
#define DLL_DELAY_TEST_LOWER_MASK		0xff
#define DLL_BYPASS_EN				0x1

#define EMMC_LOGIC_TIMING_ADJUST		(EMMC_PHY_REG_BASE + 0x18)
#define EMMC_LOGIC_TIMING_ADJUST_LOW		(EMMC_PHY_REG_BASE + 0x1c)

#define LOGIC_TIMING_VALUE			0x5a54 /* Recommend by HW team */

/* Hyperion only have one slot 0 */
#define XENON_MMC_SLOT_ID_HYPERION		(0)

#define MMC_TIMING_LEGACY	0
#define MMC_TIMING_MMC_HS	1
#define MMC_TIMING_SD_HS	2
#define MMC_TIMING_UHS_SDR12	3
#define MMC_TIMING_UHS_SDR25	4
#define MMC_TIMING_UHS_SDR50	5
#define MMC_TIMING_UHS_SDR104	6
#define MMC_TIMING_UHS_DDR50	7
#define MMC_TIMING_MMC_HS200	8
#define MMC_TIMING_MMC_HS400	10

/* Data time out default value 0xE: TMCLK x 227 */
#define DATA_TIMEOUT_DEF_VAL	0xe

/* SDMA start address should allign with 0x8, align mask 0x7 */
#define DMA_START_ADDR_ALIGN_MASK 0x7

/* Maxmium MMC port number
 * Armada-3700 has one eMMC port and one SD only port
 */
#define XENON_MMC_PORTS_MAX	2

#define  SDHCI_RETUNE_EVT_INTSIG	0x00001000

/* MMC modes */
#define XENON_MMC_MODE_EMMC	0
#define XENON_MMC_MODE_SD_SDIO	1

struct xenon_mmc_cfg {
	u64 reg_base;
	u32 version;
	u32 quirks;
	u32 clk;
	u32 bus_width;
	u32 mmc_mode;
	void *aligned_buffer;
	struct mmc_config cfg;
	struct mmc *mmc;
	struct fdt_gpio_state sdio_vcc_gpio;
};

#endif /* _XENON_MMC_H_ */

