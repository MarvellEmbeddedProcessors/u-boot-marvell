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

	*	Redistributions of source code must retain the above copyright notice,
		this list of conditions and the following disclaimer.

	*	Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

	*	Neither the name of Marvell nor the names of its contributors may be
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
#include "ctrlEnv/sys/mvCpuIf.h"
#include "mvSysPexConfig.h"
#include "mvPexRegs.h"
/* #include "pci-if/mvPciIf.h" */
#include "mvPex.h"

/* #define MV_DEBUG */
/* defines  */
#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

static MV_PEX_HAL_DATA pexHalData[MV_PEX_MAX_IF];

MV_STATUS mvPexInit(MV_U32 pexIf, MV_PEX_TYPE pexType, MV_PEX_HAL_DATA *halData)
{
	MV_PEX_MODE pexMode;
	MV_U32 regVal;
	MV_U32 status;
	MV_U32 ctrlFamily;

	mvOsMemcpy(&pexHalData[pexIf], halData, sizeof(MV_PEX_HAL_DATA));
	ctrlFamily=pexHalData[pexIf].ctrlFamily;

	if (mvPexModeGet(pexIf, &pexMode) != MV_OK) {
		mvOsPrintf("PEX init ERR. mvPexModeGet failed (pexType=%d)\n", pexMode.pexType);
		return MV_ERROR;
	}

	/* Check that required PEX type is the one set in reset time */
	if (pexType != pexMode.pexType) {
		/* No Link. Shut down the Phy */
		mvPexPhyPowerDown(pexIf);
		mvOsPrintf("PEX init ERR. PEX type sampled mismatch (%d,%d)\n", pexType, pexMode.pexType);
		return MV_ERROR;
	}

	if (MV_PEX_ROOT_COMPLEX == pexType) {
		mvPexLocalBusNumSet(pexIf, PEX_HOST_BUS_NUM(pexIf));
		mvPexLocalDevNumSet(pexIf, PEX_HOST_DEV_NUM(pexIf));

		/* Local device master Enable */
		mvPexMasterEnable(pexIf, MV_TRUE);

		/* Local device slave Enable */
		mvPexSlaveEnable(pexIf, mvPexLocalBusNumGet(pexIf), mvPexLocalDevNumGet(pexIf), MV_TRUE);
		/* Interrupt disable */
		status = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_STATUS_AND_COMMAND));
		status |= PXSAC_INT_DIS;
		MV_REG_WRITE(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_STATUS_AND_COMMAND), status);
		if (ctrlFamily==MV_67XX) {
			/* PEX capability */
			regVal = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CAPABILITY_REG)) & 0xF;
			if (regVal == 0x2) {
				/* KW40 set to Gen 2.0 - conf_auto_speed_change */
				/* when bit is set, link will issue link speed change to the max link speed possible */
				regVal = MV_REG_READ(PEX_CTRL_REG(pexIf)) | (1<<10);
				MV_REG_WRITE(PEX_CTRL_REG(pexIf), regVal);
			} 
		} 
	} else { /* if (MV_PEX_ROOT_COMPLEX != pexType) */
		if (ctrlFamily==MV_67XX) {
			/* TODO: 14/12/10 - requested by CV to support EP Compliance */
			MV_REG_WRITE(PEX_DBG_CTRL_REG(pexIf), 0x0F62F0C0);

			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexIf), (0x80C2 << 16));
			regVal = MV_REG_READ(PEX_PHY_ACCESS_REG(pexIf)) & 0xFFFF;
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexIf), (0xC2 << 16) | regVal | (1 << 2));

			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexIf), (0x8080 << 16));
			regVal = MV_REG_READ(PEX_PHY_ACCESS_REG(pexIf)) & 0xFFFF;
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexIf), (0x80 << 16) | regVal | (0x2 << 1));		
		} else {  /* if Armada-XP : */
			regVal = MV_REG_READ(PEX_DBG_CTRL_REG(pexIf));
			regVal &= ~(BIT16 | BIT19);
			MV_REG_WRITE(PEX_DBG_CTRL_REG(pexIf), regVal);
		}
	}
	
	if (ctrlFamily==MV_67XX) {
		 mvCpuIfEnablePex(pexIf);
	}
	/* now wait 1ms to be sure the link is valid */
	mvOsDelay(1);
	/* Check if we have link */
	if (MV_REG_READ(PEX_STATUS_REG(pexIf)) & PXSR_DL_DOWN) {
		/*mvOsPrintf("PEX%d interface detected no Link.\n", pexIf);*/
		return MV_NO_SUCH;
	}
