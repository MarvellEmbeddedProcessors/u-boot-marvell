/*
 * Copyright (C) 2015 Marvell International Ltd.
 *
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <malloc.h>
#include <spi.h>
#include <wait_bit.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

#define MVEBU_SPI_A3700_XFER_RDY		BIT(1)
#define MVEBU_SPI_A3700_FIFO_FLUSH		BIT(9)
#define MVEBU_SPI_A3700_BYTE_LEN		BIT(5)
#define MVEBU_SPI_A3700_CLK_PHA			BIT(6)
#define MVEBU_SPI_A3700_CLK_POL			BIT(7)
#define MVEBU_SPI_A3700_FIFO_EN			BIT(17)
#define MVEBU_SPI_A3700_SPI_EN_0		BIT(16)
#define MVEBU_SPI_A3700_WFIFO_FULL		BIT(7)
#define MVEBU_SPI_A3700_WFIFO_EMPTY		BIT(6)
#define MVEBU_SPI_A3700_RFIFO_EMPTY		BIT(4)
#define MVEBU_SPI_A3700_WFIFO_RDY		BIT(3)
#define MVEBU_SPI_A3700_RFIFO_RDY		BIT(2)
#define MVEBU_SPI_A3700_XFER_START		BIT(15)
#define MVEBU_SPI_A3700_XFER_STOP		BIT(14)
#define MVEBU_SPI_A3700_RW_EN			BIT(8)
#define MVEBU_SPI_A3700_WFIFO_THRS_BIT		28
#define MVEBU_SPI_A3700_RFIFO_THRS_BIT		24
#define MVEBU_SPI_A3700_FIFO_THRS_MASK		0x7
#define MVEBU_SPI_A3700_CLK_PRESCALE_BIT	0
#define MVEBU_SPI_A3700_CLK_PRESCALE_MASK	\
	(0x1f << MVEBU_SPI_A3700_CLK_PRESCALE_BIT)

/* SPI registers */
struct spi_reg {
	u32 ctrl;	/* 0x10600 */
	u32 cfg;	/* 0x10604 */
	u32 dout;	/* 0x10608 */
	u32 din;	/* 0x1060c */
	u32 inst_addr;	/* 0x10610 */
	u32 addr;	/* 0x10614 */
	u32 rmode;	/* 0x10618 */
	u32 hdr_cnt;	/* 0x1061c */
	u32 din_cnt;	/* 0x10620 */
};

struct mvebu_spi_platdata {
	struct spi_reg *spireg;
	unsigned int frequency;
	unsigned int clock;
	bool fifo_enabled;
};

static void spi_cs_activate(struct spi_reg *reg, int cs)
{
	setbits_le32(&reg->ctrl, MVEBU_SPI_A3700_SPI_EN_0 << cs);
}

static void spi_cs_deactivate(struct spi_reg *reg, int cs)
{
	clrbits_le32(&reg->ctrl, MVEBU_SPI_A3700_SPI_EN_0 << cs);
}

static inline void spi_bytelen_set(struct spi_reg *reg, unsigned int len)
{
	unsigned int data;

	data = readl(&reg->cfg);
	if (len == 4)
		data = data | MVEBU_SPI_A3700_BYTE_LEN;
	else
		data = data & (~MVEBU_SPI_A3700_BYTE_LEN);

	writel(data, &reg->cfg);
}

static inline int spi_is_wfifo_full(struct spi_reg *reg)
{
	u32 val;

	val = readl(&reg->ctrl);
	return val & MVEBU_SPI_A3700_WFIFO_FULL;
}

static inline int spi_is_wfifo_empty(struct spi_reg *reg)
{
	u32 val;

	val = readl(&reg->ctrl);
	return val & MVEBU_SPI_A3700_WFIFO_EMPTY;
}

