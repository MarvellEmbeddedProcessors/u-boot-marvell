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
#ifndef __INCmvBoardEnvLibh
#define __INCmvBoardEnvLibh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */
/* The below constant macros defines the board I2C EEPROM data offsets */

#include "ctrlEnv/mvCtrlEnvLib.h"
#include "mvSysHwConfig.h"
#include "boardEnv/mvBoardEnvSpec.h"
#include "twsi/mvTwsi.h"
#include "nfc/mvNfc.h"

/* DUART stuff for Tclk detection only */
#define DUART_BAUD_RATE			115200
#define MAX_CLOCK_MARGINE		5000000	/* Maximum detected clock margine */

#define MV_BOARD_MAX_MPP		9	/* number of MPP conf registers */
#define MV_BOARD_NAME_LEN  		0x20

typedef enum _devBoardMppTypeClass {
	MV_BOARD_AUTO = 0,
	MV_BOARD_MII_GMII,
	MV_BOARD_OTHER
} MV_BOARD_MPP_TYPE_CLASS;

typedef struct _boardModuleTypeInfo {
	MV_U32 boardMppMod;
} MV_BOARD_MODULE_TYPE_INFO;

typedef enum _devBoardClass {
	BOARD_DEV_NOR_FLASH,
	BOARD_DEV_NAND_FLASH,
	BOARD_DEV_SEVEN_SEG,
	BOARD_DEV_FPGA,
	BOARD_DEV_SRAM,
	BOARD_DEV_SPI_FLASH,
	BOARD_DEV_OTHER
} MV_BOARD_DEV_CLASS;

typedef enum _devTwsiBoardClass {
	BOARD_DEV_TWSI_PLD,
	BOARD_DEV_TWSI_ZARLINK,
	BOARD_DEV_TWSI_SATR,
	BOARD_DEV_TWSI_INIT_EPROM,
	BOARD_DEV_TWSI_PCA9555_IO_EXPANDER,
	BOARD_DEV_TWSI_PCA9548_IO_MUX
} MV_BOARD_TWSI_CLASS;

typedef enum _devGppBoardClass {
	BOARD_GPP_RTC,
	BOARD_GPP_MV_SWITCH,
	BOARD_GPP_USB_VBUS,
	BOARD_GPP_USB_VBUS_EN,
	BOARD_GPP_USB_OC,
	BOARD_GPP_USB_HOST_DEVICE,
	BOARD_GPP_REF_CLCK,
	BOARD_GPP_VOIP_SLIC,
	BOARD_GPP_LIFELINE,
	BOARD_GPP_BUTTON,
	BOARD_GPP_TS_BUTTON_C,
	BOARD_GPP_TS_BUTTON_U,
	BOARD_GPP_TS_BUTTON_D,
	BOARD_GPP_TS_BUTTON_L,
	BOARD_GPP_TS_BUTTON_R,
	BOARD_GPP_POWER_BUTTON,
	BOARD_GPP_RESTOR_BUTTON,
	BOARD_GPP_WPS_BUTTON,
	BOARD_GPP_HDD0_POWER,
	BOARD_GPP_HDD1_POWER,
	BOARD_GPP_FAN_POWER,
	BOARD_GPP_RESET,
	BOARD_GPP_POWER_ON_LED,
	BOARD_GPP_HDD_POWER,
	BOARD_GPP_SDIO_POWER,
	BOARD_GPP_SDIO_DETECT,
	BOARD_GPP_SDIO_WP,
	BOARD_GPP_SWITCH_PHY_INT,
	BOARD_GPP_TSU_DIRCTION,
	BOARD_GPP_CONF,
	BOARD_GPP_OTHER
} MV_BOARD_GPP_CLASS;

typedef struct _devCsInfo {
	MV_U8 deviceCS;
	MV_U32 params;
	MV_U32 devClass;	/* MV_BOARD_DEV_CLASS */
	MV_U8 devWidth;
	MV_U8 busWidth;
} MV_DEV_CS_INFO;

typedef struct _boardSwitchInfo {
	MV_32 switchIrq;
	MV_32 switchPort[BOARD_ETH_SWITCH_PORT_NUM];
	MV_32 cpuPort;
	MV_32 connectedPort[MV_ETH_MAX_PORTS];
	MV_32 smiScanMode;
	MV_32 quadPhyAddr;
	MV_U32 forceLinkMask; /* Bitmask of switch ports to have force link (1Gbps) */
} MV_BOARD_SWITCH_INFO;

typedef struct _boardGppInfo {
	MV_BOARD_GPP_CLASS devClass;
	MV_U8 gppPinNum;
} MV_BOARD_GPP_INFO;

typedef struct _boardTwsiInfo {
	MV_BOARD_TWSI_CLASS devClass;
	MV_U8 twsiDevAddr;
	MV_U8 twsiDevAddrType;
} MV_BOARD_TWSI_INFO;

typedef enum _boardMacSpeed {
	BOARD_MAC_SPEED_10M,
	BOARD_MAC_SPEED_100M,
	BOARD_MAC_SPEED_1000M,
	BOARD_MAC_SPEED_AUTO
} MV_BOARD_MAC_SPEED;