#ifdef PCIE_VIRTUAL_BRIDGE_SUPPORT
	mvPexVrtBrgInit(pexIf);
#endif
	return MV_OK;
}

/*******************************************************************************
* mvPexModeGet - Get Pex Mode
*
* DESCRIPTION:
*
* INPUT:
*       pexIf   - PEX interface number.
*
* OUTPUT:
*       pexMode - Pex mode structure
*
* RETURN:
*       MV_OK on success , MV_ERROR otherwise
*
*******************************************************************************/
MV_U32 mvPexModeGet(MV_U32 pexIf, MV_PEX_MODE *pexMode)
{
	MV_U32 pexData;

	if (pexIf >= MV_PEX_MAX_IF)
		return MV_BAD_PARAM;

	
	pexData = MV_REG_READ(PEX_CTRL_REG(pexIf));

	switch (pexData & PXCR_DEV_TYPE_CTRL_MASK) {
	case PXCR_DEV_TYPE_CTRL_CMPLX:
		pexMode->pexType = MV_PEX_ROOT_COMPLEX;
		break;
	case PXCR_DEV_TYPE_CTRL_POINT:
		pexMode->pexType = MV_PEX_END_POINT;
		break;

	}

	/* Check if we have link */
/*	if (MV_REG_READ(PEX_STATUS_REG(pexIf)) & PXSR_DL_DOWN) { */

	if ((MV_REG_READ(PEX_DBG_STATUS_REG(pexIf)) & 0x7f) != 0x7E) {

		pexMode->pexLinkUp = MV_FALSE;

		/* If there is no link, the auto negotiation data is worthless */
		pexMode->pexWidth = MV_PEX_WITDH_INVALID;
	}
	else { /* We have Link negotiation started */
			pexMode->pexLinkUp = MV_TRUE;
		/* We have link. The link width is now valid */
		pexData = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CTRL_STAT_REG));
		pexMode->pexWidth = ((pexData & PXLCSR_NEG_LNK_WDTH_MASK) >> PXLCSR_NEG_LNK_WDTH_OFFS);
		pexMode->pexGen = ((pexData & PXLCSR_NEG_LNK_GEN_MASK) >> PXLCSR_NEG_LNK_GEN_OFFS);
	}

	return MV_OK;
}

/* PEX configuration space read write */

/*******************************************************************************
* mvPexConfigRead - Read from configuration space
*
* DESCRIPTION:
*       This function performs a 32 bit read from PEX configuration space.
*       It supports both type 0 and type 1 of Configuration Transactions
*       (local and over bridge). In order to read from local bus segment, use
*       bus number retrieved from mvPexLocalBusNumGet(). Other bus numbers
*       will result configuration transaction of type 1 (over bridge).
*
* INPUT:
*       pexIf   - PEX interface number.
*       bus     - PEX segment bus number.
*       dev     - PEX device number.
*       func    - Function number.
*       regOffs - Register offset.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit register data, 0xffffffff on error
*
*******************************************************************************/
MV_U32 mvPexConfigRead(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff)
{
#if defined(PCIE_VIRTUAL_BRIDGE_SUPPORT)
	return mvPexVrtBrgConfigRead(pexIf, bus, dev, func, regOff);
}

