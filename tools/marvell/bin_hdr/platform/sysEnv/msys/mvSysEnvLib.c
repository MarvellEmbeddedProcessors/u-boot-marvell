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

#include "mv_os.h"
#include "config_marvell.h"  	/* Required to identify SOC and Board */
#include "mvHighSpeedEnvSpec.h"
#include "mvSysEnvLib.h"
#include "mvCtrlPex.h"
#include "mv_seq_exec_ext.h"

#if defined(MV_MSYS_BC2)
#include "ddr3_msys_bc2.h"
#include "ddr3_msys_bc2_config.h"
#elif defined(MV_MSYS_AC3)
#include "ddr3_msys_ac3.h"
#include "ddr3_msys_ac3_config.h"
#elif defined(MV_MSYS_BOBK)
#include "ddr3_msys_bobk.h"
#include "ddr3_msys_bobk_config.h"
#endif

#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"
#include "printf.h"

static MV_U32 gBoardId = -1;
MV_U32 mvBoardIdGet(MV_VOID)
{
	if (gBoardId != -1)
		return gBoardId;

#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT

	#if defined CONFIG_ALLEYCAT3
		#ifdef CONFIG_CUSTOMER_BOARD_0
			gBoardId = AC3_CUSTOMER_BOARD_ID0;
		#elif CONFIG_CUSTOMER_BOARD_1
			gBoardId = AC3_CUSTOMER_BOARD_ID1;
		#endif
	#elif defined (CONFIG_BOBCAT2)
		#ifdef CONFIG_CUSTOMER_BOARD_0
			gBoardId = BC2_CUSTOMER_BOARD_ID0;
		#elif CONFIG_CUSTOMER_BOARD_1
			gBoardId = BC2_CUSTOMER_BOARD_ID1;
		#endif
	#elif defined (CONFIG_BOBK)
		#ifdef CONFIG_CUSTOMER_BOARD_0
			gBoardId = BOBK_CETUS_CUSTOMER_BOARD_ID0;
		#elif CONFIG_CUSTOMER_BOARD_1
			gBoardId = BOBK_CAELUM_CUSTOMER_BOARD_ID1;
		#endif
	#endif

#else	/* !CONFIG_CUSTOMER_BOARD_SUPPORT */

	MV_U8 readValue;

	if (mvBoardSarBoardIdGet(&readValue) != MV_OK) {
		mvPrintf("%s: Error obtaining Board ID from EEPROM (%d)\n", __func__, readValue);
		mvPrintf("%s: Setting default board to: %d\n", __func__, MV_DEFAULT_BOARD_ID);
		readValue = MV_DEFAULT_BOARD_ID;
	}
	readValue = readValue & (BOARD_ID_INDEX_MASK - 1);

	if (readValue >= MV_MARVELL_BOARD_NUM) {
		mvPrintf("%s: Error: read wrong board ID (%d)\n", __func__, readValue);
		mvPrintf("%s: Setting default board to: %d\n", __func__, MV_DEFAULT_BOARD_ID);
		readValue = MV_DEFAULT_BOARD_ID;
	}
	gBoardId = MARVELL_BOARD_ID_BASE + readValue;

#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */

	return gBoardId;
}

/*******************************************************************************
* mvBoardIdIndexGet
*
* DESCRIPTION:
*	returns an index for board arrays with direct memory access, according to board id
*
* INPUT:
*       boardId.
*
* OUTPUT:
*       direct access index for board arrays
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_U32 mvBoardIdIndexGet(MV_U32 boardId)
{
/* Marvell Boards use 0x10 (BC2) and 0x30 (AC3) as base for Board ID mask MSB to receive index for board ID*/
	return boardId & (BOARD_ID_INDEX_MASK - 1);
}


/*******************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
	return MV_BOARD_TCLK_200MHZ;
}

/* Use flagTwsiInit global flag to init the Twsi once */
static int flagTwsiInit = -1;
MV_STATUS mvHwsTwsiInitWrapper(MV_VOID)
{
	MV_TWSI_ADDR slave;

	if (flagTwsiInit == -1) {
		slave.type = ADDR7_BIT;
		slave.address = 0;
		mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);
		flagTwsiInit = 1;

	}
	return MV_OK;
}

/*******************************************************************************/
/* The Board Id is taken from the first address-value pair of the EEPROM initialization sequence
 * In order to support normal TWSI init sequence flow, the first pair of DWORDS on EEPROM
 * should contain an address (bytes 0-3) of some scratch pad register (for instance an UART SCR)
 * and a value (bytes 4-7), which will be partially interpreted as Board ID (bits[7:0] of byte 7)
 */
