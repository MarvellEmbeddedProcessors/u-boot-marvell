/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
		used to endorse or promote products derived from this software without
		specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef _INC_A370_VARS_H
#define _INC_A370_VARS_H

#include "ddr3_a370_config.h"
#include "ddr3_a370_mc_static.h"
#include "ddr3_a370_training_static.h"

/* Board/Soc revisions define */
typedef enum  {
	A0,
	A0_PCAC,
	A0_RD,
} MV_SOC_BOARD_REV;

typedef struct __mvDramModes {
	char *mode_name;
	MV_U8 cpuFreq;
	MV_U8 fabFreq;
	MV_U8 chipId;
	MV_SOC_BOARD_REV chipBoardRev;
	MV_DRAM_MC_INIT *regs;
	MV_DRAM_TRAINING_INIT *vals;
} MV_DRAM_MODES;

MV_DRAM_MODES ddr_modes[MV_DDR3_MODES_NUMBER] =
{
	/*	Conf name			CPUFreq		FabFreq		Chip ID		Chip/Board	MC regs		Training Values */
	/* db board values: */
	{"db_1200-300-600",		0x6,		0x5,		0x0,		A0,			ddr3_A0_db_600,		ddr3_db_600	},
	{"pcac_1200-300-600",	0x6,		0x5,		0x0,		A0_PCAC,	ddr3_A0_db_600,		ddr3_pcac_600 },
	{"rd_1200-300-600",		0x6,		0x5,		0x0,		A0_RD,		ddr3_A0_rd_600,		NULL },
};

/* ODT settings - if needed update the following tables: (ODT_OPT - represents the CS configuration bitmap) */

MV_U16 auiODTStatic[ODT_OPT][MAX_CS] =
{
	{0,		0,		0,		0		}, 	/* 0000 */
	{ODT60,	0,		0,		0		}, 	/* 0001 */
	{0,		0,		0,		0		}, 	/* 0010 */
	{ODT60,	0,		0,		0		}, 	/* 0011 */
	{0,		0,		0,		0		}, 	/* 0100 */
	{0,		0,		0,		0		}, 	/* 0101 */
	{0,		0,		0,		0		}, 	/* 0110 */
	{0,		0,		0,		0		}, 	/* 0111 */
	{0,		0,		0,		0		}, 	/* 1000 */
	{0,		0,		0,		0		}, 	/* 1001 */
	{0,		0,		0,		0		}, 	/* 1010 */
	{0,		0,		0,		0		}, 	/* 1011 */
	{0,		0,		0,		0		}, 	/* 1100 */
	{0,		0,		0,		0		}, 	/* 1101 */
	{0,		0,		0,		0		}, 	/* 1110 */
	{0,		0,		0,		0		}, 	/* 1111 */
};

MV_U16 auiODTDynamic[ODT_OPT][MAX_CS] =
{
	{0,		0,		0,		0		}, 	/* 0000 */
	{0,		0,		0,		0		}, 	/* 0001 */
	{0,		0,		0,		0		}, 	/* 0010 */
	{0,		0,		0,		0		}, 	/* 0011 */
	{0,		0,		0,		0		}, 	/* 0100 */
	{0,		0,		0,		0		}, 	/* 0101 */
	{0,		0,		0,		0		}, 	/* 0110 */
	{0,		0,		0,		0		}, 	/* 0111 */
	{0,		0,		0,		0		}, 	/* 1000 */
	{0,		0,		0,		0		}, 	/* 1001 */
	{0,		0,		0,		0		}, 	/* 1010 */
	{0,		0,		0,		0		}, 	/* 1011 */
	{0,		0,		0,		0		}, 	/* 1100 */
	{0,		0,		0,		0		}, 	/* 1101 */
	{0,		0,		0,		0		}, 	/* 1110 */
	{0,		0,		0,		0		} 	/* 1111 */
};

