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
#define SPI_CONFIG_ERR		2
#define SPI_MMAP_ERR		3
#define SPI_IMG_VALIDATE_ERR	4
#define SPI_IMG_UPDATE_ERR	5
#define SPI_BAD_MAGIC_NUMBER	6
#define SPI_BAD_PARAMETER	7

#define VER_MAX_NAME_LENGTH	32
#define SMC_MAX_OBJECTS		32

#define VERIFY_LOG_SIZE		1024

/** Maximum length of NUL terminated version string */
#define VERSION_STRING_LENGTH	32

enum update_ret {
	/** No errors */
	UPDATE_OK = 0,
	/** Error with the CPIO image */
	UPDATE_CPIO_ERROR = -1,
	/** Invalid TIM found in update */
	UPDATE_TIM_ERROR = -2,
	/** One or more files failed hash check */
	UPDATE_HASH_ERROR = -3,
	/** Update authentication error */
	UPDATE_AUTH_ERROR = -4,
	/** I/O error reading or writing to the flash */
	UPDATE_IO_ERROR = -5,
	/**
	 * Error found that requires all objects to be updated,
	 * i.e. a corrupt object found in the existing flash
	 */
	UPDATE_REQUIRE_FULL = -6,
	/** Out of resources, too many files, etc. */
	UPDATE_NO_MEM = -7,
	/** Problem found with device tree firmware-update section */
	UPDATE_DT_ERROR = -8,
	/** Incomplete file grouping found */
	UPDATE_GROUP_ERROR = -9,
	/** Location or size of an object invalid */
	UPDATE_LOCATION_ERROR = -10,
	/** Unsupported media */
	UPDATE_INVALID_MEDIA = -11,
	/** Invalid alignment of update file */
	UPDATE_BAD_ALIGNMENT = -12,
	/** TIM is missing in an object */
	UPDATE_MISSING_TIM = -13,
	/** File is missing in an object */
	UPDATE_MISSING_FILE = -14,
	/** TIM is missing in flash */
	UPDATE_TIM_MISSING = -15,
	/** I/O issue with eHSM component */
	UPDATE_EHSM_ERROR = -16,
	/** Update rejected due to version check */
	UPDATE_VERSION_CHECK_FAIL = -17,
	/** Bad magic number in update descriptor */
	UPDATE_BAD_DESC_MAGIC = -18,
	/** Unsupported version in update descriptor */
	UPDATE_BAD_DESC_VERSION = -19,
	/** Error mapping update to secure memory */
	UPDATE_MMAP_ERROR = -20,

	UPDATE_WORK_BUFFER_TOO_SMALL = -21,
	/** Unknown error */
	UPDATE_UNKNOWN_ERROR = -1000,
};

struct smc_update_obj_info {

};

/**
 * The following is taken directly from libtim.h
 */
struct tim_opaque_data_version_info {
    uint8_t	major_version;	/** Major version number */
    uint8_t	minor_version;	/** Minor version number */
    uint8_t	revision_number;/** Revision number */
    uint8_t	revision_type;	/** Revision type (TBD) */
    uint16_t	year;		/** GIT Year */
    uint8_t	month;		/** GIT Month */
    uint8_t	day;		/** GIT Day */
    uint8_t	hour;		/** GIT Hour */
    uint8_t	minute;		/** GIT Minute */
    uint16_t	flags;		/** Flags (TBD) */
    uint32_t	customer_version;/** Customer defined version number */
    /**
     * String representation of version
     * The version string contains:
     * XX.YY.ZZ YYYYMMDD-HHmm (customer hex)
     * Where:
     * XX: Major version number
     * YY: Minor version number
     * ZZ: Revision number
     * YYYY: 4-digit year
     * MM: 2-digit month
     * DD: 2-digit day of month
     * HH: Hour
     * mm: Minute
     *
     * The timestamp is extracted from GIT
     */
    uint8_t	version_string[VERSION_STRING_LENGTH];
} __attribute__((packed, aligned(4)));
/**
 * Note: the following needs to be updated in U-Boot and other update tools
 * whenever this is changed.
 */
#define UPDATE_MAGIC		0x55504454	/* UPDT */
/** Current smc_update_descriptor version */
#define UPDATE_VERSION		0x0001

#define UPDATE_FLAG_BACKUP	0x0001	/** Set to update secondary location */
#define UPDATE_FLAG_EMMC	0x0002	/** Set to update eMMC instead of SPI */
#define UPDATE_FLAG_ERASE_PART	0x0004	/** Erase eMMC partition data */
#define UPDATE_FLAG_IGNORE_VERSION 0x0008 /** Don't perform version check */
/** Set when user parameters are passed */
#define UPDATE_FLAG_USER_PARMS	0x8000

/** Offset from the beginning of the flash where the backup image is located */
#define BACKUP_IMAGE_OFFSET	0x2000000
/**
 * This descriptor is passed by U-Boot or other software performing an update
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
	uintptr_t	work_buffer;	/** Used for compressed objects */
	uint64_t	work_buffer_size;/** Size of work buffer */
	struct smc_update_obj_info object_retinfo[SMC_MAX_OBJECTS];
};

