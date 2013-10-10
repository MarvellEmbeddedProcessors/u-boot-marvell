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

/* I2C bus addresses */
#define MV_BOARD_DIMM0_I2C_ADDR                 0x56
#define MV_BOARD_DIMM0_I2C_ADDR_TYPE            ADDR7_BIT
#define MV_BOARD_DIMM1_I2C_ADDR                 0x57
#define MV_BOARD_DIMM1_I2C_ADDR_TYPE            ADDR7_BIT
#define MV_BOARD_DIMM_I2C_CHANNEL               0x0

/* Board specific configuration */
/* ============================ */
#ifndef MV_ASMLANGUAGE
/* New board ID numbers */

/* boards ID numbers */
#define BOARD_ID_BASE			0x0

#define RD_6650_ID			(BOARD_ID_BASE)
#define DB_6650_ID			(RD_6650_ID + 1)
#define RD_6660_ID			(DB_6650_ID + 1)
#define DB_6660_ID			(RD_6660_ID + 1)
#define AVANTA_LP_CUSTOMER_ID		(DB_6660_ID + 1)
#define MV_BOARD_ID_AVANTA_LP_FPGA	(AVANTA_LP_CUSTOMER_ID + 1)
#define MV_MAX_BOARD_ID			(MV_BOARD_ID_AVANTA_LP_FPGA + 1)
#define MV_INVALID_BOARD_ID		0xFFFFFFFF

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
#define GROUP1_DEFAULT_MPP_SPI_I2C         0x22000022  /* SPI , I2C */
/*******************************************************************************
* Avanata LP DB-6660 board
*******************************************************************************/
#define DB_88F6660_MPP0_7               0x00020020  /* SPI */
#define DB_88F6660_MPP8_15              0x22000022  /* SPI , I2C */
#define DB_88F6660_MPP16_23             0x00000022
#define DB_88F6660_MPP24_31             0x22222222  /* RGMII-1*/
#define DB_88F6660_MPP32_39             0x04422222  /* GE_SMI ,GE1, PON */
#define DB_88F6660_MPP40_47             0x22100220  /* PON , GE0 */
#define DB_88F6660_MPP48_55             0x22222222  /* GE0*/
#define DB_88F6660_MPP56_63             0x44224422  /* UART1, GE0 , LED */
#define DB_88F6660_MPP64_67             0x004

#define DB_88F6660_GPP_OUT_ENA_LOW      0xFFFFFFFF
#define DB_88F6660_GPP_OUT_ENA_MID      0xFFFFFFFF
#define DB_88F6660_GPP_OUT_ENA_HIGH     0xFFFFFFFF
#define DB_88F6660_GPP_OUT_VAL_LOW      0x0
#define DB_88F6660_GPP_OUT_VAL_MID      0x0
#define DB_88F6660_GPP_OUT_VAL_HIGH     0x0
#define DB_88F6660_GPP_POL_LOW          0x0
#define DB_88F6660_GPP_POL_MID          0x0
#define DB_88F6660_GPP_POL_HIGH         0x0

/*******************************************************************************
* Avanata LP DB-6650 board
*******************************************************************************/
#define DB_88F6650_MPP0_7               0x00020020  /* SPI */
#define DB_88F6650_MPP8_15              0x22000022  /* SPI , I2C */
#define DB_88F6650_MPP16_23             0x00000022
#define DB_88F6650_MPP24_31             0x22222222  /* RGMII-1*/
#define DB_88F6650_MPP32_39             0x04422222  /* GE_SMI ,GE1, PON */
#define DB_88F6650_MPP40_47             0x22100220  /* PON , GE0 */
#define DB_88F6650_MPP48_55             0x22222222  /* GE0*/
#define DB_88F6650_MPP56_63             0x44444422  /* GE0 , LED */
#define DB_88F6650_MPP64_67             0x004

#define DB_88F6650_GPP_OUT_ENA_LOW      0xFFFFFFFF
#define DB_88F6650_GPP_OUT_ENA_MID      0xFFFFFFFF
#define DB_88F6650_GPP_OUT_ENA_HIGH     0xFFFFFFFF
#define DB_88F6650_GPP_OUT_VAL_LOW      0x0
#define DB_88F6650_GPP_OUT_VAL_MID      0x0
#define DB_88F6650_GPP_OUT_VAL_HIGH     0x0
#define DB_88F6650_GPP_POL_LOW          0x0
#define DB_88F6650_GPP_POL_MID          0x0
#define DB_88F6650_GPP_POL_HIGH         0x0

/*******************************************************************************
* Avanata LP RD-6660 board
*******************************************************************************/
#define RD_88F6660_MPP0_7               0x55555555  /* NAND */
#define RD_88F6660_MPP8_15              0x22555555  /* NAND , I2C */
#define RD_88F6660_MPP16_23             0x22222222  /* UA0, TDM */
#define RD_88F6660_MPP24_31             0x33333333  /* SDIO, SPI1 */
#define RD_88F6660_MPP32_39             0x00023330  /* SD_Stat (GPIO_input), SPI1,PON BEN, 3xGPIOs  */
#define RD_88F6660_MPP40_47             0x44120020  /* Switch P4, PON_[XVR,TX_PD,TX_SD]*/
#define RD_88F6660_MPP48_55             0x44444444  /* Switch P4 */
#define RD_88F6660_MPP56_63             0x44004444  /* GE0 , LED, GPIO's */
#define RD_88F6660_MPP64_67             0x004	    /* GPIO's */

