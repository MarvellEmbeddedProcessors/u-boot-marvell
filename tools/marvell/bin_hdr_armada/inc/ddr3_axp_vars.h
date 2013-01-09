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

#ifndef _INC_AXP_VARS_H
#define _INC_AXP_VARS_H

#include "ddr3_axp_config.h"
#include "ddr3_axp_mc_static.h"
#include "ddr3_axp_training_static.h"

/* Board/Soc revisions define */
typedef enum  {
	Z1,
	Z1_PCAC,
	Z1_RD_SLED,
	A0,
	A0_AMC
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
	/*	Conf name		CPUFreq		FabFreq		Chip ID	Chip/Board	MC regs			Training Values */
	/* db board values: */
	{"db_800-400",		0xA,		0x5,		0x0,	A0,			ddr3_A0_db_400,	NULL},
	{"db_1200-300",		0x2,		0xC,		0x0,	A0,			ddr3_A0_db_400,	NULL},	
	{"db_1200-600",		0x2,		0x5,		0x0,	A0,			NULL,			NULL},
	{"db_1333-667",		0x3,		0x5,		0x0,	A0,			ddr3_A0_db_667,	ddr3_db_rev2_667},
	{"db_1600-800",		0xB,		0x5,		0x0,	A0,			ddr3_A0_db_667,	ddr3_db_rev2_800},
	{"amc_1333-667",	0x3,		0x5,		0x0,	A0_AMC,		ddr3_A0_AMC_667,NULL},
	{"db_667-667",		0x9,		0x13,		0x0,	Z1,			ddr3_Z1_db_600,	ddr3_db_667},	
	{"db_800-400",		0xA,		0x1,		0x0,	Z1,			ddr3_Z1_db_300,	ddr3_db_400},
	{"db_1066-533",		0x1,		0x1,		0x0,	Z1,			ddr3_Z1_db_300,	ddr3_db_533},
	{"db_1200-300",		0x2,		0xC,		0x0,	Z1,			ddr3_Z1_db_300,	ddr3_db_667},	
	{"db_1200-600",		0x2,		0x5,		0x0,	Z1,			ddr3_Z1_db_600,	NULL},
	{"db_1333-333",		0x3,		0xC,		0x0,	Z1,			ddr3_Z1_db_300,	ddr3_db_400},	
	{"db_1333-667",		0x3,		0x5,		0x0,	Z1,			ddr3_Z1_db_600,	ddr3_db_667},
	/* pcac board values (Z1 device): */
	{"pcac_1200-600",	0x2,		0x5,		0x0,	Z1_PCAC,	ddr3_Z1_db_600,	ddr3_pcac_600},
	/* rd board values (Z1 device): */
	{"rd_667_0",		0x3,		0x5,		0x0,	Z1_RD_SLED,	ddr3_Z1_db_600,	ddr3_rd_667_0},
	{"rd_667_1",		0x3,		0x5,		0x1,	Z1_RD_SLED,	ddr3_Z1_db_600,	ddr3_rd_667_1},
	{"rd_667_2",		0x3,		0x5,		0x2,	Z1_RD_SLED,	ddr3_Z1_db_600,	ddr3_rd_667_2},
	{"rd_667_3",		0x3,		0x5,		0x3,	Z1_RD_SLED,	ddr3_Z1_db_600,	ddr3_rd_667_3}
};

/* ODT settings - if needed update the following tables: (ODT_OPT - represents the CS configuration bitmap) */

MV_U16 auiODTStatic[ODT_OPT][MAX_CS] =
{  										/* 	  NearEnd/FarEnd */
	{0,		0,		0,		0		}, 	/* 0000 	0/0 - Not supported */
	{ODT40,	0,		0,		0		}, 	/* 0001 	0/1 */
	{0,		0,		0,		0		}, 	/* 0010 	0/0 - Not supported */
	{ODT40,	ODT40,	0,		0		}, 	/* 0011 	0/2 */
	{0,		0,		ODT40,	0		}, 	/* 0100 	1/0 */
	{ODT30,	0,		ODT30,	0		}, 	/* 0101 	1/1 */
	{0,		0,		0,		0		}, 	/* 0110 	0/0 - Not supported */
	{ODT120, ODT20, ODT20,	0		}, 	/* 0111 	1/2 */
	{0,		0,		0,		0		}, 	/* 1000 	0/0 - Not supported */
	{0,		0,		0,		0		}, 	/* 1001 	0/0 - Not supported */
	{0,		0,		0,		0		}, 	/* 1010 	0/0 - Not supported */
	{0,		0,		0,		0		}, 	/* 1011 	0/0 - Not supported */
	{0,		0,		ODT40,	0		}, 	/* 1100 	2/0 */
	{ODT20,	0,		ODT120,	ODT20	}, 	/* 1101 	2/1 */
	{0,		0,		0,		0		}, 	/* 1110 	0/0 - Not supported */
	{ODT120, ODT30,	ODT120,	ODT30	} 	/* 1111 	2/2 */
};

