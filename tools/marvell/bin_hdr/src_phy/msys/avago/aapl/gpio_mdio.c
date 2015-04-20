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

/** Doxygen File Header */
/** @file */
/** @brief Raspberry PI GPIO example */

#include "aapl.h"

#if AAPL_ALLOW_GPIO_MDIO
typedef enum {mw_data, mw_addr} mw_t;
static char MDIO_data = '?';

/* Forward declarations */
void aapl_gpio_mdio_write(Aapl_t *aapl, const mw_t type, const int prtad, const int devad, const int data);
void aapl_gpio_mdio_read(Aapl_t *aapl, const int prtad, const int devad, unsigned int * datap);
void aapl_gpio_mdio_start(Aapl_t *aapl, const int op, const int prtad, const int devad);
void aapl_gpio_mdio_send_bit(Aapl_t *aapl, const int out);
int aapl_gpio_mdio_get_bit(Aapl_t *aapl);

int avago_gpio_mdio_open(Aapl_t *aapl)
{
    static BOOL initialized = FALSE;
    if( !initialized && aapl_gpio_gpio_init() < 0)
        return aapl_fail(aapl,__func__,__LINE__,"Failed to Initialize GPIO\n");
    initialized = TRUE;
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "open status = 0\n");
    return 0;
}

int avago_gpio_mdio_close(Aapl_t *aapl)
{
    int rc = 0;
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "close status = %d\n", rc);
    return rc;
}

/* This is the main function that is called from the Avago AAPL. Based on the  */
/* requested mdio command, call the aapl_gpio_mdio_write or aapl_gpio_mdio_read function. */
unsigned int aapl_gpio_mdio_function(
    Aapl_t *aapl,
    Avago_mdio_cmd_t mdio_cmd,
    unsigned int port_addr,
    unsigned int dev_addr,
    unsigned int reg_addr,
    unsigned int data)
{

    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "MDIO_cmd = %d; Port Addr = %d; Dev Addr = %d; Register Addr = %d; Data = %d\n", mdio_cmd, port_addr, dev_addr, reg_addr, data);

    switch (mdio_cmd)
    {
    case AVAGO_MDIO_ADDR:
        aapl_gpio_mdio_write(aapl, mw_addr, port_addr, dev_addr, reg_addr);
        break;
    case AVAGO_MDIO_WRITE:
        aapl_gpio_mdio_write(aapl, mw_data, port_addr, dev_addr, data);
        break;
    case AVAGO_MDIO_READ:
        aapl_gpio_mdio_read(aapl, port_addr, dev_addr, &data);
        break;
    default:
        aapl_fail(aapl, __func__, __LINE__, "Unrecognized mdio command %d\n", mdio_cmd);
    }

    return data;

}

/*============================================================================================ */
/* M D I O   W R I T E */
/* */
/* Given: */
/* */
/* - mw_t for type of operation */
/* - prtad = port   address, 0..31 */
/* - devad = device address, 0..31 */
/* - data, 16 bits */
/* */
/* Write a Clause 45 (only) management frame to the MDIO bus, which is actually */
/* a pair of uproc GPIO lines ("bit-banging" the I/O). */
/* IEEE P802.3/D2.1, Clause 45 management frame fields: */
/* */
/*         PRE   ST OP PRTAD DEVAD TA ADDRESS/DATA     IDLE */
/*   Addr  1...1 00 00 PPPPP EEEEE 10 AAAAAAAAAAAAAAAA Z */
/*   Write 1...1 00 01 PPPPP EEEEE 10 DDDDDDDDDDDDDDDD Z */
/*   Read  1...1 00 11 PPPPP EEEEE Z0 DDDDDDDDDDDDDDDD Z  # slave sends data. */
/* # Incr  1...1 00 10 PPPPP EEEEE Z0 DDDDDDDDDDDDDDDD Z  # unsupported by our ASICs. */
/* */
/* Where "PRE" = 32 contiguous 1s, "TA" = turnaround, and "Z" means the master */
/* puts the line in high impedance. */
void aapl_gpio_mdio_write(Aapl_t *aapl, const mw_t type, const int prtad, const int devad, const int data)
{
    int at;
    aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "Type = %d; Port Addr = %d; Dev Addr = %d; Data = %d\n", type, prtad, devad, data);

    aapl_gpio_mdio_start(aapl, /* op = */ (type == mw_data) ? 1 : 0, prtad, devad);
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Write TA state: send 10\n");
    aapl_gpio_mdio_send_bit(aapl, 1); aapl_gpio_mdio_send_bit(aapl, 0);
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Send the data, data = %d\n", data);
    for (at = 15; at >= 0; --at)
    {
        aapl_gpio_mdio_send_bit(aapl, (data >> at) & 1);
    }

    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Set MDIO port to high Z\n");
    GPIO_SET_DIR(MDIO_port, PORT_DIR_INPUT); MDIO_data = ('Z');
#ifdef MDIO_EXTRA_CYCLES
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Send two additional clock cycles\n");
    MDC_CYCLE; MDC_CYCLE;
