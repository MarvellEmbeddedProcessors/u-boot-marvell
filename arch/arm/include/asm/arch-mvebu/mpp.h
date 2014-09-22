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

enum mpp_bus_id {
	UART_0_MPP_BUS,
	UART_1_MPP_BUS,
	SPI_0_MPP_BUS,
	SPI_1_MPP_BUS,
	NAND_0_MPP_BUS,
	RGMII_0_MPP_BUS,
	I2C_0_MPP_BUS,
	GE_0_MPP_BUS,
	GE_1_MPP_BUS,
	M_VTT_0_MPP_BUS,
	SDIO_0_MPP_BUS,
	MAX_MPP_BUS
};

struct mpp_pin {
	u8 id;
	u8 val;
};

struct mpp_bus {
	char name[MAX_BUS_NAME];
	int pin_cnt;
	int bus_cnt;
	struct mpp_pin pin_data[MAX_BUS_OPTS][MAX_PINS_PER_BUS];
};

u8   mpp_get_pin(int mpp_id);
void mpp_set_pin(int mpp_id, int value);
int  mpp_enable_bus(u32 *mpp_reg, int bus_id, int bus_alt);
int  mpp_is_bus_valid(struct mpp_bus *bus);
int  mpp_is_bus_enabled(struct mpp_bus *bus);
void mpp_set_and_update(u32 *mpp_reg);
struct mpp_bus *soc_get_mpp_bus(int bus_id);

#endif /* _MPP_H_ */
