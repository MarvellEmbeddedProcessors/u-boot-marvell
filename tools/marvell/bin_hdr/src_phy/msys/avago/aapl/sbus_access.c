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

/* AAPL Revision: 1.2.1 */
/* Template for the user_supplied functions. */
/* */

/** Doxygen File Header */
/** @file */
/** @brief User-supplied functions. */
#include "aapl.h"
#ifndef MV_HWS_BIN_HEADER
#include <common/siliconIf/mvSiliconIf.h>
#else
/* use mv_hws_avago_if.h for genRegisterSet/Get functions */
#include <mv_hws_avago_if.h>
#include <gtOs/gtGenTypes.h>
#include <mvSiliconIf.h>
#endif

#define SBC_UNIT_BASE_ADDRESS         (0x60000000)
#define SBC_UNIT_REG_ADDR(reg)        (SBC_UNIT_BASE_ADDRESS | reg)
#define SBC_UNIT_COMMOM_CTRL_REG_ADDR (0x0)
#define SBC_UNIT_SOFT_RESET           (0x1)
#define SBC_UNIT_INTERNAL_ROM_ENABLE  (0x2)

#define SBC_MASTER_BASE_ADDRESS       (0x60040000)
#define SBC_MASTER_SERDES_NUM_SHIFT   (10)
#define SBC_MASTER_REG_ADDR_SHIFT     (2)

/*******************************************************************************
* user_supplied_pex_address
*
* DESCRIPTION:
*       Build PEX address from sbus_addr, and reg_addr
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pex register address
*******************************************************************************/
unsigned int user_supplied_pex_address
(
    unsigned char sbus_addr,
    unsigned char reg_addr
)
{
    unsigned int serdesAddress;

    serdesAddress = ((SBC_MASTER_BASE_ADDRESS)                  |
                     (sbus_addr << SBC_MASTER_SERDES_NUM_SHIFT) |
                     (reg_addr  << SBC_MASTER_REG_ADDR_SHIFT));

    return serdesAddress;
}

/*******************************************************************************
* mvHwsMgAccessDelayWA
*
* DESCRIPTION:
*       WA for MG access
*       Add configurable delay in MG Access
*       Default set to 10000 "nop"
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pex register address
*******************************************************************************/
void mvHwsMgAccessDelayWA(void)
{
    unsigned int delayIndex;
    for (delayIndex = 0; delayIndex < AVAGO_MG_ACCESS_THRESHOLD; delayIndex++)
    {
        __asm__("nop");
    }
}

/*******************************************************************************
* user_supplied_sbus_function
*
* DESCRIPTION:
*       Execute an sbus command
*
* INPUTS:
*       aapl      - Pointer to Aapl_t structure
*       addr      - SBus slice address
*       reg_addr  - SBus register to read/write
*       command   - 0 = reset, 1 = write, 2 = read
*       sbus_data - Data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Reads, returns read data
*       Writes and reset, returns 0
*******************************************************************************/
unsigned int user_supplied_sbus_function
(
    Aapl_t *aapl,
    unsigned int addr,
    unsigned char reg_addr,
    unsigned char command,
    unsigned int sbus_data,
    int recv_data_back)
{
    Avago_addr_t addr_struct;
    unsigned int commandAddress;
    unsigned int mask      = 0xFFFFFFFF;
    unsigned int data      = 0;
    unsigned int devNum    = aapl->devNum;
    unsigned int portGroup = aapl->portGroup;
    unsigned int rcode     = 0x0;

    avago_addr_to_struct(addr,&addr_struct);
    if (command == 1/*Write Command - WRITE_SBUS_DEVICE*/)
    {
        commandAddress = user_supplied_pex_address(addr_struct.sbus, reg_addr);
        mvHwsMgAccessDelayWA();
        genRegisterSet(devNum, portGroup, commandAddress, sbus_data, mask);
    }
    else if (command == 2/*Read Command - READ_SBUS_DEVICE*/)
    {
        commandAddress = user_supplied_pex_address(addr_struct.sbus, reg_addr);
        mvHwsMgAccessDelayWA();
        genRegisterGet (devNum, portGroup, commandAddress, &data, mask);
        /* return data in case of read command */
        rcode = data;
    }
    else if (command == 0/*Soft Reset - RESET_SBUS_DEVICE*/)
    {
        user_supplied_sbus_soft_reset(aapl);
    }
    else if (command == 3/*Hard Reset - CORE_SBUS_RESET*/)
    {
        /* This command is part of the entire chip reset */
        /* The AAPL start execute after chip reset, therefore this command can be removed */
    }

    return rcode;
}

/*******************************************************************************
* user_supplied_sbus_soft_reset
*
* DESCRIPTION:
*       Execute Software reset
*
* INPUTS:
*       aapl - Pointer to Aapl_t structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*******************************************************************************/
void user_supplied_sbus_soft_reset
(
    Aapl_t *aapl
)
{
    unsigned int mask      = 0xFFFFFFFF;
    unsigned int data      = 0;
    unsigned int devNum    = aapl->devNum;
    unsigned int portGroup = aapl->portGroup;

    /* Read Common control register */
    genRegisterGet(devNum, portGroup, SBC_UNIT_REG_ADDR(SBC_UNIT_COMMOM_CTRL_REG_ADDR), &data, mask);

    if (data & SBC_UNIT_SOFT_RESET)
        return;

    /* Set SBC in reset */
    data &= ~SBC_UNIT_SOFT_RESET;
    /* Clear internal ROM enable - loading ROM from the application */
    data &= ~SBC_UNIT_INTERNAL_ROM_ENABLE;
    genRegisterSet(devNum, portGroup, SBC_UNIT_REG_ADDR(SBC_UNIT_COMMOM_CTRL_REG_ADDR), data, mask);
    /* Take SBC out of reset */
    data |= SBC_UNIT_SOFT_RESET;
    genRegisterSet(devNum, portGroup, SBC_UNIT_REG_ADDR(SBC_UNIT_COMMOM_CTRL_REG_ADDR), data, mask);
}

