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
* mvHwsDdr3TrainingIpEngine.c
*
* DESCRIPTION: DDR3 training IP Engine
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 80 $
******************************************************************************/

#include "mvDdr3TrainingIpEngine.h"
#include "mvDdrTrainingIpDb.h"
#include "mvDdr3TrainingIpBist.h"
#include "mvDdr3LoggingDef.h"

extern GT_U8  debugTrainingIp;

#define PATTERN_1 (0x55555555)
#define PATTERN_2 (0xAAAAAAAA)

#define VALIDATE_TRAINING_LIMIT(e1,e2) (((e2-e1+1)>33) && (e1<67))
GT_U32 phyRegBk[MAX_INTERFACE_NUM][MAX_BUS_NUM][BUS_WIDTH_IN_BITS];

#define PARAM_NOT_CARE   (0)

extern MV_HWS_TOPOLOGY_MAP *topologyMap;
extern GT_U32 effective_cs;

/************************** globals ***************************************/
GT_U32 trainingRes[MAX_INTERFACE_NUM * MAX_BUS_NUM * BUS_WIDTH_IN_BITS* MV_HWS_SearchDirLimit];

GT_U16 maskResultsDqRegMap[] =
{
    RESULT_CONTROL_PUP_0_BIT_0_REG, RESULT_CONTROL_PUP_0_BIT_1_REG, RESULT_CONTROL_PUP_0_BIT_2_REG, RESULT_CONTROL_PUP_0_BIT_3_REG,
    RESULT_CONTROL_PUP_0_BIT_4_REG, RESULT_CONTROL_PUP_0_BIT_5_REG, RESULT_CONTROL_PUP_0_BIT_6_REG, RESULT_CONTROL_PUP_0_BIT_7_REG,
    RESULT_CONTROL_PUP_1_BIT_0_REG, RESULT_CONTROL_PUP_1_BIT_1_REG, RESULT_CONTROL_PUP_1_BIT_2_REG, RESULT_CONTROL_PUP_1_BIT_3_REG,
    RESULT_CONTROL_PUP_1_BIT_4_REG, RESULT_CONTROL_PUP_1_BIT_5_REG, RESULT_CONTROL_PUP_1_BIT_6_REG, RESULT_CONTROL_PUP_1_BIT_7_REG,
    RESULT_CONTROL_PUP_2_BIT_0_REG, RESULT_CONTROL_PUP_2_BIT_1_REG, RESULT_CONTROL_PUP_2_BIT_2_REG, RESULT_CONTROL_PUP_2_BIT_3_REG,
    RESULT_CONTROL_PUP_2_BIT_4_REG, RESULT_CONTROL_PUP_2_BIT_5_REG, RESULT_CONTROL_PUP_2_BIT_6_REG, RESULT_CONTROL_PUP_2_BIT_7_REG,
    RESULT_CONTROL_PUP_3_BIT_0_REG, RESULT_CONTROL_PUP_3_BIT_1_REG, RESULT_CONTROL_PUP_3_BIT_2_REG, RESULT_CONTROL_PUP_3_BIT_3_REG,
    RESULT_CONTROL_PUP_3_BIT_4_REG, RESULT_CONTROL_PUP_3_BIT_5_REG, RESULT_CONTROL_PUP_3_BIT_6_REG, RESULT_CONTROL_PUP_3_BIT_7_REG,
    RESULT_CONTROL_PUP_4_BIT_0_REG, RESULT_CONTROL_PUP_4_BIT_1_REG, RESULT_CONTROL_PUP_4_BIT_2_REG, RESULT_CONTROL_PUP_4_BIT_3_REG,
    RESULT_CONTROL_PUP_4_BIT_4_REG, RESULT_CONTROL_PUP_4_BIT_5_REG, RESULT_CONTROL_PUP_4_BIT_6_REG, RESULT_CONTROL_PUP_4_BIT_7_REG,
};

GT_U16   maskResultsPupRegMap[] =
{
    RESULT_CONTROL_BYTE_PUP_0_REG, RESULT_CONTROL_BYTE_PUP_1_REG, RESULT_CONTROL_BYTE_PUP_2_REG, RESULT_CONTROL_BYTE_PUP_3_REG, RESULT_CONTROL_BYTE_PUP_4_REG
};

GT_U16 maskResultsDqRegMapPup3ECC[] =
{
    RESULT_CONTROL_PUP_0_BIT_0_REG, RESULT_CONTROL_PUP_0_BIT_1_REG, RESULT_CONTROL_PUP_0_BIT_2_REG, RESULT_CONTROL_PUP_0_BIT_3_REG,
    RESULT_CONTROL_PUP_0_BIT_4_REG, RESULT_CONTROL_PUP_0_BIT_5_REG, RESULT_CONTROL_PUP_0_BIT_6_REG, RESULT_CONTROL_PUP_0_BIT_7_REG,
    RESULT_CONTROL_PUP_1_BIT_0_REG, RESULT_CONTROL_PUP_1_BIT_1_REG, RESULT_CONTROL_PUP_1_BIT_2_REG, RESULT_CONTROL_PUP_1_BIT_3_REG,
    RESULT_CONTROL_PUP_1_BIT_4_REG, RESULT_CONTROL_PUP_1_BIT_5_REG, RESULT_CONTROL_PUP_1_BIT_6_REG, RESULT_CONTROL_PUP_1_BIT_7_REG,
    RESULT_CONTROL_PUP_2_BIT_0_REG, RESULT_CONTROL_PUP_2_BIT_1_REG, RESULT_CONTROL_PUP_2_BIT_2_REG, RESULT_CONTROL_PUP_2_BIT_3_REG,
    RESULT_CONTROL_PUP_2_BIT_4_REG, RESULT_CONTROL_PUP_2_BIT_5_REG, RESULT_CONTROL_PUP_2_BIT_6_REG, RESULT_CONTROL_PUP_2_BIT_7_REG,
    RESULT_CONTROL_PUP_4_BIT_0_REG, RESULT_CONTROL_PUP_4_BIT_1_REG, RESULT_CONTROL_PUP_4_BIT_2_REG, RESULT_CONTROL_PUP_4_BIT_3_REG,
    RESULT_CONTROL_PUP_4_BIT_4_REG, RESULT_CONTROL_PUP_4_BIT_5_REG, RESULT_CONTROL_PUP_4_BIT_6_REG, RESULT_CONTROL_PUP_4_BIT_7_REG,
    RESULT_CONTROL_PUP_3_BIT_0_REG, RESULT_CONTROL_PUP_3_BIT_1_REG, RESULT_CONTROL_PUP_3_BIT_2_REG, RESULT_CONTROL_PUP_3_BIT_3_REG,
    RESULT_CONTROL_PUP_3_BIT_4_REG, RESULT_CONTROL_PUP_3_BIT_5_REG, RESULT_CONTROL_PUP_3_BIT_6_REG, RESULT_CONTROL_PUP_3_BIT_7_REG,
};

GT_U16   maskResultsPupRegMapPup3ECC[] =
{
    RESULT_CONTROL_BYTE_PUP_0_REG, RESULT_CONTROL_BYTE_PUP_1_REG, RESULT_CONTROL_BYTE_PUP_2_REG, RESULT_CONTROL_BYTE_PUP_4_REG, RESULT_CONTROL_BYTE_PUP_4_REG
};

