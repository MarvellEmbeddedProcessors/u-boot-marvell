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

/* Code for setting up pagetables or the protection unit,
 * and enabling the cache. */

#include <config.h>
#include <common.h>
#include <asm/types.h>
#include <command.h>

#include "mv_mon_init.h"
#if defined(MV_INCLUDE_MONT_EXT) && defined (MV_INCLUDE_MONT_MMU)

#include "mvCpuIf.h"
#include "cpu/mvCpu.h"

inline void cpuCfgEnable(unsigned long flags, MV_BOOL enable);

extern char *		ub_env_get(const char *name);
extern void		ub_env_set(const char *name, char *value);
extern unsigned int whoAmI(void);

int mpuMap(void);

/* This file refers to the A.R.M.--The ARM Architecture Reference Manual */

enum access
{
	NO_ACCESS,
	NO_USR_W,
	SVC_RW,
	ALL_ACCESS
};

enum entrytype
{
	INVALID,
	PAGE,
	SECTION
};

#define U_BIT 0 
#define C_BIT (1 << 3)
#define B_BIT (1 << 2)
#define S_BIT (1 << 16)
#define TEX0_BIT	(1 << 12)
#define TEX1_BIT	(1 << 13)
#define TEX2_BIT	(1 << 14)

#define WRITE_BACK	(C_BIT|B_BIT)
#define WRITE_THROUGH	(C_BIT)

#define L1Entry(type,addr,dom,ucb,acc) \
  ( (type == SECTION) ? ( ((addr) & 0xfff00000) |        \
                          ((acc) << 10) | ((dom) << 5) | \
                          (ucb) | (type) ) :             \
    (type == PAGE) ? ( ((addr) &0xfffffc00) |            \
                       ((dom) << 5) |                    \
                       ((ucb) & U_BIT) | (type) ) :      \
    0)

#define L1EntryAddr(type, entry) \
  ( (type == SECTION) ? ((entry) & 0xfff00000): \
    (type == PAGE) ? ((entry) &0xfffffc00):0)

#define L1EntryAcc(type, entry) \
  (((entry) & 0xc00) >> 10)

#define L1EntryUcb(type, entry) \
  ((entry) & 0x1c)

#define L1EntryType(type, entry) \
  ((entry) & 0x3)

void detectPageTable(void)
{
	int i;
	unsigned int cpu = whoAmI();
	unsigned int *p = (unsigned int *)MV_PT_BASE(cpu);
	unsigned int entry;
	unsigned int ucb;
	unsigned int acc;
	unsigned int nextEntry;
	char* envCacheMode = (char *)ub_env_get("cacheMode");
	unsigned int startVirAddr = 0;
	unsigned int startPhyAddr = 0;


	if(envCacheMode && (strcmp(envCacheMode,"write-through") == 0))
	{

		printf("Cache Mode - write-through\n");
	}
	else /*"write-back"*/
	{
		printf("Cache Mode - write-back\n");
	}

	printf("page table:\n");
	for (i = 0; i < 4096; i++)
	{
		entry = *p;
		nextEntry = *(++p);
		ucb = L1EntryUcb(SECTION, entry);
		acc = L1EntryAcc(SECTION, entry);
		if ( (ucb != L1EntryUcb(SECTION, nextEntry))||
		    (acc != L1EntryAcc(SECTION, nextEntry))||
		    (L1EntryAddr(SECTION, entry) > L1EntryAddr(SECTION, nextEntry)) ||
		    ((L1EntryAddr(SECTION, entry) + _1M) < L1EntryAddr(SECTION, nextEntry)))
		{
			printf("Section (0x%08x - 0x%08x) =>",startPhyAddr, ((i << 20)| 0xfffff));
			printf(" (0x%08x - 0x%08x)",startVirAddr, (L1EntryAddr(SECTION, entry)| 0xfffff));
			if (ucb & C_BIT)
				printf(" Cachable/Bufferable");
			else
				printf(" Non-Cachable/Bufferable");

			switch(acc){
			case(NO_ACCESS):
				printf("\tNO_ACCESS");
				break;
			case(NO_USR_W):
				printf("\tNO_USR_W");
				break;
			case(SVC_RW):
				printf("\tSVC_RW");
				break;
			case(ALL_ACCESS):
				printf("\tALL_ACCESS");
				break;
			default:
				printf("\tALL_ACCESS");
				break;
			}
		printf("\n");
		startVirAddr = L1EntryAddr(SECTION, nextEntry);
		startPhyAddr = ((i+1) << 20);
		}
	}
}

