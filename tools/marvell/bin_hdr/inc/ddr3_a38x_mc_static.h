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
} MV_DRAM_MC_INIT;


MV_DRAM_MC_INIT ddr3_a38x_667[MV_MAX_DDR3_STATIC_SIZE] =
{
	{0x0001400, 0x7B00Ca28},   /* DDR SDRAM Configuration Register             */
	{0x0001404, 0x36301820},   /* Dunit Control Low Register - kw40 bit11 high */
	{0x0001408, 0x43149997},   /* DDR SDRAM Timing (Low) Register              */
	{0x000140C, 0x38411bc7},   /* DDR SDRAM Timing (High) Register             */
	{0x0001410, 0x14300000},   /* DDR SDRAM Address Control Register           */
	{0x0001414, 0x00000700},   /* DDR SDRAM Open Pages Control Register        */
	{0x0001424, 0x0060f3ff},   /* Dunit Control High Register ( 2 :1 - bits 15:12 = 0xD )  */
	{0x0001428, 0x000F8830},   /* Dunit Control High Register                              */
	{0x000142C, 0x028c50f8},   /* Dunit Control High Register  ( 2:1 -  bit 29 = '1' )     */
	{0x000147C, 0x0000c671},   /*  */
	{0x0001494, 0x00030000},  /* DDR SDRAM ODT Control (Low) Register  */
	{0x000149C, 0x00000300},  /* DDR Dunit ODT Control Register  */
	{0x00014a8, 0x00000000},
	{0x00014cc, 0xbd09000d},
	{0x0001474, 0x00000000},
	{0x0001538, 0x00000009},  /* Read Data Sample Delays Register */
	{0x000153C, 0x0000000c},  /* Read Data Ready Delay Register  */
	{0x0001504, 0xFFFFFFF1},
	{0x000150c, 0xFFFFFFE5},
	{0x0001514, 0x00000000},
	{0x000151c, 0x00000000},
	{0x00015D0, 0x00000650},  /* MR0 */
	{0x00015D4, 0x00000046},  /* MR1 */
	{0x00015D8, 0x00000010},  /* MR2 */
	{0x00015DC, 0x00000000},  /* MR3 */
	{0x00015E0, 0x23},
	{0x00015E4, 0x00203c18}, /* ZQC Configuration Register  */
	{0x00015EC, 0xf8000019}, /* DDR PHY                     */
	{0x00016A0, 0xe8243dfe},    /* ZNR / SPR                */
	{0x00016A0, 0xe8280434},   /* disable clamp and Vref   */
	{0x00016A0, 0x281020da},   /* Clock skew               */
	{0x00016A0, 0xe8260cb2},
	{0x00016A0, 0xe8290000},
	{0x00016A0, 0xf810001f},
	{0x00016A0, 0xCC000012},
	{0x00016A0, 0xC803000f},
	{0x0, 0x0}
};

MV_DRAM_MC_INIT ddr3_a38x_800[MV_MAX_DDR3_STATIC_SIZE] =
{
	{0x00001400, 0x7B00CC30},
	{0x00001404, 0x36301820},
	{0x00001408, 0x5415BAAB},
	{0x0000140C, 0x38411DEF},
	{0x00001410, 0x18300000},
	{0x00001414, 0x00000700},
	{0x00001424, 0x0060f3ff},
	{0x00001428, 0x0011A940},
	{0x0000142C, 0x28c5134},
	{0x0000147C, 0x0000D771},
	{0x00001494, 0x00030000},
	{0x0000149C, 0x00000300},
	{0x000014a8, 0x00000000},
	{0x000014cc, 0xbd09000d},
	{0x00001474, 0x00000000},
	{0x00001538, 0x0000000b},
	{0x0000153C, 0x0000000c},
	{0x00001504, 0xFFFFFFF1},
	{0x0000150c, 0xFFFFFFE5},
	{0x00001514, 0x00000000},
	{0x0000151c, 0x0},
	{0x000015D0, 0x00000670},
	{0x000015D4, 0x00000046},
	{0x000015D8, 0x00000018},
	{0x000015DC, 0x00000000},
	{0x000015E0, 0x23},
	{0x000015E4, 0x00203c18},
	{0x000015EC, 0xf8000019},
	{0x00016A0, 0xe8243dfe},    /* ZNR / SPR                */
	{0x00016A0, 0xe8280434},   /* disable clamp and Vref   */
	{0x00016A0, 0x281020da},   /* Clock skew               */
	{0x00016A0, 0xe8260cb2},
	{0x00016A0, 0xe8290000},
	{0x00016A0, 0xf810001f},
	{0x00016A0, 0xCC000012},
	{0x00016A0, 0xC803000f},
	{0x0, 0x0}
};

#endif /* _INC_A375_MC_STATIC_H */
