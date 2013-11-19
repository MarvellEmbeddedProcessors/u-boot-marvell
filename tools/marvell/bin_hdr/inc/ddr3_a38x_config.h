
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


#ifndef _DDR3_A380_CONFIG_H
#define _DDR3_A380_CONFIG_H

#define SPEED_BIN		SPEED_BIN_DDR_1866L
#define DDR_BUS_WIDTH		BUS_WIDTH_8
#define MEM_SIZE		MEM_4G


//Igorp
/*DDR3_LOG_LEVEL Information
Level 0: Provides an error code in a case of failure, RL, WL errors and other algorithm failure
Level 1: Provides the D-Unit setup (SPD/Static configuration)
Level 2: Provides the windows margin as a results of DQS centeralization
Level 3: Provides the windows margin of each DQ as a results of DQS centeralization */

#define	DDR3_LOG_LEVEL	0
#define DDR3_PBS        1
/* this flag allows the execution of SW WL/RL oppon HW failure */
#define DDR3_RUN_SW_WHEN_HW_FAIL    1

/* General Configurations */
/* The following parameters are required for proper setup */
/* DDR_TARGET_FABRIC - Set desiered fabric configuration (for sample@Reset fabfreq parameter) */
/* DRAM_ECC - set ECC support TRUE/FALSE */
/* BUS_WIDTH - 64/32 bit */
/* SPD_SUPPORT - Enables auto detection of DIMMs and their timing values */
/* DQS_CLK_ALIGNED - Set this if CLK and DQS signals are aligned on board */
/* MIXED_DIMM_STATIC - Mixed DIMM + On board devices support (ODT registers values are taken statically) */
/* DDR3_TRAINING_DEBUG - debug prints of internal code */
#define DDR_TARGET_FABRIC						5
#define DRAM_ECC								FALSE

#ifdef MV_DDR_32BIT
#define BUS_WIDTH                               32
#else
#define BUS_WIDTH								64
#endif
#define SPD_SUPPORT
#undef DQS_CLK_ALIGNED
#undef MIXED_DIMM_STATIC
#define DDR3_TRAINING_DEBUG						FALSE
#define REG_DIMM_SKIP_WL						FALSE

#ifdef DRAM_ECC
/* ECC support parameters: */
/* U_BOOT_START_ADDR, U_BOOT_SCRUB_SIZE - relevant when using ECC and need to configure the scrubbing area */
#define TRAINING_SIZE                           0x20000
#define U_BOOT_START_ADDR						0
#define U_BOOT_SCRUB_SIZE                       0x1000000 /*- TRAINING_SIZE*/
#endif
/* Registered DIMM Support - In case registered DIMM is attached, please supply the following values:
(see JEDEC - JESD82-29A "Definition of the SSTE32882 Registering Clock Driver with Parity and Quad Chip
Selects for DDR3/DDR3L/DDR3U RDIMM 1.5 V/1.35 V/1.25 V Applications") */
/* RC0: Global Features Control Word */
/* RC1: Clock Driver Enable Control Word */
/* RC2: Timing Control Word */
/* RC3-RC5 - taken from SPD */
/* RC8: Additional IBT Setting Control Word */
/* RC9: Power Saving Settings Control Word */
/* RC10: Encoding for RDIMM Operating Speed */
/* RC11: Operating Voltage VDD and VREFCA Control Word */
#define RDIMM_RC0								0
#define RDIMM_RC1								0
#define RDIMM_RC2								0
#define RDIMM_RC8								0
#define RDIMM_RC9								0
#define RDIMM_RC10								0x2
#define RDIMM_RC11								0x0

//------------STUBS-----------------
MV_U32 s_auiCpuFabClkToHClk[1][1];
MV_U8 s_auiDivRatio2to1[1][1];
MV_U16 auiODTStatic[1][1];
MV_U8 s_auiDivRatio1to1[1][1];
//------------/STUBS------------------
//Endof Igorp


#endif /* _DDR3_A380_CONFIG_H */
