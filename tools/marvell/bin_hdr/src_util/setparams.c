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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../inc/soc_spec.h"
#include "../inc/bootstrap_def.h"

#define  BUFSIZE	80

void printusage(char *name)
{
	printf("Utility for BIN header preparation (parameters and linker script):\n");
	printf("%s [options] <parameters output file name> <linker script output file name>\n", name);
	printf("Options:\n");
	printf("-S		Secure mode (secure header is included in boot image)\n");
	printf("-R <reg file>	REG header is present and produced from \"reg file\" content\n");
	printf("-P <par file>	Text file with BIN header parameters. The text file contains\n");
	printf("-O <Hex value>	Calculated offset of the bin header\n");
	printf("		one parametes per line, up to 256 lines total. Each parameter is\n");
	printf("		a hexadecimal number in range of 0 to 2^32.\n");

}

int main(int argc, char **argv)
{

	int		err = -1;
	int		secure = 0;
	char		buf[BUFSIZE];
	unsigned long	args[256] = {0};
	unsigned long	offset, argsnum = 0, reghdr_argsnum = 0;
	unsigned long	memalign;
	FILE		*f_outparm = NULL, *f_outlink = NULL;
	FILE		*f_reg = NULL, *f_param = NULL;
	int 		optch; /* command-line option char */
	static char	optstring[] = "SR:P:B:";
	struct stat st;

	offset  = RAM_TOP + HDR_BLK_OFFSET;

	while ((optch = getopt(argc, argv, optstring)) != -1) {
		switch (optch) {
		case 'S': /* secure mode */
			secure = 1;
			break;

		case 'R': /* Register config file */
			f_reg = fopen(optarg, "r");
			if (f_reg == NULL) {
				printf("Error opening register config. file %s!\n", optarg);
				goto parse_error;
			}
			break;

		case 'P': /* Parameters file */
			f_param = fopen(optarg, "r");
			if (f_param == NULL) {
				printf("Error opening input parameters file %s!\n", optarg);
				goto parse_error;
			}
			break;

		case 'B': /* Parameters file */
			stat(optarg, &st);
			offset += st.st_size + 8; /* bytes (4 - header type and length + 4- Next header and 3 reserved bytes) */
			break;


		default:
			goto parse_error;
		}
	}

	f_outparm = fopen(argv[optind], "w");
	if (f_outparm == NULL) {
		printf("Error opening output parameters file - %s!\n", argv[optind]);
		goto parse_error;
	}

	optind++;
	f_outlink = fopen(argv[optind], "w");
	if (f_outlink == NULL) {
		printf("Error opening output linker script file -- %s!\n", argv[optind]);
		goto parse_error;
	}

	/* Count number of REG header parametres */
	if (f_reg != NULL) {
		while (1) {
			int addr, val;

			if (fscanf(f_reg, "%x %x\n", (unsigned int *)&addr, (unsigned int *)&val) == EOF)
				break;
			else if ((addr == 0x0) && (val == 0x0))
				break;
			else
				reghdr_argsnum++;
		}
	}

	/* Count number of BIN header parametres */
	if (f_param != NULL) {
		while (fgets(buf, BUFSIZE-2, f_param) != NULL) {
			if (strncmp(buf, "0x", 2) != 0)
				continue;

			args[argsnum] = strtoul(buf, 0, 16);
			argsnum++;
		}
	}
	printf("Supplied %ld parameters in TXT file\n", argsnum);

	/* main and secure headers */
	offset += sizeof(BHR_t) + secure * sizeof(secExtBHR_t);
	/* register header */
	if (reghdr_argsnum != 0) {
		offset += EXT_HDR_BASE_SIZE + reghdr_argsnum * 8;
		printf("The REG configuration consists %ld (ADDR,VAL) pairs\n", reghdr_argsnum);
	}
	/* binary header - one additional word is for number of arguments */
	offset += sizeof(headExtBHR_t) + 4 + argsnum * 4;
	/* printf("SUSUSUSU %x\n\n\n",offset); */

	/* The start address of executable code should be aligned to 128bit boundary using dumb arguments
	   Othervise the linker will silently align it and the execution will start at the wrong place */
	memalign = offset & 0xF;
	if (memalign != 0) {
		argsnum += (0x10 - memalign) / 4;
		offset = (offset & 0xFFFFFFF0UL) + 0x10;
		printf("\n****WARNING: Adding %ld empty parameters to the BIN header\n\n",
		       (0x10 - memalign) / 4);
	}

	/* Number of arguments field always presents in BIN header */
	fwrite(&argsnum, 4, 1, f_outparm);
	if (argsnum != 0)
		fwrite(args, 4, argsnum, f_outparm);

	fprintf(f_outlink, "OUTPUT_FORMAT(\"elf32-littlearm\", \"elf32-bigarm\", \"elf32-littlearm\")\n");
	fprintf(f_outlink, "OUTPUT_ARCH(arm)\n");
	fprintf(f_outlink, "SECTIONS\n{\n");
	fprintf(f_outlink, "\t. = %#08lX;\n", offset);
	fprintf(f_outlink, "\t.text : { *(.text) }\n");
	fprintf(f_outlink, "\t.data : { *(.data) }\n}\n");

	err = 0;

parse_error:
	if (f_reg)
		fclose(f_reg);
	if (f_param)
		fclose(f_param);
	if (f_outparm)
		fclose(f_outparm);
	if (f_outlink)
		fclose(f_outlink);
	if (err != 0)
		printusage(argv[0]);

	return err;
}
