/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

struct adec_win {
	u32 target;
	uintptr_t base_addr;
	uintptr_t win_size;
	u32 attribute;
	u32 remapped;
	u32 enabled;
};

enum mbus_target_id {
	TBL_TERM		= -1,
	DRAM_TARGET_ID		= 0,	/* Port 0 -> DRAM interface             */
	DEV_TARGET_ID		= 1,	/* Port 1 -> Device port, BootROM, SPI  */
	PEX_1_3_TARGET_ID	= 4,	/* Port 4 -> PCI Express 1 and 3        */
	PEX_0_TARGET_ID		= 8,	/* Port 4 -> PCI Express 0 and 2        */
	CRYPT_TARGET_ID		= 10,	/* Port 9 --> Crypto Engine SRAM        */
	PNC_BM_TARGET_ID	= 12,	/* Port 12 -> PNC + BM Unit             */
	INVALID_TARGET_ID	= 0xFF,
	MAX_TARGETS_ID
};

enum mbus_attribute {
	/* Memory chip select */
	MEM_CS_0_ATTR		= 0x0E,
	MEM_CS_1_ATTR		= 0x0D,
	MEM_CS_2_ATTR		= 0x0B,
	MEM_CS_3_ATTR		= 0x07,
	CS_DEC_BY_ATTR		= 0x0F,
	/* Device bus interface */
	SPI0_CS0_ATTR		= 0x1E,
	SPI0_CS1_ATTR		= 0x5E,
	SPI0_CS2_ATTR		= 0x9E,
	SPI0_CS3_ATTR		= 0xDE,
	SPI1_CS0_ATTR		= 0x1A,
	SPI1_CS1_ATTR		= 0x5A,
	SPI1_CS2_ATTR		= 0x9A,
	SPI1_CS3_ATTR		= 0xDA,
	DEVICE_CS0_ATTR		= 0x3E,
	DEVICE_CS1_ATTR		= 0x3D,
	DEVICE_CS2_ATTR		= 0x3B,
	DEVICE_CS3_ATTR		= 0x37,
	BOOT_CS_ATTR		= 0x2F,
	BOOTROM_ATTR		= 0x1D,
	/* PCI Express interface */
	PCI_0_IO_ATTR		= 0xE0,
	PCI_0_MEM_ATTR		= 0xE8,
	PCI_1_IO_ATTR		= 0xE0,
	PCI_1_MEM_ATTR		= 0xE8,
	PCI_2_IO_ATTR		= 0xD0,
	PCI_2_MEM_ATTR		= 0xD8,
	PCI_3_IO_ATTR		= 0xB0,
	PCI_3_MEM_ATTR		= 0xB8,
	/* Security Accelerator SRAM */
	ENGINE_0_BYTES_ATTR	= 0x04,
	ENGINE_0_NOS_ATTR	= 0x05,
	ENGINE_0_BYTE_WORDS_ATTR= 0x06,
	ENGINE_0_WORDS_ATTR	= 0x07,
	ENGINE_1_BYTES_ATTR	= 0x08,
	ENGINE_1_NOS_ATTR	= 0x09,
	ENGINE_1_BYTE_WORDS_ATTR= 0x0A,
	ENGINE_1_WORDS_ATTR	= 0x0B,
	NOT_VALID_ATTR		= 0xFF,
};

#define MAX_MBUS_WINS				19
#define MAX_MBUS_REMAP_WINS			8
#define INTERNAL_REG_WIN_NUM			20


#define MBUS_WIN_CTRL_REG(win_num)		((win_num < MAX_MBUS_REMAP_WINS) ? \
		(win_num * 0x10) : (0x90 + (win_num-8)*0x08))
#define MBUS_CR_WIN_ENABLE			0x1
#define MBUS_CR_WIN_TARGET_OFFS			4
#define MBUS_CR_WIN_TARGET_MASK			(0xf << MBUS_CR_WIN_TARGET_OFFS)
#define MBUS_CR_WIN_ATTR_OFFS			8
#define MBUS_CR_WIN_ATTR_MASK			(0xff << MBUS_CR_WIN_ATTR_OFFS)
#define MBUS_CR_WIN_SIZE_OFFS			16
#define MBUS_CR_WIN_SIZE_MASK			(0xffff << MBUS_CR_WIN_SIZE_OFFS)
#define MBUS_CR_WIN_SIZE_ALIGNMENT		0x10000

#define MBUS_WIN_BASE_REG(win_num)		((win_num < MAX_MBUS_REMAP_WINS) ? \
		(0x4 + win_num*0x10) :	(0x94 + (win_num-8)*0x08))
#define MBUS_BR_BASE_OFFS			16
#define MBUS_BR_BASE_MASK			(0xffff << 	MBUS_BR_BASE_OFFS)

#define MBUS_WIN_REMAP_LOW_REG(win_num)		((win_num < MAX_MBUS_REMAP_WINS) ? \
		(0x8 + win_num*0x10) : (0))
#define MBUS_RLR_REMAP_LOW_OFFS			16
#define MBUS_RLR_REMAP_LOW_MASK			(0xffff << MBUS_RLR_REMAP_LOW_OFFS)

#define MBUS_WIN_REMAP_HIGH_REG(win_num)	((win_num < MAX_MBUS_REMAP_WINS) ? \
		(0xC + win_num*0x10) : (0))
#define MBUS_RHR_REMAP_HIGH_OFFS		0
#define MBUS_RHR_REMAP_HIGH_MASK		(0xffffffff << MBUS_RHR_REMAP_HIGH_OFFS)

#define MBUS_WIN_INTEREG_REG			(0x80)

#define MBUS_SDRAM_BASE_REG(win)		(MVEBU_ADEC_BASE + 0x180 + (win * 0x8))
#define MBUS_SDRAM_CTRL_REG(win)		(MVEBU_ADEC_BASE + 0x184 + (win * 0x8))
#define MBUS_SDRAM_SIZE_MASK			(0xFF << 24)
#define MBUS_SDRAM_SIZE_ALIGN			(1 << 24)

