/* AAPL CORE Revision: 2.1.0 */

/* Copyright 2014 Avago Technologies. All rights reserved.                   */
/*                                                                           */
/* This file is part of the AAPL CORE library.                               */
/*                                                                           */
/* AAPL CORE is free software: you can redistribute it and/or modify it      */
/* under the terms of the GNU Lesser General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or         */
/* (at your option) any later version.                                       */
/*                                                                           */
/* AAPL CORE is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU Lesser General Public License for more details.                       */
/*                                                                           */
/* You should have received a copy of the GNU Lesser General Public License  */
/* along with AAPL CORE.  If not, see http://www.gnu.org/licenses.           */

/* Support for AAPL (ASIC and ASSP Programming Layer) generic handling of */
/* SerDes (Serializer/Deserializer) slices on ASIC SBus rings. */

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include <aapl.h>

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#define AAPL_LOG_PRINT5 if(aapl->debug >= 5) aapl_log_printf
#define AAPL_LOG_PRINT6 if(aapl->debug >= 6) aapl_log_printf
#else
#define AAPL_LOG_PRINT5 aapl_log_printf
#define AAPL_LOG_PRINT6 aapl_log_printf
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
#ifdef MV_HWS_BIN_HEADER
Avago_serdes_init_config_t avagoSerdesInitStruct;
#endif
/** @defgroup SerDes Generic SerDes API */
/** @{ */

/** @brief   Gets the SerDes LSB revision. */
/** @details Reads the revision of SerDes LSB using an AVAGO_LSB_DIRECT read. */
/** @return  Revision of the LSB (> 0), or -1 on error. */
/** @see avago_firmware_get_rev(), avago_firmware_get_build_id(). */
int avago_serdes_get_lsb_rev(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr)          /**< [in] Device address number. */
{
    int rc = -1;
    if (!aapl_check_process(aapl, addr, __func__, __LINE__, TRUE, 3, AVAGO_PROCESS_B, AVAGO_PROCESS_F, AVAGO_PROCESS_A)) return rc;

    if( aapl_check_ip_type(aapl, addr, __func__, __LINE__, TRUE, 2, AVAGO_SERDES, AVAGO_M4))
    {
        if (aapl_check_process(aapl, addr, __func__, __LINE__, FALSE, 2, AVAGO_PROCESS_B, AVAGO_PROCESS_F))
        {
            rc = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB_DIRECT, 0x026);
            rc = (rc >> 12) & 0xf;
        }
#ifndef MV_HWS_REDUCED_BUILD
        else if (aapl_check_process(aapl, addr, __func__, __LINE__, FALSE, 1, AVAGO_PROCESS_A))
        {
            rc = (avago_sbus_rd(aapl, addr, 0x64) >> 28) & 0xf;
        }
#endif /* MV_HWS_REDUCED_BUILD */
    }

    AAPL_LOG_PRINT5(aapl, AVAGO_DEBUG5, __func__, __LINE__, "SBus %s, ret = %d.\n", aapl_addr_to_str(addr), rc);
    return rc;
}


/** @brief   Reads a memory location from a SerDes device. */
/** */
/** @return  Value read.  On error, aapl->return_code is set to negative value. */
/** @see     avago_serdes_mem_wr(), avago_serdes_mem_rmw(). */
int avago_serdes_mem_rd(
    Aapl_t *aapl,                   /**< [in] Pointer to Aapl_t structure. */
    uint addr,                      /**< [in] Device address number. */
    Avago_serdes_mem_type_t type,   /**< [in] The memory access type. */
    uint mem_addr)                  /**< [in] Memory address to access */
{
    int ret = 0;
#ifndef MV_HWS_REDUCED_BUILD
    if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
    {
        if(      type == AVAGO_ESB )        type = AVAGO_LSB;
        else if( type == AVAGO_ESB_DIRECT ) type = AVAGO_LSB_DIRECT;
    }
#endif /* MV_HWS_REDUCED_BUILD */
    switch( type )
    {
    case AVAGO_LSB:
        if( aapl_get_spico_running_flag(aapl,addr) ||
            aapl_set_spico_running_flag(aapl,addr, avago_spico_running(aapl, addr)) )
        {
            ret = avago_spico_int(aapl, addr, (1 << 14) | (mem_addr & 0x3fff), 0x00);
            break;
        }

    case AVAGO_LSB_DIRECT:
#ifndef MV_HWS_REDUCED_BUILD
        if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
        {
            avago_sbus_wr(aapl, addr, 0x01, (1<<30) | (mem_addr & 0x03ff));
            ret = (avago_sbus_rd(aapl, addr, 0x01) >> 12) & 0xFFFF;
        } else
#endif /* MV_HWS_REDUCED_BUILD */
        {
            avago_sbus_wr(aapl, addr, 0x02, ((mem_addr & 0x1ff) << 16));
            ret = avago_sbus_rd(aapl, addr, 0x40) & 0xFFFF;
        }
        break;

    case AVAGO_ESB:
        if( aapl_get_spico_running_flag(aapl,addr) ||
            aapl_set_spico_running_flag(aapl,addr, avago_spico_running(aapl, addr)) )
        {
            avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x18, 0x4000 | (mem_addr & 0x3fff));
            ret = avago_spico_int(aapl, addr, 0x1a, 0x00);
            break;
        }

    case AVAGO_ESB_DIRECT:
    {
        int lsb_esb_addr;
        int spico_run_state = avago_spico_halt(aapl, addr);

        int reset_status = avago_sbus_rd(aapl, addr, 0x07);
        if( reset_status & 0x01 )
            avago_sbus_wr(aapl, addr, 0x07, reset_status & ~1);

        lsb_esb_addr = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB_DIRECT, 0x030);

        if( lsb_esb_addr & 0x8000 )
            avago_serdes_mem_wr(aapl, addr, AVAGO_LSB_DIRECT, 0x030, lsb_esb_addr & 0x7fff);

        avago_serdes_mem_wr(aapl, addr, AVAGO_LSB_DIRECT, 0x030, mem_addr);
        ret = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB_DIRECT, 0x032);

        if( lsb_esb_addr & 0x8000 )
            avago_serdes_mem_wr(aapl, addr, AVAGO_LSB_DIRECT, 0x030, lsb_esb_addr & 0x7fff);
        avago_serdes_mem_wr(    aapl, addr, AVAGO_LSB_DIRECT, 0x030, lsb_esb_addr);

        if( reset_status & 0x01 )
            avago_sbus_wr(aapl, addr, 0x07, reset_status);
        avago_spico_resume(aapl, addr, spico_run_state);
        break;
    }
#ifndef MV_HWS_REDUCED_BUILD
    case AVAGO_DMEM:
    case AVAGO_DMEM_PREHALTED:
    {
        int spico_run_state, initial_value_0x01;
        if (type == AVAGO_DMEM)  spico_run_state = avago_spico_halt(aapl, addr);
        initial_value_0x01 =    avago_sbus_rmw(aapl, addr, 0x01, 0x40000000 | (mem_addr & 0x3ff), 0x400003ff);
        ret =                  (avago_sbus_rd(aapl, addr, 0x01) >> 12) & 0xffff;
                                avago_sbus_wr(aapl, addr, 0x01, initial_value_0x01);
        if (type == AVAGO_DMEM) avago_spico_resume(aapl, addr, spico_run_state);
        break;
    }

    case AVAGO_IMEM:
    case AVAGO_IMEM_PREHALTED:
    {
        int spico_run_state;
        if (type == AVAGO_IMEM) spico_run_state = avago_spico_halt(aapl, addr);
        avago_sbus_wr(aapl, addr, 0x00, (mem_addr & 0xffff) | 1 << 30);
        ret = (avago_sbus_rd(aapl, addr, 0x00) >> 16 ) & 0x3ff;
        avago_sbus_wr(aapl, addr, 0x00, 0x00);
        if (type == AVAGO_IMEM) avago_spico_resume(aapl, addr, spico_run_state);
        break;
    }
#endif /* MV_HWS_REDUCED_BUILD */
    default:
        aapl_fail(aapl, __func__, __LINE__, "SBus %s, Invalid DMA type (%d).\n", aapl_addr_to_str(addr), type);
        return 0;
    }
    AAPL_LOG_PRINT6(aapl,AVAGO_DEBUG6,__func__,__LINE__,"SBus %s, Read  %s 0x%04x -> 0x%x\n",
        aapl_addr_to_str(addr), aapl_mem_type_to_str(type), mem_addr, ret);
    return ret;
}

