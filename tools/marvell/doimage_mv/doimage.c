/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <limits.h>

#define _HOST_COMPILER
#include "bootstrap_def.h"

#include "polarssl/havege.h"
#include "polarssl/sha2.h"
#include "polarssl/rsa.h"
#include "polarssl/aes.h"
#include "doimage.h"

#include "polarssl/mvMemPool.h"

#undef DEBUG

#ifdef DEBUG
#define DB(x...)		fprintf(stdout, x);
#else
#define DB(x...)
#endif

/* Global variables */

int 		f_in = -1;
int 		f_out = -1;
int 		f_header = -1;
struct stat 	fs_stat;
rsa_context	rsa;
aes_context	aes;
unsigned char	IV[16] = {0};

/*******************************************************************************
*    create_rsa_signature (memory buffer content)
*          create RSA-2048 signature for memory buffer
*    INPUT:
*          input      memory buffer
*          ilen       buffer length
*          print_val  if not 0, print SHA-256 digest of the memory buffer (debug)
*    OUTPUT:
*          signature  RSA-2048 signature
*    RETURN:
*          0 on success
*******************************************************************************/
int create_rsa_signature (
			unsigned char	*input,
			int 		ilen,
			unsigned char	*signature,
			char		*print_val)
{
	unsigned char	sha_256[32];
	int i;

	memset(sha_256, 0, 32 * sizeof(unsigned char));

	/* compute SHA-256 digest */
	sha2(input, ilen, sha_256, 0);

	if (print_val != NULL) {
		printf("\n%s ", print_val);
		for (i = 0; i < 32; i++)
			printf("%02X", sha_256[i]);
		printf("\n");
	}

	return rsa_pkcs1_sign(&rsa, RSA_PRIVATE, RSA_SHA256, 32, sha_256, signature);

} /* end of create_rsa_signature() */

/*******************************************************************************
*    verify_rsa_signature (memory buffer content)
*          verify RSA-2048 signature for memory buffer
*    INPUT:
*          input      memory buffer
*          ilen       buffer length
          signature  RSA-2048 signature
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int verify_rsa_signature (
			unsigned char	*input,
			int 		ilen,
			unsigned char	*signature)
{
	unsigned char	sha_256[32];

	memset(sha_256, 0, 32 * sizeof(unsigned char));

	/* compute SHA-256 digest */
	sha2(input, ilen, sha_256, 0);

	return rsa_pkcs1_verify(&rsa, RSA_PUBLIC, RSA_SHA256, 32, sha_256, signature);

} /* end of create_rsa_signature() */

/*******************************************************************************
*    pre_load_image
*          pre-load the binary image into memory buffer taking into account paddings
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int pre_load_image (USER_OPTIONS *opt, char *buf_in)
{
	int	offset = 0;

	opt->image_buf = malloc(opt->image_sz);
	if (opt->image_buf == NULL)
		return -1;

	memset(opt->image_buf, 0, opt->image_sz);

	if ((opt->pre_padding) && (opt->prepadding_size)) {
		memset(opt->image_buf, 0x5, opt->prepadding_size);
		offset = opt->prepadding_size;
	}

	if ((opt->post_padding) && (opt->postpadding_size))
		memset(opt->image_buf + opt->image_sz - 4 - opt->postpadding_size, 0xA, opt->postpadding_size);

	memcpy(opt->image_buf + offset, buf_in, fs_stat.st_size);

	/* IV must be the last before checksum */
	if (opt->flags & A_OPTION_MASK)
		memcpy(opt->image_buf + opt->image_sz - 20, IV, 16);

	return 0;
} /* end of pre_load_image() */

/*******************************************************************************
*    build_twsi_header
*          create TWSI header and write it into output stream
*    INPUT:
*          opt        user options
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_twsi_header (USER_OPTIONS *opt)
{
	FILE	*f_twsi;
	MV_U8   *tmpTwsi = NULL;
	MV_U32	*twsi_reg;
	int     i;
	MV_U32  twsi_size = 0;

	tmpTwsi = malloc(MAX_TWSI_HDR_SIZE);
	if (tmpTwsi == NULL) {
		fprintf(stderr,"TWSI space allocation error!\n");
		return -1;
	}
	memset(tmpTwsi, 0xFF, MAX_TWSI_HDR_SIZE);
	twsi_reg = (MV_U32 *)tmpTwsi;

	f_twsi = fopen(opt->fname_twsi, "r");
	if (f_twsi == NULL) {
		fprintf(stderr,"Failed to open file '%s'\n", opt->fname_twsi);
		perror("Error:");
		return -1;
	}

	for (i = 0; i < (MAX_TWSI_HDR_SIZE / 4); i++) {
		if (EOF == fscanf(f_twsi,"%x\n",twsi_reg))
			break;

		/* Swap Enianess */
		*twsi_reg = ( ((*twsi_reg >> 24) & 0xFF)	|
				((*twsi_reg >> 8)  & 0xFF00)	|
				((*twsi_reg << 8)  & 0xFF0000)	|
				((*twsi_reg << 24) & 0xFF000000) );
		twsi_reg++;
	}

	fclose(f_twsi);

	/* Align to size = 512,1024,.. with at least 8 0xFF bytes @ the end */
	twsi_size = ((((i + 2) * 4) & ~0x1FF) + 0x200);

	if ((write(f_out, tmpTwsi, twsi_size)) != twsi_size) {
		fprintf(stderr,"Error writing %s file \n",opt->fname.out);
		return -1;
	}

	return 0;
} /* end of build_twsi_header() */

/*******************************************************************************
*    build_reg_header
*        create BIN header and write it into output stream
*    INPUT:
*       fname        - source file name
*       buffer       - Start address of boot image buffer
*       current_size - number of bytes already placed into the boot image buffer
*    OUTPUT:
*       none
*    RETURN:
*	size of a reg header or 0 on fail
*******************************************************************************/
int build_reg_header (char *fname, MV_U8 *buffer, MV_U32 current_size)
{
	FILE		*f_dram;
	BHR_t		*mainHdr = (BHR_t *)buffer;
	headExtBHR_t	*headExtHdr = headExtHdr = (headExtBHR_t *)(buffer + current_size);
	tailExtBHR_t	*prevExtHdrTail = NULL; /* tail of the previous extention header */
	MV_U32		max_bytes_to_write;
	MV_U32		*dram_reg = (MV_U32 *)(buffer + current_size + sizeof(headExtBHR_t));
	MV_U32		tmp_len;
	int		i;

	if (mainHdr->ext == 255) {
		fprintf(stderr,"Maximum number of extentions reached!\n");
		return 0;
	}

	/* Indicate next header in previous extention if any */
	if (mainHdr->ext != 0) {
		prevExtHdrTail = (tailExtBHR_t*)(buffer + current_size - sizeof(tailExtBHR_t));
		prevExtHdrTail->nextHdr = 1;
	}

	/* Count extension headers in the main header */
	mainHdr->ext++;

	headExtHdr->type = EXT_HDR_TYP_REGISTER;
	max_bytes_to_write = MAX_HEADER_SIZE - current_size - EXT_HDR_BASE_SIZE;

	if ((f_dram = fopen(fname, "r")) == NULL) {
		fprintf(stderr,"Failed to open file '%s'\n", fname);
		perror("Error:");
		return 0;
	}

	for (i = 0; i < (max_bytes_to_write / 8); i += 2) {
		if (fscanf(f_dram,"%x %x\n",&dram_reg[i], &dram_reg[i+1]) == EOF)
			break;
		else if ((dram_reg[i] == 0x0) && (dram_reg[i+1] == 0x0))
			break;
	}

	fclose(f_dram);

	if (i >= (max_bytes_to_write / 8)) {
		fprintf(stderr,"Registers configuration exceeds the maximum"
				"size of %d bytes\n", max_bytes_to_write);
		return 0;
	}

	/* Include extended header head and tail sizes */
	tmp_len = EXT_HDR_BASE_SIZE + i * 4;
	/* Write total length into the current header fields */
	EXT_HDR_SET_LEN(headExtHdr, tmp_len);

	return tmp_len;
} /* end of build_reg_header() */

