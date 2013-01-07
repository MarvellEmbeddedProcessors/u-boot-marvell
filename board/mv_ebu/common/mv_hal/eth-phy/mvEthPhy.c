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

#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysEthPhyConfig.h"
#include "mvEthPhyRegs.h"
#include "mvEthPhy.h"
#undef DEBUG
#ifdef DEBUG
#define DB(x) x
#define DB2(x) x
#else
#define DB(x)
#define DB2(x)
#endif /* DEBUG */

static 	MV_VOID	mvEthPhyPower(MV_U32 ethPortNum, MV_BOOL enable);

static MV_ETHPHY_HAL_DATA ethphyHalData;

/*******************************************************************************
* mvEthPhyHalInit -
*
* DESCRIPTION:
*       Initialize the ethernet phy unit HAL.
*
* INPUT:
*       halData	- Ethernet PHY HAL data.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK on success, MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvEthPhyHalInit(MV_ETHPHY_HAL_DATA *halData)
{
	mvOsMemcpy(&ethphyHalData, halData, sizeof(MV_ETHPHY_HAL_DATA));

	return MV_OK;
}
/*******************************************************************************
* mvEthPhyInit -
*
* DESCRIPTION:
*       Initialize the ethernet phy unit.
*
* INPUT:
*       ethPortNum - The port number on which to initialize the PHY.
*	eeeEnable  - Whether to enable EEE or not.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK on success, MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvEthPhyInit(MV_U32 ethPortNum, MV_BOOL eeeEnable)
{
	MV_U32     phyAddr = 0;
	MV_U16     deviceId;
	MV_U16     id1, id2;

	if (ethPortNum != ((MV_U32) -1))
		phyAddr = ethphyHalData.phyAddr[ethPortNum];

	/* Set page as 0 */
	if (mvEthPhyRegWrite(phyAddr, 22, 0) != MV_OK)
		return MV_ERROR;

	/* Reads ID1 */
	if (mvEthPhyRegRead(phyAddr, 2, &id1) != MV_OK)
		return MV_ERROR;

	/* Reads ID2 */
	if (mvEthPhyRegRead(phyAddr, 3, &id2) != MV_OK)
		return MV_ERROR;

	if (!MV_IS_MARVELL_OUI(id1, id2)) {
		mvOsPrintf("Not Marvell PHY id1 %x id2 %x\n", id1, id2);
		return MV_ERROR;
	}

	deviceId = (id2 & 0x3F0) >> 4;
	switch (deviceId) {
	case MV_PHY_88E1116:
	case MV_PHY_88E1116R:
		mvEthE1116PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E3061:
		mvEthE3016PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E1111:
		mvEthE1111PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E1112:
		mvEthE1112PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E1011:
		mvEthE1011PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E1121:
		mvEth1121PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E114X:
		mvEth1145PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E154X:
	case MV_PHY_88E1340M: /* BUG IN THE 1543 IS RETURNING THE 1340 ID FOR PORTS 3-4 */
	/* case MV_PHY_KW2_INTERNAL_GE:
		if (ethPortNum != ((MV_U32) -1))
			mvEthInternalGEPhyBasicInit(ethPortNum);
		else
	*/
			mvEth1540PhyBasicInit(ethPortNum, eeeEnable);
		break;
	case MV_PHY_88E1340S:
	case MV_PHY_88E1340:
		mvEth1340PhyBasicInit();
		break;
	case MV_PHY_88E1310:
		mvEthE1310PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E1512:
		mvEthE1512PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E104X:
	case MV_PHY_88E10X0:
	case MV_PHY_88E10X0S:
	case MV_PHY_88E3082:
	case MV_PHY_88E1149:
	case MV_PHY_88E1181:
	case MV_PHY_88E3016_88E3019:
	case MV_PHY_88E1240:
	case MV_PHY_88E1149R:
	case MV_PHY_88E1119R:
		return MV_NO_SUCH;
	default:
		mvOsPrintf("Unknown Device(%#x). Initialization failed\n", deviceId);
		return MV_ERROR;
	}
	return MV_OK;
}

void    rdPhy(MV_U32 phyAddr, MV_U32 regOffs)
{
	MV_U16      data;
	MV_STATUS   status;

	status = mvEthPhyRegRead(phyAddr, regOffs, &data);
	if (status == MV_OK)
		mvOsPrintf("reg=%d: 0x%04x\n", regOffs, data);
	else
		mvOsPrintf("Read failed\n");
}


/*******************************************************************************
* mvEthPhyRegRead - Read from ethernet phy register.
*
* DESCRIPTION:
*       This function reads ethernet phy register.
*
* INPUT:
*       phyAddr - Phy address.
*       regOffs - Phy register offset.
*
* OUTPUT:
*       None.
*
* RETURN:
*       16bit phy register value, or 0xffff on error
*
*******************************************************************************/
MV_STATUS mvEthPhyRegRead(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 *data)
{
	MV_U32 			smiReg;
	volatile MV_U32 timeout;

	/* check parameters */
	if ((phyAddr << ETH_PHY_SMI_DEV_ADDR_OFFS) & ~ETH_PHY_SMI_DEV_ADDR_MASK) {
		mvOsPrintf("mvEthPhyRegRead: Err. Illegal PHY device address %d\n",
				phyAddr);
		return MV_FAIL;
	}
	if ((regOffs <<  ETH_PHY_SMI_REG_ADDR_OFFS) & ~ETH_PHY_SMI_REG_ADDR_MASK) {
		mvOsPrintf("mvEthPhyRegRead: Err. Illegal PHY register offset %d\n",
				regOffs);
		return MV_FAIL;
	}

	timeout = ETH_PHY_TIMEOUT;
	/* wait till the SMI is not busy*/
	do {
		/* read smi register */
		smiReg = MV_REG_READ(ethphyHalData.ethPhySmiReg);
		if (timeout-- == 0) {
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return MV_FAIL;
		}
	} while (smiReg & ETH_PHY_SMI_BUSY_MASK);

	/* fill the phy address and regiser offset and read opcode */
	smiReg = (phyAddr <<  ETH_PHY_SMI_DEV_ADDR_OFFS) | (regOffs << ETH_PHY_SMI_REG_ADDR_OFFS)|
			   ETH_PHY_SMI_OPCODE_READ;

	/* write the smi register */
	MV_REG_WRITE(ethphyHalData.ethPhySmiReg, smiReg);

	timeout = ETH_PHY_TIMEOUT;

	/*wait till readed value is ready */
	do {
		/* read smi register */
		smiReg = MV_REG_READ(ethphyHalData.ethPhySmiReg);

		if (timeout-- == 0) {
			mvOsPrintf("mvEthPhyRegRead: SMI read-valid timeout\n");
			return MV_FAIL;
		}
	} while (!(smiReg & ETH_PHY_SMI_READ_VALID_MASK));

	/* Wait for the data to update in the SMI register */
	for (timeout = 0; timeout < ETH_PHY_TIMEOUT; timeout++)
		;

	*data = (MV_U16)(MV_REG_READ(ethphyHalData.ethPhySmiReg) & ETH_PHY_SMI_DATA_MASK);

	return MV_OK;
}

MV_STATUS mvEthPhyRegPrint(MV_U32 phyAddr, MV_U32 regOffs)
{
	MV_U16      data;
	MV_STATUS   status;

	status = mvEthPhyRegRead(phyAddr, regOffs, &data);
	if (status == MV_OK)
		mvOsPrintf("phy=0x%x, reg=0x%x: 0x%04x\n", phyAddr, regOffs, data);
	else
		mvOsPrintf("Read failed\n");

	return status;
}

/*******************************************************************************
* mvEthPhyRegWrite - Write to ethernet phy register.
*
* DESCRIPTION:
*       This function write to ethernet phy register.
*
* INPUT:
*       phyAddr - Phy address.
*       regOffs - Phy register offset.
*       data    - 16bit data.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK if write succeed, MV_BAD_PARAM on bad parameters , MV_ERROR on error .
*		MV_TIMEOUT on timeout
*
*******************************************************************************/
MV_STATUS mvEthPhyRegWrite(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 data)
{
	MV_U32 			smiReg;
	volatile MV_U32 timeout;

	/* check parameters */
	if ((phyAddr <<  ETH_PHY_SMI_DEV_ADDR_OFFS) & ~ETH_PHY_SMI_DEV_ADDR_MASK) {
		mvOsPrintf("mvEthPhyRegWrite: Err. Illegal phy address 0x%x\n", phyAddr);
		return MV_BAD_PARAM;
	}
	if ((regOffs <<  ETH_PHY_SMI_REG_ADDR_OFFS) & ~ETH_PHY_SMI_REG_ADDR_MASK) {
		mvOsPrintf("mvEthPhyRegWrite: Err. Illegal register offset 0x%x\n", regOffs);
		return MV_BAD_PARAM;
	}

	timeout = ETH_PHY_TIMEOUT;

	/* wait till the SMI is not busy*/
	do {
		/* read smi register */
		smiReg = MV_REG_READ(ethphyHalData.ethPhySmiReg);
		if (timeout-- == 0) {
			mvOsPrintf("mvEthPhyRegWrite: SMI busy timeout\n");
		return MV_TIMEOUT;
		}
	} while (smiReg & ETH_PHY_SMI_BUSY_MASK);

	/* fill the phy address and regiser offset and write opcode and data*/
	smiReg = (data << ETH_PHY_SMI_DATA_OFFS);
	smiReg |= (phyAddr <<  ETH_PHY_SMI_DEV_ADDR_OFFS) | (regOffs << ETH_PHY_SMI_REG_ADDR_OFFS);
	smiReg &= ~ETH_PHY_SMI_OPCODE_READ;

	/* write the smi register */
	MV_REG_WRITE(ethphyHalData.ethPhySmiReg, smiReg);

	return MV_OK;
}


