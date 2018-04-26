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


#ifndef __INCmvBHboardEnvSpech
#define __INCmvBHboardEnvSpech
#include "ddr3_hws_hw_training.h"

/* define units per card */
#if defined CONFIG_ALLEYCAT3
#define MV_USB_UNIT                   /* define USB unit for AC3 */
#define MV_HWS_COM_PHY_SERDES
#elif defined CONFIG_BOBK
#define MV_USB_UNIT                   /* define USB unit for BOBK */
#define MV_HWS_AVAGO_SERDES
#define MV_HWS_SERDES_20_OFFSET     0x40000
#else /*CONFIG_BOBCAT2 */
#define MV_HWS_COM_PHY_SERDES
#endif
/* Board specific configuration */
/* ============================ */
/* Bobcat2 Customer Boards */
#define BC2_CUSTOMER_BOARD_ID_BASE	0x0
#define BC2_CUSTOMER_BOARD_ID0		(BC2_CUSTOMER_BOARD_ID_BASE + 0)
#define BC2_CUSTOMER_BOARD_ID1		(BC2_CUSTOMER_BOARD_ID_BASE + 1)
#define BC2_CUSTOMER_MAX_BOARD_ID	(BC2_CUSTOMER_BOARD_ID_BASE + 2)
#define BC2_CUSTOMER_BOARD_NUM		(BC2_CUSSTOMER_MAX_BOARD_ID - BC2_CUSTOMER_BOARD_ID_BASE)

/* Bobcat2 Marvell boards */
#define BC2_MARVELL_BOARD_ID_BASE	0x10
#define DB_DX_BC2_ID				(BC2_MARVELL_BOARD_ID_BASE + 0)
#define RD_DX_BC2_ID				(BC2_MARVELL_BOARD_ID_BASE + 1)
#define RD_MTL_BC2					(BC2_MARVELL_BOARD_ID_BASE + 2)
#define BC2_MARVELL_MAX_BOARD_ID	(BC2_MARVELL_BOARD_ID_BASE + 3)
#define BC2_MARVELL_BOARD_NUM		(BC2_MARVELL_MAX_BOARD_ID - BC2_MARVELL_BOARD_ID_BASE)

/* BobCat2  Family */
#define MV_BOBCAT2_DEV_ID		0xFC00


/* AlleyCat3 Customer Boards */
#define AC3_CUSTOMER_BOARD_ID_BASE	0x20
#define AC3_CUSTOMER_BOARD_ID0		(AC3_CUSTOMER_BOARD_ID_BASE + 0)
#define AC3_CUSTOMER_BOARD_ID1		(AC3_CUSTOMER_BOARD_ID_BASE + 1)
#define AC3_CUSTOMER_MAX_BOARD_ID	(AC3_CUSTOMER_BOARD_ID_BASE + 2)
#define AC3_CUSTOMER_BOARD_NUM		(AC3_CUSTOMER_MAX_BOARD_ID - AC3_CUSTOMER_BOARD_ID_BASE)

/* AlleyCat3 Marvell boards */
#define AC3_MARVELL_BOARD_ID_BASE	0x30
#define DB_AC3_ID					(AC3_MARVELL_BOARD_ID_BASE + 0)
#define RD_MTL_4XG_AC3_ID			(AC3_MARVELL_BOARD_ID_BASE + 1)
#define RD_MTL_2XXG_2XG_AC3_ID		(AC3_MARVELL_BOARD_ID_BASE + 2)
#define DB_MISL_24G4G_AC3_ID		(AC3_MARVELL_BOARD_ID_BASE + 3)
#define RD_MTL_24G_AC3_ID			(AC3_MARVELL_BOARD_ID_BASE + 4)
#define AC3_MARVELL_MAX_BOARD_ID	(AC3_MARVELL_BOARD_ID_BASE + 5)
#define AC3_MARVELL_BOARD_NUM		(AC3_MARVELL_MAX_BOARD_ID - AC3_MARVELL_BOARD_ID_BASE)

/* AlleyCat3  Family */
#define MV_ALLEYCAT3_DEV_ID		0xF400

