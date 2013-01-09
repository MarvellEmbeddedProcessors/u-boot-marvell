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
#ifndef _SOC_SPEC_H
#define _SOC_SPEC_H

#include "bootstrap_def.h"

/************************/
/* CPU ADDRESS DECODING */
/************************/
#define BOOTROM_BASE    		0xFFFF0000

#ifdef MV_FPGA_HW
	#define INTER_REGS_BASE		0xF1000000
#else
	#define INTER_REGS_BASE		0xD0000000
#endif

#define SPI_FLASH_BASE			0xD4000000
#define NOR_FLASH_BASE			0xD8000000 /* Boot CS - NOR Flash on BootCS */
#define NAND_FLASH_BASE			0xD8000000 /* Boot CS - Legacy NAND Controller */

#define RAM_TOP					0x40004000 /*  L2 cache 512KB - (16KB for MMU table) */

#define HDR_BLK_OFFSET			0x00000000
#define MEM_POOLS_OFFSET		(HDR_BLK_OFFSET + MAX_HEADER_SIZE)
#define AES_TBLS_OFFSET			(MEM_POOLS_OFFSET + 0x00002A00)

#ifdef MV_EFUSE_IN_RAM
#warning This switch is not allowed in production release!
	#define BOOT_HEAP_OFFSET	(AES_TBLS_OFFSET + 0x00001340) /* 64 bytes for eFuse simulation */
#else
	#define BOOT_HEAP_OFFSET	(AES_TBLS_OFFSET + 0x00001300)
#endif
/***************************/
/* BOOTROM_ROUTINE_ERR_REG */
/***************************/
#define BOOTROM_ROUTINE_ERR_REG			0x182D0
#define BOOTROM_EXEC_ADDR_REG			0x182D4

/* Error Code , 0 - Pass, Non-Zero - Fail*/
#define BRE_ERR_CODE_OFFS			0
#define BRE_ERR_CODE_MASK			(0xFF << BRE_ERR_CODE_OFFS)
#define NO_ERR              			(0x00 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_HEADER_ID			(0x11 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_HDR_CHKSUM		(0x12 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_IMG_CHKSUM		(0x13 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_HDR_VERSION		(0x14 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_EXT_HDR_CHKSUM		(0x15 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_EXT_HDR_TYPE		(0x16 << BRE_ERR_CODE_OFFS)
#define ERROR_BUSY				(0x17 << BRE_ERR_CODE_OFFS)
#define ERROR_LINK				(0x18 << BRE_ERR_CODE_OFFS)
#define ERROR_DMA				(0x19 << BRE_ERR_CODE_OFFS)
#define ERROR_PIO				(0x1a << BRE_ERR_CODE_OFFS)
#define ERROR_TIMEOUT				(0x1b << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_HDR_SIZE			(0x1c << BRE_ERR_CODE_OFFS)
#define ERROR_UNSUPPORTED_BOOT			(0x1d << BRE_ERR_CODE_OFFS)
#define ERROR_UNSUPPORTED_SAR_VAL		(0x1e << BRE_ERR_CODE_OFFS)
#define ERROR_END_OF_MEDIA_REACHED		(0x1f << BRE_ERR_CODE_OFFS)
#define ERROR_MEMORY_INIT			(0x20 << BRE_ERR_CODE_OFFS)
#define ERROR_ALIGN_SIZE			(0x21 << BRE_ERR_CODE_OFFS)
#define ERROR_ALIGN_SRC				(0x22 << BRE_ERR_CODE_OFFS)
#define ERROR_ALIGN_DEST			(0x23 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_READ_FAIL			(0x30 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_BOOTROM_CHKSUM		(0x31 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_IMAGE_NOT_ALIGN_256		(0x32 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_ECC_ERROR			(0x33 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_TIMEOUT			(0x34 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_EXCESS_BAD_BLK		(0x35 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_BAD_BLK			(0x36 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_UNSUPPORTED_ECC		(0x37 << BRE_ERR_CODE_OFFS)
#define ERROR_NAND_CTL_INIT			(0x38 << BRE_ERR_CODE_OFFS)
#define ERROR_I2C_READ_ERROR			(0x40 << BRE_ERR_CODE_OFFS)
#define ERROR_I2C_WRITE_ERROR			(0x41 << BRE_ERR_CODE_OFFS)
#define ERROR_I2C_START_ERROR			(0x42 << BRE_ERR_CODE_OFFS)
#define ERROR_I2C_STOP_ERROR			(0x43 << BRE_ERR_CODE_OFFS)
#define ERROR_I2C_NO_HDR_FOUND_ERROR		(0x44 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_SECURITY_HDR		(0x51 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_RSA_PUB_KEY		(0x52 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_RSA_PUB_KEY_FMT		(0x53 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_RSA_HDR_SIGN		(0x54 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_RSA_IMG_SIGN		(0x55 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_BOX_ID			(0x56 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_FLASH_ID			(0x57 << BRE_ERR_CODE_OFFS)
#define ERROR_RSA_LIB_ERROR			(0x58 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_BINARY			(0x61 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_IMG_LEN			(0x62 << BRE_ERR_CODE_OFFS)
#define ERROR_INVALID_HDR_LEN			(0x63 << BRE_ERR_CODE_OFFS)

/* Routine Number */
#define BRE_RTN_NUM_OFFS			8
#define BRE_RTN_NUM_MASK			(0xF << BRE_RTN_NUM_OFFS)
#define BRE_RTN_UART_BOOT			(0x1 << BRE_RTN_NUM_OFFS)
#define BRE_RTN_SATA_BOOT			(0x4 << BRE_RTN_NUM_OFFS)
#define BRE_RTN_SPI_BOOT			(0x7 << BRE_RTN_NUM_OFFS)
#define BRE_RTN_NOR_BOOT			(0x8 << BRE_RTN_NUM_OFFS)
#define BRE_RTN_NAND_BOOT			(0x9 << BRE_RTN_NUM_OFFS)
#define BRE_RTN_EXCEP				(0xb << BRE_RTN_NUM_OFFS)
#define BRE_RTN_EXEC				(0xc << BRE_RTN_NUM_OFFS)
#define BRE_RTN_I2C				(0xd << BRE_RTN_NUM_OFFS)
#define BRE_RTN_PEX_BOOT			(0xe << BRE_RTN_NUM_OFFS)

/* Bit that says the DRAM is initiated */
#define BRE_DRAM_INIT_OFFS			12
#define BRE_DRAM_INIT_MASK			(0x1 << BRE_DRAM_INIT_OFFS)