/*******************************************************************************
* mvEthPhyReset - Reset ethernet Phy.
*
* DESCRIPTION:
*       This function resets a given ethernet Phy.
*
* INPUT:
*       phyAddr - Phy address.
*       timeout - in millisec
*
* OUTPUT:
*       None.
*
* RETURN:   MV_OK       - Success
*           MV_TIMEOUT  - Timeout
*
*******************************************************************************/
MV_STATUS mvEthPhyReset(MV_U32 phyAddr, int timeout)
{
	MV_U16  phyRegData;

	/* Reset the PHY */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &phyRegData) != MV_OK)
		return MV_FAIL;

	/* Set bit 15 to reset the PHY */
	phyRegData |= ETH_PHY_CTRL_RESET_MASK;
	mvEthPhyRegWrite(phyAddr, ETH_PHY_CTRL_REG, phyRegData);

	/* Wait untill Reset completed */
	while (timeout > 0) {
		mvOsSleep(100);
		timeout -= 100;

		if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &phyRegData) != MV_OK)
			return MV_FAIL;

		if ((phyRegData & ETH_PHY_CTRL_RESET_MASK) == 0)
			return MV_OK;
	}
	return MV_TIMEOUT;
}


/*******************************************************************************
* mvEthPhyRestartAN - Restart ethernet Phy Auto-Negotiation.
*
* DESCRIPTION:
*       This function resets a given ethernet Phy.
*
* INPUT:
*       phyAddr - Phy address.
*       timeout - in millisec; 0 - no timeout (don't wait)
*
* OUTPUT:
*       None.
*
* RETURN:   MV_OK       - Success
*           MV_TIMEOUT  - Timeout
*
*******************************************************************************/
MV_STATUS mvEthPhyRestartAN(MV_U32 phyAddr, int timeout)
{
	MV_U16  phyRegData;

	/* Reset the PHY */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &phyRegData) != MV_OK)
		return MV_FAIL;

	/* Set bit 12 to Enable autonegotiation of the PHY */
	phyRegData |= ETH_PHY_CTRL_AN_ENABLE_MASK;

	/* Set bit 9 to Restart autonegotiation of the PHY */
	phyRegData |= ETH_PHY_CTRL_AN_RESTART_MASK;
	mvEthPhyRegWrite(phyAddr, ETH_PHY_CTRL_REG, phyRegData);

	if (timeout == 0)
		return MV_OK;

	/* Wait untill Auotonegotiation completed */
	while (timeout > 0) {
		mvOsSleep(100);
		timeout -= 100;

		if (mvEthPhyRegRead(phyAddr, ETH_PHY_STATUS_REG, &phyRegData) != MV_OK)
			return MV_FAIL;

		if (phyRegData & ETH_PHY_STATUS_AN_DONE_MASK)
			return MV_OK;
	}
	return MV_TIMEOUT;
}


/*******************************************************************************
* mvEthPhyDisableAN - Disable Phy Auto-Negotiation and set forced Speed and Duplex
*
* DESCRIPTION:
*       This function disable AN and set duplex and speed.
*
* INPUT:
*       phyAddr - Phy address.
*       speed   - port speed. 0 - 10 Mbps, 1-100 Mbps, 2 - 1000 Mbps
*       duplex  - port duplex. 0 - Half duplex, 1 - Full duplex
*
* OUTPUT:
*       None.
*
* RETURN:   MV_OK   - Success
*           MV_FAIL - Failure
*
*******************************************************************************/
MV_STATUS mvEthPhyDisableAN(MV_U32 phyAddr, int speed, int duplex)
{
	MV_U16  phyRegData;

	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &phyRegData) != MV_OK)
		return MV_FAIL;

	switch (speed) {
	case 0: /* 10 Mbps */
			phyRegData &= ~ETH_PHY_CTRL_SPEED_LSB_MASK;
			phyRegData &= ~ETH_PHY_CTRL_SPEED_MSB_MASK;
			break;

	case 1: /* 100 Mbps */
			phyRegData |= ETH_PHY_CTRL_SPEED_LSB_MASK;
			phyRegData &= ~ETH_PHY_CTRL_SPEED_MSB_MASK;
			break;

	case 2: /* 1000 Mbps */
			phyRegData &= ~ETH_PHY_CTRL_SPEED_LSB_MASK;
			phyRegData |= ETH_PHY_CTRL_SPEED_MSB_MASK;
			break;

	default:
			mvOsOutput("Unexpected speed = %d\n", speed);
			return MV_FAIL;
	}

	switch (duplex) {
	case 0: /* half duplex */
			phyRegData &= ~ETH_PHY_CTRL_DUPLEX_MASK;
			break;

	case 1: /* full duplex */
			phyRegData |= ETH_PHY_CTRL_DUPLEX_MASK;
			break;

	default:
			mvOsOutput("Unexpected duplex = %d\n", duplex);
	}
	/* Clear bit 12 to Disable autonegotiation of the PHY */
	phyRegData &= ~ETH_PHY_CTRL_AN_ENABLE_MASK;

	/* Clear bit 9 to DISABLE, Restart autonegotiation of the PHY */
	phyRegData &= ~ETH_PHY_CTRL_AN_RESTART_MASK;
	mvEthPhyRegWrite(phyAddr, ETH_PHY_CTRL_REG, phyRegData);

	return MV_OK;
}

MV_STATUS   mvEthPhyLoopback(MV_U32 phyAddr, MV_BOOL isEnable)
{
	MV_U16      regVal, ctrlVal;
	MV_STATUS   status;

	/* Set loopback speed and duplex accordingly with current */
	/* Bits: 6, 8, 13 */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &ctrlVal) != MV_OK)
		return MV_FAIL;

	if (isEnable) {
		/* Select page 2 */
		mvEthPhyRegWrite(phyAddr, 22, 2);

		mvEthPhyRegRead(phyAddr, 21, &regVal);
		regVal &= ~(ETH_PHY_CTRL_DUPLEX_MASK | ETH_PHY_CTRL_SPEED_LSB_MASK |
				ETH_PHY_CTRL_SPEED_MSB_MASK | ETH_PHY_CTRL_AN_ENABLE_MASK);
		regVal |= (ctrlVal & (ETH_PHY_CTRL_DUPLEX_MASK | ETH_PHY_CTRL_SPEED_LSB_MASK |
					ETH_PHY_CTRL_SPEED_MSB_MASK | ETH_PHY_CTRL_AN_ENABLE_MASK));
		mvEthPhyRegWrite(phyAddr, 21, regVal);

		/* Select page 0 */
		mvEthPhyRegWrite(phyAddr, 22, 0);

		/* Disable Energy detection   R16[9:8] = 00 */
		/* Disable MDI/MDIX crossover R16[6:5] = 00 */
		mvEthPhyRegRead(phyAddr, ETH_PHY_SPEC_CTRL_REG, &regVal);
		regVal &= ~(BIT5 | BIT6 | BIT8 | BIT9);
		mvEthPhyRegWrite(phyAddr, ETH_PHY_SPEC_CTRL_REG, regVal);

		status = mvEthPhyReset(phyAddr, 1000);
		if (status != MV_OK) {
			mvOsPrintf("mvEthPhyReset failed: status=0x%x\n", status);
			return status;
		}

		/* Set loopback */
		ctrlVal |= ETH_PHY_CTRL_LOOPBACK_MASK;
		mvEthPhyRegWrite(phyAddr, ETH_PHY_CTRL_REG, ctrlVal);
	} else {
		/* Cancel Loopback */
		ctrlVal &= ~ETH_PHY_CTRL_LOOPBACK_MASK;
		mvEthPhyRegWrite(phyAddr, ETH_PHY_CTRL_REG, ctrlVal);

		status = mvEthPhyReset(phyAddr, 1000);
		if (status != MV_OK) {
			mvOsPrintf("mvEthPhyReset failed: status=0x%x\n", status);
			return status;
		}

		/* Enable Energy detection   R16[9:8] = 11 */
		/* Enable MDI/MDIX crossover R16[6:5] = 11 */
		mvEthPhyRegRead(phyAddr, ETH_PHY_SPEC_CTRL_REG, &regVal);
		regVal |= (BIT5 | BIT6 | BIT8 | BIT9);
		mvEthPhyRegWrite(phyAddr, ETH_PHY_SPEC_CTRL_REG, regVal);

		status = mvEthPhyReset(phyAddr, 1000);
		if (status != MV_OK) {
			mvOsPrintf("mvEthPhyReset failed: status=0x%x\n", status);
			return status;
		}
	}

	return MV_OK;
}

