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
* mvBoardPortTypeGet
*
* DESCRIPTION:
*       This routine returns port type
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None
*
* RETURN:
*       Mode of the port
*
*******************************************************************************/
MV_U32 mvBoardPortTypeGet(MV_U32 ethPortNum)
{
	if (mvBoardIsPortInSgmii(ethPortNum))
		return MV_PORT_TYPE_SGMII;
	if (mvBoardIsPortInRgmii(ethPortNum))
		return MV_PORT_TYPE_RGMII;
	if (mvBoardIsPortInRxaui(ethPortNum))
		return MV_PORT_TYPE_RXAUI;
	if (mvBoardIsPortInXaui(ethPortNum))
		return MV_PORT_TYPE_XAUI;
	if (mvBoardIsPortInQsgmii(ethPortNum))
		return MV_PORT_TYPE_QSGMII;
	return MV_PORT_TYPE_UNKNOWN;
}

/*******************************************************************************
* mvBoardIsPortInGmii - No Gmii interface for Armada 39x
*******************************************************************************/
MV_BOOL mvBoardIsPortInGmii(MV_U32 ethPortNum)
{
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsPortInMii - No Mii interface for Armada 39x
*******************************************************************************/
MV_BOOL mvBoardIsPortInMii(MV_U32 ethPortNum)
{
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
	/* only port number 1 can work in RGMII */
	if (ethPortNum != 1)
		return MV_FALSE;
	if (mvBoardIsPortInQsgmii(ethPortNum) || mvBoardIsPortInSgmii(ethPortNum))
		return MV_FALSE;
	return MV_TRUE;
}

/*******************************************************************************
* mvBoardIsPortInRxaui -
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in RXAUI or MV_FALSE
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
MV_BOOL mvBoardIsPortInRxaui(MV_U32 ethPortNum)
{
	MV_U32 netComplex;
	netComplex = mvBoardNetComplexConfigGet();
	/* only port number 0 can work in RXAUI */
	if (ethPortNum != 0)
		return MV_FALSE;
	if (netComplex & (MV_NETCOMP_GE_MAC0_2_RXAUI))
		return MV_TRUE;
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsPortInXaui -
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in RXAUI or MV_FALSE
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
MV_BOOL mvBoardIsPortInXaui(MV_U32 ethPortNum)
{
	MV_U32 netComplex;
	netComplex = mvBoardNetComplexConfigGet();
	/* only port number 0 can work in XAUI */
	if (ethPortNum != 0)
		return MV_FALSE;
	if (netComplex & (MV_NETCOMP_GE_MAC0_2_XAUI))
		return MV_TRUE;
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardIsPortInQsgmii-
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in QSGMII or MV_FALSE
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
MV_BOOL mvBoardIsPortInQsgmii(MV_U32 ethPortNum)
{
	MV_U32 netComplex;
	netComplex = mvBoardNetComplexConfigGet();
	switch (ethPortNum) {
	case 0:
		if (netComplex & (MV_NETCOMP_GE_MAC0_2_QSGMII))
			return MV_TRUE;
		return MV_FALSE;
	case 1:
		if (netComplex & MV_NETCOMP_GE_MAC1_2_QSGMII)
			return MV_TRUE;
		return MV_FALSE;
	case 2:
		if (netComplex & MV_NETCOMP_GE_MAC2_2_QSGMII)
			return MV_TRUE;
		return MV_FALSE;
	case 3:
		if (netComplex & MV_NETCOMP_GE_MAC3_2_QSGMII)
			return MV_TRUE;
		return MV_FALSE;
	}
	return MV_FALSE;
}

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
	MV_U32 netComplex;
	netComplex = mvBoardNetComplexConfigGet();
	switch (ethPortNum) {
	case 0:
		if (netComplex & (MV_NETCOMP_GE_MAC0_2_SGMII_L0 | MV_NETCOMP_GE_MAC0_2_SGMII_L1 |
					MV_NETCOMP_GE_MAC0_2_SGMII_L6))
			return MV_TRUE;
		break;
	case 1:
		if (netComplex & (MV_NETCOMP_GE_MAC1_2_SGMII_L1 | MV_NETCOMP_GE_MAC1_2_SGMII_L2 |
					MV_NETCOMP_GE_MAC1_2_SGMII_L4))
			return MV_TRUE;
		break;
	case 2:
		if (netComplex & (MV_NETCOMP_GE_MAC2_2_SGMII_L3 | MV_NETCOMP_GE_MAC2_2_SGMII_L5))
			return MV_TRUE;
		break;
	case 3:
		if (netComplex & (MV_NETCOMP_GE_MAC3_2_SGMII_L4 | MV_NETCOMP_GE_MAC3_2_SGMII_L6))
			return MV_TRUE;
		break;
	}
	/*if QSGMII module connected then every single MAC port is SGMII port*/
	return mvBoardIsPortInQsgmii(ethPortNum);
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
#ifdef CONFIG_CMD_BOARDCFG
	MV_U32 smiAddress = -1, boardCfg, gpConfig;
	MV_U32 smiQuadAddr = 0x8;
	MV_U32 netComplexOptions = 0x0;
	MV_U32 defaultNetComplex = (MV_NETCOMP_GE_MAC0_2_RXAUI | MV_NETCOMP_GE_MAC1_2_SGMII_L4);
	/*default value for serdes5/6 mode is 1(XSMI) because that lane5/6
		by default connected to RXUAI with XSMI mode*/
	MV_U32 serdes5Mode = 1, serdes6Mode = 1;
	MV_BOARD_INFO *board = mvBoardInfoStructureGet();
	const char *boardNameArr[4] = {"DB-88F6925-GP-EAP-10G", "DB-88F6925-GP-EAP-1G",
				"DB-88F6925-GP-HGW-2.5G", "DB-88F6925-GP-HGW-2.5-mSATA"};

	switch (mvBoardIdGet()) {
	case A39X_RD_69XX_ID:
		gpConfig = mvBoardSysConfigGet(MV_CONFIG_GP_CONFIG);
		/*each case describes one configuration for db-gp 395 board*/
		switch (gpConfig) {
		case MV_GP_CONFIG_EAP_10G:
			/*option 0: MAC0=>SerDeses 5+6 using RAXUAI
				       MAC1=>SerDes 4 using switch*/
			mvBoardSdioConnectionSet(MV_FALSE);
			mvBoardTdmConnectionSet(MV_TRUE);
			mvBoardSetDevState(0, BOARD_DEV_SPI_FLASH, MV_TRUE);
			netComplexOptions |= defaultNetComplex;
			break;
		case MV_GP_CONFIG_EAP_1G:
			/*option 1: MAC0=>SerDeses 6 using SGMII
					MAC1=>SerDes 4 using switch*/
			mvBoardSdioConnectionSet(MV_FALSE);
			netComplexOptions |= (MV_NETCOMP_GE_MAC0_2_SGMII_L6 | MV_NETCOMP_GE_MAC1_2_SGMII_L4);
			mvBoardAudioConnectionSet(MV_TRUE); /*Enabled for Marvell 88CE156 I2S/TDM Codec*/
			break;
		case MV_GP_CONFIG_HGW_AP_2_5G_SATA:
			/* one of the uses of this configuration is to use the board as AMC board.
			   nothing will happen if the kernel not detect any switch devices over pex*/
			mvBoardSetisAmc(MV_TRUE);
		case MV_GP_CONFIG_HGW_AP_2_5G:
			/*options 2+3: MAC0=>SerDes 6 using SFP or 2.5G PHY on modules
				       MAC2=>SerDes 5 using switch on board*/
			netComplexOptions |= (MV_NETCOMP_GE_MAC0_2_SGMII_L6 |
				MV_NETCOMP_GE_MAC2_2_SGMII_L5);
			mvBoardMacStatusSet(2, MV_TRUE);
			mvBoardMacStatusSet(1, MV_FALSE);
			mvBoardPhyAddrSet(0, -1);
			mvBoardQuadPhyAddr0Set(0, -1);
			mvBoardPhyNegotiationTypeSet(0, SMI);
			break;
		default:
			mvOsPrintf("%s: Error: Invalid GP config value," , __func__);
			mvOsPrintf("Update netComplexOptions with default value.\n");
			gpConfig = 0;
			netComplexOptions |= defaultNetComplex;
		}
		strcpy(board->boardName, boardNameArr[gpConfig]);
		mvBoardNetComplexConfigSet(netComplexOptions);
		mvBoardIoExpanderUpdate();
		mvBoardMppIdUpdate();
		break;
	case A39X_DB_69XX_ID:
		mvBoardNetComplexInfoUpdate();
		/* Enable USB3.0 port 0 if  SerDes is connected to USB Host #0 */
		/* COMPHY1: 6 = USB_HOST#0 (connected via module) */
		boardCfg = mvBoardSysConfigGet(MV_CONFIG_LANE1);
		if (boardCfg == 0x6)
			mvBoardUsbPortStatusSet(USB3_UNIT_ID, 0, MV_TRUE);
		/* COMPHY4: 4 = USB_HOST#0 (connected via module) */
		boardCfg = mvBoardSysConfigGet(MV_CONFIG_LANE4);
		if (boardCfg == 0x4)
			mvBoardUsbPortStatusSet(USB3_UNIT_ID, 0, MV_TRUE);
		boardCfg = mvBoardSysConfigGet(MV_CONFIG_5_SMI_MODE);
		if (!boardCfg)
			serdes5Mode = 0;
		boardCfg = mvBoardSysConfigGet(MV_CONFIG_6_SMI_MODE);
		if (!boardCfg)
			serdes6Mode = 0;
		mvBoardMppIdUpdate();

		boardCfg = mvBoardNetComplexConfigGet();
		if (boardCfg & MV_NETCOMP_GE_MAC0_2_SGMII_L1) {
			smiAddress = 0x5;
			mvBoardPhyNegotiationTypeSet(0, SMI);
		} else if (boardCfg & (MV_NETCOMP_GE_MAC0_2_RXAUI | MV_NETCOMP_GE_MAC0_2_XAUI)) {
			smiAddress = 0x0;
			mvBoardPhyNegotiationTypeSet(0, XSMI);
			/*check if MAC0 connected to SGMII on lane0/6 via module*/
		} else if (boardCfg & MV_NETCOMP_GE_MAC0_2_SGMII_L0) {
			smiAddress = 0x10;
			mvBoardPhyNegotiationTypeSet(0, SMI);
		} else if (boardCfg & MV_NETCOMP_GE_MAC0_2_SGMII_L6) {
			if (serdes6Mode) {
				smiAddress = 0x0;
				mvBoardPhyNegotiationTypeSet(0, XSMI);
			} else {
				smiAddress = 0x10;
				mvBoardPhyNegotiationTypeSet(0, SMI);
			}
		} else if (boardCfg & MV_NETCOMP_GE_MAC0_2_QSGMII) {
			smiAddress = smiQuadAddr;
			mvBoardPhyNegotiationTypeSet(0, SMI);
			mvBoardQuadPhyAddr0Set(0, smiQuadAddr);
		}
		mvBoardPhyAddrSet(0, smiAddress);

		smiAddress = -1;
		if (boardCfg & MV_NETCOMP_GE_MAC1_2_SGMII_L1)
			smiAddress = 0x5;
		else if (boardCfg & (MV_NETCOMP_GE_MAC1_2_RGMII1))
			smiAddress = 0x1;
			/*check if MAC1 connected to SGMII on lane2/4 via module*/
		else if (boardCfg & (MV_NETCOMP_GE_MAC1_2_SGMII_L2 | MV_NETCOMP_GE_MAC1_2_SGMII_L4))
			smiAddress = 0x11;
		else if (boardCfg & MV_NETCOMP_GE_MAC1_2_QSGMII) {
			smiAddress = smiQuadAddr + 1;
			mvBoardQuadPhyAddr0Set(1, smiQuadAddr);
		}
		mvBoardPhyAddrSet(1, smiAddress);

		smiAddress = -1;

		if (boardCfg & MV_NETCOMP_GE_MAC2_2_SGMII_L3) {
			smiAddress = 0x4;
			mvBoardPhyNegotiationTypeSet(2, SMI);
			/*check if MAC2 connected to SGMII on lane5 via module*/
		} else if (boardCfg & MV_NETCOMP_GE_MAC2_2_SGMII_L5) {
			if (serdes5Mode) {
				smiAddress = 0x0;
				mvBoardPhyNegotiationTypeSet(2, XSMI);
			} else {
				smiAddress = 0x12;
				mvBoardPhyNegotiationTypeSet(2, SMI);
			}
		} else if (boardCfg & MV_NETCOMP_GE_MAC2_2_QSGMII) {
			smiAddress = smiQuadAddr + 2;
			mvBoardQuadPhyAddr0Set(2, smiQuadAddr);
			mvBoardPhyNegotiationTypeSet(2, SMI);
		}
		mvBoardPhyAddrSet(2, smiAddress);

		smiAddress = -1;
		if (boardCfg & (MV_NETCOMP_GE_MAC3_2_SGMII_L6)) {
			/*
			 * there is another option the is not supported anymore
			 * which is to connect a bridge from the PCI to one of the
			 * normal phys which means that mac3 would be connected with
			 * lane 6 with smiAddress = 0x6
			 * changes that has been done:
			 * smiAddress = 0x6; --> smiAddress = 0x0;
			 * now when mac3 is connected with lane 6 with sgmii(Mode XSMI)
			 * it means that it's through the 10 gig phy
			 */
			if (serdes6Mode) {
				smiAddress = 0x0;
				mvBoardPhyNegotiationTypeSet(3, XSMI);
			} else {
				/*check if mac3 connected with lane 6 with sgmii(Mode SMI)
					then it's through external module*/
				smiAddress = 0x13;
				mvBoardPhyNegotiationTypeSet(3, SMI);
			}
			/*check if MAC3 connected to SGMII on lane4 via module*/
		} else if (boardCfg & MV_NETCOMP_GE_MAC3_2_SGMII_L4) {
			smiAddress = 0x13;
			mvBoardPhyNegotiationTypeSet(3, SMI);
		} else if (boardCfg & MV_NETCOMP_GE_MAC3_2_QSGMII) {
			smiAddress = smiQuadAddr + 3;
			mvBoardPhyNegotiationTypeSet(3, SMI);
			mvBoardQuadPhyAddr0Set(3, smiQuadAddr);
		}
		mvBoardPhyAddrSet(3, smiAddress);

		mvBoardFlashDeviceUpdate();
		break;
	case A39x_DB_AMC_69XX_ID:
		/* nothing to be updated at run-time for AMC board */
		break;
	default:
		mvOsPrintf("%s: Error: Auto detection update sequence is not supported by current board.\n" , __func__);
	}
#endif /* CONFIG_CMD_BOARDCFG */
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
	MV_U32 netComplex = mvBoardNetComplexConfigGet();
	MV_BOOL isConnected = MV_FALSE;
	switch (ethNum) {
	case 0:
		if (netComplex & (MV_NETCOMP_GE_MAC0_2_RXAUI | MV_NETCOMP_GE_MAC0_2_XAUI |
					MV_NETCOMP_GE_MAC0_2_SGMII_L0 | MV_NETCOMP_GE_MAC0_2_SGMII_L1 |
					MV_NETCOMP_GE_MAC0_2_QSGMII | MV_NETCOMP_GE_MAC0_2_SGMII_L6))
			isConnected = MV_TRUE;
		break;
	case 1:
		if (netComplex & (MV_NETCOMP_GE_MAC1_2_SGMII_L1 | MV_NETCOMP_GE_MAC1_2_RGMII1 |
					MV_NETCOMP_GE_MAC1_2_SGMII_L2 | MV_NETCOMP_GE_MAC1_2_SGMII_L4 |
					MV_NETCOMP_GE_MAC1_2_QSGMII))
			isConnected = MV_TRUE;
		break;
	case 2:
		if (netComplex & (MV_NETCOMP_GE_MAC2_2_SGMII_L3 | MV_NETCOMP_GE_MAC2_2_SGMII_L5 |
					MV_NETCOMP_GE_MAC2_2_QSGMII))
			isConnected = MV_TRUE;
		break;
	case 3:
		if (netComplex & (MV_NETCOMP_GE_MAC3_2_SGMII_L4 | MV_NETCOMP_GE_MAC3_2_SGMII_L6 |
					MV_NETCOMP_GE_MAC3_2_QSGMII))
			isConnected = MV_TRUE;
		break;
	}
	return isConnected;
}

/*******************************************************************************
* mvBoardNetComplexInfoUpdate
*
* DESCRIPTION:
*	Update etherntComplex configuration,
*	according to modules detection (S@R & board configuration)
*
** INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK - on success,
*	MV_ERROR - On failure.
*
*******************************************************************************/
#ifdef CONFIG_CMD_BOARDCFG
MV_STATUS mvBoardNetComplexInfoUpdate(MV_VOID)
{
	MV_U32 netComplexOptions = 0x0;
	MV_U32 sysConfig, sysConfig3, sysConfig4, sysConfig5, sysConfig6;

	sysConfig = mvBoardSysConfigGet(MV_CONFIG_LANE0);
	/* COMPHY0: 3 = GBE port 0, 4 = GBE v3 port 0 */
	if (sysConfig == 0x3 || sysConfig == 0x4)
		netComplexOptions |= MV_NETCOMP_GE_MAC0_2_SGMII_L0;

	sysConfig = mvBoardSysConfigGet(MV_CONFIG_LANE1);
	/* COMPHY1: 4 = GBE port 0, 8 = GBE v3 port 0 */
	if (sysConfig == 0x4 || sysConfig == 0x8)
		netComplexOptions |= MV_NETCOMP_GE_MAC0_2_SGMII_L1;
	/* COMPHY1: 5 = GBE port 1, 9 = GBE v3 port 1 */
	else if (sysConfig == 0x5 || sysConfig == 0x9)
		netComplexOptions |= MV_NETCOMP_GE_MAC1_2_SGMII_L1;
	/* COMPHY1: 7 = QSGMII */
	else if (sysConfig == 0x7) {
		netComplexOptions |= MV_NETCOMP_GE_MAC0_2_QSGMII;
		netComplexOptions |= MV_NETCOMP_GE_MAC1_2_QSGMII;
		netComplexOptions |= MV_NETCOMP_GE_MAC2_2_QSGMII;
		netComplexOptions |= MV_NETCOMP_GE_MAC3_2_QSGMII;
	}

	sysConfig = mvBoardSysConfigGet(MV_CONFIG_LANE2);
	/* COMPHY2: 4 = GBE port 1, 5 = GBE v3 port 1 */
	if (sysConfig == 0x4 || sysConfig == 0x5)
		netComplexOptions |= MV_NETCOMP_GE_MAC1_2_SGMII_L2;

	sysConfig3 = mvBoardSysConfigGet(MV_CONFIG_LANE3);
	/* COMPHY3: 4 = GBE port 2, 7 = GBE v3 port 2 */
	if (sysConfig3 == 0x4 || sysConfig3 == 0x7)
		netComplexOptions |= MV_NETCOMP_GE_MAC2_2_SGMII_L3;

	sysConfig4 = mvBoardSysConfigGet(MV_CONFIG_LANE4);
	/* COMPHY4: 3 = GBE port 1, 8 = GBE v3 port 3 */
	if (sysConfig4 == 0x3)
		netComplexOptions |= MV_NETCOMP_GE_MAC1_2_SGMII_L4;
	else if (sysConfig4 == 0x8)
		netComplexOptions |= MV_NETCOMP_GE_MAC3_2_SGMII_L4;

	sysConfig5 = mvBoardSysConfigGet(MV_CONFIG_LANE5);
	/* COMPHY5: 3 = GBE port 2, 6 = GBE v3 port 2 */
	if (sysConfig5 == 0x3 || sysConfig5 == 0x6)
		netComplexOptions |= MV_NETCOMP_GE_MAC2_2_SGMII_L5;

	sysConfig6 = mvBoardSysConfigGet(MV_CONFIG_LANE6);
	/* COMPHY6: 2 = GBE v3 port 3 */
	if (sysConfig6 == 0x2)
		netComplexOptions |= MV_NETCOMP_GE_MAC3_2_SGMII_L6;
	/* COMPHY6: 0xC = GBE v3 port 0 */
	else if (sysConfig6 == 0xc)
		netComplexOptions |= MV_NETCOMP_GE_MAC0_2_SGMII_L6;

	/* COMPHY3: 8 = XAUI, COMPHY4: 9 = XAUI, COMPHY5: 8 = XAUI, COMPHY6: 4 = XAUI */
	if (sysConfig3 == 0x8 && sysConfig4 == 0x9 && sysConfig5 == 0x8 && sysConfig6 == 0x4)
		netComplexOptions |= MV_NETCOMP_GE_MAC0_2_XAUI;
	/* COMPHY5: 8 = RXAUI, COMPHY6: 4 = RXAUI */
	else if (sysConfig5 == 0x8 && sysConfig6 == 0x4)
		netComplexOptions |= MV_NETCOMP_GE_MAC0_2_RXAUI;

	/* If MAC1 is not connected to SGMII and no conflicts of RGMII1(vs NAND), connect MAC1 to RGMII1 */
	if ((!(netComplexOptions & (MV_NETCOMP_GE_MAC1_2_SGMII_L4 | MV_NETCOMP_GE_MAC1_2_SGMII_L2 |
					MV_NETCOMP_GE_MAC1_2_SGMII_L1 | MV_NETCOMP_GE_MAC1_2_QSGMII)))
					&& mvCtrlPortIsRgmii(1))
		netComplexOptions |= MV_NETCOMP_GE_MAC1_2_RGMII1;

	mvBoardNetComplexConfigSet(netComplexOptions);
	return MV_OK;
}
#endif /* CONFIG_CMD_BOARDCFG */

/*******************************************************************************
* mvBoardPhyTypeGet - Get the phy negtiation type ( SMI / XSMI )
*
* DESCRIPTION:
*       This routine returns the Phy negotiation type of a given ethernet port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing Phy negotiation type, -1 if the port number is wrong.
*
*******************************************************************************/
MV_PHY_NEGOTIATION_PORT_TYPE mvBoardPhyNegotiationTypeGet(MV_U32 ethPortNum)
{
	MV_BOARD_INFO *board = mvBoardInfoStructureGet();
	if (!board || ethPortNum >= board->numBoardMacInfo) {
		DB(mvOsPrintf("%s: Error: invalid ethPortNum (%d)\n", __func__, ethPortNum));
		return MV_ERROR;
	}

	return board->pBoardMacInfo[ethPortNum].negType;
}
/*******************************************************************************
* mvBoardPhyNegotiationTypeSet - Set the phy type
*
* DESCRIPTION:
*	This routine sets the Phy type of a given ethernet port.
*
* INPUT:
*	ethPortNum - Ethernet port number.
*	negType    - requested phy type
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardPhyNegotiationTypeSet(MV_U32 ethPortNum, MV_PHY_NEGOTIATION_PORT_TYPE negType)
{
	MV_BOARD_INFO *board = mvBoardInfoStructureGet();
	if (!board || ethPortNum >= board->numBoardMacInfo) {
		DB(mvOsPrintf("%s: Error: invalid ethPortNum (%d)\n", __func__, ethPortNum));
		return;
	}
	board->pBoardMacInfo[ethPortNum].negType = negType;
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
	int port;
	mvOsOutput("Board configuration:\n");
	mvOsOutput("|  port  | Interface  | PHY address  |\n");
	mvOsOutput("|--------|------------|--------------|\n");
	for (port = 0; port < mvCtrlEthMaxPortGet(); port++) {
		if (MV_FALSE ==  mvBoardIsGbEPortConnected(port)) /* verify port is active */
			continue;
		mvOsOutput("| egiga%d ", port);
		switch (mvBoardPortTypeGet(port)) {
		case MV_PORT_TYPE_SGMII:
			mvOsOutput("|   SGMII    ");
			break;
		case MV_PORT_TYPE_RGMII:
			mvOsOutput("|   RGMII    ");
			break;
		case MV_PORT_TYPE_RXAUI:
			mvOsOutput("|   RXAUI    ");
			break;
		case MV_PORT_TYPE_XAUI:
			mvOsOutput("|   XAUI     ");
			break;
		case MV_PORT_TYPE_QSGMII:
			mvOsOutput("|   QSGMII   ");
			break;
		}
		if (mvBoardPhyAddrGet(port) != -1)
			mvOsOutput("|     %#04x     |\n", (MV_U8)mvBoardPhyAddrGet(port));
		else
			mvOsOutput("|     In-Band  |\n");
	}
}

