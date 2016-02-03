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
#include <asm/arch-mvebu/sata.h>
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/comphy_hpipe.h>
#include <asm/arch-mvebu/utmi_phy.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/fdt.h>

DECLARE_GLOBAL_DATA_PTR;

#define SD_ADDR(base, lane)			(base + 0x1000 * lane)
#define HPIPE_ADDR(base, lane)			(SD_ADDR(base, lane) + 0x800)
#define COMPHY_ADDR(base, lane)			(base + 0x28 * lane)

/* For CP-110 we have 2 Selector registers "PHY Selectors", and " PIPE
   Selectors".
   PIPE selector include USB and PCIe options.
   PHY selector include the Ethernet and SATA options, every Ethernet option has different options,
   for example: serdes lane2 had option Eth_port_0 that include (SGMII0, XAUI0, RXAUI0, KR) */
struct comphy_mux_data cp110_comphy_phy_mux_data[] = {
/* Lane 0 */ {4, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_SGMII2, 0x1},
			{PHY_TYPE_XAUI2, 0x1}, {PHY_TYPE_SATA1, 0x4} } },
/* Lane 1 */ {4, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_SGMII3, 0x1},
			{PHY_TYPE_XAUI3, 0x1}, {PHY_TYPE_SATA1, 0x4} } },
/* Lane 2 */ {6, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_SGMII0, 0x1},
			{PHY_TYPE_XAUI0, 0x1}, {PHY_TYPE_RXAUI0, 0x1}, {PHY_TYPE_KR, 0x1}, {PHY_TYPE_SATA0, 0x4} } },
/* Lane 3 */ {8, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_SGMII0, 0x1}, {PHY_TYPE_XAUI0, 0x1}, {PHY_TYPE_RXAUI0, 0x1},
			{PHY_TYPE_KR, 0x1}, {PHY_TYPE_XAUI1, 0x1}, {PHY_TYPE_RXAUI1, 0x1}, {PHY_TYPE_SATA1, 0x4} } },
/* Lane 4 */ {7, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_SGMII0, 0x1}, {PHY_TYPE_XAUI0, 0x1},
			{PHY_TYPE_RXAUI0, 0x1}, {PHY_TYPE_KR, 0x1}, {PHY_TYPE_SGMII2, 0x1}, {PHY_TYPE_XAUI2, 0x1} } },
/* Lane 5 */ {6, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_XAUI1, 0x1}, {PHY_TYPE_RXAUI1, 0x1}, {PHY_TYPE_SGMII3, 0x1},
					{PHY_TYPE_XAUI3, 0x1}, {PHY_TYPE_SATA1, 0x4} } },
};

struct comphy_mux_data cp110_comphy_pipe_mux_data[] = {
/* Lane 0 */ {2, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_PEX0, 0x4} } },
/* Lane 1 */ {4, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_USB3_HOST0, 0x1},
			{PHY_TYPE_USB3_DEVICE, 0x2}, {PHY_TYPE_PEX0, 0x4} } },
/* Lane 2 */ {3, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_USB3_HOST0, 0x1},
			{PHY_TYPE_PEX0, 0x4} } },
/* Lane 3 */ {3, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_USB3_HOST1, 0x1},
			{PHY_TYPE_PEX0, 0x4} } },
/* Lane 4 */ {4, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_USB3_HOST1, 0x1},
			{PHY_TYPE_USB3_DEVICE, 0x2}, {PHY_TYPE_PEX0, 0x4} } },
/* Lane 5 */ {2, {{PHY_TYPE_UNCONNECTED, 0x0}, {PHY_TYPE_PEX1, 0x4} } },
};