/*******************************************************************************
*    build_bin_header
*        create BIN header and write it into putput stream
*    INPUT:
*       fname        - source file name
*       buffer       - Start address of boot image buffer
*       current_size - number of bytes already placed into the boot image buffer
*    OUTPUT:
*       none
*    RETURN:
*	size of reg header
*******************************************************************************/
int build_bin_header (char *fname, MV_U8 *buffer, MV_U32 current_size)
{
	FILE		*f_bin;
	BHR_t		*mainHdr = (BHR_t *)buffer;
	headExtBHR_t	*headExtHdr = (headExtBHR_t *)(buffer + current_size);
	tailExtBHR_t	*prevExtHdrTail = NULL; /* tail of the previous extention header */
	MV_U32		max_bytes_to_write;
	MV_U32		*bin_reg = (MV_U32 *)(buffer + current_size + sizeof(headExtBHR_t));
	MV_U32		tmp_len;
	int		i;

	if (mainHdr->ext == 255) {
		fprintf(stderr,"Maximum number of extentions reached!\n");
		return 0;
	}

	/* Indicate next header in previous extention if any */
	if (mainHdr->ext != 0) {
		prevExtHdrTail = (tailExtBHR_t*)(buffer + current_size - sizeof(tailExtBHR_t));
		prevExtHdrTail->nextHdr = 1;
	}

	/* Count extension headers in main header */
	mainHdr->ext++;

	headExtHdr->type = EXT_HDR_TYP_BINARY;
	max_bytes_to_write = MAX_HEADER_SIZE - current_size - EXT_HDR_BASE_SIZE;

	f_bin = fopen(fname, "r");
	if (f_bin == NULL) {
		fprintf(stderr,"Failed to open file '%s'\n", fname);
		perror("Error:");
		return 0;
	}

	for (i = 0; i < (max_bytes_to_write / 4); i++) {
		if (fread (bin_reg, 1, 4, f_bin) != 4)
			break;

		bin_reg++;
	}

	fclose(f_bin);

	if (i >= (max_bytes_to_write / 4)) {
		fprintf(stderr,"Binary extention exeeds the maximum size "
				"of %d bytes\n", max_bytes_to_write);
	return 0;
}

	/* Include extended header head and tail sizes */
	tmp_len = EXT_HDR_BASE_SIZE + i * 4;
	/* Write total length into the current header fields */
	EXT_HDR_SET_LEN(headExtHdr, tmp_len);

	return tmp_len;
} /* end of build_exec_header() */

