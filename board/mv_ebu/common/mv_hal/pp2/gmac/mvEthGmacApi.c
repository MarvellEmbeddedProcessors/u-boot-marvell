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
#include "pp2/gbe/mvPp2Gbe.h"
#include "mvEthGmacApi.h"

void mvEthPortEnable(int port)
{
	MV_U32 regVal;

	regVal = MV_REG_READ(ETH_GMAC_CTRL_0_REG(port));
	regVal |= ETH_GMAC_PORT_EN_MASK;

	mvPp2WrReg(ETH_GMAC_CTRL_0_REG(port), regVal);
}

void mvEthPortDisable(int port)
{
	MV_U32 regVal;

	regVal = MV_REG_READ(ETH_GMAC_CTRL_0_REG(port));
	regVal &= ~(ETH_GMAC_PORT_EN_MASK);
	mvPp2WrReg(ETH_GMAC_CTRL_0_REG(port), regVal);
}

static void mvEthPortRgmiiSet(int port, int enable)
{
	MV_U32  regVal;

	regVal = MV_REG_READ(ETH_GMAC_CTRL_2_REG(port));
	if (enable)
		regVal |= ETH_GMAC_PORT_RGMII_MASK;
	else
		regVal &= ~ETH_GMAC_PORT_RGMII_MASK;

	mvPp2WrReg(ETH_GMAC_CTRL_2_REG(port), regVal);
}

static void mvEthPortSgmiiSet(int port, int enable)
{
	MV_U32 regVal;

	regVal = MV_REG_READ(ETH_GMAC_CTRL_2_REG(port));
	if (enable)
		regVal |= ETH_GMAC_PCS_ENABLE_MASK;
	else
		regVal &= ~ETH_GMAC_PCS_ENABLE_MASK;

	mvPp2WrReg(ETH_GMAC_CTRL_2_REG(port), regVal);
}

void mvEthPortLbSet(int port, int isGmii, int isPcsEn)
{
	MV_U32 regVal;

	regVal = MV_REG_READ(ETH_GMAC_CTRL_1_REG(port));

	if (isGmii)
		regVal |= ETH_GMAC_GMII_LB_EN_MASK;
	else
		regVal &= ~ETH_GMAC_GMII_LB_EN_MASK;

	if (isPcsEn)
		regVal |= ETH_GMAC_PCS_LB_EN_MASK;
	else
		regVal &= ~ETH_GMAC_PCS_LB_EN_MASK;

	mvPp2WrReg(ETH_GMAC_CTRL_1_REG(port), regVal);
}

void mvEthPortPowerUp(int port, MV_BOOL isSgmii, MV_BOOL isRgmii)
{
	MV_U32 regVal;

	mvEthPortSgmiiSet(port, isSgmii);
	mvEthPortRgmiiSet(port, isRgmii);

	/* Cancel Port Reset */
	regVal = MV_REG_READ(ETH_GMAC_CTRL_2_REG(port));
	regVal &= (~ETH_GMAC_PORT_RESET_MASK);
	mvPp2WrReg(ETH_GMAC_CTRL_2_REG(port), regVal);
	while ((MV_REG_READ(ETH_GMAC_CTRL_2_REG(port)) & ETH_GMAC_PORT_RESET_MASK) != 0)
		continue;
}

void mvEthPortPowerDown(int port)
{
}

MV_BOOL mvEthPortIsLinkUp(int port)
{
	return (MV_REG_READ(ETH_GMAC_STATUS_REG(port)) & ETH_GMAC_LINK_UP_MASK);
}

