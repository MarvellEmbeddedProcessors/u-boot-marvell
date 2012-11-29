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
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

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
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/


#ifndef MV_OS_H
#define MV_OS_H

/*************/
/* Includes  */
/*************/

#include "mvTypes.h"
#include "mvCommon.h"
#if defined(CV_PLATFORM)
#include <string.h>
#include <stdlib.h>
#else
#include <malloc.h>
#include <common.h>
#endif

#include "mvSysHwConfig.h"
#include "mvCtrlEnvSpec.h"



#ifdef __cplusplus
extern "C" {
#endif
#undef REG_DEBUG


/*************/
/* Constants */
/*************/

#define MV_OS_WAIT_FOREVER				0

/*************/
/* Datatypes */
/*************/

#define CPU_PHY_MEM(x)			(MV_U32)x
#define CPU_MEMIO_CACHED_ADDR(x)    	(void*)x
#define CPU_MEMIO_UNCACHED_ADDR(x)	(void*)x


/* CPU architecture dependent 32, 16, 8 bit read/write IO addresses */
#define MV_MEMIO32_WRITE(addr, data)	\
    ((*((volatile unsigned int*)(addr))) = ((unsigned int)(data)))

#define MV_MEMIO32_READ(addr)       	\
    ((*((volatile unsigned int*)(addr))))

#define MV_MEMIO16_WRITE(addr, data)	\
    ((*((volatile unsigned short*)(addr))) = ((unsigned short)(data)))

#define MV_MEMIO16_READ(addr)       	\
    ((*((volatile unsigned short*)(addr))))

#define MV_MEMIO8_WRITE(addr, data) 	\
    ((*((volatile unsigned char*)(addr))) = ((unsigned char)(data)))

#define MV_MEMIO8_READ(addr)        	\
    ((*((volatile unsigned char*)(addr))))


/* No Fast Swap implementation (in assembler) for ARM */
#define MV_32BIT_LE_FAST(val)            MV_32BIT_LE(val)
#define MV_16BIT_LE_FAST(val)            MV_16BIT_LE(val)
#define MV_32BIT_BE_FAST(val)            MV_32BIT_BE(val)
#define MV_16BIT_BE_FAST(val)            MV_16BIT_BE(val)

/* 32 and 16 bit read/write in big/little endian mode */

/* 16bit write in little endian mode */
#define MV_MEMIO_LE16_WRITE(addr, data) \
        MV_MEMIO16_WRITE(addr, MV_16BIT_LE_FAST(data))

/* 16bit read in little endian mode */
static __inline MV_U16 MV_MEMIO_LE16_READ(MV_U32 addr)
{
	MV_U16 data;

	data= (MV_U16)MV_MEMIO16_READ(addr);

	return (MV_U16)MV_16BIT_LE_FAST(data);
}

/* 32bit write in little endian mode */
#define MV_MEMIO_LE32_WRITE(addr, data) \
        MV_MEMIO32_WRITE(addr, MV_32BIT_LE_FAST(data))

/* 32bit read in little endian mode */
static __inline MV_U32 MV_MEMIO_LE32_READ(MV_U32 addr)
{
	MV_U32 data;

	data= (MV_U32)MV_MEMIO32_READ(addr);

	return (MV_U32)MV_32BIT_LE_FAST(data);
}

/******************************************************************************
* This debug function enable the write of each register that u-boot access to
* to an array in the DRAM, the function record only MV_REG_WRITE access.
* The function could not be operate when booting from flash.
* In order to print the array we use the printreg command.
******************************************************************************/
/*#define REG_DEBUG*/
#define REG_ARRAY_SIZE 4096
#if defined(REG_DEBUG)
extern int reg_arry[REG_ARRAY_SIZE][2];
extern int reg_arry_index;

int reglog(unsigned int offset, unsigned int data);
#endif
int reglog(unsigned int offset, unsigned int data);



/* Marvell controller register read/write macros */
#define MV_REG_VALUE(offset)          \
                (MV_MEMIO32_READ((INTER_REGS_BASE | (offset))))

#define MV_REG_READ(offset)             \
        (MV_MEMIO_LE32_READ(INTER_REGS_BASE | (offset)))
#if defined(REG_DEBUG)
#define MV_REG_WRITE(offset, val)    \
	(MV_MEMIO_LE32_WRITE((INTER_REGS_BASE | (offset)), (val))+ \
	reglog((INTER_REGS_BASE | (offset)), (val)))
#else
#define MV_REG_WRITE(offset, val)     \
        MV_MEMIO_LE32_WRITE((INTER_REGS_BASE | (offset)), (val))
#endif

#if defined(REG_DEBUG)
#define MV_REG_WORD_WRITE(offset, val)  \
(	MV_MEMIO_LE16_WRITE((INTER_REGS_BASE | (offset)), (val))+ \
	reglog( (INTER_REGS_BASE | (offset) ), (val) )) 
#else
#define MV_REG_WORD_WRITE(offset, val)  \
        MV_MEMIO_LE16_WRITE((INTER_REGS_BASE | (offset)), (val))
#endif

#define MV_REG_WORD_READ(offset)        \
        (MV_MEMIO16_READ((INTER_REGS_BASE | (offset))))

#define MV_REG_BYTE_READ(offset)        \
        (MV_MEMIO8_READ((INTER_REGS_BASE | (offset))))

#if defined(REG_DEBUG)
#define MV_REG_BYTE_WRITE(offset, val)  \
	MV_MEMIO8_WRITE((INTER_REGS_BASE | (offset)), (val))+ \
	reglog((INTER_REGS_BASE | (offset)), (val))
#else
#define MV_REG_BYTE_WRITE(offset, val)  \
        MV_MEMIO8_WRITE((INTER_REGS_BASE | (offset)), (val))
#endif

#if defined(REG_DEBUG)
#define MV_REG_BIT_SET(offset, bitMask)                 \
	(MV_MEMIO32_WRITE((INTER_REGS_BASE | (offset)), \
        (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)) | \
          MV_32BIT_LE_FAST(bitMask))))+\
	reglog((INTER_REGS_BASE | (offset)), (MV_MEMIO32_READ(INTER_REGS_BASE | (offset))))
