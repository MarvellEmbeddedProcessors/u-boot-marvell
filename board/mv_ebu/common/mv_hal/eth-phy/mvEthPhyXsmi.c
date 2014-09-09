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
#include "mvSysEthPhyConfig.h"
#include "mvEthPhyRegs.h"
#include "mvEthPhyXsmi.h"

#undef DEBUG
#ifdef DEBUG
#define DB(x) x
#define DB2(x) x
#else
#define DB(x)
#define DB2(x)
#endif /* DEBUG */

static MV_ETHPHY_XSMI_HAL_DATA ethphyXsmiHalData;

/*******************************************************************************
* mvEthPhyXsmiHalInit -
*
* DESCRIPTION:
*       Initialize the ethernet phy XSMI unit HAL.
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
MV_STATUS mvEthPhyXsmiHalInit(MV_ETHPHY_XSMI_HAL_DATA *halData)
{
	mvOsMemcpy(&ethphyXsmiHalData, halData, sizeof(MV_ETHPHY_XSMI_HAL_DATA));

	return MV_OK;
}

/*******************************************************************************
* mvEthPhyXsmiCheckParam - check parameters of XSMI interface
*
* DESCRIPTION:
*       check parameters of XSMI interface
*
* INPUT:
*       phyAddr - Phy address.
*       devAddr - device address.
*       regAddr	- register address of the XSMI.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK if parameters is OK, MV_FAIL if parameters is wrong.
*
*******************************************************************************/
MV_STATUS mvEthPhyXsmiCheckParam(MV_U32 phyAddr, MV_U32 devAddr, MV_U16 regAddr)
{
	/* check parameters */
	if ((phyAddr << ETH_PHY_XSMI_PHY_ADDR_OFFS) & ~ETH_PHY_XSMI_PHY_ADDR_MASK) {
		mvOsPrintf("%s: Err. Illegal PHY address %d\n", __func__, phyAddr);
		return MV_FAIL;
	}
	if ((devAddr <<  ETH_PHY_XSMI_DEV_ADDR_OFFS) & ~ETH_PHY_XSMI_DEV_ADDR_MASK) {
		mvOsPrintf("%s: Err. Illegal device address %d\n", __func__, devAddr);
		return MV_FAIL;
	}
	if ((regAddr << ETH_PHY_XSMI_REG_ADDR_OFFS) & ~ETH_PHY_XSMI_REG_ADDR_MASK) {
		mvOsPrintf("%s: Err. Illegal register address %d\n", __func__, regAddr);
		return MV_FAIL;
	}
	return MV_OK;
}

/*******************************************************************************
* mvEthPhyXsmiWaitReady -
*
* DESCRIPTION:
*       read the XSMI management register until XBusy is 0
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK if no timeout, MV_FAIL when timeout while waiting.
*
*******************************************************************************/
MV_STATUS mvEthPhyXsmiWaitReady(void)
{
	MV_U32 xSmiReg;
	volatile MV_U32 timeout;
	timeout = ETH_PHY_XSMI_TIMEOUT;
	/* wait till the XSMI is not busy*/
	do {
		/* read xsmi register */
		xSmiReg = MV_MEMIO_LE32_READ(ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_MANAGEMENT);
		if (timeout-- == 0) {
			mvOsPrintf("%s: XSMI busy timeout\n", __func__);
			return MV_FAIL;
		}
	} while (xSmiReg & ETH_PHY_XSMI_BUSY_MASK);

	return MV_OK;
}

/*******************************************************************************
* mvEthPhyXsmiWaitValid -
*
* DESCRIPTION:
*       read the XSMI management register until XValid is 1,
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK if no timeout, MV_FAIL when timeout while waiting.
*
*******************************************************************************/
MV_STATUS mvEthPhyXsmiWaitValid(void)
{
	MV_U32 xSmiReg;
	volatile MV_U32 timeout;
	timeout = ETH_PHY_XSMI_TIMEOUT;

	/*wait till readed value is ready */
	do {
		/* read xsmi register */
		xSmiReg = MV_MEMIO_LE32_READ(ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_MANAGEMENT);

		if (timeout-- == 0) {
			mvOsPrintf("%s: XSMI read-valid timeout\n", __func__);
			return MV_FAIL;
		}
	} while (!(xSmiReg & ETH_PHY_XSMI_READ_VALID_MASK));

	return MV_OK;
}

