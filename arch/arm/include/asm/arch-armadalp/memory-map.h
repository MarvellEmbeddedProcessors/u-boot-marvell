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
/* 0x00000000 - 0x7FFFFFFF - DRAM */
/* 0x80000000 - 0x8FFFFFFF - IHB-PCIe */
/* 0xD0000000 - 0xD0300000 - Internal Reg */
/* 0xD0700000 - 0xD070FFFF - CCI */
/* 0xE0000000 - 0xE7FFFFFF - IHB */
/* 0xE8000000 - 0xEFFFFFFF - PCIe */
/* 0xF0000000 - 0xF001FFFF - rWTM */
/* 0xFFF00000 - 0xFFFFFFFF - Boot Room */

#ifdef __ASSEMBLY__
#define MVEBU_REGS_BASE		(0xD0000000)
#else
#define MVEBU_REGS_BASE		(0xD0000000UL)
#endif
#define MVEBU_REGS_SIZE		(0x300000)


