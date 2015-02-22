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

/*******************************************************************************
* mvXor.h - Header File for :
*
* DESCRIPTION:
*       This file contains Marvell Controller XOR HW library API.
*       NOTE: This HW library API assumes XOR source, destination and
*       descriptors are cache coherent.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __INCMVxorh
#define __INCMVxorh

#define SRAM_BASE       	0x40000000

#include "gtGenTypes.h"
#include "mv_os.h"
#include "printf.h"
#include "ddr3_hws_hw_training_def.h"

#define MV_XOR_MAX_UNIT					2 /* XOR unit == XOR engine */
#define MV_XOR_MAX_CHAN         		4 /* total channels for all units together*/
#define MV_XOR_MAX_CHAN_PER_UNIT		2 /* channels for units */

#define MV_IS_POWER_OF_2(num) ((num != 0) && ((num & (num - 1)) == 0))

/* typedefs */
/* This structure describes address space window. Window base can be        */
/* 64 bit, window size up to 4GB                                            */
typedef struct _mvAddrWin {
	GT_U32 baseLow;		/* 32bit base low       */
	GT_U32 baseHigh;	/* 32bit base high      */
	GT_U32 size;		/* 32bit size           */
} MV_ADDR_WIN;


/* This structure describes SoC units address decode window	*/
typedef struct {
	MV_ADDR_WIN addrWin;	/* An address window */
	GT_BOOL enable;		/* Address decode window is enabled/disabled    */
	GT_U8 attrib;		/* chip select attributes */
	GT_U8 targetId;		/* Target Id of this MV_TARGET */
} MV_UNIT_WIN_INFO;


/* This enumerator describes the type of functionality the XOR channel      */
/* can have while using the same data structures.                           */
	typedef enum _mvXorType {
		MV_XOR,		/* XOR channel functions as XOR accelerator     */
		MV_DMA,		/* XOR channel functions as IDMA channel        */
		MV_CRC32	/* XOR channel functions as CRC 32 calculator   */
	} MV_XOR_TYPE;

	
	/* This enumerator describes the set of commands that can be applied on   	*/
	/* an engine (e.g. IDMA, XOR). Appling a comman depends on the current   	*/
	/* status (see MV_STATE enumerator)                      					*/
	/* Start can be applied only when status is IDLE                         */
	/* Stop can be applied only when status is IDLE, ACTIVE or PAUSED        */
	/* Pause can be applied only when status is ACTIVE                          */
	/* Restart can be applied only when status is PAUSED                        */
	typedef enum _mvCommand {
		MV_START,		/* Start     */
		MV_STOP,		/* Stop     */
	  	MV_PAUSE,		/* Pause    */
	  	MV_RESTART		/* Restart  */
	} MV_COMMAND;
	
	
	/* This enumerator describes the set of state conditions.					*/
	/* Moving from one state to other is stricted.   							*/
	typedef enum _mvState {
		MV_IDLE,
  		MV_ACTIVE,
  		MV_PAUSED,
  		MV_UNDEFINED_STATE
	} MV_STATE;

	/* Controller peripherals are designated memory/IO address spaces that the  */
	/* controller can access. They are also refered as "targets"                */
	typedef enum _mvTarget {
		TBL_TERM = -1, 	/* none valid target, used as targets list terminator*/
  		SRAM,
		SDRAM_CS0,	/*0 SDRAM chip select 0		*/
/*		SDRAM_CS1, */ /*1 SDRAM chip select 1		*/ 
/*		SDRAM_CS2, */ /*2 SDRAM chip select 2		*/ 
/*		SDRAM_CS3, */ /*3 SDRAM chip select 3		*/ 
  		MAX_TARGETS
	} MV_TARGET;

#if defined(MV_CPU_LE)
	/* This structure describes XOR descriptor size 64bytes                     */
	typedef struct _mvXorDesc {
		GT_U32 status;	/* Successful descriptor execution indication */
		GT_U32 crc32Result;	/* Result of CRC-32 calculation */
		GT_U32 descCommand;	/* type of operation to be carried out on the data */
		GT_U32 phyNextDescPtr;	/* Next descriptor address pointer */
		GT_U32 byteCnt;	/* Size of source and destination blocks in bytes */
		GT_U32 phyDestAdd;	/* Destination Block address pointer */
		GT_U32 srcAdd0;	/* source block #0 address pointer */
		GT_U32 srcAdd1;	/* source block #1 address pointer */
		GT_U32 srcAdd2;	/* source block #2 address pointer */
		GT_U32 srcAdd3;	/* source block #3 address pointer */
		GT_U32 srcAdd4;	/* source block #4 address pointer */
		GT_U32 srcAdd5;	/* source block #6 address pointer */
		GT_U32 srcAdd6;	/* source block #6 address pointer */
		GT_U32 srcAdd7;	/* source block #7 address pointer */
		GT_U32 reserved0;
		GT_U32 reserved1;
	} MV_XOR_DESC;

/* XOR descriptor structure for CRC and DMA descriptor */
	typedef struct _mvCrcDmaDesc {
		GT_U32 status;	/* Successful descriptor execution indication */
		GT_U32 crc32Result;	/* Result of CRC-32 calculation */
		GT_U32 descCommand;	/* type of operation to be carried out on the data */
		GT_U32 nextDescPtr;	/* Next descriptor address pointer */
		GT_U32 byteCnt;	/* Size of source block part represented by the descriptor */
		GT_U32 destAdd;	/* Destination Block address pointer (not used in CRC32 */
		GT_U32 srcAdd0;	/* Mode: Source Block address pointer */
		GT_U32 srcAdd1;	/* Mode: Source Block address pointer */
	} MV_CRC_DMA_DESC;

