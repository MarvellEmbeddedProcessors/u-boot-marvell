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

/* AAPL (ASIC and ASSP Programming Layer) SBus-related functions defined in */
/* sbus.cpp. */

/** Doxygen File Header */
/** @file */
/** @brief Declarations for SBus functions. */

#ifndef AVAGO_SBUS_H_
#define AVAGO_SBUS_H_
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT int avago_sbus_open(Aapl_t *aapl);
EXT int avago_sbus_close(Aapl_t *aapl);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
EXT uint avago_sbus(     Aapl_t *aapl, uint sbus_addr, unsigned char reg, unsigned char cmd, uint data, int recv_data_back);
EXT uint avago_sbus_rd(  Aapl_t *aapl, uint sbus_addr, unsigned char reg);
EXT uint avago_sbus_wr(  Aapl_t *aapl, uint sbus_addr, unsigned char reg, uint data);
#ifndef MV_HWS_REDUCED_BUILD
EXT uint avago_sbus_wr_flush(Aapl_t *aapl, uint sbus_addr, unsigned char reg, uint data);
#endif /* MV_HWS_REDUCED_BUILD */
EXT uint avago_sbus_rmw( Aapl_t *aapl, uint sbus_addr, unsigned char reg, uint data, uint mask);
EXT void avago_sbus_reset(Aapl_t *aapl, uint sbus_addr, int hard);
EXT BOOL avago_diag_sbus_rw_test(Aapl_t *aapl, uint sbus_addr, int cycles);

EXT uint user_supplied_sbus_function(
        Aapl_t *aapl,
        unsigned int sbus_addr,
        unsigned char reg_addr,
        unsigned char command,
        unsigned int sbus_data,
        int recv_data_back);

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
# if AAPL_ALLOW_USER_SUPPLIED_SBUS
    EXT int user_supplied_sbus_open_function(Aapl_t *aapl);
    EXT int user_supplied_sbus_close_function(Aapl_t *aapl);
# endif
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

void user_supplied_sbus_soft_reset(Aapl_t *aapl);
EXT int avago_sbm_get_temperature(Aapl_t *aapl, uint sensor_addr, uint sensor);
#endif
