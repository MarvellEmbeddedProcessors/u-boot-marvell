
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
#ifndef _DDR3_TOPOLOGY_CONFIG_H
#define _DDR3_TOPOLOGY_CONFIG_H


#define MAX_INTERFACE_NUM  		(12)
#define MAX_BUS_NUM        		(8)

/*************************TOPOLOGY*******************************************/
typedef enum
{
   SPEED_BIN_DDR_800D,
   SPEED_BIN_DDR_800E,
   SPEED_BIN_DDR_1066E,
   SPEED_BIN_DDR_1066F,
   SPEED_BIN_DDR_1066G,
   SPEED_BIN_DDR_1333F,
   SPEED_BIN_DDR_1333G,
   SPEED_BIN_DDR_1333H,
   SPEED_BIN_DDR_1333J,
   SPEED_BIN_DDR_1600G,
   SPEED_BIN_DDR_1600H,
   SPEED_BIN_DDR_1600J,
   SPEED_BIN_DDR_1600K,
   SPEED_BIN_DDR_1866J,
   SPEED_BIN_DDR_1866K,
   SPEED_BIN_DDR_1866L,
   SPEED_BIN_DDR_1866M,
   SPEED_BIN_DDR_2133K,
   SPEED_BIN_DDR_2133L,
   SPEED_BIN_DDR_2133M,
   SPEED_BIN_DDR_2133N

}MV_HWS_SPEED_BIN;

typedef enum
{
   DDR_BOARD_ETP,
   DDR_BOARD_FUNCTIONAL,
   DDR_BOARD_CUSTOMER,
   DDR_BOARD_MAX

} MV_HWS_DDR_BOARD;

/* bus width in bits */
typedef enum
{
   BUS_WIDTH_4,
   BUS_WIDTH_8,
   BUS_WIDTH_16,
   BUS_WIDTH_32

} MV_HWS_BUS_WIDTH;

typedef enum
{
   MEM_512M,
   MEM_1G,
   MEM_2G,
   MEM_4G,
   MEM_8G,

   MEM_SIZE_LAST
}MV_HWS_MEM_SIZE;

typedef enum
{
  DDR_FREQ_LOW_FREQ,
  DDR_FREQ_400,
  DDR_FREQ_533,
  DDR_FREQ_667,
  DDR_FREQ_800,
  DDR_FREQ_933,
  DDR_FREQ_1066,
  DDR_FREQ_311,
  DDR_FREQ_333,
  DDR_FREQ_467,
   DDR_FREQ_LIMIT
}MV_HWS_DDR_FREQ;

typedef enum
{
   MV_HWS_TEMP_LOW,
   MV_HWS_TEMP_NORMAL,
   MV_HWS_TEMP_HIGH

}MV_HWS_TEMPERTURE;

typedef struct
{
   /* Chip Select (CS) bitmask (bits 0-CS0, bit 1- CS1 ...) */
   MV_U32      csBitmask;

   /* mirror enable/disable (bits 0-CS0 mirroring, bit 1- CS1 mirroring ...)*/
   MV_BOOL      mirrorEnableBitmask;

   /* DQS Swap (polarity) - true if enable*/
   MV_BOOL      isDqsSwap;

   /* CK swap (polarity) - true if enable*/
   MV_BOOL      isCkSwap;

} BusParams;

typedef struct
{
   /* bus configuration */
   BusParams   asBusParams[MAX_BUS_NUM];

   /* Speed Bin Table*/
   MV_HWS_SPEED_BIN      speedBinIndex;

   /* bus width of memory */
   MV_HWS_BUS_WIDTH   busWidth;

   /* Bus memory size (MBit) */
   MV_HWS_MEM_SIZE      memorySize;

   /* The DDR frequency for each interfaces */
   MV_HWS_DDR_FREQ      memoryFreq;

   /* delay CAS Write Latency - 0 for using default value (jedec suggested) */
   MV_U32      casWL;

   /* delay CAS Latency - 0 for using default value (jedec suggested) */
   MV_U32      casL;

   /* operation temperature */
   MV_HWS_TEMPERTURE   interfaceTemp;

} InterfaceParams;

/***********************************/

typedef struct
{
    /* Number of interfaces (default is 12)*/
    MV_U32              interfaceActiveMask;

   /* Controller configuration per interface */
   InterfaceParams      interfaceParams[MAX_INTERFACE_NUM];

   /* BUS per interface (default is 4)*/
   MV_U32               numOfBusPerInterface;

   /* Bit mask for active buses*/
   MV_U32               activeBusMask;

   /* Board type. Relevant for static configuration only */
   MV_U32               boardId;


} MV_HWS_TOPOLOGY_MAP;


#endif /* _DDR3_TOPOLOGY_CONFIG_H */
