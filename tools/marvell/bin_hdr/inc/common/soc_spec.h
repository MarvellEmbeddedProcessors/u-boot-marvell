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

#include "config_marvell.h"     /* Required to identify SOC and Board */

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

#if defined(MV_TEST_PLATFORM)
	#define RAM_TOP			0x81004000 /*  Use PEX memory - (16KB for MMU table) */
#elif defined(MV88F6710) || defined(MV88F78X60)
	#define RAM_TOP			0x40004000 /*  L2 cache 512KB - (16KB for MMU table) */
#elif defined(MV88F66XX) || defined(MV88F68XX) || defined(MV88F672X)
	#define RAM_TOP			0x40000000 /*  L2 cache 512KB */
#else
	#define RAM_TOP			0x40004000 /*  L2 cache 512KB */
#endif

#define HDR_BLK_OFFSET		0x00000000	/* Header is 64KB long? */
#define MEM_POOLS_OFFSET	0x00010000
#define AES_TBLS_OFFSET		0x00012A00

#ifdef MV_EFUSE_IN_RAM
	#define BOOT_HEAP_OFFSET	0x00013D40 /* 64 bytes for eFuse simulation */
#else
	#define BOOT_HEAP_OFFSET	0x00013D00
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
#define BRE_MAX_RETRY				(0x10 << BRE_RTN_RETRY_OFFS)

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


/********/
/* NAND */
/********/
#define DEV_NAND_CTRL_REG			0x10470
#define DINFCR_NF_ACT_CE			BIT1

/*																		*/
/* Timing register parameter 0.                                         */
/*                                                                      */
/*  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1                         */
/*  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0     */
/* +---------+-+-------+-----+-----+---+-----+-----+---+-----+-----+    */
/* |  tADL   |S|  RCD  | tCH | tCS |   | tWH | tWP |   | tRH | tRP |    */
/* +---------+-+-------+-----+-----+---+-----+-----+---+-----+-----+    */
/*                                                                      */

#define TIMING_MAX_tADL		31U
#define TIMING_DEF_SEL_CNTR	1
#define TIMING_MAX_RD_CNT_DEL	0
#define TIMING_MAX_tCH		7
#define TIMING_MAX_tCS		7
#define TIMING_MAX_tWH		7
#define TIMING_MAX_tWP		7
#define TIMING_MAX_etRP		0
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

/*																		*/
/* Timing register parameter 1.                                         */
/*                                                                      */
/*  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1                         */
/*  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0     */
/* +-------------------------------+-+-+-----------+-------+-------+    */
/* |               tR              | | | RESERVED  | tWHR  |  tAR  |    */
/* +-------------------------------+-+-+-----------+-------+-------+    */
/*                W P                                                   */
/*                A R                                                   */
/*                I E                                                   */
/*                T S                                                   */
/*                _ C                                                   */
/*                M A                                                   */
/*                O L                                                   */
/*                D E                                                   */
/*                E                                                     */

#define TIMING_MAX_tR		65535U
#define TIMING_WAIT_MODE	1   /* Ignore RnB signal */
#define TIMING_PRESCALE		0   /* no prescalling */
#define TIMING_MAX_tWHR		15
#define TIMING_MAX_tAR		15

#define DFC_NDTR1CS0_DEF   ((TIMING_MAX_tR << 16) | \
             (TIMING_WAIT_MODE << 15) | \
             (TIMING_PRESCALE << 14) | \
             (TIMING_MAX_tWHR << 4) | \
             (TIMING_MAX_tAR))

