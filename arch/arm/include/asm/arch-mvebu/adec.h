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

#ifndef _ADEC_H_
#define _ADEC_H_

#define RAR_EN_OFFSET		(13)
#define RAR_EN_MASK		(1)
#define TARGET_ID_OFFSET	(8)
#define TARGET_ID_MASK		(0x1F)
#define WIN_ENABLE_BIT		(0x1)

#define ADDRESS_SHIFT		(20)
#define ADDRESS_MASK		(0xFFFFFFF0)

#define MAX_AP_WINDOWS		(8)
#define MAX_CP_WINDOWS		(16)

#define CCU_WIN_CR_OFFSET(win)	(0 + (12 * win))
#define CCU_WIN_ALR_OFFSET(win)	(4 + (12 * win))
#define CCU_WIN_AHR_OFFSET(win)	(8 + (12 * win))

enum adec_target_ids_ap {
	IO_0_TID    = 0x0,
	DRAM_0_TID  = 0x3,
	DRAM_1_TID  = 0x6,
	IO_1_TID    = 0x9,
	CFG_REG_TID = 0x10,
	INVALID_TID = 0x1F
};

struct adec_win {
	uintptr_t base_addr;
	uintptr_t win_size;
	u8 target_id;
	bool rar_enable;
};


void adec_dump(void);
int adec_ap_init(struct adec_win *windows, void __iomem *ap_base);

#endif /* _ADEC_H_ */
