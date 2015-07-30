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

/* Declarations for ASIC-specific values used in AAPL (ASIC and ASSP
 * Programming Layer).
 */

#ifndef AVAGO_ASIC_INFO_H_
#define AVAGO_ASIC_INFO_H_
#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
typedef struct
{
        uint jtag_idcode;
        const char *name;
        const char *rev;
        const Avago_process_id_t process_id;
        uint sbus_rings;
        uint sbus_ring_start;
} Avago_chip_id_t;

/* Only one compilation unit defines this global variable; others just get the
 * definition:
 */

Avago_chip_id_t avago_chip_id[] =
{
    /* NOTE: Add new entries near beginning of list, not at end. */

    {0x0912657f, "\x41\x56\x53\x50\x2d\x31\x31\x30\x34",                     "2",    AVAGO_PROCESS_F, 1, 0},
    {0x0912457f, "\x41\x56\x53\x50\x2d\x31\x31\x30\x34",                     "1",    AVAGO_PROCESS_F, 1, 0},
    {0x0968257f, "\x41\x56\x53\x50\x2d\x34\x34\x31\x32\x2f\x38\x38\x30\x31", "4",    AVAGO_PROCESS_F, 1, 0},
    {0x096d157f, "\x41\x56\x53\x50\x2d\x34\x34\x31\x32\x2f\x38\x38\x30\x31", "3",    AVAGO_PROCESS_F, 1, 0},
    {0x1954657f, "\x41\x56\x53\x50\x2d\x34\x34\x31\x32\x2f\x38\x38\x30\x31", "2",    AVAGO_PROCESS_F, 1, 0},
    {0x0954657f, "\x41\x56\x53\x50\x2d\x34\x34\x31\x32\x2f\x38\x38\x30\x31", "1",    AVAGO_PROCESS_F, 1, 0},
    {0x1973157f, "\x41\x56\x53\x50\x2d\x38\x38\x31\x32",                     "2",    AVAGO_PROCESS_F, 1, 0},
    {0x0973157f, "\x41\x56\x53\x50\x2d\x38\x38\x31\x32",                     "1",    AVAGO_PROCESS_F, 1, 0},
    {0x2964257f, "\x41\x56\x53\x50\x2d\x39\x31\x30\x34",                     "3",    AVAGO_PROCESS_F, 1, 0},
    {0x1964257f, "\x41\x56\x53\x50\x2d\x39\x31\x30\x34",                     "2",    AVAGO_PROCESS_F, 1, 0},
    {0x0964257f, "\x41\x56\x53\x50\x2d\x39\x31\x30\x34",                     "1",    AVAGO_PROCESS_F, 1, 0},


    {0x0911657f, "\x44\x65\x6e\x61\x6c\x69",                                 "Bx",   AVAGO_PROCESS_F, 1, 0},
    {0x090b857f, "\x44\x65\x6e\x61\x6c\x69",                                 "B25",  AVAGO_PROCESS_F, 1, 0},
    {0x0912257f, "\x44\x65\x6e\x61\x6c\x69",                                 "B15",  AVAGO_PROCESS_F, 1, 0},
    {0x0901457f, "\x44\x65\x6e\x61\x6c\x69",                                 "1",    AVAGO_PROCESS_F, 1, 0},
    {0x0954957f, "\x4d\x63\x4b\x69\x6e\x6c\x65\x79",                         "XTAL", AVAGO_PROCESS_F, 1, 0},
    {0x0954857f, "\x4d\x63\x4b\x69\x6e\x6c\x65\x79",                         "25",   AVAGO_PROCESS_F, 1, 0},
    {0x0912557f, "\x4d\x63\x4b\x69\x6e\x6c\x65\x79",                         "15",   AVAGO_PROCESS_F, 1, 0}

    /* NOTE: Add new entries at the front of this list, NOT here. */
};
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#endif /* AVAGO_ASIC_INFO_H_ */
