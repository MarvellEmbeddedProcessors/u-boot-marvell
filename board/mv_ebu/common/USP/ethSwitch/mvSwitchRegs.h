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

#ifndef __INCethswitchregsh
#define __INCethswitchregsh

#include "eth-phy/mvEthPhyRegs.h"

#define MV_SWITCH_PORTS_OFFSET					0x10

#define MV_SWITCH_PORT_STATUS_REG				0x0
#define MV_SWITCH_PHYS_CONTROL_REG				0x1
#define MV_SWITCH_PORT_CONTROL_REG				0x4
#define MV_SWITCH_PORT_VMAP_REG					0x6
#define MV_SWITCH_PORT_VID_REG					0x7


#define MV_SWITCH_PORT_OFFSET(port) (MV_SWITCH_PORTS_OFFSET + port)

/* E6063 related */
#define MV_E6063_CPU_PORT						5
#define MV_E6063_PORTS_OFFSET					0x8
#define MV_E6063_MAX_PORTS_NUM					7
#define MV_E6063_ENABLED_PORTS					((1 << 0)|(1 << 1)|(1 << 2)|	\
													(1 << 3)|(1 << 4)|(1 << 5))

/* E6065 related */
#define MV_E6065_CPU_PORT						5
#define MV_E6065_PORTS_OFFSET					0x8
#define MV_E6065_MAX_PORTS_NUM					6
#define MV_E6065_ENABLED_PORTS					((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 4)|(1 << 5))

/* E6063 related */
#define MV_E6131_CPU_PORT						0x3
#define MV_E6131_PORTS_OFFSET					0x10
#define MV_E6131_MAX_PORTS_NUM					8
#define MV_E6131_ENABLED_PORTS					((1 << 0)|(1 << 1)|(1 << 2)|	\
												(1 << 3)|(1 << 5)|(1 << 7))
/* E6161 related */
#define MV_E6161_CPU_PORT						0x5
#define MV_E6161_PORTS_OFFSET					0x10
#define MV_E6161_SMI_PHY_COMMAND				0x18
#define MV_E6161_SMI_PHY_DATA					0x19
#define MV_E6161_GLOBAL_2_REG_DEV_ADDR			0x1C
#define MV_E6161_MAX_PORTS_NUM					6
#define MV_E6161_ENABLED_PORTS					((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 4)|(1 << 5))
#define E6161_PHY_TIMEOUT						10000
#define E6161_PHY_SMI_BUSY_BIT					15  /* Busy */
#define E6161_PHY_SMI_BUSY_MASK					(1 << ETH_PHY_SMI_BUSY_BIT)

/* E6171 related */
#define MV_E6171_CPU_PORT						0x6
#define MV_E6171_PORTS_OFFSET					0x10
#define MV_E6171_SWITCH_PHIYSICAL_CTRL_REG		0x1
#define MV_E6171_SWITCH_PORT_CTRL_REG		    0x4
#define MV_E6171_SMI_PHY_COMMAND				0x18
#define MV_E6171_SMI_PHY_DATA					0x19
#define MV_E6171_GLOBAL_2_REG_DEV_ADDR			0x1C
#define MV_E6171_MAX_PORTS_NUM					7
#define MV_E6171_ENABLED_PORTS					((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 4)|(1 << 6))
#define E6171_PHY_TIMEOUT					    10000
#define E6171_PHY_SMI_BUSY_BIT		    		15  /* Busy */
#define E6171_PHY_SMI_BUSY_MASK		    		(1 << ETH_PHY_SMI_BUSY_BIT)

/* Armada 370 internal related */
#define MV_SW_CPU_PORT							0x6
#define MV_SW_PORTS_OFFSET						0x10
#define MV_SW_SWITCH_PHIYSICAL_CTRL_REG			0x1
#define MV_SW_SWITCH_PORT_CTRL_REG				0x4
#define MV_SW_SMI_PHY_COMMAND					0x18
#define MV_SW_SMI_PHY_DATA						0x19
#define MV_SW_GLOBAL_2_REG_DEV_ADDR				0x1C
#define MV_SW_MAX_PORTS_NUM						7
#define MV_SW_ENABLED_PORTS						((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 4))
#define MV_SW_PHY_TIMEOUT						10000
#define MV_SW_PHY_SMI_BUSY_BIT					15  /* Busy */
#define MV_SW_PHY_SMI_BUSY_MASK					(1 << MV_SW_PHY_SMI_BUSY_BIT)

/* KW2 internal related */
#define MV_KW2_SW_CPU_PORT						0x6
#define MV_KW2_SW_PORTS_OFFSET					0x10
#define MV_KW2_SW_SWITCH_PHIYSICAL_CTRL_REG		0x1
#define MV_KW2_SW_SWITCH_PORT_CTRL_REG			0x4
#define MV_KW2_SW_SMI_PHY_COMMAND				0x18
#define MV_KW2_SW_SMI_PHY_DATA					0x19
#define MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR			0x1C
#define MV_KW2_SW_MAX_PORTS_NUM					7
#define MV_KW2_SW_ENABLED_PORTS					((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 4))
#define KW2_SW_PHY_TIMEOUT						10000
#define KW2_SW_PHY_SMI_BUSY_BIT					15  /* Busy */
#define KW2_SW_PHY_SMI_BUSY_MASK				(1 << ETH_PHY_SMI_BUSY_BIT)

/* ALP internal related */
#define MV_ALP_SW_PORTS_OFFSET					0x10
#define MV_ALP_SW_MAX_PORTS_NUM					7

#endif /* __INCethswitchregsh */
