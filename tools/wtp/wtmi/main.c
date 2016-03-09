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
#include "mbox.h"
#include "efuse.h"
#include "clock.h"
#include "wtmi.h"

/***************************************************************************************************
  * exception_handler
  *
  * return: None
 ***************************************************************************************************/
static void exception_handler(int exception)
{
	return;
}

/***************************************************************************************************
  * cmd_execute
  *
  * return: None
 ***************************************************************************************************/
u32 cmd_execute(u32 cmd, u32 *args, u32 *nargs)
{
	u32 row;
	u32 status;

	*nargs = 0;

	if ((cmd >= CMD_WRITE_EFUSE_ROW_BASE) &&
	    (cmd <= (CMD_WRITE_EFUSE_ROW_BASE + EFUSE_MAX_ROW))) {
		/* WRITE */
		row = cmd - CMD_WRITE_EFUSE_ROW_BASE;
		status = efuse_write(row, args);
	} else if ((cmd >= CMD_READ_EFUSE_ROW_BASE) &&
		   (cmd <= (CMD_READ_EFUSE_ROW_BASE + EFUSE_MAX_ROW))) {
		/* READ */
		row = cmd - CMD_READ_EFUSE_ROW_BASE;
		*nargs = EFUSE_WORDS_IN_ROW;
		status = efuse_read(row, args);
	} else {
		/* ERROR */
		status = ERR_INVALID_COMMAND;
	}

	return status;
}

/***************************************************************************************************
  * main
  *
  * return: None
 ***************************************************************************************************/
int main(int exception, char **dummy)
{
	u32 cmd, args[MAILBOX_MAX_ARGS];
	u32 status, nargs;

	if (exception != 0) {
		exception_handler(exception);
		return NO_ERROR;
	}

	/* Initialization stuff */
	clock_init();

	/* Mailbox commands handling loop */
	while (1) {
		status = mbox_receive(&cmd, args);
		if (status == NO_ERROR)
			status = cmd_execute(cmd, args, &nargs);

		if (status != NO_ERROR)
			nargs = 0;


		mbox_send(status, args, nargs);

	} /* read and execute mailbox commands */

	return NO_ERROR;
}
