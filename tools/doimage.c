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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>

#ifdef CONFIG_MVEBU_SECURE_BOOT
#include <libconfig.h>	/* for parsing config file */

#if !defined(POLARSSL_CONFIG_FILE)
#include "polarssl/config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

/* PolarSSL (mbedTLS) stuff */
#if defined(POLARSSL_BIGNUM_C) && defined(POLARSSL_ENTROPY_C) && \
	defined(POLARSSL_SHA256_C) && \
	defined(POLARSSL_PK_PARSE_C) && defined(POLARSSL_FS_IO) && \
	defined(POLARSSL_CTR_DRBG_C)
#include <polarssl/error.h>
#include <polarssl/entropy.h>
#include <polarssl/ctr_drbg.h>
#include <polarssl/md.h>
#include <polarssl/pk.h>
#include <polarssl/sha256.h>
#include <polarssl/x509.h>
#else
#error "Bad PolarSSL/mbedTLS configuration!"
#endif
#endif /* CONFIG_MVEBU_SECURE_BOOT */

#define MAX_FILENAME		256
#define CSK_ARR_SZ		16
#define CSK_ARR_EMPTY_FILE	"*"
#define AES_KEY_BIT_LEN		256
#define AES_KEY_BYTE_LEN	(AES_KEY_BIT_LEN >> 3)
#define AES_BLOCK_SZ		16
#define RSA_SIGN_BYTE_LEN	256
#define MAX_RSA_DER_BYTE_LEN	524

#define VERSION_STRING		"Marvell(C) doimage utility version 3.0"

/* A8K definitions */

/* Extension header types */
#define EXT_TYPE_SECURITY	0x1
#define EXT_TYPE_BINARY		0x2
#define EXT_TYPE_REGISTER	0x3

#define MAIN_HDR_MAGIC		0xB105B002

/* PROLOG alignment considerations:
**  128B: To allow supporting XMODEM protocol.
**  8KB: To align the boot image to the largest NAND page size, and simplify
**  the read operations from NAND.
**  We choose the largest page size, in order to use a single image for all
**  NAND page sizes.
*/
#define PROLOG_ALIGNMENT	(8 << 10)

/* UART argument bitfield */
#define UART_MODE_UNMODIFIED	0x0
#define UART_MODE_DISABLE	0x1
#define UART_MODE_UPDATE	0x2

