/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/stat.h>

#define MAX_FILENAME	256


/* A8K definitions */

/* Extension header types */
#define EXT_TYPE_SECURITY	0x1
#define EXT_TYPE_BINARY		0x2
#define EXT_TYPE_REGISTER	0x3

#define MAIN_HDR_MAGIC		0xB105B002

/* UART argument bitfield */
#define UART_MODE_UNMODIFIED	0x0
#define UART_MODE_DISABLE	0x1
#define UART_MODE_UPDATE	0x2

typedef struct _main_header {
	uint32_t	magic;			/*  0-3  */
	uint32_t	prolog_size;		/*  4-7  */
	uint32_t	boot_image_size;	/*  8-11 */
	uint32_t	source_addr;		/* 12-15 */
	uint32_t	load_addr;		/* 16-19 */
	uint32_t	exec_addr;		/* 20-23 */
	uint16_t	uart_args;		/* 24-25 */
	uint8_t		has_extension;		/*  26   */
	uint8_t		flags;			/*  27   */
	uint32_t	io_arg_0;		/* 28-31 */
	uint32_t	io_arg_1;		/* 32-35 */
	uint32_t	io_arg_2;		/* 36-39 */
	uint32_t	io_arg_3;		/* 40-43 */
	uint32_t	prolog_checksum;	/* 44-47 */
	uint32_t	rsrvd0;			/* 48-51 */
	uint32_t	rsrvd1;			/* 52-53 */
	uint32_t	rsrvd2;			/* 56-59 */
	uint32_t	rsrvd3;			/* 60-63 */
} header_t;

typedef struct _ext_header {
	uint8_t		type;
	uint8_t		offset;
	uint16_t	reserved;
	uint32_t	size;
} ext_header_t;


typedef struct _reg_entry {
	uint32_t	addr;
	uint32_t	value;
} reg_entry_t;

/* A8K definitions end */


typedef struct _options {
	char bin_ext_file[MAX_FILENAME];
	char reg_ext_file[MAX_FILENAME];
	uint32_t  load_addr;
	uint32_t  exec_addr;
	uint32_t  source_addr;

} options_t;

void usage_err(char *msg)
{
	printf("Error: %s\n", msg);
	printf("run 'doimage -h' to get usage information\n");
	exit(-1);
}

void usage(void)
{
	printf("Usage: doimage [options] <input_file> [output_file]\n");
	printf("create bootrom image from u-boot and boot extensions\n\n");

	printf("Arguments\n");
	printf("  input_file   name of boot image file.\n");
	printf("               if -p is used, name of bootrom image file to parse.\n");
	printf("  output_file  name of output bootrom image file\n");

	printf("\nOptions\n");
	printf("  -s        target SOC name. supports a8020,a7020\n");
	printf("            different SOCs may have different boot image format so\n");
	printf("            it's mandatory to know the target SOC\n");
	printf("  -i        boot I/F name. supports nand, spi, nor\n");
	printf("            This affects certain parameters coded in the image header\n");
	printf("  -l        boot image load address. default is 0x0\n");
	printf("  -e        boot image entry address. default is 0x0\n");
	printf("  -a        boot image source address. default is 0x0\n");
	printf("  -b        binary extension image file.\n");
	printf("            This image is executed before the boot image. this is typically\n");
	printf("            used to initiliaze the memory controller.\n");
	printf("            Currently supports only a single file.\n");
	printf("  -r        register extension file.\n");
	printf("            A text file containing pairs of register address and value.\n");
	printf("            These values are written before the execution of the boot image.\n");
	printf("            Currently supports only a single file.\n");
	printf("  -p        Parse and display a pre-built boot image\n");
	printf("  -h        Dispalys this help message\n");

	exit(-1);
}

#define EXT_FILENAME	"/tmp/ext_file"

/* globals */
options_t opts = {
	.bin_ext_file = "NA",
	.reg_ext_file = "NA",
	.load_addr = 0x0,
	.exec_addr = 0x0,
	.source_addr = 0x0,
};

int get_file_size(char *filename)
{
	struct stat st;

	if (stat(filename, &st) == 0)
		return st.st_size;

	return -1;
}

uint32_t checksum32(uint32_t *start, uint32_t len)
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


#define FMT_HEX		0
#define FMT_DEC		1
#define FMT_BIN		2
#define FMT_NONE	3

void do_print_field(unsigned int value, char *name, int start, int size, int format)
{
	printf("[0x%05x : 0x%05x]  %-18s", start, start + size - 1, name);

	switch (format) {
	case FMT_HEX:
		printf("0x%x\n", value);
		break;
	case FMT_DEC:
		printf("%d\n", value);
		break;
	default:
		printf("\n");
		break;
	}
}