static int comphy_pcie_power_up(u32 lane, u32 pcie_by4, void __iomem *hpipe_base, void __iomem *comphy_base)
{
	u32 mask, data, ret = 1;
	void __iomem *hpipe_addr = HPIPE_ADDR(hpipe_base, lane);
	void __iomem *comphy_addr = COMPHY_ADDR(comphy_base, lane);
	u32 pcie_clk = 0; /* input */

	debug_enter();
	debug("stage: RFU configurations - hard reset comphy\n");
	/* RFU configurations - hard reset comphy */
	mask = COMMON_PHY_CFG1_PWR_UP_MASK;
	data = 0x1 << COMMON_PHY_CFG1_PWR_UP_OFFSET;
	mask |= COMMON_PHY_CFG1_PIPE_SELECT_MASK;
	data |= 0x1 << COMMON_PHY_CFG1_PIPE_SELECT_OFFSET;
	mask |= COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
	data |= 0x0 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
	mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
	data |= 0x0 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
	mask |= COMMON_PHY_PHY_MODE_MASK;
	data |= 0x0 << COMMON_PHY_PHY_MODE_OFFSET;
	reg_set(comphy_addr + COMMON_PHY_CFG1_REG, data, mask);

	/* release from hard reset */
	mask = COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
	data = 0x1 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
	mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
	data |= 0x1 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
	reg_set(comphy_addr + COMMON_PHY_CFG1_REG, data, mask);

	/* Wait 1ms - until band gap and ref clock ready */
	mdelay(1);
	/* Start comphy Configuration */
	debug("stage: Comphy configuration\n");
	/* Set PIPE soft reset */
	mask = HPIPE_RST_CLK_CTRL_PIPE_RST_MASK;
	data = 0x1 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET;
	/* Set PHY datapath width mode for V0 */
	mask |= HPIPE_RST_CLK_CTRL_FIXED_PCLK_MASK;
	data |= 0x1 << HPIPE_RST_CLK_CTRL_FIXED_PCLK_OFFSET;
	/* Set Data bus width USB mode for V0 */
	mask |= HPIPE_RST_CLK_CTRL_PIPE_WIDTH_MASK;
	data |= 0x0 << HPIPE_RST_CLK_CTRL_PIPE_WIDTH_OFFSET;
	/* Set CORE_CLK output frequency for 250Mhz */
	mask |= HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_MASK;
	data |= 0x0 << HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_OFFSET;
	reg_set(hpipe_addr + HPIPE_RST_CLK_CTRL_REG, data, mask);
	/* Set PLL ready delay for 0x2 */
	reg_set(hpipe_addr + HPIPE_CLK_SRC_LO_REG,
		0x2 << HPIPE_CLK_SRC_LO_PLL_RDY_DL_OFFSET, HPIPE_CLK_SRC_LO_PLL_RDY_DL_MASK);
	/* Set PIPE mode interface to PCIe3 - 0x1 */
	reg_set(hpipe_addr + HPIPE_CLK_SRC_HI_REG,
		0x1 << HPIPE_CLK_SRC_HI_MODE_PIPE_OFFSET, HPIPE_CLK_SRC_HI_MODE_PIPE_MASK);
	/* Config update polarity equalization */
	reg_set(hpipe_addr + HPIPE_LANE_EQ_CFG1_REG,
		0x1 << HPIPE_CFG_UPDATE_POLARITY_OFFSET, HPIPE_CFG_UPDATE_POLARITY_MASK);
	/* Set PIPE version 4 to mode enable */
	reg_set(hpipe_addr + HPIPE_DFE_CTRL_28_REG,
		0x1 << HPIPE_DFE_CTRL_28_PIPE4_OFFSET, HPIPE_DFE_CTRL_28_PIPE4_MASK);
	/* TODO: check if pcie clock is output/input - for bringup use input*/
	/* Enable PIN clock 100M_125M */
	mask = HPIPE_MISC_CLK100M_125M_MASK;
	data = 0x1 << HPIPE_MISC_CLK100M_125M_OFFSET;
	/* Set PIN_TXDCLK_2X Clock Frequency Selection for outputs 500MHz clock */
	mask |= HPIPE_MISC_TXDCLK_2X_MASK;
	data |= 0x0 << HPIPE_MISC_TXDCLK_2X_OFFSET;
	/* Enable 500MHz Clock */
	mask |= HPIPE_MISC_CLK500_EN_MASK;
	data |= 0x1 << HPIPE_MISC_CLK500_EN_OFFSET;
	if (pcie_clk) { /* output */
		/* Set reference clock comes from group 1 */
		mask |= HPIPE_MISC_REFCLK_SEL_MASK;
		data |= 0x0 << HPIPE_MISC_REFCLK_SEL_OFFSET;
	} else {
		/* Set reference clock comes from group 2 */
		mask |= HPIPE_MISC_REFCLK_SEL_MASK;
		data |= 0x1 << HPIPE_MISC_REFCLK_SEL_OFFSET;
	}
	reg_set(hpipe_addr + HPIPE_MISC_REG, data, mask);
	if (pcie_clk) { /* output */
		/* Set reference frequcency select - 0x2 for 25MHz*/
		mask = HPIPE_PWR_PLL_REF_FREQ_MASK;
		data = 0x2 << HPIPE_PWR_PLL_REF_FREQ_OFFSET;
	} else {
		/* Set reference frequcency select - 0x0 for 100MHz*/
		mask = HPIPE_PWR_PLL_REF_FREQ_MASK;
		data = 0x0 << HPIPE_PWR_PLL_REF_FREQ_OFFSET;
	}
	/* Set PHY mode to PCIe */
	mask |= HPIPE_PWR_PLL_PHY_MODE_MASK;
	data |= 0x3 << HPIPE_PWR_PLL_PHY_MODE_OFFSET;
	reg_set(hpipe_addr + HPIPE_PWR_PLL_REG, data, mask);
	/* Set the amount of time spent in the LoZ state - set for 0x7 only if
	   the PCIe clock is output */
	if (pcie_clk)
		reg_set(hpipe_addr + HPIPE_GLOBAL_PM_CTRL,
			0x7 << HPIPE_GLOBAL_PM_RXDLOZ_WAIT_OFFSET, HPIPE_GLOBAL_PM_RXDLOZ_WAIT_MASK);

	/* Set Maximal PHY Generation Setting(8Gbps) */
	mask = HPIPE_INTERFACE_GEN_MAX_MASK;
	data = 0x1 << HPIPE_INTERFACE_GEN_MAX_OFFSET;
	/* Set Link Train Mode (Tx training control pins are used) */
	mask |= HPIPE_INTERFACE_LINK_TRAIN_MASK;
	data |= 0x1 << HPIPE_INTERFACE_LINK_TRAIN_OFFSET;
	reg_set(hpipe_addr + HPIPE_INTERFACE_REG, data, mask);

	/* Set Idle_sync enable */
	mask = HPIPE_PCIE_IDLE_SYNC_MASK;
	data = 0x1 << HPIPE_PCIE_IDLE_SYNC_OFFSET;
	/* Select bits for PCIE Gen3(32bit) */
	mask |= HPIPE_PCIE_SEL_BITS_MASK;
	data |= 0x2 << HPIPE_PCIE_SEL_BITS_OFFSET;
	reg_set(hpipe_addr + HPIPE_PCIE_REG0, data, mask);

	/* Enable Tx_adapt_g1 */
	mask = HPIPE_TX_TRAIN_CTRL_G1_MASK;
	data = 0x1 << HPIPE_TX_TRAIN_CTRL_G1_OFFSET;
	/* Enable Tx_adapt_gn1 */
	mask |= HPIPE_TX_TRAIN_CTRL_GN1_MASK;
	data |= 0x1 << HPIPE_TX_TRAIN_CTRL_GN1_OFFSET;
	/* Disable Tx_adapt_g0 */
	mask |= HPIPE_TX_TRAIN_CTRL_G0_MASK;
	data |= 0x0 << HPIPE_TX_TRAIN_CTRL_G0_OFFSET;
	reg_set(hpipe_addr + HPIPE_TX_TRAIN_CTRL_REG, data, mask);

	/* Set reg_tx_train_chk_init */
	mask = HPIPE_TX_TRAIN_CHK_INIT_MASK;
	data = 0x0 << HPIPE_TX_TRAIN_CHK_INIT_OFFSET;
	/* Enable TX_COE_FM_PIN_PCIE3_EN */
	mask |= HPIPE_TX_TRAIN_COE_FM_PIN_PCIE3_MASK;
	data |= 0x1 << HPIPE_TX_TRAIN_COE_FM_PIN_PCIE3_OFFSET;
	reg_set(hpipe_addr + HPIPE_TX_TRAIN_REG, data, mask);

	/* TODO: Set analog paramters from ETP(HW) - for now use the default datas */
	debug("stage: Analog paramters from ETP(HW)\n");

	debug("stage: Comphy power up\n");
	/* Release from PIPE soft reset */
	reg_set(hpipe_addr + HPIPE_RST_CLK_CTRL_REG,
		0x0 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET, HPIPE_RST_CLK_CTRL_PIPE_RST_MASK);

	/* wait 10ms - for comphy calibration done */
	mdelay(10);

	debug("stage: Check PLL\n");
	/* Read lane status */
	data = readl(hpipe_addr + HPIPE_LANE_STATUS0_REG);
	if ((data & HPIPE_LANE_STATUS0_PCLK_EN_MASK) == 0) {
		debug("Read from reg = %p - value = 0x%x\n", hpipe_addr + HPIPE_LANE_STATUS0_REG, data);
		error("HPIPE_LANE_STATUS0_PCLK_EN_MASK is 0\n");
		ret = 0;
	}

	debug_exit();
	return ret;
}