/* BobK Customer Boards */
#define BOBK_CUSTOMER_BOARD_ID_BASE	0x40
#define BOBK_CETUS_CUSTOMER_BOARD_ID0		(BOBK_CUSTOMER_BOARD_ID_BASE + 0)
#define BOBK_CAELUM_CUSTOMER_BOARD_ID1		(BOBK_CUSTOMER_BOARD_ID_BASE + 1)
#define BOBK_CUSTOMER_MAX_BOARD_ID	(BOBK_CUSTOMER_BOARD_ID_BASE + 2)
#define BOBK_CUSTOMER_BOARD_NUM		(BOBK_CUSTOMER_MAX_BOARD_ID - BOBK_CUSTOMER_BOARD_ID_BASE)

/* BobK Marvell Boards */
#define BOBK_MARVELL_BOARD_ID_BASE	0x50
#define BOBK_CETUS_DB_ID				    (BOBK_MARVELL_BOARD_ID_BASE + 0)
#define BOBK_CAELUM_DB_ID				    (BOBK_MARVELL_BOARD_ID_BASE + 1)
#define BOBK_LEWIS_RD_ID				    (BOBK_MARVELL_BOARD_ID_BASE + 2)
#define BOBK_CYGNUS_RD_ID				    (BOBK_MARVELL_BOARD_ID_BASE + 3)
#define BOBK_MARVELL_MAX_BOARD_ID	(BOBK_MARVELL_BOARD_ID_BASE + 4)
#define BOBK_MARVELL_BOARD_NUM		(BOBK_MARVELL_MAX_BOARD_ID - BOBK_MARVELL_BOARD_ID_BASE)

/* BobK  Family */
#define MV_BOBK_DEV_ID		0xBC00

#define INVALID_BOARD_ID			0xFFFF
#define BOARD_ID_INDEX_MASK			0x10	/* Mask used to return board index via board Id */

#if defined CONFIG_ALLEYCAT3
	#define MARVELL_BOARD_ID_BASE		AC3_MARVELL_BOARD_ID_BASE
	#define MV_MAX_MARVELL_BOARD_ID		AC3_MARVELL_MAX_BOARD_ID
	#define MV_MARVELL_BOARD_NUM		AC3_MARVELL_BOARD_NUM
	#define MV_DEFAULT_BOARD_ID			DB_AC3_ID
	#define MV_MSYS_DEV_ID				MV_ALLEYCAT3_DEV_ID
#elif  defined CONFIG_BOBCAT2
	#define MARVELL_BOARD_ID_BASE		BC2_MARVELL_BOARD_ID_BASE
	#define MV_MAX_MARVELL_BOARD_ID		BC2_MARVELL_MAX_BOARD_ID
	#define MV_MARVELL_BOARD_NUM		BC2_MARVELL_BOARD_NUM
	#define MV_DEFAULT_BOARD_ID			DB_DX_BC2_ID
	#define MV_MSYS_DEV_ID				MV_BOBCAT2_DEV_ID
#else
/* BobK Marvell boards  TBD!!!!*/
	#define MARVELL_BOARD_ID_BASE		BOBK_MARVELL_BOARD_ID_BASE
	#define MV_MAX_MARVELL_BOARD_ID		BOBK_MARVELL_MAX_BOARD_ID
	#define MV_MARVELL_BOARD_NUM		BOBK_MARVELL_BOARD_NUM
	#define MV_DEFAULT_BOARD_ID			BOBK_CETUS_DB_ID
	#define MV_MSYS_DEV_ID				MV_BOBK_DEV_ID
#endif

/* Device-ID number of the PCIe DevID register address
   for BC2, AC3 and BOBK                              */
#define MV_MSYS_DEV_VERSION_ID_REG		       0xF8240	/* under server space */

/* Bobcat2 device revision */
#define BC2_JTAG_DEV_ID_STATUS_REG_ADDR        0x000F8244
#define BC2_REVISON_ID_OFFS			              16
#define BC2_JTAG_DEV_ID_STATUS_VERSION_OFFSET     28
#define BC2_REVISON_ID_MASK			0xF
#define MV_MSYS_BC2_A0_ID			0x0
#define MV_MSYS_BC2_B0_ID			0x1

