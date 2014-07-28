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

#include "mvPtpRegs.h"

void mvPtpRegRead(int port, MV_U8 ptpBlock, MV_U8 ptpRegAddr, MV_U16 *ptpRegData)
{
	MV_U16 value;
	MV_U8 phy_port = port;

	/* If accessing global registers, port is 0xE or 0xF */
	if (phy_port > 3)
		phy_port = 0;

	/* check busy bit, must be 0 before start OP */
	for (;;) {
		value = MV_REG_READ(PTP_CMD_OFF(phy_port));
		if (!(value & PTP_BUSY_MASK))
			break;
	}

	/* Set busy to 1 */
	value = 0;
	value |= (1 << PTP_BUSY_OFF);
	MV_REG_WRITE(PTP_CMD_OFF(phy_port), value);

	/* Start operation */
	value |= PTP_OP_READ << PTP_OP_OFF;
	value |= port << PTP_PORT_OFF;
	value |= ptpBlock << PTP_BLOCK_OFF;
	value |= ptpRegAddr;
	MV_REG_WRITE(PTP_CMD_OFF(phy_port), value);

	/* check busy bit */
	for (;;) {
		value = MV_REG_READ(PTP_CMD_OFF(phy_port));
		if (!(value & PTP_BUSY_MASK))
			break;
	}

	/* Read the result */
	*ptpRegData = MV_REG_READ(PTP_DATA_OFF(phy_port));
}

void mvPtpRegWrite(int port, MV_U8 ptpBlock, MV_U8 ptpRegAddr, MV_U16 ptpRegData)
{
	MV_U16 value;
	MV_U8 phy_port = port;

	/* If accessing global registers, port is 0xE or 0xF */
	if (phy_port > 3)
		phy_port = 0;

	/* check busy bit, must be 0 before start OP */
	for (;;) {
		value = MV_REG_READ(PTP_CMD_OFF(phy_port));
		if (!(value & PTP_BUSY_MASK))
			break;
	}

	/* Set busy to 1 */
	value = 0;
	value |= (1 << PTP_BUSY_OFF);
	MV_REG_WRITE(PTP_CMD_OFF(phy_port), value);

	/* Prepare data */
	MV_REG_WRITE(PTP_DATA_OFF(phy_port), ptpRegData);
	/* Start operation */
	value |= PTP_OP_WRITE << PTP_OP_OFF;
	value |= port << PTP_PORT_OFF;
	value |= ptpBlock << PTP_BLOCK_OFF;
	value |= ptpRegAddr;
	MV_REG_WRITE(PTP_CMD_OFF(phy_port), value);
}

void mvPtpPowerDown(MV_U8 port)
{
	MV_U16 value;

	value &= ~PTP_RESET_BIT_MASK;
	MV_REG_WRITE(PTP_RESET_OFF(port), value);
}

void mvPtpPowerUp(MV_U8 port)
{
	MV_U16 value;

	value |= 1 << PTP_RESET_BIT_OFF;
	MV_REG_WRITE(PTP_RESET_OFF(port), value);
}

void mvPtpRefClockSourceSet(MV_U8 port, MV_U8 clkSource)
{
	MV_U16 value;

	value = MV_REG_READ(PTP_CLK_OFF(port));
	value &= ~PTP_CLK_SEL_MASK;
	value |= clkSource << PTP_CLK_SEL_OFF;
	MV_REG_WRITE(PTP_CLK_OFF(port), value);
}

void mvPtpETypeSet(MV_U8 port, MV_U16 type)
{
	mvPtpRegWrite(port, 0, PTP_GLOBAL_CFG0_OFF, type);
}

void mvPtpMsgIdTsEnSet(MV_U8 port, MV_U8 msgId)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_GLOBAL_CFG1_OFF, &value);
	value |= 1 << msgId;
	mvPtpRegWrite(port, 0, PTP_GLOBAL_CFG1_OFF, 1 << value);
}

void mvPtpTsArrPtrSet(MV_U8 port, MV_U8 msgId, MV_U8 arrNum)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_GLOBAL_CFG1_OFF, &value);

	if (arrNum == 0)
		value &= ~(1 << msgId);
	else
		value |= 1 << msgId;
	mvPtpRegWrite(port, 0, PTP_GLOBAL_CFG1_OFF, value);
}

