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

/* AAPL (ASIC and ASSP Programming Layer) "system level" calls (not AVAGO_SERDES, or */
/* SPICO, etc). */

/** Doxygen File Header */
/** @file */
/** @brief Core AAPL functions. */

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"
#if  !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
#include "asic_info.h"
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#define AAPL_3D_ARRAY_ADDR(addr) (addr).chip][(addr).ring][(addr).sbus

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
/* Global default values: */
const char *aapl_default_server = "localhost";
const uint  aapl_default_port   = 90;

BOOL aapl_is_i2c_communication_method(Aapl_t *aapl)
{
    return aapl->communication_method == AVAGO_USER_SUPPLIED_I2C ||
           aapl->communication_method == AVAGO_AACS_I2C ||
           aapl->communication_method == AVAGO_SYSTEM_I2C;
}

BOOL aapl_is_mdio_communication_method(Aapl_t *aapl)
{
    return aapl->communication_method == AVAGO_USER_SUPPLIED_MDIO ||
           aapl->communication_method == AVAGO_AACS_MDIO ||
           aapl->communication_method == AVAGO_SYSTEM_MDIO ||
           aapl->communication_method == AVAGO_GPIO_MDIO;
}

BOOL aapl_is_sbus_communication_method(Aapl_t *aapl)
{
    return aapl->communication_method == AVAGO_USER_SUPPLIED_SBUS_DIRECT ||
           aapl->communication_method == AVAGO_AACS_SBUS;
}

BOOL aapl_is_aacs_communication_method(Aapl_t *aapl)
{
    return aapl->communication_method == AVAGO_AACS_SBUS ||
           aapl->communication_method == AVAGO_AACS_MDIO ||
           aapl->communication_method == AVAGO_AACS_I2C;
}

BOOL aapl_is_user_communication_method(Aapl_t *aapl)
{
    return aapl->communication_method == AVAGO_USER_SUPPLIED_SBUS_DIRECT ||
           aapl->communication_method == AVAGO_USER_SUPPLIED_MDIO ||
           aapl->communication_method == AVAGO_USER_SUPPLIED_I2C;
}

BOOL aapl_is_system_communication_method(Aapl_t *aapl)
{
    return aapl->communication_method == AVAGO_SYSTEM_I2C ||
           aapl->communication_method == AVAGO_SYSTEM_MDIO;
}
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#ifdef MV_HWS_REDUCED_BUILD
int  aapl_fail_reduce(Aapl_t *aapl)
{
    if( aapl->return_code > 0 )
        aapl->return_code = 0;

    aapl->return_code -= 1;
    return -1;
}
#endif /* MV_HWS_REDUCED_BUILD */

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#ifndef MV_HWS_REDUCED_BUILD
/** @brief   Sets the value of the async_cancel flag in Aapl_t. */
/** @details The async_cancel flag is used to terminate long AAPL */
/**          operations before they would otherwise return.  This is a */
/**          cooperative cancel, so only certain operations can be canceled. */
/**          The caller should clear this flag after use, before calling */
/**          another cancelable function. */
/** @see     aapl_get_async_cancel_flag(). */
/** @return  Returns the previous value of the flag. */
int aapl_set_async_cancel_flag(Aapl_t *aapl, int new_value)
{
    int old_value = aapl->async_cancel;
    aapl->async_cancel = new_value;
    return old_value;
}

/** @brief   Gets the current value of the async_cancel flag from Aapl_t. */
/** @see     aapl_set_async_cancel_flag(). */
int aapl_get_async_cancel_flag(Aapl_t *aapl)
{
    return aapl->async_cancel;
}
#endif /* MV_HWS_REDUCED_BUILD */

/** @brief   Opens a connection to an Avago device. */
/** @details If aacs_server is NULL, assumes that the aacs_server and tcp_port */
/**          fields of Aapl_t have already been set. */
/** @return  void, but decrements aapl->return_code value on error. */
void aapl_connect(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    const char *aacs_server,    /**< [in] AACS server name string. */
    int tcp_port)               /**< [in] TCP port number for AACS server. */
{
    (void)aacs_server;
    (void)tcp_port;
#if AAPL_ALLOW_AACS
    if( aacs_server )
    {
        uint len;
        if (!aapl->aacs_server)
        {
            aapl_fail(aapl, __func__, __LINE__, "aapl_connect() called using Aapl_t struct that wasn't initialized.\n", 0);
            return;
        }
        len = strlen(aacs_server)+1;
        aapl->aacs_server = (char *) aapl_realloc(aapl, aapl->aacs_server, len, __func__);
        strncpy(aapl->aacs_server, aacs_server, len);
    }
    if( tcp_port )
        aapl->tcp_port = tcp_port;

    if( aapl_is_aacs_communication_method(aapl) )
        avago_aacs_open(aapl);
    else
#endif
#if AAPL_ALLOW_SYSTEM_I2C || AAPL_ALLOW_USER_SUPPLIED_I2C
    if( aapl_is_i2c_communication_method(aapl) )
        avago_i2c_open(aapl);
    else
#endif
#if AAPL_ALLOW_SYSTEM_MDIO || AAPL_ALLOW_USER_SUPPLIED_MDIO || AAPL_ALLOW_GPIO_MDIO
    if( aapl_is_mdio_communication_method(aapl) )
        avago_mdio_open(aapl);
    else
#endif
#if AAPL_ALLOW_USER_SUPPLIED_SBUS || AAPL_ALLOW_OFFLINE_SBUS
    if( aapl_is_sbus_communication_method(aapl) )
        avago_sbus_open(aapl);
    else
#endif
    return;
}

/* @brief  Closes the current connection to the AAPL device. */
/* @detail Does not flush buffered commands to avoid infinite loops on error. */
/* @return void */
static void aapl_close_connection2(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
#if AAPL_ALLOW_AACS
    if( aapl_is_aacs_communication_method(aapl) )
        avago_aacs_close(aapl);
    else
#endif
#if AAPL_ALLOW_SYSTEM_I2C || AAPL_ALLOW_USER_SUPPLIED_I2C
    if( aapl_is_i2c_communication_method(aapl) )
        avago_i2c_close(aapl);
    else
#endif
#if AAPL_ALLOW_SYSTEM_MDIO || AAPL_ALLOW_USER_SUPPLIED_MDIO || AAPL_ALLOW_GPIO_MDIO
    if( aapl_is_mdio_communication_method(aapl) )
        avago_mdio_close(aapl);
    else
#endif
#if AAPL_ALLOW_USER_SUPPLIED_SBUS || AAPL_ALLOW_OFFLINE_SBUS
    if( aapl_is_sbus_communication_method(aapl) )
        avago_sbus_close(aapl);
    else
#endif
    return;
}

/** @brief   Closes the current connection to the AAPL device. */
/** @details Will flush any buffered commands first. */
/** @return  void */
void aapl_close_connection(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Closing connection %d.\n", aapl->socket);
    avago_aacs_flush(aapl);
    AAPL_SUPPRESS_ERRORS_POP(aapl);
    aapl_close_connection2(aapl);
}
/** @brief Checks the socket status of aapl->socket */
/** */
/** @return 0 if the socket is still connected, -1 if it is closed */
int aapl_connection_status(
    Aapl_t *aapl)   /**< [in] Pointer to AAPL structure. */
{
    if( aapl->socket < 0 )
        return -1;
    #if AAPL_ALLOW_AACS
    if (aapl_is_aacs_communication_method(aapl))
    {
        int rc, error = 0;
        socklen_t len = sizeof (error);
        avago_aacs_send_command(aapl, "version");

        rc = getsockopt (aapl->socket, SOL_SOCKET, SO_ERROR, (void *)&error, &len );

        aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "socket: %d getsockopt: %d errno: %s\n", aapl->socket, rc, aapl_tcp_strerr());

        return rc;
    }
    #endif
    if ( aapl_is_i2c_communication_method(aapl) || aapl_is_mdio_communication_method(aapl) || aapl_is_sbus_communication_method(aapl) )
    {
        return 0;
    }
    return -1;
}

