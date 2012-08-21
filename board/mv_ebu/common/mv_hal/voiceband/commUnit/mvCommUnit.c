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

#include "voiceband/commUnit/mvCommUnit.h"

#undef MV_COMM_UNIT_DEBUG

/* defines */
#define TOTAL_CHAINS		2
#define CONFIG_RBSZ		16
#define NEXT_BUFF(buff)		((buff + 1) % TOTAL_CHAINS)
#define PREV_BUFF(buff)		(buff == 0 ? (TOTAL_CHAINS-1) : (buff-1))
#define MAX_POLL_USEC		10000	/* 10ms */

/* globals */
static MV_STATUS tdmEnable;
static MV_STATUS pcmEnable;
static MV_U8 spiMode;
static MV_U8 maxCs;
static MV_U8 sampleSize;
static MV_U8 samplingCoeff;
static MV_U16 totalChannels;
static MV_U8 prevRxBuff, nextTxBuff;
static MV_U8 *rxBuffVirt[TOTAL_CHAINS], *txBuffVirt[TOTAL_CHAINS];
static MV_ULONG rxBuffPhys[TOTAL_CHAINS], txBuffPhys[TOTAL_CHAINS];
static MV_TDM_MCDMA_RX_DESC *mcdmaRxDescPtr[TOTAL_CHAINS];
static MV_TDM_MCDMA_TX_DESC *mcdmaTxDescPtr[TOTAL_CHAINS];
static MV_ULONG mcdmaRxDescPhys[TOTAL_CHAINS], mcdmaTxDescPhys[TOTAL_CHAINS];
static MV_TDM_DPRAM_ENTRY defDpramEntry = { 0, 0, 0x1, 0x1, 0, 0, 0x1, 0, 0, 0, 0 };

#ifdef MV_COMM_UNIT_DEBUG
static MV_U8 *iqcVirt;
static MV_ULONG iqcPhys;
#endif

/* static APIs */

