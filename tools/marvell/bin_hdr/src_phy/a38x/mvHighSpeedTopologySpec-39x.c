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
#include "mvHighSpeedTopologySpec.h"
#include "mvSysEnvLib.h"
#include "printf.h"

#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
/**************************************************************************
 * loadTopologyCustomer -
 *
 * DESCRIPTION:          Loads the board topology for customer board
 * INPUT:                serdesMapArray  -   Not relevant
 * OUTPUT:               The board topology.
 * RETURNS:              MV_OK           -   for success
 ***************************************************************************/
MV_STATUS loadTopologyCustomer(SERDES_MAP  *serdesMapArray);

/************************* Load Topology - Customer Boards ****************************/
SERDES_MAP CustomerBoardTopologyConfig[][MAX_SERDES_LANES] =
{
{	/* Customer Board 0 Toplogy */
	{ PEX0,         __5Gbps,		   PEX_ROOT_COMPLEX_x1 },
	{ SGMII1,       __1_25Gbps,		   SERDES_DEFAULT_MODE },
	{ PEX1,	        __5Gbps,		   PEX_ROOT_COMPLEX_x1 },
	{ SGMII2,       __1_25Gbps,		   SERDES_DEFAULT_MODE },
	{ PEX2,         __5Gbps,		   PEX_ROOT_COMPLEX_x1 },
	{ RXAUI,        __6_25Gbps,		   SERDES_DEFAULT_MODE },
    { RXAUI,        __6_25Gbps,		   SERDES_DEFAULT_MODE },
},
{	/* Customer Board 1 Toplogy */
	{ PEX0,         __5Gbps,		   PEX_ROOT_COMPLEX_x1 },
	{ SGMII1,       __1_25Gbps,		   SERDES_DEFAULT_MODE },
	{ PEX1,	        __5Gbps,		   PEX_ROOT_COMPLEX_x1 },
	{ SGMII2,       __1_25Gbps,		   SERDES_DEFAULT_MODE },
	{ PEX2,         __5Gbps,		   PEX_ROOT_COMPLEX_x1 },
	{ RXAUI,        __6_25Gbps,		   SERDES_DEFAULT_MODE },
    { RXAUI,        __6_25Gbps,		   SERDES_DEFAULT_MODE },
}};

