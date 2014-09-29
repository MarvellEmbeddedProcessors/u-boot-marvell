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
#include "mvHighSpeedEnvSpec.h"
#include "mvSysEnvLib.h"
#include "printf.h"

#define	SERDES_VERION	"2.0"

/************************* Globals ********************************************/

MV_U8 commonPhysSelectorsSerdesRev1Map[LAST_SERDES_TYPE][MAX_SERDES_LANES] =
{
	/* 0     1       2       3       4       5 */
	{ 0x1,  0x1,    NA,	    NA,	    NA,	    NA	  },    /* PEX0 */
	{ NA,   0x2,    0x1,	NA,	    0x1,	NA	  },    /* PEX1 */
	{ NA,   NA,     0x2,	NA,	    NA,	    0x1	  },    /* PEX2 */
	{ NA,   NA,     NA,	    0x1,    NA,	    NA	  },    /* PEX3 */
	{ 0x2,  0x3,    NA,	    NA,	    NA,	    NA	  },    /* SATA0 */
	{ NA,   NA,     0x3,	NA,	    0x2,	NA	  },    /* SATA1 */
	{ NA,   NA,     NA,	    NA,	    0x6,	0x2	  },    /* SATA2 */
	{ NA,   NA,     NA,	    0x3,    NA,	    NA	  },    /* SATA3 */
	{ 0x3,  0x4,    NA,	    NA,	    NA,	    NA	  },    /* SGMII0 */
	{ NA,   0x5,    0x4,	NA,	    0x3,	NA	  },    /* SGMII1 */
	{ NA,   NA,     NA,	    0x4,    NA,	    0x3	  },    /* SGMII2 */
	{ NA,   0X7,    NA,	    NA,	    NA,	    NA	  },    /* QSGMII */
	{ NA,   0x6,    NA,	    NA,	    0x4,	NA	  },    /* USB3_HOST0 */
	{ NA,   NA,     NA,	    0x5,    NA,	    0x4	  },    /* USB3_HOST1 */
	{ NA,   NA,     NA,	    0x6,    0x5,	0x5	  },    /* USB3_DEVICE */
	{ 0x0,  0x0,    0x0,	0x0,    0x0,	0x0	  }     /* DEFAULT_SERDES */
};

/* Serdes type to ref clock map */
REF_CLOCK serdesTypeToRefClock38xMap[LAST_SERDES_TYPE] =
{
	REF_CLOCK__100MHz,      /* PEX0 */
	REF_CLOCK__100MHz,      /* PEX1 */
	REF_CLOCK__100MHz,      /* PEX2 */
	REF_CLOCK__100MHz,      /* PEX3 */
	REF_CLOCK__25MHz,       /* SATA0 */
	REF_CLOCK__25MHz,       /* SATA1 */
	REF_CLOCK__25MHz,       /* SATA2 */
	REF_CLOCK__25MHz,       /* SATA3 */
	REF_CLOCK__25MHz,       /* SGMII0 */
	REF_CLOCK__25MHz,       /* SGMII1 */
	REF_CLOCK__25MHz,       /* SGMII2 */
	REF_CLOCK__25MHz,       /* QSGMII */
	REF_CLOCK__25MHz,      	/* USB3_HOST0 */
	REF_CLOCK__25MHz,      	/* USB3_HOST1 */
	REF_CLOCK__25MHz,       /* USB3_DEVICE */
	REF_CLOCK_UNSUPPORTED   /* DEFAULT_SERDES */
};

/************************* Local functions declarations ***********************/

MV_STATUS mvHwsSerdesSeqInit(MV_VOID)
{
	DEBUG_INIT_FULL_S("\n### serdesSeqInit ###\n");

	if (mvHwsSerdesSeqDbInit() != MV_OK){
		mvPrintf("mvHwsSerdesSeqInit: Error: Serdes initialization fail\n");
		return MV_FAIL;
	}

	return MV_OK;
}

/***************************************************************************/
MV_STATUS mvSerdesPowerUpCtrlExt
(
	MV_U32 serdesNum,
	MV_BOOL serdesPowerUp,
	SERDES_TYPE serdesType,
	SERDES_SPEED baudRate,
	SERDES_MODE  serdesMode,
	REF_CLOCK refClock
)
{
	return MV_NOT_SUPPORTED;
}

/***************************************************************************/
MV_U32 mvHwsSerdesGetRefClockVal
(
	SERDES_TYPE serdesType
)
{
    return serdesTypeToRefClock38xMap[serdesType];
}

/***************************************************************************/
MV_U32 mvHwsSerdesGetMaxLane
(
	MV_VOID
)
{
    return 6;
}

/***************************************************************************/
MV_STATUS mvHwsGetExtBaseAddr
(
	MV_U32 serdesNum,
	MV_U32 baseAddr,
	MV_U32 unitBaseOffset,
	MV_U32 *unitBaseReg,
	MV_U32 *unitOffset
)
{
	*unitBaseReg = baseAddr;
	*unitOffset  = unitBaseOffset;

	return MV_OK;
}



