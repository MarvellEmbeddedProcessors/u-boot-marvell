/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvDdr3TrainingIPDef.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 42 $
*
*******************************************************************************/

#ifndef __mvDdr3TrainingIPDef_H
#define __mvDdr3TrainingIPDef_H

#include "common/siliconIf/mvSiliconIf.h"

#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
/* general define to be used instead of CHX_FAMILY & EXMXPM_FAMILY */
#define CPSS_BUILD
#include <common/os/hwsEnv.h>
#else
/* for bin header and other env */
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsIo.h>
#endif

#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY) || defined (WIN32)
#define INLINE
#else
#define INLINE inline
#endif

#if !defined(CHX_FAMILY) && !defined(EXMXPM_FAMILY) /* to avoid duplicated definition
													   of ASIC_SIMULATION in case it was defined */
#ifdef WIN32
#define ASIC_SIMULATION
#endif
#endif

#define PATTERN_55 (0x55555555)
#define PATTERN_AA (0xAAAAAAAA)
#define PATTERN_80 (0x80808080)
#define PATTERN_20 (0x20202020)
#define PATTERN_01 (0x01010101)
#define PATTERN_FF (0xFFFFFFFF)
#define PATTERN_00 (0x00000000)

/*16bit bus width patterns*/
#define PATTERN_55AA (0x5555AAAA)
#define PATTERN_00FF (0x0000FFFF)
#define PATTERN_0080 (0x00008080)

#define INVALID_VALUE (0xffffffff)
#define MAX_NUM_OF_DUNITS (32)
/* length *2 = length in words of pattern, first low address, second high address */
#define TEST_PATTERN_LENGTH (4) 
#define KILLER_PATTERN_DQ_NUMBER (8)
#define SSO_DQ_NUMBER (4)
#define PATTERN_MAXIMUM_LENGTH (64)
#define ADLL_TX_LENGTH (64)
#define ADLL_RX_LENGTH (32)

#define PARAM_NOT_CARE (0)

#define READ_LEVELING_PHY_OFFSET    (2)
#define WRITE_LEVELING_PHY_OFFSET   (0)

#define MASK_ALL_BITS (0xffffffff)

#define CS_BIT_MASK					(0xF)

/* DFX access */
#define BROADCAST_ID                        (28)
#define MULTICAST_ID                        (29)

#define  XSB_BASE_ADDR                      (0x00004000)
#define  XSB_CTRL_0_REG                     (0x00000000)
#define  XSB_CTRL_1_REG                     (0x00000004)
#define  XSB_CMD_REG                        (0x00000008)
#define  XSB_ADDRESS_REG                    (0x0000000C)
#define  XSB_DATA_REG                       (0x00000010)
#define  PIPE_ENABLE_ADDR	                (0x000f8000)
#define  ENABLE_DDR_TUNING_ADDR             (0x000f829C)

#define  CLIENT_BASE_ADDR                   (0x00002000)
#define  CLIENT_CTRL_REG                    (0x00000000)

#define  TARGET_INT                         (0x1801)
#define  TARGET_EXT                         (0x180E)
#define  BYTE_EN                            (0)
#define  CMD_READ                           (0)
#define  CMD_WRITE                          (1)

#define  INTERNAL_ACCESS_PORT               (1)
#define  EXECUTING                          (1)
#define  ACCESS_EXT                         (1)
#define  CS2_EXIST_BIT                      (2)
#define  TRAINING_ID                        (0xF)
#define  EXT_TRAINING_ID                    (1)
#define  EXT_MODE                           (0x4)

#define GET_RESULT_STATE(res)               (res )
#define SET_RESULT_STATE(res, state)        (res = state)
#if !defined(CHX_FAMILY) && !defined(EXMXPM_FAMILY)
#define _1K         0x00000400
#define _4K         0x00001000
#define _8K         0x00002000
#define _16K        0x00004000
#define _32K        0x00008000
#define _64K        0x00010000
#define _128K       0x00020000
#define _256K       0x00040000
#define _512K       0x00080000

#define _1M         0x00100000
#define _2M         0x00200000
#define _4M         0x00400000
#define _8M         0x00800000
#define _16M        0x01000000
#define _32M        0x02000000
#define _64M        0x04000000
#define _128M       0x08000000
#define _256M       0x10000000
#define _512M       0x20000000

#define _1G         0x40000000
#endif
#define _2G         0x80000000


#define      ADDR_SIZE_512Mb    0x04000000
#define      ADDR_SIZE_1Gb      0x08000000
#define      ADDR_SIZE_2Gb      0x10000000
#define      ADDR_SIZE_4Gb      0x20000000
#define      ADDR_SIZE_8Gb      0x40000000

/******************************** enum ***************************************/

typedef enum 
{
   FreqField_PLL_NDIV,
   FreqField_PLL_MDIV,
   FreqField_PLL_KDIV,
   FreqField_PLL_VCO_BAND,
   FreqField_PLL_LPF_R1_CTRL,

   MV_HWS_FreqField_LIMIT
}MV_HWS_FreqField;



typedef enum 
{
   EDGE_PF,
   EDGE_FP,
   EDGE_FPF,
   EDGE_PFP
}MV_HWS_EdgeCompare;



typedef enum 
{
   MV_HWS_ControlElement_ADLL, /* per bit 1 edge */
   MV_HWS_ControlElement_DQ_SKEW,
   MV_HWS_ControlElement_DQS_SKEW

}MV_HWS_ControlElement;


typedef enum 
{
   MV_HWS_TrainingType_PBS, /* per bit 1 edge */
   MV_HWS_TrainingType_ADLL_SHIFT, /* per pup 1 edge */
   MV_HWS_TrainingType_CENTRALIZATION /* per pup 2 edge */

}MV_HWS_TrainingType;

typedef enum 
{
   MV_HWS_Low2High,
   MV_HWS_High2Low,
   MV_HWS_SearchDirLimit
}MV_HWS_SearchDirection;


typedef enum
{
   PageSize_1K,
   PageSize_2K

}MV_HWS_PageSize;


typedef enum 
{
   MV_HWS_InitialValues_ADLL,
   MV_HWS_InitialValues_PBS

}MV_HWS_InitialValues;


typedef enum 
{
   Operation_READ = 0,
   Operation_WRITE = 1

} MV_HWS_Operation;

typedef enum 
{
   MV_HWS_TrainingIpStatus_FAIL,
   MV_HWS_TrainingIpStatus_SUCCESS,
   MV_HWS_TrainingIpStatus_TIMEOUT
}MV_HWS_TrainingIpStatus;


typedef enum _MV_HWS_CsType
{
   CS_SINGLE,
   CS_NON_SINGLE
}MV_HWS_DDR_CS;

typedef enum 
{
   DDR_PHY_DATA = 0,
   DDR_PHY_CONTROL = 1

} MV_HWS_DDR_PHY;

typedef enum 
{
   MV_HWS_MemoryType_Dummy
} MV_HWS_MemoryType;

typedef enum 
{
   OPER_WRITE,
   OPER_READ,
   OPER_WRITE_AND_READ

}MV_HWS_DIRECTION;

typedef enum 
{
   PHASE_SHIFT,
   CLOCK_SHIFT,
   ALIGN_SHIFT

}MV_HWS_WL_SUPP;
/************************* Structures ***********************************************/

typedef struct
{
	GT_U32                  regAddr;
	GT_U32                  regData;
	GT_U32                  regMask;
} RegData;


#endif /* __mvDdr3TrainingIPDef_H */