/*******************************************************************************
* mvEthPhyCheckLink -
*
* DESCRIPTION:
*	check link in phy port
*
* INPUT:
*       phyAddr - Phy address.
*
* OUTPUT:
*       None.
*
* RETURN:   MV_TRUE if link is up, MV_FALSE if down
*
*******************************************************************************/
MV_BOOL mvEthPhyCheckLink(MV_U32 phyAddr)
{
	MV_U16 val_st, val_ctrl, val_spec_st;

	/* read status reg */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_STATUS_REG, &val_st) != MV_OK)
		return MV_FALSE;

	/* read control reg */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &val_ctrl) != MV_OK)
		return MV_FALSE;

	/* read special status reg */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_SPEC_STATUS_REG, &val_spec_st) != MV_OK)
		return MV_FALSE;

	/* Check for PHY exist */
	if ((val_ctrl == ETH_PHY_SMI_DATA_MASK) && (val_st & ETH_PHY_SMI_DATA_MASK))
		return MV_FALSE;


	if (val_ctrl & ETH_PHY_CTRL_AN_ENABLE_MASK) {
		if (val_st & ETH_PHY_STATUS_AN_DONE_MASK)
			return MV_TRUE;
		else
			return MV_FALSE;
	} else {
		if (val_spec_st & ETH_PHY_SPEC_STATUS_LINK_MASK)
			return MV_TRUE;
	}
	return MV_FALSE;
}

/*******************************************************************************
* mvEthPhyPrintStatus -
*
* DESCRIPTION:
*	print port Speed, Duplex, Auto-negotiation, Link.
*
* INPUT:
*       phyAddr - Phy address.
*
* OUTPUT:
*       None.
*
* RETURN:   16bit phy register value, or 0xffff on error
*
*******************************************************************************/
MV_STATUS	mvEthPhyPrintStatus(MV_U32 phyAddr)
{
	MV_U16 val;

	/* read control reg */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &val) != MV_OK)
		return MV_ERROR;

	if (val & ETH_PHY_CTRL_AN_ENABLE_MASK)
		mvOsOutput("Auto negotiation: Enabled\n");
	else
		mvOsOutput("Auto negotiation: Disabled\n");


	/* read specific status reg */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_SPEC_STATUS_REG, &val) != MV_OK)
		return MV_ERROR;

	switch (val & ETH_PHY_SPEC_STATUS_SPEED_MASK) {
	case ETH_PHY_SPEC_STATUS_SPEED_1000MBPS:
			mvOsOutput("Speed: 1000 Mbps\n");
			break;
	case ETH_PHY_SPEC_STATUS_SPEED_100MBPS:
			mvOsOutput("Speed: 100 Mbps\n");
			break;
	case ETH_PHY_SPEC_STATUS_SPEED_10MBPS:
			mvOsOutput("Speed: 10 Mbps\n");
	default:
			mvOsOutput("Speed: Uknown\n");
			break;

	}

	if (val & ETH_PHY_SPEC_STATUS_DUPLEX_MASK)
		mvOsOutput("Duplex: Full\n");
	else
		mvOsOutput("Duplex: Half\n");


	if (val & ETH_PHY_SPEC_STATUS_LINK_MASK)
		mvOsOutput("Link: up\n");
	else
		mvOsOutput("Link: down\n");

	return MV_OK;
}

/*******************************************************************************
* mvEthPhyAdvertiseSet -
*
* DESCRIPTION:
*	Set advertisement mode
*
* INPUT:
*       phyAddr - Phy address.
*	advertise - 	0x1: 10 half
*			0x2: 10 full
*			0x4: 100 half
*			0x8: 100 full
*			0x10: 1000 half
*			0x20: 1000 full
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_STATUS mvEthPhyAdvertiseSet(MV_U32 phyAddr, MV_U16 advertise)
{
	MV_U16 regVal, tmp;
	/* 10 - 100 */
	mvEthPhyRegRead(phyAddr, ETH_PHY_AUTONEGO_AD_REG, &regVal);

	regVal = regVal & (~ETH_PHY_10_100_BASE_ADVERTISE_MASK);
	tmp = (advertise & 0xf);
	regVal = regVal | (tmp << ETH_PHY_10_100_BASE_ADVERTISE_OFFSET);
	mvEthPhyRegWrite(phyAddr, ETH_PHY_AUTONEGO_AD_REG, regVal);
	/* 1000 */
	mvEthPhyRegRead(phyAddr, ETH_PHY_1000BASE_T_CTRL_REG, &regVal);
	/* keep only bits 4-5 + shift to the right */
	tmp = ((advertise >> 4) & 0x3);
	regVal = regVal & (~ETH_PHY_1000BASE_ADVERTISE_MASK);
	regVal = regVal | (tmp << ETH_PHY_1000BASE_ADVERTISE_OFFSET);
	mvEthPhyRegWrite(phyAddr, ETH_PHY_1000BASE_T_CTRL_REG, regVal);
	return MV_OK;
}

/*******************************************************************************
* mvEthPhyAdvertiseGet -
*
* DESCRIPTION:
*	Get advertisement mode
*
* INPUT:
*       phyAddr - Phy address.
*
* OUTPUT:
*       advertise - 	0x1: 10 half
*			0x2: 10 full
*			0x4: 100 half
*			0x8: 100 full
*			0x10: 1000 half
*			0x20: 1000 full
*
* RETURN:
*
*******************************************************************************/
MV_STATUS mvEthPhyAdvertiseGet(MV_U32 phyAddr, MV_U16 *advertise)
{
	MV_U16 regVal, tmp;

	if (advertise == NULL)
		return MV_BAD_PARAM;

	/* 10 - 100 */
	mvEthPhyRegRead(phyAddr, ETH_PHY_AUTONEGO_AD_REG, &regVal);
	tmp = ((regVal & ETH_PHY_10_100_BASE_ADVERTISE_MASK) >> ETH_PHY_10_100_BASE_ADVERTISE_OFFSET);
	/* 1000 */
	mvEthPhyRegRead(phyAddr, ETH_PHY_1000BASE_T_CTRL_REG, &regVal);
	tmp |= (((regVal & ETH_PHY_1000BASE_ADVERTISE_MASK) >> ETH_PHY_1000BASE_ADVERTISE_OFFSET) << 4);
	*advertise = tmp;

	return MV_OK;
}

/*******************************************************************************
* mvEthE1111PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE1111PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Phy recv and tx delay */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 20, &reg);
	reg |= BIT1 | BIT7;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 20, reg);

	/* Leds link and activity*/
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 24, 0x4111);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);

	if (ethphyHalData.boardSpecInit == MV_TRUE)
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum],
				ethphyHalData.specRegOff , ethphyHalData.specData);

}

/*******************************************************************************
* mvEthE1112PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE1112PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Set phy address */
	/*MV_REG_WRITE(ETH_PHY_ADDR_REG(ethPortNum), mvBoardPhyAddrGet(ethPortNum));*/

	/* Implement PHY errata */
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 2);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, 0x140);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, 0x8140);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 3);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, 0x103);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);

}
/*******************************************************************************
* mvEthE1310PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE1310PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Leds link and activity*/
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0x3);
	/*mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 16, &reg);
	reg &= ~0xFF;*/
	reg	= 0x0083;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, reg);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0x0);

	/* Set RGMII delay */
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 2);
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 21, &reg);
	reg	|= (BIT5 | BIT4);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 21, reg);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);
}
/*******************************************************************************
* mvEthE1116PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE1116PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Leds link and activity*/
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0x3);
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 16, &reg);
	reg &= ~0xf;
	reg |= 0x1;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, reg);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0x0);

	/* Set RGMII delay */
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 2);
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 21, &reg);
	reg	|= (BIT5 | BIT4);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 21, reg);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);
}


/*******************************************************************************
* mvEthE3016PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE3016PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Leds link and activity*/
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 22, &reg);
	reg &= ~0xf;
	reg	|= 0xa;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, reg);

	/* Set RGMII (RX) delay and copper mode */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 28, &reg);
	reg &= ~(BIT3 | BIT10 | BIT11);
	reg	|= (BIT10);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 28, reg);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);
}


/*******************************************************************************
* mvEthE1011PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE1011PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Phy recv and tx delay */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 20, &reg);
	reg &= ~(BIT1 | BIT7);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 20, reg);

	/* Leds link and activity*/
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 24, 0x4111);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);

}

