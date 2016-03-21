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

#ifndef _A3700_MBOX_H_
#define _A3700_MBOX_H_

#define MBOX_MAX_ARGS			(16)
#define MBOX_CMD_TIMEOUT		(100000)

/* Mailbox command, arguments and status */
#define MBOX_SEND_ARG_OFFS(n)		(MVEBU_MBOX_REGS_BASE + (n) * 4)
#define MBOX_SEND_CMD_OFFS		(MVEBU_MBOX_REGS_BASE + 0x40)
#define MBOX_RECEIVE_STAT_OFFS		(MVEBU_MBOX_REGS_BASE + 0x80)
#define MBOX_RECEIVE_ARG_OFFS(n)	(MVEBU_MBOX_REGS_BASE + 0x84 + (n) * 4)

/* Host intterrupt reset - to complete received command/status processing */
#define MBOX_HOST_INT_RESET		(MVEBU_MBOX_REGS_BASE + 0xC8)
#define MBOX_SEC_CPU_CMD_COMPLETE	(BIT0)

/* Host interrupt status - to poll for new command/status received from secure CPU */
#define MBOX_SEC_CPU_INT_STAT_REG	(MVEBU_REGS_BASE + 0x17814)
#define MBOX_SEC_CPU_CMD_SET		(BIT31)

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

	MB_STAT_MAX
};

int mbox_send(enum mbox_opsize opsz, enum mbox_op op, uint32_t row, uint32_t offs, uint32_t *args);
int mbox_receive(enum mbox_status *stat, uint32_t *args, uint32_t timeout_us);

#endif /* _A3700_MBOX_H_ */
