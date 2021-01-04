/***********************license start***********************************
 * Copyright (C) 2021 Marvell International Ltd.
 * SPDX-License-Identifier: BSD-3-Clause
 * https://spdx.org/licenses
 **********************license end**************************************/

#include <clk.h>
#include <dm.h>
#include <malloc.h>
#include <spi.h>
#include <spi-mem.h>
#include <watchdog.h>
#include <asm/io.h>
#include <asm/unaligned.h>
#include <linux/bitfield.h>
#include <linux/compat.h>
#include <linux/delay.h>
#include <linux/iopoll.h>
#include <linux/log2.h>

#include <linux/dma-mapping.h>
#include <linux/dma-direction.h>

#define CDNS_XSPI_MAGIC_NUM_VALUE 0x6522
#define CDNS_XSPI_NAME "spi-cadence-octeon"
#define CDNS_XSPI_AUX_REGISTER_OFFSET (0x2000)
#define CDNS_XSPI_DIRECT_OFFSET       (0x10000000)

#define MEMORY_ALIGN_TO     (8)
#define DIRECT_SIZE         (0x20000)
#define MAX_CS_COUNT        (4)

/*
 * Note: below are additional auxiliary registers to
 * configure XSPI controller pin-strap settings
 */

/* PHY DQ timing register */
#define CDNS_XSPI_CCP_PHY_DQ_TIMING           0x0000
#define CDNS_XSPI_CCP_DATA_SELECT_OE_START    GENMASK(6, 4)
#define CDNS_XSPI_CCP_DATA_SELECT_OE_END      GENMASK(2, 0)
#define CDNS_XSPI_CCP_PHY_DQ_TIMING_INIT_VAL  0x80000000

/* PHY DQS timing register */
#define CDNS_XSPI_CCP_PHY_DQS_TIMING          0x0004
#define CDNS_XSPI_CCP_USE_EXT_LPBCK_DQS       BIT(22)
#define CDNS_XSPI_CCP_USE_LPBCK_DQS           BIT(21)
#define CDNS_XSPI_CCP_USE_PHONY               BIT(20)
#define CDNS_XSPI_CCP_DQS_SELECT_OE_START     GENMASK(7, 4)
#define CDNS_XSPI_CCP_DQS_SELECT_OE_END       GENMASK(3, 0)

/* PHY gate loopback control register */
#define CDNS_XSPI_CCP_PHY_GATE_LPBCK_CTRL     0x0008
#define CDNS_XSPI_CCP_READ_DATA_DELAY_SEL     GENMASK(24, 19)
#define CDNS_XSPI_CCP_GATE_CFG_CLOSE          GENMASK(5, 4)
#define CDNS_XSPI_CCP_GATE_CFG                GENMASK(3, 0)

/* PHY DLL slave control register */
#define CDNS_XSPI_CCP_PHY_DLL_SLAVE_CTRL      0x0010
#define CDNS_XSPI_CCP_CLK_WR_DELAY            GENMASK(15, 8)
#define CDNS_XSPI_CCP_READ_DQS_DELAY          GENMASK(7, 0)

#define CDNS_XSPI_AUX_PHY_ADDONS_REG          0x0040
#define CDNS_XSPI_AUX_PHY_ADDONS_VALUE        0xE0012000
#define CDNS_XSPI_AUX_DEV_DISC_CONFIG_REG     0x0048
#define CDNS_XSPI_AUX_DEV_DISC_CONFIG_VALUE   0x00000000
#define CDNS_XSPI_AUX_DRIVING_REG             0x0050
#define CDNS_XSPI_AUX_CTRL_RESET              BIT(0)

/* DLL PHY control register */
#define CDNS_XSPI_DLL_PHY_CTRL                0x1034
#define CDNS_XSPI_CCP_DQS_LAST_DATA_DROP_EN   BIT(20)

/* Command registers */
#define CDNS_XSPI_CMD_REG_0                   0x0000
#define CDNS_XSPI_CMD_REG_1                   0x0004
#define CDNS_XSPI_CMD_REG_2                   0x0008
#define CDNS_XSPI_CMD_REG_3                   0x000C
#define CDNS_XSPI_CMD_REG_4                   0x0010
#define CDNS_XSPI_CMD_REG_5                   0x0014

/* Command status registers */
#define CDNS_XSPI_CMD_STATUS_REG              0x0044
#define CDNS_XSPI_CMD_COMPLETED               BIT(15)
#define CDNS_XSPI_CMD_FAILED                  BIT(14)

/* Controller status register */
#define CDNS_XSPI_CTRL_STATUS_REG             0x0100
#define CDNS_XSPI_INIT_COMPLETED              BIT(16)
#define CDNS_XSPI_INIT_LEGACY                 BIT(9)
#define CDNS_XSPI_INIT_FAIL                   BIT(8)
#define CDNS_XSPI_CTRL_BUSY                   BIT(7)
#define CDNS_XSPI_DISCOVERY_BUSY              BIT(6)

/* Controller interrupt status register */
#define CDNS_XSPI_INTR_STATUS_REG             0x0110
#define CDNS_XSPI_STIG_DONE                   BIT(23)
#define CDNS_XSPI_SDMA_ERROR                  BIT(22)
#define CDNS_XSPI_SDMA_TRIGGER                BIT(21)

/* Controller interrupt enable register */
#define CDNS_XSPI_INTR_ENABLE_REG             0x0114
#define CDNS_XSPI_INTR_EN                     BIT(31)
#define CDNS_XSPI_STIG_DONE_EN                BIT(23)
#define CDNS_XSPI_SDMA_ERROR_EN               BIT(22)
#define CDNS_XSPI_SDMA_TRIGGER_EN             BIT(21)

#define CDNS_XSPI_INTR_MASK (CDNS_XSPI_INTR_EN | \
	CDNS_XSPI_STIG_DONE_EN  | \
	CDNS_XSPI_SDMA_ERROR_EN | \
	CDNS_XSPI_SDMA_TRIGGER_EN)

/* Controller config register */
#define CDNS_XSPI_CTRL_CONFIG_REG             0x0230
#define CDNS_XSPI_CTRL_WORK_MODE              GENMASK(6, 5)

/* SDMA trigger transaction registers */
#define CDNS_XSPI_SDMA_SIZE_REG               0x0240
#define CDNS_XSPI_SDMA_TRD_INFO_REG           0x0244
#define CDNS_XSPI_SDMA_DIR                    BIT(8)