typedef struct _boardMacInfo {
	MV_BOARD_MAC_SPEED boardMacSpeed;
	MV_32	boardEthSmiAddr;
	MV_32 boardEthSmiAddr0;
} MV_BOARD_MAC_INFO;

typedef struct _boardMppInfo {
	MV_U32 mppGroup[MV_BOARD_MAX_MPP];
} MV_BOARD_MPP_INFO;

typedef struct _boardPexUnitCfg {
	MV_PEX_UNIT_CFG 	pexCfg;
	MV_U8			pexLaneStat[4];	/* 1: enabled, 2: disabled */
} MV_BOARD_PEX_UNIT_CFG;

typedef struct _boardPexInfo {
	MV_PEXIF_INDX 		pexMapping[MV_PEX_MAX_IF];
	MV_BOARD_PEX_UNIT_CFG 	pexUnitCfg[MV_PEX_MAX_UNIT];
	MV_U32			boardPexIfNum;
} MV_BOARD_PEX_INFO;

typedef struct _boardInfo {
	char boardName[MV_BOARD_NAME_LEN];
	MV_U8 numBoardMppTypeValue;
	MV_BOARD_MODULE_TYPE_INFO *pBoardModTypeValue;
	MV_U8 numBoardMppConfigValue;
	MV_BOARD_MPP_INFO *pBoardMppConfigValue;
	MV_U32 intsGppMaskLow;
	MV_U32 intsGppMaskMid;
	MV_U32 intsGppMaskHigh;
	MV_U8 numBoardDeviceIf;
	MV_DEV_CS_INFO *pDevCsInfo;
	MV_U8 numBoardTwsiDev;
	MV_BOARD_TWSI_INFO *pBoardTwsiDev;
	MV_U8 numBoardMacInfo;
	MV_BOARD_MAC_INFO *pBoardMacInfo;
	MV_U8 numBoardGppInfo;
	MV_BOARD_GPP_INFO *pBoardGppInfo;
	MV_U8 activeLedsNumber;
	MV_U8 *pLedGppPin;
	MV_U8 ledsPolarity;	/* '0' or '1' to turn on led */

	MV_U8	pmuPwrUpPolarity;
	MV_U32	pmuPwrUpDelay;
	/* GPP values */
	MV_U32 gppOutEnValLow;
	MV_U32 gppOutEnValMid;
	MV_U32 gppOutEnValHigh;
	MV_U32 gppOutValLow;
	MV_U32 gppOutValMid;
	MV_U32 gppOutValHigh;
	MV_U32 gppPolarityValLow;
	MV_U32 gppPolarityValMid;
	MV_U32 gppPolarityValHigh;

	/* External Switch Configuration */
	MV_BOARD_SWITCH_INFO *pSwitchInfo;
	MV_U32 switchInfoNum;

	/* NAND init params */
	MV_U32 nandFlashReadParams;
	MV_U32 nandFlashWriteParams;
	MV_U32 nandFlashControl;
	MV_BOARD_PEX_INFO	boardPexInfo;	/* filled in runtime */
	MV_U32 norFlashReadParams;
	MV_U32 norFlashWriteParams;

} MV_BOARD_INFO;