/*		*/
/* DFC register values for each control point.                                      */
/*                                                                                  */
/*  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1                                     */
/*  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0                 */
/* +-+-+-+-+-+-+---+-+-+-+-+-+-----+-+---+-+-+-+-+-+-+-+-+-+-+-+-+-+                */
/* |1|1|0|0|x|x|x x|1|0|0|0|0|x x x|x|x x|1|1|1|1|1|1|1|1|1|1|1|1|1| S/W DEFAULT	*/
/* +-+-+-+-+-+-+---+-+-+-+-+-+-----+-+---+-+-+-+-+-+-+-+-+-+-+-+-+-+                */
/*  S E D N D D  P  S N F C S   R   R  P  N R C C C C C C D S W R W                 */
/*  P C M D W W  A  E D O L T   D   A  G  D D S S S S S S B B R D R                 */
/*  A C A _ I I  G  Q _ R R O   _   _  _  _ Y 0 1 0 1 0 1 E E D D C                 */
/*  R _ _ R D D  E  _ S C _ P   I   S  P  A M _ _ _ _ _ _ R R R R M                 */
/*  E E E U T T  _  D T E P _   D   T  E  R   P P C C B B R R E E D                 */
/*  _ N N N H H  S  I O - G O   _   A  R  B   A A M M B B M M Q Q R                 */
/*  E       _ _  Z  S P C _ N   C   R  _  _   G G D D D D     M M E                 */
/*  N       C M         S C _   N   T  B  E   E E D D M M         Q                 */
/*                      X N U   T      L  N   D D M M             M                 */
/*                        T N          K      M M                                   */
/*             C                                                                    */
/*             O                                                                    */
/*             R                                                                    */
/*                                                                                  */

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

/*
 DFC register offsets for each control point in command buffer 0.

  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 +-----------+-+-+-----+-+-+-----+---------------+---------------+
 |           | | |     | | |     |               |               |
 +-----------+-+-+-----+-+-+-----+---------------+---------------+
   reserved   A C   C   N D   A         CMD2            CMD1
              U S   M   C B   D
              T E   D     C   D
              O L   _         R
              _     T         _
              R     Y         C
              S     P         Y
                    E         C
*/
#define DFC_NDCB0_RESET		0x00A000FF
#define DFC_NDCB0_READID	0x00610090
#define DFC_NDCB0_READ_LB	0x000D3000   /* 5 address cycles w/0x30*/
#define DFC_NDCB0_READ_SB_4	0x00040000   /* 4 address cycels */
#define DFC_NDCB0_READ_SB_3	0x00030000   /* 3 address cycels */


/*
 DFC register offsets for each control point in command buffer 1.

  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 +---------------+---------------+---------------+---------------+
 |               |               |               |               |
 +---------------+---------------+---------------+---------------+
      ADDR4           ADDR3           ADDR2           ADDR1
*/
#define DFC_CMD_OFFSET_ADDR4	24
#define DFC_CMD_OFFSET_ADDR3	16
#define DFC_CMD_OFFSET_ADDR2	8
#define DFC_CMD_OFFSET_ADDR1	0


/*
 DFC register offsets for each control point in command buffer 2.

  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 +-----------------------------------+-----------+---------------+
 |                                   |           |               |
 +-----------------------------------+-----------+---------------+
               reserved                PAGE_COUNT     ADDR5
*/
#define DFC_CMD_OFFSET_PAGE_COUNT	8
#define DFC_CMD_OFFSET_ADDR5		0

/*
 DFC status register masks for each status bit.

  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 +---------------------------------------+-+-+-+-+-+-+-+-+-+-+-+-+
 |                                       | | | | | | | | | | | | |
 +---------------------------------------+-+-+-+-+-+-+-+-+-+-+-+-+
                 reserved                 R C C C C C C D S W R W
                                          D S S S S S S B B R D R
                                          Y 0 1 0 1 0 1 E E D D C
                                            _ _ _ _ _ _ R R R R M
                                            P P C C B B R R E E D
                                            A A M M B B     Q Q R
                                            G G D D D D         E
                                            E E D D             Q
                                            D D
*/
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

