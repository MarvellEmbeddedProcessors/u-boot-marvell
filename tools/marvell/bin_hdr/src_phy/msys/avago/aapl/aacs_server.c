/* AAPL CORE Revision: 2.2.3-beta */

/* Copyright 2014-2015 Avago Technologies. All rights reserved.
 *
 * This file is part of the AAPL CORE library.
 *
 * AAPL CORE is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AAPL CORE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with AAPL CORE.  If not, see http://www.gnu.org/licenses.
 */


/** Doxygen File Header
 ** @file
 ** @brief Implementation of AACS Server functionality.
 **/

#if !defined __MINGW32__ && !defined ASIC_SIMULATION
#include <arpa/inet.h>
#endif

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"

#define BUFLEN 4096  /* for local I/O buffers. */

char cmd[BUFLEN];

#define EQCN(str1,str2,len) (! aapl_strncasecmp(str1, str2, len))
#define EOS               '\0'
#define ISNUL(cp)         (*(cp) == EOS)
#define ISEOL(cp)         ((*(cp) == '#') || ISNUL(cp))
#define ISTERM(cp)        (isspace(*(cp)) || ISEOL(cp))
#define SKIPSPACE(cp)     while (isspace(*(cp))) ++(cp)
#define ISCMD(cp,cmd,len) (EQCN(cp, cmd, len) && ISTERM((cp) + len))


#define RES_CLOSE "(close)"
#define RES_EXIT  "(exit)"


#if AAPL_ENABLE_DIAG
#define RES_HELP  "Valid commands are: sbus, jtag, i2c, set_debug, chips, chipnum, version, status, send, help, close, exit, spico_int, commands, sleep, diag, sbus_reset. Command batching supported."
#else
#define RES_HELP  "Valid commands are: sbus, jtag, i2c, set_debug, chips, chipnum, version, status, send, help, close, exit, spico_int, commands, sleep, sbus_reset. Command batching supported."
#endif

/** @details     Parse a hex number and return the equivalent binary number,
 **              with *endp modified to the first char after the token.
 ** @param cp    String that should be all hex digits.
 ** @param endp  Pointer to string to return.
 ** @param min   Minimum token length in chars (must be >= 1).
 ** @param max   Maximum token length in chars.
 ** @return uint Binary equivalent, or 0 for any error, with *endp set to the
 **              original cp (as a failure flag).
 **/

static uint aapl_num_from_hex(const char *cp, const char **endp, int min, int max)
{
    uint result = 0;
    const char * start = *endp = cp;

    while (isxdigit(*cp))
    {
        if (cp - start >= max) return(0);
        result = (result << 4) | (uint)(isdigit(*cp) ? (*cp - '0') : (10 + tolower(*cp) - 'a'));
        ++cp;
    }
    if ((cp - start < min) || (! ISTERM(cp))) return(0);
    *endp = cp;
    return(result);
}

/** @details     Parse an ASCII binary number possibly containing 'x'/'X'
 **              digits, and return the equivalent binary number, with *endp
 **              modified to the first char after the token.
 ** @param cp    String that should be all '0'/'1'/'x'/'X' digits.
 ** @param endp  Pointer to string to return.
 ** @param min   Minimum token length in chars (must be >= 1).
 ** @param max   Maximum token length in chars.
 ** @param maskp Pointer to a 32-bit mask value to set, default = all 1s, but
 **              returns 0s for 'x'/'X' meaning bits not to be changed
 **              (read/modify/write).
 ** @return uint Binary equivalent, or 0 for any error, with *endp set to the
 **              original cp (as a failure flag); and with *maskp (always)
 **              modified to reflect the data value.  In case of an empty
 **              string (following a data value consisting of just "z"),
 **              assuming min >= 1, returns 0 with an apparent error, but with
 **              mask = 0, and the caller can proceed from there.
 **/

static uint aapl_num_from_bin(const char *cp, const char **endp, int min, int max, uint *maskp)
{
    uint result = 0;
    uint mask   = 0;
    const char * start = *endp = cp;

    while ((*cp == '0') || (*cp == '1') || (*cp == 'x') || (*cp == 'X'))
    {
        if (cp - start >= max) return(0);

        if ((*cp == '0') || (*cp == '1'))
        {
            result = (result << 1) | (uint)(*cp - '0');
            mask = (mask << 1) | 1;
        }
        else
        {
            result <<= 1;
            mask <<= 1;
        }
        ++cp;
    }
    *maskp = mask;

    if ((cp - start < min) || (! ISTERM(cp))) return(0);
    *endp = cp;
    return(result);
}



