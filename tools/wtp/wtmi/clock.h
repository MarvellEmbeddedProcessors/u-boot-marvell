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

#ifndef __CLOCK_H
#define __CLOCK_H

#define MVEBU_XTAL_MODE_MASK		BIT9
#define MVEBU_XTAL_MODE_OFFS		(9)
#define MVEBU_XTAL_CLOCK_25MHZ		(0x0)
#define MVEBU_XTAL_CLOCK_40MHZ		(0x1)

enum clock_src {
	TBG_A,
	TBG_B
};

enum clock_line {
	TBG_A_P = 0,
	TBG_B_P = 1,
	TBG_A_S = 2,
	TBG_B_S = 3
};


u32 clock_init(void);
u32 get_ref_clk(void);
u32 get_cm3_clk(void);
void wait_ns(u32 wait_ns);

#endif /* __CLOCK_H */
