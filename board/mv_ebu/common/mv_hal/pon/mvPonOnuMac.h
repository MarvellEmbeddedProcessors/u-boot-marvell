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

#ifndef _ONU_PON_MAC_H
#define _ONU_PON_MAC_H

/* PON port */
#define ONU_PON_PORT_DOWN                  (0)
#define ONU_PON_PORT_UP                    (1)

/******************************************************************************/
/******************************************************************************/
/* ========================================================================== */
/* ========================================================================== */
/* ==                                                                      == */
/* ==           =========   =========   =========   ===       ==           == */
/* ==           =========   =========   =========   ====      ==           == */
/* ==           ==          ==     ==   ==     ==   == ==     ==           == */
/* ==           ==          ==     ==   ==     ==   ==  ==    ==           == */
/* ==           =========   =========   ==     ==   ==   ==   ==           == */
/* ==           =========   =========   ==     ==   ==    ==  ==           == */
/* ==           ==     ==   ==          ==     ==   ==     == ==           == */
/* ==           ==     ==   ==          ==     ==   ==      ====           == */
/* ==           =========   ==          =========   ==       ===           == */
/* ==           =========   ==          =========   ==        ==           == */
/* ==                                                                      == */
/* ========================================================================== */
/* ========================================================================== */
/******************************************************************************/
/******************************************************************************/

/* Definitions
   ------------------------------------------------------------------------------*/
#define UINT8_MASK                          (0xFF)
#define UINT8_OFFSET                        (8)
#define UINT8_NUM_OF_BITS                   (8)

/* Frame lengths */
#define GPON_MAX_GEM_PAYLOAD_LEN            (0x800)             /* GEM payload length */
#define GPON_MIN_ETH_FRAME_LEN              (0x40)              /* Min ETH frame length */
#define GPON_MAX_ETH_FRAME_LEN              (0x800)             /* Max ETH frame length */
#define GPON_MIN_OMCI_FRAME_LEN             (0x30)              /* Min OMCI frame length */
#define GPON_MAX_OMCI_FRAME_LEN             (0x30)              /* Max OMCI frame length */

/* Frame Delineation Register */
#define GPON_FRAME_DELINEATION_FR_1244      (0x0000)            /* DS Frequency - 1.244Gb */
#define GPON_FRAME_DELINEATION_FR_2488      (0x0001)            /* DS Frequency - 2.488Gb */
#define GPON_FRAME_DELINEATION_FR           (GPON_FRAME_DELINEATION_FR_1244)
#define GPON_FRAME_DELINEATION_M1           (0x0002)            /* M1 field */
#define GPON_FRAME_DELINEATION_M2           (0x0005)            /* M2 field */

/* ONU-ID Register */
#define GPON_ONU_ID_ONU_ID_DEF              (ONU_GPON_UNDEFINED_ONU_ID) /* ONU-ID Field */

/* ONU State Register */
#define GPON_ONU_ID_STATE_01                (ONU_GPON_01_INIT)  /* ONU State 01 */

/* ONU BIP period Counter Register */
#define GPON_BIP_PERIOD_CNTR                (0)                                 /* BIP period Counter */

/* TX BW Translation Register */
#define GPON_BW_MAP_IDLE_TCONT              (0)

/* TX Delay Register - const */
#define GPON_TX_DELAY_TD_622                (0x0AA1)            /* Const Delay */
#define GPON_TX_DELAY_TD_1244               (0x15E9)            /* Const Delay */

/* TX Delay Register - equilization */
#define GPON_TX_EQUAL_DELAY_TD              (0x0000)            /* Equilization Delay */

/* TX Preamble Register */
#define ONU_TX_PREAMBLE_TYPE_03_DEF_US_155  (0x0001)                                    /* Preamble type 3 counter - default US rate 155M */
#define ONU_TX_PREAMBLE_TYPE_03_DEF_US_622  (0x0003)                                    /* Preamble type 3 counter - default US rate 622M */
#define ONU_TX_PREAMBLE_TYPE_03_DEF_US_1244 (0x0005)                                    /* Preamble type 3 counter - default US rate 1244M */
#define ONU_TX_PREAMBLE_TYPE_03_DEF_US_2488 (0x000D)                                    /* Preamble type 3 counter - default US rate 2488M */

#define ONU_TX_PREAMBLE_TYPE_03_RATE_622    (0x0008)                                    /* Preamble type 3 counter - US rate 622M */
#define ONU_TX_PREAMBLE_TYPE_03_RATE_1244   (0x0010)                                    /* Preamble type 3 counter - US rate 1244M */
#define ONU_TX_PREAMBLE_TYPE_03_RATE_2488   (0x0020)                                    /* Preamble type 3 counter - US rate 2488M */

#define ONU_TX_PREAMBLE_TYPE_01_P           (0x00FF)                                    /* Preamble type 1 value */
#define ONU_TX_PREAMBLE_TYPE_01_PC          (0)                                         /* Preamble type 1 counter */
#define ONU_TX_PREAMBLE_TYPE_02_P           (0x0000)                                    /* Preamble type 2 value */
#define ONU_TX_PREAMBLE_TYPE_02_PC          (0)                                         /* Preamble type 2 counter */
#define ONU_TX_PREAMBLE_TYPE_03_P           (0x00AA)                                    /* Preamble type 3 value */
#define ONU_TX_PREAMBLE_TYPE_03_DEF_PC      (ONU_TX_PREAMBLE_TYPE_03_DEF_US_622)        /* Preamble type 3 counter - default */
#define ONU_TX_PREAMBLE_TYPE_03_RT_PC       (ONU_TX_PREAMBLE_TYPE_03_RATE_622)          /* Preamble type 3 counter - runtime */

/* TX Delimiter Register */
#if 0
#define GPON_TX_DELIMITER                   (0x02BEB433)
#define GPON_TX_DELIMITER_D0                (0x0083)            /* Delimiter 0 */
#define GPON_TX_DELIMITER_D1                (0x00B4)            /* Delimiter 1 */
#define GPON_TX_DELIMITER_D2                (0x00BE)            /* Delimiter 2 */
#define GPON_TX_DELIMITER_DS                (0x0002)            /* Delimiter size */
#else
#define GPON_TX_DELIMITER                   (0x02AB5983)
#define GPON_TX_DELIMITER_D0                (0x0083)            /* Delimiter 0 */
#define GPON_TX_DELIMITER_D1                (0x0059)            /* Delimiter 1 */
#define GPON_TX_DELIMITER_D2                (0x00AB)            /* Delimiter 2 */
#define GPON_TX_DELIMITER_DS                (0x0003)            /* Delimiter size */
#endif

/* TX Final Delay Register */
#define GPON_TX_FINAL_DELAY_FD              (0x0020)            /* Final Delay */
#define GPON_TX_FINAL_DELAY_MASK            (0x0007)
#define GPON_TX_FINAL_DELAY_MAX             (0x003F)
#define GPON_TX_FINAL_DELAY_MIN             (0x0000)
#define GPON_TX_EQUAL_DELAY_MASK            (0x000FFFFF)
#define GPON_TX_EQUAL_DELAY_SHIFT           (3)

/* BURST enable parameter  */
#define GPON_BURST_EN_P_ACT_HIGH            (0x0000)            /* BURST enable parameter - polarity active high */
#define GPON_BURST_EN_P_ACT_LOW             (0x0001)            /* BURST enable parameter - polarity active low */

#define GPON_BURST_EN_MASK                  (0x00C0)
#define GPON_BURST_EN_P                     (GPON_BURST_EN_P_ACT_HIGH)

