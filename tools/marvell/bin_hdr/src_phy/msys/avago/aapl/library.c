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

/* Internal library API for AAPL (ASIC and ASSP Programming Layer). */

/** Doxygen File Header */
/** @file */
/** @brief Internal library functions. */

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"
#ifdef MV_HWS_FREE_RTOS
#include <hw.h>
#include <common/siliconIf/mvSiliconIf.h>
#endif /* MV_HWS_FREE_RTOS */
/** @defgroup Address IP Addressing Functions */
/** @{ */

/** @brief  Initializes an Avago_addr_t structure to default values. */
/** @return void */
void avago_addr_init(Avago_addr_t *addr_struct)
{
    addr_struct->chip = 0;
    addr_struct->ring = 0;
    addr_struct->sbus = 0;
    addr_struct->lane = AVAGO_INVALID_ADDR;
    addr_struct->next = 0;
}

/** @brief  Converts an address integer into an address structure. */
/** @return On success, returns TRUE. */
/** @return Returns FALSE if addr is not a recognized value. */
BOOL avago_addr_to_struct(
    uint addr,                  /**< [in]  Address value. */
    Avago_addr_t *addr_struct)  /**< [out] Hardware address structure. */
{
/* Implementation matching that of AAPL 1.x: */
    avago_addr_init(addr_struct);
    addr_struct->chip = (addr >> 12) & 0x0f;
    addr_struct->ring = (addr >>  8) & 0x0f;
    addr_struct->sbus =  addr        & 0xff;
    addr_struct->lane = AVAGO_INVALID_ADDR;
    if( addr_struct->chip == 0x0f ) addr_struct->chip = AVAGO_BROADCAST;
    if( addr_struct->ring == 0x0f ) addr_struct->ring = AVAGO_BROADCAST;
    if( addr_struct->sbus == 0xff ) addr_struct->sbus = AVAGO_BROADCAST;
    return (addr & 0xffff0000) == 0;
}

/** @brief  Converts hardware address structure into an address integer. */
/** @return The address value representation of the hardware address. */
uint avago_struct_to_addr(
    Avago_addr_t *addr_struct)  /**< [in] Hardware address structure. */
{
/* Implementation matching that of AAPL 1.x: */
    return (addr_struct->chip & 0x0f) << 12 |
           (addr_struct->ring & 0x0f) <<  8 |
           (addr_struct->sbus & 0xff) <<  0;
}

/** @brief  Combines the chip, ring and sbus values into an address integer. */
/** @return The address integer. */
uint avago_make_addr3(
    int chip,           /**< Chip number, range [0..15]. */
    int ring,           /**< Ring number, range [0..15]. */
    int sbus)           /**< SerDes number, range [0..255]. */
{
    Avago_addr_t addr_struct;
    avago_addr_init(&addr_struct);
    addr_struct.chip = chip;
    addr_struct.ring = ring;
    addr_struct.sbus = sbus;
    return avago_struct_to_addr(&addr_struct);
}

