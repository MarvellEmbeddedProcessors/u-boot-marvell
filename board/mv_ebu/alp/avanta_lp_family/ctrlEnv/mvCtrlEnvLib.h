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

#ifndef __INCmvCtrlEnvLibh
#define __INCmvCtrlEnvLibh

#include "mvSysHwConfig.h"
#include "mvCommon.h"
#include "mvTypes.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"

typedef enum _mvSatRTypeID {
	// "Bios" Device
	MV_SATR_CPU_DDR_L2_FREQ,
	MV_SATR_CORE_CLK_SELECT,
	MV_SATR_CPU1_ENABLE,
	MV_SATR_SSCG_DISABLE,
	// Jumpers  - change by removing Jumper. S@R will be changed by this option
	MV_SATR_I2C0_SERIAL_ROM,
	MV_SATR_EXTERNAL_CPU_RESET,
	MV_SATR_EXTERNAL_CORE_RESET,
	// DIP Switch - change by removing Switch. S@R will be changed by this option:
	MV_SATR_BOOT_DEVICE,
	// DPR's -modified by moving resistor with solderer. S@R will be changed by this
	MV_SATR_CPU_PLL_XTAL_BYPASS,
	MV_SATR_CPU0_ENDIANESS,
	MV_SATR_CPU0_NMFI,
	MV_SATR_CPU0_THUMB,
	MV_SATR_PEX0_CLOCK,
	MV_SATR_PEX1_CLOCK,
	MV_SATR_REF_CLOCK_ENABLE,
	MV_SATR_EFUSE_BYPASS,
	MV_SATR_POR_BYPASS,
	MV_SATR_TESTER_OPTIONS,
	MV_SATR_BOARD_ID,
	MV_SATR_READ_MAX_OPTION,
	MV_SATR_WRITE_CPU_FREQ,
	MV_SATR_WRITE_CORE_CLK_SELECT,
	MV_SATR_WRITE_CPU1_ENABLE,
	MV_SATR_WRITE_SSCG_DISABLE,
	MV_SATR_WRITE_MAX_OPTION
} MV_SATR_TYPE_ID;

typedef enum _mvConfigTypeID {
	MV_CONFIG_MAC0,
	MV_CONFIG_MAC1,
	MV_CONFIG_PON_SERDES,
	MV_CONFIG_PON_BEN_POLARITY,
	MV_CONFIG_SGMII0_CAPACITY,
	MV_CONFIG_SGMII1_CAPACITY,
	MV_CONFIG_LANE1,
	MV_CONFIG_LANE2,
	MV_CONFIG_LANE3,
	MV_CONFIG_DEVICE_BUS_MODULE,
	MV_CONFIG_SLIC_TDM_DEVICE,
	MV_CONFIG_TYPE_MAX_OPTION
} MV_CONFIG_TYPE_ID;

typedef enum _mvIoExpanderTypeID {
	MV_IO_EXPANDER_SFP0_TX_DIS,
	MV_IO_EXPANDER_SFP0_PRSNT,
	MV_IO_EXPANDER_SFP0_TX_FAULT,
	MV_IO_EXPANDER_SFP0_LOS,
	MV_IO_EXPANDER_SFP1_PRSNT,
	MV_IO_EXPANDER_SFP1_TX_FAULT,
	MV_IO_EXPANDER_SFP1_LOS,
	MV_IO_EXPANDER_SFP1_TX_DIS,
	MV_IO_EXPANDER_USB_VBUS,
	MV_IO_EXPANDER_MAC0_RJ45_PORT_LED,
	MV_IO_EXPANDER_MAC0_SFP_PORT_LED,
	MV_IO_EXPANDER_MAC1_RJ45_PORT_LED,
	MV_IO_EXPANDER_MAC1_SFP_PORT_LED,
	MV_IO_EXPANDER_PON_PORT_LED,
	MV_IO_EXPANDER_SD_STATUS,
	MV_IO_EXPANDER_SD_WRITE_PROTECT,
	MV_IO_EXPANDER_JUMPER1,
	MV_IO_EXPANDER_JUMPER2_EEPROM_ENABLED,
	MV_IO_EXPANDER_JUMPER3,
	MV_IO_EXPANDER_EXT_PHY_SMI_EN,
	MV_IO_EXPANDER_SPI1_CS_MSB0,
	MV_IO_EXPANDER_SPI1_CS_MSB1,
	MV_IO_EXPANDER_INTEG_PHY_PORTS_LED,
	MV_IO_EXPANDER_USB_SUPER_SPEED,
} MV_IO_EXPANDER_TYPE_ID;

