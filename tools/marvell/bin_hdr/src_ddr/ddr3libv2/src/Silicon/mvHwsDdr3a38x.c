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
extern GT_U32 delayEnable, ckDelay, ckDelay_16, caDelay;
GT_U32  pipeMulticastMask;

#define A38X_NUM_BYTES                  (3)
#define A38X_NUMBER_OF_INTERFACES		5

#define SAR_DEV_ID_OFFS                         27
#define SAR_DEV_ID_MASK                         0x7

#define A38X_PLL_REG(a,b,c)  ((a << 12) + (b << 8) + (c << 2))
#define R_MOD_W(writeData,readData,mask) ((writeData & mask) + (data & (~mask)))

extern MV_BOOL ddr3IfEccEnabled(void);
/************************** pre-declaration ******************************/
extern GT_U32 maskTuneFunc;
extern GT_U32 rlVersion;
extern GT_BOOL rlMidFreqWA;
extern GT_U8 calibrationUpdateControl; /*2 external only, 1 is internal only*/


#ifdef CONFIG_DDR3
static GT_U16 freqVal[DDR_FREQ_LIMIT] =
{
    130, /*DDR_FREQ_LOW_FREQ*/
    400, /*DDR_FREQ_400,*/
    533, /*DDR_FREQ_533,*/
    666, /*DDR_FREQ_667,*/
    800, /*DDR_FREQ_800,*/
    933, /*DDR_FREQ_933,*/
   1066, /*DDR_FREQ_1066,*/
    311, /*DDR_FREQ_311,*/
    333, /*DDR_FREQ_333,*/
    467, /*DDR_FREQ_467,*/
    850, /*DDR_FREQ_850,*/
    600, /*DDR_FREQ_600,*/
    300, /*DDR_FREQ_300,*/
	900,  /*DDR_FREQ_900*/
	360,  /*DDR_FREQ_360*/
	1000  /*DDR_FREQ_1000*/
};
#else /* CONFIG_DDR4 */
static GT_U16 freqVal[DDR_FREQ_LIMIT] =
{
    130,    /* DDR_FREQ_LOW_FREQ */
    666,    /* DDR_FREQ_667 */
    800,    /* DDR_FREQ_800 */
    933,    /* DDR_FREQ_933 */
    1066,  /* DDR_FREQ_1066 */
	900,  	/*DDR_FREQ_900*/
	1000  	/*DDR_FREQ_1000*/
};
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
/*TBD*/
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

static GT_U16 A38xVcoFreqPerSar[] =
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

#ifdef CONFIG_ARMADA_38X_Z1_OBSOLETE
GT_U32 DQbitMap2Phypin[] =
{
    1, 0,  2, 3,  6, 7, 8,  9, 	//0
    0, 1, 10, 3,  6, 7, 8,  9,	//1
    8, 1,  2, 0, 10, 9, 6,  7,	//2
    0, 1,  2, 3,  7, 8, 6, 10,	//3
    0, 1,  2, 3,  6, 7, 8,  9,	//4
};
#else
GT_U32 DQbitMap2Phypin[] =
{
	1, 0, 2, 6, 9, 8, 3, 7,	//0
	8, 9, 1, 7, 2, 6, 3, 0,	//1
	3, 9, 7, 8, 1, 0, 2, 6,	//2
	1, 0, 6, 2, 8, 3, 7, 9,	//3
	0, 1, 2, 9, 7, 8, 3, 6,	//4
};
#endif

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

    /* new read leveling version */
    configFunc.tipDunitReadFunc = ddr3TipA38xIFRead;
    configFunc.tipDunitWriteFunc = ddr3TipA38xIFWrite;
    configFunc.tipDunitMuxSelectFunc = ddr3TipA38xSelectDdrController;
    configFunc.tipGetFreqConfigInfoFunc = ddr3TipA38xGetFreqConfig;
	configFunc.tipSetFreqDividerFunc = ddr3TipA38xSetDivider;
	configFunc.tipGetDeviceInfoFunc = ddr3TipA38xGetDeviceInfo;
	configFunc.tipGetTemperature = ddr3CtrlGetJuncTemp;

    mvHwsDdr3TipInitConfigFunc(devNum, &configFunc);

	ddr3TipRegisterDqTable(devNum, DQbitMap2Phypin);

