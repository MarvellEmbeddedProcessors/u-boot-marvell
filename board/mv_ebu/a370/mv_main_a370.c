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
#include "mvSysHwConfig.h"

int mv_get_arch_number(void)
{
	switch (mvBoardIdGet()) {
	case DB_88F6710_BP_ID:
		return 3038;
		break;
	default:
		return 3038;
		break;
	}
}

/* Set unit in power off mode acording to the detection of MPP/SERDES */
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
void mv_set_power_scheme(void)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_U32 mppGrp1 = mvBoardMppModulesCfgGet(1);
	MV_U32 mppGrp2 = mvBoardMppModulesCfgGet(2);
	MV_U32 srdsCfg = mvBoardSerdesModeGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mv_set_power_scheme: Board unknown.\n");
		return;
	}

	mvOsOutput("Shutting down unused interfaces:\n");

	/* PCI-E */
	if (!(srdsCfg & SRDS_MOD_PCIE0_LANE0)) {
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
		mvOsOutput("       PEX0\n");
	}
	if (!(srdsCfg & SRDS_MOD_PCIE1_LANE1)) {
		mvCtrlPwrClckSet(PEX_UNIT_ID, 1, MV_FALSE);
		mvOsOutput("       PEX1\n");
	}

	/* SATA */
	/* Disable SATA 0 only if SATA 1 is not required as well */
	if (!(srdsCfg & SRDS_MOD_SATA1_LANE3) || (mvCtrlSataMaxPortGet() < 2)) {
		mvCtrlPwrClckSet(SATA_UNIT_ID, 1, MV_FALSE);
		if (mvCtrlSataMaxPortGet() == 2)
			mvOsOutput("       SATA1\n");

		if (!(srdsCfg & (SRDS_MOD_SATA0_LANE0 | SRDS_MOD_SATA0_LANE2))) {
			mvCtrlPwrClckSet(SATA_UNIT_ID, 0, MV_FALSE);
			mvOsOutput("       SATA0\n");
		}
	}

	/* GBE - SMI Bus is connected to EGIGA0 so we only shut down EGIGA1 if needed */
	if (!(srdsCfg & SRDS_MOD_SGMII1_LANE3)) {
		if (!(mppGrp1 & (MV_BOARD_GMII0 | MV_BOARD_RGMII1))) {
			mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, 1, MV_FALSE);
			mvOsOutput("       GBE1\n");
		}
	}

	/* SDIO */
	if (!(mppGrp1 & MV_BOARD_SDIO)) {
		mvCtrlPwrClckSet(SDIO_UNIT_ID, 0, MV_FALSE);
		mvOsOutput("       SDIO\n");
	}

	/* Audio */
	if (!((mppGrp1 & MV_BOARD_I2S) || (mppGrp2 & MV_BOARD_I2S))) {
		mvCtrlPwrClckSet(AUDIO_UNIT_ID, 0, MV_FALSE);
		if (mvCtrlAudioSupport())
			mvOsOutput("       AUDIO\n");
	}
	
	/* TDM */
	if(!((mppGrp1 & MV_BOARD_TDM) || (mppGrp2 & MV_BOARD_TDM)) || !(mvCtrlTdmSupport())) {
		mvCtrlPwrClckSet(TDM_2CH_UNIT_ID, 0, MV_FALSE);
		if (mvCtrlTdmSupport())
			mvOsOutput("       TDM\n");
	}
}
#endif /* defined(MV_INCLUDE_CLK_PWR_CNTRL) */