/** @brief   Writes data to a memory location. */
/** */
/** @return  On error, decrements aapl->return_code. */
/** @see     avago_serdes_mem_rd(), avago_serdes_mem_rmw(). */
void avago_serdes_mem_wr(
    Aapl_t *aapl,                   /**< [in] Pointer to Aapl_t structure. */
    uint addr,                      /**< [in] Device address number. */
    Avago_serdes_mem_type_t type,   /**< [in] The memory access type. */
    uint mem_addr,                  /**< [in] Memory address to access */
    uint data)                      /**< [in] Data to write into memory location addr. */
{
    AAPL_LOG_PRINT6(aapl,AVAGO_DEBUG6,__func__,__LINE__,"SBus %s, Write %s 0x%04x <- 0x%x\n",
        aapl_addr_to_str(addr), aapl_mem_type_to_str(type), mem_addr, data);
#ifndef MV_HWS_REDUCED_BUILD
    if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
    {
        if(      type == AVAGO_ESB )        type = AVAGO_LSB;
        else if( type == AVAGO_ESB_DIRECT ) type = AVAGO_LSB_DIRECT;
    }
#endif /* MV_HWS_REDUCED_BUILD */
    switch( type )
    {
    case AVAGO_LSB:
        if( aapl_get_spico_running_flag(aapl,addr) ||
            aapl_set_spico_running_flag(aapl,addr, avago_spico_running(aapl, addr)) )
        {
            avago_spico_int(aapl, addr, (2 << 14) | (mem_addr & 0x3fff), data);
            return;
        }

    case AVAGO_LSB_DIRECT:
        if(aapl_get_lsb_rev(aapl,addr) >= 3)
        {
            if(!( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) ))
              avago_sbus_wr(aapl, addr, 0x02, (0<<31) | ((mem_addr & 0x1ff) << 16) | (data & 0xffff));
        }
#ifndef MV_HWS_REDUCED_BUILD
        if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
            avago_sbus_wr(aapl, addr, 0x01, (1<<31) | ((data & 0xffff) << 12) | (mem_addr & 0x03ff));
#endif /* MV_HWS_REDUCED_BUILD */
        else
            avago_sbus_wr(aapl, addr, 0x02, (1<<31) | ((mem_addr & 0x1ff) << 16) | (data & 0xffff));
        return;
        if(aapl_get_lsb_rev(aapl,addr) >= 3)
        {
            if(!( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) ))
              avago_sbus_wr(aapl, addr, 0x02, (0<<31) | ((mem_addr & 0x1ff) << 16) | (data & 0xffff));
        }
            avago_sbus_wr(aapl, addr, 0x02, (1<<31) | ((mem_addr & 0x1ff) << 16) | (data & 0xffff));
        return;

    case AVAGO_ESB:
        if( aapl_get_spico_running_flag(aapl,addr) ||
            aapl_set_spico_running_flag(aapl,addr, avago_spico_running(aapl, addr)) )
        {
            avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x18, 0x4000 | (mem_addr & 0x3fff));
            avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x19, data);
            return;
        }

    case AVAGO_ESB_DIRECT:
    {
        int lsb_esb_addr, lsb_esb_write_data;
        int spico_run_state = avago_spico_halt(aapl, addr);

        int reset_status = avago_sbus_rd(aapl, addr, 0x07);
        if( reset_status & 0x01 )
            avago_sbus_wr(aapl, addr, 0x07, reset_status & ~1);

        lsb_esb_write_data = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB_DIRECT, 0x031);
        lsb_esb_addr = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB_DIRECT, 0x030);

        if( lsb_esb_addr & 0x8000 )
            avago_serdes_mem_wr(aapl, addr, AVAGO_LSB_DIRECT, 0x030, lsb_esb_addr & 0x7fff);

        avago_serdes_mem_wr(aapl, addr, AVAGO_LSB_DIRECT, 0x031, data);
        avago_serdes_mem_wr(aapl, addr, AVAGO_LSB_DIRECT, 0x030, mem_addr);
        avago_serdes_mem_wr(aapl, addr, AVAGO_LSB_DIRECT, 0x030, mem_addr | 0x8000);
        avago_serdes_mem_wr(aapl, addr, AVAGO_LSB_DIRECT, 0x030, mem_addr);

        avago_serdes_mem_wr(aapl, addr, AVAGO_LSB_DIRECT, 0x031, lsb_esb_write_data);
        if( lsb_esb_addr & 0x8000 )
            avago_serdes_mem_wr(aapl, addr, AVAGO_LSB_DIRECT, 0x030, lsb_esb_addr & 0x7fff);
        avago_serdes_mem_wr(    aapl, addr, AVAGO_LSB_DIRECT, 0x030, lsb_esb_addr);

        if( reset_status & 0x01 )
            avago_sbus_wr(aapl, addr, 0x07, reset_status);
        avago_spico_resume(aapl, addr, spico_run_state);
        return;
    }
#ifndef MV_HWS_REDUCED_BUILD
    case AVAGO_DMEM:
    {
        int spico_run_state = avago_spico_halt(aapl, addr);
        int initial_value_0x01 = avago_sbus_rd(aapl, addr,  0x01);
                                 avago_sbus_wr(aapl, addr,  0x01, 0x40000000 | (mem_addr & 0x3ff) | ((data & 0xffff) << 12));
                                 avago_sbus_wr(aapl, addr,  0x01, 0xc0000000 | (mem_addr & 0x3ff) | ((data & 0xffff) << 12));
                                 avago_sbus_wr(aapl, addr,  0x01, initial_value_0x01);
        avago_spico_resume(aapl, addr, spico_run_state);
        return;
    }

    case AVAGO_IMEM:
    {
        int spico_run_state = avago_spico_halt(aapl, addr);
        int initial_value_0x00 = avago_sbus_rd(aapl, addr,  0x00);
                                 avago_sbus_wr(aapl, addr,  0x00, 0x40000000 | (mem_addr & 0xffff) | ((data & 0x03ff) << 16));
                                 avago_sbus_wr(aapl, addr,  0x00, 0xc0000000 | (mem_addr & 0xffff) | ((data & 0x03ff) << 16));
                                 avago_sbus_wr(aapl, addr,  0x00, initial_value_0x00);
        avago_spico_resume(aapl, addr, spico_run_state);
        return;
    }
#endif /* MV_HWS_REDUCED_BUILD */
    default: ;
    }

    aapl_fail(aapl, __func__, __LINE__, "SBus %s, Invalid DMA type (%d).\n", aapl_addr_to_str(addr), type);
}

/** @brief   Performs a read-modify-write operation on SerDes memory location. */
/** */
/** @return  Returns the value of the memory location before it was modified. */
/** @return  On error, decrements aapl->return_code. */
/** @see     avago_serdes_mem_rd(), avago_serdes_mem_wr(). */
int avago_serdes_mem_rmw(
    Aapl_t *aapl,                   /**< [in] Pointer to Aapl_t structure. */
    uint addr,                      /**< [in] Device address number. */
    Avago_serdes_mem_type_t type,   /**< [in] The memory access type. */
    uint mem_addr,                  /**< [in] Memory address to access. */
    uint data,                      /**< [in] Data to write. */
    uint mask)                      /**< [in] Bits in existing data to modify. */
{
    int initial_value;
    int spico_run_state = avago_spico_halt(aapl, addr);

    if(      type == AVAGO_LSB ) type = AVAGO_LSB_DIRECT;
    else if( type == AVAGO_ESB ) type = AVAGO_ESB_DIRECT;

    initial_value = avago_serdes_mem_rd(aapl, addr, type, mem_addr);
    avago_serdes_mem_wr(aapl, addr, type, mem_addr, (data & mask) | (initial_value & ~mask));

    avago_spico_resume(aapl, addr, spico_run_state);
    return initial_value;
}



/* Helper function to get the Int 0x01 bits which are set in mask. */
/*    Needed to support Int 0x01 read-modify-write. */
static int serdes_get_int01_bits(Aapl_t *aapl, uint addr, uint mask)
{
    int bits = 0;
    if( mask & 0x03 )
        bits  |= 0x03 & avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x026);
    if( mask & 0x04 )
    {
#ifndef MV_HWS_REDUCED_BUILD
        if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
            bits |= (0x01 & avago_serdes_mem_rd(aapl, addr, AVAGO_ESB, 0xdf)) << 2;
        else
#endif /* MV_HWS_REDUCED_BUILD */
            bits |= (0x02 & avago_serdes_mem_rd(aapl, addr, AVAGO_ESB, 0x213)) << 1;
    }
    if( mask & 0x08 )
        bits |= 0;
    if( mask & 0x10 )
        bits |= 0;
    if( mask & 0x20 )
        bits |= 0;
    if( mask & 0x40 )
        bits |= 0;
    if( mask & 0x80 )
        bits |= 0;
    return bits;
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief   Gets the TX & RX ready states. */
/** @details Returns if the TX and RX devices are initialized and ready */
/**          for operation. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_tx_output_enable(), avago_serdes_set_tx_output_enable(), avago_serdes_set_tx_rx_enable(). */
int avago_serdes_get_tx_rx_ready(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr,          /**< [in] Device address number. */
    BOOL *tx,           /**< [out] Where to store TX state. */
    BOOL *rx)           /**< [out] Where to store RX state. */
{
    int return_code = aapl->return_code;
    int bits = serdes_get_int01_bits(aapl, addr, 0x03);
    *tx = (bits & 0x01) == 0x01;
    *rx = (bits & 0x02) == 0x02;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, TX %s, RX %s\n",
                aapl_addr_to_str(addr),aapl_enable_to_str(*tx),aapl_enable_to_str(*rx));
    return return_code == aapl->return_code ? 0 : -1;
}
#endif /* MV_HWS_REDUCED_BUILD */
/** @brief   Sets TX & RX enable states, and TX output values. */
/** @details If enabling TX or RX, waits for the device to become ready. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @return  On timeout, returns -1 and does not decrement aapl->return_code. */
/** @see     avago_serdes_get_tx_output_enable(), avago_serdes_get_tx_rx_ready(), avago_serdes_set_tx_output_enable(). */
int avago_serdes_set_tx_rx_enable(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr,          /**< [in] Device address number. */
    BOOL tx,            /**< [in] New TX state. */
    BOOL rx,            /**< [in] New RX state. */
    BOOL tx_output)     /**< [in] New TX output state. */
{
    int return_code = aapl->return_code;
    int loops = 0;
    int mask;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, tx_en=%s, rx_en=%s,tx_output_en=%s\n",
                aapl_addr_to_str(addr),aapl_bool_to_str(tx),aapl_bool_to_str(rx),aapl_bool_to_str(tx_output));

    mask = serdes_get_int01_bits(aapl, addr, ~0x07);
    if( tx        ) mask |= 0x01;
    if( rx        ) mask |= 0x02;
    if( tx_output ) mask |= 0x04;

    if (!avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x01, mask)) return -1;

    mask &= 0x03;
    for( loops = 0; loops <= AAPL_SERDES_INIT_RDY_TIMEOUT; loops++ )
    {
        if( loops > 0 )
            ms_sleep(1);

        if( (serdes_get_int01_bits(aapl, addr, mask) & mask) == mask )
        {
#ifndef MV_HWS_REDUCED_BUILD
            if( loops > 0 )
                AAPL_LOG_PRINT6(aapl,AVAGO_DEBUG6,__func__,__LINE__,"SBus %s, Int 0x01 loops: %d\n",aapl_addr_to_str(addr), loops);
#endif /* MV_HWS_REDUCED_BUILD */
            return return_code == aapl->return_code ? 0 : -1;
        }
    }
    aapl_log_printf(aapl,AVAGO_WARNING,__func__,__LINE__,"SBus %s, TX/RX RDY check timed out after %d loops\n", aapl_addr_to_str(addr), loops);
    return -1;
}

