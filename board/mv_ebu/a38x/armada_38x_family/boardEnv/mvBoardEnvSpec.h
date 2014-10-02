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

/* Board specific configuration */
/* ============================ */
#ifndef __INCmvBoardEnvSpech
#define __INCmvBoardEnvSpech

#include "mvSysHwConfig.h"
#ifdef CONFIG_ARMADA_38X
#include "boardEnv/mvBoardEnvSpec38x.h"
#elif defined CONFIG_ARMADA_39X
#include "boardEnv/mvBoardEnvSpec39x.h"
#endif

/* Board specific configuration */
/* ============================ */
#ifndef MV_ASMLANGUAGE
/* Board ID numbers */

/* Armada-38x Customer boards */
#define A38X_CUTOMER_BOARD_ID_BASE		0x0
#define A38X_CUSTOMER_BOARD_ID0			(A38X_CUTOMER_BOARD_ID_BASE + 0)
#define A38X_CUSTOMER_BOARD_ID1			(A38X_CUTOMER_BOARD_ID_BASE + 1)
#define A38X_MV_MAX_CUSTOMER_BOARD_ID		(A38X_CUTOMER_BOARD_ID_BASE + 2)
#define A38X_MV_CUSTOMER_BOARD_NUM		(A38X_MV_MAX_CUSTOMER_BOARD_ID - A38X_CUTOMER_BOARD_ID_BASE)

/* Armada-38x Marvell boards */
#define A38X_MARVELL_BOARD_ID_BASE		0x10
#define RD_NAS_68XX_ID				(A38X_MARVELL_BOARD_ID_BASE + 0)
#define DB_68XX_ID				(A38X_MARVELL_BOARD_ID_BASE + 1)
#define RD_AP_68XX_ID				(A38X_MARVELL_BOARD_ID_BASE + 2)
#define DB_AP_68XX_ID				(A38X_MARVELL_BOARD_ID_BASE + 3)
#define DB_GP_68XX_ID				(A38X_MARVELL_BOARD_ID_BASE + 4)
#define A38X_MV_MAX_MARVELL_BOARD_ID		(A38X_MARVELL_BOARD_ID_BASE + 5)
#define A38X_MV_MARVELL_BOARD_NUM		(A38X_MV_MAX_MARVELL_BOARD_ID - A38X_MARVELL_BOARD_ID_BASE)


/* Armada-39x Customer boards */
#define A39X_CUTOMER_BOARD_ID_BASE		0x20
#define A39X_CUSTOMER_BOARD_ID0			(A39X_CUTOMER_BOARD_ID_BASE + 0)
#define A39X_CUSTOMER_BOARD_ID1			(A39X_CUTOMER_BOARD_ID_BASE + 1)
#define A39X_MV_MAX_CUSTOMER_BOARD_ID		(A39X_CUTOMER_BOARD_ID_BASE + 2)
#define A39X_MV_CUSTOMER_BOARD_NUM		(A39X_MV_MAX_CUSTOMER_BOARD_ID - A39X_CUTOMER_BOARD_ID_BASE)

/* Armada-39x Marvell boards */
#define A39X_MARVELL_BOARD_ID_BASE		0x30
#define A39X_RD_69XX_ID				(A39X_MARVELL_BOARD_ID_BASE + 0)
#define A39X_DB_69XX_ID				(A39X_MARVELL_BOARD_ID_BASE + 1)
#define A39X_MV_MAX_MARVELL_BOARD_ID		(A39X_MARVELL_BOARD_ID_BASE + 2)
#define A39X_MV_MARVELL_BOARD_NUM		(A39X_MV_MAX_MARVELL_BOARD_ID - A39X_MARVELL_BOARD_ID_BASE)

#define MV_INVALID_BOARD_ID			0xFFFFFFFF

#define BOARD_ID_INDEX_MASK			0x10 /* Mask used to return board index via board Id */

#endif  /* MV_ASMLANGUAGE */

#endif  /* __INCmvBoardEnvSpech */
