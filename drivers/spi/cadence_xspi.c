// SPDX-License-Identifier: GPL-2.0+
// Cadence XSPI flash controller driver
// Copyright (C) 2020-21 Cadence

#include <dm.h>
#include <spi.h>
#include <spi-mem.h>
#include <dm/device_compat.h>
#include <asm/io.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/sizes.h>
#include <linux/bitfield.h>
#include <linux/log2.h>
#include <asm/arch/board.h>
#include <linux/iopoll.h>

#define CDNS_XSPI_MAGIC_NUM_VALUE	0x6522
#define CDNS_XSPI_MAX_BANKS		8
#define CDNS_XSPI_NAME			"cadence-xspi"

/*
 * Note: below are additional auxiliary registers to
 * configure XSPI controller pin-strap settings
 */

/* PHY DQ timing register */
#define CDNS_XSPI_CCP_PHY_DQ_TIMING		0x0000

/* PHY DQS timing register */
#define CDNS_XSPI_CCP_PHY_DQS_TIMING		0x0004

/* PHY gate loopback control register */
#define CDNS_XSPI_CCP_PHY_GATE_LPBCK_CTRL	0x0008

/* PHY DLL slave control register */
#define CDNS_XSPI_CCP_PHY_DLL_SLAVE_CTRL	0x0010

/* DLL PHY control register */
#define CDNS_XSPI_DLL_PHY_CTRL			0x1034

/* Command registers */
#define CDNS_XSPI_CMD_REG_0			0x0000
#define CDNS_XSPI_CMD_REG_1			0x0004
#define CDNS_XSPI_CMD_REG_2			0x0008
#define CDNS_XSPI_CMD_REG_3			0x000C
#define CDNS_XSPI_CMD_REG_4			0x0010
#define CDNS_XSPI_CMD_REG_5			0x0014

/* Command status registers */
#define CDNS_XSPI_CMD_STATUS_REG		0x0044

/* Controller status register */
#define CDNS_XSPI_CTRL_STATUS_REG		0x0100
#define CDNS_XSPI_INIT_COMPLETED		BIT(16)
#define CDNS_XSPI_INIT_LEGACY			BIT(9)
#define CDNS_XSPI_INIT_FAIL			BIT(8)
#define CDNS_XSPI_CTRL_BUSY			BIT(7)
#define CDNS_XSPI_GCMD_BUSY                     BIT(3)

/* Controller interrupt status register */
#define CDNS_XSPI_INTR_STATUS_REG		0x0110
#define CDNS_XSPI_STIG_DONE			BIT(23)
#define CDNS_XSPI_SDMA_ERROR			BIT(22)
#define CDNS_XSPI_SDMA_TRIGGER			BIT(21)
#define CDNS_XSPI_CMD_IGNRD_EN			BIT(20)
#define CDNS_XSPI_DDMA_TERR_EN			BIT(18)
#define CDNS_XSPI_CDMA_TREE_EN			BIT(17)
#define CDNS_XSPI_CTRL_IDLE_EN			BIT(16)

#define CDNS_XSPI_TRD_COMP_INTR_STATUS		0x0120
#define CDNS_XSPI_TRD_ERR_INTR_STATUS		0x0130
#define CDNS_XSPI_TRD_ERR_INTR_EN		0x0134

/* Controller interrupt enable register */
#define CDNS_XSPI_INTR_ENABLE_REG		0x0114
#define CDNS_XSPI_INTR_EN			BIT(31)
#define CDNS_XSPI_STIG_DONE_EN			BIT(23)
#define CDNS_XSPI_SDMA_ERROR_EN			BIT(22)
#define CDNS_XSPI_SDMA_TRIGGER_EN		BIT(21)

#define CDNS_XSPI_INTR_MASK (CDNS_XSPI_INTR_EN | \
	CDNS_XSPI_STIG_DONE_EN  | \
	CDNS_XSPI_SDMA_ERROR_EN | \
	CDNS_XSPI_SDMA_TRIGGER_EN)

/* Controller config register */
#define CDNS_XSPI_CTRL_CONFIG_REG		0x0230
#define CDNS_XSPI_CTRL_WORK_MODE		GENMASK(6, 5)