/*
 DFC Ecc Control

  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 +---------------------------------+---------------+-----------+-+
 |                                 |               |           | |
 +---------------------------------+---------------+-----------+-+
               reserved               ECC_SPARE      ECC_THRESH B
                          C
                          H
                          _
                          E
                          N
*/
#define DFC_ECC_CTRL_BCH_EN_OFFS	0
#define DFC_ECC_CTRL_BCH_EN_MASK	(1 << DFC_ECC_CTRL_BCH_EN_OFFS)
#define DFC_ECC_CTRL_BCH_EN		(1 << DFC_ECC_CTRL_BCH_EN_OFFS)
#define DFC_ECC_CTRL_THRESH_OFFS	1
#define DFC_ECC_CTRL_THRESH_MASK	(0x3F << DFC_ECC_CTRL_BCH_EN_OFFS)
#define DFC_ECC_CTRL_SPARE_OFFS		7
#define DFC_ECC_CTRL_SPARE_MASK		(0xFF << DFC_ECC_CTRL_BCH_EN_OFFS)

/*ommand Buffer 0

  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 +-----+-+-------------------------------------------------------+
 |     | |                                                       |
 +-----+-+-------------------------------------------------------+
   CMD  L
    X   E
    T   N
    Y   _
    P   O
    E   V
        R
        D
*/
#define DFC_CB0_LEN_OVRD_OFFS		28
#define DFC_CB0_LEN_OVRD_MASK		(0x1 << DFC_CB0_LEN_OVRD_OFFS)
#define DFC_CB0_CMD_XTYPE_OFFS		29
#define DFC_CB0_CMD_XTYPE_MASK		(0x7U << DFC_CB0_CMD_XTYPE_OFFS)
#define DFC_CB0_CMD_XTYPE_MONOLITHIC	(0x0U << DFC_CB0_CMD_XTYPE_OFFS)
#define DFC_CB0_CMD_XTYPE_LAST_NAKED	(0x1U << DFC_CB0_CMD_XTYPE_OFFS)
#define DFC_CB0_CMD_XTYPE_MULTIPLE	(0x4U << DFC_CB0_CMD_XTYPE_OFFS)
#define DFC_CB0_CMD_XTYPE_NAKED		(0x5U << DFC_CB0_CMD_XTYPE_OFFS)
#define DFC_CB0_CMD_XTYPE_DISPATCH	(0x6U << DFC_CB0_CMD_XTYPE_OFFS)



/* Data flash controller register locations. */

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
#define MSAR_0_BOOT_DEV_BUS_WIDTH_8BIT		(0x0 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
#define MSAR_0_BOOT_DEV_BUS_WIDTH_16BIT		(0x1 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)
#define MSAR_0_BOOT_DEV_BUS_WIDTH_32BIT		(0x2 << MSAR_0_BOOT_DEV_BUS_WIDTH_OFFS)

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

/* NAND page size */
#define MSAR_0_NAND_PAGE_SZ_OFFS		11
#define MSAR_0_NAND_PAGE_SZ_MASK		(0x3 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_512B		(0x0 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_2KB			(0x1 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_4KB			(0x2 << MSAR_0_NAND_PAGE_SZ_OFFS)
#define MSAR_0_NAND_PAGE_SZ_8KB			(0x3 << MSAR_0_NAND_PAGE_SZ_OFFS)

/* NAND ECC */
#define MSAR_0_NAND_ECC_OFFS			14
#define MSAR_0_NAND_ECC_MASK			(0x3 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_4BIT			(0x0 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_8BIT			(0x1 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_12BIT			(0x2 << MSAR_0_NAND_ECC_OFFS)
#define MSAR_0_NAND_ECC_16BIT			(0x3 << MSAR_0_NAND_ECC_OFFS)

/* TCLK frequency selector (two bit field) */
#define MSAR_0_TCLK0_OFFS			28
#define MSAR_0_TCLK0_MASK			(0x1 << MSAR_0_TCLK0_OFFS)
#define MSAR_1_TCLK1_OFFS			8
#define MSAR_1_TCLK1_MASK			(0x1 << MSAR_1_TCLK1_OFFS)

