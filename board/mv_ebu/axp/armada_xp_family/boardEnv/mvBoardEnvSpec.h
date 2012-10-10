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

/* For future use */
#define BD_ID_DATA_START_OFFS			0x0
#define BD_DETECT_SEQ_OFFS			0x0
#define BD_SYS_NUM_OFFS				0x4
#define BD_NAME_OFFS				0x8

/* I2C bus addresses TODO - take from board design */
#define MV_BOARD_DIMM0_I2C_ADDR			0x56
#define MV_BOARD_DIMM0_I2C_ADDR_TYPE 		ADDR7_BIT
#define MV_BOARD_DIMM1_I2C_ADDR			0x57
#define MV_BOARD_DIMM1_I2C_ADDR_TYPE 		ADDR7_BIT
#define MV_BOARD_DIMM_I2C_CHANNEL		0x0


/* Board specific configuration */
/* ============================ */

/* boards ID numbers */
#define BOARD_ID_BASE				0x0

/* New board ID numbers */
#define DB_88F78XX0_BP_ID			(BOARD_ID_BASE)
#define RD_78460_SERVER_ID			(DB_88F78XX0_BP_ID + 1)
#define DB_78X60_PCAC_ID			(RD_78460_SERVER_ID + 1)
#define FPGA_88F78XX0_ID			(DB_78X60_PCAC_ID + 1)
#define DB_88F78XX0_BP_REV2_ID		(FPGA_88F78XX0_ID + 1)
#define RD_78460_NAS_ID				(DB_88F78XX0_BP_REV2_ID + 1)
#define DB_78X60_AMC_ID				(RD_78460_NAS_ID + 1)
#define DB_78X60_PCAC_REV2_ID		(DB_78X60_AMC_ID + 1)
#define RD_78460_SERVER_REV2_ID		(DB_78X60_PCAC_REV2_ID + 1)
#define DB_784MP_GP_ID				(RD_78460_SERVER_REV2_ID + 1)
#define RD_78460_CUSTOMER_ID		(DB_784MP_GP_ID+ 1)
#define MV_MAX_BOARD_ID				(RD_78460_CUSTOMER_ID + 1)
#define INVALID_BAORD_ID			0xFFFFFFFF

/******************/
/* DB-88F78XX0-BP */
/******************/
#define DB_88F78XX0_MPP0_7			0x11111111
#define DB_88F78XX0_MPP8_15			0x22221111
#define DB_88F78XX0_MPP16_23			0x22222222
/* TODO Kostap - change MPP29 (CPU0 Vdd) back to default value 5
   when PM configuration changed to have it as active "high"
   Otherwise setting it to default value will shut down CPU0 */
#define DB_88F78XX0_MPP24_31			0x11040000 /* bits[27:24] = 0x5 to enable PMm for CPU0 */
#define DB_88F78XX0_MPP32_39			0x11111111
#define DB_88F78XX0_MPP40_47			0x04221130 /* bits[3:0] = 0x3 to enable PM for CPU1 */
#define DB_88F78XX0_MPP48_55			0x11111110
#define DB_88F78XX0_MPP56_63			0x11111101 /* bits[7:4] = 0x1 to enable PM for CPU2/3 */
#define DB_88F78XX0_MPP64_67			0x00002111

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
24	UsbDevice_Vbus		IN
25	Touch SG/ MII Int#	IN
26	7seg bit0		OUT
27	7seg bit1		OUT
48	7seg bit2		OUT
*/
#define DB_88F78XX0_GPP_OUT_ENA_LOW		(~(BIT26 | BIT27))
#define DB_88F78XX0_GPP_OUT_ENA_MID		(~(BIT16 | BIT15))
#define DB_88F78XX0_GPP_OUT_ENA_HIGH		(~(0x0))

#define DB_88F78XX0_GPP_OUT_VAL_LOW		0x0
#define DB_88F78XX0_GPP_OUT_VAL_MID		BIT15
#define DB_88F78XX0_GPP_OUT_VAL_HIGH		0x0

#define DB_88F78XX0_GPP_POL_LOW			0x0
#define DB_88F78XX0_GPP_POL_MID			0x0
#define DB_88F78XX0_GPP_POL_HIGH		0x0