/* Controller features register */
#define CDNS_XSPI_CTRL_FEATURES_REG           0x0F04
#define CDNS_XSPI_NUM_BANKS                   GENMASK(25, 24)
#define CDNS_XSPI_DMA_DATA_WIDTH              BIT(21)
#define CDNS_XSPI_NUM_THREADS                 GENMASK(3, 0)

/* Controller version register */
#define CDNS_XSPI_CTRL_VERSION_REG            0x0F00
#define CDNS_XSPI_MAGIC_NUM                   GENMASK(31, 16)
#define CDNS_XSPI_CTRL_REV                    GENMASK(7, 0)

/* STIG Profile 1.0 instruction fields (split into registers) */
#define CDNS_XSPI_CMD_INSTR_TYPE              GENMASK(6, 0)
#define CDNS_XSPI_CMD_P1_R1_ADDR0             GENMASK(31, 24)
#define CDNS_XSPI_CMD_P1_R2_ADDR1             GENMASK(7, 0)
#define CDNS_XSPI_CMD_P1_R2_ADDR2             GENMASK(15, 8)
#define CDNS_XSPI_CMD_P1_R2_ADDR3             GENMASK(23, 16)
#define CDNS_XSPI_CMD_P1_R2_ADDR4             GENMASK(31, 24)
#define CDNS_XSPI_CMD_P1_R3_ADDR5             GENMASK(7, 0)
#define CDNS_XSPI_CMD_P1_R3_CMD               GENMASK(23, 16)
#define CDNS_XSPI_CMD_P1_R3_NUM_ADDR_BYTES    GENMASK(30, 28)
#define CDNS_XSPI_CMD_P1_R4_ADDR_IOS          GENMASK(1, 0)
#define CDNS_XSPI_CMD_P1_R4_CMD_IOS           GENMASK(9, 8)
#define CDNS_XSPI_CMD_P1_R4_BANK              GENMASK(14, 12)

/* STIG data sequence instruction fields (split into registers) */
#define CDNS_XSPI_CMD_DSEQ_R2_DCNT_L          GENMASK(31, 16)
#define CDNS_XSPI_CMD_DSEQ_R3_DCNT_H          GENMASK(15, 0)
#define CDNS_XSPI_CMD_DSEQ_R3_NUM_OF_DUMMY    GENMASK(25, 20)
#define CDNS_XSPI_CMD_DSEQ_R4_BANK            GENMASK(14, 12)
#define CDNS_XSPI_CMD_DSEQ_R4_DATA_IOS        GENMASK(9, 8)
#define CDNS_XSPI_CMD_DSEQ_R4_DIR             BIT(4)

/* STIG command status fields */
#define CDNS_XSPI_CMD_STATUS_COMPLETED        BIT(15)
#define CDNS_XSPI_CMD_STATUS_FAILED           BIT(14)
#define CDNS_XSPI_CMD_STATUS_DQS_ERROR        BIT(3)
#define CDNS_XSPI_CMD_STATUS_CRC_ERROR        BIT(2)
#define CDNS_XSPI_CMD_STATUS_BUS_ERROR        BIT(1)
#define CDNS_XSPI_CMD_STATUS_INV_SEQ_ERROR    BIT(0)

#define CDNS_XSPI_CTRL_WORK_MODE_AUTO         0x03
#define CDNS_XSPI_CTRL_WORK_MODE_STIG         0x01
#define CDNS_XSPI_CTRL_WORK_MODE_DIRECT       0x00

#define CDNS_XSPI_STIG_DONE_FLAG              BIT(0)

/* DISCOVERY */
#define CDNS_XSPI_CTRL_DISCOVERY_REG          0x260
#define CDNS_XSPI_DISCOVERY_BANK_ID           GENMASK(18, 16)
#define CDNS_XSPI_DISCOVERY_NUM_LINES         GENMASK(15, 12)
#define CDNS_XSPI_DISCOVERY_REQUEST_FLAG      BIT(0)

/* CONFIG REGS */
#define CDNS_XSPI_WRITE_SEQ_CFG_0             0x0420
#define CDNS_XSPI_WRITE_SEQ_CFG_1             0x0424
#define CDNS_XSPI_WRITE_SEQ_CFG_2             0x0428
#define CDNS_XSPI_READ_SEQ_CFG_0              0x0430
#define CDNS_XSPI_READ_SEQ_CFG_1              0x0434
#define CDNS_XSPI_READ_SEQ_CFG_2              0x0438
#define CDNS_XSPI_ERASE_SEQ_CFG_0             0x0410
#define CDNS_XSPI_PROGRAM_SEQ_CFG_0           0x0420

#define CDNS_XSPI_ADDR_CNT_MAP GENMASK(14, 12)
#define CDNS_XSPI_OPCODE_MSK   GENMASK(7,0)
#define ERASE_4B_OPCODE        0x21

/* REMAP CONFIGURATION */
#define CDNS_XSPI_DIRECT_CFG                  0x0398
#define CDNS_XSPI_DIRECT_ENABLE               BIT(12)
#define CDNS_XSPI_DIRECT_BANK                 GENMASK(2, 0)

#define CDNS_XSPI_DIRECT_REMAP_0              0x039c
#define CDNS_XSPI_DIRECT_REMAP_1              0x03a0

/* Helper macros for filling command registers */
#define CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_1(op, data_phase) ( \
	FIELD_PREP(CDNS_XSPI_CMD_INSTR_TYPE, (data_phase) ? \
		CDNS_XSPI_STIG_INSTR_TYPE_1 : CDNS_XSPI_STIG_INSTR_TYPE_0) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R1_ADDR0, op->addr.val & 0xff))

#define CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_2(op) ( \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R2_ADDR1, (op->addr.val >> 8)  & 0xFF) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R2_ADDR2, (op->addr.val >> 16) & 0xFF) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R2_ADDR3, (op->addr.val >> 24) & 0xFF) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R2_ADDR4, (op->addr.val >> 32) & 0xFF))

#define CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_3(op) ( \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R3_ADDR5, (op->addr.val >> 40) & 0xFF) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R3_CMD, op->cmd.opcode) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R3_NUM_ADDR_BYTES, op->addr.nbytes))

#define CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_4(op, chipsel) ( \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R4_ADDR_IOS, ilog2(op->addr.buswidth)) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R4_CMD_IOS, ilog2(op->cmd.buswidth)) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R4_BANK, chipsel))

#define CDNS_XSPI_CMD_FLD_DSEQ_CMD_1(op) \
	FIELD_PREP(CDNS_XSPI_CMD_INSTR_TYPE, CDNS_XSPI_STIG_INSTR_TYPE_DATA_SEQ)

