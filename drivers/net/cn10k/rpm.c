// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <net.h>
#include <malloc.h>
#include <dm.h>
#include <misc.h>
#include <asm/io.h>
#include <errno.h>
#include <linux/list.h>
#include <asm/arch/board.h>
#include <asm/arch/csrs/csrs-rpm.h>
#include "rpm.h"

char lmac_type_to_str[][8] = {
	"SGMII",
	"XAUI",
	"RXAUI",
	"10G_R",
	"40G_R",
	"RGMII",
	"QSGMII",
	"25G_R",
	"50G_R",
	"100G_R",
	"USXGMII",
};

char lmac_speed_to_str[][8] = {
	"0",
	"10M",
	"100M",
	"1G",
	"2.5G",
	"5G",
	"10G",
	"20G",
	"25G",
	"40G",
	"50G",
	"80G",
	"100G",
};

static u64 sh_fwbase;

extern int eth_intf_get_fwdata_base(u64 *base);

void init_sh_fwdata(void)
{
	int ret;

	ret = eth_intf_get_fwdata_base(&sh_fwbase);
	if (ret)
		printf("Shared FW Base init failed\n");
}

void print_fwdata_lmac_type(int rpm_id, int lmac_id)
{
	int lmac_type;
	struct sh_fwdata *data = (struct sh_fwdata *)sh_fwbase;

	if (data) {
		lmac_type = data->eth_fw_data[rpm_id][lmac_id].lmac_type;
		printf("RPM%d LMAC%d [%s]\n", rpm_id, lmac_id,
		       lmac_type_to_str[lmac_type]);
	}
}

/**
 * Given an LMAC/PF instance number, return the lmac
 * Per design, each PF has only one LMAC mapped.
 *
 * @param instance	instance to find
 *
 * @return	pointer to lmac data structure or NULL if not found
 */
struct lmac *nix_get_rpm_lmac(int lmac_instance)
{
	struct rpm *rpm;
	struct udevice *dev;
	int i, idx, err;

	for (i = 0; i < RPM_PER_NODE; i++) {
		err = dm_pci_find_device(PCI_VENDOR_ID_CAVIUM,
					 PCI_DEVICE_ID_CN10K_RPM, i,
					 &dev);
		if (err)
			continue;

		rpm = dev_get_priv(dev);
		debug("%s udev %p rpm %p instance %d\n", __func__, dev, rpm,
		      lmac_instance);
		for (idx = 0; idx < rpm->lmac_count; idx++) {
			if (rpm->lmac[idx]->instance == lmac_instance)
				return rpm->lmac[idx];
		}
	}
	return NULL;
}

void rpm_lmac_mac_filter_clear(struct lmac *lmac)
{
	union rpmx_cmrx_rx_dmac_ctl0 dmac_ctl0;
	union rpmx_cmr_rx_dmacx_cam0 dmac_cam0;
	void *reg_addr;

	dmac_cam0.u = 0x0;
	reg_addr = lmac->rpm->reg_base +
			RPMX_CMR_RX_DMACX_CAM0(lmac->lmac_id * 8);
	writeq(dmac_cam0.u, reg_addr);
	debug("%s: reg %p dmac_cam0 %llx\n", __func__, reg_addr, dmac_cam0.u);

	dmac_ctl0.u = 0x0;
	dmac_ctl0.s.bcst_accept = 1;
	dmac_ctl0.s.mcst_mode = 1;
	dmac_ctl0.s.cam_accept = 0;
	reg_addr = lmac->rpm->reg_base +
			RPMX_CMRX_RX_DMAC_CTL0(lmac->lmac_id);
	writeq(dmac_ctl0.u, reg_addr);
	debug("%s: reg %p dmac_ctl0 %llx\n", __func__, reg_addr, dmac_ctl0.u);
}

void rpm_lmac_mac_filter_setup(struct lmac *lmac)
{
	union rpmx_cmrx_rx_dmac_ctl0 dmac_ctl0;
	union rpmx_cmr_rx_dmacx_cam0 dmac_cam0;
	u64 mac, tmp;
	void *reg_addr;

	memcpy((void *)&tmp, lmac->mac_addr, 6);
	mac = swab64(tmp) >> 16;
	debug("%s: mac %llx\n", __func__, mac);
	dmac_cam0.u = 0x0;
	dmac_cam0.s.id = lmac->lmac_id;
	dmac_cam0.s.adr = mac;
	dmac_cam0.s.en = 1;
	reg_addr = lmac->rpm->reg_base +
			RPMX_CMR_RX_DMACX_CAM0(lmac->lmac_id * 8);
	writeq(dmac_cam0.u, reg_addr);
	debug("%s: reg %p dmac_cam0 %llx\n", __func__, reg_addr, dmac_cam0.u);
	dmac_ctl0.u = 0x0;
	dmac_ctl0.s.bcst_accept = 1;
	dmac_ctl0.s.mcst_mode = 0;
	dmac_ctl0.s.cam_accept = 1;
	reg_addr = lmac->rpm->reg_base +
			RPMX_CMRX_RX_DMAC_CTL0(lmac->lmac_id);
	writeq(dmac_ctl0.u, reg_addr);
	debug("%s: reg %p dmac_ctl0 %llx\n", __func__, reg_addr, dmac_ctl0.u);
}

