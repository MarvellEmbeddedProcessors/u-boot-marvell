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
/* xenon_mmc.c
 *
 * XENON SDIO/EMMC implementation
 *
 * Author: Victor Gu, Marvell <xigu@marvell.com>
 *
 * March 6, 2015
 *
 */

#include <common.h>
#include <malloc.h>
#include <fdtdec.h>
#include <part.h>
#include <asm/io.h>
#include <asm/arch/regs-base.h>
#include <asm/arch-mvebu/fdt.h>
#include <mmc.h>
#include <sdhci.h>
#include <xenon_mmc.h>
#include <linux/sizes.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

static const char driver_name[] = "XENON-SDHCI";
const u32 block_size[4] = {512, 1024, 2048, 512};

/* GPIO output set to 1 to be 1.8v and 0 to be 3.3v */
#ifndef MVEBU_GPIO_SDIO_VOLTAGE_1_8V
#define MVEBU_GPIO_SDIO_VOLTAGE_1_8V 1
#endif
#ifndef MVEBU_GPIO_SDIO_VOLTAGE_3_3V
#define MVEBU_GPIO_SDIO_VOLTAGE_3_3V 0
#endif

static void xenon_mmc_writel(struct xenon_mmc_cfg *mmc_cfg, u32 offs, u32 val)
{
	writel(val, mmc_cfg->reg_base + (offs));
}

static u32 xenon_mmc_readl(struct xenon_mmc_cfg *mmc_cfg, u32 offs)
{
	return readl(mmc_cfg->reg_base + (offs));
}

static void xenon_mmc_writew(struct xenon_mmc_cfg *mmc_cfg, u32 offs, u16 val)
{
	writew(val, mmc_cfg->reg_base + (offs));
}

static u16 xenon_mmc_readw(struct xenon_mmc_cfg *mmc_cfg, u32 offs)
{
	return readw(mmc_cfg->reg_base + (offs));
}

static void xenon_mmc_writeb(struct xenon_mmc_cfg *mmc_cfg, u32 offs, u8 val)
{
	writeb(val, mmc_cfg->reg_base + (offs));
}

static u8 xenon_mmc_readb(struct xenon_mmc_cfg *mmc_cfg, u32 offs)
{
	return readb(mmc_cfg->reg_base + (offs));
}

static void xenon_mmc_reset(struct xenon_mmc_cfg *mmc_cfg, u8 mask)
{
	u32 timeout;

	/* Wait max 100 ms */
	timeout = 1000;
	xenon_mmc_writeb(mmc_cfg, SDHCI_SOFTWARE_RESET, mask);
	while (xenon_mmc_readb(mmc_cfg, SDHCI_SOFTWARE_RESET) & mask) {
		if (timeout == 0) {
			error("Reset 0x%x never completed.\n", (int)mask);
			return;
		}
		timeout--;
		udelay(100);
	}
}

int xenon_mmc_phy_init(struct xenon_mmc_cfg *mmc_cfg)
{
	u32 var;
	u32 wait;
	u16 clk_ctrl;
	u32 clock = mmc_cfg->clk;
	u32 time;

	debug_enter();

	/* Need to disable the clock to set EMMC_PHY_TIMING_ADJUST register */
	clk_ctrl = xenon_mmc_readw(mmc_cfg, SDHCI_CLOCK_CONTROL);
	clk_ctrl &= ~(SDHCI_CLOCK_CARD_EN | SDHCI_CLOCK_INT_EN);
	xenon_mmc_writew(mmc_cfg, SDHCI_CLOCK_CONTROL, clk_ctrl);

	/* Enable QSP PHASE SELECT */
	var = xenon_mmc_readl(mmc_cfg, EMMC_PHY_TIMING_ADJUST);
	var |= SAMPL_INV_QSP_PHASE_SELECT;
	xenon_mmc_writel(mmc_cfg, EMMC_PHY_TIMING_ADJUST, var);

	/* Enable internal clock */
	clk_ctrl = xenon_mmc_readw(mmc_cfg, SDHCI_CLOCK_CONTROL);
	xenon_mmc_writew(mmc_cfg, SDHCI_CLOCK_CONTROL, clk_ctrl | SDHCI_CLOCK_INT_EN);

	/* Poll for host MMC PHY clock init to be stable */
	/* Wait up to 10ms */
	time = 100;
	while (time--) {
		var = xenon_mmc_readl(mmc_cfg, SDHCI_CLOCK_CONTROL);
		if (var & SDHCI_CLOCK_INT_STABLE)
			break;

		udelay(100);
	}
	if (time <= 0) {
		error("Failed to enable MMC internal clock in time\n");
		return -1;
	}

	/* Enable bus clock */
	clk_ctrl = xenon_mmc_readw(mmc_cfg, SDHCI_CLOCK_CONTROL);
	xenon_mmc_writew(mmc_cfg, SDHCI_CLOCK_CONTROL, clk_ctrl | SDHCI_CLOCK_CARD_EN);

	/* Delay 200us to wait for the completion of bus clock */
	udelay(200);

	/* Init PHY */
	var = xenon_mmc_readl(mmc_cfg, EMMC_PHY_TIMING_ADJUST);
	var |= PHY_INITIALIZAION;
	xenon_mmc_writel(mmc_cfg, EMMC_PHY_TIMING_ADJUST, var);

	/* Add duration of FC_SYNC_RST */
	wait = ((var >> FC_SYNC_RST_DURATION_SHIFT) & FC_SYNC_RST_DURATION_MASK);
	/* Add interval between FC_SYNC_EN and FC_SYNC_RST */
	wait += ((var >> FC_SYNC_RST_EN_DURATION_SHIFT) & FC_SYNC_RST_EN_DURATION_MASK);
	/* Add duration of asserting FC_SYNC_EN */
	wait += ((var >> FC_SYNC_EN_DURATION_SHIFT) & FC_SYNC_EN_DURATION_MASK);
	/* Add duration of waiting for PHY */
	wait += ((var >> WAIT_CYCLE_BEFORE_USING_SHIFT) & WAIT_CYCLE_BEFORE_USING_MASK);
	/* According to Moyang, 4 addtional bus clock and 4 AXI bus clock are required */
	/* left shift 20 bits */
	wait += 8;
	wait <<= 20;

	if (clock == 0)
		/* Use the possibly slowest bus frequency value */
		clock = 100000;
	/* Get the wait time in unit of ms */
#ifdef CONFIG_PALLADIUM
	wait = (wait * 20000) / clock;
#else
	wait = wait / clock;
#endif
	wait++;

	/* Poll for host eMMC PHY init to complete */
	/* Wait up to 10ms */
	time = 100;
	while (time--) {
		var = xenon_mmc_readl(mmc_cfg, EMMC_PHY_TIMING_ADJUST);
		var &= PHY_INITIALIZAION;
		if (!var)
			break;

		/* wait for host eMMC PHY init to complete */
		udelay(100);
	}
	if (time <= 0) {
		error("Failed to init MMC PHY in time\n");
		return -1;
	}

	debug_exit();
	return 0;
}

