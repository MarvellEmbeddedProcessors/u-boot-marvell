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

/* Internal library header file for AAPL (ASIC and ASSP Programming Layer). */

/** Doxygen File Header */
/** @file */
/** @brief Declarations for library functions. */

#ifndef AAPL_LIBRARY_H_
#define AAPL_LIBRARY_H_

/* Typedefs: */
/* */
/* Note:  BOOL is a macro, not a typedef "bool" with "false" and "true" values, */
/* to avoid reserved word conflicts for C++ compilations. */
/* Also TRUE and FALSE for Perl (swig) only come from */
/* /opt/ictools/64bit/perl/lib/5.8.9/x86_64-linux-thread-multi/CORE/handy.h. */

#define BOOL int
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#if ! HAVE_UINT
typedef unsigned int uint;
#endif

/* Define type for eye gather counts: */
#if defined __STDC_VERSION && __STDC_VERSION__ > 199409L
  typedef long long bigint;
#elif defined __MINGW_EXTENSION
  typedef __int64 bigint;
#elif defined _STDINT_H
  typedef int64_t bigint;
#else
  typedef long long bigint;
#endif

/* Determine number of elements in a static array: */
#define AAPL_ARRAY_LENGTH(a)   (int)(sizeof(a) / sizeof((a)[0]))
#ifndef MV_HWS_REDUCED_BUILD
/* Specify how to declare a 64 bit constant: */
#ifdef __INT64_C
#   define AAPL_CONST_INT64(x) __INT64_C(x)
#elif defined(__GNUC__) && !defined(__STDC_VERSION__)
#   define AAPL_CONST_INT64(x) ((bigint)x)
#elif defined(__GNUC__) && defined(WIN32)
#   define AAPL_CONST_INT64(x) (x ## LL)
#elif defined(WIN32)
#   define AAPL_CONST_INT64(x) (x ## i64)
#else
#   define AAPL_CONST_INT64(x) (x)
#endif
#endif /* MV_HWS_REDUCED_BUILD */

#ifdef AAPL_ENABLE_INTERNAL_FUNCTIONS

EXT void  ms_sleep(uint msec);
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
/* Converts data to a 32 character ASCII encoded binary str with optional underscores every 8 bits */
EXT char *aapl_hex_2_bin(char *str, uint data, int underscore_en, int bits);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/* Portability utility functions: */
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT int   aapl_strcasecmp(const char *s1, const char *s2);
EXT int   aapl_strncasecmp(const char *s1, const char *s2, size_t len);
EXT char *aapl_strdup(const char *string);
EXT void  aapl_str_rep(char *str, char search, char replace);
EXT char *aapl_strtok_r(char *str, const char *delim, char **saveptr);
EXT size_t aapl_local_strftime(char *buf, size_t max, const char *format);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
#endif

#endif
