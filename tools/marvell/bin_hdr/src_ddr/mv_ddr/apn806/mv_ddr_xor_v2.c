/*******************************************************************************
Copyright (C) 2016 Marvell International Ltd.

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the three
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 2 of the License, or any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************************
Marvell GNU General Public License FreeRTOS Exception

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the Lesser
General Public License Version 2.1 plus the following FreeRTOS exception.
An independent module is a module which is not derived from or based on
FreeRTOS.
Clause 1:
Linking FreeRTOS statically or dynamically with other modules is making a
combined work based on FreeRTOS. Thus, the terms and conditions of the GNU
General Public License cover the whole combination.
As a special exception, the copyright holder of FreeRTOS gives you permission
to link FreeRTOS with independent modules that communicate with FreeRTOS solely
through the FreeRTOS API interface, regardless of the license terms of these
independent modules, and to copy and distribute the resulting combined work
under terms of your choice, provided that:
1. Every copy of the combined work is accompanied by a written statement that
details to the recipient the version of FreeRTOS used and an offer by yourself
to provide the FreeRTOS source code (including any modifications you may have
made) should the recipient request it.
2. The combined work is not itself an RTOS, scheduler, kernel or related
product.
3. The independent modules add significant and primary functionality to
FreeRTOS and do not merely extend the existing functionality already present in
FreeRTOS.
Clause 2:
FreeRTOS may not be used for any competitive or comparative purpose, including
the publication of any form of run time or compile time metric, without the
express permission of Real Time Engineers Ltd. (this is the norm within the
industry and is intended to ensure information accuracy).

********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

	* Neither the name of Marvell nor the names of its contributors may be
	  used to endorse or promote products derived from this software without
	  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "ddr3_init.h"
#include "mv_ddr_xor_v2.h"

/* dma engine registers */
#define DMA_DESQ_BALR_OFF		0x000
#define DMA_DESQ_BAHR_OFF		0x004
#define DMA_DESQ_SIZE_OFF		0x008
#define DMA_DESQ_DONE_OFF		0x00c
#define DMA_DESQ_DONE_PENDING_MASK	0x7fff
#define DMA_DESQ_DONE_PENDING_SHIFT	0
#define DMA_DESQ_DONE_READ_PTR_MASK	0x1fff
#define DMA_DESQ_DONE_READ_PTR_SHIFT	16
#define DMA_DESQ_ARATTR_OFF		0x010
#define DMA_DESQ_ATTR_CACHE_MASK	0x3f3f
#define DMA_DESQ_ATTR_OUTER_SHAREABLE	0x202
#define DMA_DESQ_ATTR_CACHEABLE		0x3c3c
#define DMA_IMSG_CDAT_OFF		0x014
#define DMA_IMSG_THRD_OFF		0x018
#define DMA_IMSG_THRD_MASK		0x7fff
#define DMA_IMSG_THRD_SHIFT		0x0
#define DMA_DESQ_AWATTR_OFF		0x01c
/* same flags as DMA_DESQ_ARATTR_OFF */
#define DMA_DESQ_ALLOC_OFF		0x04c
#define DMA_DESQ_ALLOC_WRPTR_MASK	0xffff
#define DMA_DESQ_ALLOC_WRPTR_SHIFT	16
#define DMA_IMSG_BALR_OFF		0x050
#define DMA_IMSG_BAHR_OFF		0x054
#define DMA_DESQ_CTRL_OFF		0x100
#define DMA_DESQ_CTRL_32B		1
#define DMA_DESQ_CTRL_128B		7
#define DMA_DESQ_STOP_OFF		0x800
#define DMA_DESQ_STOP_QUEUE_RESET_OFFS	1
#define DMA_DESQ_STOP_QUEUE_RESET_ENA	1
#define DMA_DESQ_STOP_QUEUE_DIS_OFFS	0
#define DMA_DESQ_STOP_QUEUE_DIS_ENA	0
#define DMA_DESQ_DEALLOC_OFF		0x804
#define DMA_DESQ_ADD_OFF		0x808

/* xor global registers */
#define GLOB_BW_CTRL				0x10004
#define GLOB_BW_CTRL_NUM_OSTD_RD_SHIFT		0
#define GLOB_BW_CTRL_NUM_OSTD_RD_VAL		64
#define GLOB_BW_CTRL_NUM_OSTD_WR_SHIFT		8
#define GLOB_BW_CTRL_NUM_OSTD_WR_VAL		8
#define GLOB_BW_CTRL_RD_BURST_LEN_SHIFT		12
#define GLOB_BW_CTRL_RD_BURST_LEN_VAL		4
#define GLOB_BW_CTRL_RD_BURST_LEN_VAL_Z1	2
#define GLOB_BW_CTRL_WR_BURST_LEN_SHIFT		16
#define GLOB_BW_CTRL_WR_BURST_LEN_VAL		4
#define GLOB_BW_CTRL_WR_BURST_LEN_VAL_Z1	2
#define GLOB_PAUSE				0x10014
#define GLOB_PAUSE_AXI_TIME_DIS_VAL		0x8
#define GLOB_SYS_INT_CAUSE			0x10200
#define GLOB_SYS_INT_MASK			0x10204
#define GLOB_MEM_INT_CAUSE			0x10220
#define GLOB_MEM_INT_MASK			0x10224

