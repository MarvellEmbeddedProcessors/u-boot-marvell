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
* mvHwsDdr3Bc2.c
*
* DESCRIPTION: DDR3 Bc2 configuration
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 60 $
******************************************************************************/

#define DEFINE_GLOBALS

#include "mvHwsDdr3Bc2.h"
#include "mvDdr3TrainingIpStatic.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpDef.h"
#include "mvDdr3TrainingIpPrvIf.h"
#include "mvDdr3LoggingDef.h"

/************************** definitions ******************************/
#define BC2_NUM_BYTES                   (3)

#define BC2_CLIENT_FIELD(client)		(client << 15)
#define BC2_GET_CLIENT_FIELD(interfaceAccess,interfaceId) ((interfaceAccess == ACCESS_TYPE_MULTICAST) ? BC2_CLIENT_FIELD(MULTICAST_ID):BC2_CLIENT_FIELD(interfaceId))
#define BC2_XSB_MAPPING(interfaceId, interfaceAccess, regaddr)   (regaddr+ XSB_BASE_ADDR + BC2_GET_CLIENT_FIELD(interfaceAccess,interfaceMap[interfaceId].client))
#define BC2_CLIENT_MAPPING(interfaceId, regaddr) ((regaddr << 2)+ CLIENT_BASE_ADDR + BC2_CLIENT_FIELD(interfaceMap[interfaceId].client))
#define CS_CBE_VALUE(csNum)   (csCbeReg[csNum])

#define TM_PLL_REG_DATA(a,b,c)  ((a << 12) + (b << 8) + (c << 2))
#define BC2_PLL_REG(a,b,c)  ((a << 12) + (b << 8) + (c << 2))
#define R_MOD_W(writeData,readData,mask) ((writeData & mask) | (readData & (~mask)))


/************************** pre-declaration ******************************/

GT_STATUS    ddr3TipBC2ExtRead
(
    GT_U32      devNum,
    GT_U32      interfaceId,
    GT_U32      regAddr,
    GT_U32      numOfBursts,
    GT_U32      *data
);

GT_STATUS    ddr3TipBC2ExtWrite
(
    GT_U32      devNum,
    GT_U32      interfaceId,
    GT_U32      regAddr,
    GT_U32      numOfBursts,
    GT_U32      *addr
);
static GT_STATUS ddr3TipBc2GetMediumFreq
(
    GT_U32          devNum,
	GT_U32			interfaceId,
    MV_HWS_DDR_FREQ *freq
);

GT_STATUS ddr3TipBc2SetDivider
(
	GT_U8							devNum,
	GT_U32                			interfaceId,
    MV_HWS_DDR_FREQ                 freq
);

static GT_STATUS ddr3TipTmSetDivider
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 frequency
);

static GT_STATUS ddr3TipCpuSetDivider
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 frequency
);

GT_STATUS ddr3Bc2UpdateTopologyMap
(
    GT_U32 devNum,
    MV_HWS_TOPOLOGY_MAP* topologyMap
);

GT_STATUS ddr3TipBc2GetInitFreq
(
    GT_U8       	devNum,
	GT_U32			interfaceId,
    MV_HWS_DDR_FREQ* freq
);

static GT_STATUS ddr3TipTmGetInitFreq
(
    GT_STATUS       devNum,
    MV_HWS_DDR_FREQ *freq
);

static GT_STATUS ddr3TipCpuGetInitFreq
(
    GT_STATUS       devNum,
    MV_HWS_DDR_FREQ *freq
);

GT_STATUS    ddr3TipBc2IFRead
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                *data,
    GT_U32                mask
);

GT_STATUS    ddr3TipBc2IFWrite
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                dataValue,
    GT_U32                mask
);

GT_STATUS ddr3TipBC2IsfAccessDone
(
    GT_U32 devNum,
    GT_U32 interfaceId
);

GT_STATUS ddr3TipBc2GetDeviceInfo
(
	GT_U8      devNum,
	MV_DDR3_DEVICE_INFO * infoPtr
);

DDR_IF_ASSIGNMENT ddr3GetSdramAssignment(GT_U8 devNum);

/************************** Globals ******************************/

extern GT_U32 maskTuneFunc;
extern GT_U32 freqVal[];
extern MV_HWS_DDR_FREQ mediumFreq;
extern MV_HWS_TOPOLOGY_MAP *topologyMap;
extern GT_U32 firstActiveIf;
extern MV_HWS_DDR_FREQ initFreq;
extern GT_U32 dfsLowFreq;
extern GT_U32 dfsLowPhy1;
extern GT_U32  PhyReg1Val;
extern GT_U32 isPllBeforeInit;
extern GT_U32 useBroadcast;
extern GT_U32 isCbeRequired;
extern GT_U32 delayEnable, ckDelay,caDelay;
extern GT_U8 debugTrainingAccess;
extern GT_U8 calibrationUpdateControl; /*2 external only, 1 is internal only*/
extern GT_U32 dfsLowFreq;

GT_U32 debugBc2 = 0;
GT_U32  pipeMulticastMask;

