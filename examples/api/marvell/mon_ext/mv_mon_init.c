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
#include <api_public.h>

#include "mv_mon_init.h"
#if defined(MV_INCLUDE_MONT_EXT)

#include "mvTypes.h"
#include "mvCtrlEnvLib.h"
#include "mvCpuIf.h"
#include "cpu/mvCpu.h"
#include "../../glue.h"
#include "mvBoardEnvLib.h"

#if defined(MV_INCLUDE_MONT_MMU)
extern void pageTableInit(void);
extern void detectPageTable(void);
#endif
#if defined(MV_INCLUDE_MONT_MPU)
extern void MPU_Init(void);
#endif

#if defined(MV_INCLUDE_MONT_MMU)

void add_monext_command(void);

/***********************************************************************************/
/* hook function that is called after CPU detection and Serial init and before     */
/* initializing of the DRAM.							   */
/***********************************************************************************/

int PTexist(void)
{
	char *env;
#ifdef MV_PT
	env = ub_env_get("enaPT");
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

int main(void)
{
#if defined(MV_INCLUDE_MONT_FFS)
	unsigned int status;
#endif
	struct api_signature *sig = NULL;

	if (!api_search_sig(&sig))
		return -1;

	syscall_ptr = sig->syscall;
	if (syscall_ptr == NULL)
		return -2;

	if (sig->version > API_SIG_VERSION)
		return -3;
	printf("API signature found @%x\n", (unsigned int)sig);

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

			ub_env_set("enaPT","yes");
		}
		else
			ub_env_set("enaPT","no");
	}
#endif /* MV_INCLUDE_MONT_MMU */
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
	add_monext_command();
	return 1;
}


/******************************************************************************
* Functionality- Go to an address and execute the code there and return,
*    defualt address is 0x40004
*****************************************************************************/
extern void cpu_dcache_flush_all(void);
extern void cpu_icache_flush_invalidate_all(void);

void mv_go(unsigned long addr,int argc, char * const argv[])
{
	addr = MV_CACHEABLE(addr);
	char* envCacheMode = ub_env_get("cacheMode");

	/*
	 * pass address parameter as argv[0] (aka command name),
	 * and all remaining args
	 */

    if(envCacheMode && (strcmp(envCacheMode,"write-through") == 0))
	{
		int i=0;

		/* Flush Invalidate I-Cache */
		cpu_icache_flush_invalidate_all();

		/* Drain write buffer */
		asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i));


	}
	else /*"write-back"*/
	{
		int i=0;

		/* Flush Invalidate I-Cache */
		cpu_icache_flush_invalidate_all();

		/* Drain write buffer */
		asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i));


		/* Flush invalidate D-cache */
		cpu_dcache_flush_all();


    }
	return;
}

int g_cmd (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
       ulong   addr;
       addr = 0x40000;

        if (argc > 1) {
               addr = simple_strtoul(argv[1], NULL, 16);
        }
       mv_go(addr,argc, argv);
       return 1;
}

/******************************************************************************
* Functionality- Searches for a value
*****************************************************************************/
int fi_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    MV_U32 s_address,e_address,value,i,tempData;
    MV_BOOL  error = MV_FALSE;

    if(argc == 4){
	value = simple_strtoul(argv[1], NULL, 16);
	s_address = simple_strtoul(argv[2], NULL, 16);
	e_address = simple_strtoul(argv[3], NULL, 16);
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
	return 0;
    }

    if(s_address == 0xffffffff || e_address == 0xffffffff) error = MV_TRUE;
    if(s_address%4 != 0 || e_address%4 != 0) error = MV_TRUE;
    if(s_address > e_address) error = MV_TRUE;
    if(error)
    {
	printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    for(i = s_address; i < e_address ; i+=4)
    {
        tempData = (*((volatile unsigned int *)i));
        if(tempData == value)
        {
            printf("Value: %x found at ",value);
            printf("address: %x\n",i);
            return 1;
        }
    }
    printf("Value not found!!\n");
    return 1;
}

