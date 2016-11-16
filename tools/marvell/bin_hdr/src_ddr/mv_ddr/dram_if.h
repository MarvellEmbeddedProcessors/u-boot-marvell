/*
* ***************************************************************************
* Copyright (C) 2016 Marvell International Ltd.
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

#ifndef _DRAM_IF_H_
#define _DRAM_IF_H_

#define MAX_DRAM_IFACE		4
#define MAX_DRAM_BUS		5

typedef void (*udelay_func_t)(int usecs);
typedef int  (*printf_func_t)(const char * __restrict, ...);

struct dram_bus_cfg {
	uint64_t size_mb;
	uint32_t bus_width;
};

struct dram_iface_cfg {
	void *mac_base;
	void *phy_base;

	uint32_t ecc_enabled;
	uint32_t cs_count;
	uint32_t freq_mhz;

	struct dram_bus_cfg bus[MAX_DRAM_BUS];
};

struct dram_config {
	udelay_func_t	udelay;
	printf_func_t	printf;
	uint32_t	iface_mask;
	struct dram_iface_cfg iface[MAX_DRAM_IFACE];
};

int dram_init(struct dram_config *cfg);
struct dram_config *mv_ddr_dram_config_get(void);

#endif /* _DRAM_IF_H_ */