/* Disable ECC in NAND boot */
#define BRE_ECC_OVERRIDE_OFFS			13
#define BRE_ECC_OVERRIDE_MASK			(0x3 << BRE_ECC_OVERRIDE_OFFS)
#define BRE_ECC_NORMAL				(0x0 << BRE_ECC_OVERRIDE_OFFS)
#define BRE_ECC_HAMMING				(0x1 << BRE_ECC_OVERRIDE_OFFS)
#define BRE_ECC_BCH				(0x2 << BRE_ECC_OVERRIDE_OFFS)
#define BRE_ECC_NONE				(0x3 << BRE_ECC_OVERRIDE_OFFS)

/* Disable Phy configuration in SATA boot */
#define BRE_PHY_CFG_DISABLE_OFFS		13
#define BRE_PHY_CFG_DISABLE_MASK		(0x1 << BRE_PHY_CFG_DISABLE_OFFS)

/* Disable controller configuration in SATA boot */
#define BRE_SATA_CFG_DISABLE_OFFS		14
#define BRE_SATA_CFG_DISABLE_MASK		(0x1 << BRE_SATA_CFG_DISABLE_OFFS)

/* BOOTROM test mode */
#define BRE_TEST_MODE_OFFS			15
#define BRE_TEST_MODE_MASK			(0x1 << BRE_TEST_MODE_OFFS)

/* Retry */
#define BRE_RTN_RETRY_OFFS			16
#define BRE_RTN_RETRY_MASK			(0xFF << BRE_RTN_RETRY_OFFS)
#ifdef MV_RTL_PLATFORM
#define BRE_MAX_RETRY				(0x2 << BRE_RTN_RETRY_OFFS)
#else
#define BRE_MAX_RETRY				(0x10 << BRE_RTN_RETRY_OFFS)
#endif

#define BRE_RTN_TRACE_OFFS			24
#define BRE_RTN_TRACE_MASK			(0xFFUL << BRE_RTN_TRACE_OFFS)

#define TRACE_INIT				0x10
#define TRACE_MAIN				0x20
#define TRACE_EXEC				0x30
#define TRACE_DRAM				0x40
#define TRACE_EXTENTION				0x50
#define TRACE_UART				0x60
#define TRACE_I2C				0x70
#define TRACE_PEX				0x80
#define TRACE_SPI				0x90
#define TRACE_NAND				0xa0
#define TRACE_SATA				0xb0
#define TRACE_RDRW_RWG				0xc0
#define TRACE_NAND_ECC				0xd0

/******************************/
/* Analog Group Configuration */
/******************************/
#define ANALOG_GROUP_CFG_REG			0x1847c
#define ANALOG_GRP0_BGR_OFFS			3
#define ANALOG_GRP0_BGR_MASK			(0x3 << ANALOG_GRP0_BGR_OFFS)
#define ANALOG_GRP0_BGR_MODIFIED_BJT_MDL	(0x1 << ANALOG_GRP0_BGR_OFFS)

/************/
/* DEVBUS   */
/************/
#define DEV_BOOTSCN_READ_PARAM_REG		0x10400
#define DEV_BOOTSCN_READ_RD_SETUP_OFFS		12
#define DEV_BOOTSCN_READ_RD_SETUP_MASK		(0x1F << DEV_BOOTSCN_READ_RD_SETUP_OFFS)
#define DEV_BOOTSCN_READ_RD_SETUP_VAL		(0x8 << DEV_BOOTSCN_READ_RD_SETUP_OFFS)
#define DEV_BOOTSCN_READ_RD_HOLD_OFFS		23
#define DEV_BOOTSCN_READ_RD_HOLD_MASK		(0x1F << DEV_BOOTSCN_READ_RD_HOLD_OFFS)
#define DEV_BOOTSCN_READ_RD_HOLD_VAL		(0x8 << DEV_BOOTSCN_READ_RD_HOLD_OFFS)

#define DEV_BOOTSCN_READ_BUS_WIDTH_OFFS		30
#define DEV_BOOTSCN_READ_BUS_WIDTH_MASK		(0x3 << DEV_BOOTSCN_READ_BUS_WIDTH_OFFS)
#define DEV_BOOTSCN_READ_BUS_WIDTH_8_BIT	(0x0 << DEV_BOOTSCN_READ_BUS_WIDTH_OFFS)
#define DEV_BOOTSCN_READ_BUS_WIDTH_16_BIT	(0x1 << DEV_BOOTSCN_READ_BUS_WIDTH_OFFS)
#define DEV_BOOTSCN_READ_BUS_WIDTH_32_BIT	(0x2 << DEV_BOOTSCN_READ_BUS_WIDTH_OFFS)

#define DEV_BOOTSCN_WRITE_PARAM_REG		0x10404

#define DEV_BUS_IFACE_CTRL_REG			0x104C0
#define DEV_BUS_IFACE_CTRL_OEWE_SHARED_OFFS	16
#define DEV_BUS_IFACE_CTRL_OEWE_SHARED_MASK	(0x1 << DEV_BUS_IFACE_CTRL_OEWE_SHARED_OFFS)
#define DEV_BUS_IFACE_CTRL_ALE_TIMING_OFFS	17
#define DEV_BUS_IFACE_CTRL_ALE_TIMING_MASK	(0x3 << DEV_BUS_IFACE_CTRL_ALE_TIMING_OFFS)

#define DEV_BUS_SYNC_CTRL_REG			0x104C8
#define DEV_BUS_BOOTCS_READY_IGNORE		(0x1 << 4)
#define DEV_BUS_BOOTCS_POLARITY			(0x1 << 5)

/********/
/* NAND */
/********/
#define DEV_NAND_CTRL_REG			0x10470
#define DINFCR_NF_ACT_CE			BIT1

//
// Timing register parameter 0.
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +---------+-+-------+-----+-----+---+-----+-----+---+-----+-----+
// |  tADL   |S|  RCD  | tCH | tCS |   | tWH | tWP |   | tRH | tRP |
// +---------+-+-------+-----+-----+---+-----+-----+---+-----+-----+
//

#define TIMING_MAX_tADL		31U
#define TIMING_DEF_SEL_CNTR	1
#define TIMING_MAX_RD_CNT_DEL	0
#define TIMING_MAX_tCH		7
#define TIMING_MAX_tCS		7
#define TIMING_MAX_tWH		7
#define TIMING_MAX_tWP		7
#define TIMING_MAX_etRP		1	/* New setting - 1, old - 0*/
#define TIMING_MAX_tRH		7
#define TIMING_MAX_tRP		7

#define DFC_NDTR0CS0_DEF   ((TIMING_MAX_tADL << 27) | \
             (TIMING_DEF_SEL_CNTR << 26) | \
             (TIMING_MAX_RD_CNT_DEL << 22) | \
             (TIMING_MAX_tCH << 19) | \
             (TIMING_MAX_tCS << 16) | \
             (TIMING_MAX_tWH << 11) | \
             (TIMING_MAX_tWP << 8) | \
             (TIMING_MAX_etRP << 6) | \
             (TIMING_MAX_tRH << 3) | \
             (TIMING_MAX_tRP))

