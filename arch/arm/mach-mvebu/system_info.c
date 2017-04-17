/*
 * ***************************************************************************
 * Copyright (C) 2017 Marvell International Ltd.
 * ***************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of Marvell nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************
 */

#include <malloc.h>
#include <common.h>
#include <asm/io.h>
#include <mach/system_info.h>

struct sys_info *sys_info_in_ptr = (struct sys_info *)SYSTEM_INFO_ADDRESS;
#ifdef CONFIG_SPL_BUILD
/* skip index 0 because this index in dedicated to array system-info size*/
int sys_info_size = 1;

void set_info(enum sys_info_type field, unsigned int value)
{
	if (sys_info_size == 1)
		sys_info_in_ptr[ARRAY_SIZE].field_id = ARRAY_SIZE;
	sys_info_in_ptr[sys_info_size].field_id = field;
	sys_info_in_ptr[sys_info_size].value = value;
	sys_info_in_ptr[ARRAY_SIZE].value = ++sys_info_size;
}

#else /*for U-Boot */
DECLARE_GLOBAL_DATA_PTR;

unsigned int get_info(enum sys_info_type field)
{
	int i;
	for (i = 1; i < sys_info_in_ptr[ARRAY_SIZE].value; i++)
		if (gd->arch.local_sys_info[i].field_id == field)
			return gd->arch.local_sys_info[i].value;
	return -1;
}

void sys_info_init(void)
{
	int i;

	/*
	 * invalidate cache for memory in order
	 * to pass infomration by DDR from ATF to uboot
	 */
	invalidate_dcache_all();

	if (sys_info_in_ptr[ARRAY_SIZE].field_id != ARRAY_SIZE)
		sys_info_in_ptr[ARRAY_SIZE].value = 1;

	for (i = 1; i < sys_info_in_ptr[ARRAY_SIZE].value ; i++) {
		gd->arch.local_sys_info[i].field_id =
			sys_info_in_ptr[i].field_id;
		gd->arch.local_sys_info[i].value = sys_info_in_ptr[i].value;
	}
}
#endif
