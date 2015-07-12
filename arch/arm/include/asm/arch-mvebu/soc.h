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

#ifndef _SOC_H_
#define _SOC_H_

#include <asm/arch/soc-info.h>

#define MAX_SOC_NAME	32
#define MAX_SOC_REVS	8
#define MAX_REV_NAME	4

struct mvebu_soc_info {
	char	name[MAX_SOC_NAME];
	int	id;
};

struct mvebu_soc_family {
	int	id;
	char	name[MAX_SOC_NAME];
	char	rev_name[MAX_SOC_REVS][MAX_REV_NAME];
	struct	mvebu_soc_info *soc_table;
	struct	mvebu_soc_info *curr_soc;
};

int common_soc_init(struct mvebu_soc_family *soc_family_info);

/* API required from all SOCs */
int soc_get_rev(void);
int soc_get_id(void);
struct mvebu_soc_family *soc_init(void);

/* Common SOC API */
int mvebu_soc_init(void);
void mvebu_print_soc_info(void);

DECLARE_GLOBAL_DATA_PTR;
#define get_soc_family()  (struct mvebu_soc_family *)(gd->arch.soc_family)
#define set_soc_family(x) (gd->arch.soc_family = (struct mvebu_soc_family *)(x))
#define get_soc_info()  (struct mvebu_soc_info *)(gd->arch.soc_family->curr_soc)

#endif /* _SOC_H_ */
