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
/* 0x00000000 - 0xE0000000 - DRAM */
/* 0xE0000000 - 0xE2000000 - PCI-0 */
/* 0xE2000000 - 0xE4000000 - PCI-1 */
/* 0xE4000000 - 0xE6000000 - PCI-2 */
/* 0xE6000000 - 0xE8000000 - PCI-3 */
/* 0xF1000000 - 0xF1100000 - INTERNAL_REG */
/* 0xFD000000 - 0xFD100000 - NOR_CS */

#define MVEBU_REGS_BASE			(0xF1000000)
#define MVEBU_REGS_SIZE			(0x100000)

#define PEX_MEM_BASE(id)		(0xE0000000 + (id * 0x2000000))
#define PEX_MEM_SIZE(id)		(0x2000000)

#define NOR_CS_BASE				(0xfd000000)
#define NOR_CS_SIZE				(0x1000000)
