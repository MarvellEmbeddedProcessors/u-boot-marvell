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

#include "mv_ddr_mc6_drv.h"
#include "ddr3_init.h"

/* extern */
extern struct page_element page_param[];	/* FIXME: this data base should have get, set functions */

void mv_ddr_mc6_timing_regs_cfg(unsigned int freq_mhz)
{
	struct mv_ddr_mc6_timing mc6_timing;
	unsigned int page_size;
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	/* get the spped bin index */
	enum hws_speed_bin speed_bin_index = tm->interface_params[IF_ID_0].speed_bin_index;

	/* calculate memory size */
	enum mv_ddr_die_capacity memory_size = tm->interface_params[IF_ID_0].memory_size;

	/* calculate page size */
	page_size = tm->interface_params[IF_ID_0].bus_width == MV_DDR_DEV_WIDTH_8BIT ?
		page_param[memory_size].page_size_8bit : page_param[memory_size].page_size_16bit;
	/* printf("page_size = %d\n", page_size); */

	/* calculate t_clck */
	mc6_timing.t_ckclk = MEGA / freq_mhz;
	/* printf("t_ckclk = %d\n", mc6_timing.t_ckclk); */

	/* calculate t_refi  */
	mc6_timing.t_refi = (tm->interface_params[IF_ID_0].interface_temp == MV_DDR_TEMP_HIGH) ? TREFI_HIGH : TREFI_LOW;

	/* the t_refi is in nsec */
	mc6_timing.t_refi = mc6_timing.t_refi / (MEGA / MV_DDR_MC6_FCLK_200MHZ_IN_KILO);
	/* printf("t_refi = %d\n", mc6_timing.t_refi); */
	mc6_timing.t_wr = speed_bin_table(speed_bin_index, SPEED_BIN_TWR);
	/* printf("t_wr = %d\n", mc6_timing.t_wr); */
	mc6_timing.t_wr = time_to_nclk(mc6_timing.t_wr, mc6_timing.t_ckclk);
	/* printf("t_wr = %d\n", mc6_timing.t_wr); */

	/* calculate t_rrd */
	mc6_timing.t_rrd = (page_size == 1) ? speed_bin_table(speed_bin_index, SPEED_BIN_TRRD1K) :
		speed_bin_table(speed_bin_index, SPEED_BIN_TRRD2K);
	/* printf("t_rrd = %d\n", mc6_timing.t_rrd); */
	mc6_timing.t_rrd = GET_MAX_VALUE(mc6_timing.t_ckclk * 4, mc6_timing.t_rrd);
	/* printf("t_rrd = %d\n", mc6_timing.t_rrd); */
	mc6_timing.t_rrd = time_to_nclk(mc6_timing.t_rrd, mc6_timing.t_ckclk);
	/* printf("t_rrd = %d\n", mc6_timing.t_rrd); */

	/* calculate t_faw */
	if (page_size == 1) {
		mc6_timing.t_faw = speed_bin_table(speed_bin_index, SPEED_BIN_TFAW1K);
		/* printf("t_faw = %d\n", mc6_timing.t_faw); */
		mc6_timing.t_faw = time_to_nclk(mc6_timing.t_faw, mc6_timing.t_ckclk);
		mc6_timing.t_faw = GET_MAX_VALUE(mc6_timing.t_ckclk * 20, mc6_timing.t_faw);
		/* printf("t_faw = %d\n", mc6_timing.t_faw); */
	} else {	/* page size =2, we do not support page size 0.5k */
		mc6_timing.t_faw = speed_bin_table(speed_bin_index, SPEED_BIN_TFAW2K);
		/* printf("t_faw = %d\n", mc6_timing.t_faw); */
		mc6_timing.t_faw = time_to_nclk(mc6_timing.t_faw, mc6_timing.t_ckclk);
		mc6_timing.t_faw = GET_MAX_VALUE(mc6_timing.t_ckclk * 28, mc6_timing.t_faw);
		/* printf("t_faw = %d\n", mc6_timing. t_faw); */
	}

	/* calculate t_rtp */
	mc6_timing.t_rtp = speed_bin_table(speed_bin_index, SPEED_BIN_TRTP);
	/* printf("t_rtp = %d\n", mc6_timing.t_rtp); */
	mc6_timing.t_rtp = GET_MAX_VALUE(mc6_timing.t_ckclk * 4, mc6_timing.t_rtp);
	/* printf("t_rtp = %d\n", mc6_timing.t_rtp); */
	mc6_timing.t_rtp = time_to_nclk(mc6_timing.t_rtp, mc6_timing.t_ckclk);
	/* printf("t_rtp = %d\n", mc6_timing.t_rtp); */

	/* calculate t_mode */
	mc6_timing.t_mod = speed_bin_table(speed_bin_index, SPEED_BIN_TMOD);
	/* printf("t_mod = %d\n", mc6_timing.t_mod); */
	mc6_timing.t_mod = GET_MAX_VALUE(mc6_timing.t_ckclk * 24, mc6_timing.t_mod);
	/* printf("t_mod = %d\n", mc6_timing.t_mod); */
	mc6_timing.t_mod = time_to_nclk(mc6_timing.t_mod, mc6_timing.t_ckclk);
	/* printf("t_mod = %d\n",mc6_timing. t_mod); */

	/* calculate t_wtr */
	mc6_timing.t_wtr = speed_bin_table(speed_bin_index, SPEED_BIN_TWTR);
	/* printf("t_wtr = %d\n", mc6_timing.t_wtr); */
	mc6_timing.t_wtr = GET_MAX_VALUE(mc6_timing.t_ckclk * 2, mc6_timing.t_wtr);
	/* printf("t_wtr = %d\n", mc6_timing.t_wtr); */
	mc6_timing.t_wtr = time_to_nclk(mc6_timing.t_wtr, mc6_timing.t_ckclk);
	/* printf("t_wtr = %d\n", mc6_timing.t_wtr); */

	/* calculate t_wtr_l */
	mc6_timing.t_wtr_l = speed_bin_table(speed_bin_index, SPEED_BIN_TWTRL);
	/* printf("t_wtr_l = %d\n", mc6_timing.t_wtr_l); */
	mc6_timing.t_wtr_l = GET_MAX_VALUE(mc6_timing.t_ckclk * 4, mc6_timing.t_wtr_l);
	/* printf("t_wtr_l = %d\n", mc6_timing.t_wtr_l); */
	mc6_timing.t_wtr_l = time_to_nclk(mc6_timing.t_wtr_l, mc6_timing.t_ckclk);
	/* printf("t_wtr_l = %d\n", mc6_timing.t_wtr_l); */

	/* calculate t_xp */
	mc6_timing.t_xp = MV_DDR_MC6_TIMING_T_XP;
	/* printf("t_xp = %d\n", mc6_timing.t_xp); */
	mc6_timing.t_xp = GET_MAX_VALUE(mc6_timing.t_ckclk * 4, mc6_timing.t_xp);
	/* printf("t_xp = %d\n", mc6_timing.t_xp); */
	mc6_timing.t_xp = time_to_nclk(mc6_timing.t_xp, mc6_timing.t_ckclk);
	/* printf("t_xp = %d\n", mc6_timing.t_xp); */

	/* calculate t_cke */
	mc6_timing.t_cke = MV_DDR_MC6_TIMING_T_CKE;
	/* printf("t_cke = %d\n", mc6_timing.t_cke); */
	mc6_timing.t_cke = GET_MAX_VALUE(mc6_timing.t_ckclk * 3, mc6_timing.t_cke);
	/* printf("t_cke = %d\n", mc6_timing.t_cke); */
	mc6_timing.t_cke = time_to_nclk(mc6_timing.t_cke, mc6_timing.t_ckclk);
	/* printf("t_cke = %d\n", mc6_timing.t_cke); */

	/* calculate t_ckesr */
	mc6_timing.t_ckesr = mc6_timing.t_cke + 1;
	/* printf("t_ckesr = %d\n", mc6_timing.t_ckesr); */

	/* calculate t_cpded */
	mc6_timing.t_cpded = mc6_timing.t_ckclk * 4;
	/* printf("t_cpded = %d\n", mc6_timing.t_cpded); */

	/* calculate t_cksrx */
	mc6_timing.t_cksrx = MV_DDR_MC6_TIMING_T_CKSRX;
	/* printf("t_cksrx = %d\n", mc6_timing.t_cksrx); */
	mc6_timing.t_cksrx = GET_MAX_VALUE(mc6_timing.t_ckclk * 5, mc6_timing.t_cksrx);
	/* printf("t_cksrx = %d\n", mc6_timing.t_cksrx); */
	mc6_timing.t_cksrx = time_to_nclk(mc6_timing.t_cksrx, mc6_timing.t_ckclk);
	/* printf("t_cksrx = %d\n", mc6_timing.t_cksrx); */

	/* calculate t_cksre */
	mc6_timing.t_cksre = mc6_timing.t_cksrx;
	/* printf("t_cksre = %d\n", mc6_timing.t_cksre); */

	/* calculate t_ras */
	mc6_timing.t_ras = speed_bin_table(speed_bin_index, SPEED_BIN_TRAS);
	/* printf("t_ras = %d\n", mc6_timing.t_ras); */
	mc6_timing.t_ras = time_to_nclk(mc6_timing.t_ras, mc6_timing.t_ckclk);
	/* printf("t_ras = %d\n", mc6_timing.t_ras); */

	/* calculate t_rcd */
	mc6_timing.t_rcd = speed_bin_table(speed_bin_index, SPEED_BIN_TRCD);
	/* printf("t_rcd = %d\n", mc6_timing.t_rcd); */
	mc6_timing.t_rcd = time_to_nclk(mc6_timing.t_rcd, mc6_timing.t_ckclk);
	/* printf("t_rcd = %d\n", mc6_timing.t_rcd); */

	/* calculate t_rp */
	mc6_timing.t_rp = speed_bin_table(speed_bin_index, SPEED_BIN_TRP);
	/* printf("t_rp = %d\n", mc6_timing.t_rp); */
	mc6_timing.t_rp = time_to_nclk(mc6_timing.t_rp, mc6_timing.t_ckclk);
	/* printf("t_rp = %d\n", mc6_timing.t_rp); */

	/*calculate t_rfc */
	mc6_timing.t_rfc = time_to_nclk(rfc_table[memory_size] * 1000, mc6_timing.t_ckclk);
	/* printf("t_rfc = %d\n", mc6_timing.t_rfc); */

	/* calculate t_xs */
	mc6_timing.t_xs = mc6_timing.t_rfc + time_to_nclk(MV_DDR_MC6_TIMING_T_XS_OVER_TRFC, mc6_timing.t_ckclk);
	/* printf("t_xs = %d\n", mc6_timing.t_xs); */

	/* calculate t_rrd_l */
	mc6_timing.t_rrd_l = MV_DDR_MC6_TIMING_T_RRDL;
	/* printf("t_rrd_l = %d\n", mc6_timing.t_rrd_l); */
	mc6_timing.t_rrd_l = GET_MAX_VALUE(mc6_timing.t_ckclk * 4, mc6_timing.t_rrd_l);
	/* printf("t_rrd_l = %d\n", mc6_timing. t_rrd_l); */
	mc6_timing.t_rrd_l = time_to_nclk(mc6_timing.t_rrd_l, mc6_timing.t_ckclk);
	/* printf("t_rrd_l = %d\n", mc6_timing.t_rrd_l); */

	/* calculate t_ccd_l */
	mc6_timing.t_ccd_l = 6; /* FIXME: insert to speed bin table */
	/* printf("t_ccd_l = %d\n", mc6_timing.t_ccd_l); */

	/* calculate t_rc */
	mc6_timing.t_rc = speed_bin_table(speed_bin_index, SPEED_BIN_TRC);
	/* printf("t_rc = %d\n", mc6_timing.t_rc); */
	mc6_timing.t_rc = time_to_nclk(mc6_timing.t_rc, mc6_timing.t_ckclk);
	/* printf("t_rc = %d\n", mc6_timing.t_rc); */

	/* constant timing parameters */
	mc6_timing.read_gap_extend = MV_DDR_MC6_TIMING_READ_GAP_EXTEND;
	/* printf("read_gap_extend = %d\n", mc6_timing.read_gap_extend); */

	mc6_timing.t_zqoper = MV_DDR_MC6_TIMING_T_ZQOPER;
	/* printf("t_zqoper = %d\n", mc6_timing.t_zqoper); */

	mc6_timing.t_res = MV_DDR_MC6_TIMING_T_RES;
	/* printf("t_res = %d\n", mc6_timing.t_res); */
	mc6_timing.t_res = time_to_nclk(mc6_timing.t_res, mc6_timing.t_ckclk);
	/* printf("t_res = %d\n", mc6_timing.t_res); */

	mc6_timing.t_resinit = MV_DDR_MC6_TIMING_T_RESINIT;
	/* printf("t_resinit = %d\n", mc6_timing.t_resinit); */
	mc6_timing.t_resinit = time_to_nclk(mc6_timing.t_resinit, mc6_timing.t_ckclk);
	/* printf("t_resinit = %d\n", mc6_timing.t_resinit); */

	mc6_timing.t_restcke = MV_DDR_MC6_TIMING_T_RESTCKE;
	/* printf("t_restcke = %d\n", mc6_timing.t_restcke); */
	mc6_timing.t_restcke = time_to_nclk(mc6_timing.t_restcke, mc6_timing.t_ckclk);
	/* printf("t_restcke = %d\n", mc6_timing.t_restcke); */

	mc6_timing.t_actpden = MV_DDR_MC6_TIMING_T_ACTPDEN;
	/* printf("t_actpden = %d\n", mc6_timing.t_actpden); */

	mc6_timing.t_zqinit = MV_DDR_MC6_TIMING_T_ZQINIT;
	/* printf("t_zqinit = %d\n", mc6_timing.t_zqinit); */

	mc6_timing.t_zqcs = MV_DDR_MC6_TIMING_T_ZQCS;
	/* printf("t_zqcs = %d\n", mc6_timing.t_zqcs); */

	mc6_timing.t_ccd = MV_DDR_MC6_TIMING_T_CCD;
	/* printf("t_ccd = %d\n", mc6_timing.t_ccd); */

	mc6_timing.t_mrd = MV_DDR_MC6_TIMING_T_MRD;
	/* printf("t_mrd = %d\n", mc6_timing.t_mrd); */

	mc6_timing.t_mpx_lh = MV_DDR_MC6_TIMING_T_MPX_LH;
	/* printf("t_mpx_lh = %d\n", mc6_timing.t_mpx_lh); */

	mc6_timing.t_mpx_s = MV_DDR_MC6_TIMING_T_MPX_S;
	/* printf("t_mpx_s = %d\n", mc6_timing.t_mpx_s); */

	mc6_timing.t_xmp = mc6_timing.t_rfc + time_to_nclk(MV_DDR_MC6_TIMING_T_XMP_OVER_TRFC, mc6_timing.t_ckclk);
	/* printf("t_xmp = %d\n", mc6_timing.t_xmp); */

	mc6_timing.t_mrd_pda = MV_DDR_MC6_TIMING_T_MRD_PDA;
	/* printf("t_mrd_pda = %d\n", mc6_timing.t_mrd_pda); */
	mc6_timing.t_mrd_pda = GET_MAX_VALUE(mc6_timing.t_ckclk * 16, mc6_timing.t_mrd_pda);
	/* printf("t_mrd_pda = %d\n", mc6_timing. t_mrd_pda); */

	mc6_timing.t_xsdll = speed_bin_table(speed_bin_index, SPEED_BIN_TXSDLL);
	/* printf("t_xsdll = %d\n", mc6_timing.t_xsdll); */

	mc6_timing.t_rwd_ext_dly = MV_DDR_MC6_TIMING_T_RWD_EXT_DLY;
	/* printf("t_rwd_ext_dly = %d\n", mc6_timing.t_rwd_ext_dly); */

	mc6_timing.t_wl_early = MV_DDR_MC6_TIMING_T_WL_EARLY;
	/* printf("t_wl_early = %d\n", mc6_timing.t_wl_early); */

	mc6_timing.t_ccd_ccs_wr_ext_dly = MV_DDR_MC6_TIMMING_T_CCD_CCS_WR_EXT_DLY;
	/* printf("t_ccd_ccs_wr_ext_dly = %d\n", mc6_timing.t_ccd_ccs_wr_ext_dly); */

	mc6_timing.t_ccd_ccs_ext_dly = MV_DDR_MC6_TIMING_T_CCD_CCS_EXT_DLY;
	/* printf("t_ccd_ccs_ext_dly = %d\n", mc6_timing.t_ccd_ccs_ext_dly); */

	mc6_timing.cl = tm->interface_params[IF_ID_0].cas_l;
	mc6_timing.cwl = tm->interface_params[IF_ID_0].cas_wl;

	/* configure the timing registers */
		reg_bit_clrset(MC6_REG_DRAM_CFG1,
			       mc6_timing.cwl << MC6_CWL_OFFS | mc6_timing.cl << MC6_CL_OFFS,
			       MC6_CWL_MASK << MC6_CWL_OFFS | MC6_CL_MASK << MC6_CL_OFFS);
	/* printf("MC6_REG_DRAM_CFG1 addr 0x%x, data 0x%x\n", MC6_REG_DRAM_CFG1,
	       reg_read(MC6_REG_DRAM_CFG1)); */

	reg_bit_clrset(MC6_REG_INIT_TIMING_CTRL_0,
		       mc6_timing.t_restcke << MC6_INIT_COUNT_NOP_OFFS,
		       MC6_INIT_COUNT_NOP_MASK << MC6_INIT_COUNT_NOP_OFFS);
	/* printf("MC6_REG_INIT_TIMING_CTRL_0 addr 0x%x, data 0x%x\n", MC6_REG_INIT_TIMING_CTRL_0,
	       reg_read(MC6_REG_INIT_TIMING_CTRL_0)); */

	reg_bit_clrset(MC6_REG_INIT_TIMING_CTRL_1,
		       mc6_timing.t_resinit << MC6_INIT_COUNT_OFFS,
		       MC6_INIT_COUNT_MASK << MC6_INIT_COUNT_OFFS);
	/* printf("MC6_REG_INIT_TIMING_CTRL_1 addr 0x%x, data 0x%x\n", MC6_REG_INIT_TIMING_CTRL_1,
	       reg_read(MC6_REG_INIT_TIMING_CTRL_1)); */

	reg_bit_clrset(MC6_REG_INIT_TIMING_CTRL_2,
		       mc6_timing.t_res << MC6_RESET_COUNT_OFFS,
		       MC6_RESET_COUNT_MASK << MC6_RESET_COUNT_OFFS);
	/* printf("MC6_REG_INIT_TIMING_CTRL_2 addr 0x%x, data 0x%x\n", MC6_REG_INIT_TIMING_CTRL_2,
	       reg_read(MC6_REG_INIT_TIMING_CTRL_2)); */

	reg_bit_clrset(MC6_REG_ZQC_TIMING_0,
		       mc6_timing.t_zqinit << MC6_TZQINIT_OFFS,
		       MC6_TZQINIT_MASK << MC6_TZQINIT_OFFS);
	/* printf("MC6_REG_ZQC_TIMING_0 addr 0x%x, data 0x%x\n", MC6_REG_ZQC_TIMING_0,
	       reg_read(MC6_REG_ZQC_TIMING_0)); */

	reg_bit_clrset(MC6_REG_ZQC_TIMING_1,
		       mc6_timing.t_zqoper << MC6_TZQCL_OFFS |
		       mc6_timing.t_zqcs << MC6_TZQCS_OFFS,
		       MC6_TZQCL_MASK << MC6_TZQCL_OFFS |
		       MC6_TZQCS_MASK << MC6_TZQCS_OFFS);
	/* printf("MC6_REG_ZQC_TIMING_1 addr 0x%x, data 0x%x\n", MC6_REG_ZQC_TIMING_1,
	       reg_read(MC6_REG_ZQC_TIMING_1)); */

	reg_bit_clrset(MC6_REG_REFRESH_TIMING,
		       mc6_timing.t_refi << MC6_TREFI_OFFS |
		       mc6_timing.t_rfc << MC6_TRFC_OFFS,
		       MC6_TREFI_MASK << MC6_TREFI_OFFS |
		       MC6_TRFC_MASK << MC6_TRFC_OFFS);
	/* printf("MC6_REG_REFRESH_TIMING addr 0x%x, data 0x%x\n", MC6_REG_REFRESH_TIMING,
	       reg_read(MC6_REG_REFRESH_TIMING)); */

	reg_bit_clrset(MC6_REG_SELF_REFRESH_TIMING_0,
		       mc6_timing.t_xsdll << MC6_TXSRD_OFFS |
		       mc6_timing.t_xs << MC6_TXSNR_OFFS,
		       MC6_TXSRD_MASK << MC6_TXSRD_OFFS |
		       MC6_TXSNR_MASK << MC6_TXSNR_OFFS);
	/* printf("MC6_REG_SELF_REFRESH_TIMING_0 addr 0x%x, data 0x%x\n", MC6_REG_SELF_REFRESH_TIMING_0,
	       reg_read(MC6_REG_SELF_REFRESH_TIMING_0)); */

	reg_bit_clrset(MC6_REG_SELF_REFRESH_TIMING_1,
		       mc6_timing.t_cksrx << MC6_TCKSRX_OFFS |
		       mc6_timing.t_cksre << MC6_TCKSRE_OFFS,
		       MC6_TCKSRX_MASK << MC6_TCKSRX_OFFS |
		       MC6_TCKSRE_MASK << MC6_TCKSRE_OFFS);
	/* printf("MC6_REG_SELF_REFRESH_TIMING_1 addr 0x%x, data 0x%x\n", MC6_REG_SELF_REFRESH_TIMING_1,
	       reg_read(MC6_REG_SELF_REFRESH_TIMING_1)); */

	reg_bit_clrset(MC6_REG_POWER_DOWN_TIMING_0,
		       TXARDS << MC6_TCKSRX_OFFS |
		       mc6_timing.t_xp << MC6_TXP_OFFS |
		       mc6_timing.t_ckesr << MC6_TCKESR_OFFS |
		       mc6_timing.t_cpded << MC6_TCPDED_OFFS,
		       MC6_TXARDS_MASK << MC6_TCKSRX_OFFS |
		       MC6_TXP_MASK << MC6_TXP_OFFS |
		       MC6_TCKESR_MASK << MC6_TCKESR_OFFS |
		       MC6_TCPDED_MASK << MC6_TCPDED_OFFS);
	/* printf("MC6_REG_POWER_DOWN_TIMING_0 addr 0x%x, data 0x%x\n", MC6_REG_POWER_DOWN_TIMING_0,
	       reg_read(MC6_REG_POWER_DOWN_TIMING_0)); */

	reg_bit_clrset(MC6_REG_POWER_DOWN_TIMING_1,
		       mc6_timing.t_actpden << MC6_TPDEN_OFFS,
		       MC6_TPDEN_MASK << MC6_TPDEN_OFFS);
	/* printf("MC6_REG_POWER_DOWN_TIMING_1 addr 0x%x, data 0x%x\n", MC6_REG_POWER_DOWN_TIMING_1,
	       reg_read(MC6_REG_POWER_DOWN_TIMING_1)); */

	reg_bit_clrset(MC6_REG_MRS_TIMING,
		       mc6_timing.t_mrd << MC6_TMRD_OFFS |
		       mc6_timing.t_mod << MC6_TMOD_OFFS,
		       MC6_TMRD_MASK << MC6_TMRD_OFFS |
		       MC6_TMOD_MASK << MC6_TMOD_OFFS);
	/* printf("MC6_REG_MRS_TIMING addr 0x%x, data 0x%x\n", MC6_REG_MRS_TIMING,
	       reg_read(MC6_REG_MRS_TIMING)); */

	reg_bit_clrset(MC6_REG_ACT_TIMING,
		       mc6_timing.t_ras << MC6_TRAS_OFFS |
		       mc6_timing.t_rcd << MC6_TRCD_OFFS |
		       mc6_timing.t_rc << MC6_TRC_OFFS |
		       mc6_timing.t_faw << MC6_TFAW_OFFS,
		       MC6_TRAS_MASK << MC6_TRAS_OFFS |
		       MC6_TRCD_MASK << MC6_TRCD_OFFS |
		       MC6_TRC_MASK << MC6_TRC_OFFS |
		       MC6_TFAW_MASK << MC6_TFAW_OFFS);
	/* printf("MC6_REG_ACT_TIMING addr 0x%x, data 0x%x\n", MC6_REG_ACT_TIMING,
	       reg_read(MC6_REG_ACT_TIMING)); */

	reg_bit_clrset(MC6_REG_PRE_CHARGE_TIMING,
		       mc6_timing.t_rp << MC6_TRP_OFFS |
		       mc6_timing.t_rtp << MC6_TRTP_OFFS |
		       mc6_timing.t_wr << MC6_TWR_OFFS |
		       mc6_timing.t_rp << MC6_TRPA_OFFS,
		       MC6_TRP_MASK << MC6_TRP_OFFS |
		       MC6_TRTP_MASK << MC6_TRTP_OFFS |
		       MC6_TWR_MASK << MC6_TWR_OFFS |
		       MC6_TRPA_MASK << MC6_TRPA_OFFS);
	/* printf("MC6_REG_PRE_CHARGE_TIMING addr 0x%x, data 0x%x\n", MC6_REG_PRE_CHARGE_TIMING,
	       reg_read(MC6_REG_PRE_CHARGE_TIMING)); */

	reg_bit_clrset(MC6_REG_CAS_RAS_TIMING_0,
		       mc6_timing.t_wtr << MC6_TWTR_S_OFFS |
		       mc6_timing.t_wtr_l << MC6_TWTR_OFFS |
		       mc6_timing.t_ccd << MC6_TCCD_S_OFFS |
		       mc6_timing.t_ccd_l << MC6_TCCD_OFFS,
		       MC6_TWTR_S_MASK << MC6_TWTR_S_OFFS |
		       MC6_TWTR_MASK << MC6_TWTR_OFFS |
		       MC6_TCCD_S_MASK << MC6_TCCD_S_OFFS |
		       MC6_TCCD_MASK << MC6_TCCD_OFFS);
	/* printf("MC6_REG_CAS_RAS_TIMING_0 addr 0x%x, data 0x%x\n", MC6_REG_CAS_RAS_TIMING_0,
	       reg_read(MC6_REG_CAS_RAS_TIMING_0)); */

	/* TODO: check why change default of 17:16 tDQS2DQ from '1' to '0' */
	reg_bit_clrset(MC6_REG_CAS_RAS_TIMING_1,
		       mc6_timing.t_rrd << MC6_TRRD_S_OFFS |
		       mc6_timing.t_rrd_l << MC6_TRRD_OFFS |
		       TDQS2DQ << MC6_TDQS2DQ_OFFS,
		       MC6_TRRD_S_MASK << MC6_TRRD_S_OFFS |
		       MC6_TRRD_MASK << MC6_TRRD_OFFS |
		       MC6_TDQS2DQ_MASK << MC6_TDQS2DQ_OFFS);
	/* printf("MC6_REG_CAS_RAS_TIMING_1 addr 0x%x, data 0x%x\n", MC6_REG_CAS_RAS_TIMING_1,
	       reg_read(MC6_REG_CAS_RAS_TIMING_1)); */

	reg_bit_clrset(MC6_REG_OFF_SPEC_TIMING_0,
		       mc6_timing.t_ccd_ccs_ext_dly << MC6_TCCD_CCS_EXT_DLY_OFFS |
		       mc6_timing.t_ccd_ccs_wr_ext_dly << MC6_TCCD_CCS_WR_EXT_DLY_OFFS |
		       mc6_timing.t_rwd_ext_dly << MC6_TRWD_EXT_DLY_OFFS |
		       mc6_timing.t_wl_early << MC6_TWL_EARLY_OFFS,
		       MC6_TCCD_CCS_EXT_DLY_MASK << MC6_TCCD_CCS_EXT_DLY_OFFS |
		       MC6_TCCD_CCS_WR_EXT_DLY_MASK << MC6_TCCD_CCS_WR_EXT_DLY_OFFS |
		       MC6_TRWD_EXT_DLY_MASK << MC6_TRWD_EXT_DLY_OFFS |
		       MC6_TWL_EARLY_MASK << MC6_TWL_EARLY_OFFS);
	/* printf("MC6_REG_OFF_SPEC_TIMING_0 addr 0x%x, data 0x%x\n", MC6_REG_OFF_SPEC_TIMING_0,
	       reg_read(MC6_REG_OFF_SPEC_TIMING_0)); */

	reg_bit_clrset(MC6_REG_OFF_SPEC_TIMING_1,
		       mc6_timing.read_gap_extend << MC6_READ_GAP_EXTEND_OFFS |
		       mc6_timing.t_ccd_ccs_ext_dly << MC6_TCCD_CCS_EXT_DLY_MIN_OFFS |
		       mc6_timing.t_ccd_ccs_wr_ext_dly << MC6_TCCD_CCS_WR_EXT_DLY_MIN_OFFS,
		       MC6_READ_GAP_EXTEND_MASK << MC6_READ_GAP_EXTEND_OFFS |
		       MC6_TCCD_CCS_EXT_DLY_MIN_MASK << MC6_TCCD_CCS_EXT_DLY_MIN_OFFS |
		       MC6_TCCD_CCS_WR_EXT_DLY_MIN_MASK << MC6_TCCD_CCS_WR_EXT_DLY_MIN_OFFS);
	/* printf("MC6_REG_OFF_SPEC_TIMING_1 addr 0x%x, data 0x%x\n", MC6_REG_OFF_SPEC_TIMING_1,
	       reg_read(MC6_REG_OFF_SPEC_TIMING_1)); */

	/* TODO: check why change default of 3:0 tDQSCK from '3' to '0' */
	reg_bit_clrset(MC6_REG_DRAM_READ_TIMING,
		       TDQSCK << MC6_TDQSCK_OFFS,
		       MC6_TDQSCK_MASK << MC6_TDQSCK_OFFS);
	/* printf("MC6_REG_DRAM_READ_TIMING addr 0x%x, data 0x%x\n", MC6_REG_DRAM_READ_TIMING,
	       reg_read(MC6_REG_DRAM_READ_TIMING)); */

	reg_bit_clrset(MC6_REG_MPD_TIMING,
		       mc6_timing.t_xmp << MC6_TXMP_OFFS |
		       mc6_timing.t_mpx_s << MC6_TMPX_S_OFFS |
		       mc6_timing.t_mpx_lh << MC6_TMPX_LH_OFFS,
		       MC6_TXMP_MASK << MC6_TXMP_OFFS |
		       MC6_TMPX_S_MASK << MC6_TMPX_S_OFFS |
		       MC6_TMPX_LH_MASK << MC6_TMPX_LH_OFFS);
	/* printf("MC6_REG_MPD_TIMING addr 0x%x, data 0x%x\n", MC6_REG_MPD_TIMING,
	       reg_read(MC6_REG_MPD_TIMING)); */

	reg_bit_clrset(MC6_REG_PDA_TIMING,
		       mc6_timing.t_mrd_pda << MC6_TMRD_PDA_OFFS,
		       MC6_TMRD_PDA_MASK << MC6_TMRD_PDA_OFFS);
	/* printf("MC6_REG_PDA_TIMING addr 0x%x, data 0x%x\n", MC6_REG_PDA_TIMING,
	       reg_read(MC6_REG_PDA_TIMING)); */
}

