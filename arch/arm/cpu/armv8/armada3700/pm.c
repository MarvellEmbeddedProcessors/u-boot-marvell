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
/*#define DEBUG*/
#include <common.h>
#include <fdtdec.h>
#include <asm/io.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch/clock.h>
#include <asm/arch/avs.h>
#include <asm/io.h>
#include <asm/errno.h>

DECLARE_GLOBAL_DATA_PTR;

/* North bridge PM configuration registers */
#define PM_POWER_DOWN_REG	(0x8)
#define  PM_AVS_VDD2_MODE	(BIT13)
#define  PM_AVS_DISABLE_MODE	(BIT14)
#define PM_NB_L0_L1_CONFIG_REG	(0x18)
#define PM_NB_L2_L3_CONFIG_REG	(0x1C)
#define  PM_NB_TBG_DIV_LX_OFF	(13)
#define  PM_NB_TBG_DIV_LX_MASK	(0x7)
#define  PM_NB_CLK_SEL_LX_OFF	(11)
#define  PM_NB_CLK_SEL_LX_MASK	(0x1)
#define  PM_NB_TBG_SEL_LX_OFF	(9)
#define  PM_NB_TBG_SEL_LX_MASK	(0x3)
#define  PM_NB_VDD_SEL_LX_OFF	(6)
#define  PM_NB_VDD_SEL_LX_MASK	(0x3)
#define  PM_NB_LX_CONFIG_SHIFT	(16)

/* VDD level selection */
enum vdd_select {
	VDD_SEL_HIGH = 0,
	VDD_SEL_MEDIUM,
	VDD_SEL_LOW,
	VDD_SEL_LOWEST,
};

/* DVFS parameters per load */
struct dvfs_load {
	u32 clk_sel;
	u32 tbg_sel;
	u32 tbg_div;
	u32 vdd_sel;
};

struct pm_config {
	u8 *reg_base;
	struct dvfs_load dvfs_load;
};

/* DVFS LOAD index */
enum dvfs_load_index {
	DVFS_LOAD_0 = 0,
	DVFS_LOAD_1,
	DVFS_LOAD_2,
	DVFS_LOAD_3,
	DVFS_LOAD_MAX_NUM
};

/* There is only one PM node in Armada-3700 */
#define PM_MAX_DT_NUM	1
struct pm_config __attribute__((section(".data"))) g_pm_config;

/*
 * CPU clock TBG divider array
 * TBG divider 0 and 7 mean active-high for clock output,
 * thus they are not listed below.
 * For other divider values from 1 to 6, proper divider values should be set
 * for each level from small to big.
 * Recommended divider values for real deployment:
 * original divider   1st divider    2nd divider   3rd divider   4th divider
 *   1                  1              2             4             6
 *   2                  2              4             5             6
 * CPU frequency mapping:
 * original CPU freq    1st CPU freq     2nd CPU freq    3rd CPU freq    4th CPU freq
 *   1200M                1200M             600M             300M            200M
 *   1000M                1000M             500M             250M            166M
 *   800M                 800M              400M             200M            133M
 *   600M                 600M              300M             240M            200M
 *   400M                 400M              200M             160M            133M
 */
enum a3700_tbg_divider tbg_div_arr[TBG_DIVIDER_NUM][DVFS_LOAD_MAX_NUM] = {
	{TBG_DIVIDER_1, TBG_DIVIDER_2, TBG_DIVIDER_4, TBG_DIVIDER_6},
	{TBG_DIVIDER_2, TBG_DIVIDER_4, TBG_DIVIDER_5, TBG_DIVIDER_6},
	{TBG_DIVIDER_3, TBG_DIVIDER_4, TBG_DIVIDER_5, TBG_DIVIDER_6},
	{TBG_DIVIDER_4, TBG_DIVIDER_4, TBG_DIVIDER_5, TBG_DIVIDER_6},
	{TBG_DIVIDER_5, TBG_DIVIDER_5, TBG_DIVIDER_6, TBG_DIVIDER_6},
	{TBG_DIVIDER_6, TBG_DIVIDER_6, TBG_DIVIDER_6, TBG_DIVIDER_6},
};

/* CPU VDD selection array */
enum vdd_select vdd_sel_arr[DVFS_LOAD_MAX_NUM] = {
	VDD_SEL_HIGH, VDD_SEL_MEDIUM, VDD_SEL_LOW, VDD_SEL_LOWEST
};

/******************************************************************************
* Name: pm_en_avs
*
* Description: Enable AVS in DVFS
*
* Input:	None
* Output:	None
* Return:	Non-zero if the requested settings are not supported
******************************************************************************/
static int pm_en_avs(void)
{
	u32 reg_val;

	reg_val = readl(g_pm_config.reg_base + PM_POWER_DOWN_REG);
	/* Clear the AVS disable mode bit */
	reg_val &= ~(PM_AVS_DISABLE_MODE);
	/* Set AVS VDD2 mode bit */
	reg_val |= PM_AVS_VDD2_MODE;
	writel(reg_val, g_pm_config.reg_base + PM_POWER_DOWN_REG);

	return 0;
}