typedef struct _main_header {
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

typedef struct _sec_entry {
	uint8_t		kak_key[MAX_RSA_DER_BYTE_LEN];
	uint32_t	jtag_delay;
	uint32_t	box_id;
	uint32_t	flash_id;
	uint32_t	jtag_en;
	uint32_t	encrypt_en;
	uint32_t	efuse_dis;
	uint8_t		header_sign[RSA_SIGN_BYTE_LEN];
	uint8_t		image_sign[RSA_SIGN_BYTE_LEN];
	uint8_t		csk_keys[CSK_ARR_SZ][MAX_RSA_DER_BYTE_LEN];
	uint8_t		csk_sign[RSA_SIGN_BYTE_LEN];
} sec_entry_t;

/* A8K definitions end */

/* UART argument bitfield */
#define UART_MODE_UNMODIFIED	0x0
#define UART_MODE_DISABLE	0x1
#define UART_MODE_UPDATE	0x2

#define uart_set_mode(arg, mode)	(arg |= (mode & 0x3))

typedef struct _sec_options {
#ifdef CONFIG_MVEBU_SECURE_BOOT
	char aes_key_file[MAX_FILENAME];
	char kak_key_file[MAX_FILENAME];
	char csk_key_file[CSK_ARR_SZ][MAX_FILENAME];
	uint32_t	box_id;
	uint32_t	flash_id;
	uint32_t	jtag_delay;
	uint8_t		csk_index;
	uint8_t		jtag_enable;
	uint8_t		efuse_disable;
	uint32_t	*control;
	pk_context	kak_pk;
	pk_context	csk_pk[CSK_ARR_SZ];
	uint8_t		aes_key[AES_KEY_BYTE_LEN];
	uint8_t		*encrypted_image;
	uint32_t	enc_image_sz;
#endif
} sec_options;

typedef struct _options {
	char bin_ext_file[MAX_FILENAME];
	char reg_ext_file[MAX_FILENAME];
	char sec_cfg_file[MAX_FILENAME];
	sec_options *sec_opts;
	uint32_t  load_addr;
	uint32_t  exec_addr;
	uint32_t  baudrate;
	uint8_t	  disable_print;
	int8_t    key_index; /* For header signatures verification only */
	uint32_t  nfc_io_args;
} options_t;

void usage_err(char *msg)
{
	printf("Error: %s\n", msg);
	printf("run 'doimage -h' to get usage information\n");
	exit(-1);
}

void usage(void)
{
	printf("\n\n%s\n\n", VERSION_STRING);
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
	printf("  -b        binary extension image file.\n");
	printf("            This image is executed before the boot image. this is typically\n");
	printf("            used to initiliaze the memory controller.\n");
	printf("            Currently supports only a single file.\n");
	printf("  -r        register extension file.\n");
	printf("            A text file containing pairs of register address and value.\n");
	printf("            These values are written before the execution of the boot image.\n");
	printf("            Currently supports only a single file.\n");
#ifdef CONFIG_MVEBU_SECURE_BOOT
	printf("  -c        Make trusted boot image using parameters from the configuration file.\n");
#endif
	printf("  -p        Parse and display a pre-built boot image\n");
#ifdef CONFIG_MVEBU_SECURE_BOOT
	printf("  -k        Key index for RSA signatures verification when parsing the boot image\n");
#endif
	printf("  -m        Disable prints of bootrom and binary extension\n");
	printf("  -u        UART baudrate used for bootrom prints. Must be multiple of 1200\n");
	printf("  -h        Dispalys this help message\n");
	printf(" IO-ROM NFC-NAND boot parameters:\n");
	printf("  -n        NAND device block size (in KB) [Default is 64KB].\n");
	printf("  -t        NAND cell technology (SLC or MLC) [Default is SLC].\n");

	exit(-1);
}

#define EXT_FILENAME	"/tmp/ext_file"

/* globals */
options_t opts = {
	.bin_ext_file = "NA",
	.reg_ext_file = "NA",
	.sec_cfg_file = "NA",
	.sec_opts = 0,
	.load_addr = 0x0,
	.exec_addr = 0x0,
	.disable_print = 0,
	.baudrate = 0,
	.key_index = -1,
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

/*******************************************************************************
*    create_rsa_signature (memory buffer content)
*          Create RSASSA-PSS/SHA-256 signature for memory buffer
*          using RSA Private Key
*    INPUT:
*          pk_ctx     Private Key context
*          input      memory buffer
*          ilen       buffer length
*          pers       personalization string for seeding the RNG.
*                     For instance a private key file name.
*    OUTPUT:
*          signature  RSA-2048 signature
*    RETURN:
*          0 on success
*******************************************************************************/
#ifdef CONFIG_MVEBU_SECURE_BOOT
int create_rsa_signature(pk_context		*pk_ctx,
			 const unsigned char	*input,
			 size_t			ilen,
			 const char		*pers,
			 uint8_t		*signature)
{
	entropy_context		entropy;
	ctr_drbg_context	ctr_drbg;
	unsigned char		hash[32];
	unsigned char		buf[POLARSSL_MPI_MAX_SIZE];
	int					rval;

	/* Not sure this is required, but it's safer to start with empty buffers */
	memset(hash, 0, sizeof(hash));
	memset(buf, 0, sizeof(buf));

	entropy_init(&entropy);

	/* Seed the random number generator */
	rval = ctr_drbg_init(&ctr_drbg, entropy_func, &entropy,
			     (const unsigned char *)pers, strlen(pers));
	if (rval != 0) {
		fprintf(stderr, " Failed in ctr_drbg_init call (%d)!\n", rval);
		goto sign_exit;
	}

	/* The PK context should be already initalized.
	   Set the padding type for this PK context */
	rsa_set_padding(pk_rsa(*pk_ctx), RSA_PKCS_V21, POLARSSL_MD_SHA256);

	/* First compute the SHA256 hash for the input blob */
	sha256(input, ilen, hash, 0);

	/* Then calculate the hash signature */
	rval = rsa_rsassa_pss_sign(pk_rsa(*pk_ctx), ctr_drbg_random, &ctr_drbg,
				   RSA_PRIVATE, POLARSSL_MD_SHA256, 0, hash, buf);
	if (rval != 0) {
		fprintf(stderr,  "Failed to create RSA signature for %s. Error %d\n", pers, rval);
		goto sign_exit;
	}
	memcpy(signature, buf, 256);

sign_exit:
	ctr_drbg_free(&ctr_drbg);
	entropy_free(&entropy);

	return rval;
} /* end of create_rsa_signature */

/*******************************************************************************
*    verify_rsa_signature (memory buffer content)
*          Verify RSASSA-PSS/SHA-256 signature for memory buffer
*          using RSA Public Key
*    INPUT:
*          pub_key    Public Key buffer
*          ilen       Public Key buffer length
*          input      memory buffer
*          ilen       buffer length
*          pers       personalization string for seeding the RNG.
*          signature  RSA-2048 signature
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int verify_rsa_signature(const unsigned char	*pub_key,
			 size_t			klen,
			 const unsigned char	*input,
			 size_t			ilen,
			 const char		*pers,
			 uint8_t		*signature)
{
	entropy_context		entropy;
	ctr_drbg_context	ctr_drbg;
	pk_context		pk_ctx;
	unsigned char		hash[32];
	int			rval;

	/* Not sure this is required, but it's safer to start with empty buffer */
	memset(hash, 0, sizeof(hash));

	pk_init(&pk_ctx);
	entropy_init(&entropy);

	/* Seed the random number generator */
	rval = ctr_drbg_init(&ctr_drbg, entropy_func, &entropy,
			     (const unsigned char *)pers, strlen(pers));
	if (rval != 0) {
		fprintf(stderr, " Failed in ctr_drbg_init call (%d)!\n", rval);
		goto verify_exit;
	}

	/* Check ability to read the public key */
	rval = pk_parse_public_key(&pk_ctx, pub_key, MAX_RSA_DER_BYTE_LEN);
	if (rval != 0) {
		fprintf(stderr, " Failed in pk_parse_public_key (%#x)!\n", rval);
		goto verify_exit;
	}

	/* Set the padding type for the new PK context */
	rsa_set_padding(pk_rsa(pk_ctx), RSA_PKCS_V21, POLARSSL_MD_SHA256);

	/* Compute the SHA256 hash for the input buffer */
	sha256(input, ilen, hash, 0);

	rval = rsa_rsassa_pss_verify(pk_rsa(pk_ctx), ctr_drbg_random, &ctr_drbg,
				     RSA_PUBLIC, POLARSSL_MD_SHA256, 0, hash, signature);
	if (rval != 0)
		fprintf(stderr, "Failed to verify signature (%d)!\n", rval);

verify_exit:

	pk_free(&pk_ctx);
	ctr_drbg_free(&ctr_drbg);
	entropy_free(&entropy);
	return rval;
} /* end of verify_rsa_signature */

/*******************************************************************************
*    image_encrypt
*           Encrypt image buffer using AES-256-CBC scheme.
*           The resulting image is saved into opts.sec_opts->encrypted_image
*           and the adjusted image size into opts.sec_opts->enc_image_sz
*           First AES_BLOCK_SZ bytes of the output image contain IV
*    INPUT:
*          buf        Source buffer to encrypt
*          blen       Source buffer length
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int image_encrypt(uint8_t *buf, uint32_t blen)
{
	struct timeval	tv;
	char			*ptmp = (char *)&tv;
	unsigned char	digest[32];
	unsigned char	IV[AES_BLOCK_SZ];
	int				i, k;
	aes_context		aes_ctx;
	int				rval = -1;
	uint8_t			*test_img = 0;

	if (AES_BLOCK_SZ > 32) {
		fprintf(stderr, "Unsupported AES block size %d\n", AES_BLOCK_SZ);
		return rval;
	}

	aes_init(&aes_ctx);
	memset(IV, 0, AES_BLOCK_SZ);
	memset(digest, 0, 32);

	/* Generate initialization vector and init the AES engine */
	/* Use file name XOR current time and finaly SHA-256 [0...AES_BLOCK_SZ-1] */
	k = strlen(opts.sec_opts->aes_key_file);
	if (k > AES_BLOCK_SZ)
		k = AES_BLOCK_SZ;
	memcpy(IV, opts.sec_opts->aes_key_file, k);
	gettimeofday(&tv, 0);

	for (i = 0, k = 0; i < AES_BLOCK_SZ; i++, k = (k+1) % sizeof(struct timeval))
		IV[i] ^= ptmp[k];

	/* compute SHA-256 digest of the results and use it as the init vector (IV) */
	sha256(IV, AES_BLOCK_SZ, digest, 0);
	memcpy(IV, digest, AES_BLOCK_SZ);
	aes_setkey_enc(&aes_ctx, opts.sec_opts->aes_key, AES_KEY_BIT_LEN);

	/* The output image has to include extra space for IV
	   and to be aligned to the AES block size.
	   The input image buffer has to be already aligned to AES_BLOCK_SZ
	   and padded with zeroes */
	opts.sec_opts->enc_image_sz = (blen + 2 * AES_BLOCK_SZ - 1) & ~(AES_BLOCK_SZ - 1);
	opts.sec_opts->encrypted_image = calloc(opts.sec_opts->enc_image_sz, 1);
	if (opts.sec_opts->encrypted_image == 0) {
		fprintf(stderr, "Failed to allocate encrypted image!\n");
		goto encrypt_exit;
	}

	/* Put IV into the output buffer next to the encrypted image
	   Since the IV is modified by the encryption function,
	   this should be done now */
	memcpy(opts.sec_opts->encrypted_image +
		   opts.sec_opts->enc_image_sz - AES_BLOCK_SZ,
		   IV, AES_BLOCK_SZ);
	rval = aes_crypt_cbc(&aes_ctx, AES_ENCRYPT,
			     opts.sec_opts->enc_image_sz - AES_BLOCK_SZ,
			     IV, buf, opts.sec_opts->encrypted_image);
	if (rval != 0) {
		fprintf(stderr, "Failed to encrypt the image! Error %d\n", rval);
		goto encrypt_exit;
	}

	aes_free(&aes_ctx);

	/* Try to decrypt the image and compare it with the original data */
	aes_init(&aes_ctx);
	aes_setkey_dec(&aes_ctx, opts.sec_opts->aes_key, AES_KEY_BIT_LEN);

	test_img = calloc(opts.sec_opts->enc_image_sz - AES_BLOCK_SZ, 1);
	if (test_img == 0) {
		fprintf(stderr, "Failed to allocate test image!d\n");
		rval = -1;
		goto encrypt_exit;
	}

	memcpy(IV, opts.sec_opts->encrypted_image +
		   opts.sec_opts->enc_image_sz - AES_BLOCK_SZ,
		   AES_BLOCK_SZ);
	rval = aes_crypt_cbc(&aes_ctx, AES_DECRYPT,
			     opts.sec_opts->enc_image_sz - AES_BLOCK_SZ,
			     IV, opts.sec_opts->encrypted_image, test_img);
	if (rval != 0) {
		fprintf(stderr, "Failed to decrypt the image! Error %d\n", rval);
		goto encrypt_exit;
	}

	for (i = 0; i < blen; i++) {
		if (buf[i] != test_img[i]) {
			fprintf(stderr, "Failed to compare the image after decryption! Byte count %d\n", i);
			rval = -1;
			goto encrypt_exit;
		}
	}

encrypt_exit:

	aes_free(&aes_ctx);
	if (test_img)
		free(test_img);

	return rval;
} /* end of image_encrypt */