/** @brief   Gets the TX output enable state. */
/** @details */
/** @return  Returns TRUE if TX output is enabled, FALSE otherwise. */
/** @see     avago_serdes_get_tx_rx_ready(), avago_serdes_set_tx_output_enable(), avago_serdes_set_tx_rx_enable(). */
BOOL avago_serdes_get_tx_output_enable(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr)                  /**< [in] Device address number. */
{
    BOOL ret = serdes_get_int01_bits(aapl, addr, 0x04) == 0x04;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, TX output %s\n",aapl_addr_to_str(addr),aapl_enable_to_str(ret));
    return ret;
}
/** @brief   Sets the TX output enable state. */
/** @details Turns on TX if not already on. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_tx_output_enable(), avago_serdes_get_tx_rx_ready(), avago_serdes_set_tx_rx_enable(). */
int avago_serdes_set_tx_output_enable(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr,                  /**< [in] Device address number. */
    BOOL enable)                /**< [in] TRUE to enable output signal. */
{
    int return_code = aapl->return_code;
    int mask;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, TX output_en=%s\n",aapl_addr_to_str(addr),aapl_bool_to_str(enable));

    mask = serdes_get_int01_bits(aapl, addr, ~4) | (enable ? 4 : 0);
    avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x01, mask);
    return return_code == aapl->return_code ? 0 : -1;
}


/* @brief   Helper function to retry spico int 0x02 if it times out. */
/* @return  On success, returns 0. */
/* @return  On error, decrements aapl->return_code and returns -1. */
static int spico_int_02_retry(Aapl_t *aapl, uint addr, int data, int retry)
{
    int i;
    for( i = 0; i < retry; i++ )
    {
        if( 0x02 == avago_spico_int(aapl, addr, 0x02, data) )
            return 0;
        avago_sbus_rd(aapl, addr, 0x04);
    }
    return aapl_fail(aapl,__func__,__LINE__,"SBus %s, PRBS reconfigure timed out.\n",aapl_addr_to_str(addr));
}

/** @brief   Retrieves the 80-bit TX user data register. */
/** @details Note: Only the first 20 bits of the pattern are returned */
/**          as that is all that is readily available from the SerDes. */
/**          These bits are then duplicated into the other bits. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_set_tx_user_data(). */
EXT int avago_serdes_get_tx_user_data(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr,                  /**< [in] Device address number. */
    long user_data[4])          /**< [out] 80-bit user data (4 x 20). */
{
    int return_code = aapl->return_code;
    int value = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x002C);
    value |= (avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x002D) & 0x0f) << 16;
    user_data[0] =
    user_data[1] =
    user_data[2] =
    user_data[3] = value & 0x000fffff;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, user_data=0x%05x\n",aapl_addr_to_str(addr),user_data[0]);
    return return_code == aapl->return_code ? 0 : -1;
}

/** @brief   Loads the 80-bit value into the TX user data register */
/**          and select it as the TX input. */
/** @details The bit transmit order is from the least significant bit [0] of */
/**          user_data[0] through bit [19] of user_data[3]. */
/**          Each word holds 20 significant bits. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_tx_user_data(). */
EXT int avago_serdes_set_tx_user_data(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr,                  /**< [in] Device address number. */
    long user_data[4])          /**< [in] 80-bit user data (4 x 20). */
{
    int i, rc;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, user_data=0x%05x,%05x,%05x,%05x\n",
        aapl_addr_to_str(addr),user_data[0],user_data[1],user_data[2],user_data[3]);
    rc = avago_spico_int(aapl, addr, 0x0018, 0x0000);
    if( rc != 0x18 )
        return aapl_fail(aapl,__func__,__LINE__,"SBus %s, spico_int(0x18,0) returned 0x%x\n",aapl_addr_to_str(addr),rc);
    rc = 1;
    for( i = 0; i < 4; i++ )
    {
        rc = rc && 0x19 == avago_spico_int(aapl, addr, 0x0019,  user_data[i] & 0x03ff)
                && 0x19 == avago_spico_int(aapl, addr, 0x0019, (user_data[i] >> 10) & 0x03ff);
    }
    if( !rc )
        return aapl_fail(aapl,__func__,__LINE__,"SBus %s, spico_int(0x19) returned 0x%x\n",aapl_addr_to_str(addr),rc);
    return avago_serdes_set_tx_data_sel(aapl, addr, AVAGO_SERDES_TX_DATA_SEL_USER);
}

/** @brief   Gets the selected TX data source. */
/** */
/** @return  Returns the selected TX data source. */
/** @return  On error, aapl->return_code is set negative. */
/** @see     avago_serdes_set_tx_data_sel(). */
Avago_serdes_tx_data_sel_t avago_serdes_get_tx_data_sel(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr)          /**< [in] Device address number. */
{
    Avago_serdes_tx_data_sel_t ret = AVAGO_SERDES_TX_DATA_SEL_CORE;
    int data = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x021);
    if( data & (1 << 5) )
        ret = AVAGO_SERDES_TX_DATA_SEL_LOOPBACK;
    else if( data & (1 << 4) )
    {
        data = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x029);
        ret = (Avago_serdes_tx_data_sel_t)(data & 0x07);
    }
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, data_sel=%s\n",aapl_addr_to_str(addr),aapl_data_sel_to_str(ret));
    return ret;
}

/** @brief   Sets the TX data source. */
/** */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_tx_data_sel(). */
int avago_serdes_set_tx_data_sel(
    Aapl_t *aapl,                       /**< [in] Pointer to Aapl_t structure. */
    uint addr,                          /**< [in] Device address number. */
    Avago_serdes_tx_data_sel_t source)  /**< [in] New TX data selection. */
{
    int return_code = aapl->return_code;
    int int_data;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, data_sel=%s\n",aapl_addr_to_str(addr),aapl_data_sel_to_str(source));

    int_data = source==AVAGO_SERDES_TX_DATA_SEL_LOOPBACK ? 0x0210 : 0x0200;
    if (!avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x08, int_data)) return -1;

    switch( source )
    {
    case AVAGO_SERDES_TX_DATA_SEL_CORE:
        spico_int_02_retry(aapl, addr, 0x01ff, 8);
        break;

    case AVAGO_SERDES_TX_DATA_SEL_PRBS7:
    case AVAGO_SERDES_TX_DATA_SEL_PRBS9:
    case AVAGO_SERDES_TX_DATA_SEL_PRBS11:
    case AVAGO_SERDES_TX_DATA_SEL_PRBS15:
    case AVAGO_SERDES_TX_DATA_SEL_PRBS23:
    case AVAGO_SERDES_TX_DATA_SEL_PRBS31:
    case AVAGO_SERDES_TX_DATA_SEL_USER:
        int_data = 0x0100 | 0x0020 | (source & 0x0007);
        spico_int_02_retry(aapl, addr, int_data, 8);
        break;

    case AVAGO_SERDES_TX_DATA_SEL_LOOPBACK:
        break;
    }
    return return_code == aapl->return_code ? 0 : -1;
}

/** @brief   Gets the TX inverter polarity. */
/** @return  Returns TRUE if inverter is enabled, FALSE if data is not being inverted. */
/** @return  On error, decrements aapl->return_code. */
/** @see     avago_serdes_get_rx_invert(). */
/** @see     avago_serdes_set_tx_invert(). */
BOOL avago_serdes_get_tx_invert(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr)                  /**< [in] Device address number. */
{
    int ret;
#ifndef MV_HWS_REDUCED_BUILD
    if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
        ret = !!(avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0xdf) & 0x20);
    else
