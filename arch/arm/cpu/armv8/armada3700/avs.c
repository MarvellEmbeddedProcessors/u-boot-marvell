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
/*#define DEBUG*/
#include <common.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch/clock.h>
#include <asm/errno.h>
#include <asm/io.h>
#ifdef CONFIG_DEVEL_BOARD
#include <asm/arch/board-info.h>
#include <mvebu/board.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

/* AVS registers */
#define MVEBU_AVS_CTRL_0			(0x0)
#define  AVS_SOFT_RESET				(BIT31)
#define  AVS_ENABLE				(BIT30)
#define  AVS_SPEED_TARGET_MASK			(0x0000FFFF)
#define  AVS_HIGH_VDD_LIMIT_OFFS		(16)
#define  AVS_LOW_VDD_LIMIT_OFFS			(22)
#define  AVS_VDD_MASK				(0x3F)
#define MVEBU_AVS_CTRL_2			(0x8)
#define  AVS_LOW_VDD_EN				(BIT6)
#define MVEBU_AVS_VSET(x)			(0x1C + 4 * (x - 1))

/*
 * The lowest AVS voltage as default
 */
#define AVS_VDD_LOWEST		26 /* 1050mV */
#define AVS_VDD_1200M_LOWEST	31 /* 1108mV */

/* There is only one AVS node for Armada-3700 */
#define AVS_DT_NUM_MAX		1

struct mvebu_avs_config {
	void __iomem *reg_base;
	u32 vdd_min;
};
struct mvebu_avs_config avs_config;

/* Set the VDD values for the VSET loads except VDD0,
 * VDD0 is set in TIM header and will be overwritten by avs
 */
int set_avs_vdd_loads(void)
{
	void __iomem *reg_base;
	u32 reg_val;
	u32 vdd_min;
	int i;

	reg_base = avs_config.reg_base;
	vdd_min = avs_config.vdd_min;

	/* Enable low voltage mode */
	reg_val = readl(reg_base + MVEBU_AVS_CTRL_2);
	reg_val |= AVS_LOW_VDD_EN;
	writel(reg_val, reg_base + MVEBU_AVS_CTRL_2);

	/* Disable AVS before the configuration */
	reg_val = readl(reg_base + MVEBU_AVS_CTRL_0);
	reg_val &= ~(AVS_ENABLE);
	writel(reg_val, reg_base + MVEBU_AVS_CTRL_0);

	/* Set VDD for VSET 1,2 and 3 with lowest VDD */
	reg_val = readl(reg_base + MVEBU_AVS_CTRL_0);
	reg_val &= ~((AVS_VDD_MASK << AVS_HIGH_VDD_LIMIT_OFFS) |
			(AVS_VDD_MASK << AVS_LOW_VDD_LIMIT_OFFS));

	/*
	 * For preset CPU 1200 MHZ, 1108mV (vdd value is 31) is the lowest
	 * voltage that CPU could switch back from 600MHz/300MHz/200MHz.
	 * Now VSET1/2/3 (vdd min) value is read from fdt, yet in Armada3700
	 * rev2 DB's dts file, vdd min value is 26, which works well for preset
	 * CPU 600 MHz, 800 MHz and 1000 MHz, but not for CPU 1200MHz.
	 * Below is a temp fix, to set vdd min value to 31 for CPU 1200MHz.
	 */
	if (get_cpu_clk() == 1200)
		vdd_min = AVS_VDD_1200M_LOWEST;

	/* Set VDD for VSET 1, VSET 2 and VSET 3 */
	for (i = 1; i <= 3; i++) {
		reg_val = readl(reg_base + MVEBU_AVS_VSET(i));
		reg_val &= ~((AVS_VDD_MASK << AVS_HIGH_VDD_LIMIT_OFFS) |
				(AVS_VDD_MASK << AVS_LOW_VDD_LIMIT_OFFS));
		reg_val |= ((vdd_min << AVS_HIGH_VDD_LIMIT_OFFS) |
				(vdd_min << AVS_LOW_VDD_LIMIT_OFFS));
		writel(reg_val, reg_base + MVEBU_AVS_VSET(i));
	}

	/* Enable AVS after the configuration */
	reg_val = readl(reg_base + MVEBU_AVS_CTRL_0);
	reg_val |= AVS_ENABLE;
	writel(reg_val, reg_base + MVEBU_AVS_CTRL_0);

	return 0;
}

int init_avs(void)
{
	int ret = 0;
	int node_list[AVS_DT_NUM_MAX];
	int node;
	u32 count;

	debug_enter();

	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "avs",
			COMPAT_MVEBU_A3700_AVS, node_list, AVS_DT_NUM_MAX);
	if (count <= 0)
		return -ENODEV;

	/* There should be only one "AVS" DT node */
	node = node_list[0];
	if (node <= 0)
		return -ENODEV;

	/* Get register base from FDT */
	avs_config.reg_base = (u8 *)fdt_get_regs_offs(gd->fdt_blob, node, "reg");
	if (avs_config.reg_base == 0)
		error("Can not find reg in avs node\n");

	/* Get lowest voltage */
	avs_config.vdd_min = fdtdec_get_int(gd->fdt_blob, node, "vdd-min", AVS_VDD_LOWEST);

	debug_exit();
	return ret;
}