MV_STATUS mvBoardSarBoardIdGet(MV_U8 *value)
{
	MV_U8			boardId;
	MV_TWSI_SLAVE	twsiSlave;

	/* Initializing twsiSlave in order to read from the TWSI address */
	twsiSlave.slaveAddr.address = 0x50;	/* 0x50: address of init EEPROM */
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 7;

	/* in case the offset should be set to a TWSI slave which support
	 * 2 bytes offset, the offset setting will be done in 2 transactions.
	 * For accessing EEPROM, always using 2 bytes address offset
	 */
	twsiSlave.moreThen256 = MV_TRUE;

	if (MV_ERROR == mvTwsiRead(0, &twsiSlave, &boardId, 1))
		return MV_ERROR;

	*value = (boardId & 0x7);

	return MV_OK;
}

#ifdef CONFIG_BOBK
#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
MV_STATUS mvSysBypassCoreFreqGet(MV_U8 *value)
{
	/* set to fixed 365MHZ for customer's board */
	*value = MV_MSYS_CORECLOCK_OVERIDE_VAL;
}

#else
/*******************************************************************************/
/* The Core Frequency in Bypass mode is taken from the first address-value pair of the EEPROM
 * initialization sequence, In order to support normal TWSI init sequence flow, the first pair
 * of DWORDS on EEPROM should contain an address (bytes 0-3) of some scratch pad register
 * (for instance an UART SCR) and a value (bytes 4-7), which will be partially interpreted
 * as Core Freq in bypass mode ('bypass_clcok': SW EEPROM(0x50), reg#6 bits[2:0])
*/
MV_STATUS mvSysBypassCoreFreqGet(MV_U8 *value)
{
	MV_U8			bypass_coreclock;
	MV_TWSI_SLAVE	twsiSlave;

	/* Initializing twsiSlave in order to read from the TWSI address */
	twsiSlave.slaveAddr.address = 0x50;	/* 0x50: address of init EEPROM */
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 6;

	/* in case the offset should be set to a TWSI slave which support
	 * 2 bytes offset, the offset setting will be done in 2 transactions.
	 * For accessing EEPROM, always using 2 bytes address offset
	 */
	twsiSlave.moreThen256 = MV_TRUE;

	if (MV_ERROR == mvTwsiRead(0, &twsiSlave, &bypass_coreclock, 1))
	{
		mvPrintf("%s: Twsi failed to read bypass_coreclock, using default configuration\n", __func__);
		*value = MV_MSYS_CORECLOCK_OVERIDE_VAL;
		return MV_ERROR;
	}

	*value = (bypass_coreclock & 0x7);

	return MV_OK;
}
#endif/* CONFIG_CUSTOMER_BOARD_SUPPORT */
#endif/* CONFIG_BOBK */

/*******************************************************************************
* mvSysEnvDeviceRevGet - Get Marvell controller device revision number
*
* DESCRIPTION:
*       This function returns 8bit describing the device revision as defined
*       Revision ID Register.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       8bit describing Marvell controller revision number
*
*******************************************************************************/
MV_U8 mvSysEnvDeviceRevGet(MV_VOID)
{
#if defined MV_MSYS_AC3
	return MV_MSYS_AC3_A0_ID;
#elif defined MV_MSYS_BC2
	MV_U32   uiRegData;

	CHECK_STATUS(mvGenUnitRegisterGet(SERVER_REG_UNIT, 0, BC2_JTAG_DEV_ID_STATUS_REG_ADDR, &uiRegData, MV_ALL_BITS_MASK));
	return ((uiRegData >> BC2_JTAG_DEV_ID_STATUS_VERSION_OFFSET) & BC2_REVISON_ID_MASK) ;
#elif defined MV_MSYS_BOBK
	return MV_MSYS_BOBK_A0_ID;
#else
	#error "Un-defined silicon"
#endif
}

/*******************************************************************************
* mvSysEnvSuspendWakeupCheck
* DESCRIPTION:	 	Reads GPIO input for suspend-wakeup indication.
* INPUT:	 	None.
* OUTPUT:
* RRETURNS:	MV_U32 indicating suspend wakeup status:
* 	0 - normal initialization, 1 - read magic word to know, 2 - yes from GPIO.
 ***************************************************************************/
MV_SUSPEND_WAKEUP_STATUS mvSysEnvSuspendWakeupCheck(void)
{
	/* TBD */
	return MV_SUSPEND_WAKEUP_DISABLED;
}

/*******************************************************************************
* mvSysEnvCheckWakeupDramEnable
*
* DESCRIPTION: Check the magic wakeup enabled
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*       MV_SUSPEND_WAKEUP_ENABLED_MEM_DETECTED or MV_SUSPEND_WAKEUP_DISABLED
*
*******************************************************************************/
MV_SUSPEND_WAKEUP_STATUS mvSysEnvCheckWakeupDramEnable(void)
{
	return MV_SUSPEND_WAKEUP_DISABLED;
}