#endif /* MV_HWS_REDUCED_BUILD */
        ret = !!(avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0x211) & 8);
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, tx_invert=%s\n",aapl_addr_to_str(addr),aapl_onoff_to_str(ret));
    return ret;
}

/** @brief   Sets the TX inverter polarity. */
/** @details Set invert to TRUE to enable the inverter, FALSE to not invert the data. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_tx_invert(). */
/** @see     avago_serdes_set_rx_invert(). */
int avago_serdes_set_tx_invert(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr,                  /**< [in] Device address number. */
    BOOL invert)                /**< [in] TRUE to invert output data. */
{
    uint int_data;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, tx_invert=%s\n",aapl_addr_to_str(addr),aapl_onoff_to_str(invert));

    int_data = invert ? 0x0101 : 0x0100;
    if (!avago_spico_int_check(aapl, __func__, __LINE__, addr,0x13,int_data)) return -1;
    return 0;
}


/** @cond INTERNAL */
static int width_tab[] = {10,16,20,32,40,64,80,80};
int avago_serdes_bits_to_width(uint bits)
{
    return width_tab[ bits & 7 ];
}
static int width_val[] = {0,4,2,5,3,6,7,7};
int avago_serdes_width_to_bits(int width)
{
    int i;
    for( i = 0; i < AAPL_ARRAY_LENGTH(width_tab); i++ )
        if( width_tab[i] == width )
            return width_val[i];
    return -1;
}
/** @endcond */
#ifndef MV_HWS_REDUCED_BUILD
/** @brief   Sets the TX and RX data width. */
/** @details Values of 10, 20 and 40 bits are supported. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_tx_rx_width(). */
int avago_serdes_set_tx_rx_width(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr,          /**< [in] Device address number. */
    int tx,             /**< [in] New TX width. */
    int rx)             /**< [in] New RX width. */
{
    return avago_serdes_set_tx_rx_width_pam(aapl, addr, tx, rx, AVAGO_SERDES_NRZ, AVAGO_SERDES_PAM2);
}
#endif /* MV_HWS_REDUCED_BUILD */

/** @brief   Sets the TX and RX data width. */
/** @details Values of 10, 20 and 40 bits are supported. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_tx_rx_width(). */
int avago_serdes_set_tx_rx_width_pam(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr,          /**< [in] Device address number. */
    int tx,             /**< [in] New TX width. */
    int rx,             /**< [in] New RX width. */
    Avago_serdes_encoding_mode_t tx_encoding, /**< [in] Transmit serial line encoding */
    Avago_serdes_encoding_mode_t rx_encoding  /**< [in] Receive serial line encoding */
    )
{
    int return_code = aapl->return_code;
    int build_id = avago_firmware_get_build_id(aapl, addr);
    int bits = avago_serdes_width_to_bits(tx) | (avago_serdes_width_to_bits(rx) << 4);
    if( bits < 0 )
        return aapl_fail(aapl, __func__, __LINE__, "ERROR: SBus %s, invalid width(s) %d/%d requested.\n",aapl_addr_to_str(addr),tx,rx);
    if( (build_id & 2) == 0 )
        bits |= 0x8800;
#ifndef MV_HWS_REDUCED_BUILD
    if( aapl_get_ip_type(aapl, addr) == AVAGO_M4 ) {
      if (tx_encoding == AVAGO_SERDES_PAM4) bits |= 0x08;
      if (rx_encoding == AVAGO_SERDES_PAM4) bits |= 0x80;
    }
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, tx/rx width mode %d/%d bits, mask=0x%x\n",aapl_addr_to_str(addr),tx,rx,bits);
#endif /* MV_HWS_REDUCED_BUILD */
    avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x14, bits);
    return return_code == aapl->return_code ? 0 : -1;
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief  Initialize the structure to default values. */
/** @return void */
void avago_serdes_tx_eq_init(
    Avago_serdes_tx_eq_t *ptr)  /**< Structure pointer to initialize */
{
    memset(ptr,0,sizeof(Avago_serdes_tx_eq_t));
}
#endif /* MV_HWS_REDUCED_BUILD */
#define AAPL_HAVE_IP_INFO 0
/** @brief   Sets the TX equalization values. */
/** @details Set all values, if applicable. */
/**          Note: The slew rate is not adjustable on high speed SerDes and */
/**                must be set to 0 in this case. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_tx_eq(), avago_serdes_get_tx_eq_limits(). */
int avago_serdes_set_tx_eq(
    Aapl_t *aapl,                   /**< [in] Pointer to Aapl_t structure. */
    uint addr,                      /**< [in] Device address number. */
    Avago_serdes_tx_eq_t *tx_eq)    /**< [in] New set of values. */
{
    int return_code = aapl->return_code;
    int rc;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, tx_eq=%d,%d,%d\n",
                    aapl_addr_to_str(addr),tx_eq->pre,tx_eq->atten,tx_eq->post);
    if( tx_eq->atten == tx_eq->pre && tx_eq->atten == tx_eq->post )
        rc = avago_spico_int(aapl, addr, 0x15, (3 << 14) | (tx_eq->atten & 0xFF));
    else
    {
        rc =   0x15 == avago_spico_int(aapl, addr, 0x15, (3<<14) | 0)
            && 0x15 == avago_spico_int(aapl, addr, 0x15, (2<<14) | (tx_eq->post  & 0xFF))
            && 0x15 == avago_spico_int(aapl, addr, 0x15, (1<<14) | (tx_eq->atten & 0xFF))
            && 0x15 == avago_spico_int(aapl, addr, 0x15, (0<<14) | (tx_eq->pre   & 0xFF));
    }
    if( !rc )
        aapl_fail(aapl, __func__,__LINE__,
                    "SBus %s, Invalid TX equalization set: %d, %d, %d.\n",
                    aapl_addr_to_str(addr), tx_eq->pre, tx_eq->atten, tx_eq->post);
#if 0
    else if( avago_get_ip_info(aapl,addr)->center_freq > 10000 )
    {
        if( tx_eq->slew != 0 )
            aapl_fail(aapl,__func__,__LINE__,
                "SBus %s, Invalid slew value: %d, high speed SerDes requires value of 0.\n", aapl_addr_to_str(addr), tx_eq->slew);
    }
    else
#endif
    if( tx_eq->slew > 3 || tx_eq->slew < 0 )
        aapl_fail(aapl,__func__,__LINE__,
            "SBus %s, Invalid slew value: %d, valid range [0..3].\n", aapl_addr_to_str(addr), tx_eq->slew);
#ifndef MV_HWS_REDUCED_BUILD
    else if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
        avago_serdes_mem_rmw(aapl, addr, AVAGO_ESB, 0xe5, tx_eq->slew, 3);
#endif /* MV_HWS_REDUCED_BUILD */
    else
        avago_serdes_mem_rmw(aapl, addr, AVAGO_ESB, 0x240, tx_eq->slew, 3);

    return return_code == aapl->return_code ? 0 : -1;
}

/** @brief   Gets the TX equalization values. */
/** @details Retrieves all values (pre, atten, post, slew). */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_set_tx_eq(), avago_serdes_get_tx_eq_limits(). */
EXT int avago_serdes_get_tx_eq(
    Aapl_t *aapl,                   /**< [in] Pointer to Aapl_t structure. */
    uint addr,                      /**< [in] Device address number. */
    Avago_serdes_tx_eq_t *tx_eq)    /**< [in] New TX equalization values. */
{
    int return_code = aapl->return_code;
    if( aapl_check_firmware_rev(aapl,addr, __func__, __LINE__, FALSE, 1, 0x1043) )
    {
        tx_eq->pre   = (short) avago_spico_int(aapl, addr, 0x15, (1 << 8) | (0 << 14));
        tx_eq->atten = (short) avago_spico_int(aapl, addr, 0x15, (1 << 8) | (1 << 14));
        tx_eq->post  = (short) avago_spico_int(aapl, addr, 0x15, (1 << 8) | (2 << 14));
    }
    else    /* Pre-0x1043 firmware: */
    {
        uint i = avago_spico_int(aapl, addr, 0x15, 1 << 8);
        tx_eq->pre   = (i >> 12) & 0x0F;
        tx_eq->atten = (i >> 5 ) & 0x1F;
        tx_eq->post  = (i >> 0 ) & 0x1F;
    }
    tx_eq->slew = (short) avago_serdes_mem_rd(aapl, addr, AVAGO_ESB, 0x240) & 3;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, tx_eq=%d,%d,%d,%d\n",
                    aapl_addr_to_str(addr),tx_eq->pre,tx_eq->atten,tx_eq->post,tx_eq->slew);
    return return_code == aapl->return_code ? 0 : -1;
}

/** @brief   Gets the RX compare data configuration. */
/** */
/** @return  Avago_serdes_rx_cmp_data_t value. */
/**          On error, aapl->return_code is set negative. */
/** @see     avago_serdes_set_rx_cmp_data(). */
Avago_serdes_rx_cmp_data_t avago_serdes_get_rx_cmp_data(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    uint addr)              /**< [in] Device address number. */
{
    uint data = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x02a);
    Avago_serdes_rx_cmp_data_t return_data = data & 0x8000 ? (Avago_serdes_rx_cmp_data_t)(data & 0x07) : AVAGO_SERDES_RX_CMP_DATA_OFF;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, cmp_data=%s\n", aapl_addr_to_str(addr), aapl_cmp_data_to_str(return_data));
    return return_data;
}

