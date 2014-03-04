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

#ifndef _ONU_GPON_REG_H
#define _ONU_GPON_REG_H

/* Include Files
   ------------------------------------------------------------------------------*/

/* Definitions
   ------------------------------------------------------------------------------*/
#define MV_ASIC_ONT_BASE (0)

//#define KW2_ASIC
/* Enums
   ------------------------------------------------------------------------------*/

/* ========================== */
/* = New ASIC Register Enum = */
/* ========================== */
typedef enum {
/* Enumeration                                          Description                        */
/* ======================================================================================= */
	mvAsicReg_Start                               = 0,

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

	/* Interrupt Registers */
	/* =================== */
	mvAsicReg_GPON_INTERRUPT_PON                  = 1,      /* GponMainInterrupt               */
	mvAsicReg_GPON_INTERRUPT_PON_MASK             = 2,      /* GponMainInterruptMask           */
	mvAsicReg_BIP_INTR_INTERVAL                   = 3,      /* GponBipInterruptCfgInterval     */
	mvAsicReg_RX_BIP_STATUS_FOR_INTERRUPT         = 4,      /* GponBipInterruptStatAccumulator */
	mvAsicReg_RAM_TEST_CONFIG                     = 5,      /* RamTestCfg                      */

	/* General Registers */
	/* ================= */
	mvAsicReg_GPON_GEN_MAC_VERSION                = 6, /* GenMACVersion                   */
	mvAsicReg_GPON_GEN_MAC_VERSION_ID             = 7,
	mvAsicReg_GPON_GEN_MAC_VERSION_MAC_ID         = 8,
	mvAsicReg_GPON_GEN_MAC_SCRATCH                = 9,      /* GenMACScratch                   */
	mvAsicReg_GPON_ONU_STATE                      = 10,     /* GenONUState                     */
	mvAsicReg_GPON_GEN_MICRO_SEC_CNT              = 11,     /* GenMicroSecCnt                  */
	mvAsicReg_GPON_TX_SERIAL_NUMBER               = 12,     /* GenSerialNumber                 */
	mvAsicReg_GPON_ONU_ID                         = 13,     /* GenONUId                        */
	mvAsicReg_GPON_ONU_ID_OID                     = 14,
	mvAsicReg_GPON_ONU_ID_V                       = 15,
	mvAsicReg_GPON_GEN_TOD_INT_SFC                = 16,     /* GenSFCforToDInterrupt           */
	mvAsicReg_GPON_GEN_TOD_INT_WIDTH              = 17,     /* GenToDInterruptWidth            */

	/* RX Registers */
	/* ============ */
	mvAsicReg_GPON_RX_CONFIG                      = 18, /* GrxCfg                          */
	mvAsicReg_GPON_RX_CONFIG_EN                   = 19,
	mvAsicReg_GPON_RX_CONFIG_BIT_ORDER            = 20,
	mvAsicReg_GPON_RX_PSA_CONFIG                  = 21, /* GrxCfgPsa                       */
	mvAsicReg_GPON_RX_PSA_CONFIG_SFM3             = 22,
	mvAsicReg_GPON_RX_PSA_CONFIG_SFM2             = 23,
	mvAsicReg_GPON_RX_PSA_CONFIG_SFM1             = 24,
	mvAsicReg_GPON_RX_PSA_CONFIG_FHM1             = 25,
	mvAsicReg_GPON_RX_FEC_CONFIG                  = 26, /* GrxCfgFec                       */
	mvAsicReg_GPON_RX_FEC_CONFIG_SWVAL            = 27,
	mvAsicReg_GPON_RX_FEC_CONFIG_FSW              = 28,
	mvAsicReg_GPON_RX_FEC_CONFIG_IGNP             = 29,
	mvAsicReg_GPON_RX_FEC_CONFIG_IND              = 30,
	mvAsicReg_GPON_RX_FEC_STAT0                   = 31,     /* GrxStatFec0                     */
	mvAsicReg_GPON_RX_FEC_STAT1                   = 32,     /* GrxStatFec1                     */
	mvAsicReg_GPON_RX_FEC_STAT2                   = 33,     /* GrxStatFec2                     */
	mvAsicReg_GPON_RX_SFRAME_COUNTER              = 34,     /* GrxStatSuperFrameCnt            */
	mvAsicReg_GPON_RX_PLOAMD_DATA_READ            = 35,     /* GrxDataPloamRead                */
	mvAsicReg_GPON_RX_PLOAMD_DATA_USED            = 36,     /* GrxDataPloamUsed                */
	mvAsicReg_GPON_RX_PLOAMD_CONFIG               = 37,     /* GrxCfgPrm                       */
	mvAsicReg_GPON_RX_PLOAMD_CONFIG_IGNC          = 38,
	mvAsicReg_GPON_RX_PLOAMD_CONFIG_IGNB          = 39,
	mvAsicReg_GPON_RX_PLOAMD_CONFIG_FALL          = 40,
	mvAsicReg_GPON_RX_PLOAMD_LOST_CRC_STATUS      = 41,     /* GrxStatPrmLostCrcCnt            */
	mvAsicReg_GPON_RX_PLOAMD_LOST_FULL_STATUS     = 42,     /* GrxStatPrmLostFullCnt           */
	mvAsicReg_GPON_RX_PLOAMD_RCVD_IDLE_STATUS     = 43,     /* GrxStatPrmRcvdIdleCnt           */
	mvAsicReg_GPON_RX_PLOAMD_RCVD_BROAD_STATUS    = 44,     /* GrxStatPrmRcvdBroadCnt          */
	mvAsicReg_GPON_RX_PLOAMD_RCVD_MYID_STATUS     = 45,     /* GrxStatPrmRcvdMyIdCnt           */
	mvAsicReg_GPON_RX_BIP_STATUS                  = 46,     /* GrxStatBip                      */
	mvAsicReg_GPON_RX_PLEND_CONFIG                = 47,     /* GrxCfgPdb                       */
	mvAsicReg_GPON_RX_PLEND_CONFIG_FN             = 48,
	mvAsicReg_GPON_RX_PLEND_CONFIG_UF             = 49,
	mvAsicReg_GPON_RX_PLEND_CONFIG_IGND           = 50,
	mvAsicReg_GPON_RX_PLEND_CONFIG_DFIX           = 51,
	mvAsicReg_GPON_RX_PLEND_CONFIG_IGNC           = 52,
	mvAsicReg_GPON_RX_PLEND_STATUS                = 53,     /* GrxStatPdb                      */
	mvAsicReg_GPON_RX_BWMAP_CONFIG                = 54,     /* GrxCfgBmd                       */
	mvAsicReg_GPON_RX_BWMAP_CONFIG_MSD            = 55,
	mvAsicReg_GPON_RX_BWMAP_CONFIG_DFIX           = 56,
	mvAsicReg_GPON_RX_BWMAP_CONFIG_IGNC           = 57,
	mvAsicReg_GPON_RX_BWMAP_STATUS0               = 58,     /* GrxStatBmd0                     */
	mvAsicReg_GPON_RX_BWMAP_STATUS1               = 59,     /* GrxStatBmd1                     */
	mvAsicReg_GPON_RX_AES_CONFIG                  = 60,     /* GrxCfgAesKeySwitch              */
	mvAsicReg_GPON_RX_AES_CONFIG_TIME             = 61,     /* GrxStatAesKeySwitch             */
	mvAsicReg_GPON_RX_AES_CONFIG_SWS              = 62,
	mvAsicReg_GPON_RX_AES_STATUS                  = 63,
	mvAsicReg_GPON_RX_AES_STATUS_HST              = 64,
	mvAsicReg_GPON_RX_AES_STATUS_HWS              = 65,
	mvAsicReg_GPON_RX_EQULIZATION_DELAY           = 66,     /* GrxCfgEqDelay                   */
	mvAsicReg_GPON_RX_INTERNAL_DELAY              = 67,     /* GrxCfgInterDelay                */
	mvAsicReg_GPON_RX_BW_MAP                      = 68,     /* GrxCfgBmdMapTcont               */
	mvAsicReg_GPON_RX_BW_MAP_ALID                 = 60,
	mvAsicReg_GPON_RX_BW_MAP_TCN                  = 70,
	mvAsicReg_GPON_RX_BW_MAP_EN                   = 71,

	/* GEM Registers */
	/* ============= */
	mvAsicReg_GPON_GEM_STAT_IDLE_GEM_CNT          = 72,     /* GemStatIdleGEMCnt               */
	mvAsicReg_GPON_GEM_STAT_VALID_GEM_CNT         = 73,     /* GemStatValidGEMCnt              */
	mvAsicReg_GPON_GEM_STAT_UNDEF_GEM_CNT         = 74,     /* GemStatUndefGEMCnt              */
	mvAsicReg_GPON_GEM_STAT_VALID_OMCI_CNT        = 75,     /* GemStatValidOMCICnt             */
	mvAsicReg_GPON_GEM_STAT_DROPPED_GEM_CNT       = 76,     /* GemStatDroppedGEMCnt            */
	mvAsicReg_GPON_GEM_STAT_DROPPED_OMCI_CNT      = 77,     /* GemStatDroppedOMCICnt           */
	mvAsicReg_GPON_GEM_STAT_ERROR_GEM_CNT         = 78,     /* GemStatErrorGEMCnt              */
	mvAsicReg_GPON_GEM_STAT_ONE_FIXED_HEC_ERR_CNT = 79,     /* GemStatOneFixedHecErrCnt        */
	mvAsicReg_GPON_GEM_STAT_TWO_FIXED_HEC_ERR_CNT = 80,     /* GemStatTwoFixedHecErrCnt        */
	mvAsicReg_GPON_GEM_STAT_VALID_GEM_PAYLOAD_CNT = 81,     /* GemStatValidGEMPayload          */
	mvAsicReg_GPON_GEM_STAT_UNDEF_GEM_PAYLOAD_CNT = 82,     /* GemStatUndefGEMPayload          */
	mvAsicReg_GPON_GEM_STAT_RSMBL_MEM_FLUSH_CNT   = 83,     /* GemStatRsmblMemFlushCnt         */
	mvAsicReg_GPON_GEM_STAT_LCDGi_CNT             = 84,     /* GemStatLCDGiCnt                 */
	mvAsicReg_GPON_GEM_STAT_CORR_ETH_FRAME_CNT    = 85,     /* GemStatCorrEthFrameCnt          */
	mvAsicReg_GPON_GEM_STAT_ERR_ETH_FRAME_CNT     = 86,     /* GemStatErrEthFrameCnt           */
	mvAsicReg_GPON_GEM_STAT_CORR_OMCI_FRAME_CNT   = 87,     /* GemStatCorrOMCIFrameCnt         */
	mvAsicReg_GPON_GEM_STAT_ERR_OMCI_FRAME_CNT    = 88,     /* GemStatErrOMCIFrameCnt          */
	mvAsicReg_GPON_GEM_CONFIG_PLI                 = 89,     /* GemCfgPLI                       */
	mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN       = 90,     /* GemCfgEthLength                 */
	mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN_MAX   = 91,
	mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN_MIN   = 92,
	mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN      = 93, /* GemCfgOMCILength                */
	mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN_MAX  = 94,
	mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN_MIN  = 95,
	mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT           = 96, /* GemCfgOMCIPort                  */
	mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT_ID        = 97,
	mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT_VALID     = 98,
	mvAsicReg_GPON_GEM_CONFIG_OMCI_FAST_PATH_VALID = 99,    /* GemCfgOMCIFastPath              */
	mvAsicReg_GPON_GEM_CONFIG_AES                 = 100,    /* GemCfgAES                       */
	mvAsicReg_GPON_GEM_AES_SHADOW_KEY             = 101,    /* GemCfgAESShadowKey0/1/2/3       */
	mvAsicReg_GPON_GEM_CFG_IPG_VALUE              = 102,    /* GemCfgIPGValue                  */
	mvAsicReg_GPON_GEM_CFG_IPG_VALID              = 103,    /* GemCfgIPGValid                  */
	mvAsicReg_GPON_GEM_RX_CTRL_Q_READ             = 104,    /* GemCfgQRead                     */
	mvAsicReg_GPON_GEM_RX_CTRL_DATA_USED          = 105,    /* GemCfgDataUsed                  */
	mvAsicReg_GPON_GEM_RX_CTRL_CMD_USED           = 106,    /* GemCfgCmdUsed                   */
	mvAsicReg_GPON_GEM_RX_CTRL_HQ_HEAD_LEN        = 107,    /* GemCfgHqHeadLen                 */
	mvAsicReg_GPON_GEM_RX_CTRL_HQ_GEM_PORT_ID     = 108,    /* GemCfgHqGemPortId               */
	mvAsicReg_GPON_GEM_RX_CTRL_HQ_GEM_PTI         = 109,    /* GemCfgHqGemPti                  */
	mvAsicReg_GPON_GEM_RX_CTRL_HQ_GEM_ERR         = 110,    /* GemCfgHqGemErr                  */

	/* TX Registers */
	/* ============ */
	mvAsicReg_GPON_TX_PREAMBLE                    = 111, /* GtxCfgPreamble0/1/2            */
	mvAsicReg_GPON_TX_PREAMBLE_PATT               = 112,
	mvAsicReg_GPON_TX_PREAMBLE_SIZE               = 113,
	mvAsicReg_GPON_TX_DELIMITER                   = 114, /* GtxCfgDelimiter                 */
	mvAsicReg_GPON_TX_DELIMITER_PAT               = 115,
	mvAsicReg_GPON_TX_DELIMITER_SIZE              = 116,
	mvAsicReg_GPON_TX_BURST_EN_PARAM              = 117, /* GtxCfgBurstEnable               */
	mvAsicReg_GPON_TX_BURST_EN_PARAM_STR          = 118,
	mvAsicReg_GPON_TX_BURST_EN_PARAM_STP          = 119,
	mvAsicReg_GPON_TX_BURST_EN_PARAM_DLY          = 120,
	mvAsicReg_GPON_TX_BURST_EN_PARAM_P            = 121,
	mvAsicReg_GPON_TX_BURST_EN_PARAM_MASK         = 122,
	mvAsicReg_GPON_TX_FDELAY                      = 123,    /* GtxCfgFinelDelay                */
	mvAsicReg_GPON_TX_PLOAM_DATA_WRITE            = 124,    /* GtxDataPloamWrite               */
	mvAsicReg_GPON_TX_PLOAM_DATA_WRITE_FREE       = 125,    /* GtxDataPloamStatus              */
	mvAsicReg_GPON_TX_PLS_CONSTANT                = 126,    /* GtxCfgPls                       */
	mvAsicReg_GPON_TX_DBR_REPORT_BLOCK_IDX        = 127,    /* GtxCfgDbr                       */
	mvAsicReg_GPON_TX_FE_RDI_INDICATION           = 128,    /* GtxCfgRdi                       */
	mvAsicReg_GPON_TX_STAT_GEM_PTI1               = 129,    /* GtxStatGemPti1                  */
	mvAsicReg_GPON_TX_STAT_GEM_PTI0               = 130,    /* GtxStatGemPti0                  */
	mvAsicReg_GPON_TX_STAT_GEM_IDLE               = 131,    /* GtxStatGemIdle                  */
	mvAsicReg_GPON_TX_STAT_TX_EN_CNT              = 132,    /* GtxStatTxEnableCounter          */
	mvAsicReg_GPON_TX_CONFIG_EN_THRESHOLD         = 133,    /* GtxCfgTxEnableThreshold         */
	mvAsicReg_GPON_TX_GSE_TRANS_THRESHOLD         = 134,    /* GtxCfgGseTransThreshold         */
	mvAsicReg_GPON_TX_CFG_AC_COUPLING             = 135,    /* GtxCfgAcCoupling                */
	mvAsicReg_GPON_TX_CFG_RAND_DELAY              = 136,    /* GtxCfgRandomDelay               */
	mvAsicReg_GPON_TX_STAT_TCONT_i_ETH_FRAMES     = 137,    /* GtxStatTcontiEtherFrames        */
	mvAsicReg_GPON_TX_STAT_TCONT_i_ETH_BYTES      = 138,    /* GtxStatTcontiEtherBytes         */
	mvAsicReg_GPON_TX_STAT_TCONT_i_GEM_FRAMES     = 139,    /* GtxStatTcontiGemFrames          */
	mvAsicReg_GPON_TX_STAT_TCONT_i_IDLE_GEM       = 140,    /* GtxStatTcontiIdleGem            */
	mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_ETH_FRAME = 141,    /* GtxStatTcontiEtherFrames        */
	mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_ETH_BYTES = 142,    /* GtxStatTcontiEtherBytes         */
	mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_GEM_FRAME = 143,    /* GtxStatTcontiGemFrames          */
	mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_IDLE_GEM  = 144,    /* GtxStatTcontiIdleGem            */
	mvAsicReg_GPON_TX_HIGH_PRI_CONFIG_EN          = 145,    /* GtxHighPriConfigEn              */
	mvAsicReg_GPON_TX_HIGH_PRI_FOR_FRAGMENT       = 146,    /* GtxHighPriForFragment           */
	mvAsicReg_GPON_TX_HIGH_PRI_VALID_BITMAP       = 147,    /* GtxHighPriValidBm               */
	mvAsicReg_GPON_TX_HIGH_PRI_MAP_0              = 148,    /* GtxHighPriMap0                  */
	mvAsicReg_GPON_TX_HIGH_PRI_MAP_1              = 149,    /* GtxHighPriMap1                  */

	/* UTM Registers */
	/* ============= */
	mvAsicReg_GPON_UTM_CONFIG_OMCI_PORT_ID        = 187,    /* UtmCfgOmciPnum                  */
	mvAsicReg_GPON_UTM_CONFIG_OMCI_PORT_VALID     = 188,    /* UtmCfgOmciPvalid                */
	mvAsicReg_GPON_UTM_CONFIG_TC_PERIOD           = 189,    /* UtmCfgTcPeriod                  */
	mvAsicReg_GPON_UTM_CONFIG_TC_PERIOD_VALID     = 190,    /* UtmCfgTcPeriodValid             */
	mvAsicReg_GPON_UTM_ACTIVE_TX_BITMAP           = 191,    /* UtmActiveTxBitmap               */
	mvAsicReg_GPON_UTM_ACTIVE_TX_BITMAP_VALID     = 192,    /* UtmActiveTxBitmapValid          */
	mvAsicReg_GPON_UTM_FLUSH                      = 193,    /* UtmFlush                        */
	mvAsicReg_GPON_UTM_GENERAL                    = 194,    /* UtmGeneral                      */
	mvAsicReg_GPON_UTM_DEBUG                      = 195,    /* UtmDebug                        */
	mvAsicReg_GPON_UTM_REPORT_CONFIG              = 196,    /* UtmReportConfig                 */
	mvAsicReg_GPON_UTM_REPORT_MAP_0               = 197,    /* UtmReportMap0                   */
	mvAsicReg_GPON_UTM_REPORT_MAP_1               = 198,    /* UtmReportMap1                   */


	/* SGL Registers */
	/* ============= */
	mvAsicReg_GPON_SGL_SW_RESET                   = 200,    /* SglSwResetReg                   */
	mvAsicReg_GPON_SGL_CONFIG_REG                 = 201,    /* SglCfgReg                       */
	mvAsicReg_GPON_SGL_STATS_REG                  = 202,    /* SglStatReg                      */

	/*  Memory Registers           */
	/* =========================== */
	mvAsicReg_GPON_TX_CONST_DATA_RAM              = 206,
	mvAsicReg_GPON_TX_CONST_DATA_RAM_IDLE         = 207,
	mvAsicReg_GPON_TX_CONST_DATA_RAM_SN           = 208,
	mvAsicReg_GPON_GEM_AES_PID_TABLE              = 209,
	mvAsicReg_GPON_GEM_VALID_PID_TABLE            = 210,
	mvAsicReg_GPON_TEST                           = 211,

	/*  GPON MIB Counter Set       */
	/* =========================== */
	mvAsicReg_PON_MAC_MIB_COUNTERS_0              = 212,
	mvAsicReg_PON_MAC_MIB_COUNTERS_1              = 213,
	mvAsicReg_PON_MAC_MIB_COUNTERS_2              = 214,
	mvAsicReg_PON_MAC_MIB_COUNTERS_3              = 215,
	mvAsicReg_PON_MAC_MIB_COUNTERS_4              = 216,
	mvAsicReg_PON_MAC_MIB_COUNTERS_5              = 217,
	mvAsicReg_PON_MAC_MIB_COUNTERS_6              = 218,
	mvAsicReg_PON_MAC_MIB_COUNTERS_7              = 219,
	mvAsicReg_PON_MAC_MIB_COUNTERS_8              = 220,
	mvAsicReg_PON_MAC_MIB_COUNTERS_9              = 221,
	mvAsicReg_PON_MAC_MIB_COUNTERS_10             = 222,
	mvAsicReg_PON_MAC_MIB_COUNTERS_11             = 223,
	mvAsicReg_PON_MAC_MIB_COUNTERS_12             = 224,
	mvAsicReg_PON_MAC_MIB_COUNTERS_13             = 225,
	mvAsicReg_PON_MAC_MIB_COUNTERS_14             = 226,
	mvAsicReg_PON_MAC_MIB_COUNTERS_15             = 227,
	mvAsicReg_PON_MAC_MIB_COUNTERS_16             = 228,
	mvAsicReg_PON_MAC_MIB_COUNTERS_17             = 229,
	mvAsicReg_PON_MAC_MIB_COUNTERS_18             = 230,
	mvAsicReg_PON_MAC_MIB_COUNTERS_19             = 231,
	mvAsicReg_PON_MAC_MIB_COUNTERS_20             = 232,
	mvAsicReg_PON_MAC_MIB_COUNTERS_21             = 233,
	mvAsicReg_PON_MAC_MIB_COUNTERS_22             = 234,
	mvAsicReg_PON_MAC_MIB_COUNTERS_23             = 235,
	mvAsicReg_PON_MAC_MIB_COUNTERS_24             = 236,
	mvAsicReg_PON_MAC_MIB_COUNTERS_25             = 237,
	mvAsicReg_PON_MAC_MIB_COUNTERS_26             = 238,
	mvAsicReg_PON_MAC_MIB_COUNTERS_27             = 239,
	mvAsicReg_PON_MAC_MIB_COUNTERS_28             = 240,
	mvAsicReg_PON_MAC_MIB_COUNTERS_29             = 241,
	mvAsicReg_PON_MAC_MIB_COUNTERS_30             = 242,
	mvAsicReg_PON_MAC_MIB_COUNTERS_31             = 243,

	/*  GPON MIB Counter Control Registers */
	/* =================================== */

	mvAsicReg_PON_MIB_DEFAULT                     = 244,
	mvAsicReg_PON_MAC_MIB_CONTROL_0               = 245,
	mvAsicReg_PON_MAC_MIB_CONTROL_1               = 246,
	mvAsicReg_PON_MAC_MIB_CONTROL_2               = 247,
	mvAsicReg_PON_MAC_MIB_CONTROL_3               = 248,
	mvAsicReg_PON_MAC_MIB_CONTROL_4               = 249,
	mvAsicReg_PON_MAC_MIB_CONTROL_5               = 250,
	mvAsicReg_PON_MAC_MIB_CONTROL_6               = 251,
	mvAsicReg_PON_MAC_MIB_CONTROL_7               = 252,
	mvAsicReg_PON_MAC_MIB_CONTROL_8               = 253,
	mvAsicReg_PON_MAC_MIB_CONTROL_9               = 254,
	mvAsicReg_PON_MAC_MIB_CONTROL_10              = 255,
	mvAsicReg_PON_MAC_MIB_CONTROL_11              = 256,
	mvAsicReg_PON_MAC_MIB_CONTROL_12              = 257,
	mvAsicReg_PON_MAC_MIB_CONTROL_13              = 258,
	mvAsicReg_PON_MAC_MIB_CONTROL_14              = 259,
	mvAsicReg_PON_MAC_MIB_CONTROL_15              = 260,
	mvAsicReg_PON_MAC_MIB_CONTROL_16              = 261,
	mvAsicReg_PON_MAC_MIB_CONTROL_17              = 262,
	mvAsicReg_PON_MAC_MIB_CONTROL_18              = 263,
	mvAsicReg_PON_MAC_MIB_CONTROL_19              = 264,
	mvAsicReg_PON_MAC_MIB_CONTROL_20              = 265,
	mvAsicReg_PON_MAC_MIB_CONTROL_21              = 266,
	mvAsicReg_PON_MAC_MIB_CONTROL_22              = 267,
	mvAsicReg_PON_MAC_MIB_CONTROL_23              = 268,
	mvAsicReg_PON_MAC_MIB_CONTROL_24              = 269,
	mvAsicReg_PON_MAC_MIB_CONTROL_25              = 270,
	mvAsicReg_PON_MAC_MIB_CONTROL_26              = 271,
	mvAsicReg_PON_MAC_MIB_CONTROL_27              = 272,
	mvAsicReg_PON_MAC_MIB_CONTROL_28              = 273,
	mvAsicReg_PON_MAC_MIB_CONTROL_29              = 274,
	mvAsicReg_PON_MAC_MIB_CONTROL_30              = 275,
	mvAsicReg_PON_MAC_MIB_CONTROL_31              = 276,

	mvAsicReg_PON_MAC_MIB_COUNTERS_START          = 280,
	mvAsicReg_PON_MAC_MIB_CONTROL_START           = 281,

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
/* ==           ==          ==          ==     ==   ==     == ==           == */
/* ==           ==          ==          ==     ==   ==      ====           == */
/* ==           =========   ==          =========   ==       ===           == */
/* ==           =========   ==          =========   ==        ==           == */
/* ==                                                                      == */
/* ========================================================================== */
/* ========================================================================== */
/******************************************************************************/
/******************************************************************************/

	/*  EPON Interrupt Registers   */
	/* =========================== */
	mvAsicReg_EPON_INTR_REG                            = 300,
	mvAsicReg_EPON_INTR_MASK                           = 301,
	mvAsicReg_EPON_DBR_INT                             = 302,
	mvAsicReg_EPON_DBR_INT_LLID_MASK                   = 303,
	mvAsicReg_EPON_DBR_INT_MASK                        = 304,
	mvAsicReg_EPON_DBR_LLID_INT                        = 305,

	/*  EPON General Registers     */
	/* =========================== */
	mvAsicReg_EPON_GEN_MAC_VERSION_ADDR                = 314,
	mvAsicReg_EPON_GEN_ONT_ENABLE                      = 315,
	mvAsicReg_EPON_GEN_ONT_RX_ENABLE                   = 316,
	mvAsicReg_EPON_GEN_ONT_TX_ENABLE                   = 317,
	mvAsicReg_EPON_GEN_ONT_STATE                       = 318,
	mvAsicReg_EPON_GEN_ONT_STATE_REG_AUTO_EN           = 319,
	mvAsicReg_EPON_GEN_ONT_STATE_REG                   = 320,
	mvAsicReg_EPON_GEN_ONT_STATE_REREG_AUTO_EN         = 321,
	mvAsicReg_EPON_GEN_ONT_STATE_DEREG_AUTO_EN         = 322,
	mvAsicReg_EPON_GEN_ONT_STATE_NACK_AUTO_EN          = 323,
	mvAsicReg_EPON_GEN_ONT_UP_RX_LLID_PAD              = 324,
	mvAsicReg_EPON_GEN_TQ_SIZE                         = 325,
	mvAsicReg_EPON_GEN_LASER_PARAM                     = 326,
	mvAsicReg_EPON_GEN_TAIL_GUARD                      = 327,
	mvAsicReg_EPON_GEN_SYNC_TIME                       = 328,
	mvAsicReg_EPON_GEN_BROADCAST_ADDR_LOW              = 329,
	mvAsicReg_EPON_GEN_BROADCAST_ADDR_HIGH             = 330,
	mvAsicReg_EPON_GEN_CONFIGURATION                   = 331,
	mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_REPORT       = 332,
	mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_ACK          = 333,
	mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_REQUEST      = 334,
	mvAsicReg_EPON_GEN_SLD                             = 335,
	mvAsicReg_EPON_GEN_MAC_CONTROL_TYPE                = 336,
	mvAsicReg_EPON_GEN_LOCAL_TIMESTAMP                 = 337,
	mvAsicReg_EPON_GEN_NUM_OF_PENDING_GRANTS           = 338,
	mvAsicReg_EPON_GEN_SGL_STATUS                      = 339,
	mvAsicReg_EPON_GEN_PON_PHY_DEBUG                   = 340,
	mvAsicReg_EPON_GEN_SGL_SW_RESET                    = 341,
	mvAsicReg_EPON_GEN_SGL_CONFIG                      = 342,
	mvAsicReg_EPON_GEN_SGL_DEBOUNCE_CONFIG             = 343,
	mvAsicReg_EPON_GEN_SGL_DEBOUNCE_ENABLE             = 344,
	mvAsicReg_EPON_GEN_TCH_CHURNING_KEY                = 345,
	mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE1              = 346,
	mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE2              = 347,
	mvAsicReg_EPON_GEN_DRIFT_THRESHOLD                 = 348,
	mvAsicReg_EPON_GEN_TIMESTAMP_VAL_FOR_INTR          = 349,
	mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE3              = 350,
	mvAsicReg_EPON_GEN_LLID_CRC_CONFIG                 = 351,
	mvAsicReg_EPON_GEN_TIMESTAMP_CONFIG                = 352,
	mvAsicReg_EPON_GEN_ONT_RX_TIMESTAMP_CONFIG         = 353,
	mvAsicReg_EPON_GEN_ONT_REGISTER_PACKET_PADDING     = 354,
	mvAsicReg_EPON_GEN_MAC_ADDR_LOW                    = 355,
	mvAsicReg_EPON_GEN_MAC_ADDR_HIGH                   = 356,
	mvAsicReg_EPON_GEN_UTM_TC_PERIOD                   = 357,
	mvAsicReg_EPON_GEN_UTM_TC_PERIOD_VALID             = 358,
	mvAsicReg_EPON_GEN_GEM_IPG_VAL                     = 359,
	mvAsicReg_EPON_GEN_GEM_IPG_VAL_VALID               = 360,
	mvAsicReg_EPON_GEN_ACT_TX_BITMAP                   = 361,
	mvAsicReg_EPON_GEN_ACT_TX_VALID                    = 362,
	mvAsicReg_EPON_GEN_TOD_INT_WIDTH                   = 363, /* General_ToD_Interrupt_Width */
	mvAsicReg_EPON_UTM_DEBUG                           = 364,
	mvAsicReg_EPON_UTM_GENERAL                         = 365,
	mvAsicReg_EPON_TXM_EXT_REPORT_STATE                = 366,
	mvAsicReg_EPON_TXM_EXT_REPORT_STATE_PER_TX         = 367,
	mvAsicReg_EPON_TXM_EXT_REPORT_MAP_0                = 368,
	mvAsicReg_EPON_TXM_EXT_REPORT_MAP_1                = 369,

	/*  EPON RXP Registers         */
	/* =========================== */
	mvAsicReg_EPON_RXP_PACKET_SIZE_LIMIT               = 374,
	mvAsicReg_EPON_RXP_PACKET_FILTER                   = 375,
	mvAsicReg_EPON_RXP_CTRL_FRAME_FORWARD              = 376,
	mvAsicReg_EPON_RXP_LLT_LLID_DATA                   = 377,
	mvAsicReg_EPON_RXP_LLT_LLID_DATA_VALUE             = 378,
	mvAsicReg_EPON_RXP_LLT_LLID_DATA_INDEX             = 379,
	mvAsicReg_EPON_RXP_LLT_LLID_DATA_VALID             = 380,
	mvAsicReg_EPON_RXP_ENCRYPTION_CONFIG               = 381,
	mvAsicReg_EPON_RXP_ENCRYPTION_KEY0                 = 382,
	mvAsicReg_EPON_RXP_ENCRYPTION_KEY1                 = 383,
	mvAsicReg_EPON_RXP_DATA_FIFO_THRESH                = 384,

	/*  EPON GPM Registers         */
	/* =========================== */
	mvAsicReg_EPON_GPM_MAX_FUTURE_GRANT_TIME           = 385,
	mvAsicReg_EPON_GPM_MIN_PROCESSING_TIME             = 386,
	mvAsicReg_EPON_GPM_DISCOVERY_GRANT_LENGTH          = 387,
	mvAsicReg_EPON_GPM_RX_SYNC_TIME                    = 388,
	mvAsicReg_EPON_GPM_GRANT_VALID                     = 389,
	mvAsicReg_EPON_GPM_GRANT_MAX_FUTURE_TIME_ERR       = 390,
	mvAsicReg_EPON_GPM_MIN_PROC_TIME_ERR               = 391,
	mvAsicReg_EPON_GPM_LENGTH_ERR                      = 392,
	mvAsicReg_EPON_GPM_DISCOVERY_AND_REGISTERED_ERR    = 393,
	mvAsicReg_EPON_GPM_FIFO_FULL_ERR                   = 394,
	mvAsicReg_EPON_GPM_OPC_DISC_NOT_REG_BCAST          = 395,
	mvAsicReg_EPON_GPM_OPC_REG_NOT_DISC                = 396,
	mvAsicReg_EPON_GPM_OPC_DISC_NOT_REG_NOT_BCAST      = 397,
	mvAsicReg_EPON_GPM_OPC_DROPED_GRANT                = 398,
	mvAsicReg_EPON_GPM_OPC_HIDDEN_GRANT                = 399,
	mvAsicReg_EPON_GPM_OPC_BACK_TO_BACK_GRANT          = 400,

	/*  EPON TXM Registers         */
	/* =========================== */
	mvAsicReg_EPON_TXM_DEFAULT_OVERHEAD                = 403,
	mvAsicReg_EPON_TXM_CONFIGURATION                   = 404,
	mvAsicReg_EPON_TXM_POWER_UP                        = 405, /* Tx_Multiplexer_Power_Up */
	mvAsicReg_EPON_TXM_TX_LLID                         = 406,
	mvAsicReg_EPON_TXM_CPP_RPRT_CONFIG                 = 407,
	mvAsicReg_EPON_TXM_CPP_RPRT_BIT_MAP                = 408,
	mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_SET              = 409,
	mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_0                = 410,
	mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_1                = 411,
	mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_2                = 412,
	mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_3                = 413,
	mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_4                = 414,
	mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_5                = 415,
	mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_6                = 416,
	mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_7                = 417,
	mvAsicReg_EPON_TXM_CPP_RPRT_FEC_CONFIG             = 418,
	mvAsicReg_EPON_TXM_CPP_RPM_REPORT_CONFIG           = 419,
	mvAsicReg_EPON_TXM_CPP_RPM_CTRL_FIFO_DBA           = 420,
	mvAsicReg_EPON_TXM_CPP_RPM_ADX_REPORT_TBL          = 421,
	mvAsicReg_EPON_TXM_CTRL_FIFO_FLUSH                 = 425,
	mvAsicReg_EPON_TXM_HIGH_PRI_TX_STATE_BITMAP        = 426,
	mvAsicReg_EPON_TXM_HIGH_PRI_TX_MAP_0               = 427,
	mvAsicReg_EPON_TXM_HIGH_PRI_TX_MAP_1               = 428,

	/*  EPON PCS Registers         */
	/* =========================== */
	mvAsicReg_EPON_PCS_CONFIGURATION                   = 430,
	mvAsicReg_EPON_PCS_CONFIGURATION_RX_ENABLE         = 431,
	mvAsicReg_EPON_PCS_CONFIGURATION_TX_ENABLE         = 432,
	mvAsicReg_EPON_PCS_DELAY_CONFIG                    = 433,
	mvAsicReg_EPON_PCS_STATS_FEC_0                     = 434,
	mvAsicReg_EPON_PCS_STATS_FEC_1                     = 435,
	mvAsicReg_EPON_PCS_STATS_FEC_2                     = 436,
	mvAsicReg_EPON_PCS_STATS_0                         = 437,
	mvAsicReg_EPON_PCS_STATS_1                         = 438,
	mvAsicReg_EPON_PCS_STATS_2                         = 439,
	mvAsicReg_EPON_PCS_STATS_3                         = 440,
	mvAsicReg_EPON_PCS_STATS_4                         = 441,
	mvAsicReg_EPON_PCS_FRAME_SZ_LIMITS                 = 442,

	/*  EPON DDM Registers         */
	/* =========================== */
	mvAsicReg_EPON_DDM_DELAY_CONFIG                    = 458,
	mvAsicReg_EPON_DDM_TX_POLARITY                     = 459,
	/* statistics */
	mvAsicReg_EPON_STAT_RXP_FCS_ERROR_CNT              = 460,
	mvAsicReg_EPON_STAT_RXP_SHORT_ERROR_CNT            = 461,
	mvAsicReg_EPON_STAT_RXP_LONG_ERROR_CNT             = 462,
	mvAsicReg_EPON_STAT_RXP_DATA_FRAMES_CNT            = 463,
	mvAsicReg_EPON_STAT_RXP_CTRL_FRAMES_CNT            = 464,
	mvAsicReg_EPON_STAT_RXP_REPORT_FRAMES_CNT          = 465,
	mvAsicReg_EPON_STAT_RXP_GATE_FRAMES_CNT            = 466,
	mvAsicReg_EPON_STAT_TXP_CTRL_REG_REQ_FRAMES_CNT    = 467,
	mvAsicReg_EPON_STAT_TXP_CTRL_REG_ACK_FRAMES_CNT    = 468,
	mvAsicReg_EPON_STAT_TXP_CTRL_REPORT_FRAMES_CNT     = 469,
	mvAsicReg_EPON_STAT_TXP_DATA_FRAMES_CNT            = 470,
	mvAsicReg_EPON_STAT_TXP_TX_ALLOWED_BYTE_CNT        = 471,

	/*  EPON Control Packet queue Registers   */
	/* =====================================  */
	mvAsicReg_EPON_CPQ_RX_CTRL_Q_READ                  = 472,
	mvAsicReg_EPON_CPQ_RX_CTRL_Q_USED                  = 473,
	mvAsicReg_EPON_CPQ_RX_RPRT_Q_READ                  = 474,
	mvAsicReg_EPON_CPQ_RX_RPRT_Q_USED                  = 475,
	mvAsicReg_EPON_CPQ_RX_CTRL_HQ_READ_L               = 476,
	mvAsicReg_EPON_CPQ_RX_CTRL_HQ_READ_H               = 477,
	mvAsicReg_EPON_CPQ_RX_CTRL_HQ_USED                 = 478,
	mvAsicReg_EPON_CPQ_RX_RPRT_HQ_READ_L               = 479,
	mvAsicReg_EPON_CPQ_RX_RPRT_HQ_READ_H               = 480,
	mvAsicReg_EPON_CPQ_RX_RPRT_HQ_USED                 = 481,
	mvAsicReg_EPON_CPQ_TX_CTRL_Q_WRITE                 = 482,
	mvAsicReg_EPON_CPQ_TX_CTRL_Q_FREE                  = 483,
	mvAsicReg_EPON_CPQ_TX_CTRL_HQ_WRITE                = 484,
	mvAsicReg_EPON_CPQ_TX_CTRL_HQ_FREE                 = 485,

	/*  P2P Registers                         */
	/* =====================================  */
	mvAsicReg_P2P_GEN_ONT_MODE                         = 490,
	mvAsicReg_P2P_PCS_CONFIGURATION                    = 491,
	mvAsicReg_P2P_TXM_CFG_MODE                         = 492,

	/*  SERDES Registers                      */
	/* =====================================  */
	mvAsicReg_PON_SERDES_PHY_CTRL_0                    = 500,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_Pll             = 501,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_RX              = 502,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_TX              = 503,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_RST                = 504,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_RST_TX_DOUT        = 505,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_RST_CORE           = 506,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_RX_INIT            = 507,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_RX_RATE            = 508,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_TX_RATE            = 509,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_SEL_GEPON          = 510,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_SEL_REF_CLK        = 511,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_REF_CLK_25M        = 512,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_CID_REV            = 513,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_TX_SOURCE       = 514,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_REF_CLK_DISABLE    = 515,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_TX_IDLE            = 516,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_IO_EN          = 517,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_SW_HW_SELECT   = 518,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_SW_FORCE       = 519,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_SW_VALUE       = 520,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_TOD_OVERRIDE       = 521,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_DIG_TEST_EN        = 522,
	mvAsicReg_PON_SERDES_PHY_CTRL_0_EPON_MODE_SEL      = 523,

	mvAsicReg_PON_SERDES_PHY_CTRL_1_PHY_SOURCE         = 524,

	mvAsicReg_PON_SERDES_PHY_STATUS_INIT_DONE          = 525,
	mvAsicReg_PON_SERDES_PHY_STATUS_READY_TX           = 526,
	mvAsicReg_PON_SERDES_PHY_STATUS_READY_RX           = 527,
	mvAsicReg_PON_SERDES_PHY_STATUS_REF_CLK_DIS_ACK    = 528,

	mvAsicReg_PON_SERDES_CLK_SEL                       = 529,
	mvAsicReg_PON_SERDES_CLK_EN                        = 530,
	mvAsicReg_PON_MAC_CLK_SEL                          = 531,
	mvAsicReg_PON_REF_CLK_EN                           = 532,

	mvAsicReg_PON_MAC_GPON_CLK_EN                      = 533,
	mvAsicReg_PON_MAC_SW_RESET_CTRL                    = 534,

	mvAsicReg_PON_SERDES_INTERNAL_POWER_REG_0          = 535,
	mvAsicReg_PON_SERDES_INTERNAL_REG_51               = 536,
	mvAsicReg_PON_SERDES_INTERNAL_REG_3D               = 537,
	mvAsicReg_PON_SERDES_INTERNAL_REG_23_LOOP_TIMING   = 538,
	mvAsicReg_PON_SERDES_INTERNAL_REG_26_TX_DRV_IDLE   = 539,
	mvAsicReg_PON_SERDES_INTERNAL_REG_OFFSET_148       = 540,
	mvAsicReg_PON_SERDES_INTERNAL_REG_OFFSET_178       = 541,

	mvAsicReg_PON_XVR_TX_DATA_OUT_17                   = 550,
	mvAsicReg_PON_XVR_TX_DATA_OUT_36                   = 551,
	mvAsicReg_PON_XVR_TX_DATA_OUT_37                   = 552,
	mvAsicReg_PON_XVR_TX_DATA_OUT_41                   = 553,
	mvAsicReg_PON_XVR_TX_DATA_OUT_45                   = 554,
	mvAsicReg_PON_XVR_TX_DATA_OUT_65                   = 555,
	mvAsicReg_PON_XVR_TX_DATA_OUT_68                   = 556,
	mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_17              = 557,
	mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_36              = 558,
	mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_37              = 559,
	mvAsicReg_PON_XVR_RX_INPUT_41                      = 560,
	mvAsicReg_PON_XVR_RX_INPUT_45                      = 561,
	mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_65              = 562,
	mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_68              = 563,
	mvAsicReg_PON_SERDES_INTERNAL_OPEN_TX_DOOR_15      = 564,
	mvAsicReg_PON_SERDES_POWER_PHY_MODE                = 565,
	mvAsicReg_PON_SYS_LED_ENABLE                       = 566,
	mvAsicReg_PON_SYS_LED_STATE                        = 567,
	mvAsicReg_PON_SYS_LED_BLINK                        = 568,
	mvAsicReg_PON_SYS_LED_BLINK_FREQ                   = 569,
	mvAsicReg_PON_SYNC_LED_ENABLE                      = 570,
	mvAsicReg_PON_SYNC_LED_STATE                       = 571,
	mvAsicReg_PON_SYNC_LED_BLINK                       = 572,
	mvAsicReg_PON_SYNC_LED_BLINK_FREQ                  = 573,
	mvAsicReg_PON_LED_BLINK_FREQ_A_ON                  = 574,
	mvAsicReg_PON_LED_BLINK_FREQ_A_OFF                 = 575,
	mvAsicReg_PON_LED_BLINK_FREQ_B_ON                  = 576,
	mvAsicReg_PON_LED_BLINK_FREQ_B_OFF                 = 577,
	mvAsicReg_PON_DG_CTRL_EN                           = 578,
	mvAsicReg_PON_DG_CTRL_POLARITY                     = 579,
	mvAsicReg_PON_DG_THRESHOLD                         = 580,
	mvAsicReg_PT_PATTERN_SELECT                        = 581,
	mvAsicReg_PT_PATTERN_ENABLED                       = 582,
	mvAsicReg_PT_PATTERN_DATA                          = 583,
	mvAsicReg_PT_PATTERN_USER_DATA_01                  = 584,
	mvAsicReg_PT_PATTERN_USER_DATA_02                  = 585,
	mvAsicReg_PT_PATTERN_USER_DATA_03                  = 586,
	mvAsicReg_PT_PATTERN_USER_DATA_04                  = 587,
	mvAsicReg_PT_PATTERN_USER_DATA_05                  = 588,

	mvAsicReg_GUNIT_TX_0_PKT_MOD_MAX_HEAD_SIZE_CFG     = 590,
	mvAsicReg_GUNIT_TX_1_PKT_MOD_MAX_HEAD_SIZE_CFG     = 591,
	mvAsicReg_GUNIT_TX_2_PKT_MOD_MAX_HEAD_SIZE_CFG     = 592,
	mvAsicReg_GUNIT_TX_3_PKT_MOD_MAX_HEAD_SIZE_CFG     = 593,
	mvAsicReg_GUNIT_TX_4_PKT_MOD_MAX_HEAD_SIZE_CFG     = 594,
	mvAsicReg_GUNIT_TX_5_PKT_MOD_MAX_HEAD_SIZE_CFG     = 595,
	mvAsicReg_GUNIT_TX_6_PKT_MOD_MAX_HEAD_SIZE_CFG     = 596,
	mvAsicReg_GUNIT_TX_7_PKT_MOD_MAX_HEAD_SIZE_CFG     = 597,

	mvAsicReg_GUNIT_TX_0_PKT_MOD_STATS_PKT_COUNT       = 600,
	mvAsicReg_GUNIT_TX_1_PKT_MOD_STATS_PKT_COUNT       = 601,
	mvAsicReg_GUNIT_TX_2_PKT_MOD_STATS_PKT_COUNT       = 602,
	mvAsicReg_GUNIT_TX_3_PKT_MOD_STATS_PKT_COUNT       = 603,
	mvAsicReg_GUNIT_TX_4_PKT_MOD_STATS_PKT_COUNT       = 604,
	mvAsicReg_GUNIT_TX_5_PKT_MOD_STATS_PKT_COUNT       = 605,
	mvAsicReg_GUNIT_TX_6_PKT_MOD_STATS_PKT_COUNT       = 606,
	mvAsicReg_GUNIT_TX_7_PKT_MOD_STATS_PKT_COUNT       = 607,

	mvAsicReg_PPv2_TX_PON_BYTE_CNT_INDEX               = 610,
	mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG1_QUE_OVR     = 611,
	mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG1_PKT_MOD     = 612,
	mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG2_POST_FEC    = 613,
	mvAsicReg_PPv2_PON_FEC                             = 614,

	mvAsicReg_C5000_BIT_1                              = 615,
	mvAsicReg_C5008_BIT_0                              = 616,
	mvAsicReg_C5008_BIT_3                              = 617,
	mvAsicReg_18900_BIT_14                             = 618,
	mvAsicReg_18900_BIT_19                             = 619,
	mvAsicReg_18900_BIT_21                             = 620,
	mvAsicReg_GMAC1_AUTONEG_SPEED                      = 621,
	mvAsicReg_GMAC1_AUTONEG_FLOW_CONTROL_MODE          = 622,
	mvAsicReg_GMAC1_AUTONEG_FLOW_CONTROL_ENABLE        = 623,
	mvAsicReg_GMAC1_AUTONEG_DUPLEX_MODE                = 624,
	mvAsicReg_GMAC1_AUTONEG_DUPLEX_ENABLE              = 625,

	/*  MPP Registers                         */
	/* =====================================  */
	mvAsicReg_PON_MPP_00                               = 700,
	mvAsicReg_PON_MPP_01                               = 701,
	mvAsicReg_PON_MPP_02                               = 702,
	mvAsicReg_PON_MPP_03                               = 703,
	mvAsicReg_PON_MPP_04                               = 704,
	mvAsicReg_PON_MPP_05                               = 705,
	mvAsicReg_PON_MPP_06                               = 706,
	mvAsicReg_PON_MPP_07                               = 707,
	mvAsicReg_PON_MPP_08                               = 708,
	mvAsicReg_PON_MPP_09                               = 709,
	mvAsicReg_PON_MPP_10                               = 710,
	mvAsicReg_PON_MPP_11                               = 711,
	mvAsicReg_PON_MPP_12                               = 712,
	mvAsicReg_PON_MPP_13                               = 713,
	mvAsicReg_PON_MPP_14                               = 714,
	mvAsicReg_PON_MPP_15                               = 715,
	mvAsicReg_PON_MPP_16                               = 716,
	mvAsicReg_PON_MPP_17                               = 717,
	mvAsicReg_PON_MPP_18                               = 718,
	mvAsicReg_PON_MPP_19                               = 719,
	mvAsicReg_PON_MPP_20                               = 720,
	mvAsicReg_PON_MPP_21                               = 721,
	mvAsicReg_PON_MPP_22                               = 722,
	mvAsicReg_PON_MPP_23                               = 723,
	mvAsicReg_PON_MPP_24                               = 724,
	mvAsicReg_PON_MPP_25                               = 725,
	mvAsicReg_PON_MPP_26                               = 726,
	mvAsicReg_PON_MPP_27                               = 727,
	mvAsicReg_PON_MPP_28                               = 728,
	mvAsicReg_PON_MPP_29                               = 729,
	mvAsicReg_PON_MPP_30                               = 730,
	mvAsicReg_PON_MPP_31                               = 731,
	mvAsicReg_PON_MPP_32                               = 732,
	mvAsicReg_PON_MPP_33                               = 733,
	mvAsicReg_PON_MPP_34                               = 734,
	mvAsicReg_PON_MPP_35                               = 735,
	mvAsicReg_PON_MPP_36                               = 736,
	mvAsicReg_PON_MPP_37                               = 737,
	mvAsicReg_PON_MPP_38                               = 738,
	mvAsicReg_PON_MPP_39                               = 739,
	mvAsicReg_PON_MPP_40                               = 740,
	mvAsicReg_PON_MPP_41                               = 741,
	mvAsicReg_PON_MPP_42                               = 742,
	mvAsicReg_PON_MPP_43                               = 743,
	mvAsicReg_PON_MPP_44                               = 744,
	mvAsicReg_PON_MPP_45                               = 745,
	mvAsicReg_PON_MPP_46                               = 746,
	mvAsicReg_PON_MPP_47                               = 747,
	mvAsicReg_PON_MPP_48                               = 748,
	mvAsicReg_PON_MPP_49                               = 749,
	mvAsicReg_PON_MPP_50                               = 750,
	mvAsicReg_PON_MPP_51                               = 751,
	mvAsicReg_PON_MPP_52                               = 752,
	mvAsicReg_PON_MPP_53                               = 753,
	mvAsicReg_PON_MPP_54                               = 754,
	mvAsicReg_PON_MPP_55                               = 755,
	mvAsicReg_PON_MPP_56                               = 756,
	mvAsicReg_PON_MPP_57                               = 757,
	mvAsicReg_PON_MPP_58                               = 758,
	mvAsicReg_PON_MPP_59                               = 759,
	mvAsicReg_PON_MPP_60                               = 760,
	mvAsicReg_PON_MPP_61                               = 761,
	mvAsicReg_PON_MPP_62                               = 762,
	mvAsicReg_PON_MPP_63                               = 763,
	mvAsicReg_PON_MPP_64                               = 764,
	mvAsicReg_PON_MPP_65                               = 765,
	mvAsicReg_PON_MPP_66                               = 766,

	mvAsicReg_MAX_NUM_OF_REGS

} E_asicGlobalRegs;

