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
#ifndef __INCmvSysEnvLibh
#define __INCmvSysEnvLibh
#include "ddr3_hws_hw_training.h"

/********************************* Definitions ********************************/

#define INTER_REGS_BASE                                     0xD0000000

/* Serdes definitions */
#define	COMMON_PHY_BASE_ADDR					0x18300

#define DEVICE_CONFIGURATION_REG0		0x18284
#define DEVICE_CONFIGURATION_REG1		0x18288
#define COMMON_PHY_CONFIGURATION1_REG           0x18300
#define COMMON_PHY_CONFIGURATION2_REG           0x18304
#define COMMON_PHY_CONFIGURATION4_REG           0x1830C
#define COMMON_PHY_STATUS1_REG                  0x18318
#define COMMON_PHYS_SELECTORS_REG               0x183FC
#define SOC_CONTROL_REG1                        0x18204
#define GENERAL_PURPOSE_RESERVED0_REG			0x182E0
#define GBE_CONFIGURATION_REG                   0x18460
#define DEVICE_SAMPLE_AT_RESET1_REG             0x18600
#define DEVICE_SAMPLE_AT_RESET2_REG		0x18604
#define DEV_ID_REG				0x18238

#define QSGMII_CONTROL_REG1						0x18494

#define DEV_ID_REG_DEVICE_ID_OFFS		16
#define DEV_ID_REG_DEVICE_ID_MASK		0xFFFF0000


#define SAR_DEV_ID_OFFS				27
#define SAR_DEV_ID_MASK          		0x7

#define POWER_AND_PLL_CTRL_REG                  0xa0004
#define CALIBRATION_CTRL_REG                    0xa0008
#define LOOPBACK_REG                            0xa008C
#define INTERFACE_REG                           0xa0094
#define ISOLATE_REG                             0xa0098
#define MISC_REG                                0xa013C
#define GLUE_REG                                0xa0140
#define GENERATION_DIVIDER_FORCE_REG            0xa0144
#define RESET_DFE_REG                           0xa0148
#define LANE_ALIGN_REG0							0xa0124
#define SQUELCH_FFE_SETTING_REG					0xa0018
#define G1_SETTINGS_0_REG						0xa0034
#define G1_SETTINGS_1_REG						0xa0038
#define G2_SETTINGS_0_REG						0xa003C
#define G2_SETTINGS_1_REG						0xa0040
#define G2_SETTINGS_2_REG						0xa00F8
#define G3_SETTINGS_0_REG						0xa0044
#define G3_SETTINGS_1_REG						0xa0048
#define VTHIMPCAL_CTRL_REG						0xa0104
#define REF_REG0                                0xa0134
#define CAL_REG6								0xa0168
#define RX_REG2									0xa0184
#define LANE_CFG1_REG							0xa0604
#define LANE_CFG4_REG							0xa0620
#define GLOBAL_CLK_CTRL                         0xa0704
#define GLOBAL_MISC_CTRL                        0xa0718
#define	GLOBAL_CLK_SRC_HI						0xa0710

#define GLOBAL_CLK_CTRL                         0xa0704
#define GLOBAL_MISC_CTRL                        0xa0718
#define GLOBAL_PM_CTRL							0xa0740

#define SATA_CTRL_REG_INDIRECT                  0xa80a0
#define SATA_CTRL_REG                           0xa80a4

/* Reference clock values and mask */
#define POWER_AND_PLL_CTRL_REG_100MHZ_VAL		0x0
#define POWER_AND_PLL_CTRL_REG_25MHZ_VAL_1		0x1
#define POWER_AND_PLL_CTRL_REG_25MHZ_VAL_2		0x2
#define POWER_AND_PLL_CTRL_REG_40MHZ_VAL		0x3
#define GLOBAL_PM_CTRL_REG_25MHZ_VAL			0x7
#define GLOBAL_PM_CTRL_REG_40MHZ_VAL			0xC
#define LANE_CFG4_REG_25MHZ_VAL					0x200
#define LANE_CFG4_REG_40MHZ_VAL					0x300

