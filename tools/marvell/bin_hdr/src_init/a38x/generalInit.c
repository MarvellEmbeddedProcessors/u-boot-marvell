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

/* includes */
#include "mv_os.h"
#include "config_marvell.h"  	/* Required to identify SOC and Board */
#include "mvUart.h"
#include "util.h"
#include "generalInit.h"
#include "printf.h"
#include "mvSysEnvLib.h"

/******************************************************************************************
* mvDeviceIdConfig - set SoC Unit configuration and device ID according to detected flavour
*******************************************************************************************/
static MV_VOID mvDeviceIdConfig(void)
{
	MV_DEVICE_ID_VAL dev_id_val[] = MV_DEVICE_ID_VAL_INFO;
	MV_U32 ctrlId, devId = mvSysEnvDeviceIdGet(); /* read Sample at reset for device ID*/

	/* Verify Marvell A38x boards limitation */
#if !defined(CONFIG_CUSTOMER_BOARD_SUPPORT) && defined(MV88F68XX)
	/* only A381/2 board support 6821 device ID */
	if (devId == MV_6811 && mvBoardIdGet() != DB_BP_6821_ID)
		mvPrintf("%s: Error: Device 6811/21 is not supported on current board (only A381/2 DB-BP)\n", __func__);
#endif

	if (devId == MV_NONE) {
		mvPrintf("%s: Error: Read an unknown device ID from 'S@R'\n", __func__);
#if !defined(CONFIG_CUSTOMER_BOARD_SUPPORT)
		mvPrintf("Setting default device ID\n");
		devId = MV_DEFAULT_DEVICE_ID; /* fallback to default only for Marvell boards */
#endif

	}
	/* Configure Units according to detected deviceId (flavor) */
	MV_REG_WRITE(DEVICE_CONFIGURATION_REG0, dev_id_val[devId].wo_reg_val0);
	MV_REG_WRITE(DEVICE_CONFIGURATION_REG1, dev_id_val[devId].wo_reg_val1);

	/* set device ID register (flavor) */
	ctrlId = MV_REG_READ(DEV_ID_REG) & ~DEV_ID_REG_DEVICE_ID_MASK;
	ctrlId |= dev_id_val[devId].ctrlModel << DEV_ID_REG_DEVICE_ID_OFFS;
	MV_REG_WRITE(DEV_ID_REG, ctrlId);
}

/* mvMppConfig() prepares UART and I2C configuration (MPP's and UART interface selection) */
static inline MV_VOID mvMppConfig()
{
        /* UART0 & I2C MPP's (MPP[0:3] = 0x1) */
        MV_U32 regData = (MV_REG_READ(MPP_CONTROL_REG(0))  & MPP_SET_MASK) | MPP_SET_DATA;
        MV_REG_WRITE(MPP_CTRL_REG, regData);

        /* UART1 MPP's (MPP[19:20] = 0x6) - UART1 is the default UART interface on DB-AP-68xx */
        regData = (MV_REG_READ(MPP_CONTROL_REG(2))  & MPP_UART1_SET_MASK) | MPP_UART1_SET_DATA;
        MV_REG_WRITE(MPP_CONTROL_REG(2), regData);
}

/* update RTC (Read Timing Control) values of PCIe memory wrappers.
 * use slower memory Read Timing, to allow more efficient energy consumption, in order to lower
 * the minimum VDD of the memory.
 * This will lead to more robust memory when voltage drop occurs (VDDSEG)
 */
MV_VOID mvRtcConfig()
{
	MV_U32 i, pipeSelectVal;

	/* Activate pipe0 for read/write transaction, and set XBAR client number #1 */
	pipeSelectVal = 0x1 << DFX_PIPE_SELECT_PIPE0_ACTIVE_OFFS \
			| 0x1 << DFX_PIPE_SELECT_XBAR_CLIENT_SEL_OFFS;
	MV_REG_WRITE(DFX_REG_BASE, pipeSelectVal);

	/* Set new RTC value for all memory wrappers */
	for (i = 0; i < RTC_MEMORY_WRAPPER_COUNT; i++)
		MV_REG_WRITE(RTC_MEMORY_WRAPPER_REG(i), RTC_MEMORY_WRAPPER_CTRL_VAL);

}

MV_STATUS mvGeneralInit(void)
{
	MV_U32 regData;

	/* Update AVS debug control register */
    MV_REG_WRITE(AVS_DEBUG_CNTR_REG, AVS_DEBUG_CNTR_DEFAULT_VALUE);
    MV_REG_WRITE(AVS_DEBUG_CNTR_REG, AVS_DEBUG_CNTR_DEFAULT_VALUE);

    regData = MV_REG_READ(AVS_ENABLED_CONTROL);
	regData &= ~(AVS_LOW_VDD_LIMIT_MASK | AVS_HIGH_VDD_LIMIT_MASK);
#ifdef CONFIG_ARMADA_38X
	/* 1. Armada38x was signed off for 1600/800 at 1.15V (AVS)
	 * 2. Based on ATE/system correlation, in order to achieve higher speeds (1866MHz, 2000MHz),
	 *    we need to overdrive the chip to 1.25V (AVS)
	 * 3. Current U-Boot (in the absence of SVC) - must align with this requirement and select
	 *    voltage between the two levels, based on S@R (CPU <= 1600MHz --> AVS@ 1.15V)
	 */
	{
		MV_U32 satrFreq;
		satrFreq = (MV_REG_READ(DEVICE_SAMPLE_AT_RESET1_REG) >> SAR_FREQ_OFFSET) & SAR_FREQ_MASK;

		/*Set AVS value only for normative core freq(1600Mhz and less), for high freq leave default value*/
		if(satrFreq <= 0xD){
			regData |= ( (AVS_LIMIT_VAL_SLOW << AVS_LOW_VDD_LIMIT_OFFS) | (AVS_LIMIT_VAL_SLOW << AVS_HIGH_VDD_LIMIT_OFFS));
			MV_REG_WRITE(AVS_ENABLED_CONTROL, regData);
		}
	}
#else
		regData |= ( (AVS_LIMIT_VAL << AVS_LOW_VDD_LIMIT_OFFS) | (AVS_LIMIT_VAL << AVS_HIGH_VDD_LIMIT_OFFS));
		MV_REG_WRITE(AVS_ENABLED_CONTROL, regData);
#endif

	mvRtcConfig(); /* update RTC (Read Timing Control) values of PCIe memory wrappers*/
	mvMppConfig();

	/* - Init the TWSI before all I2C transaction */
	DEBUG_INIT_FULL_S("mvGeneralInit: Init TWSI interface.\n");
	mvHwsTwsiInitWrapper();

#if !defined(MV_NO_PRINT)
	mvUartInit();
	mvPrintf("\n\nGeneral initialization - Version: " GENERAL_VERION "\n");
#endif

	/* Device general configuration was not supported on a38x Z0 revision */
	if (mvSysEnvDeviceRevGet() != MV_88F68XX_Z1_ID)
		mvDeviceIdConfig();

	mvSysEnvUsbVbusReset();

	return MV_OK;
}

