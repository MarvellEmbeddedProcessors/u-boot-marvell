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
* mvHwsTrainingIpDdr3TrainingIf.c
*
* DESCRIPTION: DDR3 training IP configuration
* Based on JEDEC: JESD79-3F
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
******************************************************************************/

#include "mvDdrTrainingIpDb.h"
#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpFlow.h"
#include "mvDdr3TrainingIpDef.h"

/************************** Globals ******************************/
/*Device attributes structures*/
MV_DDR_DEV_ATTRIBUTE ddrDevAttributes[HWS_MAX_DEVICE_NUM][MV_ATTR_LAST];  /* holds device attributes */
GT_BOOL ddrDevAttrInitDone[HWS_MAX_DEVICE_NUM] = {0};	/* described whether device attributes were initialized */

#ifdef CONFIG_DDR3
MV_HWS_TOPOLOGY_MAP *topologyMapDb[HWS_MAX_DEVICE_NUM] = {NULL};          /* holds device DDR topology */

/* list of allowed frequency listed in order of MV_HWS_DDR_FREQ */
GT_U32 freqVal[DDR_FREQ_LIMIT] =
{
    0, /*DDR_FREQ_LOW_FREQ*/
    400, /*DDR_FREQ_400,*/
    533, /*DDR_FREQ_533,*/
    666, /*DDR_FREQ_667,*/
    800, /*DDR_FREQ_800,*/
    933, /*DDR_FREQ_933,*/
   1066, /*DDR_FREQ_1066,*/
    311, /*DDR_FREQ_311,*/
    333, /*DDR_FREQ_333,*/
    467,  /*DDR_FREQ_467,*/
    850,  /*DDR_FREQ_850,*/
    600,  /*DDR_FREQ_600*/
    300,  /*DDR_FREQ_300*/
	900,  /*DDR_FREQ_900*/
	360,  /*DDR_FREQ_360*/
	1000  /*DDR_FREQ_1000*/
};

