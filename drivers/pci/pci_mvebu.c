/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <common.h>
#include <pci.h>
#include <asm/io.h>
#include <asm/arch/regs-base.h>
#include <asm/arch/memory-map.h>
#include <errno.h>

/* #define DEBUG */

/*
 * PCIe unit register offsets.
 */

/* Standard PCI-E header reigsters */
#define PCIE_DEV_ID_OFF(x)		(MVEBU_PCIE_BASE(x) + 0x0)
#define PCIE_CMD_OFF(x)			(MVEBU_PCIE_BASE(x) + 0x4)
#define PCIE_DEV_REV_OFF(x)		(MVEBU_PCIE_BASE(x) + 0x8)
#define PCIE_BAR_LO_OFF(x, n)		(MVEBU_PCIE_BASE(x) + (0x10 + (0x8 * n)))
#define PCIE_BAR_HI_OFF(x, n)		(MVEBU_PCIE_BASE(x) + (0x14 + (0x8 * n)))
#define PCIE_BAR_CTRL_OFF(x, n)		(MVEBU_PCIE_BASE(x) + (0x1800 + (0x4 * n)))
#define PCIE_BAR_TYPE_MEM		(0xC)
#define PCIE_BAR_ENABLE			(0x1)
#define PCIE_BAR_CNT			(3)

/* Memory access control */
#define PCIE_WIN_OFF(n)			((n < 5) ? (0x0) : (0x10))
#define PCIE_WIN_CTRL_OFF(x, n)		(MVEBU_PCIE_BASE(x) + 0x1820 + PCIE_WIN_OFF(n) + (0x10 * n))
#define PCIE_WIN_BASE_OFF(x, n)		(MVEBU_PCIE_BASE(x) + 0x1824 + PCIE_WIN_OFF(n) + (0x10 * n))
#define PCIE_WIN_REMAP_OFF(x, n)	(MVEBU_PCIE_BASE(x) + 0x182C + PCIE_WIN_OFF(n) + (0x10 * n))
#define PCIE_DDR_TARGET_ID		(0)
#define PCIE_XBAR_WIN_CNT		(6)

/* Configuration access */
#define PCIE_CONF_ADDR_OFF(x)		(MVEBU_PCIE_BASE(x) + 0x18f8)
#define PCIE_CONF_DATA_OFF(x)		(MVEBU_PCIE_BASE(x) + 0x18fc)
#define PCIE_CONF_ADDR_EN		0x80000000
#define PCIE_CONF_REG(r)		((((r) & 0xf00) << 16) | ((r) & 0xfc))
#define PCIE_CONF_BUS(b)		(((b) & 0xff) << 16)
#define PCIE_CONF_DEV(d)		(((d) & 0x1f) << 11)
#define PCIE_CONF_FUNC(f)		(((f) & 0x7) << 8)
#define PCIE_CONF_ADDR(bdf, where)	(bdf | PCIE_CONF_REG(where) | PCIE_CONF_ADDR_EN)

/* Controler status */
#define PCIE_STAT_OFF(x)	(MVEBU_PCIE_BASE(x) + 0x1a04)
#define PCIE_STAT_BUS_OFF	(8)
#define PCIE_STAT_BUS_MASK	(0xFF << PCIE_STAT_BUS_OFF)
#define PCIE_STAT_DEV_OFF	(16)
#define PCIE_STAT_DEV_MASK	(0x1F << PCIE_STAT_DEV_OFF)
#define PCIE_STAT_LINK		(1)

/* Link status */
#define PCIE_DBG_STATUS_OFF(x)	(MVEBU_PCIE_BASE(x) + 0x1a64)
#define PCIE_LTSSM_LINK_UP	0x7E
#define PCIE_LTSSM_MASK		0x7F
#define PCIE_LINK_CTL_OFF(x)	(MVEBU_PCIE_BASE(x) + 0x70)

DECLARE_GLOBAL_DATA_PTR;

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
		debug("CFG address out of range (%d,%d,%d)\n",
		      PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));
		*val = 0xFFFFFFFF;
		return 1;
	}
	writel(PCIE_CONF_ADDR(bdf, where), hose->cfg_addr);

	*val = readl(hose->cfg_data);

	debug("PCIE CFG read:  (b,d,f)=(%2d,%2d,%2d) (addr,val)=(0x%04x, 0x%08x)\n",
	      PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), where, (*val));

	return 0;
}

