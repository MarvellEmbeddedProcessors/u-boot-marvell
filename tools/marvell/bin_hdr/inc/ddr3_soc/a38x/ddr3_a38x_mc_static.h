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

#ifndef _INC_A375_MC_STATIC_H
#define _INC_A375_MC_STATIC_H

#include "ddr3_a38x.h"

typedef struct __mvDramMcInit {
	MV_U32 reg_addr;
	MV_U32 reg_value;
	MV_U32 reg_mask;
} MV_DRAM_MC_INIT;

#ifdef SUPPORT_STATIC_DUNIT_CONFIG

#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
static MV_DRAM_MC_INIT ddr3_customer_800[] =
{
/*parameters for customer board (based on 800MHZ)*/

	{0x1400,	0x7B00CC30, 0xffffffff},
	{0x1404,	0x36301820, 0xffffffff},
	{0x1408,	0x5415BAAB, 0xffffffff},
	{0x140C,	0x38411DEF, 0xffffffff},
	{0x1410,	0x18300000, 0xffffffff},
	{0x1414,	0x00000700, 0xffffffff},
	{0x1424,	0x0060f3ff, 0xffffffff},
	{0x1428,	0x0011A940, 0xffffffff},
	{0x142C,	0x28c5134,  0xffffffff},
	{0x1474,	0x00000000, 0xffffffff},
	{0x147C,	0x0000D771, 0xffffffff},
	{0x1494,	0x00030000, 0xffffffff},
	{0x149C,	0x00000300, 0xffffffff},
	{0x14A8,	0x00000000, 0xffffffff},
	{0x14cc,	0xbd09000d, 0xffffffff},
	{0x1504,	0xFFFFFFF1, 0xffffffff},
	{0x150C,	0xFFFFFFE5, 0xffffffff},
	{0x1514,	0x00000000, 0xffffffff},
	{0x151C,	0x00000000, 0xffffffff},
	{0x1538,	0x00000B0B, 0xffffffff},
	{0x153C,	0x00000C0C, 0xffffffff},
	{0x15D0,	0x00000670, 0xffffffff},
	{0x15D4,	0x00000046, 0xffffffff},
	{0x15D8,	0x00000010, 0xffffffff},
	{0x15DC,	0x00000000, 0xffffffff},
	{0x15E0,	0x00000023, 0xffffffff},
	{0x15E4,	0x00203c18, 0xffffffff},
	{0x15EC,	0xf8000019, 0xffffffff},
	{0x16A0,	0xCC000006, 0xffffffff}, //  Clock Delay
	{0xE4124,   0x08008073, 0xffffffff},  //  AVS BG default
	{0,   0, 0}
};

#else /* CONFIG_CUSTOMER_BOARD_SUPPORT */
MV_DRAM_MC_INIT ddr3_a38x_933[MV_MAX_DDR3_STATIC_SIZE] =
{
/*parameters for 933MHZ*/
	{0x1400,	0x7B00CE3A, 0xffffffff},
	{0x1404,	0x36301820, 0xffffffff},
	{0x1408,	0x7417ECCF, 0xffffffff},
	{0x140C,	0x3E421F98, 0xffffffff},
	{0x1410,	0x1A300000, 0xffffffff},
	{0x1414,	0x00000700, 0xffffffff},
	{0x1424,	0x0060F3FF, 0xffffffff},
	{0x1428,	0x0013CA50, 0xffffffff},
	{0x142C,	0x028C5165, 0xffffffff},
	{0x1474,	0x00000000, 0xffffffff},
	{0x147C,	0x0000E871, 0xffffffff},
	{0x1494,	0x00010000, 0xffffffff},
	{0x149C,	0x00000001, 0xffffffff},
	{0x14A8,	0x00000000, 0xffffffff},
	{0x14cc,	0xbd09000d, 0xffffffff},
	{0x1504,	0xFFFFFFE1, 0xffffffff},
	{0x150C,	0xFFFFFFE5, 0xffffffff},
	{0x1514,	0x00000000, 0xffffffff},
	{0x151C,	0x00000000, 0xffffffff},
	{0x1538,	0x00000D0D, 0xffffffff},
	{0x153C,	0x00000D0D, 0xffffffff},
	{0x15D0,	0x00000608, 0xffffffff},
	{0x15D4,	0x00000044, 0xffffffff},
	{0x15D8,	0x00000020, 0xffffffff},
	{0x15DC,	0x00000000, 0xffffffff},
	{0x15E0,	0x00000021, 0xffffffff},
	{0x15E4,	0x00203C18, 0xffffffff},
	{0x15EC,	0xf8000019, 0xffffffff},
	{0x16A0,	0xCC000006, 0xffffffff}, //  Clock Delay
	{0xE4124,   0x08008073, 0xffffffff},  //  AVS BG default
	{0,   0, 0}
};


