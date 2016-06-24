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

#include <config.h>
#include <common.h>
#include <command.h>
#include <vsprintf.h>
#include <errno.h>

#include <spi_flash.h>
#include <spi.h>

#include <nand.h>

#include <mvebu_chip_sar.h>

#include <usb.h>
#include <fs.h>
#include <mmc.h>

#if defined(CONFIG_TARGET_ARMADA_8K)
#define MAIN_HDR_MAGIC		0xB105B002

struct mvebu_image_header {
	uint32_t	magic;			/*  0-3  */
	uint32_t	prolog_size;		/*  4-7  */
	uint32_t	prolog_checksum;	/*  8-11 */
	uint32_t	boot_image_size;	/* 12-15 */
	uint32_t	boot_image_checksum;	/* 16-19 */
	uint32_t	rsrvd0;			/* 20-23 */
	uint32_t	load_addr;		/* 24-27 */
	uint32_t	exec_addr;		/* 28-31 */
	uint8_t		uart_cfg;		/*  32   */
	uint8_t		baudrate;		/*  33   */
	uint8_t		ext_count;		/*  34   */
	uint8_t		aux_flags;		/*  35   */
	uint32_t	io_arg_0;		/* 36-39 */
	uint32_t	io_arg_1;		/* 40-43 */
	uint32_t	io_arg_2;		/* 43-47 */
	uint32_t	io_arg_3;		/* 48-51 */
	uint32_t	rsrvd1;			/* 52-55 */
	uint32_t	rsrvd2;			/* 56-59 */
	uint32_t	rsrvd3;			/* 60-63 */
};
#else /* A38x */
struct mvebu_image_header {
	u8	block_id;		/* 0 */
	u8	rsvd1;			/* 1 */
	u16	nand_page_size;		/* 2-3 */
	u32	block_size;		/* 4-7 */
	u8	version;		/* 8 */
	u8	hdr_size_msb;		/* 9 */
	u16	hdr_size_lsb;		/* 10-11 */
	u32	source_addr;		/* 12-15 */
	u32	dest_addr;		/* 16-19 */
	u32	exec_addr;		/* 20-23 */
	u8	rsvd3;			/* 24 */
	u8	nand_blk_size;		/* 25 */
	u8	nand_technology;	/* 26 */
	u8	rsvd4;			/* 27 */
	u16	rsvd2;			/* 28-29 */
	u8	ext;			/* 30 */
	u8	checksum;		/* 31 */
};
#endif

struct bubt_dev {
	char name[8];
	size_t (*read)(const char *file_name);
	int (*write)(size_t image_size);
	int (*active)(void);
};

static ulong get_load_addr(void)
{
	const char *addr_str;
	unsigned long addr;

	addr_str = getenv("loadaddr");
	if (addr_str != NULL)
		addr = simple_strtoul(addr_str, NULL, 16);
	else
		addr = CONFIG_SYS_LOAD_ADDR;

	return addr;
}

/********************************************************************
 *     eMMC services
 ********************************************************************/
