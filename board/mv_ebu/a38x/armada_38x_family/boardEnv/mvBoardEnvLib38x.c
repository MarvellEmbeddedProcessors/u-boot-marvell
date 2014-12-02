/*******************************************************************************
   Copyright (C) Marvell International Ltd. and its affiliates

   This software file (the "File") is owned and distributed by Marvell
   International Ltd. and/or its affiliates ("Marvell") under the following
   alternative licensing terms.  Once you have made an election to distribute the
   File under one of the following license alternatives, please (i) delete this
   introductory statement regarding license alternatives, (ii) delete the two
   license alternatives that you have not elected to use and (iii) preserve the
   Marvell copyright notice above.

********************************************************************************
   Marvell Commercial License Option

   If you received this File from Marvell and you have entered into a commercial
   license agreement (a "Commercial License") with Marvell, the File is licensed
   to you under the terms of the applicable Commercial License.

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
********************************************************************************
   Marvell BSD License Option

   If you received this File from Marvell, you may opt to use, redistribute and/or
   modify this File under the following licensing terms.
   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

*   Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

*   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

*   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "boardEnv/mvBoardEnvLib.h"

/* defines  */
#undef MV_DEBUG
#ifdef MV_DEBUG
#define DB(x)   x
#else
#define DB(x)
#endif

/*******************************************************************************
* mvBoardIsPortInSgmii -
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in SGMII or MV_FALSE
*	For all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in SGMII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInSgmii(MV_U32 ethPortNum)
{
	return mvCtrlPortIsSerdesSgmii(ethPortNum);
}

/*******************************************************************************
* mvBoardIsPortInGmii
*
* DESCRIPTION:
*	This routine returns MV_TRUE for port number works in GMII or MV_FALSE
*	For all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in GMII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInGmii(MV_U32 ethPortNum)
{
	/* If module MII connected return port MII as GMII for NETA init configuration */
	if (mvBoardIsPortInMii(ethPortNum))
		return MV_TRUE;
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsPortInMii
*
* DESCRIPTION:
*	This routine returns MV_TRUE for port number works in MII or MV_FALSE
*	For all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in MII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInMii(MV_U32 ethPortNum)
{
	/* On DB board if MII module detected then port 0 is MII */
	if ((mvBoardIsModuleConnected(MV_MODULE_MII)) && (ethPortNum == 0))
		return MV_TRUE;
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsPortInRgmii
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in RGMII or MV_FALSE
*	For all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in RGMII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInRgmii(MV_U32 ethPortNum)
{
	if (mvBoardIsPortInGmii(ethPortNum) || mvBoardIsPortInSgmii(ethPortNum))
		return MV_FALSE;
	if (ethPortNum < 2)
		return MV_TRUE;
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardDevStateUpdate -
*
* DESCRIPTION:
*	Update Board devices state (active/passive) according to boot source
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardFlashDeviceUpdate(MV_VOID)
{
	MV_BOARD_BOOT_SRC	bootSrc = mvBoardBootDeviceGet();
	MV_U32				numOfDevices;
	MV_U8				devNum;
	MV_U32				devBus;

	/* Assume that the board sctructure sets SPI flash active as the default boot device */
	if (bootSrc == MSAR_0_BOOT_NAND_NEW) {
		/* Activate first NAND device */
		mvBoardSetDevState(0, BOARD_DEV_NAND_FLASH, MV_TRUE);

		/* Deactivate all SPI0 devices */
		numOfDevices = mvBoardGetDevicesNumber(BOARD_DEV_SPI_FLASH);
		for (devNum = 0; devNum < numOfDevices; devNum++) {
			devBus = mvBoardGetDevBusNum(devNum, BOARD_DEV_SPI_FLASH);
			if (devBus == 0)
				mvBoardSetDevState(devNum, BOARD_DEV_SPI_FLASH, MV_FALSE);
		}

	} else if (bootSrc == MSAR_0_BOOT_NOR_FLASH) {

		/* Activate first NOR device */
		mvBoardSetDevState(0, BOARD_DEV_NOR_FLASH, MV_TRUE);

		/* Deactivate all SPI devices */
		numOfDevices = mvBoardGetDevicesNumber(BOARD_DEV_SPI_FLASH);
		for (devNum = 0; devNum < numOfDevices; devNum++)
			mvBoardSetDevState(devNum, BOARD_DEV_SPI_FLASH, MV_FALSE);

		/* Deactivate all NAND devices */
		numOfDevices = mvBoardGetDevicesNumber(BOARD_DEV_NAND_FLASH);
		for (devNum = 0; devNum < numOfDevices; devNum++)
			mvBoardSetDevState(devNum, BOARD_DEV_NAND_FLASH, MV_FALSE);

	}
}

/*******************************************************************************
* mvBoardInfoUpdate - Update Board information structures according to auto-detection.
*
* DESCRIPTION:
*	Update board information according to detection using TWSI bus.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardInfoUpdate(MV_VOID)
{
	MV_U32 reg, sgmiiPhyAddr;

	switch (mvBoardIdGet()) {
	case DB_BP_6821_ID:
		/* need to add similar detection as DB board:
		 * - mvBoardMppIdUpdate();
		 * - check for MV_SATR_DB_SERDES1_CFG, MV_SATR_DB_SERDES2_CFG, MV_SATR_SGMII_MODE */

		mvBoardFlashDeviceUpdate();
		break;
	case DB_GP_68XX_ID:
		mvBoardMppIdUpdate();
		mvBoardIoExpanderUpdate();
		break;
	case RD_NAS_68XX_ID:
	case RD_AP_68XX_ID:
		mvBoardIoExpanderUpdate();
		/* SGMII utilizes in-Band SMI access, no SMI address is used (set -1 to disable MAC SMI polling)*/
		if (mvBoardSatRRead(MV_SATR_RD_SERDES4_CFG) == 1) /* 0 = USB3.  1 = SGMII. */
			mvBoardPhyAddrSet(1, -1);
		break;
	case DB_68XX_ID:
		if ((mvBoardIsModuleConnected(MV_MODULE_MII)))	/* MII Module uses different PHY address */
			mvBoardPhyAddrSet(0, 8);	/*set SMI address 8 for port 0*/

		/* Update MPP group types and values according to board configuration */
		mvBoardMppIdUpdate();
		/* board on test mode  */
		reg = MV_REG_READ(MPP_SAMPLE_AT_RESET) & BIT20;
		if (reg) {
			/* if board on test mode reset MPP19 */
			reg = mvBoardMppGet(2);
			reg &= 0xffff0fff;
			mvBoardMppSet(2, reg);
		}

		/* 'SatR' PCIe modules configuration ('dbserdes1', 'dbserdes2' , 'sgmiimode') */

		sgmiiPhyAddr = mvBoardSatRRead(MV_SATR_SGMII_MODE) ? 0x10 : -1;

		switch (mvBoardSatRRead(MV_SATR_DB_SERDES1_CFG)) {
		case 0x3: /* SGMII port 0 */
			mvBoardPhyAddrSet(0, sgmiiPhyAddr);
			break;
		case 0x4: /* SGMII port 1 */
			mvBoardPhyAddrSet(1, sgmiiPhyAddr);
			break;
		case 0x6: /* QSGMII */
			sgmiiPhyAddr = 0x8; /* 0x8 = SMI address of 1st Quad PHY */
			mvBoardPhyAddrSet(0, sgmiiPhyAddr);
			mvBoardPhyAddrSet(1, sgmiiPhyAddr + 1);
			mvBoardPhyAddrSet(2, sgmiiPhyAddr + 2);
			mvBoardQuadPhyAddr0Set(0, sgmiiPhyAddr);
			mvBoardQuadPhyAddr0Set(1, sgmiiPhyAddr);
			mvBoardQuadPhyAddr0Set(2, sgmiiPhyAddr);
			break;
		}
		if (mvBoardSatRRead(MV_SATR_DB_SERDES2_CFG) == 0x4) /* SGMII port 1 */
			mvBoardPhyAddrSet(1, sgmiiPhyAddr);

		mvBoardFlashDeviceUpdate();
		break;
	default:
		mvOsPrintf("%s: Error: Auto detection update sequence is not supported by current board.\n" , __func__);
	}
}

