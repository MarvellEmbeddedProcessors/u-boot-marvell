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

#include "ddr3_init.h"
#include "xor_regs.h"

/* defines  */
#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

static u32 ui_xor_regs_ctrl_backup;
static u32 ui_xor_regs_base_backup[MAX_CS + 1];
static u32 ui_xor_regs_mask_backup[MAX_CS + 1];

void mv_sys_xor_init(u32 num_of_cs, u32 cs_ena, u32 cs_size, u32 base_delta)
{
	u32 reg, ui, base, cs_count, size_mask;

	ui_xor_regs_ctrl_backup = reg_read(XOR_WINDOW_CTRL_REG(0, 0));
	for (ui = 0; ui < MAX_CS + 1; ui++)
		ui_xor_regs_base_backup[ui] =
			reg_read(XOR_BASE_ADDR_REG(0, ui));
	for (ui = 0; ui < MAX_CS + 1; ui++)
		ui_xor_regs_mask_backup[ui] =
			reg_read(XOR_SIZE_MASK_REG(0, ui));

	reg = 0;
	for (ui = 0, cs_count = 0;
	     (cs_count < num_of_cs) && (ui < 8);
	     ui++, cs_count++) {
		if (cs_ena & (1 << ui)) {
			/* Enable Window x for each CS */
			reg |= (0x1 << (ui));
			/* Enable Window x for each CS */
			reg |= (0x3 << ((ui * 2) + 16));
		}
	}

	reg_write(XOR_WINDOW_CTRL_REG(0, 0), reg);

	cs_count = 0;
	for (ui = 0, cs_count = 0;
	     (cs_count < num_of_cs) && (ui < 8);
	     ui++, cs_count++) {
		if (cs_ena & (1 << ui)) {
			/*
			 * window x - Base - 0x00000000,
			 * Attribute 0x0e - DRAM
			 */
			base = cs_size * ui + base_delta;
			/* fixed size 2GB for each CS */
			size_mask = 0x7FFF0000;
			switch (ui) {
			case 0:
				base |= 0xe00;
				break;
			case 1:
				base |= 0xd00;
				break;
			case 2:
				base |= 0xb00;
				break;
			case 3:
				base |= 0x700;
				break;
			case 4: /* SRAM */
				base = 0x40000000;
				/* configure as shared transaction */
				base |= 0x1F00;
				size_mask = 0xF0000;
				break;
			}

			reg_write(XOR_BASE_ADDR_REG(0, ui), base);

			/* window x - Size */
			reg_write(XOR_SIZE_MASK_REG(0, ui), size_mask);
		}
	}

	mv_xor_hal_init(1);

	return;
}

void mv_sys_xor_finish(void)
{
	u32 ui;

	reg_write(XOR_WINDOW_CTRL_REG(0, 0), ui_xor_regs_ctrl_backup);
	for (ui = 0; ui < MAX_CS + 1; ui++)
		reg_write(XOR_BASE_ADDR_REG(0, ui),
			  ui_xor_regs_base_backup[ui]);
	for (ui = 0; ui < MAX_CS + 1; ui++)
		reg_write(XOR_SIZE_MASK_REG(0, ui),
			  ui_xor_regs_mask_backup[ui]);

	reg_write(XOR_ADDR_OVRD_REG(0, 0), 0);
}

/*
 * mv_xor_hal_init - Initialize XOR engine
 *
 * DESCRIPTION:
 *               This function initialize XOR unit.
 * INPUT:
 *       None.
 *
 * OUTPUT:
 *       None.
 *
 * RETURN:
 *       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
 */
void mv_xor_hal_init(u32 xor_chan_num)
{
	u32 i;

	/* Abort any XOR activity & set default configuration */
	for (i = 0; i < xor_chan_num; i++) {
		mv_xor_command_set(i, MV_STOP);
		mv_xor_ctrl_set(i, (1 << XEXCR_REG_ACC_PROTECT_OFFS) |
				(4 << XEXCR_DST_BURST_LIMIT_OFFS) |
				(4 << XEXCR_SRC_BURST_LIMIT_OFFS));
	}
}

