/**
* spi-nand.h
*
* Copyright (c) 2009-2015 Micron Technology, Inc.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/
#ifndef _DM_SPI_NAND_H
#define _DM_SPI_NAND_H

#include <dm.h>
#include <spi-nand.h>

struct dm_spi_nand_flash_ops {
	int (*read)(struct udevice *dev, u32 offset, size_t len, void *buf);
	int (*write)(struct udevice *dev, u32 offset, size_t len,
		     const void *buf);
	int (*erase)(struct udevice *dev, u32 offset, size_t len, bool spread);
	int (*block_is_bad)(struct udevice *dev, loff_t offset);
	int (*block_mark_bad)(struct udevice *dev, loff_t offset);
};

/* Access the serial operations for a device */
#define snf_get_ops(dev) ((struct dm_spi_nand_flash_ops *)(dev)->driver->ops)

/**
 * spi_nand_flash_read_dm() - Read data from SPI nand flash
 *
 * @dev:	SPI nand flash device
 * @offset:	Offset into device in bytes to read from
 * @len:	Number of bytes to read
 * @buf:	Buffer to put the data that is read
 * @return 0 if OK, -ve on error
 */
int spi_nand_flash_read_dm(struct udevice *dev, u32 offset, size_t len,
			   void *buf);

/**
 * spi_nand_flash_write_dm() - Write data to SPI nand flash
 *
 * @dev:	SPI nand flash device
 * @offset:	Offset into device in bytes to write to
 * @len:	Number of bytes to write
 * @buf:	Buffer containing bytes to write
 * @return 0 if OK, -ve on error
 */
int spi_nand_flash_write_dm(struct udevice *dev, u32 offset, size_t len,
		       const void *buf);

/**
 * spi_nand_flash_erase_dm() - Erase blocks of the SPI nand flash
 *
 * Note that @len must be a muiltiple of the flash sector size.
 *
 * @dev:	SPI nand flash device
 * @offset:	Offset into device in bytes to start erasing
 * @len:	Number of bytes to erase
 * @return 0 if OK, -ve on error
 */
int spi_nand_flash_erase_dm(struct udevice *dev, u32 offset, size_t len,
			    bool spread);

/**
 * spi_nand_flash_block_is_bad_dm() - check if a SPI nand flash block is bad
 *
 *
 * @dev:	SPI nand flash device
 * @offset:	Offset into device in bytes which the block starts from
 * @return 0 if OK, -ve on error
 */
int spi_nand_flash_block_is_bad_dm(struct udevice *dev, loff_t offset);

/**
 * spi_nand_flash_block_mark_bad_dm() - Mark a SPI nand flash block as bad block
 *
 *
 * @dev:	SPI nand flash device
 * @offset:	Offset into device in bytes which the block starts from
 * @return 0 if OK, -ve on error
 */
int spi_nand_flash_block_mark_bad_dm(struct udevice *dev, loff_t offset);

int spi_nand_flash_probe_bus_cs(unsigned int busnum, unsigned int cs,
			   unsigned int max_hz, unsigned int spi_mode,
			   struct udevice **devp);

/* Compatibility function - this is the old U-Boot API */
struct spi_nand_chip *spi_nand_flash_probe(unsigned int bus, unsigned int cs,
				  unsigned int max_hz, unsigned int spi_mode);

/* Compatibility function - this is the old U-Boot API */
void spi_nand_flash_free(struct spi_nand_chip *flash);

static inline int spi_nand_flash_read(struct spi_nand_chip *flash, u32 offset,
				 size_t len, void *buf)
{
	return spi_nand_flash_read_dm(flash->dev, offset, len, buf);
}

static inline int spi_nand_flash_write(struct spi_nand_chip *flash, u32 offset,
				  size_t len, const void *buf)
{
	return spi_nand_flash_write_dm(flash->dev, offset, len, buf);
}

static inline int spi_nand_flash_erase(struct spi_nand_chip *flash, u32 offset,
				  size_t len, bool spread)
{
	return spi_nand_flash_erase_dm(flash->dev, offset, len, spread);
}

static inline int spi_nand_flash_block_is_bad(struct spi_nand_chip *flash,
					      u32 offset)
{
	return spi_nand_flash_block_is_bad_dm(flash->dev, offset);
}

static inline int spi_nand_flash_block_mark_bad(struct spi_nand_chip *flash,
						u32 offset)
{
	return spi_nand_flash_block_mark_bad_dm(flash->dev, offset);
}

#endif