#ifndef MV_HWS_REDUCED_BUILD
/** @brief  Modifies the addr_struct passed in to be all broadcast addresses (chip, ring, SBus, etc) */
/** @return The address integer, and modifies the addr_struct passed in. */
uint avago_addr_init_broadcast(Avago_addr_t *addr_struct)
{
    avago_addr_init(addr_struct);
    addr_struct->chip = AVAGO_BROADCAST;
    addr_struct->ring = AVAGO_BROADCAST;
    addr_struct->sbus = AVAGO_BROADCAST;
    return avago_struct_to_addr(addr_struct);
}
#endif /* MV_HWS_REDUCED_BUILD */
#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
/** @brief  Returns the SBus Master address for the addressed chip and ring. */
/** @return The SBus Master address corresponding to given input address. */
uint avago_make_sbus_master_addr(
    uint addr)  /**< [in] Chip/ring address. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    addr_struct.sbus = AVAGO_SBUS_MASTER_ADDRESS;
    return avago_struct_to_addr(&addr_struct);
}
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/** @brief  Returns the SBus Controller address for the addressed chip and ring. */
/** @return The SBus Controller address corresponding to given input address. */
uint avago_make_sbus_controller_addr(
    uint addr)  /**< [in] Chip/ring address. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    addr_struct.sbus = AVAGO_SBUS_CONTROLLER_ADDRESS;
    return avago_struct_to_addr(&addr_struct);
}

/** @brief  Returns the SerDes Broadcast address for the addressed chip and ring. */
/** @return The SerDes Broadcast address for the addressed chip and ring. */
uint avago_make_serdes_broadcast_addr(
    uint addr)  /**< [in] Chip/ring address. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    addr_struct.sbus = AVAGO_SERDES_BROADCAST_ADDRESS;
    return avago_struct_to_addr(&addr_struct);
}

/** @brief   Begins traversal of addresses selected by sbus_addr. */
/** @details start, stop and first are updated based on any */
/**          chip, ring or sbus broadcast addresses. */
/**          start and stop should be passed unchanged to aapl_broadcast_next(), */
/**          and first should be passed as the addr field. */
/** @return  Returns TRUE and first is initialized to the first address. */
BOOL aapl_broadcast_first(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    Avago_addr_t *addr,     /**< [in] SBus slice (broadcast) address. */
    Avago_addr_t *start,    /**< [out] Start of hardware address range. */
    Avago_addr_t *stop,     /**< [out] End of hardware address range. */
    Avago_addr_t *first)    /**< [out] First hardware address. */
{
    avago_addr_init(first);
    avago_addr_init(stop);
    avago_addr_init(start);
    start->next = addr->next;
    first->chip = start->chip = addr->chip != AVAGO_BROADCAST ? addr->chip : 0;
    first->ring = start->ring = addr->ring != AVAGO_BROADCAST ? addr->ring : 0;
    first->sbus = start->sbus = addr->sbus != AVAGO_BROADCAST ? addr->sbus : 1;
    stop->chip  = addr->chip != AVAGO_BROADCAST ? addr->chip : (aapl->chips > 0) ? (aapl->chips - 1) : 0;
    stop->ring  = addr->ring != AVAGO_BROADCAST ? addr->ring : (AAPL_MAX_RINGS > 0) ? AAPL_MAX_RINGS - 1 : 0;
    stop->sbus  = addr->sbus != AVAGO_BROADCAST ? addr->sbus : 0xee;
    return TRUE;
}

/** @brief   Continues traversal of addresses between start and stop. */
/** @details addr must contain the previous address on input and is updated to the next address on output. */
/** @return  Updates addr and returns TRUE while traversing the selected addresses. */
/** @return  Returns FALSE when all addresses have been traversed. */
BOOL aapl_broadcast_next(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    Avago_addr_t *addr,     /**< [in,out] Previous/next hardware address. */
    Avago_addr_t *start,    /**< [in] Start of hardware address range. */
    Avago_addr_t *stop)     /**< [in] End of hardware address range. */
{
    if( ++(addr->sbus) > stop->sbus )
    {
        addr->sbus = start->sbus;
        if( ++(addr->ring) > stop->ring )
        {
            addr->ring = start->ring;
            if( ++(addr->chip) > stop->chip )
            {
                if( !start->next )
                    return FALSE;
                return aapl_broadcast_first(aapl, start->next, start, stop, addr);
            }
        }
    }
    return TRUE;
}

/** @} */

