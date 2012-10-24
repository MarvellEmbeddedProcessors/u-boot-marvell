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
#ifndef __INCMVAudioH
#define __INCMVAudioH

#ifdef __cplusplus
extern "C" {
#endif

#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysAudioConfig.h"

/*********************************/
/* General enums and structures */
/*********************************/

	typedef struct {
		MV_U32 tclk;
	} MV_AUDIO_HAL_DATA;

/* Audio source Clocks enum*/
	typedef enum _mvAudioClock {
		AUDIO_DCO_CLK = 0,
		AUDIO_SPCR_CLK = 2,
		AUDIO_EXT_CLK = 3
	} MV_AUDIO_CLOCK;

	typedef enum _mvAudioFreq {
		AUDIO_FREQ_44_1KH = 0,	/* 11.2896Mhz */
		AUDIO_FREQ_48KH = 1,	/* 12.288Mhz */
		AUDIO_FREQ_96KH = 2,	/* 24.576Mhz */
		AUDIO_FREQ_LOWER_44_1KH = 3,	/*Lower than 11.2896MHz */
		AUDIO_FREQ_HIGHER_96KH = 4,	/*Higher than 24.576MHz */
		AUDIO_FREQ_OTHER = 7,	/*Other frequency */
	} MV_AUDIO_FREQ;

	typedef enum _mvAudioSampleFreq {
		SMAPLE_8KHZ = 0,
		SMAPLE_16KHZ,
		SMAPLE_22_05KHZ,
		SMAPLE_24KHZ,
		SMAPLE_32KHZ,
		SMAPLE_44_1KHZ,
		SMAPLE_48KHZ,
		SMAPLE_64KHZ,
		SMAPLE_88KHZ,
		SMAPLE_96KHZ,
		SMAPLE_176KHZ,
		SMAPLE_192KHZ
	} MV_AUDIO_SAMPLE_FREQ;

	typedef enum _mvAudioBurstSize {
		AUDIO_32BYTE_BURST = 1,
		AUDIO_128BYTE_BURST = 2,

	} MV_AUDIO_BURST_SIZE;

	typedef enum _mvAudioPlaybackMono {
		AUDIO_PLAY_MONO_OFF = 0,
		AUDIO_PLAY_LEFT_MONO = 1,
		AUDIO_PLAY_RIGHT_MONO = 2,
		AUDIO_PLAY_BOTH_MONO = 3,
		AUDIO_PLAY_OTHER_MONO = 4
	} MV_AUDIO_PLAYBACK_MONO;

	typedef enum _mvAudioRecordMono {
		AUDIO_REC_LEFT_MONO = 0,
		AUDIO_REC_RIGHT_MONO = 1,

	} MV_AUDIO_RECORD_MONO;

	typedef enum _mvAudioSampleSize {
		SAMPLE_32BIT = 0,
		SAMPLE_24BIT = 1,
		SAMPLE_20BIT = 2,
		SAMPLE_16BIT = 3,
		SAMPLE_16BIT_NON_COMPACT = 7
	} MV_AUDIO_SAMPLE_SIZE;

	typedef enum _mvAudioI2SJustification {
		LEFT_JUSTIFIED = 0,
		I2S_JUSTIFIED = 5,
		RISE_BIT_CLCK_JUSTIFIED = 7,
		RIGHT_JUSTIFIED = 8,

	} MV_AUDIO_I2S_JUSTIFICATION;

/* Type of Audio operations*/
	typedef enum _mvAudioOperation {
		AUDIO_PLAYBACK = 0,
		AUDIO_RECORD = 1
	} MV_AUDIO_OP;

	typedef struct _mvAudioFreqData {
		MV_AUDIO_FREQ baseFreq;	/* Control FS, selects the base frequency of the DCO */
		MV_U32 offset;	/* Offset control in which each step equals to 0.9536 ppm */

	} MV_AUDIO_FREQ_DATA;

/*********************************/
/* Play Back related structures */
/*********************************/

	typedef struct _mvAudioPlaybackCtrl {
		MV_AUDIO_BURST_SIZE burst;	/* Specifies the Burst Size of the DMA */
		MV_BOOL loopBack;	/* When Loopback is enabled, playback
					   data is looped back to be recorded */
		MV_AUDIO_PLAYBACK_MONO monoMode;	/* Mono Mode is used */
		MV_U32 bufferPhyBase;	/* Physical Address of DMA buffer */
		MV_U32 bufferSize;	/* Size of DMA buffer */
		MV_U32 intByteCount;	/* Number of bytes after which an
					   interrupt will be issued. */
		MV_AUDIO_SAMPLE_SIZE sampleSize;	/* Playback Sample Size */
	} MV_AUDIO_PLAYBACK_CTRL;

	typedef struct _mvAudioPlaypackStatus {
		MV_BOOL muteI2S;
		MV_BOOL enableI2S;
		MV_BOOL muteSPDIF;
		MV_BOOL enableSPDIF;
		MV_BOOL pause;

	} MV_AUDIO_PLAYBACK_STATUS;

	typedef struct _mvSpdifPlaybackCtrl {
		MV_BOOL nonPcm;	/* PCM or non-PCM mode */
		MV_BOOL validity;	/* Validity bit value when using
					   registers (userBitsFromMemory=0) */
		MV_BOOL underrunData;	/* If true send last frame on mute/pause/underrun
					   otherwise send 24 binary */
		MV_BOOL userBitsFromMemory;	/* otherwise from intenal registers */
		MV_BOOL validityFromMemory;	/* otherwise from internal registers */
		MV_BOOL blockStartInternally;	/* When user and valid bits are form registers
						   then this bit should be zero */
	} MV_SPDIF_PLAYBACK_CTRL;

	typedef struct _mvI2SPlaybackCtrl {
		MV_AUDIO_SAMPLE_SIZE sampleSize;
		MV_AUDIO_I2S_JUSTIFICATION justification;
		MV_BOOL sendLastFrame;	/* If true send last frame on mute/pause/underrun
					   otherwise send 64 binary */
	} MV_I2S_PLAYBACK_CTRL;

/*********************************/
/* Recording  related structures */
/*********************************/

	typedef struct _mvAudioRecordCtrl {
		MV_AUDIO_BURST_SIZE burst;	/* Recording DMA Burst Size */
		MV_AUDIO_SAMPLE_SIZE sampleSize;	/*Recording Sample Size */
		MV_BOOL mono;	/* If true then recording mono else recording stereo */
		MV_AUDIO_RECORD_MONO monoChannel;	/* Left or right moono */
		MV_U32 bufferPhyBase;	/* Physical Address of DMA buffer */
		MV_U32 bufferSize;	/* Size of DMA buffer */

		MV_U32 intByteCount;	/* Number of bytes after which an
					   interrupt will be issued. */

	} MV_AUDIO_RECORD_CTRL;

	typedef struct _mvAudioRecordStatus {
		MV_BOOL mute;
		MV_BOOL pause;
		MV_BOOL spdifEnable;
		MV_BOOL I2SEnable;

	} MV_AUDIO_RECORD_STATUS;

	typedef struct _mvSPDIFRecordStatus {
		MV_BOOL nonLinearPcm;	/* pcm non-pcm */
		MV_BOOL validPcm;	/* valid non-valid pcm */
		MV_AUDIO_SAMPLE_FREQ freq;	/* sampled frequency */

	} MV_SPDIF_RECORD_STATUS;

	typedef struct _mvI2SRecordCntrl {
		MV_AUDIO_SAMPLE_SIZE sample;	/* I2S Recording Sample Size */
		MV_AUDIO_I2S_JUSTIFICATION justf;
	} MV_I2S_RECORD_CTRL;

/*********************************/
/* Usefull Macros 				*/
/*
 -- Clocks Control and Status related --
mvAudioIsDcoLocked()
mvAudioIsSpcrLocked()
mvAudioIsPllLocked()
mvAudioAllCountersClear()
mvAudioPlayCounterClear()
mvAudioRecCounterClear()
mvAudioAllCountersStart()
mvAudioPlayCounterStart()
mvAudioRecCounterStart()
mvAudioAllCountersStop()
mvAudioPlayCounterStop()
mvAudioRecCounterStop()

 -- PlayBack related --
mvAudioIsPlaybackBusy()
mvAudioI2SPlaybackMute(mute)
mvAudioI2SPlaybackEnable(enable)
mvAudioSPDIFPlaybackMute(mute)
mvAudioSPDIFPlaybackEnable(enable)
mvAudioPlaybackPause(pause)

---- Recording ---
mvAudioSPDIFRecordingEnable(enable)
mvAudioI2SRecordingEnable(enable)
mvAudioRecordMute(mute)
mvAudioRecordPause(pause)

********************************/

/* Clocks Control and Status related*/
#define mvAudioIsDcoLocked()	\
	(ASDSR_DCO_LOCK_MASK & MV_REG_READ(MV_AUDIO_SPCR_DCO_STATUS_REG))
#define	mvAudioIsSpcrLocked()	\
	(ASDSR_SPCR_LOCK_MASK & MV_REG_READ(AUDIO_SPCR_DCO_STATUS_REG))
#define	mvAudioIsPllLocked()	\
	(ASDSR_PLL_LOCK_MASK & MV_REG_READ(AUDIO_SPCR_DCO_STATUS_REG))

#define mvAudioAllCountersClear()	\
	(MV_REG_BIT_SET(MV_AUDIO_SAMPLE_CNTR_CTRL_REG(0), (ASCCR_CLR_REC_CNTR_MASK|ASCCR_CLR_PLAY_CNTR_MASK)))
#define mvAudioPlayCounterClear()	\
	(MV_REG_BIT_SET(MV_AUDIO_SAMPLE_CNTR_CTRL_REG(0), (ASCCR_CLR_PLAY_CNTR_MASK)))
#define mvAudioRecCounterClear()	\
	(MV_REG_BIT_SET(MV_AUDIO_SAMPLE_CNTR_CTRL_REG(0), (ASCCR_CLR_REC_CNTR_MASK)))

#define	mvAudioAllCountersStart()	\
	(MV_REG_BIT_SET(MV_AUDIO_SAMPLE_CNTR_CTRL_REG(0), (ASCCR_ACTIVE_PLAY_CNTR_MASK|ASCCR_ACTIVE_REC_CNTR_MASK)))
#define mvAudioPlayCounterStart()	\
	(MV_REG_BIT_SET(MV_AUDIO_SAMPLE_CNTR_CTRL_REG(0), (ASCCR_ACTIVE_PLAY_CNTR_MASK)))
#define mvAudioRecCounterStart()	\
	(MV_REG_BIT_SET(MV_AUDIO_SAMPLE_CNTR_CTRL_REG(0), (ASCCR_ACTIVE_REC_CNTR_MASK)))

#define	mvAudioAllCountersStop()	\
	(MV_REG_BIT_RESET(MV_AUDIO_SAMPLE_CNTR_CTRL_REG(0), (ASCCR_ACTIVE_PLAY_CNTR_MASK|ASCCR_ACTIVE_REC_CNTR_MASK)))
#define mvAudioPlayCounterStop()	\
	(MV_REG_BIT_RESET(MV_AUDIO_SAMPLE_CNTR_CTRL_REG(0), (ASCCR_ACTIVE_PLAY_CNTR_MASK)))
#define mvAudioRecCounterStop()	\
	(MV_REG_BIT_RESET(MV_AUDIO_SAMPLE_CNTR_CTRL_REG(0), (ASCCR_ACTIVE_REC_CNTR_MASK)))

/* Audio PlayBack related*/
#define	mvAudioIsPlaybackBusy()	\
	(APCR_PLAY_BUSY_MASK & MV_REG_READ(MV_AUDIO_PLAYBACK_CTRL_REG(0)))

#define	mvAudioI2SPlaybackMute(mute)	\
	(void)((mute) ? (MV_REG_BIT_SET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_PLAY_I2S_MUTE_MASK)) : 	\
			 MV_REG_BIT_RESET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_PLAY_I2S_MUTE_MASK))