MV_U16 auiODTDynamic[ODT_OPT][MAX_CS] =
{										/* 	  NearEnd/FarEnd */
	{0,		0,		0,		0		}, 	/* 0000 	0/0 */
	{0,		0,		0,		0		}, 	/* 0001 	0/1 */
	{0,		0,		0,		0		}, 	/* 0010 	0/0 - Not supported */
	{0,		0,		0,		0		}, 	/* 0011 	0/2 */
	{0,		0,		0,		0		}, 	/* 0100 	1/0 */
	{ODT120D, 0,	ODT120D, 0		}, 	/* 0101 	1/1 */
	{0,		0,		0,		0		}, 	/* 0110 	0/0 - Not supported */
	{0,		0,		ODT120D, 0		}, 	/* 0111 	1/2 */
	{0,		0,		0,		0		}, 	/* 1000 	0/0 - Not supported */
	{0,		0,		0,		0		}, 	/* 1001 	0/0 - Not supported */
	{0,		0,		0,		0		}, 	/* 1010 	0/0 - Not supported */
	{0,		0,		0,		0		}, 	/* 1011 	0/0 - Not supported */
	{0,		0,		0,		0		}, 	/* 1100 	2/0 */
	{ODT120D, 0,	0,		0		}, 	/* 1101 	2/1 */
	{0,		0,		0,		0		}, 	/* 1110 	0/0 - Not supported */
	{0,		0,		0,		0		} 	/* 1111 	2/2 */
};

MV_U32 auiODTConfig[ODT_OPT] = {
	0, 0x00010000, 0, 0x00030000, 0x04000000, 0x05050104, 0, 0x07430340, 0, 0, 0 , 0,
	0x30000, 0x1C0D100C, 0, 0x3CC330C0
};

/*	User can manually set SPD values (in case SPD is not available on DIMM/System).
	SPD Values can simplify calculating the DUNIT registers values */	
MV_U8 ucData[SPD_SIZE] = 
{ 
	/* AXP DB Board DIMM SPD Values - manually set */
	0x92, 0x10, 0x0B, 0x2, 0x3, 0x19, 0x0, 0x9, 0x09, 0x52, 0x1, 0x8, 0x0C, 0x0, 0x7E, 0x0, 0x69, 0x78,
	0x69, 0x30, 0x69, 0x11, 0x20, 0x89, 0x0, 0x5, 0x3C, 0x3C, 0x0, 0xF0, 0x82, 0x5, 0x80, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0F, 0x1, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x80, 0x2C, 0x1, 0x10, 0x23, 0x35, 0x28, 0xEB, 0xCA, 0x19, 0x8F
};


/*******************************************************************/
/* Controller Specific configurations Starts Here - DO NOT MODIFY  */
/*******************************************************************/

#ifdef MV88F78X60_Z1
/* Frequency - values are 1/HCLK in ps */
MV_U32 s_auiCpuFabClkToHClk[FAB_OPT][CLK_CPU] =
/* CPU Frequency:
	1000	1066	1200	1333	1500	1666	1800	2000	600		667		800		1600	Fabric */
{
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, DDR_S	, 0		, 0		, 0		, 0		, 0		, 0		, 0 	, 0		, DDR_S	, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 3330	, DDR_S	, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 3330	, DDR_S	, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, DDR_S	, 0		, 0		},
	{0		, 0		, 2500	, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		}
};

