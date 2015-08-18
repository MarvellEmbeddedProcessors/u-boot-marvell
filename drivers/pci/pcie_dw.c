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
/*
** Based on:
**   - drivers/pci/pcie_imx.c
**   - drivers/pci/pci_mvebu.c
*/

#include <common.h>
#include <pci.h>
#include <fdtdec.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <errno.h>
#include <asm/arch-mvebu/fdt.h>
#include "pcie_dw.h"


DECLARE_GLOBAL_DATA_PTR;

#define MAX_PCIE_PORTS	10

/* iATU registers */
#define PCIE_ATU_VIEWPORT		0x900
#define PCIE_ATU_REGION_INBOUND		(0x1 << 31)
#define PCIE_ATU_REGION_OUTBOUND	(0x0 << 31)
#define PCIE_ATU_REGION_INDEX1		(0x1 << 0)
#define PCIE_ATU_REGION_INDEX0		(0x0 << 0)
#define PCIE_ATU_CR1			0x904
#define PCIE_ATU_TYPE_MEM		(0x0 << 0)
#define PCIE_ATU_TYPE_IO		(0x2 << 0)
#define PCIE_ATU_TYPE_CFG0		(0x4 << 0)
#define PCIE_ATU_TYPE_CFG1		(0x5 << 0)
#define PCIE_ATU_CR2			0x908
#define PCIE_ATU_ENABLE			(0x1 << 31)
#define PCIE_ATU_BAR_MODE_ENABLE	(0x1 << 30)
#define PCIE_ATU_LOWER_BASE		0x90C
#define PCIE_ATU_UPPER_BASE		0x910
#define PCIE_ATU_LIMIT			0x914
#define PCIE_ATU_LOWER_TARGET		0x918
#define PCIE_ATU_BUS(x)			(((x) & 0xff) << 24)
#define PCIE_ATU_DEV(x)			(((x) & 0x1f) << 19)
#define PCIE_ATU_FUNC(x)		(((x) & 0x7) << 16)
#define PCIE_ATU_UPPER_TARGET		0x91C

/*
 * iATU region setup
 */
static int dw_pcie_regions_setup(uintptr_t regs_base, struct pcie_win *cfg_win)
{
	/* Region #0 is used for Outbound CFG space access.
	** Direction = Outbound
	** Region Index = 0
	*/
	writel(0, regs_base + PCIE_ATU_VIEWPORT);

	writel(cfg_win->base, regs_base + PCIE_ATU_LOWER_BASE);
	writel(0, regs_base + PCIE_ATU_UPPER_BASE);
	writel(cfg_win->base + cfg_win->size, regs_base + PCIE_ATU_LIMIT);

	writel(0, regs_base + PCIE_ATU_LOWER_TARGET);
	writel(0, regs_base + PCIE_ATU_UPPER_TARGET);
	writel(PCIE_ATU_TYPE_CFG0, regs_base + PCIE_ATU_CR1);
	writel(PCIE_ATU_ENABLE, regs_base + PCIE_ATU_CR2);

	return 0;
}

/*
 * PCI Express accessors
 */
static uintptr_t set_cfg_address(struct pci_controller *hose, uintptr_t regs_base, pci_dev_t d, int where)
{
	uintptr_t va_address;

	/* Region #0 is used for Outbound CFG space access.
	** Direction = Outbound
	** Region Index = 0
	*/
	writel(0, regs_base + PCIE_ATU_VIEWPORT);

	if (PCI_BUS(d) == (hose->first_busno + 1))
		/* For local bus, change TLP Type field to 4. */
		writel(PCIE_ATU_TYPE_CFG0, regs_base + PCIE_ATU_CR1);
	else
		/* Otherwise, change TLP Type field to 5. */
		writel(PCIE_ATU_TYPE_CFG1, regs_base + PCIE_ATU_CR1);

	if (PCI_BUS(d) == hose->first_busno) {
		/* Accessing root port configuration space. */
		va_address = regs_base;
	} else {
		writel(d << 8, regs_base + PCIE_ATU_LOWER_TARGET);
		va_address = (uintptr_t)hose->cfg_addr;
	}

	va_address += (where & ~0x3);

	return va_address;
}

