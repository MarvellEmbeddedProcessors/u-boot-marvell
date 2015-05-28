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

/** Doxygen File Header */
/** @file */
/** @brief AAPL i2c implementation. */
/** @details This file contains AAPL i2c related function implementations. */

#include "aapl.h"

/** */
/** @name TWI commands */
/** */
#define NOP                 0x00
#define WRITE_SBUS_DEVICE   0x01
#define READ_SBUS_DEVICE    0x02
#define RESET_SBUS_DEVICE   0x03
#define CORE_SBUS_RESET     0x04
#define TWI_REGISTER_0      0x80
/** @} */

/** */
/** @name TWI slave device registers */
/** */
#define SBUS_DATA_REG_ADDR  0x00
#define SBUS_DEVICE_ADDR    0x01
#define SBUS_DATA_OUT_0     0x02
#define SBUS_DATA_OUT_1     0x03
#define SBUS_DATA_OUT_2     0x04
#define SBUS_DATA_OUT_3     0x05
#define SBUS_STATUS         0x06
#define SBUS_GP_OUT_0       0x07
#define SBUS_GP_OUT_1       0x08
#define SBUS_GP_OUT_2       0x09
#define SBUS_GP_OUT_3       0x0a
#define SBUS_DATA_IN_0      0x0b
#define SBUS_DATA_IN_1      0x0c
#define SBUS_DATA_IN_2      0x0d
#define SBUS_DATA_IN_3      0x0e
/** @} */

/** */
/** @name Utility macros/defines */
/** */
#define OFFSET_BYTE(d,o)        ((d>>(o*8))&0xFF)
#define TO_UPPER(x)             ((x>96) && (x<123) ? (x-32) : x)
#define HEX_TO_NUM(x)           ((x>47) && (x<58) ? (x-48) : (TO_UPPER(x)-55))
#define I2C_SBUS_HEADER_LEN     3
/** @} */

/** */
/** @brief   Read <i>length</i> number of bytes from a specified I2C device. */
/** @details Read a variable number of bytes from a I2C device, blocking until */
/** the requested number of bytes are read from the device. */
/** */
/** @return  The number of bytes read (> 0) on success, -1 on failure. */
int avago_i2c_read(
    Aapl_t *aapl,             /**< [in/out] Pointer to AAPL structure. */
    unsigned int dev_addr,    /**< [in] I2C address to read from. */
    unsigned int length,      /**< [in] Number of bytes to read into buffer. */
    unsigned char *buffer)    /**< [out] Pointer to the buffer to store bytes read. */
{
#if AAPL_ALLOW_AACS
    if( aapl->communication_method == AVAGO_AACS_I2C )
    {
        char commandstr[64];

        snprintf(commandstr, 64, "i2c r %x %x",  dev_addr, length);

        avago_aacs_send_command_options(aapl, commandstr, /* recv_data_back */ 1, /* strtol = */ 0);

        if( aapl->data > 0 )
        {
            char *pData = aapl->data_char;
            unsigned int i = 0;
            while (*pData != '\0' && i < length)
            {
                buffer[i++] = (HEX_TO_NUM(pData[0])<<4) | HEX_TO_NUM(pData[1]);
                pData += 2;
                while( *pData == ' ' )
                    pData++;
            }
        }

        aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "%s -> 0x%08x\n", commandstr, aapl->data);
        return aapl->data;
    }
#endif

#if AAPL_ALLOW_SYSTEM_I2C
#  ifndef I2C_SLAVE
#    define I2C_SLAVE 0
#  endif
    if( aapl->communication_method == AVAGO_SYSTEM_I2C )
    {
        int read_len = -1;
        if( ioctl(aapl->socket, I2C_SLAVE, dev_addr) < 0 )
            return aapl_fail(aapl,__func__,__LINE__,"ioctl error: %s, addr 0x%x, read\n", strerror(errno), dev_addr);

        if( (read_len = read(aapl->socket, buffer, length)) != (int)length )
            return aapl_fail(aapl,__func__,__LINE__,"i2c read(dev_addr=0x%x,length=%u) failed: returned %d (%s).\n",
                dev_addr, length, read_len, strerror(errno));

        if( aapl->debug >= AVAGO_DEBUG8 )
        {
            int i;
            char data_str[64] = "";
            char *ptr = data_str;
            ptr += sprintf(ptr, "%02x", buffer[0]);
            for( i = 1; i < read_len && ptr <= data_str + sizeof(data_str) - 8; i++ )
                ptr += sprintf(ptr, " %02x", buffer[i]);
            if( i < read_len )
                sprintf(ptr," ...");
            aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "i2c read of %d bytes from 0x%x --> %s\n", length, dev_addr, data_str);
        }
        return read_len;
    }
#endif

