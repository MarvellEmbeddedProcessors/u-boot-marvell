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

#include <asm/arch/board.h>

#define CDNS_XSPI_MAGIC_NUM_VALUE               0x6522
#define CDNS_XSPI_MAX_BANKS                     8
#define CDNS_XSPI_NAME                          "cadence-xspi"
#define CDNS_XSPI_AUX_REGISTER_OFFSET           (0x2000)
#define CDNS_XSPI_DIRECT_OFFSET                 (0x10000000)

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
#define CDNS_XSPI_GCMD_BUSY                   BIT(3)

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

#define CDNS_XSPI_CTRL_WORK_MODE_STIG         0x01

#define CDNS_XSPI_STIG_DONE_FLAG              BIT(0)

/* clock config register */
#define CDNS_XSPI_CLK_CTRL_REG		      0x4020
#define CDNS_XSPI_CLK_ENABLE                  BIT(0)
#define CDNS_XSPI_CLK_DIV                     GENMASK(4, 1)

/* Clock macros */
#define CDNS_XSPI_CLOCK_IO_HZ 800000000
#define CDNS_XSPI_CLOCK_DIVIDED(div) ((CDNS_XSPI_CLOCK_IO_HZ) / (div))

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

	bool sdma_error;
	void *in_buffer;
	const void *out_buffer;

	struct cdns_xspi_platform_data *plat_data;
};

const int cdns_xspi_clk_div_list[] = {
	4,	//0x0 = Divide by 4.   SPI clock is 200 MHz.
	6,	//0x1 = Divide by 6.   SPI clock is 133.33 MHz.
	8,	//0x2 = Divide by 8.   SPI clock is 100 MHz.
	10,	//0x3 = Divide by 10.  SPI clock is 80 MHz.
	12,	//0x4 = Divide by 12.  SPI clock is 66.666 MHz.
	16,	//0x5 = Divide by 16.  SPI clock is 50 MHz.
	18,	//0x6 = Divide by 18.  SPI clock is 44.44 MHz.
	20,	//0x7 = Divide by 20.  SPI clock is 40 MHz.
	24,	//0x8 = Divide by 24.  SPI clock is 33.33 MHz.
	32,	//0x9 = Divide by 32.  SPI clock is 25 MHz.
	40,	//0xA = Divide by 40.  SPI clock is 20 MHz.
	50,	//0xB = Divide by 50.  SPI clock is 16 MHz.
	64,	//0xC = Divide by 64.  SPI clock is 12.5 MHz.
	128,	//0xD = Divide by 128. SPI clock is 6.25 MHz.
	-1	//End of list
};

static int cdns_xspi_controller_init(struct cdns_xspi_dev *cdns_xspi);
int board_acquire_flash_arb(bool acquire);

// Find max avalible clocl
static bool cdns_xspi_setup_clock(struct cdns_xspi_dev *cdns_xspi, int requested_clk)
{
	int i = 0;
	int clk_val;
	u32 clk_reg;
	bool update_clk;

	while (cdns_xspi_clk_div_list[i] > 0) {
		clk_val = CDNS_XSPI_CLOCK_DIVIDED(cdns_xspi_clk_div_list[i]);
		if (clk_val <= requested_clk)
			break;
		i++;
	}

	if (cdns_xspi_clk_div_list[i] == -1) {
		printf("Unable to find clock divider for CLK: %d - setting 6.25MHz\n",
		       requested_clk);
		i = 0x0D;
	} else {
		log_debug("Found clk div: %d, clk val: %d\n", cdns_xspi_clk_div_list[i],
			  CDNS_XSPI_CLOCK_DIVIDED(cdns_xspi_clk_div_list[i]));
	}

	clk_reg = readl(cdns_xspi->iobase + CDNS_XSPI_CLK_CTRL_REG);

	if (FIELD_GET(CDNS_XSPI_CLK_DIV, clk_reg) != i) {
		clk_reg = FIELD_PREP(CDNS_XSPI_CLK_DIV, i);
		clk_reg |= CDNS_XSPI_CLK_ENABLE;
		update_clk = true;
	}

	if (update_clk)
		writel(clk_reg, cdns_xspi->iobase + CDNS_XSPI_CLK_CTRL_REG);

	return update_clk;
}