#define print_field(st, type, field, hex, base) do_print_field((int)st->field, #field, \
		    base + offsetof(type, field), sizeof(st->field), hex)

int print_header(uint8_t *buf, int base)
{
	header_t *main_hdr;

	main_hdr = (header_t *)buf;

	printf("################### Main header ######################\n\n");
	print_field(main_hdr, header_t, magic, FMT_HEX, base);
	print_field(main_hdr, header_t, prolog_size, FMT_DEC, base);
	print_field(main_hdr, header_t, boot_image_size, FMT_DEC, base);
	print_field(main_hdr, header_t, source_addr, FMT_HEX, base);
	print_field(main_hdr, header_t, load_addr, FMT_HEX, base);
	print_field(main_hdr, header_t, exec_addr, FMT_HEX, base);
	print_field(main_hdr, header_t, uart_args, FMT_HEX, base);
	print_field(main_hdr, header_t, has_extension, FMT_DEC, base);
	print_field(main_hdr, header_t, flags, FMT_HEX, base);
	print_field(main_hdr, header_t, io_arg_0, FMT_HEX, base);
	print_field(main_hdr, header_t, io_arg_1, FMT_HEX, base);
	print_field(main_hdr, header_t, io_arg_2, FMT_HEX, base);
	print_field(main_hdr, header_t, io_arg_3, FMT_HEX, base);
	print_field(main_hdr, header_t, prolog_checksum, FMT_HEX, base);
	print_field(main_hdr, header_t, rsrvd0, FMT_HEX, base);
	print_field(main_hdr, header_t, rsrvd1, FMT_HEX, base);
	print_field(main_hdr, header_t, rsrvd2, FMT_HEX, base);
	print_field(main_hdr, header_t, rsrvd3, FMT_HEX, base);

	return sizeof(header_t);
}

int print_extension(void *buf, int base, int ext_size)
{
	ext_header_t *ext_hdr = buf;
	int total_size = ext_size;

	while (ext_size > 0) {
		if (ext_hdr->type == EXT_TYPE_BINARY)
			printf("\n################ Binary extension ###################\n\n");
		else if (ext_hdr->type == EXT_TYPE_REGISTER)
			printf("\n################ Register extension #################\n\n");
		else if (ext_hdr->type == EXT_TYPE_REGISTER)
			printf("\n################ Secure extension ###################\n\n");

		print_field(ext_hdr, ext_header_t, type, FMT_HEX, base);
		print_field(ext_hdr, ext_header_t, offset, FMT_HEX, base);
		print_field(ext_hdr, ext_header_t, reserved, FMT_HEX, base);
		print_field(ext_hdr, ext_header_t, size, FMT_DEC, base);

		do_print_field(0, "binary image", base + sizeof(ext_header_t), ext_hdr->size, FMT_NONE);

		ext_size -= sizeof(ext_header_t) + ext_hdr->size;
		base     += sizeof(ext_header_t) + ext_hdr->size;
	}

	return total_size;
}

int parse_image(FILE *in_fd, int size)
{
	uint8_t *buf;
	int read;
	int base = 0;
	int ret = 1;
	int prolog_size;
	header_t *main_hdr;
	uint32_t checksum, boot_checksum, prolog_checksum;

	buf = malloc(size);
	if (buf == NULL) {
		printf("Error: failed allocating parse buffer\n");
		return 1;
	}

	read = fread(buf, size, 1, in_fd);
	if (read != 1) {
		printf("Error: failed to read boot image\n");
		goto error;
	}

	main_hdr = (header_t *)buf;
	base += print_header(buf, base);

	if (main_hdr->has_extension)
		base += print_extension(buf + base, base, main_hdr->prolog_size - sizeof(header_t));

	prolog_size = base;

	printf("\n#################### Boot image ######################\n\n");
	if (main_hdr->source_addr) {
		do_print_field(0, "pre padding", base, main_hdr->source_addr, FMT_HEX);
		base += main_hdr->source_addr;
	}

	do_print_field(0, "boot image", base, size - base - 4, FMT_NONE);

	boot_checksum = *((uint32_t *)(buf + size - 4));
	do_print_field(boot_checksum, "checksum", base + main_hdr->boot_image_size, 4, FMT_HEX);

	printf("\n################### Image end ########################\n\n");

	/* Check sanity for certain values */
	printf("Checking values:\n");

	if (main_hdr->magic == MAIN_HDR_MAGIC) {
		printf("Headers magic:    OK!\n");
	} else {
		printf("\n****** ERROR: HEADER MAGIC 0x%08x != 0x%08x\n", main_hdr->magic, MAIN_HDR_MAGIC);
		goto error;
	}

	/* headers checksum */
	/* clear the checksum field in header to calculate checksum */
	prolog_checksum = main_hdr->prolog_checksum;
	main_hdr->prolog_checksum = 0;
	checksum = checksum32((uint32_t *)buf, prolog_size);

	if (checksum == prolog_checksum) {
		printf("Headers checksum: OK!\n");
	} else {
		printf("\n****** ERROR: BAD HEADER CHECKSUM 0x%08x != 0x%08x ********\n",
		       checksum, prolog_checksum);
		goto error;
	}

	/* boot image checksum */
	checksum = checksum32((uint32_t *)(buf + base), size - base - 4);
	if (checksum == boot_checksum) {
		printf("Image checksum:   OK!\n");
	} else {
		printf("\n****** ERROR: BAD IMAGE CHECKSUM 0x%08x != 0x%08x ********\n",
		       checksum, boot_checksum);
		goto error;
	}


	ret = 0;
error:
	free(buf);
	return ret;
}


