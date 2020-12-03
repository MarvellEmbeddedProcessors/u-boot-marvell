// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#ifndef __UPDATE_H__
#define __UPDATE_H__

/*
 * The following is copied from the ATF file
 * include/drivers/marvell/spi_smc_update.h.  This should be updated
 * whenever ATF is updated.
 */
#define UPDATE_MAGIC		0x55504454	/** UPDT */
/** Current smc_update_descriptor version */
#define UPDATE_VERSION		0x0001

#define UPDATE_FLAG_BACKUP	0x0001	/** Set to update secondary location */
#define UPDATE_FLAG_EMMC	0x0002	/** Set to update eMMC instead of SPI */
/** Set when user parameters are passed */
#define UPDATE_FLAG_USER_PARMS	0x8000

/**
 * This descriptor is passed from U-Boot or other software performing an update
 * for CN10K.
 */
struct smc_update_descriptor {
	uint32_t	magic;		/** UPDATE_MAGIC */
	uint16_t	version;	/** Version of descriptor */
	uint16_t	update_flags;	/** Flags passed to update process */
	uint64_t	image_addr;	/** Address of image (CPIO file) */
	uint64_t	image_size;	/** Size of image (CPIO file) */
	uint32_t	bus;		/** SPI BUS number */
	uint32_t	cs;		/** SPI chip select number */
	uint64_t	reserved;	/** Space to add stuff */
	uint64_t	user_addr;	/** Passed to customer function */
	uint64_t	user_size;	/** Passed to customer function */
	uint64_t	user_flags;	/** Passed to customer function */
};

#endif /* __UPDATE_H__ */
