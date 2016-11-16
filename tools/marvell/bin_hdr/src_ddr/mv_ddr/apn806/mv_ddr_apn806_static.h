/*******************************************************************************
Copyright (C) 2016 Marvell International Ltd.

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the three
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 2 of the License, or any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************************
Marvell GNU General Public License FreeRTOS Exception

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the Lesser
General Public License Version 2.1 plus the following FreeRTOS exception.
An independent module is a module which is not derived from or based on
FreeRTOS.
Clause 1:
Linking FreeRTOS statically or dynamically with other modules is making a
combined work based on FreeRTOS. Thus, the terms and conditions of the GNU
General Public License cover the whole combination.
As a special exception, the copyright holder of FreeRTOS gives you permission
to link FreeRTOS with independent modules that communicate with FreeRTOS solely
through the FreeRTOS API interface, regardless of the license terms of these
independent modules, and to copy and distribute the resulting combined work
under terms of your choice, provided that:
1. Every copy of the combined work is accompanied by a written statement that
details to the recipient the version of FreeRTOS used and an offer by yourself
to provide the FreeRTOS source code (including any modifications you may have
made) should the recipient request it.
2. The combined work is not itself an RTOS, scheduler, kernel or related
product.
3. The independent modules add significant and primary functionality to
FreeRTOS and do not merely extend the existing functionality already present in
FreeRTOS.
Clause 2:
FreeRTOS may not be used for any competitive or comparative purpose, including
the publication of any form of run time or compile time metric, without the
express permission of Real Time Engineers Ltd. (this is the norm within the
industry and is intended to ensure information accuracy).

********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

	* Neither the name of Marvell nor the names of its contributors may be
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

#ifndef _MV_DDR_APN806_STATIC_H
#define _MV_DDR_APN806_STATIC_H

#if defined(CONFIG_PHY_STATIC) || defined(CONFIG_MC_STATIC)

#define MCK6_DRAM_STATUS_REG		(0x8)
#define INIT_DONE0_CH0			(0x1)

#define MCK6_USER_COMMAND_0_REG		(0x20)
#define SDRAM_INIT_REQ_MASK		(0x1)
#define CMD_CH_ENABLE(c)		(1 << (28 + c))
#define CMD_CS_MASK(m)			((m) << 24)

#define MCK6_CTRL_0_REG			(0x44)
#define CTRL_DATA_WIDTH_OFFSET		8
#define CTRL_DATA_WIDTH_MASK		(0xF << 8)
#define BUS_WIDTH_2_IDX(w)		(((w) <= 16) ? ((w) / 8) : (((w) / 32) + 2))
#define CTRL_DATA_WIDTH(w)		(BUS_WIDTH_2_IDX(w) << 8)
#define CTRL_DATA_WIDTH_CALC(v)		(1 << ((v) + 2))

#define MCK6_MMAP0_LOW_CH(i)		(0x200 + 8*(i))
#define MMAP_AREA_LEN_OFFSET		16
#define MMAP_AREA_LEN_MASK		(0x1F << 16)
#define MMAP_AREA_LEN(x)		((x) << 16)

#define DRAM_INIT_TIMEOUT		1000

/* DLL Tune definitions */
#define DLL_PHSEL_START			0x00
#define DLL_PHSEL_END			0x3F
#define DLL_PHSEL_STEP			0x1
#define DLL_RST_TIMER_VAL		0x10
#define DLL_RESET_WAIT_US		100
#define DLL_UPDATE_WAIT_US		50

#define MC6_CH0_PHY_CONTROL_8		(0x1C)
#define DLL_AUTO_UPDATE_EN		(BIT2)
#define DLL_AUTO_MANUAL_UPDATE		(BIT3)
#define DLL_RESET_TIMER(m)		((m & 0xFF) << 24)

#define MC6_CH0_PHY_CONTROL_9		(0x20)
#define PHY_DLL_RESET			(BIT29)
#define DLL_UPDATE_EN_PULSE		(BIT30)

#define MC6_CH0_PHY_DLL_CONTROL_B0	(0x50)
#define DLL_PHASE_POS_SHIFT		(16)
#define DLL_PHASE_NEG_SHIFT		(24)
#define DLL_PHASE_SZ_MASK		(0x3F)

#define DRAM_DIRECT_START		(0x6000000)
#define DRAM_DIRECT_SIZE		(2048)

struct mk6_reg_data {
	uint32_t offset;
	uint32_t value;
};

