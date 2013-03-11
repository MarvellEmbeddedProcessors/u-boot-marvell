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

/* DUART stuff for Tclk detection only */
#define DUART_BAUD_RATE                 115200
#define MAX_CLOCK_MARGINE               5000000 /* Maximum detected clock margin */

/* Voice devices assembly modes */
#define DAISY_CHAIN_MODE                1
#define DUAL_CHIP_SELECT_MODE           0
#define INTERRUPT_TO_MPP                1
#define INTERRUPT_TO_TDM                0

#define BOARD_ETH_PORT_NUM  MV_ETH_MAX_PORTS
#define BOARD_ETH_SWITCH_PORT_NUM       8
#define BOARD_ETH_QD_SWITCH_PORT_NUM    5
#define BOARD_ETH_SWITCH_CPUPORT_NUM    2
#define MV_BOARD_MAX_USB_IF             3
#define MV_BOARD_MAX_MPP                9       /* number of MPP conf registers */
#define MV_BOARD_MAX_MPP_GROUPS         9
#define MV_BOARD_NAME_LEN               0x20

/* EPPROM Modules detection information */

#define MV_BOARD_EEPROM_MODULE_ADDR             0x50
#define MV_BOARD_EEPROM_MODULE_ADDR_TYPE        ADDR7_BIT

#define MV_BOARD_PEX_MODULE_ADDR                0x23
#define MV_BOARD_PEX_MODULE_ADDR_TYPE           ADDR7_BIT
#define MV_BOARD_PEX_MODULE_ID                  0

#define MV_BOARD_LVDS_MODULE_ADDR               0x21
#define MV_BOARD_LVDS_MODULE_ADDR_TYPE          ADDR7_BIT
#define MV_BOARD_LVDS_MODULE_ID                 0

#define MV_BOARD_SETM_MODULE_ADDR               0x23
#define MV_BOARD_SETM_MODULE_ADDR_TYPE          ADDR7_BIT
#define MV_BOARD_SETM_MODULE_ID                 1

#define MV_BOARD_MPP_MODULE_ADDR                0x20
#define MV_BOARD_MPP_MODULE_ADDR_TYPE           ADDR7_BIT
#define MV_BOARD_LCD_DVI_MODULE_ID              0
#define MV_BOARD_TDM_MODULE_ID                  1
#define MV_BOARD_MII_GMII_MODULE_ID             4

typedef struct _boardData {
	MV_U32 magic;
	MV_U16 boardId;
	MV_U8 boardVer;
	MV_U8 boardRev;
	MV_U32 reserved1;
	MV_U32 reserved2;
} BOARD_DATA;

typedef enum _devBoardMppGroupClass {
	MV_BOARD_MPP_GROUP_1,
	MV_BOARD_MPP_GROUP_2,
	MV_BOARD_MPP_GROUP_3,
	MV_BOARD_MAX_MPP_GROUP
} MV_BOARD_MPP_GROUP_CLASS;

typedef enum _devBoardMppTypeClass {
	MV_BOARD_AUTO,
	MV_BOARD_TDM_2CH,
	MV_BOARD_TDM_32CH,
	MV_BOARD_RGMII0,
	MV_BOARD_RGMII1,
	MV_BOARD_SW_P4,
	MV_BOARD_OTHER
} MV_BOARD_MPP_TYPE_CLASS;

typedef enum _devBoardOtherTypeClass {
	MV_BOARD_NONE    = 0x00000000,
	MV_BOARD_LVDS    = 0x00000001,
	MV_BOARD_PEX     = 0x00000002,
	MV_BOARD_SWITCH  = 0x00000004,
	MV_BOARD_SETM    = 0x00000008,
	MV_BOARD_UNKNOWN = 0x80000000
} MV_BOARD_OTHER_TYPE_CLASS;

#define MV_BOARD_TDM    MV_BOARD_TDM_32CH

typedef struct _boardModuleTypeInfo {
	MV_BOARD_MPP_TYPE_CLASS boardMppMod;
	MV_BOARD_OTHER_TYPE_CLASS boardOtherMod;
} MV_BOARD_MODULE_TYPE_INFO;

