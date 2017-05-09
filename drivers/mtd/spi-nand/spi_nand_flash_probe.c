/*
 * ***************************************************************************
 * Copyright (C) 2017 Marvell International Ltd.
 * ***************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of Marvell nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************
 */

#include <common.h>
#include <errno.h>
#include <malloc.h>
#include <spi.h>
#include <dm-spi-nand.h>

static int spi_nand_flash_std_read(struct udevice *dev, u32 offset, size_t len,
			      void *buf)
{
	struct spi_nand_chip *chip = dev_get_uclass_priv(dev);


	return spi_nand_cmd_read_ops(chip, offset, len, buf);
}

static int spi_nand_flash_std_write(struct udevice *dev, u32 offset, size_t len,
			const void *buf)
{
	struct spi_nand_chip *chip = dev_get_uclass_priv(dev);

	return spi_nand_cmd_write_ops(chip, offset, len, buf);
}

static int spi_nand_flash_std_erase(struct udevice *dev, u32 offset,
				    size_t len, bool spread)
{
	struct spi_nand_chip *chip = dev_get_uclass_priv(dev);

	return spi_nand_cmd_erase_ops(chip, offset, len, spread);
}

static int spi_nand_flash_std_block_is_bad(struct udevice *dev, loff_t offset)
{
	struct spi_nand_chip *chip = dev_get_uclass_priv(dev);

	return spi_nand_block_isbad(chip, offset);
}

static int spi_nand_flash_std_block_mark_bad(struct udevice *dev,
					     loff_t offset)
{
	struct spi_nand_chip *chip = dev_get_uclass_priv(dev);

	return spi_nand_block_markbad(chip, offset);
}

static int spi_nand_flash_std_probe(struct udevice *dev)
{
	struct spi_slave *slave = dev_get_parent_priv(dev);
	struct dm_spi_slave_platdata *plat = dev_get_parent_platdata(dev);
	struct spi_nand_chip *chip;

	chip = dev_get_uclass_priv(dev);
	memset(chip, 0, sizeof(struct spi_nand_chip));
	chip->dev = dev;
	debug("%s: slave=%p, cs=%d\n", __func__, slave, plat->cs);
	return spi_nand_probe_slave(slave, chip);
}

static const struct dm_spi_nand_flash_ops spi_nand_flash_std_ops = {
	.read = spi_nand_flash_std_read,
	.write = spi_nand_flash_std_write,
	.erase = spi_nand_flash_std_erase,
	.block_is_bad = spi_nand_flash_std_block_is_bad,
	.block_mark_bad = spi_nand_flash_std_block_mark_bad,
};

static const struct udevice_id spi_nand_flash_std_ids[] = {
	{ .compatible = "spi-nand-flash" },
	{ }
};

U_BOOT_DRIVER(spi_nand_flash_std) = {
	.name		= "spi_nand_flash_std",
	.id		= UCLASS_SPI_NAND_FLASH,
	.of_match	= spi_nand_flash_std_ids,
	.probe		= spi_nand_flash_std_probe,
	.ops		= &spi_nand_flash_std_ops,
};

