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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

//#define _HOST_COMPILER
#include "bootstrap_def.h"
#include "soc_spec.h"

#include "polarssl/sha2.h"
#include "polarssl/rsa.h"
#include "polarssl/aes.h"

#define VERSION_NUMBER	"1.0"

/* Security context */
typedef struct secCtx_t
{
	rsa_context		rsa;
	aes_context		aes;

} secCtx_t;


/* Globals */
char	*buf_in = NULL;


/*
 * 8 bit checksum
 */
MV_U8 checksum8(void *start, MV_U32 len, MV_U8 csum)
{
	register MV_U8	sum = csum;
	MV_U8 *startp = (MV_U8 *)start;

	do {
		sum += *startp;
		startp++;

	} while(--len);

	return (sum);
}

/*
 * 32 bit checksum
 */
MV_U32 checksum32(void *start, MV_U32 len, MV_U32 csum)
{
	register MV_U32	sum = csum;
	MV_U32	*startp = (MV_U32 *)start;

	do {
		sum += *startp;
		startp++;
		len -= 4;

	} while(len);

	return (sum);
}

/*******************************************************************************
*    print_usage
*******************************************************************************/
void print_usage(char *myname)
{
	printf("Marvell boot image parser version %s\n", VERSION_NUMBER);
	printf("Usage: \n");
	printf("%s [options] <file_name>\n\n", myname);
	printf("Options: \n");
	printf("-t      Test the header and image secure signatures\n");
	printf("\n");
}

/*******************************************************************************
*    parse_main_header
*******************************************************************************/
int parse_main_header(pBHR_t	pHdr)
{
	MV_U8	chksum = 0;
	MV_U8	tmp8;

	fprintf(stdout, "-----------------------------------------------------\n");
	fprintf(stdout, "                     MAIN HEADER\n");
	fprintf(stdout, "-----------------------------------------------------\n");
	fprintf(stdout, "[00:00]Image type:                ");
	switch (pHdr->blockID) {
		case IBR_HDR_I2C_ID:
			fprintf(stdout, "I2C\n");
			break;
		case IBR_HDR_SPI_ID:
			fprintf(stdout, "SPI/NOR\n");
			break;
		case IBR_HDR_NAND_ID:
			fprintf(stdout, "NAND\n");
			break;
		case IBR_HDR_SATA_ID:
			fprintf(stdout, "SATA\n");
			break;
		case IBR_HDR_PEX_ID:
			fprintf(stdout, "PEX\n");
			break;
		case IBR_HDR_UART_ID:
			fprintf(stdout, "UART\n");
			break;
		case IBR_HDR_MMC_ID:
			fprintf(stdout, "MMC\n");
			break;
		default:
			fprintf(stdout, "UNKNOWN\n");
			return -1;
	}

	fprintf(stdout, "[01:01]Reserved:                  0x%02X\n",
		pHdr->rsvd1);
	fprintf(stdout, "[03:02]NAND page size:            %d (0x%04X)\n",
		pHdr->nandPageSize, pHdr->nandPageSize);
	fprintf(stdout, "[07:04]Boot image size:           %d (0x%08X)\n",
		pHdr->blockSize, pHdr->blockSize);
	fprintf(stdout, "[08:08]Header version:            %d (0x%02X)\n",
		pHdr->version, pHdr->version);
	fprintf(stdout, "[11:09]Headers block size:        %d (0x%06X)\n",
		MAIN_HDR_GET_LEN(pHdr), MAIN_HDR_GET_LEN(pHdr));
	if (pHdr->blockID == IBR_HDR_SATA_ID) {
		fprintf(stdout, "[15:12]Source address (LBA):      0x%08X\n",
			pHdr->sourceAddr, pHdr->sourceAddr);
	} else {
		fprintf(stdout, "[15:12]Source address:            0x%08X\n",
			pHdr->sourceAddr, pHdr->sourceAddr);
	}
	fprintf(stdout, "[19:16]Destination address:       0x%08X\n",
		pHdr->destinationAddr);
	fprintf(stdout, "[23:20]Execution address:         0x%08X\n",
		pHdr->executionAddr);
	fprintf(stdout, "[24:24]Reserved:                  0x%02X\n",
		pHdr->rsvd3);
	fprintf(stdout, "[25:25]NAND Block size:           %d (%dKB)\n",
		pHdr->nandBlockSize, pHdr->nandBlockSize*64);
	fprintf(stdout, "[26:26]NAND Technology:           %d (%s)\n",
		pHdr->nandTechnology, pHdr->blockID == IBR_HDR_NAND_ID ?
			(pHdr->nandTechnology == 1 ? "MLC" : "SLC") : "N/A");
	fprintf(stdout, "[27:27]Reserved:                  0x%02X\n",
		pHdr->rsvd4);
	fprintf(stdout, "[29:28]Reserved:                  0x%04X\n",
		pHdr->rsvd2);
	fprintf(stdout, "[30:30]Num of ext. headers:       %d (0x%02X)\n",
		pHdr->ext, pHdr->ext);
	fprintf(stdout, "[31:31]Header block checksum:     0x%02X",
		pHdr->checkSum);

	tmp8 = pHdr->checkSum;
	pHdr->checkSum = 0;
	chksum = checksum8((void *)pHdr, MAIN_HDR_GET_LEN(pHdr), 0);
	fprintf(stdout, " (%s)\n", tmp8 == chksum ? "GOOD" : "BAD");

	return 0;
}

