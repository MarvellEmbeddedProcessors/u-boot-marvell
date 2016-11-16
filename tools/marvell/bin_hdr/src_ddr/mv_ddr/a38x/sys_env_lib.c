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

#ifdef CONFIG_ARMADA_38X
enum unit_id sys_env_soc_unit_nums[MAX_UNITS_ID][MAX_DEV_ID_NUM] = {
/*                     6820    6810     6811     6828     */
/* PEX_UNIT_ID      */ { 4,     3,       3,       4},
/* ETH_GIG_UNIT_ID  */ { 3,	2,       3,       3},
/* USB3H_UNIT_ID    */ { 2,     2,       2,       2},
/* USB3D_UNIT_ID    */ { 1,     1,       1,       1},
/* SATA_UNIT_ID     */ { 2,     2,       2,       4},
/* QSGMII_UNIT_ID   */ { 1,     0,       0,       1},
/* XAUI_UNIT_ID     */ { 0,     0,       0,       0},
/* RXAUI_UNIT_ID    */ { 0,     0,       0,       0}
};
#else  /* if (CONFIG_ARMADA_39X) */
enum unit_id sys_env_soc_unit_nums[MAX_UNITS_ID][MAX_DEV_ID_NUM] = {
/*                      6920     6928     */
/* PEX_UNIT_ID      */ { 4,       4},
/* ETH_GIG_UNIT_ID  */ { 3,       4},
/* USB3H_UNIT_ID    */ { 1,       2},
/* USB3D_UNIT_ID    */ { 0,       1},
/* SATA_UNIT_ID     */ { 0,       4},
/* QSGMII_UNIT_ID   */ { 0,       1},
/* XAUI_UNIT_ID     */ { 1,       1},
/* RXAUI_UNIT_ID    */ { 1,	  1}
};
#endif

u32 g_dev_id = -1;

u32 mv_board_id_get(void)
{
#if defined(CONFIG_DB_88F6820_GP)
	return DB_GP_68XX_ID;
#else
	/*
	 * Return 0 here for custom board as this should not be used
	 * for custom boards.
	 */
	return 0;
#endif
}

u32 mv_board_tclk_get(void)
{
	u32 value;

	value = (reg_read(DEVICE_SAMPLE_AT_RESET1_REG) >> 15) & 0x1;

	switch (value) {
	case (0x0):
		return 250000000;
	case (0x1):
		return 200000000;
	default:
		return 0xffffffff;
	}
}

u32 mv_board_id_index_get(u32 board_id)
{
	/*
	 * Marvell Boards use 0x10 as base for Board ID:
	 * mask MSB to receive index for board ID
	 */
	return board_id & (MARVELL_BOARD_ID_MASK - 1);
}

/*
 * sys_env_suspend_wakeup_check
 * DESCRIPTION:		Reads GPIO input for suspend-wakeup indication.
 * INPUT:		None.
 * OUTPUT:
 * RETURNS:		u32 indicating suspend wakeup status:
 * 0 - Not supported,
 * 1 - supported: read magic word detect wakeup,
 * 2 - detected wakeup from GPIO.
 */
enum suspend_wakeup_status sys_env_suspend_wakeup_check(void)
{
	u32 reg, board_id_index, gpio;
	struct board_wakeup_gpio board_gpio[] = MV_BOARD_WAKEUP_GPIO_INFO;

	board_id_index = mv_board_id_index_get(mv_board_id_get());
	if (!(sizeof(board_gpio) / sizeof(struct board_wakeup_gpio) >
	      board_id_index)) {
		printf("\n_failed loading Suspend-Wakeup information (invalid board ID)\n");
		return SUSPEND_WAKEUP_DISABLED;
	}

	/*
	 * - Detect if Suspend-Wakeup is supported on current board
	 * - Fetch the GPIO number for wakeup status input indication
	 */
	if (board_gpio[board_id_index].gpio_num == -1) {
		/* Suspend to RAM is not supported */
		return SUSPEND_WAKEUP_DISABLED;
	} else if (board_gpio[board_id_index].gpio_num == -2) {
		/*
		 * Suspend to RAM is supported but GPIO indication is
		 * not implemented - Skip
		 */
		return SUSPEND_WAKEUP_ENABLED;
	} else {
		gpio = board_gpio[board_id_index].gpio_num;
	}

	/* Initialize MPP for GPIO (set MPP = 0x0) */
	reg = reg_read(MPP_CONTROL_REG(MPP_REG_NUM(gpio)));
	/* reset MPP21 to 0x0, keep rest of MPP settings*/
	reg &= ~MPP_MASK(gpio);
	reg_write(MPP_CONTROL_REG(MPP_REG_NUM(gpio)), reg);

	/* Initialize GPIO as input */
	reg = reg_read(GPP_DATA_OUT_EN_REG(GPP_REG_NUM(gpio)));
	reg |= GPP_MASK(gpio);
	reg_write(GPP_DATA_OUT_EN_REG(GPP_REG_NUM(gpio)), reg);

	/*
	 * Check GPP for input status from PIC: 0 - regular init,
	 * 1 - suspend wakeup
	 */
	reg = reg_read(GPP_DATA_IN_REG(GPP_REG_NUM(gpio)));

