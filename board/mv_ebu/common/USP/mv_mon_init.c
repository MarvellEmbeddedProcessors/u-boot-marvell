/*
 * (C) Copyright 2001
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
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

 /*
 * Dink_command.c - driver for special Marvell coammnds taken from Dink.
 */

#include <config.h>
#include <common.h>
#include <command.h>

#include "mvSysHwConfig.h"
#if defined(MV_INCLUDE_MONT_EXT)

#include "mvTypes.h"
#include "mvCtrlEnvLib.h"
#include "mvCpuIf.h"
#include "cpu/mvCpu.h"

#if defined(MV_INCLUDE_MONT_MMU)
extern void pageTableInit(void);
#endif
#if defined(MV_INCLUDE_MONT_MPU)
extern void MPU_Init(void);
#endif

#if defined(MV_INCLUDE_MONT_MMU)

/***********************************************************************************/
/* hook function that is called after CPU detection and Serial init and before     */
/* initializing of the DRAM.							   */
/***********************************************************************************/

int PTexist(void)
{
	char *env;
#ifdef MV_PT
	env = getenv("enaPT");
#ifdef CONFIG_MV_LE
	/* if LE page table disable is the default. ( MIPS - unstable, PPC need to change the page settings) */
	if( ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) ){
#else
	/* if BE page table enable is the default. */
	if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) ){
#endif
		return 1;
	}
#endif
	return 0;
}
#endif
/***********************************************************************************/
/* hook function that is called after rellocating to the DRAM, Flash Init, PCI init*/
/* and malloc init are done, and before Gig port init.				   */
/***********************************************************************************/
void mon_extension_after_relloc(void)
{
#if defined(MV_INCLUDE_MONT_FFS)
	unsigned int status;
#endif

#if defined(MV_INCLUDE_MONT_MMU)
	if ((mvOsCpuPartGet() == CPU_PART_ARM926)  ||
		(mvOsCpuPartGet() == CPU_PART_MRVL131) ||
		(mvOsCpuPartGet() == CPU_PART_MRVLPJ4B_UP) ||
		(mvOsCpuPartGet() == CPU_PART_MRVLPJ4B_MP))
	{
		/* Page Table */
		if(PTexist() )
		{
			pageTableInit();

			setenv("enaPT","yes");
		}
		else
			setenv("enaPT","no");
	}
#endif /* MV_INCLUDE_MONT_MMU */

#if defined(MV_INCLUDE_MONT_MPU)
	else if (mvOsCpuPartGet() == CPU_PART_ARM946)
	{
		char *env;
		env = getenv("enaMPU");

		if ((!env)||( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ))
		{
				setenv("enaMPU","yes");
				MPU_Init();
		}
	}
#endif /* MV_INCLUDE_MONT_MPU */


#if defined(CONFIG_CMD_BSP)
	#if defined(MV_INCLUDE_MONT_FFS)
    if (whoAmI() == MASTER_CPU)
	{
	status = mvFSInit(FS_NO_CACHE);


	if(!(status == FS_NO_VALID_FAT_STRING || status == FS_OFFSET_OUT_OF_RANGE))
	{
		printf("File system present and initialized on the main Flash Memory\n");
		}
	}
	else
	{
		printf("No File system on the main Flash Memory\n");
	}
	#endif /* MV_INCLUDE_MONT_FFS */
#endif

	return;
}
#endif /* #if defined(MV_INCLUDE_MONT_EXT) */
