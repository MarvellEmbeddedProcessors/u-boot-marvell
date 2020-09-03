// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <console.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <malloc.h>
#include <errno.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <linux/libfdt.h>
#include <fdt_support.h>
#include <asm/arch/smc.h>
#include <asm/arch/soc.h>
#include <asm/arch/board.h>
#include <dm/util.h>

DECLARE_GLOBAL_DATA_PTR;

extern unsigned long fdt_base_addr;
extern void eth_intf_shutdown(void);

void cleanup_env_ethaddr(void)
{
	char ename[32];

	for (int i = 0; i < 20; i++) {
		sprintf(ename, i ? "eth%daddr" : "ethaddr", i);
		if (env_get(ename))
			env_set(ename, NULL);
	}
}

void cn10k_board_get_mac_addr(u8 index, u8 *mac_addr)
{
	u64 tmp_mac, mac;
	static int mac_num;
	bool use_id;

	memset(mac_addr, 0, ARP_HLEN);
	mac_num = fdt_get_board_mac_cnt(&use_id);

	if (mac_num && index < mac_num) {
		mac = fdt_get_board_mac_addr(use_id, index);
		if (!is_zero_ethaddr((u8 *)&mac)) {
			tmp_mac = mac;
			if (!use_id)
				tmp_mac += index;
			tmp_mac = swab64(tmp_mac) >> 16;
			memcpy(mac_addr, (u8 *)&tmp_mac, ARP_HLEN);
		}
	}
	debug("%s mac %pM\n", __func__, mac_addr);
}

void board_get_env_spi_bus_cs(int *bus, int *cs)
{
	const void *blob = gd->fdt_blob;
	int env_bus, env_cs;
	int node, preg;

	env_bus = -1;
	env_cs = -1;
	node = fdt_node_offset_by_compatible(blob, -1, "spi-flash");
	while (node > 0) {
		if (fdtdec_get_bool(blob, node, "u-boot,env")) {
			env_cs = fdtdec_get_int(blob, node, "reg", -1);
			preg = fdtdec_get_int(blob,
					      fdt_parent_offset(blob, node),
					      "reg", -1);
			/* SPI node will have PCI addr, so map it */
			if (preg == 0x3000)
				env_bus = 0;
			if (preg == 0x3800)
				env_bus = 1;
			debug("\n Env SPI [bus:cs] [%d:%d]\n",
			      env_bus, env_cs);
			break;
		}
		node = fdt_node_offset_by_compatible(blob, node, "spi-flash");
	}
	if (env_bus == -1)
		debug("\'u-boot,env\' property not found in fdt\n");

	*bus = env_bus;
	*cs = env_cs;
}

int board_early_init_r(void)
{
	pci_init();
	return 0;
}

int board_init(void)
{
	return 0;
}

int timer_init(void)
{
	return 0;
}

int dram_init(void)
{
	gd->ram_size = smc_dram_size(0);
	gd->ram_size -= CONFIG_SYS_SDRAM_BASE;

	mem_map_fill();

	return 0;
}

#ifdef CONFIG_NET_CN10K
#define	PCI_DEVID_CN10K_RPM			0xA060
#define	PCI_DEVID_OCTEONTX2_RVU_AF		0xA065
void board_late_probe_devices(void)
{
	struct udevice *dev;
	int err, rpm_cnt = 3, i;

	/* Probe MAC(RPM) and NIC AF devices before Network stack init */
	for (i = 0; i < rpm_cnt; i++) {
		err = dm_pci_find_device(PCI_VENDOR_ID_CAVIUM,
					 PCI_DEVID_CN10K_RPM, i, &dev);
		if (err)
			debug("%s RPM%d device not found\n", __func__, i);
	}
	err = dm_pci_find_device(PCI_VENDOR_ID_CAVIUM,
				 PCI_DEVID_OCTEONTX2_RVU_AF, 0, &dev);
	if (err)
		debug("RVU AF device not found\n");
}
#endif

#define	PCI_DEVID_SWITCH	0x9000
#define	AP_FW_FLASH_ADDR	0x5a0000
#define	AP_FW_RAM_ADDR		0x10100000
#define	AP_FW_SIZE		119768
#define	SUPER_FW_FLASH_ADDR	0x520000
#define	SUPER_FW_RAM_ADDR	0x10040000
#define	SUPER_FW_SIZE		30664

void copy32(void *source, void *dest, u32 count)
{
	u32 *src = source, *dst = dest;

	count >>= 2;
	while (count--)
		writel(readl(src++), dst++);
}

