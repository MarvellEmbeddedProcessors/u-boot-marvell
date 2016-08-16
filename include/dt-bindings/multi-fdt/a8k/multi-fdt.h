/*
 * ***************************************************************************
 * Copyright (C) 2016 Marvell International Ltd.
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

#ifndef _MULTI_FDT_H_
#define _MULTI_FDT_H_

#define A8K_MARVELL_BOARD_ID_BASE			0x10
#define A8K_APN_806_DB					(A8K_MARVELL_BOARD_ID_BASE + 0)
#define A8K_7040_RZ					(A8K_MARVELL_BOARD_ID_BASE + 1)
#define A8K_7040_DB					(A8K_MARVELL_BOARD_ID_BASE + 2)
#define A8K_8040_DB					(A8K_MARVELL_BOARD_ID_BASE + 3)
#define A8K_7040_PCAC					(A8K_MARVELL_BOARD_ID_BASE + 4)
#define A8K_MARVELL_MAX_BOARD_ID			(A8K_MARVELL_BOARD_ID_BASE + 5)
#define A8K_MARVELL_BOARD_NUM				(A8K_MARVELL_MAX_BOARD_ID - A8K_MARVELL_BOARD_ID_BASE)

#define MARVELL_BOARD_ID_BASE				A8K_MARVELL_BOARD_ID_BASE
#define MV_MAX_MARVELL_BOARD_ID				A8K_MARVELL_MAX_BOARD_ID
#define MV_MARVELL_BOARD_NUM				A8K_MARVELL_BOARD_NUM
#define A8K_A7K_DEFAULT_BOARD_ID			A8K_MARVELL_MAX_BOARD_ID
#define MV_DEFAULT_BOARD_ID				A8K_A7K_DEFAULT_BOARD_ID
#define FIRST_SUPPORT_BOARD_ID				A8K_7040_DB

#define BOARD_DEV_TWSI_INIT_EEPROM 0x57
#define MULTI_FDT_EEPROM_ADDR_LEN 2

/* a minimal fdt, only to load u-boot for configuring board id on the a8k & a7k boards */
#define A8K_A7K_default		1

/*
 * There are main presets for Armada70x0 DB:
 * The specification of any preset found in doc/mvebu/a7k-a8k/armada70x0-db-setup.txt
 */

#define A70X0_ROUTER		1
#define A70X0_CUSTOMER		2
#define A70X0_KR		3
#define A70X0_ALL_PCIE		4
#define A70X0_NAND		5
#define A70X0_RGMII_ONLY	6

/*
 * There are main presets for Armada80x0 DB:
 * The specification of any preset found in doc/mvebu/a7k-a8k/
 */
#define A80X0_10G		1
#define A80X0_SGMII		2
#define A80X0_CP1_SERDESB	3

/*
 * There are main presets for Armada70x0 PCAC:
 * The specification of any preset found in doc/mvebu/a7k-a8k/
 */
#define A70X0_PCAC_KR		1

/* default FDT selection for each board */
#define DEFAULT_FDT_PER_BOARD { \
				0x04,			/* APN_806_DB	*/ \
				0x01,			/* 7040_RZ_DB	*/ \
				A70X0_ROUTER,		/* 7040_DB	*/ \
				A80X0_10G,		/* 8040_DB	*/ \
				A70X0_PCAC_KR,		/* 7040_PCAC	*/ \
				A8K_A7K_default,	/* DEFAULT BOARD_ID, This must be the last value*/ \
}

#endif /* _MULTI_FDT_H_ */