static int comphy_usb3_power_up(u32 lane, void __iomem *hpipe_base, void __iomem *comphy_base)
{
	u32 mask, data, ret = 1;
	void __iomem *hpipe_addr = HPIPE_ADDR(hpipe_base, lane);
	void __iomem *comphy_addr = COMPHY_ADDR(comphy_base, lane);

	debug_enter();
	debug("stage: RFU configurations - hard reset comphy\n");
	/* RFU configurations - hard reset comphy */
	mask = COMMON_PHY_CFG1_PWR_UP_MASK;
	data = 0x1 << COMMON_PHY_CFG1_PWR_UP_OFFSET;
	mask |= COMMON_PHY_CFG1_PIPE_SELECT_MASK;
	data |= 0x1 << COMMON_PHY_CFG1_PIPE_SELECT_OFFSET;
	mask |= COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
	data |= 0x0 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
	mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
	data |= 0x0 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
	mask |= COMMON_PHY_PHY_MODE_MASK;
	data |= 0x1 << COMMON_PHY_PHY_MODE_OFFSET;
	reg_set(comphy_addr + COMMON_PHY_CFG1_REG, data, mask);

	/* release from hard reset */
	mask = COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
	data = 0x1 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
	mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
	data |= 0x1 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
	reg_set(comphy_addr + COMMON_PHY_CFG1_REG, data, mask);

	/* Wait 1ms - until band gap and ref clock ready */
	mdelay(1);

	/* Start comphy Configuration */
	debug("stage: Comphy configuration\n");
	/* Set PIPE soft reset */
	mask = HPIPE_RST_CLK_CTRL_PIPE_RST_MASK;
	data = 0x1 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET;
	/* Set PHY datapath width mode for V0 */
	mask |= HPIPE_RST_CLK_CTRL_FIXED_PCLK_MASK;
	data |= 0x0 << HPIPE_RST_CLK_CTRL_FIXED_PCLK_OFFSET;
	/* Set Data bus width USB mode for V0 */
	mask |= HPIPE_RST_CLK_CTRL_PIPE_WIDTH_MASK;
	data |= 0x0 << HPIPE_RST_CLK_CTRL_PIPE_WIDTH_OFFSET;
	/* Set CORE_CLK output frequency for 250Mhz */
	mask |= HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_MASK;
	data |= 0x0 << HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_OFFSET;
	reg_set(hpipe_addr + HPIPE_RST_CLK_CTRL_REG, data, mask);
	/* Set PLL ready delay for 0x2 */
	reg_set(hpipe_addr + HPIPE_CLK_SRC_LO_REG,
		0x2 << HPIPE_CLK_SRC_LO_PLL_RDY_DL_OFFSET, HPIPE_CLK_SRC_LO_PLL_RDY_DL_MASK);
	/* Set reference clock to come from group 1 - 25Mhz */
	reg_set(hpipe_addr + HPIPE_MISC_REG, 0x0 << HPIPE_MISC_REFCLK_SEL_OFFSET, HPIPE_MISC_REFCLK_SEL_MASK);
	/* Set reference frequcency select - 0x2 */
	mask = HPIPE_PWR_PLL_REF_FREQ_MASK;
	data = 0x2 << HPIPE_PWR_PLL_REF_FREQ_OFFSET;
	/* Set PHY mode to USB - 0x5 */
	mask |= HPIPE_PWR_PLL_PHY_MODE_MASK;
	data |= 0x5 << HPIPE_PWR_PLL_PHY_MODE_OFFSET;
	reg_set(hpipe_addr + HPIPE_PWR_PLL_REG, data, mask);
	/* Set the amount of time spent in the LoZ state - set for 0x7 */
	reg_set(hpipe_addr + HPIPE_GLOBAL_PM_CTRL,
		0x7 << HPIPE_GLOBAL_PM_RXDLOZ_WAIT_OFFSET, HPIPE_GLOBAL_PM_RXDLOZ_WAIT_MASK);
	/* Set max PHY generation setting - 5Gbps */
	reg_set(hpipe_addr + HPIPE_INTERFACE_REG,
		0x1 << HPIPE_INTERFACE_GEN_MAX_OFFSET, HPIPE_INTERFACE_GEN_MAX_MASK);
	/* Set select data width 20Bit (SEL_BITS[2:0]) */
	reg_set(hpipe_addr + HPIPE_LOOPBACK_REG,
		0x1 << HPIPE_LOOPBACK_SEL_OFFSET, HPIPE_LOOPBACK_SEL_MASK);

	/* Start analog paramters from ETP(HW) */
	debug("stage: Analog paramters from ETP(HW)\n");
	/* Set Pin DFE_PAT_DIS -> Bit[1]: PIN_DFE_PAT_DIS = 0x0 */
	mask = HPIPE_LANE_CFG4_DFE_CTRL_MASK;
	data = 0x1 << HPIPE_LANE_CFG4_DFE_CTRL_OFFSET;
	/* Set Override PHY DFE control pins for 0x1 */
	mask |= HPIPE_LANE_CFG4_DFE_OVER_MASK;
	data |= 0x1 << HPIPE_LANE_CFG4_DFE_OVER_OFFSET;
	/* Set Spread Spectrum Clock Enable fot 0x1 */
	mask |= HPIPE_LANE_CFG4_SSC_CTRL_MASK;
	data |= 0x1 << HPIPE_LANE_CFG4_SSC_CTRL_OFFSET;
	reg_set(hpipe_addr + HPIPE_LANE_CFG4_REG, data, mask);
	/* End of analog parameters */

	debug("stage: Comphy power up\n");
	/* Release from PIPE soft reset */
	reg_set(hpipe_addr + HPIPE_RST_CLK_CTRL_REG,
		0x0 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET, HPIPE_RST_CLK_CTRL_PIPE_RST_MASK);

	/* wait 10ms - for comphy calibration done */
	mdelay(10);

	debug("stage: Check PLL\n");
	/* Read lane status */
	data = readl(hpipe_addr + HPIPE_LANE_STATUS0_REG);
	if ((data & HPIPE_LANE_STATUS0_PCLK_EN_MASK) == 0) {
		error("HPIPE_LANE_STATUS0_PCLK_EN_MASK is 0\n");
		ret = 0;
	}

	debug_exit();
	return ret;
}

