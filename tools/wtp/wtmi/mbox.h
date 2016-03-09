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

u32 mbox_receive(u32 *cmd, u32 *args);
void mbox_send(u32 cmd, u32 *args, u32 nargs);

#endif /* __MBOX_H */
