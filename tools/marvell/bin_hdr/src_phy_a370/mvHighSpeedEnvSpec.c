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
MV_BIN_SERDES_CFG db88f6710InfoBoardSerdesConfigValue[] = {
/*	.boardName				= "DB-88F6710-BP", */
	{ MV_PEX_ROOT_COMPLEX, 0x00001111, MV_TRUE, {PEX_BUS_MODE_X1, PEX_BUS_MODE_X1}, 0xc, db88f78XX0BoardSerdesChangeMphy}
};
MV_BIN_SERDES_CFG db88f6710pcacInfoBoardSerdesConfigValue[] = {
/*	.boardName					= "DB-88F6710-PCAC", */
	/* DB6710 PCAC Serdes static configuration */
	{ MV_PEX_END_POINT, 0x000021, MV_FALSE, {PEX_BUS_MODE_X1, PEX_BUS_DISABLED}, 0x2, db88f78XX0BoardSerdesChangeMphy}
};
MV_BIN_SERDES_CFG rd88F6710InfoBoardSerdesConfigValue[] = {
/*	.boardName				= "RD-88F6710", */
	{ MV_PEX_ROOT_COMPLEX, 0x00001211, MV_FALSE, {PEX_BUS_MODE_X1, PEX_BUS_MODE_X1}, 0x8, db88f78XX0BoardSerdesChangeMphy}
};


/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG* SerdesInfoTbl[] = {
db88f6710InfoBoardSerdesConfigValue,
db88f6710pcacInfoBoardSerdesConfigValue,
rd88F6710InfoBoardSerdesConfigValue
};