/* This enumerator describes the possible SMI control options */
typedef enum _mvSMIctrl {
	CPU_SMI_CTRL,
	SWITCH_SMI_CTRL,
} MV_SMI_CTRL;

/* typedefs */
typedef MV_STATUS (*MV_WIN_GET_FUNC_PTR)(MV_U32, MV_U32, MV_UNIT_WIN_INFO*);

/* This enumerator describes the possible HW cache coherency policies the
 * controllers supports.
 */
typedef enum _mvCachePolicy {
	NO_COHERENCY,   /* No HW cache coherency support                        */
	WT_COHERENCY,   /* HW cache coherency supported in Write Through policy */
	WB_COHERENCY    /* HW cache coherency supported in Write Back policy    */
} MV_CACHE_POLICY;

/* The swapping is referred to a 64-bit words (as this is the controller
 * internal data path width). This enumerator describes the possible
 * data swap types. Below is an example of the data 0x0011223344556677
 */
typedef enum _mvSwapType {
	MV_BYTE_SWAP,           /* Byte Swap                77 66 55 44 33 22 11 00 */
	MV_NO_SWAP,             /* No swapping              00 11 22 33 44 55 66 77 */
	MV_BYTE_WORD_SWAP,      /* Both byte and word swap  33 22 11 00 77 66 55 44 */
	MV_WORD_SWAP,           /* Word swap                44 55 66 77 00 11 22 33 */
	SWAP_TYPE_MAX           /* Delimiter for this enumerator                    */
} MV_SWAP_TYPE;

/*
 * Define the different Ethernet complex sources for the RGMIIA/B and
 * the FE/GE phy interfaces.
 */
typedef enum {
	MV_ETHCOMP_SW_P0_2_GE_PHY_P0		=	BIT0,
	MV_ETHCOMP_SW_P1_2_GE_PHY_P1		=	BIT1,
	MV_ETHCOMP_SW_P2_2_GE_PHY_P2		=	BIT2,
	MV_ETHCOMP_SW_P3_2_GE_PHY_P3		=	BIT3,
	MV_ETHCOMP_SW_P4_2_RGMII0		=	BIT4,
	/* Switch port 5 does not exist, ==> BIT5 is omitted */
	MV_ETHCOMP_GE_MAC0_2_SW_P6		=	BIT6,
	MV_ETHCOMP_GE_MAC0_2_GE_PHY_P0		=	BIT7,
	MV_ETHCOMP_GE_MAC0_2_RGMII0		=	BIT8,
	MV_ETHCOMP_GE_MAC0_2_COMPHY_1		=	BIT9,
	MV_ETHCOMP_GE_MAC0_2_COMPHY_2		=	BIT10,
	MV_ETHCOMP_GE_MAC0_2_COMPHY_3		=	BIT11,
	MV_ETHCOMP_GE_MAC1_2_SW_P4		=	BIT12,
	MV_ETHCOMP_GE_MAC1_2_GE_PHY_P3		=	BIT13,
	MV_ETHCOMP_GE_MAC1_2_RGMII1		=	BIT14,
	MV_ETHCOMP_GE_MAC1_2_PON_ETH_SERDES	=	BIT15,
	MV_ETHCOMP_P2P_MAC_2_PON_ETH_SERDES	=	BIT16,
} MV_ETH_COMPLEX_TOPOLOGY;