#ifndef PON_FPGA

#ifdef BURST_CFG_FOR_AVANTA_LP
#define GPON_BURST_EN_ORDER                 (1)
#else
#define GPON_BURST_EN_ORDER                 (0)
#endif /* BURST_CFG_FOR_AVANTA_LP */

#else
#define GPON_BURST_EN_ORDER                 (0)
#endif /* PON_FPGA */

#ifndef PON_FPGA

#ifdef BURST_CFG_FOR_AVANTA_LP
#define GPON_BURST_EN_START                 (0x0001)            /* BURST enable parameter - start */
#define GPON_BURST_EN_STOP                  (0x0000)            /* BURST enable parameter - stop */
#else
#define GPON_BURST_EN_START                 (0x0006)            /* BURST enable parameter - start */
#define GPON_BURST_EN_STOP                  (0x0006)            /* BURST enable parameter - stop */
#endif /* BURST_CFG_FOR_AVANTA_LP */

#else /* PON_FPGA */
#define GPON_BURST_EN_START                 (0x0006)            /* BURST enable parameter - start */
#define GPON_BURST_EN_STOP                  (0x0006)            /* BURST enable parameter - stop */
#endif /* PON_FPGA */

#define GPON_BURST_THRESHOLD                (0x4E20)            /* 20000 bytes */

#define MAC_LAST_UPSTREAM_PLOAM             (ONU_GPON_US_MSG_ACK)
#define MAC_LAST_DOWNSTREAM_PLOAM           (ONU_GPON_DS_MSG_EXT_BURST_LEN)

/* SW FIFO parameters */
#define GPON_SW_FIFO_SIZE                   (1024)              /* Size of SW FIFO pool for PLOAM messages */
#define GPON_PLOAM_MSG_WORDS                (3)                 /* PLOAM message length is 12 bytes or 3 words */
#define GPON_PLOAM_MSG_BYTES                (12)                /* PLOAM message length is 12 bytes or 3 words */
#define GPON_MAX_US_MSG_MOVING              (3)                 /* Max SW FIFO entries which can be moved from SW to HW FIFO by ISR */
#define GPON_MAX_US_MSG_TO_PROCESS          (10)                /* Max SW FIFO entries which can be processed by FIFO Audit */

/*AC coupling burst mode*/
#define GPON_TX_AC_COUPL_BUST_MODE_0        (0)
#define GPON_TX_AC_COUPL_BUST_MODE_1        (1)
#define GPON_TX_AC_COUPL_PREACT_BURST_TIME  (0x800)
#define GPON_TX_AC_COUPL_DATA_PATTERN_1     (0x00)
#define GPON_TX_AC_COUPL_DATA_PATTERN_2     (0x66)

/*UTM Active TX Bitmap*/
#define GPON_UTM_ACTIVE_TX_BITMAP           (0xFFFF)
#define GPON_UTM_ACTIVE_TX_BITMAP_VALID     (1)

/*GSE Transmit threshold*/
#define GPON_GST_TX_DATA_SHIFT              (12)
#define GPON_GST_TX_DATA_THRESHOLD          (0x30)
#define GPON_GST_TX_IDLE_THRESHOLD          (0x10)

#define GPON_TOD_INT_POL_ACTIVE_HIGH        (0)
#define GPON_TOD_INT_POL_ACTIVE_LOW         (1)
#define GPON_TOD_INT_MODE_BIP_INTERVAL      (0)
#define GPON_TOD_INT_MODE_SFC_INTERRUPT     (1)
#define GPON_TOD_INT_DEFAULT_WIDTH          (5)

/* Internal dyinggasp option */
#define DYINGASP_INTERNAL_ENABLE            (1)
#define DYINGASP_INTERNAL_DISABLE           (0)
#define DYINGASP_INTERNAL_ONDIE             (0)
#define DYINGASP_INTERNAL_ONBOARD           (1)
#define DYINGASP_INTERNAL_VOLTAGE_1V        (0x2)

/* Typedefs
   ------------------------------------------------------------------------------*/
typedef MV_STATUS (*MACTXPLOAMCTRFUNC)(MV_U8 msgId, MV_BOOL status);
typedef MV_STATUS (*MACRXPLOAMCTRFUNC)(MV_U8 msgId);

typedef struct {
	MV_U32 txErrMsgIdPloamCounter [ONU_GPON_US_MSG_LAST + 1];
	MV_U32 txMsgIdPloamCounter    [ONU_GPON_US_MSG_LAST + 1];
	MV_U32 txMsgTotalPloamCounter;
} S_MacTxPloamPm;

typedef struct {
	MV_U32 msg[GPON_PLOAM_MSG_WORDS];
} S_MacPloamMsg;

typedef struct {
	MV_U32 fifoWriteIndex;
	MV_U32 fifoReadIndex;
	MV_U32 fifoReparoundNum;
	S_MacPloamMsg fifoData[GPON_SW_FIFO_SIZE];
} S_MacFifoCtrl;

/* Macros
   ------------------------------------------------------------------------------*/
#define M_ONU_GPON_RANG_MSG_FINAL_DELAY(eqD) (eqD & GPON_TX_FINAL_DELAY_MASK)
#define M_ONU_GPON_RANG_MSG_EQUAL_DELAY(eqD) ((eqD >> GPON_TX_EQUAL_DELAY_SHIFT) & GPON_TX_EQUAL_DELAY_MASK)

#define M_ONU_GPON_BASE_ADDRESS(address) ((address != NULL) ? (address) : (0))

/* Global functions
   ------------------------------------------------------------------------------*/
/******************************************************************************/
/* ========================================================================== */
/*                         High-Level MAC Section                             */
/* ========================================================================== */
/******************************************************************************/
MV_STATUS mvOnuGponMacMessageReceive(MV_U8 *msgOnuId, MV_U8 *msgId, MV_U8 *msgData);
MV_STATUS mvOnuGponMacMessageSend(MV_U8 msgOnuId, MV_U8 msgId, MV_U8 *msgData);
MV_STATUS mvOnuGponMacMessageCleanSwFifo(void);
MV_STATUS mvOnuGponMacPreambleSet(MV_U32 preamType1Val, MV_U32 preamType1Cnt,
				  MV_U32 preamType2Val, MV_U32 preamType2Cnt,
				  MV_U32 preamType3Val, MV_U32 preamType3Cnt);
MV_STATUS mvOnuGponMacPreambleType3Set(MV_U32 preamType3Val, MV_U32 preamType3Cnt);

/* Upstream Messages send routines */
MV_STATUS mvOnuGponMacPasswordMessageSend(MV_U8 onuId, MV_U8 *password, MV_U32 counter);
MV_STATUS mvOnuGponMacReiMessageSend(MV_U8 onuId, MV_U32 errorsCounter);
MV_STATUS mvOnuGponMacAcknowledgeMessageSend(MV_U8 onuId, MV_U8 dmId, MV_U8 *dmByte);
MV_STATUS mvOnuGponMacEncryptionKeyMessageSend(MV_U8 onuId, MV_U8 KeyIndex, MV_U8 fragIndex, MV_U8 *key);
MV_STATUS mvOnuGponMacDgMessageSend(MV_U8 onuId);

/******************************************************************************/
/* ========================================================================== */
/*                         Low-Level MAC Section                              */
/* ========================================================================== */
/******************************************************************************/

