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
* mvHwsDdr3NP5.c
*
* DESCRIPTION: DDR3 NP5 configuration
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 94 $
******************************************************************************/

#define DEFINE_GLOBALS

#include "mvSiliconIf.h"
#include "mvHwsDdr3NP5.h"
#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3TrainingIpPbs.h"
#include "mvDdr3TrainingIpDef.h"
#include "mvDdr3TrainingIpStatic.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3TrainingIpDef.h"

#define  GT_DEBUG_TRAINING_NP5

extern GT_U8  debugTrainingAccess;

#ifdef  GT_DEBUG_TRAINING_NP5
#include "gtOsIo.h"
#ifdef FreeRTOS
#include "printf.h"
#define osPrintf    printf
#endif
#define DEBUG_TRAINING_IP(level,s) if (level >= debugTrainingAccess) {osPrintf s;} 
#else
#define DEBUG_TRAINING_IP(level,s)				
#endif

#define NUM_BYTES                (3)
#define NP5_NUMBER_OF_INTERFACES (12)
#define CLIENT_FIELD(interfaceId)		(interfaceId << 15)
/*#define XSB_MAPPING(interfaceId, interfaceAccess, regaddr) ((regaddr)+ CLIENT_FIELD(interfaceId)+XSB_BASE_ADDR)*/
#define GET_CLIENT_FIELD(interfaceAccess,interfaceId) ((interfaceAccess == ACCESS_TYPE_MULTICAST) ? CLIENT_FIELD(BROADCAST_ID):CLIENT_FIELD(interfaceId))
#define XSB_MAPPING(interfaceId, interfaceAccess, regaddr)   (regaddr+ XSB_BASE_ADDR + GET_CLIENT_FIELD(interfaceAccess,interfaceId))

GT_U32 freqDelay = 300;
GT_U32 useDivider = 0;
GT_U32 isPllOld = 0;
GT_U32 debugNewPhy1 = 0;
GT_U32 odtConfig;

#define PLL_NUM_OF_IF (6)
/************************** pre-declaration ******************************/
/* Read 1st active interface */
extern GT_U32 maskTuneFunc;
extern MV_HWS_DDR_FREQ mediumFreq;
extern MV_HWS_DDR_FREQ lowFreq;
extern GT_U32 isPllBeforeInit;
extern GT_U32 dfsLowFreq;
extern MV_HWS_DDR_FREQ initFreq;
extern GT_U32 odtAdditional;
extern GT_U8  debugTraining;
extern GT_U32 delayEnable, ckDelay, caDelay;
extern GT_U8 calibrationUpdateControl; /*2 external only, 1 is internal only*/
extern GT_U32 dfsLowFreq;

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



GT_STATUS    ddr3TipPipeEnable
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_BOOL               enable
);

GT_STATUS    ddr3TipNp5IFRead
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                *data,
    GT_U32                mask 
);

GT_STATUS    ddr3TipNp5IFWrite
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                dataValue,
    GT_U32                mask 
);

#if 0
static GT_STATUS ddr3TipNp5SetDivider
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 frequency
);
#endif

static GT_STATUS ddr3TipNp5SetFreq
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 freq
);



static GT_STATUS ddr3TipGetFreqTablePtr
(
    MV_HWS_DDR_FREQ freq, 
    GT_U32**        pFreqTable
);

static GT_STATUS ddr3TipNp5IsAccessDone
(
    GT_U32 devNum,
    GT_U32 interfaceId
);

static GT_STATUS ddr3TipNp5SetPll
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 frequency
);

/************************** Globals ******************************/
GT_U32 debugNp5 = 0;
GT_U32  pipeMulticastMask;
/**************************** Static configuration data *********************/


/************* board delendent ***************/
typedef struct
{
    GT_U32 regAddr;
    GT_U32 bitClkDivReloadRatio;
    GT_U32 maskClkDivReloadRatio;
    GT_U32 bitSdrClkDivRatioFull;
    GT_U32 maskSdrClkDivRatioFull;
    GT_U32 bitDdrClkDivRatioFull;
    GT_U32 maskDdrClkDivRatioFull;
}dividerStruct;

dividerStruct dividerArr[] =
{
    { 0x000F8258, 18, 0x40000,    0, 0x3f,  6,  0xfc0   },
    { 0x000F8270, 21, 0x600000,   4, 0x3f0, 10, 0xfc00  },
    { 0x000F825c, 21, 0x600000,   4, 0x3f0, 10, 0xfc00  },
    { 0x000F8264, 30, 0x40000000, 0, 0x3f,  6,  0xfc0   },
    { 0x000F8274, 21, 0x600000,   4, 0x3f0, 10, 0xfc00  },
    { 0x000F826C, 21, 0x600000,   4, 0x3f0, 10, 0xfc00  }
};

static MV_DFX_ACCESS interfaceMap[NP5_NUMBER_OF_INTERFACES] = 
{
	/* Pipe	Client*/
	{   7,	 0 },
	{   7,	 1 },
	{   7,	 2 },
	{   7,	 3 },
	{   7,	 4 },
	{   7,	 5 },
	{   7,	 6 },
	{   7,	 7 },
	{   7,	 8 },
	{   7,	 9 },
	{   7,	 10 },
	{   7,	 11 }
};


