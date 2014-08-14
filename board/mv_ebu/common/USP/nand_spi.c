#define UBOOT_CODE
#ifdef UBOOT_CODE

#define debug_spi printf

#include <common.h>
#include <malloc.h>
#include <nand.h>
#include <spi.h>
#include <spi_flash.h>
#include <nand_spi.h>
#include "../../../../drivers/mtd/spi/spi_flash_internal.h"  /*TODO:change it to related path*/


static void nand_spi_init(struct nand_chip *nand, struct nand_spi *nspi);
static int nand_spi_enable(struct nand_spi *nspi);
static void nand_spi_select_chip(struct mtd_info *mtd, int chip);
static void nand_spi_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr);

static void nand_spi_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len);
static void nand_spi_cmd_write(struct nand_spi *nspi);
static void nand_spi_cmd_read(struct nand_spi *nspi);

static void nand_spi_cmd_read_oob(struct nand_spi *nspi);
static void nand_spi_cmd_erase(struct nand_spi *nspi);
static void nand_spi_cmd_status(struct nand_spi *nspi);

static void nand_spi_cmd_seqin(struct nand_spi *nspi);
static void nand_spi_cmd_read_id(struct nand_spi *nspi);

static int  nand_spi_waitfunc(struct mtd_info *mtd, struct nand_chip *nspi);
static void nand_spi_ecc_hwctl(struct mtd_info *mtd, int mode);
static int  nand_spi_ecc_calculate(struct mtd_info *mtd, const uint8_t *dat, uint8_t *ecc_code);
static int  nand_spi_ecc_correct(struct mtd_info *mtd, uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc);

static uint8_t nand_spi_read_byte(struct mtd_info *mtd);
static u16     nand_spi_read_word(struct mtd_info *mtd);
static void    nand_spi_read_buf(struct mtd_info *mtd, uint8_t *buf, int len);

static uint8_t bbt_pattern = { 0xff };


static struct nand_bbt_descr nand_spi_bbt = {
	.options = 0,
	.offs = 0,
	.len = 1,
	.pattern = &bbt_pattern,
};

static struct nand_ecclayout nand_spi_ecc = {
	.eccbytes = 16,
	.eccpos = {
			12, 13, 14, 15,
			28, 29, 30, 31,
			44, 45, 46, 47,
			60, 61, 62, 63},
	.oobfree = { {1, 3}, {4, 8}, {16, 4}, {20, 8}, {32, 4}, {36, 8}, {48, 4}, {52, 8} }
};

int board_nand_init(struct nand_chip *nand)
{
	struct nand_spi *nspi;
	nspi = malloc(sizeof(struct nand_spi));

	nand_spi_init(nand, nspi);
	nand->priv = nspi;
	return 0;
}

static void nand_spi_init(struct nand_chip *nand, struct nand_spi *nspi)
{
	nand->num_devs       = 1;
	nand->waitfunc       = nand_spi_waitfunc;
	nand->dev_ready      = 0;

	nand->cmdfunc       = nand_spi_cmdfunc;
	nand->select_chip   = nand_spi_select_chip;
	nand->read_byte     = nand_spi_read_byte;

	nand->read_word     = nand_spi_read_word;
	nand->read_buf      = nand_spi_read_buf;
	nand->write_buf     = nand_spi_write_buf;
	nand->verify_buf    = 0;
	nand->block_markbad = 0;

	nand->ecc.mode      = NAND_ECC_HW;
	nand->ecc.hwctl     = nand_spi_ecc_hwctl;
	nand->ecc.calculate = nand_spi_ecc_calculate;
	nand->ecc.correct   = nand_spi_ecc_correct;

/* Hardware generates ECC per 512 Bytes, 4 Bytes per step */
	nand->ecc.size      = 512;
	nand->ecc.bytes     = 4;
	nand->ecc.layout    = &nand_spi_ecc;

	nand->bbt_td           = 0;
	nand->bbt_md           = 0;
	nand->badblock_pattern = &nand_spi_bbt;

	nspi->page_size       = NAND_SPI_PAGE_SIZE;
	nspi->page_per_sector = 64;
	nspi->sector_size     = nspi->page_size * nspi->page_per_sector;

	nspi->data_buf        = malloc(nspi->page_size + NAND_SPI_OOB_SIZE);
	nspi->data_pos        = 0;
	nspi->ecc_status      = 0;

	nspi->spi = spi_setup_slave(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS, CONFIG_ENV_SPI_MAX_HZ, SPI_MODE_3);
	nand_spi_enable(nspi);
}

