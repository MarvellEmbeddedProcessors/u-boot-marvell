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

#ifdef __ASSEMBLY__
#define MVEBU_CP0_REGS_BASE	(0xF2000000)
#else
#define MVEBU_CP0_REGS_BASE	(0xF2000000UL)
#endif

#define DRAM_0_BASE_ADDR	0x0
#define DRAM_0_SIZE		0x40000000

#define IO_0_BASE_ADDR		0x40000000
#define IO_0_SIZE		0x20000000

#define PEX_BASE_ADDR(id)	(0x60000000 + (0x1000000 * id))
#define PEX_SIZE(id)		(0x1000000)

#define NSS_BASE_ADDR		(0x62000000)
#define NSS_SIZE		(0x1000000)