void mv_ddr_mc6_and_dram_timing_set(void)
{
	/* get the frequency */
	u32 freq_mhz;
	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();

	/* get the frequency form the topology */
	freq_mhz = freq_val[tm->interface_params[IF_ID_0].memory_freq];

	mv_ddr_mc6_timing_regs_cfg(freq_mhz);
}


struct mv_ddr_addressing_table mv_ddr_addresing_table_get(enum mv_ddr_die_capacity memory_size,
						   enum mv_ddr_dev_width bus_width)
{
	struct mv_ddr_addressing_table addr_table;
	switch (memory_size) {
	case MV_DDR_DIE_CAP_2GBIT:
	{
		switch (bus_width) {
		case MV_DDR_DEV_WIDTH_8BIT:
			addr_table.num_of_bank_groups = 4;
			addr_table.num_of_bank_addr_in_bank_group = 4;
			addr_table.row_addr = 14;
			addr_table.column_addr = 10;
			addr_table.page_size_k_byte = 1;
			break;

		case MV_DDR_DEV_WIDTH_16BIT:
			addr_table.num_of_bank_groups = 2;
			addr_table.num_of_bank_addr_in_bank_group = 4;
			addr_table.row_addr = 14;
			addr_table.column_addr = 10;
			addr_table.page_size_k_byte = 2;
			break;

		default:
			printf("%s: the current bus width in not supported\n", __func__);
		}
	}
		break;

	case MV_DDR_DIE_CAP_4GBIT:
	{
		switch (bus_width) {
		case MV_DDR_DEV_WIDTH_8BIT:
			addr_table.num_of_bank_groups = 4;
			addr_table.num_of_bank_addr_in_bank_group = 4;
			addr_table.row_addr = 15;
			addr_table.column_addr = 10;
			addr_table.page_size_k_byte = 1;
			break;

		case MV_DDR_DEV_WIDTH_16BIT:
			addr_table.num_of_bank_groups = 2;
			addr_table.num_of_bank_addr_in_bank_group = 4;
			addr_table.row_addr = 15;
			addr_table.column_addr = 10;
			addr_table.page_size_k_byte = 2;
			break;

		default:
			printf("%s: the current bus width in not supported\n", __func__);
		}
	}
		break;

	case MV_DDR_DIE_CAP_8GBIT:
	{
		switch (bus_width) {
		case MV_DDR_DEV_WIDTH_8BIT:
			addr_table.num_of_bank_groups = 4;
			addr_table.num_of_bank_addr_in_bank_group = 4;
			addr_table.row_addr = 16;
			addr_table.column_addr = 10;
			addr_table.page_size_k_byte = 1;
			break;

		case MV_DDR_DEV_WIDTH_16BIT:
			addr_table.num_of_bank_groups = 2;
			addr_table.num_of_bank_addr_in_bank_group = 4;
			addr_table.row_addr = 16;
			addr_table.column_addr = 10;
			addr_table.page_size_k_byte = 2;
			break;

		default:
			printf("%s: the current bus width in not supported\n", __func__);
		}
	}
		break;

	case MV_DDR_DIE_CAP_16GBIT:
	{
		switch (bus_width) {
		case MV_DDR_DEV_WIDTH_8BIT:
			addr_table.num_of_bank_groups = 4;
			addr_table.num_of_bank_addr_in_bank_group = 4;
			addr_table.row_addr = 17;
			addr_table.column_addr = 10;
			addr_table.page_size_k_byte = 1;
			break;

		case MV_DDR_DEV_WIDTH_16BIT:
			addr_table.num_of_bank_groups = 2;
			addr_table.num_of_bank_addr_in_bank_group = 4;
			addr_table.row_addr = 17;
			addr_table.column_addr = 10;
			addr_table.page_size_k_byte = 2;
			break;

		default:
			printf("%s: the current bus width in not supported\n", __func__);
		}
	}
		break;

	default:
		printf("%s: the current memory size in not supported\n", __func__);
	}

