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

#include <asm/arch/regs-base.h>

struct adec_win {
	u8 target_id;
	uintptr_t base_addr;
	uintptr_t win_size;
	u8 win_type;
	u8 enabled;
};

enum adec_target_ids_ap {
	IO_0_TID	= 0x00,
	DRAM_0_TID	= 0x03,
	IO_1_TID	= 0x0F,
	CFG_REG_TID	= 0x10,
	RAR_TID		= 0x20,
	SRAM_TID	= 0x40,
	DRAM_1_TID	= 0xC0,
};

enum adec_target_ids_iob {
	INTERNAL_TID	= 0x0,
	IOB_TID		= 0x1,
	IHB1_TID	= 0x2,
	PEX0_TID	= 0x3,
	PEX1_TID	= 0x4,
	PEX2_TID	= 0x5,
	PEX3_TID	= 0x6,
	NSS_TID		= 0x7,
	NAND_TID	= 0x8,
	RUNIT_TID	= 0x9,
	INVALID_TID	= 0xFF
};

enum adec_win_type {
	ADEC_AP_WIN,
	ADEC_IOB_WIN,
	ADEC_RFU_WIN,
};

/* common defines */
#define WIN_ENABLE_BIT			(0x1)
#define ADDRESS_SHIFT			(20)
#define ADDRESS_MASK			(0xFFFFFFF0)
#define CR_WIN_SIZE_ALIGNMENT		(0x10000)

/* AP registers */
#define MAX_AP_WINDOWS			(8)

#define AP_WIN_CR_OFFSET(win)		(MVEBU_ADEC_AP_BASE + 0x0 + (0x10 * win))
#define AP_TARGET_ID_OFFSET		(8)
#define AP_TARGET_ID_MASK		(0x7F)

#define AP_WIN_SCR_OFFSET(win)		(MVEBU_ADEC_AP_BASE + 0x4 + (0x10 * win))
#define AP_WIN_ENA_READ_SECURE		(0x1)
#define AP_WIN_ENA_WRITE_SECURE		(0x2)

#define AP_WIN_ALR_OFFSET(win)		(MVEBU_ADEC_AP_BASE + 0x8 + (0x10 * win))
#define AP_WIN_AHR_OFFSET(win)		(MVEBU_ADEC_AP_BASE + 0xC + (0x10 * win))

#define AP_WIN_GCR_OFFSET		(MVEBU_ADEC_AP_BASE + 0xD0)

/* RFU registers */
#define MAX_RFU_WINDOWS			3
#define BOOTROM_RFU_WINDOW_NUM		0
#define STM_RFU_WINDOW_NUM		1
#define SPI_RFU_WINDOW_NUM		2


#define RFU_WIN_ALR_OFFSET(win)		(MVEBU_ADEC_RFU_BASE + 0x0 + (0x10 * win))
#define RFU_WIN_AHR_OFFSET(win)		(MVEBU_ADEC_RFU_BASE + 0x8 + (0x10 * win))

/* IOB registers */
#define MAX_IOB_WINDOWS			(23)
#define INTERNAL_REG_WIN_NUM		(0)

#define IOB_WIN_CR_OFFSET(win)		(MVEBU_ADEC_IOB_BASE + 0x0 + (0x32 * win))
#define IOB_TARGET_ID_OFFSET		(8)
#define IOB_TARGET_ID_MASK		(0xF)

#define IOB_WIN_SCR_OFFSET(win)		(MVEBU_ADEC_IOB_BASE + 0x4 + (0x32 * win))
#define IOB_WIN_ENA_CTRL_WRITE_SECURE	(0x1)
#define IOB_WIN_ENA_CTRL_READ_SECURE	(0x2)
#define IOB_WIN_ENA_WRITE_SECURE	(0x4)
#define IOB_WIN_ENA_READ_SECURE		(0x8)

#define IOB_WIN_ALR_OFFSET(win)		(MVEBU_ADEC_IOB_BASE + 0x8 + (0x32 * win))
#define IOB_WIN_AHR_OFFSET(win)		(MVEBU_ADEC_IOB_BASE + 0xC + (0x32 * win))


void adec_dump(void);
int adec_init(struct adec_win *windows);
