/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <malloc.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"

/* M25Pxx-specific commands */
#define CMD_M25PXX_WREN		0x06	/* Write Enable */
#define CMD_M25PXX_WRDI		0x04	/* Write Disable */
#define CMD_M25PXX_RDSR		0x05	/* Read Status Register */
#define CMD_M25PXX_WRSR		0x01	/* Write Status Register */
#define CMD_M25PXX_READ		0x03	/* Read Data Bytes */
#define CMD_M25PXX_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_M25PXX_PP		0x02	/* Page Program */
#define CMD_M25PXX_SE		0xd8	/* Sector Erase */
#define CMD_M25PXX_BE		0xc7	/* Bulk Erase */
#define CMD_M25PXX_DP		0xb9	/* Deep Power-down */
#define CMD_M25PXX_RES		0xab	/* Release from DP, and Read Signature */

#define STM_ID_M25P64		0x17
#define STM_ID_M25P80		0x14
#define STM_ID_M25Q128		0x18

#define STMICRO_SR_WIP		(1 << 0)	/* Write-in-Progress */

#define STM_PROTECT_ALL		0x5C
#define STM_SRWD			0x80

struct stmicro_spi_flash_params {
	u16 id;
	u8 protected;
 	u16 page_size;
	u16 pages_per_sector;
	u16 nr_sectors;
	u8 addr_cycles;
	const char *name;
};

static const struct stmicro_spi_flash_params stmicro_spi_flash_table[] = {
	{
		.id = 0x2011,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 128,
		.nr_sectors = 4,
		.addr_cycles = 3,
		.name = "M25P10",
	},
	{
		.id = 0x2015,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 32,
		.addr_cycles = 3,
		.name = "M25P16",
	},
	{
		.id = 0x2012,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 4,
		.addr_cycles = 3,
		.name = "M25P20",
	},
	{
		.id = 0x2016,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 64,
		.addr_cycles = 3,
		.name = "M25P32",
	},
	{
		.id = 0x2013,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 8,
		.addr_cycles = 3,
		.name = "M25P40",
	},
	{
		.id = 0x2017,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 128,
		.addr_cycles = 3,
		.name = "M25P64",
	},
	{
		.id = 0x2014,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 16,
		.addr_cycles = 3,
		.name = "M25P80",
	},
	{
		.id = 0x2018,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 256,
		.addr_cycles = 3,
		.name = "M25Q128",

	},
	{
		.id = 0xba18,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 256,
		.addr_cycles = 3,
		.name = "N25Q128",
	},
	{
		.id = 0xbb18,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 256,
		.addr_cycles = 3,
		.name = "N25Q128A",
	},
	{
		.id = 0xba19,
		.protected = 1,
 		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 512,
		.addr_cycles = 3,
		.name = "N25Q256",
	},
	{
		.id = 0xba17,
		.protected = 1,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 128,
		.addr_cycles = 3,
		.name = "N25Q64",
	},
};

static int stmicro_protect(struct spi_flash *flash, int enable)
{
	int ret;
	u8 cmd[2];

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		debug("SF: Unable to claim SPI bus\n");
		return ret;
	}

	ret = 0;

	cmd[0] = CMD_M25PXX_WRSR;
	if (enable == 1)
		cmd[1] = STM_SRWD | STM_PROTECT_ALL;
	else
		cmd[1] = STM_SRWD;

	ret = spi_flash_cmd(flash->spi, CMD_M25PXX_WREN, NULL, 0);
	if (ret < 0) {
		debug("SF: Enabling Write failed\n");
		return ret;
	}

	ret = spi_flash_cmd_write(flash->spi, cmd, 2, NULL, 0);
	if (ret < 0) {
		debug("SF: STMicro Write Status Register failed\n");
		return ret;
	}

	ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
	if (ret < 0) {
		debug("SF: STMicro page programming timed out\n");
		return ret;
	}

	spi_release_bus(flash->spi);
	return ret;
}

struct spi_flash *spi_flash_probe_stmicro(struct spi_slave *spi, u8 * idcode)
{
	const struct stmicro_spi_flash_params *params;
	struct spi_flash *flash;
	unsigned int i;
	u16 id;

	if (idcode[0] == 0xff) {
		i = spi_flash_cmd(spi, CMD_M25PXX_RES, idcode, 4);
		if (i)
			return NULL;
		if ((idcode[3] & 0xf0) == 0x10) {
			idcode[0] = 0x20;
			idcode[1] = 0x20;
			idcode[2] = idcode[3] + 1;
		} else
			return NULL;
	}

	id = ((idcode[1] << 8) | idcode[2]);

	for (i = 0; i < ARRAY_SIZE(stmicro_spi_flash_table); i++) {
		params = &stmicro_spi_flash_table[i];
		if (params->id == id) {
			break;
		}
	}

	if (i == ARRAY_SIZE(stmicro_spi_flash_table)) {
		debug("SF: Unsupported STMicro ID %04x\n", id);
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
	flash->sector_size = 256 * params->pages_per_sector;
	flash->size = flash->sector_size * params->nr_sectors;
	flash->addr_cycles=params->addr_cycles;
	/* In first prove remove HW protection */
	if(params->protected)
		stmicro_protect(flash, 0);

	return flash;
}
