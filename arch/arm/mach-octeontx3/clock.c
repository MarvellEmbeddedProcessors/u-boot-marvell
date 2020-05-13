// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/board.h>
#include <asm/arch/clock.h>

/**
 * Returns the I/O clock speed in Hz
 */
u64 octeontx_get_io_clock(void)
{
	return 15 * PLL_REF_CLK;
}

