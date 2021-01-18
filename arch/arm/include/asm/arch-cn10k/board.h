/* SPDX-License-Identifier:    GPL-2.0
 *
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <asm/arch/soc.h>

/** Reg offsets */

extern unsigned long fdt_base_addr;

/** Function definitions */
void mem_map_fill(u64 rvu_addr, u64 rvu_size);
int fdt_get_board_mac_cnt(bool *use_id);
u64 fdt_get_board_mac_addr(bool use_id, u8 id);
const char *fdt_get_board_model(void);
const char *fdt_get_board_serial(void);
const char *fdt_get_board_revision(void);
void cn10k_board_get_mac_addr(u8 index, u8 *mac_addr);
void eth_intf_shutdown(void);
void init_sh_fwdata(void);
void board_get_env_spi_bus_cs(int *bus, int *cs);
void board_get_env_offset(int *offset, const char *property);
void board_get_spi_bus_cs(struct udevice *dev, int *bus, int *cs);
#endif /* __BOARD_H__ */
