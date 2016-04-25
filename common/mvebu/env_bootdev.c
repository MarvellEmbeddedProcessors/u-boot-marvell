/*
 * ***************************************************************************
 * Copyright (C) 2015 Marvell International Ltd.
 * ***************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************
 */
/* #define DEBUG */
#include <common.h>
#include <environment.h>
#include <mvebu_chip_sar.h>
#include <asm/arch-mvebu/mvebu.h>

DECLARE_GLOBAL_DATA_PTR;

char *env_name_spec = "Undefined";

int saveenv(void)
{
	return gd->arch.env_func.save_env();
}

/* this has an initial support for SPI only,
 * other API's will be supported in the future.
 */
int env_init(void)
{
	struct sar_val sar;

	debug_enter();
	mvebu_sar_value_get(SAR_BOOT_SRC, &sar);
	/* Use ifdef as a temporary solution till we add sample-at-reset
	** logic. */
	switch (sar.bootsrc.type) {
	case BOOTSRC_NAND:
		nand_env_init();
		break;
	case BOOTSRC_SPI:
	case BOOTSRC_AP_SPI:
		sf_env_init();
		break;
	case BOOTSRC_AP_SD_EMMC:
	case BOOTSRC_SD_EMMC:
		mmc_boot_env_init();
		break;
	default:
		error("Sample at reset boot source type %x is not supported\n", sar.bootsrc.type);
		return 0;
	}

	debug_exit();

	/* if pointer initialized & routine returned 0 (successfully) */
	if (gd->arch.env_func.init_env && !(gd->arch.env_func.init_env()))
			return 0;

	error("Failed to initialize environment variables.\n");
	return 0;
}

void env_relocate_spec(void)
{
	struct sar_val sar;

	debug_enter();
	mvebu_sar_value_get(SAR_BOOT_SRC, &sar);
	/* env_init is called again here because of the
	 * address relocation, the addreses need to be corrected
	 */
	/* Use ifdef as a temporary solution till we add sample-at-reset
	** logic. */
	switch (sar.bootsrc.type) {
	case BOOTSRC_NAND:
		nand_env_init();
		env_name_spec = "NAND Flash";
		break;
	case BOOTSRC_SPI:
	case BOOTSRC_AP_SPI:
		sf_env_init();
		env_name_spec = "SPI Flash";
		break;
	case BOOTSRC_AP_SD_EMMC:
	case BOOTSRC_SD_EMMC:
		mmc_boot_env_init();
		env_name_spec = "SD/MMC Flash";
		break;
	default:
		error("Sample at reset boot source type %x is not supported\n", sar.bootsrc.type);
		return;
	}

	debug_exit();

	if (gd->arch.env_func.reloc_env)
		gd->arch.env_func.reloc_env();
	else
		error("Failed to initialize environment variables.\n");
}

