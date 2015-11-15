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


/** @file */
/** @brief   Customer level configuration file. */
/** @details aapl.h is one of two files we expect customers to configure. */
/**          The other is user_code.c. */
/** @details aapl.h consists mostly of defines taking a value of 0 or 1. */
/**          To disable functionality, set the define value to 0.  Do not */
/**          comment or remove the define as that will cause compile failures. */
/** @details aapl.h must be included to access AAPL functions.  Typically, */
/**          it's the only AAPL header a library user needs to include. */

#ifndef AAPL_H_
#define AAPL_H_

#ifdef MV_HWS_BIN_HEADER
#include "config_marvell.h"
#endif /* MV_HWS_BIN_HEADER */

#ifndef MV_HWS_REDUCED_BUILD
#define MV_HWS_REDUCED_BUILD
#endif /* MV_HWS_REDUCED_BUILD */
/*#define MV_HWS_REDUCED_BUILD_EXT_CM3*/

/* #define MV_HWS_AAPL_DEBUG_PRINT_ENABLE */

#ifdef MV_HWS_REDUCED_BUILD
/* AAPL reduced package build */
#ifdef WIN32
/* for WIN32 agr... should be arg, ...*/
#define aapl_log_printf(aapl, type, function, line, msg, arg, ...)
#define aapl_check_ip_type(aapl, addr, function, line, error, args, arg, ...) aapl_check_ip_type_reduce(aapl, addr, error, args, arg)
#define aapl_check_firmware_rev(aapl, addr, function, line, error, args, arg, ...) aapl_check_firmware_rev_reduce(aapl, addr,  error, args, arg)
#define  aapl_check_process(aapl, addr, function, line, error, args, arg, ...)  (TRUE)
#define aapl_fail(aapl, function, line, msg, arg, ...) aapl_fail_reduce(aapl)
#else /* WIN32*/
#if (!defined MV_HWS_BIN_HEADER) && (defined MV_HWS_AAPL_DEBUG_PRINT_ENABLE)
    #define aapl_log_printf(aapl, type, function, line, msg, arg...) aapl_log_printf_reduce(aapl, type, function, line, msg, arg)
#else
    #define aapl_log_printf(aapl, type, function, line, msg, arg...)
#endif
#define aapl_check_ip_type(aapl, addr, function, line, error, args, arg...) aapl_check_ip_type_reduce(aapl, addr, error, args, arg)
#define aapl_check_firmware_rev(aapl, addr, function, line, error, args, arg...) aapl_check_firmware_rev_reduce(aapl, addr,  error, args, arg)
#define  aapl_check_process(aapl, addr, function, line, error, args, arg...)  (TRUE)
#define aapl_fail(aapl, function, line, msg, arg...) aapl_fail_reduce(aapl)
#endif
#define aapl_check_broadcast_address(aapl, addr, function, line, error_on_match) aapl_check_broadcast_address_reduce(aapl, addr, error_on_match)
#define avago_spico_int_check(aapl, function, line, addr, int_num, param) avago_spico_int_check_reduce(aapl, addr, int_num, param)
#else /* MV_HWS_REDUCED_BUILD */
/* AAPL full package build */
#define aapl_log_printf(aapl, type, function, line, msg, arg...) aapl_log_printf_full(aapl, type, function, line, msg, arg)
#define aapl_fail(aapl, function, line, msg, arg...) aapl_fail_full(aapl, function, line, msg, arg)
#define aapl_check_ip_type(aapl, addr, function, line, error, args, arg...) aapl_check_ip_type_full(aapl, addr, function, line, error, args, arg)
#define aapl_check_broadcast_address(aapl, addr, function, line, error_on_match) aapl_check_broadcast_address_full(aapl, addr, function, line, error_on_match)
#define avago_spico_int_check(aapl, function, line, addr, int_num, param) avago_spico_int_check_full(aapl, function, line, addr, int_num, param)
#define aapl_check_firmware_rev(aapl, addr, function, line, error, args, arg...) aapl_check_firmware_rev_full(aapl, addr, function, line, error, args, arg)
#define  aapl_check_process(aapl, addr, function, line, error, args, arg...)  aapl_check_process_full(aapl, addr, function, line, error, args, arg)
#endif  /* MV_HWS_REDUCED_BUILD */