//
// Timing register parameter 1.
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-------------------------------+-+-+------+----+-------+-------+
// |               tR              | | | RES |tRHW| tWHR  |  tAR  |
// +-------------------------------+-+-+-----+----+-------+-------+
//                                 W P
//                                 A R
//                                 I E
//                                 T S
//                                 _ C
//                                 M A
//                                 O L
//                                 D E
//                                 E

#define TIMING_MAX_tR		65535U
#define TIMING_WAIT_MODE	1	/* Work with RnB signal (1) or ignore (0) it */
#define TIMING_PRESCALE		0	/* no prescalling */
#define TIMING_MAX_tRHW		3	/* New setting - 3, old - 0*/
#define TIMING_MAX_tWHR		15
#define TIMING_MAX_tAR		15

#define DFC_NDTR1CS0_DEF   ((TIMING_MAX_tR << 16) | \
             (TIMING_WAIT_MODE << 15) | \
             (TIMING_PRESCALE << 14) | \
             (TIMING_MAX_tRHW << 8) | \
             (TIMING_MAX_tWHR << 4) | \
             (TIMING_MAX_tAR))

#define DFC_NDTR1CS0_IGNORE_RNB_DEF   ((TIMING_MAX_tR << 16) | \
             (TIMING_PRESCALE << 14) | \
             (TIMING_MAX_tRHW << 8) | \
             (TIMING_MAX_tWHR << 4) | \
             (TIMING_MAX_tAR))

//
// DFC register values for each control point.
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-+-+-+-+-+-+---+-+-+-+-+-+-----+-+---+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |1|1|0|0|x|x|x x|1|0|0|0|0|x x x|x|x x|1|1|1|1|1|1|1|1|1|1|1|1|1| S/W DEFAULT
// +-+-+-+-+-+-+---+-+-+-+-+-+-----+-+---+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  S E D N D D  P  S N F C S   R   R  P  N R C C C C C C D S W R W
//  P C M D W W  A  E D O L T   D   A  G  D D S S S S S S B B R D R
//  A C A _ I I  G  Q _ R R O   _   _  _  _ Y 0 1 0 1 0 1 E E D D C
//  R _ _ R D D  E  _ S C _ P   I   S  P  A M _ _ _ _ _ _ R R R R M
//  E E E U T T  _  D T E P _   D   T  E  R   P P C C B B R R E E D
//  _ N N N H H  S  I O - G O   _   A  R  B   A A M M B B M M Q Q R
//  E       _ _  Z  S P C _ N   C   R  _  _   G G D D D D     M M E
//  N       C M         S C _   N   T  B  E   E E D D M M         Q
//                      X N U   T      L  N   D D M M             M
//                        T N          K      M M
//             C
//             O
//             R
//

#define DFC_NDCR_PG_PER_BLK_OFFS	13
#define DFC_NDCR_PG_PER_BLK_MASK	(0x3 << DFC_NDCR_PG_PER_BLK_OFFS)
#define DFC_NDCR_PG_PER_BLK_32		(0x0 << DFC_NDCR_PG_PER_BLK_OFFS)
#define DFC_NDCR_PG_PER_BLK_64		(0x2 << DFC_NDCR_PG_PER_BLK_OFFS)
#define DFC_NDCR_PG_PER_BLK_128		(0x1 << DFC_NDCR_PG_PER_BLK_OFFS)
#define DFC_NDCR_PG_PER_BLK_256		(0x3 << DFC_NDCR_PG_PER_BLK_OFFS)
#define DFC_NDCR_RA_START_OFFS		15
#define DFC_NDCR_RA_START_MASK		(0x1 << DFC_NDCR_RA_START_OFFS)
#define DFC_NDCR_RA_START_2ND_CYC	(0x0 << DFC_NDCR_RA_START_OFFS)
#define DFC_NDCR_RA_START_3RD_CYC	(0x1 << DFC_NDCR_RA_START_OFFS)
#define DFC_NDCR_RD_ID_CNT_OFFS		16
#define DFC_NDCR_RD_ID_CNT_MASK		(0x7 << DFC_NDCR_RD_ID_CNT_OFFS)
#define DFC_NDCR_RD_ID_CNT_2		(0x2 << DFC_NDCR_RD_ID_CNT_OFFS)
#define DFC_NDCR_RD_ID_CNT_4		(0x4 << DFC_NDCR_RD_ID_CNT_OFFS)
#define DFC_NDCR_PAGE_SZ_OFFS		24
#define DFC_NDCR_PAGE_SZ_MASK		(0x3 << DFC_NDCR_PAGE_SZ_OFFS)
#define DFC_NDCR_PAGE_SZ_512B		(0x0 << DFC_NDCR_PAGE_SZ_OFFS)
#define DFC_NDCR_PAGE_SZ_2K		(0x1 << DFC_NDCR_PAGE_SZ_OFFS)
#define DFC_NDCR_PAGE_SZ_4K		(0x2 << DFC_NDCR_PAGE_SZ_OFFS)
#define DFC_NDCR_PAGE_SZ_8K		(0x3 << DFC_NDCR_PAGE_SZ_OFFS)
#define DFC_NDCR_DWIDTH_M_C_OFFS	26
#define DFC_NDCR_DWIDTH_M_C_MASK	(0x3 << DFC_NDCR_DWIDTH_M_C_OFFS)
#define DFC_NDCR_ND_RUN_OFFS		28
#define DFC_NDCR_ND_RUN_MASK		(0x1 << DFC_NDCR_ND_RUN_OFFS)
#define DFC_NDCR_ECC_EN_OFFS		30
#define DFC_NDCR_ECC_EN_MASK		(0x1 << DFC_NDCR_ECC_EN_OFFS)
#define DFC_NDCR_ECC_EN			(0x1 << DFC_NDCR_ECC_EN_OFFS)

#define DFC_NDCR_DEFAULT_VAL		0x80801FFF
#define DFC_NDCR_DEFAULT_VAL_NO_SPARE	0x00801FFF

//
// DFC register offsets for each control point in command buffer 0.
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-----------+-+-+-----+-+-+-----+---------------+---------------+
// |           | | |     | | |     |               |               |
// +-----------+-+-+-----+-+-+-----+---------------+---------------+
//   reserved   A C   C   N D   A         CMD2            CMD1
//              U S   M   C B   D
//              T E   D     C   D
//              O L   _         R
//              _     T         _
//              R     Y         C
//              S     P         Y
//                    E         C
//
#define DFC_NDCB0_RESET		0x00A000FF
#define DFC_NDCB0_READID	0x00610090
#define DFC_NDCB0_READ_LB	0x000D3000   /* 5 address cycles w/0x30*/
#define DFC_NDCB0_READ_SB_4	0x00040000   /* 4 address cycels */
#define DFC_NDCB0_READ_SB_3	0x00030000   /* 3 address cycels */


