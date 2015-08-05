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

/* Core AAPL types and functions. */
/* */

#ifndef AAPL_CORE_H_
#define AAPL_CORE_H_

/* Macros to suppress error accounting when errors are expected and handled: */
#ifndef MV_HWS_REDUCED_BUILD
#define AAPL_SUPPRESS_ERRORS_PUSH(aapl) { int return_code = (aapl->suppress_errors++, aapl->return_code)
#define AAPL_SUPPRESS_ERRORS_POP(aapl)        aapl->return_code = return_code; aapl->suppress_errors--; }
#else
#define AAPL_SUPPRESS_ERRORS_PUSH(aapl)
#define AAPL_SUPPRESS_ERRORS_POP(aapl)
#endif /* MV_HWS_REDUCED_BUILD */

/* AACS Server capabilities flags: */
#define AACS_SERVER_NO_CAPABILITIES 0x1
#define AACS_SERVER_SPICO_INT 0x2
#define AACS_SERVER_DIAG 0x4
#define AACS_SERVER_SBUS_RESET 0x8

typedef enum
{
    AVAGO_UNKNOWN_PROCESS = 0,
#ifndef MV_HWS_REDUCED_BUILD
    AVAGO_PROCESS_D,
    AVAGO_PROCESS_E,
    AVAGO_PROCESS_A,
#endif /* MV_HWS_REDUCED_BUILD */
    AVAGO_PROCESS_F,
    AVAGO_PROCESS_B,
#ifndef MV_HWS_REDUCED_BUILD
    AVAGO_PROCESS_G
#endif /* MV_HWS_REDUCED_BUILD */
} Avago_process_id_t;

typedef enum
{
    AVAGO_UNKNOWN_IP                 = 0x00,
    AVAGO_SERDES                     = 0x01,
    AVAGO_SBUS_CONTROLLER            = 0x02,
    AVAGO_SPICO                      = 0x03,
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    AVAGO_QPI                        = 0x04,
    AVAGO_FBD                        = 0x05,
    AVAGO_PCS64B66B                  = 0x06,
    AVAGO_AUTO_NEGOTIATION           = 0x07,
    AVAGO_PCS64B66B_FEC              = 0x08,
    AVAGO_PCIE_PCS                   = 0x09,
    AVAGO_PLL                        = 0x0a,
    AVAGO_PMRO                       = 0x0b,
    AVAGO_DDR_ADDRESS                = 0x0c,
    AVAGO_DDR_DATA                   = 0x0d,
    AVAGO_DDR_TRAIN                  = 0x0e,
    AVAGO_DDR_CTC                    = 0x0f,
    AVAGO_DDR_STOP                   = 0x10,
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
    AVAGO_THERMAL_SENSOR             = 0x11,
    AVAGO_RMON                       = 0x12,
    AVAGO_LINK_EMULATOR              = 0x13,
    AVAGO_AVSP_CONTROL_LOGIC         = 0x14,
    AVAGO_M4                         = 0x15,
    AVAGO_P1                         = 0x16,
    AVAGO_MLD                        = 0x17,
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    AVAGO_RSFEC_BRIDGE               = 0x18,
    AVAGO_CROSSPOINT                 = 0x19,
    AVAGO_SAPPH_GBX                  = 0x20,
    AVAGO_SAPPH_GBX_TOP              = 0x21,
    AVAGO_OPAL_RSFEC528              = 0x22,
    AVAGO_OPAL_RSFEC528_544          = 0x23,
    AVAGO_OPAL_HOST_ALIGNER          = 0x24,
    AVAGO_OPAL_MOD_ALIGNER           = 0x25,
    AVAGO_OPAL_CONTROL               = 0x26,

    AVAGO_LINK_EMULATOR_2            = 0x81,
    AVAGO_SLE_PKT                    = 0x82,
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
    AVAGO_SLE                        = 0x83,


    AVAGO_RAM_PMRO,
    AVAGO_PANDORA_LSB,
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    AVAGO_MAX_RING_ADDRESS           = 0xdf,
    AVAGO_SERDES_P1_BROADCAST        = 0xed,
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
    AVAGO_SERDES_M4_BROADCAST        = 0xee,
#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
    AVAGO_SERDES_D6_BROADCAST        = 0xff,
    AVAGO_THERMAL_SENSOR_BROADCAST   = 0xef,
    AVAGO_DDR_STOP_BROADCAST         = 0xf0,
    AVAGO_DDR_CTC_BROADCAST          = 0xf1,
    AVAGO_DDR_TRAIN_BROADCAST        = 0xf2,
    AVAGO_DDR_DATA_BROADCAST         = 0xf3,
    AVAGO_DDR_ADDRESS_BROADCAST      = 0xf4,
    AVAGO_PMRO_BROADCAST             = 0xf5,
    AVAGO_RESERVED_BROADCAST         = 0xf6,
    AVAGO_PCIE_PCS_BROADCAST         = 0xf7,
    AVAGO_PCS64B66B_BROADCAST        = 0xf8,
    AVAGO_AUTO_NEGOTIATION_BROADCAST = 0xf9,
    AVAGO_FBD_BROADCAST              = 0xfb,
    AVAGO_QPI_BROADCAST              = 0xfc,
#endif /* !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER */
    AVAGO_SPICO_BROADCAST            = 0xfd,
    AVAGO_SERDES_BROADCAST           = 0xff
} Avago_ip_type_t;

