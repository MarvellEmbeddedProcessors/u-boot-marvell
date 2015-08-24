/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsDdr3A38x.c
*
* DESCRIPTION: DDR3 A38x configuration
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 50 $
******************************************************************************/

#define DEFINE_GLOBALS
#include "mv_os.h"
#include "ddr3_a38x.h"
#include "ddr3_a38x_tip_training_static.h"
#include "ddr3_hws_sil_training.h"
#include "mvXor.h"
#include "mvSysEnvLib.h"

#include "mvHwsDdr3A38x.h"
#include "mvSiliconIf.h"
#include "mvDdr3TrainingIpStatic.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpDef.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvDdr3LoggingDef.h"

/************************** definitions ******************************/
extern GT_U8  debugTrainingAccess;
extern GT_U8  debugTrainingA38x;
extern GT_U32 firstActiveIf;
extern MV_HWS_DDR_FREQ initFreq;
extern GT_U32 delayEnable, ckDelay, caDelay;
extern GT_U32 dfsLowFreq;
extern GT_U32 mode2T;

GT_U32  pipeMulticastMask;
GT_BOOL ddr3AsyncModeAtTF = GT_FALSE;
#define A38X_NUM_BYTES                  (3)
#define A38X_NUMBER_OF_INTERFACES		5

#define SAR_DEV_ID_OFFS                         27
#define SAR_DEV_ID_MASK                         0x7

#define A38X_PLL_REG(a,b,c)  ((a << 12) + (b << 8) + (c << 2))
#define R_MOD_W(writeData,readData,mask) ((writeData & mask) + (data & (~mask)))

extern MV_BOOL ddr3IfEccEnabled(void);
/************************** pre-declaration ******************************/
extern GT_U32 maskTuneFunc;
extern GT_BOOL rlMidFreqWA;
extern GT_U8 calibrationUpdateControl; /*2 external only, 1 is internal only*/
extern GT_U32 freqVal[];

#ifdef CONFIG_DDR4
extern GT_U8	vrefCalibrationWA; /*1 means SSTL & POD gets the same Vref and a WA is needed*/
#endif

extern MV_HWS_DDR_FREQ mediumFreq;

extern GT_STATUS ddr3TipRegWrite
(
    GT_U32 devNum,
    GT_U32 regAddr,
    GT_U32 dataValue
);

extern GT_STATUS ddr3TipRegRead
(
    GT_U32 devNum,
    GT_U32 regAddr,
    GT_U32 *dataValue,
    GT_U32 regMask
);

extern MV_STATUS ddr3SiliconGetDdrTargetFreq(MV_U32 *uiDdrFreq);

GT_STATUS ddr3TipA38xGetFreqConfig
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 freq,
	MV_HWS_TIP_FREQ_CONFIG_INFO		*freqConfigInfo
);

static GT_STATUS ddr3TipA38xSetDivider
(
	GT_U8							devNum,
    GT_U32                			interfaceId,
    MV_HWS_DDR_FREQ                 freq
);
GT_STATUS ddr3A38xUpdateTopologyMap
(
    GT_U32 devNum,
	MV_HWS_TOPOLOGY_MAP* topologyMap
);
GT_STATUS ddr3TipA38xGetInitFreq
(
    GT_STATUS       devNum,
    MV_HWS_DDR_FREQ *freq
);

GT_U8    ddr3TipClockMode( GT_U32 frequency );

#ifdef CONFIG_DDR3
GT_STATUS ddr3TipA38xGetMediumFreq
(
    GT_STATUS       devNum,
    MV_HWS_DDR_FREQ *freq
);
#endif
GT_STATUS    ddr3TipA38xIFRead
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                *data,
    GT_U32                mask 
);

GT_STATUS    ddr3TipA38xIFWrite
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                dataValue,
    GT_U32                mask
);

GT_STATUS ddr3TipA38xGetDeviceInfo ( GT_U8     devNum,
									MV_DDR3_DEVICE_INFO * infoPtr);