MV_STATUS mvEthLinkStatus(int port, MV_ETH_PORT_STATUS *pStatus)
{
	MV_U32 regVal;

	if (MV_PON_PORT(port)) {
		pStatus->linkup = MV_TRUE;
		pStatus->speed = MV_ETH_SPEED_1000;
		pStatus->duplex = MV_ETH_DUPLEX_FULL;
		pStatus->rxFc = MV_ETH_FC_DISABLE;
		pStatus->txFc = MV_ETH_FC_DISABLE;
		return MV_OK;
	}

	regVal = MV_REG_READ(ETH_GMAC_STATUS_REG(port));

	if (regVal & ETH_GMAC_SPEED_1000_MASK)
		pStatus->speed = MV_ETH_SPEED_1000;
	else if (regVal & ETH_GMAC_SPEED_100_MASK)
		pStatus->speed = MV_ETH_SPEED_100;
	else
		pStatus->speed = MV_ETH_SPEED_10;

	if (regVal & ETH_GMAC_LINK_UP_MASK)
		pStatus->linkup = MV_TRUE;
	else
		pStatus->linkup = MV_FALSE;

	if (regVal & ETH_GMAC_FULL_DUPLEX_MASK)
		pStatus->duplex = MV_ETH_DUPLEX_FULL;
	else
		pStatus->duplex = MV_ETH_DUPLEX_HALF;

	if (regVal & ETH_TX_FLOW_CTRL_ACTIVE_MASK)
		pStatus->txFc = MV_ETH_FC_ACTIVE;
	else if (regVal & ETH_TX_FLOW_CTRL_ENABLE_MASK)
		pStatus->txFc = MV_ETH_FC_ENABLE;
	else
		pStatus->txFc = MV_ETH_FC_DISABLE;

	if (regVal & ETH_RX_FLOW_CTRL_ACTIVE_MASK)
		pStatus->rxFc = MV_ETH_FC_ACTIVE;
	else if (regVal & ETH_RX_FLOW_CTRL_ENABLE_MASK)
		pStatus->rxFc = MV_ETH_FC_ENABLE;
	else
		pStatus->rxFc = MV_ETH_FC_DISABLE;

	return MV_OK;
}
/******************************************************************************/
/*                          Port Configuration functions                      */
/******************************************************************************/

/*******************************************************************************
* mvNetaMaxRxSizeSet -
*
* DESCRIPTION:
*       Change maximum receive size of the port. This configuration will take place
*       imidiately.
*
* INPUT:
*
* RETURN:
*******************************************************************************/
void mvEthMaxRxSizeSet(int port, int maxRxSize)
{
    MV_U32		regVal;

	if (!MV_PON_PORT(port)) {

		regVal =  MV_REG_READ(ETH_GMAC_CTRL_0_REG(port));
		regVal &= ~ETH_GMAC_MAX_RX_SIZE_MASK;
		regVal |= (((maxRxSize - MV_ETH_MH_SIZE) / 2) << ETH_GMAC_MAX_RX_SIZE_OFFS);
		mvPp2WrReg(ETH_GMAC_CTRL_0_REG(port), regVal);
/*
		mvOsPrintf("%s: port=%d, maxRxSize=%d, regAddr=0x%x, regVal=0x%x\n",
			__func__, port, maxRxSize, ETH_GMAC_CTRL_0_REG(port), regVal);
*/
	}
}

/*******************************************************************************
* mvEthForceLinkModeSet -
*
* DESCRIPTION:
*       Sets "Force Link Pass" and "Do Not Force Link Fail" bits.
* 	Note: This function should only be called when the port is disabled.
*
* INPUT:
* 	int		portNo			- port number
* 	MV_BOOL force_link_pass	- Force Link Pass
* 	MV_BOOL force_link_fail - Force Link Failure
*		0, 0 - normal state: detect link via PHY and connector
*		1, 1 - prohibited state.
*
* RETURN:
*******************************************************************************/
MV_STATUS mvEthForceLinkModeSet(int portNo, MV_BOOL force_link_up, MV_BOOL force_link_down)
{
	MV_U32	regVal;

	/* Can't force link pass and link fail at the same time */
	if ((force_link_up) && (force_link_down))
		return MV_BAD_PARAM;

	regVal = MV_REG_READ(ETH_GMAC_AN_CTRL_REG(portNo));

	if (force_link_up)
		regVal |= ETH_FORCE_LINK_PASS_MASK;
	else
		regVal &= ~ETH_FORCE_LINK_PASS_MASK;

	if (force_link_down)
		regVal |= ETH_FORCE_LINK_FAIL_MASK;
	else
		regVal &= ~ETH_FORCE_LINK_FAIL_MASK;

	mvPp2WrReg(ETH_GMAC_AN_CTRL_REG(portNo), regVal);

    return MV_OK;
}