/**********************/
/* DB-88F78XX0-BP Rev2*/
/**********************/
#define DB_88F78XX0_REV2_MPP0_7			0x11111111
#define DB_88F78XX0_REV2_MPP8_15		0x22221111
#define DB_88F78XX0_REV2_MPP16_23		0x22222222
/* TODO Kostap - change MPP29 (CPU0 Vdd) back to default value 5
   when PM configuration changed to have it as active "high"
   Otherwise setting it to default value will shut down CPU0 */
#define DB_88F78XX0_REV2_MPP24_31		0x11040000 /* bits[27:24] = 0x5 to enable PMm for CPU0 */
#define DB_88F78XX0_REV2_MPP32_39		0x11111111
#define DB_88F78XX0_REV2_MPP40_47		0x04221130 /* bits[3:0] = 0x3 to enable PM for CPU1 */
#define DB_88F78XX0_REV2_MPP48_55		0x11111113
#define DB_88F78XX0_REV2_MPP56_63		0x11111101 /* bits[7:4] = 0x1 to enable PM for CPU2/3 */
#define DB_88F78XX0_REV2_MPP64_67		0x00002111

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
24	UsbDevice_Vbus		IN
25	Touch SG/ MII Int#	IN
26	7seg bit0		OUT
27	7seg bit1		OUT
48	7seg bit2		OUT
*/
#define DB_88F78XX0_REV2_GPP_OUT_ENA_LOW	(~(BIT26 | BIT27))
#define DB_88F78XX0_REV2_GPP_OUT_ENA_MID	(~(BIT16 | BIT15))
#define DB_88F78XX0_REV2_GPP_OUT_ENA_HIGH	(~(0x0))

#define DB_88F78XX0_REV2_GPP_OUT_VAL_LOW	0x0
#define DB_88F78XX0_REV2_GPP_OUT_VAL_MID	BIT15
#define DB_88F78XX0_REV2_GPP_OUT_VAL_HIGH	0x0

#define DB_88F78XX0_REV2_GPP_POL_LOW		0x0
#define DB_88F78XX0_REV2_GPP_POL_MID		0x0
#define DB_88F78XX0_REV2_GPP_POL_HIGH		0x0

/**********************/
/* DB-AXP-NAS         */
/**********************/

#define RD_78460_NAS_MPP0_7             0x00000000
#define RD_78460_NAS_MPP8_15		0x00000000
#define RD_78460_NAS_MPP16_23		0x33000000
#define RD_78460_NAS_MPP24_31		0x11000000 /* bits[27:24] = 0x5 to enable PMm for CPU0 */
#define RD_78460_NAS_MPP32_39		0x11111111
#define RD_78460_NAS_MPP40_47		0x00221100 /* bits[3:0] = 0x3 to enable PM for CPU1 */
#define RD_78460_NAS_MPP48_55		0x00000003
#define RD_78460_NAS_MPP56_63		0x00000000 /* bits[7:4] = 0x1 to enable PM for CPU2/3 */
#define RD_78460_NAS_MPP64_67		0x00000000

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
21	SW_Reset_		OUT
25      Phy_Int#                IN
28      SDI_WP                  IN
29      SDI_Status              IN
54-61   On GPP Connector        ?
62      Switch Interrupt        IN
63-65   Reserved from SW Board  ?
66      SW_BRD connected        IN

*/
#define RD_78460_NAS_GPP_OUT_ENA_LOW	(~(BIT21))
#define RD_78460_NAS_GPP_OUT_ENA_MID	(~(0x0))
#define RD_78460_NAS_GPP_OUT_ENA_HIGH	(~(0x0))

#define RD_78460_NAS_GPP_OUT_VAL_LOW	(BIT21)
#define RD_78460_NAS_GPP_OUT_VAL_MID	0x0
#define RD_78460_NAS_GPP_OUT_VAL_HIGH	0x0

#define RD_78460_NAS_GPP_POL_LOW		0x0
#define RD_78460_NAS_GPP_POL_MID		0x0
#define RD_78460_NAS_GPP_POL_HIGH		0x0


