/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * Derived from drivers/spi/mpc8xxx_spi.c
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <malloc.h>
#include <spi.h>
#include <asm/io.h>
#include <asm/arch/soc.h>
#ifdef CONFIG_KIRKWOOD
#include <asm/arch/mpp.h>
#endif
#include <asm/arch-mvebu/spi.h>

static void _spi_cs_activate(struct kwspi_registers *reg)
{
	setbits_le32(&reg->ctrl, KWSPI_CSN_ACT);
}

static void _spi_cs_deactivate(struct kwspi_registers *reg)
{
	clrbits_le32(&reg->ctrl, KWSPI_CSN_ACT);
}

static int _spi_xfer(struct kwspi_registers *reg, unsigned int bitlen,
		     const void *dout, void *din, unsigned long flags)
{
	unsigned int tmpdout, tmpdin;
	int tm;

	debug("spi_xfer: dout %p din %p bitlen %u\n", dout, din, bitlen);

	if (flags & SPI_XFER_BEGIN)
		_spi_cs_activate(reg);

	/* Handle data in 2 bytes xfer mode by default */
	clrsetbits_le32(&reg->cfg, KWSPI_XFERLEN_MASK, KWSPI_XFERLEN_2BYTE);

	while (bitlen > 4) {
		debug("loopstart bitlen %d\n", bitlen);
		tmpdout = 0;

		/* Shift data so it's msb-justified */
		if (dout) {
			if (bitlen <= 8) {
				/* 1 byte xfer mode for the last one byte */
				clrsetbits_le32(&reg->cfg,
						KWSPI_XFERLEN_MASK,
						KWSPI_XFERLEN_1BYTE);
				tmpdout = (*(u8 *)dout);
			} else {
				tmpdout = __cpu_to_le16(*(u16 *)dout);
			}
		}

		clrbits_le32(&reg->irq_cause, KWSPI_SMEMRDIRQ);
		writel(tmpdout, &reg->dout);	/* Write the data out */

		debug("*** spi_xfer: ... %08x written, bitlen %d\n",
		      tmpdout, bitlen);

		/*
		 * Wait for SPI transmit to get out
		 * or time out (1 second = 1000 ms)
		 * The NE event must be read and cleared first
		 */
		for (tm = 0; tm < KWSPI_TIMEOUT; ++tm) {
			if (readl(&reg->irq_cause) & KWSPI_SMEMRDIRQ) {
				u8 *p;

				if (din) {
					tmpdin = readl(&reg->din);
					debug("spi_xfer: din %p..%08x read\n",
					      din, tmpdin);

					p = (u8 *)din;
					if (bitlen <= 8) {
						*p = (u8)tmpdin;
						p += 1;
						bitlen -= 8;
					} else {
						*(u16 *)p =
							__le16_to_cpu(tmpdin);
						p += 2;
						bitlen -= 16;
					}
					din = p;
				}

				if (dout) {
					p = (u8 *)dout;
					if (bitlen <= 8) {
						p += 1;
						bitlen -= 8;
					} else {
						p += 2;
						bitlen -= 16;
					}
					dout = p;
				}

				break;
			}
		}
		if (tm >= KWSPI_TIMEOUT)
			printf("*** spi_xfer: Time out during SPI transfer\n");

		debug("loopend bitlen %d\n", bitlen);
	}

	if (flags & SPI_XFER_END)
		_spi_cs_deactivate(reg);

	return 0;
}

#ifndef CONFIG_DM_SPI

static struct kwspi_registers *spireg =
	(struct kwspi_registers *)MVEBU_SPI_BASE;

