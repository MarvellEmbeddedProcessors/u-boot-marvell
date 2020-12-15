// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <command.h>
#include <div64.h>
#include <asm/arch/update.h>
#include <asm/arch/smc.h>

static int do_bootimgup(struct cmd_tbl *cmdtp, int flag, int argc,
			char * const argv[])
{
	unsigned long value;
	int ret;
	struct smc_update_descriptor desc;
	const char *env_addr, *env_size;
	bool spi = false, mmc = false;

	memset(&desc, 0, sizeof(desc));
	desc.magic = UPDATE_MAGIC;
	desc.version = UPDATE_VERSION;
	argv++;
	argc--;

	if (CONFIG_IS_ENABLED(CMD_BOOTIMGUP_CUST_SIG)) {
		/*
		 * The customer signature information can be either at the end
		 * or stored in environment variables (or both).
		 */
		env_addr = env_get("sigaddr");
		env_size = env_get("sigsize");

		if (env_addr && env_size) {
			ret = strict_strtoul(env_addr, 16, &value);
			if (ret) {
				printf("Invalid value for environment variable %s\n",
				       "sigaddr");
				return CMD_RET_FAILURE;
			}
			desc.user_addr = value;
			ret = strict_strtoul(env_size, 16, &value);
			if (ret) {
				printf("Invalid value for environment variable %s\n",
				       "siglen");
				return CMD_RET_FAILURE;
			}
			desc.user_size = value;
			pr_debug("Signature address: 0x%llx, size: 0x%llx\n",
				 desc.user_addr, desc.user_size);
		}
	}

	env_addr = env_get("fileaddr");
	env_size = env_get("filesize");

	if (env_addr) {
		ret = strict_strtoul(env_addr, 16, &value);
		if (ret) {
			printf("Invalid fileaddr value %s\n", env_addr);
			return CMD_RET_FAILURE;
		}
		desc.image_addr = value;
		pr_debug("fileaddr: 0x%llx\n", desc.image_addr);
	}
	if (env_size) {
		ret = strict_strtoul(env_size, 16, &value);
		if (ret) {
			printf("Invalid filesize value %s\n", env_size);
			return CMD_RET_FAILURE;
		}
		desc.image_size = value;
		pr_debug("filesize: 0x%llx\n", desc.image_size);
	}

	/* Walk through all arguments */
	while (argc > 0) {
		pr_debug("Parsing argument \"%s\"\n", argv[0]);
		if (!strcmp(argv[0], "-v")) {
			desc.update_flags |= UPDATE_FLAG_IGNORE_VERSION;
			argc--;
			argv++;
			continue;
		}
		if (CONFIG_IS_ENABLED(CMD_BOOTIMGUP_BACKUP) &&
		    !strcmp(argv[0], "-b")) {
			desc.update_flags |= UPDATE_FLAG_BACKUP;
			argc--;
			argv++;
			pr_debug("Update backup image\n");
			continue;
		}
		/* Parse customer signature */
		if (CONFIG_IS_ENABLED(CMD_BOOTIMGUP_CUST_SIG) &&
		    !strcmp(argv[0], "-sig")) {
			if (argc < 3) {
				printf("Missing address and/or size for signature data\n");
				return CMD_RET_USAGE;
			}
			ret = strict_strtoul(argv[1], 16, &value);
			if (ret) {
				printf("Invalid signature address %s\n",
				       argv[1]);
				return CMD_RET_USAGE;
			}
			desc.user_addr = value;
			ret = strict_strtoul(argv[2], 16, &value);
			if (ret) {
				printf("Invalid signature size %s\n", argv[2]);
				return CMD_RET_USAGE;
			}
			desc.user_size = value;
			argc -= 3;
			argv += 3;
			pr_debug("Signature address: 0x%llx, size: 0x%llx\n",
				 desc.user_addr, desc.user_size);
			continue;
		}
		if (!strcmp(argv[0], "-p")) {
			desc.update_flags |= UPDATE_FLAG_ERASE_PART;
			argc--;
			argv++;
			pr_debug("Overwrite partition data\n");
			continue;
		}
		if (!strcmp(argv[0], "mmc")) {
			argv++;
			argc--;
			/*
			 * mmc can have the parameters:
			 *   [device ID/bus]
			 * or
			 *   [-p] [device ID/bus]
			 * or
			 *   [device ID/bus] [image address] [image size]
			 * or
			 *   [-p] [device ID/bus] [image address] [image size]
			 */
			if (argc > 0 && !strcmp(argv[1], "-p")) {
				desc.update_flags |= UPDATE_FLAG_ERASE_PART;
				pr_debug("Overwrite partition data\n");
				argc--;
				argv++;
			}
			if (argc < 1) {
				printf("mmc missing device ID\n");
				return CMD_RET_USAGE;
			}
			ret = strict_strtoul(argv[1], 0, &value);
			if (ret) {
				printf("Error parsing mmc device/bus number\n");
				return CMD_RET_USAGE;
			}
			desc.bus = value;
			pr_debug("MMC device: %lu\n", value);
			argv++;
			argc--;
			mmc = true;
			if (argc >= 2) {
				ret = strict_strtoul(argv[0], 16, &value);
				if (ret)
					continue;

				desc.image_addr = value;
				pr_debug("mmc image address: 0x%lx\n", value);

				ret = strict_strtoul(argv[1], 16, &value);
				if (ret) {
					printf("Error parsing mmc image size\n");
					return CMD_RET_USAGE;
				}
				pr_debug("mmc image size: 0x%lx\n", value);
				desc.image_size = value;
				argc -= 2;
				argv += 2;
				continue;
			}
		}
		if (!strcmp(argv[0], "spi")) {
			char *end;

			spi = true;
			argv++;
			argc--;
			desc.bus = 0;
			if (argc > 0) {
				desc.bus = simple_strtoul(argv[0], &end, 0);
				if (end && *end == ':')
					desc.cs = simple_strtoul(end + 1,
								 NULL, 0);
				else
					desc.cs = 0;
				pr_debug("SPI bus: 0x%x, cs: 0x%x\n",
					 desc.bus, desc.cs);
				argv++;
				argc--;
			}
			if (argc >= 2) {
				ret = strict_strtoul(argv[0], 16, &value);
				if (ret)
					continue;

				desc.image_addr = value;
				ret = strict_strtoul(argv[1], 16, &value);
				if (ret) {
					printf("Error parsing mmc image size\n");
					return CMD_RET_USAGE;
				}

				desc.image_size = value;
				pr_debug("SPI image address: 0x%llx, size: 0x%llx\n",
					 desc.image_addr, desc.image_size);
				argc -= 2;
				argv += 2;
			}
			continue;
		}
		printf("Unexpected argument %s\n", argv[0]);
		return CMD_RET_USAGE;
	}

	if (mmc && spi) {
		printf("Only specify mmc or spi, not both\n");
		return CMD_RET_USAGE;
	}
	if (!spi && !mmc) {
		printf("Error: either SPI or eMMC must be specified.\n");
		return CMD_RET_USAGE;
	}
	if (!mmc && (desc.update_flags & UPDATE_FLAG_ERASE_PART)) {
		printf("Error: -p may only be used with eMMC devices\n");
		return CMD_RET_USAGE;
	}
	if (desc.image_size == 0 || desc.image_addr == 0) {
		printf("Error: Image address and/or size cannot be zero\n");
		return CMD_RET_USAGE;
	}
	ret = smc_spi_update(&desc);

	if (ret) {
		printf("ERROR %d\n", ret);
		return CMD_RET_FAILURE;
	}
	printf("Bootloader update %s: %llu bytes\n", mmc ? "MMC" : "SPI",
	       desc.image_size);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
#if defined(CMD_BOOTIMGUP_CUST_SIG) && defined(CMD_BOOTIMGUP_BACKUP)
	bootimgup, 11, 1, do_bootimgup, "Updates Boot Image",
	" <[-v]> <[-b]> <[-p]> <mmc [devid] | spi [bus:cs]> [image_address] [image_size] -sig [signature address] [signature size]\n"
#elif defined(CMD_BOOTIMGUP_CUST_SIG) && !defined(CMD_BOOTIMGUP_BACKUP)
	bootimgup, 10, 1, do_bootimgup, "Updates Boot Image",
	" <[-v]> <[-p]> <mmc [devid] | spi [bus:cs]> [image_address] [image_size] -sig [signature address] [signature size]\n"
#elif !defined(CMD_BOOTIMGUP_CUST_SIG) && defined(CMD_BOOTIMGUP_BACKUP)
	bootimgup, 8, 1, do_bootimgup, "Updates Boot Image",
	" <[-v]> <[-b]> <[-p]> <mmc | spi> <[devid] | [bus:cs]> [image_address] [image_size]\n"
#else
	bootimgup, 7, 1, do_bootimgup, "Updates Boot Image",
	" <[-v]> <[-p]> <mmc | spi> <[devid] | [bus:cs]> [image_address] [image_size]\n"
#endif
#ifdef CMD_BOOTIMGUP_BACKUP
	" -b - updates the backup image location\n"
#endif
	" -v - skip version check\n"
	" -p - (MMC only) overwrite the partition table\n"
	" spi - updates boot image on spi flash\n"
	" bus and cs should be passed together, if missing, 0 is assumed.\n"
	" image_address - address at which image is located in RAM\n"
	" image_size    - size of image in hex\n"
	" eg. to load on spi0 chipselect 0\n"
	" bootimgup spi 0:0 $fileaddr $filesize\n"
	" eg. to load on spi1 chipselect 1\n"
	" bootimgup spi 1:1 $fileaddr $filesize\n"
	"\n"
	" mmc - updates boot image on mmc card/chip\n"
	" eg. to load on device 0\n"
	" bootimgup mmc 0 $fileaddr $filesize\n"
	" eg. to load on device 1. If device id not given, 0 is used\n"
	" bootimgup mmc 1 $fileaddr $filesize\n"
	" image_address - address at which image is located in RAM\n"
	" image_size    - size of image in hex\n"
	" image_address, image_size should be passed together,\n"
	" passing only one of them treated as invalid.\n"
	"\n"
	" If not given, then $fileaddr and $filesize values in\n"
	" environment are used, otherwise fail to update.\n"
#ifdef CMD_BOOTIMGUP_CUST_SIG
	" -sig [signature address] [signature size size]\n"
	"  Address and size of image signature\n"
	" If missing, the environment variables $sigaddr and $sigsize will be used.\n"
#endif
);