/*******************************************************************************
* mvEthPhyXsmiRegRead - Read from ethernet phy register.
*
* DESCRIPTION:
*       This function reads ethernet phy register using XSMI.
*
* INPUT:
*       phyAddr - Phy address.
*       devAddr - Phy register offset.
*
* OUTPUT:
*       None.
*
* RETURN:
*       16bit phy register value, or 0xffff on error
*
*******************************************************************************/
MV_STATUS mvEthPhyXsmiRegRead(MV_U32 phyAddr, MV_U32 devAddr, MV_U16 regAddr, MV_U16 *data)
{
	MV_U32 xSmiReg, xSmiAddr;

	if (mvEthPhyXsmiCheckParam(phyAddr, devAddr, regAddr) != MV_OK)
		return MV_FAIL;

	if (mvEthPhyXsmiWaitReady() != MV_OK)
		return MV_FAIL;

	xSmiAddr = (regAddr << ETH_PHY_XSMI_REG_ADDR_OFFS);
	MV_MEMIO_LE32_WRITE(ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_REG_ADDR, xSmiAddr);

	/* fill the phy address and regiser offset and read opcode */
	xSmiReg = (phyAddr <<  ETH_PHY_XSMI_PHY_ADDR_OFFS) | (devAddr << ETH_PHY_XSMI_DEV_ADDR_OFFS)|
			   ETH_PHY_XSMI_OPCODE_ADDR_READ;

	/* write the xsmi register */
	MV_MEMIO_LE32_WRITE(ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_MANAGEMENT, xSmiReg);

	if (mvEthPhyXsmiWaitValid() != MV_OK)
		return MV_FAIL;

	*data = (MV_U16)(MV_MEMIO_LE32_READ(ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_MANAGEMENT) &
			ETH_PHY_XSMI_DATA_MASK);

	return MV_OK;
}

/*******************************************************************************
* mvEthPhyXsmiRegWrite - Write to ethernet phy register.
*
* DESCRIPTION:
*       This function write to ethernet phy register using XSMI.
*
* INPUT:
*       phyAddr - Phy address.
*       devAddr - Phy register offset.
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
MV_STATUS mvEthPhyXsmiRegWrite(MV_U32 phyAddr, MV_U32 devAddr, MV_U16 regAddr, MV_U16 data)
{
	MV_U32 xSmiReg, xSmiAddr;

	if (mvEthPhyXsmiCheckParam(phyAddr, devAddr, regAddr) != MV_OK)
		return MV_FAIL;

	if (mvEthPhyXsmiWaitReady() != MV_OK)
		return MV_FAIL;

	xSmiAddr = (regAddr << ETH_PHY_XSMI_REG_ADDR_OFFS);
	MV_MEMIO_LE32_WRITE(ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_REG_ADDR, xSmiAddr);

	/* fill the phy address and regiser offset and write opcode and data*/
	xSmiReg = (data << ETH_PHY_XSMI_DATA_OFFS);
	xSmiReg |= (phyAddr <<  ETH_PHY_XSMI_PHY_ADDR_OFFS) | (devAddr << ETH_PHY_XSMI_DEV_ADDR_OFFS);
	xSmiReg |= ETH_PHY_XSMI_OPCODE_ADDR_WRITE;

	/* write the xsmi register */
	DB(printf("%s: phyAddr=0x%x offset = 0x%x data=0x%x\n", __func__, phyAddr, devAddr, data));
	DB(printf("%s: ethphyXsmiHalData.ethPhyXsmiReg = 0x%x xSmiReg=0x%x\n", __func__,
				ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_MANAGEMENT, xSmiReg));
	MV_MEMIO_LE32_WRITE(ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_MANAGEMENT, xSmiReg);

	return MV_OK;
}