#ifdef CONFIG_KIRKWOOD
static u32 cs_spi_mpp_back[2];
#endif

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				unsigned int max_hz, unsigned int mode)
{
	struct spi_slave *slave;
	u32 data;
#ifdef CONFIG_KIRKWOOD
	static const u32 kwspi_mpp_config[2][2] = {
		{ MPP0_SPI_SCn, 0 }, /* if cs == 0 */
		{ MPP7_SPI_SCn, 0 } /* if cs != 0 */
	};
#endif

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	slave = spi_alloc_slave_base(bus, cs);
	if (!slave)
		return NULL;

	writel(KWSPI_SMEMRDY, &spireg->ctrl);

	/* calculate spi clock prescaller using max_hz */
	data = ((CONFIG_SYS_TCLK / 2) / max_hz) + 0x10;
	data = data < KWSPI_CLKPRESCL_MIN ? KWSPI_CLKPRESCL_MIN : data;
	data = data > KWSPI_CLKPRESCL_MASK ? KWSPI_CLKPRESCL_MASK : data;

	/* program spi clock prescaller using max_hz */
	writel(KWSPI_ADRLEN_3BYTE | data, &spireg->cfg);
	debug("data = 0x%08x\n", data);

	writel(KWSPI_SMEMRDIRQ, &spireg->irq_cause);
	writel(KWSPI_IRQMASK, &spireg->irq_mask);

#ifdef CONFIG_KIRKWOOD
	/* program mpp registers to select  SPI_CSn */
	kirkwood_mpp_conf(kwspi_mpp_config[cs ? 1 : 0], cs_spi_mpp_back);
#endif

	return slave;
}

void spi_free_slave(struct spi_slave *slave)
{
#ifdef CONFIG_KIRKWOOD
	kirkwood_mpp_conf(cs_spi_mpp_back, NULL);
#endif
	free(slave);
}

#if defined(CONFIG_SYS_KW_SPI_MPP)
u32 spi_mpp_backup[4];
#endif

__attribute__((weak)) int board_spi_claim_bus(struct spi_slave *slave)
{
	return 0;
}

int spi_claim_bus(struct spi_slave *slave)
{
#if defined(CONFIG_SYS_KW_SPI_MPP)
	u32 config;
	u32 spi_mpp_config[4];

	config = CONFIG_SYS_KW_SPI_MPP;

	if (config & MOSI_MPP6)
		spi_mpp_config[0] = MPP6_SPI_MOSI;
	else
		spi_mpp_config[0] = MPP1_SPI_MOSI;

	if (config & SCK_MPP10)
		spi_mpp_config[1] = MPP10_SPI_SCK;
	else
		spi_mpp_config[1] = MPP2_SPI_SCK;

	if (config & MISO_MPP11)
		spi_mpp_config[2] = MPP11_SPI_MISO;
	else
		spi_mpp_config[2] = MPP3_SPI_MISO;

	spi_mpp_config[3] = 0;
	spi_mpp_backup[3] = 0;

	/* set new spi mpp and save current mpp config */
	kirkwood_mpp_conf(spi_mpp_config, spi_mpp_backup);
#endif

	return board_spi_claim_bus(slave);
}

__attribute__((weak)) void board_spi_release_bus(struct spi_slave *slave)
{
}

void spi_release_bus(struct spi_slave *slave)
{
#if defined(CONFIG_SYS_KW_SPI_MPP)
	kirkwood_mpp_conf(spi_mpp_backup, NULL);
#endif

	board_spi_release_bus(slave);
}

#ifndef CONFIG_SPI_CS_IS_VALID
/*
 * you can define this function board specific
 * define above CONFIG in board specific config file and
 * provide the function in board specific src file
 */
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && (cs == 0 || cs == 1);
}
#endif

void spi_init(void)
{
}

void spi_cs_activate(struct spi_slave *slave)
{
	_spi_cs_activate(spireg);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	_spi_cs_deactivate(spireg);
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
	     const void *dout, void *din, unsigned long flags)
{
	return _spi_xfer(spireg, bitlen, dout, din, flags);
}

#else

/* Here now the DM part */

struct mvebu_spi_platdata {
	struct kwspi_registers *spireg;
};

struct mvebu_spi_priv {
	struct kwspi_registers *spireg;
};