int format_bin_ext(char *filename, FILE *out_fd)
{
	ext_header_t header;
	FILE *in_fd;
	int size, written;
	int aligned_size, pad_bytes;
	char c;

	in_fd = fopen(filename, "rb");
	if (in_fd == NULL) {
		printf("failed to open bin extension file %s\n", filename);
		return 1;
	}

	size = get_file_size(filename);
	if (size <= 0) {
		printf("bin extension file size is bad\n");
		return 1;
	}

	/* Align extension size to 8 bytes */
	aligned_size = (size + 7) & (~7);
	pad_bytes    = aligned_size - size;

	header.type = EXT_TYPE_BINARY;
	header.offset = 0;
	header.size = aligned_size;
	header.reserved = 0;

	/* Write header */
	written = fwrite(&header, sizeof(ext_header_t), 1, out_fd);
	if (written != 1) {
		printf("failed writing header to extension file\n");
		return 1;
	}

	/* Write image */
	while (size--) {
		c = getc(in_fd);
		fputc(c, out_fd);
	}

	while (pad_bytes--)
		fputc(0, out_fd);

	fclose(in_fd);

	return 0;
}

/* ****************************************
 *
 * Write all extensions (binary, reg, secure
 * extensions) to file
 *
 * ****************************************/

int format_extensions(char *ext_filename)
{
	FILE *out_fd;
	int ret = 0;

	out_fd = fopen(ext_filename, "wb");
	if (out_fd == NULL) {
		printf("failed to open extension output file %s", ext_filename);
		return 1;
	}

	if (strncmp(opts.bin_ext_file, "NA", MAX_FILENAME)) {
		if (format_bin_ext(opts.bin_ext_file, out_fd)) {
			ret = 1;
			goto error;
		}
	}


error:
	fflush(out_fd);
	fclose(out_fd);
	return ret;
}
/* ****************************************
 *
 * Write the image prolog, i.e.
 * main header and extensions, to file
 *
 * ****************************************/

int write_prolog(int has_ext, char *ext_filename, int image_size, FILE *out_fd)
{
	header_t header;
	int main_hdr_size = sizeof(header_t);
	int prolog_size = main_hdr_size;
	FILE *ext_fd;
	char *buf;
	int written, read;
	int ret = 1;

	memset(&header, 0, main_hdr_size);

	if (has_ext)
		prolog_size +=  get_file_size(ext_filename);

	header.magic          = MAIN_HDR_MAGIC;
	header.prolog_size = prolog_size;
	header.source_addr = opts.source_addr;
	header.load_addr   = opts.load_addr;
	header.exec_addr   = opts.exec_addr;
	header.has_extension = has_ext;
	header.boot_image_size = (image_size + 3) & (~0x3);

	/* TODO - fill these with something */
	header.uart_args = 0;
	header.flags     = 0;

	buf = malloc(prolog_size);
	if (buf == NULL) {
		printf("Error: failed allocating checksum buffer\n");
		return 1;
	}

	/* Populate buffer with main header and extensions */
	memcpy(buf, &header, main_hdr_size);
	if (has_ext) {
		ext_fd = fopen(ext_filename, "rb");
		if (ext_fd == NULL) {
			printf("Error: failed to open extensions file\n");
			goto error;
		}

		read = fread(&buf[main_hdr_size],  get_file_size(ext_filename), 1, ext_fd);
		if (read != 1) {
			printf("Error: failed to open extensions file\n");
			goto error;
		}
	}

	/* Update the total prolog checksum */
	header.prolog_checksum = checksum32((uint32_t *)buf, prolog_size);

	/* Update the header in the buffer */
	memcpy(buf, &header, main_hdr_size);

	/* Now spill everything to output file */
	written = fwrite(buf, prolog_size, 1, out_fd);
	if (written != 1) {
		printf("Error: failed to write headers to output file\n");
		goto error;
	}

	ret = 0;

error:
	free(buf);
	return ret;
}

