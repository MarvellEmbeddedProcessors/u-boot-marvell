/* SPDX-License-Identifier:    GPL-2.0
 *
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#ifndef __SWITCH_H__
#define __SWITCH_H__

/** Reg offsets */

/** Function definitions */
void board_switch_reset(void);
void board_switch_init(void);
struct udevice *get_switch_dev(void);
int read_virtual_reg_offset(u32 offset, u32 *reg_val);
u8 switch_cmd_opcode4(u8 profile_num);
int switch_cmd_opcode5(char *buffer);
int switch_cmd_opcode6(u32 dev_num, u32 interface_num, u32 *status);
int switch_cmd_opcode7(u32 group_num,  u32 id, char *buffer);
int switch_cmd_opcode10(u32 dev_num, u32 interface_num, u32 speed, u32 mode, u32 fec);
int switch_cmd_opcode11(u32 dev_num, u32 interface_num, u32 lane_num, u32 prbs_mode, u32 enable);
int switch_cmd_opcode12(u32 dev_num, u32 interface_num, u32 lane_num, u32 clear_count, u32 *count);
int switch_cmd_opcode13(u32 dev_num, u32 interface_num, u32 amplitude, u32 tx_amp_adj,
			int emph_0, int emph_1, u32 amp_shift);
int switch_cmd_opcode14(u32 dev_num, u32 interface_num, u32 lane_num, u32 dc_gain,
			u32 lf_gain, u32 hf_gain, u32 ctle_bw, u32 ctle_loop_bw, u32 sq_thresh);
int switch_cmd_opcode15(u32 dev_num, u32 interface_num, u32 lane_num, u32 fc_pause,
			u32 fc_am_dir, u32 fec_supp, u32 fec_req, u32 mode, u32 speed);
int switch_cmd_opcode17(u32 dev_num, u32 interface_num, u32 lane_num, u32 speed,
			u32 sq_thresh, u32 lf_gain, u32 hf_gain, u32 dc_gain,
			u32 ctle_bw, u32 ctle_loop_bw, u32 etl_min_delay,
			u32 etl_max_delay, u32 etl_enable, u32 override_bitmap);
int switch_cmd_opcode18(u32 dev_num, u32 interface_num, u32 lane_num, u32 speed,
			int tx_amp_offset, int emph0_offset, int emph1_offset);
int switch_cmd_opcode19(u32 dev_num, u32 intf_num, u32 lpbk_mode);
int switch_cmd_opcode20(u32 dev_num, u32 intf_num, u32 nego_mode);
int switch_cmd_opcode21(u32 dev_num, u32 intf_num, u32 lane_num, u32 *buffer);
int switch_cmd_opcode22(u32 dev_num, u32 intf_num, u32 lane_num, u32 *buffer);

/* Supported switch speeds */
#define MI_BOOT_CH_PORT_SPEED_1000_E		0
#define MI_BOOT_CH_PORT_SPEED_10000_E		1
#define MI_BOOT_CH_PORT_SPEED_2500_E		2
#define MI_BOOT_CH_PORT_SPEED_5000_E		3
#define MI_BOOT_CH_PORT_SPEED_20000_E		4
#define MI_BOOT_CH_PORT_SPEED_25000_E		5
#define MI_BOOT_CH_PORT_SPEED_40000_E		6
#define MI_BOOT_CH_PORT_SPEED_50000_E		7
#define MI_BOOT_CH_PORT_SPEED_100000_E		8

/* Supported modes */
#define MI_BOOT_CH_INTF_SGMII_E			0
#define MI_BOOT_CH_INTF_1000_BASEX_E		1
#define MI_BOOT_CH_INTF_QSGMII_E		2
#define MI_BOOT_CH_INTF_RXAUI_E			3
#define MI_BOOT_CH_INTF_CR_E			4
#define MI_BOOT_CH_INTF_CR_C_E			5
#define MI_BOOT_CH_INTF_CR_S_E			6
#define MI_BOOT_CH_INTF_CR2_C_E			7
#define MI_BOOT_CH_INTF_CR4_E			8
#define MI_BOOT_CH_INTF_KR_E			9
#define MI_BOOT_CH_INTF_KR_C_E			10
#define MI_BOOT_CH_INTF_KR_S_E			11
#define MI_BOOT_CH_INTF_KR2_C_E			12
#define MI_BOOT_CH_INTF_KR4_E			13
#define MI_BOOT_CH_INTF_SR_LR_E			14

/* Supported FEC modes */
#define MI_BOOT_CH_FEC_OFF			0
#define MI_BOOT_CH_FEC_FC			1
#define MI_BOOT_CH_FEC_RS			2

/* Supported PRBS modes */
#define MI_BOOT_CH_PRBS_1T			0
#define MI_BOOT_CH_PRBS_2T			1
#define MI_BOOT_CH_PRBS_5T			2
#define MI_BOOT_CH_PRBS_10T			3
#define MI_BOOT_CH_PRBS_PRBS7			4
#define MI_BOOT_CH_PRBS_PRBS9			5
#define MI_BOOT_CH_PRBS_PRBS15			6
#define MI_BOOT_CH_PRBS_PRBS23			7
#define MI_BOOT_CH_PRBS_PRBS31			8
#define MI_BOOT_CH_PRBS_DFE_TRAIN		9
#define MI_BOOT_CH_PRBS_OTHER			10
#define MI_BOOT_CH_PRBS_PRBS11			11
#define MI_BOOT_CH_PRBS_PRBS13			12

/* Field override bitmap */
#define MI_BOOT_CH_OVERRD_SQUELCH		BIT(0)
#define MI_BOOT_CH_OVERRD_FFE_RES		BIT(1)
#define MI_BOOT_CH_OVERRD_FFE_CAP		BIT(2)
#define MI_BOOT_CH_OVERRD_DC_GAIN		BIT(3)
#define MI_BOOT_CH_OVERRD_CTLE_BW		BIT(4)
#define MI_BOOT_CH_OVERRD_CTLE_LOOP_BW		BIT(5)
#define MI_BOOT_CH_OVERRD_ETL_MIN_DELAY		BIT(6)
#define MI_BOOT_CH_OVERRD_ETL_MAX_DELAY		BIT(7)
#define MI_BOOT_CH_OVERRD_ETL_ENABLE		BIT(8)

/* Supported loopback modes */
#define MI_BOOT_CH_LPBK_OFF			0
#define MI_BOOT_CH_LPBK_SRDS_TX2RX		1
#define MI_BOOT_CH_LPBK_SRDS_RX2TX		2
#define MI_BOOT_CH_LPBK_MAC_TX2RX		3

#endif /* __SWITCH_H__ */

