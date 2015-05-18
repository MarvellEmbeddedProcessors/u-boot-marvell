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

/* AAPL (ASIC and ASSP Programming Layer) MDIO-related functions defined in */
/* mdio.cpp. */

/** Doxygen File Header */
/** @file */
/** @brief Declarations for MDIO bus functions. */

#ifndef AVAGO_MDIO_H_
#define AVAGO_MDIO_H_

#define AVSP_DEVAD 8

#define AVAGO_MDIO_REG_STATUS      32777
#define AVAGO_MDIO_REG_STAT_MISC1  32787
#define AVAGO_MDIO_REG_RESET       32784
#define AVAGO_MDIO_REG_CONTROL     0
#define AVAGO_MDIO_REG_CONTROL2    7

typedef enum
{
    AVAGO_MDIO_ADDR,
    AVAGO_MDIO_WRITE,
    AVAGO_MDIO_READ,
    AVAGO_MDIO_WAIT
} Avago_mdio_cmd_t;

EXT int avago_mdio_open(Aapl_t *aapl);
EXT int avago_mdio_close(Aapl_t *aapl);
EXT uint avago_mdio(     Aapl_t *aapl, Avago_mdio_cmd_t mdio_cmd, uint port_addr, uint dev_addr, uint data);
EXT uint avago_mdio_rd(  Aapl_t *aapl, uint port_addr, uint dev_addr, uint reg_addr);
EXT uint avago_mdio_wr(  Aapl_t *aapl, uint port_addr, uint dev_addr, uint reg_addr, uint data);
EXT uint avago_mdio_sbus(Aapl_t *aapl, uint sbus_addr, unsigned char reg_addr, unsigned char command, uint sbus_data);


#  ifdef AAPL_ALLOW_USER_SUPPLIED_MDIO
    EXT int  user_supplied_mdio_open_function(Aapl_t *aapl);
    EXT int  user_supplied_mdio_close_function(Aapl_t *aapl);
    EXT uint user_supplied_mdio_function(
        Aapl_t *aapl,
        Avago_mdio_cmd_t mdio_cmd,
        uint port_addr,
        uint dev_addr,
        uint reg_addr,
        uint data);
#  endif

#endif