void mvPtpTransSpecSet(MV_U8 port, MV_U8 protocol)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG0_OFF, &value);
	value &= ~PTP_TRANS_SPEC_MASK;
	value |= protocol << PTP_TRANS_SPEC_OFF;
	mvPtpRegWrite(port, 0, PTP_PORT_CFG0_OFF, value);
}

void mvPtpDisDsOverwrite(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG0_OFF, &value);
	value &= ~PTP_DIS_TS_OV_MASK;
	value |= 1 << PTP_DIS_TS_OV_OFF;
	mvPtpRegWrite(port, 0, PTP_PORT_CFG0_OFF, value);
}

void mvPtpEnableDsOverwrite(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG0_OFF, &value);
	value &= ~PTP_DIS_TS_OV_MASK;
	mvPtpRegWrite(port, 0, PTP_PORT_CFG0_OFF, value);
}

void mvPtpLogicDisable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG0_OFF, &value);
	value |= 1 << PTP_DISABLE_OFF;
	mvPtpRegWrite(port, 0, PTP_PORT_CFG0_OFF, value);
}

void mvPtpLogicEnable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG0_OFF, &value);
	value &= ~(1 << PTP_DISABLE_OFF);
	mvPtpRegWrite(port, 0, PTP_PORT_CFG0_OFF, value);
}

void mvPtpIpJumpSet(MV_U8 port, MV_U8 jump)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG1_OFF, &value);
	value &= ~PTP_IP_JUMP_MASK;
	value |= jump << PTP_IP_JUMP_OFF;
	mvPtpRegWrite(port, 0, PTP_PORT_CFG1_OFF, value);
}

void mvPtpEtJumpSet(MV_U8 port, MV_U8 jump)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG1_OFF, &value);
	value &= ~PTP_ET_JUMP_MASK;
	value |= jump << PTP_ET_JUMP_OFF;
	mvPtpRegWrite(port, 0, PTP_PORT_CFG1_OFF, value);
}

void mvPtpTsIntEnable()
{
}

void mvPtpArrIntEnable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG2_OFF, &value);
	value &= ~PTP_ARR_INT_ENA_MASK;
	value |= 1 << PTP_ARR_INT_ENA_OFF;
	mvPtpRegWrite(port, 0, PTP_PORT_CFG2_OFF, value);
}

void mvPtpArrIntDisable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG2_OFF, &value);
	value &= ~PTP_ARR_INT_ENA_MASK;
	mvPtpRegWrite(port, 0, PTP_PORT_CFG2_OFF, value);
}

void mvPtpDepIntEnable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG2_OFF, &value);
	value &= ~PTP_DEP_INT_ENA_MASK;
	value |= 1 << PTP_DEP_INT_ENA_OFF;
	mvPtpRegWrite(port, 0, PTP_PORT_CFG2_OFF, value);
}

void mvPtpDepIntDisable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_CFG2_OFF, &value);
	value &= ~PTP_DEP_INT_ENA_MASK;
	mvPtpRegWrite(port, 0, PTP_PORT_CFG2_OFF, value);
}

MV_U8 mvPtpArr0IntStatus(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS0_OFF, &value);
	return (value & PTP_ARR0_INT_STATUS_MASK) >> PTP_ARR0_INT_STATUS_OFF;
}

void mvPtpArr0TimeValidReset(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS0_OFF, &value);
	value &= ~PTP_ARR0_TIME_VALID_MASK;
	mvPtpRegWrite(port, 0, PTP_PORT_STATUS0_OFF, value);
}

MV_U8 mvPtpArr0TimeValidGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS0_OFF, &value);
	return (value & PTP_ARR0_TIME_VALID_MASK) >> PTP_ARR0_TIME_VALID_OFF;
}

MV_U32 mvPtpArr0TimeGet(MV_U8 port)
{
	MV_U16 value1;
	MV_U16 value2;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS1_OFF, &value1);
	mvPtpRegRead(port, 0, PTP_PORT_STATUS1_OFF + 1, &value2);
	return (value2 << 16) | value1;
}

MV_U16 mvPtpArr0SeqIdGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS2_OFF, &value);
	return value;
}

MV_U8 mvPtpArr1IntStatus(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS3_OFF, &value);
	return (value & PTP_ARR1_INT_STATUS_MASK) >> PTP_ARR1_INT_STATUS_OFF;
}

void mvPtpArr1TimeValidReset(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS3_OFF, &value);
	value &= ~PTP_ARR1_TIME_VALID_MASK;
	mvPtpRegWrite(port, 0, PTP_PORT_STATUS3_OFF, value);
}

