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

#ifndef _BOOT_MODE_H_
#define _BOOT_MODE_H_

/* BOOT MODE */
enum boot_mode_ids {
	BOOT_MODE_AUTO_SCAN = 0,
	BOOT_MODE_SPI,
	BOOT_MODE_EMMC,
	BOOT_MODE_EMMC_ALTERNATE,
	BOOT_MODE_SATA,
	BOOT_MODE_NAND,
	BOOT_MODE_UART,
	BOOT_MODE_MAX,
};

/* EEPROM FOR BOOT MODE */
#define BOOT_MODE_I2C_CHIP	0x4c
#define BOOT_MODE_I2C_ADDR	0
#define BOOT_MODE_I2C_LENG	1

void mvebu_boot_mode_set(uchar boot_mode);
void mvebu_boot_mode_get(uchar *boot_mode);

#endif /* _BOOT_MODE_H_ */
