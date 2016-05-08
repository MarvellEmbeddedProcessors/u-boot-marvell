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
/*#define DEBUG*/
#include <common.h>
#include <command.h>
#include <vsprintf.h>
#include <malloc.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch-mvebu/mvebu.h>
#include "a8k_efuse.h"

#define NUM_OF_CPS			1
#define EFUSE_AP_HD_BASE_ADDR		(MVEBU_REGS_BASE + 0x006F9000)
#define EFUSE_AP_LD_BASE_ADDR		(MVEBU_REGS_BASE + 0x006F8F00)
#define EFUSE_CPN_LD_BASE_ADDR(n)	(MVEBU_REGS_BASE + ((n) + 1) * 0x02000000 + 0x400F00) /* n >= 0 */
#define EFUSE_AP_HD_ROW_ADDR(row)	(EFUSE_AP_HD_BASE_ADDR + (row) * 16)

#define EFUSE_AP_SRV_CTRL_REG		(MVEBU_REGS_BASE + 0x006F8008)
#define EFUSE_CPN_SRV_CTRL_REG(n)	(MVEBU_REGS_BASE + ((n) + 1) * 0x02000000 + 0x400008) /* n >= 0 */

/* Control fields are the same on AP and CPn */
#define EFUSE_SRV_CTRL_PROG_EN_OFFS	31
#define EFUSE_SRV_CTRL_PROG_EN_MASK	(1 << EFUSE_SRV_CTRL_PROG_EN_OFFS)
#define EFUSE_SRV_CTRL_LD_SELECT_OFFS	6
#define EFUSE_SRV_CTRL_LD_SEL_USER_MASK	(1 << EFUSE_SRV_CTRL_LD_SELECT_OFFS)
#define EFUSE_SRV_CTRL_LD_SEC_EN_OFFS	7
#define EFUSE_SRV_CTRL_LD_SEC_EN_MASK	(1 << EFUSE_SRV_CTRL_LD_SEC_EN_OFFS)

static struct a8k_efuse_info efuse_info[EFUSE_ID_MAX] = A8K_EFUSE_INFO;
/* Boot device names for AP */
static char *ap_boot_dev_names[] = A8K_AP_BOOT_DEV_NAMES;

/************************************
*	Local functions
*************************************/

/******************************************************************************
 *	efuse_read_ld
 *****************************************************************************/
static inline void efuse_read_ld(enum a8k_efuse_type typ,
				 uint32_t efuse_row_id,
				 uint32_t *value)
{
	uintptr_t	row_addr = efuse_row_id * 4;
	uintptr_t	efuse_ctrl_reg;
	uint8_t		cp_id;
	uint32_t	reg32;

	/* In LD eFuses "row ID" used for a word number inside the single row */
	if (typ > LD_AP) {
		cp_id = typ - LD_CP0;
		if (cp_id > (NUM_OF_CPS - 1)) {
			printf("%s: Unsupported CP ID %d\n", __func__, cp_id);
			return;
		}
		efuse_ctrl_reg = EFUSE_CPN_SRV_CTRL_REG(cp_id);
		row_addr += EFUSE_CPN_LD_BASE_ADDR(cp_id);
	} else {
		efuse_ctrl_reg = EFUSE_AP_SRV_CTRL_REG;
		row_addr += EFUSE_AP_LD_BASE_ADDR;
	}

	/* Select user LD - we read only user LD eFuse */
	reg32 = readl(efuse_ctrl_reg);
	reg32 |= EFUSE_SRV_CTRL_LD_SEL_USER_MASK;
	writel(reg32, efuse_ctrl_reg);

	*value = readl(row_addr);
}

/******************************************************************************
 *	efuse_read_hd_32bit
 *****************************************************************************/
