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
* mvHwsDdr3msys_ac3.c
*
* DESCRIPTION: DDR3 AC3 configuration
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 50 $
******************************************************************************/

#define DEFINE_GLOBALS
#include "mv_os.h"
#include "ddr3_msys_ac3.h"
#include "ddr3_msys_ac3_tip_training_static.h"
#include "ddr3_hws_sil_training.h"
#include "mvXor.h"

#include "mvSiliconIf.h"
#include "mvDdr3TrainingIpStatic.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpDef.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvDdr3LoggingDef.h"

/************************** definitions ******************************/

#define AC3_NUM_BYTES                  (3)
#define AC3_NUMBER_OF_INTERFACES        5

#define AC3_PLL_REG(a,b,c)  ((a << 12) + (b << 8) + (c << 2))
#define R_MOD_W(writeData,readData,mask) ((writeData & mask)|(readData & (~mask)))

/************************** globals ******************************/

extern MV_BOOL ddr3IfEccEnabled(void);
extern GT_U32 maskTuneFunc;
extern GT_U32 delayEnable, ckDelay, caDelay;
extern GT_U32 firstActiveIf;
extern MV_HWS_DDR_FREQ initFreq;
extern GT_U8  debugTrainingAccess;
extern GT_U8  debugTrainingAc3;
extern MV_HWS_DDR_FREQ mediumFreq;
extern GT_U8 calibrationUpdateControl; /*2 external only, 1 is internal only*/
extern GT_U32 dfsLowFreq;
extern GT_U32 freqVal[];

GT_U32  pipeMulticastMask;

static GT_U8 Ac3BwPerFreq[DDR_FREQ_LIMIT] =
{
    0x3, /*DDR_FREQ_100*/
    0x4, /*DDR_FREQ_400*/
    0x4, /*DDR_FREQ_533*/
    0x5, /*DDR_FREQ_667*/
    0x5, /*DDR_FREQ_800*/
    0x5, /*DDR_FREQ_933*/
    0x5, /*DDR_FREQ_1066*/
    0x3, /*DDR_FREQ_311*/
    0x3, /*DDR_FREQ_333*/
    0x4,  /*DDR_FREQ_467*/
    0x5,  /*DDR_FREQ_850,*/
    0x5,  /*DDR_FREQ_600,*/
    0x3,  /*DDR_FREQ_300,*/
	0x5,  /*DDR_FREQ_900*/
	0x3  /*DDR_FREQ_360*/
};

static GT_U8 Ac3RatePerFreq[DDR_FREQ_LIMIT] =
{
/*TBD*/
    0x1, /*DDR_FREQ_100*/
    0x2, /*DDR_FREQ_400*/
    0x2, /*DDR_FREQ_533*/
    0x2, /*DDR_FREQ_667*/
    0x2, /*DDR_FREQ_800*/
    0x3, /*DDR_FREQ_933*/
    0x3, /*DDR_FREQ_1066*/
    0x1, /*DDR_FREQ_311*/
    0x1, /*DDR_FREQ_333*/
    0x2,  /*DDR_FREQ_467*/
    0x2,  /*DDR_FREQ_850,*/
    0x2,  /*DDR_FREQ_600,*/
    0x1,  /*DDR_FREQ_300,*/
	0x3,  /*DDR_FREQ_900*/
	0x1  /*DDR_FREQ_360*/
};

static GT_U16 Ac3VcoFreqPerSar[] =
{
	400,
	533,
	2000,
	800,
	2000,
	800,
	800,
	800
};

GT_U32 DQbitMap2Phypin[] =
{
    0,  1,  2,  3,  10,  7,  8,  9, //0
    0,  1,  2,  3,  10,  7,  8,  9,	//1
    0,  1,  2,  3,  10,  7,  8,  9,	//placeholder
    0,  1,  2,  3,  10,  7,  8,  9,	//placeholder
    0,  1,  2,  3,  10,  7,  8,  9,	//ECC
};

/************************** pre-declaration ******************************/

