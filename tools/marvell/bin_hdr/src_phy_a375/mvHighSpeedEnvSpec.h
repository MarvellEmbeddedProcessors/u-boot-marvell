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

#define MAX_LANE_NUM      4
#define MAX_BOARD_CONFIG_OPTION 4
#define MV_IO_EXP_MAX_REGS    3

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

typedef struct _boardConfigTypesInfo {
  MV_CONFIG_TYPE_ID configId;
  MV_U32 mask;
  MV_U32 offset;
  MV_U32 expanderNum;
  MV_U32 regNum;
  MV_U32 isActiveForBoard[MV_MAX_BOARD_ID];
} MV_BOARD_CONFIG_TYPE_INFO;

/*typedef struct _boardIoExapnderTypesInfo {
  MV_IO_EXPANDER_TYPE_ID ioFieldid;
  MV_U32 offset;
  MV_U32 expanderNum;
  MV_U32 regNum;
} MV_BOARD_IO_EXPANDER_TYPE_INFO;*/
/*typedef enum _mvIoExpanderTypeID {
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
    MV_IO_EXPANDER_JUMPER1_EEPROM_ENABLED,
    MV_IO_EXPANDER_JUMPER2,
    MV_IO_EXPANDER_JUMPER3,
    MV_IO_EXPANDER_EXT_PHY_SMI_EN,
    MV_IO_EXPANDER_SPI1_CS_MSB0,
    MV_IO_EXPANDER_SPI1_CS_MSB1,
    MV_IO_EXPANDER_INTEG_PHY_PORTS_LED,
    MV_IO_EXPANDER_USB_SUPER_SPEED,
} MV_IO_EXPANDER_TYPE_ID;*/

typedef struct _boardSerdesChangeMphy {
  MV_BIN_SERDES_UNIT_INDX serdesType;
  MV_U32  serdesRegLowSpeed;
  MV_U32  serdesValueLowSpeed;
  MV_U32  serdesRegHiSpeed;
  MV_U32  serdesValueHiSpeed;
} MV_SERDES_CHANGE_M_PHY;
/* Configuration per SERDES line.
   Each nibble is MV_SERDES_LINE_TYPE */
typedef struct _boardSerdesConf {
  MV_PEX_TYPE pexType; /* MV_PEX_ROOT_COMPLEX MV_PEX_END_POINT */
  MV_U32  serdesLine0_3;  /* Lines 0 to 3*/
  MV_PEX_UNIT_CFG   pexMod[4];
  MV_U32  busSpeed; /* Bus speed - one bit per SERDES line:
  Low speed (0)   High speed (1)
  PEX 2.5 G (10 bit)    5 G (20 bit)
  SATA  1.5 G     3 G
  SGMII   1.25 Gbps   3.125 Gbps  */
  MV_SERDES_CHANGE_M_PHY * serdesMphyChange;
} MV_BIN_SERDES_CFG;


typedef struct _boardTwsiInfo {
  MV_U8 twsiDevAddr;
  MV_U8 twsiDevAddrType;
} MV_BOARD_BIN_TWSI_INFO;


#endif /* _MV_HIGHSPEED_ENV_SPEC_H */