#if defined(a70x0)
struct mk6_reg_data ddr_static_setup[] = {
	/* Traffic thru Mckinley 6::MC init for MISL BOARD 2CS 4Gb x8 devices of micron - 2133P */
	/*
	 * the two registers below are needed when enabling the training through the dunit
	 * {0xF0000030, 0x00000003}, set private ID
	 * {0xF000070C, 0x000000FF}, enable SMC for STX access
	 */
	/*{0xF06F0108, 0xD88FFFF}, neg edge */
	{0xF00114C8, 0x1840008},
	{0xF00117C8, 0x28A0008},
	{0xF0011DC8, 0x1840008},
	{0xF0011EC8, 0x28A0008},
	{0xf00116a0, 0xE83F0001},
	{0xF00114CC, 0x1200D},
	{0xF0011400, 0x6b144c30},
	{0xF0011400, 0x7b144c30},
	{0xF0011538, 0xD0D},
	{0xF001153C, 0x1515},

	{0xF0020064, 0x606},		/* MC "readReady"+ MC2PHYlatency */
	{0xF0021180, 0x500},
	{0xF0021000, 0x60},		/* phy_rfifo_rptr_dly_val */
	{0xF00210c0, 0x81000001},
	{0xF00202c8, 0xfefe},

	{0xf00116a0, 0xcc000000},	/* CKdelayADLL */
	{0xf00116a0, 0xc0009c5f},
	{0xf00116a0, 0xc0405c8f},
	{0xf00116a0, 0xc0804c4b},
	{0xf00116a0, 0xc0c02040},
	{0xf00116a0, 0xc0020108},
	{0xf00116a0, 0xc0420109},
	{0xf00116a0, 0xc08200Cf},
	{0xf00116a0, 0xc0c200de},
	{0xf00116a0, 0xc0030007},
	{0xf00116a0, 0xc043000A},
	{0xf00116a0, 0xc0830008},
	{0xf00116a0, 0xc0c30008},
	{0xf00116a0, 0xc0040044},	/* WLpup0CS1 */
	{0xf00116a0, 0xc044004E},	/* WLpup1 */
	{0xf00116a0, 0xc0840010},	/* WLpup2 */
	{0xf00116a0, 0xc0c4000B},	/* WLpup3 */
	{0xf00116a0, 0xc006004B},	/* RLpup0 */
	{0xf00116a0, 0xc046004C},	/* RLpup1 */
	{0xf00116a0, 0xc0860015},	/* RLpup2 */
	{0xf00116a0, 0xc0c60046},	/* RLpup3 */
	{0xf00116a0, 0xc005000E},	/* Ctxpup0 */
	{0xf00116a0, 0xc0450018},	/* Ctxpup1 */
	{0xf00116a0, 0xc085001A},	/* Ctxpup2 */
	{0xf00116a0, 0xc0c50015},	/* Ctxpup3 */
	{0xf00116a0, 0xE8021503},	/* ADLL 0x92 */
	{0xf00116a0, 0xE8130050},	/* ADLL */
	{0xf00116a0, 0xE8140012},	/* ADLL */
	{0xf00116a0, 0xEC021503},	/* ADLL */
	{0xf00116a0, 0xEC130050},	/* ADLL */
	{0xf00116a0, 0xEC140012},	/* ADLL */
	{0xf00116a0, 0xF81B0020},	/* datareceivercalibrationBC */
	{0xf00116a0, 0xE8280427},	/* vrefconfigurationBC */
	{0xf00116a0, 0xEC24070C},	/* ctrldrivestrengthBC */
	{0xf00116a0, 0xE824070E},	/* datadrivestrengthBC */
	{0xf00116a0, 0xE8260200},	/* dataODTBC */

	{0xF0011400, 0x7b144c30},
	{0xF0011404, 0x36301848},
	{0xF0011408, 0x5411cbb9},
	{0xF001140c, 0x784A1f97},
	{0xF0011410, 0x17114444},
	{0xF0011414, 0x7c8},
	{0xF0011424, 0x60f3f7},
	{0xF001142c, 0x14c5138},
	{0xF0011498, 0xf},
	{0xF001149c, 0x303},
	{0xF00114e0, 0x400},
	{0xF00114e4, 0x3ff},
	{0xF00114e8, 0x7f01ff},
	{0xF00114ec, 0x54},
	{0xF00115e0, 0x23},
	{0xF00115e4, 0x203c18},
	{0xF00115ec, 0xd9ff0029},
	{0xF0011900, 0x314},		/* MR0 */
	{0xF0011904, 0x001},		/* MR1 */
	{0xF0011908, 0x200},		/* MR2 */
	{0xF001190c, 0x800},		/* MR3 */
	{0xF0011910, 0x0},		/* MR4 */
	{0xF0011914, 0x4A0},		/* MR5 */
	{0xF0011918, 0x80A},		/* MR6 */

