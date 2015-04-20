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

#ifndef GPIO_GPIO_H_
#define GPIO_GPIO_H_

/* Defines for to set the port directions and set and get the port value. */
/* These settings work on a raspberry PI, board revision 2. */
/* Pin assignment for JTAG and MDIO */
#define TCK_port  RPI_V2_GPIO_P1_26
#define TMS_port  RPI_V2_GPIO_P1_24
#define TDI_port  RPI_V2_GPIO_P1_21
#define TDO_port  RPI_V2_GPIO_P1_19
#define TRST_port RPI_V2_GPIO_P1_23
#define MDC_port  RPI_V2_GPIO_P1_07
#define MDIO_port RPI_V2_GPIO_P1_22

#define GPIO_SET_DIR(port, dir)     bcm2835_gpio_fsel(port, dir)
#define PORT_DIR_OUTPUT             BCM2835_GPIO_FSEL_OUTP
#define PORT_DIR_INPUT              BCM2835_GPIO_FSEL_INPT
#define GPIO_SET_VALUE(port, value) bcm2835_gpio_write(port, value)
#define GPIO_GET_VALUE(port)        bcm2835_gpio_lev(port)

#define MDIO_EXTRA_CYCLES
#define MDIO_DELAY 1

#define MDC_CYCLE {                                                                                         \
    int count;                                                                                              \
    int mdio_loop_cnt = MDIO_DELAY;                                                                         \
    aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "MDIO clock = 1; MDIO data = %c\n", MDIO_data); \
    for (count = 0; count <= mdio_loop_cnt; count++)                                                        \
    { GPIO_SET_VALUE(MDC_port, 1); }                                                                        \
    aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "MDIO clock = 0; MDIO data = %c\n", MDIO_data); \
    for (count = 0; count <= mdio_loop_cnt; count++)                                                        \
    {  GPIO_SET_VALUE(MDC_port, 0); }                                                                       \
}

/* User defined functions */
EXT int avago_gpio_mdio_open(Aapl_t *aapl);
EXT int avago_gpio_mdio_close(Aapl_t *aapl);
EXT int aapl_gpio_gpio_init(void);
EXT unsigned int aapl_gpio_mdio_function(Aapl_t *aapl, Avago_mdio_cmd_t mdio_cmd, unsigned int port_addr, unsigned int dev_addr, unsigned int reg_addr, unsigned int data);

#endif