/*******************************************************************************
* mvBoardIsEthConnected - detect if a certain Ethernet port is active
*
* DESCRIPTION:
*	This routine returns true if a certain Ethernet port is active
*
* INPUT:
*	ethNum - index of the ethernet port requested
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_TRUE if the requested ethernet port is connected.
*
*******************************************************************************/
MV_BOOL mvBoardIsEthConnected(MV_U32 ethNum)
{
	return mvBoardIsGbEPortConnected(ethNum);
}

/*******************************************************************************
* mvBoardMppModuleTypePrint
*
* DESCRIPTION:
*	Print on-board detected modules.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardMppModuleTypePrint(MV_VOID)
{
	int i;
	char *moduleStr[MV_MODULE_TYPE_MAX_MODULE] = { \
		"MII",                                  \
		"TDM",                                  \
		"AUDIO I2S",                     \
		"AUDIO SPDIF",                   \
		"NOR 16bit",                                  \
		"NAND 16bit",                                 \
		"SDIO 4bit",                                 \
		"SGMII",                                 \
	};
	mvOsOutput("Board configuration detected:\n");

	for (i = 0; i < MV_MODULE_TYPE_MAX_MODULE; i++) {
		if (mvBoardIsModuleConnected(1 << i))
			mvOsOutput("       %s module.\n", moduleStr[i]);

	}
}