void xenon_mmc_phy_set(struct xenon_mmc_cfg *mmc_cfg, u8 timing)
{
	u32 var;

	debug_enter();

	/* Setup pad, set bit[30], bit[28] and bits[26:24] */
	var = xenon_mmc_readl(mmc_cfg, EMMC_PHY_PAD_CONTROL);
	var |= (AUTO_RECEN_CTRL | OEN_QSN | FC_QSP_RECEN | FC_CMD_RECEN | FC_DQ_RECEN);
	xenon_mmc_writel(mmc_cfg, EMMC_PHY_PAD_CONTROL, var);

	/*
	 * If timing belongs to high speed, set bit[17] of
	 * EMMC_PHY_TIMING_ADJUST register
	 */
	if ((timing == MMC_TIMING_MMC_HS400) ||
	    (timing == MMC_TIMING_MMC_HS200) ||
	    (timing == MMC_TIMING_UHS_SDR50) ||
	    (timing == MMC_TIMING_UHS_SDR104) ||
	    (timing == MMC_TIMING_UHS_DDR50) ||
	    (timing == MMC_TIMING_UHS_SDR25)) {
		var = xenon_mmc_readl(mmc_cfg, EMMC_PHY_TIMING_ADJUST);

#ifdef CONFIG_PALLADIUM
		/*
		 * For current eMMC PHY bitfile, this high speed enable bit
		 * cannot be set because timing is not stable
		 */
		var &= ~OUTPUT_QSN_PHASE_SELECT;
#else
		var |= OUTPUT_QSN_PHASE_SELECT;
#endif
		xenon_mmc_writel(mmc_cfg, EMMC_PHY_TIMING_ADJUST, var);
	}

	/*
	 * If Palladium implementation, the output timing is hard to control.
	 * Thus enable DDR output timing for writing tuning,
	 * in all the speed mode
	 */
#ifndef CONFIG_PALLADIUM
	var = xenon_mmc_readl(mmc_cfg, EMMC_PHY_FUNC_CONTROL);
	var |= (DQ_DDR_MODE_MASK << DQ_DDR_MODE_SHIFT) | CMD_DDR_MODE;
	xenon_mmc_writel(mmc_cfg, EMMC_PHY_FUNC_CONTROL, var);
#else
	if ((timing == MMC_TIMING_UHS_DDR50) ||
	    (timing == MMC_TIMING_MMC_HS400)) {
		var = xenon_mmc_readl(mmc_cfg, EMMC_PHY_FUNC_CONTROL);
		var |= (DQ_DDR_MODE_MASK << DQ_DDR_MODE_SHIFT) | CMD_DDR_MODE;
		xenon_mmc_writel(mmc_cfg, EMMC_PHY_FUNC_CONTROL, var);
	}
#endif

	if (timing == MMC_TIMING_MMC_HS400) {
		var = xenon_mmc_readl(mmc_cfg, EMMC_PHY_FUNC_CONTROL);
		var &= ~DQ_ASYNC_MODE;
		xenon_mmc_writel(mmc_cfg, EMMC_PHY_FUNC_CONTROL, var);

#ifndef CONFIG_PALLADIUM
		/* Hardware team recommend a value for HS400 */
		xenon_mmc_writel(mmc_cfg, EMMC_LOGIC_TIMING_ADJUST, LOGIC_TIMING_VALUE);
#endif
	}

	xenon_mmc_phy_init(mmc_cfg);

	debug_exit();
}

/* Enable/Disable the Auto Clock Gating function of this slot */
static void xenon_mmc_set_acg(struct xenon_mmc_cfg *mmc_cfg, bool enable)
{
	u32 var;

	var = xenon_mmc_readl(mmc_cfg, SDHC_SYS_OP_CTRL);
	if (enable)
		var &= ~AUTO_CLKGATE_DISABLE_MASK;
	else
		var |= AUTO_CLKGATE_DISABLE_MASK;

	xenon_mmc_writel(mmc_cfg, SDHC_SYS_OP_CTRL, var);
}

