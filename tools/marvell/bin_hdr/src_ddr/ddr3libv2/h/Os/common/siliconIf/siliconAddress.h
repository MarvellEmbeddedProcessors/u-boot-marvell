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
* mvHwsLion2If.c
*
* DESCRIPTION:
*       Lion2 specific HW Services init
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*
*******************************************************************************/

#ifndef __Lion2_AddressEnums_h
#define __Lion2_AddressEnums_h


typedef enum lion2Regs
{
    MG_Media_Interface_Reg = 0x38C,

/********* Addresses for UNIT port ********/
    GIG_PORT_MAC_CONTROL_REGISTER0 = 0,
    GIG_PORT_MAC_CONTROL_REGISTER1 = 4,
    GIG_PORT_MAC_CONTROL_REGISTER2 = 8,
    GIG_Port_Auto_Negotiation_Configuration = 0xC,
    GIG_Port_Status_Reg = 0x10,
    GIG_Port_PRBS_Status_Reg = 0x38,
    GIG_Port_PRBS_Error_Cntr_Reg = 0x3C,

/********* Addresses for TABLE MSM_Regs.cfg ********/

    MSM_PORT_MAC_CONTROL_REGISTER0 = 0,
    MSM_PORT_MAC_CONTROL_REGISTER1 = 0x4,
    MSM_PORT_MAC_CONTROL_REGISTER2 = 0x8,
    MSM_PORT_STATUS = 0xc,
    MSM_PORT_FIFOS_THRESHOLDS_CONFIGURATION = 0x10,
    MSM_PORT_INTERRUPT_CAUSE = 0x14,
    MSM_PORT_INTERRUPT_MASK = 0x18,
    MSM_PORT_MAC_CONTROL_REGISTER3 = 0x1c,
    MSM_PORT_PER_PRIO_FC_STATUS = 0x20,
    DEBUG_BUS_STATUS = 0x24,
    MSM_PORT_METAL_FIX = 0x2c,
    MSM_MIB_COUNTERS_CONTROL_REGISTER = 0x30,
    MSM_CCFC_FC_TIMERP = 0x38,
    EXTERNAL_UNITS_INTERRUPTS_CAUSE = 0x58,
    EXTERNAL_UNITS_INTERRUPTS_MASK = 0x5c,
    MSM_PPFC_CONTROL = 0x60,
    MSM_LLFC_DSA_TAG_REGISTER_0 = 0x68,
    MSM_LLFC_DSA_TAG_REGISTER_1 = 0x6c,
    MSM_LLFC_DSA_TAG_REGISTER_2 = 0x70,
    MSM_LLFC_DSA_TAG_REGISTER_3 = 0x74,
    MSM_LLFC_FLOW_CONTROL_WINDOW_REG_0 = 0x78,
    MSM_LLFC_FLOW_CONTROL_WINDOW_REG_1 = 0x7c,
    MSM_PORT_MAC_CONTROL_REGISTER4 = 0x84,
    MSM_PORT_MAC_CONTROL_REGISTER5 = 0x88,
    EXTERNAL_CONTROL = 0x90,
    MACRO_CONTROL = 0x94,

    XPCS_Global_Configuration_Reg0 = 0,
    XPCS_Global_Configuration_Reg1 = 0x4,
    XPCS_Global_Fifo_Threshold_Configuration = 0x8,
    XPCS_Global_Status_Reg = 0x10,
    XPCS_Global_Deskew_Error_Counter = 0x20,

    XPCS_Tx_Packets_Cntr_LSB = 0x30,
    XPCS_Tx_Packets_Cntr_MSB = 0x34,
    XPCS_Internal_Metal_Fix = 0x40,
    XPCS_Rx_Bad_Packets_Cntr_LSB_Lane0 = 0x7C,
    XPCS_Rx_Bad_Packets_Cntr_MSB_Lane0 = 0x80,
    XPCS_Rx_Bad_Packets_Cntr_LSB_Lane1 = 0xC0,
    XPCS_Rx_Bad_Packets_Cntr_MSB_Lane1 = 0xC4,
    XPCS_Rx_Bad_Packets_Cntr_LSB_Lane2 = 0x104,
    XPCS_Rx_Bad_Packets_Cntr_MSB_Lane2 = 0x108,
    XPCS_Rx_Bad_Packets_Cntr_LSB_Lane3 = 0x148,
    XPCS_Rx_Bad_Packets_Cntr_MSB_Lane3 = 0x14C,
    XPCS_Rx_Bad_Packets_Cntr_LSB_Lane4 = 0x18C,
    XPCS_Rx_Bad_Packets_Cntr_MSB_Lane4 = 0x190,
    XPCS_Rx_Bad_Packets_Cntr_LSB_Lane5 = 0x1D0,
    XPCS_Rx_Bad_Packets_Cntr_MSB_Lane5 = 0x1D4,
    XPCS_Lane0_configuration_Reg = 0x50,
    XPCS_Lane1_configuration_Reg = 0x94,
    XPCS_Lane2_configuration_Reg = 0xD8,
    XPCS_Lane3_configuration_Reg = 0x11C,
    XPCS_Lane4_configuration_Reg = 0x160,
    XPCS_Lane5_configuration_Reg = 0x1A4,

    XPCS_PRBS_Error_Cntr_Lane0 = 0x70,
    XPCS_PRBS_Error_Cntr_Lane1 = 0xB4,
    XPCS_PRBS_Error_Cntr_Lane2 = 0xF8,
    XPCS_PRBS_Error_Cntr_Lane3 = 0x13C,
    XPCS_PRBS_Error_Cntr_Lane4 = 0x180,
    XPCS_PRBS_Error_Cntr_Lane5 = 0x1C4,
    XPCS_Lane_Status_Lane0 = 0x5C,
    XPCS_Lane_Status_Lane1 = 0xA0,
    XPCS_Lane_Status_Lane2 = 0xE4,
    XPCS_Lane_Status_Lane3 = 0x128,
    XPCS_Lane_Status_Lane4 = 0x16C,
    XPCS_Lane_Status_Lane5 = 0x1B0,


/********* Addresses for TABLE MMPCS_Regs.cfg ********/
    PCS40G_COMMON_INTERRUPT_CAUSE = 0x8,
    PCS40G_COMMON_INTERRUPT_MASK = 0xc,
    PCS40G_ALIGNMENT_CYCLE_CONTROL = 0x10,
    PCS40G_COMMON_CONTROL = 0x14,
    TX_IDLE_REMOVAL_THRESHOLDS = 0x20,
    TX_IDLE_INSERTION_THRESHOLDS = 0x24,
    RX_IDLE_REMOVAL_THRESHOLDS = 0x28,
    RX_IDLE_INSERTION_THRESHOLDS = 0x2c,
    PCS40G_COMMON_STATUS = 0x30,
    RX_PPM_FIFO_COUNTERS = 0x34,
    TX_PPM_FIFO_COUNTERS = 0x38,
    TEST_PATTERN_CONFIGURATION = 0x3c,
    TEST_PATTERN_ERROR_COUNT = 0x40,
    CHANNEL0_BER_COUNTER = 0x44,
    CHANNEL1_BER_COUNTER = 0x48,
    CHANNEL2_BER_COUNTER = 0x4c,
    CHANNEL3_BER_COUNTER = 0x50,
    CHANNEL0_BIP_COUNTER = 0x54,
    CHANNEL1_BIP_COUNTER = 0x58,
    CHANNEL2_BIP_COUNTER = 0x5c,
    CHANNEL3_BIP_COUNTER = 0x60,
    CHANNEL0_BIP_NOT_COUNTER = 0x64,
    CHANNEL1_BIP_NOT_COUNTER = 0x68,
    CHANNEL2_BIP_NOT_COUNTER = 0x6c,
    CHANNEL3_BIP_NOT_COUNTER = 0x70,
    CHANNEL_CONFIGURATION = 0x74,
    CHANNEL0_RX_GB_FIFO_CONFIGURATION = 0x78,
    CHANNEL1_RX_GB_FIFO_CONFIGURATION = 0x7c,
    CHANNEL2_RX_GB_FIFO_CONFIGURATION = 0x80,
    CHANNEL3_RX_GB_FIFO_CONFIGURATION = 0x84,
    LANE0_ALIGNMENT_MARKER_0 = 0x88,
    LANE0_ALIGNMENT_MARKER_1 = 0x8c,
    LANE0_ALIGNMENT_MARKER_2 = 0x90,
    LANE0_ALIGNMENT_MARKER_3 = 0x94,
    LANE1_ALIGNMENT_MARKER_0 = 0x98,
    LANE1_ALIGNMENT_MARKER_1 = 0x9c,
    LANE1_ALIGNMENT_MARKER_2 = 0xa0,
    LANE1_ALIGNMENT_MARKER_3 = 0xa4,
    LANE2_ALIGNMENT_MARKER_0 = 0xa8,
    LANE2_ALIGNMENT_MARKER_1 = 0xac,
    LANE2_ALIGNMENT_MARKER_2 = 0xb0,
    LANE2_ALIGNMENT_MARKER_3 = 0xb4,
    LANE3_ALIGNMENT_MARKER_0 = 0xb8,
    LANE3_ALIGNMENT_MARKER_1 = 0xbc,
    LANE3_ALIGNMENT_MARKER_2 = 0xc0,
    LANE3_ALIGNMENT_MARKER_3 = 0xc4,
    GEARBOX_STATUS = 0xd0,
    METAL_FIX = 0xd4,
    FEC_DEC_DMA_CONTROL = 0xd8,
    FEC_DEC_DMA_WR_DATA = 0xdc,
    FEC_DEC_DMA_RD_DATA = 0xe0,
    FEC_DEC_CONFIGURATION_REG_0 = 0xe4,
    FEC_DEC_STATUS_REG = 0xe8,
    FDEC_RECEIVED_BLOCKS_REG_0 = 0xec,
    FDEC_RECEIVED_BLOCKS_REG_1 = 0xf0,
    FDEC_RECEIVED_NERROR_REG_0 = 0xf4,
    FDEC_RECEIVED_NERROR_REG_1 = 0xf8,
    FDEC_RECEIVED_CERROR_REG_0 = 0xfc,
    FDEC_RECEIVED_CERROR_REG_1 = 0x100,
    FDEC_RECEIVED_UNCERROR_REG_0 = 0x104,
    FDEC_RECEIVED_UNCERROR_REG_1 = 0x108,
    FDEC_RECEIVED_CERROR_BITS_REG_0 = 0x10c,
    FDEC_RECEIVED_CERROR_BITS_REG_1 = 0x110,
    FDEC_RECEIVED_FERROR_REG_0 = 0x114,
    FDEC_RECEIVED_FERROR_REG_1 = 0x118,
    FDEC_RG_LAST_P_S = 0x11c,
    FDEC_RG_LAST_E_S = 0x120,
    FENC_CONTROL_REG_0 = 0x128,
    FENC_CONTROL_REG_1 = 0x12c,
    FENC_CONTROL_REG_2 = 0x130,
    FENC_BLCK_NUM_LSB = 0x134,
    FENC_BLCK_NUM_MSB = 0x138,
    AP_CONFIGURATION_REG_0 = 0x13c,
    AP_CONFIGURATION_REG_1 = 0x140,
    AP_STATUS_REG_0 = 0x144,
    AP_STATUS_REG_1 = 0x148,
    PCS_RESET_REG = 0x14c,
    PRBS_CONTROL_REG0 = 0x150,
    PRBS_CONTROL_REG1 = 0x154,
    PRBS_STATUS_REG0 = 0x158,
    TEST_PATTERN_SEED_A_REG_0 = 0x15c,
    TEST_PATTERN_SEED_A_REG_1 = 0x160,
    TEST_PATTERN_SEED_A_REG_2 = 0x164,
    TEST_PATTERN_SEED_A_REG_3 = 0x168,
    TEST_PATTERN_SEED_B_REG_0 = 0x16c,
    TEST_PATTERN_SEED_B_REG_1 = 0x170,
    TEST_PATTERN_SEED_B_REG_2 = 0x174,
    TEST_PATTERN_SEED_B_REG_3 = 0x178,
    ENCODER_CONFIGURATION_REG = 0x17c,

/********* Addresses for TABLE SD_Regs.cfg ********/
    SERDES_EXTERNAL_CONFIGURATION_0 = 0x0,
    SERDES_EXTERNAL_CONFIGURATION_1 = 0x4,
    SERDES_EXTERNAL_CONFIGURATION_2 = 0x8,
    SERDES_EXTERNAL_CONFIGURATION_3 = 0xc,
    SD_METAL_FIX = 0x14,
    SERDES_EXTERNAL_STATUS_0 = 0x18,
    SERDES_EXTERNAL_STATUS_1 = 0x1c,
    SERDES_EXTERNAL_CONFIGURATION_4 = 0x20,
    SERDES_EXTERNAL_STATUS_2 = 0x24,
    SERDES_RESERVED_REGISTER_0 = 0x28,
    SERDES_RESERVED_REGISTER_1 = 0x2c,
    SERDES_RESERVED_REGISTER_2 = 0x30,
    SERDES_RESERVED_STATUS_0 = 0x34,
    SERDES_PHY_REGS = 0x200

}MV_LION2_REGS;

