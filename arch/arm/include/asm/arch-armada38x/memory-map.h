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

/* Memory Map */
/* 0x00000000 - 0xE0000000 - DRAM */
/* 0xE0000000 - 0xE2000000 - PCI-0 */
/* 0xE2000000 - 0xE4000000 - PCI-1 */
/* 0xE4000000 - 0xE6000000 - PCI-2 */
/* 0xE6000000 - 0xE8000000 - PCI-3 */
/* 0xF1000000 - 0xF1100000 - INTERNAL_REG */
/* 0xFD000000 - 0xFD100000 - NOR_CS */

#ifndef CONFIG_SPL_BUILD
#define MVEBU_REGS_BASE			(0xF1000000)
#else
#define MVEBU_REGS_BASE                 (0xD0000000)
#endif
#define MVEBU_REGS_SIZE			(0x100000)

/* Memory windows settings */
#define MVEBU_PCIE_MEM_BASE(id)		(0xE0000000 + (id * 0x2000000))
#define MVEBU_PCIE_MEM_SIZE(id)		(0x2000000)

#define NOR_CS_BASE				(0xfd000000)
#define NOR_CS_SIZE				(0x1000000)
