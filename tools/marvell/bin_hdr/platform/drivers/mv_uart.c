/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "config_marvell.h"     /* Required to identify SOC and Board */
#include "soc_spec.h"
#include "bootstrap_os.h"
#if defined(MV88F78X60)
#include "ddr3_axp.h"
#elif defined(MV88F6710)
#include "ddr3_a370.h"
#elif defined(MV88F66XX)
#include "ddr3_alp.h"
#elif defined(MV88F672X)
#include "ddr3_a375.h"
#elif defined(MV88F68XX)
#include "ddr3_a38x.h"
#elif defined(MV_MSYS_BC2)
#include "ddr3_msys_bc2.h"
#elif defined(MV_MSYS_AC3)
#include "ddr3_msys_ac3.h"
#else
#error "No SOC define for uart in binary header."
#endif
#ifdef MV88F68XX
#include "mvSysEnvLib.h"
#endif

#include "mvUart.h"

extern MV_U32 mvBoardTclkGet(MV_VOID);

/*******************************************************************************
* mvUartInit - Init a uart port.
*
* DESCRIPTION:
*       This routine Initialize one of the uarts ports (channels).
*   It initialize the baudrate, stop bit,parity bit etc.
*
* INPUT:
*       port - uart port number.
*   baudDivisor - baud divisior to use for the uart port.
*
* OUTPUT:
*       None.
*
* RETURN:
*   None.
*
*******************************************************************************/
#if !defined(MV_NO_PRINT)
MV_U32 uartOffset = UART0_REG_OFFSET;
#endif

MV_VOID mvUartInit(void)
{
#if !defined(MV_NO_PRINT)
	MV_U32  tmpTClkRate;
	MV_U32 baudDivisor = 0;

	tmpTClkRate = mvBoardTclkGet();

#ifdef MV88F68XX
	/* UART1 on DB-AP board is the default UART interface
	 - Init the TWSI to read the board ID from the EEPROM
	 - Update the global Uart interface to use UART1 register offset */
	DEBUG_INIT_FULL_S("mvHwsTwsiInitWrapper: Init TWSI interface.\n");
	mvHwsTwsiInitWrapper();

	if( mvBoardIdGet() == DB_AP_68XX_ID)
		uartOffset = UART1_REG_OFFSET;
#endif
	volatile MV_UART_PORT *pUartPort = (volatile MV_UART_PORT *)(INTER_REGS_BASE + uartOffset);
	/*  UART Init */
	switch (tmpTClkRate) {
		case _25MHZ:
			baudDivisor =  13; /* actually 13.5 */
			break;
		case _166MHZ:
			baudDivisor =  90;
			break;
		case _200MHZ:
			baudDivisor = 108;
			break;
		case _250MHZ:
		default:
			baudDivisor =  135;
			break;
	}

    pUartPort->ier = 0x00;
    pUartPort->lcr = LCR_DIVL_EN;           /* Access baud rate */
    pUartPort->dll = baudDivisor & 0xff;    /* 9600 baud */
    pUartPort->dlm = (baudDivisor >> 8) & 0xff;
    pUartPort->lcr = LCR_8N1;               /* 8 data, 1 stop, no parity */

    /* Clear & enable FIFOs */
    pUartPort->fcr = FCR_FIFO_EN | FCR_RXSR | FCR_TXSR;
#endif
    return;
}

/*******************************************************************************
* mvUartPutc - write one character to the UART-0
*
* DESCRIPTION:
*       This routine puts one charachetr on one of the uart ports.
*
* INPUT:
*       port - uart port number.
*   c - character.
*
* OUTPUT:
*       None.
*
* RETURN:
*   None.
*
*******************************************************************************/
MV_VOID mvUartPutc(MV_U8 c)
{
#if !defined(MV_NO_PRINT)
    volatile MV_UART_PORT *pUartPort = (volatile MV_UART_PORT *)(INTER_REGS_BASE + uartOffset);
    while ((pUartPort->lsr & LSR_THRE) == 0) ;
    pUartPort->thr = c;
#endif
    return;
}