/*============================================================================= */
/* AAPL CONSTRUCT */
/** @brief Constructs the Aapl_t structure. Allocates memory and sets default values. */
/** @warning Generally the user should not read or modify any elements of this struct. */
/** Use functions provided to get and set information in this structure. */
/** @return A pointer to the Aapl_t struct */
Aapl_t *aapl_construct()
{
    int i;
/* TBD:  The malloc()s here can't call aapl_malloc() because it needs an aapl */
/* pointer with log buffer contents, so probably they should report an error */
/* somehow and exit(). */

    Aapl_t* aapl = (Aapl_t *) AAPL_MALLOC(sizeof(Aapl_t));

    memset(aapl, 0, sizeof(*aapl));

    for( i = 0; i < AAPL_MAX_CHIPS; i++ )
        aapl->chip_name[i] = aapl->chip_rev[i] = "";

    aapl->log            = (char *) AAPL_MALLOC(AAPL_LOG_BUF_SIZE);
    aapl->log_end        = aapl->log;
    aapl->log_size       = AAPL_LOG_BUF_SIZE;
    *aapl->log           = '\0';

    aapl->socket         = -1;
    aapl->aacs_server    = aapl_strdup(aapl_default_server);

    aapl->tcp_port       = aapl_default_port;

    aapl->data_char      = (char *) AAPL_MALLOC(AAPL_LOG_BUF_SIZE);
    aapl->data_char_end  = aapl->data_char;
    aapl->data_char_size = AAPL_LOG_BUF_SIZE;
    *aapl->data_char     = '\0';
    aapl->debug = 0;
    aapl->enable_debug_logging      = AAPL_DEFAULT_ENABLE_DEBUG_LOGGING;
    aapl->enable_stream_logging     = AAPL_DEFAULT_ENABLE_STREAM_LOGGING;
    aapl->enable_stream_err_logging = AAPL_DEFAULT_ENABLE_STREAM_ERR_LOGGING;
    aapl->enable_serdes_core_port_interrupt = AAPL_DEFAULT_SERDES_CORE_PORT_INT;
    aapl->chips                 = AAPL_NUMBER_OF_CHIPS_OVERRIDE;
    aapl->sbus_rings            = AAPL_NUMBER_OF_RINGS_OVERRIDE;

# ifdef AAPL_LOG_TIME_STAMPS
    aapl->log_time_stamps       = AAPL_LOG_TIME_STAMPS;
# endif
    aapl->serdes_int_timeout    = AAPL_SERDES_INT_TIMEOUT;
    aapl->sbus_mdio_timeout     = AAPL_SBUS_MDIO_TIMEOUT;
    aapl->max_cmds_buffered     = AAPL_MAX_CMDS_BUFFERED;
    aapl->communication_method  = AAPL_DEFAULT_COMM_METHOD;
    aapl->i2c_base_addr         = AAPL_DEFAULT_I2C_BASE_ADDR;
    aapl->mdio_base_port_addr   = AAPL_DEFAULT_MDIO_BASE_PORT_ADDR;

    return aapl;
}

/*============================================================================= */
/* AAPL DESTRUCT */
/** */
/** @brief Frees memory of an aapl struct when it is no longer needed. */
/** @return void */
void aapl_destruct(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    aapl_close_connection(aapl);

    if (aapl->data_char)            AAPL_FREE(aapl->data_char);
    if (aapl->buf_cmd)              AAPL_FREE(aapl->buf_cmd);
    if (aapl->log)                  AAPL_FREE(aapl->log);
    if (aapl->aacs_server)          AAPL_FREE(aapl->aacs_server);
    if (aapl->aacs_server_buffer)   AAPL_FREE(aapl->aacs_server_buffer);
#if 0 /* aapl is staticly defined */
    AAPL_FREE(aapl);
#endif
}
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/*============================================================================= */
/* AAPL GET RETURN CODE */
/** */
/** @brief Returns the return code and then sets it to zero. */
/**    Values less than 0 indicate that some failure has occurred. */
/** @warning Calling this function clears the return code. */
/** @return The current value of the AAPL return code. */
int aapl_get_return_code(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    int x;
    x = aapl->return_code;
    aapl->return_code = 0;
    return x;
}

/*============================================================================= */
/* AAPL INIT */
/** @brief Constructs the Aapl_t structure.and sets default values. */
/** @warning Generally the user should not read or modify any elements of this struct. */
/** Use functions provided to get and set information in this structure. */
void aapl_init(Aapl_t *aapl)
{
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    int i;
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
    memset(aapl, 0, sizeof(Aapl_t));
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    for( i = 0; i < AAPL_MAX_CHIPS; i++ )
        aapl->chip_name[i] = aapl->chip_rev[i] = "";

    aapl->log            = (char *) AAPL_MALLOC(AAPL_LOG_BUF_SIZE);
    aapl->log_end        = aapl->log;
    aapl->log_size       = 0;
    *aapl->log           = '\0';

    aapl->socket         = -1;
    aapl->aacs_server    = aapl_strdup(aapl_default_server);

    aapl->tcp_port       = aapl_default_port;

    aapl->data_char      = (char *) AAPL_MALLOC(AAPL_LOG_BUF_SIZE);
    aapl->data_char_end  = aapl->data_char;
    aapl->data_char_size = 0;
    *aapl->data_char     = '\0';
    aapl->debug = 0;
    aapl->enable_debug_logging      = AAPL_DEFAULT_ENABLE_DEBUG_LOGGING;
    aapl->enable_stream_logging     = AAPL_DEFAULT_ENABLE_STREAM_LOGGING;
    aapl->enable_stream_err_logging = AAPL_DEFAULT_ENABLE_STREAM_ERR_LOGGING;
    aapl->enable_serdes_core_port_interrupt = AAPL_DEFAULT_SERDES_CORE_PORT_INT;
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

    aapl->chips                 = AAPL_NUMBER_OF_CHIPS_OVERRIDE;
    aapl->sbus_rings            = AAPL_NUMBER_OF_RINGS_OVERRIDE;
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
# ifdef AAPL_LOG_TIME_STAMPS
    aapl->log_time_stamps       = AAPL_LOG_TIME_STAMPS;
# endif
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

    aapl->serdes_int_timeout    = AAPL_SERDES_INT_TIMEOUT;
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    aapl->sbus_mdio_timeout     = AAPL_SBUS_MDIO_TIMEOUT;
    aapl->max_cmds_buffered     = AAPL_MAX_CMDS_BUFFERED;
    aapl->communication_method  = AAPL_DEFAULT_COMM_METHOD;
    aapl->i2c_base_addr         = AAPL_DEFAULT_I2C_BASE_ADDR;
    aapl->mdio_base_port_addr   = AAPL_DEFAULT_MDIO_BASE_PORT_ADDR;
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
}

/* The following functions return information from the AAPL struct. addr is used to return specific information for that element. */

/** @brief  Gets the number of chips found on the current ring. */
/** @return The number of chips on the current ring. */
uint aapl_get_chips(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    return aapl->chips;
}

/** @brief  Gets the maximum sbus address for the specified chip and sbus ring within that chip. */
/** @return Maximum sbus address of the sbus ring and chip specified by the sbus address. */
uint aapl_get_max_sbus_addr(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS || addr_struct.ring >= AAPL_MAX_RINGS )
        return 0;
    return aapl->max_sbus_addr[addr_struct.chip][addr_struct.ring];
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief Get the JTAG idcode of the specified chip */
/** @return JTAG idcode of the current sbus ring (specified by the sbus address) */
uint aapl_get_jtag_idcode(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS ) addr_struct.chip = 0;
    return aapl->jtag_idcode[addr_struct.chip];
}

/** @brief Get the chip name of the specified chip */
/** @return The chip name of the current chip (specified by the sbus address) */
/**        Returns the aapl->data_char string pointer, which is a C-string holding the command status. */
/**        aapl->data is set to length of the returned string. */
/** */
const char *aapl_get_chip_name(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS )
        return "unknown";
    return aapl->chip_name[addr_struct.chip];
}

/** @brief  Gets the chip revision number of the specified chip. */
/** @return The chip revision number of the current chip (specified by the sbus address). */
const char *aapl_get_chip_rev_str(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS )
        return "";
    return aapl->chip_rev[addr_struct.chip];
}
#endif /* MV_HWS_REDUCED_BUILD */



/** @brief Gets the Logical Sub Block (LSB) revision number of the device. */
/** @details */
/** Note: aapl_get_ip_info() stores the lowest lsb rev in [0][0][0xff] */
/** @return The Logical Sub Block (LSB) revision number of the addressed device. */
uint aapl_get_lsb_rev(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);

    if( aapl_check_broadcast_address(aapl, addr, __func__, __LINE__, FALSE) )
        return aapl->lsb_rev[0][0][0xff];

    if( addr_struct.chip >= AAPL_MAX_CHIPS ||
        addr_struct.ring >= AAPL_MAX_RINGS ||
        addr_struct.sbus >  255 )
        return 0;

    if (!aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)])
         aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] = avago_serdes_get_lsb_rev(aapl, addr);

    if( (aapl->lsb_rev[0][0][0xff] == 0) ||
        (aapl->lsb_rev[0][0][0xff] > aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)]) )
         aapl->lsb_rev[0][0][0xff] = aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)];

    return aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)];
}


