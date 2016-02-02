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
#include <fdtdec.h>
#include <asm/io.h>
#include <asm/arch/memory-map.h>
#include <asm/arch-mvebu/fdt.h>
#include <errno.h>
#include <linux/sizes.h>

/* #define DEBUG */
/* #define DEBUG_CFG_CYCLE */
#ifdef DEBUG_CFG_CYCLE
#define debug_cfg(fmt, args...) printf(fmt, ##args)
#else
#define debug_cfg(fmt, args...)
#endif

/*
 * PCIe unit register offsets.
 */

/* Standard PCI-E header reigsters */
#define PCIE_DEV_ID_OFF(x)		(x + 0x0)
#define PCIE_CMD_OFF(x)			(x + 0x4)
#define PCIE_DEV_REV_OFF(x)		(x + 0x8)
#define PCIE_CAP_OFF(x)			(x + 0x60)
#define PCIE_CAP_DEV_TYPE_MASK		(0xF << 20)
#define PCIE_CAP_DEV_TYPE_EP		(0x1 << 20)
#define PCIE_CAP_DEV_TYPE_RC		(0x4 << 20)

#define PCIE_BAR_LO_OFF(x, n)		(x + (0x10 + (0x8 * n)))
#define PCIE_BAR_HI_OFF(x, n)		(x + (0x14 + (0x8 * n)))
#define PCIE_BAR_CTRL_OFF(x, n)		(x + (0x1800 + (0x4 * n)))
#define PCIE_BAR_TYPE_MEM		(0xC)
#define PCIE_BAR_ENABLE			(0x1)
#define PCIE_BAR_CNT			(3)

#define PCIE_CONTROL			(0x1A00)
#define PCIE_OUTSTANDING_REQ_MASK	(3 << 8)

#define PCIE_FLOW_CONTROL		(0x1A20)
#define POSTED_CREDIT_MASK		(0xFF)
#define POSTED_CREDIT(x)		(0xFF)
#define NONPOSTED_CREDIT_MASK		(0xFF << 8)
#define NONPOSTED_CREDIT(x)		(x << 8)

/* Memory access control */
#define PCIE_WIN_OFF(n)			((n < 5) ? (0x0) : (0x10))
#define PCIE_WIN_CTRL_OFF(x, n)		(x + 0x1820 + PCIE_WIN_OFF(n) + (0x10 * n))
#define PCIE_WIN_BASE_OFF(x, n)		(x + 0x1824 + PCIE_WIN_OFF(n) + (0x10 * n))
#define PCIE_WIN_REMAP_OFF(x, n)	(x + 0x182C + PCIE_WIN_OFF(n) + (0x10 * n))
#define PCIE_DDR_TARGET_ID		(0)
#define PCIE_XBAR_WIN_CNT		(6)

/* Configuration access */
#define PCIE_CONF_ADDR_OFF(x)		(x + 0x18f8)
#define PCIE_CONF_DATA_OFF(x)		(x + 0x18fc)
#define PCIE_CONF_ADDR_EN		0x80000000
#define PCIE_CONF_REG(r)		((((r) & 0xf00) << 16) | ((r) & 0xfc))
#define PCIE_CONF_BUS(b)		(((b) & 0xff) << 16)
#define PCIE_CONF_DEV(d)		(((d) & 0x1f) << 11)
#define PCIE_CONF_FUNC(f)		(((f) & 0x7) << 8)
#define PCIE_CONF_ADDR(bdf, where)	(bdf | PCIE_CONF_REG(where) | PCIE_CONF_ADDR_EN)

/* Controler status */
#define PCIE_STAT_OFF(x)	(x + 0x1a04)
#define PCIE_STAT_BUS_OFF	(8)
#define PCIE_STAT_BUS_MASK	(0xFF << PCIE_STAT_BUS_OFF)
#define PCIE_STAT_DEV_OFF	(16)
#define PCIE_STAT_DEV_MASK	(0x1F << PCIE_STAT_DEV_OFF)
#define PCIE_STAT_LINK		(1)

/* Link status */
#define PCIE_DBG_STATUS_OFF(x)	(x + 0x1a64)
#define PCIE_LTSSM_LINK_UP	0x7E
#define PCIE_LTSSM_MASK		0x7F
#define PCIE_LINK_CTL_OFF(x)	(x + 0x70)

#define MAX_PCIE_PORTS		10
#ifdef CONFIG_TARGET_ARMADA_8K
/* Set First PCIe Bus to 1. Because when set to 0, Linux bus enumeration
** for PCIe bridge does not work. */
#define FIRST_PCI_BUS		1
#else
#define FIRST_PCI_BUS		0
#endif

DECLARE_GLOBAL_DATA_PTR;