inline static void copy_mem(uint8_t *dst, uint8_t *src, int len) {
	int i;

	for (i=0; i<len; i++) {
		dst[i] = src[i];
	}
}

static void ioread8_rep(void __iomem  *addr, uint8_t *buf, int len)
{
	int i = 0;
	int rcount = len / 8;
	int rcount_nf = len % 8;
	uint64_t tmp;
	uint8_t ptr;

	for (i = 0; i < rcount; i++) {
		tmp = readq(addr);
		copy_mem(&buf[i*8], &tmp, 8);
	}

	if (rcount_nf != 0) {
		tmp = readq(addr);
		copy_mem(&buf[i*8], &tmp, rcount_nf);
	}
}

static void iowrite8_rep(void __iomem *addr, const uint8_t *buf, int len)
{
	int i = 0;
	int rcount = len / 8;
	int rcount_nf = len % 8;
	uint64_t tmp;

	for (i = 0; i < rcount; i++) {
		copy_mem(&tmp, &buf[i*8], 8);
		writeq(tmp, addr);
	}

	if (rcount_nf != 0) {
		copy_mem(&tmp, &buf[i*8], rcount_nf);
		writeq(tmp, addr);
	}
}

static int cdns_xspi_claim_bus(struct udevice *dev)
{
	if ((IS_ENABLED(CONFIG_ARCH_CN10K))) {
		if (board_acquire_flash_arb(true))
			board_acquire_flash_arb(false);
	}

	return 0;
}

static int cdns_xspi_release_bus(struct udevice *dev)
{
	if ((IS_ENABLED(CONFIG_ARCH_CN10K)))
		board_acquire_flash_arb(false);

	return 0;
}

static bool cdns_xspi_supports_op(struct spi_slave *slave,
				  const struct spi_mem_op *op)
{
	return true;
}

