/*******************************************************************************
Copyright (C) 2016 Marvell International Ltd.

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the three
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 2 of the License, or any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************************
Marvell GNU General Public License FreeRTOS Exception

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the Lesser
General Public License Version 2.1 plus the following FreeRTOS exception.
An independent module is a module which is not derived from or based on
FreeRTOS.
Clause 1:
Linking FreeRTOS statically or dynamically with other modules is making a
combined work based on FreeRTOS. Thus, the terms and conditions of the GNU
General Public License cover the whole combination.
As a special exception, the copyright holder of FreeRTOS gives you permission
to link FreeRTOS with independent modules that communicate with FreeRTOS solely
through the FreeRTOS API interface, regardless of the license terms of these
independent modules, and to copy and distribute the resulting combined work
under terms of your choice, provided that:
1. Every copy of the combined work is accompanied by a written statement that
details to the recipient the version of FreeRTOS used and an offer by yourself
to provide the FreeRTOS source code (including any modifications you may have
made) should the recipient request it.
2. The combined work is not itself an RTOS, scheduler, kernel or related
product.
3. The independent modules add significant and primary functionality to
FreeRTOS and do not merely extend the existing functionality already present in
FreeRTOS.
Clause 2:
FreeRTOS may not be used for any competitive or comparative purpose, including
the publication of any form of run time or compile time metric, without the
express permission of Real Time Engineers Ltd. (this is the norm within the
industry and is intended to ensure information accuracy).

********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

	* Neither the name of Marvell nor the names of its contributors may be
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

#ifndef _DDR_ML_WRAPPER_H
#define _DDR_ML_WRAPPER_H

#include <common.h>
#include <i2c.h>
#include <spl.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/soc.h>

#include "ddr3_hws_hw_training_def.h"

/*
 * MV_DEBUG_INIT need to be defines, otherwise the output of the
 * DDR2 training code is not complete and misleading
 */
#define MV_DEBUG_INIT

#ifdef MV_DEBUG_INIT
#define DEBUG_INIT_S(s)			puts(s)
#define DEBUG_INIT_D(d, l)		printf("%x", d)
#define DEBUG_INIT_D_10(d, l)		printf("%d", d)
#else
#define DEBUG_INIT_S(s)
#define DEBUG_INIT_D(d, l)
#define DEBUG_INIT_D_10(d, l)
#endif

#ifdef MV_DEBUG_INIT_FULL
#define DEBUG_INIT_FULL_S(s)		puts(s)
#define DEBUG_INIT_FULL_D(d, l)		printf("%x", d)
#define DEBUG_INIT_FULL_D_10(d, l)	printf("%d", d)
#define DEBUG_WR_REG(reg, val) \
	{ DEBUG_INIT_S("Write Reg: 0x"); DEBUG_INIT_D((reg), 8); \
	  DEBUG_INIT_S("= "); DEBUG_INIT_D((val), 8); DEBUG_INIT_S("\n"); }
#define DEBUG_RD_REG(reg, val) \
	{ DEBUG_INIT_S("Read  Reg: 0x"); DEBUG_INIT_D((reg), 8); \
	  DEBUG_INIT_S("= "); DEBUG_INIT_D((val), 8); DEBUG_INIT_S("\n"); }
#else
#define DEBUG_INIT_FULL_S(s)
#define DEBUG_INIT_FULL_D(d, l)
#define DEBUG_INIT_FULL_D_10(d, l)
#define DEBUG_WR_REG(reg, val)
#define DEBUG_RD_REG(reg, val)
#endif

#define DEBUG_INIT_FULL_C(s, d, l)			\
	{ DEBUG_INIT_FULL_S(s);				\
	  DEBUG_INIT_FULL_D(d, l);			\
	  DEBUG_INIT_FULL_S("\n"); }
#define DEBUG_INIT_C(s, d, l) \
	{ DEBUG_INIT_S(s); DEBUG_INIT_D(d, l); DEBUG_INIT_S("\n"); }

/*
 * Debug (Enable/Disable modules) and Error report
 */

#ifdef BASIC_DEBUG
#define MV_DEBUG_WL
#define MV_DEBUG_RL
#define MV_DEBUG_DQS_RESULTS
#endif

