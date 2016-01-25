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
#include <asm/arch-armadalp/clock.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;
static struct a3700_clock_cfg a3700_clock_configs[] = MVEBU_A3700_CLOCK_CFGS;


/***************************************************************************************************
  * get_ref_clk
  *
  * return: reference clock in MHz (25 or 40)
 ***************************************************************************************************/
u32 get_ref_clk(void)
{
	u32 regval;

	regval = (readl(MVEBU_TEST_PIN_LATCH_N) & MVEBU_XTAL_MODE_MASK) >> MVEBU_XTAL_MODE_OFFS;

	if (regval == MVEBU_XTAL_CLOCK_25MHZ)
		return 25;
	else
		return 40;
}

/******************************************************************************
* Name: get_tbg_vco_sel
*
* Description:	calculate the vco parameters for TBG based on the target clock.
*
* Input:	kvco_mhz - KVCO clock freq in MHz
*
* Output:	vco_intpi - Phase Interpolator Bias Current
*		vco_range - VCO frequency range (0x8 - 0xF)
*
* Return: 0 - OK
******************************************************************************/
static int get_tbg_vco_sel(u32 kvco_mhz,
			   u32 *vco_intpi,
			   u32 *vco_range)
{
	u8  index;
	u16 vco_top_ranges_mhz[8] = {1350, 1500, 1750, 2000, 2200, 2400, 2600, 3000};

	debug_enter();

	if ((kvco_mhz <= MVEBU_KVCO_MIN) && (kvco_mhz >= MVEBU_KVCO_MAX)) {
		error("Invalid KVCO CLK requested - %d MHz\n", kvco_mhz);
		return 1;
	}

	/* Select the range the KVCO fits to.
	   Valid values are 0x8 through 0xF.
	   See TBG_A_KVCO and TBG_B_KVCO fields for details
	 */
	for (index = 0; index < 8; index++) {
		if (kvco_mhz <= vco_top_ranges_mhz[index]) {
			*vco_range = index + 8;
			break;
		}
	}

	/* INTPI - Phase Interpolator Bias Current
	  8 <= KVCO is 2.5 ~ 3.0 GHz,
	  6 <= KVCO is 2.0 ~ 2.5 GHz,
	  5 <= KVCO is 1.5 ~ 2.0 GHz
	 */
	if (kvco_mhz > 2500)
		*vco_intpi = 8;
	else if (kvco_mhz < 2000)
		*vco_intpi = 5;
	else
		*vco_intpi = 6;

	debug_exit();
	return 0;
}

/******************************************************************************
* Name: set_tbg_clock
*
* Description:	Change the TBG(A/B) clock
*		All affected North/South bridge clock sources should be
*		switched to XTAL mode prior to calling this function!
*
* Input:	kvco_mhz:     AKVCO clock for TBG-X in MHz
*		se_vco_div:   single-ended clock VCO divider (TBG_X_S)
*		diff_vco_div: differential clock VCO post divider (TBG_X_P)
*		tbg_typ:      TBG type (A/B)
* Output:	None
* Return:	0 if OK
******************************************************************************/
static u32 set_tbg_clock(u32 kvco_mhz,
			       u32 se_vco_div,
			       u32 diff_vco_div,
			       enum a3700_clock_src tbg_typ)
{
	u32 vco_range = 0, vco_intpi = 0;
	u32 tbg_M, tbg_N;
	u32 icp;
	u32 regval;
	int ret;

	debug_enter();

	/* Try to keep Fref/M as close as possible to 10 MHz */
	if (get_ref_clk() == 40) {
		/* 40MHz */
		debug("REF clock is 40MHz\n");
		tbg_M = 4;	/* 10MHz */
		icp = 5;	/* 8 */
	} else	{
		/* 25MHz */
		debug("REF clock is 25MHz\n");
		tbg_M = 3;	/* 8.33MHz */
		icp = 6;	/* 9 */
	}

	ret = get_tbg_vco_sel(kvco_mhz, &vco_intpi, &vco_range);
	if (ret != 0) {
		error("Failed to obtain VCO divider selection\n");
		return ret;
	}