/*******************************************************************************
*    parse_sec_header
*******************************************************************************/
int parse_sec_header(pSecExtBHR_T	pHdr)
{
	fprintf(stdout, "-----------------------------------------------------\n");
	fprintf(stdout, "             SECURITY HEADER @ %p\n", (void*)((char*)pHdr - buf_in));
	fprintf(stdout, "-----------------------------------------------------\n");
	fprintf(stdout, "[0000:0000]Header type:           0x%02X\n",
		pHdr->head.type);
	fprintf(stdout, "[0003:0001]Header length:         %d (0x%06X)\n",
		EXT_HDR_GET_LEN(&pHdr->head), EXT_HDR_GET_LEN(&pHdr->head));
	fprintf(stdout, "[0004:0004]Image is encrypted:    %s\n",
		pHdr->encrypt == 1 ? "YES" : "NOT");
	fprintf(stdout, "[0005:0005]Reserved:              0x%02X\n",
		pHdr->rsrvd0);
	fprintf(stdout, "[0007:0006]Reserved:              0x%04X\n",
		pHdr->rsrvd1);
	fprintf(stdout, "[0531:0008]Public Key:            %02X %02X %02X %02X ...\n",
		pHdr->pubKey[0], pHdr->pubKey[1], pHdr->pubKey[2], pHdr->pubKey[3]);
	fprintf(stdout, "[0532:0532]JTAG enabled:          %s [%d]\n",
		pHdr->jtagEn != 0 ? "YES" : "NOT", pHdr->jtagEn);
	fprintf(stdout, "[0533:0533]Reserved:              0x%02X\n",
		pHdr->rsrvd2);
	fprintf(stdout, "[0535:0534]Reserved:              0x%04X\n",
		pHdr->rsrvd3);
	fprintf(stdout, "[0539:0536]Box ID:                0x%08X\n",
		pHdr->boxId);
	fprintf(stdout, "[0541:0540]Flash ID:              0x%04X\n",
		pHdr->flashId);
	fprintf(stdout, "[0543:0542]Reserved:              0x%04X\n",
		pHdr->rsrvd4);
	fprintf(stdout, "[0799:0544]Header signature:      %02X %02X %02X %02X ...\n",
		pHdr->hdrSign[0], pHdr->hdrSign[1], pHdr->hdrSign[2], pHdr->hdrSign[3]);
	fprintf(stdout, "[1055:0800]Image signature:       %02X %02X %02X %02X ...\n",
		pHdr->imgSign[0], pHdr->imgSign[1], pHdr->imgSign[2], pHdr->imgSign[3]);
	fprintf(stdout, "[1057:1056]More headers follow:   %s\n",
		pHdr->tail.nextHdr != 0 ? "YES" : "NOT");
	fprintf(stdout, "[1058:1058]Reserved:              0x%02X\n",
		pHdr->tail.delay);
	fprintf(stdout, "[1060:1059]Reserved:              0x%04X\n",
		pHdr->tail.rsvd2);

	return 0;
}

