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

/* AAPL (ASIC and ASSP Programming Layer) Logging functions. */

/** Doxygen File Header */
/** @file */
/** @brief Logging and core type--string conversion functions. */

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"

/** @defgroup Logging Logging Functions and Types */
/** @{ */

/** @brief   Gets a pointer to the contents of the memory log. */
/** @details The memory log pointer is the aapl->log variable. */
/**          Any aapl function may invalidate this pointer */
/**          so it should be used immediately. */
/**          aapl_log_clear() will invalidate this value. */
/** @return  A pointer to the memory log. */
const char *aapl_log_get(
    Aapl_t *aapl)           /**< AAPL structure pointer. */
{
    return aapl->log;
}

/** @brief Clears AAPL's memory log. */
void aapl_log_clear(
    Aapl_t *aapl)           /**< AAPL structure pointer. */
{
    aapl->log_end = aapl->log;
    if( aapl->log )
        aapl->log[0] = '\0';
}

static void aapl_log_vprintf(
    Aapl_t *aapl,
    Aapl_log_type_t log_sel,
    const char *caller,
    int line,
    const char *fmt,
    va_list ap)
{
    char buf[AAPL_LOG_PRINTF_BUF_SIZE];

    int size = vsnprintf(buf, AAPL_LOG_PRINTF_BUF_SIZE, fmt, ap);
    aapl_log_add(aapl, log_sel, buf, caller, line);

    if( size >= (AAPL_LOG_PRINTF_BUF_SIZE - 1) )
    {
        aapl_log_printf(aapl, AVAGO_WARNING, 0, 0, "\n", 0);
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Call to aapl_log_printf from %s:%d (log_sel: %s) reached max buffer size (%d). Please increase this size.\n", caller, line, aapl_log_type_to_str(log_sel), AAPL_LOG_PRINTF_BUF_SIZE);
    }
}

/** @brief Prints the specified message and marks an error. */
/** */
/** @return aapl_fail() always returns -1 to indicate an error. */
int aapl_fail_full(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    const char *caller,     /**< Caller function, usually __func__ */
    int line,               /**< Caller line number, usually __LINE__ */
    const char *fmt,        /**< Format control string (printf format) */
    ...)                    /**< Format string parameters. */
{
    va_list ap;
    va_start(ap, fmt);
    aapl_log_vprintf(aapl, AVAGO_ERR, caller, line, fmt, ap);
    va_end(ap);

    if( aapl->return_code > 0 )
        aapl->return_code = 0;

    aapl->return_code -= 1;

    return -1;
}

/* returns 0 if the debug level was sufficient (ie the caller should return if 1) */
static int check_debug_level(
    Aapl_log_type_t log_sel,
    int debug_level)
{
    if( (int)log_sel > debug_level && log_sel < AVAGO_MEM_LOG )
        return 1;
    return 0;
}

#ifdef MV_HWS_AAPL_DEBUG_PRINT_ENABLE
/** @brief  Prints message to the specified log. */
/** @details If log_sel is one of the AVAGO_DEBUGn values, */
/**  logging only occurs if the aapl->debug field is greater than or equal */
/**  to the log_sel value. */
/** @return void */
void aapl_log_printf_reduce(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    Aapl_log_type_t log_sel,    /**< [in] Type of message logging. */
    const char *caller,         /**< [in] Caller's __func__ value, or 0. */
    int line,                   /**< [in] Caller's __LINE__ value, or 0. */
    const char *fmt,            /**< [in] printf format string. */
    ...)                        /**< [in] printf arguments. */
{
    va_list ap;
    if( check_debug_level(log_sel, aapl->debug) )
        return;

    va_start(ap, fmt);
    char buf[AAPL_LOG_PRINTF_BUF_SIZE];
    vsprintf(buf, fmt, ap);
    printf("%s", buf);
    va_end(ap);
}
#endif

