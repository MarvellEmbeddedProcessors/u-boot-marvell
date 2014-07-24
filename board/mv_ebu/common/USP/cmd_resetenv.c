/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.

*******************************************************************************/

#include <config.h>
#include <common.h>
#include "mvCommon.h"
#include <command.h>
#include <net.h>
#include <environment.h>

#if defined(CONFIG_CMD_NAND)
#include <nand.h>
extern nand_info_t nand_info[];       /* info for NAND chips */
#endif

#ifdef CONFIG_CMD_SF
#include <spi_flash.h>
extern struct spi_flash *flash;
#endif


#ifdef CONFIG_CMD_FLASH
#include <flash.h>
extern flash_info_t flash_info[];       /* info for FLASH chips */
#endif
#if defined(CONFIG_ENV_IS_IN_NAND)
int nand_get_env_offs(void);
#endif
/*******************************************************************************
Reset environment variables.
********************************************************************************/
int resetenv_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#if defined(CONFIG_ENV_IS_IN_FLASH )
        ulong stop_addr;
	ulong start_addr;

#endif

#if defined(CONFIG_ENV_IS_IN_NAND)
	size_t env_offset = CONFIG_ENV_OFFSET;
	nand_info_t *nand = &nand_info[0];

	printf("Erasing 0x%x - 0x%x:",env_offset, env_offset + CONFIG_ENV_RANGE);
	nand_erase(nand, env_offset, CONFIG_ENV_RANGE);
	puts ("[Done]\n");

#elif defined(CONFIG_ENV_IS_IN_SPI_FLASH)
	u32 sector = 1;

	if (CONFIG_ENV_SIZE > CONFIG_ENV_SECT_SIZE) {
		sector = CONFIG_ENV_SIZE / CONFIG_ENV_SECT_SIZE;
		if (CONFIG_ENV_SIZE % CONFIG_ENV_SECT_SIZE)
			sector++;
	}

#ifdef CONFIG_SPI_FLASH_PROTECTION
	printf("Unprotecting flash:");
	spi_flash_protect(flash, 0);
	printf("\t\t[Done]\n");
#endif

	printf("Erasing 0x%x - 0x%x:",CONFIG_ENV_OFFSET, CONFIG_ENV_OFFSET + sector * CONFIG_ENV_SECT_SIZE);
	if(!flash) {
		flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS,
								CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
		if (!flash) {
			printf("Failed to probe SPI Flash\n");
			set_default_env("!spi_flash_probe() failed");
			return 0;
		}
	}
	if (spi_flash_erase(flash, CONFIG_ENV_OFFSET, sector * CONFIG_ENV_SECT_SIZE))
		return 1;
	puts("\t[Done]\n");

#ifdef CONFIG_SPI_FLASH_PROTECTION
	printf("Protecting flash:");
	spi_flash_protect(flash, 1);
	printf("\t\t[Done]\n");
#endif

#elif defined(CONFIG_ENV_IS_IN_FLASH )
	start_addr = CONFIG_ENV_ADDR;
	stop_addr = start_addr + CONFIG_ENV_SIZE - 1;

	printf("Erasing sector 0x%x:",CONFIG_ENV_OFFSET);
	flash_sect_protect (0, start_addr, stop_addr);

	flash_sect_erase (start_addr, stop_addr);

	flash_sect_protect (1, start_addr, stop_addr);
	printf("\t[Done]\n");

#endif
	printf("Warning: Default Environment Variables will take effect Only after RESET\n");
	return 0;
}

U_BOOT_CMD(
        resetenv,      1,     1,      resetenv_cmd,
        "resetenv	- Erase environment sector to reset all variables to default.\n",
        " \n"
        "\t Erase the environemnt variable sector.\n"
);
