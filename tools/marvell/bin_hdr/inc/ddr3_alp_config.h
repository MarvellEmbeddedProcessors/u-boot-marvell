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

#ifndef _DDR3_ALP_CONFIG_H
#define _DDR3_ALP_CONFIG_H

#undef CUSTOMER_BOARD
/*	define: Using Customer board only
	undef : Using Marvell board only*/

#define DDR3_LOG_LEVEL				0
/*DDR3_LOG_LEVEL Information
Level 0: Provides an error code in a case of failure, RL, WL errors and other algorithm failure
Level 1: Provides the D-Unit setup (SPD/Static configuration)
Level 2: Provides the windows margin as a results of DQS centeralization
Level 3: Provides the windows margin of each DQ as a results of DQS centeralization */

#define DDR3_PBS					1
/*	1: Enable Per Bit Skew mechanism
	0: Disable Per Bit Skew mechanism*/

#define DDR3_RUN_SW_WHEN_HW_FAIL	1
/*	1: Enable execution of Software Write/Read leveling upon Hardware Write/Read leveling failures
	0: Disable execution of Software Write/Read leveling upon Hardware Write/Read leveling failures*/

#define DDR3_FAST_PATH_EN			1

/*	1: Open Fast-Path windows after training
	0: Open X-BAR windows after training*/

#undef STATIC_TRAINING
/*	define: Enable static training (not supported)
	undef : Enable Dynamic training*/

#define DUNIT_STATIC
/*	define: Enable static DUNIT configurations
	undef : Enable Dynamic DUNIT configuration using SPD (not supported)*/

#define DDR3_TRAINING_DEBUG			FALSE
/*	TRUE: Enable training debug sequence printing
	FALSE : Disable training debug sequence printing*/

#define REG_DIMM_SKIP_WL			FALSE
/*	TRUE: skip Write leveling stage when using registered DIMM(not supported)
	FALSE : don't skip Write leveling*/


#endif /* _DDR3_ALP_CONFIG_H */