/*******************************************************************************
*    verify_secure_header_signatures
*          Verify CSK array, header and image signatures and print results
*    INPUT:
*          main_hdr       Main header
*          sec_ext        Secure extention
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int verify_secure_header_signatures(header_t *main_hdr, sec_entry_t *sec_ext)
{
	uint8_t	*image = (uint8_t *)main_hdr + main_hdr->prolog_size;
	uint8_t	signature[RSA_SIGN_BYTE_LEN];
	int		rval = -1;

	/* Save headers signature and reset it in the secure header */
	memcpy(signature, sec_ext->header_sign, RSA_SIGN_BYTE_LEN);
	memset(sec_ext->header_sign, 0, RSA_SIGN_BYTE_LEN);

	fprintf(stdout, "\nCheck RSA Signatures\n");
	fprintf(stdout, "#########################\n");
	fprintf(stdout, "CSK Block Signature: ");
	if (verify_rsa_signature(sec_ext->kak_key, MAX_RSA_DER_BYTE_LEN,
				 &sec_ext->csk_keys[0][0], sizeof(sec_ext->csk_keys),
				 "CSK Block Signature: ",
				 sec_ext->csk_sign) != 0) {
		fprintf(stdout, "ERROR\n");
		goto ver_error;
	}
	fprintf(stdout, "OK\n");

	if (opts.key_index != -1) {
		fprintf(stdout, "Image Signature:     ");
		if (verify_rsa_signature(sec_ext->csk_keys[opts.key_index], MAX_RSA_DER_BYTE_LEN,
					 image, main_hdr->boot_image_size,
					 "Image Signature: ",
					 sec_ext->image_sign) != 0) {
			fprintf(stdout, "ERROR\n");
			goto ver_error;
		}
		fprintf(stdout, "OK\n");

		fprintf(stdout, "Header Signature:    ");
		if (verify_rsa_signature(sec_ext->csk_keys[opts.key_index], MAX_RSA_DER_BYTE_LEN,
					 (uint8_t *)main_hdr, main_hdr->prolog_size,
					 "Header Signature: ",
					 signature) != 0) {
			fprintf(stdout, "ERROR\n");
			goto ver_error;
		}
		fprintf(stdout, "OK\n");
	}
	fprintf(stdout, "SKIP Image and Header Signatures check (undefined key index)\n");

	rval = 0;

ver_error:
	memcpy(sec_ext->header_sign, signature, RSA_SIGN_BYTE_LEN);
	return rval;
}

/*******************************************************************************
*    verify_and_copy_file_name_entry
*    INPUT:
*          element_name
*          element
*    OUTPUT:
*          copy_to
*    RETURN:
*          0 on success
*******************************************************************************/
int verify_and_copy_file_name_entry(const char *element_name, const char *element, char *copy_to)
{
	int element_length = strlen(element);

	if (element_length >= MAX_FILENAME) {
		fprintf(stderr, "The file name %s for %s is too long (%d). Maximum allowed %d characters!\n",
			element, element_name, element_length, MAX_FILENAME);
		return -1;
	} else if (element_length == 0) {
		fprintf(stderr, "The file name for %s is empty!\n", element_name);
		return -1;
	}
	memcpy(copy_to, element, element_length);

	return 0;
}

