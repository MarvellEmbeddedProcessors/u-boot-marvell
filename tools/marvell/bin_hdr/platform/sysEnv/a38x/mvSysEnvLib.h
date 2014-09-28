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

/********************************* Definitions ********************************/

#define INTER_REGS_BASE                                     0xD0000000

/* Serdes definitions */
#define	COMMON_PHY_BASE_ADDR					0x18300

#define COMMON_PHY_CONFIGURATION1_REG           0x18300
#define COMMON_PHY_CONFIGURATION2_REG           0x18304
#define COMMON_PHY_CONFIGURATION4_REG           0x1830C
#define COMMON_PHY_STATUS1_REG                  0x18318
#define COMMON_PHYS_SELECTORS_REG               0x183FC
#define SOC_CONTROL_REG1                        0x18204
#define GENERAL_PURPOSE_RESERVED0_REG			0x182E0
#define GBE_CONFIGURATION_REG                   0x18460
#define DEVICE_SAMPLE_AT_RESET1_REG             0x18600
#define DEVICE_SAMPLE_AT_RESET2_REG				0x18604

#define POWER_AND_PLL_CTRL_REG                  0xa0004
#define CALIBRATION_CTRL_REG                    0xa0008
#define G1_SETTINGS_0_REG						0xa0034
#define LOOPBACK_REG                            0xa008C
#define INTERFACE_REG                           0xa0094
#define ISOLATE_REG                             0xa0098
#define MISC_REG                                0xa013C
#define GLUE_REG                                0xa0140
#define GENERATION_DIVIDER_FORCE_REG            0xa0144
#define RESET_DFE_REG                           0xa0148
#define LANE_ALIGN_REG0							0xa0124
#define SQUELCH_FFE_SETTING_REG					0xa0018
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

#define LANE_CFG4_REG							0xa0620

#define GLOBAL_CLK_CTRL                         0xa0704
#define GLOBAL_MISC_CTRL                        0xa0718
#define GLOBAL_PM_CTRL							0xa0740

#define SATA_CTRL_REG_INDIRECT                  0xa80a0
#define SATA_CTRL_REG                           0xa80a4

#define MAX_SELECTOR_VAL						0x9

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

/* TWSI addresses */
#define BOARD_ID_GET_ADDR                       0x50
#define RD_GET_MODE_ADDR                        0x4C
#define DB_GET_MODE_SLM1363_ADDR                0x25
#define DB_GET_MODE_SLM1364_ADDR                0x24

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
/* Customer boards */
#define CUTOMER_BOARD_ID_BASE		0x0
#define ARMADA_38x_CUSTOMER_BOARD_ID0	(CUTOMER_BOARD_ID_BASE + 0)
#define ARMADA_38x_CUSTOMER_BOARD_ID1	(CUTOMER_BOARD_ID_BASE + 1)
#define MV_MAX_CUSTOMER_BOARD_ID	(CUTOMER_BOARD_ID_BASE + 2)
#define MV_CUSTOMER_BOARD_NUM		(MV_MAX_CUSTOMER_BOARD_ID - CUTOMER_BOARD_ID_BASE)

/* Marvell boards */
#define MARVELL_BOARD_ID_BASE		0x10
#define RD_NAS_68XX_ID			(MARVELL_BOARD_ID_BASE + 0)
#define DB_68XX_ID			    (MARVELL_BOARD_ID_BASE + 1)
#define RD_AP_68XX_ID			(MARVELL_BOARD_ID_BASE + 2)
#define DB_AP_68XX_ID			(MARVELL_BOARD_ID_BASE + 3)
#define MV_MAX_MARVELL_BOARD_ID		(MARVELL_BOARD_ID_BASE + 4)
#define MV_MARVELL_BOARD_NUM		(MV_MAX_MARVELL_BOARD_ID - MARVELL_BOARD_ID_BASE)

#define DB_69XX_ID			    (MARVELL_BOARD_ID_BASE + 0)

#define MV_INVALID_BOARD_ID		0xFFFFFFFF