static GT_U32 csCbeReg[]=
{
    0xE , 0xD, 0xB, 0x7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static MV_DFX_ACCESS *interfaceMap;

static MV_DFX_ACCESS interfaceMap_A0[] =
{
	{   0,	 17 },
	{   1,	 7	},
	{   1,	 11 },
	{   0,	 3	},
	{   1,	 25 }
};

static MV_DFX_ACCESS interfaceMap_B0[] =
{
	/* Pipe	Client*/
	{   0,	 17 },
	{   1,	 6	},
	{   1,	 10 },
	{   0,	 3	},
	{   1,	 18 }
};


#ifdef STATIC_ALGO_SUPPORT
static RegData bc2SpecificInitControllerConfig[] =
{

	{0x16D8,    0x2,        0x7},
    {0x15EC,    0,          0x2},
    {0x15EC,    0x8,        0xC},
    {0x15EC,    0x20,       0x30},
	{0x15EC,    0x0,        0xff80},
    {0x16D8,    2,          0x3},
    {0x14cc,    0x1,        0x1},
    {0x14cc,    0x4,        0x4},
    {0x14cc,    0x8,        0x18},
    {0x14cc,    0x12000,    0x7DFFE000},
    {0x1400,    0x73014C30, 0xffffffff},
    {0x1404,    0x30000800, 0xffffffff},
    {0x1408,    0x5415BAAA, 0xffffffff},
    {0x140C,    0x38D93FEF, 0xffffffff},
    {0x1410,    0x1C300121, 0xffffffff},
    {0x1414,    0x700,      0xffffffff},
    {0x1424,    0xF3FF,     0xffffffff},
    {0x1428,    0x11A940,   0xffffffff},
    {0x142C,    0x14C5134,  0xffffffff},
    {0x147C,    0xD771,     0xffffffff},
    {0x1494,    0x10000,    0xffffffff},
    {0x149C,    0x1,        0xffffffff},
    {0x14A8,    0x101,      0xffffffff},
    {0x1504,    0x7FFFFFE1, 0xffffffff},
    {0x150C,    0x7FFFFFE5, 0xffffffff},
    {0x1514,    0,          0xffffffff},
    {0x151C,    0,          0xffffffff},
    {0x1538,    0xB,        0xffffffff},
    {0x153C,    0xB,        0xffffffff},
    {0x15D0,    0x670,      0xffffffff},
    {0x15D4,    0x44,       0xffffffff},
    {0x15D8,    0x18,       0xffffffff},
    {0x15DC,    0x0,        0xffffffff},
    {0x15E0,    0x1,        0xffffffff},
    {0x15E4,    0x203C18,   0xffffffff},
	{0,   0, 0}
};
#endif

static GT_U8 bc2BwPerFreq[DDR_FREQ_LIMIT] =
{
    0x3, /*DDR_FREQ_LOW_FREQ*/
    0x4, /*DDR_FREQ_400*/
    0x4, /*DDR_FREQ_533*/
    0x5, /*DDR_FREQ_667*/
    0x5, /*DDR_FREQ_800*/
    0x5, /*DDR_FREQ_933,*/
    0x5, /*DDR_FREQ_1066*/
    0x3, /*DDR_FREQ_311*/
    0x3, /*DDR_FREQ_333*/
    0x4,  /*DDR_FREQ_467*/
    0x5, /*DDR_FREQ_850*/
    0x5,  /*DDR_FREQ_600*/
    0x3,  /*DDR_FREQ_300*/
	0x5, /*DDR_FREQ_900*/
	0x3 /*DDR_FREQ_360*/
};

static GT_U8 bc2RatePerFreq[DDR_FREQ_LIMIT] =
{
   0x1, /*DDR_FREQ_LOW_FREQ,*/
   0x2, /*DDR_FREQ_400,*/
   0x2, /*DDR_FREQ_533,*/
   0x2, /*DDR_FREQ_667,*/
   0x2, /*DDR_FREQ_800,*/
   0x3, /*DDR_FREQ_933,*/
   0x3, /*DDR_FREQ_1066,*/
   0x1, /*DDR_FREQ_311*/
   0x1, /*DDR_FREQ_333*/
   0x2, /*DDR_FREQ_467*/
   0x2, /*DDR_FREQ_850*/
   0x2, /*DDR_FREQ_600*/
   0x1, /*DDR_FREQ_300*/
   0x3,  /*DDR_FREQ_900*/
   0x1  /*DDR_FREQ_360*/
};


/* Bit mapping (for PBS) */
GT_U32 bc2DQbitMap2Phypin[] =
{
	/* Interface 0 */
	8, 1, 0, 7, 9, 2, 3, 6 , /* dq[0:7]   */
	8, 1, 6, 3, 9, 7, 2, 0 , /* dq[8:15]  */
	8, 1, 9, 2, 6, 7, 3, 0 , /* dq[16:23] */
	1, 6, 0, 8, 7, 3, 2, 9 , /* dq[24:31] */
    0, 0, 0, 0, 0, 0, 0, 0 , /* dq[ECC]   */
	/* Interface 1 */
	3, 7, 0, 1, 8, 2, 6, 9 , /* dq[0:7]   */
	1, 6, 3, 9, 7, 8, 0, 2 , /* dq[8:15]  */
	2, 3, 6, 7, 1, 8, 0, 9 , /* dq[16:23] */
	7, 1, 0, 2, 9, 6, 3, 8 , /* dq[24:31] */
    0, 0, 0, 0, 0, 0, 0, 0 , /* dq[ECC]   */
	/* Interface 2 */
	2, 0, 3, 9, 7, 8, 6, 1 , /* dq[0:7]   */
	3, 2, 1, 6, 0, 9, 7, 8 , /* dq[8:15]  */
	9, 6, 3, 2, 1, 7, 0, 8 , /* dq[16:23] */
	3, 1, 7, 2, 0, 8, 9, 6 , /* dq[24:31] */
    0, 0, 0, 0, 0, 0, 0, 0 , /* dq[ECC]   */
	/* Interface 3 */
	6, 2, 8, 9, 0, 1, 7, 3 , /* dq[0:7]   */
	1, 3, 7, 9, 0, 2, 8, 6 , /* dq[8:15]  */
	2, 0, 7, 1, 8, 9, 6, 3 , /* dq[16:23] */
	1, 6, 8, 2, 0, 7, 3, 9 , /* dq[24:31] */
    0, 0, 0, 0, 0, 0, 0, 0 , /* dq[ECC]   */
    /* Interface 4 */
	1, 9, 6, 3, 0, 7, 8, 2 , /* dq[0:7]   */
	3, 2, 8, 1, 9, 6, 7, 0 , /* dq[8:15]  */
	0, 6, 2, 1, 9, 3, 8, 7 , /* dq[16:23] */
	0, 6, 1, 8, 3, 9, 7, 2 , /* dq[24:31] */
    0, 0, 0, 0, 0, 0, 0, 0 , /* dq[ECC]   */
};

#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
MV_HWS_TOPOLOGY_MAP bc2TopologyMap[] =
{
    /* 1st board */
    {
    0x1f, /* active interfaces */
    /*cs_mask, mirror, dqs_swap, ck_swap X PUPs                         speed_bin           memory_width  mem_size  frequency  casL casWL      temperature */
 {  {{{0x1,1,0,0}, {0x1,1,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_1866M, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_667, 0 ,   0 , MV_HWS_TEMP_HIGH} ,
    {{{0x1,1,0,0}, {0x1,1,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_1866M, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_667, 0 ,   0 , MV_HWS_TEMP_HIGH} ,
    {{{0x1,1,0,0}, {0x1,1,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_1866M, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_667, 0 ,   0 , MV_HWS_TEMP_HIGH} ,
    {{{0x1,1,0,0}, {0x1,1,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_1866M, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_667, 0 ,   0 , MV_HWS_TEMP_HIGH} ,
    {{{0x1,1,0,0}, {0x1,1,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_1866M, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_667, 0 ,   0 , MV_HWS_TEMP_HIGH} },
    0xF  /* Buses mask */
    },

    /* 2nd board */
    {
    0x1f, /* active interfaces */
    /*cs_mask, mirror, dqs_swap, ck_swap X PUPs                         speed_bin           memory_width  mem_size  frequency  casL casWL      temperature */
 {  {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_667, 0 ,   0 , MV_HWS_TEMP_HIGH} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_667, 0 ,   0 , MV_HWS_TEMP_HIGH} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_667, 0 ,   0 , MV_HWS_TEMP_HIGH} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_667, 0 ,   0 , MV_HWS_TEMP_HIGH} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_667, 0 ,   0 , MV_HWS_TEMP_HIGH} },
    0xF  /* Buses mask */
    }
};
#endif

static GT_U8    ddr3TipClockMode( GT_U32 frequency );

/************************** Server Access ******************************/

GT_STATUS ddr3TipBc2ServerRegWrite
(
	GT_U32 devNum,
	GT_U32 regAddr,
	GT_U32 dataValue
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    regAddr = regAddr;
    dataValue = dataValue;

	return GT_OK;
#else
	return hwsServerRegSetFuncPtr(devNum, regAddr, dataValue);
#endif
}

GT_STATUS ddr3TipBc2ServerRegRead
(
	GT_U32 devNum,
	GT_U32 regAddr,
	GT_U32 *dataValue,
	GT_U32 regMask
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    regAddr = regAddr;
    regMask = regMask;

	*dataValue = 0;
#else
	hwsServerRegGetFuncPtr(devNum, regAddr, dataValue);


	*dataValue  = *dataValue & regMask;
#endif
	return GT_OK;
}

/**********************************************************************************/

GT_STATUS ddr3TipBc2GetFreqConfig
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 freq,
	MV_HWS_TIP_FREQ_CONFIG_INFO		*freqConfigInfo
)
{
    devNum = devNum; /* avoid warnings */

    if (bc2BwPerFreq[freq] == 0xff)
    {
        return GT_NOT_SUPPORTED;
    }

    if (freqConfigInfo == NULL)
    {
        return GT_BAD_PARAM;
    }

    freqConfigInfo->bwPerFreq = bc2BwPerFreq[freq];
    freqConfigInfo->ratePerFreq = bc2RatePerFreq[freq];

    freqConfigInfo->isSupported = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************/
GT_STATUS  ddr3TipBC2RevGet(GT_U8 devNum,GT_U32 *version )
{
	GT_U32 data = 0; /* initialized for simulation */

	CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, BC2_JTAG_DEV_ID_STATUS_REG_ADDR,  &data, MASK_ALL_BITS));
	*version = ((data >> BC2_JTAG_DEV_ID_STATUS_VERSION_OFFSET) & 0xF) ;

	return GT_OK;
}

/**********************************************************************************/

static GT_STATUS  ddr3TipBc2GetInterfaceMap( GT_U8 devNum )
{

	GT_U32 version;

	CHECK_STATUS(ddr3TipBC2RevGet(devNum, &version));
	if (version == 0 )    /* BC2 A0 ID = 0*/
		interfaceMap = &interfaceMap_A0[0];
	else
		interfaceMap = &interfaceMap_B0[0];

	return GT_OK;
}

/*****************************************************************************
Enable Pipe
******************************************************************************/
static GT_STATUS    ddr3TipPipeEnable
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
    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, PIPE_ENABLE_ADDR, &dataValue, MASK_ALL_BITS));
    dataValue = (dataValue & (~0xFF)) | pipeEnableMask;
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, PIPE_ENABLE_ADDR, dataValue));

    return GT_OK;
}