/*******************************************************************************
* mvEthSpeedDuplexSet -
*
* DESCRIPTION:
*       Sets port speed to Auto Negotiation / 1000 / 100 / 10 Mbps.
*	Sets port duplex to Auto Negotiation / Full / Half Duplex.
*
* INPUT:
* 	int portNo - port number
* 	MV_ETH_PORT_SPEED speed - port speed
*	MV_ETH_PORT_DUPLEX duplex - port duplex mode
*
* RETURN:
*******************************************************************************/
MV_STATUS mvEthSpeedDuplexSet(int portNo, MV_ETH_PORT_SPEED speed, MV_ETH_PORT_DUPLEX duplex)
{
	MV_U32 regVal;

	/* Check validity */
	if ((speed == MV_ETH_SPEED_1000) && (duplex == MV_ETH_DUPLEX_HALF))
		return MV_BAD_PARAM;

	regVal = MV_REG_READ(ETH_GMAC_AN_CTRL_REG(portNo));

	switch (speed) {
	case MV_ETH_SPEED_AN:
		regVal |= ETH_ENABLE_SPEED_AUTO_NEG_MASK;
		/* the other bits don't matter in this case */
		break;
	case MV_ETH_SPEED_1000:
		regVal &= ~ETH_ENABLE_SPEED_AUTO_NEG_MASK;
		regVal |= ETH_SET_GMII_SPEED_1000_MASK;
		regVal &= ~ETH_SET_MII_SPEED_100_MASK;
		/* the 100/10 bit doesn't matter in this case */
		break;
	case MV_ETH_SPEED_100:
		regVal &= ~ETH_ENABLE_SPEED_AUTO_NEG_MASK;
		regVal &= ~ETH_SET_GMII_SPEED_1000_MASK;
		regVal |= ETH_SET_MII_SPEED_100_MASK;
		break;
	case MV_ETH_SPEED_10:
		regVal &= ~ETH_ENABLE_SPEED_AUTO_NEG_MASK;
		regVal &= ~ETH_SET_GMII_SPEED_1000_MASK;
		regVal &= ~ETH_SET_MII_SPEED_100_MASK;
		break;
	default:
		mvOsPrintf("Unexpected Speed value %d\n", speed);
		return MV_BAD_PARAM;
	}

	switch (duplex) {
	case MV_ETH_DUPLEX_AN:
		regVal  |= ETH_ENABLE_DUPLEX_AUTO_NEG_MASK;
		/* the other bits don't matter in this case */
		break;
	case MV_ETH_DUPLEX_HALF:
		regVal &= ~ETH_ENABLE_DUPLEX_AUTO_NEG_MASK;
		regVal &= ~ETH_SET_FULL_DUPLEX_MASK;
		break;
	case MV_ETH_DUPLEX_FULL:
		regVal &= ~ETH_ENABLE_DUPLEX_AUTO_NEG_MASK;
		regVal |= ETH_SET_FULL_DUPLEX_MASK;
		break;
	default:
		mvOsPrintf("Unexpected Duplex value %d\n", duplex);
		return MV_BAD_PARAM;
	}

	mvPp2WrReg(ETH_GMAC_AN_CTRL_REG(portNo), regVal);
	return MV_OK;
}

