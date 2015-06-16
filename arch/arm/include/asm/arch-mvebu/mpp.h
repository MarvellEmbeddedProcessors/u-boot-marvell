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

#ifndef _MPP_H_
#define _MPP_H_

#include  <asm/arch/soc-info.h>

#define		MAX_MPP_NAME		32
#define		MAX_BUS_NAME		32
#define		MAX_PINS_PER_BUS	14
#define		MAX_BUS_OPTS		2

char **mpp_get_desc_table(void);

#ifndef CONFIG_MVEBU_MPP

#define mpp_set_pin(mpp_id)
#define mpp_get_pin(mpp_id)
#define mpp_set_and_update(mpp_reg)
#define mpp_is_bus_enabled(bus)
#define mpp_is_bus_valid(bus)
#define mpp_enable_bus(mpp_reg, bus_id, bus_alt)


#else
int mpp_bus_probe(void);

u8   mpp_get_pin(int mpp_id);
void mpp_set_pin(int mpp_id, int value);
int mpp_enable_bus(char *name);
int  mpp_is_bus_valid(char *name);
int  mpp_is_bus_enabled(char *name);

#endif /* CONFIG_MVEBU_MPP */

#endif /* _MPP_H_ */
