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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/boot_mode.h>
#include <i2c.h>

/*
 * switch to uart boot mode, this routine will be invoked
 * 1. in early stage of ATF to switch boot mode to UART
 * 2. in the latest stage of u-boot switch back to original mode
 */
void mvebu_boot_mode_set(uchar boot_mode)
{
	i2c_write(BOOT_MODE_I2C_CHIP, BOOT_MODE_I2C_ADDR, BOOT_MODE_I2C_LENG, &boot_mode, BOOT_MODE_I2C_LENG);
}

void mvebu_boot_mode_get(uchar *boot_mode)
{
	i2c_read(BOOT_MODE_I2C_CHIP, BOOT_MODE_I2C_ADDR, BOOT_MODE_I2C_LENG, boot_mode, BOOT_MODE_I2C_LENG);
}

