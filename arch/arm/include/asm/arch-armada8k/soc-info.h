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

/* General MPP definitions */
#define MAX_MPP_OPTS		7
#define MAX_MPP_ID		15

#define MPP_BIT_CNT		4
#define MPP_FIELD_MASK		0x7
#define MPP_FIELD_BITS		3
#define MPP_VAL_MASK		0xF

#define MPPS_PER_REG		(32 / MPP_BIT_CNT)
#define MAX_MPP_REGS		((MAX_MPP_ID + MPPS_PER_REG) / MPPS_PER_REG)

/* MPP pins and functions correcsponding to UART RX connections
   This information is used for detection of recovery boot mode (boot from UART) */
#define MPP_UART_RX_PINS	{ 3, 5 }
#define MPP_UART_RX_FUNCTIONS	{ 1, 2 }

/* Pin Ctrl driver definitions */
#define BITS_PER_PIN		4
#define PIN_FUNC_MASK		((1 << BITS_PER_PIN) - 1)
#define PIN_REG_SHIFT		3
#define PIN_FIELD_MASK		((1 << PIN_REG_SHIFT) - 1)

#endif	/* _SOC_INFO_H_ */
