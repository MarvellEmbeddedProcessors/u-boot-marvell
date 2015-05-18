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

/* AAPL (ASIC and ASSP Programming Layer) AACS functions. */

/** Doxygen File Header */
/** @file */
/** @brief AACS functions. */

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"

#if AAPL_ALLOW_AACS

/* Since only SBus commands are queued the max size for */
/* this buffer is the maximum number of commands queued * the size of each cmd */
#define AAPL_CMD_LOG_BUF_SIZE ((AAPL_MAX_CMDS_BUFFERED * AAPL_SBUS_CMD_LOG_BUF_SIZE) + 1)


/* Wrap strerror functionality. */
const char *aapl_tcp_strerr()
{
    # ifdef WIN32
        static char error[20];
        snprintf(error, 19, "%d", WSAGetLastError());
        return error;
    #else
        return strerror(errno);
    # endif
}


/*============================================================================= */
/* AAPL CHECK CAPABILITIES */
/* */
/* Internal function that sets aapl->capabilities variable based on information */
/* from the remote AACS server */
/* */
void aapl_check_capabilities(
    Aapl_t *aapl)           /**< [in] Pointer to Aapl_t structure. */
{
    int no_cmd_buff = 0;

    int set_capabilities = !aapl->capabilities;
    aapl->prior_connection = 1;

    avago_aacs_send_command(aapl, "version");
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "AAPL socket %d connected to %s:%d (%s).\n", aapl->socket, aapl->aacs_server, aapl->tcp_port, aapl->data_char);

    if( strstr(aapl->data_char, "AAPL AACS server") || strstr(aapl->data_char, "aacs_server.pl") || strstr(aapl->data_char, "Verilog") )
        no_cmd_buff = 1;

    if (strstr(aapl->data_char, "ATS"))
    {
        int queries = 2;
        char buf[10];
        #if AAPL_ENABLE_DIAG
        queries += 1;
        #endif
        snprintf(buf, 9, "%d", queries);
        avago_aacs_send_command(aapl, buf);
    }
    else if (aapl->capabilities) return;

    avago_aacs_send_command(aapl, "help");
    if (set_capabilities && strstr(aapl->data_char, "spico_int")) aapl->capabilities |= AACS_SERVER_SPICO_INT;
    if (set_capabilities && strstr(aapl->data_char, "sbus_reset")) aapl->capabilities |= AACS_SERVER_SBUS_RESET;

    if (no_cmd_buff && !strstr(aapl->data_char, "batching supported"))
    {
        aapl->max_cmds_buffered = 0;
        aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Command buffering disabled for aacs_server.pl, and older AAPL AACS servers.\n",0);
    }
    else aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Command buffering set to %d.\n", aapl->max_cmds_buffered);


#if AAPL_ENABLE_DIAG
    if (strstr(aapl->data_char,"diag"))
    {
        avago_aacs_send_command_options(aapl, "diag size", 1, 10);
        aapl_log_printf(aapl, AVAGO_DEBUG3, __func__, __LINE__, "Remote diag size: %d. Local: %d\n", aapl->data, sizeof(Avago_diag_config_t));
        if (set_capabilities && aapl->data == sizeof(Avago_diag_config_t))
            aapl->capabilities |= AACS_SERVER_DIAG;
    }
#endif

    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "AAPL capabilities set to: 0x%08x.\n", aapl->capabilities);
}

/** @cond INTERNAL */

void avago_aacs_close(Aapl_t *aapl)
{
    if( aapl->socket != -1 )
    {
        int ret = closesocket(aapl->socket);
        if( ret < 0 )
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "close() failed: %s on socket %d.\n", aapl_tcp_strerr(), aapl->socket);
        aapl->socket = -1;
#       ifdef WIN32
        WSACleanup();
#       endif
    }
}

