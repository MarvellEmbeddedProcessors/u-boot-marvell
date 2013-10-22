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

#define ARRSZ(x)                (sizeof(x) / sizeof(x[0]))
#define BOARD_ETH_SWITCH_PORT_NUM       7
#define BOARD_ETH_SWITCH_SMI_SCAN_MODE	1	/* Use manual scanning mode */
#define MV_BOARD_MAX_MPP                59       /* number of MPP conf registers */
#define MV_BOARD_MAX_MPP_GROUPS         9
#define MV_BOARD_MPP_GROUPS_MAX_TYPES   8
#define MV_BOARD_NAME_LEN               0x20

typedef enum _devBoardOtherTypeClass {
	MV_BOARD_NONE		= 0x00000000,
	MV_BOARD_SPDIF		= 0x00000001,
	MV_BOARD_I2S		= 0x00000002,
	MV_BOARD_TDM		= 0x00000004,
	MV_BOARD_MII		= 0x00000008,
	MV_BOARD_SGMII		= 0x00000010,
	MV_BOARD_SERDES_MUX	= 0x00000020,
	MV_BOARD_NOR		= 0x00000040,
	MV_BOARD_NAND16BIT	= 0x00000080,
	MV_BOARD_SDIO4BIT	= 0x00000100,
	MV_BOARD_UNKNOWN	= 0x80000000
} MV_BOARD_OTHER_TYPE_CLASS;


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
	BOARD_DEV_TWSI_SATR,
	BOARD_TWSI_MODULE_DETECT,
	BOARD_DEV_TWSI_EEPROM,
	BOARD_TWSI_OTHER
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
	BOARD_GPP_PEX_RESET,
	BOARD_GPP_CONF,
	BOARD_GPP_PON_XVR_TX,
	BOARD_GPP_SYS_LED,
	BOARD_GPP_PON_LED,
	BOARD_GPP_OTHER
} MV_BOARD_GPP_CLASS;

typedef enum _mvSatRTypeID {
/*  "Bios" Device  */
	MV_SATR_CPU_DDR_L2_FREQ,
	MV_SATR_CORE_CLK_SELECT,
	MV_SATR_CPU1_ENABLE,
	MV_SATR_SSCG_DISABLE,
	/*  SW parameters: */
	MV_SATR_DDR4_SELECT,
	MV_SATR_DDR_BUS_WIDTH,
	MV_SATR_DDR_ECC_ENABLE,
	MV_SATR_NAND_DETECT,
	MV_SATR_RD_LANE1_2_CFG,
	MV_SATR_RD_LANE4_CFG,
	MV_SATR_RD_LANE0_CFG,
	MV_SATR_RD_APPS_CFG,
	MV_SATR_BOOT_DEVICE,
	MV_SATR_BOOT2_DEVICE,
	MV_SATR_BOARD_ID,
	MV_SATR_BOARD_ECO_VERSION,
	MV_SATR_MAX_OPTION,
} MV_SATR_TYPE_ID;


typedef struct _devCsInfo {
	MV_U8 deviceCS;
	MV_U32 params;
	MV_U32 devClass;        /* MV_BOARD_DEV_CLASS */
	MV_U8 devWidth;
	MV_U8 busWidth;
} MV_DEV_CS_INFO;

typedef enum _SatRstatus {
	BOARD_SATR_READ_ONLY = 0x01,
	BOARD_SATR_SWAP_BIT  = 0x02,
} MV_BOARD_SATR_STATUS;

typedef struct _boardLedInfo {
	MV_U8 activeLedsNumber;
	MV_U8 ledsPolarity;     /* '0' or '1' to turn on led */
	MV_U8 *gppPinNum;       /* Pointer to GPP values */
} MV_BOARD_LED_INFO;

typedef struct _boardGppInfo {
	MV_BOARD_GPP_CLASS devClass;
	MV_U8 gppPinNum;
} MV_BOARD_GPP_INFO;

