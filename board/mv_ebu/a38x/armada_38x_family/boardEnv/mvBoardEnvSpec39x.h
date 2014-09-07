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

/* Board specific ID configuration */
/* =============================== */
/* Armada-39x Customer boards */
#define CUTOMER_BOARD_ID_BASE			A39X_CUTOMER_BOARD_ID_BASE
#define CUSTOMER_BOARD_ID0			A39X_CUSTOMER_BOARD_ID0
#define CUSTOMER_BOARD_ID1			A39X_CUSTOMER_BOARD_ID1
#define MV_MAX_CUSTOMER_BOARD_ID		A39X_MV_MAX_CUSTOMER_BOARD_ID
#define MV_CUSTOMER_BOARD_NUM			A39X_MV_CUSTOMER_BOARD_NUM

/* Armada-39x Marvell boards */
#define MARVELL_BOARD_ID_BASE			A39X_MARVELL_BOARD_ID_BASE
#define MV_MAX_MARVELL_BOARD_ID			A39X_MV_MAX_MARVELL_BOARD_ID
#define MV_MARVELL_BOARD_NUM			A39X_MV_MARVELL_BOARD_NUM

#define MV_DEFAULT_BOARD_ID                     A39X_DB_69XX_ID

/****************************** Customer Boards *******************************/
/*******************************************************************************
 * A39x Customer Board 0 - Based on DB
 *******************************************************************************/
#define A39X_CUSTOMER_BOARD_0_MPP0_7			0x77111111
#define A39X_CUSTOMER_BOARD_0_MPP8_15			0x30777777
#define A39X_CUSTOMER_BOARD_0_MPP16_23			0x55066005
#define A39X_CUSTOMER_BOARD_0_MPP24_31			0x05050050
#define A39X_CUSTOMER_BOARD_0_MPP32_39			0x05055555
#define A39X_CUSTOMER_BOARD_0_MPP40_47			0x51132565
#define A39X_CUSTOMER_BOARD_0_MPP48_55			0x00000000
#define A39X_CUSTOMER_BOARD_0_MPP56_63			0x00004444

#define A39X_CUSTOMER_BOARD_0_GPP_OUT_ENA_LOW	0xFFFFFFFF
#define A39X_CUSTOMER_BOARD_0_GPP_OUT_ENA_MID	(~(0x0E))
#define A39X_CUSTOMER_BOARD_0_GPP_OUT_VAL_LOW	0x0
#define A39X_CUSTOMER_BOARD_0_GPP_OUT_VAL_MID	0x0
#define A39X_CUSTOMER_BOARD_0_GPP_POL_LOW		0x0
#define A39X_CUSTOMER_BOARD_0_GPP_POL_MID		0x0

/******************************* Marvell Boards *******************************/

/*******************************************************************************
 * A39x DB-88F6920 board */
/******************************************************************************
   MPP#			DEFAULT UNIT		        MPP Values (respectively)
   -----------------------------------------------------------------------------------
   0-1			UART0				1
   2-3			I2C0				1
   4-5			SMI1				7
   6-7			XSMI				7
   8-10			NSS_PTP				7
   11-13		LED_DRV				7
   15			PCIe0_RSTOUTn (out)		3
   16			PCIe0_CLKREQ (in)		5
   19-20		UA1				6
   22-23,25,28,30,
   32-36,38,40-42	NAND				5
   43			M_VTT_CTRL (out)		2
   44			SATA2_PRESENT_ACTIVEn (out)	3
   45			TEF_CLK_OUT[0] (out)		1
   46			REF_CLK_OUT[1] (out)		1
   47			SATA3_PRESENT_ACTIVEn (out)	5
   56-59		SPI1				4
*/
#define GROUP0_DEFAULT_MPP_TWSI_I2C         0x1111  /* TWSI , I2C */
#define GROUP0_DEFAULT_MPP_TWSI_I2C_MASK    0xFFFF  /* TWSI , I2C */
/*******************************************************************************
* A39x DB-69xx board
*******************************************************************************/
#define DB_88F69XX_MPP0_7               0x77111111
#define DB_88F69XX_MPP8_15              0x30777777
#define DB_88F69XX_MPP16_23             0x55066005
#define DB_88F69XX_MPP24_31             0x05050050
#define DB_88F69XX_MPP32_39             0x05055555
#define DB_88F69XX_MPP40_47             0x51132565
#define DB_88F69XX_MPP48_55             0x00000000
#define DB_88F69XX_MPP56_63             0x00004444

#define DB_88F69XX_GPP_OUT_ENA_LOW      0xFFFFFFFF
#define DB_88F69XX_GPP_OUT_ENA_MID      0xFFFFFFFF
#define DB_88F69XX_GPP_OUT_VAL_LOW      0x0
#define DB_88F69XX_GPP_OUT_VAL_MID      0x0
#define DB_88F69XX_GPP_POL_LOW          0x0
#define DB_88F69XX_GPP_POL_MID          0x0