#else
#define MV_REG_BIT_SET(offset, bitMask)                 \
        (MV_MEMIO32_WRITE((INTER_REGS_BASE | (offset)), \
         (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)) | \
          MV_32BIT_LE_FAST(bitMask))))
#endif

#if defined(REG_DEBUG)
#define MV_REG_BIT_RESET(offset,bitMask)                \
        (MV_MEMIO32_WRITE((INTER_REGS_BASE | (offset)), \
         (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)) & \
          MV_32BIT_LE_FAST(~bitMask)))+ \
	reglog((INTER_REGS_BASE | (offset)), (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)))))
#else
#define MV_REG_BIT_RESET(offset,bitMask)                \
        (MV_MEMIO32_WRITE((INTER_REGS_BASE | (offset)), \
         (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)) & \
          MV_32BIT_LE_FAST(~bitMask))))
#endif


/* Flash APIs */
#define MV_FL_8_READ            MV_MEMIO8_READ
#define MV_FL_16_READ           MV_MEMIO_LE16_READ
#define MV_FL_32_READ           MV_MEMIO_LE32_READ
#define MV_FL_8_DATA_READ       MV_MEMIO8_READ
#define MV_FL_16_DATA_READ      MV_MEMIO16_READ
#define MV_FL_32_DATA_READ      MV_MEMIO32_READ
#define MV_FL_8_WRITE           MV_MEMIO8_WRITE
#define MV_FL_16_WRITE          MV_MEMIO_LE16_WRITE
#define MV_FL_32_WRITE          MV_MEMIO_LE32_WRITE
#define MV_FL_8_DATA_WRITE      MV_MEMIO8_WRITE
#define MV_FL_16_DATA_WRITE     MV_MEMIO16_WRITE
#define MV_FL_32_DATA_WRITE     MV_MEMIO32_WRITE