static int comphy_sata_power_up(u32 lane, void __iomem *hpipe_base, void __iomem *comphy_base)
{
	u32 mask, data, ret = 1;
	void __iomem *hpipe_addr = HPIPE_ADDR(hpipe_base, lane);
	void __iomem *sd_ip_addr = SD_ADDR(hpipe_base, lane);
	void __iomem *comphy_addr = COMPHY_ADDR(comphy_base, lane);
	void __iomem *sata_base;

	debug_enter();
	sata_base = (void __iomem *)MVEBU_SATA3_GENERAL_BASE;
	debug("SATA3 General address base %p\n", sata_base);

	debug("stage: MAC configuration - power down comphy\n");
	/* MAC configuration powe down comphy
	   use indirect address for vendor spesific SATA control register */
	reg_set(sata_base + SATA3_VENDOR_ADDRESS,
		SATA_CONTROL_REG << SATA3_VENDOR_ADDR_OFSSET, SATA3_VENDOR_ADDR_MASK);
	/* SATA 0 power down */
	mask = SATA3_CTRL_SATA0_PD_MASK;
	data = 0x1 << SATA3_CTRL_SATA0_PD_OFFSET;
	/* SATA 1 power down */
	mask |= SATA3_CTRL_SATA1_PD_MASK;
	data |= 0x1 << SATA3_CTRL_SATA1_PD_OFFSET;
	/* SATA SSU disable */
	mask |= SATA3_CTRL_SATA1_ENABLE_MASK;
	data |= 0x0 << SATA3_CTRL_SATA1_ENABLE_OFFSET;
	/* SATA port 1 disable */
	mask |= SATA3_CTRL_SATA_SSU_MASK;
	data |= 0x0 << SATA3_CTRL_SATA_SSU_OFFSET;
	reg_set(sata_base + SATA3_VENDOR_DATA, data, mask);

	debug("stage: RFU configurations - hard reset comphy\n");
	/* RFU configurations - hard reset comphy */
	mask = COMMON_PHY_CFG1_PWR_UP_MASK;
	data = 0x1 << COMMON_PHY_CFG1_PWR_UP_OFFSET;
	mask |= COMMON_PHY_CFG1_PIPE_SELECT_MASK;
	data |= 0x0 << COMMON_PHY_CFG1_PIPE_SELECT_OFFSET;
	mask |= COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
	data |= 0x0 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
	mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
	data |= 0x0 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
	reg_set(comphy_addr + COMMON_PHY_CFG1_REG, data, mask);

	/* Set select data  width 40Bit - SATA mode only */
	reg_set(comphy_addr + COMMON_PHY_CFG6_REG,
		0x1 << COMMON_PHY_CFG6_IF_40_SEL_OFFSET, COMMON_PHY_CFG6_IF_40_SEL_MASK);

	/* release from hard reset in SD external */
	mask = SD_EXTERNAL_CONFIG1_RESET_IN_MASK;
	data = 0x1 << SD_EXTERNAL_CONFIG1_RESET_IN_OFFSET;
	mask |= SD_EXTERNAL_CONFIG1_RESET_CORE_MASK;
	data |= 0x1 << SD_EXTERNAL_CONFIG1_RESET_CORE_OFFSET;
	reg_set(sd_ip_addr + SD_EXTERNAL_CONFIG1_REG, data, mask);

	/* Wait 1ms - until band gap and ref clock ready */
	mdelay(1);

	debug("stage: Comphy configuration\n");
	/* Start comphy Configuration */
	/* Set reference clock to comes from group 1 - choose 25Mhz */
	reg_set(hpipe_addr + HPIPE_MISC_REG,
		0x0 << HPIPE_MISC_REFCLK_SEL_OFFSET, HPIPE_MISC_REFCLK_SEL_MASK);
	/* Reference frequency select set 1 (for SATA = 25Mhz) */
	mask = HPIPE_PWR_PLL_REF_FREQ_MASK;
	data = 0x1 << HPIPE_PWR_PLL_REF_FREQ_OFFSET;
	/* PHY mode select (set SATA = 0x0 */
	mask |= HPIPE_PWR_PLL_PHY_MODE_MASK;
	data |= 0x0 << HPIPE_PWR_PLL_PHY_MODE_OFFSET;
	reg_set(hpipe_addr + HPIPE_PWR_PLL_REG, data, mask);
	/* Set max PHY generation setting - 6Gbps */
	reg_set(hpipe_addr + HPIPE_INTERFACE_REG,
		0x2 << HPIPE_INTERFACE_GEN_MAX_OFFSET, HPIPE_INTERFACE_GEN_MAX_MASK);
	/* Set select data  width 40Bit (SEL_BITS[2:0]) */
	reg_set(hpipe_addr + HPIPE_LOOPBACK_REG,
		0x2 << HPIPE_LOOPBACK_SEL_OFFSET, HPIPE_LOOPBACK_SEL_MASK);

	debug("stage: Analog paramters from ETP(HW)\n");
	/* TODO: Set analog paramters from ETP(HW) - for now use the default datas */

	/* DFE reset sequence */
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x1 << HPIPE_PWR_CTR_RST_DFE_OFFSET, HPIPE_PWR_CTR_RST_DFE_MASK);
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x0 << HPIPE_PWR_CTR_RST_DFE_OFFSET, HPIPE_PWR_CTR_RST_DFE_MASK);
	/* SW reset for interupt logic */
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x1 << HPIPE_PWR_CTR_SFT_RST_OFFSET, HPIPE_PWR_CTR_SFT_RST_MASK);
	reg_set(hpipe_addr + HPIPE_PWR_CTR_REG,
		0x0 << HPIPE_PWR_CTR_SFT_RST_OFFSET, HPIPE_PWR_CTR_SFT_RST_MASK);

	debug("stage: Comphy power up\n");
	/* MAC configuration power up comphy - power up PLL/TX/RX
	   use indirect address for vendor spesific SATA control register */
	reg_set(sata_base + SATA3_VENDOR_ADDRESS,
		SATA_CONTROL_REG << SATA3_VENDOR_ADDR_OFSSET, SATA3_VENDOR_ADDR_MASK);
	/* SATA 0 power up */
	mask = SATA3_CTRL_SATA0_PD_MASK;
	data = 0x0 << SATA3_CTRL_SATA0_PD_OFFSET;
	/* SATA 1 power up */
	mask |= SATA3_CTRL_SATA1_PD_MASK;
	data |= 0x0 << SATA3_CTRL_SATA1_PD_OFFSET;
	/* SATA SSU enable */
	mask |= SATA3_CTRL_SATA1_ENABLE_MASK;
	data |= 0x1 << SATA3_CTRL_SATA1_ENABLE_OFFSET;
	/* SATA port 1 enable */
	mask |= SATA3_CTRL_SATA_SSU_MASK;
	data |= 0x1 << SATA3_CTRL_SATA_SSU_OFFSET;
	reg_set(sata_base + SATA3_VENDOR_DATA, data, mask);

	/* Wait 10ms - Wait for comphy calibration done */
	mdelay(10);

	/* MBUS request size and interface select register */
	reg_set(sata_base + SATA3_VENDOR_ADDRESS,
		SATA_MBUS_SIZE_SELECT_REG << SATA3_VENDOR_ADDR_OFSSET, SATA3_VENDOR_ADDR_MASK);
	/* Mbus regret enable */
	reg_set(sata_base + SATA3_VENDOR_DATA, 0x1 << SATA_MBUS_REGRET_EN_OFFSET, SATA_MBUS_REGRET_EN_MASK);

	debug("stage: Check PLL\n");
	data = readl(sd_ip_addr + SD_EXTERNAL_STATUS0_REG);

	/* check the PLL TX */
	if ((data & SD_EXTERNAL_STATUS0_PLL_TX_MASK) == 0) {
		error("SD_EXTERNAL_STATUS0_PLL_TX is 0\n");
		ret = 0;
	}

	/* check the PLL RX */
	if ((data & SD_EXTERNAL_STATUS0_PLL_RX_MASK) == 0) {
		error("SD_EXTERNAL_STATUS0_PLL_RX is 0\n");
		ret = 0;
	}

	debug_exit();
	return ret;
}