MV_STATUS mvCommUnitHalInit(MV_TDM_PARAMS *tdmParams, MV_TDM_HAL_DATA *halData)
{
	MV_U8 index;
	MV_U16 pcmSlot;
	MV_U32 buffSize, chan;
	MV_U32 totalRxDescSize, totalTxDescSize;
	MV_U32 rxDescPhysAddr, txDescPhysAddr, maxPoll;
	MV_TDM_DPRAM_ENTRY actDpramEntry, *pActDpramEntry;

	MV_TRC_REC("->%s\n", __func__);

	/* Initialize driver resources */
	tdmEnable = MV_FALSE;
	pcmEnable = MV_FALSE;
	spiMode = halData->spiMode;
	maxCs = halData->maxCs;
	totalChannels = tdmParams->totalChannels;
	prevRxBuff = 0;
	nextTxBuff = 0;

	/* Check parameters */
	if ((tdmParams->totalChannels > MV_TDM_TOTAL_CHANNELS) ||
	    (tdmParams->samplingPeriod > MV_TDM_MAX_SAMPLING_PERIOD)) {
		mvOsPrintf("%s: Error, bad parameters\n", __func__);
		return MV_ERROR;
	}

	/* Extract sampling period coefficient */
	samplingCoeff = (tdmParams->samplingPeriod / MV_TDM_BASE_SAMPLING_PERIOD);

	sampleSize = tdmParams->pcmFormat;

	/* Calculate single Rx/Tx buffer size */
	buffSize = (sampleSize * MV_TDM_TOTAL_CH_SAMPLES * samplingCoeff);

#ifdef MV_COMM_UNIT_DEBUG
#if 0
	iqcVirt = (MV_U8 *) mvOsIoUncachedMalloc(NULL, 516, &iqcPhys, NULL);

	if ((MV_U32) iqcVirt & 0x3f)
		mvOsPrintf("Error, unaligned IQC buffer(0x%x)\n", (MV_U32) iqcVirt);

	memset(iqcVirt, 0, 516);
#endif
#endif

	/* Allocate cached data buffers for all channels */
	TRC_REC("%s: allocate %dB for data buffers\n", __func__, (buffSize * totalChannels));
	for (index = 0; index < TOTAL_CHAINS; index++) {
		rxBuffVirt[index] =
		    (MV_U8 *) mvOsIoCachedMalloc(NULL, ((buffSize * totalChannels) + CPU_D_CACHE_LINE_SIZE),
						 &rxBuffPhys[index], NULL);
		txBuffVirt[index] =
		    (MV_U8 *) mvOsIoCachedMalloc(NULL, ((buffSize * totalChannels) + CPU_D_CACHE_LINE_SIZE),
						 &txBuffPhys[index], NULL);

		/* Check Rx buffer address & size alignment */
		if (((MV_U32) rxBuffVirt[index] | buffSize) & (CONFIG_RBSZ - 1)) {
			mvOsPrintf("%s: Error, unaligned Rx buffer address or size\n", __func__);
			return MV_ERROR;
		}

		/* Clear buffers */
		memset(rxBuffVirt[index], 0, (buffSize * totalChannels));
		memset(txBuffVirt[index], 0, (buffSize * totalChannels));
#if 0
		{
			int i;
			for (i = 0; i < 160; i += 2)
				*(MV_U16 *) (txBuffVirt[index] + i) = (MV_U16) (0xca00 + ((i / 2) + (index * 80)));
		}
#endif
		/* Flush+Inv buffers */
		mvOsCacheFlushInv(NULL, rxBuffVirt[index], (buffSize * totalChannels));
		mvOsCacheFlushInv(NULL, txBuffVirt[index], (buffSize * totalChannels));
	}

	/* Allocate non-cached MCDMA Rx/Tx descriptors */
	totalRxDescSize = totalChannels * sizeof(MV_TDM_MCDMA_RX_DESC);
	totalTxDescSize = totalChannels * sizeof(MV_TDM_MCDMA_TX_DESC);

	TRC_REC("%s: allocate %dB for Rx/Tx descriptors\n", __func__, totalRxDescSize);
	for (index = 0; index < TOTAL_CHAINS; index++) {
		mcdmaRxDescPtr[index] = (MV_TDM_MCDMA_RX_DESC *) mvOsIoUncachedMalloc(NULL, totalRxDescSize,
										      &mcdmaRxDescPhys[index], NULL);
		mcdmaTxDescPtr[index] = (MV_TDM_MCDMA_TX_DESC *) mvOsIoUncachedMalloc(NULL, totalTxDescSize,
										      &mcdmaTxDescPhys[index], NULL);

		/* Check descriptors alignment */
		if (((MV_U32) mcdmaRxDescPtr[index] | (MV_U32) mcdmaTxDescPtr[index]) &
		    (sizeof(MV_TDM_MCDMA_RX_DESC) - 1)) {
			mvOsPrintf("%s: Error, unaligned MCDMA Rx/Tx descriptors\n", __func__);
			return MV_ERROR;
		}

		/* Clear descriptors */
		memset(mcdmaRxDescPtr[index], 0, totalRxDescSize);
		memset(mcdmaTxDescPtr[index], 0, totalTxDescSize);
	}

	/* Initialize descriptors fields */
	for (chan = 0; chan < totalChannels; chan++) {
		for (index = 0; index < TOTAL_CHAINS; index++) {
			/* Associate data buffers to descriptors physBuffPtr */
			((MV_TDM_MCDMA_RX_DESC *) (mcdmaRxDescPtr[index] + chan))->physBuffPtr =
			    (MV_U32) (rxBuffPhys[index] + (chan * buffSize));
			((MV_TDM_MCDMA_TX_DESC *) (mcdmaTxDescPtr[index] + chan))->physBuffPtr =
			    (MV_U32) (txBuffPhys[index] + (chan * buffSize));

			/* Build cyclic descriptors chain for each channel */
			((MV_TDM_MCDMA_RX_DESC *) (mcdmaRxDescPtr[index] + chan))->physNextDescPtr =
			    (MV_U32) (mcdmaRxDescPhys[((index + 1) % TOTAL_CHAINS)] +
				      (chan * sizeof(MV_TDM_MCDMA_RX_DESC)));

			((MV_TDM_MCDMA_TX_DESC *) (mcdmaTxDescPtr[index] + chan))->physNextDescPtr =
			    (MV_U32) (mcdmaTxDescPhys[((index + 1) % TOTAL_CHAINS)] +
				      (chan * sizeof(MV_TDM_MCDMA_TX_DESC)));

			/* Set Byte_Count/Buffer_Size Rx descriptor fields */
			((MV_TDM_MCDMA_RX_DESC *) (mcdmaRxDescPtr[index] + chan))->byteCnt = 0;
			((MV_TDM_MCDMA_RX_DESC *) (mcdmaRxDescPtr[index] + chan))->buffSize = buffSize;

			/* Set Shadow_Byte_Count/Byte_Count Tx descriptor fields */
			((MV_TDM_MCDMA_TX_DESC *) (mcdmaTxDescPtr[index] + chan))->shadowByteCnt = buffSize;
			((MV_TDM_MCDMA_TX_DESC *) (mcdmaTxDescPtr[index] + chan))->byteCnt = buffSize;

			/* Set Command/Status Rx/Tx descriptor fields */
			((MV_TDM_MCDMA_RX_DESC *) (mcdmaRxDescPtr[index] + chan))->cmdStatus =
			    (CONFIG_MCDMA_DESC_CMD_STATUS);
			((MV_TDM_MCDMA_TX_DESC *) (mcdmaTxDescPtr[index] + chan))->cmdStatus =
			    (CONFIG_MCDMA_DESC_CMD_STATUS);
		}
	}

	/* Poll MCDMA for reset completion */
	maxPoll = 0;
	while ((maxPoll < MAX_POLL_USEC) && !(MV_REG_READ(MCDMA_GLOBAL_CONTROL_REG) & MCDMA_RID_MASK)) {
		mvOsUDelay(1);
		maxPoll++;
	}

	if (maxPoll >= MAX_POLL_USEC) {
		mvOsPrintf("Error, MCDMA reset completion timout\n");
		return MV_ERROR;
	}

	/* Poll MCSC for RAM initialization done */
	if (!(MV_REG_READ(MCSC_GLOBAL_INT_CAUSE_REG) & MCSC_GLOBAL_INT_CAUSE_INIT_DONE_MASK)) {
		maxPoll = 0;
		while ((maxPoll < MAX_POLL_USEC) &&
		       !(MV_REG_READ(MCSC_GLOBAL_INT_CAUSE_REG) & MCSC_GLOBAL_INT_CAUSE_INIT_DONE_MASK)) {
			mvOsUDelay(1);
			maxPoll++;
		}

		if (maxPoll >= MAX_POLL_USEC) {
			mvOsPrintf("Error, MCDMA RAM initialization timout\n");
			return MV_ERROR;
		}
	}

	/***************************************************************/
	/* MCDMA Configuration(use default MCDMA linked-list settings) */
	/***************************************************************/
	/* Set Rx Service Queue Arbiter Weight Register */
	MV_REG_WRITE(RX_SERVICE_QUEUE_ARBITER_WEIGHT_REG,
			(MV_REG_READ(RX_SERVICE_QUEUE_ARBITER_WEIGHT_REG) & ~(0x1f << 24))); /*| MCDMA_RSQW_MASK));*/

	/* Set Tx Service Queue Arbiter Weight Register */
	MV_REG_WRITE(TX_SERVICE_QUEUE_ARBITER_WEIGHT_REG,
			(MV_REG_READ(TX_SERVICE_QUEUE_ARBITER_WEIGHT_REG) & ~(0x1f << 24)));	/*| MCDMA_TSQW_MASK));*/

	for (chan = 0; chan < totalChannels; chan++) {
		/* Set RMCCx */
		MV_REG_WRITE(MCDMA_RECEIVE_CONTROL_REG(chan), CONFIG_RMCCx);

		/* Set TMCCx */
		MV_REG_WRITE(MCDMA_TRANSMIT_CONTROL_REG(chan), CONFIG_TMCCx);
	}

	/**********************/
	/* MCSC Configuration */
	/**********************/
	for (chan = 0; chan < totalChannels; chan++) {
		MV_REG_WRITE(MCSC_CHx_RECEIVE_CONFIG_REG(chan), CONFIG_MRCRx);
		MV_REG_WRITE(MCSC_CHx_TRANSMIT_CONFIG_REG(chan), CONFIG_MTCRx);
	}

	/***********************************************/
	/* Shared Bus to Crossbar Bridge Configuration */
	/***********************************************/
	/* Set Timeout Counter Register */
	MV_REG_WRITE(TIME_OUT_COUNTER_REG, (MV_REG_READ(TIME_OUT_COUNTER_REG) | TIME_OUT_THRESHOLD_COUNT_MASK));

	/*************************************************/
	/* Time Division Multiplexing(TDM) Configuration */
	/*************************************************/
	/* Reset all Rx/Tx DPRAM entries to default value */
	pActDpramEntry = &actDpramEntry;
	memcpy(&actDpramEntry, &defDpramEntry, sizeof(MV_TDM_DPRAM_ENTRY));
	for (index = 0; index < MV_TDM_MAX_DPRAM_ENTRIES; index++) {
		MV_REG_WRITE(FLEX_TDM_RDPR_REG(index), *((MV_U32 *) pActDpramEntry));
		MV_REG_WRITE(FLEX_TDM_TDPR_REG(index), *((MV_U32 *) pActDpramEntry));
	}

	/* Fill active Rx/Tx DPRAM entries */
	pActDpramEntry->mask = 0xff;
	for (chan = 0; chan < totalChannels; chan++) {
		pcmSlot = tdmParams->pcmSlot[chan];
		pActDpramEntry->ch = chan;
		for (index = 0; index < sampleSize; index++) {
			MV_REG_WRITE(FLEX_TDM_RDPR_REG((pcmSlot + index)), *((MV_U32 *) pActDpramEntry));
			MV_REG_WRITE(FLEX_TDM_TDPR_REG((pcmSlot + index)), *((MV_U32 *) pActDpramEntry));
		}
	}

	/* Fill non-active Rx/Tx DPRAM entries(except last) */
	pActDpramEntry->mask = 0;
	pActDpramEntry->ch = 0;

	/* Fill last Tx DPRAM entry('LAST'=1) */
	pActDpramEntry->last = 1;
	MV_REG_WRITE(FLEX_TDM_TDPR_REG((MV_TDM_MAX_DPRAM_ENTRIES - 1)), *((MV_U32 *) pActDpramEntry));
#if !defined(MV_COMM_UNIT_FSYNC_STRB_SUPPORT)
	MV_REG_WRITE(FLEX_TDM_RDPR_REG((MV_TDM_MAX_DPRAM_ENTRIES - 1)), *((MV_U32 *) pActDpramEntry));
#else
	/* Spread last Rx byte on 8 entries in bit mode */
	pActDpramEntry->last = 0;
	pActDpramEntry->mask = 1;
	pActDpramEntry->byte = 0;
	pActDpramEntry->tbs = 0;
	for (index = (MV_TDM_MAX_DPRAM_ENTRIES - 1); index < (MV_TDM_MAX_DPRAM_ENTRIES + 7); index++) {
		if (index == (MV_TDM_MAX_DPRAM_ENTRIES + 6)) {
			pActDpramEntry->last = 1;
			pActDpramEntry->tbs = 1;
			pActDpramEntry->strb = 3;
		}

		MV_REG_WRITE(FLEX_TDM_RDPR_REG(index), *((MV_U32 *) pActDpramEntry));
	}
#endif /* MV_COMM_UNIT_FSYNC_STRB_SUPPORT */

	/* Set TDM_CLK_AND_SYNC_CONTROL register */
	MV_REG_WRITE(TDM_CLK_AND_SYNC_CONTROL_REG, CONFIG_TDM_CLK_AND_SYNC_CONTROL);

	/* Set TDM TCR register */
	MV_REG_WRITE(FLEX_TDM_CONFIG_REG, (MV_REG_READ(FLEX_TDM_CONFIG_REG) | CONFIG_FLEX_TDM_CONFIG));

	/* Set TDM_CLK_DIVIDER_CONTROL register */
	/*MV_REG_WRITE(TDM_CLK_DIVIDER_CONTROL_REG, TDM_RX_FIXED_DIV_ENABLE_MASK); */

	/* Enable SLIC/s interrupt detection(before Rx/Tx are active) */
	/*MV_REG_WRITE(TDM_MASK_REG, TDM_SLIC_INT); */

	/* Errata(#1) */
	MV_REG_BIT_RESET(0xb8ad0, BIT16);
	MV_REG_BIT_RESET(0xb8ad4, BIT16);
	MV_REG_BIT_RESET(0xb8ad8, BIT16);

#ifdef MV_COMM_UNIT_DEBUG
	/* Enable IQC */
	/* MV_REG_WRITE(0xb2814, (MV_U32)iqcVirt);
	   MV_REG_WRITE(0xb2818, (MV_U32)(iqcVirt+512));
	   MV_REG_WRITE(0xb2824, 0xffffffff); */
#endif

	/**********************************************************************/
	/* Time Division Multiplexing(TDM) Interrupt Controller Configuration */
	/**********************************************************************/
	/* Clear TDM cause and mask registers */
	MV_REG_WRITE(COMM_UNIT_TOP_MASK_REG, 0);
	MV_REG_WRITE(TDM_MASK_REG, 0);
	MV_REG_WRITE(COMM_UNIT_TOP_CAUSE_REG, 0);
	MV_REG_WRITE(TDM_CAUSE_REG, 0);

	/* Clear MCSC cause and mask registers(except InitDone bit) */
	MV_REG_WRITE(MCSC_GLOBAL_INT_MASK_REG, 0);
	MV_REG_WRITE(MCSC_EXTENDED_INT_MASK_REG, 0);
	MV_REG_WRITE(MCSC_GLOBAL_INT_CAUSE_REG, MCSC_GLOBAL_INT_CAUSE_INIT_DONE_MASK);
	MV_REG_WRITE(MCSC_EXTENDED_INT_CAUSE_REG, 0);

	/* Set current Rx/Tx descriptors  */
	for (chan = 0; chan < totalChannels; chan++) {
		rxDescPhysAddr = mcdmaRxDescPhys[0] + (chan * sizeof(MV_TDM_MCDMA_RX_DESC));
		txDescPhysAddr = mcdmaTxDescPhys[0] + (chan * sizeof(MV_TDM_MCDMA_TX_DESC));
		MV_REG_WRITE(MCDMA_CURRENT_RECEIVE_DESC_PTR_REG(chan), rxDescPhysAddr);
		MV_REG_WRITE(MCDMA_CURRENT_TRANSMIT_DESC_PTR_REG(chan), txDescPhysAddr);
	}

	/* Set Rx/Tx periodical interrupts */
	MV_REG_WRITE(VOICE_PERIODICAL_INT_CONTROL_REG, CONFIG_VOICE_PERIODICAL_INT_CONTROL);

	/* MCSC Global Tx Enable */
	MV_REG_BIT_SET(MCSC_GLOBAL_CONFIG_REG, MCSC_GLOBAL_CONFIG_TXEN_MASK);

	/* Enable MCSC-Tx & MCDMA-Rx */
	for (chan = 0; chan < totalChannels; chan++) {
		/* Enable Tx in TMCCx */
		MV_REG_BIT_SET(MCSC_CHx_TRANSMIT_CONFIG_REG(chan), MTCRx_ET_MASK);

		/* Enable Rx in: MCRDPx */
		MV_REG_BIT_SET(MCDMA_RECEIVE_CONTROL_REG(chan), MCDMA_ERD_MASK);
	}

	/* MCSC Global Rx Enable */
	MV_REG_BIT_SET(MCSC_GLOBAL_CONFIG_REG, MCSC_GLOBAL_CONFIG_RXEN_MASK);

	/* Enable MCSC-Rx & MCDMA-Tx */
	for (chan = 0; chan < totalChannels; chan++) {
		/* Enable Rx in RMCCx */
		MV_REG_BIT_SET(MCSC_CHx_RECEIVE_CONFIG_REG(chan), MRCRx_ER_MASK);

		/* Enable Tx in MCTDPx */
		MV_REG_BIT_SET(MCDMA_TRANSMIT_CONTROL_REG(chan), MCDMA_TXD_MASK);
	}

	/* Wait enough time until MCDMA FIFOs are loaded with data for all active channels */
	mvOsDelay(100);

	/* Enable TDM */
	MV_REG_BIT_SET(FLEX_TDM_CONFIG_REG, TDM_TEN_MASK);

	/* Mark TDM I/F as enabled */
	tdmEnable = MV_TRUE;

	/* MCSC Enter Hunt State */
	for (chan = 0; chan < totalChannels; chan++)
		MV_REG_BIT_SET(MCSC_CHx_RECEIVE_CONFIG_REG(chan), MRCRx_ENTER_HUNT_MASK);


	/* Poll for Enter Hunt Execution Status */
	for (chan = 0; chan < totalChannels; chan++) {
		maxPoll = 0;
		while ((maxPoll < MAX_POLL_USEC) &&
		       !(MV_REG_READ(MCSC_CHx_COMM_EXEC_STAT_REG(chan)) & MCSC_EH_E_STAT_MASK)) {
			mvOsUDelay(1);
			maxPoll++;
		}

		if (maxPoll >= MAX_POLL_USEC) {
			mvOsPrintf("%s: Error, enter hunt execution timeout(ch%d)\n", __func__, chan);
			return MV_ERROR;
		}

		MV_REG_BIT_RESET(MCSC_CHx_RECEIVE_CONFIG_REG(chan), MRCRx_ENTER_HUNT_MASK);
	}

#ifdef MV_COMM_UNIT_DEBUG
	mvCommUnitShow();
#endif
	MV_TRC_REC("<-%s\n", __func__);
	return MV_OK;
}

