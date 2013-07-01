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

MV_SERDES_CHANGE_M_PHY db88f78XX0BoardSerdesChangeMphy[]={
	/*  SERDES TYPE    Low REG OFFSET      Low REG VALUE,Hi REG OFS,      Hi REG VALUE      */
	{SERDES_UNIT_PEX, 	   	   0x0, (0x0F << 16)| 0x2a21,       0x0, (0x0F << 16)| 0x2a21},   /* PEX: Change of Slew Rate port0   */ 
	{SERDES_UNIT_PEX, 	   	   0x0, (0x4F << 16)| 0x6219,       0x0, (0x4F << 16)| 0x6219},   /* PEX: Change PLL BW port0    		*/ 
	{SERDES_UNIT_SATA,	   0x0083C,				  0x8a31,   0x0083C, 			   0x8a31},   /* SATA: Slew rate change port 0 	*/ 
	{SERDES_UNIT_SATA,	   0x00834,				  0xc928,   0x00834, 			   0xc928},   /* SATA: Slew rate change port 0 	*/ 
	{SERDES_UNIT_SATA,	   0x00838,				  0x30f0,   0x00838,               0x30f0},   /* SATA: Slew rate change port 0 	*/ 
	{SERDES_UNIT_SATA,	   0x00840,				  0x30f5,   0x00840,               0x30f5},   /* SATA: Slew rate change port 0 	*/ 
	{SERDES_UNIT_SGMII0,   0x00E18, 			  0x989F,   0x00E18, 			   0x989F},   /* SGMII: FFE setting Port0         */ 
	{SERDES_UNIT_SGMII0,   0x00E38, 			  0x10FA,   0x00E38, 			   0x10FA},   /* SGMII: SELMUP and SELMUF Port0   */ 
	{SERDES_UNIT_SGMII0,   0x00E34, 			  0xC968,   0x00E34, 			   0xC66C},   /* SGMII: Amplitude new setting gen2 Port3 */
	{SERDES_UNIT_QSGMII,   0x72E34, 			  0xaa58,   0x72E34, 			   0xaa58},   /* QSGMII: Amplitude and slew rate change  */ 
	{SERDES_UNIT_QSGMII,   0x72e38, 			  0x10aF,   0x72e38, 			   0x10aF},   /* QSGMII: SELMUP and SELMUF               */
	{SERDES_UNIT_QSGMII,   0x72e18, 			  0x98AC,   0x72e18, 			   0x98AC},   /* QSGMII: 0x72e18                         */
	{SERDES_UNIT_UNCONNECTED, 0,0}		/* Null terminated */
};

MV_BIN_SERDES_CFG db88f78XX0InfoBoardSerdesConfigValue[] = {
        /* Z1B */
        {MV_PEX_ROOT_COMPLEX, 0x32221111, 0x11111111, {PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4}, 0x0030, db88f78XX0BoardSerdesChangeMphy},      /* Default */
        {MV_PEX_ROOT_COMPLEX, 0x31211111, 0x11111111, {PEX_BUS_MODE_X1, PEX_BUS_MODE_X1, PEX_BUS_MODE_X4, PEX_BUS_MODE_X4}, 0x0030, db88f78XX0BoardSerdesChangeMphy},       /* PEX module */
        /* Z1A */
        {MV_PEX_ROOT_COMPLEX, 0x32220000, 0x00000000, {PEX_BUS_DISABLED, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED}, 0x0030, db88f78XX0BoardSerdesChangeMphy},   /* Default - Z1A */
        {MV_PEX_ROOT_COMPLEX, 0x31210000, 0x00000000, {PEX_BUS_DISABLED, PEX_BUS_MODE_X1, PEX_BUS_DISABLED, PEX_BUS_DISABLED}, 0x0030, db88f78XX0BoardSerdesChangeMphy}     /* PEX module - Z1A */
};

/*----------------------------------------------*/
/*----------------------------------------------*/

