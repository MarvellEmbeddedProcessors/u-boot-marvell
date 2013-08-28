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


#ifndef __INCmvBHboardEnvSpech
#define __INCmvBHboardEnvSpech

/* Board specific configuration */
/* ============================ */


/* Avanta LP family*/
#define MV_6660_DEV_ID      0x6660
#define MV_6650_DEV_ID      0x6650
#define MV_6610_DEV_ID      0x6610

/* SGMII speed */
#define SGMII0_CAPACITY_2_5G	BIT7
#define SGMII1_CAPACITY_2_5G	BIT8

/* boards ID numbers */
#define BOARD_ID_BASE               0x0

#define RD_88F6650_BP_ID            (BOARD_ID_BASE)
#define DB_88F6650_BP_ID            (RD_88F6650_BP_ID + 1)
#define RD_88F6660_BP_ID            (DB_88F6650_BP_ID + 1)
#define DB_88F6660_BP_ID            (RD_88F6660_BP_ID + 1)

#define MV_MAX_BOARD_ID             (DB_88F6660_BP_ID + 1)
#define INVALID_BAORD_ID            0xFFFFFFFF

/* Sample at Reset */
#define MPP_SAMPLE_AT_RESET(id)		(0xE8200 + (id * 4))
#define PEX_CLK_100MHZ_MASK		0x00000004
#define PEX_CLK_100MHZ_OFFSET		2
#define SATR_DEVICE_ID_2_0_OFFS		21
#define SATR_DEVICE_ID_2_0_MASK		(3 << SATR_DEVICE_ID_2_0_OFFS)

#define MSAR_TCLK_OFFS			22
#define MSAR_TCLK_MASK			(0x1 << MSAR_TCLK_OFFS)

#define DEV_ID_REG			    0x18238
#define VENDOR_ID_OFFS			0
#define VENDOR_ID_MASK			0xFFFF
#define DEVICE_ID_OFFS			16
#define DEVICE_ID_MASK			0xFFFF0000

#define DEV_VERSION_ID_REG      0x1823C
#define REVISON_ID_OFFS         8
#define REVISON_ID_MASK         0xF00

#define TSMC_Z1                0
#define UMC_Z1                 1
#define TSMC_Z1_GPON_METAL_FIX 2
#define UMC_A0                 3

#define COMMON_PHY_SELECTOR_REG     0x18300
#define COMPHY0_SELCTOR_OFFS        0
#define COMPHY1_SELCTOR_OFFS        1
#define COMPHY2_SELCTOR_OFFS        3
#define COMPHY3_SELCTOR_OFFS        4

#define COMMON_PHY_CONFIGURATION1_REG(phy)   (0x18310 + (phy * 4))

/* COMMON_PHYn_CONFIGURATION1_REG fields */
#define PIPE_SELECT_OFFS                2
#define PIPE_SELECT_MASK                (1 << PIPE_SELECT_OFFS)
#define POWER_UP_IVREF_OFFS             1
#define POWER_UP_IVREF_MASK             (1 << POWER_UP_IVREF_OFFS)
#define GEN_RX_OFFS                     22
#define GEN_RX_MASK                     (0xF << GEN_RX_OFFS)
#define GEN_TX_OFFS                     26
#define GEN_TX_MASK                     (0xF << GEN_TX_OFFS)
#define PHY_SOFTWARE_RESET_OFFS         11
#define PHY_SOFTWARE_RESET_MASK         (1 << PHY_SOFTWARE_RESET_OFFS)
#define PHY_RESET_CORE_OFFS             12
#define PHY_RESET_CORE_MASK             (1 << PHY_RESET_CORE_OFFS)
#define PHY_CORE_RST_OFFS               13
#define PHY_CORE_RST_MASK               (1 << PHY_CORE_RST_OFFS)
#define PHY_POWER_ON_RESET_OFFS         14
#define PHY_POWER_ON_RESET_MASK         (1 << PHY_POWER_ON_RESET_OFFS)
#define PHY_MODE_OFFS                   15
#define PHY_MODE_MASK                   (1 << PHY_MODE_OFFS)

