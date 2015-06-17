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

#ifndef _MPP_BUS_H_
#define _MPP_BUS_H_

#define MAX_MPP_BUSES	10

struct mpp_pin {
	u32 id;
	u32 func;
};

struct mpp_bus {
	const char *name;
	int pin_cnt;
	int bank_id;
	struct mpp_pin *pins;
	int valid;
};

int  mpp_bus_probe(void);
int  mpp_enable_bus(const char *name);
int  mpp_is_bus_enabled(const char *name);
struct mpp_bus *mpp_get_bus(int id);

u32 mpp_get_pin_func(int bank_id, u32 pin_id);
int mpp_set_pin_func(int bank_id, u32 pin_id, u32 func);
int mpp_get_bank_pins(int bank_id);
const char *mpp_get_bank_name(int bank_id);

#endif /* _MPP_BUS_H_ */
