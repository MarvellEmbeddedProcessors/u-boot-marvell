/*
* ***************************************************************************
* Copyright (C) 2016 Marvell International Ltd.
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

#ifndef _A8K_EFUSE_H_
#define _A8K_EFUSE_H_

#include <asm/arch-mvebu/efuse.h>

enum a8k_efuse_type {
	HD_AP	= 0,
	LD_AP	= 1,
	LD_CP0	= 2,
	LD_CP1	= 3,

	INVALID_TYPE
};

/* RD_* and *_NA devices are not (and cannot be) supported and listed here for information only */
enum a8k_ap_boot_dev {
	RE_CP0		= 0,	/* CP device and BootROM enabled */
	RE_NA		= 1,	/* N/A with BootROM enabled - NOT SUPPORTED */
	RE_AP_SPI0	= 2,	/* AP CPI CS0 and BootROM enabled */
	RE_AP_SD	= 3,	/* AP SD and BootROM enabled */
	RD_CP0		= 4,	/* CP device and BootROM disabled - NOT SUPPORTED */
	RD_NA		= 5,	/* N/A with BootROM disabled -NOT SUPPORTED */
	RD_AP_SPI0	= 6,	/* AP CPI CS0 and BootROM disabled - NOT SUPPORTED*/
	RE_AP_EMMC	= 7,	/* AP EMMC and BootROM enabled */

	MAX_AP_BOOT_DEV
};

#define MAX_CP_BOOT_DEV		64

/*	SAR			  0     1    2       3    4   5   6    7   */
#define A8K_AP_BOOT_DEV_NAMES	{"CP0", "", "SPI0", "SD", "", "", "", "EMMC"}

struct a8k_efuse_info {
	enum efuse_id		id;		/* efuse ID */
	enum a8k_efuse_type	typ;		/* efuse type (LD/HD) */
	uint32_t		row;		/* efuse row or start row for multi-row values */
	uint32_t		numbits;	/* number of bits in the field */
	uint32_t		bitoffs;	/* efuse bit offset within the row for less than row fields */
};

#define A8K_EFUSE_INFO	{ \
		{ EFUSE_ID_BOOT_DEVICE,       LD_AP,  0,   3,    3  }, \
		{ EFUSE_ID_KAK_DIGEST,        HD_AP,  25,  256,  0  }, \
		{ EFUSE_ID_CSK_INDEX,         HD_AP,  37,  32,   0  }, \
		{ EFUSE_ID_AES_KEY,           HD_AP,  32,  256,  0  }, \
		{ EFUSE_ID_FLASH_ID,          HD_AP,  30,  32,   0  }, \
		{ EFUSE_ID_BOX_ID,            HD_AP,  31,  32,   0  }, \
		{ EFUSE_ID_JTAG_PERM_EN,      LD_AP,  0,   1,    1  }, \
		{ EFUSE_ID_TRUSTED_EN,        LD_AP,  0,   1,    0  }, \
		{ EFUSE_ID_CFG_BASE_ADDR,     LD_AP,  0,   12,   6  }, \
		{ EFUSE_ID_CFG_BASE_USE,      LD_AP,  0,   1,    18 }, \
		{ EFUSE_ID_ROM_BASE_ADDR,     LD_AP,  0,   12,   19 }, \
		{ EFUSE_ID_ROM_BASE_USE,      LD_AP,  0,   1,    31 }, \
		{ EFUSE_ID_CP0_BOOT_DEVICE,   LD_CP0, 0,   6,    2 }, \
	}

#endif /* _A8K_EFUSE_H_ */