/*******************************************************************************
*    build_headers
*          build headers block based on user options and write it into output stream
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_headers (USER_OPTIONS	*opt, char *buf_in)
{
	BHR_t          *hdr = NULL;
	secExtBHR_t    *secExtHdr = NULL;
	headExtBHR_t   *headExtHdr = NULL;
	tailExtBHR_t   *tailExtHdr = NULL;
	MV_U8          *tmpHeader = NULL;
	int            i;
	MV_U32         header_size = 0;
	int            size_written = 0;
	MV_U32         max_bytes_to_write;
	int            error = 1;

	tmpHeader = malloc(MAX_HEADER_SIZE);
	if (tmpHeader == NULL) {
		fprintf(stderr,"Header space allocation error!\n");
		goto header_error;
	}

	memset(tmpHeader, 0 , MAX_HEADER_SIZE);
	hdr = (BHR_t *)tmpHeader;

	switch (opt->image_type) {
		case IMG_SATA:
			hdr->blockID = IBR_HDR_SATA_ID;
			break;

		case IMG_UART:
			hdr->blockID = IBR_HDR_UART_ID;
			break;

		case IMG_FLASH:
			hdr->blockID = IBR_HDR_SPI_ID;
			break;

		case IMG_MMC:
			hdr->blockID = IBR_HDR_MMC_ID;
			break;

		case IMG_NAND:
			hdr->blockID = IBR_HDR_NAND_ID;
			/*hdr->nandEccMode = (MV_U8)opt->nandEccMode; <<== reserved */
			/*hdr->nandPageSize = (MV_U16)opt->nandPageSize; <<== reserved */
			hdr->nandBlockSize = (MV_U8)opt->nandBlkSize;
			if ((opt->nandCellTech == 'S') || (opt->nandCellTech == 's'))
				hdr->nandTechnology = MAIN_HDR_NAND_SLC;
			else
				hdr->nandTechnology = MAIN_HDR_NAND_MLC;
			break;

		case IMG_PEX:
			hdr->blockID = IBR_HDR_PEX_ID;
			break;

		case IMG_I2C:
			hdr->blockID = IBR_HDR_I2C_ID;
			break;

		default:
			fprintf(stderr,"Illegal image type %d for header construction!\n",
					opt->image_type);
			return 1;
	}

	hdr->destinationAddr	= opt->image_dest;
	hdr->executionAddr	= (MV_U32)opt->image_exec;
	hdr->version		= MAIN_HDR_VERSION;
	hdr->blockSize		= fs_stat.st_size;

	header_size = sizeof(BHR_t);

	/* Update Block size address */
	if ((opt->flags & X_OPTION_MASK) || (opt->flags & Y_OPTION_MASK)) {
		/* Align padding to 32 bit */
		if (opt->prepadding_size & 0x3)
			opt->prepadding_size += (4 - (opt->prepadding_size & 0x3));

		if (opt->postpadding_size & 0x3)
			opt->postpadding_size += (4 - (opt->postpadding_size & 0x3));

		hdr->blockSize += opt->prepadding_size + opt->postpadding_size;
	}

	/* Align the image size to 4 byte boundary */
	if (hdr->blockSize & 0x3) {
		opt->bytesToAlign  = (4 - (hdr->blockSize & 0x3));
		hdr->blockSize     += opt->bytesToAlign;
	}

	/* Create extension header(s) */
	/* Security Header  - always single and always first after the Main header */
	if (opt->flags & Z_OPTION_MASK) {
		if (opt->flags & A_OPTION_MASK) {
			int align16 = 0;

			if (hdr->blockSize & 0xF)
				align16 = 16 - hdr->blockSize & 0xF;
			/* At this stage everything is 4-bytes aligned          */
			/* For AES-128 the image should be aligned to 16 bytes  */
			/* Additional 16 bytes are also required for IV storage */
			hdr->blockSize        += align16 + 16;
			opt->postpadding_size += align16;
			opt->post_padding      = 1;
		}

		/* Allocate space for security header, but do not fill in it now */
		secExtHdr = (secExtBHR_t *)(tmpHeader + header_size);
		secExtHdr->head.type = EXT_HDR_TYP_SECURITY;
		header_size += sizeof(secExtBHR_t);
		/* Indicate extention header existance in main header */
		hdr->ext = 1;
		/* Prepare header extension tail for next extension (if any) */
		tailExtHdr = &secExtHdr->tail;
	}

	/***************************** TWSI Header ********************************/

	/* TWSI header has a special purpose and placed ahead of the main header */
	if (opt->flags & M_OPTION_MASK) {
		if (opt->fname_twsi) {
			if (build_twsi_header(opt) != 0)
				goto header_error;
		} /* opt->fname_twsi */
	} /* (opt->flags & M_OPTION_MASK) */

	/************************** End of TWSI Header ****************************/

	/********************** Single Register Header ***************************/

	if (opt->flags & R_OPTION_MASK) {
		if (opt->fname_dram) {
			MV_U32	 rhdr_len = build_reg_header(opt->fname_dram, tmpHeader, header_size);
			if (rhdr_len <= 0)
				goto header_error;

			header_size  += rhdr_len;
			tailExtHdr = (tailExtBHR_t *)(tmpHeader + header_size - sizeof(tailExtBHR_t));
		} /* if (fname_dram) */
	} /* if (opts & R_OPTION_MASK) */

	/******************** End of Single Register Header ************************/

	/************************* Single Binary Header ****************************/

	if (opt->flags & G_OPTION_MASK) {
		if (opt->fname_bin) {
			MV_U32	 bhdr_len = build_bin_header(opt->fname_bin, tmpHeader, header_size);
			if (bhdr_len <= 0)
				goto header_error;

			header_size  += bhdr_len;
			tailExtHdr = (tailExtBHR_t *)(tmpHeader + header_size - sizeof(tailExtBHR_t));
		} /* if (fname_bin) */
	} /* (opt->flags & G_OPTION_MASK) */

	/******************* End of Single Binary Header ***************************/

	/************************* BIN/REG Headers list ****************************/

	if (opt->flags & C_OPTION_MASK) {
		if (opt->fname_list) {
			FILE	*f_list;
			char	buffer[PATH_MAX + 5];
			char	*fname;
			MV_U32	hdr_len = 0, last;
			int (*build_hdr_func)(char*, MV_U8*, MV_U32);

			f_list = fopen(opt->fname_list, "r");
			if (f_list == NULL) {
				fprintf(stderr,"File '%s' not found \n", opt->fname_list);
				goto header_error;
			}
			/* read the headers list row by row */
			while (fgets(buffer, PATH_MAX + 4, f_list) != NULL) {
				/* Ignore strings that are not starting with BIN/REG */
				if (strncmp(buffer, "BIN", 3) == 0) {
					build_hdr_func = build_bin_header;
				} else if (strncmp(buffer, "REG", 3) == 0) {
					build_hdr_func = build_reg_header;
				} else
					continue;

				fname = buffer + 3;
				/* strip leading spaces/tabs if any */
				while ((strncmp(fname, " ", 1) == 0) || (strncmp(fname, "\t", 1) == 0))
					fname++;

				/* strip trailing LF/CR symbols */
				last = strlen(fname) - 1;
				while ((strncmp(fname + last, "\n", 1) == 0) ||
					(strncmp(fname + last, "\r", 1) == 0)) {
					fname[last] = 0;
					last--;
				}
				/* Insert required header into the output buffer */
				hdr_len = build_hdr_func(fname, tmpHeader, header_size);
				if (hdr_len <= 0)
				goto header_error;

				header_size  += hdr_len;
			tailExtHdr = (tailExtBHR_t *)(tmpHeader + header_size - sizeof(tailExtBHR_t));
		}

			fclose(f_list);
		} /* if (fname_list) */
	}/* (opt->flags & C_OPTION_MASK) */

	/********************** End of BIN/REG Headers list ************************/

	/* Align the headers block to... */
	if (opt->image_type == IMG_NAND) {
		/* ... NAND page size */
		header_size += opt->nandPageSize - (header_size & (opt->nandPageSize - 1));
	} else if (opt->image_type == IMG_SATA) {
		/* ... disk logical block size */
		header_size += 512 - (header_size & 0x1FF);
	} else if (opt->image_type == IMG_UART) {
		/* ... Xmodem packet size */
		header_size += 128 - (header_size & 0x7F);
	}
	/* Setup the image source address */
	if (opt->image_type == IMG_SATA) {
		if ((opt->image_source) && (opt->image_source > header_size))
		hdr->sourceAddr = opt->image_source;
	else
			hdr->sourceAddr = header_size >> 9; /* Already aligned to 512 */
	} else {
		if ((opt->image_source) && (opt->image_source > header_size)) {
			hdr->sourceAddr = opt->image_source;
			opt->img_gap =  opt->image_source - header_size;
		} else {
			/* The source imgage address should be aligned
			   to 32 byte boundary (cache line size).
			   For NAND it should be aligned to 512 bytes boundary (for ECC)
			   The image immediately follows the header block,
			   so if the source addess is undefined, it should be
			   derived from the header size.
			   The headers size is always  alighed to 4 byte boundary */
			int boundary = 32;

			if (opt->image_type == IMG_NAND)
				boundary = 512;

			if (header_size & (boundary - 1))
				opt->img_gap =  boundary - (header_size & (boundary - 1));

			hdr->sourceAddr = header_size + opt->img_gap;
		}
	}

	/* source address and extension headers number can be written now */
	fprintf(stdout, "Ext. headers = %d, Header size = %d bytes Hdr-to-Img gap = %d bytes\n",
			hdr->ext, header_size, opt->img_gap);

	/* If not UART/TWSI image, an extra word for boot image checksum is needed */
	if ((opt->image_type == IMG_FLASH)	||
	    (opt->image_type == IMG_NAND)	||
	    (opt->image_type == IMG_MMC)	||
	    (opt->image_type == IMG_SATA)	||
	    (opt->image_type == IMG_PEX)	||
	    (opt->image_type == IMG_I2C))
		hdr->blockSize += 4;

	fprintf(stdout, "New image size = %#x[%d] Source image size = %#x[%d]\n",
			hdr->blockSize, hdr->blockSize, (unsigned int)fs_stat.st_size, (int)fs_stat.st_size);

	hdr->hdrSizeMsb  = (header_size & 0x00FF0000) >> 16;
	hdr->hdrSizeLsb  =  header_size & 0x0000FFFF;

	opt->image_sz = hdr->blockSize;

	/* Load image into memory buffer */
	if (REGULAR_IMAGE(opt)) {
		if (0 != pre_load_image(opt, buf_in)) {
			fprintf(stderr, "Failed image pre-load!\n");
			goto header_error;
		}
	}

	/* Security Header should be filled after all the rest of headers */
	/* Since it includes the headers block RSA signature */
	if (opt->flags & Z_OPTION_MASK) {
		MV_U16		rsa_exp_len;
		MV_U16		rsa_key_len;
		unsigned char	rsa_signature[RSA_MAX_KEY_LEN_BYTES];
		FILE		*f_sha;
		unsigned char	rsa_digest[32];
		int		k;

		if(opt->flags & B_OPTION_MASK)
			secExtHdr->boxId = opt->boxId;

		if(opt->flags & F_OPTION_MASK)
			secExtHdr->flashId = opt->flashId;

		if(opt->flags & J_OPTION_MASK)
			secExtHdr->jtagEn = opt->jtagDelay;

		/* Security header has a constant length */
		secExtHdr->head.lenLsb = sizeof(secExtBHR_t);

		rsa_exp_len = (mpi_msb(&rsa.E) + 7) >> 3; /* exponent length in bytes */
		/* Full RSA public key lengh in DER encoding includes:
			- modulus (N) length
			- exponent (E) length
			- 4 bytes for each of the above components (type and length fields)
			Four bytes for the data block header are not included in this calculation
		*/
		rsa_key_len = rsa.len + rsa_exp_len + 8;

		/* First the RSA public key should be inserted into security header */
		/* Use DER encoding and long length field form (3 bytes) */
		secExtHdr->pubKey[0] = 0x30; /* Type field for entire block */
		secExtHdr->pubKey[1] = 0x82; /* long form, 2 bytes length field */
		secExtHdr->pubKey[2] = (rsa_key_len & 0xFF00) >> 8;
		secExtHdr->pubKey[3] = rsa_key_len & 0x00FF;
		secExtHdr->pubKey[4] = 0x02; /* modulus type (integer) */
		secExtHdr->pubKey[5] = 0x82; /* long form, 2 bytes length field */
		secExtHdr->pubKey[6] = (rsa.len & 0x0000FF00) >> 8;
		secExtHdr->pubKey[7] = rsa.len; /*we support up to 256 bytes key length */
		secExtHdr->pubKey[8 + rsa.len] = 0x02; /* exponent type (integer) */
		secExtHdr->pubKey[9 + rsa.len] = 0x82; /* long form, 2 bytes length field */
		secExtHdr->pubKey[10 + rsa.len] = (rsa_exp_len & 0x0000FF00) >> 8;
		secExtHdr->pubKey[11 + rsa.len] = rsa_exp_len;

		if ((0 != mpi_write_binary(&rsa.N, &secExtHdr->pubKey[8], rsa.len)) ||
			(0 != mpi_write_binary(&rsa.E, &secExtHdr->pubKey[12 + rsa.len], rsa_exp_len))) {
			fprintf(stderr, "Failed to write RSA key to security header\n");
			goto header_error;
		}

		sha2((unsigned char *)secExtHdr->pubKey, rsa_key_len + 4, rsa_digest, 0);
		if ((f_sha = fopen("./sha2_pub.txt", "w")) == NULL)
			fprintf(stderr, "Error opening SHA-256 digest file ./sha2_pub.txt\n");

		fprintf(f_sha, "SHA256 = ");

		for (k = 0; k < 32; k++)
			fprintf(f_sha, "%02X", rsa_digest[k]);

		fprintf(f_sha, "\n");
		fclose(f_sha);

		DB(stdout, "Box ID = 0x%08X, Flash ID = 0x%04X, JTAG %s\n",
				opt->boxId, opt->flashId, opt->jtagDelay == 0 ? "DISABLED" : "ENABLED");

		/* Encrypt boot image using AES-128-CBC if required */
		if (opt->flags & A_OPTION_MASK) {
			MV_U32		chsum32 = 0;

			secExtHdr->encrypt = 1;

			if ((opt->image_dest == 0xFFFFFFFF) &&
				(opt->image_type == IMG_FLASH)) {
				fprintf(stderr, "Wrong setup - Encrypted image with direct SPI boot!\n");
				fprintf(stderr, "The image should be downloaded into RAM for decryption.\n");
				fprintf(stderr, "Please define a valid destination address!\n");
				goto header_error;
			}
			/* 16 bytes at tail - IV, followed by 4 bytes of CHKSUM */
			chsum32 = checksum32((void*)(opt->image_buf), opt->image_sz - 20, 0);
			fprintf(stdout, "The image (plain img. CHKSUM = %08X) is encrypted using AES-128.\n", chsum32);

			if ((opt->image_sz - 4) % 16) {
				fprintf(stderr, "Total boot image size (%d) is not multiple of 16!\n",
						opt->image_sz);
				free(opt->image_buf);
				goto header_error;
			}

			aes_crypt_cbc(&aes, AES_ENCRYPT, opt->image_sz - 20, IV,
							opt->image_buf, opt->image_buf);
		} /* AES-128 encryption */

		/* Create RSA signature for the boot image file (checksum is not included) */
		error = create_rsa_signature(opt->image_buf, opt->image_sz - 4, secExtHdr->imgSign, "IMG SHA256: ");
		if (0 != error) {
			fprintf(stderr, "Failed to create boot image RSA signature, error %d\n", error);
			goto header_error;
		}
#if defined MV_MEMPOOL_STAT && defined NO_HEAP

		fprintf(stdout, "Boot Image signature creation\n");
		mpool_print_stat();
		mpool_reset_stat();
#endif
		error = verify_rsa_signature(opt->image_buf, opt->image_sz - 4, secExtHdr->imgSign);
		if (0 != error) {
			fprintf(stderr, "Failed to verify boot image RSA signature, error %d\n", error);
			goto header_error;
		}

#if defined MV_MEMPOOL_STAT && defined NO_HEAP
		fprintf(stdout, "Boot Image signature verification\n");
		mpool_print_stat();
		mpool_reset_stat();
#endif

		/* the RSA signature is now can be created for the entire headers block */
		error = create_rsa_signature(tmpHeader, header_size, rsa_signature, "HDR SHA256: ");
		if (0 != error) {
			fprintf(stderr, "Failed to create header RSA signature, error %d\n", error);
			goto header_error;
		}

#if defined MV_MEMPOOL_STAT && defined NO_HEAP
		fprintf(stdout, "Header signature creation\n");
		mpool_print_stat();
		mpool_reset_stat();
#endif

		/* Header signature MUST be checked when it's placeholder in the heqader is zeroed */
		error = verify_rsa_signature(tmpHeader, header_size, rsa_signature);
		if (0 != error) {
			fprintf(stderr, "Failed to verify header RSA signature, error %d\n", error);
			goto header_error;
		}

#if defined MV_MEMPOOL_STAT && defined NO_HEAP
		fprintf(stdout, "Header signature verification\n");
		mpool_print_stat();
		mpool_reset_stat();
#endif

		memcpy(secExtHdr->hdrSign, rsa_signature, rsa.len);

	} /* if (opt->flags & Z_OPTION_MASK)*/

	/* Now the headers block checksum should be calculated and wrote in the header */
	/* This checksum value should be valid for both secure and unsecure boot modes */
	/* This value will be checked first before RSA key and signature verification */
	hdr->checkSum = checksum8((void*)hdr, MAIN_HDR_GET_LEN(hdr), 0);

	/* Write to file(s) */
	if (opt->header_mode == HDR_IMG_TWO_FILES) {
		/* copy header to output image */
		size_written = write(f_header, tmpHeader, header_size);
		if (size_written != header_size) {
			fprintf(stderr,"Error writing %s file \n",opt->fname.hdr_out);
			goto header_error;
		}

		fprintf(stdout, "====>>>> %s was created\n", opt->fname_arr[HDR_FILE_INDX]);
	/* if (header_mode == HDR_IMG_TWO_FILES) */
	} else {
		/* copy header to output image */
		size_written = write(f_out, tmpHeader, header_size);
		if (size_written != header_size) {
			fprintf(stderr,"Error writing %s file \n",opt->fname.out);
			goto header_error;
		}

	} /* if (header_mode != HDR_IMG_TWO_FILES) */

	error = 0;

