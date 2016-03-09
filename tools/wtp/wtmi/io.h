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

#ifndef __IO_H
#define __IO_H

#define readw(addr)		(*(volatile u16 *)(addr))
#define readl(addr)		(*(volatile u32 *)(addr))
#define writew(val, addr)	((*(volatile u16 *)(addr)) = (val))
#define writel(val, addr)	((*(volatile u32 *)(addr)) = (val))

#endif /* __IO_H */
