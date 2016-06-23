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
/* pic_advk.c
 *
 * Implementation for Aardvark PCIe IP
 *
 * Author: Victor Gu <xigu@marvell.com>
 *
 * March 6, 2015
 *
 */
#include <common.h>
#include <fdtdec.h>
#include <asm/io.h>
#include <asm/arch/pcie-core.h>
#include <pci.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/gpio.h>

/* #define DEBUG */
/* #define DEBUG_CFG_CYCLE */
/* During PCIe scan, PCIe drive will check the target BDF(bus, device, function) for many times,
 * here use a local debug flag to control the print in PIO read and write routines, to make sure
 * there will not be too much prints when global DEBUG is enabled.
 * By default this flag(DEBUG_CFG_CYCLE) is disabled.
 */
#ifdef DEBUG_CFG_CYCLE
#define debug_cfg(fmt, args...) printf(fmt, ##args)
#else
#define debug_cfg(fmt, args...)
#endif

DECLARE_GLOBAL_DATA_PTR;

struct pcie_win {
	u32 base;
	u32 size;
};

static struct pci_controller	pci_hose[PCIE_ADVK_MAX_PORT];
static const char speed_str[4][8] = {"NA", "2.5GHz", "5GHz", "8GHz"};
static const char width_str[9][8] = {"NA", "x1", "x2", "NA", "x4", "NA", "NA", "NA", "x8"};
static const char mode_str[2][16] = {"Endpoint", "Root Complex"};

/*
 * We dont use a host bridge so don't let the
 * stack skip CFG cycle for dev = 0 func = 0
 */
int pci_skip_dev(struct pci_controller *hose, pci_dev_t dev)
{
	return 0;
}

/*
 * Check PIO status
 */
int advk_pcie_check_pio_status(void __iomem *reg_base)
{
	unsigned int pio_status;
	unsigned char comp_status;
	char *strcomp_status;

	pio_status = readl(PCIE_CORE_PIO_REG_ADDR(reg_base, PCIE_CORE_PIO_STAT));
	comp_status = (pio_status >> 7) & 0x7;

	switch (comp_status) {
	case 0:
		break;
	case 1:
		strcomp_status = "UR";
		break;
	case 2:
		strcomp_status = "CRS";
		break;
	case 4:
		strcomp_status = "CA";
		break;
	default:
		strcomp_status = "Unkown";
		break;
	}
	if (0 != comp_status) {
		if (pio_status & (0x1 << 10))
			printf("Non-posted PIO Response Status: %s, %#x @ %#x\n",
			       strcomp_status, pio_status,
			       readl(PCIE_CORE_PIO_REG_ADDR(reg_base, PCIE_CORE_PIO_ADDR_LS)));
		else
			printf("Posted PIO Response Status: %s, %#x @ %#x\n",
			       strcomp_status, pio_status,
			       readl(PCIE_CORE_PIO_REG_ADDR(reg_base, PCIE_CORE_PIO_ADDR_LS)));
	}
	return 0;
}

static int advk_pcie_addr_valid(pci_dev_t bdf, int first_busno)
{
	/*
	 * In PCIE-E only a single device (0) can exist
	 * on the local bus. Beyound the local bus, there might be
	 * a Switch and everyting is possible.
	 */
	if ((PCI_BUS(bdf) == first_busno) && (PCI_DEV(bdf) > 0))
		return 0;

	return 1;
}

/*
 * Set PCIe address window register which could be used for memory mapping.
 * These address window registers are within PCIe IP internally.
 * It should be called and set correctly if want to access external PCIe device
 * by accessing CPU memory space directly.
 */