extern MV_STATUS ddr3SiliconGetDdrTargetFreq(MV_U32 *uiDdrFreq);

GT_STATUS ddr3TipAc3GetFreqConfig
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 freq,
	MV_HWS_TIP_FREQ_CONFIG_INFO		*freqConfigInfo
);

static GT_STATUS ddr3TipAc3SetDivider
(
	GT_U8							devNum,
	GT_U32                  		interfaceId,
    MV_HWS_DDR_FREQ                 freq
);

GT_STATUS ddr3Ac3UpdateTopologyMap
(
    GT_U32 devNum,
	MV_HWS_TOPOLOGY_MAP* topologyMap
);

GT_STATUS ddr3TipAc3GetInitFreq
(
    GT_STATUS       devNum,
    GT_U32          interfaceId,
    MV_HWS_DDR_FREQ *freq
);

GT_STATUS ddr3TipAc3GetMediumFreq
(
    GT_STATUS       devNum,
    MV_HWS_DDR_FREQ *freq
);

GT_STATUS    ddr3TipAc3IFRead
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                *data,
    GT_U32                mask
);

GT_STATUS    ddr3TipAc3IFWrite
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                dataValue,
    GT_U32                mask
);

GT_STATUS ddr3TipAC3GetDeviceInfo
(
	GT_U8      devNum,
	MV_DDR3_DEVICE_INFO * infoPtr
);

GT_U8    ddr3TipClockMode( GT_U32 frequency );

/************************** Server Access ******************************/

GT_STATUS ddr3TipAc3ServerRegWrite
(
	GT_U32 devNum,
	GT_U32 regAddr,
	GT_U32 dataValue
)
{
#ifdef ASIC_SIMULATION
	return GT_OK;
#else
	return hwsServerRegSetFuncPtr(devNum, regAddr, dataValue);
#endif
}

GT_STATUS ddr3TipAc3ServerRegRead
(
	GT_U32 devNum,
	GT_U32 regAddr,
	GT_U32 *dataValue,
	GT_U32 regMask
)
{
#ifdef ASIC_SIMULATION
	*dataValue = 0;
	return GT_OK;
#else
	hwsServerRegGetFuncPtr(devNum, regAddr, dataValue);

	*dataValue  = *dataValue & regMask;
#endif
	return GT_OK;
}

/******************************************************************************
* Name:     ddr3TipAc3GetFreqConfig.
* Desc:
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS ddr3TipAc3GetFreqConfig
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 freq,
	MV_HWS_TIP_FREQ_CONFIG_INFO		*freqConfigInfo
)
{
    if (Ac3BwPerFreq[freq] == 0xff)
    {
        return GT_NOT_SUPPORTED;
    }

    if (freqConfigInfo == NULL)
    {
        return GT_BAD_PARAM;
    }

    freqConfigInfo->bwPerFreq = Ac3BwPerFreq[freq];
    freqConfigInfo->ratePerFreq = Ac3RatePerFreq[freq];
    freqConfigInfo->isSupported = GT_TRUE;

    return GT_OK;
}

/******************************************************************************
* Name:     ddr3TipAc3IFWrite.
* Desc:
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipAc3IFWrite
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
        if (debugAc3 >= 2)
        {
            //mvPrintf("ddr3TipAc3IFWrite active interface = %d\n",interfaceId);
        }
        /*CHECK_STATUS(ddr3DunitAccessRead(devNum, ACCESS_TYPE_UNICAST,interfaceId, regAddr,  &uiDataRead, MASK_ALL_BITS));
        dataValue = (uiDataRead & (~mask)) | (dataValue & mask);*/
		CHECK_STATUS(ddr3TipAc3IFRead(devNum, ACCESS_TYPE_UNICAST,interfaceId, regAddr,  &uiDataRead, MASK_ALL_BITS));
        dataValue = (uiDataRead & (~mask)) | (dataValue & mask);
    }

    if (debugAc3 >= 1)
    {
       // mvPrintf("Dunit Write: interface %d  access %d Address 0x%x Data 0x%x\n", interfaceId, interfaceAccess, regAddr, dataValue);
    }

    MV_REG_WRITE(regAddr, dataValue);
	//mvPrintf("write: 0x%x = 0x%x\n",regAddr,dataValue);

    return GT_OK;
}

