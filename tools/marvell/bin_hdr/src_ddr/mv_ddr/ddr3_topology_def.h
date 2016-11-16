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

#ifndef _DDR3_TOPOLOGY_DEF_H
#define _DDR3_TOPOLOGY_DEF_H

#define DEV_NUM_0	0

/* TOPOLOGY */
#if defined(CONFIG_DDR4)
enum hws_speed_bin {
	SPEED_BIN_DDR_1600J,
	SPEED_BIN_DDR_1600K,
	SPEED_BIN_DDR_1600L,
	SPEED_BIN_DDR_1866L,
	SPEED_BIN_DDR_1866M,
	SPEED_BIN_DDR_1866N,
	SPEED_BIN_DDR_2133N,
	SPEED_BIN_DDR_2133P,
	SPEED_BIN_DDR_2133R,
	SPEED_BIN_DDR_2400P,
	SPEED_BIN_DDR_2400R,
	SPEED_BIN_DDR_2400S,
	SPEED_BIN_DDR_2400U,
};

enum hws_ddr_freq {
	DDR_FREQ_LOW_FREQ,
	DDR_FREQ_650,
	DDR_FREQ_667,
	DDR_FREQ_800,
	DDR_FREQ_933,
	DDR_FREQ_1066,
	DDR_FREQ_900,
	DDR_FREQ_1000,
	DDR_FREQ_1050,
	DDR_FREQ_1200,
	DDR_FREQ_LAST
};

enum speed_bin_table_elements {
	SPEED_BIN_TRCD,
	SPEED_BIN_TRP,
	SPEED_BIN_TRAS,
	SPEED_BIN_TRC,
	SPEED_BIN_TRRD0_5K,
	SPEED_BIN_TRRD1K,
	SPEED_BIN_TRRD2K,
	SPEED_BIN_TRRDL0_5K,
	SPEED_BIN_TRRDL1K,
	SPEED_BIN_TRRDL2K,
	SPEED_BIN_TPD,
	SPEED_BIN_TFAW0_5K,
	SPEED_BIN_TFAW1K,
	SPEED_BIN_TFAW2K,
	SPEED_BIN_TWTR,
	SPEED_BIN_TWTRL,
	SPEED_BIN_TRTP,
	SPEED_BIN_TWR,
	SPEED_BIN_TMOD,
	SPEED_BIN_TXPDLL,
	SPEED_BIN_TXSDLL
};

#else /* CONFIG_DDR4 */
enum hws_speed_bin {
	SPEED_BIN_DDR_800D,
	SPEED_BIN_DDR_800E,
	SPEED_BIN_DDR_1066E,
	SPEED_BIN_DDR_1066F,
	SPEED_BIN_DDR_1066G,
	SPEED_BIN_DDR_1333F,
	SPEED_BIN_DDR_1333G,
	SPEED_BIN_DDR_1333H,
	SPEED_BIN_DDR_1333J,
	SPEED_BIN_DDR_1600G,
	SPEED_BIN_DDR_1600H,
	SPEED_BIN_DDR_1600J,
	SPEED_BIN_DDR_1600K,
	SPEED_BIN_DDR_1866J,
	SPEED_BIN_DDR_1866K,
	SPEED_BIN_DDR_1866L,
	SPEED_BIN_DDR_1866M,
	SPEED_BIN_DDR_2133K,
	SPEED_BIN_DDR_2133L,
	SPEED_BIN_DDR_2133M,
	SPEED_BIN_DDR_2133N,

	SPEED_BIN_DDR_1333H_EXT,
	SPEED_BIN_DDR_1600K_EXT,
	SPEED_BIN_DDR_1866M_EXT
};

enum hws_ddr_freq {
	DDR_FREQ_LOW_FREQ,
	DDR_FREQ_400,
	DDR_FREQ_533,
	DDR_FREQ_667,
	DDR_FREQ_800,
	DDR_FREQ_933,
	DDR_FREQ_1066,
	DDR_FREQ_311,
	DDR_FREQ_333,
	DDR_FREQ_467,
	DDR_FREQ_850,
	DDR_FREQ_600,
	DDR_FREQ_300,
	DDR_FREQ_900,
	DDR_FREQ_360,
	DDR_FREQ_1000,
	DDR_FREQ_LAST
};

enum speed_bin_table_elements {
	SPEED_BIN_TRCD,
	SPEED_BIN_TRP,
	SPEED_BIN_TRAS,
	SPEED_BIN_TRC,
	SPEED_BIN_TRRD1K,
	SPEED_BIN_TRRD2K,
	SPEED_BIN_TPD,
	SPEED_BIN_TFAW1K,
	SPEED_BIN_TFAW2K,
	SPEED_BIN_TWTR,
	SPEED_BIN_TRTP,
	SPEED_BIN_TWR,
	SPEED_BIN_TMOD,
	SPEED_BIN_TXPDLL
};
#endif /* CONFIG_DDR4 */

#endif /* _DDR3_TOPOLOGY_DEF_H */
