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

#include "mv_os.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"
#include "mv_seq_exec.h"

/********************************* Definitions ********************************/
#define MAX_SERDES_LANES        7 /* as in a39x*/

/* Serdes revision */
#define MV_SERDES_REV_1_2		0x0		/* SerDes revision 1.2 (for A38x-Z1) */
#define MV_SERDES_REV_2_1		0x1		/* SerDes revision 2.1 (for A39x-Z1, A38x-A0) */

#define MV_SERDES_REV_NA		0xFF

#define	SERDES_REGS_LANE_BASE_OFFSET(lane)	(0x800 * lane)

#define PEXx4_ENABLE_OFFS		(mvHwsCtrlSerdesRevGet() == MV_SERDES_REV_1_2 ? 18 : 31)

/* For setting or clearing a certain bit
   (bit is a number between 0 and 31) in the data*/
#define SET_BIT(data, bit)          (data | (0x1 << bit))
#define CLEAR_BIT(data, bit)        (data & (~(0x1 << bit)))

/* For checking function return values */
#define CHECK_STATUS(origFunc) \
	{ \
		MV_STATUS mvStatus; \
		mvStatus = origFunc; \
		if (MV_OK != mvStatus) \
		{ \
			return mvStatus; \
		} \
	}

/************************************ enums ***********************************/
/* Serdes lane types */
typedef enum {
	PEX0,
	PEX1,
	PEX2,
	PEX3,
	SATA0,
	SATA1,
	SATA2,
	SATA3,
	SGMII0,
	SGMII1,
	SGMII2,
	QSGMII,
	USB3_HOST0,
	USB3_HOST1,
	USB3_DEVICE,
	SGMII3,
	XAUI,
	RXAUI,
	DEFAULT_SERDES,
	LAST_SERDES_TYPE
} SERDES_TYPE;

/* Serdes baud rates */
typedef enum {
	__1_25Gbps,
	__1_5Gbps,
	__2_5Gbps,
	__3Gbps,
	__3_125Gbps,
	__5Gbps,
	__6Gbps,
	__6_25Gbps,
	LAST_SERDES_SPEED
} SERDES_SPEED;

/* Serdes modes */
typedef enum {
	PEX_ROOT_COMPLEX_x1,
	PEX_ROOT_COMPLEX_x4,
	PEX_END_POINT_x1,
	PEX_END_POINT_x4,

	SERDES_DEFAULT_MODE, /* not pex */

	SERDES_LAST_MODE
} SERDES_MODE;

typedef struct {
	SERDES_TYPE 	serdesType;
	SERDES_SPEED 	serdesSpeed;
	SERDES_MODE 	serdesMode;
	MV_BOOL			swapRx;
	MV_BOOL			swapTx;
} SERDES_MAP;

/* Serdes ref clock options */
typedef enum {
	REF_CLOCK__25MHz,
	REF_CLOCK__100MHz,
	REF_CLOCK__40MHz,
	REF_CLOCK_UNSUPPORTED
} REF_CLOCK;