static u16 nand_spi_read_word(struct mtd_info *mtd)
{
	u16 val;
	struct nand_spi *nspi = (struct nand_spi *)((struct nand_chip *)mtd->priv)->priv;
	val = *((u16 *)(nspi->data_buf + nspi->data_pos));
	nspi->data_pos += sizeof(u16);
	return val;
}

static uint8_t nand_spi_read_byte(struct mtd_info *mtd)
{
	char val;
	struct nand_spi *nspi   = (struct nand_spi *)((struct nand_chip *)mtd->priv)->priv;
	val = *(nspi->data_buf + nspi->data_pos);
	nspi->data_pos++;
	return val;
}

static void nand_spi_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_spi *nspi = (struct nand_spi *)((struct nand_chip *)mtd->priv)->priv;
	memcpy(buf, nspi->data_buf + nspi->data_pos, len);
	nspi->data_pos += len;
}

static void nand_spi_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct nand_spi *nspi = (struct nand_spi *)((struct nand_chip *)mtd->priv)->priv;
	memcpy(nspi->data_buf + nspi->data_pos, buf, len);
	nspi->data_pos += len;
}

static int nand_spi_wait_ready(struct nand_spi *nspi, unsigned long timeout)
{
	u8 cmd[2], buf[3];
	unsigned long timebase;

	timebase = get_timer(0);
	debug_spi("nand-spi:status register: (cmd 0x%x, %x)", CMD_RDSR, 0xC0);

	cmd[0] = CMD_RDSR;
	cmd[1] = 0xC0; /* Protection status */

	do {
		spi_flash_cmd_read(nspi->spi, cmd, 2, buf, 1);
		debug_spi(" = 0x%x", buf[0]);

		if ((buf[0] & SR_OIP_BIT) == 0)
			break;

	} while (get_timer(timebase) < timeout);


	if ((buf[0] & SR_OIP_BIT) == 0) {
		debug_spi(" (device ready)\n");
		return 0;
	}

	/* Timed out */
	debug_spi(" timed out\n");
	return -1;
}


static void nand_spi_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
	struct nand_spi *nspi = (struct nand_spi *)((struct nand_chip *)mtd->priv)->priv;

	debug_spi("nand-spi:command=0x%x(%d) column=%d page_addr=%d\n", command, command, column, page_addr);
	
	switch (command) {
	case NAND_CMD_SEQIN:
		nspi->column    = column;
		nspi->page_addr = page_addr;
		nand_spi_cmd_seqin(nspi);
		break;
	case NAND_CMD_READOOB:
		nspi->column    = column;
		nspi->page_addr = page_addr;
		nand_spi_cmd_read_oob(nspi);
		break;
	case NAND_CMD_STATUS:
		nand_spi_cmd_status(nspi);
		break;
	case NAND_CMD_READ0:
		nspi->column    = column;
		nspi->page_addr = page_addr;
		nand_spi_cmd_read(nspi);
		break;
	case NAND_CMD_RESET:
		break;
	case NAND_CMD_ERASE1:
		nspi->column    = column;
		nspi->page_addr = page_addr;
		nand_spi_cmd_erase(nspi);
		break;
	case NAND_CMD_READID:
		nand_spi_cmd_read_id(nspi);
		break;
	case NAND_CMD_PAGEPROG:
		nand_spi_cmd_write(nspi);
		break;
	}
}

static void nand_spi_ecc_hwctl(struct mtd_info *mtd, int mode)
{
}

static int nand_spi_ecc_calculate(struct mtd_info *mtd, const uint8_t *dat, uint8_t *ecc_code)
{
	return 0;
}

static int nand_spi_waitfunc(struct mtd_info *mtd, struct nand_chip *nspi)
{
	return 0;
}

static void nand_spi_select_chip(struct mtd_info *mtd, int chip)
{
	int ret;
	struct nand_spi *nspi = (struct nand_spi *)((struct nand_chip *)mtd->priv)->priv;

	/*spi_cs_activate(nspi->spi);*/

	ret = spi_flash_cmd(nspi->spi, CMD_WREN, NULL, 0);
	if (ret < 0) {
		debug_spi("nand-spi:enabling write failed\n");
	}
}

