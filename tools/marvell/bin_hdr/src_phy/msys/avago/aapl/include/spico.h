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

/* AAPL (ASIC and ASSP Programming Layer) support for talking with SPICO */
/* processors in SBus master and SerDes slices. */

/** Doxygen File Header */
/** @file */
/** @brief Declarations for SPICO processor functions. */

#ifndef AVAGO_SPICO_H_
#define AVAGO_SPICO_H_

#define AVAGO_SPICO_HALT 2
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
typedef enum
{
    AVAGO_SPICO_RESET=0,
    AVAGO_SPICO_RUNNING=1,
    AVAGO_SPICO_PAUSE=2,
    AVAGO_SPICO_ERROR=3
} Avago_spico_state_t;

/**@brief SPICO processor status values */
typedef struct
{
    uint enabled;               /**< Indicates if processor is enabled and runnning */
    uint pc;                    /**< Program Counter value */
    uint revision;              /**< Revision of firmware */
    uint build;                 /**< Build ID of firmware */
    uint clk;                   /**< Clock Processor is running on */
    Avago_spico_state_t state;  /**< State the Processor is in */
} Avago_spico_status_t;
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT int  avago_spico_status( Aapl_t *aapl, uint sbus_addr, Avago_spico_status_t *st);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
EXT uint avago_spico_running(Aapl_t *aapl, uint sbus_addr);
EXT int  avago_spico_reset(  Aapl_t *aapl, uint sbus_addr);

/* uploads SPICO machine code. Returns number of words uploaded */
#ifndef SWIG
/* disabling this routine from SWIG access to avoid supporting an unbounded array argument */
#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
EXT int avago_spico_upload_swap_image(Aapl_t *aapl, uint sbus_addr, int words, const short rom[]);
EXT int avago_spico_upload(Aapl_t *aapl, uint sbus_addr, BOOL ram_bist, int words, const short rom[]);
EXT int avago_firmware_upload(Aapl_t *aapl, uint addr,
                              int serdes_rom_size, const short *serdes_rom,
                              int sbm_rom_size,    const short *sbm_rom,
                              int sdi_rom_size,    const short *sdi_rom);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
#endif

EXT void avago_spico_wait_for_upload(Aapl_t *aapl, uint sbus_addr);
#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
EXT void avago_twi_wait_for_complete(Aapl_t *aapl, uint sbus_addr);
#endif /* !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER */

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#if AAPL_ENABLE_FILE_IO
EXT int avago_load_rom_from_file(Aapl_t *aapl, const char *filename, int *rom_size, short **rom);
EXT int avago_firmware_upload_file(Aapl_t *aapl, uint addr,
                              const char *serdes_rom_file,
                              const char *sbm_rom_file,
                              const char *sdi_rom_file);
EXT char *avago_find_swap_file(Aapl_t *aapl, const char *filename);
EXT int avago_spico_upload_file(Aapl_t *aapl, uint sbus_addr, BOOL ram_bist, const char *filename);
#endif
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/* executes a serdes interrupt */
EXT uint avago_spico_int(Aapl_t *aapl, uint sbus_addr, int int_num, int param);
#ifndef MV_HWS_REDUCED_BUILD
EXT BOOL avago_spico_int_check_full(Aapl_t *aapl, const char *caller, int line, uint addr, int int_num, int param);
#else
EXT BOOL avago_spico_int_check_reduce(Aapl_t *aapl, uint addr, int int_num, int param);
#endif /* MV_HWS_REDUCED_BUILD */

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT uint avago_spico_broadcast_int(Aapl_t *aapl, int int_num, int param, int args, ...);
EXT uint avago_spico_broadcast_int_w_mask(Aapl_t *aapl, uint addr_mask, int int_num, int param, int args, ...);

typedef enum
{
    AVAGO_SPICO_INT_ALL=0,
    AVAGO_SPICO_INT_FIRST=1,
    AVAGO_SPICO_INT_NOT_FIRST=2
} Avago_spico_int_flags_t;
typedef struct
{
    int                     interrupt;
    int                     param;
    int                     ret;
    Avago_spico_int_flags_t flags;
} Avago_spico_int_t;
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
#ifndef MV_HWS_REDUCED_BUILD
EXT int  avago_spico_int_array(Aapl_t *aapl, uint sbus_addr, int num_elements, Avago_spico_int_t *interrupts);
#endif /* MV_HWS_REDUCED_BUILD */
EXT uint avago_firmware_get_rev(     Aapl_t *aapl, uint sbus_addr);
EXT uint avago_firmware_get_build_id(Aapl_t *aapl, uint sbus_addr);
EXT uint avago_spico_crc(            Aapl_t *aapl, uint sbus_addr);
#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
/* TBD write code for sram_reset */
EXT int avago_spico_ram_bist( Aapl_t *aapl, uint sbus_addr);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
#ifndef MV_HWS_REDUCED_BUILD
# if AAPL_ENABLE_USER_SERDES_INT
    EXT unsigned int user_supplied_serdes_interrupt_function(
        Aapl_t *aapl,
        uint sbus_addr,
        int int_num,
        int param);
# endif
#endif /* MV_HWS_REDUCED_BUILD */

# ifdef AAPL_ENABLE_INTERNAL_FUNCTIONS
    EXT void aapl_crc_one_byte(int *crc_ptr, int value);
    EXT int aapl_crc_rom(short *memory, int length);

    EXT int avago_spico_halt(Aapl_t *aapl, uint addr);
    EXT int avago_spico_resume(Aapl_t *aapl, uint addr, int spico_run_state);
# endif

#endif