static inline void efuse_read_hd_32bit(uint32_t efuse_row_id, uint32_t *value)
{
	uintptr_t	row_addr = EFUSE_AP_HD_ROW_ADDR(efuse_row_id);
	/* Each HD eFUSE row is 65 bits, thus the offset between rows is 16 bytes
	 * (due to line alignment)
	 * We have to use 32b access due to internal registers fabric limitation
	 */
	*value = readl(row_addr);
}

/******************************************************************************
 *	efuse_read_hd_256bit
 *****************************************************************************/
static inline void efuse_read_hd_256bit(uint32_t efuse_row_id, uint8_t *value)
{
	uint8_t		row_idx, byte;
	uint32_t	val_idx;
	uint64_t	row_val;
	uintptr_t	row_addr;

	/* Each efuse row has 7 bytes of data and 1 byte + 1 bit of ECC/flags
	 * The 256 bit entry uses 56 bits on each row except the last one,
	 * which only stores 32 bit of the entry.
	 * Total space required for holding 256-bit efuse value is 5 rows.
	 */
	for (row_idx = 0; row_idx < 5; row_idx++) {
		/* Each HD eFUSE row is 65 bits, thus the offset beween rows is 16 bytes
		 * (due to line alignment).
		 * We have to use 32b access due to internal registers fabric limitation
		 */
		row_addr = EFUSE_AP_HD_ROW_ADDR(efuse_row_id + row_idx);
		row_val = readl(row_addr);
		row_val |= (u64)readl(row_addr + 4) << 32;

		for (byte = 0; byte < 7; byte++) {
			/* 7 significant bytes in each efuse row */
			val_idx   = byte + 7 * row_idx;

			/* For the last row */
			if (val_idx >= 32)
				break;

			value[val_idx] = (u8)(((row_val >> (byte * 8))) & 0x00000000000000FF);
		}
	}
}

/******************************************************************************
 *	efuse_write_enable
 *****************************************************************************/