/* omriii:  decide between MODULE_TYPE or MPP_TYPE */
typedef struct _boardMppTypeInfo {
	MV_BOARD_MPP_TYPE_CLASS boardMppTdm;

	/* Ethernet / Sata complex                      */
	/* A bitmask of MV_ETH_SATA_COMPLEX_OPTIONS     */
	MV_U32 ethSataComplexOpt;
	MV_U32 ethPortsMode;    /* bitmask of MV_ETH_PORT_MODE */
} MV_BOARD_MPP_TYPE_INFO;

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
	BOARD_TWSI_RTC,
	BOARD_DEV_TWSI_EXP,
	BOARD_DEV_TWSI_SATR,
	BOARD_TWSI_MUX,
	BOARD_DEV_TWSI_EEPROM,          //omriii : _EEPROM should be added ? or used as _OTHER ?
	BOARD_DEV_TWSI_IO_EXPANDER,     //omriii : _IO_EXPANDER should be added ? or used as _OTHER?
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

typedef struct _devCsInfo {
	MV_U8 deviceCS;
	MV_U32 params;
	MV_U32 devClass;        /* MV_BOARD_DEV_CLASS */
	MV_U8 devWidth;
	MV_U8 busWidth;
} MV_DEV_CS_INFO;

typedef struct _boardSwitchInfo {
	MV_32 switchIrq;
	MV_32 switchPort[BOARD_ETH_SWITCH_PORT_NUM];
	MV_32 cpuPort;
	MV_32 connectedPort[MV_ETH_MAX_PORTS];
	MV_32 smiScanMode;
	MV_32 quadPhyAddr[BOARD_ETH_QD_SWITCH_PORT_NUM];
	MV_U32 forceLinkMask; /* Bitmask of switch ports to have force link (1Gbps) */
} MV_BOARD_SWITCH_INFO;

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
	MV_U8 twsiDevAddr;
	MV_U8 twsiDevAddrType;
} MV_BOARD_TWSI_INFO;

typedef struct _boardSarInfo {
	MV_SATR_TYPE_ID sarid;
	MV_U32 mask;
	MV_U32 offset;
	MV_U32 regNum;
} MV_BOARD_SAR_INFO;

typedef struct _boardConfigTypesInfo {
	MV_CONFIG_TYPE_ID configid;
	MV_U32 mask;
	MV_U32 offset;
	MV_U32 regNum;
} MV_BOARD_CONFIG_TYPE_INFO;

typedef enum _boardMacSpeed {
	BOARD_MAC_SPEED_10M,
	BOARD_MAC_SPEED_100M,
	BOARD_MAC_SPEED_1000M,
	BOARD_MAC_SPEED_AUTO
} MV_BOARD_MAC_SPEED;

typedef struct _boardMacInfo {
	MV_BOARD_MAC_SPEED boardMacSpeed;
	MV_U8 boardEthSmiAddr;
	MV_U16 LinkCryptPortAddr;
	MV_U8 boardEthSmiAddr0;
} MV_BOARD_MAC_INFO;

typedef struct _boardMppInfo {
	MV_U32 mppGroup[MV_BOARD_MAX_MPP];
} MV_BOARD_MPP_INFO;

typedef struct _boardMppGroupInfo {
	MV_U32 mppGroupType[MV_BOARD_MAX_MPP_GROUPS];
} MV_BOARD_MPP_GROUP_INFO;

typedef enum {
	BOARD_EPON_CONFIG,
	BOARD_GPON_CONFIG,
	BOARD_PON_NONE,
	BOARD_PON_AUTO
} MV_BOARD_PON_CONFIG;

typedef struct {
	MV_U8 spiCs;
} MV_BOARD_TDM_INFO;

typedef struct _boardPexUnitCfg {
	MV_PEX_UNIT_CFG pexCfg;
	MV_U8 pexLaneStat[4];                   /* 1: enabled, 2: disabled */
} MV_BOARD_PEX_UNIT_CFG;

typedef struct _boardPexInfo {
	MV_PEXIF_INDX pexMapping[MV_PEX_MAX_IF];
	MV_BOARD_PEX_UNIT_CFG pexUnitCfg[MV_PEX_MAX_UNIT];
	MV_U32 boardPexIfNum;
} MV_BOARD_PEX_INFO;

typedef struct {
	MV_U8 spiId;
} MV_BOARD_TDM_SPI_INFO;