#ifdef CONFIG_GENERIC_MMC
static int mmc_burn_image(size_t image_size)
{
	struct mmc	*mmc;
	lbaint_t	start_lba;
	lbaint_t	blk_count;
	ulong		blk_written;
#ifdef CONFIG_SYS_MMC_ENV_DEV
	const u8	mmc_dev_num = CONFIG_SYS_MMC_ENV_DEV;
#else
	const u8	mmc_dev_num = 0;
#endif

	mmc = find_mmc_device(mmc_dev_num);
	if (!mmc) {
		printf("No SD/MMC/eMMC card found\n");
		return 1;
	}

	if (mmc_init(mmc)) {
		printf("%s(%d) init failed\n", IS_SD(mmc) ? "SD" : "MMC", mmc_dev_num);
		return 1;
	}

#ifdef CONFIG_SYS_MMC_ENV_PART
	if (CONFIG_SYS_MMC_ENV_PART != mmc->part_num) {
		if (mmc_switch_part(mmc_dev_num, CONFIG_SYS_MMC_ENV_PART)) {
			printf("MMC partition switch failed\n");
			return 1;
		}
	}
#endif

	/* SD reserves LBA-0 for MBR and boots from LBA-1, MMC/eMMC boots from LBA-0 */
	start_lba = IS_SD(mmc) ? 1 : 0;
	blk_count = image_size / mmc->block_dev.blksz;
	if (image_size % mmc->block_dev.blksz)
		blk_count += 1;

	blk_written = mmc->block_dev.block_write(mmc_dev_num,
						start_lba, blk_count, (void *)get_load_addr());
	if (blk_written != blk_count) {
		printf("Error - written %#lx blocks\n", blk_written);
		return 1;
	} else {
		printf("Done!\n");
	}

#ifdef CONFIG_SYS_MMC_ENV_PART
	if (CONFIG_SYS_MMC_ENV_PART != mmc->part_num)
		mmc_switch_part(mmc_dev_num, mmc->part_num);
#endif

	return 0;
}

static size_t mmc_read_file(const char *file_name)
{
	loff_t act_read = 0;
	int rc;
	struct mmc	*mmc;
#ifdef CONFIG_SYS_MMC_ENV_DEV
	const u8	mmc_dev_num = CONFIG_SYS_MMC_ENV_DEV;
#else
	const u8	mmc_dev_num = 0;
#endif

	mmc = find_mmc_device(mmc_dev_num);
	if (!mmc) {
		printf("No SD/MMC/eMMC card found\n");
		return 0;
	}

	if (mmc_init(mmc)) {
		printf("%s(%d) init failed\n", IS_SD(mmc) ? "SD" : "MMC", mmc_dev_num);
		return 0;
	}

	/* Load from data partition (0) */
	if (fs_set_blk_dev("mmc", "0", FS_TYPE_ANY)) {
		printf("Error: MMC 0 not found\n");
		return 0;
	}

	/* Perfrom file read */
	rc = fs_read(file_name, get_load_addr(), 0, 0, &act_read);
	if (rc)
		return 0;

	return act_read;
}

int is_mmc_active(void)
{
	return 1;
}
#else
#define mmc_burn_image 0
#define mmc_read_file 0
#define is_mmc_active 0
#endif /* CONFIG_GENERIC_MMC */


/********************************************************************
 *     SPI services
 ********************************************************************/
#ifdef CONFIG_SPI_FLASH
static int spi_burn_image(size_t image_size)
{
	int ret;
	struct spi_flash *flash;

	/* Probe the SPI bus to get the flash device */
	flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS,
				CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
	if (!flash) {
		printf("Failed to probe SPI Flash\n");
		return 1;
	}

#ifdef CONFIG_SPI_FLASH_PROTECTION
	spi_flash_protect(flash, 0);
#endif

	ret = spi_flash_update(flash, 0, image_size, (void *)get_load_addr());
	if (ret)
		printf("Error!\n");
	else
		printf("Done!\n");

#ifdef CONFIG_SPI_FLASH_PROTECTION
	spi_flash_protect(flash, 1);
#endif

	return ret;
}

int is_spi_active(void)
{
	return 1;
}
#else
#define spi_burn_image 0
#define is_spi_active 0
#endif /* CONFIG_SPI_FLASH */

/********************************************************************
 *     NAND services
 ********************************************************************/
