/*
 * mv_nand.c
 *
 * Copyright c 2014 Marvell International Ltd.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "mvBoardEnvLib.h"
#include "nand.h"
#include "nand_nfc.h"
#include "nand_spi.h"

int board_nand_init(struct nand_chip *nand)
{
#ifdef MV_NAND_SPI
	MV_NAND_IF_MODE mode = mvBoardNandIfGet();
#else
	MV_NAND_IF_MODE mode = NAND_IF_NFC;
#endif

	switch (mode) {
#if defined(MV_NAND)
	case NAND_IF_NFC:
		return board_nand_nfc_init(nand);
#endif

#ifdef MV_NAND_SPI
	case NAND_IF_SPI:
		return board_nand_spi_init(nand);
#endif
	default:
		return -1;
	}
}