int advk_pcie_set_ob_win(void __iomem *reg_base,
			u32 win_num,
			u32 match_ms,
			u32 match_ls,
			u32 mask_ms,
			u32 mask_ls,
			u32 remap_ms,
			u32 remap_ls,
			u32 action)
{
	writel(match_ms, PCIE_CORE_OB_REG_ADDR(reg_base, OB_WIN_MATCH_MS, win_num));
	writel(mask_ms, PCIE_CORE_OB_REG_ADDR(reg_base, OB_WIN_MASK_MS, win_num));
	writel(mask_ls, PCIE_CORE_OB_REG_ADDR(reg_base, OB_WIN_MASK_LS, win_num));
	writel(remap_ms, PCIE_CORE_OB_REG_ADDR(reg_base, OB_WIN_REMAP_MS, win_num));
	writel(remap_ls, PCIE_CORE_OB_REG_ADDR(reg_base, OB_WIN_REMAP_LS, win_num));
	writel(action, PCIE_CORE_OB_REG_ADDR(reg_base, OB_WIN_ACTIONS, win_num));
	writel(match_ls, PCIE_CORE_OB_REG_ADDR(reg_base, OB_WIN_MATCH_LS, win_num));
	writel(match_ls | 0x1, PCIE_CORE_OB_REG_ADDR(reg_base, OB_WIN_MATCH_LS, win_num));

	return 0;
}

/*
 * This routine is used to enable or dieable AXI address window location generation.
 * Disabled: No address window mapping. Use AXI user fields
 * provided by the AXI fabric.
 * Enabled: Enable the address window mapping. The HAL bridge
 * generates the AXI user field locally. Use the local generated AXI user fields.
 * It should be disabled when access PCIe device by PIO.
 * It should be enabled when access PCIe device by memory access directly.
 */
int advk_pcie_enable_axi_addr_gen(void __iomem *reg_base, int enable)
{
	u32 reg32;

	reg32 = readl(PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL2_REG));
	if (enable)
		reg32 |= OB_WIN_ENABLE;
	else
		reg32 &= ~(OB_WIN_ENABLE);
	writel(reg32, PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL2_REG));
	return 0;
}

/*
 * Configuration read
 */
int advk_pcie_pio_read_config(struct pci_controller *hose, pci_dev_t bdf, int where, u32 *val)
{
	int i;
	u32 reg32;
	u32 is_done;
	int ret = 0;

	if (!advk_pcie_addr_valid(bdf, hose->first_busno)) {
		debug_cfg("CFG read: address out of range (%ld,%ld,%ld)\n",
			  PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));
		*val = 0xFFFFFFFF;
		return 1;
	}

	/* Disabled AXI address window location generation.
	 * Use AXI user fields provided by the AXI fabric.
	 */
	advk_pcie_enable_axi_addr_gen((void __iomem *)hose->cfg_addr, 0);

	/* Start PIO */
	writel(0x0, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_START));
	writel(0x1, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_ISR));

	/* Program the control register */
	if (PCI_BUS(bdf) > hose->first_busno + PCIE_ADVK_ACTUAL_FIRST_BUSNO)
		reg32 = (0x4 << PCIE_CORE_PIO_CTRL_BYTE_COUNT_SHIFT) | PCIE_CONFIG_RD_TYPE1;
	else
		reg32 = (0x4 << PCIE_CORE_PIO_CTRL_BYTE_COUNT_SHIFT) | PCIE_CONFIG_RD_TYPE0;
	writel(reg32, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_CTRL));

	/* Program the address registers */
	reg32 = PCIE_BDF(bdf)|(PCIE_REG_ADDR(where));
	writel(reg32, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_ADDR_LS));
	writel(0, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_ADDR_MS));

	/* Program the data strobe */
	writel(0xf, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_WR_DATA_STRB));

	/* Start the transfer */
	writel(1, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_START));

	for (i = 0; i < PCIE_CORE_PIO_TIMEOUT_NUM; i++) {
		reg32 = readl(PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_START));
		is_done = readl(PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_ISR));
		if ((!reg32) && is_done)
			break;
		/* do not check the PIO state too frequently, 100us delay is appropriate */
		udelay(100);
	}

	if (i == PCIE_CORE_PIO_TIMEOUT_NUM) {
		printf("%s(%d): wait for PIO time out\n", __func__, __LINE__);
		/* Enable AXI address window location generation.
		 * The HAL bridge generates the AXI user field locally.
		 */
		advk_pcie_enable_axi_addr_gen((void __iomem *)hose->cfg_addr, 1);
		return 1;
	}

	advk_pcie_check_pio_status((void __iomem *)hose->cfg_addr);

	*val = readl(PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_RD_DATA));

	/* Enable AXI address window location generation.
	 * The HAL bridge generates the AXI user field locally.
	 */
	advk_pcie_enable_axi_addr_gen((void __iomem *)hose->cfg_addr, 1);

	return ret;
}