/* Table for CL values per frequency for each speed bin index */
 ClValuePerFreq casLatencyTable[] =
{
   /*      400M   667M     933M   311M     467M	 600M	 360
       100M    533M    800M    1066M   333M    850M 	900   1000(the order is 100, 400, 533 etc.)*/
	{{ 6 , 5 , 0 , 0 , 0 , 0 , 0 , 5 , 5 , 0 , 0 , 0, 5, 0, 5, 0}}, /* DDR3-800D  */
	{{ 6 , 6 , 0 , 0 , 0 , 0 , 0 , 6 , 6 , 0 , 0 , 0, 6, 0, 6, 0}}, /* DDR3-800E  */
	{{ 6 , 5 , 6 , 0 , 0 , 0 , 0 , 5 , 5 , 6 , 0 , 0, 5, 0,	5, 0}}, /* DDR3-1066E  */
	{{ 6 , 6 , 7 , 0 , 0 , 0 , 0 , 6 , 6 , 7 , 0 , 0, 6, 0,	6, 0}}, /* DDR3-1066F  */
	{{ 6 , 6 , 8 , 0 , 0 , 0 , 0 , 6 , 6 , 8 , 0 , 0, 6, 0, 6, 0}}, /* DDR3-1066G  */
	{{ 6 , 5 , 6 , 7 , 0 , 0 , 0 , 5 , 5 , 6 , 0 , 7, 5, 0, 5, 0}}, /* DDR3-1333F*  */
	{{ 6 , 5 , 7 , 8 , 0 , 0 , 0 , 5 , 5 , 7 , 0 , 8, 5, 0, 5, 0}}, /* DDR3-1333G  */
	{{ 6 , 6 , 8 , 9 , 0 , 0 , 0 , 6 , 6 , 8 , 0 , 9, 6, 0, 6, 0}}, /* DDR3-1333H  */
	{{ 6 , 6 , 8 , 10 ,0 , 0 , 0 , 6 , 6 , 8 , 0 , 10, 6,0, 6, 0}}, /* DDR3-1333J*  */
	{{ 6 , 5 , 6 , 7 , 8 , 0 , 0 , 5 , 5 , 6 , 0 , 7, 5, 0, 5, 0}}, /* DDR3-1600G*  */
	{{ 6 , 5 , 6 , 8 , 9 , 0 , 0 , 5 , 5 , 6 , 0 , 8, 5, 0, 5, 0}}, /* DDR3-1600H  */
	{{ 6 , 5 , 7 , 9 , 10 ,0 , 0 , 5 , 5 , 7 , 0 , 9, 5, 0, 5, 0}}, /* DDR3-1600J  */
	{{ 6 , 6 , 8 , 10 ,11 ,0 , 0 , 6 , 6 , 8 , 0 , 10, 6, 0, 6, 0}}, /* DDR3-1600K  */
	{{ 6 , 5 , 6 , 8 , 9 , 11 , 0 , 5 , 5 , 6 , 11 , 8, 5, 0, 5, 0}}, /* DDR3-1866J*  */
	{{ 6 , 5 , 7 , 8 , 10 ,11 , 0 , 5 , 5 , 7 , 11 , 8, 5, 11, 5, 11}}, /* DDR3-1866K  */
	{{ 6 , 6 , 7 , 9 , 11 ,12 , 0 , 6 , 6 , 7 , 12 , 9, 6, 12, 6, 12}}, /* DDR3-1866L  */
	{{ 6 , 6 , 8 , 10 ,11 ,13 , 0 , 6 , 6 , 8 , 13 , 10, 6, 13, 6, 13}}, /* DDR3-1866M*   */
	{{ 6 , 5 , 6 , 7 , 9 , 10 , 11 , 5 , 5 , 6 , 10 , 7, 5, 11, 5, 11}}, /* DDR3-2133K*  */
	{{ 6 , 5 , 6 , 8 , 9 , 11 , 12 , 5 , 5 , 6 , 11 , 8, 5, 12, 5, 12}}, /* DDR3-2133L  */
	{{ 6 , 5 , 7 , 9 , 10 ,12 , 13 , 5 , 5 , 7 , 12 , 9, 5, 13, 5, 13}}, /* DDR3-2133M  */
	{{ 6 , 6 , 7 , 9 , 11 ,13 , 14 , 6 , 6 , 7 , 13 , 9, 6, 14, 6, 14}}, /* DDR3-2133N*  */

	{{ 6 , 6 , 7 , 9 , 0 , 0 , 0 , 6 , 6 , 7 , 0 , 9, 6, 0, 6, 0}}, /* DDR3-1333H-ext  */
	{{ 6 , 6 , 7 , 9 , 11 ,0 , 0 , 6 , 6 , 7 , 0 , 9, 6, 0, 6, 0}}, /* DDR3-1600K-ext  */
	{{ 6 , 6 , 7 , 9 , 11 ,13 ,0 , 6 , 6 , 7 , 13 , 9, 6,13,6, 13}}, /* DDR3-1866M-ext  */
};