/**************/
/* Device Bus */
/**************/
#define DEV_BOOTSCN_READ_PARAMS_REG		0x10000

#define DBSCN_RP_DEV_WIDTH_OFFS			30
#define DBSCN_RP_DEV_WIDTH_MASK			(0x3UL << DBSCN_RP_DEV_WIDTH_OFFS)

#define DEV_BOOTSCN_WRITE_PARAMS_REG		0x10004


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
#define CPU_CTRL_STATUS_REG			0x20104
#define PEX_LINK_DISABLE_OFFS			0
#define PEX_LINK_DISABLE_MASK			(0x1 << PEX_LINK_DISABLE_OFFS)

#define PEX_CMND_STATUS_REG			0x40004
#define PEX_IO_SPACE_ENABLE_OFFS		0
#define PEX_IO_SPACE_ENABLE_MASK		(0x1 << PEX_IO_SPACE_ENABLE_OFFS)
#define PEX_MEM_SPACE_ENABLE_OFFS		1
#define PEX_MEM_SPACE_ENABLE_MASK		(0x1 << PEX_MEM_SPACE_ENABLE_OFFS)
#define PEX_MASTER_ENABLE_OFFS			2
#define PEX_MASTER_ENABLE_MASK			(0x1 << PEX_MASTER_ENABLE_OFFS)

#define PEX_PHY_INDIRECT_ACC_REG		0x41b00

#define PEX_CONTROL_REG				0x41a00
#define PEX_CONF_ROOT_COMPLEX_OFFS		1
#define PEX_CONF_ROOT_COMPLEX_MASK		(0x1 << PEX_CONF_ROOT_COMPLEX_OFFS)
#define PEX_CONF_MAX_OUTSTANDING_OFFS		8
#define PEX_CONF_MAX_OUTSTANDING_MASK		(0x3 << PEX_CONF_MAX_OUTSTANDING_OFFS)
#define PEX_CONF_MAX_OUTSTANDING_ROOT_CMPLX	(0x0 << PEX_CONF_MAX_OUTSTANDING_OFFS)
#define PEX_CONF_MAX_OUTSTANDING_END_POINT	(0x2 << PEX_CONF_MAX_OUTSTANDING_OFFS)

#define PEX_FLOW_CTRL_REG			0x41A20
#define PEX_PSTD_HDR_FC_INIT_VAL_OFFS		0
#define PEX_PSTD_HDR_FC_INIT_VAL_MASK		(0xFF << PEX_PSTD_HDR_FC_INIT_VAL_OFFS)
#define PEX_PSTD_HDR_FC_INIT_VAL_ROOT_CMPLX	(0x04 << PEX_PSTD_HDR_FC_INIT_VAL_OFFS)
#define PEX_PSTD_HDR_FC_INIT_VAL_END_POINT	(0x01 << PEX_PSTD_HDR_FC_INIT_VAL_OFFS)

#define PEX_TL_CTRL_REG				0x41AB0
#define PEX_TL_CTRL_CMP_BUFF_NUM_OFFS		8
#define PEX_TL_CTRL_CMP_BUFF_NUM_MASK		(0xF << PEX_TL_CTRL_CMP_BUFF_NUM_OFFS)
#define PEX_TL_CTRL_CMP_BUFF_NUM_ROOT_CMPLX	(0x4 << PEX_TL_CTRL_CMP_BUFF_NUM_OFFS)
#define PEX_TL_CTRL_CMP_BUFF_NUM_END_POINT	(0x1 << PEX_TL_CTRL_CMP_BUFF_NUM_OFFS)