	return addr_table;
}

unsigned int mv_ddr_area_length_convert(unsigned int area_length)
{
	unsigned int area_length_map = 0xffffffff;

	switch (area_length) {
	case 384:
		area_length_map = 0x0;
		break;
	case 768:
		area_length_map = 0x1;
		break;
	case 1536:
		area_length_map = 0x2;
		break;
	case 3072:
		area_length_map = 0x3;
		break;
	case 8:
		area_length_map = 0x7;
		break;
	case 16:
		area_length_map = 0x8;
		break;
	case 32:
		area_length_map = 0x9;
		break;
	case 64:
		area_length_map = 0xA;
		break;
	case 128:
		area_length_map = 0xB;
		break;
	case 256:
		area_length_map = 0xC;
		break;
	case 512:
		area_length_map = 0xD;
		break;
	case 1024:
		area_length_map = 0xE;
		break;
	case 2048:
		area_length_map = 0xF;
		break;
	case 4096:
		area_length_map = 0x10;
		break;
	case 8192:
		area_length_map = 0x11;
		break;
	case 16384:
		area_length_map = 0x12;
		break;
	case 32768:
		area_length_map = 0x13;
		break;
	default:
		/* over than 32GB is not supported */
		printf("%s: unsupported area length %d\n", __func__, area_length);
	}

	return area_length_map;
}