/*============================================================================= */
/* IP REV */
/* Returns the IP revision number */
static int ip_rev(Aapl_t *aapl, uint sbus_addr)
{
    if (!aapl_check_process(aapl, sbus_addr, __func__, __LINE__, TRUE, 3, AVAGO_PROCESS_B, AVAGO_PROCESS_F, AVAGO_PROCESS_A)) return -1;

    if( aapl_check_ip_type(aapl, sbus_addr, __func__,__LINE__,FALSE,2,AVAGO_SERDES, AVAGO_M4) )
    {
        if( aapl_get_process_id(aapl, sbus_addr) == AVAGO_PROCESS_F )
        {
            int ret_data = avago_sbus_rd(aapl, sbus_addr, 0xfe);
            if( 0 == ret_data )
            {
                ret_data = avago_serdes_mem_rd(aapl, sbus_addr, AVAGO_ESB, 0x0303);
            }
            return (ret_data >> 8) & 0xff;
        }
#ifndef MV_HWS_REDUCED_BUILD
        else if( aapl_get_process_id(aapl, sbus_addr) == AVAGO_PROCESS_B )
        {
            int ret_data = avago_sbus_rd(aapl, sbus_addr, 0xfe);
            return (ret_data >> 8) & 0xff;
        }
#endif /* MV_HWS_REDUCED_BUILD */
        return avago_sbus_rd(aapl, sbus_addr, 0xfe) & 0xff;
    }
    else if( aapl_check_ip_type(aapl, sbus_addr, __func__,__LINE__,FALSE,1,AVAGO_SPICO) ) return (avago_sbus_rd(aapl, sbus_addr, 0x0f) >> 12) & 0xf;
    else if( aapl_check_ip_type(aapl, sbus_addr, __func__,__LINE__,FALSE,1,AVAGO_SBUS_CONTROLLER) ) return avago_sbus_rd(aapl, sbus_addr, 0xfe);
#ifndef MV_HWS_REDUCED_BUILD
    else if( aapl_check_ip_type(aapl, sbus_addr, __func__,__LINE__,FALSE,4,AVAGO_OPAL_RSFEC528,AVAGO_OPAL_RSFEC528_544,AVAGO_OPAL_HOST_ALIGNER,AVAGO_OPAL_MOD_ALIGNER) ) return avago_sbus_rd(aapl, sbus_addr, 0xfc);
    else if( aapl_check_ip_type(aapl, sbus_addr, __func__,__LINE__,FALSE,1,AVAGO_OPAL_CONTROL) ) return avago_sbus_rd(aapl, sbus_addr, 0xfc) >> 12;
#endif /* MV_HWS_REDUCED_BUILD */

    return -1;
}

/** @brief Gets the revision number of the sbus device for the specified chip */
/**        and sbus ring */
/** @return The revision number of the sbus device for the current chip and */
/**        sbus ring (specified by the sbus address) */
/** */
uint aapl_get_ip_rev(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS || addr_struct.chip == AVAGO_BROADCAST ||
        addr_struct.ring >= AAPL_MAX_RINGS || addr_struct.ring == AVAGO_BROADCAST ||
        addr_struct.sbus >  255 )
        return 0;
    if( addr_struct.sbus == AVAGO_BROADCAST ) addr_struct.sbus = 255;
    if (aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)] == 0xff) aapl_set_ip_type(aapl, addr);
    if (aapl->ip_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] == 0xfffe) aapl->ip_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] = ip_rev(aapl, addr) & 0xFFFF;
    return aapl->ip_rev[AAPL_3D_ARRAY_ADDR(addr_struct)];
}


/** @brief  Returns the firmware rev from AAPL's cache */
/** @return Returns the firmware rev from AAPL's cache */
int aapl_get_firmware_rev(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS || addr_struct.chip == AVAGO_BROADCAST ||
        addr_struct.ring >= AAPL_MAX_RINGS || addr_struct.ring == AVAGO_BROADCAST ||
        addr_struct.sbus >  255 )
        return 0;
    if( addr_struct.sbus == AVAGO_BROADCAST ) addr_struct.sbus = 255;
    if (aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)] == 0xff) aapl_set_ip_type(aapl, addr);
    if (!aapl->firm_rev[AAPL_3D_ARRAY_ADDR(addr_struct)]) aapl->firm_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] = avago_firmware_get_rev(aapl, addr);
    return aapl->firm_rev[AAPL_3D_ARRAY_ADDR(addr_struct)];
}


/** @brief  Returns the firmware build from AAPL's cache */
/** @return Returns the firmware build from AAPL's cache */
int aapl_get_firmware_build(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS || addr_struct.chip == AVAGO_BROADCAST ||
        addr_struct.ring >= AAPL_MAX_RINGS || addr_struct.ring == AVAGO_BROADCAST ||
        addr_struct.sbus >  255 )
        return 0;
    if( addr_struct.sbus == AVAGO_BROADCAST ) addr_struct.sbus = 255;
    if (aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)] == 0xff) aapl_set_ip_type(aapl, addr);
    if (!aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)]) aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)] = avago_firmware_get_build_id(aapl, addr);
    return aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)];
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief  Sets the client data pointer. */
/** @return void */
void aapl_bind_set(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    void *client_data)  /**< [in] Pointer to client data. */
{
    aapl->client_data = client_data;
}

/** @brief  Retrieves the client data pointer. */
/** @return Pointer to client data. */
void *aapl_bind_get(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    return aapl->client_data;
}
#endif /* MV_HWS_REDUCED_BUILD */

/*============================================================================= */
/* AAPL GET IP TYPE */

/** @brief  Gets the IP type for the sbus address. */
/** @return The IP type of the sbus device for the current chip and */
/**         sbus ring as specified by the sbus address. */
Avago_ip_type_t aapl_get_ip_type(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS ) addr_struct.chip = 0;
    if( addr_struct.ring >= AAPL_MAX_RINGS ) addr_struct.ring = 0;
    if( addr_struct.sbus >= 255            ) addr_struct.sbus = 255;
    if (aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)] == 0xff) aapl_set_ip_type(aapl, addr);
    return (Avago_ip_type_t)(aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)]);
}

#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
/** @brief Internal function that returns index from avago_chip_id array (from asic_info.h) */
static int avago_find_chip_index(uint jtag, int jtag_mask)
{
    int at;
    for( at = 0; at < AAPL_ARRAY_LENGTH(avago_chip_id); ++at )
    {
        if( jtag == (avago_chip_id[at].jtag_idcode & jtag_mask) )
            return at;
    }
    return -1;
}
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/** @brief Internal function that returns the process id of a device. Can be used as a check to see if SBus is working. */
static Avago_process_id_t get_process_id(Aapl_t *aapl, uint chip)
{
#ifndef MV_HWS_REDUCED_BUILD
    int rc = aapl->return_code;
    Avago_process_id_t ret = AVAGO_UNKNOWN_PROCESS;
    uint data = avago_sbus_rd(aapl, avago_make_addr3(chip, 0, 0xfe), 0xfe);

    if( aapl->return_code == rc )
    {
        switch (data)
        {
        case 0xbb: ret = AVAGO_PROCESS_D; break;
        case 0xbc: ret = AVAGO_PROCESS_E; break;
        default: aapl_log_printf(aapl,AVAGO_WARNING,__func__,__LINE__,"Unknown process id: 0x%x\n",data);
        case 0xbd:
            {
                int id = avago_find_chip_index(aapl->jtag_idcode[chip], 0xffffffff);
                if( id >= 0 )
                    ret = avago_chip_id[id].process_id;
                if( ret == AVAGO_UNKNOWN_PROCESS && data == 0xbd )
                    ret = AVAGO_PROCESS_F;
            }
            break;

        case 0xbe: ret = AVAGO_PROCESS_F; break;
        case 0xbf: ret = AVAGO_PROCESS_F; break;
        case 0xc1: ret = AVAGO_PROCESS_F; break;

        case 0xc2:
        case 0xc0: ret = AVAGO_PROCESS_B; break;
        }
    }
    return ret;
#else
    return AVAGO_PROCESS_F;
#endif /* MV_HWS_REDUCED_BUILD */
}

Avago_process_id_t aapl_get_process_id(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);

    if (addr_struct.chip == AVAGO_BROADCAST)
    {
        Avago_process_id_t process = AVAGO_UNKNOWN_PROCESS;
        for (addr_struct.chip = 0; addr_struct.chip < aapl_get_chips(aapl); addr_struct.chip ++)
        {
            Avago_process_id_t curr_process = aapl_get_process_id(aapl, avago_struct_to_addr(&addr_struct));
            if (addr_struct.chip > 0 && curr_process != process)
                return AVAGO_UNKNOWN_PROCESS;
            process = curr_process;
        }
        return process;
    }
    if( addr_struct.chip >= AAPL_MAX_CHIPS ) return AVAGO_UNKNOWN_PROCESS;
    if (aapl->process_id[addr_struct.chip] == AVAGO_UNKNOWN_PROCESS)
        aapl->process_id[addr_struct.chip] = get_process_id(aapl, addr_struct.chip);
    return aapl->process_id[addr_struct.chip];
}

BOOL aapl_get_spico_running_flag(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr, &addr_struct);
    if (aapl_check_broadcast_address(aapl, addr, __func__, __LINE__, FALSE))
    {
        BOOL st;
        Avago_addr_t start, stop, next;
        BOOL running = TRUE;
        for( st = aapl_broadcast_first(aapl, &addr_struct, &start, &stop, &next);
             st;
             st = aapl_broadcast_next(aapl,&next,&start,&stop) )
        {
            uint sbus_addr = avago_struct_to_addr(&next);
            if (aapl_get_ip_type(aapl, sbus_addr) != AVAGO_SERDES) continue;
            running &= aapl->spico_running[next.chip][next.ring][next.sbus];
        }
        return running;
    }
    else
    {
        if( addr_struct.chip >= AAPL_MAX_CHIPS ||
            addr_struct.ring >= AAPL_MAX_RINGS || addr_struct.sbus >  255 )
            return FALSE;
        return aapl->spico_running[addr_struct.chip][addr_struct.ring][addr_struct.sbus];
    }
}

BOOL aapl_set_spico_running_flag(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Address number. */
    BOOL running)   /**< [in] New running value */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS ||
        addr_struct.ring >= AAPL_MAX_RINGS || addr_struct.sbus >  255 )
        return FALSE;
    return aapl->spico_running[addr_struct.chip][addr_struct.ring][addr_struct.sbus] = running;
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief  Get a string representation of the Process ID of the specified chip */
/**         and sbus ring */
/** @return The Process ID string of the sbus device for the current chip and */
/**         sbus ring (specified by the sbus address) */
const char *aapl_get_process_id_str(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    return aapl_process_id_to_str(aapl_get_process_id(aapl, addr));
}

/** @brief Sends and receives JTAG information. */
/** @return Data received back from the JTAG command */
/**        Returns the aapl->data_char string pointer, which is a C-string holding the command status. */
/**        aapl->data is set to length of the returned string. */
char *avago_jtag(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    int bits,           /**< [in] The number of bits to be sent. */
    const char *data)   /**< [in] Data to be sent to scan chain. */
{
    int buf_size = bits+1+32;
    char *buffer = (char *)aapl_malloc(aapl, buf_size, __func__);

    snprintf(buffer, buf_size, "jtag %d %04x %s", bits, opcode, data);
    avago_aacs_send_command(aapl, buffer);

    aapl_free(aapl, buffer, __func__);
    return aapl->data_char;
}

/** @brief Read JTAG information. */
/** @return Data received back from the JTAG command */
/**        Returns the aapl->data_char string pointer, which is a C-string holding the command status. */
/**        aapl->data is set to length of the returned string. */
char *avago_jtag_rd(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    int bits)           /**< [in] The number of bits to be sent. */
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "jtag %d %04x read",bits,opcode);
    avago_aacs_send_command(aapl, buffer);
    return aapl->data_char;
}

/** @brief Sets single bit in JTAG scan chain */
void avago_jtag_set_bit(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    uint bits,          /**< [in] The number of bits in the scan chain */
    uint set_bit,       /**< [in] Bit number to set */
    uint value)         /**< [in] Value to set */
{
    avago_jtag_rd(aapl, opcode, bits);
    aapl->data_char[(bits-1) - set_bit] = value;
    avago_jtag(aapl, opcode, bits, aapl->data_char);
}
#endif /* MV_HWS_REDUCED_BUILD*/

/** @brief   Checks sbus_addr to see if it is a broadcast address (including the SerDes broadcast address) */
/** @details Checks to see if sbus_addr is a chip broadcast address (0xfXXX), */
/**          a ring broadcast address (0xXfXX), or the SerDes broadcast address (0xXXff) */
/** @return  TRUE if sbus_addr belongs to any of the above broadcast addresses */
#ifndef MV_HWS_REDUCED_BUILD
BOOL aapl_check_broadcast_address_full(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    const char *caller,     /**< Caller function, usually __func__. */
    int line,               /**< Caller line number, usually __LINE__. */
    int error_on_match)     /**< Whether to print error on match. */
#else
BOOL aapl_check_broadcast_address_reduce(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    int error_on_match)     /**< Whether to print error on match. */
#endif /* MV_HWS_REDUCED_BUILD */
{
    BOOL match;
    Avago_addr_t addr_struct;
    avago_addr_to_struct(sbus_addr,&addr_struct);

    match = (addr_struct.chip == AVAGO_BROADCAST)
         || (addr_struct.ring == AVAGO_BROADCAST)
         || (addr_struct.sbus == AVAGO_BROADCAST)
         || (addr_struct.sbus >= 0xef && addr_struct.sbus <= 0xfc);

    if( match && error_on_match )
        aapl_fail(aapl, caller, line, "Broadcast address (0x%02x) not supported.\n", sbus_addr);

    return match;
}


/** @brief   Checks sbus_addr against firmware revisions given in args list. */
/** @details Checks if the device firmware revision is >= what is in the list. */
/**          If the revision has bit 19 set (0x80000), it must match */
/**          exactly (not >=) after ignoring bit 19. */
/** <pre> */
/** Example: To check that the firmware revision == 0x1234 or >= 0x5678, */
/**          and error if not, call: */
/**       aapl_check_firmware_rev(aapl,sbus_addr,__func__,__LINE__,TRUE,2,0x81234,0x5678); */
/** </pre> */
/** @return  TRUE if the firmware revision matches one of those listed, FALSE if not. */
/** */
#ifndef MV_HWS_REDUCED_BUILD
BOOL aapl_check_firmware_rev_full(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    const char *caller,     /**< Caller function, usually __func__. */
    int line,               /**< Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< Whether to print error on no match. */
    int arg_count,          /**< The number of firmware revision arguments which follow. */
    ...)                    /**< List of valid firmware revisions. */
#else
BOOL aapl_check_firmware_rev_reduce(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    int error_on_no_match,  /**< Whether to print error on no match. */
    int arg_count,          /**< The number of firmware revision arguments which follow. */
    ...)                    /**< List of valid firmware revisions. */
#endif /* MV_HWS_REDUCED_BUILD */
{
    BOOL match = FALSE;
    int firmware_rev;
    int i;
    Avago_addr_t addr_struct;
    va_list firm_rev_type;
    avago_addr_to_struct(sbus_addr,&addr_struct);

    if (addr_struct.chip == AVAGO_BROADCAST) addr_struct.chip = 0;
    if (addr_struct.ring == AVAGO_BROADCAST) addr_struct.ring = 0;
    if (addr_struct.sbus == AVAGO_BROADCAST) addr_struct.sbus = 0xff;
    if( addr_struct.chip >= AAPL_MAX_CHIPS ||
        addr_struct.ring >= AAPL_MAX_RINGS || addr_struct.sbus > 255 )
        return FALSE;
    sbus_addr = avago_struct_to_addr(&addr_struct);
    firmware_rev = aapl_get_firmware_rev(aapl, sbus_addr) & 0xFFF;

    va_start(firm_rev_type, arg_count);
    for( i=0; i < arg_count; i++ )
    {
        int firm_rev_check = va_arg(firm_rev_type, int);

        if( (!(firm_rev_check & 0x80000) && (firmware_rev >= (firm_rev_check & 0x0fff))) ||
                                            (firmware_rev == (firm_rev_check & 0x0fff)) )
        {
             match = 1;
             break;
        }
    }

    va_end(firm_rev_type);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "Unsupported firmware rev 0x%04X on SBus address %s.\n",
            aapl_get_firmware_rev(aapl, sbus_addr), aapl_addr_to_str(sbus_addr));

    return match;
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief   Checks sbus_addr against firmware build given in args list. */
/** @details Checks if the device firmware build is >= what is in the list. */
/**          If the build has bit 19 set (0x80000), it must match */
/**          exactly (not >=) after ignoring bit 19. */
/** <pre> */
/** Example: To check that the firmware build == 0x1234 or >= 0x5678, */
/**          and error if not, call: */
/**       aapl_check_firmware_build(aapl,sbus_addr,__func__,__LINE__,TRUE,2,0x81234,0x5678); */
/** </pre> */
/** @return  TRUE if the firmware build matches one of those listed, FALSE if not. */
/** */
BOOL aapl_check_firmware_build(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    const char *caller,     /**< Caller function, usually __func__. */
    int line,               /**< Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< Whether to print error on no match. */
    int arg_count,          /**< The number of firmware build arguments which follow. */
    ...)                    /**< List of valid firmware builds. */
{
    BOOL match = FALSE;
    int i;
    Avago_addr_t addr_struct;
    va_list firm_build_type;
    int firmware_build;
    avago_addr_to_struct(sbus_addr,&addr_struct);

    if (addr_struct.chip == AVAGO_BROADCAST) addr_struct.chip = 0;
    if (addr_struct.ring == AVAGO_BROADCAST) addr_struct.ring = 0;
    if (addr_struct.sbus == AVAGO_BROADCAST) addr_struct.sbus = 0xff;
    if( addr_struct.chip >= AAPL_MAX_CHIPS ||
        addr_struct.ring >= AAPL_MAX_RINGS || addr_struct.sbus > 255 )
        return FALSE;
    sbus_addr = avago_struct_to_addr(&addr_struct);
    firmware_build = aapl_get_firmware_build(aapl, sbus_addr);

    va_start(firm_build_type, arg_count);
    for( i=0; i < arg_count; i++ )
    {
        int firm_build_check = va_arg(firm_build_type, int);

        if( (!(firm_build_check & 0x80000) && (firmware_build >= (firm_build_check & 0x0fff))) ||
                                              (firmware_build == (firm_build_check & 0x0fff)) )
        {
             match = 1;
             break;
        }
    }

    va_end(firm_build_type);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "SBus address %s is running firmware build 0x%x, which is not supported by %s.\n",
            aapl_addr_to_str(sbus_addr), aapl_get_firmware_build(aapl, sbus_addr), caller);

    return match;
}
#endif /* MV_HWS_REDUCED_BUILD*/


/** @brief   Checks sbus_addr against list of supported IP types. */
/** @details Args is the number of arguments passed in for ip_type or process\n */
/**          for example: aapl_check_ip_type(aapl, sbus_addr, __FUNC__, __line__, TRUE, 2, AVAGO_SERDES, AVAGO_QPI); */
/** @return  TRUE if the device is one of the listed types, FALSE if not. */
/** */
#ifndef MV_HWS_REDUCED_BUILD
BOOL aapl_check_ip_type_full(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    const char *caller,     /**< Caller function, usually __func__. */
    int line,               /**< Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< Whether to print error on no match. */
    int arg_count,          /**< The number of ip_type arguments which follow. */
    ...)                    /**< List of valid IP types. */
#else
BOOL aapl_check_ip_type_reduce(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    int error_on_no_match,  /**< Whether to print error on no match. */
    int arg_count,          /**< The number of ip_type arguments which follow. */
    ...)                    /**< List of valid IP types. */
#endif /* MV_HWS_REDUCED_BUILD */
{
    BOOL match = FALSE;
    Avago_addr_t addr_struct;
    int i;
    va_list ip_types;

    va_start(ip_types, arg_count);

    avago_addr_to_struct(sbus_addr,&addr_struct);
    for( i=0; i < arg_count; i++ )
    {
        Avago_ip_type_t ip_type = (Avago_ip_type_t)va_arg(ip_types, int);
        if( ip_type == AVAGO_SERDES_BROADCAST &&
            (   addr_struct.chip == AVAGO_BROADCAST
             || addr_struct.ring == AVAGO_BROADCAST
             || addr_struct.sbus == AVAGO_BROADCAST)
             && addr_struct.sbus != 0xfd)
        {
            match = TRUE;
            break;
        }
        if( ip_type == AVAGO_SPICO_BROADCAST &&
            (   addr_struct.chip == AVAGO_BROADCAST || addr_struct.ring == AVAGO_BROADCAST )
             && addr_struct.sbus == 0xfd)
        {
            match = TRUE;
            break;
        }
        if( addr_struct.chip < AAPL_MAX_CHIPS &&
            addr_struct.ring < AAPL_MAX_RINGS &&
            addr_struct.sbus < 255 &&
            aapl_get_ip_type(aapl, sbus_addr) == ip_type )
        {
            match = TRUE;
            break;
        }
    }
    va_end(ip_types);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "SBus %s, of IP type 0x%x, in process %s, is not supported by %s.\n",
            aapl_addr_to_str(sbus_addr),
            aapl_get_ip_type(aapl,sbus_addr),
            aapl_get_process_id_str(aapl,sbus_addr),
            caller);

    return match;
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief   Checks sbus_addr against list of supported IP types. */
/** @details Args is the number of arguments passed in for ip_type or process\n */
/**          for example: aapl_check_ip_type(aapl, sbus_addr, __FUNC__, __line__, TRUE, 2, AVAGO_SERDES, AVAGO_QPI); */
/** @return  TRUE if the device is one of the listed types, FALSE if not. */
/** */
BOOL aapl_check_ip_type_exists(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus address to provide chip & ring to search. */
    const char *caller,     /**< Caller function, usually __func__. */
    int line,               /**< Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< Whether to print error on no match. */
    int arg_count,          /**< The number of ip_type arguments which follow. */
    ...)                    /**< List of valid IP types. */
{
    BOOL match = TRUE;
    int i;
    Avago_addr_t addr_struct;
    va_list ip_types;
    va_start(ip_types, arg_count);

    avago_addr_to_struct(sbus_addr,&addr_struct);
    for( i=0; i < arg_count; i++ )
    {
        Avago_ip_type_t ip_type = (Avago_ip_type_t)va_arg(ip_types, int);
        Avago_addr_t    start, stop, next;
        BOOL            st;

        for( st = aapl_broadcast_first(aapl, &addr_struct, &start, &stop, &next);
             st;
             st = aapl_broadcast_next(aapl, &next, &start, &stop) )
        {
            if (aapl_get_ip_type(aapl, avago_struct_to_addr(&next)) == ip_type )
              break;
        }
        if(st == FALSE)
        {
            match = FALSE;
            break;
        }
    }
    va_end(ip_types);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "SBus %s, of IP type 0x%x, in process %s, is not supported by %s.\n",
            aapl_addr_to_str(sbus_addr),
            aapl_get_ip_type(aapl,sbus_addr),
            aapl_get_process_id_str(aapl,sbus_addr),
            caller);

    return match;
}


/** @brief   Check sbus_addr against list of supported process types. */
/** @details Example:  Call aapl_check_process(aapl, sbus_addr, __func__, __LINE__, TRUE, 1, AVAGO_PROCESS_F); */
/**          to check that the device at sbus_addr is of type AVAGO_PROCESS_F. */
/** @return  TRUE if the device is one of the listed types, FALSE if not. */
BOOL aapl_check_process_full(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    const char *caller,     /**< Caller function, usually __func__. */
    int line,               /**< Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< Whether to print error on no match. */
    int arg_count,          /**< The number of process_type arguments which follow. */
    ...)                    /**< List of valid processes. */
{
    BOOL match = FALSE;
    int i;
    va_list processes;
    Avago_addr_t addr_struct;
    Avago_process_id_t process_id;

    avago_addr_to_struct(sbus_addr,&addr_struct);

    if (addr_struct.chip == AVAGO_BROADCAST) addr_struct.chip = 0;
    if (addr_struct.chip >= AAPL_MAX_CHIPS ) return 0;

    sbus_addr = avago_struct_to_addr(&addr_struct);
    process_id = aapl_get_process_id(aapl, sbus_addr);

    va_start(processes, arg_count);
    for( i=0; i < arg_count; i++ )
        if( process_id == (Avago_process_id_t)va_arg(processes, int) )
        {
            match = TRUE;
            break;
        }

    va_end(processes);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "SBus %s, IP type 0x%x, in process %s, is not supported by %s.\n",
            aapl_addr_to_str(sbus_addr),
            aapl_get_ip_type(aapl,sbus_addr),
            aapl_get_process_id_str(aapl,sbus_addr),
            caller);

    return match;
}
#endif /* MV_HWS_REDUCED_BUILD */

#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER

#ifndef MV_HWS_REDUCED_BUILD
/** @brief Internal function that returns the tap_gen from an HS1 */
uint avago_get_tap_gen(Aapl_t *aapl)
{
    uint tap_gen = 0;

    avago_aacs_send_command(aapl, "version");

    if(strstr(aapl->data_char, "AAPL AACS server")) avago_aacs_send_command(aapl, "send status");
    else                                            avago_aacs_send_command(aapl, "status");
    aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "The status command returned: \"%s\".\n", aapl->data_char);

    if( strlen(aapl->data_char) >= 1 )
    {
        char *string = strstr(aapl->data_char, "TAP generation:");
        if( string )
        {
            string += 15;
            tap_gen = strtoul(string, NULL, 10);
        }
        else aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Can not determine tap gen. Received: %s.\n", aapl->data_char);
        if (strstr(aapl->data_char, ";;;;;;;;;;"))
        {
            uint supp = aapl->suppress_errors;
            if (supp) aapl->suppress_errors -= 1;
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Invalid response received from HS1 (%s). This may be caused by an unsupported version of HS1 firmware. This may cause some issues, including the inability to reset the device.\n", aapl->data_char );
            if (supp) aapl->suppress_errors += 1;
        }
    }

    aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "TAP gen: %d\n", tap_gen);

    return tap_gen;
}

static void set_hs1_reset_override(Aapl_t *aapl, uint chip, uint override)
{
    if(!aapl_is_aacs_communication_method(aapl)) return;

    aapl_log_printf(aapl, AVAGO_DEBUG3, __func__, __LINE__, "HS1 reset override set to: %d.\n", override);
    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    {
        uint tap_gen = avago_get_tap_gen(aapl);
        char hs1_mode[64];
        char set_sbus[32];
        uint set_sbus_bit = 49;
        Aapl_comm_method_t comm_method;

        snprintf (hs1_mode, 64, "sbus_mode %s", avago_aacs_send_command(aapl, "sbus_mode"));
        {
            char *ptr = strstr(hs1_mode," I2C");
            if( ptr )
                *ptr = '\0';
        }
        comm_method = aapl->communication_method;
        aapl->communication_method = AVAGO_AACS_SBUS;
        avago_aacs_send_command(aapl, "sbus_mode jtag");

        if      (tap_gen == 1) set_sbus_bit = 47;
        else if (tap_gen == 2) set_sbus_bit = 48;
        else if (tap_gen == 3) set_sbus_bit = 49;
        else if (tap_gen == 4) set_sbus_bit = 48;

        snprintf(set_sbus, 32, "set_sbus %u %d", set_sbus_bit, override);
        avago_aacs_send_command(aapl, set_sbus);

        if      (tap_gen == 1) set_sbus_bit = 46;
        else if (tap_gen == 2) set_sbus_bit = 46;
        else if (tap_gen == 3) set_sbus_bit = 47;
        else if (tap_gen == 4) set_sbus_bit = 46;
        snprintf(set_sbus, 32, "set_sbus %u %d", set_sbus_bit, 0);
        avago_aacs_send_command(aapl, set_sbus);

        avago_sbus_rd(aapl, avago_make_addr3(chip, 0, 0xfe), 0x2);

        avago_aacs_send_command(aapl, hs1_mode);
        aapl->communication_method = comm_method;

    } AAPL_SUPPRESS_ERRORS_POP(aapl);
}
#endif /* MV_HWS_REDUCED_BUILD */