#define PEX_DBG_CTRL_REG			0x41A60
#define PEX_MSK_SOFT_RST_OFFS			12
#define PEX_MSK_SOFT_RST_MASK			(0x1 << PEX_MSK_SOFT_RST_OFFS)
#define PEX_SOFT_RST_REGFILE_RST_OFFS		13
#define PEX_SOFT_RST_REGFILE_RST_MASK		(0x1 << PEX_SOFT_RST_REGFILE_RST_OFFS)
#define PEX_LINK_DIS_REGFILE_RST_OFFS		14
#define PEX_LINK_DIS_REGFILE_RST_MASK		(0x1 << PEX_LINK_DIS_REGFILE_RST_OFFS)
#define PEX_MASK_LINK_DIS_OFFS			15
#define PEX_MASK_LINK_DIS_MASK			(0x1 << PEX_MASK_LINK_DIS_OFFS)
#define PEX_MSK_LNK_FAIL_OFFS			16
#define PEX_MSK_LNK_FAIL_MASK			(0x1 << PEX_MSK_LNK_FAIL_OFFS)
#define PEX_MSK_HOT_RST_OFFS			17
#define PEX_MSK_HOT_RST_MASK			(0x1 << PEX_MSK_HOT_RST_OFFS)
#define PEX_DIS_TL_FLUSH_OFFS			18
#define PEX_DIS_TL_FLUSH_MASK			(0x1 << PEX_DIS_TL_FLUSH_OFFS)
#define PEX_DIS_LF_REG_RST_OFFS			19
#define PEX_DIS_LF_REG_RST_MASK			(0x1 << PEX_DIS_LF_REG_RST_OFFS)
#define PEX_DIS_REG_RST_HOT_RST_OFFS		21
#define PEX_DIS_REG_RST_HOT_RST_MASK		(0x1 << PEX_DIS_REG_RST_HOT_RST_OFFS)

#define PEX_CAPABILITY_REG			0x40060
#define	PEX_DEVICE_TYPE_OFFS			20
#define PEX_DEVICE_TYPE_MASK			(0xF << PEX_DEVICE_TYPE_OFFS)
#define PEX_DEVICE_TYPE_ROOT_COMPLEX		(0x4 << PEX_DEVICE_TYPE_OFFS)
#define PEX_DEVICE_TYPE_END_POINT		(0x1 << PEX_DEVICE_TYPE_OFFS)

/*******/
/* SPI */
/*******/

#define SPI_IF_CTRL_REG				0x10600

#define SPI_IF_CONFIG_REG			0x10604
#define SPI_ADDRESS_SIZE_OFFS			8
#define SPI_ADDRESS_SIZE_MASK			(0x3 << SPI_ADDRESS_SIZE_OFFS)
#define SPI_ADDRESS_3_CYCLES			(0x2 << SPI_ADDRESS_SIZE_OFFS)
#define SPI_ADDRESS_4_CYCLES			(0x3 << SPI_ADDRESS_SIZE_OFFS)

/********/
/* SATA */
/********/
#define SATA_REGS_BASE                  	0x80000

/* reserved areas on the DRAM*/
#define MV_SATA_EDMA_REQUEST_BUFFER_START	_8K
#define MV_SATA_EDMA_REQUEST_BUFFER_SIZE	32 /* the size of single CRQB*/
#define MV_SATA_EDMA_REQUEST_BUFFER_END		((MV_SATA_EDMA_REQUEST_BUFFER_START) + (MV_SATA_EDMA_REQUEST_BUFFER_SIZE))
#define MV_SATA_EDMA_RESPONSE_BUFFER_START	MV_SATA_EDMA_REQUEST_BUFFER_END
#define MV_SATA_EDMA_RESPONSE_BUFFER_SIZE	8 /* the size of single CRPB*/

/********/
/* UART */
/********/
#define UART0_REG_OFFSET			0x12000
#define UART1_REG_OFFSET                        0x12100

/********/
/* GPIO */
/********/
#define GPIO_DATA_OUT_REG			0x18100
#define GPIO_TEST_PIN_OFFS			7
#define GPIO_TEST_PIN_MASK			(0x1 << GPIO_TEST_PIN_OFFS)