#define FAILRET(rc,fd1,fd2) {if ((fd1) >= 0) close(fd1); if ((fd2) >= 0) close(fd2); return rc;}




static void cmd_error(Aapl_t *aapl, char *result, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(result, fmt, ap);

    aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "%s\n", result);

    memmove(result+7, result, strlen(result)+1);
    memcpy(result, "ERROR: ", 7);
    va_end(ap);
}




static BOOL sbus_parse_da_cmd(Aapl_t *aapl, const char * cmd, const char ** cpp, uint * valp, const int argnum, const char * valname, char * result)
{
    const char * cp2;
    uint val = aapl_num_from_hex(*cpp, &cp2, 2, 2);

    if (cp2 == *cpp)
    {
        cmd_error(aapl, result, "Invalid \"sbus\" command ignored: Arg %d = <%s> must be 2 hex digits. Got: \"%s\".", argnum, valname, cmd);
        return(FALSE);
    }
    SKIPSPACE(cp2);
    *cpp  = cp2;
    *valp = val;
    return(TRUE);
}


static int sbus_parse_data(Aapl_t *aapl, const char * cmd, const char ** cpp, uint * datap, uint * maskp, char * result)
{
    const char * cp = *cpp;
    const char * cp2;
    int  reslen;
    uint data = 0;




    if ((*cp == 'z') || (*cp == 'Z'))
    {
        data = aapl_num_from_bin(++cp, &cp2, 1, 31, maskp);

        if (cp == cp2)
        {
            if (! ISTERM(cp + 1))
            {
                cmd_error(aapl, result, "Invalid \"sbus\" command ignored. Arg 4 = <data> 'z' prefix must be followed by 0-31 '0'/'1'/'x'/'X' chars, but got: \"%s\".", cmd);
                return(0);
            }
        }

        reslen = 32;
    }


    else
    {
        int len;
        *maskp = 0xffffffff;

        for (cp2 = cp; isxdigit(*cp2) || (*cp2 == 'x') || (*cp2 == 'X'); ++cp2)
        /* null */;

        len = cp2 - cp;

        if      ( len ==  2) data = aapl_num_from_hex(cp, &cp2,  2,  2);
        else if ( len ==  8) data = aapl_num_from_bin(cp, &cp2,  8,  8, maskp);
        else if ((len == 10) && EQCN(cp, "0x", 2))
                   {cp += 2; data = aapl_num_from_hex(cp, &cp2,  8,  8);}
        else if ( len == 32) data = aapl_num_from_bin(cp, &cp2, 32, 32, maskp);
        else cp2 = cp;

        if (cp == cp2)
        {
            cmd_error(aapl, result, "Invalid \"sbus\" command ignored. Arg 4 = <data> must be 2-hex, 8-bin, 0x-8-hex, 32-bin, or \"z\" prefix variable-length, but got: \"%s\".", cmd);
            return(0);
        }
        reslen = ((len <= 8) ? 8 : 32);
    }
    *cpp   = cp2;
    *datap = data;
    return(reslen);

}



static BOOL check_term(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (!cp || ISTERM(cp)) return(TRUE);

    cmd_error(aapl, result, "Unexpected extra word(s) on command line, ignored. Got: \"%s\".", cmd);
    return(FALSE);
}