static int dw_pcie_addr_valid(pci_dev_t d, int first_busno)
{
	/*
	 * In PCIE-E only a single device (0) can exist
	 * on the local bus. Beyound the local bus, there might be
	 * a Switch and everyting is possible.
	 */
	if ((PCI_BUS(d) == first_busno) && (PCI_DEV(d) > 0))
		return 0;
	return 1;
}

static int dw_pcie_read_config(struct pci_controller *hose, pci_dev_t bdf,
				int where, u32 *val)
{
	uintptr_t va_address;

	debug("PCIE CFG read:  (b,d,f)=(%2ld,%2ld,%2ld) ", PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));

	if (!dw_pcie_addr_valid(bdf, hose->first_busno)) {
		debug("address out of range\n");
		*val = 0xffffffff;
		return 1;
	}

	va_address = set_cfg_address(hose, (uintptr_t)hose->priv_data, bdf, where);

	writel(0xffffffff, val);
	*val = readl(va_address);

	debug("(addr,val)=(0x%04x, 0x%08x)\n", where, (*val));

	return 0;
}


static int dw_pcie_write_config(struct pci_controller *hose, pci_dev_t bdf,
			int where, u32 val)
{
	uintptr_t va_address = 0;

	debug("PCIE CFG write: (b,d,f)=(%2ld,%2ld,%2ld) ", PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));
	debug("(addr,val)=(0x%04x, 0x%08x)\n", where, val);

	if (!dw_pcie_addr_valid(bdf, hose->first_busno)) {
		debug("address out of range\n");
		return 1;
	}

	va_address = set_cfg_address(hose, (uintptr_t)hose->priv_data, bdf, where);

	writel(val, va_address);

	return 0;
}

/*
 * We dont use a host bridge so don't let the
 * stack skip CFG cycle for dev = 0 func = 0
 */
int pci_skip_dev(struct pci_controller *hose, pci_dev_t dev)
{
	return 0;
}

static struct pci_controller	pci_hose[MAX_PCIE_PORTS];
int dw_pcie_init(int host_id, uintptr_t regs_base, struct pcie_win *mem_win,
		struct pcie_win *cfg_win, int first_busno)
{
	struct pci_controller *hose = &pci_hose[host_id];
	uintptr_t temp;
	u32 reg;

	dw_pcie_regions_setup(regs_base, cfg_win);

	/* PCI MEM space */
	pci_set_region(&hose->regions[0], mem_win->base, mem_win->base, mem_win->size, PCI_REGION_MEM);
	hose->region_count = 1;

	pci_set_ops(hose,
		    pci_hose_read_config_byte_via_dword,
		    pci_hose_read_config_word_via_dword,
		    dw_pcie_read_config,
		    pci_hose_write_config_byte_via_dword,
		    pci_hose_write_config_word_via_dword,
		    dw_pcie_write_config);

	/* CFG cycle pointers */
	temp = (uintptr_t)cfg_win->base; /* Overcome compiler casting warning. */
	hose->cfg_addr = (unsigned int *)temp;

	hose->priv_data = (void *)regs_base;
	hose->first_busno = first_busno;

	/* CMD reg:I/O space, MEM space, and Bus Master Enable */
	reg = readl(regs_base + PCI_COMMAND);
	reg |= (PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);
	writel(reg, regs_base + PCI_COMMAND);

	/* Set the CLASS_REV of RC CFG header to PCI_CLASS_BRIDGE_PCI */
	reg = readl(regs_base + PCI_CLASS_REVISION);
	reg &= ~(0xFFFF << 16);
	reg |= (PCI_CLASS_BRIDGE_PCI << 16);
	writel(reg, regs_base + PCI_CLASS_REVISION);

	/* Register the host */
	pci_register_hose(hose);

	/* Scan the bus */
	hose->last_busno = pci_hose_scan(hose);

	return hose->last_busno + 1;
}


int dw_pcie_set_endpoint(u32 hid, u32 regs_base)
{
	printf("PCIE-%d: End point mode not supported yet.\n", hid);
	return 0;
}