/** @brief   Sets the RX compare data configuration. */
/** @details Sets the PRBS pattern to compare incoming data against. */
/**          If AVAGO_SERDES_RX_CMP_DATA_SELF_SEED is selected, the comparator */
/**          automatically synchronizes to an incoming 80 bit pattern. */
/**          The error counter then reports deviations from that pattern. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_rx_cmp_data(). */
int avago_serdes_set_rx_cmp_data(
    Aapl_t *aapl,                       /**< [in] Pointer to Aapl_t structure. */
    uint addr,                          /**< [in] Device address number. */
    Avago_serdes_rx_cmp_data_t data)    /**< [in] New RX compare data setting. */
{
    int int_data = 0x0200 | 0x0020 | 0x0010 | (data & 0x0007);
    if( data == AVAGO_SERDES_RX_CMP_DATA_OFF ) int_data = 0x2ff;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, cmp_data=%s\n", aapl_addr_to_str(addr), aapl_cmp_data_to_str(data));

    return spico_int_02_retry(aapl, addr, int_data, 8);
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief   Gets the RX termination. */
/** */
/** @return  One of AVAGO_SERDES_RX_TERM_AGND, AVAGO_SERDES_RX_TERM_AVDD, or */
/**          AVAGO_SERDES_RX_TERM_FLOAT. */
/** @return  On error, decrements aapl->return_code. */
/** @see     avago_serdes_set_rx_term(). */
Avago_serdes_rx_term_t avago_serdes_get_rx_term(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr)                  /**< [in] Device address number. */
{
    Avago_process_id_t process = aapl_get_process_id(aapl,addr);
    Avago_serdes_rx_term_t term = AVAGO_SERDES_RX_TERM_AGND;
    if( (process == AVAGO_PROCESS_B || aapl_get_ip_type(aapl,addr) == AVAGO_M4)
        && (avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0xc7) & 0x0040) )
        term = AVAGO_SERDES_RX_TERM_FLOAT;
    else if( process == AVAGO_PROCESS_F && aapl_get_ip_type(aapl,addr) != AVAGO_M4
        && (avago_serdes_mem_rd(aapl,addr,AVAGO_LSB,0x024) & 0x0400) )
        term = AVAGO_SERDES_RX_TERM_FLOAT;
    else
    {
        int val;
        if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
        {
            val = avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0xc7);
            if( val & 0x0020 )
                term = AVAGO_SERDES_RX_TERM_AVDD;
        } else {
            val = avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0x020);
            if( val & 0x0400 )
                term = AVAGO_SERDES_RX_TERM_AVDD;
        }
    }

    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, term=%s\n", aapl_addr_to_str(addr), aapl_term_to_str(term));
    return term;
}

/** @brief   Sets the RX termination. */
/** @details Term is AVAGO_SERDES_RX_TERM_AGND, AVAGO_SERDES_RX_TERM_AVDD, or */
/**          AVAGO_SERDES_RX_TERM_FLOAT. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_rx_term(). */
int avago_serdes_set_rx_term(
    Aapl_t *aapl,                   /**< [in] Pointer to Aapl_t structure. */
    uint addr,                      /**< [in] Device address number. */
    Avago_serdes_rx_term_t term)    /**< [in] New termination value. */
{
    int return_code = aapl->return_code;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, term=%s\n", aapl_addr_to_str(addr), aapl_term_to_str(term));
    if( aapl_get_lsb_rev(aapl,addr) >= 3 )
    {
        int int_data = 0;
        switch( term )
        {
        case AVAGO_SERDES_RX_TERM_AGND : int_data = 0; break;
        case AVAGO_SERDES_RX_TERM_AVDD : int_data = 1; break;
        case AVAGO_SERDES_RX_TERM_FLOAT: int_data = 2; break;
        }
        if(!avago_spico_int_check(aapl, __func__, __LINE__, addr,0x2B,int_data) ) return -1;
    }
    else if( term == AVAGO_SERDES_RX_TERM_FLOAT )
    {
        avago_serdes_mem_rmw(aapl,addr,AVAGO_LSB,0x024,0x0400,0x0400);
    }
    else
    {
        int value = (term == AVAGO_SERDES_RX_TERM_AVDD) ? 0x0400 : 0x0000;
        avago_serdes_mem_rmw(aapl,addr,AVAGO_ESB,0x020,value,0x0400);
        avago_serdes_mem_rmw(aapl,addr,AVAGO_LSB,0x024,0x0000,0x0400);
    }
    return return_code == aapl->return_code ? 0 : -1;
}
#endif /* MV_HWS_REDUCED_BUILD */

/** @brief   Injects errors into the RX data. */
/** @details RX error injection affects only the main data channel. */
/**          Therefore, errors are only detected if the MAIN channel */
/**          is one of the selected compare inputs. */
/** @details RX injected errors are detected in XOR and MAIN_PATGEN compare */
/**          modes, but are not detected in TEST_PATGEN mode. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_tx_inject_error(). */
int avago_serdes_rx_inject_error(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr,                  /**< [in] Device address number. */
    uint num_bits)              /**< [in] Number of error bits to inject. */
{
    int bits, return_code = aapl->return_code;
    if( aapl_get_lsb_rev(aapl,addr) < 3 )
    {
        return aapl_fail(aapl, __func__, __LINE__,
            "ERROR: LSB revisions 01 & 02 don't support RX ERROR injection.\n", 0);
    }

    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, injecting %u errors.\n", aapl_addr_to_str(addr), num_bits);

    bits = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x02b);
    for(         ; num_bits > 0; num_bits -- )
    {
        avago_serdes_mem_wr(aapl, addr, AVAGO_LSB, 0x02b, bits & ~2);
        avago_serdes_mem_wr(aapl, addr, AVAGO_LSB, 0x02b, bits |= 2);
    }
    return return_code == aapl->return_code ? 0 : -1;
}

/** @brief   Injects errors into the TX data. */
/** @details TX error injection affects the transmitted data stream. Therefore, */
/**          errors are only detected when comparing with a known pattern. */
/**          TX injected errors will be detected in RX TEST_PATGEN and MAIN_PATGEN modes. */
/**          TX injected errors will not be detected in XOR mode. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_rx_inject_error(). */
int avago_serdes_tx_inject_error(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr,                  /**< [in] Device address number. */
    uint num_bits)              /**< [in] Number of error bits to inject (max=65535). */
{
    uint int_data = MIN(num_bits,0x0ffff);
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, injecting %u errors.\n",aapl_addr_to_str(addr),int_data);

    if (!avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x1B, int_data)) return -1;
    return 0;
}

/** @brief  Sets the RX compare mode. */
/** */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_rx_cmp_mode(). */
int avago_serdes_set_rx_cmp_mode(
    Aapl_t *aapl,                       /**< [in] Pointer to Aapl_t structure. */
    uint addr,                          /**< [in] Device address number. */
    Avago_serdes_rx_cmp_mode_t mode)    /**< [in] New compare mode. */
{
    int return_code = aapl->return_code;
    int curr_mode;
    int new_mode = 0x0003;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, cmp_mode=%s\n", aapl_addr_to_str(addr), aapl_cmp_mode_to_str(mode));
    curr_mode = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x017);
    new_mode |= (curr_mode & ~0x0770) | (0x0770 & mode);
    avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x03, new_mode);
    return return_code == aapl->return_code ? 0 : -1;
}

/** @brief   Gets the RX inverter polarity. */
/** @return  Returns TRUE if inverter is enabled, FALSE if data is not being inverted. */
/** @return  On error, decrements aapl->return_code. */
/** @see     avago_serdes_get_tx_invert(). */
/** @see     avago_serdes_set_rx_invert(). */
BOOL avago_serdes_get_rx_invert(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr)                  /**< [in] Device address number. */
{
    int reg;
#ifndef MV_HWS_REDUCED_BUILD
    if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
        reg = !!(avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0x024) & 0x01);
    else
#endif /* MV_HWS_REDUCED_BUILD */
        reg = !!(avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0x060) & 8);
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, rx_invert=%s\n", aapl_addr_to_str(addr), aapl_onoff_to_str(reg));
    return reg;
}

/** @brief   Sets the RX inverter polarity. */
/** @details Set <b>invert</b> TRUE to enable the inverter, FALSE to not invert the data. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_rx_invert(). */
/** @see     avago_serdes_set_tx_invert(). */
int avago_serdes_set_rx_invert(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr,                  /**< [in] Device address number. */
    BOOL invert)                /**< [in] TRUE to invert input data. */
{
    int int_data = invert ? 0x0210 : 0x0200;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, rx_invert=%s\n", aapl_addr_to_str(addr), aapl_onoff_to_str(invert));
    if (!avago_spico_int_check(aapl, __func__, __LINE__, addr,0x13,int_data)) return -1;
    return 0;
}

/** @brief   Gets the RX signal input source. */
/** @return  Returns TRUE if internal loopback is enabled, FALSE if external signal. */
/** @return  On error, aapl->return_code becomes negative. */
/** @see     avago_serdes_set_rx_input_loopback(). */
BOOL avago_serdes_get_rx_input_loopback(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr)                  /**< [in] Device address number. */
{
    int reg = avago_serdes_mem_rd(aapl,addr,AVAGO_LSB,0x024);
    BOOL ret = (reg & 0x0002) ? TRUE : FALSE;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, rx_in=%s\n",
        aapl_addr_to_str(addr), ret ? "internal" : "external");
    return ret;
}

