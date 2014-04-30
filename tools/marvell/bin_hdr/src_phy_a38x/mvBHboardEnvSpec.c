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
#include "mv_os.h"
#include "mvBHboardEnvSpec.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"

#ifdef WIN32
#define mvPrintf    printf
#endif

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
{		/* Customer Board 0 Toplogy */
	{ SATA0,	__3Gbps,	SERDES_DEFAULT_MODE },
	{ PEX0,		__5Gbps,	PEX_ROOT_COMPLEX_x1 },
	{ PEX1,		__5Gbps,	PEX_ROOT_COMPLEX_x1 },
	{ SATA3,	__3Gbps,	SERDES_DEFAULT_MODE },
	{ USB3_HOST0,	__5Gbps,	SERDES_DEFAULT_MODE },
	{ SATA2,	__3Gbps,	SERDES_DEFAULT_MODE }
},
{		/* Customer Board 1 Toplogy */
	{ PEX0,		__5Gbps,	PEX_ROOT_COMPLEX_x1 },
	{ SATA0,	__3Gbps,	SERDES_DEFAULT_MODE },
	{ SATA1,	__3Gbps,	SERDES_DEFAULT_MODE },
	{ SATA3,	__3Gbps,	SERDES_DEFAULT_MODE },
	{ USB3_HOST0,	__5Gbps,	SERDES_DEFAULT_MODE },
	{ SATA2,	__3Gbps,	SERDES_DEFAULT_MODE }
}};


