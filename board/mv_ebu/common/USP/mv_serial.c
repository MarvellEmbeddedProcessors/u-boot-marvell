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
#include <serial.h>

extern unsigned int whoAmI(void);

extern void print_mvBanner(void);

int mv_serial_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	int clock_divisor = (CONFIG_SYS_TCLK / 16)/gd->baudrate;

	/* muti-core support, initiate each Uart to each cpu */
	mvUartInit(whoAmI(), clock_divisor, mvUartBase(whoAmI()));

	console_init_f();

#if !defined(DB_78X60_PCAC) && !defined(DB_78X60_PCAC_REV2)
	/* print banner */
	print_mvBanner();
#endif

	return (0);
}

void mv_serial_putc(const char c)
{
	if (c == '\n')

	mvUartPutc(whoAmI(), '\r');
	mvUartPutc(whoAmI(), c);
}

int mv_serial_getc(void)
{
	return mvUartGetc(whoAmI());
}

int mv_serial_tstc(void)
{
	return mvUartTstc(whoAmI());
}

void mv_serial_setbrg (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	int clock_divisor = (CONFIG_SYS_TCLK / 16)/gd->baudrate;

	/* muti-core support, initiate each Uart to each cpu */
	mvUartInit(whoAmI(), clock_divisor, mvUartBase(whoAmI()));
}

void mv_serial_puts (const char *s)
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

static struct serial_device mv_serial_drv = {
	.name	= "mv_serial",
	.start	= mv_serial_init,
	.stop	= NULL,
	.setbrg	= mv_serial_setbrg,
	.putc	= mv_serial_putc,
	.puts	= default_serial_puts,
	.getc	= mv_serial_getc,
	.tstc	= mv_serial_tstc,
};

void mv_serial_initialize(void)
{
	serial_register(&mv_serial_drv);
}

__weak struct serial_device *default_serial_console(void)
{
	return &mv_serial_drv;
}
