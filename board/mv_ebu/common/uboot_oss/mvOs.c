/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.


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
* mvOsCpuArchLib.c - Marvell CPU architecture library
*
* DESCRIPTION:
*       This library introduce Marvell API for OS dependent CPU architecture
*       APIs. This library introduce single CPU architecture services APKI
*       cross OS.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

/* includes */
#include <asm/processor.h>
#include "mvOs.h"

static MV_U32 read_p15_c0 (void);
static MV_U32 read_p15_c1 (void);

/* defines  */
#define ARM_ID_REVISION_OFFS	0
#define ARM_ID_REVISION_MASK	(0xf << ARM_ID_REVISION_OFFS)

#define ARM_ID_PART_NUM_OFFS	4
#define ARM_ID_PART_NUM_MASK	(0xfff << ARM_ID_PART_NUM_OFFS)

#define ARM_ID_ARCH_OFFS	16
#define ARM_ID_ARCH_MASK	(0xf << ARM_ID_ARCH_OFFS)

#define ARM_ID_VAR_OFFS		20
#define ARM_ID_VAR_MASK		(0xf << ARM_ID_VAR_OFFS)

#define ARM_ID_ASCII_OFFS	24
#define ARM_ID_ASCII_MASK	(0xff << ARM_ID_ASCII_OFFS)

#define ARM_FEATURE_THUMBEE_OFFS	12
#define ARM_FEATURE_THUMBEE_MASK	(0xf << ARM_FEATURE_THUMBEE_OFFS)

/*******************************************************************************
* mvOsCpuVerGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Revision
*
*******************************************************************************/
MV_U32 mvOsCpuRevGet( MV_VOID )
{
	return ((read_p15_c0() & ARM_ID_REVISION_MASK ) >> ARM_ID_REVISION_OFFS);
}
/*******************************************************************************
* mvOsCpuPartGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Part number
*
*******************************************************************************/
MV_U32 mvOsCpuPartGet( MV_VOID )
{
	return ((read_p15_c0() & ARM_ID_PART_NUM_MASK ) >> ARM_ID_PART_NUM_OFFS);
}
/*******************************************************************************
* mvOsCpuArchGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Architicture number
*
*******************************************************************************/
MV_U32 mvOsCpuArchGet( MV_VOID )
{
    return ((read_p15_c0() & ARM_ID_ARCH_MASK ) >> ARM_ID_ARCH_OFFS);
}
/*******************************************************************************
* mvOsCpuVarGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Variant number
*
*******************************************************************************/
MV_U32 mvOsCpuVarGet( MV_VOID )
{
    return ((read_p15_c0() & ARM_ID_VAR_MASK ) >> ARM_ID_VAR_OFFS);
}
/*******************************************************************************
* mvOsCpuAsciiGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Variant number
*
*******************************************************************************/
MV_U32 mvOsCpuAsciiGet( MV_VOID )
{
    return ((read_p15_c0() & ARM_ID_ASCII_MASK ) >> ARM_ID_ASCII_OFFS);
}
/*******************************************************************************
* mvOsCpuThumbEEGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Variant number
*
*******************************************************************************/
MV_U32 mvOsCpuThumbEEGet( MV_VOID )
{
	return ((read_p15_c1() & ARM_FEATURE_THUMBEE_MASK ) >> ARM_FEATURE_THUMBEE_OFFS);
}

/*
static unsigned long read_p15_c0 (void)
*/
/* read co-processor 15, register #0 (ID register) */
static MV_U32 read_p15_c0 (void)
{
	MV_U32 value;

	__asm__ __volatile__(
		"mrc	p15, 0, %0, c0, c0, 0   @ read control reg\n"
		: "=r" (value)
		:
		: "memory");

	return value;
}
/* read co-processor 15, register #1 (Feature 0) */
static MV_U32 read_p15_c1 (void)
{
	MV_U32 value;

	__asm__ __volatile__(
						 "mrc	p15, 0, %0, c0, c1, 0   @ read feature0 reg\n"
	: "=r" (value)
	:
	: "memory");

	return value;
}

MV_U32 mvOsIoVirtToPhy( void* pDev, void* pVirtAddr )
{
    return (MV_U32)pVirtAddr;
}

void* mvOsIoUncachedMalloc( void* osHandle, MV_U32 size, MV_ULONG* pPhyAddr, MV_U32* memHandle)
{
    *pPhyAddr = (MV_ULONG)malloc(size);
    return (void *)(*pPhyAddr);
}

void mvOsIoUncachedFree( void* osHandle, MV_U32 size, MV_ULONG phyAddr, void* pVirtAddr, MV_U32 memHandle)
{
    free(pVirtAddr);
}

/*
 * use of mvOsIoUncachedMemalign() is coupled with mvOsIoUncachedAlignedFree();
 * Alignment should be power of two.
 */
void *mvOsIoUncachedAlignedMalloc(void *osHandle, MV_U32 alignment, MV_U32 size, MV_ULONG *pPhyAddr, MV_U32* memHandle)
{
    MV_U32 add = alignment - 1 + sizeof(void *);
    void *p, *alignedP;

    p = malloc(size + add);
    if (!p)
        return NULL;

    alignedP = (void *)(((int)p + add) & ~(alignment - 1));
    *((int *)alignedP - 1) = (int)p;

    if (pPhyAddr)
        *pPhyAddr = mvOsIoVirtToPhy(osHandle, alignedP);

    return alignedP;
}

void mvOsIoUncachedAlignedFree(void* osHandle, MV_U32 size, MV_ULONG phyAddr, void* pVirtAddr, MV_U32 memHandle)
{
    void *toFreeP = (void *)(*((int *)pVirtAddr - 1));

    if (pVirtAddr)
        free(toFreeP);
}

void* mvOsIoCachedMalloc( void* osHandle, MV_U32 size, MV_ULONG* pPhyAddr, MV_U32* memHandle)
{
    *pPhyAddr = (MV_ULONG)malloc(size);
    return (void *)(*pPhyAddr);
}

void mvOsIoCachedFree( void* osHandle, MV_U32 size, MV_ULONG phyAddr, void* pVirtAddr, MV_U32 memHandle)
{
    free(pVirtAddr);
}

MV_U32 mvOsCacheFlush( void* osHandle, void* p, int size )
{
    return (MV_U32)p;/* ronen - need to be filled */
}

MV_U32 mvOsCacheInvalidate( void* osHandle, void* p, int size )
{
    return (MV_U32)p;/* ronen - need to be filled */
}

int mvOsRand(void)
{
    return 0;
}

int mvOsStrCmp(const char *str1,const char *str2)
{

	do
	{
		if ((*str1++) != (*str2++)) return 1; /* not equal */

	}
	while ((*str1 != '\0') && (*str2 != '\0'));

	if (*str1 != *str2) return 1; /* not equal */

	/* equal */
	return 0;

}

#if defined(REG_DEBUG)
extern int reg_arry[REG_ARRAY_SIZE][2];
extern int reg_arry_index;
int reglog(unsigned int offset, unsigned int data)
{
	reg_arry[reg_arry_index%REG_ARRAY_SIZE][0] = (offset);
	reg_arry[reg_arry_index%REG_ARRAY_SIZE][1] = (data);
	reg_arry_index++;
	return 0;
}
#endif
