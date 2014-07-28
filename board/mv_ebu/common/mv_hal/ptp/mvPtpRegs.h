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

#ifndef _MV_PTP_REG_H
#define _MV_PTP_REG_H

#define PTP_CMD_OFF(port) (((port == 0) || (port == 1)) ? 0x3E000 : 0x3C000)
#define PTP_DATA_OFF(port) (((port == 0) || (port == 1)) ? 0x3E008 : 0x3C008)
#define PTP_RESET_OFF(port) (((port == 0) || (port == 1)) ? 0x3E010 : 0x3C010)
#define PTP_CLK_OFF(port) (((port == 0) || (port == 1)) ? 0x3E018 : 0x3C018)

#define PTP_GLOBAL_CFG0_OFF 0x0
#define PTP_GLOBAL_CFG1_OFF 0x1
#define PTP_GLOBAL_CFG2_OFF 0x2
#define PTP_GLOBAL_CFG3_OFF 0x3
#define PTP_GLOBAL_STATUS0_OFF 0x8

#define PTP_PORT_CFG0_OFF 0x0
#define PTP_PORT_CFG1_OFF 0x1
#define PTP_PORT_CFG2_OFF 0x2
#define PTP_PORT_STATUS0_OFF 0x8
#define PTP_PORT_STATUS1_OFF 0x9
#define PTP_PORT_STATUS2_OFF 0xB
#define PTP_PORT_STATUS3_OFF 0xC
#define PTP_PORT_STATUS4_OFF 0xD
#define PTP_PORT_STATUS5_OFF 0xF
#define PTP_PORT_STATUS6_OFF 0x10
#define PTP_PORT_STATUS7_OFF 0x11
#define PTP_PORT_STATUS8_OFF 0x13
#define PTP_PORT_STATUS9_OFF 0x15

#define TAI_GLOBAL_CFG_OFF 0x0
#define TAI_GLOBAL_CFG_REG_OFF 0x1
#define TAI_GLOBAL_CFG0_OFF 0x2
#define TAI_GLOBAL_CFG1_OFF 0x4
#define TAI_GLOBAL_CFG2_OFF 0x5
#define TAI_GLOBAL_CFG3_OFF 0x6
#define TAI_GLOBAL_CFG4_OFF 0x7
#define TAI_GLOBAL_STATUS0_OFF 0x8
#define TAI_GLOBAL_STATUS1_OFF 0x9
#define TAI_GLOBAL_STATUS2_OFF 0xA
#define PTP_GLOBAL_TIMER_OFF 0xE

/* PTP Command Register */
#define PTP_BUSY_OFF 15
#define PTP_BUSY_MASK 0x8000
#define PTP_OP_OFF 12
#define PTP_OP_MASK 0x7000
#define PTP_PORT_OFF 8
#define PTP_PORT_MASK 0x0F00
#define PTP_BLOCK_OFF 5
#define PTP_BLOCK_MASK 0x00E0
#define PTP_ADDR_OFF 0
#define PTP_ADDR_MASK 0x001F

#define PTP_OP_NO_OP 0
#define PTP_OP_WRITE 3
#define PTP_OP_READ 4
#define PTP_OP_READ2 6

#define PTP_PORT_PTP 0xF
#define PTP_PORT_TAI 0xE

/* PTP Reset Register */
#define PTP_RESET_BIT_OFF 0
#define PTP_RESET_BIT_MASK 0x1

/* PTP Clock Select Register */
#define PTP_CLK_SEL_OFF 0
#define PTP_CLK_SEL_MASK 0x1
#define PTP_CLK_SEL_125M 0
#define PTP_CLK_SEL_EXT_PTP 1

/* PTP Global Status 0 */
#define PTP_INT_OFF 0
#define PTP_INT_MASK 0x3F

/* PTP Port Config 0 */
#define PTP_TRANS_SPEC_OFF 12
#define PTP_TRANS_SPEC_MASK 0xF000
#define PTP_DIS_TRANS_SPEC_CHECK_OFF 11
#define PTP_DIS_TRANS_SPEC_CHECK_MASK 0x800
#define PTP_DIS_TS_OV_OFF 1
#define PTP_DIS_TS_OV_MASK 0x2
#define PTP_DISABLE_OFF 0
#define PTP_DISABLE_MASK 0x1

#define PTP_TRANS_SPEC_1588 0x0
#define PTP_TRANS_SPEC_8021AS 0x1

/* PTP Port Config 1 */
#define PTP_IP_JUMP_OFF 8
#define PTP_IP_JUMP_MASK 0x3F00
#define PTP_ET_JUMP_OFF 0
#define PTP_ET_JUMP_MASK 0x1F