/*-----------------------------------------------------------------------------------       */
/* Name:            putstring       */
/*      */
/* Description:     This function writes a string to the serial port.       */
/*      */
/* Input value:     char *str           */
/*      */
/* Return Value:    none            */
/*-----------------------------------------------------------------------------------       */
void putstring(char *str)
{
#if !defined(MV_NO_PRINT)
/* For each character in the string...      */
    while (*str != '\0')
    {

        /* Write it to the serial port  */
        mvUartPutc(*str);
        if (*str == '\n') mvUartPutc('\r');

        str++;
    }
#endif
}

/*-----------------------------------------------------------------------------------   */
/* Name:            putdata */
/*                                                                                      */
/* Description:     This function convert a decimal number into hex character string    */
/*          and put this string into the serial port.   */
/*                                                                                      */
/* Input value:     unit16 dec_num      */
/*                                                                                      */
/* Return Value:    none        */
/*-----------------------------------------------------------------------------------   */
void putdata (u32 dec_num,u32 length)
{
#if !defined(MV_NO_PRINT)
    char str[11];
    u32 i, flag = 0, mod_val;/*, length = 8;*/

    /* Initial the modular value    */
    mod_val = dec_num;
    for (i=length; i > 0; i--)
    {
    /* compute 1 hex number at a time   */
    if (dec_num >= 16)
    {
         mod_val = dec_num & (16 -1);
         dec_num = (dec_num >> 4);

        /*mod_val = dec_num % 16;
        dec_num = dec_num / 16; */
    }
    /* set a flag to indicate the last digit    */
    else
    {
        mod_val = dec_num;
        flag = 1;
        }
    /* convert into ASCII hex number range from 0-9 */
    if (mod_val <= 9)
    {
        str[i-1] = mod_val + 48;
    }
    /* convert into ASCII hex number range from A-F     */
    else
    {
        str[i-1] = mod_val + 55;
    }
    /* indicate the last computed digit, so set the remaining digit to 0    */
    if (flag)
    {
        dec_num = 0;
    }
    }

    /*str[8] = '\n';
    str[9] = '';
    str[10] = '\0';*/
    str[length] = '\0';
    putstring(str);

#endif
}
/*-----------------------------------------------------------------------------------   */
/* Name:            putdataDec  */
/*                                                                                      */
/* Description:     This function prints a decimal number into character string     */
/*          and put this string into the serial port.   */
/*                                                                                      */
/* Input value:     unit16 dec_num      */
/*                                                                                      */
/* Return Value:    none        */
/*-----------------------------------------------------------------------------------   */
void putdataDec (u32 dec_num,u32 length)
{
    char str[11];
    u32 i;/*, length = 8;*/

    for (i=length; i > 0; i--)
    {
        str[i-1] = (dec_num % 10) + '0';
        dec_num = dec_num / 10;
    }
    str[length] = '\0';
    /* replace leading zeroes with blank */
    for (i=0; i < (length-1); i++)
    {
        if ('0' == str[i])
		str[i] = ' ';
	else
		break;
    }
    putstring(str);
}
/*******************************************************************************
* mvUartGetc - Read one character from the UART
*
* DESCRIPTION:
*       This routine gets one charachetr from one of the uart ports.
*
* INPUT:
*       port - uart port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*   carachter from the uart port.
*
*******************************************************************************/
MV_U8   mvUartGetc()
{
#if !defined(MV_NO_INPUT)
	 volatile MV_UART_PORT *pUartPort = (volatile MV_UART_PORT *)(INTER_REGS_BASE + uartOffset);
    while ((pUartPort->lsr & LSR_DR) == 0) ;
    return (pUartPort->rbr);
#else
    return 0xff;
#endif
}

/*******************************************************************************
* mvUartTstc - Check if new data available in the UART.
*
* DESCRIPTION:
*       This routine heck if a charachter is ready to be read from one of the
*   the uart ports.
*
* INPUT:
*       port - uart port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*   None.
*
*******************************************************************************/
MV_BOOL mvUartTstc()
{
#if !defined(MV_NO_INPUT)
    volatile MV_UART_PORT *pUartPort = (volatile MV_UART_PORT *)(INTER_REGS_BASE + uartOffset);
    return ((pUartPort->lsr & LSR_DR) != 0);
#else
    return FALSE;
#endif /* #if !defined(MV_NO_INPUT) */
}