/* Enable specific slot */
static void xenon_mmc_set_slot(struct xenon_mmc_cfg *mmc_cfg, u8 slot, bool enable)
{
	u32 var;

	var = xenon_mmc_readl(mmc_cfg, SDHC_SYS_OP_CTRL);
	if (enable)
		var |= ((0x1 << slot) << SLOT_ENABLE_SHIFT);
	else
		var &= ~((0x1 << slot) << SLOT_ENABLE_SHIFT);
	xenon_mmc_writel(mmc_cfg, SDHC_SYS_OP_CTRL, var);
}

/* Enable Parallel Transfer Mode */
static void xenon_mmc_set_parallel_tran(struct xenon_mmc_cfg *mmc_cfg, u8 slot, bool enable)
{
	u32 var;

	var = xenon_mmc_readl(mmc_cfg, SDHC_SYS_EXT_OP_CTRL);
	if (enable)
		var |= (0x1 << slot);
	else
		var &= ~(0x1 << slot);
	xenon_mmc_writel(mmc_cfg, SDHC_SYS_EXT_OP_CTRL, var);
}

static void xenon_mmc_set_tuning(struct xenon_mmc_cfg *mmc_cfg, u8 slot, bool enable)
{
	u32 var;

	/* Set the Re-Tuning Request functionality */
	var = xenon_mmc_readl(mmc_cfg, SDHC_SLOT_RETUNING_REQ_CTRL);
	if (enable)
		var |= RETUNING_COMPATIBLE;
	else
		var &= ~RETUNING_COMPATIBLE;
	xenon_mmc_writel(mmc_cfg, SDHC_SLOT_RETUNING_REQ_CTRL, var);

	/* Set the Re-tuning Event Signal Enable */
	var = xenon_mmc_readl(mmc_cfg, SDHCI_SIGNAL_ENABLE);
	if (enable)
		var |= SDHCI_RETUNE_EVT_INTSIG;
	else
		var &= ~SDHCI_RETUNE_EVT_INTSIG;
	xenon_mmc_writel(mmc_cfg, SDHCI_SIGNAL_ENABLE, var);
}

static void xenon_mmc_set_sdio_voltage(struct xenon_mmc_cfg *mmc_cfg, int voltage)
{
#ifdef CONFIG_MVEBU_GPIO
	if (!fdt_gpio_isvalid(&mmc_cfg->sdio_vcc_gpio)) {
		printf("ERROR: SDID vcc gpio is not set!\n");
		return;
	}

	if (MVEBU_GPIO_SDIO_VOLTAGE_1_8V == voltage)
		/* Set SDIO gpio to 1 which is 1.8v */
		fdtdec_set_gpio(&mmc_cfg->sdio_vcc_gpio, 1);
	else
		/* Set SDIO gpio to 0 which is 3.3v */
		fdtdec_set_gpio(&mmc_cfg->sdio_vcc_gpio, 0);
#else
	printf("ERROR: voltage not changed to %x, need to implement gpio in SOC code\n", voltage);
#endif

	return;
}

static void xenon_mmc_set_power(struct xenon_mmc_cfg *mmc_cfg, u32 vcc, u32 vccq)
{
	u8 pwr = 0;
	u32 ctrl = 0;

	/* Set VCC */
	switch (vcc) {
	case MMC_VDD_165_195:
		pwr = SDHCI_POWER_180;
		if (mmc_cfg->mmc_mode == XENON_MMC_MODE_SD_SDIO)
			xenon_mmc_set_sdio_voltage(mmc_cfg, MVEBU_GPIO_SDIO_VOLTAGE_1_8V);
		break;
	case MMC_VDD_29_30:
	case MMC_VDD_30_31:
		pwr = SDHCI_POWER_300;
		if (mmc_cfg->mmc_mode == XENON_MMC_MODE_SD_SDIO)
			xenon_mmc_set_sdio_voltage(mmc_cfg, MVEBU_GPIO_SDIO_VOLTAGE_3_3V);
		break;
	case MMC_VDD_32_33:
	case MMC_VDD_33_34:
		pwr = SDHCI_POWER_330;
		if (mmc_cfg->mmc_mode == XENON_MMC_MODE_SD_SDIO)
			xenon_mmc_set_sdio_voltage(mmc_cfg, MVEBU_GPIO_SDIO_VOLTAGE_3_3V);
		break;
	default:
		error("Does not support power mode(0x%X)\n", vcc);
		break;
	}

	if (pwr == 0) {
		debug("Disable power\n");
		xenon_mmc_writeb(mmc_cfg, SDHCI_POWER_CONTROL, 0);
		return;
	}

	pwr |= SDHCI_POWER_ON;

	xenon_mmc_writeb(mmc_cfg, SDHCI_POWER_CONTROL, pwr);

	/* Set VCCQ */
	ctrl = xenon_mmc_readl(mmc_cfg, SDHC_SLOT_eMMC_CTRL);
	ctrl |= vccq;
	xenon_mmc_writel(mmc_cfg, SDHC_SLOT_eMMC_CTRL, ctrl);
}

static void xenon_mmc_cmd_done(struct xenon_mmc_cfg *mmc_cfg, struct mmc_cmd *cmd)
{
	int i;
	if (cmd->resp_type & MMC_RSP_136) {
		/* CRC is stripped so we need to do some shifting. */
		for (i = 0; i < 4; i++) {
			cmd->response[i] = xenon_mmc_readl(mmc_cfg, SDHCI_RESPONSE + (3-i)*4) << 8;

			if (i != 3)
				cmd->response[i] |= xenon_mmc_readb(mmc_cfg, SDHCI_RESPONSE + (3-i)*4-1);
		}
	} else {
		cmd->response[0] =  xenon_mmc_readl(mmc_cfg, SDHCI_RESPONSE);
	}
}

