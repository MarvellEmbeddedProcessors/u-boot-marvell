// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <dm.h>
#include <malloc.h>
#include <errno.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <asm/arch/soc.h>
#include <asm/arch/board.h>
#include <dm/util.h>

platform_t read_platform(void)
{
	platform_t plat = PLATFORM_HW;

	const char *model = fdt_get_board_model();

	if (model && !strncmp(model, "ASIM-", 5))
		plat = PLATFORM_ASIM;
	if (model && !strncmp(model, "EMUL-", 5))
		plat = PLATFORM_EMULATOR;

	return plat;
}

static inline u64 read_midr(void)
{
	u64 result;

	asm ("mrs %[rd],MIDR_EL1" : [rd] "=r" (result));
	return result;
}

u8 read_partvar(void)
{
	return ((read_midr() >> 20) & 0xF);
}

#define FUS_CACHE0_REG 0x87e003001000
u8 read_partnum(void)
{
	u64 fus_cache0 = readq(FUS_CACHE0_REG);
	return (u8)(fus_cache0 & 0xff);
}

const char *read_board_name(void)
{
	return fdt_get_board_model();
}
