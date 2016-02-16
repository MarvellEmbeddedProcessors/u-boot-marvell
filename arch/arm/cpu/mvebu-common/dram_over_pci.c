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

#include <common.h>
#include <pci.h>
#include <asm/io.h>
#include <asm/arch-mvebu/comphy.h>

/* mapping addresses */
#define DRAM_OFFSET				0
#define PCI_CONFIGURATION_OFFSET		0x50000000
#define PCI_DEVICE_CONFIG_SPACE			0xF1000000
#define DRAM_WIN_SIZE				0x80000000

/* BARs offset */
#define BAR0_LOW_ADDR_OFFSET	0x10
#define BAR0_HIGH_ADDR_OFFSET	0x14
#define BAR1_LOW_ADDR_OFFSET	0x18
#define BAR1_HIGH_ADDR_OFFSET	0x1c

#define	PCI_DEVICE_INIT_DELAY	3000000

#ifdef CONFIG_TARGET_ARMADA_8K

/* common defines */
#define WIN_ENABLE_BIT			(0x1)
#define WIN_DISABLE_BIT			(0x0)

/* Physical address of the base of the window = {AddrLow[19:0],20’h0} */
#define ADDRESS_SHIFT			(20 - 4)
#define ADDRESS_MASK			(0xFFFFFFF0)
#define CCU_WIN_ALIGNMENT		(0x100000)

/* AP registers */
#define SPL_AP_BASE				0xF0000000
#define CCU_BASE				(SPL_AP_BASE + 0x4000)
#define CCU_DDR_WIN				(2)
#define	CCU_IO0_TARGET_ID		(0)
#define CCU_WIN_CR_OFFSET(win)		(CCU_BASE + 0x0 + (0x10 * win))
#define CCU_TARGET_ID_OFFSET		(8)
#define CCU_TARGET_ID_MASK			(0x7F)

#define CCU_WIN_SCR_OFFSET(win)		(CCU_BASE + 0x4 + (0x10 * win))
#define CCU_WIN_ENA_READ_SECURE		(0x1)
#define CCU_WIN_ENA_WRITE_SECURE	(0x2)

#define CCU_WIN_ALR_OFFSET(win)		(CCU_BASE + 0x8 + (0x10 * win))
#define CCU_WIN_AHR_OFFSET(win)		(CCU_BASE + 0xC + (0x10 * win))

/* RFU defines */
#define RFU_WIN_PEX_CFG			(SPL_AP_BASE + 0x6F0208)
#define RFU_WIN_PEX_ALR			(SPL_AP_BASE + 0x6F0218)
#define RFU_WIN_PEX_AHR			(SPL_AP_BASE + 0x6F021c)
#define RFU_WIN_PEX_CFG_ADDR_SHIFT	16
#define RFU_WIN_PEX_ALR_ADDR_OFFSET 4

/* MAC regsisters */
#define	PCIE_DEBUG_STATUS		(PCI_CONFIGURATION_OFFSET + 0x1A64)

void dram_over_pci_window_config(void)
{
	u32 alr, ahr, val;

	alr = (u32)((DRAM_OFFSET >> ADDRESS_SHIFT) & ADDRESS_MASK);
	ahr = (u32)(((DRAM_OFFSET + DRAM_WIN_SIZE - 1) >> ADDRESS_SHIFT)
				& ADDRESS_MASK);

	/* disable DDR window opened by BootROM */
	writel(WIN_DISABLE_BIT, CCU_WIN_CR_OFFSET(CCU_DDR_WIN));

	/* set PEX RFU - enable, base address = 0 */
	val = (WIN_ENABLE_BIT | (alr << RFU_WIN_PEX_ALR_ADDR_OFFSET));
	writel(val, RFU_WIN_PEX_ALR);

	/* high address = 0xfffffff */
	writel(ahr, RFU_WIN_PEX_AHR);

	/* configuration address = 0x50000000 */
	val = (PCI_CONFIGURATION_OFFSET >> RFU_WIN_PEX_CFG_ADDR_SHIFT);
	writel(val, RFU_WIN_PEX_CFG);
}

#elif defined(CONFIG_TARGET_ARMADA_3700)

void dram_over_pci_window_config(void)
{
}

#elif defined(CONFIG_TARGET_ARMADA_38X)

void dram_over_pci_window_config(void)
{
	/* disable DRAM */
	writel(0, 0xd0020184);

	/* L2 filtering - enable access PCAC dram from address 0 */
	writel(0x40000001, 0xd0008c00);
	writel(0x4ff00000, 0xd0008c04);

	/* open window */
	writel(0x3fffe881, 0xd0020000);
	writel(0, 0xd0020004);
	writel(0, 0xd0020008);

	/* close 0xf0 window to avoid collision */
	writel(0, 0xd00200b0);

	/* reset global timer - required by the BootROM */
	writel(1, 0xd0020300);
}
#endif

void dram_over_pci_init(const void *fdt_blob)
{
	int linkup_timeout_ms = 1000;
	struct pci_controller *hose;
	pci_dev_t bdf;

	dram_over_pci_window_config();

	/* wait until the PCIE card finish */
	udelay(PCI_DEVICE_INIT_DELAY);
	comphy_init(fdt_blob);

#ifdef CONFIG_TARGET_ARMADA_8K

	/* wait untill link training is done */
	while (linkup_timeout_ms) {
		if ((readl(PCIE_DEBUG_STATUS) & 0x7F) == 0x7E)
			break;
		udelay(1000);
		linkup_timeout_ms--;
	}

	/* Check for linkup */
	if (linkup_timeout_ms == 0) {
		error("PCIe didn't reach linkup");
		printf("LTSSM reg = 0x%08x\n", readl(PCIE_DEBUG_STATUS));
	}
#else
	mdelay(linkup_timeout_ms);

#endif /* CONFIG_TARGET_ARMADA_8K */

	pci_init();

#ifdef CONFIG_MVEBU_SPL_DDR_OVER_PCI_SWITCH
	/* open 0 - 2G for address space beyound the main switch */
	hose = pci_bus_to_hose(1);
	bdf  = PCI_BDF(1, 0, 0);
	hose->write_dword(hose, bdf, 0x20, 0x7fff0000);

	/* open 0 - 1G for PCAC located behind for switch port */
	hose = pci_bus_to_hose(2);
	bdf  = PCI_BDF(2, 2, 0);
	hose->write_dword(hose, bdf, 0x20, 0x3fff0000);
	hose->write_dword(hose, bdf, 0x0, 0x0);

	/* set DRAM device PCIE bars:
	   bar 0 configuration space = 0xf1000000
	   bar 1  address = 0x0 - dram address */
	hose = pci_bus_to_hose(3);
	bdf  = PCI_BDF(3, 0, 0);
#else
	hose = pci_bus_to_hose(1);
	bdf  = PCI_BDF(1, 0, 0);
#endif
	hose->write_dword(hose, bdf, BAR0_LOW_ADDR_OFFSET, PCI_DEVICE_CONFIG_SPACE);
	hose->write_dword(hose, bdf, BAR0_HIGH_ADDR_OFFSET, 0);
	hose->write_dword(hose, bdf, BAR1_LOW_ADDR_OFFSET, DRAM_OFFSET);
	hose->write_dword(hose, bdf, BAR1_HIGH_ADDR_OFFSET, 0);
}