/*============================================================================= */
/* SYSTEM CHIP RESET */
/** @brief Reset the chip */
/** @details <pre> */
/** Performs a JTAG reset (if using AACS) */
/** Performs a MDIO reset (if using MDIO) */
/** Performs any necessary JTAG setup steps (if using AACS). */
/** </pre> */
/** @return void */
/** */
void avago_system_chip_setup(
    Aapl_t *aapl,   /**< [in] Pointer to AAPL structure. */
    int reset,      /**< [in] If set, send the "reset" command to the chip. */
    int chip)       /**< [in] The chip number. */
{
    BOOL rw;
#ifndef MV_HWS_REDUCED_BUILD
    char chip_cmd_buffer[16];
    sprintf(chip_cmd_buffer, "chipnum %d",chip);
    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    avago_aacs_send_command(aapl, chip_cmd_buffer);
    AAPL_SUPPRESS_ERRORS_POP(aapl);

    if( 0 == strcmp("Aswan", aapl->chip_name[chip]) )
    {
        avago_jtag_set_bit(aapl, 0x02b4, 830, 80, '0');
        avago_jtag_set_bit(aapl, 0x02b4, 830, 425, '1');
    }

    if( 0 == strcmp("AVSP-521", aapl->chip_name[chip]) )
    {
        avago_jtag_set_bit(aapl, 0x02b4, 1293, 500, '1');
    }

    if (reset) set_hs1_reset_override(aapl, chip, 0);

    if(aapl_is_mdio_communication_method(aapl))
    {
        uint reset_val = avago_mdio_rd(aapl, chip, AVSP_DEVAD, AVAGO_MDIO_REG_RESET);

        aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Initial MDIO reset reg: 0x%0x\n", reset_val);

        reset_val &= 0x1;
        reset_val |= (reset_val << 1);

        AAPL_SUPPRESS_ERRORS_PUSH(aapl);
        rw = avago_diag_sbus_rw_test(aapl, avago_make_addr3(chip, 0, 0xfe), 1);
        AAPL_SUPPRESS_ERRORS_POP(aapl);

        if (rw == TRUE) reset_val ^= 3;
        else
        {
            int prev_timeout = aapl->sbus_mdio_timeout;

            avago_mdio_wr(aapl, chip, AVSP_DEVAD, AVAGO_MDIO_REG_RESET, 0x0100 | (reset_val ^ 3));
            aapl->sbus_mdio_timeout = 10000;

            avago_sbus_rd(aapl, avago_make_addr3(chip, 0, 0xfe), 0xfe);
            aapl->sbus_mdio_timeout = prev_timeout;

            aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Final MDIO reset reg: 0x%0x\n", 0x0100 | (reset_val ^ 3));
        }
        avago_mdio_wr(aapl, chip, AVSP_DEVAD, 32785, 0x0000);
    }
#endif /* MV_HWS_REDUCED_BUILD */

    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    rw = avago_diag_sbus_rw_test(aapl, avago_make_addr3(chip, 0, 0xfe), 1);
    AAPL_SUPPRESS_ERRORS_POP(aapl);
    if (rw == FALSE)
    {
#ifndef MV_HWS_REDUCED_BUILD
        set_hs1_reset_override(aapl, chip, 1);
        avago_diag_sbus_rw_test(aapl, avago_make_addr3(chip, 0, 0xfe), 1);
#endif /* MV_HWS_REDUCED_BUILD */
    }

    if (reset) avago_sbus_reset(aapl, avago_make_addr3(chip, 0, AVAGO_BROADCAST), 1);
}

/** @brief Internal function that returns chip name from asic_info.h based on the JTAG idcode. */
static void get_chip_name(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    int chip)       /**< [in] Chip number. */
{
    const char *jtag_string = 0;
    uint jtag_idcode = 0;
    uint ring;
    int at;

    aapl->data_char[0] = 0;

    #ifdef AAPL_CHIP_ID_OVERRIDE0
        if (chip == 0) jtag_string = AAPL_CHIP_ID_OVERRIDE0;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE0
        if (chip == 0) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE0;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE1
        if (chip == 1) jtag_string = AAPL_CHIP_ID_OVERRIDE1;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE1
        if (chip == 1) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE1;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE2
        if (chip == 2) jtag_string = AAPL_CHIP_ID_OVERRIDE2;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE2
        if (chip == 2) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE2;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE3
        if (chip == 3) jtag_string = AAPL_CHIP_ID_OVERRIDE3;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE3
        if (chip == 3) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE3;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE4
        if (chip == 4) jtag_string = AAPL_CHIP_ID_OVERRIDE4;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE4
        if (chip == 4) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE4;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE5
        if (chip == 5) jtag_string = AAPL_CHIP_ID_OVERRIDE5;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE5
        if (chip == 5) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE5;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE6
        if (chip == 6) jtag_string = AAPL_CHIP_ID_OVERRIDE6;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE6
        if (chip == 6) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE6;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE7
        if (chip == 7) jtag_string = AAPL_CHIP_ID_OVERRIDE7;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE7
        if (chip == 7) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE7;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE8
        if (chip == 8) jtag_string = AAPL_CHIP_ID_OVERRIDE8;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE8
        if (chip == 8) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE8;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE9
        if (chip == 9) jtag_string = AAPL_CHIP_ID_OVERRIDE9;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE9
        if (chip == 9) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE9;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE10
        if (chip == 10) jtag_string = AAPL_CHIP_ID_OVERRIDE10;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE10
        if (chip == 10) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE10;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE11
        if (chip == 11) jtag_string = AAPL_CHIP_ID_OVERRIDE11;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE11
        if (chip == 11) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE11;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE12
        if (chip == 12) jtag_string = AAPL_CHIP_ID_OVERRIDE12;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE12
        if (chip == 12) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE12;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE13
        if (chip == 13) jtag_string = AAPL_CHIP_ID_OVERRIDE13;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE13
        if (chip == 13) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE13;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE14
        if (chip == 14) jtag_string = AAPL_CHIP_ID_OVERRIDE14;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE14
        if (chip == 14) jtag_idcode = AAPL_CHIP_ID_HEX_OVERRIDE14;
    #endif

    if (aapl->jtag_idcode[chip]) jtag_idcode = aapl->jtag_idcode[chip];
#ifndef MV_HWS_BIN_HEADER
    if (jtag_idcode == 0 && jtag_string)
        jtag_idcode = strtoul(jtag_string, NULL, 2);
#else
    /* Clear warning from BIN HEADER */
    jtag_string = jtag_string;
#endif /* MV_HWS_BIN_HEADER */

    aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "User supplied IDCODE for device %d (from aapl->jtag_idcode[] or aapl.h defines): 0x%08x\n", chip, jtag_idcode);

    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    if (jtag_idcode == 0)
    {
        int rc = aapl->return_code;
#ifndef MV_HWS_BIN_HEADER
        char chip_cmd_buffer[16];
        snprintf(chip_cmd_buffer, sizeof(chip_cmd_buffer), "chipnum %d", chip);
        avago_aacs_send_command(aapl, chip_cmd_buffer);
        avago_aacs_send_command(aapl, "jtag 32 02b6 0");

        jtag_idcode = strtoul(aapl->data_char, NULL, 2);
        aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "IDCODE read from JTAG for device %d: 0x%08x\n", chip, jtag_idcode);
#endif /* MV_HWS_BIN_HEADER */
        if ((rc != aapl->return_code) || ( jtag_idcode == 0) || (jtag_idcode == 0xffffffff))
        {
            uint addr_80;
            uint cntl_addr = avago_make_addr3(chip, 0, 0xfe);

            addr_80 = avago_sbus_rd(aapl, cntl_addr, 0x80);

            jtag_idcode = 0;
            if ((addr_80 & 0x3) == 0x00)
            {
                jtag_idcode = (avago_sbus_rd(aapl, cntl_addr, 0x81) & 0xff) << 12;
                if ((avago_find_chip_index(jtag_idcode, 0xff << 12) >= 0) && jtag_idcode != 0)
                {
                    avago_sbus_rmw(aapl, cntl_addr, 0x80, 0x1, 0x3);
                    jtag_idcode |= (avago_sbus_rd(aapl, cntl_addr, 0x81) & 0xff) << 20;
                    avago_sbus_rmw(aapl, cntl_addr, 0x80, 0x2, 0x3);
                    jtag_idcode |= (avago_sbus_rd(aapl, cntl_addr, 0x81) & 0x0f) << 28;
                    avago_sbus_rmw(aapl, cntl_addr, 0x80, 0x0, 0x3);

                    jtag_idcode |= 0x57f;
                }
                else jtag_idcode = 0;

                aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "IDCODE read from SBus for device %d: 0x%08x\n", chip, jtag_idcode);
            }
        }
    }
    AAPL_SUPPRESS_ERRORS_POP(aapl);

    aapl->jtag_idcode[chip] = jtag_idcode;