/* Alleycat3 device revision */
#define MV_MSYS_AC3_A0_ID			0x0

/* BobK device revision TBD !!!*/
#define MV_MSYS_BOBK_A0_ID			0x0
#define MV_MSYS_BOBK_A1_ID			0x1

/* MV_DFX_REG_READ/WRITE API are used to access DFX data prior to SerDes initialization.
   As a part of SerDes init flow, mvGenUnitRegisterSet/Get API's are enabled to access
   DFX domain, after mvUnitInfoSet(SERVER_REF_UNIT,..) is called.
*/
#define DFX_REGS_BASE_BOOTROM	(MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(SERVER_WIN_ID)))
#define MV_DFX_REG_READ(offset)		\
	(MV_MEMIO_LE32_READ(DFX_REGS_BASE_BOOTROM | (offset)))	\

#define MV_DFX_REG_WRITE(offset, val)	\
{					\
	MV_MEMIO_LE32_WRITE((DFX_REGS_BASE_BOOTROM | (offset)), (val));	\
}

typedef enum _mvSuspendWakeupStatus {
	MV_SUSPEND_WAKEUP_DISABLED,
	MV_SUSPEND_WAKEUP_ENABLED,
	MV_SUSPEND_WAKEUP_ENABLED_GPIO_DETECTED,
	MV_SUSPEND_WAKEUP_ENABLED_MEM_DETECTED,
} MV_SUSPEND_WAKEUP_STATUS;

/* MISC defines */
#define INTER_REGS_BASE								0xD0000000

#define PLL0_CNFIG_OFFSET			21
#define PLL0_CNFIG_MASK				0x7
#define PLL1_CNFIG_OFFSET			18
#define PLL1_CNFIG_MASK				0x7

#if  defined CONFIG_BOBCAT2
	#define REG_DEVICE_SAR1_ADDR                        0xF8200
	#define REG_DEVICE_SAR2_ADDR                        0xF8204
	#define REG_DEVICE_SERVER_CONTROL_0                 0xF8250
	#define REG_DEVICE_SERVER_CONTROL_14                0xF8288
	#define REG_DEVICE_SAR1_MSYS_TM_SDRAM_SEL_OFFSET    11
	#define REG_DEVICE_SAR1_MSYS_TM_SDRAM_SEL_MASK      0x1
#else
	#define REG_DEVICE_SAR0_ADDR                        0xF8200
	#define REG_DEVICE_SAR1_ADDR                        0xF8204
#endif

#if defined CONFIG_BOBK
	#define REG_DEVICE_SAR1_OVERRIDE_ADDR				0xF82D8
#endif

/* MPP */
#define MPP_CONTROL_REG(id)                     (0x18000 + (id * 4))
#define MPP_UART1_SET_MASK			(~(0xff000))
#define MPP_UART1_SET_DATA			0x22000

/* defines for DFX reg, used in core clock WA */
#if defined CONFIG_BOBK
#define REG_SERVER_RESET_CTRL		0xF800C
#define MG_SOFT_RESET_TRIG_BIT		1

#define REG_CONF_SKIP_INIT_MATRIX	0xF8020
#define REG_RAM_SKIP_INIT_MATRIX	0xF8030
#define REG_CPU_SKIP_INIT_MATRIX	0xF8058
#define REG_TABLES_SKIP_INIT_MATRIX	0xF8060
#define REG_SERDES_SKIP_INIT_MATRIX	0xF8064
#define REG_EEPROM_SKIP_INIT_MATRIX	0xF8068
#define SKIP_INIT_MG_SOFTRST_OFFSET	8

#define REG_PLL_CORE_PARAMETERS		0xF82E0
#define REG_PLL_CORE_CONFIG			0xF82E4
#define PLL_CORE_CONFIG_USE_RF_BIT	9
#define PLL_CORE_CONFIG_BYPASS_BIT	0