static int nand_spi_ecc_correct(struct mtd_info *mtd, uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc)
{
	/*
	 * ecc_status: 
	 * 00b = No bit errors detected during last read opeartion 
	 * 01b = bit error was detected and corrected, error bit number = 1~7
	 * 10b = bit error was detected and bot corrected
	 * 11b = bit error was detected and corrected, error bit number = 8
	 */

	struct nand_spi *nspi = (struct nand_spi *)((struct nand_chip *)mtd->priv)->priv;
	if(nspi->ecc_status == ECC_ERR_NOT_CORRECTED)
		return -1;
	
	return (nspi->ecc_status >> 1);
}

static int nand_spi_enable(struct nand_spi *nspi)
{
	int ret;
	u8 cmd[3];

	/* Write Enable */
	ret = spi_flash_cmd(nspi->spi, CMD_WREN, NULL, 0);
	if(ret < 0) {
		debug_spi("nand-spi:enabling write failed\n");
		return ret;
	}

	/* Set Features : Status register */
	cmd[0] = CMD_WRSR;
	cmd[1] = 0xA0;
	cmd[2] = 0x0;
	ret = spi_flash_cmd_write(nspi->spi, cmd, 3, NULL, 0);
	if(ret < 0) {
		debug_spi("nand-spi:write status register failed\n");
		return ret;
	}

	/* Enable ECC                     */
	/* Set Features : Status register */
	cmd[0] = CMD_WRSR;
	cmd[1] = 0xB0;
	cmd[2] = ECC_EN_BIT;
	ret = spi_flash_cmd_write(nspi->spi, cmd, 3, NULL, 0);
	if (ret < 0) {
		debug_spi("nand-spi:write status register failed\n");
		return ret;
	}

	return 0;
}

static void nand_spi_cmd_seqin(struct nand_spi *nspi)
{
	nspi->data_pos = 0;
}

static void nand_spi_cmd_read_id(struct nand_spi *nspi)
{
	int ret;
	u8 idcode[8];
	
	nspi->data_pos = 0;
	memset(idcode, 0, sizeof(idcode));
	ret = spi_flash_cmd(nspi->spi, CMD_READ_ID, idcode, sizeof(idcode));
	if(ret) {
		goto err_read_id;
	}
	memcpy(nspi->data_buf, idcode, sizeof(idcode));
	return;

err_read_id:
	spi_free_slave(nspi->spi);
}

static void nand_spi_cmd_status(struct nand_spi *nspi)
{
	u8 cmd[3], buf[1];

	/* Get Features : protection register */
	cmd[0] = CMD_RDSR;
	cmd[1] = 0xA0; /* Protection status */
	spi_flash_cmd_read(nspi->spi, cmd, 2, buf, 1);
	debug_spi("nand-spi:get features:\nprotection register (command 0x%x, %x), value=0x%x\n", 
			  CMD_RDSR, 0xA0, buf[0]);

	/* Get Features : feature register */
	cmd[0] = CMD_RDSR;
	cmd[1] = 0xB0; /* Protection status */
	spi_flash_cmd_read(nspi->spi, cmd, 2, buf, 3);
	debug_spi("feature register (command 0x%x, %x), value=0x%x\n", CMD_RDSR, 0xB0, buf[0]);

	/* Get Features : status register */
	cmd[0] = CMD_RDSR;
	cmd[1] = 0xC0; /* Protection status */
	spi_flash_cmd_read(nspi->spi, cmd, 2, buf, 3);
	debug_spi("status register (command 0x%x, %x), value=0x%x\n", CMD_RDSR, 0xC0, buf[0]);

	/* The nand stack check if the device is in write protected state by test Bit-7 (0x80) */
	/*   while NAND-SPI use Bit-1 (0x02) to indicates write protected state                */
	if(buf[0] & WP_BIT) {
		buf[0] |= 0x80;
	}

	memcpy(nspi->data_buf, buf, sizeof(buf));
	nspi->data_pos = 0;
}