/** @details          Sleeps until a number of milliseconds has elapsed. */
/** @return           Void */
void ms_sleep(
    uint ms_delay)  /**< [in] The number of milliseconds to wait before returning. */
{
#ifdef MV_HWS_BIN_HEADER
    mvOsDelay(ms_delay);
#elif MS_SLEEP

    MS_SLEEP(ms_delay);

#elif defined __MINGW32__ || defined WIN32

    Sleep(ms_delay);

#elif defined __CYGWIN__

    usleep(ms_delay * 1000);

#elif defined MV_HWS_FREE_RTOS

    hwsOsExactDelayPtr(0, 0, ms_delay);

#elif defined unix

    extern int usleep(unsigned int);
    usleep(ms_delay * 1000);

#elif 0

    struct timespec req, rem;
    req.tv_sec = ms_delay / 1000;
    req.tv_nsec = (ms_delay % 1000) * 1000000;
    while( nanosleep(&req,&rem) != 0 && errno == EINTR )
    {
        req = rem;
    }

#elif defined(HWS_ENV_DEFINED)

    hwsDelayMs(ms_delay);

#else

	clock_t end = clock() + (ms_delay * CLOCKS_PER_SEC / 1000);

	while( clock() < end )
        continue;

#endif
}

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#ifndef aapl_malloc
/*============================================================================= */
/* A A P L   M A L L O C */
/* */
/** @details Wrapper around malloc() with standardized error handling. */
/**          Note:  Use aapl_free() to delete the memory, but remember */
/**          to clear the pointer to it afterward, if the pointer is reused. */
/** @return  Pointer to resulting memory (which is not initialized). */
/** @return  On error, decrements aapl->return_code and returns NULL. */
void *aapl_malloc(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    const size_t bytes,     /**< [in] Size to allocate. */
    const char *description)/**< [in] Description of object for error reporting. */
{
    void * memp = AAPL_MALLOC(bytes);
    if (memp) return(memp);

    aapl_fail(aapl, __func__, __LINE__, "Cannot malloc() %u bytes for %s.\n", bytes, description);
    return NULL;
}
/*============================================================================= */
/* A A P L   R E A L L O C */
/* */
/** @details Wrapper around realloc() with standardized error handling. */
/**          Note:  Use aapl_free() to delete the memory, but remember */
/**          to clear the pointer to it afterward, if the pointer is reused. */
/** @return  Pointer to resulting memory (which is not initialized) beyond the */
/**          original allocation. */
/** @return  On error, decrements aapl->return_code and returns NULL. */
void *aapl_realloc(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    void *ptr,              /**< [in] Pointer to memory reallocate to new bytes. */
    const size_t bytes,     /**< [in] Size of new space. */
    const char *description)/**< [in] Description of object for error reporting. */
{
    void *memp = AAPL_REALLOC(ptr,bytes);
    if (memp) return memp;

    aapl_fail(aapl, __func__, __LINE__, "Cannot realloc() %u bytes for %s.\n", bytes, description);
    return NULL;
}

/*============================================================================= */
/* A A P L   F R E E */
/* */
/** @details Wrapper around free() for abstraction and NULL ptr release checking. */
/** @return  void. On error, decrements aapl->return_code. */
void aapl_free(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    void *ptr,              /**< [in] Pointer to memory to free. */
    const char *description)/**< [in] Description of object for error reporting. */
{
    if( ptr )
        AAPL_FREE(ptr);
    else if( description )
        aapl_fail(aapl, __func__, __LINE__, "Free NULL ptr for %s\n", description);
    else
        aapl_fail(aapl, __func__, __LINE__, "Free NULL ptr for unknown.\n",0);
}
#endif

/** @cond INTERNAL */

/** @details Provide function where it is missing */
/** @return  Same as strncmp, except comparisons are case insensitive. */
int aapl_strncasecmp(const char *s1, const char *s2, size_t n)
{
    int left = 0, right = 0;
    size_t i;
    for( i = 0; i < n; i++ )
    {
        left = *s1++;
        right = *s2++;
        if( !left || !right )
            break;
        if( left == right )
            continue;

        left = tolower(left);
        right = tolower(right);
        if( left != right )
            break;
    }
    return left - right;
}

/** @details Provide function where it is missing */
/** @return  Same as strcmp, except comparisons are case insensitive. */
int aapl_strcasecmp(const char *s1, const char *s2)
{
    int left = 0, right = 0;
    for(;;)
    {
        left = *s1++;
        right = *s2++;
        if (!left || !right)
            break;
        if (left == right)
            continue;

        left = tolower(left);
        right = tolower(right);
        if (left != right)
            break;
    }
    return left - right;
}