static void sbus_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result, int * chip_nump, int * ring_nump)
{
    const char * cp2;
    uint new_chip = 0x10;
    uint new_ring = 0x10;
    uint sa;
    uint data_out;
    uint da, scmd, data, mask = 0xffffffff;

    int reslen;

    SKIPSPACE(cp);

    if (ISEOL(cp))
    {
        cmd_error(aapl, result, "Invalid \"sbus\" command ignored. Must be: \"sbus <sa> <da> <cmd> <data> [# <comment>]\".");
        return;
    }


    sa = aapl_num_from_hex(cp, &cp2, 2, 4);

    if (cp2 == cp)
    {
        cmd_error(aapl, result, "Invalid \"sbus\" command ignored: Arg 1 = <sa> must be 2-4 hex digits. Got: \"%s\".", cmd);
        return;
    }
    if (cp2 - cp == 4) new_chip = (sa & 0xf000) >> 12;
    if (cp2 - cp >= 3) new_ring = (sa & 0x0f00) >>  8;
    cp = cp2; SKIPSPACE(cp);


    if (! (sbus_parse_da_cmd(aapl, cmd, &cp, &da,   2, "da",  result)
        && sbus_parse_da_cmd(aapl, cmd, &cp, &scmd, 3, "cmd", result)
        && ((reslen = sbus_parse_data(aapl, cmd, &cp, &data, &mask, result)))))
    {return;}


    if (! check_term(aapl, cmd, cp, result)) return;
    if (new_chip < 0x10) *chip_nump = new_chip;
    if (new_ring < 0x10) *ring_nump = new_ring;


    sa |= (*chip_nump << 12) | (*ring_nump << 8);
    aapl_get_return_code(aapl);

    data_out = ((scmd == 1 && mask != 0xffffffff) ? avago_sbus_rmw(aapl, sa, da, data, mask) :
                (scmd == 1)                       ? avago_sbus_wr( aapl, sa, da, data) :
                (scmd == 2)                       ? avago_sbus_rd( aapl, sa, da) :
                                                    avago_sbus(    aapl, sa, da, scmd, data, /* recv_data_back = */ 1));


    if (aapl_get_return_code(aapl) < 0)
    {
        cmd_error(aapl, result, "Command failed: \"%s\" => \"%s\".", cmd, aapl->data_char);
        return;
    }
    aapl_hex_2_bin(result, data_out, /* underscore_en = */ 0, /* bits */ 32);
    if (reslen == 8) strcpy(result, result + 24);

}


#if AAPL_ALLOW_I2C || AAPL_ALLOW_SYSTEM_I2C
static void i2c_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    typedef enum { cmd_read = 0, cmd_write, cmd_writeread} i2c_cmd_t;
    unsigned int i2c_address;
    const char *cp2;
    unsigned char buffer[256];
    i2c_cmd_t command;
    int num_bytes_r = 0, num_bytes = 0;

    SKIPSPACE(cp);
    if( *cp == '\0' )
    {
BadI2cCommand:
        sprintf(result, "ERROR: Invalid \"i2c\" command ignored. Must be: "
        "\"i2c r <addr> <num_bytes> [# <comments>]\", "
        "\"i2c w <addr> <byte>[ <byte>...] [# <comments>]\", or "
        "\"i2c wr <addr> <num_bytes> <byte>[ <byte>...] [# <comments>]\". "
        "Got: \"%s\".", cmd);
        return;
    }

    if(      (cp[0] == 'r' || cp[0] == 'R') && isspace(cp[1]) )
        command = cmd_read;
    else if( (cp[1] == 'r' || cp[1] == 'R') &&
             (cp[0] == 'w' || cp[0] == 'W') && isspace(cp[2]) )
    {
        cp++;
        command = cmd_writeread;
    }
    else if( (cp[0] == 'w' || cp[0] == 'W') && (isspace(cp[1]) || cp[1]=='\0') )
        command = cmd_write;
    else
    {
        sprintf(result, "ERROR: Invalid \"i2c\" command ignored. Arg 1 must be \"r\", \"w\", or \"wr\".  Got \"%s\".", cmd);
        return;
    }
    cp++; SKIPSPACE(cp);

    i2c_address = aapl_num_from_hex(cp, &cp2, 1, 2);
    if( cp2 == cp || i2c_address > 0x7f )
    {
        sprintf(result, "ERROR: Invalid \"i2c\" command ignored. Arg 2 must be a hex I2C address in the range 00..7f. Got: \"%s\".", cmd);
        return;
    }
    cp = cp2; SKIPSPACE(cp);

    if( *cp == '\0' )
        goto BadI2cCommand;

    if( command == cmd_read || command == cmd_writeread )
    {
        num_bytes_r = (int) strtol(cp, (char **)&cp2, 10);
        if( cp2 == cp || !(isspace(*cp2) || *cp2 == '\0') || num_bytes_r < 0 || num_bytes_r > (int)sizeof(buffer) )
        {
            sprintf(result, "ERROR: Invalid \"i2c %s\" command ignored. Arg 3 must be a number of bytes in the range 1..%d. Got: \"%s\".",
                            command==cmd_read ? "r" : "wr", (int)sizeof(buffer), cmd);
            return;
        }
        cp = cp2; SKIPSPACE(cp);
    }

    if( command == cmd_write || command == cmd_writeread )
    {
        while( *cp )
        {
            int value;
            if( num_bytes >= (int)sizeof(buffer) )
            {
                sprintf(result, "ERROR: Invalid \"i2c %s\" command ignored, too many bytes specified, limit is %d. Got: \"%s\".",
                                command==cmd_write ? "w" : "wr", (int)sizeof(buffer), cmd);
                return;
            }
            value = aapl_num_from_hex(cp, &cp2, 1, 2);
            if( cp2 == cp || value > 0xff )
            {
                sprintf(result, "ERROR: Invalid \"i2c %s\" command ignored. Arg %d must be a hex byte value in the range 00..ff. Got: \"%s\".",
                                command==cmd_write ? "w" : "wr", 3 + num_bytes, cmd);
                return;
            }
            cp = cp2; SKIPSPACE(cp);
            buffer[num_bytes++] = value;
        }
        if( num_bytes == 0 )
        {
            sprintf(result, "ERROR: Invalid \"i2c %s\" command ignored. "
                            "Must specify one or more bytes to send. "
                            "Invoke with no arguments for a usage summary.",
                                command==cmd_write ? "w" : "wr");
            return;
        }
    }

    if( command == cmd_write || command == cmd_writeread )
    {
        if( avago_i2c_write(aapl, i2c_address, num_bytes, buffer) < 0 )
        {
            sprintf(result, "ERROR: i2c command \"%s\" failed.", cmd);
            return;
        }
        if( command == cmd_write )
            sprintf(result, "Address 0x%02x, bytes written: %d.", i2c_address, num_bytes);
    }
    if( command == cmd_read || command == cmd_writeread )
    {
        int i;
        char *ptr = result;
        if( avago_i2c_read(aapl, i2c_address, num_bytes_r, buffer) < 0 )
        {
            sprintf(result, "ERROR: i2c command \"%s\" failed.", cmd);
            return;
        }
        for( i = 0; i < num_bytes_r; i++ )
            ptr += sprintf(ptr, "%02x ", buffer[i]);
        ptr[-1] = '\0';
    }
}
#endif

