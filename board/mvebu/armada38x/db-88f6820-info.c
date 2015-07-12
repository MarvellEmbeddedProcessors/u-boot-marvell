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

struct mvebu_board_config db_a38x_config = {
	.sar_cnt = 2,
	.active_sar = {BOOT_SRC_SAR, CPUS_NUM_SAR},
	.cfg_cnt = 2,
	.active_cfg = {BOARD_ID_CFG, SERDES_0_CFG}
};

int board_get_id(void)
{
	return ARMADA_38X_DB_ID;
}

int db_a38x_configure_mpp(void)
{
#ifdef CONFIG_MVEBU_MPP_BUS
#ifdef CONFIG_MVEBU_NAND_BOOT
	mpp_enable_bus("nand");
#endif
#ifdef CONFIG_MVEBU_SPI_BOOT
	mpp_enable_bus("spi0");
#endif
#endif
	return 0;
}

struct mvebu_board_info a38x_db_info = {
	.name = "DB-88F6820-BP",
	.id = ARMADA_38X_DB_ID,
	.mpp_regs = {0x11111111, 0x11111111, 0x11266011, 0x22222011,
		     0x22200002, 0x40042022, 0x55550555, 0x00005550},
	.configure_mpp = &db_a38x_configure_mpp,
	.configurable = 1,
	.config_data = &db_a38x_config,
};