struct pcie_win {
	u32 base;
	u32 size;
};

static int mvebu_pcie_addr_valid(pci_dev_t d, int first_busno)
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

static int mvebu_pcie_read_config(struct pci_controller *hose, pci_dev_t bdf,
				  int where, u32 *val)
{
	if (!mvebu_pcie_addr_valid(bdf, hose->first_busno)) {
		debug_cfg("CFG read: address out of range (%ld,%ld,%ld)\n",
			  PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));
		*val = 0xFFFFFFFF;
		return 1;
	}
	writel(PCIE_CONF_ADDR(bdf, where), hose->cfg_addr);

	*val = readl(hose->cfg_data);

	debug_cfg("PCIE CFG read:  (b,d,f)=(%2ld,%2ld,%2ld) (addr,val)=(0x%04x, 0x%08x)\n",
		  PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), where, (*val));

	return 0;
}

static int mvebu_pcie_write_config(struct pci_controller *hose, pci_dev_t bdf,
				   int where, u32 val)
{
	if (!mvebu_pcie_addr_valid(bdf, hose->first_busno)) {
		debug_cfg("CFG write: address out of range (%ld,%ld,%ld)\n",
			  PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));
		return 1;
	}

#if defined(CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT) && !defined(CONFIG_SPL_BUILD)
	/* Control modifications to switch ports that host
	 * the DDR controller maintain DDR access over PCI */
	if ((bdf == PCI_BDF(1, 0, 0)) || (bdf == PCI_BDF(2, 2, 0))) {
		/* Never disable Master & Memory capabilities */
		if (where == PCI_COMMAND)
			val |= (PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

		/* Avoid changing the main bridge BARs */
		if (where == PCI_MEMORY_BASE)
			return 0;
	}
#endif
	debug_cfg("PCIE CFG write: (b,d,f)=(%2ld,%2ld,%2ld) (addr,val)=(0x%04x, 0x%08x)\n",
		  PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), where, val);

	writel(PCIE_CONF_ADDR(bdf, where), hose->cfg_addr);
	writel(val, hose->cfg_data);

	return 0;
}

static void mvebu_pcie_set_local_bus_nr(void __iomem *reg_base, int nr)
{
	u32 stat;

	stat = readl(PCIE_STAT_OFF(reg_base));
	stat &= ~PCIE_STAT_BUS_MASK;
	stat |= nr << PCIE_STAT_BUS_OFF;
	writel(stat, PCIE_STAT_OFF(reg_base));
}

static void mvebu_pcie_set_local_dev_nr(void __iomem *reg_base, int nr)
{
	u32 stat;

	stat = readl(PCIE_STAT_OFF(reg_base));
	stat &= ~PCIE_STAT_DEV_MASK;
	stat |= nr << PCIE_STAT_DEV_OFF;
	writel(stat, PCIE_STAT_OFF(reg_base));
}

/*
 * Setup PCIE BARs and Address Decode Wins:
 * BAR[0,2] -> disabled, BAR[1] -> covers all DRAM banks
 * WIN[0-3] -> DRAM bank[0-3]
 */
static void mvebu_pcie_setup_mapping(void __iomem *reg_base)
{
	u64 size;
	u32 ctrl, attr;
	int i;

	/*
	 * Disable and clear BARs and windows except
	 * BAR0 which is fixed to internal registers
	 */
	for (i = 1; i < PCIE_BAR_CNT; i++) {
		writel(0, PCIE_BAR_CTRL_OFF(reg_base, i));
		writel(0, PCIE_BAR_LO_OFF(reg_base, i));
		writel(0, PCIE_BAR_HI_OFF(reg_base, i));
	}

	for (i = 0; i < PCIE_XBAR_WIN_CNT; i++) {
		writel(0, PCIE_WIN_CTRL_OFF(reg_base, i));
		writel(0, PCIE_WIN_BASE_OFF(reg_base, i));
		writel(0, PCIE_WIN_REMAP_OFF(reg_base, i));
	}

	/* Config BAR 0 - to internal regs */
	writel(PCIE_BAR_TYPE_MEM | MVEBU_REGS_BASE  , PCIE_BAR_LO_OFF(reg_base, 0));
	writel(0, PCIE_BAR_HI_OFF(reg_base, 0));

	/* Config BAR 1 - to 4G */
	size = (SZ_4G - 1) & 0xFFFF0000;
	writel(PCIE_BAR_TYPE_MEM, PCIE_BAR_LO_OFF(reg_base, 1));
	writel(0, PCIE_BAR_HI_OFF(reg_base, 1));
	writel(size | PCIE_BAR_ENABLE, PCIE_BAR_CTRL_OFF(reg_base, 1));

	/* Config XWindow 0 */
	writel(0, PCIE_WIN_BASE_OFF(reg_base, 0));
	attr = ~(1 << 0) & 0xF; /* Zero bit indicates the CS */
	ctrl = size | (attr << 8) | (PCIE_DDR_TARGET_ID << 4) | 1;
	writel(ctrl, PCIE_WIN_CTRL_OFF(reg_base, 0));
	writel(0, PCIE_WIN_REMAP_OFF(reg_base, 0));

	/* Config BAR 2 - to 4G */
	writel(PCIE_BAR_TYPE_MEM, PCIE_BAR_LO_OFF(reg_base, 2));
	writel(0x1, PCIE_BAR_HI_OFF(reg_base, 2));
	writel(size | PCIE_BAR_ENABLE, PCIE_BAR_CTRL_OFF(reg_base, 2));

	/* Config XWindow 1 */
	writel(0, PCIE_WIN_BASE_OFF(reg_base, 1));
	attr = ~(1 << 1) & 0xF; /* Zero bit indicates the CS */
	ctrl = size | (attr << 8) | (PCIE_DDR_TARGET_ID << 4) | (0x1 << 1) | 0x1;
	writel(ctrl, PCIE_WIN_CTRL_OFF(reg_base, 1));
	writel(0, PCIE_WIN_REMAP_OFF(reg_base, 1));
}