static void ddr3TipIsUnicastAccess( GT_U8 devNum,GT_U32* interfaceId, MV_HWS_ACCESS_TYPE* interfaceAccess)
{
	GT_U32 indexCnt, totalCnt = 0 , interfaceTmp = 0;
    MV_HWS_TOPOLOGY_MAP *topologyMap = ddr3TipGetTopologyMap(devNum);

	if (*interfaceAccess == ACCESS_TYPE_MULTICAST)
	{
		for(indexCnt = 0; indexCnt < MAX_INTERFACE_NUM; indexCnt++)
		{
			if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, indexCnt) !=  0)
			{
				totalCnt++;
				interfaceTmp = indexCnt;
				continue;
			}
		}
		if (totalCnt == 1) {
			*interfaceAccess = ACCESS_TYPE_UNICAST;
			*interfaceId = interfaceTmp;
		}
	}
}

/******************************************************************************
* Name:     ddr3TipBc2SelectDdrController.
* Desc:     Enable/Disable access to Marvell's server.
* Args:     devNum     - device number
*           enable        - whether to enable or disable the server
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipBc2SelectDdrController
(
    GT_U8    devNum,
    GT_BOOL  enable
)
{
    GT_U32 interfaceId = 0, dataValue = 0;
	for(interfaceId=0; interfaceId <BC2_NUMBER_OF_INTERFACES ;interfaceId++)
		{
			ddr3TipPipeEnable(devNum, ACCESS_TYPE_UNICAST, interfaceId, GT_TRUE);
			if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, interfaceId) ==  0)
			{
				 /*disable in-active interfaces (so they won't be accessed upon multicast)*/
				CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, BC2_CLIENT_MAPPING(interfaceId, CLIENT_CTRL_REG), &dataValue, MASK_ALL_BITS));
				dataValue &= ~0x40;
				CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_CLIENT_MAPPING(interfaceId, CLIENT_CTRL_REG), dataValue));
				continue;
			}
				/* Enable relevant Pipe */
			pipeMulticastMask |= (1 << interfaceMap[interfaceId].pipe);
			/* multicast Macro is subscribed to Multicast 0x1D */
			CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, BC2_CLIENT_MAPPING(interfaceId, CLIENT_CTRL_REG), &dataValue, MASK_ALL_BITS));
			dataValue |= 0x40;
			CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_CLIENT_MAPPING(interfaceId, CLIENT_CTRL_REG), dataValue));

			ddr3TipPipeEnable(devNum, ACCESS_TYPE_UNICAST, interfaceId, GT_FALSE);

		}
		/* enable access to relevant pipes (according to the pipe mask) */
		ddr3TipPipeEnable(devNum, ACCESS_TYPE_MULTICAST, interfaceId, GT_TRUE);

   if (enable)
    {
       /* Enable DDR Tuning */
        CHECK_STATUS(ddr3TipBc2IFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, READ_BUFFER_SELECT_REG,  0x8000, MASK_ALL_BITS));
        /* configure duplicate CS */
		CHECK_STATUS(ddr3TipBc2IFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, CS_ENABLE_REG,  0x4A/*Haim 0x42*/, MASK_ALL_BITS));
    }
    else
    {
        /* Disable DDR Tuning Select */
        CHECK_STATUS(ddr3TipBc2IFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, CS_ENABLE_REG,  0x2, MASK_ALL_BITS));
        CHECK_STATUS(ddr3TipBc2IFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, READ_BUFFER_SELECT_REG, 0,MASK_ALL_BITS));
   }
   return GT_OK;
}

/*****************************************************************************
Check if Dunit access is done
******************************************************************************/
GT_STATUS ddr3TipBC2IsAccessDone
(
    GT_U32 devNum,
    GT_U32 interfaceId
)
{
    GT_U32 rdData = 1;
    int cnt = 0;

	CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, BC2_XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_CMD_REG), &rdData, MASK_ALL_BITS));
	rdData &= 1;

	while(rdData != 0)
    {
		if (cnt++ >= MAX_POLLING_ITERATIONS)
            break;
        CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, BC2_XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_CMD_REG), &rdData, MASK_ALL_BITS));
        rdData &= 1;