static unsigned int createPageTable(void)
{
	int i;
	unsigned int *p = 0;
	unsigned int entry;
	char *envCacheMode, *envCacheshare;
	unsigned int cacheMode, sharedCacheMode;
	int cpu = whoAmI();


	if ((mvOsCpuPartGet() == CPU_PART_MRVL_A9)   ||
		(mvOsCpuPartGet() == CPU_PART_MRVLPJ4B_UP) ||
		(mvOsCpuPartGet() == CPU_PART_MRVLPJ4B_MP))
	{
		/* We use VMSAv6 compatible page table. Set XP bit in 	*/
		/* CP15 control register 				*/
		cpuCfgEnable((1 << 23), MV_TRUE);
	}

	p = (unsigned int *)MV_PT_BASE(cpu);
	envCacheMode  = ub_env_get("cacheMode");
	if(envCacheMode && (strcmp(envCacheMode,"write-through") == 0))
	{
		ub_env_set("cacheMode","write-through");
		ub_env_set("enaWrAllo","no");	/* No Wirte Allocate */
		cacheMode = WRITE_THROUGH;
	}
	else /*"write-back"*/
	{
		ub_env_set("cacheMode","write-back");
		cacheMode = WRITE_BACK;

		/* write allocate is now set through page table */
	        envCacheMode = ub_env_get("enaWrAllo");
	        if((strcmp(envCacheMode,"yes") == 0) || (strcmp(envCacheMode,"Yes") == 0) )
	                cacheMode |= TEX0_BIT;
	}
	envCacheshare = ub_env_get("cacheShare");
	if((strcmp(envCacheshare,"yes") == 0) || (strcmp(envCacheshare,"Yes") == 0) )
		sharedCacheMode = cacheMode | S_BIT;
	else
		sharedCacheMode = cacheMode;

	unsigned long dramSize = ub_dram_size();
	dramSize = dramSize >> 20;
	/* New page table requested by CV */
	/* 1st 0 MB - 0x02000000(32MB) Normal none cacheable/bufferable */
	entry = L1Entry(SECTION, 0, 0, U_BIT|(1 << 12), ALL_ACCESS);
	for (i = 0; i < 32; i++)
		*p++ = (entry | (i << 20));

	/* 2nd 32MB - (DRAM SIZE - 32M) - sharable/cacheable/bufferable */
	entry = L1Entry(SECTION, 0, 0,  sharedCacheMode , ALL_ACCESS);
	for (; i < (dramSize-32); i++)
		*p++ = (entry | (i << 20));

	/* 3nd (DRAM SIZE -32)MB - (DRAM SIZE) MB Normal none cacheable/bufferable */
	entry = L1Entry(SECTION, 0, 0, U_BIT|(1 << 12), ALL_ACCESS);
	for (; i < dramSize; i++)
		*p++ = (entry | (i << 20));

	/* 4rd (DRAM SIZE)MB - 0x7e000000 (2GB) sharable/cacheable/bufferable */
	entry = L1Entry(SECTION, 0, 0,  sharedCacheMode , ALL_ACCESS);
	for (; i < 2048 ; i++)
		*p++ = (entry | (i << 20));

	/* 4rd region 0x80000000  - 0x82000000 sharable/cacheable/bufferable 0x0 */
	entry = L1Entry(SECTION, 0, 0,  sharedCacheMode , ALL_ACCESS);
	for (; i < 2080; i++)
		*p++ = (entry | ((i - 2048) << 20));

	/* 5rd region 0x82000000  - 0x90000000 not cacheable/not bufferable mapped to 0x1000000 */
	entry = L1Entry(SECTION, 0, 0, U_BIT, ALL_ACCESS);
	for (; i < 2304; i++)
		*p++ = (entry | ((i - 2048) << 20));

	/* 6rd region 0x90000000  - 0xC0000000 (3GB) sharable/cacheable/bufferable */
	entry = L1Entry(SECTION, 0, 0,  sharedCacheMode , ALL_ACCESS);
	for (; i < 3072; i++)
		*p++ = (entry | (i << 20));

	/* 7th region 0xC0000000  - 0xffffffff none cacheable/ none bufferable */
	entry = L1Entry(SECTION, 0, 0, U_BIT, ALL_ACCESS);
	for (; i < 4096; i++)
		*p++ = (entry | (i << 20));


	//-------------------------------------------
	//-------------------------------------------
	//-------------------------------------------

	return MV_PT_BASE(cpu);
}

/* These are all the bits currently defined for the control register */
/* A.R.M. 7.4.2 */
#define MMU_XP	0x00800000
#define MMU_V 	0x00002000
#define MMU_I 	0x00001000
#define MMU_Z 	0x00000800
#define MMU_F 	0x00000400
#define MMU_R 	0x00000200
#define MMU_S 	0x00000100
#define MMU_B 	0x00000080
#define MMU_RES 0x00050078 /* reserved bits should be 1 */
#define MMU_C 	0x00000004
#define MMU_A 	0x00000002
#define MMU_M 	0x00000001



