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

#define DEBUG

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-armadalp/armadalp.h>

int soc_early_init_f(void)
{
	return 0;
}

int soc_get_id(void)
{
	/* TO-DO, get soc ID from PCIe register */
	/* in ArmadaLP, there is no device ID register, like A38x,
	    it needs to be got from PCIe register, like A370 and AXP */
	u32 id = 0x9991;
	return id;
}

struct mvebu_soc_family *soc_init(void)
{
	/* Do early SOC specific init here */

	return &armadalp_family_info;
}
