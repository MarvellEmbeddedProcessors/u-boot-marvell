/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <usb.h>
#include "ehci.h"
#ifndef CONFIG_USB_EHCI_MARVELL_BYPASS_BRG_ADDR_DEC
#include <asm/arch/cpu.h>
#endif
#ifdef CONFIG_OF_CONTROL
#include <fdtdec.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/errno.h>
#endif

#if defined(CONFIG_KIRKWOOD)
#include <asm/arch/soc.h>
#elif defined(CONFIG_ORION5X)
#include <asm/arch/orion5x.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_OF_CONTROL
/* without FDT support, MVUSB0_BASE is defined in header file,
 * and be used in routine ehci_hcd_init.
 *
 * with FDT support, to keep legacy SoCs' ehci working,
 * MVUSB0_BASE is redefined as usb_reg_base,
 * and will be initialized in routine ehci_hcd_init.
 */
static uintptr_t usb_reg_base;
#undef MVUSB0_BASE
#define MVUSB0_BASE usb_reg_base
#endif

#ifdef CONFIG_USB_EHCI_MARVELL_SBUSCFG
#define USB2_SBUSCFG_OFF	0x90

#define USB_SBUSCFG_BAWR_OFF	0x6
#define USB_SBUSCFG_BARD_OFF	0x3
#define USB_SBUSCFG_AHBBRST_OFF	0x0

#define USB_SBUSCFG_BAWR_ALIGN_64B	0x4
#define USB_SBUSCFG_BARD_ALIGN_64B	0x4
#define USB_SBUSCFG_AHBBRST_INCR16	0x7
#endif

#ifndef CONFIG_USB_EHCI_MARVELL_BYPASS_BRG_ADDR_DEC
#define rdl(off)	readl(MVUSB0_BASE + (off))
#define wrl(off, val)	writel((val), MVUSB0_BASE + (off))

#define USB_WINDOW_CTRL(i)	(0x320 + ((i) << 4))
#define USB_WINDOW_BASE(i)	(0x324 + ((i) << 4))
#define USB_TARGET_DRAM		0x0

/*
 * USB 2.0 Bridge Address Decoding registers setup
 * that only relevant to AXP, a37x, a38x, a39x and etc..
 */
static void usb_brg_adrdec_setup(void)
{
	int i;
	u32 size, base, attrib;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {

		/* Enable DRAM bank */
		switch (i) {
		case 0:
			attrib = MVUSB0_CPU_ATTR_DRAM_CS0;
			break;
		case 1:
			attrib = MVUSB0_CPU_ATTR_DRAM_CS1;
			break;
		case 2:
			attrib = MVUSB0_CPU_ATTR_DRAM_CS2;
			break;
		case 3:
			attrib = MVUSB0_CPU_ATTR_DRAM_CS3;
			break;
		default:
			/* invalide bank, disable access */
			attrib = 0;
			break;
		}

		size = gd->bd->bi_dram[i].size;
		base = gd->bd->bi_dram[i].start;
		if ((size) && (attrib))
			wrl(USB_WINDOW_CTRL(i),
				MVCPU_WIN_CTRL_DATA(size, USB_TARGET_DRAM,
					attrib, MVCPU_WIN_ENABLE));
		else
			wrl(USB_WINDOW_CTRL(i), MVCPU_WIN_DISABLE);

		wrl(USB_WINDOW_BASE(i), base);
	}
}
#endif

/* Device tree global data scanned at 1st init for usb2 nodes */
int node_list[CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS];

#ifdef CONFIG_OF_CONTROL
/* Parse and save enabled device tree usb nodes, and return enabled node count */
int board_usb_get_enabled_port_count(void)
{
	static int count = -1;

	/* Scan the device tree once only */
	if (count < 0) {
		/* Scan device tree usb nodes once, and save relevant nodes in static node_list */
		count = fdtdec_find_aliases_for_id(gd->fdt_blob, "usb",
				COMPAT_MVEBU_USB, node_list, CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS);
	}

	if (count == 0)
		printf("%s: 'usb' is disabled in Device Tree\n", __func__);

	/* Return enabled port count */
	return count;
}
#endif

#ifdef CONFIG_USB_EHCI_MARVELL_SBUSCFG
/* For SoC without hlock, need to program sbuscfg value to guarantee
 * AHB master's burst would not overrun or underrun FIFO.
 *
 * sbuscfg reg has to be set after usb controller reset, otherwise the value
 * would be override to 0, in order not to change mainline ehci driver,
 * __weak routine ehci_powerup_fixup which is invoked in mainline ehci driver
 * is override to perform this configuration.
 */
void ehci_powerup_fixup(uint32_t *status_reg, uint32_t *reg)
{
	/* set default value for reg SBUSCFG, which is Control for the AMBA
	 * system bus interface,
	 * BAWR = BARD = 4 : Align read/write bursts packets larger than 64 bytes
	 * AHBBRST = 7     : Align AHB Burst for packets larger than 64 bytes
	 */
	writel((USB_SBUSCFG_BAWR_ALIGN_64B << USB_SBUSCFG_BAWR_OFF)
	      | (USB_SBUSCFG_BARD_ALIGN_64B << USB_SBUSCFG_BARD_OFF)
	      | (USB_SBUSCFG_AHBBRST_INCR16 << USB_SBUSCFG_AHBBRST_OFF),
	      usb_reg_base + USB2_SBUSCFG_OFF);
	mdelay(50);
}
#endif

/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
int ehci_hcd_init(int index, enum usb_init_type init,
		struct ehci_hccr **hccr, struct ehci_hcor **hcor)
{
#ifdef CONFIG_OF_CONTROL
	unsigned long node;

	/* node_list: Enabled DT nodes were initialized in usb_device_tree_init(),
	 * so it's valid to use node_list[index] to fetch its registers */
	node = node_list[index];

	/* fetch 'reg' property from 'usb' node */
	usb_reg_base = (unsigned long)fdt_get_regs_offs(gd->fdt_blob, node, "reg");
	if (usb_reg_base == FDT_ADDR_T_NONE) {
		error("could not find reg in usb node, initialization skipped!\n");
		return -ENXIO;
	}
#endif
#ifndef CONFIG_USB_EHCI_MARVELL_BYPASS_BRG_ADDR_DEC
	usb_brg_adrdec_setup();
#endif

	/* without FDT support, MVUSB0_BASE is defined in header file,
	 * and only one usb2 controller is supported.
	 *
	 * with FDT support, MVUSB0_BASE is defined as usb_reg_base,
	 * and will be initialized again in this routine for different
	 * usb2 controller, so multiple usb2 controllers are supported.
	 */
	*hccr = (struct ehci_hccr *)(MVUSB0_BASE + 0x100);
	*hcor = (struct ehci_hcor *)((uintptr_t) *hccr
			+ HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));

	debug("ehci-marvell: init hccr %lx and hcor %lx hc_length %ld\n",
		(uintptr_t)*hccr, (uintptr_t)*hcor,
		(uintptr_t)HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));

	return 0;
}

/*
 * Destroy the appropriate control structures corresponding
 * the the EHCI host controller.
 */
int ehci_hcd_stop(int index)
{
	return 0;
}
