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

#ifndef __INCmvMONEXTh
#define __INCmvMONEXTh

#ifdef __cplusplus
extern "C" {
#endif

/************************************************/
/* U-Boot Specific				*/
/************************************************/
#define MV_INCLUDE_MONT_EXT

#if defined(MV_INCLUDE_MONT_EXT)
#define MV_INCLUDE_MONT_MMU
#if defined(MV_INC_BOARD_NOR_FLASH)
#define MV_INCLUDE_MONT_FFS
#endif
#endif

void mon_extension_before_relloc(void);
void mon_extension_after_relloc(void);

#ifdef __cplusplus
}
#endif

#endif /* __INCmvMONEXTh */