static void efuse_write_enable(struct a8k_efuse_info *efuse_info, bool enable)
{
	uint32_t	reg32;
	uintptr_t	efuse_ctrl_reg;
	uint32_t	n;

	/* AP or CPn ? */
	if (efuse_info->typ > LD_AP)
		efuse_ctrl_reg = EFUSE_CPN_SRV_CTRL_REG(efuse_info->typ - LD_CP0);
	else
		efuse_ctrl_reg = EFUSE_AP_SRV_CTRL_REG;

	if (enable != 0) {
		/* Enable eFuse program mode on AP and/or CPn */
		reg32 = readl(efuse_ctrl_reg);
		debug("%s: Read from %#lx => %#x\n", __func__, efuse_ctrl_reg, reg32);
		reg32 |= EFUSE_SRV_CTRL_PROG_EN_MASK;
		if (efuse_info->typ != HD_AP) {
			/* Select the user LD efuse - we never touch the "chip" LD efuse */
			reg32 |= EFUSE_SRV_CTRL_LD_SEL_USER_MASK;
			/* The following bit will disable any furhter modifications to LD efuse */
			if (efuse_info->id == EFUSE_ID_TRUSTED_EN)
				reg32 |= EFUSE_SRV_CTRL_LD_SEC_EN_MASK;
		}
		debug("%s: Write to %#lx => %#x\n", __func__, efuse_ctrl_reg, reg32);
		writel(reg32, efuse_ctrl_reg);
		/* When trusted boot mode bit or JTAG enable bit are programmed,
		 * they should be propagated accross all CPn
		 */
		if ((efuse_info->id == EFUSE_ID_TRUSTED_EN) ||
		    (efuse_info->id == EFUSE_ID_JTAG_PERM_EN)) {
			for (n = 0; n < NUM_OF_CPS; n++) {
				reg32 = readl(EFUSE_CPN_SRV_CTRL_REG(n));
				debug("%s: Read from %#lx => %#x\n",
				      __func__, EFUSE_CPN_SRV_CTRL_REG(n), reg32);
				reg32 |= EFUSE_SRV_CTRL_PROG_EN_MASK;
				/* Only modify user LD efuse on CPn */
				reg32 |= EFUSE_SRV_CTRL_LD_SEL_USER_MASK;
				/* The following bit will disable any furhter modifications to LD efuse */
				if (efuse_info->id == EFUSE_ID_TRUSTED_EN)
					reg32 |= EFUSE_SRV_CTRL_LD_SEC_EN_MASK;
				debug("%s: Write to %#lx => %#x\n",
				      __func__, EFUSE_CPN_SRV_CTRL_REG(n), reg32);
				writel(reg32, EFUSE_CPN_SRV_CTRL_REG(n));
			}
		}
	} else {
		/* Disable eFuse program mode on AP and/or CPn */
		reg32 = readl(efuse_ctrl_reg);
		debug("%s: Read from %#lx => %#x\n", __func__, efuse_ctrl_reg, reg32);
		reg32 &= ~EFUSE_SRV_CTRL_PROG_EN_MASK;
		debug("%s: Write to %#lx => %#x\n", __func__, efuse_ctrl_reg, reg32);
		writel(reg32, efuse_ctrl_reg);
		/* When trusted boot mode bit or JTAG enable bit are programmed,
		 * they are propagated accross all CPn.
		 * So the burn mode on all CPn should be disabled afterwards.
		 */
		 if ((efuse_info->id == EFUSE_ID_TRUSTED_EN) ||
		     (efuse_info->id == EFUSE_ID_JTAG_PERM_EN)) {
			for (n = 0; n < NUM_OF_CPS; n++) {
				reg32 = readl(EFUSE_CPN_SRV_CTRL_REG(n));
				debug("%s: Read from %#lx => %#x\n",
				      __func__, EFUSE_CPN_SRV_CTRL_REG(n), reg32);
				reg32 &= ~EFUSE_SRV_CTRL_PROG_EN_MASK;
				debug("%s: Write to %#lx => %#x\n",
				      __func__, EFUSE_CPN_SRV_CTRL_REG(n), reg32);
				writel(reg32, EFUSE_CPN_SRV_CTRL_REG(n));
			}
		}
	}
}

/******************************************************************************
 *	efuse_write_hd_256bit
 ****************************************************************************/
static void efuse_write_hd_256bit(uint32_t efuse_row_id, uint8_t *value)
{
	uint32_t	word[2];
	uintptr_t	row_addr;
	uint32_t	row, n;
	uint32_t	efval[9];

	/* Each efuse row has 7 bytes of data and 1 byte + 1 bit of ECC/flags
	 * The 256 bit entry uses 56 bits on each row except the last one,
	 * which only stores 32 bit of the entry.
	 * Total space required for holding 256-bit efuse value is 5 rows.
	 */
	for (row = 0; row < 5; row++) {
		row_addr = EFUSE_AP_HD_ROW_ADDR(row + efuse_row_id);
		/* Read entire row before write */
		for (n = 0; n < 3; n++) {
			efval[n] = readl(row_addr + n * 4);
			debug("%s: Read from %#lx => %#x\n",
			      __func__, row_addr + n * 4, efval[n]);
		}
		/* 7 bytes per row */
		/* 4 bytes in first word */
		word[0]  = value[0 + row * 7];
		word[0] |= value[1 + row * 7] << 8;
		word[0] |= value[2 + row * 7] << 16;
		word[0] |= value[3 + row * 7] << 24;
		writel(word[0], row_addr);
		debug("%s: Write to %#lx => %#x\n", __func__, row_addr, word[0]);
		/* 5 rows => 7 + 7 + 7 + 7 + 4 bytes */
		if (row < 4) {
			/* 3 bytes in second word */
			word[1]  = value[4 + row * 7];
			word[1] |= value[5 + row * 7] << 8;
			word[1] |= value[6 + row * 7] << 16;
			writel(word[1], row_addr + 4);
			debug("%s: Write to %#lx => %#x\n", __func__, row_addr + 4, word[1]);
		} else {
			/* last row has no value in its second word */
			writel(0, row_addr + 4);
			debug("%s: Write to %#lx => %#x\n", __func__, row_addr + 4, 0);
		}
		/* Validity bit for HD eFuse row.
		 * Multi-DWORD eFuses are modified only once.
		 * Writing bit[64] writes down the register valules into eFuse
		 * row and locks it down preventing from further modifications.
		 * Activation of secure boot mode validates the secure boot eFuse
		 * row and all preceding lines.
		 * The eFuse content is valid (and protected from writes) when
		 * bit[64] of the eFuse line is row.
		 */
		writel(1, row_addr + 8);
		debug("%s: Write to %#lx => %#x\n", __func__, row_addr + 8, 1);
		/* Must wait 1ms after last word write access on each line */
#ifndef CONFIG_PALLADIUM
		mdelay(1);
#endif
	}
}