/*******************************************************************************
* mvBoardMppIdUpdate - Update MPP ID's according to modules auto-detection.
*
* DESCRIPTION:
*	Update MPP ID's according to on-board modules as detected using TWSI bus.
*	Update board information for changed mpp values
	Must run AFTER mvBoardNetComplexInfoUpdate
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
MV_VOID mvBoardMppIdUpdate(MV_VOID)
{
#ifdef CONFIG_CMD_BOARDCFG
	struct _mvBoardMppModule spi0Boot[6] = MPP_SPI0_BOOT;
	struct _mvBoardMppModule sdioSpi0[7] = MPP_SDIO_SPI0;
	struct _mvBoardMppModule sdio[4] = MPP_SDIO;

	switch (mvBoardIdGet()) {
	case A39X_RD_69XX_ID:
		switch (mvBoardSysConfigGet(MV_CONFIG_GP_CONFIG)) {
		case MV_GP_CONFIG_EAP_10G:
			break;
		case MV_GP_CONFIG_HGW_AP_2_5G:
			mvModuleMppUpdate(7, sdioSpi0);
			break;
		case MV_GP_CONFIG_EAP_1G:
		case MV_GP_CONFIG_HGW_AP_2_5G_SATA:
			mvModuleMppUpdate(4, sdio);
			break;
		default:
			mvOsPrintf("%s: Error: Invalid GP config value.\n" , __func__);
		}
		break;
	case A39X_DB_69XX_ID:
		/* When boot from SPI 0, need to update the MPPs for SPI0, RGMII, and SDIO */
		if (mvBoardSpiBusGet() == 0)
			mvModuleMppUpdate(6, spi0Boot);
		break;
	}
