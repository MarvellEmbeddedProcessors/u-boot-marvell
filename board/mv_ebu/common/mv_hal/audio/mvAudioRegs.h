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
#ifndef __INCMVAudioRegsH
#define __INCMVAudioRegsH

#ifdef __cplusplus
extern "C" {
#endif

#include "mvSysAudioConfig.h"

#define APBBCR_SIZE_MAX						0x3FFFFF
#define APBBCR_SIZE_SHIFT					0x2

#define AUDIO_REG_TO_SIZE(reg)		(((reg) + 1) << APBBCR_SIZE_SHIFT)
#define AUDIO_SIZE_TO_REG(size)		(((size) >> APBBCR_SIZE_SHIFT) - 1)

#define MV_AUDIO_BUFFER_MIN_ALIGN	0x8

/********************/
/* Clocking Control*/
/*******************/
#define MV_AUDIO_PLL_CTRL1_REG(unit)			(MV_AUDIO_REGS_BASE(unit) + 0x1200)
#define MV_AUDIO_DCO_CTRL_REG(unit)			(MV_AUDIO_REGS_BASE(unit) + 0x1204)
#define MV_AUDIO_SPCR_DCO_STATUS_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x120c)
#define MV_AUDIO_SAMPLE_CNTR_CTRL_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x1220)
#define MV_AUDIO_PLAYBACK_SAMPLE_CNTR_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x1224)
#define MV_AUDIO_RECORD_SAMPLE_CNTR_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x1228)
#define MV_AUDIO_CLOCK_CTRL_REG(unit)			(MV_AUDIO_REGS_BASE(unit) + 0x1230)

/* MV_AUDIO_DCO_CTRL_REG */
#define ADCR_DCO_CTRL_FS_OFFS		0
#define ADCR_DCO_CTRL_FS_MASK		(0x3 << ADCR_DCO_CTRL_FS_OFFS)
#define ADCR_DCO_CTRL_FS_44_1KHZ	(0x0 << ADCR_DCO_CTRL_FS_OFFS)
#define ADCR_DCO_CTRL_FS_48KHZ		(0x1 << ADCR_DCO_CTRL_FS_OFFS)
#define ADCR_DCO_CTRL_FS_96KHZ		(0x2 << ADCR_DCO_CTRL_FS_OFFS)

#define ADCR_DCO_CTRL_OFFSET_OFFS	2
#define ADCR_DCO_CTRL_OFFSET_MASK	(0xfff << ADCR_DCO_CTRL_OFFSET_OFFS)

/* MV_AUDIO_SPCR_DCO_STATUS_REG */
#define ASDSR_SPCR_CTRLFS_OFFS		0
#define ASDSR_SPCR_CTRLFS_MASK		(0x7 << ASDSR_SPCR_CTRLFS_OFFS)
#define ASDSR_SPCR_CTRLFS_44_1KHZ	(0x0 << ASDSR_SPCR_CTRLFS_OFFS)
#define ASDSR_SPCR_CTRLFS_48KHZ		(0x1 << ASDSR_SPCR_CTRLFS_OFFS)
#define ASDSR_SPCR_CTRLFS_96KHZ		(0x2 << ASDSR_SPCR_CTRLFS_OFFS)
#define ASDSR_SPCR_CTRLFS_44_1KHZ_LESS	(0x3 << ASDSR_SPCR_CTRLFS_OFFS)
#define ASDSR_SPCR_CTRLFS_96KHZ_MORE	(0x4 << ASDSR_SPCR_CTRLFS_OFFS)
#define ASDSR_SPCR_CTRLFS_OTHER		(0x7 << ASDSR_SPCR_CTRLFS_OFFS)

#define ASDSR_SPCR_CTRLOFFSET_OFFS	3
#define ASDSR_SPCR_CTRLOFFSET_MASK	(0xfff << ASDSR_SPCR_CTRLOFFSET_OFFS)

#define ASDSR_SPCR_LOCK_OFFS		15
#define ASDSR_SPCR_LOCK_MASK		(0x1 << ASDSR_SPCR_LOCK_OFFS)

#define ASDSR_DCO_LOCK_OFFS		16
#define ASDSR_DCO_LOCK_MASK		(0x1 << ASDSR_DCO_LOCK_OFFS)

#define ASDSR_PLL_LOCK_OFFS		17
#define ASDSR_PLL_LOCK_MASK		(0x1 << ASDSR_PLL_LOCK_OFFS)

/*MV_AUDIO_SAMPLE_CNTR_CTRL_REG */

#define ASCCR_CLR_PLAY_CNTR_OFFS	9
#define ASCCR_CLR_PLAY_CNTR_MASK	(0x1 << ASCCR_CLR_PLAY_CNTR_OFFS)

#define ASCCR_CLR_REC_CNTR_OFFS		8
#define ASCCR_CLR_REC_CNTR_MASK		(0x1 << ASCCR_CLR_REC_CNTR_OFFS)

#define ASCCR_ACTIVE_PLAY_CNTR_OFFS	1
#define ASCCR_ACTIVE_PLAY_CNTR_MASK	(0x1 << ASCCR_ACTIVE_PLAY_CNTR_OFFS)

#define ASCCR_ACTIVE_REC_CNTR_OFFS	0
#define ASCCR_ACTIVE_REC_CNTR_MASK	(0x1 << ASCCR_ACTIVE_REC_CNTR_OFFS)

/* MV_AUDIO_CLOCK_CTRL_REG */
#define ACCR_MCLK_SOURCE_OFFS		0
#define ACCR_MCLK_SOURCE_MASK		(0x3 << ACCR_MCLK_SOURCE_OFFS)
#define ACCR_MCLK_SOURCE_DCO		(0x0 << ACCR_MCLK_SOURCE_OFFS)
#define ACCR_MCLK_SOURCE_SPCR		(0x2 << ACCR_MCLK_SOURCE_OFFS)
#define ACCR_MCLK_SOURCE_EXT		(0x3 << ACCR_MCLK_SOURCE_OFFS)

/********************/
/* Interrupts		*/
/*******************/
#define MV_AUDIO_ERROR_CAUSE_REG(unit)			(MV_AUDIO_REGS_BASE(unit) + 0x1300)
#define MV_AUDIO_ERROR_MASK_REG(unit)			(MV_AUDIO_REGS_BASE(unit) + 0x1304)
#define MV_AUDIO_INT_CAUSE_REG(unit)			(MV_AUDIO_REGS_BASE(unit) + 0x1308)
#define MV_AUDIO_INT_MASK_REG(unit)			(MV_AUDIO_REGS_BASE(unit) + 0x130C)
#define MV_AUDIO_RECORD_BYTE_CNTR_INT_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x1310)
#define MV_AUDIO_PLAYBACK_BYTE_CNTR_INT_REG(unit)	(MV_AUDIO_REGS_BASE(unit) + 0x1314)

/* MV_AUDIO_INT_CAUSE_REG*/
#define AICR_RECORD_BYTES_INT			(0x1 << 13)
#define AICR_PLAY_BYTES_INT			(0x1 << 14)

#define ARBCI_BYTE_COUNT_MASK                   0xFFFFFF
#define APBCI_BYTE_COUNT_MASK                   0xFFFFFF

/********************/
/* Audio Playback	*/
/*******************/
/* General */
#define MV_AUDIO_PLAYBACK_CTRL_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x1100)
#define MV_AUDIO_PLAYBACK_BUFF_START_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x1104)
#define MV_AUDIO_PLAYBACK_BUFF_SIZE_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x1108)
#define MV_AUDIO_PLAYBACK_BUFF_BYTE_CNTR_REG(unit)	(MV_AUDIO_REGS_BASE(unit) + 0x110c)

/* SPDIF */
#define MV_AUDIO_SPDIF_PLAY_CTRL_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x2204)
#define MV_AUDIO_SPDIF_PLAY_CH_STATUS_LEFT_REG(unit, ind)	        \
					(MV_AUDIO_REGS_BASE(unit) + 0x2280 + (ind << 2))
#define MV_AUDIO_SPDIF_PLAY_CH_STATUS_RIGHT_REG(unit, ind)	    \
					(MV_AUDIO_REGS_BASE(unit) + 0x22a0 + (ind << 2))
#define MV_AUDIO_SPDIF_PLAY_USR_BITS_LEFT_REG(unit, ind)          \
					(MV_AUDIO_REGS_BASE(unit) + 0x22c0 + (ind << 2))
#define MV_AUDIO_SPDIF_PLAY_USR_BITS_RIGHT_REG(unit, ind)              \
					(MV_AUDIO_REGS_BASE(unit) + 0x22e0 + (ind << 2))
/*I2S*/
#define MV_AUDIO_I2S_PLAY_CTRL_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x2508)

/* MV_AUDIO_PLAYBACK_CTRL_REG */
#define APCR_PLAY_SAMPLE_SIZE_OFFS			0
#define APCR_PLAY_SAMPLE_SIZE_MASK			(0x7 << APCR_PLAY_SAMPLE_SIZE_OFFS)

#define APCR_PLAY_I2S_ENABLE_OFFS			3
#define APCR_PLAY_I2S_ENABLE_MASK			(0x1 << APCR_PLAY_I2S_ENABLE_OFFS)

#define APCR_PLAY_SPDIF_ENABLE_OFFS			4
#define APCR_PLAY_SPDIF_ENABLE_MASK			(0x1 << APCR_PLAY_SPDIF_ENABLE_OFFS)

#define APCR_PLAY_MONO_OFFS				5
#define APCR_PLAY_MONO_MASK				(0x3 << APCR_PLAY_MONO_OFFS)

#define APCR_PLAY_I2S_MUTE_OFFS				7
#define APCR_PLAY_I2S_MUTE_MASK				(0x1 << APCR_PLAY_I2S_MUTE_OFFS)

#define APCR_PLAY_SPDIF_MUTE_OFFS			8
#define APCR_PLAY_SPDIF_MUTE_MASK			(0x1 << APCR_PLAY_SPDIF_MUTE_OFFS)

#define APCR_PLAY_PAUSE_OFFS				9
#define APCR_PLAY_PAUSE_MASK				(0x1 << APCR_PLAY_PAUSE_OFFS)

#define APCR_LOOPBACK_OFFS				10
#define APCR_LOOPBACK_MASK				(0x1 << APCR_LOOPBACK_OFFS)

#define APCR_PLAY_BURST_SIZE_OFFS			11
#define APCR_PLAY_BURST_SIZE_MASK			(0x3 << APCR_PLAY_BURST_SIZE_OFFS)

#define APCR_PLAY_BUSY_OFFS				16
#define APCR_PLAY_BUSY_MASK				(0x1 << APCR_PLAY_BUSY_OFFS)

/* MV_AUDIO_PLAYBACK_BUFF_BYTE_CNTR_REG */
#define APBBCR_SIZE_MAX					0x3FFFFF
#define APBBCR_SIZE_SHIFT				0x2

/* MV_AUDIO_SPDIF_PLAY_CTRL_REG */
#define ASPCR_SPDIF_BLOCK_START_OFFS		0x0
#define ASPCR_SPDIF_BLOCK_START_MASK		(0x1 << ASPCR_SPDIF_BLOCK_START_OFFS)

#define ASPCR_SPDIF_PB_EN_MEM_VALIDITY_OFFS	0x1
#define ASPCR_SPDIF_PB_EN_MEM_VALIDITY_MASK	(0x1 << ASPCR_SPDIF_PB_EN_MEM_VALIDITY_OFFS)

#define ASPCR_SPDIF_PB_MEM_USR_EN_OFFS		0x2
#define ASPCR_SPDIF_PB_MEM_USR_EN_MASK		(0x1 << ASPCR_SPDIF_PB_MEM_USR_EN_OFFS)

#define ASPCR_SPDIF_UNDERRUN_DATA_OFFS		0x5
#define ASPCR_SPDIF_UNDERRUN_DATA_MASK		(0x1 << ASPCR_SPDIF_UNDERRUN_DATA_OFFS)

#define ASPCR_SPDIF_PB_REG_VALIDITY_OFFS	16
#define ASPCR_SPDIF_PB_REG_VALIDITY_MASK	(0x1 << ASPCR_SPDIF_PB_REG_VALIDITY_OFFS)

#define ASPCR_SPDIF_PB_NONPCM_OFFS		17
#define ASPCR_SPDIF_PB_NONPCM_MASK		(0x1 << ASPCR_SPDIF_PB_NONPCM_OFFS)

/* MV_AUDIO_I2S_PLAY_CTRL_REG */
#define AIPCR_I2S_SEND_LAST_FRM_OFFS		23
#define AIPCR_I2S_SEND_LAST_FRM_MASK		(1 << AIPCR_I2S_SEND_LAST_FRM_OFFS)

#define AIPCR_I2S_PB_JUSTF_OFFS			26
#define AIPCR_I2S_PB_JUSTF_MASK			(0xf << AIPCR_I2S_PB_JUSTF_OFFS)

#define AIPCR_I2S_PB_SAMPLE_SIZE_OFFS		30
#define AIPCR_I2S_PB_SAMPLE_SIZE_MASK		(0x3 << AIPCR_I2S_PB_SAMPLE_SIZE_OFFS)

/********************/
/* Audio Recordnig	*/
/*******************/
/* General */
#define MV_AUDIO_RECORD_CTRL_REG(unit)			(MV_AUDIO_REGS_BASE(unit) + 0x1000)
#define MV_AUDIO_RECORD_START_ADDR_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x1004)
#define MV_AUDIO_RECORD_BUFF_SIZE_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x1008)
#define MV_AUDIO_RECORD_BUF_BYTE_CNTR_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x100C)

/*SPDIF */
#define MV_AUDIO_SPDIF_REC_GEN_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x2004)
#define MV_AUDIO_SPDIF_REC_INT_CAUSE_MASK_REG(unit)	(MV_AUDIO_REGS_BASE(unit) + 0x2008)
#define MV_AUDIO_SPDIF_REC_CH_STATUS_LEFT_REG(unit, ind)                  \
						(MV_AUDIO_REGS_BASE(unit) + 0x2180 + ((ind) << 2))
#define MV_AUDIO_SPDIF_REC_CH_STATUS_RIGHT_REG(unit, ind)                 \
						(MV_AUDIO_REGS_BASE(unit) + 0x21a0 + ((ind) << 2))
#define MV_AUDIO_SPDIF_REC_USR_BITS_LEFT_REG(unit, ind)                   \
						(MV_AUDIO_REGS_BASE(unit) + 0x21c0 + ((ind) << 2))
#define MV_AUDIO_SPDIF_REC_USR_BITS_RIGHT_REG(unit, ind)                  \
						(MV_AUDIO_REGS_BASE(unit) + 0x21e0 + ((ind) << 2))

/*I2S*/
#define MV_AUDIO_I2S_REC_CTRL_REG(unit)		(MV_AUDIO_REGS_BASE(unit) + 0x2408)

/* MV_AUDIO_RECORD_CTRL_REG*/
#define ARCR_RECORD_SAMPLE_SIZE_OFFS   	  	0
#define ARCR_RECORD_SAMPLE_SIZE_MASK   	  	(0x7 << ARCR_RECORD_SAMPLE_SIZE_OFFS)

#define ARCR_RECORDED_MONO_CHNL_OFFS   	  	3
#define ARCR_RECORDED_MONO_CHNL_MASK   	  	(0x1 << ARCR_RECORDED_MONO_CHNL_OFFS)

#define ARCR_RECORD_MONO_OFFS		   	4
#define ARCR_RECORD_MONO_MASK		   	(0x1 << ARCR_RECORD_MONO_OFFS)

#define ARCR_RECORD_BURST_SIZE_OFFS	   	5
#define ARCR_RECORD_BURST_SIZE_MASK	   	(0x3 << ARCR_RECORD_BURST_SIZE_OFFS)

#define ARCR_RECORD_MUTE_OFFS		   	8
#define ARCR_RECORD_MUTE_MASK		   	(0x1 << ARCR_RECORD_MUTE_OFFS)

#define ARCR_RECORD_PAUSE_OFFS		   	9
#define ARCR_RECORD_PAUSE_MASK		   	(0x1 << ARCR_RECORD_PAUSE_OFFS)

#define ARCR_RECORD_I2S_EN_OFFS		   	10
#define ARCR_RECORD_I2S_EN_MASK		   	(0x1 << ARCR_RECORD_I2S_EN_OFFS)

#define ARCR_RECORD_SPDIF_EN_OFFS	   	11
#define ARCR_RECORD_SPDIF_EN_MASK	   	(0x1 << ARCR_RECORD_SPDIF_EN_OFFS)

/* MV_AUDIO_SPDIF_REC_GEN_REG*/
#define ASRGR_CORE_CLK_FREQ_OFFS		1
#define ASRGR_CORE_CLK_FREQ_MASK		(0x3 << ASRGR_CORE_CLK_FREQ_OFFS)
#define ASRGR_CORE_CLK_FREQ_133MHZ		(0x0 << ASRGR_CORE_CLK_FREQ_OFFS)
#define ASRGR_CORE_CLK_FREQ_150MHZ		(0x1 << ASRGR_CORE_CLK_FREQ_OFFS)
#define ASRGR_CORE_CLK_FREQ_166MHZ		(0x2 << ASRGR_CORE_CLK_FREQ_OFFS)
#define ASRGR_CORE_CLK_FREQ_200MHZ		(0x3 << ASRGR_CORE_CLK_FREQ_OFFS)

#define ASRGR_VALID_PCM_INFO_OFFS		7
#define ASRGR_VALID_PCM_INFO_MASK		(0x1 << ASRGR_VALID_PCM_INFO_OFFS)

#define ASRGR_SAMPLE_FREQ_OFFS			8
#define ASRGR_SAMPLE_FREQ_MASK			(0xf << ASRGR_SAMPLE_FREQ_OFFS)

#define ASRGR_NON_PCM_OFFS			14
#define ASRGR_NON_PCM_MASK			(1 << ASRGR_NON_PCM_OFFS)

/* MV_AUDIO_I2S_REC_CTRL_REG*/
#define AIRCR_I2S_RECORD_JUSTF_OFFS		26
#define AIRCR_I2S_RECORD_JUSTF_MASK		(0xf << AIRCR_I2S_RECORD_JUSTF_OFFS)

#define AIRCR_I2S_SAMPLE_SIZE_OFFS		30
#define AIRCR_I2S_SAMPLE_SIZE_MASK		(0x3 << AIRCR_I2S_SAMPLE_SIZE_OFFS)

/*
** Address decoding related.
*/

#define MV_AUDIO_MAX_ADDR_DECODE_WIN 		2
#define MV_AUDIO_RECORD_WIN_NUM			0
#define MV_AUDIO_PLAYBACK_WIN_NUM		1

#define MV_AUDIO_WIN_CTRL_REG(unit, win)	(MV_AUDIO_REGS_BASE(unit) + 0xA04 + ((win)<<3))
#define MV_AUDIO_WIN_BASE_REG(unit, win)	(MV_AUDIO_REGS_BASE(unit) + 0xA00 + ((win)<<3))

#define MV_AUDIO_RECORD_WIN_CTRL_REG(unit)	MV_AUDIO_WIN_CTRL_REG(unit, MV_AUDIO_RECORD_WIN_NUM)
#define MV_AUDIO_RECORD_WIN_BASE_REG(unit)	MV_AUDIO_WIN_BASE_REG(unit, MV_AUDIO_RECORD_WIN_NUM)
#define MV_AUDIO_PLAYBACK_WIN_CTRL_REG(unit)	MV_AUDIO_WIN_CTRL_REG(unit, MV_AUDIO_PLAYBACK_WIN_NUM)
#define MV_AUDIO_PLAYBACK_WIN_BASE_REG(unit)	MV_AUDIO_WIN_BASE_REG(unit, MV_AUDIO_PLAYBACK_WIN_NUM)

/* BITs in Windows 0-3 Control and Base Registers */
#define MV_AUDIO_WIN_ENABLE_BIT               0
#define MV_AUDIO_WIN_ENABLE_MASK              (1<<MV_AUDIO_WIN_ENABLE_BIT)

#define MV_AUDIO_WIN_TARGET_OFFSET            4
#define MV_AUDIO_WIN_TARGET_MASK              (0xF<<MV_AUDIO_WIN_TARGET_OFFSET)

#define MV_AUDIO_WIN_ATTR_OFFSET              8
#define MV_AUDIO_WIN_ATTR_MASK                (0xFF<<MV_AUDIO_WIN_ATTR_OFFSET)

#define MV_AUDIO_WIN_SIZE_OFFSET              16
#define MV_AUDIO_WIN_SIZE_MASK                (0xFFFF<<MV_AUDIO_WIN_SIZE_OFFSET)

#define MV_AUDIO_WIN_BASE_OFFSET              16
#define MV_AUDIO_WIN_BASE_MASK                (0xFFFF<<MV_AUDIO_WIN_BASE_OFFSET)

#ifdef __cplusplus
}
#endif
#endif				/* __INCMVAudioRegsH */