/******************************************************************************
 *	efuse_write_hd_csk_idx
 *****************************************************************************/
static void efuse_write_hd_csk_idx(uint32_t efuse_row_id, uint32_t val32)
{
	uintptr_t	row_addr;
	uint32_t	row, n;
	uint32_t	efval[9];

	for (row = 0; row < 15; row++) {
		/* Valid CSK index is the index of the first empty row in CSK block */
		if (val32 == row)
			break;

		row_addr = EFUSE_AP_HD_ROW_ADDR(row + efuse_row_id);
		/* Read entire row before write */
		for (n = 0; n < 3; n++) {
			efval[n] = readl(row_addr + n * 4);
			debug("%s: Read from %#lx => %#x\n",
			      __func__, row_addr + n * 4, efval[n]);
		}

		writel(1, row_addr);
		debug("%s: Write to %#lx => %#x\n", __func__, row_addr, 1);
		writel(0, row_addr + 4);
		debug("%s: Write to %#lx => %#x\n", __func__, row_addr + 4, 0);
		/* Validity bit for HD eFuse row.
		 * Write CSK entry locks the preceding eFuse rows starting at CSK0.
		 */
		writel(1, row_addr + 8);
		debug("%s: Write to %#lx => %#x\n", __func__, row_addr + 8, 1);
		/* Must wait 1ms after last word write access on each line */
#ifndef CONFIG_PALLADIUM
		mdelay(1);
#endif
	}
}

/******************************************************************************
 *	efuse_write_hd_ld_short
 *****************************************************************************/