#ifdef STATIC_ALGO_SUPPORT
	{
		MV_HWS_TIP_STATIC_CONFIG_INFO staticConfig;
    	GT_U32 boardOffset = boardId * A38X_NUMBER_OF_INTERFACES *topologyMap->numOfBusPerInterface;

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

    rlVersion = 1;
#ifdef CONFIG_DDR3
    maskTuneFunc =     (SET_LOW_FREQ_MASK_BIT |
						LOAD_PATTERN_MASK_BIT |
						SET_MEDIUM_FREQ_MASK_BIT |
						WRITE_LEVELING_MASK_BIT |
//						LOAD_PATTERN_2_MASK_BIT |
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
#endif

	if( ckDelay == MV_PARAMS_UNDEFINED )
		ckDelay = 160;
	if( ckDelay_16 == MV_PARAMS_UNDEFINED )
		ckDelay_16 = 160;
	caDelay = 0;
	delayEnable = 1;

	calibrationUpdateControl = 1;

	initFreq = topologyMap->interfaceParams[firstActiveIf].memoryFreq;

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
	GT_U32 uiReg;

    /* Read sample at reset setting */
    uiReg = (MV_REG_READ(REG_DEVICE_SAR1_ADDR)>> RST2_CPU_DDR_CLOCK_SELECT_IN_OFFSET) & RST2_CPU_DDR_CLOCK_SELECT_IN_MASK;
    switch(uiReg) {
#ifdef CONFIG_DDR3
    case 0x0:
    case 0x1:
        *freq = DDR_FREQ_333;
        break;
    case 0x2:
    case 0x3:
        *freq = DDR_FREQ_400;
        break;
    case 0x4:
    case 0xd:
        *freq = DDR_FREQ_533;
        break;
    case 0x6:
        *freq = DDR_FREQ_600;
        break;
#endif
    case 0x8:
	case 0x11:
	case 0x14:
        *freq = DDR_FREQ_667;
        break;
	case 0xC:
	case 0x15:
	case 0x1b:
        *freq = DDR_FREQ_800;
        break;
	case 0x10:
        *freq = DDR_FREQ_933;
        break;
	case 0x12:
        *freq = DDR_FREQ_900;
        break;
	case 0x13:
        *freq = DDR_FREQ_1000;
        break;
    default:
        *freq = 0;
	    return MV_NOT_SUPPORTED;
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
	GT_U32 uiReg;

    /* Read sample at reset setting */
    uiReg = (MV_REG_READ(REG_DEVICE_SAR1_ADDR)>> RST2_CPU_DDR_CLOCK_SELECT_IN_OFFSET) & RST2_CPU_DDR_CLOCK_SELECT_IN_MASK;
    switch(uiReg) {
    case 0x0:
    case 0x1:
        *freq = DDR_FREQ_333; /*Medium is same as TF to run PBS in this freq*/
        break;
    case 0x2:
    case 0x3:
        *freq = DDR_FREQ_400; /*Medium is same as TF to run PBS in this freq*/
        break;
    case 0x4:
    case 0xd:
        *freq = DDR_FREQ_533;
        break;
    case 0x8:
	case 0x11:
	case 0x14:
        *freq = DDR_FREQ_333;
        break;
	case 0xC:
	case 0x15:
	case 0x1b:
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
    GT_U32 sarVal;

	if (interfaceId != 0) {
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("A38x does not support interface 0x%x\n", interfaceId));
		return GT_BAD_PARAM;
	}

	/* get VCO freq index */
	sarVal = (MV_REG_READ(REG_DEVICE_SAR1_ADDR)>> RST2_CPU_DDR_CLOCK_SELECT_IN_OFFSET) & RST2_CPU_DDR_CLOCK_SELECT_IN_MASK;

    divider = A38xVcoFreqPerSar[sarVal]/freqVal[frequency];

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

	/* Dunit training clock + 1:1 mode */
	if((frequency == DDR_FREQ_LOW_FREQ) || (freqVal[frequency] <= 400)) {
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x18488, (1 << 16), (1 << 16)));
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x1524, (0 << 15), (1 << 15)));
	}
	else {
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x18488, 0, (1 << 16)));
		CHECK_STATUS(ddr3TipA38xIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, 0x1524, (1 << 15), (1 << 15)));
	}

	return GT_OK;
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


