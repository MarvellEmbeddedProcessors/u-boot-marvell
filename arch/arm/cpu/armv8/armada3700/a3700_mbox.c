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
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-armada3700/mbox.h>

/******************************************************************************
 *	mbox_send
 ******************************************************************************/
int mbox_send(enum mbox_opsize opsz, enum mbox_op op, uint32_t row, uint32_t offs, uint32_t *args)
{
	uint32_t	params[MBOX_MAX_ARGS];
	uint32_t	n, params_to_send;

	if (args == 0) {
		printf("%s: Invalid argument\n", __func__);
		return 1;
	}

	if (op != MB_OP_READ && op != MB_OP_WRITE) {
		printf("%s: Invalid operation\n", __func__);
		return 1;
	}

	memset(params, 0, MBOX_MAX_ARGS * sizeof(uint32_t));

	/* First parameter in the list describes eFuse row */
	params[0] = row;

	switch (opsz) {
	case MB_OPSZ_BIT:
	case MB_OPSZ_BYTE:
	case MB_OPSZ_WORD:
		params_to_send = 3;
		params[1] = offs;
		params[2] = args[0];
		break;
	case MB_OPSZ_DWORD:
		params_to_send = 3;
		params[1] = args[0];
		params[2] = args[1];
		break;
	case MB_OPSZ_256B:
		params_to_send = 9;
		memcpy(&params[1], args, 8 * sizeof(uint32_t));
		break;
	default:
		printf("%s: Invalid size\n", __func__);
		return 1;
	}

	/* First, fill all command arguments */
	for (n = 0; n < params_to_send; n++) {
		debug("=>MBOX WRITE PARAM[%d] = %08X\n", n, params[n]);
		writel(params[n], MBOX_SEND_ARG_OFFS(n));
	}

	/* Writing command triggers mailbox dispatch and
	   intarrupt on secure CPU side */
	debug("=>MBOX WRITE CMD = %08X\n", MBOX_COMMAND(opsz, op));
	writel(MBOX_COMMAND(opsz, op), MBOX_SEND_CMD_OFFS);

	return 0;
}

/******************************************************************************
 *	mbox_receive - BLOCKING
 ******************************************************************************/
int mbox_receive(enum mbox_status *stat, uint32_t *args, uint32_t timeout_us)
{
	uint32_t n;
	uint32_t regval;

	if (args == 0) {
		*stat = MB_STAT_BAD_ARGUMENT;
		return 1;
	}

	/* Poll for secure CPU command completion */
	for (n = 0; n < timeout_us; n++) {
		regval = readl(MBOX_SEC_CPU_INT_STAT_REG);
		if (regval & MBOX_SEC_CPU_CMD_SET)
			break;
		mdelay(100);
	}

	if (n == timeout_us) {
		printf("%s: MB timeout\n", __func__);
		return 1;
	}

	/* Read comamnd status and arguments */
	for (n = 0; n < MBOX_MAX_ARGS; n++) {
		args[n] = readl(MBOX_RECEIVE_ARG_OFFS(n));
		debug("<=MBOX READ ARG[%d] = %08X\n", n, args[n]);
	}

	*stat = readl(MBOX_RECEIVE_STAT_OFFS);
	debug("<=MBOX READ STATUS = %08X\n", *stat);

	/* Reset host interrupt */
	regval = readl(MBOX_HOST_INT_RESET) | MBOX_SEC_CPU_CMD_COMPLETE;
	writel(regval, MBOX_HOST_INT_RESET);

	return 0;
}
