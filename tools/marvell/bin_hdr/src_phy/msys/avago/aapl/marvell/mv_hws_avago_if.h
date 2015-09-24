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
* mv_hws_avago_if.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/

#ifndef __mv_hws_avago_if_H
#define __mv_hws_avago_if_H

#ifdef MV_HWS_BIN_HEADER
#include "util.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

/************************* definition *****************************************************/
#define  AVAGO_ETH_CONNECTION         1
#define  AVAGO_I2C_CONNECTION         2
#define  AVAGO_SBUS_CONNECTION        3
#define  MAX_AVAGO_SPICO_NUMBER       27 /* There are 26 Spico but numbering starts from 1 */
#define  MAX_AVAGO_SERDES_NUMBER      40
#define  MAX_AVAGO_SERDES_ADDRESS     255
#define  NA_VALUE                     0
#define  AVAGO_INVALID_SBUS_ADDR      (-1)

#define AVAGO_AAPL_LGPL
/*#define MARVELL_AVAGO_DEBUG_FLAG*/
/*#define MARVELL_AVAGO_DB_BOARD*/

#ifdef MARVELL_AVAGO_DEBUG_FLAG
#define AVAGO_DBG(s) osPrintf s
#else
#define AVAGO_DBG(s)
#endif /* MARVELL_AVAGO_DEBUG_FLAG */

#define CHECK_AVAGO_RET_CODE() \
{ \
    if(aaplSerdesDb[devNum]->return_code < 0) \
    { \
        osPrintf("%s failed (return code %d)\n", __func__, aaplSerdesDb[devNum]->return_code); \
        return GT_INIT_ERROR; \
    } \
}

#ifdef MV_HWS_BIN_HEADER
#define HWS_MAX_DEVICE_NUM 1
#define osPrintf     mvPrintf

#define genRegisterSet(devNum, portGroup, address, sbus_data, mask) \
     devNum = devNum; \
     portGroup = portGroup; \
     genSwitchRegisterSet(address, sbus_data, mask)

#define genRegisterGet(devNum, portGroup, address, data_ptr, mask) \
     devNum = devNum; \
     portGroup = portGroup; \
     genSwitchRegisterGet(address, data_ptr, mask)
#endif /* MV_HWS_BIN_HEADER */

/************************* Globals *******************************************************/
int mvHwsAvagoInitializationCheck
(
    unsigned char devNum,
    unsigned int  serdesNum
);

int mvHwsAvagoConvertSerdesToSbusAddr
(
    unsigned char devNum,
    unsigned int  serdesNum,
    unsigned int  *sbusAddr
);

int mvHwsAvagoConvertSbusAddrToSerdes
(
    unsigned char *serdesNum,
    unsigned int  sbusAddr
);

#ifndef ASIC_SIMULATION
/*******************************************************************************
* mvHwsAvagoSerdesInit
*
* DESCRIPTION:
*       Initialize Avago related configurations
*
* INPUTS:
*       devNum - system device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
int mvHwsAvagoSerdesInit
(
    unsigned char devNum
);
#endif

/*******************************************************************************
* mvHwsAvagoSerdesSpicoInterrupt
*
* DESCRIPTION:
*       Issue the interrupt to the Spico processor.
*       The return value is the interrupt number.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       interruptCode - Code of interrupt
*       interruptData - Data to write
*
* OUTPUTS:
*       result - spico interrupt return value
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvHwsAvagoSerdesSpicoInterrupt
(
    unsigned char   devNum,
    unsigned int    portGroup,
    unsigned int    serdesNum,
    unsigned int    interruptCode,
    unsigned int    interruptData,
    unsigned int    *result
);

/*******************************************************************************
* mvHwsAvagoSerdesTemperatureGet
*
* DESCRIPTION:
*       Get the Temperature (in C) from Avago Serdes
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       temperature - Serdes temperature degree value
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvHwsAvagoSerdesTemperatureGet
(
    unsigned char   devNum,
    unsigned int    portGroup,
    unsigned int    serdesNum,
    int             *temperature
);

/*******************************************************************************
* mvHwsAvagoSerdesPolarityConfigImpl
*
* DESCRIPTION:
*       Per serdes invert the Tx or Rx.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvHwsAvagoSerdesPolarityConfigImpl
(
    unsigned char     devNum,
    unsigned int      portGroup,
    unsigned int      serdesNum,
    unsigned int     invertTx,
    unsigned int     invertRx

);

/*******************************************************************************
* mvHwsAvagoSerdesPolarityConfigGetImpl
*
* DESCRIPTION:
*       Per SERDES invert the Tx or Rx.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       invertTx  - invert TX polarity (GT_TRUE - invert, GT_FALSE - don't)
*       invertRx  - invert RX polarity (GT_TRUE - invert, GT_FALSE - don't)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvHwsAvagoSerdesPolarityConfigGetImpl
(
    unsigned char    devNum,
    unsigned int     portGroup,
    unsigned int     serdesNum,
    unsigned int     *invertTx,
    unsigned int     *invertRx
);

/*******************************************************************************
* mvHwsAvagoSerdesResetImpl
*
* DESCRIPTION:
*       Per SERDES Clear the serdes registers (back to defaults.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - serdes number
*       analogReset - Analog Reset (On/Off
*       digitalReset - digital Reset (On/Off)
*       syncEReset - SyncE Reset (On/Off)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
unsigned int mvHwsAvagoSerdesResetImpl
(
    unsigned char    devNum,
    unsigned int     portGroup,
    unsigned int     serdesNum,
    unsigned int     analogReset,
    unsigned int     digitalReset,
    unsigned int     syncEReset
);

/*******************************************************************************
* mvHwsAvagoSerdesPowerCtrlImpl
*
* DESCRIPTION:
*       Power up SERDES list.
*
* INPUTS:
*       devNum    - device number
*       portGroup - port group (core) number
*       serdesNum - SERDES number
*       powerUp   - True for PowerUP, False for PowerDown
*       divider   - divider of Serdes speed
*       refClock  - ref clock value
*       refClockSource - ref clock source (primary line or secondary)
*       media     - RXAUI or XAUI
*       mode      - Serdes bus modes: 10Bits/20Bits/40Bits
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvHwsAvagoSerdesPowerCtrlImpl
(
    unsigned char       devNum,
    unsigned int        portGroup,
    unsigned int        serdesNum,
    unsigned char       powerUp,
    unsigned int        divider,
    unsigned char       refClock,
    unsigned char       refClockSource,
    unsigned char       media,
    unsigned char       mode
);

/*******************************************************************************
* mvHwsAvagoSerdesSbmVoltageGet
*
* DESCRIPTION:
*       Gets the voltage data from a given AVAGO_THERMAL_SENSOR sensor.
*       Returns the voltage in milli-volt.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       serdesNum   - physical serdes number
*       sensorAddr  - SBus address of the AVAGO_THERMAL_SENSOR
*
* OUTPUTS:
*       voltage - Serdes voltage in milli-volt
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvHwsAvagoSerdesSbmVoltageGet
(
    unsigned char   devNum,
    unsigned int    portGroup,
    unsigned int    serdesNum,
    unsigned int    sensorAddr,
    unsigned int    *voltage
);

#ifdef __cplusplus
}
#endif

#endif /* __mv_hws_avago_if_H */