#define	mvAudioI2SPlaybackEnable(enable)	\
	 (void)((enable) ? (MV_REG_BIT_SET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_PLAY_I2S_ENABLE_MASK)) : 	\
			  MV_REG_BIT_RESET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_PLAY_I2S_ENABLE_MASK))

#define	mvAudioSPDIFPlaybackMute(mute)	\
	(void)((mute) ? (MV_REG_BIT_SET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_PLAY_SPDIF_MUTE_MASK)) : 	\
			 MV_REG_BIT_RESET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_PLAY_SPDIF_MUTE_MASK))
#define	mvAudioSPDIFPlaybackEnable(enable)	\
	 (void)((enable) ? (MV_REG_BIT_SET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_PLAY_SPDIF_ENABLE_MASK)) : 	\
			  MV_REG_BIT_RESET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_PLAY_SPDIF_ENABLE_MASK))

#define	mvAudioAllIfPlaybackEnable(enable)	\
	 (void)((enable) ?     \
			(MV_REG_BIT_SET(MV_AUDIO_PLAYBACK_CTRL_REG(0),     \
			(APCR_PLAY_I2S_ENABLE_MASK | APCR_PLAY_SPDIF_ENABLE_MASK))) : 	\
			MV_REG_BIT_RESET(MV_AUDIO_PLAYBACK_CTRL_REG(0),  \
			(APCR_PLAY_I2S_ENABLE_MASK | APCR_PLAY_SPDIF_ENABLE_MASK)))

