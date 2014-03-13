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
/*******************************************************************************
* mv_print_appl_partitions - Print u-boot partitions on SPI flash
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
static void mv_print_appl_partitions(void)
{
		run_command("sf probe 0", 0);
		printf("\n");
		printf("%s partitions on spi flash\n", CFG_APPL_SPI_FLASH_PARTITIONS_NAME);
		printf("--------------------------------------\n\n");
		printf("Spi flash Address (rx)   : 0x%08x\n", CFG_APPL_SPI_FLASH_START_DIRECT);
		printf("Spi flash size           : %dMB\n", CFG_APPL_SPI_FLASH_SIZE/(1<<20));
		printf("u-boot               : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_SPI_FLASH_PART_UBOOT_START,
			   CFG_APPL_SPI_FLASH_PART_UBOOT_SIZE,
			   CFG_APPL_SPI_FLASH_PART_UBOOT_SIZE/(1<<10));

		printf("kernel/vxWorks-image : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_SPI_FLASH_PART_KERNEL_START,
			   CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE,
			   CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE/(1<<10));

		printf("Linux rootfs         : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_SPI_FLASH_PART_ROOTFS_START,
			   CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE,
			   CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE/(1<<10));

		printf("\n");
		printf("Single Image         : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_SPI_FLASH_PART_KERNEL_START,
			   CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE + CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE,
			   (CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE + CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE)/(1<<10));

		run_command("nand info", 0);
		printf("\n");
		printf("%s partitions on nand flash\n", CFG_APPL_SPI_FLASH_PARTITIONS_NAME);
		printf("--------------------------------------\n\n");
		printf("Nand flash size           : %dMB\n", CFG_APPL_NAND_FLASH_SIZE/(1<<20));

		printf("kernel/vxWorks-image : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_NAND_FLASH_PART_KERNEL_START,
			   CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE,
			   CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE/(1<<10));

		printf("Linux rootfs         : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_NAND_FLASH_PART_ROOTFS_START,
			   CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE,
			   CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE/(1<<10));

		printf("\n");
		printf("Single Image         : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_NAND_FLASH_PART_KERNEL_START,
			   CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE,
			   (CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE)/(1<<10));

		printf("\n");
}

static int do_print_spi_part ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[] ) {
		mv_print_appl_partitions();
		return 1;
}

U_BOOT_CMD(
	print_spi_part,    2,    1,    do_print_spi_part,
	"print_spi_part  - print SPI FLASH memory information\n",
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
static int do_cpss_env( cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[] ) {
		char buf[1024];
		char buf1[512];

		printf("Saving cpss environment variable\n");
		setenv("setenv standalone", "");
		setenv("bootcmd", "run standalone_mtd");
		setenv("consoledev","ttyS0");
		setenv("linux_loadaddr","0x2000000");
		setenv("netdev","eth0");
		setenv("rootpath","/tftpboot/rootfs_arm-mv7sft");
		setenv("othbootargs","null=null");

		setenv("nfsboot","setenv bootargs root=/dev/nfs rw nfsroot=$serverip:$rootpath ip=$ipaddr:$serverip:$gatewayip:$netmask:$hostname:$netdev:off console=$consoledev,$baudrate $othbootargs $linux_parts; tftp $linux_loadaddr $image_name;bootm $linux_loadaddr");

		sprintf(buf, "spi_flash:0x%08x(spi_uboot)ro,"
				"0x%08x(spi_kernel),0x%08x(spi_rootfs)\%%armada-nand:0x%08x(nand_kernel),0x%08x(nand_rootfs)",
				CFG_APPL_SPI_FLASH_PART_UBOOT_SIZE,
				CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE,
				CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE,
				CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE,
				CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE);

		setenv("mtdparts",buf);
		sprintf(buf1, "mtdparts=%s", buf);
		setenv("linux_parts",buf1);

		sprintf(buf,
				"setenv bootargs ubi.mtd=4 root=ubi0:rootfs_nand "
				"rw rootfstype=ubifs "
				"ip=$ipaddr:$serverip:$gatewayip:$netmask:$hostname:$netdev:off "
				"console=$consoledev,$baudrate $othbootargs $linux_parts; "
				"nand read $linux_loadaddr 0x%08x 0x%08x; bootm $linux_loadaddr ",
				CFG_APPL_NAND_FLASH_PART_KERNEL_START,
				CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE);

		setenv("standalone_mtd_nand", buf);

		sprintf(buf,
				"setenv bootargs ubi.mtd=2 root=ubi0:rootfs_spi "
				"rw rootfstype=ubifs "
				"ip=$ipaddr:$serverip:$gatewayip:$netmask:$hostname:$netdev:off "
				"console=$consoledev,$baudrate $othbootargs $linux_parts; "
				"bootm 0x%08x ",
				CFG_APPL_SPI_FLASH_START_DIRECT +
				CFG_APPL_SPI_FLASH_PART_KERNEL_START);

		setenv("standalone_mtd_spi", buf);

		sprintf(buf,
				"setenv bootargs root=/dev/mtdblock2 "
				"rw rootfstype=jffs2 "
				"ip=$ipaddr:$serverip:$gatewayip:$netmask:$hostname:$netdev:off "
				"console=$consoledev,$baudrate $othbootargs $linux_parts; "
				"bootm 0x%08x ",
				CFG_APPL_SPI_FLASH_START_DIRECT +
				CFG_APPL_SPI_FLASH_PART_KERNEL_START);

		setenv("standalone_mtd", buf);

		setenv("image_name","uImage_armadaxp");
		run_command("saveenv", 0);

		return 1;
}

U_BOOT_CMD(
	cpss_env,      2,     1,      do_cpss_env,
	"set cpss environment variables permanently\n",
	""
);

extern struct spi_flash *flash;

static int do_spi_mtdburn(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
		int filesize;
		ulong addr, src_addr, dest_addr;
		uint kernel_unc_len, rootfs_unc_len = 0, unc_len, src_len;
		uint kernel_addr = 0x6000000;
		uint rootfs_addr = 0x7000000;
		uint total_in;
		int rc;
		int single_file = 0;
		ulong erase_end_offset;
		int bz2_file = 0;

		addr = load_addr = 0x5000000;

		if (!flash) {
				flash = spi_flash_probe(0, 0, CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
				if (!flash) {
						printf("Failed to probe SPI Flash\n");
						return 0;
				}
		}


		if (argc == 2) {
				copy_filename (BootFile, argv[1], sizeof(BootFile));
		} else {
				copy_filename (BootFile, "ubifs_arm_spi.image", sizeof(BootFile));
				printf("\nUsing default file \"ubifs_arm_spi.image\" \n");
		}

		if ((filesize = NetLoop(TFTPGET)) < 0)
				return 0;

		if (filesize > CFG_APPL_SPI_FLASH_SIZE) {
				printf("file too big\n");
				return 0;
		}

		printf("\nTrying separation of kernel/vxWorks-image and root_fs. "
			   "Work areas=0x%08x,0x%08x\n",
			   kernel_addr, rootfs_addr);

		dest_addr = kernel_addr; // uncompress the kernel here.
		src_addr = addr;
		src_len = unc_len = 0x1000000*3; //16MB*3 = max+

		rc = BZ2_bzBuffToBuffDecompress_extended (
												 (char*)dest_addr,
												 &unc_len, (char *)src_addr, src_len, &total_in,
												 0, 0);
		printf("\n");
		kernel_unc_len = unc_len;

		if (rc == 0) {
				printf("kernel separation ended ok. unc_len=%d, total_in=%d\n",
					   unc_len, total_in);
				bz2_file++;
		}

		else if (rc == -5) {
				printf("Not a bz2 file, assuming plain single image file\n");
				single_file = 1;
				kernel_unc_len = filesize;
				kernel_addr = load_addr;
		}

		else {
				printf("Uncompress of kernel ended with error. rc=%d\n", rc);
				return 0;
		}

		if (!single_file) {
				//
				// now try to separate the rootfs. If we get -5 then we have a single file.
				//
				dest_addr = rootfs_addr; // uncompress the rootfs here.
				src_addr += total_in;
				src_len = unc_len = 0x1000000*3; //16MB*3 = max+

				rc = BZ2_bzBuffToBuffDecompress_extended (
														 (char*)dest_addr,
														 &unc_len, (char *)src_addr, src_len, &total_in,
														 0, 0);
				printf("\n");
				rootfs_unc_len = unc_len;
				if (rc == 0) {
						bz2_file++;

						printf("rootfs separation ended ok. unc_len=%d, total_in=%d\n",
							   unc_len, total_in);
						if (unc_len > CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE) {
								printf("rootfs too big\n");
								return 0;
						}
				}

				else if (rc == -5) {
						printf("One single bz2 file detected\n");
						single_file = 1;
				}

				else {
						printf("Uncompress of rootfs ended with error. rc=%d\n", rc);
						return 0;
				}
		}

		if (!single_file && kernel_unc_len > CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE) {
				printf("kernel too big to fit in flash\n");
				return 0;
		} else if (kernel_unc_len > (CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE + CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE)) {
				// we are now dealing with single file
				printf("Single image too big to fit in flash\n");

				if (bz2_file) {
						printf("Trying to fit the compressed image on flash\n");
						if (filesize  > (CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE + CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE)) {
								printf("Single image compressed format too big to fit in flash\n");
								return 0;
						}

						// point to the bz2 image in memory
						kernel_unc_len = filesize;
						kernel_addr = load_addr;
				}

				else {
						return 0;
				}
		}

		printf("\nBurning %s on flash at 0x%08x, length=%dK\n",
			   (single_file) ? "single image" : "kernel",
			   CFG_APPL_SPI_FLASH_PART_KERNEL_START, kernel_unc_len/(1<<10));

		erase_end_offset =  CFG_APPL_SPI_FLASH_PART_KERNEL_START +
							(kernel_unc_len & 0xfff00000) + 0x100000;

		printf("Erasing 0x%x - 0x%lx: ", CFG_APPL_SPI_FLASH_PART_KERNEL_START, erase_end_offset);
		spi_flash_erase(flash, CFG_APPL_SPI_FLASH_PART_KERNEL_START,
						(erase_end_offset - CFG_APPL_SPI_FLASH_PART_KERNEL_START));
		printf("\t\t[Done]\n");

		printf("Copy to Flash... ");

		spi_flash_write(flash, CFG_APPL_SPI_FLASH_PART_KERNEL_START,
						kernel_unc_len, (const void *)kernel_addr);

		printf("\n");

		if (!single_file) {
				printf("\nBurning ubifs rootfs on flash at 0x%08x\n",
					   CFG_APPL_SPI_FLASH_PART_ROOTFS_START);

				erase_end_offset =  CFG_APPL_SPI_FLASH_PART_ROOTFS_START +
									(rootfs_unc_len & 0xfff00000) + 0x100000;

				printf("Erasing 0x%x - 0x%lx: ", CFG_APPL_SPI_FLASH_PART_ROOTFS_START, erase_end_offset);
				spi_flash_erase(flash, CFG_APPL_SPI_FLASH_PART_ROOTFS_START,
								(erase_end_offset - CFG_APPL_SPI_FLASH_PART_ROOTFS_START));

				printf("Copy to Flash... ");

				spi_flash_write(flash, CFG_APPL_SPI_FLASH_PART_ROOTFS_START,
								rootfs_unc_len, (const void *)rootfs_addr);

		}
		return 1;
}

static int do_spi_mtdburn_legacy(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
		char buf[128];
		if (argc == 2) {
				copy_filename (BootFile, argv[1], sizeof(BootFile));
		} else {
				copy_filename (BootFile, "jffs2_arm.image", sizeof(BootFile));
				printf("\nUsing default file \"jffs2_arm.image\" \n");
		}

		sprintf(buf, "mtdburn_spi %s", BootFile);
		run_command(buf, 0);
		return 0;
}

U_BOOT_CMD(
		  mtdburn_spi,      2,     1,      do_spi_mtdburn,
		  "Burn a Linux/VxWorks image image on the spi flash.\n",
		  " file-name \n"
		  "\tburn a Linux/Vxworks image the flash.\n\tdefault file-name is ubifs_arm.image.\n"
		  );

U_BOOT_CMD(
		  mtdburn,      2,     1,      do_spi_mtdburn_legacy,
		  "Burn a Linux/VxWorks image image on the spi flash.\n",
		  " file-name \n"
		  "\tburn a Linux/Vxworks image the flash.\n\tdefault file-name is jffs2_arm.image.\n"
		  );


static int do_nand_mtdburn(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
		char buf[1024];
		int filesize;
		ulong addr, src_addr, dest_addr;
		uint kernel_unc_len, rootfs_unc_len = 0, unc_len, src_len;
		uint kernel_addr = 0x6000000;
		uint rootfs_addr = 0x7000000;
		uint total_in;
		int rc;
		int single_file = 0;
		int bz2_file = 0;

		addr = load_addr = 0x5000000;

		if (argc == 2) {
				copy_filename (BootFile, argv[1], sizeof(BootFile));
		} else {
				copy_filename (BootFile, "ubifs_arm_nand.image", sizeof(BootFile));
				printf("\nUsing default file \"ubifs_arm_nand.image\" \n");
		}

		if ((filesize = NetLoop(TFTPGET)) < 0)
				return 0;

		if (filesize > CFG_APPL_NAND_FLASH_SIZE) {
				printf("file too big\n");
				return 0;
		}

		printf("\nTrying separation of kernel/vxWorks-image and root_fs. "
			   "Work areas=0x%08x,0x%08x\n",
			   kernel_addr, rootfs_addr);

		dest_addr = kernel_addr; // uncompress the kernel here.
		src_addr = addr;
		src_len = unc_len = 0x1000000*3; //16MB*3 = max+

		rc = BZ2_bzBuffToBuffDecompress_extended (
												 (char*)dest_addr,
												 &unc_len, (char *)src_addr, src_len, &total_in,
												 0, 0);
		printf("\n");
		kernel_unc_len = unc_len;

		if (rc == 0) {
				printf("kernel separation ended ok. unc_len=%d, total_in=%d\n",
					   unc_len, total_in);
				bz2_file++;
		}

		else if (rc == -5) {
				printf("Not a bz2 file, assuming plain single image file\n");
				single_file = 1;
				kernel_unc_len = filesize;
				kernel_addr = load_addr;
		}

		else {
				printf("Uncompress of kernel ended with error. rc=%d\n", rc);
				return 0;
		}

		if (!single_file) {
				//
				// now try to separate the rootfs. If we get -5 then we have a single file.
				//
				dest_addr = rootfs_addr; // uncompress the rootfs here.
				src_addr += total_in;
				src_len = unc_len = 0x1000000*3; //16MB*3 = max+

				rc = BZ2_bzBuffToBuffDecompress_extended (
														 (char*)dest_addr,
														 &unc_len, (char *)src_addr, src_len, &total_in,
														 0, 0);
				printf("\n");
				rootfs_unc_len = unc_len;
				if (rc == 0) {
						bz2_file++;

						printf("rootfs separation ended ok. unc_len=%d, total_in=%d\n",
							   unc_len, total_in);
						if (unc_len > CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE) {
								printf("rootfs too big\n");
								return 0;
						}
				}

				else if (rc == -5) {
						printf("One single bz2 file detected\n");
						single_file = 1;
				}

				else {
						printf("Uncompress of rootfs ended with error. rc=%d\n", rc);
						return 0;
				}
		}

		if (!single_file && kernel_unc_len > CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE) {
				printf("kernel too big to fit in flash\n");
				return 0;
		} else if (kernel_unc_len > (CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE)) {
				// we are now dealing with single file
				printf("Single image too big to fit in flash\n");

				if (bz2_file) {
						printf("Trying to fit the compressed image on flash\n");
						if (filesize  > (CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE)) {
								printf("Single image compressed format too big to fit in flash\n");
								return 0;
						}

						// point to the bz2 image in memory
						kernel_unc_len = filesize;
						kernel_addr = load_addr;
				}

				else {
						return 0;
				}
		}

		printf("\nBurning %s on flash at 0x%08x, length=%dK\n",
			   (single_file) ? "single image" : "kernel",
			   CFG_APPL_NAND_FLASH_PART_KERNEL_START, kernel_unc_len/(1<<10));

		sprintf(buf, "nand erase 0x%08x 0x%08x",
				CFG_APPL_NAND_FLASH_PART_KERNEL_START,
				CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE);
		printf("Erasing kernel partition: %s\n", buf);
		run_command(buf, 0);

		sprintf(buf, "nand write 0x%08x 0x%08x 0x%08x",
				kernel_addr,
				CFG_APPL_NAND_FLASH_PART_KERNEL_START,
				CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE);
		printf("Copy to Flash: %s \n", buf);
		run_command(buf, 0);

		if (!single_file) {
				printf("\nBurning ubifs rootfs on flash at 0x%08x\n",
					   CFG_APPL_NAND_FLASH_PART_ROOTFS_START);

				sprintf(buf, "nand erase 0x%08x 0x%08x",
						CFG_APPL_NAND_FLASH_PART_ROOTFS_START,
						CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE);
				printf("Erasing rootfs partition: %s\n", buf);
				run_command(buf, 0);

				sprintf(buf, "nand write 0x%08x 0x%08x 0x%08x",
						rootfs_addr,
						CFG_APPL_NAND_FLASH_PART_ROOTFS_START,
						rootfs_unc_len);
				printf("Copy to Flash: %s \n", buf);
				run_command(buf, 0);
		}
		return 1;
}

U_BOOT_CMD(
		  mtdburn_nand,      2,     1,      do_nand_mtdburn,
		  "Burn a Linux/VxWorks image image on the nand flash.\n",
		  " file-name \n"
		  "\tburn a Linux/Vxworks image the flash.\n\tdefault file-name is ubifs_arm.image.\n"
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