static void xenon_mmc_transfer_pio(struct xenon_mmc_cfg *mmc_cfg, struct mmc_data *data)
{
	int i;
	char *offs;
	for (i = 0; i < data->blocksize; i += 4) {
		offs = data->dest + i;
		if (data->flags == MMC_DATA_READ)
			*(u32 *)offs = xenon_mmc_readl(mmc_cfg, SDHCI_BUFFER);
		else
			xenon_mmc_writel(mmc_cfg, SDHCI_BUFFER, *(u32 *)offs);
	}
}

static int xenon_mmc_transfer_data(struct xenon_mmc_cfg *mmc_cfg, struct mmc_data *data,
				u32 start_addr)
{
	u32 stat;
	u32 rdy;
	u32 mask;
	u32 timeout;
	u32 block = 0;
#ifdef CONFIG_MMC_SDMA
	u8 ctrl;
	ctrl = xenon_mmc_readb(mmc_cfg, SDHCI_HOST_CONTROL);
	ctrl &= ~SDHCI_CTRL_DMA_MASK;
	xenon_mmc_writeb(mmc_cfg, SDHCI_HOST_CONTROL, ctrl);
#endif

	/* wait 1s at max */
	timeout = 100000;
	rdy = SDHCI_INT_SPACE_AVAIL | SDHCI_INT_DATA_AVAIL;
	mask = SDHCI_DATA_AVAILABLE | SDHCI_SPACE_AVAILABLE;
	do {
		stat = xenon_mmc_readl(mmc_cfg, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			debug("Error detected in status(0x%X)\n", stat);
			return -1;
		}
		if (stat & rdy) {
			if (!(xenon_mmc_readl(mmc_cfg, SDHCI_PRESENT_STATE) & mask))
				continue;
			xenon_mmc_writel(mmc_cfg, SDHCI_INT_STATUS, rdy);
			xenon_mmc_transfer_pio(mmc_cfg, data);
			data->dest += data->blocksize;
			if (++block >= data->blocks)
				break;
		}
#ifdef CONFIG_MMC_SDMA
		if (stat & SDHCI_INT_DMA_END) {
			xenon_mmc_writel(mmc_cfg, SDHCI_INT_STATUS, SDHCI_INT_DMA_END);
			start_addr &= ~(SDHCI_DEFAULT_BOUNDARY_SIZE - 1);
			start_addr += SDHCI_DEFAULT_BOUNDARY_SIZE;
			xenon_mmc_writel(mmc_cfg, SDHCI_DMA_ADDRESS, start_addr);
		}
#endif
		if (timeout-- > 0) {
			udelay(10);
		} else {
			debug("Transfer data timeout\n");
			return -1;
		}
	} while (!(stat & SDHCI_INT_DATA_END));
	return 0;
}