/** @brief  Prints message to the specified log. */
/** @details If log_sel is one of the AVAGO_DEBUGn values, */
/**  logging only occurs if the aapl->debug field is greater than or equal */
/**  to the log_sel value. */
/** @return void */
void aapl_log_printf_full(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    Aapl_log_type_t log_sel,    /**< [in] Type of message logging. */
    const char *caller,         /**< [in] Caller's __func__ value, or 0. */
    int line,                   /**< [in] Caller's __LINE__ value, or 0. */
    const char *fmt,            /**< [in] printf format string. */
    ...)                        /**< [in] printf arguments. */
{
    va_list ap;
    if( check_debug_level(log_sel, aapl->debug) )
        return;

    va_start(ap, fmt);
    aapl_log_vprintf(aapl, log_sel, caller, line, fmt, ap);
    va_end(ap);
}

#if !defined __STDC_VERSION__ && !defined __cplusplus
/* c89 compile: */
# define USE_VARIABLE_LENGTH_ARRAY 0
#else
# define USE_VARIABLE_LENGTH_ARRAY 1
#endif

/** @brief Add new txt to AAPL's log */
/** @details DEBUG* messages will be sent to AAPL_STREAM (stdout by default, see aapl.h) */
/**          and will also be added to the AAPL log if Aapl_t::enable_debug_logging is TRUE. */
/** */
/** Condition,                     Action: */
/** When caller == 0 && line == 0, print "string" */
/** When caller == 0 && line >= 1, print "LABEL string" */
/** When caller != 0 && line == 0, print "LABEL function [time_stamp] string" */
/** When caller != 0             , print "LABEL function:line [time_stamp] string" */
/** @return void */
void aapl_log_add(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    Aapl_log_type_t log_sel,    /**< [in] Type of message logging. */
    const char *string,         /**< [in] String to add to the log. */
    const char *caller,         /**< [in] Caller's __func__ value, or 0. */
    int line)                   /**< [in] Caller's __LINE__ value, or 0. */
{
    BOOL is_data_char_log = (log_sel == AVAGO_DATA_CHAR || log_sel == AVAGO_DATA_CHAR_ADD);
    char **log      = is_data_char_log ? &aapl->data_char      : &aapl->log;
    char **log_end  = is_data_char_log ? &aapl->data_char_end  : &aapl->log_end;
    int   *log_size = is_data_char_log ? &aapl->data_char_size : &aapl->log_size;
    size_t new_item_length, temp_size;

    if (aapl->suppress_errors && (log_sel == AVAGO_WARNING || log_sel == AVAGO_ERR)) log_sel = AVAGO_DEBUG1;
    if (aapl->upgrade_warnings && log_sel == AVAGO_WARNING)
    {
        log_sel = AVAGO_ERR;
        aapl->return_code -= 1;
    }

    if( check_debug_level(log_sel, aapl->debug) )
        return;

    if( !caller ) caller = "";
    if( !string ) string = "";

    /* */
    new_item_length = strlen(string);
    temp_size = new_item_length + 40;

    if( *caller )
        temp_size += strlen(caller);

    {
    BOOL        add_to_buffer;
    const char *buf_begin;
#if USE_VARIABLE_LENGTH_ARRAY
    char buf[temp_size];
#else
    char *buf = 0;
#endif

    if( new_item_length > 64 * 1024 )
    {
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Call to aapl_log_add from %s:%d (log_sel: %d) exceeds recommended max size (%d > %d).\n", caller, line, log_sel, new_item_length, 64 * 1024);
    }

    if( is_data_char_log || (line == 0 && *caller == '\0') )
    {
        if( log_sel == AVAGO_DATA_CHAR )
            *log_end = *log;

        buf_begin = string;
    }
    else
    {
        const char *log_type = aapl_log_type_to_str(log_sel);
        char *buf_end;
#if !USE_VARIABLE_LENGTH_ARRAY
        buf = (char *)aapl_malloc(aapl, temp_size, __func__);
#endif
        buf_begin = buf_end = buf;

        if( *caller )
        {
            buf_end += sprintf(buf_end, "%s (%s:%d", log_type, caller, line);

# ifdef AAPL_LOG_TIME_STAMPS
            if( aapl->log_time_stamps )
                buf_end += aapl_local_strftime(buf_end, 10, " %H:%M:%S");
# endif
            buf_end += sprintf(buf_end, "): ");
        }
        else
            buf_end += sprintf(buf_end, "%s: ", log_type);

        memcpy(buf_end, string, new_item_length);
        buf_end += new_item_length;
        buf_end[0] = '\0';
        new_item_length = buf_end - buf_begin;
    }

    if( !is_data_char_log )
    {
        /* */
# ifdef AAPL_STREAM
        if( (log_sel == AVAGO_INFO || (uint)log_sel < AVAGO_MEM_LOG) && aapl->enable_stream_logging )
        {
            fwrite(buf_begin, 1, new_item_length, AAPL_STREAM);
        }
# endif
# ifdef AAPL_STREAM_ERR
        if( (log_sel == AVAGO_ERR || log_sel == AVAGO_WARNING) && aapl->enable_stream_err_logging )
            fwrite(buf_begin, 1, new_item_length, AAPL_STREAM_ERR);
# endif
    }

    /* */
    add_to_buffer = aapl->enable_debug_logging || (uint)log_sel >= AVAGO_MEM_LOG;

#ifdef USER_SPECIFIED_LOGGING_FUNCTION4
    if( add_to_buffer && !is_data_char_log )
    {
        USER_SPECIFIED_LOGGING_FUNCTION4(aapl, log_sel, buf_begin, new_item_length);
        add_to_buffer = FALSE;
    }
#endif
#ifdef USER_SPECIFIED_LOGGING_FUNCTION3
    if( add_to_buffer && !is_data_char_log )
    {
        USER_SPECIFIED_LOGGING_FUNCTION3(aapl, buf_begin, new_item_length);
        add_to_buffer = FALSE;
    }
#endif
#ifdef USER_SPECIFIED_LOGGING_FUNCTION
    if( add_to_buffer && !is_data_char_log )
    {
        USER_SPECIFIED_LOGGING_FUNCTION(buf_begin, new_item_length);
        add_to_buffer = FALSE;
    }
#endif

    if( add_to_buffer )
    {
        int current_log_len = *log_end - *log;
        int min_buffer_size = new_item_length + current_log_len + 1;
        int allocated = 0, wasted = 0;

        if( min_buffer_size > *log_size )
        {
            int new_buffer_size = min_buffer_size + current_log_len * 2;
            *log = (char *) aapl_realloc(aapl, *log, new_buffer_size, __func__);
            if( !*log ) return;
            *log_end = *log + current_log_len;
            *log_size = new_buffer_size;
            allocated = 1;
        }
        else if( (*log_size - min_buffer_size) > (current_log_len * 2 + min_buffer_size + AAPL_LOG_BUF_SIZE) )
        {
            *log = (char *) aapl_realloc(aapl, *log, min_buffer_size + AAPL_LOG_BUF_SIZE, __func__);
            if( !*log ) return;
            *log_end = *log + current_log_len;
            *log_size = min_buffer_size + AAPL_LOG_BUF_SIZE;
            wasted = 1;
        }

        /* */
        memcpy(*log_end, buf_begin, new_item_length);
        *log_end += new_item_length;
        (*log_end)[0] = '\0';

        if (allocated)
        {
            if (is_data_char_log)
                aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Allocating %d additional bytes to data_char log. Size is now %d bytes.\n", *log_size - current_log_len, *log_size);
            else
                aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Allocating %d additional bytes to AAPL log. Size is now %d bytes.\n", *log_size - current_log_len, *log_size);
        }
        else if (wasted)
        {
            if (is_data_char_log)
                aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "Wasted %d bytes in data_char log. Buffer resized to %d bytes.\n", *log_size - min_buffer_size, min_buffer_size + AAPL_LOG_BUF_SIZE);
            else
                aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "Wasted %d bytes in AAPL log. Buffer resized to %d bytes.\n", *log_size - min_buffer_size, min_buffer_size + AAPL_LOG_BUF_SIZE);
        }
    }
