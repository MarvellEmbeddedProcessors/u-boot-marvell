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

#include "mvSysHwConfig.h"
#if defined(MV_INCLUDE_MONT_EXT) && defined (MV_INCLUDE_MONT_MPU)


#include "mvTypes.h"
#include "mvCtrlEnvLib.h"
#include "mvCpuIf.h"
#include "cpu/mvCpu.h"


/* This file refers to the A.R.M.--The ARM Architecture Reference Manual */


typedef enum _access_type{

				/* priveliged -    user */
	NO_NO = 0 ,	/* No access _ No Access */
	RW_NO = 1,	/* Read/Write _ No Access */
	RW_RO = 2 ,	/* Read/Write _ Read Only */
	RW_RW = 3,	/* Read/Write _ Read/Write */
	RO_NO = 5 ,	/* Read Only  _ No Access */
	RO_RO = 6	/* Read Only  _ Read Only */

}ACCESS_TYPE;


typedef enum _region_size{

	REG_4KB		= 0xB,
	REG_8KB		= 0xC,
	REG_16KB	= 0xD,
	REG_32KB	= 0xE,
	REG_64KB	= 0xF,
	REG_128KB	= 0x10,
	REG_256KB	= 0x11,
	REG_512KB	= 0x12,
	REG_1MB		= 0x13,
	REG_2MB		= 0x14,
	REG_4MB		= 0x15,
	REG_8MB		= 0x16,
	REG_16MB	= 0x17,
	REG_32MB	= 0x18,
	REG_64MB	= 0x19,
	REG_128MB	= 0x1A,
	REG_256MB	= 0x1B,
	REG_512MB	= 0x1C,
	REG_1GB		= 0x1D,
	REG_2GB		= 0x1E,
	REG_4GB		= 0x1F

}REGION_SIZE;

typedef unsigned char bool;

typedef struct _mpu_region
{
	unsigned int base;
	REGION_SIZE	size;
	bool 		iCache;
	bool 		dCache;
	bool		wb;
	ACCESS_TYPE dAccess;
	ACCESS_TYPE iAccess;

}MPU_REGION;


typedef enum _cache_type{

	D_CACHE,
	I_CACHE

}CACHE_TYPE;


typedef enum _mem_type{

	D_MEM,
	I_MEM

}MEM_TYPE;

static unsigned int sizeToBits(unsigned int size)
{

	switch (size)
	{
	case 0x00001000:
		return REG_4KB;
		break;
	case 0x00002000:
		return REG_8KB;
		break;
	case 0x00004000:
		return REG_16KB;
		break;
	case 0x00008000:
		return REG_32KB;
		break;
	case 0x00010000:
		return REG_64KB;
		break;
	case 0x00020000:
		return REG_128KB;
		break;
	case 0x00040000:
		return REG_256KB;
		break;
	case 0x00080000:
		return REG_512KB;
		break;
	case 0x00100000:
		return REG_1MB;
		break;
	case 0x00200000:
		return REG_2MB;
		break;
	case 0x00400000:
		return REG_4MB;
		break;
	case 0x00800000:
		return REG_8MB;
		break;
	case 0x01000000:
		return REG_16MB;
		break;
	case 0x02000000:
		return REG_32MB;
		break;
	case 0x04000000:
		return REG_64MB;
		break;
	case 0x08000000:
		return REG_128MB;
		break;
	case 0x10000000:
		return REG_256MB;
		break;
	case 0x20000000:
		return REG_512MB;
		break;
	case 0x40000000:
		return REG_1GB;
		break;
	case 0x80000000:
		return REG_2GB;
		break;

	}
	return 0;
}

static void printSizeOfRegion(REGION_SIZE bits)
{

	switch (bits)
	{
	case REG_4KB:
		printf("4KB");
		break;
	case REG_8KB:
		printf("8KB");
		break;
	case REG_16KB:
		printf("16KB");
		break;
	case REG_32KB:
		printf("32KB");
		break;
	case REG_64KB:
		printf("64KB");
		break;
	case REG_128KB:
		printf("128KB");
		break;
	case REG_256KB:
		printf("256KB");
		break;
	case REG_512KB:
		printf("512KB");
		break;
	case REG_1MB:
		printf("1MB");
		break;
	case REG_2MB:
		printf("2MB");
		break;
	case REG_4MB:
		printf("4MB");
		break;
	case REG_8MB:
		printf("8MB");
		break;
	case REG_16MB:
		printf("16MB");
		break;
	case REG_32MB:
		printf("32MB");
		break;
	case REG_64MB:
		printf("64MB");
		break;
	case REG_128MB:
		printf("128MB");
		break;
	case REG_256MB:
		printf("256MB");
		break;
	case REG_512MB:
		printf("512MB");
		break;
	case REG_1GB:
		printf("1GB");
	case REG_2GB:
		printf("2GB");
		break;
	case REG_4GB:
		printf("4GB");
		break;
	}
}