#define POWER_AND_PLL_CTRL_REG_MASK				(~(0x1F))
#define GLOBAL_PM_CTRL_REG_MASK					(~(0xFF))
#define LANE_CFG4_REG_MASK						(~(0x1F00))

#define	REF_CLK_SELECTOR_VAL(regVal)			(regVal & 0x1)

#define MAX_SELECTOR_VAL			10

/* TWSI addresses */
/* starting from A38x A0, i2c address of EEPROM is 0x57 */
#ifdef MV88F69XX
#define EEPROM_I2C_ADDR				0x50
#else
#define EEPROM_I2C_ADDR				(mvSysEnvDeviceRevGet() == MV_88F68XX_Z1_ID ? 0x50 : 0x57)
#endif
#define RD_GET_MODE_ADDR                        0x4C
#define DB_GET_MODE_SLM1363_ADDR                0x25
#define DB_GET_MODE_SLM1364_ADDR                0x24

#define SATR_DB_LANE1_MAX_OPTIONS		7
#define SATR_DB_LANE1_CFG_MASK			0x7
#define SATR_DB_LANE1_CFG_OFFSET		0
#define SATR_DB_LANE2_MAX_OPTIONS		4
#define SATR_DB_LANE2_CFG_MASK			0x38
#define SATR_DB_LANE2_CFG_OFFSET		3

/* For setting MPP2 and MPP3 to be TWSI mode and MPP 0,1 to UART mode */
#define MPP_CTRL_REG                            0x18000
#define MPP_SET_MASK                            (~(0xFFFF))
#define MPP_SET_DATA                            (0x1111)
#define MPP_UART1_SET_MASK                      (~(0xFF000))
#define MPP_UART1_SET_DATA                      (0x66000)

#define AVS_DEBUG_CNTR_REG              0xE4124
#define AVS_DEBUG_CNTR_DEFAULT_VALUE    0x08008073

#define AVS_ENABLED_CONTROL             0xE4130
#define AVS_LOW_VDD_LIMIT_OFFS          4
#define AVS_LOW_VDD_LIMIT_MASK          (0xff << AVS_LOW_VDD_LIMIT_OFFS)
#define AVS_LOW_VDD_LIMIT_VAL           (0x27 << AVS_LOW_VDD_LIMIT_OFFS)

#define AVS_HIGH_VDD_LIMIT_OFFS         12
#define AVS_HIGH_VDD_LIMIT_MASK         (0xff << AVS_HIGH_VDD_LIMIT_OFFS)
#define AVS_HIGH_VDD_LIMIT_VAL          (0x27 << AVS_HIGH_VDD_LIMIT_OFFS)

/* Board ID numbers */
#define MARVELL_BOARD_ID_MASK			0x10
/* Customer boards for A38x*/
#define A38X_CUSTOMER_BOARD_ID_BASE		0x0
#define A38X_CUSTOMER_BOARD_ID0			(A38X_CUSTOMER_BOARD_ID_BASE + 0)
#define A38X_CUSTOMER_BOARD_ID1			(A38X_CUSTOMER_BOARD_ID_BASE + 1)
#define A38X_MV_MAX_CUSTOMER_BOARD_ID		(A38X_CUSTOMER_BOARD_ID_BASE + 2)
#define A38X_MV_CUSTOMER_BOARD_NUM		(A38X_MV_MAX_CUSTOMER_BOARD_ID - A38X_CUSTOMER_BOARD_ID_BASE)

/* Marvell boards for A38x*/
#define A38X_MARVELL_BOARD_ID_BASE		0x10
#define RD_NAS_68XX_ID				(A38X_MARVELL_BOARD_ID_BASE + 0)
#define DB_68XX_ID				(A38X_MARVELL_BOARD_ID_BASE + 1)
#define RD_AP_68XX_ID				(A38X_MARVELL_BOARD_ID_BASE + 2)
#define DB_AP_68XX_ID				(A38X_MARVELL_BOARD_ID_BASE + 3)
#define DB_GP_68XX_ID				(A38X_MARVELL_BOARD_ID_BASE + 4)
#define DB_BP_6821_ID				(A38X_MARVELL_BOARD_ID_BASE + 5)
#define A38X_MV_MAX_MARVELL_BOARD_ID		(A38X_MARVELL_BOARD_ID_BASE + 6)
#define A38X_MV_MARVELL_BOARD_NUM		(A38X_MV_MAX_MARVELL_BOARD_ID - A38X_MARVELL_BOARD_ID_BASE)