	{0xf0020340, 0x0},
	{0xf0020344, 0x30000000},
	{0xf0020310, 0x21000000},
	{0xf0020314, 0x21010000},
	{0xf0020318, 0x0},
	{0xf002031c, 0x0},
	{0xf0020304, 0x0},
	{0xf0020308, 0x1},
	{0xf0020200, 0xF0001},
	{0xf0020204, 0x0},
	{0xf0020208, 0x800F0001},
	{0xf002020c, 0x0},
	{0xf0020210, 0x0},
	{0xf0020214, 0x0},
	{0xf0020218, 0x0},
	{0xf002021c, 0x0},
	{0xf0020400, 0xF0001},
	{0xf0020404, 0x1},
	{0xf0020408, 0x800F0001},
	{0xf002040c, 0x1},
	{0xf0020220, 0x5010539},
	{0xf0020224, 0x5010539},
	{0xf0020228, 0x0},
	{0xf002022c, 0x0},
	{0xf0020044, 0x30300},
	{0xf00202c0, 0x6000},
	{0xf00202c4, 0x120030},
	{0xf0020180, 0x30200},
	{0xf0020050, 0xff},
	{0xf002004c, 0x2},	/*{0xf002004c, 0x0 = ECC disable},*/
	{0xf0020054, 0x4c0},
	{0xf0020300, 0x90c},
	{0xf002030c, 0x90000},
	{0xf0020380, 0x61a80},
	{0xf0020384, 0x27100},
	{0xf0020388, 0x960050},
	{0xf002038c, 0x400},
	{0xf0020390, 0x800200},
	{0xf0020394, 0xd00618},
	{0xf0020398, 0x1680300},
	{0xf002039c, 0x200808},
	{0xf00203a0, 0x1050600},
	{0xf00203a4, 0x2},
	{0xf00203a8, 0x1808},
	{0xf00203ac, 0x11260C1B},
	{0xf00203b0, 0x0C0C060C},
	{0xf00203b4, 0x06040602},
	{0xf00203b8, 0x404},
	{0xf00203bc, 0x1050505},
	{0xf00203c0, 0x10400},
	{0xf00203c4, 0x0},
	{0xf00203cc, 0xf010345},
	/* FIXME: add the two registers configuration below when fixing the timing
	 * algorithm also remove it from the main flow function and from the
	 * static function
	 */
	/* {0xF0011480, 0x1},		Tipinit */
	/* {0xF0020020, 0x13000001},	MCinit */
	{0xf06f812C, 0xFDE1FFFF},	/* FIXME: extension avs */
	{0xf06f8130, 0x1002f2f5},	/* FIXME: avs to 1.13V */
	{-1, -1},
};
#elif defined(a70x0_cust)
struct mk6_reg_data ddr_static_setup[] = {
	/* Traffic thru Mckinley 6::MC init for CISCO with 1CS 8Gb devices of Samsung */
	/*
	 * the two registers below are needed when enabling the training through the dunit
	 * {0xF0000030, 0x00000003}, set private ID
	 * {0xF000070C, 0x000000FF}, enable SMC for STX access
	 */
	{0xF00114C8, 0x5145941},
	{0xF00117C8, 0x15145941},
	{0xF0011DC8, 0x5145941},
	{0xF0011EC8, 0x15145941},
	{0xF00114CC, 0x12015},
	{0xF0011538, 0xB},
	{0xF001153C, 0x11},
	{0xF0020064, 0x15},	/* MC"readReady"+MC2PHYlatency */
	{0xF0021180, 0x000},
	{0xF0021000, 0xC0},	/* phy_rfifo_rptr_dly_val */
	{0xf00116a0, 0xcc000000},	/* CKdelayADLL */
	{0xf00116a0, 0xc0000017},	/* WLpup0 */
	{0xf00116a0, 0xc0400012},	/* WLpup1 */
	{0xf00116a0, 0xc080000d},	/* WLpup2 */
	{0xf00116a0, 0xc0c00018},	/* WLpup3 */
	{0xf00116a0, 0xc0020059},	/* RLpup0 */
	{0xf00116a0, 0xc0420053},	/* RLpup1 */
	{0xf00116a0, 0xc082004d},	/* RLpup2 */
	{0xf00116a0, 0xc0c20084},	/* RLpup3 */
	{0xf00116a0, 0xc0010021},	/* Ctxpup0 */
	{0xf00116a0, 0xc041001c},	/* Ctxpup1 */
	{0xf00116a0, 0xc0810017},	/* Ctxpup2 */
	{0xf00116a0, 0xc0c1002a},	/* Ctxpup3 */
	{0xf00116a0, 0xE8021503},	/* ADLL 0x92 */
	{0xf00116a0, 0xE8130050},	/* ADLL */
	{0xf00116a0, 0xE8140012},	/* ADLL */
	{0xf00116a0, 0xEC021503},	/* ADLL */
	{0xf00116a0, 0xEC130050},	/* ADLL */
	{0xf00116a0, 0xEC140012},	/* ADLL */
	{0xf00116a0, 0xF81B0020},	/* datareceivercalibrationBC */
	{0xf00116a0, 0xE8280427},	/* vrefconfigurationBC */
	{0xf00116a0, 0xEC24070C},	/* ctrldrivestrengthBC */
	{0xf00116a0, 0xE824070E},	/* datadrivestrengthBC */
	{0xf00116a0, 0xE8260200},	/* dataODTBC */
	{0xf0020340, 0x0},
	{0xf0020344, 0x10000000},
	{0xf0020310, 0x21010000},
	{0xf0020314, 0x0},
	{0xf0020318, 0x0},
	{0xf002031c, 0x0},
	{0xf0020304, 0x0},
	{0xf0020308, 0x1},
	{0xf0020200, 0x100001},
	{0xf0020204, 0x0},
	{0xf0020208, 0x0},
	{0xf002020c, 0x0},
	{0xf0020210, 0x0},
	{0xf0020214, 0x0},
	{0xf0020218, 0x0},
	{0xf002021c, 0x0},
	{0xf0020400, 0x100001},
	{0xf0020404, 0x1},
	{0xf0020408, 0x0},
	{0xf002040c, 0x0},
	{0xf0020220, 0x5010539},
	{0xf0020224, 0x0},
	{0xf0020228, 0x0},
	{0xf002022c, 0x0},
	{0xf0020044, 0x30300},
	{0xf00202c0, 0x6000},
	{0xf00202c4, 0x100030},
	{0xf0020180, 0x30200},
	{0xf0020050, 0xff},
	{0xf002004c, 0x2},	/*{0xf002004c, 0x0 = ECC disable},*/
	{0xf0020054, 0x4c0},
	{0xf0020300, 0x90b},
	{0xf002030c, 0x90000},
	{0xf0020380, 0x61a80},
	{0xf0020384, 0x27100},
	{0xf0020388, 0x960050},
	{0xf002038c, 0x400},
	{0xf0020390, 0x800200},
	{0xf0020394, 0x1180618},
	{0xf0020398, 0x1680300},
	{0xf002039c, 0x200808},
	{0xf00203a0, 0x1050600},
	{0xf00203a4, 0x2},
	{0xf00203a8, 0x1808},
	{0xf00203ac, 0x11250C1B},
	{0xf00203b0, 0x0C0C060C},
	{0xf00203b4, 0x06040602},
	{0xf00203b8, 0x404},
	{0xf00203bc, 0x1050505},
	{0xf00203c0, 0x10400},
	{0xf00203c4, 0x0},
	{0xf00203cc, 0xf010345},
	/* FIXME: add the two registers configuration below when fixing the timing
	 * algorithm also remove it from the main flow function and from the
	 * static function
	 */
	/* {0xF0011480, 0x1},		Tipinit */
	/* {0xF0020020, 0x13000001},	MCinit */
	{0xf06f812C, 0xFDE1FFFF},	/* FIXME: extension avs */
	{0xf06f8130, 0x1002f2f5},	/* FIXME: avs to 1.13V */
	{-1, -1},
};
#elif defined(a80x0)
struct mk6_reg_data ddr_static_setup[] = {
	/* Traffic thru MC6::TIP init for Marvell with 1CS 8Gb x4 devices of Micron 2400T */
	/*
	 * the two registers below are needed when enabling the training through the dunit
	 * {0xF0000030, 0x00000003}, set private ID
	 * {0xF000070C, 0x000000FF}, enable SMC for STX access
	 */
	{0xF00114C8, 0x1840008},
	{0xF00117C8, 0x28A0008},
	{0xF0011DC8, 0x1840008},
	{0xF0011EC8, 0x28A0008},
	{0xf00116a0, 0xE83F0001},
	{0xF00114CC, 0x1200D},
#if defined(CONFIG_64BIT)
	{0xF0011400, 0x6b14cc30},
	{0xF0011400, 0x7b14cc30},
#else
	{0xF0011400, 0x6b144c30},
	{0xF0011400, 0x7b144c30},
#endif
	{0xF0011538, 0xD0D},
	{0xF001153C, 0x1515},

