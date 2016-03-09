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
#include "types.h"
#include "bits.h"
#include "io.h"
#include "regs.h"
#include "mbox.h"

/***************************************************************************************************
  * mbox_receive - BLOCKING
  *
  * cmd - command buffer
  * args - arguments buffer
  *
  * return: 0 on success
 ***************************************************************************************************/
u32 mbox_receive(u32 *cmd, u32 *args)
{
	u32 n, status = NO_ERROR;
	u32 regval;

	if ((cmd == 0) || (args == 0)) {
		status = ERR_INVALID_ARGUMENT;
		goto rx_exit;
	}

	while (!(readl(SP_CONTROL) & CMD_REG_OCCUPIED_BIT))
		/* continue polling */;

	*cmd = readl(MAILBOX_IN_CMD) & MAILBOX_CMD_MASK;

	for (n = 0; n < MAILBOX_MAX_ARGS; n++)
		args[n] = readl(MAILBOX_IN_ARG(n));

rx_exit:
	/* Reset INT */
	writel(CMD_REG_OCCUPIED_RESET_BIT, SP_INT_RESET);
	regval = readl(SP_CONTROL) & ~CMD_REG_OCCUPIED_BIT;
	writel(regval, SP_CONTROL);

	return status;
}

/***************************************************************************************************
  * mbox_send
  *
  * status - status to return to host
  * args - arguments to return to host
  * nargs - number or argumens
 *
  * return: 0 on success
 ***************************************************************************************************/
void mbox_send(u32 status, u32 *args, u32 nargs)
{
	u32 n, regval;

	if ((args == 0) || (nargs >= MAILBOX_MAX_ARGS))
		return;

	for (n = 0; n < nargs; n++)
		writel(args[n], MAILBOX_OUT_ARG(n));

	/* Set status */
	writel(status, MAILBOX_OUT_STATUS);
	/* Pop host INT CMD complete */
	regval = readl(HOST_INT_SET) | HOST_INT_CMD_COMPLETE_BIT;
	writel(regval, HOST_INT_SET);
}