/** @brief   Sets the RX signal input source. */
/** @details Set <b>internal_loopback</b> TRUE to select loopback from own TX output. */
/** @details Set to FALSE to select external signal. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_rx_input_loopback(). */
int avago_serdes_set_rx_input_loopback(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    uint addr,                  /**< [in] Device address number. */
    BOOL internal_loopback)     /**< [in] New loopback state. */
{
    int int_data = internal_loopback ? 0x0101 : 0x0100;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, loopback=%s\n",
        aapl_addr_to_str(addr), internal_loopback ? "internal" : "external");
    if (!avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x08, int_data)) return -1;
    return 0;
}

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
/* Helper function to support Int 0x30 read-modify-write. */
/* Gets the bits which are set in mask. */
static int serdes_get_int30_bits(Aapl_t *aapl, uint addr, uint mask)
{
    int bits = 0;
    if( mask & 0x0007 )
        bits |= (avago_serdes_mem_rd(aapl,addr,AVAGO_LSB,0x00c) >>  8) & 0x7;
    if( mask & 0x00f0 )
    {
        if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
        {
            bits |= (avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0xd1) >> 6) & 0x30;
            bits |= (avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0xf0)   << 3) & 0xc0;
        }
        else
        {
            bits |= (avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0x200) >> 6) & 0x30;
            bits |= (avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0x300)   >> 0) & 0x0c0;
        }
        if(      (bits & 0x10) == 0x00 ) bits &= 0xff0f;
        else if( (bits & 0x30) == 0x10 ) bits &= 0xff1f;
        else if( (bits & 0x70) == 0x30 ) bits &= 0xff3f;
    }
    if( mask & 0x0f00 )
    {
        if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
        {
            uint pcs_read = avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0xc1) & 0x0708;
            if( (pcs_read & 0x8) == 0 )
                bits |= ((pcs_read) & 0x0700) | 0x0800;
        }
        else
        {
            uint pcs_read = avago_serdes_mem_rd(aapl,addr,AVAGO_ESB,0x073) & 0x1f;
            if( (pcs_read & 0x1) == 0 )
                bits |= ((pcs_read << 6) & 0x0700) | 0x0800;
        }
    }
    return bits;
}


/** @brief   Gets the TX PLL clock source. */
/** @details */
/** @return  Returns the TX PLL clock source. */
/** @return  On error, decrements aapl->return_code. */
/** @see     avago_serdes_set_tx_pll_clk_src(), avago_serdes_get_spico_clk_src(), avago_serdes_get_pcs_fifo_clk_div(). */
Avago_serdes_tx_pll_clk_t avago_serdes_get_tx_pll_clk_src(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr)          /**< [in] Device address number. */
{
    Avago_serdes_tx_pll_clk_t ret = (Avago_serdes_tx_pll_clk_t)serdes_get_int30_bits(aapl, addr, 0x00f0);
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, tx_pll_clk %s\n",aapl_addr_to_str(addr),aapl_pll_clk_to_str(ret));
    return ret;
}

/** @brief   Sets the SerDes's PLL Reference clock source. */
/** @details */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_tx_pll_clk_src(), avago_serdes_set_spico_clk_src(), avago_serdes_set_pcs_fifo_clk_div(). */
int avago_serdes_set_tx_pll_clk_src(
    Aapl_t *aapl,                   /**< [in] Pointer to Aapl_t structure. */
    uint addr,                      /**< [in] Device address number. */
    Avago_serdes_tx_pll_clk_t src)  /**< [in] New clock source. */
{
    int return_code = aapl->return_code;
    int mask;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, tx_pll_clk=%s\n",aapl_addr_to_str(addr),aapl_pll_clk_to_str(src));

    mask = serdes_get_int30_bits(aapl, addr, ~0x00f0) | src;
    avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x30, mask);
    return return_code == aapl->return_code ? 0 : -1;
}
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */


/** @brief   Reads whether or not signal OK has been initialized. */
/** @return  Returns TRUE if signal OK detection is enabled, FALSE if not. */
/** @return  On error, decrements aapl->return_code and returns FALSE. */
/** @see     avago_serdes_initialize_signal_ok(). */
BOOL avago_serdes_get_signal_ok_enable(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr)          /**< [in] Device address number. */
{
    BOOL ret = FALSE;
    if( aapl_check_firmware_rev(aapl,addr, __func__, __LINE__, FALSE, 1, 0x1046) )
    {
#ifndef MV_HWS_REDUCED_BUILD
        if( (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) || (aapl_get_ip_type(aapl,addr) == AVAGO_M4) )
        {
            int val = avago_serdes_mem_rd(aapl, addr, AVAGO_ESB, 0xc4);
            ret = ((val & 0x30)>>8) == 0x02;
        } else
#endif /* MV_HWS_REDUCED_BUILD */
        {
            int val = avago_serdes_mem_rd(aapl, addr, AVAGO_ESB, 0x080);
            ret = (val & 0x03) == 0x02;
        }
    }
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, signal_ok_enable=%s\n", aapl_addr_to_str(addr), aapl_bool_to_str(ret));
    return ret;
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief   Checks for presence of electrical idle. */
/** @return  Returns TRUE if electrical idle is detected, FALSE if a signal is */
/**          detected. */
BOOL avago_serdes_get_electrical_idle(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr)          /**< [in] Device address number. */
{
    int val = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x025);
    BOOL ret = (val & 0x04) == 0x04;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, elec_idle=%s\n", aapl_addr_to_str(addr), aapl_bool_to_str(ret));
    return ret;
}
#endif /* MV_HWS_REDUCED_BUILD */

/** @brief   Reads the signal OK threshold value. */
/** @details Reads a sticky signal lost detector and returns whether any */
/**          loss was detected since the last call to this function. */
/**          Resets the detector if signal loss was detected so can tell */
/**          if a signal is once again present. */
/** @return  On success, returns threshold value. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_initialize_signal_ok(). */
int avago_serdes_get_signal_ok_threshold(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr)          /**< [in] Device address number. */
{
    int return_code = aapl->return_code;
    int threshold = 0;
    if( aapl_check_firmware_rev(aapl,addr, __func__, __LINE__, FALSE, 1, 0x1046) )
    {
#ifndef MV_HWS_REDUCED_BUILD
        if (aapl_get_process_id(aapl,addr) == AVAGO_PROCESS_B) {
          threshold = avago_serdes_mem_rd(aapl, addr, AVAGO_ESB, 0xc4);
          threshold = (threshold >> 0) & 0x1f;
        }
         else
#endif /* MV_HWS_REDUCED_BUILD */
        {
          threshold = avago_serdes_mem_rd(aapl, addr, AVAGO_ESB, 0x080);
          threshold = (threshold >> 2) & 0x0f;
        }
    }
    if( return_code != aapl->return_code )
        threshold = -1;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, signal_ok_threshold=%d\n", aapl_addr_to_str(addr), threshold);
    return threshold;
}

/** @brief   Reads and resets the signal_ok_deasserted signal. */
/** @details Reads a sticky signal lost detector and returns whether any */
/**          loss was detected since the last call to this function. */
/**          Resets the detector if signal loss was detected so can tell */
/**          if a signal is once again present. */
/** @return  Returns TRUE if no signal loss has been detected. */
/** @return  Returns FALSE if signal loss was detected since last check. */
/** @see     avago_serdes_initialize_signal_ok(). */
BOOL avago_serdes_get_signal_ok(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr,          /**< [in] Device address number. */
    BOOL reset)         /**< [in] Clear signal loss sticky bit if set. */
{
    int bits = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x26);
    BOOL sig_ok = (bits & 0x0010) ? 0 : 1;
    if( reset && !sig_ok )
        avago_serdes_mem_rmw(aapl, addr, AVAGO_LSB, 0x26, 0, 0x0010);
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, sig_ok=%s, reset=%d\n", aapl_addr_to_str(addr), aapl_bool_to_str(sig_ok), reset);
    return sig_ok;
}

/** @brief   Enables the RX Idle Detector to aide detecting signal loss. */
/** @details Call this function once to initialize the idle detector. */
/**          Then call avago_serdes_get_signal_ok() to check if signal loss is detected. */
/**          Functional for firmware 0x1046 and newer.  No-op on older versions. */
/** @return  On success, returns 0. */
/** @return  On error, decrements aapl->return_code and returns -1. */
/** @see     avago_serdes_get_signal_ok(), avago_serdes_get_signal_ok_threshold(). */
int avago_serdes_initialize_signal_ok(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr,          /**< [in] Device address number. */
    int threshold)      /**< [in] Threshold for signal OK (0-15). */
{
    int return_code = aapl->return_code;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, threshold %d\n", aapl_addr_to_str(addr), threshold);
    if( aapl_check_firmware_rev(aapl,addr, __func__, __LINE__, FALSE, 1, 0x1046) )
    {
        avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x20, 0x20);
        avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x20, 0x40 | ((threshold & 0xf)<<8));
    }
    avago_serdes_get_signal_ok(aapl, addr, TRUE);
    return return_code == aapl->return_code ? 0 : -1;
}

