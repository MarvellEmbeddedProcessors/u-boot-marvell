/*******************************************************************************
   Copyright (C) Marvell International Ltd. and its affiliates

   This software file (the "File") is owned and distributed by Marvell
   International Ltd. and/or its affiliates ("Marvell") under the following
   alternative licensing terms.  Once you have made an election to distribute the
   File under one of the following license alternatives, please (i) delete this
   introductory statement regarding license alternatives, (ii) delete the two
   license alternatives that you have not elected to use and (iii) preserve the
   Marvell copyright notice above.

********************************************************************************
   Marvell Commercial License Option

   If you received this File from Marvell and you have entered into a commercial
   license agreement (a "Commercial License") with Marvell, the File is licensed
   to you under the terms of the applicable Commercial License.

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
********************************************************************************
   Marvell BSD License Option

   If you received this File from Marvell, you may opt to use, redistribute and/or
   modify this File under the following licensing terms.
   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

*   Redistributions of source code must retain the above copyright notice,
            this list of conditions and the following disclaimer.

*   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

*   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include <common.h>
#include <config.h>
#include <common.h>
#include <command.h>
#include <pci.h>
#include <net.h>
#include <spi_flash.h>
#include <bzlib.h>
#include <fs.h>
#ifdef MV_INCLUDE_USB
#include <usb.h>
#endif
#ifdef CONFIG_MMC
#include <mmc.h>
#endif

#include "mvCommon.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "cpu/mvCpu.h"

#if defined(MV_INC_BOARD_NOR_FLASH)
		#include "norflash/mvFlash.h"
#endif

#if defined(MV_INCLUDE_GIG_ETH)
		#include "eth-phy/mvEthPhy.h"
#endif

#if defined(MV_INCLUDE_PEX)
		#include "pex/mvPex.h"
#endif

#if defined(MV_INCLUDE_PDMA)
		#include "pdma/mvPdma.h"
		#include "mvSysPdmaApi.h"
#endif

#if defined(MV_INCLUDE_XOR)
		#include "xor/mvXorRegs.h"
		#include "xor/mvXor.h"
#endif

#if defined(MV_INCLUDE_PMU)
		#include "pmu/mvPmuRegs.h"
#endif

#include "cntmr/mvCntmrRegs.h"
#include "switchingServices.h"

MV_BOOL mvVerifyRequest(void)
{
	printf("\nDo you want to continue ? [Y/n]");
	readline(" ");
	if(strlen(console_buffer) == 0 || /* if pressed Enter */
		strcmp(console_buffer,"y") == 0 ||
		strcmp(console_buffer,"Yes") == 0 ||
		strcmp(console_buffer,"Y") == 0 ) {
		printf("\n");
		return MV_TRUE;
	}

	return MV_FALSE;
}

int mvLoadFile4cpss(int loadfrom, const char* file_name, char * devPart, int fstype)
{
	MV_U32 filesize;
	char interface[4] = "mmc";

	switch(loadfrom) {
	case 0:
		if ((filesize = NetLoop(TFTPGET)) < 0)
			return 0;
		return filesize;;
	case 1:
#ifdef MV_INCLUDE_USB
		usb_stop();
		printf("(Re)start USB...\n");

		if (usb_init() < 0) {
			printf("usb_init failed\n");
			return 0;
		}

		/* try to recognize storage devices immediately */
		if (-1 == usb_stor_scan(1)) {
			printf("USB storage device not found\n");
			return 0;
		}
		strcpy(interface, "usb");

		break;
#else
		printf("USB is not supported\n");
		return 0;
#endif
	case 2:
#ifdef CONFIG_MMC
		run_command("mmc rescan", 0);
		break;
#else
		printf("MMC is not supported\n");
		return 0;
#endif
	}


	/* always load from usb 0 */
	if (fs_set_blk_dev(interface, devPart, fstype))
	{
		printf("%s %s not found\n", interface, (devPart != NULL) ? devPart: "0");
		return 0;
	}

	return (fs_read(file_name, load_addr, 0, 0));
}