#if !USE_VARIABLE_LENGTH_ARRAY
    if( buf ) aapl_free(aapl, buf, __func__);
#endif
    }
}

/** @cond INTERNAL */
/** @brief   Printf append to a dynamically growing buffer. */
/** @details (*buf) must be initialized to a NULL pointer, and */
/**          released by calling aapl_free(). */
/** @return  void */
void aapl_buf_add(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    char **buf,         /**< [in,out] Pointer to buffer pointer. */
    char **buf_end,     /**< [in,out] Pointer into *buf of where to append. */
    int *size,          /**< [in,out] Pointer to current size of *buf. */
    const char *fmt,    /**< [in] printf format string. */
    ...)                /**< [in] printf format parameters. */
{
    va_list ap;
    if (!*buf)
    {
        *buf = (char *) aapl_malloc(aapl, AAPL_LOG_BUF_SIZE * 2, __func__);
        *buf_end = *buf;
        *size = AAPL_LOG_BUF_SIZE * 2;
    }

    va_start(ap, fmt);
    *buf_end += vsnprintf(*buf_end, AAPL_LOG_PRINTF_BUF_SIZE, fmt, ap);
    va_end(ap);
    if ((*size - (*buf_end - *buf)) < AAPL_LOG_BUF_SIZE)
    {
        aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__,
            "Allocating %d additional bytes to user supplied buffer. Size is now %d bytes.\n", AAPL_LOG_BUF_SIZE * 2, *size);
        *buf = (char *) aapl_realloc(aapl, *buf, *size + AAPL_LOG_BUF_SIZE * 2, __func__);
        *size += AAPL_LOG_BUF_SIZE * 2;
        *buf_end = *buf + strlen(*buf);
    }
}