	{0xF0020064, 0x606},		/* MC "readReady"+ MC2PHYlatency */
	{0xF0021180, 0x500},
	{0xF0021000, 0x60},		/* phy_rfifo_rptr_dly_val */
	{0xF00210c0, 0x81000001},
	{0xF00202c8, 0xfefe},

	/* FIXME: unremark the register below "0xf00116a0, 0xcc000000"
	 * and set the correct value for the clk delay after implementing
	 * the supplementary algorithm
	 */
	/* {0xf00116a0, 0xcc000000},	CKdelayADLL */

	{0xf00116a0, 0xc0009c5f},
	{0xf00116a0, 0xc0405c8f},
	{0xf00116a0, 0xc0804c4b},
	{0xf00116a0, 0xc0c02040},
	{0xf00116a0, 0xc0020108},
	{0xf00116a0, 0xc0420109},
	{0xf00116a0, 0xc08200Cf},
	{0xf00116a0, 0xc0c200de},
	{0xf00116a0, 0xc0030007},
	{0xf00116a0, 0xc043000A},
	{0xf00116a0, 0xc0830008},
	{0xf00116a0, 0xc0c30008},
	{0xf00116a0, 0xc0040044},	/* WLpup0CS1 */
	{0xf00116a0, 0xc044004E},	/* WLpup1 */
	{0xf00116a0, 0xc0840010},	/* WLpup2 */
	{0xf00116a0, 0xc0c4000B},	/* WLpup3 */
	{0xf00116a0, 0xc006004B},	/* RLpup0 */
	{0xf00116a0, 0xc046004C},	/* RLpup1 */
	{0xf00116a0, 0xc0860015},	/* RLpup2 */
	{0xf00116a0, 0xc0c60046},	/* RLpup3 */
	{0xf00116a0, 0xc005000E},	/* Ctxpup0 */
	{0xf00116a0, 0xc0450018},	/* Ctxpup1 */
	{0xf00116a0, 0xc085001A},	/* Ctxpup2 */
	{0xf00116a0, 0xc0c50015},	/* Ctxpup3 */
	{0xf00116a0, 0xE8021503},	/* ADLL 0x92 */
	{0xf00116a0, 0xE8130050},	/* ADLL */
	{0xf00116a0, 0xE8140012},	/* ADLL */
	{0xf00116a0, 0xEC021503},	/* ADLL */
	{0xf00116a0, 0xEC130050},	/* ADLL */
	{0xf00116a0, 0xEC140012},	/* ADLL */
	{0xf00116a0, 0xF81B0020},	/* datareceivercalibrationBC */
	/* {0xf00116a0, 0xE8280428},	 vrefconfigurationBC */
	{0xf00116a0, 0xE8280427},	/* for debug  vref*/