MV_VOID mvBoardEnvInit(MV_VOID);
MV_U16 mvBoardModelGet(MV_VOID);
MV_U16 mvBoardRevGet(MV_VOID);
MV_STATUS mvBoardNameGet(char *pNameBuff);
MV_BOOL mvBoardIsEthConnected(MV_U32 ethNum);
MV_BOOL mvBoardIsEthActive(MV_U32 ethNum);
MV_BOOL mvBoardIsPortInSgmii(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInGmii(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInMii(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInRgmii(MV_U32 ethPortNum);
MV_32 mvBoardPhyAddrGet(MV_U32 ethPortNum);
MV_32 mvBoardQuadPhyAddr0Get(MV_U32 ethPortNum);
MV_32 mvBoardSwitchCpuPortGet(MV_U32 switchIdx);
MV_32 mvBoardSmiScanModeGet(MV_U32 switchIdx);
MV_U32 mvBoardTclkGet(MV_VOID);
MV_U32 mvBoardSysClkGet(MV_VOID);
MV_32 mvBoarGpioPinNumGet(MV_BOARD_GPP_CLASS class, MV_U32 index);
MV_VOID mvBoardReset(MV_VOID);
MV_32 mvBoardResetGpioPinGet(MV_VOID);
MV_32 mvBoardSDIOGpioPinGet(MV_BOARD_GPP_CLASS type);
MV_U32 mvBoardGpioIntMaskGet(MV_U32 gppGrp);
MV_32 mvBoardMppGet(MV_U32 mppGroupNum);
MV_U32 mvBoardGppConfigGet(void);
MV_VOID mvBoardMppModuleTypePrint(MV_VOID);
MV_VOID mvBoardOtherModuleTypePrint(MV_VOID);
MV_BOOL mvBoardIsGbEPortConnected(MV_U32 ethPortNum);
MV_32 mvBoardGetDevicesNumber(MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceBaseAddr(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceBusWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceWinSize(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_U32 boardGetDevCSNum(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_U8 mvBoardTwsiAddrTypeGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index);
MV_U8 mvBoardTwsiAddrGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index);
MV_32 mvBoardNandWidthGet(void);
MV_VOID mvBoardSet(MV_U32 boardId);
MV_U32 mvBoardIdGet(MV_VOID);
MV_U32 mvBoardSledCpuNumGet(MV_VOID);

MV_STATUS mvBoardTwsiWrite(MV_BOARD_TWSI_CLASS twsiClass, MV_U8 devNum, MV_U8 regNum, MV_U8 regVal);
MV_STATUS mvBoardTwsiRead(MV_BOARD_TWSI_CLASS class1, MV_U8 devNum, MV_U8 regNum, MV_U8 *pData);
MV_STATUS mvBoardTwsiSatRGet(MV_U8 devNum, MV_U8 regNum, MV_U8 *pData);
MV_STATUS mvBoardTwsiSatRSet(MV_U8 devNum, MV_U8 regNum, MV_U8 regVal);
MV_STATUS mvBoardCpuFreqGet(MV_U8 *value);
MV_STATUS mvBoardCpuFreqSet(MV_U8 freqVal);
MV_U8 mvBoardCpuFreqModeGet(MV_VOID);
MV_STATUS mvBoardCpuFreqModeSet(MV_U8 freqVal);
MV_STATUS mvBoardBootDevGet(MV_U8 *value);
MV_STATUS mvBoardBootDevSet(MV_U8 val);
MV_STATUS mvBoardDeviceIdGet(MV_U8 *value);
MV_STATUS mvBoardDeviceIdSet(MV_U8 val);
MV_STATUS mvBoardDeviceNumGet(MV_U8 *value);
MV_STATUS mvBoardDeviceNumSet(MV_U8 val);
MV_STATUS mvBoardPcieModeGet(MV_U8 *val);
MV_STATUS mvBoardPcieModeSet(MV_U8 val);
MV_STATUS mvBoardPcieClockGet(MV_U8 *val);
MV_STATUS mvBoardPcieClockSet(MV_U8 val);
MV_STATUS mvBoardPllClockGet(MV_U8 *val);
MV_STATUS mvBoardPllClockSet(MV_U8 val);
MV_STATUS mvBoardSarBoardIdGet(MV_U8 *val);
MV_STATUS mvBoardSarBoardIdSet(MV_U8 val);

MV_U8 mvBoardCpuCoresNumGet(MV_VOID);
MV_STATUS mvBoardCpuCoresNumSet(MV_U8 val);
MV_STATUS mvBoardConIdSet(MV_U16 conf);
MV_U16 mvBoardConfIdGet(MV_VOID);
MV_STATUS mvBoardDramBusWidthSet(MV_U16 conf);
MV_U16 mvBoardDramBusWidthGet(MV_VOID);
MV_STATUS mvBoardCoreFreqGet(MV_U8 *value);
MV_STATUS mvBoardTmFreqGet(MV_U8 *value);
MV_STATUS mvBoardTmFreqSet(MV_U8 freqVal);
MV_STATUS mvBoardCoreFreqSet(MV_U8 freqVal);
MV_STATUS mvBoardDdrEccEnableGet(MV_U8 *val);
MV_STATUS mvBoardDdrEccEnableSet(MV_U8 val);

MV_STATUS mvBoardMppModulesScan(void);
MV_STATUS mvBoardOtherModulesScan(void);
MV_BOOL mvBoardIsPexModuleConnected(void);
MV_BOOL mvBoardIsSetmModuleConnected(void);
MV_BOOL mvBoardIsSwitchModuleConnected(void);
MV_BOOL mvBoardIsLvdsModuleConnected(void);
MV_BOOL mvBoardIsLcdDviModuleConnected(void);
MV_BOOL mvBoardIsGMIIModuleConnected(void);
MV_STATUS mvBoardTwsiMuxChannelSet(MV_U8 muxChNum);
MV_STATUS mvBoardTwsiReadByteThruMux(MV_U8 muxChNum, MV_U8 chNum, MV_TWSI_SLAVE *pTwsiSlave, MV_U8 *data);
MV_BOARD_MAC_SPEED mvBoardMacSpeedGet(MV_U32 ethPortNum);
MV_BOARD_PEX_INFO *mvBoardPexInfoGet(void);
MV_STATUS mvBoardConfIdSet(MV_U16 conf);
MV_U16 mvBoardPexModeGet(MV_VOID);
MV_STATUS mvBoardPexModeSet(MV_U16 conf);
MV_VOID mvBoardDebugLed(MV_U32 hexNum);
MV_NFC_ECC_MODE mvBoardNandECCModeGet(void);
MV_U32 mvBoardCpssBoardIdSet(MV_U8);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __INCmvBoardEnvLibh */
