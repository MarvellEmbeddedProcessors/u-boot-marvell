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
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/mpp.h>
#include "board-info.h"


struct mvebu_board_config rd_a38x_config = {
	.sar_cnt = 1,
	.active_sar = {CPUS_NUM_SAR},
	.cfg_cnt = 1,
	.active_cfg = {BOARD_ID_CFG}
};

u16 a38x_rd_unit_disable[MAX_UNIT_ID] = {
	[NAND_UNIT_ID] = id_to_mask(0),
	[SDIO_UNIT_ID] = id_to_mask(0)
};

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
	.name = "RD-88F6820-GP",
	.id = ARMADA_38X_RD_ID,
	.unit_mask = a38x_rd_unit_disable,
	.unit_update_mode = UNIT_INFO_DISABLE,
	.mpp_regs = {0x11111111, 0x11111111, 0x11266011, 0x22222011,
				0x22200002, 0x40042022, 0x55550555, 0x00005550},
	.configure_mpp = &rd_a38x_configure_mpp,
	.configurable = 1,
	.config_data = &rd_a38x_config,
};