/************************** Globals ******************************/
static GT_U8 A38xBwPerFreq[DDR_FREQ_LIMIT] =
{
    0x3, /*DDR_FREQ_100*/
#ifdef CONFIG_DDR3
    0x4, /*DDR_FREQ_400*/
    0x4, /*DDR_FREQ_533*/
#endif
    0x5, /*DDR_FREQ_667*/
    0x5, /*DDR_FREQ_800*/
    0x5, /*DDR_FREQ_933*/
    0x5, /*DDR_FREQ_1066*/
#ifdef CONFIG_DDR4
	0x5, /*DDR_FREQ_900*/
	0x5, /*DDR_FREQ_1000*/
#endif
#ifdef CONFIG_DDR3
    0x3, /*DDR_FREQ_311*/
    0x3, /*DDR_FREQ_333*/
    0x4, /*DDR_FREQ_467*/
    0x5, /*DDR_FREQ_850*/
    0x5, /*DDR_FREQ_600*/
    0x3, /*DDR_FREQ_300*/
	0x5, /*DDR_FREQ_900*/
	0x3,  /*DDR_FREQ_360*/
	0x5  /*DDR_FREQ_1000*/
#endif
};

static GT_U8 A38xRatePerFreq[DDR_FREQ_LIMIT] =
{
    0x1, /*DDR_FREQ_100*/
#ifdef CONFIG_DDR3
    0x2, /*DDR_FREQ_400*/
    0x2, /*DDR_FREQ_533*/
#endif
    0x2, /*DDR_FREQ_667*/
    0x2, /*DDR_FREQ_800*/
    0x3, /*DDR_FREQ_933*/
    0x3, /*DDR_FREQ_1066*/
#ifdef CONFIG_DDR4
	0x2, /*DDR_FREQ_900*/
	0x2, /*DDR_FREQ_1000*/
#endif
#ifdef CONFIG_DDR3
    0x1, /*DDR_FREQ_311*/
    0x1, /*DDR_FREQ_333*/
    0x2, /*DDR_FREQ_467*/
    0x2, /*DDR_FREQ_850*/
    0x2, /*DDR_FREQ_600*/
    0x1, /*DDR_FREQ_300*/
	0x2, /*DDR_FREQ_900*/
	0x1, /*DDR_FREQ_360*/
	0x2 /*DDR_FREQ_1000*/
#endif
};

static GT_U16 A38xVcoFreqPerSarRefClk25Mhz[] =
{
	666, /*0*/
	1332,
	800,
	1600,
	1066,
	2132,
	1200,
	2400,
	1332,
	1332,
	1500,
	1500,
	1600,/*12*/
	1600,
	1700,
	1700,
	1866,
	1866,
	1800,/*18*/
	2000,
	2000,
	4000,
	2132,
	2132,
	2300,
	2300,
	2400,
	2400,
	2500,
	2500,
	800
};

static GT_U16 A38xVcoFreqPerSarRefClk40Mhz[] =
{
	666, /*0*/
	1332,
	800,
	800, /*0x3*/
	1066,
	1066, /*0x5*/
	1200,
	2400,
	1332,
	1332,
	1500,/*10*/
	1600, /*0xB*/
	1600,
	1600,
	1700,
	1560, /*0xF*/
	1866,
	1866,
	1800,
	2000,
	2000,/*20*/
	4000,
	2132,
	2132,
	2300,
	2300,
	2400,
	2400,
	2500,
	2500,
	1800 /*30 - 0x1E*/
};

GT_U16 A38xODTSlope[] =
{
	21443,
	1452,
	482,
	240,
	141,
	90,
	67,
	52
};

GT_U16 A38xODTIntercept[] =
{
	1517,
	328,
	186,
	131,
	100,
	80,
	69,
	61
};

GT_U32 DQbitMap2Phypin[] =
{
	1, 0, 2, 6, 9, 8, 3, 7,	//0
	8, 9, 1, 7, 2, 6, 3, 0,	//1
	3, 9, 7, 8, 1, 0, 2, 6,	//2
	1, 0, 6, 2, 8, 3, 7, 9,	//3
	0, 1, 2, 9, 7, 8, 3, 6,	//4
};

/**********************************************************************************/


