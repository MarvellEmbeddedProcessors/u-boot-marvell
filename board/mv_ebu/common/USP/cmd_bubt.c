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

#if 0 /* def MV_NOR_BOOT */
static unsigned int flash_in_which_sec(flash_info_t *fl,unsigned int offset)
{
	unsigned int sec_num;
	if(NULL == fl)
		return 0xFFFFFFFF;

	for( sec_num = 0; sec_num < fl->sector_count ; sec_num++){
		/* If last sector*/
		if (sec_num == fl->sector_count -1)
		{
			if((offset >= fl->start[sec_num]) &&
			   (offset <= (fl->size + fl->start[0] - 1)) )
			{
				return sec_num;
			}

		}
		else
		{
			if((offset >= fl->start[sec_num]) &&
			   (offset < fl->start[sec_num + 1]) )
			{
				return sec_num;
			}

		}
	}
	/* return illegal sector Number */
	return 0xFFFFFFFF;

}
#endif /* !defined(MV_NOR_BOOT) */

#if defined(MV_NAND_BOOT)
/* Boot from NAND flash */
/* Write u-boot image into the nand flash */
int nand_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	nand_info_t *nand = &nand_info[0];
	size_t blocksize = nand_info[0].erasesize;
	size_t env_offset = CONFIG_ENV_OFFSET;
	size_t size = CONFIG_UBOOT_SIZE;
	size_t offset = 0;

	/* Align U-Boot size to currently used blocksize */
	size = ((size + (blocksize - 1)) & (~(blocksize-1)));

#if defined(CONFIG_SKIP_BAD_BLOCK)
	int i = 0;
	int sum = 0;

	while(i * blocksize < nand_info[0].size) {
		if (!nand_block_isbad(&nand_info[0], (i * blocksize)))
			sum += blocksize;
		else {
			sum = 0;
			offset = (i + 1) * blocksize;
		}

		if (sum >= size)
			break;
		i++;
	}
#endif
	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else {
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}
	filesize = NetLoop(TFTPGET);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t[Fail]\n");
		return 0;
	}
	printf("\t[Done]\n");
	printf("Override Env parameters to default? [y/N]");
	readline(" ");
	if( strcmp(console_buffer,"Y") == 0 ||
	    strcmp(console_buffer,"yes") == 0 ||
	    strcmp(console_buffer,"y") == 0 ) {

		printf("Erasing 0x%x - 0x%x:",env_offset, CONFIG_ENV_RANGE);
		nand_erase(nand, env_offset, CONFIG_ENV_RANGE);
		printf("\t[Done]\n");
	}

	printf("Erasing 0x%x - 0x%x: ", offset, offset + size);
	nand_erase(nand, offset, size);
	printf("\t[Done]\n");

	printf("Writing image to NAND:");
	ret = nand_write(nand, offset, &size, (u_char *)load_addr);
	if (ret)
		printf("\t[Fail]\n");
	else
		printf("\t[Done]\n");

	return 1;
}

U_BOOT_CMD(
        bubt,      2,     1,      nand_burn_uboot_cmd,
        "bubt	- Burn an image on the Boot Nand Flash.\n",
        " file-name \n"
        "\tBurn a binary image on the Boot Nand Flash, default file-name is u-boot.bin .\n"
);
#endif /* defined(CONFIG_NAND_BOOT) */

#if defined(MV_SPI_BOOT)

/* Boot from SPI flash */
/* Write u-boot image into the SPI flash */
int spi_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	load_addr = CONFIG_SYS_LOAD_ADDR;

	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else {
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}

	if(!flash) {
		flash = spi_flash_probe(0, 0, CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
		if (!flash) {
			printf("Failed to probe SPI Flash\n");
			set_default_env("!spi_flash_probe() failed");
			return 0;
		}
	}

	filesize = NetLoop(TFTPGET);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t\t[ERR!]\n");
		return 0;
	}
	printf("\t\t[Done]\n");
	printf("Override Env parameters to default? [y/N]");
	readline(" ");

#ifdef CONFIG_SPI_FLASH_PROTECTION
	printf("Unprotecting flash:");
	spi_flash_protect(flash, 0);
	printf("\t\t[Done]\n");
