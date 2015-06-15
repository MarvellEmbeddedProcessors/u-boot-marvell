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

#ifndef _PINCTL_H_
#define _PINCTL_H_

const char *pinctl_get_name(int bank_id);
int  pinctl_set_pin_func(int bank, int pin_id, int func);
int  pinctl_get_pin_func(int bank, int pin_id);
int  pinctl_get_bank_id(const char *bank_name);
int  pinctl_get_pin_cnt(int bank_id);
int  mvebu_pinctl_probe(void);

#endif /* _PINCTL_H_ */