/******************************************************************************
* Read temperature TJ value
*/
GT_U32 ddr3CtrlGetJuncTemp(GT_U8 devNum)
{
	GT_32 reg = 0;

	/* Initiates TSEN hardware reset once */
	if ((MV_REG_READ(TSEN_CONF_REG) & TSEN_CONF_RST_MASK) == 0)
		MV_REG_BIT_SET(TSEN_CONF_REG, TSEN_CONF_RST_MASK);
	mvOsDelay(10);

	/* Check if the readout field is valid */
	if ((MV_REG_READ(TSEN_STATUS_REG) & TSEN_STATUS_READOUT_VALID_MASK) == 0) {
		mvPrintf("%s: TSEN not ready\n", __func__);
		return 0;
	}
	reg = MV_REG_READ(TSEN_STATUS_REG);
	reg = (reg & TSEN_STATUS_TEMP_OUT_MASK) >> TSEN_STATUS_TEMP_OUT_OFFSET;

	return ((((10000 * reg) / 21445) * 1000) - 272674) / 1000;
}

/******************************************************************************
* Name:     ddr3TipA38xGetFreqConfig.
* Desc:     
* Args:     
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS ddr3TipA38xGetFreqConfig
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 freq,
	MV_HWS_TIP_FREQ_CONFIG_INFO		*freqConfigInfo
)
{
    if (A38xBwPerFreq[freq] == 0xff)
    {
        return GT_NOT_SUPPORTED;
    }

    if (freqConfigInfo == NULL)
    {
        return GT_BAD_PARAM;
    }

    freqConfigInfo->bwPerFreq = A38xBwPerFreq[freq];
    freqConfigInfo->ratePerFreq = A38xRatePerFreq[freq];
    freqConfigInfo->isSupported = GT_TRUE;

    return GT_OK;
}


/******************************************************************************
* Name:     ddr3TipA38xPipeEnable.
* Desc:     
* Args:     
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipA38xPipeEnable
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_BOOL               enable
)
{
    GT_U32 dataValue, pipeEnableMask = 0;

    if (enable == GT_FALSE)
    {
        pipeEnableMask = 0;
    }
    else
    {
       if (interfaceAccess == ACCESS_TYPE_MULTICAST)
        {
            pipeEnableMask = pipeMulticastMask;
        }
        else
        {
            pipeEnableMask = (1 << interfaceMap[interfaceId].pipe);
        }
    }
    CHECK_STATUS(ddr3TipRegRead(devNum, PIPE_ENABLE_ADDR, &dataValue, MASK_ALL_BITS));
    dataValue = (dataValue & (~0xFF)) | pipeEnableMask;
    CHECK_STATUS(ddr3TipRegWrite(devNum, PIPE_ENABLE_ADDR, dataValue));

    return GT_OK;
}

/******************************************************************************
* Name:     ddr3TipA38xIFWrite.
* Desc:     
* Args:     
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipA38xIFWrite
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                dataValue,
    GT_U32                mask
)
{
	GT_U32 uiDataRead;
#ifdef WIN32
    //mvPrintf(DEBUG_LEVEL_TRACE,"ddr3DunitAccessWrite   0x%x: 0x%x (mask 0x%x)\n", regAddr, dataValue);
    return GT_OK;
#endif
    if (mask != MASK_ALL_BITS)
    {
        if (debugA38x >= 2)
        {
            //mvPrintf("ddr3TipA38xIFWrite active interface = %d\n",interfaceId);
        }
        /*CHECK_STATUS(ddr3DunitAccessRead(devNum, ACCESS_TYPE_UNICAST,interfaceId, regAddr,  &uiDataRead, MASK_ALL_BITS));
        dataValue = (uiDataRead & (~mask)) | (dataValue & mask);*/
		CHECK_STATUS(ddr3TipA38xIFRead(devNum, ACCESS_TYPE_UNICAST,interfaceId, regAddr,  &uiDataRead, MASK_ALL_BITS));
        dataValue = (uiDataRead & (~mask)) | (dataValue & mask);
    }

    if (debugA38x >= 1)
    {
       // mvPrintf("Dunit Write: interface %d  access %d Address 0x%x Data 0x%x\n", interfaceId, interfaceAccess, regAddr, dataValue);
    }

    MV_REG_WRITE(regAddr, dataValue);
	//mvPrintf("write: 0x%x = 0x%x\n",regAddr,dataValue);

    return GT_OK;
}