/* Table for CWL values per speedbin index */

 ClValuePerFreq casWriteLatencyTable[] =
{
   /*      400M   667M     933M   311M     467M  600M	 360
       100M    533M    800M    1066M   333M    850M 	900(the order is 100, 400, 533 etc.)*/
	{{ 5 , 5 , 0 , 0 , 0 , 0 , 0 , 5 , 5 , 0 , 0 , 0,  5, 0, 5, 0}}, /* DDR3-800D  */
	{{ 5 , 5 , 0 , 0 , 0 , 0 , 0 , 5 , 5 , 0 , 0 , 0,  5, 0, 5, 0}}, /* DDR3-800E  */
	{{ 5 , 5 , 6 , 0 , 0 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1066E  */
	{{ 5 , 5 , 6 , 0 , 0 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1066F  */
	{{ 5 , 5 , 6 , 0 , 0 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1066G  */
	{{ 5 , 5 , 6 , 7 , 0 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1333F*  */
	{{ 5 , 5 , 6 , 7 , 0 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1333G  */
	{{ 5 , 5 , 6 , 7 , 0 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1333H  */
	{{ 5 , 5 , 6 , 7 , 0 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1333J*  */
	{{ 5 , 5 , 6 , 7 , 8 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1600G*  */
	{{ 5 , 5 , 6 , 7 , 8 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1600H  */
	{{ 5 , 5 , 6 , 7 , 8 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1600J  */
	{{ 5 , 5 , 6 , 7 , 8 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1600K  */
	{{ 5 , 5 , 6 , 7 , 8 , 9 , 0 , 5 , 5 , 6 , 9 , 7,  5, 0, 5, 0}}, /* DDR3-1866J*  */
	{{ 5 , 5 , 6 , 7 , 8 , 9 , 0 , 5 , 5 , 6 , 9 , 7,  5, 0, 5, 0}}, /* DDR3-1866K  */
	{{ 5 , 5 , 6 , 7 , 8 , 9 , 0 , 5 , 5 , 6 , 9 , 7,  5, 9, 5, 9}}, /* DDR3-1866L  */
	{{ 5 , 5 , 6 , 7 , 8 , 9 , 0 , 5 , 5 , 6 , 9 , 7,  5, 9, 5, 9}}, /* DDR3-1866M*   */
	{{ 5 , 5 , 6 , 7 , 8 , 9 , 10 , 5 , 5 , 6 , 9 , 7,  5,9, 5, 10}}, /* DDR3-2133K*  */
	{{ 5 , 5 , 6 , 7 , 8 , 9 , 10 , 5 , 5 , 6 , 9 , 7,  5,9, 5, 10}}, /* DDR3-2133L  */
	{{ 5 , 5 , 6 , 7 , 8 , 9 , 10 , 5 , 5 , 6 , 9 , 7,  5,9, 5, 10}}, /* DDR3-2133M  */
	{{ 5 , 5 , 6 , 7 , 8 , 9 , 10 , 5 , 5 , 6 , 9 , 7,  5,9, 5, 10}}, /* DDR3-2133N*  */

	{{ 5 , 5 , 6 , 7 , 0 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1333H-ext  */
	{{ 5 , 5 , 6 , 7 , 8 , 0 , 0 , 5 , 5 , 6 , 0 , 7,  5, 0, 5, 0}}, /* DDR3-1600K-ext  */
	{{ 5 , 5 , 6 , 7 , 8 , 9 , 0 , 5 , 5 , 6 , 9 , 7,  5, 9, 5, 9}}, /* DDR3-1866M-ext  */
};

GT_U8 twrMaskTable[] =
{
    10,
    10,
    10,
    10,
    10,
    1, /*5*/
    2, /*6*/
    3, /*7*/
    4, /*8*/
    10,
    5, /*10*/
    10,
    6, /*12*/
    10,
    7, /*14*/
    10,
    0  /*16*/
};

GT_U8 clMaskTable[] =
{
    0,
    0,
    0,
    0,
    0,
    0x2,
    0x4,
    0x6,
    0x8,
    0xA,
    0xC,
    0xE,
    0x1,
    0x3,
    0x5,
    0x5
};

GT_U8 cwlMaskTable[] =
{
    0,
    0,
    0,
    0,
    0,
    0,
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8,
    0x9,
    0x9
};

/* RFC values (in ns) */
GT_U16 rfcTable[] =
{
   90, /* 512M */
   110,/* 1G */
   160,/* 2G */
   260,/* 4G */
   350 /* 8G */
};

GT_U32 speedBinTableTRc[] =
{
	50000,
	52500,
	48750,
	50625,
	52500,
	46500,
	48000,
	49500,
	51000,
	45000,
	46250,
	47500,
	48750,
	44700,
	45770,
	46840,
	47910,
	43285,
	44220,
	45155,
	46090
};

GT_U32 speedBinTableTRcdTRp[] =
{
	12500,
	15000,
	11250,
	13125,
	15000,
	10500,
	12000,
	13500,
	15000,
	10000,
	11250,
	12500,
	13750,
	10700,
	11770,
	12840,
	13910,
	10285,
	11220,
	12155,
	13090,
};

typedef enum
{
   patternKillerPatternTableMapRoleAggressor = 0,
   patternKillerPatternTableMapRoleVictim
}patternKillerPatternTableMapRole;

static  GT_U8 patternKillerPatternTableMap[KILLER_PATTERN_LENGTH*2][2] =
{/*Aggressor / Victim*/
	{ 1 , 0 },
	{ 0 , 0 },
	{ 1 , 0 },
	{ 1 , 1 },
	{ 0 , 1 },
	{ 0 , 1 },
	{ 1 , 0 },
	{ 0 , 1 },
	{ 1 , 0 },
	{ 0 , 1 },
	{ 1 , 0 },
	{ 1 , 0 },
	{ 0 , 1 },
	{ 1 , 0 },
	{ 0 , 1 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 1 , 0 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 0 , 0 },
	{ 0 , 0 },
	{ 0 , 0 },
	{ 0 , 1 },
	{ 0 , 1 },
	{ 1 , 1 },
	{ 0 , 0 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 1 , 1 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 1 , 1 },
	{ 0 , 0 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 0 , 1 },
	{ 0 , 0 },
	{ 0 , 1 },
	{ 0 , 1 },
	{ 0 , 0 },
	{ 1 , 1 },
	{ 1 , 1 },
	{ 1 , 0 },
	{ 1 , 0 },
	{ 1 , 1 },
	{ 1 , 1 },
	{ 1 , 1 },
	{ 1 , 1 },
	{ 1 , 1 },
	{ 1 , 1 },
	{ 1 , 1 }
};

static  GT_U8 patternVrefPatternTableMap[] =
{/*1 means 0xFFFFFFFF, 0 is 0x0*/
	0xB8,
	0x52,
	0x55,
	0x8A,
	0x33,
	0xA6,
	0x6D,
	0xFE
};

/************************** functions ******************************/

/*Return speed Bin value for selected index and t* element*/
GT_U32 speedBinTable
(
    GT_U8 index,
    speedBinTableElements element
)
{
	GT_U32 result = 0;

	switch(element)
	{
		case speedBinTableElements_tRCD:
		case speedBinTableElements_tRP:
			result = speedBinTableTRcdTRp[index];
			break;
		case speedBinTableElements_tRAS:
			if(index <= SPEED_BIN_DDR_1066G) result = 37500;
			else if(index <= SPEED_BIN_DDR_1333J) result = 36000;
			else if(index <= SPEED_BIN_DDR_1600K) result = 35000;
			else if(index <= SPEED_BIN_DDR_1866M) result = 34000;
			else result = 33000;
			break;
		case speedBinTableElements_tRC:
			result = speedBinTableTRc[index];
			break;
        case speedBinTableElements_tRRD1K:
            if(index <= SPEED_BIN_DDR_800E) result = 10000;
			else if(index <= SPEED_BIN_DDR_1066G) result = 7500;
			else if(index <= SPEED_BIN_DDR_1600K) result = 6000;
			else result = 5000;
			break;
		case speedBinTableElements_tRRD2K:
			if(index <= SPEED_BIN_DDR_1066G) result = 10000;
			else if(index <= SPEED_BIN_DDR_1600K) result = 7500;
			else result = 6000;
			break;
		case speedBinTableElements_tPD:
			if(index <= SPEED_BIN_DDR_800E) result = 7500;
			else if(index <= SPEED_BIN_DDR_1333J) result = 5625;
			else result = 5000;
			break;
		case speedBinTableElements_tFAW1K:
			if(index <= SPEED_BIN_DDR_800E) result = 40000;
			else if(index <= SPEED_BIN_DDR_1066G) result = 37500;
			else if(index <= SPEED_BIN_DDR_1600K) result = 30000;
			else if(index <= SPEED_BIN_DDR_1866M) result = 27000;
			else result = 25000;
			break;
		case speedBinTableElements_tFAW2K:
			if(index <= SPEED_BIN_DDR_1066G) result = 50000;
			else if(index <= SPEED_BIN_DDR_1333J) result = 45000;
			else if(index <= SPEED_BIN_DDR_1600K) result = 40000;
			else result = 35000;
			break;
		case speedBinTableElements_tWTR:
			result = 7500;
			break;
		case speedBinTableElements_tRTP:
			result = 7500;
			break;
		case speedBinTableElements_tWR:
			result = 15000;
			break;
		case speedBinTableElements_tMOD:
			result = 15000;
			break;
		case speedBinTableElements_tXPDLL:
                        result = 24000;
                        break;

		default:
			break;
	}

	return result;
}

INLINE static GT_U32 patternTableGetKillerWord( GT_U8 dqs, GT_U8 index)
{
	GT_U8 i, byte = 0;

	for(i=0; i<8; i++){
		GT_U8 role = (i==dqs)?(patternKillerPatternTableMapRoleAggressor):(patternKillerPatternTableMapRoleVictim);
		byte |= patternKillerPatternTableMap[index][role] << i;
	}

	return byte | (byte << 8) | (byte << 16) | (byte << 24);
}

INLINE static GT_U32 patternTableGetKillerWord16( GT_U8 dqs, GT_U8 index)
{
	GT_U8 i, byte0 = 0, byte1 = 0;

	for(i=0; i<8; i++){
		GT_U8 role = (i==dqs)?(patternKillerPatternTableMapRoleAggressor):(patternKillerPatternTableMapRoleVictim);
		byte0 |= patternKillerPatternTableMap[index*2][role] << i;
	}

	for(i=0; i<8; i++){
		GT_U8 role = (i==dqs)?(patternKillerPatternTableMapRoleAggressor):(patternKillerPatternTableMapRoleVictim);
		byte1 |= patternKillerPatternTableMap[index*2+1][role] << i;
	}

	return byte0 | (byte0 << 8) | (byte1 << 16) | (byte1 << 24);
}

INLINE static GT_U32 patternTableGetSsoWord( GT_U8 sso, GT_U8 index)
{
	GT_U8 step = sso + 1;

	if( 0 == ((index/step)&1) ){
		return 0x0;
	}
	else{
		return 0xFFFFFFFF;
	}
}

INLINE static GT_U32 patternTableGetSsoFullXtalkWord( GT_U8 bit, GT_U8 index)
{
	GT_U8 byte = (1 << bit);

	if( 1 == (index&1) ){
		byte = ~byte;
	}

	return (byte | (byte<<8) | (byte<<16) | (byte<<24));
}

INLINE static GT_U32 patternTableGetSsoXtalkFreeWord( GT_U8 bit, GT_U8 index)
{
	GT_U8 byte = (1 << bit);

	if( 1 == (index&1) ){
		byte = 0;
	}

	return (byte | (byte<<8) | (byte<<16) | (byte<<24));
}

INLINE static GT_U32 patternTableGetISIWord( GT_U8 index)
{
	GT_U8 I0 =  index%32;
	GT_U8 I1 =  index%8;
	GT_U32	Word;

	if(I0 > 15)
	{
		Word =  ((I1 ==5)|(I1==7))?0xFFFFFFFF:0x0;
	}else{
		Word =  (I1 ==6)?0xFFFFFFFF:0x0;
	}

	Word = ((I0%16)>7)?~Word:Word;
	return Word;
}

INLINE static GT_U32 patternTableGetSsoFullXtalkWord_16( GT_U8 bit, GT_U8 index)
{
	GT_U8 byte = (1 << bit);

	if( 1 == (index&1) ){
		byte = ~byte;
	}

	return (byte | (byte<<8) | ((~byte)<<16) | ((~byte)<<24));
}

INLINE static GT_U32 patternTableGetSsoXtalkFreeWord_16( GT_U8 bit, GT_U8 index)
{
	GT_U8 byte = (1 << bit);

	if( 0 == ((index)&1) ){
		return ((byte<<16) | (byte<<24));
	}
	else{
		return (byte | (byte<<8));
	}

}
INLINE static GT_U32 patternTableGetISIWord_16( GT_U8 index)
{
	GT_U8 I0 =  index%16;
	GT_U8 I1 =  index%4;
	GT_U32	Word;

	if(I0 > 7)
	{
 		Word = ( I1>1)?0x0000FFFF:0x0;
	}else{
		Word =  (I1 ==3)?0xFFFF0000:0x0;
	}

	Word = ((I0%8)>3)?~Word:Word;
	return Word;
}

INLINE static GT_U32 patternTableGetVrefWord(GT_U8 index)
{
	if( 0 == ((patternVrefPatternTableMap[index/8] >> (index%8))&1) ){
		return 0x0;
	}
	else{
		return 0xFFFFFFFF;
	}
}

INLINE static GT_U32 patternTableGetVrefWord16(GT_U8 index)
{
	if( 0 == patternKillerPatternTableMap[patternKillerPatternTableMapRoleVictim][index*2] &&
		0 == patternKillerPatternTableMap[patternKillerPatternTableMapRoleVictim][index*2+1] )
		return 0x00000000;
	else if( 1 == patternKillerPatternTableMap[patternKillerPatternTableMapRoleVictim][index*2] &&
		0 == patternKillerPatternTableMap[patternKillerPatternTableMapRoleVictim][index*2+1] )
		return 0xFFFF0000;
	else if( 0 == patternKillerPatternTableMap[patternKillerPatternTableMapRoleVictim][index*2] &&
		1 == patternKillerPatternTableMap[patternKillerPatternTableMapRoleVictim][index*2+1] )
		return 0x0000FFFF;
	else
		return 0xFFFFFFFF;
}

INLINE static GT_U32 patternTableGetStaticPbsWord(GT_U8 index)
{
	GT_U16 temp;

	temp = ((0x00FF<<(index/3)) & 0xFF00) >> 8;
	return temp | (temp << 8) | (temp << 16) | (temp << 24);
}

INLINE GT_U32 patternTableGetWord
(
    GT_U32  devNum,
    MV_HWS_PATTERN type,
    GT_U8 index
)
{
	GT_U32 pattern;
    MV_HWS_TOPOLOGY_MAP* topologyMap = ddr3TipGetTopologyMap(devNum);

	if( DDR3_IS_16BIT_DRAM_MODE(topologyMap->activeBusMask) == GT_FALSE){
	/*32bit patterns*/
		switch(type){
		case PATTERN_PBS1:
		case PATTERN_PBS2:
			if(index == 0 || index == 2 || index == 5 || index == 7 ){
				pattern = PATTERN_55;
			}
			else{
				pattern = PATTERN_AA;
			}
			break;
		case PATTERN_PBS3:
			if(0 == (index&1)){
				pattern = PATTERN_55;
			}
			else{
				pattern = PATTERN_AA;
			}
			break;
		case PATTERN_RL:
			if(index < 6){ 
				pattern = PATTERN_00;
			}
			else{
				pattern = PATTERN_80;
			}
			break;
		case PATTERN_STATIC_PBS:
			pattern = patternTableGetStaticPbsWord(index);
			break;
		case PATTERN_KILLER_DQ0:
		case PATTERN_KILLER_DQ1:
		case PATTERN_KILLER_DQ2:
		case PATTERN_KILLER_DQ3:
		case PATTERN_KILLER_DQ4:
		case PATTERN_KILLER_DQ5:
		case PATTERN_KILLER_DQ6:
		case PATTERN_KILLER_DQ7:
				pattern = patternTableGetKillerWord((GT_U8)(type - PATTERN_KILLER_DQ0), index);
			break;
		case PATTERN_RL2:
			if(index < 6){
				pattern = PATTERN_00;
			}
			else{
				pattern = PATTERN_01;
			}
			break;
		case PATTERN_TEST:
			if( index>1 && index<6){
				pattern = PATTERN_20;
			}
			else{
				pattern = PATTERN_00;
			}
			break;
		case PATTERN_FULL_SSO0:
		case PATTERN_FULL_SSO1:
		case PATTERN_FULL_SSO2:
		case PATTERN_FULL_SSO3:
			pattern = patternTableGetSsoWord((GT_U8)(type - PATTERN_FULL_SSO0), index);
			break;
		case PATTERN_VREF:
			pattern = patternTableGetVrefWord(index);
			break;
		case PATTERN_SSO_FULL_XTALK_DQ0:
		case PATTERN_SSO_FULL_XTALK_DQ1:
		case PATTERN_SSO_FULL_XTALK_DQ2:
		case PATTERN_SSO_FULL_XTALK_DQ3:
		case PATTERN_SSO_FULL_XTALK_DQ4:
		case PATTERN_SSO_FULL_XTALK_DQ5:
		case PATTERN_SSO_FULL_XTALK_DQ6:
		case PATTERN_SSO_FULL_XTALK_DQ7:
			pattern = patternTableGetSsoFullXtalkWord((GT_U8)(type - PATTERN_SSO_FULL_XTALK_DQ0), index);
			break;
		case PATTERN_SSO_XTALK_FREE_DQ0:
		case PATTERN_SSO_XTALK_FREE_DQ1:
		case PATTERN_SSO_XTALK_FREE_DQ2:
		case PATTERN_SSO_XTALK_FREE_DQ3:
		case PATTERN_SSO_XTALK_FREE_DQ4:
		case PATTERN_SSO_XTALK_FREE_DQ5:
		case PATTERN_SSO_XTALK_FREE_DQ6:
		case PATTERN_SSO_XTALK_FREE_DQ7:
			pattern = patternTableGetSsoXtalkFreeWord((GT_U8)(type - PATTERN_SSO_XTALK_FREE_DQ0), index);
			break;
		case PATTERN_ISI_XTALK_FREE:
			pattern = patternTableGetISIWord( index);
			break;
		default:
			pattern = 0;
			break;
		}
	}
	else{
	/*16bit patterns*/
		switch(type){
		case PATTERN_PBS1:
		case PATTERN_PBS2:
		case PATTERN_PBS3:
			pattern = PATTERN_55AA;
			break;
		case PATTERN_RL:
			if(index < 3){ 
				pattern = PATTERN_00;
			}
			else{
				pattern = PATTERN_80;
			}
			break;
		case PATTERN_STATIC_PBS:
			pattern = PATTERN_00FF;
			break;
		case PATTERN_KILLER_DQ0:
		case PATTERN_KILLER_DQ1:
		case PATTERN_KILLER_DQ2:
		case PATTERN_KILLER_DQ3:
		case PATTERN_KILLER_DQ4:
		case PATTERN_KILLER_DQ5:
		case PATTERN_KILLER_DQ6:
		case PATTERN_KILLER_DQ7:
				pattern = patternTableGetKillerWord16((GT_U8)(type - PATTERN_KILLER_DQ0), index);
			break;
		case PATTERN_RL2:
			if(index < 3){
				pattern = PATTERN_00;
			}
			else{
				pattern = PATTERN_01;
			}
			break;
		case PATTERN_TEST:
			/*pattern = PATTERN_0080;*/
           if((index == 0) || (index == 3)){
				pattern = 0x00000000;
			}
			else{
				pattern = 0xFFFFFFFF;
			}
			break;
		case PATTERN_FULL_SSO0:
			pattern = 0x0000FFFF;
			break;
		case PATTERN_FULL_SSO1:
		case PATTERN_FULL_SSO2:
		case PATTERN_FULL_SSO3:
			pattern = patternTableGetSsoWord((GT_U8)(type - PATTERN_FULL_SSO1), index);
			break;
		case PATTERN_VREF:
			pattern = patternTableGetVrefWord16(index);
			break;
		case PATTERN_SSO_FULL_XTALK_DQ0:
		case PATTERN_SSO_FULL_XTALK_DQ1:
		case PATTERN_SSO_FULL_XTALK_DQ2:
		case PATTERN_SSO_FULL_XTALK_DQ3:
		case PATTERN_SSO_FULL_XTALK_DQ4:
		case PATTERN_SSO_FULL_XTALK_DQ5:
		case PATTERN_SSO_FULL_XTALK_DQ6:
		case PATTERN_SSO_FULL_XTALK_DQ7:
			pattern = patternTableGetSsoFullXtalkWord_16((GT_U8)(type - PATTERN_SSO_FULL_XTALK_DQ0), index);
			break;
		case PATTERN_SSO_XTALK_FREE_DQ0:
		case PATTERN_SSO_XTALK_FREE_DQ1:
		case PATTERN_SSO_XTALK_FREE_DQ2:
		case PATTERN_SSO_XTALK_FREE_DQ3:
		case PATTERN_SSO_XTALK_FREE_DQ4:
		case PATTERN_SSO_XTALK_FREE_DQ5:
		case PATTERN_SSO_XTALK_FREE_DQ6:
		case PATTERN_SSO_XTALK_FREE_DQ7:
			pattern = patternTableGetSsoXtalkFreeWord_16((GT_U8)(type - PATTERN_SSO_XTALK_FREE_DQ0), index);
			break;
		case PATTERN_ISI_XTALK_FREE:
			pattern = patternTableGetISIWord_16( index);
			break;
		default:
			pattern = 0;
			break;
		}
	}

	return pattern;
}


MV_HWS_TOPOLOGY_MAP*    ddr3TipGetTopologyMap
(
    GT_U32  devNum
)
{
    return topologyMapDb[devNum];
}

void    ddr3TipSetTopologyMap
(
    GT_U32  devNum,
    MV_HWS_TOPOLOGY_MAP* topology
)
{
    topologyMapDb[devNum] = topology;
}

#endif /* CONFIG_DDR3 */

/*****************************************************************************
Device attributes functions
******************************************************************************/
void    ddr3TipDevAttrInit
(
    GT_U32  devNum
)
{
    GT_U32 attrId;

    for(attrId = 0; attrId < MV_ATTR_LAST; attrId++)
    {
        ddrDevAttributes[devNum][attrId] = 0xFF;
    }

    ddrDevAttrInitDone[devNum] = GT_TRUE;
}

GT_U32    ddr3TipDevAttrGet
(
    GT_U32                  devNum,
    MV_DDR_DEV_ATTRIBUTE    attrId
)
{
    if(ddrDevAttrInitDone[devNum] == GT_FALSE)
    {
        ddr3TipDevAttrInit(devNum);
    }

    return ddrDevAttributes[devNum][attrId];
}

void    ddr3TipDevAttrSet
(
    GT_U32                  devNum,
    MV_DDR_DEV_ATTRIBUTE    attrId,
    GT_U32                  value
)
{
    if(ddrDevAttrInitDone[devNum] == GT_FALSE)
    {
        ddr3TipDevAttrInit(devNum);
    }

    ddrDevAttributes[devNum][attrId] = value;
}