/*******************/
/* RD-78460-SERVER */
/*******************/
#define RD_78460_MPP0_7				0x00000000
#define RD_78460_MPP8_15			0x00000000
#define RD_78460_MPP16_23			0x00000000
#define RD_78460_MPP24_31			0x00000000
#define RD_78460_MPP32_39			0x11110000
#define RD_78460_MPP40_47			0x00000000
#define RD_78460_MPP48_55			0x00000000
#define RD_78460_MPP56_63			0x00000000
#define RD_78460_MPP64_67			0x00000000

/* GPPs
MPP#	NAME		IN/OUT
----------------------------------------------
16		CPU ID		IN
17		CPU ID		IN
18		CPU ID		IN
19		CPU ID		IN
23		USER LED	OUT
25		Touch SG/ MII Int#	IN
28		RESET GPIO FOR OTHER DEVICES
29		RESET GPIO FOR OTHER DEVICES
30		RESET GPIO FOR OTHER DEVICES
31		RESET GPIO FOR OTHER DEVICES
32		RESET GPIO FOR OTHER DEVICES
33		RESET GPIO FOR OTHER DEVICES

40		GPIOs TO SLED CONNECTOR(CPU0 ONLY)
41		GPIOs TO SLED CONNECTOR(CPU0 ONLY)
42		GPIOs TO SLED CONNECTOR(CPU0 ONLY)
43		GPIOs TO SLED CONNECTOR(CPU0 ONLY)
44		GPIOs TO SLED CONNECTOR(CPU0 ONLY)

*/

#define RD_78460_GPP_OUT_ENA_LOW		(~(BIT23 | BIT28 | BIT29 | BIT30 | BIT31))
#define RD_78460_GPP_OUT_ENA_MID		(~(BIT0 | BIT1 | BIT2 | BIT8 | BIT9 | BIT10 | BIT11 | BIT12))
#define RD_78460_GPP_OUT_ENA_HIGH		(~(0x0))

#define RD_78460_GPP_OUT_VAL_LOW		(BIT28 | BIT29 | BIT30 | BIT31)
#define RD_78460_GPP_OUT_VAL_MID		(BIT0)
#define RD_78460_GPP_OUT_VAL_HIGH		0x0

#define RD_78460_GPP_POL_LOW			0x0
/* (BIT16 | BIT17 | BIT18 | BIT19) */
#define RD_78460_GPP_POL_MID			0x0
#define RD_78460_GPP_POL_HIGH			0x0

/************************/
/* RD-78460-SERVER-REV2 */
/************************/
#define RD_78460_SERVER_REV2_MPP0_7				0x00000000
#define RD_78460_SERVER_REV2_MPP8_15			0x00000000
#define RD_78460_SERVER_REV2_MPP16_23			0x00000000
#define RD_78460_SERVER_REV2_MPP24_31			0x00000000
#define RD_78460_SERVER_REV2_MPP32_39			0x11110000
#define RD_78460_SERVER_REV2_MPP40_47			0x00000000
#define RD_78460_SERVER_REV2_MPP48_55			0x00000000
#define RD_78460_SERVER_REV2_MPP56_63			0x00000000
#define RD_78460_SERVER_REV2_MPP64_67			0x00000000

/* GPPs
MPP#	NAME		IN/OUT
----------------------------------------------
16		CPU ID		IN
17		CPU ID		IN
18		CPU ID		IN
19		CPU ID		IN
23		USER LED	OUT
25		Touch SG/ MII Int#	IN
28		RESET GPIO FOR OTHER DEVICES
29		RESET GPIO FOR OTHER DEVICES
30		RESET GPIO FOR OTHER DEVICES
31		RESET GPIO FOR OTHER DEVICES
32		RESET GPIO FOR OTHER DEVICES
33		RESET GPIO FOR OTHER DEVICES

40		GPIOs TO SLED CONNECTOR(CPU0 ONLY)
41		GPIOs TO SLED CONNECTOR(CPU0 ONLY)
42		GPIOs TO SLED CONNECTOR(CPU0 ONLY)
43		GPIOs TO SLED CONNECTOR(CPU0 ONLY)
44		GPIOs TO SLED CONNECTOR(CPU0 ONLY)

*/

