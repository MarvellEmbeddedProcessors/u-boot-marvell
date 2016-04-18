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

/* APN-806 Marvell boards */
#define A8K_MARVELL_BOARD_ID_BASE			0x10
#define A8K_APN_806_DB					(A8K_MARVELL_BOARD_ID_BASE + 0)
#define A8K_7040_RZ					(A8K_MARVELL_BOARD_ID_BASE + 1)
#define A8K_7040					(A8K_MARVELL_BOARD_ID_BASE + 2)
#define A8K_8040					(A8K_MARVELL_BOARD_ID_BASE + 3)
#define A8K_MARVELL_MAX_BOARD_ID			(A8K_MARVELL_BOARD_ID_BASE + 4)
#define A8K_MARVELL_BOARD_NUM				(A8K_MARVELL_MAX_BOARD_ID - A8K_MARVELL_BOARD_ID_BASE)

#define MARVELL_BOARD_ID_BASE				A8K_MARVELL_BOARD_ID_BASE
#define MV_MAX_MARVELL_BOARD_ID				A8K_MARVELL_MAX_BOARD_ID
#define MV_MARVELL_BOARD_NUM				A8K_MARVELL_BOARD_NUM
#define MV_DEFAULT_BOARD_ID				A8K_7040

#define BOARD_DEV_TWSI_INIT_EEPROM 0x57
#define MULTI_FDT_EEPROM_ADDR_LEN 2

/* default FDT selection for each board */
#define DEFAULT_FDT_PER_BOARD { \
				0x04,	/* APN_806_DB	*/ \
				0x01,	/* 7040_RZ_DB	*/ \
				0x01,	/* 7040_DB	*/ \
				0x01,	/* 8040_DB	*/ \
}