static void efuse_write_hd_ld_short(struct a8k_efuse_info *efuse_info,
				    uint32_t val32)
{
	uintptr_t	row_addr;
	uint32_t	n, words;
	uint32_t	efval[9];

	if (efuse_info->typ == HD_AP) {
		row_addr = EFUSE_AP_HD_ROW_ADDR(efuse_info->row);
		words = 3;
	} else if (efuse_info->typ == LD_AP) {
		row_addr = EFUSE_AP_LD_BASE_ADDR + efuse_info->row * 4;
		words = 9;
	} else {/* (efuse_info->typ == LD_CP0) */
		row_addr = EFUSE_CPN_LD_BASE_ADDR(efuse_info->typ - LD_CP0) +
			   efuse_info->row * 4;
		words = 9;
	}
	/* In current HW all short eFuse values are located in LSB row word
	 * However we must read the entire HD row or LD efuse before write
	 */
	for (n = 0; n < words; n++) {
		efval[n] = readl(row_addr + n * 4);
		debug("%s: Read from %#lx => %#x\n",
		      __func__, row_addr + n * 4, efval[n]);
	}

	efval[efuse_info->bitoffs / 32] |= val32 << efuse_info->bitoffs % 32;

	if (efuse_info->id == EFUSE_ID_TRUSTED_EN) { /* AP LD only, CPn burned later */
		/* For the last write also burn the following bits:
		 * AP LD1  - bit[46] - CPU WakeUP, bit[47] CPU WakeUP enable
		 * CPn LD1 - bit[32] - disable reset de-assetion
		 */
		 efval[1] |= 0x3 << 14;
	}
	/* Validity bit for HD eFuse row.
	 * Short HD eFuses do not share the row, so after writing
	 * eFuse value the row must be locked (validated).
	*/
	if (efuse_info->typ == HD_AP)
		efval[words - 1] = 1;
	else	/* For LD efuses writing 0 to the last word initiates the burn operation */
		efval[words - 1] = 0;

	/* Write all the words of the row/efuse */
	for (n = 0; n < words; n++) {
		debug("%s: Write to %#lx => %#x\n",
		      __func__, row_addr + n * 4, efval[n]);
		writel(efval[n], row_addr + n * 4);
	}

	/* Must wait 1ms after last word write access on each line */
#ifndef CONFIG_PALLADIUM
	mdelay(1);
#endif
	/* When trusted boot mode bit or JTAG enable bit are programmed,
	 * they should be propagated accross all CPn
	 */
	if ((efuse_info->id == EFUSE_ID_TRUSTED_EN) ||
	    (efuse_info->id == EFUSE_ID_JTAG_PERM_EN)) {
		for (n = 0; n < NUM_OF_CPS; n++) {
			row_addr = EFUSE_CPN_LD_BASE_ADDR(n) + efuse_info->row * 4;
			/* Read entire LD efuse before write */
			for (n = 0; n < 9; n++) {
				efval[n] = readl(row_addr + n * 4);
				debug("%s: Read from %#lx => %#x\n",
				      __func__, row_addr + n * 4, efval[n]);
			}

			efval[efuse_info->bitoffs / 32] |=
					val32 << efuse_info->bitoffs % 32;
			/* CPn only, AP LD already burned */
			if (efuse_info->id == EFUSE_ID_TRUSTED_EN) {
				/* For the last write also burn the following bits:
				 * AP LD1  - bit[46] - CPU WakeUP, bit[47] CPU WakeUP enable
				 * CPn LD1 - bit[32] - disable reset de-assetion
				 */
				 efval[1] |= 1;
			}
			/* For LD efuses writing 0 to the last word initiates burn operation */
			efval[8] = 0;
			/* Write everything back */
			for (n = 0; n < 9; n++) {
				debug("%s: Write to %#lx => %#x\n",
				      __func__, row_addr + n * 4, efval[n]);
				writel(efval[n], row_addr + n * 4);
			}
			/* Must wait 1ms after last word write access on each line */
#ifndef CONFIG_PALLADIUM
			mdelay(1);
#endif
		}
	}
}

/************************************
*	Global functions
*************************************/

/******************************************************************************
 *	efuse_id_valid
 *****************************************************************************/
int efuse_id_valid(enum efuse_id fid)
{
	if (fid < EFUSE_ID_MAX)
		return 1;
	else
		return 0;
}

/******************************************************************************
 *	efuse_write
 *****************************************************************************/
