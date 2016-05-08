/*
* ***************************************************************************
* Copyright (C) 2015 Marvell International Ltd.
* ***************************************************************************
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 2 of the License, or any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ***************************************************************************
*/

/*#define DEBUG */
#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/comphy_hpipe.h>
#include <asm/arch-mvebu/mvebu.h>

#define HPIPE_ADDR(base, lane)		(base + 0x800 * lane)
#define COMPHY_RESET_REG		0x120

#define COMPHY_RESET_SW_OFFSET		14
#define COMPHY_RESET_SW_MASK		(1 << COMPHY_RESET_SW_OFFSET)
#define COMPHY_RESET_CORE_OFFSET	13
#define COMPHY_RESET_CORE_MASK		(1 << COMPHY_RESET_CORE_OFFSET)

#define COMPHY_PCI_MAC_CTRL		0x200

#define COMPHY_PCI_EN_OFFSET		0
#define COMPHY_PCI_EN_MASK		(0x1 << COMPHY_PCI_EN_OFFSET)
#define COMPHY_PCI_AXI_CACHE_OFFSET	8
#define COMPHY_PCI_AXI_CACHE_MASK	(0xF << COMPHY_PCI_AXI_CACHE_OFFSET)
#define COMPHY_PCI_COHERENT		0x7
#define COMPHY_PCI_X1_EN_OFFSET		14
#define COMPHY_PCI_X1_EN_MASK		(0x1 << COMPHY_PCI_X1_EN_OFFSET)

static void comphy_pcie_release_soft_reset(void __iomem *hpipe_addr)
{
	/* Set MAX PLL Calibration */
	reg_set(hpipe_addr + HPIPE_KVCO_CALIB_CTRL_REG,
		0x1 << HPIPE_KVCO_CALIB_CTRL_MAX_PLL_OFFSET, HPIPE_KVCO_CALIB_CTRL_MAX_PLL_MASK);
	reg_set(hpipe_addr + HPIPE_LANE_CONFIG1_REG,
		0x1 << HPIPE_LANE_CONFIG1_MAX_PLL_OFFSET, HPIPE_LANE_CONFIG1_MAX_PLL_MASK);
	reg_set(hpipe_addr + HPIPE_LANE_CONFIG1_REG,
		0x1 << HPIPE_LANE_CONFIG1_GEN2_PLL_OFFSET, HPIPE_LANE_CONFIG1_GEN2_PLL_MASK);

	/* DFE reset sequence */
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x1 << HPIPE_PWR_CTR_RST_DFE_OFFSET, HPIPE_PWR_CTR_RST_DFE_MASK);
	udelay(10);
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x0 << HPIPE_PWR_CTR_RST_DFE_OFFSET, HPIPE_PWR_CTR_RST_DFE_MASK);

	/* SW reset for interupt logic */
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x1 << HPIPE_PWR_CTR_SFT_RST_OFFSET, HPIPE_PWR_CTR_SFT_RST_MASK);
	udelay(10);
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x0 << HPIPE_PWR_CTR_SFT_RST_OFFSET, HPIPE_PWR_CTR_SFT_RST_MASK);
}