MV_VOID mvCommUnitRelease(MV_VOID)
{
	MV_U32 buffSize, totalRxDescSize, totalTxDescSize, index, chan;
	MV_U32 maxPoll;

	MV_TRC_REC("->%s\n", __func__);

	/* Disable Rx/Tx periodical interrupts */
	MV_REG_WRITE(VOICE_PERIODICAL_INT_CONTROL_REG, 0xffffffff);

	/**********************************/
	/* Stop MCSC/Rx, MCDMA/Tx and TDM */
	/**********************************/
	/* MCSC Rx Abort */
	for (chan = 0; chan < totalChannels; chan++)
		MV_REG_BIT_SET(MCSC_CHx_RECEIVE_CONFIG_REG(chan), MRCRx_ABORT_MASK);

	for (chan = 0; chan < totalChannels; chan++) {
		maxPoll = 0;
		while ((maxPoll < MAX_POLL_USEC)
		       && !(MV_REG_READ(MCSC_CHx_COMM_EXEC_STAT_REG(chan)) & MCSC_ABR_E_STAT_MASK)) {
			mvOsUDelay(1);
			maxPoll++;
		}

		if (maxPoll >= MAX_POLL_USEC) {
			mvOsPrintf("%s: Error, MCSC Rx abort timeout(ch%d)\n", __func__, chan);
			return;
		}

		MV_REG_BIT_RESET(MCSC_CHx_RECEIVE_CONFIG_REG(chan), MRCRx_ABORT_MASK);
	}

	/* MCDMA Tx Abort */
	for (chan = 0; chan < totalChannels; chan++)
		MV_REG_BIT_SET(MCDMA_TRANSMIT_CONTROL_REG(chan), MCDMA_AT_MASK);

	for (chan = 0; chan < totalChannels; chan++) {
		maxPoll = 0;
		while ((maxPoll < MAX_POLL_USEC) && (MV_REG_READ(MCDMA_RECEIVE_CONTROL_REG(chan)) & MCDMA_ERD_MASK)) {
			mvOsUDelay(1);
			maxPoll++;
		}

		if (maxPoll >= MAX_POLL_USEC) {
			mvOsPrintf("%s: Error, MCDMA Rx abort timeout(ch%d)\n", __func__, chan);
			return;
		}

		maxPoll = 0;
		while ((maxPoll < MAX_POLL_USEC) && (MV_REG_READ(MCDMA_TRANSMIT_CONTROL_REG(chan)) & MCDMA_AT_MASK)) {
			mvOsUDelay(1);
			maxPoll++;
		}

		if (maxPoll >= MAX_POLL_USEC) {
			mvOsPrintf("%s: Error, MCDMA Tx abort timeout(ch%d)\n", __func__, chan);
			return;
		}
	}

	/* Clear MCSC Rx/Tx channel enable */
	for (chan = 0; chan < totalChannels; chan++) {
		MV_REG_BIT_RESET(MCSC_CHx_RECEIVE_CONFIG_REG(chan), MRCRx_ER_MASK);
		MV_REG_BIT_RESET(MCSC_CHx_TRANSMIT_CONFIG_REG(chan), MTCRx_ET_MASK);
		/*MV_REG_BIT_RESET(MCDMA_RECEIVE_CONTROL_REG(chan), MCDMA_ERD_MASK);
		MV_REG_BIT_RESET(MCDMA_TRANSMIT_CONTROL_REG(chan), MCDMA_TXD_MASK);*/
	}

	/* MCSC Global Rx/Tx Disable */
	MV_REG_BIT_RESET(MCSC_GLOBAL_CONFIG_REG, MCSC_GLOBAL_CONFIG_RXEN_MASK);
	MV_REG_BIT_RESET(MCSC_GLOBAL_CONFIG_REG, MCSC_GLOBAL_CONFIG_TXEN_MASK);

	/* Disable TDM */
	MV_REG_BIT_RESET(FLEX_TDM_CONFIG_REG, TDM_TEN_MASK);

	/* Calculate total Rx/Tx buffer size */
	buffSize = (sampleSize * MV_TDM_TOTAL_CH_SAMPLES * samplingCoeff * totalChannels)
	    + CPU_D_CACHE_LINE_SIZE;

	/* Calculate total MCDMA Rx/Tx descriptors chain size */
	totalRxDescSize = totalChannels * sizeof(MV_TDM_MCDMA_RX_DESC);
	totalTxDescSize = totalChannels * sizeof(MV_TDM_MCDMA_TX_DESC);

	for (index = 0; index < TOTAL_CHAINS; index++) {
		/* Release Rx/Tx data buffers */
		mvOsIoCachedFree(NULL, buffSize, rxBuffPhys[index], rxBuffVirt[index], 0);
		mvOsIoCachedFree(NULL, buffSize, txBuffPhys[index], txBuffVirt[index], 0);

		/* Release MCDMA Rx/Tx descriptors */
		mvOsIoUncachedFree(NULL, totalRxDescSize, mcdmaRxDescPhys[index], mcdmaRxDescPtr[index], 0);
		mvOsIoUncachedFree(NULL, totalTxDescSize, mcdmaTxDescPhys[index], mcdmaTxDescPtr[index], 0);
	}

#ifdef MV_COMM_UNIT_DEBUG
	/*mvOsIoUncachedFree(NULL, 516, iqcPhys, iqcVirt, 0); */
#endif

	MV_TRC_REC("<-%s\n", __func__);
}

