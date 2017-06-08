/*
 * Copyright (C) 2017 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#ifndef __MVEBU_PHY_INDIRECT_H__
#define __MVEBU_PHY_INDIRECT_H__

enum mci_unit {
	AP_PHY,
	AP_CTRL,
	CP_PHY,
	CP_CTRL,
	MCI_MAX
};

enum mci_region {
	MCI_CTRL_REGION     = 0,
	MCI_PHY_REG_REGION  = 3,
};

/* Functions prototypes */
int mvebu_mci_phy_read(enum mci_region region, enum mci_unit unit,
		       int unit_id, int reg_ofs, u32 *val);
int mvebu_mci_phy_write(enum mci_region region, enum mci_unit unit,
			int unit_id, int reg_ofs, u32 val);

#endif /* __MVEBU_MMC_H__ */