/** @brief   Returns whether or not the RX PLL is frequency locked. */
/** @details NOTE: The PLL may be locked even when valid data is not */
/**          present at the RX inputs. The user should generally rely */
/**          on signal ok instead. */
/** @return  Returns TRUE if the PLL is locked */
/** @return  Returns FALSE if the PLL is not locked */
BOOL avago_serdes_get_frequency_lock(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint addr)          /**< [in] Device address number. */
{
    BOOL flock = (avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, 0x1c) & 0x8000) >> 15;
    AAPL_LOG_PRINT5(aapl,AVAGO_DEBUG5,__func__,__LINE__,"SBus %s, freq lock=%d\n", aapl_addr_to_str(addr), flock);
    return flock;
}

/** @brief   Retrieves the value of the error counter. */
/** @details Validates the SBus address refers to a 28nm SerDes.  A WARNING is issued */
/** if the addr is the general SerDes broadcast address (0xFF).  SerDes DMA access */
/** is made using the DMA type supplied: AVAGO_LSB_DIRECT uses SBus to read/write the data */
/** and doesn't interrupt the processor while AVAGO_LSB issues interrupts to the processor to */
/** read/write the necessary data.  Using AVAGO_LSB_DIRECT is recommended when constantly polling */
/** the error count to minimize the processor interrupts.  If continous error gathering is */
/** enabled, that feature is turned off while the error count is read, and then re-enabled. */
/** Once the error count is read, if reset is non-zero, the error counter is reset. */
/** */
/** @return  On success, returns the error count. */
/** @return  On error, decrements aapl->return_code and returns an all ones value. */
/** @see     avago_serdes_set_dac(), avago_serdes_step_phase(). */
uint avago_serdes_get_errors(
    Aapl_t *aapl,                 /**< [in] Pointer to Aapl_t structure. */
    uint addr,                    /**< [in] Device address number. */
    Avago_serdes_mem_type_t type, /**< [in] DMA type: AVAGO_LSB or AVAGO_LSB_DIRECT. */
    BOOL reset)                   /**< [in] Set true to reset error count after read. */
{
    int return_code = aapl->return_code;
    uint data;
    uint rc, err_count;

    if( !aapl_check_ip_type(aapl, addr, __func__, __LINE__, TRUE, 2, AVAGO_SERDES, AVAGO_M4) ||
        !aapl_check_process(aapl, addr, __func__, __LINE__, TRUE, 2, AVAGO_PROCESS_B, AVAGO_PROCESS_F) )
        return ~0;
    if( type != AVAGO_LSB && type != AVAGO_LSB_DIRECT )
    {
        aapl_fail(aapl, __func__, __LINE__, "SBus %s, Can only read error counter from the LSB.\n", aapl_addr_to_str(addr));
        type = AVAGO_LSB_DIRECT;
    }

    data = avago_serdes_mem_rd(aapl, addr, type, 0x00d);
    if( data & 0x0008 )
    {
        avago_serdes_mem_wr(aapl, addr, type, 0x00d, data & 0xFFF7);
    }
    err_count  =  avago_serdes_mem_rd(aapl, addr, type, 0x00e) & 0xFFFF;
    err_count += (avago_serdes_mem_rd(aapl, addr, type, 0x00f) & 0xFFFF) << 16;
    if( reset && err_count != 0 )
    {
        avago_serdes_mem_wr(aapl, addr, type, 0x00d, data | 0x0001);
    }
    if( (data & 0x0008) || (reset && err_count != 0) )
    {
        avago_serdes_mem_wr(aapl, addr, type, 0x00d, data);
    }
    rc = return_code == aapl->return_code ? err_count : ~0U;
    AAPL_LOG_PRINT5(aapl, AVAGO_DEBUG5, __func__, __LINE__, "SBus %s, dma_type=%d, reset=%d, errors = %u.\n", aapl_addr_to_str(addr), type, reset, rc);
    return rc;
}

/* MRVL taken LGPL aapl-2.2.1 */
/** @brief   Resets the error counter. */
/** @see     avago_serdes_get_errors(). */
/** @return  On success, returns TRUE. */
/** @return  On failure, returns FALSE and decrements aapl->return_code. */
BOOL avago_serdes_error_reset(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Device address number. */
{
    BOOL rc = avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x17, 0);
    AAPL_LOG_PRINT5(aapl, AVAGO_DEBUG5, __func__, __LINE__, "SBus %s; rc = %s.\n", aapl_addr_to_str(addr), aapl_bool_to_str(rc));
    return rc;
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief   Slips (drops) bits on the Rx. */
/** @details Slips up to 127 bits. */
/** @return  On success, returns TRUE. */
/** @return  On failure, returns FALSE and decrements aapl->return_code. */
BOOL avago_serdes_slip_bits(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Device address number. */
    uint bits)      /**< [in] Number of bits to slip.  127 max. */
{
    return avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x0c, bits & 0x7f);
}

/** @brief   Slips the Rx FIFO clock phase. */
/** @details Valid values are 1 to the word width. */
/** @return  On success, returns TRUE. */
/** @return  On failure, returns FALSE and decrements aapl->return_code. */
BOOL avago_serdes_slip_rx_phase(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Device address number. */
    uint bits,      /**< [in] Number of bits to slip. 63 max. */
    BOOL remember)  /**< [in] If TRUE, applies slips every time the Rx is enabled. */
{
    int flags = remember ? 0x8000 : 0;
    return avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x0e, ((bits & 0x3f) << 8) | flags);
}

/** @brief   Slips the Tx phase. */
/** @details Each slip slips the Tx load by 2 UI.  Up to 31 slips can be done. */
/** @return  On success, returns TRUE. */
/** @return  On failure, returns FALSE and decrements aapl->return_code. */
BOOL avago_serdes_slip_tx_phase(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Device address number. */
    uint slips,     /**< [in] Number of slips (2UI each) to perform. */
    BOOL remember)  /**< [in] If TRUE, applies slips every time the Tx is enabled. */
{
    int flags = remember ? 0x8000 : 0;
    return avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x0d, (slips & 0x1f) | flags);
}
#endif /* MV_HWS_REDUCED_BUILD */

#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
/** @brief   Allocates and initializes a Avago_serdes_init_config_t struct. */
/** @details The return value should be released using */
/**          avago_serdes_init_config_destruct() after use. */
/** @return  On success, returns a pointer to the initialized structure. */
/** @return  On failure, returns NULL. */
/** @see     avago_serdes_init_config_destruct(), avago_serdes_init(). */
Avago_serdes_init_config_t *avago_serdes_init_config_construct(
    Aapl_t *aapl)       /**< [in] Pointer to Aapl_t structure. */
{
    Avago_serdes_init_config_t *config;
#ifdef MV_HWS_BIN_HEADER
    config = &avagoSerdesInitStruct;
#else
    size_t bytes = sizeof(Avago_serdes_init_config_t);
    if( ! (config = (Avago_serdes_init_config_t *) aapl_malloc(aapl, bytes, "Avago_serdes_init_config_t struct")) )
        return NULL;
#endif
    memset(config, 0, sizeof(*config));

    config->sbus_reset   = TRUE;
    config->init_mode    = AVAGO_PRBS31_ILB;

    config->init_tx      = TRUE;
    config->tx_divider   = 10;
    config->tx_width     = 20;
    config->tx_phase_cal = FALSE;
    config->refclk_sync_master = TRUE;
    config->tx_output_en = TRUE;

    config->init_rx      = TRUE;
    config->rx_divider   = 10;
    config->rx_width     = 20;
    config->signal_ok_en = TRUE;
    config->signal_ok_threshold = 0x0;

    return config;
}

/** @brief   Releases a Avago_serdes_init_config_t struct. */
/** */
/** @return  None. */
/** @see     avago_serdes_init_config_construct(), avago_serdes_init(). */
void avago_serdes_init_config_destruct(
    Aapl_t *aapl,                       /**< [in] Pointer to Aapl_t structure. */
    Avago_serdes_init_config_t *config) /**< [in] Pointer to struct to release. */
{
#ifndef MV_HWS_BIN_HEADER
    aapl_free(aapl, config, "Avago_serdes_init_config_t struct");
#endif
}

/** @brief   Initializes a SerDes device to the given divider. */
/** */
/** @return  0 on success, */
/** @return  > 0 if channel errors after initialization, */
/** @return  aapl->return_code (< 0) if errors during initialization. */
/** @see     avago_serdes_init(). */
int avago_serdes_init_quick(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Device address number. */
    uint divider)   /**< [in] Desired divider to configure. */
{
    int errors;
    Avago_serdes_init_config_t *config = avago_serdes_init_config_construct(aapl);
    config->tx_divider = divider;
    config->rx_divider = divider;
    config->tx_width   = 40;
    config->rx_width   = 40;
    errors = avago_serdes_init(aapl, addr, config);
    avago_serdes_init_config_destruct(aapl, config);
    return errors;
}
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */


/** @brief   Allocates and initializes a Avago_serdes_init_config_t struct. */
/** @details The return value should be released using */
/**          avago_serdes_init_config_destruct() after use. */
/** @return  On success, returns a pointer to the initialized structure. */
/** @return  On failure, returns NULL. */
/** @see     avago_serdes_init_config_destruct(), avago_serdes_init(). */
void avago_serdes_init_config(Avago_serdes_init_config_t *config)
{
    memset(config, 0, sizeof(Avago_serdes_init_config_t));

    config->sbus_reset   = TRUE;
    config->init_mode    = AVAGO_PRBS31_ILB;

    config->init_tx      = TRUE;
    config->tx_divider   = 10;
    config->tx_width     = 20;
    config->tx_phase_cal = FALSE;
    config->refclk_sync_master = TRUE;
    config->tx_output_en = TRUE;

    config->init_rx      = TRUE;
    config->rx_divider   = 10;
    config->rx_width     = 20;
    config->signal_ok_en = TRUE;
    config->signal_ok_threshold = 0x0;
}

/** @brief   Configures and calibrates a SerDes. */
/** @details Call this to initialize a SerDes. */
/** @return  0 on success, aapl->return_code (< 0) on error. */
/** @see     avago_serdes_init_config_construct(), avago_serdes_init_config_destruct(), avago_serdes_init_quick(). */
int avago_serdes_init(
    Aapl_t *aapl,                       /**< [in] Pointer to Aapl_t structure. */
    uint addr,                          /**< [in] Device address number. */
    Avago_serdes_init_config_t *config) /**< [in] Desired SerDes configuration. */
{
    int errors;
    if (aapl_check_broadcast_address(aapl, addr, __func__, __LINE__, TRUE)) return -1;
    if( !aapl_check_ip_type(aapl, addr, __func__, __LINE__, TRUE, 2, AVAGO_SERDES, AVAGO_M4) ||
        !aapl_check_process(aapl, addr, __func__, __LINE__, TRUE, 2, AVAGO_PROCESS_B, AVAGO_PROCESS_F) )
        return -1;

    aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "SBus %s\n",aapl_addr_to_str(addr));
    if( config->sbus_reset )
        avago_sbus_reset(aapl, addr, 0);
    avago_spico_reset(aapl, addr);

    aapl_set_spico_running_flag(aapl, addr, 1);

    if( !avago_spico_crc(aapl, addr) )
        return aapl_fail(aapl, __func__, __LINE__, "SerDes %s failed CRC check.\n", aapl_addr_to_str(addr));

    avago_serdes_set_tx_rx_enable(aapl, addr, FALSE, FALSE, FALSE);
    avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x11, 0x03);

    avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x0b, config->tx_phase_cal ? 1 : 0);

    if( config->tx_divider == config->rx_divider )
    {
        int div_val = (config->tx_divider & 0xff) | (1 << 15);
        if (config->refclk_sync_master) div_val |= 1<<12;
        avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x05, div_val);
    }
    else
    {
        avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x05, (config->tx_divider & 0xff) | (config->refclk_sync_master ? 1<<12 : 0));
        avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x06, config->rx_divider & 0xff);
    }

    avago_serdes_set_tx_rx_width_pam(aapl, addr, config->tx_width, config->rx_width, config->tx_encoding, config->rx_encoding);

    avago_serdes_set_tx_rx_enable(aapl, addr, config->init_tx, config->init_rx, config->tx_output_en);

    if( config->signal_ok_en )
        avago_serdes_initialize_signal_ok(aapl,addr,config->signal_ok_threshold);

    if( config->init_mode == AVAGO_INIT_ONLY )
    {
        avago_serdes_set_rx_input_loopback(aapl, addr, FALSE);
        aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "\n", 0);
        return 0;
    }
    avago_serdes_set_rx_input_loopback(aapl, addr, TRUE);
    avago_serdes_set_tx_data_sel(aapl, addr, AVAGO_SERDES_TX_DATA_SEL_PRBS31);
    avago_serdes_set_rx_cmp_data(aapl, addr, AVAGO_SERDES_RX_CMP_DATA_PRBS31);
    avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x03, AVAGO_SERDES_RX_CMP_MODE_MAIN_PATGEN | 0x0003);

    if (config->init_tx && config->init_rx) errors = avago_serdes_get_errors(aapl, addr, AVAGO_LSB, 1);
    else errors = 0;

    if( config->init_mode == AVAGO_PRBS31_ELB    || config->init_mode == AVAGO_CORE_DATA_ELB )
    {
        if (config->tx_inversion) avago_serdes_set_tx_invert(aapl, addr, TRUE);
        if (config->rx_inversion) avago_serdes_set_rx_invert(aapl, addr, TRUE);
        avago_serdes_set_rx_input_loopback(aapl, addr, FALSE);
    }
    if( config->init_mode == AVAGO_CORE_DATA_ILB || config->init_mode == AVAGO_CORE_DATA_ELB )
    {
        avago_serdes_set_tx_data_sel(aapl, addr, AVAGO_SERDES_TX_DATA_SEL_CORE);
        avago_serdes_set_rx_cmp_data(aapl, addr, AVAGO_SERDES_RX_CMP_DATA_OFF);
    }

    aapl_log_printf(aapl,AVAGO_DEBUG1, "", 0, "Errors in ILB: %d.\n", errors);

    if( errors )
        aapl_log_printf(aapl,AVAGO_WARNING,__func__,__LINE__,
            "%d PRBS errors detected during ILB check of SerDes %s during serdes_init. Use this SerDes with caution.\n", errors, aapl_addr_to_str(addr));

    return errors;
}

/** @brief Initialize struct with default values */
void avago_serdes_tune_init(
    Aapl_t *aapl,                       /**< [in] Pointer to Aapl_t structure. */
    Avago_serdes_dfe_tune_t *control)   /**< [in] Tune options structure to initialize. */
{
    (void)aapl;
    memset(control,0,sizeof(*control));

    control->tune_mode=AVAGO_DFE_ICAL;

    control->value[0] = 56;
    control->value[1] = 12;
    control->value[2] =  0;
    control->value[3] = 15;
}

/** @brief  Performs SerDes tuning with a few options. */
/** @note   Requires SerDes firmware revision 0x1046 or higher. */
void avago_serdes_tune(
    Aapl_t *aapl,                       /**< [in] Pointer to Aapl_t structure. */
    uint addr,                          /**< [in] Device address number. */
    Avago_serdes_dfe_tune_t *control)   /**< [in] Tune options. */
{
    uint int_data = 0;
    aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "SBus %s, tune_mode %s\n", aapl_addr_to_str(addr), aapl_dfe_tune_mode_to_str(control->tune_mode));

    if (!aapl_check_ip_type(aapl, addr, __func__, __LINE__, TRUE, 2, AVAGO_SERDES, AVAGO_M4)) return;
    if (!aapl_check_process(aapl, addr, __func__, __LINE__, TRUE, 2, AVAGO_PROCESS_B, AVAGO_PROCESS_F)) return;
    if (!aapl_check_firmware_rev(aapl, addr, __func__, __LINE__, TRUE, 1, 0x1046)) return;

    if (control->dfe_disable){ int_data |= 0x0040; }

    if (control->fixed[0]) { int_data |= 0x0080; avago_spico_int(aapl, addr, 0x26, (2 << 12) | (2 << 8) | (control->value[0] & 0xff)); }
    if (control->fixed[1]) { int_data |= 0x0100; avago_spico_int(aapl, addr, 0x26, (2 << 12) | (1 << 8) | (control->value[1] & 0xff)); }
    if (control->fixed[2]) { int_data |= 0x0200; avago_spico_int(aapl, addr, 0x26, (2 << 12) | (0 << 8) | (control->value[2] & 0xff)); }

    if (aapl_check_firmware_rev(aapl, addr, __func__, __LINE__, FALSE, 1, 0x104D) && control->fixed[3]) {
        int_data |= 0x0400;
        avago_spico_int(aapl, addr, 0x26, (2 << 12) | (3 << 8) | (control->value[3] & 0xff)); }

    switch(control->tune_mode)
    {
    case AVAGO_DFE_ICAL           : { int_data |= 0x0001; break; }
    case AVAGO_DFE_PCAL           : { int_data |= 0x0002; break; }
    case AVAGO_DFE_ENABLE_RR      : { int_data |= 0x000A; break; }
    case AVAGO_DFE_START_ADAPTIVE : { int_data |= 0x0006; break; }
    case AVAGO_DFE_STOP_ADAPTIVE  : { int_data  = 0x0002; break; }
    case AVAGO_DFE_DISABLE_RR     : { int_data  = 0x0008; break; }
    }

    avago_spico_int_check(aapl, __func__, __LINE__, addr, 0x0A, int_data);
}

/** @brief   Returns a simple eye metric in range [0..1000]. */
/** @details Value is derived from SerDes tuning, and returns 0 if tuning */
/**          has not completed. */
uint avago_serdes_eye_get_simple_metric(
    Aapl_t *aapl,                       /**< [in] Pointer to Aapl_t structure. */
    uint addr)                          /**< [in] Device address number. */
{
    int i, results = 1000;
    for( i = 0; i < 8; i += 2 )
    {
        short val1 = avago_spico_int(aapl,addr, 0x126, (4 << 12) | ((i+0) << 8));
        short val2 = avago_spico_int(aapl,addr, 0x126, (4 << 12) | ((i+1) << 8));
        int height = val2 - val1;
        if( height < results )
            results = height;
    }
    results = results * 1000 / 255;
    return results;
}

/* */
/** @} */