/******************************************************************************
* Name: set_dvfs_param
*
* Description: Set DVFS parameters for dedicated LOAD level
*
* Input:	load_level: CPU LOAD level from 0 to 3
*		pm_load: CPU LOAD parameters
* Output:	None
* Return:	Non-zero if the requested settings are not supported
******************************************************************************/
int set_dvfs_param(u32 load_level, struct dvfs_load *dvfs_load)
{
	u8 *reg_addr;
	u32 reg_val;
	u32 shift;

	/* Acquire register address */
	reg_addr = g_pm_config.reg_base;
	if (load_level <= DVFS_LOAD_1)
		reg_addr += PM_NB_L0_L1_CONFIG_REG;
	else
		reg_addr += PM_NB_L2_L3_CONFIG_REG;

	/* Acquire shift within register */
	if (load_level == DVFS_LOAD_0 || load_level == DVFS_LOAD_2)
		shift = PM_NB_LX_CONFIG_SHIFT;
	else
		shift = 0;

	reg_val = readl(reg_addr);

	/* Set clock source */
	reg_val &= ~(PM_NB_CLK_SEL_LX_MASK << (shift + PM_NB_CLK_SEL_LX_OFF));
	reg_val |= (dvfs_load->clk_sel & PM_NB_CLK_SEL_LX_MASK) << (shift + PM_NB_CLK_SEL_LX_OFF);

	/* Set TBG source */
	reg_val &= ~(PM_NB_TBG_SEL_LX_MASK << (shift + PM_NB_TBG_SEL_LX_OFF));
	reg_val |= (dvfs_load->tbg_sel & PM_NB_TBG_SEL_LX_MASK) << (shift + PM_NB_TBG_SEL_LX_OFF);

	/* Set clock divider */
	reg_val &= ~(PM_NB_TBG_DIV_LX_MASK << (shift + PM_NB_TBG_DIV_LX_OFF));
	reg_val |= (dvfs_load->tbg_div & PM_NB_TBG_DIV_LX_MASK) << (shift + PM_NB_TBG_DIV_LX_OFF);

	/* Set VDD divider */
	reg_val &= ~(PM_NB_VDD_SEL_LX_MASK << (shift + PM_NB_VDD_SEL_LX_OFF));
	reg_val |= (dvfs_load->vdd_sel & PM_NB_VDD_SEL_LX_MASK) << (shift + PM_NB_VDD_SEL_LX_OFF);

	writel(reg_val, reg_addr);

	return 0;
}

/******************************************************************************
* Name: set_dvfs
*
* Description: Configure entire clock tree according to CPU and DDR frequency
*
* Input:	cpu_clk_sel: TBG source selection for CPU clock
*		cpu_clk_prscl: prescaling for CPU clock
* Output:	None
* Return:	Non-zero if the requested settings are not supported
******************************************************************************/
int set_dvfs(u32 cpu_clk_sel, u32 cpu_clk_prscl)
{
	int ret = 0;
	int i;
	struct dvfs_load *p_dvfs_load;

	debug_enter();

	/* Check input parameters */
	if (cpu_clk_sel > TBG_B_S) {
		error("CPU TBG CLK selection %d is out of range\n", cpu_clk_sel);
		return -EINVAL;
	}

	if (cpu_clk_prscl < TBG_DIVIDER_1 || cpu_clk_prscl > TBG_DIVIDER_6) {
		error("CPU CLK divider %d is out of range\n", cpu_clk_prscl);
		return -EINVAL;
	}

	/* set CPU clk divider */
	p_dvfs_load = &g_pm_config.dvfs_load;
	for (i = DVFS_LOAD_0; i <= DVFS_LOAD_3; i++) {
		/* Always select TBG */
		p_dvfs_load->clk_sel = CLK_SEL_TBG;

		/* set TBG selection */
		p_dvfs_load->tbg_sel = cpu_clk_sel;

		/* set CPU clock TBG divider */
		p_dvfs_load->tbg_div = tbg_div_arr[cpu_clk_prscl - TBG_DIVIDER_1][i];

		/* set VDD selection */
		p_dvfs_load->vdd_sel = vdd_sel_arr[i];

		ret = set_dvfs_param(i, p_dvfs_load);
		if (ret) {
			error("Failed to set DVFS parameter for LOAD%d\n", i);
			break;
		}
	}

	debug_exit();

	return ret;
}

/******************************************************************************
* Name: init_pm
*
* Description: Initialize power management according to DT node
*
* Input:	None
* Output:	None
* Return:	Non-zero if the initialization failed
******************************************************************************/
int init_pm(void)
{
	int ret = 0;
	int node_list[PM_MAX_DT_NUM];
	int node;
	u32 count;
	u32 clk_sel;
	u32 clk_prscl;
	const char *vdd_control;

	debug_enter();

	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "pm",
			COMPAT_MVEBU_A3700_PM, node_list, PM_MAX_DT_NUM);
	if (count <= 0)
		return -ENODEV;

	/* There should be only one "pm" DT node */
	node = node_list[0];

	if (node <= 0)
		return -ENODEV;

	/* Get reg_base from FDT */
	g_pm_config.reg_base = (u8 *)fdt_get_regs_offs(gd->fdt_blob, node, "reg");

	ret = get_cpu_clk_src_div(&clk_sel, &clk_prscl);
	if (ret) {
		error("Failed to get CPU clock source and prescaling divider\n");
		return -EINVAL;
	}
	if (clk_prscl == 0 || clk_prscl == 7) {
		error("CPU clk output is always high!!\n");
		return -EINVAL;
	}

	ret = set_dvfs(clk_sel, clk_prscl);
	if (ret) {
		error("Failed to set DVFS parameters\n");
		return -EINVAL;
	}

	/* Set VDD control method, currently only support AVS */
	fdt_get_string(gd->fdt_blob, node, "vdd_control", &vdd_control);
	if (!strcmp(vdd_control, "avs")) {
		/* Enale AVS in PM */
		pm_en_avs();
		if (ret) {
			error("Failed to enable AVS in PM\n");
			return -EINVAL;
		}

		/* Set AVS VDD load values */
		ret = set_avs_vdd_loads();
		if (ret) {
			error("Failed to set AVS VDD load values\n");
			return -EINVAL;
		}
	}

	debug_exit();
	return ret;
}