static int comphy_pcie_power_up(u32 lane, u32 pcie_by4, void __iomem *hpipe_addr)
{
	u32 start_val, break_val, master_val;
	debug_enter();

	/* Enable CLK 500 */
	reg_set(hpipe_addr + HPIPE_MISC_REG, 0x1 << HPIPE_MISC_CLK500_EN_OFFSET, HPIPE_MISC_CLK500_EN_MASK);
	/* Clear lane align off */
	if (pcie_by4)
		reg_set(hpipe_addr + HPIPE_LANE_ALIGN_REG,
			0x0 << HPIPE_LANE_ALIGN_OFF_OFFSET, HPIPE_LANE_ALIGN_OFF_MASK);
	/* Reference Frequency Select set 0 (for PCIe 0 = 100Mhz) */
	reg_set(hpipe_addr + HPIPE_PWR_PLL_REG, 0x0 << HPIPE_PWR_PLL_REF_FREQ_OFFSET, HPIPE_PWR_PLL_REF_FREQ_MASK);
	/* PHY Mode Select (set PCIe = 0x3) */
	reg_set(hpipe_addr + HPIPE_PWR_PLL_REG, 0x3 << HPIPE_PWR_PLL_PHY_MODE_OFFSET, HPIPE_PWR_PLL_PHY_MODE_MASK);
	/* Set PIPE RESET - SW reset for the PIPE */
	reg_set(hpipe_addr + HPIPE_RST_CLK_CTRL_REG,
		0x1 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET, HPIPE_RST_CLK_CTRL_PIPE_RST_MASK);
	/* Set PCIe fixed mode to 8 bit @ 250 Mhz */
	reg_set(hpipe_addr + HPIPE_RST_CLK_CTRL_REG,
		0x1 << HPIPE_RST_CLK_CTRL_FIXED_PCLK_OFFSET, HPIPE_RST_CLK_CTRL_FIXED_PCLK_MASK);
	/* Set 5Gbps for RX and TX */
	reg_set(hpipe_addr + HPIPE_ISOLATE_MODE_REG,
		0x1 << HPIPE_ISOLATE_MODE_GEN_RX_OFFSET, HPIPE_ISOLATE_MODE_GEN_RX_MASK);
	reg_set(hpipe_addr + HPIPE_ISOLATE_MODE_REG,
		0x1 << HPIPE_ISOLATE_MODE_GEN_TX_OFFSET, HPIPE_ISOLATE_MODE_GEN_TX_MASK);
	/* Set Max PHY generation setting - 5GBps */
	reg_set(hpipe_addr + HPIPE_INTERFACE_REG,
		0x1 << HPIPE_INTERFACE_GEN_MAX_OFFSET, HPIPE_INTERFACE_GEN_MAX_MASK);
	/* Set lane Break/Start/Master:
	 master - Provide RefClock to MAC
	 start - Start of providing RefClock
	 break - Stop passing the RefClock */
	if (pcie_by4) {
		/* if By4 Lane 0 - is master and start PHY
		    lane 1-2 - pass refclock to next phy
		    lane 3 - stop passing refclock */
		if (lane == 0) {
			start_val = 0x1;
			break_val = 0x0;
			master_val = 0x1;
		} else if (lane == 3) {
			start_val = 0x0;
			break_val = 0x1;
			master_val = 0x0;
		} else {
			start_val = 0x0;
			break_val = 0x0;
			master_val = 0x0;
		}
	} else {
		start_val = 0x1;
		break_val = 0x1;
		master_val = 0x1;
	}
	reg_set(hpipe_addr + HPIPE_CLK_SRC_HI_REG,
		start_val << HPIPE_CLK_SRC_HI_LANE_STRT_OFFSET, HPIPE_CLK_SRC_HI_LANE_STRT_MASK);
	reg_set(hpipe_addr + HPIPE_CLK_SRC_HI_REG,
		break_val << HPIPE_CLK_SRC_HI_LANE_BREAK_OFFSET, HPIPE_CLK_SRC_HI_LANE_BREAK_MASK);
	reg_set(hpipe_addr + HPIPE_CLK_SRC_HI_REG,
		master_val << HPIPE_CLK_SRC_HI_LANE_MASTER_OFFSET, HPIPE_CLK_SRC_HI_LANE_MASTER_MASK);

	/* For PCIe by4 need to reset after configure all 4 lanes */
	if (pcie_by4) {
		debug_exit();
		return 1;
	}

	comphy_pcie_release_soft_reset(hpipe_addr);
	/* release PIPE RESET - release PHY from reset */
	reg_set(hpipe_addr + HPIPE_RST_CLK_CTRL_REG,
		0x0 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET, HPIPE_RST_CLK_CTRL_PIPE_RST_MASK);
	udelay(20000);

	debug_exit();
	/* Return the status of the PLL */
	return readl(hpipe_addr + HPIPE_LANE_STATUS1_REG) & HPIPE_LANE_STATUS1_PCLK_EN_MASK;
}

