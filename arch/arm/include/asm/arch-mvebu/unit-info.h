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

#ifndef _UNIT_INFO_H_
#define _UNIT_INFO_H_

enum mvebu_unit_id {
	DRAM_UNIT_ID,
	CPU_UNIT_ID,
	PEX_UNIT_ID,
	PEX_IF_UNIT_ID,
	PCI_UNIT_ID,
	PCI_IF_UNIT_ID,
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
void unit_info_disable_units(enum mvebu_unit_id id, u16 mask);
void unit_info_disable_unit_instance(enum mvebu_unit_id id, int index);

#endif /* _UNIT_INFO_H_ */