unsigned int mv_ddr_bank_addr_convert(unsigned int num_of_bank_addr_in_bank_group)
{
	unsigned int num_of_bank_addr_in_bank_group_map = 0xff;

	switch (num_of_bank_addr_in_bank_group) {
	case 2:
		num_of_bank_addr_in_bank_group_map = 0x0;
		break;
	case 4:
		num_of_bank_addr_in_bank_group_map = 0x1;
		break;
	case 8:
		num_of_bank_addr_in_bank_group_map = 0x2;
		break;
	default:
		printf("%s: number of bank address in bank group %d is not supported\n", __func__,
		       num_of_bank_addr_in_bank_group);
	}

	return num_of_bank_addr_in_bank_group_map;
}

unsigned int mv_ddr_bank_groups_convert(unsigned int num_of_bank_groups)
{
	unsigned int num_of_bank_groups_map = 0xff;

	switch (num_of_bank_groups) {
	case 1:
		num_of_bank_groups_map = 0x0;
		break;
	case 2:
		num_of_bank_groups_map = 0x1;
		break;
	case 4:
		num_of_bank_groups_map = 0x2;
		break;
	default:
		printf("%s: number of bank group %d is not supported\n", __func__,
		       num_of_bank_groups);
	}

	return num_of_bank_groups_map;
}

