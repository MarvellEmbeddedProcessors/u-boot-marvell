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
#include "config_marvell.h"  	/* Required to identify SOC and Board */
#include "mvHighSpeedEnvSpec.h"
#include "mvBHboardEnvSpec.h"
#include "mvCtrlPex.h"

#if defined(MV_MSYS_BC2)
#include "ddr3_msys_bc2.h"
#include "ddr3_msys_bc2_config.h"
#elif defined(MV_MSYS_AC3)
#include "ddr3_msys_ac3.h"
#include "ddr3_msys_ac3_config.h"
#endif

#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"

/**************************************************************************
 * boardTopologyLoad -
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
MV_STATUS boardTopologyLoad(SERDES_MAP  *serdesMapArray);


/***************************************************************************/
MV_U8 mvHwsBoardIdGet(MV_VOID)
{
	return 0;
}

#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT

/************************* Load Topology - Customer Boards ****************************/
SERDES_MAP CustomerBoardTopologyConfig[][MAX_SERDES_LANES] =
{
{	/* Customer Board 0 Toplogy */
	/* Type		Serdes		Speed/			Mode	*/
	/*			Number		index					*/
	{ PEX0,		0,				0,		SERDES_DEFAULT_MODE },
	{ SGMII0,	10,		__1_25Gbps,		SERDES_DEFAULT_MODE },
	{ SGMII1,	11,		__1_25Gbps,		SERDES_DEFAULT_MODE },
},
{	/* Customer Board 1 Toplogy */
	/* Type		Serdes		Speed/			Mode	*/
	/*			Number		index					*/
	{ PEX0,		0,				0,		SERDES_DEFAULT_MODE },
	{ SGMII0,	10,		__1_25Gbps,		SERDES_DEFAULT_MODE },
	{ SGMII1,	11,		__1_25Gbps,		SERDES_DEFAULT_MODE },
}};


/**********************************************************************/
/* Load topology functions - Board ID is the index for function array */
/**********************************************************************/

loadTopologyFuncPtr loadTopologyFuncArr[] =
{
	boardTopologyLoad,         /* Customer Board 0 */
	boardTopologyLoad,         /* Customer Board 1*/
};

#else /* CONFIG_CUSTOMER_BOARD_SUPPORT */

/*********************************** Enums ************************************/


/************************* Local functions declarations ***********************/

loadTopologyFuncPtr loadTopologyFuncArr[] =
{
	boardTopologyLoad,
	boardTopologyLoad,
};

/*********************************** Globals **********************************/
/********************************/
/** Load topology - Marvell DB/RD **/
/********************************/

/* Configuration options */
SERDES_MAP serdesDbTopology[MAX_SERDES_LANES] =
{
	/* Type		Serdes		Speed			Mode	*/
	/*			Number							*/
	{ PEX0,		0,				0,		SERDES_DEFAULT_MODE },
	{ SGMII0,	10,		__1_25Gbps,		SERDES_DEFAULT_MODE },
	{ SGMII1,	11,		__1_25Gbps,		SERDES_DEFAULT_MODE },
};

SERDES_MAP serdesRdTopology[MAX_SERDES_LANES] =
{
	/* Type		Serdes		Speed			Mode	*/
	/*			Number							*/
	{ PEX0,		0,				0,		SERDES_DEFAULT_MODE },
	{ SGMII0,	10,		__1_25Gbps,		SERDES_DEFAULT_MODE },
	{ SGMII1,	11,		__1_25Gbps,		SERDES_DEFAULT_MODE },
};

SERDES_MAP* marvellBoardSerdesTopology[] =
{
	serdesDbTopology,
	serdesRdTopology,
};

/*************************** Functions implementation *************************/


#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */

/**************************************************************************
 * boardTopologyLoad -
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
MV_STATUS boardTopologyLoad(SERDES_MAP  *serdesMapArray)
{
	MV_U32		laneNum;
	MV_U32		boardId;
	SERDES_MAP	*topologyConfigPtr;

	DEBUG_INIT_FULL_S("\n### loadTopologyDB ###\n");

	boardId = mvHwsBoardIdGet();
#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
	topologyConfigPtr = CustomerBoardTopologyConfig[boardId];
#else
	topologyConfigPtr = marvellBoardSerdesTopology[boardId];
#endif
	/* Updating the topology map */
	for (laneNum = 0; laneNum < MAX_SERDES_LANES; laneNum++) {
		if (laneNum == topologyConfigPtr[laneNum].serdesNum) {
			serdesMapArray[laneNum].serdesMode  =  topologyConfigPtr[laneNum].serdesMode;
			serdesMapArray[laneNum].serdesNum   =  topologyConfigPtr[laneNum].serdesNum;
			serdesMapArray[laneNum].serdesSpeed =  topologyConfigPtr[laneNum].serdesSpeed;
			serdesMapArray[laneNum].serdesType  =  topologyConfigPtr[laneNum].serdesType;
		} else {
			serdesMapArray[laneNum].serdesNum  = laneNum;
			serdesMapArray[laneNum].serdesType = DEFAULT_SERDES;
		}
	}

	return MV_OK;
}

//Placeholder for future code
