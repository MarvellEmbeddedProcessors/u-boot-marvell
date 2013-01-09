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

#define FILL_TWSI_SLAVE(slv, addr)				\
{								\
	slv.slaveAddr.address = addr;				\
	slv.slaveAddr.type = MV_BOARD_MODULES_ADDR_TYPE;	\
	slv.validOffset = MV_TRUE;				\
	slv.offset = 0;						\
	slv.moreThen256 = MV_FALSE;				\
}



typedef enum {
	SERDES_UNIT_UNCONNECTED	= 0x0,
	SERDES_UNIT_PEX		= 0x1,
	SERDES_UNIT_SATA	= 0x2,
	SERDES_UNIT_SGMII0	= 0x3,
	SERDES_UNIT_SGMII1	= 0x4,
	SERDES_UNIT_SGMII2	= 0x5,
	SERDES_UNIT_SGMII3	= 0x6,
	SERDES_UNIT_QSGMII	= 0x7,
	SERDES_UNIT_SETM        = 0x8,
	SERDES_LAST_UNIT
} MV_BIN_SERDES_UNIT_INDX;


typedef enum {
	PEX_BUS_DISABLED	= 0,
	PEX_BUS_MODE_X1		= 1,
	PEX_BUS_MODE_X4		= 2,
	PEX_BUS_MODE_X8		= 3
} MV_PEX_UNIT_CFG;


typedef enum _mvPexType {
          MV_PEX_ROOT_COMPLEX,    /* root complex device */
          MV_PEX_END_POINT        /* end point device */
} MV_PEX_TYPE;
typedef struct _boardSerdesChangeMphy {
	MV_BIN_SERDES_UNIT_INDX serdesType;
	MV_U32	serdesRegLowSpeed;
	MV_U32	serdesValueLowSpeed;
	MV_U32	serdesRegHiSpeed;
	MV_U32	serdesValueHiSpeed;
} MV_SERDES_CHANGE_M_PHY;

/* Configuration per SERDES line.
   Each nibble is MV_SERDES_LINE_TYPE */
typedef struct _boardSerdesConf {
	MV_PEX_TYPE pexType; /* MV_PEX_ROOT_COMPLEX MV_PEX_END_POINT */
	MV_U32	serdesLine0_3;	/* Lines 0 to 7 SERDES MUX one nibble per line */
	MV_BOOL enableModuleScan;
	MV_U32	pexMod[2];
	MV_U32	busSpeed;	/* Bus speed - one bit per SERDES line:
	Low speed (0)		High speed (1)
	PEX	2.5 G (10 bit)		5 G (20 bit)
	SATA	1.5 G			3 G
	SGMII 	1.25 Gbps		3.125 Gbps	*/
	MV_SERDES_CHANGE_M_PHY * serdesMphyChange;
} MV_BIN_SERDES_CFG;


#define BIN_SERDES_CFG {	\
	{0,  1,  2,  3}, /* Lane 0 */	\
	{0,  1, -1,  2}, /* Lane 1 */	\
	{0, -1,  1,  2}, /* Lane 2 */	\
	{0, -1,  1,  2}  /* Lane 3 */	\
}

typedef struct _boardTwsiInfo {
	MV_U8 twsiDevAddr;
	MV_U8 twsiDevAddrType;
} MV_BOARD_BIN_TWSI_INFO;


#endif /* _MV_HIGHSPEED_ENV_SPEC_H */
