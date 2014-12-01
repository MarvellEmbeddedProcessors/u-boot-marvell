/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsSiliconIf.c
*
* DESCRIPTION:
*       General silicon related HW Services API
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/
#include "mvSiliconIf.h"

/* store base address and unit index per unit per device type */
static HWS_UNIT_INFO   hwsDeviceSpecUnitInfo[LAST_UNIT];

/*******************************************************************************
* mvUnitExtInfoGet
*
* DESCRIPTION:
*       Return silicon specific base address and index for specified unit based on
*       unit index.
*
* INPUTS:
*       devNum    - Device Number
*       unitId    - unit ID (MAC, PCS, SERDES)
*       unitIndex - unit index
*
* OUTPUTS:
*       baseAddr  - unit base address in device
*       unitIndex - unit index in device
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void  mvUnitInfoGet
(
	MV_HWS_UNITS_ID	unitId,
	MV_U8			unitNum,
	MV_U32			*baseAddr,
	MV_U32			*unitIndexOffset
)
{
	if ((baseAddr == NULL) || (unitIndexOffset == NULL) || (unitId >= LAST_UNIT))
		return;

	*baseAddr        = hwsDeviceSpecUnitInfo[unitId].baseAddr;
	*unitIndexOffset = hwsDeviceSpecUnitInfo[unitId].regOffset;
}

/*******************************************************************************
* mvUnitInfoSet
*
* DESCRIPTION:
*       Init silicon specific base address and index for specified unit
*
* INPUTS:
*       devType   - Device type
*       unitId    - unit ID (MAC, PCS, SERDES)
*       baseAddr  - unit base address in device
*       unitIndex - unit index in device
*
* OUTPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS  mvUnitInfoSet
(
	MV_HWS_UNITS_ID	unitId,
	MV_U32			baseAddr,
	MV_U32			unitIndexOffset
)
{
	if(unitId >= LAST_UNIT)
		return MV_BAD_PARAM;

	hwsDeviceSpecUnitInfo[unitId].baseAddr  = baseAddr;
	hwsDeviceSpecUnitInfo[unitId].regOffset = unitIndexOffset;

	return MV_OK;
}

/*******************************************************************************
* genSwitchRegisterGet
*
* DESCRIPTION:
*       Read access to device registers.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       address   - address to access
*
* OUTPUTS:
*       data      - read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS genSwitchRegisterGet
(
	MV_U32			address,
	MV_U32			*data,
	MV_U32			mask
)
{
	SWITCH_ADDR_COMPL_SET(address); /* Only MSB is important, serdes number offset does not matter */

	*data  = MV_MEMIO_LE32_READ(SWITCH_BUS_ADDR(address)) & mask;
	return MV_OK;
}

/*******************************************************************************
* genSwitchRegisterSet
*
* DESCRIPTION:
*       Implement write access to device registers.
*
* INPUTS:
*       address   - address to access
*       data      - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS genSwitchRegisterSet
(
	MV_U32			address,
	MV_U32			data,
	MV_U32			mask
)
{
	MV_U32 regData;

	if ((mask & 0xFFFF) != 0xFFFF) { /* since switch registers are 16 bits - check only the relevant bits */
		genSwitchRegisterGet(address, &regData, ~mask);
		regData |= (data & mask);
	} else
		regData = data;

	SWITCH_ADDR_COMPL_SET(address); /* Only MSB is important, serdes number offset does not matter */

	MV_MEMIO_LE32_WRITE(SWITCH_BUS_ADDR(address), regData);
		return MV_OK;
}

/*******************************************************************************
* genUnitRegisterGet
*
* DESCRIPTION:
*       Read access to device registers.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       address   - address to access
*
* OUTPUTS:
*       data      - read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS genRegisterGet
(
	MV_U32			address,
	MV_U32			*data,
	MV_U32			mask
)
{
	*data = MV_MEMIO_LE32_READ(address) & mask;
	return MV_OK;
}

/*******************************************************************************
* genRegisterSet
*
* DESCRIPTION:
*       Implement write access to device registers.
*
* INPUTS:
*       address   - address to access
*       data      - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS genRegisterSet
(
	MV_U32			address,
	MV_U32			data,
	MV_U32			mask
)
{
	MV_U32 regData;

	if (mask != 0xFFFFFFFF) {
		regData = MV_MEMIO_LE32_READ(address);
		regData = (regData & ~mask) | (data & mask);
	} else
		regData = data;
	MV_MEMIO_LE32_WRITE(address, regData);
	return MV_OK;
}

/*******************************************************************************
* mvGenUnitRegisterSet
*
* DESCRIPTION:
*       Implement write access to device registers.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       address   - address to access
*       data      - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS mvGenUnitRegisterSet
(
	MV_HWS_UNITS_ID	unitId,
	MV_U8			unitNum,
	MV_U32			regOffset,
	MV_U32			data,
	MV_U32			mask
)
{
	MV_U32 address;
	MV_U32 unitAddr = 0;
	MV_U32 unitIndexOffset = 0;

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(unitId, unitNum, &unitAddr, &unitIndexOffset);
	if ((unitNum != 0) && (unitIndexOffset == 0))
		return MV_BAD_PARAM;

	if(mask == 0) /* the convention in the upper level code - if mask = 0, check all bits */
		mask = 0xFFFFFFFF;

	address = unitAddr + unitIndexOffset * unitNum + regOffset;

	switch (unitId) {
	case MG_UNIT:
	case SERDES_UNIT:
	case SERDES_PHY_UNIT:
		/* Units requre address completion */
		CHECK_STATUS(genSwitchRegisterSet(address, data, mask));
		break;
	case INTERNAL_REG_UNIT:
	case USB_REG_UNIT:
	case SERVER_REG_UNIT:
		/* Direct memory access units */
		CHECK_STATUS(genRegisterSet(address, data, mask));
		break;
	default:
		return MV_BAD_PARAM;
	}

	return MV_OK;
}

/*******************************************************************************
* mvGenUnitRegisterGet
*
* DESCRIPTION:
*       Read access to device registers.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       address   - address to access
*
* OUTPUTS:
*       data      - read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS mvGenUnitRegisterGet
(
	MV_HWS_UNITS_ID	unitId,
	MV_U8			unitNum,
	MV_U32			regOffset,
	MV_U32			*data,
	MV_U32			mask
)
{
	MV_U32 address;
	MV_U32 unitAddr = 0;
	MV_U32 unitIndexOffset = 0;

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(unitId, unitNum, &unitAddr, &unitIndexOffset);
	if ((unitAddr == 0) ||
		((unitNum != 0) && (unitIndexOffset == 0)))
		return MV_BAD_PARAM;

	address = unitAddr + unitIndexOffset * unitNum + regOffset;

	switch (unitId) {
	case MG_UNIT:
	case SERDES_UNIT:
	case SERDES_PHY_UNIT:
		/* Units requre address completion */
		CHECK_STATUS(genSwitchRegisterGet(address, data, mask));
		break;
	case INTERNAL_REG_UNIT:
	case USB_REG_UNIT:
	case SERVER_REG_UNIT:
		/* Direct memory access units */
		CHECK_STATUS(genRegisterGet(address, data, mask));
		break;
	default:
		return MV_BAD_PARAM;
	}

	return MV_OK;
}