/* ========================================================================== */
/*                        Rx Functions Section                                */
/* ========================================================================== */
MV_STATUS mvOnuGponMacRxConfigSet(MV_BOOL enable);
MV_STATUS mvOnuGponMacRxConfigBitOrderSet(MV_U32 value);
MV_STATUS mvOnuGponMacRxPsaConfigSet(MV_U32 syncFsmM1, MV_U32 syncFsmM2, MV_U32 syncFsmM3, MV_U32 fecHyst);
MV_STATUS mvOnuGponMacRxFecHysteresisSet(MV_U32 fecHyst);
MV_STATUS mvOnuGponMacRxFecConfigSet(MV_BOOL swIndication, MV_BOOL forceSw, MV_BOOL ignoreParity);
MV_STATUS mvOnuGponMacRxFecStatusGet(MV_U32 *fecStatus);
MV_STATUS mvOnuGponMacRxPloamDataGet(MV_U32  *ploamData);
MV_STATUS mvOnuGponMacRxPloamDataUsedGet(MV_U32 *ploamFifoSize);
MV_STATUS mvOnuGponMacRxPlenConfigSet(MV_BOOL forceNum, MV_BOOL useForce, MV_BOOL ignDiff, MV_BOOL notFix, MV_BOOL ignCrc);
MV_STATUS mvOnuGponMacRxPloamConfigSet(MV_BOOL ignCrc, MV_BOOL ignBrd, MV_BOOL fwdAll);
MV_STATUS mvOnuGponMacRxBwMapConfigSet(MV_U32 minSaDelta, MV_BOOL notCrcFix, MV_BOOL ignCrc);
MV_STATUS mvOnuGponMacRxBwMapSet(MV_U32 entry, MV_U32 allocId, MV_U32 tcontCnt, MV_BOOL enable);
MV_STATUS mvOnuGponMacRxBwMapGet(MV_U32 entry, MV_U32 *allocId, MV_BOOL *enable, MV_U32 *tcontCnt);
MV_STATUS mvOnuGponMacRxEqualizationDelaySet(MV_U32 delay);
MV_STATUS mvOnuGponMacRxEqualizationDelayGet(MV_U32 *delay);
MV_STATUS mvOnuGponMacRxInternalDelaySet(MV_U32 delay);
MV_STATUS mvOnuGponMacRxInternalDelayGet(MV_U32 *delay);
MV_STATUS mvOnuGponMacRxSuperFrameCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacRxSuperFrameCounterSet(MV_U32 counter);
/* ========================================================================== */
/*                        Tx Functions Section                                */
/* ========================================================================== */
MV_STATUS mvOnuGponMacTxDelimiterSet(MV_U32 delimiterPattern, MV_U32 delimiterSize);
MV_STATUS mvOnuGponMacTxDelimiterGet(MV_U32 *pattern, MV_U32 *size);
MV_STATUS mvOnuGponMacTxPloamDataFifoSet(MV_U32 txPloamData);
MV_STATUS mvOnuGponMacTxPloamDataFreeGet(MV_U32 *ploamFifoSize);
MV_STATUS mvOnuGponMacTxBurstEnParamSet(MV_U32 mask, MV_U32 polarity, MV_U32 delay, MV_U32 enStop, MV_U32 enStart);
MV_STATUS mvOnuGponMacTxBurstEnPolaritySet(MV_U32 polarity);
MV_STATUS mvOnuGponMacTxBurstEnParamGet(MV_U32 *mask, MV_U32 *polarity, MV_U32 *order, MV_U32 *enStop, MV_U32 *enStart);
MV_STATUS mvOnuGponMacTxFinalDelaySet(MV_U32 txFinalDelay);
MV_STATUS mvOnuGponMacTxFinalDelayGet(MV_U32 *txFinalDelay);
MV_STATUS mvOnuGponMacTxPreambleSet(MV_U32 type, MV_U32 pattern, MV_U32 size);
MV_STATUS mvOnuGponMacTxPlsSet(MV_U32 pattern);
MV_STATUS mvOnuGponMacTxConstIdleMsgSet(MV_U32 onuId);
MV_STATUS mvOnuGponMacTxConstSerialNumberMsgSet(MV_U32 onuId, MV_U8 *serialNumber, MV_U32 randomDelay);
MV_STATUS mvOnuGponMacTxDbrBlockSizeSet(MV_U32 blockSize);
MV_STATUS mvOnuGponMacTxSwCountersAdd(MV_U8 msgId, MV_BOOL sucsses);
MV_STATUS mvOnuGponMacTxSwCountersGet(void *txPloamPm);
MV_STATUS mvOnuGponHighPriTxEnableSet(MV_U32 txEnable);
MV_STATUS mvOnuGponHighPriTxValidBmSet(MV_U32 validBm);
MV_STATUS mvOnuGponHighPriTxValidBmGet(MV_U32 *validBm);
MV_STATUS mvOnuGponHighPriTxMap0Set(MV_U32 map0);
MV_STATUS mvOnuGponHighPriTxMap0Get(MV_U32 *map0);
MV_STATUS mvOnuGponHighPriTxMap1Set(MV_U32 map1);
MV_STATUS mvOnuGponHighPriTxMap1Get(MV_U32 *map1);
MV_STATUS mvOnuGponQueueOverheadSet(MV_U32 tcontNum, MV_U32 queue, MV_U32 overhead);
/* ========================================================================== */
/*                        General Functions Section                           */
/* ========================================================================== */
MV_STATUS mvOnuGponMacVersionGet(MV_U32 *version);
MV_STATUS mvOnuGponMacOnuIdSet(MV_U32 onuId, MV_BOOL valid);
MV_STATUS mvOnuGponMacOnuStateSet(MV_U32 onuState);
MV_STATUS mvOnuGponMacSerialNumberSet(MV_U32 serialNum);
MV_STATUS mvOnuGponTimeOfDayIntWidthSet(MV_U32 width, MV_U8 mode, MV_U8 polarity);
MV_STATUS mvOnuGponTimeOfDayIntWidthGet(MV_U32 *width, MV_U8 *mode, MV_U8 *polarity);
MV_STATUS mvOnuGponSuperFrameCouterSet(MV_U32 sfc);
MV_STATUS mvOnuGponSuperFrameCouterGet(MV_U32 *sfc);
/* ========================================================================== */
/*                        Gem Functions Section                               */
/* ========================================================================== */
MV_STATUS mvOnuGponMacGemPayloadLenSet(MV_U32 payloadlen);
MV_STATUS mvOnuGponMacGemPayloadLenGet(MV_U32 *payloadlen);
MV_STATUS mvOnuGponMacGemEthFrameLenSet(MV_U32 maxFrameLen, MV_U32 minFrameLen);
MV_STATUS mvOnuGponMacGemEthFrameLenGet(MV_U32 *maxFrameLen, MV_U32 *minFrameLen);
MV_STATUS mvOnuGponMacGemOmciFrameLenSet(MV_U32 maxFrameLen, MV_U32 minFrameLen);
MV_STATUS mvOnuGponMacGemOmciFrameLenGet(MV_U32 *maxFrameLen, MV_U32 *minFrameLen);
MV_STATUS mvOnuGponMacGemOmciPortIdSet(MV_U32 portId, MV_U32 valid);
MV_STATUS mvOnuGponMacGemOmciPortIdGet(MV_U32 *portId, MV_U32 *valid);
MV_STATUS mvOnuGponMacGemOmciFastPathSet(MV_U32 valid);
MV_STATUS mvOnuGponMacGemOmciFastPathGet(MV_U32 *valid);
MV_STATUS mvOnuGponMacRxCtrlCmdGet(MV_U32 *ctrlCmd);
MV_STATUS mvOnuGponMacRxCtrlDataUsedGet(MV_U32 *dataFifoSize);
MV_STATUS mvOnuGponMacRxCtrlCmdUsedGet(MV_U32 *ctrlFifoSize);
MV_STATUS mvOnuGponMacRxCtrlHeadLenGet(MV_U32 *headLen);
/* ========================================================================== */
/*                        UTM Functions Section                               */
/* ========================================================================== */
MV_STATUS mvOnuGponMacUtmOmciPortIdSet(MV_U32 portId, MV_U32 valid);
MV_STATUS mvOnuGponMacUtmOmciPortIdGet(MV_U32 *portId, MV_U32 *valid);
MV_STATUS mvOnuGponMacUtmTcPeriodSet(MV_U32 period);
MV_STATUS mvOnuGponMacUtmTcValidSet(MV_U32 valid);
MV_STATUS mvOnuGponMacUtmTcConfigGet(MV_U32 *period, MV_U32 *valid);
MV_STATUS mvOnuGponMacUtmActiveTxBitmapSet(MV_U32 bitmap);
MV_STATUS mvOnuGponMacUtmActiveTxBitmapValidSet(MV_U32 valid);
MV_STATUS mvOnuGponMacUtmActiveTxBitmapConfigGet(MV_U32 *bitmap, MV_U32 *valid);
MV_STATUS mvOnuGponMacUtmFlushSet(MV_U32 tcontNum, MV_U32 value);
MV_STATUS mvOnuGponMacUtmFlushGet(MV_U32 tcontNum, MV_U32 *value);
MV_STATUS mvOnuGponMacUtmGeneralSet(MV_U32 latencyMode, MV_U32 latencyThresh, MV_U32 latencyIpg);
MV_STATUS mvOnuGponMacUtmGeneralGet(MV_U32 *latencyMode, MV_U32 *latencyThresh, MV_U32 *latencyIpg);
MV_STATUS mvOnuGponMacUtmDebugGet(MV_U32 tcontNum, MV_U32 *state);