MV_VOID mvCommUnitPcmStart(MV_VOID)
{
	MV_U32 maskReg;

	MV_TRC_REC("->%s\n", __func__);

	if (pcmEnable == MV_FALSE) {
		/* Mark PCM I/F as enabled  */
		pcmEnable = MV_TRUE;

		/* Clear TDM cause and mask registers */
		MV_REG_WRITE(COMM_UNIT_TOP_MASK_REG, 0);
		MV_REG_WRITE(TDM_MASK_REG, 0);
		MV_REG_WRITE(COMM_UNIT_TOP_CAUSE_REG, 0);
		MV_REG_WRITE(TDM_CAUSE_REG, 0);

		/* Clear MCSC cause and mask registers(except InitDone bit) */
		MV_REG_WRITE(MCSC_GLOBAL_INT_MASK_REG, 0);
		MV_REG_WRITE(MCSC_EXTENDED_INT_MASK_REG, 0);
		MV_REG_WRITE(MCSC_GLOBAL_INT_CAUSE_REG, MCSC_GLOBAL_INT_CAUSE_INIT_DONE_MASK);
		MV_REG_WRITE(MCSC_EXTENDED_INT_CAUSE_REG, 0);

		/* Enable unit interrupts */
		maskReg = MV_REG_READ(TDM_MASK_REG);
		MV_REG_WRITE(TDM_MASK_REG, (maskReg | CONFIG_TDM_CAUSE));
		MV_REG_WRITE(COMM_UNIT_TOP_MASK_REG, CONFIG_COMM_UNIT_TOP_MASK);
	}

	MV_TRC_REC("<-%s\n", __func__);
}

