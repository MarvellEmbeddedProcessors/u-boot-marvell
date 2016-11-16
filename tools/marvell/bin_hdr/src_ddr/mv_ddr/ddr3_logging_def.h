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

#ifndef _DDR3_LOGGING_CONFIG_H
#define _DDR3_LOGGING_CONFIG_H

#ifdef SILENT_LIB
#define DEBUG_TRAINING_BIST_ENGINE(level, s)
#define DEBUG_TRAINING_IP(level, s)
#define DEBUG_CENTRALIZATION_ENGINE(level, s)
#define DEBUG_TRAINING_HW_ALG(level, s)
#define DEBUG_TRAINING_IP_ENGINE(level, s)
#define DEBUG_LEVELING(level, s)
#define DEBUG_PBS_ENGINE(level, s)
#define DEBUG_TRAINING_STATIC_IP(level, s)
#define DEBUG_TRAINING_ACCESS(level, s)
#else
#ifdef LIB_FUNCTIONAL_DEBUG_ONLY
#define DEBUG_TRAINING_BIST_ENGINE(level, s)
#define DEBUG_TRAINING_IP_ENGINE(level, s)
#define DEBUG_TRAINING_IP(level, s)		\
	if (level >= debug_training)		\
		printf s
#define DEBUG_CENTRALIZATION_ENGINE(level, s)	\
	if (level >= debug_centralization)	\
		printf s
#define DEBUG_TRAINING_HW_ALG(level, s)		\
	if (level >= debug_training_hw_alg)	\
		printf s
#define DEBUG_LEVELING(level, s)		\
	if (level >= debug_leveling)		\
		printf s
#define DEBUG_PBS_ENGINE(level, s)		\
	if (level >= debug_pbs)			\
		printf s
#define DEBUG_TRAINING_STATIC_IP(level, s)	\
	if (level >= debug_training_static)	\
		printf s
#define DEBUG_TRAINING_ACCESS(level, s)		\
	if (level >= debug_training_access)	\
		printf s
#else
#define DEBUG_TRAINING_BIST_ENGINE(level, s)	\
	if (level >= debug_training_bist)	\
		printf s

#define DEBUG_TRAINING_IP_ENGINE(level, s)	\
	if (level >= debug_training_ip)		\
		printf s
#define DEBUG_TRAINING_IP(level, s)		\
	if (level >= debug_training)		\
		printf s
#define DEBUG_CENTRALIZATION_ENGINE(level, s)	\
	if (level >= debug_centralization)	\
		printf s
#define DEBUG_TRAINING_HW_ALG(level, s)		\
	if (level >= debug_training_hw_alg)	\
		printf s
#define DEBUG_LEVELING(level, s)		\
	if (level >= debug_leveling)		\
		printf s
#define DEBUG_PBS_ENGINE(level, s)		\
	if (level >= debug_pbs)			\
		printf s
#define DEBUG_TRAINING_STATIC_IP(level, s)	\
	if (level >= debug_training_static)	\
		printf s
#define DEBUG_TRAINING_ACCESS(level, s)		\
	if (level >= debug_training_access)	\
		printf s
#endif
#endif

#ifdef CONFIG_DDR4
#ifdef SILENT_LIB
#define DEBUG_TAP_TUNING_ENGINE(level, s)
#define DEBUG_CALIBRATION(level, s)
#define DEBUG_DDR4_CENTRALIZATION(level, s)
#else /* SILENT_LIB */
#define DEBUG_TAP_TUNING_ENGINE(level, s)	\
	if (level >= debug_tap_tuning)		\
		printf s
#define DEBUG_CALIBRATION(level, s)		\
	if (level >= debug_calibration)		\
		printf s
#define DEBUG_DDR4_CENTRALIZATION(level, s)	\
	if (level >= debug_ddr4_centralization)	\
		printf s
#endif /* SILENT_LIB */
#endif /* CONFIG_DDR4 */

/* Logging defines */
#define DEBUG_LEVEL_TRACE	1
#define DEBUG_LEVEL_INFO	2
#define DEBUG_LEVEL_ERROR	3

enum ddr_lib_debug_block {
	DEBUG_BLOCK_STATIC,
	DEBUG_BLOCK_TRAINING_MAIN,
	DEBUG_BLOCK_LEVELING,
	DEBUG_BLOCK_CENTRALIZATION,
	DEBUG_BLOCK_PBS,
	DEBUG_BLOCK_IP,
	DEBUG_BLOCK_BIST,
	DEBUG_BLOCK_ALG,
	DEBUG_BLOCK_DEVICE,
	DEBUG_BLOCK_ACCESS,
	DEBUG_STAGES_REG_DUMP,
#if defined(CONFIG_DDR4)
	DEBUG_TAP_TUNING_ENGINE,
	DEBUG_BLOCK_CALIBRATION,
	DEBUG_BLOCK_DDR4_CENTRALIZATION,
#endif /* CONFIG_DDR4 */
	/* All excluding IP and REG_DUMP, should be enabled separatelly */
	DEBUG_BLOCK_ALL
};

int ddr3_tip_print_log(u32 dev_num, u32 mem_addr);
int ddr3_tip_print_stability_log(u32 dev_num);

#endif /* _DDR3_LOGGING_CONFIG_H */