/******************************************************************************
* Name:     ddr3TipAc3IFRead.
* Desc:
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipAc3IFRead
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
	if (interfaceId !=0)
	{
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("AC3 wrong interface ID %d\n", interfaceId));
		return GT_FAIL;
	}
	data[interfaceId] = MV_REG_READ(regAddr) & mask;
    //mvPrintf("Read: 0x%x = 0x%x\n",regAddr,*data);
	if (debugAc3 >= 2)
	{
		//mvPrintf("DunitRead   0x%x: 0x%x (mask 0x%x)\n",  regAddr,  *data, mask);
	}

    return GT_OK;
}

/******************************************************************************
* Name:     ddr3TipAc3SelectDdrController.
* Desc:     Enable/Disable access to Marvell's server.
* Args:     devNum     - device number
*           enable        - whether to enable or disable the server
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipAc3SelectDdrController
(
    GT_U8    devNum,
    GT_BOOL  enable
)
{
    GT_U32 uiReg;
	//mvPrintf("ddr3TipAc3SelectDdrController\n");
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
* Name:     ddr3TipInitAc3Silicon.
* Desc:     init Training SW DB.
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
static GT_STATUS ddr3TipInitAc3Silicon
(
    GT_U32  devNum,
    GT_U32  boardId
)
{
	GT_U32 interfaceId = 0;
	MV_HWS_TIP_STATIC_CONFIG_INFO staticConfig;
    MV_HWS_TIP_CONFIG_FUNC_DB configFunc;
	MV_HWS_DDR_FREQ uiDdrFreq;
	MV_STATUS status;
	MV_HWS_TOPOLOGY_MAP* topologyMap = ddr3TipGetTopologyMap(devNum);
	GT_U8 numOfBusPerInterface = 5;

    GT_U32 boardOffset = boardId * AC3_NUMBER_OF_INTERFACES *numOfBusPerInterface;

    /* new read leveling version */
    staticConfig.siliconDelay = Ac3SiliconDelayOffset[boardId];
    staticConfig.packageTraceArr = Ac3PackageRoundTripDelayArray;
    staticConfig.boardTraceArr = &Ac3BoardRoundTripDelayArray[boardOffset];

    configFunc.tipDunitReadFunc = ddr3TipAc3IFRead;
    configFunc.tipDunitWriteFunc = ddr3TipAc3IFWrite;
    configFunc.tipDunitMuxSelectFunc = ddr3TipAc3SelectDdrController;
    configFunc.tipGetFreqConfigInfoFunc = ddr3TipAc3GetFreqConfig;
	configFunc.tipSetFreqDividerFunc = ddr3TipAc3SetDivider;
	configFunc.tipGetDeviceInfoFunc = ddr3TipAC3GetDeviceInfo;
	configFunc.tipGetTemperature = NULL;
	configFunc.tipGetClockRatio = ddr3TipClockMode;

    mvHwsDdr3TipInitConfigFunc(devNum, &configFunc);

	ddr3TipRegisterDqTable(devNum, DQbitMap2Phypin);

	/*Set device attributes*/
	ddr3TipDevAttrInit(devNum);
	ddr3TipDevAttrSet(devNum, MV_ATTR_TIP_REV, MV_TIP_REV_3);
	ddr3TipDevAttrSet(devNum, MV_ATTR_PHY_EDGE, MV_DDR_PHY_EDGE_NEGATIVE);
	ddr3TipDevAttrSet(devNum, MV_ATTR_OCTET_PER_INTERFACE, numOfBusPerInterface);
	ddr3TipDevAttrSet(devNum, MV_ATTR_INTERLEAVE_WA, GT_FALSE);

#ifdef STATIC_ALGO_SUPPORT
    ddr3TipInitStaticConfigDb(devNum, &staticConfig);