typedef struct _boardTwsiInfo {
	MV_BOARD_TWSI_CLASS devClass;
	MV_U8 devClassId;
	MV_U8 twsiDevAddr;
	MV_U8 twsiDevAddrType;
} MV_BOARD_TWSI_INFO;

typedef struct _boardSatrInfo {
	MV_SATR_TYPE_ID satrId;
	MV_U32 mask;
	MV_U32 bitOffset;
	MV_U32 devClassId;
	MV_U32 regOffset;
	MV_U32 isActiveForBoard[MV_MAX_BOARD_ID];
	MV_BOARD_SATR_STATUS status;
} MV_BOARD_SATR_INFO;

typedef struct _boardConfigTypesInfo {
	MV_CONFIG_TYPE_ID configId;
	MV_U32 twsiAddr;
	MV_U32 offset;
	MV_U32 twsiId;
	MV_U32 isActiveForBoard[MV_MAX_BOARD_ID];
} MV_BOARD_CONFIG_TYPE_INFO;

typedef enum _boardMacSpeed {
	BOARD_MAC_SPEED_10M,
	BOARD_MAC_SPEED_100M,
	BOARD_MAC_SPEED_1000M,
	BOARD_MAC_SPEED_2000M,
	BOARD_MAC_SPEED_AUTO
} MV_BOARD_MAC_SPEED;

typedef struct _boardMacInfo {
	MV_BOARD_MAC_SPEED boardMacSpeed;
	MV_32 boardEthSmiAddr;
} MV_BOARD_MAC_INFO;

typedef struct _boardMppInfo {
	MV_U32 mppGroup[MV_BOARD_MAX_MPP];
} MV_BOARD_MPP_INFO;

typedef struct _boardMppTypeInfo {
	MV_BOARD_OTHER_TYPE_CLASS externalModule;
	MV_BOARD_MPP_INFO		  ModuleMpp;
} MV_BOARD_MPP_TYPE_INFO;

typedef enum {
	BOARD_EPON_CONFIG,
	BOARD_GPON_CONFIG,
	BOARD_PON_NONE,
	BOARD_PON_AUTO
} MV_BOARD_PON_CONFIG;

typedef struct {
	MV_U8 spiCs;
} MV_BOARD_TDM_INFO;

typedef struct _boardPexInfo {
	MV_PEXIF_INDX	pexMapping[MV_PEX_MAX_IF];
	MV_PEX_UNIT_CFG	pexUnitCfg[MV_PEX_MAX_UNIT];
	MV_U32		boardPexIfNum;
} MV_BOARD_PEX_INFO;

typedef enum _devBoardSlicType {
	MV_BOARD_SLIC_DISABLED,
	MV_BOARD_SLIC_SSI_ID, /* Lantiq Integrated SLIC */
	MV_BOARD_SLIC_ISI_ID, /* Silicon Labs ISI Bus */
	MV_BOARD_SLIC_ZSI_ID, /* Zarlink ZSI Bus */
	MV_BOARD_SLIC_EXTERNAL_ID /* Cross vendor external SLIC */
} MV_BOARD_SLIC_TYPE;

typedef struct {
	MV_U8 spiId;
} MV_BOARD_TDM_SPI_INFO;

typedef enum {
	BOARD_SLIC_880 = 0,
	BOARD_SLIC_792,
	BOARD_SLIC_SSI,
	BOARD_SLIC_ISI,
	BOARD_SLIC_ZSI,
	BOARD_TDM_SLIC_COUNT
} MV_BOARD_TDM_SLIC_TYPE;

/* Board specific initialization. Performed before initializing the SoC. */
typedef struct {
	MV_U32 reg;
	MV_U32 mask;
	MV_U32 val;
} MV_BOARD_SPEC_INIT;