/* Customer boards for A39x*/
#define A39X_CUSTOMER_BOARD_ID_BASE		0x20
#define A39X_CUSTOMER_BOARD_ID0			(A39X_CUSTOMER_BOARD_ID_BASE + 0)
#define A39X_CUSTOMER_BOARD_ID1			(A39X_CUSTOMER_BOARD_ID_BASE + 1)
#define A39X_MV_MAX_CUSTOMER_BOARD_ID		(A39X_CUSTOMER_BOARD_ID_BASE + 2)
#define A39X_MV_CUSTOMER_BOARD_NUM		(A39X_MV_MAX_CUSTOMER_BOARD_ID - A39X_CUSTOMER_BOARD_ID_BASE)

/* Marvell boards for A39x*/
#define A39X_MARVELL_BOARD_ID_BASE		0x30
#define A39X_DB_69XX_ID				(A39X_MARVELL_BOARD_ID_BASE + 0)
#define A39X_RD_69XX_ID				(A39X_MARVELL_BOARD_ID_BASE + 1)
#define A39X_MV_MAX_MARVELL_BOARD_ID		(A39X_MARVELL_BOARD_ID_BASE + 2)
#define A39X_MV_MARVELL_BOARD_NUM		(A39X_MV_MAX_MARVELL_BOARD_ID - A39X_MARVELL_BOARD_ID_BASE)

#ifdef MV88F68XX
#define CUTOMER_BOARD_ID_BASE			A38X_CUSTOMER_BOARD_ID_BASE
#define CUSTOMER_BOARD_ID0			A38X_CUSTOMER_BOARD_ID0
#define CUSTOMER_BOARD_ID1			A38X_CUSTOMER_BOARD_ID1
#define MV_MAX_CUSTOMER_BOARD_ID		A38X_MV_MAX_CUSTOMER_BOARD_ID
#define MV_CUSTOMER_BOARD_NUM			A38X_MV_CUSTOMER_BOARD_NUM
#define MARVELL_BOARD_ID_BASE			A38X_MARVELL_BOARD_ID_BASE
#define MV_MAX_MARVELL_BOARD_ID			A38X_MV_MAX_MARVELL_BOARD_ID
#define MV_MARVELL_BOARD_NUM			A38X_MV_MARVELL_BOARD_NUM
#define MV_DEFAULT_BOARD_ID			DB_68XX_ID
#define MV_DEFAULT_DEVICE_ID			MV_6811
#elif defined(MV88F69XX)
#define CUTOMER_BOARD_ID_BASE			A39X_CUSTOMER_BOARD_ID_BASE
#define CUSTOMER_BOARD_ID0			A39X_CUSTOMER_BOARD_ID0
#define CUSTOMER_BOARD_ID1			A39X_CUSTOMER_BOARD_ID1
#define MV_MAX_CUSTOMER_BOARD_ID		A39X_MV_MAX_CUSTOMER_BOARD_ID
#define MV_CUSTOMER_BOARD_NUM			A39X_MV_CUSTOMER_BOARD_NUM
#define MARVELL_BOARD_ID_BASE			A39X_MARVELL_BOARD_ID_BASE
#define MV_MAX_MARVELL_BOARD_ID			A39X_MV_MAX_MARVELL_BOARD_ID
#define MV_MARVELL_BOARD_NUM			A39X_MV_MARVELL_BOARD_NUM
#define MV_DEFAULT_BOARD_ID			A39X_DB_69XX_ID
#define MV_DEFAULT_DEVICE_ID			MV_6920
#endif