header_error:

	if (tmpHeader)
		free(tmpHeader);

	return error;

} /* end of build_headers() */

/*******************************************************************************
*    build_bootrom_img
*          create image in bootrom format and write it into output stream
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_bootrom_img (USER_OPTIONS *opt, char *buf_in)
{
	unsigned int 	CRC_32 = 0;
	int 		tmpSize = BOOTROM_SIZE - sizeof(CRC_32);
	char 		*tmpImg  = NULL ;
	int		size_written = 0;
	int		error = 1;

	tmpImg = malloc(tmpSize);
	if (tmpImg == NULL)
		return 1;

	/* PAD image with Zeros until BOOTROM_SIZE*/
	memcpy(tmpImg, buf_in, fs_stat.st_size);
	memset(tmpImg + fs_stat.st_size, 0, tmpSize - fs_stat.st_size);

	/* copy input image to output image */
	size_written = write(f_out, tmpImg, tmpSize);

	/* calculate checsum */
	CRC_32 = crc32(0, (u32*)tmpImg, tmpSize/4);
	tmpSize += sizeof(CRC_32) ;
	printf("Image CRC32 (size = %d) = 0x%08x\n", tmpSize, CRC_32);

	size_written += write(f_out, &CRC_32, sizeof(CRC_32));

	if (size_written == tmpSize)
		error = 0;

bootrom_img_error:

if (tmpImg)
	free(tmpImg);

	return error;
} /* end of build_bootrom_img() */

/*******************************************************************************
*    build_hex_img
*          create image in hex format and write it into output stream
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
 int build_hex_img (USER_OPTIONS *opt, char *buf_in)
{
	FILE    	*f_desc[2] = {NULL};
	char 		*tmpImg = NULL;
	int 		hex_len;
	int 		hex_unaligned_len = 0;
	unsigned char	*hex8 = NULL;
	unsigned char	tmp8;
	unsigned short	*hex16 = NULL;
	unsigned short	tmp16;
	unsigned int	*hex32 = NULL;
	unsigned int	tmp32;
	unsigned int	tmp32_low;
	int		size_written = 0;
	int 		alignment = 0;
	int		files_num;
	int		i;
	int		error = 1;

	/* Calculate aligned image size */
	hex_len = fs_stat.st_size;

	alignment = opt->hex_width >> 3;
	hex_unaligned_len = fs_stat.st_size & (alignment - 1);

	if (hex_unaligned_len) {
		hex_len -= hex_unaligned_len;
		hex_len += alignment;
	}

	/* Copy the input image to memory buffer */
	tmpImg = malloc(hex_len);
	if (tmpImg == NULL)
		goto hex_image_end;

	memset(tmpImg, 0, hex_len);
	memcpy(tmpImg, buf_in, fs_stat.st_size);

	if (opt->fname.hdr_out)
		files_num = 2;
	else
		files_num = 1;

	for (i = 0; i < files_num; i++) {
		f_desc[i] = fopen(opt->fname_arr[i + 1], "w");
		if (f_desc[i] == NULL)
			goto hex_image_end;
	}

	switch (opt->hex_width) {
		case 8:
			hex8 = (unsigned char*)tmpImg;

			for (i = 0; hex_len > 0; hex_len--) {
				fprintf(f_desc[i],"%02X\n",*hex8++);
				size_written += 1;
				i ^= files_num - 1;
			}
			break;

		case 16:
			hex16 = (unsigned short*)tmpImg;

			for (; hex_len > 0; hex_len -= 2) {
				fprintf(f_desc[0],"%04X\n",*hex16++);
				size_written += 2;
			}
			break;

		case 32:
			hex32 = (unsigned int*)tmpImg;

			for (; hex_len > 0; hex_len -= 4) {
				fprintf(f_desc[0],"%08X\n",*hex32++);
				size_written += 4;
			}
			break;

		case 64:
			hex32 = (unsigned int*)tmpImg;

			for (; hex_len > 0; hex_len -= 8) {
				fprintf(f_desc[0],"%08X%08X\n",*hex32++, *hex32++);
				size_written += 8;
			}
			break;

	} /* switch (opt->hex_width)*/

	error = 0;

hex_image_end:

	if (tmpImg)
		free(tmpImg);

	for (i = 0; i < files_num; i++) {
		if (f_desc[i] != NULL)
			fclose(f_desc[i]);
	}

	return error;
} /* end of build_hex_img() */

