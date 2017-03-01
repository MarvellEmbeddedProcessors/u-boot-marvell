/*
 * Command for updating boot image.
 *
 * Copyright (C) 2008 Atmel Corporation
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <div64.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <malloc.h>
#include <mapmem.h>
#include <memalign.h>
#include <mmc.h>
#include <spi.h>
#include <spi_flash.h>
#include <jffs2/jffs2.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>

struct dos_partition {
	unsigned char boot_ind;		/* 0x80 - active			*/
	unsigned char head;		/* starting head			*/
	unsigned char sector;		/* starting sector			*/
	unsigned char cyl;		/* starting cylinder			*/
	unsigned char sys_ind;		/* What partition type			*/
	unsigned char end_head;		/* end head				*/
	unsigned char end_sector;	/* end sector				*/
	unsigned char end_cyl;		/* end cylinder				*/
	unsigned int  start4;		/* starting sector counting from 0	*/
	unsigned int  size4;		/* nr of sectors in partition		*/
};

static struct spi_flash *flash;

static int validate_bootimg_header(unsigned long addr)
{
	char flash_hdr[] = {"CVM_CLIB"};
	char bdk_magic[] = {"THUNDERX"};
	char *buf1 = (char *)(addr + 0x10000); /* flash hdr offset */
	char *buf2 = (char *)(addr + 0x20008); /* bdk magic offset */
	char *buf3 = (char *)(addr + 0x50008); /* sec bdk magic offset */

	if (strncmp(buf1, flash_hdr, 8) == 0)
		if (strncmp(buf2, bdk_magic, 8) == 0)
			if (strncmp(buf3, bdk_magic, 8) == 0)
				return 0;
	return 1;
}

/**
 * This function takes a byte length and a delta unit of time to compute the
 * approximate bytes per second
 *
 * @param len		amount of bytes currently processed
 * @param start_ms	start time of processing in ms
 * @return bytes per second if OK, 0 on error
 */
static ulong bytes_per_second(unsigned int len, ulong start_ms)
{
	/* less accurate but avoids overflow */
	if (len >= ((unsigned int)-1) / 1024)
		return len / (max(get_timer(start_ms) / 1024, 1UL));
	else
		return 1024 * len / max(get_timer(start_ms), 1UL);
}

/**
 * Write a block of data to SPI flash, first checking if it is different from
 * what is already there.
 *
 * If the data being written is the same, then *skipped is incremented by len.
 *
 * @param flash		flash context pointer
 * @param offset	flash offset to write
 * @param len		number of bytes to write
 * @param buf		buffer to write from
 * @param cmp_buf	read buffer to use to compare data
 * @return NULL if OK, else a string containing the stage which failed
 */
static const char *spi_flash_update_block(struct spi_flash *flash, u32 offset,
					  size_t len, const char *buf)
{
	char *ret = NULL;
	char *ptr = (char *)buf;
	char *rbuf = memalign(ARCH_DMA_MINALIGN, len);

	debug("\n offset=%#x, sector_size=%#x, len=%#zx\n",
	      offset, flash->sector_size, len);

	/* Erase the entire sector */
	if (spi_flash_erase(flash, offset, flash->sector_size))
		ret = "erase";

	/* Write one complete sector */
	if (spi_flash_write(flash, offset, len, ptr))
		ret = "write";

	if (spi_flash_read(flash, offset, len, rbuf))
		ret = "read";

	if (memcmp(ptr, rbuf, len))
		ret = "compare";

	free(rbuf);
	return ret;
}

/**
 * Update an area of SPI flash by erasing and writing any blocks which need
 * to change. Existing blocks with the correct data are left unchanged.
 *
 * @param flash		flash context pointer
 * @param offset	flash offset to write
 * @param len		number of bytes to write
 * @param buf		buffer to write from
 * @return 0 if ok, 1 on error
 */