#define CDNS_XSPI_WORK_MODE_DIRECT		0
#define CDNS_XSPI_WORK_MODE_STIG		1
#define CDNS_XSPI_WORK_MODE_ACMD		3

/* SDMA trigger transaction registers */
#define CDNS_XSPI_SDMA_SIZE_REG			0x0240
#define CDNS_XSPI_SDMA_TRD_INFO_REG		0x0244
#define CDNS_XSPI_SDMA_DIR			BIT(8)

/* Controller features register */
#define CDNS_XSPI_CTRL_FEATURES_REG		0x0F04
#define CDNS_XSPI_NUM_BANKS			GENMASK(25, 24)
#define CDNS_XSPI_DMA_DATA_WIDTH		BIT(21)
#define CDNS_XSPI_NUM_THREADS			GENMASK(3, 0)

/* Controller version register */
#define CDNS_XSPI_CTRL_VERSION_REG		0x0F00
#define CDNS_XSPI_MAGIC_NUM			GENMASK(31, 16)
#define CDNS_XSPI_CTRL_REV			GENMASK(7, 0)

/* STIG Profile 1.0 instruction fields (split into registers) */
#define CDNS_XSPI_CMD_INSTR_TYPE		GENMASK(6, 0)
#define CDNS_XSPI_CMD_P1_R1_ADDR0		GENMASK(31, 24)
#define CDNS_XSPI_CMD_P1_R2_ADDR1		GENMASK(7, 0)
#define CDNS_XSPI_CMD_P1_R2_ADDR2		GENMASK(15, 8)
#define CDNS_XSPI_CMD_P1_R2_ADDR3		GENMASK(23, 16)
#define CDNS_XSPI_CMD_P1_R2_ADDR4		GENMASK(31, 24)
#define CDNS_XSPI_CMD_P1_R3_ADDR5		GENMASK(7, 0)
#define CDNS_XSPI_CMD_P1_R3_CMD			GENMASK(23, 16)
#define CDNS_XSPI_CMD_P1_R3_NUM_ADDR_BYTES	GENMASK(30, 28)
#define CDNS_XSPI_CMD_P1_R4_ADDR_IOS		GENMASK(1, 0)
#define CDNS_XSPI_CMD_P1_R4_CMD_IOS		GENMASK(9, 8)
#define CDNS_XSPI_CMD_P1_R4_BANK		GENMASK(14, 12)

/* STIG data sequence instruction fields (split into registers) */
#define CDNS_XSPI_CMD_DSEQ_R2_DCNT_L		GENMASK(31, 16)
#define CDNS_XSPI_CMD_DSEQ_R3_DCNT_H		GENMASK(15, 0)
#define CDNS_XSPI_CMD_DSEQ_R3_NUM_OF_DUMMY	GENMASK(25, 20)
#define CDNS_XSPI_CMD_DSEQ_R4_BANK		GENMASK(14, 12)
#define CDNS_XSPI_CMD_DSEQ_R4_DATA_IOS		GENMASK(9, 8)
#define CDNS_XSPI_CMD_DSEQ_R4_DIR		BIT(4)

/* STIG command status fields */
#define CDNS_XSPI_CMD_STATUS_COMPLETED		BIT(15)
#define CDNS_XSPI_CMD_STATUS_FAILED		BIT(14)
#define CDNS_XSPI_CMD_STATUS_DQS_ERROR		BIT(3)
#define CDNS_XSPI_CMD_STATUS_CRC_ERROR		BIT(2)
#define CDNS_XSPI_CMD_STATUS_BUS_ERROR		BIT(1)
#define CDNS_XSPI_CMD_STATUS_INV_SEQ_ERROR	BIT(0)

#define CDNS_XSPI_STIG_DONE_FLAG		BIT(0)
#define CDNS_XSPI_TRD_STATUS			0x0104

/* Helper macros for filling command registers */
#define CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_1(op, data_phase) ( \
	FIELD_PREP(CDNS_XSPI_CMD_INSTR_TYPE, (data_phase) ? \
		CDNS_XSPI_STIG_INSTR_TYPE_1 : CDNS_XSPI_STIG_INSTR_TYPE_0) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R1_ADDR0, (op)->addr.val & 0xff))