static MV_DRAM_MC_INIT ddr3_a38x_800[] =
{
/*parameters for 800MHZ*/

	{0x1400,	0x7B00CC30, 0xffffffff},
	{0x1404,	0x36301820, 0xffffffff},
	{0x1408,	0x5415BAAB, 0xffffffff},
	{0x140C,	0x38411DEF, 0xffffffff},
	{0x1410,	0x18300000, 0xffffffff},
	{0x1414,	0x00000700, 0xffffffff},
	{0x1424,	0x0060f3ff, 0xffffffff},
	{0x1428,	0x0011A940, 0xffffffff},
	{0x142C,	0x28c5134,  0xffffffff},
	{0x1474,	0x00000000, 0xffffffff},
	{0x147C,	0x0000D771, 0xffffffff},
	{0x1494,	0x00030000, 0xffffffff},
	{0x149C,	0x00000300, 0xffffffff},
	{0x14A8,	0x00000000, 0xffffffff},
	{0x14cc,	0xbd09000d, 0xffffffff},
	{0x1504,	0xFFFFFFF1, 0xffffffff},
	{0x150C,	0xFFFFFFE5, 0xffffffff},
	{0x1514,	0x00000000, 0xffffffff},
	{0x151C,	0x00000000, 0xffffffff},
	{0x1538,	0x00000B0B, 0xffffffff},
	{0x153C,	0x00000C0C, 0xffffffff},
	{0x15D0,	0x00000670, 0xffffffff},
	{0x15D4,	0x00000046, 0xffffffff},
	{0x15D8,	0x00000010, 0xffffffff},
	{0x15DC,	0x00000000, 0xffffffff},
	{0x15E0,	0x00000023, 0xffffffff},
	{0x15E4,	0x00203c18, 0xffffffff},
	{0x15EC,	0xf8000019, 0xffffffff},
	{0x16A0,	0xCC000006, 0xffffffff}, //  Clock Delay
	{0xE4124,   0x08008073, 0xffffffff},  //  AVS BG default
	{0,   0, 0}
};


static MV_DRAM_MC_INIT ddr3_a38x_667[] =
{
	/*parameters for 667MHZ*/

	{0x1400,    0x7B00Ca28, 0xffffffff}, // DDR SDRAM Configuration Register
	{0x1404,    0x36301820, 0xffffffff}, // Dunit Control Low Register - kw28 bit12 low (disable CLK1)
	{0x1408,    0x43149997, 0xffffffff}, // DDR SDRAM Timing (Low) Register
	{0x140C,    0x38411bc7, 0xffffffff}, //  DDR SDRAM Timing (High) Register
	{0x1410,    0x14330000, 0xffffffff}, //  DDR SDRAM Address Control Register
	{0x1414,    0x00000700, 0xffffffff}, //  DDR SDRAM Open Pages Control Register
	//{0x1418,    0x00000e00, 0xffffffff}, //  DDR SDRAM Operation Register
	//{0x141C,    0x00000672, 0xffffffff}, //  DDR SDRAM Mode Register
	//{0x1420,    0x00000004, 0xffffffff}, //  DDR SDRAM Extended Mode Register
	{0x1424,    0x0060f3ff, 0xffffffff}, //  Dunit Control High Register ( 2 :1 - bits 15:12 = 0xD )
	{0x1428,    0x000F8830, 0xffffffff}, //  Dunit Control High Register
	{0x142C,    0x28c50f8,  0xffffffff}, //  Dunit Control High Register  ( 2:1 -  bit 29 = '1' )
	{0x147C,    0x0000c671, 0xffffffff}, //
	//{0x14a8,    0x00000100, 0xffffffff}, // DSMP "101"
	//{0x20220,   0x00000006, 0xffffffff}, // DSMP 7
	{0x1494,    0x00030000, 0xffffffff}, //  DDR SDRAM ODT Control (Low) Register
	{0x1498,    0x00000000, 0xffffffff}, //  DDR SDRAM ODT Control (High) Register //will be configured at WL
	//{0x149C,    0x00000003, 0xffffffff}, //  DDR Dunit ODT Control Register
	{0x149C,    0x00000300, 0xffffffff}, //  DDR Dunit ODT Control Register
	{0x14a8,    0x00000000, 0xffffffff}, //
	{0x14cc,    0xbd09000d, 0xffffffff}, //
	{0x1474,    0x00000000, 0xffffffff}, //
	{0x1538,    0x00000009, 0xffffffff}, //  Read Data Sample Delays Register
	{0x153C,    0x0000000c, 0xffffffff}, //  Read Data Ready Delay Register
	{0x1504,    0xFFFFFFF1, 0xffffffff}, //
	{0x150c,    0xFFFFFFE5, 0xffffffff}, //
	{0x1514,    0x00000000, 0xffffffff}, //
	{0x151c,    0x0, 		0xffffffff}, //
	{0x15D0,    0x00000650, 0xffffffff}, //  MR0
	{0x15D4,    0x00000046, 0xffffffff}, //  MR1
	{0x15D8,    0x00000010, 0xffffffff}, //  MR2
	{0x15DC,    0x00000000, 0xffffffff}, //  MR3
	{0x15E0,    0x23, 		0xffffffff}, //
	{0x15E4,    0x00203c18, 0xffffffff}, //  ZQC Configuration Register
	{0x15EC,    0xf8000019, 0xffffffff}, //  DDR PHY
	{0x16A0,    0xCC000006, 0xffffffff}, //  Clock Delay
	{0xE4124,   0x08008073, 0xffffffff}, //  AVS BG default
	{0,   0, 0}
};