#define MV_INVALID_BOARD_ID			0xFFFFFFFF

/* device revesion */
#define DEV_VERSION_ID_REG			0x1823C
#define REVISON_ID_OFFS				8
#define REVISON_ID_MASK				0xF00

/* A38x revisions */
#define MV_88F68XX_Z1_ID		0x0
#define MV_88F68XX_A0_ID		0x4
/* A39x revisions */
#define MV_88F69XX_Z1_ID		0x2

#define MPP_CONTROL_REG(id)                     (0x18000 + (id * 4))
#define GPP_DATA_OUT_REG(grp)			(MV_GPP_REGS_BASE(grp) + 0x00)
#define GPP_DATA_OUT_EN_REG(grp)		(MV_GPP_REGS_BASE(grp) + 0x04)
#define GPP_DATA_IN_REG(grp)			(MV_GPP_REGS_BASE(grp) + 0x10)
#define MV_GPP_REGS_BASE(unit) 			(0x18100 + ((unit) * 0x40))

#define MPP_REG_NUM(GPIO_NUM)		(GPIO_NUM / 8)
#define MPP_MASK(GPIO_NUM)		(0xf << 4 * (GPIO_NUM - (MPP_REG_NUM(GPIO_NUM) * 8)));
#define GPP_REG_NUM(GPIO_NUM)		(GPIO_NUM / 32)
#define GPP_MASK(GPIO_NUM)		(1 << GPIO_NUM % 32)

/* device ID */
/* Armada 38x Family */
#define MV_6810_DEV_ID		0x6810
#define MV_6811_DEV_ID		0x6811
#define MV_6820_DEV_ID		0x6820
#define MV_6828_DEV_ID		0x6828
/* Armada 39x Family */
#define MV_6920_DEV_ID		0x6920
#define MV_6928_DEV_ID		0x6928


typedef enum _mvDeviceId {
	MV_6810,
	MV_6820,
	MV_6811,
	MV_6828,
	MV_NONE,
	MV_6920,
	MV_6928,
	MV_MAX_DEV_ID,
} MV_DEVICE_ID;

typedef struct boardDeviceIdWoVal {
	MV_DEVICE_ID devId;
	MV_32 ctrlModel;
	MV_32 wo_reg_val0;
	MV_32 wo_reg_val1;
} MV_DEVICE_ID_VAL;

/* predefined values for FUNCTION_ENABLE_CONTROL per flavour */
#define MV_DEVICE_ID_VAL_INFO {\
/* 		dev,	ctrlModel,		WO_VAL0,	WO_VAL1 */\
/* A380 */ {MV_6810,	MV_6810_DEV_ID,		0x71fd7f3,	0x00 },\
/* A381 */ {MV_6820,	MV_6820_DEV_ID,		0x77fdfff,	0x00 },\
/* A385 */ {MV_6811,	MV_6811_DEV_ID,		0x77ffffff,	0x00 },\
/* A388 */ {MV_6828,	MV_6828_DEV_ID,		0x77fffff,	0x00 },\
/* NONE */ {MV_NONE,	0x0,			0x0,		0x0  },\
/* A395 */ {MV_6920,	MV_6920_DEV_ID,		0x77ffffff,	0x00 },\
/* A398 */ {MV_6928,	MV_6928_DEV_ID,		0x77ffffff,	0x00 },\
};

typedef struct boardWakeupGPIO {
	MV_U32 boardId;
	MV_32 gpioNum;
} MV_BOARD_WAKEUP_GPIO;

/* GPIO status indication for Suspend Wakeup:
 * if suspend to RAM is supported and GPIO inidcation is implemented, set the gpio number
 * if suspend to RAM is supported but GPIO indication is not implemented set '-2'
 * if suspend to RAM is not supported set '-1'
 */
#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
#ifdef MV88F68XX
#define MV_BOARD_WAKEUP_GPIO_INFO {\
{A38X_CUSTOMER_BOARD_ID0,	-1 },\
{A38X_CUSTOMER_BOARD_ID0,	-1 },\
};
#else
#define MV_BOARD_WAKEUP_GPIO_INFO {\
{A39X_CUSTOMER_BOARD_ID0,	-1 },\
{A39X_CUSTOMER_BOARD_ID0,	-1 },\
};
#endif /* MV88F68XX */