/*******************************************************************************
* mvEthSpeedDuplexGet -
*
* DESCRIPTION:
*       Gets port speed
*	Gets port duplex
*
* INPUT:
* 	int portNo - port number
* OUTPUT:
* 	MV_ETH_PORT_SPEED *speed - port speed
*	MV_ETH_PORT_DUPLEX *duplex - port duplex mode
*
* RETURN:
*******************************************************************************/
MV_STATUS mvEthSpeedDuplexGet(int portNo, MV_ETH_PORT_SPEED *speed, MV_ETH_PORT_DUPLEX *duplex)
{
	MV_U32 regVal;

	/* Check validity */
	if (!speed || !duplex)
		return MV_BAD_PARAM;

	regVal = MV_REG_READ(ETH_GMAC_AN_CTRL_REG(portNo));
	if (regVal & ETH_ENABLE_SPEED_AUTO_NEG_MASK)
		*speed = MV_ETH_SPEED_AN;
	else if (regVal & ETH_SET_GMII_SPEED_1000_MASK)
		*speed = MV_ETH_SPEED_1000;
	else if (regVal & ETH_SET_MII_SPEED_100_MASK)
		*speed = MV_ETH_SPEED_100;
	else
		*speed = MV_ETH_SPEED_10;

	if (regVal & ETH_ENABLE_DUPLEX_AUTO_NEG_MASK)
		*duplex = MV_ETH_DUPLEX_AN;
	else if (regVal & ETH_SET_FULL_DUPLEX_MASK)
		*duplex = MV_ETH_DUPLEX_FULL;
	else
		*duplex = MV_ETH_DUPLEX_HALF;

	return MV_OK;
}

/*******************************************************************************
* mvEthFlowCtrlSet - Set Flow Control of the port.
*
* DESCRIPTION:
*       This function configures the port's Flow Control properties.
*
* INPUT:
*       int				port		- Port number
*       MV_ETH_PORT_FC  flowControl - Flow control of the port.
*
* RETURN:   MV_STATUS
*       MV_OK           - Success
*       MV_OUT_OF_RANGE - Failed. Port is out of valid range
*       MV_BAD_VALUE    - Value flowControl parameters is not valid
*
*******************************************************************************/
MV_STATUS mvEthFlowCtrlSet(int port, MV_ETH_PORT_FC flowControl)
{
	MV_U32 regVal;

	regVal = MV_REG_READ(ETH_GMAC_AN_CTRL_REG(port));

	switch (flowControl) {
	case MV_ETH_FC_AN_NO:
		regVal |= ETH_ENABLE_FLOW_CONTROL_AUTO_NEG_MASK;
		regVal &= ~ETH_FLOW_CONTROL_ADVERTISE_MASK;
		regVal &= ~ETH_FLOW_CONTROL_ASYMETRIC_MASK;
		break;

	case MV_ETH_FC_AN_SYM:
		regVal |= ETH_ENABLE_FLOW_CONTROL_AUTO_NEG_MASK;
		regVal |= ETH_FLOW_CONTROL_ADVERTISE_MASK;
		regVal &= ~ETH_FLOW_CONTROL_ASYMETRIC_MASK;
		break;

	case MV_ETH_FC_AN_ASYM:
		regVal |= ETH_ENABLE_FLOW_CONTROL_AUTO_NEG_MASK;
		regVal |= ETH_FLOW_CONTROL_ADVERTISE_MASK;
		regVal |= ETH_FLOW_CONTROL_ASYMETRIC_MASK;
		break;

	case MV_ETH_FC_DISABLE:
		regVal &= ~ETH_ENABLE_FLOW_CONTROL_AUTO_NEG_MASK;
		regVal &= ~ETH_SET_FLOW_CONTROL_MASK;
		break;

	case MV_ETH_FC_ENABLE:
		regVal &= ~ETH_ENABLE_FLOW_CONTROL_AUTO_NEG_MASK;
		regVal |= ETH_SET_FLOW_CONTROL_MASK;
		break;

	default:
		mvOsPrintf("ethDrv: Unexpected FlowControl value %d\n", flowControl);
		return MV_BAD_VALUE;
	}

	mvPp2WrReg(ETH_GMAC_AN_CTRL_REG(port), regVal);

	return MV_OK;
}