static u32 comphy_utmi_phy_init(u32 utmi_index, void __iomem *utmi_base_addr,
				void __iomem *usb_cfg_addr, void __iomem *utmi_cfg_addr, u32 utmi_phy_port)
{
	u32 mask, data, ret = 1;

	debug_enter();

	debug("stage: Power down transceiver (power down Phy), Power down PLL, and SuspendDM\n");
	/* Power down UTMI PHY */
	reg_set(utmi_cfg_addr, 0x0 << UTMI_PHY_CFG_PU_OFFSET, UTMI_PHY_CFG_PU_MASK);
	/* Config USB3 Device UTMI enable */
	mask = UTMI_USB_CFG_DEVICE_EN_MASK;
	/* Prior to PHY init, configure mux for Device
	(Device can be connected to UTMI0 or to UTMI1) */
	if (utmi_phy_port == UTMI_PHY_TO_USB_DEVICE0) {
		data = 0x1 << UTMI_USB_CFG_DEVICE_EN_OFFSET;
		/* Config USB3 Device UTMI MUX */
		mask |= UTMI_USB_CFG_DEVICE_MUX_MASK;
		data |= utmi_index << UTMI_USB_CFG_DEVICE_MUX_OFFSET;
	} else {
		data = 0x0 << UTMI_USB_CFG_DEVICE_EN_OFFSET;
	}
	/* Power down PLL */
	mask |= UTMI_USB_CFG_PLL_MASK;
	data |= 0x0 << UTMI_USB_CFG_PLL_OFFSET;
	reg_set(usb_cfg_addr, data, mask);
	/* Set Test suspendm mode */
	mask = UTMI_CTRL_STATUS0_SUSPENDM_MASK;
	data = 0x1 << UTMI_CTRL_STATUS0_SUSPENDM_OFFSET;
	/* Enable Test UTMI select */
	mask |= UTMI_CTRL_STATUS0_TEST_SEL_MASK;
	data |= 0x1 << UTMI_CTRL_STATUS0_TEST_SEL_OFFSET;
	reg_set(utmi_base_addr + UTMI_CTRL_STATUS0_REG, data, mask);

	/* Wait for UTMI power down */
	mdelay(1);

	debug("stage: Configure UTMI PHY registers\n");
	/* Reference Clock Divider Select */
	mask = UTMI_PLL_CTRL_REFDIV_MASK;
	data = 0x5 << UTMI_PLL_CTRL_REFDIV_OFFSET;
	/* Feedback Clock Divider Select - 90 for 25Mhz*/
	mask |= UTMI_PLL_CTRL_FBDIV_MASK;
	data |= 60 << UTMI_PLL_CTRL_FBDIV_OFFSET;
	/* Select LPFR - 0x0 for 25Mhz/5=5Mhz*/
	mask |= UTMI_PLL_CTRL_SEL_LPFR_MASK;
	data |= 0x0 << UTMI_PLL_CTRL_SEL_LPFR_OFFSET;
	reg_set(utmi_base_addr + UTMI_PLL_CTRL_REG, data, mask);

	/* Impedance Calibration Threshold Setting */
	reg_set(utmi_base_addr + UTMI_CALIB_CTRL_REG,
		0x6 << UTMI_CALIB_CTRL_IMPCAL_VTH_OFFSET, UTMI_CALIB_CTRL_IMPCAL_VTH_MASK);

	/* Set LS TX driver strength coarse control */
	mask = UTMI_TX_CH_CTRL_DRV_EN_LS_MASK;
	data = 0x3 << UTMI_TX_CH_CTRL_DRV_EN_LS_OFFSET;
	/* Set LS TX driver fine adjustment */
	mask |= UTMI_TX_CH_CTRL_IMP_SEL_LS_MASK;
	data |= 0x3 << UTMI_TX_CH_CTRL_IMP_SEL_LS_OFFSET;
	reg_set(utmi_base_addr + UTMI_TX_CH_CTRL_REG, data, mask);

	/* Enable SQ */
	mask = UTMI_RX_CH_CTRL0_SQ_DET_MASK;
	data = 0x0 << UTMI_RX_CH_CTRL0_SQ_DET_OFFSET;
	/* Enable analog squelch detect */
	mask |= UTMI_RX_CH_CTRL0_SQ_ANA_DTC_MASK;
	data |= 0x1 << UTMI_RX_CH_CTRL0_SQ_ANA_DTC_OFFSET;
	reg_set(utmi_base_addr + UTMI_RX_CH_CTRL0_REG, data, mask);

	/* Set External squelch calibration number */
	mask = UTMI_RX_CH_CTRL1_SQ_AMP_CAL_MASK;
	data = 0x1 << UTMI_RX_CH_CTRL1_SQ_AMP_CAL_OFFSET;
	/* Enable the External squelch calibration */
	mask |= UTMI_RX_CH_CTRL1_SQ_AMP_CAL_EN_MASK;
	data |= 0x1 << UTMI_RX_CH_CTRL1_SQ_AMP_CAL_EN_OFFSET;
	reg_set(utmi_base_addr + UTMI_RX_CH_CTRL1_REG, data, mask);

	/* Set Control VDAT Reference Voltage - 0.325V */
	mask = UTMI_CHGDTC_CTRL_VDAT_MASK;
	data = 0x1 << UTMI_CHGDTC_CTRL_VDAT_OFFSET;
	/* Set Control VSRC Reference Voltage - 0.6V */
	mask |= UTMI_CHGDTC_CTRL_VSRC_MASK;
	data |= 0x1 << UTMI_CHGDTC_CTRL_VSRC_OFFSET;
	reg_set(utmi_base_addr + UTMI_CHGDTC_CTRL_REG, data, mask);

	debug("stage: Power up transceiver(Power up Phy), Power up PLL, and exit SuspendDM\n");
	/* Power UP UTMI PHY */
	reg_set(utmi_cfg_addr, 0x1 << UTMI_PHY_CFG_PU_OFFSET, UTMI_PHY_CFG_PU_MASK);
	/* Power up PLL */
	reg_set(usb_cfg_addr, 0x1 << UTMI_USB_CFG_PLL_OFFSET, UTMI_USB_CFG_PLL_MASK);
	/* Disable Test UTMI select */
	reg_set(utmi_base_addr + UTMI_CTRL_STATUS0_REG,
		0x1 << UTMI_CTRL_STATUS0_TEST_SEL_OFFSET, UTMI_CTRL_STATUS0_TEST_SEL_MASK);

	debug("stage: Wait for PLL and impedance calibration done, and PLL ready done\n");
	mdelay(5);

	debug("stage: Check PLL\n");
	data = readl(utmi_base_addr + UTMI_CALIB_CTRL_REG);
	if ((data & UTMI_CALIB_CTRL_IMPCAL_DONE_MASK) == 0) {
		error("Impedance calibration is not done\n");
		ret = 0;
	}
	if ((data & UTMI_CALIB_CTRL_PLLCAL_DONE_MASK) == 0) {
		error("PLL calibration is not done\n");
		ret = 0;
	}
	data = readl(utmi_base_addr + UTMI_PLL_CTRL_REG);
	if ((data & UTMI_PLL_CTRL_PLL_RDY_MASK) == 0) {
		error("PLL is not ready\n");
		ret = 0;
	}

	debug_exit();
	return ret;
}