/* ========================================================================== */
/*                        Interrupt Functions Section                         */
/* ========================================================================== */
MV_STATUS mvOnuGponMacPonInterruptGet(MV_U32 *interrupt);
MV_STATUS mvOnuGponMacPonInterruptMaskGet(MV_U32 *mask);
MV_STATUS mvOnuGponMacPonInterruptMaskSet(MV_U32 mask);
MV_STATUS mvOnuGponMacBipStatusValueGet(MV_U32 *value);
MV_STATUS mvOnuGponMacBipInterruptIntervalSet(MV_U32 interval);
MV_STATUS mvOnuGponMacBipInterruptStatusValueGet(MV_U32 *value);
MV_STATUS mvOnuGponMacRamTestCfgSet(MV_U32 config);
MV_STATUS mvOnuGponMacRamTestCfgGet(MV_U32 *config);
/* ========================================================================== */
/*                        Statistics Functions Section                        */
/* ========================================================================== */
/* Gem */
/* === */
MV_STATUS mvOnuGponMacGemRcvdIdleGemFramesCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdValidGemFramesCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdUndefGemFramesCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdOmciFramesCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemDropGemFramesCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemDropOmciFramesCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdGemFramesWithUncorrHecErrCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdGemFramesWithOneFixedHecErrCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdGemFramesWithTwoFixedHecErrCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdValidGemFramesTotalByteCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdUndefGemFramesTotalByteCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemReassembleMemoryFlushCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemSynchLostCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdEthFramesWithCorrFcsCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdEthFramesWithFcsErrCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdOmciFramesWithCorrCrcCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacGemRcvdOmciFramesWithCrcErrCounterGet(MV_U32 *counter);
/* Rx Ploam */
/* ======== */
MV_STATUS mvOnuGponMacRxPloamLostCrcCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacRxPloamLostFullCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacRxPloamRcvdIdleCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacRxPloamRcvdBroadCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacRxPloamRcvdOnuIdCounterGet(MV_U32 *counter);
/* Tx */
/* == */
MV_STATUS mvOnuGponMacTxGemPtiTypeOneFrameCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacTxGemPtiTypeZeroFrameCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacTxIdleGemFramesCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacTxTxEnableCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacTxTxEnableCounterThresholdGet(MV_U32 *threshold);
MV_STATUS mvOnuGponMacTxTxEnableCounterThresholdSet(MV_U32 threshold);
MV_STATUS mvOnuGponMacTxGseTransCounterThresholdGet(MV_U32 *threshold);
MV_STATUS mvOnuGponMacTxGseTransCounterThresholdSet(MV_U32 threshold);
MV_STATUS mvOnuGponMacTxConfigAcCouplingGet(MV_U32 *busrtMode, MV_U32 *preActiveBurstTime,
					    MV_U8 *dataPattern1, MV_U8 *dataPattern2);
MV_STATUS mvOnuGponMacTxConfigAcCouplingSet(MV_U32 busrtMode, MV_U32 preActiveBurstTime,
					    MV_U8 dataPattern1, MV_U8 dataPattern2);
MV_STATUS mvOnuGponMacTxRandomDelayUnitsGet(MV_U32 *units);
MV_STATUS mvOnuGponMacTxRandomDelayUnitsSet(MV_U32 units);
MV_STATUS mvOnuGponMacTxEthFramesTcontiCounterGet(MV_U32 tcontNum, MV_U32 *counter);
MV_STATUS mvOnuGponMacTxEthBytesTcontiCounterGet(MV_U32 tcontNum, MV_U32 *counter);
MV_STATUS mvOnuGponMacTxGemFramesTcontiCounterGet(MV_U32 tcontNum, MV_U32 *counter);
MV_STATUS mvOnuGponMacTxIdleGemFramesTcontiCounterGet(MV_U32 tcontNum, MV_U32 *counter);
/* FEC */
/* === */
MV_STATUS mvOnuGponMacRxFecBytesCounterGet(MV_U32 *counter);
MV_STATUS mvOnuGponMacRxFecCorrectCounterGet(MV_U32 *bytes, MV_U32 *bits);
MV_STATUS mvOnuGponMacRxFecInorrectCounterGet(MV_U32 *bytes, MV_U32 *bits);
/* Bw Map */
/* ====== */
MV_STATUS mvOnuGponMacRxBwMapAllocatrionsCounterGet(MV_U32 *cor, MV_U32 *incor, MV_U32 *crcCor, MV_U32 *errCrc);
MV_STATUS mvOnuGponMacRxBwMapTotalBwGet(MV_U32 *bw);
/* Plend */
/* ===== */
MV_STATUS mvOnuGponMacInComingPlendCntGet(MV_U32 *cntValue);

/* ========================================================================== */
/*                        AES                                                 */
/* ========================================================================== */
MV_STATUS mvOnuGponMacAesInit(void);
MV_STATUS mvOnuGponMacAesEnableSet(MV_BOOL enable);
MV_STATUS mvOnuGponMacAesPortIdSet(MV_U32 portId, MV_BOOL status);
MV_BOOL   mvOnuGponMacAesPortIdGet(MV_U32 portId);
MV_STATUS mvOnuGponMacAesKeyShadowWrite(MV_U8 *key);