MV_U8 mvPtpArr1TimeValidGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS3_OFF, &value);
	return (value & PTP_ARR1_TIME_VALID_MASK) >> PTP_ARR1_TIME_VALID_OFF;
}

MV_U32 mvPtpArr1TimeGet(MV_U8 port)
{
	MV_U16 value1;
	MV_U16 value2;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS4_OFF, &value1);
	mvPtpRegRead(port, 0, PTP_PORT_STATUS4_OFF + 1, &value2);
	return (value2 << 16) | value1;
}

MV_U16 mvPtpArr1SeqIdGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS5_OFF, &value);
	return value;
}

MV_U8 mvPtpDepIntStatus(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS6_OFF, &value);
	return (value & PTP_DEP_INT_STATUS_MASK) >> PTP_DEP_INT_STATUS_OFF;
}

void mvPtpDepTimeValidReset(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS6_OFF, &value);
	value &= ~PTP_DEP_TIME_VALID_MASK;
	mvPtpRegWrite(port, 0, PTP_PORT_STATUS6_OFF, value);
}

MV_U8 mvPtpDepTimeValidGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS6_OFF, &value);
	return (value & PTP_DEP_TIME_VALID_MASK) >> PTP_DEP_TIME_VALID_OFF;
}

MV_U32 mvPtpDepTimeGet(MV_U8 port)
{
	MV_U16 value1;
	MV_U16 value2;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS7_OFF, &value1);
	mvPtpRegRead(port, 0, PTP_PORT_STATUS7_OFF + 1, &value2);
	return (value2 << 16) | value1;
}

MV_U16 mvPtpDepSeqIdGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, PTP_PORT_STATUS8_OFF, &value);
	return value;
}

void mvPtpDiscardCntrsReset(MV_U8 port)
{
}

void mvPtpDiscardCntrsShow(MV_U8 port)
{
}

void mvTaiEventCapOvEnable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_OFF, &value);
	value |= 1 << TAI_EVT_CAP_OV_OFF;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_OFF, value);
}

void mvTaiEventCapOvDisable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_OFF, &value);
	value &= ~TAI_EVT_CAP_OV_MASK;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_OFF, value);
}

void mvTaiEventCntrStart(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_OFF, &value);
	value |= 1 << TAI_EVT_CNTR_START_OFF;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_OFF, value);
}

void mvTaiTrigGenIntEnable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_OFF, &value);
	value |= 1 << TAI_TRIG_GEN_INT_ENA_OFF;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_OFF, value);
}

void mvTaiTrigGenIntDisable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_OFF, &value);
	value &= ~TAI_TRIG_GEN_INT_ENA_MASK;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_OFF, value);
}

void mvTaiEventCapIntEnable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_OFF, &value);
	value |= 1 << TAI_EVT_CAP_INT_ENA_OFF;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_OFF, value);
}

void mvTaiEventCapIntDisable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_OFF, &value);
	value &= ~TAI_EVT_CAP_INT_ENA_MASK;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_OFF, value);
}

void mvTaiTimeIncDecEnable(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_OFF, &value);
	value |= 1 << TAI_TIME_INC_DEC_ENA_OFF;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_OFF, value);
}

void mvTaiTrigModeSet(MV_U8 port, MV_U8 mode)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_OFF, &value);
	value &= ~TAI_TRIG_MODE_MASK;
	value |= mode << TAI_TRIG_MODE_OFF;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_OFF, value);
}

void mvTaiTrigGenReq(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_OFF, &value);
	value |= 1 << TAI_TRIG_GEN_REQ_OFF;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_OFF, value);
}

void mvTaiTsClkPerSet(MV_U8 port, MV_U16 piso)
{
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG_REG_OFF, piso);
}

MV_U16 mvTaiTsClkPerGet(MV_U8 port)
{
	MV_U16 piso;
	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG_REG_OFF, &piso);
	return piso;
}

void mvTaiTrigGenAmtSet(MV_U8 port, MV_U16 amount)
{
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG0_OFF, amount);
}

MV_U16 mvTaiTrigGenAmtGet(MV_U8 port)
{
	MV_U16 amount;
	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG0_OFF, &amount);
	return amount;
}

void mvTaiTrigClkCmpSet(MV_U8 port, MV_U16 comp)
{
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG1_OFF, comp);
}