static void mdio_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    (void)aapl;
    (void)cmd;
    (void)cp;
    sprintf(result, "ERROR: The mdio command is not implemented.\n");
}


static void jtag_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result, int * chip_nump)
{
    const char my_string[] = "32";

    SKIPSPACE(cp); if (0 != strncmp( cp, my_string,   2)) goto BadJtagCmd; cp += 2;
    SKIPSPACE(cp); if (! EQCN(cp, "02b6", 4)) goto BadJtagCmd; cp += 4;
    SKIPSPACE(cp); if (*cp != '0')            goto BadJtagCmd; ++cp;
    while (*cp == '0') ++cp;
    if (! check_term(aapl, cmd, cp, result)) return;

    aapl_hex_2_bin(result, aapl->jtag_idcode[*chip_nump], 0, 32);
    return;

    BadJtagCmd:
    cmd_error(aapl, result, "Unsupported \"jtag\" command ignored. Only \"jtag 32 02b6 0...\" (one or more 0s) is allowed. Got: \"%s\".", cmd);
}


static void chips_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        sprintf(result, "%d", aapl->chips);
}


static void chipnum_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result, int * chip_nump)
{
    const char *cp2;
    uint num;

    SKIPSPACE(cp);
    if (ISEOL(cp)) {sprintf(result, "%d", *chip_nump); return;}

    cp2 = cp;
    num = aapl_num_from_hex(cp, &cp2, 1, 1);

    if (cp2 == cp)
        cmd_error(aapl, result, "Invalid \"chipnum\" command ignored: Parameter must be a single hex digit. Got: \"%s\".", cmd);
    else if (num >= aapl->chips)
        cmd_error(aapl, result, "Invalid \"chipnum\" command ignored: Max chip number is 0x%x. Got: \"%s\".", aapl->chips - 1, cmd);
    else {*chip_nump = num; result[0] = *cp; result[1] = '\0';}
}

static void set_debug_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    const char *cp2;
    uint num;

    SKIPSPACE(cp);
    if (ISEOL(cp)) {snprintf(result, AACS_SERVER_BUFFERS, "%d", aapl->debug); return;}

    cp2 = cp;
    num = aapl_num_from_hex(cp, &cp2, 1, 1);

    if (cp2 == cp)
        cmd_error(aapl, result, "Invalid \"set_debug\" command ignored: Parameter must be a single integer. Got: \"%s\".", cmd);
    else
    {
        aapl->debug = num;
        snprintf(result, AACS_SERVER_BUFFERS, "%d", aapl->debug);
    }
}