/* When auto-setting Avago_ip_type_t, what is the max value to be read from the SBus address 0xff. */
/* Additional Avago_ip_type_t values above what is in the SBus master spec */
/*   (ie, special conditions that obtain the ip_type elsewhere) may still be used, */
/* but the value listed here is the max that will be read from the SBus. */
#define AVAGO_IP_TYPE_MAX 0x26

#define AVAGO_IP_TYPE_ALT_RANGE_LO 0x81
#define AVAGO_IP_TYPE_ALT_RANGE_HI 0x85

/** @brief AAPL struct used by virtually every function in this API. */
/** @warning Generally the user should not read or modify any elements of this struct. */
/** Use functions provided to get and set information in this structure. */
typedef struct
{
    uint devNum;
    uint portGroup;
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    uint debug;                               /**< global debug level */
    uint verbose;                             /**< global verbosity level */
    uint suppress_errors;                     /**< Turn aapl_log_add ERR and WARNINGS into DEBUG1 messages */
    uint upgrade_warnings;                    /**< If set, all WARNINGS are upgraded to ERRORS */

    int enable_debug_logging;                 /**< When enabled, data sent to DEBUGn will also be added to the AAPL log */
    int enable_stream_logging;                /**< When enabled, INFO and DEBUGn messages are written to AAPL_STREAM (if defined) */
    int enable_stream_err_logging;            /**< When enabled, warnings and errors are written to AAPL_STREAM_ERR (if defined) */

    int log_time_stamps;                      /**< When enabled, time stamps are added to AAPL logs and DEBUGn output */
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

    int serdes_int_timeout;                   /**< 28nm SerDes SPICO interrupt maximum number of tries */
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    int sbus_mdio_timeout;                    /**< sbus-over-mdio SBUS_RESULT polling maximum number of tries */
    Aapl_comm_method_t communication_method;  /**< Method for communicating with Avago devices. */
    uint enable_serdes_core_port_interrupt;   /**< When == 0: Sends SerDes interrupts via the SBus */
                                              /**< When == 1: calls the user_supplied_serdes_interrupt_function() for SerDes interrupts. */
                                              /**<            (must have AAPL_ENABLE_USER_SERDES_INT defined) */
                                              /**< When == 2: uses the Avago LE block (only available on some Avago test chips) to send SerDes */
                                              /**<            interrupts to the SerDes' core interrupt interface */
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3*/

    uint chips;                               /**< number of die this struct points to */
    uint sbus_rings;                          /**< number of SBus rings this struct points to */
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3

    int max_cmds_buffered;                    /**< Maximum bumber of commands to queue before sending them via TCP over AACS. */
                                              /**< Setting this to 0 disables command buffering */
    int prior_connection;                     /**< Indicates if this AAPL struct has ever connected to an AACS server. */
    int disable_reconnect;                    /**< Don't allow AAPL to reconnect to TCP clients */

    int cmds_buffered;                        /**< number of commands in buf_cmd (which are bufferred AACS commands) */

    uint capabilities;                        /**< Bitmask of the remote AACS Server's capabilities */
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3*/

# ifndef SWIG

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    char *buf_cmd, *buf_cmd_end;              /**< Pointer to start/end of AACS command buffer */
    char *aacs_server_buffer;                 /**< Pointer to aacs_server's internal buffer */
    char *aacs_server;          /**< Server name (or IP address) used to open the AACS socket */
    int socket;                 /**< Socket used for AACS TCP communications */
    int tcp_port;               /**< TCP port used to open the AACS socket */

    uint last_mdio_addr[AAPL_MAX_CHIPS];    /**< Last MDIO address used */
    uint mdio_base_port_addr;               /**< MDIO base port address. */
    int  i2c_base_addr;                     /**< I2C base address. */
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
    const char *chip_name[AAPL_MAX_CHIPS];  /**< Array of chip name pointers */
    const char *chip_rev[AAPL_MAX_CHIPS];   /**< Array of chip revision pointers */
    uint jtag_idcode[AAPL_MAX_CHIPS];       /**< JTAG IDCODE for each chip */
#endif /* !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER */

    Avago_process_id_t process_id[AAPL_MAX_CHIPS]; /**< Process Identifier for each chip */
    unsigned short   ip_rev[AAPL_MAX_CHIPS][AAPL_MAX_RINGS][256]; /**< IP revision for each SBus Rx */
    unsigned short firm_rev[AAPL_MAX_CHIPS][AAPL_MAX_RINGS][256]; /**< Revision of firmware load, populated if ip_type is SERDES or SPICO */
    unsigned short firm_build[AAPL_MAX_CHIPS][AAPL_MAX_RINGS][256]; /**< Build of firmware load, populated if ip_type is SERDES or SPICO */
    char      spico_running[AAPL_MAX_CHIPS][AAPL_MAX_RINGS][256]; /**< Indicator of SPICO processor is running. */
    unsigned char   ip_type[AAPL_MAX_CHIPS][AAPL_MAX_RINGS][256]; /**< Avago_ip_type_t identifier for each SBus Rx */
    unsigned char   lsb_rev[AAPL_MAX_CHIPS][AAPL_MAX_RINGS][256]; /**< Revision of LSB block, populated if ip_type is SERDES */
    unsigned char max_sbus_addr[AAPL_MAX_CHIPS][AAPL_MAX_RINGS];  /**< max SBus address for each die and SBus ring */

    int return_code;        /**< set by most functions to indicate success/fail status */

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    int   log_size;         /**< memory log managment */
    int   data;             /**< used for functions that return int data */
    int   data_char_size;   /**< data_char memory management */
    char *data_char_end;    /**< to truncate data_char, set data_char_end = data_char; */
    char *log;              /**< memory log, logs commands, info,errors, warnings, and debug statements */
    char *log_end;          /**< to truncate log, set log_end = log; */
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
    char *data_char;        /**< used for functions that return strings */
#endif /* !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER */

#endif /* SWIG */

#ifndef MV_HWS_REDUCED_BUILD
    void *client_data;
    volatile int async_cancel;
#endif /* MV_HWS_REDUCED_BUILD */
} Aapl_t;

