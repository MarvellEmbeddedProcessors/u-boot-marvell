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

/* Logging and core type conversion functions */
/* */

/** Doxygen File Header */
/** @file */
/** @brief Logging and core type conversion functions. */

#ifndef AAPL_LOGGING_H_
#define AAPL_LOGGING_H_

/*////////////////////////////////////////////////////////////////////// */
/* AAPL LOGGING functions: */

/** @defgroup Logging Logging Functions and Types */
/** @{ */

/** @brief Aapl_log_type_t is used for the AAPL struct log buffers */
typedef enum
{
    AVAGO_DEBUG0 = 0, AVAGO_DEBUG1, AVAGO_DEBUG2, AVAGO_DEBUG3,
    AVAGO_DEBUG4, AVAGO_DEBUG5, AVAGO_DEBUG6,
    AVAGO_DEBUG7, AVAGO_DEBUG8, AVAGO_DEBUG9,
    AVAGO_MEM_LOG, AVAGO_DATA_CHAR, AVAGO_DATA_CHAR_ADD,
    AVAGO_ERR, AVAGO_WARNING, AVAGO_INFO
} Aapl_log_type_t;


EXT int  aapl_fail_full(      Aapl_t *aapl, const char *caller, int line, const char *fmt, ...);
EXT void aapl_log_printf_reduce(Aapl_t *aapl, Aapl_log_type_t log_sel, const char *caller, int line, const char * fmt, ...);
EXT void aapl_log_printf_full(Aapl_t *aapl, Aapl_log_type_t log_sel, const char *caller, int line, const char * fmt, ...);
EXT void aapl_log_add(   Aapl_t *aapl, Aapl_log_type_t log_sel, const char *string, const char *caller, int line);

/* Printf append to a dynamically growing buffer: */
EXT void aapl_buf_add(Aapl_t *aapl, char **buf, char **buf_end, int *size, const char *fmt, ...);

EXT const char *aapl_log_get(Aapl_t *aapl);
EXT void        aapl_log_clear(Aapl_t *aapl);

/** @} */

/*////////////////////////////////////////////////////////////////////// */
/* Functions to convert types to strings, and strings to types. */
/* Allows for human readable output of these enums during logging. */

# ifdef AAPL_ENABLE_INTERNAL_FUNCTIONS
/** @cond INTERNAL */
    typedef struct
    {
        const char *const name;
        int value;
    } Aapl_conv_table_t;

    EXT int name_to_index(Aapl_conv_table_t tbl[], const char *name, uint skip_prefix);
    EXT int value_to_index(Aapl_conv_table_t tbl[], int value);
/** @endcond */
# endif

EXT const char *aapl_addr_to_str          (uint                          val);
EXT const char *aapl_bool_to_str          (int                           val);
EXT const char *aapl_comm_method_to_str   (Aapl_comm_method_t            val);
EXT const char *aapl_cmp_data_to_str      (Avago_serdes_rx_cmp_data_t    val);
EXT const char *aapl_cmp_mode_to_str      (Avago_serdes_rx_cmp_mode_t    val);
EXT const char *aapl_data_sel_to_str      (Avago_serdes_tx_data_sel_t    val);
EXT const char *aapl_dfe_tune_mode_to_str (Avago_serdes_dfe_tune_mode_t  val);
EXT const char *aapl_enable_to_str        (int                           val);
EXT const char *aapl_log_type_to_str      (Aapl_log_type_t               val);
EXT const char *aapl_mem_type_to_str      (Avago_serdes_mem_type_t       val);
EXT const char *aapl_onoff_to_str         (int                           val);
EXT const char *aapl_pll_clk_to_str       (Avago_serdes_tx_pll_clk_t     val);
EXT const char *aapl_process_id_to_str    (Avago_process_id_t            val);
EXT const char *aapl_term_to_str          (Avago_serdes_rx_term_t        val);

/* ported from aapl 2.2.3 for AACS Server integration */
EXT BOOL aapl_str_to_addr         (const char *name, char **endptr, uint *addr);
EXT BOOL aapl_str_to_cmp_data     (const char *name, Avago_serdes_rx_cmp_data_t *out);
EXT BOOL aapl_str_to_cmp_mode     (const char *name, Avago_serdes_rx_cmp_mode_t *out);
EXT BOOL aapl_str_to_comm_method  (const char *name, Aapl_comm_method_t *out);
EXT BOOL aapl_str_to_data_sel     (const char *name, Avago_serdes_tx_data_sel_t *out);
EXT BOOL aapl_str_to_dfe_tune_mode(const char *name, Avago_serdes_dfe_tune_mode_t *out);
EXT BOOL aapl_str_to_mem_type     (const char *name, Avago_serdes_mem_type_t *out);
EXT BOOL aapl_str_to_pll_clk      (const char *name, Avago_serdes_tx_pll_clk_t *out);
EXT BOOL aapl_str_to_term         (const char *name, Avago_serdes_rx_term_t *out);

#endif