static RegData np5SpecificInitControllerConfig[] =
{
    {0x16D8,    0,          0x8},
    {0x15EC,    0,          0x2},
    {0x15EC,    0x8,        0xC},
    {0x15EC,    0x20,       0x30},
    {0x16D8,    2,          0x3},
    {0x14cc,    0x1,        0x1},
    {0x14cc,    0x4,        0x4},
    {0x14cc,    0x8,        0x18},
    {0x14cc,    0x12000,    0x7DFFE000},
    {0x1400,    0x7B00CC30, 0xffffffff},
    {0x1404,    0x36301800, 0xffffffff},
    {0x1408,    0x5415F3AA, 0xffffffff},
    {0x140C,    0x38401DFF, 0xffffffff},
    {0x1410,    0x1C000001, 0xffffffff},
    {0x1414,    0x700,      0xffffffff},
    {0x1424,    0x60F3FF,   0xffffffff},
    {0x1428,    0x11A940,   0xffffffff},
    {0x142C,    0x28C5134,  0xffffffff},
    {0x147C,    0xD771,     0xffffffff},
    {0x1494,    0x10000,    0xffffffff},
    {0x149C,    0x301,      0xffffffff},
    {0x14A8,    0x0,        0xffffffff},
    {0x1504,    0x3FFFFFE1, 0xffffffff},
    {0x150C,    0x7FFFFFE5, 0xffffffff},
    {0x1514,    0,          0xffffffff},
    {0x151C,    0,          0xffffffff},
    {0x1538,    0xB,        0xffffffff},
    {0x153C,    0xB,        0xffffffff},
    {0x15D0,    0x70,       0xffffffff},
    {0x15D4,    0,          0xffffffff},
    {0x15D8,    0x218,      0xffffffff},
    {0x15DC,    0x0,        0xffffffff},
    {0x15E0,    0x27,       0xffffffff},
    {0x15E4,    0x203C18,   0xffffffff},
	{0,   0, 0}
};


static GT_U32 np5BwPerFreq[DDR_FREQ_LIMIT] =
{
    0x2, /*DDR_FREQ_LOW_FREQ*/
    0x3, /*DDR_FREQ_400*/
    0x3, /*DDR_FREQ_533*/
    0x3, /*DDR_FREQ_667*/
    0x3, /*DDR_FREQ_800*/
    0x3, /*DDR_FREQ_933*/
    0x3, /*DDR_FREQ_1066*/
    0xFF, /*DDR_FREQ_311*/
    0xFF, /*DDR_FREQ_333*/
    0xFF, /*DDR_FREQ_467*/
    0x3,  /*DDR_FREQ_850*/
    0x5,  /*DDR_FREQ_600*/
    0xFF,  /*DDR_FREQ_300*/
	0x3 /*DDR_FREQ_900*/
};

static GT_U32 np5RatePerFreq[DDR_FREQ_LIMIT] =
{
   0x1, /*DDR_FREQ_LOW_FREQ,*/
   0x2, /*DDR_FREQ_400,*/
   0x2, /*DDR_FREQ_533,*/
   0x3, /*DDR_FREQ_667,*/
   0x3, /*DDR_FREQ_800,*/
   0x3, /*DDR_FREQ_933,*/
   0x3, /*DDR_FREQ_1066,*/
   0x2, /*DDR_FREQ_311*/
   0x2, /*DDR_FREQ_333*/
   0x2, /*DDR_FREQ_467*/
   0x3  /*DDR_FREQ_850*/
   0x3,  /*DDR_FREQ_600*/
   0x2,  /*DDR_FREQ_300*/
   0x3 /*DDR_FREQ_900*/
};



GT_U32 freqParams[DDR_FREQ_LIMIT][MV_HWS_FreqField_LIMIT] = 
/*   PLL_NDIV  PLL_MDIV PLL_KDIV  PLL_VCO_BAND  PLL_LPF_R1_CTRL */                  
{ 
    {  39,     0,       4,       2,        4 }, /* 100MHz/200Mb  */
    {  31,     0,       2,       1,        4 }, /* 400MHz/800Mb  */
    { 127,     2,       2,       2,        1 }, /* 533MHz/1066Mb */
    { 159,     2,       2,       3,        1 }, /* 667MHz/1333Mb */
    { 191,     2,       2,       5,        1 }, /* 800MHz/1600Mb */
    { 223,     2,       2,       6,        1 }, /* 933MHz/1866Mb */
    { 255,     2,       2,       7,        1 }, /* 1066MHz/2133Mb */
    { 1,       1,       1,       1,        1 }, /* 311Mhz */
    { 1,       1,       1,       1,        1 }, /* 333Mhz */
    { 1,       1,       1,       1,        1 }, /* 467Mhz */
    { 101,     2,       1,       1,        4 }, /* 850Mhz */
};  






GT_U32 phy1ValTable[DDR_FREQ_LIMIT] =
{
  0,   /* DDR_FREQ_LOW_FREQ */
  0xf, /* DDR_FREQ_400 */
  0xf, /* DDR_FREQ_533 */
  0xf, /* DDR_FREQ_667 */
  0xc, /* DDR_FREQ_800 */
  0x8, /* DDR_FREQ_933 */
  0x8, /* DDR_FREQ_1066 */
  0xf, /* DDR_FREQ_311 */
  0xf, /* DDR_FREQ_333 */
  0xf, /* DDR_FREQ_467 */
  0xc  /* DDR_FREQ_850 */
  0xC,  /*DDR_FREQ_600*/
  0xF,  /*DDR_FREQ_300*/
  0x8 	/*DDR_FREQ_900*/
};