#endif
	status = ddr3TipAc3GetInitFreq(devNum, interfaceId, &uiDdrFreq);
	if (MV_OK != status) {
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("DDR3 silicon get target frequency - FAILED 0x%x\n", status));
		return status;
	}

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

	/*Supplementary not supported for ECC modes*/
	if( MV_TRUE == ddr3IfEccEnabled()){
		maskTuneFunc &=	~WRITE_LEVELING_SUPP_TF_MASK_BIT;
		maskTuneFunc &=	~WRITE_LEVELING_SUPP_MASK_BIT;
	}

	if( ckDelay == MV_PARAMS_UNDEFINED )
		ckDelay = 200;
	delayEnable = 1;
	caDelay = 0;
	calibrationUpdateControl = 1;
	freqVal[DDR_FREQ_LOW_FREQ] = dfsLowFreq = 130;

	initFreq = topologyMap->interfaceParams[interfaceId].memoryFreq;

    ddr3TipAc3GetMediumFreq(devNum, &mediumFreq);
    return GT_OK;
}

GT_STATUS ddr3Ac3UpdateTopologyMap(GT_U32 devNum, MV_HWS_TOPOLOGY_MAP* topologyMap)
{
    GT_U32 interfaceId = 0; /* only 1 interface in AC3 */
    MV_HWS_DDR_FREQ freq;

	/* read S@R frequency configuration */
	ddr3TipAc3GetInitFreq(devNum, interfaceId, &freq);

	topologyMap->interfaceParams[interfaceId].memoryFreq = freq;

	/* re-calc topology parameters according to topology updates (if needed) */
	CHECK_STATUS(mvHwsDdr3TipLoadTopologyMap(devNum, topologyMap));

    return GT_OK;
}

GT_STATUS ddr3TipInitAc3
(
    GT_U32  devNum,
    GT_U32  boardId
)
{
	MV_HWS_TOPOLOGY_MAP* topologyMap = ddr3TipGetTopologyMap(devNum);

	if(NULL == topologyMap)
		return GT_FAIL;

	ddr3Ac3UpdateTopologyMap(devNum, topologyMap);

    ddr3TipInitAc3Silicon(devNum, boardId);
    return GT_OK;
}

GT_STATUS ddr3TipAc3GetInitFreq
(
    GT_STATUS       devNum,
	GT_U32			interfaceId,
    MV_HWS_DDR_FREQ *freq
)
{
	GT_U32 data;

    /* calc SAR */
    CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum, REG_DEVICE_SAR1_ADDR,  &data, MASK_ALL_BITS ));
    data = (data >> PLL1_CNFIG_OFFSET) & PLL1_CNFIG_MASK;

    switch(data)
    {
        case 0:
		case 5:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("MSYS_AC3 DDR_FREQ_400\n"));
            *freq = DDR_FREQ_400;
            break;
        case 1:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("MSYS_AC3 DDR_FREQ_533\n"));
            *freq = DDR_FREQ_533;
            break;
        case 2:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("MSYS_AC3 DDR_FREQ_667\n"));
            *freq = DDR_FREQ_667;
            break;
        case 3:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("MSYS_AC3 DDR_FREQ_800\n"));
            *freq = DDR_FREQ_800;
            break;
        default:
            DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("Freq_AC3 SAR Unknown\n"));
            *freq = DDR_FREQ_LIMIT;
            return GT_BAD_PARAM;
    }
    return GT_OK;
}

GT_STATUS ddr3TipAc3GetMediumFreq
(
    GT_STATUS       devNum,
    MV_HWS_DDR_FREQ *freq
)
{
	/*TODO allow to user configure sync/async mode and minimum allowed mid freq*/
	/*PBS was disabled, so midFreq 533 disabled also
		to not work with TF 533 in ASYNC mode - this implies on performance*/
    *freq = DDR_FREQ_400;
    return GT_OK;
}

GT_U32 ddr3TipGetInitFreq()
{
	GT_U8			devNum = 0;
    GT_U32          interfaceId = 0;
	MV_HWS_DDR_FREQ freq;

	ddr3TipAc3GetInitFreq(devNum, interfaceId, &freq);

	return freq;
}

