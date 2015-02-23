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
*       $Revision: 10 $
*
*******************************************************************************/
#include "mvSiliconIf.h"

MV_OS_EXACT_DELAY_FUNC   hwsOsExactDelayPtr = NULL;
MV_OS_TIME_WK_AFTER_FUNC hwsOsTimerWkFuncPtr = NULL;
MV_OS_MEM_SET_FUNC       hwsOsMemSetFuncPtr = NULL;
MV_OS_FREE_FUNC          hwsOsFreeFuncPtr = NULL;
MV_OS_MALLOC_FUNC        hwsOsMallocFuncPtr = NULL;
MV_SERDES_REG_ACCESS_SET hwsSerdesRegSetFuncPtr = NULL;
MV_SERDES_REG_ACCESS_GET hwsSerdesRegGetFuncPtr = NULL;
MV_SERVER_REG_ACCESS_SET hwsServerRegSetFuncPtr = NULL;
MV_SERVER_REG_ACCESS_GET hwsServerRegGetFuncPtr = NULL;
MV_OS_MEM_COPY_FUNC      hwsOsMemCopyFuncPtr = NULL;

/* poiner to a function which returns the ILKN registers DB */
MV_SIL_ILKN_REG_DB_GET hwsIlknRegDbGetFuncPtr = NULL;

HWS_DEVICE_INFO hwsDeviceSpecInfo[HWS_MAX_DEVICE_NUM];

/* store base address and unit index per unit per device type */
static HWS_UNIT_INFO   hwsDeviceSpecUnitInfo[LAST_SIL_TYPE][LAST_UNIT];
/* device specific functions pointers */
static MV_HWS_DEV_FUNC_PTRS hwsDevFunc[LAST_SIL_TYPE];

GT_STATUS genInterlakenRegSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data, GT_U32 mask);
GT_STATUS genInterlakenRegGet (GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data, GT_U32 mask);

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
void  mvUnitExtInfoGet
(
    GT_U8           devNum,
    MV_HWS_UNITS_ID unitId,
    GT_U8           unitNum,
    GT_U32          *baseAddr,
    GT_U32          *unitIndex
)
{
    if ((baseAddr == NULL) || (unitIndex == NULL) || (unitId >= LAST_UNIT))
    {
        return;
    }
    *baseAddr = hwsDeviceSpecUnitInfo[hwsDeviceSpecInfo[devNum].devType][unitId].baseAddr;
    *unitIndex = hwsDeviceSpecUnitInfo[hwsDeviceSpecInfo[devNum].devType][unitId].regOffset;
    if (hwsDeviceSpecInfo[devNum].devType == BobcatA0)
    {
        /* fixed base address for some units (according to CIDER) */
        switch (unitId)
        {
        case GEMAC_UNIT:
        case XLGMAC_UNIT:
		case MMPCS_UNIT:
        case XPCS_UNIT:
			if (unitNum >= 56)
            {
                *baseAddr += (0x200000 - ((*unitIndex) * 56)); /* the proper offset will be added by the caller of this function */
            }
			break;
        default:
            break;
        }
    }
}