#else

#ifdef MV88F68XX
#define MV_BOARD_WAKEUP_GPIO_INFO {\
{RD_NAS_68XX_ID, -2 },\
{DB_68XX_ID,	 -1 },\
{RD_AP_68XX_ID,	 -2 },\
{DB_AP_68XX_ID,	 -2 },\
{DB_GP_68XX_ID,	 -2 },\
{DB_BP_6821_ID,	 -2 },\
};
#else
#define MV_BOARD_WAKEUP_GPIO_INFO {\
{A39X_RD_69XX_ID, -1 },\
{A39X_DB_69XX_ID, -1 },\
};
#endif /* MV88F68XX */
#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */
extern MV_DRAM_DLB_CONFIG ddr3DlbConfigTable[];
extern MV_DRAM_DLB_CONFIG ddr3DlbConfigTable_A0[];

/**************************************************************************
 * mvBoardTclkGet -
 *
 * DESCRIPTION:          Returns the board id
 * INPUT:                None.
 * OUTPUT:               None.
 * RETURNS:              board id.
 ***************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID);


/**************************************************************************
 * mvBoardIdGet -
 *
 * DESCRIPTION:          Returns the board ID
 * INPUT:                None.
 * OUTPUT:               None.
 * RETURNS:              Tclk
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
 * mvHwsTwsiInitWrapper -
 *
 * DESCRIPTION:          Wrapper for initializing the TWSI unit
 * INPUT:                None.
 * OUTPUT:               None.
 * RETURNS:              None.
 ***************************************************************************/
MV_STATUS mvHwsTwsiInitWrapper(MV_VOID);

/************************************************************************************
* mvSysEnvSuspendWakeupCheck
* DESCRIPTION:	 	Reads GPIO input for suspend-wakeup indication.
* INPUT:	 	None.
* OUTPUT:
* RRETURNS:	MV_U32 indicating suspend wakeup status:
* 		0 - normal initialization, otherwise - suspend wakeup.
 ***************************************************************************/
MV_U32 mvSysEnvSuspendWakeupCheck(MV_VOID);

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
*       8bit desscribing Marvell controller revision number
*
*******************************************************************************/
MV_U8 mvSysEnvDeviceRevGet(MV_VOID);

/************************************************************************************
* mvSysEnvDeviceIdGet
* DESCRIPTION:	 	Returns enum (0..7) index of the device model (ID)
* INPUT:	 	None.
* OUTPUT:
* RRETURNS:	MV_U32 with Device ID
 ***************************************************************************/
MV_U32 mvSysEnvDeviceIdGet(MV_VOID);
/************************************************************************************
* mvSysEnvDeviceIdGet
* DESCRIPTION:	 	returns 16bit describing the device model (id)
* INPUT:	 	None.
* OUTPUT:
* RRETURNS:	MV_U32 with Device ID
 ***************************************************************************/
MV_U16 mvSysEnvModelGet(MV_VOID);
/*******************************************************************************
* mvSysEnvDlbConfigPtrGet
* DESCRIPTION: defines pointer to to DLB COnfiguration table
* INPUT: none
* OUTPUT: 
* RETURN:
*       returns pointer to DLB COnfiguration table
*******************************************************************************/
MV_DRAM_DLB_CONFIG  *mvSysEnvDlbConfigPtrGet(MV_VOID);

#ifdef CONFIG_CMD_BOARDCFG

#define MV_BOARD_CONFIG_MAX_BYTE_COUNT		8
#define MV_BOARD_CONFIG_DEFAULT_VALUE		{0x0021D0A9, 0x4 }
#define MV_BOARD_CONFIG_PATTERN_OFFSET		0x14
#define EEPROM_VERIFICATION_PATTERN             0xFADECAFE