int comphy_ap806_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map)
{
	struct comphy_map *ptr_comphy_map;
	void __iomem *comphy_base_addr, *hpipe_base_addr;
	u32 comphy_max_count, lane, ret = 0;
	u32 pcie_by4 = 1;

	debug_enter();

	comphy_base_addr = ptr_chip_cfg->comphy_base_addr;
	comphy_max_count = ptr_chip_cfg->comphy_lanes_count;
	hpipe_base_addr = ptr_chip_cfg->hpipe3_base_addr;

	/* Set PHY to Normal mode */
	reg_set(comphy_base_addr + COMPHY_RESET_REG, 1 << COMPHY_RESET_SW_OFFSET, COMPHY_RESET_SW_MASK);
	reg_set(comphy_base_addr + COMPHY_RESET_REG, 1 << COMPHY_RESET_CORE_OFFSET, COMPHY_RESET_CORE_MASK);

	/* Check if the first 4 lanes configured as By-4 */
	for (lane = 0, ptr_comphy_map = serdes_map; lane < 4; lane++, ptr_comphy_map++) {
		if (ptr_comphy_map->type != PHY_TYPE_PEX0) {
			pcie_by4 = 0;
			break;
		}
	}

	for (lane = 0, ptr_comphy_map = serdes_map; lane < comphy_max_count; lane++, ptr_comphy_map++) {
		debug("Initialize serdes number %d\n", lane);
		debug("Serdes type = 0x%x\n", ptr_comphy_map->type);
		switch (ptr_comphy_map->type) {
		case PHY_TYPE_UNCONNECTED:
			continue;
			break;
		case PHY_TYPE_PEX0:
		case PHY_TYPE_PEX1:
		case PHY_TYPE_PEX2:
		case PHY_TYPE_PEX3:
			ret = comphy_pcie_power_up(lane, pcie_by4, HPIPE_ADDR(hpipe_base_addr, lane));
			udelay(20);
			break;
		default:
			debug("Unknown SerDes type, skip initialize SerDes %d\n", lane);
			break;
		}
		if (ret == 0)
			error("PLL is not locked - Failed to initialize lane %d\n", lane);
	}

	/* SW reset for PCIe for all lanes after power up */
	if (pcie_by4) {
		for (lane = 0; lane < 4; lane++) {
			comphy_pcie_release_soft_reset(HPIPE_ADDR(hpipe_base_addr, lane));
		}

		/* release PIPE RESET - release PHY from reset
		   need to release the lanes withot delay between them */
		debug("%s: Release PIPE reset for PCIe-By4, write to Reset Clock control register\n", __func__);
		for (lane = 0; lane < 4; lane++) {
			reg_set_silent(HPIPE_ADDR(hpipe_base_addr, lane) + HPIPE_RST_CLK_CTRL_REG,
				       0x0 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET, HPIPE_RST_CLK_CTRL_PIPE_RST_MASK);
		}

		udelay(20000);
		for (lane = 0; lane < 4; lane++) {
			ret = readl(HPIPE_ADDR(hpipe_base_addr, lane) +
				   HPIPE_LANE_STATUS1_REG) & HPIPE_LANE_STATUS1_PCLK_EN_MASK;
			if (ret == 0)
				error("PLL is not locked - Failed to initialize lane %d\n", lane);
		}
	}

#ifdef CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT
	u32 reg;
	if (!pcie_by4) {
		/* This change force the mac to work by 1 */
		reg = readl(0x5000006c);
		reg &= ~(0x3F << 4);
		reg |= (1 << 4);
		writel(reg, 0x5000006c);
	}
#endif

	/* Set PCIe transactions towards A2 as:
	 * - read allocate
	 * - write non alocate
	 * - outer sharable */
	reg_set(comphy_base_addr + COMPHY_PCI_MAC_CTRL, COMPHY_PCI_COHERENT << COMPHY_PCI_AXI_CACHE_OFFSET,
			COMPHY_PCI_AXI_CACHE_MASK);

	/* Set the Port x1 */
	if (pcie_by4)
		reg_set(comphy_base_addr + COMPHY_PCI_MAC_CTRL, 0 << COMPHY_PCI_X1_EN_OFFSET, COMPHY_PCI_X1_EN_MASK);
	else
		reg_set(comphy_base_addr + COMPHY_PCI_MAC_CTRL, 1 << COMPHY_PCI_X1_EN_OFFSET, COMPHY_PCI_X1_EN_MASK);

	/* Enable PCIe unit */
	reg_set(comphy_base_addr + COMPHY_PCI_MAC_CTRL, 1 << COMPHY_PCI_EN_OFFSET, COMPHY_PCI_EN_MASK);

	debug_exit();
	return 0;
}
