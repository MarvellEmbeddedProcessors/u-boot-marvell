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

#ifndef _UNIT_INFO_H_
#define _UNIT_INFO_H_

enum mvebu_unit_id {
	DRAM_UNIT_ID,
	CPU_UNIT_ID,
	PCIE_UNIT_ID,
	ETH_GIG_UNIT_ID,
	USB_UNIT_ID,
	USB3_UNIT_ID,
	IDMA_UNIT_ID,
	IDMA_CHAN_UNIT_ID,
	XOR_UNIT_ID,
	XOR_CHAN_UNIT_ID,
	SATA_UNIT_ID,
	TDM_32CH_UNIT_ID,
	TDM_PORTS_UNIT_ID,
	UART_UNIT_ID,
	CESA_UNIT_ID,
	SPI_UNIT_ID,
	SDIO_UNIT_ID,
	BM_UNIT_ID,
	PNC_UNIT_ID,
	NAND_UNIT_ID,
	SERDES_LANES_UNIT_ID,
	DEVICE_BUS_UNIT_ID,
	MAX_UNIT_ID
};

enum unit_update_mode {
	UNIT_INFO_DISABLE,
	UNIT_INFO_ENABLE,
	UNIT_INFO_OVERRIDE
};

#define MAX_UNIT_COUNT		16

#define disable_unit(dst, mask)		((*dst) &= (~mask))
#define enable_unit(dst, mask)		((*dst) |= mask)
#define override_unit(dst, mask)	((*dst) = mask)

#define cnt_to_mask(count)	((1 << count) - 1)
#define id_to_mask(id)		(1 << id)

void dump_unit_info(void);
void update_unit_info(u16 *unit_mask, u16 *new_mask, enum unit_update_mode);
int  unit_info_get_count(enum mvebu_unit_id id);
bool unit_info_is_active(enum mvebu_unit_id id, int index);
u16  unit_info_get_mask(enum mvebu_unit_id id);
void unit_info_disable_units(enum mvebu_unit_id id, u16 mask);
void unit_info_disable_unit_instance(enum mvebu_unit_id id, int index);

#endif /* _UNIT_INFO_H_ */