MV_U32 auiODTConfig[ODT_OPT] = {
	0, 0x00010000, 0, 0x00030000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*	User can manually set SPD values (in case SPD is not available on DIMM/System).
	SPD Values can simplify calculating the DUNIT registers values */	
MV_U8 ucData[SPD_SIZE] = 
{ 
	/* A370 DB Board DIMM SPD Values - manually set */
/* 2 CS */
	0x92, 0x10, 0x0B, 0x2, 0x3, 0x19, 0x0, 0x9, 0x09, 0x52, 0x1, 0x8, 0x0C, 0x0, 0x7E, 0x0, 0x69, 0x78,
/* 1 CS */
/*	0x92, 0x10, 0x0B, 0x2, 0x3, 0x19, 0x0, 0x1, 0x09, 0x52, 0x1, 0x8, 0x0C, 0x0, 0x7E, 0x0, 0x69, 0x78, */
	0x69, 0x30, 0x69, 0x11, 0x20, 0x89, 0x0, 0x5, 0x3C, 0x3C, 0x0, 0xF0, 0x82, 0x5, 0x80, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0F, 0x1, 0x3, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x80, 0x2C, 0x1, 0x10, 0x23, 0x35, 0x28, 0xEB, 0xCA, 0x19, 0x8F
};

/*******************************************************************/
/* Controller Specific configurations Starts Here - DO NOT MODIFY  */
/*******************************************************************/

/* Frequency - values are 1/HCLK in ps */
MV_U32 s_auiCpuFabClkToHClk[FAB_OPT][CLK_CPU] =
/* CPU Frequency:
	400		533		667		800		1000	1067	1200	1333	Fabric */
{
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 3330	, 2500	, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
    {0      , 0     , 0     , 0     , 4000  , 3750  , 3000  , 3000  },
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 2500	, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
 	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
    {0      , 3750  , 3000  , 0     , 0     , 0     , 0     , 0     },
	{2500	, 0		, 0		, 3750	, 3000	, 0		, 0		, 0		}
};

MV_U32 s_auiCpuDdrRatios[FAB_OPT][CLK_CPU] =
/* CPU Frequency:
	400		533		667		800		1000	1067	1200	1333	Fabric */
{
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, DDR_333, DDR_400, 0	, 0		, 0		, 0		},	
 	{0		, 0		, 0		, 0		,0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
 	{0		, 0		, 0		, 0		, DDR_500, DDR_533, DDR_600, DDR_666 },
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
 	{0		, 0		, 0		, 0		, 0		, 0		, DDR_400, 0	},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
 	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
 	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
    {0      , DDR_533, DDR_666, 0   , 0     , 0     , 0     , 0     },
	{DDR_400, 0		, 0		, DDR_533, DDR_666, 0	, 0		, 0		}
};

MV_U8 s_auiDivRatio1to1[CLK_VCO][CLK_DDR] =
/* DDR Frequency:
	100		333	400 533	600	666		*/
{ 	{0x4,	0,	1,	0,	0,	0	},	/*	1:1	CLK_CPU_400 = VCO  */
	{0x5,	0,	0,	1,	0,	0	},	/*	1:1	CLK_CPU_533 = VCO   */
	{0x6,	2,	0,	0,	0,	0	},	/*	1:1	CLK_CPU_667 = VCO   */
	{0x8,	0,	2,	0,	0,	0	},	/*	1:1	CLK_CPU_800 = VCO   */
	{0xA,	3,	5,	2,	0,	0	},	/*	1:1	CLK_CPU_1000 = VCO   */
	{0xA,	0,	0,	2,	0,	0	},	/*	1:1	CLK_CPU_1067 = VCO   */
	{0xC,	0,	6,	0,	2,	0	},	/*	1:1	CLK_CPU_1200 = VCO   */
	{0xD,	0,	0,	0,	0,	2	},	/*	1:1	CLK_CPU_1333 = VCO   */
	{0x8,	0,	0,	0,	0,	0	},	/*	1:1	CLK_CPU_400 VCO_800  */
	{0xA,	0,	0,	0,	0,	0	},	/*	1:1	CLK_CPU_533 VCO_1066 */
	{0xC,	0,	0,	0,	0,	0	},	/*	1:1	CLK_CPU_667 VCO_1333 */
	{0x10,	0,	0,	0,	0,	0	},	/*	1:1	CLK_CPU_800 VCO_1600 */
	{0x14,	0,	0,	0,	0,	0	},	/*	1:1	CLK_CPU_1000 VCO_2000  */
	{0x15,	0,	0,	0,	0,	0	},	/*	1:1	CLK_CPU_1067 VCO_2133 */
	{0x18,	0,	6,	0,	0,	0	},	/*	1:1	CLK_CPU_1200 VCO_2400 */
	{0x1A,	0,	0,	0,	0,	0	}	/*	1:1	CLK_CPU_1333 VCO_2666 */
};

MV_U8 s_auiDivRatio2to1[CLK_VCO][CLK_DDR] =
/* DDR Frequency:
	100		333	400	533	600	666		*/
{ 	{0,		0,	0,	0,	0,	0	},	/*	2:1	CLK_CPU_400 = VCO  */
	{0,		0,	0,	1,	0,	0	},	/*	2:1	CLK_CPU_533 = VCO  */
    {0,     2,  0,  0,  0,  1   },  /*  2:1 CLK_CPU_667 = VCO  */
	{0,		0,	2,	3,	0,	0	},	/*	2:1	CLK_CPU_800 = VCO  */
	{0,		0,	5,	2,	0,	0	},	/*	2:1	CLK_CPU_1000 = VCO  */
	{0,		0,	0,	2,	0,	5	},	/*	2:1	CLK_CPU_1067 = VCO  */
	{0,		0,	3,	0,	2,	0	},	/*	2:1	CLK_CPU_1200 = VCO  */
	{0,		0,	0,	5,	0,	2	},	/*	2:1	CLK_CPU_1333 = VCO  */
	{0,		0,	0,	0,	0,	0	},	/*	2:1	CLK_CPU_400 VCO_800  */
	{0,		0,	0,	0,	0,	0	},	/*	2:1	CLK_CPU_533 VCO_1066 */
	{0,		0,	0,	0,	0,	0	},	/*	2:1	CLK_CPU_667 VCO_1333 */
	{0,		0,	0,	3,	0,	0	},	/*	2:1	CLK_CPU_800 VCO_1600 */
	{0,		0,	0,	0,	0,	3	},	/*	2:1	CLK_CPU_1000 VCO_2000  */
	{0,		0,	0,	0,	0,	0	},	/*	2:1	CLK_CPU_1067 VCO_2133 */
	{0,		0,	6,	0,	0,	0	},	/*	2:1	CLK_CPU_1200 VCO_2400 */
	{0,		0,	0,	0,	0,	0	}	/*	2:1	CLK_CPU_1333 VCO_2666 */
};

#endif /* _INC_A370_VARS_H */