static int xenon_mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			      struct mmc_data *data)
{
	u32 stat = 0;
	int ret = 0;
	int trans_bytes = 0;
	int is_aligned = 1;
	u32 mask = 0;
	u32 flags;
	u32 mode;
	u32 time = 0;
	u32 start_addr = 0;
	u32 retry = 1000;
	u32 cmd_timeout = XENON_MMC_CMD_DEFAULT_TIMEOUT;
	struct xenon_mmc_cfg *mmc_cfg = mmc->priv;
	int mmc_dev = mmc->block_dev.dev;

	/* Clear status */
	xenon_mmc_writel(mmc_cfg, SDHCI_INT_STATUS, SDHCI_INT_ALL_MASK);

	/* Init mask */
	if (cmd)
		mask |= SDHCI_CMD_INHIBIT;
	if (data)
		mask |= SDHCI_DATA_INHIBIT;

	/*
	 * We shouldn't wait for data inihibit for stop commands, even
	 * though they might use busy signaling
	 */
	if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
		mask &= ~SDHCI_DATA_INHIBIT;

	while (xenon_mmc_readl(mmc_cfg, SDHCI_PRESENT_STATE) & mask) {
		if (time >= cmd_timeout) {
			debug("MMC(%d) busy\n", mmc_dev);
			if (2 * cmd_timeout <= XENON_MMC_CMD_MAX_TIMEOUT) {
				cmd_timeout += cmd_timeout;
				debug("timeout increasing to: %u ms\n", cmd_timeout);
			} else {
				debug("timeout.\n");
				if (cmd)
					xenon_mmc_reset(mmc_cfg, SDHCI_RESET_CMD);
				if (data)
					xenon_mmc_reset(mmc_cfg, SDHCI_RESET_DATA);
				return TIMEOUT;
			}
		}
		time++;
		mdelay(1);
	}

	mask = SDHCI_INT_RESPONSE;
	if (!(cmd->resp_type & MMC_RSP_PRESENT)) {
		flags = SDHCI_CMD_RESP_NONE;
	} else if (cmd->resp_type & MMC_RSP_136) {
		flags = SDHCI_CMD_RESP_LONG;
	} else if (cmd->resp_type & MMC_RSP_BUSY) {
		flags = SDHCI_CMD_RESP_SHORT_BUSY;
		mask |= SDHCI_INT_DATA_END;
	} else {
		flags = SDHCI_CMD_RESP_SHORT;
	}

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= SDHCI_CMD_CRC;
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= SDHCI_CMD_INDEX;
	if (data)
		flags |= SDHCI_CMD_DATA;

	/* Set Transfer mode regarding to data flag */
	if (data != 0) {
		xenon_mmc_writeb(mmc_cfg, SDHCI_TIMEOUT_CONTROL, DATA_TIMEOUT_DEF_VAL);
		mode = SDHCI_TRNS_BLK_CNT_EN;
		trans_bytes = data->blocks * data->blocksize;
		if (data->blocks > 1)
			mode |= SDHCI_TRNS_MULTI;

		if (data->flags == MMC_DATA_READ)
			mode |= SDHCI_TRNS_READ;

#ifdef CONFIG_MMC_SDMA
		if (data->flags == MMC_DATA_READ)
			start_addr = (unsigned int)data->dest;
		else
			start_addr = (unsigned int)data->src;
		if ((mmc_cfg->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) &&
		    (start_addr & DMA_START_ADDR_ALIGN_MASK) != 0x0) {
			is_aligned = 0;
			start_addr = (unsigned int)mmc_cfg->aligned_buffer;
			if (data->flags != MMC_DATA_READ)
				memcpy(mmc_cfg->aligned_buffer, data->src, trans_bytes);
		}

		xenon_mmc_writel(mmc_cfg, SDHCI_DMA_ADDRESS, start_addr);
		mode |= SDHCI_TRNS_DMA;
#endif
		xenon_mmc_writew(mmc_cfg, SDHCI_BLOCK_SIZE,
				 SDHCI_MAKE_BLKSZ(SDHCI_DEFAULT_BOUNDARY_ARG, data->blocksize));
		xenon_mmc_writew(mmc_cfg, SDHCI_BLOCK_COUNT, data->blocks);
		xenon_mmc_writew(mmc_cfg, SDHCI_TRANSFER_MODE, mode);
	}

	xenon_mmc_writel(mmc_cfg, SDHCI_ARGUMENT, cmd->cmdarg);
#ifdef CONFIG_MMC_SDMA
	flush_cache(start_addr, trans_bytes);
#endif
	xenon_mmc_writew(mmc_cfg, SDHCI_COMMAND, SDHCI_MAKE_CMD(cmd->cmdidx, flags));

	do {
		stat = xenon_mmc_readl(mmc_cfg, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			debug("SDHCI_INT_ERROR stat(%x)\n", stat);
			break;
		}
		udelay(100);

		if (--retry == 0)
			break;
	} while ((stat & mask) != mask);

	if (retry == 0) {
		if (mmc_cfg->quirks & SDHCI_QUIRK_BROKEN_R1B) {
			return 0;
		} else {
			error("Timeout for RX status update!\n");
			return TIMEOUT;
		}
	}

	if ((stat & (SDHCI_INT_ERROR | mask)) == mask) {
		xenon_mmc_cmd_done(mmc_cfg, cmd);
		xenon_mmc_writel(mmc_cfg, SDHCI_INT_STATUS, mask);
	} else {
		ret = -1;
	}

	if (!ret && data)
		ret = xenon_mmc_transfer_data(mmc_cfg, data, start_addr);

	if (mmc_cfg->quirks & SDHCI_QUIRK_WAIT_SEND_CMD)
		udelay(1000);

	stat = xenon_mmc_readl(mmc_cfg, SDHCI_INT_STATUS);

	xenon_mmc_writel(mmc_cfg, SDHCI_INT_STATUS, SDHCI_INT_ALL_MASK);
	if (!ret && data) {
		if ((mmc_cfg->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) &&
		    (!is_aligned) && (data->flags == MMC_DATA_READ))
			memcpy(data->dest, mmc_cfg->aligned_buffer, trans_bytes);
		return 0;
	}

	if (stat & SDHCI_INT_ERROR) {
		if (cmd)
			xenon_mmc_reset(mmc_cfg, SDHCI_RESET_CMD);
		if (data)
			xenon_mmc_reset(mmc_cfg, SDHCI_RESET_DATA);

		if (stat & (SDHCI_INT_TIMEOUT | SDHCI_INT_DATA_TIMEOUT))
			return TIMEOUT;
		else
			return COMM_ERR;
	} else {
		return 0;
	}
}

static int xenon_mmc_set_clk(struct mmc *mmc, u32 clock)
{
	u32 div;
	u32 clk;
	u32 timeout;

	struct xenon_mmc_cfg *mmc_cfg = mmc->priv;

	debug_enter();

	xenon_mmc_writew(mmc_cfg, SDHCI_CLOCK_CONTROL, 0);

	if (clock == 0)
		return 0;

	if (SDHCI_GET_VERSION(mmc_cfg) >= SDHCI_SPEC_300) {
		/* Version 3.00 divisors must be a multiple of 2. */
		if (mmc->cfg->f_max <= clock) {
			div = 1;
		} else {
			for (div = 2; div < SDHCI_MAX_DIV_SPEC_300; div += 2) {
				if ((mmc->cfg->f_max / div) <= clock)
					break;
			}
		}
	} else {
		/* Version 2.00 divisors must be a power of 2. */
		for (div = 1; div < SDHCI_MAX_DIV_SPEC_200; div *= 2) {
			if ((mmc->cfg->f_max / div) <= clock)
				break;
		}
	}
	div >>= 1;

#ifdef CONFIG_PALLADIUM
	div = 4;
#endif
	clk = (div & SDHCI_DIV_MASK) << SDHCI_DIVIDER_SHIFT;
	clk |= ((div & SDHCI_DIV_HI_MASK) >> SDHCI_DIV_MASK_LEN)
		<< SDHCI_DIVIDER_HI_SHIFT;
	clk |= SDHCI_CLOCK_INT_EN;
	xenon_mmc_writew(mmc_cfg, SDHCI_CLOCK_CONTROL, clk);

	/* Wait max 20 ms */
	timeout = 200;
	while (!((clk = xenon_mmc_readw(mmc_cfg, SDHCI_CLOCK_CONTROL))
		& SDHCI_CLOCK_INT_STABLE)) {
		if (timeout == 0) {
			error("Internal clock never stablised.\n");
			return -1;
		}
		timeout--;
		udelay(100);
	}

	clk |= SDHCI_CLOCK_CARD_EN;
	xenon_mmc_writew(mmc_cfg, SDHCI_CLOCK_CONTROL, clk);

	/* Save clk */
	mmc_cfg->clk = clk;
	debug_exit();

	return 0;
}

