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
#include "types.h"
#include "bits.h"
#include "regs.h"
#include "io.h"
#include "clock.h"
#include "mbox.h"
#include "efuse.h"

static inline void seq1(void)
{
	writel(0x0300, EFUSE_CONTROL_REG);
	writel(0x0700, EFUSE_CONTROL_REG);
}

static inline void seq0(void)
{
	writel(0x0100, EFUSE_CONTROL_REG);
	writel(0x0500, EFUSE_CONTROL_REG);
}

/***************************************************************************************************
 * efuse_write_enable
 *
 * return: status
 ***************************************************************************************************/
static u32 efuse_write_enable(u32 enable)
{
	u32	status = NO_ERROR;
	u32	regval;
	u32	n, timeout;

	if (enable == 0) {	/* Disable */

		/* Deactivate Program Sequence Code & Burning Circuitry
		   by toggling PROG_SEQ_CODE_CLK
		 */
		 writel(0x0005, EFUSE_CONTROL_REG);
		 writel(0x0405, EFUSE_CONTROL_REG);
		 writel(0x0005, EFUSE_CONTROL_REG);

		 /* Unset Master OTP Program Enable
		    Applies to both Security Control OTP & Security Data OTP
		  */
		 writel(EFUSE_MASTER_OTP_PRG_DIS, EFUSE_MASTER_OTP_CTRL_REG);

	} else {		/* Enable */

		/* Power up SD OTP */
		writel(0x0, EFUSE_CONTROL_REG);
		wait_ns(EFUSE_POWER_UP_TIME_NS);

		/* Set Master OTP Program Enable
		   Applies to both Security Control OTP & Security Data OTP.
		 */
		writel(EFUSE_MASTER_OTP_PRG_EN, EFUSE_MASTER_OTP_CTRL_REG);

		/* Turn on the clock and run sequence ”1011000110” 6 times */
		writel(EFUSE_CTRL_SCLK_BIT, EFUSE_CONTROL_REG);
		for (n = 0; n < 6; n++) {
			seq1();
			seq0();
			seq1();
			seq1();
			seq0();
			seq0();
			seq0();
			seq1();
			seq1();
			seq0();
		}
		/* Program Sequence Done */
		writel(0x0, EFUSE_CONTROL_REG);
		/* Check for CODE MATCH */
		for (timeout = 0, status = ERR_TIMEOUT;
		     timeout < EFUSE_CODE_MATCH_TIMEOUT_LOOPS; timeout++) {
			regval = readl(EFUSE_AUXILIARY_REG);
			regval &= EFUSE_AUX_CODE_MATCH_BIT;
			if (regval) {
				status = NO_ERROR;
				break;
			}
			wait_ns(EFUSE_CODE_MATCH_LOOP_WAIT_NS);
		}
	}

	return status;
}

/***************************************************************************************************
 * efuse_read_row_no_ecc
 *
 * return: status
 ***************************************************************************************************/