MV_U32 mvPexHwConfigRead(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff)
{
#endif
	MV_U32 pexData = 0;
	MV_U32 localDev, localBus;

	if (pexIf >= MV_PEX_MAX_IF)
		return 0xFFFFFFFF;

	/* Parameter checking   */
	if (PEX_DEFAULT_IF != pexIf) {
		if (pexIf >= pexHalData[pexIf].maxPexIf) {
			mvOsPrintf("mvPexConfigRead: ERR. Invalid PEX interface %d\n", pexIf);
			return 0xFFFFFFFF;
		}
	}

	if (dev >= MAX_PEX_DEVICES) {
		DB(mvOsPrintf("mvPexConfigRead: ERR. device number illigal %d\n", dev));
		return 0xFFFFFFFF;
	}

	if (func >= MAX_PEX_FUNCS) {
		DB(mvOsPrintf("mvPexConfigRead: ERR. function num illigal %d\n", func));
		return 0xFFFFFFFF;
	}

	if (bus >= MAX_PEX_BUSSES) {
		DB(mvOsPrintf("mvPexConfigRead: ERR. bus number illigal %d\n", bus));
		return MV_ERROR;
	}

	DB(mvOsPrintf("mvPexConfigRead: pexIf %d, bus %d, dev %d, func %d, regOff 0x%x\n",
		      pexIf, bus, dev, func, regOff));

	localDev = mvPexLocalDevNumGet(pexIf);
	localBus = mvPexLocalBusNumGet(pexIf);

	/* Speed up the process. In case on no link, return MV_ERROR */
	if ((dev != localDev) || (bus != localBus)) {
		pexData = MV_REG_READ(PEX_STATUS_REG(pexIf));

		if ((pexData & PXSR_DL_DOWN))
			return MV_ERROR;
	}

	/* in PCI Express we have only one device number */
	/* and this number is the first number we encounter
	   else that the localDev */
	/* spec pex define return on config read/write on any device */
	if (bus == localBus) {
		if (localDev == 0) {
			/* if local dev is 0 then the first number we encounter
			   after 0 is 1 */
			if ((dev != 1) && (dev != localDev))
				return MV_ERROR;
		} else {
			/* if local dev is not 0 then the first number we encounter
			   is 0 */

			if ((dev != 0) && (dev != localDev))
				return MV_ERROR;
		}
	}
	/* Creating PEX address to be passed */
	pexData = (bus << PXCAR_BUS_NUM_OFFS);
	pexData |= (dev << PXCAR_DEVICE_NUM_OFFS);
	pexData |= (func << PXCAR_FUNC_NUM_OFFS);
	pexData |= (regOff & PXCAR_REG_NUM_MASK);	/* lgacy register space */
	/* extended register space */
	pexData |= (((regOff & PXCAR_REAL_EXT_REG_NUM_MASK) >>
		     PXCAR_REAL_EXT_REG_NUM_OFFS) << PXCAR_EXT_REG_NUM_OFFS);

	pexData |= PXCAR_CONFIG_EN;

	/* Write the address to the PEX configuration address register */
	MV_REG_WRITE(PEX_CFG_ADDR_REG(pexIf), pexData);
	DB(mvOsPrintf("mvPexConfigRead:address pexData=%x ", pexData));

	/* In order to let the PEX controller absorbed the address of the read  */
	/* transaction we perform a validity check that the address was written */
	if (pexData != MV_REG_READ(PEX_CFG_ADDR_REG(pexIf)))
		return MV_ERROR;

	/* cleaning Master Abort */
	MV_REG_BIT_SET(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_STATUS_AND_COMMAND), PXSAC_MABORT);
		/* Read the Data returned in the PEX Data register */
		pexData = MV_REG_READ(PEX_CFG_DATA_REG(pexIf));

	DB(mvOsPrintf("mvPexConfigRead: got : %x \n", pexData));

	return pexData;

}

