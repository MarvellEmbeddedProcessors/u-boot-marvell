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


#ifndef __INCmvAhbToMbusRegsh
#define __INCmvAhbToMbusRegsh

#define MAX_AHB_TO_MBUS_WINS			21
#define MAX_AHB_TO_MBUS_REMAP_WINS		8
#define MV_AHB_TO_MBUS_INTREG_WIN		20

/***********************/
/* AHB TO MBUS WINDOWS */
/***********************/
/* Window-X Control Registers */
#define AHB_TO_MBUS_WIN_CTRL_REG(winNum)	((winNum < MAX_AHB_TO_MBUS_REMAP_WINS) ? \
						 (MV_MBUS_REGS_OFFSET + winNum * 0x10) : \
						 (MV_MBUS_REGS_OFFSET + 0x90 + (winNum-8)*0x08))
#define ATMWCR_WIN_ENABLE			BIT0
#define ATMWCR_WIN_TARGET_OFFS			4
#define ATMWCR_WIN_TARGET_MASK			(0xf << ATMWCR_WIN_TARGET_OFFS)
#define ATMWCR_WIN_ATTR_OFFS			8
#define ATMWCR_WIN_ATTR_MASK			(0xff << ATMWCR_WIN_ATTR_OFFS)
#define ATMWCR_WIN_SIZE_OFFS			16
#define ATMWCR_WIN_SIZE_MASK			(0xffff << ATMWCR_WIN_SIZE_OFFS)
#define ATMWCR_WIN_SIZE_ALIGNMENT		0x10000

/* Window-X Base Register */
#define AHB_TO_MBUS_WIN_BASE_REG(winNum)	((winNum < MAX_AHB_TO_MBUS_REMAP_WINS) ? \
						 (MV_MBUS_REGS_OFFSET + 0x4 + winNum*0x10) : \
						 (MV_MBUS_REGS_OFFSET + 0x94 + (winNum-8)*0x08))
#define ATMWBR_BASE_OFFS			16
#define ATMWBR_BASE_MASK			(0xffff << 	ATMWBR_BASE_OFFS)
#define ATMWBR_BASE_ALIGNMENT			0x10000

/* Window-X Remap Low Register */
#define AHB_TO_MBUS_WIN_REMAP_LOW_REG(winNum)	((winNum < MAX_AHB_TO_MBUS_REMAP_WINS) ? \
						 (MV_MBUS_REGS_OFFSET + 0x8 + winNum*0x10) : \
						 (0))
#define ATMWRLR_REMAP_LOW_OFFS			16
#define ATMWRLR_REMAP_LOW_MASK			(0xffff << ATMWRLR_REMAP_LOW_OFFS)
#define ATMWRLR_REMAP_LOW_ALIGNMENT		0x10000

/* Window-X Remap Hi Register */
#define AHB_TO_MBUS_WIN_REMAP_HIGH_REG(winNum)	((winNum < MAX_AHB_TO_MBUS_REMAP_WINS) ? \
						 (MV_MBUS_REGS_OFFSET + 0xC + winNum*0x10) : \
						 (0))
#define ATMWRHR_REMAP_HIGH_OFFS			0
#define ATMWRHR_REMAP_HIGH_MASK			(0xffffffff << ATMWRHR_REMAP_HIGH_OFFS)

/*****************************/
/* INTERNAL REGISTERS WINDOW */
/*****************************/
/* Internal Registers Base Address in set to be window 20 */
#define AHB_TO_MBUS_WIN_INTEREG_REG		(MV_MBUS_REGS_OFFSET + 0x80)

/************************/
/* SDRAM DECODE WINDOWS */
/************************/
/* All DRAM Window definitions are declared under the ddr2_3 HAL */

/****************************/
/* SRAM (L2) DECODE WINDOWS */
/****************************/
#define SRAM_WIN_CTRL_REG(winNum)		(MV_MBUS_REGS_OFFSET + 0x240 + winNum * 0x4)
#define SRAMWCR_ENABLE				BIT0
#define SRAMWCR_SIZE_OFFS			8
#define SRAMWCR_SIZE_MASK			(0x7 << SRAMWCR_SIZE_OFFS)
#define SRAMWCR_BASE_OFFS			16
#define SRAMWCR_BASE_MASK			(0xFFFF << SRAMWCR_BASE_OFFS)

/**********************/
/* MBUS BRIDGE WINDOW */
/**********************/
#define MBUS_BRIDGE_WIN_CTRL_REG		(MV_MBUS_REGS_OFFSET + 0x250)
#define BRIDGWCR_ENABLE				BIT0
#define BRIDGWCR_SIZE_OFFS			16
#define BRIDGWCR_SIZE_MASK			(0xFFFF << BRIDGWCR_SIZE_OFFS)
#define MBUS_BRIDGE_WIN_BASE_REG		(MV_MBUS_REGS_OFFSET + 0x254)

#endif /* __INCmvAhbToMbusRegsh */