/*******/
/* PMU */
/*******/
/*	*/
/*  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1		*/
/*  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0		*/
/* +-----------+-+-+---------+-+-+-+-------------------------+-+-+-+		*/
/* |  RESERVED | | |RESERVED | | | |         RESERVED        | | | |		*/
/* +-----------+-+-+---------+-+-+-+-------------------------+-+-+-+		*/
/*              L H           D D P                           D D P		*/
/*              T T           V F W                           V F W		*/
/*              H H           S S R                           S S R		*/
/*              R R                                                		*/
/*                            M M M                           D D U		*/
/*              M M           A A A                           O O P		*/
/*              S S           S S S                           N N 		*/
/*              K K           K K K                           E E 		*/
/*		*/
#define PMU_EVENT_STATUS_L2C_REG		0x22020

/*  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1		*/
/*  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0		*/
/* +----------+-+-+---------+-+-+-+-----------+-+-+---------+-+-+-+		*/
/* |  RESERVED | | |RESERVED | | | |  RESERVED | | | RESERVED| | | |		*/
/* +-----------+-+-+---------+-+-+-+-----------+-+-+---------+-+-+-+		*/
/*              L H           D D P             L H           D D P		*/
/*              T T           V F W             T T           V F W		*/
/*              H H           S S R             H H           S S R		*/
/*              R R                             R R                		*/
/*                            M M M             S S           D D U		*/
/*              M M           A A A             H H           O O P		*/
/*              S S           S S S             L L           N N 		*/
/*              K K           K K K             D D           E E 		*/
/*		*/
#define PMU_EVENT_STATUS_REG(CPUID)		(0x22120 + ((CPUID) * 0x100))

#define PMU_RESUME_TYPE_OFFS			0
#define PMU_RESUME_TYPE_MASK			(0x1 << PMU_RESUME_TYPE_OFFS)
#define PMU_RESUME_TYPE_NONE			(0x0 << PMU_RESUME_TYPE_OFFS)
#define PMU_RESUME_TYPE_STBY			(0x1 << PMU_RESUME_TYPE_OFFS)

/*  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1		*/
/*  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0		*/
/* +---------------------------------------------------------------+		*/
/* |            RESUME_ADDR (BOOT ADDRESS REDIRECT)                |		*/
/* +---------------------------------------------------------------+		*/
#define PMU_RESUME_ADDR_REG(CPUID)		(0x22124+ ((CPUID) * 0x100))

/*  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0		*/
/* +---------------------------------------------------------------+		*/
/* |                ADDRESS OF DESCRIPTORS ARRAY                   |		*/
/* +---------------------------------------------------------------+		*/
/*		*/
#define PMU_RESUME0_CTRL_REG			0x20980
#define PMU_RESUME1_CTRL_REG			0x20988

/*  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1		*/
/*  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0		*/
/* +---------------------------------+---------------------+-------+		*/
/* |         CONFIGS_US_DLY          |    CONFIGS_CNT      |  TYP  |		*/
/* +---------------------------------|---------------------+-------+		*/
/*		*/
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
#if 0
#define TWSI_DATA_REG				0x11004
#define TWSI_DATA_COMMAND_OFFS			0x0
#define TWSI_DATA_COMMAND_MASK 			(0x1 << TWSI_DATA_COMMAND_OFFS)
#define TWSI_DATA_ADDR_7BIT_ADDR		0xA0 /* device address 0 */

#define TWSI_CONTROL_REG				0x11008
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

/*#define TWSI_CPU_MAIN_INT_CAUSE_REG		0x20200		*/
/*#define TWSI_CPU_MAIN_INT_TWSI_OFFS		29 */ /*5*/

#define TWSI_CPU_MAIN_INT_TWSI_BIT		(1 << TWSI_CPU_MAIN_INT_TWSI_OFFS)