#define CDNS_XSPI_CMD_FLD_DSEQ_CMD_2(op) \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R2_DCNT_L, op->data.nbytes & 0xFFFF)

#define CDNS_XSPI_CMD_FLD_DSEQ_CMD_3(op) ( \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R3_DCNT_H, \
		(op->data.nbytes >> 16) & 0xffff) | \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R3_NUM_OF_DUMMY, op->dummy.nbytes * 8))

#define CDNS_XSPI_CMD_FLD_DSEQ_CMD_4(op, chipsel) ( \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R4_BANK, chipsel) | \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R4_DATA_IOS, ilog2(op->data.buswidth)) | \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R4_DIR, \
		(op->data.dir == SPI_MEM_DATA_IN) ? \
		CDNS_XSPI_STIG_CMD_DIR_READ : CDNS_XSPI_STIG_CMD_DIR_WRITE))

enum cdns_xspi_stig_instr_type {
	CDNS_XSPI_STIG_INSTR_TYPE_0,
	CDNS_XSPI_STIG_INSTR_TYPE_1,
	CDNS_XSPI_STIG_INSTR_TYPE_DATA_SEQ = 127,
};

enum cdns_xspi_sdma_dir {
	CDNS_XSPI_SDMA_DIR_READ,
	CDNS_XSPI_SDMA_DIR_WRITE,
};

enum cdns_xspi_stig_cmd_dir {
	CDNS_XSPI_STIG_CMD_DIR_READ,
	CDNS_XSPI_STIG_CMD_DIR_WRITE,
};

enum cdns_xspi_mode {
	CDNS_XSPI_MODE_STIG,
	CDNS_XSPI_MODE_DIRECT,
	CDNS_XSPI_MODE_AUTO,
};

struct cdns_xspi_platform_data {
	u32 phy_data_sel_oe_start;
	u32 phy_data_sel_oe_end;
	u32 phy_dqs_sel_oe_start;
	u32 phy_dqs_sel_oe_end;
	u32 phy_gate_cfg_close;
	u32 phy_gate_cfg;
	u32 phy_rd_del_sel;
	u32 clk_wr_delay;
	bool dqs_last_data_drop;
	bool use_lpbk_dqs;
	bool use_ext_lpbk_dqs;
};

struct cdns_xspi_dev {
	struct udevice *dev;

	void __iomem *iobase;
	void __iomem *auxbase;
	void __iomem *directbase;

	int hw_num_banks;
	int current_cs;
	int lane_config[MAX_CS_COUNT];
	int clock_config[MAX_CS_COUNT];

	struct cdns_xspi_platform_data *plat_data;
};

static int cdns_xspi_claim_bus(struct udevice *dev)
{
	return 0;
}

static int cdns_xspi_release_bus(struct udevice *dev)
{
	return 0;
}

static bool cdns_xspi_supports_op(struct spi_slave *slave,
				  const struct spi_mem_op *op)
{
	return true;
}

static int cdns_xspi_set_speed(struct udevice *bus, uint max_hz)
{
	return 0;
}

static int cdns_xspi_set_mode(struct udevice *bus, uint mode)
{
	return 0;
}

static int cdns_xspi_wait_for_controller_idle(struct cdns_xspi_dev *cdns_xspi)
{
	u32 ctrl_stat;

	return readl_relaxed_poll_timeout(cdns_xspi->iobase +
		CDNS_XSPI_CTRL_STATUS_REG,
		ctrl_stat, ((ctrl_stat & CDNS_XSPI_CTRL_BUSY) == 0), 1000);
}

void cdns_xspi_start_discovery(struct cdns_xspi_dev *cdns_xspi)
{
	u32 discovery_ctrl = readl(cdns_xspi->iobase + CDNS_XSPI_CTRL_DISCOVERY_REG);
	u32 max_lane_cfg = cdns_xspi->lane_config[cdns_xspi->current_cs];
	u32 ctrl_stat;
	int tmp;

	discovery_ctrl |= CDNS_XSPI_DISCOVERY_REQUEST_FLAG;
	discovery_ctrl &= ~CDNS_XSPI_DISCOVERY_BANK_ID;
	discovery_ctrl &= ~CDNS_XSPI_DISCOVERY_NUM_LINES;
	discovery_ctrl |= FIELD_PREP(CDNS_XSPI_DISCOVERY_NUM_LINES, max_lane_cfg);
	discovery_ctrl |= FIELD_PREP(CDNS_XSPI_DISCOVERY_BANK_ID, cdns_xspi->current_cs);

	writel(discovery_ctrl,
	       cdns_xspi->iobase + CDNS_XSPI_CTRL_DISCOVERY_REG);
	tmp = readl_relaxed_poll_timeout
		  (cdns_xspi->iobase + CDNS_XSPI_CTRL_STATUS_REG,
		  ctrl_stat,
		  ((ctrl_stat & CDNS_XSPI_DISCOVERY_BUSY) == 0),
		  1000);
	if (tmp)
		dev_err(cdns_xspi->dev, "Discovery failure\n");
}

static void cdns_xspi_set_direct_bank(struct cdns_xspi_dev *cdns_xspi)
{
	u32 direct_cfg = readl(cdns_xspi->iobase + CDNS_XSPI_DIRECT_CFG);

	direct_cfg &= ~CDNS_XSPI_DIRECT_BANK;
	direct_cfg |= FIELD_PREP(CDNS_XSPI_DIRECT_BANK, cdns_xspi->current_cs);

	writel(direct_cfg, cdns_xspi->iobase + CDNS_XSPI_DIRECT_CFG);
}

static void cdns_xspi_set_work_mode(struct cdns_xspi_dev *cdns_xspi, enum cdns_xspi_mode mode)
{
	/* Wait for controller idle */
	u32 ctrl_reg;

	cdns_xspi_wait_for_controller_idle(cdns_xspi);
	log_debug("Changing mode to: ");
	switch (mode) {
	case CDNS_XSPI_MODE_STIG:
		log_debug("STIG\n");
		ctrl_reg = FIELD_PREP(CDNS_XSPI_CTRL_WORK_MODE,
				      CDNS_XSPI_CTRL_WORK_MODE_STIG);
		break;
	case CDNS_XSPI_MODE_DIRECT:
		log_debug("DIRECT\n");
		ctrl_reg = FIELD_PREP(CDNS_XSPI_CTRL_WORK_MODE,
				      CDNS_XSPI_CTRL_WORK_MODE_DIRECT);
		break;
	case CDNS_XSPI_MODE_AUTO:
		log_debug("AUTO\n");
		ctrl_reg = FIELD_PREP(CDNS_XSPI_CTRL_WORK_MODE,
				      CDNS_XSPI_CTRL_WORK_MODE_AUTO);
		break;
	}
	writel(ctrl_reg, cdns_xspi->iobase + CDNS_XSPI_CTRL_CONFIG_REG);
}