/** @} */


/** @defgroup Conversions Type to String and String to Type Conversion Functions */
/** @{ */

/** @cond INTERNAL */

/*////////////////////////////////////////////////////////////////////// */
/* Functions to convert types to strings, and strings to types. */
/* Allows for human readable output of these enums during logging. */
/* And for more meaningful input from a CLI. */


/* Return index of matching entry, or -1 if no match. */
EXT int name_to_index(Aapl_conv_table_t tbl[], const char *name, uint skip_prefix)
{
    int i;
    for( i = 0; tbl[i].name; i++ )
        if( 0==aapl_strcasecmp(name,tbl[i].name) )
            return i;
    if( skip_prefix > 0 )
    {
        for( i = 0; tbl[i].name; i++ )
        {
            if( strlen(tbl[i].name) > skip_prefix &&
                0==aapl_strcasecmp(name,tbl[i].name+skip_prefix) )
                return i;
        }
    }
    return -1;
}

/* Return index of matching entry, or -1 if no match. */
EXT int value_to_index(Aapl_conv_table_t tbl[], int value)
{
    int i;
    for( i = 0; tbl[i].name; i++ )
        if( value == tbl[i].value )
            return i;
    return -1;
}

/** @endcond */


static Aapl_conv_table_t comm_method_table[] =
{
    { "AACS_SBUS",                 AVAGO_AACS_SBUS                 },
    { "AACS_MDIO",                 AVAGO_AACS_MDIO                 },
    { "AACS_I2C",                  AVAGO_AACS_I2C                  },
    { "USER_SUPPLIED_I2C",         AVAGO_USER_SUPPLIED_I2C         },
    { "USER_SUPPLIED_MDIO",        AVAGO_USER_SUPPLIED_MDIO        },
    { "USER_SUPPLIED_SBUS_DIRECT", AVAGO_USER_SUPPLIED_SBUS_DIRECT },
    { "SYSTEM_I2C",                AVAGO_SYSTEM_I2C                },
    { "SYSTEM_MDIO",               AVAGO_SYSTEM_MDIO               },
    { "GPIO_MDIO",                 AVAGO_GPIO_MDIO                 },
    { "OFFLINE",                   AVAGO_OFFLINE                   },
    { "USER_I2C",                  AVAGO_USER_SUPPLIED_I2C         },
    { "USER_MDIO",                 AVAGO_USER_SUPPLIED_MDIO        },
    { "USER_SBUS",                 AVAGO_USER_SUPPLIED_SBUS_DIRECT },
    { 0,                           0 }
};
const char *aapl_comm_method_to_str(Aapl_comm_method_t value)
{
    Aapl_conv_table_t *table = comm_method_table;
    int index = value_to_index(table,value);
    return index >= 0 ? table[index].name : "unknown";
}
BOOL aapl_str_to_comm_method(const char *name, Aapl_comm_method_t *out)
{
    Aapl_conv_table_t *table = comm_method_table;
    int index = name_to_index(table,name,0);
    if( index >= 0 )
        *out = (Aapl_comm_method_t)table[index].value;
    return index >= 0;
}

