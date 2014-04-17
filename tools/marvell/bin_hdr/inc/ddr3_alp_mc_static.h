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

#ifndef _INC_ALP_MC_STATIC_H
#define _INC_ALP_MC_STATIC_H

typedef struct __mvDramMcInit {
	MV_U32 reg_addr;
	MV_U32 reg_value;
} MV_DRAM_MC_INIT;

#if defined(CONFIG_CUSTOMER_BOARD_SUPPORT)

MV_DRAM_MC_INIT ddr3_costumer_0_533[MV_MAX_DDR3_STATIC_SIZE] =
{
    {0x00001400, 0x7B00C820}, 	/*DDR SDRAM Configuration Register */
    {0x00001404, 0x36300820}, 	/*Dunit Control Low Register */
    {0x00001408, 0x33137772}, 	/*DDR SDRAM Timing (Low) Register */
    {0x0000140C, 0x3841199F}, 	/*DDR SDRAM Timing (High) Register */
    {0x00001410, 0x16100011}, 	/*DDR SDRAM Open Pages Control Register */
    {0x00001414, 0x00000700}, 	/*DDR SDRAM Open Pages Control Register */

    {0x00001424, 0x0060F3FF}, 	/*Dunit Control High Register */
    {0x00001428, 0x000D6720}, 	/*DDR ODT Timing (Low) Register */
    {0x0000142C, 0x028C50C3}, 	/*DDR3 Timing Register */
    {0x0000147C, 0x0000B571},  /*DDR ODT Timing (High) Register */
    {0x00001494, 0x00010000},  /*SDRAM_SRAM_ODT_CNTL_LOW */
    {0x0000149C, 0x00000301},  /*SDRAM_ODT_CONTROL */

    {0x000014a8, 0x00000000}, 	/*AXI Control Register */
    {0x00001504, 0x7FFFFFE1}, 	/* CS0 Size */
    {0x0000150C, 0x7FFFFFE5}, 	/* CS1 Size */
    {0x00001514, 0x00000000}, 	/* CS2 Size */
    {0x0000151C, 0x00000000}, 	/* CS3 Size */
    {0x00001538, 0x00000007}, 	/*Read Data Sample Delays Register */
    {0x0000153C, 0x00000007}, 	/*Read Data Ready Delay Register */

    {0x000015D0, 0x00000630}, 	/*MR0 */
    {0x000015D4, 0x00000044}, 	/*MR1 */
    {0x000015D8, 0x00000008}, 	/*MR2 */
    {0x000015DC, 0x00000000}, 	/*MR3 */

    {0x000015E0, 0x00000001}, 	/*DDR3 Rank Control Register */
    {0x000015E4, 0x00203c18}, 	/*ZQC Configuration Register */
#ifdef CONFIG_ALP_A375_ZX_REV
    {0x000015EC, 0xF8000f25}, 	/*DDR PHY */
#else
	{0x000015EC, 0xF8000025}, 	/*DDR PHY */
#endif

    {0x000014C0, 0x192424C9}, 	/*DRAM address and Control Driving Strenght */
    {0x000014C4, 0x192424C9}, 	/*DRAM Data and DQS Driving Strenght */
    {0x0, 0x0}
};

#else /* CONFIG_CUSTOMER_BOARD_SUPPORT */

/* DB-88F6650 (Avanta_LP_16 bit) 533MHz  */
MV_DRAM_MC_INIT ddr3_Z0_db_88F6650_533[MV_MAX_DDR3_STATIC_SIZE] =
{
    {0x00001400, 0x7B004820}, 	/*DDR SDRAM Configuration Register */
    {0x00001404, 0x36301820}, 	/*Dunit Control Low Register */
    {0x00001408, 0x33137772}, 	/*DDR SDRAM Timing (Low) Register */
    {0x0000140C, 0x384019D5}, 	/*DDR SDRAM Timing (High) Register */
    {0x00001410, 0x10000000}, 	/*DDR SDRAM Open Pages Control Register */
    {0x00001414, 0x00000700}, 	/*DDR SDRAM Open Pages Control Register */

    {0x00001424, 0x0060F3FF}, 	/*Dunit Control High Register */
    {0x00001428, 0x000D6720}, 	/*DDR ODT Timing (Low) Register */
    {0x0000142C, 0x028C50C3}, 	/*DDR3 Timing Register */
    {0x0000147C, 0x0000B571},  /*DDR ODT Timing (High) Register */
    {0x00001494, 0x00030000},  /*SDRAM_SRAM_ODT_CNTL_LOW */
    {0x0000149C, 0x00000303},  /*SDRAM_ODT_CONTROL */

    {0x000014a8, 0x00000000}, 	/*AXI Control Register */
    {0x00001504, 0x3FFFFFE1}, 	/* CS0 Size */
    {0x0000150C, 0x3FFFFFE5}, 	/* CS1 Size */
    {0x00001514, 0x00000000}, 	/* CS2 Size */
    {0x0000151C, 0x00000000}, 	/* CS3 Size */
    {0x00001538, 0x00000707}, 	/*Read Data Sample Delays Register */
    {0x0000153C, 0x00000707}, 	/*Read Data Ready Delay Register */

    {0x000015D0, 0x00000630}, 	/*MR0 */
    {0x000015D4, 0x00000046}, 	/*MR1 */
    {0x000015D8, 0x00000008}, 	/*MR2 */
    {0x000015DC, 0x00000000}, 	/*MR3 */

    {0x000015E0, 0x00000023/*0x00000003*/}, 	/*DDR3 Rank Control Register */
    {0x000015E4, 0x00203c18}, 	/*ZQC Configuration Register */
#ifdef CONFIG_ALP_A375_ZX_REV
    {0x000015EC, 0xF8000f25}, 	/*DDR PHY */
#else
	{0x000015EC, 0xF8000025}, 	/*DDR PHY */
#endif

    {0x000014C0, 0x192434E9}, 	/*DRAM address and Control Driving Strenght */
    {0x000014C4, 0x192434E9}, 	/*DRAM Data and DQS Driving Strenght */
    {0x0, 0x0}
};