int efuse_write(enum efuse_id fid, const char *value)
{
	uint32_t		info_idx, n;
	enum a8k_ap_boot_dev	ap_bdev;
	uint32_t		val32 = 0;
	uint8_t			val256[32] = {0};
	char			ascii_buf[3] = {0, 0, 0};
	uint32_t		value_len = strlen(value);

	/* Find efuse info with length and offset */
	for (info_idx = 0; info_idx < EFUSE_ID_MAX; info_idx++) {
		if (efuse_info[info_idx].id == fid) {
			debug(":%s: Requested efuse ID %d\n", __func__, fid);
			break;
		}
	}

	if (info_idx == EFUSE_ID_MAX) {
		error("%s: Unsupported eFuse ID\n", __func__);
		return 1;
	}

	/* Parse input parameter */
	switch (fid) {
	case EFUSE_ID_BOOT_DEVICE:
		for (ap_bdev = 0; ap_bdev < MAX_AP_BOOT_DEV; ap_bdev++) {
			if (strcmp(value, ap_boot_dev_names[ap_bdev]) == 0)
				break;
		}
		if (ap_bdev == MAX_AP_BOOT_DEV) {
			error("%s: Unsupported AP Boot Device \"%s\"\n", __func__, value);
			return 1;
		}
		val32 = ap_bdev;
		break;

	case EFUSE_ID_CP0_BOOT_DEVICE:
		val32 = simple_strtoul(value, 0, 16);
		if (val32 >= MAX_CP_BOOT_DEV) {
			error("%s: Unsupported CP Boot Device \"%#0x\"\n", __func__, val32);
			return 1;
		}
		break;

	case EFUSE_ID_KAK_DIGEST:
	case EFUSE_ID_AES_KEY:
		if (value_len != MVEBU_EFUSE_256B_ASCII_LEN) {
			error("%s: Unsupported key length (%d) - expected exactly %d bytes\n",
			      __func__, value_len >> 1, MVEBU_EFUSE_256B_ASCII_LEN >> 1);
			return 1;
		}
		/* Convert ASCII representation to array of byte integers */
		for (n = 0; n < 32; n++) {
			memcpy(ascii_buf, &value[n * 2], 2);	/* 2 ASCII characters per byte */
			val256[n] = 0xFF & simple_strtoul(ascii_buf, 0, 16);
		}
		break;

	case EFUSE_ID_CSK_INDEX:
		val32 = simple_strtoul(value, 0, 10);
		if (val32 > 15) {
			error("%s: Bad CSK index \"%s\", expected 0 - 15\n", __func__, value);
			return 1;
		}
		break;

	case EFUSE_ID_FLASH_ID:
	case EFUSE_ID_BOX_ID:
		if (value_len > (efuse_info[info_idx].numbits >> 2)) {
			/* 4 bits per ASCII symbol */
			error("%s: Unsupported value length (%d) - expected up to %d bytes\n",
			      __func__, value_len >> 3, efuse_info[info_idx].numbits >> 3);
			return 1;
		}
		val32 = simple_strtoul(value, 0, 16);
		break;

	case EFUSE_ID_CFG_BASE_ADDR:
	case EFUSE_ID_ROM_BASE_ADDR:
		if (value_len > 8) { /* 32 bit address, 4 bits per ASCII symbol */
			error("%s: Unsupported address length (%d) - expected up to 4 bytes\n",
			      __func__, value_len >> 3);
			return 1;
		}
		val32 = simple_strtoul(value, 0, 16);
		if (val32 != (val32 & 0xFFFFF)) {
			error("%s: Malformed address - should be 1MB aligned\n", __func__);
			return 1;
		}
		val32 >>= 20;
		break;

	case EFUSE_ID_JTAG_PERM_EN:	/* same field on AP and CPn LD1 eFuse */
	case EFUSE_ID_TRUSTED_EN:	/* same field on AP and CPn LD1 eFuse */
	case EFUSE_ID_CFG_BASE_USE:
	case EFUSE_ID_ROM_BASE_USE:
		val32 = simple_strtoul(value, 0, 10);
		if (val32 != 1) {
			error("%s: Unsupported value (%d) - expected 1\n", __func__, val32);
			return 1;
		}
		break;

	default:
		printf("%s: This eFuse ID write function is not implemented\n", __func__);
		return 1;
	}

	/* Enable eFuse program mode on AP and/or CPn */
	efuse_write_enable(&efuse_info[info_idx], 1);

	/* Write the eFuse value */
	if (efuse_info[info_idx].numbits == 256) {
		/* Long, multi-row  keys - exist only in HD efuses */
		efuse_write_hd_256bit(efuse_info[info_idx].row, val256);
	} else if (fid == EFUSE_ID_CSK_INDEX) {
		/* CSK IDX is a special case */
		efuse_write_hd_csk_idx(efuse_info[info_idx].row, val32);
	} else {
		/* Short HD and LD eFuse values */
		efuse_write_hd_ld_short(&efuse_info[info_idx], val32);
	}

	/* Disable eFuse program mode on AP and/or CPn */
	efuse_write_enable(&efuse_info[info_idx], 0);

	return 0;
}

