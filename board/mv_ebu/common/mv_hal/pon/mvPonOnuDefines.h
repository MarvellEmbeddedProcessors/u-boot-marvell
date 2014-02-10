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

#ifndef _ONU_GPON_DEFINES_H
#define _ONU_GPON_DEFINES_H

/* Include Files
   ------------------------------------------------------------------------------*/

/* Definitions
   ------------------------------------------------------------------------------*/

#define MV_6601_DEV_ID  0x11 /* TO BE REMOVED */

#ifdef CONFIG_PON_Z1
#define PON_Z1
#endif /* CONFIG_PON_Z1 */

#ifdef CONFIG_PON_A0
#define PON_A0
#endif /* CONFIG_PON_A0 */

#ifdef CONFIG_PON_FPGA
#define PON_FPGA
#endif /* CONFIG_PON_FPGA */

#define ONU_GPON_MAX_NUM_OF_T_CONTS (CONFIG_MV_PON_TCONTS)
#define EPON_MAX_MAC_NUM (CONFIG_MV_EPON_LLID)
#if defined(ONU_GPON_MAX_NUM_OF_T_CONTS) && (ONU_GPON_MAX_NUM_OF_T_CONTS < EPON_MAX_MAC_NUM)
#error ERROR: Invalid Init Params - #num of T-Conts < #num of LLID
#endif

#ifdef CONFIG_MV_GPON

#ifdef CONFIG_MV_GPON_STATIC_GEM_PORT
#define MV_GPON_STATIC_GEM_PORT
#endif /* CONFIG_MV_GPON_STATIC_GEM_PORT */

#ifdef CONFIG_MV_GPON_DEBUG_PRINT
#define MV_GPON_DEBUG_PRINT
#endif /* CONFIG_MV_GPON_DEBUG_PRINT */

#ifdef CONFIG_MV_GPON_PERFORMANCE_CHECK
#define MV_GPON_PERFORMANCE_CHECK
#endif /* CONFIG_MV_GPON_PERFORMANCE_CHECK */

#ifdef CONFIG_MV_GPON_HW_INTERRUPT
#define MV_GPON_HW_INTERRUPT
#endif  /* CONFIG_MV_GPON_HW_INTERRUPT */

#endif  /* CONFIG_MV_GPON */

#ifdef CONFIG_MV_EPON

#ifdef CONFIG_MV_EPON_HW_INTERRUPT
#define MV_EPON_HW_INTERRUPT
#endif /* CONFIG_MV_EPON_HW_INTERRUPT */

#define ONU_EPON_DBA_LOW_RATE_VALUE             (CONFIG_MV_EPON_DBA_LOW_RATE_VALUE)
#define ONU_EPON_DBA_LOW_RATE_THRESHOLD         (CONFIG_MV_EPON_DBA_LOW_RATE_THRESHOLD)
#define ONU_EPON_DBA_LOW_RATE_CONST_VALUE       (CONFIG_MV_EPON_DBA_LOW_RATE_CONST_VALUE)
#define ONU_EPON_DBA_HIGH_RATE_THRESHOLD        (CONFIG_MV_EPON_DBA_HIGH_RATE_THRESHOLD)
#define ONU_EPON_DBA_HIGH_RATE_VALUE            (CONFIG_MV_EPON_DBA_HIGH_RATE_VALUE)

#endif /* CONFIG_MV_EPON */

/* T-CONT mask */
#define ONU_GPON_TCONT_MASK                     (0xf)
#define ONU_GPON_TCONT_BIT_WIDTH                (4)  /* bit number used by T-CONT */

/* GEM MAC Ports */
#define GPON_ONU_MAC_OPEN_DS_BROADCAST_PORT     (1000)
#define GPON_ONU_MAC_SYNC_DS_BROADCAST_PORT     (500)
#define GPON_US_622_PLOU_TOTAL_BYTES_SIZE       (64 / 8)
#define GPON_US_1244_PLOU_TOTAL_BYTES_SIZE      (96 / 8)
#define GPON_ONU_MAX_GEM_PORTS                  (4096)