	{0xf00116a0, 0xEC24070C},	/* ctrldrivestrengthBC */

	/* {0xf00116a0, 0xE824070E},	datadrivestrengthBC */
	{0xf00116a0, 0xE8240489},

	/* {0xf00116a0, 0xE8260040},	dataODTBC */
	{0xf00116a0, 0xE82601C0},	/* for debug termination */

	/* FIXME: run over calibration values in static array patch */
	{0xf00114c8, 0x018F0008},/* 6,F		H SSTL */
	{0xf00117c8, 0x020E0008},/* 8,E		H-POD	OB: The driving strength was too strong */
	{0xf0011dc8, 0x018F0008},/* 6,F		V  SSTL */
	{0xf0011ec8, 0x01CC0008},/* 7,C		V-POD */

	{0xf00116a0, 0xEC24050A},/* 5,10	ctrldrivestrengthBC 34Ohm drive (the DIMM is 5Ohm */
	{0xf00116a0, 0xE824070E},/* 7,14	datadrivestrengthBC  set 34Ohm drive */
	{0xf00116a0, 0xE8260200},/* 6,0		dataODTBC single CS set termination of 60Ohm */
	/* FIXME: run over calibration values in static array patch */

	{0xF0011404, 0x5321BAA4},	/* 0x36301848 */
	{0xF0011408, 0x5411cbb9},
	{0xF001140c, 0x784A1f97},
	{0xF0011410, 0x17114444},
	{0xF0011414, 0x7c8},
	{0xF0011424, 0x60f3f7},
	{0xF001142c, 0x14c5138},
	{0xF0011498, 0xf},
	{0xF001149c, 0x303},
	{0xF00114e0, 0x400},
	{0xF00114e4, 0x3ff},
	{0xF00114e8, 0x7f01ff},
	{0xF00114ec, 0x54},
	{0xF00115e0, 0x23},
	{0xF00115e4, 0x203c18},
	{0xF00115ec, 0xd9ff0029},
	{0xF0011900, 0x314},	/* MR0 */
	{0xF0011904, 0x001},	/* MR1 */
	{0xF0011908, 0x200},	/* MR2 */
	{0xF001190c, 0x800},	/* MR3 */
	{0xF0011910, 0x0},	/* MR4 */
	{0xF0011914, 0x4A0},	/* MR5 */
	{0xF0011918, 0x80A},	/* MR6 */

