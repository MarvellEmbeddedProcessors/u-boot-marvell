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
* mvDdr3TrainingIpFlow.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 39 $
*
*******************************************************************************/




#ifndef _MV_DDR3_TRAININGIP_FLOW_H_
#define _MV_DDR3_TRAININGIP_FLOW_H_

#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpPbs.h"

#ifdef __cplusplus
   extern "C"
   {
#endif


/******************************** defined ***************************************/
#define MV_DDR_ILLEGAL_PARAMETER   GT_FAIL
#define MV_DDR_INTERFACE_EXTERNAL_FUNC_NOT_IMPLEMENTED GT_FAIL
#define MV_DDR_INTERFACE_INTERNAL_FUNC_NOT_IMPLEMENTED GT_FAIL
#define MV_DDR_POLLING_TIMEOUT GT_FAIL
#define MV_DDR_INTERFACE_NOT_IMPLEMENTED GT_FAIL
#define MAX_CHANNELS 4

#define MRS0_CMD (0x3)
#define MRS1_CMD (0x4)
#define MRS2_CMD (0x8)
#define MRS3_CMD (0x9)
/******************************************************************************/
/* Definitions of INTERFACE registers                                             */
/******************************************************************************/

#define READ_BUFFER_SELECT                   0x14a4

/******************************************************************************/
/* Definitions of PHY registers                                               */
/******************************************************************************/

#define PBS_PATTERN_LENGTH (2)
#define RL_PATTERN_LENGTH (2)
#define KILLER_PATTERN_LENGTH (32)
#define KILLER_PATTERN_DQ_NUMBER (8)
#define PATTERN_MAXIMUM_LENGTH (64)
#define EXT_ACCESS_BURST_LENGTH (8)
#define  CHIP_ID                0
#define IS_INTERFACE_ACTIVE( _uiInterfaceMask_ , _uiInterfaceId_ )   ( _uiInterfaceMask_ & ( 1 << _uiInterfaceId_ ) )
#define IS_BUS_ACTIVE( _uiInterfaceMask_ , _uiInterfaceId_ )   ( (_uiInterfaceMask_ >> _uiInterfaceId_) & 1 )
       /*Haim ( _uiInterfaceMask_ & ( 1 << _uiInterfaceId_ ) )*/  
#define DDR3_IS_ECC_PUP3_MODE(_uiInterfaceMask_)	((_uiInterfaceMask_ == 0xB)?(GT_TRUE):(GT_FALSE))
#define DDR3_IS_ECC_PUP4_MODE(_uiInterfaceMask_)	((((_uiInterfaceMask_ & 0x10) == 0))?(GT_FALSE):(GT_TRUE))
#define DDR3_IS_16BIT_DRAM_MODE(_activeBusMask_)	((((_activeBusMask_ & 0x4) == 0))?(GT_TRUE):(GT_FALSE))

#define MEGA                        (1000000)
#define MAXIMUM_NUMBER_OF_POLLING   (1000)
#define BUS_WIDTH_IN_BITS           (8)
#define CLOCK_TIME_PSEC             (7800000000)
#define NUM_OF_KILLER_PATTERNS      (8)
/*DFX address Space
Table 2: DFX address space
Address Bits   Value   Description
[31 : 20]   0x?   DFX base address � bases PCIe mapping
[19 : 15]   0...NumberOfClient-1   Client Index inside pipe. See also Table 1 MultiCast = d�29 Broadcast =d�28
[14 : 13]   2'b01   Access to Client Internal Register
[12 : 0]   Client Internal Register offset   See related Client Registers
[14 : 13]   2'b00   Access to Ram Wrappers Internal Register
[12 : 6]   0�NumberOfRams-1   Ram Index inside Client
[5 : 0]   Ram Wrapper Internal Register offset   See related Ram Wrappers Registers
*/

/* nsec */
#define  TREFI_LOW                        (7800)
#define  TREFI_HIGH                       (3900)

#define  TR2R_VALUE_REG                   (0x180)
#define  TR2R_MASK_REG                    (0x180)
#define  TRFC_MASK_REG                    (0x7f)
#define  TR2W_MASK_REG                    (0x600)
#define  TW2W_HIGH_VALUE_REG              (0x1800)
#define  TW2W_HIGH_MASK_REG               (0xF800)
#define  TRFC_HIGH_VALUE_REG              (0x20000)
#define  TRFC_HIGH_MASK_REG               (0x70000)
#define  TR2R_HIGH_VALUE_REG              (0x0)
#define  TR2R_HIGH_MASK_REG               (0x380000)
#define  TMOD_VALUE_REG                   (0x16000000)
#define  TMOD_MASK_REG                    (0x1E000000)
#define  T_VALUE_REG                      (0x40000000)
#define  T_MASK_REG                       (0xC0000000)
#define  AUTO_ZQC_TIMING                  (15384)
#define  WRITE_XBAR_PORT1                 (0xC03F8077)
#define  READ_XBAR_PORT1                  (0xC03F8073)
#define  DISABLE_DDR_TUNING_DATA          (0x02294285)
#define  ENABLE_DDR_TUNING_DATA           (0x12294285)

#if !defined(CPSS_BUILD) && !defined(CONFIG_BOBCAT2)
#define ODPG_TRAINING_STATUS_REG          (0x18488)
#else
#define ODPG_TRAINING_STATUS_REG          (0x1030)
#endif
#define ODPG_TRAINING_TRIGGER_REG         (0x1030)
#define ODPG_STATUS_DONE_REG         	  (0x16FC)
#define ODPG_ENABLE_REG                   (0x186D4)
#define ODPG_ENABLE_OFFS                  (0)
#define ODPG_DISABLE_OFFS                 (8)

#define ODPG_TRAINING_CONTROL_REG         (0x1034)
#define ODPG_OBJ1_OPCODE_REG              (0x103C)
#define ODPG_OBJ1_ITER_CNT_REG            (0x10B4)
#define CALIB_OBJ_PRFA_REG                (0x10C4)
#define ODPG_WRITE_LEVELING_DONE_CNTR_REG (0x10F8)
#define ODPG_WRITE_READ_MODE_ENABLE_REG   (0x10FC)
#define TRAINING_OPCODE_1_REG             (0x10B4)
#define SDRAM_CONFIGURATION_REG           (0x1400)
#define DDR_CONTROL_LOW_REG               (0x1404)
#define SDRAM_TIMING_LOW_REG              (0x1408)
#define SDRAM_TIMING_HIGH_REG             (0x140C)
#define SDRAM_ACCESS_CONTROL_REG          (0x1410)
#define SDRAM_OPEN_PAGE_CONTROL_REG       (0x1414)
#define SDRAM_OPERATION_REG               (0x1418)
#define DUNIT_CONTROL_HIGH_REG            (0x1424)
#define ODT_TIMING_LOW                    (0x1428)
#define DDR_TIMING_REG                    (0x142c)
#define ODT_TIMING_HI_REG                 (0x147c)
#define SDRAM_INIT_CONTROL_REG            (0x1480)
#define SDRAM_ODT_CONTROL_HIGH_REG        (0x1498)
#define DUNIT_ODT_CONTROL_REG             (0x149c)
#define READ_BUFFER_SELECT_REG            (0x14a4)
#define DUNIT_MMASK_REG                   (0x14b0)
#define CALIB_MACHINE_CTRL_REG            (0x14cc)
#define DRAM_DLL_TIMING_REG               (0x14E0)
#define DRAM_ZQ_INIT_TIMIMG_REG           (0x14E4)
#define DRAM_ZQ_TIMING_REG                (0x14E8)
#define DFS_REG                           (0x1528)
#define READ_DATA_SAMPLE_DELAY            (0x1538)
#define READ_DATA_READY_DELAY             (0x153C)
#define TRAINING_REG                      (0x15B0)
#define TRAINING_SW_1_REG                 (0x15B4)
#define TRAINING_SW_2_REG                 (0x15B8)
#define TRAINING_PATTERN_BASE_ADDRESS_REG (0x15BC)
#define TRAINING_DBG_1_REG                (0x15C0)
#define TRAINING_DBG_2_REG                (0x15C4)
#define TRAINING_DBG_3_REG                (0x15C8)
#define RANK_CTRL_REG                     (0x15E0)
#define TIMING_REG                        (0x15E4)
#define DRAM_PHY_CONFIGURATION            (0x15EC)
#define MR0_REG                           (0x15D0)
#define MR1_REG                           (0x15D4)
#define MR2_REG                           (0x15D8)
#define MR3_REG                           (0x15DC)
#define TIMING_REG                        (0x15E4)
#define ODPG_CTRL_CONTROL_REG             (0x1600)
#define ODPG_DATA_CONTROL_REG             (0x1630)
#define ODPG_PATTERN_ADDR_OFFSET_REG      (0x1638)
#define ODPG_DATA_BUF_SIZE_REG            (0x163C)
#define PHY_LOCK_STATUS_REG               (0x1674)
#define PHY_REG_FILE_ACCESS               (0x16A0)
#define TRAINING_WRITE_LEVELING_REG       (0x16AC)
#define ODPG_PATTERN_ADDR_REG             (0x16B0)
#define ODPG_PATTERN_DATA_HI_REG          (0x16B4)
#define ODPG_PATTERN_DATA_LOW_REG         (0x16B8) 
#define ODPG_BIST_LAST_FAIL_ADDR_REG      (0x16BC)
#define ODPG_BIST_DATA_ERROR_COUNTER_REG  (0x16C0)
#define ODPG_BIST_FAILED_DATA_HI_REG      (0x16C4)
#define ODPG_BIST_FAILED_DATA_LOW_REG     (0x16C8)
#define ODPG_WRITE_DATA_ERROR_REG         (0x16CC)
#define CS_ENABLE_REG                     (0x16D8)
#define WR_LEVELING_DQS_PATTERN_REG       (0x16DC)

#if !defined(CPSS_BUILD) && !defined(CONFIG_BOBCAT2)
#define ODPG_BIST_DONE                    (0x186D4)
#else
#define ODPG_BIST_DONE                    (0x16FC)
#endif
#define ODPG_BIST_DONE_BIT_OFFS           (0)
#define ODPG_BIST_DONE_BIT_VALUE_REV2          (1)
#define ODPG_BIST_DONE_BIT_VALUE_REV3          (0)

#if !defined(CPSS_BUILD) && !defined(CONFIG_BOBCAT2)
#define RESULT_CONTROL_BYTE_PUP_0_REG     (0x1830)
#define RESULT_CONTROL_BYTE_PUP_1_REG     (0x1834)
#define RESULT_CONTROL_BYTE_PUP_2_REG     (0x1838)
#define RESULT_CONTROL_BYTE_PUP_3_REG     (0x183C)
#define RESULT_CONTROL_BYTE_PUP_4_REG     (0x18B0)

#define RESULT_CONTROL_PUP_0_BIT_0_REG    (0x18B4)
#define RESULT_CONTROL_PUP_0_BIT_1_REG    (0x18B8)
#define RESULT_CONTROL_PUP_0_BIT_2_REG    (0x18BC)
#define RESULT_CONTROL_PUP_0_BIT_3_REG    (0x18C0)
#define RESULT_CONTROL_PUP_0_BIT_4_REG    (0x18C4)
#define RESULT_CONTROL_PUP_0_BIT_5_REG    (0x18C8)
#define RESULT_CONTROL_PUP_0_BIT_6_REG    (0x18CC)
#define RESULT_CONTROL_PUP_0_BIT_7_REG    (0x18F0)
#define RESULT_CONTROL_PUP_1_BIT_0_REG    (0x18F4)
#define RESULT_CONTROL_PUP_1_BIT_1_REG    (0x18F8)
#define RESULT_CONTROL_PUP_1_BIT_2_REG    (0x18FC)
#define RESULT_CONTROL_PUP_1_BIT_3_REG    (0x1930)
#define RESULT_CONTROL_PUP_1_BIT_4_REG    (0x1934)
#define RESULT_CONTROL_PUP_1_BIT_5_REG    (0x1938)
#define RESULT_CONTROL_PUP_1_BIT_6_REG    (0x193C)
#define RESULT_CONTROL_PUP_1_BIT_7_REG    (0x19B0)
#define RESULT_CONTROL_PUP_2_BIT_0_REG    (0x19B4)
#define RESULT_CONTROL_PUP_2_BIT_1_REG    (0x19B8)
#define RESULT_CONTROL_PUP_2_BIT_2_REG    (0x19BC)
#define RESULT_CONTROL_PUP_2_BIT_3_REG    (0x19C0)
#define RESULT_CONTROL_PUP_2_BIT_4_REG    (0x19C4)
#define RESULT_CONTROL_PUP_2_BIT_5_REG    (0x19C8)
#define RESULT_CONTROL_PUP_2_BIT_6_REG    (0x19CC)
#define RESULT_CONTROL_PUP_2_BIT_7_REG    (0x19F0)
#define RESULT_CONTROL_PUP_3_BIT_0_REG    (0x19F4)
#define RESULT_CONTROL_PUP_3_BIT_1_REG    (0x19F8)
#define RESULT_CONTROL_PUP_3_BIT_2_REG    (0x19FC)
#define RESULT_CONTROL_PUP_3_BIT_3_REG    (0x1A30)
#define RESULT_CONTROL_PUP_3_BIT_4_REG    (0x1A34)
#define RESULT_CONTROL_PUP_3_BIT_5_REG    (0x1A38)
#define RESULT_CONTROL_PUP_3_BIT_6_REG    (0x1A3C)
#define RESULT_CONTROL_PUP_3_BIT_7_REG    (0x1AB0)
#define RESULT_CONTROL_PUP_4_BIT_0_REG    (0x1AB4)
#define RESULT_CONTROL_PUP_4_BIT_1_REG    (0x1AB8)
#define RESULT_CONTROL_PUP_4_BIT_2_REG    (0x1ABC)
#define RESULT_CONTROL_PUP_4_BIT_3_REG    (0x1AC0)
#define RESULT_CONTROL_PUP_4_BIT_4_REG    (0x1AC4)
#define RESULT_CONTROL_PUP_4_BIT_5_REG    (0x1AC8)
#define RESULT_CONTROL_PUP_4_BIT_6_REG    (0x1ACC)
#define RESULT_CONTROL_PUP_4_BIT_7_REG    (0x1AF0)
#else
#define RESULT_CONTROL_BYTE_PUP_0_REG     (0x1830)
#define RESULT_CONTROL_BYTE_PUP_1_REG     (0x1834)
#define RESULT_CONTROL_BYTE_PUP_2_REG     (0x1838)
#define RESULT_CONTROL_BYTE_PUP_3_REG     (0x183C)

#define RESULT_CONTROL_PUP_0_BIT_0_REG    (0x18B0)
#define RESULT_CONTROL_PUP_0_BIT_1_REG    (0x18B4)
#define RESULT_CONTROL_PUP_0_BIT_2_REG    (0x18B8)
#define RESULT_CONTROL_PUP_0_BIT_3_REG    (0x18BC)
#define RESULT_CONTROL_PUP_0_BIT_4_REG    (0x18C0)
#define RESULT_CONTROL_PUP_0_BIT_5_REG    (0x18C4)
#define RESULT_CONTROL_PUP_0_BIT_6_REG    (0x18C8)
#define RESULT_CONTROL_PUP_0_BIT_7_REG    (0x18CC)
#define RESULT_CONTROL_PUP_1_BIT_0_REG    (0x18F0)
#define RESULT_CONTROL_PUP_1_BIT_1_REG    (0x18F4)
#define RESULT_CONTROL_PUP_1_BIT_2_REG    (0x18F8)
#define RESULT_CONTROL_PUP_1_BIT_3_REG    (0x18FC)
#define RESULT_CONTROL_PUP_1_BIT_4_REG    (0x1930)
#define RESULT_CONTROL_PUP_1_BIT_5_REG    (0x1934)
#define RESULT_CONTROL_PUP_1_BIT_6_REG    (0x1938)
#define RESULT_CONTROL_PUP_1_BIT_7_REG    (0x193C)
#define RESULT_CONTROL_PUP_2_BIT_0_REG    (0x19B0)
#define RESULT_CONTROL_PUP_2_BIT_1_REG    (0x19B4)
#define RESULT_CONTROL_PUP_2_BIT_2_REG    (0x19B8)
#define RESULT_CONTROL_PUP_2_BIT_3_REG    (0x19BC)
#define RESULT_CONTROL_PUP_2_BIT_4_REG    (0x19C0)
#define RESULT_CONTROL_PUP_2_BIT_5_REG    (0x19C4)
#define RESULT_CONTROL_PUP_2_BIT_6_REG    (0x19C8)
#define RESULT_CONTROL_PUP_2_BIT_7_REG    (0x19CC)
#define RESULT_CONTROL_PUP_3_BIT_0_REG    (0x19F0)
#define RESULT_CONTROL_PUP_3_BIT_1_REG    (0x19F4)
#define RESULT_CONTROL_PUP_3_BIT_2_REG    (0x19F8)
#define RESULT_CONTROL_PUP_3_BIT_3_REG    (0x19FC)
#define RESULT_CONTROL_PUP_3_BIT_4_REG    (0x1A30)
#define RESULT_CONTROL_PUP_3_BIT_5_REG    (0x1A34)
#define RESULT_CONTROL_PUP_3_BIT_6_REG    (0x1A38)
#define RESULT_CONTROL_PUP_3_BIT_7_REG    (0x1A3C)

/*Should not be used in non-a38x cases*/
#define RESULT_CONTROL_BYTE_PUP_4_REG     (0x18B0)
#define RESULT_CONTROL_PUP_4_BIT_0_REG    (0x1AB4)
#define RESULT_CONTROL_PUP_4_BIT_1_REG    (0x1AB8)
#define RESULT_CONTROL_PUP_4_BIT_2_REG    (0x1ABC)
#define RESULT_CONTROL_PUP_4_BIT_3_REG    (0x1AC0)
#define RESULT_CONTROL_PUP_4_BIT_4_REG    (0x1AC4)
#define RESULT_CONTROL_PUP_4_BIT_5_REG    (0x1AC8)
#define RESULT_CONTROL_PUP_4_BIT_6_REG    (0x1ACC)
#define RESULT_CONTROL_PUP_4_BIT_7_REG    (0x1AF0)

#endif

#define WL_PHY_REG                        (0x0)
#define WRITE_CENTRALIZATION_PHY_REG      (0x1)
#define RL_PHY_REG                        (0x2)
#define READ_CENTRALIZATION_PHY_REG       (0x3)
#define PBS_RX_PHY_REG       			  (0x50)
#define PBS_TX_PHY_REG       			  (0x10)
#define PHY_CONTROL_PHY_REG               (0x90)
#define BW_PHY_REG                        (0x92)
#define RATE_PHY_REG                      (0x94)
#define CMOS_CONFIG_PHY_REG               (0xA2)
#define PAD_ZRI_CALIB_PHY_REG             (0xA4)
#define PAD_ODT_CALIB_PHY_REG             (0xA6)
#define PAD_CONFIG_PHY_REG                (0xA8)
#define PAD_PRE_DISABLE_PHY_REG           (0xA9)
#define TEST_ADLL_REG                     (0xBF)
#define CSn_IOB_VREF_REG(cs)              (0xDB + (cs * 12))
#define CSn_IO_BASE_VREF_REG(cs)          (0xD0 + (cs * 12))

#define RESULT_DB_PHY_REG_ADDR			0xC0
#define RESULT_DB_PHY_REG_RX_OFFSET		5
#define RESULT_DB_PHY_REG_TX_OFFSET		0

#define PHY_WRITE_DELAY( _uiCs_ )            WL_PHY_REG /* TBD - for NP5 use only CS 0 */
/*( ( _uiCs_ == 0 ) ? 0x0 : 0x4 )*/
#define PHY_READ_DELAY( _uiCs_ )             RL_PHY_REG /* TBD - for NP5 use only CS 0 */


#define DDR0_ADDR_1                       (0xF8258)
#define DDR0_ADDR_2                       (0xF8254)
#define DDR1_ADDR_1                       (0xF8270)
#define DDR1_ADDR_2                       (0xF8270)
#define DDR2_ADDR_1                       (0xF825c)
#define DDR2_ADDR_2                       (0xF825c)
#define DDR3_ADDR_1                       (0xF8264)
#define DDR3_ADDR_2                       (0xF8260)
#define DDR4_ADDR_1                       (0xF8274)
#define DDR4_ADDR_2                       (0xF8274)

#define GENERAL_PURPOSE_RESERVED0_REG           0x182E0

#define GET_BLOCK_ID_MAX_FREQ( _devNum_ , _uiBlockId_ )   800000
#define CS0_RD_LVL_REF_DLY_OFFS   0
#define CS0_RD_LVL_REF_DLY_LEN   0
#define CS0_RD_LVL_PH_SEL_OFFS   0
#define CS0_RD_LVL_PH_SEL_LEN   0

#define CS_REGISTER_ADDR_OFFSET  4
#define CALIBRATED_OBJECTS_REG_ADDR_OFFSET   0x10

#define MAX_POLLING_ITERATIONS              (1000000)

#define NUM_BYTES                   (3)

#define IF_LOOP(cnt,start,end)    for( cnt = (start);  cnt <= (end); cnt++)
#define VALIDATE_IF_ACTIVE(mask,id)    if (IS_INTERFACE_ACTIVE(mask, id) ==  GT_FALSE)  { continue;   }
#define VALIDATE_BUS_ACTIVE(mask,id)    if (IS_BUS_ACTIVE(mask, id) ==  GT_FALSE)  { continue;   }

#define PHASE_REG_OFFSET   (32)
#define NUM_BYTES_IN_BURST (31)
#define NUM_OF_CS          (4)
#define CS_BYTE_GAP(csNum)   (csNum*0x4)
#define CS_PBS_GAP(csNum)   (csNum*0x10)
#define ADLL_LENGTH (32)

#ifdef CONFIG_DDR4
/* DDR4 MRS */
#define MRS4_CMD (0x10)
#define MRS5_CMD (0X11)
#define MRS6_CMD (0X12)

/* DDR4 Registers */
#define DDR4_MR0_REG                      (0x1900)
#define DDR4_MR1_REG                      (0x1904)
#define DDR4_MR2_REG                      (0x1908)
#define DDR4_MR3_REG                      (0x190C)
#define DDR4_MR4_REG                      (0x1910)
#define DDR4_MR5_REG                      (0x1914)
#define DDR4_MR6_REG                      (0x1918)
#define DDR4_MPR_WR_REG                   (0x19D0)
#define DRAM_PINS_MUX_REG                 (0x19D4)
#endif


/************************* Structures ***********************************************/

typedef struct
{
    MV_HWS_WL_SUPP  stage; 
    GT_BOOL         isPupFail;
}WriteSuppResultStruct;

typedef struct
{
   MV_HWS_PageSize ePageSize_8_BITS;
      /* page size in 8 bits bus width */
   MV_HWS_PageSize ePageSize_16_BITS;   
      /* page size in 16 bits bus width */
   GT_U32 uiPageMask;
      /* Mask used in register */
}PageElement;

#ifdef CONFIG_DDR4
/******************************************************************************
* Name:     ddr4TipDynamicWriteLevelingSupp
* Desc:     Write leveling phase correction
* Args:     devNum - device number
*
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS ddr4TipDynamicWriteLevelingSupp
(
	GT_U32 devNum
);

GT_STATUS    ddr4TipConfigurePhy
(
    GT_U32    devNum
);

GT_STATUS ddr4TipSetTiming
(
    GT_U32				    devNum,
    MV_HWS_ACCESS_TYPE      accessType,
    GT_U32					interfaceId,
    MV_HWS_DDR_FREQ			frequency
);

GT_STATUS ddr4ModeRegsInit
(
    GT_U8 devNum
);

GT_STATUS ddr4SdramConfig
(
    GT_U32 devNum
);

GT_STATUS    ddr4TipCalibrationAdjust
(
    GT_U32		devNum,
	GT_U8		Vref_en,
	GT_U8		POD_Only
);

GT_STATUS ddr3TipDDR4Ddr4TrainingMainFlow
(
    GT_U32 devNum
);

#endif

/******************************************************************************
* Name:     ddr3TipWriteLevelingStaticConfig.
* Desc:     Configure the write leveling parameters
* Args:     devNum     - channel id.
*           interfaceId - memory interface 
*           frequency   - frequency (Mhz)
*           roundTripDelayArr 
* Notes:    For Internal Use
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    ddr3TipWriteLevelingStaticConfig
( 
    GT_U32				devNum,
    GT_U32				interfaceId,
    MV_HWS_DDR_FREQ     frequency,
    GT_U32				*roundTripDelayArr 
);

/******************************************************************************
* Name:     ddr3TipReadLevelingStaticConfig.
* Desc:     Configure the read leveling parameters
* Args:     devNum     
*           interfaceId
*           frequency
*           totalRoundTripDelayArr
* Notes:    For Internal Use
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    ddr3TipReadLevelingStaticConfig
(
    GT_U32             devNum,
    GT_U32             interfaceId,
    MV_HWS_DDR_FREQ    frequency,
    GT_U32             *totalRoundTripDelayArr
);

/******************************************************************************
* Name:     mvHwsDdr3TipIFWrite.
* Desc:     Write to Interface.
* Args:     devNum           - channel id.
*           interfaceAccess  - Interface access type (multicast/unicast)
*           interfaceId      - INTERFACE ID. for broadcast use 0xFF
*           regAddr          - register address
*           dataValue        - data to write
*           mask
* Notes:    For Internal Use
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    mvHwsDdr3TipIFWrite
( 
    GT_U32                devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                dataValue,
    GT_U32                mask 
);


/******************************************************************************
* Name:     ddr3TipIfPolling.
* Desc:     Poll  Interface.
* Args:
* Notes:    For Internal Use
* Returns:  OK if success, other error code if fail.
*/

  GT_STATUS    ddr3TipIfPolling
( 
    GT_U32                devNum,
    MV_HWS_ACCESS_TYPE	  accessType,
    GT_U32                interfaceId,
    GT_U32                expValue,
    GT_U32                mask,
    GT_U32                offset,
    GT_U32                pollTries
);

/******************************************************************************
* Name:     mvHwsDdr3TipIFRead.
* Desc:     Read from Interface.
* Args:     
* Notes:    For Internal Use    In case of multicast read, the return variable should be an array
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    mvHwsDdr3TipIFRead
( 
    GT_U32                devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                *data,
    GT_U32                mask 
);


GT_STATUS    ddr3TipBusReadModifyWrite
( 
    GT_U32      devNum,
    MV_HWS_ACCESS_TYPE		accessType,
    GT_U32      interfaceId, 
    GT_U32      phyId, 
    MV_HWS_DDR_PHY     phyType, 
    GT_U32      regAddr, 
    GT_U32      dataValue,
    GT_U32      regMask 
);


/******************************************************************************
* Name:     mvHwsDdr3TipBUSRead.
* Desc:     Read from phy.
* Args:     
* Notes:    For Internal Use    In case of multicast read, the return variable should be an array
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    mvHwsDdr3TipBUSRead
( 
    GT_U32              devNum,
    GT_U32              interfaceId, 
    MV_HWS_ACCESS_TYPE  phyAccess,
    GT_U32              phyId, 
    MV_HWS_DDR_PHY      phyType, 
    GT_U32              regAddr, 
    GT_U32              *data  
);


/******************************************************************************
* Name:     mvHwsDdr3TipBUSWrite.
* Desc:     Write to phy.
* Args:     uiChannel     - channel id.
*           eInterfaceAccess  - Interface unit access (unicast/multicast)
*           uiInterfaceId     - INTERFACE ID. for broadcast use 0xFF
*           ePhyAccess    - Phy unit access (unicast/multicast)
*           uiPhyId       - PHY ID. for broadcast use 0xFF
*           ePhyType      - phy Type (data/Control)
*           regAddr     - register address
*           regData        - data to write
* Notes:    For Internal Use
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    mvHwsDdr3TipBUSWrite
(  
    GT_U32              devNum,
    MV_HWS_ACCESS_TYPE  eInterfaceAccess,
    GT_U32              uiInterfaceId, 
    MV_HWS_ACCESS_TYPE  ePhyAccess,
    GT_U32              uiPhyId, 
    MV_HWS_DDR_PHY      ePhyType, 
    GT_U32              regAddr, 
    GT_U32              dataValue 
);

/******************************************************************************
* Name:     ddr3TipFreqSet
* Desc:     Set frequency of the DDR memory.
* Args:     devNum       - channel id.
*           eAccess           - unicast/multicast
*           uiInterfaceId     - INTERFACE ID. for broadcast use 0xFF
*           eMemoryType       - memory type  
*           memoryFreq        - target frequency
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipFreqSet
( 
    GT_U32        devNum,
    MV_HWS_ACCESS_TYPE    eAccess,
    GT_U32        uiInterfaceId,
    MV_HWS_DDR_FREQ       memoryFreq 
);

/******************************************************************************
* Name:     ddr3TipAdjustDqs
* Desc:     Adjust DQS Bit
* Args:     devNum       - channel id.
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS ddr3TipAdjustDqs
(
    GT_U32        devNum
);


/******************************************************************************
* Name:     ddr3TipInitController.
* Desc:     Initilize DDR Controller
* Args:     devNum - channel ID 
* Notes:    For Internal Use
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    ddr3TipInitController( GT_U32    devNum);



/******************************************************************************
* Name:     ddr3TipExtRead.
* Desc:     read from external memory
* Args:     devNum - channel ID 
*         uiInterfaceId - interface ID
*         regAddr - memory address
*         uiNumOfBursts - number of bursts to write, each burst is 8*4 bytes
*         puiAddr - pointer to array of 32bits register, where we insert the data read from memory 
* Notes:    For internal use only
* Returns:  OK if success, other error code if fail.
*/

GT_STATUS    ddr3TipExtRead
(  
    GT_U32      devNum,
    GT_U32      uiInterfaceId,
    GT_U32      regAddr,
    GT_U32      uiNumOfBursts,
    GT_U32*     puiAddr
);

/******************************************************************************
* Name:     ddr3TipExtWrite.
* Desc:     write to external memory
* Args:     devNum - channel ID 
*         uiInterfaceId - interface ID
*         regAddr - memory address
*         uiNumOfBursts - number of bursts to write, each burst is 8*4 bytes
*         puiAddr - pointer to array of 32bits register to be written into memory
* Notes:    For internal use only
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipExtWrite
( 
    GT_U32      devNum,
    GT_U32      uiInterfaceId,
    GT_U32      regAddr,
    GT_U32      uiNumOfBursts,
    GT_U32*     puiAddr
);


/******************************************************************************
* Name:     ddr3TipDDR3DynamicReadLeveling.
* Desc:     Dynmaic read leveling
* Args:     devNum - device number
*           uiFreq
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipDynamicReadLeveling(  GT_U32               devNum,
                                          GT_U32              uiFreq);
/******************************************************************************
* Name:     ddr3TipDDR3LegacyDynamicReadLeveling.
* Desc:     Dynmaic read leveling
* Args:     devNum - device number
*           uiFreq
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipLegacyDynamicReadLeveling(  GT_U32               devNum);


/******************************************************************************
* Name:     ddr3TipDDR3DynamicPerBitReadLeveling.
* Desc:     Dynmaic read leveling
* Args:     devNum - device number
*           uiFreq
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipDynamicPerBitReadLeveling(  GT_U32               devNum,
                                                GT_U32              uiFreq);


/******************************************************************************
* Name:     ddr3TipLegacyDynamicWriteLeveling.
* Desc:     Dynmaic write leveling
* Args:     devNum - device number
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipLegacyDynamicWriteLeveling(  GT_U32    devNum);

/******************************************************************************
* Name:     ddr3TipDynamicWriteLeveling.
* Desc:     Dynmaic write leveling
* Args:     devNum - device number
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipDynamicWriteLeveling(  GT_U32    devNum);

/******************************************************************************
* Name:     ddr3TipDynamicWriteLevelingSupp.
* Desc:     Dynamic write leveling supplementary
* Args:     devNum
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipDynamicWriteLevelingSupp(GT_U32    devNum);

/******************************************************************************
* Name:     ddr3TipStaticInitController.
* Desc:     Init controller with predefined values 
* Args:     devNum - channel ID 
* Notes:    For internal use only
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipStaticInitController( GT_U32    devNum );

/******************************************************************************
* Name:     ddr3TipConfigurePhy.
* Desc:     Init phy controller with predefined values 
* Args:     devNum - channel ID 
* Notes:    For internal use only
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipConfigurePhy( GT_U32    devNum );

/******************************************************************************
* Name:     ddr3TipLoadPatternToOdpg.
* Desc:     Load pattern to ODPG 
* Args:     devNum - channel ID 
*           ePattern
* Notes:    For internal use only
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipLoadPatternToOdpg
(   
    GT_U32              devNum,
    MV_HWS_ACCESS_TYPE  accessType,
    GT_U32              uiInterfaceId,
    MV_HWS_PATTERN      pattern,
	GT_U32              loadAddr
);

/******************************************************************************
* Name:     ddr3TipLoadPatternToMem.
* Desc:     Load expected Pattern to external memory
* Args:     TBD
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipLoadPatternToMem
(  
	GT_U32	        devNum,
	MV_HWS_PATTERN  ePattern
);

/******************************************************************************
* Name:     ddr3TipConfigureOdpg.
* Desc:     TBD
* Args:     TBD
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipConfigureOdpg
(      
	GT_U32					devNum,
	MV_HWS_ACCESS_TYPE		accessType,
	GT_U32                  interfaceId,
	MV_HWS_DIRECTION    	direction,
	GT_U32                  txPhases,
	GT_U32                  txBurstSize,
	GT_U32                  uiRxPhases,
	GT_U32                  delayBetweenBurst,
	GT_U32                  rdMode,
	GT_U32                  csNum,
	GT_U32                  addrStressJump,
	GT_U32                  singlePattern
);


/******************************************************************************
* Init Debug Variables
*/
 GT_STATUS  ddr3TipSetAtr
(
    GT_U32 devNum,
    GT_U32 flagId, 
    GT_U32 value
);

 /******************************************************************************
* Name:     ddr3TipWriteMRSCmd.
* Desc:     Write MRS 1
* Args:     TBD
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipWriteMRSCmd
(
    GT_U32        devNum,
    GT_U32        *csMaskArr,
    GT_U32        cmd,
    GT_U32        data,
    GT_U32        mask
);

 /******************************************************************************
* Name:     ddr3TipIFRead.
* Desc:     
* Args:     
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipIFRead
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                *data,
    GT_U32                mask 
);

 /******************************************************************************
* Name:     ddr3TipIFWrite.
* Desc:     
* Args:     
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipIFWrite
(
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                dataValue,
    GT_U32                mask 
);

 /******************************************************************************
* Name:     ddr3TipWriteCsResult.
* Desc:     
* Args:     
* Notes:    
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS    ddr3TipWriteCsResult
(
    GT_U32 devNum,
    GT_U32 offset
);

/******************************************************************************
* Name:     ddr3TipGetFirstActiveIf.
* Desc:     returns the first active interface
* Args:
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS ddr3TipGetFirstActiveIf
(
    GT_U8   devNum,
    GT_U32  interfaceMask,
    GT_U32  *ifId
);



GT_STATUS GetCentrlization(GT_32 direction, GT_32 adll[12*4]);

GT_BOOL GetRL(GT_32 RL[12*4]);

GT_BOOL GetWL(GT_32 WL[12*4]);

GT_STATUS    ddr3TipResetFifoPtr(GT_U32 devNum);

GT_BOOL mvHwsDdr3TipReadPupValue(GT_U32 devNum, GT_32 PupValues[MAX_INTERFACE_NUM*MAX_BUS_NUM], int regAddr, GT_U32 mask);

GT_BOOL mvHwsDdr3TipReadAdllValue(GT_U32 devNum,GT_U32 PupValues[MAX_INTERFACE_NUM*MAX_BUS_NUM], int regAddr, GT_U32 mask);

GT_BOOL mvHwsDdr3TipWriteAdllValue(GT_U32 devNum,GT_U32 PupValues[MAX_INTERFACE_NUM*MAX_BUS_NUM], int regAddr);


/******************************************************************************
* Name:     ddr3TipTuneTrainingParams.
* Desc:     Set key training params
* Args:
* Notes:
* Returns:  OK if success, other error code if fail.
*/
GT_STATUS ddr3TipTuneTrainingParams
(
    GT_U32                  devNum,
    GT_TUNE_TRAINING_PARAMS *params
);

#ifdef __cplusplus
   }
#endif

#endif /* _MV_DDR3_TRAININGIP_FLOW_H_ */