#define	mvAudioPlaybackPause(pause)	\
	 (void)((pause) ? (MV_REG_BIT_SET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_PLAY_PAUSE_MASK)) : 	\
			  MV_REG_BIT_RESET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_PLAY_PAUSE_MASK))

#define	mvAudioPlaybackLoopbackEnable(enable)	\
	 (void)((enable) ? (MV_REG_BIT_SET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_LOOPBACK_MASK)) : 	\
			  MV_REG_BIT_RESET(MV_AUDIO_PLAYBACK_CTRL_REG(0), APCR_LOOPBACK_MASK))

/* Audio Recording*/
#define	mvAudioSPDIFRecordingEnable(enable)	\
	 (void)((enable) ? (MV_REG_BIT_SET(MV_AUDIO_RECORD_CTRL_REG(0), ARCR_RECORD_SPDIF_EN_MASK)) : 	\
			  MV_REG_BIT_RESET(MV_AUDIO_RECORD_CTRL_REG(0), ARCR_RECORD_SPDIF_EN_MASK))

#define	mvAudioI2SRecordingEnable(enable)	\
	 (void)((enable) ? (MV_REG_BIT_SET(MV_AUDIO_RECORD_CTRL_REG(0), ARCR_RECORD_I2S_EN_MASK)) : 	\
			  MV_REG_BIT_RESET(MV_AUDIO_RECORD_CTRL_REG(0), ARCR_RECORD_I2S_EN_MASK))