	{0xf0020340, 0x0},
	{0xf0020344, 0x30000000},
	{0xf0020310, 0x21000000},
	{0xf0020314, 0x0},	/* TODO: for 2 cs add this configuration register */
	{0xf0020318, 0x0},
	{0xf002031c, 0x0},
	{0xf0020304, 0x0},
	{0xf0020308, 0x1},
	{0xf0020200, 0x100001}, /* mmap0_low_ch0 */
	{0xf0020204, 0x0},
	{0xf0020208, 0x0},
	{0xf002020c, 0x0},
	{0xf0020210, 0x0},
	{0xf0020214, 0x0},
	{0xf0020218, 0x0},
	{0xf002021c, 0x0},
	{0xf0020400, 0x100001},
	{0xf0020404, 0x1},
	{0xf0020408, 0x0},
	{0xf002040c, 0x0},
	{0xf0020220, 0x5010539},
	{0xf0020224, 0x0},
	{0xf0020228, 0x0},
	{0xf002022c, 0x0},
#if defined(CONFIG_64BIT)
	{0xf0020044, 0x30400}, /* mc control register 0 */
#else
	{0xf0020044, 0x30300},
#endif
	{0xf00202c0, 0x6000},
	{0xf00202c4, 0x120030},
	{0xf0020180, 0x30200},
	{0xf0020050, 0xff},
	{0xf002004c, 0x2},	/*{0xf002004c, 0x0 = ECC disable},*/
	{0xf0020054, 0x4c0},
	{0xf0020300, 0x90b},
	{0xf002030c, 0x90000},
	{0xf0020380, 0x61a80},
	{0xf0020384, 0x27100},
	{0xf0020388, 0x960050},
	{0xf002038c, 0x400},
	{0xf0020390, 0x800200},
	{0xf0020394, 0x1180618},
	{0xf0020398, 0x1680300},
	{0xf002039c, 0x200808},
	{0xf00203a0, 0x1050600},
	{0xf00203a4, 0x2},
	{0xf00203a8, 0x1808},
	{0xf00203ac, 0x11250b1B},	/*0x11260C1B RAS fix*/
	{0xf00203b0, 0x0C0C060C},
	{0xf00203b4, 0x06040602},
	{0xf00203b8, 0x504},
	{0xf00203bc, 0x1050505},
	{0xf00203c0, 0x10400},		/*0x30700*/
	{0xf00203c4, 0x0},
	{0xf00203cc, 0xf010345},
	/* {0xF06F8130, 0x1002A2A5},	AVS*/
	/* FIXME: add the two registers configuration below when fixing the timing
	 * algorithm also remove it from the main flow function and from the
	 * static function
	 */
	/* {0xF0011480, 0x1},		Tipinit */
	/* {0xF0020020, 0x13000001},	MCinit */
	{0xf06f812C, 0xFDE1FFFF},	/* FIXME: extension avs */
	{0xf06f8130, 0x1002f2f5},	/* FIXME: avs to 1.13V */
	{-1, -1},
};
#elif defined(a80x0_cust)
struct mk6_reg_data ddr_static_setup[] = {
	{-1, -1},
};
#endif /* a80x0_cust */
#endif /* defined(CONFIG_PHY_STATIC) || defined(CONFIG_MC_STATIC) */
#endif /* _MV_DDR_APN806_STATIC_H */
