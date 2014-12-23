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
#include "ddr3_hws_hw_training.h"
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
#define RD_MTL_4XG_AC3_ID			(AC3_MARVELL_BOARD_ID_BASE + 1)
#define RD_MTL_2XXG_2XG_AC3_ID		(AC3_MARVELL_BOARD_ID_BASE + 2)
#define DB_MISL_24G4G_AC3_ID		(AC3_MARVELL_BOARD_ID_BASE + 3)
#define RD_MTL_24G_AC3_ID			(AC3_MARVELL_BOARD_ID_BASE + 4)
#define AC3_MARVELL_MAX_BOARD_ID	(AC3_MARVELL_BOARD_ID_BASE + 5)
#define AC3_MARVELL_BOARD_NUM		(AC3_MARVELL_MAX_BOARD_ID - AC3_MARVELL_BOARD_ID_BASE)

#define INVALID_BOARD_ID			0xFFFF
#define BOARD_ID_INDEX_MASK			0x10	/* Mask used to return board index via board Id */


/* Bobcat2 device revision */
#define BC2_DEV_VERSION_ID_REG		           0xF8240	/* under server space */
#define BC2_JTAG_DEV_ID_STATUS_REG_ADDR        0x000F8244
#define BC2_REVISON_ID_OFFS			              16
#define BC2_JTAG_DEV_ID_STATUS_VERSION_OFFSET     28
#define BC2_REVISON_ID_MASK			0xF
#define MV_MSYS_BC2_A0_ID			0x0
#define MV_MSYS_BC2_B0_ID			0x1

/* Alleycat3 device revision */
#define MV_MSYS_AC3_A0_ID			0x0

typedef enum _mvSuspendWakeupStatus {
	MV_SUSPEND_WAKEUP_DISABLED,
	MV_SUSPEND_WAKEUP_ENABLED,
	MV_SUSPEND_WAKEUP_ENABLED_GPIO_DETECTED,
} MV_SUSPEND_WAKEUP_STATUS;

/*************************** Globals ***************************/

extern MV_DRAM_DLB_CONFIG ddr3DlbConfigTable[];

/*************************** Functions declarations ***************************/

/**************************************************************************
 * mvBoardIdGet -
 *
 * DESCRIPTION:          Returns the board ID
 *
 * INPUT:
 *		None.
 *
 * OUTPUT:
 *		None.
 *
 * RETURNS:
 *		Board ID.
 ***************************************************************************/
MV_U32 mvBoardIdGet(MV_VOID);

/*******************************************************************************
* mvBoardIdIndexGet
*
* DESCRIPTION:
*	returns an index for board arrays with direct memory access, according to board id
*
* INPUT:
*       boardId.
*
* OUTPUT:
*       direct access index for board arrays
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_U32 mvBoardIdIndexGet(MV_U32 boardId);

/**************************************************************************
 * mvBoardTclkGet -
 *
 * DESCRIPTION:          Returns the board id
 *
 * INPUT:
 *		None.
 *
 * OUTPUT:
 *		None.
 *
 * RETURNS:
 *		board ID.
 ***************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID);

/**************************************************************************
 * mvBoardSarBoardIdGet -
 *
 * DESCRIPTION:          Returns the board ID from SatR
 *
 * INPUT:
 *		None.
 *
 * OUTPUT:
 *		None.
 *
 * RETURNS:
 *		Tclk
 ***************************************************************************/
MV_STATUS mvBoardSarBoardIdGet(MV_U8 *value);

/************************************************************************************
* mvSysEnvSuspendWakeupCheck
* DESCRIPTION:
*		Reads GPIO input for suspend-wakeup indication.
*					Not supported for AC3/BC2
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RRETURN:
*		MV_U32 indicating suspend wakeup status:
* 		0 - normal initialization, otherwise - suspend wakeup.
 ***************************************************************************/
MV_SUSPEND_WAKEUP_STATUS mvSysEnvSuspendWakeupCheck(MV_VOID);

/*******************************************************************************
* mvSysEnvDeviceRevGet - Get Marvell controller device revision number
*
* DESCRIPTION:
*       This function returns 8bit describing the device revision as defined
*       Revision ID Register.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       8bit describing Marvell controller revision number
*
*******************************************************************************/
MV_U8 mvSysEnvDeviceRevGet(MV_VOID);

/*******************************************************************************
* mvSysEnvDlbConfigPtrGet
* DESCRIPTION: defines pointer to to DLB COnfiguration table
* INPUT: none
* OUTPUT: 
* RETURN:
*       returns pointer to DLB COnfiguration table
*******************************************************************************/
MV_DRAM_DLB_CONFIG  *mvSysEnvDlbConfigPtrGet(MV_VOID);

/*******************************************************************************
* mvSysEnvGetTopologyUpdateInfo
*
* DESCRIPTION: Read TWSI fields to update DDR topology structure
*
* INPUT: None
*
* OUTPUT: None, 0 means no topology update
*
* RETURN:
*       Bit mask of changes topology features
*
*******************************************************************************/
MV_U32 mvSysEnvGetTopologyUpdateInfo(MV_TOPOLOGY_UPDATE_INFO *topologyUpdateInfo);

#endif /* __INCmvBHboardEnvSpech */