unsigned int mv_ddr_column_num_convert(unsigned int column_addr)
{
	unsigned int column_addr_map = 0xff;

	switch (column_addr) {
	case 8:
		column_addr_map = 0x1;
		break;
	case 9:
		column_addr_map = 0x2;
		break;
	case 10:
		column_addr_map = 0x3;
		break;
	case 11:
		column_addr_map = 0x4;
		break;
	case 12:
		column_addr_map = 0x5;
		break;
	default:
		printf("%s: number of columns %d is not supported\n", __func__,
		       column_addr);
	}

	return column_addr_map;
}

unsigned int mv_ddr_row_num_convert(unsigned int row_addr)
{
	unsigned int row_addr_map = 0xff;

	switch (row_addr) {
	case 11:
		row_addr_map = 0x1;
		break;
	case 12:
		row_addr_map = 0x2;
		break;
	case 13:
		row_addr_map = 0x3;
		break;
	case 14:
		row_addr_map = 0x4;
		break;
	case 15:
		row_addr_map = 0x5;
		break;
	case 16:
		row_addr_map = 0x6;
		break;
	default:
		printf("%s: number of rows %d is not supported\n", __func__,
		       row_addr);
	}

	return row_addr_map;
}

unsigned int mv_ddr_stack_addr_num_convert(unsigned int stack_addr)
{
	unsigned int stack_addr_map = 0xff;

	switch (stack_addr) {
	case 1:
		stack_addr_map = 0x0;
		break;
	case 2:
		stack_addr_map = 0x1;
		break;
	case 4:
		stack_addr_map = 0x2;
		break;
	case 8:
		stack_addr_map = 0x3;
		break;
	default:
		printf("%s: number of stacks %d is not supported\n", __func__,
		       stack_addr);
	}

	return stack_addr_map;
}