static Aapl_conv_table_t log_type_table[] =
{
    { "DEBUG0",         AVAGO_DEBUG0          },
    { "DEBUG1",         AVAGO_DEBUG1          },
    { "DEBUG2",         AVAGO_DEBUG2          },
    { "DEBUG3",         AVAGO_DEBUG3          },
    { "DEBUG4",         AVAGO_DEBUG4          },
    { "DEBUG5",         AVAGO_DEBUG5          },
    { "DEBUG6",         AVAGO_DEBUG6          },
    { "DEBUG7",         AVAGO_DEBUG7          },
    { "DEBUG8",         AVAGO_DEBUG8          },
    { "DEBUG9",         AVAGO_DEBUG9          },
    { "MEM_LOG",        AVAGO_MEM_LOG         },
    { "DATA_CHAR",      AVAGO_DATA_CHAR       },
    { "DATA_CHAR_ADD",  AVAGO_DATA_CHAR_ADD   },
    { "ERROR",          AVAGO_ERR             },
    { "WARNING",        AVAGO_WARNING         },
    { "INFO",           AVAGO_INFO            },
    { 0,                0 }
};
const char *aapl_log_type_to_str(Aapl_log_type_t value)
{
    Aapl_conv_table_t *table = log_type_table;
    int index = value_to_index(table,value);
    return index >= 0 ? table[index].name : "INVALID";
}


static Aapl_conv_table_t pll_clk_table[] =
{
    { "REFCLK",             AVAGO_SERDES_TX_PLL_REFCLK             },
    { "RX_DIVX",            AVAGO_SERDES_TX_PLL_RX_DIVX            },
    { "OFF",                AVAGO_SERDES_TX_PLL_OFF                },
    { "PCIE_CORE_CLK",      AVAGO_SERDES_TX_PLL_PCIE_CORE_CLK      },
    { "PCIE_CORE_CLK_DIV2", AVAGO_SERDES_TX_PLL_PCIE_CORE_CLK_DIV2 },
    { 0,                0 }
};
const char *aapl_pll_clk_to_str(Avago_serdes_tx_pll_clk_t value)
{
    Aapl_conv_table_t *table = pll_clk_table;
    int index = value_to_index(table,value);
    return index >= 0 ? table[index].name : "unknown";
}
BOOL aapl_str_to_pll_clk(const char *name, Avago_serdes_tx_pll_clk_t *out)
{
    Aapl_conv_table_t *table = pll_clk_table;
    int index = name_to_index(table,name,0);
    if( index >= 0 )
        *out = (Avago_serdes_tx_pll_clk_t)table[index].value;
    return index >= 0;
}

static Aapl_conv_table_t process_id_table[] =
{
    { "PROCESS_B", AVAGO_PROCESS_B },
#ifndef MV_HWS_REDUCED_BUILD
    { "PROCESS_G", AVAGO_PROCESS_G },
#endif /* MV_HWS_REDUCED_BUILD */
    { "PROCESS_F", AVAGO_PROCESS_F },
#ifndef MV_HWS_REDUCED_BUILD
    { "PROCESS_A", AVAGO_PROCESS_A },
    { "PROCESS_E", AVAGO_PROCESS_E },
    { "PROCESS_D", AVAGO_PROCESS_D },
#endif /* MV_HWS_REDUCED_BUILD */
    { 0,         0 }
};
const char *aapl_process_id_to_str(Avago_process_id_t value)
{
    Aapl_conv_table_t *table = process_id_table;
    int index = value_to_index(table,value);
    return index >= 0 ? table[index].name : "UNKNOWN_PROCESS";
}