static int spi_fifo_out(struct spi_reg *reg, unsigned int buf_len,
				 unsigned char *tx_buf)
{
	while (!spi_is_wfifo_full(reg) && buf_len) {
		/*
		 * TODO:
		 * Add 4-byte mode for FIFO mode.
		 */
		writel(*tx_buf, &reg->dout);
		buf_len--;
		tx_buf++;
	}

	/* Return the unwritten bytes number */
	return buf_len;
}

static inline int spi_is_rfifo_empty(struct spi_reg *reg)
{
	u32 val;

	val = readl(&reg->ctrl);
	return val & MVEBU_SPI_A3700_RFIFO_EMPTY;
}

static int spi_fifo_in(struct spi_reg *reg, unsigned int buf_len,
				 unsigned char *rx_buf)
{
	unsigned int val;

	while (!spi_is_rfifo_empty(reg)) {
		/*
		 * TODO:
		 * Add 4-byte mode for FIFO mode.
		 */
		val = readl(&reg->din);
		*rx_buf = val & 0xff;
		rx_buf++;
		buf_len--;
	}

	/* Return the unread bytes number */
	return buf_len;
}

static int spi_fifo_xfer_finisher(struct spi_reg *reg, bool force_stop)
{
	unsigned int val;
	int ret = 0;

	val = readl(&reg->cfg);
	if (force_stop) {
		val |= MVEBU_SPI_A3700_XFER_STOP;
		writel(val, &reg->cfg);
	}

	ret = wait_for_bit(__func__, &reg->ctrl,
			   MVEBU_SPI_A3700_XFER_START, false, 100, false);
	if (ret) {
		printf("spi_fifo_abort_xfer timeout\n");
		return ret;
	}

	val = readl(&reg->cfg);
	if (force_stop) {
		val &= ~MVEBU_SPI_A3700_XFER_STOP;
		writel(val, &reg->cfg);
	}

	return ret;
}

/**
 * spi_legacy_shift_byte() - triggers the real SPI transfer
 * @bytelen:	Indicate how many bytes to transfer.
 * @dout:	Buffer address of what to send.
 * @din:	Buffer address of where to receive.
 *
 * This function triggers the real SPI transfer in legacy mode. It
 * will shift out char buffer from @dout, and shift in char buffer to
 * @din, if necessary.
 *
 * This function assumes that only one byte is shifted at one time.
 * However, it is not its responisbility to set the transfer type to
 * one-byte. Also, it does not guarantee that it will work if transfer
 * type becomes two-byte. See spi_set_legacy() for details.
 *
 * In legacy mode, simply write to the SPI_DOUT register will trigger
 * the transfer.
 *
 * If @dout == NULL, which means no actual data needs to be sent out,
 * then the function will shift out 0x00 in order to shift in data.
 * The XFER_RDY flag is checked every time before accessing SPI_DOUT
 * and SPI_DIN register.
 *
 * The number of transfers to be triggerred is decided by @bytelen.
 *
 * Return:	0 - cool
 *		-ETIMEDOUT - XFER_RDY flag timeout
 */
static int spi_legacy_shift_byte(struct spi_reg *reg, unsigned int bytelen,
				 const void *dout, void *din)
{
	const u8 *dout_8;
	u8 *din_8;
	int ret;

	/* Use 0x00 as dummy dout */
	const u8 dummy_dout = 0x0;
	u32 pending_dout = 0x0;

	/* dout_8: pointer of current dout */
	dout_8 = dout;
	/* din_8: pointer of current din */
	din_8 = din;

	while (bytelen) {
		ret = wait_for_bit(__func__, &reg->ctrl,
				   MVEBU_SPI_A3700_XFER_RDY, true, 100, false);
		if (ret)
			return ret;

		if (dout)
			pending_dout = (u32)*dout_8;
		else
			pending_dout = (u32)dummy_dout;

		/* Trigger the xfer */
		writel(pending_dout, &reg->dout);

		if (din) {
			ret = wait_for_bit(__func__, &reg->ctrl,
					   MVEBU_SPI_A3700_XFER_RDY,
					   true, 100, false);
			if (ret)
				return ret;

			/* Read what is transferred in */
			*din_8 = (u8)readl(&reg->din);
		}

		/* Don't increment the current pointer if NULL */
		if (dout)
			dout_8++;
		if (din)
			din_8++;

		bytelen--;
	}

	return 0;
}