#define CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_2(op) ( \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R2_ADDR1, ((op)->addr.val >> 8)  & 0xFF) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R2_ADDR2, ((op)->addr.val >> 16) & 0xFF) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R2_ADDR3, ((op)->addr.val >> 24) & 0xFF) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R2_ADDR4, ((op)->addr.val >> 32) & 0xFF))

#define CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_3(op) ( \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R3_ADDR5, ((op)->addr.val >> 40) & 0xFF) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R3_CMD, (op)->cmd.opcode) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R3_NUM_ADDR_BYTES, (op)->addr.nbytes))

#define CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_4(op, chipsel) ( \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R4_ADDR_IOS, ilog2((op)->addr.buswidth)) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R4_CMD_IOS, ilog2((op)->cmd.buswidth)) | \
	FIELD_PREP(CDNS_XSPI_CMD_P1_R4_BANK, chipsel))

#define CDNS_XSPI_CMD_FLD_DSEQ_CMD_1(op) \
	FIELD_PREP(CDNS_XSPI_CMD_INSTR_TYPE, CDNS_XSPI_STIG_INSTR_TYPE_DATA_SEQ)

#define CDNS_XSPI_CMD_FLD_DSEQ_CMD_2(op) \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R2_DCNT_L, (op)->data.nbytes & 0xFFFF)

#define CDNS_XSPI_CMD_FLD_DSEQ_CMD_3(op) ( \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R3_DCNT_H, \
		  (op->data.nbytes >> 16) & 0xffff) | \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R3_NUM_OF_DUMMY, \
		  op->dummy.buswidth != 0 ? \
		  ((op->dummy.nbytes * 8) / op->dummy.buswidth) : \
		  0))

#define CDNS_XSPI_CMD_FLD_DSEQ_CMD_4(op, chipsel) ( \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R4_BANK, chipsel) | \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R4_DATA_IOS, \
		ilog2((op)->data.buswidth)) | \
	FIELD_PREP(CDNS_XSPI_CMD_DSEQ_R4_DIR, \
		((op)->data.dir == SPI_MEM_DATA_IN) ? \
		CDNS_XSPI_STIG_CMD_DIR_READ : CDNS_XSPI_STIG_CMD_DIR_WRITE))

/* clock config register */
#define CDNS_XSPI_CLK_CTRL_REG		      0x4020
#define CDNS_XSPI_CLK_ENABLE                  BIT(0)
#define CDNS_XSPI_CLK_DIV                     GENMASK(4, 1)

/* Clock macros */
#define CDNS_XSPI_CLOCK_IO_HZ 800000000
#define CDNS_XSPI_CLOCK_DIVIDED(div) ((CDNS_XSPI_CLOCK_IO_HZ) / (div))

#define CDNS_XSPI_CTRL_WORK_MODE_STIG           0x01

/*PHY default values*/
#define REGS_DLL_PHY_CTRL	  0x00000707
#define CTB_RFILE_PHY_CTRL	  0x00004000
#define RFILE_PHY_TSEL		  0x00000000
#define RFILE_PHY_DQ_TIMING	  0x00000101
#define RFILE_PHY_DQS_TIMING	  0x00700404
#define RFILE_PHY_GATE_LPBK_CTRL  0x00200030
#define RFILE_PHY_DLL_MASTER_CTRL 0x00800000
#define RFILE_PHY_DLL_SLAVE_CTRL  0x0000ff01

/*PHY config rtegisters*/
#define CDNS_XSPI_RF_MINICTRL_REGS_DLL_PHY_CTRL			0x1034
#define CDNS_XSPI_PHY_CTB_RFILE_PHY_CTRL			0x2080
#define CDNS_XSPI_PHY_CTB_RFILE_PHY_TSEL			0x2084
#define CDNS_XSPI_PHY_DATASLICE_RFILE_PHY_DQ_TIMING		0x2000
#define CDNS_XSPI_PHY_DATASLICE_RFILE_PHY_DQS_TIMING		0x2004
#define CDNS_XSPI_PHY_DATASLICE_RFILE_PHY_GATE_LPBK_CTRL	0x2008
#define CDNS_XSPI_PHY_DATASLICE_RFILE_PHY_DLL_MASTER_CTRL	0x200c
#define CDNS_XSPI_PHY_DATASLICE_RFILE_PHY_DLL_SLAVE_CTRL	0x2010
#define CDNS_XSPI_DATASLICE_RFILE_PHY_DLL_OBS_REG_0		0x201c

