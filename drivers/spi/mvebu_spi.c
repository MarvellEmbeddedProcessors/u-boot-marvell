/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <common.h>
#include <malloc.h>
#include <spi.h>
#include <asm/io.h>
#include <asm/arch/regs-base.h>

/* Constants */
#define SPI_WAIT_RDY_MAX_LOOP		100000
#define SPI_SERIAL_BAUDRATE		(20 << 20)

/* Marvell Flash Device Controller Registers */
#define SPI_IF_CTRL_REG(x)		(MVEBU_SPI_REGS_BASE(x) + 0x00)
#define SPI_IF_CONFIG_REG(x)		(MVEBU_SPI_REGS_BASE(x) + 0x04)
#define SPI_DATA_OUT_REG(x)		(MVEBU_SPI_REGS_BASE(x) + 0x08)
#define SPI_DATA_IN_REG(x)		(MVEBU_SPI_REGS_BASE(x) + 0x0c)
#define SPI_INT_CAUSE_REG(x)		(MVEBU_SPI_REGS_BASE(x) + 0x10)

/* Serial Memory Interface Control Register Masks */
#define SPI_CS_ENABLE_OFFSET		0		/* bit 0 */
#define SPI_CS_ENABLE_MASK		(0x1  << SPI_CS_ENABLE_OFFSET)
#define SPI_CS_NUM_OFFSET		2
#define SPI_CS_NUM_MASK			(0x7 << SPI_CS_NUM_OFFSET)

/* Serial Memory Interface Configuration Register Masks */
#define SPI_BYTE_LENGTH_OFFSET		5	/* bit 5 */
#define SPI_BYTE_LENGTH_MASK		(0x1  << SPI_BYTE_LENGTH_OFFSET)
#define SPI_CPOL_OFFSET			11
#define SPI_CPOL_MASK			(0x1 << SPI_CPOL_OFFSET)
#define SPI_CPHA_OFFSET			12
#define SPI_CPHA_MASK			(0x1 << SPI_CPHA_OFFSET)
#define SPI_TXLSBF_OFFSET		13
#define SPI_TXLSBF_MASK			(0x1 << SPI_TXLSBF_OFFSET)
#define SPI_RXLSBF_OFFSET		14
#define SPI_RXLSBF_MASK			(0x1 << SPI_RXLSBF_OFFSET)

#define SPI_SPR_OFFSET			0
#define SPI_SPR_MASK			(0xF << SPI_SPR_OFFSET)
#define SPI_SPPR_0_OFFSET		4
#define SPI_SPPR_0_MASK			(0x1 << SPI_SPPR_0_OFFSET)
#define SPI_SPPR_HI_OFFSET		6
#define SPI_SPPR_HI_MASK		(0x3 << SPI_SPPR_HI_OFFSET)

void mv_spi_cs_set(u8 spi_id, u8 cs_id)
{
	u32	ctrl_reg;
	static u8 last_cs_id = 0xFF;

	if (last_cs_id == cs_id)
		return;

	ctrl_reg = readl(SPI_IF_CTRL_REG(spi_id));
	ctrl_reg &= ~SPI_CS_NUM_MASK;
	ctrl_reg |= (cs_id << SPI_CS_NUM_OFFSET);
	writel(ctrl_reg, SPI_IF_CTRL_REG(spi_id));

	last_cs_id = cs_id;

	return;
}

/* the SPR together with the SPPR define the SPI clk frequency as
** follows: SPI actual frequency = core_clk / (SPR *(2^SPPR)); */
u32 mv_spi_baud_rate_set(u8 spi_id, u32 cpu_clk)
{
	u32 spr, sppr, divider;
	u32 best_spr = 0, best_sppr = 0, exact_match = 0;
	u32 min_baud_offset = 0xFFFFFFFF;
	u32 cfg_reg;

	/* Find the best prescale configuration - less or equal */
	for (spr = 1; spr <= 15; spr++) {
		for (sppr = 0; sppr <= 7; sppr++) {
			divider = spr * (1 << sppr);
			/* check for higher - irrelevent */
			if ((cpu_clk / divider) > SPI_SERIAL_BAUDRATE)
				continue;

			/* check for exact fit */
			if ((cpu_clk / divider) == SPI_SERIAL_BAUDRATE) {
				best_spr = spr;
				best_sppr = sppr;
				exact_match = 1;
				break;
			}

			/* check if this is better than the previous one */
			if ((SPI_SERIAL_BAUDRATE - (cpu_clk / divider)) < min_baud_offset) {
				min_baud_offset = (SPI_SERIAL_BAUDRATE - (cpu_clk / divider));
				best_spr = spr;
				best_sppr = sppr;
			}
		}

		if (exact_match == 1)
			break;
	}

	if (best_spr == 0) {
		error("SPI baud rate prescale error!\n");
		return 1;
	}

	/* configure the Prescale */
	cfg_reg = readl(SPI_IF_CONFIG_REG(spi_id));
	cfg_reg &= ~(SPI_SPR_MASK | SPI_SPPR_0_MASK | SPI_SPPR_HI_MASK);
	cfg_reg |= ((best_spr << SPI_SPR_OFFSET) |
			((best_sppr & 0x1) << SPI_SPPR_0_OFFSET) |
			((best_sppr >> 1) << SPI_SPPR_HI_OFFSET));
	writel(cfg_reg, SPI_IF_CONFIG_REG(spi_id));

	return 0;
}