/** This is used for each object (version entry) */
enum smc_version_entry_retcode {
	RET_OK = 0,
	RET_NOT_FOUND = 1,
	RET_TIM_INVALID = 2,
	RET_BAD_HASH = 3,
	RET_NOT_ENOUGH_MEMORY = 4,
	/**
	 * If the names mismatch, this return code is set and the actual name
	 * found is copied into the name field.
	 */
	RET_NAME_MISMATCH = 5,
	RET_TIM_NO_VERSION = 6,
	RET_TIM_NO_HASH = 7,
	RET_HASH_ENGINE_ERROR = 8,
	RET_HASH_NO_MATCH = 9,
	/**
	 * This is returned if the length reported by the TIM header is greater
	 * than the maximum size allowed by the device tree entry.  The TIM
	 * image size will be reported as the entry size.
	 */
	RET_IMAGE_TOO_BIG = 10,
	RET_DEVICE_TREE_ENTRY_ERROR = 11,
};



struct smc_version_info_entry {
	char name[VER_MAX_NAME_LENGTH];
	struct tim_opaque_data_version_info version;
	uint8_t tim_hash[512 / 8];	/** Hash value stored in the TIM */
	uint8_t obj_hash[512 / 8];	/** Calculated hash value */
	uint64_t tim_address;		/** Address of TIM in flash */
	uint64_t max_size;		/** Maximum space for object and TIM */
	uint64_t object_size;		/** Size of flash object in bytes */
	uint64_t object_address;	/** Address of object in flash */
	uint16_t hash_size;		/** Size of hash in bytes */
	uint16_t flags;			/** Flags for this object */
	enum smc_version_entry_retcode retcode;	/** Return code if error */
	uint64_t reserved[8];		/** Reserved for future growth */
	uint8_t log[VERIFY_LOG_SIZE];	/** Log for object */
};

#define VERSION_FLAG_BACKUP	BIT(0)	/** Set to use backup offset */

/**
 * Set if objects are stored in eMMC, leave zero for SPI NOR
 */
#define VERSION_FLAG_EMMC	BIT(1)

/**
 * If this bit is set, only the object names specified in the objects
 * will be checked, otherwise, all objects will be checked and any data
 * in the objects array will be ignored.
 */
#define SMC_VERSION_CHECK_SPECIFIC_OBJECTS	BIT(2)

/**
 * If set, either the specified or all of the objects will have their hashes
 * verified, otherwise, no verification will be performed.
 */
#define SMC_VERSION_CHECK_VALIDATE_HASH		BIT(3)

/**
 * Maximum number of objects that can return the version info
 */
#define SMC_MAX_VERSION_ENTRIES			32

/** Return code for version info */
enum smc_version_ret {
	VERSION_OK,			/** Header is good */
	/**
	 * The firmware layout has changed so not all objects can be
	 * verified.  Objects are located using the device tree which
	 * is initialized at boot time.
	 */
	FIRMWARE_LAYOUT_CHANGED,
	/**
	 * If the number of objects exceeds the num_objects field then this
	 * is returned and num_objects will contain the number of objects
	 * found.
	 */
	TOO_MANY_OBJECTS,
	INVALID_DEVICE_TREE,		/** firmware-layout section missing */
	VERSION_NOT_SUPPORTED,		/** Version descriptor not supported */
};

#define VERSION_MAGIC		0x4e535256	/** VRSN */
#define VERSION_INFO_VERSION	0x0100	/** 1.0 */

struct smc_version_info {
	uint32_t	magic_number;	/** VRSN */
	uint16_t	version;	/** Version of descriptor */
	uint16_t	version_flags;	/** Flags passed to version process */
	uint32_t	bus;		/** SPI BUS number */
	uint32_t	cs;		/** SPI chip select number */
	/*
	 * Note that currently the work buffers are not used since the images
	 * are read from flash in chunks for verification purposes.
	 */
	uintptr_t	work_buffer_addr;/** Used to decompress objects */
	uint64_t	work_buffer_size;/** Size of decompression buffer */
	enum smc_version_ret	retcode;
	/**
	 * On entry, if all objects are to be verified then this contains
	 * the maximum number of objects to verify and this specifies the size
	 * of the objects array.
	 * On exit this will contain the number of objects actually verified.
	 *
	 * If the return code is TOO_MANY_OBJECTS then this will return the
	 * actual number of objects which will be greater than the number
	 * of available entries.
	 */
	uint32_t	num_objects;
	uint32_t	timeout;	/** Timeout in ms */
	uint32_t	pad32;		/** Pad to 64 bits */
	uint64_t	reserved[5];	/** Reserved for future growth */
	/** Array of objects to verify */
	struct smc_version_info_entry objects[SMC_MAX_VERSION_ENTRIES];
};

#endif /* __UPDATE_H__ */