#ifdef CONFIG_CMD_NAND
static int nand_burn_image(size_t image_size)
{
	int ret, block_size;
	nand_info_t *nand;
	int dev = nand_curr_device;

	if ((dev < 0) || (dev >= CONFIG_SYS_MAX_NAND_DEVICE) ||
	    (!nand_info[dev].name)) {
		puts("\nno devices available\n");
		return 1;
	}
	nand = &nand_info[dev];
	block_size = nand->erasesize;

	/* Align U-Boot size to currently used blocksize */
	image_size = ((image_size + (block_size - 1)) & (~(block_size-1)));

	/* Erase the U-BOOT image space */
	printf("Erasing 0x%x - 0x%x:...", 0, (int)image_size);
	ret = nand_erase(nand, 0, image_size);
	if (ret) {
		printf("Error!\n");
		goto error;
	}
	printf("Done!\n");

	/* Write the image to flash */
	printf("Writing image:...");
	printf("&image_size = 0x%p\n", (void*)&image_size);
	ret = nand_write(nand, 0, &image_size, (void *)get_load_addr());
	if (ret)
		printf("Error!\n");
	else
		printf("Done!\n");

error:
	return ret;
}

int is_nand_active(void)
{
	return 1;
}
#else
#define nand_burn_image 0
#define is_nand_active 0
#endif /* CONFIG_CMD_NAND */

/********************************************************************
 *     NOR services
 ********************************************************************/
#ifdef CONFIG_SYS_FLASH_CFI
static int nor_burn_image(size_t image_size)
{
	return 0;
}

int is_nor_active(void)
{
	return 1;
}
#else
#define nor_burn_image 0
#define is_nor_active 0
#endif /* CONFIG_SYS_FLASH_CFI */

/********************************************************************
 *     USB services
 ********************************************************************/
#ifdef CONFIG_USB_STORAGE
static size_t usb_read_file(const char *file_name)
{
	loff_t act_read = 0;
	int rc;

	usb_stop();

	if (usb_init() < 0) {
		printf("Error: usb_init failed\n");
		return 0;
	}

	/* Try to recognize storage devices immediately */
	if (-1 == usb_stor_scan(1)) {
		printf("Error: USB storage device not found\n");
		return 0;
	}

	/* Always load from usb 0 */
	if (fs_set_blk_dev("usb", "0", FS_TYPE_ANY)) {
		printf("Error: USB 0 not found\n");
		return 0;
	}

	/* Perfrom file read */
	rc = fs_read(file_name, get_load_addr(), 0, 0, &act_read);
	if (rc)
		return 0;

	return act_read;
}

int is_usb_active(void)
{
	return 1;
}
#else
#define usb_read_file 0
#define is_usb_active 0
#endif /* CONFIG_USB_STORAGE */

/********************************************************************
 *     Network services
 ********************************************************************/
#ifdef CONFIG_CMD_NET
static size_t tftp_read_file(const char *file_name)
{
	/* update global variable load_addr before tftp file from network */
	load_addr = get_load_addr();
	return NetLoop(TFTPGET);
}

int is_tftp_active(void)
{
	return 1;
}
#else
#define tftp_read_file 0
#define is_tftp_active 0
#endif /* CONFIG_CMD_NET */

enum bubt_devices {
	BUBT_DEV_NET = 0,
	BUBT_DEV_USB,
	BUBT_DEV_MMC,
	BUBT_DEV_SPI,
	BUBT_DEV_NAND,
	BUBT_DEV_NOR,

	BUBT_MAX_DEV
};

struct bubt_dev bubt_devs[BUBT_MAX_DEV] = {
	{"tftp", tftp_read_file, NULL, is_tftp_active},
	{"usb",  usb_read_file,  NULL, is_usb_active},
	{"mmc",  mmc_read_file,  mmc_burn_image, is_mmc_active},
	{"spi",  NULL, spi_burn_image,  is_spi_active},
	{"nand", NULL, nand_burn_image, is_nand_active},
	{"nor",  NULL, nor_burn_image,  is_nor_active}
};

static int bubt_write_file(struct bubt_dev *dst, size_t image_size)
{
	if (!dst->write) {
		printf("Error: Write not supported on device %s\n", dst->name);
		return 1;
	}

	return dst->write(image_size);
}