#endif
	if( strcmp(console_buffer,"Y") == 0 ||
	    strcmp(console_buffer,"yes") == 0 ||
	    strcmp(console_buffer,"y") == 0 ) {

		printf("Erasing 0x%x - 0x%x:",CONFIG_ENV_OFFSET, CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE);
		spi_flash_erase(flash, CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE);
		printf("\t[Done]\n");
	}
	if (filesize > CONFIG_ENV_OFFSET)
	{
		printf("Error Image size(%x) bigger than CONFIG_ENV_OFFSET=%x ",filesize,CONFIG_ENV_OFFSET);
		return 0;
	}
	printf("Erasing 0x%x - 0x%x: ",0, 0 + CONFIG_ENV_OFFSET);
	spi_flash_erase(flash, 0, CONFIG_ENV_OFFSET);
	printf("\t\t[Done]\n");

	printf("Writing image to flash:");
	ret = spi_flash_write(flash, 0, filesize, (const void *)load_addr);

	if (ret)
		printf("\t\t[Err!]\n");
	else
		printf("\t\t[Done]\n");

#ifdef CONFIG_SPI_FLASH_PROTECTION
	printf("Protecting flash:");
	spi_flash_protect(flash, 1);
	printf("\t\t[Done]\n");
#endif
	return 1;
}

U_BOOT_CMD(
        bubt,      2,     1,      spi_burn_uboot_cmd,
        "bubt	- Burn an image on the Boot SPI Flash.\n",
        " file-name \n"
        "\tBurn a binary image on the Boot SPI Flash, default file-name is u-boot.bin .\n"
);
#endif


#if defined(MV_NOR_BOOT)

/* Boot from Nor flash */
int nor_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	load_addr = CONFIG_SYS_LOAD_ADDR;
//	MV_U32 s_first,s_end,env_sec;

	ulong stop_addr;
	ulong start_addr;

//	s_first = flash_in_which_sec(&flash_info[0], CONFIG_SYS_MONITOR_BASE);
//	s_end = flash_in_which_sec(&flash_info[0], CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN -1);
//	env_sec = flash_in_which_sec(&flash_info[0], CONFIG_ENV_ADDR);

	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else {
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}

	filesize = NetLoop(TFTPGET);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t[ERR!]\n");
		return 0;
	}
	printf("\t[Done]\n");
	printf("Override Env parameters to default? [y/N]");
	readline(" ");
	if( strcmp(console_buffer,"Y") == 0 ||
		   strcmp(console_buffer,"yes") == 0 ||
		   strcmp(console_buffer,"y") == 0 ) {

		start_addr = CONFIG_ENV_ADDR;
		stop_addr = start_addr + CONFIG_ENV_SIZE - 1;

		printf("Erasing sector 0x%x:",CONFIG_ENV_OFFSET);
		flash_sect_protect (0, start_addr, stop_addr);

		flash_sect_erase (start_addr, stop_addr);

		flash_sect_protect (1, start_addr, stop_addr);
		printf("\t[Done]\n");
	}

	start_addr = NOR_CS_BASE;
	stop_addr = start_addr + CONFIG_ENV_OFFSET - 1;

	flash_sect_protect (0, start_addr, stop_addr);

	printf("Erasing 0x%x - 0x%x: ", (unsigned int)start_addr, (unsigned int)(start_addr + CONFIG_ENV_OFFSET));
	flash_sect_erase (start_addr, stop_addr);
	printf("\t[Done]\n");

	printf("Writing image to NOR:");
	ret = flash_write((char *)CONFIG_SYS_LOAD_ADDR, start_addr, filesize);

	if (ret)
	   printf("\t[Err!]\n");
	else
	   printf("\t[Done]\n");

	flash_sect_protect (1, start_addr, stop_addr);
	return 1;
}

U_BOOT_CMD(
		   bubt,      2,     1,      nor_burn_uboot_cmd,
	 "bubt	- Burn an image on the Boot Flash.\n",
  " file-name \n"
		  "\tBurn a binary image on the Boot Flash, default file-name is u-boot.bin .\n"
		  );
#endif /* MV_NOR_BOOT */