#ifdef ASIC_SIMULATION
        rdData = 0;/* no more iterations needed */
#endif /*ASIC_SIMULATION*/
    }
    if (cnt < MAX_POLLING_ITERATIONS)
    {
		return GT_OK;
    }
    else
    {
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO,("AccessDone fail: if = %d\n",interfaceId));
        return GT_FAIL;
    }
}

/******************************************************************************/
GT_STATUS    ddr3TipBc2IFRead
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

   	ddr3TipIsUnicastAccess(devNum, &interfaceId, &interfaceAccess);

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
        if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, interfaceId) ==  0)
        {
            continue;
        }
        ddr3TipPipeEnable(devNum, ACCESS_TYPE_UNICAST, interfaceId, GT_TRUE);
        CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_CTRL_1_REG) ,TRAINING_ID));

        /*working with XSB client InterfaceNum  Write Interface ADDR as data to XSB address C*/
        CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_ADDRESS_REG), regAddr));
        dataValue = ((GT_U32)(TARGET_INT << 19)) + (BYTE_EN << 11) + (BC2_NUM_BYTES << 4) + (CMD_READ << 2) +(INTERNAL_ACCESS_PORT  << 1) + EXECUTING;
        /*Write Interface COMMAND as data to XSB address 8 */
        CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_CMD_REG), dataValue));

		CHECK_STATUS(ddr3TipBC2IsAccessDone(devNum, interfaceId));

		/* consider that input data is always a vector, and for unicast the writing will be in the interface index in vector */
        CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, BC2_XSB_MAPPING(interfaceId, ACCESS_TYPE_UNICAST, XSB_DATA_REG), &data[interfaceId], mask));
        if (debugBc2 >= 2)
        {
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("DunitRead   0x%x: 0x%x (mask 0x%x)\n",  regAddr,  *data, mask));
        }
        /*ddr3TipPipeEnable(devNum, ACCESS_TYPE_UNICAST, interfaceId, GT_FALSE);*/
    }
    return GT_OK;
}

/******************************************************************************/
GT_STATUS    ddr3TipBc2IFWrite
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                dataValue,
    GT_U32                mask
)
{
    GT_U32 dataCmd = 0, uiDataRead = 0, uiReadInterfaceId=0;
    GT_U32 dataRead[MAX_INTERFACE_NUM];
	GT_U32 startIf, endIf;
    MV_HWS_TOPOLOGY_MAP *topologyMap = ddr3TipGetTopologyMap(devNum);

   	ddr3TipIsUnicastAccess(devNum, &interfaceId, &interfaceAccess);
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
        if (debugBc2 >= 2)
        {
            DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("ddr3TipBc2IFWrite active interface = %d\n",uiReadInterfaceId));
        }
        CHECK_STATUS(ddr3TipBc2IFRead(devNum, ACCESS_TYPE_UNICAST,uiReadInterfaceId, regAddr,  dataRead,MASK_ALL_BITS));
        uiDataRead = dataRead[uiReadInterfaceId];
        dataValue = (uiDataRead & (~mask)) | (dataValue & mask);
    }
    ddr3TipPipeEnable(devNum, interfaceAccess, interfaceId, GT_TRUE);
    /* set the ID */
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, interfaceAccess, XSB_CTRL_1_REG) , TRAINING_ID));

    /*working with XSB multicast client , Write Interface ADDR as data to XSB address C */
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, interfaceAccess, XSB_ADDRESS_REG), regAddr));
    /*Write Interface DATA as data to XSB address 0x10.*/
    if (debugBc2 >= 1)
    {
        DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("Dunit Write: interface %d  access %d Address 0x%x Data 0x%x\n", interfaceId, interfaceAccess, regAddr, dataValue));
    }
	CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, interfaceAccess, XSB_DATA_REG),  dataValue));
    dataCmd = ((GT_U32)(TARGET_INT << 19)) + (BYTE_EN << 11) + (BC2_NUM_BYTES << 4) +  (CMD_WRITE << 2) + (INTERNAL_ACCESS_PORT << 1);
    /*Write Interface COMMAND as data to XSB address 8
      Execute an internal write to xBar port1 */
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, interfaceAccess, XSB_CMD_REG), dataCmd));
	dataCmd |= EXECUTING;
	CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, interfaceAccess, XSB_CMD_REG), dataCmd));

	 /*check that write is completed; the test should be done per interface */
	if (interfaceAccess == ACCESS_TYPE_MULTICAST)
    {
        startIf = 0;
        endIf = MAX_INTERFACE_NUM-1;
    }
    else
    {
        startIf = interfaceId;
        endIf = interfaceId;
    }

    for(interfaceId = startIf; interfaceId <= endIf; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
		ddr3TipPipeEnable(devNum, ACCESS_TYPE_UNICAST, interfaceId, GT_TRUE);
		CHECK_STATUS(ddr3TipBC2IsAccessDone(devNum, interfaceId));
	}
   /* ddr3TipPipeEnable(devNum, interfaceAccess, interfaceId, GT_FALSE);*/
    return GT_OK;
}


/******************************************************************************
* Name:     ddr3TipInitBc2Silicon.
* Desc:     init Training SW DB.
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
static GT_STATUS ddr3TipInitBc2Silicon
(
    GT_U32  devNum,
    GT_U32  boardId
)
{
    MV_HWS_TIP_CONFIG_FUNC_DB   configFunc;
	MV_HWS_TOPOLOGY_MAP*        topologyMap = ddr3TipGetTopologyMap(devNum);
    GT_TUNE_TRAINING_PARAMS     tuneParams;

	if(topologyMap == NULL)
	{
		return GT_NOT_INITIALIZED;
	}

    ddr3TipBc2GetInterfaceMap((GT_U8)devNum);

#ifdef STATIC_ALGO_SUPPORT
	MV_HWS_TIP_STATIC_CONFIG_INFO staticConfig;

    GT_U32 boardOffset = boardId * BC2_NUMBER_OF_INTERFACES * BC2_NUMBER_OF_PUP;

    staticConfig.siliconDelay = bc2SiliconDelayOffset[boardId];
    staticConfig.packageTraceArr = bc2PackageRoundTripDelayArray;
    staticConfig.boardTraceArr = &bc2BoardRoundTripDelayArray[boardOffset];

	ddr3TipInitStaticConfigDb(devNum, &staticConfig);

	ddr3TipInitSpecificRegConfig(devNum, bc2SpecificInitControllerConfig);
#else
    boardId = boardId; /* avoid warnings */
