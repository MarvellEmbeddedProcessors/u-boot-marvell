/* SPDX-License-Identifier:    GPL-2.0
 *
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#ifndef __RPM_H__
#define __RPM_H__

#include "eth_intf.h"

#define PCI_DEVICE_ID_CN10K_RPM	0xA060

#define MAX_LMAC_PER_RPM		4
#define RPM_PER_NODE			3

/* Register offsets */
#define RPM_CMR_INT		0x87e0e0000040
#define RPM_CMR_SCRATCH0	0x87e0e0001050
#define RPM_CMR_SCRATCH1	0x87e0e0001058

#define RPM_SHIFT(x)		(0x1000000 * ((x) & 0x3))
#define CMR_SHIFT(x)		(0x100000 * ((x) & 0x3))

enum lmac_type {
	LMAC_MODE_SGMII		= 0,
	LMAC_MODE_XAUI		= 1,
	LMAC_MODE_RXAUI		= 2,
	LMAC_MODE_10G_R		= 3,
	LMAC_MODE_40G_R		= 4,
	LMAC_MODE_QSGMII	= 6,
	LMAC_MODE_25G_R		= 7,
	LMAC_MODE_50G_R		= 8,
	LMAC_MODE_100G_R	= 9,
	LMAC_MODE_USXGMII	= 10,
};

extern char lmac_type_to_str[][8];

extern char lmac_speed_to_str[][8];

struct lmac_priv {
	u8 enable:1;
	u8 full_duplex:1;
	u8 speed:4;
	u8 mode:1;
	u8 rsvd:1;
	u8 mac_addr[6];
};

struct rpm;
struct nix;
struct nix_af;

struct lmac {
	struct rpm	*rpm;
	struct nix	*nix;
	char		name[16];
	enum lmac_type	lmac_type;
	bool		init_pend;
	u8		instance;
	u8		lmac_id;
	u8		pknd;
	u8		link_num;
	u32		chan_num;
	u8		mac_addr[6];
};

struct rpm {
	struct nix_af		*nix_af;
	void __iomem		*reg_base;
	struct udevice		*dev;
	struct lmac		*lmac[MAX_LMAC_PER_RPM];
	u8			rpm_id;
	u8			lmac_count;
};

static inline void rpm_write(struct rpm *rpm, u8 lmac, u64 offset, u64 val)
{
	writeq(val, rpm->reg_base + CMR_SHIFT(lmac) + offset);
}

static inline u64 rpm_read(struct rpm *rpm, u8 lmac, u64 offset)
{
	return readq(rpm->reg_base + CMR_SHIFT(lmac) + offset);
}

/**
 * Given an LMAC/PF instance number, return the lmac
 * Per design, each PF has only one LMAC mapped.
 *
 * @param instance	instance to find
 *
 * @return	pointer to lmac data structure or NULL if not found
 */
struct lmac *nix_get_rpm_lmac(int lmac_instance);

int rpm_lmac_set_chan(struct lmac *lmac);
int rpm_lmac_set_pkind(struct lmac *lmac, u8 lmac_id, int pkind);
int rpm_lmac_internal_loopback(struct lmac *lmac, int lmac_id, bool enable);
int rpm_lmac_rx_tx_enable(struct lmac *lmac, int lmac_id, bool enable);
int rpm_lmac_link_enable(struct lmac *lmac, int lmac_id, bool enable,
			 u64 *status);
int rpm_lmac_link_status(struct lmac *lmac, int lmac_id, u64 *status);
void rpm_lmac_mac_filter_setup(struct lmac *lmac);

int eth_intf_get_link_sts(u8 rpm, u8 lmac, u64 *lnk_sts);
int eth_intf_link_up_dwn(u8 rpm, u8 lmac, u8 up_dwn, u64 *lnk_sts);
int eth_intf_get_mac_addr(u8 rpm, u8 lmac, u8 *mac);
int eth_intf_set_macaddr(struct udevice *dev);
int eth_intf_prbs(u8 qlm, u8 mode, u32 time, u8 lane);
int eth_intf_display_eye(u8 qlm, u8 lane);
int eth_intf_display_serdes(u8 qlm, u8 lane);

#endif /* __RPM_H__ */
