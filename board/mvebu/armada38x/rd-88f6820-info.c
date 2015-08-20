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
#include <linux/compiler.h>
#include <asm/arch-mvebu/mpp.h>
#include "board-info.h"

int rd_a38x_configure_mpp(void)
{
#ifdef CONFIG_MVEBU_MPP_BUS
#ifdef CONFIG_MVEBU_SPI_BOOT
	mpp_enable_bus("spi0");
#endif
#endif
	return 0;
}

struct mvebu_board_info a38x_rd_info = {
	.id = ARMADA_38X_RD_ID,
	.configure_mpp = &rd_a38x_configure_mpp,
};