/*******************************************************************************
* mvPexConfigWrite - Write to configuration space
*
* DESCRIPTION:
*       This function performs a 32 bit write to PEX configuration space.
*       It supports both type 0 and type 1 of Configuration Transactions
*       (local and over bridge). In order to write to local bus segment, use
*       bus number retrieved from mvPexLocalBusNumGet(). Other bus numbers
*       will result configuration transaction of type 1 (over bridge).
*
* INPUT:
*       pexIf   - PEX interface number.
*       bus     - PEX segment bus number.
*       dev     - PEX device number.
*       func    - Function number.
*       regOffs - Register offset.
*       data    - 32bit data.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPexConfigWrite(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff, MV_U32 data)
{
#if defined(PCIE_VIRTUAL_BRIDGE_SUPPORT)
	return mvPexVrtBrgConfigWrite(pexIf, bus, dev, func, regOff, data);
}

MV_STATUS mvPexHwConfigWrite(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff, MV_U32 data)
{
#endif
	MV_U32 pexData = 0;
	MV_U32 localDev, localBus;

	if (pexIf >= MV_PEX_MAX_IF)
		return MV_BAD_PARAM;

	/* Parameter checking   */
	if (PEX_DEFAULT_IF != pexIf) {
		if (pexIf >= pexHalData[pexIf].maxPexIf) {
			mvOsPrintf("mvPexConfigWrite: ERR. Invalid PEX interface %d\n", pexIf);
			return MV_ERROR;
		}
	}

	if (dev >= MAX_PEX_DEVICES) {
		mvOsPrintf("mvPexConfigWrite: ERR. device number illigal %d\n", dev);
		return MV_BAD_PARAM;
	}

	if (func >= MAX_PEX_FUNCS) {
		mvOsPrintf("mvPexConfigWrite: ERR. function number illigal %d\n", func);
		return MV_ERROR;
	}

	if (bus >= MAX_PEX_BUSSES) {
		mvOsPrintf("mvPexConfigWrite: ERR. bus number illigal %d\n", bus);
		return MV_ERROR;
	}

	localDev = mvPexLocalDevNumGet(pexIf);
	localBus = mvPexLocalBusNumGet(pexIf);

	/* in PCI Express we have only one device number other than ourselves */
	/* and this number is the first number we encounter
	   else than the localDev that can be any valid dev number */
	/* pex spec define return on config read/write on any device */
	if (bus == localBus) {
		if (localDev == 0) {
			/* if local dev is 0 then the first number we encounter
			   after 0 is 1 */
			if ((dev != 1) && (dev != localDev))
				return MV_ERROR;
		} else {
			/* if local dev is not 0 then the first number we encounter
			   is 0 */

			if ((dev != 0) && (dev != localDev))
				return MV_ERROR;
		}

	}

	/* if we are not accessing ourselves , then check the link */
	if ((dev != localDev) || (bus != localBus)) {
		/* workarround */
		/* when no link return MV_ERROR */

		pexData = MV_REG_READ(PEX_STATUS_REG(pexIf));

		if ((pexData & PXSR_DL_DOWN))
			return MV_ERROR;
	}

	pexData = 0;

	/* Creating PEX address to be passed */
	pexData |= (bus << PXCAR_BUS_NUM_OFFS);
	pexData |= (dev << PXCAR_DEVICE_NUM_OFFS);
	pexData |= (func << PXCAR_FUNC_NUM_OFFS);
	pexData |= (regOff & PXCAR_REG_NUM_MASK);	/* lgacy register space */
	/* extended register space */
	pexData |= (((regOff & PXCAR_REAL_EXT_REG_NUM_MASK) >>
		     PXCAR_REAL_EXT_REG_NUM_OFFS) << PXCAR_EXT_REG_NUM_OFFS);
	pexData |= PXCAR_CONFIG_EN;

	DB(mvOsPrintf("mvPexConfigWrite: If=%x bus=%x func=%x dev=%x regOff=%x data=%x \n",
		      pexIf, bus, func, dev, regOff, data, pexData));

	/* Write the address to the PEX configuration address register */
	MV_REG_WRITE(PEX_CFG_ADDR_REG(pexIf), pexData);

	/* Clear CPU pipe. Important where CPU can perform OOO execution */
	CPU_PIPE_FLUSH;

	/* In order to let the PEX controller absorbed the address of the read  */
	/* transaction we perform a validity check that the address was written */
	if (pexData != MV_REG_READ(PEX_CFG_ADDR_REG(pexIf)))
		return MV_ERROR;

	/* Write the Data passed to the PEX Data register */
	MV_REG_WRITE(PEX_CFG_DATA_REG(pexIf), data);

	return MV_OK;

}

/*******************************************************************************
* mvPexMasterEnable - Enable/disale PEX interface master transactions.
*
* DESCRIPTION:
*       This function performs read modified write to PEX command status
*       (offset 0x4) to set/reset bit 2. After this bit is set, the PEX
*       master is allowed to gain ownership on the bus, otherwise it is
*       incapable to do so.
*
* INPUT:
*       pexIf  - PEX interface number.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPexMasterEnable(MV_U32 pexIf, MV_BOOL enable)
{
	MV_U32 pexCommandStatus;

	/* Parameter checking   */
	if (pexIf >= pexHalData[pexIf].maxPexIf) {
		mvOsPrintf("mvPexMasterEnable: ERR. Invalid PEX interface %d\n", pexIf);
		return MV_ERROR;
	}

	pexCommandStatus = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_STATUS_AND_COMMAND));

	if (MV_TRUE == enable)
		pexCommandStatus |= PXSAC_MASTER_EN;
	else
		pexCommandStatus &= ~PXSAC_MASTER_EN;

	MV_REG_WRITE(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_STATUS_AND_COMMAND), pexCommandStatus);

	return MV_OK;
}