/*******************************************************************************
* mvUnitInfoGet
*
* DESCRIPTION:
*       Return silicon specific base address and index for specified unit
*
* INPUTS:
*       devNum    - Device Number
*       unitId    - unit ID (MAC, PCS, SERDES)
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
    GT_U8           devNum,
    MV_HWS_UNITS_ID unitId,
    GT_U32          *baseAddr,
    GT_U32          *unitIndex
)
{
    if ((baseAddr == NULL) || (unitIndex == NULL) || (unitId >= LAST_UNIT))
    {
        return;
    }
    *baseAddr = hwsDeviceSpecUnitInfo[hwsDeviceSpecInfo[devNum].devType][unitId].baseAddr;
    *unitIndex = hwsDeviceSpecUnitInfo[hwsDeviceSpecInfo[devNum].devType][unitId].regOffset;
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
GT_STATUS  mvUnitInfoSet
(
    MV_HWS_DEV_TYPE devType,
    MV_HWS_UNITS_ID unitId,
    GT_U32          baseAddr,
    GT_U32          unitIndex
)
{
    if ((devType >= LAST_SIL_TYPE) || (unitId >= LAST_UNIT))
    {
        return GT_BAD_PARAM;
    }
    hwsDeviceSpecUnitInfo[devType][unitId].baseAddr = baseAddr;
    hwsDeviceSpecUnitInfo[devType][unitId].regOffset = unitIndex;

    return GT_OK;
}

/*******************************************************************************
* mvUnitInfoGetByAddr
*
* DESCRIPTION:
*       Return unit ID by unit address in device
*
* INPUTS:
*       devNum    - Device Number
*       baseAddr  - unit base address in device
*
* OUTPUTS:
*       unitId    - unit ID (MAC, PCS, SERDES)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void  mvUnitInfoGetByAddr
(
    GT_U8           devNum,
    GT_U32          baseAddr,
    MV_HWS_UNITS_ID *unitId
)
{
    GT_U32 i;

    if (unitId == NULL)
    {
        return;
    }
    *unitId = LAST_UNIT;
    for (i = 0; i < LAST_UNIT; i++)
    {
        if (baseAddr == hwsDeviceSpecUnitInfo[hwsDeviceSpecInfo[devNum].devType][i].baseAddr)
        {
            *unitId = i;
            return;
        }
    }
}

/*******************************************************************************
* mvHwsRedundancyVectorGet
*
* DESCRIPTION:
*       Get SD vector.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsRedundancyVectorGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  *sdVector
)
{
    if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].redundVectorGetFunc == NULL)
    {
      return GT_BAD_PARAM;
    }

    return hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].redundVectorGetFunc(devNum, portGroup, sdVector);
}

/*******************************************************************************
* hwsDeviceSpecGetFuncPtr
*
* DESCRIPTION:
*       Get function structure pointer.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void hwsDeviceSpecGetFuncPtr(MV_HWS_DEV_FUNC_PTRS **hwsFuncsPtr)
{
    *hwsFuncsPtr = &hwsDevFunc[0];
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* genUnitRegisterSet
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
GT_STATUS genUnitRegisterSet
(
    GT_U8  devNum,
    GT_UOPT         portGroup,
    MV_HWS_UNITS_ID unitId,
    GT_U8           unitNum,
    GT_UREG_DATA    regOffset,
    GT_UREG_DATA    data,
    GT_UREG_DATA    mask
)
{
    GT_U32 address;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    /* get unit base address and unit index for current device */
    mvUnitExtInfoGet(devNum, unitId, unitNum, &unitAddr, &unitIndex);
    address = unitAddr + unitIndex * unitNum + regOffset;

	if ((unitId == INTLKN_RF_UNIT) || (unitId == ETI_ILKN_RF_UNIT))
    {
        CHECK_STATUS(genInterlakenRegSet(devNum, portGroup, address, data, mask));
    }
    else
    {
		CHECK_STATUS(genRegisterSet(devNum, portGroup, address, data, mask));
    }
    
    return GT_OK;
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
GT_STATUS genUnitRegisterGet
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    MV_HWS_UNITS_ID unitId,
    GT_U8           unitNum,
    GT_UREG_DATA    regOffset,
    GT_UREG_DATA    *data,
    GT_UREG_DATA    mask
)
{
    GT_U32 address;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    /* get unit base address and unit index for current device */
    mvUnitExtInfoGet(devNum, unitId, unitNum, &unitAddr, &unitIndex);
    address = unitAddr + unitIndex * unitNum + regOffset;

	if ((unitId == INTLKN_RF_UNIT) || (unitId == ETI_ILKN_RF_UNIT))
    {
        CHECK_STATUS(genInterlakenRegGet(devNum, portGroup, address, data, mask));
    }
    else
    {
		CHECK_STATUS(genRegisterGet(devNum, portGroup, address, data, mask));
    }

    return GT_OK;
}