#endif
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Set Clock port to high Z\n");
    GPIO_SET_DIR(MDC_port,  PORT_DIR_INPUT);

}

/*============================================================================================ */
/* M D I O   R E A D */
/* */
/* Given: */
/* */
/* - prtad = port   address, 0..31 */
/* - devad = device address, 0..31 */
/* - pointer to data, where to return 16 bits */
void aapl_gpio_mdio_read(Aapl_t *aapl, const int prtad, const int devad, unsigned int * datap)
{
    int at;
    aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "Port Addr = %d; Dev Addr = %d\n", prtad, devad);

    aapl_gpio_mdio_start(aapl, /* op = */ 3, prtad, devad);
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Read TA state: MDIO data = high Z for two clock cycles.\n");
    GPIO_SET_DIR(MDIO_port, PORT_DIR_INPUT); MDIO_data = ('Z');
    MDC_CYCLE; MDC_CYCLE;

    *datap = 0;
    for (at = 15; at >= 0; --at)
    {
        *datap = (*datap << 1) | aapl_gpio_mdio_get_bit(aapl);
    }
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Read data: data = %d\n", *datap);

#ifdef MDIO_EXTRA_CYCLES
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Send two additional clock cycles\n");
    MDC_CYCLE; MDC_CYCLE;
#endif
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Set Clock port to high Z\n");
    GPIO_SET_DIR(MDC_port, PORT_DIR_INPUT);
}

/*============================================================================================ */
/* M D I O   S T A R T */
/* */
/* Given an MDIO operation (OP: addr = 0, write = 1, read = 3), and prtad (port */
/* address) and devad (device address), both 0..31, start an MDIO transfer. */
void aapl_gpio_mdio_start(Aapl_t *aapl, const int op, const int prtad, const int devad)
{
    aapl_log_printf(aapl, AVAGO_DEBUG7, __func__, __LINE__, "Port Addr = %d; Dev Addr = %d\n", prtad, devad);

    GPIO_SET_VALUE(MDC_port, 0);


    GPIO_SET_DIR(MDIO_port, PORT_DIR_OUTPUT);
    GPIO_SET_DIR(MDC_port,  PORT_DIR_OUTPUT);

    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Send stop bits (2 bits): 0x00\n");
    aapl_gpio_mdio_send_bit(aapl, 0); aapl_gpio_mdio_send_bit(aapl, 0);
    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Send OP bits (2 bits): 0x%x\n", op);
    aapl_gpio_mdio_send_bit(aapl, op >> 1); aapl_gpio_mdio_send_bit(aapl, op & 1);

    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Send port address bits (5 bits): 0x%x\n", prtad);
    aapl_gpio_mdio_send_bit(aapl, (prtad >> 4) & 1);
    aapl_gpio_mdio_send_bit(aapl, (prtad >> 3) & 1);
    aapl_gpio_mdio_send_bit(aapl, (prtad >> 2) & 1);
    aapl_gpio_mdio_send_bit(aapl, (prtad >> 1) & 1);
    aapl_gpio_mdio_send_bit(aapl,  prtad       & 1);

    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "Send device address bits (5 bits): 0x%x\n", devad);
    aapl_gpio_mdio_send_bit(aapl, (devad >> 4) & 1);
    aapl_gpio_mdio_send_bit(aapl, (devad >> 3) & 1);
    aapl_gpio_mdio_send_bit(aapl, (devad >> 2) & 1);
    aapl_gpio_mdio_send_bit(aapl, (devad >> 1) & 1);
    aapl_gpio_mdio_send_bit(aapl,  devad       & 1);
}

/*============================================================================================ */
/* M D I O   S E N D   B I T */
/* */
/* Given one bit value (0/1), clock it out to the MDIO bus. */
/* Assumes the caller already set line directions as appropriate. */
void aapl_gpio_mdio_send_bit(Aapl_t *aapl, const int out)
{
    GPIO_SET_VALUE(MDIO_port, out);
    MDIO_data = (out ? '1' : '0');
    MDC_CYCLE;
}

/*============================================================================================ */
/* M D I O   G E T   B I T */
/* */
/* Clock one bit (0/1) in from the MDIO bus. */
/* Assumes the caller already set line directions as appropriate. */
int aapl_gpio_mdio_get_bit(Aapl_t *aapl)
{
    int bit = GPIO_GET_VALUE(MDIO_port);
    MDC_CYCLE;
    return(bit);
}

/*============================================================================================ */
/* A A P L  U S E R  G P I O  I N I T */
/* */
/* Initialize the GPIO.  This function is specific to each processor or micro-controller */
/* This implementation is based on the Raspberry PI, using the bcm2835 package. */
/* Access from ARM Running Linux */
int aapl_gpio_gpio_init()
{
    if (!bcm2835_init())
        return -1;

    GPIO_SET_DIR(MDC_port,  PORT_DIR_OUTPUT);
    GPIO_SET_DIR(MDIO_port, PORT_DIR_OUTPUT);

    return 0;
}


#endif