#define CPU_INT_SOURCE_CONTROL_ENA_OFFS		28
#define CPU_INT_SOURCE_CONTROL_ENA_MASK		(1 << CPU_INT_SOURCE_CONTROL_ENA_OFFS)
#define CPU_MAIN_INT_TWSI_OFFS(i)			(2 + i)
#define CPU_MAIN_INT_CAUSE_TWSI(i)			(31 + i)
#define MV_MBUS_REGS_OFFSET					(0x20000)
#define MV_CPUIF_SHARED_REGS_BASE			(MV_MBUS_REGS_OFFSET)
#define CPU_INT_SOURCE_CONTROL_REG(i)		(MV_CPUIF_SHARED_REGS_BASE + 0xB00 + (i * 0x4))


/* TWSI */

/*
** Specific definition for Main CPU interrupt cause register.
** Needed for TWSI operation completion monitoring.
*/
/*#define MV_TWSI_CPU_MAIN_INT_CASUE(chNum)			*/


/*#define MV_TWSI_SLAVE_REGS_OFFSET(chanNum)	(0x11000 + (chanNum * 0x100))		*/
/*
** Base address for TWSI registers.
*/
/*#define MV_TWSI_SLAVE_REGS_BASE(unit) 		(MV_TWSI_SLAVE_REGS_OFFSET(unit))		*/

/*
** Specific definition for Main CPU interrupt cause register.
** Needed for TWSI operation completion monitoring.
*/
/*#define MV_TWSI_CPU_MAIN_INT_CASUE(chNum)	TWSI_CPU_MAIN_INT_CAUSE_REG		*/
/*
#define TWSI_CONTROL_REG(chanNum)			(MV_TWSI_SLAVE_REGS_BASE(chanNum) + 0x08)
#define TWSI_CONTROL_ACK            		BIT2
#define TWSI_CONTROL_INT_FLAG_SET   		BIT3
#define TWSI_CONTROL_STOP_BIT    			BIT4
#define TWSI_CONTROL_START_BIT 				BIT5
#define TWSI_CONTROL_ENA     				BIT6
#define TWSI_CONTROL_INT_ENA    			BIT7
*/

#define TWSI_DATA_ADDR_MASK		0x7
#define TWSI_DATA_ADDR_OFFS		1

#define MV_CPUIF_REGS_OFFSET(cpu)		(0x21000 + (cpu) * 0x100)
#define MV_CPUIF_REGS_BASE(cpu)				(MV_CPUIF_REGS_OFFSET(cpu))
#define CPU_MAIN_INT_CAUSE_REG(vec, cpu)	(MV_CPUIF_REGS_BASE(cpu) + 0x880 + (vec * 0x4))
#define CPU_MAIN_INT_TWSI_OFFS(i)			(2 + i)
#define CPU_MAIN_INT_CAUSE_TWSI(i)			(31 + i)


#define TWSI_CPU_MAIN_INT_CAUSE_REG		CPU_MAIN_INT_CAUSE_REG(1, 0)
 /*whoAmI()) */
#define TWSI0_CPU_MAIN_INT_BIT(ch)		((ch) + 3)

#endif
/************/
/* Security */
/************/
#ifdef MV_EFUSE_IN_RAM
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
	#define EFUSE_GET_SECURE_BOOT		((MV_REG_READ(SEC_BOOT_CTRL_REG) \
			& SEC_BOOT_MODE_MASK)  >> SEC_BOOT_MODE_OFFS)
	#define EFUSE_GET_JTAG_DELAY		((MV_REG_READ(SEC_BOOT_CTRL_REG) \
			& SEC_JTAG_DSBL_MASK)  >> SEC_JTAG_DSBL_OFFS)
	#define EFUSE_GET_BOOT_SOURCE		((MV_REG_READ(SEC_BOOT_CTRL_REG) \
			& SEC_BOOT_DEV_MASK)  >> SEC_BOOT_DEV_OFFS)
#endif

#endif /* _SOC_SPEC_H */