#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
#ifndef AVAGO_FIRMWARE_PATH
/** @brief This optional define provides a shortcut for accessing firmware */
/**        by [fw_rev] and [build_id] rather than only by full path. */
/** */
/** It points to a directory tree organized as follows: */
/** */
/**  #AVAGO_FIRMWARE_PATH/serdes/[fw_rev]/serdes.[fw_rev]_[build_id].rom */
/** */
/**  #AVAGO_FIRMWARE_PATH/serdes/[fw_rev]/serdes.[fw_rev]_[build_id].swap */
/** */
/**  #AVAGO_FIRMWARE_PATH/sbus_master/[fw_rev]/sbus_master.[fw_rev]_[build_id].rom */
#define AVAGO_FIRMWARE_PATH "/firmware/"
#endif

/* The AAPL_ALLOW_* defines enable/disable code sections of AAPL. */
/* To remove an unneeded code section from AAPL, set the define */
/*      value to 0. Do not comment or remove a define as that may */
/*      cause compile failures. */

#if !defined(MV_HWS_BIN_HEADER) && !defined(CPSS_BLOB)
#define AAPL_ALLOW_AACS                1  /**< Set to 0 to remove AVAGO_AACS_SBUS, AVAGO_AACS_MDIO, and AVAGO_AACS_I2C communication methods. */
#else
#define AAPL_ALLOW_AACS                0  /**< Set to 0 to remove AVAGO_AACS_SBUS, AVAGO_AACS_MDIO, and AVAGO_AACS_I2C communication methods. */
#endif /* MV_HWS_BIN_HEADER */
#define AAPL_ALLOW_GPIO_MDIO           0  /**< Set to 0 to remove the AVAGO_GPIO_MDIO communication method. */
#define AAPL_ALLOW_OFFLINE_SBUS        0  /**< Set to 0 to remove the AVAGO_OFFLINE communication method. */
#define AAPL_ALLOW_SYSTEM_I2C          0  /**< Set to 0 to remove the AVAGO_SYSTEM_I2C communication method. */
#define AAPL_ALLOW_SYSTEM_MDIO         0  /**< Set to 0 to remove the AVAGO_SYSTEM_MDIO communication method (not implemented). */
#define AAPL_ALLOW_USER_SUPPLIED_I2C   0  /**< Set to 0 to remove the AVAGO_USER_SUPPLIED_I2C communication method. */
#define AAPL_ALLOW_USER_SUPPLIED_MDIO  0  /**< Set to 0 to remove the AVAGO_USER_SUPPLIED_MDIO communication method. */
#define AAPL_ENABLE_USER_SERDES_INT    0  /**< Set to 0 to remove support for using the user_supplied_serdes_interrupt_function() for SerDes interrupts */
#define AAPL_ALLOW_USER_SUPPLIED_SBUS  1  /**< Set to 0 to remove the AVAGO_USER_SUPPLIED_SBUS_DIRECT communication method. */
#endif /* !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER */

#if !defined(MV_HWS_REDUCED_BUILD_EXT_CM3) && !defined(CPSS_BLOB)
/* Set value to 0 to disable corresponding feature support: */
#define AAPL_ENABLE_AACS_SERVER        1  /**< Enable the AACS server. */
#define AAPL_ENABLE_FILE_IO            1  /**< Enable use of file IO and the FILE type. */
#else
#define AAPL_ENABLE_AACS_SERVER        0  /**< Enable the AACS server. */
#define AAPL_ENABLE_FILE_IO            0  /**< Enable use of file IO and the FILE type. */
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
#define AAPL_ENABLE_C_LINKING          1  /**< Set to 0 if library and callers all are C++ compiled. */

/** @brief   Defines the available communication methods. */
/** @details AAPL supports several methods for communicating with Avago IP. */
/**          The communication_method member of the aapl structure defines */
/**          which is used by the library, and can be changed at run time. */
/** */
/**          The default value compiled into the library is configured using */
/**          the AAPL_DEFAULT_COMM_METHOD define in the aapl.h header file. */
/** */
/**          Note also that several AAPL_ALLOW_* defines configure which */
/**          methods are compiled into the library. */
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
typedef enum
{
    AVAGO_AACS_SBUS,                /**< Use the AACS TCP protocol */
    AVAGO_AACS_MDIO,                /**< Send commands using MDIO commands via the AACS TCP protocol */
    AVAGO_AACS_I2C,                 /**< Send commands using I2C commands via the AACS TCP protocol */
    AVAGO_USER_SUPPLIED_I2C,        /**< Use the user_supplied_i2c_write_function() and user_supplied_i2c_read_function() from user_code.c */
    AVAGO_USER_SUPPLIED_MDIO,       /**< Use the user_supplied_mdio_function() from user_code.c */
    AVAGO_USER_SUPPLIED_SBUS_DIRECT,/**< Use the user_supplied_sbus_function() from user_code.c */
    AVAGO_SYSTEM_I2C,               /**< Send commands using the Linux /dev/i2c-1 device. */
    AVAGO_SYSTEM_MDIO,              /**< Currently not implemented */
    AVAGO_GPIO_MDIO,                /**< Example of using bit banged GPIO to communicate with MDIO */
    AVAGO_OFFLINE                   /**< Run AAPL in offline mode. Some SBus commands will be emulated. */
} Aapl_comm_method_t;