/* comphy_dedicated_phys_init initialize the dedicated PHYs - not muxed SerDes lanes
** e.g. UTMI PHY */
static void comphy_dedicated_phys_init(void)
{
	void __iomem *utmi_base_addr, *usb_cfg_addr, *utmi_cfg_addr;
	int node_list[MAX_UTMI_PHY_COUNT], node;
	int i, count, utmi_phy_port;

	debug_enter();
	debug("Initialize USB UTMI PHYs\n");
	/* Find the UTMI phy node in device tree and go over them */
	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "utmi",
			COMPAT_MVEBU_UTMI_PHY, node_list, MAX_UTMI_PHY_COUNT);

	/* check if UTMI phy nodes exists */
	if (count == 0) {
		debug("could not find UTMI phy node in FDT, initialization skipped!\n");
	} else {
		/* Go over UTMI phy node and get the base address & call UTMI phy */
		for (i = 0 ; i < count ; i++) {
			node = node_list[i];
			if (node <= 0)
				continue;
			/* get base address of UTMI phy */
			utmi_base_addr = fdt_get_regs_offs(gd->fdt_blob, node, "reg-utmi-unit");
			if (utmi_base_addr == NULL) {
				error("UTMI PHY base address is invalid\n");
				continue;
			}
			/* get usb config address */
			usb_cfg_addr = fdt_get_regs_offs(gd->fdt_blob, node, "reg-usb-cfg");
			if (usb_cfg_addr == NULL) {
				error("UTMI PHY base address is invalid\n");
				continue;
			}
			/* get UTMI config address */
			utmi_cfg_addr = fdt_get_regs_offs(gd->fdt_blob, node, "reg-utmi-cfg");
			if (utmi_cfg_addr == NULL) {
				error("UTMI PHY base address is invalid\n");
				continue;
			}
			/* get the port number (to check if the utmi connected to host/device) */
			utmi_phy_port = fdtdec_get_int(gd->fdt_blob, node, "utmi-port", UTMI_PHY_INVALID);
			if (utmi_phy_port == UTMI_PHY_INVALID) {
				error("UTMI PHY port type is invalid\n");
				continue;
			}
			if (!comphy_utmi_phy_init(i, utmi_base_addr, usb_cfg_addr, utmi_cfg_addr, utmi_phy_port)) {
				error("Failed to initialize UTMI PHY %d\n", i);
				continue;
			}
			printf("UTMI PHY %d initizliazed to ", i);
			if (utmi_phy_port == UTMI_PHY_TO_USB_DEVICE0)
				printf("USB Device\n");
			else
				printf("USB Host%d\n", utmi_phy_port);
		}
	}

	debug_exit();
}