/*******************************************************************************
* mvEthE1112PhyPowerDown -
*
* DESCRIPTION:
*	Power down the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID	mvEthE1112PhyPowerDown(MV_U32 ethPortNum)
{
	mvEthPhyPower(ethPortNum, MV_FALSE);
}

/*******************************************************************************
* mvEthE1112PhyPowerUp -
*
* DESCRIPTION:
*	Power up the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID	mvEthE1112PhyPowerUp(MV_U32 ethPortNum)
{
	mvEthPhyPower(ethPortNum, MV_TRUE);
}

/*******************************************************************************
* mvEthPhyPower -
*
* DESCRIPTION:
*	Do a basic power down/up to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*	enable - MV_TRUE - power up
*		 MV_FALSE - power down
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
static MV_VOID	mvEthPhyPower(MV_U32 ethPortNum, MV_BOOL enable)
{
	MV_U16 reg;
	if (enable == MV_FALSE) {
	/* Power down command */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 2); 		/* select page 2 */
		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 16, &reg);
		reg |= BIT3;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, reg);		/* select to disable the SERDES */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0); 		/* select page 0 */

		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 3);		/* Power off LED's */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, 0x88);
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, &reg);
		reg |= ETH_PHY_CTRL_RESET_MASK;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, reg);	/* software reset */
		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, &reg);
		reg |= ETH_PHY_CTRL_POWER_DOWN_MASK;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, reg);	/* power down the PHY */
	} else {
	/* Power up command */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 2); 		/* select page 2 */
		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 16, &reg);
		reg &= ~BIT3;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, reg);		/* select to enable the SERDES */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0); 		/* select page 0 */

		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 3);		/* Power on LED's */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, 0x03);
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, &reg);
		reg |= ETH_PHY_CTRL_RESET_MASK;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, reg);	/* software reset */
		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, &reg);
		reg &= ~ETH_PHY_CTRL_POWER_DOWN_MASK;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, reg);	/* power up the PHY */
	}
}


/*******************************************************************************
* mvEth1145PhyInit - Initialize MARVELL 1145 Phy
*
* DESCRIPTION:
*
* INPUT:
*       phyAddr - Phy address.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvEth1145PhyBasicInit(MV_U32 port)
{
    MV_U16 value;

    /* Set Link1000 output pin to be link indication, set Tx output pin to be activity */
    mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x18, ETH_PHY_LED_ACT_LNK_DV);
    mvOsDelay(10);

	    /* Add delay to RGMII Tx and Rx */
    mvEthPhyRegRead(ethphyHalData.phyAddr[port], 0x14, &value);
    mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x14, (value | BIT1 | BIT7));
    mvOsDelay(10);

    /* Set Phy TPVL to 0 */
    mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x10, 0x60);
    mvOsDelay(10);

    /* Reset Phy */
    mvEthPhyRegRead(ethphyHalData.phyAddr[port], 0x00, &value);
    mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x00, (value | BIT15));
    mvOsDelay(10);

    return;
}


/*******************************************************************************
* mvEthSgmiiToCopperPhyInit - Initialize Test board 1112 Phy
*
* DESCRIPTION:
*
* INPUT:
*       phyAddr - Phy address.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvEthSgmiiToCopperPhyBasicInit(MV_U32 port)
{
    MV_U16 value;
    MV_U16 phyAddr = 0xC;

   /* Port 0 phyAdd c */
   /* Port 1 phyAdd d */
    mvEthPhyRegWrite(phyAddr + port, 22, 3);
    mvEthPhyRegWrite(phyAddr + port, 16, 0x103);
    mvEthPhyRegWrite(phyAddr + port, 22, 0);

		/* reset the phy */
    mvEthPhyRegRead(phyAddr + port, 0, &value);
    value |= BIT15;
    mvEthPhyRegWrite(phyAddr + port, 0, value);
}


MV_VOID mvEth1121PhyBasicInit(MV_U32 port)
{
	MV_U16 value;
	MV_U16 phyAddr = ethphyHalData.phyAddr[port];

	/* Change page select to 2 */
	value = 2;
	mvEthPhyRegWrite(phyAddr, 22, value);
	mvOsDelay(10);

	/* Set RGMII rx delay */
	mvEthPhyRegRead(phyAddr, 21, &value);
	value |= BIT5;
	mvEthPhyRegWrite(phyAddr, 21, value);
	mvOsDelay(10);

	/* Change page select to 0 */
	value = 0;
	mvEthPhyRegWrite(phyAddr, 22, value);
	mvOsDelay(10);

	/* reset the phy */
	mvEthPhyRegRead(phyAddr, 0, &value);
	value |= BIT15;
	mvEthPhyRegWrite(phyAddr, 0, value);
	mvOsDelay(10);
}
MV_VOID mvEthInternalGEPhyBasicInit(MV_U32 ethPortNum)
{
}

