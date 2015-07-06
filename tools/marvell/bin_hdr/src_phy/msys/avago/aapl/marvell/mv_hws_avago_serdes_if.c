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
* mvAvagoSerdesIf.c.c
*
* DESCRIPTION:
*         Avago interface
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifdef MV_HWS_BIN_HEADER
#include "mvSiliconIf.h"
#include <gtGenTypes.h>
#else
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <serdes/avago/mvAvagoIf.h>
#endif

#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
#include "sd28firmware/avago_fw_load.h"
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#include "mv_hws_avago_if.h"
/* Avago include */
#include "aapl.h"

#ifdef MARVELL_AVAGO_DB_BOARD
unsigned int mvAvagoDb = 1;
#else
unsigned int mvAvagoDb = 0;
#endif /* MARVELL_AVAGO_DB_BOARD */


/************************* Globals *******************************************************/
unsigned int avagoConnection = AVAGO_I2C_CONNECTION;
char avagoSerdesNum2SbusAddr[MAX_AVAGO_SERDES_NUMBER];
/************************* * Pre-Declarations *******************************************************/
#ifndef ASIC_SIMULATION

Aapl_t* aaplSerdesDb[HWS_MAX_DEVICE_NUM] = {0};

extern GT_STATUS mvHwsAvagoInitI2cDriver(GT_VOID);
#endif /* ASIC_SIMULATION */
/***************************************************************************************************/

int mvHwsAvagoCheckSerdesAccess
(
    unsigned char devNum,
    unsigned int  serdesNum
)
{
    return (aaplSerdesDb[devNum] == NULL) ? GT_NOT_INITIALIZED : GT_OK;
}

int mvHwsAvagoConvertSerdesToSbusAddr
(
    unsigned char devNum,
    unsigned int  serdesNum,
    unsigned int  *sbusAddr
)
{
    CHECK_STATUS(mvHwsAvagoCheckSerdesAccess(devNum, serdesNum));

    if(serdesNum >= MAX_AVAGO_SERDES_NUMBER) {
        return GT_BAD_PARAM;
    }

    *sbusAddr = avagoSerdesNum2SbusAddr[serdesNum];

    return GT_OK;
}

#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
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
#ifndef ASIC_SIMULATION
int mvHwsAvagoSerdesInit(unsigned char devNum)
{
    unsigned int sbus_addr;
    unsigned int curr_adr;
    unsigned int curr_serdes=0;
    unsigned int chip=devNum;
    unsigned int ring =0; /*we have only one ring*/
    Avago_addr_t addr_struct;
#ifndef AVAGO_AAPL_LGPL
    unsigned int addr = avago_make_addr3(AVAGO_BROADCAST, AVAGO_BROADCAST, AVAGO_BROADCAST);
#endif

    /* Validate AAPL */
    if (aaplSerdesDb[devNum] != NULL)
    {
        /* structures were already initialized */
        return GT_ALREADY_EXIST;
    }

    /* Initialize SBUS address Array */
    memset(avagoSerdesNum2SbusAddr, AVAGO_INVALID_SBUS_ADDR, MAX_AVAGO_SERDES_NUMBER);

    /* Construct AAPL structure */
    aaplSerdesDb[devNum] = aapl_construct();
#ifndef MV_HWS_BIN_HEADER
    if (avagoConnection == AVAGO_ETH_CONNECTION)
    {
        aaplSerdesDb[devNum]->communication_method = AAPL_DEFAULT_COMM_METHOD;
        CHECK_STATUS(mvHwsAvagoEthDriverInit(devNum));
    }
    else if (avagoConnection == AVAGO_I2C_CONNECTION)
    {
        aaplSerdesDb[devNum]->communication_method = AVAGO_USER_SUPPLIED_I2C;
        CHECK_STATUS(mvHwsAvagoInitI2cDriver());
    }
    else
    {
        osPrintf("mvHwsAvagoSerdesInit : unknown communication method %x\n",
                          aaplSerdesDb[devNum]->communication_method);
        return GT_INIT_ERROR;
    }

    /* Initialize AAPL structure */
    aapl_get_ip_info(aaplSerdesDb[devNum],1);

#ifndef AVAGO_AAPL_LGPL
    /* Print AAPL structure */
    aapl_print_struct(aaplSerdesDb[devNum],aaplSerdesDb[devNum]->debug > 0, addr, 0);
#endif /* AVAGO_AAPL_LGPL */
#endif /*MV_HWS_BIN_HEADER*/
    osPrintf("Loading Avago Firmware.......\n");

    /* Converts the address into an address structure */
    addr_struct.chip = devNum;
    addr_struct.ring = ring;

    /* Download Serdes's Firmware */
    /* ========================== */
    for( curr_adr = 1; curr_adr <= aaplSerdesDb[devNum]->max_sbus_addr[chip][ring]; curr_adr++ )
    {
        if (aaplSerdesDb[devNum]->ip_type[0][0][curr_adr] == AVAGO_SERDES)
        {
            addr_struct.sbus = curr_adr;
            sbus_addr = avago_struct_to_addr(&addr_struct);

            /* save sbus_addr in avagoSerdesNum2SbusAddr */
            avagoSerdesNum2SbusAddr[curr_serdes++] = curr_adr;

#ifdef FW_DOWNLOAD_FROM_SERVER
            AVAGO_DBG(("Loading file: %s to SBus address %x chip %d, ring %x sbus %x data[0]=%x\n",
                                       serdesFileName, sbus_addr, addr_struct.chip, addr_struct.ring, addr_struct.sbus));

            avago_spico_upload_file(aaplSerdesDb[devNum], sbus_addr , FALSE,serdesFileName);
            if (serdesSwapFileName != NULL)
            {
                AVAGO_DBG(("Loading swap file: %s to SBus address %x chip %d, ring %x sbus %x data[0]=%x\n",
                                           serdesSwapFileName, sbus_addr, addr_struct.chip, addr_struct.ring, addr_struct.sbus));
                avago_spico_upload_file(aaplSerdesDb[devNum], sbus_addr , FALSE,serdesSwapFileName);
            }
#else /* Download from embedded firmware file */
            AVAGO_DBG(("Loading to SBus address %x chip %d, ring %x sbus %x data[0]=%x\n",
                                       sbus_addr ,addr_struct.chip,addr_struct.ring ,addr_struct.sbus, serdesFwPtr[0]));
            CHECK_STATUS(avago_spico_upload(aaplSerdesDb[devNum], sbus_addr , FALSE, AVAGO_SERDES_FW_IMAGE_SIZE,serdesFwPtr));

#ifdef AVAGO_FW_SWAP_IMAGE_EXIST
            CHECK_STATUS(avago_spico_upload_swap_image(aaplSerdesDb[devNum], sbus_addr, FW_SWAP_IMAGE_SIZE, serdesFwDataSwapPtr));
#endif /* AVAGO_FW_SWAP_IMAGE_EXIST */
#endif /* FW_DOWNLOAD_FROM_SERVER */
        }
    }

    /* Download SBus_Master Firmware */
    /* ============================= */
    sbus_addr = AVAGO_SBUS_MASTER_ADDRESS;

#ifdef FW_DOWNLOAD_FROM_SERVER
    AVAGO_DBG(("Loading file: %s to SBus address %x chip %d, ring %x sbus %x data[0]=%x\n",
                              sbusMasterFileName, sbus_addr ,addr_struct.chip,addr_struct.ring ,addr_struct.sbus));
    avago_spico_upload_file(aaplSerdesDb[devNum], sbus_addr , FALSE,sbusMasterFileName);
#else
    AVAGO_DBG(("Loading to SBus address %x  data[0]=%x\n", sbus_addr , sbusMasterFwPtr[0]));
    CHECK_STATUS(avago_spico_upload(aaplSerdesDb[devNum], sbus_addr, FALSE, AVAGO_SBUS_MASTER_FW_IMAGE_SIZE, sbusMasterFwPtr));
#endif /* FW_DOWNLOAD_FROM_SERVER */

    if(aaplSerdesDb[devNum]->return_code < 0)
    {
        osPrintf("aapl_get_ip_info failed (return code 0x%x)\n", aaplSerdesDb[devNum]->return_code);
        return GT_INIT_ERROR;
    }

    osPrintf("Done\n");

#ifdef MV_HWS_ENABLE_INTERNAL_AAPL_DEBUG
    if (avagoConnection != AVAGO_ETH_CONNECTION)
    {
        int res;

        res = mvHwsAvagoSerdesDebugInit("mvHwsAvagoAACS_Server", avagoAACS_ServerRoutine, devNum);
        if (res != GT_OK)
        {
            osPrintf("Failed to init Avago AACS Server\n");
            return res;
        }
    }
#endif /* MV_HWS_ENABLE_INTERNAL_AAPL_DEBUG */

    return GT_OK;
}
#endif /* ASIC_SIMULATION */

#elif defined MV_HWS_REDUCED_BUILD_EXT_CM3  && !defined MV_HWS_BIN_HEADER

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
static int mvHwsAvagoSerdesInit(unsigned char devNum)
{
    /* Validate AAPL */
    if (aaplSerdesDb[devNum] != NULL)
    {
        /* structures were already initialized */
        return GT_ALREADY_EXIST;
    }

    /* Initialize AAPL structure in CM3 */

    return GT_OK;
}

#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/*******************************************************************************
* mvHwsAvagoSerdesPowerCtrlImpl
*
* DESCRIPTION:
*       Power up SERDES list.
*
* INPUTS:
*       devNum    -  system device number
*       serdesNum  - SERDES number
*       baudRate  -  baudrate
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
    unsigned char               devNum,
    unsigned int                serdesNum,
    unsigned int                divider
)
{
    Avago_serdes_init_config_t *config;
    unsigned int sbus_addr;
    unsigned int errors;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));
    if (sbus_addr == AVAGO_INVALID_SBUS_ADDR)
    {
        osPrintf("Invalid HW configuration !!!\n");
        return (GT_OK);
    }

    /*avago_serdes_init_quick(aaplSerdesDb[devNum], sbus_addr, divider);*/
#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
    /* Initialize the SerDes slice */
    config = avago_serdes_init_config_construct(aaplSerdesDb[devNum]);
#else
    /* Initialize the SerDes slice in CM3 */
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

    /* Change the config struct values from their defaults: */
    if (divider != NA_VALUE)
        config->tx_divider = config->rx_divider = divider;

    if (mvAvagoDb)
    {
        config->init_mode = AVAGO_PRBS31_ILB;
        osPrintf("Note: This is Avago DB configuration (with PRBS)\n");
    }
    else
    {
        config->init_mode = AVAGO_INIT_ONLY;
    }

    AVAGO_DBG(("mvHwsAvagoSerdesPowerCtrlImpl init_configuration:\n"));
    AVAGO_DBG(("   sbus_reset = %x \n",config->sbus_reset));
    AVAGO_DBG(("   init_tx = %x \n",config->init_tx));
    AVAGO_DBG(("   init_rx = %x \n",config->init_rx));
    AVAGO_DBG(("   init_mode = %x \n",config->init_mode));
    AVAGO_DBG(("   tx_divider = 0x%x \n",config->tx_divider));
    AVAGO_DBG(("   rx_divider = 0x%x \n",config->rx_divider));
    AVAGO_DBG(("   tx_width = 0x%x \n",config->tx_width));
    AVAGO_DBG(("   rx_width = 0x%x \n",config->rx_width));
    AVAGO_DBG(("   tx_phase_cal = %x \n",config->tx_phase_cal));
    AVAGO_DBG(("   tx_output_en = %x \n",config->tx_output_en));
    AVAGO_DBG(("   signal_ok_en = %x \n",config->signal_ok_en));
    AVAGO_DBG(("   signal_ok_threshold= %x \n",config->signal_ok_threshold));

    errors = avago_serdes_init(aaplSerdesDb[devNum], sbus_addr, config);
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    /* Releases a Avago_serdes_init_config_t struct */
    avago_serdes_init_config_destruct(aaplSerdesDb[devNum], config);
#else
    /* Initialize the SerDes slice in CM3 */
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
    CHECK_AVAGO_RET_CODE();
    if(errors > 0)
    {
#ifndef MV_HWS_REDUCED_BUILD
        osPrintf("SerDes init complete for SerDes at addr %s; Errors in ILB: %d. \n", aapl_addr_to_str(sbus_addr), errors);
#else
        osPrintf("SerDes init complete for SerDes at addr 0x%x; Errors in ILB: %d. \n", sbus_addr, errors);
#endif /* MV_HWS_REDUCED_BUILD */
    }
    if(errors == 0 && aapl_get_return_code(aaplSerdesDb[devNum]) == 0)
    {
#ifndef MV_HWS_REDUCED_BUILD
        osPrintf("The SerDes at address %s is initialized.\n", aapl_addr_to_str(sbus_addr));
#else
        osPrintf("The SerDes at address 0x%x is initialized.\n", sbus_addr);
#endif /* MV_HWS_REDUCED_BUILD */
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesPolarityConfigImpl
*
* DESCRIPTION:
*       Per Serdes invert the Tx or Rx.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical Serdes number
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
)
{
#ifndef ASIC_SIMULATION
    unsigned int sbus_addr;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    /* Tx polarity En */
    avago_serdes_set_tx_invert(aaplSerdesDb[devNum], sbus_addr, invertTx);
    CHECK_AVAGO_RET_CODE();

    /* Rx Polarity En */
    avago_serdes_set_rx_invert(aaplSerdesDb[devNum], sbus_addr, invertRx);
    CHECK_AVAGO_RET_CODE();

#endif /* ASIC_SIMULATION */
    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesPolarityConfigGetImpl
*
* DESCRIPTION:
*       Returns the Tx and Rx SERDES invert state.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       invertTx - invert TX polarity (GT_TRUE - invert, GT_FALSE - don't)
*       invertRx - invert RX polarity (GT_TRUE - invert, GT_FALSE - don't)
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
)
{
#ifndef ASIC_SIMULATION
    unsigned int sbus_addr;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    /* Get the TX inverter polarity mode: TRUE - inverter is enabled, FALSE - data is not being inverted */
    *invertTx = avago_serdes_get_tx_invert(aaplSerdesDb[devNum], sbus_addr);
    CHECK_AVAGO_RET_CODE();

    /* Get the RX inverter polarity mode: TRUE - inverter is enabled, FALSE - data is not being inverted */
    *invertRx = avago_serdes_get_rx_invert(aaplSerdesDb[devNum], sbus_addr);
    CHECK_AVAGO_RET_CODE();

#endif /* ASIC_SIMULATION */
    return GT_OK;
}