MV_U32 s_auiCpuDdrRatios[FAB_OPT][CLK_CPU] =
/* CPU Frequency:
	1000	1066	1200	1333	1500	1666	1800	2000	600		667		800		1600	Fabric */
{
	{1		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0	 	, 0		, 0		, 0		},
	{0		, DDR_S_1TO1, 0	, 0		, 0		, 0		, 0		, 0		, 0		, 0		, DDR_S_1TO1, 0	},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, DDR_600, DDR_S, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, DDR_300, DDR_S_1TO1, 0, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, DDR_S	, 0		, 0		},
	{0		, 0		, DDR_S	, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		}
};
#else
/* Frequency - values are 1/HCLK in ps */
MV_U32 s_auiCpuFabClkToHClk[FAB_OPT][CLK_CPU] =
/* CPU Frequency:
	1000	1066	1200	1333	1500	1666	1800	2000	600		667		800		1600	Fabric */
{
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0 	, 3000	, 2500	, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 4500	, 3750	, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 2500	, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{4000	, 3750	, 3333	, 3000	, 2666	, 2400	, 0		, 0		, 0		, 0		, 5000	, 2500	},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 3000	, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{2500	, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 5000	, 0		, 4000	, 0		, 0		, 0		, 0		, 0		, 0		, 3750	},
	{5000	, 0		, 0		, 3750	, 3333	, 0		, 0		, 0		, 0		, 0		, 0		, 3125	},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 3330	, 3000	, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 2500	},
    {0      , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 3750  },
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 3000	, 2500	, 0		},
	{3000	, 0		, 2500	, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 3750	, 0		}
};

MV_U32 s_auiCpuDdrRatios[FAB_OPT][CLK_CPU] =
/* CPU Frequency:
	1000	1066	1200	1333	1500	1666	1800	2000	600		667		800		1600	Fabric */
{
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0	 	, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, DDR_333, DDR_400 , 0	},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, DDR_444, DDR_533, 0	},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, DDR_400, 0	, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{DDR_500, DDR_533, DDR_600, DDR_666, DDR_750, DDR_833, 0, 0		, 0		, 0		, DDR_400 , DDR_800	},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, DDR_333, 0	, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{DDR_400, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, DDR_400, 0	, DDR_500, 0	, 0		, 0		, 0		, 0		, 0		, DDR_533 },
	{DDR_400, 0		, 0		, DDR_533, DDR_600, 0	, 0		, 0		, 0		, 0		, 0		, DDR_640 },
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
 	{0		, 0		, DDR_300, DDR_333, 0	, 0		, 0		, 0		, 0		, 0		, 0		, DDR_400 },
    {0      , 0     , 0     , 0     , 0     , 0     , DDR_600, DDR_666, 0   , 0     , 0     , DDR_533 },
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		},
	{0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, 0		, DDR_666, DDR_800 , 0	},
	{DDR_666, 0		, DDR_800, 0	, 0		, 0		, 0		, 0		, 0		, 0		, DDR_533  , 0		}
};
#endif

