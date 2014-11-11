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
#include "config_marvell.h"
#include "printf.h"

#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT

/************************* Load Topology - Customer Boards ****************************/

/* Customer Board 0 Topology */
SERDES_MAP customerBoard0Topology[MAX_SERDES_LANES] =
{
	/* Type		Serdes		Speed			Mode				Swap		Swap */
	/*			Number											RX			TX   */
	{ PEX0,		0,				0,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII0,	10,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII1,	11,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
};

/* Customer Board 1 Topology */
SERDES_MAP customerBoard1Topology[MAX_SERDES_LANES] =
{
	/* Type		Serdes		Speed			Mode				Swap		Swap */
	/*			Number											RX			TX   */
	{ PEX0,		0,				0,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII0,	10,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII1,	11,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
};

/* Configuration options */
SERDES_MAP* customerAc3BoardSerdesTopology[] =
{
	customerBoard0Topology, /* Board 0 */
	customerBoard1Topology, /* Board 1 */
};

#else /* CONFIG_CUSTOMER_BOARD_SUPPORT */

/****************************************/
/*  Marvell AC3 DB/RD Topology		    */
/****************************************/

/* Configuration options */
SERDES_MAP ac3SerdesDbTopology[MAX_SERDES_LANES] =
{ /* DB_MISL_24G4XG */
	/* Type		Serdes		Speed			Mode				SwapRX		SwapTX */
	{ PEX0,		0,			0,			SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII0,	10,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII1,	11,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
};

SERDES_MAP ac3SerdesRd48_4xg_Topology[MAX_SERDES_LANES] =
{ /* RD_MTL_48G4XG */
	/* Type		Serdes		Speed			Mode				SwapRX		SwapTX */
	{ PEX0,		0,			0,			SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII0,	10,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII1,	11,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_TRUE,	MV_FALSE },
};

SERDES_MAP ac3SerdesRd48_2xxg_2xg_Topology[MAX_SERDES_LANES] =
{ /* RD_MTL_48G_2XXG_2XG */
	/* Type		Serdes		Speed			Mode				SwapRX		SwapTX */
	{ PEX0,		0,			0,			SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII0,	10,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII1,	11,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_TRUE,	MV_FALSE },
};

SERDES_MAP ac3SerdesDb24_g46_Topology[MAX_SERDES_LANES] =
{ /* DB_MISL_24G46 */
	/* Type		Serdes		Speed			Mode				SwapRX		SwapTX */
	{ PEX0,		0,			0,			SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII0,	10,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
	{ SGMII1,	11,		__1_25Gbps,		SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
};

SERDES_MAP ac3SerdesRd24_Topology[MAX_SERDES_LANES] =
{ /* RD_MTL_24G - no build-in OOB ports */
	/* Type		Serdes		Speed			Mode				SwapRX		SwapTX */
	{ PEX0,		0,			0,			SERDES_DEFAULT_MODE,	MV_FALSE,	MV_FALSE },
};

SERDES_MAP* marvellBoardAc3SerdesTopology[] =
{
	ac3SerdesDbTopology,
	ac3SerdesRd48_4xg_Topology,
	ac3SerdesRd48_2xxg_2xg_Topology,
	ac3SerdesDb24_g46_Topology,
	ac3SerdesRd24_Topology,
};

#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */

/*************************** Functions implementation *************************/

SERDES_MAP* mvHwsSerdesTopologyGet(MV_U32 boardIdIndex)
{
	SERDES_MAP** topologyArr;
	MV_U32 arrSize;

#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
	topologyArr = customerAc3BoardSerdesTopology;
	arrSize = sizeof(customerAc3BoardSerdesTopology)/sizeof(SERDES_MAP*);
#else
	topologyArr = marvellBoardAc3SerdesTopology;
	arrSize = sizeof(marvellBoardAc3SerdesTopology)/sizeof(SERDES_MAP*);
#endif

	if (boardIdIndex >= arrSize)
	{
		mvPrintf("%s: Error: board index %d is not valid\n", __func__, boardIdIndex);
		return NULL;
	}

	return topologyArr[boardIdIndex];
}