	/* TBG frequency is calculated according to formula:
		Ftbg = Fkvco / se_vco_div
	   where
		Fkvco = N * Fref * 4 / M

	   N - TBG_A_FBDIV or TBG_B_FBDIV
	   M - TBG_A_REFDIV or TBG_B_REFDIV
	   se_vco_div - TBG_A_VCODIV_SEL_SE or TBG_B_VCODIV_SEL_SE
	   Fref - reference clock 25MHz or 40 MHz
	  */
	tbg_N = (kvco_mhz * tbg_M / get_ref_clk()) >> 2;
	debug("TBG-%s: SE vco_div %#x, DIFF vco_div %#x,vco_range %#x tbg_N %#x KVCO = %d MHz\n",
	      tbg_typ == TBG_A ? "A" : "B", se_vco_div, diff_vco_div, vco_range, tbg_N, kvco_mhz);

	/* 1. Switch all North/South Bridge clock sources to XTAL
	      Should be already done by the function caller */

	/* 2. Set TBG-A (bit[0]) or TBG-B (bit[16]) to reset state  */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL0);
	regval |= (tbg_typ == TBG_A) ? BIT0 : BIT16;
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL0);

	/* The A3700 FS requires to power down TGB before
	   changing PLL parameters. However it turns out that
	   this step is not essential. Putting TBG into reset
	   state is enough.
	   3. Power down TBG-A (bit[1]) or TBG-B (bit[17])
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL0);
	regval |= (tbg_typ == TBG_A) ? BIT1 : BIT17;
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL0);
	*/

	/* 4. Set TBG-A (bit[10:2]) or TBG-B (bit[26:18]) FBDIV (N) value */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL0);
	regval &= (tbg_typ == TBG_A) ? ~(0xFFUL << 2) : ~(0xFFUL << 18);
	regval |= (tbg_typ == TBG_A) ? (tbg_N << 2) : (tbg_N << 18);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL0);

	/* 5. Set TBG-A (bit[8:0]) or TBG-B (bit[24:16]) REFDIV (M) value */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL7);
	regval &= (tbg_typ == TBG_A) ? ~(0x1FFUL << 0) : ~(0x1FFUL << 16);
	regval |= (tbg_typ == TBG_A) ? (tbg_M << 0) : (tbg_M << 16);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL7);

	/* 6a. Set TBG-A (bit[8:0]) or TBG-B (bit[24:16]) SE VCODIV value  */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL1);
	regval &= (tbg_typ == TBG_A) ? ~(0x1FFUL << 0) : ~(0x1FFUL << 16);
	regval |= (tbg_typ == TBG_A) ? (se_vco_div << 0) : (se_vco_div << 16);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL1);

	/* 6b. Set TBG-A (bit[9:1]) or TBG-B (bit[25:17]) DIFF VCODIV value  */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL8);
	regval &= (tbg_typ == TBG_A) ? ~(0x1FFUL << 1) : ~(0x1FFUL << 17);
	regval |= (tbg_typ == TBG_A) ? (diff_vco_div << 1) : (diff_vco_div << 17);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL8);

	/* 7. Set TBG-A (bit[11]) or TBG-B (bit[27]) PLL Bandwidth to normal (0x0) */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL1);
	regval &= (tbg_typ == TBG_A) ? ~(0x1UL << 11) : ~(0x1UL << 27);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL1);

	/* 8. Set TBG-A (bit[11:8]) or TBG-B (bit[27:24]) ICP Charge Pump Current value */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL2);
	regval &= (tbg_typ == TBG_A) ? ~(0xFUL << 8) : ~(0xFUL << 24);
	regval |= (tbg_typ == TBG_A) ? (icp << 8) : (icp << 24);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL2);

	/* 9. Set TBG-A (bit[3:0]) or TBG-B (bit[19:16]) KVCO range value  */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL3);
	regval &= (tbg_typ == TBG_A) ? ~(0xFUL << 0) : ~(0xFUL << 16);
	regval |= (tbg_typ == TBG_A) ? (vco_range << 0) : (vco_range << 16);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL3);

	/* 10. Set TBG-A (bit[3:0]) or TBG-B (bit[19:16]) INTPI value  */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL2);
	regval &= (tbg_typ == TBG_A) ? ~(0xFUL << 0) : ~(0xFUL << 16);
	regval |= (tbg_typ == TBG_A) ? (vco_intpi << 0) : (vco_intpi << 16);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL2);

	/* The A3700 FS requires to power down TGB before
	   changing PLL parameters. However it turns out that
	   this step is not essential. Putting TBG into reset
	   state is enough.
	   11. Power up TBG-A (bit[1]) or TBG-B (bit[17])
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL0);
	regval &= (tbg_typ == TBG_A) ? ~(BIT1) : ~(BIT17);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL0);
	*/

	/* 12. Release TBG-A (bit[0]) or TBG-B (bit[16]) from reset */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL0);
	regval &= (tbg_typ == TBG_A) ? ~(BIT0) : ~(BIT16);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL0);

	/* 13. Set TBG-A (bit[9]) or TBG-B (bit[25]) Bypass to disable - get CLKOUT from PLL */
	regval = readl(MVEBU_NORTH_BRG_TBG_CTRL1);
	regval &= (tbg_typ == TBG_A) ? ~(BIT9) : ~(BIT25);
	writel(regval, MVEBU_NORTH_BRG_TBG_CTRL1);

	/* 14. Wait for TBG-A (bit[15] and TBG-B (bit[31]) PLL lock */
	do {
		regval = readl(MVEBU_NORTH_BRG_TBG_CTRL1);
		regval &= BIT15 | BIT31;
	} while (regval != (BIT15 | BIT31));

	/* 15. Switch all North/South Bridge clock sources to clock divider
	       Will be done by function caller when required */

	/* Wait for stable clock output, suggested 40us */
	udelay(40);

	debug_exit();
	return 0;
}