#ifndef MV_HWS_BIN_HEADER
    if (aapl->jtag_idcode[chip] == 0x0 && !aapl_is_aacs_communication_method(aapl))
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "AAPL has not been configured to use AACS, and an override (AAPL_CHIP_ID_OVERRIDE, aapl->jtag_idcode[], or AAPL_CHIP_ID_HEX_OVERRIDE) has not been set for chip %d, which may cause AAPL to not function properly as it has no way of determining exactly what device it is communicating with.\n", chip);
#endif /* MV_HWS_BIN_HEADER */
    for( ring = 0; ring < AAPL_MAX_RINGS; ring++ )
        aapl->max_sbus_addr[chip][ring] = 0;

    if (aapl->sbus_rings)
    {
        for( ring = 0; ring < aapl->sbus_rings; ring++ ) aapl->max_sbus_addr[chip][ring] = 1;
    }

    at = avago_find_chip_index(aapl->jtag_idcode[chip], 0xffffffff);
    if( at >= 0 )
    {
        aapl->chip_name[chip] = avago_chip_id[at].name;
        aapl->chip_rev[chip]  = avago_chip_id[at].rev;

        if (!aapl->sbus_rings)
        {
            aapl->sbus_rings = avago_chip_id[at].sbus_rings;
            for( ring = 0; ring < avago_chip_id[at].sbus_rings; ring++ )
                aapl->max_sbus_addr[chip][avago_chip_id[at].sbus_ring_start + ring] = 1;
        }

        aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "IDCODE 0x%08x: %s %s\n", aapl->jtag_idcode[chip], aapl->chip_name[chip], aapl->chip_rev[chip]);
        return;
    }
    if (!aapl->sbus_rings)
    {
        aapl->sbus_rings = 1;
        aapl->max_sbus_addr[chip][0] = 1;
    }
    aapl->chip_name[chip] = "";
    aapl->chip_rev[chip] = "";
    aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "IDCODE 0x%08x: %s %s\n", aapl->jtag_idcode[chip], aapl->chip_name[chip], aapl->chip_rev[chip]);
}

/* Query the AACS server for the number of die connected to it. */
static int get_number_of_chips(
    Aapl_t *aapl)   /**< [in] Pointer to AAPL structure. */
{
    if (aapl->chips == 0)
    {
#if AAPL_ALLOW_AACS || AAPL_ALLOW_OFFLINE_SBUS
        if(aapl_is_aacs_communication_method(aapl) || aapl->communication_method == AVAGO_OFFLINE)
        {
            avago_aacs_send_command(aapl, "status");

            aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "The status command returned: \"%s\".\n", aapl->data_char);

            if( strlen(aapl->data_char) >= 1 )
            {
                const char *current_chip = "Current chip:";
                char *string = strstr(aapl->data_char,current_chip);
                if( string ) string = strstr(string,"..");
                if( string )
                {
                    string += 2;
                    aapl->chips = strtoul(string, NULL, 10);
                    aapl->chips += 1;
                }
                else
                    aapl->chips = 0;
            }
        }
        else
#endif
        {
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "AAPL is not using AACS (TCP) to communicate with devices and AAPL_NUMBER_OF_CHIPS_OVERRIDE has not been defined in aapl.h. Since AACS is the only method to auto-detect the number of devices present, AAPL is assuming there is 1 device.\n", 0);
            aapl->chips = 1;
        }
    }
    if (aapl->chips > AAPL_MAX_CHIPS)
    {
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "AAPL discovered %d devices using the AACS protocol, but AAPL is compiled to support a maximum of %d devices. Setting aapl->chips to %d.\n", aapl->chips, AAPL_MAX_CHIPS, AAPL_MAX_CHIPS);
        aapl->chips = AAPL_MAX_CHIPS;
    }

    aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "aapl->chips set to: %d\n", aapl->chips);
    return aapl->chips;
}


/** @brief Fills out the AAPL struct with information about all devices it can communicate with */
/** @return void */
void aapl_get_ip_info(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    int chip_reset) /**< [in] Reset the chip if non-zero. */
{
    int chip, chips;

    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "AAPL Version " AAPL_VERSION ", " AAPL_COPYRIGHT "\n", 0);
#ifndef MV_HWS_BIN_HEADER
    if( chip_reset ) avago_aacs_send_command(aapl, "reset");
    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    avago_aacs_send_command(aapl, "chip");
    AAPL_SUPPRESS_ERRORS_POP(aapl);
#endif /* MV_HWS_BIN_HEADER */

    chips = get_number_of_chips(aapl);
    for (chip=0; chip<chips; chip++)
    {
        int ring;
        get_chip_name(aapl, chip);
        avago_system_chip_setup(aapl, chip_reset, chip);
        if (aapl->jtag_idcode[chip] == 0) get_chip_name(aapl, chip);

        #ifdef AAPL_PROCESS_ID_OVERRIDE
            aapl->process_id[chip] = AAPL_PROCESS_ID_OVERRIDE;
        #else
            AAPL_SUPPRESS_ERRORS_PUSH(aapl);
            aapl->process_id[chip] = get_process_id(aapl, chip);
            AAPL_SUPPRESS_ERRORS_POP(aapl);
        #endif

        for( ring=0; ring < AAPL_MAX_RINGS; ring++ )
        {
            if( 0 != aapl->max_sbus_addr[chip][ring] )
            {
                uint data;
                int rc = aapl->return_code;
                Avago_addr_t addr_struct;
                addr_struct.chip = chip;
                addr_struct.ring = ring;
                addr_struct.sbus = AVAGO_SBUS_CONTROLLER_ADDRESS;
                addr_struct.lane = AVAGO_BROADCAST;

                AAPL_SUPPRESS_ERRORS_PUSH(aapl);
                data = avago_sbus_rd(aapl, avago_struct_to_addr(&addr_struct), 0x02);
                AAPL_SUPPRESS_ERRORS_POP(aapl);

                aapl->max_sbus_addr[chip][ring] =
                    (aapl->return_code < rc || data == 0)
                        ? 0
                        : data - 1;

                aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "Chip %d (%s %s), ring %d, SBus devices: %d\n", chip, aapl->chip_name[chip], aapl->chip_rev[chip], ring, aapl->max_sbus_addr[chip][ring]);

                if( aapl->max_sbus_addr[chip][ring] == 0 )
                {
                    aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Error ocurred while trying to determine the number of devices on chip %d, SBus ring %d. aapl->return_code: %d, 0xfe 0x02 0x02 0x00: %d.\n", chip, ring, rc, data);
                    break;
                }

                for( addr_struct.sbus = 1; addr_struct.sbus <= aapl->max_sbus_addr[chip][ring]; addr_struct.sbus++ )
                {
                    /* Bug fix in AAPL LGPL package
                       Fixed value of aapl->ip_type[chip][ring][addr_struct.sbus] ==0xFF
                       Does not detect all Avago SERDES interfaces
                    aapl->ip_type[chip][ring][addr_struct.sbus] = 0xff;*/
                    aapl_set_ip_type(aapl,avago_struct_to_addr(&addr_struct));
                    aapl->spico_running[chip][ring][addr_struct.sbus] = 0;
                }

                for (addr_struct.sbus=0xef; addr_struct.sbus<=0xff; addr_struct.sbus++)
                {
                    aapl->firm_rev     [chip][ring][addr_struct.sbus] = 0;
                    aapl->firm_build   [chip][ring][addr_struct.sbus] = 0;
                    aapl->ip_rev       [chip][ring][addr_struct.sbus] = 0xfffe;
                    aapl->spico_running[chip][ring][addr_struct.sbus] = 0;

                    if      (addr_struct.sbus == 0xef) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_THERMAL_SENSOR_BROADCAST;
                    else if (addr_struct.sbus == 0xf0) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_DDR_STOP_BROADCAST;
                    else if (addr_struct.sbus == 0xf1) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_DDR_CTC_BROADCAST;
                    else if (addr_struct.sbus == 0xf2) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_DDR_TRAIN_BROADCAST;
                    else if (addr_struct.sbus == 0xf3) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_DDR_DATA_BROADCAST;
                    else if (addr_struct.sbus == 0xf4) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_DDR_ADDRESS_BROADCAST;
                    else if (addr_struct.sbus == 0xf5) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_PMRO_BROADCAST;
                    else if (addr_struct.sbus == 0xf6) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_RESERVED_BROADCAST;
                    else if (addr_struct.sbus == 0xf7) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_PCIE_PCS_BROADCAST;
                    else if (addr_struct.sbus == 0xf8) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_PCS64B66B_BROADCAST;
                    else if (addr_struct.sbus == 0xf9) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_AUTO_NEGOTIATION_BROADCAST;
                    else if (addr_struct.sbus == 0xfb) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_FBD_BROADCAST;
                    else if (addr_struct.sbus == 0xfc) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_QPI_BROADCAST;
                    else if (addr_struct.sbus == 0xfd) aapl->ip_type[chip][ring][addr_struct.sbus] = 0xff;
                    else if (addr_struct.sbus == 0xfe) aapl_set_ip_type(aapl,avago_struct_to_addr(&addr_struct));
                    else if (addr_struct.sbus == 0xff) aapl->ip_type[chip][ring][addr_struct.sbus] = AVAGO_SERDES_BROADCAST;
                }
                avago_twi_wait_for_complete(aapl, avago_make_sbus_controller_addr(avago_struct_to_addr(&addr_struct)));
            }
        }
    }
    aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "End of get_ip_info.\n", 0);
}
#endif /* !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER */

/** @brief Sets the ip_type, ip_rev, spico_running, firm_rev, and lsb_rev fields of aapl. */
/** @param aapl Aapl_t struct */
/** @param sbus_addr sbus address of the sbus device, sbus ring, and chip */
/** @return void */
void aapl_set_ip_type(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint sbus_addr) /**< [in] SBus slice address. */
{
    uint data;
    uint jtag_idcode;
    Avago_addr_t addr_struct;
    if (aapl_check_broadcast_address(aapl, sbus_addr, __func__, __LINE__, FALSE)) return;

    avago_addr_to_struct(sbus_addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS ) addr_struct.chip = 0;
    if( addr_struct.ring >= AAPL_MAX_RINGS ) addr_struct.ring = 0;
    if( addr_struct.sbus >= 255            ) addr_struct.sbus = 255;
    aapl->ip_type      [AAPL_3D_ARRAY_ADDR(addr_struct)] = AVAGO_UNKNOWN_IP;
    aapl->ip_rev       [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xfffe;
    aapl->spico_running[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
    aapl->firm_rev     [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
    aapl->firm_build   [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
    aapl->lsb_rev      [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;

    data = avago_sbus_rd(aapl, sbus_addr, 0xff);
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    jtag_idcode = aapl->jtag_idcode[addr_struct.chip];
#else
    jtag_idcode = 0x0912557f;
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

    switch( data )
    {
    case 0x00:
        if(    jtag_idcode == 0x0912557f
            || jtag_idcode == 0x0954857f
            || jtag_idcode == 0x0954957f
            || jtag_idcode == 0x0972357f
            || jtag_idcode == 0x0975457f
            || jtag_idcode == 0x0901457f
            || jtag_idcode == 0x0912257f
            || jtag_idcode == 0x090b857f
            || jtag_idcode == 0x0911657f
            || jtag_idcode == 0x18e5657f
            )
            data = AVAGO_LINK_EMULATOR;
        break;

    case 0x08:
        if( jtag_idcode == 0x14211001 &&
            (addr_struct.sbus == 0x05 || addr_struct.sbus == 0x06)
            )
            data = AVAGO_MLD;
        break;

    case 0x0b:
        if( jtag_idcode == 0x14211001 && addr_struct.sbus == 0x1c )
            data = AVAGO_RAM_PMRO;
        break;

    case 0x11:
        if(    (jtag_idcode == 0x1911357f && addr_struct.sbus < 0x17)
            || (jtag_idcode == 0x090b957f && addr_struct.sbus < 0x16)
            || (jtag_idcode == 0x0911457f && addr_struct.sbus < 0x16)
            )
            data = AVAGO_AVSP_CONTROL_LOGIC;
        break;

    case 0x83:
        if( aapl_get_process_id(aapl, sbus_addr) == AVAGO_PROCESS_B )
            data = AVAGO_SLE;
        if( aapl_get_process_id(aapl, sbus_addr) == AVAGO_PROCESS_F )
            data = AVAGO_RAM_PMRO;
        break;

    case 0xc1:
        if( aapl_get_process_id(aapl, sbus_addr) == AVAGO_PROCESS_F )
            data = AVAGO_PANDORA_LSB;
        break;
    }

    if (data <= AVAGO_IP_TYPE_MAX || (data >= AVAGO_IP_TYPE_ALT_RANGE_LO && data <= AVAGO_IP_TYPE_ALT_RANGE_HI))
        aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)] = (Avago_ip_type_t) data;
    else
    {
        aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)] = AVAGO_UNKNOWN_IP;
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "IP type 0x%02x unrecognized for SBus address %s.\n", data, aapl_addr_to_str(sbus_addr));
    }

    switch (aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)])
    {
    case AVAGO_M4:
    case AVAGO_SERDES:
        switch (aapl_get_process_id(aapl, sbus_addr))
        {
        case AVAGO_PROCESS_B:
        case AVAGO_PROCESS_F:
            avago_spico_wait_for_upload(aapl, sbus_addr);
            avago_sbus_wr(aapl, sbus_addr, 0x01, 0x20000000);
            break;
#ifndef MV_HWS_REDUCED_BUILD
        case AVAGO_PROCESS_A:
#endif /* MV_HWS_REDUCED_BUILD */
        default:
            aapl->firm_rev  [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
            aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
            break;
        }
        break;
    case AVAGO_SPICO:
        switch (aapl_get_process_id(aapl, sbus_addr))
        {
        case AVAGO_PROCESS_B:
        case AVAGO_PROCESS_F:
            avago_spico_wait_for_upload(aapl, sbus_addr);
            aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xff;
            break;
#ifndef MV_HWS_REDUCED_BUILD
        case AVAGO_PROCESS_A:
            aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xff;
            break;
#endif /* MV_HWS_REDUCED_BUILD */
        default:
            aapl->firm_rev  [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
            aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
            aapl->ip_rev    [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
            aapl->lsb_rev   [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xff;
            break;
        }
        break;
    case AVAGO_SBUS_CONTROLLER:
        aapl->firm_rev  [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
        aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
        aapl->lsb_rev   [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xff;
        break;
    default:
        aapl->spico_running[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
        aapl->firm_rev     [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
        aapl->firm_build   [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
        aapl->lsb_rev      [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xff;
    }
}

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
/** @brief  Converts an address number into a printable string. */
/** @return A printable string is returned. */
const char *aapl_addr_to_str(
    uint addr)                  /**< [in] Address number. */
{
#   define NUM_BUFS (5)
    static int which = 0;
    static char num_bufs[NUM_BUFS][20];

    char *buf = num_bufs[which = (which + 1) % NUM_BUFS];
    char *ptr = buf;
    char *end = buf + sizeof(num_bufs[0]);
#if 1
    Avago_addr_t addr_struct;

    avago_addr_to_struct(addr,&addr_struct);

    if( addr_struct.chip > 0 )
    {
        if( addr_struct.chip == AVAGO_BROADCAST )
            ptr += snprintf(ptr,end-ptr,"*:");
        else
            ptr += snprintf(ptr,end-ptr,"%x:",addr_struct.chip);
    }

    if( addr_struct.ring > 0 )
    {
        if( addr_struct.ring == AVAGO_BROADCAST )
            ptr += snprintf(ptr,end-ptr,"*");
        else
            ptr += snprintf(ptr,end-ptr,"%x",addr_struct.ring);
    }

    if( addr_struct.sbus == AVAGO_BROADCAST )
        ptr += snprintf(ptr,end-ptr,":*");
    else
        snprintf(ptr,end-ptr,":%02x", addr_struct.sbus);

    if( addr_struct.lane != AVAGO_INVALID_ADDR )
        snprintf(ptr,end-ptr,".%x", addr_struct.lane);
#else
    snprintf(ptr,end-ptr,"0x%02x",addr);
#endif
    return buf;
}

/* ported from aapl 2.2.3 for AACS Server integration */
/** @brief  Converts a string representation of an address into a number.
** @return Returns TRUE on successful parsing, FALSE otherwise.
** @return If parsing fails, *addr is set to 0.
**/
BOOL aapl_str_to_addr(
    const char *str,    /**< [in]  String representation of address. */
    char **endptr,      /**< [out] Pointer to the end of the processed string. */
    uint *addr)         /**< [out] Address number. */
{
#if 1
    char *ptr = 0;
    Avago_addr_t addr_struct;
    avago_addr_init(&addr_struct);
    while (str[0] == ' ') str++;
    if( 0 == strchr(str,':') )
    {
        addr_struct.sbus = strtol(str,&ptr,0);
        if( addr_struct.sbus & ~0x00ff )
            return (*addr = 0), FALSE;
    }
    else
        addr_struct.sbus = strtol(str,&ptr,16);
    if( str == ptr && *ptr == '*' )
    {
        ptr++;
        addr_struct.sbus = AVAGO_ADDR_BROADCAST;
    }
    if( *ptr == ':' )
    {
        ptr++;
        addr_struct.ring = addr_struct.sbus;
        if( *ptr == '*' )
        {
            ptr++;
            addr_struct.sbus = AVAGO_ADDR_BROADCAST;
        }
        else
            addr_struct.sbus = strtol(ptr,&ptr,16);
        if( *ptr == ':' )
        {
            ptr++;
            addr_struct.chip = addr_struct.ring;
            addr_struct.ring = addr_struct.sbus;
            if( *ptr == '*' )
            {
                ptr++;
                addr_struct.sbus = AVAGO_ADDR_BROADCAST;
            }
            else
                addr_struct.sbus = strtol(ptr,&ptr,16);
       }
    }
    if( *ptr == '.' )
    {
        ptr++;
        if(      *ptr == '*'                ) addr_struct.lane = AVAGO_ADDR_QUAD_ALL;
        else if( *ptr == 'l' || *ptr == 'L' ) addr_struct.lane = AVAGO_ADDR_QUAD_LOW;
        else if( *ptr == 'h' || *ptr == 'H' ) addr_struct.lane = AVAGO_ADDR_QUAD_HIGH;
        else if( *ptr >= '0' && *ptr <= '7' ) addr_struct.lane = *ptr - '0';
        else                                  addr_struct.lane = AVAGO_ADDR_IGNORE_LANE;
        ptr++;
    }
    *addr = avago_struct_to_addr(&addr_struct);
    if (endptr) *endptr = ptr;
    return ptr > str;
#else
    char *ptr;
    *addr = strtol(str,&ptr,0);
    return ptr > str;
#endif
}

#endif /* MV_HWS_REDUCED_BUILD */