MV_VOID mvCommUnitPcmStop(MV_VOID)
{
	MV_U32 index, buffSize;

	MV_TRC_REC("->%s\n", __func__);

	if (pcmEnable == MV_TRUE) {
		/* Mark PCM I/F as disabled  */
		pcmEnable = MV_FALSE;

		/* Calculate total Rx/Tx buffer size */
		buffSize = (sampleSize * MV_TDM_TOTAL_CH_SAMPLES * samplingCoeff * totalChannels);

		/* CommUnit still active, only mask interrupts */
		/* Clear TDM cause and mask registers */
		MV_REG_WRITE(COMM_UNIT_TOP_MASK_REG, 0);
		MV_REG_WRITE(TDM_MASK_REG, 0);
		MV_REG_WRITE(COMM_UNIT_TOP_CAUSE_REG, 0);
		MV_REG_WRITE(TDM_CAUSE_REG, 0);

		/* Clear MCSC cause and mask registers(except InitDone bit) */
		MV_REG_WRITE(MCSC_GLOBAL_INT_MASK_REG, 0);
		MV_REG_WRITE(MCSC_EXTENDED_INT_MASK_REG, 0);
		MV_REG_WRITE(MCSC_GLOBAL_INT_CAUSE_REG, MCSC_GLOBAL_INT_CAUSE_INIT_DONE_MASK);
		MV_REG_WRITE(MCSC_EXTENDED_INT_CAUSE_REG, 0);

#if 0
		/* Clear Rx/Tx buffers */
		for (index = 0; index < TOTAL_CHAINS; index++) {
			memset(rxBuffVirt[index], 0, buffSize);
			memset(txBuffVirt[index], 0, buffSize);

			/* Flush+Inv buffers */
			mvOsCacheFlushInv(NULL, rxBuffVirt[index], buffSize);
			mvOsCacheFlushInv(NULL, txBuffVirt[index], buffSize);
		}
#endif
		/* Enable SLIC/s interrupt detection */
		/*MV_REG_WRITE(TDM_MASK_REG, TDM_SLIC_INT); */
	}

	MV_TRC_REC("<-%s\n", __func__);
}