/* CPU cache information */
#define CPU_I_CACHE_LINE_SIZE   32    /* 2do: replace 32 with linux core macro */
#define CPU_D_CACHE_LINE_SIZE   32    /* 2do: replace 32 with linux core macro */


/* Data cache flush one line */
#define mvOsCacheLineFlushInv(handle, addr)
#define mvOsCacheLineFlush(handle, addr)
#define mvOsCacheLineInv(handle, addr)
/* Flush CPU pipe */
#define CPU_PIPE_FLUSH

#define INLINE             inline
#define mvOsSPrintf        sprintf

/* In order to minimize image size printf, is defined as NULL */

#ifdef MV_RT_DEBUG
#   define mvOsPrintf      printf
#else
#   define mvOsPrintf(fmt,args...)
#endif /* MV_RT_DEBUG */

#define mvOsOutput         printf
#define mvOsMalloc         malloc
#define mvOsFree           free
#define mvOsMemcpy         memcpy
#define mvOsDelay(ms)      udelay(ms*1000)
#define mvOsSleep(us)      mvOsDelay(us)
#define mvOsTaskLock()
#define mvOsTaskUnlock()
#define mvOsIntLock()       0
#define mvOsIntUnlock(key)
#define mvOsUDelay(x)       udelay(x)
#define mvOsWarning()

#define strtol             simple_strtoul

#if defined(MV_BRIDGE_SYNC_REORDER)
extern MV_U32 *mvUncachedParam;

static INLINE void mvOsBridgeReorderWA(void)
{
        /* sync write reordering in the bridge */
        volatile MV_U32 val = 0;

        val = mvUncachedParam[0];
}
#endif

static INLINE void mvOsBCopy(MV_U8* srcAddr, MV_U8* dstAddr, int byteCount)

{

    while(byteCount != 0)

    {

        *dstAddr = *srcAddr;

        dstAddr++;

        srcAddr++;

        byteCount--;

    }

}

/* ARM architecture APIs */
MV_U32  mvOsCpuRevGet (MV_VOID);
MV_U32  mvOsCpuPartGet (MV_VOID);
MV_U32  mvOsCpuArchGet (MV_VOID);
MV_U32  mvOsCpuVarGet (MV_VOID);
MV_U32  mvOsCpuAsciiGet (MV_VOID);
MV_U32 mvOsCpuThumbEEGet (MV_VOID);
MV_U32 mvOsIoVirtToPhy( void* osHandle, void* pVirtAddr );
void* mvOsIoUncachedMalloc( void* osHandle, MV_U32 size, MV_ULONG* pPhyAddr, MV_U32* memHandle);
void mvOsIoUncachedFree( void* osHandle, MV_U32 size, MV_ULONG phyAddr, void* pVirtAddr, MV_U32 memHandle);
void *mvOsIoUncachedAlignedMalloc(void *osHandle, MV_U32 alignment, MV_U32 size, MV_ULONG *pPhyAddr, MV_U32* memHandle);
void mvOsIoUncachedAlignedFree(void* osHandle, MV_U32 size, MV_ULONG phyAddr, void* pVirtAddr, MV_U32 memHandle);
void* mvOsIoCachedMalloc( void* osHandle, MV_U32 size, MV_ULONG* pPhyAddr, MV_U32* memHandle);
void mvOsIoCachedFree( void* osHandle, MV_U32 size, MV_ULONG phyAddr, void* pVirtAddr, MV_U32 memHandle);
MV_U32 mvOsCacheFlush( void* osHandle, void* p, int size );
MV_U32 mvOsCacheInvalidate( void* osHandle, void* p, int size );
int mvOsRand(void);
int mvOsStrCmp(const char *str1,const char *str2);

#ifdef __cplusplus
}
#endif

#endif /* MV_OS_H */
