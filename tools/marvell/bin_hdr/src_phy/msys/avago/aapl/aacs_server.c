/* AAPL Revision: 2.0.0-beta */
/* AAPL (ASIC and ASSP Programming Layer), AACS (Avago ASIC Communication */
/* Service), LAN (command) server process/thread.  This allows a customer */
/* application to start a server and send it commands over TCP/IP that are */
/* processed locally using AAPL. */
/* */
/* This replaces Perl script aacs2/aacs_translator.pl with a C-based function. */
/* */
/* To test/run as a standalone program, compile with -DMAIN (see Makefile.am). */

/** Doxygen File Header */
/** @file */
/** @brief Implementation of AACS Server functionality. */

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"
#ifndef __MINGW32__
#include <arpa/inet.h>
#endif

#define BUFLEN 4096  /* for local I/O buffers. */

/* Command buffer - removed from Task Stack to global memory */
/* Incremental use of command buffer with size of 4KB trigger stack overrun */
char cmd[BUFLEN];

#define EQCN(str1,str2,len) (! aapl_strncasecmp(str1, str2, len))
#define EOS               '\0'
#define ISNUL(cp)         (*(cp) == EOS)
#define ISEOL(cp)         ((*(cp) == '#') || ISNUL(cp))  /* comment or end. */
#define ISTERM(cp)        (isspace(*(cp)) || ISEOL(cp))  /* end of token. */
#define SKIPSPACE(cp)     while (isspace(*(cp))) ++(cp)
#define ISCMD(cp,cmd,len) (EQCN(cp, cmd, len) && ISTERM((cp) + len))

/* Special result strings for special commands, used both as flags and for */
/* logging: */

#define RES_CLOSE "(close)"
#define RES_EXIT  "(exit)"

/* Keep this string consistent with avago_aacs_process_cmd(): */

#define RES_HELP  "Valid commands are: sbus, jtag, i2c, set_debug, chips, chipnum, version, status, send, help, close, exit."

/* A common abbreviation that optionally closes 0-2 files: */

#define FAILRET(rc,fd1,fd2) {if ((fd1) >= 0) close(fd1); if ((fd2) >= 0) close(fd2); return rc;}

/* Note:  Would like to declare char arrays/pointers "properly", but then every */
/* call to a library like strlen() requires a cast to the explicit, wrong type */
/* "char *": */
/* */
/* typedef unsigned char uc_t, * str_t;  // for brevity here. */


/*============================================================================= */
/* N U M   F R O M   H E X */
/* */
/** @details     Parse a hex number and return the equivalent binary number, */
/**              with *endp modified to the first char after the token. */
/** @param cp    String that should be all hex digits. */
/** @param endp  Pointer to string to return. */
/** @param min   Minimum token length in chars (must be >= 1). */
/** @param max   Maximum token length in chars. */
/** @return uint Binary equivalent, or 0 for any error, with *endp set to the */
/**              original cp (as a failure flag). */

uint aapl_num_from_hex(const char * cp, const char ** endp, const int min, const int max)
{
    uint result = 0;
    const char * start = *endp = cp;

    while (isxdigit(*cp))
    {
        if (cp - start >= max) return(0);
        result = (result << 4) | (isdigit(*cp) ? (*cp - '0') : (10 + tolower(*cp) - 'a'));
        ++cp;
    }
    if ((cp - start < min) || (! ISTERM(cp))) return(0);
    *endp = cp;
    return(result);
}

/*============================================================================= */
/* N U M   F R O M   B I N */
/* */
/** @details     Parse an ASCII binary number possibly containing 'x'/'X' */
/**              digits, and return the equivalent binary number, with *endp */
/**              modified to the first char after the token. */
/** @param cp    String that should be all '0'/'1'/'x'/'X' digits. */
/** @param endp  Pointer to string to return. */
/** @param min   Minimum token length in chars (must be >= 1). */
/** @param max   Maximum token length in chars. */
/** @param maskp Pointer to a 32-bit mask value to set, default = all 1s, but */
/**              returns 0s for 'x'/'X' meaning bits not to be changed */
/**              (read/modify/write). */
/** @return uint Binary equivalent, or 0 for any error, with *endp set to the */
/**              original cp (as a failure flag); and with *maskp (always) */
/**              modified to reflect the data value.  In case of an empty */
/**              string (following a data value consisting of just "z"), */
/**              assuming min >= 1, returns 0 with an apparent error, but with */
/**              mask = 0, and the caller can proceed from there. */

uint aapl_num_from_bin(const char * cp, const char ** endp, const int min, const int max, uint * maskp)
{
    uint result = 0;
    uint mask   = 0;  /* default = no bits to modify. */
    const char * start = *endp = cp;

    while ((*cp == '0') || (*cp == '1') || (*cp == 'x') || (*cp == 'X'))
    {
        if (cp - start >= max) return(0);

        if ((*cp == '0') || (*cp == '1'))
        {result = (result << 1) | (*cp - '0'); mask = (mask << 1) | 1;}
        else
        {result <<= 1; mask <<= 1;}  /* 0-bit, masked out. */

        ++cp;
    }
    *maskp = mask;

    if ((cp - start < min) || (! ISTERM(cp))) return(0);
    *endp = cp;
    return(result);
}

/*============================================================================= */
/* C M D   E R R O R */
/* */
/* Given a buffer of size BUFLEN, plus a printf() format string and any */
/* varargs, print a client command error into the buffer with "ERROR:" */
/* inserted. */

static void cmd_error(Aapl_t *aapl, char *result, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(result, fmt, ap);

    /* Print to the aapl log */
    aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "%s\n", result);
    memmove(result+7, result, strlen(result)+1); /* move the string to the right by 7 */
    memcpy(result, "ERROR: ", 7);
    va_end(ap);
}

/*============================================================================= */
/* All command handling functions below receive: */
/* */
/* - full command line string for error messages (possibly including leading */
/*   whitespace), but with any trailing CR/NL removed */
/* */
/* - place in the string immediately after the command (known to be ISTERM()) */
/* */
/* - a result buffer to write */
/* */
/* All return non-null text (result or error message) in result[], without a */
/* trailing newline. */


/*============================================================================= */
/* S B U S   P A R S E   D A   C M D */
/* */
/* Parse an SBus command <da> or <cmd> field.  Given: */
/* */
/* - command string read from a socket (any trailing CR/NL already removed) */
/* - pointer to start of SBus <da> or <cmd> token to parse */
/* - pointer to uint value to return */
/* - argument number on command line, for error message */
/* - description of the value, for error message */
/* - result buffer */
/* */
/* Parse the token as a 2-char hex number.  If successful, sets *valp, advances */
/* *cpp to the next token, and returns TRUE; otherwise, writes an error message */
/* into result, and returns FALSE. */

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

/*============================================================================================ */
/* S B U S   P A R S E   D A T A */
/* */
/* Parse an SBus command <data> field; given: */
/* */
/* - command string read from a socket (any trailing CR/NL already removed) */
/* - pointer to start of SBus <data> token to parse */
/* - pointer to uint value to return */
/* - pointer to uint mask to return */
/* - result buffer */
/* - pointer to read/modify/write flag to set */
/* */
/* Parse various formats (see the HW ServerUISpec, or summary below).  If */
/* successful, sets *datap and *maskp (latter with 1s for bits to set), */
/* advances *cpp to the next token, and returns the number of bits to send the */
/* client in the reply (8 or 32); otherwise, writes an error message into */
/* result, and returns 0. */

static int sbus_parse_data(Aapl_t *aapl, const char * cmd, const char ** cpp, uint * datap, uint * maskp, char * result)
{
    const char * cp = *cpp;
    const char * cp2;
    int  reslen;  /* number of bits to send client, based on input. */
    uint data = 0;

/* ----------- excerpt from HW ServerUISpec 110224 ------------ */
/*  Address formats for <data>: */
/* */
/*  C:  2 chars:  8-bit ASCII-encoded hex, such as "a5" */
/*  D:  8 chars:  8-bit ASCII-encoded binary, such as "10100101" */
/*  E: 10 chars:  32-bit ASCII-encoded hex, such as "0x012345a5" */
/*                (the leading "0x" or "0X" is required) */
/*  F: 32 chars:  32-bit ASCII-encoded binary, such as */
/*                "00000001001000110100010110100101" */
/*  G:  N <= 31 chars (variable length) beginning with 'z'/'Z', such as "z01011": */
/*                same as previous except the 'z'/'Z' is replaced with an */
/*                appropriate number of 'x's to make a 32-char <data> value (see */
/*                below) */
/* */
/*  For short formats (C and D), leading bits are prefilled as 0s. */
/* */
/*  For all ASCII-encoded binary formats (D, F, G), any character except */
/*  the 'z'/'Z' prefix for case G can be specified as 'x'/'X' instead of */
/*  '0'/'1'.  Special case:  D or F can start with "0x"|"0X". */


/* Check for 'z' prefix (case G) followed by 0-31 ASCII binary bits: */

    if ((*cp == 'z') || (*cp == 'Z'))
    {
        data = aapl_num_from_bin(++cp, &cp2, 1, 31, maskp);

        if (cp == cp2)  /* empty or invalid binary string found: */
        {
            if (! ISTERM(cp + 1))  /* naked "z" only is OK; data == *maskp == 0. */
            {
                cmd_error(aapl, result, "Invalid \"sbus\" command ignored. Arg 4 = <data> 'z' prefix must be followed by 0-31 '0'/'1'/'x'/'X' chars, but got: \"%s\".", cmd);
                return(0);
            }
        }
        /* else leaves cp2 set after token. */

        reslen = 32;
    }

/* Check for cases C,D,E,F starting with token length (2,8,10,32): */

    else
    {
        int len;
        *maskp = 0xffffffff;  /* default = no read/modify/write. */

        for (cp2 = cp; isxdigit(*cp2) || (*cp2 == 'x') || (*cp2 == 'X'); ++cp2)
        /* null */;

        len = cp2 - cp;

        if      ( len ==  2) data = aapl_num_from_hex(cp, &cp2,  2,  2);         /* case C. */
        else if ( len ==  8) data = aapl_num_from_bin(cp, &cp2,  8,  8, maskp);  /* case D. */
        else if ((len == 10) && EQCN(cp, "0x", 2))
                   {cp += 2; data = aapl_num_from_hex(cp, &cp2,  8,  8);}        /* case E. */
        else if ( len == 32) data = aapl_num_from_bin(cp, &cp2, 32, 32, maskp);  /* case F. */
        else cp2 = cp;  /* marks an error. */

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

} /* sbus_parse_data() */


/*============================================================================= */
/* C H E C K   T E R M */
/* */
/* Given: */
/* */
/* - command line string for error messages */
/* */
/* - location of trailing part of command, after last token, but can start */
/*   with whitespace */
/* */
/* - result buffer to write */
/* */
/* Check if the command terminates properly:  if not a white space, */
/* at end of line or a comment mark, write an error message to result and */
/* return FALSE, otherwise just return TRUE. */

static BOOL check_term(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (ISTERM(cp)) return(TRUE);

    cmd_error(aapl, result, "Unexpected extra word(s) on command line, ignored. Got: \"%s\".", cmd);
    return(FALSE);
}

/*============================================================================= */
/* S B U S   C O M M A N D */
/* */
/* Handles an "sbus" command and writes into result an error message or the */
/* SBus command response.  See command handling summary above. */
/* */
/* This code versus the HW server and sbus*() backends: */
/* */
/* - This server parses the SBus parameters:  All values are allowed the same */
/*   as in the HW server, except no ASCII binary for <sa>, <da>, or <cmd> args. */
/*   Comments are allowed but presently ignored, not forwarded to sbus*() */
/*   back-ends.  Unlike the HW server, parses strings to binary values, not to */
/*   strings.  Any "z" or "x" in <data> sets corresponding mask bits to 0, */
/*   otherwise it's all-1s. */
/* */
/* - For <sa>, parses/saves optional chip_num (4th char) and ring_num (3rd */
/*   char) values, and applies them to all following SBus commands until */
/*   changed.  Note that "chip_num" can independently alter chip_num between */
/*   SBus commands. */
/* */
/* - This server does not range-check chip_num or ring_num values at all.  This */
/*   allows passing 0xf (for broadcast) to AAPL sbus*() (unlike no special */
/*   meaning of 0xf for HW the server), and lets it do any other range checking */
/*   at a lower level. */
/* */
/* - This server does not handle broadcast (<sa> & 0xff = special values like */
/*   0xff) and/or RMW (zero'd mask bits).  It just passes parameters (as ints, */
/*   plus mask) to sbus*() for it to handle. */
/* */
/* - Lower-level sbus*() handles both broadcast and/or RMW, versus what's */
/*   passed to the HW server if that's the backend.  Might be slower but more */
/*   portable to not forward broadcast and/or RMW to the HW server. */
/* */
/* sbus <sa> <da> <cmd> <data> [# <comment>]  # in these formats: */
/* */
/*   <sa>:    SBus (slice) address, optionally including chip + ring numbers */
/*   <da>:    data (register) address */
/*   <cmd>:   command, usually 01 = write or 02 = read */
/*   <data>:  data to send */
/* */
/* <sa>,<da>,<cmd> = 2-char hex only, such as "00" or "a5" (except <sa>, see */
/*   below); unlike Perl/HW servers, does not support ASCII binary for these */
/*   values. */
/* */
/*   For <sa> only:  Optional 4/3-char format to set *chip_nump (0..f) (such as */
/*   "2" in "2081") and *ring_nump (0..f) (such as "3" in "37a").  Previous */
/*   chip and ring numbers are remembered and applied to new "sbus" commands */
/*   where they are not respecified, plus "chip_num" can revise chip_num */
/*   between SBus commands. */
/* */
/* <data> = 2-char hex, 8-char binary, 8-char hex (preceded by "0x"), or */
/*   32-char binary; with "z" and "x" allowed in binary values for leading */
/*   zeroes and read/modify/write, respectively. */
/* */
/* Examples: */
/*   sbus fe 0a 02 00          # read SBus master clock divider value. */
/*   sbus fe 0a 01 0x00000003  # write divider using 32-bit/char value. */
/*   sbus fe 0a 01 00000000000000000000000000000011  # write using binary. */
/*   sbus fe 0a 01 z1x0        # RMW setting bits [2,0] to '1x0'. */
/* */
/* Note:  This AAPL AACS server's "sbus" command code is similar to that in the */
/* HW server (HS1, PS1, PC1), but far from identical. */

static void sbus_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result, int * chip_nump, int * ring_nump)
{
    const char * cp2;
    uint new_chip = 0x10;
    uint new_ring = 0x10;
    uint sa;
    uint data_out;
    uint da, scmd, data, mask;

    int reslen;

    SKIPSPACE(cp);

    if (ISEOL(cp))
    {
        cmd_error(aapl, result, "Invalid \"sbus\" command ignored. Must be: \"sbus <sa> <da> <cmd> <data> [# <comment>]\".");
        return;
    }

/* Parse <sa> with special case handling of 4/3-char hex <sa> starting with */
/* [chip number and] SBus ring address: */
/* */
/* Note:  Does not range-check these values, leaves that for sbus*() calls. */

    sa = aapl_num_from_hex(cp, &cp2, 2, 4);

    if (cp2 == cp)
    {
        cmd_error(aapl, result, "Invalid \"sbus\" command ignored: Arg 1 = <sa> must be 2-4 hex digits. Got: \"%s\".", cmd);
        return;
    }
    if (cp2 - cp == 4) new_chip = (sa & 0xf000) >> 12;
    if (cp2 - cp >= 3) new_ring = (sa & 0x0f00) >>  8;
    cp = cp2; SKIPSPACE(cp);

/* Parse <da>, <cmd>, <data> fields: */

    if (! (sbus_parse_da_cmd(aapl, cmd, &cp, &da,   2, "da",  result)
        && sbus_parse_da_cmd(aapl, cmd, &cp, &scmd, 3, "cmd", result)
        && ((reslen = sbus_parse_data(aapl, cmd, &cp, &data, &mask, result)))))
    {return;}

/* Check end of command line: */

    if (! check_term(aapl, cmd, cp, result)) return;
    if (new_chip < 0x10) *chip_nump = new_chip;
    if (new_ring < 0x10) *ring_nump = new_ring;

/* Forward the command for execution: */

    sa |= (*chip_nump << 12) | (*ring_nump << 8);  /* a no-op if just got new values. */
    aapl_get_return_code(aapl);  /* set it to 0. */

    data_out = ((scmd == 1 && mask != 0xffffffff) ? avago_sbus_rmw(aapl, sa, da, data, mask) :
                (scmd == 1)                       ? avago_sbus_wr( aapl, sa, da, data) :
                (scmd == 2)                       ? avago_sbus_rd( aapl, sa, da) :
                                                    avago_sbus(    aapl, sa, da, scmd, data, /* recv_data_back = */ 1));

/* Check for anomaly: */

    if (aapl_get_return_code(aapl) < 0)
    {
        cmd_error(aapl, result, "Command failed: \"%s\" => \"%s\".", cmd, aapl->data_char);
        return;
    }
    aapl_hex_2_bin(result, data_out, /* underscore_en = */ 0, /* bits */ 32);
    if (reslen == 8) strcpy(result, result + 24);  /* last "byte" only. */

} /* sbus_command() */