#define mvAudioRecordMute(mute)	\
	 (void)((mute) ? (MV_REG_BIT_SET(MV_AUDIO_RECORD_CTRL_REG(0), ARCR_RECORD_MUTE_MASK)) : 	\
			  MV_REG_BIT_RESET(MV_AUDIO_RECORD_CTRL_REG(0), ARCR_RECORD_MUTE_MASK))

#define mvAudioRecordPause(pause)	\
	 (void)((pause) ? (MV_REG_BIT_SET(MV_AUDIO_RECORD_CTRL_REG(0), ARCR_RECORD_PAUSE_MASK)) : 	\
			  MV_REG_BIT_RESET(MV_AUDIO_RECORD_CTRL_REG(0), ARCR_RECORD_PAUSE_MASK))

/*********************************/
/* Functions API 				*/
/*********************************/

	MV_VOID mvAudioHalInit(MV_U8 unit, MV_AUDIO_HAL_DATA *halData);
	MV_STATUS mvAudioWinInit(MV_U32 unit, MV_UNIT_WIN_INFO *addrWinMap);
	MV_STATUS mvAudioWinRead(MV_U32 unit, MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin);
	MV_STATUS mvAudioWinWrite(MV_U32 unit, MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin);

/* Clocks Control and Status related*/
	MV_STATUS mvAudioDCOCtrlSet(int unit, MV_AUDIO_FREQ_DATA *dcoCtrl);
	MV_VOID mvAudioDCOCtrlGet(int unit, MV_AUDIO_FREQ_DATA *dcoCtrl);
	MV_VOID mvAudioSpcrCtrlGet(int unit, MV_AUDIO_FREQ_DATA *spcrCtrl);

/* Audio PlayBack related*/
	MV_STATUS mvAudioPlaybackControlSet(int unit, MV_AUDIO_PLAYBACK_CTRL *ctrl);
	MV_VOID mvAudioPlaybackControlGet(int unit, MV_AUDIO_PLAYBACK_CTRL *ctrl);
	MV_VOID mvAudioPlaybackStatusGet(int unit, MV_AUDIO_PLAYBACK_STATUS *status);

/* Audio SPDIF PlayBack related*/
	MV_VOID mvSPDIFPlaybackCtrlSet(int unit, MV_SPDIF_PLAYBACK_CTRL *ctrl);
	MV_VOID mvSPDIFPlaybackCtrlGet(int unit, MV_SPDIF_PLAYBACK_CTRL *ctrl);

/* Audio I2S PlayBack related*/
	MV_STATUS mvI2SPlaybackCtrlSet(int unit, MV_I2S_PLAYBACK_CTRL *ctrl);
	MV_VOID mvI2SPlaybackCtrlGet(int unit, MV_I2S_PLAYBACK_CTRL *ctrl);

/* Audio Recording*/
	MV_STATUS mvAudioRecordControlSet(int unit, MV_AUDIO_RECORD_CTRL *ctrl);
	MV_VOID mvAudioRecordControlGet(int unit, MV_AUDIO_RECORD_CTRL *ctrl);
	MV_VOID mvAudioRecordStatusGet(int unit, MV_AUDIO_RECORD_STATUS *status);

/* SPDIF Recording Related*/
	MV_STATUS mvSPDIFRecordTclockSet(int unit);
	MV_U32 mvSPDIFRecordTclockGet(int unit);
	MV_VOID mvSPDIFRecordStatusGet(int unit, MV_SPDIF_RECORD_STATUS *status);

/* I2S Recording Related*/
	MV_STATUS mvI2SRecordCntrlSet(int unit, MV_I2S_RECORD_CTRL *ctrl);
	MV_VOID mvI2SRecordCntrlGet(int unit, MV_I2S_RECORD_CTRL *ctrl);

#ifdef __cplusplus
}
#endif
#endif