/*
	get Control register
*/
unsigned int get_control(void)
{
	unsigned int value;

	__asm__ __volatile__(
		"mrc	p15, 0, %0, c1, c0, 0\n"
		: "=r" (value)
		:
		: "memory");

	return value;
}

/*
	set Control register
*/
void set_control(unsigned int value)
{
	__asm__ __volatile__(
		"mcr	p15, 0, %0, c1, c0, 0\n"
		:
		: "r" (value));
}

/*
	get Write Buffer Configuration
*/
static unsigned int get_wb(void)
{
	unsigned int value;

	__asm__ __volatile__(
		"mrc	p15, 0, %0, c3, c0, 0\n"
		: "=r" (value)
		:
		: "memory");

	return value;
}

/*
	set Write Buffer Configuration
*/
static void set_wb(unsigned int value)
{
	__asm__ __volatile__(
		"mcr	p15, 0, %0, c3, c0, 0\n"
		:
		: "r" (value));
}

/*
	Read cache configuration
*/

static unsigned int get_cache_config(CACHE_TYPE type)
{
	unsigned int value = 0;

	switch (type)
	{
	case D_CACHE:
		__asm__ __volatile__(
			"mrc	p15, 0, %0, c2, c0, 0\n"
			: "=r" (value)
			:
			: "memory");

		break;
	case I_CACHE:
		__asm__ __volatile__(
			"mrc	p15, 0, %0, c2, c0, 1\n"
			: "=r" (value)
			:
			: "memory");

		break;

	}

	return value;
}

/*
	Write cache configuration
*/

static void set_cache_config(CACHE_TYPE type,unsigned int value)
{
	switch (type) {
		case D_CACHE:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c2, c0, 0\n"
				:
				: "r" (value));
			break;
		case I_CACHE:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c2, c0, 1\n"
				:
				: "r" (value));
			break;
	}
}

/*
	Read access permision
*/

static unsigned int get_access(MEM_TYPE type)
{
	unsigned int value = 0;

	switch (type) {
		case D_MEM:
			__asm__ __volatile__(
				"mrc	p15, 0, %0, c5, c0, 2\n"
				: "=r" (value)
				:
				: "memory");
			break;
		case I_MEM:
			__asm__ __volatile__(
				"mrc	p15, 0, %0, c5, c0, 3\n"
				: "=r" (value)
				:
				: "memory");
			break;
	}

	return value;
}

/*
	Write cache configuration
*/

static void set_access(MEM_TYPE type,unsigned int value)
{
	switch (type) {
		case D_MEM:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c5, c0, 2\n"
				:
				: "r" (value));
			break;
		case I_MEM:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c5, c0, 3\n"
				:
				: "r" (value));
			break;
	}
}

/*
	get protection region size\base\enable
*/

unsigned int get_prot_attrib(int region)
{
	unsigned int value = 0;

	switch (region) {
		case 0:
			__asm__ __volatile__(
				"mrc	p15, 0, %0, c6, c0, 0\n"
				: "=r" (value)
				:
				: "memory");
			break;
		case 1:
			__asm__ __volatile__(
				"mrc	p15, 0, %0, c6, c1, 0\n"
				: "=r" (value)
				:
				: "memory");
			break;
		case 2:
			__asm__ __volatile__(
				"mrc	p15, 0, %0, c6, c2, 0\n"
				: "=r" (value)
				:
				: "memory");
			break;
		case 3:
			__asm__ __volatile__(
				"mrc	p15, 0, %0, c6, c3, 0\n"
				: "=r" (value)
				:
				: "memory");
			break;
		case 4:
			__asm__ __volatile__(
				"mrc	p15, 0, %0, c6, c4, 0\n"
				: "=r" (value)
				:
				: "memory");
			break;
		case 5:
			__asm__ __volatile__(
				"mrc	p15, 0, %0, c6, c5, 0\n"
				: "=r" (value)
				:
				: "memory");
			break;
		case 6:
			__asm__ __volatile__(
				"mrc	p15, 0, %0, c6, c6, 0\n"
				: "=r" (value)
				:
				: "memory");
			break;
		case 7:
			__asm__ __volatile__(
				"mrc	p15, 0, %0, c6, c7, 0\n"
				: "=r" (value)
				:
				: "memory");
			break;
	}
	return value;
}

/*
	set protection region size\base\enable
*/