/******************************************************************************
 *	efuse_read
 *****************************************************************************/
int efuse_read(enum efuse_id fid, char *value)
{
	uint32_t	info_idx, n;
	uint8_t		buf256[32] = {0};
	uint32_t	buf32 = 0;

	/* Find efuse info with length and offset */
	for (info_idx = 0; info_idx < EFUSE_ID_MAX; info_idx++) {
		if (efuse_info[info_idx].id == fid)
			break;
	}

	if (info_idx == EFUSE_ID_MAX) {
		printf("%s: Unsupported eFuse ID\n", __func__);
		return 1;
	}

	/* Read the eFuse value */
	if (efuse_info[info_idx].typ == HD_AP) {
		if (efuse_info[info_idx].numbits == 256) {
			efuse_read_hd_256bit(efuse_info[info_idx].row, buf256);
		} else {
			/* Current HW supports 32bit short HD eFuse values
			   No need for masking and shifting value */
			efuse_read_hd_32bit(efuse_info[info_idx].row, &buf32);
		}
	} else if (efuse_info[info_idx].typ < INVALID_TYPE) {
		efuse_read_ld(efuse_info[info_idx].typ, efuse_info[info_idx].row, &buf32);
		buf32 >>= efuse_info[info_idx].bitoffs;
		buf32 &= (1 << efuse_info[info_idx].numbits) - 1;
	} else {
		printf("%s: BUG! Wrong eFuse type\n", __func__);
		return 1;
	}

	/* Format the efuse value */
	switch (fid) {
	case EFUSE_ID_BOOT_DEVICE:
		if ((buf32 >= MAX_AP_BOOT_DEV) || (strlen(ap_boot_dev_names[buf32]) == 0))
			sprintf(value, "INVALID VALUE (%d)", buf32);
		else
			sprintf(value, "%s (%d)", ap_boot_dev_names[buf32], buf32);
		break;

	case EFUSE_ID_CP0_BOOT_DEVICE:
		if (buf32 >= MAX_CP_BOOT_DEV)
			sprintf(value, "INVALID VALUE (%d)", buf32);
		else
			sprintf(value, "%02X", buf32);
		break;

	case EFUSE_ID_KAK_DIGEST:
	case EFUSE_ID_AES_KEY:
		for (n = 0; n < 32; n++)
			sprintf(value + n * 2, "%02X", buf256[n]);
		break;

	case EFUSE_ID_CSK_INDEX:
		n = 0;
		if (buf32 != 0) {
			/* CSK index is not 0, read the rest of efuse rows to find out its value */
			for (n = 1; n < 16; n++) {
				efuse_read_hd_32bit(efuse_info[info_idx].row + n, &buf32);
				if (buf32 == 0)
					break;
			}
		}
		if (n == 16) {
			/* All CSK indexes invalidated */
			sprintf(value, "INVALID IDX");
			return 1;
		}
		sprintf(value, "%d", n);
		break;

	case EFUSE_ID_FLASH_ID:
	case EFUSE_ID_BOX_ID:
		sprintf(value, "%08X", buf32);
		break;

	case EFUSE_ID_CFG_BASE_ADDR:
	case EFUSE_ID_ROM_BASE_ADDR:
		sprintf(value, "%08X", buf32 << 20);
		break;

	case EFUSE_ID_JTAG_PERM_EN:	/* same field on AP and CPn LD1 eFuse */
	case EFUSE_ID_TRUSTED_EN:	/* same field on AP and CPn LD1 eFuse */
	case EFUSE_ID_CFG_BASE_USE:
	case EFUSE_ID_ROM_BASE_USE:
		sprintf(value, "%d", buf32);
		break;

	default:
		sprintf(value, "NOT IMPLEMENTED");
	}

	return 0;
}

