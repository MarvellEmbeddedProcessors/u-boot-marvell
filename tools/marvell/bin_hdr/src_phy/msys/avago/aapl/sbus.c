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

/* AAPL (ASIC and ASSP Programming Layer) SBus-related functions.  "SBus" is a */
/* frequently used command to a server (in contact with an ASIC, usually on a */
/* characterization board) to do I/O with an SBus master or individual slices */
/* (such as, but not necessarily, AVAGO_SERDES). */

/** Doxygen File Header */
/** @file */
/** @brief SBus functions. */

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"

#ifndef MV_HWS_REDUCED_BUILD
#if AAPL_ALLOW_OFFLINE_SBUS

static int *sbus_reg = 0;
static int offline_sbus_open(Aapl_t *aapl)
{
    (void)aapl;
    return 0;
}
static int offline_sbus_close(Aapl_t *aapl)
{
    (void)aapl;
    if( sbus_reg )
        AAPL_FREE(sbus_reg);
    sbus_reg = 0;
    return 0;
}
static uint offline_sbus(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    unsigned char reg_addr, /**< [in] SBus register to read/write */
    unsigned char command,  /**< [in] 0=reset, 1=write, 2=read */
    uint sbus_data)         /**< [in] Data to write */
{
    uint data = 0;
    uint index;
    Avago_addr_t addr_struct;
    avago_addr_to_struct(sbus_addr,&addr_struct);

    if( !sbus_reg )
    {
        uint index, chip, ring, sbus, reg_addr;
        int size = AAPL_MAX_CHIPS*AAPL_MAX_RINGS*256*256*sizeof(int);
        sbus_reg = (int *)AAPL_MALLOC(size);
        if( sbus_reg )
        {
            memset(sbus_reg,0,size);
        }
        for (chip = 0; chip < AAPL_MAX_CHIPS; chip++)
        {
            for (ring = 0; ring < AAPL_MAX_RINGS; ring++)
            {
                sbus = AVAGO_SBUS_CONTROLLER_ADDRESS;
                reg_addr = 0x2;
                index = ((chip * AAPL_MAX_RINGS + ring) * 256 + sbus) * 256 + reg_addr;
                if (chip == 0) sbus_reg[index] = 0x5 + 1;
                else           sbus_reg[index] = 0x2 + 1;

                for (sbus = 1; sbus <= 5; sbus ++)
                {
                    reg_addr = 0xff;
                    index = ((chip * AAPL_MAX_RINGS + ring) * 256 + sbus) * 256 + reg_addr;
                    sbus_reg[index] = AVAGO_SERDES;
                }

                sbus = AVAGO_SBUS_CONTROLLER_ADDRESS;
                reg_addr = 0xff;
                index = ((chip * AAPL_MAX_RINGS + ring) * 256 + sbus) * 256 + reg_addr;
                sbus_reg[index] = AVAGO_SBUS_CONTROLLER;

                sbus = AVAGO_SBUS_CONTROLLER_ADDRESS;
                reg_addr = 0xfe;
                index = ((chip * AAPL_MAX_RINGS + ring) * 256 + sbus) * 256 + reg_addr;
                sbus_reg[index] = 0xbe;

                sbus = AVAGO_SBUS_MASTER_ADDRESS;
                reg_addr = 0xff;
                index = ((chip * AAPL_MAX_RINGS + ring) * 256 + sbus) * 256 + reg_addr;
                sbus_reg[index] = AVAGO_SPICO;
            }
        }
    }


    index = ((addr_struct.chip * AAPL_MAX_RINGS + addr_struct.ring) * 256 + addr_struct.sbus) * 256 + reg_addr;

    aapl_log_printf(aapl, AVAGO_DEBUG9, 0,0,"OFFLINE: index=%x, %s, %x, %x, %x\n", index, aapl_addr_to_str(sbus_addr), reg_addr, command, sbus_data);


    if      (command == 0x01) data = sbus_reg[index] = sbus_data;
    else if (command == 0x02) data = sbus_reg[index];
    return data;
}
#endif

#endif /* MV_HWS_REDUCED_BUILD */