typedef enum {
	BOARD_TDM_SLIC_880 = 0,
	BOARD_TDM_SLIC_792,
	BOARD_TDM_SLIC_3215,
	BOARD_TDM_SLIC_OTHER,
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
	MV_U8 numBoardMppGroupValue;
	MV_BOARD_MPP_GROUP_INFO **pBoardMppGroupValue;
	MV_U8 numBoardSerdesConfigValue;
	MV_SERDES_CFG *pBoardSerdesConfigValue;
	MV_U32 intsGppMaskLow;
	MV_U32 intsGppMaskMid;
	MV_U32 intsGppMaskHigh;
	MV_U8 numBoardDeviceIf;
	MV_DEV_CS_INFO *pDevCsInfo;
	MV_U8 numBoardSarInfo;
	MV_BOARD_SAR_INFO *pBoardSarInfo;
	MV_U8 numBoardConfigTypes;
	MV_BOARD_CONFIG_TYPE_INFO *pBoardConfigTypes;
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

	/* External Switch Configuration */
	MV_BOARD_SWITCH_INFO *pSwitchInfo;
	MV_U32 switchInfoNum;

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
	MV_BOARD_PEX_INFO	boardPexInfo;	/* filled in runtime */
	MV_U32 norFlashReadParams;
	MV_U32 norFlashWriteParams;

	/* Indicates if auto-detection of modules is enabled on this board. */
	/* Set to MV_FALSE for any board that is not a DB. */
	MV_BOOL moduleAutoDetect;
} MV_BOARD_INFO;

typedef enum {
	MSAR_0_BOOT_NOR_FLASH,
	MSAR_0_BOOT_NAND_NEW,
	MSAR_0_BOOT_UART,
	MSAR_0_BOOT_SPI_FLASH,
	MSAR_0_BOOT_PEX,
	MSAR_0_BOOT_SATA,
	MSAR_0_BOOT_NAND_LEGACY,
	MSAR_0_BOOT_PROMPT,
	MSAR_0_BOOT_SPI1_FLASH
} MV_BOARD_BOOT_SRC;

/* Boot device bus width */
#define MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS          3
/* Bus width field meaning for NOR/NAND */
#define MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT          (0x0 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
#define MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT         (0x1 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
#define MSAR_0_BOOT_DEV_BUS_WIDTH_32BIT         (0x2 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
/* Bus width field meaning for SPI */
#define MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24_16BIT  (0x1 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
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

typedef struct _mvSARBootTable {
	MV_BOARD_BOOT_SRC bootSrc;
	MV_U32 attr1;                           /* Device width/Port */
	MV_U32 attr2;                           /* ALE TIming Parameters/Page Size/Serdes Lane/Address cycles */
	MV_U32 attr3;                           /* Dev_Wen Dev_Oen Muxed/Address cycle/SPI interface */
} MV_SAR_BOOT_TABLE;