MV_U8 s_auiDivRatio1to1[CLK_VCO][CLK_DDR] =
/* DDR Frequency:
	100		300	360	400	444	500	533	600	666	750	800	833  */
{ 	{0xA,	3,	0,	3,	0,	2,	0,	0,	0,	0,	0,	0}, /*	1:1	CLK_CPU_1000  */
	{0xB,	3,	0,	3,	0,	0,	2,	0,	0,	0,	0,	0}, /*	1:1	CLK_CPU_1066  */
	{0xC,	4,	0,	3,	0,	0,	0,	2,	0,	0,	0,	0}, /*	1:1	CLK_CPU_1200  */
	{0xD,	4,	0,	4,	0,	0,	0,	0,	2,	0,	0,	0},	/*	1:1	CLK_CPU_1333  */
	{0xF,	5,	0,	4,	0,	3,	0,	0,	0,	0,	0,	0},	/*	1:1	CLK_CPU_1500  */
	{0x11,	5,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1	CLK_CPU_1666  */
	{0x12,	6,	5,	4,	0,	0,	0,	3,	0,	0,	0,	0},	/*	1:1	CLK_CPU_1800  */
	{0x14,	7,	0,	5,	0,	4,	0,	0,	3,	0,	0,	0},	/*	1:1	CLK_CPU_2000  */
	{0x6,	2,	0,	2,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1 CLK_CPU_600   */
	{0x6,	2,	0,	2,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1	CLK_CPU_667   */
	{0x8,	2,	0,	2,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1 CLK_CPU_800   */
 	{0x10,	5,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1 CLK_CPU_1600   */
	{0x14,	0,	0,	5,	0,	0,	0,	0,	0,	0,	0,	0}, /*	1:1	CLK_CPU_1000 VCO_2000 */
	{0x15,	0,	0,	6,	0,	0,	0,	0,	0,	0,	0,	0}, /*	1:1	CLK_CPU_1066 VCO_2133 */
	{0x18,	0,	0,	6,	0,	0,	0,	0,	0,	0,	0,	0}, /*	1:1	CLK_CPU_1200 VCO_2400 */
	{0x1A,	0,	0,	7,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1	CLK_CPU_1333 VCO_2666 */
	{0x1E,	0,	0,	8,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1	CLK_CPU_1500 VCO_3000 */
	{0x21,	0,	0,	9,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1	CLK_CPU_1666 VCO_3333 */
	{0x24,	0,	0,	9,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1	CLK_CPU_1800 VCO_3600 */
	{0x28,	0,	0,	10,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1	CLK_CPU_2000 VCO_4000 */
	{0xC,	0,	0,	3,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1 CLK_CPU_600 VCO_1200 */
	{0xD,	0,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0},	/*	1:1	CLK_CPU_667 VCO_1333 */
	{0x10,	0,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0}, /*	1:1 CLK_CPU_800 VCO_1600 */
	{0x20,	10,	0,	8,	0,	0,	0,	0,	0,	0,	0,	0} 	/*	1:1 CLK_CPU_1600 VCO_3200 */
};

MV_U8 s_auiDivRatio2to1[CLK_VCO][CLK_DDR] =
/* DDR Frequency:
		100	300	360	400	444	500	533	600	666	750	800	833  */
{ 	{	0,	0,	0,	0,	0,	2,	0,	0,	3,	0,	0,	0},	/*	2:1	CLK_CPU_1000  */
	{	0,	0,	0,	0,	0,	0,	2,	0,	0,	0,	0,	0},	/*	2:1	CLK_CPU_1066  */
	{	0,	0,	0,	3,	5,	0,	0,	2,	0,	0,	3,	3},	/*	2:1	CLK_CPU_1200  */
	{	0,	0,	0,	0,	0,	0,	5,	0,	2,	0,	3,	0},	/*	2:1	CLK_CPU_1333  */
	{	0,	0,	0,	0,	0,	3,	0,	5,	0,	2,	0,	0},	/*	2:1	CLK_CPU_1500  */
	{	0,	0,	0,	0,	0,	0,	0,	0,	5,	0,	0,	2},	/*	2:1	CLK_CPU_1666  */
	{	0,	0,	0,	0,	0,	0,	0,	3,	0,	5,	0,	0},	/*	2:1	CLK_CPU_1800  */
	{	0,	0,	0,	0,	0,	0,	0,	0,	3,	0,	0,	5},	/*	2:1	CLK_CPU_2000  */
	{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	/*	2:1	CLK_CPU_600   */
	{	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0}, /*	2:1 CLK_CPU_667   */
	{	0,	0,	0,	2,	0,	0,	0,	0,	0,	0,	1,	0},	/*	2:1 CLK_CPU_800   */
	{	0,	0,	0,	0,	0,	0,	3,	0,	0,	0,	2,	0},	/*	2:1 CLK_CPU_1600   */
	{	0,	0,	0,	5,	0,	0,	0,	0,	3,	0,	0,	0},	/*	2:1	CLK_CPU_1000 VCO_2000 */
	{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	/*	2:1	CLK_CPU_1066 VCO_2133 */
	{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	3,	0},	/*	2:1	CLK_CPU_1200 VCO_2400 */
	{	0,	0,	0,	0,	0,	0,	5,	0,	0,	0,	0,	0},	/*	2:1	CLK_CPU_1333 VCO_2666 */
	{	0,	0,	0,	0,	0,	0,	0,	5,	0,	0,	0,	0},	/*	2:1	CLK_CPU_1500 VCO_3000 */
	{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	/*	2:1	CLK_CPU_1666 VCO_3333 */
	{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	/*	2:1	CLK_CPU_1800 VCO_3600 */
	{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	/*	2:1	CLK_CPU_2000 VCO_4000 */
	{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	/*	2:1	CLK_CPU_600 VCO_1200 */
	{	0,	0,	0,	0,	3,	0,	0,	0,	0,	0,	0,	0}, /*	2:1 CLK_CPU_667 VCO_1333 */
	{	0,	0,	0,	0,	0,	0,	3,	0,	0,	0,	0,	0},	/*	2:1 CLK_CPU_800 VCO_1600 */
	{	0,	0,	0,	0,	0,	0,	0,	5,	5,	0,	0,	0} 	/*	2:1 CLK_CPU_1600 VCO_3200 */
};

#endif /* _INC_AXP_VARS_H */