/* ========================================================================== */
/*                        GEM Port                                            */
/* ========================================================================== */
MV_STATUS mvOnuGponMacIpgValueSet(MV_U32 ipgVal);
MV_STATUS mvOnuGponMacIpgValueGet(MV_U32 *ipgVal);
MV_STATUS mvOnuGponMacIpgValidSet(MV_BOOL enable);
MV_STATUS mvOnuGponMacIpgValidGet(MV_BOOL *enable);
/* ========================================================================== */
MV_STATUS mvOnuGponMacGemInit(void);
MV_STATUS mvOnuGponMacPortIdValidSet(MV_U32 portId, MV_BOOL status);
MV_BOOL   mvOnuGponMacPortIdValidGet(MV_U32 portId);
MV_STATUS mvOnuEponMacInternalDyingGaspSet(MV_U32 enable, MV_U32 onDie, MV_U32 voltage);
MV_STATUS mvOnuGponMacXvrReset(MV_U32 xvrType);
MV_STATUS mvOnuGponMacXvrActivate(void);
MV_STATUS mvOnuGponMacAsicVersionGet(MV_U32 *asicVer);
MV_STATUS mvOnuGponMacMicroSecCounterGet(MV_U32 *microSec);
MV_STATUS mvOnuGponMacDyingGaspConfigSet(MV_U32 a_id, MV_U32 a_disableEnable, MV_U32 a_polarity, MV_U32 a_threshold);

/* ========================================================================== */
/*                        SW FIFO                                             */
/* ========================================================================== */
void      mvOnuGponMacFifoInit(void);
void      mvOnuGponFifoAuditTimerExpireHndl(void);
void      mvOnuGponMacFifoGetInfo(S_MacFifoCtrl **ptr, MV_32 *supported);
void      mvOnuGponMacFifoGetStat(MV_32 *callCnt, MV_32 *addSwCnt, MV_32 *freeSwCnt, MV_32 *sendHwCnt);
void      mvOnuGponMacFifoAuditGetStat(MV_32 *enterCnt, MV_32 *actionCnt);
void      mvOnuGponMacFifoCountersSet(MV_U32 value);
void      mvOnuGponMacFifoSupportSet(MV_32 value);

/******************************************************************************/
/******************************************************************************/
/* ========================================================================== */
/* ========================================================================== */
/* ==                                                                      == */
/* ==           =========   =========   =========   ==        ==           == */
/* ==           =========   =========   =========   ===       ==           == */
/* ==           ==          ==     ==   ==     ==   == ==     ==           == */
/* ==           ==          ==     ==   ==     ==   ==  ==    ==           == */
/* ==           =========   =========   ==     ==   ==   ==   ==           == */
/* ==           =========   =========   ==     ==   ==    ==  ==           == */
/* ==           ==          ==          ==     ==   ==     == ==           == */
/* ==           ==          ==          ==     ==   ==      ====           == */
/* ==           =========   ==          =========   ==       ===           == */
/* ==           =========   ==          =========   ==        ==           == */
/* ==                                                                      == */
/* ========================================================================== */
/* ========================================================================== */
/******************************************************************************/
/******************************************************************************/

/* Definitions
   ------------------------------------------------------------------------------*/
#define ADDITIONAL_OPCODE_1                 (1)
#define ADDITIONAL_OPCODE_2                 (2)
#define ADDITIONAL_OPCODE_3                 (3)

/*RPM Module configuration*/
#define EPON_RPM_PACKET_GEN_INDICATE_DATA   (0)
#define EPON_RPM_PACKET_GEN_INDICATE_HEAD   (1)
#define EPON_RPM_BITMAP_ORDER_HW            (0)
#define EPON_RPM_BITMAP_ORDER_SW            (0)
#define EPON_RPM_QSET_ORDER_HW              (1)
#define EPON_RPM_QSET_ORDER_SW              (1)
#define EPON_RPM_REPORT_MODE_HW             (2)
#define EPON_RPM_REPORT_MODE_SW             (0)

/*TOD INT*/
#define EPON_TOD_INT_POL_ACTIVE_HIGH        (0)
#define EPON_TOD_INT_POL_ACTIVE_LOW         (1)
#define EPON_TOD_INT_DEFAULT_WIDTH          (5)

/*TX MUX POWER UP*/
#define EPON_TXM_POWERUP_DEFAULT_TIME       (0x800)

/*EPON TX MUX POWER UP*/
#define EPON_DDM_TX_EN_MASK_DEFAULT         (2)
#define EPON_DDM_TX_TX_POL_DEFAULT          (0)
#define EPON_DDM_TX_SERDES_POL_DEFAULT      (0)
#define EPON_DDM_TX_XVR_POL_DEFAULT         (0)
#define EPON_DDM_TX_BURST_ENA_DEFAULT       (0)

/* PCS RX */
#define EPON_PCS_CONFIG_RX_ENABLE            (1)
#define EPON_PCS_CONFIG_RX_DISABLE           (0)

/* EPON 2K packet supported */
#define EPON_MAC_RXP_PACKET_SIZE_LIMIT_2K_SUPP        (0x800)
#define EPON_MAC_RXP_DATA_FIFO_THRESHOLD_2K_SUPP      (0x800)
#define EPON_MAC_PCS_FRAME_SIZE_LIMIT_SIZE_2K_SUPP    (0x800)
#define EPON_MAC_PCS_FRAME_SIZE_LIMIT_LATENCY_2K_SUPP (0x1324)

/* Default value of these registers */
#define EPON_MAC_RXP_PACKET_SIZE_LIMIT_MAX_DEF    (0x640)
#define EPON_MAC_RXP_PACKET_SIZE_LIMIT_MIN_DEF    (0x40)
#define EPON_MAC_RXP_DATA_FIFO_THRESHOLD_DEF      (0x780)
#define EPON_MAC_PCS_FRAME_SIZE_LIMIT_SIZE_DEF    (0x640)
#define EPON_MAC_PCS_FRAME_SIZE_LIMIT_LATENCY_DEF (0xED8)

/* UTM TX Bitmap */
#define EPON_UTM_TX_BITMAP_DEFAULT          (0xFF)
#define EPON_UTM_TX_BITMAP_VALID_DEFAULT    (1)

/* PPv2 EPON DBA  */
#define PPV2_DBA_POST_STATE_ENABLE          (1)
#define PPV2_DBA_POST_STATE_DISABLE         (0)
#define PPV2_DBA_POST_FEC_OVERHEAD          (14)

#define PPV2_PON_PORT_INDEX                             (3)
#define PPV2_PON_FEC_TX_DESC_SET                        (1)
#define PPV2_PON_FEC_TX_DESC_CLR                        (0)

/* EPON RX timestamp  */
#define EPON_RX_TIMESTAMP_SELECT            (1)
#define EPON_RX_TIMESTAMP_SIGN_SUB          (0)
#define EPON_RX_TIMESTAMP_OFFSET            (0x22C) /* The value after testing */

/* Global functions
   ------------------------------------------------------------------------------*/

/* ========================================================================== */
/*                        Interrupt Functions Section                         */
/* ========================================================================== */
MV_STATUS mvOnuEponMacPonInterruptGet(MV_U32 *interrupt);
MV_STATUS mvOnuEponMacPonInterruptMaskGet(MV_U32 *mask);
MV_STATUS mvOnuEponMacPonInterruptMaskSet(MV_U32 mask);
MV_STATUS mvOnuEponMacPonDbrLlidInterruptGet(MV_U32 llid, MV_U32 *interrupt);
MV_STATUS mvOnuEponMacPonDbrInterruptGet(MV_U32 *interrupt);
MV_STATUS mvOnuEponMacPonDbrInterruptMaskGet(MV_U32 *mask);
MV_STATUS mvOnuEponMacPonDbrInterruptMaskSet(MV_U32 mask);