#define PLL_CORE_PARAM_KDIV_MASK	0x7
#define PLL_CORE_PARAM_MDIV_OFFSET	3
#define PLL_CORE_PARAM_MDIV_MASK	(0x1FF<<PLL_CORE_PARAM_MDIV_OFFSET)
#define PLL_CORE_PARAM_NDIV_OFFSET	12
#define PLL_CORE_PARAM_NDIV_MASK	(0x1FF<<PLL_CORE_PARAM_NDIV_OFFSET)
#define PLL_CORE_PARAM_VCO_OFFSET	21
#define PLL_CORE_PARAM_VCO_MASK		(0xF<<PLL_CORE_PARAM_VCO_OFFSET)
#define PLL_CORE_PARAM_LPF_OFFSET	25
#define PLL_CORE_PARAM_LPF_MASK		(0xF<<PLL_CORE_PARAM_LPF_OFFSET)
#define PLL_CORE_PARAM_IADJ_OFFSET	29
#define PLL_CORE_PARAM_IADJ_MASK	(0x7<<PLL_CORE_PARAM_IADJ_OFFSET)

#define DEVICE_GENERAL_CONTROL_17	0xF8294
#define DEVICE_GENERAL_CONTROL_20	0xF82A0
#define DEVICE_GENERAL_CONTROL_21	0xF82A4

#define DEV_REV_ID_REG			0xF8244
#define REVISON_ID_OFFS			28
#define REVISON_ID_MASK			0xF0000000

/* BobK Core Clock setting index*/
typedef enum _mvBypassCoreClcokFreq {
	MV_MSYS_CORECLOCK_365M = 0,
	MV_MSYS_CORECLOCK_220M,
	MV_MSYS_CORECLOCK_250M,
	MV_MSYS_CORECLOCK_200M,
	MV_MSYS_CORECLOCK_167M,
} MV_BYPASS_CORECLOCK_FREQ;

/* For Bobk ASIC, the CPSS program need the CoreClock WA("mvBypassCoreClockWA") to set
core PLL manually in bypass mode(HW SAR "coreclock"=7), this MV_MSYS_CORECLOCK_OVERIDE_VAL
is used for BobK customer boards to define the default coreclock frequency in
bypass mode. Refer to the WA funtion for detail. */
#define MV_MSYS_CORECLOCK_OVERIDE_VAL MV_MSYS_CORECLOCK_365M

typedef struct mvBypassCoreClockVal {
	MV_BYPASS_CORECLOCK_FREQ freqId;
	MV_32 freq;
	MV_32 kDiv;
	MV_32 mDiv;
	MV_32 nDiv;
	MV_32 vcoBand;
	MV_32 lpf;
	MV_32 iAdj;
	MV_32 enable;
} MV_BYPASS_CORECLOCK_VAL;

/* predefined values for MV_BYPASS_CORECLOCK_VAL per frequency */
/*	ex:	Core PLL (365 MHz)
		i.	 bit[2:0] K div = 0x3 ( k = 4)
		ii.  bit [11:3]  M div = 2 ( m = 3)
		iii. bit [20:12] N_div = 350 ( N = 351)
		iv.  bit[24:21] VCO band = 11
		v.	 bit [28:25]  LPF  = 1
		vi.  bit[31:29] Iadj = 1
*/
#define MV_BYPASS_CORECLOCK_VAL_INFO {\
/*          freqId,                 freq, k, m, n,   vcoBand, lpf, jAdj, enable */\
/* 365M */ {MV_MSYS_CORECLOCK_365M,	365,  3, 2, 350, 11,      1,   1,    MV_TRUE },\
/* 220M */ {MV_MSYS_CORECLOCK_220M,	220,  3, 2, 211, 5,       1,   1,    MV_FALSE },\
/* 250M */ {MV_MSYS_CORECLOCK_250M,	250,  3, 0, 79,	 6,       4,   1,    MV_TRUE },\
/* 200M */ {MV_MSYS_CORECLOCK_200M, 200,  3, 0, 63,	 5,       4,   1,    MV_TRUE },\
/* 167M */ {MV_MSYS_CORECLOCK_167M,	167,  2, 1, 106, 3,       4,   1,    MV_TRUE },\
};
#endif