/******************************************************************************
* Name:     ddr3TipA38xIFRead.
* Desc:     
* Args:     
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipA38xIFRead
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                *data,
    GT_U32                mask 
)
{
#ifdef WIN32
     //mvPrintf("ddr3DunitAccessRead   0x%x: 0x%x (mask 0x%x)\n", regAddr, *data);
     return GT_OK;
#endif
  
	*data = MV_REG_READ(regAddr) & mask;
    //mvPrintf("Read: 0x%x = 0x%x\n",regAddr,*data);
	if (debugA38x >= 2)
	{
		//mvPrintf("DunitRead   0x%x: 0x%x (mask 0x%x)\n",  regAddr,  *data, mask);
	}

    return GT_OK;
}


 

/******************************************************************************
* Name:     ddr3TipA38xSelectDdrController.
* Desc:     Enable/Disable access to Marvell's server.
* Args:     devNum     - device number
*           enable        - whether to enable or disable the server
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipA38xSelectDdrController
(
    GT_U8    devNum,
    GT_BOOL  enable
)
{
    GT_U32 uiReg;
	//mvPrintf("ddr3TipA38xSelectDdrController\n");
	uiReg = MV_REG_READ(CS_ENABLE_REG);

    if (enable)
    {         
    uiReg |= (1 << 6);
    }
    else
    {
	uiReg &= ~(1 << 6);
    }
	MV_REG_WRITE(CS_ENABLE_REG, uiReg);
	//mvPrintf("write: 0x%x = 0x%x\n",CS_ENABLE_REG,uiReg);
    return GT_OK;
}

/******************************************************************************
* Name:     ddr3TipInitA38xSilicon.
* Desc:     init Training SW DB.
* Args:     
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
static GT_STATUS ddr3TipInitA38xSilicon
(
    GT_U32  devNum,
    GT_U32  boardId
) 
{
    MV_HWS_TIP_CONFIG_FUNC_DB configFunc;
	MV_HWS_DDR_FREQ uiDdrFreq;
	MV_STATUS status;
	MV_HWS_TOPOLOGY_MAP* topologyMap = ddr3TipGetTopologyMap(devNum);
	GT_U8 numOfBusPerInterface = 5;

    /* new read leveling version */
    configFunc.tipDunitReadFunc = ddr3TipA38xIFRead;
    configFunc.tipDunitWriteFunc = ddr3TipA38xIFWrite;
    configFunc.tipDunitMuxSelectFunc = ddr3TipA38xSelectDdrController;
    configFunc.tipGetFreqConfigInfoFunc = ddr3TipA38xGetFreqConfig;
	configFunc.tipSetFreqDividerFunc = ddr3TipA38xSetDivider;
	configFunc.tipGetDeviceInfoFunc = ddr3TipA38xGetDeviceInfo;
	configFunc.tipGetTemperature = ddr3CtrlGetJuncTemp;
	configFunc.tipGetClockRatio = ddr3TipClockMode;

    mvHwsDdr3TipInitConfigFunc(devNum, &configFunc);

	ddr3TipRegisterDqTable(devNum, DQbitMap2Phypin);

	/*Set device attributes*/
	ddr3TipDevAttrInit(devNum);
	ddr3TipDevAttrSet(devNum, MV_ATTR_TIP_REV, MV_TIP_REV_4);
	ddr3TipDevAttrSet(devNum, MV_ATTR_PHY_EDGE, MV_DDR_PHY_EDGE_POSITIVE);
	ddr3TipDevAttrSet(devNum, MV_ATTR_OCTET_PER_INTERFACE, numOfBusPerInterface);
#ifdef CONFIG_ARMADA_39X
	ddr3TipDevAttrSet(devNum, MV_ATTR_INTERLEAVE_WA, GT_TRUE);
#else
	ddr3TipDevAttrSet(devNum, MV_ATTR_INTERLEAVE_WA, GT_FALSE);
#endif

#ifdef STATIC_ALGO_SUPPORT
	{
		MV_HWS_TIP_STATIC_CONFIG_INFO staticConfig;
    	GT_U32 boardOffset = boardId * A38X_NUMBER_OF_INTERFACES *numOfBusPerInterface;

		staticConfig.siliconDelay = A38xSiliconDelayOffset[boardId];
		staticConfig.packageTraceArr = A38xPackageRoundTripDelayArray;
		staticConfig.boardTraceArr = &A38xBoardRoundTripDelayArray[boardOffset];
		ddr3TipInitStaticConfigDb(devNum, &staticConfig);
	}