/* ========================================================================== */
/*                        General Functions Section                           */
/* ========================================================================== */
MV_STATUS mvOnuEponMacVersionGet(MV_U32 *version);
MV_STATUS mvOnuEponMacPcsRxEnableSet(MV_U32 rxEnable);
MV_STATUS mvOnuEponMacPcsTxEnableSet(MV_U32 txEnable);
MV_STATUS mvOnuEponMacOnuEnableSet(MV_U32 rxEnable, MV_U32 txEnable);
MV_STATUS mvOnuEponMacOnuRxEnableSet(MV_U32 rxEnable);
MV_STATUS mvOnuEponMacOnuTxEnableSet(MV_U32 txEnable, MV_U32 macId);
MV_STATUS mvOnuEponMacOnuStateSet(MV_U32 onuState, MV_U32 macId);
MV_STATUS mvOnuEponMacOnuStateGet(MV_U32 *onuState, MV_U32 macId);
MV_STATUS mvOnuEponMacOnuRegAutoUpdateStateSet(MV_U32 value);
MV_STATUS mvOnuEponMacOnuReRegAutoUpdateStateSet(MV_U32 value);
MV_STATUS mvOnuEponMacOnuDeRegAutoUpdateStateSet(MV_U32 value);
MV_STATUS mvOnuEponMacOnuNackAutoUpdateStateSet(MV_U32 value);
MV_STATUS mvOnuEponMacGenTqSizeSet(MV_U32 tqRxSize, MV_U32 tqTxSize);
MV_STATUS mvOnuEponMacGenTqSizeGet(MV_U32 *tqRxSize, MV_U32 *tqTxSize);
MV_STATUS mvOnuEponMacGenLaserParamSet(MV_U32 onOffTime, MV_U32 onTime, MV_U32 offTime);
MV_STATUS mvOnuEponMacGenLaserParamGet(MV_U32 *onOffTime, MV_U32 *onTime, MV_U32 *offTime);
MV_STATUS mvOnuEponMacGenTailGuardSet(MV_U32 packetTailGuard, MV_U32 tailGuard);
MV_STATUS mvOnuEponMacGenSyncTimeSet(MV_U32 syncTime, MV_U32 addSyncTime, MV_U32 forceSwUpdate,
				     MV_U32 disGateAutoUpdate, MV_U32 disDiscoveryAutoUpdate);
MV_STATUS mvOnuEponMacGenSyncTimeGet(MV_U32 *syncTime, MV_U32 *addSyncTime, MV_U32 *forceSwUpdate,
				     MV_U32 *disGateAutoUpdate, MV_U32 *disDiscoveryAutoUpdate);
MV_STATUS mvOnuEponMacGenBcastAddrSet(MV_U32 bcastMacAddrHigh, MV_U32 bcastMacAddrLow);
MV_STATUS mvOnuEponMacGenUcastAddrSet(MV_U32 bcastMacAddrHigh, MV_U32 bcastMacAddrLow, MV_U32 macId);
MV_STATUS mvOnuEponMacGenOnuConfigSet(MV_U32 rxPcsFecEn, MV_U32 txPcsFecEn, MV_U32 reportAutoResponse,
				      MV_U32 ackAutoResponse, MV_U32 requestAutoResponse, MV_U32 txFecEn);
MV_STATUS mvOnuEponMacGenOnuConfigAutoReportSet(MV_U32 reportAutoResponse);
MV_STATUS mvOnuEponMacGenOnuConfigAutoAckSet(MV_U32 ackAutoResponse);
MV_STATUS mvOnuEponMacGenOnuConfigAutoRequestSet(MV_U32 requestAutoResponse);
MV_STATUS mvOnuEponMacGenSldSet(MV_U32 sld);
MV_STATUS mvOnuEponMacGenCtrlTypeSet(MV_U32 type);
MV_STATUS mvOnuEponMacGenLocalTimeStampSet(MV_U32 timeStamp);
MV_STATUS mvOnuEponMacGenLocalTimeStampGet(MV_U32 *timeStamp);
MV_STATUS mvOnuEponMacGenPendGrantSet(MV_U32 grant);
MV_STATUS mvOnuEponMacGenSglStatusGet(MV_U32 *sglStatus);
MV_STATUS mvOnuEponMacGenPonPhyDbgSet(MV_U32 ponDebug);
MV_STATUS mvOnuEponMacGenPonPhyDbgGet(MV_U32 *ponDebug);
MV_STATUS mvOnuEponMacGenSglSwReset(MV_U32 swReset);
MV_STATUS mvOnuEponMacGenSglConfig(MV_U32 startThreshold, MV_U32 dataAlignment);
MV_STATUS mvOnuEponMacGenSglDebounceConfig(MV_U32 polarity, MV_U32 threshold);
MV_STATUS mvOnuEponMacGenSglDebounceEnable(MV_U32 enable);
MV_STATUS mvOnuEponMacGenChurningKeySet(MV_U32 key);
MV_STATUS mvOnuEponMacGenOpcodeSet(MV_U32 opcodeType, MV_U32 value, MV_U32 index,
				   MV_U32 timestamp, MV_U32 valid);
MV_STATUS mvOnuEponMacGenTimestampConfig(MV_U32 gpmAddSyncTimeToTimestamp, MV_U32 gpmAimestampOffset,
					 MV_U32 txmAddSyncTimeToTimestamp, MV_U32 txmUseTimestampImage,
					 MV_U32 txmTimestampOffset);
MV_STATUS mvOnuEponMacGenRxTimestampConfigSet(MV_U32 rxpTimestampSelect, MV_U32 rxpTimestampSign,
					      MV_U32 rxpTimestampOffset);
MV_STATUS mvOnuEponMacGenRxTimestampConfigGet(MV_U32 *rxpTimestampSelect, MV_U32 *rxpTimestampSign,
					      MV_U32 *rxpTimestampOffset);
MV_STATUS mvOnuEponMacGenRegPacketPaddingSet(MV_U32 regAckPadding, MV_U32 regReqPadding);
MV_STATUS mvOnuEponMacGenRegPacketPaddingGet(MV_U32 *regAckPadding, MV_U32 *regReqPadding);
MV_STATUS mvOnuEponMacGenDriftThresholdSet(MV_U32 threshold);
MV_STATUS mvOnuEponMacGenTimeStampForIntrSet(MV_U32 timestamp);
MV_STATUS mvOnuEponMacGenTimeStampForIntrGet(MV_U32 *timestamp);
MV_STATUS mvOnuEponMacGenUtmTcPeriodSet(MV_U32 tcPeriod);
MV_STATUS mvOnuEponMacGenUtmTcPeriodEnSet(MV_U32 tcPeriodEnable);
MV_STATUS mvOnuEponMacGenGemIpgValueSet(MV_U32 ipgVal);
MV_STATUS mvOnuEponMacGenGemIpgValueEnSet(MV_U32 ipgValEn);
MV_STATUS mvOnuEponMacGenUtmActTxBitmapSet(MV_U32 txBitmap);
MV_STATUS mvOnuEponMacGenUtmActTxBitmapEnSet(MV_U32 txBitmapEnable);
MV_STATUS mvOnuEponMacGenTimeOfDayIntConfigSet(MV_U32 polarity, MV_U32 width);
MV_STATUS mvOnuEponMacGenTimeOfDayIntConfigGet(MV_U32 *polarity, MV_U32 *width);
MV_STATUS mvOnuEponMacGenUtmExtReportEnSet(MV_U32 extReportEnable);
MV_STATUS mvOnuEponMacGenUtmExtReportBitmapEnSet(MV_U32 llid, MV_U32 reportBitmapEnable);
MV_STATUS mvOnuEponMacGenUtmExtReportMapSet(MV_U32 host_tx, MV_U32 target_tx);