static MV_DRAM_MC_INIT ddr3_a38x_533[] =
{
	/*parameters for 533MHZ*/

	{0x1400,    0x7B00D040, 0xffffffff}, // DDR SDRAM Configuration Register
	{0x1404,    0x36301820, 0xffffffff}, // Dunit Control Low Register - kw28 bit12 low (disable CLK1)
	{0x1408,    0x33137772, 0xffffffff}, // DDR SDRAM Timing (Low) Register
	{0x140C,    0x3841199F, 0xffffffff}, //  DDR SDRAM Timing (High) Register
	{0x1410,    0x10330000, 0xffffffff}, //  DDR SDRAM Address Control Register
	{0x1414,    0x00000700, 0xffffffff}, //  DDR SDRAM Open Pages Control Register
	{0x1424,    0x0060F3FF, 0xffffffff}, //  Dunit Control High Register ( 2 :1 - bits 15:12 = 0xD )
	{0x1428,    0x000D6720, 0xffffffff}, //  Dunit Control High Register
	{0x142C,    0x028C50C3,  0xffffffff}, //  Dunit Control High Register  ( 2:1 -  bit 29 = '1' )
	{0x147C,    0x0000B571, 0xffffffff}, //
	{0x1494,    0x00030000, 0xffffffff}, //  DDR SDRAM ODT Control (Low) Register
	{0x1498,    0x00000000, 0xffffffff}, //  DDR SDRAM ODT Control (High) Register //will be configured at WL
	{0x149C,    0x00000003, 0xffffffff}, //  DDR Dunit ODT Control Register
	{0x14a8,    0x00000000, 0xffffffff}, //
	{0x14cc,    0xbd09000d, 0xffffffff}, //
	{0x1474,    0x00000000, 0xffffffff}, //
	{0x1538,    0x00000707, 0xffffffff}, //  Read Data Sample Delays Register
	{0x153C,    0x00000707, 0xffffffff}, //  Read Data Ready Delay Register
	{0x1504,    0xFFFFFFE1, 0xffffffff}, //
	{0x150c,    0xFFFFFFE5, 0xffffffff}, //
	{0x1514,    0x00000000, 0xffffffff}, //
	{0x151c,    0x00000000,	0xffffffff}, //
	{0x15D0,    0x00000630, 0xffffffff}, //  MR0
	{0x15D4,    0x00000046, 0xffffffff}, //  MR1
	{0x15D8,    0x00000008, 0xffffffff}, //  MR2
	{0x15DC,    0x00000000, 0xffffffff}, //  MR3
	{0x15E0,    0x00000023,	0xffffffff}, //
	{0x15E4,    0x00203c18, 0xffffffff}, //  ZQC Configuration Register
	{0x15EC,    0xF8000019, 0xffffffff}, //  DDR PHY
	{0x16A0,    0xCC000006, 0xffffffff}, //  Clock Delay
	{0xE4124,   0x08008073, 0xffffffff}, //  AVS BG default
	{0,   0, 0}
};

#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */

#endif /*SUPPORT_STATIC_DUNIT_CONFIG*/

#endif /* _INC_A375_MC_STATIC_H */

