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

#ifndef _A3700_EFUSE_H_
#define _A3700_EFUSE_H_

#include <asm/arch-mvebu/efuse.h>
#include <asm/arch-armada3700/mbox.h>


enum a3700_boot_dev {
	AUTO		= 0,
	SPINOR		= 1,
	EMMCNORM	= 2,
	EMMCALT		= 3,
	SATA		= 4,
	SPINAND		= 5,
	UART		= 6,
	INVALID		= 7,
	VECTOR_DIV	= 8,
	VECTOR_XTAL	= 9,

	MAX_BOOT_DEVS
};

#define A3700_BOOT_DEV_NAMES	{"AUTO", "SPINOR", "EMMCNORM", "EMMCALT", "SATA", "SPINAND", "UART", "", "", ""}

struct a3700_efuse_info {
	enum efuse_id		id;		/* efuse ID */
	enum mbox_opsize	mbopsz;		/* mailbox operation size bit/byte/word, etc. */
	uint32_t		row;		/* efuse row or start row for multi-row values */
	uint32_t		numops;		/* number of write operations required for setting the field */
	int32_t			bitoffs[4];	/* efuse bit offset within the row for bit size efuses */
};

#define A3700_EFUSE_INFO	{ \
		{ EFUSE_ID_BOOT_DEVICE,       MB_OPSZ_BIT,   1,   4, { 48, 52, 56, 60 } }, \
		{ EFUSE_ID_KAK_DIGEST,        MB_OPSZ_256B,  8,   1, { 0, 0, 0, 0 } }, \
		{ EFUSE_ID_CSK_INDEX,         MB_OPSZ_DWORD, 3,   1, { 0, 0, 0, 0 } }, \
		{ EFUSE_ID_AES_KEY,           MB_OPSZ_256B,  26,  1, { 0, 0, 0, 0 } }, \
		{ EFUSE_ID_ENCRYPTION_EN,     MB_OPSZ_BIT,   0,   2, { 56, 60, 0, 0 } }, \
		{ EFUSE_ID_JTAG_DIGECT,       MB_OPSZ_256B,  16,  1, { 0, 0, 0, 0 } }, \
		{ EFUSE_ID_SEC_JTAG_DIS,      MB_OPSZ_BIT,   0,   1, { 24, 0, 0, 0 } }, \
		{ EFUSE_ID_SEC_JTAG_PERM_DIS, MB_OPSZ_BIT,   0,   1, { 28, 0, 0, 0 } }, \
		{ EFUSE_ID_AP_JTAG_DIS,       MB_OPSZ_BIT,   0,   1, { 16, 0, 0, 0 } }, \
		{ EFUSE_ID_AP_JTAG_PERM_DIS,  MB_OPSZ_BIT,   0,   1, { 20, 0, 0, 0 } }, \
		{ EFUSE_ID_SPI_NAND_CFG,      MB_OPSZ_DWORD, 6,   1, { 0, 0, 0, 0 } }, \
		{ EFUSE_ID_PIN,               MB_OPSZ_DWORD, 4,   1, { 0, 0, 0, 0 } }, \
		{ EFUSE_ID_TOKEN,             MB_OPSZ_DWORD, 5,   1, { 0, 0, 0, 0 } }, \
		{ EFUSE_ID_SPI_CS,            MB_OPSZ_BIT,   1,   2, { 40, 44, 0, 0 } }, \
		{ EFUSE_ID_EMMC_CLOCK,        MB_OPSZ_BIT,   1,   2, { 32, 36, 0, 0 } }, \
		{ EFUSE_ID_OPERATION_MODE,    MB_OPSZ_BIT,   1,   2, { 0, 4, 0, 0 } }, \
		{ EFUSE_ID_UART_DIS,          MB_OPSZ_BIT,   0,   1, { 32, 0, 0, 0 } }, \
		{ EFUSE_ID_UART_PERM_DIS,     MB_OPSZ_BIT,   0,   1, { 36, 0, 0, 0 } }, \
		{ EFUSE_ID_ESC_SEQ_DIS,       MB_OPSZ_BIT,   1,   1, { 20, 0, 0, 0 } }, \
		{ EFUSE_ID_GPIO_TOGGLE_DIS,   MB_OPSZ_BIT,   1,   1, { 16, 0, 0, 0 } }, \
		{ EFUSE_ID_LONG_KEY_EN,       MB_OPSZ_BIT,   1,   1, { 12, 0, 0, 0 } } \
	}

#endif /* _A3700_EFUSE_H_ */
