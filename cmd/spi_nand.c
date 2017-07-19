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
#include <malloc.h>
#include <spi.h>
#include <dm-spi-nand.h>
#include <asm/io.h>
#include <dm/device-internal.h>
#include <jffs2/jffs2.h>

static struct spi_nand_chip *chip;
/*
 * This function computes the length argument for the erase command.
 * The length on which the command is to operate can be given in two forms:
 * 1. <cmd> offset len  - operate on <'offset',  'len')
 * 2. <cmd> offset +len - operate on <'offset',  'round_up(len)')
 * If the second form is used and the length doesn't fall on the
 * sector boundary, than it will be adjusted to the next sector boundary.
 * If it isn't in the flash, the function will fail (return -1).
 * Input:
 *    arg: length specification (i.e. both command arguments)
 * Output:
 *    len: computed length for operation
 * Return:
 *    1: success
 *   -1: failure (bad format, bad address).
 */
static int sf_parse_len_arg(char *arg, ulong *len)
{
	char *ep;
	char round_up_len; /* indicates if the "+length" form used */
	ulong len_arg;

	round_up_len = 0;
	if (*arg == '+') {
		round_up_len = 1;
		++arg;
	}

	len_arg = simple_strtoul(arg, &ep, 16);
	if (ep == arg || *ep != '\0')
		return -1;

	if (round_up_len && chip->block_size > 0)
		*len = ROUND(len_arg, chip->block_size);
	else
		*len = len_arg;

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
	if (len >= ((unsigned int) -1) / 1024)
		return len / (max(get_timer(start_ms) / 1024, 1UL));
	else
		return 1024 * len / max(get_timer(start_ms), 1UL);
}