//
// DFC register offsets for each control point in command buffer 1.
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +---------------+---------------+---------------+---------------+
// |               |               |               |               |
// +---------------+---------------+---------------+---------------+
//      ADDR4           ADDR3           ADDR2           ADDR1
//
#define DFC_CMD_OFFSET_ADDR4	24
#define DFC_CMD_OFFSET_ADDR3	16
#define DFC_CMD_OFFSET_ADDR2	8
#define DFC_CMD_OFFSET_ADDR1	0


//
// DFC register offsets for each control point in command buffer 2.
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-----------------------------------+-----------+---------------+
// |                                   |           |               |
// +-----------------------------------+-----------+---------------+
//               reserved                PAGE_COUNT     ADDR5
//
#define DFC_CMD_OFFSET_PAGE_COUNT	8
#define DFC_CMD_OFFSET_ADDR5		0

//
// DFC status register masks for each status bit.
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +---------------------------------------+-+-+-+-+-+-+-+-+-+-+-+-+
// |                                       | | | | | | | | | | | | |
// +---------------------------------------+-+-+-+-+-+-+-+-+-+-+-+-+
//                 reserved                 R C C C C C C D S W R W
//                                          D S S S S S S B B R D R
//                                          Y 0 1 0 1 0 1 E E D D C
//                                            _ _ _ _ _ _ R R R R M
//                                            P P C C B B R R E E D
//                                            A A M M B B     Q Q R
//                                            G G D D D D         E
//                                            E E D D             Q
//                                            D D
//
#define DFC_SR_RDY0                   0x00000800
#define DFC_SR_RDY1                   0x00001000
#define DFC_SR_CS0_PAGED              0x00000400
#define DFC_SR_CS1_PAGED              0x00000200
#define DFC_SR_CS0_CMDD               0x00000100
#define DFC_SR_CS1_CMDD               0x00000080
#define DFC_SR_CS0_BBD                0x00000040
#define DFC_SR_CS1_BBD                0x00000020
#define DFC_SR_UNCOR_ERR              0x00000010
#define DFC_SR_CORR_ERR               0x00000008
#define DFC_SR_WRDREQ                 0x00000004
#define DFC_SR_RDDREQ                 0x00000002
#define DFC_SR_WRCMDREQ               0x00000001

//
// DFC Ecc Control
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +---------------------------------+---------------+-----------+-+
// |                                 |               |           | |
// +---------------------------------+---------------+-----------+-+
//               reserved               ECC_SPARE      ECC_THRESH B
//                          C
//                          H
//                          _
//                          E
//                          N
#define DFC_ECC_CTRL_BCH_EN_OFFS	0
#define DFC_ECC_CTRL_BCH_EN_MASK	(1 << DFC_ECC_CTRL_BCH_EN_OFFS)
#define DFC_ECC_CTRL_BCH_EN		(1 << DFC_ECC_CTRL_BCH_EN_OFFS)
#define DFC_ECC_CTRL_THRESH_OFFS	1
#define DFC_ECC_CTRL_THRESH_MASK	(0x3F << DFC_ECC_CTRL_BCH_EN_OFFS)
#define DFC_ECC_CTRL_SPARE_OFFS		7
#define DFC_ECC_CTRL_SPARE_MASK		(0xFF << DFC_ECC_CTRL_BCH_EN_OFFS)

// Command Buffer 0
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-----+-+-------------------------------------------------------+
// |     | |                                                       |
// +-----+-+-------------------------------------------------------+
//   CMD  L
//    X   E
//    T   N
//    Y   _
//    P   O
//    E   V
//        R
//        D
#define DFC_CB0_LEN_OVRD_OFFS		28
#define DFC_CB0_LEN_OVRD_MASK		(0x1 << DFC_CB0_LEN_OVRD_OFFS)
#define DFC_CB0_CMD_XTYPE_OFFS		29
#define DFC_CB0_CMD_XTYPE_MASK		(0x7U << DFC_CB0_CMD_XTYPE_OFFS)
#define DFC_CB0_CMD_XTYPE_MONOLITHIC	(0x0U << DFC_CB0_CMD_XTYPE_OFFS)
#define DFC_CB0_CMD_XTYPE_LAST_NAKED	(0x1U << DFC_CB0_CMD_XTYPE_OFFS)
#define DFC_CB0_CMD_XTYPE_MULTIPLE	(0x4U << DFC_CB0_CMD_XTYPE_OFFS)
#define DFC_CB0_CMD_XTYPE_NAKED		(0x5U << DFC_CB0_CMD_XTYPE_OFFS)
#define DFC_CB0_CMD_XTYPE_DISPATCH	(0x6U << DFC_CB0_CMD_XTYPE_OFFS)


//
// Data flash controller register locations.
//
#define DFC_CONTROL                  0xD0000  /* NDCR     : Control.       */
#define DFC_TIMING_0                 0xD0004  /* NDTR0CS0 : Timing reg 0.  */
#define DFC_TIMING_1                 0xD000C  /* NDTR1CS0 : Timing reg 1.  */
#define DFC_STATUS                   0xD0014  /* NDSR     : Status.        */
#define DFC_PAGES                    0xD0018  /* NDPCR    : Page count.    */
#define DFC_BADBLOCK0                0xD001C  /* NDBDR0   : Bad block 0.   */
#define DFC_BADBLOCK1                0xD0020  /* NDBDR1   : Bad block 1.   */
#define DFC_ECC_CONTROL              0xD0028  /* NDECCCTRL: ECC Control    */
#define DFC_DATA                     0xD0040  /* NDDB     : Data buffer.   */
#define DFC_COMMAND0                 0xD0048  /* NDCB0    : Command buffer.*/

/*******************/
/* SAMPLE AT RESET */
/*******************/
#define MPP_SAMPLE_AT_RESET_0			0x18230
#define MPP_SAMPLE_AT_RESET_1			0x18234

/* Boot device bus width */
#define MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS		3
#define MSAR_0_BOOT_DEV_BUS_WIDTH_MASK		(0x3 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
/* Bus width field meaning for NOR/NAND */
#define MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT		(0x0 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
#define MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT		(0x1 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
#define MSAR_0_BOOT_DEV_BUS_WIDTH_32BIT		(0x2 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
/* Bus width field meaning for SPI */
#define MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_24_16BIT	(0x1 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
#define MSAR_0_BOOT_DEV_BUS_WIDTH_SPI_32BIT	(0x0 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)

