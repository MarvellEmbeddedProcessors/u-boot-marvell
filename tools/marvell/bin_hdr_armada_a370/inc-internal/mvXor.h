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

#include "ddr3_hw_training.h"
/*#include "mvCommon.h" */
/*#include "mvSysXorConfig.h" */


#define MV_XOR_MAX_UNIT					2 /* XOR unit == XOR engine */
#define MV_XOR_MAX_CHAN         		4 /* total channels for all units together*/
#define MV_XOR_MAX_CHAN_PER_UNIT		2 /* channels for units */

#define MV_IS_POWER_OF_2(num) ((num != 0) && ((num & (num - 1)) == 0))

/* typedefs */
/* This structure describes address space window. Window base can be        */
/* 64 bit, window size up to 4GB                                            */
typedef struct _mvAddrWin {
	MV_U32 baseLow;		/* 32bit base low       */
	MV_U32 baseHigh;	/* 32bit base high      */
	MV_U32 size;		/* 32bit size           */
} MV_ADDR_WIN;


/* This structure describes SoC units address decode window	*/
typedef struct {
	MV_ADDR_WIN addrWin;	/* An address window */
	MV_BOOL enable;		/* Address decode window is enabled/disabled    */
	MV_U8 attrib;		/* chip select attributes */
	MV_U8 targetId;		/* Target Id of this MV_TARGET */
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
		MV_U32 status;	/* Successful descriptor execution indication */
		MV_U32 crc32Result;	/* Result of CRC-32 calculation */
		MV_U32 descCommand;	/* type of operation to be carried out on the data */
		MV_U32 phyNextDescPtr;	/* Next descriptor address pointer */
		MV_U32 byteCnt;	/* Size of source and destination blocks in bytes */
		MV_U32 phyDestAdd;	/* Destination Block address pointer */
		MV_U32 srcAdd0;	/* source block #0 address pointer */
		MV_U32 srcAdd1;	/* source block #1 address pointer */
		MV_U32 srcAdd2;	/* source block #2 address pointer */
		MV_U32 srcAdd3;	/* source block #3 address pointer */
		MV_U32 srcAdd4;	/* source block #4 address pointer */
		MV_U32 srcAdd5;	/* source block #6 address pointer */
		MV_U32 srcAdd6;	/* source block #6 address pointer */
		MV_U32 srcAdd7;	/* source block #7 address pointer */
		MV_U32 reserved0;
		MV_U32 reserved1;
	} MV_XOR_DESC;

/* XOR descriptor structure for CRC and DMA descriptor */
	typedef struct _mvCrcDmaDesc {
		MV_U32 status;	/* Successful descriptor execution indication */
		MV_U32 crc32Result;	/* Result of CRC-32 calculation */
		MV_U32 descCommand;	/* type of operation to be carried out on the data */
		MV_U32 nextDescPtr;	/* Next descriptor address pointer */
		MV_U32 byteCnt;	/* Size of source block part represented by the descriptor */
		MV_U32 destAdd;	/* Destination Block address pointer (not used in CRC32 */
		MV_U32 srcAdd0;	/* Mode: Source Block address pointer */
		MV_U32 srcAdd1;	/* Mode: Source Block address pointer */
	} MV_CRC_DMA_DESC;

#elif defined(MV_CPU_BE)
/* This structure describes XOR descriptor size 64bytes                     */
	typedef struct _mvXorDesc {
		MV_U32 crc32Result;	/* Result of CRC-32 calculation */
		MV_U32 status;	/* Successful descriptor execution indication */
		MV_U32 phyNextDescPtr;	/* Next descriptor address pointer */
		MV_U32 descCommand;	/* type of operation to be carried out on the data */
		MV_U32 phyDestAdd;	/* Destination Block address pointer */
		MV_U32 byteCnt;	/* Size of source and destination blocks in bytes */
		MV_U32 srcAdd1;	/* source block #1 address pointer */
		MV_U32 srcAdd0;	/* source block #0 address pointer */
		MV_U32 srcAdd3;	/* source block #3 address pointer */
		MV_U32 srcAdd2;	/* source block #2 address pointer */
		MV_U32 srcAdd5;	/* source block #5 address pointer */
		MV_U32 srcAdd4;	/* source block #4 address pointer */
		MV_U32 srcAdd7;	/* source block #7 address pointer */
		MV_U32 srcAdd6;	/* source block #6 address pointer */
		MV_U32 reserved0;
		MV_U32 reserved1;
	} MV_XOR_DESC;