/** @brief Set the default AAPL communication_method value. */
/** @details The AAPL communication method actually used to communicate with */
/**          devices is controlled by Aapl_t::communication_method (of type */
/**          Aapl_comm_method_t).  This */
/**          define simply sets the compiled in default value for this field. */
/** @details Note: The AAPL_ALLOW_* defines determine if the code for */
/**          specific communication methods is compiled into the library. */
#define AAPL_DEFAULT_COMM_METHOD         AVAGO_AACS_SBUS

/** @brief The default value for Aapl_t::i2c_base_addr. */
/** The base I2C address for Avago devices is usually 0x40. */
/** Normally, this is set to the address of the first Avago device. */
#define AAPL_DEFAULT_I2C_BASE_ADDR       0x48

/** @brief The default value for Aapl_t::mdio_base_port_addr. */
/** Normally, this is set to the address of the first Avago device. */
#define AAPL_DEFAULT_MDIO_BASE_PORT_ADDR    0

#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
/* AAPL_PROCESS_ID_OVERRIDE allows the user to override the process ID. */
/* If AAPL is not using AACS to communicate with the device, you should */
/* specify this here, otherwise AAPL will assume the process type */
/* AVAGO_PROCESS_F and issue a warning. (AACS can auto determine the process type, */
/* whereas other communication methods can not.) */

/*#define AAPL_PROCESS_ID_OVERRIDE AVAGO_PROCESS_F */

/** @brief Allows the user to override the number of devices with which */
/**        AAPL will communicate.  The value 0 indicates that AAPL should */
/**        auto-discover this value, if possible. */
/** */
/** If AAPL is not using AACS to communicate with the device, you should */
/** specify this here, otherwise AAPL will assume one device */
/** and issue a warning. */

#define AAPL_NUMBER_OF_CHIPS_OVERRIDE 0


/** @brief Allows the user to override the number of SBus rings with which */
/**        AAPL will communicate.  The value 0 indicates that AAPL should */
/**        auto-discover this value, if possible. */
/** */
/** If AAPL is not using AACS to communicate with the device, you should */
/** specify this here, otherwise AAPL will assume one SBus ring. */

#define AAPL_NUMBER_OF_RINGS_OVERRIDE 0

/* The AAPL_CHIP_ID_HEX_OVERRIDEn defines JTAG IDCODES for devices AAPL will be */
/* communicating with. If AAPL is using an AVAGO_AACS_* communication method */
/* these IDCODES will be auto-discovered. */
/* If you are not using an AVAGO_AACS_* communication method you should define */
/* AAPL_CHIP_ID_HEX_OVERRIDE0 through AAPL_CHIP_ID_HEX_OVERRIDEn, where n is */
/* the number of devices AAPL will be communicating with. */
/* You must always start at AAPL_CHIP_ID_HEX_OVERRIDE0. */
/* WARNING: The values below are examples, and are not valid IDCODEs. */
/*   The user should contact Avago if they do not know the proper  */
/*   IDCODE value to use here. */
/*#define AAPL_CHIP_ID_HEX_OVERRIDE0 0xaaaaaaaa */
/*#define AAPL_CHIP_ID_HEX_OVERRIDE1 0xaaaaaaaa */
/*... */
/*#define AAPL_CHIP_ID_HEX_OVERRIDE14 0xaaaaaaaa */
/* */
/* Id codes can also be entered as binary strings, using the following defines: */
/* */
/*#define AAPL_CHIP_ID_OVERRIDE0  "10101010101010101010101010101010" */
/*#define AAPL_CHIP_ID_OVERRIDE1  "10101010101010101010101010101010" */
/*.... */
/*#define AAPL_CHIP_ID_OVERRIDE14 "10101010101010101010101010101010" */
#define AAPL_CHIP_ID_OVERRIDE0 "00001001000100100101010101111111"
/* */
/* If both are given, the hex values will be used. */


/** Maximum number of devices AAPL supports. */
/** Reducing this value to that actually used will reduce the memory footprint of the Aapl_t struct. */
/** Valid range: [1-15]. */
#define AAPL_MAX_CHIPS 1