#define MV_SAR_TABLE_VAL { \
/*00*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, 0, 0 }, \
/*01*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,             0, 1 }, \
/*02*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, 0, 0 }, \
/*03*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,             0, 1 }, \
/*04*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, 0, 0 }, \
/*05*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,             0, 1 }, \
/*06*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, 0, 0 }, \
/*07*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT,             0, 1 }, \
/*08*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 0, 0 }, \
/*09*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 6, 1 }, \
/*10*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 0, 0 }, \
/*11*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 6, 1 }, \
/*12*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 4, 0 }, \
/*13*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 6, 1 }, \
/*14*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 5, 0 }, \
/*15*/ { MSAR_0_BOOT_NOR_FLASH,  MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT, 5, 1 }, \
/*16*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_512B, MSAR_0_NAND_ECC_4BIT }, \
/*17*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_512B, MSAR_0_NAND_ECC_4BIT }, \
/*18*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_4BIT },	\
/*19*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_8BIT },	\
/*20*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_12BIT }, \
/*21*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_2KB, MSAR_0_NAND_ECC_16BIT }, \
/*22*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_4BIT },	\
/*23*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_8BIT },	\
/*24*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_12BIT }, \
/*25*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_4KB, MSAR_0_NAND_ECC_16BIT }, \
/*26*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_4BIT },	\
/*27*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_8BIT },	\
/*28*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_12BIT }, \
/*29*/ { MSAR_0_BOOT_NAND_NEW, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB, MSAR_0_NAND_ECC_16BIT }, \
/*30*/ { 0,           5,            60,          60           }, \
/*31*/ { 0,           2,            40,          40           }, \
/*32*/ { MSAR_0_BOOT_NAND_LEGACY, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_512B,     2                }, \
/*33*/ { MSAR_0_BOOT_NAND_LEGACY, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB,       2                }, \
/*34*/ { MSAR_0_BOOT_NAND_LEGACY, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_512B,     3                }, \
/*35*/ { MSAR_0_BOOT_NAND_LEGACY, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB,       3                }, \
/*36*/ { MSAR_0_BOOT_NAND_LEGACY, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_512B,     3                }, \
/*37*/ { MSAR_0_BOOT_NAND_LEGACY, MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT, MSAR_0_NAND_PAGE_SZ_8KB,       2                }, \
/*38*/ { 0,           1,            2,            2              }, \
/*39*/ { 0,           3,            6,            6              }, \
/*40*/ { 0,           3,            5,            5              }, \
/*41*/ { 0,           2,            6,            3              }, \
/*42*/ { 0,           4,            10,          5              }, \
/*43*/ { 0,           3,            6,            6              }, \
/*44*/ { 0,           2,            4,            4              }, \
/*45*/ { 0,           3,            6,            3              }, \
/*46*/ { 0,           2,            5,            5              }, \
/*47*/ { MSAR_0_BOOT_PROMPT,         2,            5,            5              }, \
/*48*/ { MSAR_0_BOOT_UART,               1,            3,            3              }, \
/*49*/ { MSAR_0_BOOT_SATA,                5,            10,          10           }, \
/*50*/ { MSAR_0_BOOT_PEX,   3,            8,            4              }, \
/*51*/ { MSAR_0_BOOT_PEX,   1,            2,            1              }, \
/*52*/ { 0,           3,            6,            3              }, \
/*53*/ { 0,           2,            8,            4              }, \
/*54*/ { 0,           5,            10,          5              }, \
/*55*/ { 0,           1,            20,          20           }, \
/*56*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24_16BIT, 60        }, \
/*57*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 60               }, \
/*58*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI1, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24_16BIT, 60        }, \
/*59*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI1, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 60               }, \
/*60*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24_16BIT, 60        }, \
/*61*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI0, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 60               }, \
/*62*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI1, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24_16BIT, 60        }, \
/*63*/ { MSAR_0_BOOT_SPI_FLASH,     MSAR_0_SPI1, MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT, 40               } \
}

#define FREQ_MODES_NUM		29
typedef struct {
	MV_U8 id;
	char *cpuFreq;
	char *ddrFreq;
	char *l2Freq;
} MV_FREQ_MODE;

#define MV_SAR_FREQ_MODES {\
	{0,  "266",  "266", "133" },\
	{1,  "333",  "167", "167" },\
	{2,  "333",  "222", "167" },\
	{3,  "333",  "333", "167" },\
	{4,  "400",  "200", "200" },\
	{5,  "400",  "267", "200" },\
	{6,  "400",  "400", "200" },\
	{7,  "500",  "250", "250" },\
	{8,  "500",  "334", "250" },\
	{9,  "500",  "400", "250" },\
	{10,  "533", "267", "267" },\
	{11, "533",  "356", "267" },\
	{12, "533",  "533", "267" },\
	{13, "600",  "300", "300" },\
	{14, "600",  "400", "300" },\
	{15, "600",  "600", "300" },\
	{16, "666",  "333", "333" },\
	{17, "666",  "444", "333" },\
	{18, "666",  "666", "333" },\
	{19, "800",  "267", "400" },\
	{20, "800",  "400", "400" },\
	{21, "800",  "534", "400" },\
	{22, "900",  "300", "450" },\
	{23, "900",  "450", "450" },\
	{24, "900",  "600", "450" },\
	{25, "1000", "500", "500" },\
	{26, "1000", "667", "500" },\
	{27, "1000", "500", "333" },\
	{28, "400",  "400", "400" },\
	{29, "1100", "550", "550" }\
};

/* For backward compatability with Legacy mode */
#define mvBoardSwitchConnectedPortGet(port)	(-1)
#define mvBoardIsSwitchConnected(port)  	(mvBoardSwitchConnectedPortGet(port) != -1)
/*#define mvBoardLinkStatusIrqGet(port)		mvBoardSwitchIrqGet()*/