/*******************************************************************************
* mv_print_appl_partitions - Print u-boot partitions on SPI flash
*******************************************************************************/
static int do_print_flash_part ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[] ) {
#ifdef MV_INCLUDE_SPI
		run_command("sf probe 0", 0);
		printf("\n");
		printf("%s partitions on spi flash\n", CFG_APPL_SPI_FLASH_PARTITIONS_NAME);
		printf("--------------------------------------\n\n");
		printf("Spi flash Address (rx)   : 0x%08x\n", CFG_APPL_SPI_FLASH_START_DIRECT);
		printf("Spi flash size           : %dMB\n", CFG_APPL_SPI_FLASH_SIZE/(1<<20));
		printf("u-boot               : offset=0x%08x, size=0x%08x (%dMB)\n",
			   CFG_APPL_FLASH_PART_UBOOT_START,
			   CFG_APPL_FLASH_PART_UBOOT_SIZE,
			   CFG_APPL_FLASH_PART_UBOOT_SIZE/(_1M));

		printf("kernel/vxWorks-image : offset=0x%08x, size=0x%08x (%dMB)\n",
			   CFG_APPL_SPI_FLASH_PART_KERNEL_START,
			   CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE,
			   CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE/(_1M));

		printf("Linux rootfs         : offset=0x%08x, size=0x%08x (%dMB)\n",
			   CFG_APPL_SPI_FLASH_PART_ROOTFS_START,
			   CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE,
			   CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE/(_1M));

		printf("\n");
		printf("Single Image         : offset=0x%08x, size=0x%08x (%dMB)\n",
			   CFG_APPL_SPI_FLASH_PART_KERNEL_START,
			   CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE + CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE,
			   (CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE + CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE)/(_1M));
#endif /* MV_INCLUDE_SPI */

#ifdef MV_NAND
		run_command("nand info", 0);
		printf("\n");
		printf("%s partitions on nand flash\n", CFG_APPL_SPI_FLASH_PARTITIONS_NAME);
		printf("--------------------------------------\n\n");
		printf("Nand flash size          : %dMB\n", CFG_APPL_NAND_FLASH_SIZE/(1<<20));
		printf("kernel/vxWorks-image : offset=0x%08x, size=0x%08x (%dMB)\n",
			   CFG_APPL_NAND_FLASH_PART_KERNEL_START,
			   CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE,
			   CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE/(_1M));

		printf("Linux rootfs         : offset=0x%08x, size=0x%08x (%dMB)\n",
			   CFG_APPL_NAND_FLASH_PART_ROOTFS_START,
			   CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE,
			   CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE/(_1M));

		printf("\n");
		printf("Single Image         : offset=0x%08x, size=0x%08x (%dMB)\n",
			   CFG_APPL_NAND_FLASH_PART_KERNEL_START,
			   CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE,
			   (CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE)/(_1M));
#endif
		printf("\n");
		return 1;
}

U_BOOT_CMD(
	flash_part_print,    2,    1,    do_print_flash_part,
	"flash_part_print  - print spi/nand FLASH memory information\n",
	"\n    - print information for all SPI FLASH memory banks\n"
);

/*******************************************************************************
* do_cpss_env - Save CPSS enviroment on flash
*
* DESCRIPTION:
*
* INPUT:  None
*
* OUTPUT: None,
*
* RETURN:
*	None
*
*******************************************************************************/
static int do_cpss_env( cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[] )
{
	char buf[1024];

	printf("Saving cpss environment variable\n");
	setenv("standalone", "");
	setenv("bootcmd", "run standalone_mtd");
	setenv("consoledev","ttyS0");
	setenv("linux_loadaddr","0x2000000");
	setenv("netdev","eth0");
	setenv("rootpath","/tftpboot/rootfs_arm-mv7sft");
	setenv("othbootargs","null=null");

	setenv("nfsboot","setenv bootargs root=/dev/nfs rw nfsroot=${serverip}:${rootpath} "
					 "ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:${netdev}:off "
					 "console=${consoledev},${baudrate} ${othbootargs} ${linux_parts}; tftp ${linux_loadaddr} "
					 "${image_name};bootm ${linux_loadaddr}");

	sprintf(buf,"'spi_flash:%dm(spi_uboot)ro,%dm(spi_kernel),%dm(spi_rootfs),-(remainder)"
		";armada-nand:%dm(nand_kernel),-(nand_rootfs)'", CFG_APPL_FLASH_PART_UBOOT_SIZE / _1M,
		CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE / _1M, CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE / _1M,
		CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE / _1M);

	setenv("mtdparts", buf);
	printf("mtdparts = %s\n", buf);
	setenv("linux_parts", buf);

#ifdef MV_INCLUDE_SPI
	sprintf(buf,
		"sf probe; sf read ${loadaddr} 0x%x 0x%x; setenv bootargs ${console} "
		"root=/dev/mtdblock2 rw init=/linuxrc rootfstype=jffs2 rootwait mtdparts=${mtdparts} "
		"${mvNetConfig}; bootm ${loadaddr} ",
		CFG_APPL_SPI_FLASH_PART_KERNEL_START, CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE);
#ifndef MV_NAND
	setenv("standalone_mtd", buf);
	printf("standalone_mtd = %s\n", buf);
#else
	setenv("standalone_mtd_spi", buf);
	printf("standalone_mtd_spi = %s\n", buf);
#endif
#endif /* MV_INCLUDE_SPI */

#ifdef MV_NAND
	sprintf(buf,
		"nand read ${loadaddr} 0x%x 0x%x; setenv bootargs ${console} mtdparts=${mtdparts} "
		"ubi.mtd=5 root=ubi0:rootfs_nand ro rootfstype=ubifs ${mvNetConfig}; bootm 0x2000000;" ,
		CFG_APPL_NAND_FLASH_PART_KERNEL_START,
		CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE);

	setenv("standalone_mtd", buf);
	printf("standalone_mtd = %s\n", buf);
#endif
	run_command("saveenv", 0);
	printf("\nboot command prepared: run 'boot' to load kernel and file system from flash.\n");
	return 1;
}