#endif
	status = ddr3TipA38xGetInitFreq(devNum, &uiDdrFreq);
	if (MV_OK != status) {
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("DDR3 silicon get target frequency - FAILED 0x%x\n", status));
		return status;
	}

#ifdef CONFIG_DDR3
    maskTuneFunc =     (SET_LOW_FREQ_MASK_BIT |
						LOAD_PATTERN_MASK_BIT |
						SET_MEDIUM_FREQ_MASK_BIT |
						WRITE_LEVELING_MASK_BIT |
						WRITE_LEVELING_SUPP_MASK_BIT |
						READ_LEVELING_MASK_BIT |
						PBS_RX_MASK_BIT |
						PBS_TX_MASK_BIT |
						SET_TARGET_FREQ_MASK_BIT |
						WRITE_LEVELING_TF_MASK_BIT |
						WRITE_LEVELING_SUPP_TF_MASK_BIT |
                        READ_LEVELING_TF_MASK_BIT |
						CENTRALIZATION_RX_MASK_BIT |
						CENTRALIZATION_TX_MASK_BIT
						);

	rlMidFreqWA = GT_TRUE;

	if( (uiDdrFreq == DDR_FREQ_333) || (uiDdrFreq == DDR_FREQ_400)){
		maskTuneFunc =     (
							WRITE_LEVELING_MASK_BIT |
							LOAD_PATTERN_2_MASK_BIT |
							WRITE_LEVELING_SUPP_MASK_BIT |
							READ_LEVELING_MASK_BIT |
							PBS_RX_MASK_BIT |
							PBS_TX_MASK_BIT |
							CENTRALIZATION_RX_MASK_BIT |
							CENTRALIZATION_TX_MASK_BIT
							);
		rlMidFreqWA = GT_FALSE; /*WA not needed if 333/400 is TF*/
	}

	/*Supplementary not supported for ECC modes*/
	if( MV_TRUE == ddr3IfEccEnabled()){
		maskTuneFunc &=	~WRITE_LEVELING_SUPP_TF_MASK_BIT;
		maskTuneFunc &=	~WRITE_LEVELING_SUPP_MASK_BIT;
		maskTuneFunc &=	~PBS_TX_MASK_BIT;
		maskTuneFunc &=	~PBS_RX_MASK_BIT;
	}
#else /* CONFIG_DDR4 */
	maskTuneFunc = (
			SET_LOW_FREQ_MASK_BIT |
			LOAD_PATTERN_MASK_BIT |
			SET_TARGET_FREQ_MASK_BIT |
			WRITE_LEVELING_TF_MASK_BIT |
			READ_LEVELING_TF_MASK_BIT |
			RECEIVER_CALIBRATION_MASK_BIT |
			WL_PHASE_CORRECTION_MASK_BIT |
			DQ_VREF_CALIBRATION_MASK_BIT |
			DQ_MAPPING_MASK_BIT
		);

	rlMidFreqWA = GT_FALSE;

	/*detect if VrefCalib WA needed by device ID(a382 didn't need this WA)*/
	if( ((MV_REG_READ(DEV_ID_REG) & 0xFFFF0000) >> 16) == 0x6811){
		mvPrintf("vrefCalibrationWA disabled\n");
		vrefCalibrationWA = 0;
	}

#if 0
	/*detect if VrefCalib WA needed by revision ID(a390 A0 didn't need this WA)*/
	if( ( ((MV_REG_READ(DEV_ID_REG) & 0xFFFF0000) >> 16) == 0x6920) &&
		(((MV_REG_READ(DEV_VERSION_ID_REG) & REVISON_ID_MASK) >> REVISON_ID_OFFS) == 0x6) ){
		mvPrintf("vrefCalibrationWA disabled\n");
		vrefCalibrationWA = 0;
	}
#endif
#endif

	if( ckDelay == MV_PARAMS_UNDEFINED )
		ckDelay = 160;
	caDelay = 0;
	delayEnable = 1;
	freqVal[DDR_FREQ_LOW_FREQ] = dfsLowFreq = 120;
	calibrationUpdateControl = 1;

	initFreq = topologyMap->interfaceParams[firstActiveIf].memoryFreq;