#define RD_78460_SERVER_REV2_GPP_OUT_ENA_LOW		(~(BIT23 | BIT28 | BIT29 | BIT30 | BIT31))
#define RD_78460_SERVER_REV2_GPP_OUT_ENA_MID		(~(BIT0 | BIT1 | BIT2 | BIT8 | BIT9 | BIT10 | BIT11 | BIT12))
#define RD_78460_SERVER_REV2_GPP_OUT_ENA_HIGH		(~(0x0))

#define RD_78460_SERVER_REV2_GPP_OUT_VAL_LOW		(BIT28 | BIT29 | BIT30 | BIT31)
#define RD_78460_SERVER_REV2_GPP_OUT_VAL_MID		(BIT0)
#define RD_78460_SERVER_REV2_GPP_OUT_VAL_HIGH		0x0

#define RD_78460_SERVER_REV2_GPP_POL_LOW			0x0
/* (BIT16 | BIT17 | BIT18 | BIT19) */
#define RD_78460_SERVER_REV2_GPP_POL_MID			0x0
#define RD_78460_SERVER_REV2_GPP_POL_HIGH			0x0


/********************/
/* DB-88F78XX0-PCAC */
/********************/

#define DB_78X60_PCAC_MPP0_7			0x00000000
#define DB_78X60_PCAC_MPP8_15			0x00000000
#define DB_78X60_PCAC_MPP16_23			0x00000000
#define DB_78X60_PCAC_MPP24_31			0x11000000
#define DB_78X60_PCAC_MPP32_39			0x11111111
#define DB_78X60_PCAC_MPP40_47			0x00221105
#define DB_78X60_PCAC_MPP48_55			0x00000000
#define DB_78X60_PCAC_MPP56_63			0x00000000
#define DB_78X60_PCAC_MPP64_67			0x00000000

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
16	Jumper 1 			IN
17	Jumper 2 			IN
18	Jumper 3 			IN
19	Jumper 4 			IN
20	Jumper 5 			IN
21	GP				OUT
23	UsbDevice_Vbus			IN
53	7seg bit0			OUT
54	7seg bit1			OUT
55	7seg bit2			OUT
56	7seg bit3			OUT
*/

#define DB_78X60_PCAC_GPP_OUT_ENA_LOW		(~(BIT26 | BIT27))
#define DB_78X60_PCAC_GPP_OUT_ENA_MID		(~(BIT16 | BIT21 | BIT22 | BIT23 | BIT24))
#define DB_78X60_PCAC_GPP_OUT_ENA_HIGH		(~(0x0))

#define DB_78X60_PCAC_GPP_OUT_VAL_LOW		0x0
#define DB_78X60_PCAC_GPP_OUT_VAL_MID		0x0
#define DB_78X60_PCAC_GPP_OUT_VAL_HIGH		0x0

#define DB_78X60_PCAC_GPP_POL_LOW		0x0
#define DB_78X60_PCAC_GPP_POL_MID		0x0
#define DB_78X60_PCAC_GPP_POL_HIGH		0x0

/*************************/
/* DB-88F78XX0-PCAC-REV2 */
/*************************/

#define DB_78X60_PCAC_REV2_MPP0_7			0x00000000
#define DB_78X60_PCAC_REV2_MPP8_15			0x00000000
#define DB_78X60_PCAC_REV2_MPP16_23			0x00000000
#define DB_78X60_PCAC_REV2_MPP24_31			0x11000000
#define DB_78X60_PCAC_REV2_MPP32_39			0x11111111
#define DB_78X60_PCAC_REV2_MPP40_47			0x00221105
#define DB_78X60_PCAC_REV2_MPP48_55			0x00000000
#define DB_78X60_PCAC_REV2_MPP56_63			0x00000000
#define DB_78X60_PCAC_REV2_MPP64_67			0x00000000

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
16	Jumper 1 			IN
17	Jumper 2 			IN
18	Jumper 3 			IN
19	Jumper 4 			IN
20	Jumper 5 			IN
21	GP				OUT
23	UsbDevice_Vbus			IN
53	7seg bit0			OUT
54	7seg bit1			OUT
55	7seg bit2			OUT
56	7seg bit3			OUT
*/