/*******************************************************************************
*    build_bin_img
*          create image in binary format and write it into output stream
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_bin_img (USER_OPTIONS	*opt, char *buf_in)
{
	FILE		*f_ds = NULL;
	FILE		*f_desc[4] = {NULL};
	char 		*tmpImg = NULL;
	int 		hex_len = 0;
	int		one_file_len = 0;
	int		size_written = 0;
	int 		alignment = 0;
	int		hex_unaligned_len = 0;
	unsigned char	*hex8 = NULL;
	unsigned char	tmp8;
	unsigned short	*hex16 = NULL;
	unsigned short	tmp16;
	unsigned long	*hex32 = NULL;
	unsigned long	tmp32;
	unsigned long	tmp32low;
	int		i;
	int		fidx;
	int		files_num = 1;
	int		error = 1;

	/* Calculate aligned image size */
	hex_len = fs_stat.st_size;

	alignment = opt->hex_width >> 3;
	hex_unaligned_len = fs_stat.st_size & (alignment - 1);

	if (hex_unaligned_len) {
		hex_len -= hex_unaligned_len;
		hex_len += alignment;
	}

	/* prepare output files */
	if (opt->fname.romd) /*16KB*/
		files_num = 4;
	else if (opt->fname.romc) /*12KB*/
		files_num = 3;
	else if (opt->fname.hdr_out)
		files_num = 2;

	one_file_len = hex_len / files_num;

	for (i = 0; i < files_num; i++) {
		f_desc[i] = fopen(opt->fname_arr[i + 1], "w");
		if (f_desc[i] == NULL)
			goto bin_image_end;
	}

	/* Copy the input image to memory buffer */
	tmpImg = malloc(hex_len);
	if (tmpImg == NULL)
		goto bin_image_end;

	memset(tmpImg, 0, (hex_len));
	memcpy(tmpImg, buf_in, fs_stat.st_size);

	/* Split output image buffer according to width and number of files */
	switch (opt->hex_width) {
		case 8:
			hex8 = (unsigned char*)tmpImg;
			if (files_num != 2) {
				fprintf(stderr, "Must supply two output file names for this width!\n");
				goto bin_image_end;
			}

			for (fidx = 1; (fidx >= 0) && (hex_len > 0);  fidx--) {
				f_ds = f_desc[1 - fidx];

				for (; hex_len > (fidx * one_file_len); hex_len--) {
					tmp8 = *hex8;

					for (i = 0; i < opt->hex_width ; i++) {
						fprintf(f_ds, "%d", ((tmp8 & 0x80) >> 7));
						tmp8 <<= 1;
					}
					fprintf(f_ds,"\n");

					hex8++;
					size_written += 1;
				}
			}
			break;

		case 16:
			hex16 = (unsigned short*)tmpImg;

			for (; hex_len > 0; hex_len -= 2) {
				tmp16 = *hex16;

				for (i = 0; i < opt->hex_width ; i++) {
					fprintf(f_desc[0], "%d", ((tmp16 & 0x8000) >> 15));
					tmp16 <<= 1;
				}
				fprintf(f_desc[0],"\n");

				hex16++;
				size_written += 2;
			}
			break;

		case 32:
			hex32 = (long*)tmpImg;

			for (fidx = files_num - 1; (fidx >= 0) && (hex_len > 0);  fidx--) {
				f_ds = f_desc[files_num - 1 - fidx];

				for (; hex_len > (fidx * one_file_len); hex_len -= 4) {
					tmp32 = *hex32;

					for (i = 0; i < opt->hex_width ; i++) {
						fprintf(f_ds, "%ld", ((tmp32 & 0x80000000) >> 31));
						tmp32 <<= 1;
					}
					fprintf(f_ds,"\n");
					hex32++;
					size_written += 4;
				}
			}
			break;

		case 64:
			hex32 = (long*)tmpImg;

			for (; hex_len > 0; hex_len -= 8) {
				tmp32low = *hex32++;
				tmp32    = *hex32++;

				for (i = 0; i < 32 ; i++) {
					fprintf(f_desc[0], "%ld", ((tmp32 & 0x80000000) >> 31));
					tmp32 <<= 1;
				}
				for (i = 0; i < 32 ; i++) {
					fprintf(f_desc[0], "%ld", ((tmp32low & 0x80000000) >> 31));
					tmp32low <<= 1;
				}
				fprintf(f_desc[0],"\n");
				size_written += 8;
			}
			break;
	} /* switch (opt->hex_width) */

	error = 0;

bin_image_end:


		if (tmpImg)
		free(tmpImg);

	for (i = 0; i < files_num; i++) {
		if (f_desc[i] != NULL)
			fclose(f_desc[i]);
	}

	return error;

} /*  end of build_bin_img() */

/*******************************************************************************
*    build_regular_img
*          create regular boot image and write it into output stream
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_regular_img (USER_OPTIONS	*opt, char *buf_in)
{
	int	size_written = 0;
	int	new_file_size = 0;
	MV_U32 	chsum32 = 0;

	new_file_size = opt->image_sz;

	if (0 != opt->img_gap) {/* cache line/NAND page/requested offset image alignment */
		MV_U8	*gap_buf;

		gap_buf = calloc(opt->img_gap, sizeof(MV_U8));
		if (gap_buf == NULL) {
			fprintf(stderr, "Failed to allocate memory for header to image gap!\n");
			return 1;
		}
		size_written += write(f_out, gap_buf, opt->img_gap);
		new_file_size += opt->img_gap ;
		free(gap_buf);
	}

	/* Calculate checksum and copy it to the image tail */
	chsum32 = checksum32((void*)opt->image_buf, opt->image_sz - 4, 0);
	memcpy(opt->image_buf + opt->image_sz - 4, &chsum32, 4);

	/* copy input image to output image */
	size_written += write(f_out, opt->image_buf, opt->image_sz);
	free(opt->image_buf);

	if (new_file_size != size_written) {
		fprintf(stderr, "Size mismatch between calculated %d "
				"and written %d amount!\n", new_file_size, size_written);
		return 1;
	}

	return 0;
} /* end of build_other_img() */

/*******************************************************************************
*    read_rsa_key
*          read RSA key from file
*    INPUT:
*          fname      private key file name
*          rsa        RSA context
*    OUTPUT:
*          rsa        RSA context
*    RETURN:
*          0 on success
*******************************************************************************/
int read_rsa_key(char *fname)
{
	FILE	*f_prkey;
	int	ret;

	if ((f_prkey = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "Error opening RSA private key file %s\n", fname);
		return -1;
	}

	rsa_init(&rsa, RSA_PKCS_V15, 0, NULL, NULL);

	ret  = mpi_read_file(&rsa.N , 16, f_prkey);
	ret += mpi_read_file(&rsa.E , 16, f_prkey);
	ret += mpi_read_file(&rsa.D , 16, f_prkey);
	ret += mpi_read_file(&rsa.P , 16, f_prkey);
	ret += mpi_read_file(&rsa.Q , 16, f_prkey);
	ret += mpi_read_file(&rsa.DP, 16, f_prkey);
	ret += mpi_read_file(&rsa.DQ, 16, f_prkey);
	ret += mpi_read_file(&rsa.QP, 16, f_prkey);

	fclose(f_prkey);

	return ret;
}

/*******************************************************************************
*    generate_rsa_key
*          generate RSA key pair and save new keys into text files
*    INPUT:
*          rsa        RSA context
*    OUTPUT:
*          rsa        RSA context
*    RETURN:
*          0 on success
*******************************************************************************/
int generate_rsa_key(void)
{
	int		ret;
	havege_state	hs;
	FILE		*fpub  = NULL;
	FILE		*fpriv = NULL;

	havege_init(&hs);

	rsa_init(&rsa, RSA_PKCS_V15, 0, havege_rand, &hs);

	if (rsa_gen_key(&rsa, RSA_KEY_SIZE, RSA_EXPONENT) != 0) {
		fprintf(stderr, "Failed to generate RSA key\n");
		return -1;
	}

	fpub = fopen("rsa_pub.txt", "w+");
	if (fpub == NULL) {
		fprintf(stderr, "Could not open rsa_pub.txt file for writing\n");
		return -1;
	}

	ret  = mpi_write_file("N = ", &rsa.N, 16, fpub);
	ret += mpi_write_file("E = ", &rsa.E, 16, fpub);
	fclose(fpub);
	if (ret != 0) {
		fprintf(stderr, "Failed to write into rsa_pub.txt file!\n");
		return -1;
	}

	fpriv = fopen("rsa_priv.txt", "w+");
	if (fpriv == NULL) {
		fprintf(stderr, "Could not open rsa_priv.txt file for writing\n");
		return -1;
	}

	ret  = mpi_write_file("N = " , &rsa.N , 16, fpriv);
	ret += mpi_write_file("E = " , &rsa.E , 16, fpriv);
	ret += mpi_write_file("D = " , &rsa.D , 16, fpriv);
	ret += mpi_write_file("P = " , &rsa.P , 16, fpriv);
	ret += mpi_write_file("Q = " , &rsa.Q , 16, fpriv);
	ret += mpi_write_file("DP = ", &rsa.DP, 16, fpriv);
	ret += mpi_write_file("DQ = ", &rsa.DQ, 16, fpriv);
	ret += mpi_write_file("QP = ", &rsa.QP, 16, fpriv);
	if (ret != 0)
		fprintf(stderr, "Failed to write into rsa_priv.txt file!\n");

	return ret;
}

/*******************************************************************************
*    read_aes_key
*          read AES key from file
*    INPUT:
*          fname      AES key file name
*          aes_key    AES key
*          init_vect  AES IV
*    OUTPUT:
*          aes_key    AES key
*          init_vect  AES IV
*    RETURN:
*          0 on success
*******************************************************************************/
int read_aes_key(char *fname, unsigned char *aes_key, unsigned char *init_vect)
{
	int		ret = -1;
	size_t		num;
	FILE		*fkey = NULL;
	char		line[120] = {0};
	unsigned char	*curr_val = NULL;
	char		*num_str = NULL;

	fkey = fopen(fname, "r");
	if (fkey == NULL) {
		fprintf(stderr, "Could not open AES key file %s\n", fname);
		return -1;
	}

	/* Read and parse every string in file */
	while (fgets(line, 120, fkey) != NULL) {

		if (strncmp("AES = ", line, 6) == 0) {
			num_str = line + 6;
			curr_val = aes_key;
		} else if (strncmp("IV = ", line, 5) == 0) {
			num_str = line + 5;
			curr_val = init_vect;
		} else
			continue;

		num = strlen(num_str);
		if ((num_str[num - 1] == '\n') || (num_str[num - 1] == '\r')) {
			num_str[num - 1] = '\0';
			num--;
		}

		/* The row should contain 2 characters per key digit */
		if (strlen(num_str) != (AES_KEY_SIZE >> 2)) {
			fprintf(stderr, "Badly formed string [%s] in file %s\n", line, fname);
			goto aes_read_end;
		}

		for (; num > 0; num -= 2) {
			if (sscanf(num_str, "%02hhx", curr_val) == EOF) {
				fprintf(stderr, "Conversion error starting %s\n", num_str);
				goto aes_read_end;
			}
			curr_val++;
			num_str += 2;
		}

	} /* for each line in file */

	ret = 0;

aes_read_end:

	fclose(fkey);
	return ret;
}