/* the following definition is used to calculate a byte offset in the
   EEPROM according to byte number and bit offset, as they are stored
   in MV_EEPROM_CONFIG_INFO */
#define CALC_BYTE_OFFSET(off, byte) ((4 - ((off / 8) + 1)) + byte)

#ifdef MV88F69XX /* tables below are relevant for A390 SoC */
typedef enum _mvConfigTypeID {
	MV_CONFIG_BOARDID,
	MV_CONFIG_LANE0,
	MV_CONFIG_LANE1,
	MV_CONFIG_LANE2,
	MV_CONFIG_LANE3,
	MV_CONFIG_LANE4,
	MV_CONFIG_LANE5,
	MV_CONFIG_LANE6,
	MV_CONFIG_NSS_EN,
	MV_CONFIG_DDR_BUSWIDTH,
	MV_CONFIG_DDR_ECC_EN,
	MV_CONFIG_BOARDCFG_EN,
	MV_CONFIG_BOARDCFG_VALID,
	MV_CONFIG_TYPE_MAX_OPTION,
	MV_CONFIG_TYPE_CMD_DUMP_ALL,
	MV_CONFIG_TYPE_CMD_SET_DEFAULT
} MV_CONFIG_TYPE_ID;

#define MV_EEPROM_CONFIG_INFO { \
{ MV_CONFIG_BOARDID,		0xFF000000,	24,	0,	{1, 1} }, \
{ MV_CONFIG_LANE0,		0x7,		0,	0,	{1, 1} }, \
{ MV_CONFIG_LANE1,		0x78,		3,	0,	{1, 1} }, \
{ MV_CONFIG_LANE2,		0x380,		7,	0,	{1, 1} }, \
{ MV_CONFIG_LANE3,		0x3C00,		10,	0,	{1, 1} }, \
{ MV_CONFIG_LANE4,		0x3C000,	14,	0,	{1, 1} }, \
{ MV_CONFIG_LANE5,		0x3C0000,	18,	0,	{1, 1} }, \
{ MV_CONFIG_LANE6,		0x7,		0,	4,	{1, 1} }, \
{ MV_CONFIG_NSS_EN,		0x8,		3,	4,	{1, 1} }, \
{ MV_CONFIG_DDR_BUSWIDTH,	0x10,		4,	4,	{1, 1} }, \
{ MV_CONFIG_DDR_ECC_EN,		0x20,		5,	4,	{1, 1} }, \
{ MV_CONFIG_BOARDCFG_EN,	0x40,		6,	4,	{1, 1} }, \
{ MV_CONFIG_BOARDCFG_VALID,	0x3,		0,	16,	{1, 1} }, \
};

typedef struct _boardConfigTypesInfo {
	MV_CONFIG_TYPE_ID configId;
	MV_U32 mask;
	MV_U32 offset;
	MV_U32 byteNum;
	MV_U32 isActiveForBoard[MV_MARVELL_BOARD_NUM];
} MV_BOARD_CONFIG_TYPE_INFO;

/*******************************************************************************
* mvSysEnvConfigInit
*
* DESCRIPTION: Initialize EEPROM configuration
*       1. initialize all board configuration fields
*       3. read relevant board configuration (using TWSI/EEPROM access)
*
* INPUT:  None
*
* OUTPUT: None
*
* RETURN:
*       MV_OK if initialize pass, MV_INIT_ERROR if read board topology fail,
*       MV_BAD_VALUE if mvSysUpdateLaneConfig function fail
*
*******************************************************************************/
MV_STATUS mvSysEnvConfigInit(void);

/*******************************************************************************
* mvSysEnvConfigGet
*
* DESCRIPTION: Read Board configuration Field
*
* INPUT: configField - Field description enum
*
* OUTPUT: None
*
* RETURN:
*       if field is valid - returns requested Board configuration field value
*
*******************************************************************************/
MV_U32 mvSysEnvConfigGet(MV_CONFIG_TYPE_ID configField);

#endif /* MV88F69XX */
#endif /* CONFIG_CMD_BOARDCFG */

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

#endif /* __INCmvSysEnvLibh */