#ifdef FULL_DEBUG
#define MV_DEBUG_WL
#define MV_DEBUG_RL
#define MV_DEBUG_DQS

#define MV_DEBUG_PBS
#define MV_DEBUG_DFS
#define MV_DEBUG_MAIN_FULL
#define MV_DEBUG_DFS_FULL
#define MV_DEBUG_DQS_FULL
#define MV_DEBUG_RL_FULL
#define MV_DEBUG_WL_FULL
#endif


/* The following is a list of Marvell status */
#define MV_ERROR	(-1)
#define MV_OK		(0x00)	/* Operation succeeded                   */
#define MV_FAIL		(0x01)	/* Operation failed                      */
#define MV_BAD_VALUE	(0x02)	/* Illegal value (general)               */
#define MV_OUT_OF_RANGE	(0x03)	/* The value is out of range             */
#define MV_BAD_PARAM	(0x04)	/* Illegal parameter in function called  */
#define MV_BAD_PTR	(0x05)	/* Illegal pointer value                 */
#define MV_BAD_SIZE	(0x06)	/* Illegal size                          */
#define MV_BAD_STATE	(0x07)	/* Illegal state of state machine        */
#define MV_SET_ERROR	(0x08)	/* Set operation failed                  */
#define MV_GET_ERROR	(0x09)	/* Get operation failed                  */
#define MV_CREATE_ERROR	(0x0a)	/* Fail while creating an item           */
#define MV_NOT_FOUND	(0x0b)	/* Item not found                        */
#define MV_NO_MORE	(0x0c)	/* No more items found                   */
#define MV_NO_SUCH	(0x0d)	/* No such item                          */
#define MV_TIMEOUT	(0x0e)	/* Time Out                              */
#define MV_NO_CHANGE	(0x0f)	/* Parameter(s) is already in this value */
#define MV_NOT_SUPPORTED (0x10)	/* This request is not support           */
#define MV_NOT_IMPLEMENTED (0x11) /* Request supported but not implemented*/
#define MV_NOT_INITIALIZED (0x12) /* The item is not initialized          */
#define MV_NO_RESOURCE	(0x13)	/* Resource not available (memory ...)   */
#define MV_FULL		(0x14)	/* Item is full (Queue or table etc...)  */
#define MV_EMPTY	(0x15)	/* Item is empty (Queue or table etc...) */
#define MV_INIT_ERROR	(0x16)	/* Error occured while INIT process      */
#define MV_HW_ERROR	(0x17)	/* Hardware error                        */
#define MV_TX_ERROR	(0x18)	/* Transmit operation not succeeded      */
#define MV_RX_ERROR	(0x19)	/* Recieve operation not succeeded       */
#define MV_NOT_READY	(0x1a)	/* The other side is not ready yet       */
#define MV_ALREADY_EXIST (0x1b)	/* Tried to create existing item         */
#define MV_OUT_OF_CPU_MEM   (0x1c) /* Cpu memory allocation failed.      */
#define MV_NOT_STARTED	(0x1d)	/* Not started yet                       */
#define MV_BUSY		(0x1e)	/* Item is busy.                         */
#define MV_TERMINATE	(0x1f)	/* Item terminates it's work.            */
#define MV_NOT_ALIGNED	(0x20)	/* Wrong alignment                       */
#define MV_NOT_ALLOWED	(0x21)	/* Operation NOT allowed                 */
#define MV_WRITE_PROTECT (0x22)	/* Write protected                       */
#define MV_INVALID	(int)(-1)

/*
 * Accessor functions for the registers
 */
static inline void reg_write(u32 addr, u32 val)
{
	writel(val, INTER_REGS_BASE + addr);
}

static inline u32 reg_read(u32 addr)
{
	return readl(INTER_REGS_BASE + addr);
}

static inline void reg_bit_set(u32 addr, u32 mask)
{
	setbits_le32(INTER_REGS_BASE + addr, mask);
}

static inline void reg_bit_clr(u32 addr, u32 mask)
{
	clrbits_le32(INTER_REGS_BASE + addr, mask);
}

#endif /* _DDR_ML_WRAPPER_H */