/*******************************************************************************
* mvEthPhyXsmiRegReadModifyWrite - Read modify write to ethernet phy register.
*
* DESCRIPTION:
*       This function read modify and write to ethernet phy register using XSMI.
*
* INPUT:
*       phyAddr - Phy address.
*       devAddr - Phy register offset.
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
MV_STATUS mvEthPhyXsmiRegReadModifyWrite(MV_U32 phyAddr, MV_U32 devAddr, MV_U16 regAddr, MV_U16 data)
{
	MV_U32 xSmiReg;
	MV_U16 orgData;
	MV_STATUS operationStatus;

	operationStatus = mvEthPhyXsmiRegRead(phyAddr, devAddr, regAddr, &orgData);
	if (operationStatus != MV_OK)
		return operationStatus;

	/* fill the phy address and regiser offset and write opcode and data*/
	xSmiReg = (data << ETH_PHY_XSMI_DATA_OFFS);
	xSmiReg |= (phyAddr <<  ETH_PHY_XSMI_PHY_ADDR_OFFS) | (devAddr << ETH_PHY_XSMI_DEV_ADDR_OFFS);
	xSmiReg |= ETH_PHY_XSMI_OPCODE_ADDR_WRITE;

	/* write the xsmi register */
	DB(printf("%s: phyAddr=0x%x offset = 0x%x data=0x%x\n", __func__, phyAddr, devAddr, data));
	DB(printf("%s: ethphyXsmiHalData.ethPhyXsmiRegOff = 0x%x xSmiReg=0x%x\n", __func__,
				ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_MANAGEMENT, xSmiReg));
	MV_MEMIO_LE32_WRITE(ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_MANAGEMENT, xSmiReg);

	return MV_OK;
}

/*******************************************************************************
* mvEthPhyXsmiSeveralRegRead - Read of several successive PHY registers.
*
* DESCRIPTION:
*       This function read of several successive PHY registers using XSMI.
*
* INPUT:
*       phyAddr - Phy address.
*       devAddr - Phy register offset.
*       ptrData - pointer to array 16 bit.
*       count - count of data to read.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_FAIL on error, MV_OK if succeed
*
*******************************************************************************/
MV_STATUS mvEthPhyXsmiSeveralRegRead(MV_U32 phyAddr, MV_U32 devAddr, MV_U16 regAddr, MV_U16 *ptrData, MV_U32 count)
{
	MV_U32 xSmiReg, xSmiAddr, i;

	if (mvEthPhyXsmiCheckParam(phyAddr, devAddr, regAddr) != MV_OK)
		return MV_FAIL;

	if (mvEthPhyXsmiWaitReady() != MV_OK)
		return MV_FAIL;

	xSmiAddr = (regAddr << ETH_PHY_XSMI_REG_ADDR_OFFS);
	MV_MEMIO_LE32_WRITE(ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_REG_ADDR, xSmiAddr);

	/* Perform reading as many as needed with OpCode ETH_PHY_XSMI_OPCODE_ADDR_INC_READ
	 * to read all registers except the last one with OpCode ETH_PHY_XSMI_OPCODE_READ */
	for (i = 0; i < count; i++) {
		/* fill the phy address and regiser offset and read opcode */
		if (i != count - 1)
			xSmiReg = (phyAddr <<  ETH_PHY_XSMI_PHY_ADDR_OFFS) | (devAddr << ETH_PHY_XSMI_DEV_ADDR_OFFS)|
				ETH_PHY_XSMI_OPCODE_ADDR_INC_READ;
		else
			xSmiReg = (phyAddr <<  ETH_PHY_XSMI_PHY_ADDR_OFFS) | (devAddr << ETH_PHY_XSMI_DEV_ADDR_OFFS)|
				ETH_PHY_XSMI_OPCODE_READ;
		/* write the xsmi register */
		MV_MEMIO_LE32_WRITE(ethphyXsmiHalData.ethPhyXsmiRegOff + ETH_PHY_XSMI_MANAGEMENT, xSmiReg);

		if (mvEthPhyXsmiWaitValid() != MV_OK)
			return MV_FAIL;

		ptrData[i] = (MV_U16)(MV_MEMIO_LE32_READ(ethphyXsmiHalData.ethPhyXsmiRegOff +
					ETH_PHY_XSMI_MANAGEMENT) & ETH_PHY_XSMI_DATA_MASK);
	}

	return MV_OK;
}
