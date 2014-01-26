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
#include "mvBHboardEnvSpec.h"

/***************************   defined ******************************/

#define MV_IO_EXP_MAX_REGS    3

#define BOARD_DEV_TWSI_EEPROM               0x55
#define BOARD_DEV_TWSI_IO_EXPANDER          0x21
#define BOARD_DEV_TWSI_IO_EXPANDER_SW7		0x22
#define BOARD_DEV_TWSI_IO_EXPANDER_JUMPER1  0x24
#define DEV_NUM0							0x0
#define DEV_NUM1							0x1
#define REG_NUM0							0x0
#define REG_NUM1							0x1
#define REG_NUM2							0x2


#define MAX_LANE_NUM      		4
#define MAX_BOARD_CONFIG_OPTION 4
#define MV_IO_EXP_MAX_REGS    	3

#define FREQ_MODES_NUM_6610		0
#define FREQ_MODES_NUM_6650		4
#define FREQ_MODES_NUM_6660		5


#define MV_USER_SAR_FREQ_MODES { \
		{ 6,  400,  400, 200 }, \
		{ 14, 600,  400, 300 }, \
		{ 20, 800,  400, 400 }, \
		{ 21, 800,  534, 400 }, \
		{ 25, 1000, 500, 500 }, \
};

/**************************** enums ********************************/

/*typedef enum _mvConfigTypeID {
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
} MV_CONFIG_TYPE_ID;*/


typedef enum {
  SERDES_UNIT_UNCONNECTED = 0x0,
  SERDES_UNIT_PEX   = 0x1,
  SERDES_UNIT_SATA  = 0x2,
  SERDES_UNIT_SGMII = 0x3,
  SERDES_UNIT_USB3  = 0x4,
  SERDES_LAST_UNIT

} MV_BIN_SERDES_UNIT_INDX;

typedef enum {
  PEX_BUS_DISABLED  = 0,
  PEX_BUS_MODE_X1   = 1,
} MV_PEX_UNIT_CFG;

typedef enum _mvPexType {
          MV_PEX_ROOT_COMPLEX,    /* root complex device */
          MV_PEX_END_POINT        /* end point device */
} MV_PEX_TYPE;

typedef enum mvSgmiiSpeed {
	MV_SGMII_GEN1,	/* GEN1 - 1.25G*/
	MV_SGMII_GEN2,	/* GEN2 - 3.125G*/
	MV_SGMII_NA
} MV_SGMII_SPEED;

/**************************** structures ***************************/

typedef struct mvBoardSerdesTopology {
	MV_U8 lane1;
	MV_U8 lane2;
	MV_U8 lane3;

} MV_BOARD_SERDES_TOPOLOGY;

typedef struct mvBoardTopologyConfig {
	char *boardName;
	MV_BOARD_SERDES_TOPOLOGY serdesTopology;
	MV_SGMII_SPEED sgmiiSpeed;

} MV_BOARD_TOPOLOGY_CONFIG;

typedef struct {
	MV_U8 id;
	MV_U32 cpuFreq;
	MV_U32 ddrFreq;
	MV_U32 l2Freq;
} MV_FREQ_MODE;

/**************************** functions ***************************/

MV_STATUS mvBoardTwsiGet(MV_U32 address, MV_U8 devNum, MV_U8 regNum, MV_BOOL isMoreThen256, MV_U8 *pData);
MV_STATUS mvBoardUpdateBoardTopologyConfig(MV_U32  boardId);
MV_STATUS mvBoardDb6660LaneConfigGet(MV_U8 *tempVal);

#if 0
typedef struct _boardTwsiInfo {
  MV_U8 twsiDevAddr;
  MV_U8 twsiDevAddrType;
} MV_BOARD_BIN_TWSI_INFO;
#endif

#endif /* _MV_HIGHSPEED_ENV_SPEC_H */
