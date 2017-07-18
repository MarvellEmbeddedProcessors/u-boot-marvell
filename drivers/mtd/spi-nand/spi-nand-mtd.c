/*
 * ***************************************************************************
 * Copyright (C) 2017 Marvell International Ltd.
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
#include <linux/errno.h>
#include <linux/mtd/mtd.h>
#include <dm-spi-nand.h>
#include <mtd/mtd-abi.h>

static struct mtd_info sn_mtd_info;
static char sn_mtd_name[8];

static int spi_nand_mtd_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct spi_nand_chip *chip = mtd->priv;
	int err;

	instr->state = MTD_ERASING;

	err = spi_nand_flash_erase(chip, instr->addr, instr->len, false);
	if (err) {
		instr->state = MTD_ERASE_FAILED;
		instr->fail_addr = MTD_FAIL_ADDR_UNKNOWN;
		return -EIO;
	}

	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);

	return 0;
}

static int spi_nand_mtd_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	struct spi_nand_chip *chip = mtd->priv;
	int err;

	err = spi_nand_flash_read(chip, from, len, buf);
	if (!err)
		*retlen = len;

	return err;
}

static int spi_nand_mtd_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	struct spi_nand_chip *chip = mtd->priv;
	int err;

	err = spi_nand_flash_write(chip, to, len, buf);
	if (!err)
		*retlen = len;

	return err;
}

static void spi_nand_mtd_sync(struct mtd_info *mtd)
{
}

static int spi_nand_mtd_number(void)
{
#ifdef CONFIG_SYS_MAX_FLASH_BANKS
	return CONFIG_SYS_MAX_FLASH_BANKS;
#else
	return 0;
#endif
}

int spi_nand_mtd_register(struct spi_nand_chip *chip)
{
	memset(&sn_mtd_info, 0, sizeof(sn_mtd_info));
	sprintf(sn_mtd_name, "spinand%d", spi_nand_mtd_number());

	sn_mtd_info.name = sn_mtd_name;
	sn_mtd_info.type = MTD_SPINANDFLASH;
	sn_mtd_info.flags = MTD_CAP_NANDFLASH;
	sn_mtd_info.writesize = 1;
	sn_mtd_info.writebufsize = chip->page_size;

	sn_mtd_info._erase = spi_nand_mtd_erase;
	sn_mtd_info._read = spi_nand_mtd_read;
	sn_mtd_info._write = spi_nand_mtd_write;
	sn_mtd_info._sync = spi_nand_mtd_sync;

	sn_mtd_info.size = chip->size;
	sn_mtd_info.priv = chip;

	sn_mtd_info.numeraseregions = 0;
	sn_mtd_info.erasesize = chip->block_size;

	return add_mtd_device(&sn_mtd_info);
}

void spi_nand_mtd_unregister(void)
{
	del_mtd_device(&sn_mtd_info);
}
