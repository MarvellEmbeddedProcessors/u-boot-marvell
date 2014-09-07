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

typedef enum mvNetComplexPhase {
	MV_NETC_FIRST_PHASE,
	MV_NETC_SECOND_PHASE,
} MV_NETC_PHASE_CFG;

/******************************************************************************/
/* System Soft Reset 1 */
#define MV_NETCOMP_SYSTEM_SOFT_RESET			(MV_NET_COMPLEX_OFFSET + 0x8)

#define NETC_GOP_SOFT_RESET_OFFSET			6
#define NETC_GOP_SOFT_RESET_MASK			(0x1 << NETC_GOP_SOFT_RESET_OFFSET)

/* Ports Control 0 */
#define MV_NETCOMP_PORTS_CONTROL_0			(MV_NET_COMPLEX_OFFSET + 0x10)

#define NETC_CLK_DIV_PHASE_OFFSET			31
#define NETC_CLK_DIV_PHASE_MASK				(0x1 << NETC_CLK_DIV_PHASE_OFFSET)

#define NETC_GIG_RX_DATA_SAMPLE_OFFSET			29
#define NETC_GIG_RX_DATA_SAMPLE_MASK			(0x1 << NETC_GIG_RX_DATA_SAMPLE_OFFSET)

#define NETC_BUS_WIDTH_SELECT_OFFSET			1
#define NETC_BUS_WIDTH_SELECT_MASK			(0x1 << NETC_BUS_WIDTH_SELECT_OFFSET)

/* Ports Control 1 */
#define MV_NETCOMP_PORTS_CONTROL_1			(MV_NET_COMPLEX_OFFSET + 0x14)

#define NETC_PORT_GIG_RF_RESET_OFFSET(port)		(28 + port)
#define NETC_PORT_GIG_RF_RESET_MASK(port)		(0x1 << NETC_PORT_GIG_RF_RESET_OFFSET(port))

#define NETC_PORTS_ACTIVE_OFFSET(port)			(0 + port)
#define NETC_PORTS_ACTIVE_MASK(port)			(0x1 << NETC_PORTS_ACTIVE_OFFSET(port))

/* Networking Complex Control 0 */
#define MV_NETCOMP_CONTROL_0				(MV_NET_COMPLEX_OFFSET + 0x20)

#define NETC_CTRL_ENA_XAUI_OFFSET			11
#define NETC_CTRL_ENA_XAUI_MASK				(0x1 << NETC_CTRL_ENA_XAUI_OFFSET)

#define NETC_CTRL_ENA_RXAUI_OFFSET			10
#define NETC_CTRL_ENA_RXAUI_MASK			(0x1 << NETC_CTRL_ENA_RXAUI_OFFSET)

#define NETC_GBE_PORT1_MODE_OFFSET			1
#define NETC_GBE_PORT1_MODE_MASK			(0x1 << NETC_GBE_PORT1_MODE_OFFSET)

/* Networking Complex AMB Access Control 0 */
#define MV_NETCOMP_AMB_ACCESS_CTRL_0			(MV_NET_COMPLEX_OFFSET + 0xC0)

#define NETC_AMB_ACCESS_CTRL_OFFSET			24
#define NETC_AMB_ACCESS_CTRL_MASK			(0xff << NETC_AMB_ACCESS_CTRL_OFFSET)

/* QSGMII Control 1 */
#define MV_NETCOMP_QSGMII_CTRL_1			(MV_IP_CONFIG_REGS_OFFSET + 0x94)

#define NETC_QSGMII_CTRL_RSTN_OFFSET			31
#define NETC_QSGMII_CTRL_RSTN_MASK			(0x1 << NETC_QSGMII_CTRL_RSTN_OFFSET)

#define NETC_QSGMII_CTRL_V3ACTIVE_OFFSET		29
#define NETC_QSGMII_CTRL_V3ACTIVE_MASK			(0x1 << NETC_QSGMII_CTRL_V3ACTIVE_OFFSET)

#define NETC_QSGMII_CTRL_VERSION_OFFSET			28
#define NETC_QSGMII_CTRL_VERSION_MASK			(0x1 << NETC_QSGMII_CTRL_VERSION_OFFSET)

/* Function Enable Control 1 */
#define MV_NETCOMP_FUNCTION_ENABLE_CTRL_1		(MV_MISC_REGS_OFFSET + 0X88)

#define NETC_PACKET_PROCESS_OFFSET			1
#define NETC_PACKET_PROCESS_MASK			(0x1 << NETC_PACKET_PROCESS_OFFSET)

/* ComPhy Selector */
#define COMMON_PHYS_SELECTORS_REG			(MV_COMMON_PHY_REGS_OFFSET + 0xFC)

#define COMMON_PHYS_SELECTOR_LANE_OFFSET(lane)		(4 * lane)
#define COMMON_PHYS_SELECTOR_LANE_MASK(lane)		(0xF << COMMON_PHYS_SELECTOR_LANE_OFFSET(lane))

MV_STATUS mvNetComplexInit(MV_U32 netCompConfig, MV_NETC_PHASE_CFG phase);