#define MV_XOR_V2_MIN_DESC_SIZE	32
#define MV_XOR_V2_EXT_DESC_SIZE	128

#define MV_XOR_V2_DESC_RESERVED_SIZE		12
#define MV_XOR_V2_DESC_BUFF_D_ADDR_SIZE		12

#define MV_XOR_V2_CMD_LINE_NUM_MAX_D_BUF	8

/* descriptors queue size; set to 32, but can be increased, if required */
#define MV_XOR_V2_MAX_DESC_NUM			32

/* xor base address */
#define MV_XOR_BASE	0x00400000

/* descriptors queue memory size */
#define MV_XOR_QMEM_SIZE		((MV_XOR_V2_MAX_DESC_NUM) * (MV_XOR_V2_EXT_DESC_SIZE))
#define MV_XOR_QMEM_START_ADDR		0x0
#define MV_XOR_QMEM_END_ADDR		((MV_XOR_QMEM_START_ADDR) + (MV_XOR_QMEM_SIZE))

#define MV_XOR_MAX_BURST_512B		5
#define MV_XOR_MAX_BURST_MASK		0x1ff
#define MV_XOR_MAX_TRANSFER_SIZE	((UINT32_MAX) & ~MV_XOR_MAX_BURST_MASK)

/*
 * struct mv_xor_v2_descriptor - dma hardware descriptor
 * @desc_id: used by software; not affected by hardware
 * @flags: error and status flags
 * @crc32_result: crc32 calculation result
 * @desc_ctrl: operation mode and control flags
 * @buff_size: amount of bytes to be processed
 * @fill_pattern_src_addr: fill-pattern or source-address and
 * aw-attributes
 * @data_buff_addr: source (and might be raid 6 destination)
 * addresses of data buffers in raid 5 and raid 6
 * @reserved: reserved
 */
struct mv_xor_v2_descriptor {
	u16 desc_id;
	u16 flags;
	u32 crc32_result;
	u32 desc_ctrl;

/* definitions for desc_ctrl */
#define DESC_NUM_ACTIVE_D_BUF_SHIFT	22
#define DESC_OP_MODE_SHIFT		28
#define DESC_OP_MODE_NOP		0	/* idle operation */
#define DESC_OP_MODE_MEMCPY		1	/* pure-dma operation */
#define DESC_OP_MODE_MEMSET		2	/* mem-fill operation */
#define DESC_OP_MODE_MEMINIT		3	/* mem-init operation */
#define DESC_OP_MODE_MEM_COMPARE	4	/* mem-compare operation */
#define DESC_OP_MODE_CRC32		5	/* crc32 calculation */
#define DESC_OP_MODE_XOR		6	/* raid 5 (xor) operation */
#define DESC_OP_MODE_RAID6		7	/* raid 6 p&q-generation */
#define DESC_OP_MODE_RAID6_REC		8	/* raid 6 recovery */
#define DESC_Q_BUFFER_ENABLE		BIT(16)
#define DESC_P_BUFFER_ENABLE		BIT(17)
#define DESC_IOD			BIT(27)

	u32 buff_size;
	u32 fill_pattern_src_addr[4];
	u32 data_buff_addr[MV_XOR_V2_DESC_BUFF_D_ADDR_SIZE];
	u32 reserved[MV_XOR_V2_DESC_RESERVED_SIZE];
};