/* Create a pointer to a new AAPL struct */
EXT void aapl_init(Aapl_t *aapl);
EXT Aapl_t *aapl_construct(void);
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT void    aapl_destruct(Aapl_t *aapl);
EXT int aapl_get_async_cancel_flag(Aapl_t *aapl);
EXT int aapl_set_async_cancel_flag(Aapl_t *aapl, int new_value);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
/* Returns the return code and then sets it to zero. Values less than 0 indicate failure, while 0 indicates no error. */
EXT int         aapl_get_return_code(Aapl_t *aapl);

/* The following functions return information from the Aapl_t struct. addr is used to return specific information for that element. */
EXT uint        aapl_get_chips(Aapl_t *aapl);
EXT uint        aapl_get_max_sbus_addr(Aapl_t *aapl, uint addr);
#ifndef MV_HWS_REDUCED_BUILD
EXT uint        aapl_get_jtag_idcode(Aapl_t *aapl, uint addr);
EXT const char *aapl_get_chip_name(Aapl_t *aapl, uint addr);
EXT const char *aapl_get_chip_rev_str(Aapl_t *aapl, uint addr);
EXT const char *aapl_get_process_id_str(Aapl_t *aapl, uint addr);
#endif /* MV_HWS_REDUCED_BUILD*/
EXT uint        aapl_get_ip_rev(Aapl_t *aapl, uint addr);
EXT uint        aapl_get_lsb_rev(Aapl_t *aapl, uint addr);
EXT int         aapl_get_firmware_rev(Aapl_t *aapl, uint addr);
EXT int         aapl_get_firmware_build(Aapl_t *aapl, uint addr);
/* Functions to access client defined aapl data */
#ifndef MV_HWS_REDUCED_BUILD
EXT void        aapl_bind_set(Aapl_t *aapl, void *client_data );
EXT void       *aapl_bind_get(Aapl_t *aapl);
#endif /* MV_HWS_REDUCED_BUILD*/

