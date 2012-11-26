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


MV_BIN_SERDES_CFG db88f78XX0InfoBoardSerdesConfigValue[] = {
        /* Z1B */
        {MV_PEX_ROOT_COMPLEX, 0x32221111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x0030},      /* Default */
        {MV_PEX_ROOT_COMPLEX, 0x31211111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_MODE_X1,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x0030},       /* PEX module */
        /* Z1A */
        {MV_PEX_ROOT_COMPLEX, 0x32220000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x0030},   /* Default - Z1A */
        {MV_PEX_ROOT_COMPLEX, 0x31210000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_MODE_X1,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x0030}     /* PEX module - Z1A */
};

/*----------------------------------------------*/
/*----------------------------------------------*/

MV_BIN_SERDES_CFG db88f78XX0rev2InfoBoardSerdesConfigValue[] = {
	/* A0 */
       {MV_PEX_ROOT_COMPLEX, 0x33221111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED, PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x0030},/* Default: No Pex module, PEX0 x1, disabled*/
	{MV_PEX_ROOT_COMPLEX, 0x31211111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_MODE_X1, PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x0030},/* Pex module, PEX0 x1, PEX1 x1*/
	{MV_PEX_ROOT_COMPLEX, 0x33221111, 0x11111111, PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x0030},/* no Pex module, PEX0 x4, PEX1 disabled*/
	{MV_PEX_ROOT_COMPLEX, 0x31211111, 0x11111111, PEX_BUS_MODE_X4, PEX_BUS_MODE_X1,PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x0030},/* Pex module, PEX0 x4, PEX1 x1*/
	{MV_PEX_ROOT_COMPLEX, 0x11111111, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_MODE_X4,PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x0030},/* Pex module, PEX0 x1, PEX1 x4*/
	{MV_PEX_ROOT_COMPLEX, 0x11111111, 0x11111111, PEX_BUS_MODE_X4, PEX_BUS_MODE_X4,PEX_BUS_MODE_X4,PEX_BUS_MODE_X4, 0x0030},/* Pex module, PEX0 x4, PEX1 x4*/
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG rd78460nasInfoBoardSerdesConfigValue[] = {
	{MV_PEX_ROOT_COMPLEX, 0x00223001, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x0030},	/* Default */
	{MV_PEX_ROOT_COMPLEX, 0x33320201, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00f4},	/* Switch module */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG rd78460InfoBoardSerdesConfigValue[] = {
	{MV_PEX_ROOT_COMPLEX, 0x22321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x0010},	/* CPU0 */
	{MV_PEX_ROOT_COMPLEX, 0x00321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x0010}	/* CPU1-3 */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG db78X60pcacInfoBoardSerdesConfigValue[] = {
	 {MV_PEX_END_POINT, 0x22321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED,PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x0010} /* Default */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG db78X60pcacrev2InfoBoardSerdesConfigValue[] = {
	 {MV_PEX_END_POINT, 0x23321111, 0x00000000, PEX_BUS_MODE_X4, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x0010} /* Default */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG fpga88f78XX0InfoBoardSerdesConfigValue[] = {
	{MV_PEX_ROOT_COMPLEX, 0x00000000, 0x00000000, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED, PEX_BUS_DISABLED, 0x0000} /* No PEX in FPGA */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG db78X60amcInfoBoardSerdesConfigValue[] = {
	 {MV_PEX_ROOT_COMPLEX, 0x23111111, 0x11111111, PEX_BUS_MODE_X4, PEX_BUS_MODE_X1, PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x0030} /* Default */
};


/****************************/
/* ARMADA-XP CUSTOMER BOARD */
/****************************/

MV_BIN_SERDES_CFG rd78460customerInfoBoardSerdesConfigValue[] = {
	{MV_PEX_ROOT_COMPLEX, 0x00223001, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00000030},	/* Default */
	{MV_PEX_ROOT_COMPLEX, 0x33320201, 0x11111111, PEX_BUS_MODE_X1, PEX_BUS_DISABLED,PEX_BUS_MODE_X4, PEX_BUS_MODE_X4, 0x00000030},	/* Switch module */
};

/*----------------------------------------------*/
/*----------------------------------------------*/
MV_BIN_SERDES_CFG* SerdesInfoTbl[] = {
	&db88f78XX0InfoBoardSerdesConfigValue,
	&rd78460InfoBoardSerdesConfigValue,
	&db78X60pcacInfoBoardSerdesConfigValue,
	&fpga88f78XX0InfoBoardSerdesConfigValue,
	&db88f78XX0rev2InfoBoardSerdesConfigValue,
	&rd78460nasInfoBoardSerdesConfigValue,
	&db78X60amcInfoBoardSerdesConfigValue,
	&db78X60pcacrev2InfoBoardSerdesConfigValue,
	&rd78460customerInfoBoardSerdesConfigValue
};
#include "bin_hdr_twsi.h"

MV_BOARD_BIN_TWSI_INFO SatInfoTbl[] = {
	{1,0x4D, ADDR7_BIT},//db88f78XX0InfoBoardSatTWSIValue
	{0,0,0},//rd78460InfoBoardSatTWSIValue,
	{1,0x4D, ADDR7_BIT},//db78X60pcacInfoBoardSatTWSIValue,
	{1,0x4D, ADDR7_BIT},//fpga88f78XX0InfoBoardSatTWSIValue,
	{1,0x4D, ADDR7_BIT},//db88f78XX0rev2InfoBoardSatTWSIValue,
	{0,0,0},//rd78460nasInfoBoardSatTWSIValue,
	{0,0,0},//db78X60amcInfoBoardSatTWSIValue,
	{0,0,0}//db78X60pcacrev2InfoBoardSatTWSIValue
};






