/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.

*******************************************************************************/

#include <config.h>
#include <common.h>
#include "mvCommon.h"
#include <command.h>
#include "ddr2_3/mvDramIf.h"

int ddr_phy_read_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U16 phyReg;

	mvDdrPhyRegRead(simple_strtoul( argv[1], NULL, 16 ),
	                simple_strtoul( argv[2], NULL, 16), &phyReg);

	printf ("0x%x\n", phyReg);

	return 1;
}

U_BOOT_CMD(
	ddrPhyRead,      3,     3,      ddr_phy_read_cmd,
	"ddrPhyRead - Read DDR PHY register\n",
	"<regNum> <pupNum>\n"
	"\t<regNum> - register number in the PHY\n"
	"\t<pupNum> - PuP number in the PHY\n"

);

int ddr_phy_write_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	mvDdrPhyRegWrite(simple_strtoul( argv[1], NULL, 16 ),
					 simple_strtoul( argv[2], NULL, 16 ),
					 simple_strtoul( argv[3], NULL, 16 ));

	return 1;
}

U_BOOT_CMD(
	ddrPhyWrite,      4,     4,      ddr_phy_write_cmd,
	"ddrPhyWrite - Write DDR PHY register\n",
	"<regNum> <pupNum> <regValue>\n"
	"\t<regNum> - register number in the PHY\n"
	"\t<pupNum> - PuP number in the PHY\n"
	"\t<regValue> - the new register value to write\n"
);