/*******************************************************************************
* mvSysEnvDlbConfigPtrGet
* DESCRIPTION: defines pointer to to DLB COnfiguration table
* INPUT: none
* OUTPUT: 
* RETURN:
*       returns pointer to DLB COnfiguration table
*******************************************************************************/
MV_DRAM_DLB_CONFIG  *mvSysEnvDlbConfigPtrGet(MV_VOID)
{
	return (&ddr3DlbConfigTable[0]);
}

/*******************************************************************************
* mvSysEnvDevStepGet - Get Marvell controller stepping ID
*
* DESCRIPTION:
*       This function returns 2bit describing the device stepping as defined
*       in PCI Device and Vendor ID configuration register offset 0x0.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       2bit desscribing Marvell controller stepping ID
*
*******************************************************************************/
MV_U8 mvSysEnvCtrlStepGet(MV_VOID)
{
#if defined CONFIG_ALLEYCAT3
	MV_U32	ctrlId = MV_REG_READ(DEV_ID_REG);

	ctrlId = (ctrlId & (DEVICE_ID_MASK)) >> DEVICE_ID_OFFS;
	return (ctrlId >> DEVICE_STEP_OFFS) & DEVICE_STEP_MASK;
#else
	return 0;
#endif
}

/*******************************************************************************
* mvSysEnvGetTopologyUpdateInfo
*
* DESCRIPTION: Read TWSI fields to update DDR topology structure
*
* INPUT: None
*
* OUTPUT: None, 0 means no topology update
*
* RETURN:
*       Bit mask of changes topology features
*
*******************************************************************************/
MV_U32 mvSysEnvGetTopologyUpdateInfo(MV_TOPOLOGY_UPDATE_INFO *topologyUpdateInfo)
{
	MV_U32 topologyFeaturesMap = 0;
	MV_U8	configVal;
	MV_TWSI_SLAVE twsiSlave;

	/* on AC3, DDR configuration (i.e ECC) supported only for DB board */
#if defined CONFIG_ALLEYCAT3
	if (mvBoardIdGet() != DB_AC3_ID)
		return 0;

	/* Alleycat-3S does not support ECC */
	if (mvSysEnvCtrlStepGet()) {
		mvPrintf("Running on AC3S board, DDR ECC is not supported\n");
		topologyUpdateInfo->mvUpdateECC = MV_TRUE;
		topologyUpdateInfo->mvECC = MV_TOPOLOGY_UPDATE_ECC_OFF;
		return 0;
	}
#elif defined CONFIG_BOBK
	return 0;
#endif

	/*Fix the topology for msys by SatR values*/
	twsiSlave.slaveAddr.address = 0x4D;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;

	/* Reading update DDR ECC Enable from device
	 * address: 0x4D, register: 0 bits[0:1]
	 */
	if (mvTwsiRead(0, &twsiSlave, &configVal, 1) != MV_OK) {
		mvPrintf("%s: Twsi failed to read enable DDR ECC update\n", __func__);
		return 0;
	}

	if( (configVal & DDR_SATR_ECC_CONFIG_MASK) == DDR_SATR_ECC_ENABLE_VALUE){
			topologyUpdateInfo->mvUpdateECC = MV_TRUE;
			topologyUpdateInfo->mvECC = MV_TOPOLOGY_UPDATE_ECC_ON;
	}
	else{
			topologyUpdateInfo->mvUpdateECC = MV_TRUE;
			topologyUpdateInfo->mvECC = MV_TOPOLOGY_UPDATE_ECC_OFF;
	}

	/*ECC always on PUP4*/
	topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP4;

	return topologyFeaturesMap;
}

/*******************************************************************************
* mvSysEnvGetCSEnaFromReg
*
* DESCRIPTION: Get bit mask of enabled CS
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*       Bit mask of enabled CS, 1 if only CS0 enabled, 3 if both CS0 and CS1 enabled
*
*******************************************************************************/
MV_U32 mvSysEnvGetCSEnaFromReg(void)
{
#if defined(CONFIG_ALLEYCAT3)
	return MV_REG_READ(REG_DDR3_RANK_CTRL_ADDR) & REG_DDR3_RANK_CTRL_CS_ENA_MASK;
#else
	/*BC2 have a different indication for CS mask, TBD after DualCS in BC2 support*/
	return 1;
#endif
}
/* mvSysEnvTimerIsRefClk25Mhz:
 * MSYS Bobcat2 and AlleyCat3 doesn't support 25Mhz as ref.clock for timer
 */
MV_BOOL mvSysEnvTimerIsRefClk25Mhz(void)
{
	return MV_FALSE;
}

#if defined CONFIG_BOBK
MV_U8 mvCtrlRevGet(MV_VOID)
{
	MV_U32 value;

	value = MV_DFX_REG_READ(DEV_REV_ID_REG);
	return (value & (REVISON_ID_MASK)) >> REVISON_ID_OFFS;
}
#endif
