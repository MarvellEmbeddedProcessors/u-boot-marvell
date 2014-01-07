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


#ifndef _MV_CTRL_PEX_H
#define _MV_CTRL_PEX_H

#include "mvBHboardEnvSpec.h"

/********************************* Definitions ********************************/

/* Sample at Reset */
#define MPP_SAMPLE_AT_RESET(id)		            (0xE4200 + (id * 4))
#define DEV_ID_REG			                    0x18238

#define DEVICE_ID_OFFS			                16
#define DEVICE_ID_MASK			                0xFFFF0000

#define SATR_DEVICE_ID_2_0_OFFS		            21
#define SATR_DEVICE_ID_2_0_MASK		            (3 << SATR_DEVICE_ID_2_0_OFFS)

/*  Power Management Clock Gating Control Register  */
#define MV_PEX_IF_REGS_OFFSET(pexIf)            (pexIf > 0 ? (0x40000 + (pexIf-1)*0x4000) : 0x80000)
#define MV_PEX_IF_REGS_BASE(unit)               (MV_PEX_IF_REGS_OFFSET(unit))
#define PEX_CFG_DIRECT_ACCESS(pexIf, cfgReg)    (MV_PEX_IF_REGS_BASE(pexIf) + (cfgReg))

/* PCI Express Control and Status Registers */
#define MAX_PEX_BUSSES                          256

#define PEX_CAPABILITIES_REG(pexIf)			    ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x60)
#define PEX_LINK_CTRL_STATUS2_REG(pexIf)        ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x90)
#define PEX_CTRL_REG(pexIf)                     ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A00)
#define PEX_STATUS_REG(pexIf)                   ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A04)
#define PEX_DBG_STATUS_REG(pexIf)               ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A64)
#define PEX_LINK_CAPABILITY_REG                 0x6C
#define PEX_LINK_CTRL_STAT_REG                  0x70
#define PXSR_PEX_DEV_NUM_OFFS                   16  /* Device Number Indication */
#define PXSR_PEX_DEV_NUM_MASK                   (0x1f << PXSR_PEX_DEV_NUM_OFFS)
#define PXSR_PEX_BUS_NUM_OFFS                   8 /* Bus Number Indication */
#define PXSR_PEX_BUS_NUM_MASK                   (0xff << PXSR_PEX_BUS_NUM_OFFS)

/* SOC_CTRL_REG fields */
#define MV_MISC_REGS_OFFSET                     (0x18200)
#define SOC_CTRL_REG                            (MV_MISC_REGS_BASE + 0x4)

/* PEX_CAPABILITIES_REG fields */
#define PCIE0_ENABLE_OFFS                       0
#define PCIE0_ENABLE_MASK                       (0x1 << PCIE0_ENABLE_OFFS)
#define PCIE1_ENABLE_OFFS                       1
#define PCIE1_ENABLE_MASK                       (0x1 << PCIE1_ENABLE_OFFS)
#define PCIE2_ENABLE_OFFS                       2
#define PCIE2_ENABLE_MASK                       (0x1 << PCIE2_ENABLE_OFFS)
#define PCIE3_ENABLE_OFFS                       3
#define PCIE4_ENABLE_MASK                       (0x1 << PCIE3_ENABLE_OFFS)

/* Controller revision info */
#define PEX_DEVICE_AND_VENDOR_ID                0x000

/* PCI Express Configuration Address Register */
#define PXCAR_REG_NUM_OFFS                      2
#define PXCAR_REG_NUM_MAX                       0x3F
#define PXCAR_REG_NUM_MASK                      (PXCAR_REG_NUM_MAX << PXCAR_REG_NUM_OFFS)
#define PXCAR_FUNC_NUM_OFFS                     8
#define PXCAR_FUNC_NUM_MAX                      0x7
#define PXCAR_FUNC_NUM_MASK                     (PXCAR_FUNC_NUM_MAX << PXCAR_FUNC_NUM_OFFS)
#define PXCAR_DEVICE_NUM_OFFS                   11
#define PXCAR_DEVICE_NUM_MAX                    0x1F
#define PXCAR_DEVICE_NUM_MASK                   (PXCAR_DEVICE_NUM_MAX << PXCAR_DEVICE_NUM_OFFS)
#define PXCAR_BUS_NUM_OFFS                      16
#define PXCAR_BUS_NUM_MAX                       0xFF
#define PXCAR_BUS_NUM_MASK                      (PXCAR_BUS_NUM_MAX << PXCAR_BUS_NUM_OFFS)
#define PXCAR_EXT_REG_NUM_OFFS                  24
#define PXCAR_EXT_REG_NUM_MAX                   0xF

#define PEX_CFG_ADDR_REG(pexIf)                 ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x18F8)
#define PEX_CFG_DATA_REG(pexIf)                 ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x18FC)

#define PXCAR_REAL_EXT_REG_NUM_OFFS             8
#define PXCAR_REAL_EXT_REG_NUM_MASK             (0xF << PXCAR_REAL_EXT_REG_NUM_OFFS)

#define PXCAR_CONFIG_EN                         BIT31
#define PEX_STATUS_AND_COMMAND                  0x004
#define PXSAC_MABORT                            BIT29 /* Recieved Master Abort */

/*************************** Functions declarations ***************************/

/**************************************************************************
* mvHwsPexConfig -
*
* DESCRIPTION:          Executes PEX MAC configuration
* INPUT:                serdesMap       -   The board topology map
* OUTPUT:               Configuration for PEX lanes.
* RETURNS:              MV_OK           -   for success
***************************************************************************/
MV_STATUS mvHwsPexConfig();

#endif