/*
 * The functions below take arguments to specify which "caches" the
 * action is to be directed at. For the I-cache, pass "MMU_I". For
 * the D-cache, "MMU_C". For both, pass "MMU_ID". For combined ID-Cache
 * processors, use "MMU_C"
 */
#define MMU_ID (MMU_I | MMU_C)

/*
 * Inline functions for MMU functions
 */

/* Set the page tables base register: register 2 (A.R.M. 7.4.3) */
inline void mmuSetPageTabBase(unsigned int pagetab)
{
	__asm__ __volatile__(
	"mcr	p15, 0, %0, c2, c0\n"
	:
	: "r" (pagetab));
	asm ("mcr p15, 0, %0, c7, c5, 4": :"r" (pagetab)); /* imb */
}

/* Set the domain access-control register: register 3 (A.R.M. 7.4.4) */
inline void mmuSetDomainAccessControl(unsigned long flags)
{
	__asm__ __volatile__(
	"mcr	p15, 0, %0, c3, c0\n"
	:
	: "r" (flags));
	asm ("mcr p15, 0, %0, c7, c5, 4": :"r" (flags)); /* imb */
}

/* Flush the cache(s).
 */

#ifndef MV88F78X60_Z1
	void armv7_mmu_cache_flush(void);
#endif
inline void mmuInvCache(unsigned caches)
{
	unsigned long dummy = 0;

	switch (caches)
	{
		case MMU_C:
			__asm__ __volatile__(
			"mcr	p15, 0, %0, c7, c6, 0\n"
			:
			: "r" (dummy));
			break;
		case MMU_I:
			__asm__ __volatile__(
			"mcr	p15, 0, %0, c7, c5, 0\n"
			:
			: "r" (dummy));

			break;
		case MMU_ID:
#ifndef MV88F78X60_Z1
			armv7_mmu_cache_flush();
#endif
			__asm__ __volatile__(
			"mcr	p15, 0, %0, c7, c7, 0\n"
			:
			: "r" (dummy));
            break;
	}
	asm ("mcr p15, 0, %0, c7, c5, 4": :"r" (dummy)); /* imb */
}

/* Flush the TLB(s)
 */
inline void mmuFlushTLB( unsigned tlbs)
{
	unsigned long dummy = 0;

	/* flush TLB(s): write to register 8, with flags (A.R.M. 7.4.9) */
	switch (tlbs)
	{
		case MMU_C:
			__asm__ __volatile__(
			"mcr	p15, 0, %0, c8, c6, 0\n"
			:
			: "r" (dummy));
            break;
		case MMU_I:
			__asm__ __volatile__(
			"mcr	p15, 0, %0, c8, c5, 0\n"
			:
			: "r" (dummy));
            break;
		case MMU_ID:
			__asm__ __volatile__(
			"mcr	p15, 0, %0, c8, c7, 0\n"
			:
			: "r" (dummy));
            break;
	}
	asm ("mcr p15, 0, %0, c7, c5, 4": :"r" (dummy)); /* imb */
}

/* Enable the cache/MMU/TLB etc.
 */
inline void cpuCfgEnable(unsigned long flags, MV_BOOL enable)
{
	unsigned long tmp;

	if (enable == MV_TRUE)
	{
		asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (tmp));
		tmp |= flags;
		asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (tmp));
	}
	else
	{
		asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (tmp));
		tmp &= ~flags;
		asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (tmp));
	}

	asm ("mcr p15, 0, %0, c7, c5, 4": :"r" (tmp)); /* imb */

}

/* Disable the I/D cache.
 */

inline void disableIDCache(void)
{
	unsigned long tmp;
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (tmp));
	tmp &= ~MMU_ID;
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (tmp));

	/* invalidate I/D-cache */
	tmp = 0;
	asm ("mcr p15, 0, %0, c7, c7, 0": :"r" (tmp));
	asm ("mcr p15, 0, %0, c7, c5, 4": :"r" (tmp)); /* imb */
}

/* return 0 in case cache is absent */
inline unsigned long getDCacheAssociativity(void)
{

	unsigned long tmp, dassoc, mul;

	/* Read cache type information */
	/* Bit[11:0] - I cache size */
	/* Bit[23:12] - D cache size */
	/* Bit 24 - if '1' Seperate I-cache and D-cache */
	/* bit[28:24] - ctype */
	asm ("mrc p15, 0, %0, c0, c0, 1":"=r" (tmp));

	/* D cache data only */
	/* Bit[1:0] - cache line length */
	/* Bit2 - M */
	/* Bit[5:3] - cache associativity */
	/* Bit[9:6] - cache size */
	tmp = (tmp >> 12) & 0xfff;

	/* Detect cache associativity - the formulae are teken from ARM DDI reference manual */
	mul = 2 + ((tmp >> 2) & 1);
	dassoc = ((tmp >> 3) & 0x7);
	if ((dassoc == 0) && (mul == 2))
	    dassoc = 1;
	else
	{
	    if ((dassoc == 0) && (mul == 3))
		dassoc = 0;
	    else
		dassoc = mul << (dassoc - 1);
	}

	return dassoc;
}