#define CDNS_XSPI_DLL_RST_N BIT(24)
#define CDNS_XSPI_DLL_LOCK  BIT(0)

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

enum cdns_xspi_sdma_size {
	CDNS_XSPI_SDMA_SIZE_8B = 0,
	CDNS_XSPI_SDMA_SIZE_64B = 1,
};

struct cdns_xspi_dev {
	void __iomem *iobase;
	void __iomem *auxbase;
	void __iomem *sdmabase;

	int irq;
	int cur_cs;
	unsigned int sdmasize;

	bool sdma_error;

	void *in_buffer;
	const void *out_buffer;

	u8 hw_num_banks;
	enum cdns_xspi_sdma_size read_size;
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

static void iowrite8_rep(void *addr, const uint8_t *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
		writeb(buf[i], addr);
}

static void ioread8_rep(void *addr, uint8_t *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
		buf[i] = readb(addr);
}

static bool cdns_xspi_reset_dll(struct cdns_xspi_dev *cdns_xspi)
{
	u32 dll_cntrl = readl(cdns_xspi->iobase + CDNS_XSPI_RF_MINICTRL_REGS_DLL_PHY_CTRL);
	u32 dll_lock;

	/*Reset DLL*/
	dll_cntrl |= CDNS_XSPI_DLL_RST_N;
	writel(dll_cntrl, cdns_xspi->iobase + CDNS_XSPI_RF_MINICTRL_REGS_DLL_PHY_CTRL);

	/*Wait for DLL lock*/
	return readl_relaxed_poll_timeout(cdns_xspi->iobase +
		CDNS_XSPI_INTR_STATUS_REG,
		dll_lock, ((dll_lock & CDNS_XSPI_DLL_LOCK) == 1), 10000);
}

//Static confiuration of PHY
static bool cdns_xspi_configure_phy(struct cdns_xspi_dev *cdns_xspi)
{
	writel(REGS_DLL_PHY_CTRL, cdns_xspi->iobase + CDNS_XSPI_RF_MINICTRL_REGS_DLL_PHY_CTRL);
	writel(CTB_RFILE_PHY_CTRL, cdns_xspi->iobase + CDNS_XSPI_PHY_CTB_RFILE_PHY_CTRL);
	writel(RFILE_PHY_TSEL, cdns_xspi->iobase + CDNS_XSPI_PHY_CTB_RFILE_PHY_TSEL);
	writel(RFILE_PHY_DQ_TIMING, cdns_xspi->iobase + CDNS_XSPI_PHY_DATASLICE_RFILE_PHY_DQ_TIMING);
	writel(RFILE_PHY_DQS_TIMING, cdns_xspi->iobase + CDNS_XSPI_PHY_DATASLICE_RFILE_PHY_DQS_TIMING);
	writel(RFILE_PHY_GATE_LPBK_CTRL, cdns_xspi->iobase + CDNS_XSPI_PHY_DATASLICE_RFILE_PHY_GATE_LPBK_CTRL);
	writel(RFILE_PHY_DLL_MASTER_CTRL, cdns_xspi->iobase + CDNS_XSPI_PHY_DATASLICE_RFILE_PHY_DLL_MASTER_CTRL);
	writel(RFILE_PHY_DLL_SLAVE_CTRL, cdns_xspi->iobase + CDNS_XSPI_PHY_DATASLICE_RFILE_PHY_DLL_SLAVE_CTRL);

	return cdns_xspi_reset_dll(cdns_xspi);
}

// Find max avalible clocl
static bool cdns_xspi_setup_clock(struct cdns_xspi_dev *cdns_xspi, int requested_clk)
{
	int i = 0;
	int clk_val;
	u32 clk_reg;
	bool update_clk = false;

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

static void cdns_xspi_set_interrupts(struct cdns_xspi_dev *cdns_xspi,
				     bool enabled)
{
	u32 intr_enable;

	if (!cdns_xspi->irq)
		return;

	intr_enable = readl(cdns_xspi->iobase + CDNS_XSPI_INTR_ENABLE_REG);
	if (enabled)
		intr_enable |= CDNS_XSPI_INTR_MASK;
	else
		intr_enable &= ~CDNS_XSPI_INTR_MASK;
	writel(intr_enable, cdns_xspi->iobase + CDNS_XSPI_INTR_ENABLE_REG);
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
	cdns_xspi_set_interrupts(cdns_xspi, false);

	return 0;
}

static int cdns_xspi_probe(struct udevice *bus)
{
	struct cdns_xspi_dev *cdns_xspi = dev_get_priv(bus);
	int ret;

	cdns_xspi_setup_clock(cdns_xspi, 25000000);
	cdns_xspi_configure_phy(cdns_xspi);

	ret = cdns_xspi_controller_init(cdns_xspi);
	if (ret) {
		dev_err(bus, "Failed to initialize controller\n");
		return ret;
	}

	return 0;
}

static int cdns_xspi_ofdata_to_platdata(struct udevice *bus)
{
	struct cdns_xspi_dev *plat = dev_get_priv(bus);
	ofnode node;
	int property;

	plat->iobase = ofnode_get_addr_index(bus->node, 0);
	plat->sdmabase = ofnode_get_addr_index(bus->node, 1);
	plat->auxbase = ofnode_get_addr_index(bus->node, 2);
	plat->irq = 0;

	if (ofnode_read_u32(bus->node, "cdns,read-size", &plat->read_size)) {
		dev_info(pdev, "Failed to get read_size. Using 8 bit.\n");
		plat->read_size = 0;
	}

	ofnode_for_each_subnode(node, bus->node) {
		if (ofnode_read_u32(node, "reg", &property)) {
			dev_err(bus, "Couldn't determine CS value\n");
			return -ENXIO;
		}
	}

	debug(bus, "%s: regbase=%llx ahbbase=%llx sdma-base=%llx read_size=%d\n",
	      __func__, plat->iobase, plat->auxbase, plat->sdmabase, plat->read_size);

	return 0;
}

static void cdns_ioreadq(void __iomem  *addr, void *buf, int len)
{
	int i = 0;
	int rcount = len / 8;
	int rcount_nf = len % 8;
	u64 tmp;
	u64 *buf64 = (uint64_t *)buf;

	if (((uint64_t)buf % 8) == 0) {
		for (i = 0; i < rcount; i++)
			*buf64++ = readq(addr);
	} else {
		for (i = 0; i < rcount; i++) {
			tmp = readq(addr);
			memcpy(buf + (i * 8), &tmp, 8);
		}
	}

	if (rcount_nf != 0) {
		tmp = readq(addr);
		memcpy(buf + (i * 8), &tmp, rcount_nf);
	}
}

static void cdns_iowriteq(void __iomem *addr, const void *buf, int len)
{
	int i = 0;
	int rcount = len / 8;
	int rcount_nf = len % 8;
	u64 tmp;
	u64 *buf64 = (uint64_t *)buf;

	if (((uint64_t)buf % 8) == 0) {
		for (i = 0; i < rcount; i++)
			writeq(*buf64++, addr);
	} else {
		for (i = 0; i < rcount; i++) {
			memcpy(&tmp, buf + (i * 8), 8);
			writeq(tmp, addr);
		}
	}

	if (rcount_nf != 0) {
		memcpy(&tmp, buf + (i * 8), rcount_nf);
		writeq(tmp, addr);
	}
}

static void cdns_xspi_sdma_memread(struct cdns_xspi_dev *cdns_xspi, enum cdns_xspi_sdma_size size, int len)
{
	switch (size) {
	case CDNS_XSPI_SDMA_SIZE_8B:
		ioread8_rep(cdns_xspi->sdmabase,
			    cdns_xspi->in_buffer, len);
		break;
	case CDNS_XSPI_SDMA_SIZE_64B:
		cdns_ioreadq(cdns_xspi->sdmabase, cdns_xspi->in_buffer, len);
		break;
	}
}

static void cdns_xspi_sdma_memwrite(struct cdns_xspi_dev *cdns_xspi, enum cdns_xspi_sdma_size size, int len)
{
	switch (size) {
	case CDNS_XSPI_SDMA_SIZE_8B:
		iowrite8_rep(cdns_xspi->sdmabase,
			     cdns_xspi->out_buffer, len);
		break;
	case CDNS_XSPI_SDMA_SIZE_64B:
		cdns_iowriteq(cdns_xspi->sdmabase, cdns_xspi->in_buffer, len);
		break;
	}
}

static void cdns_xspi_sdma_handle(struct cdns_xspi_dev *cdns_xspi)
{
	u32 sdma_size, sdma_trd_info;
	u8 sdma_dir;

	sdma_size = readl(cdns_xspi->iobase + CDNS_XSPI_SDMA_SIZE_REG);
	sdma_trd_info = readl(cdns_xspi->iobase + CDNS_XSPI_SDMA_TRD_INFO_REG);
	sdma_dir = FIELD_GET(CDNS_XSPI_SDMA_DIR, sdma_trd_info);

	switch (sdma_dir) {
	case CDNS_XSPI_SDMA_DIR_READ:
		cdns_xspi_sdma_memread(cdns_xspi,
				       cdns_xspi->read_size,
				       sdma_size);
		break;

	case CDNS_XSPI_SDMA_DIR_WRITE:
		cdns_xspi_sdma_memwrite(cdns_xspi,
					cdns_xspi->read_size,
					sdma_size);
		break;
	}
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

bool cdns_xspi_sdma_ready(struct cdns_xspi_dev *cdns_xspi)
{
	u32 ctrl_stat;

	if (!otx_is_platform(PLATFORM_ASIM))
		return readl_relaxed_poll_timeout(cdns_xspi->iobase +
			CDNS_XSPI_INTR_STATUS_REG,
			ctrl_stat, ((ctrl_stat & CDNS_XSPI_SDMA_TRIGGER)), 1000);
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

static int cdns_xspi_check_command_status(struct cdns_xspi_dev *cdns_xspi)
{
	int ret = 0;
	u32 cmd_status = readl(cdns_xspi->iobase + CDNS_XSPI_CMD_STATUS_REG);

	if (otx_is_platform(PLATFORM_ASIM))
		return 0;

	if (cmd_status & CDNS_XSPI_CMD_STATUS_COMPLETED) {
		if ((cmd_status & CDNS_XSPI_CMD_STATUS_FAILED) != 0) {
			if (cmd_status & CDNS_XSPI_CMD_STATUS_DQS_ERROR) {
				dev_err(cdns_xspi->dev,
					"Incorrect DQS pulses detected\n");
				ret = -EPROTO;
			}
			if (cmd_status & CDNS_XSPI_CMD_STATUS_CRC_ERROR) {
				dev_err(cdns_xspi->dev,
					"CRC error received\n");
				ret = -EPROTO;
			}
			if (cmd_status & CDNS_XSPI_CMD_STATUS_BUS_ERROR) {
				dev_err(cdns_xspi->dev,
					"Error resp on system DMA interface\n");
				ret = -EPROTO;
			}
			if (cmd_status & CDNS_XSPI_CMD_STATUS_INV_SEQ_ERROR) {
				dev_err(cdns_xspi->dev,
					"Invalid command sequence detected\n");
				ret = -EPROTO;
			}
		}
	} else {
		dev_err(cdns_xspi->dev, "Fatal err - command not completed\n");
		ret = -EPROTO;
	}

	return ret;
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

static int cdns_xspi_send_stig_command(struct cdns_xspi_dev *cdns_xspi,
				       const struct spi_mem_op *op,
				       bool data_phase)
{
	u32 cmd_regs[6];
	u32 cmd_status;
	int ret;

	ret = cdns_xspi_wait_for_controller_idle(cdns_xspi);
	if (ret < 0)
		return -EIO;
	if (cdns_verify_stig_mode_config(cdns_xspi)) {
		printf("Failed to configure xSPI");
		return -1;
	}

	writel(FIELD_PREP(CDNS_XSPI_CTRL_WORK_MODE, CDNS_XSPI_WORK_MODE_STIG),
	       cdns_xspi->iobase + CDNS_XSPI_CTRL_CONFIG_REG);

	cdns_xspi_set_interrupts(cdns_xspi, true);
	cdns_xspi->sdma_error = false;

	memset(cmd_regs, 0, sizeof(cmd_regs));
	cmd_regs[1] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_1(op, data_phase);
	cmd_regs[2] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_2(op);
	cmd_regs[3] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_3(op);
	cmd_regs[4] = CDNS_XSPI_CMD_FLD_P1_INSTR_CMD_4(op,
						       cdns_xspi->cur_cs);

	cdns_xspi_trigger_command(cdns_xspi, cmd_regs);

	if (data_phase) {
		cmd_regs[0] = CDNS_XSPI_STIG_DONE_FLAG;
		cmd_regs[1] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_1(op);
		cmd_regs[2] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_2(op);
		cmd_regs[3] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_3(op);
		cmd_regs[4] = CDNS_XSPI_CMD_FLD_DSEQ_CMD_4(op,
							   cdns_xspi->cur_cs);

		cdns_xspi->in_buffer = op->data.buf.in;
		cdns_xspi->out_buffer = op->data.buf.out;

		cdns_xspi_trigger_command(cdns_xspi, cmd_regs);

		if (cdns_xspi->irq) {
			if (cdns_xspi->sdma_error) {
				cdns_xspi_set_interrupts(cdns_xspi, false);
				return -EIO;
			}
		} else {
			if (cdns_xspi_sdma_ready(cdns_xspi))
				return -EIO;
		}
		cdns_xspi_sdma_handle(cdns_xspi);
	}

	if (cdns_xspi->irq) {
		cdns_xspi_set_interrupts(cdns_xspi, false);
	} else {
		if (cdns_xspi_stig_ready(cdns_xspi))
			return -EIO;
	}

	cmd_status = cdns_xspi_check_command_status(cdns_xspi);
	if (cmd_status)
		return -EPROTO;

	return 0;
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

	cdns_xspi->cur_cs = slave_dev->cs;

	log_debug("SPI opcode: %X buswidth: %d\n",
		  op->cmd.opcode, op->cmd.buswidth);
	log_debug("SPI addr: nbytes: %X buswidth: %d, val: %llX\n",
		  op->addr.nbytes, op->addr.buswidth, op->addr.val);
	log_debug("SPI data: direction: %X, len: %d, ptr: %p\n",
		  op->data.dir, op->data.nbytes, op->data.buf.in);
	log_debug("SPI dummy: nbytes: %d, buswidth: %d\n",
		  op->dummy.nbytes, op->dummy.buswidth);
	if (data_phase)
		log_debug("Data Phase enabled\n");
	else
		log_debug("Data Phase disabled\n");
	log_debug("--------------------------------------------------\n\n");

	ret = cdns_xspi_send_stig_command(cdns_xspi, op, data_phase);

	return ret;
}

#if defined(CONFIG_ARCH_CN10K)
int board_acquire_flash_arb(bool acquire);
#endif

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

static bool cdns_xspi_supports_op(struct spi_slave *slave,
				  const struct spi_mem_op *op)
{
	return true;
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

static const struct udevice_id cdns_xspi_ids[] = {
	{ .compatible = "cdns,xspi-nor" },
	{ }
};

U_BOOT_DRIVER(cadence_spi) = {
	.name = "cadence_spi",
	.id = UCLASS_SPI,
	.of_match = cdns_xspi_ids,
	.ops = &cdns_spi_ops,
	.ofdata_to_platdata = cdns_xspi_ofdata_to_platdata,
	.priv_auto_alloc_size = sizeof(struct cdns_xspi_dev),
	.probe = cdns_xspi_probe,
	.flags = DM_FLAG_OS_PREPARE,
};