EXT Avago_process_id_t     aapl_get_process_id(Aapl_t *aapl, uint addr);
EXT Avago_ip_type_t        aapl_get_ip_type(Aapl_t *aapl, uint addr);
EXT void                   aapl_set_ip_type(Aapl_t *aapl, uint addr);
EXT BOOL aapl_get_spico_running_flag(Aapl_t *aapl, uint addr);
EXT BOOL aapl_set_spico_running_flag(Aapl_t *aapl, uint addr, BOOL running);

#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
/* Query ASIC(s) to fully fill out the AAPL struct. Should be done before using. */
EXT void    aapl_get_ip_info(Aapl_t *aapl, int chip_reset);
#endif /* !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER */ 

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
/* Send / receive JTAG information. */
EXT char *avago_jtag(Aapl_t *aapl, int opcode, int bits, const char *data);
EXT char *avago_jtag_rd(Aapl_t *aapl, int opcode, int bits);
EXT void  avago_jtag_set_bit(Aapl_t *aapl, int opcode, uint bits, uint set_bit, uint value);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/* checks addr against ip_type or process. Args is the number of arguments passed in for ip_type or process */
/* for example: aapl_check_ip_type(aapl, addr, __func__,__LINE__, 2, AVAGO_SERDES, AVAGO_QPI); */
#ifndef MV_HWS_REDUCED_BUILD
EXT BOOL aapl_check_ip_type_full(       Aapl_t *aapl, uint addr, const char *caller, int line, int error, int args, ...);
EXT BOOL aapl_check_firmware_rev_full(  Aapl_t *aapl, uint addr, const char *caller, int line, int error, int args, ...);
EXT BOOL aapl_check_broadcast_address_full( Aapl_t *aapl, uint addr, const char *caller, int line, int error_on_match);
EXT BOOL aapl_check_process_full(       Aapl_t *aapl, uint addr, const char *caller, int line, int error, int args, ...);
#else
EXT BOOL aapl_check_ip_type_reduce(       Aapl_t *aapl, uint addr, int error, int args, ...);
EXT BOOL aapl_check_firmware_rev_reduce(  Aapl_t *aapl, uint addr, int error, int args, ...);
EXT BOOL aapl_check_broadcast_address_reduce( Aapl_t *aapl, uint addr, int error_on_match);
#endif