static int spi_fifo_write(struct spi_reg *reg, unsigned int bytelen,
				 const void *dout)
{
	unsigned int val;
	int ret = 0;
	unsigned char *char_p;
	int remain_len;

	/*
	 * Clean number of bytes for instruction, address,
	 * dummy field and read mode
	 */
	writel(0x0, &reg->inst_addr);
	writel(0x0, &reg->addr);
	writel(0x0, &reg->rmode);
	writel(0x0, &reg->hdr_cnt);

	/* Start Write transfer */
	val = readl(&reg->cfg);
	val |= (MVEBU_SPI_A3700_XFER_START | MVEBU_SPI_A3700_RW_EN);
	writel(val, &reg->cfg);

	/* Write data to spi */
	char_p = (unsigned char *)dout;
	while (bytelen) {
		ret = wait_for_bit(__func__, &reg->ctrl,
				   MVEBU_SPI_A3700_WFIFO_RDY, true, 100, false);
		if (ret) {
			printf("SPI: waiting write_fifo_ready timeout.n");
			return ret;
		}

		ret = wait_for_bit(__func__, &reg->ctrl,
				   MVEBU_SPI_A3700_WFIFO_FULL,
				   false, 100, false);
		if (ret) {
			printf("SPI: write fifo is full.\n");
			return ret;
		}

		remain_len = spi_fifo_out(reg, bytelen, char_p);
		/* move fifo out pointer to unfinished data for next shift */
		char_p += bytelen - remain_len;
		bytelen = remain_len;
	}

	ret = wait_for_bit(__func__, &reg->ctrl,
			   MVEBU_SPI_A3700_WFIFO_EMPTY, true, 100, false);
	if (ret) {
		printf("SPI: waiting write_fifo_empty timeout.\n");
		return ret;
	}

	/* When write xfer finishes, force stop is needed */
	ret = spi_fifo_xfer_finisher(reg, true);

	return ret;
}

static int spi_fifo_read(struct spi_reg *reg, unsigned int bytelen,
				 void *din)
{
	unsigned int val;
	int ret = 0;
	unsigned char *char_p;
	int remain_len;

	/*
	 * Clean number of bytes for instruction, address,
	 * dummy field and read mode
	 */
	writel(0x0, &reg->inst_addr);
	writel(0x0, &reg->addr);
	writel(0x0, &reg->rmode);
	writel(0x0, &reg->hdr_cnt);

	/* Set read data length */
	writel(bytelen, &reg->din_cnt);
	/* Start READ transfer */
	val = readl(&reg->cfg);
	val &= ~MVEBU_SPI_A3700_RW_EN;
	val |= MVEBU_SPI_A3700_XFER_START;

	writel(val, &reg->cfg);

	/* Read data from spi */
	char_p = (unsigned char *)din;

	while (bytelen) {
		ret = wait_for_bit(__func__, &reg->ctrl,
				   MVEBU_SPI_A3700_RFIFO_RDY,
				   true, 100, false);
		if (ret) {
			printf("SPI: read fifo ready is timeout.\n");
			return ret;
		}

		remain_len = spi_fifo_in(reg, bytelen, char_p);
		/* Move fifo in pointer to unfinished data for next shift */
		char_p += bytelen - remain_len;
		bytelen = remain_len;
	}

	/* When read xfer finishes, force stop is not needed. */
	ret = spi_fifo_xfer_finisher(reg, false);

	return ret;
}

