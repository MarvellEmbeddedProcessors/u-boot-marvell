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

#ifndef __MBOX_H
#define __MBOX_H

#define MAILBOX_MAX_ARGS		16
#define MAILBOX_CMD_MASK		0x0000FFFF

#define CMD_REG_OCCUPIED_RESET_BIT	BIT1
#define CMD_REG_OCCUPIED_BIT		BIT0
#define HOST_INT_CMD_COMPLETE_BIT	BIT0

#define MBOX_COMMAND(sz, op)		((sz) | (op << 8))
#define MBOX_OP_SIZE(cmd)		(cmd & 0xF)
#define MBOX_OPERATION(cmd)		((cmd >> 8) & 0xF)

enum mbox_opsize {
	MB_OPSZ_BIT	= 1,	/* single bit */
	MB_OPSZ_BYTE	= 2,	/* single byte */
	MB_OPSZ_WORD	= 3,	/* 4 bytes - half row */
	MB_OPSZ_DWORD	= 4,	/* 8 bytes - one row */
	MB_OPSZ_256B	= 5,	/* 32 bytes - 4 rows */
	MB_OPSZ_MAX
};

enum mbox_op {
	MB_OP_READ	= 1,
	MB_OP_WRITE	= 2,
	MB_OP_MAX
};

enum mbox_status {
	MB_STAT_SUCCESS			= 0,
	MB_STAT_HW_ERROR		= 1,
	MB_STAT_TIMEOUT			= 2,
	MB_STAT_BAD_ARGUMENT		= 3,
	MB_STAT_BAD_COMMAND		= 4,

	MB_STAT_MAX
};

u32 mbox_receive(u32 *cmd, u32 *args);
void mbox_send(u32 cmd, u32 *args, u32 nargs);

#endif /* __MBOX_H */