static int spi_flash_update(struct spi_flash *flash, u32 offset,
			    size_t len, const char *buf)
{
	const char *err_oper = NULL;
	const char *end = buf + len;
	size_t todo;		/* number of bytes to do in this pass */
	const ulong start_time = get_timer(0);
	size_t scale = 1;
	const char *start_buf = buf;
	ulong delta;

	if (end - buf >= 200)
		scale = (end - buf) / 100;
	ulong last_update = get_timer(0);

	for (; (buf < end) && (!err_oper); buf += todo, offset += todo) {
		todo = min_t(size_t, end - buf, flash->sector_size);
		if (get_timer(last_update) > 100) {
			printf("   \rUpdating, %zu%% %lu B/s",
			       100 - (end - buf) / scale,
				bytes_per_second(buf - start_buf,
						 start_time));
			last_update = get_timer(0);
		}
		err_oper = spi_flash_update_block(flash, offset, todo, buf);
		if (err_oper)
			break;
	}
	putc('\r');
	if (err_oper) {
		printf("SPI flash failed in %s step\n", err_oper);
		return 1;
	}

	delta = get_timer(start_time);
	printf("%zu bytes written", len);
	printf(" in %ld.%lds, speed %ld B/s\n",
	       delta / 1000, delta % 1000, bytes_per_second(len, start_time));

	return 0;
}

static int do_spi_flash_probe(int argc, char * const argv[])
{
	unsigned int bus = CONFIG_SF_DEFAULT_BUS;
	unsigned int cs = CONFIG_SF_DEFAULT_CS;
	unsigned int speed = CONFIG_SF_DEFAULT_SPEED;
	unsigned int mode = CONFIG_SF_DEFAULT_MODE;
	struct udevice *new, *bus_dev;
	int ret;

	/* Remove the old device, otherwise probe will just be a nop */
	ret = spi_find_bus_and_cs(bus, cs, &bus_dev, &new);
	if (!ret) {
		device_remove(new);
		device_unbind(new);
	}
	flash = NULL;
	ret = spi_flash_probe_bus_cs(bus, cs, speed, mode, &new);
	if (ret) {
		printf("Failed to initialize SPI flash at %u:%u (error %d)\n",
		       bus, cs, ret);
		return 1;
	}

	flash = dev_get_uclass_priv(new);

	return 0;
}

static int do_bootu_spi(int argc, char * const argv[])
{
	unsigned long addr, offset, len;
	void *buf;
	char *env1, *env2;
	int ret = 1;

	if ((argc < 1) || (argc > 3))
		return -1;

	offset = 0;

	debug("%s argv0 %s argv1 %s\n", __func__, argv[0], argv[1]);

	if (argc == 1) {
		env1 = getenv("kernel_addr");
		env2 = getenv("filesize");
	} else {
		if (!argv[1] || !argv[2])
			return -1;
		env1 = argv[1];
		env2 = argv[2];
	}

	debug("%s kernel_addr %s filesize %s\n", __func__, env1, env2);

	ret = strict_strtoul(env1, 16, &addr);
	if (ret)
		return -1;
	debug("%s addr %ld\n", __func__, addr);

	ret = strict_strtoul(env2, 16, &len);
	if (ret)
		return -1;
	debug("%s len %ld\n", __func__, len);

	if (validate_bootimg_header(addr)) {
		printf("\n No valid boot image header found \n");
		return 1;
	}

	/* The remaining commands require a selected device */
	if (!flash) {
		debug("No SPI flash selected.  Executing 'sf probe'\n");
		ret = do_spi_flash_probe(0, NULL);
		if (ret)
			return 1;
	}
	/* Consistency checking */
	if (offset + len > flash->size) {
		printf("ERROR: attempting %s past flash size (%#x)\n",
		       argv[0], flash->size);
		return 1;
	}

	buf = map_physmem(addr, len, MAP_WRBACK);
	if (!buf) {
		puts("Failed to map physical memory\n");
		return 1;
	}

	ret = spi_flash_update(flash, offset, len, buf);

	printf("bootu SPI : %zu bytes @ %#x Written ", (size_t)len, (u32)offset);
	if (ret)
		printf("ERROR %d\n", ret);
	else
		printf("OK\n");

	unmap_physmem(buf, len);

	return ret == 0 ? 0 : 1;
}

static int validate_partition_table(unsigned char *buf)
{
	struct dos_partition *p;

	if ((buf[510] != 0x55) || (buf[511] != 0xaa))
		return 1; /* no DOS Signature at all */

	/* checks for FAT12 as partition 1 */
	p = (struct dos_partition *)&buf[446];
	if (p->sys_ind != 0x01)
		return 1;

	/* check for second partition start <16MB */
	p = (struct dos_partition *)&buf[446 + 16];
	if (p->sys_ind != 0)
		if (__swab32p(&p->start4) < 0x8000)
			return 1;

	/* check for third partition start <16MB */
	p = (struct dos_partition *)&buf[446 + 16 * 2];
	if (p->sys_ind != 0)
		if (__swab32p(&p->start4) < 0x8000)
			return 1;

	/* check for fourth partition start <16MB */
	p = (struct dos_partition *)&buf[446 + 16 * 3];
	if (p->sys_ind != 0)
		if (__swab32p(&p->start4) < 0x8000)
			return 1;

	return 0;
}