/*============================================================================= */
/* S B U S */
/* */
/** @brief   Execute an sbus command. */
/** @details Lowest level SBus function.  Allows the user to fully specify */
/**          all fields in the sbus command.  Generally, one should use */
/**          sbus_rd, sbus_wr and sbus_rmw instead of sbus. */
/** @return  For reads, returns read data. */
/**          For writes and reset, returns 0. */
uint avago_sbus(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    unsigned char reg_addr, /**< [in] SBus register to read/write */
    unsigned char command,  /**< [in] 0=reset, 1=write, 2=read */
    uint sbus_data,         /**< [in] Data to write */
    int recv_data_back)     /**< [in] Allows AACS protocol optimization. */
{
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    char sbus_cmd[AAPL_SBUS_CMD_LOG_BUF_SIZE+1];
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
    uint data = 0;
    Avago_addr_t addr_struct;

    avago_addr_to_struct(sbus_addr,&addr_struct);
    sbus_data &= 0xffffffff;

    if( addr_struct.chip == AVAGO_BROADCAST )
    {
        Avago_addr_t a_struct = addr_struct;
        for( a_struct.chip = 0; a_struct.chip < aapl->chips; a_struct.chip++ )
            data = avago_sbus(aapl, avago_struct_to_addr(&a_struct), reg_addr, command, sbus_data, recv_data_back);
        return data;
    }
    if( addr_struct.ring == AVAGO_BROADCAST )
    {
        Avago_addr_t a_struct = addr_struct;
        for( a_struct.ring = 0; a_struct.ring < AAPL_MAX_RINGS; a_struct.ring++ )
        {
            if( aapl->max_sbus_addr[a_struct.chip][a_struct.ring] != 0 )
                data = avago_sbus(aapl, avago_struct_to_addr(&a_struct), reg_addr, command, sbus_data, recv_data_back);
        }
        return data;
    }

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    snprintf(sbus_cmd, AAPL_SBUS_CMD_LOG_BUF_SIZE, "sbus %x%x%02x %02x %02x 0x%08x",
        addr_struct.chip, addr_struct.ring, addr_struct.sbus & 0xff, reg_addr, command, sbus_data);

    if( !aapl->max_sbus_addr[addr_struct.chip][addr_struct.ring] )
    {
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__,
            "SBus command (%s) destined for SBus address %s sent to ring %d which may not exist.\n",
            sbus_cmd, aapl_addr_to_str(sbus_addr), addr_struct.ring);
    }

#if AAPL_ALLOW_USER_SUPPLIED_SBUS
    if( aapl->communication_method == AVAGO_USER_SUPPLIED_SBUS_DIRECT )
    {

        data = user_supplied_sbus_function(aapl, sbus_addr, reg_addr,
                                         command, sbus_data, recv_data_back);
    }
    else
#endif
#if AAPL_ALLOW_OFFLINE_SBUS
    if( aapl->communication_method == AVAGO_OFFLINE )
    {
        data = offline_sbus(aapl, sbus_addr, reg_addr, command, sbus_data);
    }
    else
#endif
#if AAPL_ALLOW_AACS
    if( aapl->communication_method == AVAGO_AACS_SBUS )
    {
        avago_aacs_send_command_options(aapl, sbus_cmd, recv_data_back, /* strtol = */ 2);
        data = (uint)aapl->data;
    }
    else
#endif
    if( aapl_is_mdio_communication_method(aapl) )
    {
        data = avago_mdio_sbus(aapl, sbus_addr, reg_addr, command, sbus_data);
    }
    else if( aapl_is_i2c_communication_method(aapl) )
    {
        data = (uint)avago_i2c_sbus(aapl, sbus_addr, reg_addr, command, sbus_data);
    }
    else
    {
        aapl_fail(aapl,__func__,__LINE__,"Implementation missing.\n",0);
        return -1;
    }
#else
    data = user_supplied_sbus_function(aapl, sbus_addr, reg_addr,
                                     command, sbus_data, recv_data_back);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "%s -> 0x%08x\n", sbus_cmd, data);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
    return data;
}

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
int avago_sbus_open(Aapl_t *aapl)
{
    int rc;
#if AAPL_ALLOW_USER_SUPPLIED_SBUS
    if( aapl->communication_method == AVAGO_USER_SUPPLIED_SBUS_DIRECT )
        rc = user_supplied_sbus_open_function(aapl);
    else
#endif
#if AAPL_ALLOW_OFFLINE_SBUS
    if( aapl->communication_method == AVAGO_OFFLINE )
        rc = offline_sbus_open(aapl);
    else
#endif
        rc = 0;
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "open status = %d\n", rc);
    return rc;
}

int avago_sbus_close(Aapl_t *aapl)
{
    int rc;
#if AAPL_ALLOW_USER_SUPPLIED_SBUS
    if( aapl->communication_method == AVAGO_USER_SUPPLIED_SBUS_DIRECT )
        rc = user_supplied_sbus_close_function(aapl);
    else
#endif
#if AAPL_ALLOW_OFFLINE_SBUS
    if( aapl->communication_method == AVAGO_OFFLINE )
        rc = offline_sbus_close(aapl);
    else
#endif
        rc = 0;
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "close status = %d\n", rc);
    return rc;
}
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/*============================================================================= */
/*  SBUS RD */
/* */
/** @brief  SBus read operation */
/** @return Returns the result of the read operation.  */
/** */
uint avago_sbus_rd(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    unsigned char reg_addr) /**< [in] Register to read */
{
    return avago_sbus(aapl, sbus_addr, reg_addr, 0x02, 0x00000000, /* recv_data_back */ 1);
}

/*============================================================================= */
/*  SBUS WR */
/* */
/** @brief  SBus write operation */
/** @return Returns the result of the avago_sbus() write function, which should generally be ignored. */
/** */
uint avago_sbus_wr(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    unsigned char reg_addr, /**< [in] Register to update */
    uint sbus_data)         /**< [in] Data to write */
{
    return avago_sbus(aapl, sbus_addr, reg_addr, 0x01, sbus_data, 0);
}

#ifndef MV_HWS_REDUCED_BUILD
/*============================================================================= */
/*  SBUS WR FLUSH */
/* */
/** @brief  SBus write operation with flush */
/** @return Returns the result of the avago_sbus() write function, which should generally be ignored. */
/** */
uint avago_sbus_wr_flush(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    unsigned char reg_addr, /**< [in] Register to update */
    uint sbus_data)         /**< [in] Data to write */
{
    return avago_sbus(aapl, sbus_addr, reg_addr, 0x01, sbus_data, 1);
}
#endif /* MV_HWS_REDUCED_BUILD */
/*============================================================================= */
/*  SBUS RMW */
/* */
/** @brief  Modify some bits in an existing SBus register */
/** @return Returns the initial value of the SBus register before it was modified. */
uint avago_sbus_rmw(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    unsigned char reg_addr, /**< [in] Register to update */
    uint sbus_data,         /**< [in] Data to write */
    uint mask)              /**< [in] Bits in existing data to modify */
{


    uint initial_value = avago_sbus_rd(aapl, sbus_addr, reg_addr);
    avago_sbus_wr(aapl, sbus_addr, reg_addr, (sbus_data & mask) | (initial_value & ~mask));
    return initial_value;
}

/*============================================================================= */
/* S B U S   R E S E T */
/* */
/** @brief   Perform an sbus reset action. */
/** */
/** @return  void */
void avago_sbus_reset(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    uint sbus_addr_in,      /**< [in] SBus slice address. */
    int hard)
{
    BOOL st;
    Avago_addr_t addr_struct, start, stop, next;

    if( hard )
    {
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3

#if AAPL_ALLOW_AACS
#ifndef MV_HWS_REDUCED_BUILD
        if (aapl->capabilities & AACS_SERVER_SBUS_RESET)
        {
            char cmd[64];
            snprintf(cmd, 63, "sbus_reset 0x%x", sbus_addr_in);
            avago_aacs_send_command(aapl, cmd);
        }
        else if(aapl_is_aacs_communication_method(aapl))
        {
            Aapl_comm_method_t comm_method;
            char hs1_mode[64];
            char set_sbus[32];
            uint set_sbus_bit = 47;
            uint tap_gen;

            AAPL_SUPPRESS_ERRORS_PUSH(aapl); /* can be done only after parameters declaration */
            tap_gen = avago_get_tap_gen(aapl);
            snprintf (hs1_mode, 64, "sbus_mode %s", avago_aacs_send_command(aapl, "sbus_mode"));
            {
                char *ptr = strstr(hs1_mode," I2C");
                if( ptr )
                    *ptr = '\0';
            }
            comm_method = aapl->communication_method;
            aapl->communication_method = AVAGO_AACS_SBUS;
            avago_aacs_send_command(aapl, "sbus_mode jtag");

            if      (tap_gen == 1) set_sbus_bit = 46;
            else if (tap_gen == 2) set_sbus_bit = 46;
            else if (tap_gen == 3) set_sbus_bit = 47;
            else if (tap_gen == 4) set_sbus_bit = 46;

            snprintf(set_sbus, 32, "set_sbus %u %d", set_sbus_bit, 1);
            avago_aacs_send_command(aapl, set_sbus);
            avago_sbus_rd(aapl, avago_make_sbus_master_addr(sbus_addr_in), 0x2);

            snprintf(set_sbus, 32, "set_sbus %u %d", set_sbus_bit, 0);
            avago_aacs_send_command(aapl, set_sbus);
            avago_sbus_rd(aapl, avago_make_sbus_master_addr(sbus_addr_in), 0x2);

            avago_aacs_send_command(aapl, hs1_mode);
            aapl->communication_method = comm_method;
            AAPL_SUPPRESS_ERRORS_POP(aapl);
        }
#endif /* MV_HWS_REDUCED_BUILD */
#endif

        if(aapl_is_mdio_communication_method(aapl))
        {
            int prev_timeout;
            uint reset_val;

            avago_addr_to_struct(sbus_addr_in, &addr_struct);

            reset_val = avago_mdio_rd(aapl, addr_struct.chip, AVSP_DEVAD, AVAGO_MDIO_REG_RESET);

            aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Initial MDIO reset reg: 0x%0x\n", reset_val);

            reset_val &= 0x1;
            reset_val |= (reset_val << 1);

            reset_val ^= 3;

            prev_timeout = aapl->sbus_mdio_timeout;
            avago_mdio_wr(aapl, addr_struct.chip, AVSP_DEVAD, AVAGO_MDIO_REG_RESET, 0x0100 | reset_val);
            avago_mdio_wr(aapl, addr_struct.chip, AVSP_DEVAD, AVAGO_MDIO_REG_RESET, 0x0100 | (reset_val ^ 3));
            aapl->sbus_mdio_timeout = 10000;

            AAPL_SUPPRESS_ERRORS_PUSH(aapl);
            avago_sbus_rd(aapl, avago_make_sbus_controller_addr(sbus_addr_in), 0xfe);
            AAPL_SUPPRESS_ERRORS_POP(aapl);
            aapl->sbus_mdio_timeout = prev_timeout;

            aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Final MDIO reset reg: 0x%0x\n", 0x0100 | (reset_val ^ 3));
        }
        else if(aapl_is_i2c_communication_method(aapl))
        {
            avago_i2c_sbus(aapl, sbus_addr_in, 0, 3, 0);
#else
            avago_sbus(aapl, sbus_addr_in, 0x00, 0x03, 0x00, 0);
#endif /* MV_HWS_REDUCED_BUILD */

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
        }
        else if (!aapl_is_aacs_communication_method(aapl))
        {
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Hard reset not implemented, using soft SBus reset instead.\n", 0);
            hard = 0;
        }
#endif /* MV_HWS_REDUCED_BUILD */
    }

    if(hard)
    {
        sbus_addr_in = avago_make_serdes_broadcast_addr(sbus_addr_in);
    }
    avago_addr_to_struct(sbus_addr_in, &addr_struct);
    for( st = aapl_broadcast_first(aapl, &addr_struct, &start, &stop, &next);
         st;
         st = aapl_broadcast_next(aapl,&next,&start,&stop) )
    {
        uint sbus_addr = avago_struct_to_addr(&next);

        if( aapl_check_ip_type(aapl, sbus_addr, __func__, __LINE__, FALSE, 1, AVAGO_UNKNOWN_IP) )
            continue;

        if (!hard)
            avago_sbus(aapl, sbus_addr, 0x00, 0x00, 0x00, 1);
#ifndef MV_HWS_REDUCED_BUILD
        if( aapl_check_ip_type(aapl, sbus_addr, __func__, __LINE__, FALSE, 1, AVAGO_SERDES) &&
            aapl_check_process(aapl, sbus_addr, __func__, __LINE__, FALSE, 2, AVAGO_PROCESS_B, AVAGO_PROCESS_F) )
#else
        if( aapl_check_ip_type(aapl, sbus_addr, __func__, __LINE__, FALSE, 1, AVAGO_SERDES))
#endif /* MV_HWS_REDUCED_BUILD */
        {
            avago_sbus_wr(aapl, sbus_addr, 0x01, 0x20000000);
        }
    }
}