/*
 * Configuration write
 */
int advk_pcie_pio_write_config(struct pci_controller *hose, pci_dev_t bdf, int where, u32 val)
{
	int i;
	u32 reg32;
	int ret = 0;

	if (!advk_pcie_addr_valid(bdf, hose->first_busno)) {
		debug_cfg("CFG write: address out of range (%ld,%ld,%ld)\n",
			  PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));
		return 1;
	}

	/* Disabled AXI address window location generation.
	 * Use AXI user fields provided by the AXI fabric.
	 */
	advk_pcie_enable_axi_addr_gen((void __iomem *)hose->cfg_addr, 0);

	/* Start PIO */
	writel(0x0, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_START));
	writel(0x1, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_ISR));

	/* Program the control register */
	if (PCI_BUS(bdf) > hose->first_busno + PCIE_ADVK_ACTUAL_FIRST_BUSNO)
		reg32 = (0x4 << PCIE_CORE_PIO_CTRL_BYTE_COUNT_SHIFT) | PCIE_CONFIG_WR_TYPE1;
	else
		reg32 = (0x4 << PCIE_CORE_PIO_CTRL_BYTE_COUNT_SHIFT) | PCIE_CONFIG_WR_TYPE0;
	writel(reg32, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_CTRL));

	/* Program the address registers */
	reg32 = PCIE_BDF(bdf) | PCIE_REG_ADDR(where);  /* Ignore the last 2 bits of where */
	writel(reg32, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_ADDR_LS));
	writel(0, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_ADDR_MS));

	/* Program the data register */
	writel(val, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_WR_DATA));

	/* Program the data strobe */
	writel(0xf, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_WR_DATA_STRB));

	/* Start the transfer */
	writel(1, PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_START));

	for (i = 0; i < PCIE_CORE_PIO_TIMEOUT_NUM; i++) {
		reg32 = readl(PCIE_CORE_PIO_REG_ADDR(hose->cfg_addr, PCIE_CORE_PIO_START));
		if (!reg32)
			break;
		/* do not check the PIO state too frequently, 100us delay is appropriate */
		udelay(100);
	}
	if (i == PCIE_CORE_PIO_TIMEOUT_NUM) {
		printf("%s(%d): wait for PIO time out\n", __func__, __LINE__);
		/* Enable AXI address window location generation.
		 * The HAL bridge generates the AXI user field locally.
		 */
		advk_pcie_enable_axi_addr_gen((void __iomem *)hose->cfg_addr, 1);
		return 1;
	}

	advk_pcie_check_pio_status((void __iomem *)hose->cfg_addr);

	/* Enable AXI address window location generation.
	 * The HAL bridge generates the AXI user field locally.
	 */
	advk_pcie_enable_axi_addr_gen((void __iomem *)hose->cfg_addr, 1);

	return ret;
}