#ifdef MVEBU_A3700_ENABLE_SSC
/******************************************************************************
* Name: set_ssc_mode
*
* Description: Setup Spread Spectrum Clcok for TBG-A/B
*
* Input:	None
* Output:	None
* Return:	Non-zero on error
******************************************************************************/
static u32 set_ssc_mode(void)
{
	debug_enter();
	return 0;
	debug_exit();
}
#endif /* MVEBU_A3700_ENABLE_SSC */

/******************************************************************************
* Name: set_clocks
*
* Description: Configure entire clock tree according to CPU and DDR frequency
*
* Input:	cpu_clk_mhz: required CPU clock
*		ddr_clk_mhz: required DDR clock
*		tbg_a_kvco_mhz: required TBG-A KVCO frequency or 0 for any available
*		tbg_b_kvco_mhz: required TBG-B KVCO frequency or 0 for any available
* Output:	None
* Return:	Non-zero if the requested settings are not supported
******************************************************************************/
u32 set_clocks(u32 cpu_clk_mhz, u32 ddr_clk_mhz, u32 tbg_a_kvco_mhz, u32 tbg_b_kvco_mhz)
{
	u32 clock_cfgs_cnt = sizeof(a3700_clock_configs)/sizeof(a3700_clock_configs[0]);
	u32 cfg;
	struct a3700_clock_cfg *clk_cfg = 0;
	u32 rval, reg_val;

	debug_enter();

	/* Search for the supported clock tree configuration */
	for (cfg = 0; cfg < clock_cfgs_cnt; cfg++) {
		if ((a3700_clock_configs[cfg].cpu_freq_mhz == cpu_clk_mhz) &&
		    (a3700_clock_configs[cfg].ddr_freq_mhz == ddr_clk_mhz) &&
		    ((tbg_a_kvco_mhz == 0) || (a3700_clock_configs[cfg].tbg_a.kvco_mhz == tbg_a_kvco_mhz)) &&
		    ((tbg_b_kvco_mhz == 0) || (a3700_clock_configs[cfg].tbg_b.kvco_mhz == tbg_b_kvco_mhz))) {
			clk_cfg = &a3700_clock_configs[cfg];
			debug("Found valid FREQ preset(#%d): CPU=%dMHz, DDR=%dMHz\n", cfg,
			      a3700_clock_configs[cfg].cpu_freq_mhz, a3700_clock_configs[cfg].ddr_freq_mhz);
			break;
		}
	}

	if (clk_cfg == 0) {
		error("Unable to find valid FREQ preset for CPU=%dMHz, DDR=%dMHz, TBG-A=%dMHz, TBG-B=%dMHz\n",
		      cpu_clk_mhz, ddr_clk_mhz, tbg_a_kvco_mhz, tbg_b_kvco_mhz);
		return 1;
	}

	/* Switch all North/South Bridge clock sources to XTAL
	   prior to make any change to the clock configuration */
	writel(0x00000000, MVEBU_NORTH_CLOCK_SELECT_REG);
	writel(0x00000000, MVEBU_SOUTH_CLOCK_SELECT_REG);

	rval = set_tbg_clock(clk_cfg->tbg_a.kvco_mhz,
			     clk_cfg->tbg_a.se_vcodiv,
			     clk_cfg->tbg_a.diff_vcodiv,
			     TBG_A);
	if (rval) {
		error("Failed to set TBG-A clock to %dMHz\n", clk_cfg->tbg_a.kvco_mhz);
		return rval;
	}

	rval = set_tbg_clock(clk_cfg->tbg_b.kvco_mhz,
			     clk_cfg->tbg_b.se_vcodiv,
			     clk_cfg->tbg_b.diff_vcodiv,
			     TBG_B);
	if (rval) {
		error("Failed to set TBG-B clock to %dMHz\n", clk_cfg->tbg_b.kvco_mhz);
		return rval;
	}

	/* North Bridge clock tree configuration */
	/* Enable all clocks */
	writel(NB_ALL_CLK_ENABLE, MVEBU_NORTH_CLOCK_ENABLE_REG);

	/* Setup dividers */
	reg_val = readl(MVEBU_NORTH_CLOCK_DIVIDER_SELECT0_REG);
	reg_val &= NB_CLK_DIV0_MASK_ALL;
	reg_val |= clk_cfg->nb_clk_cfg.div0.pwm_clk_prscl2 & 0x7;
	reg_val |= (clk_cfg->nb_clk_cfg.div0.pwm_clk_prscl1 & 0x7) << 3;
	reg_val |= (clk_cfg->nb_clk_cfg.div0.ddr_fclk_clk_prscl2 & 0x7) << 12;
	reg_val |= (clk_cfg->nb_clk_cfg.div0.ddr_fclk_clk_prscl1 & 0x7) << 15;
	reg_val |= (clk_cfg->nb_clk_cfg.div0.ddr_phy_mck_div_sel & 0x1) << 18;
	reg_val |= (clk_cfg->nb_clk_cfg.div0.trace_div_sel & 0x7) << 20;
	reg_val |= (clk_cfg->nb_clk_cfg.div0.counter_clk_prscl & 0x7) << 23;
	reg_val |= (clk_cfg->nb_clk_cfg.div0.a53_cpu_clk_prscl & 0x7) << 28;
	writel(reg_val, MVEBU_NORTH_CLOCK_DIVIDER_SELECT0_REG);

	reg_val = readl(MVEBU_NORTH_CLOCK_DIVIDER_SELECT1_REG);
	reg_val &= NB_CLK_DIV1_MASK_ALL;
	reg_val |= clk_cfg->nb_clk_cfg.div1.sec_at_clk_prscl2 & 0x7;
	reg_val |= (clk_cfg->nb_clk_cfg.div1.sec_at_clk_prscl1 & 0x7) << 3;
	reg_val |= (clk_cfg->nb_clk_cfg.div1.sec_dap_clk_prscl2 & 0x7) << 6;
	reg_val |= (clk_cfg->nb_clk_cfg.div1.sec_dap_clk_prscl1 & 0x7) << 9;
	reg_val |= (clk_cfg->nb_clk_cfg.div1.tsecm_clk_prscl2 & 0x7) << 12;
	reg_val |= (clk_cfg->nb_clk_cfg.div1.tsecm_clk_prscl1 & 0x7) << 15;
	reg_val |= (clk_cfg->nb_clk_cfg.div1.setm_tmx_clk_prscl & 0x7) << 18;
	reg_val |= (clk_cfg->nb_clk_cfg.div1.sqf_clk_prscl2 & 0x7) << 24;
	reg_val |= (clk_cfg->nb_clk_cfg.div1.sqf_clk_prscl1 & 0x7) << 27;
	writel(reg_val, MVEBU_NORTH_CLOCK_DIVIDER_SELECT1_REG);

	reg_val = readl(MVEBU_NORTH_CLOCK_DIVIDER_SELECT2_REG);
	reg_val &= NB_CLK_DIV2_MASK_ALL;
	reg_val |= clk_cfg->nb_clk_cfg.div2.wc_ahb_div_sel & 0x7;
	reg_val |= (clk_cfg->nb_clk_cfg.div2.sata_h_clk_prscl2 & 0x7) << 7;
	reg_val |= (clk_cfg->nb_clk_cfg.div2.sata_h_clk_prscl1 & 0x7) << 10;
	reg_val |= (clk_cfg->nb_clk_cfg.div2.mmc_clk_prscl2 & 0x7) << 13;
	reg_val |= (clk_cfg->nb_clk_cfg.div2.mmc_clk_prscl1 & 0x7) << 16;
	reg_val |= (clk_cfg->nb_clk_cfg.div2.eip97_clk_prscl2 & 0x7) << 19;
	reg_val |= (clk_cfg->nb_clk_cfg.div2.eip97_clk_prscl1 & 0x7) << 22;
	reg_val |= (clk_cfg->nb_clk_cfg.div2.atb_clk_div_sel & 0x3) << 25;
	/* Always use "divide by 1 (0)" for counter clock and ignore table value
	reg_val |= (clk_cfg->nb_clk_cfg.div2.cpu_cnt_clk_div_sel & 0x3) << 27;
	*/
	reg_val |= (clk_cfg->nb_clk_cfg.div2.plkdbg_clk_div_sel & 0x3) << 29;
	writel(reg_val, MVEBU_NORTH_CLOCK_DIVIDER_SELECT2_REG);

	/* Setup connection to clock lines */
	reg_val = readl(MVEBU_NORTH_CLOCK_TBG_SELECT_REG);
	reg_val &= NB_CLK_TBG_SEL_MASK_ALL;
	reg_val |= clk_cfg->nb_clk_cfg.clock_sel.mmc_pclk_sel & 0x3;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.sata_host_pclk_sel & 0x3) << 2;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.sec_at_pclk_sel & 0x3) << 4;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.sec_dap_pclk_sel & 0x3) << 6;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.tsecm_pclk_sel & 0x3) << 8;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.setm_tmx_pclk_sel & 0x3) << 10;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.sqf_pclk_sel & 0x3) << 12;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.pwm_pclk_sel & 0x3) << 14;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.ddr_fclk_pclk_sel & 0x3) << 16;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.trace_pclk_sel & 0x3) << 18;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.counter_pclk_sel & 0x3) << 20;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.a53_cpu_pclk_sel & 0x3) << 22;
	reg_val |= (clk_cfg->nb_clk_cfg.clock_sel.eip97_pclk_sel & 0x3) << 24;
	writel(reg_val, MVEBU_NORTH_CLOCK_TBG_SELECT_REG);

	/* South Bridge clock tree configuration */
	/* Enable all clocks */
	writel(SB_ALL_CLK_ENABLE, MVEBU_SOUTH_CLOCK_ENABLE_REG);

	/* Setup dividers */
	reg_val = readl(MVEBU_SOUTH_CLOCK_DIVIDER_SELECT0_REG);
	reg_val &= SB_CLK_DIV0_MASK_ALL;
	reg_val |= (clk_cfg->sb_clk_cfg.div0.sdio_clk_prscl2 & 0x7) << 3;
	reg_val |= (clk_cfg->sb_clk_cfg.div0.sdio_clk_prscl1 & 0x7) << 6;
	reg_val |= (clk_cfg->sb_clk_cfg.div0.usb32_usb2_sys_clk_prscl2 & 0x7) << 9;
	reg_val |= (clk_cfg->sb_clk_cfg.div0.usb32_usb2_sys_clk_prscl1 & 0x7) << 12;
	reg_val |= (clk_cfg->sb_clk_cfg.div0.usb32_ss_sys_clk_prscl2 & 0x7) << 15;
	reg_val |= (clk_cfg->sb_clk_cfg.div0.usb32_ss_sys_clk_prscl1 & 0x7) << 18;
	reg_val |= (clk_cfg->sb_clk_cfg.div0.sb_axi_clk_prscl2 & 0x7) << 21;
	reg_val |= (clk_cfg->sb_clk_cfg.div0.sb_axi_clk_prscl1 & 0x7) << 24;
	writel(reg_val, MVEBU_SOUTH_CLOCK_DIVIDER_SELECT0_REG);

	reg_val = readl(MVEBU_SOUTH_CLOCK_DIVIDER_SELECT1_REG);
	reg_val &= SB_CLK_DIV1_MASK_ALL;
	reg_val |= (clk_cfg->sb_clk_cfg.div1.gbe_125_clk_prscl2 & 0x7) << 6;
	reg_val |= (clk_cfg->sb_clk_cfg.div1.gbe_125_clk_prscl1 & 0x7) << 9;
	reg_val |= (clk_cfg->sb_clk_cfg.div1.gbe_bm_core_clk_div2_sel & 0x1) << 12;
	reg_val |= (clk_cfg->sb_clk_cfg.div1.gbe1_core_clk_div2_sel & 0x1) << 13;
	reg_val |= (clk_cfg->sb_clk_cfg.div1.gbe0_core_clk_div2_sel & 0x1) << 14;
	reg_val |= (clk_cfg->sb_clk_cfg.div1.gbe_core_clk_prscl2 & 0x7) << 18;
	reg_val |= (clk_cfg->sb_clk_cfg.div1.gbe_core_clk_prscl1 & 0x7) << 21;
	writel(reg_val, MVEBU_SOUTH_CLOCK_DIVIDER_SELECT1_REG);

	reg_val = readl(MVEBU_SOUTH_CLOCK_DIVIDER_SELECT2_REG);
	reg_val &= SB_CLK_DIV2_MASK_ALL;
	reg_val |= (clk_cfg->sb_clk_cfg.div2.gbe_50_clk_prscl2 & 0x7) << 6;
	reg_val |= (clk_cfg->sb_clk_cfg.div2.gbe_50_clk_prscl1 & 0x7) << 9;
	writel(reg_val, MVEBU_SOUTH_CLOCK_DIVIDER_SELECT2_REG);

	/* Setup connection to clock lines */
	reg_val = readl(MVEBU_SOUTH_CLOCK_TBG_SELECT_REG);
	reg_val &= SB_CLK_TBG_SEL_MASK_ALL;
	reg_val |= (clk_cfg->sb_clk_cfg.clock_sel.gbe_50_pclk_sel & 0x3) << 6;
	reg_val |= (clk_cfg->sb_clk_cfg.clock_sel.gbe_core_pclk_sel & 0x3) << 8;
	reg_val |= (clk_cfg->sb_clk_cfg.clock_sel.gbe_125_pclk_sel & 0x3) << 10;
	reg_val |= (clk_cfg->sb_clk_cfg.clock_sel.sdio_pclk_sel & 0x3) << 14;
	reg_val |= (clk_cfg->sb_clk_cfg.clock_sel.usb32_usb2_sys_pclk_sel & 0x3) << 16;
	reg_val |= (clk_cfg->sb_clk_cfg.clock_sel.usb32_ss_sys_pclk_sel & 0x3) << 18;
	reg_val |= (clk_cfg->sb_clk_cfg.clock_sel.sb_axi_pclk_sel & 0x3) << 20;
	writel(reg_val, MVEBU_SOUTH_CLOCK_TBG_SELECT_REG);