#elif defined(MV_CPU_BE)
/* This structure describes XOR descriptor size 64bytes                     */
	typedef struct _mvXorDesc {
		GT_U32 crc32Result;	/* Result of CRC-32 calculation */
		GT_U32 status;	/* Successful descriptor execution indication */
		GT_U32 phyNextDescPtr;	/* Next descriptor address pointer */
		GT_U32 descCommand;	/* type of operation to be carried out on the data */
		GT_U32 phyDestAdd;	/* Destination Block address pointer */
		GT_U32 byteCnt;	/* Size of source and destination blocks in bytes */
		GT_U32 srcAdd1;	/* source block #1 address pointer */
		GT_U32 srcAdd0;	/* source block #0 address pointer */
		GT_U32 srcAdd3;	/* source block #3 address pointer */
		GT_U32 srcAdd2;	/* source block #2 address pointer */
		GT_U32 srcAdd5;	/* source block #5 address pointer */
		GT_U32 srcAdd4;	/* source block #4 address pointer */
		GT_U32 srcAdd7;	/* source block #7 address pointer */
		GT_U32 srcAdd6;	/* source block #6 address pointer */
		GT_U32 reserved0;
		GT_U32 reserved1;
	} MV_XOR_DESC;

/* XOR descriptor structure for CRC and DMA descriptor */
	typedef struct _mvCrcDmaDesc {
		GT_U32 crc32Result;	/* Result of CRC-32 calculation */
		GT_U32 status;	/* Successful descriptor execution indication */
		GT_U32 nextDescPtr;	/* Next descriptor address pointer */
		GT_U32 descCommand;	/* type of operation to be carried out on the data */
		GT_U32 destAdd;	/* Destination Block address pointer (not used in CRC32 */
		GT_U32 byteCnt;	/* Size of source block part represented by the descriptor */
		GT_U32 srcAdd1;	/* Mode: Source Block address pointer */
		GT_U32 srcAdd0;	/* Mode: Source Block address pointer */
	} MV_CRC_DMA_DESC;

#endif

	typedef struct _mvXorEcc {
		GT_U32 destPtr;	/* Target block pointer to ECC/MemInit operation */
		GT_U32 blockSize;	/* Block size in bytes for ECC/MemInit operation */
		GT_BOOL periodicEnable;	/* Enable Timer Mode                                                         */
		GT_U32 tClkTicks;	/* ECC timer mode initial count - down value     */
		GT_U32 sectorSize;	/* section size for ECC timer mode operation     */
	} MV_XOR_ECC;

	typedef enum _mvXorOverrideTarget {
		SRC_ADDR0,	/* Source Address #0 Control */
		SRC_ADDR1,	/* Source Address #1 Control */
		SRC_ADDR2,	/* Source Address #2 Control */
		SRC_ADDR3,	/* Source Address #3 Control */
		SRC_ADDR4,	/* Source Address #4 Control */
		SRC_ADDR5,	/* Source Address #5 Control */
		SRC_ADDR6,	/* Source Address #6 Control */
		SRC_ADDR7,	/* Source Address #7 Control */
		XOR_DST_ADDR,	/* Destination Address Control */
		XOR_NEXT_DESC	/* Next Descriptor Address Control */
	} MV_XOR_OVERRIDE_TARGET;

	GT_VOID mvSysXorInit(GT_U32 uiNumOfCS, GT_U32 uiCsEna, GT_U32 csSize, GT_U32 baseDelta);
	GT_VOID mvSysXorFinish(void);
	GT_VOID mvXorHalInit(GT_U32 xorChanNum);
	GT_STATUS mvXorCtrlSet(GT_U32 chan, GT_U32 xorCtrl);
	GT_STATUS mvXorEccClean(GT_U32 chan, MV_XOR_ECC *pXorEccConfig);
	GT_U32 mvXorEccCurrTimerGet(GT_U32 chan, GT_U32 tClk);
	GT_STATUS mvXorMemInit(GT_U32 chan, GT_U32 startPtr, GT_U32 blockSize, GT_U32 initValHigh, GT_U32 initValLow);
	GT_STATUS mvXorTransfer(GT_U32 chan, MV_XOR_TYPE xorType, GT_U32 xorChainPtr);
	MV_STATE mvXorStateGet(GT_U32 chan);
	GT_STATUS mvXorCommandSet(GT_U32 chan, MV_COMMAND command);
	GT_STATUS mvXorOverrideSet(GT_U32 chan, MV_XOR_OVERRIDE_TARGET target, GT_U32 winNum, GT_BOOL enable);

	GT_STATUS mvXorWinInit(MV_UNIT_WIN_INFO *addrWinMap);
	GT_STATUS mvXorTargetWinWrite(GT_U32 unit, GT_U32 winNum,MV_UNIT_WIN_INFO *pAddrDecWin);
	GT_STATUS mvXorTargetWinRead(GT_U32 unit, GT_U32 winNum,MV_UNIT_WIN_INFO *pAddrDecWin);
	GT_STATUS mvXorTargetWinEnable(GT_U32 unit, GT_U32 winNum, GT_BOOL enable);
	GT_STATUS mvXorProtWinSet(GT_U32 unit, GT_U32 chan, GT_U32 winNum, GT_BOOL access, GT_BOOL write);
	GT_STATUS mvXorPciRemap(GT_U32 unit, GT_U32 winNum, GT_U32 addrHigh);

	
#endif
