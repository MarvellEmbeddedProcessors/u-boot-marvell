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

#ifndef _SYSTEM_INFO_H_
#define _SYSTEM_INFO_H_

#define SYSTEM_INFO_ADDRESS	0x4000000

enum sys_info_type {
	ARRAY_SIZE,
	DRAM_CS0_SIZE,
	DRAM_CS1_SIZE,
	DRAM_CS2_SIZE,
	DRAM_CS3_SIZE,
	DRAM_BUS_WIDTH,
	DRAM_ECC,
	DRAM_CS0,
	DRAM_CS1,
	DRAM_CS2,
	DRAM_CS3,
	RECOVERY_MODE,
	BOOT_MODE,
	CPU_DEC_WIN0_BASE,
	CPU_DEC_WIN1_BASE,
	CPU_DEC_WIN2_BASE,
	CPU_DEC_WIN3_BASE,
	CPU_DEC_WIN4_BASE,
	CPU_DEC_WIN0_SIZE,
	CPU_DEC_WIN1_SIZE,
	CPU_DEC_WIN2_SIZE,
	CPU_DEC_WIN3_SIZE,
	CPU_DEC_WIN4_SIZE,
	MAX_OPTION,
};

struct sys_info {
	enum sys_info_type field_id;
	unsigned int value;
};

unsigned int get_info(enum sys_info_type field);
void set_info(enum sys_info_type field, unsigned int value);
void sys_info_init(void);

#endif /* _SYSTEM_INFO_H_ */