static void mvebu_pcie_hw_init(void __iomem *reg_base, int first_busno)
{
	u32 cmd;

#ifdef CONFIG_MVEBU_PCI_BURST_RELAX
	u32 reg;
	/* Set posted / non-posted credits to 0x0. */
	reg = readl(reg_base + PCIE_FLOW_CONTROL);
	reg &= ~(POSTED_CREDIT_MASK | NONPOSTED_CREDIT_MASK);
	reg |= (POSTED_CREDIT(1) | NONPOSTED_CREDIT(1));
	writel(reg, reg_base + PCIE_FLOW_CONTROL);

	/* Set outstanding requests to 1 */
	reg = readl(reg_base + PCIE_CONTROL);
	reg &= ~PCIE_OUTSTANDING_REQ_MASK;
	writel(reg, reg_base + PCIE_CONTROL);
#endif /* CONFIG_MVEBU_PCI_BURST_RELAX */

	/*
	 * Set our controller as device No 1 to avoid
	 * Answering CFG cycle by our host (memory controller)
	 */
	mvebu_pcie_set_local_dev_nr(reg_base, 1);
	mvebu_pcie_set_local_bus_nr(reg_base, first_busno);

	mvebu_pcie_setup_mapping(reg_base);

	/* Master + slave enable. */
	cmd = readw(PCIE_CMD_OFF(reg_base));
	cmd |= PCI_COMMAND_IO;
	cmd |= PCI_COMMAND_MEMORY;
	cmd |= PCI_COMMAND_MASTER;
	writew(cmd, PCIE_CMD_OFF(reg_base));
}

/*
 * We dont use a host bridge so don't let the
 * stack skip CFG cycle for dev = 0 func = 0
 */
int pci_skip_dev(struct pci_controller *hose, pci_dev_t dev)
{
#ifndef CONFIG_SPL_BUILD
#ifdef CONFIG_MVEBU_SPL_DDR_OVER_PCI_SWITCH
	/* skip the switch port connected to the DRAM
	 * PCIe controller to retain its configuration */
	if (dev == PCI_BDF(3, 0, 0)) {
		debug_cfg("Skipping (b,d,f)=(%2ld,%2ld,%2ld)\n", PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev));
		return 1;
	}
#elif defined(CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT)
	/* skip the DRAM PCIe controller to retain
	** its configuration */
	if (dev == PCI_BDF(1, 0, 0)) {
		debug_cfg("Skipping (b,d,f)=(%2ld,%2ld,%2ld)\n", PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev));
		return 1;
	}

#endif
#endif /* !SPL_BUILD */
	return 0;
}

static struct pci_controller	pci_hose[MAX_PCIE_PORTS];
static const char speed_str[3][8] = {"NA", "2.5GHz", "5GHz"};
static const char width_str[5][8] = {"NA", "x1", "NA", "NA", "x4"};