/*************************** Globals ***************************/

extern MV_DRAM_DLB_CONFIG ddr3DlbConfigTable[];

/*************************** Functions declarations ***************************/

/**************************************************************************
 * mvBoardIdGet -
 *
 * DESCRIPTION:          Returns the board ID
 *
 * INPUT:
 *		None.
 *
 * OUTPUT:
 *		None.
 *
 * RETURNS:
 *		Board ID.
 ***************************************************************************/
MV_U32 mvBoardIdGet(MV_VOID);

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
MV_U32 mvBoardIdIndexGet(MV_U32 boardId);

/**************************************************************************
 * mvBoardTclkGet -
 *
 * DESCRIPTION:          Returns the board id
 *
 * INPUT:
 *		None.
 *
 * OUTPUT:
 *		None.
 *
 * RETURNS:
 *		board ID.
 ***************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID);

/**************************************************************************
 * mvBoardSarBoardIdGet -
 *
 * DESCRIPTION:          Returns the board ID from SatR
 *
 * INPUT:
 *		None.
 *
 * OUTPUT:
 *		None.
 *
 * RETURNS:
 *		Tclk
 ***************************************************************************/
MV_STATUS mvBoardSarBoardIdGet(MV_U8 *value);

/**************************************************************************
 * mvSysBypassCoreFreqGet -
 *
 * DESCRIPTION:          Returns the Core Freq in Bypass mode from SatR
 *
 * INPUT:
 *		None.
 *
 * OUTPUT:
 *		None.
 *
 * RETURNS:
 *		Tclk
 ***************************************************************************/
MV_STATUS mvSysBypassCoreFreqGet(MV_U8 *value);


/************************************************************************************
* mvSysEnvSuspendWakeupCheck
* DESCRIPTION:
*		Reads GPIO input for suspend-wakeup indication.
*					Not supported for AC3/BC2
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RRETURN:
*		MV_U32 indicating suspend wakeup status:
* 		0 - normal initialization, otherwise - suspend wakeup.
 ***************************************************************************/
MV_SUSPEND_WAKEUP_STATUS mvSysEnvSuspendWakeupCheck(MV_VOID);

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
MV_U8 mvSysEnvDeviceRevGet(MV_VOID);

/*******************************************************************************
* mvSysEnvDlbConfigPtrGet
* DESCRIPTION: defines pointer to to DLB COnfiguration table
* INPUT: none
* OUTPUT: 
* RETURN:
*       returns pointer to DLB COnfiguration table
*******************************************************************************/
MV_DRAM_DLB_CONFIG  *mvSysEnvDlbConfigPtrGet(MV_VOID);

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
MV_U8 mvSysEnvCtrlStepGet(MV_VOID);

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
MV_U32 mvSysEnvGetTopologyUpdateInfo(MV_TOPOLOGY_UPDATE_INFO *topologyUpdateInfo);

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
MV_U32 mvSysEnvGetCSEnaFromReg(void);

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
MV_SUSPEND_WAKEUP_STATUS mvSysEnvCheckWakeupDramEnable(void);
/**************************************************************************
 * mvSysEnvTimerIsRefClk25Mhz -
 *
 * DESCRIPTION:          Routine to indicate if 25Mhz ref clock for timer is supported
 * INPUT:                None.
 * OUTPUT:               None.
 * RETURNS:              None.
 ***************************************************************************/
MV_STATUS mvSysEnvTimerIsRefClk25Mhz(MV_VOID);
/**************************************************************************
 * mvHwsTwsiInitWrapper -
 *
 * DESCRIPTION:          Wrapper for initializing the TWSI unit
 * INPUT:                None.
 * OUTPUT:               None.
 * RETURNS:              None.
 ***************************************************************************/
MV_STATUS mvHwsTwsiInitWrapper(MV_VOID);
MV_U8 mvCtrlRevGet(MV_VOID);
#endif /* __INCmvBHboardEnvSpech */