/* gpon definitions */
#define ONU_GPON_NUM_OF_EVENTS                  (64)
#define ONU_GPON_NUM_OF_STATES                  (8)
#define ONU_GPON_NUM_OF_ALARMS                  (14)

#define ONU_GPON_UNDEFINED_ONU_ID               (0xFF)
#define ONU_GPON_BROADCAST_ONU_ID               (0xFF)

#define ONU_GPON_DS_DEF_RATE                    (2488)  /* M Bit per sec */

/* BER definitions */
#define ONU_GPON_DEF_INTERNAL_BER_INTERVAL      (10)    /* 10 seconds */
#define ONU_GPON_MAX_INTERNAL_BER_INTERVAL      (10)    /* 10 seconds */
#define ONU_GPON_BER_DEF_COEFF_NUMERATOR        (1)
#define ONU_GPON_BER_DEF_COEFF_DENOMINATOR      (10)
#define ONU_GPON_DEF_SF_THRESHOLD               (5)
#define ONU_GPON_DEF_SD_THRESHOLD               (9)
#define ONU_GPON_MAX_SF_THRESHOLD               (9)
#define ONU_GPON_MIN_SF_THRESHOLD               (4)
#define ONU_GPON_MAX_SD_THRESHOLD               (8)
#define ONU_GPON_MIN_SD_THRESHOLD               (3)
#define ONU_GPON_DEF_REI_SEQ_NUM                (0)

/* overhead message status options */
#define ONU_GPON_OVER_MSG_STATUS_XX             (0xC0)
#define ONU_GPON_OVER_MSG_STATUS_E              (0x20)
#define ONU_GPON_OVER_MSG_STATUS_M              (0x10)
#define ONU_GPON_OVER_MSG_STATUS_SS             (0x0C)
#define ONU_GPON_OVER_MSG_STATUS_PP             (0x03)

/* Dying Gasp definitions */
#define ONU_GPON_DG_DEF_DEBUG_THRESH            (16000000)
#define ONU_GPON_DG_DEF_NORMAL_THRESH           (1600)
#define ONU_GPON_DG_DEF_PIN_NUMBER              (1)
#define ONU_GPON_DG_DEF_STATUS                  (0)
#define ONU_GPON_DG_ACTIVE_HIGH                 (1)
#define ONU_GPON_DG_ACTIVE_LOW                  (3)

/* port definitions */
#define ONU_GPON_PORT_ID_SHIFT                  (4)

/* States */
/* ====== */
#define ONU_GPON_01_INIT                        (0x01)
#define ONU_GPON_02_STANDBY                     (0x02)
#define ONU_GPON_03_SERIAL_NUM                  (0x03)
#define ONU_GPON_04_RANGING                     (0x04)
#define ONU_GPON_05_OPERATION                   (0x05)
#define ONU_GPON_06_POPUP                       (0x06)
#define ONU_GPON_07_EMERGANCY_STOP              (0x07)

/* Messages */
/* ======== */

