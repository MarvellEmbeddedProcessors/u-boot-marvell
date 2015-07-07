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


#ifndef _MV_HIGHSPEED_ENV_SPEC_H
#define _MV_HIGHSPEED_ENV_SPEC_H

#include "bootstrap_os.h"

/********************************* Definitions *************************/

#if defined(MV_MSYS_AC3)
#define MAX_SERDES_LANES		3
#define LAST_LANE_NUM			12
#else
#define MAX_SERDES_LANES		3
#define LAST_LANE_NUM			21
#endif

#define MV_SERDES_28NM_REV_1				0x0
#define MV_SERDES_28NM_REV_3				0x1
#define MV_SERDES_AVAGO_REV_0				0x2

#define CONF_ROOT_COMPLEX_OFFSET			1
#define PEX_PHY_INDIRECT_ACC_REG			0x41b00
#define PHY_ACCESS_MODE_OFFSET				31
#define PHY_ADDRESS_OFFSET				16
/* COMPHY_M_PIPE_R2P3_28HPM_REG_MIDAS_INTERNAL_PCIE001 registers -
  with indirect access via PEX_PHY_INDIRECT_ACC_REG */
#define G2_SETTING_0					0x00F
#define PHYTST_REG0					0x015
#define VTHIMPCAL_CTRL_REG				0x041
#define CAL_REG3					0x057
#define GLOB_CLK_CTRL					0x1C1

#define TX_AND_RX_IMPEDANCE_THRESHIOLD			0x11

#define IMPEDANCE_THRESHIOLD_OFFSET			8
#define RX_CALIBRATION_OFFSET				7
#define TX_CALIBRATION_OFFSET				14
#define TX_SLEW_CNTRL_ENABLE_OFFSET			15

#define  AVAGO_1_25G_BAUDRATE_DIVIDER_VALUE 8

/********************************* Enum ********************************/
typedef enum {
	PEX0,
	SGMII0,
	SGMII1,
	DEFAULT_SERDES,
	LAST_SERDES_TYPE
} SERDES_TYPE;

/* Serdes baud rates */
typedef enum {
	__1_25Gbps,
	__5Gbps,
	LAST_SERDES_SPEED
} SERDES_SPEED;

/* Serdes modes */
typedef enum {
	PEX_ROOT_COMPLEX_x1,
	PEX_END_POINT_x1,

	SERDES_DEFAULT_MODE, /* not pex */

	SERDES_LAST_MODE
} SERDES_MODE;

/* Serdes sequences */
typedef enum {
	REF_CLOCK__25MHz,
	REF_CLOCK__100MHz,
	REF_CLOCK_UNSUPPORTED
} REF_CLOCK;

/* Serdes sequences */
typedef enum {
	SGMII_EXT_SPEED_CONFIG_SEQ,
	SGMII_INT_SPEED_CONFIG_SEQ,
	SGMII_SD_RESET_SEQ,
	SGMII_SD_UNRESET_SEQ,
	SGMII_RF_RESET_SEQ,
	SGMII_RF_UNRESET_SEQ,
	SGMII_CORE_RESET_SEQ,
	SGMII_CORE_UNRESET_SEQ,
	SGMII_SYNCE_RESET_SEQ,
	SGMII_SYNCE_UNRESET_SEQ,
	SGMII_POWER_UP_SEQ,
	SGMII_POWER_DOWN_SEQ,

	USB2_POWER_UP_SEQ,
	SERDES_LAST_SEQ
} SERDES_SEQ;

typedef enum
{
    PRIMARY,
    SECONDARY
}MV_HWS_REF_CLOCK_SRC;
typedef enum
{
    _10BIT_ON,
    _10BIT_OFF,
    _10BIT_NA,
    _20BIT_ON,
    _40BIT_ON

}MV_HWS_SERDES_10B_MODE;

typedef enum
{
    XAUI_MEDIA,
    RXAUI_MEDIA

}MV_HWS_SERDES_MEDIA;
enum{ EXTERNAL_REG, INTERNAL_REG};

/********* Addresses for TABLE SD_Regs.cfg ********/
enum
{
    SERDES_EXTERNAL_CONFIGURATION_0 = 0x0,
    SERDES_EXTERNAL_CONFIGURATION_1 = 0x4,
    SERDES_EXTERNAL_CONFIGURATION_2 = 0x8,
    SERDES_EXTERNAL_CONFIGURATION_3 = 0xc

}SERDES_EXTERNAL_CONFIG;


/************************************ structures ******************************/
typedef struct {
	SERDES_TYPE		serdesType;
	MV_U32			serdesNum;
	SERDES_SPEED	serdesSpeed;
	SERDES_MODE		serdesMode;
	MV_BOOL			swapRx;
	MV_BOOL			swapTx;
} SERDES_MAP;


/*********************************** Globals **********************************/
/* A generic function pointer for loading the board topology map */
typedef MV_STATUS (*loadTopologyFuncPtr)(SERDES_MAP  *serdesMapArray);

/*************************** Functions declarations ***************************/

/*******************************************************************************
* mvHwsSerdesRevGet
*
* DESCRIPTION: Get the Serdes revision number
*
* INPUT: configField - Field description enum
*
* OUTPUT: None
*
* RETURN:
* 		8 bit Serdes revision number
*
*******************************************************************************/
MV_U8 mvHwsSerdesRevGet(MV_VOID);

/**************************************************************************
 * mvHwsSerdesLastLaneGet -
 *
 * DESCRIPTION:          return last lane number.
 * INPUT:   			 NONE.
 * OUTPUT:               None.
 * RETURNS:              number of lanes
 ***************************************************************************/
MV_U32 mvHwsSerdesLastLaneGet(MV_VOID);

#endif /* _MV_HIGHSPEED_ENV_SPEC_H */

