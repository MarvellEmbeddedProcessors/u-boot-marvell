/*
 * ***************************************************************************
 * Copyright (C) 2015 Marvell International Ltd.
 * ***************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************
 */

#include <common.h>
#include <asm/io.h>
#include <serial.h>
#include <linux/compiler.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/fdt.h>

DECLARE_GLOBAL_DATA_PTR;

/* to use global variables before u-boot relocation, initialize it to something !=0 */
unsigned long mvebu_uart_reg_base = 100;
unsigned long mvebu_uart_clock_frequency = 100;
unsigned long mvebu_uart_baudrate = 100;

#define MVEBU_UART_BASE(x)	mvebu_uart_reg_base

/* max uart port num */
#define CONFIG_MAX_UART_NUM	2

#define UART_DRIVER_NAME	"serial"

/*
 * Register Offset
 */
#define UART_RX_REG		0x00
#define UART_TX_REG		0x04
#define UART_CTRL_REG		0x08
#define UART_STATUS_REG		0x0c
#define UART_BAUD_REG		0x10
#define UART_POSSR_REG		0x14


#define UART_STATUS_RX_RDY	0x10
#define UART_STATUS_TXFIFO_FULL			0x800

#define UART_CTRL_RXFIFO_RESET		0x4000
#define UART_CTRL_TXFIFO_RESET		0x8000

/* Read from APB register into buffer in memory */
#define UART_REG_READ(_reg)			readl(MVEBU_UART_BASE(0) + _reg)

/* Write from buffer in memory into APB register */
#define UART_REG_WRITE(_reg, pdata)		writel(pdata, MVEBU_UART_BASE(0) + _reg)

static void mvebu_serial_setbrg(void)
{
	/*
	 * calculate divider.
	 * baudrate = clock / 16 / divider
	 */
	UART_REG_WRITE(UART_BAUD_REG, (mvebu_uart_clock_frequency / mvebu_uart_baudrate / 16));
	/* set Programmable Oversampling Stack to 0, UART defaults to 16X scheme */
	UART_REG_WRITE(UART_POSSR_REG, 0);
}

/*
 * Read a single byte from the serial port.
 */
static int mvebu_serial_getc(void)
{
	while (!(UART_REG_READ(UART_STATUS_REG) & UART_STATUS_RX_RDY))
		/* No Operation */;

	return UART_REG_READ(UART_RX_REG) & 0xFF;
}

/*
 * Output a single byte to the serial port.
 */
static void mvebu_serial_putc(const char c)
{
	if (c == '\n')
		serial_putc('\r');

	while (UART_REG_READ(UART_STATUS_REG) & UART_STATUS_TXFIFO_FULL)
		/* NOP */;

	UART_REG_WRITE(UART_TX_REG, (unsigned short)c);
}

/*
 * Test whether a character is in the RX buffer
 */
static int mvebu_serial_tstc(void)
{
	if (UART_REG_READ(UART_STATUS_REG) & UART_STATUS_RX_RDY)
		return 1;

	return 0;
}

static int mvebu_serial_init(void)
{
	int node_list[CONFIG_MAX_UART_NUM], node;
	u32 i, count;

	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "serial",
			COMPAT_MVEBU_UART, node_list, 2);

	/* in FDT file, there should be only one "serial" node that are enabled,
	 * which has the 'reg' attribute for register base of UART unit */
	for (i = 0; i < count ; i++) {
		node = node_list[i];

		if (node <= 0)
			continue;

		mvebu_uart_reg_base = (unsigned long)fdt_get_regs_offs(gd->fdt_blob, node, "reg");
		mvebu_uart_clock_frequency = (unsigned long)fdtdec_get_int(gd->fdt_blob, node, "clock_frequency", 0);
		mvebu_uart_baudrate = (unsigned long)fdtdec_get_int(gd->fdt_blob, node, "baudrate", 0);
	}

	/* 115200KBps fixed Baud rate  */
	mvebu_serial_setbrg();

	/* reset FIFOs */
	UART_REG_WRITE(UART_CTRL_REG, UART_CTRL_RXFIFO_RESET | UART_CTRL_TXFIFO_RESET);

	udelay(1);

	/* No Parity, 1 Stop */
	UART_REG_WRITE(UART_CTRL_REG, 0);
	return 0;
}

static struct serial_device mvebu_serial_drv = {
	.name	= UART_DRIVER_NAME,
	.start	= mvebu_serial_init,
	.stop	= NULL,
	.setbrg	= mvebu_serial_setbrg,
	.putc	= mvebu_serial_putc,
	.puts	= default_serial_puts,
	.getc	= mvebu_serial_getc,
	.tstc	= mvebu_serial_tstc,
};

void mvebu_serial_initialize(void)
{
	serial_register(&mvebu_serial_drv);
}

__weak struct serial_device *default_serial_console(void)
{
	return &mvebu_serial_drv;
}