int rpm_lmac_set_chan(struct lmac *lmac)
{
	union rpmx_cmrx_link_cfg link_cfg;

	link_cfg.u = 0;
	link_cfg.s.log2_range = 0x4;
	link_cfg.s.base_chan = lmac->chan_num;
	rpm_write(lmac->rpm, lmac->lmac_id, RPMX_CMRX_LINK_CFG(0),
		  link_cfg.u);
	return 0;
}

int rpm_lmac_set_pkind(struct lmac *lmac, u8 lmac_id, int pkind)
{
	rpm_write(lmac->rpm, lmac_id, RPMX_CMRX_RX_ID_MAP(0),
		  (pkind & 0x3f));
	return 0;
}

int rpm_lmac_link_status(struct lmac *lmac, int lmac_id, u64 *status)
{
	int ret = 0;

	ret = eth_intf_get_link_sts(lmac->rpm->rpm_id, lmac_id, status);
	if (ret) {
		debug("%s request failed for rpm%d lmac%d\n",
		      __func__, lmac->rpm->rpm_id, lmac->lmac_id);
		ret = -1;
	}
	return ret;
}

int rpm_lmac_rx_tx_enable(struct lmac *lmac, int lmac_id, bool enable)
{
	struct rpm *rpm = lmac->rpm;
	union rpmx_cmrx_config cmrx_config;

	if (!rpm || lmac_id >= rpm->lmac_count)
		return -ENODEV;

	cmrx_config.u = rpm_read(rpm, lmac_id, RPMX_CMRX_CONFIG(0));
	cmrx_config.s.data_pkt_rx_en =
	cmrx_config.s.data_pkt_tx_en = enable ? 1 : 0;
	rpm_write(rpm, lmac_id, RPMX_CMRX_CONFIG(0), cmrx_config.u);
	return 0;
}

int rpm_lmac_link_enable(struct lmac *lmac, int lmac_id, bool enable,
			 u64 *status)
{
	int ret = 0;

	ret = eth_intf_link_up_dwn(lmac->rpm->rpm_id, lmac_id, enable,
				   status);
	if (ret) {
		debug("%s request failed for rpm%d lmac%d\n",
		      __func__, lmac->rpm->rpm_id, lmac->lmac_id);
		ret = -1;
	}
	return ret;
}

static int rpm_lmac_init(struct rpm *rpm)
{
	struct lmac *lmac;
	static int instance = 1;
	union rpmx_cmr_rx_lmacs rx_lmacs;
	int i;

	rx_lmacs.u = rpm_read(rpm, 0, RPMX_CMR_RX_LMACS());
	rpm->lmac_count = fls(rx_lmacs.s.lmac_exist);
	debug("%s: Found %d lmacs for rpm %d@%p\n", __func__, rpm->lmac_count,
	      rpm->rpm_id, rpm->reg_base);

	for (i = 0; i < rpm->lmac_count; i++) {
		lmac = calloc(1, sizeof(*lmac));
		if (!lmac)
			return -ENOMEM;
		lmac->instance = instance++;
		snprintf(lmac->name, sizeof(lmac->name), "rpm_fwi_%d_%d",
			 rpm->rpm_id, i);

		lmac->lmac_id = i;
		lmac->rpm = rpm;
		rpm->lmac[i] = lmac;
		debug("%s: map id %d to lmac %p (%s), instance %d\n",
		      __func__, i, lmac, lmac->name, lmac->instance);
		lmac->init_pend = 1;
		cn10k_board_get_mac_addr((lmac->instance - 1),
					 lmac->mac_addr);
		debug("%s: MAC %pM\n", __func__, lmac->mac_addr);
		rpm_lmac_mac_filter_setup(lmac);
		print_fwdata_lmac_type(rpm->rpm_id, i);
	}
	return 0;
}

int rpm_probe(struct udevice *dev)
{
	struct rpm *rpm = dev_get_priv(dev);
	int err;

	rpm->reg_base = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_0,
				       PCI_REGION_MEM);
	rpm->dev = dev;
	rpm->rpm_id = ((u64)(rpm->reg_base) >> 24) & 0x7;

	err = rpm_lmac_init(rpm);

	return err;
}

int rpm_remove(struct udevice *dev)
{
	struct rpm *rpm = dev_get_priv(dev);
	int i;

	for (i = 0; i < rpm->lmac_count; i++)
		rpm_lmac_mac_filter_clear(rpm->lmac[i]);

	return 0;
}

U_BOOT_DRIVER(rpm) = {
	.name	= "rpm",
	.id	= UCLASS_MISC,
	.probe	= rpm_probe,
	.remove	= rpm_remove,
	.priv_auto_alloc_size = sizeof(struct rpm),
};

static struct pci_device_id rpm_supported[] = {
	{PCI_VDEVICE(CAVIUM, PCI_DEVICE_ID_CN10K_RPM) },
	{}
};

U_BOOT_PCI_DEVICE(rpm, rpm_supported);