#ifndef MV_HWS_REDUCED_BUILD
EXT BOOL aapl_check_ip_type_exists(Aapl_t *aapl, uint addr, const char *caller, int line, int error, int args, ...);
#endif /* MV_HWS_REDUCED_BUILD*/

#ifndef MV_HWS_REDUCED_BUILD
EXT BOOL aapl_check_firmware_build(Aapl_t *aapl, uint addr, const char *caller, int line, int error, int args, ...);
#endif /* MV_HWS_REDUCED_BUILD */

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT void avago_system_chip_setup(Aapl_t *aapl, int reset, int chip);
#ifndef MV_HWS_REDUCED_BUILD
EXT uint avago_get_tap_gen(Aapl_t *aapl);
#endif
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */


/* Setup AAPL's connection to a device. */
/* If using an AVAGO_AACS_* communication method, this will setup a TCP connection. */
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT void aapl_connect(Aapl_t *aapl, const char *aacs_server, int tcp_port);
EXT void aapl_close_connection(Aapl_t *aapl);
EXT int  aapl_connection_status(Aapl_t *aapl);

EXT BOOL aapl_is_i2c_communication_method(Aapl_t *aapl);
EXT BOOL aapl_is_mdio_communication_method(Aapl_t *aapl);
EXT BOOL aapl_is_sbus_communication_method(Aapl_t *aapl);
EXT BOOL aapl_is_aacs_communication_method(Aapl_t *aapl);
EXT BOOL aapl_is_user_communication_method(Aapl_t *aapl);
EXT BOOL aapl_is_system_communication_method(Aapl_t *aapl);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

#ifdef MV_HWS_REDUCED_BUILD
EXT int  aapl_fail_reduce(Aapl_t *aapl);
#endif /* MV_HWS_REDUCED_BUILD */

/*////////////////////////////////////////////////////////////////////// */
/* AAPL Address functions: */

/** @defgroup Address IP Addressing Functions */
/** @{ */

/** @brief Internal hardware address structure. */
/** Not generally end-user accessible. */
/** Each value can be all ones to indicate broadcast to all at that level. */
/** Else the valid values are hardware dependent. */
typedef struct Avago_addr_t
{
    uint chip;   /**< Device number. */
    uint ring;   /**< Ring on device. */
    uint sbus;   /**< SBus address on ring. */
    uint lane;   /**< Lane at SBus address. */

    struct Avago_addr_t * next;  /**< Address of next Avago_addr_t element -- used to generate a linked list of addresses */
} Avago_addr_t;

/** @} */
/* ported from aapl 2.2.3 for AACS Server integration */
/** @brief AAPL addressing constants. */
typedef enum
{
    AVAGO_ADDR_BROADCAST      = 0xff,
    AVAGO_ADDR_IGNORE_LANE    = 0xf0,
    AVAGO_ADDR_QUAD_LOW       = 0xf1,
    AVAGO_ADDR_QUAD_HIGH      = 0xf2,
    AVAGO_ADDR_QUAD_ALL       = 0xf3
} Aapl_broadcast_control_t;

#define AVAGO_BROADCAST                (0xff)
#define AVAGO_INVALID_ADDR             (0xbad)
#define AVAGO_SBUS_MASTER_ADDRESS      (0xfd)
#define AVAGO_SBUS_CONTROLLER_ADDRESS  (0xfe)
#define AVAGO_SERDES_BROADCAST_ADDRESS (0xff)
#define AVAGO_MAKE_SERDES_BROADCAST_ADDRESS(addr) (avago_make_serdes_broacast_addr(addr))

EXT void avago_addr_init(Avago_addr_t *addr_struct);
#ifndef MV_HWS_REDUCED_BUILD
EXT uint avago_addr_init_broadcast(Avago_addr_t *addr_struct);
#endif /* MV_HWS_REDUCED_BUILD */
EXT BOOL avago_addr_to_struct(uint addr, Avago_addr_t *addr_struct);
EXT uint avago_struct_to_addr(Avago_addr_t *addr);
EXT uint avago_make_addr3(int chip, int ring, int sbus);
#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
EXT uint avago_make_sbus_master_addr(uint addr);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
EXT uint avago_make_sbus_controller_addr(uint addr);
EXT uint avago_make_serdes_broadcast_addr(uint addr);

