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

/* API required from all SOCs */
int soc_get_rev(void);
int soc_get_id(void);
void soc_init(void);

/* Common SOC API */
#ifdef CONFIG_USB_XHCI
void board_usb_vbus_init(void);
#endif
int mvebu_soc_init(void);
void mvebu_print_soc_info(void);
int init_func_i2c(void);

bool mvebu_is_in_recovery_mode(void);

DECLARE_GLOBAL_DATA_PTR;

#endif /* _SOC_H_ */