static RegData pllConfig[] =
{

   {0xF8258   ,   0         , 0x4000},      /*clock ratio */
   {0xF8254   ,   0         , 0xc0000000},  /* clock enable */
   {0xF8270   ,   0         , 0x40000},     /*clock ratio */
   {0xF8270   ,   0         , 0xc},         /* clock enable */
   {0xF825c   ,   0         , 0x600000},    /*clock ratio */
   {0xF825c   ,   0         , 0xc},         /* clock enable */
   {0xF8264   ,   0         , 0x40000000},  /*clock ratio */
   {0xF8260   ,   0         , 0x60},        /* clock enable */
   {0xF8274   ,   0         , 0x600000},    /*clock ratio */
   {0xF8274   ,   0         , 0xc},         /* clock enable */
   {0xF826c   ,   0         , 0x40000000},  /*clock ratio */
   {0xF8268   ,   0         , 0x60}         /* clock enable */
};

/* pll values */
static RegData pllValueTable[] =
{
   {0xF82F0   ,0x2821f002  , 0xffffffff          },
   {0xF8300   ,0x2821f002  , 0xffffffff          },
   {0xF8308   ,0x2821f002  , 0xffffffff          },
   {0xF8310   ,0x2821f002  , 0xffffffff          },
   {0xF8318   ,0x2821f002  , 0xffffffff          },
   {0xF8320   ,0x2821f002  , 0xffffffff          }

};

/* pll configuration disable*/ 
static RegData pllDisableTable[] =
{

   {0xF82F4   ,0  , 0x100          },
   {0xF8304   ,0  , 0x100          },
   {0xF830c   ,0  , 0x100          },
   {0xF8314   ,0  , 0x100          },
   {0xF831c   ,0  , 0x100          },
   {0xF8324   ,0  , 0x100          }
};

/* pll configuration enable*/ 
static RegData pllEbaleTable[] =
{

   {0xF82F4   ,0x100  , 0x100          },
   {0xF8304   ,0x100  , 0x100          },
   {0xF830c   ,0x100  , 0x100          },
   {0xF8314   ,0x100  , 0x100          },
   {0xF831c   ,0x100  , 0x100          },
   {0xF8324   ,0x100  , 0x100          }
};


const GT_U32 DQbitMap2Phypin[] = 
{
/* bit 0 bit 1 ...*/
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3,
    2, 7, 3, 0, 1, 9, 6, 8,
    6, 7, 0, 9, 3, 8, 2, 1,
    0, 6, 8, 3, 9, 2, 7, 1,
    9, 1, 7, 0, 8, 2, 6, 3
};

/******************************************************************************
* Get pointer to global pointer array using the fiven frequency
*/
static
GT_STATUS ddr3TipGetFreqTablePtr(MV_HWS_DDR_FREQ freq, GT_U32** pFreqTable)
{
   DEBUG_TRAINING_IP(DEBUG_LEVEL_TRACE, ("ddr3TipGetFreqTablePtr freq  0x%x \n", freq));

   *pFreqTable = freqParams[freq];
   return GT_OK;
}


GT_STATUS ddr3TipNp5GetFreqConfig
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 freq,
	MV_HWS_TIP_FREQ_CONFIG_INFO		*freqConfigInfo
)
{
    if (np5BwPerFreq[freq] == 0xff)
    {
		freqConfigInfo->isSupported = GT_FALSE;
        return GT_NOT_SUPPORTED;
    }

    if (freqConfigInfo == NULL)
    {
        return GT_BAD_PARAM;
    }
    freqConfigInfo->bwPerFreq = np5BwPerFreq[freq];
    freqConfigInfo->ratePerFreq = np5RatePerFreq[freq];
    freqConfigInfo->isSupported = GT_TRUE;

    return GT_OK;
}

/******************************************************************************
*/
GT_STATUS    ddr3TipNp5PipeEnable
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
    CHECK_STATUS(ddr3TipRegRead(devNum, PIPE_ENABLE_ADDR, &dataValue, MASK_ALL_BITS))
    dataValue = (dataValue & (~0xFF)) | pipeEnableMask;
    CHECK_STATUS(ddr3TipRegWrite(devNum, PIPE_ENABLE_ADDR, dataValue));

    return GT_OK;
}

GT_STATUS    ddr3TipNp5IFWrite
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                dataValue,
    GT_U32                mask 
)
{

    GT_U32 dataCmd = 0, uiDataRead = 0, uiReadInterfaceId=0, startIf, endIf;
    MV_HWS_TOPOLOGY_MAP *topologyMap = ddr3TipGetTopologyMap(devNum);
    GT_U32 rdData = 1;
    GT_U32 dataRead[MAX_INTERFACE_NUM];

#ifdef WIN32
    return 0;
#endif
    if (mask != MASK_ALL_BITS)
    {
        /* if all bits should be written there is no need for read-modify-write */
        if (interfaceAccess == ACCESS_TYPE_MULTICAST)
        { 
            CHECK_STATUS(ddr3TipGetFirstActiveIf(devNum, topologyMap->interfaceActiveMask, &uiReadInterfaceId));
        }
        else
        {
            uiReadInterfaceId = interfaceId;
        }
        if (debugNp5 >= 2)
        {
            DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO,( "ddr3TipNp5IFWrite active interface = %d\n",uiReadInterfaceId));
        }
        CHECK_STATUS(ddr3TipNp5IFRead(devNum, ACCESS_TYPE_UNICAST,uiReadInterfaceId, regAddr,  dataRead,MASK_ALL_BITS));
        uiDataRead = dataRead[uiReadInterfaceId];
        /*ddr3TipNp5PipeEnable(devNum, interfaceAccess, interfaceId, GT_TRUE);*/
        dataValue = (uiDataRead & (~mask)) | (dataValue & mask);
    }
    /*ddr3TipNp5PipeEnable(devNum, interfaceAccess, interfaceId, GT_TRUE);*/
    /*CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_CTRL_1_REG) , TRAINING_ID));*/

    /*working with XSB multicast client , Write Interface ADDR as data to XSB address C */
    CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_ADDRESS_REG), (regAddr)));
    /*Write Interface DATA as data to XSB address 0x10.*/
    if (debugNp5 >= 1)
    {
        DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("Dunit Write: interface %d  access %d Address 0x%x Data 0x%x\n", interfaceId, interfaceAccess, regAddr, dataValue));
    }
   
	CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_DATA_REG),  dataValue));

	
    dataCmd = (TARGET_INT << 19) + (BYTE_EN << 11) + (NUM_BYTES << 4) +  (CMD_WRITE << 2) + (INTERNAL_ACCESS_PORT << 1);
    /*Write Interface COMMAND as data to XSB address 8
      Execute an internal write to xBar port1 */
    CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_CMD_REG), dataCmd));
    dataCmd |= EXECUTING;
    CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_CMD_REG), dataCmd));
    /*CHECK_STATUS(ddr3TipRegRead(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_CMD_REG), &rdData, MASK_ALL_BITS));*/

    if (interfaceAccess == ACCESS_TYPE_MULTICAST)
    {
        startIf = 0;
        endIf = MAX_INTERFACE_NUM - 1;
    }  
    else
    {
        startIf = interfaceId;
        endIf = interfaceId;
    }

    for(interfaceId = startIf; interfaceId <= endIf; interfaceId++)    
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        CHECK_STATUS(ddr3TipNp5IsAccessDone(devNum, interfaceId));
    }
    /*ddr3TipNp5PipeEnable(devNum, interfaceAccess, interfaceId, GT_FALSE);*/
    return GT_OK;
}