/***************************************************************************/
MV_STATUS loadTopologyCustomer(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	MV_U32 boardId = mvBoardIdGet();
	DEBUG_INIT_S("\nInit Customer board ");

	/* Updating the topology map */
	for (laneNum = 0; laneNum < mvHwsSerdesGetMaxLane(); laneNum++) {
		serdesMapArray[laneNum].serdesMode  =  CustomerBoardTopologyConfig[boardId][laneNum].serdesMode;
		serdesMapArray[laneNum].serdesSpeed =  CustomerBoardTopologyConfig[boardId][laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType  =  CustomerBoardTopologyConfig[boardId][laneNum].serdesType;
	}

	return MV_OK;
}

/**********************************************************************/
/* Load topology functions - Board ID is the index for function array */
/**********************************************************************/

loadTopologyFuncPtr loadTopologyFuncArr[] =
{
	loadTopologyCustomer,         /* Customer Board 0 */
	loadTopologyCustomer,         /* Customer Board 1*/
};

#else /* CONFIG_CUSTOMER_BOARD_SUPPORT */

/************************* Local functions declarations ***********************/

/*******************************************************************************
* mvSysUpdateLaneConfig
*
* DESCRIPTION: Configure the Serdes type for each lane number
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*       MV_OK if configuration pass,
*       MV_BAD_VALUE if selector value or Serdes type is wrong
*
*******************************************************************************/
MV_STATUS mvSysUpdateLaneConfig(SERDES_MAP* serdesTopology);

/**************************************************************************
 * loadTopologyDB -
 *
 * DESCRIPTION:          Loads the board topology for the DB_A38X_BP board
 * INPUT:                serdesMapArray  -   The struct that will contain
 *                                           the board topology map
 * OUTPUT:               The board topology map.
 * RETURNS:              MV_OK           -   for success
 *                       MV_FAIL         -   for failure (a wrong
 *                                           topology mode was read
 *                                           from the board)
 ***************************************************************************/
MV_STATUS loadTopologyDB(SERDES_MAP  *serdesMapArray);

loadTopologyFuncPtr loadTopologyFuncArr[] =
{
	loadTopologyDB,		/* DB */
};

/*********************************** Globals **********************************/

/* Serdes type Speed mapping */
SERDES_SPEED defaultSerdesSpeedMap[LAST_SERDES_TYPE] =
{
    __5Gbps,    /* PEX0 */
    __5Gbps,    /* PEX1 */
    __5Gbps,    /* PEX2 */
    __5Gbps,    /* PEX3 */
    __3Gbps,    /* SATA0 */
    __3Gbps,    /* SATA1 */
    __3Gbps,    /* SATA2 */
    __3Gbps,    /* SATA3 */
    __1_25Gbps, /* SGMII0 */
    __1_25Gbps, /* SGMII1 */
    __1_25Gbps, /* SGMII2 */
    __5Gbps,    /* QSGMII */
    __5Gbps,    /* USB3_HOST0 */
    __5Gbps,    /* USB3_HOST1 */
    __5Gbps,    /* USB3_DEVICE */
    __1_25Gbps, /* SGMII3 */
    __3_125Gbps,/* XAUI */
    __6_25Gbps, /* RXAUI */
};

/* Selector to Serdes type mapping */
SERDES_TYPE commonPhysType[MAX_SERDES_LANES][MAX_SELECTOR_VAL] =
{
   /* 0x1     0x2       0x3      0x4           0x5            0x6            0x7       0x8       0x9 */
	{ PEX0,   SATA0,    NA,	     SGMII0,	   NA,	          NA,            NA,       NA,       NA      },  /* Lane 0 */
	{ PEX0,   NA,       SATA0,	 NA,	       NA,	          USB3_HOST0,    QSGMII,   SGMII0,   SGMII1  },  /* Lane 1 */
	{ NA,     PEX1,     SATA1,	 NA,	       SGMII1,	      NA,            NA,	   NA,       NA      },  /* Lane 2 */
	{ PEX3,   NA,       SATA3,	 NA,	       USB3_HOST1,	  USB3_DEVICE,   SGMII2,   XAUI,     NA      },  /* Lane 3 */
    { PEX1,   NA,       NA,	     USB3_HOST0,   USB3_DEVICE,	  SATA2,         PEX2,	   SGMII3,   XAUI    },  /* Lane 4 */
	{ PEX2,   SATA2,    NA,	     USB3_HOST1,   USB3_DEVICE,	  SGMII2,        NA,	   XAUI,     NA      },  /* Lane 5 */
    { PEX1,   SGMII3,   NA,	     XAUI,	       NA,	          NA,            NA,	   NA,       NA      },  /* Lane 6 */
};

/*************************************/
/** Load topology - Marvell DB - BP **/
/*************************************/

/* Configuration options */
SERDES_MAP DbConfigDefault[MAX_SERDES_LANES] =
{
	{ PEX0,         __5Gbps,		   PEX_ROOT_COMPLEX_x1 },
	{ SGMII1,       __1_25Gbps,		   SERDES_DEFAULT_MODE },
	{ PEX1,	        __5Gbps,		   PEX_ROOT_COMPLEX_x1 },
	{ SGMII2,       __1_25Gbps,		   SERDES_DEFAULT_MODE },
	{ PEX2,         __5Gbps,		   PEX_ROOT_COMPLEX_x1 },
	{ DEFAULT_SERDES/*RXAUI*/,        __6_25Gbps,		   SERDES_DEFAULT_MODE },
    { DEFAULT_SERDES/*RXAUI*/,        __6_25Gbps,		   SERDES_DEFAULT_MODE },
};

/*************************** Functions implementation *************************/
/***************************************************************************/

/************************** Load topology - Marvell DB ********************************/
/***************************************************************************/

MV_STATUS loadTopologyDB(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	SERDES_MAP* serdesTopology = DbConfigDefault;
	MV_STATUS res = MV_OK;

#ifdef CONFIG_CMD_BOARDCFG
	SERDES_MAP sysTopology[MAX_SERDES_LANES];

	DEBUG_INIT_FULL_S("\n### loadTopologyDB (Dynamic) ###\n");

	/* Configure the Serdes type for each lane number */
	res = mvSysUpdateLaneConfig(sysTopology);
	if(res != MV_OK) {
		mvPrintf("loadTopologyDB: Error: mvSysUpdateLaneConfig failed (res = 0x%x)\n", res);
		mvPrintf("\tusing default topology\n");
	}
	else
	{
		serdesTopology = sysTopology;
	}
#else
	DEBUG_INIT_FULL_S("\n### loadTopologyDB ###\n");
#endif

    /* Updating the topology map */
	for (laneNum = 0; laneNum < mvHwsSerdesGetMaxLane(); laneNum++) {
		serdesMapArray[laneNum].serdesMode  =  serdesTopology[laneNum].serdesMode;
		serdesMapArray[laneNum].serdesSpeed =  serdesTopology[laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType  =  serdesTopology[laneNum].serdesType;
	}

	return res;
}

#ifdef CONFIG_CMD_BOARDCFG
/*******************************************************************************
* mvSysUpdateLaneConfig
*
* DESCRIPTION: Configure the Serdes type for each lane number
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*       MV_OK if configuration pass,
*       MV_BAD_VALUE if selector value or Serdes type is wrong
*
*******************************************************************************/
MV_STATUS mvSysUpdateLaneConfig
(
	SERDES_MAP* serdesTopology
)
{
    MV_CONFIG_TYPE_ID configId;
    SERDES_TYPE serdesType;
    MV_U8 serdesNum;
    MV_U8 selectorVal;
	MV_STATUS res = MV_OK;

    /* Initialize all board configuration */
    res = mvSysEnvConfigInit();
    if(res != MV_OK) {
		mvPrintf("loadTopologyDB: Error: mvSysEnvConfigInit failed (res = 0x%x)\n", res);
		return MV_INIT_ERROR;
    }

    for (configId = MV_CONFIG_LANE0; configId <= MV_CONFIG_LANE6; configId++) {
        serdesNum = configId - MV_CONFIG_LANE0;
        selectorVal = mvSysEnvConfigGet(configId);
        serdesType = commonPhysType[serdesNum][selectorVal];

        if (selectorVal > MAX_SELECTOR_VAL) {
            mvPrintf("mvSysUpdateLaneConfig: Error: Selector value %0x%x (Serdes %d) is bigger then max value (0x%x)\n",
					 selectorVal, serdesNum, MAX_SELECTOR_VAL);
            return MV_BAD_VALUE;
        }

        if (serdesType == NA) {
            mvPrintf("mvSysUpdateLaneConfig: Error: Serdes type for selctor value 0x%x (Serdes %d) is not valid\n",
					 selectorVal, serdesNum);
            return MV_BAD_VALUE;
        }

		serdesTopology[serdesNum].serdesType  = serdesType;
		serdesTopology[serdesNum].serdesSpeed = defaultSerdesSpeedMap[serdesType];
        serdesTopology[serdesNum].serdesMode  = SERDES_DEFAULT_MODE; /* set to default */

        /* Update the Serdes mode for PEX type:
		   The Serdes mode can be PEXx1 or PEXx4 and it can be determind accroding to the
		   Selector mapping of PEXs 1-3: if their selectorVal are 0x2 the Serdes mode
		   is PEXx4, otherwise it will be PEXx1 (this is done after all
		   Serdes types are updated). */
        if(serdesTopology[serdesNum].serdesType <= PEX3) {
            serdesTopology[serdesNum].serdesMode = (selectorVal == 0x2) ?
                PEX_ROOT_COMPLEX_x4 : PEX_ROOT_COMPLEX_x1;
        }

        /* The Selector mapping for lanes 5 and 6 can be either XAUI or RXAUI mode type
           and it is depends on previous lanes 4.
		   If the Selector value for lanes 4 is 0x9, then the Serdes type for lanes
		   5 and 6 is XAUI, otherwise the Serdes type for lanes 5 and 6 is RXAUI */
        if ((configId == MV_CONFIG_LANE5) && (selectorVal == 0x8)) {
            serdesTopology[serdesNum].serdesType =
				(serdesTopology[4].serdesType == XAUI) ? XAUI : RXAUI;
        }
        else if ((configId == MV_CONFIG_LANE6) && (selectorVal == 0x4)) {
            serdesTopology[serdesNum].serdesType =
				(serdesTopology[4].serdesType == XAUI) ? XAUI : RXAUI;
        }
    }

    /* if serdes 0 type is PEX, we can know it's mode only after Serdes 1-3 mode
       is detectied (since its selector value (0x1) is the same for PEXx1 and PEXx4 */
    if(serdesTopology[0].serdesType == PEX0 && serdesTopology[1].serdesType == PEX1) {
            serdesTopology[0].serdesMode = serdesTopology[1].serdesMode;
	}

    return MV_OK;
}
#endif /* CONFIG_CMD_BOARDCFG */


#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */

