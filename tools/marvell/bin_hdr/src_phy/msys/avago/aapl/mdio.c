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

/* AAPL (ASIC and ASSP Programming Layer) MDIO-related functions. */

/** Doxygen File Header */
/** @file */
/** @brief MDIO related functions.  */

#include "aapl.h"

#define REG_SBUS_CMD         32768
#define REG_SBUS_SA          32769
#define REG_SBUS_DA          32770
#define REG_SBUS_DATA_IN_LO  32771
#define REG_SBUS_DATA_IN_HI  32772
#define REG_SBUS_EXEC        32773
#define REG_SBUS_RESULT      32774
#define REG_SBUS_DATA_OUT_LO 32775
#define REG_SBUS_DATA_OUT_HI 32776


/** @brief  Initializes mdio access. */
/** @return On success, returns 0. */
/** @return On error, decrements aapl->return_code and returns -1. */
int avago_mdio_open(Aapl_t *aapl)
{
    int rc;
#if AAPL_ALLOW_USER_SUPPLIED_MDIO
    if( aapl->communication_method == AVAGO_USER_SUPPLIED_MDIO )
    {
        rc = user_supplied_mdio_open_function(aapl);
    }
    else
#endif
#if AAPL_ALLOW_GPIO_MDIO
    if( aapl->communication_method == AVAGO_GPIO_MDIO )
    {
        rc = avago_gpio_mdio_open(aapl);
    }
    else
#endif
        rc = 0;
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "open status = %d\n", rc);
    return rc;
}

/** @brief  Terminates mdio access. */
/** @return On success, returns 0. */
/** @return On error, decrements aapl->return_code and returns -1. */
int avago_mdio_close(Aapl_t *aapl)
{
    int rc;
#if AAPL_ALLOW_USER_SUPPLIED_MDIO
    if( aapl->communication_method == AVAGO_USER_SUPPLIED_MDIO )
    {
        rc = user_supplied_mdio_close_function(aapl);
    }
    else
#endif
#if AAPL_ALLOW_GPIO_MDIO
    if( aapl->communication_method == AVAGO_GPIO_MDIO )
    {
        rc = avago_gpio_mdio_close(aapl);
    }
    else
#endif
        rc = 0;
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "return status = %d\n", rc);
    return rc;
}

/** @brief   Execute an MDIO command. */
/** */
/** @return  0 on success, aapl->return_code (< 0) on failure. */
uint avago_mdio(
    Aapl_t *aapl,               /**< [in] Pointer to AAPL structure */
    Avago_mdio_cmd_t mdio_cmd,  /**< [in] Command type */
    uint port_addr,             /**< [in] Physical port address (aka SBus address) */
    uint dev_addr,              /**< [in] Register address */
    uint data)                  /**< [in] Data to write, ignored for read. */
{
    char commandstr[64];

    if( mdio_cmd == AVAGO_MDIO_ADDR )
        aapl->last_mdio_addr[port_addr] = data;

    if( port_addr == AVAGO_BROADCAST )
    {
        uint rc = 0, chip;
        for( chip = 0; chip < aapl->chips; chip ++ )
            rc = avago_mdio(aapl, mdio_cmd, chip, dev_addr, data);
        return rc;
    }

    if      (mdio_cmd == AVAGO_MDIO_ADDR)  snprintf(commandstr, 64, "mdio a %u %u 0x%x", port_addr + aapl->mdio_base_port_addr, dev_addr, data);
    else if (mdio_cmd == AVAGO_MDIO_WRITE) snprintf(commandstr, 64, "mdio w %u %u 0x%x", port_addr + aapl->mdio_base_port_addr, dev_addr, data);
    else if (mdio_cmd == AVAGO_MDIO_READ)  snprintf(commandstr, 64, "mdio r %u %u",      port_addr + aapl->mdio_base_port_addr, dev_addr);
    else if (mdio_cmd == AVAGO_MDIO_WAIT)  snprintf(commandstr, 64, "mdio wait %u %u",   port_addr + aapl->mdio_base_port_addr, dev_addr);

    #if AAPL_ALLOW_AACS
    if( aapl_is_aacs_communication_method(aapl) )
    {
        uint recv_data_back = mdio_cmd == AVAGO_MDIO_READ || mdio_cmd == AVAGO_MDIO_WAIT;
        avago_aacs_send_command_options(aapl, commandstr, recv_data_back, /* strtol = */ 16);
        return aapl->data;
    }
    #endif

    #if AAPL_ALLOW_USER_SUPPLIED_MDIO
    if( aapl->communication_method == AVAGO_USER_SUPPLIED_MDIO )
    {


        int rc = user_supplied_mdio_function(aapl, mdio_cmd, port_addr + aapl->mdio_base_port_addr, dev_addr, aapl->last_mdio_addr[port_addr], data);

        aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "%s -> 0x%08x\n", commandstr, rc);
        return rc;
    }
    #endif

    #if AAPL_ALLOW_GPIO_MDIO
    if( aapl->communication_method == AVAGO_GPIO_MDIO )
    {


        int rc = aapl_gpio_mdio_function(aapl, mdio_cmd, port_addr + aapl->mdio_base_port_addr, dev_addr, aapl->last_mdio_addr[port_addr], data);

        aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "%s -> 0x%08x\n", commandstr, rc);
        return rc;
    }
    #endif

    aapl_fail(aapl,__func__,__LINE__,"Implementation missing.\n",0);
    return ~0;
}