/* Downstream */
#define ONU_GPON_DS_MSG_OVERHEAD                (0x01)
#define ONU_GPON_DS_MSG_SN_MASK                 (0x02)
#define ONU_GPON_DS_MSG_ASSIGN_ONU_ID           (0x03)
#define ONU_GPON_DS_MSG_RANGING_TIME            (0x04)
#define ONU_GPON_DS_MSG_DACT_ONU_ID             (0x05)
#define ONU_GPON_DS_MSG_DIS_SN                  (0x06)
#define ONU_GPON_DS_MSG_CONFIG_VP_VC            (0x07)
#define ONU_GPON_DS_MSG_ENCRYPT_VPI_PORT_ID     (0x08)
#define ONU_GPON_DS_MSG_REQ_PASSWORD            (0x09)
#define ONU_GPON_DS_MSG_ASSIGN_ALLOC_ID         (0x0A)
#define ONU_GPON_DS_MSG_NO_MESSAGE              (0x0B)
#define ONU_GPON_DS_MSG_POPUP                   (0x0C)
#define ONU_GPON_DS_MSG_REQ_KEY                 (0x0D)
#define ONU_GPON_DS_MSG_CONFIG_PORT_ID          (0x0E)
#define ONU_GPON_DS_MSG_PHYSICAL_EQUIP_ERR      (0x0F)
#define ONU_GPON_DS_MSG_CHANGE_POWER_LEVEL      (0x10)
#define ONU_GPON_DS_MSG_PST                     (0x11)
#define ONU_GPON_DS_MSG_BER_INTERVAL            (0x12)
#define ONU_GPON_DS_MSG_KEY_SWITCH_TIME         (0x13)
#define ONU_GPON_DS_MSG_EXT_BURST_LEN           (0x14)
#define ONU_GPON_DS_MSG_LAST                    (0x14)

/* Upstream */
#define ONU_GPON_US_MSG_SN_ONU                  (0x01)
#define ONU_GPON_US_MSG_PASSWORD                (0x02)
#define ONU_GPON_US_MSG_DYING_GASP              (0x03)
#define ONU_GPON_US_MSG_NO_MESSAGE              (0x04)
#define ONU_GPON_US_MSG_ENCRYPT_KEY             (0x05)
#define ONU_GPON_US_MSG_PHYSICAL_EQUIP_ERR      (0x06)
#define ONU_GPON_US_MSG_PST                     (0x07)
#define ONU_GPON_US_MSG_REI                     (0x08)
#define ONU_GPON_US_MSG_ACK                     (0x09)
#define ONU_GPON_US_MSG_LAST                    (0x09)

/* Alarms */
/* ====== */
#define ONU_GPON_EVENT_ALARM_LOS                (0x0)
#define ONU_GPON_EVENT_ALARM_LOF                (0x1)
#define ONU_GPON_EVENT_ALARM_LCD                (0x2)
#define ONU_GPON_EVENT_ALARM_LCDG               (0x3)
#define ONU_GPON_EVENT_ALARM_TF                 (0x4)
#define ONU_GPON_EVENT_ALARM_SUF                (0x5)
#define ONU_GPON_EVENT_ALARM_MEM                (0x6)
#define ONU_GPON_EVENT_ALARM_DACT               (0x7)
#define ONU_GPON_EVENT_ALARM_DIS                (0x8)
#define ONU_GPON_EVENT_ALARM_MIS                (0x9)
#define ONU_GPON_EVENT_ALARM_PEE                (0xA)
#define ONU_GPON_EVENT_ALARM_RDI                (0xB)

/* Generate alarm */
#define ONU_GPON_ALARM_GEN_BASE                 (0x20)

/* Cancel alarm */
#define ONU_GPON_ALARM_CAN_BASE                 (0x30)

/* Events */
/* ====== */