/* Register access enumeration */
typedef enum {
	asicRO = 0x01,
	asicWO = 0x02,
	asicRW = 0x03
} E_asicAccessType;

typedef enum {
	funcRegR = 0x00,
	funcRegW = 0x01
} E_asicRegFuncType;

/* Typedefs
   ------------------------------------------------------------------------------*/
typedef struct {
	E_asicGlobalRegs enumVal;       /* The enumeration value of the Register */
	MV_U32 address;                 /* The absolute address of the Register */
	MV_U32 offset;                  /* The relative address of the Register */
	E_asicAccessType accessType;    /* Access Type: Read Only, Write Only, Read/Write */
	MV_U32 mask;                    /* Mask for sub-fields Register */
	MV_U32 shift;                   /* Shift field location for sub-fields Register */
	MV_U32 tblLength;               /* Number of entries in the table, Only relevant for tables */
	MV_U32 tblEntrySize;            /* The length in interval of 32bits of a table entry, Only relevant for tables */
	MV_U32 accessCount;             /* Number accesses to the specific Register */
	MV_U8 description[64];          /* Description */
} S_asicGlobalRegDb;

/* Global variables
   ------------------------------------------------------------------------------*/

/* Global functions
   ------------------------------------------------------------------------------*/
MV_STATUS asicOntGlbRegRead(E_asicGlobalRegs reg, MV_U32 *pvalue_p, MV_U32 entry);
MV_STATUS asicOntGlbRegWrite(E_asicGlobalRegs reg, MV_U32 value, MV_U32 entry);
MV_STATUS asicOntGlbRegReadNoCheck(E_asicGlobalRegs reg, MV_U32 *pvalue, MV_U32 entry);
MV_STATUS asicOntGlbRegWriteNoCheck(E_asicGlobalRegs reg, MV_U32 value, MV_U32 entry);
MV_STATUS asicOntMiscRegWrite(E_asicGlobalRegs reg, MV_U32 value, MV_U32 entry);
MV_STATUS asicOntMiscRegRead(E_asicGlobalRegs reg, MV_U32 *pvalue, MV_U32 entry);
MV_STATUS asicOntMiscRegAddressGet(E_asicGlobalRegs reg, MV_U32 entry, MV_U32 *regAddr);

/* Macros
   ------------------------------------------------------------------------------*/
#define PON_GPIO_NOT_USED (0xFFFF)
#define PON_GPIO_GET(gppClass, gpioGroup, gpioMask) \
	{ \
		MV_32 gpioNum; \
		gpioNum = mvBoarGpioPinNumGet(gppClass, 0); \
		if (gpioNum < 0) \
		{ \
			gpioGroup = PON_GPIO_NOT_USED; \
			gpioMask  = PON_GPIO_NOT_USED; \
		} else { \
			gpioGroup = gpioNum >> 5; \
			gpioMask  = (1 << (gpioNum & 0x1F)); \
		} \
	}

#endif /* _ONU_GPON_REG_H */