U_BOOT_CMD(
	cpss_env,      2,     1,      do_cpss_env,
	"set cpss environment variables permanently\n",
	""
);

extern struct spi_flash *flash;

struct partitionInformation nandInfo = {
	.defaultImage = "ubifs_arm_nand.image",
	.FLASH_SIZE = CFG_APPL_NAND_FLASH_SIZE,
	.KERNEL_SIZE = CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE,
	.ROOTFS_SIZE = CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE,
	.KERNEL_START = CFG_APPL_NAND_FLASH_PART_KERNEL_START,
	.BLOCK_ALIMENT = CFG_APPL_NAND_FLASH_BLOCK_ALIMENT,
	.ROOTFS_START = CFG_APPL_NAND_FLASH_PART_ROOTFS_START,
};

struct partitionInformation spiInfo = {
	.defaultImage = "jffs2_arm.image",
	.FLASH_SIZE = CFG_APPL_SPI_FLASH_SIZE,
	.KERNEL_SIZE = CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE,
	.ROOTFS_SIZE = CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE,
	.KERNEL_START = CFG_APPL_SPI_FLASH_PART_KERNEL_START,
	.BLOCK_ALIMENT = CFG_APPL_SPI_FLASH_BLOCK_ALIMENT,
	.ROOTFS_START = CFG_APPL_SPI_FLASH_PART_ROOTFS_START,
};


void flashWrite (MV_U32 destination, MV_U32 len, MV_U32 source, MV_BOOL isNand)
{
	char cmdBuf[128];

	if (isNand == MV_TRUE) {
		sprintf(cmdBuf, "nand write.trimffs %x %x %x\n", source, destination, len);
		printf(cmdBuf);
		run_command(cmdBuf, 0);
	} else
		spi_flash_write(flash, destination, len, (const void *)source);
}

void flashErase (MV_U32 destination, MV_U32 len, MV_BOOL isNand)
{
	char cmdBuf[128];
	if (isNand == MV_TRUE) {
		sprintf(cmdBuf, "nand erase %x %x\n", destination, len);
		printf(cmdBuf);
		run_command(cmdBuf, 0);
	} else
		spi_flash_erase(flash, destination, len);

}