	/* if GPIO is ON: wakeup from S2RAM indication detected */
	return (reg & GPP_MASK(gpio)) ? SUSPEND_WAKEUP_ENABLED_GPIO_DETECTED :
		SUSPEND_WAKEUP_DISABLED;
}

/*
 * mv_ctrl_dev_id_index_get
 *
 * DESCRIPTION: return SOC device index
 * INPUT: None
 * OUTPUT: None
 * RETURN:
 *        return SOC device index
 */
u32 sys_env_id_index_get(u32 ctrl_model)
{
	switch (ctrl_model) {
	case MV_6820_DEV_ID:
		return MV_6820_INDEX;
	case MV_6810_DEV_ID:
		return MV_6810_INDEX;
	case MV_6811_DEV_ID:
		return MV_6811_INDEX;
	case MV_6828_DEV_ID:
		return MV_6828_INDEX;
	case MV_6920_DEV_ID:
		return MV_6920_INDEX;
	case MV_6928_DEV_ID:
		return MV_6928_INDEX;
	default:
		return MV_6820_INDEX;
	}
}

u32 sys_env_unit_max_num_get(enum unit_id unit)
{
	u32 dev_id_index;

	if (unit >= MAX_UNITS_ID) {
		printf("%s: Error: Wrong unit type (%u)\n", __func__, unit);
		return 0;
	}

	dev_id_index = sys_env_id_index_get(sys_env_model_get());
	return sys_env_soc_unit_nums[unit][dev_id_index];
}

/*
 * sys_env_model_get
 * DESCRIPTION:	Returns 16bit describing the device model (ID) as defined
 *		in Vendor ID configuration register
 */
u16 sys_env_model_get(void)
{
	u32 default_ctrl_id, ctrl_id = reg_read(DEV_ID_REG);
	ctrl_id = (ctrl_id & (DEV_ID_REG_DEVICE_ID_MASK)) >>
		DEV_ID_REG_DEVICE_ID_OFFS;

	switch (ctrl_id) {
	case MV_6820_DEV_ID:
	case MV_6810_DEV_ID:
	case MV_6811_DEV_ID:
	case MV_6828_DEV_ID:
	case MV_6920_DEV_ID:
	case MV_6928_DEV_ID:
		return ctrl_id;
	default:
		/* Device ID Default for A38x: 6820 , for A39x: 6920 */
	#ifdef CONFIG_ARMADA_38X
		default_ctrl_id =  MV_6820_DEV_ID;
	#else
		default_ctrl_id = MV_6920_DEV_ID;
	#endif
		printf("%s: Error retrieving device ID (%x), using default ID = %x\n",
		       __func__, ctrl_id, default_ctrl_id);
		return default_ctrl_id;
	}
}

/*
 * sys_env_device_id_get
 * DESCRIPTION:	Returns enum (0..7) index of the device model (ID)
 */
u32 sys_env_device_id_get(void)
{
	char *device_id_str[7] = {
		"6810", "6820", "6811", "6828", "NONE", "6920", "6928"
	};

	if (g_dev_id != -1)
		return g_dev_id;

	g_dev_id = reg_read(DEVICE_SAMPLE_AT_RESET1_REG);
	g_dev_id = g_dev_id >> SAR_DEV_ID_OFFS & SAR_DEV_ID_MASK;
	printf("Detected Device ID %s\n", device_id_str[g_dev_id]);

	return g_dev_id;
}

/*
 * sys_env_device_rev_get - Get Marvell controller device revision number
 *
 * DESCRIPTION:
 *       This function returns 8bit describing the device revision as defined
 *       Revision ID Register.
 *
 * INPUT:
 *       None.
 *
 * OUTPUT:
 *       None.
 *
 * RETURN:
 *       8bit desscribing Marvell controller revision number
 */
u8 sys_env_device_rev_get(void)
{
	u32 value;

	value = reg_read(DEV_VERSION_ID_REG);
	return (value & (REVISON_ID_MASK)) >> REVISON_ID_OFFS;
}

/*
 * sys_env_dlb_config_ptr_get
 *
 * DESCRIPTION: defines pointer to to DLB COnfiguration table
 *
 * INPUT: none
 *
 * OUTPUT: pointer to DLB COnfiguration table
 *
 * RETURN:
 *       returns pointer to DLB COnfiguration table
 */
struct dlb_config *sys_env_dlb_config_ptr_get(void)
{
	return &ddr3_dlb_config_table[0];
}

/*
 * sys_env_get_cs_ena_from_reg
 *
 * DESCRIPTION: Get bit mask of enabled CS
 *
 * INPUT: None
 *
 * OUTPUT: None
 *
 * RETURN:
 *       Bit mask of enabled CS, 1 if only CS0 enabled,
 *       3 if both CS0 and CS1 enabled
 */
u32 sys_env_get_cs_ena_from_reg(void)
{
	return reg_read(REG_DDR3_RANK_CTRL_ADDR) &
		REG_DDR3_RANK_CTRL_CS_ENA_MASK;
}