/*******************************************************************************
* mvEthFlowCtrlGet - Get Flow Control configuration of the port.
*
* DESCRIPTION:
*       This function returns the port's Flow Control properties.
*
* INPUT:
*       int				port		- Port number
*
* OUTPUT:
*       MV_ETH_PORT_FC  *flowCntrl	- Flow control of the port.
*
* RETURN:   MV_STATUS
*       MV_OK           - Success
*       MV_OUT_OF_RANGE - Failed. Port is out of valid range
*
*******************************************************************************/
MV_STATUS mvEthFlowCtrlGet(int port, MV_ETH_PORT_FC *pFlowCntrl)
{
	MV_U32 regVal;

	regVal = MV_REG_READ(ETH_GMAC_AN_CTRL_REG(port));

	if (regVal & ETH_ENABLE_FLOW_CONTROL_AUTO_NEG_MASK) {
		/* Auto negotiation is enabled */
		if (regVal & ETH_FLOW_CONTROL_ADVERTISE_MASK) {
			if (regVal & ETH_FLOW_CONTROL_ASYMETRIC_MASK)
				*pFlowCntrl = MV_ETH_FC_AN_ASYM;
			else
				*pFlowCntrl = MV_ETH_FC_AN_SYM;
		} else
			*pFlowCntrl = MV_ETH_FC_AN_NO;
	} else {
		/* Auto negotiation is disabled */
		if (regVal & ETH_SET_FLOW_CONTROL_MASK)
			*pFlowCntrl = MV_ETH_FC_ENABLE;
		else
			*pFlowCntrl = MV_ETH_FC_DISABLE;
	}
	return MV_OK;
}

/******************************************************************************/
/*                         PHY Control Functions                              */
/******************************************************************************/
void mvEthPhyAddrSet(int port, int phyAddr)
{
	unsigned int regData;

	regData = MV_REG_READ(ETH_PHY_ADDR_REG);

	regData &= ~ETH_PHY_ADDR_MASK(port);
	regData |= (phyAddr << ETH_PHY_ADDR_OFFS(port));

	mvPp2WrReg(ETH_PHY_ADDR_REG, regData);

	return;
}

int mvEthPhyAddrGet(int port)
{
	unsigned int 	regData;

	regData = MV_REG_READ(ETH_PHY_ADDR_REG);

	return ((regData & ETH_PHY_ADDR_MASK(port)) >> ETH_PHY_ADDR_OFFS(port));
}

/******************************************************************************/
/*                      MIB Counters functions                                */
/******************************************************************************/

/*******************************************************************************
* mvEthMibCounterRead - Read a MIB counter
*
* DESCRIPTION:
*       This function reads a MIB counter of a specific ethernet port.
*       NOTE - Read from ETH_MIB_GOOD_OCTETS_RECEIVED_LOW or
*              ETH_MIB_GOOD_OCTETS_SENT_LOW counters will return 64 bits value,
*              so pHigh32 pointer should not be NULL in this case.
*
* INPUT:
*       port        - Ethernet Port number.
*       mibOffset   - MIB counter offset.
*
* OUTPUT:
*       MV_U32*       pHigh32 - pointer to place where 32 most significant bits
*                             of the counter will be stored.
*
* RETURN:
*       32 low sgnificant bits of MIB counter value.
*
*******************************************************************************/
MV_U32 mvEthMibCounterRead(int port, unsigned int mibOffset, MV_U32 *pHigh32)
{
	MV_U32 valLow32, valHigh32;

	valLow32 = MV_REG_READ(ETH_MIB_COUNTERS_BASE(port) + mibOffset);

	/* Implement FEr ETH. Erroneous Value when Reading the Upper 32-bits    */
	/* of a 64-bit MIB Counter.                                             */
	if ((mibOffset == ETH_MIB_GOOD_OCTETS_RECEIVED_LOW) || (mibOffset == ETH_MIB_GOOD_OCTETS_SENT_LOW)) {
		valHigh32 = MV_REG_READ(ETH_MIB_COUNTERS_BASE(port) + mibOffset + 4);
		if (pHigh32 != NULL)
			*pHigh32 = valHigh32;
	}
	return valLow32;
}

/*******************************************************************************
* mvEthMibCountersClear - Clear all MIB counters
*
* DESCRIPTION:
*       This function clears all MIB counters
*
* INPUT:
*       port      - Ethernet Port number.
*
* RETURN:   void
*
*******************************************************************************/
void mvEthMibCountersClear(int port)
{
	int i;
	MV_U32 dummy;

#if defined(CONFIG_MV_PON) && !defined(MV_PON_MIB_SUPPORT)
	if (MV_PON_PORT(port))
		return;
#endif /* CONFIG_MV_PON && !MV_PON_MIB_SUPPORT */

	/* Perform dummy reads from MIB counters */
	for (i = ETH_MIB_GOOD_OCTETS_RECEIVED_LOW; i < ETH_MIB_LATE_COLLISION; i += 4)
		dummy = MV_REG_READ((ETH_MIB_COUNTERS_BASE(port) + i));
}