MV_BIN_SERDES_CFG db88f78XX0rev2InfoBoardSerdesConfigValue[] = {
	/* A0 */
    {MV_PEX_ROOT_COMPLEX, 0x33221111, 0x11111111, {PEX_BUS_MODE_X1, PEX_BUS_DISABLED, PEX_BUS_MODE_X4,PEX_BUS_MODE_X4}, 0x0030, db88f78XX0BoardSerdesChangeMphy },/* Default: No Pex module, PEX0 x1, disabled*/
	{MV_PEX_ROOT_COMPLEX, 0x33211111, 0x11111111, {PEX_BUS_MODE_X1, PEX_BUS_MODE_X1,  PEX_BUS_MODE_X4,PEX_BUS_MODE_X4}, 0x0030, db88f78XX0BoardSerdesChangeMphy },/* Pex module, PEX0 x1, PEX1 x1*/
	{MV_PEX_ROOT_COMPLEX, 0x33221111, 0x11111111, {PEX_BUS_MODE_X4, PEX_BUS_DISABLED, PEX_BUS_MODE_X4,PEX_BUS_MODE_X4}, 0x0030, db88f78XX0BoardSerdesChangeMphy },/* no Pex module, PEX0 x4, PEX1 disabled*/
	{MV_PEX_ROOT_COMPLEX, 0x33211111, 0x11111111, {PEX_BUS_MODE_X4, PEX_BUS_MODE_X1,  PEX_BUS_MODE_X4,PEX_BUS_MODE_X4}, 0x0030, db88f78XX0BoardSerdesChangeMphy },/* Pex module, PEX0 x4, PEX1 x1*/
	{MV_PEX_ROOT_COMPLEX, 0x11111111, 0x11111111, {PEX_BUS_MODE_X1, PEX_BUS_MODE_X4,  PEX_BUS_MODE_X4,PEX_BUS_MODE_X4}, 0x0030, db88f78XX0BoardSerdesChangeMphy },/* Pex module, PEX0 x1, PEX1 x4*/
	{MV_PEX_ROOT_COMPLEX, 0x11111111, 0x11111111, {PEX_BUS_MODE_X4, PEX_BUS_MODE_X4,  PEX_BUS_MODE_X4,PEX_BUS_MODE_X4}, 0x0030, db88f78XX0BoardSerdesChangeMphy },/* Pex module, PEX0 x4, PEX1 x4*/
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG rd78460nasInfoBoardSerdesConfigValue[] = {
	{MV_PEX_ROOT_COMPLEX, 0x00223001, 0x11111111, {PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4}, 0x0030, db88f78XX0BoardSerdesChangeMphy},	/* Default */
	{MV_PEX_ROOT_COMPLEX, 0x33320201, 0x11111111, {PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4}, 0x00f4, db88f78XX0BoardSerdesChangeMphy},	/* Switch module */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG rd78460InfoBoardSerdesConfigValue[] = {
	{MV_PEX_ROOT_COMPLEX, 0x22321111, 0x00000000, {PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED}, 0x0010, db88f78XX0BoardSerdesChangeMphy},	/* CPU0 */
	{MV_PEX_ROOT_COMPLEX, 0x00321111, 0x00000000, {PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED}, 0x0010, db88f78XX0BoardSerdesChangeMphy}	/* CPU1-3 */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG rd78460ServerRev2InfoBoardSerdesConfigValue[] = {
	{MV_PEX_ROOT_COMPLEX, 0x00321111, 0x00000000, {PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED}, 0x0010, db88f78XX0BoardSerdesChangeMphy},	/* CPU0 */
	{MV_PEX_ROOT_COMPLEX, 0x00321111, 0x00000000, {PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED}, 0x0010, db88f78XX0BoardSerdesChangeMphy}	/* CPU1-3 */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG db78X60pcacInfoBoardSerdesConfigValue[] = {
	 {MV_PEX_END_POINT, 0x22321111, 0x00000000, {PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED}, 0x0010, db88f78XX0BoardSerdesChangeMphy} /* Default */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG db78X60pcacrev2InfoBoardSerdesConfigValue[] = {
	 {MV_PEX_END_POINT, 0x23321111, 0x00000000, {PEX_BUS_MODE_X4, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED}, 0x0010, db88f78XX0BoardSerdesChangeMphy} /* Default */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG fpga88f78XX0InfoBoardSerdesConfigValue[] = {
	{MV_PEX_ROOT_COMPLEX, 0x00000000, 0x00000000, {PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED}, 0x0000, db88f78XX0BoardSerdesChangeMphy} /* No PEX in FPGA */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG db78X60amcInfoBoardSerdesConfigValue[] = {
	 {MV_PEX_ROOT_COMPLEX, 0x33111111,0x00010001, {PEX_BUS_MODE_X4, PEX_BUS_MODE_X1, PEX_BUS_MODE_X1, PEX_BUS_MODE_X1}, 0x0030, db88f78XX0BoardSerdesChangeMphy} /* Default */
};


/****************************/
/* ARMADA-XP CUSTOMER BOARD */
/****************************/

MV_BIN_SERDES_CFG rd78460customerInfoBoardSerdesConfigValue[] = {
	{MV_PEX_ROOT_COMPLEX, 0x00223001, 0x11111111, {PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4}, 0x00000030, db88f78XX0BoardSerdesChangeMphy},	/* Default */
	{MV_PEX_ROOT_COMPLEX, 0x33320201, 0x11111111, {PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4}, 0x00000030, db88f78XX0BoardSerdesChangeMphy},	/* Switch module */
};
/*----------------------------------------------*/

MV_BIN_SERDES_CFG rd78460AXP_GP_InfoBoardSerdesConfigValue[] = {
	 {MV_PEX_ROOT_COMPLEX, 0x00223001, 0x11111111, {PEX_BUS_MODE_X1, PEX_BUS_DISABLED, PEX_BUS_MODE_X4, PEX_BUS_MODE_X4}, 0x0030, db88f78XX0BoardSerdesChangeMphy} /* Default */
};
/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG* SerdesInfoTbl[] = {
	db88f78XX0InfoBoardSerdesConfigValue,
	rd78460InfoBoardSerdesConfigValue,
	db78X60pcacInfoBoardSerdesConfigValue,
	fpga88f78XX0InfoBoardSerdesConfigValue,
	db88f78XX0rev2InfoBoardSerdesConfigValue,
	rd78460nasInfoBoardSerdesConfigValue,
	db78X60amcInfoBoardSerdesConfigValue,
	db78X60pcacrev2InfoBoardSerdesConfigValue,
	rd78460ServerRev2InfoBoardSerdesConfigValue,
	rd78460AXP_GP_InfoBoardSerdesConfigValue,
	rd78460customerInfoBoardSerdesConfigValue
};


MV_U8 rd78460gpInfoBoardTwsiDev[] = {0x4C,0x4D,0x4E};
MV_U8 db88f78XX0rev2InfoBoardTwsiDev[] = {0x4C,0x4D,0x4E,0x4F};