/* device ID */
#define ARMADA_380_DEVIDE_ID                    0x6810
#define ARMADA_385_DEVIDE_ID                    0x6820
#define ARMADA_388_DEVIDE_ID                    0x6828
#define ARMADA_390_DEVIDE_ID                    0x6920
#define ARMADA_398_DEVIDE_ID                    0x6928

#define MPP_CONTROL_REG(id)                     (0x18000 + (id * 4))
#define GPP_DATA_OUT_REG(grp)			(MV_GPP_REGS_BASE(grp) + 0x00)
#define GPP_DATA_OUT_EN_REG(grp)		(MV_GPP_REGS_BASE(grp) + 0x04)
#define GPP_DATA_IN_REG(grp)			(MV_GPP_REGS_BASE(grp) + 0x10)
#define MV_GPP_REGS_BASE(unit) 			(0x18100 + ((unit) * 0x40))

#define MPP_REG_NUM(GPIO_NUM)		(GPIO_NUM / 8)
#define MPP_MASK(GPIO_NUM)		(0xf << 4 * (GPIO_NUM - (MPP_REG_NUM(GPIO_NUM) * 8)));
#define GPP_REG_NUM(GPIO_NUM)		(GPIO_NUM / 32)
#define GPP_MASK(GPIO_NUM)		(1 << GPIO_NUM % 32)

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
#define MV_BOARD_WAKEUP_GPIO_INFO {\
{ARMADA_38x_CUSTOMER_BOARD_ID0,	-1 },\
{ARMADA_38x_CUSTOMER_BOARD_ID1,	-1 },\
};

#else

#ifdef MV88F68XX
#define MV_BOARD_WAKEUP_GPIO_INFO {\
{RD_NAS_68XX_ID, -2 },\
{DB_68XX_ID,	 -1 },\
{RD_AP_68XX_ID,	 -2 },\
{DB_AP_68XX_ID,	 21 },\
};
#else
#define MV_BOARD_WAKEUP_GPIO_INFO {\
{DB_69XX_ID, -1 },\
};
#endif
#endif
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

#ifdef CONFIG_CMD_BOARDCFG
#define MV_INFO_TWSI_EEPROM_DEV     0x50
#define EEPROM_VERIFICATION_PATTERN     0xfadecafe
#define MV_BOARD_CONFIG_MAX_BYTE_COUNT  8
#define MV_BOARD_CONFIG_DEFAULT_VALUE		{0x1921d0a1, 0x4 }

#ifdef CONFIG_ARMADA_39X /* tables below are relevant for A390 SoC */
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
	MV_CONFIG_TYPE_MAX_OPTION,
	MV_CONFIG_TYPE_CMD_DUMP_ALL,
	MV_CONFIG_TYPE_CMD_SET_DEFAULT
} MV_CONFIG_TYPE_ID;

#define MV_EEPROM_CONFIG_INFO { \
{ MV_CONFIG_BOARDID,		0xFF000000,	24,	0,	{1} }, \
{ MV_CONFIG_LANE0,		0x7,		0,	0,	{1} }, \
{ MV_CONFIG_LANE1,		0x78,		3,	0,	{1} }, \
{ MV_CONFIG_LANE2,		0x380,		7,	0,	{1} }, \
{ MV_CONFIG_LANE3,		0x3C00,		10,	0,	{1} }, \
{ MV_CONFIG_LANE4,		0x3C000,	14,	0,	{1} }, \
{ MV_CONFIG_LANE5,		0x3C0000,	18,	0,	{1} }, \
{ MV_CONFIG_LANE6,		0x7,		0,	4,	{1} }, \
{ MV_CONFIG_NSS_EN,		0x8,		3,	4,	{1} }, \
{ MV_CONFIG_DDR_BUSWIDTH,	0x10,		4,	4,	{1} }, \
{ MV_CONFIG_DDR_ECC_EN,		0x20,		5,	4,	{1} }, \
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

#endif /* CONFIG_ARMADA_39X */
#endif /* CONFIG_CMD_BOARDCFG */

#endif /* __INCmvSysEnvLibh */