#ifdef CONFIG_ARMADA_38X
    /* For a38x Only, change to 2T mode to resolve low freq instability */
    mode2T = 1;
#endif

#ifdef CONFIG_DDR3
    ddr3TipA38xGetMediumFreq(devNum, &mediumFreq);
#endif
    return GT_OK;
}

GT_STATUS ddr3A38xUpdateTopologyMap(GT_U32 devNum, MV_HWS_TOPOLOGY_MAP* topologyMap)
{
    GT_U32 interfaceId = 0;
    MV_HWS_DDR_FREQ freq;
	
    ddr3TipA38xGetInitFreq(devNum, &freq);

	topologyMap->interfaceParams[interfaceId].memoryFreq = freq;

	/* re-calc topology parameters according to topology updates (if needed) */
	CHECK_STATUS(mvHwsDdr3TipLoadTopologyMap(devNum, topologyMap));

    return GT_OK;
}

GT_STATUS ddr3TipInitA38x
(
    GT_U32  devNum,
    GT_U32  boardId
) 
{
	MV_HWS_TOPOLOGY_MAP* topologyMap = ddr3TipGetTopologyMap(devNum);

	if(NULL == topologyMap)
		return GT_FAIL;

	ddr3A38xUpdateTopologyMap(devNum, topologyMap);

    ddr3TipInitA38xSilicon(devNum, boardId);
    return GT_OK;
}

GT_STATUS ddr3TipA38xGetInitFreq
(
    GT_STATUS       devNum,
    MV_HWS_DDR_FREQ *freq
)
{
	GT_U32 uiReg, refClkSatR;

    /* Read sample at reset setting */
    uiReg = (MV_REG_READ(REG_DEVICE_SAR1_ADDR)>> RST2_CPU_DDR_CLOCK_SELECT_IN_OFFSET) & RST2_CPU_DDR_CLOCK_SELECT_IN_MASK;

	refClkSatR = MV_REG_READ(DEVICE_SAMPLE_AT_RESET2_REG);
	if(((refClkSatR >> DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_OFFSET) & 0x1) == DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_25MHZ){
		switch(uiReg) {
	#ifdef CONFIG_DDR3
		case 0x1:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Warning: Unsupported freq mode for 333Mhz configured(%d)\n", uiReg));
		case 0x0:
		    *freq = DDR_FREQ_333;
		    break;
		case 0x3:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Warning: Unsupported freq mode for 400Mhz configured(%d)\n", uiReg));
		case 0x2:
		    *freq = DDR_FREQ_400;
		    break;
		case 0xd:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Warning: Unsupported freq mode for 533Mhz configured(%d)\n", uiReg));
		case 0x4:
		    *freq = DDR_FREQ_533;
		    break;
		case 0x6:
		    *freq = DDR_FREQ_600;
		    break;
	#endif
		case 0x11:
		case 0x14:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Warning: Unsupported freq mode for 667Mhz configured(%d)\n", uiReg));
		case 0x8:
		    *freq = DDR_FREQ_667;
		    break;
		case 0x15:
		case 0x1b:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Warning: Unsupported freq mode for 800Mhz configured(%d)\n", uiReg));
		case 0xC:
		    *freq = DDR_FREQ_800;
		    break;
		case 0x10:
		    *freq = DDR_FREQ_933;
		    break;
		case 0x12:
		    *freq = DDR_FREQ_900;
		    break;
#ifdef CONFIG_DDR3
		case 0x13:
		    *freq = DDR_FREQ_933;
			ddr3AsyncModeAtTF = GT_TRUE;
		    break;
#else
		case 0x13:
		    *freq = DDR_FREQ_1000;
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Warning: Unsupported freq mode for 1000Mhz configured(%d)\n", uiReg));
		    break;
#endif
		default:
		    *freq = 0;
			return MV_NOT_SUPPORTED;
		}
	}
	else{ /*REFCLK 40 MHZ case*/
		switch(uiReg) {
	#ifdef CONFIG_DDR3
		case 0x3:
		    *freq = DDR_FREQ_400;
		    break;
		case 0x5:
		    *freq = DDR_FREQ_533;
		    break;
	#endif
		case 0xB:
		    *freq = DDR_FREQ_800;
		    break;
		case 0x1E:
		    *freq = DDR_FREQ_900;
		    break;
		default:
		    *freq = 0;
			return MV_NOT_SUPPORTED;
    	}
	}
    return GT_OK;
}

