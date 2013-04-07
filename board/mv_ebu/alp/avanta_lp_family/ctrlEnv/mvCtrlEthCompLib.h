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

    * 	Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *	Neither the name of Marvell nor the names of its contributors may be
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

#ifndef __INCmvCtrlEthCompLibh
#define __INCmvCtrlEthCompLibh

#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysEthConfig.h"

#define MV_ETHCOMP_CTRL_REG(id)			(MV_ETH_COMPLEX_BASE + 0x10 + (id * 4))

/* Ethernet Complex Control 0 */
#define ETHCC_SW_PORT_0_SRC_OFFSET		4
#define ETHCC_SW_PORT_0_SRC_MASK		(0x1 << ETHCC_SW_PORT_0_SRC_OFFSET)
#define ETHCC_SW_PORT_3_SRC_OFFSET		5
#define ETHCC_SW_PORT_3_SRC_MASK		(0x1 << ETHCC_SW_PORT_3_SRC_OFFSET)
#define ETHCC_SW_PORT_4_SRC_OFFSET		6
#define ETHCC_SW_PORT_4_SRC_MASK		(0x1 << ETHCC_SW_PORT_4_SRC_OFFSET)
#define ETHCC_SW_PORT_6_SRC_OFFSET		7
#define ETHCC_SW_PORT_6_SRC_MASK		(0x1 << ETHCC_SW_PORT_6_SRC_OFFSET)
#define ETHCC_GBE_MAC0_SRC_OFFSET		10
#define ETHCC_GBE_MAC0_SRC_MASK			(0x3 << ETHCC_GBE_MAC0_SRC_OFFSET)
#define ETHCC_GBE_MAC1_SRC_OFFSET		12
#define ETHCC_GBE_MAC1_SRC_MASK			(0x3 << ETHCC_GBE_MAC1_SRC_OFFSET)
#define ETHCC_GBE_PHY_PORT_0_SRC_OFFSET		14
#define ETHCC_GBE_PHY_PORT_0_SRC_MASK		(0x1 << ETHCC_GBE_PHY_PORT_0_SRC_OFFSET)
#define ETHCC_GBE_PHY_PORT_1_SMI_SRC_OFFSET	15
#define ETHCC_GBE_PHY_PORT_1_SMI_SRC_MASK	(0x1 << ETHCC_GBE_PHY_PORT_1_SMI_SRC_OFFSET)
#define ETHCC_GBE_PHY_PORT_2_SMI_SRC_OFFSET	16
#define ETHCC_GBE_PHY_PORT_2_SMI_SRC_MASK	(0x1 << ETHCC_GBE_PHY_PORT_2_SMI_SRC_OFFSET)
#define ETHCC_GBE_PHY_PORT_3_SRC_OFFSET		17
#define ETHCC_GBE_PHY_PORT_3_SRC_MASK		(0x1 << ETHCC_GBE_PHY_PORT_0_SRC_OFFSET)
#define ETHCC_GE_MAC0_SW_PORT_6_SPD_OFFSET	18
#define ETHCC_GE_MAC0_SW_PORT_6_SPD_MASK	(0x1 << ETHCC_GE_MAC0_SW_PORT_6_SPD_OFFSET)
#define ETHCC_LOOPBACK_PORT_SPD_OFFSET		18
#define ETHCC_LOOPBACK_PORT_SPD_MASK		(0x1 << ETHCC_LOOPBACK_PORT_SPD_OFFSET)

MV_VOID mvEthCompSkipInitSet(MV_BOOL skip);
MV_STATUS mvEthCompMac2SwitchConfig(MV_U32 ethCompCfg, MV_BOOL muxCfgOnly);
MV_STATUS mvEthCompSwitchReset(MV_U32 ethCompCfg);
MV_STATUS mvEthCompMac2RgmiiConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthCompSwP56ToRgmiiConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthCompSataConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthernetComplexShutdownIf(MV_BOOL integSwitch, MV_BOOL gePhy, MV_BOOL fePhy);
MV_STATUS mvEthernetComplexInit(MV_VOID);
MV_STATUS mvEthernetComplexChangeMode(MV_U32 oldCfg, MV_U32 newCfg);

#endif /* __INCmvCtrlEthCompLibh */