static int do_mtdburn(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 i, filesize, addr, src_addr, dest_addr;
	MV_U32 kernel_unc_len, rootfs_unc_len = 0, unc_len, src_len;
	MV_U32 kernel_addr = 0x6000000, rootfs_addr = 0x7000000;
	MV_U32 total_in, rc, single_file = 0, bz2_file = 0;
	char *from[] = {"tftp","usb","mmc", "ram"};
	struct partitionInformation *partitionInfo = &nandInfo;	/* default destination = NAND */
	MV_U32 loadfrom = 0;					/* Default source = tftp */
	char * devPart = NULL;
	MV_U32 fsys = FS_TYPE_FAT;				/* default FS = FAT */
	MV_BOOL isNand = MV_TRUE;				/* default destination = NAND */
	addr = load_addr = 0x2000000;
	int fileSizeFromRam = -1;

	/* scan for flash destination in arguments (allowing usage of only 'mtdburn spi') */
	for (i = 1 ; i < argc ; i++) {
		if ((strcmp(argv[i], "spi") == 0) || (strcmp(argv[i], "SPI") == 0)) {
			isNand = MV_FALSE;
			partitionInfo = &spiInfo;
			argc--; /* disregard 'spi' for next steps */
		}
		if ((strcmp(argv[i], "nand") == 0) || (strcmp(argv[i], "NAND") == 0))
			argc--; /* disregard 'nand' for next steps */
	}

	switch(argc) {
	case 7:
		fileSizeFromRam = simple_strtoul(argv[6], NULL, 16);
	case 6:/* arg#6 is flash destination, scanned previously --> fall to 5*/
	case 5:
		copy_filename (BootFile, argv[4], sizeof(BootFile));
		/* fall to 4*/
	case 4:
		if(strcmp(argv[3], "EXT2") == 0)
			fsys = FS_TYPE_EXT;
		/* fall to 3*/
	case 3:
		devPart = argv[2];
		/* fall to 2*/
	case 2:
		if(strcmp(argv[1], "usb") == 0)
			loadfrom = 1;
		else if(strcmp(argv[1], "mmc") == 0)
			loadfrom = 2;
		else if(strcmp(argv[1], "ram") == 0) {
			loadfrom = 3;
			if (devPart == NULL)
				devPart = "2000000"; /*if source location not specified, use default load_addr */
			else
				addr = load_addr = (unsigned int)simple_strtoul(devPart, NULL, 16);
		}
		if ((loadfrom == 1 || loadfrom == 2) && devPart == NULL) /* if using USB/MMC, and not selected interface num */
			devPart = "0";					 /* default interface number is 0 */
		/* fall to 1*/
	case 1:    /* no parameter all default */
		if(argc < 4)
			copy_filename (BootFile, partitionInfo->defaultImage, sizeof(BootFile));
		break;
	default:
		return 0;
	}

	printf(" - Load from device \t: %s", from[loadfrom]);
	if (devPart != NULL) {
		if (loadfrom == 3)	/* source = DRAM */
			printf(", load from :%s" ,devPart);
		else {			/* source = USB/MMC */
			printf(", Interface :%s" ,devPart);
			if (fsys == FS_TYPE_FAT)
				printf("\n - File System \t\t: FAT");
			else if (fsys == FS_TYPE_EXT)
				printf("\n - File System \t\t: EXT2");
		}
	}
	printf("\n - Filename\t\t: %s \n" ,BootFile);
	printf(" - Flash destination\t: %s\n" , isNand == MV_TRUE ? "NAND" : "SPI");

	if (mvVerifyRequest() == MV_FALSE)
		return 0;

	if (isNand == MV_FALSE && !flash) {
		flash = spi_flash_probe(0, 0, CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
		if (!flash) {
			printf("Failed to probe SPI Flash\n");
			return 0;
		}
	}

	/* Fetch requested file, filesize is needed for single image only */
	if (loadfrom < 3) /* if Source is not RAM, fetch file (if source = RAM, file should be ready */
		filesize = mvLoadFile4cpss(loadfrom, BootFile, devPart, fsys);
	else if (fileSizeFromRam != -1) /* if specified a specific file size for single image file on ram */
		filesize = fileSizeFromRam;
	else 	/* if filesize not specified, try maximum limit for single image */
		filesize = partitionInfo->KERNEL_SIZE + partitionInfo->ROOTFS_SIZE;

	if(filesize <=0 )
		return 0;

	if (filesize > partitionInfo->FLASH_SIZE) {
		printf("file too big\n");
		return 0;
	}

	printf("\nTrying separation of kernel/vxWorks-image and root_fs. Work areas=0x%08x,0x%08x\n",
	       kernel_addr, rootfs_addr);

	dest_addr = kernel_addr; // uncompress the kernel here.
	src_addr = addr;
	src_len = unc_len = kernel_addr - addr;

	rc = BZ2_bzBuffToBuffDecompress_extended ((char*)dest_addr, &unc_len,
						  (char *)src_addr, src_len,
						  &total_in, 0, 0);
	printf("\n");
	kernel_unc_len = unc_len;

	if (rc == 0) {
		printf("kernel separation ended ok. unc_len=%d, total_in=%d\n",unc_len, total_in);
		bz2_file++;
	} else if (rc == -5) {
		printf("Not a valid bz2 file, assume plain single image file ?");
		if (mvVerifyRequest() == MV_FALSE)
			return 0;

		single_file = 1;
		kernel_unc_len = filesize;
		kernel_addr = load_addr;
	} else {
		printf("Uncompress of kernel ended with error. rc=%d\n", rc);
		return 0;
	}

	if (!single_file) {
	/* now try to separate the rootfs. If we get -5 then we have a single file. */
		dest_addr = rootfs_addr; // uncompress the rootfs here.
		src_addr += total_in;
		src_len = unc_len = kernel_addr - addr - total_in;

		rc = BZ2_bzBuffToBuffDecompress_extended ((char*)dest_addr, &unc_len, (char *)src_addr,
							  src_len, &total_in, 0, 0);
		printf("\n");
		rootfs_unc_len = unc_len;
		if (rc == 0) {
			bz2_file++;

			printf("rootfs separation ended ok. unc_len=%d, total_in=%d\n", unc_len, total_in);
			if (unc_len > partitionInfo->ROOTFS_SIZE) {
				printf("rootfs too big\n");
				return 0;
			}
		} else if (rc == -5) {
			printf("One single bz2 file detected\n");
			single_file = 1;
		} else {
			printf("Uncompress of rootfs ended with error. rc=%d\n", rc);
			return 0;
		}
	}

	if (!single_file && kernel_unc_len > partitionInfo->KERNEL_SIZE) {
		printf("kernel too big to fit in flash\n");
		return 0;
	} else if (kernel_unc_len > (partitionInfo->KERNEL_SIZE + partitionInfo->ROOTFS_SIZE)) {
		// we are now dealing with single file
		printf("Single image too big to fit in flash\n");
		if (bz2_file) {
			printf("Trying to fit the compressed image on flash\n");
			if (filesize  > (partitionInfo->KERNEL_SIZE + partitionInfo->ROOTFS_SIZE)) {
				printf("Single image compressed format too big to fit in flash\n");
				return 0;
			}
			/* point to the bz2 image in memory */
			kernel_unc_len = filesize;
			kernel_addr = load_addr;
		} else
			return 0;
	}

	/* Erase entire NAND flash - avoid invalid file system preparations */
	printf("\nGoing to erase the entire NAND flash. ");
	if (mvVerifyRequest() == MV_TRUE)
		run_command("nand erase.chip", 0);

	printf("\nBurning %s on flash at 0x%08x, length=%dK\n",
		(single_file) ? "single image" : "kernel",
		partitionInfo->KERNEL_START, kernel_unc_len/_1K);

	printf("Erasing 0x%x - 0x%x: (%dMB)\n", partitionInfo->KERNEL_START,
			partitionInfo->KERNEL_START + partitionInfo->KERNEL_SIZE,
			partitionInfo->KERNEL_SIZE / _1M);
	flashErase(partitionInfo->KERNEL_START, partitionInfo->KERNEL_SIZE, isNand);
	printf("\t\t[Done]\n");

	printf("\nCopy to Flash\n");
	flashWrite(partitionInfo->KERNEL_START, kernel_unc_len, kernel_addr, isNand);
	printf("\n");

	if (!single_file) {
		printf("Erasing 0x%x - 0x%x: (%dMB)\n", partitionInfo->ROOTFS_START,
				partitionInfo->ROOTFS_START + partitionInfo->ROOTFS_SIZE,
				partitionInfo->ROOTFS_SIZE / _1M);
		flashErase(partitionInfo->ROOTFS_START, partitionInfo->ROOTFS_SIZE, isNand);

		printf("\nBurning rootfs on flash at 0x%08x, length=%dK\n",
		       partitionInfo->ROOTFS_START, (rootfs_unc_len / _1K));
		printf("\nCopy to Flash\n");
		flashWrite(partitionInfo->ROOTFS_START, rootfs_unc_len, rootfs_addr, isNand);
		printf("\n");
	}


	printf("\nDo you want to prepare CPSS environment variables (mtdparts & bootcmd) ? [y/N]");
	if (mvVerifyRequest() == MV_TRUE)
		run_command("cpss_env", 0);

	return 1;
}

U_BOOT_CMD(
	mtdburn,      6,     1,      do_mtdburn,
	"Burn a Linux image and Filesystem` on the NAND/SPI flash.\n",
	"[interface [<dev[:part]>  [File system [filename]]]] [flash destination] [single file size on RAM]\n"
	"\tinterface  : ram <load address>, tftp, or mmc/usb <interface_num> (default is tftp)\n"
	"\tFile system: FAT or EXT2 (default is FAT).\n"
	"\tNAND default file-name is ubifs_arm_nand.image.\n"
	"\tSPI default file-name is jffs2_arm.image.\n"
	"\tFlash Destination: nand or spi (default is nand). \n"
	"\te.g. MMC: 'mtdburn mmc 0 FAT ubifs_arm_nand.image nand'\n"
	"\te.g. RAM: 'mtdburn ram 5000000 nand'\n"

);

#define  SMI_WRITE_ADDRESS_MSB_REGISTER   (0x00)
#define  SMI_WRITE_ADDRESS_LSB_REGISTER   (0x01)
#define  SMI_WRITE_DATA_MSB_REGISTER      (0x02)
#define  SMI_WRITE_DATA_LSB_REGISTER      (0x03)

#define  SMI_READ_ADDRESS_MSB_REGISTER    (0x04)
#define  SMI_READ_ADDRESS_LSB_REGISTER    (0x05)
#define  SMI_READ_DATA_MSB_REGISTER       (0x06)
#define  SMI_READ_DATA_LSB_REGISTER       (0x07)

#define  SMI_STATUS_REGISTER              (0x1f)

#define SMI_STATUS_WRITE_DONE             (0x02)
#define SMI_STATUS_READ_READY             (0x01)

//#define SMI_WAIT_FOR_STATUS_DONE
#define SMI_TIMEOUT_COUNTER  10000

static int phy_in_use(MV_U32 phy_addr)
{
		int i;
		for (i = 0; i < mvCtrlEthMaxPortGet(); i++) {
				if (mvBoardPhyAddrGet(i) == phy_addr)
						return 1;
		}
		return 0;
}

static inline void smiWaitForStatus(MV_U32 phyAddr)
{
#ifdef SMI_WAIT_FOR_STATUS_DONE
		volatile unsigned int stat;
		unsigned int timeOut;
		MV_STATUS rc;

		/* wait for write done */
		timeOut = SMI_TIMEOUT_COUNTER;
		do {
				rc = mvEthPhyRegRead(phyAddr, SMI_STATUS_REGISTER, &stat);
				if (rc != MV_OK)
						return;
				if (--timeOut < 1) {
						printf("bspSmiWaitForStatus timeout !\n");
						return;
				}
		} while ((stat & SMI_STATUS_WRITE_DONE) == 0);
#endif
}

int bspSmiReadReg(MV_U32 phyAddr, MV_U32 actSmiAddr, MV_U32 regAddr, MV_U32 *valuePtr)
{
		/* Perform indirect smi read reg */
		MV_STATUS		rc;
		MV_U16			msb, lsb;

		/* write addr to read */
		msb = regAddr >> 16;
		lsb = regAddr & 0xFFFF;
		rc = mvEthPhyRegWrite(phyAddr, SMI_READ_ADDRESS_MSB_REGISTER,msb);
		if (rc != MV_OK)
				return rc;

		rc = mvEthPhyRegWrite(phyAddr, SMI_READ_ADDRESS_LSB_REGISTER,lsb);
		if (rc != MV_OK)
				return rc;

		smiWaitForStatus(phyAddr);

		/* read data */
		rc = mvEthPhyRegRead(phyAddr, SMI_READ_DATA_MSB_REGISTER, &msb);
		if (rc != MV_OK)
				return rc;

		rc = mvEthPhyRegRead(phyAddr, SMI_READ_DATA_LSB_REGISTER, &lsb);
		if (rc != MV_OK)
				return rc;

		*valuePtr = ((msb & 0xFFFF) << 16) | (lsb & 0xFFFF);
		return 0;
}

int bspSmiScan(int instance, int noisy)
{
		int found1 = 0;
		int found2 = 0;
		int i;
		unsigned int data;

		/* scan for SMI devices */
		for (i = 0; i < 32;  i++) {
				bspSmiReadReg(i, 0, 0x3, &data);
				if (data != 0xffffffff && data != 0xffff) {
						bspSmiReadReg(i, 0, 0x50, &data);
						if (data == 0x000011ab  || data == 0xab110000) {
								if (instance == found1++) {
										bspSmiReadReg(i, 0, 0x4c, &data);
										printf("Smi Scan found Marvell device at smi_addr 0x%x, "
											   "reg 0x4c=0x%08x\n", i, data);
										found2 = 1;
										break;
								}
						}
				}
		}

		if (!found2) {
				if (noisy)
						printf("Smi scan found no device\n");
				return 0;
		}

		return  i;
}


static int do_smi_scan(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
		int i;
		int rc = 0;

		printf("Performing SMI scan\n");
		for (i = 0; i < 32; i++) {
				if (!phy_in_use(i))
						rc += bspSmiScan(i, 0);
		}

		if (rc == 0)
				printf("smiscan found not device\n");
		return 1;
}

U_BOOT_CMD(
		  smiscan,      1,     1,      do_smi_scan,
		  "smiscan - Scan for marvell smi devices.\n",
		  ""
		  );

SILICON_TYPE get_attached_silicon_type(void)
{
		int Device;
		int Function;
		int BusNum;
		unsigned char HeaderType;
		unsigned short VendorID;
		unsigned short DeviceID;
		pci_dev_t dev;
		SILICON_TYPE silt = SILT_OTHER;

		for (BusNum = 0; BusNum < 10; BusNum ++)
				for (Device = 0; Device < PCI_MAX_PCI_DEVICES; Device++) {
						HeaderType = 0;
						VendorID = 0;
						for (Function = 0; Function < PCI_MAX_PCI_FUNCTIONS; Function++) {
								/*
								 * If this is not a multi-function device, we skip the rest.
								 */
								if (Function && !(HeaderType & 0x80))
										break;

								dev = PCI_BDF(BusNum, Device, Function);

								pci_read_config_word(dev, PCI_VENDOR_ID, &VendorID);
								if ((VendorID == 0xFFFF) || (VendorID == 0x0000))
										continue;

								pci_read_config_word(dev, PCI_DEVICE_ID, &DeviceID);
								if ((DeviceID == 0xFFFF) || (DeviceID == 0x0000))
										continue;

								if ( (VendorID == MARVELL_VENDOR_ID) &&
									 (((DeviceID & MV_PP_CHIP_TYPE_MASK) >> MV_PP_CHIP_TYPE_OFFSET) ==
									  MV_PP_CHIP_TYPE_XCAT2)
								   )
										silt = SILT_XCAT2;

								else if ((VendorID == MARVELL_VENDOR_ID) && (DeviceID == NP5_DEVICE_ID))
										silt = SILT_NP5;

								else if ((VendorID == MARVELL_VENDOR_ID) && ( (DeviceID & 0xffe0) == BC2_DEVICE_ID))
										silt = SILT_BC2;

								return silt;
						}
				}
		return silt;
}

static int do_qsgmii_sel(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

		int bit_mask, rc;

		if (argc < 2)
				goto usage;


		bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x0000ffff);

		rc = mvBoardTwsiWrite(BOARD_DEV_TWSI_PCA9555_IO_EXPANDER, 0, 2, (MV_U8)((bit_mask >> 0) & 0xff));
		if(rc == MV_OK) {
				mvBoardTwsiWrite(BOARD_DEV_TWSI_PCA9555_IO_EXPANDER, 0, 3, (MV_U8)((bit_mask >> 8) & 0xff));
				mvBoardTwsiWrite(BOARD_DEV_TWSI_PCA9555_IO_EXPANDER, 0, 6, 0);
				mvBoardTwsiWrite(BOARD_DEV_TWSI_PCA9555_IO_EXPANDER, 0, 7, 0);
				return 1;
		}

		usage:
		cmd_usage(cmdtp);
		return 1;
}


U_BOOT_CMD(
		  qsgmii_sel,      2,     1,      do_qsgmii_sel,
		  " Select SFP or QSGMII modes on bc2.\n",
		  " apply 16 bit array to select SFP or QSGMII modes"
		  );

void hwServicesLateInit(void)
{
		char *env;
		char buf[128];
		printf("hwServices late init: ");

		if ((env = getenv("qsgmii_ports_def"))) {
				sprintf(buf, "qsgmii_sel %s\n", env);
				printf(buf);
				run_command(buf, 0);
		}
		printf("\n");
}
