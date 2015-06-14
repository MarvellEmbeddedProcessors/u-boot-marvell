/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt or on the worldwide
 * web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
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