#ifdef MVEBU_A3700_ENABLE_SSC
	set_ssc_mode();
#else
	/* Disable SSC for TBG-A (bit[10]) and TBG-B (bit[26]) */
	reg_val = readl(MVEBU_NORTH_BRG_TBG_CTRL3);
	reg_val &= ~(BIT10 | BIT26);
	writel(reg_val, MVEBU_NORTH_BRG_TBG_CTRL3);
#endif /* MVEBU_A3700_ENABLE_SSC */

	/* Switch all North/South Bridge clock sources from XTAL to clock divider
	   excepting counter clock, which remains to be connected to XTAL */
	writel(0x00009FFF, MVEBU_NORTH_CLOCK_SELECT_REG);
	writel(0x000007AA, MVEBU_SOUTH_CLOCK_SELECT_REG);

	debug_exit();
	return rval;
}

int init_clock(void)
{
	int node, count, idx, ret;
	const void *blob = gd->fdt_blob;
	int tbl_sz = sizeof(a3700_clock_configs)/sizeof(a3700_clock_configs[0]);

	debug_enter();

	count = fdtdec_find_aliases_for_id(blob, "freq", COMPAT_MVEBU_A3700_FREQ, &node, 1);
	if (count == 0) {
		error("The frequency preset is not defined in DT, using default\n");
		idx = MVEBU_A3700_DEF_CLOCK_PRESET_IDX;
	} else {
		idx = fdtdec_get_int(blob, node, "preset", MVEBU_A3700_DEF_CLOCK_PRESET_IDX);
		if ((idx >= tbl_sz) || (idx < 0)) {
			error("Unsupported frequency preset in DT (%d), using default\n", idx);
			idx = MVEBU_A3700_DEF_CLOCK_PRESET_IDX;
		}
	}

	printf("Setting clocks to CPU=%dMHz and DDR=%dMHz\n",
		a3700_clock_configs[idx].cpu_freq_mhz, a3700_clock_configs[idx].ddr_freq_mhz);

	ret =  set_clocks(a3700_clock_configs[idx].cpu_freq_mhz,
			  a3700_clock_configs[idx].ddr_freq_mhz,
			  a3700_clock_configs[idx].tbg_a.kvco_mhz,
			  a3700_clock_configs[idx].tbg_b.kvco_mhz);
	if (ret)
		error("Failed to configure system clocks\n");

	debug_exit();
	return ret;
}

