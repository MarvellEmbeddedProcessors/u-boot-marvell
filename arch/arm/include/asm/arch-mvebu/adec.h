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

#ifdef CONFIG_MVEBU_CA9
#include <asm/arch-mvebu/adec_mbus.h>
#else
#include <asm/arch-mvebu/adec_ccu.h>
#endif

/* Macro for testing alignment. Positive if number is NOT aligned */
#define IS_NOT_ALIGN(number, align)	((number) & ((align) - 1))

/* Macro for alignment up. For example, ALIGN_UP(0x0330, 0x20) = 0x0340 */
#define ALIGN_UP(number, align) (((number) & ((align) - 1)) ? \
		(((number) + (align)) & ~((align)-1)) : (number))

void adec_dump(void);
int adec_init(struct adec_win *windows);
int adec_remap(phys_addr_t input, phys_addr_t output);

#endif /* _ADEC_H_ */