/** @details Provide function where it is missing */
/** @details Same functionality as strdup. */
/** @return  Returns AAPL_MALLOC'd space on success.  The return value should be freed by calling AAPL_FREE(ret). */
/**          Returns NULL if an allocation error occurs. */
char *aapl_strdup(const char *string)
{
    char *buf = (char *)AAPL_MALLOC(strlen(string)+1);
    if( buf )
        return strcpy(buf,string);
    return NULL;
}

/** @details  Replaces all occurrences of the character \b search with the character \b replace. */
/** */
void aapl_str_rep(char *str, char search, char replace)
{
    char *ptr = str;
    while( 0 != (ptr = strchr(ptr, search)) )
        *ptr++ = replace;
}
#ifndef MV_HWS_REDUCED_BUILD

/** @details Provide function where it is missing */
/** @details Same functionality as strtok_r */
/** @return  Returns pointer to first token, and updates saveptr for next call. */
EXT char *aapl_strtok_r(char *str, const char *delim, char **saveptr)
{
    int len;
    if( !str ) str = *saveptr;
    str += strspn(str,delim);
    len = strcspn(str,delim);
    *saveptr = str + len;
    if( **saveptr != 0 )
        *(*saveptr)++ = '\0';
    return len ? str : 0;
}
#endif /* MV_HWS_REDUCED_BUILD */

/** @brief Converts data to a 32 character ASCII encoded binary string with optional */
/** underscores every 8 bits. */
/** */
/** @return str is returned; if bits requested is greater than 32, then NULL is returned. */

static const int kBitsPerByte = 8;
char *aapl_hex_2_bin(
        char *str,              /**< Output string, also returned on success */
        uint data,              /**< Data to convert. */
        int underscore_en,      /**< Non-zero to add underscores every 8 bits. */
        int bits)               /**< Number of bits to convert. */
{
    /* Only convert if the number of bits requested to be equal to or less
       than the width of the data variable. */
    if ( bits <= (int)(sizeof(data) * kBitsPerByte) ) {
        char *pDest = str;
        int offset = (kBitsPerByte*(bits%kBitsPerByte) - bits);
        int i;
        for (i = 1; i <= bits; ++i) {
            /* Check bit value MSB to LSB*/
            *pDest++ = ((data & (1 << (bits-i))) ? '1' :'0');

            /* Insert underscore at byte boundry when enabled. */
            if (underscore_en && (i != bits) && ((i+offset)%kBitsPerByte == 0)) {
                *pDest++ = '_';
            }
        }

        /* Insert end of string marker. */
        *pDest = '\0';

        return str;
    }
    return NULL;
}

/** @brief Works like strftime, but always returns local time information. */
/** @return The number of characters placed into buf, not including the */
/**         terminating null byte, or 0 if the results do not fit. */
size_t aapl_local_strftime(char *buf, size_t size_of_buf, const char *format)
{
#ifdef HWS_ENV_DEFINED
    return hwsStrfTime(buf, size_of_buf, format, hwsTime());
#else
    time_t t = time(0);
    struct tm *tm_info = localtime(&t);
    return strftime(buf, size_of_buf, format, tm_info);
#endif
}
#ifndef MV_HWS_REDUCED_BUILD

#if AAPL_ENABLE_FILE_IO
/** @details Reads the file contents into memory. */
/** @return  Allocated buffer with contents of file.  Call aapl_free() to */
/**          release the buffer after use. */
char *aapl_read_file(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    char *filename)         /**< [in] Name of file to read. */
{
    int size;
    char *data;
    FILE *file;
    file = fopen(filename, "r");

    if( !file )
    {
        aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Could not open file: %s\n",filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Reading %d bytes from file: %s\n", size, filename);

    data = (char *) aapl_malloc(aapl, sizeof(char) * size + 1, filename);
    if( !data )
    {
        fclose(file);
        return 0;
    }

    fread(data, 1, size, file);
    data[size] = 0;

    fclose(file);
    return data;
}
#endif
#endif /* MV_HWS_REDUCED_BUILD */
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/** @endcond */