/* Input (cmd) is one of: */
/*      i2c r <addr> <num_bytes> [# <comment>] */
/*      i2c w <addr> <byte> [<byte>...] [# <comment>] */
/*      i2c wr <addr> <num_bytes> <byte> [<byte>...] [# <comment>] */
/*  Where <addr> and <byte> are 2 digit hex values, and */
/*        <num_bytes> is a decimal value. */
static void i2c_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    typedef enum { cmd_read = 0, cmd_write, cmd_writeread} i2c_cmd_t;
    unsigned int i2c_address;  /* [in] Address of the sbus device */
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

    /* Determine command type: */
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

    /* Parse address: */
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
        if( cp2 == cp || !(isspace(*cp2) || *cp2 == '\0') || num_bytes_r < 0 || num_bytes_r > sizeof(buffer) )
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
            if( num_bytes >= sizeof(buffer) )
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
        if( num_bytes <= 0 )
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

static void mdio_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    sprintf(result, "ERROR: The mdio command is not implemented.\n");
}

/*============================================================================= */
/* J T A G   C O M M A N D */
/* */
/* See command handling summary above. */
/* */
/*   jtag 32 02b6 0...  [#<comments>]  # one or more 0s. */
/* */
/* Handles a "jtag 32 02b6 00" command (chip TAP ID).  This is a necessary */
/* special case, so a client can identify the ASIC.  Writes into result one of: */
/* */
/* - AAPL_CHIP_ID_OVERRIDE, if defined */
/* - else if AAPL_ALLOW_AACS, result of sending the command to the backend server */
/* - else an error message */