/*******************************************************************************
*    parse_sec_config_file
*          Read the secure boot configuration from a file
*          into internal structures
*    INPUT:
*          filename      File name
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int parse_sec_config_file(char *filename)
{
	config_t		sec_cfg;
	int			array_sz, element, rval = -1;
	const char		*cfg_string;
	int			cfg_int;
	long			cfg_long;
	const config_setting_t	*csk_array, *control_aray;
	sec_options		*sec_opt = 0;

	config_init(&sec_cfg);

	if (config_read_file(&sec_cfg, filename) != CONFIG_TRUE) {
		fprintf(stderr, "Failed to read data from config file %s\n\t%s at line %d\n",
			filename, config_error_text(&sec_cfg), config_error_line(&sec_cfg));
		goto exit_parse;
	}

	sec_opt = (sec_options *)calloc(sizeof(sec_options), 1);
	if (sec_opt == 0) {
		fprintf(stderr, "Cannot allocate memory for secure boot options!\n");
		goto exit_parse;
	}

	/* KAK file name */
	if (config_lookup_string(&sec_cfg, "kak_key_file", &cfg_string) != CONFIG_TRUE) {
		fprintf(stderr, "The \"kak_key_file\" undefined!\n");
		goto exit_parse;
	}
	if (verify_and_copy_file_name_entry("kak_key_file", cfg_string, sec_opt->kak_key_file))
		goto exit_parse;


	/* AES file name - can be empty/undefined */
	if (config_lookup_string(&sec_cfg, "aes_key_file", &cfg_string) == CONFIG_TRUE) {
		if (verify_and_copy_file_name_entry("aes_key_file", cfg_string, sec_opt->aes_key_file))
			goto exit_parse;
	}

	/* CSK file names array */
	csk_array = config_lookup(&sec_cfg, "csk_key_file");
	if (csk_array == NULL) {
		fprintf(stderr, "The \"csk_key_file\" undefined!\n");
		goto exit_parse;
	}
	array_sz = config_setting_length(csk_array);
	if (array_sz > CSK_ARR_SZ) {
		fprintf(stderr, "The \"csk_key_file\" array is too big! "
			"Only first %d elements will be used\n",
			CSK_ARR_SZ);
		array_sz = CSK_ARR_SZ;
	} else if (array_sz == 0) {
		fprintf(stderr, "The \"csk_key_file\" array is empty!\n");
		goto exit_parse;
	}

	for (element = 0; element < array_sz; element++) {
		cfg_string = config_setting_get_string_elem(csk_array, element);
		if (verify_and_copy_file_name_entry(
				"csk_key_file", cfg_string, sec_opt->csk_key_file[element])) {
			fprintf(stderr, "Bad csk_key_file[%d] entry!\n", element);
			goto exit_parse;
		}
	}

	/* JTAG options */
	if (config_lookup_bool(&sec_cfg, "jtag.enable", &cfg_int) != CONFIG_TRUE) {
		fprintf(stderr, "Error obtaining \"jtag.enable\" element. Using default - FALSE\n");
		cfg_int = 0;
	}
	sec_opt->jtag_enable = cfg_int;

	if (config_lookup_int(&sec_cfg, "jtag.delay", &cfg_long) != CONFIG_TRUE) {
		fprintf(stderr, "Error obtaining \"jtag.delay\" element. Using default - 0us\n");
		cfg_long = 0;
	}
	sec_opt->jtag_delay = cfg_long;

	/* eFUSE option */
	if (config_lookup_bool(&sec_cfg, "efuse_disable", &cfg_int) != CONFIG_TRUE) {
		fprintf(stderr, "Error obtaining \"efuse_disable\" element. Using default - FALSE\n");
		cfg_int = 0;
	}
	sec_opt->efuse_disable = cfg_int;

	/* Box ID option */
	if (config_lookup_int(&sec_cfg, "box_id", &cfg_long) != CONFIG_TRUE) {
		fprintf(stderr, "Error obtaining \"box_id\" element. Using default - 0x0\n");
		cfg_long = 0;
	}
	sec_opt->box_id = cfg_long;

	/* Flash ID option */
	if (config_lookup_int(&sec_cfg, "flash_id", &cfg_long) != CONFIG_TRUE) {
		fprintf(stderr, "Error obtaining \"flash_id\" element. Using default - 0x0\n");
		cfg_long = 0;
	}
	sec_opt->flash_id = cfg_long;

	/* CSK index option */
	if (config_lookup_int(&sec_cfg, "csk_key_index", &cfg_long) != CONFIG_TRUE) {
		fprintf(stderr, "Error obtaining \"flash_id\" element. Using default - 0x0\n");
		cfg_long = 0;
	}
	sec_opt->csk_index = cfg_long;

	/* Secure boot control array */
	control_aray = config_lookup(&sec_cfg, "control");
	if (csk_array != NULL) {
		array_sz = config_setting_length(control_aray);
		if (array_sz == 0)
			fprintf(stderr, "The \"control\" array is empty!\n");
	} else {
		fprintf(stderr, "The \"control\" is undefined!\n");
		array_sz = 0;
	}

	sec_opt->control = (uint32_t *)malloc(sizeof(array_sz + 1)); /* 1 DWORD for the size */
	if (sec_opt->control == 0) {
		fprintf(stderr, "Cannot allocate memory for secure boot control block!\n");
		goto exit_parse;
	}
	memset(sec_opt->control, 0, sizeof(uint32_t));

	sec_opt->control[0] = array_sz;
	for (element = 0; element < array_sz; element++)
		sec_opt->control[element + 1] = config_setting_get_int_elem(control_aray, element);

	opts.sec_opts = sec_opt;
	rval = 0;

exit_parse:
	config_destroy(&sec_cfg);
	if (sec_opt && (rval != 0))
		free(sec_opt);
	return rval;
} /* end of parse_sec_config_file */