#if AAPL_ALLOW_USER_SUPPLIED_I2C
    if( aapl->communication_method == AVAGO_USER_SUPPLIED_I2C )
    {
        uint read_len = user_supplied_i2c_read_function(aapl, dev_addr, length, buffer);
        if( read_len != length )
            return aapl_fail(aapl,__func__,__LINE__,"user_suppled_i2c_read_function(dev_addr=0x%x,length=%u) returned %u (%s).\n",
                dev_addr, length, read_len, strerror(errno));

        if( aapl->debug >= AVAGO_DEBUG8 )
        {
            uint i;
            char data_str[64] = "";
            char *ptr = data_str;
            ptr += sprintf(ptr, "%02x", buffer[0]);
            for( i = 1; i < read_len && ptr <= data_str + sizeof(data_str) - 8; i++ )
                ptr += sprintf(ptr, " %02x", buffer[i]);
            if( i < read_len )
                sprintf(ptr," ...");
            aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "i2c read of %d bytes from 0x%x --> %s\n", length, dev_addr, data_str);
        }
        return read_len;
    }
#endif
    (void)dev_addr;
    (void)length;
    (void)*buffer;

    return aapl_fail(aapl,__func__,__LINE__,"Implementation missing.\n",0);
}

/** */
/** @brief   Write <i>length</i> number of bytes to a specified I2C device. */
/** @details Writes variable number of bytes to a I2C device. Will block till */
/** the requested number of bytes are written to the device. */
/** */
/** @return  length (>= 0) on success, -1 on failure. */
int avago_i2c_write(
    Aapl_t *aapl,            /**< [in/out] Pointer to AAPL structure. */
    unsigned int dev_addr,   /**< [in] I2C address to write to. */
    unsigned int length,     /**< [in] Number of bytes to write from buffer. */
    unsigned char *buffer)   /**< [in] Pointer to the buffer containing bytes to be written */
                             /**<      to the device. */
{
    int write_len = -1;
    unsigned int i;
    unsigned int alloc_len = length * 3 + 10;
    char buf[500];
    char *commandstr = (alloc_len <= sizeof(buf)) ? buf : (char *)aapl_malloc(aapl, alloc_len, __func__);
    char *ptr = commandstr;

    ptr += snprintf(ptr, alloc_len, "i2c w %x", dev_addr);
    for( i = 0; i < length; i++ )
    {
        ptr += snprintf(ptr, alloc_len - (ptr - commandstr), " %02x", buffer[i]);
    }

#if AAPL_ALLOW_AACS
    if( aapl->communication_method == AVAGO_AACS_I2C )
    {
        const char *result = avago_aacs_send_command_options(aapl, commandstr, /* recv_data_back */ 1, /* strtol = */ 0);
        const char *ptr;
        if( 0 == strncmp(result, "Address 0x", 10) && 0 != (ptr = strrchr(result,':')) )
            write_len = strtol(ptr+1,0,0);
    }
    else
#endif

#if AAPL_ALLOW_SYSTEM_I2C
    if( aapl->communication_method == AVAGO_SYSTEM_I2C )
    {
        if( ioctl(aapl->socket, I2C_SLAVE, dev_addr) < 0 )
            aapl_fail(aapl,__func__,__LINE__,"ioctl error: %s, addr 0x%x, command %s\n", strerror(errno), dev_addr, commandstr);
        else if( (write_len = write(aapl->socket, buffer, length)) != (int)length )
        {
            aapl_fail(aapl,__func__,__LINE__,"i2c write failed (%s): %u bytes to addr 0x%x. Data: %s -> %d\n",
                        strerror(errno), length, dev_addr, commandstr, write_len);
        }
    }
    else
#endif

#if AAPL_ALLOW_USER_SUPPLIED_I2C
    if( aapl->communication_method == AVAGO_USER_SUPPLIED_I2C )
    {
        write_len = user_supplied_i2c_write_function(aapl, dev_addr, length, buffer);
    }
    else
#endif

        aapl_fail(aapl,__func__,__LINE__,"%s implementation missing for communication method %s.\n",
                            __func__, aapl_comm_method_to_str(aapl->communication_method));

    if( write_len != -1 )
        aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "i2c write of %d bytes to addr 0x%x. Data: %s -> %d\n", length, dev_addr, commandstr, write_len);

    if( buf != commandstr )
        aapl_free(aapl, commandstr, __func__);

    return write_len;
}

/** @brief  Initializes i2c access. */
/** @return On success, returns 0. */
/** @return On error, decrements aapl->return_code and returns -1. */
int avago_i2c_open(Aapl_t *aapl)
{
    int rc;
#if AAPL_ALLOW_USER_SUPPLIED_I2C
    if( aapl->communication_method == AVAGO_USER_SUPPLIED_I2C )
    {
        rc = user_supplied_i2c_open_function(aapl);
    }
    else
#endif
#if AAPL_ALLOW_SYSTEM_I2C
    if( aapl->communication_method == AVAGO_SYSTEM_I2C )
    {
        const char *device = "/dev/i2c-1";
        aapl->socket = open(device, O_RDWR);
        aapl_log_printf(aapl,AVAGO_DEBUG2,__func__,__LINE__,"open(%s,O_RDWR) = %d\n",device,aapl->socket);

        if( aapl->socket < 0 )
            rc = aapl_fail(aapl,__func__,__LINE__,"Error opening file: %s\n", strerror(errno));
        else
        {
            struct flock lockinfo;
            lockinfo.l_type = F_WRLCK;
            lockinfo.l_whence = SEEK_SET;
            lockinfo.l_start  = 0;
            lockinfo.l_len    = 0;
            if( 0 != fcntl(aapl->socket, F_SETLKW, &lockinfo) )
                aapl_log_printf(aapl,AVAGO_WARNING,__func__,__LINE__,"Lock on %s failed: %s\n", device, strerror(errno));
            rc = 0;
        }
    }
    else
#endif
        rc = 0;
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "open status = %d\n", rc);
    return rc;
}