static void xenon_mmc_set_bus(struct mmc *mmc, u32 bus)
{
	u8 ctrl;

	struct xenon_mmc_cfg *mmc_cfg = mmc->priv;

	debug_enter();

	if (bus == 1)
		bus = EXT_CSD_BUS_WIDTH_1;
	else if (bus == 4)
		bus = EXT_CSD_BUS_WIDTH_4;
	else if (bus == 8)
		bus = EXT_CSD_BUS_WIDTH_8;

	/* Set bus width */
	ctrl = xenon_mmc_readb(mmc_cfg, SDHCI_HOST_CONTROL);
	if (bus == EXT_CSD_BUS_WIDTH_8) {
		ctrl &= ~SDHCI_CTRL_4BITBUS;
		if ((SDHCI_GET_VERSION(mmc_cfg) >= SDHCI_SPEC_300) ||
		    (mmc_cfg->quirks & SDHCI_QUIRK_USE_WIDE8))
			ctrl |= SDHCI_CTRL_8BITBUS;
	} else {
		if (SDHCI_GET_VERSION(mmc_cfg) >= SDHCI_SPEC_300)
			ctrl &= ~SDHCI_CTRL_8BITBUS;
		if (bus == EXT_CSD_BUS_WIDTH_4)
			ctrl |= SDHCI_CTRL_4BITBUS;
		else
			ctrl &= ~SDHCI_CTRL_4BITBUS;
	}

	if (mmc->clock > 26000000)
		ctrl |= SDHCI_CTRL_HISPD;
	else
		ctrl &= ~SDHCI_CTRL_HISPD;
#ifdef CONFIG_PALLADIUM
	ctrl |= SDHCI_CTRL_HISPD;
#endif
	if (mmc_cfg->quirks & SDHCI_QUIRK_NO_HISPD_BIT)
		ctrl &= ~SDHCI_CTRL_HISPD;

	xenon_mmc_writeb(mmc_cfg, SDHCI_HOST_CONTROL, ctrl);

	/* Save bus width */
	mmc_cfg->bus_width = bus;

	debug_exit();
}

static void xenon_mmc_set_ios(struct mmc *mmc)
{
	struct xenon_mmc_cfg *mmc_cfg = mmc->priv;

	debug_enter();
	debug("bus_width(%d) clock(%d)\n", mmc->bus_width, mmc->clock);

	/* Only update with valid bus_width and clock */
	if (mmc->bus_width)
		xenon_mmc_set_bus(mmc, mmc->bus_width);
	if (mmc->clock)
		xenon_mmc_set_clk(mmc, mmc->clock);

	/* Re-init the PHY */
	xenon_mmc_phy_init(mmc_cfg);

	debug_exit();
}

static int xenon_mmc_init(struct mmc *mmc)
{
	struct xenon_mmc_cfg *mmc_cfg = mmc->priv;

	debug_enter();
	debug("reg_base(%llx) version(%d) quirks(%x) clk(%d) bus_width(%d)\n",
	      mmc_cfg->reg_base, mmc_cfg->version, mmc_cfg->quirks, mmc_cfg->clk, mmc_cfg->bus_width);

	/* Set FIFO */
	xenon_mmc_writel(mmc_cfg, SDHC_SLOT_FIFO_CTRL, 0x315);

	if ((mmc_cfg->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) && !mmc_cfg->aligned_buffer) {
		mmc_cfg->aligned_buffer = memalign(8, SZ_512K);
		if (!mmc_cfg->aligned_buffer) {
			error("Aligned buffer alloc failed\n");
			return -1;
		}
	}

	/* Disable auto clock gating during init */
	xenon_mmc_set_acg(mmc_cfg, false);

	/* Enable slot */
	xenon_mmc_set_slot(mmc_cfg, XENON_MMC_SLOT_ID_HYPERION, true);

	/* Set default power
	 * eMMC mode, VDD: 1.8V, VCCQ: 1.8V
	 * SD/SDIO mode, VDD: 3.3V, VCCQ: 3.3V
	 */
	if (mmc_cfg->mmc_mode == XENON_MMC_MODE_EMMC)
		xenon_mmc_set_power(mmc_cfg, MMC_VDD_165_195, eMMC_VCCQ_1_8V);
	else
		xenon_mmc_set_power(mmc_cfg, MMC_VDD_32_33, eMMC_VCCQ_3_3V);

	/* Set default clock */
	xenon_mmc_set_clk(mmc, mmc_cfg->clk);

	/* Init PHY and set default timing */
	xenon_mmc_phy_set(mmc_cfg, MMC_TIMING_UHS_SDR50);

	/* Clear interrupt status */
	xenon_mmc_writel(mmc_cfg, SDHCI_INT_STATUS, 0xFFFFFFFF);
	xenon_mmc_writel(mmc_cfg, SDHCI_INT_STATUS, 0xFFFFFFFF);

	/* Enable only interrupts served by the SD controller */
	/* SDHCI_INT_DATA_MASK | SDHCI_INT_CMD_MASK | SDHCI_INT_CARD_INSERT | SDHCI_INT_CARD_REMOVE); */
	xenon_mmc_writel(mmc_cfg, SDHCI_INT_ENABLE, 0xFFFFFEBF);

	/* Mask all sdhci interrupt sources */
	/* SDHCI_INT_DATA_MASK | SDHCI_INT_CMD_MASK | SDHCI_INT_CARD_INSERT | SDHCI_INT_CARD_REMOVE); */
	xenon_mmc_writel(mmc_cfg, SDHCI_SIGNAL_ENABLE, 0xFFFFFEFF);

	/* Enable parallel transfer */
	xenon_mmc_set_parallel_tran(mmc_cfg, XENON_MMC_SLOT_ID_HYPERION, true);

	/* Disable tuning functionality of this slot */
	xenon_mmc_set_tuning(mmc_cfg, XENON_MMC_SLOT_ID_HYPERION, false);

	/* Enable auto clock gating after init */
	xenon_mmc_set_acg(mmc_cfg, true);

	debug_exit();

	return 0;
}