/*******************************************************************************
* mvPexSlaveEnable - Enable/disale PEX interface slave transactions.
*
* DESCRIPTION:
*       This function performs read modified write to PEX command status
*       (offset 0x4) to set/reset bit 0 and 1. After those bits are set,
*       the PEX slave is allowed to respond to PEX IO space access (bit 0)
*       and PEX memory space access (bit 1).
*
* INPUT:
*       pexIf  - PEX interface number.
*       dev     - PEX device number.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPexSlaveEnable(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_BOOL enable)
{
	MV_U32 pexCommandStatus;
	MV_U32 RegOffs;

	/* Parameter checking   */
	if (pexIf >= pexHalData[pexIf].maxPexIf) {
		mvOsPrintf("mvPexSlaveEnable: ERR. Invalid PEX interface %d\n", pexIf);
		return MV_BAD_PARAM;
	}
	if (dev >= MAX_PEX_DEVICES) {
		mvOsPrintf("mvPexLocalDevNumSet: ERR. device number illigal %d\n", dev);
		return MV_BAD_PARAM;

	}

	RegOffs = PEX_STATUS_AND_COMMAND;

	pexCommandStatus = mvPexConfigRead(pexIf, bus, dev, 0, RegOffs);

	if (MV_TRUE == enable)
		pexCommandStatus |= (PXSAC_IO_EN | PXSAC_MEM_EN);
	else
		pexCommandStatus &= ~(PXSAC_IO_EN | PXSAC_MEM_EN);

	mvPexConfigWrite(pexIf, bus, dev, 0, RegOffs, pexCommandStatus);

	return MV_OK;

}

/*******************************************************************************
* mvPexLocalBusNumSet - Set PEX interface local bus number.
*
* DESCRIPTION:
*       This function sets given PEX interface its local bus number.
*       Note: In case the PEX interface is PEX-X, the information is read-only.
*
* INPUT:
*       pexIf  - PEX interface number.
*       busNum - Bus number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_NOT_ALLOWED in case PEX interface is PEX-X.
*		MV_BAD_PARAM on bad parameters ,
*       otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPexLocalBusNumSet(MV_U32 pexIf, MV_U32 busNum)
{
	MV_U32 pexStatus;

	/* Parameter checking   */
	if (pexIf >= pexHalData[pexIf].maxPexIf) {
		mvOsPrintf("mvPexLocalBusNumSet: ERR. Invalid PEX interface %d\n", pexIf);
		return MV_BAD_PARAM;
	}
	if (busNum >= MAX_PEX_BUSSES) {
		mvOsPrintf("mvPexLocalBusNumSet: ERR. bus number illigal %d\n", busNum);
		return MV_ERROR;
	}

	pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));

	pexStatus &= ~PXSR_PEX_BUS_NUM_MASK;

	pexStatus |= (busNum << PXSR_PEX_BUS_NUM_OFFS) & PXSR_PEX_BUS_NUM_MASK;

	MV_REG_WRITE(PEX_STATUS_REG(pexIf), pexStatus);

	return MV_OK;
}

/*******************************************************************************
* mvPexLocalBusNumGet - Get PEX interface local bus number.
*
* DESCRIPTION:
*       This function gets the local bus number of a given PEX interface.
*
* INPUT:
*       pexIf  - PEX interface number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Local bus number.0xffffffff on Error
*
*******************************************************************************/
MV_U32 mvPexLocalBusNumGet(MV_U32 pexIf)
{
	MV_U32 pexStatus;

	if (pexIf >= MV_PEX_MAX_IF)
		return 0xFFFFFFFF;

	/* Parameter checking   */
	if (PEX_DEFAULT_IF != pexIf) {
		if (pexIf >= pexHalData[pexIf].maxPexIf) {
			mvOsPrintf("mvPexLocalBusNumGet: ERR. Invalid PEX interface %d\n", pexIf);
			return 0xFFFFFFFF;
		}
	}

	pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));

	pexStatus &= PXSR_PEX_BUS_NUM_MASK;

	return (pexStatus >> PXSR_PEX_BUS_NUM_OFFS);
}

