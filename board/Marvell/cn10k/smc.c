// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <asm/io.h>

#include <asm/system.h>
#include <asm/arch/smc.h>

#include <asm/psci.h>

#include <malloc.h>

DECLARE_GLOBAL_DATA_PTR;

ssize_t smc_dram_size(unsigned int node)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_DRAM_SIZE;
	regs.regs[1] = node;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_disable_rvu_lfs(unsigned int node)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_DISABLE_RVU_LFS;
	regs.regs[1] = node;
	smc_call(&regs);

	return regs.regs[0];
}

/*
 * Perform Switch Firmware load to DRAM in ATF
 *
 * x1 - super image location
 * x2 - cm3 image location
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *		-2 -- SPI_CONFIG_ERR
 *		-3 -- SPI_MMAP_ERR
 *		-5 -- EIO
 */
int smc_switch_fw_load(u64 super_img_addr, u64 cm3_img_addr)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SWITCH_FW_LOAD;
	regs.regs[1] = super_img_addr;
	regs.regs[2] = cm3_img_addr;
	smc_call(&regs);

	return regs.regs[0];
}

/*
 * Perform SPI Update from ATF
 *
 * x1 - user_buffer
 * x2 - size
 * x3 - bus
 * x4 - chip select
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *		-2 -- SPI_CONFIG_ERR
 *		-3 -- SPI_MMAP_ERR
 *		-4 -- SPI_IMG_VALIDATE_ERR
 *		-5 -- SPI_IMG_UPDATE_ERR
 */
int smc_spi_update(u64 user_buffer, u32 size, u32 bus, u32 cs)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SPI_SECURE_UPDATE;
	regs.regs[1] = user_buffer;
	regs.regs[2] = size;
	regs.regs[3] = bus;
	regs.regs[4] = cs;
	smc_call(&regs);

	return regs.regs[0];
}
