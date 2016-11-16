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

#ifndef _DDR3_TRAINING_IP_DB_H_
#define _DDR3_TRAINING_IP_DB_H_

enum hws_pattern {
#if defined(CONFIG_64BIT) /* DDR3/4 64-bit */
	PATTERN_PBS1,
	PATTERN_PBS2,
	PATTERN_PBS3,
	PATTERN_TEST,
	PATTERN_RL,
	PATTERN_RL2,
	PATTERN_STATIC_PBS,
	PATTERN_KILLER_DQ0,
	PATTERN_KILLER_DQ1,
	PATTERN_KILLER_DQ2,
	PATTERN_KILLER_DQ3,
	PATTERN_KILLER_DQ4,
	PATTERN_KILLER_DQ5,
	PATTERN_KILLER_DQ6,
	PATTERN_KILLER_DQ7,
	PATTERN_KILLER_DQ0_64,
	PATTERN_KILLER_DQ1_64,
	PATTERN_KILLER_DQ2_64,
	PATTERN_KILLER_DQ3_64,
	PATTERN_KILLER_DQ4_64,
	PATTERN_KILLER_DQ5_64,
	PATTERN_KILLER_DQ6_64,
	PATTERN_KILLER_DQ7_64,
	PATTERN_KILLER_DQ0_INV,
	PATTERN_KILLER_DQ1_INV,
	PATTERN_KILLER_DQ2_INV,
	PATTERN_KILLER_DQ3_INV,
	PATTERN_KILLER_DQ4_INV,
	PATTERN_KILLER_DQ5_INV,
	PATTERN_KILLER_DQ6_INV,
	PATTERN_KILLER_DQ7_INV,
	PATTERN_KILLER_DQ0_INV_64,
	PATTERN_KILLER_DQ1_INV_64,
	PATTERN_KILLER_DQ2_INV_64,
	PATTERN_KILLER_DQ3_INV_64,
	PATTERN_KILLER_DQ4_INV_64,
	PATTERN_KILLER_DQ5_INV_64,
	PATTERN_KILLER_DQ6_INV_64,
	PATTERN_KILLER_DQ7_INV_64,
	PATTERN_SSO_FULL_XTALK_DQ0,
	PATTERN_SSO_FULL_XTALK_DQ1,
	PATTERN_SSO_FULL_XTALK_DQ2,
	PATTERN_SSO_FULL_XTALK_DQ3,
	PATTERN_SSO_FULL_XTALK_DQ4,
	PATTERN_SSO_FULL_XTALK_DQ5,
	PATTERN_SSO_FULL_XTALK_DQ6,
	PATTERN_SSO_FULL_XTALK_DQ7,
	PATTERN_SSO_FULL_XTALK_DQ0_64,
	PATTERN_SSO_FULL_XTALK_DQ1_64,
	PATTERN_SSO_FULL_XTALK_DQ2_64,
	PATTERN_SSO_FULL_XTALK_DQ3_64,
	PATTERN_SSO_FULL_XTALK_DQ4_64,
	PATTERN_SSO_FULL_XTALK_DQ5_64,
	PATTERN_SSO_FULL_XTALK_DQ6_64,
	PATTERN_SSO_FULL_XTALK_DQ7_64,
	PATTERN_SSO_XTALK_FREE_DQ0,
	PATTERN_SSO_XTALK_FREE_DQ1,
	PATTERN_SSO_XTALK_FREE_DQ2,
	PATTERN_SSO_XTALK_FREE_DQ3,
	PATTERN_SSO_XTALK_FREE_DQ4,
	PATTERN_SSO_XTALK_FREE_DQ5,
	PATTERN_SSO_XTALK_FREE_DQ6,
	PATTERN_SSO_XTALK_FREE_DQ7,
	PATTERN_SSO_XTALK_FREE_DQ0_64,
	PATTERN_SSO_XTALK_FREE_DQ1_64,
	PATTERN_SSO_XTALK_FREE_DQ2_64,
	PATTERN_SSO_XTALK_FREE_DQ3_64,
	PATTERN_SSO_XTALK_FREE_DQ4_64,
	PATTERN_SSO_XTALK_FREE_DQ5_64,
	PATTERN_SSO_XTALK_FREE_DQ6_64,
	PATTERN_SSO_XTALK_FREE_DQ7_64,
	PATTERN_ISI_XTALK_FREE,
	PATTERN_ISI_XTALK_FREE_64,
	PATTERN_VREF,
	PATTERN_VREF_64,
	PATTERN_VREF_INV,
	PATTERN_FULL_SSO0,
	PATTERN_FULL_SSO1,
	PATTERN_FULL_SSO2,
	PATTERN_FULL_SSO3,
	PATTERN_RESONANCE_1T,
	PATTERN_RESONANCE_2T,
	PATTERN_RESONANCE_3T,
	PATTERN_RESONANCE_4T,
	PATTERN_RESONANCE_5T,
	PATTERN_RESONANCE_6T,
	PATTERN_RESONANCE_7T,
	PATTERN_RESONANCE_8T,
	PATTERN_RESONANCE_9T,
#elif defined(CONFIG_DDR4) /* DDR4 16/32-bit */
	PATTERN_PBS1,/*0*/
	PATTERN_PBS2,
	PATTERN_PBS3,
	PATTERN_TEST,
	PATTERN_RL,
	PATTERN_RL2,
	PATTERN_STATIC_PBS,
	PATTERN_KILLER_DQ0,
	PATTERN_KILLER_DQ1,
	PATTERN_KILLER_DQ2,
	PATTERN_KILLER_DQ3,/*10*/
	PATTERN_KILLER_DQ4,
	PATTERN_KILLER_DQ5,
	PATTERN_KILLER_DQ6,
	PATTERN_KILLER_DQ7,
	PATTERN_KILLER_DQ0_INV,
	PATTERN_KILLER_DQ1_INV,
	PATTERN_KILLER_DQ2_INV,
	PATTERN_KILLER_DQ3_INV,
	PATTERN_KILLER_DQ4_INV,
	PATTERN_KILLER_DQ5_INV,/*20*/
	PATTERN_KILLER_DQ6_INV,
	PATTERN_KILLER_DQ7_INV,
	PATTERN_VREF,
	PATTERN_VREF_INV,
	PATTERN_FULL_SSO0,
	PATTERN_FULL_SSO1,
	PATTERN_FULL_SSO2,
	PATTERN_FULL_SSO3,
	PATTERN_SSO_FULL_XTALK_DQ0,
	PATTERN_SSO_FULL_XTALK_DQ1,/*30*/
	PATTERN_SSO_FULL_XTALK_DQ2,
	PATTERN_SSO_FULL_XTALK_DQ3,
	PATTERN_SSO_FULL_XTALK_DQ4,
	PATTERN_SSO_FULL_XTALK_DQ5,
	PATTERN_SSO_FULL_XTALK_DQ6,
	PATTERN_SSO_FULL_XTALK_DQ7,
	PATTERN_SSO_XTALK_FREE_DQ0,
	PATTERN_SSO_XTALK_FREE_DQ1,
	PATTERN_SSO_XTALK_FREE_DQ2,
	PATTERN_SSO_XTALK_FREE_DQ3,/*40*/
	PATTERN_SSO_XTALK_FREE_DQ4,
	PATTERN_SSO_XTALK_FREE_DQ5,
	PATTERN_SSO_XTALK_FREE_DQ6,
	PATTERN_SSO_XTALK_FREE_DQ7,
	PATTERN_ISI_XTALK_FREE,
	PATTERN_RESONANCE_1T,
	PATTERN_RESONANCE_2T,
	PATTERN_RESONANCE_3T,
	PATTERN_RESONANCE_4T,
	PATTERN_RESONANCE_5T,/*50*/
	PATTERN_RESONANCE_6T,
	PATTERN_RESONANCE_7T,
	PATTERN_RESONANCE_8T,
	PATTERN_RESONANCE_9T,
#else /* DDR3 16/32-bit */
	PATTERN_PBS1,
	PATTERN_PBS2,
	PATTERN_PBS3,
	PATTERN_TEST,
	PATTERN_RL,
	PATTERN_RL2,
	PATTERN_STATIC_PBS,
	PATTERN_KILLER_DQ0,
	PATTERN_KILLER_DQ1,
	PATTERN_KILLER_DQ2,
	PATTERN_KILLER_DQ3,
	PATTERN_KILLER_DQ4,
	PATTERN_KILLER_DQ5,
	PATTERN_KILLER_DQ6,
	PATTERN_KILLER_DQ7,
	PATTERN_VREF,
	PATTERN_FULL_SSO0,
	PATTERN_FULL_SSO1,
	PATTERN_FULL_SSO2,
	PATTERN_FULL_SSO3,
	PATTERN_SSO_FULL_XTALK_DQ0,
	PATTERN_SSO_FULL_XTALK_DQ1,
	PATTERN_SSO_FULL_XTALK_DQ2,
	PATTERN_SSO_FULL_XTALK_DQ3,
	PATTERN_SSO_FULL_XTALK_DQ4,
	PATTERN_SSO_FULL_XTALK_DQ5,
	PATTERN_SSO_FULL_XTALK_DQ6,
	PATTERN_SSO_FULL_XTALK_DQ7,
	PATTERN_SSO_XTALK_FREE_DQ0,
	PATTERN_SSO_XTALK_FREE_DQ1,
	PATTERN_SSO_XTALK_FREE_DQ2,
	PATTERN_SSO_XTALK_FREE_DQ3,
	PATTERN_SSO_XTALK_FREE_DQ4,
	PATTERN_SSO_XTALK_FREE_DQ5,
	PATTERN_SSO_XTALK_FREE_DQ6,
	PATTERN_SSO_XTALK_FREE_DQ7,
	PATTERN_ISI_XTALK_FREE,
#endif /* CONFIG_64BIT */
	PATTERN_LAST
};