MV_VOID mvBoardEnvInit(MV_VOID);
MV_U16 mvBoardModelGet(MV_VOID);
MV_U16 mvBoardRevGet(MV_VOID);
MV_STATUS mvBoardNameGet(char *pNameBuff);
MV_BOARD_SPEC_INIT *mvBoardSpecInitGet(MV_VOID);
MV_BOOL mvBoardIsPortInSgmii(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInGmii(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortLb(MV_U32 ethPortNum);
MV_32 mvBoardPhyAddrGet(MV_U32 ethPortNum);
MV_32 mvBoardPhyLinkCryptPortAddrGet(MV_U32 ethPortNum);

MV_32 mvBoardQuadPhyAddr0Get(MV_U32 ethPortNum);
MV_STATUS mvBoardSarInfoGet(MV_SATR_TYPE_ID sarClass, MV_BOARD_SAR_INFO *sarInfo);
MV_STATUS mvBoardConfigTypeGet(MV_CONFIG_TYPE_ID configClass, MV_BOARD_CONFIG_TYPE_INFO *configInfo);
MV_U32 mvBoardTclkGet(MV_VOID);
MV_U32 mvBoardSysClkGet(MV_VOID);
MV_U32 mvBoardDebugLedNumGet(MV_U32 boardId);
MV_VOID mvBoardDebugLed(MV_U32 hexNum);
MV_32 mvBoarGpioPinNumGet(MV_BOARD_GPP_CLASS class, MV_U32 index);
MV_VOID mvBoardReset(MV_VOID);
MV_32 mvBoardResetGpioPinGet(MV_VOID);
MV_32 mvBoardSDIOGpioPinGet(MV_BOARD_GPP_CLASS type);
MV_32 mvBoardUSBVbusGpioPinGet(MV_32 devId);
MV_32 mvBoardUSBVbusEnGpioPinGet(MV_32 devId);
MV_BOOL mvBoardIsOurPciSlot(MV_U32 busNum, MV_U32 slotNum);
MV_U32 mvBoardGpioIntMaskGet(MV_U32 gppGrp);
MV_32 mvBoardMppGet(MV_U32 mppGroupNum);
MV_32 mvBoardMppTypeGet(MV_U32 mppGroupNum);
MV_VOID mvBoardMppTypeSet(MV_U32 mppGroupNum, MV_U32 groupType);
MV_U32 mvBoardGppConfigGet(void);
MV_32 mvBoardTdmSpiModeGet(MV_VOID);
MV_U8 mvBoardTdmDevicesCountGet(void);
MV_U8 mvBoardTdmSpiCsGet(MV_U8 devId);
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
MV_U32 mvBoardIdGet(MV_VOID);
MV_U32 mvBoardIdReadFromSatR(void);
MV_BOOL mvBoardIdSet(MV_U32 boardId);
MV_U32 mvBoardSledCpuNumGet(MV_VOID);
MV_VOID mvBoardConfigInit(MV_VOID);
void mvBoardConfigWrite(MV_VOID);
MV_ETH_COMPLEX_TOPOLOGY mvBoardMac0ConfigGet(MV_VOID);
MV_ETH_COMPLEX_TOPOLOGY mvBoardMac1ConfigGet(MV_VOID);
MV_ETH_COMPLEX_TOPOLOGY mvBoardLaneSGMIIGet(MV_VOID);
MV_BOARD_BOOT_SRC mvBoardBootDeviceGroupSet(MV_U32 sarBootDeviceValue);
MV_BOARD_BOOT_SRC mvBoardBootDeviceGet(MV_U32 sarBootDeviceValue);
MV_U32 mvBoardBootAttrGet(MV_U32 sarBootDeviceValue, MV_U8 attrNum);
MV_STATUS mvBoardFreqModeGet(MV_FREQ_MODE *freqMode);
MV_U8 mvBoardTwsiGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U8 devNum, MV_U8 regNum);
MV_STATUS mvBoardTwsiSet(MV_BOARD_TWSI_CLASS twsiClass, MV_U8 devNum, MV_U8 regNum, MV_U8 regVal);

MV_U8 mvBoardCpuFreqGet(MV_VOID);
MV_STATUS mvBoardCpuFreqSet(MV_U8 freqVal);

MV_U8 mvBoardCpuCoresNumGet(MV_VOID);

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
MV_BOOL mvBoardIsSerdesConfigurationEnabled(void);
MV_STATUS  mvBoardSerdesConfigurationEnableSet(MV_BOOL enableSerdesconfig);
MV_SERDES_CFG *mvBoardSerdesCfgGet(void);
MV_BOARD_PEX_INFO *mvBoardPexInfoGet(void);
MV_STATUS mvBoardConfIdSet(MV_U16 conf);
MV_U16 mvBoardPexModeGet(MV_VOID);
MV_STATUS mvBoardPexModeSet(MV_U16 conf);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __INCmvBoardEnvLibh */