static int mvebu_spi_xfer_non_fifo(struct udevice *dev, unsigned int bitlen,
			  const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);
	struct spi_reg *reg = plat->spireg;
	unsigned int bytelen;
	int ret;

	bytelen = bitlen / 8;

	if (dout && din)
		debug("This is a duplex transfer.\n");

	/* Activate CS */
	if (flags & SPI_XFER_BEGIN) {
		debug("SPI: activate cs.\n");
		spi_cs_activate(reg, spi_chip_select(dev));
	}

	/* Send and/or receive */
	if (dout || din) {
		ret = spi_legacy_shift_byte(reg, bytelen, dout, din);
		if (ret)
			return ret;
	}

	/* Deactivate CS */
	if (flags & SPI_XFER_END) {
		ret = wait_for_bit(__func__, &reg->ctrl,
				   MVEBU_SPI_A3700_XFER_RDY, true, 100, false);
		if (ret)
			return ret;

		debug("SPI: deactivate cs.\n");
		spi_cs_deactivate(reg, spi_chip_select(dev));
	}

	return 0;
}

static int mvebu_spi_xfer_fifo(struct udevice *dev, unsigned int bitlen,
			  const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);
	struct spi_reg *reg = plat->spireg;
	unsigned int bytelen;
	int ret = 0;
	u32 data;

	bytelen = bitlen / 8;

	spi_bytelen_set(reg, 1);

	if (dout && din)
		debug("This is a duplex transfer.\n");

	/* Activate CS */
	if (flags & SPI_XFER_BEGIN) {
		debug("SPI: activate cs.\n");
		spi_cs_activate(reg, spi_chip_select(dev));
	}

	/* Flush read/write FIFO */
	data = readl(&reg->cfg);
	writel(data | MVEBU_SPI_A3700_FIFO_FLUSH, &reg->cfg);
	ret = wait_for_bit(__func__, &reg->cfg, MVEBU_SPI_A3700_FIFO_FLUSH,
			   false, 1000, false);
	if (ret) {
		printf("SPI: fifo flush action is timeout.\n");
		return ret;
	}

	/* Send and/or receive */
	if (dout)
		ret = spi_fifo_write(reg, bytelen, dout);
	else if (din)
		ret = spi_fifo_read(reg, bytelen, din);

	/* Deactivate CS */
	if (flags & SPI_XFER_END) {
		ret = wait_for_bit(__func__, &reg->ctrl,
				   MVEBU_SPI_A3700_XFER_RDY, true, 100, false);
		if (ret) {
			printf("SPI: waiting spi transfer data ready timeout\n");
			return ret;
		}

		debug("SPI: deactivate cs.\n");
		spi_cs_deactivate(reg, spi_chip_select(dev));
	}

	return ret;
}

static int mvebu_spi_xfer(struct udevice *dev, unsigned int bitlen,
			  const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);

	if (plat->fifo_enabled)
		return mvebu_spi_xfer_fifo(dev, bitlen, dout, din, flags);
	else
		return mvebu_spi_xfer_non_fifo(dev, bitlen, dout, din, flags);
}

static int mvebu_spi_set_speed(struct udevice *bus, uint hz)
{
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);
	struct spi_reg *reg = plat->spireg;
	u32 data;

	data = readl(&reg->cfg);

	/* Set Prescaler */
	data &= ~MVEBU_SPI_A3700_CLK_PRESCALE_MASK;

	/* Calculate Prescaler = (spi_input_freq / spi_max_freq) */
	if (hz > plat->frequency)
		hz = plat->frequency;
	data |= plat->clock / hz;

	writel(data, &reg->cfg);

	return 0;
}

static int mvebu_spi_set_mode(struct udevice *bus, uint mode)
{
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);
	struct spi_reg *reg = plat->spireg;

	/*
	 * Set SPI polarity
	 * 0: Serial interface clock is low when inactive
	 * 1: Serial interface clock is high when inactive
	 */
	if (mode & SPI_CPOL)
		setbits_le32(&reg->cfg, MVEBU_SPI_A3700_CLK_POL);
	else
		clrbits_le32(&reg->cfg, MVEBU_SPI_A3700_CLK_POL);
	if (mode & SPI_CPHA)
		setbits_le32(&reg->cfg, MVEBU_SPI_A3700_CLK_PHA);
	else
		clrbits_le32(&reg->cfg, MVEBU_SPI_A3700_CLK_PHA);

	return 0;
}