/*
 * mv_xor_ctrl_set - Set XOR channel control registers
 *
 * DESCRIPTION:
 *
 * INPUT:
 *
 * OUTPUT:
 *       None.
 *
 * RETURN:
 *       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
 * NOTE:
 *  This function does not modify the Operation_mode field of control register.
 */
int mv_xor_ctrl_set(u32 chan, u32 xor_ctrl)
{
	u32 old_value;

	/* update the XOR Engine [0..1] Configuration Registers (XEx_c_r) */
	old_value = reg_read(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan))) &
		XEXCR_OPERATION_MODE_MASK;
	xor_ctrl &= ~XEXCR_OPERATION_MODE_MASK;
	xor_ctrl |= old_value;
	reg_write(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)), xor_ctrl);

	return MV_OK;
}

int mv_xor_mem_init(u32 chan, u32 start_ptr, u32 block_size,
		    u32 init_val_high, u32 init_val_low)
{
	u32 temp;

	/* Parameter checking */
	if (chan >= MV_XOR_MAX_CHAN)
		return MV_BAD_PARAM;

	if (MV_ACTIVE == mv_xor_state_get(chan))
		return MV_BUSY;

	if ((block_size < XEXBSR_BLOCK_SIZE_MIN_VALUE) ||
	    (block_size > XEXBSR_BLOCK_SIZE_MAX_VALUE))
		return MV_BAD_PARAM;

	/* set the operation mode to Memory Init */
	temp = reg_read(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)));
	temp &= ~XEXCR_OPERATION_MODE_MASK;
	temp |= XEXCR_OPERATION_MODE_MEM_INIT;
	reg_write(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)), temp);

	/*
	 * update the start_ptr field in XOR Engine [0..1] Destination Pointer
	 * Register
	 */
	reg_write(XOR_DST_PTR_REG(XOR_UNIT(chan), XOR_CHAN(chan)), start_ptr);

	/*
	 * update the Block_size field in the XOR Engine[0..1] Block Size
	 * Registers
	 */
	reg_write(XOR_BLOCK_SIZE_REG(XOR_UNIT(chan), XOR_CHAN(chan)),
		  block_size);

	/*
	 * update the field Init_val_l in the XOR Engine Initial Value Register
	 * Low (XEIVRL)
	 */
	reg_write(XOR_INIT_VAL_LOW_REG(XOR_UNIT(chan)), init_val_low);

	/*
	 * update the field Init_val_h in the XOR Engine Initial Value Register
	 * High (XEIVRH)
	 */
	reg_write(XOR_INIT_VAL_HIGH_REG(XOR_UNIT(chan)), init_val_high);

	/* start transfer */
	reg_bit_set(XOR_ACTIVATION_REG(XOR_UNIT(chan), XOR_CHAN(chan)),
		    XEXACTR_XESTART_MASK);

	return MV_OK;
}

/*
 * mv_xor_state_get - Get XOR channel state.
 *
 * DESCRIPTION:
 *       XOR channel activity state can be active, idle, paused.
 *       This function retrunes the channel activity state.
 *
 * INPUT:
 *       chan     - the channel number
 *
 * OUTPUT:
 *       None.
 *
 * RETURN:
 *       XOR_CHANNEL_IDLE    - If the engine is idle.
 *       XOR_CHANNEL_ACTIVE  - If the engine is busy.
 *       XOR_CHANNEL_PAUSED  - If the engine is paused.
 *       MV_UNDEFINED_STATE  - If the engine state is undefind or there is no
 *                             such engine
 */
enum mv_state mv_xor_state_get(u32 chan)
{
	u32 state;

	/* Parameter checking   */
	if (chan >= MV_XOR_MAX_CHAN) {
		DB(printf("%s: ERR. Invalid chan num %d\n", __func__, chan));
		return MV_UNDEFINED_STATE;
	}

	/* read the current state */
	state = reg_read(XOR_ACTIVATION_REG(XOR_UNIT(chan), XOR_CHAN(chan)));
	state &= XEXACTR_XESTATUS_MASK;

