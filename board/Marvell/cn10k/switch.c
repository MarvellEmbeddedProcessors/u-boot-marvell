// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <console.h>
#include <dm.h>
#include <log.h>
#include <linux/delay.h>
#include <dm/uclass-internal.h>
#include <malloc.h>
#include <errno.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <asm/arch/smc.h>
#include <asm/arch/soc.h>
#include <asm/arch/board.h>
#include <dm/util.h>

DECLARE_GLOBAL_DATA_PTR;

#define	SUPER_FW_RAM_ADDR	0x10040000
#define	CM3_FW_RAM_ADDR		0x10100000

static struct pci_device_id switch_supported[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x9000) },
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x9200) },
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x9210) },
	{}
};

void copy32(void *source, void *dest, u32 count)
{
	u32 *src = source, *dst = dest;

	count >>= 2;
	while (count--)
		writel(readl(src++), dst++);
}

int load_switch_images(u64 *cm3_size)
{
	return smc_load_switch_fw(SUPER_FW_RAM_ADDR, CM3_FW_RAM_ADDR,
				  cm3_size);
}

struct udevice *get_switch_dev(void)
{
	int ret;
	struct udevice *udev = NULL;

	ret = pci_find_device_id(switch_supported, 0, &udev);

	return udev;
}

void board_switch_reset(void)
{
	struct udevice *dev;
	void *sw_bar0, *sw_bar2;
	u32 sw_bar2_lo, sw_bar2_hi, val;

	dev = get_switch_dev();
	if (!dev)
		return;

	sw_bar0 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
	if (!sw_bar0) {
		debug("Switch device BAR not configured\n");
		return;
	}
	sw_bar2 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_2, PCI_REGION_MEM);
	if (!sw_bar2) {
		debug("Switch device BAR not configured\n");
		return;
	}
	sw_bar2_lo = (u32)((ulong)sw_bar2 & 0xffffffff);
	sw_bar2_hi = (u32)(((ulong)sw_bar2 >> 32) & 0xffffffff);

	/*
	 * Open 1M iATU address translation window into Prestera DFX-server,
	 * at ‘second window’ of BAR2 , for addresses with prefix 0x000xxxxx
	 * (DFX-server)
	 * mw 0x872000001500 0x00000000; ATU ctrl reg 1
	 * mw 0x872000001504 0x80000000; ATU ctrl reg 2 (bit 31: enable window)
	 * mw 0x872000001508 0x00900000; ATU lower base address (start of BAR2)
	 * mw 0x87200000150c 0x00008720; ATU upper base address
	 * mw 0x872000001510 0x009fffff; ATU limit address (limit to 1MB)
	 * mw 0x872000001514 0x00000000; ATU target lower base address
					 (start of DFX-server)
	 * mw 0x872000001518 0x00000000; ATU target upper base address
	 */
	writel(0x0, sw_bar0 + 0x1500);
	writel(0x80000000, sw_bar0 + 0x1504);
	writel(sw_bar2_lo + 0x100000, sw_bar0 + 0x1508);
	writel(sw_bar2_hi, sw_bar0 + 0x150c);
	writel(sw_bar2_lo + 0x1fffff, sw_bar0 + 0x1510);
	writel(0x0, sw_bar0 + 0x1514);
	writel(0x0, sw_bar0 + 0x1518);

	/*
	 * Open 1M iATU address translation window into Prestera CNM-RUNIT,
	 * at ‘third window’ of BAR2 , for addresses with prefix 0x3c0xxxxx
	 * (CNM-RUNIT) :
	 * mw 0x872000001700 0x00000000; ATU ctrl reg 1
	 * mw 0x872000001704 0x80000000; ATU ctrl reg 2 (bit 31: enable window)
	 * mw 0x872000001708 0x00a00000; ATU lower base address (start of BAR2)
	 * mw 0x87200000170c 0x00008720; ATU upper base address
	 * mw 0x872000001710 0x00afffff; ATU limit address (limit to 1MB)
	 * mw 0x872000001714 0x3c000000; ATU target lower base address
					 (start of CNM-RUNIT)
	 * mw 0x872000001718 0x00000000; ATU target upper base address
	 */
	writel(0x0, sw_bar0 + 0x1700);
	writel(0x80000000, sw_bar0 + 0x1704);
	writel(sw_bar2_lo + 0x200000, sw_bar0 + 0x1708);
	writel(sw_bar2_hi, sw_bar0 + 0x170c);
	writel(sw_bar2_lo + 0x2fffff, sw_bar0 + 0x1710);
	writel(0x3c000000, sw_bar0 + 0x1714);
	writel(0x0, sw_bar0 + 0x1718);

	/*
	 * Write_mask 0x8720009F806C DATA 0x00000000 MASK 0x00000002
	 * Write_mask 0x8720009F800C DATA 0x00000000 MASK 0x00001010
	 * Write_mask 0x872000a00044 DATA 0x00000001 MASK 0x00000001
	 * Write_mask 0x872000a00044 DATA 0x00000000 MASK 0x00000001
	 */
	val = readl(sw_bar2 + 0x100000 + 0xf806c);
	val &= ~0x00000002;
	val |= 0x0;
	writel(val, sw_bar2 + 0x100000 + 0xf806c);
	val = readl(sw_bar2 + 0x100000 + 0xf800c);
	val &= ~0x00001010;
	val |= 0x0;
	writel(val, sw_bar2 + 0x100000 + 0xf800c);
	val = readl(sw_bar2 + 0x200000 + 0x44);
	val &= ~0x00000001;
	val |= 0x1;
	writel(val, sw_bar2 + 0x200000 + 0x44);
	val = readl(sw_bar2 + 0x200000 + 0x44);
	val &= ~0x00000001;
	val |= 0x0;
	writel(val, sw_bar2 + 0x200000 + 0x44);
	mdelay(5);
}