#define PHY_POWER_UP_PLL_OFFS           16
#define PHY_POWER_UP_PLL_MASK           (1 << PHY_POWER_UP_PLL_OFFS)
#define PHY_POWER_UP_RX_OFFS            17
#define PHY_POWER_UP_RX_MASK            (1 << PHY_POWER_UP_RX_OFFS)
#define PHY_POWER_UP_TX_OFFS            18
#define PHY_POWER_UP_TX_MASK            (1 << PHY_POWER_UP_TX_OFFS)

#define MPP_CONTROL_REG(id)			(0x18000 + (id * 4))

#define COMPHY_REGS_CONFIGURATION_REG(comphy)  (0x30000 + ((comphy+2)<<13))

/* COMPHY_REGS_CONFIGURATION offset fields */
#define POWER_AND_PLL_CONTROL_REG(comphy)       (COMPHY_REGS_CONFIGURATION_REG(comphy) + (4 * 0x1))
#define KVCO_CALOBRATION_CONTROL_REG(comphy)    (COMPHY_REGS_CONFIGURATION_REG(comphy) + (4 * 0x2))
#define GENERETION_2_SETTINGS_1_REG(comphy)     (COMPHY_REGS_CONFIGURATION_REG(comphy) + (4 * 0x10))
#define DIGITAL_LOOPBACK_ENABLE_REG(comphy)     (COMPHY_REGS_CONFIGURATION_REG(comphy) + (4 * 0x23))
#define PHY_ISOLATION_MODE_CONTROL_REG(comphy)  (COMPHY_REGS_CONFIGURATION_REG(comphy) + (4 * 0x26))
#define RESERVED_46_REG(comphy)                 (COMPHY_REGS_CONFIGURATION_REG(comphy) + (4 * 0x46))
#define MISCELLANEOUS_CONTROL0_REG(comphy)      (COMPHY_REGS_CONFIGURATION_REG(comphy) + (4 * 0x4f))
#define RESET_AND_CLOCK_CONTROL_REG(comphy)     (COMPHY_REGS_CONFIGURATION_REG(comphy) + (4 * 0xC1))


/* SOC_CTRL_REG fields */
#define SCR_PEX_ENA_OFFS(pex)       ((pex) & 0x3)
#define SCR_PEX_ENA_MASK(pex)       (1 << pex)

#define PCIE0_QUADX1_EN             (1<<7)
#define PCIE1_QUADX1_EN             (1<<8)

#define SCR_PEX_4BY1_OFFS(pex)      ((pex) + 7)
#define SCR_PEX_4BY1_MASK(pex)      (1 << SCR_PEX_4BY1_OFFS(pex))

#define PCIE1_CLK_OUT_EN_OFF        5
#define PCIE1_CLK_OUT_EN_MASK       (1 << PCIE1_CLK_OUT_EN_OFF)

#define PCIE0_CLK_OUT_EN_OFF        4
#define PCIE0_CLK_OUT_EN_MASK       (1 << PCIE0_CLK_OUT_EN_OFF)

#define SCR_PEX0_4BY1_OFFS          7
#define SCR_PEX0_4BY1_MASK          (1 << SCR_PEX0_4BY1_OFFS)

#define SCR_PEX1_4BY1_OFFS          8
#define SCR_PEX1_4BY1_MASK          (1 << SCR_PEX1_4BY1_OFFS)


#define MV_MISC_REGS_OFFSET         (0x18200)
#define MV_MISC_REGS_BASE           (MV_MISC_REGS_OFFSET)
#define SOC_CTRL_REG                (MV_MISC_REGS_BASE + 0x4)

#define GPIO_DATA_OUT_REG_BASE          0x18100
#define GPIO_DATA_OUT_REG(Num)          (GPIO_DATA_OUT_REG_BASE + (0x40*Num))
#define GPIO_DATA_OUT_ENABLE_REG_BASE   0x18104
#define GPIO_DATA_OUT_ENABLE_REG(Num)   (GPIO_DATA_OUT_ENABLE_REG_BASE + (0x40*Num))