#define DB_78X60_PCAC_REV2_GPP_OUT_ENA_LOW		(~(BIT26 | BIT27))
#define DB_78X60_PCAC_REV2_GPP_OUT_ENA_MID		(~(BIT16 | BIT21 | BIT22 | BIT23 | BIT24))
#define DB_78X60_PCAC_REV2_GPP_OUT_ENA_HIGH		(~(0x0))

#define DB_78X60_PCAC_REV2_GPP_OUT_VAL_LOW		0x0
#define DB_78X60_PCAC_REV2_GPP_OUT_VAL_MID		0x0
#define DB_78X60_PCAC_REV2_GPP_OUT_VAL_HIGH		0x0

#define DB_78X60_PCAC_REV2_GPP_POL_LOW		0x0
#define DB_78X60_PCAC_REV2_GPP_POL_MID		0x0
#define DB_78X60_PCAC_REV2_GPP_POL_HIGH		0x0

/********************/
/* FPGA-88F78XX0-BP */
/********************/
#define FPGA_88F78XX0_MPP0_7			0x11111111
#define FPGA_88F78XX0_MPP8_15			0x22221111
#define FPGA_88F78XX0_MPP16_23			0x22222222
#define FPGA_88F78XX0_MPP24_31			0x11500000
#define FPGA_88F78XX0_MPP32_39			0x11111111
#define FPGA_88F78XX0_MPP40_47			0x44221133
#define FPGA_88F78XX0_MPP48_55			0x11111111
#define FPGA_88F78XX0_MPP56_63			0x11111111
#define FPGA_88F78XX0_MPP64_67			0x00002111

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
24	UsbDevice_Vbus		IN
25	Touch SG/ MII Int#	IN
26	7seg bit0		OUT
27	7seg bit1		OUT
48	7seg bit2		OUT
*/
#define FPGA_88F78XX0_GPP_OUT_ENA_LOW		(BIT26 | BIT27)
#define FPGA_88F78XX0_GPP_OUT_ENA_MID		(BIT16)
#define FPGA_88F78XX0_GPP_OUT_ENA_HIGH		0x0

#define FPGA_88F78XX0_GPP_OUT_VAL_LOW		0x0
#define FPGA_88F78XX0_GPP_OUT_VAL_MID		0x0
#define FPGA_88F78XX0_GPP_OUT_VAL_HIGH		0x0

#define FPGA_88F78XX0_GPP_POL_LOW		0x0
#define FPGA_88F78XX0_GPP_POL_MID		0x0
#define FPGA_88F78XX0_GPP_POL_HIGH		0x0



/********************/
/* DB-78460-AMC     */
/********************/

#define DB_78X60_AMC_MPP0_7			0x11111111
#define DB_78X60_AMC_MPP8_15			0x00001111
#define DB_78X60_AMC_MPP16_23			0x00000000
#define DB_78X60_AMC_MPP24_31			0x00000000
#define DB_78X60_AMC_MPP32_39			0x11110000
#define DB_78X60_AMC_MPP40_47			0x00004000
#define DB_78X60_AMC_MPP48_55			0x00001113
#define DB_78X60_AMC_MPP56_63			0x11111110
#define DB_78X60_AMC_MPP64_67			0x00000111

/* GPPs
MPP#	NAME		IN/OUT
----------------------------------------------
16	MB_INT#		IN
17	Phy1_INT#	IN
18	Phy2_INT#	IN
19	Brd_Led_0	IN (for next board)
21	Brd_Led_1	OUT
23	Brd_Led_2	OUT
29	Brd_Led_3	OUT
30	Brd_Led_4	OUT
34	Dbg_JP0		IN
35	Dbg_JP1		IN
40	Dbg_JP2		IN
41	Dbg_JP3		IN
42	Dbg_JP4		IN
53	7 Segment 0	OUT
54	7 Segment 1	OUT
55	7 Segment 2	OUT
56	7 Segment 3	OUT
*/

#define DB_78X60_AMC_GPP_OUT_ENA_LOW		(~(BIT21 | BIT23 | BIT29 | BIT30))
#define DB_78X60_AMC_GPP_OUT_ENA_MID		(~(BIT21 | BIT22 | BIT23 | BIT24))
#define DB_78X60_AMC_GPP_OUT_ENA_HIGH		(~(0x0))