static Aapl_conv_table_t dma_type_table[] =
{
    { "ESB",            AVAGO_ESB           },
    { "ESB_DIRECT",     AVAGO_ESB_DIRECT    },
    { "LSB",            AVAGO_LSB           },
    { "LSB_DIRECT",     AVAGO_LSB_DIRECT    },
#ifndef MV_HWS_REDUCED_BUILD
    { "DMEM",           AVAGO_DMEM          },
    { "DMEM_PREHALTED", AVAGO_DMEM_PREHALTED},
    { "IMEM",           AVAGO_IMEM          },
    { "IMEM_PREHALTED", AVAGO_IMEM_PREHALTED},
#endif /* MV_HWS_REDUCED_BUILD */
    { 0,            0 }
};
const char *aapl_mem_type_to_str(Avago_serdes_mem_type_t value)
{
    Aapl_conv_table_t *table = dma_type_table;
    int index = value_to_index(table,value);
    return index >= 0 ? table[index].name : "unknown";
}
BOOL aapl_str_to_mem_type(const char *name, Avago_serdes_mem_type_t *out)
{
    Aapl_conv_table_t *table = dma_type_table;
    int index = name_to_index(table,name,0);
    if( index >= 0 )
        *out = (Avago_serdes_mem_type_t)table[index].value;
    return index >= 0;
}

const char *aapl_bool_to_str(int value)
{
    return value ? "TRUE" : "FALSE";
}

const char *aapl_onoff_to_str(int value)
{
    return value ? "ON" : "OFF";
}

static Aapl_conv_table_t data_sel_table[] =
{
    { "LOOPBACK", AVAGO_SERDES_TX_DATA_SEL_LOOPBACK },
    { "CORE",     AVAGO_SERDES_TX_DATA_SEL_CORE     },
    { "PRBS7",    AVAGO_SERDES_TX_DATA_SEL_PRBS7    },
    { "PRBS9",    AVAGO_SERDES_TX_DATA_SEL_PRBS9    },
    { "PRBS11",   AVAGO_SERDES_TX_DATA_SEL_PRBS11   },
    { "PRBS15",   AVAGO_SERDES_TX_DATA_SEL_PRBS15   },
    { "PRBS23",   AVAGO_SERDES_TX_DATA_SEL_PRBS23   },
    { "PRBS31",   AVAGO_SERDES_TX_DATA_SEL_PRBS31   },
    { "USER",     AVAGO_SERDES_TX_DATA_SEL_USER     },
    { 0,          0 }
};
const char *aapl_data_sel_to_str(Avago_serdes_tx_data_sel_t value)
{
    Aapl_conv_table_t *table = data_sel_table;
    int index = value_to_index(table,value);
    return index >= 0 ? table[index].name : "unknown";
}
BOOL aapl_str_to_data_sel(const char *name, Avago_serdes_tx_data_sel_t *out)
{
    Aapl_conv_table_t *table = data_sel_table;
    int index = name_to_index(table,name,0);
    if( index >= 0 )
        *out = (Avago_serdes_tx_data_sel_t)table[index].value;
    return index >= 0;
}

static Aapl_conv_table_t cmp_data_table[] =
{
    { "PRBS7",      AVAGO_SERDES_RX_CMP_DATA_PRBS7     },
    { "PRBS9",      AVAGO_SERDES_RX_CMP_DATA_PRBS9     },
    { "PRBS11",     AVAGO_SERDES_RX_CMP_DATA_PRBS11    },
    { "PRBS15",     AVAGO_SERDES_RX_CMP_DATA_PRBS15    },
    { "PRBS23",     AVAGO_SERDES_RX_CMP_DATA_PRBS23    },
    { "PRBS31",     AVAGO_SERDES_RX_CMP_DATA_PRBS31    },
    { "OFF",        AVAGO_SERDES_RX_CMP_DATA_OFF       },
    { "SELF_SEED",  AVAGO_SERDES_RX_CMP_DATA_SELF_SEED },
    { 0,            0 }
};
const char *aapl_cmp_data_to_str(Avago_serdes_rx_cmp_data_t value)
{
    Aapl_conv_table_t *table = cmp_data_table;
    int index = value_to_index(table,value);
    return index >= 0 ? table[index].name : "unknown";
}
BOOL aapl_str_to_cmp_data(const char *name, Avago_serdes_rx_cmp_data_t *out)
{
    Aapl_conv_table_t *table = cmp_data_table;
    int index = name_to_index(table,name,0);
    if( index >= 0 )
        *out = (Avago_serdes_rx_cmp_data_t)table[index].value;
    return index >= 0;
}