static const struct mmc_ops xenon_mmc_ops = {
	.send_cmd	= xenon_mmc_send_cmd,
	.set_ios	= xenon_mmc_set_ios,
	.init		= xenon_mmc_init,
};

int xenon_mmc_create(int dev_idx, void __iomem *reg_base, u32 max_clk,
				u32 mmc_mode, u32 dt_mmc_host_cap, struct fdt_gpio_state *gpio)
{
	u32 caps;
	struct xenon_mmc_cfg *mmc_cfg = NULL;

	mmc_cfg = (struct xenon_mmc_cfg *)calloc(1, sizeof(struct xenon_mmc_cfg));
	if (!mmc_cfg) {
		error("xenon_mmc_cfg malloc fail\n");
		return 1;
	}

	/* Set quirks */
	mmc_cfg->quirks = SDHCI_QUIRK_NO_CD | SDHCI_QUIRK_WAIT_SEND_CMD |
			  SDHCI_QUIRK_32BIT_DMA_ADDR;

	/* Set reg base, mode and name */
	mmc_cfg->reg_base = (u64)reg_base;
	mmc_cfg->mmc_mode = mmc_mode;
	mmc_cfg->cfg.name = driver_name;

	/* Set version and ops */
	mmc_cfg->version = xenon_mmc_readw(mmc_cfg, SDHCI_HOST_VERSION);
	mmc_cfg->cfg.ops = &xenon_mmc_ops;

#ifdef CONFIG_MVEBU_GPIO
	/*Set sdio vcc gpio*/
	memcpy(&mmc_cfg->sdio_vcc_gpio, gpio, sizeof(mmc_cfg->sdio_vcc_gpio));
#endif

	caps = xenon_mmc_readl(mmc_cfg, SDHCI_CAPABILITIES);

#ifdef CONFIG_MMC_SDMA
	if (!(caps & SDHCI_CAN_DO_SDMA)) {
		error("SDIO controller doesn't support SDMA\n");
		free(mmc_cfg);
		return -1;
	}
#endif

#ifdef CONFIG_MMC_ADMA
	if (!(caps & SDHCI_CAN_DO_ADMA2)) {
		error("SDIO controller doesn't support ADMA2\n");
		free(mmc_cfg);
		return -1;
	}
#endif

	/* Set max and min clk */
	if (max_clk) {
		mmc_cfg->cfg.f_max = max_clk;
	} else {
		if (SDHCI_GET_VERSION(mmc_cfg) >= SDHCI_SPEC_300)
			mmc_cfg->cfg.f_max = (caps & SDHCI_CLOCK_V3_BASE_MASK)
				>> SDHCI_CLOCK_BASE_SHIFT;
		else
			mmc_cfg->cfg.f_max = (caps & SDHCI_CLOCK_BASE_MASK)
				>> SDHCI_CLOCK_BASE_SHIFT;
		mmc_cfg->cfg.f_max *= 1000000;
	}

	if (mmc_cfg->cfg.f_max == 0) {
		error("Hardware doesn't specify base clock frequency\n");
		free(mmc_cfg);
		return -1;
	}

	if (SDHCI_GET_VERSION(mmc_cfg) >= SDHCI_SPEC_300)
		mmc_cfg->cfg.f_min = mmc_cfg->cfg.f_max /
			SDHCI_MAX_DIV_SPEC_300;
	else
		mmc_cfg->cfg.f_min = mmc_cfg->cfg.f_max /
			SDHCI_MAX_DIV_SPEC_200;
	mmc_cfg->clk = mmc_cfg->cfg.f_max;

	/* Set voltages */
	mmc_cfg->cfg.voltages = 0;
	if (caps & SDHCI_CAN_VDD_330)
		mmc_cfg->cfg.voltages |= MMC_VDD_32_33 | MMC_VDD_33_34;
	if (caps & SDHCI_CAN_VDD_300)
		mmc_cfg->cfg.voltages |= MMC_VDD_29_30 | MMC_VDD_30_31;
	if (caps & SDHCI_CAN_VDD_180)
		mmc_cfg->cfg.voltages |= MMC_VDD_165_195;

	/* Set host capabilities */
	mmc_cfg->cfg.host_caps = MMC_MODE_HS | MMC_MODE_HS_52MHz | MMC_MODE_4BIT | MMC_MODE_HC;

	/* Do not enable DDR mode for palladium since it is quite slow */
#ifndef CONFIG_PALLADIUM
	mmc_cfg->cfg.host_caps |= MMC_MODE_DDR_52MHz;
#endif
	if (SDHCI_GET_VERSION(mmc_cfg) >= SDHCI_SPEC_300) {
		if (caps & SDHCI_CAN_DO_8BIT)
			mmc_cfg->cfg.host_caps |= MMC_MODE_8BIT;
	}
	/* Update the capability of the host, by the capability of the system/board that
	** got from device-tree */
	mmc_cfg->cfg.host_caps &= dt_mmc_host_cap;

	/* Set max block size in byte and part type */
	mmc_cfg->cfg.b_max = block_size[(caps & SDHCI_MAX_BLOCK_MASK) >> SDHCI_MAX_BLOCK_SHIFT];
	mmc_cfg->cfg.part_type = PART_TYPE_DOS;

	xenon_mmc_reset(mmc_cfg, SDHCI_RESET_ALL);

	mmc_cfg->mmc = mmc_create(&mmc_cfg->cfg, mmc_cfg);
	if (mmc_cfg->mmc == NULL) {
		error("mmc create failed\n");
		free(mmc_cfg);
		return -1;
	}

	return 0;
}