/***************************************************************************/
MV_STATUS loadTopologyCustomer(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	MV_U32 boardId = mvBoardIdGet();
	DEBUG_INIT_S("\nInit Customer board ");

	/* Updating the topology map */
	for (laneNum = 0; laneNum < MAX_SERDES_LANES; laneNum++) {
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

/**************************************************************************
 * topologyConfigDBModeGet -
 *
 * DESCRIPTION:          Gets the relevant topology mode (index).
 *                       for loadTopologyDB use only.
 * INPUT:                None.
 * OUTPUT:               None.
 * RETURNS:              the topology mode
 ***************************************************************************/
/**************************************************************************
 * loadTopologyDBAp -
 *
 * DESCRIPTION:          Loads the board topology for the DB_A38X_-AP board
 * INPUT:                serdesMapArray  -   The struct that will contain
 *                                           the board topology map
 * OUTPUT:               The board topology map.
 * RETURNS:              MV_OK           -   for success
 *                       MV_FAIL         -   for failure (a wrong
 *                                           topology mode was read
 *                                           from the board)
 ***************************************************************************/
MV_STATUS loadTopologyDBAp(SERDES_MAP  *serdesMapArray);

MV_U8 topologyConfigDBModeGet(MV_VOID);

/**************************************************************************
 * loadTopologyRD -
 *
 * DESCRIPTION:          Loads the board topology from RD
 * INPUT:                serdesMapArray  -   The struct that will contain
 *                                           the board topology map
 * OUTPUT:               The board topology.
 * RETURNS:              MV_OK           -   for success
 ***************************************************************************/
MV_STATUS loadTopologyRD(SERDES_MAP  *serdesMapArray);

/**************************************************************************
 * loadTopologyRDNas -
 *
 * DESCRIPTION:          Loads the board topology for RD NAS board
 * INPUT:                serdesMapArray  -   The struct that will contain
 *                                           the board topology map
 * OUTPUT:               The board topology.
 * RETURNS:              MV_OK           -   for success
 ***************************************************************************/
MV_STATUS loadTopologyRDNas(SERDES_MAP  *serdesMapArray);

/**************************************************************************
 * loadTopologyRDAp -
 *
 * DESCRIPTION:          Loads the board topology for RD AP board
 * INPUT:                serdesMapArray  -   The struct that will contain
 *                                           the board topology map
 * OUTPUT:               The board topology.
 * RETURNS:              MV_OK           -   for success
 ***************************************************************************/
MV_STATUS loadTopologyRDAp(SERDES_MAP  *serdesMapArray);

/**************************************************************************
 * loadTopologyRDSgmiiUsb -
 *
 * DESCRIPTION:          For RD board check if lane 4 is USB3 or SGMII
 * INPUT:                None
 * OUTPUT:               isSgmii - return MV_TRUE if lane 4 is SGMII
 * 				   return MV_FALSE if lane 4 is USB.
 * RETURNS:              MV_OK           -   for success
 ***************************************************************************/
MV_STATUS loadTopologyRDSgmiiUsb(MV_BOOL *isSgmii);

loadTopologyFuncPtr loadTopologyFuncArr[] =
{
	loadTopologyRD,         /* RD NAS */
	loadTopologyDB,		/* DB BP */
	loadTopologyRD,         /* RD AP */
	loadTopologyDBAp,	/* DB AP */
};

/*********************************** Globals **********************************/
/*************************************/
/** Load topology - Marvell DB - BP **/
/*************************************/

/* Configuration options */
SERDES_MAP DbConfigDefault[MAX_SERDES_LANES] =
{
	{ SATA0,     __3Gbps,		   SERDES_DEFAULT_MODE		      },
	{ PEX0,	     __5Gbps,		   PEX_ROOT_COMPLEX_x1		      },
	{ PEX1,	     __5Gbps,		   PEX_ROOT_COMPLEX_x1		      },
	{ SATA3,     __3Gbps,		   SERDES_DEFAULT_MODE		      },
	{ USB3_HOST0, __5Gbps,		   SERDES_DEFAULT_MODE		      },
	{ USB3_HOST1, __5Gbps,		   SERDES_DEFAULT_MODE		      }
};

SERDES_MAP DbConfigSLM1363_C[MAX_SERDES_LANES] =
{
	{ PEX0,		  __5Gbps,		PEX_ROOT_COMPLEX_x1		   },
	{ DEFAULT_SERDES, LAST_SERDES_SPEED,	SERDES_DEFAULT_MODE		   },
	{ PEX1,		  __5Gbps,		PEX_ROOT_COMPLEX_x1		   },
	{ PEX3,		  __5Gbps,		PEX_ROOT_COMPLEX_x1		   },
	{ SATA1,	  __3Gbps,		SERDES_DEFAULT_MODE		   },
	{ SATA2,	  __3Gbps,		SERDES_DEFAULT_MODE		   }
};

SERDES_MAP DbConfigSLM1363_D[MAX_SERDES_LANES] =
{
	{ PEX0,		  __5Gbps,		PEX_ROOT_COMPLEX_x4		   },
	{ PEX1,		  __5Gbps,		PEX_ROOT_COMPLEX_x4		   },
	{ PEX2,		  __5Gbps,		PEX_ROOT_COMPLEX_x4		   },
	{ PEX3,		  __5Gbps,		PEX_ROOT_COMPLEX_x4		   },
	{ SATA1,	  __3Gbps,		SERDES_DEFAULT_MODE		   },
	{ SATA2,	  __3Gbps,		SERDES_DEFAULT_MODE		   }
};

SERDES_MAP DbConfigSLM1363_E[MAX_SERDES_LANES] =
{
	{ PEX0,		  __5Gbps,		PEX_ROOT_COMPLEX_x1		   },
	{ USB3_HOST0,	  __5Gbps,		SERDES_DEFAULT_MODE		   },
	{ SATA1,	  __3Gbps,		SERDES_DEFAULT_MODE		   },
	{ USB3_HOST1,	  __5Gbps,		SERDES_DEFAULT_MODE		   },
	{ DEFAULT_SERDES, LAST_SERDES_SPEED,	SERDES_DEFAULT_MODE		   },
	{ SATA2,	  __3Gbps,		SERDES_DEFAULT_MODE		   }
};

SERDES_MAP DbConfigSLM1363_F[MAX_SERDES_LANES] =
{
	{ PEX0,		  __5Gbps,		PEX_ROOT_COMPLEX_x1		   },
	{ DEFAULT_SERDES, LAST_SERDES_SPEED,	SERDES_DEFAULT_MODE		   },
	{ PEX1,		  __5Gbps,		PEX_ROOT_COMPLEX_x1		   },
	{ PEX3,		  __5Gbps,		PEX_ROOT_COMPLEX_x1		   },
	{ SATA1,	  __3Gbps,		SERDES_DEFAULT_MODE		   },
	{ USB3_HOST1,	  __5Gbps,		SERDES_DEFAULT_MODE		   }
};

SERDES_MAP DbConfigSLM1364_D[MAX_SERDES_LANES] =
{
	{ DEFAULT_SERDES, LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE		 },
	{ SGMII0,	  __3_125Gbps,		  SERDES_DEFAULT_MODE		 },
	{ DEFAULT_SERDES, LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE		 },
	{ DEFAULT_SERDES, LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE		 },
	{ SGMII1,	  __3_125Gbps,		  SERDES_DEFAULT_MODE		 },
	{ SGMII2,	  __3_125Gbps,		  SERDES_DEFAULT_MODE		 }
};

SERDES_MAP DbConfigSLM1364_E[MAX_SERDES_LANES] =
{
	{ SGMII0,	  __3_125Gbps,		  SERDES_DEFAULT_MODE		     },
	{ SGMII1,	  __3_125Gbps,		  SERDES_DEFAULT_MODE		     },
	{ DEFAULT_SERDES, LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE		     },
	{ SGMII2,	  __3_125Gbps,		  SERDES_DEFAULT_MODE		     },
	{ DEFAULT_SERDES, LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE		     },
	{ PEX2,		  __5Gbps,		  PEX_ROOT_COMPLEX_x1		     }
};

SERDES_MAP DbConfigSLM1364_F[MAX_SERDES_LANES] =
{
	{ SGMII0,	  __3_125Gbps,		  SERDES_DEFAULT_MODE		     },
	{ DEFAULT_SERDES, LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE		     },
	{ SGMII1,	  __3_125Gbps,		  SERDES_DEFAULT_MODE		     },
	{ SGMII2,	  __3_125Gbps,		  SERDES_DEFAULT_MODE		     },
	{ DEFAULT_SERDES, LAST_SERDES_SPEED,	  SERDES_DEFAULT_MODE		     },
	{ PEX2,		  __5Gbps,		  PEX_ROOT_COMPLEX_x1		     }
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
	{ PEX0,		__5Gbps,		PEX_ROOT_COMPLEX_x1	},
	{ SGMII1,	__3_125Gbps,		SERDES_DEFAULT_MODE	},
	{ PEX1,		__5Gbps,		PEX_ROOT_COMPLEX_x1	},
	{ SGMII2,	__3_125Gbps,		SERDES_DEFAULT_MODE	},
	{ USB3_HOST0,	__5Gbps,		SERDES_DEFAULT_MODE	},
	{ PEX2,		__5Gbps,		PEX_ROOT_COMPLEX_x1	}
};

/*************************** Functions implementation *************************/
/***************************************************************************/
MV_U8 mvHwsBoardIdGet(MV_VOID)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 boardId;

	DEBUG_INIT_FULL_S("\n### mvHwsBoardIdGet ###\n");

	twsiSlave.slaveAddr.address = BOARD_ID_GET_ADDR;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;

	/* Reading board id */
	DEBUG_INIT_FULL_S("mvHwsBoardIdGet: getting board id\n");
	if (mvTwsiRead(0, &twsiSlave, &boardId, 1) != MV_OK) {
		DEBUG_INIT_S("mvHwsBoardIdGet: TWSI Read failed\n");
		return (MV_U8)MV_INVALID_BOARD_ID;
	}

	DEBUG_INIT_FULL_S("boardId from HW = 0x");
	DEBUG_INIT_FULL_D(boardId, 2);
	DEBUG_INIT_FULL_S("\n");

	return boardId & 0x7; /* bits 0-2 */
}

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

	DEBUG_INIT_FULL_S("topologyConfigDBModeGet: getting mode\n");

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
		default:    /* not the right module */
			break;
		}
	}

	/* SLM1364 Module */
	twsiSlave.slaveAddr.address = DB_GET_MODE_SLM1364_ADDR;
	if (mvTwsiRead(0, &twsiSlave, &mode, 1) != MV_OK)
	{
		DEBUG_INIT_S("\nInit DB board default topology\n");
		return DB_NO_TOPOLOGY;
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

/***************************************************************************/
MV_STATUS loadTopologyDB(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	MV_U8 topologyMode;
	SERDES_MAP* topologyConfigPtr;
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 twsiData;
	MV_U8 usb3Host0OrDevice = 0, usb3Host1OrDevice = 0;

	DEBUG_INIT_FULL_S("\n### loadTopologyDB ###\n");

	/* Getting the relevant topology mode (index) */
	DEBUG_INIT_FULL_S("loadTopologyDB: calling topologyConfigDBModeGet\n");

	topologyMode = topologyConfigDBModeGet();

	if (topologyMode == DB_NO_TOPOLOGY)
		topologyMode = DB_CONFIG_DEFAULT;

	topologyConfigPtr = topologyConfigDB[topologyMode];

	/* Read SatR usb3port0 & usb3port1 */
	twsiSlave.slaveAddr.address = RD_GET_MODE_ADDR;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;
	if (mvTwsiRead(0, &twsiSlave, &twsiData, 1) == MV_OK) {
		usb3Host0OrDevice = (twsiData & 0x1);
		if (usb3Host0OrDevice == 0)
		{
			/* Only one USB3 device is enabled */
			usb3Host1OrDevice = ((twsiData >> 1) & 0x1);
		}
	}


	/* Updating the topology map */
	for (laneNum = 0; laneNum < MAX_SERDES_LANES; laneNum++) {
		serdesMapArray[laneNum].serdesMode =  topologyConfigPtr[laneNum].serdesMode;
		serdesMapArray[laneNum].serdesSpeed =  topologyConfigPtr[laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType =  topologyConfigPtr[laneNum].serdesType;

		/* Update USB3 device if needed - relvant for lane 3,4,5 only */
		if (laneNum >= 3)
		{
			if ((serdesMapArray[laneNum].serdesType == USB3_HOST0) &&
				(usb3Host0OrDevice == 1))
			{
				serdesMapArray[laneNum].serdesType = USB3_DEVICE;
			}

			if ((serdesMapArray[laneNum].serdesType == USB3_HOST1) &&
				(usb3Host1OrDevice == 1))
			{
				serdesMapArray[laneNum].serdesType = USB3_DEVICE;
			}
		}
	}

	return MV_OK;
}

MV_STATUS loadTopologyDBAp(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;
	SERDES_MAP* topologyConfigPtr;

	DEBUG_INIT_FULL_S("\n### loadTopologyDBAp ###\n");

	topologyConfigPtr = DbApConfigDefault;

	/* Updating the topology map */
	for (laneNum = 0; laneNum < MAX_SERDES_LANES; laneNum++) {
		serdesMapArray[laneNum].serdesMode =  topologyConfigPtr[laneNum].serdesMode;
		serdesMapArray[laneNum].serdesSpeed =  topologyConfigPtr[laneNum].serdesSpeed;
		serdesMapArray[laneNum].serdesType =  topologyConfigPtr[laneNum].serdesType;
	}

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
	twsiSlave.slaveAddr.address = BOARD_ID_GET_ADDR;
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


	return MV_OK;
}

/***************************************************************************/
MV_STATUS loadTopologyRDNas(SERDES_MAP  *serdesMapArray)
{
	MV_BOOL isSgmii = MV_FALSE;

	DEBUG_INIT_S("\nInit RD NAS topology ");

	/* check if lane 4 is USB3 or SGMII */

	if (loadTopologyRDSgmiiUsb(&isSgmii) != MV_OK) {
		DEBUG_INIT_S("loadTopologyRD NAS: TWSI Read failed\n");
		return MV_FAIL;
	}

	/* Lane 0 */
	serdesMapArray[0].serdesType = PEX0;
	serdesMapArray[0].serdesSpeed = __5Gbps;
	serdesMapArray[0].serdesMode = PEX_ROOT_COMPLEX_x1;

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

	return MV_OK;
}


/***************************************************************************/
MV_STATUS loadTopologyRDAp(SERDES_MAP  *serdesMapArray)
{
	MV_BOOL isSgmii = MV_FALSE;

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

	return MV_OK;
}

/***************************************************************************/
MV_STATUS loadTopologyRDSgmiiUsb(MV_BOOL *isSgmii)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 mode;

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

	return MV_OK;
}

#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */
/***************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
	MV_U32 value;

	value = MV_MEMIO_LE32_READ((INTER_REGS_BASE | DEVICE_SAMPLE_AT_RESET1_REG)) & (0x3 << 22);

	switch (value) {
	case 0:
		return MV_BOARD_TCLK_250MHZ;
	case (0x1 << 22):
		return MV_BOARD_TCLK_200MHZ;
	default:
		return MV_BOARD_TCLK_ERROR;
	}
}