#ifdef CONFIG_DDR3
PatternInfo patternTable_16[] =
{
/* num tx phases  tx burst   delay between   rx pattern start_address   patternLen*/
   {0x1,	1,	2,  1, 0x0080,  2 },	/* PATTERN_PBS1*/
   {0x1,	1,	2,  1, 0x00C0,  2 },	/* PATTERN_PBS2*/
   {0x1,	1,	2,  1, 0x0380,  2 },	/* PATTERN_PBS3*/
   {0x1,	1,	2,  1, 0x0040,  2 },	/* PATTERN_TEST*/
   {0x1,	1,	2,  1, 0x0100,  2 },	/* PATTERN_RL*/
   {0x1,	1,	2,  1, 0x0000,  2 },	/* PATTERN_RL2*/
   {0xf,	7,  2,	7, 0x0140, 16  },	/* PATTERN_STATIC_PBS*/
   {0xf,	7,  2,	7, 0x0190, 16  },	/* PATTERN_KILLER_DQ0*/
   {0xf,  	7,  2,	7, 0x01D0, 16  },	/* PATTERN_KILLER_DQ1*/
   {0xf,  	7,  2,	7, 0x0210, 16  },	/* PATTERN_KILLER_DQ2*/
   {0xf,  	7,  2,	7, 0x0250, 16  },	/* PATTERN_KILLER_DQ3*/
   {0xf,  	7,  2,	7, 0x0290, 16  },	/* PATTERN_KILLER_DQ4*/
   {0xf,  	7,  2,	7, 0x02D0, 16  },	/* PATTERN_KILLER_DQ5*/
   {0xf,  	7,  2,	7, 0x0310, 16  },	/* PATTERN_KILLER_DQ6*/
   {0xf,  	7,  2,	7, 0x0350, 16  },	/* PATTERN_KILLER_DQ7*/
   {0xf,  	7,  2,	7, 0x04C0, 16  },	/* PATTERN_VREF*/
   {0xf,  	7,  2,	7, 0x03C0, 16  },	/* PATTERN_FULL_SSO_1T*/
   {0xf,  	7,  2,	7, 0x0400, 16  },	/* PATTERN_FULL_SSO_2T*/
   {0xf,  	7,  2,	7, 0x0440, 16  },	/* PATTERN_FULL_SSO_3T*/
   {0xf,  	7,  2,	7, 0x0480, 16  },	/* PATTERN_FULL_SSO_4T*/
   {0xf,  	7,  2,	7, 0x6280, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ0*/
   {0xf,  	7,  2,	7, 0x6680, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ1*/
   {0xf,  	7,  2,	7, 0x6A80, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ2*/
   {0xf,  	7,  2,	7, 0x6E80, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ3*/
   {0xf,  	7,  2,	7, 0x7280, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ4*/
   {0xf,  	7,  2,	7, 0x7680, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ5*/
   {0xf,  	7,  2,	7, 0x7A80, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ6*/
   {0xf,  	7,  2,	7, 0x7E80, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ7*/
   {0xf,  	7,  2,	7, 0x8280, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ0*/
   {0xf,  	7,  2,	7, 0x8680, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ1*/
   {0xf,  	7,  2,	7, 0x8A80, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ2*/
   {0xf,  	7,  2,	7, 0x8E80, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ3*/
   {0xf,  	7,  2,	7, 0x9280, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ4*/
   {0xf,  	7,  2,	7, 0x9680, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ5*/
   {0xf,  	7,  2,	7, 0x9A80, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ6*/
   {0xf,  	7,  2,	7, 0x9E80, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ7*/
   {0xf,  	7,  2,	7, 0xA280, 16  } /* PATTERN_ISI_XTALK_FREE*/
/*Note: actual start_address is <<3 of defined addess*/
};

PatternInfo patternTable_32[] =
{
/* num tx phases  tx burst   delay between   rx pattern start_address   patternLen*/
   {0x3,	0x3,  2,  0x3, 0x0080,  4 },	/* PATTERN_PBS1*/
   {0x3,	0x3,  2,  0x3, 0x00C0,  4 },	/* PATTERN_PBS2*/
   {0x3,	0x3,  2,  0x3, 0x0380,  4 },	/* PATTERN_PBS3*/
   {0x3,	0x3,  2,  0x3, 0x0040,  4 },	/* PATTERN_TEST*/
   {0x3,	0x3,  2,  0x3, 0x0100,  4 },	/* PATTERN_RL*/
   {0x3,	0x3,  2,  0x3, 0x0000,  4 },	/* PATTERN_RL2*/
   {0x1f,	0xF,  2,  0xf, 0x0140, 32  },	/* PATTERN_STATIC_PBS*/
   {0x1f,	0xF,  2,  0xf, 0x0190, 32  },	/* PATTERN_KILLER_DQ0*/
   {0x1f,  	0xF,  2,  0xf, 0x01D0, 32  },	/* PATTERN_KILLER_DQ1*/
   {0x1f,  	0xF,  2,  0xf, 0x0210, 32  },	/* PATTERN_KILLER_DQ2*/
   {0x1f,  	0xF,  2,  0xf, 0x0250, 32  },	/* PATTERN_KILLER_DQ3*/
   {0x1f,  	0xF,  2,  0xf, 0x0290, 32  },	/* PATTERN_KILLER_DQ4*/
   {0x1f,  	0xF,  2,  0xf, 0x02D0, 32  },	/* PATTERN_KILLER_DQ5*/
   {0x1f,  	0xF,  2,  0xf, 0x0310, 32  },	/* PATTERN_KILLER_DQ6*/
   {0x1f,  	0xF,  2,  0xf, 0x0350, 32  },	/* PATTERN_KILLER_DQ7*/
   {0x1f,  	0xF,  2,  0xf, 0x04C0, 32  },	/* PATTERN_VREF*/
   {0x1f,  	0xF,  2,  0xf, 0x03C0, 32  },	/* PATTERN_FULL_SSO_1T*/
   {0x1f,  	0xF,  2,  0xf, 0x0400, 32  },	/* PATTERN_FULL_SSO_2T*/
   {0x1f,  	0xF,  2,  0xf, 0x0440, 32  },	/* PATTERN_FULL_SSO_3T*/
   {0x1f,  	0xF,  2,  0xf, 0x0480, 32  },	/* PATTERN_FULL_SSO_4T*/
   {0x1f,  	0xF,  2,  0xf, 0x6280, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ0*/
   {0x1f,  	0xF,  2,  0xf, 0x6680, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ1*/
   {0x1f,  	0xF,  2,  0xf, 0x6A80, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ2*/
   {0x1f,  	0xF,  2,  0xf, 0x6E80, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ3*/
   {0x1f,  	0xF,  2,  0xf, 0x7280, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ4*/
   {0x1f,  	0xF,  2,  0xf, 0x7680, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ5*/
   {0x1f,  	0xF,  2,  0xf, 0x7A80, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ6*/
   {0x1f,  	0xF,  2,  0xf, 0x7E80, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ7*/
   {0x1f,  	0xF,  2,  0xf, 0x8280, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ0*/
   {0x1f,  	0xF,  2,  0xf, 0x8680, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ1*/
   {0x1f,  	0xF,  2,  0xf, 0x8A80, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ2*/
   {0x1f,  	0xF,  2,  0xf, 0x8E80, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ3*/
   {0x1f,  	0xF,  2,  0xf, 0x9280, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ4*/
   {0x1f,  	0xF,  2,  0xf, 0x9680, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ5*/
   {0x1f,  	0xF,  2,  0xf, 0x9A80, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ6*/
   {0x1f,  	0xF,  2,  0xf, 0x9E80, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ7*/
   {0x1f,  	0xF,  2,  0xf, 0xA280, 32  } /* PATTERN_ISI_XTALK_FREE*/


/*Note: actual start_address is <<3 of defined addess*/
};
#else
PatternInfo patternTable_16[] =
{
/* num tx phases  tx burst   delay between   rx pattern start_address   patternLen*/
   {0x1, 	0x1,	2,  0x1, 0x0000,  2  }, /* PATTERN_PBS1*/
   {0x1, 	0x1,	2,  0x1, 0x0080,  2  }, /* PATTERN_PBS2*/
   {0x1, 	0x1,	2,  0x1, 0x0100,  2  }, /* PATTERN_PBS3*/
   {0x1, 	0x1,	2,  0x1, 0x0180,  2  }, /* PATTERN_TEST*/
   {0x1, 	0x1,	2,  0x1, 0x0200,  2  }, /* PATTERN_RL*/
   {0x1, 	0x1,	2,  0x1, 0x0280,  2  }, /* PATTERN_RL2*/
   {0xf,  	0x7,    2,	0x7, 0x0680, 16  }, /* PATTERN_STATIC_PBS*/
   {0xf,  	0x7,    2,	0x7, 0x0A80, 16  }, /* PATTERN_KILLER_DQ0*/
   {0xf,  	0x7,    2,	0x7, 0x0E80, 16  }, /* PATTERN_KILLER_DQ1*/
   {0xf,  	0x7,    2,	0x7, 0x1280, 16  }, /* PATTERN_KILLER_DQ2*/
   {0xf,  	0x7,    2,	0x7, 0x1680, 16  }, /* PATTERN_KILLER_DQ3*/
   {0xf,  	0x7,    2,	0x7, 0x1A80, 16  }, /* PATTERN_KILLER_DQ4*/
   {0xf,  	0x7,    2,	0x7, 0x1E80, 16  }, /* PATTERN_KILLER_DQ5*/
   {0xf,  	0x7,    2,	0x7, 0x2280, 16  }, /* PATTERN_KILLER_DQ6*/
   {0xf,  	0x7,    2,	0x7, 0x2680, 16  }, /* PATTERN_KILLER_DQ7*/
   {0xf,  	0x7,    2,	0x7, 0x2A80, 16  }, /* PATTERN_KILLER_DQ0_INV*/
   {0xf,  	0x7,    2,	0x7, 0x2E80, 16  }, /* PATTERN_KILLER_DQ1_INV*/
   {0xf,  	0x7,    2,	0x7, 0x3280, 16  }, /* PATTERN_KILLER_DQ2_INV*/
   {0xf,  	0x7,    2,	0x7, 0x3680, 16  }, /* PATTERN_KILLER_DQ3_INV*/
   {0xf,  	0x7,    2,	0x7, 0x3A80, 16  }, /* PATTERN_KILLER_DQ4_INV*/
   {0xf,  	0x7,    2,	0x7, 0x3E80, 16  }, /* PATTERN_KILLER_DQ5_INV*/
   {0xf,  	0x7,    2,	0x7, 0x4280, 16  }, /* PATTERN_KILLER_DQ6_INV*/
   {0xf,  	0x7,    2,	0x7, 0x4680, 16  }, /* PATTERN_KILLER_DQ7_INV*/
   {0xf,  	0x7,    2,	0x7, 0x4A80, 16  }, /* PATTERN_VREF*/
   {0xf,  	0x7,    2,	0x7, 0x4E80, 16  }, /* PATTERN_VREF_INV*/
   {0xf,  	0x7,    2,	0x7, 0x5280, 16  }, /* PATTERN_FULL_SSO_0T*/
   {0xf,  	0x7,    2,	0x7, 0x5680, 16  }, /* PATTERN_FULL_SSO_1T*/
   {0xf,  	0x7,    2,	0x7, 0x5A80, 16  }, /* PATTERN_FULL_SSO_2T*/
   {0xf,  	0x7,    2,	0x7, 0x5E80, 16  }, /* PATTERN_FULL_SSO_3T*/
   {0xf,  	0x7,    2,	0x7, 0x6280, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ0*/
   {0xf,  	0x7,    2,	0x7, 0x6680, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ1*/
   {0xf,  	0x7,    2,	0x7, 0x6A80, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ2*/
   {0xf,  	0x7,    2,	0x7, 0x6E80, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ3*/
   {0xf,  	0x7,    2,	0x7, 0x7280, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ4*/
   {0xf,  	0x7,    2,	0x7, 0x7680, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ5*/
   {0xf,  	0x7,    2,	0x7, 0x7A80, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ6*/
   {0xf,  	0x7,    2,	0x7, 0x7E80, 16  }, /* PATTERN_SSO_FULL_XTALK_DQ7*/
   {0xf,  	0x7,    2,	0x7, 0x8280, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ0*/
   {0xf,  	0x7,    2,	0x7, 0x8680, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ1*/
   {0xf,  	0x7,    2,	0x7, 0x8A80, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ2*/
   {0xf,  	0x7,    2,	0x7, 0x8E80, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ3*/
   {0xf,  	0x7,    2,	0x7, 0x9280, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ4*/
   {0xf,  	0x7,    2,	0x7, 0x9680, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ5*/
   {0xf,  	0x7,    2,	0x7, 0x9A80, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ6*/
   {0xf,  	0x7,    2,	0x7, 0x9E80, 16  }, /* PATTERN_SSO_XTALK_FREE_DQ7*/
   {0xf,  	0x7,    2,	0x7, 0xA280, 16  }, /* PATTERN_ISI_XTALK_FREE*/
   {0xf,  	0x7,    2,	0x7, 0xA680, 16  }, /* PATTERN_RESONANCE_1T*/
   {0xf,  	0x7,    2,	0x7, 0xAA80, 16  }, /* PATTERN_RESONANCE_2T*/
   {0xf,  	0x7,    2,	0x7, 0xAE80, 16  }, /* PATTERN_RESONANCE_3T*/
   {0xf,  	0x7,    2,	0x7, 0xB280, 16  }, /* PATTERN_RESONANCE_4T*/
   {0xf,  	0x7,    2,	0x7, 0xB680, 16  }, /* PATTERN_RESONANCE_5T*/
   {0xf,  	0x7,    2,	0x7, 0xBA80, 16  }, /* PATTERN_RESONANCE_6T*/
   {0xf,  	0x7,    2,	0x7, 0xBE80, 16  }, /* PATTERN_RESONANCE_7T*/
   {0xf,  	0x7,    2,	0x7, 0xC280, 16  }, /* PATTERN_RESONANCE_8T*/
   {0xf,  	0x7,    2,	0x7, 0xC680, 16  }  /* PATTERN_RESONANCE_9T*/
/*Note: actual start_address is <<3 of defined addess*/
};

PatternInfo patternTable_32[] =
{
/* num tx phases  tx burst   delay between   rx pattern start_address   patternLen*/
   {0x3, 	0x3,	2,  0x3, 0x0000,  4  }, /* PATTERN_PBS1*/
   {0x3, 	0x3,	2,  0x3, 0x0080,  4  }, /* PATTERN_PBS2*/
   {0x3, 	0x3,	2,  0x3, 0x0100,  4  }, /* PATTERN_PBS3*/
   {0x3, 	0x3,	2,  0x3, 0x0180,  4  }, /* PATTERN_TEST*/
   {0x3, 	0x3,	2,  0x3, 0x0200,  4  }, /* PATTERN_RL*/
   {0x3, 	0x3,	2,  0x3, 0x0280,  4  }, /* PATTERN_RL2*/
   {0x1f,  	0xf,    2,	0xf, 0x0680, 32  }, /* PATTERN_STATIC_PBS*/
   {0x1f,  	0xf,    2,	0xf, 0x0A80, 32  }, /* PATTERN_KILLER_DQ0*/
   {0x1f,  	0xf,    2,	0xf, 0x0E80, 32  }, /* PATTERN_KILLER_DQ1*/
   {0x1f,  	0xf,    2,	0xf, 0x1280, 32  }, /* PATTERN_KILLER_DQ2*/
   {0x1f,  	0xf,    2,	0xf, 0x1680, 32  }, /* PATTERN_KILLER_DQ3*/
   {0x1f,  	0xf,    2,	0xf, 0x1A80, 32  }, /* PATTERN_KILLER_DQ4*/
   {0x1f,  	0xf,    2,	0xf, 0x1E80, 32  }, /* PATTERN_KILLER_DQ5*/
   {0x1f,  	0xf,    2,	0xf, 0x2280, 32  }, /* PATTERN_KILLER_DQ6*/
   {0x1f,  	0xf,    2,	0xf, 0x2680, 32  }, /* PATTERN_KILLER_DQ7*/
   {0x1f,  	0xf,    2,	0xf, 0x2A80, 32  }, /* PATTERN_KILLER_DQ0_INV*/
   {0x1f,  	0xf,    2,	0xf, 0x2E80, 32  }, /* PATTERN_KILLER_DQ1_INV*/
   {0x1f,  	0xf,    2,	0xf, 0x3280, 32  }, /* PATTERN_KILLER_DQ2_INV*/
   {0x1f,  	0xf,    2,	0xf, 0x3680, 32  }, /* PATTERN_KILLER_DQ3_INV*/
   {0x1f,  	0xf,    2,	0xf, 0x3A80, 32  }, /* PATTERN_KILLER_DQ4_INV*/
   {0x1f,  	0xf,    2,	0xf, 0x3E80, 32  }, /* PATTERN_KILLER_DQ5_INV*/
   {0x1f,  	0xf,    2,	0xf, 0x4280, 32  }, /* PATTERN_KILLER_DQ6_INV*/
   {0x1f,  	0xf,    2,	0xf, 0x4680, 32  }, /* PATTERN_KILLER_DQ7_INV*/
   {0x1f,  	0xf,    2,	0xf, 0x4A80, 32  }, /* PATTERN_VREF*/
   {0x1f,  	0xf,    2,	0xf, 0x4E80, 32  }, /* PATTERN_VREF_INV*/
   {0x1f,  	0xf,    2,	0xf, 0x5280, 32  }, /* PATTERN_FULL_SSO_0T*/
   {0x1f,  	0xf,    2,	0xf, 0x5680, 32  }, /* PATTERN_FULL_SSO_1T*/
   {0x1f,  	0xf,    2,	0xf, 0x5A80, 32  }, /* PATTERN_FULL_SSO_2T*/
   {0x1f,  	0xf,    2,	0xf, 0x5E80, 32  }, /* PATTERN_FULL_SSO_3T*/
   {0x1f,  	0xf,    2,	0xf, 0x6280, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ0*/
   {0x1f,  	0xf,    2,	0xf, 0x6680, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ1*/
   {0x1f,  	0xf,    2,	0xf, 0x6A80, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ2*/
   {0x1f,  	0xf,    2,	0xf, 0x6E80, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ3*/
   {0x1f,  	0xf,    2,	0xf, 0x7280, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ4*/
   {0x1f,  	0xf,    2,	0xf, 0x7680, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ5*/
   {0x1f,  	0xf,    2,	0xf, 0x7A80, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ6*/
   {0x1f,  	0xf,    2,	0xf, 0x7E80, 32  }, /* PATTERN_SSO_FULL_XTALK_DQ7*/
   {0x1f,  	0xf,    2,	0xf, 0x8280, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ0*/
   {0x1f,  	0xf,    2,	0xf, 0x8680, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ1*/
   {0x1f,  	0xf,    2,	0xf, 0x8A80, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ2*/
   {0x1f,  	0xf,    2,	0xf, 0x8E80, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ3*/
   {0x1f,  	0xf,    2,	0xf, 0x9280, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ4*/
   {0x1f,  	0xf,    2,	0xf, 0x9680, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ5*/
   {0x1f,  	0xf,    2,	0xf, 0x9A80, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ6*/
   {0x1f,  	0xf,    2,	0xf, 0x9E80, 32  }, /* PATTERN_SSO_XTALK_FREE_DQ7*/
   {0x1f,  	0xf,    2,	0xf, 0xA280, 32  }, /* PATTERN_ISI_XTALK_FREE*/
   {0x1f,  	0xf,    2,	0xf, 0xA680, 32  }, /* PATTERN_RESONANCE_1T*/
   {0x1f,  	0xf,    2,	0xf, 0xAA80, 32  }, /* PATTERN_RESONANCE_2T*/
   {0x1f,  	0xf,    2,	0xf, 0xAE80, 32  }, /* PATTERN_RESONANCE_3T*/
   {0x1f,  	0xf,    2,	0xf, 0xB280, 32  }, /* PATTERN_RESONANCE_4T*/
   {0x1f,  	0xf,    2,	0xf, 0xB680, 32  }, /* PATTERN_RESONANCE_5T*/
   {0x1f,  	0xf,    2,	0xf, 0xBA80, 32  }, /* PATTERN_RESONANCE_6T*/
   {0x1f,  	0xf,    2,	0xf, 0xBE80, 32  }, /* PATTERN_RESONANCE_7T*/
   {0x1f,  	0xf,    2,	0xf, 0xC280, 32  }, /* PATTERN_RESONANCE_8T*/
   {0x1f,  	0xf,    2,	0xf, 0xC680, 32  }  /* PATTERN_RESONANCE_9T*/
/*Note: actual start_address is <<3 of defined addess*/
};

#endif

/*******************************************************************************/
GT_U32                      trainDevNum;
MV_HWS_DDR_CS               traintrainCsType;
GT_U32                      trainPupNum;
MV_HWS_TRAINING_RESULT      trainResultType;
MV_HWS_ControlElement       trainControlElement;
MV_HWS_SearchDirection      traineSearchDir;
MV_HWS_DIRECTION            trainDirection;
GT_U32                      trainIfSelect;
GT_U32                      trainInitValue;
GT_U32                      trainNumberIterations;
MV_HWS_PATTERN              trainPattern;
MV_HWS_EdgeCompare          trainEdgeCompare;
GT_U32                      trainCsNum;
GT_U32                      trainIfAcess, trainIfId, trainPupAccess;
#ifdef CONFIG_DDR4
GT_U32                      maxPollingForDone = 100000000;  /* this counter was increased for DDR4
                                                               due to A390 DB-GP DDR4 failure */
#else /* DDR3 */

GT_U32                      maxPollingForDone = 1000000;

#endif /* CONFIG_DDR4 */

extern MV_HWS_RESULT trainingResult[MAX_STAGE_LIMIT][MAX_INTERFACE_NUM];
extern AUTO_TUNE_STAGE trainingStage;



/******************************************************************************/

GT_U32* ddr3TipGetBufPtr
(
    GT_U32                 devNum,
    MV_HWS_SearchDirection search,
    MV_HWS_TRAINING_RESULT resultType,
    GT_U32                 interfaceNum
)
{
    GT_U32 *bufPtr = NULL;

	/* avoid warnings */
	devNum = devNum;
	resultType = resultType;

    bufPtr = &trainingRes[MAX_INTERFACE_NUM * MAX_BUS_NUM * BUS_WIDTH_IN_BITS* search +
                               interfaceNum *MAX_BUS_NUM * BUS_WIDTH_IN_BITS];
    return bufPtr;
}

/*****************************************************************************
IP Training search
Note: for one edge search only from fail to pass, else jitter can be be entered into solution.
******************************************************************************/
GT_STATUS    ddr3TipIpTraining
(
    GT_U32                      devNum,
    MV_HWS_ACCESS_TYPE          accessType,
    GT_U32                      interfaceNum, 
    MV_HWS_ACCESS_TYPE          pupAccessType,
    GT_U32                      pupNum, 
    MV_HWS_TRAINING_RESULT      resultType,
    MV_HWS_ControlElement       controlElement,
    MV_HWS_SearchDirection      searchDir,
    MV_HWS_DIRECTION            direction,
    GT_U32                      interfaceMask, 
    GT_U32                      initValue,
    GT_U32                      numIter,
    MV_HWS_PATTERN              pattern,
    MV_HWS_EdgeCompare          edgeComp,
    MV_HWS_DDR_CS               csType,
    GT_U32                      csNum,
    MV_HWS_TrainingIpStatus*    trainStatus
)
{
    
    GT_U32 maskDqNumOfRegs, maskPupNumOfRegs, indexCnt,pollCnt, regData, pupId, triggerRegAddr;
    GT_U32 txBurstSize;
    GT_U32 delayBetweenBurst;
    GT_U32 rdMode;
    GT_U32 readData[MAX_INTERFACE_NUM];
	PatternInfo *patternTable = ddr3TipGetPatternTable();
	GT_U16 *maskResultsPupRegMap = ddr3TipGetMaskResultsPupRegMap();
	GT_U16 *maskResultsDqRegMap 	= ddr3TipGetMaskResultsDqReg();
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    if (pupNum >= octetsPerInterfaceNum)
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("pupNum %d not valid\n",pupNum));
    }
    if (interfaceNum >= MAX_INTERFACE_NUM)
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("interfaceId %d not valid\n",interfaceNum));
    }
    if (trainStatus == NULL)
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("error param 4\n"));
        return GT_BAD_PARAM;
    }

    /* load pattern */
    if (csType == CS_SINGLE)  
    {
        /* All CS\92s to CS0     */
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, CS_ENABLE_REG, 1<<3, 1<<3));
        /* All CS\92s to CS0     */
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, ODPG_DATA_CONTROL_REG, (0x3 | (effective_cs<<26)) , 0xC000003));
     }
    else
    {
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, CS_ENABLE_REG, 0, 1<<3));
        /*  CS select*/
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, ODPG_DATA_CONTROL_REG, 0x3 | csNum<<26, 0x3 | 3<<26));
    }
    /* LOAD PATTERN TO ODPG */
    /* load pattern to ODPG */

    ddr3TipLoadPatternToOdpg(devNum, accessType, interfaceNum, pattern, patternTable[pattern].startAddr);
    txBurstSize = (direction == OPER_WRITE) ? patternTable[pattern].txBurstSize : 0;
    delayBetweenBurst = (direction == OPER_WRITE) ? 2 : 0;
    rdMode = (direction == OPER_WRITE) ? 1 : 0;
    CHECK_STATUS(ddr3TipConfigureOdpg(devNum, accessType, interfaceNum, direction, patternTable[pattern].numOfPhasesTx,
                                        txBurstSize, patternTable[pattern].numOfPhasesRx, delayBetweenBurst, 
                                        rdMode,  effective_cs, STRESS_NONE, DURATION_SINGLE));
    regData = (direction == OPER_READ) ?   0  : (0x3<<30) ;
    regData |= (direction == OPER_READ) ?  0x60  : 0xFA ;
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, ODPG_WRITE_READ_MODE_ENABLE_REG, regData, MASK_ALL_BITS));
    regData = (edgeComp == EDGE_PF || edgeComp == EDGE_FP) ? 0 : 1<<6;
    regData |= (edgeComp == EDGE_PF || edgeComp == EDGE_PFP) ? (1<<7) : 0;
    /* change from Pass to Fail will lock the result*/
#if 0
    if (csType == CS_SINGLE)  
    {
        /* Write2Dunit(0x1034, 0xe , [17:14]) */ /* All PUPs selected */
        regData |= 0xe<<14;
    }
    else
    {
        /* Write2Dunit(0x1034, PUP# , [17:14]) */  /* PUP select */
       regData |= pupNum<<14;
    }
#else
    if (pupAccessType == ACCESS_TYPE_MULTICAST)
    {
        regData |= 0xe<<14;
    }
    else
    {
        regData |= pupNum<<14;
    }
#endif

    if (edgeComp == EDGE_FP)
    {
        regData |= (0<<20); /* don't search for readl edge change, only the state */
    }
    else if (edgeComp == EDGE_FPF)
    {
        regData |= (0<<20);
    }
    else
    {
        regData |= (3<<20);
    }

/*    regData |= (0x7<<8);
    if (edgeComp == EDGE_FP)
        regData |= (0x7<<11);
        */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, ODPG_TRAINING_CONTROL_REG, regData | (0x7<<8) | (0x7<<11), (0x3 | (0x3<<2) | (0x3<<6) | (1<<5) | (0x7<<8) | (0x7<<11) | (0xf<<14) | (0x3<<18) | (3<<20))));
    regData = (searchDir == MV_HWS_Low2High) ? 0 : (1 << 8);
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, ODPG_OBJ1_OPCODE_REG, 1 | regData | initValue<<9 |  (1<<25) | (1<<26), 0xFF | (1<<8) | (0xFFFF<<9) |  (1<<25) | (1<<26)));
    /*Write2Dunit(0x10B4, NumberIteration , [15:0])   Max number of iterations */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, ODPG_OBJ1_ITER_CNT_REG, numIter, 0xFFFF));
    if (controlElement == MV_HWS_ControlElement_DQ_SKEW && direction == OPER_READ)
    {
        /*Write2Dunit(0x10C0, 0x5F , [7:0])   MC PBS Reg Address at DDR PHY*/
        regData = 0x5f + effective_cs * CALIBRATED_OBJECTS_REG_ADDR_OFFSET;
    }
    else if (controlElement == MV_HWS_ControlElement_DQ_SKEW && direction == OPER_WRITE)
    {
       regData = 0x1f + effective_cs * CALIBRATED_OBJECTS_REG_ADDR_OFFSET;
    }
    else if (controlElement == MV_HWS_ControlElement_ADLL && direction == OPER_WRITE)
    {
		/*LOOP         0x00000001 + 4*n: where n (0-3) represents M_CS number */
		/*Write2Dunit(0x10C0, 0x1 , [7:0])    ADLL WR Reg Address at DDR PHY*/
		regData = 1 + effective_cs * CS_REGISTER_ADDR_OFFSET;
    }
    else if (controlElement == MV_HWS_ControlElement_ADLL && direction == OPER_READ)
    {
        /* ADLL RD Reg Address at DDR PHY*/
        regData = 3 + effective_cs * CS_REGISTER_ADDR_OFFSET;
    }
    else if (controlElement == MV_HWS_ControlElement_DQS_SKEW && direction == OPER_WRITE)
    {
        /*TBD not defined in 0.5.0 requirement  */
    }
    else if (controlElement == MV_HWS_ControlElement_DQS_SKEW && direction == OPER_READ)
    {
        /*TBD not defined in 0.5.0 requirement */
    }
    regData |= (0x6 << 28);
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, CALIB_OBJ_PRFA_REG, regData | (initValue << 8) , 0xff | (0xFFFF << 8)| (0xF << 24) | (GT_U32)(0xF << 28)));

	maskDqNumOfRegs = octetsPerInterfaceNum * BUS_WIDTH_IN_BITS;
	maskPupNumOfRegs = octetsPerInterfaceNum;

    if (resultType == RESULT_PER_BIT)
    {
        for(indexCnt=0; indexCnt<maskDqNumOfRegs;indexCnt++)
        {
            CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, maskResultsDqRegMap[indexCnt], 0, 1<<24));
        }

		/*Mask disabled buses*/
        for(pupId = 0; pupId < octetsPerInterfaceNum ; pupId++)
        {
			if (IS_BUS_ACTIVE(topologyMap->activeBusMask, pupId) == GT_TRUE) continue;

		    for(indexCnt=pupId*8; indexCnt<(pupId+1)*8;indexCnt++)
		    {
		        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, maskResultsDqRegMap[indexCnt], (1<<24), 1<<24));
		    }
		}

        for(indexCnt=0; indexCnt<maskPupNumOfRegs;indexCnt++)
        {
            CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, maskResultsPupRegMap[indexCnt], (1<<24) , 1<<24));
        }
    }
    else if (resultType == RESULT_PER_BYTE)
    {
        /* write to adll */
        for(indexCnt=0; indexCnt<maskPupNumOfRegs;indexCnt++)
        {
            CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, maskResultsPupRegMap[indexCnt], 0, 1<<24));
        }
        for(indexCnt=0; indexCnt<maskDqNumOfRegs;indexCnt++)
        {
            CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, maskResultsDqRegMap[indexCnt],(1 << 24), (1<<24)));
        }
    }

    /*Start Training Trigger */
	triggerRegAddr = (ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3)?(ODPG_TRAINING_TRIGGER_REG):(ODPG_TRAINING_STATUS_REG);
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceNum, triggerRegAddr, 1, 1));

    /*wait for all RFU tests to finish (or timeout)*/
	/*WA for 16 bit mode, more investigation needed*/
    hwsOsExactDelayPtr((GT_U8)devNum, devNum, 1); /* 1 mSec */

    /* Training "Done ?"  for CPU contolled TIP*/
    if(ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3)
    {
		for(indexCnt=0; indexCnt < MAX_INTERFACE_NUM;indexCnt++)
		{
		    if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, indexCnt) ==  0)
		    {
		        continue;
		    }
		    if (interfaceMask & (1<<indexCnt))
		    {
		        /*need to check results for this Dunit */
		        for(pollCnt=0;pollCnt < maxPollingForDone;pollCnt++)
		        {
		            CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, indexCnt, ODPG_TRAINING_STATUS_REG, readData, MASK_ALL_BITS));
		            if ((readData[indexCnt] & 0x2) != 0)
		            {
		                /*done */
						trainStatus[indexCnt] = MV_HWS_TrainingIpStatus_SUCCESS;
		                break;
		            }
		        }
		        if (pollCnt == maxPollingForDone)
		        {
		            trainStatus[indexCnt] = MV_HWS_TrainingIpStatus_TIMEOUT;
		        }
		    }
			/*Be sure that ODPG done*/
		    CHECK_STATUS(isOdpgAccessDone(devNum, indexCnt));
		}
		/*Write ODPG done in Dunit*/
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_STATUS_DONE_REG, 0, 0x1));
	}

    /*wait for all Dunit tests to finish (or timeout)*/
    /* Training "Done ?" */
    /* Training "Pass ?" */
    for(indexCnt=0; indexCnt < MAX_INTERFACE_NUM;indexCnt++)
    {
        if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, indexCnt) ==  0)
        {
            continue;
        }
        if (interfaceMask & (1<<indexCnt))
        {
            /*need to check results for this Dunit */
            for(pollCnt=0;pollCnt < maxPollingForDone;pollCnt++)
            {
                CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, indexCnt, triggerRegAddr, readData, MASK_ALL_BITS));
                regData = readData[indexCnt];
                 if ((regData & 0x2) != 0)
                {
                    /*done*/
                    if ((regData & 0x4) == 0)
                    {
                        trainStatus[indexCnt] = MV_HWS_TrainingIpStatus_SUCCESS;
                    }
                    else
                    {
						trainStatus[indexCnt] = MV_HWS_TrainingIpStatus_FAIL;
                    }
                    break;
                }
            }
            if (pollCnt == maxPollingForDone)
            {
                trainStatus[indexCnt] = MV_HWS_TrainingIpStatus_TIMEOUT;
            }
        }
    }
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, 0, MASK_ALL_BITS));
    return GT_OK;
}




/*****************************************************************************
Load expected Pattern to ODPG
******************************************************************************/
GT_STATUS    ddr3TipLoadPatternToOdpg
(
    GT_U32              devNum,
    MV_HWS_ACCESS_TYPE  accessType,
    GT_U32              interfaceId,
    MV_HWS_PATTERN      pattern,
    GT_U32              loadAddr
)
{
    GT_U32 patternLengthCnt = 0;
	PatternInfo *patternTable = ddr3TipGetPatternTable();

    for(patternLengthCnt = 0; patternLengthCnt < patternTable[pattern].patternLen ; patternLengthCnt++)
    {
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceId, ODPG_PATTERN_DATA_LOW_REG,
				patternTableGetWord(devNum, pattern, (GT_U8)(patternLengthCnt*2)), MASK_ALL_BITS));
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceId, ODPG_PATTERN_DATA_HI_REG,
				patternTableGetWord(devNum, pattern, (GT_U8)(patternLengthCnt*2 + 1)), MASK_ALL_BITS));
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceId, ODPG_PATTERN_ADDR_REG, patternLengthCnt, MASK_ALL_BITS));
    }

    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, accessType, interfaceId,  ODPG_PATTERN_ADDR_OFFSET_REG,  loadAddr, MASK_ALL_BITS));

    return GT_OK;
}

/*****************************************************************************
Configure ODPG
******************************************************************************/
GT_STATUS    ddr3TipConfigureOdpg
(   
    GT_U32                 devNum,
    MV_HWS_ACCESS_TYPE     accessType,
    GT_U32                 interfaceId,
    MV_HWS_DIRECTION       direction,
    GT_U32                 txPhases,
    GT_U32                 txBurstSize,
    GT_U32                 rxPhases,
    GT_U32                 delayBetweenBurst,
    GT_U32                 rdMode,
    GT_U32                 csNum,
    GT_U32                 addrStressJump,
    GT_U32                 singlePattern
)
{
   GT_U32 dataValue = 0;
   GT_STATUS retVal;

   direction = direction; /* avoid warnings */

   dataValue = ((singlePattern << 2) | (txPhases << 5) | (txBurstSize << 11) | (delayBetweenBurst << 15) | (rxPhases << 21) | (rdMode << 25)  | (csNum << 26) | (addrStressJump << 29));
   retVal = mvHwsDdr3TipIFWrite(devNum,  accessType, interfaceId,  ODPG_DATA_CONTROL_REG,  dataValue, 0xAFFFFFFC);
   if (retVal != GT_OK)
   {
      return retVal;
   }
   return GT_OK;
}


void    ddr3TipDDR3IpTrainingStart
(
)
{
    MV_HWS_TrainingIpStatus    sTrainIpStatus[MAX_INTERFACE_NUM];
    ddr3TipIpTraining(
    trainDevNum,
    trainIfAcess,
    trainIfId,
    trainPupAccess,
    trainPupNum, 
    trainResultType,
    trainControlElement,
    traineSearchDir,
    trainDirection,
    trainIfSelect, 
    trainInitValue,
    trainNumberIterations,
    trainPattern,
    trainEdgeCompare,
    traintrainCsType,
    trainCsNum,
    sTrainIpStatus);
}

GT_STATUS ddr3TipProcessResult(GT_U32 *arResult, MV_HWS_Edge eEdge, MV_HWS_EdgeSearch eEdgeSearch, GT_U32 *edgeResult)
{
    GT_U32 i, ResVal;
	GT_32 tapVal, maxVal = -10000, minVal = 10000;
    GT_BOOL lockSuccess = GT_TRUE;

    for(i=0; i < BUS_WIDTH_IN_BITS; i++)
    {
        ResVal = GET_LOCK_RESULT(arResult[i]);
        if (ResVal == 0)
        {
            lockSuccess = GT_FALSE;
            break;
        }
        {
            DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("lock failed for bit %d\n",i));
        }
    }
    if (lockSuccess == GT_TRUE)
    {
        for(i=0; i < BUS_WIDTH_IN_BITS; i++)
        {
            tapVal = GET_TAP_RESULT(arResult[i], eEdge);
            if (tapVal > maxVal)
                maxVal = tapVal;
            if (tapVal < minVal)
                minVal = tapVal;
            if (eEdgeSearch == TRAINING_EDGE_MAX)
                *edgeResult = (GT_U32)maxVal;
            else
                *edgeResult = (GT_U32)minVal;
            DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("i %d arResult[i] 0x%x tapVal %d maxVal %d minVal %d EdgeResult %d\n", i, arResult[i], tapVal, maxVal, minVal, *edgeResult));
        }
    }
    else
    {
        return GT_FAIL;
    }
    return GT_OK;
}

/*****************************************************************************
Read training search result
******************************************************************************/
GT_STATUS    ddr3TipReadTrainingResult
(
    GT_U32                          devNum,
    GT_U32                          interfaceId, 
    MV_HWS_ACCESS_TYPE              pupAccessType,
    GT_U32                          pupNum,
    GT_U32                          bitNum,
    MV_HWS_SearchDirection          search,
    MV_HWS_DIRECTION                direction,
    MV_HWS_TRAINING_RESULT          resultType,
    MV_HWS_TrainingLoadOperation    operation,
    GT_U32                          csNumType,
    GT_U32                          **loadRes,
    GT_BOOL                         isReadFromDB,
    GT_U8                         	consTap,
    GT_BOOL                         isCheckResultValidity
)
{
    GT_U32 regOffset,  pupCnt, startPup, endPup, startReg, endReg;    
    GT_U32  *interfaceTrainRes = NULL;
    GT_U16  *regAddr = NULL;
    GT_U32 readData[MAX_INTERFACE_NUM];
	GT_U16 *maskResultsPupRegMap = ddr3TipGetMaskResultsPupRegMap();
	GT_U16 *maskResultsDqRegMap 	= ddr3TipGetMaskResultsDqReg();
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    /* Agreed assumption: all CS mask contain same number of bits, i.e. in multi CS, the number of CS per memory is the same for all pups */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST,  interfaceId, CS_ENABLE_REG, (csNumType == 0) ? 1<<3 : 0, (1 << 3)));
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST,  interfaceId,  ODPG_DATA_CONTROL_REG, (csNumType << 26), (3 << 26)));
    DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_TRACE, ("ReadFromDB %d csType %d oper %d resultType %d direction %d search %d pupNum %d interfaceId %d pupAccessType %d\n",isReadFromDB, csNumType , operation , resultType , direction , search , pupNum , interfaceId,pupAccessType));
    if ((loadRes == NULL) && (isReadFromDB == GT_TRUE))
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("ddr3TipReadTrainingResult loadRes = NULL"));
        return GT_FAIL;
    }
    if (pupNum >= octetsPerInterfaceNum)
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("pupNum %d not valid\n",pupNum));
    }
    if (interfaceId >= MAX_INTERFACE_NUM)
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("interfaceId %d not valid\n",interfaceId));
    }
    if (resultType == RESULT_PER_BIT)
    {
        regAddr = maskResultsDqRegMap;
    }
    else 
    {
        regAddr = maskResultsPupRegMap;
    }
    if (pupAccessType == ACCESS_TYPE_UNICAST)
    {
        startPup = pupNum;
        endPup = pupNum;
    }
    else /*pupAccessType == ACCESS_TYPE_MULTICAST)*/
    {
        startPup = 0;
        endPup = octetsPerInterfaceNum-1;
    } 
    for(pupCnt = startPup; pupCnt <= endPup ; pupCnt++)
    {
       	VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pupCnt)
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_TRACE, ("interfaceId %d startPup %d endPup %d pupCnt %d\n",interfaceId, startPup, endPup, pupCnt));
        if (resultType == RESULT_PER_BIT)
        {
            if (bitNum == ALL_BITS_PER_PUP)
            {
                startReg = pupCnt*BUS_WIDTH_IN_BITS;
                endReg = (pupCnt+1)*BUS_WIDTH_IN_BITS - 1;
            }
            else
            {
                startReg = pupCnt*BUS_WIDTH_IN_BITS + bitNum;
                endReg = pupCnt*BUS_WIDTH_IN_BITS + bitNum;
            }

        }
        else
        {
            startReg = pupCnt;
            endReg = pupCnt;
        } 
        interfaceTrainRes = ddr3TipGetBufPtr(devNum, search,resultType,interfaceId);
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_TRACE, ("startReg %d endReg %d interface 0x%x\n",startReg, endReg, interfaceTrainRes));
        if (interfaceTrainRes == NULL)
        {
            DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("interfaceTrainRes is NULL\n"));
            return GT_FAIL;
        }
        for(regOffset=startReg; regOffset <= endReg; regOffset++)
        {
            if (operation == TrainingLoadOperation_UNLOAD)
            {
                if (isReadFromDB == GT_FALSE)
                {
                   CHECK_STATUS(mvHwsDdr3TipIFRead(devNum,  ACCESS_TYPE_UNICAST, interfaceId,
                                                regAddr[regOffset], readData  , MASK_ALL_BITS));
					if(isCheckResultValidity == GT_TRUE){
						if( (readData[interfaceId] & 0x02000000) == 0 ){
							interfaceTrainRes[regOffset] = 0x02000000 + 64 + consTap;
						}
						else{
							interfaceTrainRes[regOffset] = readData[interfaceId] + consTap;
						}
					}
					else{
						interfaceTrainRes[regOffset] = readData[interfaceId] + consTap;
					}
                    DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_TRACE, ("regOffset %d value 0x%x addr 0x%x\n",regOffset, interfaceTrainRes[regOffset], &interfaceTrainRes[regOffset]));
                }
                else
                {
                    *loadRes = &interfaceTrainRes[startReg];
                    DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_TRACE, ("*loadRes %d\n",*loadRes));
                }
            }
            else
            {
                DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_TRACE, ("not supported\n"));
            }
        }
    }
    return GT_OK;
}