int format_sec_ext(char *filename, FILE *out_fd)
{
	ext_header_t	header;
	sec_entry_t	sec_ext;
	int		index;
	int		written;

#define DER_BUF_SZ	1600

	/* First, parse the configuration file */
	if (parse_sec_config_file(filename)) {
		printf("failed parsing configuration file %s\n", filename);
		return 1;
	}

	/* Everything except signatures can be created at this stage */
	header.type = EXT_TYPE_SECURITY;
	header.offset = 0;
	/* Size of secure boot extention can vary only in control block content */
	header.size = sizeof(sec_entry_t) + sizeof(uint32_t) * (opts.sec_opts->control[0] + 1);
	header.reserved = 0;

	/* Bring up RSA context and read private keys from their files */
	for (index = 0; index < (CSK_ARR_SZ + 1); index++) {
		/* for every private key file */
		pk_context	*pk_ctx = (index == CSK_ARR_SZ) ?
					&opts.sec_opts->kak_pk :
					&opts.sec_opts->csk_pk[index];
		char		*fname = (index == CSK_ARR_SZ) ?
					opts.sec_opts->kak_key_file :
					opts.sec_opts->csk_key_file[index];
		uint8_t		*out_der_key = (index == CSK_ARR_SZ) ?
					sec_ext.kak_key : sec_ext.csk_keys[index];
		size_t		output_len;
		unsigned char	output_buf[DER_BUF_SZ];
		unsigned char	*der_buf_start;

		/* Handle invalid/reserved file names */
		if (strncmp(CSK_ARR_EMPTY_FILE, fname, strlen(CSK_ARR_EMPTY_FILE)) == 0) {
			if (opts.sec_opts->csk_index == index) {
				fprintf(stderr, "CSK file name with index %d cannot be %s\n",
					index, CSK_ARR_EMPTY_FILE);
				return 1;
			} else if (index == CSK_ARR_SZ) {
				fprintf(stderr, "KAK file name cannot be %s\n", CSK_ARR_EMPTY_FILE);
				return 1;
			}
			/* this key will be empty in CSK array */
			continue;
		}

		pk_init(pk_ctx);
		/* Read the private RSA key into the context and verify it (no password) */
		if (pk_parse_keyfile(pk_ctx, fname, "") != 0) {
			fprintf(stderr, "Cannot read RSA private key file %s\n", fname);
			return 1;
		}

		/* Create a public key out of private one and store it in DER format */
		output_len = pk_write_pubkey_der(pk_ctx, output_buf, DER_BUF_SZ);
		if (output_len < 0) {
			fprintf(stderr, "Failed to create DER coded PUB key (%s)\n", fname);
			return 1;
		}
		/* Data in the output buffer is aligned to the buffer end */
		der_buf_start = output_buf + sizeof(output_buf) - output_len;
		/* In the header DER data is aligned to the start of appropriate field */
		memcpy(out_der_key, der_buf_start, output_len);

	} /* for every private key file */

	/* The CSK block signature can be created here */
	if (create_rsa_signature(&opts.sec_opts->kak_pk,
				 &sec_ext.csk_keys[0][0], sizeof(sec_ext.csk_keys),
				 opts.sec_opts->csk_key_file[opts.sec_opts->csk_index],
				 sec_ext.csk_sign) != 0) {
		fprintf(stderr, "Failed to sign CSK keys block!\n");
		return 1;
	}
	/* Check that everything is correct */
	if (verify_rsa_signature(sec_ext.kak_key, MAX_RSA_DER_BYTE_LEN,
				 &sec_ext.csk_keys[0][0], sizeof(sec_ext.csk_keys),
				 opts.sec_opts->kak_key_file,
				 sec_ext.csk_sign) != 0) {
		fprintf(stderr, "Failed to verify CSK keys block signature!\n");
		return 1;
	}

	/* AES encryption stuff */
	if (strlen(opts.sec_opts->aes_key_file) != 0) {
		FILE		*in_fd;
		uint8_t		key_buf[AES_KEY_BYTE_LEN];

		in_fd = fopen(opts.sec_opts->aes_key_file, "rb");
		if (in_fd == NULL) {
			fprintf(stderr, "Failed to open AES key file %s\n", opts.sec_opts->aes_key_file);
			return 1;
		}

		/* Read the AES key in ASCII format byte by byte */
		for (index = 0; index < AES_KEY_BYTE_LEN; index++) {
			if (fscanf(in_fd, "%02hhx", key_buf + index) != 1) {
				fprintf(stderr, "Failed to read AES key byte %d from file %s\n",
					index, opts.sec_opts->aes_key_file);
				fclose(in_fd);
				return 1;
			}
		}
		fclose(in_fd);
		sec_ext.encrypt_en = 1;
	} else {
		sec_ext.encrypt_en = 0;
	}

	/* Fill the rest of the trusted boot extension fields */
	sec_ext.box_id		= opts.sec_opts->box_id;
	sec_ext.flash_id	= opts.sec_opts->flash_id;
	sec_ext.efuse_dis	= opts.sec_opts->efuse_disable;
	sec_ext.jtag_delay	= opts.sec_opts->jtag_delay;
	sec_ext.jtag_en		= opts.sec_opts->jtag_enable;

	/* Write the resulting extention to file
	   (image and header signature fields are still empty) */

	/* Write extention header */
	written = fwrite(&header, sizeof(ext_header_t), 1, out_fd);
	if (written != 1) {
		fprintf(stderr, "Failed to write SEC extension header to the file\n");
		return 1;
	}
	/* Write extention body */
	written = fwrite(&sec_ext, sizeof(sec_entry_t), 1, out_fd);
	if (written != 1) {
		fprintf(stderr, "Failed to write SEC extension body to the file\n");
		return 1;
	}
	/* Write secure control field */
	written = fwrite(opts.sec_opts->control, sizeof(uint32_t) * (opts.sec_opts->control[0] + 1), 1, out_fd);
	if (written != 1) {
		fprintf(stderr, "Failed to write SEC extension control field to the file\n");
		return 1;
	}

	return 0;
}

/*******************************************************************************
*    finalize_secure_ext
*          Make final changes to secure extension - calculate image and header
*          signatures and encrypt the image if needed.
*          The main header checksum and image size fileds are updated accordingly
*    INPUT:
*          header       Main header
*          prolog_buf   the entire prolog buffer
*          prolog_size  prolog buffer length
*          image_buf    buffer containing the input binary image
*          image_size   image buffer size.
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int finalize_secure_ext(header_t *header,
			uint8_t *prolog_buf, uint32_t prolog_size,
			uint8_t *image_buf, int image_size)
{
	int		cur_ext, offset;
	uint8_t		*final_image = image_buf;
	uint32_t	final_image_sz = image_size;
	uint8_t		hdr_sign[RSA_SIGN_BYTE_LEN];
	sec_entry_t	*sec_ext = 0;

	/* Find the Trusted Boot Header between available extensions */
	for (cur_ext = 0, offset = sizeof(header_t); cur_ext < header->ext_count; cur_ext++) {
		ext_header_t *ext_hdr = (ext_header_t *)(prolog_buf + offset);

		if (ext_hdr->type == EXT_TYPE_SECURITY) {
			sec_ext = (sec_entry_t *)(prolog_buf + offset + sizeof(ext_header_t) + ext_hdr->offset);
			break;
		}

		offset += sizeof(ext_header_t);
		/* If offset is Zero, the extention follows its header */
		if (ext_hdr->offset == 0)
			offset += ext_hdr->size;
	}

	if (sec_ext == 0) {
		fprintf(stderr, "Error: No Trusted Boot extension found!\n");
		return -1;
	}

	if (sec_ext->encrypt_en) {
		/* Encrypt the image if needed */
		fprintf(stdout, "Encrypting the image...\n");

		if (image_encrypt(image_buf, image_size) != 0) {
			fprintf(stderr, "Failed to encrypt the image!\n");
			return -1;
		}

		/* Image size and checksum should be updated after encryption.
		   This way the image could be verified by BootROM before decryption. */
		final_image = opts.sec_opts->encrypted_image;
		final_image_sz = opts.sec_opts->enc_image_sz;

		header->boot_image_size = final_image_sz;
		header->boot_image_checksum = checksum32((uint32_t *)final_image, final_image_sz);
	} /* AES encryption */

	/* Create the image signature first, since it will be later
	   signed along with the header signature */
	if (create_rsa_signature(&opts.sec_opts->csk_pk[opts.sec_opts->csk_index],
				 final_image, final_image_sz,
				 opts.sec_opts->csk_key_file[opts.sec_opts->csk_index],
				 sec_ext->image_sign) != 0) {
		fprintf(stderr, "Failed to sign image!\n");
		return -1;
	}
	/* Check that the image signature is correct */
	if (verify_rsa_signature(sec_ext->csk_keys[opts.sec_opts->csk_index], MAX_RSA_DER_BYTE_LEN,
				 final_image, final_image_sz,
				 opts.sec_opts->csk_key_file[opts.sec_opts->csk_index],
				 sec_ext->image_sign) != 0) {
		fprintf(stderr, "Failed to verify image signature!\n");
		return -1;
	}

	/* Sign the headers and all the extensions block
	   when the header signature field is empty */
	if (create_rsa_signature(&opts.sec_opts->csk_pk[opts.sec_opts->csk_index],
				 prolog_buf, prolog_size,
				 opts.sec_opts->csk_key_file[opts.sec_opts->csk_index],
				 hdr_sign) != 0) {
		fprintf(stderr, "Failed to sign header!\n");
		return -1;
	}
	/* Check that the header signature is correct */
	if (verify_rsa_signature(sec_ext->csk_keys[opts.sec_opts->csk_index], MAX_RSA_DER_BYTE_LEN,
				 prolog_buf, prolog_size,
				 opts.sec_opts->csk_key_file[opts.sec_opts->csk_index],
				 hdr_sign) != 0) {
		fprintf(stderr, "Failed to verify header signature!\n");
		return -1;
	}

	/* Finally, copy the header signature into the trusted boot extension */
	memcpy(sec_ext->header_sign, hdr_sign, RSA_SIGN_BYTE_LEN);

	return 0;
}