static GT_STATUS ddr3TipAc3SetDivider
(
	GT_U8							devNum,
	GT_U32                  		interfaceId,
    MV_HWS_DDR_FREQ                 frequency
)
{
	GT_U32 divider = 0;
	GT_U32 divRatio = 0;
    GT_U32 sarVal;
	GT_U32 data = 0;
	GT_U32 select = 0;
	GT_U8 value;
	MV_HWS_TOPOLOGY_MAP* topologyMap = ddr3TipGetTopologyMap(devNum);

	/* Dunit training clock + 1:1/2:1 mode */
	divRatio = (ddr3TipClockMode(frequency) << 30);
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF8294,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF8294,  R_MOD_W(divRatio, data, (0x3 << 30))));

	/* Configure Dunit to 1:1 in case of DLL off mode else 2:1*/
	value = (ddr3TipClockMode(frequency) == 1)? 0 : 1;
	CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, interfaceId, 0x1524, (value << 15), (1 << 15)));

	select = (frequency == topologyMap->interfaceParams[firstActiveIf].memoryFreq)?(0 << 29) : (1 << 29);//TF is the function mode so the selector have to be 0.
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF8294,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF8294,  R_MOD_W(select, data, (0x1 << 29))));
	/* get VCO freq index */
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum, REG_DEVICE_SAR1_ADDR,  &data, MASK_ALL_BITS ));
	sarVal = (data >> PLL1_CNFIG_OFFSET) & PLL1_CNFIG_MASK;

    divider = Ac3VcoFreqPerSar[sarVal]/freqVal[frequency];
    DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("SAR value %d divider %d\n", sarVal, divider));

	/* cpupll_clkdiv_reset_mask */
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF8260,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF8260,  R_MOD_W(0x2, data, 0x2)));

	/* cpupll_clkdiv_reload_smooth */
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF825C,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF825C,  R_MOD_W((0x2 << 22), data, (0xFF << 22))));

	/* cpupll_clkdiv_relax_en */
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF825C,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF825C,  R_MOD_W((0x2 << 14), data, (0xFF << 14))));

	/* write the divider */
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF8264,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF8264,  R_MOD_W((divider << 12), data, (0x3F << 12))));

	/* set cpupll_clkdiv_reload_ratio */
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF8270,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF8270,  R_MOD_W((1 << 10), data, (1 << 10))));

	/* undet cpupll_clkdiv_reload_ratio */
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF8270,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF8270,  R_MOD_W(0x0, data, (1 << 10))));

	/* clear cpupll_clkdiv_reload_force */
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF8268,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF8268,  R_MOD_W(0x0, data, (0xFF << 21))));

	/* clear cpupll_clkdiv_relax_en */
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF825C,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF825C,  R_MOD_W(0x0, data, (0xFF << 14))));
	/* clear cpupll_clkdiv_reload_smooth */
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF825C,  R_MOD_W(0x0, data, (0xFF << 22))));

	/* clear cpupll_clkdiv_reset_mask */
	CHECK_STATUS(ddr3TipAc3ServerRegRead(devNum,  0xF8260,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipAc3ServerRegWrite(devNum, 0xF8260,  R_MOD_W(0x0, data, 0x2 )));

	return GT_OK;
}

/******************************************************************************
* return 1 of core/DUNIT clock ration is 1 for given freq, 0 if clock ratios is 2:1
*/
GT_U8    ddr3TipClockMode( GT_U32 frequency )
{
	if(frequency == DDR_FREQ_LOW_FREQ){
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

    for(burstNum=0 ; burstNum < numOfBursts*4; burstNum++)// 4 per 16 bit I/F
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

GT_STATUS ddr3TipAC3GetDeviceInfo ( GT_U8      devNum,
									MV_DDR3_DEVICE_INFO * infoPtr)
{
	infoPtr->deviceId = 0xF400;
	infoPtr->ckDelay = ckDelay;

	return GT_OK;
}