typedef enum {
	MAC0_2_SW_P6		=	0x0,
	MAC0_2_GE_PHY_P0	=	0x1,
	MAC0_2_RGMII		=	0x2,
	MAC0_2_SGMII		=	0x3,
} MV_ETH_MAC0_CONFIG;

typedef enum {
	EPM_DEFAULT = 0x0,      /* RGMII */
	EPM_MAC0_MII = 0x01,
	EPM_MAC1_MII = 0x10,
	EPM_SW_PORT_5_MII = 0x100,
	EPM_SW_PORT_6_MII = 0x1000
} MV_ETH_PORT_MODE;

typedef enum {
	SERDES_UNIT_UNCONNECTED = 0x0,
	SERDES_UNIT_PEX         = 0x1,
	SERDES_UNIT_SATA        = 0x2,
	SERDES_UNIT_SGMII0      = 0x3,
	SERDES_UNIT_SGMII1      = 0x4,
	SERDES_UNIT_SGMII2      = 0x5,
	SERDES_UNIT_SGMII3      = 0x6,
	SERDES_UNIT_QSGMII      = 0x7,
	SERDES_UNIT_LAST
} MV_SERDES_UNIT_INDX;

typedef enum {
	PEX_BUS_DISABLED        = 0,
	PEX_BUS_MODE_X1         = 1,
	PEX_BUS_MODE_X4         = 2,
	PEX_BUS_MODE_X8         = 3
} MV_PEX_UNIT_CFG;

/* Configuration per SERDES line.
   Each nibble is MV_SERDES_LINE_TYPE */
typedef struct _boardSerdesConf {
	MV_U32 enableSerdesConfiguration;       /*This will determine if mvCtrlSerdesPhyConfig will configure the serdes*/
	MV_U32 serdesLine0_7;                   /* Lines 0 to 7 SERDES MUX one nibble per line */
	MV_U32 serdesLine8_15;                  /* Lines 8 to 15 SERDES MUX one nibble per line */
	MV_PEX_UNIT_CFG pex0Mod;
	MV_PEX_UNIT_CFG pex1Mod;
	MV_PEX_UNIT_CFG pex2Mod;
	MV_PEX_UNIT_CFG pex3Mod;
	MV_U32 busSpeed;        /* Bus speed - one bit per SERDES line:
	                           Low speed (0)		High speed (1)
	                           PEX	2.5 G (10 bit)		5 G (20 bit)
	                           SATA	1.5 G			3 G
	                           SGMII        1.25 Gbps		3.125 Gbps	*/
} MV_SERDES_CFG;

/* Termal Sensor Registers */
#define TSEN_STATE_REG							0xE8070
#define TSEN_STATE_OFFSET						31
#define TSEN_STATE_MASK							(0x1 << TSEN_STATE_OFFSET)

#define TSEN_CONF_REG							0xE8074
#define TSEN_CONF_OTF_CALIB_MASK				(0x1 << 8)

#define TSEN_STATUS_REG							0xE8078
#define TSEN_STATUS_TEMP_OUT_OFFSET				0
#define TSEN_STATUS_TEMP_OUT_MASK				(0x1FF << TSEN_STATUS_TEMP_OUT_OFFSET)

/* BIOS Modes related defines */
#define SAR_CPU_FAB_GET(cpu, fab)       (((cpu & 0x7) << 21) | ((fab & 0xF) << 24))