static void nand_spi_cmd_write(struct nand_spi *nspi)
{
	unsigned long page_num, sector_num;
	u8 cmd[4];
	int ret = 0, len;

	len = nspi->data_pos;

	/* Enable ECC                     */
	/* Set Features : Status register */
	cmd[0] = CMD_WRSR;
	cmd[1] = 0xB0;
	cmd[2] = ECC_EN_BIT;
	cmd[2] = 0x10;
	ret = spi_flash_cmd_write(nspi->spi, cmd, 3, NULL, 0);
	if (ret < 0) {
		debug_spi("nand-spi:write status register failed\n");
		return;
	}

	page_num   = nspi->page_addr;
	sector_num = page_num >> 6;

	/* PROGRAM LOAD: Write page to Cache */
	cmd[0] = CMD_PP_TO_CACHE;
	cmd[1] = 0;
	cmd[2] = 0;

	ret = spi_flash_cmd_write(nspi->spi, cmd, 3, nspi->data_buf, len);

	/* Write Enable */
	ret = spi_flash_cmd(nspi->spi, CMD_WREN, NULL, 0);
	if(ret < 0) {
		debug_spi("nand-spi:enabling Write failed\n");
		return;
	}

	/* Read status - wait for device to finish transaction */
	ret = nand_spi_wait_ready(nspi, SPI_FLASH_PROG_TIMEOUT);
	if (ret < 0) {
		debug_spi("nand-spi:page programming timed out\n");
	}

	/* Program Execute: Write page from Cache to flash    */
	/* command requires a 24-bit address consisting of:   */
	/* - 8 dummy bits                                     */
	/* - 10 bits of sector number                         */
	/* - 6 bit of page number                             */
	cmd[0]  = CMD_PROGRAM_EXECUTE;
	cmd[1]  = 0;				        /* Dummy */
	cmd[2]  = sector_num >> 2;		    /* 8 bits sector number (8 MSB bits)                  */
	cmd[3]  = page_num & 0x3f;		    /* 6 LSB bits of cmd[0] = page address                */
	cmd[3] |= (sector_num & 0x3) << 6;	/* 2 LSB bits of cmd[0]= 1st 2 bits of sector address */

	ret = spi_flash_cmd_write(nspi->spi, cmd, 4, NULL, 0);
	if (ret < 0) {
		debug_spi("nand-spi:program execute to flash failed\n");
		return;
	}

	/* Read status - wait for device to finish transaction */
	ret = nand_spi_wait_ready(nspi, SPI_FLASH_PROG_TIMEOUT);
	if (ret < 0) {
		debug_spi("nand-spi:page programming timed out\n");
		return;
	}

	printf("nand-spi:Successfully programmed %u bytes\n", len);

	/*spi_release_bus(nspi->spi);*/
}

static void nand_spi_cmd_read(struct nand_spi *nspi)
{
	char buf[3];
	unsigned long page_num, sector_num;
	u8 cmd[5];
	int ret = -1, len;

	len = NAND_SPI_PAGE_SIZE + NAND_SPI_OOB_SIZE;
	page_num = nspi->page_addr;


	/*64 pages per sector*/
	sector_num = (page_num >> 6);

	/* Read page from Flash to Cache:                  */
	/* command requires a 24-bit address consisting of */
	/* - 8 dummy bits                                  */
	/* - 10 bits of sector number                      */
	/* - 6 bit of page number                          */
	cmd[0] = CMD_READ_TO_CACHE;
	cmd[1] = 0;
	cmd[2] = sector_num >> 2;
	cmd[3] = page_num & 0x3f;		    /* 6 LSB bits */
	cmd[3] |= (sector_num & 0x3) << 6;	/* 2 MSB bits */

	ret = spi_flash_cmd_write(nspi->spi, cmd, 4, NULL, 0);
	if (ret < 0) {
		debug_spi("nand-spi:read to cache failed\n");
		return;
	}

	/* Read status - wait for device to finish transaction */
	ret = nand_spi_wait_ready(nspi, SPI_FLASH_PROG_TIMEOUT);
	if (ret < 0) {
		debug_spi("nand-spi:page programming timed out\n");
		return;
	}

	/* Read first page from Cache */
	cmd[0] = CMD_READ_FROM_CACHE;
	cmd[1] = 0x0;
	cmd[2] = 0x0;
	cmd[3] = 0x0;

	ret = spi_flash_cmd_read(nspi->spi, cmd, 4, nspi->data_buf, len);
	if (ret < 0) {
		debug_spi("nand-spi:read from cache failed\n");
		return;
	}
	nspi->data_pos = 0;

	/* Get Features : Status register */
	cmd[0] = CMD_RDSR;
	cmd[1] = 0xC0; /* status */
	spi_flash_cmd_read(nspi->spi, cmd, 2, buf, 3);

	nspi->ecc_status = (buf[0] & 0x30) >> 4;
	debug_spi("nand-spi:read sector=%d page=%d. ecc reg=0x%x\n", sector_num, page_num, nspi->ecc_status);
}

