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
		if (netComplex & (MV_NETCOMP_GE_MAC0_2_SGMII_L0 | MV_NETCOMP_GE_MAC0_2_SGMII_L1))
			return MV_TRUE;
		return MV_FALSE;
	case 1:
		if (netComplex & (MV_NETCOMP_GE_MAC1_2_SGMII_L1 | MV_NETCOMP_GE_MAC1_2_SGMII_L2 |
					MV_NETCOMP_GE_MAC1_2_SGMII_L4))
			return MV_TRUE;
		return MV_FALSE;
	case 2:
		if (netComplex & (MV_NETCOMP_GE_MAC2_2_SGMII_L3 | MV_NETCOMP_GE_MAC2_2_SGMII_L5))
			return MV_TRUE;
		return MV_FALSE;
	case 3:
		if (netComplex & (MV_NETCOMP_GE_MAC3_2_SGMII_L4 | MV_NETCOMP_GE_MAC3_2_SGMII_L6))
			return MV_TRUE;
		return MV_FALSE;
	}
	return MV_FALSE;
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
	MV_U32 smiAddress = -1, netComplex;

	switch (mvBoardIdGet()) {
	case A39X_DB_69XX_ID:
#ifdef CONFIG_CMD_BOARDCFG
		mvBoardNetComplexInfoUpdate();
#endif

		netComplex = mvBoardNetComplexConfigGet();
		if (netComplex & (MV_NETCOMP_GE_MAC0_2_SGMII_L0 | MV_NETCOMP_GE_MAC0_2_SGMII_L1))
			smiAddress = 0x4;
		else if (netComplex & (MV_NETCOMP_GE_MAC0_2_RXAUI | MV_NETCOMP_GE_MAC0_2_XAUI))
			smiAddress = 0x0;
		mvBoardPhyAddrSet(0, smiAddress);

		if (netComplex & (MV_NETCOMP_GE_MAC1_2_SGMII_L1 | MV_NETCOMP_GE_MAC1_2_SGMII_L2))
			smiAddress = 0x5;
		else if (netComplex & (MV_NETCOMP_GE_MAC1_2_RGMII1))
			smiAddress = 0x1;
		mvBoardPhyAddrSet(1, smiAddress);

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
	MV_U32 netComplex = mvBoardNetComplexConfigGet();
	MV_BOOL isConnected = MV_FALSE;
	switch (ethNum) {
	case 0:
		if (netComplex & (MV_NETCOMP_GE_MAC0_2_RXAUI | MV_NETCOMP_GE_MAC0_2_XAUI |
					MV_NETCOMP_GE_MAC0_2_SGMII_L0 | MV_NETCOMP_GE_MAC0_2_SGMII_L1 |
					MV_NETCOMP_GE_MAC0_2_QSGMII))
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

	/* COMPHY3: 8 = XAUI, COMPHY4: 9 = XAUI, COMPHY5: 8 = XAUI, COMPHY6: 4 = XAUI */
	if (sysConfig3 == 0x8 && sysConfig4 == 0x9 && sysConfig5 == 0x8 && sysConfig6 == 0x4)
		netComplexOptions |= MV_NETCOMP_GE_MAC0_2_XAUI;
	/* COMPHY5: 8 = RXAUI, COMPHY6: 4 = RXAUI */
	else if (sysConfig5 == 0x8 && sysConfig6 == 0x4)
		netComplexOptions |= MV_NETCOMP_GE_MAC0_2_RXAUI;

	/* If MAC1 is not connected to SGMII, connect MAC1 to RGMII1 */
	if (!(netComplexOptions & (MV_NETCOMP_GE_MAC1_2_SGMII_L4 | MV_NETCOMP_GE_MAC1_2_SGMII_L2 |
					MV_NETCOMP_GE_MAC1_2_SGMII_L1 | MV_NETCOMP_GE_MAC1_2_QSGMII)))
		netComplexOptions |= MV_NETCOMP_GE_MAC1_2_RGMII1;

	mvBoardNetComplexConfigSet(netComplexOptions);
	return MV_OK;
}
#endif /* CONFIG_CMD_BOARDCFG */

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
			mvOsOutput("|     In-Band     |\n");
	}
}