static Aapl_conv_table_t mode_table[] =
{
    { "OFF",         AVAGO_SERDES_RX_CMP_MODE_OFF },
    { "XOR",         AVAGO_SERDES_RX_CMP_MODE_XOR },
    { "TEST_PATGEN", AVAGO_SERDES_RX_CMP_MODE_TEST_PATGEN },
    { "MAIN_PATGEN", AVAGO_SERDES_RX_CMP_MODE_MAIN_PATGEN },
    { 0,             0 }
};
const char *aapl_cmp_mode_to_str(Avago_serdes_rx_cmp_mode_t value)
{
    static char buf[16];
    Aapl_conv_table_t *table = mode_table;
    int index = value_to_index(table,value);
    if( index >= 0 )
        return table[index].name;
    snprintf(buf,sizeof(buf),"0x%04x",value);
    return buf;
}
BOOL aapl_str_to_cmp_mode(const char *name, Avago_serdes_rx_cmp_mode_t *out)
{
    Aapl_conv_table_t *table = mode_table;
    int index = name_to_index(table,name,0);
    if( index >= 0 )
        *out = (Avago_serdes_rx_cmp_mode_t)table[index].value;
    else
    {
        char *endptr;
        Avago_serdes_rx_cmp_mode_t value = (Avago_serdes_rx_cmp_mode_t)strtol(name,&endptr,16);
        if( *endptr == '\0' )
        {
            *out = value;
            return TRUE;
        }
    }
    return index >= 0;
}

const char *aapl_enable_to_str(int value)
{
    return value ? "enabled" : "disabled";
}

static Aapl_conv_table_t term_table[] =
{
    { "AVDD",  AVAGO_SERDES_RX_TERM_AVDD  },
    { "FLOAT", AVAGO_SERDES_RX_TERM_FLOAT },
    { "AGND",  AVAGO_SERDES_RX_TERM_AGND  },
    { 0,       0 }
};
const char *aapl_term_to_str(Avago_serdes_rx_term_t value)
{
    Aapl_conv_table_t *table = term_table;
    int index = value_to_index(table,value);
    return index >= 0 ? table[index].name : "unknown";
}
BOOL aapl_str_to_term(const char *name, Avago_serdes_rx_term_t *out)
{
    Aapl_conv_table_t *table = term_table;
    int index = name_to_index(table,name,0);
    if( index >= 0 )
        *out = (Avago_serdes_rx_term_t)table[index].value;
    return index >= 0;
}

static Aapl_conv_table_t dfe_tune_mode_table[] =
{
    { "ICAL",           AVAGO_DFE_ICAL           },
    { "PCAL",           AVAGO_DFE_PCAL           },
    { "START_ADAPTIVE", AVAGO_DFE_START_ADAPTIVE },
    { "STOP_ADAPTIVE",  AVAGO_DFE_STOP_ADAPTIVE  },
    { "ENABLE_RR",      AVAGO_DFE_ENABLE_RR      },
    { "DISABLE_RR",     AVAGO_DFE_DISABLE_RR     },
    { 0,                0 }
};
const char *aapl_dfe_tune_mode_to_str(Avago_serdes_dfe_tune_mode_t value)
{
    Aapl_conv_table_t *table = dfe_tune_mode_table;
    int index = value_to_index(table,value);
    return index >= 0 ? table[index].name : "unknown";
}
BOOL aapl_str_to_dfe_tune_mode(const char *name, Avago_serdes_dfe_tune_mode_t *out)
{
    Aapl_conv_table_t *table = dfe_tune_mode_table;
    int index = name_to_index(table,name,0);
    if( index >= 0 )
        *out = (Avago_serdes_dfe_tune_mode_t)table[index].value;
    return index >= 0;
}

/** @} */