/* ========================================================================== */
/*                        RXP Functions Section                               */
/* ========================================================================== */
MV_STATUS mvOnuEponMacRxpPacketSizeLimitSet(MV_U32 minSize, MV_U32 maxSize);
MV_STATUS mvOnuEponMacRxpPacketSizeLimitGet(MV_U32 *minSize, MV_U32 *maxSize);
MV_STATUS mvOnuEponMacRxpPacketFilterSet(MV_U32 ignoreLlidCrcError, MV_U32 ignoreFcsError,
					 MV_U32 ignoreGmiiError, MV_U32 ignoreLengthError,
					 MV_U32 forwardAllLlid, MV_U32 forwardBc0FFF,
					 MV_U32 forwardBc1FFF, MV_U32 forwardBc1xxx, MV_U32 dropBc1nnn);
MV_STATUS mvOnuEponMacRxpPacketFilterGet(MV_U32 *ignoreLlidCrcError, MV_U32 *ignoreFcsError,
					 MV_U32 *ignoreGmiiError, MV_U32 *ignoreLengthError,
					 MV_U32 *forwardAllLlid, MV_U32 *forwardBc0FFF,
					 MV_U32 *forwardBc1FFF, MV_U32 *forwardBc1xxx, MV_U32 *dropBc1nnn);
#ifdef PON_A0
MV_STATUS mvOnuEponMacRxpPacketForwardSet(MV_U32 ctrlFrameToDataQueue,
					  MV_U32 ctrlFrameToCtrlQueue,
					  MV_U32 rprtFrameToDataQueue,
					  MV_U32 rprtFrameToRprtQueue,
					  MV_U32 slowFrameToRprtQueue,
					  MV_U32 slowFrameToCtrlQueue,
					  MV_U32 rxpTsUpdateFcsError,
					  MV_U32 rxpTsUpdateGmiiError,
					  MV_U32 rxpTsUpdateLengthError,
					  MV_U32 rxpTsUpdateCrcError);
MV_STATUS mvOnuEponMacRxpPacketForwardGet(MV_U32 *ctrlFrameToDataQueue,
					  MV_U32 *ctrlFrameToCtrlQueue,
					  MV_U32 *rprtFrameToDataQueue,
					  MV_U32 *rprtFrameToRprtQueue,
					  MV_U32 *slowFrameToRprtQueue,
					  MV_U32 *slowFrameToCtrlQueue,
					  MV_U32 *rxpTsUpdateFcsError,
					  MV_U32 *rxpTsUpdateGmiiError,
					  MV_U32 *rxpTsUpdateLengthError,
					  MV_U32 *rxpTsUpdateCrcError);
#else
MV_STATUS mvOnuEponMacRxpPacketForwardSet(MV_U32 ctrlFrameToDataQueue, MV_U32 ctrlFrameToCtrlQueue,
					  MV_U32 rprtFrameToDataQueue, MV_U32 rprtFrameToRprtQueue,
					  MV_U32 slowFrameToRprtQueue, MV_U32 slowFrameToCtrlQueue);
MV_STATUS mvOnuEponMacRxpPacketForwardGet(MV_U32 *ctrlFrameToDataQueue, MV_U32 *ctrlFrameToCtrlQueue,
					  MV_U32 *rprtFrameToDataQueue, MV_U32 *rprtFrameToRprtQueue,
					  MV_U32 *slowFrameToRprtQueue, MV_U32 *slowFrameToCtrlQueue);
#endif
MV_STATUS mvOnuEponMacRxpLlidDataSet(MV_U32 llid, MV_U32 index);
MV_STATUS mvOnuEponMacRxpLlidDataGet(MV_U32 *llid, MV_U32 index);
MV_STATUS mvOnuEponMacRxpEncConfigSet(MV_U32 config);
MV_STATUS mvOnuEponMacRxpEncConfigGet(MV_U32 *config);
MV_STATUS mvOnuEponMacRxpEncKeySet(MV_U32 key, MV_U32 index, MV_U32 macId);
MV_STATUS mvOnuEponMacRxpEncKeyGet(MV_U32 *keyIndex0, MV_U32 *keyIndex1, MV_U32 macId);
MV_STATUS mvOnuEponMacRxpDataFifoThresholdSet(MV_U32 threshold);
MV_STATUS mvOnuEponMacRxpDataFifoThresholdGet(MV_U32 *threshold);

/* ========================================================================== */
/*                        GPM Functions Section                               */
/* ========================================================================== */
MV_STATUS mvOnuEponMacGpmMacFutureGrantTimeSet(MV_U32 grant);
MV_STATUS mvOnuEponMacGpmMinProcessingTimeSet(MV_U32 processingTime, MV_U32 effectiveLength);
MV_STATUS mvOnuEponMacGpmDiscoveryGrantLengthSet(MV_U32 grantLength, MV_U32 addOffsetForCalc, MV_U32 grantLengthMultiTq);
MV_STATUS mvOnuEponMacGpmRxSyncGet(MV_U32 *syncTime, MV_U32 *syncTimeUpdateInd);
MV_STATUS mvOnuEponMacGpmGrantValidCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmGrantMaxFutureTimeErrorCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmMinProcTimeErrorCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmLengthErrorCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmDiscoveryAndRegisterCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmFifoFullErrorCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmOpcDiscoveryNotRegisterBcastCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmOpcRegisterNotDiscoveryCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmOpcDiscoveryNotRegisterNotBcastCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmOpcDropGrantCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmOpcHiddenGrantCounterGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacGpmOpcBackToBackCounterGet(MV_U32 *counter, MV_U32 macId);

/* ========================================================================== */
/*                        TXM Functions Section                               */
/* ========================================================================== */
MV_STATUS mvOnuEponMacTxmOverheadSet(MV_U32 overhead);
MV_STATUS mvOnuEponMacTxmOverheadGet(MV_U32 *overhead);
MV_STATUS mvOnuEponMacTxmConfigSet(MV_U32 tFec1Cfg, MV_U32 tFec2Cfg, MV_U32 ctrlFifoDisFcs,
				   MV_U32 maskGmiiSignal, MV_U32 blockDataFromDataPath,
				   MV_U32 addIpgToLenCalc, MV_U32 mode, MV_U32 alignment,
				   MV_U32 priority);
MV_STATUS mvOnuEponMacTxmPowerUpIndSet(MV_U32 time);
MV_STATUS mvOnuEponMacTxmPowerUpIndGet(MV_U32 *time);
MV_STATUS mvOnuEponMacTxmLlidSet(MV_U32 llid, MV_U32 macId);
MV_STATUS mvOnuEponMacTxmCppReportConfig(MV_U32 queueReportQ2, MV_U32 reportBitmapQ2,
					 MV_U32 queueReport, MV_U32 queueSet,
					 MV_U32 reportBitmap, MV_U32 macId);
