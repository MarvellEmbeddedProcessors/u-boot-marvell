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
	ARMAMA3700_MARVELL_BOARD_ID_BASE = 0x20, for multi-fdt feature,
	each Soc takes 0x10 boards, so, A3700 should take 0x20 to 0x2f.
	And as A3700 DB, it is the first board, should take board_id as 0x20.
*/
#define ARMAMA3700_MARVELL_BOARD_ID_BASE	 A3700_DDR3_DB_ID
#define ARMAMA3700_DDR3_DB		(ARMAMA3700_MARVELL_BOARD_ID_BASE + 0)
#define ARMAMA3700_DDR4_DB		(ARMAMA3700_MARVELL_BOARD_ID_BASE + 1)
#define ARMAMA3700_MARVELL_MAX_BOARD_ID	(ARMAMA3700_MARVELL_BOARD_ID_BASE + 2)
#define ARMAMA3700_MARVELL_BOARD_NUM	(ARMAMA3700_MARVELL_MAX_BOARD_ID - ARMAMA3700_MARVELL_BOARD_ID_BASE)

#define MARVELL_BOARD_ID_BASE		ARMAMA3700_MARVELL_BOARD_ID_BASE
#define MV_MAX_MARVELL_BOARD_ID		ARMAMA3700_MARVELL_MAX_BOARD_ID
#define MV_MARVELL_BOARD_NUM		ARMAMA3700_MARVELL_BOARD_NUM
#define MV_DEFAULT_BOARD_ID		ARMAMA3700_DDR3_DB

#define BOARD_DEV_TWSI_INIT_EEPROM 0x57
#define MULTI_FDT_EEPROM_ADDR_LEN 2

#define DEFAULT_DDR3_FDT_CONFIG_ID	A3700_DDR3_DB_CONFIG_ID_USB3
#define DEFAULT_DDR4_FDT_CONFIG_ID	A3700_DDR4_DB_CONFIG_ID_USB3

#define DEFAULT_FDT_PER_BOARD { \
	DEFAULT_DDR3_FDT_CONFIG_ID,	/* default fdt_config_id for armada-3700-ddr3-db board */	\
	DEFAULT_DDR4_FDT_CONFIG_ID,	/* default fdt_config_id for armada-3700-ddr4-db board */	\
}