unsigned int mv_ddr_device_type_convert(enum mv_ddr_dev_width bus_width)
{
	unsigned int device_type_map = 0xff;

	switch (bus_width) {
	case MV_DDR_DEV_WIDTH_8BIT:
		device_type_map = 0x1;
		break;
	case MV_DDR_DEV_WIDTH_16BIT:
		device_type_map = 0x2;
		break;
	default:
		printf("%s: device type is not supported\n", __func__);
	}

	return device_type_map;
}

unsigned int mv_ddr_bank_map_convert(enum mv_ddr_mc6_bank_boundary mc6_bank_boundary)
{
	unsigned int mv_ddr_mc6_bank_boundary_map = 0xff;

	switch (mc6_bank_boundary) {
	case BANK_MAP_512B:
		mv_ddr_mc6_bank_boundary_map = 0x2;
		break;
	case BANK_MAP_1KB:
		mv_ddr_mc6_bank_boundary_map = 0x3;
		break;
	case BANK_MAP_2KB:
		mv_ddr_mc6_bank_boundary_map = 0x4;
		break;
	case BANK_MAP_4KB:
		mv_ddr_mc6_bank_boundary_map = 0x5;
		break;
	case BANK_MAP_8KB:
		mv_ddr_mc6_bank_boundary_map = 0x6;
		break;
	case BANK_MAP_16KB:
		mv_ddr_mc6_bank_boundary_map = 0x7;
		break;
	case BANK_MAP_32KB:
		mv_ddr_mc6_bank_boundary_map = 0x8;
		break;
	case BANK_MAP_64KB:
		mv_ddr_mc6_bank_boundary_map = 0x9;
		break;
	case BANK_MAP_128KB:
		mv_ddr_mc6_bank_boundary_map = 0xa;
		break;
	case BANK_MAP_256KB:
		mv_ddr_mc6_bank_boundary_map = 0xb;
		break;
	case BANK_MAP_512KB:
		mv_ddr_mc6_bank_boundary_map = 0xc;
		break;
	case BANK_MAP_1MB:
		mv_ddr_mc6_bank_boundary_map = 0xd;
		break;
	case BANK_MAP_2MB:
		mv_ddr_mc6_bank_boundary_map = 0xe;
		break;
	case BANK_MAP_4MB:
		mv_ddr_mc6_bank_boundary_map = 0xf;
		break;
	case BANK_MAP_8MB:
		mv_ddr_mc6_bank_boundary_map = 0x10;
		break;
	case BANK_MAP_16MB:
		mv_ddr_mc6_bank_boundary_map = 0x11;
		break;
	case BANK_MAP_32MB:
		mv_ddr_mc6_bank_boundary_map = 0x12;
		break;
	case BANK_MAP_64MB:
		mv_ddr_mc6_bank_boundary_map = 0x13;
		break;
	case BANK_MAP_128MB:
		mv_ddr_mc6_bank_boundary_map = 0x14;
		break;
	case BANK_MAP_256MB:
		mv_ddr_mc6_bank_boundary_map = 0x15;
		break;
	case BANK_MAP_512MB:
		mv_ddr_mc6_bank_boundary_map = 0x16;
		break;
	case BANK_MAP_1GB:
		mv_ddr_mc6_bank_boundary_map = 0x17;
		break;
	case BANK_MAP_2GB:
		mv_ddr_mc6_bank_boundary_map = 0x18;
		break;
	case BANK_MAP_4GB:
		mv_ddr_mc6_bank_boundary_map = 0x19;
		break;
	case BANK_MAP_8GB:
		mv_ddr_mc6_bank_boundary_map = 0x1a;
		break;
	case BANK_MAP_16GB:
		mv_ddr_mc6_bank_boundary_map = 0x1b;
		break;
	case BANK_MAP_32GB:
		mv_ddr_mc6_bank_boundary_map = 0x1c;
		break;
	case BANK_MAP_64GB:
		mv_ddr_mc6_bank_boundary_map = 0x1d;
		break;
	default:
		printf("%s: bank_boundary is not supported\n", __func__);
	}

	return mv_ddr_mc6_bank_boundary_map;
}