static void jtag_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result, int * chip_nump)
{
/* Look for "jtag 32 02b6 0..." (one or more 0s): */

    SKIPSPACE(cp); if (0 != strncmp( cp, "32",   2)) goto BadJtagCmd; cp += 2;
    SKIPSPACE(cp); if (! EQCN(cp, "02b6", 4)) goto BadJtagCmd; cp += 4;
    SKIPSPACE(cp); if (*cp != '0')            goto BadJtagCmd; ++cp;
    while (*cp == '0') ++cp;
    if (! check_term(aapl, cmd, cp, result)) return;

    aapl_hex_2_bin(result, aapl->jtag_idcode[*chip_nump], 0, 32);
    return;

    BadJtagCmd:
    cmd_error(aapl, result, "Unsupported \"jtag\" command ignored. Only \"jtag 32 02b6 0...\" (one or more 0s) is allowed. Got: \"%s\".", cmd);
}

/*============================================================================= */
/* C H I P S   C O M M A N D */
/* */
/* See command handling summary above. */
/* */
/*   chips [#<comments>] */
/* */
/* Writes into result an error message or the number of ASICs in the TAP loop */
/* (aapl->chips). */

static void chips_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        sprintf(result, "%d", aapl->chips);
}

/*============================================================================= */
/* C H I P N U M   C O M M A N D */
/* */
/* See command handling summary above. */
/* */
/*   chipnum [#<comments>] */
/* */
/* Writes into result an error message, the present chip number 0..f (see */
/* "sbus" above), or with an argument, sets and returns the number, but only */
/* within the valid range 0 .. (aapl->chips - 1). */

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