enum mv_wl_supp_mode {
	WRITE_LEVELING_SUPP_REG_MODE,
	WRITE_LEVELING_SUPP_ECC_MODE_DATA_PUPS,
	WRITE_LEVELING_SUPP_ECC_MODE_ECC_PUP4,
	WRITE_LEVELING_SUPP_ECC_MODE_ECC_PUP3,
	WRITE_LEVELING_SUPP_ECC_MODE_ECC_PUP8
};

enum mv_ddr_dev_attribute {
	MV_ATTR_TIP_REV,
	MV_ATTR_PHY_EDGE,
	MV_ATTR_OCTET_PER_INTERFACE,
	MV_ATTR_PLL_BEFORE_INIT,
	MV_ATTR_TUNE_MASK,
	MV_ATTR_INIT_FREQ,
	MV_ATTR_MID_FREQ,
	MV_ATTR_DFS_LOW_FREQ,
	MV_ATTR_DFS_LOW_PHY,
	MV_ATTR_DELAY_ENABLE,
	MV_ATTR_CK_DELAY,
	MV_ATTR_CA_DELAY,
	MV_ATTR_INTERLEAVE_WA,
	MV_ATTR_LAST
};

enum mv_ddr_tip_revison {
	MV_TIP_REV_NA,
	MV_TIP_REV_1, /* NP5 */
	MV_TIP_REV_2, /* BC2 */
	MV_TIP_REV_3, /* AC3 */
	MV_TIP_REV_4, /* A-380/A-390 */
	MV_TIP_REV_LAST
};

enum mv_ddr_phy_edge {
	MV_DDR_PHY_EDGE_POSITIVE,
	MV_DDR_PHY_EDGE_NEGATIVE
};

/* Device attribute functions */
void ddr3_tip_dev_attr_init(u32 dev_num);
u32 ddr3_tip_dev_attr_get(u32 dev_num, enum mv_ddr_dev_attribute attr_id);
void ddr3_tip_dev_attr_set(u32 dev_num, enum mv_ddr_dev_attribute attr_id, u32 value);

#endif /* _DDR3_TRAINING_IP_DB_H_ */