void mv_ddr_mc6_sizes_cfg(void)
{
	unsigned int cs_idx;
	unsigned int cs_num;
	unsigned int reserved_mem_idx;
	unsigned long long area_length_bits;
	unsigned int are_length_mega_bytes;
	unsigned long long start_addr_bytes;
	unsigned int start_addr_low, start_addr_high;

	struct mv_ddr_topology_map *tm = mv_ddr_topology_map_get();
	calc_cs_num(DEV_NUM_0, IF_ID_0, &cs_num);

	area_length_bits = mv_ddr_get_memory_size_per_cs_in_bits();
	are_length_mega_bytes = area_length_bits / (MV_DDR_MEGA_BITS * MV_DDR_NUM_BITS_IN_BYTE);

	struct mv_ddr_addressing_table addr_tbl = mv_ddr_addresing_table_get(tm->interface_params[IF_ID_0].memory_size,
									     tm->interface_params[IF_ID_0].bus_width);
	/* configure all length per cs here and activate the cs */
	for (cs_idx = 0; cs_idx < cs_num; cs_idx++) {
		start_addr_bytes = area_length_bits / MV_DDR_NUM_BITS_IN_BYTE * cs_idx;
		start_addr_low = start_addr_bytes & MV_DDR_32_BITS_MASK;
		start_addr_high = (start_addr_bytes >> START_ADDR_HIGH_TO_LOW_OFFS) & MV_DDR_32_BITS_MASK;

		reg_bit_clrset(MC6_REG_MMAP_LOW_CH0(cs_idx),
			       ACTIVATE_CS << MC6_CS_VALID_OFFS |
			       NON_INTERLEAVE << MC6_INTERLEAVE_OFFS |
			       mv_ddr_area_length_convert(are_length_mega_bytes) << MC6_AREA_LENGTH_OFFS |
			       start_addr_low,
			       MC6_CS_VALID_MASK << MC6_CS_VALID_OFFS |
			       MC6_INTERLEAVE_MASK << MC6_INTERLEAVE_OFFS |
			       MC6_AREA_LENGTH_MASK << MC6_AREA_LENGTH_OFFS |
			       MC6_START_ADDRESS_L_MASK << MC6_START_ADDRESS_L_OFFS);
		/* printf("MC6_REG_MMAP_LOW_CH0(cs_idx) addr 0x%x, data 0x%x\n", MC6_REG_MMAP_LOW_CH0(cs_idx),
			  reg_read(MC6_REG_MMAP_LOW_CH0(cs_idx))); */

		reg_bit_clrset(MC6_REG_MMAP_HIGH_CH0(cs_idx),
			       start_addr_high << MC6_START_ADDRESS_H_OFFS,
			       MC6_START_ADDRESS_H_MASK << MC6_START_ADDRESS_H_OFFS);
		/* printf("MC6_REG_MMAP_HIGH_CH0(cs_idx) addr 0x%x, data 0x%x\n", MC6_REG_MMAP_HIGH_CH0(cs_idx),
			  reg_read(MC6_REG_MMAP_HIGH_CH0(cs_idx))); */

		reg_bit_clrset(MC6_REG_MC_CONFIG(cs_idx),
			       mv_ddr_bank_addr_convert(addr_tbl.num_of_bank_addr_in_bank_group) <<
			       MC6_BA_NUM_OFFS |
			       mv_ddr_bank_groups_convert(addr_tbl.num_of_bank_groups) <<
			       MC6_BG_NUM_OFFS |
			       mv_ddr_column_num_convert(addr_tbl.column_addr) <<
			       MC6_CA_NUM_OFFS |
			       mv_ddr_row_num_convert(addr_tbl.row_addr) <<
			       MC6_RA_NUM_OFFS |
			       mv_ddr_stack_addr_num_convert(SINGLE_STACK) <<
			       MC6_SA_NUM_OFFS |
			       mv_ddr_device_type_convert(tm->interface_params[IF_ID_0].bus_width) <<
			       MC6_DEVICE_TYPE_OFFS |
			       mv_ddr_bank_map_convert(BANK_MAP_4KB) <<
			       MC6_BANK_MAP_OFFS,
			       MC6_BA_NUM_MASK << MC6_BA_NUM_OFFS |
			       MC6_BG_NUM_MASK << MC6_BG_NUM_OFFS |
			       MC6_CA_NUM_MASK << MC6_CA_NUM_OFFS |
			       MC6_RA_NUM_MASK << MC6_RA_NUM_OFFS |
			       MC6_SA_NUM_MASK << MC6_SA_NUM_OFFS |
			       MC6_DEVICE_TYPE_MASK << MC6_DEVICE_TYPE_OFFS |
			       MC6_BANK_MAP_MASK << MC6_BANK_MAP_OFFS);
	}

	/* configure here the channel 1 reg_map_low and reg_map_high to unused memory area due to mc6 bug */
	for (cs_idx = 0, reserved_mem_idx = cs_num; cs_idx < cs_num; cs_idx++, reserved_mem_idx++) {
		start_addr_bytes = area_length_bits / MV_DDR_NUM_BITS_IN_BYTE * reserved_mem_idx;
		start_addr_low = start_addr_bytes & MV_DDR_32_BITS_MASK;
		start_addr_high = (start_addr_bytes >> START_ADDR_HIGH_TO_LOW_OFFS) & MV_DDR_32_BITS_MASK;

		reg_bit_clrset(MC6_REG_MMAP_LOW_CH1(cs_idx),
			       ACTIVATE_CS << MC6_CS_VALID_OFFS |
			       NON_INTERLEAVE << MC6_INTERLEAVE_OFFS |
			       mv_ddr_area_length_convert(are_length_mega_bytes) << MC6_AREA_LENGTH_OFFS |
			       start_addr_low,
			       MC6_CS_VALID_MASK << MC6_CS_VALID_OFFS |
			       MC6_INTERLEAVE_MASK << MC6_INTERLEAVE_OFFS |
			       MC6_AREA_LENGTH_MASK << MC6_AREA_LENGTH_OFFS |
			       MC6_START_ADDRESS_L_MASK << MC6_START_ADDRESS_L_OFFS);
		/* printf("MC6_REG_MMAP_LOW_CH1(cs_idx) addr 0x%x, data 0x%x\n", MC6_REG_MMAP_LOW_CH1(cs_idx),
			  reg_read(MC6_REG_MMAP_LOW_CH1(cs_idx))); */

		reg_bit_clrset(MC6_REG_MMAP_HIGH_CH1(cs_idx),
			       start_addr_high << MC6_START_ADDRESS_H_OFFS,
			       MC6_START_ADDRESS_H_MASK << MC6_START_ADDRESS_H_OFFS);
		/* printf("MC6_REG_MMAP_HIGH_CH1(cs_idx) addr 0x%x, data 0x%x\n", MC6_REG_MMAP_HIGH_CH1(cs_idx),
			  reg_read(MC6_REG_MMAP_HIGH_CH1(cs_idx))); */
	}
}

void  mv_ddr_mc6_ecc_enable(void)
{
	reg_bit_clrset(MC6_REG_RAS_CTRL,
		       ECC_ENABLE << MC6_ECC_ENABLE_OFFS,
		       MC6_ECC_ENABLE_MASK << MC6_ECC_ENABLE_OFFS);
}