static int do_spi_nand_flash_probe(int argc, char * const argv[])
{
	unsigned int bus = CONFIG_SNF_DEFAULT_BUS;
	unsigned int cs = CONFIG_SNF_DEFAULT_CS;
	char *endp;
	struct udevice *new, *bus_dev;
	int ret;

	if (argc >= 2) {
		cs = simple_strtoul(argv[1], &endp, 0);
		if (*argv[1] == 0 || (*endp != 0 && *endp != ':'))
			return -1;
		if (*endp == ':') {
			if (endp[1] == 0)
				return -1;

			bus = cs;
			cs = simple_strtoul(endp + 1, &endp, 0);
			if (*endp != 0)
				return -1;
		}
	}

	/* Remove the old device, otherwise probe will just be a nop */
	ret = spi_find_bus_and_cs(bus, cs, &bus_dev, &new);
	if (!ret)
		device_remove(new, DM_REMOVE_NORMAL);

	chip = NULL;
	ret = spi_nand_flash_probe_bus_cs(bus, cs, 0, 0, &new);
	if (ret) {
		printf("Failed to initialize SPI flash at %u:%u (error %d)\n",
		       bus, cs, ret);
		return 1;
	}

	chip = dev_get_uclass_priv(new);

	return 0;
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
static int spi_nand_flash_update(struct spi_nand_chip *chip, u32 offset,
		size_t len, const char *buf)
{
	int ret = 0;
	const ulong start_time = get_timer(0);
	ulong delta;

	ret = spi_nand_flash_erase(chip, offset, len, true);
	if (ret) {
		printf("SPI-NAND: %zu bytes @ %#x Erased: ERROR\n",
		       (size_t)len, (u32)offset);
		return ret;
	}
	ret = spi_nand_flash_write(chip, offset, len, buf);
	if (ret)
		printf("SPI-NAND: %zu bytes @ %#x Written: ERROR\n",
		       (size_t)len, (u32)offset);

	delta = get_timer(start_time);
	printf("%zu bytes written in %ld.%lds, speed %ld B/s\n",
	       len, delta / 1000, delta % 1000,
	       bytes_per_second(len, start_time));
	return ret;
}

static int do_spi_nand_flash_read_write(int argc, char * const argv[])
{
	unsigned long addr;
	unsigned long offset;
	unsigned long len;
	void *buf;
	char *endp;
	int ret = 1;

	if (argc < 4)
		return -1;

	addr = simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		return -1;
	offset = simple_strtoul(argv[2], &endp, 16);
	if (*argv[2] == 0 || *endp != 0)
		return -1;
	len = simple_strtoul(argv[3], &endp, 16);
	if (*argv[3] == 0 || *endp != 0)
		return -1;

	/* Consistency checking */
	if (offset + len > chip->size) {
		printf("ERROR: attempting %s past flash size (%#llx)\n",
		       argv[0], chip->size);
		return 1;
	}

	buf = map_physmem(addr, len, MAP_WRBACK);
	if (!buf) {
		puts("Failed to map physical memory\n");
		return 1;
	}

	if (strcmp(argv[0], "update") == 0) {
		ret = spi_nand_flash_update(chip, offset, len, buf);
		printf("SPI-NAND: %zu bytes @ %#x Updated: %s\n",
		       (size_t)len, (u32)offset, ret ? "ERROR" : "OK");
	} else if (strncmp(argv[0], "read", 4) == 0 ||
			strncmp(argv[0], "write", 5) == 0) {
		int read;

		read = strncmp(argv[0], "read", 4) == 0;
		if (read)
			ret = spi_nand_flash_read(chip, offset, len, buf);
		else
			ret = spi_nand_flash_write(chip, offset, len, buf);

		printf("SPI-NAND: %zu bytes @ %#x %s: %s\n",
		       (size_t)len, (u32)offset,
		       read ? "Read" : "Written",
		       ret ? "ERROR" : "OK");
	}

	unmap_physmem(buf, len);

	return ret == 0 ? 0 : 1;
}

static int do_spi_nand_flash_erase(int argc, char * const argv[], bool spread)
{
	unsigned long offset;
	unsigned long len;
	char *endp;
	int ret;

	if (argc < 3)
		return -1;

	offset = simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		return -1;

	ret = sf_parse_len_arg(argv[2], &len);
	if (ret != 1)
		return -1;

	/* Consistency checking */
	if (offset + len > chip->size) {
		printf("ERROR: attempting %s past flash size (%#llx)\n",
		       argv[0], chip->size);
		return 1;
	}

	ret = spi_nand_flash_erase(chip, offset, len, spread);
	printf("SPI-NAND: %zu bytes @ %#x Erased: %s\n",
	       (size_t)len, (u32)offset, ret ? "ERROR" : "OK");

	return ret == 0 ? 0 : 1;
}

static int do_spi_nand_flash_show_bad(void)
{
	loff_t offset;

	for (offset = 0; offset < chip->size; offset += chip->block_size) {
		if (spi_nand_flash_block_is_bad(chip, offset))
			printf("Bad block at %#llx\n", offset);
	}

	return 0;
}

static int do_spi_nand_flash_mark_bad(int argc, char * const argv[])
{
	int ret;
	loff_t offset;
	char *endp;

	if (argc < 2)
		return -1;

	offset = simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		return -1;

	ret = spi_nand_flash_block_mark_bad(chip, offset);
	if (!ret)
		printf("SPI-NAND: %#llx marked as bad block\n", offset);

	return ret == 0 ? 0 : 1;
}

static int do_spi_nand_flash(cmd_tbl_t *cmdtp, int flag, int argc,
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

	if (strcmp(cmd, "probe") == 0) {
		ret = do_spi_nand_flash_probe(argc, argv);
		goto done;
	}

	/* The remaining commands require a selected device */
	if (!chip) {
		puts("No SPI flash selected. Please run `spi_nand probe'\n");
		return 1;
	}

	if (strcmp(cmd, "read") == 0 || strcmp(cmd, "write") == 0 ||
	    strcmp(cmd, "update") == 0)
		ret = do_spi_nand_flash_read_write(argc, argv);
	else if (strcmp(cmd, "erase") == 0)
		ret = do_spi_nand_flash_erase(argc, argv, false);
	else if (strcmp(cmd, "erase.spread") == 0)
		ret = do_spi_nand_flash_erase(argc, argv, true);
	else if (strcmp(cmd, "bad") == 0)
		ret = do_spi_nand_flash_show_bad();
	else if (strcmp(cmd, "markbad") == 0)
		ret = do_spi_nand_flash_mark_bad(argc, argv);
	else
		ret = -1;

done:
	if (ret != -1)
		return ret;

usage:
	return CMD_RET_USAGE;
}


U_BOOT_CMD(
	spi_nand,	5,	1,	do_spi_nand_flash,
	"SPI NAND flash sub-system",
	"probe [[bus:]cs]		- init flash device on given SPI bus\n"
	"				  and chip select\n"
	"spi_nand read addr offset len	- read `len' bytes starting at\n"
	"				  `offset' to memory at `addr', skipping bad blocks.\n"
	"spi_nand write addr offset len	- write `len' bytes from memory\n"
	"				  at `addr' to flash at `offset', skipping bad blocks.\n"
	"spi_nand erase[.spread] offset [+]len		- erase `len' bytes from `offset'\n"
	"				  `+len' round up `len' to block size\n"
	"				  With '.spread', erase enough for given file size, otherwise,\n"
	"				  'size' includes skipped bad blocks.\n"
	"spi_nand update addr offset len	- erase and write `len' bytes from memory\n"
	"				  at `addr' to flash at `offset'\n"
	"spi_nand bad - show bad blocks\n"
	"spi_nand markbad offset - mark block include `offset' as bad block\n"
);