#ifdef AAPL_ENABLE_INTERNAL_FUNCTIONS
EXT BOOL aapl_broadcast_first(Aapl_t *aapl, Avago_addr_t *addr, Avago_addr_t *start, Avago_addr_t *stop, Avago_addr_t *first);
EXT BOOL aapl_broadcast_next(Aapl_t *aapl, Avago_addr_t *addr, Avago_addr_t *start, Avago_addr_t *stop);

#ifndef MV_HWS_REDUCED_BUILD
#  if AAPL_ENABLE_FILE_IO
    EXT char *aapl_read_file(Aapl_t *aapl, char *filename);
#  endif
#endif /* MV_HWS_REDUCED_BUILD */

#endif

/*////////////////////////////////////////////////////////////////////// */
/* AAPL memory functions that log on allocation failure: */
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#ifndef aapl_malloc
EXT void *aapl_malloc(Aapl_t *aapl, const size_t bytes, const char *description);
EXT void *aapl_realloc(Aapl_t *aapl, void *ptr, const size_t bytes, const char *description);
EXT void  aapl_free(Aapl_t *aapl, void *ptr, const char *description);
#endif

/* MRVL taken LGPL aapl-2.2.1 */
EXT BOOL avago_serdes_error_reset(Aapl_t *aapl, uint addr);


/*////////////////////////////////////////////////////////////////////// */
/* AACS Functions and Globals */

/* Size of buffer for SBus commands. Largest command could be: */
/* sbus 00 00 00 0x00000000 */
/* NOTE: Currently no comments are sent, so this is the biggest possible command */
#define AAPL_SBUS_CMD_LOG_BUF_SIZE (32)

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
/* Always available, but may be empty: */
EXT const char *avago_aacs_send_command(        Aapl_t *aapl, const char *cmd);
EXT const char *avago_aacs_send_command_options(Aapl_t *aapl, const char *cmd, int recv_data_back, int strtol);
EXT void avago_aacs_flush(Aapl_t *aapl);

#if AAPL_ALLOW_AACS

EXT void avago_aacs_open(Aapl_t *aapl);
EXT void avago_aacs_close(Aapl_t *aapl);

# ifdef AAPL_ENABLE_INTERNAL_FUNCTIONS
    EXT const char *aapl_tcp_strerr(void);
    EXT int aapl_recv(Aapl_t *aapl);
    EXT void aapl_check_capabilities(Aapl_t *aapl);
# endif
#endif
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

# ifndef SWIG
    EXT const char *aapl_default_server;
    EXT const uint  aapl_default_port;
# endif

#if AAPL_ENABLE_DIAG
/** @brief Structure specifying what diagnostics to run. */
/**        Note that these functions are not part of the core functionality. */
typedef struct
{
    BOOL sbus_dump;             /**< Dump SBus */
    BOOL serdes_init_only;      /**< Stop after running SerDes init */
    BOOL state_dump;            /**< Dump SerDes state information */
    BOOL pmd_debug;             /**< Dump SerDes PMD tuning information */
    BOOL dma_dump;              /**< Dump LSB/ESB DMA data */
    BOOL dmem_dump;             /**< Dump SPICO DMEM data */
    BOOL imem_dump;             /**< Dump SPICO IMEM data */
    BOOL binary;                /**< display binary along with hex */
    BOOL columns;               /**< Display data in multiple columns */

    BOOL destructive;           /**< perform destructive tests */
    BOOL use_existing_divider;  /**< When performing destructive tests, use current divider */
    int cycles;                 /**< Number of cycles to check SPICO PC */
    int *dividers;              /**< When performing destructive tests, use this built in divider list (populated in the constructor) */

} Avago_diag_config_t;
#endif
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
#endif