#endif /* CONFIG_MVEBU_SECURE_BOOT */


#define FMT_HEX		0
#define FMT_DEC		1
#define FMT_BIN		2
#define FMT_NONE	3

void do_print_field(unsigned int value, char *name, int start, int size, int format)
{
	printf("[0x%05x : 0x%05x]  %-26s", start, start + size - 1, name);

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

	printf("########### Header ##############\n");
	print_field(main_hdr, header_t, magic, FMT_HEX, base);
	print_field(main_hdr, header_t, prolog_size, FMT_DEC, base);
	print_field(main_hdr, header_t, prolog_checksum, FMT_HEX, base);
	print_field(main_hdr, header_t, boot_image_size, FMT_DEC, base);
	print_field(main_hdr, header_t, boot_image_checksum, FMT_HEX, base);
	print_field(main_hdr, header_t, rsrvd0, FMT_HEX, base);
	print_field(main_hdr, header_t, load_addr, FMT_HEX, base);
	print_field(main_hdr, header_t, exec_addr, FMT_HEX, base);
	print_field(main_hdr, header_t, uart_cfg, FMT_HEX, base);
	print_field(main_hdr, header_t, baudrate, FMT_HEX, base);
	print_field(main_hdr, header_t, ext_count, FMT_DEC, base);
	print_field(main_hdr, header_t, aux_flags, FMT_HEX, base);
	print_field(main_hdr, header_t, io_arg_0, FMT_HEX, base);
	print_field(main_hdr, header_t, io_arg_1, FMT_HEX, base);
	print_field(main_hdr, header_t, io_arg_2, FMT_HEX, base);
	print_field(main_hdr, header_t, io_arg_3, FMT_HEX, base);
	print_field(main_hdr, header_t, rsrvd1, FMT_HEX, base);
	print_field(main_hdr, header_t, rsrvd2, FMT_HEX, base);
	print_field(main_hdr, header_t, rsrvd3, FMT_HEX, base);

	return sizeof(header_t);
}

int print_ext_hdr(ext_header_t *ext_hdr, int base)
{
	print_field(ext_hdr, ext_header_t, type, FMT_HEX, base);
	print_field(ext_hdr, ext_header_t, offset, FMT_HEX, base);
	print_field(ext_hdr, ext_header_t, reserved, FMT_HEX, base);
	print_field(ext_hdr, ext_header_t, size, FMT_DEC, base);

	return base + sizeof(ext_header_t);
}

void print_sec_ext(ext_header_t *ext_hdr, int base)
{
	uint32_t	*control;
	sec_entry_t	*sec_entry;
	uint32_t	new_base;

	fprintf(stdout, "\n########### Secure extension ###########\n");

	new_base = print_ext_hdr(ext_hdr, base);

	sec_entry = (sec_entry_t *)(ext_hdr + 1);
	control = (uint32_t *)(sec_entry + 1);

	do_print_field(0, "KAK key", new_base, MAX_RSA_DER_BYTE_LEN, FMT_NONE);
	new_base += MAX_RSA_DER_BYTE_LEN;
	print_field(sec_entry, sec_entry_t, jtag_delay, FMT_DEC, base);
	print_field(sec_entry, sec_entry_t, box_id, FMT_HEX, base);
	print_field(sec_entry, sec_entry_t, flash_id, FMT_HEX, base);
	print_field(sec_entry, sec_entry_t, encrypt_en, FMT_DEC, base);
	print_field(sec_entry, sec_entry_t, efuse_dis, FMT_DEC, base);
	new_base += 6 * sizeof(uint32_t);
	do_print_field(0, "header signature", new_base, RSA_SIGN_BYTE_LEN, FMT_NONE);
	new_base += RSA_SIGN_BYTE_LEN;
	do_print_field(0, "image signature", new_base, RSA_SIGN_BYTE_LEN, FMT_NONE);
	new_base += RSA_SIGN_BYTE_LEN;
	do_print_field(0, "CSK keys", new_base, CSK_ARR_SZ * MAX_RSA_DER_BYTE_LEN, FMT_NONE);
	new_base += CSK_ARR_SZ * MAX_RSA_DER_BYTE_LEN;
	do_print_field(0, "CSK block signature", new_base, RSA_SIGN_BYTE_LEN, FMT_NONE);
	new_base += RSA_SIGN_BYTE_LEN;
	do_print_field(control[0], "control", new_base, sizeof(uint32_t) * (control[0] + 1), FMT_HEX);

}

void print_reg_ext(ext_header_t *ext_hdr, int base)
{
	uint32_t *reg_list;
	int size = ext_hdr->size;
	int i = 0;

	printf("\n########### Register extension #########\n");

	base = print_ext_hdr(ext_hdr, base);

	reg_list = (uint32_t *)((uintptr_t)(ext_hdr) + sizeof(ext_header_t));
	while (size) {
		do_print_field(reg_list[i++], "address", base, 4, FMT_HEX);
		do_print_field(reg_list[i++], "value  ", base + 4, 4, FMT_HEX);
		base += 8;
		size -= 8;
	}
}