/* Boot device */
#define MSAR_0_BOOT_MODE_OFFS			5
#define MSAR_0_BOOT_MODE_MASK			(0xF << MSAR_0_BOOT_MODE_OFFS)
#define MSAR_0_BOOT_NOR_FLASH			(0x0 << MSAR_0_BOOT_MODE_OFFS)
#define MSAR_0_BOOT_NAND_NEW			(0x1 << MSAR_0_BOOT_MODE_OFFS)
#define MSAR_0_BOOT_UART			(0x2 << MSAR_0_BOOT_MODE_OFFS)
#define MSAR_0_BOOT_SPI_FLASH			(0x3 << MSAR_0_BOOT_MODE_OFFS)
#define MSAR_0_BOOT_PEX				(0x4 << MSAR_0_BOOT_MODE_OFFS)
#define MSAR_0_BOOT_SATA			(0x5 << MSAR_0_BOOT_MODE_OFFS)
#define MSAR_0_BOOT_NAND_LEGACY			(0x6 << MSAR_0_BOOT_MODE_OFFS)
#define MSAR_0_BOOT_PROMPT			(0x7 << MSAR_0_BOOT_MODE_OFFS)
#define MSAR_0_BOOT_NOR_FLASH_DELAYED		(0xC << MSAR_0_BOOT_MODE_OFFS)

/* NAND page size */
#define MSAR_0_NAND_PAGE_SZ_OFFS		11
#define MSAR_0_NAND_PAGE_SZ_MASK		(0x3 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_512B		(0x0 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_2KB			(0x1 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_4KB			(0x2 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_8KB			(0x3 << MSAR_0_NAND_PAGE_SZ_OFFS)

/* SATA SERDES lane used for boot - dual use with NAND page size */
#define MSAR_0_SATA_SERDES_LINE_OFFS		11
#define MSAR_0_SATA_SERDES_LINE_MASK		(0x3 << MSAR_0_SATA_SERDES_LINE_OFFS)
#define MSAR_0_SATA_SERDES_LINE_4		(0x0 << MSAR_0_SATA_SERDES_LINE_OFFS)
#define MSAR_0_SATA_SERDES_LINE_5		(0x1 << MSAR_0_SATA_SERDES_LINE_OFFS)
#define MSAR_0_SATA_SERDES_LINE_6		(0x2 << MSAR_0_SATA_SERDES_LINE_OFFS)
#define MSAR_0_SATA_SERDES_LINE_RES		(0x3 << MSAR_0_SATA_SERDES_LINE_OFFS)

/* NOR OE/WE muxing */
#define MSAR_0_NOR_WE_OE_MUX_OFFS		13
#define MSAR_0_NOR_WE_OE_MUX_MASK		(1 << MSAR_0_NOR_WE_OE_MUX_OFFS)

/* NAND ECC */
#define MSAR_0_NAND_ECC_OFFS			14
#define MSAR_0_NAND_ECC_MASK			(0x3 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_4BIT			(0x0 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_8BIT			(0x1 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_12BIT			(0x2 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_16BIT			(0x3 << MSAR_0_NAND_ECC_OFFS)

#if 0 /* Not needed anymore - BTS#416 */
/* NOR ALE timing - dual use with NAND ECC */
#define MSAR_0_NOR_ALE_TIMING_OFFS		14
#define MSAR_0_NOR_ALE_TIMING_MASK		(0x3 << MSAR_0_NOR_ALE_TIMING_OFFS)
#endif

/* TCLK frequency selector (two bit field) */
#define MSAR_0_TCLK0_OFFS			28
#define MSAR_0_TCLK0_MASK			(0x1 << MSAR_0_TCLK0_OFFS)
#ifdef MV_DSMP_Z1B
#define MSAR_1_TCLK1_OFFS			8
#define MSAR_1_TCLK1_MASK			(0x1 << MSAR_1_TCLK1_OFFS)
#endif

/**********/
/* TIMERS */
/**********/
#define TIMERS_CTRL_REG				0x20300
#define TIMER0_ENABLE_OFFS			0x0
#define TIMER0_ENABLE_MASK			(0x1 << TIMER0_ENABLE_OFFS)
#define TIMER0_AUTO_OFFS			0x1
#define TIMER0_AUTO_MASK			(0x1 << TIMER0_ENABLE_OFFS)

#define TIMER0_RELOAD_REG			0x20310
#define TIMER0_VALUE_REG			0x20314

/*******/
/* PEX */
/*******/
/************/
/* SOC MISC */
/************/

#define SOC_CTRL_REG				0x18204
#define SCR_PEX_ENA_OFFS(unit)			((unit) & 0x3)
#define SCR_PEX_ENA_MASK(unit)			(1 << SCR_PEX_ENA_OFFS(unit))

#define SOC_CTRL_PEX0_X1_MODE_OFFSET		7
#define SOC_CTRL_PEX0_X1_MODE_MASK		(1 << SOC_CTRL_PEX0_X1_MODE_OFFSET)

#define SOC_DEV_MUX_REG				0x18208
#define SOC_DEV_MUX_NEW_NAND_MASK		BIT0
#define SOC_DEV_MUX_NF_ARBITER_MASK		BIT27

#define GEN_PURP_RES_2_REG			0x182F8
#define GEN_PURP_COMPHY_0_3_X4_MODE_MASK	0x0000000F

#define SOC_RSTOUTN_MASK_REG			0x18260
#define SOC_RSTOUTN_PEX0_TRST_MASK		BIT5

/*******************/
/* Special purpose */
/******************/
#define GEN_PURPOSE_RESERVED_1			0x182E4
#define GPR1_DEV_BOOT_CS_ASSERT_DELAY_EN_OFFSET	4
#define GPR1_RD_HOLD_AD_EN_OFFSET		5

/*******/
/* SPI */
/*******/

#define SPI_IF_CTRL_REG				0x10600

#define SPI_IF_CONFIG_REG			0x10604
#define SPI_ADDRESS_SIZE_OFFS			8
#define SPI_ADDRESS_SIZE_MASK			(0x3 << SPI_ADDRESS_SIZE_OFFS)
#define SPI_ADDRESS_1_CYCLES			(0x0 << SPI_ADDRESS_SIZE_OFFS)
#define SPI_ADDRESS_2_CYCLES			(0x1 << SPI_ADDRESS_SIZE_OFFS)
#define SPI_ADDRESS_3_CYCLES			(0x2 << SPI_ADDRESS_SIZE_OFFS)
#define SPI_ADDRESS_4_CYCLES			(0x3 << SPI_ADDRESS_SIZE_OFFS)