/*******************************************************************************
*    parse_bin_header
*******************************************************************************/
int parse_bin_header(headExtBHR_t	*pHdr)
{
	tailExtBHR_t	*pTail = (tailExtBHR_t*)((MV_U8*)pHdr +
			EXT_HDR_GET_LEN(pHdr) - sizeof(tailExtBHR_t));
	MV_U8		*buf = (MV_U8*)(pHdr+1);
	int		code_bytes = (EXT_HDR_GET_LEN(pHdr) - EXT_HDR_BASE_SIZE - (*buf + 1)*4);

	fprintf(stdout, "-----------------------------------------------------\n");
	fprintf(stdout, "             BINARY HEADER @ %p\n", (void*)((char*)pHdr - buf_in));
	fprintf(stdout, "-----------------------------------------------------\n");

	fprintf(stdout, "[0000:0000]Header type:           0x%02X\n",
		pHdr->type);
	fprintf(stdout, "[0003:0001]Header length:         %d (0x%06X)\n",
		EXT_HDR_GET_LEN(pHdr), EXT_HDR_GET_LEN(pHdr));
	fprintf(stdout, "[0004:0004]Number of parameters:  %d (0x%06X)\n",
		*buf, *buf);
	fprintf(stdout, "[0005:0005]Reserved:              0x%02X\n",
		*(buf + 1), *(buf + 1));
	fprintf(stdout, "[0007:0005]Reserved:              0x%04X\n",
		*(MV_U16*)(buf + 2), *(MV_U16*)(buf + 2));
	if (*buf != 0)
		fprintf(stdout, "[%04d:0008]Parameters\n", (7 + (*buf)*4));
	fprintf(stdout, "[%04d:%04d]ARM Code link offset:  %p\n",
		(8 + (*buf)*4), (code_bytes + 8 + (*buf)*4), (void*)((char*)pHdr - buf_in) + 8 + (*buf)*4);
	fprintf(stdout, "[%04d:%04d]More headers follow:   %s\n",
		EXT_HDR_GET_LEN(pHdr) - 4, EXT_HDR_GET_LEN(pHdr) - 4, pTail->nextHdr != 0 ? "YES" : "NOT");
	fprintf(stdout, "[%04d:%04d]Reserved:              0x%02X\n",
		EXT_HDR_GET_LEN(pHdr) - 3, EXT_HDR_GET_LEN(pHdr) - 3, pTail->delay);
	fprintf(stdout, "[%04d:%04d]Reserved:              0x%04X\n",
		EXT_HDR_GET_LEN(pHdr) - 2, EXT_HDR_GET_LEN(pHdr) - 1, pTail->rsvd2);

	return 0;
}

/*******************************************************************************
*    parse_reg_header
*******************************************************************************/
int parse_reg_header(headExtBHR_t	*pHdr)
{
	tailExtBHR_t	*pTail = (tailExtBHR_t*)((MV_U8*)pHdr +
				EXT_HDR_GET_LEN(pHdr) - sizeof(tailExtBHR_t));
	int		num_elem = (EXT_HDR_GET_LEN(pHdr) - EXT_HDR_BASE_SIZE)/8;

	fprintf(stdout, "-----------------------------------------------------\n");
	fprintf(stdout, "             REGISTER HEADER @ %p\n", (void*)((char*)pHdr - buf_in));
	fprintf(stdout, "-----------------------------------------------------\n");
	fprintf(stdout, "[0000:0000]Header type:           0x%02X\n",
		pHdr->type);
	fprintf(stdout, "[0003:0001]Header length:         %d (0x%06X)\n",
		EXT_HDR_GET_LEN(pHdr), EXT_HDR_GET_LEN(pHdr));
	fprintf(stdout, "[%04d:0004]Num of ADDR-VAL pairs: %d (0x%02X)\n",
		(num_elem*8 + 3), num_elem, num_elem);
	num_elem = num_elem * 8 + 4;
	fprintf(stdout, "[%04d:%04d]More headers follow:   %s\n",
		num_elem, num_elem, pTail->nextHdr != 0 ? "YES" : "NOT");
	fprintf(stdout, "[%04d:%04d]Delay(mS):             0x%02X\n",
		num_elem + 1, num_elem + 1, pTail->delay);
	fprintf(stdout, "[%04d:%04d]Reserved:              0x%04X\n",
		num_elem + 3, num_elem + 2, pTail->rsvd2);

	return 0;
}

/*******************************************************************************
* verify_rsa_signature - check the image/header RSA signature
*******************************************************************************/
int verify_rsa_signature (secCtx_t *pSecCtx, MV_U8 *start, MV_U32 len, MV_U8 *sign)
{
	unsigned char   sha256[32];

	memset(sha256, 0, 32 * sizeof(unsigned char));

	sha2(start, len, sha256, 0);

	return (rsa_pkcs1_verify(&pSecCtx->rsa, RSA_PUBLIC, RSA_SHA256, 32, sha256, sign));
}