/*******************************************************************************
* genInterlakenRegSet
*
* DESCRIPTION:
*       Write ILKN registers.
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
GT_STATUS genInterlakenRegSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data, GT_U32 mask)
{
    MV_INTLKN_REG *dbArray;
    GT_U32 regData;
    GT_U32 offset;

	if(hwsIlknRegDbGetFuncPtr == NULL)
	{
		return GT_NOT_INITIALIZED;
	}

    dbArray = hwsIlknRegDbGetFuncPtr(devNum, portGroup, address);
	
	if(dbArray == NULL)
	{
		return GT_NOT_SUPPORTED;
	}

	/*osPrintf("genInterlakenRegSet address 0x%x\n", address);*/

    if (mask == 0)
    {
        /* store the register data */
        regData = data;
    }
    else
    {
		CHECK_STATUS(genInterlakenRegGet(devNum, portGroup, address, &regData, 0));

		/*osPrintf("Data: 0x%x", regData);*/

		/* Update the relevant bits at the register data */
        regData = (regData & ~mask) | (data & mask);

		/*osPrintf("\tNew Data: 0x%x\n", regData);*/
    }

	offset = address & 0x7FF;

	/*osPrintf("(Set) offset 0x%x\n", offset);*/
	
    /* store new value in shadow */
    switch (offset)
    {
    case 0:
        dbArray->ilkn0MacCfg0 = regData;
        break;
    case 4:
        dbArray->ilkn0ChFcCfg0 = regData;
        break;
    case 0xC:
        dbArray->ilkn0MacCfg2 = regData;
        break;
    case 0x10:
        dbArray->ilkn0MacCfg3 = regData;
        break;
    case 0x14:
        dbArray->ilkn0MacCfg4 = regData;
        break;
    case 0x1C:
        dbArray->ilkn0MacCfg6 = regData;
        break;
    case 0x60:
        dbArray->ilkn0ChFcCfg1 = regData;
        break;
    case 0x200:
        dbArray->ilkn0PcsCfg0 = regData;
        break;
    case 0x204:
        dbArray->ilkn0PcsCfg1 = regData;
        break;
    case 0x20C:
        dbArray->ilkn0En = regData;
        break;
    case 0x238:
        dbArray->ilkn0StatEn = regData;
        break;
    default:
        return GT_NOT_SUPPORTED;
    }

    return genRegisterSet(devNum, portGroup, address, regData, 0);
}

/*******************************************************************************
* genInterlakenRegGet
*
* DESCRIPTION:
*       Read ILKN registers.
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
GT_STATUS genInterlakenRegGet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data, GT_U32 mask)
{
    MV_INTLKN_REG *dbArray;
    GT_U32 regData;
    GT_U32 offset;

    devNum = devNum;
    portGroup = portGroup;

    if (data == NULL)
    {
        return GT_BAD_PARAM;
    }

	if(hwsIlknRegDbGetFuncPtr == NULL)
	{
		return GT_NOT_INITIALIZED;
	}

	/*osPrintf("genInterlakenRegGet address 0x%x\n", address);*/

	dbArray = hwsIlknRegDbGetFuncPtr(devNum, portGroup, address);
	
	if(dbArray == NULL)
	{
		return GT_NOT_SUPPORTED;
	}

	offset = address & 0x7FF;

	/*osPrintf("(Set) offset 0x%x\n", offset);*/

	switch (offset)
    {
    case 0:      /*ILKN_0_MAC_CFG_0 = 0,*/
        regData = dbArray->ilkn0MacCfg0;
        break;
    case 4:
        regData = dbArray->ilkn0ChFcCfg0;
        break;
    case 0xC:
        regData = dbArray->ilkn0MacCfg2;
        break;
    case 0x10:
        regData = dbArray->ilkn0MacCfg3;
        break;
    case 0x14:
        regData = dbArray->ilkn0MacCfg4;
        break;
    case 0x1C:
        regData = dbArray->ilkn0MacCfg6;
        break;
    case 0x60:
        regData = dbArray->ilkn0ChFcCfg1;
        break;
    case 0x200:
        regData = dbArray->ilkn0PcsCfg0;
        break;
    case 0x204:
        regData = dbArray->ilkn0PcsCfg1;
        break;
    case 0x20C:
        regData = dbArray->ilkn0En;
        break;
    case 0x238:
        regData = dbArray->ilkn0StatEn;
        break;
    default:
        *data = 0;
        return GT_NOT_SUPPORTED;
    }

    if (mask == 0)
    {
        *data = regData;
    }
    else
    {
        /* Retrieve the relevant bits from the register's data and shift left */
        *data = (regData & mask);
    }
    return GT_OK;
}
#endif
