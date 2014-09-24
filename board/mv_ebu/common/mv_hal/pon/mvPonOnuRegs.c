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

/* Include Files
   ------------------------------------------------------------------------------*/
#include "mvTypes.h"
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"

#include "mvPonOnuDefines.h"
#include "mvPonOnuRegs.h"
#include "mvPonOnuMac.h"

/* Local Constant
   ------------------------------------------------------------------------------*/
#define MV_ASIC_ONT_GLB_ADDR   (MV_ASIC_ONT_BASE)

/* Global Variables
   ------------------------------------------------------------------------------*/
MV_U32 ponOnuGlbAddr;

/* Local Variables
   ------------------------------------------------------------------------------*/

S_asicGlobalRegDb asicGlbDb[] =
{
/* Enumeration                                                                                          |Address                        |Offset   |Access     |Mask           |Shift  |Table |Table  |Cnt   |Description
 *                                                                                                      |                               |         |Type       |               |       |Len   |Entry  |      |
 *                                                                                                      |                               |         |           |               |       |      |Size   |      |
 * ============================================================================================================================================================================================================================= */
	[mvAsicReg_Start]                                 =  { mvAsicReg_Start                                                       },

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

	/* =========================== */
	/*  Interrupts Registers       */
	/* =========================== */
	[mvAsicReg_GPON_INTERRUPT_PON]                    =  { mvAsicReg_GPON_INTERRUPT_PON,                    MV_ASIC_ONT_GLB_ADDR + 0x0000,   0x0000,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Interrupt PON"},
	[mvAsicReg_GPON_INTERRUPT_PON_MASK]               =  { mvAsicReg_GPON_INTERRUPT_PON_MASK,               MV_ASIC_ONT_GLB_ADDR + 0x0004,   0x0004,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Interrupt PON Mask"},
	[mvAsicReg_BIP_INTR_INTERVAL]                     =  { mvAsicReg_BIP_INTR_INTERVAL,                     MV_ASIC_ONT_GLB_ADDR + 0x0008,   0x0008,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "BIP INTR interval"},
	[mvAsicReg_RX_BIP_STATUS_FOR_INTERRUPT]           =  { mvAsicReg_RX_BIP_STATUS_FOR_INTERRUPT,           MV_ASIC_ONT_GLB_ADDR + 0x000C,   0x000C,   asicRO,     0x0000FFFF,     0,      0,     0,      0,     "BIP INTR stats accumulator"},
	[mvAsicReg_RAM_TEST_CONFIG]                       =  { mvAsicReg_RAM_TEST_CONFIG,                       MV_ASIC_ONT_GLB_ADDR + 0x0010,   0x000C,   asicRW,     0x0000000F,     0,      0,     0,      0,     "Ram test config PE polarity"},

	/* =========================== */
	/*  General Registers          */
	/* =========================== */
	[mvAsicReg_GPON_GEN_MAC_VERSION]                  =  { mvAsicReg_GPON_GEN_MAC_VERSION,                  MV_ASIC_ONT_GLB_ADDR + 0x0400,   0x0400,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "GPON version"},
	[mvAsicReg_GPON_GEN_MAC_VERSION_ID]               =  { mvAsicReg_GPON_GEN_MAC_VERSION_ID,               MV_ASIC_ONT_GLB_ADDR + 0x0400,   0x0400,   asicRO,     0x0000FFFF,     0,      0,     0,      0,     "GPON MAC version"},
	[mvAsicReg_GPON_GEN_MAC_VERSION_MAC_ID]           =  { mvAsicReg_GPON_GEN_MAC_VERSION_MAC_ID,           MV_ASIC_ONT_GLB_ADDR + 0x0400,   0x0400,   asicRO,     0x0000FFFF,    16,      0,     0,      0,     "GPON MAC Id"},
	[mvAsicReg_GPON_GEN_MAC_SCRATCH]                  =  { mvAsicReg_GPON_GEN_MAC_SCRATCH,                  MV_ASIC_ONT_GLB_ADDR + 0x0404,   0x0404,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "MAC scratch register"},
	[mvAsicReg_GPON_ONU_STATE]                        =  { mvAsicReg_GPON_ONU_STATE,                        MV_ASIC_ONT_GLB_ADDR + 0x0408,   0x0408,   asicRW,     0x000000FF,     0,      0,     0,      0,     "ONU state"},
	[mvAsicReg_GPON_GEN_MICRO_SEC_CNT]                =  { mvAsicReg_GPON_GEN_MICRO_SEC_CNT,                MV_ASIC_ONT_GLB_ADDR + 0x040C,   0x040C,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Micro second counter"},
	[mvAsicReg_GPON_TX_SERIAL_NUMBER]                 =  { mvAsicReg_GPON_TX_SERIAL_NUMBER,                 MV_ASIC_ONT_GLB_ADDR + 0x0410,   0x0410,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Tx serial number"},
	[mvAsicReg_GPON_ONU_ID]                           =  { mvAsicReg_GPON_ONU_ID,                           MV_ASIC_ONT_GLB_ADDR + 0x0414,   0x0414,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "ONU Id"},
	[mvAsicReg_GPON_ONU_ID_OID]                       =  { mvAsicReg_GPON_ONU_ID_OID,                       MV_ASIC_ONT_GLB_ADDR + 0x0414,   0x0414,   asicRW,     0x000000FF,     0,      0,     0,      0,     "GPON ONU Id"},
	[mvAsicReg_GPON_ONU_ID_V]                         =  { mvAsicReg_GPON_ONU_ID_V,                         MV_ASIC_ONT_GLB_ADDR + 0x0414,   0x0414,   asicRW,     0x00000001,     8,      0,     0,      0,     "GPON ONU valid indication"},
	[mvAsicReg_GPON_GEN_TOD_INT_WIDTH]                =  { mvAsicReg_GPON_GEN_TOD_INT_WIDTH,                MV_ASIC_ONT_GLB_ADDR + 0x0418,   0x0418,   asicRW,     0x3FFFFFFF,     0,      0,     0,      0,     "GPON Interrupt Width"},
	[mvAsicReg_GPON_GEN_TOD_INT_SFC]                  =  { mvAsicReg_GPON_GEN_TOD_INT_SFC,                  MV_ASIC_ONT_GLB_ADDR + 0x041C,   0x041C,   asicRW,     0x3FFFFFFF,     0,      0,     0,      0,     "GPON SPF Counter for TOD INT"},

	/* =========================== */
	/*  Rx Registers               */
	/* =========================== */
	[mvAsicReg_GPON_RX_CONFIG]                        =  { mvAsicReg_GPON_RX_CONFIG,                        MV_ASIC_ONT_GLB_ADDR + 0x0800,   0x0800,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Rx config"},
	[mvAsicReg_GPON_RX_CONFIG_EN]                     =  { mvAsicReg_GPON_RX_CONFIG_EN,                     MV_ASIC_ONT_GLB_ADDR + 0x0800,   0x0800,   asicRW,     0x00000001,     0,      0,     0,      0,     "Rx config - enable"},
	[mvAsicReg_GPON_RX_CONFIG_BIT_ORDER]              =  { mvAsicReg_GPON_RX_CONFIG_BIT_ORDER,              MV_ASIC_ONT_GLB_ADDR + 0x0800,   0x0800,   asicRW,     0x00000001,     1,      0,     0,      0,     "Rx config - bit order"},
	[mvAsicReg_GPON_RX_PSA_CONFIG]                    =  { mvAsicReg_GPON_RX_PSA_CONFIG,                    MV_ASIC_ONT_GLB_ADDR + 0x0804,   0x0804,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Rx PSA config"},
	[mvAsicReg_GPON_RX_PSA_CONFIG_SFM3]               =  { mvAsicReg_GPON_RX_PSA_CONFIG_SFM3,               MV_ASIC_ONT_GLB_ADDR + 0x0804,   0x0804,   asicRW,     0x000000FF,     0,      0,     0,      0,     "PLOAM Sync SM: sync => hunt state"},
	[mvAsicReg_GPON_RX_PSA_CONFIG_SFM2]               =  { mvAsicReg_GPON_RX_PSA_CONFIG_SFM2,               MV_ASIC_ONT_GLB_ADDR + 0x0804,   0x0804,   asicRW,     0x000000FF,     8,      0,     0,      0,     "PLOAM Sync SM: sync => hunt state"},
	[mvAsicReg_GPON_RX_PSA_CONFIG_SFM1]               =  { mvAsicReg_GPON_RX_PSA_CONFIG_SFM1,               MV_ASIC_ONT_GLB_ADDR + 0x0804,   0x0804,   asicRW,     0x000000FF,     16,     0,     0,      0,     "PLOAM Sync SM: pre-sync => sync state"},
	[mvAsicReg_GPON_RX_PSA_CONFIG_FHM1]               =  { mvAsicReg_GPON_RX_PSA_CONFIG_FHM1,               MV_ASIC_ONT_GLB_ADDR + 0x0804,   0x0804,   asicRW,     0x0000000F,     24,     0,     0,      0,     "FEC hysteresis mechanisem"},
	[mvAsicReg_GPON_RX_FEC_CONFIG]                    =  { mvAsicReg_GPON_RX_FEC_CONFIG,                    MV_ASIC_ONT_GLB_ADDR + 0x0808,   0x0808,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Rx FEC config"},
	[mvAsicReg_GPON_RX_FEC_CONFIG_SWVAL]              =  { mvAsicReg_GPON_RX_FEC_CONFIG_SWVAL,              MV_ASIC_ONT_GLB_ADDR + 0x0808,   0x0808,   asicRW,     0x00000001,     0,      0,     0,      0,     "SW value for internal FEC indication"},
	[mvAsicReg_GPON_RX_FEC_CONFIG_FSW]                =  { mvAsicReg_GPON_RX_FEC_CONFIG_FSW,                MV_ASIC_ONT_GLB_ADDR + 0x0808,   0x0808,   asicRW,     0x00000001,     1,      0,     0,      0,     "Force SW FEC indication"},
	[mvAsicReg_GPON_RX_FEC_CONFIG_IGNP]               =  { mvAsicReg_GPON_RX_FEC_CONFIG_IGNP,               MV_ASIC_ONT_GLB_ADDR + 0x0808,   0x0808,   asicRW,     0x00000001,     2,      0,     0,      0,     "Force SW FEC ignore parity"},
	[mvAsicReg_GPON_RX_FEC_CONFIG_IND]                =  { mvAsicReg_GPON_RX_FEC_CONFIG_IND,                MV_ASIC_ONT_GLB_ADDR + 0x0808,   0x0808,   asicRO,     0x00000001,     31,     0,     0,      0,     "Internal FEC indication"},
	[mvAsicReg_GPON_RX_FEC_STAT0]                     =  { mvAsicReg_GPON_RX_FEC_STAT0,                     MV_ASIC_ONT_GLB_ADDR + 0x080C,   0x080C,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Num of bytes rece from last read"},
	[mvAsicReg_GPON_RX_FEC_STAT1]                     =  { mvAsicReg_GPON_RX_FEC_STAT1,                     MV_ASIC_ONT_GLB_ADDR + 0x0810,   0x0810,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Num of correct bytes / bits"},
	[mvAsicReg_GPON_RX_FEC_STAT2]                     =  { mvAsicReg_GPON_RX_FEC_STAT2,                     MV_ASIC_ONT_GLB_ADDR + 0x0814,   0x0814,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Num of incorrect / rece bytes"},
	[mvAsicReg_GPON_RX_SFRAME_COUNTER]                =  { mvAsicReg_GPON_RX_SFRAME_COUNTER,                MV_ASIC_ONT_GLB_ADDR + 0x0818,   0x0818,   asicRO,     0x3FFFFFFF,     0,      0,     0,      0,     "Rx Superframe Counter"},
	[mvAsicReg_GPON_RX_PLOAMD_DATA_READ]              =  { mvAsicReg_GPON_RX_PLOAMD_DATA_READ,              MV_ASIC_ONT_GLB_ADDR + 0x081C,   0x081C,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Rx PLOAM data fifo, PLOAM = 3xread"},
	[mvAsicReg_GPON_RX_PLOAMD_DATA_USED]              =  { mvAsicReg_GPON_RX_PLOAMD_DATA_USED,              MV_ASIC_ONT_GLB_ADDR + 0x0820,   0x0820,   asicRO,     0x000000FF,     0,      0,     0,      0,     "Num of free entry's in PLOAM data fifo"},
	[mvAsicReg_GPON_RX_PLOAMD_CONFIG]                 =  { mvAsicReg_GPON_RX_PLOAMD_CONFIG,                 MV_ASIC_ONT_GLB_ADDR + 0x0824,   0x0824,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Rx PLOAM config"},
	[mvAsicReg_GPON_RX_PLOAMD_CONFIG_IGNC]            =  { mvAsicReg_GPON_RX_PLOAMD_CONFIG_IGNC,            MV_ASIC_ONT_GLB_ADDR + 0x0824,   0x0824,   asicRW,     0x00000001,     0,      0,     0,      0,     "Ignore PLOAM crc, forward regardless of crc"},
	[mvAsicReg_GPON_RX_PLOAMD_CONFIG_IGNB]            =  { mvAsicReg_GPON_RX_PLOAMD_CONFIG_IGNB,            MV_ASIC_ONT_GLB_ADDR + 0x0824,   0x0824,   asicRW,     0x00000001,     1,      0,     0,      0,     "Ignore broadcast PLOAM, forward only onuId"},
	[mvAsicReg_GPON_RX_PLOAMD_CONFIG_FALL]            =  { mvAsicReg_GPON_RX_PLOAMD_CONFIG_FALL,            MV_ASIC_ONT_GLB_ADDR + 0x0824,   0x0824,   asicRW,     0x00000001,     2,      0,     0,      0,     "Forward all PLOAM"},
	[mvAsicReg_GPON_RX_PLOAMD_LOST_CRC_STATUS]        =  { mvAsicReg_GPON_RX_PLOAMD_LOST_CRC_STATUS,        MV_ASIC_ONT_GLB_ADDR + 0x0828,   0x0828,   asicRO,     0x0000FFFF,     0,      0,     0,      0,     "Num of PLOAM lost due to faulty crc"},
	[mvAsicReg_GPON_RX_PLOAMD_LOST_FULL_STATUS]       =  { mvAsicReg_GPON_RX_PLOAMD_LOST_FULL_STATUS,       MV_ASIC_ONT_GLB_ADDR + 0x082C,   0x082C,   asicRO,     0x0000FFFF,     0,      0,     0,      0,     "Num of PLOAM lost due to cpu fifo full"},
	[mvAsicReg_GPON_RX_PLOAMD_RCVD_IDLE_STATUS]       =  { mvAsicReg_GPON_RX_PLOAMD_RCVD_IDLE_STATUS,       MV_ASIC_ONT_GLB_ADDR + 0x0830,   0x0830,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Num of idle PLOAM rece"},
	[mvAsicReg_GPON_RX_PLOAMD_RCVD_BROAD_STATUS]      =  { mvAsicReg_GPON_RX_PLOAMD_RCVD_BROAD_STATUS,      MV_ASIC_ONT_GLB_ADDR + 0x0834,   0x0834,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Num of broadcast PLOAM rece"},
	[mvAsicReg_GPON_RX_PLOAMD_RCVD_MYID_STATUS]       =  { mvAsicReg_GPON_RX_PLOAMD_RCVD_MYID_STATUS,       MV_ASIC_ONT_GLB_ADDR + 0x0838,   0x0838,   asicRO,     0x0000FFFF,     0,      0,     0,      0,     "Num of PLOAM rece assigned to MYID"},
	[mvAsicReg_GPON_RX_BIP_STATUS]                    =  { mvAsicReg_GPON_RX_BIP_STATUS,                    MV_ASIC_ONT_GLB_ADDR + 0x083C,   0x083C,   asicRO,     0x0000FFFF,     0,      0,     0,      0,     "Num of BIP errors since last read"},
	[mvAsicReg_GPON_RX_PLEND_CONFIG]                  =  { mvAsicReg_GPON_RX_PLEND_CONFIG,                  MV_ASIC_ONT_GLB_ADDR + 0x0840,   0x0840,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Rx PLEND config"},
	[mvAsicReg_GPON_RX_PLEND_CONFIG_FN]               =  { mvAsicReg_GPON_RX_PLEND_CONFIG_FN,               MV_ASIC_ONT_GLB_ADDR + 0x0840,   0x0840,   asicRW,     0x00000001,     0,      0,     0,      0,     "Force SW plend field selection"},
	[mvAsicReg_GPON_RX_PLEND_CONFIG_UF]               =  { mvAsicReg_GPON_RX_PLEND_CONFIG_UF,               MV_ASIC_ONT_GLB_ADDR + 0x0840,   0x0840,   asicRW,     0x00000001,     1,      0,     0,      0,     "SW use of plend field"},
	[mvAsicReg_GPON_RX_PLEND_CONFIG_IGND]             =  { mvAsicReg_GPON_RX_PLEND_CONFIG_IGND,             MV_ASIC_ONT_GLB_ADDR + 0x0840,   0x0840,   asicRW,     0x00000001,     2,      0,     0,      0,     "Ignore diff between two plend fields"},
	[mvAsicReg_GPON_RX_PLEND_CONFIG_DFIX]             =  { mvAsicReg_GPON_RX_PLEND_CONFIG_DFIX,             MV_ASIC_ONT_GLB_ADDR + 0x0840,   0x0840,   asicRW,     0x00000001,     3,      0,     0,      0,     "Do not fix plend field according to crc result"},
	[mvAsicReg_GPON_RX_PLEND_CONFIG_IGNC]             =  { mvAsicReg_GPON_RX_PLEND_CONFIG_IGNC,             MV_ASIC_ONT_GLB_ADDR + 0x0840,   0x0840,   asicRW,     0x00000001,     4,      0,     0,      0,     "Ignore crc during plend field selection"},
	[mvAsicReg_GPON_RX_PLEND_STATUS]                  =  { mvAsicReg_GPON_RX_PLEND_STATUS,                  MV_ASIC_ONT_GLB_ADDR + 0x0844,   0x0844,   asicRO,     0x0000FFFF,     0,      0,     0,      0,     "Rx PLEND status"},
	[mvAsicReg_GPON_RX_BWMAP_CONFIG]                  =  { mvAsicReg_GPON_RX_BWMAP_CONFIG,                  MV_ASIC_ONT_GLB_ADDR + 0x0848,   0x0848,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Rx BW map config"},
	[mvAsicReg_GPON_RX_BWMAP_CONFIG_MSD]              =  { mvAsicReg_GPON_RX_BWMAP_CONFIG_MSD,              MV_ASIC_ONT_GLB_ADDR + 0x0848,   0x0848,   asicRW,     0x00000FFF,     0,      0,     0,      0,     "Min num of bytes between bursts"},
	[mvAsicReg_GPON_RX_BWMAP_CONFIG_DFIX]             =  { mvAsicReg_GPON_RX_BWMAP_CONFIG_DFIX,             MV_ASIC_ONT_GLB_ADDR + 0x0848,   0x0848,   asicRW,     0x00000001,    12,      0,     0,      0,     "Do not fix BW map according to crc result"},
	[mvAsicReg_GPON_RX_BWMAP_CONFIG_IGNC]             =  { mvAsicReg_GPON_RX_BWMAP_CONFIG_IGNC,             MV_ASIC_ONT_GLB_ADDR + 0x0848,   0x0848,   asicRW,     0x00000001,    13,      0,     0,      0,     "Ignore crc result for BW allocation"},
	[mvAsicReg_GPON_RX_BWMAP_STATUS0]                 =  { mvAsicReg_GPON_RX_BWMAP_STATUS0,                 MV_ASIC_ONT_GLB_ADDR + 0x084C,   0x084C,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Num of BW bytes rece with crc: bad/error/corr/uncorr"},
	[mvAsicReg_GPON_RX_BWMAP_STATUS1]                 =  { mvAsicReg_GPON_RX_BWMAP_STATUS1,                 MV_ASIC_ONT_GLB_ADDR + 0x0850,   0x0850,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Num of BW bytes rece from last read"},
	[mvAsicReg_GPON_RX_AES_CONFIG]                    =  { mvAsicReg_GPON_RX_AES_CONFIG,                    MV_ASIC_ONT_GLB_ADDR + 0x0854,   0x0854,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Rx AES key switch config"},
	[mvAsicReg_GPON_RX_AES_CONFIG_TIME]               =  { mvAsicReg_GPON_RX_AES_CONFIG_TIME,               MV_ASIC_ONT_GLB_ADDR + 0x0854,   0x0854,   asicRW,     0x3FFFFFFF,     0,      0,     0,      0,     "AES Key switch time value based on superframe counter"},
	[mvAsicReg_GPON_RX_AES_CONFIG_SWS]                =  { mvAsicReg_GPON_RX_AES_CONFIG_SWS,                MV_ASIC_ONT_GLB_ADDR + 0x0854,   0x0854,   asicRW,     0x00000001,    30,      0,     0,      0,     "AES SW Force key switch according to superframe counter"},
	[mvAsicReg_GPON_RX_AES_STATUS]                    =  { mvAsicReg_GPON_RX_AES_STATUS,                    MV_ASIC_ONT_GLB_ADDR + 0x0858,   0x0858,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Rx AES status"},
	[mvAsicReg_GPON_RX_AES_STATUS_HST]                =  { mvAsicReg_GPON_RX_AES_STATUS_HST,                MV_ASIC_ONT_GLB_ADDR + 0x0858,   0x0858,   asicRO,     0x3FFFFFFF,     0,      0,     0,      0,     "Indicates the time to switch AES Key"},
	[mvAsicReg_GPON_RX_AES_STATUS_HWS]                =  { mvAsicReg_GPON_RX_AES_STATUS_HWS,                MV_ASIC_ONT_GLB_ADDR + 0x0858,   0x0858,   asicRO,     0x00000001,    30,      0,     0,      0,     "indicates AES key switch time is valid"},
	[mvAsicReg_GPON_RX_EQULIZATION_DELAY]             =  { mvAsicReg_GPON_RX_EQULIZATION_DELAY,             MV_ASIC_ONT_GLB_ADDR + 0x085C,   0x085C,   asicRW,     0x000FFFFF,     0,      0,     0,      0,     "Rx equalization delay, sent from the OLT"},
	[mvAsicReg_GPON_RX_INTERNAL_DELAY]                =  { mvAsicReg_GPON_RX_INTERNAL_DELAY,                MV_ASIC_ONT_GLB_ADDR + 0x0860,   0x0860,   asicRW,     0x00001FFF,     0,      0,     0,      0,     "Rx Internal Delay - Constant"},
	[mvAsicReg_GPON_RX_BW_MAP]                        =  { mvAsicReg_GPON_RX_BW_MAP,                        MV_ASIC_ONT_GLB_ADDR + 0x0880,   0x0880,   asicRW,     0xFFFFFFFF,     0,     16,     1,      0,     "Rx BW map"},
	[mvAsicReg_GPON_RX_BW_MAP_ALID]                   =  { mvAsicReg_GPON_RX_BW_MAP_ALID,                   MV_ASIC_ONT_GLB_ADDR + 0x0880,   0x0880,   asicRW,     0x00000FFF,     0,     16,     1,      0,     "Alloc Id number to be mapped to tcont number"},
	[mvAsicReg_GPON_RX_BW_MAP_TCN]                    =  { mvAsicReg_GPON_RX_BW_MAP_TCN,                    MV_ASIC_ONT_GLB_ADDR + 0x0880,   0x0880,   asicRW,     0x0000001F,    12,     16,     1,      0,     "Tcont number to which the allocId will be mapped"},
	[mvAsicReg_GPON_RX_BW_MAP_EN]                     =  { mvAsicReg_GPON_RX_BW_MAP_EN,                     MV_ASIC_ONT_GLB_ADDR + 0x0880,   0x0880,   asicRW,     0x00000001,    17,     16,     1,      0,     "Rx BW map mapping fields are valid"},

	/* =========================== */
	/*  GEM Registers              */
	/* =========================== */
	[mvAsicReg_GPON_GEM_STAT_IDLE_GEM_CNT]            =  { mvAsicReg_GPON_GEM_STAT_IDLE_GEM_CNT,            MV_ASIC_ONT_GLB_ADDR + 0x0C00,   0x0C00,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece idle gem frames"},
	[mvAsicReg_GPON_GEM_STAT_VALID_GEM_CNT]           =  { mvAsicReg_GPON_GEM_STAT_VALID_GEM_CNT,           MV_ASIC_ONT_GLB_ADDR + 0x0C04,   0x0C04,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece valid gem frames"},
	[mvAsicReg_GPON_GEM_STAT_UNDEF_GEM_CNT]           =  { mvAsicReg_GPON_GEM_STAT_UNDEF_GEM_CNT,           MV_ASIC_ONT_GLB_ADDR + 0x0C08,   0x0C08,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece undefined gem frames"},
	[mvAsicReg_GPON_GEM_STAT_VALID_OMCI_CNT]          =  { mvAsicReg_GPON_GEM_STAT_VALID_OMCI_CNT,          MV_ASIC_ONT_GLB_ADDR + 0x0C0C,   0x0C0C,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece omci frames"},
	[mvAsicReg_GPON_GEM_STAT_DROPPED_GEM_CNT]         =  { mvAsicReg_GPON_GEM_STAT_DROPPED_GEM_CNT,         MV_ASIC_ONT_GLB_ADDR + 0x0C10,   0x0C10,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of dropped gem frames"},
	[mvAsicReg_GPON_GEM_STAT_DROPPED_OMCI_CNT]        =  { mvAsicReg_GPON_GEM_STAT_DROPPED_OMCI_CNT,        MV_ASIC_ONT_GLB_ADDR + 0x0C14,   0x0C14,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of dropped omci frames"},
	[mvAsicReg_GPON_GEM_STAT_ERROR_GEM_CNT]           =  { mvAsicReg_GPON_GEM_STAT_ERROR_GEM_CNT,           MV_ASIC_ONT_GLB_ADDR + 0x0C18,   0x0C18,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece gem frames with uncorr HEC errors"},
	[mvAsicReg_GPON_GEM_STAT_ONE_FIXED_HEC_ERR_CNT]   =  { mvAsicReg_GPON_GEM_STAT_ONE_FIXED_HEC_ERR_CNT,   MV_ASIC_ONT_GLB_ADDR + 0x0C1C,   0x0C1C,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece gem frames with 1 HEC error in gem header"},
	[mvAsicReg_GPON_GEM_STAT_TWO_FIXED_HEC_ERR_CNT]   =  { mvAsicReg_GPON_GEM_STAT_TWO_FIXED_HEC_ERR_CNT,   MV_ASIC_ONT_GLB_ADDR + 0x0C20,   0x0C20,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece gem frames with 2 HEC error in gem header"},
	[mvAsicReg_GPON_GEM_STAT_VALID_GEM_PAYLOAD_CNT]   =  { mvAsicReg_GPON_GEM_STAT_VALID_GEM_PAYLOAD_CNT,   MV_ASIC_ONT_GLB_ADDR + 0x0C24,   0x0C24,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Total payload byte count of rece valid gem frames"},
	[mvAsicReg_GPON_GEM_STAT_UNDEF_GEM_PAYLOAD_CNT]   =  { mvAsicReg_GPON_GEM_STAT_UNDEF_GEM_PAYLOAD_CNT,   MV_ASIC_ONT_GLB_ADDR + 0x0C28,   0x0C28,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Total payload byte count of rece undefined gem frames"},
	[mvAsicReg_GPON_GEM_STAT_RSMBL_MEM_FLUSH_CNT]     =  { mvAsicReg_GPON_GEM_STAT_RSMBL_MEM_FLUSH_CNT,     MV_ASIC_ONT_GLB_ADDR + 0x0C2C,   0x0C2C,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of times gem reassemble memory was flushed"},
	[mvAsicReg_GPON_GEM_STAT_LCDGi_CNT]               =  { mvAsicReg_GPON_GEM_STAT_LCDGi_CNT,               MV_ASIC_ONT_GLB_ADDR + 0x0C30,   0x0C30,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of times gem synchronization was lost"},
	[mvAsicReg_GPON_GEM_STAT_CORR_ETH_FRAME_CNT]      =  { mvAsicReg_GPON_GEM_STAT_CORR_ETH_FRAME_CNT,      MV_ASIC_ONT_GLB_ADDR + 0x0C34,   0x0C34,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece ethernet frames with corr fcs"},
	[mvAsicReg_GPON_GEM_STAT_ERR_ETH_FRAME_CNT]       =  { mvAsicReg_GPON_GEM_STAT_ERR_ETH_FRAME_CNT,       MV_ASIC_ONT_GLB_ADDR + 0x0C38,   0x0C38,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece ethernet frames with fcs error"},
	[mvAsicReg_GPON_GEM_STAT_CORR_OMCI_FRAME_CNT]     =  { mvAsicReg_GPON_GEM_STAT_CORR_OMCI_FRAME_CNT,     MV_ASIC_ONT_GLB_ADDR + 0x0C3C,   0x0C3C,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece omci frames with corr crc"},
	[mvAsicReg_GPON_GEM_STAT_ERR_OMCI_FRAME_CNT]      =  { mvAsicReg_GPON_GEM_STAT_ERR_OMCI_FRAME_CNT,      MV_ASIC_ONT_GLB_ADDR + 0x0C40,   0x0C40,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Num of rece omci frames with crc error"},
	[mvAsicReg_GPON_GEM_CONFIG_PLI]                   =  { mvAsicReg_GPON_GEM_CONFIG_PLI,                   MV_ASIC_ONT_GLB_ADDR + 0x0C44,   0x0C44,   asicRW,     0x00000FFF,      0,     0,     0,      0,     "Gem frames max payload length"},
	[mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN]         =  { mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN,         MV_ASIC_ONT_GLB_ADDR + 0x0C48,   0x0C48,   asicRW,     0xFFFFFFFF,      0,     0,     0,      0,     "Gem ethernet frames length config"},
	[mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN_MAX]     =  { mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN_MAX,     MV_ASIC_ONT_GLB_ADDR + 0x0C48,   0x0C48,   asicRW,     0x00000FFF,      0,     0,     0,      0,     "Gem ethernet frames max length"},
	[mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN_MIN]     =  { mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN_MIN,     MV_ASIC_ONT_GLB_ADDR + 0x0C48,   0x0C48,   asicRW,     0x00000FFF,     16,     0,     0,      0,     "Gem ethernet frames min length"},
	[mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN]        =  { mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN,        MV_ASIC_ONT_GLB_ADDR + 0x0C4C,   0x0C4C,   asicRW,     0xFFFFFFFF,      0,     0,     0,      0,     "Gem omci frames length config"},
	[mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN_MAX]    =  { mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN_MAX,    MV_ASIC_ONT_GLB_ADDR + 0x0C4C,   0x0C4C,   asicRW,     0x00000FFF,      0,     0,     0,      0,     "Gem omci frames max length"},
	[mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN_MIN]    =  { mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN_MIN,    MV_ASIC_ONT_GLB_ADDR + 0x0C4C,   0x0C4C,   asicRW,     0x00000FFF,     16,     0,     0,      0,     "Gem omci frames min length"},
	[mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT]             =  { mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT,             MV_ASIC_ONT_GLB_ADDR + 0x0C50,   0x0C50,   asicRW,     0xFFFFFFFF,      0,     0,     0,      0,     "Omci gem port Id config"},
	[mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT_ID]          =  { mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT_ID,          MV_ASIC_ONT_GLB_ADDR + 0x0C50,   0x0C50,   asicRW,     0x00000FFF,      0,     0,     0,      0,     "Omci gem port Id value"},
	[mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT_VALID]       =  { mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT_VALID,       MV_ASIC_ONT_GLB_ADDR + 0x0C50,   0x0C50,   asicRW,     0x00000001,     16,     0,     0,      0,     "Omci gem port Id valid indication"},
	[mvAsicReg_GPON_GEM_CONFIG_OMCI_FAST_PATH_VALID]  =  { mvAsicReg_GPON_GEM_CONFIG_OMCI_FAST_PATH_VALID,  MV_ASIC_ONT_GLB_ADDR + 0x0C50,   0x0C50,   asicRW,     0x00000001,     31,     0,     0,      0,     "D/S OMCI fast path valid indication"},
	[mvAsicReg_GPON_GEM_CONFIG_AES]                   =  { mvAsicReg_GPON_GEM_CONFIG_AES,                   MV_ASIC_ONT_GLB_ADDR + 0x0C54,   0x0C54,   asicRW,     0x00000001,      0,     0,     0,      0,     "Gem AES enable"},
	[mvAsicReg_GPON_GEM_AES_SHADOW_KEY]               =  { mvAsicReg_GPON_GEM_AES_SHADOW_KEY,               MV_ASIC_ONT_GLB_ADDR + 0x0C58,   0x0C58,   asicRW,     0xFFFFFFFF,      0,     4,     1,      0,     "Rx AES shadow key"},
	[mvAsicReg_GPON_GEM_CFG_IPG_VALUE]                =  { mvAsicReg_GPON_GEM_CFG_IPG_VALUE,                MV_ASIC_ONT_GLB_ADDR + 0x0C68,   0x0C68,   asicRW,     0x000000FF,      0,     0,     0,      0,     "Gem Cfg IPG Value"},
	[mvAsicReg_GPON_GEM_CFG_IPG_VALID]                =  { mvAsicReg_GPON_GEM_CFG_IPG_VALID,                MV_ASIC_ONT_GLB_ADDR + 0x0C6C,   0x0C6C,   asicRW,     0x00000001,      0,     0,     0,      0,     "Gem Cfg IPG Valid"},
	[mvAsicReg_GPON_GEM_RX_CTRL_Q_READ]               =  { mvAsicReg_GPON_GEM_RX_CTRL_Q_READ,               MV_ASIC_ONT_GLB_ADDR + 0x0C74,   0x0C74,   asicRO,     0xFFFFFFFF,      0,     0,     0,      0,     "Rx control cmd and data fifo"},
	[mvAsicReg_GPON_GEM_RX_CTRL_DATA_USED]            =  { mvAsicReg_GPON_GEM_RX_CTRL_DATA_USED,            MV_ASIC_ONT_GLB_ADDR + 0x0C78,   0x0C78,   asicRO,     0x000003FF,      0,     0,     0,      0,     "Control data fifo used"},
	[mvAsicReg_GPON_GEM_RX_CTRL_CMD_USED]             =  { mvAsicReg_GPON_GEM_RX_CTRL_CMD_USED,             MV_ASIC_ONT_GLB_ADDR + 0x0C78,   0x0C78,   asicRO,     0x000000FF,     16,     0,     0,      0,     "Control cmd fifo used"},
	[mvAsicReg_GPON_GEM_RX_CTRL_HQ_HEAD_LEN]          =  { mvAsicReg_GPON_GEM_RX_CTRL_HQ_HEAD_LEN,          MV_ASIC_ONT_GLB_ADDR + 0x0C7C,   0x0C7C,   asicRO,     0x00000FFF,      0,     0,     0,      0,     "Rx control cmd and data header length"},
	[mvAsicReg_GPON_GEM_RX_CTRL_HQ_GEM_PORT_ID]       =  { mvAsicReg_GPON_GEM_RX_CTRL_HQ_GEM_PORT_ID,       MV_ASIC_ONT_GLB_ADDR + 0x0C7C,   0x0C7C,   asicRO,     0x00000FFF,     12,     0,     0,      0,     "Rx control cmd and data GEM port ID"},
	[mvAsicReg_GPON_GEM_RX_CTRL_HQ_GEM_PTI]           =  { mvAsicReg_GPON_GEM_RX_CTRL_HQ_GEM_PTI,           MV_ASIC_ONT_GLB_ADDR + 0x0C7C,   0x0C7C,   asicRO,     0x00000007,     24,     0,     0,      0,     "Rx control cmd and data GEM PTI"},
	[mvAsicReg_GPON_GEM_RX_CTRL_HQ_GEM_ERR]           =  { mvAsicReg_GPON_GEM_RX_CTRL_HQ_GEM_ERR,           MV_ASIC_ONT_GLB_ADDR + 0x0C7C,   0x0C7C,   asicRO,     0x00000001,     27,     0,     0,      0,     "Rx control cmd and data GEM error"},

	/* =========================== */
	/*  Tx Registers               */
	/* =========================== */
	[mvAsicReg_GPON_TX_PREAMBLE]                      =  { mvAsicReg_GPON_TX_PREAMBLE,                      MV_ASIC_ONT_GLB_ADDR + 0x1000,   0x1000,   asicRW,     0xFFFFFFFF,     0,      3,     1,      0,     "Tx preamble config"},
	[mvAsicReg_GPON_TX_PREAMBLE_PATT]                 =  { mvAsicReg_GPON_TX_PREAMBLE_PATT,                 MV_ASIC_ONT_GLB_ADDR + 0x1000,   0x1000,   asicRW,     0x000000FF,     0,      3,     1,      0,     "Tx preamble type 1/2/3 pattern"},
	[mvAsicReg_GPON_TX_PREAMBLE_SIZE]                 =  { mvAsicReg_GPON_TX_PREAMBLE_SIZE,                 MV_ASIC_ONT_GLB_ADDR + 0x1000,   0x1000,   asicRW,     0x000000FF,     8,      3,     1,      0,     "Tx preamble type 1/2/3 size"},
	[mvAsicReg_GPON_TX_DELIMITER]                     =  { mvAsicReg_GPON_TX_DELIMITER,                     MV_ASIC_ONT_GLB_ADDR + 0x100C,   0x100C,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Tx delimiter config"},
	[mvAsicReg_GPON_TX_DELIMITER_PAT]                 =  { mvAsicReg_GPON_TX_DELIMITER_PAT,                 MV_ASIC_ONT_GLB_ADDR + 0x100C,   0x100C,   asicRW,     0x00FFFFFF,     0,      0,     0,      0,     "Tx delimiter byte 1/2/3 pattern"},
	[mvAsicReg_GPON_TX_DELIMITER_SIZE]                =  { mvAsicReg_GPON_TX_DELIMITER_SIZE,                MV_ASIC_ONT_GLB_ADDR + 0x100C,   0x100C,   asicRW,     0x000000FF,     24,     0,     0,      0,     "Tx delimiter size"},
	[mvAsicReg_GPON_TX_BURST_EN_PARAM]                =  { mvAsicReg_GPON_TX_BURST_EN_PARAM,                MV_ASIC_ONT_GLB_ADDR + 0x1010,   0x1010,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Tx burst enable config"},
	[mvAsicReg_GPON_TX_BURST_EN_PARAM_STR]            =  { mvAsicReg_GPON_TX_BURST_EN_PARAM_STR,            MV_ASIC_ONT_GLB_ADDR + 0x1010,   0x1010,   asicRW,     0x0000001F,     0,      0,     0,      0,     "Num of tx enable bytes before first US data bit is trans"},
	[mvAsicReg_GPON_TX_BURST_EN_PARAM_STP]            =  { mvAsicReg_GPON_TX_BURST_EN_PARAM_STP,            MV_ASIC_ONT_GLB_ADDR + 0x1010,   0x1010,   asicRW,     0x0000001F,     8,      0,     0,      0,     "Num of tx enable bytes after last US data bit is trans"},
	[mvAsicReg_GPON_TX_BURST_EN_PARAM_DLY]            =  { mvAsicReg_GPON_TX_BURST_EN_PARAM_DLY,            MV_ASIC_ONT_GLB_ADDR + 0x1010,   0x1010,   asicRW,     0x00000001,     14,     0,     0,      0,     "Swap the order of tx data and tx enable bytes"},
	[mvAsicReg_GPON_TX_BURST_EN_PARAM_P]              =  { mvAsicReg_GPON_TX_BURST_EN_PARAM_P,              MV_ASIC_ONT_GLB_ADDR + 0x1010,   0x1010,   asicRW,     0x00000001,     15,     0,     0,      0,     "Tx Burst enable polarity"},
	[mvAsicReg_GPON_TX_BURST_EN_PARAM_MASK]           =  { mvAsicReg_GPON_TX_BURST_EN_PARAM_MASK,           MV_ASIC_ONT_GLB_ADDR + 0x1010,   0x1010,   asicRW,     0x000000FF,     16,     0,     0,      0,     "Mask for 8 bits that are added to the end of tx enable signal"},
	[mvAsicReg_GPON_TX_FDELAY]                        =  { mvAsicReg_GPON_TX_FDELAY,                        MV_ASIC_ONT_GLB_ADDR + 0x1014,   0x1014,   asicRW,     0x0000003F,     0,      0,     0,      0,     "Tx final delay"},
	[mvAsicReg_GPON_TX_PLOAM_DATA_WRITE]              =  { mvAsicReg_GPON_TX_PLOAM_DATA_WRITE,              MV_ASIC_ONT_GLB_ADDR + 0x1018,   0x1018,   asicWO,     0xFFFFFFFF,     0,      0,     0,      0,     "Tx PLOAM data fifo"},
	[mvAsicReg_GPON_TX_PLOAM_DATA_WRITE_FREE]         =  { mvAsicReg_GPON_TX_PLOAM_DATA_WRITE_FREE,         MV_ASIC_ONT_GLB_ADDR + 0x101C,   0x101C,   asicRO,     0x000000FF,     0,      0,     0,      0,     "Num of free entry's in Us PLOAM data fifo"},
	[mvAsicReg_GPON_TX_PLS_CONSTANT]                  =  { mvAsicReg_GPON_TX_PLS_CONSTANT,                  MV_ASIC_ONT_GLB_ADDR + 0x1020,   0x1020,   asicRW,     0x000000FF,     0,      0,     0,      0,     "Tx PLS Constant"},
	[mvAsicReg_GPON_TX_DBR_REPORT_BLOCK_IDX]          =  { mvAsicReg_GPON_TX_DBR_REPORT_BLOCK_IDX,          MV_ASIC_ONT_GLB_ADDR + 0x1024,   0x1024,   asicRW,     0x000000FF,     0,      0,     0,      0,     "Tx dbr block index"},
	[mvAsicReg_GPON_TX_FE_RDI_INDICATION]             =  { mvAsicReg_GPON_TX_FE_RDI_INDICATION,             MV_ASIC_ONT_GLB_ADDR + 0x1028,   0x1028,   asicRW,     0x00000001,     0,      0,     0,      0,     "Tx rdi indication for US trans"},
	[mvAsicReg_GPON_TX_STAT_GEM_PTI1]                 =  { mvAsicReg_GPON_TX_STAT_GEM_PTI1,                 MV_ASIC_ONT_GLB_ADDR + 0x102C,   0x102C,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Total num of trans gem frames with pti indication set to 1"},
	[mvAsicReg_GPON_TX_STAT_GEM_PTI0]                 =  { mvAsicReg_GPON_TX_STAT_GEM_PTI0,                 MV_ASIC_ONT_GLB_ADDR + 0x1030,   0x1030,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Total num of trans gem frames with pti indication set to 0"},
	[mvAsicReg_GPON_TX_STAT_GEM_IDLE]                 =  { mvAsicReg_GPON_TX_STAT_GEM_IDLE,                 MV_ASIC_ONT_GLB_ADDR + 0x1034,   0x1034,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Total num of trans idle gem frames"},
	[mvAsicReg_GPON_TX_STAT_TX_EN_CNT]                =  { mvAsicReg_GPON_TX_STAT_TX_EN_CNT,                MV_ASIC_ONT_GLB_ADDR + 0x1038,   0x1038,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "Total num of cycles with tx enable indication asserted"},
	[mvAsicReg_GPON_TX_CONFIG_EN_THRESHOLD]           =  { mvAsicReg_GPON_TX_CONFIG_EN_THRESHOLD,           MV_ASIC_ONT_GLB_ADDR + 0x103C,   0x103C,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Saturation threshold for tx enable indication counter"},
	[mvAsicReg_GPON_TX_GSE_TRANS_THRESHOLD]           =  { mvAsicReg_GPON_TX_GSE_TRANS_THRESHOLD,           MV_ASIC_ONT_GLB_ADDR + 0x1040,   0x1040,   asicRW,     0x00FFFFFF,     0,      0,     0,      0,     "GSE threshold for starting forwarding data to the tx burst fifo"},
	[mvAsicReg_GPON_TX_CFG_AC_COUPLING]               =  { mvAsicReg_GPON_TX_CFG_AC_COUPLING,               MV_ASIC_ONT_GLB_ADDR + 0x104C,   0x104C,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Transmit Configuration AC Coupling"},
	[mvAsicReg_GPON_TX_CFG_RAND_DELAY]                =  { mvAsicReg_GPON_TX_CFG_RAND_DELAY,                MV_ASIC_ONT_GLB_ADDR + 0x1050,   0x1050,   asicRW,     0x00000007,     0,      0,     0,      0,     "Transmit Configuration Random Delay"},
	[mvAsicReg_GPON_TX_STAT_TCONT_i_ETH_FRAMES]       =  { mvAsicReg_GPON_TX_STAT_TCONT_i_ETH_FRAMES,       MV_ASIC_ONT_GLB_ADDR + 0x10A0,   0x10A0,   asicRO,     0xFFFFFFFF,     0,      8,     1,      0,     "Num of ethernet frames trans via tcont i (0-7)"},
	[mvAsicReg_GPON_TX_STAT_TCONT_i_ETH_BYTES]        =  { mvAsicReg_GPON_TX_STAT_TCONT_i_ETH_BYTES,        MV_ASIC_ONT_GLB_ADDR + 0x10C8,   0x10C8,   asicRO,     0xFFFFFFFF,     0,      8,     1,      0,     "Num of ethernet payload bytes trans via tcont i (0-7)"},
	[mvAsicReg_GPON_TX_STAT_TCONT_i_GEM_FRAMES]       =  { mvAsicReg_GPON_TX_STAT_TCONT_i_GEM_FRAMES,       MV_ASIC_ONT_GLB_ADDR + 0x10F0,   0x10F0,   asicRO,     0xFFFFFFFF,     0,      8,     1,      0,     "Num of gem frames trans via tcont i (0-7)"},
	[mvAsicReg_GPON_TX_STAT_TCONT_i_IDLE_GEM]         =  { mvAsicReg_GPON_TX_STAT_TCONT_i_IDLE_GEM,         MV_ASIC_ONT_GLB_ADDR + 0x1118,   0x1118,   asicRO,     0xFFFFFFFF,     0,      8,     1,      0,     "Num of idle gem frames trans via tcont i (0-7)"},
	[mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_ETH_FRAME]   =  { mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_ETH_FRAME,   MV_ASIC_ONT_GLB_ADDR + 0x1140,   0x1140,   asicRO,     0xFFFFFFFF,     0,      8,     1,      0,     "Num of ethernet frames trans via tcont i (8-15)"},
	[mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_ETH_BYTES]   =  { mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_ETH_BYTES,   MV_ASIC_ONT_GLB_ADDR + 0x1168,   0x1168,   asicRO,     0xFFFFFFFF,     0,      8,     1,      0,     "Num of ethernet payload bytes trans via tcont i (8-15)"},
	[mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_GEM_FRAME]   =  { mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_GEM_FRAME,   MV_ASIC_ONT_GLB_ADDR + 0x1190,   0x1190,   asicRO,     0xFFFFFFFF,     0,      8,     1,      0,     "Num of gem frames trans via tcont i (8-15)"},
	[mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_IDLE_GEM]    =  { mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_IDLE_GEM,    MV_ASIC_ONT_GLB_ADDR + 0x11B8,   0x11B8,   asicRO,     0xFFFFFFFF,     0,      8,     1,      0,     "Num of idle gem frames trans via tcont i (8-15)"},
	[mvAsicReg_GPON_TX_HIGH_PRI_CONFIG_EN]            =  { mvAsicReg_GPON_TX_HIGH_PRI_CONFIG_EN,            MV_ASIC_ONT_GLB_ADDR + 0x11E0,   0x11E0,   asicRW,     0x00000001,     31,     0,     0,      0,     "Enable or disable GPON high priority TX feature"},
	[mvAsicReg_GPON_TX_HIGH_PRI_FOR_FRAGMENT]         =  { mvAsicReg_GPON_TX_HIGH_PRI_FOR_FRAGMENT,         MV_ASIC_ONT_GLB_ADDR + 0x11E0,   0x11E0,   asicRW,     0x00000001,     30,     0,     0,      0,     "Enable or disable high priority for fragment packet"},
	[mvAsicReg_GPON_TX_HIGH_PRI_VALID_BITMAP]         =  { mvAsicReg_GPON_TX_HIGH_PRI_VALID_BITMAP,         MV_ASIC_ONT_GLB_ADDR + 0x11E0,   0x11E0,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Bitmap of allowed low priority T-CONT"},
	[mvAsicReg_GPON_TX_HIGH_PRI_MAP_0]                =  { mvAsicReg_GPON_TX_HIGH_PRI_MAP_0,                MV_ASIC_ONT_GLB_ADDR + 0x11E4,   0x11E4,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "GPON high priority mapping 0"},
	[mvAsicReg_GPON_TX_HIGH_PRI_MAP_1]                =  { mvAsicReg_GPON_TX_HIGH_PRI_MAP_1,                MV_ASIC_ONT_GLB_ADDR + 0x11E8,   0x11E8,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "GPON high priority mapping 1"},

	/* =========================== */
	/*  UTM Registers              */
	/* =========================== */
	[mvAsicReg_GPON_UTM_CONFIG_OMCI_PORT_ID]          =  { mvAsicReg_GPON_UTM_CONFIG_OMCI_PORT_ID,          MV_ASIC_ONT_GLB_ADDR + 0x1400,   0x1400,   asicRW,     0x00000FFF,     0,      0,     0,      0,     "Omci gem port Id - upstream direction"},
	[mvAsicReg_GPON_UTM_CONFIG_OMCI_PORT_VALID]       =  { mvAsicReg_GPON_UTM_CONFIG_OMCI_PORT_VALID,       MV_ASIC_ONT_GLB_ADDR + 0x1404,   0x1404,   asicRW,     0x00000001,     0,      0,     0,      0,     "Omci gem port valid - upstream direction"},
	[mvAsicReg_GPON_UTM_CONFIG_TC_PERIOD]             =  { mvAsicReg_GPON_UTM_CONFIG_TC_PERIOD,             MV_ASIC_ONT_GLB_ADDR + 0x1408,   0x1408,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Num of cycles to pause between two counter gathering sweeps"},
	[mvAsicReg_GPON_UTM_CONFIG_TC_PERIOD_VALID]       =  { mvAsicReg_GPON_UTM_CONFIG_TC_PERIOD_VALID,       MV_ASIC_ONT_GLB_ADDR + 0x140C,   0x140C,   asicRW,     0x00000001,     0,      0,     0,      0,     "Tc period valid"},
	[mvAsicReg_GPON_UTM_ACTIVE_TX_BITMAP]             =  { mvAsicReg_GPON_UTM_ACTIVE_TX_BITMAP,             MV_ASIC_ONT_GLB_ADDR + 0x1410,   0x1410,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Active TX bitmap"},
	[mvAsicReg_GPON_UTM_ACTIVE_TX_BITMAP_VALID]       =  { mvAsicReg_GPON_UTM_ACTIVE_TX_BITMAP_VALID,       MV_ASIC_ONT_GLB_ADDR + 0x1414,   0x1414,   asicRW,     0x00000001,     0,      0,     0,      0,     "Active TX bitmap valid"},
	[mvAsicReg_GPON_UTM_FLUSH]                        =  { mvAsicReg_GPON_UTM_FLUSH,                        MV_ASIC_ONT_GLB_ADDR + 0x1418,   0x1418,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "UTM flush"},
	[mvAsicReg_GPON_UTM_GENERAL]                      =  { mvAsicReg_GPON_UTM_GENERAL,                      MV_ASIC_ONT_GLB_ADDR + 0x141C,   0x141C,   asicRW,     0x01FFFFFF,     0,      0,     0,      0,     "UTM general"},
	[mvAsicReg_GPON_UTM_DEBUG]                        =  { mvAsicReg_GPON_UTM_DEBUG,                        MV_ASIC_ONT_GLB_ADDR + 0x1420,   0x1420,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "UTM debug"},
	[mvAsicReg_GPON_UTM_REPORT_CONFIG]                =  { mvAsicReg_GPON_UTM_REPORT_CONFIG,                MV_ASIC_ONT_GLB_ADDR + 0x1424,   0x1424,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "UTM report configuration"},
	[mvAsicReg_GPON_UTM_REPORT_MAP_0]                 =  { mvAsicReg_GPON_UTM_REPORT_MAP_0,                 MV_ASIC_ONT_GLB_ADDR + 0x1428,   0x1428,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "UTM report bitmap 0"},
	[mvAsicReg_GPON_UTM_REPORT_MAP_0]                 =  { mvAsicReg_GPON_UTM_REPORT_MAP_0,                 MV_ASIC_ONT_GLB_ADDR + 0x142C,   0x142C,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "UTM report bitmap 1"},

	/* =========================== */
	/*  SGL Registers              */
	/* =========================== */
	[mvAsicReg_GPON_SGL_SW_RESET]                     =  { mvAsicReg_GPON_SGL_SW_RESET,                     MV_ASIC_ONT_GLB_ADDR + 0x1800,   0x1800,   asicRW,     0x00000001,     0,      0,     0,      0,     "Serdes glue logic - sw reset"},
	[mvAsicReg_GPON_SGL_CONFIG_REG]                   =  { mvAsicReg_GPON_SGL_CONFIG_REG,                   MV_ASIC_ONT_GLB_ADDR + 0x1804,   0x1804,   asicRW,     0x0000000F,     0,      0,     0,      0,     "Serdes glue logic - config"},
	[mvAsicReg_GPON_SGL_STATS_REG]                    =  { mvAsicReg_GPON_SGL_STATS_REG,                    MV_ASIC_ONT_GLB_ADDR + 0x1808,   0x1808,   asicRO,     0x000000FF,     0,      0,     0,      0,     "Serdes glue logic - stats"},

	/* =========================== */
	/*  Memory Registers           */
	/* =========================== */
	[mvAsicReg_GPON_TX_CONST_DATA_RAM]                =  { mvAsicReg_GPON_TX_CONST_DATA_RAM,                MV_ASIC_ONT_GLB_ADDR + 0x2000,   0x2000,   asicWO,     0xFFFFFFFF,     0,      8,     1,      0,     "Tx constantant data ram"},
	[mvAsicReg_GPON_TX_CONST_DATA_RAM_IDLE]           =  { mvAsicReg_GPON_TX_CONST_DATA_RAM_IDLE,           MV_ASIC_ONT_GLB_ADDR + 0x2000,   0x2000,   asicWO,     0xFFFFFFFF,     0,      3,     1,      0,     "Tx constantant data - Idle message"},
	[mvAsicReg_GPON_TX_CONST_DATA_RAM_SN]             =  { mvAsicReg_GPON_TX_CONST_DATA_RAM_SN,             MV_ASIC_ONT_GLB_ADDR + 0x2020,   0x2020,   asicWO,     0xFFFFFFFF,     0,      3,     1,      0,     "Tx constantant data - SN message"},

	[mvAsicReg_GPON_GEM_AES_PID_TABLE]                =  { mvAsicReg_GPON_GEM_AES_PID_TABLE,                MV_ASIC_ONT_GLB_ADDR + 0x2200,   0x2200,   asicWO,     0xFFFFFFFF,     0,      128,   1,      0,     "Gem AES port Id table"},
	[mvAsicReg_GPON_GEM_VALID_PID_TABLE]              =  { mvAsicReg_GPON_GEM_VALID_PID_TABLE,              MV_ASIC_ONT_GLB_ADDR + 0x2400,   0x2400,   asicWO,     0xFFFFFFFF,     0,      128,   1,      0,     "Gem valid port Id table"},

	[mvAsicReg_GPON_TEST]                             =  { mvAsicReg_GPON_TEST,                             MV_ASIC_ONT_GLB_ADDR + 0x0940,   0x0940,   asicRW,     0x03FF03FF,     0,      0,     0,      0,     "Gem valid port Id table"},

	/* =========================== */
	/*  GEM Port MIB Counters      */
	/* =========================== */
	[mvAsicReg_PON_RX_MIB_CONTROL]                    =  {mvAsicReg_PON_RX_MIB_CONTROL,                   0xCB800,                         0xCB800,  asicRW,     0x0001FFFF,     0,      16,     1,      0,     "PON MAC MIB RX control"},
	[mvAsicReg_PON_TX_MIB_CONTROL]                    =  {mvAsicReg_PON_TX_MIB_CONTROL,                   0xCB840,                         0xCB840,  asicRW,     0x0001FFFF,     0,      16,     1,      0,     "PON MAC MIB TX control"},
	[mvAsicReg_PON_RX_MIB_DEFAULT]                    =  {mvAsicReg_PON_RX_MIB_DEFAULT,                   0xCB880,                         0xCB880,  asicRW,     0x0000001F,    12,      0,      0,      0,     "PON MAC MIB RX default"},
	[mvAsicReg_PON_TX_MIB_DEFAULT]                    =  {mvAsicReg_PON_TX_MIB_DEFAULT,                   0xCB884,                         0xCB884,  asicRW,     0x0000001F,    12,      0,      0,      0,     "PON MAC MIB TX default"},
	[mvAsicReg_PON_MIB_MIN_MAX_LENGTH]                =  {mvAsicReg_PON_MIB_MIN_MAX_LENGTH,               0xCB888,                         0xCB888,  asicRW,     0x001FFFFF,     0,      0,      0,      0,     "PON MAC MIB MIN MAX length"},
	[mvAsicReg_PON_MIB_CONFIG]                        =  {mvAsicReg_PON_MIB_CONFIG,                       0xCB88C,                         0xCB88C,  asicRW,     0x00000001,     0,      0,      0,      0,     "PON MAC MIB configuration"},

	[mvAsicReg_PON_MIB_RX_GOOD_OCT_LS]                =  {mvAsicReg_PON_MIB_RX_GOOD_OCT_LS,               0xCB000,                         0xCB000,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX good octects LSN"},
	[mvAsicReg_PON_MIB_RX_GOOD_OCT_MS]                =  {mvAsicReg_PON_MIB_RX_GOOD_OCT_MS,               0xCB040,                         0xCB040,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX good octects MSN"},
	[mvAsicReg_PON_MIB_RX_GOOD_FRAME]                 =  {mvAsicReg_PON_MIB_RX_GOOD_FRAME,                0xCB080,                         0xCB080,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX good frame"},
	[mvAsicReg_PON_MIB_RX_BAD_OCTECT]                 =  {mvAsicReg_PON_MIB_RX_BAD_OCTECT,                0xCB0C0,                         0xCB0C0,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX bad octects"},
	[mvAsicReg_PON_MIB_RX_BAD_FRAME]                  =  {mvAsicReg_PON_MIB_RX_BAD_FRAME,                 0xCB100,                         0xCB100,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX bad frame"},
	[mvAsicReg_PON_MIB_RX_MULTICAST]                  =  {mvAsicReg_PON_MIB_RX_MULTICAST,                 0xCB140,                         0xCB140,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX multicast packet"},
	[mvAsicReg_PON_MIB_RX_BROADCAST]                  =  {mvAsicReg_PON_MIB_RX_BROADCAST,                 0xCB180,                         0xCB180,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX broadcast packet"},
	[mvAsicReg_PON_MIB_RX_64_OCT]                     =  {mvAsicReg_PON_MIB_RX_64_OCT,                    0xCB1C0,                         0xCB1C0,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX octects below or equal 64 bytes"},
	[mvAsicReg_PON_MIB_RX_65_127_OCT]                 =  {mvAsicReg_PON_MIB_RX_65_127_OCT,                0xCB200,                         0xCB200,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX octects between 65 and 127 bytes"},
	[mvAsicReg_PON_MIB_RX_128_255_OCT]                =  {mvAsicReg_PON_MIB_RX_128_255_OCT,               0xCB240,                         0xCB240,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX octects between 128 and 255 bytes"},
	[mvAsicReg_PON_MIB_RX_256_511_OCT]                =  {mvAsicReg_PON_MIB_RX_256_511_OCT,               0xCB280,                         0xCB280,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX octects between 256 and 511 bytes"},
	[mvAsicReg_PON_MIB_RX_512_1023_OCT]               =  {mvAsicReg_PON_MIB_RX_512_1023_OCT,              0xCB2C0,                         0xCB2C0,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX octects between 511 and 1023 bytes"},
	[mvAsicReg_PON_MIB_RX_1024_OCT]                   =  {mvAsicReg_PON_MIB_RX_1024_OCT,                  0xCB300,                         0xCB300,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB RX octects above or equal 1024 bytes"},
	[mvAsicReg_PON_MIB_TX_GOOD_OCT_LS]                =  {mvAsicReg_PON_MIB_TX_GOOD_OCT_LS,               0xCB400,                         0xCB400,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX good octects LSN"},
	[mvAsicReg_PON_MIB_TX_GOOD_OCT_MS]                =  {mvAsicReg_PON_MIB_TX_GOOD_OCT_MS,               0xCB440,                         0xCB440,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX good octects MSN"},
	[mvAsicReg_PON_MIB_TX_GOOD_FRAME]                 =  {mvAsicReg_PON_MIB_TX_GOOD_FRAME,                0xCB480,                         0xCB480,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX good frame"},
	[mvAsicReg_PON_MIB_TX_BAD_OCTECT]                 =  {mvAsicReg_PON_MIB_TX_BAD_OCTECT,                0xCB4C0,                         0xCB4C0,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX bad octects"},
	[mvAsicReg_PON_MIB_TX_MAC_ERROR]                  =  {mvAsicReg_PON_MIB_TX_MAC_ERROR,                 0xCB500,                         0xCB500,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX bad frames by MAC errors"},
	[mvAsicReg_PON_MIB_TX_MULTICAST]                  =  {mvAsicReg_PON_MIB_TX_MULTICAST,                 0xCB540,                         0xCB540,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX multicast packet"},
	[mvAsicReg_PON_MIB_TX_BROADCAST]                  =  {mvAsicReg_PON_MIB_TX_BROADCAST,                 0xCB580,                         0xCB580,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX broadcast packet"},
	[mvAsicReg_PON_MIB_TX_64_OCT]                     =  {mvAsicReg_PON_MIB_TX_64_OCT,                    0xCB5C0,                         0xCB5C0,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX octects below or equal 64 bytes"},
	[mvAsicReg_PON_MIB_TX_65_127_OCT]                 =  {mvAsicReg_PON_MIB_TX_65_127_OCT,                0xCB600,                         0xCB600,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX octects between 65 and 127 bytes"},
	[mvAsicReg_PON_MIB_TX_128_255_OCT]                =  {mvAsicReg_PON_MIB_TX_128_255_OCT,               0xCB640,                         0xCB640,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX octects between 128 and 255 bytes"},
	[mvAsicReg_PON_MIB_TX_256_511_OCT]                =  {mvAsicReg_PON_MIB_TX_256_511_OCT,               0xCB680,                         0xCB680,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX octects between 256 and 511 bytes"},
	[mvAsicReg_PON_MIB_TX_512_1023_OCT]               =  {mvAsicReg_PON_MIB_TX_512_1023_OCT,              0xCB6C0,                         0xCB6C0,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX octects between 511 and 1023 bytes"},
	[mvAsicReg_PON_MIB_TX_1024_OCT]                   =  {mvAsicReg_PON_MIB_TX_1024_OCT,                  0xCB700,                         0xCB700,  asicRO,     0xFFFFFFFF,     0,      16,     1,      0,     "PON MAC MIB TX octects above or equal 1024 bytes"},



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

	/* =========================== */
	/*  EPON Interrupt Registers   */
	/* =========================== */
	[mvAsicReg_EPON_INTR_REG]                         =  { mvAsicReg_EPON_INTR_REG,                         MV_ASIC_ONT_GLB_ADDR + 0x0000,   0x0000,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Interrupt PON"},
	[mvAsicReg_EPON_INTR_MASK]                        =  { mvAsicReg_EPON_INTR_MASK,                        MV_ASIC_ONT_GLB_ADDR + 0x0004,   0x0004,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Interrupt PON Mask"},
	[mvAsicReg_EPON_DBR_INT_LLID_MASK]                =  { mvAsicReg_EPON_DBR_INT_LLID_MASK,                MV_ASIC_ONT_GLB_ADDR + 0x001C,   0x001C,   asicRW,     0x000000FF,     16,     0,     0,      0,     "DBR Interrupt mask per LLID"},
	[mvAsicReg_EPON_DBR_INT_MASK]                     =  { mvAsicReg_EPON_DBR_INT_MASK,                     MV_ASIC_ONT_GLB_ADDR + 0x001C,   0x001C,   asicRW,     0x00000001,     7,      0,     0,      0,     "DBR Interrupt mask"},
	[mvAsicReg_EPON_DBR_INT]                          =  { mvAsicReg_EPON_DBR_INT,                          MV_ASIC_ONT_GLB_ADDR + 0x0020,   0x0020,   asicRO,     0x000000FF,     0,      0,     0,      0,     "DBR Interrupt"},
	[mvAsicReg_EPON_DBR_LLID_INT]                     =  { mvAsicReg_EPON_DBR_LLID_INT,                     MV_ASIC_ONT_GLB_ADDR + 0x0040,   0x0040,   asicRO,     0x00000001,     7,      8,     1,      0,     "DBR LLID Interrupt"},

	/* =========================== */
	/*  EPON General Registers     */
	/* =========================== */
	[mvAsicReg_EPON_GEN_MAC_VERSION_ADDR]             =  { mvAsicReg_EPON_GEN_MAC_VERSION_ADDR,             MV_ASIC_ONT_GLB_ADDR + 0x0400,   0x0400,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "EPON MAC version"},
	[mvAsicReg_EPON_GEN_ONT_ENABLE]                   =  { mvAsicReg_EPON_GEN_ONT_ENABLE,                   MV_ASIC_ONT_GLB_ADDR + 0x040C,   0x040C,   asicRW,     0x0000FF01,     0,      0,     0,      0,     "ONU Rx/Tx Enable"},
	[mvAsicReg_EPON_GEN_ONT_RX_ENABLE]                =  { mvAsicReg_EPON_GEN_ONT_RX_ENABLE,                MV_ASIC_ONT_GLB_ADDR + 0x040C,   0x040C,   asicRW,     0x00000001,     0,      0,     0,      0,     "ONU Rx Enable"},
	[mvAsicReg_EPON_GEN_ONT_TX_ENABLE]                =  { mvAsicReg_EPON_GEN_ONT_TX_ENABLE,                MV_ASIC_ONT_GLB_ADDR + 0x040C,   0x040C,   asicRW,     0x000000FF,     8,      0,     0,      0,     "ONU Tx Enable"},
#ifdef PON_A0
	[mvAsicReg_EPON_GEN_ONT_STATE]                    =  { mvAsicReg_EPON_GEN_ONT_STATE,                    MV_ASIC_ONT_GLB_ADDR + 0x0410,   0x0410,   asicRW,     0x00001FFF,     0,      0,     0,      0,     "ONU State"},
#else /* PON_Z1 */
	[mvAsicReg_EPON_GEN_ONT_STATE]                    =  { mvAsicReg_EPON_GEN_ONT_STATE,                    MV_ASIC_ONT_GLB_ADDR + 0x0410,   0x0410,   asicRW,     0x00000FFF,     0,      0,     0,      0,     "ONU State"},
#endif
	[mvAsicReg_EPON_GEN_ONT_STATE_REG]                =  { mvAsicReg_EPON_GEN_ONT_STATE_REG,                MV_ASIC_ONT_GLB_ADDR + 0x0410,   0x0410,   asicRW,     0x000000FF,     0,      0,     0,      0,     "ONU Register State"},
	[mvAsicReg_EPON_GEN_ONT_STATE_REG_AUTO_EN]        =  { mvAsicReg_EPON_GEN_ONT_STATE_REG_AUTO_EN,        MV_ASIC_ONT_GLB_ADDR + 0x0410,   0x0410,   asicRW,     0x00000001,     8,      0,     0,      0,     "ONU Register State Auto Enable"},
	[mvAsicReg_EPON_GEN_ONT_STATE_REREG_AUTO_EN]      =  { mvAsicReg_EPON_GEN_ONT_STATE_REREG_AUTO_EN,      MV_ASIC_ONT_GLB_ADDR + 0x0410,   0x0410,   asicRW,     0x00000001,     9,      0,     0,      0,     "ONU ReRegister State Auto Enable"},
	[mvAsicReg_EPON_GEN_ONT_STATE_DEREG_AUTO_EN]      =  { mvAsicReg_EPON_GEN_ONT_STATE_DEREG_AUTO_EN,      MV_ASIC_ONT_GLB_ADDR + 0x0410,   0x0410,   asicRW,     0x00000001,    10,      0,     0,      0,     "ONU DeRegister State Auto Enable"},
	[mvAsicReg_EPON_GEN_ONT_STATE_NACK_AUTO_EN]       =  { mvAsicReg_EPON_GEN_ONT_STATE_NACK_AUTO_EN,       MV_ASIC_ONT_GLB_ADDR + 0x0410,   0x0410,   asicRW,     0x00000001,    11,      0,     0,      0,     "ONU Nack State Auto Enable"},
	[mvAsicReg_EPON_GEN_ONT_UP_RX_LLID_PAD]           =  { mvAsicReg_EPON_GEN_ONT_UP_RX_LLID_PAD,           MV_ASIC_ONT_GLB_ADDR + 0x0410,   0x0410,   asicRW,     0x00000001,    12,      0,     0,      0,     "ONU UP RX LLID Padding"},
	[mvAsicReg_EPON_GEN_TQ_SIZE]                      =  { mvAsicReg_EPON_GEN_TQ_SIZE,                      MV_ASIC_ONT_GLB_ADDR + 0x0414,   0x0414,   asicRW,     0x000000FF,     0,      0,     0,      0,     "TQ Size"},
	[mvAsicReg_EPON_GEN_LASER_PARAM]                  =  { mvAsicReg_EPON_GEN_LASER_PARAM,                  MV_ASIC_ONT_GLB_ADDR + 0x0418,   0x0418,   asicRW,     0x01FFFFFF,     0,      0,     0,      0,     "Laser Params"},
	[mvAsicReg_EPON_GEN_TAIL_GUARD]                   =  { mvAsicReg_EPON_GEN_TAIL_GUARD,                   MV_ASIC_ONT_GLB_ADDR + 0x0420,   0x0420,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Tail Guard"},
	[mvAsicReg_EPON_GEN_SYNC_TIME]                    =  { mvAsicReg_EPON_GEN_SYNC_TIME,                    MV_ASIC_ONT_GLB_ADDR + 0x0424,   0x0424,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Sync Time"},
	[mvAsicReg_EPON_GEN_BROADCAST_ADDR_LOW]           =  { mvAsicReg_EPON_GEN_BROADCAST_ADDR_LOW,           MV_ASIC_ONT_GLB_ADDR + 0x0428,   0x0428,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Broadcast Address Low"},
	[mvAsicReg_EPON_GEN_BROADCAST_ADDR_HIGH]          =  { mvAsicReg_EPON_GEN_BROADCAST_ADDR_HIGH,          MV_ASIC_ONT_GLB_ADDR + 0x042C,   0x042C,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Broadcast Address High"},
	[mvAsicReg_EPON_GEN_CONFIGURATION]                =  { mvAsicReg_EPON_GEN_CONFIGURATION,                MV_ASIC_ONT_GLB_ADDR + 0x0430,   0x0430,   asicRW,     0x0000FF73,     0,      0,     0,      0,     "Gen Configuration"},
	[mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_REPORT]    =  { mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_REPORT,    MV_ASIC_ONT_GLB_ADDR + 0x0430,   0x0430,   asicRW,     0x00000001,     4,      0,     0,      0,     "Gen Configuration Auto Report"},
	[mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_ACK]       =  { mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_ACK,       MV_ASIC_ONT_GLB_ADDR + 0x0430,   0x0430,   asicRW,     0x00000001,     5,      0,     0,      0,     "Gen Configuration Auto Ack"},
	[mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_REQUEST]   =  { mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_REQUEST,   MV_ASIC_ONT_GLB_ADDR + 0x0430,   0x0430,   asicRW,     0x00000001,     6,      0,     0,      0,     "Gen Configuration Auto Request"},
	[mvAsicReg_EPON_GEN_SLD]                          =  { mvAsicReg_EPON_GEN_SLD,                          MV_ASIC_ONT_GLB_ADDR + 0x0434,   0x0434,   asicRW,     0x000000FF,     0,      0,     0,      0,     "Sld"},
	[mvAsicReg_EPON_GEN_MAC_CONTROL_TYPE]             =  { mvAsicReg_EPON_GEN_MAC_CONTROL_TYPE,             MV_ASIC_ONT_GLB_ADDR + 0x0438,   0x0438,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Mac Control type"},
	[mvAsicReg_EPON_GEN_LOCAL_TIMESTAMP]              =  { mvAsicReg_EPON_GEN_LOCAL_TIMESTAMP,              MV_ASIC_ONT_GLB_ADDR + 0x043C,   0x043C,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Local TimeStamp"},
	[mvAsicReg_EPON_GEN_NUM_OF_PENDING_GRANTS]        =  { mvAsicReg_EPON_GEN_NUM_OF_PENDING_GRANTS,        MV_ASIC_ONT_GLB_ADDR + 0x0448,   0x0448,   asicRW,     0x000000FF,     0,      0,     0,      0,     "Num Of Pending Grants"},
	[mvAsicReg_EPON_GEN_SGL_STATUS]                   =  { mvAsicReg_EPON_GEN_SGL_STATUS,                   MV_ASIC_ONT_GLB_ADDR + 0x044C,   0x044C,   asicRW,     0x000000FF,     0,      0,     0,      0,     "SGL Status"},
	[mvAsicReg_EPON_GEN_PON_PHY_DEBUG]                =  { mvAsicReg_EPON_GEN_PON_PHY_DEBUG,                MV_ASIC_ONT_GLB_ADDR + 0x0450,   0x0450,   asicRW,     0x000000FF,     0,      0,     0,      0,     "Phy Debug"},
	[mvAsicReg_EPON_GEN_SGL_SW_RESET]                 =  { mvAsicReg_EPON_GEN_SGL_SW_RESET,                 MV_ASIC_ONT_GLB_ADDR + 0x0454,   0x0454,   asicRW,     0x00000001,     0,      0,     0,      0,     "SGL SW Reset"},
	[mvAsicReg_EPON_GEN_SGL_CONFIG]                   =  { mvAsicReg_EPON_GEN_SGL_CONFIG,                   MV_ASIC_ONT_GLB_ADDR + 0x0458,   0x0458,   asicRW,     0x0000001F,     0,      0,     0,      0,     "SGL Config"},
	[mvAsicReg_EPON_GEN_SGL_DEBOUNCE_CONFIG]          =  { mvAsicReg_EPON_GEN_SGL_DEBOUNCE_CONFIG,          MV_ASIC_ONT_GLB_ADDR + 0x045C,   0x045C,   asicRW,     0x0001FFFF,     0,      0,     0,      0,     "SGL Debounce Config"},
	[mvAsicReg_EPON_GEN_SGL_DEBOUNCE_ENABLE]          =  { mvAsicReg_EPON_GEN_SGL_DEBOUNCE_ENABLE,          MV_ASIC_ONT_GLB_ADDR + 0x0460,   0x0460,   asicRW,     0x00000001,     0,      0,     0,      0,     "SGL Debounce Enable"},
	[mvAsicReg_EPON_GEN_TCH_CHURNING_KEY]             =  { mvAsicReg_EPON_GEN_TCH_CHURNING_KEY,             MV_ASIC_ONT_GLB_ADDR + 0x0464,   0x0464,   asicRW,     0x00FFFFFF,     0,      0,     0,      0,     "Churning Key random for SW"},
	[mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE1]           =  { mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE1,           MV_ASIC_ONT_GLB_ADDR + 0x0468,   0x0468,   asicRW,     0x001FFFFF,     0,      0,     0,      0,     "Additional Opcode 01"},
	[mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE2]           =  { mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE2,           MV_ASIC_ONT_GLB_ADDR + 0x046C,   0x046C,   asicRW,     0x001FFFFF,     0,      0,     0,      0,     "Additional Opcode 02"},
	[mvAsicReg_EPON_GEN_DRIFT_THRESHOLD]              =  { mvAsicReg_EPON_GEN_DRIFT_THRESHOLD,              MV_ASIC_ONT_GLB_ADDR + 0x0470,   0x0470,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Drift Threshold"},
	[mvAsicReg_EPON_GEN_TIMESTAMP_VAL_FOR_INTR]       =  { mvAsicReg_EPON_GEN_TIMESTAMP_VAL_FOR_INTR,       MV_ASIC_ONT_GLB_ADDR + 0x0474,   0x0474,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Timestamp valuer for interrupt"},
	[mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE3]           =  { mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE3,           MV_ASIC_ONT_GLB_ADDR + 0x0478,   0x0478,   asicRW,     0x001FFFFF,     0,      0,     0,      0,     "Additional Opcode 03"},
	[mvAsicReg_EPON_GEN_LLID_CRC_CONFIG]              =  { mvAsicReg_EPON_GEN_LLID_CRC_CONFIG,              MV_ASIC_ONT_GLB_ADDR + 0x047C,   0x047C,   asicRW,     0x000003FF,     0,      0,     0,      0,     "LLID configuration"},
	[mvAsicReg_EPON_GEN_TIMESTAMP_CONFIG]             =  { mvAsicReg_EPON_GEN_TIMESTAMP_CONFIG,             MV_ASIC_ONT_GLB_ADDR + 0x0480,   0x0480,   asicRW,     0x87FFC7FF,     0,      0,     0,      0,     "Timestamp configuration"},
	[mvAsicReg_EPON_GEN_ONT_RX_TIMESTAMP_CONFIG]      =  { mvAsicReg_EPON_GEN_ONT_RX_TIMESTAMP_CONFIG,      MV_ASIC_ONT_GLB_ADDR + 0x0484,   0x0484,   asicRW,     0x00008FFF,     0,      0,     0,      0,     "ONT Rx Timestamp configuration"},
	[mvAsicReg_EPON_GEN_ONT_REGISTER_PACKET_PADDING]  =  { mvAsicReg_EPON_GEN_ONT_REGISTER_PACKET_PADDING,  MV_ASIC_ONT_GLB_ADDR + 0x0488,   0x0488,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "ONT Register packet pading"},
	[mvAsicReg_EPON_GEN_MAC_ADDR_LOW]                 =  { mvAsicReg_EPON_GEN_MAC_ADDR_LOW,                 MV_ASIC_ONT_GLB_ADDR + 0x04A0,   0x04A0,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "MAC address low part"},
	[mvAsicReg_EPON_GEN_MAC_ADDR_HIGH]                =  { mvAsicReg_EPON_GEN_MAC_ADDR_HIGH,                MV_ASIC_ONT_GLB_ADDR + 0x04C8,   0x04C8,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "MAC address high part"},
	[mvAsicReg_EPON_GEN_UTM_TC_PERIOD]                =  { mvAsicReg_EPON_GEN_UTM_TC_PERIOD,                MV_ASIC_ONT_GLB_ADDR + 0x0720,   0x0720,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "UTM TC Period"},
	[mvAsicReg_EPON_GEN_UTM_TC_PERIOD_VALID]          =  { mvAsicReg_EPON_GEN_UTM_TC_PERIOD_VALID,          MV_ASIC_ONT_GLB_ADDR + 0x0724,   0x0724,   asicRW,     0x00000001,     0,      0,     0,      0,     "UTM TC Period valid"},
	[mvAsicReg_EPON_GEN_GEM_IPG_VAL]                  =  { mvAsicReg_EPON_GEN_GEM_IPG_VAL,                  MV_ASIC_ONT_GLB_ADDR + 0x0728,   0x0728,   asicRW,     0x000000FF,     0,      0,     0,      0,     "IPG Value"},
	[mvAsicReg_EPON_GEN_GEM_IPG_VAL_VALID]            =  { mvAsicReg_EPON_GEN_GEM_IPG_VAL_VALID,            MV_ASIC_ONT_GLB_ADDR + 0x072C,   0x072C,   asicRW,     0x00000001,     0,      0,     0,      0,     "IPG Value valid"},
	[mvAsicReg_EPON_GEN_ACT_TX_BITMAP]                =  { mvAsicReg_EPON_GEN_ACT_TX_BITMAP,                MV_ASIC_ONT_GLB_ADDR + 0x0730,   0x0730,   asicRW,     0x000000FF,     0,      0,     0,      0,     "UTM Active TX Bitmap"},
	[mvAsicReg_EPON_GEN_ACT_TX_VALID]                 =  { mvAsicReg_EPON_GEN_ACT_TX_VALID,                 MV_ASIC_ONT_GLB_ADDR + 0x0734,   0x0734,   asicRW,     0x00000001,     0,      0,     0,      0,     "UTM Active TX Valid"},
	[mvAsicReg_EPON_GEN_TOD_INT_WIDTH]                =  { mvAsicReg_EPON_GEN_TOD_INT_WIDTH,                MV_ASIC_ONT_GLB_ADDR + 0x0738,   0x0738,   asicRW,     0x1FFFFFFF,     0,      0,     0,      0,     "Time of Day Interrupt Width and Polarity"},
	[mvAsicReg_EPON_UTM_DEBUG]                        =  { mvAsicReg_EPON_UTM_DEBUG,                        MV_ASIC_ONT_GLB_ADDR + 0x0740,   0x0740,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "UTM debug"},
	[mvAsicReg_EPON_UTM_GENERAL]                      =  { mvAsicReg_EPON_UTM_GENERAL,                      MV_ASIC_ONT_GLB_ADDR + 0x0748,   0x0748,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "UTM general"},
	[mvAsicReg_EPON_TXM_EXT_REPORT_STATE]             =  { mvAsicReg_EPON_TXM_EXT_REPORT_STATE,             MV_ASIC_ONT_GLB_ADDR + 0x074C,   0x074C,   asicRW,     0x00000001,     31,     0,     0,      0,     "Extend report enable"},
	[mvAsicReg_EPON_TXM_EXT_REPORT_STATE_PER_TX]      =  { mvAsicReg_EPON_TXM_EXT_REPORT_STATE_PER_TX,      MV_ASIC_ONT_GLB_ADDR + 0x074C,   0x074C,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Extend report enable bitmap"},
	[mvAsicReg_EPON_TXM_EXT_REPORT_MAP_0]             =  { mvAsicReg_EPON_TXM_EXT_REPORT_MAP_0,             MV_ASIC_ONT_GLB_ADDR + 0x0750,   0x0750,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Extend report mapping 0"},
	[mvAsicReg_EPON_TXM_EXT_REPORT_MAP_1]             =  { mvAsicReg_EPON_TXM_EXT_REPORT_MAP_1,             MV_ASIC_ONT_GLB_ADDR + 0x0754,   0x0754,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Extend report mapping 1"},

	/* =========================== */
	/*  EPON RXP Registers         */
	/* =========================== */
	[mvAsicReg_EPON_RXP_PACKET_SIZE_LIMIT]            =  { mvAsicReg_EPON_RXP_PACKET_SIZE_LIMIT,            MV_ASIC_ONT_GLB_ADDR + 0x0810,   0x0810,   asicRW,     0x0FFF0FFF,     0,      0,     0,      0,     "Rxp packet limit size"},
#ifdef PON_Z1
	[mvAsicReg_EPON_RXP_PACKET_FILTER]                =  { mvAsicReg_EPON_RXP_PACKET_FILTER,                MV_ASIC_ONT_GLB_ADDR + 0x0814,   0x0814,   asicRW,     0x000000FF,     0,      0,     0,      0,     "Rxp packet filter"},
#else /* PON_A0 */
	[mvAsicReg_EPON_RXP_PACKET_FILTER]                =  { mvAsicReg_EPON_RXP_PACKET_FILTER,                MV_ASIC_ONT_GLB_ADDR + 0x0814,   0x0814,   asicRW,     0x000001FF,     0,      0,     0,      0,     "Rxp packet filter"},
#endif
#ifdef PON_Z1
	[mvAsicReg_EPON_RXP_CTRL_FRAME_FORWARD]           =  { mvAsicReg_EPON_RXP_CTRL_FRAME_FORWARD,           MV_ASIC_ONT_GLB_ADDR + 0x0818,   0x0818,   asicRW,     0x0000003F,     0,      0,     0,      0,     "Rxp ctrl frame forward"},
#else /* PON_A0 */
	[mvAsicReg_EPON_RXP_CTRL_FRAME_FORWARD]           =  { mvAsicReg_EPON_RXP_CTRL_FRAME_FORWARD,           MV_ASIC_ONT_GLB_ADDR + 0x0818,   0x0818,   asicRW,     0x000003FF,     0,      0,     0,      0,     "Rxp ctrl frame forward"},
#endif
	[mvAsicReg_EPON_RXP_LLT_LLID_DATA]                =  { mvAsicReg_EPON_RXP_LLT_LLID_DATA,                MV_ASIC_ONT_GLB_ADDR + 0x081C,   0x081C,   asicRW,     0x000FFFFF,     0,     10,     1,      0,     "Rxp LLID data"},
	[mvAsicReg_EPON_RXP_LLT_LLID_DATA_VALUE]          =  { mvAsicReg_EPON_RXP_LLT_LLID_DATA_VALUE,          MV_ASIC_ONT_GLB_ADDR + 0x081C,   0x081C,   asicRW,     0x00007FFF,     0,     10,     1,      0,     "Rxp LLID data value"},
	[mvAsicReg_EPON_RXP_LLT_LLID_DATA_INDEX]          =  { mvAsicReg_EPON_RXP_LLT_LLID_DATA_INDEX,          MV_ASIC_ONT_GLB_ADDR + 0x081C,   0x081C,   asicRW,     0x0000000F,    15,     10,     1,      0,     "Rxp LLID data index"},
	[mvAsicReg_EPON_RXP_LLT_LLID_DATA_VALID]          =  { mvAsicReg_EPON_RXP_LLT_LLID_DATA_VALID,          MV_ASIC_ONT_GLB_ADDR + 0x081C,   0x081C,   asicRW,     0x00000001,    19,     10,     1,      0,     "Rxp LLID data valid"},
	[mvAsicReg_EPON_RXP_ENCRYPTION_CONFIG]            =  { mvAsicReg_EPON_RXP_ENCRYPTION_CONFIG,            MV_ASIC_ONT_GLB_ADDR + 0x0844,   0x0844,   asicRW,     0x00000031,     0,      0,     0,      0,     "Rxp Enctyption Config"},
	[mvAsicReg_EPON_RXP_ENCRYPTION_KEY0]              =  { mvAsicReg_EPON_RXP_ENCRYPTION_KEY0,              MV_ASIC_ONT_GLB_ADDR + 0x0850,   0x0850,   asicRW,     0x00FFFFFF,     0,      8,     1,      0,     "Rxp Enctyption Key 0"},
	[mvAsicReg_EPON_RXP_ENCRYPTION_KEY1]              =  { mvAsicReg_EPON_RXP_ENCRYPTION_KEY1,              MV_ASIC_ONT_GLB_ADDR + 0x0878,   0x0878,   asicRW,     0x00FFFFFF,     0,      8,     1,      0,     "Rxp Enctyption Key 1"},
	[mvAsicReg_EPON_RXP_DATA_FIFO_THRESH]             =  { mvAsicReg_EPON_RXP_DATA_FIFO_THRESH,             MV_ASIC_ONT_GLB_ADDR + 0x08C0,   0x08C0,   asicRW,     0x00000FFF,     0,      8,     1,      0,     "Rxp FIFO Threshold"},

	/* =========================== */
	/*  EPON GPM Registers         */
	/* =========================== */
	[mvAsicReg_EPON_GPM_MAX_FUTURE_GRANT_TIME]        =  { mvAsicReg_EPON_GPM_MAX_FUTURE_GRANT_TIME,        MV_ASIC_ONT_GLB_ADDR + 0x0C10,   0x0C10,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Max future grant time"},
#ifdef PON_Z1
	[mvAsicReg_EPON_GPM_MIN_PROCESSING_TIME]          =  { mvAsicReg_EPON_GPM_MIN_PROCESSING_TIME,          MV_ASIC_ONT_GLB_ADDR + 0x0C14,   0x0C14,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Min processing time"},
#else /* PON_A0 */
	[mvAsicReg_EPON_GPM_MIN_PROCESSING_TIME]          =  { mvAsicReg_EPON_GPM_MIN_PROCESSING_TIME,          MV_ASIC_ONT_GLB_ADDR + 0x0C14,   0x0C14,   asicRW,     0x00FFFFFF,     0,      0,     0,      0,     "Min processing time"},
#endif
	[mvAsicReg_EPON_GPM_DISCOVERY_GRANT_LENGTH]       =  { mvAsicReg_EPON_GPM_DISCOVERY_GRANT_LENGTH,       MV_ASIC_ONT_GLB_ADDR + 0x0C18,   0x0C18,   asicRW,     0x0FFFFFFF,     0,      0,     0,      0,     "Discovery grant length"},
	[mvAsicReg_EPON_GPM_RX_SYNC_TIME]                 =  { mvAsicReg_EPON_GPM_RX_SYNC_TIME,                 MV_ASIC_ONT_GLB_ADDR + 0x0C1C,   0x0C1C,   asicRW,     0x0001FFFF,     0,      0,     0,      0,     "Rx sync time"},
	[mvAsicReg_EPON_GPM_GRANT_VALID]                  =  { mvAsicReg_EPON_GPM_GRANT_VALID,                  MV_ASIC_ONT_GLB_ADDR + 0x0C50,   0x0C50,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Grant valid counter"},
	[mvAsicReg_EPON_GPM_GRANT_MAX_FUTURE_TIME_ERR]    =  { mvAsicReg_EPON_GPM_GRANT_MAX_FUTURE_TIME_ERR,    MV_ASIC_ONT_GLB_ADDR + 0x0C70,   0x0C70,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Grant max future time error counter"},
	[mvAsicReg_EPON_GPM_MIN_PROC_TIME_ERR]            =  { mvAsicReg_EPON_GPM_MIN_PROC_TIME_ERR,            MV_ASIC_ONT_GLB_ADDR + 0x0C90,   0x0C90,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Min proc time error counter"},
	[mvAsicReg_EPON_GPM_LENGTH_ERR]                   =  { mvAsicReg_EPON_GPM_LENGTH_ERR,                   MV_ASIC_ONT_GLB_ADDR + 0x0CB0,   0x0CB0,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Length error counter"},
	[mvAsicReg_EPON_GPM_DISCOVERY_AND_REGISTERED_ERR] =  { mvAsicReg_EPON_GPM_DISCOVERY_AND_REGISTERED_ERR, MV_ASIC_ONT_GLB_ADDR + 0x0CD0,   0x0CD0,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Discovery and register error counter"},
	[mvAsicReg_EPON_GPM_FIFO_FULL_ERR]                =  { mvAsicReg_EPON_GPM_FIFO_FULL_ERR,                MV_ASIC_ONT_GLB_ADDR + 0x0CF0,   0x0CF0,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Fifo full error counter"},
	[mvAsicReg_EPON_GPM_OPC_DISC_NOT_REG_BCAST]       =  { mvAsicReg_EPON_GPM_OPC_DISC_NOT_REG_BCAST,       MV_ASIC_ONT_GLB_ADDR + 0x0D10,   0x0D10,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "OPC discard not register broadcast counter"},
	[mvAsicReg_EPON_GPM_OPC_REG_NOT_DISC]             =  { mvAsicReg_EPON_GPM_OPC_REG_NOT_DISC,             MV_ASIC_ONT_GLB_ADDR + 0x0D30,   0x0D30,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "OPC register not broadcast counter"},
	[mvAsicReg_EPON_GPM_OPC_DISC_NOT_REG_NOT_BCAST]   =  { mvAsicReg_EPON_GPM_OPC_DISC_NOT_REG_NOT_BCAST,   MV_ASIC_ONT_GLB_ADDR + 0x0D50,   0x0D50,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "OPC discard not register not broadcast counter"},
	[mvAsicReg_EPON_GPM_OPC_DROPED_GRANT]             =  { mvAsicReg_EPON_GPM_OPC_DROPED_GRANT,             MV_ASIC_ONT_GLB_ADDR + 0x0D70,   0x0D70,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "OPC dropped grant counter"},
	[mvAsicReg_EPON_GPM_OPC_HIDDEN_GRANT]             =  { mvAsicReg_EPON_GPM_OPC_HIDDEN_GRANT,             MV_ASIC_ONT_GLB_ADDR + 0x0D90,   0x0D90,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "OPC hidden grant counter"},
	[mvAsicReg_EPON_GPM_OPC_BACK_TO_BACK_GRANT]       =  { mvAsicReg_EPON_GPM_OPC_BACK_TO_BACK_GRANT,       MV_ASIC_ONT_GLB_ADDR + 0x0DB0,   0x0DB0,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "OPC back to back grant counter"},

	/* =========================== */
	/*  EPON TXM Registers         */
	/* =========================== */
	[mvAsicReg_EPON_TXM_DEFAULT_OVERHEAD]             =  { mvAsicReg_EPON_TXM_DEFAULT_OVERHEAD,             MV_ASIC_ONT_GLB_ADDR + 0x1010,   0x1010,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Default overhead"},
	[mvAsicReg_EPON_TXM_CONFIGURATION]                =  { mvAsicReg_EPON_TXM_CONFIGURATION,                MV_ASIC_ONT_GLB_ADDR + 0x1018,   0x1018,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Tx Configuration"},
	[mvAsicReg_EPON_TXM_POWER_UP]                     =  { mvAsicReg_EPON_TXM_POWER_UP,                     MV_ASIC_ONT_GLB_ADDR + 0x1020,   0x1020,   asicRW,     0x00001FFF,     0,      0,     0,      0,     "Tx MUX Power UP"},
	[mvAsicReg_EPON_TXM_TX_LLID]                      =  { mvAsicReg_EPON_TXM_TX_LLID,                      MV_ASIC_ONT_GLB_ADDR + 0x1050,   0x1050,   asicRW,     0x00007FFF,     0,      8,     1,      0,     "Tx LLID"},

	[mvAsicReg_EPON_TXM_CPP_RPRT_CONFIG]              =  { mvAsicReg_EPON_TXM_CPP_RPRT_CONFIG,              MV_ASIC_ONT_GLB_ADDR + 0x1078,   0x1078,   asicRW,     0x03FD03FF,     0,      8,     1,      0,     "Cpp report configuration"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_BIT_MAP]             =  { mvAsicReg_EPON_TXM_CPP_RPRT_BIT_MAP,             MV_ASIC_ONT_GLB_ADDR + 0x1098,   0x1098,   asicRW,     0x0000FFFF,     0,      8,     1,      0,     "Cpp report bit map"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_SET]           =  { mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_SET,           MV_ASIC_ONT_GLB_ADDR + 0x10B8,   0x10B8,   asicRW,     0x0000003F,     0,      8,     1,      0,     "Cpp report queue sets"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_0]             =  { mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_0,             MV_ASIC_ONT_GLB_ADDR + 0x10D8,   0x10D8,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Cpp report queue0"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_1]             =  { mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_1,             MV_ASIC_ONT_GLB_ADDR + 0x10F8,   0x10F8,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Cpp report queue1"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_2]             =  { mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_2,             MV_ASIC_ONT_GLB_ADDR + 0x1118,   0x1118,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Cpp report queue2"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_3]             =  { mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_3,             MV_ASIC_ONT_GLB_ADDR + 0x1138,   0x1138,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Cpp report queue3"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_4]             =  { mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_4,             MV_ASIC_ONT_GLB_ADDR + 0x1158,   0x1158,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Cpp report queue4"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_5]             =  { mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_5,             MV_ASIC_ONT_GLB_ADDR + 0x1178,   0x1178,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Cpp report queue5"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_6]             =  { mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_6,             MV_ASIC_ONT_GLB_ADDR + 0x1198,   0x1198,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Cpp report queue6"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_7]             =  { mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_7,             MV_ASIC_ONT_GLB_ADDR + 0x11B8,   0x11B8,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "Cpp report queue7"},
	[mvAsicReg_EPON_TXM_CPP_RPRT_FEC_CONFIG]          =  { mvAsicReg_EPON_TXM_CPP_RPRT_FEC_CONFIG,          MV_ASIC_ONT_GLB_ADDR + 0x11DC,   0x11DC,   asicRW,     0x00FFFFFF,     0,      8,     1,      0,     "Cpp report fec config"},
	[mvAsicReg_EPON_TXM_CPP_RPM_REPORT_CONFIG]        =  { mvAsicReg_EPON_TXM_CPP_RPM_REPORT_CONFIG,        MV_ASIC_ONT_GLB_ADDR + 0x11E0,   0x11E0,   asicRW,     0x0000001F,     0,      0,     0,      0,     "RPM Report config"},
	[mvAsicReg_EPON_TXM_CPP_RPM_CTRL_FIFO_DBA]        =  { mvAsicReg_EPON_TXM_CPP_RPM_CTRL_FIFO_DBA,        MV_ASIC_ONT_GLB_ADDR + 0x11E0,   0x11E0,   asicRW,     0x000007FF,     5,      0,     0,      0,     "RPM contrl FIFO DBA config"},
	[mvAsicReg_EPON_TXM_CPP_RPM_ADX_REPORT_TBL]       =  { mvAsicReg_EPON_TXM_CPP_RPM_ADX_REPORT_TBL,       MV_ASIC_ONT_GLB_ADDR + 0x11E4,   0x11E4,   asicRW,     0x003F00FF,     0,      0,     0,      0,     "RPM Index Report Table"},
	[mvAsicReg_EPON_TXM_CTRL_FIFO_FLUSH]              =  { mvAsicReg_EPON_TXM_CTRL_FIFO_FLUSH,              MV_ASIC_ONT_GLB_ADDR + 0x11F0,   0x11F0,   asicRW,     0x000000FF,     0,      0,     0,      0,     "TX control FIFO flush"},
	[mvAsicReg_EPON_TXM_HIGH_PRI_TX_STATE_BITMAP]     =  { mvAsicReg_EPON_TXM_HIGH_PRI_TX_STATE_BITMAP,     MV_ASIC_ONT_GLB_ADDR + 0x11F4,   0x11F4,   asicRW,     0x000000FF,     0,      0,     0,      0,     "High priority TX valid bitmap"},
	[mvAsicReg_EPON_TXM_HIGH_PRI_TX_MAP_0]            =  { mvAsicReg_EPON_TXM_HIGH_PRI_TX_MAP_0,            MV_ASIC_ONT_GLB_ADDR + 0x11F8,   0x11F8,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "High priority TX map 0"},
	[mvAsicReg_EPON_TXM_HIGH_PRI_TX_MAP_1]            =  { mvAsicReg_EPON_TXM_HIGH_PRI_TX_MAP_1,            MV_ASIC_ONT_GLB_ADDR + 0x11FC,   0x11FC,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "High priority TX map 1"},

	/* =========================== */
	/*  EPON PCS Registers         */
	/* =========================== */
	[mvAsicReg_EPON_PCS_CONFIGURATION]                =  { mvAsicReg_EPON_PCS_CONFIGURATION,                MV_ASIC_ONT_GLB_ADDR + 0x1414,   0x1414,   asicRW,     0x00000033,     0,      0,     0,      0,     "PCS configuration"},
	[mvAsicReg_EPON_PCS_CONFIGURATION_RX_ENABLE]      =  { mvAsicReg_EPON_PCS_CONFIGURATION_RX_ENABLE,      MV_ASIC_ONT_GLB_ADDR + 0x1414,   0x1414,   asicRW,     0x00000001,     0,      0,     0,      0,     "PCS configuration Rx enable"},
	[mvAsicReg_EPON_PCS_CONFIGURATION_TX_ENABLE]      =  { mvAsicReg_EPON_PCS_CONFIGURATION_TX_ENABLE,      MV_ASIC_ONT_GLB_ADDR + 0x1414,   0x1414,   asicRW,     0x00000001,     4,      0,     0,      0,     "PCS configuration Tx enable"},
	[mvAsicReg_EPON_PCS_DELAY_CONFIG]                 =  { mvAsicReg_EPON_PCS_DELAY_CONFIG,                 MV_ASIC_ONT_GLB_ADDR + 0x1418,   0x1418,   asicRW,     0x00001FFF,     0,      0,     0,      0,     "PCS delay config"},
	[mvAsicReg_EPON_PCS_STATS_FEC_0]                  =  { mvAsicReg_EPON_PCS_STATS_FEC_0,                  MV_ASIC_ONT_GLB_ADDR + 0x141C,   0x141C,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "PCS stats Fec 0"},
	[mvAsicReg_EPON_PCS_STATS_FEC_1]                  =  { mvAsicReg_EPON_PCS_STATS_FEC_1,                  MV_ASIC_ONT_GLB_ADDR + 0x1420,   0x1420,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "PCS stats Fec 1"},
	[mvAsicReg_EPON_PCS_STATS_FEC_2]                  =  { mvAsicReg_EPON_PCS_STATS_FEC_2,                  MV_ASIC_ONT_GLB_ADDR + 0x1424,   0x1424,   asicRW,     0x00FFFFFF,     0,      0,     0,      0,     "PCS stats Fec 2"},
	[mvAsicReg_EPON_PCS_STATS_0]                      =  { mvAsicReg_EPON_PCS_STATS_0,                      MV_ASIC_ONT_GLB_ADDR + 0x1428,   0x1428,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "PCS stats 0"},
	[mvAsicReg_EPON_PCS_STATS_1]                      =  { mvAsicReg_EPON_PCS_STATS_1,                      MV_ASIC_ONT_GLB_ADDR + 0x142C,   0x142C,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "PCS stats 1"},
	[mvAsicReg_EPON_PCS_STATS_2]                      =  { mvAsicReg_EPON_PCS_STATS_2,                      MV_ASIC_ONT_GLB_ADDR + 0x1430,   0x1430,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "PCS stats 2"},
	[mvAsicReg_EPON_PCS_STATS_3]                      =  { mvAsicReg_EPON_PCS_STATS_3,                      MV_ASIC_ONT_GLB_ADDR + 0x1434,   0x1434,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "PCS stats 3"},
	[mvAsicReg_EPON_PCS_STATS_4]                      =  { mvAsicReg_EPON_PCS_STATS_4,                      MV_ASIC_ONT_GLB_ADDR + 0x1438,   0x1438,   asicRO,     0xFFFFFFFF,     0,      0,     0,      0,     "PCS stats 4"},
	[mvAsicReg_EPON_PCS_FRAME_SZ_LIMITS]              =  { mvAsicReg_EPON_PCS_FRAME_SZ_LIMITS,              MV_ASIC_ONT_GLB_ADDR + 0x143C,   0x143C,   asicRW,     0x1FFF0FFF,     0,      0,     0,      0,     "PCS Frame Size Limits"},

	/* =========================== */
	/*  EPON DDM Registers         */
	/* =========================== */
	[mvAsicReg_EPON_DDM_DELAY_CONFIG]                 =  { mvAsicReg_EPON_DDM_DELAY_CONFIG,                 MV_ASIC_ONT_GLB_ADDR + 0x1814,   0x1814,   asicRW,     0x0000FFFF,     0,      0,     0,      0,     "DDM delay config"},
	[mvAsicReg_EPON_DDM_TX_POLARITY]                  =  { mvAsicReg_EPON_DDM_TX_POLARITY,                  MV_ASIC_ONT_GLB_ADDR + 0x1818,   0x1818,   asicRW,     0x0000003F,     0,      0,     0,      0,     "DDM tx ploarity"},
	[mvAsicReg_EPON_STAT_RXP_FCS_ERROR_CNT]           =  { mvAsicReg_EPON_STAT_RXP_FCS_ERROR_CNT,           MV_ASIC_ONT_GLB_ADDR + 0x1828,   0x1828,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "RXP fcs error counter"},
	[mvAsicReg_EPON_STAT_RXP_SHORT_ERROR_CNT]         =  { mvAsicReg_EPON_STAT_RXP_SHORT_ERROR_CNT,         MV_ASIC_ONT_GLB_ADDR + 0x1848,   0x1848,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "RXP short error counter"},
	[mvAsicReg_EPON_STAT_RXP_LONG_ERROR_CNT]          =  { mvAsicReg_EPON_STAT_RXP_LONG_ERROR_CNT,          MV_ASIC_ONT_GLB_ADDR + 0x1868,   0x1868,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "RXP long error counter"},
	[mvAsicReg_EPON_STAT_RXP_DATA_FRAMES_CNT]         =  { mvAsicReg_EPON_STAT_RXP_DATA_FRAMES_CNT,         MV_ASIC_ONT_GLB_ADDR + 0x1888,   0x1888,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "RXP data frame counter"},
	[mvAsicReg_EPON_STAT_RXP_CTRL_FRAMES_CNT]         =  { mvAsicReg_EPON_STAT_RXP_CTRL_FRAMES_CNT,         MV_ASIC_ONT_GLB_ADDR + 0x18A8,   0x18A8,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "RXP ctrl frame counter"},
	[mvAsicReg_EPON_STAT_RXP_REPORT_FRAMES_CNT]       =  { mvAsicReg_EPON_STAT_RXP_REPORT_FRAMES_CNT,       MV_ASIC_ONT_GLB_ADDR + 0x18C8,   0x18C8,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "RXP report frame counter"},
	[mvAsicReg_EPON_STAT_RXP_GATE_FRAMES_CNT]         =  { mvAsicReg_EPON_STAT_RXP_GATE_FRAMES_CNT,         MV_ASIC_ONT_GLB_ADDR + 0x18E8,   0x18E8,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "RXP gate frame counter"},
	[mvAsicReg_EPON_STAT_TXP_CTRL_REG_REQ_FRAMES_CNT] =  { mvAsicReg_EPON_STAT_TXP_CTRL_REG_REQ_FRAMES_CNT, MV_ASIC_ONT_GLB_ADDR + 0x1908,   0x1908,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "TXP ctrl register req frame counter"},
	[mvAsicReg_EPON_STAT_TXP_CTRL_REG_ACK_FRAMES_CNT] =  { mvAsicReg_EPON_STAT_TXP_CTRL_REG_ACK_FRAMES_CNT, MV_ASIC_ONT_GLB_ADDR + 0x1928,   0x1928,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "TXP ctrl register ack frame counter"},
	[mvAsicReg_EPON_STAT_TXP_CTRL_REPORT_FRAMES_CNT]  =  { mvAsicReg_EPON_STAT_TXP_CTRL_REPORT_FRAMES_CNT,  MV_ASIC_ONT_GLB_ADDR + 0x1948,   0x1948,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "TXP ctrl report frame counter"},
	[mvAsicReg_EPON_STAT_TXP_DATA_FRAMES_CNT]         =  { mvAsicReg_EPON_STAT_TXP_DATA_FRAMES_CNT,         MV_ASIC_ONT_GLB_ADDR + 0x1968,   0x1968,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "TXP data frame counter"},
	[mvAsicReg_EPON_STAT_TXP_TX_ALLOWED_BYTE_CNT]     =  { mvAsicReg_EPON_STAT_TXP_TX_ALLOWED_BYTE_CNT,     MV_ASIC_ONT_GLB_ADDR + 0x1988,   0x1988,   asicRW,     0xFFFFFFFF,     0,      8,     1,      0,     "TXP tx allowed byte counter"},

	/* ===================================== */
	/*  EPON Control Packet queue Registers  */
	/* ===================================== */
	[mvAsicReg_EPON_CPQ_RX_CTRL_Q_READ]               =  { mvAsicReg_EPON_CPQ_RX_CTRL_Q_READ,               MV_ASIC_ONT_GLB_ADDR + 0x1C10,   0x1C10,   asicRW,     0x000000FF,     0,      0,     0,      0,     "CPQ rx ctrl queue read"},
	[mvAsicReg_EPON_CPQ_RX_CTRL_Q_USED]               =  { mvAsicReg_EPON_CPQ_RX_CTRL_Q_USED,               MV_ASIC_ONT_GLB_ADDR + 0x1C14,   0x1C14,   asicRW,     0x000007FF,     0,      0,     0,      0,     "CPQ rx ctrl queue used"},
	[mvAsicReg_EPON_CPQ_RX_RPRT_Q_READ]               =  { mvAsicReg_EPON_CPQ_RX_RPRT_Q_READ,               MV_ASIC_ONT_GLB_ADDR + 0x1C18,   0x1C18,   asicRW,     0x000000FF,     0,      0,     0,      0,     "CPQ rx report queue read"},
	[mvAsicReg_EPON_CPQ_RX_RPRT_Q_USED]               =  { mvAsicReg_EPON_CPQ_RX_RPRT_Q_USED,               MV_ASIC_ONT_GLB_ADDR + 0x1C1C,   0x1C1C,   asicRW,     0x000007FF,     0,      0,     0,      0,     "CPQ rx report queue used"},
	[mvAsicReg_EPON_CPQ_RX_CTRL_HQ_READ_L]            =  { mvAsicReg_EPON_CPQ_RX_CTRL_HQ_READ_L,            MV_ASIC_ONT_GLB_ADDR + 0x1C20,   0x1C20,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "CPQ rx ctrl header queue read L"},
	[mvAsicReg_EPON_CPQ_RX_CTRL_HQ_READ_H]            =  { mvAsicReg_EPON_CPQ_RX_CTRL_HQ_READ_H,            MV_ASIC_ONT_GLB_ADDR + 0x1C24,   0x1C24,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "CPQ rx ctrl header queue read H"},
	[mvAsicReg_EPON_CPQ_RX_CTRL_HQ_USED]              =  { mvAsicReg_EPON_CPQ_RX_CTRL_HQ_USED,              MV_ASIC_ONT_GLB_ADDR + 0x1C28,   0x1C28,   asicRW,     0x0000001F,     0,      0,     0,      0,     "CPQ rx ctrl header queue read used"},
	[mvAsicReg_EPON_CPQ_RX_RPRT_HQ_READ_L]            =  { mvAsicReg_EPON_CPQ_RX_RPRT_HQ_READ_L,            MV_ASIC_ONT_GLB_ADDR + 0x1C2C,   0x1C2C,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "CPQ rx report header queue read L"},
	[mvAsicReg_EPON_CPQ_RX_RPRT_HQ_READ_H]            =  { mvAsicReg_EPON_CPQ_RX_RPRT_HQ_READ_H,            MV_ASIC_ONT_GLB_ADDR + 0x1C30,   0x1C30,   asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "CPQ rx report header queue read H"},
	[mvAsicReg_EPON_CPQ_RX_RPRT_HQ_USED]              =  { mvAsicReg_EPON_CPQ_RX_RPRT_HQ_USED,              MV_ASIC_ONT_GLB_ADDR + 0x1C34,   0x1C34,   asicRW,     0x0000001F,     0,      0,     0,      0,     "CPQ rx report header queue read used"},
	[mvAsicReg_EPON_CPQ_TX_CTRL_Q_WRITE]              =  { mvAsicReg_EPON_CPQ_TX_CTRL_Q_WRITE,              MV_ASIC_ONT_GLB_ADDR + 0x1C78,   0x1C78,   asicWO,     0x000000FF,     0,      8,     1,      0,     "CPQ tx ctrl queue write"},
#ifdef PON_Z1
	[mvAsicReg_EPON_CPQ_TX_CTRL_Q_FREE]               =  { mvAsicReg_EPON_CPQ_TX_CTRL_Q_FREE,               MV_ASIC_ONT_GLB_ADDR + 0x1C98,   0x1C98,   asicRO,     0x000001FF,     0,      8,     1,      0,     "CPQ tx ctrl queue free"},
#else /* PON_A0 */
	[mvAsicReg_EPON_CPQ_TX_CTRL_Q_FREE]               =  { mvAsicReg_EPON_CPQ_TX_CTRL_Q_FREE,               MV_ASIC_ONT_GLB_ADDR + 0x1C98,   0x1C98,   asicRO,     0x00000FFF,     0,      8,     1,      0,     "CPQ tx ctrl queue free"},
#endif
#ifdef PON_Z1
	[mvAsicReg_EPON_CPQ_TX_CTRL_HQ_WRITE]             =  { mvAsicReg_EPON_CPQ_TX_CTRL_HQ_WRITE,             MV_ASIC_ONT_GLB_ADDR + 0x1C48,   0x1C48,   asicRW,     0x00FFFFFF,     0,      0,     0,      0,     "CPQ tx ctrl header queue write"},
	[mvAsicReg_EPON_CPQ_TX_CTRL_HQ_FREE]              =  { mvAsicReg_EPON_CPQ_TX_CTRL_HQ_FREE,              MV_ASIC_ONT_GLB_ADDR + 0x1C4C,   0x1C4C,   asicRW,     0x00FFFFFF,     0,      0,     0,      0,     "CPQ tx ctrl header queue free"},
#else /* PON_A0 */
	[mvAsicReg_EPON_CPQ_TX_CTRL_HQ_WRITE]             =  { mvAsicReg_EPON_CPQ_TX_CTRL_HQ_WRITE,             MV_ASIC_ONT_GLB_ADDR + 0x1CB8,   0x1CB8,   asicRW,     0x00FFFFFF,     0,      8,     1,      0,     "CPQ tx ctrl header queue write"},
	[mvAsicReg_EPON_CPQ_TX_CTRL_HQ_FREE]              =  { mvAsicReg_EPON_CPQ_TX_CTRL_HQ_FREE,              MV_ASIC_ONT_GLB_ADDR + 0x1CD8,   0x1CD8,   asicRW,     0x00FFFFFF,     0,      8,     1,      0,     "CPQ tx ctrl header queue free"},
#endif

	/* =========================== */
	/*  P2P Registers              */
	/* =========================== */
	[mvAsicReg_P2P_GEN_ONT_MODE]                      =  { mvAsicReg_P2P_GEN_ONT_MODE,                      MV_ASIC_ONT_GLB_ADDR + 0x040C,   0x040C,   asicRW,     0x00000001,    31,      0,     0,      0,     "ONU mode"},
	[mvAsicReg_P2P_PCS_CONFIGURATION]                 =  { mvAsicReg_P2P_PCS_CONFIGURATION,                 MV_ASIC_ONT_GLB_ADDR + 0x1414,   0x1414,   asicRW,     0x00000613,    16,      0,     0,      0,     "P2P PCS configuration"},
	[mvAsicReg_P2P_TXM_CFG_MODE]                      =  { mvAsicReg_P2P_TXM_CFG_MODE,                      MV_ASIC_ONT_GLB_ADDR + 0x1018,   0x1018,   asicRW,     0x00000003,     3,      0,     0,      0,     "Tx Configuration mode"},

#ifndef PON_FPGA
#ifdef DISABLE_DG_FOR_AVANTA_LP
	[mvAsicReg_PON_MPP_00]                            = {mvAsicReg_PON_MPP_00,                            0x18000,                         0x18000,  asicRW,     0x0000000F,     0,      0,     0,      0,     "Gpon MMP 00"},
	[mvAsicReg_PON_MPP_01]                            = {mvAsicReg_PON_MPP_01,                            0x18000,                         0x18000,  asicRW,     0x0000000F,     4,      0,     0,      0,     "Gpon MMP 01"},
	[mvAsicReg_PON_MPP_02]                            = {mvAsicReg_PON_MPP_02,                            0x18000,                         0x18000,  asicRW,     0x0000000F,     8,      0,     0,      0,     "Gpon MMP 02"},
	[mvAsicReg_PON_MPP_03]                            = {mvAsicReg_PON_MPP_03,                            0x18000,                         0x18000,  asicRW,     0x0000000F,    12,      0,     0,      0,     "Gpon MMP 03"},
	[mvAsicReg_PON_MPP_04]                            = {mvAsicReg_PON_MPP_04,                            0x18000,                         0x18000,  asicRW,     0x0000000F,    16,      0,     0,      0,     "Gpon MMP 04"},
	[mvAsicReg_PON_MPP_05]                            = {mvAsicReg_PON_MPP_05,                            0x18000,                         0x18000,  asicRW,     0x0000000F,    20,      0,     0,      0,     "Gpon MMP 05"},
	[mvAsicReg_PON_MPP_06]                            = {mvAsicReg_PON_MPP_06,                            0x18000,                         0x18000,  asicRW,     0x0000000F,    24,      0,     0,      0,     "Gpon MMP 06"},
	[mvAsicReg_PON_MPP_07]                            = {mvAsicReg_PON_MPP_07,                            0x18000,                         0x18000,  asicRW,     0x0000000F,    28,      0,     0,      0,     "Gpon MMP 07"},
	[mvAsicReg_PON_MPP_08]                            = {mvAsicReg_PON_MPP_08,                            0x18004,                         0x18004,  asicRW,     0x0000000F,     0,      0,     0,      0,     "Gpon MMP 08"},
	[mvAsicReg_PON_MPP_09]                            = {mvAsicReg_PON_MPP_09,                            0x18004,                         0x18004,  asicRW,     0x0000000F,     4,      0,     0,      0,     "Gpon MMP 09"},
	[mvAsicReg_PON_MPP_10]                            = {mvAsicReg_PON_MPP_10,                            0x18004,                         0x18004,  asicRW,     0x0000000F,     8,      0,     0,      0,     "Gpon MMP 10"},
	[mvAsicReg_PON_MPP_11]                            = {mvAsicReg_PON_MPP_11,                            0x18004,                         0x18004,  asicRW,     0x0000000F,    12,      0,     0,      0,     "Gpon MMP 11"},
	[mvAsicReg_PON_MPP_12]                            = {mvAsicReg_PON_MPP_12,                            0x18004,                         0x18004,  asicRW,     0x0000000F,    16,      0,     0,      0,     "Gpon MMP 12"},
	[mvAsicReg_PON_MPP_13]                            = {mvAsicReg_PON_MPP_13,                            0x18004,                         0x18004,  asicRW,     0x0000000F,    20,      0,     0,      0,     "Gpon MMP 13"},
	[mvAsicReg_PON_MPP_14]                            = {mvAsicReg_PON_MPP_14,                            0x18004,                         0x18004,  asicRW,     0x0000000F,    24,      0,     0,      0,     "Gpon MMP 14"},
	[mvAsicReg_PON_MPP_15]                            = {mvAsicReg_PON_MPP_15,                            0x18004,                         0x18004,  asicRW,     0x0000000F,    28,      0,     0,      0,     "Gpon MMP 15"},
	[mvAsicReg_PON_MPP_16]                            = {mvAsicReg_PON_MPP_16,                            0x18008,                         0x18008,  asicRW,     0x0000000F,     0,      0,     0,      0,     "Gpon MMP 16"},
	[mvAsicReg_PON_MPP_17]                            = {mvAsicReg_PON_MPP_17,                            0x18008,                         0x18008,  asicRW,     0x0000000F,     4,      0,     0,      0,     "Gpon MMP 17"},
	[mvAsicReg_PON_MPP_18]                            = {mvAsicReg_PON_MPP_18,                            0x18008,                         0x18008,  asicRW,     0x0000000F,     8,      0,     0,      0,     "Gpon MMP 18"},
	[mvAsicReg_PON_MPP_19]                            = {mvAsicReg_PON_MPP_19,                            0x18008,                         0x18008,  asicRW,     0x0000000F,    12,      0,     0,      0,     "Gpon MMP 19, TDM RST"},
	[mvAsicReg_PON_MPP_20]                            = {mvAsicReg_PON_MPP_20,                            0x18008,                         0x18008,  asicRW,     0x0000000F,    16,      0,     0,      0,     "Gpon MMP 20"},
	[mvAsicReg_PON_MPP_21]                            = {mvAsicReg_PON_MPP_21,                            0x18008,                         0x18008,  asicRW,     0x0000000F,    20,      0,     0,      0,     "Gpon MMP 21"},
	[mvAsicReg_PON_MPP_22]                            = {mvAsicReg_PON_MPP_22,                            0x18008,                         0x18008,  asicRW,     0x0000000F,    24,      0,     0,      0,     "Gpon MMP 22"},
	[mvAsicReg_PON_MPP_23]                            = {mvAsicReg_PON_MPP_23,                            0x18008,                         0x18008,  asicRW,     0x0000000F,    28,      0,     0,      0,     "Gpon MMP 23"},
	[mvAsicReg_PON_MPP_24]                            = {mvAsicReg_PON_MPP_24,                            0x1800C,                         0x1800C,  asicRW,     0x0000000F,     0,      0,     0,      0,     "Gpon MMP 24"},
	[mvAsicReg_PON_MPP_25]                            = {mvAsicReg_PON_MPP_25,                            0x1800C,                         0x1800C,  asicRW,     0x0000000F,     4,      0,     0,      0,     "Gpon MMP 25"},
	[mvAsicReg_PON_MPP_26]                            = {mvAsicReg_PON_MPP_26,                            0x1800C,                         0x1800C,  asicRW,     0x0000000F,     8,      0,     0,      0,     "Gpon MMP 26"},
	[mvAsicReg_PON_MPP_27]                            = {mvAsicReg_PON_MPP_27,                            0x1800C,                         0x1800C,  asicRW,     0x0000000F,    12,      0,     0,      0,     "Gpon MMP 27"},
	[mvAsicReg_PON_MPP_28]                            = {mvAsicReg_PON_MPP_28,                            0x1800C,                         0x1800C,  asicRW,     0x0000000F,    16,      0,     0,      0,     "Gpon MMP 28"},
	[mvAsicReg_PON_MPP_29]                            = {mvAsicReg_PON_MPP_29,                            0x1800C,                         0x1800C,  asicRW,     0x0000000F,    20,      0,     0,      0,     "Gpon MMP 29"},
	[mvAsicReg_PON_MPP_30]                            = {mvAsicReg_PON_MPP_30,                            0x1800C,                         0x1800C,  asicRW,     0x0000000F,    24,      0,     0,      0,     "Gpon MMP 30"},
	[mvAsicReg_PON_MPP_31]                            = {mvAsicReg_PON_MPP_31,                            0x1800C,                         0x1800C,  asicRW,     0x0000000F,    28,      0,     0,      0,     "Gpon MMP 31"},
	[mvAsicReg_PON_MPP_32]                            = {mvAsicReg_PON_MPP_32,                            0x18010,                         0x18010,  asicRW,     0x0000000F,     0,      0,     0,      0,     "Gpon MMP 32"},
	[mvAsicReg_PON_MPP_33]                            = {mvAsicReg_PON_MPP_33,                            0x18010,                         0x18010,  asicRW,     0x0000000F,     4,      0,     0,      0,     "Gpon MMP 33"},
	[mvAsicReg_PON_MPP_34]                            = {mvAsicReg_PON_MPP_34,                            0x18010,                         0x18010,  asicRW,     0x0000000F,     8,      0,     0,      0,     "Gpon MMP 34"},
	[mvAsicReg_PON_MPP_35]                            = {mvAsicReg_PON_MPP_35,                            0x18010,                         0x18010,  asicRW,     0x0000000F,    12,      0,     0,      0,     "Gpon MMP 35"},
	[mvAsicReg_PON_MPP_36]                            = {mvAsicReg_PON_MPP_36,                            0x18010,                         0x18010,  asicRW,     0x0000000F,    16,      0,     0,      0,     "Gpon MMP 36"},
	[mvAsicReg_PON_MPP_37]                            = {mvAsicReg_PON_MPP_37,                            0x18010,                         0x18010,  asicRW,     0x0000000F,    20,      0,     0,      0,     "Gpon MMP 37"},
	[mvAsicReg_PON_MPP_38]                            = {mvAsicReg_PON_MPP_38,                            0x18010,                         0x18010,  asicRW,     0x0000000F,    24,      0,     0,      0,     "Gpon MMP 38"},
	[mvAsicReg_PON_MPP_39]                            = {mvAsicReg_PON_MPP_39,                            0x18010,                         0x18010,  asicRW,     0x0000000F,    28,      0,     0,      0,     "Gpon MMP 39"},
	[mvAsicReg_PON_MPP_40]                            = {mvAsicReg_PON_MPP_40,                            0x18014,                         0x18014,  asicRW,     0x0000000F,     0,      0,     0,      0,     "Gpon MMP 40"},
	[mvAsicReg_PON_MPP_41]                            = {mvAsicReg_PON_MPP_41,                            0x18014,                         0x18014,  asicRW,     0x0000000F,     4,      0,     0,      0,     "Gpon MMP 41"},
	[mvAsicReg_PON_MPP_42]                            = {mvAsicReg_PON_MPP_42,                            0x18014,                         0x18014,  asicRW,     0x0000000F,     8,      0,     0,      0,     "Gpon MMP 42"},
	[mvAsicReg_PON_MPP_43]                            = {mvAsicReg_PON_MPP_43,                            0x18014,                         0x18014,  asicRW,     0x0000000F,    12,      0,     0,      0,     "Gpon MMP 43"},
	[mvAsicReg_PON_MPP_44]                            = {mvAsicReg_PON_MPP_44,                            0x18014,                         0x18014,  asicRW,     0x0000000F,    16,      0,     0,      0,     "Gpon MMP 44"},
	[mvAsicReg_PON_MPP_45]                            = {mvAsicReg_PON_MPP_45,                            0x18014,                         0x18014,  asicRW,     0x0000000F,    20,      0,     0,      0,     "Gpon MMP 45"},
	[mvAsicReg_PON_MPP_46]                            = {mvAsicReg_PON_MPP_46,                            0x18014,                         0x18014,  asicRW,     0x0000000F,    24,      0,     0,      0,     "Gpon MMP 46"},
	[mvAsicReg_PON_MPP_47]                            = {mvAsicReg_PON_MPP_47,                            0x18014,                         0x18014,  asicRW,     0x0000000F,    28,      0,     0,      0,     "Gpon MMP 47"},
	[mvAsicReg_PON_MPP_48]                            = {mvAsicReg_PON_MPP_48,                            0x18018,                         0x18018,  asicRW,     0x0000000F,     0,      0,     0,      0,     "Gpon MMP 48"},
	[mvAsicReg_PON_MPP_49]                            = {mvAsicReg_PON_MPP_49,                            0x18018,                         0x18018,  asicRW,     0x0000000F,     4,      0,     0,      0,     "Gpon MMP 49"},
	[mvAsicReg_PON_MPP_50]                            = {mvAsicReg_PON_MPP_50,                            0x18018,                         0x18018,  asicRW,     0x0000000F,     8,      0,     0,      0,     "Gpon MMP 50, RD Board Dying Gasp"},
	[mvAsicReg_PON_MPP_51]                            = {mvAsicReg_PON_MPP_51,                            0x18018,                         0x18018,  asicRW,     0x0000000F,    12,      0,     0,      0,     "Gpon MMP 51"},
	[mvAsicReg_PON_MPP_52]                            = {mvAsicReg_PON_MPP_52,                            0x18018,                         0x18018,  asicRW,     0x0000000F,    16,      0,     0,      0,     "Gpon MMP 52"},
	[mvAsicReg_PON_MPP_53]                            = {mvAsicReg_PON_MPP_53,                            0x18018,                         0x18018,  asicRW,     0x0000000F,    20,      0,     0,      0,     "Gpon MMP 53"},
	[mvAsicReg_PON_MPP_54]                            = {mvAsicReg_PON_MPP_54,                            0x18018,                         0x18018,  asicRW,     0x0000000F,    24,      0,     0,      0,     "Gpon MMP 54"},
	[mvAsicReg_PON_MPP_55]                            = {mvAsicReg_PON_MPP_55,                            0x18018,                         0x18018,  asicRW,     0x0000000F,    28,      0,     0,      0,     "Gpon MMP 55"},
	[mvAsicReg_PON_MPP_56]                            = {mvAsicReg_PON_MPP_56,                            0x1801C,                         0x1801C,  asicRW,     0x0000000F,     0,      0,     0,      0,     "Gpon MMP 56"},
	[mvAsicReg_PON_MPP_57]                            = {mvAsicReg_PON_MPP_57,                            0x1801C,                         0x1801C,  asicRW,     0x0000000F,     4,      0,     0,      0,     "Gpon MMP 57"},
	[mvAsicReg_PON_MPP_58]                            = {mvAsicReg_PON_MPP_58,                            0x1801C,                         0x1801C,  asicRW,     0x0000000F,     8,      0,     0,      0,     "Gpon MMP 58"},
	[mvAsicReg_PON_MPP_59]                            = {mvAsicReg_PON_MPP_59,                            0x1801C,                         0x1801C,  asicRW,     0x0000000F,    12,      0,     0,      0,     "Gpon MMP 59"},
	[mvAsicReg_PON_MPP_60]                            = {mvAsicReg_PON_MPP_60,                            0x1801C,                         0x1801C,  asicRW,     0x0000000F,    16,      0,     0,      0,     "Gpon MMP 60"},
	[mvAsicReg_PON_MPP_61]                            = {mvAsicReg_PON_MPP_61,                            0x1801C,                         0x1801C,  asicRW,     0x0000000F,    20,      0,     0,      0,     "Gpon MMP 61"},
	[mvAsicReg_PON_MPP_62]                            = {mvAsicReg_PON_MPP_62,                            0x1801C,                         0x1801C,  asicRW,     0x0000000F,    24,      0,     0,      0,     "Gpon MMP 62"},
	[mvAsicReg_PON_MPP_63]                            = {mvAsicReg_PON_MPP_63,                            0x1801C,                         0x1801C,  asicRW,     0x0000000F,    28,      0,     0,      0,     "Gpon MMP 63"},
	[mvAsicReg_PON_MPP_64]                            = {mvAsicReg_PON_MPP_64,                            0x18020,                         0x18020,  asicRW,     0x0000000F,     0,      0,     0,      0,     "Gpon MMP 64"},
	[mvAsicReg_PON_MPP_65]                            = {mvAsicReg_PON_MPP_65,                            0x18020,                         0x18020,  asicRW,     0x0000000F,     4,      0,     0,      0,     "Gpon MMP 65"},
	[mvAsicReg_PON_MPP_66]                            = {mvAsicReg_PON_MPP_66,                            0x18020,                         0x18020,  asicRW,     0x0000000F,     8,      0,     0,      0,     "Gpon MMP 66"},
#endif
#endif

	/* =========================== */
	/*  SERDES Registers           */
	/* =========================== */
	[mvAsicReg_PON_SERDES_PHY_CTRL_0]                 =  { mvAsicReg_PON_SERDES_PHY_CTRL_0,                 0x184F4,                         0x184F4,  asicRW,     0xFFFFFFFF,     0,      0,     0,      0,     "Gpon Phy Control 0"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_Pll]          =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_Pll,          0x184F4,                         0x184F4,  asicRW,     0x00000001,     0,      0,     0,      0,     "Gpon Phy Control 0 Power Up Pll"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_RX]           =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_RX,           0x184F4,                         0x184F4,  asicRW,     0x00000001,     1,      0,     0,      0,     "Gpon Phy Control 0 Power Up Rx"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_TX]           =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_TX,           0x184F4,                         0x184F4,  asicRW,     0x00000001,     2,      0,     0,      0,     "Gpon Phy Control 0 Power Up Tx"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_RST]             =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_RST,             0x184F4,                         0x184F4,  asicRW,     0x00000001,     3,      0,     0,      0,     "Gpon Phy Control 0 Reset"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_RST_TX_DOUT]     =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_RST_TX_DOUT,     0x184F4,                         0x184F4,  asicRW,     0x00000001,     4,      0,     0,      0,     "Gpon Phy Control 0 Reset Tx Dout"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_RST_CORE]        =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_RST_CORE,        0x184F4,                         0x184F4,  asicRW,     0x00000001,     5,      0,     0,      0,     "Gpon Phy Control 0 Reset Core"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_RX_INIT]         =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_RX_INIT,         0x184F4,                         0x184F4,  asicRW,     0x00000001,     6,      0,     0,      0,     "Gpon Phy Control 0 Rx Init"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_RX_RATE]         =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_RX_RATE,         0x184F4,                         0x184F4,  asicRW,     0x00000001,     7,      0,     0,      0,     "Gpon Phy Control 0 Rx Rate"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_TX_RATE]         =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_TX_RATE,         0x184F4,                         0x184F4,  asicRW,     0x00000001,     8,      0,     0,      0,     "Gpon Phy Control 0 Tx Rate"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_SEL_GEPON]       =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_SEL_GEPON,       0x184F4,                         0x184F4,  asicRW,     0x00000001,     9,      0,     0,      0,     "Gpon Phy Control 0 Sel GEPON"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_SEL_REF_CLK]     =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_SEL_REF_CLK,     0x184F4,                         0x184F4,  asicRW,     0x00000003,    10,      0,     0,      0,     "Gpon Phy Control 0 Sel Ref Clk"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_REF_CLK_25M]     =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_REF_CLK_25M,     0x184F4,                         0x184F4,  asicRW,     0x00000001,    12,      0,     0,      0,     "Gpon Phy Control 0 Ref Clk 25M"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_CID_REV]         =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_CID_REV,         0x184F4,                         0x184F4,  asicRW,     0x000000FF,    13,      0,     0,      0,     "Gpon Phy Control 0 Cid Rev"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_TX_SOURCE]    =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_TX_SOURCE,    0x184F4,                         0x184F4,  asicRW,     0x00000001,    21,      0,     0,      0,     "Gpon Phy Control 0 Pu Tx Source"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_REF_CLK_DISABLE] =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_REF_CLK_DISABLE, 0x184F4,                         0x184F4,  asicRW,     0x00000001,    22,      0,     0,      0,     "Gpon Phy Control 0 Ref clock disable"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_TX_IDLE]         =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_TX_IDLE,         0x184F4,                         0x184F4,  asicRW,     0x00000001,    23,      0,     0,      0,     "Gpon Phy Control 0 Tx Idle"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_IO_EN]       =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_IO_EN,       0x184F4,                         0x184F4,  asicRW,     0x00000001,    24,      0,     0,      0,     "Gpon Phy Control 0 BEN IO Enable"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_SW_HW_SELECT] = { mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_SW_HW_SELECT, 0x184F4,                         0x184F4,  asicRW,     0x00000001,    25,      0,     0,      0,     "Gpon Phy Control 0 Select SW or HW Control"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_SW_FORCE]    =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_SW_FORCE,    0x184F4,                         0x184F4,  asicRW,     0x00000001,    26,      0,     0,      0,     "Gpon Phy Control 0 Software Control Force Enable"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_SW_VALUE]    =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_BEN_SW_VALUE,    0x184F4,                         0x184F4,  asicRW,     0x00000001,    27,      0,     0,      0,     "Gpon Phy Control 0 Software Control Force Value"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_TOD_OVERRIDE]    =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_TOD_OVERRIDE,    0x184F4,                         0x184F4,  asicRW,     0x00000001,    28,      0,     0,      0,     "Gpon Phy Control 0 ToD Overide"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_DIG_TEST_EN]     =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_DIG_TEST_EN,     0x184F4,                         0x184F4,  asicRW,     0x00000001,    29,      0,     0,      0,     "Gpon Phy Control 0 Dig Test En"},
	[mvAsicReg_PON_SERDES_PHY_CTRL_0_EPON_MODE_SEL]   =  { mvAsicReg_PON_SERDES_PHY_CTRL_0_EPON_MODE_SEL,   0x184F4,                         0x184F4,  asicRW,     0x00000001,    30,      0,     0,      0,     "Gpon Phy Control 0 Epon Mode Sel"},

	[mvAsicReg_PON_SERDES_PHY_CTRL_1_PHY_SOURCE]      =  { mvAsicReg_PON_SERDES_PHY_CTRL_1_PHY_SOURCE,      0x184F8,                         0x184F8,  asicRW,     0x00000001,     0,      0,     0,      0,     "Gpon Phy Control 1 Gpon Phy Source"},

	[mvAsicReg_PON_SERDES_PHY_STATUS_REF_CLK_DIS_ACK] =  { mvAsicReg_PON_SERDES_PHY_STATUS_REF_CLK_DIS_ACK, 0x184FC,                         0x184FC,  asicRW,     0x00000001,    16,      0,     0,      0,     "Gpon Phy Status Ref Clock Disable Ack"},
	[mvAsicReg_PON_SERDES_PHY_STATUS_READY_RX]        =  { mvAsicReg_PON_SERDES_PHY_STATUS_READY_RX,        0x184FC,                         0x184FC,  asicRW,     0x00000001,    17,      0,     0,      0,     "Gpon Phy Status Pll Ready Rx"},
	[mvAsicReg_PON_SERDES_PHY_STATUS_READY_TX]        =  { mvAsicReg_PON_SERDES_PHY_STATUS_READY_TX,        0x184FC,                         0x184FC,  asicRW,     0x00000001,    18,      0,     0,      0,     "Gpon Phy Status Pll Ready Tx"},
	[mvAsicReg_PON_SERDES_PHY_STATUS_INIT_DONE]       =  { mvAsicReg_PON_SERDES_PHY_STATUS_INIT_DONE,       0x184FC,                         0x184FC,  asicRW,     0x00000001,    19,      0,     0,      0,     "Gpon Phy Status Rx Init Done"},

	[mvAsicReg_PON_SERDES_CLK_SEL]                    =  { mvAsicReg_PON_SERDES_CLK_SEL,                    0x18754,                         0x18754,  asicRW,     0x00000001,     1,      0,     0,      0,     "Gpon Serdes Clock Select"},
	[mvAsicReg_PON_SERDES_CLK_EN]                     =  { mvAsicReg_PON_SERDES_CLK_EN,                     0x18754,                         0x18754,  asicRW,     0x00000001,     2,      0,     0,      0,     "Gpon Serdes Clock Enable"},
	[mvAsicReg_PON_MAC_CLK_SEL]                       =  { mvAsicReg_PON_MAC_CLK_SEL,                       0x18754,                         0x18754,  asicRW,     0x00000001,     3,      0,     0,      0,     "Gpon MAC Clock Select"},
	[mvAsicReg_PON_REF_CLK_EN]                        =  { mvAsicReg_PON_REF_CLK_EN,                        0x18754,                         0x18754,  asicRW,     0x00000001,     4,      0,     0,      0,     "Gpon Ref Clock Enable"},

	[mvAsicReg_PON_MAC_GPON_CLK_EN]                   =  { mvAsicReg_PON_MAC_GPON_CLK_EN,                   0x18220,                         0x18220,  asicRW,     0x00000001,     1,      0,     0,      0,     "Gpon MAC Gpon T-Clock Enable"},
	[mvAsicReg_PON_MAC_SW_RESET_CTRL]                 =  { mvAsicReg_PON_MAC_SW_RESET_CTRL,                 0x18258,                         0x18258,  asicRW,     0x00000001,     3,      0,     0,      0,     "Gpon MAC Software Reset Control"},

	[mvAsicReg_PON_SERDES_INTERNAL_POWER_REG_0]       =  { mvAsicReg_PON_SERDES_INTERNAL_POWER_REG_0,       0x32004,                         0x32004,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Gpon Serded Internal - Power reg 0"},
	[mvAsicReg_PON_SERDES_INTERNAL_REG_D]             =  { mvAsicReg_PON_SERDES_INTERNAL_REG_D,             0x32034,                         0x32034,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Gpon Serded Internal - Reg 0xD"},
	[mvAsicReg_PON_SERDES_INTERNAL_REG_23_LOOP_TIMING] = { mvAsicReg_PON_SERDES_INTERNAL_REG_23_LOOP_TIMING, 0x3208c,                        0x3208c,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Gpon Serded Internal - Loop Timing"},
	[mvAsicReg_PON_SERDES_INTERNAL_REG_26_TX_DRV_IDLE] = { mvAsicReg_PON_SERDES_INTERNAL_REG_26_TX_DRV_IDLE, 0x32098,                        0x32098,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Gpon Serded Internal - Tx Driver Idle"},
	[mvAsicReg_PON_SERDES_INTERNAL_REG_3D]            =  { mvAsicReg_PON_SERDES_INTERNAL_REG_3D,            0x320f4,                         0x320f4,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Gpon Serded Internal - Reg 0x3D"},
	[mvAsicReg_PON_SERDES_INTERNAL_REG_4D]            =  { mvAsicReg_PON_SERDES_INTERNAL_REG_4D,            0x32134,                         0x32134,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Gpon Serded Internal - Reg 0x4D"},
	[mvAsicReg_PON_SERDES_INTERNAL_REG_51]            =  { mvAsicReg_PON_SERDES_INTERNAL_REG_51,            0x32144,                         0x32144,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Gpon Serded Internal - Reg 0x51"},
	[mvAsicReg_PON_SERDES_INTERNAL_REG_OFFSET_148]    =  { mvAsicReg_PON_SERDES_INTERNAL_REG_OFFSET_148,    0x32148,                         0x32148,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Gpon Serded Internal - Offse 0x32148"},
	[mvAsicReg_PON_SERDES_INTERNAL_REG_55_BIT6_7]     =  { mvAsicReg_PON_SERDES_INTERNAL_REG_55_BIT6_7,     0x32154,                         0x32154,  asicRW,     0x00000003,     6,      0,     0,      0,     "Gpon Serded Internal - Reg 0x54 bit6 and bit7"},
	[mvAsicReg_PON_SERDES_INTERNAL_REG_OFFSET_178]    =  { mvAsicReg_PON_SERDES_INTERNAL_REG_OFFSET_178,    0x32178,                         0x32178,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Gpon Serded Internal - Offse 0x32178"},

	[mvAsicReg_PON_XVR_TX_DATA_OUT_17]                =  { mvAsicReg_PON_XVR_TX_DATA_OUT_17,                0x18104,                         0x18104,  asicRW,     0x00000001,    17,      0,     0,      0,     "GPIO 17 data out transmit"},
	[mvAsicReg_PON_XVR_TX_DATA_OUT_36]                =  { mvAsicReg_PON_XVR_TX_DATA_OUT_36,                0x18144,                         0x18144,  asicRW,     0x00000001,    4,       0,     0,      0,     "GPIO 36 data out transmit"},
	[mvAsicReg_PON_XVR_TX_DATA_OUT_37]                =  { mvAsicReg_PON_XVR_TX_DATA_OUT_37,                0x18144,                         0x18144,  asicRW,     0x00000001,    5,       0,     0,      0,     "GPIO 37 data out transmit"},
	[mvAsicReg_PON_XVR_TX_DATA_OUT_41]                =  { mvAsicReg_PON_XVR_TX_DATA_OUT_41,                0x18144,                         0x18144,  asicRW,     0x00000001,    9,       0,     0,      0,     "GPIO 41 data out transmit"},
	[mvAsicReg_PON_XVR_TX_DATA_OUT_45]                =  { mvAsicReg_PON_XVR_TX_DATA_OUT_45,                0x18144,                         0x18144,  asicRW,     0x00000001,    13,      0,     0,      0,     "GPIO 45 data out transmit"},
	[mvAsicReg_PON_XVR_TX_DATA_OUT_65]                =  { mvAsicReg_PON_XVR_TX_DATA_OUT_65,                0x18184,                         0x18184,  asicRW,     0x00000001,    1,       0,     0,      0,     "GPIO 68 data out transmit"},
	[mvAsicReg_PON_XVR_TX_DATA_OUT_68]                =  { mvAsicReg_PON_XVR_TX_DATA_OUT_68,                0x18184,                         0x18184,  asicRW,     0x00000001,    4,       0,     0,      0,     "GPIO 68 data out transmit"},
	[mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_17]           =  { mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_17,           0x18100,                         0x18100,  asicRW,     0x00000001,    17,      0,     0,      0,     "GPIO 17 data out enabled"},
	[mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_36]           =  { mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_36,           0x18140,                         0x18140,  asicRW,     0x00000001,    4,       0,     0,      0,     "GPIO 36 data out enabled"},
	[mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_37]           =  { mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_37,           0x18140,                         0x18140,  asicRW,     0x00000001,    5,       0,     0,      0,     "GPIO 37 data out enabled"},
	[mvAsicReg_PON_XVR_RX_INPUT_41]                   =  { mvAsicReg_PON_XVR_RX_INPUT_41,                   0x18150,                         0x18150,  asicRW,     0x00000001,    9,       0,     0,      0,     "GPIO 41 data in "},
	[mvAsicReg_PON_XVR_RX_INPUT_45]                   =  { mvAsicReg_PON_XVR_RX_INPUT_45,                   0x18150,                         0x18150,  asicRW,     0x00000001,    13,      0,     0,      0,     "GPIO 45 data in "},
	[mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_65]           =  { mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_65,           0x18180,                         0x18180,  asicRW,     0x00000001,    1,       0,     0,      0,     "GPIO 65 data out enabled"},
	[mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_68]           =  { mvAsicReg_PON_XVR_TX_OUTPUT_ENABLE_68,           0x18180,                         0x18180,  asicRW,     0x00000001,    4,       0,     0,      0,     "GPIO 68 data out enabled"},
	[mvAsicReg_PON_SERDES_INTERNAL_OPEN_TX_DOOR_15]   =  { mvAsicReg_PON_SERDES_INTERNAL_OPEN_TX_DOOR_15,   0xA2E98,                         0xA2E98,  asicRW,     0x00000001,    15,      0,     0,      0,     "Gpon Serded Internal - open tx door"},
	[mvAsicReg_PON_SERDES_POWER_PHY_MODE]             =  { mvAsicReg_PON_SERDES_POWER_PHY_MODE,             0xA2E04,                         0xA2E04,  asicRW,     0x0000FFFF,    0,       0,     0,      0,     "Gpon Serded power up in PHY mode"},
	[mvAsicReg_PON_SYS_LED_ENABLE]                    =  { mvAsicReg_PON_SYS_LED_ENABLE,                    0x18144,                         0x18144,  asicRW,     0x00000001,    12,      0,     0,      0,     "ONU Sys Led enable"},
	[mvAsicReg_PON_SYS_LED_STATE]                     =  { mvAsicReg_PON_SYS_LED_STATE,                     0x18140,                         0x18140,  asicRW,     0x00000001,    12,      0,     0,      0,     "ONU Sys Led state"},
	[mvAsicReg_PON_SYS_LED_BLINK]                     =  { mvAsicReg_PON_SYS_LED_BLINK,                     0x18148,                         0x18148,  asicRW,     0x00000001,    12,      0,     0,      0,     "ONU Sys Led blink"},
	[mvAsicReg_PON_SYS_LED_BLINK_FREQ]                =  { mvAsicReg_PON_SYS_LED_BLINK_FREQ,                0x18160,                         0x18160,  asicRW,     0x00000001,    12,      0,     0,      0,     "ONU Sys Led blink freq"},
	[mvAsicReg_PON_SYNC_LED_ENABLE]                   =  { mvAsicReg_PON_SYNC_LED_ENABLE,                   0x18144,                         0x18144,  asicRW,     0x00000001,    13,      0,     0,      0,     "ONU Sync Led enable"},
	[mvAsicReg_PON_SYNC_LED_STATE]                    =  { mvAsicReg_PON_SYNC_LED_STATE,                    0x18140,                         0x18140,  asicRW,     0x00000001,    13,      0,     0,      0,     "ONU Sync Led state"},
	[mvAsicReg_PON_SYNC_LED_BLINK]                    =  { mvAsicReg_PON_SYNC_LED_BLINK,                    0x18148,                         0x18148,  asicRW,     0x00000001,    13,      0,     0,      0,     "ONU Sync Led blink"},
	[mvAsicReg_PON_SYNC_LED_BLINK_FREQ]               =  { mvAsicReg_PON_SYNC_LED_BLINK_FREQ,               0x18160,                         0x18160,  asicRW,     0x00000001,    13,      0,     0,      0,     "ONU Sync Led blink freq"},
	[mvAsicReg_PON_LED_BLINK_FREQ_A_ON]               =  { mvAsicReg_PON_LED_BLINK_FREQ_A_ON,               0x181C0,                         0x181C0,  asicRW,     0x00000000,     0,      0,     0,      0,     "ONU Led Freq A On"},
	[mvAsicReg_PON_LED_BLINK_FREQ_A_OFF]              =  { mvAsicReg_PON_LED_BLINK_FREQ_A_OFF,              0x181C4,                         0x181C4,  asicRW,     0x00000000,     0,      0,     0,      0,     "ONU Led Freq A Off"},
	[mvAsicReg_PON_LED_BLINK_FREQ_B_ON]               =  { mvAsicReg_PON_LED_BLINK_FREQ_B_ON,               0x181C8,                         0x181C8,  asicRW,     0x00000000,     0,      0,     0,      0,     "ONU Led Freq B On"},
	[mvAsicReg_PON_LED_BLINK_FREQ_B_OFF]              =  { mvAsicReg_PON_LED_BLINK_FREQ_B_OFF,              0x181CC,                         0x181CC,  asicRW,     0x00000000,     0,      0,     0,      0,     "ONU Led Freq B Off"},
#ifndef PON_FPGA
#ifdef DISABLE_DG_FOR_AVANTA_LP
	[mvAsicReg_PON_DG_CTRL_EN]                        =  { mvAsicReg_PON_DG_CTRL_EN,                        0x18210,                         0x18210,  asicRW,     0x00000001,     0,      0,     0,      0,     "ONU Dying Gasp enable"},
	[mvAsicReg_PON_DG_CTRL_POLARITY]                  =  { mvAsicReg_PON_DG_CTRL_POLARITY,                  0x18210,                         0x18210,  asicRW,     0x00000001,     1,      0,     0,      0,     "ONU Dying Gasp polarity"},
	[mvAsicReg_PON_DG_THRESHOLD]                      =  { mvAsicReg_PON_DG_THRESHOLD,                      0x18214,                         0x18214,  asicRW,     0x00000000,     0,      0,     0,      0,     "ONU Dying Gasp threshold"},
#else
	[mvAsicReg_PON_DG_CTRL_EN]                        =  { mvAsicReg_PON_DG_CTRL_EN,                        0x18270,                         0x18270,  asicRW,     0x00000001,     0,      0,     0,      0,     "ONU Dying Gasp enable"},
	[mvAsicReg_PON_DG_CTRL_POLARITY]                  =  { mvAsicReg_PON_DG_CTRL_POLARITY,                  0x18270,                         0x18270,  asicRW,     0x00000001,     1,      0,     0,      0,     "ONU Dying Gasp polarity"},
	[mvAsicReg_PON_DG_THRESHOLD]                      =  { mvAsicReg_PON_DG_THRESHOLD,                      0x18274,                         0x18274,  asicRW,     0x00000000,     0,      0,     0,      0,     "ONU Dying Gasp threshold"},
#endif
#endif
	[mvAsicReg_PON_INTERNAL_DG]                       =  { mvAsicReg_PON_INTERNAL_DG,                       0x182E4,                         0x182E4,  asicRW,     0x0000007F,     0,      0,     0,      0,     "ONU internal Dying Gasp support"},

#ifdef CONFIG_AVANTA_LP
	[mvAsicReg_PT_PATTERN_SELECT]                     =  { mvAsicReg_PT_PATTERN_SELECT,                     0xA2E54,                         0x32054,  asicRW,     0x0000000F,     4,      0,     0,      0,     "PHY test pattern select"},
	[mvAsicReg_PT_PATTERN_ENABLED]                    =  { mvAsicReg_PT_PATTERN_ENABLED,                    0xA2E54,                         0x32054,  asicRW,     0x00000001,     15,     0,     0,      0,     "PHY test pattern enabled"},
	[mvAsicReg_PT_PATTERN_DATA]                       =  { mvAsicReg_PT_PATTERN_DATA,                       0xA2E6C,                         0x3206C,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test pattern data / user pattern [15:0]"},
	[mvAsicReg_PT_PATTERN_USER_DATA_01]               =  { mvAsicReg_PT_PATTERN_USER_DATA_01,               0xA2E68,                         0x32068,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test user pattern [31:16]"},
	[mvAsicReg_PT_PATTERN_USER_DATA_02]               =  { mvAsicReg_PT_PATTERN_USER_DATA_02,               0xA2E64,                         0x32064,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test user pattern [47:32]"},
	[mvAsicReg_PT_PATTERN_USER_DATA_03]               =  { mvAsicReg_PT_PATTERN_USER_DATA_03,               0xA2E60,                         0x32060,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test user pattern [63:48]"},
	[mvAsicReg_PT_PATTERN_USER_DATA_04]               =  { mvAsicReg_PT_PATTERN_USER_DATA_04,               0xA2E5C,                         0x3205C,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test user pattern [79:64]"},
	[mvAsicReg_PT_PATTERN_USER_DATA_05]               =  { mvAsicReg_PT_PATTERN_USER_DATA_05,               0xA2E58,                         0x32058,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test user pattern [96:80]"},
#else
	[mvAsicReg_PT_PATTERN_SELECT]                     =  { mvAsicReg_PT_PATTERN_SELECT,                     0xA2E54,                         0xA2E54,  asicRW,     0x00000007,     5,      0,     0,      0,     "PHY test pattern select"},
	[mvAsicReg_PT_PATTERN_ENABLED]                    =  { mvAsicReg_PT_PATTERN_ENABLED,                    0xA2E54,                         0xA2E54,  asicRW,     0x00000001,     15,     0,     0,      0,     "PHY test pattern enabled"},
	[mvAsicReg_PT_PATTERN_DATA]			  =  { mvAsicReg_PT_PATTERN_DATA,			0xA2E6C,			 0xA2E6C,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test pattern data / user pattern [15:0]"},
	[mvAsicReg_PT_PATTERN_USER_DATA_01]               =  { mvAsicReg_PT_PATTERN_USER_DATA_01,               0xA2E68,                         0xA2E68,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test user pattern [31:16]"},
	[mvAsicReg_PT_PATTERN_USER_DATA_02]               =  { mvAsicReg_PT_PATTERN_USER_DATA_02,               0xA2E64,                         0xA2E64,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test user pattern [47:32]"},
	[mvAsicReg_PT_PATTERN_USER_DATA_03]               =  { mvAsicReg_PT_PATTERN_USER_DATA_03,               0xA2E60,                         0xA2E60,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test user pattern [63:48]"},
	[mvAsicReg_PT_PATTERN_USER_DATA_04]               =  { mvAsicReg_PT_PATTERN_USER_DATA_04,               0xA2E5C,                         0xA2E5C,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test user pattern [79:64]"},
	[mvAsicReg_PT_PATTERN_USER_DATA_05]               =  { mvAsicReg_PT_PATTERN_USER_DATA_05,               0xA2E58,                         0xA2E58,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "PHY test user pattern [96:80]"},
#endif
	[mvAsicReg_GUNIT_TX_0_PKT_MOD_MAX_HEAD_SIZE_CFG]  =  { mvAsicReg_GUNIT_TX_0_PKT_MOD_MAX_HEAD_SIZE_CFG,  0xA59D0,                         0xA59D0,  asicRW,     0x000000FE,     1,      0,     0,      0,     "GPON GUNIT Pkt Mod Mac Head Size for TX 0"},
	[mvAsicReg_GUNIT_TX_1_PKT_MOD_MAX_HEAD_SIZE_CFG]  =  { mvAsicReg_GUNIT_TX_1_PKT_MOD_MAX_HEAD_SIZE_CFG,  0xA5DD0,                         0xA5DD0,  asicRW,     0x000000FE,     1,      0,     0,      0,     "GPON GUNIT Pkt Mod Mac Head Size for TX 1"},
	[mvAsicReg_GUNIT_TX_2_PKT_MOD_MAX_HEAD_SIZE_CFG]  =  { mvAsicReg_GUNIT_TX_2_PKT_MOD_MAX_HEAD_SIZE_CFG,  0xA79D0,                         0xA79D0,  asicRW,     0x000000FE,     1,      0,     0,      0,     "GPON GUNIT Pkt Mod Mac Head Size for TX 2"},
	[mvAsicReg_GUNIT_TX_3_PKT_MOD_MAX_HEAD_SIZE_CFG]  =  { mvAsicReg_GUNIT_TX_3_PKT_MOD_MAX_HEAD_SIZE_CFG,  0xA7DD0,                         0xA7DD0,  asicRW,     0x000000FE,     1,      0,     0,      0,     "GPON GUNIT Pkt Mod Mac Head Size for TX 3"},
	[mvAsicReg_GUNIT_TX_4_PKT_MOD_MAX_HEAD_SIZE_CFG]  =  { mvAsicReg_GUNIT_TX_4_PKT_MOD_MAX_HEAD_SIZE_CFG,  0xA99D0,                         0xA99D0,  asicRW,     0x000000FE,     1,      0,     0,      0,     "GPON GUNIT Pkt Mod Mac Head Size for TX 4"},
	[mvAsicReg_GUNIT_TX_5_PKT_MOD_MAX_HEAD_SIZE_CFG]  =  { mvAsicReg_GUNIT_TX_5_PKT_MOD_MAX_HEAD_SIZE_CFG,  0xA9DD0,                         0xA9DD0,  asicRW,     0x000000FE,     1,      0,     0,      0,     "GPON GUNIT Pkt Mod Mac Head Size for TX 5"},
	[mvAsicReg_GUNIT_TX_6_PKT_MOD_MAX_HEAD_SIZE_CFG]  =  { mvAsicReg_GUNIT_TX_6_PKT_MOD_MAX_HEAD_SIZE_CFG,  0xAB9D0,                         0xAB9D0,  asicRW,     0x000000FE,     1,      0,     0,      0,     "GPON GUNIT Pkt Mod Mac Head Size for TX 6"},
	[mvAsicReg_GUNIT_TX_7_PKT_MOD_MAX_HEAD_SIZE_CFG]  =  { mvAsicReg_GUNIT_TX_7_PKT_MOD_MAX_HEAD_SIZE_CFG,  0xABDD0,                         0xABDD0,  asicRW,     0x000000FE,     1,      0,     0,      0,     "GPON GUNIT Pkt Mod Mac Head Size for TX 7"},

	[mvAsicReg_GUNIT_TX_0_PKT_MOD_STATS_PKT_COUNT]    =  { mvAsicReg_GUNIT_TX_0_PKT_MOD_STATS_PKT_COUNT,    0xA59D8,                         0xA59D8,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "GPON GUNIT Pkt Mod Packet Count for TX 0"},
	[mvAsicReg_GUNIT_TX_1_PKT_MOD_STATS_PKT_COUNT]    =  { mvAsicReg_GUNIT_TX_1_PKT_MOD_STATS_PKT_COUNT,    0xA5DD8,                         0xA5DD8,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "GPON GUNIT Pkt Mod Packet Count for TX 1"},
	[mvAsicReg_GUNIT_TX_2_PKT_MOD_STATS_PKT_COUNT]    =  { mvAsicReg_GUNIT_TX_2_PKT_MOD_STATS_PKT_COUNT,    0xA79D8,                         0xA79D8,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "GPON GUNIT Pkt Mod Packet Count for TX 2"},
	[mvAsicReg_GUNIT_TX_3_PKT_MOD_STATS_PKT_COUNT]    =  { mvAsicReg_GUNIT_TX_3_PKT_MOD_STATS_PKT_COUNT,    0xA7DD8,                         0xA7DD8,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "GPON GUNIT Pkt Mod Packet Count for TX 3"},
	[mvAsicReg_GUNIT_TX_4_PKT_MOD_STATS_PKT_COUNT]    =  { mvAsicReg_GUNIT_TX_4_PKT_MOD_STATS_PKT_COUNT,    0xA99D8,                         0xA99D8,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "GPON GUNIT Pkt Mod Packet Count for TX 4"},
	[mvAsicReg_GUNIT_TX_5_PKT_MOD_STATS_PKT_COUNT]    =  { mvAsicReg_GUNIT_TX_5_PKT_MOD_STATS_PKT_COUNT,    0xA9DD8,                         0xA9DD8,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "GPON GUNIT Pkt Mod Packet Count for TX 5"},
	[mvAsicReg_GUNIT_TX_6_PKT_MOD_STATS_PKT_COUNT]    =  { mvAsicReg_GUNIT_TX_6_PKT_MOD_STATS_PKT_COUNT,    0xAB9D8,                         0xAB9D8,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "GPON GUNIT Pkt Mod Packet Count for TX 6"},
	[mvAsicReg_GUNIT_TX_7_PKT_MOD_STATS_PKT_COUNT]    =  { mvAsicReg_GUNIT_TX_7_PKT_MOD_STATS_PKT_COUNT,    0xABDD8,                         0xABDD8,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "GPON GUNIT Pkt Mod Packet Count for TX 7"},

	[mvAsicReg_PPv2_TX_PON_BYTE_CNT_INDEX]            =  { mvAsicReg_PPv2_TX_PON_BYTE_CNT_INDEX,            0xF8100,                         0xF8100,  asicRW,     0x0000007F,     0,      0,     0,      0,     "Transmit PON Byte Counters Index (TPBC_INDEX)"},
	[mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG1_QUE_OVR]  =  { mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG1_QUE_OVR,  0xF8104,                         0xF8104,  asicRW,     0x000000FF,     0,      0,     0,      0,     "Transmit PON Byte Counters Config1 (TPBC_CFG1)"},
	[mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG1_PKT_MOD]  =  { mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG1_PKT_MOD,  0xF8104,                         0xF8104,  asicRW,     0x000000FF,     8,      0,     0,      0,     "Transmit PON Byte Counters Config1 (TPBC_CFG1)"},
	[mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG2_POST_FEC] =  { mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG2_POST_FEC, 0xF8108,                         0xF8108,  asicRW,     0x0000FFFF,     0,      0,     0,      0,     "Transmit PON Byte Counters Config2 (TPBC_CFG2)"},
	[mvAsicReg_PPv2_PON_FEC]                          =  { mvAsicReg_PPv2_PON_FEC,                          0xF0170,                         0xF0170,  asicRW,     0x00000001,     0,      0,     0,      0,     "PON FEC (PON_FEC)"},

	[mvAsicReg_C5000_BIT_1]                           =  { mvAsicReg_C5000_BIT_1,                           0xC5000,                         0xC5000,  asicRW,     0x00000001,     1,      0,     0,      0,     "0xC5000 bitfield 1"},
	[mvAsicReg_C5008_BIT_0]                           =  { mvAsicReg_C5008_BIT_0,                           0xC5008,                         0xC5008,  asicRW,     0x00000001,     0,      0,     0,      0,     "0xC5008 bitfield 0"},
	[mvAsicReg_C5008_BIT_3]                           =  { mvAsicReg_C5008_BIT_3,                           0xC5008,                         0xC5008,  asicRW,     0x00000001,     3,      0,     0,      0,     "0xC5008 bitfield 3"},

	[mvAsicReg_18900_BIT_14]                          =  { mvAsicReg_18900_BIT_14,                          0x18900,                         0x18900,  asicRW,     0x00000001,     14,     0,     0,      0,     "0x18900 bitfield 14"},
	[mvAsicReg_18900_BIT_19]                          =  { mvAsicReg_18900_BIT_19,                          0x18900,                         0x18900,  asicRW,     0x00000001,     19,     0,     0,      0,     "0x18900 bitfield 19"},
	[mvAsicReg_18900_BIT_21]                          =  { mvAsicReg_18900_BIT_21,                          0x18900,                         0x18900,  asicRW,     0x00000001,     14,     0,     0,      0,     "0x18900 bitfield 21"},

	[mvAsicReg_GMAC1_AUTONEG_SPEED]                   =  { mvAsicReg_GMAC1_AUTONEG_SPEED,                   0xC500C,                         0xC500C,  asicRW,     0x00000001,     7,      0,     0,      0,     "GMAC1 autoneg speed"},
	[mvAsicReg_GMAC1_AUTONEG_FLOW_CONTROL_MODE]       =  { mvAsicReg_GMAC1_AUTONEG_FLOW_CONTROL_MODE,       0xC500C,                         0xC500C,  asicRW,     0x00000001,     9,      0,     0,      0,     "GMAC1 autoneg flow control mode"},
	[mvAsicReg_GMAC1_AUTONEG_FLOW_CONTROL_ENABLE]     =  { mvAsicReg_GMAC1_AUTONEG_FLOW_CONTROL_ENABLE,     0xC500C,                         0xC500C,  asicRW,     0x00000001,     11,     0,     0,      0,     "GMAC1 autoneg flow control enable"},
	[mvAsicReg_GMAC1_AUTONEG_DUPLEX_MODE]             =  { mvAsicReg_GMAC1_AUTONEG_DUPLEX_MODE,             0xC500C,                         0xC500C,  asicRW,     0x00000001,     12,     0,     0,      0,     "GMAC1 autoneg duplex mode"},
	[mvAsicReg_GMAC1_AUTONEG_DUPLEX_ENABLE]           =  { mvAsicReg_GMAC1_AUTONEG_DUPLEX_ENABLE,           0xC500C,                         0xC500C,  asicRW,     0x00000001,     13,     0,     0,      0,     "GMAC1 autoneg duplex enable"},

	[mvAsicReg_PON_BYTE_CNT_IDX]                      =  { mvAsicReg_PON_BYTE_CNT_IDX,                      0xF8100,                         0xF8100,  asicRW,     0x0000003F,     0,      0,     0,      0,     "PON byte counter index for T-CONT/LLID and queue"},
	[mvAsicReg_PON_BYTE_CNT_CFG]                      =  { mvAsicReg_PON_BYTE_CNT_CFG,                      0xF8104,                         0xF8104,  asicRW,     0x000000FF,     0,      0,     0,      0,     "PON byte counter configuration"},
};

/* Export Functions
   ------------------------------------------------------------------------------*/

/* Local Functions
   ------------------------------------------------------------------------------*/
MV_STATUS asicOntGlbRegValidation(E_asicGlobalRegs reg,
				  S_asicGlobalRegDb **pRegEntry,
				  MV_U32 entry,
				  MV_U32 accessType);

/*******************************************************************************
**
**  asicOntGlbAddrSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: Set ASIC global address
**
**  PARAMETERS:  MV_U32 address
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK
**
*******************************************************************************/
MV_STATUS ponOnuGlbAddrSet(MV_U32 address)
{
	ponOnuGlbAddr = address;

	return MV_OK;
}

/*******************************************************************************
**
**  asicOntGlbRegValidation
**  ____________________________________________________________________________
**
**  DESCRIPTION: Validate ASIC register
**
**  PARAMETERS:  E_asicGlobalRegs  reg,
**               S_asicGlobalRegDb **pRegEntry
**               MV_U32            entry
**               MV_U32            accessType
**
**  OUTPUTS:     S_asicGlobalRegDb **pRegEntry
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS asicOntGlbRegValidation(E_asicGlobalRegs reg,
				  S_asicGlobalRegDb **pRegEntry,
				  MV_U32 entry,
				  MV_U32 accessType)
{
	/* Check range */
	if (reg >= mvAsicReg_MAX_NUM_OF_REGS)
		return MV_ERROR;

	/* Get the register entry in DB */
	*pRegEntry = &(asicGlbDb[reg]);

	/* Check DB coherency */
	if ((*pRegEntry)->enumVal != reg)
		return MV_ERROR;

	/* Check access type */
	if (accessType == funcRegR) { /* Read */
		if ((*pRegEntry)->accessType == asicWO)
			return MV_ERROR;
	} else if (accessType == funcRegW) /* Write */
		if ((*pRegEntry)->accessType == asicRO)
			return MV_ERROR;

	/* Check table entry - only valid for tables */
	if (entry > (*pRegEntry)->tblLength)
		return MV_ERROR;

	/* Update the access count to this register */
	(*pRegEntry)->accessCount++;

	return MV_OK;
}

/*******************************************************************************
**
**  asicOntGlbRegRead
**  ____________________________________________________________________________
**
**  DESCRIPTION: Read the contents of MV_U32 ASIC register
**
**  PARAMETERS:  E_asicGlobalRegs reg
**               MV_U32           *pValue
**               MV_U32           entry
**
**  OUTPUTS:     a_value_p
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS asicOntGlbRegRead(E_asicGlobalRegs reg,
			    MV_U32          *pValue,
			    MV_U32 entry)
{
	MV_STATUS rcode;
	S_asicGlobalRegDb *pRegEntry = NULL;
	MV_U32 regAddr;
	MV_U32 baseAddr;
	MV_U32 regValue;

	rcode = asicOntGlbRegValidation(reg, &pRegEntry, entry, funcRegR);
	if (rcode != MV_OK)
		return rcode;

	baseAddr = pRegEntry->address;

	/* Absolute register address */
	regAddr = (baseAddr + (pRegEntry->tblEntrySize *
			       entry                   *
			       sizeof(MV_U32)));

	/* Read the value of the register */
	regValue = MV_REG_READ(ponOnuGlbAddr + regAddr);

	*pValue = ((regValue >> (pRegEntry->shift)) & (pRegEntry->mask));

	return MV_OK;
}

/*******************************************************************************
**
**  asicOntGlbRegWrite
**  ____________________________________________________________________________
**
**  DESCRIPTION: Write the contents of MV_U32 ASIC register
**
**  PARAMETERS:  E_asicGlobalRegs reg
**               MV_U32           value
**               MV_U32           entry
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS asicOntGlbRegWrite(E_asicGlobalRegs reg,
			     MV_U32 value,
			     MV_U32 entry)
{
	MV_STATUS rcode;
	S_asicGlobalRegDb *pRegEntry = NULL;
	MV_U32 regAddr;
	MV_U32 tempValue;
	MV_U32 baseAddr;

	rcode = asicOntGlbRegValidation(reg, &pRegEntry, entry, funcRegW);
	if (rcode != MV_OK)
		return rcode;

	baseAddr = pRegEntry->address;

	/* Absolute register address */
	regAddr = (baseAddr + (pRegEntry->tblEntrySize *
			       entry                   *
			       sizeof(MV_U32)));

	tempValue = MV_REG_READ(ponOnuGlbAddr + regAddr);

	/* Clear field */
	tempValue &= ~((pRegEntry->mask) << (pRegEntry->shift));

	/* Set field */
	tempValue |= (((value) & (pRegEntry->mask)) << (pRegEntry->shift));

	/* Write the value of the register */
	MV_REG_WRITE((ponOnuGlbAddr + regAddr), tempValue);

	return MV_OK;
}

/*******************************************************************************
**
**  asicOntGlbRegReadNoCheck
**  ____________________________________________________________________________
**
**  DESCRIPTION: Read fast (with no checkings) the contents of MV_U32 ASIC register
**
**  PARAMETERS:  E_asicGlobalRegs reg
**               MV_U32           *pValue
**               MV_U32           entry
**
**  OUTPUTS:     a_value_p
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS asicOntGlbRegReadNoCheck(E_asicGlobalRegs reg,
				   MV_U32         *pValue,
				   MV_U32 entry)
{
	S_asicGlobalRegDb *pRegEntry = NULL;
	MV_U32 regAddr;
	MV_U32 baseAddr;
	MV_U32 regValue;

	/* Check range */
	if (reg >= mvAsicReg_MAX_NUM_OF_REGS)
		return MV_ERROR;

	/* Get the register entry in DB */
	pRegEntry = &(asicGlbDb[reg]);

	/* Update the access count to this register */
	pRegEntry->accessCount++;

	baseAddr = pRegEntry->address;

	/* Absolute register address */
	regAddr = (baseAddr + (pRegEntry->tblEntrySize *
			       entry                   *
			       sizeof(MV_U32)));

	/* Read the value of the register */
	regValue = MV_REG_READ(ponOnuGlbAddr + regAddr);

	*pValue = ((regValue >> (pRegEntry->shift)) & (pRegEntry->mask));

	return MV_OK;
}

/*******************************************************************************
**
**  asicOntGlbRegWriteNoCheck
**  ____________________________________________________________________________
**
**  DESCRIPTION: Write fast (with no checkings) the contents of MV_U32 ASIC register
**
**  PARAMETERS:  E_asicGlobalRegs reg
**               MV_U32           value
**               MV_U32           entry
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS asicOntGlbRegWriteNoCheck(E_asicGlobalRegs reg,
				    MV_U32 value,
				    MV_U32 entry)
{
	S_asicGlobalRegDb *pRegEntry = NULL;
	MV_U32 regAddr;
	MV_U32 tempValue;
	MV_U32 baseAddr;

	/* Check range */
	if (reg >= mvAsicReg_MAX_NUM_OF_REGS)
		return MV_ERROR;

	/* Get the register entry in DB */
	pRegEntry = &(asicGlbDb[reg]);

	/* Update the access count to this register */
	pRegEntry->accessCount++;

	baseAddr = pRegEntry->address;

	/* Absolute register address */
	regAddr = (baseAddr + (pRegEntry->tblEntrySize *
			       entry                   *
			       sizeof(MV_U32)));

	tempValue = MV_REG_READ(ponOnuGlbAddr + regAddr);

	/* Clear field */
	tempValue &= ~((pRegEntry->mask) << (pRegEntry->shift));

	/* Set field */
	tempValue |= (((value) & (pRegEntry->mask)) << (pRegEntry->shift));

	/* Write the value of the register */
	MV_REG_WRITE((ponOnuGlbAddr + regAddr), tempValue);

	return MV_OK;
}

/*******************************************************************************
**
**  asicOntGlbSerdesRegWrite
**  ____________________________________________________________________________
**
**  DESCRIPTION: Write the contents of MV_U32 ASIC register
**
**  PARAMETERS:  E_asicGlobalRegs reg
**               MV_U32           value
**               MV_U32           entry
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS asicOntMiscRegWrite(E_asicGlobalRegs reg,
			      MV_U32 value,
			      MV_U32 entry)
{
	MV_STATUS rcode;
	S_asicGlobalRegDb *pRegEntry = NULL;
	MV_U32 regAddr;
	MV_U32 tempValue;
	MV_U32 baseAddr;

	rcode = asicOntGlbRegValidation(reg, &pRegEntry, entry, funcRegW);
	if (rcode != MV_OK)
		return rcode;

	baseAddr = pRegEntry->address;

	/* Absolute register address */
	regAddr = (baseAddr + (pRegEntry->tblEntrySize *
			       entry                   *
			       sizeof(MV_U32)));

	tempValue = MV_REG_READ(regAddr);

	/* Clear field */
	tempValue &= ~((pRegEntry->mask) << (pRegEntry->shift));

	/* Set field */
	tempValue |= (((value) & (pRegEntry->mask)) << (pRegEntry->shift));

	/* Write the value of the register */
	MV_REG_WRITE(regAddr, tempValue);

	return MV_OK;
}

/*******************************************************************************
**
**  asicOntMiscRegRead
**  ____________________________________________________________________________
**
**  DESCRIPTION: Read the contents of MV_U32 ASIC register
**
**  PARAMETERS:  E_asicGlobalRegs reg
**               MV_U32           *pValue
**               MV_U32           entry
**
**  OUTPUTS:     a_value_p
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS asicOntMiscRegRead(E_asicGlobalRegs reg,
			     MV_U32          *pValue,
			     MV_U32 entry)
{
	MV_STATUS rcode;
	S_asicGlobalRegDb *pRegEntry = NULL;
	MV_U32 regAddr;
	MV_U32 baseAddr;
	MV_U32 regValue;

	rcode = asicOntGlbRegValidation(reg, &pRegEntry, entry, funcRegR);
	if (rcode != MV_OK)
		return rcode;

	baseAddr = pRegEntry->address;

	/* Absolute register address */
	regAddr = (baseAddr + (pRegEntry->tblEntrySize *
			       entry                   *
			       sizeof(MV_U32)));

	/* Read the value of the register */
	regValue = MV_REG_READ(regAddr);

	*pValue = ((regValue >> (pRegEntry->shift)) & (pRegEntry->mask));

	return MV_OK;
}

/*******************************************************************************
**
**  asicOntMiscRegAddressGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: Get the address of MV_U32 ASIC register
**
**  PARAMETERS:  E_asicGlobalRegs reg
**               MV_U32           value
**               MV_U32           entry
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS asicOntMiscRegAddressGet(E_asicGlobalRegs reg,
				   MV_U32 entry,
				   MV_U32           *regAddr)
{
	MV_STATUS rcode;
	S_asicGlobalRegDb *pRegEntry = NULL;
	MV_U32 baseAddr;

	rcode = asicOntGlbRegValidation(reg, &pRegEntry, entry, funcRegW);
	if (rcode != MV_OK)
		return rcode;

	baseAddr = pRegEntry->address;

	/* Absolute register address */
	*regAddr = (baseAddr + (pRegEntry->tblEntrySize *
				entry                   *
				sizeof(MV_U32)));

	return MV_OK;
}