/*******************************************************************************
* mvPexLocalDevNumSet - Set PEX interface local device number.
*
* DESCRIPTION:
*       This function sets given PEX interface its local device number.
*       Note: In case the PEX interface is PEX-X, the information is read-only.
*
* INPUT:
*       pexIf  - PEX interface number.
*       devNum - Device number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_NOT_ALLOWED in case PEX interface is PEX-X.
*		MV_BAD_PARAM on bad parameters ,
*       otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPexLocalDevNumSet(MV_U32 pexIf, MV_U32 devNum)
{
	MV_U32 pexStatus;

	if (pexIf >= MV_PEX_MAX_IF)
		return MV_BAD_PARAM;

	/* Parameter checking   */
	if (pexIf >= pexHalData[pexIf].maxPexIf) {
		mvOsPrintf("mvPexLocalDevNumSet: ERR. Invalid PEX interface %d\n", pexIf);
		return MV_BAD_PARAM;
	}
	if (devNum >= MAX_PEX_DEVICES) {
		mvOsPrintf("mvPexLocalDevNumSet: ERR. device number illigal %d\n", devNum);
		return MV_BAD_PARAM;
	}

	pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));

	pexStatus &= ~PXSR_PEX_DEV_NUM_MASK;

	pexStatus |= (devNum << PXSR_PEX_DEV_NUM_OFFS) & PXSR_PEX_DEV_NUM_MASK;

	MV_REG_WRITE(PEX_STATUS_REG(pexIf), pexStatus);

	return MV_OK;
}

/*******************************************************************************
* mvPexLocalDevNumGet - Get PEX interface local device number.
*
* DESCRIPTION:
*       This function gets the local device number of a given PEX interface.
*
* INPUT:
*       pexIf  - PEX interface number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Local device number. 0xffffffff on Error
*
*******************************************************************************/
MV_U32 mvPexLocalDevNumGet(MV_U32 pexIf)
{
	MV_U32 pexStatus;

	/* Parameter checking   */

	if (PEX_DEFAULT_IF != pexIf) {
		if (pexIf >= pexHalData[pexIf].maxPexIf) {
			mvOsPrintf("mvPexLocalDevNumGet: ERR. Invalid PEX interface %d\n", pexIf);
			return 0xFFFFFFFF;
		}
	}

	pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));

	pexStatus &= PXSR_PEX_DEV_NUM_MASK;

	return (pexStatus >> PXSR_PEX_DEV_NUM_OFFS);
}

MV_VOID mvPexPhyRegRead(MV_U32 pexIf, MV_U32 regOffset, MV_U16 *value)
{

	MV_U32 regAddr;
	if (pexIf >= pexHalData[pexIf].maxPexIf) {
		mvOsPrintf("mvPexPhyRegRead: ERR. Invalid PEX interface %d\n", pexIf);
		return;
	}
	regAddr = (BIT31 | ((regOffset & 0x3fff) << 16));
	MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexIf), regAddr);
	*value = MV_REG_READ(PEX_PHY_ACCESS_REG(pexIf));
}

MV_VOID mvPexPhyRegWrite(MV_U32 pexIf, MV_U32 regOffset, MV_U16 value)
{

	MV_U32 regAddr;
	if (pexIf >= pexHalData[pexIf].maxPexIf) {
		mvOsPrintf("mvPexPhyRegWrite: ERR. Invalid PEX interface %d\n", pexIf);
		return;
	}
	regAddr = (((regOffset & 0x3fff) << 16) | value);
	MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexIf), regAddr);
}