/*******************************************************************************
* verify_sec_header - check the secure header fields
*******************************************************************************/
int verify_sec_header (BHR_t *pHdr)
{
	MV_U32		rsaKeyLen = 0;
	int		rsaModLen, rsaExpLen;
	secExtBHR_t	*pSecHdr = (secExtBHR_t *)(pHdr + 1);
	secCtx_t 	secCtx;
	unsigned char	buf256[256];
	unsigned char	buf32[32];
	int		i;

	if (pSecHdr->head.type != EXT_HDR_TYP_SECURITY)
		return 0;

	memset(buf256, 0, 256 * sizeof(unsigned char));
	memset(buf32,  0, 32  * sizeof(unsigned char));

	/* RSA public key should be in DER format with 2-bytes length field */
	if ((pSecHdr->pubKey[0] != 0x30) ||
	(pSecHdr->pubKey[1] != 0x82)) {
		fprintf(stderr,"\nError: Bad RSA key!\n");
		return -1;
	}

	/* DER format fields verification */
	rsaModLen = (pSecHdr->pubKey[6] << 8) + pSecHdr->pubKey[7];
	if (rsaModLen > RSA_MAX_KEY_LEN_BYTES) {
		fprintf(stderr,"\nError: Bad RSA key Modulo length (%d)!\n", rsaModLen);
		return -1;
	}

	rsaExpLen = (pSecHdr->pubKey[10 + rsaModLen] << 8) +
		pSecHdr->pubKey[11 + rsaModLen];
	if (rsaExpLen > RSA_MAX_KEY_LEN_BYTES) {
		fprintf(stderr,"\nError: Bad RSA key Exponent length (%d)!\n", rsaExpLen);
		return -1;
	}

	/* Load the RSA public key */
	rsa_init(&secCtx.rsa, RSA_PKCS_V15, 0, NULL, NULL);
	if ((0 != mpi_read_binary(&secCtx.rsa.N, &pSecHdr->pubKey[8], rsaModLen)) ||
	(0 != mpi_read_binary(&secCtx.rsa.E, &pSecHdr->pubKey[12 + rsaModLen], rsaExpLen))) {
		fprintf(stderr,"\nError: RSA Library error!\n");
		return -1;
	}

	secCtx.rsa.len = (mpi_msb(&secCtx.rsa.N) + 7) >> 3; /* key lenght in bytes */

	/* Key length should include 4 bytes of the data block header */
	rsaKeyLen = (pSecHdr->pubKey[2] << 8) + pSecHdr->pubKey[3] + 4;
	sha2((unsigned char *)pSecHdr->pubKey, rsaKeyLen, buf32, 0);
	fprintf(stdout,"RSA key SHA256 digest for eFuse:\n  BE FULL: ");
	for (i = 0; i < 32; i++) {
		fprintf(stdout,"%02x ", buf32[i]);
		if (i == 15)
			fprintf(stdout,"\n           ");
	}
	fprintf(stdout,"\n  LE REGS: ");
	for (i = 0; i < 8; i++) {
		fprintf(stdout,"[%d]%08x ", i, *(MV_U32*)(buf32 + i*4));
		if (i == 3)
			fprintf(stdout,"\n           ");
	}
	fprintf(stdout,"\n");

	memset(buf32,  0, 32  * sizeof(unsigned char));

	/* Header signature should be cleared in the header before verification */
	memcpy(buf256, pSecHdr->hdrSign, 256);
	memset(pSecHdr->hdrSign, 0, 256);

	/* Verify the headers block signature */
	fprintf(stdout,"Header RSA signature verification - ");
	if (verify_rsa_signature(&secCtx, (MV_U8 *)pHdr, MAIN_HDR_GET_LEN(pHdr), buf256) != 0) {
		fprintf(stderr,"FAILED\n");
		return -1;
	}
	fprintf(stdout,"PASSED\n");

	fprintf(stdout,"Image  RSA signature verification - ");
	if (verify_rsa_signature(&secCtx, buf_in + pHdr->sourceAddr,
		pHdr->blockSize - 4, pSecHdr->imgSign) != 0) {
		fprintf(stderr,"FAILED\n");
		return -1;
	}
	fprintf(stdout,"PASSED\n");

	return 0;
}

