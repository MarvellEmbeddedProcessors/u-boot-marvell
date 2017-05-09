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
#include <spi.h>
#include <dm-spi-nand.h>
#include <dm/device-internal.h>

DECLARE_GLOBAL_DATA_PTR;

int spi_nand_flash_read_dm(struct udevice *dev, u32 offset, size_t len,
			   void *buf)
{
	return snf_get_ops(dev)->read(dev, offset, len, buf);
}

int spi_nand_flash_write_dm(struct udevice *dev, u32 offset, size_t len,
			    const void *buf)
{
	return snf_get_ops(dev)->write(dev, offset, len, buf);
}

int spi_nand_flash_erase_dm(struct udevice *dev, u32 offset, size_t len,
			    bool spread)
{
	return snf_get_ops(dev)->erase(dev, offset, len, spread);
}

int spi_nand_flash_block_is_bad_dm(struct udevice *dev, loff_t offset)
{
	return snf_get_ops(dev)->block_is_bad(dev, offset);
}

int spi_nand_flash_block_mark_bad_dm(struct udevice *dev, loff_t offset)
{
	return snf_get_ops(dev)->block_mark_bad(dev, offset);
}

/*
 * TODO(sjg@chromium.org): This is an old-style function. We should remove
 * it when all SPI nand flash drivers use dm
 */
struct spi_nand_chip *spi_nand_flash_probe(unsigned int bus, unsigned int cs,
				  unsigned int max_hz, unsigned int spi_mode)
{
	struct udevice *dev;

	if (spi_nand_flash_probe_bus_cs(bus, cs, max_hz, spi_mode, &dev))
		return NULL;

	return dev_get_uclass_priv(dev);
}

void spi_nand_flash_free(struct spi_nand_chip *flash)
{
	device_remove(flash->spi->dev, DM_REMOVE_NORMAL);
}

int spi_nand_flash_probe_bus_cs(unsigned int busnum, unsigned int cs,
			   unsigned int max_hz, unsigned int spi_mode,
			   struct udevice **devp)
{
	struct spi_slave *slave;
	struct udevice *bus;
	char *str;
	int ret;

#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_USE_TINY_PRINTF)
	str = "spi_nand_flash";
#else
	char name[30];

	snprintf(name, sizeof(name), "spi_nand_flash@%d:%d", busnum, cs);
	str = strdup(name);
#endif
	ret = spi_get_bus_and_cs(busnum, cs, max_hz, spi_mode,
				  "spi_nand_flash_std", str, &bus, &slave);
	if (ret)
		return ret;

	*devp = slave->dev;
	return 0;
}

static int spi_nand_flash_post_bind(struct udevice *dev)
{
#if defined(CONFIG_NEEDS_MANUAL_RELOC)
	struct dm_spi_nand_flash_ops *ops = snf_get_ops(dev);
	static int reloc_done;

	if (!reloc_done) {
		if (ops->read)
			ops->read += gd->reloc_off;
		if (ops->write)
			ops->write += gd->reloc_off;
		if (ops->erase)
			ops->erase += gd->reloc_off;

		reloc_done++;
	}
#endif
	return 0;
}

UCLASS_DRIVER(spi_nand_flash) = {
	.id		= UCLASS_SPI_NAND_FLASH,
	.name		= "spi_nand_flash",
	.post_bind	= spi_nand_flash_post_bind,
	.per_device_auto_alloc_size = sizeof(struct spi_nand_chip),
};