void board_switch_init(void)
{
	struct udevice *dev;
	struct pci_child_platdata *pplat;
	void *sw_bar0, *sw_bar2;
	u32 sw_bar2_lo, sw_bar2_hi;
	u64 cm3_img_sz;

	dev = get_switch_dev();
	if (!dev)
		return;

	sw_bar0 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
	if (!sw_bar0) {
		debug("Switch device BAR not configured\n");
		return;
	}
	sw_bar2 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_2, PCI_REGION_MEM);
	if (!sw_bar2) {
		debug("Switch device BAR not configured\n");
		return;
	}
	sw_bar2_lo = (u32)((ulong)sw_bar2 & 0xffffffff);
	sw_bar2_hi = (u32)(((ulong)sw_bar2 >> 32) & 0xffffffff);

	pplat = dev_get_parent_platdata(dev);
	printf("Switch device [%x:%x] detected\n", pplat->vendor, pplat->device);
	debug("BAR0 %p BAR2 %p\n", sw_bar0, sw_bar2);
	printf("Executing micro-init sequence... ");
	/*
	 * Open 1M iATU address translation window into Prestera MG0, at start of BAR2:
	 *	wr bar0 + 1300 0x00000000;	ATU ctrl reg 1
	 *	wr bar0 + 1304 0x80000000; 	ATU ctrl reg 2 (bit 31: enable window)
	 *	wr bar0 + 1308 0x00800000; 	ATU lower base address (start of BAR2)
	 *	wr bar0 + 130c 0x00008720;	ATU upper base address
	 *	wr bar0 + 1310 0x008fffff; 	ATU limit address (limit to 1MB)
	 * 	wr bar0 + 1314 0x3c200000; 	ATU target lower base address (start of MG0)
	 * 	wr bar0 + 1318 0x00000000; 			ATU target upper base address

	 * In MG0 confi processor window settings, configure window #1 (Address decoding for CM3):
	 *	wr bar2 + 0490 0x10000803; 	Base address for CM3 address space
	 * 	wr bar2 + 0494 0x00ff0000; 	Size 16MB
	 * 	wr bar2 + 0498 0x00000000;
	 *	wr bar2 + 049c 0xf200000e; 	Target address 0xf2000000 (address > 2G will go out to PCIe)

	 * Open 1M oATU address translation window out to host CPU DDR with target offset 0xabcd03000000:
	 *	wr bar0 + 1200 0x00000000; 	oATU ctrl reg 1
	 * 	wr bar0 + 1204 0x80000000; 	oATU ctrl reg 2 (bit 31: enable window)
	 *	wr bar0 + 1208 0xf2000000; 	oATU lower base address (start of 'DDR' in terms internal CM3 memory space)
	 * 	wr bar0 + 120c 0x00000000; 	oATU upper base address
	 * 	wr bar0 + 1210 0xf23fffff; 	oATU limit address (limit to 4MB)
	 *	wr bar0 + 1214 0x10000000; 	oATU target lower base address (start of DDR on PCIe)
	 *	wr bar0 + 1218 0x00000000; 	oATU target upper base address (start of DDR on PCIe)

	 * MG0 completion registers:
	 * 	wr bar2 + 0124 0x0001;		MG0 window mapping for proper CM3 access to the switch (register0)
	 * 	wr bar2 + 0140 0x8102;		MG0 window mapping for proper CM3 access to the switch (register1)

	 * Load super-image to 0x10040000: (super-image must reside 0x40000 after base offset)
	 * Will be loaded directly from SPI flash or tftpboot 0x10040000 super_image.bin;

	 * Load MI FW to 0x10100000 by tftp, then copy to SRAM:
	 * Will be loaded directly from SPI flash or tftpboot 0x10200000 firmware_cm3.bin;

	 * cp.l 0x10100000 bar2 + 0x80000 $filesize; 	SRAM based at MG0 + 0x80000 (32bit at a time)

	 * MG0 Register 0x500 (Confi Processor Global Configuration) – disable and then enable CM3 CPU (bits 28:29):
	 * wr bar2 + 0500 0x000e1a80;		CM3 CPU (bits 28:29) disable.
	 * wr bar2 + 0500 0x300e1a80;		CM3 CPU (bits 28:29) enable.
	 */
	writel(0x0, sw_bar0 + 0x1300);
	writel(0x80000000, sw_bar0 + 0x1304);
	writel(sw_bar2_lo, sw_bar0 + 0x1308);
	writel(sw_bar2_hi, sw_bar0 + 0x130c);
	writel(sw_bar2_lo + 0xfffff, sw_bar0 + 0x1310);
	writel(0x3c200000, sw_bar0 + 0x1314);
	writel(0x0, sw_bar0 + 0x1318);

	writel(0x10000803, sw_bar2 + 0x0490);
	writel(0x00ff0000, sw_bar2 + 0x0494);
	writel(0x0, sw_bar2 + 0x0498);
	writel(0xf200000e, sw_bar2 + 0x049c);

	writel(0x0, sw_bar0 + 0x1200);
	writel(0x80000000, sw_bar0 + 0x1204);
	writel(0xf2000000, sw_bar0 + 0x1208);
	writel(0x0, sw_bar0 + 0x120c);
	writel(0xf23fffff, sw_bar0 + 0x1210);
	writel(SUPER_FW_RAM_ADDR - 0x40000, sw_bar0 + 0x1214);
	writel(0x0, sw_bar0 + 0x1218);

	writel(0x0001, sw_bar2 + 0x0124);
	writel(0x8102, sw_bar2 + 0x0140);

	if (load_switch_images(&cm3_img_sz)) {
		printf("Loading switch firmware to RAM failed\n");
		return;
	}

	copy32((void *)CM3_FW_RAM_ADDR, sw_bar2 + 0x80000, cm3_img_sz);

	writel(0x000e1a80, sw_bar2 + 0x0500);
	writel(0x300e1a80, sw_bar2 + 0x0500);

	//TODO - Status check of super/ap image access completion
	printf("done.\n");
}