#define CGMAC_IND_OFFSET (0x300000)
#define PUMA3_CG_BASE (0x01800000)

typedef enum puma3Regs
{
    /* CG_UNIT */
    CG_CONTROL_0 = 0x0,
    CG_RESETS = 0x10,
    CG_RXDMA_CONVERTER_PACKET_SIZE_RESTRICTION = 0x14,
    CG_CREDIT_LIMIT = 0x1c,
    CG_CONVERTERS_STATUS = 0x20,
    CG_RECEIVED_PACKET_STATUS = 0x24,
    CG_IP_STATUS = 0x28,
    CG_RX_OVERSIZE_DROPPED_PACKET_COUNTER = 0x30,
    CG_RX_UNDERSIZE_DROPPED_PACKET_COUNTER = 0x34,
    CG_CG_INTERRUPT_CAUSE = 0x40,
    CG_CG_INTERRUPT_MASK = 0x44,
    CG_FC_CONTROL_0 = 0x60,
    CG_LLFC_WINDOW = 0x64,
    CG_FC_SA_ADDRESS_LOW = 0x68,
    CG_FC_SA_ADDRESS_HIGH = 0x6c,
    CG_FC_DSA_TAG_REGISTER_0 = 0x70,
    CG_FC_DSA_TAG_REGISTER_1 = 0x74,
    CG_CCFC_FC_TIMER0 = 0x80,
    CG_CCFC_FC_TIMER1 = 0x84,
    CG_CCFC_FC_TIMER2 = 0x88,
    CG_CCFC_FC_TIMER3 = 0x8c,
    CG_FEC_SYNC_STATUS = 0x90,
    CG_FEC_CERROR_COUNTER = 0xa0,
    CG_MULTI_LANE_ALIGN_STATUS1 = 0xC8,
    CG_MULTI_LANE_ALIGN_STATUS2 = 0xCC,
    CG_FEC_NCERROR_COUNTER = 0x100,
    CG_COMMAND_CONFIG = 0x1008,
    CG_FRM_LENGTH = 0x1014,
    CG_TX_FIFO_SECTIONS = 0x1020,
    CG_TX_IPG_LENGTH = 0x1044,
    CG_INIT_CREDIT = 0x104C,
    CG_CONTROL_1 = 0x2000,

    /* Global ILKN unit INTLKN_UNIT WO */
    ILKN_0_MAC_CFG_0 = 0,
    ILKN_0_CH_FC_CFG_0 = 4,
    ILKN_0_MAC_CFG_2 = 0xC,
    ILKN_0_MAC_CFG_3 = 0x10,
    ILKN_0_MAC_CFG_4 = 0x14,
    ILKN_0_MAC_CFG_6 = 0x1C,
    ILKN_0_CH_FC_CFG_1 = 0x60,
    ILKN_0_ALIGN_STATUS = 0x10C,
    ILKN_0_PCS_CFG_0 = 0x200,
    ILKN_0_PCS_CFG_1 = 0x204,
    ILKN_0_EN = 0x20C,
    ILKN_0_STAT_EN = 0x238,

    /* Fabric Interlaken unit */
    GENX_converter_control_0 = 0,
    ILKN_RESETS = 0x4,
    SD_FC_LED_converter_control_0 = 0x10,
    TXDMA_converter_control_0 = 0x20,
    RXDMA_converter_control_0 = 0x40,

    /* ETI (external TCAM) unit */
    General_ETI_configuration0 = 0,
    General_ETI_configuration1 = 0x4,

    HWS_LAST_REG_ADDRESS
}MV_PUMA3_REGS;
#endif /* __Lion2_AddressEnums_h*/