/* Serdes sequences */
typedef enum {
	SATA_PORT_0_ONLY_POWER_UP_SEQ,
	SATA_PORT_1_ONLY_POWER_UP_SEQ,
	SATA_POWER_UP_SEQ,
	SATA__1_5_SPEED_CONFIG_SEQ,
	SATA__3_SPEED_CONFIG_SEQ,
	SATA__6_SPEED_CONFIG_SEQ,
    SATA_ELECTRICAL_CONFIG_SEQ,
    SATA_TX_CONFIG_SEQ1,
    SATA_PORT_0_ONLY_TX_CONFIG_SEQ,
	SATA_PORT_1_ONLY_TX_CONFIG_SEQ,
    SATA_TX_CONFIG_SEQ2,

	SGMII_POWER_UP_SEQ,
	SGMII__1_25_SPEED_CONFIG_SEQ,
	SGMII__3_125_SPEED_CONFIG_SEQ,
	SGMII_ELECTRICAL_CONFIG_SEQ,
	SGMII_TX_CONFIG_SEQ1,
	SGMII_TX_CONFIG_SEQ2,

	PEX_POWER_UP_SEQ,
	PEX__2_5_SPEED_CONFIG_SEQ,
	PEX__5_SPEED_CONFIG_SEQ,
	PEX_ELECTRICAL_CONFIG_SEQ,
	PEX_TX_CONFIG_SEQ1,
	PEX_TX_CONFIG_SEQ2,
	PEX_TX_CONFIG_SEQ3,
	PEX_BY_4_CONFIG_SEQ,
	PEX_CONFIG_REF_CLOCK_25MHz_SEQ,
	PEX_CONFIG_REF_CLOCK_100MHz_SEQ,
	PEX_CONFIG_REF_CLOCK_40MHz_SEQ,

	USB3_POWER_UP_SEQ,
	USB3__HOST_SPEED_CONFIG_SEQ,
	USB3__DEVICE_SPEED_CONFIG_SEQ,
	USB3_ELECTRICAL_CONFIG_SEQ,
	USB3_TX_CONFIG_SEQ1,
	USB3_TX_CONFIG_SEQ2,
	USB3_TX_CONFIG_SEQ3,
	USB3_DEVICE_CONFIG_SEQ,

	USB2_POWER_UP_SEQ,

	SERDES_POWER_DOWN_SEQ,

	SGMII3_POWER_UP_SEQ,
	SGMII3__1_25_SPEED_CONFIG_SEQ,
	SGMII3_TX_CONFIG_SEQ1,
	SGMII3_TX_CONFIG_SEQ2,

	QSGMII_POWER_UP_SEQ,
	QSGMII__5_SPEED_CONFIG_SEQ,
	QSGMII_ELECTRICAL_CONFIG_SEQ,
	QSGMII_TX_CONFIG_SEQ1,
	QSGMII_TX_CONFIG_SEQ2,

	XAUI_POWER_UP_SEQ,
	XAUI__3_125_SPEED_CONFIG_SEQ,
	XAUI_ELECTRICAL_CONFIG_SEQ,
	XAUI_TX_CONFIG_SEQ1,
	XAUI_TX_CONFIG_SEQ2,

	RXAUI_POWER_UP_SEQ,
	RXAUI__6_25_SPEED_CONFIG_SEQ,
	RXAUI_ELECTRICAL_CONFIG_SEQ,
	RXAUI_TX_CONFIG_SEQ1,
	RXAUI_TX_CONFIG_SEQ2,

	SERDES_LAST_SEQ
} SERDES_SEQ;

/* The different sequence types for PEX and USB3 */
typedef enum {
	PEX,
	USB3,
	LAST_PEX_USB_SEQ_TYPE
} PEX_USB_SEQ_TYPES;

typedef enum {
	PEX__2_5Gbps,
	PEX__5Gbps,
	USB3__5Gbps_HOST,
	USB3__5Gbps_DEVICE,
	LAST_PEX_USB_SPEED_SEQ_TYPE
} PEX_USB_SPEED_SEQ_TYPES;

/* The different sequence types for SATA and SGMII */
typedef enum {
	SATA,
	SGMII,
	SGMII_3_125,
	LAST_SATA_SGMII_SEQ_TYPE
} SATA_SGMII_SEQ_TYPES;

typedef enum {
	QSGMII_SEQ_IDX,
	LAST_QSGMII_SEQ_TYPE
} QSGMII_SEQ_TYPES;

typedef enum {
	XAUI_SEQ_IDX,
	RXAUI_SEQ_IDX,
	LAST_XAUI_RXAUI_SEQ_TYPE
} XAUI_RXAUI_SEQ_TYPES;

typedef enum {
	SATA__1_5Gbps,
	SATA__3Gbps,
	SATA__6Gbps,
	SGMII__1_25Gbps,
	SGMII__3_125Gbps,
	LAST_SATA_SGMII_SPEED_SEQ_TYPE
} SATA_SGMII_SPEED_SEQ_TYPES;

/*********************************** Globals **********************************/