static void sbus_mode_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    const char *cp2;
    Aapl_comm_method_t comm_method;
    (void)cmd;

    SKIPSPACE(cp);
    if (ISEOL(cp)) {snprintf(result, AACS_SERVER_BUFFERS, "%s", aapl_comm_method_to_str(aapl->communication_method)); return;}

    cp2 = cp;

    if( aapl_str_to_comm_method(cp2,&comm_method) ) aapl->communication_method = comm_method;

    snprintf(result, BUFLEN, "sbus_mode option must be {AACS|USER}_{I2C|MDIO|SBUS}|SYSTEM_{I2C|MDIO}|OFFLINE");
}


static void version_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        snprintf(result, AACS_SERVER_BUFFERS, "AAPL AACS server %s", AAPL_VERSION);
}


static void status_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        snprintf(result, AACS_SERVER_BUFFERS, "Version:    AAPL AACS Server %s;Current chip:         0 (of 0..%d);", AAPL_VERSION, aapl->chips-1);
}



#if AAPL_ALLOW_AACS

static void my_send_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
      (void)cmd;
      SKIPSPACE(cp);


      if (ISNUL(cp)) {result[0] = '\0'; return;}

      strncpy(result, avago_aacs_send_command(aapl, cp), AACS_SERVER_BUFFERS);
      result[AACS_SERVER_BUFFERS - 1] = '\0';
}
#endif


static void help_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result)) strcpy(result, RES_HELP);
}



static void close_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result)) strcpy(result, RES_CLOSE);
}


static void exit_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        strcpy(result, RES_EXIT);
}




EXT char *avago_aacs_process_cmd(
    Aapl_t *aapl,       /**< [in] */
    const char *cmd,    /**< [in] */
    int *chip_nump,     /**< [in/out] */
    int *ring_nump)     /**< [in/out] */
{
    const char *cp = cmd;
    static char result[BUFLEN];
    SKIPSPACE(cp);

    if (cp[0] == '@') cp++;

    if      (ISCMD(cp, "sbus",        4)) sbus_command(       aapl, cmd, cp + 4, result, chip_nump, ring_nump);
    else if (ISCMD(cp, "jtag",        4)) jtag_command(       aapl, cmd, cp + 4, result, chip_nump);
    else if (ISCMD(cp, "chip",        4)) jtag_command(       aapl, cmd, "32 02b6 0", result, chip_nump);
    else if (ISCMD(cp, "chips",       5)) chips_command(      aapl, cmd, cp + 5, result);
    else if (ISCMD(cp, "chipnum",     7)) chipnum_command(    aapl, cmd, cp + 7, result, chip_nump);
    else if (ISCMD(cp, "version",     7)) version_command(    aapl, cmd, cp + 7, result);
    else if (ISCMD(cp, "status",      6)) status_command (    aapl, cmd, cp + 6, result); /* NOTE: This is an alias to version */
#if AAPL_ALLOW_I2C || AAPL_ALLOW_SYSTEM_I2C
    else if (ISCMD(cp, "i2c",         3)) i2c_command(        aapl, cmd, cp + 3, result);
#endif
    else if (ISCMD(cp, "mdio",        4)) mdio_command(       aapl, cmd, cp + 4, result);
    else if (ISCMD(cp, "sbus_mode",   9)) sbus_mode_command(  aapl, cmd, cp + 9, result);
#if AAPL_ALLOW_AACS
    else if (ISCMD(cp, "send",        4)) my_send_command(    aapl, cmd, cp + 4, result);
    else if (ISCMD(cp, "reset",       5)) my_send_command(    aapl, cmd, cp    , result);
#endif
    else if (ISCMD(cp, "help",        4)) help_command(       aapl, cmd, cp + 4, result);
    else if (ISCMD(cp, "set_debug",   9)) set_debug_command(  aapl, cmd, cp + 9, result);
    else if (ISCMD(cp, "close",       5)) close_command(      aapl, cmd, cp + 5, result);
    else if (ISCMD(cp, "exit",        4)) exit_command(       aapl, cmd, cp + 4, result);
    else if (ISNUL(cp)) result[0] = '\0';
    else cmd_error(aapl, result, "Unrecognized command ignored: \"%s\".", cmd);

    return result;
}

#if AAPL_ENABLE_AACS_SERVER