GT_STATUS ddr3TipNp5IsAccessDone
(
    GT_U32 devNum,
    GT_U32 interfaceId
)
{
    GT_U32 rdData = 1;
    int cnt = 0;


    CHECK_STATUS(ddr3TipRegRead(devNum, XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_CMD_REG), &rdData, MASK_ALL_BITS));
    rdData &= 1;

    while(rdData != 0)
    {
        if (cnt++ >= MAX_POLLING_ITERATIONS)
            break;
        CHECK_STATUS(ddr3TipRegRead(devNum, XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_CMD_REG), &rdData, MASK_ALL_BITS));
        rdData &= 1;
    }
    if (cnt < MAX_POLLING_ITERATIONS)
    {
        return GT_OK;
    }
    else
    {
        return GT_FAIL;
    }
}

/******************************************************************************/
GT_STATUS    ddr3TipNp5IFRead
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                *data,
    GT_U32                mask 
)
{
    GT_U32 dataValue = 0, startIf, endIf;
    MV_HWS_TOPOLOGY_MAP *topologyMap = ddr3TipGetTopologyMap(devNum);
#ifdef WIN32
    return 0;
#endif
    /*ddr3TipNp5PipeEnable(devNum, interfaceAccess, interfaceId, GT_TRUE);*/
    /*CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_CTRL_1_REG) ,TRAINING_ID));*/
    /*working with XSB client InterfaceNum  Write Interface ADDR as data to XSB address C*/
    CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_ADDRESS_REG), regAddr));
    dataValue = (TARGET_INT << 19) + (BYTE_EN << 11) + (NUM_BYTES << 4) + (CMD_READ << 2) +(INTERNAL_ACCESS_PORT  << 1);
    /*Write Interface COMMAND as data to XSB address 8 */
    CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_CMD_REG), dataValue));
    dataValue |= EXECUTING;
    CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_CMD_REG), dataValue));

    if (interfaceAccess == ACCESS_TYPE_UNICAST)
    {
        startIf = interfaceId;
        endIf = interfaceId;
    }
    else
    {
        startIf = 0;
        endIf = MAX_INTERFACE_NUM-1;
    }
    
    for(interfaceId = startIf; interfaceId <= endIf; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        CHECK_STATUS(ddr3TipNp5IsAccessDone(devNum, interfaceId));
#if OLD_UNICAST
        if (interfaceAccess == ACCESS_TYPE_UNICAST)
        {
            CHECK_STATUS(ddr3TipRegRead(devNum, XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_DATA_REG), data, mask));
            if (debugNp5 >= 2)
            {
			    DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("DunitRead   0x%x: 0x%x (mask 0x%x)\n",  regAddr,  *data, mask));
            }
        }
        else
#endif
        {
            CHECK_STATUS(ddr3TipRegRead(devNum, XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_DATA_REG), &data[interfaceId], mask));
            if (debugNp5 >= 2)
            {
			    DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("DunitRead   0x%x: 0x%x (mask 0x%x)\n",  regAddr,  data[interfaceId], mask));
            }
        }
    }
    /*ddr3TipNp5PipeEnable(devNum, interfaceAccess, interfaceId, GT_FALSE);*/
    return GT_OK;
}



static GT_STATUS ddr3TipNp5SetFreq
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 frequency
)
{
    CHECK_STATUS(ddr3TipNp5SetPll(devNum, frequency));
    return GT_OK;
}

