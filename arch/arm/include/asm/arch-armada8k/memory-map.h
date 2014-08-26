/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

/* Memory Map */
/* 0x00000000 - 0x40000000 - DRAM */
/* 0x40000000 - 0x60000000 - IO-0 */
/* 0x60000000 - 0x61000000 - PEX-0 */
/* 0x61000000 - 0x62000000 - PEX-1 */
/* 0x62000000 - 0x63000000 - NSS */

#ifdef __ASSEMBLY__
#define MVEBU_REGS_BASE		(0xF0000000)
#else
#define MVEBU_REGS_BASE		(0xF0000000UL)
#endif
#define MVEBU_REGS_SIZE 	(0x100000)

#define DRAM_0_BASE_ADDR	0x0
#define DRAM_0_SIZE		0x40000000

#define IO_0_BASE_ADDR		0x40000000
#define IO_0_SIZE		0x20000000

#define PEX_BASE_ADDR(id)	(0x60000000 + (0x1000000 * id))
#define PEX_SIZE(id)		(0x1000000)

#define NSS_BASE_ADDR		(0x62000000)
#define NSS_SIZE		(0x1000000)
