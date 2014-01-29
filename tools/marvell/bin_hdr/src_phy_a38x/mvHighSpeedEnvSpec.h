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
	SERDES_TYPE serdesType;
	SERDES_SPEED serdesSpeed;
	SERDES_MODE serdesMode;
} SERDES_MAP;

/* Serdes ref clock options */
typedef enum {
	REF_CLOCK__25MHz,
	REF_CLOCK__100MHz,
	REF_CLOCK_UNSUPPORTED
} REF_CLOCK;

/* Serdes sequences */
typedef enum {
	SATA_ONLY_POWER_UP_SEQ,
	SATA_POWER_UP_SEQ,
	SATA__1_5_SPEED_CONFIG_SEQ,
	SATA__3_SPEED_CONFIG_SEQ,
	SATA__6_SPEED_CONFIG_SEQ,
	SATA_DB_TX_AMP_SEQ,
	SATA_TX_CONFIG_SEQ1,
	SATA_ONLY_TX_CONFIG_SEQ,
	SATA_TX_CONFIG_SEQ2,

	SGMII_POWER_UP_SEQ,
	SGMII__1_25_SPEED_CONFIG_SEQ,
	SGMII__3_125_SPEED_CONFIG_SEQ,
	SGMII_TX_CONFIG_SEQ1,
	SGMII_TX_CONFIG_SEQ2,

	PEX_POWER_UP_SEQ,
	PEX__2_5_SPEED_CONFIG_SEQ,
	PEX__5_SPEED_CONFIG_SEQ,
	PEX_TX_CONFIG_SEQ,
	PEX_BY_4_CONFIG_SEQ,

	USB3_POWER_UP_SEQ,
	USB3__HOST_SPEED_CONFIG_SEQ,
	USB3__DEVICE_SPEED_CONFIG_SEQ,
	USB3_TX_CONFIG_SEQ,
	USB3_DEVICE_CONFIG_SEQ,

	USB2_POWER_UP_SEQ,

	SERDES_POWER_DOWN_SEQ,

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
	LAST_SATA_SGMII_SEQ_TYPE
} SATA_SGMII_SEQ_TYPES;

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

/* Temp solution for memory allocations */
#ifdef MEM_ALLOCS
#define HEAP_SIZE 1024
char localHeap[HEAP_SIZE];
char* currHeapPtr = localHeap;

void* malloc(MV_U32 allocSize)
{
	char* ptr;

	if (currHeapPtr + allocSize >= localHeap + HEAP_SIZE)
		return NULL;
	ptr = currHeapPtr;
	currHeapPtr += allocSize;
	return (void*)ptr;
}

#endif

/*************************** Functions declarations ***************************/
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
SERDES_SEQ serdesTypeAndSpeedToSpeedSeq
(
	SERDES_TYPE serdesType,
	SERDES_SPEED baudRate
);

/**************************************************************************
 * serdesSeqInit -
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
 * RETURNS:              Nothing.
 ***************************************************************************/
MV_VOID serdesSeqInit(MV_VOID);

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
 * powerUpSerdesLanes -
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
MV_STATUS powerUpSerdesLanes(SERDES_MAP  *serdesConfigMap);

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
MV_STATUS mvSerdesPowerUpCtrl(MV_U32 serdesNum,
			      MV_BOOL serdesPowerUp,
			      SERDES_TYPE serdesType,
			      SERDES_SPEED baudRate,
			      SERDES_MODE  serdesMode,
			      REF_CLOCK refClock);

#endif /* _MV_HIGHSPEED_ENV_SPEC_H */