/* Messages */
#define ONU_GPON_EVENT_MSG_OVERHEAD             (ONU_GPON_DS_MSG_OVERHEAD)
#define ONU_GPON_EVENT_MSG_SN_MASK              (ONU_GPON_DS_MSG_SN_MASK)
#define ONU_GPON_EVENT_MSG_ASSIGN_ONU_ID        (ONU_GPON_DS_MSG_ASSIGN_ONU_ID)
#define ONU_GPON_EVENT_MSG_RANGING_TIME         (ONU_GPON_DS_MSG_RANGING_TIME)
#define ONU_GPON_EVENT_MSG_DACT_ONU_ID          (ONU_GPON_DS_MSG_DACT_ONU_ID)
#define ONU_GPON_EVENT_MSG_DIS_SN               (ONU_GPON_DS_MSG_DIS_SN)
#define ONU_GPON_EVENT_MSG_CONFIG_VP_VC         (ONU_GPON_DS_MSG_CONFIG_VP_VC)
#define ONU_GPON_EVENT_MSG_ENCRYPT_VPI_PORT_ID  (ONU_GPON_DS_MSG_ENCRYPT_VPI_PORT_ID)
#define ONU_GPON_EVENT_MSG_REQ_PASSWORD         (ONU_GPON_DS_MSG_REQ_PASSWORD)
#define ONU_GPON_EVENT_MSG_ASSIGN_ALLOC_ID      (ONU_GPON_DS_MSG_ASSIGN_ALLOC_ID)
#define ONU_GPON_EVENT_MSG_NO_MESSAGE           (ONU_GPON_DS_MSG_NO_MESSAGE)
#define ONU_GPON_EVENT_MSG_POPUP                (ONU_GPON_DS_MSG_POPUP)
#define ONU_GPON_EVENT_MSG_REQ_KEY              (ONU_GPON_DS_MSG_REQ_KEY)
#define ONU_GPON_EVENT_MSG_CONFIG_PORT_ID       (ONU_GPON_DS_MSG_CONFIG_PORT_ID)
#define ONU_GPON_EVENT_MSG_PHYSICAL_EQUIP_ERR   (ONU_GPON_DS_MSG_PHYSICAL_EQUIP_ERR)
#define ONU_GPON_EVENT_MSG_CHANGE_POWER_LEVEL   (ONU_GPON_DS_MSG_CHANGE_POWER_LEVEL)
#define ONU_GPON_EVENT_MSG_PST                  (ONU_GPON_DS_MSG_PST)
#define ONU_GPON_EVENT_MSG_BER_INTERVAL         (ONU_GPON_DS_MSG_BER_INTERVAL)
#define ONU_GPON_EVENT_MSG_KEY_SWITCH_TIME      (ONU_GPON_DS_MSG_KEY_SWITCH_TIME)
#define ONU_GPON_EVENT_MSG_EXT_BURST_LEN        (ONU_GPON_DS_MSG_EXT_BURST_LEN)

/* Alarms */
#define ONU_GPON_EVENT_ALM_GEN_BASE             (ONU_GPON_ALARM_GEN_BASE)
#define ONU_GPON_EVENT_ALM_GEN_LOS              (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_LOS )
#define ONU_GPON_EVENT_ALM_GEN_LOF              (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_LOF )
#define ONU_GPON_EVENT_ALM_GEN_LCDA             (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_LCDA)
#define ONU_GPON_EVENT_ALM_GEN_LCDG             (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_LCDG)
#define ONU_GPON_EVENT_ALM_GEN_TF               (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_TF  )
#define ONU_GPON_EVENT_ALM_GEN_SUF              (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_SUF )
#define ONU_GPON_EVENT_ALM_GEN_MEM              (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_MEM )
#define ONU_GPON_EVENT_ALM_GEN_DACT             (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_DACT)
#define ONU_GPON_EVENT_ALM_GEN_DIS              (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_DIS )
#define ONU_GPON_EVENT_ALM_GEN_MIS              (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_MIS )
#define ONU_GPON_EVENT_ALM_GEN_PEE              (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_PEE )
#define ONU_GPON_EVENT_ALM_GEN_RDI              (ONU_GPON_ALARM_GEN_BASE + ONU_GPON_EVENT_ALARM_RDI )

#define ONU_GPON_EVENT_ALM_CAN_BASE             (ONU_GPON_ALARM_CAN_BASE)
#define ONU_GPON_EVENT_ALM_CAN_LOS              (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_LOS )
#define ONU_GPON_EVENT_ALM_CAN_LOF              (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_LOF )
#define ONU_GPON_EVENT_ALM_CAN_LCDA             (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_LCDA)
#define ONU_GPON_EVENT_ALM_CAN_LCDG             (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_LCDG)
#define ONU_GPON_EVENT_ALM_CAN_TF               (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_TF  )
#define ONU_GPON_EVENT_ALM_CAN_SUF              (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_SUF )
#define ONU_GPON_EVENT_ALM_CAN_MEM              (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_MEM )
#define ONU_GPON_EVENT_ALM_CAN_DACT             (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_DACT)
#define ONU_GPON_EVENT_ALM_CAN_DIS              (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_DIS )
#define ONU_GPON_EVENT_ALM_CAN_MIS              (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_MIS )
#define ONU_GPON_EVENT_ALM_CAN_PEE              (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_PEE )
#define ONU_GPON_EVENT_ALM_CAN_RDI              (ONU_GPON_ALARM_CAN_BASE + ONU_GPON_EVENT_ALARM_RDI )