EXT int avago_aacs_server(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    int tcp_port)       /**< [in] Port of remote to connect to. */
{
    int chip_num = 0;
    int ring_num = 0;
    int error_or_exit = 0;
    int return_status = 0;
    int rc = 1;

# ifdef WIN32
    WORD vers_req;
    WSADATA wsaData;
# endif

    int fd_socket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_IPaddr;
    socklen_t          addr_len = sizeof(client_IPaddr);
    struct sockaddr_in sai;
    typedef struct sockaddr * sa_pt;

# ifdef WIN32
    vers_req = MAKEWORD(2,2);
    WSAStartup(vers_req,&wsaData);
# endif

    if( fd_socket < 0 )
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot create PF_INET socket: %s.\n", strerror(errno));
        return 0;
    }

    rc = setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, (void *) &rc, sizeof(rc));

    if( rc < 0 )
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot setsockopt(SO_REUSEADDR) on socket %d: %s.\n", fd_socket, strerror(errno));
        goto cleanup_and_exit;
    }


    sai.sin_family      = AF_INET;
    sai.sin_addr.s_addr = INADDR_ANY;
    sai.sin_port        = htons(tcp_port);

    if ((rc = bind(fd_socket, (sa_pt) &sai, sizeof (sai))) < 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot bind() AF_INET socket for clients on port %d: %s.\n", tcp_port, strerror(errno));
        goto cleanup_and_exit;
    }

    if ((rc = listen(fd_socket, /* backlog = */ 1)) < 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot listen() for command client on port %d: %s.\n", tcp_port, strerror(errno));
        goto cleanup_and_exit;
    }


    while( !error_or_exit )
    {
        int fd_conn = accept(fd_socket, (sa_pt) &client_IPaddr, &addr_len);

        if( fd_conn < 0 )
        {
            error_or_exit = 1;  /* Cleanup and exit */
            break;
        }


        aapl_connect(aapl, 0, 0);
        if( aapl->return_code < 0 )
        {
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "aapl_connect failed.\n", 0);
            break;
        }

#       if 0
        aapl_get_ip_info(aapl, /* chip_reset */ 0);
        if( aapl_get_return_code(aapl) < 0 )
        {
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "aapl_get_ip_info returned a negative value.\n");
            break;
        }
#       endif


        for(;;)
        {
            char cmd[BUFLEN];
            int read_len = 0;
            int send_len = 0;
            char *result;
            size_t res_len;
            if ((read_len = recv(fd_conn, (void *) cmd, BUFLEN - 1, /* flags = */ 0)) < 0)
            {
                #ifdef WIN32
                if( errno == WSAECONNRESET)
                #else
                if( errno == ECONNRESET )
                #endif
                     aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "could not read from socket %d on port %d (%d: %s)\n", aapl->socket, tcp_port, read_len, aapl_tcp_strerr());
                else
                {
                    aapl_fail(aapl, __func__, __LINE__, "could not read from socket %d on port %d (%d: %s).\n", aapl->socket, tcp_port, read_len, aapl_tcp_strerr());
                    error_or_exit = 1;  /* Cleanup and exit */
                }
                break;
            }

            if( 0 == read_len )
            {
                aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Server got EOF on port %d.\n", tcp_port);
                break;
            }

            if ((read_len >= 1) && (cmd[read_len - 1] == '\n')) --read_len;
            if ((read_len >= 1) && (cmd[read_len - 1] == '\r')) --read_len;
            cmd[read_len] = '\0';

            result = avago_aacs_process_cmd(aapl, cmd, &chip_num, &ring_num);
            aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "%s => %s\n", cmd, result);

            res_len = strlen(result);
            result[res_len++] = '\n';  /* append a newline. */

            if ((send_len = send(fd_conn, (void *) result, res_len, 0)) < 0)
            {
                aapl_fail(aapl, __func__, __LINE__, "Cannot send() %u bytes to command client on TCP port %d: %d.\n", res_len, tcp_port, send_len);
                error_or_exit = 1;
                break;
            }

            if( 0 == strncmp(result, RES_CLOSE, 5) )
                break;
            if( 0 == strncmp(result, RES_EXIT, 4) )
            {
                error_or_exit = 1;
                return_status = 1;
                break;
            }
        }

        aapl_close_connection(aapl);
        close(fd_conn);
        aapl_get_return_code(aapl);
    }

cleanup_and_exit:
    close(fd_socket);
    return return_status;

}

#endif
