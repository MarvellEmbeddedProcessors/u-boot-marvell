/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/*******************************************************************************
   Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
   Marvell GPL License Option

   If you received this File from Marvell, you may opt to use, redistribute and/or
   modify this File in accordance with the terms and conditions of the General
   Public License Version 2, June 1991 (the "GPL License"), a copy of which is
   available along with the File in the license.txt file or by writing to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
   on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

   THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
   WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
   DISCLAIMED.  The GPL License provides additional details about this warranty
   disclaimer.
*******************************************************************************/

#include <common.h>
#include <flash.h>

#ifdef CONFIG_SPI_FLASH
#include <spi.h>
#include <spi_flash.h>

struct spi_flash *flash;

unsigned long spi_flash_init(void)
{
	flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS, CONFIG_ENV_SPI_MAX_HZ, SPI_MODE_3);
	if (!flash) {
		printf("Failed to initialize SPI flash at %u:%u\n", CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS);
		return 1;
	}
	return 0;
}

#endif

ulong board_flash_get_legacy(ulong base, int banknum, flash_info_t *info)
{
	int i;

	info->flash_id = FLASH_MAN_AMD | AMD_ID_LV040B;
	info->size = 0x80000 /* 512 K */;
	info->sector_count = 8;
	info->portwidth = 1;
	info->chipwidth = 1;
	info->buffer_size = 0x400;

	for (i = 0; i < 8; i++) {
		info->start[i] = i * 0x10000 + base;
		info->protect[i] = 0;
	}

	return 1;
}