/* descriptors queue memory is located in dram; dram is not coherent */
static void mv_xor_v2_init(u32 base, u32 qmem)
{
	u32 reg_val;

	/* set descriptor size to dma engine */
	reg_write(base + DMA_DESQ_CTRL_OFF, DMA_DESQ_CTRL_128B);

	/* set descriptors queue size to dma engine */
	reg_write(base + DMA_DESQ_SIZE_OFF, MV_XOR_V2_MAX_DESC_NUM);

	/* set descriptors queue address to dma engine */
	reg_write(base + DMA_DESQ_BALR_OFF, qmem);
	reg_write(base + DMA_DESQ_BAHR_OFF, 0);

	/*
	 * set attributes for reading and writing data buffers and descriptors to:
	 * - outer-shareable - snoops to be performed on cpu caches;
	 * - enable cacheable - bufferable, modifiable, other allocate, and allocate.
	 */
	reg_val = reg_read(base + DMA_DESQ_ARATTR_OFF);
	reg_val &= ~DMA_DESQ_ATTR_CACHE_MASK;
	reg_val |= DMA_DESQ_ATTR_OUTER_SHAREABLE | DMA_DESQ_ATTR_CACHEABLE;
	reg_write(base + DMA_DESQ_ARATTR_OFF, reg_val);

	reg_val = reg_read(base + DMA_DESQ_AWATTR_OFF);
	reg_val &= ~DMA_DESQ_ATTR_CACHE_MASK;
	reg_val |= DMA_DESQ_ATTR_OUTER_SHAREABLE | DMA_DESQ_ATTR_CACHEABLE;
	reg_write(base + DMA_DESQ_AWATTR_OFF, reg_val);

	/*
	 * bandwidth control to optimize xor performance:
	 * - set write/read burst lengths to maximum 512B write/read transactions;
	 * - set outstanding write/read data requests to maximum value.
	 */
	reg_val = (GLOB_BW_CTRL_NUM_OSTD_RD_VAL << GLOB_BW_CTRL_NUM_OSTD_RD_SHIFT) |
		(GLOB_BW_CTRL_NUM_OSTD_WR_VAL << GLOB_BW_CTRL_NUM_OSTD_WR_SHIFT) |
		(MV_XOR_MAX_BURST_512B << GLOB_BW_CTRL_RD_BURST_LEN_SHIFT) |
		(MV_XOR_MAX_BURST_512B << GLOB_BW_CTRL_WR_BURST_LEN_SHIFT);
	reg_write(base + GLOB_BW_CTRL, reg_val);

	/* disable axi timer feature */
	reg_val = reg_read(base + GLOB_PAUSE);
	reg_val |= GLOB_PAUSE_AXI_TIME_DIS_VAL;
	reg_write(base + GLOB_PAUSE, reg_val);

	/* enable dma engine */
	reg_write(base + DMA_DESQ_STOP_OFF, DMA_DESQ_STOP_QUEUE_DIS_ENA << DMA_DESQ_STOP_QUEUE_DIS_OFFS);
}

static void mv_xor_v2_desc_create(uint64_t qmem, int desc_id, uint64_t start, uint64_t size, uint64_t data)
{
	struct mv_xor_v2_descriptor *desc = (struct mv_xor_v2_descriptor *)qmem;

	desc = &desc[desc_id];
	memset(desc, 0, sizeof(*desc));
	desc->desc_id = desc_id;
	desc->desc_ctrl = DESC_OP_MODE_MEMSET << DESC_OP_MODE_SHIFT;
	desc->buff_size = size;
	desc->fill_pattern_src_addr[0] = (u32)data;
	desc->fill_pattern_src_addr[1] = (u32)(data >> 32);
	desc->fill_pattern_src_addr[2] = (u32)start;
	desc->fill_pattern_src_addr[3] = (u32)(start >> 32);
	desc->data_buff_addr[0] = (u32)start;
	desc->data_buff_addr[2] = (u32)(start >> 32) & 0xffff;
}

static void mv_xor_v2_enqueue(u32 base, u32 n)
{
	/* write a number of new descriptors in the descriptors queue */
	reg_write(base + DMA_DESQ_ADD_OFF, n);
}

static u32 mv_xor_v2_done(u32 base)
{
	/* return a number of completed descriptors */
	return reg_read(base + DMA_DESQ_DONE_OFF) & DMA_DESQ_DONE_PENDING_MASK;
}

static void mv_xor_v2_finish(u32 base)
{
	/* reset dma engine */
	reg_write(base + DMA_DESQ_STOP_OFF, DMA_DESQ_STOP_QUEUE_RESET_ENA << DMA_DESQ_STOP_QUEUE_RESET_OFFS);
}

/* mv_ddr xor api */
void mv_ddr_xor_mem_scrubbing(uint64_t start_addr, uint64_t size, uint64_t data)
{
	uint64_t start = start_addr;
	uint64_t end = start_addr + size;
	uint64_t buffer_size;
	int desc_id = 0;

	/* initialize xor queue memory region */
	memset(MV_XOR_QMEM_START_ADDR, 0, MV_XOR_QMEM_SIZE);

	/* initialize xor engine */
	mv_xor_v2_init(MV_XOR_BASE, MV_XOR_QMEM_START_ADDR);

	if (start < MV_XOR_QMEM_END_ADDR)
		start = MV_XOR_QMEM_END_ADDR;

	while (start < end) {
		if (desc_id >= MV_XOR_V2_MAX_DESC_NUM) {
			/* increase xor max desc number if required */
			printf("mv_ddr: xor engine out of memory\n");
			return;
		}
		buffer_size = end - start;
		if (buffer_size > MV_XOR_MAX_TRANSFER_SIZE)
			buffer_size = MV_XOR_MAX_TRANSFER_SIZE;
		mv_xor_v2_desc_create(MV_XOR_QMEM_START_ADDR, desc_id, start, buffer_size, data);
		mv_xor_v2_enqueue(MV_XOR_BASE, 1);
		desc_id++;
		start += buffer_size;
	}

	/* wait for transfer completion */
	while (mv_xor_v2_done(MV_XOR_BASE) != desc_id)
		;

	/* disable xor engine */
	mv_xor_v2_finish(MV_XOR_BASE);
}