#endif /* CONFIG_CMD_BOARDCFG */
}

/*******************************************************************************
* mvBoardIoExpanderUpdate
*
* DESCRIPTION:
*	Update IO expander data in board structures only
*
** INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK - on success,
*	MV_ERROR - wriet to twsi failed.
*
*******************************************************************************/
MV_STATUS mvBoardIoExpanderUpdate(MV_VOID)
{
#ifdef CONFIG_CMD_BOARDCFG
	MV_U32 gpConfig = mvBoardSysConfigGet(MV_CONFIG_GP_CONFIG);

	/* Config reg#2: The below bits are configured as output:
		BIT0: 0x0 - SGMII3 select, 0x1 - PCIe2 on SerDes 4
		BIT2: 0x1 - PCIe1_W disable (always 0x1)
		BIT4: 0x1 - PCIe2_W disable (always 0x1)
		BIT6: 0x1 - PCIe3_W disable (always 0x1) */
	/* Config reg#3: The below bits are configured as output:
		BIT0: 0x0 - disable power for SATA, 0x1 - enable power for SATA
		BIT1: 0x0 - disable SDIO, 0x1 - enable SDIO
		BIT2: 0x1 - PWR_EN_Module (always 0x1) */
	mvBoardIoExpanderStructSet(0, 6, 0xAA);
	mvBoardIoExpanderStructSet(0, 7, 0xF8);
	switch (gpConfig) {
	case MV_GP_CONFIG_EAP_10G:
		/* SGMII3 enable & SDIO disabled */
		mvBoardIoExpanderStructSet(0, 2, 0xFE);
		mvBoardIoExpanderStructSet(0, 3, 0xFD);
		break;
	case MV_GP_CONFIG_EAP_1G:
		/* SGMII3 enable & SDIO enable */
		mvBoardIoExpanderStructSet(0, 2, 0xFE);
		mvBoardIoExpanderStructSet(0, 3, 0xFF);
		break;
	case MV_GP_CONFIG_HGW_AP_2_5G:
	case MV_GP_CONFIG_HGW_AP_2_5G_SATA:
		/* PCIe2 enable & SDIO enable */
		mvBoardIoExpanderStructSet(0, 2, 0xFF);
		mvBoardIoExpanderStructSet(0, 3, 0xFF);
		break;
	default:
		/* SGMII3 enable & SDIO disabled */
		mvBoardIoExpanderStructSet(0, 2, 0xFE);
		mvBoardIoExpanderStructSet(0, 3, 0xFD);
		mvOsPrintf("%s: Error: Invalid GP config value.\n" , __func__);
	}
#endif /* CONFIG_CMD_BOARDCFG */
	return MV_OK;
}