/******************************************************************************
 *	efuse_raw_dump
 ****************************************************************************/
void efuse_raw_dump(void)
{
	int8_t		bit_idx, row_idx, n;
	const uint8_t	first_row = 0, last_row = 51;
	uint64_t	row_val;
	uint32_t	bit64, val32;
	uintptr_t	row_addr;

	printf("-----------------------------------------------------------------|-------------------|\n");
	printf("    6         5         4         3         2         1         0|    AP HD eFuse    |\n");
	printf("43210987654321098765432109876543210987654321098765432109876543210|    HEX VALUE   |ID|\n");
	printf("-----------------------------------------------------------------|----------------|--|\n");

	for (row_idx = first_row; row_idx < last_row; row_idx++) {
		row_addr = EFUSE_AP_HD_ROW_ADDR(row_idx);
		row_val = readl(row_addr);
		row_val |= (u64)readl(row_addr + 4) << 32;
		bit64 = readl(row_addr + 8) & 0x1;

		/* Print single bits */
		printf("%01d", bit64);
		for (bit_idx = 63; bit_idx >= 0; bit_idx--)
			printf("%01d", (int)((row_val >> bit_idx) & 0x1));

		/* Print HEX values */
		printf("|%016llX|%02d|\n", row_val, row_idx);
	}
	printf("|---------|----------------------------------------------------------------|---------|\n");
	printf("|   BIT   |   6         5         4         3         2         1         0| LD eFuse|\n");
	printf("|  RANGE  |3210987654321098765432109876543210987654321098765432109876543210|   ID    |\n");
	printf("|---------|----------------------------------------------------------------|---------|\n");

	/* Select user AP LD */
	val32 = readl(EFUSE_AP_SRV_CTRL_REG);
	val32 |= EFUSE_SRV_CTRL_LD_SEL_USER_MASK;
	writel(val32, EFUSE_AP_SRV_CTRL_REG);

	for (row_idx = 7; row_idx >= 0; row_idx--) {
		if ((row_idx % 2) != 0)
			printf("|%03d - %03d|", (row_idx / 2) * 64, (row_idx / 2) * 64 + 63);

		val32 = readl(EFUSE_AP_LD_BASE_ADDR + row_idx * 4);
		for (bit_idx = 31; bit_idx >= 0; bit_idx--)
			printf("%01d", (int)((val32 >> bit_idx) & 0x1));
		if ((row_idx % 2) == 0)
			printf("| AP LD1.%d|\n", row_idx / 2);
	}

	for (n = 0; n < NUM_OF_CPS; n++) {
		/* Select user CPn LD */
		val32 = readl(EFUSE_CPN_SRV_CTRL_REG(n));
		val32 |= EFUSE_SRV_CTRL_LD_SEL_USER_MASK;
		writel(val32, EFUSE_CPN_SRV_CTRL_REG(n));

		for (row_idx = 7; row_idx >= 0; row_idx--) {
			if ((row_idx % 2) != 0)
				printf("|%03d - %03d|", (row_idx / 2) * 64, (row_idx / 2) * 64 + 63);

			val32 = readl(EFUSE_CPN_LD_BASE_ADDR(n) + row_idx * 4);
			for (bit_idx = 31; bit_idx >= 0; bit_idx--)
				printf("%01d", (int)((val32 >> bit_idx) & 0x1));
			if ((row_idx % 2) == 0)
				printf("|CP%01d LD1.%d|\n", n, row_idx / 2);
		}
	}
}
