// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/ptrace.h>
#include <asm/arch/smc.h>
#include <asm/psci.h>
#include <asm/arch/update.h>
#include <efi_loader.h>

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
 * Get RVU Reserved Memory Region Info
 *
 * Return:
 *	x0:
 *		0 -- Success
 *	x1 - region start address
 *	x2 - region size
 */
int smc_rvu_rsvd_reg_info(u64 *reg_addr, u64 *reg_size)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_RVU_RSVD_REG_INFO;
	smc_call(&regs);

	*reg_addr = regs.regs[1];
	*reg_size = regs.regs[2];
	return regs.regs[0];
}

/*
 * Perform EFI Application Image load to DRAM in ATF
 *
 * x1 - Image location
 * x2 - Pointer to store image size
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *		-2 -- SPI_CONFIG_ERR
 *		-3 -- SPI_MMAP_ERR
 *		-5 -- EIO
 */
int smc_load_efi_img(u64 img_addr, u64 *img_size)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_LOAD_EFI_APP;
	regs.regs[1] = img_addr;
	smc_call(&regs);

	*img_size = regs.regs[1];
	return regs.regs[0];
}

/*
 * Get EFI variabled shared memory info
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *	x1:
 *		Physical address of the shared memory
 *	x2:
 *		Size in bytes of this shared memory
 */
int smc_efi_var_shared_memory(u64 *mem_addr, u64 *mem_size)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_GET_EFI_SHARED_MEM;

	smc_call(&regs);

	*mem_addr = regs.regs[1];
	*mem_size = regs.regs[2];
	return regs.regs[0];
}

/*
 * Perform EFI variable store write to flash in ATF
 *
 * x1 - Variable store location
 * x2 - Variable store size
 * x3 - Flash device bus number
 * X4 - Flash device chip select
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 */
__efi_runtime int smc_write_efi_var(u64 var_addr, u64 var_size, u32 bus, u32 cs)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_WRITE_EFI_VAR;
	regs.regs[1] = var_addr;
	regs.regs[2] = var_size;
	regs.regs[3] = bus;
	regs.regs[4] = cs;

	smc_call(&regs);

	return regs.regs[0];
}

/*
 * Perform Switch Firmware load to DRAM in ATF
 *
 * x1 - super image location
 * x2 - cm3 image location
 * x3 - ptr to store cm3 size
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *		-2 -- SPI_CONFIG_ERR
 *		-3 -- SPI_MMAP_ERR
 *		-5 -- EIO
 */
int smc_load_switch_fw(u64 super_img_addr, u64 cm3_img_addr, u64 *cm3_img_size)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_LOAD_SWITCH_FW;
	regs.regs[1] = super_img_addr;
	regs.regs[2] = cm3_img_addr;
	smc_call(&regs);

	*cm3_img_size = regs.regs[1];
	return regs.regs[0];
}

/*
 * Perform SPI Update from ATF
 *
 * x1 - descriptor address
 * x2 - descriptor size
 * x3 - 0
 * x4 - 0
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
int smc_spi_update(const struct smc_update_descriptor *desc)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SPI_SECURE_UPDATE;
	regs.regs[1] = (uint64_t)desc;
	regs.regs[2] = sizeof(*desc);
	regs.regs[3] = 0;
	regs.regs[4] = 0;
	smc_call(&regs);

	return regs.regs[0];
}