static u32 efuse_read_row_no_ecc(u32 row, u32 *msb, u32 *lsb)
{
	u32	regval;
	u32	timeout;
	u32	status;

	if (msb == NULL || lsb == NULL)
		return ERR_INVALID_ARGUMENT;

	/* Set PRDT=0 to clear RD_DONE and any other SD OTP read status */
	regval = EFUSE_CTRL_DEF_VAL | EFUSE_CTR_CSB_BIT;
	writel(regval, EFUSE_CONTROL_REG);

	wait_ns(EFUSE_POWER_UP_TIME_NS);

	/* Set PRDT=1, Enter Access Mode */
	regval = readl(EFUSE_CONTROL_REG);
	regval |= EFUSE_CTR_PRDT_BIT;
	writel(regval, EFUSE_CONTROL_REG);

	/* Set CSB = 0, PGM_B = 1, LOAD = 1, Enter Read Mode */
	regval = readl(EFUSE_CONTROL_REG);
	regval &= ~EFUSE_CTR_CSB_BIT;
	regval |= EFUSE_CTR_PGM_B_BIT | EFUSE_CTR_LOAD_BIT;
	writel(regval, EFUSE_CONTROL_REG);

	/* set ROW & COL address */
	regval = EFUSE_RW_ROW(row) | EFUSE_RW_COL(0);
	writel(regval, EFUSE_READ_WRITE_REG);

	/* wait minimum 4ns */
	wait_ns(EFUSE_ROW_COL_SET_TIME_NS);

	/* Toggle SCLK = 1 for minimum 165ns */
	regval = readl(EFUSE_CONTROL_REG);
	regval |= EFUSE_CTRL_SCLK_BIT;
	writel(regval, EFUSE_CONTROL_REG);

	wait_ns(EFUSE_READ_SCLK_PULSE_WIDTH_NS);

	regval = readl(EFUSE_CONTROL_REG);
	regval &= ~EFUSE_CTRL_SCLK_BIT;
	writel(regval, EFUSE_CONTROL_REG);

	/* Set CSB = 1, LOAD = 0 to finish read cycle */
	regval = readl(EFUSE_CONTROL_REG);
	regval |= EFUSE_CTR_CSB_BIT;
	regval &= ~EFUSE_CTR_LOAD_BIT;
	writel(regval, EFUSE_CONTROL_REG);

	/* If RD_DONE then OK to read row value */
	for (timeout = 0, status = ERR_TIMEOUT;
	     timeout < EFUSE_READ_TIMEOUT_LOOPS; timeout++) {
		regval = readl(EFUSE_AUXILIARY_REG);
		regval &= EFUSE_AUX_RD_DONE_BIT;
		if (regval) {
			status = NO_ERROR;
			*lsb = readl(EFUSE_DATA0_REG);
			*msb = readl(EFUSE_DATA1_REG);
			break;
		}
		wait_ns(EFUSE_READ_LOOP_WAIT_NS);
	}

	return status;
}

/***************************************************************************************************
 * efuse_read
 *
 * return: status
 ***************************************************************************************************/
u32 efuse_read(u32 size, u32 row, u32 offset, u32 *args)
{
	u32	lsb, msb, count;
	u32	status;
	u32	loops = EFUSE_ACCESS_LOOPS(size);

	if (args == NULL || row >= EFUSE_MAX_ROW || offset > EFUSE_BITS_IN_ROW) {
		status = ERR_INVALID_ARGUMENT;
		goto rd_error;
	}

	/* read row by row */
	for (count = 0; count < loops; count++) {
		status = efuse_read_row_no_ecc(row + count, &msb, &lsb);
		if (status != NO_ERROR)
			goto rd_error;

		args[count * 2]     = lsb;
		args[count * 2 + 1] = msb;
	}

	/* remove noise from filds shorter than full row */
	if (size < EFUSE_BITS_IN_ROW) {
		/* 64-bit right shift */
		if (offset < 32) {
			args[0] >>= offset;
			args[0] |= args[1] << (32 - offset);
			args[1] >>= offset;
		} else {
			args[0] = args[1] >> (offset - 32);
			args[1] = 0;
		}

		if (size == 1) {
			/* single bit field values are calculated as
			   majority vote between 3 physical bits */
			args[0] &= 0x7;
			args[1] = 0;
			if (args[0] == 3 || args[0] > 4)
				args[0] = 1;
			else
				args[0] = 0;

		} else if (size <= 32) {
			args[0] &=  0xFFFFFFFF >> (32 - size);
			args[1] = 0;
		}
	}

rd_error:
	if (status == ERR_INVALID_ARGUMENT)
		args[0] = MB_STAT_BAD_ARGUMENT;
	else if (status == ERR_TIMEOUT)
		args[0] = MB_STAT_TIMEOUT;
	else if (status != NO_ERROR)
		args[0] = MB_STAT_HW_ERROR;

	return status;
}
/***************************************************************************************************
 * efuse_write
 *
 * return: status
 ***************************************************************************************************/