static u32 get_tbg_clk(enum a3700_clock_line tbg_typ)
{
	u32 tbg_M, tbg_N, vco_div;
	u32 ref, reg_val;

	/* get ref clock */
	ref = get_ref_clk();

	/* get M, N */
	reg_val = readl(MVEBU_NORTH_BRG_TBG_CTRL7);
	tbg_M = ((tbg_typ == TBG_A_S) || (tbg_typ == TBG_A_P)) ?
		(reg_val & 0x1FFUL) : ((reg_val >> 16) & 0x1FFUL);
	tbg_M = (tbg_M == 0) ? 1 : tbg_M;

	reg_val = readl(MVEBU_NORTH_BRG_TBG_CTRL0);
	tbg_N = ((tbg_typ == TBG_A_S) || (tbg_typ == TBG_A_P)) ?
		((reg_val >> 2) & 0x1FFUL) : ((reg_val >> 18) & 0x1FFUL);

	if ((tbg_typ == TBG_A_S) || (tbg_typ == TBG_B_S)) {
		/* get SE VCODIV */
		reg_val = readl(MVEBU_NORTH_BRG_TBG_CTRL1);
		reg_val = (tbg_typ == TBG_A_S) ?
			  (reg_val & 0x1FFUL) : ((reg_val >> 16) & 0x1FFUL);
	} else {
		/* get DIFF VCODIV */
		reg_val = readl(MVEBU_NORTH_BRG_TBG_CTRL8);
		reg_val = (tbg_typ == TBG_A_P) ?
			  ((reg_val >> 1) & 0x1FFUL) : ((reg_val >> 17) & 0x1FFUL);
	}
	if (reg_val > 7)
		return 0; /*invalid*/

	vco_div = 0x1 << reg_val;

	return ((tbg_N * ref) << 2)/(tbg_M * vco_div);
}