static int mvebu_spi_set_speed(struct udevice *bus, uint hz)
{
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);
	struct kwspi_registers *reg = plat->spireg;
	u32 data, divider;
	unsigned spr, sppr;

	/*
	 * Calculate spi clock prescaller using max_hz.
	 * SPPR is SPI Baud Rate Pre-selection, it holds bits 5 and 7:6 in
	 * SPI Interface Configuration Register;
	 * SPR is SPI Baud Rate Selection, it holds bits 3:0 in SPI Interface
	 * Configuration Register.
	 * The SPR together with the SPPR define the SPI CLK frequency as
	 * follows:
	 * SPI actual frequency = core_clk / (SPR * (2 ^ SPPR))
	 */
	divider = DIV_ROUND_UP(CONFIG_SYS_TCLK, hz);
	if (divider < 16) {
		/* This is the easy case, divider is less than 16 */
		spr = divider;
		sppr = 0;

	} else {
		unsigned two_pow_sppr;
		/*
		 * Find the highest bit set in divider. This and the
		 * three next bits define SPR (apart from rounding).
		 * SPPR is then the number of zero bits that must be
		 * appended:
		 */
		sppr = fls(divider) - 4;

		/*
		 * As SPR only has 4 bits, we have to round divider up
		 * to the next multiple of 2 ** sppr.
		 */
		two_pow_sppr = 1 << sppr;
		divider = (divider + two_pow_sppr - 1) & -two_pow_sppr;

		/*
		 * recalculate sppr as rounding up divider might have
		 * increased it enough to change the position of the
		 * highest set bit. In this case the bit that now
		 * doesn't make it into SPR is 0, so there is no need to
		 * round again.
		 */
		sppr = fls(divider) - 4;
		spr = divider >> sppr;

		/*
		 * Now do range checking. SPR is constructed to have a
		 * width of 4 bits, so this is fine for sure. So we
		 * still need to check for sppr to fit into 3 bits:
		 */
		if (sppr > 7)
			return -EINVAL;
	}

	data = ((sppr & 0x6) << 5) | ((sppr & 0x1) << 4) | spr;

	/* program spi clock prescaler using max_hz */
	writel(KWSPI_ADRLEN_3BYTE | data, &reg->cfg);
	debug("data = 0x%08x\n", data);

	return 0;
}

static int mvebu_spi_set_mode(struct udevice *bus, uint mode)
{
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);
	struct kwspi_registers *reg = plat->spireg;
	u32 data = readl(&reg->cfg);

	data &= ~(KWSPI_CPHA | KWSPI_CPOL | KWSPI_RXLSBF | KWSPI_TXLSBF);

	if (mode & SPI_CPHA)
		data |= KWSPI_CPHA;
	if (mode & SPI_CPOL)
		data |= KWSPI_CPOL;
	if (mode & SPI_LSB_FIRST)
		data |= (KWSPI_RXLSBF | KWSPI_TXLSBF);

	writel(data, &reg->cfg);

	return 0;
}

static int mvebu_spi_xfer(struct udevice *dev, unsigned int bitlen,
			  const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);

	return _spi_xfer(plat->spireg, bitlen, dout, din, flags);
}

static int mvebu_spi_claim_bus(struct udevice *dev)
{
	struct udevice *bus = dev->parent;
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);

	/* Configure the chip-select in the CTRL register */
	clrsetbits_le32(&plat->spireg->ctrl,
			KWSPI_CS_MASK << KWSPI_CS_SHIFT,
			spi_chip_select(dev) << KWSPI_CS_SHIFT);

	return 0;
}

static int mvebu_spi_probe(struct udevice *bus)
{
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);
	struct kwspi_registers *reg = plat->spireg;

	writel(KWSPI_SMEMRDY, &reg->ctrl);
	writel(KWSPI_SMEMRDIRQ, &reg->irq_cause);
	writel(KWSPI_IRQMASK, &reg->irq_mask);

	return 0;
}

static int mvebu_spi_ofdata_to_platdata(struct udevice *bus)
{
	struct mvebu_spi_platdata *plat = dev_get_platdata(bus);

	plat->spireg = (struct kwspi_registers *)dev_get_addr(bus);

	return 0;
}

static const struct dm_spi_ops mvebu_spi_ops = {
	.claim_bus	= mvebu_spi_claim_bus,
	.xfer		= mvebu_spi_xfer,
	.set_speed	= mvebu_spi_set_speed,
	.set_mode	= mvebu_spi_set_mode,
	/*
	 * cs_info is not needed, since we require all chip selects to be
	 * in the device tree explicitly
	 */
};

static const struct udevice_id mvebu_spi_ids[] = {
	{ .compatible = "marvell,armada-375-spi" },
	{ .compatible = "marvell,armada-380-spi" },
	{ .compatible = "marvell,armada-xp-spi" },
	{ }
};

U_BOOT_DRIVER(mvebu_spi) = {
	.name = "mvebu_spi",
	.id = UCLASS_SPI,
	.of_match = mvebu_spi_ids,
	.ops = &mvebu_spi_ops,
	.ofdata_to_platdata = mvebu_spi_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct mvebu_spi_platdata),
	.priv_auto_alloc_size = sizeof(struct mvebu_spi_priv),
	.probe = mvebu_spi_probe,
};
#endif