/*****************************************************************************
load all pattern to memory using ODPG
******************************************************************************/
GT_STATUS    ddr3TipLoadAllPatternToMem
(
    GT_U32          devNum
)
{
    GT_U32  pattern = 0, interfaceId;


    /*DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_INFO, ("=== LOAD ALL PATTERNS ==="));*/

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)    
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        trainingResult[trainingStage][interfaceId] = TEST_SUCCESS;
    }
    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)    
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        /* enable single cs */
        CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, CS_ENABLE_REG, (1 << 3), (1 << 3)));
    }
    for(pattern = 0; pattern < PATTERN_LIMIT; pattern++)
    {
        ddr3TipLoadPatternToMem(devNum,pattern);
    }

    return GT_OK;
}

/*****************************************************************************
wait till ODPG access is ready
******************************************************************************/
GT_STATUS isOdpgAccessDone
(
    GT_U32 devNum,
    GT_U32 interfaceId
)
{
    GT_U32 pollCnt = 0, dataValue, expectedVal;
    GT_U32 readData[MAX_INTERFACE_NUM];

    for (pollCnt = 0; pollCnt < MAX_POLLING_ITERATIONS ; pollCnt++)
    {
        CHECK_STATUS(mvHwsDdr3TipIFRead(devNum,ACCESS_TYPE_UNICAST, interfaceId, ODPG_BIST_DONE, readData, MASK_ALL_BITS));
 		dataValue = readData[interfaceId];

		expectedVal =  (ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) < MV_TIP_REV_3)?(ODPG_BIST_DONE_BIT_VALUE_REV2):(ODPG_BIST_DONE_BIT_VALUE_REV3);
        if (((dataValue >> ODPG_BIST_DONE_BIT_OFFS )& 0x1) == expectedVal)
        {
            dataValue = dataValue & 0xfffffffe;
            CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, ODPG_BIST_DONE, dataValue, MASK_ALL_BITS));
            break;
        }
    }
    if (pollCnt >= MAX_POLLING_ITERATIONS)
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR,("Bist Activate: poll failure 2\n"));
        return GT_FAIL;
    }
    return GT_OK;
}