#endif

    configFunc.tipDunitReadFunc = ddr3TipBc2IFRead;
    configFunc.tipDunitWriteFunc = ddr3TipBc2IFWrite;
    configFunc.tipDunitMuxSelectFunc = ddr3TipBc2SelectDdrController;
    configFunc.tipGetFreqConfigInfoFunc = ddr3TipBc2GetFreqConfig;
    configFunc.tipSetFreqDividerFunc = ddr3TipBc2SetDivider;
    configFunc.tipGetDeviceInfoFunc = ddr3TipBc2GetDeviceInfo;
	configFunc.tipGetTemperature = NULL;
	configFunc.tipGetClockRatio = ddr3TipClockMode;

    configFunc.tipExternalRead  = ddr3TipBC2ExtRead;
    configFunc.tipExternalWrite = ddr3TipBC2ExtWrite;

    mvHwsDdr3TipInitConfigFunc(devNum, &configFunc);

	/* register bit mapping (for PBS) */
	ddr3TipRegisterDqTable(devNum, bc2DQbitMap2Phypin);

	/*Set device attributes*/
	ddr3TipDevAttrInit(devNum);
	ddr3TipDevAttrSet(devNum, MV_ATTR_TIP_REV, MV_TIP_REV_2);
	ddr3TipDevAttrSet(devNum, MV_ATTR_PHY_EDGE, MV_DDR_PHY_EDGE_NEGATIVE);
	ddr3TipDevAttrSet(devNum, MV_ATTR_OCTET_PER_INTERFACE, BC2_NUMBER_OF_PUP);
	ddr3TipDevAttrSet(devNum, MV_ATTR_INTERLEAVE_WA, GT_FALSE);

	maskTuneFunc = ( INIT_CONTROLLER_MASK_BIT |
                     SET_MEDIUM_FREQ_MASK_BIT |
                     WRITE_LEVELING_MASK_BIT |
                     LOAD_PATTERN_2_MASK_BIT |
                     READ_LEVELING_MASK_BIT |
                     PBS_RX_MASK_BIT |
                     PBS_TX_MASK_BIT |
                     SET_TARGET_FREQ_MASK_BIT |
                     WRITE_LEVELING_TF_MASK_BIT |
                     READ_LEVELING_TF_MASK_BIT |
                     WRITE_LEVELING_SUPP_TF_MASK_BIT |
                     CENTRALIZATION_RX_MASK_BIT |
                     CENTRALIZATION_TX_MASK_BIT);

	/*Skip mid freq stages for 400Mhz DDR speed*/
	if( (topologyMap->interfaceParams[firstActiveIf].memoryFreq == DDR_FREQ_400) ){
		maskTuneFunc = (INIT_CONTROLLER_MASK_BIT |
		                WRITE_LEVELING_MASK_BIT |
		                LOAD_PATTERN_2_MASK_BIT |
		                READ_LEVELING_MASK_BIT |
		                CENTRALIZATION_RX_MASK_BIT |
		                CENTRALIZATION_TX_MASK_BIT);
	}

    if( ckDelay == MV_PARAMS_UNDEFINED )
        ckDelay = 150;
    delayEnable = 1;
    caDelay = 0;

    /* update DGL parameters */
    tuneParams.ckDelay = ckDelay;
    tuneParams.PhyReg3Val = 0xA;
    tuneParams.gRttNom = 0x44;
    tuneParams.gDic = 0x2;
    tuneParams.uiODTConfig = 0x120012;
    tuneParams.gZpriData = 123;
    tuneParams.gZnriData = 123;
    tuneParams.gZpriCtrl = 74;
    tuneParams.gZnriCtrl = 74;
    tuneParams.gZpodtData = 45;
    tuneParams.gZnodtData = 45;
    tuneParams.gZpodtCtrl = 45;
    tuneParams.gZnodtCtrl = 45;
    tuneParams.gRttWR = 0x200;

    CHECK_STATUS(ddr3TipTuneTrainingParams(devNum, &tuneParams));

    /* frequency and general parameters */
    ddr3TipBc2GetMediumFreq(devNum, firstActiveIf, &mediumFreq);
    initFreq = topologyMap->interfaceParams[firstActiveIf].memoryFreq;
    freqVal[DDR_FREQ_LOW_FREQ] = dfsLowFreq = 100;
    dfsLowPhy1 = PhyReg1Val;
    isPllBeforeInit = 0;
    useBroadcast = GT_FALSE; /* multicast */
    isCbeRequired = GT_TRUE;
	calibrationUpdateControl = 2;

    return GT_OK;
}

/******************************************************************************
* Name:     ddr3Bc2UpdateTopologyMap.
* Desc:
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS ddr3Bc2UpdateTopologyMap(GT_U32 devNum, MV_HWS_TOPOLOGY_MAP* topologyMap)
{
    GT_U32 interfaceId;
    MV_HWS_DDR_FREQ freq;

	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("topologyMap->interfaceActiveMask is 0x%x\n", topologyMap->interfaceActiveMask));

	#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
	/* first check if TM is enabled. reading frequency will check if it's enabled or not
	   in case it's not, the function will exit with error */
	CHECK_STATUS(ddr3TipTmGetInitFreq(devNum, &freq));

	/* if interface 4 is active, check it's assignment and close it if it's for MSYS */
	if(topologyMap->interfaceActiveMask & 0x10)
	{
		if(ddr3GetSdramAssignment((GT_U8)devNum) == MSYS_EN)
		{
			topologyMap->interfaceActiveMask &= ~0x10; /* remove interface 4 from mask */
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR,
								  ("Warning: DDR IF 4 is allocated to MSYS (and was removed from TM IF list)\n"));
		}
	}
	#endif

    for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++) {
    	if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, interfaceId) ==  GT_FALSE)
    	    continue;
		CHECK_STATUS(ddr3TipBc2GetInitFreq((GT_U8)devNum, interfaceId, &freq));
        topologyMap->interfaceParams[interfaceId].memoryFreq = freq;
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("Update topology frequency for interface %d to %d\n",interfaceId, freq));
   	}

	/* re-calc topology parameters according to topology updates (if needed) */
	CHECK_STATUS(mvHwsDdr3TipLoadTopologyMap(devNum, topologyMap));

    return GT_OK;
}

/******************************************************************************
* Name:     ddr3TipInitBc2.
* Desc:     init Training SW DB and updates DDR topology.
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS ddr3TipInitBc2
(
    GT_U32  devNum,
    GT_U32  boardId
)
{
	MV_HWS_TOPOLOGY_MAP* topologyMap = ddr3TipGetTopologyMap(devNum);

	if(NULL == topologyMap)
	{
		#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
		/* for CPSS, since topology is not always initialized, it is
		   needed to set it to default topology */
		topologyMap = &bc2TopologyMap[0];
		#else
		return GT_FAIL;
		#endif
	}

	CHECK_STATUS(ddr3Bc2UpdateTopologyMap(devNum, topologyMap));

    ddr3TipInitBc2Silicon(devNum, boardId);
    return GT_OK;
}