/********/
/* SATA */
/********/
#define SATA_REGS_BASE(port)                  	(0xA0000 + (port)*0x2000)

/* reserved areas on the DRAM*/
#define MV_SATA_EDMA_REQUEST_BUFFER_START	_8K
#define MV_SATA_EDMA_REQUEST_BUFFER_SIZE	32 /* the size of single CRQB*/
#define MV_SATA_EDMA_REQUEST_BUFFER_END		((MV_SATA_EDMA_REQUEST_BUFFER_START) + (MV_SATA_EDMA_REQUEST_BUFFER_SIZE))
#define MV_SATA_EDMA_RESPONSE_BUFFER_START	MV_SATA_EDMA_REQUEST_BUFFER_END
#define MV_SATA_EDMA_RESPONSE_BUFFER_SIZE	8 /* the size of single CRPB*/

/********/
/* UART */
/********/
#define UART_REG_OFFSET(port)			(0x12000 + (port) * 0x100)

/********/
/* MPPs */
/********/
#define MPP_CONTROL_0_REG			0x18000		/* MPP[7:0] */
#define MPP_CONTROL_1_REG			0x18004		/* MPP[15:8] */
#define MPP_CONTROL_2_REG			0x18008		/* MPP[23:16] */
#define MPP_CONTROL_3_REG			0x1800C		/* MPP[31:24] */
#define MPP_CONTROL_4_REG			0x18010		/* MPP[39:32] */
#define MPP_CONTROL_5_REG			0x18014		/* MPP[47:40] */

#define MPP_CONTROL_6_REG			0x18018		/* MPP[55:48] */
#define MPP_6_NOR_32B_ACCESS_MASK		0xFFFFFFF0
#define MPP_6_NOR_32B_ACCESS			0x11111110

#define MPP_CONTROL_7_REG			0x1801C		/* MPP[63:56] */
#define MPP_7_NOR_32B_ACCESS_MASK		0xFFFFFFFF
#define MPP_7_NOR_32B_ACCESS			0x11111111

#define MPP_CONTROL_8_REG			0x18020		/* MPP[71:64] */
#define MPP_8_NOR_32B_ACCESS_MASK		0x00000FFF
#define MPP_8_NOR_32B_ACCESS			0x00000111

#define MPP_CONTROL_9_REG			0x18024		/* MPP[79:72] */
#define MPP_CONTROL_10_REG			0x18028		/* MPP[87:80] */
#define MPP_CONTROL_11_REG			0x1802C		/* MPP[95:88] */

#define MPP_5_UART_2_RX_MASK			0x00000F00	/* MPP[42] */
#define MPP_5_UART_2_RX_EN			0x00000100
#define MPP_5_UART_2_TX_MASK			0x0000F000	/* MPP[43] */
#define MPP_5_UART_2_TX_EN			0x00001000
#define MPP_5_UART_3_RX_MASK			0x000F0000	/* MPP[44] */
#define MPP_5_UART_3_RX_EN			0x00020000
#define MPP_5_UART_3_TX_MASK			0x00F00000	/* MPP[45] */
#define MPP_5_UART_3_TX_EN			0x00200000

/********/
/* GPIO */
/********/
#define GPIO_DATA_OUT_REG			0x18100
#define GPIO_TEST_PIN_OFFS			7
#define GPIO_TEST_PIN_MASK			(0x1 << GPIO_TEST_PIN_OFFS)

/*******/
/* PMU */
/*******/
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-----------+-+-+---------+-+-+-+-------------------------+-+-+-+
// |  RESERVED | | |RESERVED | | | |         RESERVED        | | | |
// +-----------+-+-+---------+-+-+-+-------------------------+-+-+-+
//              L H           D D P                           D D P
//              T T           V F W                           V F W
//              H H           S S R                           S S R
//              R R
//                            M M M                           D D U
//              M M           A A A                           O O P
//              S S           S S S                           N N
//              K K           K K K                           E E
//
#define PMU_EVENT_STATUS_L2C_REG		0x22020

//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +----------+-+-+---------+-+-+-+-----------+-+-+---------+-+-+-+
// |  RESERVED | | |RESERVED | | | |  RESERVED | | | RESERVED| | | |
// +-----------+-+-+---------+-+-+-+-----------+-+-+---------+-+-+-+
//              L H           D D P             L H           D D P
//              T T           V F W             T T           V F W
//              H H           S S R             H H           S S R
//              R R                             R R
//                            M M M             S S           D D U
//              M M           A A A             H H           O O P
//              S S           S S S             L L           N N
//              K K           K K K             D D           E E
//
#define PMU_EVENT_STATUS_REG(CPUID)		(0x22120 + ((CPUID) * 0x100))

#define PMU_RESUME_TYPE_OFFS			0
#define PMU_RESUME_TYPE_MASK			(0x1 << PMU_RESUME_TYPE_OFFS)
#define PMU_RESUME_TYPE_NONE			(0x0 << PMU_RESUME_TYPE_OFFS)
#define PMU_RESUME_TYPE_STBY			(0x1 << PMU_RESUME_TYPE_OFFS)

//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +---------------------------------------------------------------+
// |            RESUME_ADDR (BOOT ADDRESS REDIRECT)                |
// +---------------------------------------------------------------+
#define PMU_RESUME_ADDR_REG(CPUID)		(0x22124 + ((CPUID) * 0x100))

//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +---------------------------------------------------------------+
// |                ADDRESS OF DESCRIPTORS ARRAY                   |
// +---------------------------------------------------------------+
//
#ifdef MV_DSMP_Z1B
#define PMU_RESUME0_CTRL_REG			0x20980
#define PMU_RESUME1_CTRL_REG			0x20988
#else
#define PMU_RESUME_CTRL_REG(CPUID)		(0x22128 + ((CPUID) * 0x100))
#endif

//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +---------------------------------+---------------------+-------+
// |         CONFIGS_US_DLY          |    CONFIGS_CNT      |  TYP  |
// +---------------------------------|---------------------+-------+
//
#define PMU_RESUME_CFG_TYP_OFFS			0
#define PMU_RESUME_CFG_TYP_MASK			(0xF << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_NONE			(0x0 << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_ADDR_VAL_32B		(0x1 << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_ADDR_VAL_16B		(0x2 << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_ADDR_VAL_8B		(0x3 << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_ADDR_MSK_VAL		(0x4 << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_BIT_CLR		(0x5 << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_BIT_SET		(0x6 << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_POLL			(0x7 << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_DELAY		(0x8 << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_DDR3_WL		(0x9 << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_TYP_DDR3_WL_SET		(0xA << PMU_RESUME_CFG_TYP_OFFS)
#define PMU_RESUME_CFG_CNT_OFFS			4
#define PMU_RESUME_CFG_CNT_MASK			(0x7FF << PMU_RESUME_CFG_CNT_OFFS)
#define PMU_RESUME_CFG_US_DLY_OFFS		15
#define PMU_RESUME_CFG_US_DLY_MASK		(0x1FFF << PMU_RESUME_CFG_US_DLY_OFFS)