/*****************************************************************************
Load specific pattern to memory using ODPG
******************************************************************************/
GT_STATUS    ddr3TipLoadPatternToMem
(
    GT_U32          devNum,
    MV_HWS_PATTERN  pattern
)
{
    GT_U32  regData, interfaceId;
	PatternInfo *patternTable = ddr3TipGetPatternTable();

    /*DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_INFO, ("=== LOAD PATTERN ==="));*/

    /* load pattern to memory */
    /* Write Tx mode, CS0, phases, Tx burst size, delay between burst, rx pattern phases */
    regData = 0x1 | (patternTable[pattern].numOfPhasesTx << 5) | (patternTable[pattern].txBurstSize << 11) |
              (patternTable[pattern].delayBetweenBursts << 15) | (patternTable[pattern].numOfPhasesRx << 21) |  (0x1 << 25 ) | (effective_cs << 26);
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, regData, MASK_ALL_BITS));
    /* ODPG Write enable from BIST */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG,  (0x1| (effective_cs<<26)), 0xC000003));
    /* disable error injection */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_WRITE_DATA_ERROR_REG,  0, 0x1));
    /* load pattern to ODPG */
    ddr3TipLoadPatternToOdpg(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, pattern, patternTable[pattern].startAddr);

	if(ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3)
	{
		for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM ; interfaceId++)
		{
			if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, interfaceId) ==  0)
		         continue;
			CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_UNICAST, interfaceId, SDRAM_ODT_CONTROL_HIGH_REG, 0x3 , 0xf));
		}

		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_ENABLE_REG, 0x1 << ODPG_ENABLE_OFFS,  (0x1 << ODPG_ENABLE_OFFS)));
	}
	else {
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, (GT_U32)(0x1 << 31),  (GT_U32)(0x1 << 31)));
	}

    hwsOsExactDelayPtr((GT_U8)devNum, devNum, 1); /* 1 mSec */

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        CHECK_STATUS(isOdpgAccessDone(devNum, interfaceId));
    }
    /* Disable ODPG and stop write to memory*/
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, (0x1 << 30), (GT_U32)(0x3 << 30)));

    /* return to default */
    CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, ODPG_DATA_CONTROL_REG, 0, MASK_ALL_BITS));

	if(ddr3TipDevAttrGet(devNum, MV_ATTR_TIP_REV) >= MV_TIP_REV_3){
		/*disable odt0 for CS0 training - need to adjust for multy CS*/
		CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST,PARAM_NOT_CARE , 0x1498, 0x0 , 0xf));
	}

    /* temporary added */
    hwsOsExactDelayPtr((GT_U8)devNum, devNum, 1);
    return GT_OK;
}

/*****************************************************************************
Training search routine
******************************************************************************/
GT_STATUS    ddr3TipIpTrainingWrapperInt
(
    GT_U32                      devNum,
    MV_HWS_ACCESS_TYPE          accessType,
    GT_U32                      interfaceId, 
    MV_HWS_ACCESS_TYPE          pupAccessType,
    GT_U32                      pupNum, 
    GT_U32                      bitNum, 
    MV_HWS_TRAINING_RESULT      resultType,
    MV_HWS_ControlElement       controlElement,
    MV_HWS_SearchDirection      searchDir,
    MV_HWS_DIRECTION            direction,
    GT_U32                      interfaceMask, 
    GT_U32                      initValueL2h,
    GT_U32                      initValueH2l,
    GT_U32                      numIter,
    MV_HWS_PATTERN              pattern,
    MV_HWS_EdgeCompare          edgeComp,
    MV_HWS_DDR_CS               trainCsType,
    GT_U32                      csNum,
    MV_HWS_TrainingIpStatus*    trainStatus
)
{
    GT_U32  interfaceNum = 0, startIf, endIf, initValueUsed;
    MV_HWS_SearchDirection searchDirId , startSearch, endSearch;
    MV_HWS_EdgeCompare     edgeCompUsed;
	GT_U8 consTap = (direction == OPER_WRITE)?(64):(0);
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    if (trainStatus == NULL)
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("trainStatus is NULL\n"));
        return GT_FAIL;
    }
    if ((trainCsType > CS_NON_SINGLE) ||
        (edgeComp >= EDGE_PFP) ||
        (pattern >= PATTERN_LIMIT) ||
        (direction > OPER_WRITE_AND_READ) ||
        (searchDir > MV_HWS_High2Low) ||
        (controlElement > MV_HWS_ControlElement_DQS_SKEW) ||
        (resultType > RESULT_PER_BYTE) ||
        (pupNum >= octetsPerInterfaceNum) ||
        (pupAccessType > ACCESS_TYPE_MULTICAST) ||
        (interfaceId > 11)||
        (accessType > ACCESS_TYPE_MULTICAST))
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("wrong parameter trainCsType %d edgeComp %d pattern %d direction %d searchDir %d controlElement %d resultType %d pupNum %d pupAccessType %d interfaceId %d accessType %d\n",trainCsType , edgeComp , pattern , direction , searchDir , controlElement , resultType , pupNum , pupAccessType , interfaceId , accessType ));
        return GT_FAIL;
    }

    if (edgeComp == EDGE_FPF)
    {
        startSearch = MV_HWS_Low2High;
        endSearch = MV_HWS_High2Low;
        edgeCompUsed = EDGE_FP;
    }
    else
    {
        startSearch = searchDir;
        endSearch = searchDir;
        edgeCompUsed = edgeComp;
    }

    for(searchDirId = startSearch ; searchDirId <= endSearch ; searchDirId++)
    {
        initValueUsed = (searchDirId == MV_HWS_Low2High) ? initValueL2h : initValueH2l;
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_TRACE, ("devNum %d, accessType %d, interfaceId %d, pupAccessType %d,pupNum %d, resultType %d, controlElement %d searchDirId %d, direction %d, interfaceMask %d,initValueUsed %d, numIter %d, pattern %d, edgeCompUsed %d, trainCsType %d, csNum %d\n", 
                            devNum, accessType, interfaceId, pupAccessType ,pupNum, resultType, controlElement,
                            searchDirId, direction, interfaceMask,
                            initValueUsed, numIter, pattern, edgeCompUsed, trainCsType, csNum));

        ddr3TipIpTraining(devNum, accessType, interfaceId, pupAccessType ,pupNum, resultType, controlElement,
                          searchDirId, direction, interfaceMask,
                          initValueUsed, numIter, pattern, edgeCompUsed, trainCsType, csNum,  trainStatus);
        if (accessType == ACCESS_TYPE_MULTICAST)
        {
           startIf = 0;
           endIf = MAX_INTERFACE_NUM-1;
        }
        else
        {
            startIf = interfaceId;
            endIf = interfaceId;
        }
        for(interfaceNum = startIf; interfaceNum <= endIf ; interfaceNum++)
        {
	    VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceNum)
            csNum = 0;
            CHECK_STATUS(ddr3TipReadTrainingResult(devNum,  interfaceNum, pupAccessType, pupNum, bitNum,
                                                   searchDirId, direction, resultType,
                                                   TrainingLoadOperation_UNLOAD, trainCsType, 
                                                   NULL , GT_FALSE, consTap, GT_FALSE));
        }
    }
    
    return GT_OK;
}


/*****************************************************************************
Training search & read result routine
******************************************************************************/
GT_STATUS    ddr3TipIpTrainingWrapper
(
    GT_U32                      devNum,
    MV_HWS_ACCESS_TYPE          accessType,
    GT_U32                      interfaceId, 
    MV_HWS_ACCESS_TYPE          pupAccessType,
    GT_U32                      pupNum, 
    MV_HWS_TRAINING_RESULT      resultType,
    MV_HWS_ControlElement       controlElement,
    MV_HWS_SearchDirection      searchDir,
    MV_HWS_DIRECTION            direction,
    GT_U32                      interfaceMask, 
    GT_U32                      initValueL2h,
    GT_U32                      initValueH2l,
    GT_U32                      numIter,
    MV_HWS_PATTERN              pattern,
    MV_HWS_EdgeCompare          edgeComp,
    MV_HWS_DDR_CS               trainCsType,
    GT_U32                      csNum,
    MV_HWS_TrainingIpStatus*    trainStatus
)
{
    GT_U8 e1,e2;
    GT_U32 interfaceCnt, bitId, startIf, endIf, bitEnd=0;
    GT_U32 *result[MV_HWS_SearchDirLimit] = {0};
	GT_U8 consTap = (direction == OPER_WRITE)?(64):(0);
	GT_U8 bitBitMask[MAX_BUS_NUM] = {0}, bitBitMaskActive = 0;
	GT_U8 pupId;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    if (pupNum >= octetsPerInterfaceNum)
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("pupNum %d not valid\n",pupNum));
    }
    if (interfaceId >= MAX_INTERFACE_NUM)
    {
        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_ERROR, ("interfaceId %d not valid\n",interfaceId));
    }

    CHECK_STATUS(ddr3TipIpTrainingWrapperInt(devNum,accessType,interfaceId, pupAccessType, pupNum, 
							ALL_BITS_PER_PUP, resultType, controlElement, searchDir, direction,
							interfaceMask, initValueL2h, initValueH2l, numIter, pattern, edgeComp, trainCsType, 
							csNum, trainStatus));
    if (accessType == ACCESS_TYPE_MULTICAST)
    {
        startIf = 0;
        endIf = MAX_INTERFACE_NUM-1;
    }
    else
    {
        startIf = interfaceId;
        endIf = interfaceId;
    }
    for(interfaceCnt = startIf; interfaceCnt <= endIf; interfaceCnt++)    
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceCnt)
        for(pupId = 0; pupId <= (octetsPerInterfaceNum-1) ; pupId++)
        {
			VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pupId)
            if (resultType == RESULT_PER_BIT)
            {
                bitEnd = BUS_WIDTH_IN_BITS-1;
            }
            else
            {
                bitEnd = 0;
            }

			bitBitMask[pupId] = 0;
            for(bitId = 0; bitId <= bitEnd  ; bitId++)
            {
				MV_HWS_SearchDirection searchDirId;
                for(searchDirId = MV_HWS_Low2High ; searchDirId <= MV_HWS_High2Low ; searchDirId++)
                {
                    CHECK_STATUS(ddr3TipReadTrainingResult(devNum, interfaceCnt, ACCESS_TYPE_UNICAST,
                                pupId, bitId, searchDirId,  direction,
                                resultType, TrainingLoadOperation_UNLOAD,
                                CS_SINGLE, &result[searchDirId] , GT_TRUE, 0, GT_FALSE));
                }
                e1 = GET_TAP_RESULT(result[MV_HWS_Low2High][0], EDGE_1);
                e2 = GET_TAP_RESULT(result[MV_HWS_High2Low][0], EDGE_1);
                DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_INFO, ("wrapper interfaceId %d pupId %d bit %d l2h 0x%x (e1 0x%x) h2l 0x%x (e2 0x%x)  \n", interfaceCnt, pupId,  bitId, result[MV_HWS_Low2High][0],e1, result[MV_HWS_High2Low][0], e2));
                /* TBD validate is valid only for tx */
                if( VALIDATE_TRAINING_LIMIT(e1,e2) == GT_TRUE &&
					GET_LOCK_RESULT(result[MV_HWS_Low2High][0]) &&
					GET_LOCK_RESULT(result[MV_HWS_Low2High][0]))
                {
					/*Mark problem bits*/
					bitBitMask[pupId] |= 1<<bitId;
					bitBitMaskActive = 1;
                }
            }/*For all bits*/
        }/*For all PUPs*/

		/*Fix problem bits*/
		if(bitBitMaskActive != 0)
		{
			GT_U32  *L2HinterfaceTrainRes = NULL;
			GT_U32  *H2LinterfaceTrainRes = NULL;
			L2HinterfaceTrainRes = ddr3TipGetBufPtr(devNum, MV_HWS_Low2High,resultType,interfaceCnt);
			H2LinterfaceTrainRes = ddr3TipGetBufPtr(devNum, MV_HWS_High2Low,resultType,interfaceCnt);

        	ddr3TipIpTraining(devNum, ACCESS_TYPE_UNICAST, interfaceCnt, ACCESS_TYPE_MULTICAST , PARAM_NOT_CARE,
						  resultType, controlElement,
                          MV_HWS_Low2High, direction, interfaceMask,
                          numIter/2, numIter/2, pattern, EDGE_FP, trainCsType, csNum,  trainStatus);

		    for(pupId = 0; pupId <= (octetsPerInterfaceNum-1) ; pupId++)
		    {
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pupId)

				if(bitBitMask[pupId] == 0)
					continue;

		        for(bitId = 0; bitId <= bitEnd  ; bitId++)
		        {
					if( (bitBitMask[pupId] & (1<<bitId)) == 0)
						continue;
					CHECK_STATUS(ddr3TipReadTrainingResult(devNum, interfaceCnt, ACCESS_TYPE_UNICAST,
		                    pupId, bitId, MV_HWS_Low2High,  direction,
		                    resultType, TrainingLoadOperation_UNLOAD,
		                    CS_SINGLE, &L2HinterfaceTrainRes , GT_FALSE, 0, GT_TRUE));
				}
			}

			ddr3TipIpTraining(devNum, ACCESS_TYPE_UNICAST, interfaceCnt, ACCESS_TYPE_MULTICAST , PARAM_NOT_CARE,
						  resultType, controlElement,
                          MV_HWS_High2Low, direction, interfaceMask,
                          numIter/2, numIter/2, pattern, EDGE_FP, trainCsType, csNum,  trainStatus);

		    for(pupId = 0; pupId <= (octetsPerInterfaceNum-1) ; pupId++)
		    {
				VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pupId)

				if(bitBitMask[pupId] == 0)
					continue;

		        for(bitId = 0; bitId <= bitEnd  ; bitId++)
		        {
					if( (bitBitMask[pupId] & (1<<bitId)) == 0)
						continue;
		        	CHECK_STATUS(ddr3TipReadTrainingResult(devNum, interfaceCnt, ACCESS_TYPE_UNICAST,
		                    pupId, bitId, MV_HWS_High2Low,  direction,
		                    resultType, TrainingLoadOperation_UNLOAD,
		                    CS_SINGLE, &H2LinterfaceTrainRes , GT_FALSE, consTap, GT_TRUE));
				}
			}
		}/*if bitBitMaskActive*/
    }/*For all Interfacess*/

    return GT_OK;
}