static void cdns_xspi_trigger_command(struct cdns_xspi_dev *cdns_xspi, u32 cmd_regs[5])
{
	int i, off;

	for (i = 0; i < 6; i++) {
		if (i == 0) {
			log_debug("32bit: start=%X\n", cmd_regs[i]);
		} else {
			off = i - 1;
			log_debug("32bit: regs(%d)(%03d-%03d)=%X\n",
				  off, off * 32, ((off + 1) * 32) - 1,
				  cmd_regs[i]);
		}
	}

	writel(cmd_regs[5], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_5);
	writel(cmd_regs[4], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_4);
	writel(cmd_regs[3], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_3);
	writel(cmd_regs[2], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_2);
	writel(cmd_regs[1], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_1);
	writel(cmd_regs[0], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_0);
}

void print_status(struct cdns_xspi_dev *cdns_xspi)
{
	log_debug("Status: CMD: %X, CTRL : %X, INTR: %X\n",
		  readl(cdns_xspi->iobase + CDNS_XSPI_CMD_STATUS_REG),
		  readl(cdns_xspi->iobase + CDNS_XSPI_CTRL_STATUS_REG),
		  readl(cdns_xspi->iobase + CDNS_XSPI_INTR_STATUS_REG));
}

static int cdns_xspi_send_stig_command(struct cdns_xspi_dev *cdns_xspi, const struct spi_mem_op *op)
{
	u32 cmd_regs[6] = {0};
	bool data_phase = op->data.dir == SPI_MEM_NO_DATA ? false : true;

	cdns_xspi_set_work_mode(cdns_xspi, CDNS_XSPI_MODE_STIG);
	cdns_xspi_wait_for_controller_idle(cdns_xspi);

	cmd_regs[0] = 0x00;
	cmd_regs[1] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_1(op, data_phase);
	cmd_regs[2] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_2(op);
	cmd_regs[3] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_3(op);
	cmd_regs[4] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_4(op, cdns_xspi->current_cs);

	log_debug("Send command phase\n");
	cdns_xspi_trigger_command(cdns_xspi, cmd_regs);

	if (data_phase) {
		cmd_regs[0] = CDNS_XSPI_STIG_DONE_FLAG;
		cmd_regs[1] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_1(op) | (1 << 24);
		cmd_regs[2] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_2(op);
		cmd_regs[3] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_3(op);
		cmd_regs[4] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_4(op, cdns_xspi->current_cs);

		log_debug("Send data phase\n");
		cdns_xspi_trigger_command(cdns_xspi, cmd_regs);
	}
	/* Wait for command completion instead of udelay*/
	mdelay(10);
	print_status(cdns_xspi);

	return 0;
}

bool cdns_xspi_direct_ready(struct cdns_xspi_dev *cdns_xspi)
{
	u32 ctrl_stat;
	int tmp = readl_relaxed_poll_timeout
		  (cdns_xspi->iobase + CDNS_XSPI_CTRL_STATUS_REG,
		  ctrl_stat,
		  ((ctrl_stat & BIT(3)) == 0),
		  1000);

	if (tmp != 0) {
		dev_err(cdns_xspi->dev, "SPI not ready fail\n");
		return false;
	}
	return true;
}

bool cdns_xspi_is_busy(struct cdns_xspi_dev *cdns_xspi)
{
	int tmp = readl(cdns_xspi + CDNS_XSPI_CTRL_STATUS_REG);

	return (tmp & CDNS_XSPI_CTRL_BUSY);
}

bool cdns_xspi_auto_completed(struct cdns_xspi_dev *cdns_xspi)
{
	u32 ctrl_stat;
	int tmp = readl_relaxed_poll_timeout
		  (cdns_xspi->iobase + CDNS_XSPI_CMD_STATUS_REG,
		  ctrl_stat,
		  ((ctrl_stat & CDNS_XSPI_CMD_COMPLETED) != 0),
		  1000);

	if (tmp != 0) {
		dev_err(cdns_xspi->dev, "Auto mode completion fail\n");
		print_status(cdns_xspi);
		return false;
	}
	return true;
}

static bool cdns_xspi_verify_cs(struct cdns_xspi_dev *cdns_xspi)
{
	u32 direct_config_cs = FIELD_GET(CDNS_XSPI_DIRECT_BANK,
					 readl(cdns_xspi->iobase + CDNS_XSPI_DIRECT_CFG));

	if (direct_config_cs == cdns_xspi->current_cs)
		return true;
	else
		return false;
}

static void cdns_xspi_direct_read(struct cdns_xspi_dev *cdns_xspi,
				  u64 spi_addr, void *buf, u64 read_len)
{
	u8 *destination = (u8 *)buf;
	u32 offset, window_read_len;
	u64 window_start, window_end, window_remap_addr;
	u32 total_window_loops;
	void *base_map_addr;

	/*Calculate total number of windows for that read */
	total_window_loops = read_len / DIRECT_SIZE;
	if (read_len % DIRECT_SIZE != 0)
		total_window_loops++;
	log_debug("Requested direct transfer:\n");
	log_debug("Transfer from: %llX, length: %llX\n", spi_addr, read_len);
	log_debug("Transfer to: %p\n", destination);
	log_debug("Total window loop: %d\n", total_window_loops);

	if (spi_addr % MEMORY_ALIGN_TO != 0) {
		dev_err(cdns_xspi->dev, "Error, SPI addr not aligned\n");
		return;
	}

	cdns_xspi_set_work_mode(cdns_xspi, CDNS_XSPI_MODE_DIRECT);
	cdns_xspi_set_direct_bank(cdns_xspi);

	while (total_window_loops) {
	/* Calculate initial window parameter */
		offset = spi_addr % DIRECT_SIZE;
		window_start = spi_addr - offset;
		window_end   = window_start + (DIRECT_SIZE - 1);
		window_read_len =  min((window_end - (offset + window_start) + 1), read_len);
		base_map_addr = cdns_xspi->directbase + offset;

		log_debug("Window transfer info start\n");
		log_debug("Transfer from: %llX, length: %llX\n", spi_addr, read_len);
		log_debug("Window start: %llX, window end: %llX\n", window_start, window_end);
		log_debug("Offset: %X\n", offset);
		log_debug("Current window read length: %X\n", window_read_len);
		log_debug("Total window loop left: %d\n", total_window_loops);

		/*Set remapping regisers if necessary*/
		if (window_start) {
			window_remap_addr = 0 - window_start;
			log_debug("New remapping: %llX\n", window_remap_addr);
			/*Enable remap */
			writel(CDNS_XSPI_DIRECT_ENABLE,
			       cdns_xspi->iobase + CDNS_XSPI_DIRECT_CFG);
			writel(window_remap_addr & 0xffffffff,
			       cdns_xspi->iobase + CDNS_XSPI_DIRECT_REMAP_0);
			writel((window_remap_addr >> 32) & 0xffffffff,
			       cdns_xspi->iobase + CDNS_XSPI_DIRECT_REMAP_1);
		}
		cdns_xspi_direct_ready(cdns_xspi);

		memcpy(destination, base_map_addr, window_read_len);

		spi_addr += window_read_len;
		read_len -= window_read_len;
		destination += window_read_len;
		total_window_loops -= 1;
	}

	/* cleanup */
	writel(0x00, cdns_xspi->iobase + CDNS_XSPI_DIRECT_CFG);
	writel(0x00, cdns_xspi->iobase + CDNS_XSPI_DIRECT_REMAP_0);
	writel(0x00, cdns_xspi->iobase + CDNS_XSPI_DIRECT_REMAP_1);
}