/** @brief  Terminates i2c access. */
/** @return On success, returns 0. */
/** @return On error, decrements aapl->return_code and returns -1. */
int avago_i2c_close(Aapl_t *aapl)
{
    int rc;
#if AAPL_ALLOW_USER_SUPPLIED_I2C
    if( aapl->communication_method == AVAGO_USER_SUPPLIED_I2C )
    {
        rc = user_supplied_i2c_close_function(aapl);
    }
    else
#endif
#if AAPL_ALLOW_SYSTEM_I2C
    if( aapl->communication_method == AVAGO_SYSTEM_I2C )
    {
        if( aapl->socket >= 0 )
        {
            rc = close(aapl->socket);
            if( rc != 0 )
                aapl_log_printf(aapl,AVAGO_WARNING,__func__,__LINE__,"close(%d) failed: %s\n",aapl->socket,strerror(errno));
            aapl->socket = -1;
        }
        else
            rc = 0;
    }
    else
#endif
        rc = 0;
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "open status = %d\n", rc);
    return rc;
}


/** */
/** @brief   Send sbus commands to the specific I2C device. */
/** @details The supported sbus commands are <b>reset</b>, <b>read</b> and <b>write</b>. */
/** The maximum amount of data this function reads from a device is 32 bits. */
/** */
/** @return  Data received from device. */
int avago_i2c_sbus(
    Aapl_t *aapl,           /**< [in/out] Pointer to AAPL structure */
    unsigned int sbus_addr, /**< [in] Address of the sbus device */
    unsigned char reg_addr, /**< [in] Address of the register */
    unsigned char command,  /**< [in] Type of command */
    unsigned int sbus_data) /**< [in] Data to write for a <b>write</b> command */
{
    unsigned char buffer[8] = {0};
    unsigned int i2c_address = aapl->i2c_base_addr;
    int rc;

    Avago_addr_t addr_struct;
    avago_addr_to_struct(sbus_addr,&addr_struct);
    i2c_address += addr_struct.chip;

    if( command == 0 )
    {
        buffer[0] = RESET_SBUS_DEVICE;
        buffer[1] = 0;
        buffer[2] = (unsigned char)addr_struct.sbus;
        buffer[3] = 0;
        buffer[4] = 0;
        buffer[5] = 0;
        buffer[6] = 0;
        rc = avago_i2c_write(aapl, i2c_address, 7, buffer);
    }
    else if( command == 3 )
    {
        int rc_prev = aapl->return_code;
        int retry;

        buffer[0] = CORE_SBUS_RESET;
        buffer[1] = 0;
        buffer[2] = 0;
        buffer[3] = 0;
        buffer[4] = 0;
        buffer[5] = 0;
        buffer[6] = 0;
        rc = avago_i2c_write(aapl, i2c_address, 7, buffer);
        AAPL_SUPPRESS_ERRORS_PUSH(aapl);

        for (retry = 0; retry <= AAPL_I2C_HARD_RESET_TIMEOUT; retry ++)
            if (avago_diag_sbus_rw_test(aapl, avago_make_sbus_controller_addr(sbus_addr), 2)) break;
        aapl->return_code = rc_prev;
        AAPL_SUPPRESS_ERRORS_POP(aapl);
    }
    else if( command == 1 )
    {
        buffer[0] = WRITE_SBUS_DEVICE;
        buffer[1] = reg_addr;
        buffer[2] = (unsigned char)addr_struct.sbus;
        buffer[3] = OFFSET_BYTE(sbus_data,0);
        buffer[4] = OFFSET_BYTE(sbus_data,1);
        buffer[5] = OFFSET_BYTE(sbus_data,2);
        buffer[6] = OFFSET_BYTE(sbus_data,3);
        rc = avago_i2c_write(aapl, i2c_address, 7, buffer);
    }
    else
    {
        buffer[0] = READ_SBUS_DEVICE;
        buffer[1] = reg_addr;
        buffer[2] = (unsigned char)addr_struct.sbus;
        avago_i2c_write(aapl, i2c_address, 3, buffer);

        if( avago_i2c_read(aapl, i2c_address, 4, buffer) > 0 )
        {
            rc = (unsigned int)((buffer[3]<<24)|(buffer[2]<<16)|(buffer[1]<<8)|buffer[0]);
        }
        else
            rc = -1;
    }

    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "%s %02x %02x -> 0x%08x\n", aapl_addr_to_str(sbus_addr), reg_addr, command, rc);
    return rc;
}