/*****************************************************************************
Load phy values
******************************************************************************/
GT_STATUS    ddr3TipLoadPhyValues(GT_BOOL bLoad)
{
    GT_U32  busCnt = 0,  interfaceId,  devNum = 0;
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
    {
        VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
        for(busCnt = 0; busCnt < octetsPerInterfaceNum; busCnt++)
        {
        	VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, busCnt)
            if (bLoad == GT_TRUE)
            {
                CHECK_STATUS(mvHwsDdr3TipBUSRead( devNum, interfaceId,  ACCESS_TYPE_UNICAST, busCnt, DDR_PHY_DATA, WRITE_CENTRALIZATION_PHY_REG + (effective_cs * CS_REGISTER_ADDR_OFFSET), &phyRegBk[interfaceId][busCnt][0]));
                CHECK_STATUS(mvHwsDdr3TipBUSRead( devNum, interfaceId,  ACCESS_TYPE_UNICAST, busCnt, DDR_PHY_DATA, RL_PHY_REG + (effective_cs * CS_REGISTER_ADDR_OFFSET), &phyRegBk[interfaceId][busCnt][1]));
                CHECK_STATUS(mvHwsDdr3TipBUSRead( devNum, interfaceId,  ACCESS_TYPE_UNICAST, busCnt, DDR_PHY_DATA, READ_CENTRALIZATION_PHY_REG + (effective_cs * CS_REGISTER_ADDR_OFFSET), &phyRegBk[interfaceId][busCnt][2]));
            }
            else
            {
                CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId,  ACCESS_TYPE_UNICAST,  busCnt, DDR_PHY_DATA, WRITE_CENTRALIZATION_PHY_REG + (effective_cs * CS_REGISTER_ADDR_OFFSET), phyRegBk[interfaceId][busCnt][0]));
                CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId,  ACCESS_TYPE_UNICAST,  busCnt, DDR_PHY_DATA, RL_PHY_REG + (effective_cs * CS_REGISTER_ADDR_OFFSET), phyRegBk[interfaceId][busCnt][1]));
                CHECK_STATUS(mvHwsDdr3TipBUSWrite(devNum,  ACCESS_TYPE_UNICAST,   interfaceId,  ACCESS_TYPE_UNICAST,  busCnt, DDR_PHY_DATA, READ_CENTRALIZATION_PHY_REG + (effective_cs * CS_REGISTER_ADDR_OFFSET), phyRegBk[interfaceId][busCnt][2]));
            }
         }
    }
    return GT_OK;
}

