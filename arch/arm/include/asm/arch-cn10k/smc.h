/* SPDX-License-Identifier:    GPL-2.0
 *
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#ifndef __SMC_H__
#define __SMC_H__

#include <asm/arch/smc-id.h>
#include <asm/arch/update.h>

ssize_t smc_dram_size(unsigned int node);
ssize_t	smc_disable_rvu_lfs(unsigned int node);

/*
 * Get RVU Reserved Memory Region Info
 *
 * Return:
 *	x0:
 *		0 -- Success
 *	x1 - region start address
 *	x2 - region size
 */
int smc_rvu_rsvd_reg_info(u64 *reg_addr, u64 *reg_size);

/*
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
int smc_spi_update(const struct smc_update_descriptor *desc);

/*
 * Perform Switch Firmware load to DRAM in ATF
 *
 * x1 - super image location
 * x2 - cm3 image location
 * x3 - ptr to cm3 image size
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *		-2 -- SPI_CONFIG_ERR
 *		-3 -- SPI_MMAP_ERR
 *		-5 -- EIO
 */
int smc_load_switch_fw(u64 super_img_addr, u64 cm3_img_addr,
		       u64 *cm3_img_size);

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
int smc_load_efi_img(u64 img_addr, u64 *img_size);

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
int smc_efi_var_shared_memory(u64 *mem_addr, u64 *mem_size);

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
int smc_write_efi_var(u64 var_addr, u64 var_size, u32 bus, u32 cs);

/*
 * Perform secure SPI flash operation
 *
 * x1 - Offset in flash
 * x2 - Buffer pointer
 * x3 - Size
 * X4 - x3[3:0] - Bus, x3[7:4] - Chipselecti, [15:8] - Operation
 *	Operation: 1 - Read, 4 - Info
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 */
unsigned long smc_sec_spi_op(u64 offset, u64 buffer, u64 size, u32 bus,
			     u32 cs, u32 op);

/**
 * Verify objects in flash
 *
 * x1 - address of descriptor
 * x2 - size of descriptor
 *
 * returns:
 *	x0:
 *		0 -- success
 *		negative - error
 */
int smc_spi_verify(struct smc_version_info *desc);

#endif