typedef struct _boardInfo {
	char boardName[MV_BOARD_NAME_LEN];
	MV_U8 numBoardMppTypeValue;
	MV_BOARD_MPP_TYPE_INFO *pBoardModTypeValue;
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
	MV_U8 ledsPolarity;     /* '0' or '1' to turn on led */

	MV_U8 pmuPwrUpPolarity;
	MV_U32 pmuPwrUpDelay;
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

	/* PON configuration. */
	MV_BOARD_PON_CONFIG ponConfigValue;
	/* TDM configuration:
	 * We hold a different configuration array for each possible slic that
	 * can be connected to board.
	 * When modules are scanned, then we select the index of the relevant
	 * slic's information array.
	 * For RD and Customers boards we only need to initialize a single
	 * entry of the arrays below, and set the boardTdmInfoIndex to 0.
	 */
	MV_U8 numBoardTdmInfo[BOARD_TDM_SLIC_COUNT];
	MV_BOARD_TDM_INFO *pBoardTdmInt2CsInfo[BOARD_TDM_SLIC_COUNT];
	MV_16 boardTdmInfoIndex;

	/* Board specific initialization. Performed before initializing the SoC. */
	MV_BOARD_SPEC_INIT      *pBoardSpecInit;
	/* Deep-Idle power up delay */
	MV_U32 deepIdlePwrUpDelay;

	/* NAND init params */
	MV_U32 nandFlashReadParams;
	MV_U32 nandFlashWriteParams;
	MV_U32 nandFlashControl;
	MV_BOARD_TDM_SPI_INFO *pBoardTdmSpiInfo;
	MV_BOARD_PEX_INFO boardPexInfo;         /* filled in runtime */
	MV_U32 norFlashReadParams;
	MV_U32 norFlashWriteParams;

	/* Indicates if auto-detection of modules is enabled on this board. */
	/* Set to MV_FALSE for any board that is not a DB. */
	MV_BOOL configAutoDetect;
} MV_BOARD_INFO;

/* {{MV_CONFIG_TYPE_ID ConfigID, twsi-ID,  Offset, ID,  isActiveForBoard[]}} */
#define MV_BOARD_CONFIG_INFO { \
{ MV_CONFIG_SGMII,		0x2,	0,	 0xD,	{ 0, 1, 0} }, \
{ MV_CONFIG_MII,		0x1,	0,	 0x4,	{ 0, 1, 0} }, \
{ MV_CONFIG_SLIC_TDM_DEVICE,	0x0,	0,	 0x1,	{ 0, 1, 0} }, \
{ MV_CONFIG_I2S_DEVICE,		0x1,	0,	 0x3,	{ 0, 1, 0} }, \
{ MV_CONFIG_SPDIF_DEVICE,	0x1,	0,	 0x2,	{ 0, 1, 0} }, \
{ MV_CONFIG_SERDES_PEX_LAN1,	0x3,	0,	 0xC,	{ 0, 1, 0} }, \
{ MV_CONFIG_SERDES_PEX_LAN2,	0x3,	0,	 0xD,	{ 0, 1, 0} }, \
{ MV_CONFIG_NOR,		0x4,	0,	 0xF,	{ 0, 1, 0} }, \
{ MV_CONFIG_NAND,		0x4,	0,	 0x1,	{ 0, 1, 0} }, \
{ MV_CONFIG_SDIO,		0x4,	0,	 0x2,	{ 0, 1, 0} }, \
{ MV_CONFIG_GIGA,		0x5,	0,	 0xE,	{ 0, 1, 0} }, \
};

struct _mvBoardMppModule {
	MV_U32 group;
	MV_U32 mppValue;
};


#define MPP_MII_MODULE		{ {0, 0x12111111}, {1, 0x11111111}, {2, 0x11266011} }
#define MPP_TDM_MODULE		{ {6, 0x45333333}, {7, 0x00004444} }
#define MPP_I2S_MODULE		{6, 0x55544554}
#define MPP_SPDIF_MODULE	{6, 0x55444444}
#define MPP_NOR_MODULE		{ {0, 0x51111111}, {1, 0x11555555}, {2, 0x55566011}, \
				  {3, 0x55555055}, {4, 0x55555555}, {5, 0x40045525 } }
