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

/* Board specific configuration */
/* ============================ */
/* Bobcat2 Customer Boards */
typedef enum {
	PEX0,
	SGMII0,
	SGMII1,
	DEFAULT_SERDES,
	LAST_SERDES_TYPE
} SERDES_TYPE;

/* Serdes baud rates */
typedef enum {
	__1_25Gbps,

	LAST_SERDES_SPEED
} SERDES_SPEED;

/* Serdes modes */
typedef enum {
	PEX_ROOT_COMPLEX_x1,
	PEX_END_POINT_x1,

	SERDES_DEFAULT_MODE, /* not pex */

	SERDES_LAST_MODE
} SERDES_MODE;

/************************************ structures ******************************/

typedef struct {
	SERDES_TYPE  serdesType;
	MV_U32		 serdesNum;
	SERDES_SPEED serdesSpeed;
	SERDES_MODE  serdesMode;
} SERDES_MAP;

/* Board specific configuration */
/* ============================ */
/* Bobcat2 Customer Boards */
#define BC2_CUSTOMER_BOARD_ID_BASE	0x0
#define BC2_CUSTOMER_BOARD_ID0		(BC2_CUSTOMER_BOARD_ID_BASE + 0)
#define BC2_CUSTOMER_BOARD_ID1		(BC2_CUSTOMER_BOARD_ID_BASE + 1)
#define BC2_CUSTOMER_MAX_BOARD_ID	(BC2_CUSTOMER_BOARD_ID_BASE + 2)
#define BC2_CUSTOMER_BOARD_NUM		(BC2_CUSSTOMER_MAX_BOARD_ID - BC2_CUSTOMER_BOARD_ID_BASE)

/* Bobcat2 Marvell boards */
#define BC2_MARVELL_BOARD_ID_BASE	0x10
#define DB_DX_BC2_ID				(BC2_MARVELL_BOARD_ID_BASE + 0)
#define RD_DX_BC2_ID				(BC2_MARVELL_BOARD_ID_BASE + 1)
#define RD_MTL_BC2					(BC2_MARVELL_BOARD_ID_BASE + 2)
#define BC2_MARVELL_MAX_BOARD_ID	(BC2_MARVELL_BOARD_ID_BASE + 3)
#define BC2_MARVELL_BOARD_NUM		(BC2_MARVELL_MAX_BOARD_ID - BC2_MARVELL_BOARD_ID_BASE)


/* AlleyCat3 Customer Boards */
#define AC3_CUSTOMER_BOARD_ID_BASE	0x20
#define AC3_CUSTOMER_BOARD_ID0		(AC3_CUSTOMER_BOARD_ID_BASE + 0)
#define AC3_CUSTOMER_BOARD_ID1		(AC3_CUSTOMER_BOARD_ID_BASE + 1)
#define AC3_CUSTOMER_MAX_BOARD_ID	(AC3_CUSTOMER_BOARD_ID_BASE + 2)
#define AC3_CUSTOMER_BOARD_NUM		(AC3_CUSTOMER_MAX_BOARD_ID - AC3_CUSTOMER_BOARD_ID_BASE)

/* AlleyCat3 Marvell boards */
#define AC3_MARVELL_BOARD_ID_BASE	0x30
#define DB_AC3_ID					(AC3_MARVELL_BOARD_ID_BASE + 0)
#define AC3_MARVELL_MAX_BOARD_ID	(AC3_MARVELL_BOARD_ID_BASE + 1)
#define AC3_MARVELL_BOARD_NUM		(AC3_MARVELL_MAX_BOARD_ID - AC3_MARVELL_BOARD_ID_BASE)

#define INVALID_BOARD_ID		0xFFFF
#define BOARD_ID_INDEX_MASK		0x10	/* Mask used to return board index via board Id */

/* A generic function pointer for loading the board topology map */
typedef MV_STATUS (*loadTopologyFuncPtr)(SERDES_MAP  *serdesMapArray);


/*************************** Functions declarations ***************************/

MV_U8 mvHwsBoardIdGet(MV_VOID);

#endif /* __INCmvBHboardEnvSpech */