/* PTP Port Config 2 */
#define PTP_DEP_INT_ENA_OFF 1
#define PTP_DEP_INT_ENA_MASK 0x2
#define PTP_ARR_INT_ENA_OFF 0
#define PTP_ARR_INT_ENA_MASK 0x1

/* PTP Port Status 0 */
#define PTP_ARR0_INT_STATUS_OFF 1
#define PTP_ARR0_INT_STATUS_MASK 0x6
#define PTP_ARR0_TIME_VALID_OFF 0
#define PTP_ARR0_TIME_VALID_MASK 0x1

#define PTP_TS_INT_STATUS_NORMAL 0
#define PTP_TS_INT_STATUS_OV 1
#define PTP_TS_INT_STATUS_PEND 2

/* PTP Port Status 3 */
#define PTP_ARR1_INT_STATUS_OFF 1
#define PTP_ARR1_INT_STATUS_MASK 0x6
#define PTP_ARR1_TIME_VALID_OFF 0
#define PTP_ARR1_TIME_VALID_MASK 0x1

/* PTP Port Status 6 */
#define PTP_DEP_INT_STATUS_OFF 1
#define PTP_DEP_INT_STATUS_MASK 0x6
#define PTP_DEP_TIME_VALID_OFF 0
#define PTP_DEP_TIME_VALID_MASK 0x1

/* PTP Port Status 9 */
#define PTP_DEP_DIS_CNTR_OFF 12
#define PTP_DEP_DIS_CNTR_MASK 0xF000
#define PTP_NON_TS_DEP_CNTR_OFF 8
#define PTP_NON_TS_DEP_CNTR_MASK 0xF00
#define PTP_ARR_DIS_CNTR_OFF 4
#define PTP_ARR_DIS_CNTR_MASK 0xF0
#define PTP_NON_TS_ARR_CNTR_OFF 0
#define PTP_NON_TS_ARR_CNTR_MASK 0xF

/* TAI Global Config */
#define TAI_EVT_CAP_OV_OFF 15
#define TAI_EVT_CAP_OV_MASK 0x8000
#define TAI_EVT_CNTR_START_OFF 14
#define TAI_EVT_CNTR_START_MASK 0x4000
#define TAI_TRIG_GEN_INT_ENA_OFF 9
#define TAI_TRIG_GEN_INT_ENA_MASK 0x200
#define TAI_EVT_CAP_INT_ENA_OFF 8
#define TAI_EVT_CAP_INT_ENA_MASK 0x100
#define TAI_TIME_INC_DEC_ENA_OFF 3
#define TAI_TIME_INC_DEC_ENA_MASK 0x8
#define TAI_TRIG_MODE_OFF 1
#define TAI_TRIG_MODE_MASK 0x2
#define TAI_TRIG_GEN_REQ_OFF 0
#define TAI_TRIG_GEN_REQ_MASK 0x1

#define TAI_TRIG_MODE_PULSE 0x1
#define TAI_TRIG_MODE_CLOCK 0x0

/* TAI Global Config 2 */
#define TAI_PULSE_WIDTH_OFF 12
#define TAI_PULSE_WIDTH_MASK 0xF000
#define TAI_TIME_OP_OFF 11
#define TAI_TIME_OP_MASK 0x800
#define TAI_TIME_AMT_OFF 0
#define TAI_TIME_AMt_MASK 0x7FF

#define TAI_TIME_OP_INC 0x0
#define TAI_TIME_OP_DEC 0x1

/* TAI Global Config 3 */
#define TAI_SOC_CLK_PER_OFF 0
#define TAI_SOC_CLK_PER_MASK 0x1FF

/* TAI Global Status 0 */
#define TAI_TRIG_GEN_INT_OFF 15
#define TAI_TRIG_GEN_INT_MASK 0x8000

/* TAI Global Status 1 */
#define TAI_EVT_INT_OFF 15
#define TAI_EVT_INT_MASK 0x8000
#define TAI_EVT_CAP_ERR_OFF 9
#define TAI_EVT_CAP_ERR_MASK 0x200
#define TAI_EVT_CAP_VALID_OFF 8
#define TAI_EVT_CAP_VALID_MASK 0x100
#define TAI_EVT_CAP_CNTR_OFF 0
#define TAI_EVT_CAP_CNTR_MASK 0xFF