/* RD-88F6650 (Avanta_LP_16 bit) 400MHz  */
MV_DRAM_MC_INIT ddr3_Z0_rd_88F6650_400[MV_MAX_DDR3_STATIC_SIZE] =
{
    {0x00001400, 0x7B004618}, 	/*DDR SDRAM Configuration Register */
    {0x00001404, 0x36300820}, 	/*Dunit Control Low Register */
    {0x00001408, 0x3303555D}, 	/*DDR SDRAM Timing (Low) Register */
    {0x0000140C, 0x384019BF}, 	/*DDR SDRAM Timing (High) Register */
    {0x00001410, 0x10000001}, 	/*DDR SDRAM Open Pages Control Register */
    {0x00001414, 0x00000700}, 	/*DDR SDRAM Open Pages Control Register */

    {0x00001424, 0x0060F3FF}, 	/*Dunit Control High Register */
    {0x00001428, 0x000C5720}, 	/*DDR ODT Timing (Low) Register */
    {0x0000142C, 0x028C5093}, 	/*DDR3 Timing Register */
    {0x0000147C, 0x0000A471},  /*DDR ODT Timing (High) Register */
    {0x00001494, 0x00010000},  /*SDRAM_SRAM_ODT_CNTL_LOW */
    {0x0000149C, 0x00000301},  /*SDRAM_ODT_CONTROL */

    {0x000014a8, 0x00000000}, 	/*AXI Control Register */
    {0x00001504, 0x1FFFFFE1}, 	/* CS0 Size */
    {0x0000150C, 0x00000000}, 	/* CS1 Size */
    {0x00001514, 0x00000000}, 	/* CS2 Size */
    {0x0000151C, 0x00000000}, 	/* CS3 Size */
    {0x00001538, 0x00000006}, 	/*Read Data Sample Delays Register */
    {0x0000153C, 0x00000006}, 	/*Read Data Ready Delay Register */

    {0x000015D0, 0x00000620}, 	/*MR0 */
    {0x000015D4, 0x00000044}, 	/*MR1 */
    {0x000015D8, 0x00000000}, 	/*MR2 */
    {0x000015DC, 0x00000000}, 	/*MR3 */

    {0x000015E0, 0x00000001}, 	/*DDR3 Rank Control Register */
    {0x000015E4, 0x00203c18}, 	/*ZQC Configuration Register */
#ifdef CONFIG_ALP_A375_ZX_REV
    {0x000015EC, 0xF8000f25}, 	/*DDR PHY */
#else
	{0x000015EC, 0xF8000025}, 	/*DDR PHY */
#endif

    {0x000014C0, 0x192424C9}, 	/*DRAM address and Control Driving Strenght */
    {0x000014C4, 0x192424C9}, 	/*DRAM Data and DQS Driving Strenght */
    {0x0, 0x0}
};