/** Maximum number of SBus rings AAPL supports. */
/** Reducing this value to that actually used will reduce the memory footprint of the Aapl_t struct. */
/** Valid range: [1-15]. */
#define AAPL_MAX_RINGS 1

/** Maximum number of AACS commands to queue before send. */
/** Setting this to 0 disables command buffering. */
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#define AAPL_MAX_CMDS_BUFFERED         1000
#define AAPL_SBUS_MDIO_TIMEOUT         100  /**< Maximum number of reads of the SBUS_RESULT register in sbus-over-mdio mode */
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
#define AAPL_SERDES_INT_TIMEOUT        500 /**< Maximum number of SBus reads to check for completion of SPICO interrupt command */
#define AAPL_SERDES_INIT_RDY_TIMEOUT   20  /**< Maximum milliseconds for pll calibration */
#define AAPL_SPICO_UPLOAD_WAIT_TIMEOUT 500  /**< Maximum milliseconds to wait for AAPL to wait for external SPICO upload to complete */

#if defined  MV_MSYS_BOBK
#define AVAGO_MG_ACCESS_THRESHOLD      5000  /**< Maximum "mop" to wait fbefore any MG access */
#elif defined MV_CM3_BOBK
#define AVAGO_MG_ACCESS_THRESHOLD      0     /**< Maximum "mop" to wait fbefore any MG access */
#else
#define AVAGO_MG_ACCESS_THRESHOLD      12000 /**< Maximum "mop" to wait fbefore any MG access */
#endif

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#define AAPL_I2C_HARD_RESET_TIMEOUT    100  /**< Maximum number of commands to send after a hard I2C reset to wait for bus to come back up */

/* */
/* Logging defines: */
/* */
/* The following table describes when and where messages get logged to the Aapl_t::log */
/*      and/or printed to an I/O stream. */
/* */
/*                     AAPL_STREAM           AAPL_STREAM_ERR                    Aapl_t::log */
/*                     ------------          ----------------                   -------------- */
/*   AVAGO_MEM_LOG         never                  never                         always written */
/* */
/*   AVAGO_DEBUGn   (#ifdef AAPL_STREAM &&        never                       (Aapl_t::enable_debug_logging */
/*                  Aapl_t::enable_stream_logging                                  && n <= Aapl_t::debug) */
/*                  && n <= Aapl_t::debug) */
/* */
/*   AVAGO_INFO     (#ifdef AAPL_STREAM &&        never                         always written */
/*                  Aapl_t::enable_stream_logging) */
/* */
/*   AVAGO_WARNING         never           (#ifdef AAPL_STREAM_ERR &&           always written */
/*                                          Aapl_t::enable_stream_err_logging) */
/*   AVAGO_ERR             never           (#ifdef AAPL_STREAM_ERR &&           always written */
/*                                          Aapl_t::enable_stream_err_logging) */
/* */
/*   NOTE: If a USER_SUPPLIED_LOGGING_FUNCTION has been provided, then data will be */
/*   sent there instead of being placed into AAPL logs. */

#ifndef CPSS_BLOB
/** Sets the stdio stream to write debug and info messages. */
/** @see aapl_log_printf(). */
/** */
/** Comment out, or set the enable_stream_logging member of aapl to false, */
/**   to suppress this type of output. */
/** */
/** If #AAPL_STREAM and #AAPL_STREAM_ERR are both undefined, then be sure */
/**   #AAPL_DEFAULT_ENABLE_DEBUG_LOGGING is defined as 1. */
/**   Otherwise, all DEBUG messages will be lost. */

/** Comment out #AAPL_STREAM and #AAPL_STREAM_ERR defines to eliminate use of */
/**    fprintf in logging functions, in which case, all logging is */
/**    into memory referenced by the aapl structure.  The user */
/**    should provide some means to read/manange this space. */
/** */
/** @see AAPL_STREAM_ERR */
/** */
#define AAPL_STREAM stdout

/** Sets the stdio stream to write warning and error messages. */
/** @see aapl_log_printf(). */
/** */
/** Comment out, or set the enable_stream_err_logging member of aapl to false, */
/**   to suppress this type of output. */
/** */
/** @see AAPL_STREAM */
/** */
#define AAPL_STREAM_ERR stderr
#endif

/** @brief Default value for Aapl_t::enable_debug_logging. */
/** This value also can be changed at run time. */
/** */
#define AAPL_DEFAULT_ENABLE_DEBUG_LOGGING    0

/** @brief Default value for Aapl_t::enable_stream_logging. */
/** This value also can be changed at run time. */
/** */
#define AAPL_DEFAULT_ENABLE_STREAM_LOGGING       0

