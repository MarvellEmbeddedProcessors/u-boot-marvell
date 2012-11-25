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
* mvOsBios.h - O.S. interface header file for BIOS IAL
*
* DESCRIPTION:
*       This header file contains OS dependent definition for BIOS
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.7 $
*******************************************************************************/

#ifndef __INCmvOsSUBooth
#define __INCmvOsSUBooth

#include "mvTypes.h"
#include "mvOs.h"

/* Definitions */
#if defined (LOG_DEBUG) || defined (LOG_ERROR)
    #define PRNTHEXTSINGLE
    #define DEBUG_IAL
    #define DEBUG_IAL_SERIAL
#endif

#if defined(REG_DEBUG)
extern int reg_arry[REG_ARRAY_SIZE][2];
extern int reg_arry_index;
#endif

/*
 * Override CORE Driver SW queue size to 1. The CORE Driver SW queue is not
 * used for issuing UDMA commands; so minimizing this to 1 makes it possible
 * to minimize footprint of BIOS Extension driver when statically allocating
 * the channel's data structures.
 */
#define MV_SATA_OVERRIDE_SW_QUEUE_SIZE
#define MV_SATA_REQUESTED_SW_QUEUE_SIZE 1


#define MV_REGS_ACCESSED_BY_IO_BAR
/* System dependent macro for flushing CPU write cache */
#define MV_CPU_WRITE_BUFFER_FLUSH()

/* System dependent register read / write in byte/word/dword variants */
unsigned long readRegister (unsigned long base, unsigned long offset);
void writeRegister (unsigned long base, unsigned long offset, unsigned long value);
/*void pioWrite16bit (unsigned short offset, unsigned short data);
unsigned short pioRead16bit (unsigned short offset);*/
void waitForKeystroke (void);
unsigned short readSegmentOffset16bit (unsigned short segment_, unsigned short offset_);
unsigned char readSegmentOffset8bit (unsigned short segment_, unsigned short offset_);
void writeSegmentOffset16bit (unsigned short segment_, unsigned short offset_, unsigned short value_);
void writeSegmentOffset8bit (unsigned short segment_, unsigned short offset_, unsigned char value_);
void memcpySegmentOffset32bitForward (unsigned short segment_,
                                      unsigned short destinationOffset_,
                                      unsigned short sourceOffset_,
                                      unsigned short byteCount_);


MV_U16 mvSwapShort(MV_U16 data);
MV_U32 mvSwapWord(MV_U32 data);

#ifdef PRNTHEXTSINGLE
void printHexSingle(char *fmt, unsigned short value_);
#else
    #define printHexSingle(x,y)
#endif

/* System dependent little endian from / to CPU conversions */
#if (_BYTE_ORDER == _BIG_ENDIAN)
#       define MV_CPU_TO_LE16(x)        MV_16BIT_LE(x)
#       define MV_CPU_TO_LE32(x)        MV_32BIT_LE(x)

#       define MV_LE16_TO_CPU(x)        MV_16BIT_LE(x)
#       define MV_LE32_TO_CPU(x)        MV_32BIT_LE(x)
#else
#       define MV_CPU_TO_LE16(x)        (x)
#       define MV_CPU_TO_LE32(x)        (x)

#       define MV_LE16_TO_CPU(x)        (x)
#       define MV_LE32_TO_CPU(x)        (x)
#endif


/* System dependent register read / write in byte/word/dword variants */
/* Write 32/16/8 bit NonCacheable */
#if defined(REG_DEBUG)
#define MV_WRITE_CHAR(address, data)                                           \
        ((*((volatile unsigned char *)(address)))=             \
        ((unsigned char)(data)))+	\
	reglog((address), (data))
#else
#define MV_WRITE_CHAR(address, data)                                           \
        ((*((volatile unsigned char *)(address)))=             \
        ((unsigned char)(data)))
#endif

#if defined(REG_DEBUG)
#define MV_WRITE_SHORT(address, data)                                          \
        ((*((volatile unsigned short *)(address))) =           \
        ((unsigned short)(data)))+	\
	reglog((address), (data))
#else
#define MV_WRITE_SHORT(address, data)                                          \
        ((*((volatile unsigned short *)(address))) =           \
        ((unsigned short)(data)))
#endif

#if defined(REG_DEBUG)
#define MV_WRITE_WORD(address, data)                                           \
        ((*((volatile unsigned int *)(address))) =             \
        ((unsigned int)(data)))+	\
	reglog((address), (data))
#else
#define MV_WRITE_WORD(address, data)                                           \
        ((*((volatile unsigned int *)(address))) =             \
        ((unsigned int)(data)))
#endif

/* Read 32/16/8 bit NonCacheable - returns data direct. */

#define MV_READCHAR(address)                                                   \
        ((*((volatile unsigned char *)(address))))

#define MV_READSHORT(address)                                                  \
        ((*((volatile unsigned short *)(address))))

#define MV_READWORD(address)                                                   \
        ((*((volatile unsigned int *)(address))))

#define MV_REG_WRITE_BYTE(base, offset, val)    MV_WRITE_CHAR(base + offset, val)
#define MV_REG_WRITE_WORD(base, offset, val)    MV_WRITE_SHORT(base + offset, MV_CPU_TO_LE16(val))
#define MV_REG_WRITE_DWORD(base, offset, val)   MV_WRITE_WORD(base + offset, MV_CPU_TO_LE32(val))
#define MV_REG_READ_BYTE(base, offset)  	MV_READCHAR(base + offset)
#define MV_REG_READ_WORD(base, offset)  	mvSwapShort(MV_READSHORT(base + offset))
#define MV_REG_READ_DWORD(base, offset) 	mvSwapWord(MV_READWORD(base + offset))


/* System dependant typedefs */
typedef void            	   *MV_VOID_PTR;
typedef unsigned long              *MV_U32_PTR;
typedef unsigned short             *MV_U16_PTR;
typedef unsigned char              *MV_U8_PTR;
typedef char                       *MV_CHAR_PTR;
typedef unsigned long   	    MV_BUS_ADDR_T;
typedef unsigned long   	    MV_CPU_FLAGS;

/* Structures  */
/* System dependent structure */
typedef struct mvOsSemaphore
{
    unsigned long lock;
    unsigned long flags;
} MV_OS_SEMAPHORE;

/* Typedefs    */
typedef enum mvBoolean
{
    MV_SFALSE, MV_STRUE
} MV_BOOLEAN;


/* Functions (User implemented)*/
#include "sata/CoreDriver/mvLog.h"

/* Semaphore init, take and release */
#define mvOsSemInit(x) MV_TRUE
#define mvOsSemTake(x)
#define mvOsSemRelease(x)

/* Delay function in micro seconds resolution */
#define mvMicroSecondsDelay(dummy, time) udelay(time)

#endif /* __INCmvOsSUBooth */