/* DB-88F6660 (Avanta_LP_32 bit) 533MHz  */
MV_DRAM_MC_INIT ddr3_Z0_db_88F6660_533[MV_MAX_DDR3_STATIC_SIZE] =
{
    {0x00001400, 0x7B00C820}, 	/*DDR SDRAM Configuration Register */
    {0x00001404, 0x36301820}, 	/*Dunit Control Low Register */
    {0x00001408, 0x33137772}, 	/*DDR SDRAM Timing (Low) Register */
    {0x0000140C, 0x384019D5}, 	/*DDR SDRAM Timing (High) Register */
    {0x00001410, 0x10000000}, 	/*DDR SDRAM Open Pages Control Register */
    {0x00001414, 0x00000700}, 	/*DDR SDRAM Open Pages Control Register */

    {0x00001424, 0x0060F3FF}, 	/*Dunit Control High Register */
    {0x00001428, 0x000D6720}, 	/*DDR ODT Timing (Low) Register */
    {0x0000142C, 0x028C50C3}, 	/*DDR3 Timing Register */
    {0x0000147C, 0x0000B571},  /*DDR ODT Timing (High) Register */
    {0x00001494, 0x00030000},  /*SDRAM_SRAM_ODT_CNTL_LOW */
    {0x0000149C, 0x00000303},  /*SDRAM_ODT_CONTROL */

    {0x000014a8, 0x00000000}, 	/*AXI Control Register */
    {0x00001504, 0x7FFFFFE1}, 	/* CS0 Size */
    {0x0000150C, 0x7FFFFFE5}, 	/* CS1 Size */
    {0x00001514, 0x00000000}, 	/* CS2 Size */
    {0x0000151C, 0x00000000}, 	/* CS3 Size */
    {0x00001538, 0x00000707}, 	/*Read Data Sample Delays Register */
    {0x0000153C, 0x00000707}, 	/*Read Data Ready Delay Register */

    {0x000015D0, 0x00000630}, 	/*MR0 */
    {0x000015D4, 0x00000046}, 	/*MR1 */
    {0x000015D8, 0x00000008}, 	/*MR2 */
    {0x000015DC, 0x00000000}, 	/*MR3 */

    {0x000015E0, 0x00000023}, 	/*DDR3 Rank Control Register */
    {0x000015E4, 0x00203c18}, 	/*ZQC Configuration Register */
#ifdef CONFIG_ALP_A375_ZX_REV
    {0x000015EC, 0xF8000f25}, 	/*DDR PHY */
#else
	{0x000015EC, 0xF8000025}, 	/*DDR PHY */
#endif

    {0x000014C0, 0x192434E9}, 	/*DRAM address and Control Driving Strenght */
    {0x000014C4, 0x192434E9}, 	/*DRAM Data and DQS Driving Strenght */
    {0x0, 0x0}
};

/* RD-88F6660 (Avanta_LP_32 bit) 533MHz  */
MV_DRAM_MC_INIT ddr3_Z0_rd_88F6660_533[MV_MAX_DDR3_STATIC_SIZE] =
{
    {0x00001400, 0x7B00C820}, 	/*DDR SDRAM Configuration Register */
    {0x00001404, 0x36300820}, 	/*Dunit Control Low Register */
    {0x00001408, 0x33137772}, 	/*DDR SDRAM Timing (Low) Register */
    {0x0000140C, 0x3841199F}, 	/*DDR SDRAM Timing (High) Register */
    {0x00001410, 0x16100011}, 	/*DDR SDRAM Open Pages Control Register */
    {0x00001414, 0x00000700}, 	/*DDR SDRAM Open Pages Control Register */

    {0x00001424, 0x0060F3FF}, 	/*Dunit Control High Register */
    {0x00001428, 0x000D6720}, 	/*DDR ODT Timing (Low) Register */
    {0x0000142C, 0x028C50C3}, 	/*DDR3 Timing Register */
    {0x0000147C, 0x0000B571},  /*DDR ODT Timing (High) Register */
    {0x00001494, 0x00010000},  /*SDRAM_SRAM_ODT_CNTL_LOW */
    {0x0000149C, 0x00000301},  /*SDRAM_ODT_CONTROL */

    {0x000014a8, 0x00000000}, 	/*AXI Control Register */
    {0x00001504, 0x7FFFFFE1}, 	/* CS0 Size */
    {0x0000150C, 0x7FFFFFE5}, 	/* CS1 Size */
    {0x00001514, 0x00000000}, 	/* CS2 Size */
    {0x0000151C, 0x00000000}, 	/* CS3 Size */
    {0x00001538, 0x00000007}, 	/*Read Data Sample Delays Register */
    {0x0000153C, 0x00000007}, 	/*Read Data Ready Delay Register */

    {0x000015D0, 0x00000630}, 	/*MR0 */
    {0x000015D4, 0x00000044}, 	/*MR1 */
    {0x000015D8, 0x00000008}, 	/*MR2 */
    {0x000015DC, 0x00000000}, 	/*MR3 */

    {0x000015E0, 0x00000001}, 	/*DDR3 Rank Control Register */
    {0x000015E4, 0x00203c18}, 	/*ZQC Configuration Register */
#ifdef CONFIG_ALP_A375_ZX_REV
    {0x000015EC, 0xF8000f25}, 	/*DDR PHY */
#else
	{0x000015EC, 0xF8000025}, 	/*DDR PHY */
#endif

    {0x000014C0, 0x192424C9}, 	/*DRAM address and Control Driving Strenght */
    {0x000014C4, 0x192424C9}, 	/*DRAM Data and DQS Driving Strenght */
    {0x0, 0x0}
};

#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */
#endif /* _INC_ALP_MC_STATIC_H */