void print_bin_ext(ext_header_t *ext_hdr, int base)
{
	printf("\n########### Binary extension ###########\n");
	base = print_ext_hdr(ext_hdr, base);
	do_print_field(0, "binary image", base, ext_hdr->size, FMT_NONE);
}

int print_extension(void *buf, int base, int count, int ext_size)
{
	ext_header_t *ext_hdr = buf;
	int pad = ext_size;
	int curr_size;

	while (count--) {
		if (ext_hdr->type == EXT_TYPE_BINARY)
			print_bin_ext(ext_hdr, base);
		else if (ext_hdr->type == EXT_TYPE_REGISTER)
			print_reg_ext(ext_hdr, base);
		else if (ext_hdr->type == EXT_TYPE_SECURITY)
			print_sec_ext(ext_hdr, base);

		curr_size = sizeof(ext_header_t) + ext_hdr->size;

		base += curr_size;
		pad  -= curr_size;
		ext_hdr = (ext_header_t *)((uintptr_t)ext_hdr + curr_size);
	}

	if (pad)
		do_print_field(0, "padding", base, pad, FMT_NONE);

	return ext_size;
}

int parse_image(uint8_t *buf, int size)
{
	int base = 0;
	int ret = 1;
	header_t *main_hdr;
	uint32_t checksum, prolog_checksum;


	printf("################### Prolog Start ######################\n\n");
	main_hdr = (header_t *)buf;
	base += print_header(buf, base);

	if (main_hdr->ext_count)
		base += print_extension(buf + base, base, main_hdr->ext_count,
					main_hdr->prolog_size - sizeof(header_t));

	if (base < main_hdr->prolog_size) {
		printf("\n########### Padding ##############\n");
		do_print_field(0, "prolog padding", base, main_hdr->prolog_size - base, FMT_HEX);
		base = main_hdr->prolog_size;
	}
	printf("\n################### Prolog End ######################\n");

	printf("\n################### Boot image ######################\n");

	do_print_field(0, "boot image", base, size - base - 4, FMT_NONE);

	printf("################### Image end ########################\n");

	/* Check sanity for certain values */
	printf("\nChecking values:\n");

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
	checksum = checksum32((uint32_t *)buf, main_hdr->prolog_size);

	if (checksum == prolog_checksum) {
		printf("Headers checksum: OK!\n");
	} else {
		printf("\n****** ERROR: BAD HEADER CHECKSUM 0x%08x != 0x%08x ********\n",
		       checksum, prolog_checksum);
		goto error;
	}

	/* boot image checksum */
	checksum = checksum32((uint32_t *)(buf + main_hdr->prolog_size), main_hdr->boot_image_size);
	if (checksum == main_hdr->boot_image_checksum) {
		printf("Image checksum:   OK!\n");
	} else {
		printf("\n****** ERROR: BAD IMAGE CHECKSUM 0x%08x != 0x%08x ********\n",
		       checksum, main_hdr->boot_image_checksum);
		goto error;
	}

#ifdef CONFIG_MVEBU_SECURE_BOOT
	/* RSA signatures */
	if (main_hdr->ext_count) {
		uint8_t		ext_num = main_hdr->ext_count;
		ext_header_t	*ext_hdr = (ext_header_t *)(main_hdr + 1);

		while (ext_num--) {
			if (ext_hdr->type == EXT_TYPE_SECURITY) {
				sec_entry_t  *sec_entry = (sec_entry_t *)(ext_hdr + 1);

				ret = verify_secure_header_signatures(main_hdr, sec_entry);
				if (ret != 0) {
					fprintf(stderr, "\n****** FAILED TO VERIFY RSA SIGNATURES ********\n");
					goto error;
				}
				break;
			}
			ext_hdr = (ext_header_t *)((uint8_t *)(ext_hdr + 1) + ext_hdr->size);
		}
	}
#endif

	ret = 0;
error:
	return ret;
}

int format_reg_ext(char *filename, FILE *out_fd)
{
	ext_header_t header;
	FILE *in_fd;
	int size, written;
	uint32_t *buf;
	int line_id = 0, entry = 0, i;
	char *addr, *value;
	char line[256];

	in_fd = fopen(filename, "rb");
	if (in_fd == NULL) {
		printf("failed to open reg extension file %s\n", filename);
		return 1;
	}

	size = get_file_size(filename);
	if (size <= 0) {
		printf("reg extension file size is bad\n");
		return 1;
	}

	buf = malloc(size);
	while (fgets(line, sizeof(line), in_fd)) {
		line_id++;
		if (line[0] == '#' || line[0] == ' ')
			continue;

		if (strlen(line) <= 1)
			continue;

		if (line[0] != '0') {
			printf("Bad register file format at line %d\n", line_id);
			return 1;
		}

		addr = strtok(&line[0], " \t");
		value = strtok(NULL, " \t");
		if ((addr == NULL) || (value == NULL)) {
			printf("Bad register file format at line %d\n", line_id);
			return 1;
		}

		buf[entry++]     = strtoul(addr, NULL, 0);
		buf[entry++] = strtoul(value, NULL, 0);
	}

	header.type = EXT_TYPE_REGISTER;
	header.offset = 0;
	header.size = (entry * 4);
	header.reserved = 0;

	/* Write header */
	written = fwrite(&header, sizeof(ext_header_t), 1, out_fd);
	if (written != 1) {
		printf("failed writing header to extension file\n");
		return 1;
	}

	/* Write register */
	for (i = 0; i < entry; i++)
		fwrite(&buf[i], 4, 1, out_fd);

	free(buf);
	fclose(in_fd);
	return 0;
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
	if (strncmp(opts.reg_ext_file, "NA", MAX_FILENAME)) {
		if (format_reg_ext(opts.reg_ext_file, out_fd)) {
			ret = 1;
			goto error;
		}
	}
#ifdef CONFIG_MVEBU_SECURE_BOOT
	if (strncmp(opts.sec_cfg_file, "NA", MAX_FILENAME)) {
		if (format_sec_ext(opts.sec_cfg_file, out_fd)) {
			ret = 1;
			goto error;
		}
	}
#endif

error:
	fflush(out_fd);
	fclose(out_fd);
	return ret;
}

void update_uart(header_t *header)
{
	header->uart_cfg = 0;
	header->baudrate = 0;

	if (opts.disable_print)
		uart_set_mode(header->uart_cfg, UART_MODE_DISABLE);

	if (opts.baudrate)
		header->baudrate = (opts.baudrate / 1200);
}

/* ****************************************
 *
 * Write the image prolog, i.e.
 * main header and extensions, to file
 *
 * ****************************************/

