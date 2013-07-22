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
on the worldwide web_http://www.gnu.org/licenses/gpl.txt.

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

#ifndef _INC_ALP_VARS_H
#define _INC_ALP_VARS_H

#include "ddr3_alp_config.h"
#include "ddr3_alp_mc_static.h"
#include "ddr3_alp_training_static.h"

/* Board/Soc revisions define */
typedef enum  {
    Z1_ALP_RD_6650 = 0,
    Z1_ALP_DB_6650,
    Z1_ALP_RD_6660,
    Z1_ALP_DB_6660,
    A0

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

MV_DRAM_MODES ddr_modes[] =
{
    /*  Conf name       CPUFreq     FabFreq     Chip ID    Chip/Board          MC regs          Training Values */
    /* db board values  */
    {"db_88F6650_533",  0x15,        0,          0x0,    Z1_ALP_DB_6650,  ddr3_Z0_db_88F6650_533, NULL},
    {"db_88F6660_533",  0x15,        0,          0x0,    Z1_ALP_DB_6660,  ddr3_Z0_db_88F6660_533, ddr3_db_88F6660},
    {"db_88F6660_400",  0x14,        0,          0x0,    Z1_ALP_DB_6660,  ddr3_Z0_db_88F6660_533, ddr3_db_88F6660},
    /* rd board values  */
    {"rd_88F6650_400",  0x14,        0,          0x0,    Z1_ALP_RD_6650,  ddr3_Z0_rd_88F6650_400, ddr3_rd_88F6650},
    {"rd_88F6660_533",  0x15,        0,          0x0,    Z1_ALP_RD_6660,  ddr3_Z0_rd_88F6660_533, NULL},
};

MV_U16 auiODTStatic[ODT_OPT][MAX_CS] =
{                                       /*    NearEnd/FarEnd */
    {0,     0,      },  /* 0000     0/0 - Not supported */
    {ODT40, 0,      },  /* 0001     0/1 */
    {0,     0,      },  /* 0010     0/0 - Not supported */
    {ODT40, ODT40,  },  /* 0011     0/2 */
    {0,     0,      },  /* 0100     1/0 */
    {ODT30, 0,      },  /* 0101     1/1 */
    {0,     0,      },  /* 0110     0/0 - Not supported */
    {ODT120, ODT20,  },  /* 0111     1/2 */
    {0,     0,      },  /* 1000     0/0 - Not supported */
    {0,     0,      },  /* 1001     0/0 - Not supported */
    {0,     0,      },  /* 1010     0/0 - Not supported */
    {0,     0,      },  /* 1011     0/0 - Not supported */
    {0,     0,      },  /* 1100     2/0 */
    {ODT20, 0,      },  /* 1101     2/1 */
    {0,     0,      },  /* 1110     0/0 - Not supported */
    {ODT120, ODT30  }   /* 1111     2/2 */
};

MV_U16 auiODTDynamic[ODT_OPT][MAX_CS] =
{                                       /*    NearEnd/FarEnd */
    {0,     0,      },  /* 0000     0/0 */
    {0,     0,      },  /* 0001     0/1 */
    {0,     0,      },  /* 0010     0/0 - Not supported */
    {0,     0,      },  /* 0011     0/2 */
    {0,     0,      },  /* 0100     1/0 */
    {ODT120D, 0,    },  /* 0101     1/1 */
    {0,     0,      },  /* 0110     0/0 - Not supported */
    {0,     0,      },  /* 0111     1/2 */
    {0,     0,      },  /* 1000     0/0 - Not supported */
    {0,     0,      },  /* 1001     0/0 - Not supported */
    {0,     0,      },  /* 1010     0/0 - Not supported */
    {0,     0,      },  /* 1011     0/0 - Not supported */
    {0,     0,      },  /* 1100     2/0 */
    {ODT120D, 0,    },  /* 1101     2/1 */
    {0,     0,      },  /* 1110     0/0 - Not supported */
    {0,     0,      }   /* 1111     2/2 */
};

MV_U32 auiODTConfig[ODT_OPT] = {
    0, 0x00010000, 0, 0x00030000, 0x04000000, 0x05050104, 0, 0x07430340, 0, 0, 0 , 0,
    0x30000, 0x1C0D100C, 0, 0x3CC330C0
};

/*  User can manually set SPD values (in case SPD is not available on DIMM/System).
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

MV_U8 s_auiDivRatio[CLK_VCO][CLK_DDR] =
/* DDR Frequency:
    100     300 360 400 444 500 533 600 666 750 800 833  */
{
    {0x2,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:1 VCO_266.67       */
    {0x3,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:2 VCO_333.33       */
    {0x6,   0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0}, /*  2:3 VCO_666.67       */
    {0x3,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:1 VCO_333.33       */
    {0x4,   0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:2 VCO_400.00       */
    {0x8,   0,  0,  2,  0,  0,  2,  0,  0,  0,  0,  0}, /*  2:3 VCO_800.00       */
    {0x4,   0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:1 VCO_400.00       */
    {0x5,   0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:2 VCO_500.00       */
    {0xA,   0,  0,  3,  0,  0,  2,  0,  0,  0,  0,  0}, /*  2:3 VCO_1,000.00     */
    {0x5,   0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:1 VCO_500.00       */
    {0x5,   0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:2 VCO_533.33       */
    {0xB,   0,  0,  3,  0,  0,  2,  0,  0,  0,  0,  0}, /*  2:3 VCO_1,066.67     */
    {0x5,   0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:1 VCO_533.33       */
    {0x6,   0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:2 VCO_600.00       */
    {0xC,   0,  0,  3,  0,  0,  2,  0,  0,  0,  0,  0}, /*  2:3 VCO_1,200.00     */
    {0x6,   0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:1 VCO_600.00       */
    {0x6,   0,  0,  2,  0,  0,  3,  0,  0,  0,  0,  0}, /*  1:2 VCO_666.67       */
    {0xD,   0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0}, /*  2:3 VCO_1,333.33     */
    {0x6,   0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:1 VCO_666.67       */
    {0x8,   0,  0,  2,  0,  0,  2,  0,  0,  0,  0,  0}, /*  1:3 VCO_800.00       */
    {0x8,   0,  0,  2,  0,  0,  2,  0,  0,  0,  0,  0}, /*  1:2 VCO_800.00       */
    {0x10,  0,  0,  4,  0,  0,  3,  0,  0,  0,  0,  0}, /*  2:3 VCO_1,600.00     */
    {0x9,   0,  0,  3,  0,  0,  2,  0,  0,  0,  0,  0}, /*  1:3 VCO_900.00         */
    {0x9,   0,  0,  3,  0,  0,  2,  0,  0,  0,  0,  0}, /*  1:2 VCO_900.00         */
    {0x12,  0,  0,  4,  0,  0,  3,  0,  0,  0,  0,  0}, /*  2:3 VCO_1,800.00       */
    {0xA,   0,  0,  3,  0,  0,  2,  0,  0,  0,  0,  0}, /*  1:2 VCO_1,000.00       */
    {0x14,  0,  0,  5,  0,  0,  4,  0,  0,  0,  0,  0}, /*  2:3 VCO_2,000.00       */
    {0xA,   0,  0,  3,  0,  0,  2,  0,  0,  0,  0,  0}, /*  1:2 VCO_1,000.00       */
    {0x4,   0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0}, /*  1:2 VCO_400.00         */
    {0xB,   0,  0,  3,  0,  0,  2,  0,  0,  0,  0,  0}, /*  1:2 VCO_1,100.00       */
    {0xC,   0,  0,  3,  0,  0,  2,  0,  0,  0,  0,  0} /*  1:2 VCO_1,200.00       */
};

typedef enum
{
    CPU_266MHz_DDR_266MHz_L2_133MHz,
    CPU_333MHz_DDR_167MHz_L2_167MHz,
    CPU_333MHz_DDR_222MHz_L2_167MHz,
    CPU_333MHz_DDR_333MHz_L2_167MHz,
    CPU_400MHz_DDR_200MHz_L2_200MHz,
    CPU_400MHz_DDR_267MHz_L2_200MHz,
    CPU_400MHz_DDR_400MHz_L2_200MHz,
    CPU_500MHz_DDR_250MHz_L2_250MHz,
    CPU_500MHz_DDR_334MHz_L2_250MHz,
    CPU_500MHz_DDR_500MHz_L2_250MHz,
    CPU_533MHz_DDR_267MHz_L2_267MHz,
    CPU_533MHz_DDR_356MHz_L2_267MHz,
    CPU_533MHz_DDR_533MHz_L2_267MHz,
    CPU_600MHz_DDR_300MHz_L2_300MHz,
    CPU_600MHz_DDR_400MHz_L2_300MHz,
    CPU_600MHz_DDR_600MHz_L2_300MHz,
    CPU_666MHz_DDR_333MHz_L2_333MHz,
    CPU_666MHz_DDR_444MHz_L2_333MHz,
    CPU_666MHz_DDR_666MHz_L2_333MHz,
    CPU_800MHz_DDR_267MHz_L2_400MHz,
    CPU_800MHz_DDR_400MHz_L2_400MHz,
    CPU_800MHz_DDR_534MHz_L2_400MHz,
    CPU_900MHz_DDR_300MHz_L2_450MHz,
    CPU_900MHz_DDR_450MHz_L2_450MHz,
    CPU_900MHz_DDR_600MHz_L2_450MHz,
    CPU_1000MHz_DDR_500MHz_L2_500MHz,
    CPU_1000MHz_DDR_667MHz_L2_500MHz,
    CPU_1000MHz_DDR_500MHz_L2_333MHz,
    CPU_400MHz_DDR_400MHz_L2_400MHz,
    CPU_1100MHz_DDR_550MHz_L2_550MHz,
    CPU_1200MHz_DDR_600MHz_L2_600MHz,

    LAST_FREQ_SUP

}DDR3_CPU_FREQ;


#endif /* _INC_ALP_VARS_H */