void mvPtpRegRead(int port, MV_U8 ptpBlock, MV_U8 ptpRegAddr, MV_U16 *ptpRegData);
void mvPtpRegWrite(int port, MV_U8 ptpBlock, MV_U8 ptpRegAddr, MV_U16 ptpRegData);
void mvPtpPowerDown(MV_U8 port);
void mvPtpPowerUp(MV_U8 port);
void mvPtpRefClockSourceSet(MV_U8 port, MV_U8 clkSource);
void mvPtpETypeSet(MV_U8 port, MV_U16 type);
void mvPtpMsgIdTsEnSet(MV_U8 port, MV_U8 msgId);
void mvPtpTsArrPtrSet(MV_U8 port, MV_U8 msgId, MV_U8 arrNum);
void mvPtpTransSpecSet(MV_U8 port, MV_U8 protocol);
void mvPtpDisDsOverwrite(MV_U8 port);
void mvPtpEnableDsOverwrite(MV_U8 port);
void mvPtpLogicDisable(MV_U8 port);
void mvPtpLogicEnable(MV_U8 port);
void mvPtpIpJumpSet(MV_U8 port, MV_U8 jump);
void mvPtpEtJumpSet(MV_U8 port, MV_U8 jump);
void mvPtpTsIntEnable(void);
void mvPtpArrIntEnable(MV_U8 port);
void mvPtpArrIntDisable(MV_U8 port);
void mvPtpDepIntEnable(MV_U8 port);
void mvPtpDepIntDisable(MV_U8 port);
MV_U8 mvPtpArr0IntStatus(MV_U8 port);
void mvPtpArr0TimeValidReset(MV_U8 port);
MV_U8 mvPtpArr0TimeValidGet(MV_U8 port);
MV_U32 mvPtpArr0TimeGet(MV_U8 port);
MV_U16 mvPtpArr0SeqIdGet(MV_U8 port);
MV_U8 mvPtpArr1IntStatus(MV_U8 port);
void mvPtpArr1TimeValidReset(MV_U8 port);
MV_U8 mvPtpArr1TimeValidGet(MV_U8 port);
MV_U32 mvPtpArr1TimeGet(MV_U8 port);
MV_U16 mvPtpArr1SeqIdGet(MV_U8 port);
MV_U8 mvPtpDepIntStatus(MV_U8 port);
void mvPtpDepTimeValidReset(MV_U8 port);
MV_U8 mvPtpDepTimeValidGet(MV_U8 port);
MV_U32 mvPtpDepTimeGet(MV_U8 port);
MV_U16 mvPtpDepSeqIdGet(MV_U8 port);
void mvPtpDiscardCntrsReset(MV_U8 port);
void mvPtpDiscardCntrsShow(MV_U8 port);
void mvTaiEventCapOvEnable(MV_U8 port);
void mvTaiEventCapOvDisable(MV_U8 port);
void mvTaiEventCntrStart(MV_U8 port);
void mvTaiTrigGenIntEnable(MV_U8 port);
void mvTaiTrigGenIntDisable(MV_U8 port);
void mvTaiEventCapIntEnable(MV_U8 port);
void mvTaiEventCapIntDisable(MV_U8 port);
void mvTaiTimeIncDecEnable(MV_U8 port);
void mvTaiTrigModeSet(MV_U8 port, MV_U8 mode);
void mvTaiTrigGenReq(MV_U8 port);
void mvTaiTsClkPerSet(MV_U8 port, MV_U16 piso);
MV_U16 mvTaiTsClkPerGet(MV_U8 port);
void mvTaiTrigGenAmtSet(MV_U8 port, MV_U16 amount);
MV_U16 mvTaiTrigGenAmtGet(MV_U8 port);
void mvTaiTrigClkCmpSet(MV_U8 port, MV_U16 comp);
MV_U16 mvTaiTrigClkCmpGet(MV_U8 port);
void mvTaiPulseWidthSet(MV_U8 port, MV_U16 width);
MV_U16 mvTaiPulseWidthGet(MV_U8 port);
void mvTaiTimeIncDecOpSet(MV_U8 port, MV_U8 op);
void mvTaiTimeIncDecAmtSet(MV_U8 port, MV_U16 amount);
MV_U16 mvTaiTimeIncDecAmtGet(MV_U8 port);
void mvTaiSocClkPerSet(MV_U8 port, MV_U16 unit);
MV_U16 mvTaiSocClkPerGet(MV_U8 port);
void mvTaiSocClkCompSet(MV_U8 port, MV_U16 pico);
MV_U16 mvTaiSocClkCompGet(MV_U8 port);
MV_U8 mvTaiTriGenIntGet(MV_U8 port);
MV_U8 mvTaiEvtCapIntGet(MV_U8 port);
MV_U8 mvTaiEvtCapErrGet(MV_U8 port);
void mvTaiEvtCapValidReset(MV_U8 port);
MV_U8 mvTaiEvtCapValidGet(MV_U8 port);
MV_U8 mvTaiEvtCapCntrGet(MV_U8 port);
MV_U32 mvPtpEventCapTimeGet(MV_U8 port);
MV_U32 mvPtpGlobalTimerGet(MV_U8 port);

#endif /* _MV_PTP_REG_H */