static void set_prot_attrib(int region,unsigned int value)
{
	switch (region) {
		case 0:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c6, c0, 0\n"
				:
				: "r" (value));
			break;
		case 1:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c6, c1, 0\n"
				:
				: "r" (value));
			break;
		case 2:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c6, c2, 0\n"
				:
				: "r" (value));
			break;
		case 3:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c6, c3, 0\n"
				:
				: "r" (value));
			break;
		case 4:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c6, c4, 0\n"
				:
				: "r" (value));
			break;
		case 5:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c6, c5, 0\n"
				:
				: "r" (value));
			break;
		case 6:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c6, c6, 0\n"
				:
				: "r" (value));
			break;
		case 7:
			__asm__ __volatile__(
				"mcr	p15, 0, %0, c6, c7, 0\n"
				:
				: "r" (value));
			break;
	}
}


static void createMPUEntry(int region,MPU_REGION *mpuEntry)
{
	unsigned int value = 0;

	/* set data cache attributes */
	value = get_cache_config(D_CACHE);

	if (mpuEntry->dCache) {
		value |= (1<< region);
	} else {
		value &= ~(1<< region);
	}

	set_cache_config(D_CACHE,value);

	/* set instruction cache attributes */
	value = get_cache_config(I_CACHE);

	if (mpuEntry->iCache) {
		value |= (1<< region);
	} else {
		value &= ~(1<< region);
	}

	set_cache_config(I_CACHE,value);

	/* set write buffer */

	value = get_wb();

	if (mpuEntry->wb) {
		value |= (1<< region);
	} else {
		value &= ~(1<< region);
	}

	set_wb(value);

	/* set access permision for data accesses*/

	value = get_access(D_MEM);
	value &= ~(0xf << (region*4));
	value |= (mpuEntry->dAccess << (region*4));
	set_access(D_MEM , value);

	/* set access permision for instruction accesses*/

	value = get_access(I_MEM);
	value &= ~(0xf << (region*4));
	value |= (mpuEntry->iAccess << (region*4));
	set_access(I_MEM, value);

	/*set base and size and enable*/
	value = 0;
	value |= (mpuEntry->base) & (0xFFFFF << 12);
	value |= ( mpuEntry->size << 1);
	value |= 1;

	set_prot_attrib(region,value);
}


/* These are all the bits currently defined for the control register */
/* A.R.M. 7.4.2 */
#define MPU_V 0x2000 /* alternae vector select */
#define MPU_I 0x1000 /* Instruction cache */
#define MPU_B 0x0080  /* big endian */
#define MPU_RES 0x00F79 /* reserved bits should be 1 */
#define MPU_C 0x0004 /* data cache */
#define MPU_P 0x0001 /* protection unit */



/*
 * The functions below take arguments to specify which "caches" the
 * action is to be directed at. For the I-cache, pass "MMU_I". For
 * the D-cache, "MMU_C". For both, pass "MMU_ID". For combined ID-Cache
 * processors, use "MMU_C"
 */
#define MPU_ID (MPU_I + MPU_C)


int mpuMap(void)
{
	unsigned int value;
	int region;

	value = get_control();

	if (value & MPU_P) {
		printf("\nProtection Unit:\n");

		for (region = 1 ; region < 8 ; region++) {
			value = get_prot_attrib(region);
			/* check if region is enabled */
			if (value & 1) {
				printf("region %d base=0x%08x size =",region,value & (0xFFFFF << 12));
				printSizeOfRegion(((value & (0x1f << 1)) >> 1));

				value = get_cache_config(D_CACHE);

				if (value & (1 << region)) {
					printf(" :DCache enabled - ");
				} else {
					printf(" :DCache disabled - ");
				}

				value = get_cache_config(I_CACHE);

				if (value & (1 << region)) {
					printf("ICache enabled - ");
				} else {
					printf("ICache disabled - ");
				}

				value = get_wb();

				if (value & (1 << region)) {
					printf("Bufferable");
				} else {
					printf("non Bufferable");
				}

				 printf("\n");
			}
		}
	} else {
		printf("MPU is disabled\n");
	}

	return 1;
}

/* Flush the cache(s).
 */
inline void mpuInvCache(unsigned caches)
{
	unsigned long dummy = 0;

	switch (caches) {
		case MPU_C:
			__asm__ __volatile__(
			"mcr	p15, 0, %0, c7, c6, 0\n"
			:
			: "r" (dummy));
			break;
		case MPU_I:
			__asm__ __volatile__(
			"mcr	p15, 0, %0, c7, c5, 0\n"
			:
			: "r" (dummy));
			break;
		case MPU_ID:
			__asm__ __volatile__(
			"mcr	p15, 0, %0, c7, c7, 0\n"
			:
			: "r" (dummy));
		break;
	}
}

/* Enable the cache/MMU/TLB etc.
 */
inline void _cpuCfgEnable(unsigned long flags, MV_BOOL enable)
{
	unsigned long tmp;

	if (enable == MV_TRUE) {
		asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (tmp));
		tmp |= flags;
		asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (tmp));
	} else {
		asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (tmp));
		tmp &= ~flags;
		asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (tmp));
	}
}