#if defined(CONFIG_TARGET_ARMADA_8K)
uint32_t do_checksum32(uint32_t *start, uint32_t len)
{
	uint32_t sum = 0;
	uint32_t *startp = start;

	do {
		sum += *startp;
		startp++;
		len -= 4;
	} while (len > 0);

	return sum;
}

static int check_image_header(void)
{
	struct mvebu_image_header *hdr = (struct mvebu_image_header *)get_load_addr();
	uint32_t header_len = hdr->prolog_size;
	uint32_t checksum;
	uint32_t checksum_ref = hdr->prolog_checksum;

	/*
	 * For now compare checksum, and magic. Later we can
	 * verify more stuff on the header like interface type, etc
	 */
	if (hdr->magic != MAIN_HDR_MAGIC) {
		printf("ERROR: Bad MAGIC 0x%08x != 0x%08x\n", hdr->magic, MAIN_HDR_MAGIC);
		return -ENOEXEC;
	}

	/* The checksum value is discarded from checksum calculation */
	hdr->prolog_checksum = 0;

	checksum = do_checksum32((uint32_t *)hdr, header_len);
	if (checksum != checksum_ref) {
		printf("Error: Bad Image checksum. 0x%x != 0x%x\n", checksum, checksum_ref);
		return -ENOEXEC;
	}

	/* Restore the checksum before writing */
	hdr->prolog_checksum = checksum_ref;
	printf("Image checksum...OK!\n");

	return 0;
}
#elif defined(CONFIG_TARGET_ARMADA_38X) /* A38x */
u8 do_checksum8(void *start, u32 len, u8 csum)
{
	register u8 sum = csum;
	u8 *byte = (u8 *)start;

	do {
		sum += *byte;
		byte++;

	} while (--len);

	return sum;
}

static int check_image_header(void)
{
	struct mvebu_image_header *hdr = (struct mvebu_image_header *)get_load_addr();
	u32 header_len = (hdr->hdr_size_msb << 16) + hdr->hdr_size_lsb;
	u8 checksum;
	u8 checksum_ref = hdr->checksum;

	/*
	 * For now just compare checksum. Later we can
	 * verify more stuff on the header like interface type, etc
	 */

	/* The checksum value is discarded from checksum calculation */
	hdr->checksum = 0;

	checksum = do_checksum8(hdr, header_len, 0);
	if (checksum != checksum_ref) {
		printf("Error: Bad Image checksum. 0x%x != 0x%x\n", checksum, checksum_ref);
		return -ENOEXEC;
	}

	/* Restore the checksum before writing */
	hdr->checksum = checksum_ref;
	printf("Image checksum...OK!\n");

	return 0;
}
#elif defined(CONFIG_TARGET_ARMADA_3700) /* Armada 3700 */
static int check_image_header(void)
{
	/* Armada3700 has different Image, without mvebu
	  * header at begining.
	  * BootRom will also do the image check, if something
	  * is not right, CM3 would not run the image.
	  */
	/* printf("Image checksum...OK!\n"); */
	return 0;
}
#else
static int check_image_header(void)
{
	printf("bubt cmd does not support this device !\n");
	return -ENOEXEC;
}
#endif

static int bubt_verify(size_t image_size)
{

	/* Check a correct image header exists */
	if (check_image_header()) {
		printf("Error: Image header is bad\n");
		return 1;
	}
	return 0;
}


static int bubt_read_file(struct bubt_dev *src)
{
	size_t image_size;

	if (!src->read) {
		printf("Error: Read not supported on device \"%s\"\n", src->name);
		return 0;
	}

	image_size = src->read(BootFile);
	if (image_size <= 0) {
		printf("Error: Failed to read file %s from %s\n", BootFile, src->name);
		return 0;
	}

	return image_size;
}

static int bubt_is_dev_active(struct bubt_dev *dev)
{
	if (!dev->active) {
		printf("Device \"%s\" not supported by U-BOOT image\n", dev->name);
		return 0;
	}

	if (!dev->active()) {
		printf("Device \"%s\" is inactive\n", dev->name);
		return 0;
	}

	return 1;
}