static int mvebu_pcie_write_config(struct pci_controller *hose, pci_dev_t bdf,
				   int where, u32 val)
{
	if (!mvebu_pcie_addr_valid(bdf, hose->first_busno)) {
		debug("CFG address out of range (%d,%d,%d)\n",
		      PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));
		return 1;
	}

	writel(PCIE_CONF_ADDR(bdf, where), hose->cfg_addr);
	writel(val, hose->cfg_data);

	debug("PCIE CFG write: (b,d,f)=(%2d,%2d,%2d) (addr,val)=(0x%04x, 0x%08x)\n",
	      PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), where, val);

	return 0;
}

static void mvebu_pcie_set_local_bus_nr(int hid, int nr)
{
	u32 stat;

	stat = readl(PCIE_STAT_OFF(hid));
	stat &= ~PCIE_STAT_BUS_MASK;
	stat |= nr << PCIE_STAT_BUS_OFF;
	writel(stat, PCIE_STAT_OFF(hid));
}

static void mvebu_pcie_set_local_dev_nr(int hid, int nr)
{
	u32 stat;

	stat = readl(PCIE_STAT_OFF(hid));
	stat &= ~PCIE_STAT_DEV_MASK;
	stat |= nr << PCIE_STAT_DEV_OFF;
	writel(stat, PCIE_STAT_OFF(hid));
}

/*
 * Setup PCIE BARs and Address Decode Wins:
 * BAR[0,2] -> disabled, BAR[1] -> covers all DRAM banks
 * WIN[0-3] -> DRAM bank[0-3]
 */
static void mvebu_pcie_setup_mapping(int hid)
{
	u32 size;
	int i;

	/*
	 * First, disable and clear BARs and windows except
	 * BAR0 which is fixed to internal registers
	 */
	for (i = 1; i < PCIE_BAR_CNT; i++) {
		writel(0, PCIE_BAR_CTRL_OFF(hid, i));
		writel(0, PCIE_BAR_LO_OFF(hid, i));
		writel(0, PCIE_BAR_HI_OFF(hid, i));
	}

	for (i = 0; i < PCIE_XBAR_WIN_CNT; i++) {
		writel(0, PCIE_WIN_CTRL_OFF(hid, i));
		writel(0, PCIE_WIN_BASE_OFF(hid, i));
		writel(0, PCIE_WIN_REMAP_OFF(hid, i));
	}

	/* Setup XBAR windows for DDR banks. */
	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		u32 size, base, attr, ctrl;

		if (gd->bd->bi_dram[i].size == 0)
			continue;

		base = gd->bd->bi_dram[i].start & 0xFFFF0000;
		size = (gd->bd->bi_dram[i].size - 1) & 0xFFFF0000;
		attr = ~(1 << i) & 0xF; /* Zero bit indicates the CS */
		ctrl = size | (attr << 8) | (PCIE_DDR_TARGET_ID << 4) | 1;

		writel(base, PCIE_WIN_BASE_OFF(hid, i));
		writel(ctrl, PCIE_WIN_CTRL_OFF(hid, i));
		writel(0, PCIE_WIN_REMAP_OFF(hid, i));
		debug("PCIE WIN-%d base = 0x%08x ctrl = 0x%08x\n", i, base, ctrl);
	}

	/* Round up 'size' to the nearest power of two. */
	size = gd->ram_size;
	if ((size & (size - 1)) != 0)
		size = 1 << fls(size);

	/* Setup BAR[1] to all DRAM banks. */
	writel(gd->bd->bi_dram[0].start | PCIE_BAR_TYPE_MEM, PCIE_BAR_LO_OFF(hid, 1));
	writel(0, PCIE_BAR_HI_OFF(hid, 1));
	writel(((size - 1) & 0xffff0000) | PCIE_BAR_ENABLE, PCIE_BAR_CTRL_OFF(hid, 1));
}

static void mvebu_pcie_hw_init(int host_id, int first_busno)
{
	u32 cmd;

	/*
	 * Set our controller as device No 1 to avoid
	 * Answering CFG cycle by our host (memory controller)
	 */
	mvebu_pcie_set_local_dev_nr(host_id, 1);
	mvebu_pcie_set_local_bus_nr(host_id, first_busno);

	mvebu_pcie_setup_mapping(host_id);

	/* Master + slave enable. */
	cmd = readw(PCIE_CMD_OFF(host_id));
	cmd |= PCI_COMMAND_IO;
	cmd |= PCI_COMMAND_MEMORY;
	cmd |= PCI_COMMAND_MASTER;
	writew(cmd, PCIE_CMD_OFF(host_id));
}