/* The board topology map,
   initialized in the beginning of mvCtrlHighSpeedSerdesPhyConfig */
SERDES_MAP serdesConfigurationMap[MAX_SERDES_LANES];

/* serdesSeqDb - holds all serdes sequences, their size and the relevant index in the data array
   initialized in serdesSeqInit */
MV_CFG_SEQ serdesSeqDb[SERDES_LAST_SEQ];

extern MV_U8 commonPhysSelectorsSerdesRev1Map[LAST_SERDES_TYPE][MAX_SERDES_LANES];
extern MV_U8 commonPhysSelectorsSerdesRev2Map[LAST_SERDES_TYPE][MAX_SERDES_LANES];

/*************************** Functions declarations ***************************/
/*******************************************************************************
* mvHwsCtrlSerdesRevGet
*
* DESCRIPTION: Get the Serdes revision number according to the device revision
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
* 		8bit Serdes revision number
*
*******************************************************************************/
MV_U8 mvHwsCtrlSerdesRevGet(MV_VOID);
/**************************************************************************
 * mvUpdateSerdesSelectPhyModeSeq -
 *
 * DESCRIPTION:          Updates the Serdes Select Phy Mode according to
 *                       the board configuration map
 * INPUT:                None.
 * OUTPUT:               Updates the sequences data base.
 * RETURNS:              MV_OK           - for success
 *                       MV_BAD_PARAM    - for bad combination of serdes num
 *                                         and serdes type
 ***************************************************************************/
MV_STATUS mvUpdateSerdesSelectPhyModeSeq(MV_VOID);
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
 * serdesTypeAndSpeedToSpeedSeq -
 *
 * DESCRIPTION:          Returns the relevant Speed Config sequence id
 *                       for power up, based on the serdes type and the
 *                       serdes speed given as input
 * INPUT:                serdesType      - Serdes type
 *                       baudRate        - Serdes speed
 * OUTPUT:               None.
 * RETURNS:              The relevant sequence id or SERDES_LAST_SEQ in case
 *                       the serdes type and speed are not supported together
 ***************************************************************************/
SERDES_SEQ serdesTypeAndSpeedToSpeedSeq(SERDES_TYPE serdesType,	SERDES_SPEED baudRate);
/**************************************************************************
 * mvHwsSerdesSeqInit -
 *
 * DESCRIPTION:          Inits serdes related Db
 * INPUT:                None.
 * OUTPUT:               serdesSeqDb is initialized
 * RETURNS:              MV_OK      - for success
 *                       MV_FAIL    - if Serdes initialization fail
 ***************************************************************************/
MV_STATUS mvHwsSerdesSeqInit(MV_VOID);
/**************************************************************************
 * mvHwsSerdesSeqDbInit -
 *
 * DESCRIPTION:          Inits serdesSeqDb, the array that contains for
 *                       every serdes sequence the following information:
 *                           1. opParamsPtr  - a pointer to the relevant
 *                                             MV_OP_PARAMS array
 *                           2. seqSize      - number of operations done
 *                                             during the sequences
 *                           3. dataArrIdx   - the relevant index in the
 *                                             data array in MV_OP_PARAMS
 * INPUT:                None.
 * OUTPUT:               serdesSeqDb is initialized
 * RETURNS:              MV_OK               - for success
 *                       MV_NOT_SUPPORTED    - if serdes revision number
 *                                             is not supported
 ***************************************************************************/
MV_STATUS mvHwsSerdesSeqDbInit(MV_VOID);
/**************************************************************************
 * mvHwsTwsiInitWrapper -
 *
 * DESCRIPTION:          Calls mvTwsiInit.
 * INPUT:                None.
 * OUTPUT:               TWSI is initialized
 * RETURNS:              MV_OK               - for success
 *                       MV_NOT_SUPPORTED    - if the tclk is not supported
 ***************************************************************************/