/*****************************************************************************
Data Reset
******************************************************************************/
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
        retVal = ddr3TipBc2ServerRegWrite(devNum,     BC2_XSB_MAPPING(interfaceId, interfaceAccess, XSB_DATA_REG+(uiWordCnt * 4)), 0xABCDEF12);
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
GT_STATUS    ddr3TipBC2ExtRead
(
    GT_U32      devNum,
    GT_U32      interfaceId,
    GT_U32      regAddr,
    GT_U32      numOfBursts,
    GT_U32      *data
)
{
    GT_U32 burstNum, wordNum , dataValue;
	GT_U32 cntPoll;
    MV_HWS_ACCESS_TYPE accessType = ACCESS_TYPE_UNICAST;
    /*DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("=== EXTERNAL READ ==="));*/

    ddr3TipPipeEnable((GT_U8)devNum, accessType, interfaceId, GT_TRUE);
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, accessType, XSB_CTRL_0_REG) ,EXT_TRAINING_ID));

    for(burstNum=0 ; burstNum < numOfBursts; burstNum++)
    {
        ddr3TipDataReset(devNum, ACCESS_TYPE_UNICAST, interfaceId);
        /*working with XSB client InterfaceNum  Write Interface ADDR as data to XSB address C*/
        CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, accessType, XSB_ADDRESS_REG + (burstNum * 32)), regAddr));
        if (isCbeRequired ==  GT_TRUE)
        {
            /*CS_CBE_VALUE(0)*/
            dataValue = CS_CBE_VALUE(0) << 19;
        }
        else
        {
            dataValue = (GT_U32)(TARGET_EXT << 19);
        }
        dataValue |= (BYTE_EN << 11) + (NUM_BYTES_IN_BURST << 4) + (ACCESS_EXT << 3);
        /*Write Interface COMMAND as data to XSB address 8 */
        CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING( interfaceId, accessType, XSB_CMD_REG),   dataValue));
        dataValue |= EXECUTING;
        CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING( interfaceId, accessType, XSB_CMD_REG),   dataValue));

		for(cntPoll=0; cntPoll < MAX_POLLING_ITERATIONS; cntPoll++)
		{
			CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, BC2_XSB_MAPPING( interfaceId, accessType, XSB_CMD_REG), &dataValue, 0x1));

			if (dataValue == 0)
				break;
		}

		if (cntPoll >= MAX_POLLING_ITERATIONS)
		{
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("No Done indication for DDR Read\n", dataValue));
			return GT_NOT_READY;
		}

        for(wordNum = 0; wordNum < EXT_ACCESS_BURST_LENGTH /*s_uiNumOfBytesInBurst/4*/; wordNum++)
        {
            CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, BC2_XSB_MAPPING(interfaceId, accessType, XSB_DATA_REG + (wordNum * 4)), &data[wordNum], MASK_ALL_BITS));
            DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("ddr3TipExtRead data 0x%x \n",data[wordNum]));
        }
    }
    return GT_OK;
}



/*****************************************************************************
XSB External write
******************************************************************************/
GT_STATUS    ddr3TipBC2ExtWrite
(
    GT_U32      devNum,
    GT_U32      interfaceId,
    GT_U32      regAddr,
    GT_U32      numOfBursts,
    GT_U32      *addr
)
{
    GT_U32        wordNum = 0,  dataCmd = 0, burstNum=0, cntPoll = 0, dataValue  = 0;
    MV_HWS_ACCESS_TYPE accessType = ACCESS_TYPE_UNICAST;

    ddr3TipPipeEnable((GT_U8)devNum, accessType, interfaceId, GT_TRUE);
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, accessType, XSB_CTRL_0_REG) ,EXT_TRAINING_ID  ));
    for(burstNum=0 ; burstNum < numOfBursts; burstNum++)
    {
        /*working with XSB multicast client , Write Interface ADDR as data to XSB address C */
        CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, accessType, XSB_ADDRESS_REG), regAddr + (burstNum * EXT_ACCESS_BURST_LENGTH * 4)));
        for(wordNum = 0; wordNum < 8 ; wordNum++)
        {
            /*Write Interface DATA as data to XSB address 0x10.*/
            CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, accessType, XSB_DATA_REG + (wordNum * 4)), addr[wordNum]));
        }
        if (isCbeRequired ==  GT_TRUE)
        {
            dataCmd =  CS_CBE_VALUE(0) << 19;
        }
        else
        {
            dataCmd = (GT_U32)(TARGET_EXT << 19);
        }
        dataCmd |= (BYTE_EN << 11) + (NUM_BYTES_IN_BURST << 4) + (ACCESS_EXT << 3) + EXT_MODE;
        CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId,  accessType, XSB_CMD_REG), dataCmd));
        /* execute xsb write */
        dataCmd |= 0x1;
        DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("ddr3TipExtWrite dataCmd 0x%x \n", dataCmd));
        CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, BC2_XSB_MAPPING(interfaceId, accessType, XSB_CMD_REG), dataCmd));

		for(cntPoll=0; cntPoll < MAX_POLLING_ITERATIONS; cntPoll++)
		{
			CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, BC2_XSB_MAPPING( interfaceId, accessType, XSB_CMD_REG), &dataValue, 0x1));

			if (dataValue == 0)
				break;
		}

		if (cntPoll >= MAX_POLLING_ITERATIONS)
		{
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("No Done indication for DDR Write External\n", dataValue));
			return GT_NOT_READY;
		}

    }
    return GT_OK;
}

/******************************************************************************
 * Name:     ddr3GetSdramAssignment
 * Desc:     read S@R and return DDR3 assignment ( 0 = TM , 1 = MSYS )
 * Args:
 * Notes:
 * Returns:  required value
 */
DDR_IF_ASSIGNMENT ddr3GetSdramAssignment(GT_U8 devNum)
{
	GT_U32 data = 0; /* initialized for simulation */

	/* Read sample at reset setting */
	CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, BC2_DEVICE_SAR1_REG_ADDR,  &data, MASK_ALL_BITS));

	data = (data >> BC2_DEVICE_SAR1_MSYS_TM_SDRAM_SEL_OFFSET) & BC2_DEVICE_SAR1_MSYS_TM_SDRAM_SEL_MASK;

	return (data == 0) ? TM_EN : MSYS_EN;
}

/******************************************************************************/
/*   PLL/Frequency Functionality                                              */
/******************************************************************************/

/******************************************************************************
* Name:     ddr3TipBc2SetDivider.
* Desc:     Pll Divider
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS ddr3TipBc2SetDivider
(
	GT_U8							devNum,
	GT_U32							interfaceId,
    MV_HWS_DDR_FREQ                 frequency
)
{
	if(interfaceId < 4)
	{
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("TM set divider for interface %d\n",interfaceId));
		return ddr3TipTmSetDivider(devNum, frequency);
	}

	else
	{
		if (ddr3GetSdramAssignment(devNum) == TM_EN) {
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("TM set divider for interface 4\n"));
			return ddr3TipTmSetDivider(devNum, frequency);

		}else {
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("CPU set divider for interface 4\n"));
			return ddr3TipCpuSetDivider(devNum, frequency);
		}
	}
}

/******************************************************************************
* return 1 of core/DUNIT clock ration is 1 for given freq, 0 if clock ratios is 2:1
*/
static GT_U8    ddr3TipClockMode(GT_U32 frequency)
{
    frequency = frequency; /* avoid warnings */

    return 2;
}