/*******************************************************************************
* mvBoardIsUsb3PortDevice
* DESCRIPTION: return true USB3 port is in device mode
*
* INPUT:  port		- port number
* OUTPUT: None.
* RETURN: MV_TRUE: if port is set to device mode
*         MV_FALSE: otherwise
*******************************************************************************/
MV_BOOL mvBoardIsUsb3PortDevice(MV_U32 port)
{
	/* Since usb3 device is not supported on current board return false */
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardUpdateConfigforDT
* DESCRIPTION: Update board configuration structure for DT update
*
* INPUT:  None.
* OUTPUT: None.
* RETURN: None.
*******************************************************************************/
MV_VOID mvBoardUpdateConfigforDT(MV_VOID)
{
#if defined(CONFIG_CMD_BOARDCFG)
	/* This temporary fix for a39x device tree blob,
	   MAC#3 is working with NSS mode only (MAC#0,#1,#2 works with both modes),
	   and U-Boot doesn't support NSS mode.
	   U-Boot: in EAP configuration SerDes Lane #4 is connected to MAC#1.
	   Linux: change Lane#4 connectivity to use MAC#3 (NSS mode).
	   1. This fix sets MAC#3 settings for linux, accoding to MAC#1 configuration in U-Boot.
	   2. Added force link for MAC#2/MAC#3 node
	   (due to partial switch support in PP3 driver in linux, regarding fetching DT settings)
	*/
	MV_U32 netComplex = 0;
	MV_U32 gpConfig = mvBoardSysConfigGet(MV_CONFIG_GP_CONFIG);
	MV_U32 boardId = mvBoardIdGet();
	if (boardId == A39X_RD_69XX_ID && (gpConfig == MV_GP_CONFIG_EAP_10G || gpConfig == MV_GP_CONFIG_EAP_1G)) {
		netComplex |= MV_NETCOMP_GE_MAC3_2_SGMII_L4;
		if (gpConfig == MV_GP_CONFIG_EAP_10G)
			netComplex |= MV_NETCOMP_GE_MAC0_2_RXAUI;
		else
			netComplex |= MV_NETCOMP_GE_MAC0_2_SGMII_L6;
		mvBoardNetComplexConfigSet(netComplex);
		mvBoardMacStatusSet(1, MV_FALSE);
		mvBoardMacStatusSet(3, MV_TRUE);
	}
#endif
}

/*******************************************************************************
* mvBoardGetModelName
*
* DESCRIPTION:
*       This function returns a string describing the board model.
*
* OUTPUT:
*       pNameBuff - Buffer to contain board model name string. Minimum size 80 chars.
*
*******************************************************************************/
void mvBoardGetModelName(char *pNameBuff)
{
	MV_BOARD_INFO *board = mvBoardInfoStructureGet();

	if (!board) {
		DB(mvOsPrintf("%s: Error: board structure not initialized\n", __func__));
		return;
	}

	mvOsSPrintf(pNameBuff, "Marvell Armada %s %s", board->modelName, board->boardName);
}
