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

/* Armada3700 Marvell boards */
#include <dt-bindings/multi-fdt/multi-fdt.h>

/*
	ARMAMA3700_MARVELL_BOARD_ID_BASE = 0x10, for multi-fdt feature,
	each Soc takes 0x10 boards that starts from 0x10, A8K has taken
	0x10 to 0x1f, but since A8K and A3700 would not be compiled
	together, to align with A8K, A3700 should also take 0x10 to 0x1f.
	And as A3700 DB, it is the first board, should take board_id as 0x10.
*/
#define ARMAMA3700_MARVELL_BOARD_ID_BASE	 A3700_DB_ID
#define ARMAMA3700_DB			(ARMAMA3700_MARVELL_BOARD_ID_BASE + 0)
#define ARMAMA3700_MARVELL_MAX_BOARD_ID	(ARMAMA3700_MARVELL_BOARD_ID_BASE + 1)
#define ARMAMA3700_MARVELL_BOARD_NUM	(ARMAMA3700_MARVELL_MAX_BOARD_ID - ARMAMA3700_MARVELL_BOARD_ID_BASE)

#define MARVELL_BOARD_ID_BASE		ARMAMA3700_MARVELL_BOARD_ID_BASE
#define MV_MAX_MARVELL_BOARD_ID		ARMAMA3700_MARVELL_MAX_BOARD_ID
#define MV_MARVELL_BOARD_NUM		ARMAMA3700_MARVELL_BOARD_NUM
#define MV_DEFAULT_BOARD_ID		ARMAMA3700_DB

#define BOARD_DEV_TWSI_INIT_EEPROM 0x57
#define MULTI_FDT_EEPROM_ADDR_LEN 2

/*
	fdt_config_id is ID for each dts for a certain board.
	It starts from 1.
	+ arch/arm/dts/armada-lp-db.dts, board_id = A3700_DB_CONFIG_ID_USB3..
	+ arch/arm/dts/armada-lp-db-sgmii1.dts, board_id = A3700_DB_CONFIG_ID_SGMII1.
*/
#define DEFAULT_FDT_CONFIG_ID	A3700_DB_CONFIG_ID_USB3

#define DEFAULT_FDT_PER_BOARD { \
	DEFAULT_FDT_CONFIG_ID,	/* default fdt_config_id for armada-3700-db board */	\
}