/* TODO - consider removing this when testing IDE */
#if CONFIG_CMD_IDE
static void mvebu_setup_ide(struct pci_controller *hose,
			 pci_dev_t dev, struct pci_config_table *entry)
{
	static const int ide_bar[] = { 8, 4, 8, 4, 16, 1024 };
	u32 bar_response, bar_value;
	int bar;

	for (bar = 0; bar < 6; bar++) {
		unsigned int offset = (bar < 2) ? bar * 8 : 0x100 + (bar - 2) * 8;

		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0 + offset, 0x0);
		pci_read_config_dword(dev, PCI_BASE_ADDRESS_0 + offset, &bar_response);

		pciauto_region_allocate(bar_response & PCI_BASE_ADDRESS_SPACE_IO ?
					hose->pci_io : hose->pci_mem, ide_bar[bar], &bar_value);

		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0 + bar * 4, bar_value);
	}
}

static void mvebu_setup_host(struct pci_controller *hose,
			  pci_dev_t dev, struct pci_config_table *entry)
{
	/* No need to configure host */
	return;
}


struct pci_config_table mvebu_config_table[] = {
	/* Storage controllers */
	{ PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_STORAGE_IDE,
	  PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, mvebu_setup_ide },

	/* Storage controllers */
	{ PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, mvebu_setup_host },
	{}
};
#endif

/*
 * We dont use a host bridge so don't let the
 * stack skip CFG cycle for dev = 0 func = 0
 */
int pci_skip_dev(struct pci_controller *hose, pci_dev_t dev)
{
	return 0;
}

static struct pci_controller	pci_hose[4];
static const char speed_str[3][8] = {"NA", "2.5GHz", "5GHz"};
static const char width_str[5][8] = {"NA", "x1", "NA", "NA", "x4"};

static int mvebu_pcie_init(int host_id, int first_busno)
{
	struct pci_controller *hose = &pci_hose[host_id];
	u32 link, speed, width;

	memset(hose, 0, sizeof(hose));

	/* Setup the HW */
	mvebu_pcie_hw_init(host_id, first_busno);

	/* Set PCI regions */
	pci_set_region(&hose->regions[0],
		       MVEBU_PCIE_MEM_BASE(host_id), MVEBU_PCIE_MEM_BASE(host_id),
		       MVEBU_PCIE_MEM_SIZE(host_id), PCI_REGION_MEM);

	hose->region_count = 1;

	pci_set_ops(hose,
		    pci_hose_read_config_byte_via_dword,
		    pci_hose_read_config_word_via_dword,
		    mvebu_pcie_read_config,
		    pci_hose_write_config_byte_via_dword,
		    pci_hose_write_config_word_via_dword,
		    mvebu_pcie_write_config);

	/* CFG cycle pointers */
	hose->cfg_addr = (uint *)PCIE_CONF_ADDR_OFF(host_id);
	hose->cfg_data = (unsigned char *)PCIE_CONF_DATA_OFF(host_id);

	/* TODO - we might need this for IDE */
	/*hose->config_table = mvebu_config_table;*/

	hose->first_busno = first_busno;

	/* Register the host */
	pci_register_hose(hose);

	/* Scan the bus */
	hose->last_busno = pci_hose_scan(hose);

	/* Check the link type - for info only */
	link = readl(PCIE_LINK_CTL_OFF(host_id));
	speed = (link >> 16) & 0xF;
	width = (link >> 20) & 0x3F;

	printf("PCIE-%d: Link up %s @ %s (Bus %d)\n", host_id, width_str[width], speed_str[speed], first_busno);

	return hose->last_busno + 1;
}

static int mvebu_pcie_check_link(int hid)
{
	return readl(PCIE_STAT_OFF(hid)) &  PCIE_STAT_LINK;
}

void mvebu_pcie_init_board(int max_hosts, u16 active_mask)
{
	int host_id;
	int first_busno = 0;

	/* Loop over all active PCI ports */
	for (host_id = 0; host_id < max_hosts; host_id++) {
		/* Check if unit is enabled */
		if ((active_mask & (1 << host_id)) == 0)
			continue;

		/* Don't register host if link is down */
		if (mvebu_pcie_check_link(host_id))
			continue;

		first_busno = mvebu_pcie_init(host_id, first_busno);
	}
}