static int mvebu_pcie_init(int host_id, void __iomem *reg_base, struct pcie_win *win, int first_busno)
{
	struct pci_controller *hose = &pci_hose[host_id];
	u32 link, speed, width;

	memset(hose, 0, sizeof(struct pci_controller));

	/* Setup the HW */
	mvebu_pcie_hw_init(reg_base, first_busno);

	/* Set PCI regions */
	pci_set_region(&hose->regions[0], win->base, win->base, win->size, PCI_REGION_MEM);
	hose->region_count = 1;

	pci_set_ops(hose,
		    pci_hose_read_config_byte_via_dword,
		    pci_hose_read_config_word_via_dword,
		    mvebu_pcie_read_config,
		    pci_hose_write_config_byte_via_dword,
		    pci_hose_write_config_word_via_dword,
		    mvebu_pcie_write_config);

	/* CFG cycle pointers */
	hose->cfg_addr = (uint *)PCIE_CONF_ADDR_OFF(reg_base);
	hose->cfg_data = (unsigned char *)PCIE_CONF_DATA_OFF(reg_base);

	hose->first_busno = first_busno;
	hose->current_busno = first_busno;

	/* Register the host */
	pci_register_hose(hose);

	/* Scan the bus */
	hose->last_busno = pci_hose_scan(hose);

	/* Check the link type - for info only */
	link = readl(PCIE_LINK_CTL_OFF(reg_base));
	speed = (link >> 16) & 0xF;
	width = (link >> 20) & 0x3F;

	printf("PCIE-%d: Link up %s @ %s (Bus %d)\n", host_id, width_str[width], speed_str[speed], first_busno);

	return hose->last_busno + 1;
}

static int mvebu_pcie_check_link(void __iomem *reg_base)
{
	return readl(PCIE_STAT_OFF(reg_base)) &  PCIE_STAT_LINK;
}

static void mvebu_pcie_set_endpoint(u32 hid, void __iomem *reg_base)
{
	u32 capability;

	/* Check the LTSSM state machine if something is connected */
	capability = readl(PCIE_CAP_OFF(reg_base));
	capability &= ~PCIE_CAP_DEV_TYPE_MASK;
	capability |= PCIE_CAP_DEV_TYPE_EP;
	writel(capability, PCIE_CAP_OFF(reg_base));

	/* Open DRAM access for master */
	mvebu_pcie_setup_mapping(reg_base);

	printf("PCIE-%d: End point mode\n", hid);
}

void pci_init_board(void)
{
	int host_id = -1;
	int first_busno = FIRST_PCI_BUS;
	int bus_node, port_node, count;
	const void *blob = gd->fdt_blob;
	struct pcie_win win;
	void __iomem *reg_base;
	int err;

	count = fdtdec_find_aliases_for_id(blob, "pcie-controller",
			COMPAT_MVEBU_PCIE, &bus_node, 1);

	if (count <= 0)
		return;


	fdt_for_each_subnode(blob, port_node, bus_node) {
		host_id++;

		if (!fdtdec_get_is_enabled(blob, port_node))
			continue;

		reg_base = fdt_get_regs_offs(blob, port_node, "reg");
		if (reg_base == 0) {
			error("Missing registers in PCIe node\n");
			continue;
		}

		if (fdtdec_get_bool(blob, port_node, "endpoint")) {
			mvebu_pcie_set_endpoint(host_id, reg_base);
			continue;
		}

		/* Don't register host if link is down */
		if (mvebu_pcie_check_link(reg_base))
			continue;

		err = fdtdec_get_int_array(blob, port_node, "mem", (u32 *)&win, 2);
		if (err) {
			error("pcie: missing pci memory space in fdt\n");
			continue;
		}

		/* If all is well register the host */
		first_busno = mvebu_pcie_init(host_id, reg_base, &win, first_busno);
	}
}

#ifdef CONFIG_MVEBU_PCI_BURST_RELAX
#define PCIE_CAP_DCR			0x8
#define DCR_MAX_READ_REQ_SIZE_MASK	(7 << 12)
void board_pci_fixup_dev(struct pci_controller *hose, pci_dev_t dev,
			 unsigned short vendor, unsigned short device,
			 unsigned short class)
{
	int pcie_cap_pos, pci_dcr;
	u32 temp32;

	/* Get PCIe capability structure. */
	pcie_cap_pos = pci_hose_find_capability(hose, dev, PCI_CAP_ID_EXP);
	if (pcie_cap_pos == 0) {
		error("Could not find PCIE CAP structure.\n");
		return;
	}

	/* Get the PCIe Device Control Register */
	pci_dcr = pcie_cap_pos + PCIE_CAP_DCR;
	pci_hose_read_config_dword(hose, dev, pci_dcr, &temp32);
	/* Set Max-Read-Request-Size field to 0x0 -> 128B */
	temp32 &= ~DCR_MAX_READ_REQ_SIZE_MASK;
	pci_hose_write_config_dword(hose, dev, pci_dcr, temp32);
	pci_hose_read_config_dword(hose, dev, pci_dcr, &temp32);
	printf("Set PCIe device capability (DCR/Read-Req-Size to 128B - 0x%x).\n", temp32);
}
#endif /* CONFIG_MVEBU_PCI_BURST_RELAX */

