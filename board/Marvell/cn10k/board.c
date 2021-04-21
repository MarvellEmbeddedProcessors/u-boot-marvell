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
#include <env.h>
#include <init.h>
#include <log.h>
#include <malloc.h>
#include <net.h>
#include <errno.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <linux/libfdt.h>
#include <fdt_support.h>
#include <asm/arch/smc.h>
#include <asm/arch/soc.h>
#include <asm/arch/board.h>
#include <asm/arch/switch.h>
#include <dm/util.h>
#include <spi.h>

extern ssize_t smc_flsf_fw_booted(void);

DECLARE_GLOBAL_DATA_PTR;

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

void board_get_spi_bus_cs(struct udevice *dev, int *bus, int *cs)
{
	struct udevice *busp, *csp;
	struct udevice *parent = dev_get_parent(dev);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			if (!spi_find_bus_and_cs(i, j, &busp, &csp)) {
				if (parent == busp && dev == csp) {
					*bus = i;
					*cs = j;
					break;
				}
			}
		}
	}
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

void board_get_env_offset(int *offset, const char *property)
{
	const void *blob = gd->fdt_blob;
	int env_offset;
	int node;

	env_offset = -1;
	node = fdt_node_offset_by_compatible(blob, -1, "spi-flash");
	while (node > 0) {
		if (fdtdec_get_bool(blob, node, "u-boot,env")) {
			env_offset = fdtdec_get_int(blob, node, property, -1);
			debug("\n %s : 0x%x\n", property, env_offset);
			break;
		}
		node = fdt_node_offset_by_compatible(blob, node, "spi-flash");
	}
	if (env_offset == -1)
		debug("\%s property not found in fdt\n", property);

	*offset = env_offset;
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
	u64 rvu_addr, rvu_size;
	int ret;

	gd->ram_size = smc_dram_size(0);
	gd->ram_size -= CONFIG_SYS_SDRAM_BASE;

	ret = smc_rvu_rsvd_reg_info(&rvu_addr, &rvu_size);
	mem_map_fill(rvu_addr, rvu_size);

	return 0;
}

void board_late_probe_devices(void)
{
	struct udevice *dev;
	int err, rpm_cnt, i;

	switch (read_partnum()) {
	case CNF10KA:
		rpm_cnt = 4;
		break;
	case CNF10KB:
		rpm_cnt = 9;
		break;
	default:
		rpm_cnt = 3;
		break;
	}

	/* Probe MAC(RPM) and NIC AF devices before Network stack init */
	for (i = 0; i < rpm_cnt; i++) {
		err = dm_pci_find_device(PCI_VENDOR_ID_CAVIUM,
					 PCI_DEVICE_ID_CAVIUM_RPM, i, &dev);
		if (err)
			debug("%s RPM%d device not found\n", __func__, i);
	}
	err = dm_pci_find_device(PCI_VENDOR_ID_CAVIUM,
				 PCI_DEVICE_ID_CAVIUM_RVU_AF, 0, &dev);
	if (err)
		debug("RVU AF device not found\n");
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

	str = fdt_get_board_model();
	if (!str)
		str = "Marvell";
	snprintf(boardname, sizeof(boardname), "%s> ", str);
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

	if (IS_ENABLED(CONFIG_CN10K_ETH_INTF))
		init_sh_fwdata();
	if (IS_ENABLED(CONFIG_NET_CN10K))
		board_late_probe_devices();

	if (IS_ENABLED(CONFIG_TARGET_CN10K_A))
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

	/* Bring down all lmac links */
	if (IS_ENABLED(CONFIG_CN10K_ETH_INTF))
		eth_intf_shutdown();

	/* Removes all RPM and RVU AF devices */
	ret = uclass_get(UCLASS_MISC, &uc_dev);
	if (uc_dev)
		ret = uclass_destroy(uc_dev);
	if (ret)
		printf("couldn't remove misc (rpm/rvu_af) devices\n");

	/* SMC call - removes all LF<->PF mappings */
	smc_disable_rvu_lfs(0);

	if (IS_ENABLED(CONFIG_TARGET_CN10K_A))
		board_switch_reset();

	/* Removes watchdog */
	ret = uclass_get(UCLASS_WDT, &uc_dev);
	if (uc_dev)
		ret = uclass_destroy(uc_dev);
	if (ret)
		printf("couldn't stop watchdog\n");
}

/*
 * Invoked before relocation, so limit to stack variables.
 */
int checkboard(void)
{
	const char *str;

	str = fdt_get_board_model();
	if (!str)
		str = "UNKNOWN";

	printf("Board: %s\n", fdt_get_board_model());

	return 0;
}

#ifdef CONFIG_LAST_STAGE_INIT
int last_stage_init(void)
{
	(void)smc_flsf_fw_booted();
	return 0;
}
#endif
