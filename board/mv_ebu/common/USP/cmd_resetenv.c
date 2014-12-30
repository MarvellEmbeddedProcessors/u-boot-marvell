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
#if defined(CONFIG_ENV_IS_IN_MMC)
#include <mmc.h>
extern unsigned long mmc_berase(int dev_num, lbaint_t start, lbaint_t blkcnt);
extern ulong mmc_bwrite(int dev_num, lbaint_t start, lbaint_t blkcnt, const void *src);
#endif

/*******************************************************************************
Reset environment variables.
********************************************************************************/
int resetenv_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#if defined(CONFIG_ENV_IS_IN_FLASH )
	ulong	stop_addr;
	ulong	start_addr;
#elif defined(CONFIG_ENV_IS_IN_MMC)
	lbaint_t	start_lba;
	lbaint_t	blk_count;
	ulong		blk_erased;
	ALLOC_CACHE_ALIGN_BUFFER(char, buf, CONFIG_ENV_SIZE);
	struct mmc	*mmc;
	int			err;
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

#elif defined(CONFIG_ENV_IS_IN_MMC)

	start_lba = CONFIG_ENV_ADDR / CONFIG_ENV_SECT_SIZE;
	blk_count = CONFIG_ENV_SIZE / CONFIG_ENV_SECT_SIZE;

	mmc = find_mmc_device(CONFIG_SYS_MMC_ENV_DEV);
	if (!mmc) {
		printf("No MMC card found\n");
		return 1;
	}

	if (mmc_init(mmc)) {
		printf("MMC(%d) init failed\n", CONFIG_SYS_MMC_ENV_DEV);
		return 1;
	}

#ifdef CONFIG_SYS_MMC_ENV_PART /* Valid for MMC/eMMC only - switch to ENV partition */
	if (CONFIG_SYS_MMC_ENV_PART != mmc->part_num) {
		if (mmc_switch_part(CONFIG_SYS_MMC_ENV_DEV, CONFIG_SYS_MMC_ENV_PART)) {
			printf("MMC partition switch failed\n");
			return 1;
		}
	}
#endif

	printf("Erasing 0x"LBAF" blocks starting at sector 0x"LBAF" :", blk_count, start_lba);
	/* For some unknown reason the mmc_berase() fails with timeout if called
	   before any futher write to MMC/SD (for instance, right after u-boot bring up).
	   However the mmc_bwrite() always succeds.
	   Writing zeroes into SD/MMC blocks is similar operation as doing so to IDE/SATA
	   disk and therefore can be used for erasing the imformation stored on the media.
	   blk_erased = mmc_berase(CONFIG_SYS_MMC_ENV_DEV, start_lba, blk_count);
	*/
	memset(buf, 0, CONFIG_ENV_SIZE);
	blk_erased = mmc_bwrite(CONFIG_SYS_MMC_ENV_DEV, start_lba, blk_count, buf);
	if (blk_erased != blk_count) {
		printf("\t[FAIL] - erased %#lx blocks\n", blk_erased);
		err = 1;
	} else {
		printf("\t[Done]\n");
		err = 0;
	}

#ifdef CONFIG_SYS_MMC_ENV_PART /* Valid for MMC/eMMC only - restore current partition */
	if (CONFIG_SYS_MMC_ENV_PART != mmc->part_num)
		mmc_switch_part(CONFIG_SYS_MMC_ENV_DEV, mmc->part_num);
#endif

	if (err)
		return err;

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