/********************************************/
/* PCI Express Control and Status Registers */
/********************************************/
#define MAX_PEX_BUSSES          256


#define PEX_CAPABILITIES_REG(pexIf)			((MV_PEX_IF_REGS_BASE(pexIf)) + 0x60)
#define PEX_CTRL_REG(pexIf)         ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A00)
#define PEX_LINK_CTRL_STATUS2_REG(pexIf)  ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x90)
#define PEX_DBG_STATUS_REG(pexIf)         ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A64)
#define PEX_STATUS_REG(pexIf)       ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A04)
#define PEX_LINK_CAPABILITY_REG     0x6C
#define PEX_LINK_CTRL_STAT_REG      0x70
#define PXSR_PEX_DEV_NUM_OFFS     16  /* Device Number Indication */
#define PXSR_PEX_DEV_NUM_MASK     (0x1f << PXSR_PEX_DEV_NUM_OFFS)
#define PXSR_PEX_BUS_NUM_OFFS     8 /* Bus Number Indication */
#define PXSR_PEX_BUS_NUM_MASK     (0xff << PXSR_PEX_BUS_NUM_OFFS)

/* PCI Express Configuration Address Register */
/* PEX_CFG_ADDR_REG (PXCAR) */
#define PXCAR_REG_NUM_OFFS    2
#define PXCAR_REG_NUM_MAX   0x3F
#define PXCAR_REG_NUM_MASK    (PXCAR_REG_NUM_MAX << PXCAR_REG_NUM_OFFS)
#define PXCAR_FUNC_NUM_OFFS   8
#define PXCAR_FUNC_NUM_MAX    0x7
#define PXCAR_FUNC_NUM_MASK   (PXCAR_FUNC_NUM_MAX << PXCAR_FUNC_NUM_OFFS)
#define PXCAR_DEVICE_NUM_OFFS   11
#define PXCAR_DEVICE_NUM_MAX    0x1F
#define PXCAR_DEVICE_NUM_MASK   (PXCAR_DEVICE_NUM_MAX << PXCAR_DEVICE_NUM_OFFS)
#define PXCAR_BUS_NUM_OFFS    16
#define PXCAR_BUS_NUM_MAX   0xFF
#define PXCAR_BUS_NUM_MASK    (PXCAR_BUS_NUM_MAX << PXCAR_BUS_NUM_OFFS)
#define PXCAR_EXT_REG_NUM_OFFS    24
#define PXCAR_EXT_REG_NUM_MAX   0xF

#define PEX_CFG_ADDR_REG(pexIf)   ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x18F8)
#define PEX_CFG_DATA_REG(pexIf)   ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x18FC)
#define PEX_CAUSE_REG(pexIf)    ((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1900)


#define PXCAR_REAL_EXT_REG_NUM_OFFS     8
#define PXCAR_REAL_EXT_REG_NUM_MASK     (0xF << PXCAR_REAL_EXT_REG_NUM_OFFS)

#define PXSR_DL_DOWN        BIT0  /* DL_Down indication. */
#define PXCAR_CONFIG_EN     BIT31
#define PEX_STATUS_AND_COMMAND            0x004
#define PXSAC_MABORT      BIT29 /* Recieved Master Abort        */

/* PEX_CAPABILITIES_REG fields */
#define PEX_DEV_TYPE_OFFS           20
#define PEX_DEV_TYPE_MASK           (0xF << PIPE_SELECT_OFFS)

/* PEX_CAPABILITIES_REG fields */
#define PCIE0_ENABLE_OFFS           0
#define PCIE0_ENABLE_MASK           (0x1 << PCIE0_ENABLE_OFFS)
#define PCIE1_ENABLE_OFFS           1
#define PCIE1_ENABLE_MASK           (0x1 << PCIE1_ENABLE_OFFS)
#endif /* __INCmvBHboardEnvSpech */