#if 0
static GT_STATUS ddr3TipNp5SetDivider
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 frequency
)
{
    GT_U32 dataValue = 0, sarFunc, dataRead, ddrCnt;

    if (frequency != DDR_FREQ_LOW_FREQ)
    {
		DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("set divider:   frequency invalid %d\n",  frequency));
        return GT_BAD_PARAM;
    }

    ddr3TipRegRead(devNum, 0xf8204, &dataRead, MASK_ALL_BITS);
    DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("set divider:  sar reg 0x%x\n",dataRead));
    dataRead = (dataRead >> 15) & 0x7;
    switch (dataRead)
    {
    case 0:
        sarFunc = 0x204;
        DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("set divider:  sarFreq 1066M\n"));
        break;
    case 1:
        sarFunc = 0x183;
        DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("set divider:  sarFreq 933M\n"));
        break;
    case 2:
        sarFunc = 0x183;
        DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("set divider:  sarFreq 800M\n"));
        break;
    default:
        DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("set divider:  SAR freq invalid %d\n",  dataRead));
        return GT_BAD_PARAM;
    }
  
    for(ddrCnt=0; ddrCnt < (NP5_NUMBER_OF_INTERFACES/2) ; ddrCnt++)
    {
        CHECK_STATUS(ddr3TipRegRead(devNum, dividerArr[ddrCnt].regAddr, &dataRead, MASK_ALL_BITS));
        dataRead &= ~(dividerArr[ddrCnt].maskClkDivReloadRatio | dividerArr[ddrCnt].maskSdrClkDivRatioFull | dividerArr[ddrCnt].maskDdrClkDivRatioFull);
        dataValue = (1 << dividerArr[ddrCnt].bitClkDivReloadRatio);
        dataValue |= dataRead;
        DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("set divider: write(0x%x,0x%x)\n",  dividerArr[ddrCnt].regAddr, dataValue));
        CHECK_STATUS(ddr3TipRegWrite(devNum, dividerArr[ddrCnt].regAddr, dataValue));
        dataValue |= ((sarFunc & 0x3f) << dividerArr[ddrCnt].bitSdrClkDivRatioFull) ;
        DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("set divider: write(0x%x,0x%x)\n",  dividerArr[ddrCnt].regAddr, dataValue));
        CHECK_STATUS (ddr3TipRegWrite(devNum, dividerArr[ddrCnt].regAddr, dataValue));
        dataValue |= (((sarFunc & (0x3f << 6)) >> 6) << dividerArr[ddrCnt].bitDdrClkDivRatioFull) ;
        DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("set divider: write(0x%x,0x%x)\n",  dividerArr[ddrCnt].regAddr, dataValue));
        CHECK_STATUS(ddr3TipRegWrite(devNum, dividerArr[ddrCnt].regAddr, dataValue));

    }
  
    if (hwsOsExactDelayPtr == NULL)
    {
        DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("hwsOsExactDelayPtr is NULL !!!"));
    }
    else
    {
        CHECK_STATUS(hwsOsExactDelayPtr(devNum, devNum, 300));
    }
    

    for(ddrCnt=0; ddrCnt < (NP5_NUMBER_OF_INTERFACES/2) ; ddrCnt++)
    {
        CHECK_STATUS(ddr3TipRegRead(devNum, dividerArr[ddrCnt].regAddr, &dataRead, MASK_ALL_BITS));
        dataRead &= ~(dividerArr[ddrCnt].maskClkDivReloadRatio );
        dataValue = (0 << dividerArr[ddrCnt].bitClkDivReloadRatio);
        dataValue |= dataRead;
        DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("set divider: write(0x%x,0x%x)\n",  dividerArr[ddrCnt].regAddr, dataValue));

        CHECK_STATUS (ddr3TipRegWrite(devNum, dividerArr[ddrCnt].regAddr, dataValue));
       CHECK_STATUS(hwsOsExactDelayPtr(devNum, devNum, 10));

    } 

    return GT_OK;
}
#endif