/*******************************************************************************
* mvPexActiveStateLinkPMEnable
*
* DESCRIPTION:
*	Enable Active Link State Power Management
*
* INPUT:
*	pexIf		- PEX interface number.
*	enable		- MV_TRUE to enable ASPM, MV_FALSE to disable.
*
* OUTPUT:
*	None
*
* RETURN:
*	MV_OK on success , MV_ERROR otherwise
*
*******************************************************************************/
MV_STATUS mvPexActiveStateLinkPMEnable(MV_U32 pexIf, MV_BOOL enable)
{
	MV_U32 reg;

	if (pexIf >= pexHalData[pexIf].maxPexIf) {
		mvOsPrintf("mvPexActiveStateLinkPMEnable: ERR. Invalid PEX interface %d\n", pexIf);
		return MV_ERROR;
	}

	reg = MV_REG_READ(PEX_PWR_MNG_EXT_REG(pexIf)) & ~PXPMER_L1_ASPM_EN_MASK;
	if (enable == MV_TRUE)
		reg |= PXPMER_L1_ASPM_EN_MASK;
	MV_REG_WRITE(PEX_PWR_MNG_EXT_REG(pexIf), reg);

	/* Enable / Disable L0/1 entry */
	reg = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CTRL_STAT_REG))
	      & ~PXLCSR_ASPM_CNT_MASK;
	if (enable == MV_TRUE)
		reg |= PXLCSR_ASPM_CNT_L0S_L1S_ENT_SUPP;
	MV_REG_WRITE(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CTRL_STAT_REG), reg);

	return MV_OK;
}

/*******************************************************************************
* mvPexForceX1
*
* DESCRIPTION:
*	shut down lanes 1-3 if recognize that attached to an x1 end-point
* INPUT:
*	pexIf		- PEX interface number.
*
* OUTPUT:
*	None
*
* RETURN:
*	MV_OK on success , MV_ERROR otherwise
*
*******************************************************************************/
MV_U32 mvPexForceX1(MV_U32 pexIf)
{
	MV_U32 regData = 0;
	if (pexIf >= pexHalData[pexIf].maxPexIf) {
		mvOsPrintf("mvPexForceX1: ERR. Invalid PEX interface %d\n", pexIf);
		return MV_BAD_PARAM;
	}

	regData = MV_REG_READ(PEX_CTRL_REG(pexIf)) & ~(PXCR_CONF_LINK_MASK);
	regData |= PXCR_CONF_LINK_X1;

	MV_REG_WRITE(PEX_CTRL_REG(pexIf), regData);
	return MV_OK;
}

/*******************************************************************************
* mvPexIfEnable
*
* DESCRIPTION:
*	This function Enables PCI Express interface.
*
* INPUT:
*	pexIf		-  PEX interface number.
*	pexType		-  MV_PEX_ROOT_COMPLEX - root complex device
*			   MV_PEX_END_POINT - end point device
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvPexIfEnable(MV_U32 pexIf, MV_PEX_TYPE pexType)
{
	MV_U32 regVal;

/* NOTE: this was asked by CV, bit is reserved in the spec, but causing problems, disabling for now. */
	/* MV_REG_BIT_SET(PEX_CTRL_REG(pexIf), PXCR_AUTO_SPEED_CTRL_MASK); */

	/* Set pex mode incase S@R not exist */
	if (pexType == MV_PEX_END_POINT) {
		MV_REG_BIT_RESET(PEX_CTRL_REG(pexIf), PXCR_DEV_TYPE_CTRL_MASK);
		/* Change pex mode in capability reg */
		MV_REG_BIT_RESET(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_CAPABILITY_REG), BIT22);
		MV_REG_BIT_SET(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_CAPABILITY_REG), BIT20);

		regVal = MV_REG_READ(PEX_CAPABILITIES_REG(pexIf));
		regVal |= 0x00F00000;
		regVal &= ~(BIT23 | BIT22 | BIT21);
		MV_REG_WRITE(PEX_CAPABILITIES_REG(pexIf), regVal);
	} else {
		regVal = MV_REG_READ(PEX_CAPABILITIES_REG(pexIf));
		regVal |= 0x00F00000;
		regVal &= ~(BIT23 | BIT21 | BIT20);
		MV_REG_WRITE(PEX_CAPABILITIES_REG(pexIf), regVal);

		MV_REG_BIT_SET(PEX_CTRL_REG(pexIf), PXCR_DEV_TYPE_CTRL_MASK);
	}
	return;
}

MV_VOID mvPexPhyPowerDown(MV_U32 pexIf)
{
	MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexIf), 0x20800087);
	return;
}