void cdns_xspi_direct_write(struct cdns_xspi_dev *cdns_xspi, u64 spi_addr, void *buf, u64 read_len)
{
	u8 *destination = (u8 *)buf;
	u32 offset, window_read_len;
	u64 window_start, window_end, window_remap_addr;
	u32 total_window_loops;
	void *base_map_addr;

	total_window_loops = read_len / DIRECT_SIZE;
	if (read_len % DIRECT_SIZE != 0)
		total_window_loops++;
	log_debug("Requested direct transfer:\n");
	log_debug("Transfer from: %llX, length: %llX\n", spi_addr, read_len);
	log_debug("Transfer to: %p\n", destination);
	log_debug("Total window loop: %d\n", total_window_loops);

	if (spi_addr % MEMORY_ALIGN_TO != 0) {
		dev_err(cdns_xspi->dev, "Error, SPI addr not aligned\n");
		return;
	}

	cdns_xspi_set_work_mode(cdns_xspi, CDNS_XSPI_MODE_DIRECT);
	cdns_xspi_set_direct_bank(cdns_xspi);

	while (total_window_loops) {
	/* Calculate initial window parameter */
		offset = spi_addr % DIRECT_SIZE;
		window_start = spi_addr - offset;
		window_end   = window_start + (DIRECT_SIZE - 1);
		window_read_len =  min((window_end - (offset + window_start) + 1), read_len);
		base_map_addr = cdns_xspi->directbase + offset;

		log_debug("Window transfer info start\n");
		log_debug("Transfer from: %llX, length: %llX\n", spi_addr, read_len);
		log_debug("Window start: %llX, window end: %llX\n", window_start, window_end);
		log_debug("Offset: %X\n", offset);
		log_debug("Current window read length: %X\n", window_read_len);
		log_debug("Total window loop left: %d\n", total_window_loops);

		/*Set remapping registers if necessary*/
		if (window_start) {
			window_remap_addr = 0 - window_start;
			log_debug("New remapping: %llX\n", window_remap_addr);
			/*Enable remap */
			writel(CDNS_XSPI_DIRECT_ENABLE,
			       cdns_xspi->iobase + CDNS_XSPI_DIRECT_CFG);
			writel(window_remap_addr & 0xffffffff,
			       cdns_xspi->iobase + CDNS_XSPI_DIRECT_REMAP_0);
			writel((window_remap_addr >> 32) & 0xffffffff,
			       cdns_xspi->iobase + CDNS_XSPI_DIRECT_REMAP_1);
		}

		cdns_xspi_direct_ready(cdns_xspi);

		memcpy(base_map_addr, destination, window_read_len);

		spi_addr += window_read_len;
		read_len -= window_read_len;
		destination += window_read_len;
		total_window_loops -= 1;
	}

	/* cleanup */
	writel(0x00, cdns_xspi->iobase + CDNS_XSPI_DIRECT_CFG);
	writel(0x00, cdns_xspi->iobase + CDNS_XSPI_DIRECT_REMAP_0);
	writel(0x00, cdns_xspi->iobase + CDNS_XSPI_DIRECT_REMAP_1);
}

#define CDNS_READ_DEFAULT_CFG_1 0xFC00
#define CDNS_READ_DEFAULT_CFG_2 0x0F0A
void cdns_auto_command(struct cdns_xspi_dev *cdns_xspi, const struct spi_mem_op *op)
{
	u32 read_config_backup[3];
	u32 write_config_backup[3];
	u32 new_config;
	u32 control_regs[6] = {0};
	dma_addr_t buf_dma = 0x00;

	if (op->data.buf.in) {
		if (op->data.dir == SPI_MEM_DATA_IN)
			buf_dma = dma_map_single(op->data.buf.in, op->data.nbytes, DMA_FROM_DEVICE);
		else if (op->data.dir == SPI_MEM_DATA_OUT)
			buf_dma = dma_map_single(op->data.buf.in, op->data.nbytes, DMA_TO_DEVICE);
		else
			buf_dma = 0x00;
	}
	cdns_xspi_set_work_mode(cdns_xspi, CDNS_XSPI_MODE_AUTO);

	for (int i = 0; i < 3; i++) {
		read_config_backup[i] = readl(cdns_xspi->iobase +
						CDNS_XSPI_READ_SEQ_CFG_0 + i * 4);
		write_config_backup[i] = readl(cdns_xspi->iobase +
						CDNS_XSPI_WRITE_SEQ_CFG_0 + i * 4);
	}

	if (op->data.dir == SPI_MEM_DATA_IN) {
		new_config |= op->cmd.opcode;
		new_config |= FIELD_PREP(GENMASK(14, 12), op->addr.nbytes);
		writel(new_config, cdns_xspi->iobase + CDNS_XSPI_READ_SEQ_CFG_0);
		writel(CDNS_READ_DEFAULT_CFG_1, cdns_xspi->iobase + CDNS_XSPI_READ_SEQ_CFG_1);
		writel(CDNS_READ_DEFAULT_CFG_2, cdns_xspi->iobase + CDNS_XSPI_READ_SEQ_CFG_2);
	} else {
		new_config |= op->cmd.opcode;
		new_config |= FIELD_PREP(GENMASK(14, 12), op->addr.nbytes);
		writel(new_config, cdns_xspi->iobase + CDNS_XSPI_WRITE_SEQ_CFG_0);
	}

	if (op->data.dir == SPI_MEM_DATA_IN)
		control_regs[0] = FIELD_PREP(GENMASK(31, 30), 0x01) |
						  BIT(19) | FIELD_PREP(GENMASK(15, 0), 0x2200);
	else
		control_regs[0] = FIELD_PREP(GENMASK(31, 30), 0x01) |
						  BIT(19) | FIELD_PREP(GENMASK(15, 0), 0x2100);
	control_regs[0] |= FIELD_PREP(GENMASK(22, 20), cdns_xspi->current_cs);
	control_regs[1] = op->addr.val & 0xffffffff;
	control_regs[2] = buf_dma & 0xffffffff;
	control_regs[3] = (buf_dma >> 32) & 0xffffffff;
	control_regs[4] = op->data.nbytes;
	control_regs[5] = (op->addr.val >> 32) & 0xffffffff;

	/* Start auto transaction */
	writel(control_regs[5], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_5);
	writel(control_regs[4], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_4);
	writel(control_regs[3], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_3);
	writel(control_regs[2], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_2);
	writel(control_regs[1], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_1);
	writel(control_regs[0], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_0);

	cdns_xspi_auto_completed(cdns_xspi);

	for (int i = 0; i < 3; i++) {
		writel(read_config_backup[i],
		       cdns_xspi->iobase + CDNS_XSPI_READ_SEQ_CFG_0 + i * 4);
		writel(write_config_backup[i],
		       cdns_xspi->iobase + CDNS_XSPI_WRITE_SEQ_CFG_0 + i * 4);
	}
}