MV_STATUS mvOnuEponMacTxmCppReportBitMap(MV_U32 bitMap, MV_U32 macId);
MV_STATUS mvOnuEponMacTxmCppReportBitMapGet(MV_U32 *bitMap, MV_U32 macId);
MV_STATUS mvOnuEponMacTxmCppReportQueueSet(MV_U32 queueSet, MV_U32 macId);
MV_STATUS mvOnuEponMacTxmCppReportQueueSetGet(MV_U32 *queueSet, MV_U32 macId);
MV_STATUS mvOnuEponMacTxmCppReportQueueX(MV_U32 queueNum, MV_U32 queueCfg, MV_U32 macId);
MV_STATUS mvOnuEponMacTxmCppReportUpdate(MV_U32 validQueueReport, MV_U32 highestReportQueue, MV_U32 macId);
MV_STATUS mvOnuEponMacTxmCppRpmReportConfigSet(MV_U32 packetIndication, MV_U32 bitmap, MV_U32 qset, MV_U32 mode);
MV_STATUS mvOnuEponMacTxmCppRpmFifoDbaConfig(MV_U32 fifoEnable, MV_U32 dbaQueue, MV_U32 dbaOverhead);
MV_STATUS mvOnuEponMacTxmCppRpmIdxReportTableSet(MV_U32 llid, MV_U32 data, MV_U32 addr);
MV_STATUS mvOnuEponMacTxmTxCtrlFiFoFlush(MV_U32 llid);
MV_STATUS mvOnuEponMacTxmHighPriBitmapSet(MV_U32 llid, MV_U32 highPriEn);
MV_STATUS mvOnuEponMacTxmHighPriMapSet(MV_U32 llid, MV_U32 tx_bm);

/* ========================================================================== */
/*                        PCS Functions Section                               */
/* ========================================================================== */
MV_STATUS mvOnuEponMacPcsConfigSet(MV_U32 config);
MV_STATUS mvOnuEponMacPcsConfigGet(MV_U32 *config);
MV_STATUS mvOnuEponMacPcsDelaySet(MV_U32 delay);
MV_STATUS mvOnuEponMacPcsStatsGet(MV_U32 *legalFrameCnt, MV_U32 *maxFrameSizeErrCnt,
				  MV_U32 *parityLenErrCnt, MV_U32 *longGateErrCnt,
				  MV_U32 *protocolErrCnt, MV_U32 *minFrameSizeErrCnt,
				  MV_U32 *legalFecFrameCnt, MV_U32 *legalNonFecFrameCnt);
MV_STATUS mvOnuEponMacPcsFrameSizeLimitsSet(MV_U32 size, MV_U32 latency);
MV_STATUS mvOnuEponMacPcsFrameSizeLimitsGet(MV_U32 *size, MV_U32 *latency);
/* ========================================================================== */
/*                        DDM Functions Section                               */
/* ========================================================================== */
MV_STATUS mvOnuEponMacDdmDelaySet(MV_U32 delay);
MV_STATUS mvOnuEponMacDdmTxPolaritySet(MV_U32 txEnable, MV_U32 txPolarity, MV_U32 serdesPolarity,
				       MV_U32 xvrPolarity, MV_U32 burstEnable);
/* ========================================================================== */
/*                        Statistics Functions Section                        */
/* ========================================================================== */
MV_STATUS mvOnuEponMacRxStatsFcsErrorGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacRxStatsShortErrorGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacRxStatsLongErrorGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacRxStatsDataFrameGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacRxStatsCtrlFrameGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacRxStatsReportFrameGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacRxStatsGateFrameGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacTxStatsCtrlRegReqFrameGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacTxStatsCtrlRegAckFrameGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacTxStatsCtrlReportFrameGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacTxStatsDataFrameGet(MV_U32 *counter, MV_U32 macId);
MV_STATUS mvOnuEponMacTxStatsTxAllowedByteCountGet(MV_U32 *counter, MV_U32 macId);

/* ========================================================================== */
/*                        CPQ Functions Section                               */
/* ========================================================================== */
MV_STATUS mvOnuEponMacCpqRxCtrlHeaderQueueUsedCountGet(MV_U32 *usedCount);
MV_STATUS mvOnuEponMacCpqRxCtrlHeaderQueueReadData(MV_U32 *dataLow, MV_U32 *dataHigh);
MV_STATUS mvOnuEponMacCpqRxCtrlQueueUsedCountGet(MV_U32 *usedCount);
MV_STATUS mvOnuEponMacCpqRxCtrlQueueReadData(MV_U32 *data);
MV_STATUS mvOnuEponMacCpqRxRprtHeaderQueueUsedCountGet(MV_U32 *usedCount);
MV_STATUS mvOnuEponMacCpqRxRprtHeaderQueueReadData(MV_U32 *dataLow, MV_U32 *dataHigh);
MV_STATUS mvOnuEponMacCpqRxRprtQueueUsedCountGet(MV_U32 *usedCount);
MV_STATUS mvOnuEponMacCpqRxRprtQueueReadData(MV_U32 *data);
MV_STATUS mvOnuEponMacCpqTxCtrlHeaderQueueFree(MV_U32 *freeCount, MV_U32 macId);
MV_STATUS mvOnuEponMacCpqTxCtrlHeaderQueueWrite(MV_U32 data, MV_U32 macId);
MV_STATUS mvOnuEponMacCpqTxCtrlQueueFree(MV_U32 *freeCount, MV_U32 macId);
MV_STATUS mvOnuEponMacCpqTxCtrlQueueWrite(MV_U32 data, MV_U32 macId);

/* ========================================================================== */
/*                        PPv2 Functions Section                              */
/* ========================================================================== */
MV_STATUS mvOnuEponMacPPv2DbaLlidAndQueueIndirectAccess(MV_U32 llidIndex, MV_U32 queueIndex);
MV_STATUS mvOnuEponMacPPv2DbaQueueOverheadSet(MV_U32 queueOverhead);
MV_STATUS mvOnuEponMacPPv2DbaPktModSet(MV_U32 packetMod);
MV_STATUS mvOnuEponMacPPv2DbaPostFecSet(MV_U32 postFecOverhead);
MV_STATUS mvOnuEponMacPPv2DbaFecStateSet(MV_U32 fecState);

/* ========================================================================== */
/*                        SERDES Functions Section                            */
/* ========================================================================== */
MV_STATUS mvOnuEponMacSerdesPuRxWrite(MV_U32 enable);

/******************************************************************************/
/******************************************************************************/
/* ========================================================================== */
/* ========================================================================== */
/* ==                                                                      == */
/* ==           ==     ==   =========   =========   ==        ==           == */
/* ==           ==     ==   =========   =========   ===       ==           == */
/* ==           ==     ==   ==     ==   ==     ==   == ==     ==           == */
/* ==           ==     ==   ==     ==   ==     ==   ==  ==    ==           == */
/* ==           ==     ==   =========   ==     ==   ==   ==   ==           == */
/* ==           ==     ==   =========   ==     ==   ==    ==  ==           == */
/* ==           ==     ==   ==          ==     ==   ==     == ==           == */
/* ==           ==     ==   ==          ==     ==   ==      ====           == */
/* ==           =========   ==          =========   ==       ===           == */
/* ==           =========   ==          =========   ==        ==           == */
/* ==                                                                      == */
/* ========================================================================== */
/* ========================================================================== */
/******************************************************************************/
/******************************************************************************/

/* ========================================================================== */
/*                        DG Functions Section                                */
/* ========================================================================== */
MV_STATUS mvOnuPonMacDgInterruptEn(MV_U32 dgPol);
MV_STATUS mvOnuPonMacDgInterruptDis(void);

#endif /* _ONU_PON_MAC_H */
