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

#ifndef __EFUSE_H
#define __EFUSE_H

#define EFUSE_MAX_ROW			43
#define EFUSE_BITS_IN_ROW		64
#define EFUSE_ACCESS_LOOPS(sz)		((sz <= EFUSE_BITS_IN_ROW) ? 1 : (sz / EFUSE_BITS_IN_ROW))

#define EFUSE_ROW_COL_SET_TIME_NS	300	/* required minimum 4ns for non-ECC mode */
#define EFUSE_READ_SCLK_PULSE_WIDTH_NS	200
#define EFUSE_READ_TIMEOUT_LOOPS	100000
#define EFUSE_READ_LOOP_WAIT_NS		100
#define EFUSE_WAIT_BETWEEN_ROWS_NS	1000000
#define EFUSE_POWER_UP_TIME_NS		300
#define EFUSE_CODE_MATCH_TIMEOUT_LOOPS	100000
#define EFUSE_CODE_MATCH_LOOP_WAIT_NS	100
#define EFUSE_WRITE_MODE_UP_TIME_NS	500
#define EFUSE_WRITE_SCLK_PULSE_WIDTH_NS	13000

/* EFUSE_CONTROL_REG fields */
#define EFUSE_CTRL_SCLK_BIT		BIT8
#define EFUSE_CTRL_PDWN_BIT		BIT4
#define EFUSE_CTR_PRDT_BIT		BIT3
#define EFUSE_CTR_CSB_BIT		BIT2
#define EFUSE_CTR_LOAD_BIT		BIT1
#define EFUSE_CTR_PGM_B_BIT		BIT0
#define EFUSE_CTRL_DEF_VAL		(0)

/* EFUSE_READ_WRITE_REG fields */
#define EFUSE_RW_ROW(row)		(((row) & 0x3F) << 7)
#define EFUSE_RW_COL(col)		((col) & 0x7F)

/* EFUSE_AUXILIARY_REG fields */
#define EFUSE_AUX_RD_DONE_BIT		BIT31
#define EFUSE_AUX_CODE_MATCH_BIT	BIT29
#define EFUSE_AUX_SFB_BIT		BIT4	/* efuse protection status */

/* EFUSE_MASTER_OTP_CTRL_REG filds */
#define EFUSE_MASTER_OTP_PRG_EN		(0x5A)
#define EFUSE_MASTER_OTP_PRG_DIS	(0)

u32 efuse_write(u32 size, u32 row, u32 offset, u32 *args);
u32 efuse_read(u32 size, u32 row, u32 offset, u32 *args);

#endif /* __EFUSE_H */