static GT_STATUS ddr3TipNp5SetPll
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 frequency
)
{
    GT_U32  dataVal = 0, dataValue = 0;
    GT_U32  *pPllFreqTable = NULL;
    GT_U32  ddrCnt = 0;
    RegData *pRegData = NULL;

  
    pRegData = pllConfig;

    CHECK_STATUS(ddr3TipGetFreqTablePtr(frequency, &pPllFreqTable)); 
    for(ddrCnt=0; ddrCnt < (NP5_NUMBER_OF_INTERFACES/2) ; ddrCnt++)
    {   
        /* write 0 to reload ratio & clock enable */

        ddr3TipRegRead(devNum, pRegData[ddrCnt*2].regAddr, &dataVal, MASK_ALL_BITS);
        dataValue = (dataVal & (~pRegData[ddrCnt*2].regMask)) ;
        CHECK_STATUS (ddr3TipRegWrite(devNum, pRegData[ddrCnt*2].regAddr, dataValue));
        ddr3TipRegRead(devNum, pRegData[ddrCnt*2 + 1].regAddr, &dataVal, MASK_ALL_BITS);
        dataValue = (dataVal & (~pRegData[ddrCnt*2 + 1].regMask)) ;
        CHECK_STATUS (ddr3TipRegWrite(devNum, pRegData[ddrCnt*2 + 1].regAddr, dataValue));
    }
    dataValue = (0x1 << 29) |  (pPllFreqTable[FreqField_PLL_LPF_R1_CTRL] << 25) |(pPllFreqTable[FreqField_PLL_VCO_BAND] << 21) | (pPllFreqTable[FreqField_PLL_NDIV] << 12) | (pPllFreqTable[FreqField_PLL_MDIV] << 3) |  (pPllFreqTable[FreqField_PLL_KDIV]);
    /*DEBUG_TRAINING_IP(("ddr3TipGetFreqTablePtr LPF  0x%x VCO  0x%x NDIV  0x%x MDIV  0x%x KDIV  0x%x\n", 
					    pPllFreqTable[FreqField_PLL_LPF_R1_CTRL], pPllFreqTable[FreqField_PLL_VCO_BAND], pPllFreqTable[FreqField_PLL_NDIV], 
					    pPllFreqTable[FreqField_PLL_MDIV], pPllFreqTable[FreqField_PLL_KDIV]));   */

    /* pll configuration disable */
    for(ddrCnt=0; ddrCnt < (NP5_NUMBER_OF_INTERFACES/2) ; ddrCnt++)
    {         
        CHECK_STATUS (ddr3TipRegWrite(devNum,pllDisableTable[ddrCnt].regAddr, pllDisableTable[ddrCnt].regData));
    }
    /* update pll configuration */
    for(ddrCnt=0; ddrCnt < (NP5_NUMBER_OF_INTERFACES/2) ; ddrCnt++)
    {         
        CHECK_STATUS (ddr3TipRegWrite(devNum,pllValueTable[ddrCnt].regAddr, dataValue));
    }
    /* pll configuration enable */
    for(ddrCnt=0; ddrCnt < (NP5_NUMBER_OF_INTERFACES/2) ; ddrCnt++)
    {         
        CHECK_STATUS (ddr3TipRegWrite(devNum,pllEbaleTable[ddrCnt].regAddr, pllEbaleTable[ddrCnt].regData));
    }
#ifndef WIN32
    if (hwsOsExactDelayPtr == NULL)
    {
        DEBUG_TRAINING_IP(DEBUG_LEVEL_TRACE, ("hwsOsExactDelayPtr is NULL !!!"));
    }
    else
    {
        CHECK_STATUS(hwsOsExactDelayPtr(devNum, devNum, freqDelay));
    }
#endif
	/*
    for(ddrCnt=0; ddrCnt < (NP5_NUMBER_OF_INTERFACES/2) ; ddrCnt++)
    {         
        CHECK_STATUS (ddr3TipRegWrite(devNum,pllEbaleTable[ddrCnt].regAddr, 0));
    }
    */
    for(ddrCnt=0; ddrCnt < (NP5_NUMBER_OF_INTERFACES/2) ; ddrCnt++)
    {  
        /* write 1 (enable) to reload ratio & clock enable */

        ddr3TipRegRead(devNum, pRegData[ddrCnt*2 + 1].regAddr, &dataVal, MASK_ALL_BITS);
        dataValue = (dataVal & (~pRegData[ddrCnt*2 + 1].regMask)) | (pRegData[ddrCnt*2 + 1].regMask);
        CHECK_STATUS (ddr3TipRegWrite(devNum, pRegData[ddrCnt*2 + 1].regAddr, dataValue));

        ddr3TipRegRead(devNum, pRegData[ddrCnt*2].regAddr, &dataVal, MASK_ALL_BITS);
        dataValue = (dataVal & (~pRegData[ddrCnt*2].regMask)) | (pRegData[ddrCnt*2].regMask);
        CHECK_STATUS (ddr3TipRegWrite(devNum, pRegData[ddrCnt*2].regAddr, dataValue)); 
    }

    return GT_OK;

}

/******************************************************************************
* Name:     ddr3TipNp5SelectDdrController.
* Desc:     Enable/Disable access to Marvell's server.
* Args:     devNum  - device number
*           enable  - whether to enable or disable the server
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipNp5SelectDdrController
(
    GT_U8    devNum,
    GT_BOOL  enable 
)
{
    GT_U32 interfaceId;
    MV_HWS_TOPOLOGY_MAP *topologyMap = ddr3TipGetTopologyMap(devNum);

    DEBUG_TRAINING_IP(DEBUG_LEVEL_TRACE, ("ddr3TipNp5SelectDdrController"));
    if (enable)
    {
        for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
        {
            VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
            pipeMulticastMask |= (1 << interfaceMap[interfaceId].pipe);
            /* multicast Macro is subscribed to Multicast 0x1D */
           /* CHECK_STATUS(ddr3TipRegRead(devNum, XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_CTRL_0_REG), &dataValue, MASK_ALL_BITS));
            dataValue |= 0x40;
            CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_CTRL_0_REG), dataValue));
            */
        }
        /* Enable Pipe 7. */
        CHECK_STATUS(ddr3TipRegWrite(devNum, PIPE_ENABLE_ADDR, (1<<7) ));
        /* Enable DDR Tuning */
        CHECK_STATUS(ddr3TipRegWrite(devNum, ENABLE_DDR_TUNING_ADDR, ENABLE_DDR_TUNING_DATA));
        CHECK_STATUS(ddr3TipNp5IFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, READ_BUFFER_SELECT_REG,  0x8000, MASK_ALL_BITS));
    }
    else
    {
        /* Disable DDR Tuning Select */
        CHECK_STATUS(ddr3TipRegWrite( devNum,  ENABLE_DDR_TUNING_ADDR, DISABLE_DDR_TUNING_DATA));
        CHECK_STATUS(ddr3TipNp5IFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, READ_BUFFER_SELECT_REG, 0,MASK_ALL_BITS));
   }
   return GT_OK;
}


