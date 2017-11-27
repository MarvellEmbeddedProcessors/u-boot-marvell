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
 * DESCRIPTION: 		 Loads the board topology for customer board
 * INPUT:   			 serdesMapArray  -   Not relevant
 * OUTPUT:  			 The board topology.
 * RETURNS: 			 MV_OK  		 -   for success
 ***************************************************************************/
MV_STATUS loadTopologyCustomer(SERDES_MAP  *serdesMapArray);

/************************* Load Topology - Customer Boards ****************************/
SERDES_MAP CustomerBoardTopologyConfig[][MAX_SERDES_LANES] =
{{
	/* Customer Board #0 Topology - reference from Marvell DB-BP board */
	{ SATA0,		__3Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ PEX0,			__5Gbps,	PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ PEX1,			__5Gbps,	PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ SATA3,		__3Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ USB3_HOST0,	__5Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ USB3_HOST1,	__5Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE }
},
{	/* ClearFog A1 board topology */
#if 1
	{ SATA0,	__3Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
#else
	{ PEX0,		__5Gbps,	PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
#endif
	{ SGMII1,	__1_25Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
#if 1
	{ PEX1,	  	__5Gbps,	PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
#else
	{ SATA1,	__3Gbps,	SERDES_DEFAULT_MODE,		MV_TRUE,	MV_FALSE },
#endif
	{ USB3_HOST1,	__5Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
#if 1
	{ PEX2,	  	__5Gbps,	PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
#else
	{ SATA2,	__3Gbps,	SERDES_DEFAULT_MODE,		MV_TRUE,	MV_FALSE },
#endif
	{ SGMII2,	__1_25Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE }
}};


/***************************************************************************/
MV_STATUS loadTopologyCustomer(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	MV_U32 boardId = mvBoardIdGet();
	MV_U32 boardIdIndex = mvBoardIdIndexGet(boardId);
	DEBUG_INIT_S("\nInit Customer board ");

	/* Updating the topology map */
	for (laneNum = 0; laneNum < mvHwsSerdesGetMaxLane(); laneNum++) {
		serdesMapArray[laneNum].serdesMode  =  CustomerBoardTopologyConfig[boardIdIndex][laneNum].serdesMode;
		serdesMapArray[laneNum].serdesSpeed =  CustomerBoardTopologyConfig[boardIdIndex][laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType  =  CustomerBoardTopologyConfig[boardIdIndex][laneNum].serdesType;
		serdesMapArray[laneNum].swapRx      =  CustomerBoardTopologyConfig[boardIdIndex][laneNum].swapRx;
		serdesMapArray[laneNum].swapTx      =  CustomerBoardTopologyConfig[boardIdIndex][laneNum].swapTx;
	}

	return MV_OK;
}

/**********************************************************************/
/* Load topology functions - Board ID is the index for function array */
/**********************************************************************/

loadTopologyFuncPtr loadTopologyFuncArr[] =
{
	loadTopologyCustomer,   	  /* Customer Board 0 */
	loadTopologyCustomer,   	  /* Customer Board 1*/
};

#else /* CONFIG_CUSTOMER_BOARD_SUPPORT */

/*********************************** Enums ************************************/

/* Topology map options for the DB_A38X_BP board */
typedef enum {
	DB_CONFIG_SLM1363_C,
	DB_CONFIG_SLM1363_D,
	DB_CONFIG_SLM1363_E,
	DB_CONFIG_SLM1363_F,
	DB_CONFIG_SLM1364_D,
	DB_CONFIG_SLM1364_E,
	DB_CONFIG_SLM1364_F,
	DB_CONFIG_DEFAULT,
	DB_NO_TOPOLOGY
} TOPOLOGY_CONFIG_DB;

/************************* Local functions declarations ***********************/
/**************************************************************************
 * topologyConfigDBModeGet -
 *
 * DESCRIPTION: 		 Gets the relevant topology mode (index).
 *  					 for loadTopologyDB use only.
 * INPUT:   			 None.
 * OUTPUT:  			 None.
 * RETURNS: 			 the topology mode
 ***************************************************************************/
MV_U8 topologyConfigDBModeGet(MV_VOID);

/**************************************************************************
 * loadTopologyXXX -
 *
 * DESCRIPTION: 		 Loads the board topology for the XXX board
 * INPUT:   			 serdesMapArray  -   The struct that will contain
 *  										 the board topology map
 * OUTPUT:  			 The board topology map.
 * RETURNS: 			 MV_OK  		 -   for success
 *  					 MV_FAIL		 -   for failure (a wrong
 *  										 topology mode was read
 *  										 from the board)
 ***************************************************************************/

/* loadTopologyDB - Loads the board topology for DB Board */
MV_STATUS loadTopologyDB(SERDES_MAP  *serdesMapArray);


/* loadTopologyRD - Loads the board topology for RD Board */
MV_STATUS loadTopologyRD(SERDES_MAP  *serdesMapArray);
/***************************************************************************/

/* loadTopologyRDNas - Loads the board topology for RD NAS Board */
MV_STATUS loadTopologyRDNas(SERDES_MAP  *serdesMapArray);
/***************************************************************************/

/* loadTopologyRDAp - Loads the board topology for RD Ap Board */
MV_STATUS loadTopologyRDAp(SERDES_MAP  *serdesMapArray);

/* loadTopologyDBAp - Loads the board topology for DB-AP Board */
MV_STATUS loadTopologyDBAp(SERDES_MAP  *serdesMapArray);
/***************************************************************************/

/* loadTopologyDBGp - Loads the board topology for DB GP Board */
MV_STATUS loadTopologyDBGp(SERDES_MAP  *serdesMapArray);

/* loadTopologyDB381 - Loads the board topology for 381 DB-BP Board */
MV_STATUS loadTopologyDB381(SERDES_MAP  *serdesMapArray);

/* loadTopologyDBAMC - Loads the board topology for DB-AMC Board */
MV_STATUS loadTopologyDBAMC(SERDES_MAP  *serdesMapArray);

/**************************************************************************
* mvHwsUpdateDeviceToplogy
* DESCRIPTION: Update the default board topology for specific device Id
* INPUT:
*   	topologyConfigPtr - pointer to the Serdes mapping
*   	topologyMode - topology mode (index)
* OUTPUT: None
* RRETURNS:
*   	MV_OK - if updating the board topology success
*   	MV_BAD_PARAM - if the input parameter is wrong
***************************************************************************/
MV_STATUS mvHwsUpdateDeviceToplogy(SERDES_MAP* topologyConfigPtr, TOPOLOGY_CONFIG_DB topologyMode);

/**************************************************************************
 * loadTopologyRDSgmiiUsb -
 *
 * DESCRIPTION: 		 For RD board check if lane 4 is USB3 or SGMII
 * INPUT:   			 None
 * OUTPUT:  			 isSgmii - return MV_TRUE if lane 4 is SGMII
 *  			   return MV_FALSE if lane 4 is USB.
 * RETURNS: 			 MV_OK  		 -   for success
 ***************************************************************************/
MV_STATUS loadTopologyRDSgmiiUsb(MV_BOOL *isSgmii);

/**************************************************************************
 * loadTopologyUSBModeGet -
 *
 * DESCRIPTION: 		 For DB board check if USB3.0 mode
 * INPUT:   			 None
 * OUTPUT:  			 twsiData - return data read from S@R via I2C
 * RETURNS: 			 MV_OK  		 -   for success
 ***************************************************************************/
MV_STATUS loadTopologyUSBModeGet(MV_U8 *twsiData);

loadTopologyFuncPtr loadTopologyFuncArr[] =
{
	loadTopologyRD, 	/* RD NAS */
	loadTopologyDB, 	/* 6820 DB-BP (A38x) */
	loadTopologyRD, 	/* RD AP */
	loadTopologyDBAp,	/* DB AP */
	loadTopologyDBGp,	/* DB GP */
	loadTopologyDB381,	/* 6821 DB-BP (A381) */
	loadTopologyDBAMC,	/* DB-AMC */
};

/*********************************** Globals **********************************/
/*****************************************/
/** Load topology - Marvell 380 DB - BP **/
/*****************************************/
/* Configuration options */
SERDES_MAP DbConfigDefault[MAX_SERDES_LANES] =
{
	{ SATA0,	__3Gbps,   	   SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ PEX0, 	__5Gbps,   	   PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ PEX1, 	__5Gbps,   	   PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ SATA3,	__3Gbps,   	   SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ USB3_HOST0,	__5Gbps,  	   SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ USB3_HOST1,	__5Gbps,  	   SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE }
};

SERDES_MAP DbConfigSLM1363_C[MAX_SERDES_LANES] =
{
	{ PEX0, 	 	 __5Gbps,  				PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ DEFAULT_SERDES,	LAST_SERDES_SPEED,			SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ PEX1, 	  	__5Gbps,  				PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ PEX3, 	  	__5Gbps,  				PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ SATA2,	  	__3Gbps,  				SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ DEFAULT_SERDES,	LAST_SERDES_SPEED,			SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
};

SERDES_MAP DbConfigSLM1363_D[MAX_SERDES_LANES] =
{
	{ PEX0, 	  __5Gbps,  	PEX_ROOT_COMPLEX_x4,		MV_FALSE,	MV_FALSE },
	{ PEX0, 	  __5Gbps,  	PEX_ROOT_COMPLEX_x4,		MV_FALSE,	MV_FALSE },
	{ PEX0, 	  __5Gbps,  	PEX_ROOT_COMPLEX_x4,		MV_FALSE,	MV_FALSE },
	{ PEX0, 	  __5Gbps,  	PEX_ROOT_COMPLEX_x4,		MV_FALSE,	MV_FALSE },
	{ USB3_HOST0,	  __5Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ USB3_HOST1,	  __5Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE }
};

SERDES_MAP DbConfigSLM1363_E[MAX_SERDES_LANES] =
{
	{ PEX0, 	  		__5Gbps,  			PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ USB3_HOST0,		  	__5Gbps,  			SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ SATA1,	  		__3Gbps,  			SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ USB3_HOST1,			__5Gbps,  			SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ DEFAULT_SERDES, 		LAST_SERDES_SPEED,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ SATA2,	  		__3Gbps,  			SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE }
};

SERDES_MAP DbConfigSLM1363_F[MAX_SERDES_LANES] =
{
	{ PEX0, 	  		__5Gbps,  			PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ DEFAULT_SERDES, 		LAST_SERDES_SPEED,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ PEX1, 	  		__5Gbps,  			PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ PEX3, 	  		__5Gbps,  			PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ SATA2,	  	 	__3Gbps,  			SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ USB3_HOST1,			__5Gbps,  			SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE }
};

SERDES_MAP DbConfigSLM1364_D[MAX_SERDES_LANES] =
{
	{ DEFAULT_SERDES, 	LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ SGMII0,     		__3_125Gbps,  	  	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ DEFAULT_SERDES, 	LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ DEFAULT_SERDES, 	LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ SGMII1,     		__3_125Gbps,  	  	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ SGMII2,    		__3_125Gbps,  	  	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE }
};

SERDES_MAP DbConfigSLM1364_E[MAX_SERDES_LANES] =
{
	{ SGMII0,     		__3_125Gbps,  	  	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ SGMII1,     		__3_125Gbps,  	  	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ DEFAULT_SERDES, 	LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ SGMII2,     		__3_125Gbps,  	  	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ DEFAULT_SERDES, 	LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ PEX2, 	  	__5Gbps,  		  PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE }
};

SERDES_MAP DbConfigSLM1364_F[MAX_SERDES_LANES] =
{
	{ SGMII0,	 	__3_125Gbps,		  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ DEFAULT_SERDES, 	LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ SGMII1,	  	__3_125Gbps,		  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ SGMII2,	  	__3_125Gbps,		  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ DEFAULT_SERDES, 	LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
	{ PEX2,		  	__5Gbps,	  	  PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE }
};

/**************************************************************************/
/** The following structs are mapping for DB board 'SatR' configuration **/
/**************************************************************************/
SERDES_MAP DBSatRConfigLane1[SATR_DB_LANE1_MAX_OPTIONS] =
{
/* 0 */	{ DEFAULT_SERDES,	LAST_SERDES_SPEED,	SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
/* 1 */	{ PEX0,			__5Gbps,		PEX_ROOT_COMPLEX_x1,	MV_FALSE,	MV_FALSE },
/* 2 */	{ SATA0,		__3Gbps,		PEX_ROOT_COMPLEX_x1,	MV_FALSE,	MV_FALSE },
/* 3 */	{ SGMII0,		__3_125Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
/* 4 */	{ SGMII1,		__3_125Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
/* 5 */	{ USB3_HOST0,		__5Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
/* 6 */	{ QSGMII,		__5Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE }
};

SERDES_MAP DBSatRConfigLane2[SATR_DB_LANE2_MAX_OPTIONS] =
{
/* 0 */	{ DEFAULT_SERDES,	LAST_SERDES_SPEED,	SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
/* 1 */	{ PEX1,			__5Gbps,		PEX_ROOT_COMPLEX_x1,	MV_FALSE,	MV_FALSE },
/* 2 */	{ SATA1,		__3Gbps,		PEX_ROOT_COMPLEX_x1,	MV_FALSE,	MV_FALSE },
/* 3 */	{ SGMII1,		__3_125Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE }
};

/*******************************************************/
/* Configuration options DB ****************************/
/* mapping from TWSI address data to configuration map */
/*******************************************************/
SERDES_MAP* topologyConfigDB[] =
{
	DbConfigSLM1363_C,
	DbConfigSLM1363_D,
	DbConfigSLM1363_E,
	DbConfigSLM1363_F,
	DbConfigSLM1364_D,
	DbConfigSLM1364_E,
	DbConfigSLM1364_F,
	DbConfigDefault
};

/*************************************/
/** Load topology - Marvell DB - AP **/
/*************************************/
SERDES_MAP DbApConfigDefault[MAX_SERDES_LANES] =
{
/* 0 */ { PEX0, 		__5Gbps,		PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
/* 1 */ { SGMII1,   	__3_125Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
/* 2 */ { PEX1, 		__5Gbps,		PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
/* 3 */ { SGMII2,   	__3_125Gbps,	SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
/* 4 */ { USB3_HOST0,   __5Gbps,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
/* 5 */ { PEX2, 		__5Gbps,		PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE }
};

/*************************************/
/** Load topology - Marvell DB - GP **/
/*************************************/
SERDES_MAP DbGpConfigDefault[MAX_SERDES_LANES] =
{
/* 0 */ { PEX0, 		__5Gbps,		PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
/* 1 */ { SATA0,		__3Gbps,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
/* 2 */ { SATA1,		__3Gbps,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
/* 3 */ { SATA3,		__3Gbps,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
/* 4 */ { SATA2,		__3Gbps,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
/* 5 */ { USB3_HOST1,   __5Gbps,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE }
};

SERDES_MAP DbAmcConfigDefault[MAX_SERDES_LANES] =
{
/* 0 */ { PEX0, 		__5Gbps,		PEX_ROOT_COMPLEX_x4,		MV_FALSE,	MV_FALSE },
/* 1 */ { PEX0, 		__5Gbps,		PEX_ROOT_COMPLEX_x4,		MV_FALSE,	MV_FALSE },
/* 2 */ { PEX0, 		__5Gbps,		PEX_ROOT_COMPLEX_x4,		MV_FALSE,	MV_FALSE },
/* 3 */ { PEX0, 		__5Gbps,		PEX_ROOT_COMPLEX_x4,		MV_FALSE,	MV_FALSE },
/* 4 */ { SGMII1,		__3_125Gbps,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
/* 5 */ { SGMII2,		__3_125Gbps,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE },
};

/*****************************************/
/** Load topology - Marvell 381 DB - BP **/
/*****************************************/
/* Configuration options */
SERDES_MAP Db381ConfigDefault[MAX_SERDES_LANES] =
{
	{ SATA0,		__3Gbps,		SERDES_DEFAULT_MODE,		MV_TRUE,	MV_TRUE  },
	{ PEX0, 		__5Gbps,		PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ PEX1, 		__5Gbps,		PEX_ROOT_COMPLEX_x1,		MV_FALSE,	MV_FALSE },
	{ USB3_HOST1,		__5Gbps,		SERDES_DEFAULT_MODE,		MV_FALSE,	MV_FALSE }
};

SERDES_MAP DbConfigSLM1427[MAX_SERDES_LANES] =
{
	{ SATA0,		__3Gbps,		SERDES_DEFAULT_MODE,		MV_TRUE,	MV_TRUE  },
	{ PEX0, 		__5Gbps,		PEX_ROOT_COMPLEX_x1,		MV_TRUE,	MV_TRUE },
	{ SATA1,		__3Gbps,		SERDES_DEFAULT_MODE,		MV_TRUE,	MV_TRUE },
	{ USB3_HOST1,		__5Gbps,		SERDES_DEFAULT_MODE,		MV_TRUE,	MV_TRUE }
};

SERDES_MAP DbConfigSLM1426[MAX_SERDES_LANES] =
{
	{ SATA0,		__3Gbps,		SERDES_DEFAULT_MODE,		MV_TRUE,	MV_TRUE  },
	{ USB3_HOST0,		__5Gbps,		SERDES_DEFAULT_MODE,		MV_TRUE,	MV_TRUE },
	{ SATA1,		__3Gbps,		SERDES_DEFAULT_MODE,		MV_TRUE,	MV_TRUE  },
	{ SGMII2,    		__3_125Gbps,  	  	  SERDES_DEFAULT_MODE,		MV_TRUE,	MV_TRUE }
};

/* this enum must be aligned with topologyConfigDB381 array,
 * every update to this enum requires update to topologyConfigDB381 array*/
typedef enum {
	DB_CONFIG_SLM1427,	/* enum for DbConfigSLM1427 */
	DB_CONFIG_SLM1426,	/* enum for DbConfigSLM1426 */
	DB_381_CONFIG_DEFAULT,
	DB_381_NO_TOPOLOGY
} TOPOLOGY_CONFIG_DB381;

/* this array must be aligned with TOPOLOGY_CONFIG_DB381 enum,
 * every update to this array requires update to TOPOLOGY_CONFIG_DB381 enum*/
SERDES_MAP* topologyConfigDB381[] =
{
	DbConfigSLM1427,
	DbConfigSLM1426,
	Db381ConfigDefault,
};

/*************************** Functions implementation *************************/
/***************************************************************************/

/************************** Load topology - Marvell DB ********************************/
/***************************************************************************/
MV_U8 topologyConfigDBModeGet(MV_VOID)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 mode;

	DEBUG_INIT_FULL_S("\n### topologyConfigDBModeGet ###\n");

	/* Default - return DB_CONFIG_DEFAULT */

	/* Initializing twsiSlave in order to read from the TWSI address */
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_FALSE;

	/* SLM1363 Module */
	twsiSlave.slaveAddr.address = DB_GET_MODE_SLM1363_ADDR;
	if (mvTwsiRead(0, &twsiSlave, &mode, 1) == MV_OK) {
		switch (mode & 0xF) {
		case 0xC:
			DEBUG_INIT_S("\nInit DB board SLM 1363 C topology\n");
			return DB_CONFIG_SLM1363_C;
		case 0xD:
			DEBUG_INIT_S("\nInit DB board SLM 1363 D topology\n");
			return DB_CONFIG_SLM1363_D;
		case 0xE:
			DEBUG_INIT_S("\nInit DB board SLM 1363 E topology\n");
			return DB_CONFIG_SLM1363_E;
		case 0xF:
			DEBUG_INIT_S("\nInit DB board SLM 1363 F topology\n");
			return DB_CONFIG_SLM1363_F;
		default:	/* not the right module */
			break;
		}
	}

	/* SLM1364 Module */
	twsiSlave.slaveAddr.address = DB_GET_MODE_SLM1364_ADDR;
	if (mvTwsiRead(0, &twsiSlave, &mode, 1) != MV_OK)
	{
		DEBUG_INIT_S("\nInit DB board default topology\n");
		return DB_CONFIG_DEFAULT;
	}

	switch (mode & 0xF) {
	case 0xD:
		DEBUG_INIT_S("\nInit DB board SLM 1364 D topology\n");
		return DB_CONFIG_SLM1364_D;
	case 0xE:
		DEBUG_INIT_S("\nInit DB board SLM 1364 E topology\n");
		return DB_CONFIG_SLM1364_E;
	case 0xF:
		DEBUG_INIT_S("\nInit DB board SLM 1364 F topology\n");
		return DB_CONFIG_SLM1364_F;
	default: /* Default configuration */
		DEBUG_INIT_S("\nInit DB board default topology\n");
		return DB_CONFIG_DEFAULT;
	}
}

MV_U8 topologyConfigDB381ModeGet(MV_VOID)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 mode;

	DEBUG_INIT_FULL_S("\n### topologyConfigDB381ModeGet ###\n");

	/* Initializing twsiSlave in order to read from the TWSI address */
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;

	/* SLM1426/7 Module */
	twsiSlave.slaveAddr.address = DB381_GET_MODE_SLM1426_1427_ADDR;
	if (mvTwsiRead(0, &twsiSlave, &mode, 1) == MV_OK) {
		switch (mode & 0xF) {
		case 0x1:
			DEBUG_INIT_S("\nInit DB-381 board SLM 1427 topology\n");
			return DB_CONFIG_SLM1427;
		case 0x2:
			DEBUG_INIT_S("\nInit DB-381 board SLM 1426 topology\n");
			return DB_CONFIG_SLM1426;
		default:	/* not the right module */
			break;
		}
	}

	/* in case not detected any supported module, use default topology */
	DEBUG_INIT_S("\nInit DB-381 board default topology\n");
	return DB_381_CONFIG_DEFAULT;
}

/* read SatR field 'sgmiispeed' and update lane topology SGMII entries speed setup */
MV_STATUS updateTopologySgmiiSpeed(SERDES_MAP  *serdesMapArray)
{
	MV_U32 serdesType, laneNum;
	MV_U8 configVal;
	MV_TWSI_SLAVE twsiSlave;

	twsiSlave.slaveAddr.address = EEPROM_I2C_ADDR;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_TRUE;

	/* update SGMII speed settings by 'sgmiispeed' SatR value */
	for (laneNum = 0; laneNum < mvHwsSerdesGetMaxLane(); laneNum++) {
		serdesType = serdesMapArray[laneNum].serdesType;
		twsiSlave.offset = 0;
		/*Read SatR configuration for SGMII speed*/
		if((serdesType == SGMII0) || (serdesType == SGMII1) || (serdesType == SGMII2) ){
			/* Read SatR 'sgmiispeed' value */
			if (mvTwsiRead(0, &twsiSlave, &configVal, 1) != MV_OK) {
				mvPrintf("%s: TWSI Read of 'sgmiispeed' failed\n", __func__);
				return MV_FAIL;
			}

			if( 0 == (configVal & 0x40)){
				serdesMapArray[laneNum].serdesSpeed = __1_25Gbps;
			}
			else{
				serdesMapArray[laneNum].serdesSpeed = __3_125Gbps;
			}
		}
	}
	return MV_OK;
}

SERDES_MAP DefaultLane = { DEFAULT_SERDES, LAST_SERDES_SPEED, SERDES_DEFAULT_MODE };
MV_BOOL isCustomTopology = MV_FALSE; /* indicate user of non-default topology */
/* read SatR fields (dbserdes1/2 , gpserdes1/2/5) and update lane topology accordingly */
MV_STATUS updateTopologySatR(SERDES_MAP  *serdesMapArray)
{
	MV_U8 configVal, laneSelect, i;
	MV_TWSI_SLAVE twsiSlave;
	MV_U32 boardId = mvBoardIdGet();

	twsiSlave.slaveAddr.address = EEPROM_I2C_ADDR;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_TRUE;

	switch (boardId) {
	case DB_68XX_ID: /* read 'dbserdes1' & 'dbserdes2' */
	case DB_BP_6821_ID:
		twsiSlave.offset = 1;
		if (mvTwsiRead(0, &twsiSlave, &configVal, 1) != MV_OK) {
			mvPrintf("%s: TWSI Read of 'dbserdes1/2' failed\n", __func__);
			return MV_FAIL;
		}

		/* Lane #1 */
		laneSelect = (configVal & SATR_DB_LANE1_CFG_MASK) >> SATR_DB_LANE1_CFG_OFFSET;
		if (laneSelect >= SATR_DB_LANE1_MAX_OPTIONS) {
			mvPrintf("\n\%s: Error: invalid value for SatR field 'dbserdes1' (%x)\n", __func__, laneSelect);
			mvPrintf("\tSkipping Topology update (run 'SatR write default')\n", __func__, laneSelect);
			return MV_FAIL;
		}
		/* if modified default serdesType for lane#1, update topology and mark it as custom */
		if (serdesMapArray[1].serdesType != DBSatRConfigLane1[laneSelect].serdesType) {
			serdesMapArray[1] = DBSatRConfigLane1[laneSelect];
			isCustomTopology = MV_TRUE;
			if (boardId == DB_BP_6821_ID)	/* DB 381/2 board has inverted SerDes polarity */
				serdesMapArray[1].swapRx = serdesMapArray[1].swapTx = MV_TRUE;
		}

		/* Lane #2 */
		laneSelect = (configVal & SATR_DB_LANE2_CFG_MASK) >> SATR_DB_LANE2_CFG_OFFSET;
		if (laneSelect >= SATR_DB_LANE2_MAX_OPTIONS) {
			mvPrintf("\n\%s: Error: invalid value for SatR field 'dbserdes2' (%x)\n", __func__, laneSelect);
			mvPrintf("\tSkipping Topology update (run 'SatR write default')\n", __func__, laneSelect);
			return MV_FAIL;
		}
		/* if modified default serdesType for lane@2, update topology and mark it as custom */
		if (serdesMapArray[2].serdesType != DBSatRConfigLane2[laneSelect].serdesType) {
			serdesMapArray[2] = DBSatRConfigLane2[laneSelect];
			isCustomTopology = MV_TRUE;
			if (boardId == DB_BP_6821_ID)	/* DB 381/2 board has inverted SerDes polarity */
				serdesMapArray[2].swapRx = serdesMapArray[2].swapTx = MV_TRUE;
		}

		if (isCustomTopology == MV_TRUE) {
			/* check for conflicts with detected lane #1 and lane #2 (Disable conflicted lanes) */
			for (i = 0; i < mvHwsSerdesGetMaxLane(); i++) {
				if (i != 1 && serdesMapArray[1].serdesType == serdesMapArray[i].serdesType) {
					mvPrintf("\tLane #%d Type conflicts with Lane #1 (Lane #%d disabled)\n", i, i);
					serdesMapArray[i] = DBSatRConfigLane1[0];
				}

				if (i != 2 && serdesMapArray[2].serdesType == serdesMapArray[i].serdesType) {
					mvPrintf("\tLane #%d Type conflicts with Lane #2 (Lane #%d disabled)\n", i, i);
					serdesMapArray[i] = DBSatRConfigLane1[0];
				}
			}
		}

		break; /* case DB_68XX_ID */
	case DB_GP_68XX_ID: /* read 'gpserdes1' & 'gpserdes2' */
		twsiSlave.offset = 2;
		if (mvTwsiRead(0, &twsiSlave, &configVal, 1) != MV_OK) {
			mvPrintf("%s: TWSI Read of 'gpserdes1/2' failed\n", __func__);
			return MV_FAIL;
		}

		/* Lane #1: laneSelect = 0 --> SATA0,  laneSelect = 1 --> PCIe0 (mini PCIe) */
		laneSelect = (configVal & SATR_GP_LANE1_CFG_MASK) >> SATR_GP_LANE1_CFG_OFFSET;
		if (laneSelect == 1) {
			serdesMapArray[1].serdesMode = PEX0;
			serdesMapArray[1].serdesSpeed = __5Gbps;
			serdesMapArray[1].serdesType = PEX_ROOT_COMPLEX_x1;
			/* if lane 1 is set to PCIe0 --> disable PCIe0 on lane 0 */
			serdesMapArray[0] = DefaultLane;
			isCustomTopology = MV_TRUE;	/* indicate user of non-default topology */
		}
		mvPrintf("Lane 1 detection: %s \n" ,laneSelect ? "PCIe0 (mini PCIe)" : "SATA0");

		/* Lane #2: laneSelect = 0 --> SATA1,  laneSelect = 1 --> PCIe1 (mini PCIe) */
		laneSelect = (configVal & SATR_GP_LANE2_CFG_MASK) >> SATR_GP_LANE2_CFG_OFFSET;
		if (laneSelect == 1) {
			serdesMapArray[2].serdesType = PEX1;
			serdesMapArray[2].serdesSpeed = __5Gbps;
			serdesMapArray[2].serdesMode = PEX_ROOT_COMPLEX_x1;
			isCustomTopology = MV_TRUE;	/* indicate user of non-default topology */
		}
		mvPrintf("Lane 2 detection: %s \n" ,laneSelect ? "PCIe1 (mini PCIe)" : "SATA1");
		break; /* case DB_GP_68XX_ID */
	}

	if (isCustomTopology)
		mvPrintf("\nDetected custom SerDes topology (to restore default run 'SatR write default')\n\n");

	return MV_OK;
}

/**************************************************************************
* mvHwsUpdateDeviceToplogy
* DESCRIPTION: Update the default board topology for specific device Id
* INPUT:
*   	topologyConfigPtr - pointer to the Serdes mapping
*   	topologyMode - topology mode (index)
* OUTPUT: None
* RRETURNS:
*   	MV_OK - if updating the board topology success
*   	MV_BAD_PARAM - if the input parameter is wrong
***************************************************************************/
MV_STATUS mvHwsUpdateDeviceToplogy(SERDES_MAP* topologyConfigPtr, TOPOLOGY_CONFIG_DB topologyMode)
{
	MV_U32 DevId = mvSysEnvDeviceIdGet();
	MV_U32 BoardId = mvBoardIdGet();

	if (BoardId == DB_BP_6821_ID) {
		switch (topologyMode) {
		case DB_381_CONFIG_DEFAULT:
			if (DevId != MV_6W22)
				return MV_OK;
			/* DB-88F6821-BP on 6W22: if lane1 is SATA0 or SGMII-1 or QSGMII set to default */
			/* --> device only supports 1 SATA, 1 SGMII and no QSGMII interface */
			if (topologyConfigPtr[1].serdesType == SGMII1) {
				mvPrintf("Device 6W22 supports only one SGMII interface: SGMII-1 @ lane1 disabled\n");
				topologyConfigPtr[1] = DefaultLane;
			} else if  (topologyConfigPtr[1].serdesType == QSGMII) {
				mvPrintf("Device 6W22 does not support QSGMII interface: QSGMII @ lane1 disabled\n");
				topologyConfigPtr[1] = DefaultLane;
			}
			/* DB-88F6821-BP on 6W22: if lane1 is SATA1 or SGMII-1 or QSGMII set to default */
			/* --> device only supports 1 SATA and 1 SGMII interface */
			if (topologyConfigPtr[2].serdesType == SATA1) {
				mvPrintf("Device 6W22 supports only one SATA ");
				mvPrintf("interface: SATA Port 1 @ lane2 disabled\n");
				topologyConfigPtr[2] = DefaultLane;
			} else if (topologyConfigPtr[2].serdesType == SGMII1) {
				mvPrintf("Device 6W22 supports only one SGMII ");
				mvPrintf("interface: SGMII-1 @ lane2 disabled\n");
				topologyConfigPtr[2] = DefaultLane;
			}
			/* DB-88F6821-BP: default for Lane3=USB3_HOST1 --> 6W22 supports only one USB3 Host*/
			/* --> device only supports 1 SATA, SGMII and no QSGMII interface */
			mvPrintf("Device 6W22 supports only one USB3 Host interface: USB3 Port 1 @ lane3 disabled\n");
			topologyConfigPtr[3] = DefaultLane;
			break;
		case DB_CONFIG_SLM1427:
			/* DB-88F6821-BP on 6W22: default for Lane2=SATA1,Lane3=USB3_HOST1 */
			/* --> device supports only 1 SATA and 1 USB3 interface */
			if (DevId == MV_6W22) {
				mvPrintf("Device 6W22 supports only one SATA ");
				mvPrintf("interface: SATA Port 1 @ lane2 disabled\n");
				topologyConfigPtr[2] = DefaultLane;
				mvPrintf("Device 6W22 supports only one USB3 ");
				mvPrintf("interface: USB3 Port 1 @ lane3 disabled\n");
				topologyConfigPtr[3] = DefaultLane;
			}
			break;
		case DB_CONFIG_SLM1426:
			/* DB-88F6821-BP on 6W22: default for Lane2=SATA1,Lane3=SGMII-2 */
			/* --> device supports only 1 SATA and 1 SGMII interface */
			if (DevId == MV_6W22) {
				mvPrintf("Device 6W22 supports only one SATA ");
				mvPrintf("interface: SATA Port 1 @ lane2 disabled\n");
				topologyConfigPtr[2] = DefaultLane;
				mvPrintf("Device 6W22 supports only one SGMII ");
				mvPrintf("interface: SGMII-2 @ lane3 disabled\n");
				topologyConfigPtr[3] = DefaultLane;
			}
			break;
		default:
			break;
		}
		return MV_OK;
	}
	switch(topologyMode) {
	case DB_CONFIG_DEFAULT:
	/* update for DB-GP, DB-AP, DB-AMC, And for DB with default topology */
		switch(DevId) {
		case MV_6810:
			/* DB-AP : default for Lane3=SGMII2 --> 6810 supports only 2 SGMII interfaces: lane 3 disabled */
			if (BoardId == DB_AP_68XX_ID) {
				mvPrintf("Device 6810 supports only 2 SGMII interfaces: SGMII-2 @ lane3 disabled\n");
				topologyConfigPtr[3] = DefaultLane;
			}

			/* 6810 has only 4 SerDes and the forth one is Serdes number 5 (i.e. Serdes 4 is not connected),
			   therefore we need to copy SerDes 5 configuration to SerDes 4 */
			mvPrintf("Device 6810 does not supports SerDes Lane #4: replaced topology entry with lane #5\n");
			topologyConfigPtr[4] = topologyConfigPtr[5];
		case MV_6820: /* no break between cases since the 1st 6820 limitation apply on 6810 */
			/* DB-GP & DB-BP: default for Lane3=SATA3 --> 6810/20 supports only 2 SATA interfaces: lane 3 disabled */
			if ((BoardId == DB_68XX_ID) || (BoardId == DB_GP_68XX_ID)) {
				mvPrintf("Device 6810/20 supports only 2 SATA interfaces: SATA Port 3 @ lane3 disabled\n");
				topologyConfigPtr[3] = DefaultLane;
			}
			/* DB-GP on 6820 only: default for Lane4=SATA2 --> 6820 supports only 2 SATA interfaces: lane 3 disabled */
			if (BoardId == DB_GP_68XX_ID && DevId == MV_6820) {
				mvPrintf("Device 6820 supports only 2 SATA interfaces: SATA Port 2 @ lane4 disabled\n");
				topologyConfigPtr[4] = DefaultLane;
			}
			break;
		case MV_6W23:
			if (BoardId == DB_GP_68XX_ID) {
				/* DB-GP on 6W23: if lane 2 changed to SATA1 by SatR change to default
				 * --> 6W23 supports only one SATA interface */
				if (topologyConfigPtr[2].serdesType == SATA1) {
					mvPrintf("Device 6W23 supports only one ");
					mvPrintf("SATA interface: SATA Port 1 @ lane2 disabled\n");
					topologyConfigPtr[2] = DefaultLane;
				}
				/* DB-GP on 6W23: default for Lane3=SATA3,Lane4=SATA2 -->
				 * 6W23 supports only 1 SATA interface */
				/* default for Lane5=USB3_HOST1 --> 6W23 support only 4 SerDes lanes */
				mvPrintf("Device 6W23 supports only one SATA interface: SATA ");
				mvPrintf("Port 3 @ lane3 disabled\n");
				topologyConfigPtr[3] = DefaultLane;
				mvPrintf("Device 6W23 supports only one SATA interface: SATA ");
				mvPrintf("Port 2 @ lane4 disabled\n");
				topologyConfigPtr[4] = DefaultLane;
				mvPrintf("Device 6W23 supports only 4 SerDes lanes: USB3-Host ");
				mvPrintf("Port 1 @ lane5 disabled\n");
				topologyConfigPtr[5] = DefaultLane;
			} else if (BoardId == DB_68XX_ID) {
				/* DB-BP on 6W23: if lane is SGMII-1 or QSGMII set to default */
				/* --> device only supports 1 SGMII and no QSGMII interface */
				if (topologyConfigPtr[1].serdesType == SGMII1) {
					mvPrintf("Device 6W23 supports only one SGMII ");
					mvPrintf("interface: SGMII-1 @ lane1 disabled\n");
					topologyConfigPtr[1] = DefaultLane;
				} else if  (topologyConfigPtr[1].serdesType == QSGMII) {
					mvPrintf("Device 6W23 does not support QSGMII ");
					mvPrintf("interface: QSGMII @ lane1 disabled\n");
					topologyConfigPtr[1] = DefaultLane;
				}
				/* DB-BP on 6W23: if lane1 is SATA1 or SGMII-1 or QSGMII set to default */
				/* --> device only supports 1 SATA and 1 SGMII interface */
				if (topologyConfigPtr[2].serdesType == SATA1) {
					mvPrintf("Device 6W23 supports only one SATA ");
					mvPrintf("interface: SATA Port 1 @ lane2 disabled\n");
					topologyConfigPtr[2] = DefaultLane;
				} else if (topologyConfigPtr[2].serdesType == SGMII1) {
					mvPrintf("Device 6W23 supports only one SGMII ");
					mvPrintf("interface: SGMII-1 @ lane2 disabled\n");
					topologyConfigPtr[2] = DefaultLane;
				}
				/* DB-GP on 6W23: default to Lane3=SATA3
				 * --> 6W23 supports only one SATA interface */
				mvPrintf("Device 6W23 supports only one ");
				mvPrintf("SATA interface: SATA Port 3 @ lane3 disabled\n");
				topologyConfigPtr[3] = DefaultLane;
				/* default for Lane4=USB3_HOST0,Lane5=USB3_HOST1
				 * --> 6W23 support only 4 SerDes lanes */
				mvPrintf("Device 6W23 supports only 4 SerDes lanes: USB3-Host ");
				mvPrintf("Port 0 @ lane4 disabled\n");
				topologyConfigPtr[4] = DefaultLane;
				mvPrintf("Device 6W23 supports only 4 SerDes lanes: USB3-Host ");
				mvPrintf("Port 1 @ lane5 disabled\n");
				topologyConfigPtr[5] = DefaultLane;
			} else if (BoardId == DB_AP_68XX_ID) {
				/* DB-AP on 6W23: default for Lane1=SGMII-1, Lane3=SGMII-2 */
				/* --> device only supports 1 SGMII interface */
				mvPrintf("Device 6W23 supports only one SGMII interface: SGMII-1 @ lane1 disabled\n");
				topologyConfigPtr[1] = DefaultLane;
				mvPrintf("Device 6W23 supports only one SGMII interface: SGMII-2 @ lane3 disabled\n");
				topologyConfigPtr[3] = DefaultLane;
				/* default for Lane4=USB3_HOST0,Lane5=PEX2
				 * --> 6W23 support only 4 SerDes lanes */
				mvPrintf("Device 6W23 supports only 4 SerDes lanes: ");
				mvPrintf("USB3-Host Port 0 @ lane4 disabled\n");
				topologyConfigPtr[4] = DefaultLane;
				mvPrintf("Device 6W23 supports only 4 SerDes lanes:  Port 1 @ lane5 disabled\n");
				topologyConfigPtr[5] = DefaultLane;
			}
		default:
			break;
		}
		break;
	default:
		mvPrintf("mvSysEnvUpdateDeviceToplogy: selected topology is not supported by this routine\n");
		break;
	}

	return MV_OK;
}

MV_STATUS loadTopologyDB381(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	MV_U8 topologyMode;
	SERDES_MAP* topologyConfigPtr;
	MV_U8 twsiData;
	MV_U8 usb3Host0OrDevice = 0, usb3Host1OrDevice = 0;

	mvPrintf("\nInitialize DB-88F6821-BP board topology\n");

	/* Getting the relevant topology mode (index) */
	topologyMode = topologyConfigDB381ModeGet();
	topologyConfigPtr = topologyConfigDB381[topologyMode];
	/* if not detected any SerDes Site module, read 'SatR' lane setup */
	if (topologyMode == DB_381_CONFIG_DEFAULT)
		updateTopologySatR(topologyConfigPtr);
	/* Update the default board topology device flavours */
	CHECK_STATUS(mvHwsUpdateDeviceToplogy(topologyConfigPtr, topologyMode));
	/* Read USB3.0 mode: HOST/DEVICE */
	if (loadTopologyUSBModeGet(&twsiData) == MV_OK) {
		usb3Host0OrDevice = (twsiData & 0x1);
		if (usb3Host0OrDevice == 0) /* Only one USB3 device is enabled */
			usb3Host1OrDevice = ((twsiData >> 1) & 0x1);
	}
	/* Updating the topology map */
	for (laneNum = 0; laneNum < mvHwsSerdesGetMaxLane(); laneNum++) {
		serdesMapArray[laneNum].serdesMode  = topologyConfigPtr[laneNum].serdesMode;
		serdesMapArray[laneNum].serdesSpeed = topologyConfigPtr[laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType  = topologyConfigPtr[laneNum].serdesType;
		serdesMapArray[laneNum].swapRx      = topologyConfigPtr[laneNum].swapRx;
		serdesMapArray[laneNum].swapTx      = topologyConfigPtr[laneNum].swapTx;
		/* Update USB3 device if needed*/
		if (usb3Host0OrDevice == 1 && serdesMapArray[laneNum].serdesType == USB3_HOST0) {
			if (mvSysEnvDeviceIdGet() != MV_6W22) {
				serdesMapArray[laneNum].serdesType = USB3_DEVICE;
			} else {
				/* Device 6W22: Disable USB3-->device doesn't support device mode */
				mvPrintf("Device 6W22 supports only USB3 Host ");
				mvPrintf("mode: USB3 Port 0 @ lane%u disabled\n", laneNum);
				serdesMapArray[laneNum] = DefaultLane;
			}
		}

		if (usb3Host1OrDevice == 1 && serdesMapArray[laneNum].serdesType == USB3_HOST1)
			serdesMapArray[laneNum].serdesType = USB3_DEVICE;
	}

	/* update 'sgmiispeed' settings */
	updateTopologySgmiiSpeed(serdesMapArray);

	return MV_OK;
}

/***************************************************************************/
MV_STATUS loadTopologyDB(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	MV_U8 topologyMode;
	SERDES_MAP* topologyConfigPtr;
	MV_U8 twsiData;
	MV_U8 usb3Host0OrDevice = 0, usb3Host1OrDevice = 0;

	mvPrintf("\nInitialize DB-88F6820-BP board topology\n");

	/* Getting the relevant topology mode (index) */
	topologyMode = topologyConfigDBModeGet();

	if (topologyMode == DB_NO_TOPOLOGY)
		topologyMode = DB_CONFIG_DEFAULT;

	topologyConfigPtr = topologyConfigDB[topologyMode];
	/* if not detected any SerDes Site module, read 'SatR' lane setup */
	if (topologyMode == DB_CONFIG_DEFAULT)
		updateTopologySatR(topologyConfigPtr);
	/* Update the default board topology device flavours */
	CHECK_STATUS(mvHwsUpdateDeviceToplogy(topologyConfigPtr, topologyMode));

	/* Read USB3.0 mode: HOST/DEVICE */
	if (loadTopologyUSBModeGet(&twsiData) == MV_OK){
		usb3Host0OrDevice = (twsiData & 0x1);
		if (usb3Host0OrDevice == 0) /* Only one USB3 device is enabled */
			usb3Host1OrDevice = ((twsiData >> 1) & 0x1);
	}

	/* Updating the topology map */
	for (laneNum = 0; laneNum < mvHwsSerdesGetMaxLane(); laneNum++) {
		serdesMapArray[laneNum].serdesMode  = topologyConfigPtr[laneNum].serdesMode;
		serdesMapArray[laneNum].serdesSpeed = topologyConfigPtr[laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType  = topologyConfigPtr[laneNum].serdesType;
		serdesMapArray[laneNum].swapRx      = topologyConfigPtr[laneNum].swapRx;
		serdesMapArray[laneNum].swapTx      = topologyConfigPtr[laneNum].swapTx;

		/* Update USB3 device if needed */
		if ((serdesMapArray[laneNum].serdesType == USB3_HOST0) &&
			(usb3Host0OrDevice == 1)) {
			if (mvSysEnvDeviceIdGet() != MV_6W23) {
				serdesMapArray[laneNum].serdesType = USB3_DEVICE;
			} else {
				/* Device 6W23: Disable USB3-->device doesn't support device mode */
				mvPrintf("Device 6W23 supports only USB3 Host ");
				mvPrintf("mode: USB3 Port 0 @ lane%u disabled\n", laneNum);
				serdesMapArray[laneNum] = DefaultLane;
			}
		}

		if ((serdesMapArray[laneNum].serdesType == USB3_HOST1) &&
			(usb3Host1OrDevice == 1))
				serdesMapArray[laneNum].serdesType = USB3_DEVICE;
	}

	/* update 'sgmiispeed' settings */
	updateTopologySgmiiSpeed(serdesMapArray);

	return MV_OK;
}

MV_STATUS loadTopologyDBAp(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	SERDES_MAP* topologyConfigPtr;

	DEBUG_INIT_FULL_S("\n### loadTopologyDBAp ###\n");

	mvPrintf("\nInitialize DB-AP board topology\n");
	topologyConfigPtr = DbApConfigDefault;

	/* Update the default board topology device flavours */
	CHECK_STATUS(mvHwsUpdateDeviceToplogy(topologyConfigPtr, DB_CONFIG_DEFAULT));

	/* Updating the topology map */
	for (laneNum = 0; laneNum < mvHwsSerdesGetMaxLane(); laneNum++) {
		serdesMapArray[laneNum].serdesMode  = topologyConfigPtr[laneNum].serdesMode;
		serdesMapArray[laneNum].serdesSpeed = topologyConfigPtr[laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType  = topologyConfigPtr[laneNum].serdesType;
		serdesMapArray[laneNum].swapRx      = topologyConfigPtr[laneNum].swapRx;
		serdesMapArray[laneNum].swapTx      = topologyConfigPtr[laneNum].swapTx;
	}

	updateTopologySgmiiSpeed(serdesMapArray);

	return MV_OK;
}

MV_STATUS loadTopologyDBGp(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	SERDES_MAP* topologyConfigPtr;
	MV_BOOL isSgmii = MV_FALSE;

	DEBUG_INIT_FULL_S("\n### loadTopologyDBGp ###\n");

	topologyConfigPtr = DbGpConfigDefault;

	mvPrintf("\nInitialize DB-GP board topology\n");

	/* check S@R: if lane 5 is USB3 or SGMII */
	if (loadTopologyRDSgmiiUsb(&isSgmii) != MV_OK)
		mvPrintf("%s: TWSI Read failed - Loading Default Topology\n", __func__);
	else {
		topologyConfigPtr[5].serdesType  =  isSgmii ? SGMII2 : USB3_HOST1;
		topologyConfigPtr[5].serdesSpeed =  isSgmii ? __3_125Gbps : __5Gbps;;
		topologyConfigPtr[5].serdesMode = SERDES_DEFAULT_MODE;
	}
	/* update 'gpserdes1/2' lane configuration */
	updateTopologySatR(topologyConfigPtr);
	/* Update the default board topology device flavours */
	CHECK_STATUS(mvHwsUpdateDeviceToplogy(topologyConfigPtr, DB_CONFIG_DEFAULT));

	/* Updating the topology map */
	for (laneNum = 0; laneNum < mvHwsSerdesGetMaxLane(); laneNum++) {
		serdesMapArray[laneNum].serdesMode =  topologyConfigPtr[laneNum].serdesMode;
		serdesMapArray[laneNum].serdesSpeed =  topologyConfigPtr[laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType =  topologyConfigPtr[laneNum].serdesType;
		serdesMapArray[laneNum].swapRx      = topologyConfigPtr[laneNum].swapRx;
		serdesMapArray[laneNum].swapTx      = topologyConfigPtr[laneNum].swapTx;
	}

	/* update 'sgmiispeed' for SGMII lanes */
	updateTopologySgmiiSpeed(serdesMapArray);

	return MV_OK;
}

MV_STATUS loadTopologyDBAMC(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	SERDES_MAP* topologyConfigPtr;

	DEBUG_INIT_FULL_S("\n### loadTopologyDBAMC ###\n");

	mvPrintf("\nInitialize DB-AMC board topology\n");
	topologyConfigPtr = DbAmcConfigDefault;

	/* Update the default board topology device flavours */
	CHECK_STATUS(mvHwsUpdateDeviceToplogy(topologyConfigPtr, DB_CONFIG_DEFAULT));

	/* Updating the topology map */
	for (laneNum = 0; laneNum < mvHwsSerdesGetMaxLane(); laneNum++) {
		serdesMapArray[laneNum].serdesMode  = topologyConfigPtr[laneNum].serdesMode;
		serdesMapArray[laneNum].serdesSpeed = topologyConfigPtr[laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType  = topologyConfigPtr[laneNum].serdesType;
		serdesMapArray[laneNum].swapRx      = topologyConfigPtr[laneNum].swapRx;
		serdesMapArray[laneNum].swapTx      = topologyConfigPtr[laneNum].swapTx;
	}

	updateTopologySgmiiSpeed(serdesMapArray);

	return MV_OK;
}


/************************** Load topology - Marvell RD boards********************************/

/***************************************************************************/
MV_STATUS loadTopologyRD(SERDES_MAP  *serdesMapArray)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 mode;

	DEBUG_INIT_FULL_S("\n### loadTopologyRD ###\n");

	DEBUG_INIT_S("\nInit RD board ");

	/* Initializing twsiSlave in order to read from the TWSI address */
	twsiSlave.slaveAddr.address = EEPROM_I2C_ADDR;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;

	/* Reading mode */
	DEBUG_INIT_FULL_S("loadTopologyRD: getting mode\n");
	if (mvTwsiRead(0, &twsiSlave, &mode, 1) != MV_OK) {
		DEBUG_INIT_S("loadTopologyRD: TWSI Read failed\n");
		return MV_FAIL;
	}

	/* Updating the topology map */
	DEBUG_INIT_FULL_S("loadTopologyRD: Loading board topology details\n");

	/* RD mode: 0 = NAS, 1 = AP */
	if (((mode >> 1) & 0x1) == 0) {
		CHECK_STATUS(loadTopologyRDNas(serdesMapArray));

	}
	else {
		CHECK_STATUS(loadTopologyRDAp(serdesMapArray));
	}

	updateTopologySgmiiSpeed(serdesMapArray);

	return MV_OK;
}

/***************************************************************************/
MV_STATUS loadTopologyRDNas(SERDES_MAP  *serdesMapArray)
{
	MV_BOOL isSgmii = MV_FALSE;
	MV_U32 i;

	DEBUG_INIT_S("\nInit RD NAS topology ");

	/* check if lane 4 is USB3 or SGMII */
	if (loadTopologyRDSgmiiUsb(&isSgmii) != MV_OK) {
		DEBUG_INIT_S("loadTopologyRD NAS: TWSI Read failed\n");
		return MV_FAIL;
	}

	/* Lane 0 */
	serdesMapArray[0].serdesType  = PEX0;
	serdesMapArray[0].serdesSpeed = __5Gbps;
	serdesMapArray[0].serdesMode  = PEX_ROOT_COMPLEX_x1;

	/* Lane 1 */
	serdesMapArray[1].serdesType = SATA0;
	serdesMapArray[1].serdesSpeed = __3Gbps;
	serdesMapArray[1].serdesMode = SERDES_DEFAULT_MODE;

	/* Lane 2 */
	serdesMapArray[2].serdesType = SATA1;
	serdesMapArray[2].serdesSpeed = __3Gbps;
	serdesMapArray[2].serdesMode = SERDES_DEFAULT_MODE;

	/* Lane 3 */
	serdesMapArray[3].serdesType =  SATA3;
	serdesMapArray[3].serdesSpeed = __3Gbps;
	serdesMapArray[3].serdesMode =  SERDES_DEFAULT_MODE;

	/* Lane 4 */
	if (isSgmii == MV_TRUE) {
		DEBUG_INIT_S("Serdes Lane 4 is SGMII\n");
		serdesMapArray[4].serdesType = SGMII1;
		serdesMapArray[4].serdesSpeed = __3_125Gbps;
		serdesMapArray[4].serdesMode = SERDES_DEFAULT_MODE;
	}
	else {
		DEBUG_INIT_S("Serdes Lane 4 is USB3\n");
		serdesMapArray[4].serdesType = USB3_HOST0;
		serdesMapArray[4].serdesSpeed = __5Gbps;
		serdesMapArray[4].serdesMode = SERDES_DEFAULT_MODE;
	}

	/* Lane 5 */
	serdesMapArray[5].serdesType =  SATA2;
	serdesMapArray[5].serdesSpeed = __3Gbps;
	serdesMapArray[5].serdesMode =  SERDES_DEFAULT_MODE;

	/* init swap configuration*/
	for(i = 0; i <= 5; i++)
	{
		serdesMapArray[i].swapRx = MV_FALSE;
		serdesMapArray[i].swapTx = MV_FALSE;
	}

	return MV_OK;
}


/***************************************************************************/
MV_STATUS loadTopologyRDAp(SERDES_MAP  *serdesMapArray)
{
	MV_BOOL isSgmii = MV_FALSE;
	MV_U32 i;

	DEBUG_INIT_S("\nInit RD AP topology ");

	/* check if lane 4 is USB3 or SGMII */
	if (loadTopologyRDSgmiiUsb(&isSgmii) != MV_OK) {
		DEBUG_INIT_S("loadTopologyRD AP: TWSI Read failed\n");
		return MV_FAIL;
	}

	/* Lane 0 */
	serdesMapArray[0].serdesType = DEFAULT_SERDES;
	serdesMapArray[0].serdesSpeed = LAST_SERDES_SPEED;
	serdesMapArray[0].serdesMode = SERDES_DEFAULT_MODE;

	/* Lane 1 */
	serdesMapArray[1].serdesType = PEX0;
	serdesMapArray[1].serdesSpeed = __5Gbps;
	serdesMapArray[1].serdesMode = PEX_ROOT_COMPLEX_x1;

	/* Lane 2 */
	serdesMapArray[2].serdesType = PEX1;
	serdesMapArray[2].serdesSpeed = __5Gbps;
	serdesMapArray[2].serdesMode = PEX_ROOT_COMPLEX_x1;

	/* Lane 3 */
	serdesMapArray[3].serdesType =  SATA3;
	serdesMapArray[3].serdesSpeed = __3Gbps;
	serdesMapArray[3].serdesMode =  SERDES_DEFAULT_MODE;

	/* Lane 4 */
	if (isSgmii == MV_TRUE) {
		DEBUG_INIT_S("Serdes Lane 4 is SGMII\n");
		serdesMapArray[4].serdesType = SGMII1;
		serdesMapArray[4].serdesSpeed = __3_125Gbps;
		serdesMapArray[4].serdesMode = SERDES_DEFAULT_MODE;
	}
	else {
		DEBUG_INIT_S("Serdes Lane 4 is USB3\n");
		serdesMapArray[4].serdesType = USB3_HOST0;
		serdesMapArray[4].serdesSpeed = __5Gbps;
		serdesMapArray[4].serdesMode = SERDES_DEFAULT_MODE;
	}

	/* Lane 5 */
	serdesMapArray[5].serdesType =  SATA2;
	serdesMapArray[5].serdesSpeed = __3Gbps;
	serdesMapArray[5].serdesMode =  SERDES_DEFAULT_MODE;

	/* init swap configuration*/
	for(i = 0; i <= 5; i++)
	{
		serdesMapArray[i].swapRx = MV_FALSE;
		serdesMapArray[i].swapTx = MV_FALSE;
	}

	return MV_OK;
}

/***************************************************************************/
MV_STATUS loadTopologyRDSgmiiUsb(MV_BOOL *isSgmii)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 mode;

	/* DB-GP board: Device 6810 supports only 2 GbE ports: SGMII2 not supported (USE USB3 Host instead) */
	if (mvSysEnvDeviceIdGet() == MV_6810) {
		mvPrintf("Device 6810 supports only 2 GbE ports: SGMII-2 @ lane5 disabled (setting USB3.0 H1 instead)\n");
		*isSgmii = MV_FALSE;
		return MV_OK;
	}

	/* Initializing twsiSlave in order to read from the TWSI address */
	twsiSlave.slaveAddr.address = RD_GET_MODE_ADDR;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 1;
	twsiSlave.moreThen256 = MV_TRUE;

	/* check if lane 4 is USB3 or SGMII */
	if (mvTwsiRead(0, &twsiSlave, &mode, 1) == MV_OK) {
		*isSgmii = ((mode >> 2) & 0x1);
	}
	else
	{
		/* else use the default - USB3 */
		*isSgmii = MV_FALSE;
	}
	if (*isSgmii)
		isCustomTopology = MV_TRUE;

	mvPrintf("Lane 5 detection: %s \n" ,*isSgmii ? "SGMII2" : "USB3.0 Host Port 1");

	return MV_OK;
}
/* 'usb3port0'/'usb3port1' fields are located in EEPROM, at 3rd byte(offset=2), bit 0:1 (respectively) */
MV_STATUS loadTopologyUSBModeGet(MV_U8 *twsiData)
{
	MV_TWSI_SLAVE twsiSlave;

	/* Read SatR 'usb3port0' & 'usb3port1' values */
	twsiSlave.slaveAddr.address = EEPROM_I2C_ADDR;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 2;
	twsiSlave.moreThen256 = MV_TRUE;
	if (mvTwsiRead(0, &twsiSlave, twsiData, 1) == MV_OK)
		return MV_OK;

	return MV_ERROR;
}

#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */
/***************************************************************************/