#ifdef CONFIG_DDR3
GT_STATUS ddr3TipA38xGetMediumFreq
(
    GT_STATUS       devNum,
    MV_HWS_DDR_FREQ *freq
)
{
	GT_U32 uiReg, refClkSatR;

    /* Read sample at reset setting */
    uiReg = (MV_REG_READ(REG_DEVICE_SAR1_ADDR)>> RST2_CPU_DDR_CLOCK_SELECT_IN_OFFSET) & RST2_CPU_DDR_CLOCK_SELECT_IN_MASK;

	refClkSatR = MV_REG_READ(DEVICE_SAMPLE_AT_RESET2_REG);
	if(((refClkSatR >> DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_OFFSET) & 0x1) == DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_25MHZ){
		switch(uiReg) {
		case 0x1:
		case 0x0:
		    *freq = DDR_FREQ_333; /*Medium is same as TF to run PBS in this freq*/
		    break;
		case 0x3:
		case 0x2:
		    *freq = DDR_FREQ_400; /*Medium is same as TF to run PBS in this freq*/
		    break;
		case 0xd:
		case 0x4:
		    *freq = DDR_FREQ_533;/*Medium is same as TF to run PBS in this freq*/
		    break;
		case 0x8:
		case 0x11:
		case 0x14:
		case 0x10:
		    *freq = DDR_FREQ_333;
		    break;
		case 0x15:
		case 0x1b:
		case 0xC:
		    *freq = DDR_FREQ_400;
		    break;
		case 0x6:
		    *freq = DDR_FREQ_300;
		    break;
		case 0x12:
		    *freq = DDR_FREQ_360;
		    break;
		case 0x13:
		    *freq = DDR_FREQ_400;
		    break;
		default:
		    *freq = 0;
			return MV_NOT_SUPPORTED;
		}
	}
	else{ /*REFCLK 40 MHZ case*/
		switch(uiReg) {
		case 0x3:
		    *freq = DDR_FREQ_400;/*Medium is same as TF to run PBS in this freq*/
		    break;
		case 0x5:
		    *freq = DDR_FREQ_533;/*Medium is same as TF to run PBS in this freq*/
		    break;
		case 0xB:
		    *freq = DDR_FREQ_400;
		    break;
		case 0x1E:
		    *freq = DDR_FREQ_360;
		    break;
		default:
		    *freq = 0;
			return MV_NOT_SUPPORTED;
		}
	}
    return GT_OK;
}
#endif


GT_U32 ddr3TipGetInitFreq()
{
    MV_HWS_DDR_FREQ freq;

	ddr3TipA38xGetInitFreq(0, &freq);

	return freq;
	
}


