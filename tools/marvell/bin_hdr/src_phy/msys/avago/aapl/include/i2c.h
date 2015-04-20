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

/* i2c.h - Defines i2c related functions and constants */

/** Doxygen File Header */
/** @file */
/** @brief Declarations for i2c bus functions. */

#ifndef AVAGO_I2C_H_
#define AVAGO_I2C_H_

EXT int avago_i2c_sbus(
    Aapl_t *aapl,
    unsigned int sbus_addr,
    unsigned char reg_addr,
    unsigned char command,
    unsigned int sbus_data);

EXT int avago_i2c_open(Aapl_t *aapl);
EXT int avago_i2c_close(Aapl_t *aapl);

EXT int avago_i2c_write(
    Aapl_t *aapl,
    unsigned int dev_addr,
    unsigned int length,
    unsigned char *buffer);

EXT int avago_i2c_read(
    Aapl_t *aapl,
    unsigned int dev_addr,
    unsigned int length,
    unsigned char *buffer);

#  if AAPL_ALLOW_USER_SUPPLIED_I2C
    EXT int  user_supplied_i2c_open_function(Aapl_t *aapl);
    EXT int  user_supplied_i2c_close_function(Aapl_t *aapl);
    EXT uint user_supplied_i2c_write_function(
        Aapl_t *aapl,
        unsigned char device_addr,
        unsigned char length,
        unsigned char *buffer);

    EXT uint user_supplied_i2c_read_function(
        Aapl_t *aapl,
        unsigned char device_addr,
        unsigned char length,
        unsigned char *buffer);
#  endif

#endif