	/* return the state */
	switch (state) {
	case XEXACTR_XESTATUS_IDLE:
		return MV_IDLE;
	case XEXACTR_XESTATUS_ACTIVE:
		return MV_ACTIVE;
	case XEXACTR_XESTATUS_PAUSED:
		return MV_PAUSED;
	}

	return MV_UNDEFINED_STATE;
}

/*
 * mv_xor_command_set - Set command of XOR channel
 *
 * DESCRIPTION:
 *       XOR channel can be started, idle, paused and restarted.
 *       Paused can be set only if channel is active.
 *       Start can be set only if channel is idle or paused.
 *       Restart can be set only if channel is paused.
 *       Stop can be set only if channel is active.
 *
 * INPUT:
 *       chan     - The channel number
 *       command  - The command type (start, stop, restart, pause)
 *
 * OUTPUT:
 *       None.
 *
 * RETURN:
 *       MV_OK on success , MV_BAD_PARAM on erroneous parameter, MV_ERROR on
 *       undefind XOR engine mode
 */
int mv_xor_command_set(u32 chan, enum mv_command command)
{
	enum mv_state state;

	/* Parameter checking */
	if (chan >= MV_XOR_MAX_CHAN) {
		DB(printf("%s: ERR. Invalid chan num %d\n", __func__, chan));
		return MV_BAD_PARAM;
	}

	/* get the current state */
	state = mv_xor_state_get(chan);

	if ((command == MV_START) && (state == MV_IDLE)) {
		/* command is start and current state is idle */
		reg_bit_set(XOR_ACTIVATION_REG
			    (XOR_UNIT(chan), XOR_CHAN(chan)),
			    XEXACTR_XESTART_MASK);
		return MV_OK;
	} else if ((command == MV_STOP) && (state == MV_ACTIVE)) {
		/* command is stop and current state is active */
		reg_bit_set(XOR_ACTIVATION_REG
			    (XOR_UNIT(chan), XOR_CHAN(chan)),
			    XEXACTR_XESTOP_MASK);
		return MV_OK;
	} else if (((enum mv_state)command == MV_PAUSED) &&
		   (state == MV_ACTIVE)) {
		/* command is paused and current state is active */
		reg_bit_set(XOR_ACTIVATION_REG
			    (XOR_UNIT(chan), XOR_CHAN(chan)),
			    XEXACTR_XEPAUSE_MASK);
		return MV_OK;
	} else if ((command == MV_RESTART) && (state == MV_PAUSED)) {
		/* command is restart and current state is paused */
		reg_bit_set(XOR_ACTIVATION_REG
			    (XOR_UNIT(chan), XOR_CHAN(chan)),
			    XEXACTR_XERESTART_MASK);
		return MV_OK;
	} else if ((command == MV_STOP) && (state == MV_IDLE)) {
		/* command is stop and current state is active */
		return MV_OK;
	}

	/* illegal command */
	DB(printf("%s: ERR. Illegal command\n", __func__));

	return MV_BAD_PARAM;
}

void ddr3_new_tip_ecc_scrub(void)
{
	u32 cs_c, max_cs;
	u32 cs_ena = 0;
	u32 dev_num = 0;

	printf("DDR Training Sequence - Start scrubbing\n");

	max_cs = ddr3_tip_max_cs_get(dev_num);
	for (cs_c = 0; cs_c < max_cs; cs_c++)
		cs_ena |= 1 << cs_c;

	mv_sys_xor_init(max_cs, cs_ena, 0x80000000, 0);

	mv_xor_mem_init(0, 0x00000000, 0x80000000, 0xdeadbeef, 0xdeadbeef);
	/* wait for previous transfer completion */
	while (mv_xor_state_get(0) != MV_IDLE)
		;

	mv_xor_mem_init(0, 0x80000000, 0x40000000, 0xdeadbeef, 0xdeadbeef);

	/* wait for previous transfer completion */
	while (mv_xor_state_get(0) != MV_IDLE)
		;

	/* Return XOR State */
	mv_sys_xor_finish();

	printf("DDR3 Training Sequence - End scrubbing\n");
}