/* Disable the I/D cache.
 */

inline void _disableIDCache(void)
{
	unsigned long tmp;
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (tmp));
	tmp &= ~MPU_ID;
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (tmp));

	/* invalidate I/D-cache */
	tmp = 0;
	asm ("mcr p15, 0, %0, c7, c7, 0": :"r" (tmp));

}


void MPU_Init(void)
{
	MPU_REGION mpuEntry;
#if defined(MV_INCLUDE_SDRAM_CS1)
	MV_CPU_DEC_WIN win;
	MV_TARGET target;
	unsigned int totalSize=0;
#endif
	int region=0;
	char *env;

	printf("Intializing Protection Unit\n");
	_disableIDCache();

	/* enable I cache - doesn't need MPU enabling */
	/*_enable(MPU_RES);*/

	/* set region 0 - include all memory and have I and D caches disabled
	and write buffer disabled */

	mpuEntry.base = 0;
	mpuEntry.size = REG_4GB;
	mpuEntry.dCache = 0;
	mpuEntry.iCache = 0;
	mpuEntry.wb = 0;
	mpuEntry.iAccess = NO_NO;
	mpuEntry.dAccess = NO_NO;

	createMPUEntry(region++ , &mpuEntry);

	/* set region 1 - CS0 DRAM region */

	mpuEntry.base = mvCpuIfTargetWinBaseLowGet(SDRAM_CS0);

	mpuEntry.size = sizeToBits(mvCpuIfTargetWinSizeGet(SDRAM_CS0));

	mpuEntry.dCache = 0;
	mpuEntry.iCache = 1;
	mpuEntry.wb = 0;

	mpuEntry.iAccess = RW_RW;
	mpuEntry.dAccess = RW_RW;

	createMPUEntry(region++ , &mpuEntry);


	/* set region 2 - CS0-CS3 DRAM region */
#if defined(MV_INCLUDE_SDRAM_CS1)
	for (target = SDRAM_CS1 ; target < MV_DRAM_MAX_CS ; target++) {
		mvCpuIfTargetWinGet(target, &win);

		if (win.enable) break;
	}

	target = SDRAM_CS1;
	mpuEntry.base = mvCpuIfTargetWinBaseLowGet(target);

	for (target = SDRAM_CS1; target < MV_DRAM_MAX_CS ; target++)
		totalSize += mvCpuIfTargetWinSizeGet(target);

	mpuEntry.size = sizeToBits(totalSize);

	mpuEntry.dCache = 0;
	mpuEntry.iCache = 0;
	mpuEntry.wb = 0;

	mpuEntry.iAccess = RW_RW;
	mpuEntry.dAccess = RW_RW;
#endif

	env = getenv("dramCached");

	if((!env)||( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) )) {
		setenv("dramCached","yes");

		mpuEntry.dCache = 1;
		mpuEntry.iCache = 1;
		mpuEntry.wb = 1;
	}

	createMPUEntry(region++ , &mpuEntry);

	/* set region 3 - devices region */
	mpuEntry.base = 0xF0000000;

	mpuEntry.size = REG_256MB;

	mpuEntry.dCache = 0;
	mpuEntry.iCache = 0;
	mpuEntry.wb = 0;

	mpuEntry.iAccess = RW_RW;
	mpuEntry.dAccess = RW_RW;

	createMPUEntry(region++ , &mpuEntry);

	/* set region 4 - pci region */
	mpuEntry.base = 0x90000000;

	mpuEntry.size = REG_256MB;

	mpuEntry.dCache = 0;
	mpuEntry.iCache = 0;
	mpuEntry.wb = 0;

	mpuEntry.iAccess = RW_RW;
	mpuEntry.dAccess = RW_RW;

	env = getenv("pciCached");

	if(!env) {
			setenv("pciCached","no");
	} else {
		if( ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) ) {
			mpuEntry.dCache = 1;
			mpuEntry.iCache = 1;
			mpuEntry.wb = 1;
		}
	}

	createMPUEntry(region++ , &mpuEntry);

	/* set region 5 - stack pointer */
	mpuEntry.base = 0xF00000;

	mpuEntry.size = REG_64KB;

	mpuEntry.dCache = 1;
	mpuEntry.iCache = 1;
	mpuEntry.wb = 1;

	mpuEntry.iAccess = RW_RW;
	mpuEntry.dAccess = RW_RW;


	createMPUEntry(region++ , &mpuEntry);


	/*mpuInvCache(MPU_ID);*/
	/* write to control register :-
	* I-cache on, 32-bit data and program space,
	* write-buffer on, D-cache on, MMU on
	*/
	_cpuCfgEnable((MPU_I|MPU_C|MPU_P|MPU_RES), MV_TRUE);

	return ;
}

#endif /* MV_INCLUDE_MONT_EXT */