void cdns_auto_command_erase(struct cdns_xspi_dev *cdns_xspi, const struct spi_mem_op *op)
{
	u32 control_regs[6] = {0};
	u32 erase_cntrl;

	cdns_xspi_set_work_mode(cdns_xspi, CDNS_XSPI_MODE_AUTO);

	control_regs[0] = FIELD_PREP(GENMASK(31, 30), 0x01) | FIELD_PREP(GENMASK(15, 0), 0x1000) |
			  FIELD_PREP(GENMASK(22, 20), cdns_xspi->current_cs);
	control_regs[1] = op->addr.val & 0xffffffff;
	control_regs[4] = op->data.nbytes;
	control_regs[5] = (op->addr.val >> 32) & 0xffffffff;

	erase_cntrl = readl(cdns_xspi->iobase + CDNS_XSPI_ERASE_SEQ_CFG_0);
	erase_cntrl &= ~CDNS_XSPI_ADDR_CNT_MAP;
	erase_cntrl &= ~CDNS_XSPI_OPCODE_MSK;
	erase_cntrl |= FIELD_PREP(CDNS_XSPI_ADDR_CNT_MAP, 4);
	erase_cntrl |= FIELD_PREP(CDNS_XSPI_OPCODE_MSK, ERASE_4B_OPCODE);
	writel(erase_cntrl, cdns_xspi->iobase + CDNS_XSPI_ERASE_SEQ_CFG_0);

	/* Start auto transaction */
	writel(control_regs[5], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_5);
	writel(control_regs[4], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_4);
	writel(control_regs[3], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_3);
	writel(control_regs[2], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_2);
	writel(control_regs[1], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_1);
	writel(control_regs[0], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_0);

	cdns_xspi_auto_completed(cdns_xspi);
}

static int cdns_xspi_exec_op(struct spi_slave *slave,
			     const struct spi_mem_op *op)
{
	int ret = 0;
	struct dm_spi_slave_platdata *slave_dev = dev_get_parent_platdata(slave->dev);
	struct udevice *dev = slave->dev->parent;
	struct cdns_xspi_dev *cdns_xspi = dev_get_priv(dev);

	cdns_xspi->current_cs = slave_dev->cs;
	if (!cdns_xspi_verify_cs(cdns_xspi))
		cdns_xspi_start_discovery(cdns_xspi);

	log_debug("SPI opcode: %X buswidth: %d\n",
		  op->cmd.opcode, op->cmd.buswidth);
	log_debug("SPI addr: nbytes: %X buswidth: %d, val: %llX\n",
		  op->addr.nbytes, op->addr.buswidth, op->addr.val);
	log_debug("SPI data: direction: %X, len: %d, ptr: %p\n",
		  op->data.dir, op->data.nbytes, op->data.buf.in);

	/* Decode commands received by linux SPI */
	switch (op->cmd.opcode) {
	case 0x05:
		*(u8 *)(op->data.buf.in) = 0x00;
		break;
	case 0x70:
		*(u8 *)(op->data.buf.in) = 0x88;
		break;
	case 0x20:
		cdns_auto_command_erase(cdns_xspi, op);
		break;
	case 0x03:
	case 0x0B:
		cdns_xspi_direct_read(cdns_xspi, op->addr.val,
				      op->data.buf.in, op->data.nbytes);
		break;
	case 0x9F:
	case 0x5A:
		cdns_auto_command(cdns_xspi, op);
		break;
	case 0x02:
		cdns_xspi_direct_write(cdns_xspi, op->addr.val,
				       op->data.buf.in, op->data.nbytes);
		break;
	case 0x04:
	case 0x06:
	case 0xB7:
		break;
	default:
		cdns_xspi_send_stig_command(cdns_xspi, op);
		break;
	}
	return ret;
}

static void cdns_xspi_print_phy_config(struct cdns_xspi_dev *cdns_xspi)
{
	log_debug("PHY configuration\n");
	log_debug("   * xspi_dll_phy_ctrl: %08x\n",
		  readl(cdns_xspi->iobase + CDNS_XSPI_DLL_PHY_CTRL));
	log_debug("   * phy_dq_timing: %08x\n",
		  readl(cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DQ_TIMING));
	log_debug("   * phy_dqs_timing: %08x\n",
		  readl(cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DQS_TIMING));
	log_debug("   * phy_gate_loopback_ctrl: %08x\n",
		  readl(cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_GATE_LPBCK_CTRL));
	log_debug("   * phy_dll_slave_ctrl: %08x\n",
		  readl(cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DLL_SLAVE_CTRL));
}