static int cdns_xspi_set_speed(struct udevice *bus, uint max_hz)
{
	struct cdns_xspi_dev *cdns_xspi = dev_get_priv(bus);

	cdns_xspi_setup_clock(cdns_xspi, max_hz);
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

static int cdns_xspi_stig_ready(struct cdns_xspi_dev *cdns_xspi)
{
	u32 ctrl_stat;

	if (!otx_is_platform(PLATFORM_ASIM))
		return readl_relaxed_poll_timeout(cdns_xspi->iobase +
			CDNS_XSPI_CTRL_STATUS_REG,
			ctrl_stat, ((ctrl_stat & CDNS_XSPI_GCMD_BUSY) == 0), 1000);
	else
		return 0;
}

static int cdns_verify_stig_mode_config(struct cdns_xspi_dev *cdns_xspi)
{
	int cntrl = readl(cdns_xspi->iobase + CDNS_XSPI_CTRL_CONFIG_REG);

	if (FIELD_GET(CDNS_XSPI_CTRL_WORK_MODE, cntrl) != CDNS_XSPI_CTRL_WORK_MODE_STIG)
		return cdns_xspi_controller_init(cdns_xspi);

	return 0;
}

static void cdns_xspi_trigger_command(struct cdns_xspi_dev *cdns_xspi,
				      u32 cmd_regs[6])
{
	writel(cmd_regs[5], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_5);
	writel(cmd_regs[4], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_4);
	writel(cmd_regs[3], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_3);
	writel(cmd_regs[2], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_2);
	writel(cmd_regs[1], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_1);
	writel(cmd_regs[0], cdns_xspi->iobase + CDNS_XSPI_CMD_REG_0);
}

static void cdns_xspi_sdma_handle(struct cdns_xspi_dev *cdns_xspi)
{
	u32 sdma_size, sdma_trd_info;
	u32 sdma_status;
	u8 sdma_dir, sdma_ready = 0;

	//Wait for SDMA - skip on asimxx
	if (!otx_is_platform(PLATFORM_ASIM)) {
		do {
			sdma_status = readl(cdns_xspi->iobase + CDNS_XSPI_INTR_STATUS_REG);
			sdma_ready = FIELD_GET(CDNS_XSPI_SDMA_TRIGGER, sdma_status);
		}while(!sdma_ready);
	}

	sdma_size = readl(cdns_xspi->iobase + CDNS_XSPI_SDMA_SIZE_REG);
	sdma_trd_info = readl(cdns_xspi->iobase + CDNS_XSPI_SDMA_TRD_INFO_REG);
	sdma_dir = FIELD_GET(CDNS_XSPI_SDMA_DIR, sdma_trd_info);

	//Clear SDMA bit
	sdma_status = readl(cdns_xspi->iobase + CDNS_XSPI_INTR_STATUS_REG);
	sdma_status |= (CDNS_XSPI_SDMA_TRIGGER);
	writel(sdma_status, cdns_xspi->iobase + CDNS_XSPI_INTR_STATUS_REG);

	switch (sdma_dir) {
	case CDNS_XSPI_SDMA_DIR_READ:
		ioread8_rep(cdns_xspi->directbase,
			    cdns_xspi->in_buffer, sdma_size);
		break;

	case CDNS_XSPI_SDMA_DIR_WRITE:
		iowrite8_rep(cdns_xspi->directbase,
			     cdns_xspi->out_buffer, sdma_size);
		break;
	}
}

static int cdns_xspi_send_stig_command(struct cdns_xspi_dev *cdns_xspi,
				       const struct spi_mem_op *op, bool data_phase)
{
	u32 cmd_regs[5] = {0};

	cdns_xspi_wait_for_controller_idle(cdns_xspi);
	if (cdns_verify_stig_mode_config(cdns_xspi)) {
		printf("Failed to configure xSPI");
		return -1;
	}

	cdns_xspi->sdma_error = false;

	cmd_regs[1] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_1(op, data_phase);
	cmd_regs[2] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_2(op);
	cmd_regs[3] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_3(op);
	cmd_regs[4] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_4(op,
						       cdns_xspi->current_cs);

	cdns_xspi_trigger_command(cdns_xspi, cmd_regs);

	if (data_phase) {
		cmd_regs[0] = CDNS_XSPI_STIG_DONE_FLAG;
		cmd_regs[1] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_1(op);
		cmd_regs[2] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_2(op);
		cmd_regs[3] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_3(op);
		cmd_regs[4] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_4(op,
							   cdns_xspi->current_cs);

		cdns_xspi->in_buffer = op->data.buf.in;
		cdns_xspi->out_buffer = op->data.buf.out;

		cdns_xspi_trigger_command(cdns_xspi, cmd_regs);
		cdns_xspi_sdma_handle(cdns_xspi);
	}
	cdns_xspi_stig_ready(cdns_xspi);

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

static int cdns_xspi_exec_op(struct spi_slave *slave,
			     const struct spi_mem_op *op)
{
	int ret = 0;
	struct dm_spi_slave_platdata *slave_dev = dev_get_parent_platdata(slave->dev);
	struct udevice *dev = slave->dev->parent;
	struct cdns_xspi_dev *cdns_xspi = dev_get_priv(dev);
	bool data_phase = (op->data.dir != SPI_MEM_NO_DATA);

	/* Uboot is generating Read enable CMD with direction out and data
	 *  nbytes = 0
	 *  In that case instruction glueing must be disabled
	 */
	if (op->data.dir == SPI_MEM_DATA_OUT && op->data.nbytes == 0)
		data_phase = false;

	cdns_xspi->current_cs = slave_dev->cs;

	log_debug("SPI opcode: %X buswidth: %d\n",
		  op->cmd.opcode, op->cmd.buswidth);
	log_debug("SPI addr: nbytes: %X buswidth: %d, val: %llX\n",
		  op->addr.nbytes, op->addr.buswidth, op->addr.val);
	log_debug("SPI data: direction: %X, len: %d, ptr: %p\n",
		  op->data.dir, op->data.nbytes, op->data.buf.in);
	if (data_phase)
		log_debug("Data Phase enabled\n");
	else
		log_debug("Data Phase disabled\n");
	log_debug("--------------------------------------------------\n\n");

	cdns_xspi_send_stig_command(cdns_xspi, op, data_phase);

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

	writel(FIELD_PREP(CDNS_XSPI_CTRL_WORK_MODE, CDNS_XSPI_CTRL_WORK_MODE_STIG),
	       cdns_xspi->iobase + CDNS_XSPI_CTRL_CONFIG_REG);

	ctrl_features = readl(cdns_xspi->iobase + CDNS_XSPI_CTRL_FEATURES_REG);
	cdns_xspi->hw_num_banks = FIELD_GET(CDNS_XSPI_NUM_BANKS, ctrl_features);
	cdns_xspi->current_cs = 0;
	return 0;
}

static void cdns_xspi_controller_reset(struct cdns_xspi_dev *cdns_xspi)
{
	u32 driving_reg = 0;

	driving_reg = readl(cdns_xspi->auxbase + CDNS_XSPI_AUX_DRIVING_REG);
	driving_reg |= CDNS_XSPI_AUX_CTRL_RESET;
	writel(driving_reg, cdns_xspi->auxbase + CDNS_XSPI_AUX_DRIVING_REG);

	udelay(10);

	driving_reg &= ~CDNS_XSPI_AUX_CTRL_RESET;
	writel(driving_reg, cdns_xspi->auxbase + CDNS_XSPI_AUX_DRIVING_REG);
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

		cdns_xspi_controller_reset(cdns_xspi);

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

int cdns_xspi_phy_init(struct cdns_xspi_dev *cdns_xspi)
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

	xspi_dll_phy_ctrl = readl(cdns_xspi->auxbase + CDNS_XSPI_DLL_PHY_CTRL);

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

	writel(xspi_dll_phy_ctrl,
	       cdns_xspi->auxbase + CDNS_XSPI_DLL_PHY_CTRL);

	writel(phy_dq_timing,
	       cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DQ_TIMING);

	writel(phy_dqs_timing,
	       cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DQS_TIMING);

	writel(phy_gate_lpbck_ctrl,
	       cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_GATE_LPBCK_CTRL);

	writel(phy_dll_slave_ctrl,
	       cdns_xspi->auxbase + CDNS_XSPI_CCP_PHY_DLL_SLAVE_CTRL);

	writel(CDNS_XSPI_AUX_PHY_ADDONS_VALUE,
	       cdns_xspi->auxbase + CDNS_XSPI_AUX_PHY_ADDONS_REG);

	writel(CDNS_XSPI_AUX_DEV_DISC_CONFIG_VALUE,
	       cdns_xspi->auxbase + CDNS_XSPI_AUX_DEV_DISC_CONFIG_REG);

	return cdns_xspi_read_dqs_delay_training(cdns_xspi);
}

static int cdns_xspi_of_get_plat_data(struct cdns_xspi_dev *pdev)
{
	struct cdns_xspi_platform_data *plat_data = pdev->plat_data;
	ofnode node_prop = pdev->dev->node;
	ofnode node;
	unsigned int property;

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

	if (!otx_is_platform(PLATFORM_ASIM)) {
		ret = cdns_xspi_phy_init(priv);
		if (ret)
			dev_err(dev, "Failed to initialize PHY\n");
		cdns_xspi_print_phy_config(priv);
	}

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