GT_STATUS ddr3TipTrainingIpTest
(
    GT_U32                      devNum,
    MV_HWS_TRAINING_RESULT      resultType,
    MV_HWS_SearchDirection      searchDir,
    MV_HWS_DIRECTION            direction,  
    MV_HWS_EdgeCompare          edge,
    GT_U32                      initVal1,
    GT_U32                      initVal2,
    GT_U32                      numOfItertaions,
    GT_U32                      startPattern,
    GT_U32                      endPattern
)
{
    GT_U32 pattern, interfaceId, pupId;
    MV_HWS_TrainingIpStatus trainStatus[MAX_INTERFACE_NUM];
    GT_U32 *pRes = NULL;
    GT_U32 searchState = 0; 
	GT_U8 octetsPerInterfaceNum = (GT_U8)ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE);

    ddr3TipLoadPhyValues(GT_TRUE);

    for(pattern = startPattern; pattern <= endPattern; pattern++)
    {
        for(searchState = 0 ; searchState < MV_HWS_SearchDirLimit ; searchState++)
        {
            ddr3TipIpTrainingWrapper(devNum, ACCESS_TYPE_MULTICAST, 0, ACCESS_TYPE_MULTICAST, 
                                        0, resultType, MV_HWS_ControlElement_ADLL,
                                        searchDir, direction, 0xFFF, initVal1, initVal2, numOfItertaions, 
                                        pattern, edge, CS_SINGLE, PARAM_NOT_CARE, trainStatus);
            for(interfaceId = 0; interfaceId <= MAX_INTERFACE_NUM-1; interfaceId++)
            {
                VALIDATE_IF_ACTIVE(topologyMap->interfaceActiveMask, interfaceId)
                for(pupId = 0; pupId < octetsPerInterfaceNum ; pupId++)
                {
					VALIDATE_BUS_ACTIVE(topologyMap->activeBusMask, pupId)
                    CHECK_STATUS(ddr3TipReadTrainingResult(devNum, interfaceId, ACCESS_TYPE_UNICAST, 
                                    pupId, ALL_BITS_PER_PUP, searchState,  direction, 
                                    resultType, TrainingLoadOperation_UNLOAD,
                                    CS_SINGLE, &pRes , GT_TRUE, 0, GT_FALSE));
                    if (resultType == RESULT_PER_BYTE)
                    {
                        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_INFO, ("searchState %d interfaceId %d pupId %d 0x%x\n",searchState, interfaceId, pupId, pRes[0]));
                    }
                    else
                    {
                        DEBUG_TRAINING_IP_ENGINE(DEBUG_LEVEL_INFO, ("searchState %d interfaceId %d pupId %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",searchState, interfaceId, pupId, pRes[0],pRes[1],pRes[2],pRes[3],pRes[4],pRes[5],pRes[6],pRes[7]));
                    }
                }
            }/*interface*/
        }/*search*/
    }/*pattern*/
   ddr3TipLoadPhyValues(GT_FALSE);
   return GT_OK;
}

/********************Get patterns structures API*******************************/

PatternInfo*  ddr3TipGetPatternTable()
{
	if( DDR3_IS_16BIT_DRAM_MODE(topologyMap->activeBusMask) == GT_FALSE)
    	return patternTable_32;
	else
    	return patternTable_16;
}

GT_U16 *ddr3TipGetMaskResultsDqReg()
{
	if(DDR3_IS_ECC_PUP3_MODE(topologyMap->activeBusMask))
    	return maskResultsDqRegMapPup3ECC;
	else
    	return maskResultsDqRegMap;
}

GT_U16 *ddr3TipGetMaskResultsPupRegMap()
{
 	if(DDR3_IS_ECC_PUP3_MODE(topologyMap->activeBusMask))
    	return maskResultsPupRegMapPup3ECC;
	else
    	return maskResultsPupRegMap;
}



