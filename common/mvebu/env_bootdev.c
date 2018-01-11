/*
 * Copyright (C) 2017 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#include <common.h>
#include <environment.h>
#include <env_bootdev.h>
#include <mvebu/mvebu_chip_sar.h>

DECLARE_GLOBAL_DATA_PTR;

char *env_name_spec = "Undefined";

int saveenv(void)
{
	return gd->arch.env_func.save_env();
}

/* Initial support for SPI only,
 * other API's will be supported in the future.
 * return values: 0 pass, -1 fail.
 */
int env_init(void)
{
	struct sar_val sar;

	mvebu_sar_value_get(SAR_BOOT_SRC, &sar);

	switch (sar.bootsrc.type) {
	case BOOTSRC_NAND:
#ifdef CONFIG_NAND_PXA3XX
		nand_env_init();
		break;
#endif
	case BOOTSRC_SPI:
	case BOOTSRC_AP_SPI:
#ifdef CONFIG_SPI_FLASH
		sf_env_init();
#endif
#ifdef CONFIG_SPI_NAND
		spi_nand_env_init();
#endif
		break;
	default:
		error("Sample at reset boot source type %x is not supported\n",
		      sar.bootsrc.type);
		return -1;
	}

	/* if pointer initialized & routine returned 0 (successfully) */
	if (gd->arch.env_func.init_env && !(gd->arch.env_func.init_env()))
		return 0;

	error("Failed to initialize environment variables.\n");
	return -1;
}

void env_relocate_spec(void)
{
	struct sar_val sar;

	mvebu_sar_value_get(SAR_BOOT_SRC, &sar);
	/* env_init is called again here because of the
	 * address relocation, the addreses need to be corrected
	 */
	switch (sar.bootsrc.type) {
	case BOOTSRC_NAND:
#ifdef CONFIG_NAND_PXA3XX
		nand_env_init();
		env_name_spec = "NAND Flash";
#endif
		break;
	case BOOTSRC_SPI:
	case BOOTSRC_AP_SPI:
#ifdef CONFIG_SPI_FLASH
		sf_env_init();
		env_name_spec = "SPI Flash";
#endif
#ifdef CONFIG_SPI_NAND
		spi_nand_env_init();
		env_name_spec = "SPI NAND Flash";
#endif
		break;
	default:
		error("Sample at reset boot source type %x is not supported\n",
		      sar.bootsrc.type);
		return;
	}

	if (gd->arch.env_func.reloc_env)
		gd->arch.env_func.reloc_env();
	else
		error("Failed to initialize environment variables.\n");
}