/******************************************************************************
* Name:     ddr3TipTmSetDivider.
* Desc:     Pll Divider of The Trafic Manager Unit
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
static GT_STATUS ddr3TipTmSetDivider
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 frequency

)
{
    GT_U32 data = 0, writeData, divider = 0, cntPoll;
	GT_U32 version, regAddr;
    MV_HWS_DDR_FREQ sarFreq;

	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("TM PLL Config\n"));

    /* Calc SAR */
    CHECK_STATUS(ddr3TipTmGetInitFreq(devNum, &sarFreq));

    divider = freqVal[sarFreq]/freqVal[frequency];

    DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\tSAR value %d divider %d (freqVal[%d] %d  freqVal[%d] %d\n",
					  sarFreq, divider, sarFreq, freqVal[sarFreq], frequency, freqVal[frequency]));

    switch (divider)
    {
	case 1:
		writeData  = TM_PLL_REG_DATA(2,1,3);
		break;
	case 2:
		writeData  = TM_PLL_REG_DATA(4,2,3);
		break;
	case 3:
		writeData  = TM_PLL_REG_DATA(6,3,3);
		break;
	case 4:
		writeData  = TM_PLL_REG_DATA(8,4,3);
		break;

	default:
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Error: ddr3TipTmSetDivider: %d divider is not supported\n", divider));
		return GT_BAD_PARAM;
    }

    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8264,  &data, MASK_ALL_BITS ));
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F8264,  R_MOD_W(writeData,data,0xFF0C)));

    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8264,  &data, MASK_ALL_BITS));
	data |= (1<<16);
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F8264,  data));

	/*hwsOsExactDelayPtr(devNum, 0, 10); */

	CHECK_STATUS(ddr3TipBC2RevGet(devNum, &version));

	 regAddr = (version == 0 ) ? BC2_DEV_GENERAL_STATUS_REG1_ADDR_A0 : BC2_DEV_GENERAL_STATUS_REG1_ADDR_B0;

    for(cntPoll=0; cntPoll < MAX_POLLING_ITERATIONS; cntPoll++)
    {
        /*
        0x000F82B4/0x000F8C84	20:19	ddr1_top_pll_clkdiv_clk_stable	0x3
        0x000F82B4/0x000F8C84	15:14	ddr0_top_pll_clkdiv_clk_stable	0x3
        0x000F82B4/0x000F8C84	10:9	ddr1_bot_pll_clkdiv_clk_stable	0x3
        0x000F82B4/0x000F8C84	 5:4	ddr0_bot_pll_clkdiv_clk_stable	0x3
		*/

        CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, regAddr, &data, MASK_ALL_BITS));
        if ((data & 0x18C630)== 0x18C630)
            break;
    }

    if (cntPoll >= MAX_POLLING_ITERATIONS)
    {
  		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Error: ddr3TipTmSetDivider data = 0x%x: PLL - No stable indication was received\n",data));
        return GT_NOT_READY;

    }

	CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8264,  &data, MASK_ALL_BITS));
	data &= ~(1<<16);
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F8264,  R_MOD_W(0, data, (1<<16))));

    return GT_OK;
}

/******************************************************************************
* Name:     ddr3TipCpuSetDivider.
* Desc:     Pll Divider of the CPU(msys) Unit
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
static GT_STATUS ddr3TipCpuSetDivider
(
	GT_U8							devNum,
    MV_HWS_DDR_FREQ                 frequency
)
{
    GT_U32 data = 0, writeData, divider = 0;
    MV_HWS_DDR_FREQ sarFreq;
#if 0
	GT_U32 version, regAddr;
#endif
	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("CPU PLL Config\n"));

    /* calc SAR */
    ddr3TipCpuGetInitFreq(devNum, &sarFreq);
    divider = freqVal[sarFreq]/freqVal[frequency];

    DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("\nSAR value %d divider %d freqVal[%d] %d  freqVal[%d] %d\n",
					  sarFreq, divider, sarFreq, freqVal[sarFreq], frequency, freqVal[frequency]));

	CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F82ec,  &data, MASK_ALL_BITS ));
	writeData = (0x1 << 9);
	CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F82ec,  R_MOD_W(writeData,data, (0x1 << 9))));

    switch (divider)
    {
        case 1:
			/*Not 800 is a 667 only*/
            writeData = (sarFreq==DDR_FREQ_800)?(0x2):(0x1);
            break;
        case 2:
			/*Not 800 is a 667 only*/
            writeData = (sarFreq==DDR_FREQ_800)?(0x3):(0x2);
            break;
        default:
            DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("Error: Wrong divider %d\n", divider));
            return GT_BAD_PARAM;
            break;
    }

	CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F82e8,  &data, MASK_ALL_BITS ));
	CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F82e8,  R_MOD_W(writeData,data, (0x7 << 0))));
#if 0 /*The configuration below will be relevant for Revision Bx*/
	writeData = (0x7f << 25);
	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\t cpu_pll_clkdiv_align_en 0x%x\n", writeData));
    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8264,  &data, MASK_ALL_BITS ));
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F8264,  R_MOD_W(writeData,data, (0x7f << 25))));

	writeData = (0x7f << 7);
	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\t reset mask 0x%x\n", writeData));
    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8268,  &data, MASK_ALL_BITS ));
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F8268,  R_MOD_W(writeData,data, (0x7f << 7))));

	mvOsDelay(10);

	writeData = (0x7f << 11);
	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\tReload smooth to 0x%x\n", writeData));
    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8270,  &data, MASK_ALL_BITS ));
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F8270,  R_MOD_W(writeData,data, (0x7f << 11))));

	mvOsDelay(10);

	writeData = (0x7f << 0);
	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\tRelax 0x%x\n", writeData));
    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8268,  &data, MASK_ALL_BITS ));
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F8268,  R_MOD_W(writeData,data, (0x7f << 0))));

    switch (divider)
    {
        case 1:
		case 2:
        case 4:
            writeData = (divider << 12);
            break;
        default:
            DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Error: ddr3TipBc2SetDivider: Wrong divider %d\n", divider));
            return GT_BAD_PARAM;
    }

	/* set ratio_full field */
	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\tSet reload_ratio to 0x%x\n", writeData));
    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F826C,  &data, MASK_ALL_BITS ));
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F826C,  R_MOD_W(writeData,data, (0x3F << 12))));

	mvOsDelay(10);

	/* set reload_force field */
	writeData = (3 << 21);
	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\tSet reload_ratio to 0x%x\n", writeData));
    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8268,  &data, MASK_ALL_BITS));
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F8268, R_MOD_W(writeData, data, (0x7f << 21))));

	mvOsDelay(10);

	/* set reload_ratio field */
	writeData = (1 << 10);
	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\tSet reload_ratio to 0x%x\n", writeData));
    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8270,  &data, MASK_ALL_BITS));
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F8270,  R_MOD_W(writeData, data, (1 << 10))));

	mvOsDelay(10);

	/*hwsOsExactDelayPtr(devNum, 0, 10); */

	/* poll clk_stable field */
	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\tPoll for Stable indication\n"));

	CHECK_STATUS(ddr3TipBC2RevGet(devNum, &version));
	if (version == 0 )    /* BC2 A0 ID = 0*/
		regAddr = 0x000F82B0;
	else
		regAddr = 0x000F8C80;
    for(cntPoll=0; cntPoll < MAX_POLLING_ITERATIONS; cntPoll++)
    {
        CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, regAddr, &data, MASK_ALL_BITS));
        //if (((data >> 17) & 0x7F) == 0x3)
		if (((data >> 17) & 0x3) == 0x3)//moti - changed to check only the two dividers
            break;
    }

    if (cntPoll >= MAX_POLLING_ITERATIONS)
    {
        DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("Error: CPU PLL - No stable indication was recieved\n", data));
		return GT_NOT_READY;
    }

	/* set reload_ratio field */
	DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\tSet reload_ratio to 0\n"));
	CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8270,  &data, MASK_ALL_BITS));
	data &= ~(1 << 10);
    CHECK_STATUS(ddr3TipBc2ServerRegWrite(devNum, 0x000F8270,  R_MOD_W(0, data, (1<<16))));