static void nand_spi_cmd_read_oob(struct nand_spi *nspi)
{
	char buf[3];
	unsigned long page_num, sector_num;
	u8 cmd[5];
	int ret = -1, len;

	len = NAND_SPI_OOB_SIZE;
	page_num = nspi->page_addr;

	/*64 pages per sector*/
	sector_num = (page_num >> 6);
	debug_spi("nand-spi:read_oob operation. sector=%lu, page=%lu\n", sector_num, page_num);
	
	/* Read page from Flash to Cache:
	 * command requires a 24-bit address consisting of
	 * - 8 dummy bits
	 * - 10 bits of sector number
	 * - 6 bit of page number */

	cmd[0] = CMD_READ_TO_CACHE;
	cmd[1] = 0;
	cmd[2] = sector_num >> 2;
	cmd[3] = page_num & 0x3f;             /* 6 LSB bits */
	cmd[3] |= (sector_num & 0x3) << 6;    /* 2 MSB bits */

	ret = spi_flash_cmd_write(nspi->spi, cmd, 4, NULL, 0);
	if (ret < 0) {
		debug_spi("nand-spi:read to cache failed\n");
		return;
	}

	/* Read status - wait for device to finish transaction */
	ret = nand_spi_wait_ready(nspi, SPI_FLASH_PROG_TIMEOUT);
	if (ret < 0) {
		debug_spi("nand-spi:page programming timed out\n");
		return;
	}

	/* Read oob data from Cache */
	cmd[0]  = CMD_READ_FROM_CACHE;
	cmd[1]  = 0x80;   /*64-len for oob area                         */
	cmd[1] |= 0x08;   /*offset(2-bits MSB) 2048 for OOB data (0x800)*/
	cmd[2]  = 0x0;    /*offset(4-bits-LSB) 2048 for OOB data (0x800)*/
	cmd[3]  = 0x0;

	ret = spi_flash_cmd_read(nspi->spi, cmd, 4, nspi->data_buf, len);
	if (ret < 0) {
		debug_spi("nand-spi:read_oob from cache failed\n");
		return;
	}

	nspi->data_pos = 0;
	
	/* Get Features : Status register */
	cmd[0] = CMD_RDSR;
	cmd[1] = 0xC0; /* status */
	spi_flash_cmd_read(nspi->spi, cmd, 2, buf, 3);

	nspi->ecc_status = ((buf[0] & 0x30) >> 4);
	debug_spi("nand-spi:read_oob sector=%d page=%d. ecc reg=0x%x\n", sector_num, page_num, nspi->ecc_status);
}

static void nand_spi_cmd_erase(struct nand_spi *nspi)
{
	unsigned long sector_num;
	int ret = 0;
	u8 cmd[4];

	/*page_addr indicates the number of the first page in erased block*/
	sector_num = nspi->page_addr >> 6;
	cmd[0] = CMD_SECTOR_ERASE;
	cmd[1] = 0x00; /* Dummy */

	/* command requires a 24-bit address consisting of:
	 * - 8 dummy bits
	 * - 10 bits of sector number
	 * - 6 bit of page number */
	cmd[2] = sector_num >> 2;              /* 6 MSB bits */
	cmd[3] = (sector_num & 0x3) << 6;      /* 2 LSB bits */
	debug_spi("nand-spi:erase. sector_num=%lu \n", sector_num);

	ret = spi_flash_cmd(nspi->spi, CMD_WREN, NULL, 0);
	if (ret < 0) {
		debug_spi("nand-spi:erase. enable write failed\n");
		/*break'*/
		return;
	}

	/* Read status - wait for device to finish transaction */
	ret = nand_spi_wait_ready(nspi, SPI_FLASH_PROG_TIMEOUT);
	if (ret < 0) {
		debug_spi("nand-spi:sector programming timed out\n");
		return;
	}

	ret = spi_flash_cmd_write(nspi->spi, cmd, 4, NULL, 0);
	if (ret < 0) {
		debug_spi("nand-spi:sector erase failed. sector=0x%lx\n", sector_num);
		return;
	}

	/* Read status - wait for device to finish transaction */
	ret = nand_spi_wait_ready(nspi, SPI_FLASH_PAGE_ERASE_TIMEOUT);
	if (ret < 0) {
		debug_spi("nand-spi:sector erase timed out. sector=0x%lx\n", sector_num);
		return;
	}

	debug_spi("nand-spi:erased sector=0x%lx\n", sector_num);
}

#endif