/*******************************************************************************
*    generate_rsa_key
*          generate AES key and IV and save them into text files
*    INPUT:
*          aes        AES context
*    OUTPUT:
*          aes        AES context
*    RETURN:
*          0 on success
*******************************************************************************/
int generate_aes_key(unsigned char *aes_key, unsigned char *init_vect)
{
	int		ret = 0, i, k;
	havege_state	hs;
	FILE		*fkey = NULL;
	unsigned char	*curr_val = aes_key;
	char		*rname[2] = {"AES", "IV"};

	havege_init(&hs);

	fkey = fopen("aes_key.txt", "w");
	if (fkey == NULL) {
		fprintf(stderr, "Could not open AES key file aes_key.txt\n");
		return -1;
	}

	/* Fill AES key and IV with random numbers generated by HAVEGE */
	for (i = 0; i < 2; i++) {

		fprintf(fkey, "%s = ", rname[i]);

		for (k = 0; k < 16; k++) {
			curr_val[k] = (unsigned char)havege_rand(&hs);
			fprintf(fkey, "%02X", curr_val[k]);
		}

		fprintf(fkey, "\n");

		curr_val = init_vect;
	}

	fclose(fkey);
	return ret;
}

/*******************************************************************************
*    process_image
*          handle input and output file options, read and verify RSA and AES keys.
*    INPUT:
*          opt        user options
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
******************************************************************************/
int process_image(USER_OPTIONS	*opt)
{
	int			i;
	int 		override[2];
	char 		*buf_in = NULL;
	int 		err = 1;

	/* check if the output image exist */
	printf(" ");
	for (i = IMG_FILE_INDX; i <= HDR_FILE_INDX; i++) {
		if (opt->fname_arr[i]) {
			override[i] = 0;

			if (0 == stat(opt->fname_arr[i], &fs_stat)) {
				char c;
				/* ask for overwrite permissions */
				fprintf(stderr,"File '%s' already exist! Overwrite it (Y/n)?",
						opt->fname_arr[i]);
				c = getc(stdin);
				if ((c == 'N')||(c == 'n')) {
					printf("exit.. nothing done. \n");
					exit(0);
				} else if ((c == 'Y')||(c == 'y')) {
					/* additional read is needed for Enter key */
				c = getc(stdin);
				}
				override[i] = 1;
			}
		}
	}

	/* open input image file and check if it's size is OK */
	if (opt->header_mode != HDR_ONLY) {
		f_in = open(opt->fname.in, O_RDONLY|O_BINARY);
		if (f_in == -1) {
			fprintf(stderr,"File '%s' not found \n", opt->fname.in);
			goto end;
		}
		/* get the size of the input image */
		if (0 != fstat(f_in, &fs_stat)) {
			fprintf(stderr,"fstat failed for file: '%s' err=%d\n", opt->fname.in, err);
			goto end;
		}
		/*Check the source image size for limited output storage (bootrom) */
		if (opt->image_type == IMG_BOOTROM) {
			int max_img_size = BOOTROM_SIZE - sizeof(u32);

			if (opt->flags & Z_OPTION_MASK) {
				fprintf(stderr,"RSA signature is not supported for this image type\n");
				goto end;
			}

			if (fs_stat.st_size > max_img_size) {
				fprintf(stderr, "ERROR : source image is bigger "
						"than %d bytes \n", max_img_size);
				goto end;
			}
		}
		/* map the input image */
		buf_in = mmap(0, fs_stat.st_size, PROT_READ, MAP_SHARED, f_in, 0);
		if (!buf_in) {
			fprintf(stderr,"Error mapping %s file \n", opt->fname.in);
			goto end;
		}
	}

	/* open the output image file */
	if (override[IMG_FILE_INDX] == 0)
		f_out = open(opt->fname.out, O_RDWR|O_TRUNC|O_CREAT|O_BINARY,0666);
	else
		f_out = open(opt->fname.out, O_RDWR|O_BINARY);

	if (f_out == -1) {
		fprintf(stderr,"Error opening %s file \n", opt->fname.out);
		goto end;
    }

	/* open the output header file */
	if (opt->header_mode == HDR_IMG_TWO_FILES) {
		if (override[HDR_FILE_INDX] == 0)
			f_header = open(opt->fname.hdr_out, O_RDWR|O_TRUNC|O_CREAT|O_BINARY, 0666);
		else
			f_header = open(opt->fname.hdr_out, O_RDWR|O_BINARY);

		if (f_header == -1) {
			fprintf(stderr,"Error opening %s file \n",opt->fname.hdr_out);
			goto end;
		}
	}

	/* secure boot support - read RSA private key */
	if (opt->flags & Z_OPTION_MASK) {

		if (strncmp(opt->fname_prkey, "@@", 2) != 0) { /* private key file supplied */
			if (read_rsa_key(opt->fname_prkey) != 0) {
				fprintf(stderr, "Cannot read RSA private key file %s\n\n", opt->fname_prkey);
				goto end;
			}
		} else { /* new key pair is required */
			DB("Generating new RSA key pair...");
			if (generate_rsa_key() != 0) {
				fprintf(stderr, "Cannot generate RSA key pair\n\n");
				goto end;
			}
			DB("OK\n");
		}

		rsa.len = (mpi_msb(&rsa.N) + 7) >> 3; /* key lenght in bytes */

		if (RSA_MAX_KEY_LEN_BYTES < rsa.len) {
			fprintf(stderr, "Wrong RSA key length - %d bytes!"
					" Supported RSA keys up to %d bytes\n",
					rsa.len, RSA_MAX_KEY_LEN_BYTES);
			goto end;
		} else
			fprintf(stdout, "The RSA private key is %d bit long\n", rsa.len * 8);

		/* Use AES-128 encryption - read the key and generate IV vector */
		if (opt->flags & A_OPTION_MASK) {
			unsigned char	aes_key[16];

			if (strncmp(opt->fname_aeskey, "@@", 2) != 0) { /* AES key file supplied */
				if (read_aes_key(opt->fname_aeskey, aes_key, IV) != 0) {
					fprintf(stderr, "Cannot read AES key file\n\n");
					goto end;
				}
			} else { /* new key is required */
				DB("Generating new AES key ...");
				if (generate_aes_key(aes_key, IV) != 0) {
					fprintf(stderr, "Cannot generate AES key\n\n");
					goto end;
				}
				DB("OK\n");
			}

			aes_setkey_enc(&aes, aes_key, 128);

		} /* AES-128 encryption */

	} /* secure boot options */

	/* Image Header(s)  */
	if (opt->header_mode != IMG_ONLY) {
		if (0 != build_headers(opt, buf_in))
			goto end;
	}

	/* Output Image  */
	if (opt->header_mode != HDR_ONLY) {
		if (opt->image_type == IMG_BOOTROM)
			err = build_bootrom_img(opt, buf_in);
		else if (opt->image_type == IMG_HEX)
			err = build_hex_img(opt, buf_in);
		else if (opt->image_type == IMG_BIN)
			err = build_bin_img(opt, buf_in);
		else
			err = build_regular_img(opt, buf_in);

		if (err != 0) {
			fprintf(stderr, "Error writing %s file \n", opt->fname.out);
			goto end;
		}

		fprintf(stdout, "====>>>> %s was created\n", opt->fname_arr[IMG_FILE_INDX]);

	} /* if (opt->header_mode != HDR_ONLY) */

end:
	/* close handles */
	if (f_out != -1)
		close(f_out);

	if (f_header != -1)
		close(f_header);

    if (buf_in)
		munmap((void*)buf_in, fs_stat.st_size);

	if (f_in != -1)
		close(f_in);

	return err;

} /* end of process_image() */