u32 get_cpu_clk(void)
{
	u32 tbg, cpu_prscl;
	enum a3700_clock_line tbg_typ;

	/* 1. check cpu clock select */
	if (!((readl(MVEBU_NORTH_CLOCK_SELECT_REG) >> 15) & 0x1))
		return 0; /* CPU clock is using XTAL output*/

	/* 2. get TBG select */
	tbg_typ = (readl(MVEBU_NORTH_CLOCK_TBG_SELECT_REG) >> 22) & 0x3;

	/* 3. get TBG clock */
	tbg = get_tbg_clk(tbg_typ);
	if (tbg == 0)
		return 0;

	/* 4. get CPU clk divider */
	cpu_prscl = (readl(MVEBU_NORTH_CLOCK_DIVIDER_SELECT0_REG) >> 28) & 0x7;
	if (cpu_prscl == 7)
		return 0; /* divider value error */

	return tbg/cpu_prscl;
}

u32 get_ddr_clk(void)
{
	u32 tbg;

	/* 1. check DDR clock select */
	if (!((readl(MVEBU_NORTH_CLOCK_SELECT_REG) >> 10) & 0x1))
		return 0; /* DDR clock is using XTAL output*/

	/* 2. get TBG_A clock */
	tbg = get_tbg_clk(TBG_A_S);
	if (tbg == 0)
		return 0;

	return tbg>>1;
}