/* Default Serial Number - Before init time */
#define ONU_GPON_SN_DEF_BYTE_1                  (0x01)
#define ONU_GPON_SN_DEF_BYTE_2                  (0x02)
#define ONU_GPON_SN_DEF_BYTE_3                  (0x03)
#define ONU_GPON_SN_DEF_BYTE_4                  (0x04)
#define ONU_GPON_SN_DEF_BYTE_5                  (0x05)
#define ONU_GPON_SN_DEF_BYTE_6                  (0x06)
#define ONU_GPON_SN_DEF_BYTE_7                  (0x07)
#define ONU_GPON_SN_DEF_BYTE_8                  (0x08)

#define ONU_GPON_IDLE_MSG_DEF_BYTE_01           (ONU_GPON_UNDEFINED_ONU_ID) /* should be update after assign onu Id */
#define ONU_GPON_IDLE_MSG_DEF_BYTE_02           (ONU_GPON_US_MSG_NO_MESSAGE)
#define ONU_GPON_IDLE_MSG_DEF_BYTE_03           (0x01)
#define ONU_GPON_IDLE_MSG_DEF_BYTE_04           (0x02)
#define ONU_GPON_IDLE_MSG_DEF_BYTE_05           (0x03)
#define ONU_GPON_IDLE_MSG_DEF_BYTE_06           (0x04)
#define ONU_GPON_IDLE_MSG_DEF_BYTE_07           (0x05)
#define ONU_GPON_IDLE_MSG_DEF_BYTE_08           (0x06)
#define ONU_GPON_IDLE_MSG_DEF_BYTE_09           (0x07)
#define ONU_GPON_IDLE_MSG_DEF_BYTE_10           (0x08)
#define ONU_GPON_IDLE_MSG_DEF_BYTE_11           (0x09)
#define ONU_GPON_IDLE_MSG_DEF_BYTE_12           (0x0A)

#define ONU_GPON_SN_MSG_DEF_BYTE_01             (ONU_GPON_UNDEFINED_ONU_ID) /* should be update after assign onu Id */
#define ONU_GPON_SN_MSG_DEF_BYTE_02             (ONU_GPON_US_MSG_SN_ONU)
#define ONU_GPON_SN_MSG_DEF_BYTE_03             (ONU_GPON_SN_DEF_BYTE_1)
#define ONU_GPON_SN_MSG_DEF_BYTE_04             (ONU_GPON_SN_DEF_BYTE_2)
#define ONU_GPON_SN_MSG_DEF_BYTE_05             (ONU_GPON_SN_DEF_BYTE_3)
#define ONU_GPON_SN_MSG_DEF_BYTE_06             (ONU_GPON_SN_DEF_BYTE_4)
#define ONU_GPON_SN_MSG_DEF_BYTE_07             (ONU_GPON_SN_DEF_BYTE_5)
#define ONU_GPON_SN_MSG_DEF_BYTE_08             (ONU_GPON_SN_DEF_BYTE_6)
#define ONU_GPON_SN_MSG_DEF_BYTE_09             (ONU_GPON_SN_DEF_BYTE_7)
#define ONU_GPON_SN_MSG_DEF_BYTE_10             (ONU_GPON_SN_DEF_BYTE_8)
#define ONU_GPON_SN_MSG_DEF_BYTE_11             (0x00)
#define ONU_GPON_SN_MSG_DEF_BYTE_12             (0x05)     /* GEM Supported / Medium Power */

