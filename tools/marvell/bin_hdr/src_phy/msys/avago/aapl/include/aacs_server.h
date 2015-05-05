/* AAPL Revision: 2.0.0-beta */
/* AAPL (ASIC and ASSP Programming Layer) aacs_server exported functions */

/** Doxygen File Header */
/** @file */
/** @brief Declarations for aacs_server function. */

#ifndef AVAGO_AACS_SERVER_H_
#define AVAGO_AACS_SERVER_H_

/* Create a new AACS server to handle incoming connection at TCP port tcp_port: */
EXT int avago_aacs_server(Aapl_t *aapl, int tcp_port);

EXT char *avago_aacs_process_cmd(Aapl_t *aapl, const char *cmd, int *chip_nump, int *ring_nump);

#endif