/* XOR descriptor structure for CRC and DMA descriptor */
	typedef struct _mvCrcDmaDesc {
		MV_U32 crc32Result;	/* Result of CRC-32 calculation */
		MV_U32 status;	/* Successful descriptor execution indication */
		MV_U32 nextDescPtr;	/* Next descriptor address pointer */
		MV_U32 descCommand;	/* type of operation to be carried out on the data */
		MV_U32 destAdd;	/* Destination Block address pointer (not used in CRC32 */
		MV_U32 byteCnt;	/* Size of source block part represented by the descriptor */
		MV_U32 srcAdd1;	/* Mode: Source Block address pointer */
		MV_U32 srcAdd0;	/* Mode: Source Block address pointer */
	} MV_CRC_DMA_DESC;

#endif

	typedef struct _mvXorEcc {
		MV_U32 destPtr;	/* Target block pointer to ECC/MemInit operation */
		MV_U32 blockSize;	/* Block size in bytes for ECC/MemInit operation */
		MV_BOOL periodicEnable;	/* Enable Timer Mode                                                         */
		MV_U32 tClkTicks;	/* ECC timer mode initial count - down value     */
		MV_U32 sectorSize;	/* section size for ECC timer mode operation     */
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

	MV_VOID mvSysXorInit(MV_DRAM_INFO *pDramInfo);
	MV_VOID mvSysXorFinish(void);
	MV_VOID mvXorHalInit(MV_U32 xorChanNum);
	MV_STATUS mvXorCtrlSet(MV_U32 chan, MV_U32 xorCtrl);
	MV_STATUS mvXorEccClean(MV_U32 chan, MV_XOR_ECC *pXorEccConfig);
	MV_U32 mvXorEccCurrTimerGet(MV_U32 chan, MV_U32 tClk);
	MV_STATUS mvXorMemInit(MV_U32 chan, MV_U32 startPtr, MV_U32 blockSize, MV_U32 initValHigh, MV_U32 initValLow);
	MV_STATUS mvXorTransfer(MV_U32 chan, MV_XOR_TYPE xorType, MV_U32 xorChainPtr);
	MV_STATE mvXorStateGet(MV_U32 chan);
	MV_STATUS mvXorCommandSet(MV_U32 chan, MV_COMMAND command);
	MV_STATUS mvXorOverrideSet(MV_U32 chan, MV_XOR_OVERRIDE_TARGET target, MV_U32 winNum, MV_BOOL enable);

	MV_STATUS mvXorWinInit(MV_UNIT_WIN_INFO *addrWinMap);
	MV_STATUS mvXorTargetWinWrite(MV_U32 unit, MV_U32 winNum,MV_UNIT_WIN_INFO *pAddrDecWin);
	MV_STATUS mvXorTargetWinRead(MV_U32 unit, MV_U32 winNum,MV_UNIT_WIN_INFO *pAddrDecWin);
	MV_STATUS mvXorTargetWinEnable(MV_U32 unit, MV_U32 winNum, MV_BOOL enable);
	MV_STATUS mvXorProtWinSet(MV_U32 unit, MV_U32 chan, MV_U32 winNum, MV_BOOL access, MV_BOOL write);
	MV_STATUS mvXorPciRemap(MV_U32 unit, MV_U32 winNum, MV_U32 addrHigh);

	
#endif