static int cdns_xspi_controller_init(struct cdns_xspi_dev *cdns_xspi)
{
	u32 ctrl_ver;
	u32 ctrl_features;
	u16 hw_magic_num;

	ctrl_ver = readl(cdns_xspi->iobase + CDNS_XSPI_CTRL_VERSION_REG);
	hw_magic_num = FIELD_GET(CDNS_XSPI_MAGIC_NUM, ctrl_ver);
	if (hw_magic_num != CDNS_XSPI_MAGIC_NUM_VALUE) {
		dev_err(cdns_xspi->dev,
			"Incorrect XSPI magic nunber: %x, expected: %x\n",
			hw_magic_num, CDNS_XSPI_MAGIC_NUM_VALUE);
		return -EIO;
	}

	ctrl_features = readl(cdns_xspi->iobase + CDNS_XSPI_CTRL_FEATURES_REG);
	cdns_xspi->hw_num_banks = FIELD_GET(CDNS_XSPI_NUM_BANKS, ctrl_features);

	cdns_xspi_set_work_mode(cdns_xspi, CDNS_XSPI_MODE_DIRECT);
	cdns_xspi->current_cs = 0;
	cdns_xspi_start_discovery(cdns_xspi);
	return 0;
}

static int cdns_xspi_read_dqs_delay_training(struct cdns_xspi_dev *cdns_xspi)
{
	int rd_dqs_del;
	int rd_dqs_del_min = -1;
	int rd_dqs_del_max = -1;

	u32 phy_dll_slave_ctrl = 0;
	u32 ctrl_status = 0;

	phy_dll_slave_ctrl = readl(cdns_xspi->auxbase +
		CDNS_XSPI_CCP_PHY_DLL_SLAVE_CTRL);

	log_debug("Running PHY training for read_dqs_delay parameter\n");

	for (rd_dqs_del = 0; rd_dqs_del < U8_MAX; rd_dqs_del++) {
		phy_dll_slave_ctrl &= ~CDNS_XSPI_CCP_READ_DQS_DELAY;
		phy_dll_slave_ctrl |= FIELD_PREP(CDNS_XSPI_CCP_READ_DQS_DELAY,
			rd_dqs_del);

		writel(phy_dll_slave_ctrl,
		       cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DLL_SLAVE_CTRL);

		readl_relaxed_poll_timeout(cdns_xspi->iobase +
			CDNS_XSPI_CTRL_STATUS_REG, ctrl_status,
			(ctrl_status & CDNS_XSPI_INIT_COMPLETED), 10000);

		if (!(ctrl_status & CDNS_XSPI_INIT_COMPLETED) ||
		    (ctrl_status & CDNS_XSPI_INIT_FAIL)) {
			if (rd_dqs_del_min != -1)
				rd_dqs_del_max = rd_dqs_del - 1;
		} else {
			if (rd_dqs_del_min == -1)
				rd_dqs_del_min = rd_dqs_del;
		}
	}

	if (rd_dqs_del_min == -1) {
		dev_err(cdns_xspi->dev, "PHY training failed\n");
		//on ASIM return 0
		return 0;
	} else if (rd_dqs_del_max == -1) {
		rd_dqs_del_max = U8_MAX;
	}

	rd_dqs_del = rd_dqs_del_min + rd_dqs_del_max / 2;
	log_debug("Using optimal read_dqs_delay value: %d\n", rd_dqs_del);

	phy_dll_slave_ctrl &= ~CDNS_XSPI_CCP_READ_DQS_DELAY;
	phy_dll_slave_ctrl |= FIELD_PREP(CDNS_XSPI_CCP_READ_DQS_DELAY,
		rd_dqs_del);

	writel(phy_dll_slave_ctrl,
	       cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DLL_SLAVE_CTRL);

	return 0;
}

static int cdns_xspi_phy_init(struct cdns_xspi_dev *cdns_xspi)
{
	u32 xspi_dll_phy_ctrl = 0;
	u32 phy_dq_timing = CDNS_XSPI_CCP_PHY_DQ_TIMING_INIT_VAL;
	u32 phy_dqs_timing = 0;
	u32 phy_gate_lpbck_ctrl = 0;
	u32 phy_dll_slave_ctrl = 0;

	if (cdns_xspi->plat_data->use_lpbk_dqs) {
		phy_dqs_timing |= FIELD_PREP(CDNS_XSPI_CCP_USE_LPBCK_DQS, 1);

		/*
		 * For XSPI protocol, phony_dqs and lpbk_dqs must
		 * have same value
		 */
		phy_dqs_timing |= FIELD_PREP(CDNS_XSPI_CCP_USE_PHONY, 1);

		if (cdns_xspi->plat_data->use_ext_lpbk_dqs)
			phy_dqs_timing |=
				FIELD_PREP(CDNS_XSPI_CCP_USE_EXT_LPBCK_DQS, 1);
	}

	/* mapped in iobase!*/
	xspi_dll_phy_ctrl = readl(cdns_xspi->iobase + CDNS_XSPI_DLL_PHY_CTRL);

	/* While using memory DQS last_data_drop parameter should be enabled */
	if (cdns_xspi->plat_data->dqs_last_data_drop)
		xspi_dll_phy_ctrl |=
			FIELD_PREP(CDNS_XSPI_CCP_DQS_LAST_DATA_DROP_EN, 1);

	phy_dq_timing |= FIELD_PREP(CDNS_XSPI_CCP_DATA_SELECT_OE_START,
		cdns_xspi->plat_data->phy_data_sel_oe_start);
	phy_dq_timing |= FIELD_PREP(CDNS_XSPI_CCP_DATA_SELECT_OE_END,
		cdns_xspi->plat_data->phy_data_sel_oe_end);

	phy_dqs_timing |= FIELD_PREP(CDNS_XSPI_CCP_DQS_SELECT_OE_START,
		cdns_xspi->plat_data->phy_dqs_sel_oe_start);
	phy_dqs_timing |= FIELD_PREP(CDNS_XSPI_CCP_DQS_SELECT_OE_END,
		cdns_xspi->plat_data->phy_dqs_sel_oe_end);

	phy_gate_lpbck_ctrl |= FIELD_PREP(CDNS_XSPI_CCP_GATE_CFG_CLOSE,
		cdns_xspi->plat_data->phy_gate_cfg_close);
	phy_gate_lpbck_ctrl |= FIELD_PREP(CDNS_XSPI_CCP_GATE_CFG,
		cdns_xspi->plat_data->phy_gate_cfg);
	phy_gate_lpbck_ctrl |= FIELD_PREP(CDNS_XSPI_CCP_READ_DATA_DELAY_SEL,
		cdns_xspi->plat_data->phy_rd_del_sel);

	phy_dll_slave_ctrl |= FIELD_PREP(CDNS_XSPI_CCP_CLK_WR_DELAY,
		cdns_xspi->plat_data->clk_wr_delay);

	/* mapped in iobase!*/
	writel(xspi_dll_phy_ctrl,
	       cdns_xspi->iobase + CDNS_XSPI_DLL_PHY_CTRL);
	writel(phy_dq_timing,
	       cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DQ_TIMING);
	writel(phy_dqs_timing,
	       cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DQS_TIMING);
	writel(phy_gate_lpbck_ctrl,
	       cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_GATE_LPBCK_CTRL);
	writel(phy_dll_slave_ctrl,
	       cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DLL_SLAVE_CTRL);

	return cdns_xspi_read_dqs_delay_training(cdns_xspi);
}