MV_VOID mvEth1540Y0PhyBasicInit(MV_U32 ethPortNum, MV_BOOL eeeEnable)
{
	MV_U16 reg;
	MV_U16 i = ethphyHalData.phyAddr[ethPortNum];
	MV_U16 LinkCryptPortAddr = ethphyHalData.LinkCryptPortAddr[ethPortNum];
	/* Set page to 0. */
	mvEthPhyRegWrite(i, 0x16, 0);

	/* Phy C_ANEG */
	mvEthPhyRegRead(i, 0x4, &reg);
	reg |= 0x1E0;
	mvEthPhyRegWrite(i, 0x4, reg);

	/* Set page to 1. */
	mvEthPhyRegWrite(i, 0x16, 1);

	/* Enable SGMII AN */
	mvEthPhyRegWrite(i, 0x0, 0x1140);

	/* Set page to 3. */
	mvEthPhyRegWrite(i, 0x16, 3);

	/* LED setting */
	mvEthPhyRegWrite(i, 0x10, 0x8083);

	/* Set page to 0. */
	mvEthPhyRegWrite(i, 0x16, 0);

	if (eeeEnable) {
		/* set ELU#0 default match */
		/* RW U1 P0 R22 H0010 */
		mvEthPhyRegWrite(i, 0x16, 0x10);
		/* RW U1 P0 R1 H0104 */
		mvEthPhyRegWrite(i, 0x1, 0x0104);
		/* RW U1 P0 R2 h0000 */
		mvEthPhyRegWrite(i, 0x2, 0x0000);
		/* RW U1 P0 R3 H4000 */
		mvEthPhyRegWrite(i, 0x3, 0x4000);
		/* RW U1 P0 R1 H0904 */
		mvEthPhyRegWrite(i, 0x1, 0x0904);
		/* RW U1 P0 R2 h0000 */
		mvEthPhyRegWrite(i, 0x2, 0x0000);
		/* RW U1 P0 R3 H4000 */
		mvEthPhyRegWrite(i, 0x3, 0x4000);
		/* RW U1 P0 R1 H1104 */
		mvEthPhyRegWrite(i, 0x1, 0x1104);
		/* RW U1 P0 R2 h0000 */
		mvEthPhyRegWrite(i, 0x2, 0x0000);
		/* RW U1 P0 R3 H4000 */
		mvEthPhyRegWrite(i, 0x3, 0x4000);
		/* RW U1 P0 R1 H1904 */
		mvEthPhyRegWrite(i, 0x1, 0x1904);
		/* RW U1 P0 R2 h0000 */
		mvEthPhyRegWrite(i, 0x2, 0x0000);
		/* RW U1 P0 R3 H4000 */
		mvEthPhyRegWrite(i, 0x3, 0x4000);
		/* RW U1 P0 R22 H0000 */
		mvEthPhyRegWrite(i, 0x16, 0x0000);

		/* set ILU#0 default match */
		/* RW U1 P0 R22 H0010 */
		mvEthPhyRegWrite(i, 0x16, 0x0010);
		/* RW U1 P0 R1 H0207 */
		mvEthPhyRegWrite(i, 0x1, 0x0207);
		/* RW U1 P0 R2 h4000 */
		mvEthPhyRegWrite(i, 0x2, 0x4000);
		/* RW U1 P0 R3 H0000 */
		mvEthPhyRegWrite(i, 0x3, 0x0000);
		/* RW U1 P0 R1 H0A07 */
		mvEthPhyRegWrite(i, 0x1, 0x0a07);
		/* RW U1 P0 R2 h4000 */
		mvEthPhyRegWrite(i, 0x2, 0x4000);
		/* RW U1 P0 R3 H0000 */
		mvEthPhyRegWrite(i, 0x3, 0x0000);
		/* RW U1 P0 R1 H1207 */
		mvEthPhyRegWrite(i, 0x1, 0x1207);
		/* RW U1 P0 R2 h4000 */
		mvEthPhyRegWrite(i, 0x2, 0x4000);
		/* RW U1 P0 R3 H0000 */
		mvEthPhyRegWrite(i, 0x3, 0x0000);
		/* RW U1 P0 R1 H1A07 */
		mvEthPhyRegWrite(i, 0x1, 0x1a07);
		/* RW U1 P0 R2 h4000 */
		mvEthPhyRegWrite(i, 0x2, 0x4000);
		/* RW U1 P0 R3 H0000 */
		mvEthPhyRegWrite(i, 0x3, 0x0000);
		/* RW U1 P0 R22 H0000 */
		mvEthPhyRegWrite(i, 0x16, 0x0000);

		/* change the wiremac ipg from 12 to 11 */
		/* RW U1 P0 R22 H0010 */
		mvEthPhyRegWrite(i, 0x16, 0x0010);
		/* RW U1 P0 R1 H0041 */
		mvEthPhyRegWrite(i, 0x1, 0x0041);
		/* RW U1 P0 R2 h00b1 */
		mvEthPhyRegWrite(i, 0x2, 0x00b1);
		/* RW U1 P0 R3 H0002 */
		mvEthPhyRegWrite(i, 0x3, 0x0002);
		/* RW U1 P0 R1 H0841 */
		mvEthPhyRegWrite(i, 0x1, 0x0841);
		/* RW U1 P0 R2 h00b1 */
		mvEthPhyRegWrite(i, 0x2, 0x00b1);
		/* RW U1 P0 R3 H0002 */
		mvEthPhyRegWrite(i, 0x3, 0x0002);
		/* RW U1 P0 R1 H1041 */
		mvEthPhyRegWrite(i, 0x1, 0x1041);
		/* RW U1 P0 R2 h00b1 */
		mvEthPhyRegWrite(i, 0x2, 0x00b1);
		/* RW U1 P0 R3 H0002 */
		mvEthPhyRegWrite(i, 0x3, 0x0002);
		/* RW U1 P0 R1 H1841 */
		mvEthPhyRegWrite(i, 0x1, 0x1841);
		/* RW U1 P0 R2 h00b1 */
		mvEthPhyRegWrite(i, 0x2, 0x00b1);
		/* RW U1 P0 R3 H0002 */
		mvEthPhyRegWrite(i, 0x3, 0x0002);
		/* RW U1 P0 R22 H0000 */
		mvEthPhyRegWrite(i, 0x16, 0x0000);


		/* change the setting to not drop badtag */
		/* RW U1 P0 R22 H0010 */
		mvEthPhyRegWrite(i, 0x16, 0x0010);
		/* RW U1 P0 R1 H000b */
		mvEthPhyRegWrite(i, 0x1, 0x000b);
		/* RW U1 P0 R2 h0000 */
		mvEthPhyRegWrite(i, 0x2, 0x0000);
		/* RW U1 P0 R3 H0fb4 */
		mvEthPhyRegWrite(i, 0x3, 0x0fb4);
		/* RW U1 P0 R1 H080b */
		mvEthPhyRegWrite(i, 0x1, 0x080b);
		/* RW U1 P0 R2 h0000 */
		mvEthPhyRegWrite(i, 0x2, 0x0000);
		/* RW U1 P0 R3 H0fb4 */
		mvEthPhyRegWrite(i, 0x3, 0x0fb4);
		/* RW U1 P0 R1 H100b */
		mvEthPhyRegWrite(i, 0x1, 0x100b);
		/* RW U1 P0 R2 h0000 */
		mvEthPhyRegWrite(i, 0x2, 0x0000);
		/* RW U1 P0 R3 H0fb4 */
		mvEthPhyRegWrite(i, 0x3, 0x0fb4);
		/* RW U1 P0 R1 H180b */
		mvEthPhyRegWrite(i, 0x1, 0x180b);
		/* RW U1 P0 R2 h0000 */
		mvEthPhyRegWrite(i, 0x2, 0x0000);
		/* RW U1 P0 R3 H0fb4 */
		mvEthPhyRegWrite(i, 0x3, 0x0fb4);
		/* RW U1 P0 R22 H0000 */
		mvEthPhyRegWrite(i, 0x16, 0x0000);

		/* set MACSec EEE Entry/Exit Timer */
		/* RW U1 P0 R22 H0010 */
		mvEthPhyRegWrite(i, 0x16, 0x0010);
		/* RW U1 P0 R1 H03C0 */
		mvEthPhyRegWrite(i, 0x3, 0x03c0);
		/* RW U1 P0 R2 H111E */
		mvEthPhyRegWrite(i, 0x3, 0x111e);
		/* RW U1 P0 R3 H111E */
		mvEthPhyRegWrite(i, 0x3, 0x111e);
		/* RW U1 P0 R1 H0BC0 */
		mvEthPhyRegWrite(i, 0x3, 0x0bc0);
		/* RW U1 P0 R2 H111E */
		mvEthPhyRegWrite(i, 0x3, 0x111e);
		/* RW U1 P0 R3 H111E */
		mvEthPhyRegWrite(i, 0x3, 0x111e);
		/* RW U1 P0 R1 H13C0 */
		mvEthPhyRegWrite(i, 0x3, 0x13c0);
		/* RW U1 P0 R2 H111E */
		mvEthPhyRegWrite(i, 0x3, 0x111e);
		/* RW U1 P0 R3 H111E */
		mvEthPhyRegWrite(i, 0x3, 0x111e);
		/* RW U1 P0 R1 H1BC0 */
		mvEthPhyRegWrite(i, 0x3, 0x1bc0);
		/* RW U1 P0 R2 H111E */
		mvEthPhyRegWrite(i, 0x3, 0x111e);
		/* RW U1 P0 R3 H111E */
		mvEthPhyRegWrite(i, 0x3, 0x111e);
		/* RW U1 P0 R22 H0000 */
		mvEthPhyRegWrite(i, 0x16, 0x0000);

		/* Start of EEE Workaround */
		/* RW U1 P0-3 R22 H00FB */
		mvEthPhyRegWrite(i, 0x16, 0x00FB);
		/* RW U1 P0-3 R11 H1120 */
		mvEthPhyRegWrite(i, 0xB , 0x1120);
		/* RW U1 P0-3 R8  H3666 */
		mvEthPhyRegWrite(i, 0x8 , 0x3666);
		/* RW U1 P0-3 R22 H00FF */
		mvEthPhyRegWrite(i, 0x16, 0x00FF);
		/* RW U1 P0-3 R17 H0F0C */
		mvEthPhyRegWrite(i, 0x11, 0x0F0C);
		/* RW U1 P0-3 R16 H2146 */
		mvEthPhyRegWrite(i, 0x10, 0x2146);
		/* RW U1 P0-3 R17 Hc090 */
		mvEthPhyRegWrite(i, 0x11, 0xC090);
		/* RW U1 P0-3 R16 H2147 */
		mvEthPhyRegWrite(i, 0x10, 0x2147);
		/* RW U1 P0-3 R17 H0000 */
		mvEthPhyRegWrite(i, 0x11, 0x0000);
		/* RW U1 P0-3 R16 H2000 */
		mvEthPhyRegWrite(i, 0x10, 0x2000);
		/* RW U1 P0-3 R17 H6000 */
		mvEthPhyRegWrite(i, 0x11, 0x6000);
		/* RW U1 P0-3 R16 H2143 */
		mvEthPhyRegWrite(i, 0x10, 0x2143);
		/* RW U1 P0-3 R17 HC004 */
		mvEthPhyRegWrite(i, 0x11, 0xC004);
		/* RW U1 P0-3 R16 H2100 */
		mvEthPhyRegWrite(i, 0x10, 0x2100);
		/* RW U1 P0-3 R17 H49E8 */
		mvEthPhyRegWrite(i, 0x11, 0x49E8);
		/* RW U1 P0-3 R16 H2144 */
		mvEthPhyRegWrite(i, 0x10, 0x2144);
		/* RW U1 P0-3 R17 H3180 */
		mvEthPhyRegWrite(i, 0x11, 0x3180);
		/* RW U1 P0-3 R16 H2148 */
		mvEthPhyRegWrite(i, 0x10, 0x2148);
		/* RW U1 P0-3 R17 HFC44 */
		mvEthPhyRegWrite(i, 0x11, 0xFC44);
		/* RW U1 P0-3 R16 H214B */
		mvEthPhyRegWrite(i, 0x10, 0x214B);
		/* RW U1 P0-3 R17 H7FD2 */
		mvEthPhyRegWrite(i, 0x11, 0x7FD2);
		/* RW U1 P0-3 R16 H214C */
		mvEthPhyRegWrite(i, 0x10, 0x214C);
		/* RW U1 P0-3 R17 H2240 */
		mvEthPhyRegWrite(i, 0x11, 0x2240);
		/* RW U1 P0-3 R16 H214D */
		mvEthPhyRegWrite(i, 0x10, 0x214D);
		/* RW U1 P0-3 R17 H3008 */
		mvEthPhyRegWrite(i, 0x11, 0x3008);
		/* RW U1 P0-3 R16 H214E */
		mvEthPhyRegWrite(i, 0x10, 0x214E);
		/* RW U1 P0-3 R17 H3DF0 */
		mvEthPhyRegWrite(i, 0x11, 0x3DF0);
		/* RW U1 P0-3 R16 H214F */
		mvEthPhyRegWrite(i, 0x10, 0x214F);
		/* RW U1 P0-3 R22 H0000 */
		mvEthPhyRegWrite(i, 0x16, 0);

		/* Enable EEE Auto-neg advertisement on P0-P7 ports */
		/* RW U1 P0-3 R13 H0007 */
		mvEthPhyRegWrite(i, 0xD , 0x0007);
		/* RW U1 P0-3 R14 H003C */
		mvEthPhyRegWrite(i, 0xE , 0x003C);
		/* RW U1 P0-3 R13 H4007 */
		mvEthPhyRegWrite(i, 0xD , 0x4007);
		/* RW U1 P0-3 R14 H0006 */
		mvEthPhyRegWrite(i, 0xE , 0x0006);

		/* Soft-Reset on P0-P7 ports */
		/* RW U1 P0-3 R22 H0000 */
		mvEthPhyRegWrite(i, 0x16, 0);
		/* RW U1 P0-3 R0  H9140 */
		mvEthPhyRegWrite(i, 0x0 , 0x9140);

		/* Enable MACsec EEE Master Mode on P0-3 ports */
		/* RW U1 P0 R22 H0010 */
		mvEthPhyRegWrite(i, 0x16, 0x0010);
		/* RW U1 P0 R1  H03C1 */
		mvEthPhyRegWrite(i, 0x1 , 0x03C1);
		/* RW U1 P0 R2  H0001 */
		mvEthPhyRegWrite(i, 0x2 , 0x0001);
		/* RW U1 P0 R3  H0000 */
		mvEthPhyRegWrite(i, 0x3 , 0x0000);
		/* RW U1 P0 R1  H0BC1 */
		mvEthPhyRegWrite(i, 0x1 , 0x0BC1);
		/* RW U1 P0 R2  H0001 */
		mvEthPhyRegWrite(i, 0x2 , 0x0001);
		/* RW U1 P0 R3  H0000 */
		mvEthPhyRegWrite(i, 0x3 , 0x0000);
		/* RW U1 P0 R1  H13C1 */
		mvEthPhyRegWrite(i, 0x1 , 0x13C1);
		/* RW U1 P0 R2  H0001 */
		mvEthPhyRegWrite(i, 0x2 , 0x0001);
		/* RW U1 P0 R3  H0000 */
		mvEthPhyRegWrite(i, 0x3 , 0x0000);
		/* RW U1 P0 R1  H1BC1 */
		mvEthPhyRegWrite(i, 0x1 , 0x1BC1);
		/* RW U1 P0 R2  H0001 */
		mvEthPhyRegWrite(i, 0x2 , 0x0001);
		/* RW U1 P0 R3  H0000 */
		mvEthPhyRegWrite(i, 0x3 , 0x0000);
		/* RW U1 P0 R22 H0000 */
		mvEthPhyRegWrite(i, 0x16, 0x0000);
	}
	/*  set ELU#0 default match   */
	/*  RW U1 P0 R22 H0010  */
	/*  RW U1 P0 R1 H0104 */
	/*  RW U1 P0 R2 h0000 */
	/*  RW U1 P0 R3 H4000 */
	/*  RW U1 P0 R1 H0904 */
	/*  RW U1 P0 R2 h0000 */
	/*  RW U1 P0 R3 H4000 */
	/*  RW U1 P0 R1 H1104 */
	/*  RW U1 P0 R2 h0000 */
	/*  RW U1 P0 R3 H4000 */
	/*  RW U1 P0 R1 H1904 */
	/*  RW U1 P0 R2 h0000 */
	/*  RW U1 P0 R3 H4000 */
	/*  RW U1 P0 R22 H0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);

	/*  set ILU#0 default match  */
	/*  RW U1 P0 R22 H0010 */
	mvEthPhyRegWrite(i, 0x16, 0x0010);
	/*  RW U1 P0 R1 H0207 */
	/*  RW U1 P0 R2 h4000 */
	/*  RW U1 P0 R3 H0000 */
	/*  RW U1 P0 R1 H0A07 */
	/*  RW U1 P0 R2 h4000 */
	/*  RW U1 P0 R3 H0000 */
	/*  RW U1 P0 R1 H1207 */
	/*  RW U1 P0 R2 h4000 */
	/*  RW U1 P0 R3 H0000 */
	/*  RW U1 P0 R1 H1A07 */
	/*  RW U1 P0 R2 h4000 */
	/*  RW U1 P0 R3 H0000 */
	/*  RW U1 P0 R22 H0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);

	/*  change the wiremac ipg from 12 to 11   */
	/*  RW U1 P0 R22 H0010 */
	mvEthPhyRegWrite(i, 0x16, 0x0010);
	/*  RW U1 P0 R1 H0041 */
	/*  RW U1 P0 R2 h00b1 */
	/*  RW U1 P0 R3 H0002 */
	/*  RW U1 P0 R1 H0841 */
	/*  RW U1 P0 R2 h00b1 */
	/*  RW U1 P0 R3 H0002 */
	/*  RW U1 P0 R1 H1041 */
	/*  RW U1 P0 R2 h00b1 */
	/*  RW U1 P0 R3 H0002 */
	/*  RW U1 P0 R1 H1841 */
	/*  RW U1 P0 R2 h00b1 */
	/*  RW U1 P0 R3 H0002 */
	/*  RW U1 P0 R22 H0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);


	/*  change the setting to not drop badtag   */
	/*  RW U1 P0 R22 H0010 */
	mvEthPhyRegWrite(i, 0x16, 0x0010);
	/*  RW U1 P0 R1 H000b */
	/*  RW U1 P0 R2 h0000 */
	/*  RW U1 P0 R3 H0fb4 */
	/*  RW U1 P0 R1 H080b */
	/*  RW U1 P0 R2 h0000 */
	/*  RW U1 P0 R3 H0fb4 */
	/*  RW U1 P0 R1 H100b */
	/*  RW U1 P0 R2 h0000 */
	/*  RW U1 P0 R3 H0fb4 */
	/*  RW U1 P0 R1 H180b */
	/*  RW U1 P0 R2 h0000 */
	/*  RW U1 P0 R3 H0fb4 */
	/*  RW U1 P0 R22 H0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);

	/*  set MACSec EEE Entry/Exit Timer  */
	/*  RW U1 P0 R22 H0010 */
	mvEthPhyRegWrite(i, 0x16, 0x0010);
	/*  RW U1 P0 R1 H03C0 */
	/*  RW U1 P0 R2 H111E */
	/*  RW U1 P0 R3 H111E */
	/*  RW U1 P0 R1 H0BC0 */
	/*  RW U1 P0 R2 H111E */
	/*  RW U1 P0 R3 H111E */
	/*  RW U1 P0 R1 H13C0 */
	/*  RW U1 P0 R2 H111E */
	/*  RW U1 P0 R3 H111E */
	/*  RW U1 P0 R1 H1BC0 */
	/*  RW U1 P0 R2 H111E */
	/*  RW U1 P0 R3 H111E */
	/*  RW U1 P0 R22 H0000 */
	/*  RW U1 P0-3 R22 H00FB */
	/*  RW U1 P0-3 R11 H1120 */
	/*  RW U1 P0-3 R8  H3666 */
	/*  RW U1 P0-3 R22 H00FF */
	/*  RW U1 P0-3 R17 H0F0C */
	/*  RW U1 P0-3 R16 H2146 */
	/*  RW U1 P0-3 R17 Hc090 */
	/*  RW U1 P0-3 R16 H2147 */
	/*  RW U1 P0-3 R17 H0000 */
	/*  RW U1 P0-3 R16 H2000 */
	/*  RW U1 P0-3 R17 H6000 */
	/*  RW U1 P0-3 R16 H2143 */
	/*  RW U1 P0-3 R17 HC004 */
	/*  RW U1 P0-3 R16 H2100 */
	/*  RW U1 P0-3 R17 H49E8 */
	/*  RW U1 P0-3 R16 H2144 */
	/*  RW U1 P0-3 R17 H3180 */
	/*  RW U1 P0-3 R16 H2148 */
	/*  RW U1 P0-3 R17 HFC44 */
	/*  RW U1 P0-3 R16 H214B */
	/*  RW U1 P0-3 R17 H7FD2 */
	/*  RW U1 P0-3 R16 H214C */
	/*  RW U1 P0-3 R17 H2240 */
	/*  RW U1 P0-3 R16 H214D */
	/*  RW U1 P0-3 R17 H3008 */
	/*  RW U1 P0-3 R16 H214E */
	/*  RW U1 P0-3 R17 H3DF0 */
	/*  RW U1 P0-3 R16 H214F */
	/*  RW U1 P0-3 R22 H0000 */
	/*  RW U1 P0-3 R13 H0007 */
	/*  RW U1 P0-3 R14 H003C */
	/*  RW U1 P0-3 R13 H4007 */
	/*  RW U1 P0-3 R14 H0006 */
	/*  RW U1 P0-3 R22 H0000 */
	/*  RW U1 P0-3 R0  H9140 */
	/*  RW U1 P0 R22 H0010 */
	/*  RW U1 P0 R1  H03C1 */
	/*  RW U1 P0 R2  H0001 */
	/*  RW U1 P0 R3  H0000 */
	/*  RW U1 P0 R1  H0BC1 */
	/*  RW U1 P0 R2  H0001 */
	/*  RW U1 P0 R3  H0000 */
	/*  RW U1 P0 R1  H13C1 */
	/*  RW U1 P0 R2  H0001 */
	/*  RW U1 P0 R3  H0000 */
	/*  RW U1 P0 R1  H1BC1 */
	/*  RW U1 P0 R2  H0001 */
	/*  RW U1 P0 R3  H0000 */
	/*  RW U1 P0 R22 H0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);

	/* Change MRU to Max : Page 16 */
	mvEthPhyRegWrite(i, 0x16, 0x0010);

	/* Write LinkCrypt Address */
	mvEthPhyRegWrite(i, 0x1, LinkCryptPortAddr+0x0040);

	/* Write LinkCrypt Data Low */
	mvEthPhyRegWrite(i, 0x2, 0xFFF9);

	/* Write LinkCrypt Data High */
	mvEthPhyRegWrite(i, 0x3, 0x1);

	mvEthPhyRegWrite(i, 0x16, 0x0000);

	/* SGMII fix*/
	mvEthPhyRegWrite(i, 0x16, 0x12);
	mvEthPhyRegWrite(i, 0x1b, 0x0);
	mvEthPhyRegWrite(i, 0x16, 0x0);


	/* Power up the phy */
	mvEthPhyRegRead(i, ETH_PHY_CTRL_REG, &reg);
	reg &= ~(ETH_PHY_CTRL_POWER_DOWN_MASK);
	mvEthPhyRegWrite(i, ETH_PHY_CTRL_REG, reg);
	/*mvOsDelay(100);*/
	/* Sleep 3000 */
	mvOsDelay(300);

}
static int initJumboPackets=0;
MV_VOID mvEth1540A1PhyBasicInit(MV_U32 ethPortNum, MV_BOOL eeeEnable)
{
	int i = ethphyHalData.phyAddr[ethPortNum];
	MV_U16 reg;
	int port0 = ethphyHalData.QuadPhyPort0[ethPortNum];

	/* Enable QSGMII AN */
	/* Set page to 4. */
	mvEthPhyRegWrite(i, 0x16, 4);
	/* Enable AN */
	mvEthPhyRegWrite(i, 0x0, 0x1140);
		/* Set page to 0. */
	mvEthPhyRegWrite(i, 0x16, 0);

	/* Phy C_ANEG */
	mvEthPhyRegRead(i, 0x4, &reg);
	reg |= 0x1E0;
	mvEthPhyRegWrite(i, 0x4, reg);

	/*A1 drop bad tag*/
	mvEthPhyRegWrite(i, 22, 0x0010);
	mvEthPhyRegWrite(i, 1,(ethPortNum * 0x100)+0x000B);
	mvEthPhyRegWrite(i, 2, 0x0000);
	mvEthPhyRegWrite(i, 3, 0x0FB4);
	mvEthPhyRegWrite(i, 22, 0x0000);

	/* Enable EEE_Auto-neg for 1000BASE-T and 100BASE-TX */
	mvEthPhyRegWrite(i, 22, 0x0000);
	mvEthPhyRegWrite(i, 13, 0x0007);
	mvEthPhyRegWrite(i, 14, 0x003C);
	mvEthPhyRegWrite(i, 13, 0x4007);
	mvEthPhyRegWrite(i, 14, 0x0006);

	/* phy initialization */
	mvEthPhyRegWrite(i, 22, 0x00FF);
	mvEthPhyRegWrite(i, 17, 0x2148);
	mvEthPhyRegWrite(i, 16, 0x2144);
	mvEthPhyRegWrite(i, 17, 0xDC0C);
	mvEthPhyRegWrite(i, 16, 0x2159);
	mvEthPhyRegWrite(i, 22, 0x00FF);

	/* Enable MACSec (Reg 27_2.13 = '1') */
	mvEthPhyRegWrite(i, 22, 0x0012);
	mvEthPhyRegRead(i, 27, &reg);
	reg |= (1 << 13);
	mvEthPhyRegWrite(i, 27, reg);

	if (eeeEnable == MV_TRUE) {
		/* Enable EEE Master (Legacy) Mode */
		mvEthPhyRegWrite(i, 22, 0x0010);
		mvEthPhyRegWrite(i, 1, 0x03c1);
		mvEthPhyRegWrite(i, 2, 0x0001);
		mvEthPhyRegWrite(i, 3, 0x0000);
		mvEthPhyRegWrite(i, 1, 0x0bc1);
		mvEthPhyRegWrite(i, 2, 0x0001);
		mvEthPhyRegWrite(i, 3, 0x0000);
		mvEthPhyRegWrite(i, 1, 0x13c1);
		mvEthPhyRegWrite(i, 2, 0x0001);
		mvEthPhyRegWrite(i, 3, 0x0000);
		mvEthPhyRegWrite(i, 1, 0x1bc1);
		mvEthPhyRegWrite(i, 2, 0x0001);
		mvEthPhyRegWrite(i, 3, 0x0000);
		mvEthPhyRegWrite(i, 22, 0x0000);
	}
/* Configuring the PHY for jumbo packets  */
	mvEthPhyRegWrite(i, 0x16, 0x02);
	mvEthPhyRegRead(i, 0x10, &reg);
	reg |= (1<<14) | (1<<15);
	mvEthPhyRegWrite(i, 0x10, reg);
	mvEthPhyRegWrite(i, 0x16, 0x0);

	DB2(printf("%s: configure jumbo QuadPhyPort=0x%x offset = 0x%x\n",__FUNCTION__,port0, offs));
	if (0 == initJumboPackets) {
		int j;
		initJumboPackets = 1;

		mvEthPhyRegWrite(port0, 0x16, 0x10);
		/* assume we want all ports to support jumbo*/
		for(j = 0 ; j < MV_ETH_MAX_PORTS; j++)
		{
			mvEthPhyRegWrite(port0, 0x01, ((0x800*j)+0x40)) ;
			mvEthPhyRegWrite(port0, 0x02, 0xFFF9);
			mvEthPhyRegWrite(port0, 0x03, 0x1);

			mvEthPhyRegWrite(port0, 0x01,  ((0x800*j)+0x50));
			mvEthPhyRegWrite(port0, 0x02, 0xFFF9);
			mvEthPhyRegWrite(port0, 0x03, 0x1);
		}
		mvEthPhyRegWrite(port0, 0x16, 0x0);
	}

	/* Configure LED */
	mvEthPhyRegWrite(i, 22, 3);
	mvEthPhyRegWrite(i, 16, 0x1111);


	/* Soft-Reset */
	mvEthPhyRegWrite(i, 22, 0x0000);
	mvEthPhyRegWrite(i, 0, 0x9140);

	/* Power up the phy */
	mvEthPhyRegRead(i, ETH_PHY_CTRL_REG, &reg);
	reg &= ~(ETH_PHY_CTRL_POWER_DOWN_MASK);
	mvEthPhyRegWrite(i, ETH_PHY_CTRL_REG, reg);

	mvOsDelay(100);
}