/** @brief Default value for Aapl_t::enable_stream_err_logging. */
/** This value also can be changed at run time. */
/** */
#define AAPL_DEFAULT_ENABLE_STREAM_ERR_LOGGING   0

/** @brief Default value for Aapl_t::serdes_core_port_interrupt */
/** This value also can be changed at run time. */
/** Set to 0 to send SerDes interrupts via SBus */
/** Set to 1 to send SerDes interrupts via the ASIC core interface */
/** Set to 2 to send SerDes interrupts via the Avago LE interface (for Avago testchips only) */

#define AAPL_DEFAULT_SERDES_CORE_PORT_INT 0

/** Default for Aapl_t::log_time_stamps. */
/** If defined, this value also can be changed at run time. */
/** */
/** Comment out the define to remove time stamp logging from the compile. */
/** Set to 0 to compile in with a disabled default. */
/** Set to 1 to compile in with an enabled default. */
/** */
#define AAPL_LOG_TIME_STAMPS 1


/** Only used when calling the aacs_server() function */
#define AACS_SERVER_BUFFERS 4096


/** Buffer used by aapl_log_printf(). */
/** If too small, output will be truncated. */
#define AAPL_LOG_PRINTF_BUF_SIZE 4096

/** Used by AAPL for the log, data_char, and debug buffers. */
/** Also used by recv. This number should always be >= the max size */
/** the AACS server will return. */
/** */
/** AAPL's AACS server can return up to AAPL_MAX_CMDS_BUFFERED * AAPL_SBUS_CMD_LOG_BUF_SIZE. */
/** */
#define AAPL_LOG_BUF_SIZE 1024
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/* Prototypes: */
/*#define USER_SPECIFIED_LOGGING_FUNCTION(buffer,len) user_specified_logging_function(buffer,len) */
/*#define USER_SPECIFIED_LOGGING_FUNCTION3(aapl,buffer,len) user_specified_logging_function(aapl,buffer,len) */
/*#define USER_SPECIFIED_LOGGING_FUNCTION4(aapl,level,buffer,len) user_specified_logging_function(aapl,level,buffer,len) */
/* Examples: */
/*#define USER_SPECIFIED_LOGGING_FUNCTION(buffer,len) fwrite(buffer, 1, len, stdout) */
/*#define USER_SPECIFIED_LOGGING_FUNCTION3(aapl,buffer,len) fwrite(buffer, 1, len, Aapl_t::client_data) */
/*#define USER_SPECIFIED_LOGGING_FUNCTION4(aapl,level,buffer,len) if(level==AVAGO_ERR) fwrite(buffer, 1, len, stdout) */



/* */
/* Portability defines: */
/* */

#define HAVE_UINT 0  /**< Set to 1 if your system headers already have the uint typedef. */

/* Add additional defines here to replace any library */
/* functions your build environment doesn't support. */

/*#define strtoul(a,b,c) simple_strtol(a,b,c) */

/* Uncomment and edit if the default ms_sleep doesn't work for you. */
/*#define MS_SLEEP(milliseconds) udelay(milliseconds*1000) */
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#define AAPL_EXIT(val)       exit(val)          /**< AAPL uses this for exit. */

/* All malloc/realloc/free calls make use of these macros: */
/* */
#define AAPL_MALLOC(sz)      malloc(sz)         /**< AAPL uses this for malloc. */
#define AAPL_REALLOC(ptr,sz) realloc(ptr,sz)    /**< AAPL uses this for realloc. */
#define AAPL_FREE(ptr)       free(ptr)          /**< AAPL uses this for free. */

#if 0
/* Use this code to replace the aapl_{malloc/realloc/free} error */
/*   tracing calls with straight malloc versions. */
/* */
# define aapl_malloc(a,sz,c)      AAPL_MALLOC(sz)
# define aapl_realloc(a,ptr,sz,c) AAPL_REALLOC(ptr,sz)
# define aapl_free(a,ptr,c)       AAPL_FREE(ptr)
#endif
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */


/* AAPL required files: */
#include "system_includes.h"
#include "aapl_library.h"
#include "aapl_core.h"
#include "sbus.h"
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#include "mdio.h"
#include "gpio_mdio.h"
#include "i2c.h"
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
#include "spico.h"
#include "serdes_core.h"
#include "pmd.h"
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#include "logging.h"
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/* Clear warning */
extern int snprintf(char *str, size_t size, const char *format, ...);
extern int vsnprintf(char *str, size_t size, const char *format, va_list ap);

#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
#include "../marvell/marvell_API_mapping.h"
#endif

#endif