int write_boot_image(FILE *in_fd, int image_size, FILE *out_fd)
{
	int aligned_size;
	int post_pad;
	int pre_pad;
	uint8_t pad_val = 0;
	int written, read;
	uint8_t *buf;
	uint32_t checksum;

	/* Image size must be aligned to 4 bytes */
	aligned_size = (image_size + 3) & (~0x3);
	post_pad = aligned_size - image_size;
	pre_pad =  opts.source_addr;

	/* fill with zeros untill the boot image start address */
	while (pre_pad--)
		fputc(pad_val, out_fd);

	/* Read entire file to buffer for checksum calculation */
	buf = malloc(aligned_size);
	if (buf == NULL) {
		printf("Error: Failed to allocate boot image buffer\n");
		return 1;
	}

	read = fread(buf, image_size, 1, in_fd);
	if (read != 1) {
		printf("Error: failed to read boot image\n");
		goto error;
	}

	/* Pad the buffer to be 4 bytes aligned */
	while (post_pad) {
		buf[aligned_size - post_pad] = pad_val;
		post_pad--;
	}

	checksum = checksum32((uint32_t *)buf, aligned_size);

	/* Write the aligned buffer to file */
	written = fwrite(buf, aligned_size, 1, out_fd);
	if (written != 1) {
		printf("Error: Failed to write boot image\n");
		goto error;
	}

	/* Finally write the checksum */
	written = fwrite(&checksum, sizeof(uint32_t), 1, out_fd);
	if (written != 1) {
		printf("Error: Failed to write boot image checksum\n");
		goto error;
	}

	free(buf);
	return 0;
error:
	free(buf);
	return 1;
}

int main(int argc, char *argv[])
{
	char in_file[MAX_FILENAME];
	char out_file[MAX_FILENAME];
	FILE *in_fd = NULL;
	FILE *out_fd = NULL;
	int parse = 0;
	int has_ext = 0;
	int opt;
	int ret = 0;
	int image_size;

	while ((opt = getopt(argc, argv, "hps:i:l:e:a:b:")) != -1) {
		switch (opt) {
		case 'h':
			usage();
			break;
		case 'l':
			opts.load_addr = strtoul(optarg, NULL, 0);
			break;
		case 'e':
			opts.exec_addr = strtoul(optarg, NULL, 0);
			break;
		case 'a':
			opts.source_addr = strtoul(optarg, NULL, 0);
			break;
		case 'b':
			strncpy(opts.bin_ext_file, optarg, MAX_FILENAME);
			has_ext = 1;
			break;
		case 'r':
			strncpy(opts.reg_ext_file, optarg, MAX_FILENAME);
			has_ext = 1;
			break;
		case 'p':
			parse = 1;
			break;
		default: /* '?' */
			usage_err("Unknown argument");
			exit(EXIT_FAILURE);
		}
	}

	/* Check validity of inputes */
	if (opts.source_addr % 8)
		usage_err("Source address must be 8 bytes aligned");

	if (opts.load_addr % 8)
		usage_err("Load address must be 8 bytes aligned");

	/* The remaining arguments are the input
	 * and potentially output file */

	/* Input file must exist so exit if not */
	if (optind >= argc)
		usage_err("missing input file name");

	strncpy(in_file, argv[optind], MAX_FILENAME);
	optind++;

	/* Output file must exist in non parse mode */
	if (optind < argc)
		strncpy(out_file, argv[optind], MAX_FILENAME);
	else if (!parse)
		usage_err("missing output file name");

	/* open the input file */
	in_fd = fopen(in_file, "rb");
	if (in_fd == NULL) {
		printf("Error: Failed to open input file %s\n", in_file);
		goto main_exit;
	}
	image_size = get_file_size(in_file);

	if (parse) {
		ret = parse_image(in_fd, image_size);
		goto main_exit;
	}

	/* Create a blob file from all extensions */
	if (has_ext) {
		ret = format_extensions(EXT_FILENAME);
		if (ret)
			goto main_exit;
	}

	out_fd = fopen(out_file, "wb");
	if (out_fd == NULL) {
		printf("Error: Failed to open output file %s\n", out_file);
		goto main_exit;
	}

	ret = write_prolog(has_ext, EXT_FILENAME, image_size, out_fd);
	if (ret)
		goto main_exit;

	ret = write_boot_image(in_fd, image_size, out_fd);
	if (ret)
		goto main_exit;


main_exit:
	if (in_fd)
		fclose(in_fd);

	if (out_fd)
		fclose(out_fd);

	exit(ret);
}

