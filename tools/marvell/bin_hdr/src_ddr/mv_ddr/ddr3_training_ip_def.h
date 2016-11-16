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

#ifndef _DDR3_TRAINING_IP_DEF_H
#define _DDR3_TRAINING_IP_DEF_H

#define PATTERN_55			0x55555555
#define PATTERN_AA			0xaaaaaaaa
#define PATTERN_80			0x80808080
#define PATTERN_20			0x20202020
#define PATTERN_01			0x01010101
#define PATTERN_FF			0xffffffff
#define PATTERN_00			0x00000000

/* 16bit bus width patterns */
#define PATTERN_55AA			0x5555aaaa
#define PATTERN_00FF			0x0000ffff
#define PATTERN_0080			0x00008080

#define INVALID_VALUE			0xffffffff
#define MAX_NUM_OF_DUNITS		32
/*
 * length *2 = length in words of pattern, first low address,
 * second high address
 */
#define TEST_PATTERN_LENGTH		4
#define KILLER_PATTERN_DQ_NUMBER	8
#define SSO_DQ_NUMBER			4
#define PATTERN_MAXIMUM_LENGTH		64
#define ADLL_TX_LENGTH			64
#define ADLL_RX_LENGTH			32

#define PARAM_NOT_CARE			0
#define PARAM_UNDEFINED			0xffffffff

#define READ_LEVELING_PHY_OFFSET	2
#define WRITE_LEVELING_PHY_OFFSET	0

#define MASK_ALL_BITS			0xffffffff

#define CS_BIT_MASK			0xf

/* DFX access */
#define BROADCAST_ID			28
#define MULTICAST_ID			29

#define XSB_BASE_ADDR			0x00004000
#define XSB_CTRL_0_REG			0x00000000
#define XSB_CTRL_1_REG			0x00000004
#define XSB_CMD_REG			0x00000008
#define XSB_ADDRESS_REG			0x0000000c
#define XSB_DATA_REG			0x00000010
#define PIPE_ENABLE_ADDR		0x000f8000
#define ENABLE_DDR_TUNING_ADDR		0x000f829c

#define CLIENT_BASE_ADDR		0x00002000
#define CLIENT_CTRL_REG			0x00000000

#define TARGET_INT			0x1801
#define TARGET_EXT			0x180e
#define BYTE_EN				0
#define CMD_READ			0
#define CMD_WRITE			1

#define INTERNAL_ACCESS_PORT		1
#define EXECUTING			1
#define ACCESS_EXT			1
#define CS2_EXIST_BIT			2
#define TRAINING_ID			0xf
#define EXT_TRAINING_ID			1
#define EXT_MODE			0x4

#define GET_RESULT_STATE(res)		(res)
#define SET_RESULT_STATE(res, state)	(res = state)

#define _1K				0x00000400
#define _4K				0x00001000
#define _8K				0x00002000
#define _16K				0x00004000
#define _32K				0x00008000
#define _64K				0x00010000
#define _128K				0x00020000
#define _256K				0x00040000
#define _512K				0x00080000

#define _1M				0x00100000
#define _2M				0x00200000
#define _4M				0x00400000
#define _8M				0x00800000
#define _16M				0x01000000
#define _32M				0x02000000
#define _64M				0x04000000
#define _128M				0x08000000
#define _256M				0x10000000
#define _512M				0x20000000

#define _1G				0x40000000
#define _2G				0x80000000

#define ADDR_SIZE_512MB			0x04000000
#define ADDR_SIZE_1GB			0x08000000
#define ADDR_SIZE_2GB			0x10000000
#define ADDR_SIZE_4GB			0x20000000
#define ADDR_SIZE_8GB			0x40000000

enum hws_edge_compare {
	EDGE_PF,
	EDGE_FP,
	EDGE_FPF,
	EDGE_PFP
};

enum hws_control_element {
	HWS_CONTROL_ELEMENT_ADLL,		/* per bit 1 edge */
	HWS_CONTROL_ELEMENT_DQ_SKEW,
	HWS_CONTROL_ELEMENT_DQS_SKEW
};

enum hws_search_dir {
	HWS_LOW2HIGH,
	HWS_HIGH2LOW,
	HWS_SEARCH_DIR_LIMIT
};

enum hws_page_size {
	PAGE_SIZE_1K,
	PAGE_SIZE_2K
};

enum hws_operation {
	OPERATION_READ = 0,
	OPERATION_WRITE = 1
};

enum hws_training_ip_stat {
	HWS_TRAINING_IP_STATUS_FAIL,
	HWS_TRAINING_IP_STATUS_SUCCESS,
	HWS_TRAINING_IP_STATUS_TIMEOUT
};

enum hws_ddr_cs {
	CS_SINGLE,
	CS_NON_SINGLE
};

enum hws_ddr_phy {
	DDR_PHY_DATA = 0,
	DDR_PHY_CONTROL = 1
};

enum hws_dir {
	OPER_WRITE,
	OPER_READ,
	OPER_WRITE_AND_READ
};

enum hws_wl_supp {
	PHASE_SHIFT,
	CLOCK_SHIFT,
	ALIGN_SHIFT
};

struct reg_data {
	u32 reg_addr;
	u32 reg_data;
	u32 reg_mask;
};

#endif /* _DDR3_TRAINING_IP_DEF_H */