/*******************************************************************************
*    print_usage
*          print command switches and their description
*    INPUT:
*          none
*    OUTPUT:
*          none
*    RETURN:
*          none
*******************************************************************************/
void print_usage(void)
{
	printf("Marvell doimage Tool version %s\n", VERSION_NUMBER);
	printf("Supported SoC devices: \n\t%s\n", PRODUCT_SUPPORT);
	printf("\n");
	printf("Usage: \n");
	printf("doimage <mandatory_opt> [other_options] <image_in> <image_out> [header_out]\n\n");

	printf("<mandatory_opt> - can be one or more of the following:\n\n");

	printf("-T image_type:   sata\\uart\\flash\\bootrom\\nand\\hex\\bin\\pex\n");
	printf("-D image_dest:   image destination in dram (in hex)\n");
	printf("-E image_exec:   execution address in dram (in hex)\n");
	printf("                 if image_type is 'flash' and image_dest is 0xffffffff\n");
	printf("                 then execution address on the flash\n");
	printf("-S image_source: if image_type is sata then the starting sector of\n");
	printf("                 the source image on the disk\n");
	printf("                 if image_type is flash\\nand then the starting offset of\n");
	printf("                 the source image at the flash - optional for flash\\nand\n");
	printf("-W hex_width :   HEX file width, can be 8,16,32,64 \n");
	printf("-M twsi_file:    ascii file name that contains the I2C init regs set by h/w.\n");
	printf("                 this is used in i2c boot only\n");

	printf("\nThe following options are mandatory for NAND image type:\n\n");

	printf("-L nand_blk_size:NAND block size in KBytes (decimal int in range 64-16320)\n");
	printf("                 This parameter is ignored for flashes with  512B pages\n");
	printf("                 Such small page flashes always use 16K block sizes\n");
	printf("-N nand_cell_typ:NAND cell technology type (char: M for MLC, S for SLC)\n");
	printf("-P nand_pg_size: NAND page size: (decimal 512, 2048, 4096 or 8192)\n");

	printf("\nSecure boot mode options - all options are mandatory once secure mode is selected by Z switch:\n");

	printf("-Z [prv_key_file]: Create image with RSA signature for secure boot mode\n");
	printf("                   If the private key file name is missing, a new key pair will be generated\n");
	printf("                   and saved in files named rsa_prv.key and rsa_pub.key\n");
	printf("                   A new file named sha2_pub.txt will be generated for a public key\n");
	printf("-J jtag_delay:   Enable JTAG and delay boot execution by \"N\" ms\n");
	printf("-B hex_box_id:   Box ID (hex) - from 0 to 0xffffffff\n");
	printf("-F hex_flash_id: Flash ID (hex) - from 0 to 0xffff \n\n");

	printf("\n<other_options> - optional and can be one or more of the following:\n\n");

	printf("-A [aes_key_file]: Valid in secure mode only. Encrypt the boot image using AES-128 key\n");
	printf("                   If the aes key file name is missing, a new AES-128 key will be generated\n");
	printf("                   and saved in file named aes_key.txt suitable for eFuse storage\n");
	printf("-G exec_file:    ascii file name that contains binary routine (ARM 5TE THUMB)\n");
	printf("                 to run before the bootloader image execution.\n");
	printf("                 The routine must contain an appropriate code for saving \n");
	printf("                 all registers at the routine start and restore them \n");
	printf("                 before return from the routine \n");
	printf("-R dram_file:    ascii file name that contains the list of dram regs\n");
	printf("-C hdrs_file:    ascii file name that defines BIN/REG headers order and their sources\n");
	printf("-X pre_padding_size (hex)\n");
	printf("-Y post_padding_size (hex)\n");
	printf("-H header_mode: Header mode, can be:\n");
	printf("   -H 1 :will create one file (image_out) for header and image\n");
	printf("   -H 2 :will create two files, (image_out) for image , (header_out) for header\n");
	printf("   -H 3 :will create one file (image_out) for header only \n");
	printf("   -H 4 :will create one file (image_out) for image only \n");

	printf("\nCommand examples: \n\n");

	printf("doimage -T hex -W width image_in image_out\n");
	printf("doimage -T bootrom image_in image_out\n");
	printf("doimage -T resume image_in image_out\n");
	printf("doimage -T sata -S sector -D image_dest -E image_exec\n");
	printf("         [other_options] image_in image_out header_out\n\n");
	printf("doimage -T flash -D image_dest -E image_exec [-S address]\n");
	printf("         [other_options] image_in image_out\n\n");
	printf("doimage -T pex -D image_dest -E image_exec \n");
	printf("         [other_options] image_in image_out\n\n");
	printf("doimage -T nand -D image_dest -E image_exec [-S address] -P page_size\n");
	printf("         -L 2 -N S [other_options] image_in image_out\n\n");
	printf("doimage -T uart -D image_dest -E image_exec\n");
	printf("         [other_options] image_in image_out\n\n");
	printf("doimage -T pex -D image_dest -E image_exec \n");
	printf("         [other_options] image_in image_out\n\n");
	printf("\n\n\n");

} /* end of print_usage() */


/*******************************************************************************
*    checksum8
*          calculate 8-bit checksum of memory buffer
*    INPUT:
*          start        buffer start
*          len          buffer length
*          csum         initial checksum value
*    OUTPUT:
*          none
*    RETURN:
*          8-bit buffer checksum
*******************************************************************************/
MV_U8 checksum8(void* start, MV_U32 len, MV_U8 csum)
{
	register MV_U8 sum = csum;
	volatile MV_U8* startp = (volatile MV_U8*)start;

	do {
		sum += *(MV_U8*)startp;
		startp++;

    } while(--len);

    return (sum);

} /* end of checksum8 */

/*******************************************************************************
*    checksum32
*          calculate 32-bit checksum of memory buffer
*    INPUT:
*          start        buffer start
*          len          buffer length
*          csum         initial checksum value
*    OUTPUT:
*          none
*    RETURN:
*          32-bit buffer checksum
*******************************************************************************/
MV_U32 checksum32(void* start, MV_U32 len, MV_U32 csum)
{
	register MV_U32 sum = csum;
	volatile MV_U32* startp = (volatile MV_U32*)start;

	do {
		sum += *(MV_U32*)startp;
		startp++;
		len -= 4;

    } while(len);

    return (sum);

} /* *end of checksum32() */

/*******************************************************************************
*    make_crc_table
*          init CRC table
*    INPUT:
*          crc_table   CRC table location
*    OUTPUT:
*          crc_table   CRC table location
*    RETURN:
*          none
*******************************************************************************/
void make_crc_table(MV_U32 *crc_table)
{
	MV_U32    c;
	MV_32     n, k;
	MV_U32    poly;

	/* terms of polynomial defining this crc (except x^32): */
	static const MV_U8 p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

	/* make exclusive-or pattern from polynomial (0xedb88320L) */
	poly = 0L;
	for (n = 0; n < sizeof(p)/sizeof(MV_U8); n++)
		poly |= 1L << (31 - p[n]);

	for (n = 0; n < 256; n++) {
		c = (MV_U32)n;
		for (k = 0; k < 8; k++)
			c = c & 1 ? poly ^ (c >> 1) : c >> 1;
		crc_table[n] = c;
	}

} /* end of make_crc_table */

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/*******************************************************************************
*    crc32
*          calculate CRC32 on memory buffer
*    INPUT:
*          crc       initial CRC value
*          buf       memory buffer
*          len       buffer length
*    OUTPUT:
*          none
*    RETURN:
*          CRC32 of the memory buffer
*******************************************************************************/
MV_U32 crc32(MV_U32 crc, volatile MV_U32 *buf, MV_U32 len)
{
	MV_U32 crc_table[256];

	/* Create the CRC table */
	make_crc_table(crc_table);

	crc = crc ^ 0xffffffffL;
	while (len >= 8) {
		DO8(buf);
		len -= 8;
	}

	if (len) {
		do {
		DO1(buf);
	} while (--len);
	}

	return crc ^ 0xffffffffL;

} /* end of crc32() */

/*******************************************************************************
*    select_image
*          select image options by the image name
*    INPUT:
*          img_name       image name
*    OUTPUT:
*          opt            image options
*    RETURN:
*          0 on success, 1 if image name is invalid
*******************************************************************************/
int select_image (char *img_name, USER_OPTIONS *opt)
{
	int	i;
	static IMG_MAP img_map[] = {
	{ IMG_SATA,	"sata",		D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK },
	{ IMG_UART, 	"uart",		D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK },
	{ IMG_FLASH, 	"flash",	D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK },
	{ IMG_MMC,	"mmc",		D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK },
	{ IMG_BOOTROM, 	"bootrom",	T_OPTION_MASK },
	{ IMG_NAND,	"nand",		D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK|
					L_OPTION_MASK|N_OPTION_MASK|P_OPTION_MASK },
	{ IMG_HEX, 	"hex",		T_OPTION_MASK|W_OPTION_MASK},
	{ IMG_BIN, 	"bin",		T_OPTION_MASK|W_OPTION_MASK},
	{ IMG_PEX, 	"pex",		D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK },
	{ IMG_I2C,	"i2c",		D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK|M_OPTION_MASK },
	};

	for (i = 0; i < ARRAY_SIZE(img_map); i++) {
		if (strcmp(img_name, img_map[i].img_name) == 0) {
			opt->image_type = img_map[i].img_type;
			opt->req_flags  = img_map[i].img_opt;
			return 0;
		}
	}

	return 1;

} /* *end of select_image() */