/* weak implementation: dependant on SoC requirements.
 * will be overrun in soc.c, in case a platforms requires special
 * SoC configuration prior to XENON usage */
void __weak mmc_soc_init(void)
{
	return;
}

int board_mmc_init(bd_t *bis)
{
	int node_list[XENON_MMC_PORTS_MAX];
	int count, err = 0;
	int port_count;
	u32 mmc_mode, dt_mmc_host_cap, bus_width;
	void __iomem *reg_base;
	const void *blob = gd->fdt_blob;
	struct fdt_gpio_state sdio_vcc_gpio;

	count = fdtdec_find_aliases_for_id(blob, "xenon-sdhci",
			COMPAT_MVEBU_XENON_MMC, &node_list[0], XENON_MMC_PORTS_MAX);
	if (count > 0)
		mmc_soc_init();

	for (port_count = 0; port_count < count; port_count++) {
		if (port_count == XENON_MMC_PORTS_MAX) {
			printf("XENON: Cannot register more than %d ports\n", XENON_MMC_PORTS_MAX);
			return -1;
		}

		debug("XENON %d: ", port_count);
		if (!fdtdec_get_is_enabled(blob, node_list[port_count]))
			continue;

		reg_base = fdt_get_regs_offs(blob, node_list[port_count], "reg");
		if (reg_base == 0) {
			error("Missing registers in XENON SDHCI node\n");
			continue;
		}

		/* Xenon emmc: this is a emmc slot.
		 * Actually, whether current slot is for emmc can be
		 * extracted from SDHC_SYS_CFG_INFO register. However, some Xenon IP
		 * versions might not implement the slot type information. In such a case,
		 * it is necessary to explicitly indicate the emmc type.
		 */
		if (fdtdec_get_bool(blob, node_list[port_count], "xenon,emmc"))
			mmc_mode = XENON_MMC_MODE_EMMC;
		else
			mmc_mode = XENON_MMC_MODE_SD_SDIO;

		/* Enable all modes for now - later we can add every parameter
		** easly to device-tree, and enable/disable it */
		dt_mmc_host_cap = MMC_MODE_HS | MMC_MODE_HS_52MHz | MMC_MODE_HC | MMC_MODE_DDR_52MHz;
		/* Get bus-width from device-tree and update the capability of
		** the SoC */
		bus_width = fdtdec_get_int(blob, node_list[port_count], "bus-width", 1);
		if (bus_width == 4) {
			dt_mmc_host_cap |= MMC_MODE_4BIT;
		} else if (bus_width == 8) {
			/* Enable 4Bit for backward compatibility for 4Bit support */
			dt_mmc_host_cap |= MMC_MODE_4BIT;
			dt_mmc_host_cap |= MMC_MODE_8BIT;
		}

		/* Only SD/SDIO mode supports vcc setting through gpio and emmc mode does not support it. */
		/* The vcc gpio should be in output mode, the output value 0 means SDIO is in 3.3v */
		/* and value 1 means SDIO is in 1.8v. */
		if (mmc_mode == XENON_MMC_MODE_SD_SDIO) {
#ifdef CONFIG_MVEBU_GPIO
			/* parse the sdio vcc gpio fdt attibute*/
			fdtdec_decode_gpio(blob, node_list[port_count], "sdio-vcc-gpio", &sdio_vcc_gpio);
			fdtdec_setup_gpio(&sdio_vcc_gpio);
			if (fdt_gpio_isvalid(&sdio_vcc_gpio)) {
				int val;

				/* initialize SDIO GPIO in low level which is 3.3v by default*/
				val = sdio_vcc_gpio.flags & FDT_GPIO_ACTIVE_LOW ? 1 : 0;

				/* Set to SDIO GPIO output mode */
				gpio_direction_output(sdio_vcc_gpio.gpio, val);
			} else {
				printf("ERROR: missing SDIO vcc gpio in XENON SDHCI node!\n");
				continue;
			}
#else
			printf("ERROR: vcc gpio is not initialized, need to implement gpio in SOC code\n");
#endif
		}
		xenon_mmc_create(port_count, reg_base, XENON_MMC_MAX_CLK, mmc_mode, dt_mmc_host_cap, &sdio_vcc_gpio);
	}

	return err;
}