MV_U16 mvTaiTrigClkCmpGet(MV_U8 port)
{
	MV_U16 comp;
	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG1_OFF, &comp);
	return comp;
}

void mvTaiPulseWidthSet(MV_U8 port, MV_U16 width)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG2_OFF, &value);
	value &= ~TAI_PULSE_WIDTH_MASK;
	value |= width << TAI_PULSE_WIDTH_OFF;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG2_OFF, value);
}

MV_U16 mvTaiPulseWidthGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG2_OFF, &value);
	return (value & TAI_PULSE_WIDTH_MASK) >> TAI_PULSE_WIDTH_OFF;
}

void mvTaiTimeIncDecOpSet(MV_U8 port, MV_U8 op)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG2_OFF, &value);
	value &= ~TAI_TIME_OP_MASK;
	value |= op << TAI_TIME_OP_OFF;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG2_OFF, value);
}

void mvTaiTimeIncDecAmtSet(MV_U8 port, MV_U16 amount)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG2_OFF, &value);
	value &= ~TAI_TIME_AMt_MASK;
	value |= amount << TAI_TIME_AMT_OFF;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG2_OFF, value);
}

MV_U16 mvTaiTimeIncDecAmtGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG2_OFF, &value);
	return (value & TAI_TIME_AMt_MASK) >> TAI_TIME_AMT_OFF;
}

void mvTaiSocClkPerSet(MV_U8 port, MV_U16 unit)
{
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG3_OFF, unit & TAI_SOC_CLK_PER_MASK);
}

MV_U16 mvTaiSocClkPerGet(MV_U8 port)
{
	MV_U16 value;
	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG3_OFF, &value);
	return value & TAI_SOC_CLK_PER_MASK;
}

void mvTaiSocClkCompSet(MV_U8 port, MV_U16 pico)
{
	mvPtpRegWrite(port, 0, TAI_GLOBAL_CFG4_OFF, pico);
}

MV_U16 mvTaiSocClkCompGet(MV_U8 port)
{
	MV_U16 pico;
	mvPtpRegRead(port, 0, TAI_GLOBAL_CFG4_OFF, &pico);
	return pico;
}

MV_U8 mvTaiTriGenIntGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_STATUS0_OFF, &value);
	return (value & TAI_TRIG_GEN_INT_MASK) >> TAI_TRIG_GEN_INT_OFF;
}

MV_U8 mvTaiEvtCapIntGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_STATUS1_OFF, &value);
	return (value & TAI_EVT_INT_MASK) >> TAI_EVT_INT_OFF;
}

MV_U8 mvTaiEvtCapErrGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_STATUS1_OFF, &value);
	return (value & TAI_EVT_CAP_ERR_MASK) >> TAI_EVT_CAP_ERR_OFF;
}

void mvTaiEvtCapValidReset(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_STATUS1_OFF, &value);
	value &= ~TAI_EVT_CAP_VALID_MASK;
	mvPtpRegWrite(port, 0, TAI_GLOBAL_STATUS1_OFF, value);
}

MV_U8 mvTaiEvtCapValidGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_STATUS1_OFF, &value);
	return (value & TAI_EVT_CAP_VALID_MASK) >> TAI_EVT_CAP_VALID_OFF;
}

MV_U8 mvTaiEvtCapCntrGet(MV_U8 port)
{
	MV_U16 value;

	mvPtpRegRead(port, 0, TAI_GLOBAL_STATUS1_OFF, &value);
	return (value & TAI_EVT_CAP_CNTR_MASK) >> TAI_EVT_CAP_CNTR_OFF;
}

MV_U32 mvPtpEventCapTimeGet(MV_U8 port)
{
	MV_U16 value1;
	MV_U16 value2;

	mvPtpRegRead(port, 0, TAI_GLOBAL_STATUS2_OFF, &value1);
	mvPtpRegRead(port, 0, TAI_GLOBAL_STATUS2_OFF + 1, &value2);
	return (value2 << 16) | value1;
}

MV_U32 mvPtpGlobalTimerGet(MV_U8 port)
{
	MV_U16 value1;
	MV_U16 value2;

	mvPtpRegRead(port, 0, PTP_GLOBAL_TIMER_OFF, &value1);
	mvPtpRegRead(port, 0, PTP_GLOBAL_TIMER_OFF + 1, &value2);
	return (value2 << 16) | value1;
}