inline unsigned long getDCacheSize(void)
{

	unsigned long tmp, dsize, mul;

	/* Read cache type information */
	/* Bit[11:0] - I cache size */
	/* Bit[23:12] - D cache size */
	/* Bit 24 - if '1' Seperate I-cache and D-cache */
	/* bit[28:24] - ctype */
	asm ("mrc p15, 0, %0, c0, c0, 1":"=r" (tmp));

	/* D cache data only */
	/* Bit[1:0] - cache line length */
	/* Bit2 - M */
	/* Bit[5:3] - cache associativity */
	/* Bit[9:6] - cache size */
	tmp = (tmp >> 12) & 0xfff;

	/* Detect cache size  - the formulae are teken from ARM DDI reference manual */
	mul = 2 + ((tmp >> 2) & 1);
	dsize = ((tmp >> 6) & 0xf);
	dsize = mul << (dsize + 8);

	return dsize;
}

inline unsigned long getDCacheLine(void)
{

	unsigned long tmp, dline;

	/* Read cache type information */
	/* Bit[11:0] - I cache size */
	/* Bit[23:12] - D cache size */
	/* Bit 24 - if '1' Seperate I-cache and D-cache */
	/* bit[28:24] - ctype */
	asm ("mrc p15, 0, %0, c0, c0, 1":"=r" (tmp));

	/* D cache data only */
	/* Bit[1:0] - cache line length */
	/* Bit2 - M */
	/* Bit[5:3] - cache associativity */
	/* Bit[9:6] - cache size */
	tmp = (tmp >> 12) & 0xfff;

	/* Detect cache line  - the formulae are teken from ARM DDI reference manual */
	dline = 1 << ((tmp & 0x3) + 3);

	return dline;
}

extern ulong _dcache_index_max;
extern ulong _dcache_index_inc;
extern ulong _dcache_set_max;
extern ulong _dcache_set_index;

void pageTableInit(void)
{
	unsigned long dsize, dassoc, dline, log2Assoc, tmp;

	/* Detect cache size */
	dsize = getDCacheSize();
	/* Detect cache number of way */
	dassoc = getDCacheAssociativity();
	/* Detect cache number of way */
	dline = getDCacheLine();

	/* Detected cache absent */
	if (dassoc == 0)
	{
	    printf("       DCache absent!\n");
	    return;
	}
	else
	{
	    printf("       DCache size: %dKB\n",(int)(dsize/1024));
	    printf("       DCache associativity: %d way\n",(int)dassoc);
	    printf("       DCache line length: %dB\n",(int)dline);
	}


	/* Calc num of digits for assoc representation */
	log2Assoc = 0;
	tmp = dassoc - 1;
	do
	{
		log2Assoc++;
		tmp = tmp >> 1;
	}while (tmp > 0);

	printf("       Intializing Page Table: ");

	/* The num of assoc is set in the ip address from bit 31 backward */
	if (dassoc == 1)
	{
		_dcache_index_max = 0;
		_dcache_index_inc = 0;
	}
	else
	{
		_dcache_index_max = ~ ((1 << (31 - log2Assoc + 1)) - 1);
		_dcache_index_inc = (1 << (31 - log2Assoc + 1));
	}

	_dcache_set_max = (dsize/dassoc) - dline;
	_dcache_set_index = dline;

	disableIDCache();
	/* Disable D cache and MMU. */
	cpuCfgEnable(MMU_C | MMU_M, MV_FALSE);
	/* set up the page table, domain control registers */
	mmuSetPageTabBase(createPageTable());
	mmuSetDomainAccessControl(3);
	/* flush caches and TLBs */
	mmuInvCache(MMU_ID);
	mmuFlushTLB(MMU_ID);
	/* write to control register :-
	* I-cache on, 32-bit data and program space,
	* write-buffer on, D-cache on, MMU on
	*/
	cpuCfgEnable(MMU_I | MMU_RES | MMU_C | MMU_M, MV_TRUE);

	/* flush caches and TLBs */
	mmuInvCache(MMU_ID);
	mmuFlushTLB(MMU_ID);

	printf("Done\n");
	return ;
}

#endif /* defined(MV_INCLUDE_MONT_EXT) */