static int advk_pcie_link_init(void __iomem *reg_base)
{
	int i;
	u32 state;

	/* Set Advanced Error Capabilities and Control PF0 register
	 * ECRC_CHCK_RCV (RD0070118h [8]) = 1h
	 * ECRC_CHCK (RD0070118h [7]) = 1h
	 * ECRC_GEN_TX_EN (RD0070118h [6]) = 1h
	 * ECRC_CHK_TX (RD0070118h [5]) = 1h
	 */
	writel(0x01E0, PCIE_CORE_CONFIG_REG_ADDR(reg_base, PCIE_CORE_ERR_CAP_CTRL_REG));

	/* Set PCIe Device Control and Status 1 PF0 register
	 * MAX_RD_REQ_SIZE (RD00700C8h [14:12])/MAX_RD_REQ_SZ (RD00700C8h [14:12]) = 2h (default)
	 * Clear EN_NO_SNOOP (RD00700C8h [11])/EN_NO_SNOOP (RD00700C8h [11]) = 0h (default is 1h)
	 * MAX_PAYLOAD_SIZEW (RD00700C8h [7:5])/MAX_PAYLOAD (RD00700C8h [7:5]) = 2
	 * EN_RELAXED_ORDERING (RD00700C8h [4])/EN_RELAXED_ORDERING (RD00700C8h [4])= 0h (default is 1h)
	 */
	writel(0x2040, PCIE_CORE_CONFIG_REG_ADDR(reg_base, PCIE_CORE_DEV_CTRL_STATS_REG));

	/* Program PCIe Control 2 (RD0074808h) to 0000001Fh to disable strict ordering by clearing
	 * STRICT_ORDERING_EN (RD0074808h [5]) = 0h (default is 1h).
	 */
	writel(0x001F, PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL2_REG));

	/* Set GEN2 */
	state = readl(PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL0_REG));
	state &= ~PCIE_GEN_SEL_MSK;
	state |= SPEED_GEN_2;
	writel(state, PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL0_REG));

	/* Set lane X1 */
	state = readl(PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL0_REG));
	state &= ~LANE_CNT_MSK;
	state |= LANE_COUNT_1;
	writel(state, PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL0_REG));

	/* Enable link training */
	state = readl(PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL0_REG));
	state |= LINK_TRAINNING_EN;
	writel(state, PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL0_REG));

	/* Disable strict ordering */
	state = readl(PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL2_REG));
	state &= ~STRICT_ORDER_ENABLE;
	writel(state, PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL2_REG));

	/* Start link training */
	state = readl(PCIE_CORE_CONFIG_REG_ADDR(reg_base, PCIE_CORE_LINK_CTRL_STAT_REG));
	state |= (1 << PCIE_CORE_LINK_TRAINING_SHIFT);
	writel(state, PCIE_CORE_CONFIG_REG_ADDR(reg_base, PCIE_CORE_LINK_CTRL_STAT_REG));

	/* Poll the link state */
	for (i = 0; i < PCIE_LINK_TIMEOUT_NUM; i++) {
		state = readl(PCIE_CORE_LMI_REG_ADDR(reg_base, PHY_CONF_REG0));
		if (((state & LTSSM_STATE_MASK) >> LTSSM_STATE_SHIFT) == LTSSM_STATE_L0)
			break;
		udelay(100);
	}
	if (i == PCIE_LINK_TIMEOUT_NUM) {
		debug("%s(%d): time out to get PCIe link\n", __func__, __LINE__);
		return 1;
	}

	/* Set PCIe Control 2 register
	 * bit[1:0] ASPM Control, set to 0 to disable L0S entry
	 */
	state = readl(PCIE_CORE_CONFIG_REG_ADDR(reg_base, PCIE_CORE_LINK_CTRL_STAT_REG));
	state &= ~0x3;
	writel(state, PCIE_CORE_CONFIG_REG_ADDR(reg_base, PCIE_CORE_LINK_CTRL_STAT_REG));

	return 0;
}

/*
 * reset PCIe device
 */
static void advk_pcie_reset_dev(struct fdt_gpio_state *gpio)
{
#ifdef CONFIG_MVEBU_GPIO
	int val;

	if (!fdt_gpio_isvalid(gpio))
		return;

	/* Set PCIe reset gpio in output mode with low level */
	val = gpio->flags & FDT_GPIO_ACTIVE_LOW ? 1 : 0;
	gpio_direction_output(gpio->gpio, val);

	/* typical delay for NIC to finish reset from NIC specification */
	udelay(100);

	/* Set PCIe reset gpio in high level */
	val = gpio->flags & FDT_GPIO_ACTIVE_LOW ? 0 : 1;
	gpio_set_value(gpio->gpio, val);
#else
	printf("ERROR: the PCIe device is not reset, need to implement gpio in SOC code\n");
#endif

	return;
}

