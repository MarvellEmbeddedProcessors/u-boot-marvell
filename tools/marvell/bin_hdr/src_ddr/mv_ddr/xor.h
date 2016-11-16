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

#ifndef _XOR_H
#define _XOR_H

#define SRAM_BASE		0x40000000

#include "ddr3_hws_hw_training_def.h"

#define MV_XOR_MAX_UNIT		2	/* XOR unit == XOR engine */
#define MV_XOR_MAX_CHAN		4	/* total channels for all units */
#define MV_XOR_MAX_CHAN_PER_UNIT 2	/* channels for units */

#define MV_IS_POWER_OF_2(num)	(((num) != 0) && (((num) & ((num) - 1)) == 0))

/*
 * This structure describes address space window. Window base can be
 * 64 bit, window size up to 4GB
 */
struct addr_win {
	u32 base_low;		/* 32bit base low       */
	u32 base_high;		/* 32bit base high      */
	u32 size;		/* 32bit size           */
};

/* This structure describes SoC units address decode window	*/
struct unit_win_info {
	struct addr_win addr_win;	/* An address window */
	int enable;		/* Address decode window is enabled/disabled  */
	u8 attrib;		/* chip select attributes */
	u8 target_id;		/* Target Id of this MV_TARGET */
};

/*
 * This enumerator describes the type of functionality the XOR channel
 * can have while using the same data structures.
 */
enum xor_type {
	MV_XOR,			/* XOR channel functions as XOR accelerator   */
	MV_DMA,			/* XOR channel functions as IDMA channel      */
	MV_CRC32		/* XOR channel functions as CRC 32 calculator */
};

enum mv_state {
	MV_IDLE,
	MV_ACTIVE,
	MV_PAUSED,
	MV_UNDEFINED_STATE
};

/*
 * This enumerator describes the set of commands that can be applied on
 * an engine (e.g. IDMA, XOR). Appling a comman depends on the current
 * status (see MV_STATE enumerator)
 *
 * Start can be applied only when status is IDLE
 * Stop can be applied only when status is IDLE, ACTIVE or PAUSED
 * Pause can be applied only when status is ACTIVE
 * Restart can be applied only when status is PAUSED
 */
enum mv_command {
	MV_START,		/* Start     */
	MV_STOP,		/* Stop     */
	MV_PAUSE,		/* Pause    */
	MV_RESTART		/* Restart  */
};

enum xor_override_target {
	SRC_ADDR0,		/* Source Address #0 Control */
	SRC_ADDR1,		/* Source Address #1 Control */
	SRC_ADDR2,		/* Source Address #2 Control */
	SRC_ADDR3,		/* Source Address #3 Control */
	SRC_ADDR4,		/* Source Address #4 Control */
	SRC_ADDR5,		/* Source Address #5 Control */
	SRC_ADDR6,		/* Source Address #6 Control */
	SRC_ADDR7,		/* Source Address #7 Control */
	XOR_DST_ADDR,		/* Destination Address Control */
	XOR_NEXT_DESC		/* Next Descriptor Address Control */
};

enum mv_state mv_xor_state_get(u32 chan);
void mv_xor_hal_init(u32 xor_chan_num);
int mv_xor_ctrl_set(u32 chan, u32 xor_ctrl);
int mv_xor_command_set(u32 chan, enum mv_command command);
int mv_xor_override_set(u32 chan, enum xor_override_target target, u32 win_num,
			int enable);
int mv_xor_transfer(u32 chan, enum xor_type type, u32 xor_chain_ptr);

#endif