#define DB_78X60_AMC_GPP_OUT_VAL_LOW		0x0
#define DB_78X60_AMC_GPP_OUT_VAL_MID		0x0
#define DB_78X60_AMC_GPP_OUT_VAL_HIGH		0x0

#define DB_78X60_AMC_GPP_POL_LOW		0x0
#define DB_78X60_AMC_GPP_POL_MID		0x0
#define DB_78X60_AMC_GPP_POL_HIGH		0x0




/*********************/
/* DB-AXP-GP         */
/*********************/

#define RD_78460_GP_MPP0_7			0x00000000
#define RD_78460_GP_MPP8_15			0x00000000
#define RD_78460_GP_MPP16_23		0x33000000
#define RD_78460_GP_MPP24_31		0x11000000
#define RD_78460_GP_MPP32_39		0x11111111
#define RD_78460_GP_MPP40_47		0x00221100
#define RD_78460_GP_MPP48_55		0x00000003
#define RD_78460_GP_MPP56_63		0x00000000
#define RD_78460_GP_MPP64_67		0x00000000

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
21	SW_Reset_		OUT
25      Phy_Int#                IN
28      SDI_WP                  IN
29      SDI_Status              IN
54-61   On GPP Connector        ?
62      Switch Interrupt        IN
63-65   Reserved from SW Board  ?
66      SW_BRD connected        IN

*/
#define RD_78460_GP_GPP_OUT_ENA_LOW	(~(BIT21 | BIT20))
#define RD_78460_GP_GPP_OUT_ENA_MID (~(BIT26 | BIT27))
#define RD_78460_GP_GPP_OUT_ENA_HIGH	(~(0x0))

#define RD_78460_GP_GPP_OUT_VAL_LOW	(BIT21 | BIT20)
#define RD_78460_GP_GPP_OUT_VAL_MID	(BIT26 | BIT27)
#define RD_78460_GP_GPP_OUT_VAL_HIGH	0x0

#define RD_78460_GP_GPP_POL_LOW		0x0
#define RD_78460_GP_GPP_POL_MID		0x0
#define RD_78460_GP_GPP_POL_HIGH	0x0




/**********************/
/* DB-AXP-CUSTOMER         */
/**********************/

#define RD_78460_CUSTOMER_MPP0_7             0x00000000
#define RD_78460_CUSTOMER_MPP8_15		0x00000000
#define RD_78460_CUSTOMER_MPP16_23		0x33000000
#define RD_78460_CUSTOMER_MPP24_31		0x11000000 /* bits[27:24] = 0x5 to enable PMm for CPU0 */
#define RD_78460_CUSTOMER_MPP32_39		0x11111111
#define RD_78460_CUSTOMER_MPP40_47		0x00221100 /* bits[3:0] = 0x3 to enable PM for CPU1 */
#define RD_78460_CUSTOMER_MPP48_55		0x00000003
#define RD_78460_CUSTOMER_MPP56_63		0x00000000 /* bits[7:4] = 0x1 to enable PM for CPU2/3 */
#define RD_78460_CUSTOMER_MPP64_67		0x00000000

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
21	SW_Reset_		OUT
25      Phy_Int#                IN
28      SDI_WP                  IN
29      SDI_Status              IN
54-61   On GPP Connector        ?
62      Switch Interrupt        IN
63-65   Reserved from SW Board  ?
66      SW_BRD connected        IN

*/
#define RD_78460_CUSTOMER_GPP_OUT_ENA_LOW	(~(BIT21))
#define RD_78460_CUSTOMER_GPP_OUT_ENA_MID	(~(0x0))
#define RD_78460_CUSTOMER_GPP_OUT_ENA_HIGH	(~(0x0))

#define RD_78460_CUSTOMER_GPP_OUT_VAL_LOW	(BIT21)
#define RD_78460_CUSTOMER_GPP_OUT_VAL_MID	0x0
#define RD_78460_CUSTOMER_GPP_OUT_VAL_HIGH	0x0

#define RD_78460_CUSTOMER_GPP_POL_LOW		0x0
#define RD_78460_CUSTOMER_GPP_POL_MID		0x0
#define RD_78460_CUSTOMER_GPP_POL_HIGH		0x0



#endif /* __INCmvBoardEnvSpech */
