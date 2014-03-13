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

#ifndef _SOC_H_
#define _SOC_H_

#include <asm/arch-mvebu/unit-info.h>
#include <asm/arch-mvebu/adec.h>
#include <asm/arch-mvebu/mpp.h>
#include <asm/arch/soc-info.h>

#define MAX_SOC_NAME	   (32)

struct mvebu_soc_family {
	char name[MAX_SOC_NAME];
	int	 id;
	u16	*base_unit_info;
	char	*mpp_desc[MAX_MPP_OPTS][MAX_MPP_NAME];
	struct mvebu_soc_info *soc_table;
	struct mvebu_soc_info *curr_soc;
	int adec_type;
};

struct mvebu_soc_info {
	char name[MAX_SOC_NAME];
	int	 id;
	u16	*unit_disable;
	struct adec_win *memory_map;
};

int common_soc_init(struct mvebu_soc_family *soc_family_info);

/* API required from all SOCs */
int soc_get_rev(void);
int soc_get_id(void);
u16 *soc_get_unit_mask_table(void);

#endif /* _SOC_H_ */
