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

#ifndef _SOC_INFO_H_
#define _SOC_INFO_H_

/* MBUS definitions*/
#define MAX_MBUS_WINS			19 /* window #20 reserved for internal register */
#define MAX_MBUS_REMAP_WINS		8
#define INTERNAL_REG_WIN_NUM		20

/* General MPP definitions */
#define MAX_MPP_OPTS		7
#define MAX_MPP_ID		59

#define MPP_BIT_CNT		4
#define MPP_FIELD_MASK		0x7
#define MPP_FIELD_BITS		3
#define MPP_VAL_MASK		0xF

#define MPPS_PER_REG		(32 / MPP_BIT_CNT)
#define MAX_MPP_REGS		((MAX_MPP_ID + MPPS_PER_REG) / MPPS_PER_REG)

/* Pin Ctrl driver definitions */
#define BITS_PER_PIN		4
#define PIN_FUNC_MASK		((1 << BITS_PER_PIN) - 1)
#define PIN_REG_SHIFT		3
#define PIN_FIELD_MASK		((1 << PIN_REG_SHIFT) - 1)


#endif	/* _SOC_INFO_H_ */
