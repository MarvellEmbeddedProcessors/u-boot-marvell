/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
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
#include "mvCommon.h"
#include "mvTypes.h"
#include "mvBoardEnvLib.h"
#include "mvCtrlEnvLib.h"
#include "mvSysHwConfig.h"
#include <ahci.h>


#ifdef CONFIG_SCSI_6820
static int boardIntegratedSataInit = -1;
void board_ahci_init(void)
{
	int sataPort = 0;
	if(boardIntegratedSataInit == 1)
		return;
	boardIntegratedSataInit = 1;
	printf ("ahci init: ");
	for(sataPort = 0; sataPort < mvCtrlSataMaxPortGet(); sataPort++) {
		printf ("port%d ", sataPort);
		if (0 != ahci_init(INTER_REGS_BASE | MV_SATA3_REGS_OFFSET(sataPort)))
			printf("AHCI init failed for port%d!\n", sataPort);
	}
	printf ("\n");
}
#endif