u32 efuse_write(u32 size, u32 row, u32 offset, u32 *args)
{
	u32	lsb, msb, count, col;
	u32	status;
	u32	loops = EFUSE_ACCESS_LOOPS(size);
	u32	regval;

	if (args == NULL || row >= EFUSE_MAX_ROW || offset > EFUSE_BITS_IN_ROW) {
		status = ERR_INVALID_ARGUMENT;
		goto wr_error;
	}

	/* Single bit - requres 2-3 bits programming for majority vote */
	if (size == 1) {
		/* expand single bit values to 3 bits for physical efuse field */
		args[0] &= 1;
		args[0] |= (args[0] << 1) | (args[0] << 2);
		args[1] = 0;

	} else if (size <= 32) {
		/* cleanup the short field value */
		args[0] &=  0xFFFFFFFF >> (32 - size);
		args[1] = 0;
	}

	/* Enable OTP programming */
	status = efuse_write_enable(1);
	if (status != NO_ERROR)
		goto wr_error;

	for (count = 0; count < loops; count++) {
		/* Each eFuse row should be read before programming */
		status = efuse_read_row_no_ecc(row + count, &msb, &lsb);
		if (status != NO_ERROR)
			goto wr_error;

		/* Set PRDT=1, Enter Access Mode */
		regval = readl(EFUSE_CONTROL_REG);
		regval |= EFUSE_CTR_PRDT_BIT;
		writel(regval, EFUSE_CONTROL_REG);

		/* Set CSB = 0, PGM_B = 0, LOAD = 0, Enter User Data Write Mode */
		regval = readl(EFUSE_CONTROL_REG);
		regval &= ~(EFUSE_CTR_CSB_BIT | EFUSE_CTR_PGM_B_BIT | EFUSE_CTR_LOAD_BIT);
		writel(regval, EFUSE_CONTROL_REG);

		/* Wait minimum 300ns */
		wait_ns(EFUSE_WRITE_MODE_UP_TIME_NS);

		/* Fields smaller than entire row should be correctly positioned */
		if (size < EFUSE_BITS_IN_ROW) {
			/* 64-bit left shift */
			if (offset < 32) {
				args[1] <<= offset;
				args[1] |= args[0] >> (32 - offset);
				args[0] <<= offset;
			} else {
				args[1] = args[0] << (offset - 32);
				args[0] = 0;
			}
		}

		/* Combile eFuse old values with new ones */
		args[count * 2]     |= lsb;
		args[count * 2 + 1] |= msb;

		/* Burn the eFuse row bit by bit (bit is column) */
		for (col = 0; col < EFUSE_BITS_IN_ROW; col++) {
			if (args[count * 2 + col / 32] & 0x1) {
				/* set ROW & COL address */
				regval = EFUSE_RW_ROW(row + count) | EFUSE_RW_COL(col);
				writel(regval, EFUSE_READ_WRITE_REG);

				/* Toggle SCLK = 1 for 11000ns - 13000ns */
				regval = readl(EFUSE_CONTROL_REG);
				regval |= EFUSE_CTRL_SCLK_BIT;
				writel(regval, EFUSE_CONTROL_REG);

				wait_ns(EFUSE_WRITE_SCLK_PULSE_WIDTH_NS);

				regval = readl(EFUSE_CONTROL_REG);
				regval &= ~EFUSE_CTRL_SCLK_BIT;
				writel(regval, EFUSE_CONTROL_REG);
			}

			args[count * 2 + col / 32] >>= 1;

		} /* column loop */

/*		wait_ns(EFUSE_WAIT_BETWEEN_ROWS_NS); */
	}

	/* Set CSB = 1, Exit */
	regval = readl(EFUSE_CONTROL_REG);
	regval |= EFUSE_CTR_CSB_BIT;
	writel(regval, EFUSE_CONTROL_REG);

	/* Set PGM_B = 1, LOAD = 0, End Programming Mode */
	regval = readl(EFUSE_CONTROL_REG);
	regval &= ~EFUSE_CTR_LOAD_BIT;
	regval |= EFUSE_CTR_PGM_B_BIT;
	writel(regval, EFUSE_CONTROL_REG);

	/* Disable OTP programming */
	efuse_write_enable(0);

wr_error:
	if (status == ERR_INVALID_ARGUMENT)
		args[0] = MB_STAT_BAD_ARGUMENT;
	else if (status == ERR_TIMEOUT)
		args[0] = MB_STAT_TIMEOUT;
	else if (status != NO_ERROR)
		args[0] = MB_STAT_HW_ERROR;

	return status;
}
