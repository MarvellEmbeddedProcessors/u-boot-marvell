/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

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

*******************************************************************************/

/*
 * serial.c - serial support.
 */

#include <common.h>
#include <command.h>
#include "mvCommon.h"
#include "uart/mvUart.h"
#include "cpu/mvCpu.h"

extern unsigned int whoAmI(void);

extern void print_mvBanner(void);

int serial_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	int clock_divisor = (CONFIG_SYS_TCLK / 16)/gd->baudrate;

	/* muti-core support, initiate each Uart to each cpu */

	mvUartInit(whoAmI(), clock_divisor, mvUartBase(whoAmI()));

#if !defined(DB_78X60_PCAC) && !defined(DB_78X60_PCAC_REV2)
	/* print banner */
	print_mvBanner();
#endif

	return (0);
}

void serial_putc(const char c)
{
	if (c == '\n')

	mvUartPutc(whoAmI(), '\r');
	mvUartPutc(whoAmI(), c);
}

int serial_getc(void)
{
	return mvUartGetc(whoAmI());
}

int serial_tstc(void)
{
	return mvUartTstc(whoAmI());
}

void serial_setbrg (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	int clock_divisor = (CONFIG_SYS_TCLK / 16)/gd->baudrate;

	/* muti-core support, initiate each Uart to each cpu */
	mvUartInit(whoAmI(), clock_divisor, mvUartBase(whoAmI()));
}

void serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}

#if defined(CONFIG_CMD_KGDB)
void kgdb_serial_init(void){}

void putDebugChar (int c)
{
	serial_putc (c);
}

void putDebugStr (const char *str)
{
	serial_puts (str);
}

int getDebugChar (void)
{
	return serial_getc();
}

void kgdb_interruptible (int yes)
{
	return;
}
#endif	/* CFG_CMD_KGDB	*/