int write_prolog(int ext_cnt, char *ext_filename, uint8_t *image_buf, int image_size, FILE *out_fd)
{
	header_t		*header;
	int main_hdr_size = sizeof(header_t);
	int prolog_size = main_hdr_size;
	FILE *ext_fd;
	char *buf;
	int written, read;
	int ret = 1;


	if (ext_cnt)
		prolog_size +=  get_file_size(ext_filename);

	prolog_size = ((prolog_size + PROLOG_ALIGNMENT) & (~(PROLOG_ALIGNMENT-1)));

	/* Allocate a zeroed buffer to zero the padding bytes */
	buf = calloc(prolog_size, 1);
	if (buf == NULL) {
		printf("Error: failed allocating checksum buffer\n");
		return 1;
	}

	header = (header_t *)buf;
	header->magic       = MAIN_HDR_MAGIC;
	header->prolog_size = prolog_size;
	header->load_addr   = opts.load_addr;
	header->exec_addr   = opts.exec_addr;
	header->ext_count   = ext_cnt;
	header->aux_flags     = 0;
	header->boot_image_size = (image_size + 3) & (~0x3);
	header->boot_image_checksum = checksum32((uint32_t *)image_buf, image_size);

	update_uart(header);

	/* Populate buffer with main header and extensions */
	if (ext_cnt) {
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

#ifdef CONFIG_MVEBU_SECURE_BOOT
		/* Secure boot mode? */
		if (opts.sec_opts != 0) {
			ret = finalize_secure_ext(header, (uint8_t *)buf,
						  prolog_size, image_buf, image_size);
			if (ret != 0) {
				printf("Error: failed to handle secure extension!\n");
				goto error;
			}
		} /* secure boot mode */
#endif
	}

	/* Update the total prolog checksum */
	header->prolog_checksum = checksum32((uint32_t *)buf, prolog_size);

	/* Now spill everything to output file */
	written = fwrite(buf, prolog_size, 1, out_fd);
	if (written != 1) {
		printf("Error: failed to write prolog to output file\n");
		goto error;
	}

	ret = 0;

error:
	free(buf);
	return ret;
}

int write_boot_image(uint8_t *buf, uint32_t image_size, FILE *out_fd)
{
	int aligned_size;
	int written;

	/* Image size must be aligned to 4 bytes */
	aligned_size = (image_size + 3) & (~0x3);

	written = fwrite(buf, aligned_size, 1, out_fd);
	if (written != 1) {
		printf("Error: Failed to write boot image\n");
		goto error;
	}

	return 0;
error:
	return 1;
}

int main(int argc, char *argv[])
{
	char in_file[MAX_FILENAME];
	char out_file[MAX_FILENAME];
	FILE *in_fd = NULL;
	FILE *out_fd = NULL;
	int parse = 0;
	int ext_cnt = 0;
	int opt;
	int ret = 0;
	int image_size;
	uint8_t *image_buf = NULL;
	int read;
	uint32_t nand_block_size_kb, mlc_nand;

	while ((opt = getopt(argc, argv, "hpms:i:l:e:a:b:r:u:n:t:c:k:")) != -1) {
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
		case 'm':
			opts.disable_print = 1;
			break;
		case 'u':
			opts.baudrate = strtoul(optarg, NULL, 0);
			break;
		case 'b':
			strncpy(opts.bin_ext_file, optarg, MAX_FILENAME);
			ext_cnt++;
			break;
		case 'r':
			strncpy(opts.reg_ext_file, optarg, MAX_FILENAME);
			ext_cnt++;
			break;
		case 'p':
			parse = 1;
			break;
		case 'n':
			nand_block_size_kb = strtoul(optarg, NULL, 0);
			opts.nfc_io_args |= (nand_block_size_kb / 64);
			break;
		case 't':
			mlc_nand = 0;
			if (!strncmp("MLC", optarg, 3))
				mlc_nand = 1;
			opts.nfc_io_args |= (mlc_nand << 8);
			break;
#ifdef CONFIG_MVEBU_SECURE_BOOT
		case 'c': /* SEC extension */
			strncpy(opts.sec_cfg_file, optarg, MAX_FILENAME);
			ext_cnt++;
			break;
		case 'k':
			opts.key_index = strtoul(optarg, NULL, 0);
			break;
#endif
		default: /* '?' */
			usage_err("Unknown argument");
			exit(EXIT_FAILURE);
		}
	}

	/* Check validity of inputes */
	if (opts.load_addr % 8)
		usage_err("Load address must be 8 bytes aligned");

	if (opts.baudrate % 1200)
		usage_err("Baudrate must be a multiple of 1200");

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

	/* Read the input file to buffer */
	image_size = get_file_size(in_file);
	image_buf = calloc((image_size + AES_BLOCK_SZ - 1) & ~(AES_BLOCK_SZ - 1), 1);
	if (image_buf == NULL) {
		printf("Error: failed allocating input buffer\n");
		return 1;
	}

	read = fread(image_buf, image_size, 1, in_fd);
	if (read != 1) {
		printf("Error: failed to read input file\n");
		goto main_exit;
	}

	/* Parse the input image and leave */
	if (parse) {
		if (opts.key_index >= CSK_ARR_SZ) {
			fprintf(stderr, "Wrong key index value. Supported values 0 - %d\n", CSK_ARR_SZ - 1);
			goto main_exit;
		}
		ret = parse_image(image_buf, image_size);
		goto main_exit;
	}

	/* Create a blob file from all extensions */
	if (ext_cnt) {
		ret = format_extensions(EXT_FILENAME);
		if (ret)
			goto main_exit;
	}

	out_fd = fopen(out_file, "wb");
	if (out_fd == NULL) {
		printf("Error: Failed to open output file %s\n", out_file);
		goto main_exit;
	}

	ret = write_prolog(ext_cnt, EXT_FILENAME, image_buf, image_size, out_fd);
	if (ret)
		goto main_exit;

#ifdef CONFIG_MVEBU_SECURE_BOOT
	if ((opts.sec_opts->encrypted_image != 0) && (opts.sec_opts->enc_image_sz != 0))
		ret = write_boot_image(opts.sec_opts->encrypted_image, opts.sec_opts->enc_image_sz, out_fd);
	else
#endif
	ret = write_boot_image(image_buf, image_size, out_fd);
	if (ret)
		goto main_exit;

main_exit:
	if (in_fd)
		fclose(in_fd);

	if (out_fd)
		fclose(out_fd);

	if (image_buf)
		free(image_buf);

#ifdef CONFIG_MVEBU_SECURE_BOOT
	if (opts.sec_opts) {
		if (opts.sec_opts->encrypted_image)
			free(opts.sec_opts->encrypted_image);
		free(opts.sec_opts);
	}
#endif
	exit(ret);
}