/*******/
/* DDR */
/*******/
#define SDRAM_CTRL_REG				0x1480
#define SDRAM_INIT_EN_OFFS			0
#define SDRAM_INIT_EN_MASK			(0x1 << SDRAM_INIT_EN_OFFS)

/*******/
/* I2C */
/*******/

#define TWSI_DATA_REG				0x11004
#define TWSI_DATA_COMMAND_OFFS			0x0
#define TWSI_DATA_COMMAND_MASK 			(0x1 << TWSI_DATA_COMMAND_OFFS)
#define TWSI_DATA_ADDR_7BIT_ADDR		0xA0 /* device address 0 */

#define TWSI_CONTROL_REG			0x11008
#define TWSI_CONTROL_ACK            		(1 << 2)
#define TWSI_CONTROL_INT_FLAG_SET   		(1 << 3)
#define TWSI_CONTROL_STOP_BIT    		(1 << 4)
#define TWSI_CONTROL_START_BIT 			(1 << 5)
#define TWSI_CONTROL_ENA     			(1 << 6)
#define TWSI_CONTROL_INT_ENA    		(1 << 7)

#define TWSI_STATUS_BAUDE_RATE_REG		0x1100c
#define TWSI_BAUD_RATE_DEF_200MHZ		0x2C	/* [6:3]M=5, [2:0]N=4 */

#define TWSI_START_CON_TRA			0x08
#define TWSI_REPEATED_START_CON_TRA		0x10
#define TWSI_AD_PLS_WR_BIT_TRA_ACK_REC		0x18
#define TWSI_M_TRAN_DATA_BYTE_ACK_REC 		0x28
#define TWSI_AD_PLS_RD_BIT_TRA_ACK_REC		0x40
#define TWSI_M_REC_RD_DATA_ACK_TRA		0x50
#define TWSI_M_REC_RD_DATA_ACK_NOT_TRA		0x58
#define TWSI_NO_REL_STS_INT_FLAG_IS_KEPT_0	0xF8

#define TWSI_SOFT_RESET_REG 			0x1101c

#define TWSI_CPU_MAIN_INT_CAUSE_REG		0x20200
#define TWSI_CPU_MAIN_INT_TWSI_OFFS		29 /*5*/
#define TWSI_CPU_MAIN_INT_TWSI_BIT		(1 << TWSI_CPU_MAIN_INT_TWSI_OFFS)

/************/
/*   PUP   */
/************/
#define PUP_ENABLE_REG				0x1864C
#define PUP_ENABLE_GPORT0_MASK			(0x1 << 0)
#define PUP_ENABLE_GPORT1_MASK			(0x1 << 1)
#define PUP_ENABLE_LCD_MASK			(0x1 << 2)
#define PUP_ENABLE_DEVICE_MASK			(0x1 << 3)
#define PUP_ENABLE_NAND_NG_MASK			(0x1 << 4)
#define PUP_ENABLE_SPI_MASK			(0x1 << 5)
#define PUP_ENABLE_SDIO_MASK			(0x1 << 6)

/************/
/*  Clocks  */
/************/
#define CORE_DIVCLK_CONTROL0_REG		0x18740
#define CORE_DIVCLK_RELOAD_FORCE_OFFS		0
#define CORE_DIVCLK_RELOAD_FORCE_MASK		0xFF
#define CORE_DIVCLK_RELOAD_FORCE_VAL		(2 << CORE_DIVCLK_RELOAD_FORCE_OFFS)
#define CORE_DIVCLK_RELOAD_RATIO_OFFS		8
#define CORE_DIVCLK_RELOAD_RATIO_MASK		(1 << CORE_DIVCLK_RELOAD_RATIO_OFFS)

#define CORE_DIVCLK_RATIO_FULL0_REG		0x18748
/* Devider for 2GHz clock to get 250MHz ECC clock and 125MHz NAND clock (ECC/2) */
#define NAND_ECC_DIVCKL_RATIO_OFFS		8
#define NAND_ECC_DIVCKL_RATIO_MASK		(0x3F << NAND_ECC_DIVCKL_RATIO_OFFS)
#define NAND_ECC_DIVCKL_RATIO_VAL		(0x8 << NAND_ECC_DIVCKL_RATIO_OFFS)

#define CPU_RTC_TIMING_CONFIG_REG(cpu)		(0x18450 + (cpu) * 8)

/************/
/* Security */
/************/
#ifdef MV_EFUSE_IN_RAM
#warning This switch is not allowed in production release!
	#define SEC_AES_KEY_REG			(RAM_TOP + BOOT_HEAP_OFFSET - 16)
	#define SEC_RSA_KEY_REG			(SEC_AES_KEY_REG - 32)
	#define SEC_BOX_ID_REG			(SEC_RSA_KEY_REG - 4)
	#define SEC_BOOT_VIS_CTRL_REG		(SEC_BOX_ID_REG - 4)
	#define SEC_BOOT_CTRL_REG		(SEC_BOOT_VIS_CTRL_REG - 4)
#else
	#define SEC_AES_KEY_REG			0x10080
	#define SEC_RSA_KEY_REG			0x10020
	#define SEC_BOX_ID_REG			0x10014
	#define SEC_BOOT_VIS_CTRL_REG		0x100B0
	#define SEC_BOOT_CTRL_REG		0x10010
#endif

#define SEC_BOOT_MODE_OFFS			0
#define SEC_BOOT_MODE_MASK			(0x1 << SEC_BOOT_MODE_OFFS)
#define SEC_JTAG_DSBL_OFFS			1
#define SEC_JTAG_DSBL_MASK			(0x1 << SEC_JTAG_DSBL_OFFS)
#define SEC_BOOT_DEV_OFFS			4
#define SEC_BOOT_DEV_MASK			(0xFF << SEC_BOOT_DEV_OFFS)
#define SEC_FLASH_ID_OFFS			16
#define SEC_FLASH_ID_MASK			(0xFFFFUL << SEC_FLASH_ID_OFFS)

#define EFUSE_BOOT_SRC_SPI			(0x1 << 0)
#define EFUSE_BOOT_SRC_NAND			(0x1 << 1)
#define EFUSE_BOOT_SRC_SATA			(0x1 << 2)
#define EFUSE_BOOT_SRC_NOR			(0x1 << 3)