MV_STATUS mvHwsTwsiInitWrapper(MV_VOID);
/**************************************************************************
 * mvHwsPowerUpSerdesLanes -
 *
 * DESCRIPTION:          Executes power up for the 6 serdes lanes
 * INPUT:                serdesConfigMap     - the topology map.
 *                                             Holds the serdes type and
 *                                             speed for each lane
 * OUTPUT:               Serdes power up is executed
 * RETURNS:              MV_OK               - for success
 *                       MV_NOT_SUPPORTED    - ref clock is not supported
 *                       MV_BAD_PARAM        - a parameter was sent to
 *                                             mvSerdesPowerUpCtrl
 ***************************************************************************/
MV_STATUS mvHwsPowerUpSerdesLanes(SERDES_MAP  *serdesConfigMap);
/**************************************************************************
 * mvCtrlHighSpeedSerdesPhyConfig -
 *
 * DESCRIPTION:          Configures the power up sequence of the serdes
 * INPUT:                None.
 * OUTPUT:               None.
 * RETURNS:              MV_OK           -   for success
 ***************************************************************************/
MV_STATUS mvHwsCtrlHighSpeedSerdesPhyConfig(MV_VOID);
/**************************************************************************
 * mvSerdesPowerUpCtrl -
 *
 * DESCRIPTION:          Executes the serdes power up/down
 * INPUT:                serdesNum       -   Serdes lane number
 *                       serdesPowerUp   -   True for power up
 *                                           False for power down
 *                       serdesType      -   PEX, SATA, SGMII
 *                                           or USB3 (host or device)
 *                       baudRate        -   serdes speed
 *                       serdesMode      -   serdes mode
 *                       refClock        -   ref clock (25 or 100)
 * OUTPUT:               None.
 * RETURNS:              MV_OK           -   for success
 ***************************************************************************/
MV_STATUS mvSerdesPowerUpCtrl
(
    MV_U32 serdesNum,
    MV_BOOL serdesPowerUp,
    SERDES_TYPE serdesType,
    SERDES_SPEED baudRate,
    SERDES_MODE  serdesMode,
    REF_CLOCK refClock
);
/**************************************************************************
 * mvSerdesPowerUpCtrlExt -
 *
 * DESCRIPTION:          Addtional executes serdes power up/down
 * INPUT:                serdesNum       -   Serdes lane number
 *                       serdesPowerUp   -   True for power up
 *                                           False for power down
 *                       serdesType      -   SGMII3, QSGMII ,XAUI or RXAUI
 *                       baudRate        -   serdes speed
 *                       serdesMode      -   serdes mode
 *                       refClock        -   ref clock (25 or 100)
 * OUTPUT:               None.
 * RETURNS:              MV_OK           -   for success
 ***************************************************************************/
MV_STATUS mvSerdesPowerUpCtrlExt
(
	MV_U32 serdesNum,
	MV_BOOL serdesPowerUp,
	SERDES_TYPE serdesType,
	SERDES_SPEED baudRate,
	SERDES_MODE  serdesMode,
	REF_CLOCK refClock
);

/**************************************************************************
* mvHwsSerdesSiliconRefClockGet -
*
* DESCRIPTION: Get the silicon reference clock
*
* INPUT:        None.
*
* OUTPUT:       refClock   - Return the REF_CLOCK value:
*                            REF_CLOCK_25MHz, REF_CLOCK_40MHz or REF_CLOCK_100MHz
*
* RETURNS:      MV_OK        - for success
*               MV_BAD_PARAM - for fail
***************************************************************************/
MV_U32 mvHwsSerdesSiliconRefClockGet(MV_VOID);