MV_STATUS mvCommUnitTx(MV_U8 *pTdmTxBuff)
{
	MV_U32 buffSize;

	MV_TRC_REC("->%s\n", __func__);

	/* Calculate total Tx buffer size */
	buffSize = (sampleSize * MV_TDM_TOTAL_CH_SAMPLES * samplingCoeff * totalChannels);

	/* Flush+Invalidate the next Tx buffer */
	mvOsCacheFlushInv(NULL, pTdmTxBuff, buffSize);

	MV_TRC_REC("<-%s\n", __func__);
	return MV_OK;
}

MV_STATUS mvCommUnitRx(MV_U8 *pTdmRxBuff)
{
	MV_U32 buffSize;

	MV_TRC_REC("->%s\n", __func__);

	/* Calculate total Rx buffer size */
	buffSize = (sampleSize * MV_TDM_TOTAL_CH_SAMPLES * samplingCoeff * totalChannels);

	/* Invalidate current received buffer from cache */
	mvOsCacheInvalidate(NULL, pTdmRxBuff, buffSize);

	MV_TRC_REC("<-%s\n", __func__);
	return MV_OK;
}

/* Low level TDM interrupt service routine */
MV_VOID mvCommUnitIntLow(MV_TDM_INT_INFO *pTdmIntInfo)
{
	MV_U32 causeReg, maskReg, causeAndMask;
	MV_U32 slicInt = 0, intAckBits = 0, currRxDesc, currTxDesc;
	MV_U8 cs, index;

	MV_TRC_REC("->%s\n", __func__);

	/* Read TDM cause & mask registers */
	causeReg = MV_REG_READ(TDM_CAUSE_REG);
	maskReg = MV_REG_READ(TDM_MASK_REG);

	MV_TRC_REC("CAUSE(0x%x), MASK(0x%x)\n", causeReg, maskReg);

	/* Refer only to unmasked bits */
	causeAndMask = causeReg & maskReg;

	/* Reset ISR params */
	pTdmIntInfo->tdmRxBuff = NULL;
	pTdmIntInfo->tdmTxBuff = NULL;
	pTdmIntInfo->intType = MV_EMPTY_INT;

#if 0
	/* Handle SLIC interrupt */
	slicInt = (causeAndMask & TDM_SLIC_INT);
	if (slicInt) {
		MV_TRC_REC("SLIC interrupt !!!\n");
		pTdmIntInfo->intType |= MV_PHONE_INT;
		for (cs = 0; cs < maxCs; cs++) {
			if (slicInt & MV_BIT_MASK(cs + EXT_INT_SLIC0_OFFS)) {
				pTdmIntInfo->cs = cs;
				mvOsPrintf("");
				intAckBits |= MV_BIT_MASK(cs + EXT_INT_SLIC0_OFFS);
				break;
			}

		}
		mvOsPrintf("pTdmIntInfo->cs = %d\n", pTdmIntInfo->cs);
	}
#endif
	/* Return in case TDM is disabled */
	if (tdmEnable == MV_FALSE) {
		MV_TRC_REC("TDM is disabled - quit low level ISR\n");
		MV_REG_WRITE(TDM_CAUSE_REG, ~intAckBits);
		return;
	}

	/* Handle TDM Error/s */
	if (causeAndMask & TDM_ERROR_INT) {
		mvOsPrintf("TDM Error: TDM_CAUSE_REG = 0x%x\n", causeReg);
		pTdmIntInfo->intType |= MV_ERROR_INT;
		intAckBits |= (causeAndMask & TDM_ERROR_INT);
	}

	/* Handle Tx */
	if (causeAndMask & TDM_TX_INT) {
		currTxDesc = MV_REG_READ(MCDMA_CURRENT_TRANSMIT_DESC_PTR_REG(0));
		MV_TRC_REC("currTxDesc = 0x%x\n", currTxDesc);
		for (index = 0; index < TOTAL_CHAINS; index++) {
			if (currTxDesc == mcdmaTxDescPhys[index]) {
				nextTxBuff = NEXT_BUFF(index);
				break;
			}
		}
		MV_TRC_REC("Tx interrupt(nextTxBuff=%d)!!!\n", nextTxBuff);
		pTdmIntInfo->tdmTxBuff = txBuffVirt[nextTxBuff];
		pTdmIntInfo->intType |= MV_TX_INT;
		intAckBits |= TDM_TX_INT;
	}

	/* Handle Rx */
	if (causeAndMask & TDM_RX_INT) {
		currRxDesc = MV_REG_READ(MCDMA_CURRENT_RECEIVE_DESC_PTR_REG(0));
		MV_TRC_REC("currRxDesc = 0x%x\n", currRxDesc);

		for (index = 0; index < TOTAL_CHAINS; index++) {
			if (currRxDesc == mcdmaRxDescPhys[index]) {
				prevRxBuff = PREV_BUFF(index);
				break;
			}
		}

		MV_TRC_REC("Rx interrupt(prevRxBuff=%d)!!!\n", prevRxBuff);
		pTdmIntInfo->tdmRxBuff = rxBuffVirt[prevRxBuff];
		pTdmIntInfo->intType |= MV_RX_INT;
		intAckBits |= TDM_RX_INT;
	}

	/* Clear TDM interrupts */
	MV_REG_WRITE(TDM_CAUSE_REG, ~intAckBits);

	TRC_REC("<-%s\n", __func__);
	return;
}

