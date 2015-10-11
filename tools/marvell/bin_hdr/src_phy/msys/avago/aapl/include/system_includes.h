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

/*  *************************************** */
/*  ASIC and ASSP Programming Layer (AAPL) */
/*  Copyright 2013-2014, Avago Technologies */
/*  *************************************** */

/* System level header file for AAPL */

/** Doxygen File Header */
/** @file */
/** @brief System level declarations. */

#ifndef AVAGO_SYSTEM_INCLUDES_H_
#define AVAGO_SYSTEM_INCLUDES_H_


#ifdef MV_HWS_BIN_HEADER
#include "util.h"
#include "mv_os.h"
#endif


#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#ifdef HAVE_CONFIG_H
#   include <config.h>
#else
#   define STDC_HEADERS 1
#   ifndef ASIC_SIMULATION
#       define HAVE_UNISTD_H 1 /* Marvell - defined here instead of cpss gtBuild */
#   endif
#endif
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#define AAPL_COPYRIGHT "Copyright 2013-2014 Avago Technologies. All rights reserved."
#undef AAPL_VERSION
#ifndef AAPL_VERSION_OVERRIDE
#  define AAPL_VERSION "2.1.0"
#else
#  define AAPL_VERSION AAPL_VERSION_OVERRIDE
#endif

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#if !(defined HAVE_READLINE_READLINE_H && defined(HAVE_NCURSES_H))
#   undef AAPL_ENABLE_CONSOLE
#   define AAPL_ENABLE_CONSOLE 0
#endif


#ifdef _MSC_VER
/* If Microsoft C: */
#  define __func__ __FUNCTION__
#  define snprintf sprintf_s
#endif

/*#ifdef WIN32 */
/* #define DECL "C" __declspec(dllexport) */
/* #include <winsock2.h> */
/* #include <windows.h> */
/* BOOL APIENTRY DllMain2( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpAVAGO_RESERVED) { return TRUE; } */
/* */
#if (defined __MINGW32__ || defined _WIN32) && !(defined WIN32)
#    define WIN32
#endif

#ifdef WIN32
#    include <ws2tcpip.h>
#endif

#if defined(WIN32) && !defined(ASIC_SIMULATION)
#    define ASIC_SIMULATION
#endif

#ifdef MV_HWS_BIN_HEADER
#    ifndef ms_sleep
#        define ms_sleep(a) Sleep(a * 1000)
#    endif
#endif

#ifndef ASIC_SIMULATION
#if (defined __MINGW32__ || defined WIN32)
#    pragma warning( disable : 4996)
#    include <winsock2.h>
#    include <windows.h>
#    include <io.h>
#    ifndef sleep
#        define sleep(a) Sleep(a * 1000)
#    endif
#else
#   define closesocket(x) close(x)
#   if AAPL_ALLOW_AACS
#        include <sys/socket.h>
#        include <netinet/in.h>
#        include <netdb.h>
#   endif
#endif
#endif /* ASIC_SIMULATION */

#if defined(__cplusplus) && AAPL_ENABLE_C_LINKING
#   define EXT extern "C"
#else
#   define EXT extern
#endif

#ifdef KERNEL_INCLUDES
#   include <linux/mm.h>
#   include <linux/slab.h>
#   include <linux/ctype.h>
#else
#   if STDC_HEADERS
#       include <ctype.h>
#       include <stdio.h>
#       include <stdarg.h>
#       include <errno.h>
#       include <time.h>
#       include <math.h>
#       include <stdlib.h>
#       include <string.h>
#   else
#     if HAVE_STDLIB_H
#       include <stdlib.h>
#     endif
#     if HAVE_STRING_H
#       include <string.h>
#     endif
#   endif
#   if HAVE_UNISTD_H && !defined __MINGW32__ && !defined ASIC_SIMULATION
#       include <unistd.h>
#   endif
#   if defined HAVE_SYS_TYPES_H && HAVE_SYS_TYPES_H
#       include <sys/types.h>
#   endif
#   if defined HAVE_STDTYPES_H && HAVE_STDTYPES_H
#       include <inttypes.h>
#   endif
#   if defined HAVE_STDINT_H && HAVE_STDINT_H
#       include <stdint.h>
#   endif
#endif

#else
#if defined(__cplusplus) && AAPL_ENABLE_C_LINKING
#   define EXT extern "C"
#else
#   define EXT extern
#endif

#include <stdarg.h>
#include <time.h>

#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#if !(defined(HAVE_FCNTL_H) && defined(HAVE_LINUX_I2C_DEV_H) && defined(HAVE_SYS_IOCTL_H))
#    undef AAPL_ALLOW_SYSTEM_I2C
#    define AAPL_ALLOW_SYSTEM_I2C 0
#endif

#if AAPL_ALLOW_SYSTEM_I2C
#    include <linux/i2c-dev.h>
#    include <fcntl.h>
#    include <sys/ioctl.h>
#endif

#ifdef HAVE_BCM2835_H
/* Include libraries specific to GPIO initialization */
/* As of March 2014, the bcm2835 library can be found here:  */
/*     http */
#    include <bcm2835.h>
#else
#    undef AAPL_ALLOW_GPIO_MDIO
#    define AAPL_ALLOW_GPIO_MDIO 0
#endif
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#endif