MV_VOID mvEth1540A0PhyBasicInit(MV_U32 ethPortNum, MV_BOOL eeeEnable)
{
	int i = ethphyHalData.phyAddr[ethPortNum];
	MV_U16 reg;
	int port0 = ethphyHalData.QuadPhyPort0[ethPortNum];
/*	int offs = ethphyHalData.LinkCryptPortAddr[ethPortNum]; */

	/* Enable QSGMII AN */
	/* Set page to 4. */
	mvEthPhyRegWrite(i, 0x16, 4);
	/* Enable AN */
	mvEthPhyRegWrite(i, 0x0, 0x1140);
	/* Set page to 0. */
	mvEthPhyRegWrite(i, 0x16, 0);

	/* Phy C_ANEG */
	mvEthPhyRegRead(i, 0x4, &reg);
	reg |= 0x1E0;
	mvEthPhyRegWrite(i, 0x4, reg);

	/* Set page to 1. */
	mvEthPhyRegWrite(i, 0x16, 1);

	/* Disable Drop BadTag */
	mvEthPhyRegWrite(i, 22, 0x0010);
	mvEthPhyRegWrite(i, 1, 0x000B);
	mvEthPhyRegWrite(i, 2, 0x0000);
	mvEthPhyRegWrite(i, 3, 0x0FB4);
	mvEthPhyRegWrite(i, 1, 0x080B);
	mvEthPhyRegWrite(i, 2, 0x0000);
	mvEthPhyRegWrite(i, 3, 0x0FB4);
	mvEthPhyRegWrite(i, 1, 0x100B);
	mvEthPhyRegWrite(i, 2, 0x0000);
	mvEthPhyRegWrite(i, 3, 0x0FB4);
	mvEthPhyRegWrite(i, 1, 0x180B);
	mvEthPhyRegWrite(i, 2, 0x0000);
	mvEthPhyRegWrite(i, 3, 0x0FB4);
	mvEthPhyRegWrite(i, 22, 0x0000);

	mvEthPhyRegWrite(i, 22, 0x00FA);
	mvEthPhyRegWrite(i, 8, 0x0010);

	mvEthPhyRegWrite(i, 22, 0x00FB);
	mvEthPhyRegWrite(i, 1, 0x4099);
	mvEthPhyRegWrite(i, 3, 0x1120);
	mvEthPhyRegWrite(i, 11, 0x113C);
	mvEthPhyRegWrite(i, 14, 0x8100);
	mvEthPhyRegWrite(i, 15, 0x112A);

	mvEthPhyRegWrite(i, 22, 0x00FC);
	mvEthPhyRegWrite(i, 1, 0x20B0);

	mvEthPhyRegWrite(i, 22, 0x00FF);
	mvEthPhyRegWrite(i, 17, 0x0000);
	mvEthPhyRegWrite(i, 16, 0x2000);
	mvEthPhyRegWrite(i, 17, 0x4444);
	mvEthPhyRegWrite(i, 16, 0x2140);
	mvEthPhyRegWrite(i, 17, 0x8064);
	mvEthPhyRegWrite(i, 16, 0x2141);
	mvEthPhyRegWrite(i, 17, 0x0108);
	mvEthPhyRegWrite(i, 16, 0x2144);
	mvEthPhyRegWrite(i, 17, 0x0F16);
	mvEthPhyRegWrite(i, 16, 0x2146);
	mvEthPhyRegWrite(i, 17, 0x8C44);
	mvEthPhyRegWrite(i, 16, 0x214B);

	mvEthPhyRegWrite(i, 17, 0x0F90);
	mvEthPhyRegWrite(i, 16, 0x214C);
	mvEthPhyRegWrite(i, 17, 0xBA33);
	mvEthPhyRegWrite(i, 16, 0x214D);
	mvEthPhyRegWrite(i, 17, 0x39AA);
	mvEthPhyRegWrite(i, 16, 0x214F);
	mvEthPhyRegWrite(i, 17, 0x8433);
	mvEthPhyRegWrite(i, 16, 0x2151);
	mvEthPhyRegWrite(i, 17, 0x2010);
	mvEthPhyRegWrite(i, 16, 0x2152);
	mvEthPhyRegWrite(i, 17, 0x99EB);
	mvEthPhyRegWrite(i, 16, 0x2153);
	mvEthPhyRegWrite(i, 17, 0x2F3B);
	mvEthPhyRegWrite(i, 16, 0x2154);
	mvEthPhyRegWrite(i, 17, 0x584E);
	mvEthPhyRegWrite(i, 16, 0x2156);
	mvEthPhyRegWrite(i, 17, 0x1223);
	mvEthPhyRegWrite(i, 16, 0x2158);
	mvEthPhyRegWrite(i, 22, 0x0000);

	/* Enable EEE_Auto-neg for 1000BASE-T and 100BASE-TX */
	mvEthPhyRegWrite(i, 22, 0x0000);
	mvEthPhyRegWrite(i, 13, 0x0007);
	mvEthPhyRegWrite(i, 14, 0x003C);
	mvEthPhyRegWrite(i, 13, 0x4007);
	mvEthPhyRegWrite(i, 14, 0x0006);

	/* Enable MACSec (Reg 27_2.13 = '1') */
	mvEthPhyRegWrite(i, 22, 0x0012);
	mvEthPhyRegRead(i, 27, &reg);
	reg |= (1 << 13);
	mvEthPhyRegWrite(i, 27, reg);

	if (eeeEnable == MV_TRUE) {
		/* Enable EEE Master (Legacy) Mode */
		mvEthPhyRegWrite(i, 22, 0x0010);
		mvEthPhyRegWrite(i, 1, 0x03c1);
		mvEthPhyRegWrite(i, 2, 0x0001);
		mvEthPhyRegWrite(i, 3, 0x0000);
		mvEthPhyRegWrite(i, 1, 0x0bc1);
		mvEthPhyRegWrite(i, 2, 0x0001);
		mvEthPhyRegWrite(i, 3, 0x0000);
		mvEthPhyRegWrite(i, 1, 0x13c1);
		mvEthPhyRegWrite(i, 2, 0x0001);
		mvEthPhyRegWrite(i, 3, 0x0000);
		mvEthPhyRegWrite(i, 1, 0x1bc1);
		mvEthPhyRegWrite(i, 2, 0x0001);
		mvEthPhyRegWrite(i, 3, 0x0000);
		mvEthPhyRegWrite(i, 22, 0x0000);
	}
/* Configuring the PHY for jumbo packets  */
	mvEthPhyRegWrite(i, 0x16, 0x02);
	mvEthPhyRegRead(i, 0x10, &reg);
	reg |= (1<<14) | (1<<15);
	mvEthPhyRegWrite(i, 0x10, reg);
	mvEthPhyRegWrite(i, 0x16, 0x0);

	DB2(printf("%s: configure jumbo QuadPhyPort=0x%x offset = 0x%x\n",__FUNCTION__,port0, offs));
	if (0 == initJumboPackets) {
		int j;
		initJumboPackets = 1;

		mvEthPhyRegWrite(port0, 0x16, 0x10);
		/* assume we want all ports to support jumbo*/
		for(j = 0 ; j < MV_ETH_MAX_PORTS; j++)
		{
			mvEthPhyRegWrite(port0, 0x01, ((0x800*j)+0x40)) ;
			mvEthPhyRegWrite(port0, 0x02, 0xFFF9);
			mvEthPhyRegWrite(port0, 0x03, 0x1);

			mvEthPhyRegWrite(port0, 0x01,  ((0x800*j)+0x50));
			mvEthPhyRegWrite(port0, 0x02, 0xFFF9);
			mvEthPhyRegWrite(port0, 0x03, 0x1);
		}
		mvEthPhyRegWrite(port0, 0x16, 0x0);
	}

	/* Configure LED */
	mvEthPhyRegWrite(i, 22, 3);
	mvEthPhyRegWrite(i, 16, 0x1111);

	mvEthPhyRegWrite(i, 22, 0x0000);
	mvEthPhyRegWrite(i, 0, 0x9140);

	/* Power up the phy */
	mvEthPhyRegRead(i, ETH_PHY_CTRL_REG, &reg);
	reg &= ~(ETH_PHY_CTRL_POWER_DOWN_MASK);
	mvEthPhyRegWrite(i, ETH_PHY_CTRL_REG, reg);

	mvOsDelay(100);
}

