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
#include <asm/arch-mvebu/efuse.h>

int efuse_id_valid(enum efuse_id fid)
{
	return 0;
}

int efuse_write(enum efuse_id fid, const char *value)
{
	return 0;
}

int efuse_read(enum efuse_id fid, char *value)
{
	return 0;
}

void efuse_raw_dump(void)
{
	return;
}
