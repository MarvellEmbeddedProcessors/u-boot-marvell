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

#include <common.h>
#include <malloc.h>
#include <spi.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/clock.h>

/* Constants */
#define CONFIG_MAX_SPI_NUM		8
#define CONFIG_MIN_SPI_CLK		500000
#define SPI_WAIT_RDY_MAX_LOOP		100000

/* Marvell Flash Device Controller Registers */
#define SPI_IF_CTRL_REG			(0x00)
#define SPI_IF_CONFIG_REG		(0x04)
#define SPI_DATA_OUT_REG		(0x08)
#define SPI_DATA_IN_REG			(0x0c)
#define SPI_INT_CAUSE_REG		(0x10)

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

DECLARE_GLOBAL_DATA_PTR;


struct mvebu_spi_bus {
	void *base_reg;
	int bus_num;
	u32 max_freq;
};

static struct mvebu_spi_bus spi_bus;

void mv_spi_cs_set(u8 spi_id, u8 cs_id)
{
	u32	ctrl_reg;
	static u8 last_cs_id = 0xFF;

	if (last_cs_id == cs_id)
		return;

	ctrl_reg = readl(spi_bus.base_reg + SPI_IF_CTRL_REG);
	ctrl_reg &= ~SPI_CS_NUM_MASK;
	ctrl_reg |= (cs_id << SPI_CS_NUM_OFFSET);
	writel(ctrl_reg, spi_bus.base_reg + SPI_IF_CTRL_REG);

	last_cs_id = cs_id;

	return;
}

/* the SPR together with the SPPR define the SPI clk frequency as
** follows: SPI actual frequency = core_clk / (SPR *(2^SPPR)); */
u32 mv_spi_baud_rate_set(u8 spi_id, u32 cpu_clk, u32 spi_max_freq)
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
			if ((cpu_clk / divider) > spi_max_freq)
				continue;

			/* check for exact fit */
			if ((cpu_clk / divider) == spi_max_freq) {
				best_spr = spr;
				best_sppr = sppr;
				exact_match = 1;
				break;
			}

			/* check if this is better than the previous one */
			if ((spi_max_freq - (cpu_clk / divider)) < min_baud_offset) {
				min_baud_offset = (spi_max_freq - (cpu_clk / divider));
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
	cfg_reg = readl(spi_bus.base_reg + SPI_IF_CONFIG_REG);
	cfg_reg &= ~(SPI_SPR_MASK | SPI_SPPR_0_MASK | SPI_SPPR_HI_MASK);
	cfg_reg |= ((best_spr << SPI_SPR_OFFSET) |
			((best_sppr & 0x1) << SPI_SPPR_0_OFFSET) |
			((best_sppr >> 1) << SPI_SPPR_HI_OFFSET));
	writel(cfg_reg, spi_bus.base_reg + SPI_IF_CONFIG_REG);

	return 0;
}

void spi_init(void)
{
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				unsigned int max_hz, unsigned int mode)
{
	struct spi_slave *slave;
	u32 ctrl_reg, clock = -1;
	int node_list[CONFIG_MAX_SPI_NUM], node;
	u32 i, count, polarity = -1;

	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "spi",
			COMPAT_MVEBU_SPI, node_list, CONFIG_MAX_SPI_NUM);

	spi_bus.bus_num = -1;
	for (i = 0; i < count ; i++) {
		node = node_list[i];

		if (node <= 0)
			continue;
		if (bus == i) {
			spi_bus.bus_num = i;
			spi_bus.base_reg = fdt_get_regs_offs(gd->fdt_blob, node, "reg");
			spi_bus.max_freq = fdtdec_get_int(gd->fdt_blob, node, "spi-max-frequency", CONFIG_MIN_SPI_CLK);
			clock = soc_clock_get(gd->fdt_blob, node);
			polarity = (1 << cs) & fdtdec_get_int(gd->fdt_blob, node, "cpol-cpha-cs-bitmap", 0);
		}
	}

	/* if no spi bus found, return NULL */
	if (spi_bus.bus_num == -1)
		return NULL;

	if (!spi_cs_is_valid(spi_bus.bus_num, cs))
		return NULL;

	slave = spi_alloc_slave_base(spi_bus.bus_num, cs);
	if (!slave)
		return NULL;

	/* Configure the default SPI mode to be 16bit */
	ctrl_reg = readl(spi_bus.base_reg + SPI_IF_CONFIG_REG);
	ctrl_reg |= SPI_BYTE_LENGTH_MASK;
	writel(ctrl_reg, spi_bus.base_reg + SPI_IF_CONFIG_REG);

	/* Verify that the CS is deactivate */
	spi_cs_deactivate(slave);

	mv_spi_cs_set(slave->bus, cs);

	mv_spi_baud_rate_set(slave->bus, clock, spi_bus.max_freq);

	/* Set the SPI interface parameters */
	ctrl_reg = readl(spi_bus.base_reg + SPI_IF_CONFIG_REG);
	ctrl_reg &= ~(SPI_CPOL_MASK | SPI_CPHA_MASK | SPI_TXLSBF_MASK | SPI_RXLSBF_MASK);
	if (polarity) {
		ctrl_reg |= SPI_CPOL_MASK;
		ctrl_reg |= SPI_CPHA_MASK;
	}
	writel(ctrl_reg, spi_bus.base_reg + SPI_IF_CONFIG_REG);

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
	ctrl_reg = readl(spi_bus.base_reg + SPI_IF_CTRL_REG);
	ctrl_reg |= SPI_CS_ENABLE_MASK;
	writel(ctrl_reg, spi_bus.base_reg + SPI_IF_CTRL_REG);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	u32 ctrl_reg = readl(spi_bus.base_reg + SPI_IF_CTRL_REG);
	ctrl_reg &= ~SPI_CS_ENABLE_MASK;
	writel(ctrl_reg, spi_bus.base_reg + SPI_IF_CTRL_REG);
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
	ctrl_reg = readl(spi_bus.base_reg + SPI_IF_CONFIG_REG);
	ctrl_reg &= ~SPI_BYTE_LENGTH_MASK;
	writel(ctrl_reg, spi_bus.base_reg + SPI_IF_CONFIG_REG);

	/* TX/RX in 8bit chunks */
	while (bitlen > 0) {
		if (pdout)
			data_out = (*pdout) & 0xff;

		/* Transmitted and wait for the transfer to be completed */
		/* First clear the bit in the interrupt cause register */
		writel(0x0, spi_bus.base_reg + SPI_INT_CAUSE_REG);
		/* Transmit data */
		writel(data_out, spi_bus.base_reg + SPI_DATA_OUT_REG);

		/* wait with timeout for memory ready */
		for (i = 0; i < SPI_WAIT_RDY_MAX_LOOP; i++)
			if (readl(spi_bus.base_reg + SPI_INT_CAUSE_REG)) {
				/* check that the RX data is needed */
				*pdin = readl(spi_bus.base_reg + SPI_DATA_IN_REG);
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
