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

#ifndef _MPP_BUS_H_
#define _MPP_BUS_H_

#define MAX_MPP_BUSES	20

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