/*============================================================================= */
/* SET DEBUG COMMAND */
/* */
/* Allows user to check and set aapl->debug */
/* */
/*   set_debug [debug_level] [#<comments>] */
/* */
/*   If debug_level is not present, return the current aapl->debug, otherwise */
/*   set aapl->debug to debug_level */
/* */
static void set_debug_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    const char *cp2;
    uint num;

    SKIPSPACE(cp);
    if (ISEOL(cp)) {snprintf(result, BUFLEN, "%d", aapl->debug); return;}

    cp2 = cp;
    num = aapl_num_from_hex(cp, &cp2, 1, 1);

    if (cp2 == cp)
        cmd_error(aapl, result, "Invalid \"set_debug\" command ignored: Parameter must be a single integer. Got: \"%s\".", cmd);
    else if (num > 6)
        cmd_error(aapl, result, "Invalid \"set_debug\" command ignored: Max debug level must be 0-6. Got: \"%s\".", cmd);
    else
    {
        aapl->debug = num;
        snprintf(result, BUFLEN, "%d", aapl->debug);
    }
}

/*============================================================================= */
/* SBUS MODE COMMAND */
/* */
/* Allows user to check and set aapl->communication_method */
/* */
/*   set_debug [debug_level] [#<comments>] */
/* */
/*   If debug_level is not present, return the current aapl->debug, otherwise */
/*   set aapl->debug to debug_level */
/* */
static void sbus_mode_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    const char *cp2;
    Aapl_comm_method_t comm_method;
    SKIPSPACE(cp);
    if (ISEOL(cp)) {snprintf(result, BUFLEN, "%s", aapl_comm_method_to_str(aapl->communication_method)); return;}
    cp2 = cp;

    if( aapl_str_to_comm_method(cp2,&comm_method) ) aapl->communication_method = comm_method;

    snprintf(result, BUFLEN, "sbus_mode option must be {AACS|USER}_{I2C|MDIO|SBUS}|SYSTEM_{I2C|MDIO}|OFFLINE");
}

/*============================================================================= */
/* V E R S I O N   C O M M A N D */
/* */
/* See command handling summary above. */
/* */
/*   version [#<comments>] */

static void version_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        snprintf(result, BUFLEN, "AAPL AACS server %s", AAPL_VERSION);
}

/*============================================================================= */
/* S T A T U S   C O M M A N D */
/* */
/* See command handling summary above. */
/* */
/*   status [#<comments>] */

static void status_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        snprintf(result, BUFLEN, "Version:    AAPL AACS Server %s;Current chip:         0 (of 0..%d);", AAPL_VERSION, aapl->chips-1);

}

#if AAPL_ALLOW_AACS
/*============================================================================= */
/* M Y   S E N D   C O M M A N D */
/* */
/* See command handling summary above. */
/* */
/*   send <anything> [#<comments>] */
/* */
/* Sends anything on the command line after "send" to the back-end server, and */
/* writes into result the command response (truncated at BUFLEN). */

static void my_send_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
      SKIPSPACE(cp);