/** @brief  Execute an MDIO write operation. */
/** */
/** @return Returns 0 on success, aapl->return_code (< 0) on error. */
uint avago_mdio_wr(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    uint port_addr,     /**< [in] Physical port address */
    uint dev_addr,      /**< [in] Register address */
    uint reg_addr,      /**< [in] Address to which to write. */
    uint data)          /**< [in] Data to write */
{
           avago_mdio(aapl, AVAGO_MDIO_ADDR, port_addr, dev_addr, reg_addr);
    return avago_mdio(aapl, AVAGO_MDIO_WRITE, port_addr, dev_addr, data);
}

/** @brief   Execute an MDIO read operation. */
/** @return  Return 32 bit data from reg_addr on port_addr. */
/**          Sets aapl->return_code and logs an error on failure. */
uint avago_mdio_rd(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    uint port_addr,     /**< [in] Physical port address */
    uint dev_addr,      /**< [in] Register address */
    uint reg_addr)      /**< [in] Address from which to read. */
{
           avago_mdio(aapl, AVAGO_MDIO_ADDR, port_addr, dev_addr, reg_addr);
    return avago_mdio(aapl, AVAGO_MDIO_READ, port_addr, dev_addr, 0);
}

/** @brief   Execute an sbus reset(00), write(01) or read(10) command */
/**          via the remote MDIO core interface. */
/** @return  Return 0 on write or reset, 32 bit data on read. */
/**          Sets aapl->return_code and logs an error on failure. */
uint avago_mdio_sbus(
    Aapl_t *aapl,               /**< [in] Pointer to AAPL structure */
    uint sbus_addr,             /**< [in] SBus address of SerDes */
    unsigned char reg_addr,     /**< [in] SBus data register address */
    unsigned char command,      /**< [in] SBus command */
    uint sbus_data)             /**< [in] 32 bit SBus data (if write command) */
{
/*////////////////////////////////////////////////////////////////////// */
/* The mdio interface is very labor intensive and inefficient with any */
/* read or write requiring an address to be set in advance of the */
/* transaction. The sbus-over-mdio has similar efficiency issues. The */
/* REG_SBUS_SA/DA/CMD etc... are all direct connections to the SBM */
/* core interface. Techincally, it's not required to rewrite all of */
/* these values for a single transaction if you have knowledge of their */
/* existing states. Reading the states doesn't actually save any time, */
/* but for a transaction dense function like eye capture (for example) */
/* it may be worthwhile to incorporate some of the hardware-server's */
/* MDIO efficieny improvements which involve maintaining shadow copies */
/* of the various settings and only updating what is necessary. This */
/* requires more overhead as it's possible for the user to cause resets */
/* or other actions that break the relationship between the shadow */
/* values and the actual core port values. This can be revisited at a */
/* later time. */
/*////////////////////////////////////////////////////////////////////// */

    int try_count = 0;
    int count = 0;
    int result = 0;
    int limit = aapl->sbus_mdio_timeout;
    uint rc = 0;
    Avago_addr_t addr_struct;
    avago_addr_to_struct(sbus_addr,&addr_struct);

    avago_mdio_wr(aapl, addr_struct.chip, AVSP_DEVAD, REG_SBUS_SA, sbus_addr);
    avago_mdio_wr(aapl, addr_struct.chip, AVSP_DEVAD, REG_SBUS_DA, reg_addr);
    avago_mdio_wr(aapl, addr_struct.chip, AVSP_DEVAD, REG_SBUS_CMD, command | 0x20);

    if (command == 0x01)
    {
        avago_mdio_wr(aapl, addr_struct.chip, AVSP_DEVAD, REG_SBUS_DATA_IN_LO,  (sbus_data & 0x0000ffff));
        avago_mdio_wr(aapl, addr_struct.chip, AVSP_DEVAD, REG_SBUS_DATA_IN_HI, ((sbus_data & 0xffff0000) >> 16));
    }

    do
    {
        int prev_result = 0;
        count = 0;

        avago_mdio(aapl, AVAGO_MDIO_ADDR, addr_struct.chip, AVSP_DEVAD, REG_SBUS_RESULT);
        do
        {
            result = avago_mdio(aapl, AVAGO_MDIO_READ, addr_struct.chip, AVSP_DEVAD, 0);
        } while ( (result & 1) == 1 && (++count <= limit));

        prev_result = result & 0x02;

        avago_mdio_wr(aapl, addr_struct.chip, AVSP_DEVAD, REG_SBUS_EXEC, 0x3);

        avago_mdio(aapl, AVAGO_MDIO_ADDR, addr_struct.chip, AVSP_DEVAD, REG_SBUS_RESULT);
        do
        {
            result = avago_mdio(aapl, AVAGO_MDIO_READ, addr_struct.chip, AVSP_DEVAD, 0);
        } while ( (result & 1) == 0 && (++count <= limit));

        if (command == 0x01 || command == 0x00)
        {
            avago_mdio_wr(aapl, addr_struct.chip, AVSP_DEVAD, REG_SBUS_EXEC, 0x2);
            return 0;
        }

        while( count <= limit )
        {
            while( ((result & 2) == prev_result) && (++count <= limit) )
            {
                result = avago_mdio(aapl, AVAGO_MDIO_READ, addr_struct.chip, AVSP_DEVAD, 0);
            }
            if( (result & 0x1c) == 0x10 )
                break;

            aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "MDIO read did not receive a read complete result: 0x%x\n",result);
            prev_result = result & 2;
        }

        avago_mdio_wr(aapl, addr_struct.chip, AVSP_DEVAD, REG_SBUS_EXEC, 0x2);

    } while( count > limit && ++try_count == 1 );

    if (count > limit)
    {
        aapl_fail(aapl, __func__, __LINE__, "SBus %s, MDIO response timed out after %d loops when reading address 0x%x, result = 0x%x.\n", aapl_addr_to_str(sbus_addr), count, reg_addr, result);
    }
    else
    {
        rc  =  avago_mdio_rd(aapl, addr_struct.chip, AVSP_DEVAD, REG_SBUS_DATA_OUT_LO) & 0xffff;
        rc |= (avago_mdio_rd(aapl, addr_struct.chip, AVSP_DEVAD, REG_SBUS_DATA_OUT_HI) & 0xffff) << 16;
    }
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "MDIO loop retries/count/limit = %d/%d/%d\n", try_count, count, limit);

    return rc;
}
