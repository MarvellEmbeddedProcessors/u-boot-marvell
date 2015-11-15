/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* marvell_API_mapping.h
*
* DESCRIPTION:
*     API mappings from POSIX to hws*
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __marvell_API_mapping_H 
#define __marvell_API_mapping_H

#include <common/os/hwsEnv.h>

/* redefine alloc macroces */
#undef AAPL_MALLOC
#undef AAPL_REALLOC
#undef AAPL_FREE
#define AAPL_MALLOC(sz)      hwsMalloc(sz)         /**< AAPL uses this for malloc. */
#define AAPL_REALLOC(ptr,sz) hwsRealloc(ptr,sz)    /**< AAPL uses this for realloc. */
#define AAPL_FREE(ptr)       hwsFree(ptr)          /**< AAPL uses this for free. */


/* redefine stdio APIS */
#define sprintf              hwsSprintf
#define snprintf             hwsSnprintf
#define vsprintf             hwsVsprintf
#define vsnprintf            hwsVsnprintf

#ifdef CPSS_BLOB
/* redefine ctype macros */
#undef tolower
#undef isspace
#undef isdigit
#undef isxdigit

#define tolower(_c)  (((_c)>='A' && (_c)<='Z')?((_c)+('a'-'A')):(_c))
#define isspace(_c)  (((_c)==' '||(_c)=='\t'||(_c)=='\r'||(_c)=='\n')?1:0)
#define isdigit(_c)  (((_c)>='0' && (_c)<='9')?1:0)
#define isxdigit(_c)  ((((_c)>='0' && (_c)<='9')||((_c)>='a' && (_c)<='f')||((_c)>='A' && (_c)<='F'))?1:0)

#define MS_SLEEP    hwsDelayMs

/* redefine str* APIs */
#undef strcpy
#undef strlen
#undef strchr
#undef strncmp
#define strcpy      hwsStrCpy
#define strlen      hwsStrLen
#define strchr      hwsStrChr
#define strncmp     hwsStrNCmp

long hws_strtol(const char *nptr, char **endptr, int base);
unsigned long hws_strtoul(const char *nptr, char **endptr, int base);
#define strtol      hws_strtol
#define strtoul     hws_strtoul

#endif /*CPSS_BLOB*/

#endif /* __marvell_API_mapping_H */
