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

#include <asm/io.h>
#include <common.h>
#include <asm/arch-armada8k/regs-base.h>

#define LLC_CTRL                       0x100
#define LLC_CACHE_SYNC                 0x700
#define L2X0_INV_WAY                    0x77C
#define L2X0_CLEAN_WAY                 0x7BC
#define L2X0_CLEAN_INV_WAY             0x7FC

#define LLC_CTRL_EN	                1
#define LLC_WAY_MASK				0xFFFFFFFF

/* insert all LLC function to secure section - part of the PSCI FW */
void llc_cache_sync(void) __attribute__ ((section (".secure_text")));
void llc_flush_all(void) __attribute__ ((section (".secure_text")));
void llc_clean_all(void) __attribute__ ((section (".secure_text")));
void llc_inv_all(void) __attribute__ ((section (".secure_text")));
void llc_disable(void) __attribute__ ((section (".secure_text")));
void llc_enable(void) __attribute__ ((section (".secure_text")));
void llc_save(void) __attribute__ ((section (".secure_text")));
void llc_resume(void) __attribute__ ((section (".secure_text")));

void llc_cache_sync(void)
{
	__raw_writel(0, MVEBU_LLC_BASE + LLC_CACHE_SYNC);
	/* Atumic write no need to wait */
}

void llc_flush_all(void)
{
	__raw_writel(LLC_WAY_MASK, MVEBU_LLC_BASE + L2X0_CLEAN_INV_WAY);
	llc_cache_sync();
}

void llc_clean_all(void)
{
	__raw_writel(LLC_WAY_MASK, MVEBU_LLC_BASE + L2X0_CLEAN_WAY);
	llc_cache_sync();
}

void llc_inv_all(void)
{
	/* Invalidating when L2 is enabled is a nono */
	BUG_ON(readl(MVEBU_LLC_BASE + LLC_CTRL) & LLC_CTRL_EN);
	__raw_writel(LLC_WAY_MASK, MVEBU_LLC_BASE + L2X0_INV_WAY);
	llc_cache_sync();
}

void llc_disable(void)
{
	llc_flush_all();
	__raw_writel(0, MVEBU_LLC_BASE + LLC_CTRL);
	asm volatile("dsb st");
}

void llc_enable(void)
{
	llc_inv_all();
	__raw_writel(LLC_CTRL_EN, MVEBU_LLC_BASE + LLC_CTRL);
}


void llc_save(void)
{
	/* TBD */
}

void llc_resume(void)
{
	/* TBD */
}