void spi_init(void)
{
}

struct spi_slave *mvebu_spi_setup_slave(unsigned int bus, unsigned int cs,
				unsigned int max_hz, unsigned int mode, u32 cpu_clk)
{
	struct spi_slave *slave;
	u32 ctrl_reg;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	slave = spi_alloc_slave_base(bus, cs);
	if (!slave)
		return NULL;

	/* Configure the default SPI mode to be 16bit */
	ctrl_reg = readl(SPI_IF_CONFIG_REG(slave->bus));
	ctrl_reg |= SPI_BYTE_LENGTH_MASK;
	writel(ctrl_reg, SPI_IF_CONFIG_REG(slave->bus));

	/* Verify that the CS is deactivate */
	spi_cs_deactivate(slave);

	mv_spi_cs_set(slave->bus, 0);

	mv_spi_baud_rate_set(slave->bus, cpu_clk);

	/* Set the SPI interface parameters */
	ctrl_reg = readl(SPI_IF_CONFIG_REG(slave->bus));
	ctrl_reg &= ~(SPI_CPOL_MASK | SPI_CPHA_MASK | SPI_TXLSBF_MASK | SPI_RXLSBF_MASK);
	ctrl_reg |= SPI_CPOL_MASK;
	ctrl_reg |= SPI_CPHA_MASK;
	writel(ctrl_reg, SPI_IF_CONFIG_REG(slave->bus));

	return slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	free(slave);
}

int spi_claim_bus(struct spi_slave *slave)
{
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
}

#ifndef CONFIG_SPI_CS_IS_VALID
/*
 * you can define this function board specific
 * define above CONFIG in board specific config file and
 * provide the function in board specific src file
 */
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return ((bus <= 1) && (cs >= 0) && (cs < 8));
}
#endif

void spi_cs_activate(struct spi_slave *slave)
{
	u32 ctrl_reg;
	mv_spi_cs_set(slave->bus, slave->cs);
	ctrl_reg = readl(SPI_IF_CTRL_REG(slave->bus));
	ctrl_reg |= SPI_CS_ENABLE_MASK;
	writel(ctrl_reg, SPI_IF_CTRL_REG(slave->bus));
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	u32 ctrl_reg = readl(SPI_IF_CTRL_REG(slave->bus));
	ctrl_reg &= ~SPI_CS_ENABLE_MASK;
	writel(ctrl_reg, SPI_IF_CTRL_REG(slave->bus));
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
	     void *din, unsigned long flags)
{
	u8 *pdout = (u8 *)dout;
	u8 *pdin = (u8 *)din;
	int i;
	u8 data_out = 0;
	u32 ctrl_reg;

	if (flags & SPI_XFER_BEGIN)
		spi_cs_activate(slave);

	/* Verify that the SPI mode is in 8bit mode */
	ctrl_reg = readl(SPI_IF_CONFIG_REG(0));
	ctrl_reg &= ~SPI_BYTE_LENGTH_MASK;
	writel(ctrl_reg, SPI_IF_CONFIG_REG(0));

	/* TX/RX in 8bit chunks */
	while (bitlen > 0) {
		if (pdout)
			data_out = (*pdout) & 0xff;

		/* Transmitted and wait for the transfer to be completed */
		/* First clear the bit in the interrupt cause register */
		writel(0x0, SPI_INT_CAUSE_REG(slave->bus));
		/* Transmit data */
		writel(data_out, SPI_DATA_OUT_REG(slave->bus));

		/* wait with timeout for memory ready */
		for (i = 0; i < SPI_WAIT_RDY_MAX_LOOP; i++)
			if (readl(SPI_INT_CAUSE_REG(slave->bus))) {
				/* check that the RX data is needed */
				*pdin = readl(SPI_DATA_IN_REG(slave->bus));
				/* increment the pointers */
				if (pdin)
					pdin++;
				if (pdout)
					pdout++;
				bitlen -= 8;
				break;
			}

		if (i >= SPI_WAIT_RDY_MAX_LOOP)
			error("Time out during SPI transfer\n");
	}

	if (flags & SPI_XFER_END)
		spi_cs_deactivate(slave);

	return 0;
}
