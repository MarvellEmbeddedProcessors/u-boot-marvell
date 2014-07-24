
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
#ifndef _DDR3_A38X_H
#define _DDR3_A38X_H

#define MAX_INTERFACE_NUM  		(2)
#define MAX_BUS_NUM        		(5)

#include "ddr3_hws_hw_training_def.h"

#define REG_DEVICE_SAR1_ADDR                        0xE4204
#define RST2_CPU_DDR_CLOCK_SELECT_IN_OFFSET         17
#define RST2_CPU_DDR_CLOCK_SELECT_IN_MASK           0x1F

/* DRAM Windows */
#define REG_XBAR_WIN_5_CTRL_ADDR					0x20050
#define REG_XBAR_WIN_5_BASE_ADDR					0x20054

/* DRAM Windows */
#define REG_XBAR_WIN_4_CTRL_ADDR                0x20040
#define REG_XBAR_WIN_4_BASE_ADDR                0x20044
#define REG_XBAR_WIN_4_REMAP_ADDR               0x20048
#define REG_XBAR_WIN_7_REMAP_ADDR               0x20078
#define REG_XBAR_WIN_16_CTRL_ADDR               0x200d0
#define REG_XBAR_WIN_16_BASE_ADDR               0x200d4
#define REG_XBAR_WIN_16_REMAP_ADDR              0x200dc
#define REG_XBAR_WIN_19_CTRL_ADDR               0x200e8

#define REG_FASTPATH_WIN_BASE_ADDR(win)         (0x20180 + (0x8 * win))
#define REG_FASTPATH_WIN_CTRL_ADDR(win)         (0x20184 + (0x8 * win))

/*SatR defined too change topology busWidth and ECC configuration*/
#define DDR_SATR_CONFIG_MASK 						0x38
#define DDR_SATR_CONFIG_OFFSET 						3
#define DDR_SATR_16BIT_VALUE 						0b000
#define DDR_SATR_16BIT_ECC_PUP3_VALUE 				0b110
#define DDR_SATR_16BIT_ECC_PUP4_VALUE 				0b010
#define DDR_SATR_16BIT_NOT_VALID_ECC_VALUE 			0b100
#define DDR_SATR_32BIT_VALUE 						0b001
#define DDR_SATR_32BIT_ECC_VALUE 					0b011
#define DDR_SATR_32BIT_NOT_VALID_NO_ECC_VALUE 		0b101
#define DDR_SATR_32BIT_NOT_VALID_ECC_VALUE 			0b111

/********************/
/* Registers offset */
/********************/
#define	REG_SAMPLE_RESET_HIGH_ADDR				0x18600

#define MV_BOARD_REFCLK			MV_BOARD_REFCLK_25MHZ

/********************/
/* Registers offset */
/********************/
typedef enum
{
    CPU_1066MHz_DDR_400MHz,
    CPU_RESERVED_DDR_RESERVED0,
    CPU_667MHz_DDR_667MHz,
    CPU_800MHz_DDR_800MHz,
    CPU_RESERVED_DDR_RESERVED1,
    CPU_RESERVED_DDR_RESERVED2,
    CPU_RESERVED_DDR_RESERVED3,
    LAST_FREQ
}MSYS_DDR3_CPU_FREQ;

#define ACTIVE_INTERFACE_MASK			  0x1

#endif /* _DDR3_A38X_H */
