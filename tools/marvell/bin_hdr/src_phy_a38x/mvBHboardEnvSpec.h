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

#include "mvHighSpeedEnvSpec.h"

/********************************* Definitions ********************************/

#define INTER_REGS_BASE                                     0xD0000000

#define COMMON_PHY_CONFIGURATION1_REG           0x18300
#define COMMON_PHY_CONFIGURATION2_REG           0x18304
#define COMMON_PHY_CONFIGURATION4_REG           0x1830C
#define COMMON_PHY_STATUS1_REG                  0x18318
#define COMMON_PHYS_SELECTORS_REG               0x183FC
#define SOC_CONTROL_REG1                        0x18204
#define GENERAL_PURPOSE_RESERVED0_REG		0x182E0
#define GBE_CONFIGURATION_REG                   0x18460
#define DEVICE_SAMPLE_AT_RESET1_REG             0xE4204

#define POWER_AND_PLL_CTRL_REG                  0xa0004
#define CALIBRATION_CTRL_REG                    0xa0008
#define LOOPBACK_REG                            0xa008C
#define GLOBAL_CLK_CTRL                         0xa0704
#define INTERFACE_REG                           0xa0094
#define ISOLATE_REG                             0xa0098
#define MISC_REG                                0xa013C
#define GLUE_REG                                0xa0140
#define GENERATION_DIVIDER_FORCE_REG            0xa0144
#define RESET_DFE_REG                           0xa0148

#define SATA_CTRL_REG_INDIRECT                  0xa80a0
#define SATA_CTRL_REG                           0xa80a4

/* TWSI addresses */
#define BOARD_ID_GET_ADDR                       0x50
#define RD_GET_MODE_ADDR                        0x4C
#define DB_GET_MODE_SLM1363_ADDR                0x25
#define DB_GET_MODE_SLM1364_ADDR                0x24

/* For setting MPP2 and MPP3 to be TWSI mode and MPP 0,1 to UART mode */
#define MPP_CTRL_REG                            0x18000
#define MPP_SET_MASK                            (~(0xFFFF))
#define MPP_SET_DATA                            (0x1111)

#define AVS_DEBUG_CNTR_REG              0xE4124
#define AVS_DEBUG_CNTR_DEFAULT_VALUE    0x08008073

#define AVS_ENABLED_CONTROL             0xE4130
#define AVS_LOW_VDD_LIMIT_OFFS          4
#define AVS_LOW_VDD_LIMIT_MASK          (0xff << AVS_LOW_VDD_LIMIT_OFFS)
#define AVS_LOW_VDD_LIMIT_VAL           (0x27 << AVS_LOW_VDD_LIMIT_OFFS)

#define AVS_HIGH_VDD_LIMIT_OFFS         12
#define AVS_HIGH_VDD_LIMIT_MASK         (0xff << AVS_HIGH_VDD_LIMIT_OFFS)
#define AVS_HIGH_VDD_LIMIT_VAL          (0x27 << AVS_HIGH_VDD_LIMIT_OFFS)

/* boards ID numbers */
#define BOARD_ID_BASE                           0x0

#define RD_A38X_DDR3_ID                         (BOARD_ID_BASE)
#define DB_A38X_BP_ID                           (RD_A38X_DDR3_ID + 1)
#define RD_AP_A38X_DDR3_ID                      (DB_A38X_BP_ID + 1)
#define CUSTOMER_BOARD_ID                       (RD_AP_A38X_DDR3_ID + 1)

#define MV_MAX_BOARD_ID                         (CUSTOMER_BOARD_ID + 1) /* dummy */

/* device ID */
#define ARMADA_380_DEVIDE_ID                    0x6810
#define ARMADA_385_DEVIDE_ID                    0x6820
#define ARMADA_388_DEVIDE_ID                    0x6828
#define ARMADA_390_DEVIDE_ID                    0x6920
#define ARMADA_398_DEVIDE_ID                    0x6928

/* A generic function pointer for loading the board topology map */
typedef MV_STATUS (*loadTopologyFuncPtr)(SERDES_MAP  *serdesMapArray);

/*********************************** Structs **********************************/

/*********************************** Globals **********************************/

/*************************** Functions declarations ***************************/

MV_U8 mvHwsBoardIdGet(MV_VOID);

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
 * mvHwsBoardTopologyLoad -
 *
 * DESCRIPTION:          Loads the board topology
 * INPUT:                serdesMapArray  -   The struct that will contain
 *                                           the board topology map
 * OUTPUT:               The board topology.
 * RETURNS:              MV_OK           -   for success
 ***************************************************************************/
MV_STATUS mvHwsBoardTopologyLoad(SERDES_MAP  *serdesMapArray);

/**************************************************************************
 * mvHwsBoardTclkGet -
 *
 * DESCRIPTION:          Returns the board Tclk
 * INPUT:                None.
 * OUTPUT:               None.
 * RETURNS:              Tclk
 ***************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID);

/**************************************************************************
 * mvHwsRefClockSet -
 *
 * DESCRIPTION:          Sets the refClock according to the serdes type
 *                       and ref clock given as input.
 * INPUT:                serdesNum       - serdes lane number.
 *                                         needed for calculating the
 *                                         address where the ref clock will
 *                                         be set.
 *                       serdesType      - PEX, USB3, SATA or SGMII.
 *                       refClock        - 25MHz or 100MHz
 * OUTPUT:               Sets the refclock in the relevant address.
 * RETURNS:              MV_OK           - for success
 *                       MV_BAD_PARAMS   - if one or more of the input
 *                                         parameters were bad
 ***************************************************************************/
MV_STATUS mvHwsRefClockSet
(
	MV_U32 serdesNum,
	SERDES_TYPE serdesType,
	REF_CLOCK refClock
);

/**************************************************************************
 * mvUpdateSerdesPhySelectors -
 *
 * DESCRIPTION:          Sets the bits 0-18 in the COMMON PHYS SELECTORS
 *                       register according to the topology map loaded from
 *                       the board, using commonPhysSelectorsMap
 *
 * INPUT:                serdesConfigMap -   the serdes configuration map
 *                                           loaded from the board
 * OUTPUT:               Sets the bits 0-18 in the COMMON PHYS SELECTORS
 *                       register
 * RETURNS:              MV_OK           - for success
 *                       MV_BAD_PARAM    - for a bad combination of serdes num
 *                                         and serdes type
 ***************************************************************************/
MV_STATUS mvHwsUpdateSerdesPhySelectors(SERDES_MAP  *serdesConfigMap);

#endif
