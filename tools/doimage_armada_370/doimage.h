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
#ifndef _INC_DOIMAGE_H
#define _INC_DOIMAGE_H

#include <sys/types.h>

/* use the same version as in "bootrom.inc" file */
#define VERSION_NUMBER	"2.00"
#define PRODUCT_SUPPORT "Marvell Armada series"

#define T_OPTION_MASK	0x1	/* image type */
#define D_OPTION_MASK	0x2	/* image destination */
#define E_OPTION_MASK	0x4	/* image execution address */
#define S_OPTION_MASK	0x8	/* starting sector */
#define R_OPTION_MASK	0x10    /* DRAM file */
#define C_OPTION_MASK	0x20	/* headers definition file */
#define P_OPTION_MASK	0x40	/* NAND Page size */
#define M_OPTION_MASK	0x80	/* TWSI serial init file */
#define W_OPTION_MASK	0x100	/* HEX file width */
#define H_OPTION_MASK	0x200	/* Header mode */
#define X_OPTION_MASK	0x400	/* Pre padding */
#define Y_OPTION_MASK	0x800	/* Post padding */
#define J_OPTION_MASK	0x1000	/* JTAG Enabled */
#define B_OPTION_MASK	0x2000	/* Box ID */
#define Z_OPTION_MASK	0x4000	/* secure boot mode - private key */
#define F_OPTION_MASK	0x8000	/* Flash ID */
#define A_OPTION_MASK	0x10000	/* AES encryption */
#define G_OPTION_MASK	0x20000	/* binary file */
#define L_OPTION_MASK	0x80000	/* NAND block size (in 64K chunks) */
#define N_OPTION_MASK	0x100000/* NAND cell technology MLC/SLC */

#ifndef O_BINARY                /* should be defined on __WIN32__ */
#define O_BINARY        0
#endif

#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))

typedef enum
{
	IMG_SATA,
	IMG_UART,
	IMG_FLASH,
	IMG_BOOTROM,
	IMG_NAND,
	IMG_HEX,
	IMG_BIN,
	IMG_PEX,
	IMG_I2C

} IMG_TYPE;

#define REGULAR_IMAGE(opt)	\
	(((opt)->image_type != IMG_BOOTROM) && \
	 ((opt)->image_type != IMG_HEX) && \
	 ((opt)->image_type != IMG_BIN))

typedef struct
{
	IMG_TYPE	img_type;	/* image type */
	char		*img_name;	/* image name string */
	unsigned int	img_opt;	/* mandatory options for this image type */

} IMG_MAP;


typedef enum
{
	IMG_FILE_INDX = 1,
	HDR_FILE_INDX = 2

} FILE_IDX;

typedef enum
{
	HDR_IMG_ONE_FILE  	= 1,	/* Create one file with header and image */
	HDR_IMG_TWO_FILES 	= 2,	/* Create seperate header and image files */
	HDR_ONLY 		= 3,	/* Create only header */
	IMG_ONLY 		= 4,	/* Create only image */

} HEADER_MODE;

typedef struct
{
	IMG_TYPE	image_type;
	char		*fname_dram; /* DRAM init file for "register" header */
	char		*fname_twsi; /* TWSI serial init file */
	char		*fname_bin;  /* binary code file for "binary" header */
	char		*fname_prkey;/* RSA Private key file */
	char		*fname_aeskey;/* AES-128 key file */
	char		*fname_list; /* headers definition file */
	u32		flags;       /* user-defined flags */
	u32		req_flags;   /* mandatory flags */
	u32		image_source;/* starting sector */
	u32		image_dest;  /* image destination  */
	u32		image_exec;  /* image execution  */
	unsigned int 	hex_width;   /* HEX file width */
	unsigned int 	header_mode; /* Header file mode */
	int 		pre_padding;
	int 		post_padding;
	int		prepadding_size;
	int		postpadding_size;
	unsigned int	bytesToAlign;
	unsigned int	nandPageSize;
	unsigned int	nandBlkSize;
	char		nandCellTech;
	u32		boxId;
	u32		flashId;
	u32		jtagDelay;
	char		*image_buf;   /* image buffer for image pre-load */
	u32		image_sz;   /* total size of pre-loaded image buffer including paddings */
	u32		img_gap;   /* gap between header and image start point */
	union
	{
		char	*fname_arr[5];
		struct
		{
			char	*in;
			char	*out;
			char	*hdr_out;
			char	*romc;
			char	*romd;
		} fname;
	};
} USER_OPTIONS;

/* Function declaration */
void print_usage(void);

/* 32 bit checksum */
//MV_U32	checksum32	(MV_U32 start, MV_U32 len, MV_U32 csum);
MV_U32	checksum32	(void* start, MV_U32 len, MV_U32 csum);
//MV_U8	checksum8	(MV_U32 start, MV_U32 len, MV_U8 csum);
MV_U8	checksum8	(void* start, MV_U32 len, MV_U8 csum);

MV_U32	crc32		(MV_U32 crc, volatile MV_U32 *buf, MV_U32 len);


#endif /* _INC_DOIMAGE_H */