#endif
    DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_INFO, ("\tCPU PLL config Done\n"));
    return GT_OK;
}

/******************************************************************************
* Name:     ddr3TipBc2GetInitFreq.
* Desc:     choose from where to extract the frequency (TM or CPU)
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
GT_STATUS ddr3TipBc2GetInitFreq
(
	GT_U8				devNum,
	GT_U32				interfaceId,
    MV_HWS_DDR_FREQ* 	sarFreq
)
{
	GT_STATUS res;

	if(interfaceId < 4)
	{
		res = ddr3TipTmGetInitFreq(devNum, sarFreq);
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("TM frequency for interface %d is %d\n",interfaceId, *sarFreq));
	}
	else
	{
		if (ddr3GetSdramAssignment(devNum) == TM_EN) {
			res = ddr3TipTmGetInitFreq(devNum, sarFreq);
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("TM frequency for interface 4 is %d\n",*sarFreq));

		}else {
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("CPU frequency for interface 4 is %d\n",*sarFreq));
			res = ddr3TipCpuGetInitFreq(devNum, sarFreq);
		}
	}
	return res;
}

/*****************************************************************************
TM interface frequency Get
******************************************************************************/
static GT_STATUS ddr3TipTmGetInitFreq
(
	GT_STATUS       devNum,
	MV_HWS_DDR_FREQ *freq
)
{
    GT_U32 data;

    /* calc SAR */
    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8204 ,  &data, MASK_ALL_BITS));
    data = (data >> 15) & 0x7;
#ifdef ASIC_SIMULATION
    data = 2;
#endif
    switch(data)
    {
	case 0:
		/* TM is disabled */
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("TM Disabled\n"));
		*freq = DDR_FREQ_LIMIT;
		return GT_NOT_INITIALIZED;

	case 1:
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("TM DDR_FREQ_800\n"));
		*freq = DDR_FREQ_800;
		break;

	case 2:
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("TM DDR_FREQ_933\n"));
		*freq = DDR_FREQ_933;
		break;

	case 3:
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("TM DDR_FREQ_667\n"));
		*freq = DDR_FREQ_667;
		break;

	default:
		DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Error: ddr3TipTmGetInitFreq: Unknown Freq SAR value 0x%x\n", data));
		*freq = DDR_FREQ_LIMIT;
		return GT_BAD_PARAM;
    }
    return GT_OK;
}

/*****************************************************************************
CPU interface frequency Get
******************************************************************************/
static GT_STATUS ddr3TipCpuGetInitFreq
(
    GT_STATUS       devNum,
    MV_HWS_DDR_FREQ *freq
)
{
    GT_U32 data;

    /* calc SAR */
    CHECK_STATUS(ddr3TipBc2ServerRegRead(devNum, 0x000F8204 ,  &data, MASK_ALL_BITS ));
    data = (data >> 18) & 0x7;
#ifdef ASIC_SIMULATION
    data = 3;
#endif

    switch(data)
    {
        case 0:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("MSYS DDR_FREQ_400\n"));
            *freq = DDR_FREQ_400;
            break;

        case 2:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("MSYS DDR_FREQ_667\n"));
            *freq = DDR_FREQ_667;
            break;

        case 3:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("MSYS DDR_FREQ_800\n"));
            *freq = DDR_FREQ_800;
            break;

        default:
            DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Error: ddr3TipCpuGetInitFreq: Unknown SAR value 0x%x\n", data));
            *freq = DDR_FREQ_LIMIT;
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/******************************************************************************
* Name:     ddr3TipBc2GetMediumFreq.
* Desc:
* Args:
* Notes:
* Returns:  GT_OK if success, other error code if fail.
*/
static GT_STATUS ddr3TipBc2GetMediumFreq
(
	GT_U32 devNum,
	GT_U32 interfaceId,
	MV_HWS_DDR_FREQ *freq
)
{
    MV_HWS_DDR_FREQ sarFreq;

	CHECK_STATUS(ddr3TipBc2GetInitFreq((GT_U8)devNum, interfaceId, &sarFreq));

    switch(sarFreq)
    {
        case DDR_FREQ_400:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("No medium freq supported for 400Mhz\n"));
            *freq = DDR_FREQ_400;
            break;

        case DDR_FREQ_667:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("Medium DDR_FREQ_333\n"));
            *freq = DDR_FREQ_333;
            break;

        case DDR_FREQ_800:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("Medium DDR_FREQ_400\n"));
            *freq = DDR_FREQ_400;
            break;

        case DDR_FREQ_933:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_TRACE, ("Medium DDR_FREQ_311\n"));
            *freq = DDR_FREQ_311;
            break;

        default:
			DEBUG_TRAINING_ACCESS(DEBUG_LEVEL_ERROR, ("Error: ddr3TipBc2GetMediumFreq: Freq %d is not supported\n", sarFreq));
            return GT_FAIL;
    }
    return GT_OK;
}


GT_STATUS ddr3TipBc2GetDeviceInfo
(
    GT_U8      devNum,
    MV_DDR3_DEVICE_INFO * infoPtr
)
{
    devNum = devNum; /* avoid warnings */

	infoPtr->deviceId = 0xFC00;
	infoPtr->ckDelay = ckDelay;

	return GT_OK;
}