static int mvebu_spi_probe(struct udevice *bus)
{
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);
	struct spi_reg *reg = plat->spireg;
	u32 data;
	int ret;


	/* Flush read/write FIFO */
	data = readl(&reg->cfg);
	writel(data | MVEBU_SPI_A3700_FIFO_FLUSH, &reg->cfg);
	ret = wait_for_bit(__func__, &reg->cfg, MVEBU_SPI_A3700_FIFO_FLUSH,
			   false, 1000, false);
	if (ret)
		return ret;

	/* Check if fifo-mode is enabled.*/
	if (fdtdec_get_bool(gd->fdt_blob, bus->of_offset, "fifo-mode")) {
		debug("SPI: FIFO Mode.\n");

		/* Enable FIFO flag */
		plat->fifo_enabled = true;
		/* Enable FIFO mode */
		data |= MVEBU_SPI_A3700_FIFO_EN;

		/*
		 * Set FIFO threshold
		 * For read FIFO threshold, value 0 presents 1 data entry,
		 * which means when data in the read FIFO is equal to or
		 * greater than 1 entry, flag RFIFO_RDY_IS will be set;
		 * For write FIFO threshold, value 7 presents 7 data entry,
		 * which means when data in the write FIFO is less than or
		 * equal to 7 entry, flag WFIFO_RDY_IS will be set;
		 */
		data |= 0 << MVEBU_SPI_A3700_RFIFO_THRS_BIT;
		data |= 7 << MVEBU_SPI_A3700_WFIFO_THRS_BIT;
	} else {
		debug("SPI: Legacy Mode.\n");
		/*
		 * Settings SPI controller to be working in legacy mode,
		 * which means use only DO pin (I/O 1) for Data Out,
		 * and DI pin (I/O 0) for Data In.
		 */
		/* Disable FIFO flag */
		plat->fifo_enabled = false;
		/* Disable FIFO mode */
		data &= ~MVEBU_SPI_A3700_FIFO_EN;
	}

	/* Always shift 1 byte at a time */
	data &= ~MVEBU_SPI_A3700_BYTE_LEN;
	writel(data, &reg->cfg);

	return 0;
}

static int mvebu_spi_ofdata_to_platdata(struct udevice *bus)
{
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);

	plat->spireg = (struct spi_reg *)dev_get_addr(bus);

	/*
	 * FIXME
	 * Right now, mvebu does not have a clock infrastructure in U-Boot
	 * which should be used to query the input clock to the SPI
	 * controller. Once this clock driver is integrated into U-Boot
	 * it should be used to read the input clock and the DT property
	 * can be removed.
	 */
	plat->clock = fdtdec_get_int(gd->fdt_blob, dev_of_offset(bus),
				     "clock-frequency", 160000);
	plat->frequency = fdtdec_get_int(gd->fdt_blob, dev_of_offset(bus),
					 "spi-max-frequency", 40000);

	return 0;
}

static const struct dm_spi_ops mvebu_spi_ops = {
	.xfer		= mvebu_spi_xfer,
	.set_speed	= mvebu_spi_set_speed,
	.set_mode	= mvebu_spi_set_mode,
	/*
	 * cs_info is not needed, since we require all chip selects to be
	 * in the device tree explicitly
	 */
};

static const struct udevice_id mvebu_spi_ids[] = {
	{ .compatible = "marvell,armada-3700-spi" },
	{ }
};

U_BOOT_DRIVER(mvebu_spi) = {
	.name = "mvebu_spi",
	.id = UCLASS_SPI,
	.of_match = mvebu_spi_ids,
	.ops = &mvebu_spi_ops,
	.ofdata_to_platdata = mvebu_spi_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct mvebu_spi_platdata),
	.probe = mvebu_spi_probe,
};