static struct mmc *init_mmc_device(int dev, bool force_init)
{
	struct mmc *mmc;

	mmc = find_mmc_device(dev);
	if (!mmc) {
		printf("no mmc device at slot %x\n", dev);
		return NULL;
	}
	if (force_init)
		mmc->has_init = 0;
	if (mmc_init(mmc))
		return NULL;
	return mmc;
}

static int do_bootu_mmc(int argc, char * const argv[])
{
	static int curr_device = -1;
	struct mmc *mmc;
	char *env1, *env2;
	unsigned long blk, len, n;
	unsigned long addr;
	int ret;

	if ((argc < 1) || (argc > 3))
		return -1;
	debug("%s argv0 %s argv1 %s\n", __func__, argv[0], argv[1]);

	blk = 0;
	if (argc == 1) {
		env1 = getenv("kernel_addr");
		env2 = getenv("filesize");
	} else {
		if (!argv[1] || !argv[2])
			return -1;
		env1 = argv[1];
		env2 = argv[2];
	}
	debug("%s kernel_addr %s filesize %s\n", __func__, env1, env2);

	ret = strict_strtoul(env1, 16, &addr);
	if (ret)
		return -1;
	debug("%s addr %ld\n", __func__, addr);

	ret = strict_strtoul(env2, 16, &len);
	if (ret)
		return -1;
	debug("%s len %ld\n", __func__, len);
	if (len % 512)
		len = len / 512 + 1;
	else
		len /= 512;
	debug("%s len %ld\n", __func__, len);

	if ((blk + 512 * len) > 0x1000000) {
		printf("\nBoot Image size exceeding 16MB\n");
		return 1;
	}

	if (validate_bootimg_header(addr)) {
		printf("\nNo valid boot image header found\n");
		return 1;
	}

	if (curr_device < 0) {
		if (get_mmc_num() > 0) {
			curr_device = 0;
		} else {
			puts("No MMC device available\n");
			return CMD_RET_FAILURE;
		}
	}

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, mmc->block_dev.blksz);
	n = mmc->block_dev.block_read(&mmc->block_dev, 0, 1, buffer);
	if (n != 1) {
		printf("ERROR: read partition table failed\n");
		return 1;
	}

	if (validate_partition_table(buffer)) {
		printf("Invalid partition setup, can't write bootimg\n");
		return 1;
	}

	printf("\nMMC write: dev # %d, block # %ld, count %ld ... ",
	       curr_device, blk, len);

	if (mmc_getwp(mmc) == 1) {
		printf("Error: card is write protected!\n");
		return CMD_RET_FAILURE;
	}

	n = mmc->block_dev.block_write(&mmc->block_dev, blk, len, (void *)addr);
	printf("%ld blocks written: %s\n", n, (n == len) ? "OK" : "ERROR");
	if (n != len)
		return CMD_RET_FAILURE;

	/* Update partition table with FAT entry of boot image */
	memcpy(&buffer[446], (void *) (addr + 446), 16);

	/* Update partition table with read boot sector */
	n = mmc->block_dev.block_write(&mmc->block_dev, 0, 1, (void *)buffer);
	printf("%ld blocks written: %s\n", n, (n == 1) ? "OK" : "ERROR");
	if (n != 1)
		return CMD_RET_FAILURE;

	return 0;
}

static int do_bootimgup(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd;
	int ret;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];
	--argc;
	++argv;

	if (strcmp(cmd, "spi") == 0)
		ret = do_bootu_spi(argc, argv);
	else if (strcmp(cmd, "mmc") == 0)
		ret = do_bootu_mmc(argc, argv);
	else
		ret = -1;

	if (ret != -1)
		return ret;

usage:
	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	bootimgup, 5, 1, do_bootimgup, "Updates Boot Image",
	" <mmc | spi> [image_address] [image_size] \n"
	" where: \n"
	" spi - updates boot image on spi flash \n"
	" mmc - updates boot image on mmc card/chip \n"
	" image_address - address image is located in RAM \n"
	" image_size    - size of image in hex \n"
	" If image_address, image_size are not specified, \n"
	" then $kernel_addr and $filesize values are used \n"
);