static void mvEthMibPrint(int port, MV_U32 offset, char *mib_name)
{
	MV_U32 regVaLo, regValHi = 0;

	regVaLo = mvEthMibCounterRead(port, offset, &regValHi);

	if (!regValHi)
		mvOsPrintf("  %-32s: %u\n", mib_name, regVaLo);
	else
		mvOsPrintf("  t%-32s: 0x%08x%08x\n", mib_name, regValHi, regVaLo);
}

/* Print MIB counters of the Ethernet port */
void mvEthMibCountersShow(int port)
{
	if (MV_PON_PORT(port)) {
		mvOsPrintf("%s: not supported for PON port\n", __func__);
		return;
	}

/*TODO: check port
	if (mvNetaTxpCheck(port))
		return;
*/
	mvOsPrintf("\nMIBs: port=%d\n", port);

	mvOsPrintf("\n[Rx]\n");
	mvEthMibPrint(port, ETH_MIB_GOOD_FRAMES_RECEIVED, "GOOD_FRAMES_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_BAD_FRAMES_RECEIVED, "BAD_FRAMES_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_BROADCAST_FRAMES_RECEIVED, "BROADCAST_FRAMES_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_MULTICAST_FRAMES_RECEIVED, "MULTICAST_FRAMES_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_GOOD_OCTETS_RECEIVED_LOW, "GOOD_OCTETS_RECEIVED");
	mvOsPrintf("\n[Rx Errors]\n");
	mvEthMibPrint(port, ETH_MIB_BAD_OCTETS_RECEIVED, "BAD_OCTETS_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_UNDERSIZE_RECEIVED, "UNDERSIZE_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_FRAGMENTS_RECEIVED, "FRAGMENTS_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_OVERSIZE_RECEIVED, "OVERSIZE_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_JABBER_RECEIVED, "JABBER_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_MAC_RECEIVE_ERROR, "MAC_RECEIVE_ERROR");
	mvEthMibPrint(port, ETH_MIB_BAD_CRC_EVENT, "BAD_CRC_EVENT");
	mvOsPrintf("\n[Tx]\n");
	mvEthMibPrint(port, ETH_MIB_GOOD_FRAMES_SENT, "GOOD_FRAMES_SENT");
	mvEthMibPrint(port, ETH_MIB_BROADCAST_FRAMES_SENT, "BROADCAST_FRAMES_SENT");
	mvEthMibPrint(port, ETH_MIB_MULTICAST_FRAMES_SENT, "MULTICAST_FRAMES_SENT");
	mvEthMibPrint(port, ETH_MIB_GOOD_OCTETS_SENT_LOW, "GOOD_OCTETS_SENT");
	mvOsPrintf("\n[Tx Errors]\n");
	mvEthMibPrint(port, ETH_MIB_INTERNAL_MAC_TRANSMIT_ERR, "INTERNAL_MAC_TRANSMIT_ERR");
	mvEthMibPrint(port, ETH_MIB_EXCESSIVE_COLLISION, "EXCESSIVE_COLLISION");
	mvEthMibPrint(port, ETH_MIB_COLLISION, "COLLISION");
	mvEthMibPrint(port, ETH_MIB_LATE_COLLISION, "LATE_COLLISION");
	mvOsPrintf("\n[FC control]\n");
	mvEthMibPrint(port, ETH_MIB_UNREC_MAC_CONTROL_RECEIVED, "UNREC_MAC_CONTROL_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_GOOD_FC_RECEIVED, "GOOD_FC_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_BAD_FC_RECEIVED, "BAD_FC_RECEIVED");
	mvEthMibPrint(port, ETH_MIB_FC_SENT, "FC_SENT");
	mvOsPrintf("\n");
}