/* TBD:  This special case is only needed until avago_aacs_send_command() can handle an */
/* empty command: */

      if (ISNUL(cp)) {result[0] = '\0'; return;}

      strncpy(result, avago_aacs_send_command(aapl, cp), BUFLEN);
      result[BUFLEN - 1] = '\0';
}
#endif /* AAPL_ALLOW_AACS */

/*============================================================================= */
/* H E L P   C O M M A N D */
/* */
/* See command handling summary above. */
/* */
/*   help [#<comments>] */
/* */
/* Writes into result an error string or RES_HELP. */

static void help_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result)) strcpy(result, RES_HELP);
}

/*============================================================================= */
/* C L O S E   C O M M A N D */
/* */
/* See command handling summary above. */
/* */
/*   close [#<comments>] */
/* */
/* Writes into result an error string or RES_CLOSE. */

static void close_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result)) strcpy(result, RES_CLOSE);
}

/*============================================================================= */
/* E X I T   C O M M A N D */
/* */
/* See command handling summary above. */
/* */
/*   exit [#<comments>] */
/* */
/* Writes into result an error string or RES_EXIT. */

static void exit_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        strcpy(result, RES_EXIT);
}



/*============================================================================= */
/* P R O C E S S   C M D */
/* */
/* Given: */
/* */
/* - aapl info pointer */
/* - command string read from a socket (any trailing CR/NL already removed) */
/* - pointers to chip_num and ring_num values to use/modify */
/* */
/* Process the string as an AACS server command, one of a limited subset of */
/* commands understood by the old Perl server and the newer HW server (HS1, */
/* PS1, PC1); see RES_HELP for a list, and *_command() for syntax summaries. */
/* */
/* Returns a newline-terminated response string in all cases, including error */
/* messages, including empty string for empty, RES_CLOSE for "close", or */
/* RES_EXIT for "exit" commands.  The result lives in static memory overwritten */
/* by the next call. */

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
    else if (ISCMD(cp, "i2c",         3)) i2c_command(        aapl, cmd, cp + 3, result);
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

/*============================================================================= */
/* A A C S   S E R V E R */
/* */
/* Given: */
/* */
/* - aapl info pointer */
/* - TCP port number */
/* */
/* Start (and restart after closure) an AACS server that accepts one TCP/IP */
/* connection on tcp_port, receives zero or more AACS commands, forwards them */
/* to avago_sbus_wr(), etc, and returns response text to the command client. */
/* */
/* Returns FALSE for internal error logged, or TRUE for client "exit" command. */
/* Does not return for client command anomalies or "close" commands. */
/* */
/* The caller must first: */
/* */
/* - Open a lower-level (back end) connection or whatever else is required for */
/*   avago_sbus_wr(), etc to function, such as by using aapl_connect() (but not */
/*   necessarily this function in the end user context). */
/* */
/* - Start a process or thread that's taken over by this function, which only */
/*   returns upon "exit" or internal error. */
/* */
/* Note:  Unlike the HW server (HS1, etc), this process does not support */
/* command batching.  It expects just one command => response at a time. */

EXT int avago_aacs_server(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    int tcp_port)       /**< [in] Port of remote to connect to. */
{
    int chip_num = 0;  /* selected for SBus; 0 .. (aapl->chips - 1). */
    int ring_num = 0;  /* which SBus ring within the current chip. */
    int error_or_exit = 0;
    int return_status = 0;
    int rc = 1;

# ifdef WIN32
    WORD vers_req;
    WSADATA wsaData;
    vers_req = MAKEWORD(2,2);
    WSAStartup(vers_req,&wsaData);
# endif

/* INITIALIZE:  Open TCP socket to command client: */

    int fd_socket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_IPaddr;
    socklen_t          addr_len = sizeof(client_IPaddr);
    struct sockaddr_in sai;  /* INET socket info. */
    typedef struct sockaddr * sa_pt;

    if( fd_socket < 0 )
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot create PF_INET socket: %s.\n", strerror(errno));
        return 0;
    }
    aapl_log_printf(aapl, AVAGO_INFO, 0, 0, "AAPL AACS server version %s is now listening for TCP connections on port %d...\n", AAPL_VERSION, tcp_port);