MV_VOID mvEth1540PhyBasicInit(MV_U32 ethPortNum, MV_BOOL eeeEnable)
{
	MV_U16 reg;
	int startAddr = ethphyHalData.phyAddr[ethPortNum];

	/* Reads ID2 */
	if (mvEthPhyRegRead(startAddr, 3, &reg) != MV_OK)
		return;


	/* Identify PHY Revision. */
	mvEthPhyRegWrite(startAddr, 0x16, 0xFB);
	mvEthPhyRegRead(startAddr, 0x1F, &reg);
	mvEthPhyRegWrite(startAddr, 0x16, 0x0);

	if (reg == 0x100)
		mvEth1540Y0PhyBasicInit(ethPortNum, eeeEnable);
	else if (reg == 0x200){
		/*Phy Identifier Register 3.15:0 is:
			A0:  0x0EB0
			A1:  0x0EB1
		*/
		mvEthPhyRegRead(startAddr, 0x3, &reg);
		mvEthPhyRegWrite(startAddr,0x16, 0x0);
		if(reg == 0x0EB1 )
			mvEth1540A1PhyBasicInit(ethPortNum, eeeEnable);
	else
		mvEth1540A0PhyBasicInit(ethPortNum, eeeEnable);
	}
	return;
}

MV_VOID mvEth1340PhyBasicInit(void)
{
	int i;
	MV_U16 reg;


	for (i = 0; i < 4; i++) {
		mvEthPhyRegWrite(i, 0x16, 0);
		mvEthPhyRegWrite(i, 0x1d, 3);
		mvEthPhyRegWrite(i, 0x1e, 2);
		mvEthPhyRegWrite(i, 0x1d, 0);

		/* Power up the phy */
		/* mvEthPhyRegRead(i,ETH_PHY_CTRL_REG, &reg); */
		/* reg |= ETH_PHY_CTRL_RESET_MASK; */
		/* mvEthPhyRegWrite(i,ETH_PHY_CTRL_REG, reg);   software reset */

		/* Enable QSGMII AN */
		/* Set page to 4. */
		mvEthPhyRegWrite(i, 0x16, 4);
		/* Enable AN */
		mvEthPhyRegWrite(i, 0x0, 0x1140);
		/* Set page to 0. */
		mvEthPhyRegWrite(i, 0x16, 0);
		mvEthPhyRegRead(i, ETH_PHY_CTRL_REG, &reg);
		reg &= ~(ETH_PHY_CTRL_POWER_DOWN_MASK);
		reg |= 0x1<<9;	/* workaround - restart workaround - restart workaround - restart workaround - restart */
		mvEthPhyRegWrite(i, ETH_PHY_CTRL_REG, reg);
	}
}
/*******************************************************************************
* mvEthE1512PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID mvEthE1512PhyBasicInit(MV_U32 ethPortNum)
{
/*
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x16, 0x12);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x14, 0x201);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x14, 0x8201);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x16, 0);
*/
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x16, 0x00FF);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x11, 0x214B);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x10, 0x2144);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x11, 0x0C28);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x10, 0x2146);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x11, 0xB233);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x10, 0x214D);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x11, 0xCC0C);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x10, 0x2159);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x16, 0x00FB);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x07, 0xC00D);

	#ifdef RD_88F6710
		/* Fix LED in front panel */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x16, 0x0003);
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x10, 0x1011);
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x16, 0x0000);
	#endif
	if (ethphyHalData.isSgmii[ethPortNum]) {

		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x16, 0x0012);
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x14, 0x0001);
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x14, 0x8001);
	}
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0x16, 0x0000);
}

MV_U32 mvEthPhyAddGet(MV_U32 ethPortNum)
{
       /* TODO Port num validation */
       return ethphyHalData.phyAddr[ethPortNum];
}


