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
* mvSysTdmConfig.h - Marvell TDM unit specific configurations
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#include "mvSysHwConfig.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvOs.h"

/****************************************************************/
/*************** Telephony configuration ************************/
/****************************************************************/
/* U-Boot support*/
#define MV_TDM_SUPPORT
//undef MV_TDM_SUPPORT for 32ch unit support (COMM unit)
#if defined(CONFIG_MV_TDM_SUPPORT)
	#define MV_TDM_SUPPORT
	#define MV_TDM_REGS_BASE	MV_TDM_REGS_OFFSET
#elif defined(CONFIG_MV_COMM_UNIT_SUPPORT)
	#define MV_COMM_UNIT_SUPPORT
	#define MV_COMM_UNIT_REGS_BASE	MV_COMM_UNIT_REGS_OFFSET
#endif

/* SLIC vendor */
#if defined(CONFIG_SILABS_SLIC_SUPPORT)
	#define SILABS_SLIC_SUPPORT
	#if defined(CONFIG_SILABS_SLIC_3215)
		#define SILABS_SLIC_3215
	#elif defined(CONFIG_SILABS_SLIC_3217)
		#define SILABS_SLIC_3217
	#endif
#elif defined(CONFIG_ZARLINK_SLIC_SUPPORT)
	#define ZARLINK_SLIC_SUPPORT
	#if defined(CONFIG_ZARLINK_SLIC_VE880)
		#define ZARLINK_SLIC_VE880
		#define SLIC_TIMER_EVENT_SUPPORT
	#elif defined(CONFIG_ZARLINK_SLIC_VE792)
		#define ZARLINK_SLIC_VE792
	#endif
#else
	#define SILABS_SLIC_3215_OLD_SUPPORT
#endif

#if defined(CONFIG_MV_TDM_USE_EXTERNAL_PCLK_SOURCE)
 #define MV_TDM_USE_EXTERNAL_PCLK_SOURCE
#endif