/******************************************************************************
*/
GT_STATUS ddr3TipInitNp5Silicon
(
    GT_U32  devNum,
    GT_U32  boardId
) 
{
    MV_HWS_TIP_STATIC_CONFIG_INFO staticConfig;
    MV_HWS_TIP_CONFIG_FUNC_DB configFunc;
    MV_HWS_XSB_INFO         xsbInfo;
	GT_U8 numOfBusPerInterface = 4;

    GT_U32 boardOffset = boardId * NP5_NUMBER_OF_INTERFACES *numOfBusPerInterface;

	DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("register functions \n"));

    staticConfig.siliconDelay = np5SiliconDelayOffset[boardId];
    staticConfig.packageTraceArr = np5PackageRoundTripDelayArray;
    staticConfig.boardTraceArr = &np5BoardRoundTripDelayArray[boardOffset];


    configFunc.tipDunitReadFunc = ddr3TipNp5IFRead;
    configFunc.tipDunitWriteFunc = ddr3TipNp5IFWrite;
    configFunc.tipDunitMuxSelectFunc = ddr3TipNp5SelectDdrController;
    configFunc.tipGetFreqConfigInfoFunc = ddr3TipNp5GetFreqConfig;
    configFunc.tipSetFreqDividerFunc = ddr3TipNp5SetFreq;

    mvHwsDdr3TipInitConfigFunc(devNum, &configFunc);

	/*Set device attributes*/
	ddr3TipDevAttrInit(devNum);
	ddr3TipDevAttrSet(devNum, MV_ATTR_TIP_REV, MV_TIP_REV_1);
	ddr3TipDevAttrSet(devNum, MV_ATTR_PHY_EDGE, MV_DDR_PHY_EDGE_NEGATIVE);
	ddr3TipDevAttrSet(devNum, MV_ATTR_OCTET_PER_INTERFACE, numOfBusPerInterface);
	ddr3TipDevAttrSet(devNum, MV_ATTR_INTERLEAVE_WA, GT_FALSE);

#ifdef STATIC_ALGO_SUPPORT
    ddr3TipInitStaticConfigDb(devNum, &staticConfig);
#endif
    ddr3TipInitSpecificRegConfig(devNum, np5SpecificInitControllerConfig);
	ddr3TipRegisterDqTable(devNum, DQbitMap2Phypin);

    xsbInfo.dfxTable = interfaceMap;
    ddr3TipRegisterXsbInfo(devNum, &xsbInfo);

    /* old machine read leveling version */
    maskTuneFunc = ( STATIC_LEVELING_MASK_BIT |
                     /*WRITE_LEVELING_MASK_BIT |*/
                     LOAD_PATTERN_2_MASK_BIT |
                     READ_LEVELING_MASK_BIT |
                     SET_TARGET_FREQ_MASK_BIT |
                     WRITE_LEVELING_TF_MASK_BIT | 
                     READ_LEVELING_TF_MASK_BIT |
                     WRITE_LEVELING_SUPP_TF_MASK_BIT |
                     PBS_TX_MASK_BIT |
                     PBS_RX_MASK_BIT | 
                     CENTRALIZATION_RX_MASK_BIT |
                     CENTRALIZATION_TX_MASK_BIT); 

    lowFreq = DDR_FREQ_LOW_FREQ;
    mediumFreq = DDR_FREQ_667;
    isPllBeforeInit = 1;
    initFreq = DDR_FREQ_667;
	debugTraining = DEBUG_LEVEL_INFO;
	odtAdditional = 0;
	odtConfig = 0;
	delayEnable = 0;
	if( ckDelay == -1 )
		ckDelay = 150;
	caDelay = 0;
	calibrationUpdateControl = 1;
	freqVal[DDR_FREQ_LOW_FREQ] = dfsLowFreq = 100;

    return GT_OK;
}


GT_STATUS ddr3TipInitNp5
(
    GT_U32  devNum,
    GT_U32  boardId
) 
{
    DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("ddr3TipInitNp5 \n"));
    CHECK_STATUS(mvHwsDdr3TipLoadTopologyMap(devNum, &np5TopologyMap[boardId]));
    ddr3TipInitNp5Silicon(devNum, boardId);
    return GT_OK;
}






/******************************************************************************
* Name:     ddr3TipPipeEnable.
* Desc:     
* Args:     
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipPipeEnable
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
    CHECK_STATUS(ddr3TipRegRead(devNum, PIPE_ENABLE_ADDR, &dataValue, MASK_ALL_BITS))
    dataValue = (dataValue & (~0xFF)) | pipeEnableMask;
    CHECK_STATUS(ddr3TipRegWrite(devNum, PIPE_ENABLE_ADDR, dataValue));

    return GT_OK;
}


/******************************************************************************
*/
static GT_STATUS    ddr3TipDataReset
(
    GT_U32                  devNum,
    MV_HWS_ACCESS_TYPE      interfaceAccess,
    GT_U32                  interfaceId
)
{
    GT_STATUS retVal;
    GT_U32  uiWordCnt;

    for(uiWordCnt = 0; uiWordCnt < 8 ; uiWordCnt++)
    {
        /*Write Interface DATA as data to XSB address 0x10.*/
        retVal = ddr3TipRegWrite(devNum,     XSB_MAPPING(interfaceId, interfaceAccess, XSB_DATA_REG+(uiWordCnt * 4)), 0xABCDEF12);
        if (retVal != GT_OK)
        {
            return retVal;
        }
    }
    return GT_OK;
}

/*****************************************************************************
XSB External read
******************************************************************************/
GT_STATUS    ddr3TipExtRead
( 
    GT_U32      devNum,
    GT_U32      interfaceId,
    GT_U32      regAddr,
    GT_U32      numOfBursts,
    GT_U32      *addr
)

{
    GT_U32 burstNum, wordNum , dataValue;
	GT_U32 cntPoll;
    MV_HWS_ACCESS_TYPE    interfaceAccess = ACCESS_TYPE_UNICAST;

    /*DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("=== EXTERNAL READ ==="));*/

  /*  if (xsbInfo[devNum].isPipeEnableRequired ==  GT_TRUE)
    {
        ddr3TipPipeEnable(devNum, ACCESS_TYPE_UNICAST, interfaceId, GT_TRUE);
    }*/
    /* ofer ????
    CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, XSB_CTRL_0_REG) ,EXT_TRAINING_ID));*/

    for(burstNum=0 ; burstNum < numOfBursts; burstNum++)
    {
        ddr3TipDataReset(devNum, ACCESS_TYPE_UNICAST, interfaceId);
        /*working with XSB client InterfaceNum  Write Interface ADDR as data to XSB address C*/
        CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_ADDRESS_REG + (burstNum * 32)), regAddr));