int load_switch_images(void)
{
	return smc_switch_fw_load(SUPER_FW_RAM_ADDR, AP_FW_RAM_ADDR);
}

void board_switch_reset(void)
{
	struct udevice *dev;
	int err;
	void *sw_bar0, *sw_bar2;
	u32 sw_bar2_lo, sw_bar2_hi, val;

	err = dm_pci_find_device(PCI_VENDOR_ID_MARVELL,
				 PCI_DEVID_SWITCH, 0, &dev);
	if (err) {
		debug("Switch device not found\n");
		return;
	}
	sw_bar0 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
	sw_bar2 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_2, PCI_REGION_MEM);
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
	int err;
	void *sw_bar0, *sw_bar2;
	u32 sw_bar2_lo, sw_bar2_hi;

	err = dm_pci_find_device(PCI_VENDOR_ID_MARVELL,
				 PCI_DEVID_SWITCH, 0, &dev);
	if (err) {
		debug("Switch device not found\n");
		return;
	}
	sw_bar0 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
	sw_bar2 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_2, PCI_REGION_MEM);
	sw_bar2_lo = (u32)((ulong)sw_bar2 & 0xffffffff);
	sw_bar2_hi = (u32)(((ulong)sw_bar2 >> 32) & 0xffffffff);

	printf("Switch device BAR0 %p BAR2 %p\n", sw_bar0, sw_bar2);
	printf("Performing micro-init sequence... ");
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

	if (load_switch_images()) {
		printf("Loading switch firmware to RAM failed\n");
		return;
	}

	copy32((void *)AP_FW_RAM_ADDR, sw_bar2 + 0x80000, AP_FW_SIZE);

	writel(0x000e1a80, sw_bar2 + 0x0500);
	writel(0x300e1a80, sw_bar2 + 0x0500);

	//TODO - Status check of super/ap image access completion
	printf("done.\n");
}

/**
 * Board late initialization routine.
 */
int board_late_init(void)
{
	char boardname[32];
	char boardserial[150], boardrev[150];
	bool save_env = false;
	const char *str;

	debug("%s()\n", __func__);

	/*
	 * Now that pci_init initializes env device.
	 * Try to cleanup ethaddr env variables, this is needed
	 * as with each boot, configuration of QLM can change.
	 */
	cleanup_env_ethaddr();

	snprintf(boardname, sizeof(boardname), "%s> ", fdt_get_board_model());
	env_set("prompt", boardname);
	set_working_fdt_addr(env_get_hex("fdtcontroladdr", fdt_base_addr));

	str = fdt_get_board_revision();
	if (str) {
		snprintf(boardrev, sizeof(boardrev), "%s", str);
		str = env_get("boardrev");
		if (str && strcmp(boardrev, str))
			save_env = true;
		env_set("boardrev", boardrev);
	}

	str = fdt_get_board_serial();
	if (str) {
		snprintf(boardserial, sizeof(boardserial), "%s", str);
		str = env_get("serial#");
		if (str && strcmp(boardserial, str))
			save_env = true;
		env_set("serial#", boardserial);
	}

#ifdef CONFIG_NET_CN10K
	board_late_probe_devices();
#endif

	board_switch_init();

	if (save_env)
		env_save();

	return 0;
}

void board_quiesce_devices(void)
{
	struct uclass *uc_dev;
	int ret;

	/* Removes all RVU PF devices */
	ret = uclass_get(UCLASS_ETH, &uc_dev);
	if (uc_dev)
		ret = uclass_destroy(uc_dev);
	if (ret)
		printf("couldn't remove rvu pf devices\n");

#ifdef CONFIG_CN10K_ETH_INTF
	/* Bring down all lmac links */
	eth_intf_shutdown();
#endif

	/* Removes all RPM and RVU AF devices */
	ret = uclass_get(UCLASS_MISC, &uc_dev);
	if (uc_dev)
		ret = uclass_destroy(uc_dev);
	if (ret)
		printf("couldn't remove misc (rpm/rvu_af) devices\n");

	/* SMC call - removes all LF<->PF mappings */
	smc_disable_rvu_lfs(0);
	board_switch_reset();
}

/*
 * Invoked before relocation, so limit to stack variables.
 */
int show_board_info(void)
{
	char *str;

	if (otx_is_soc(CN106XX))
		str = "CN106XX";
	else
		str = "UNKNOWN";
	printf("Marvell CN10K %s ARM V8 Core\n", str);

	printf("Board: %s\n", fdt_get_board_model());

	return 0;
}