#define ONU_GPON_SN_MSK_GEN_ENA                 (MV_FALSE)
#define ONU_GPON_SN_MSK_DEF_MATCH               (MV_TRUE)

#define ONU_GPON_SN_DEFAULT                     { 'M', 'R', 'V', 'L', '9', '9', '9', '9' }
#define ONU_GPON_PASSWORD_DEFAULT               { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' }

#define ONU_DBA_DEF_HIGHEST_QUEUE               (7)
#define ONU_DBA_MAX_QUEUE                       (8)

/* MIB Counters - the follow are element numbers in array
 *******************************************************************************/
#define ONU_MIB_PM_GoodOctetsReceived_low           0           // (0x0)
#define ONU_MIB_PM_GoodOctetsReceived_high          1           // (0x4)
#define ONU_MIB_PM_BadOctetsReceived                2           // (0x8)
#define ONU_MIB_PM_MACTransError                    3           // (0xC)
#define ONU_MIB_PM_GoodFramesReceived               4           // (0x10)
#define ONU_MIB_PM_BadFramesReceived                5           // (0x14)
#define ONU_MIB_PM_BroadcastFramesReceived          6           // (0x18)
#define ONU_MIB_PM_MulticastFramesReceived          7           // (0x1C)
#define ONU_MIB_PM_Frames64Octets                   8           // (0x20)
#define ONU_MIB_PM_Frames65to127Octets              9           // (0x24)
#define ONU_MIB_PM_Frames128to255Octets             10          // (0x28)
#define ONU_MIB_PM_Frames256to511Octets             11          // (0x2C)
#define ONU_MIB_PM_Frames512to1023Octets            12          // (0x30)
#define ONU_MIB_PM_Frames1024toMaxOctets            13          // (0x34)
#define ONU_MIB_PM_GoodOctetsSent_low               14          // (0x38)
#define ONU_MIB_PM_GoodOctetsSent_high              15          // (0x3C)
#define ONU_MIB_PM_GoodFramesSent                   16          // (0x40)
#define ONU_MIB_PM_MulticastFramesSent              18          // (0x48)
#define ONU_MIB_PM_BroadcastFramesSent              19          // (0x4C)

/* MIB Counters - the follow are element numbers in array
 *******************************************************************************/
#define ONU_MIB_PM_RX_MIB_Control                   0           // (0x0)
#define ONU_MIB_PM_TX_MIB_Control                   1           // (0x4)

/* MIB Counter Control - valid,counterset,gemport
 *******************************************************************************/
#define MIB_COUNTER_CONTROL_VALID_BIT           (1)
#define MIB_COUNTER_CONTROL_INVALID_BIT         (0)
#define MAKE_MIB_COUNTER_CONTROL(v, c, g)         ((((v) & 0x1) << 17) | (((c) & 0x1F) << 12) | ((g) & 0xFFF))

/* Enums
   ------------------------------------------------------------------------------*/
typedef enum {
	ONU_GPON_PREM_TYPE_01        = 0,
	ONU_GPON_PREM_TYPE_02        = 1,
	ONU_GPON_PREM_TYPE_03        = 2
} E_OnuOverheadPreambleType;

typedef enum {
	ONU_GPON_DELM_BYTE_01        = 0,
	ONU_GPON_DELM_BYTE_02        = 1,
	ONU_GPON_DELM_BYTE_03        = 2
} E_OnuOverheadDelimiterByte;

typedef enum {
	ONU_GPON_PREM_TYPE_03_DEF    = 0,
	ONU_GPON_PREM_TYPE_03_CHNG   = 1
} E_OnuOverheadExtPreambleType3;

/* Typedefs
   ------------------------------------------------------------------------------*/

/* Global variables
   ------------------------------------------------------------------------------*/

/* Global functions
   ------------------------------------------------------------------------------*/

/* Macros
   ------------------------------------------------------------------------------*/

#endif /* _ONU_GPON_DEFINES_H */