void avago_aacs_open(Aapl_t *aapl)
{
    struct sockaddr_in serv_addr;
    struct hostent *server = NULL;
    int sockfd;
    int retries = 0;

# ifdef WIN32
    WSADATA wsaData;
    WORD vers_req = MAKEWORD(2,2);
    if( 0 != WSAStartup(vers_req,&wsaData) )
    {
        aapl_fail(aapl, __func__, __LINE__, "WSAStartup failed, WSAGetLastError(): %s\n", aapl_tcp_strerr());
        return;
    }
# endif

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( sockfd < 0 )
    {
        aapl_fail(aapl, __func__, __LINE__, "socketfd == -1, errno: %s\n", aapl_tcp_strerr());
#       ifdef WIN32
        WSACleanup();
#       endif
        return;
    }

    aapl->socket = sockfd;

    if (aapl->aacs_server) server = gethostbyname(aapl->aacs_server);
    if( server == NULL )
    {
        if (aapl->aacs_server) aapl_fail(aapl, __func__, __LINE__, "aacs_server (%s) could not be found (%s).\n", aapl->aacs_server, aapl_tcp_strerr());
        else                   aapl_fail(aapl, __func__, __LINE__, "aacs_server not specified.\n", 0);
        avago_aacs_close(aapl);
        return;
    }
    serv_addr.sin_family = AF_INET;
    memmove((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);

    serv_addr.sin_port = htons(aapl->tcp_port);

    while( connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0 )
    {
        #ifdef WIN32
        errno = WSAGetLastError();
        if( errno == WSAECONNREFUSED || errno == WSAEBADF || errno == WSAENETUNREACH || errno ==WSAEHOSTUNREACH)
        #else
        if( errno == ECONNREFUSED || errno == EBADF || errno == ENETUNREACH || errno == EHOSTUNREACH)
        #endif
        {
            aapl_fail(aapl, __func__, __LINE__, "Connection attempt %d failed when connecting to the AACS Server at %s:%d (%s). %s is now returning.\n", retries, aapl->aacs_server, aapl->tcp_port, aapl_tcp_strerr(), __func__);
            avago_aacs_close(aapl);
            return;
        }
        retries ++;
        if (retries > 10 || aapl->debug) aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "Connection attempt %d failed when connecting to the AACS Server at %s:%d (%s)\n", retries, aapl->aacs_server, aapl->tcp_port, aapl_tcp_strerr());
        ms_sleep(10);
    }

    if (!aapl->prior_connection)
        aapl_check_capabilities(aapl);
    else aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "AAPL reconnected to %s:%d.\n", aapl->aacs_server, aapl->tcp_port);
    return;
}

/** @endcond */


/* local function used by send_command_options to reconnect a closed socket */
/* returns -1 for failure, 0 for success */
static int reconnect(Aapl_t *aapl, const char * command)
{
    int rc = aapl->return_code;
    aapl_connect(aapl, 0, 0);
    if (aapl->return_code - rc != 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "The command \"%s\" was sent to a closed socket.\n", command);
        aapl->data = 0;
        aapl_log_printf(aapl, AVAGO_DATA_CHAR, 0, 0, "",0);
        return -1;
    }
    return 0;
}

/* local function used by send_command_options to recv() data into the aapl->data_char buffer */
/* Automatically reallocates the buffer if more space is needed */
/* returns bytes received (<=0 for failure) */
int aapl_recv(Aapl_t *aapl)
{
    int bytes_recv;
    bytes_recv = recv(aapl->socket, aapl->data_char, aapl->data_char_size-1, 0);

    if (bytes_recv <= 0) return 0;

    while (aapl->data_char[bytes_recv-1] != 0x0a)
    {
        int last_bytes_recv;
        if (bytes_recv >= (aapl->data_char_size - 10))
        {
            char * ptr = (char *) aapl_realloc(aapl, aapl->data_char, aapl->data_char_size * 2, __func__);
            if (ptr)
            {
                aapl->data_char = ptr;
                aapl->data_char_size *= 2;
                aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__,
                    "Allocating %d additional bytes to data_char buffer. Size is now %d bytes.\n", aapl->data_char_size/2, aapl->data_char_size);
            }
            else
            {
                aapl_fail(aapl, __func__, __LINE__, "Realloc failed after recv() of %d bytes. Your data has been corrupted.\n", bytes_recv);
                aapl->data_char[bytes_recv-1] = 0x0a;
                aapl->data_char[bytes_recv] = 0x00;
                return -1;
            }
        }
        last_bytes_recv = recv(aapl->socket, aapl->data_char+bytes_recv, aapl->data_char_size-1-bytes_recv, 0);
        if (last_bytes_recv > 0) bytes_recv += last_bytes_recv;
        else return -1;
    }
    return bytes_recv;
}
#endif