#define RD_88F6660_GPP_OUT_ENA_LOW      0xFFFFFFFF
#define RD_88F6660_GPP_OUT_ENA_MID      (~(BIT7 | BIT8 | BIT11))
#define RD_88F6660_GPP_OUT_ENA_HIGH     (~(BIT1 | BIT2))
#define RD_88F6660_GPP_OUT_VAL_LOW      0x0
#define RD_88F6660_GPP_OUT_VAL_MID      0x0
#define RD_88F6660_GPP_OUT_VAL_HIGH     (BIT1 | BIT2)
#define RD_88F6660_GPP_POL_LOW          0x0
#define RD_88F6660_GPP_POL_MID          0x0
#define RD_88F6660_GPP_POL_HIGH         0x0

/*******************************************************************************
* Avanata LP RD-6650 board
*******************************************************************************/
#define RD_88F6650_MPP0_7               0x55555555  /* NAND */
#define RD_88F6650_MPP8_15              0x22555555  /* NAND , I2C */
#define RD_88F6650_MPP16_23             0x11110022  /* UA0, SSI, GPIO's */
#define RD_88F6650_MPP24_31             0x04606001  /* P0,C0_Leds & DYING GASP*/
#define RD_88F6650_MPP32_39             0x04420000  /* GE_SMI, PON & GPIO's */
#define RD_88F6650_MPP40_47             0x44120020  /* Switch P4, PON_[XVR,TX_PD,TX_SD]*/
#define RD_88F6650_MPP48_55             0x44444444  /* Switch P4 */
#define RD_88F6650_MPP56_63             0x44004444  /* GE0 , LED, GPIO's */
#define RD_88F6650_MPP64_67             0x004	    /* GPIO's */

#define RD_88F6650_GPP_OUT_ENA_LOW      (~(BIT19 | BIT25 | BIT29))
#define RD_88F6650_GPP_OUT_ENA_MID      0xFFFFFFFF
#define RD_88F6650_GPP_OUT_ENA_HIGH     0xFFFFFFFF
#define RD_88F6650_GPP_OUT_VAL_LOW      (BIT29 | BIT25) /* BIT 25 - External PHY reset */
#define RD_88F6650_GPP_OUT_VAL_MID      0x0
#define RD_88F6650_GPP_OUT_VAL_HIGH     0x0
#define RD_88F6650_GPP_POL_LOW          0x0
#define RD_88F6650_GPP_POL_MID          0x0
#define RD_88F6650_GPP_POL_HIGH         0x0

/*******************************************************************************
* AVANTA_LP_CUSTOMER_ID - customer board
*******************************************************************************/
#define AVANTA_LP_CUSTOMER_MPP0_7               0x55555555  /* NAND */
#define AVANTA_LP_CUSTOMER_MPP8_15              0x22555555  /* NAND , I2C */
#define AVANTA_LP_CUSTOMER_MPP16_23             0x11110022  /* UA0, SSI, GPIO's */
#define AVANTA_LP_CUSTOMER_MPP24_31             0x04606101  /* PCIe0CLKREQ, P0,C0_Leds & DYING GASP*/
#define AVANTA_LP_CUSTOMER_MPP32_39             0x04420000  /* GE_SMI, PON & GPIO's */
#define AVANTA_LP_CUSTOMER_MPP40_47             0x22120020  /* GE0, PON_[XVR,TX_PD,TX_SD]*/
#define AVANTA_LP_CUSTOMER_MPP48_55             0x22222222  /* GE0*/
#define AVANTA_LP_CUSTOMER_MPP56_63             0x44004422  /* GE0 , LED, GPIO's */
#define AVANTA_LP_CUSTOMER_MPP64_67             0x004	     /* GPIO's */

/* GPPs
MPP#	NAME			IN/OUT
--------------------------------------
19	TDM_reset		OUT
25	Ethernet Reset		OUT
29	PCIe0_RSTOUTn		OUT
*/
#define AVANTA_LP_CUSTOMER_GPP_OUT_ENA_LOW      (~(BIT19 | BIT25 | BIT29))
#define AVANTA_LP_CUSTOMER_GPP_OUT_ENA_MID      0xFFFFFFFF
#define AVANTA_LP_CUSTOMER_GPP_OUT_ENA_HIGH     0xFFFFFFFF
#define AVANTA_LP_CUSTOMER_GPP_OUT_VAL_LOW      BIT29
#define AVANTA_LP_CUSTOMER_GPP_OUT_VAL_MID      0x0
#define AVANTA_LP_CUSTOMER_GPP_OUT_VAL_HIGH     0x0
#define AVANTA_LP_CUSTOMER_GPP_POL_LOW          0x0
#define AVANTA_LP_CUSTOMER_GPP_POL_MID          0x0
#define AVANTA_LP_CUSTOMER_GPP_POL_HIGH         0x0

#endif  /* MV_ASMLANGUAGE */

#endif  /* __INCmvBoardEnvSpech */