/*        if (xsbInfo[devNum].isCbeRequired ==  GT_TRUE)
        {
            dataValue = CS_CBE_VALUE(0) << 19;
        }
        else*/
        {
            dataValue = (TARGET_EXT << 19);
        }
        dataValue |= (BYTE_EN << 11) + (NUM_BYTES_IN_BURST << 4) + (ACCESS_EXT << 3);
        /*Write Interface COMMAND as data to XSB address 8 */
        CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING( interfaceId, interfaceAccess, XSB_CMD_REG),   dataValue));
        dataValue |= EXECUTING;
        CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING( interfaceId, interfaceAccess, XSB_CMD_REG),   dataValue));

		for(cntPoll=0; cntPoll < MAX_POLLING_ITERATIONS; cntPoll++)
		{
			CHECK_STATUS(ddr3TipRegRead(devNum, XSB_MAPPING( interfaceId, interfaceAccess, XSB_CMD_REG), &dataValue, 0x1));
			
			if (dataValue == 0)
				break;
		}

		if (cntPoll >= MAX_POLLING_ITERATIONS)
		{
			DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("No Done indication for DDR Read\n", dataValue));
			return GT_NOT_READY;
		}

        for(wordNum = 0; wordNum < EXT_ACCESS_BURST_LENGTH /*s_uiNumOfBytesInBurst/4*/; wordNum++)
        {
            CHECK_STATUS(ddr3TipRegRead(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_DATA_REG + (wordNum * 4)), &addr[wordNum], MASK_ALL_BITS));
            DEBUG_TRAINING_IP(DEBUG_LEVEL_TRACE, ("ddr3TipExtRead data 0x%x \n",addr[wordNum]));
        }
    }
    /*
    if (xsbInfo[devNum].isPipeEnableRequired ==  GT_TRUE)    {
        ddr3TipPipeEnable(devNum, ACCESS_TYPE_UNICAST, interfaceId, GT_FALSE);
    }*/
    return GT_OK;
}



/*****************************************************************************
XSB External write
******************************************************************************/
GT_STATUS    ddr3TipExtWrite
(
    GT_U32      devNum,
    GT_U32      interfaceId,
    GT_U32      regAddr,
    GT_U32      numOfBursts,
    GT_U32      *addr
)
{
    GT_U32        wordNum = 0,  dataCmd = 0, burstNum=0, cntPoll = 0, dataValue  = 0;
    MV_HWS_ACCESS_TYPE    interfaceAccess = ACCESS_TYPE_UNICAST;    
    /*DEBUG_TRAINING_IP(DEBUG_LEVEL_INFO, ("===EXTERNAL WRITE==="));*/

   /* if (xsbInfo[devNum].isPipeEnableRequired ==  GT_TRUE)
    {
        ddr3TipPipeEnable(devNum, ACCESS_TYPE_UNICAST, interfaceId, GT_TRUE);
    }*/

    CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_CTRL_0_REG) ,EXT_TRAINING_ID  ));
    for(burstNum=0 ; burstNum < numOfBursts; burstNum++)
    {
        /*working with XSB multicast client , Write Interface ADDR as data to XSB address C */
        CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_ADDRESS_REG), regAddr + (burstNum * EXT_ACCESS_BURST_LENGTH * 4)));
        for(wordNum = 0; wordNum < 8 ; wordNum++)
        {
            /*Write Interface DATA as data to XSB address 0x10.*/
            CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_DATA_REG + (wordNum * 4)), addr[wordNum]));
        }
        /*if (xsbInfo[devNum].isCbeRequired ==  GT_TRUE)
        {
            dataCmd =  CS_CBE_VALUE(0) << 19;
        }
        else
        */
        {
            dataCmd = (TARGET_EXT << 19);
        }
        dataCmd |= (BYTE_EN << 11) + (NUM_BYTES_IN_BURST << 4) + (ACCESS_EXT << 3) + EXT_MODE;
        CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId,  interfaceAccess, XSB_CMD_REG), dataCmd));
        /* execute xsb write */
        dataCmd |= 0x1;
        DEBUG_TRAINING_IP(DEBUG_LEVEL_TRACE, ("ddr3TipExtWrite dataCmd 0x%x \n", dataCmd));
        CHECK_STATUS(ddr3TipRegWrite(devNum, XSB_MAPPING(interfaceId, interfaceAccess, XSB_CMD_REG), dataCmd));

		for(cntPoll=0; cntPoll < MAX_POLLING_ITERATIONS; cntPoll++)
		{
			CHECK_STATUS(ddr3TipRegRead(devNum, XSB_MAPPING( interfaceId, interfaceAccess, XSB_CMD_REG), &dataValue, 0x1));
			
			if (dataValue == 0)
				break;
		}

		if (cntPoll >= MAX_POLLING_ITERATIONS)
		{
			DEBUG_TRAINING_IP(DEBUG_LEVEL_ERROR, ("No Done indication for DDR Write External\n", dataValue));
			return GT_NOT_READY;
		}

    }
  /*  if (xsbInfo[devNum].isPipeEnableRequired ==  GT_FALSE)
    {
        ddr3TipPipeEnable(devNum, ACCESS_TYPE_UNICAST, interfaceId, GT_FALSE);
    }*/
    return GT_OK;
}

