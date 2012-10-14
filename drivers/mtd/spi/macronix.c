/*
 * Copyright 2009(C) Marvell International Ltd. and its affiliates
 * Prafulla Wadaskar <prafulla@marvell.com>
 *
 * Based on drivers/mtd/spi/stmicro.c
 *
 * Copyright 2008, Network Appliance Inc.
 * Jason McMullan <mcmullan@netapp.com>
 *
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <malloc.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"

struct macronix_spi_flash_params {
	u16 idcode;
	u16 nr_blocks;
	u8  addr_cycles;
	const char *name;
};

static const struct macronix_spi_flash_params macronix_spi_flash_table[] = {
	{
		.idcode = 0x2013,
		.nr_blocks = 8,
		.addr_cycles = 3,
		.name = "MX25L4005",
	},
	{
		.idcode = 0x2014,
		.nr_blocks = 16,
		.addr_cycles = 3,
		.name = "MX25L8005",
	},
	{
		.idcode = 0x2015,
		.nr_blocks = 32,
		.addr_cycles = 3,
		.name = "MX25L1605D",
	},
	{
		.idcode = 0x2016,
		.nr_blocks = 64,
		.addr_cycles = 3,
		.name = "MX25L3205D",
	},
	{
		.idcode = 0x2017,
		.nr_blocks = 128,
		.addr_cycles = 3,
		.name = "MX25L6405D",
	},
	{
		.idcode = 0x2018,
		.nr_blocks = 256,
		.addr_cycles = 3,
		.name = "MX25L12805D",
	},
	{
		.idcode = 0x2019,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 512,
		.addr_cycles = 4,
		.name = "MX25L25735E",
	},
	{
		.idcode = 0x2618,
		.nr_blocks = 256,
		.addr_cycles = 3,
		.name = "MX25L12855E",
	},
};

struct spi_flash *spi_flash_probe_macronix(struct spi_slave *spi, u8 *idcode)
{
	const struct macronix_spi_flash_params *params;
	struct spi_flash *flash;
	unsigned int i;
	u16 id = idcode[2] | idcode[1] << 8;

	for (i = 0; i < ARRAY_SIZE(macronix_spi_flash_table); i++) {
		params = &macronix_spi_flash_table[i];
		if (params->idcode == id)
			break;
	}

	if (i == ARRAY_SIZE(macronix_spi_flash_table)) {
		debug("SF: Unsupported Macronix ID %04x\n", id);
		return NULL;
	}

	flash = malloc(sizeof(*flash));
	if (!flash) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	flash->spi = spi;
	flash->name = params->name;

	flash->write = spi_flash_cmd_write_multi;
	flash->erase = spi_flash_cmd_erase;
	flash->read = spi_flash_cmd_read_fast;
	flash->page_size = 256;
	flash->sector_size = 256 * 16 * 16;
	flash->size = flash->sector_size * params->nr_blocks;
	flash->addr_cycles=params->addr_cycles;
	/* Clear BP# bits for read-only flash */
	spi_flash_cmd_write_status(flash, 0);

	return flash;
}