/* Set socket option to allow immediate reuse; otherwise it usually remains */
/* unavailable (bind() fails) in TCP/IP TIME_WAIT status for a long time. */

    rc = setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, (void *) &rc, sizeof(rc));

    if( rc < 0 )
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot setsockopt(SO_REUSEADDR) on socket %d: %s.\n", fd_socket, strerror(errno));
        goto cleanup_and_exit;
    }

/* Bind socket: */

    sai.sin_family      = AF_INET;
    sai.sin_addr.s_addr = INADDR_ANY;
    sai.sin_port        = htons(tcp_port);

    if ((rc = bind(fd_socket, (sa_pt) &sai, sizeof (sai))) < 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot bind() AF_INET socket for clients on port %d: %s.\n", tcp_port, strerror(errno));
        goto cleanup_and_exit;
    }

/* Listen for a connection: */

    if ((rc = listen(fd_socket, /* backlog = */ 1)) < 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot listen() for command client on port %d: %s.\n", tcp_port, strerror(errno));
        goto cleanup_and_exit;
    }

/* OUTER LOOP:  Await connections from command clients: */
/* */
/* The accept() function blocks until a command client requests a connection, */
/* then returns a new socket fd for communicating with the client. */


    while( !error_or_exit )
    {
        int fd_conn = accept(fd_socket, (sa_pt) &client_IPaddr, &addr_len);

        if( fd_conn < 0 )
        {
            aapl_fail(aapl, __func__, __LINE__, "Cannot listen() for command client on port %d: %s.\n", tcp_port, strerror(errno));
            error_or_exit = 1;  /* Cleanup and exit */
            break;
        }
        aapl_log_printf(aapl, AVAGO_DEBUG1, 0, 1, "Connection from %s on port %d.\n", inet_ntoa(client_IPaddr.sin_addr), tcp_port);

        aapl_connect(aapl, 0, 0);
        if( aapl->return_code < 0 )
        {
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "aapl_connect failed.\n", 0);
            break;
        }

#       if 0
        /* This clause updates info on every connection. */
        /* Disable to get info only once at startup: */
        aapl_get_ip_info(aapl, /* chip_reset */ 0);
        if( aapl_get_return_code(aapl) < 0 )
        {
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "aapl_get_ip_info returned a negative value.\n");
            break;
        }
#       endif


/* INNER LOOP:  Read commands until internal error, EOF, "close", or "exit": */
/* */
/* Note:  Assume no command over BUFLEN - 1 bytes, otherwise it gets fragmented */
/* and misparsed. */

        for(;;)  /* until break or return: */
        {
/* Command buffer - remove command buffer from Task Stack to global memory */
/* incremental use of command buffer with size of 4KB trigger stack overrun */
#if 0
            char cmd[BUFLEN];
#endif
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

/* EOF:  Normal socket closure by the client (rather than through "close" */
/* command) results in read_len == 0, otherwise there's normally at least a */
/* newline: */

            if( 0 == read_len )
            {
                aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Server got EOF on port %d.\n", tcp_port);
                break;  /* inner loop only. */
            }

/* Terminate command string; remove CR/NL if any (at least a newline is usually */
/* present); then process the command: */

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

/* Handle special cases: */
            if( 0 == strncmp(result, RES_CLOSE, 5) )
                break;  /* inner loop only. */
            if( 0 == strncmp(result, RES_EXIT, 4) )
            {
                error_or_exit = 1;
                return_status = 1;
                break;
            }
        } /* inner loop:  client commands. */

        aapl_close_connection(aapl);
        close(fd_conn);
        aapl_get_return_code(aapl); /* Cleanup for next client. */
    } /* outer loop:  new connections. */

cleanup_and_exit:
    close(fd_socket);
    return return_status;

} /* avago_aacs_server() */

#endif /* AAPL_ENABLE_AACS_SERVER */
