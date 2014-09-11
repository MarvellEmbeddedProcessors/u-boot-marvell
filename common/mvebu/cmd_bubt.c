/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <config.h>
#include <common.h>
#include <command.h>
#include <vsprintf.h>
#include <errno.h>

#include <asm/arch-mvebu/unit-info.h>

#include <spi_flash.h>

#include <nand.h>

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

struct bubt_dev {
	char name[8];
	int (*read)(const char *file_name);
	int (*write)(int image_size);
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

#ifdef CONFIG_SPI_FLASH
static int spi_burn_image(int image_size)
{
	int ret, erase_size;
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

	/* Erase the U-BOOT image space */
	erase_size = ((image_size / flash->erase_size) + 1) * flash->erase_size;
	printf("Erasing 0x%x - 0x%x:...", 0, erase_size);
	ret = spi_flash_erase(flash, 0, erase_size);
	if (ret) {
		printf("Error!\n");
		goto error;
	}
	printf("Done!\n");

	/* Write the image tp flash */
	printf("Writing image:...");
	ret = spi_flash_write(flash, 0, image_size, (void *)get_load_addr());
	if (ret)
		printf("\t\tError!\n");
	else
		printf("\t\tDone!\n");

error:
#ifdef CONFIG_SPI_FLASH_PROTECTION
	spi_flash_protect(flash, 1);
#endif

	return ret;
}

int is_spi_active(void)
{
	if (unit_info_get_count(SPI_UNIT_ID) == 0) {
		printf("Error: SPI unit is inactive or doesn't exist\n");
		return 0;
	}
	return 1;
}
#else
#define spi_burn_image 0
#define is_spi_active 0
#endif /* CONFIG_SPI_FLASH */

#ifdef CONFIG_CMD_NAND
static int nand_burn_image(int image_size)
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
	printf("Erasing 0x%x - 0x%x:...", 0, image_size);
	ret = nand_erase(nand, 0, (size_t)image_size);
	if (ret) {
		printf("Error!\n");
		goto error;
	}
	printf("Done!\n");

	/* Write the image to flash */
	printf("Writing image:...");
	ret = nand_write(nand, 0, (size_t *)&image_size, (void *)get_load_addr());
	if (ret)
		printf("Error!\n");
	else
		printf("Done!\n");

error:
	return ret;
}

int is_nand_active(void)
{
	if (unit_info_get_count(NAND_UNIT_ID) == 0) {
		printf("Error: NAND unit is inactive or doesn't exist\n");
		return 0;
	}
	return 1;
}
#else
#define nand_burn_image 0
#define is_nand_active 0
#endif /* CONFIG_CMD_NAND */

#ifdef CONFIG_SYS_FLASH_CFI
static int nor_burn_image(int image_size)
{
	return 0;
}

int is_nor_active(void)
{
	if (unit_info_get_count(DEVICE_BUS_UNIT_ID) == 0) {
		printf("Error: Device bus unit is inactive or doesn't exist\n");
		return 0;
	}
	return 1;
}
#else
#define nor_burn_image 0
#define is_nor_active 0
#endif /* CONFIG_SYS_FLASH_CFI */

#ifdef CONFIG_USB_STORAGE
static int usb_read_file(const char *file_name)
{
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
	return fs_read(file_name, get_load_addr(), 0, 0);
}

int is_usb_active(void)
{
	int unit_cnt;

	unit_cnt  = unit_info_get_count(USB_UNIT_ID);
	unit_cnt += unit_info_get_count(USB3_UNIT_ID);
	if (!unit_cnt) {
		printf("Error: USB unit is inactive or doesn't exist\n");
		return 0;
	}
	return 1;
}
#else
#define usb_read_file 0
#define is_usb_active 0
#endif /* CONFIG_USB_STORAGE */

#ifdef CONFIG_CMD_NET
static int tftp_read_file(const char *file_name)
{
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

#define BUBT_MAX_DEV 5

struct bubt_dev bubt_devs[BUBT_MAX_DEV] = {
	{"tftp", tftp_read_file, NULL, is_tftp_active},
	{"usb",  usb_read_file,  NULL, is_usb_active},
	{"spi",  NULL, spi_burn_image,  is_spi_active},
	{"nand", NULL, nand_burn_image, is_nand_active},
	{"nor",  NULL, nor_burn_image,  is_nor_active}
};

static int bubt_write_file(struct bubt_dev *dst, int image_size)
{
	if (!dst->write) {
		printf("Error: Write not supported on device %s\n", dst->name);
		return 1;
	}

	return dst->write(image_size);
}

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

static int bubt_verify(int image_size)
{
	/* Check that image doesn't overflow allocated space */
	if (image_size > CONFIG_UBOOT_SIZE) {
		printf("Error: Image size %d larget than limit %d\n", image_size, CONFIG_UBOOT_SIZE);
		return -EFBIG;
	}

	/* Check a correct image header exists */
	if (check_image_header()) {
		printf("Error: Image header is bad\n");
		return 1;
	}
	return 0;
}


static int bubt_read_file(struct bubt_dev *src)
{
	int image_size;

	if (!src->read) {
		printf("Error: Read not supported on device \"%s\"\n", src->name);
		return 1;
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

#ifdef CONFIG_MVEBU_SPI_BOOT
#define DEFAULT_BUBT_DST "spi"
#elif defined(CONFIG_MVEBU_NAND_BOOT)
#define DEFAULT_BUBT_DST "nand"
#elif defined(CONFIG_MVEBU_NOR_BOOT)
#define DEFAULT_BUBT_DST "nor"
#else
#define DEFAULT_BUBT_DST "error"
#endif

int do_bubt_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	struct bubt_dev *src, *dst;
	int image_size;
	char src_dev_name[8];
	char dst_dev_name[8];

	if (argc < 2)
		copy_filename(BootFile, "u-boot.bin", sizeof(BootFile));
	else
		copy_filename(BootFile, argv[1], sizeof(BootFile));

	if (argc >= 3)
		strncpy(dst_dev_name, argv[2], 8);
	else
		strncpy(dst_dev_name, DEFAULT_BUBT_DST, 8);

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
	bubt, 4, 1, do_bubt_cmd,
	"Burn a u-boot image to flash",
	"[file-name] [destination [source]]\n"
	"\t-file-name     The image file name to burn. default = u-boot.bin\n"
	"\t-destination   Flash to burn to [spi, nor, nand]. defualt = active flash\n"
	"\t-source        The source to load image from [tftp, usb]. default = tftp\n"
	"Examples:\n"
	"\tbubt - Burn u-boot.bin from tftp to active flash\n"
	"\tbubt latest-spi.bin nand - Burn latest-spi.bin from tftp to NAND flash\n"
	"\tbubt backup-nor.bin nor usb - Burn backup-nor.bin from usb to NOR flash\n"

);