#ifdef MV_EFUSE_IN_RAM
#warning This switch is not allowed in production release!
	#define EFUSE_GET_AES_KEY_WORD(x)	(MV_MEMIO_LE32_READ(SEC_AES_KEY_REG + 4 * (x)))
	#define EFUSE_GET_RSA_SHA_WORD(x)	(MV_MEMIO_LE32_READ(SEC_RSA_KEY_REG + 4 * (x)))
	#define EFUSE_GET_BOX_ID		(MV_MEMIO_LE32_READ(SEC_BOX_ID_REG))
	#define EFUSE_GET_FLASH_ID		((MV_MEMIO_LE32_READ(SEC_BOOT_CTRL_REG) \
			& SEC_FLASH_ID_MASK)  >> SEC_FLASH_ID_OFFS)
	#define EFUSE_GET_SECURE_BOOT		((MV_MEMIO_LE32_READ(SEC_BOOT_CTRL_REG) \
			& SEC_BOOT_MODE_MASK)  >> SEC_BOOT_MODE_OFFS)
	#define EFUSE_GET_JTAG_DSBL		((MV_MEMIO_LE32_READ(SEC_BOOT_CTRL_REG) \
			& SEC_JTAG_DSBL_MASK)  >> SEC_JTAG_DSBL_OFFS)
	#define EFUSE_GET_BOOT_SOURCE		((MV_MEMIO_LE32_READ(SEC_BOOT_CTRL_REG) \
			& SEC_BOOT_DEV_MASK)  >> SEC_BOOT_DEV_OFFS)
#else
	#define EFUSE_GET_AES_KEY_WORD(x)	(MV_REG_READ(SEC_AES_KEY_REG + 4 * (x)))
	#define EFUSE_GET_RSA_SHA_WORD(x)	(MV_REG_READ(SEC_RSA_KEY_REG + 4 * (x)))
	#define EFUSE_GET_BOX_ID		(MV_REG_READ(SEC_BOX_ID_REG))
	#define EFUSE_GET_FLASH_ID		((MV_REG_READ(SEC_BOOT_CTRL_REG) \
			& SEC_FLASH_ID_MASK)  >> SEC_FLASH_ID_OFFS)
#ifdef MV_SECURE_BOOT_DISABLE
	#define EFUSE_GET_SECURE_BOOT		(0)
#else
	#define EFUSE_GET_SECURE_BOOT		((MV_REG_READ(SEC_BOOT_CTRL_REG) \
			& SEC_BOOT_MODE_MASK)  >> SEC_BOOT_MODE_OFFS)
#endif
	#define EFUSE_GET_JTAG_DELAY		((MV_REG_READ(SEC_BOOT_CTRL_REG) \
			& SEC_JTAG_DSBL_MASK)  >> SEC_JTAG_DSBL_OFFS)
	#define EFUSE_GET_BOOT_SOURCE		((MV_REG_READ(SEC_BOOT_CTRL_REG) \
			& SEC_BOOT_DEV_MASK)  >> SEC_BOOT_DEV_OFFS)
#endif

/* Global flags */
#ifdef MV_EFUSE_IN_RAM
#warning This switch is not allowed in production release!
#define GLOBAL_FLAGS_REG		(SEC_BOOT_CTRL_REG - 4)
#else
#define GLOBAL_FLAGS_REG		(RAM_TOP + BOOT_HEAP_OFFSET - 4)
#endif

#define GLOBAL_PRINT_FLAG_OFFS		0
#define GLOBAL_PRINT_FLAG_MASK		(1 << GLOBAL_PRINT_FLAG_OFFS)
#define GLOBAL_PRINT_DISABLE		(MV_MEMIO_LE32_WRITE(GLOBAL_FLAGS_REG,\
					(MV_MEMIO_LE32_READ(GLOBAL_FLAGS_REG) & ~GLOBAL_PRINT_FLAG_MASK)))
#define GLOBAL_PRINT_ENABLE		(MV_MEMIO_LE32_WRITE(GLOBAL_FLAGS_REG,\
					(MV_MEMIO_LE32_READ(GLOBAL_FLAGS_REG) | GLOBAL_PRINT_FLAG_MASK)))
#define GLOBAL_PRINTING_ENABLED		((MV_MEMIO_LE32_READ(GLOBAL_FLAGS_REG) | GLOBAL_PRINT_FLAG_MASK) != 0)

#define MSAR_DDR_L2_CLK_RATIO_IDX(sar0,sar1)	((((sar0) >> 24) & 0xF) + ((((sar1) >> 19) & 1) << 4))
#define MSAR_CPU_CLK_IDX(sar0, sar1)		((((sar0) >> 21) & 0x7) + ((((sar1) >> 20) & 1) << 3))

#define MV_CPU_CLK_TBL { 1000, 1066, 1200, 1333, 1500, 1666, 1800, 2000,\
			  600,  667,  800, 1600, 2133, 2200, 2400, 0 }

/*		cpu	l2c	hclk	ddr	*/
#define MV_DDR_L2_CLK_RATIO_TBL			{ \
/*00*/	{	1,	1,	4,	2	},\
/*01*/	{	1,	2,	2,	2	},\
/*02*/	{	2,	2,	6,	3	},\
/*03*/	{	2,	2,	3,	3	},\
/*04*/	{	1,	2,	3,	3	},\
/*05*/	{	1,	2,	4,	2	},\
/*06*/	{	1,	1,	2,	2	},\
/*07*/	{	2,	3,	6,	6	},\
/*08*/	{	2,	3,	5,	5	},\
/*09*/	{	1,	2,	6,	3	},\
/*10*/	{	2,	4,	10,	5	},\
/*11*/	{	1,	3,	6,	6	},\
/*12*/	{	1,	2,	4,	4	},\
/*13*/	{	1,	3,	6,	3	},\
/*14*/	{	1,	2,	5,	5	},\
/*15*/	{	2,	2,	5,	5	},\
/*16*/	{	1,	1,	3,	3	},\
/*17*/	{	2,	5,	10,	10	},\
/*18*/	{	1,	3,	8,	4	},\
/*19*/	{	1,	1,	2,	1	},\
/*20*/	{	2,	3,	6,	3	},\
/*21*/	{	1,	2,	8,	4	},\
/*22*/	{	2,	5,	10,	5	},\
/*23*/	{	1,	1,	20,	20	},\
/*24*/	{	1,	1,	60,	60	},\
/*25*/	{	1,	2,	60,	60	},\
/*26*/	{	1,	5,	60,	60	},\
/*27*/	{	2,	2,	60,	60	},\
/*28*/	{	2,	3,	60,	60	},\
/*29*/	{	2,	6,	60,	60	},\
/*30*/	{	2,	5,	60,	60	},\
/*31*/	{	1,	2,	40,	40	} \
}

#endif /* _SOC_SPEC_H */