static int advk_pcie_init(int host_id,
				void __iomem *reg_base,
				struct pcie_win *win,
				int first_busno,
				struct fdt_gpio_state *gpio)
{
	int ret = 0;
	u32 state;
	u32 speed;
	u32 width;
	u32 region_id = 0;

	struct pci_controller *hose = &pci_hose[host_id];

	debug_enter();

	memset(hose, 0, sizeof(hose));

	/* reset PCIe device in RC mode */
	advk_pcie_reset_dev(gpio);

	/* start link training */
	ret = advk_pcie_link_init(reg_base);
	if (ret) {
		printf("PCIE-%d: Link down\n", host_id);
		return hose->last_busno;
	}

	/* Enable BUS, IO, Memory space assess
	 * bit2: Memory IO Request
	 * bit1: Memory Access Enable
	 * bit0: IO Access Enable
	 */
	state = readl(PCIE_CORE_CONFIG_REG_ADDR(reg_base, 4));
	state |= 0x7;
	writel(state, PCIE_CORE_CONFIG_REG_ADDR(reg_base, 4));

	/* enable direct mode */
	state = readl(PCIE_CTRL_CORE_REG_ADDR(reg_base, PCIE_CTRL_CONFIG_REG));
	state &= ~(PCIE_CTRL_MODE_MASK << PCIE_CTRL_MODE_SHIFT);
	writel(state, PCIE_CTRL_CORE_REG_ADDR(reg_base, PCIE_CTRL_CONFIG_REG));

	/* Set config address */
	hose->cfg_addr = (unsigned int *)reg_base;

	/* Set PCI memory region */
	pci_set_region(&hose->regions[region_id++], win->base, win->base, win->size, PCI_REGION_MEM);
	hose->region_count = region_id;

	/* Register outbound window for configuration and set r/w config operations */
	advk_pcie_set_ob_win(reg_base,/* reg base */
			     0,/* window block  */
			     0,/* match ms */
			     win->base, /* match ls */
			     0xffffffff, /* mask ms */
			     0xf8000000, /* mask ls */
			     0,	       /* remap ms */
			     win->base, /* remap ls */
			    (2 << 20)|OB_PCIE_MEM);
	/* Enable outbound window by default */
	advk_pcie_enable_axi_addr_gen(reg_base, 1);

	pci_set_ops(hose,
		    pci_hose_read_config_byte_via_dword,
		    pci_hose_read_config_word_via_dword,
		    advk_pcie_pio_read_config,
		    pci_hose_write_config_byte_via_dword,
		    pci_hose_write_config_word_via_dword,
		    advk_pcie_pio_write_config);

	/* Assign config table to hose */
	hose->first_busno = first_busno;

	/* Register the hose */
	pci_register_hose(hose);

	/* Scan the bus */
	debug("ArLP PCIe Scan start\n");
	hose->last_busno = pci_hose_scan(hose);

	/* Check the link type - for info only */
	state = readl(PCIE_CORE_CONFIG_REG_ADDR(reg_base, PCIE_CORE_LINK_CTRL_STAT_REG));
	speed = (state >> PCIE_CORE_LINK_SPEED_SHIFT) & PCIE_CORE_LINK_SPEED_MASK;
	width = (state >> PCIE_CORE_LINK_WIDTH_SHIFT) & PCIE_CORE_LINK_WIDTH_MASK;

	printf("PCIE-%d: Link up (Gen%d-%s %s, Bus%d)\n",
	       host_id, speed, width_str[width], speed_str[speed], first_busno);

	debug_exit();

	return hose->last_busno + 1;
}

static void advk_pcie_set_core_mode(int host_id, void __iomem *reg_base, int mode)
{
	u32 config;

	/* Set PCI global control register to RC or EP mode */
	config = readl(PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL0_REG));
	config &= ~(PCIE_CTRL_MODE_MASK << IS_RC_SHIFT);
	config |= ((mode & PCIE_CTRL_MODE_MASK) << IS_RC_SHIFT);
	writel(config, PCIE_CORE_CTRL_REG_ADDR(reg_base, PCIE_CORE_CTRL0_REG));

	debug("PCIE-%d: core mode %s\n", host_id, mode_str[mode]);
}

