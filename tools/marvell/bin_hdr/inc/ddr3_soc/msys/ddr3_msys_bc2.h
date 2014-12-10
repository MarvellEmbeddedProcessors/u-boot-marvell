
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
#ifndef _DDR3_MSYS_BC2_H
#define _DDR3_MSYS_BC2_H

#define MAX_INTERFACE_NUM  		(12)
#define MAX_BUS_NUM        		(8)

#include "ddr3_hws_hw_training_def.h"

/* MISC */
#define INTER_REGS_BASE								0xD0000000

#define REG_DEVICE_SAR1_ADDR                        0xF8200
#define REG_DEVICE_SAR2_ADDR                        0xF8204
#define REG_DEVICE_SERVER_CONTROL_0                 0xF8250
#define REG_DEVICE_SERVER_CONTROL_14                0xF8288
#define REG_DEVICE_SAR1_MSYS_TM_SDRAM_SEL_OFFSET    11
#define REG_DEVICE_SAR1_MSYS_TM_SDRAM_SEL_MASK      0x1


/* DRAM Windows */
#define REG_XBAR_WIN_5_CTRL_ADDR					0x20050
#define REG_XBAR_WIN_5_BASE_ADDR					0x20054

#define MV_78XX0_Z1_REV		0x0
#define MV_78XX0_A0_REV		0x1
#define MV_78XX0_B0_REV		0x2

/********************/
/* Registers offset */
/********************/

#define REG_SAMPLE_RESET_LOW_ADDR				0x18230
#define REG_SAMPLE_RESET_HIGH_ADDR				0x18234
#define	REG_SAMPLE_RESET_FAB_OFFS				24
#define	REG_SAMPLE_RESET_FAB_MASK				0xF000000
#define	REG_SAMPLE_RESET_TCLK_OFFS				28
#define	REG_SAMPLE_RESET_CPU_ARCH_OFFS			31
#define	REG_SAMPLE_RESET_HIGH_CPU_FREQ_OFFS		20

#define MV_BOARD_REFCLK			250000000

/* DDR3 Frequencies: */
#define DDR_100									0
#define DDR_300									1
#define DDR_333									1
#define DDR_360									2
#define DDR_400									3
#define DDR_444									4
#define DDR_500									5
#define DDR_533									6
#define DDR_600									7
#define DDR_640									8
#define DDR_666									8
#define DDR_720									9
#define DDR_750									9
#define DDR_800									10
#define DDR_833									11
#define DDR_HCLK								20
#define DDR_S									12
#define DDR_S_1TO1								13
#define MARGIN_FREQ 							DDR_400
#define DFS_MARGIN								DDR_100
/* #define DFS_MARGIN								DDR_400 */

#define ODT_OPT									16
#define ODT20									0x200
#define ODT30									0x204
#define ODT40									0x44
#define ODT120									0x40
#define ODT120D									0x400

#define MRS_DELAY								100

#define SDRAM_WL_SW_OFFS						0x100
#define SDRAM_RL_OFFS							0x0
#define SDRAM_PBS_I_OFFS						0x140
#define SDRAM_PBS_II_OFFS						0x180
#define SDRAM_PBS_NEXT_OFFS						(SDRAM_PBS_II_OFFS - SDRAM_PBS_I_OFFS)
#define SDRAM_PBS_TX_OFFS						0x180
#define SDRAM_PBS_TX_DM_OFFS					576
#define SDRAM_DQS_RX_OFFS						1024
#define SDRAM_DQS_TX_OFFS						2048
#define SDRAM_DQS_RX_SPECIAL_OFFS				5120

#define LEN_STD_PATTERN							16
#define LEN_KILLER_PATTERN						128
#define LEN_SPECIAL_PATTERN						128
#define LEN_PBS_PATTERN							16

/********************/
/* Registers offset */
/********************/

#define REG_SAMPLE_RESET_LOW_ADDR				0x18230
#define REG_SAMPLE_RESET_HIGH_ADDR				0x18234
#define	REG_SAMPLE_RESET_CPU_FREQ_OFFS			21
#define	REG_SAMPLE_RESET_CPU_FREQ_MASK			0x00E00000
#define	REG_SAMPLE_RESET_FAB_OFFS				24
#define	REG_SAMPLE_RESET_FAB_MASK				0xF000000
#define	REG_SAMPLE_RESET_TCLK_OFFS				28
#define	REG_SAMPLE_RESET_CPU_ARCH_OFFS			31
#define	REG_SAMPLE_RESET_HIGH_CPU_FREQ_OFFS		20

#define REG_FASTPATH_WIN_BASE_ADDR(win)         (0x20180 + (0x8 * win))
#define REG_FASTPATH_WIN_CTRL_ADDR(win)         (0x20184 + (0x8 * win))

typedef enum
{
    CPU_400MHz_DDR_400MHz,
    CPU_RESERVED_DDR_RESERVED0,
    CPU_667MHz_DDR_667MHz,
    CPU_800MHz_DDR_800MHz,
    CPU_RESERVED_DDR_RESERVED1,
    CPU_RESERVED_DDR_RESERVED2,
    CPU_RESERVED_DDR_RESERVED3,
    LAST_FREQ
}MSYS_DDR3_CPU_FREQ;

#define ACTIVE_INTERFACE_MASK			  0x10

/*SatR defined to change topology ECC configuration*/
#define DDR_SATR_ECC_CONFIG_MASK 						0x1
#define DDR_SATR_ECC_ENABLE_VALUE 						0b1

#endif /* _DDR3_MSYS_BC2_H */

