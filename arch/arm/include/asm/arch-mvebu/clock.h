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

#ifndef _MVEBU_CLOCK_H_
#define _MVEBU_CLOCK_H_

#define KHz			1000
#define MHz			1000000
#define GHz			1000000000

u32 soc_tclk_get(void);
u32 soc_l2_clk_get(void);
u32 soc_cpu_clk_get(void);
u32 soc_ddr_clk_get(void);
u32 soc_timer_clk_get(void);

#ifdef CONFIG_NAND_PXA3XX
unsigned long mvebu_get_nand_clock(void);
#endif

u32 soc_clock_get(const void *blob, int node);
u32 get_fdt_tclk(const void *blob, int node);
void soc_print_clock_info(void);

#endif /* _MVEBU_CLOCK_H_ */
