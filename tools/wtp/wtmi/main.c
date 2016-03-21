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
	u32			status;
	enum mbox_opsize	opsz;
	enum mbox_op		op;
	u32			row;
	u32			size;
	u32			offset;
	u32			*wr_args;

	op = MBOX_OPERATION(cmd);
	opsz = MBOX_OP_SIZE(cmd);

	/* For bit fields, the first argument is the efuse
	   row number, the second argument is the bit offset
	   within the row.
	   For other efuse fild sizes, the first argument
	   defines the row number */

	row = args[0];
	if (row > EFUSE_MAX_ROW) {
		args[0] = MB_STAT_BAD_ARGUMENT;
		return ERR_INVALID_ARGUMENT;
	}

	switch (opsz) {
	case MB_OPSZ_BIT:
		size = 1;
		offset = args[1];
		wr_args = args + 2;
		break;

	case MB_OPSZ_BYTE:
		size = 8;
		offset = args[1];
		wr_args = args + 2;
		break;

	case MB_OPSZ_WORD:
		size = 32;
		offset = args[1];
		wr_args = args + 2;
		break;

	case MB_OPSZ_DWORD:
		size = 64;
		offset = 0;
		wr_args = args + 1;
		break;

	case MB_OPSZ_256B:
		size = 256;
		offset = 0;
		wr_args = args + 1;
		break;

	default:
		args[0] = MB_STAT_BAD_ARGUMENT;
		return ERR_INVALID_ARGUMENT;
	}

	/* Execute command */
	if (op == MB_OP_WRITE) {
		/* WRITE */
		*nargs = 0;
		status = efuse_write(size, row, offset, wr_args);

	} else if (op == MB_OP_READ) {
		/* READ */
		if (opsz == MB_OPSZ_DWORD)
			*nargs = 2;
		else if (opsz == MB_OPSZ_256B)
			*nargs = 8;
		else
			*nargs = 1;
		status = efuse_read(size, row, offset, args);

	} else {
		/* ERROR */
		status = ERR_INVALID_COMMAND;
		args[0] = MB_STAT_BAD_COMMAND;

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
	u32			cmd, args[MAILBOX_MAX_ARGS];
	u32			status, nargs;
	enum mbox_status	mb_stat = MB_STAT_SUCCESS;

	if (exception != 0) {
		exception_handler(exception);
		return NO_ERROR;
	}

	/* Initialization stuff */
	status = clock_init();
	if (status)
		return status;

	/* Mailbox commands handling loop */
	while (1) {
		status = mbox_receive(&cmd, args);
		if (status == NO_ERROR)
			status = cmd_execute(cmd, args, &nargs);

		/* In case of error, the status saved in args[0] */
		if (status != NO_ERROR) {
			nargs = 0;
			mb_stat = args[0];
		}

		/* Send the results back */
		mbox_send(mb_stat, args, nargs);

	} /* read and execute mailbox commands */

	return NO_ERROR;
}