/**************************************************************************
* mvHwsSerdesPexRefClockGet -
*
* DESCRIPTION: Get the reference clock value from DEVICE_SAMPLE_AT_RESET1_REG and check:
*              bit[2] for PEX#0, bit[3] for PEX#1, bit[30] for PEX#2, bit[31] for PEX#3.
*              If bit=0 --> REF_CLOCK_100MHz
*              If bit=1 && DEVICE_SAMPLE_AT_RESET2_REG bit[0]=0 --> REF_CLOCK_25MHz
*              If bit=1 && DEVICE_SAMPLE_AT_RESET2_REG bit[0]=1 --> REF_CLOCK_40MHz
*
* INPUT:        serdesType - Type of Serdes
*
* OUTPUT:       refClock   - Return the REF_CLOCK value:
*                            REF_CLOCK_25MHz, REF_CLOCK_40MHz or REF_CLOCK_100MHz
*
* RETURNS:      MV_OK        - for success
*               MV_BAD_PARAM - for fail
***************************************************************************/
MV_STATUS mvHwsSerdesPexRefClockGet(SERDES_TYPE serdesType, REF_CLOCK *refClock);

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
MV_STATUS mvHwsRefClockSet(MV_U32 serdesNum, SERDES_TYPE serdesType, REF_CLOCK refClock);
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
/*******************************************************************************
* mvHwsSerdesGetPhySelectorVal
*
* DESCRIPTION: Get the mapping of Serdes Selector values according to the
*               Serdes revision number
*
* INPUT:    serdesNum - Serdes number
*           serdesType - Serdes type
*
* OUTPUT: None
*
* RETURN:
* 		Mapping of Serdes Selector values
*
*******************************************************************************/
MV_U32 mvHwsSerdesGetPhySelectorVal(MV_32 serdesNum, SERDES_TYPE serdesType);
/**************************************************************************
 * mvHwsSerdesGetRefClockVal -
 *
 * DESCRIPTION:          return ref clock frequency according to device type.
 * INPUT:   			 serdesType      - PEX, USB3, SATA or SGMII.
 * OUTPUT:               None.
 * RETURNS:              Ref clock frequency
 ***************************************************************************/
MV_U32 mvHwsSerdesGetRefClockVal(SERDES_TYPE serdesType);
/**************************************************************************
 * mvHwsSerdesGetMaxLane -
 *
 * DESCRIPTION:          return number of lanes according to device type.
 * INPUT:   			 NONE.
 * OUTPUT:               None.
* RETURNS:              number of lanes
 ***************************************************************************/
MV_U32 mvHwsSerdesGetMaxLane(MV_VOID);
/**************************************************************************
 * mvHwsGetExtBaseAddr -
 *
 * DESCRIPTION:          return extended base address (when needed).
 * INPUT:   			 NONE.
 * OUTPUT:               None.
 * RETURNS:              number of lanes
 ***************************************************************************/
MV_STATUS mvHwsGetExtBaseAddr
(
	MV_U32 serdesNum,
	MV_U32 baseAddr,
	MV_U32 unitBaseOffset,
	MV_U32 *unitBaseReg,
	MV_U32 *unitOffset
);

/**************************************************************************
* mvHwsPexTxConfigSeq -
*
* DESCRIPTION:          Set PEX_TX_CONFIG_SEQ sequence init for PEXx4 mode
* INPUT:                serdesMap       - The board topology map
* OUTPUT:               None
* RETURNS:              MV_OK           - for success
*                       MV_BAD_PARAM    - for fail
***************************************************************************/
MV_STATUS mvHwsPexTxConfigSeq(SERDES_MAP *serdesMap);
/**************************************************************************
* mvHwsGetPhysicalSerdesNum -
*
* DESCRIPTION:          Returns the physical Serdes number since in some devices
*                       (such as 6810) Serdes number may not be consecutive
* INPUT:                serdesNum       - Serdes number
* OUTPUT:               None
* RETURNS:              Physical Serdes number
*
***************************************************************************/
MV_U32 mvHwsGetPhysicalSerdesNum(MV_U32 serdesNum);
/**************************************************************************
* mvHwsIsSerdesActive -
*
* DESCRIPTION:          Return True, if requested lane is Valid and Active for Device
* 			(some device flavors have specific lane topology limitation)
* INPUT:                serdesNum       - Serdes number
* OUTPUT:               None
* RETURNS:              True if lane is active
***************************************************************************/
MV_BOOL mvHwsIsSerdesActive(MV_U8 laneNum);
#endif /* _MV_HIGHSPEED_ENV_SPEC_H */