/*******************************************************************************
*    main
*******************************************************************************/
int main (int argc, char** argv)
{
	int		i, rsa_verify = 0;
	char		*fname;
	int		fin;
	struct stat	fs_stat;
	int		error = 1;

	int		optch; /* command-line option char */
	static char	optstring[] = "t";
	pBHR_t		pHdr = NULL;
	MV_U32		chksum, chksum2;

	while ((optch = getopt(argc, argv, optstring)) != -1) {
		char   *endptr = NULL;

		switch (optch) {
			case 't': /* test */
				rsa_verify = 1;
				break;

			default:
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	} /* parse command-line options */

	if ((fname = argv[optind]) == NULL) {
		fprintf(stderr,"\nError: Missing filename\n");
		goto end;
	}

	if ((fin = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr,"Error opening %s file\n", fname);
		goto end;
	}

	if (0 != fstat(fin, &fs_stat)) {
		fprintf(stderr,"Failed to get %s status\n", fname);
		goto end;
	}

	buf_in = (char*)malloc(fs_stat.st_size);
	if (buf_in == NULL) {
		fprintf(stderr,"Error allocting buffer for %s file\n", fname);
		goto end;
	}

	if (read(fin, buf_in, fs_stat.st_size) != fs_stat.st_size) {
		fprintf(stderr,"Error reading %s file\n", fname);
		goto end;
	}

	fprintf(stdout,"#####################################################\n");
	fprintf(stdout,"Marvell Boot Image checker version %s \n", VERSION_NUMBER);
	fprintf(stdout,"-----------------------------------------------------\n");
	fprintf(stdout,"Image file:\n%s\nFile size = %d bytes\n", fname, (int)fs_stat.st_size);

	pHdr = (pBHR_t)buf_in;
	if (parse_main_header(pHdr) < 0)
		goto end;

	if (pHdr->ext != 0) {
	/* Header extentions exist */
		headExtBHR_t	*pHead = (headExtBHR_t*)(pHdr + 1);
		tailExtBHR_t	*pTail;

		for(;pHead != 0;) {
			pTail = (tailExtBHR_t *)((MV_U8 *)pHead +
					EXT_HDR_GET_LEN(pHead) -
					sizeof(tailExtBHR_t));

			switch (pHead->type) {
				case EXT_HDR_TYP_SECURITY:
					if (parse_sec_header((pSecExtBHR_T)pHead) < 0)
						goto end;
					break;
				case EXT_HDR_TYP_BINARY:
					if (parse_bin_header(pHead) < 0)
						goto end;
					break;
				case EXT_HDR_TYP_REGISTER:
					if (parse_reg_header(pHead) < 0)
						goto end;
					break;
				default:
					fprintf(stderr,"Unknown header type 0x%x\n", pHead->type);
					goto end;
			}

			if (pTail->nextHdr != 0)
				pHead = (headExtBHR_t*)(pTail + 1);
			else
				pHead = NULL;
		}
	/* Header extentions exist */
	}

	fprintf(stdout, "-----------------------------------------------------\n");
	fprintf(stdout, "             BOOT IMAGE @ %p\n",
	pHdr->blockID == IBR_HDR_SATA_ID ?
		(void*)((char*)pHdr - buf_in + pHdr->sourceAddr * 512) :
		(void*)((char*)pHdr - buf_in + pHdr->sourceAddr));
	fprintf(stdout, "-----------------------------------------------------\n");

	chksum = *(MV_U32*)(buf_in + fs_stat.st_size - 4);
	if (pHdr->blockID == IBR_HDR_SATA_ID)
		chksum2 = checksum32((void *)(buf_in + pHdr->sourceAddr * 512),  pHdr->blockSize - 4, 0);
	else
		chksum2 = checksum32((void *)(buf_in + pHdr->sourceAddr),  pHdr->blockSize - 4, 0);

	fprintf(stdout, "Binary image checksum = 0x%08X (%s)\n", chksum,
	       chksum == chksum2 ? "GOOD" : "BAD");

	if (rsa_verify != 0)
		error = verify_sec_header(pHdr);
	else
		error = 0;

end:
	if (fin)
		close(fin);
	if (buf_in)
		free(buf_in);

	if (error == 0) {
		fprintf(stdout,"************ T E S T   S U C C E E D E D ************\n");
		exit(EXIT_SUCCESS);
	} else {
		fprintf(stdout,"*************** T E S T   F A I L E D ***************\n");
		exit(EXIT_FAILURE);
	}
}