static GT_STATUS ddr3TipA38xSetDivider
(
	GT_U8							devNum,
    GT_U32                			interfaceId,
    MV_HWS_DDR_FREQ                 frequency
)
{
	GT_U32 divider = 0;
    GT_U32 sarVal, refClkSatR;

	if (interfaceId != 0) {
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("A38x does not support interface 0x%x\n", interfaceId));
		return GT_BAD_PARAM;
	}

	/* get VCO freq index */
	sarVal = (MV_REG_READ(REG_DEVICE_SAR1_ADDR)>> RST2_CPU_DDR_CLOCK_SELECT_IN_OFFSET) & RST2_CPU_DDR_CLOCK_SELECT_IN_MASK;

	refClkSatR = MV_REG_READ(DEVICE_SAMPLE_AT_RESET2_REG);
	if(((refClkSatR >> DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_OFFSET) & 0x1) == DEVICE_SAMPLE_AT_RESET2_REG_REFCLK_25MHZ){
    	divider = A38xVcoFreqPerSarRefClk25Mhz[sarVal]/freqVal[frequency];
	}
	else{
    	divider = A38xVcoFreqPerSarRefClk40Mhz[sarVal]/freqVal[frequency];
	}

	if( (ddr3AsyncModeAtTF == GT_TRUE) && (freqVal[frequency] > 400) ){

		/*Set Async Mode*/
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x20220, 0x1000, 0x1000));
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE42F4, 0x200, 0x200));

		/*Wait for Asunc mode setup*/
		mvOsDelay(5);

		/*Set KNL values*/
		if( frequency == DDR_FREQ_933){
			CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE42F0, 0x804A002, 0xFFFFFFFF));
		}
	}
	else{
		/*Set Sync mode*/
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x20220, 0x0, 0x1000));
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE42F4, 0x0, 0x200));

		/* cpupll_clkdiv_reset_mask */
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE4264, 0x1f, 0xFF));

		/* cpupll_clkdiv_reload_smooth */
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE4260, (0x2 << 8), (0xFF << 8)));

		/* cpupll_clkdiv_relax_en */
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE4260, (0x2 << 24), (0xFF << 24)));

		/* write the divider */
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE4268, (divider << 8), (0x3F << 8)));

		/* set cpupll_clkdiv_reload_ratio */
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE4264, (1 << 8), (1 << 8)));

		/* undet cpupll_clkdiv_reload_ratio */
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE4264, 0, (1 << 8)));

		/* clear cpupll_clkdiv_reload_force */
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE4260, 0, (0xFF << 8)));

		/* clear cpupll_clkdiv_relax_en */
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE4260, 0, (0xFF << 24)));

		/* clear cpupll_clkdiv_reset_mask */
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0xE4264, 0, 0xFF));
	}

	/* Dunit training clock + 1:1/2:1 mode */
	CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x18488, ((ddr3TipClockMode(frequency) & 0x1) << 16), (1 << 16)));
	CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x1524, ((ddr3TipClockMode(frequency)-1) << 15), (1 << 15)));

	return GT_OK;
}

/******************************************************************************
* return 1 of core/DUNIT clock ration is 1 for given freq, 0 if clock ratios is 2:1
*/
GT_U8    ddr3TipClockMode( GT_U32 frequency )
{
	if((frequency == DDR_FREQ_LOW_FREQ) || (freqVal[frequency] <= 400)){
		return 1;
	}
	else{
		return 2;
	}
}


/******************************************************************************
* external read from memory
*/
GT_STATUS    ddr3TipExtRead
( 
    GT_U32      devNum,
    GT_U32      interfaceId,
    GT_U32      regAddr,
    GT_U32      numOfBursts,
    GT_U32      *data
)
{
    GT_U32 burstNum;

    for(burstNum=0 ; burstNum < numOfBursts*8; burstNum++)
    {
		data[burstNum] = * (volatile unsigned int *) (regAddr + 4* burstNum);
    }

    return GT_OK;
}


/******************************************************************************
* external write to memory
*/
GT_STATUS    ddr3TipExtWrite
(
    GT_U32      devNum,
    GT_U32      interfaceId,
    GT_U32      regAddr,
    GT_U32      numOfBursts,
    GT_U32      *data
)
{
    GT_U32 burstNum;

    for(burstNum=0 ; burstNum < numOfBursts*8; burstNum++)
    {
		*(volatile unsigned int *) (regAddr+4*burstNum) = data[burstNum];
    }

    return GT_OK;
}

MV_STATUS ddr3SiliconPreInit(void) 
{
	MV_STATUS result;

	result = ddr3SiliconInit();

	return result;
}
MV_STATUS ddr3PostRunAlg(void) 
{
	return MV_OK;
}

MV_STATUS ddr3SiliconPostInit(void)
{
	MV_HWS_TOPOLOGY_MAP* topologyMap = ddr3TipGetTopologyMap(0);

	/*Set half bus width*/
	if(DDR3_IS_16BIT_DRAM_MODE(topologyMap->activeBusMask)){
	   	CHECK_STATUS(mvHwsDdr3TipIFWrite(0, ACCESS_TYPE_UNICAST, PARAM_NOT_CARE, REG_SDRAM_CONFIG_ADDR, 0x0, 0x8000));
	}

	return MV_OK;
}

GT_STATUS ddr3TipA38xGetDeviceInfo ( GT_U8      devNum,
									MV_DDR3_DEVICE_INFO * infoPtr)
{
	infoPtr->deviceId = 0x6800;
	infoPtr->ckDelay = ckDelay;

	return GT_OK;
}


