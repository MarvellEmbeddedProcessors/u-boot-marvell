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

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/mvebu.h>

void misc_init_cci400(void)
{
	debug_enter();

	/*
	   CCI-400 enable snoop and dvm on S3 port.
	   For details see the <CoreLink CCI-400 Cache Coherent Interconnect> document.
	   bit[0] - Enable issuing of snoop requests from this slave interface.
	   bit[1] - Enable issuing of DVM message requests from this slave interface
	   bit[29:2] - Reserved
	   bit[30] - Slave interface supports snoops
	   bit[31] - Slave interface supports DVM messages
	 */
	writel(0xC0000003, MVEBU_CCI_S3_SNOOP_CTRL_REG);

	/* Add units configuration code here */

	debug_exit();
}