MV_VOID mvCommUnitIntEnable(MV_U8 deviceId)
{
	MV_REG_BIT_SET(MV_GPP_IRQ_MASK_REG(0), BIT23);
}

MV_VOID mvCommUnitIntDisable(MV_U8 deviceId)
{
	MV_REG_BIT_RESET(MV_GPP_IRQ_MASK_REG(0), BIT23);
}

MV_VOID mvCommUnitShow(MV_VOID)
{
	MV_U32 index;

	/* Dump data buffers & descriptors addresses */
	for (index = 0; index < TOTAL_CHAINS; index++) {
		mvOsPrintf("Rx Buff(%d): virt = 0x%x, phys = 0x%x\n", index, (MV_U32) rxBuffVirt[index],
			   (MV_U32) rxBuffPhys[index]);
		mvOsPrintf("Tx Buff(%d): virt = 0x%x, phys = 0x%x\n", index, (MV_U32) txBuffVirt[index],
			   (MV_U32) txBuffPhys[index]);

		mvOsPrintf("Rx Desc(%d): virt = 0x%x, phys = 0x%x\n", index,
			   (MV_U32) mcdmaRxDescPtr[index], (MV_U32) mcdmaRxDescPhys[index]);

		mvOsPrintf("Tx Desc(%d): virt = 0x%x, phys = 0x%x\n", index,
			   (MV_U32) mcdmaTxDescPtr[index], (MV_U32) mcdmaTxDescPhys[index]);

	}
}