#define MPP_NAND_MODULE		{ {0, 0x55111111}, {1, 0x15555555}, {2, 0x55266011}, \
				  {3, 0x25255051}, {4, 0x25555555}, {5, 0x40042555 } }
#define MPP_SDIO_MODULE		{ {2, 0x11466011}, {3, 0x22242011}, {4, 0x44400002}, \
				  {5, 0x40042024 } }

/* Boot device bus width */
#define MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS          3
/* Bus width field meaning for NOR/NAND */
#define MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT          (0x0 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
#define MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT         (0x1 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
#define MSAR_0_BOOT_DEV_BUS_WIDTH_32BIT         (0x2 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
/* Bus width field meaning for SPI */
#define MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24BIT		(0x1 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
#define MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT     (0x0 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)

/* NAND page size */
#define MSAR_0_NAND_PAGE_SZ_OFFS                11
#define MSAR_0_NAND_PAGE_SZ_512B                (0x0 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_2KB                 (0x1 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_4KB                 (0x2 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_8KB                 (0x3 << MSAR_0_NAND_PAGE_SZ_OFFS)

/* NAND ECC */
#define MSAR_0_NAND_ECC_OFFS                    14
#define MSAR_0_NAND_ECC_4BIT                    (0x0 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_8BIT                    (0x1 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_12BIT                   (0x2 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_16BIT                   (0x3 << MSAR_0_NAND_ECC_OFFS)

#define MSAR_0_SPI0                             0
#define MSAR_0_SPI1                             1