struct bubt_dev *find_bubt_dev(char *dev_name)
{
	int dev;

	for (dev = 0; dev < BUBT_MAX_DEV; dev++) {
		if (strcmp(bubt_devs[dev].name, dev_name) == 0)
			return &bubt_devs[dev];
	}

	return 0;
}

#define DEFAULT_BUBT_SRC "tftp"

#ifndef DEFAULT_BUBT_DST
#ifdef CONFIG_MVEBU_SPI_BOOT
#define DEFAULT_BUBT_DST "spi"
#elif defined(CONFIG_MVEBU_NAND_BOOT)
#define DEFAULT_BUBT_DST "nand"
#elif defined(CONFIG_MVEBU_NOR_BOOT)
#define DEFAULT_BUBT_DST "nor"
#elif defined(CONFIG_MVEBU_MMC_BOOT)
#define DEFAULT_BUBT_DST "mmc"
#else
#define DEFAULT_BUBT_DST "error"
#endif
#endif /* DEFAULT_BUBT_DST */

int do_bubt_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	struct bubt_dev *src, *dst;
	size_t image_size;
	char src_dev_name[8];
	char dst_dev_name[8];
#ifdef CONFIG_ENV_IS_IN_BOOTDEV
	struct sar_val sar;
#endif
	char *name;

	if (argc < 2)
		copy_filename(BootFile, CONFIG_MVEBU_UBOOT_DFLT_NAME, sizeof(BootFile));
	else
		copy_filename(BootFile, argv[1], sizeof(BootFile));

	if (argc >= 3) {
		strncpy(dst_dev_name, argv[2], 8);
	} else {
#ifdef CONFIG_ENV_IS_IN_BOOTDEV
		mvebu_sar_value_get(SAR_BOOT_SRC, &sar);
		name = mvebu_sar_bootsrc_to_name(sar.bootsrc.type);
#else
		name = DEFAULT_BUBT_DST;
#endif
		strncpy(dst_dev_name, name, 8);
	}

	if (argc >= 4)
		strncpy(src_dev_name, argv[3], 8);
	else
		strncpy(src_dev_name, DEFAULT_BUBT_SRC, 8);

	/* Figure out the destination device */
	dst = find_bubt_dev(dst_dev_name);
	if (!dst) {
		printf("Error: Unknown destination \"%s\"\n", dst_dev_name);
		return 1;
	}

	if (!bubt_is_dev_active(dst))
		return 1;

	/* Figure out the source device */
	src = find_bubt_dev(src_dev_name);
	if (!src) {
		printf("Error: Unknown source \"%s\"\n", src_dev_name);
		return 1;
	}

	if (!bubt_is_dev_active(src))
		return 1;

	printf("Burning U-BOOT image \"%s\" from \"%s\" to \"%s\"\n", BootFile, src->name, dst->name);

	image_size = bubt_read_file(src);
	if (!image_size)
		return 1;

	if (bubt_verify(image_size))
		return 1;

	if (bubt_write_file(dst, image_size))
		return 1;

	return 0;
}

U_BOOT_CMD(
	bubt, 4, 0, do_bubt_cmd,
	"Burn a u-boot image to flash",
	"[file-name] [destination [source]]\n"
	"\t-file-name     The image file name to burn. Default = u-boot.bin\n"
	"\t-destination   Flash to burn to [spi, nor, nand, mmc]. Defualt = active boot device\n"
	"\t-source        The source to load image from [tftp, usb, mmc]. Default = tftp\n"
	"Examples:\n"
	"\tbubt - Burn u-boot.bin from tftp to active boot device\n"
	"\tbubt latest-spi.bin nand - Burn latest-spi.bin from tftp to NAND flash\n"
	"\tbubt backup-nor.bin nor usb - Burn backup-nor.bin from usb to NOR flash\n"

);
