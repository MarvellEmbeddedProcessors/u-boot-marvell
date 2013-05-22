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

#ifndef _INC_A370_CONFIG_H
#define _INC_A370_CONFIG_H

/*DDR3_LOG_LEVEL Information
Level 0: Provides an error code in a case of failure, RL, WL errors and other algorithm failure
Level 1: Provides the D-Unit setup (SPD/Static configuration)
Level 2: Provides the windows margin as a results of DQS centeralization
Level 3: Provides the windows margin of each DQ as a results of DQS centeralization */

#define DDR3_LOG_LEVEL  0
#define DDR3_PBS        0

/* General Configurations */
/* The following parameters are required for proper setup */
/* DRAM_ECC - Must be set to FALSE */
/* DQS_CLK_ALIGNED - Set this if CLK and DQS signals are aligned on board */
/* DDR3_TRAINING_DEBUG - debug prints of internal code */
/* A370_A0 - must be defined if using Marvell DB board and A370 A0 device otherwise dont care */
#define DUNIT_STATIC
#define DRAM_ECC								FALSE
#undef DQS_CLK_ALIGNED
#define DDR3_TRAINING_DEBUG						FALSE
#undef A370_A0
#define REG_DIMM_SKIP_WL						TRUE

/* Marvell boards specific configurations */
#if defined(DB_88F6710_PCAC)
#define STATIC_TRAINING
#endif

#if defined(DB_88F6710) && !defined(A370_A0)
#define AUTO_DETECTION_SUPPORT
#define SPD_SUPPORT
#define DRAM_2T									0x0
#define DIMM_CS_BITMAP							0xF
#define DUNIT_SPD
#undef DUNIT_STATIC
#endif

#endif /* _INC_A370_CONFIG_H */