/*============================================================================= */
/* AVAGO AACS FLUSH */
/* */
/** @brief Forces any buffered AACS commands to be flushed */
void avago_aacs_flush(
    Aapl_t *aapl)
{
    avago_aacs_send_command(aapl, "");
}


/*============================================================================= */
/* SEND COMMAND */
/* */
/** @brief Sends a command via AACS to the connected AACS server */
/**        and returns the results. */
/** @details The "command" must be a C string (with no newline). */
/** @return  Saves the results from the command sent to the aacs_server in */
/**         aapl->data_char and then returns that data.  In addition, */
/**         aapl->data is set to the length of the returned string. */
const char *avago_aacs_send_command(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    const char *command)    /**< [in] The AACS command to send. */
{
    return avago_aacs_send_command_options(aapl, command, /* recv_data_back */ 1, /* strtol */ 0);
}

/** @brief Sends a command via AACS to the aacs_server pointed to by aapl */
/**        and return the results. */
/** @details The "command" must be a C string (with no newline). */
/** @return The results of the command sent to the aacs_server is saved in */
/**         aapl->data_char and also returned. */
/**         If recv_data_back is 0 or 2, this value will be "0". */
/**         In addition, aapl->data is set to the length of this string */
/**         unless strtol is non-zero and recv_data_back is 1, in which */
/**         case it is set to the decoded value of the returned string. */
const char *avago_aacs_send_command_options(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    const char *command,    /**< [in] The AACS command to send. */
    int recv_data_back,     /**< [in] If 0, queue command, return value not wanted. */
                            /**<      If 1, send commands, read and return last result. */
                            /**<      If 2, queue command, return value wanted later. */
                            /**<      If 3, send commands, return all results. */
    int strtol)             /**< [in] If non-zero, interpret return string as */
                            /**<      base strtol (2..16) and store the result */
                            /**<      in aapl->data as a long value. */
{
#if AAPL_ALLOW_OFFLINE_SBUS
    if (aapl->communication_method == AVAGO_OFFLINE)
    {
        aapl->data = 0;
        aapl_log_printf(aapl, AVAGO_DATA_CHAR, 0, 0, "");

        if (0==strcmp("jtag 32 02b6 0", command))
            aapl_log_printf(aapl, AVAGO_DATA_CHAR, 0, 0, "00000000000000000000000000000000");
        else if (0==strcmp("status", command))
            aapl_log_printf(aapl, AVAGO_DATA_CHAR, 0, 0, "Version:    AAPL AACS Server %s;Current chip:         0 (of 0..%d);", AAPL_VERSION, aapl->chips-1);

        return aapl->data_char;
    }
#endif

#if AAPL_ALLOW_AACS
    if (aapl_is_aacs_communication_method(aapl))
    {
        size_t cmd_len = strlen(command);
        int bytes_sent = 0;
        int flags = 0;

        #ifdef MSG_NOSIGNAL
        flags = MSG_NOSIGNAL;
        #endif

        if (!aapl->buf_cmd) aapl->buf_cmd = (char *)aapl_malloc(aapl, AAPL_CMD_LOG_BUF_SIZE, __func__);
        if (!aapl->buf_cmd_end) aapl->buf_cmd_end = aapl->buf_cmd;

#       define APPEND(end, cmd, len) { strcpy(end, cmd); end += len; }
#       define APPEND_CMD(cmd,len) APPEND(aapl->buf_cmd_end,cmd,len)
#       define RESET_CMD()  (aapl->buf_cmd_end = aapl->buf_cmd)
#       define LENGTH_CMD() (aapl->buf_cmd_end - aapl->buf_cmd)

        if (aapl->socket < 0 && !aapl->disable_reconnect)
            if (reconnect(aapl, command) < 0) return aapl->data_char;

        if (aapl->cmds_buffered == 0) RESET_CMD();
        else                          APPEND_CMD(";",1);

        if (recv_data_back==0)        APPEND_CMD("@",1);

        if( cmd_len > (size_t)(AAPL_CMD_LOG_BUF_SIZE - LENGTH_CMD() - 4) )
        {
            if( LENGTH_CMD() > 0 )
            {
                bytes_sent = send(aapl->socket, aapl->buf_cmd, LENGTH_CMD(), flags);
                if (bytes_sent <= 0)
                {
                    aapl->socket = -1;
                    aapl_fail(aapl, __func__, __LINE__, "send() failed: %s.\n", aapl_tcp_strerr());
                }
            }
            RESET_CMD();
        }

        if (cmd_len == 0 && aapl->cmds_buffered > 0)
        {
            recv_data_back = 1;
            command = "version";
            cmd_len = strlen(command);
        }
        else if (cmd_len == 0)
        {
            strcpy(aapl->data_char, "");
            aapl->data = 0;
            return aapl->data_char;
        }

        if( cmd_len > (AAPL_CMD_LOG_BUF_SIZE - 4))
        {
            bytes_sent = send(aapl->socket, command, cmd_len, flags);
            if (bytes_sent <= 0)
            {
                aapl->socket = -1;
                aapl_fail(aapl, __func__, __LINE__, "send() failed: %s.\n", aapl_tcp_strerr());
            }
        }
        else
            APPEND_CMD(command,cmd_len);
        aapl->cmds_buffered += 1;


        if ((recv_data_back==0 || recv_data_back==2)
            && aapl->cmds_buffered < aapl->max_cmds_buffered
            && (uint)LENGTH_CMD() < (AAPL_CMD_LOG_BUF_SIZE-32) )
        {
            strcpy(aapl->data_char, "0");
            aapl->data = 1;
        }
        else
        {
            int bytes_recv;
            int retry = 0;
            APPEND_CMD("\n",1);
            while (1)
            {
                bytes_sent = 0;
                bytes_recv = 0;

                if( aapl->cmds_buffered > 1 )
                    aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "Sending %d batched commands (size: %d) %s", aapl->cmds_buffered, (int) LENGTH_CMD(), aapl->buf_cmd);

                bytes_sent = send(aapl->socket, aapl->buf_cmd, LENGTH_CMD(), flags);
                if (bytes_sent) bytes_recv = aapl_recv(aapl);

                if (bytes_recv <= 0 || bytes_sent <= 0)
                {
                    if (retry == 0 && !aapl->disable_reconnect)
                    {
                        retry ++;
                        aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "%d bytes sent and %d bytes recv -- most likely due to remote socket closure (%s). Will retry once.\n", bytes_recv, bytes_sent, aapl_tcp_strerr());
                        avago_aacs_close(aapl);
                        reconnect(aapl, aapl->buf_cmd);
                        continue;
                    }
                    else avago_aacs_close(aapl);
                    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "%d bytes sent and %d bytes recv on try %d (%s). Returning.\n", bytes_recv, bytes_sent, retry, aapl_tcp_strerr());
                    aapl->cmds_buffered = 0;
                    aapl->data_char[0] = 0x0a;
                    aapl->data_char[1] = 0x00;
                    return aapl->data_char;
                }
                else break;
            }

            if (bytes_recv) bytes_recv -= 1;
            aapl->data_char[bytes_recv] = 0x00;

            if (!aapl->suppress_errors && (strstr(aapl->data_char, "WARNING") || strstr(aapl->data_char, "ERROR")))
            {
                aapl_fail(aapl, __func__, __LINE__, "Received error or warning from AACS Server: %s -> %s\n", command, aapl->data_char);
            }

            if (aapl->cmds_buffered > 1 && recv_data_back == 1)
            {
                char *lastreply = strrchr(aapl->data_char,';');
                if( lastreply )
                {
                    lastreply++;
                    bytes_recv = strlen(lastreply);
                    memmove(aapl->data_char,lastreply,bytes_recv+1);
                }
                else
                {
                    aapl_fail(aapl, __func__, __LINE__, "Received unexpected buffered command return from AACS Server after sending %d commands: %s -> %s\n",
                        aapl->cmds_buffered, aapl->buf_cmd, aapl->data_char);
                }
            }
            aapl->cmds_buffered = 0;
            aapl->data = bytes_recv;
        }

        if( strtol && recv_data_back == 1 )
            aapl->data = strtoul(aapl->data_char, NULL, strtol);

        if( aapl->debug >= 5 )
        {
            if( strtol && recv_data_back != 3 )
                aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "%s -> 0x%08x\n", command, aapl->data);
            else
                aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "%s -> %s\n", command, aapl->data_char);
        }

        if (aapl->data_char[0] == 0x1 && aapl->data_char[1] == 0x1) aapl_str_rep(aapl->data_char, 0x2, 0xa);
        return aapl->data_char;
    }

#endif

    (void)aapl;
    (void)command;
    (void)recv_data_back;
    (void)strtol;
    return "";
}
