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
/*******************************************************************************
* mvSysCesaConfig.h - Marvell Cesa unit specific configurations
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#include "mvSysHwConfig.h"

/*
** Base address for cesa registers.
*/
#define OLD_CESA_HAL

#if defined OLD_CESA_HAL
#warning "Working with old CESA HAL (one channel only!)"
#define MV_CESA_REGS_BASE		(MV_CESA_REGS_OFFSET(0))
#define MV_CESA_TDMA_REGS_BASE		(MV_CESA_TDMA_REGS_OFFSET(0))
#else
#define MV_CESA_TDMA_REGS_BASE(port)	(MV_CESA_TDMA_REGS_OFFSET(port))
#define MV_CESA_REGS_BASE(port) 	(MV_CESA_REGS_OFFSET(port))
#endif