/*******************************************************************************
*    main
*******************************************************************************/
int main (int argc, char** argv)
{
	USER_OPTIONS	options;
	int 		optch; /* command-line option char */
	static char	optstring[] = "T:D:E:X:Y:S:P:W:H:R:M:Z:J:B:F:A:G:L:N:C:";
	int		i, k;

	if (argc < 2) goto parse_error;

	memset(&options, 0, sizeof(USER_OPTIONS));
	options.header_mode = HDR_IMG_ONE_FILE;

	fprintf(stdout, "\n");

	while ((optch = getopt(argc, argv, optstring)) != -1) {
		char	*endptr = NULL;

		switch (optch) {
		case 'T': /* image type */
			if ((select_image(optarg, &options) != 0) || (options.flags & T_OPTION_MASK))
				goto parse_error;
			options.flags |= T_OPTION_MASK;
			break;

		case 'D': /* image destination  */
			options.image_dest = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & D_OPTION_MASK))
				goto parse_error;
			options.flags |= D_OPTION_MASK;
			DB("Image destination address %#x\n", options.image_dest);
			break;

		case 'E': /* image execution  */
			options.image_exec = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & E_OPTION_MASK))
				goto parse_error;
			options.flags |= E_OPTION_MASK;
			DB("Image execution address %#x\n", options.image_exec);
			break;

		case 'X': /* Pre - Padding */
			options.prepadding_size = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & X_OPTION_MASK))
				goto parse_error;
			options.pre_padding = 1;
			options.flags |= X_OPTION_MASK;
			DB("Pre-pad image by %#x bytes\n", options.prepadding_size);
			break;

		case 'Y': /* Post - Padding */
			options.postpadding_size = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & Y_OPTION_MASK))
				goto parse_error;
			options.post_padding = 1;
			options.flags |= Y_OPTION_MASK;
			DB("Post-pad image by %#x bytes\n", options.postpadding_size);
			break;

		case 'S': /* starting sector */
			options.image_source = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & S_OPTION_MASK))
				goto parse_error;
			options.flags |= S_OPTION_MASK;
			DB("Image start sector (image source) %#x\n", options.image_source);
			break;

		case 'P': /* NAND Page Size */
			options.nandPageSize = strtoul (optarg, &endptr, 10);
			if (*endptr || (options.flags & P_OPTION_MASK))
				goto parse_error;
			options.flags |= P_OPTION_MASK;
			DB("NAND page size %d bytes\n", options.nandPageSize);
			break;

		case 'C': /* headers definition filename */
			options.fname_list = optarg;
			if (options.flags & C_OPTION_MASK)
				goto parse_error;
			options.flags |= C_OPTION_MASK;
			DB("Headers definition file name %s\n", options.fname_list);
			break;

		case 'W': /* HEX file width */
			options.hex_width = strtoul(optarg, &endptr, 10);
			if (*endptr || (options.flags & W_OPTION_MASK))
				goto parse_error;
			options.flags |= W_OPTION_MASK;
			DB("HEX file width %d bytes\n", options.hex_width);
			break;

		case 'H': /* Header file mode */
			options.header_mode = strtoul(optarg, &endptr, 10);
			if (*endptr || (options.flags & H_OPTION_MASK))
				goto parse_error;
			options.flags |= H_OPTION_MASK;
			DB("Header file mode is %d\n", options.header_mode);
			break;

		case 'R': /* dram file */
			options.fname_dram = optarg;
			if (options.flags & R_OPTION_MASK)
				goto parse_error;
			options.flags |= R_OPTION_MASK;
			DB("Registers header file name %s\n", options.fname_dram);
			break;

		case 'M': /* TWSI file */
			options.fname_twsi = optarg;
			if (options.flags & M_OPTION_MASK)
				goto parse_error;
			options.flags |= M_OPTION_MASK;
			DB("TWSI header file name %s\n", options.fname_twsi);
			break;

		case 'G': /* binary file */
			options.fname_bin = optarg;
			if (options.flags & G_OPTION_MASK)
				goto parse_error;
			options.flags |= G_OPTION_MASK;
			DB("Binary header file name %s\n", options.fname_bin);
			break;

		case 'Z': /* secure boot - private key */
			options.fname_prkey = optarg;
			if (options.flags & Z_OPTION_MASK)
				goto parse_error;
			options.flags |= Z_OPTION_MASK;
			DB("RSA private Key file name %s\n", options.fname_prkey);
			break;

		case 'J': /* JTAG Enabled */
			options.jtagDelay = strtoul(optarg, &endptr, 10);
			if (*endptr || (options.flags & J_OPTION_MASK))
				goto parse_error;
			options.flags |= J_OPTION_MASK;
			DB("JTAG delay %d ms\n", options.jtagDelay);
			break;

		case 'B': /* Box ID */
			options.boxId = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & B_OPTION_MASK))
				goto parse_error;
			options.flags |= B_OPTION_MASK;
			DB("Box ID %#x\n", options.boxId);
			break;

		case 'F': /* Flash ID */
			options.flashId = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & F_OPTION_MASK))
				goto parse_error;
			options.flags |= F_OPTION_MASK;
			DB("Flash ID %#x\n", options.flashId);
			break;

		case 'A': /* secure boot - encrypt with AES-128 key */
			options.fname_aeskey = optarg;
			if (options.flags & A_OPTION_MASK)
				goto parse_error;
			options.flags |= A_OPTION_MASK;
			DB("AES file name %s\n", options.fname_aeskey);
			break;

		case 'L': /* NAND block size */
			options.nandBlkSize = strtoul(optarg, &endptr, 10) / 64;
			if (*endptr || (options.flags & L_OPTION_MASK))
				goto parse_error;
			options.flags |= L_OPTION_MASK;
			DB("NAND block size %d\n", options.nandBlkSize);
			break;

		case 'N': /* NAND cell technology */
			options.nandCellTech = optarg[0];
			if (options.flags & N_OPTION_MASK)
				goto parse_error;
			options.flags |= N_OPTION_MASK;
			DB("NAND cell technology %c\n", options.nandCellTech);
			break;

		default:
			goto parse_error;
		}
	} /* parse command-line options */

	/* assign file names */
	for (i = 0; (optind < argc) && (i < ARRAY_SIZE(options.fname_arr)); ++optind, i++) {
		options.fname_arr[i] = argv[optind];
		DB("File @ array index %d is %s (option index is %d)\n", i, argv[optind], optind);
		/* verify that all file names are different */
		for (k = 0; k < i; k++) {
			if (0 == strcmp(options.fname_arr[i], options.fname_arr[k])) {
				fprintf(stderr,"\nError: Input and output images can't be the same\n");
				exit(1);
			}
		}
	}

	if (!(options.flags & T_OPTION_MASK))
		goto parse_error;

	/* verify HEX/BIN file width selection to be valid */
	if ((options.flags & W_OPTION_MASK) &&
	    (options.hex_width != 8)  && (options.hex_width != 16) &&
	    (options.hex_width != 32) && (options.hex_width != 64))
		goto parse_error;
	/* BootROM test images, no header is needed */
	if ((options.image_type == IMG_BOOTROM) ||
	    (options.image_type == IMG_HEX)     ||
	    (options.image_type == IMG_BIN))
		options.header_mode = IMG_ONLY;

	if (options.header_mode == IMG_ONLY) {
		/* remove unneeded options */
		options.req_flags &= ~(D_OPTION_MASK|E_OPTION_MASK|S_OPTION_MASK|
			R_OPTION_MASK|P_OPTION_MASK|L_OPTION_MASK|N_OPTION_MASK);
	}

	if (options.req_flags != (options.flags & options.req_flags))
		goto parse_error;

	if ((options.flags & F_OPTION_MASK) &&
	    (options.flashId > 0xFFFF)) {
		fprintf(stderr,"Error: Flash ID is too long!\n\n\n\n\n");
		goto parse_error;
	}

	if ((options.flags & L_OPTION_MASK) &&
	    ((options.nandBlkSize > 255) ||
	    ((options.nandBlkSize == 0) && (options.nandPageSize != 512)))) {
		fprintf(stderr,"Error: wrong NAND block size %d!\n\n\n\n\n", 64*options.nandBlkSize);
		goto parse_error;
	}

	if ((options.flags & N_OPTION_MASK) &&
	    (options.nandCellTech != 'S') && (options.nandCellTech != 'M') &&
	    (options.nandCellTech != 's') && (options.nandCellTech != 'm')) {
		fprintf(stderr,"Error: Wrong NAND cell technology type!\n\n\n\n\n");
		goto parse_error;
	}

	return process_image(&options);

parse_error:

	print_usage();
	exit(1);

} /* end of main() */