/*
* mv_xor_transfer - Transfer data from source to destination in one of
*		    three modes: XOR, CRC32 or DMA
*
* DESCRIPTION:
*	This function initiates XOR channel, according to function parameters,
*	in order to perform XOR, CRC32 or DMA transaction.
*	To gain maximum performance the user is asked to keep the following
*	restrictions:
*	1) Selected engine is available (not busy).
*	2) This module does not take into consideration CPU MMU issues.
*	   In order for the XOR engine to access the appropriate source
*	   and destination, address parameters must be given in system
*	   physical mode.
*	3) This API does not take care of cache coherency issues. The source,
*	   destination and, in case of chain, the descriptor list are assumed
*	   to be cache coherent.
*	4) Parameters validity.
*
* INPUT:
*	chan		- XOR channel number.
*	type	- One of three: XOR, CRC32 and DMA operations.
*	xor_chain_ptr	- address of chain pointer
*
* OUTPUT:
*	None.
*
* RETURN:
*       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
*
*******************************************************************************/
int mv_xor_transfer(u32 chan, enum xor_type type, u32 xor_chain_ptr)
{
	u32 temp;

	/* Parameter checking */
	if (chan >= MV_XOR_MAX_CHAN) {
		DB(printf("%s: ERR. Invalid chan num %d\n", __func__, chan));
		return MV_BAD_PARAM;
	}
	if (mv_xor_state_get(chan) == MV_ACTIVE) {
		DB(printf("%s: ERR. Channel is already active\n", __func__));
		return MV_BUSY;
	}
	if (xor_chain_ptr == 0x0) {
		DB(printf("%s: ERR. xor_chain_ptr is NULL pointer\n", __func__));
		return MV_BAD_PARAM;
	}

	/* read configuration register and mask the operation mode field */
	temp = reg_read(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)));
	temp &= ~XEXCR_OPERATION_MODE_MASK;

	switch (type) {
	case MV_XOR:
		if ((xor_chain_ptr & XEXDPR_DST_PTR_XOR_MASK) != 0) {
			DB(printf("%s: ERR. Invalid chain pointer (bits [5:0] must be cleared)\n",
				  __func__));
			return MV_BAD_PARAM;
		}
		/* set the operation mode to XOR */
		temp |= XEXCR_OPERATION_MODE_XOR;
		break;
	case MV_DMA:
		if ((xor_chain_ptr & XEXDPR_DST_PTR_DMA_MASK) != 0) {
			DB(printf("%s: ERR. Invalid chain pointer (bits [4:0] must be cleared)\n",
				  __func__));
			return MV_BAD_PARAM;
		}
		/* set the operation mode to DMA */
		temp |= XEXCR_OPERATION_MODE_DMA;
		break;
	case MV_CRC32:
		if ((xor_chain_ptr & XEXDPR_DST_PTR_CRC_MASK) != 0) {
			DB(printf("%s: ERR. Invalid chain pointer (bits [4:0] must be cleared)\n",
				  __func__));
			return MV_BAD_PARAM;
		}
		/* set the operation mode to CRC32 */
		temp |= XEXCR_OPERATION_MODE_CRC;
		break;
	default:
		return MV_BAD_PARAM;
	}

	/* write the operation mode to the register */
	reg_write(XOR_CONFIG_REG(XOR_UNIT(chan), XOR_CHAN(chan)), temp);
	/*
	 * update the NextDescPtr field in the XOR Engine [0..1] Next Descriptor
	 * Pointer Register (XExNDPR)
	 */
	reg_write(XOR_NEXT_DESC_PTR_REG(XOR_UNIT(chan), XOR_CHAN(chan)),
		  xor_chain_ptr);

	/* start transfer */
	reg_bit_set(XOR_ACTIVATION_REG(XOR_UNIT(chan), XOR_CHAN(chan)),
		    XEXACTR_XESTART_MASK);

	return MV_OK;
}
