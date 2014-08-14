/*
 * Interface to SPI flash
 *
 * Copyright (C) 2008 Atmel Corporation
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
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
#ifndef NAND_SPI_H_
#define NAND_SPI_H_

#include <spi.h>
#include <linux/types.h>
#include <linux/compiler.h>

#define SR_OIP_BIT   		    (1 << 0)	    /* Write-in-Progress  */
#define ECC_EN_BIT              (1 << 4)     /* Ecc enabled        */
#define WP_BIT				    (1 << 1)     /* Write Protected bit */

#define CMD_READ_ID			     0x9f        /*from spi_flash_internal.h*/
#define CMD_RDSR			     0x0f	    /* Read Status Register */
#define CMD_READ_TO_CACHE	     0x13	    /* Read Data Bytes to cache */
#define CMD_PP_TO_CACHE   	     0x02	    /* Program Load to cache*/
#define CMD_READ_FROM_CACHE	     0x03	    /* Read Data Bytes from cache */
#define CMD_FAST_READ		     0x0b	    /* Read Data Bytes at Higher Speed */
#define CMD_WREN		         0x06	    /* Write Enable */
#define CMD_PROGRAM_EXECUTE	     0x10	    /* Program execute */
#define CMD_WRSR	    	     0x1f	    /* Write Status Register */
#define CMD_SECTOR_ERASE	     0xd8 	    /* Sector Erase */

#define ECC_ERR_NOT_CORRECTED    0x02
#define NAND_SPI_PAGE_SIZE       2048
#define NAND_SPI_OOB_SIZE        64


struct nand_spi {
	struct spi_slave *spi;

	char *data_buf;
	int   data_pos;

	u32	page_size;
	u32	sector_size;
	u32 page_per_sector;

	int column;
	int page_addr;
	int ecc_status;
};

#endif /* NAND_SPI_H_ */