/******************************************************************************
* Functionality- Compare the memory with Value.
*****************************************************************************/
int cmpm_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    MV_U32 s_address,e_address,value,i,tempData;
    MV_BOOL  error = MV_FALSE;
 
    if(argc == 4){
	value = simple_strtoul(argv[1], NULL, 16);
	s_address = simple_strtoul(argv[2], NULL, 16);
	e_address = simple_strtoul(argv[3], NULL, 16);
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
	return 0;
    }

    if(s_address == 0xffffffff || e_address == 0xffffffff) error = MV_TRUE;
    if(s_address%4 != 0 || e_address%4 != 0) error = MV_TRUE;
    if(s_address > e_address) error = MV_TRUE;
    if(error)
    {
	printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    for(i = s_address; i < e_address ; i+=4)
    {
        tempData = (*((volatile unsigned int *)i));
        if(tempData != value)
        {
            printf("Value: %x found at address: %x\n",tempData,i);
        }
    }
    return 1;
}

extern int PTexist(void);
int cpumap_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        printf("CPU Memory mapping :\n");
	if ((mvOsCpuPartGet() == CPU_PART_ARM926)    ||
		(mvOsCpuPartGet() == CPU_PART_MRVL_A9)   ||
		(mvOsCpuPartGet() == CPU_PART_MRVL131)   ||
		(mvOsCpuPartGet() == CPU_PART_MRVLPJ4B_UP) ||
		(mvOsCpuPartGet() == CPU_PART_MRVLPJ4B_MP))
		{
			if(PTexist())
			{
				detectPageTable();
			}
			else
				printf("No page table. \n");
		}

#if defined(MV_INCLUDE_MONT_EXT) && defined (MV_INCLUDE_MONT_MPU)
		else if (mvOsCpuPartGet() == CPU_PART_ARM946)
		{
			mpuMap();
		}
#endif

	return 1;
}

void add_monext_command(void)
{
	cmd_tbl_t newcmd;
	newcmd.name = "cmpm";
	newcmd.maxargs = 4;
	newcmd.repeatable = 1;
	newcmd.cmd = cmpm_cmd;
	newcmd.usage = "cmpm	- Compare Memory\n";
	newcmd.help = " value start_address end_address.\n"
	"\tCompare the memory from address 'start_address to address 'end_address'.\n"
	"\twith value 'value'\n";
	ub_add_command(&newcmd);
	
	newcmd.name = "fiext";
	newcmd.maxargs = 4;
	newcmd.repeatable = 1;
	newcmd.cmd = fi_cmd;
	newcmd.usage = "fi	- Find value in the memory.\n";
	newcmd.help = " value start_address end_address\n"
	"\tSearch for a value 'value' in the memory from address 'start_address to\n"
	"\taddress 'end_address'.\n";
	ub_add_command(&newcmd);
	
	newcmd.name = "g";
	newcmd.maxargs = CONFIG_SYS_MAXARGS;
	newcmd.repeatable = 1;
	newcmd.cmd = g_cmd;
	newcmd.usage = "g     - start application at cached address 'addr'(default addr 0x40000)\n";
	newcmd.help = " addr [arg ...] \n"
       "\tStart application at address 'addr' cachable!!!(default addr 0x40004/0x240004)\n"
       "\tpassing 'arg' as arguments\n";
	ub_add_command(&newcmd);

	newcmd.name = "cpumap";
	newcmd.maxargs = 1;
	newcmd.repeatable = 1;
	newcmd.cmd = cpumap_cmd;
	newcmd.usage = "cpumap - Display CPU memory mapping settings.\n";
	newcmd.help = "  \n"
        "\tdisplay CPU memory mapping settings.\n";
	ub_add_command(&newcmd);

}
#endif /* #if defined(MV_INCLUDE_MONT_EXT) */