static void comphy_mux_cp110_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map)
{
	void __iomem *comphy_base_addr;
	struct comphy_map comphy_map_pipe_data[MAX_LANE_OPTIONS];
	struct comphy_map comphy_map_phy_data[MAX_LANE_OPTIONS];
	u32 lane, comphy_max_count;

	comphy_max_count = ptr_chip_cfg->comphy_lanes_count;
	comphy_base_addr = ptr_chip_cfg->comphy_base_addr;

	/* Copy the SerDes map configuration for PIPE map and PHY map
	   the comphy_mux_init modify the type of the lane if the type is not valid
	   because we have 2 selectores run the comphy_mux_init twice and after
	   that update the original serdes_map */
	for (lane = 0; lane < comphy_max_count; lane++) {
		comphy_map_pipe_data[lane].type = serdes_map[lane].type;
		comphy_map_pipe_data[lane].speed = serdes_map[lane].speed;
		comphy_map_phy_data[lane].type = serdes_map[lane].type;
		comphy_map_phy_data[lane].speed = serdes_map[lane].speed;
	}
	ptr_chip_cfg->mux_data = cp110_comphy_phy_mux_data;
	comphy_mux_init(ptr_chip_cfg, comphy_map_phy_data, comphy_base_addr + COMMON_SELECTOR_PHY_OFFSET);

	ptr_chip_cfg->mux_data = cp110_comphy_pipe_mux_data;
	comphy_mux_init(ptr_chip_cfg, comphy_map_pipe_data, comphy_base_addr + COMMON_SELECTOR_PIPE_OFFSET);
	/* Fix the type after check the PHY and PIPE configuration */
	for (lane = 0; lane < comphy_max_count; lane++)
		if ((comphy_map_pipe_data[lane].type == PHY_TYPE_UNCONNECTED) &&
		    (comphy_map_phy_data[lane].type == PHY_TYPE_UNCONNECTED))
			serdes_map[lane].type = PHY_TYPE_UNCONNECTED;
}
int comphy_cp110_init(struct chip_serdes_phy_config *ptr_chip_cfg, struct comphy_map *serdes_map)
{
	struct comphy_map *ptr_comphy_map;
	void __iomem *comphy_base_addr, *hpipe_base_addr;
	u32 comphy_max_count, lane, ret = 0;
	u32 pcie_by4 = 1;

	debug_enter();

	comphy_max_count = ptr_chip_cfg->comphy_lanes_count;
	comphy_base_addr = ptr_chip_cfg->comphy_base_addr;
	hpipe_base_addr = ptr_chip_cfg->hpipe3_base_addr;

	/* Config Comphy mux configuration */
	comphy_mux_cp110_init(ptr_chip_cfg, serdes_map);

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
			ret = comphy_pcie_power_up(lane, pcie_by4, hpipe_base_addr, comphy_base_addr);
			break;
		case PHY_TYPE_SATA0:
		case PHY_TYPE_SATA1:
		case PHY_TYPE_SATA2:
		case PHY_TYPE_SATA3:
			ret = comphy_sata_power_up(lane, hpipe_base_addr, comphy_base_addr);
			break;
		case PHY_TYPE_USB3_HOST0:
		case PHY_TYPE_USB3_HOST1:
			ret = comphy_usb3_power_up(lane, hpipe_base_addr, comphy_base_addr);
			break;
		default:
			debug("Unknown SerDes type, skip initialize SerDes %d\n", lane);
			break;
		}
		if (ret == 0)
			error("PLL is not locked - Failed to initialize lane %d\n", lane);
	}

	/* Initialize dedicated PHYs (not muxed SerDes lanes) */
	comphy_dedicated_phys_init();

	debug_exit();
	return 0;
}