/* mcspLib.h API list */
MV_STATUS mvCtrlSatRWrite(MV_SATR_TYPE_ID satrWriteField, MV_SATR_TYPE_ID satrReadField, MV_U8 val);
MV_U32 mvCtrlSatRRead(MV_SATR_TYPE_ID satrField);
MV_STATUS mvCtrlCpuDdrL2FreqGet(MV_FREQ_MODE *freqMode);
MV_VOID mvCtrlSatrInit(MV_VOID);
MV_VOID mvCtrlSysConfigInit(MV_VOID);
MV_U32 mvCtrlSysConfigGet(MV_CONFIG_TYPE_ID configField);
MV_U32 mvCtrlGetCpuNum(MV_VOID);
MV_U32 mvCtrlGetQuadNum(MV_VOID);
MV_STATUS mvCtrlUpdatePexId(MV_VOID);
MV_BOOL mvCtrlIsValidSatR(MV_VOID);
MV_BOOL mvCtrlIsEepromEnabled(MV_VOID);
MV_STATUS mvCtrlEepromEnable(MV_BOOL enable);
MV_STATUS mvCtrlBoardConfigGet(MV_U8 *tempVal);
MV_U32 mvCtrlSocUnitInfoNumGet(MV_UNIT_ID unit);
MV_STATUS mvCtrlEnvInit(MV_VOID);
MV_U32    mvCtrlMppRegGet(MV_U32 mppGroup);
MV_U32 mvCtrlGetJuncTemp(MV_VOID);

#if defined(MV_INCLUDE_PEX)
MV_U32 mvCtrlPexMaxIfGet(MV_VOID);
MV_U32 mvCtrlPexMaxUnitGet(MV_VOID);
#else
#define mvCtrlPciMaxIfGet()             1
#endif

#if defined(MV_INCLUDE_PCI)
#define mvCtrlPciIfMaxIfGet()           1
#else
#define mvCtrlPciIfMaxIfGet()           0
#endif

MV_U32 mvCtrlEthMaxPortGet(MV_VOID);
#if defined(MV_INCLUDE_XOR)
MV_U32 mvCtrlXorMaxChanGet(MV_VOID);
MV_U32 mvCtrlXorMaxUnitGet(MV_VOID);
#endif
#if defined(MV_INCLUDE_USB)
MV_U32 mvCtrlUsbMaxGet(MV_VOID);
#endif
#if defined(MV_INCLUDE_SDIO)
MV_U32 mvCtrlSdioSupport(MV_VOID);
#endif
MV_U32 mvCtrlTdmSupport(MV_VOID);
MV_U32 mvCtrlTdmMaxGet(MV_VOID);
MV_TDM_UNIT_TYPE mvCtrlTdmUnitTypeGet(MV_VOID);
MV_U32 mvCtrlTdmUnitIrqGet(MV_VOID);
MV_U32 mvCtrlDevFamilyIdGet(MV_U16 ctrlModel);
MV_U16 mvCtrlModelGet(MV_VOID);
MV_U32 mvCtrlRevGet(MV_VOID);
MV_STATUS mvCtrlNameGet(char *pNameBuff);
MV_U32 mvCtrlModelRevGet(MV_VOID);
MV_STATUS mvCtrlModelRevNameGet(char *pNameBuff);
MV_VOID mvCtrlAddrDecShow(MV_VOID);
const MV_8 *mvCtrlTargetNameGet(MV_TARGET target);
MV_U32    ctrlSizeToReg(MV_U32 size, MV_U32 alignment);
MV_U32    ctrlRegToSize(MV_U32 regSize, MV_U32 alignment);
MV_U32    ctrlSizeRegRoundUp(MV_U32 size, MV_U32 alignment);
MV_U32 mvCtrlSysRstLengthCounterGet(MV_VOID);
MV_STATUS ctrlWinOverlapTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2);
MV_STATUS ctrlWinWithinWinTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2);
MV_VOID mvCtrlSmiMasterSet(MV_SMI_CTRL smiCtrl);
MV_VOID mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable);
MV_BOOL mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index);
MV_BOOL mvCtrlIsBootFromNOR(MV_VOID);
MV_BOOL mvCtrlIsBootFromSPI(MV_VOID);
MV_BOOL mvCtrlIsBootFromNAND(MV_VOID);
MV_U32 mvCtrlDDRBudWidth(MV_VOID);
MV_BOOL mvCtrlDDRThruXbar(MV_VOID);
MV_BOOL mvCtrlDDRECC(MV_VOID);

#endif /* __INCmvCtrlEnvLibh */
