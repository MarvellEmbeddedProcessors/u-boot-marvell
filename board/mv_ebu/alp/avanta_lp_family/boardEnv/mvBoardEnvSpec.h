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

#ifndef __INCmvBoardEnvSpech
#define __INCmvBoardEnvSpech

#include "mvSysHwConfig.h"

/* I2C bus addresses TODO (omriii) - take from board design */
#define MV_BOARD_DIMM0_I2C_ADDR                 0x56
#define MV_BOARD_DIMM0_I2C_ADDR_TYPE            ADDR7_BIT
#define MV_BOARD_DIMM1_I2C_ADDR                 0x57
#define MV_BOARD_DIMM1_I2C_ADDR_TYPE            ADDR7_BIT
#define MV_BOARD_DIMM_I2C_CHANNEL               0x0

/* Board specific configuration */
/* ============================ */
#ifndef MV_ASMLANGUAGE
/* New board ID numbers */

typedef enum _mvBoardId {
	BOARD_ID_BASE,
	MV_BOARD_ID_AVANTA_LP_FPGA,
	DB_6660_ID,
	RD_6660_ID,
	DB_6650_ID,
	RD_6650_ID,
	MV_MAX_BOARD_ID
} MV_BOARD_ID;

#define MV_INVALID_BOARD_ID             0xFFFFFFFF

/*******************************************************************************
 * AvantaLP DB-88F6600 board */
/******************************************************************************
   MPP#			DEFAULT UNIT		        MPP Values (respectively)
   -----------------------------------------------------------------------------------
   0-13			Boot:Nand Flash                         (1)
   14-15			I2C0					(2)
   16-17			UART					(2)
   18,20,22,23,39	TDM :   Lantiq					(3)
   24-25			GbE : GE1				(2)
   36,41,44,45		PON					(2,2,2,1)
   37-38			SMI Management: Switch			(5)
   39			REF_CLK_OUT				(4)
   42			DYING_GASP (Power off signal)		(2)
   43			SATA_PRESENT_ACTIVEn			(2)
   46-57			Port 0 connected to: Switch		(2)
   58-64			LED_MATRIX				(4)
 */

/* default MPP Types */
#define DB_88F6600_GROUP_0_TYPE         NAND_BOOT_V2
#define DB_88F6600_GROUP_1_TYPE         NAND_BOOT_V2
#define DB_88F6600_GROUP_2_TYPE         TDM_LQ_UNIT
#define DB_88F6600_GROUP_3_TYPE         GE1_UNIT
#define DB_88F6600_GROUP_4_TYPE         GE1_CPU_SMI_CTRL_TDM_LQ_UNIT
#define DB_88F6600_GROUP_5_TYPE         SWITCH_P4_PON_TX_FAULT
#define DB_88F6600_GROUP_6_TYPE         SWITCH_P4
#define DB_88F6600_GROUP_7_TYPE         SWITCH_P4_LED_MATRIX
#define DB_88F6600_GROUP_8_TYPE         LED_MATRIX_PTP

#define DB_88F6600_GPP_OUT_ENA_LOW      0x0
#define DB_88F6600_GPP_OUT_ENA_MID      0x0
#define DB_88F6600_GPP_OUT_ENA_HIGH     0x0
#define DB_88F6600_GPP_OUT_VAL_LOW      0x0
#define DB_88F6600_GPP_OUT_VAL_MID      0x0
#define DB_88F6600_GPP_OUT_VAL_HIGH     0x0
#define DB_88F6600_GPP_POL_LOW          0x0
#define DB_88F6600_GPP_POL_MID          0x0
#define DB_88F6600_GPP_POL_HIGH         0x0

/*******************************************************************************
* AvanataLP customer board
*******************************************************************************/
#define AVANTA_LP_CUSTOMER_MPP0_7               0x00000000
#define AVANTA_LP_CUSTOMER_MPP8_15              0x00000000
#define AVANTA_LP_CUSTOMER_MPP16_23             0x33000000
#define AVANTA_LP_CUSTOMER_MPP24_31             0x11000000
#define AVANTA_LP_CUSTOMER_MPP32_39             0x11111111
#define AVANTA_LP_CUSTOMER_MPP40_47             0x00221100
#define AVANTA_LP_CUSTOMER_MPP48_55             0x00000003
#define AVANTA_LP_CUSTOMER_MPP56_63             0x00000000
#define AVANTA_LP_CUSTOMER_MPP64_67             0x00000000

#define AVANTA_LP_CUSTOMER_GPP_OUT_ENA_LOW      0x0
#define AVANTA_LP_CUSTOMER_GPP_OUT_ENA_MID      0x0
#define AVANTA_LP_CUSTOMER_GPP_OUT_ENA_HIGH     0x0
#define AVANTA_LP_CUSTOMER_GPP_OUT_VAL_LOW      0x0
#define AVANTA_LP_CUSTOMER_GPP_OUT_VAL_MID      0x0
#define AVANTA_LP_CUSTOMER_GPP_OUT_VAL_HIGH     0x0
#define AVANTA_LP_CUSTOMER_GPP_POL_LOW          0x0
#define AVANTA_LP_CUSTOMER_GPP_POL_MID          0x0
#define AVANTA_LP_CUSTOMER_GPP_POL_HIGH         0x0

#endif  /* MV_ASMLANGUAGE */

#endif  /* __INCmvBoardEnvSpech */