void pci_init_board(void)
{
	int host_id = -1;
	int first_busno = 0;
	int bus_node, port_node, count;
	const void *blob = gd->fdt_blob;
	struct pcie_win win;
	void __iomem *reg_base;
	struct fdt_gpio_state reset_gpio;
	int err;

	count = fdtdec_find_aliases_for_id(blob, "pcie-controller",
			COMPAT_MVEBU_ADVK_PCIE, &bus_node, 1);

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
			/* Set to end point mode */
			advk_pcie_set_core_mode(host_id, reg_base, PCIE_CORE_MODE_EP);

			continue;
		} else {
			/* Set to root complex mode */
			advk_pcie_set_core_mode(host_id, reg_base, PCIE_CORE_MODE_RC);
		}

		err = fdtdec_get_int_array(blob, port_node, "mem", (u32 *)&win, 2);
		if (err) {
			error("pcie: missing pci memory space in fdt\n");
			continue;
		}

#ifdef CONFIG_MVEBU_GPIO
		fdtdec_decode_gpio(blob, port_node, "reset-gpio", &reset_gpio);
		fdtdec_setup_gpio(&reset_gpio);
#else
		printf("ERROR: reset gpio is initialized, need to implement gpio in SOC code\n");
#endif

		/* If all is well register the host */
		first_busno = advk_pcie_init(host_id, reg_base, &win, first_busno, &reset_gpio);
	}
}

#ifdef CONFIG_PCI_CHECK_EP_PAYLOAD
#define PCIE_CAP_DCR1			0x4
#define MAX_PAYLOAD_SIZE_MASK	0x7
#define PCIE_CORE_MAX_PAYLAOD_SIZE_SHIFT 5

/* Update the MAX_PAYLOAD_SIZE, use the minimal value between RC and EP */
static void advk_pcie_set_payload(struct pci_controller *hose, u32 payload)
{
	u32 reg, tmp;

	/* Get the MAX_PAYLOAD_SIZE of hose */
	reg = readl(PCIE_CORE_CONFIG_REG_ADDR(hose->cfg_addr, PCIE_CORE_DEV_CTRL_STATS_REG));
	tmp = (reg >> PCIE_CORE_MAX_PAYLAOD_SIZE_SHIFT) & MAX_PAYLOAD_SIZE_MASK;

	/* Update the setting to the minimal value */
	reg &= ~(MAX_PAYLOAD_SIZE_MASK << PCIE_CORE_MAX_PAYLAOD_SIZE_SHIFT);
	reg |= (min(tmp, payload) << PCIE_CORE_MAX_PAYLAOD_SIZE_SHIFT);
	writel(reg, PCIE_CORE_CONFIG_REG_ADDR(hose->cfg_addr, PCIE_CORE_DEV_CTRL_STATS_REG));
}

void board_pci_fixup_dev(struct pci_controller *hose, pci_dev_t dev,
			 unsigned short vendor, unsigned short device,
			 unsigned short class)
{
	int pcie_cap_pos, pci_dcr1;
	u32 tmp;

	/* Get PCIe capability structure. */
	pcie_cap_pos = pci_hose_find_capability(hose, dev, PCI_CAP_ID_EXP);
	if (pcie_cap_pos == 0) {
		error("Could not find PCIE CAP structure.\n");
		return;
	}

	/* Get supported MAX_PAYLOAD_SIZE from EP */
	pci_dcr1 = pcie_cap_pos + PCIE_CAP_DCR1;
	pci_hose_read_config_dword(hose, dev, pci_dcr1, &tmp);
	tmp &= MAX_PAYLOAD_SIZE_MASK;

	/* Set the MAX_PAYLOAD_SIZE of hose according to EP */
	advk_pcie_set_payload(hose, tmp);
}
#endif /* CONFIG_PCI_CHECK_EP_PAYLOAD */