/*============================================================================= */
/* GET TEMP DATA */
/** @brief  Gets the temperature data from a given AVAGO_THERMAL_SENSOR sensor */
/** @return Returns the temperature in milli-degrees C. */
/**         On error, returns 999999. */
/** @details Verifies that the process is 28nm, the #sensor_addr is */
/**          AVAGO_THERMAL_SENSOR, and the SBus Master SPICO for the sensor */
/**          is running, then issues the get_temp_data interrupt. */
/**          Takes the resulting 12b signed value and converts it to an integer. */
int avago_sbm_get_temperature(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint sensor_addr,   /**< [in] SBus address of the AVAGO_THERMAL_SENSOR */
    uint sensor)        /**< [in] Which sensor to get data for. */
{
    const int invalid_data = 999999;
    int data, i;
    int retry=10;

    /* trying to get the Temperature value maximum 10 times and set 1ms delay between each time */
    for(i=0; i < retry; i++)
    {
        data = avago_spico_int(aapl, AVAGO_SBUS_MASTER_ADDRESS, 0x17, (0 << 12) | (sensor_addr & 0xff));
        if (data & 0x8000)
        {
            data &= 0x0FFF;     /* Mask down to 12b temp value */
            if (data & 0x0800)  /* Negative 12b temp value, do sign extension */
                data = -1 * (((~data) & 0x0FFF) + 1);

            return (data / 8);  /* Scale to degrees */
        }

        /* Delay in 1ms */
        ms_sleep(1);
    }

    return invalid_data;  /* Temperature not valid */
}

/** @brief   Performs a read-write test to the SBus Controller. */
/** @details Communication with the SBus Controller (sbus address 0xfd) is */
/**          confirmed via a quick read-write test of address 0x13, which is */
/**          restored to its original value upon return. */
/** @return  TRUE if the test passes, FALSE if any errors. */
BOOL avago_diag_sbus_rw_test(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    uint sbus_addr,     /**< [in] SBus slice address. */
    int cycles)         /**< [in] Number of write-read cycles to do. */
{
    int result = TRUE, x, ran, data, addr;
    if (aapl_check_broadcast_address(aapl, sbus_addr, __func__, __LINE__, TRUE)) return FALSE;

    addr = avago_make_sbus_controller_addr(sbus_addr);

    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    {
        int orig_val = avago_sbus_rd(aapl, addr, 0x13);

        ran = 0x1678ab4a;
        ran ^= (size_t) &ran;
        for( x=1; x <= cycles; x++ )
        {
            ran = (ran << 1) | (((ran >> 0) ^ (ran >> 6)) & 0x1);
            avago_sbus_wr(aapl, addr, 0x13, ran);
            data = avago_sbus_rd(aapl, addr, 0x13);
            if (data != ran)
            {
                result = FALSE;
                break;
            }
            if (x % 1000 == 0) aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "SBus R/W %d cycles complete on address %s.\n", x, aapl_addr_to_str(addr));
        }
        avago_sbus_wr(aapl, addr, 0x13, orig_val);
    }
    AAPL_SUPPRESS_ERRORS_POP(aapl);
#ifndef MV_HWS_REDUCED_BUILD
    if (!result) aapl_fail(aapl, __func__, __LINE__, "SBus readback failed on loop %d. Expected 0x%02x, but got 0x%02x.\n", x, ran, data);
    else         aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "SBus R/W test passed on address %s after %d cycles.\n", aapl_addr_to_str(addr), cycles);
#endif /* MV_HWS_REDUCED_BUILD */
    return result;
}
