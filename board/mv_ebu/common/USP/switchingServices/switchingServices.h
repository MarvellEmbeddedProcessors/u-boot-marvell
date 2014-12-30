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

	*	Redistributions of source code must retain the above copyright notice,
			this list of conditions and the following disclaimer.

	*	Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

	*	Neither the name of Marvell nor the names of its contributors may be
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

/*
		Linux/vxWorks partitions on nor flash
*/
struct partitionInformation {
	char *defaultImage;
	MV_U32 FLASH_SIZE;
	MV_U32 KERNEL_SIZE;
	MV_U32 ROOTFS_SIZE;
	MV_U32 KERNEL_START;
	MV_U32 BLOCK_ALIMENT;
	MV_U32 ROOTFS_START;
};

#define CFG_DEF_SOURCE_LOAD_ADDR		0x2000000
#define CFG_DEF_KERNEL_DEST_ADDR		0x10000000
#define CFG_DEF_ROOTFS_DEST_ADDR		(CFG_DEF_KERNEL_DEST_ADDR + 0x1000000)

#define CFG_APPL_SPI_FLASH_START_DIRECT 	SPI_CS_BASE
#define CFG_APPL_SPI_FLASH_SIZE 		SPI_CS_SIZE
#define CFG_APPL_SPI_FLASH_BLOCK_ALIMENT	0x100000

#define CFG_APPL_SPI_FLASH_PARTITIONS_NAME	"Linux/vxWorks"

#define CFG_APPL_FLASH_PART_UBOOT_START	(0)
#define CFG_APPL_FLASH_PART_UBOOT_SIZE	_2M

#define CFG_APPL_SPI_FLASH_PART_KERNEL_START	(CFG_APPL_FLASH_PART_UBOOT_START + CFG_APPL_FLASH_PART_UBOOT_SIZE)
#define CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE	_4M

#define CFG_APPL_SPI_FLASH_PART_ROOTFS_START	(CFG_APPL_SPI_FLASH_PART_KERNEL_START + CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE)

#define ALL_SPI_PARTS	(CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE +  CFG_APPL_FLASH_PART_UBOOT_SIZE + _16M)
#define REST_SPI_FLASH	(CFG_APPL_SPI_FLASH_SIZE - (CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE +		\
						 CFG_APPL_FLASH_PART_UBOOT_SIZE))

#define CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE 	((SPI_CS_SIZE > ALL_SPI_PARTS) ? _16M : 		\
						(CFG_APPL_SPI_FLASH_SIZE -                      \
						 (CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE +		\
						 CFG_APPL_FLASH_PART_UBOOT_SIZE)))

#define CFG_APPL_NAND_FLASH_SIZE		_1G
#define CFG_APPL_NAND_FLASH_BLOCK_ALIMENT	0x40000

#define CFG_APPL_NAND_FLASH_PART_KERNEL_START	(0)
#define CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE	(_8M + _2M)  /* _10M */

#define CFG_APPL_NAND_FLASH_PART_ROOTFS_START	(CFG_APPL_NAND_FLASH_PART_KERNEL_START + \
							CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE)

#define ALL_NAND_PARTS	(CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE)
#define REST_NAND_FLASH	(CFG_APPL_NAND_FLASH_SIZE - CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE)

#define CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE	_32M
/*
 sram definitions for freertos
*/

#define CONFIG_FREERTOS_SRAM_ADDR	0x50000000

#define MARVELL_VENDOR_ID		0x11ab
#define NP5_DEVICE_ID			0x2012
#define BC2_DEVICE_ID			0xfc00
#define INT_LVL_XCAT2_SWITCH		55	/* Switch MG interrupt */

/*
 * dev_id[15:10] bits of DeviceID register of Prestera (0x4C)
 * determine the chip type (xCat or xCat2).
 * dev_id[15:10] == 0x37 stands for xCat
 * dev_id[15:10] == 0x39 stands for xCat2
 */
#define MV_PP_CHIP_TYPE_MASK		0x000FC00
#define MV_PP_CHIP_TYPE_OFFSET		10
#define MV_PP_CHIP_TYPE_XCAT2		0x39

#ifndef __ASSEMBLY__

typedef enum {	// special device enum
		SILT_XCAT2,
		SILT_NP5,
		SILT_BC2,
		SILT_OTHER,
		SLIC_NOT_DETECT = 0x5a5a
} SILICON_TYPE;

SILICON_TYPE get_attached_silicon_type(void);
int do_sar_bc2(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[]);

#endif