static int cdns_xspi_of_get_plat_data(struct cdns_xspi_dev *pdev)
{
	struct cdns_xspi_platform_data *plat_data = pdev->plat_data;
	ofnode node_prop = pdev->dev->node;
	ofnode node;
	unsigned int property, property_1;

	if (ofnode_read_u32(node_prop, "cdns,phy-data-select-oe-start", &property)) {
		dev_err(pdev->dev, "Couldn't determine data select oe start\n");
		return -ENXIO;
	}
	plat_data->phy_data_sel_oe_start = property;

	if (ofnode_read_u32(node_prop, "cdns,phy-data-select-oe-end", &property)) {
		dev_err(pdev->dev, "Couldn't determine data select oe end\n");
		return -ENXIO;
	}
	plat_data->phy_data_sel_oe_end = property;

	if (ofnode_read_u32(node_prop, "cdns,phy-dqs-select-oe-start", &property)) {
		dev_err(pdev->dev, "Couldn't determine DQS select oe start\n");
		return -ENXIO;
	}
	plat_data->phy_dqs_sel_oe_start = property;

	if (ofnode_read_u32(node_prop, "cdns,phy-dqs-select-oe-end", &property)) {
		dev_err(pdev->dev, "Couldn't determine DQS select oe end\n");
		return -ENXIO;
	}
	plat_data->phy_dqs_sel_oe_end = property;

	if (ofnode_read_u32(node_prop, "cdns,phy-gate-cfg-close", &property)) {
		dev_err(pdev->dev, "Couldn't determine gate config close\n");
		return -ENXIO;
	}
	plat_data->phy_gate_cfg_close = property;

	if (ofnode_read_u32(node_prop, "cdns,phy-gate-cfg", &property)) {
		dev_err(pdev->dev, "Couldn't determine gate config\n");
		return -ENXIO;
	}
	plat_data->phy_gate_cfg = property;

	if (ofnode_read_u32(node_prop, "cdns,phy-rd-del-select", &property)) {
		dev_err(pdev->dev, "Couldn't determine read delay select\n");
		return -ENXIO;
	}
	plat_data->phy_rd_del_sel = property;

	if (ofnode_read_u32(node_prop, "cdns,phy-clk-wr-delay", &property)) {
		dev_err(pdev->dev, "Couldn't determine clock write delay\n");
		return -ENXIO;
	}
	plat_data->clk_wr_delay = property;

	plat_data->dqs_last_data_drop =  ofnode_read_bool(node_prop, "cdns,dqs-last-data-drop");
	plat_data->use_lpbk_dqs =  ofnode_read_bool(node_prop, "cdns,phy-use-lpbk-dqs");
	plat_data->use_ext_lpbk_dqs =  ofnode_read_bool(node_prop, "cdns,phy-use-ext-lpbk-dqs");

	ofnode_for_each_subnode(node, node_prop) {
		if (ofnode_read_u32(node, "reg", &property)) {
			dev_err(pdev->dev, "Couldn't determine CS value\n");
			return -ENXIO;
		}
		if (ofnode_read_u32(node, "cdns,lane-cnt-limit", &property_1)) {
			dev_err(pdev->dev, "Couldn't determine limiting bus value\n");
			return -ENXIO;
		}
		pdev->lane_config[property] = property_1;
		if (ofnode_read_u32(node, "spi-max-frequency", &property_1)) {
			dev_err(pdev->dev, "Couldn't determine spi max freq\n");
			return -ENXIO;
		}
		pdev->lane_config[property] = property_1;
	}

	return 0;
}

static int cdns_spi_probe(struct udevice *dev)
{
	struct cdns_xspi_dev *priv = dev_get_priv(dev);
	int ret;

	priv->plat_data = (struct cdns_xspi_platform_data *)
			malloc(sizeof(struct cdns_xspi_platform_data));
	if (!priv->plat_data) {
		dev_err(dev, "Failed to allocate memory for spi_master\n");
		return -ENOMEM;
	}
	priv->dev = dev;
	cdns_xspi_of_get_plat_data(priv);

	priv->iobase = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_0,
				      PCI_REGION_MEM);
	priv->auxbase = priv->iobase + CDNS_XSPI_AUX_REGISTER_OFFSET;
	priv->directbase = priv->iobase + CDNS_XSPI_DIRECT_OFFSET;

	ret = cdns_xspi_phy_init(priv);
	if (ret) {
		dev_err(dev, "Failed to initialize PHY\n");
	}
	cdns_xspi_print_phy_config(priv);

	ret = cdns_xspi_controller_init(priv);
	if (ret) {
		dev_err(dev, "Failed to initialize controller\n");
		return ret;
	}

	log_debug("SPI bus %s %d(%p) probed\n", dev->name, dev->seq, priv->iobase);
	return 0;
}

static const struct spi_controller_mem_ops cdns_mem_ops = {
	.supports_op = cdns_xspi_supports_op,
	.exec_op = cdns_xspi_exec_op,
};

static struct dm_spi_ops cdns_spi_ops = {
	.claim_bus	= cdns_xspi_claim_bus,
	.release_bus	= cdns_xspi_release_bus,
	.set_speed	= cdns_xspi_set_speed,
	.set_mode	= cdns_xspi_set_mode,
	.mem_ops	= &cdns_mem_ops,
};

static const struct pci_device_id cadence_octeon_spi_pci_id_table[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_CAVIUM,
			 PCI_DEVICE_ID_CAVIUM_XSPI) },
	{ },
};

U_BOOT_DRIVER(octeon_spi) = {
	.name			= "spi_cadence_octeon",
	.id			= UCLASS_SPI,
	.probe			= cdns_spi_probe,
	.priv_auto_alloc_size	= sizeof(struct cdns_xspi_dev),
	.ops			= &cdns_spi_ops,
};

U_BOOT_PCI_DEVICE(octeon_spi, cadence_octeon_spi_pci_id_table);