MV_VOID mvBoardEnvInit(MV_VOID);
MV_U16 mvBoardModelGet(MV_VOID);
MV_U32 mvBoardRevGet(MV_VOID);
MV_STATUS mvBoardNameGet(char *pNameBuff, MV_U32 size);
MV_BOARD_SPEC_INIT *mvBoardSpecInitGet(MV_VOID);
MV_BOOL mvBoardIsPortInSgmii(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInGmii(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInRgmii(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortLoopback(MV_U32 ethPortNum);
MV_32 mvBoardPhyAddrGet(MV_U32 ethPortNum);
MV_VOID mvBoardPhyAddrSet(MV_U32 ethPortNum, MV_U32 smiAddr);
MV_STATUS mvBoardSatrInfoConfig(MV_SATR_TYPE_ID satrClass, MV_BOARD_SATR_INFO *satrInfo);
MV_STATUS mvBoardConfigTypeGet(MV_CONFIG_TYPE_ID configClass, MV_BOARD_CONFIG_TYPE_INFO *configInfo);
MV_STATUS mvBoardExtPhyBufferSelect(MV_BOOL enable);
MV_STATUS mvBoardSgmiiSfp0TxSet(MV_BOOL enable);
MV_U32 mvBoardTclkGet(MV_VOID);
MV_U32 mvBoardL2ClkGetRaw(MV_VOID);
MV_U32 mvBoardSysClkGet(MV_VOID);
MV_U32 mvBoardDebugLedNumGet(MV_U32 boardId);
MV_VOID mvBoardDebugLed(MV_U32 hexNum);
MV_32 mvBoarGpioPinNumGet(MV_BOARD_GPP_CLASS class, MV_U32 index);
MV_VOID mvBoardReset(MV_VOID);
MV_BOARD_PEX_INFO *mvBoardPexInfoGet(void);
MV_32 mvBoardResetGpioPinGet(MV_VOID);
MV_32 mvBoardUSBVbusGpioPinGet(MV_32 devId);
MV_32 mvBoardUSBVbusEnGpioPinGet(MV_32 devId);
MV_BOOL mvBoardIsOurPciSlot(MV_U32 busNum, MV_U32 slotNum);
MV_U32 mvBoardGpioIntMaskGet(MV_U32 gppGrp);
MV_U32 mvBoardSlicUnitTypeGet(MV_VOID);
MV_VOID mvBoardSlicUnitTypeSet(MV_U32 slicType);
MV_32 mvBoardMppGet(MV_U32 mppGroupNum);
MV_VOID mvBoardMppSet(MV_U32 mppGroupNum, MV_U32 mppValue);
MV_U8 mvBoardTdmSpiIdGet(MV_VOID);
MV_VOID mvBoardConfigurationPrint(MV_VOID);
MV_BOOL mvBoardIsGbEPortConnected(MV_U32 ethPortNum);
MV_32 mvBoardGetDevicesNumber(MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceBaseAddr(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceBusWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceWinSize(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_U32 boardGetDevCSNum(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_U8 mvBoardTwsiAddrTypeGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index);
MV_U8 mvBoardTwsiAddrGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index);
MV_U32 mvBoardEthComplexConfigGet(MV_VOID);
MV_VOID mvBoardEthComplexConfigSet(MV_U32 ethConfig);
MV_U32 mvBoardIdGet(MV_VOID);
MV_VOID mvBoardIdSet(MV_U32 boardId);
MV_U32 mvBoardSledCpuNumGet(MV_VOID);
MV_VOID mvBoardInfoUpdate(MV_VOID);
MV_VOID mvBoardVerifySerdesCofig(MV_VOID);
MV_VOID mvBoardMppIdUpdate(MV_VOID);
MV_STATUS mvBoardEthComplexInfoUpdate(MV_VOID);
MV_VOID mvBoardConfigWrite(MV_VOID);
MV_ETH_COMPLEX_TOPOLOGY mvBoardMac0ConfigGet(MV_VOID);
MV_ETH_COMPLEX_TOPOLOGY mvBoardMac1ConfigGet(MV_VOID);
MV_BOOL mvBoardLaneSGMIIGet(MV_ETH_COMPLEX_TOPOLOGY *sgmiiConfig);
MV_BOARD_BOOT_SRC mvBoardBootDeviceGroupSet(MV_VOID);
MV_BOARD_BOOT_SRC mvBoardBootDeviceGet(MV_VOID);
MV_U32 mvBoardBootAttrGet(MV_U32 satrBootDeviceValue, MV_U8 attrNum);
MV_STATUS mvBoardTwsiGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U8 devNum, MV_U8 regNum, MV_U8 *pData);
MV_STATUS mvBoardTwsiSet(MV_BOARD_TWSI_CLASS twsiClass, MV_U8 devNum, MV_U8 regNum, MV_U8 regVal);
MV_U8 mvBoardCpuFreqGet(MV_VOID);
MV_STATUS mvBoardCpuFreqSet(MV_U8 freqVal);
MV_STATUS mvBoardIsInternalSwitchConnected(void);
MV_U32 mvBoardSwitchPortForceLinkGet(MV_U32 switchIdx);
MV_U32 mvBoardFreqModesNumGet(void);
MV_32 mvBoardSmiScanModeGet(MV_U32 switchIdx);
MV_BOARD_MAC_SPEED mvBoardMacSpeedGet(MV_U32 ethPortNum);
MV_VOID mvBoardMacSpeedSet(MV_U32 ethPortNum, MV_BOARD_MAC_SPEED macSpeed);
MV_U32 mvBoardSwitchCpuPortGet(MV_U32 switchIdx);
MV_U32 mvBoardMacCpuPortGet(MV_VOID);
MV_BOOL mvBoardIsEthConnected(MV_U32 ethNum);
MV_32 mvBoardSwitchIrqGet(MV_VOID);
MV_U32 mvBoardSwitchPortsMaskGet(MV_U32 switchIdx);
MV_BOOL mvBoardConfigAutoDetectEnabled(void);
MV_32 mvBoardSmiScanModeGet(MV_U32 switchIdx);
MV_STATUS mvBoardConfIdSet(MV_U16 conf);
MV_U16 mvBoardPexModeGet(MV_VOID);
MV_STATUS mvBoardPexModeSet(MV_U16 conf);
MV_BOOL mvBoardIsLcdDviModuleConnected(void);
MV_BOOL mvBoardIsPexModuleConnected(void);
MV_BOOL mvBoardIsLvdsModuleConnected(void);
MV_BOOL mvBoardIsSetmModuleConnected(void);
MV_U8 mvBoardCpuCoresNumGet(MV_VOID);
MV_VOID mvBoardMppModuleTypePrint(MV_VOID);
MV_VOID mvBoardOtherModuleTypePrint(MV_VOID);
MV_BOOL mvBoardIsModuleConnected(MV_U32 ModuleID);

MV_STATUS mvBoardTwsiSatRGet(MV_U8 devNum, MV_U8 regNum, MV_U8 *pData);
MV_STATUS mvBoardTwsiSatRSet(MV_U8 devNum, MV_U8 regNum, MV_U8 regVal);
MV_U32 mvBoardSatRRead(MV_SATR_TYPE_ID satrField);
MV_STATUS mvBoardSatRWrite(MV_SATR_TYPE_ID satrWriteField, MV_U8 val);

/*    SATR-ID                   Mask    bit    TWSI   Reg  board	*/
/*    SATR-ID                   Mask  offset  devID  num  active	*/
#define MV_SAR_INFO { \
{ MV_SATR_CPU_DDR_L2_FREQ,	0x1F,	0,	1,	0,	{1, 1, 0}, BOARD_SATR_SWAP_BIT},\
{ MV_SATR_CORE_CLK_SELECT,	0x08,	3,	3,	0,	{0, 1, 0}, 0},\
{ MV_SATR_CPU1_ENABLE,		0x01,	0,	2,	0,	{0, 1, 0}, 0},\
{ MV_SATR_SSCG_DISABLE,		0x10,	4,	2,	0,	{0, 1, 0}, 0},\
{ MV_SATR_DDR4_SELECT,		0x20,	5,	4,	1,	{0, 1, 0}, BOARD_SATR_READ_ONLY},\
{ MV_SATR_DDR_BUS_WIDTH,	0x02,	1,	4,	0,	{0, 1, 0}, BOARD_SATR_READ_ONLY},\
{ MV_SATR_DDR_ECC_ENABLE,	0x04,	2,	4,	0,	{0, 1, 0}, 0},\
{ MV_SATR_NAND_DETECT,		0x1,	0,	4,	1,	{0, 1, 0}, BOARD_SATR_READ_ONLY},\
{ MV_SATR_RD_LANE1_2_CFG,	0x1,	0,	1,	1,	{1, 0, 0}, 0},\
{ MV_SATR_RD_LANE4_CFG,		0x2,	1,	1,	1,	{1, 0, 0}, 0},\
{ MV_SATR_RD_LANE0_CFG,		0x4,	2,	1,	1,	{1, 0, 0}, 0},\
{ MV_SATR_RD_APPS_CFG,		0x8,	3,	1,	1,	{1, 0, 0}, 0},\
{ MV_SATR_BOOT_DEVICE,		0x3,	0,	3,	0,	{0, 1, 0}, BOARD_SATR_SWAP_BIT},\
{ MV_SATR_BOOT2_DEVICE,		0x1E,	1,	4,	0,	{0, 1, 0}, BOARD_SATR_SWAP_BIT},\
{ MV_SATR_BOARD_ID,		0x7,	0,	0,	0,	{1, 1, 0}, BOARD_SATR_READ_ONLY},\
{ MV_SATR_BOARD_ECO_VERSION,	0xff,	0,	0,	1,	{1, 1, 0}, BOARD_SATR_READ_ONLY},\
{ MV_SATR_MAX_OPTION,		0x0,	0,	0,	0,	{0, 0, 0}, 0},\
};


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __INCmvBoardEnvLibh */
