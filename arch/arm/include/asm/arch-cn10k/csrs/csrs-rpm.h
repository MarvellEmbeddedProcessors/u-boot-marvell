/* SPDX-License-Identifier:    GPL-2.0
 *
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */
#ifndef __CSRS_RPM_H__
#define __CSRS_RPM_H__

/**
 * @file
 *
 * Configuration and status register (CSR) address and type definitions for
 * RPM.
 *
 * This file is auto generated.  Do not edit.
 *
 */

/**
 * Enumeration rpm_bar_e
 *
 * RPM Base Address Register Enumeration Enumerates the base address
 * registers.
 */
#define RPM_BAR_E_RPMX_PF_BAR0(a) (0x87e0e0000000ll + 0x1000000ll * (a))
#define RPM_BAR_E_RPMX_PF_BAR0_SIZE 0x800000ull
#define RPM_BAR_E_RPMX_PF_BAR4(a) (0x87e0e0800000ll + 0x1000000ll * (a))
#define RPM_BAR_E_RPMX_PF_BAR4_SIZE 0x800000ull

/**
 * Enumeration rpm_int_vec_e
 *
 * RPM MSI-X Vector Enumeration Enumeration the MSI-X interrupt vectors.
 */
#define RPM_INT_VEC_E_CMRX_INT(a) (0 + 2 * (a))
#define RPM_INT_VEC_E_CMRX_SW(a) (0xa + (a))
#define RPM_INT_VEC_E_CMR_MEM_INT (8)
#define RPM_INT_VEC_E_SPUX_INT(a) (1 + 2 * (a))
#define RPM_INT_VEC_E_SW (9)

/**
 * Enumeration rpm_lmac_types_e
 *
 * RPM LMAC Type Enumeration Enumerates the LMAC Types that RPM supports.
 */
#define RPM_LMAC_TYPES_E_FIFTYG_R (8)
#define RPM_LMAC_TYPES_E_FORTYG_R (4)
#define RPM_LMAC_TYPES_E_HUNDREDG_R (9)
#define RPM_LMAC_TYPES_E_QSGMII (6)
#define RPM_LMAC_TYPES_E_RGMII (5)
#define RPM_LMAC_TYPES_E_RXAUI (2)
#define RPM_LMAC_TYPES_E_SGMII (0)
#define RPM_LMAC_TYPES_E_TENG_R (3)
#define RPM_LMAC_TYPES_E_TWENTYFIVEG_R (7)
#define RPM_LMAC_TYPES_E_USXGMII (0xa)
#define RPM_LMAC_TYPES_E_XAUI (1)

/**
 * Enumeration rpm_opcode_e
 *
 * INTERNAL: RPM Error Opcode Enumeration  Enumerates the error opcodes
 * created by RPM and presented to NIX.
 */
#define RPM_OPCODE_E_RE_FCS (7)
#define RPM_OPCODE_E_RE_FCS_RCV (8)
#define RPM_OPCODE_E_RE_JABBER (2)
#define RPM_OPCODE_E_RE_NONE (0)
#define RPM_OPCODE_E_RE_PARTIAL (1)
#define RPM_OPCODE_E_RE_RX_CTL (0xb)
#define RPM_OPCODE_E_RE_SKIP (0xc)
#define RPM_OPCODE_E_RE_TERMINATE (9)

/**
 * Enumeration rpm_spu_br_train_cst_e
 *
 * INTERNAL: RPM Training Coefficient Status Enumeration  2-bit status
 * for each coefficient as defined in IEEE 802.3, Table 72-5.
 */
#define RPM_SPU_BR_TRAIN_CST_E_MAXIMUM (3)
#define RPM_SPU_BR_TRAIN_CST_E_MINIMUM (2)
#define RPM_SPU_BR_TRAIN_CST_E_NOT_UPDATED (0)
#define RPM_SPU_BR_TRAIN_CST_E_UPDATED (1)

/**
 * Enumeration rpm_spu_br_train_cup_e
 *
 * INTERNAL:RPM Training Coefficient Enumeration  2-bit command for each
 * coefficient as defined in IEEE 802.3, Table 72-4.
 */
#define RPM_SPU_BR_TRAIN_CUP_E_DECREMENT (1)
#define RPM_SPU_BR_TRAIN_CUP_E_HOLD (0)
#define RPM_SPU_BR_TRAIN_CUP_E_INCREMENT (2)
#define RPM_SPU_BR_TRAIN_CUP_E_RSV_CMD (3)

/**
 * Enumeration rpm_usxgmii_rate_e
 *
 * RPM USXGMII Rate Enumeration Enumerates the USXGMII sub-port type
 * rate, RPM()_SPU()_CONTROL1[USXGMII_RATE].  Selecting a rate higher
 * than the maximum allowed for a given port sub-type (specified by
 * RPM()_SPU()_CONTROL1[USXGMII_TYPE]), e.g., selecting ::RATE_2HG (2.5
 * Gbps) for RPM_USXGMII_TYPE_E::SXGMII_2G, will cause unpredictable
 * behavior. USXGMII hardware-based autonegotiation may change this
 * setting.
 */
#define RPM_USXGMII_RATE_E_RATE_100M (1)
#define RPM_USXGMII_RATE_E_RATE_10G (5)
#define RPM_USXGMII_RATE_E_RATE_10M (0)
#define RPM_USXGMII_RATE_E_RATE_1G (2)
#define RPM_USXGMII_RATE_E_RATE_20G (6)
#define RPM_USXGMII_RATE_E_RATE_2HG (3)
#define RPM_USXGMII_RATE_E_RATE_5G (4)
#define RPM_USXGMII_RATE_E_RSV_RATE (7)

/**
 * Enumeration rpm_usxgmii_type_e
 *
 * RPM USXGMII Port Sub-Type Enumeration Enumerates the USXGMII sub-port
 * type, RPM()_SPU()_CONTROL1[USXGMII_TYPE].  The description indicates
 * the maximum rate and the maximum number of ports (LMACs) for each sub-
 * type. The minimum rate for any port is 10M. The rate selection for
 * each LMAC is made using RPM()_SPU()_CONTROL1[USXGMII_RATE] and the
 * number of active ports/LMACs is implicitly determined by the value
 * given to RPM()_CMR()_CONFIG[ENABLE] for each LMAC.  Selecting a rate
 * higher than the maximum allowed for a given port sub-type or enabling
 * more LMACs than the maximum allowed for a given port sub-type will
 * cause unpredictable behavior.
 */
#define RPM_USXGMII_TYPE_E_DXGMII_10G (3)
#define RPM_USXGMII_TYPE_E_DXGMII_20G (5)
#define RPM_USXGMII_TYPE_E_DXGMII_5G (4)
#define RPM_USXGMII_TYPE_E_QXGMII_10G (7)
#define RPM_USXGMII_TYPE_E_QXGMII_20G (6)
#define RPM_USXGMII_TYPE_E_SXGMII_10G (0)
#define RPM_USXGMII_TYPE_E_SXGMII_2G (2)
#define RPM_USXGMII_TYPE_E_SXGMII_5G (1)

/**
 * Structure rpm_spu_br_lane_train_status_s
 *
 * INTERNAL:RPM Lane Training Status Structure  This is the group of lane
 * status bits for a single lane in the BASE-R PMD status register (MDIO
 * address 1.151) as defined in IEEE 802.3ba-2010, Table 45-55.
 */
union rpm_spu_br_lane_train_status_s {
	u32 u;
	struct rpm_spu_br_lane_train_status_s_s {
		u32 rx_trained                       : 1;
		u32 frame_lock                       : 1;
		u32 training                         : 1;
		u32 training_failure                 : 1;
		u32 reserved_4_31                    : 28;
	} s;
	/* struct rpm_spu_br_lane_train_status_s_s cn; */
};

/**
 * Structure rpm_spu_br_train_cup_s
 *
 * INTERNAL:RPM Lane Training Coefficient Structure  This is the
 * coefficient update field of the BASE-R link training packet as defined
 * in IEEE 802.3, Table 72-4.
 */
union rpm_spu_br_train_cup_s {
	u32 u;
	struct rpm_spu_br_train_cup_s_s {
		u32 pre_cup                          : 2;
		u32 main_cup                         : 2;
		u32 post_cup                         : 2;
		u32 reserved_6_11                    : 6;
		u32 init                             : 1;
		u32 preset                           : 1;
		u32 reserved_14_31                   : 18;
	} s;
	struct rpm_spu_br_train_cup_s_cn {
		u32 pre_cup                          : 2;
		u32 main_cup                         : 2;
		u32 post_cup                         : 2;
		u32 reserved_6_11                    : 6;
		u32 init                             : 1;
		u32 preset                           : 1;
		u32 reserved_14_15                   : 2;
		u32 reserved_16_31                   : 16;
	} cn;
};

/**
 * Structure rpm_spu_br_train_rep_s
 *
 * INTERNAL:RPM Training Report Structure  This is the status report
 * field of the BASE-R link training packet as defined in IEEE 802.3,
 * Table 72-5.
 */
union rpm_spu_br_train_rep_s {
	u32 u;
	struct rpm_spu_br_train_rep_s_s {
		u32 pre_cst                          : 2;
		u32 main_cst                         : 2;
		u32 post_cst                         : 2;
		u32 reserved_6_14                    : 9;
		u32 rx_ready                         : 1;
		u32 reserved_16_31                   : 16;
	} s;
	/* struct rpm_spu_br_train_rep_s_s cn; */
};

/**
 * Structure rpm_spu_sds_cu_s
 *
 * INTERNAL: RPM Training Coeffiecient Structure  This structure is
 * similar to RPM_SPU_BR_TRAIN_CUP_S format, but with reserved fields
 * removed and [RCVR_READY] field added.
 */
union rpm_spu_sds_cu_s {
	u32 u;
	struct rpm_spu_sds_cu_s_s {
		u32 pre_cu                           : 2;
		u32 main_cu                          : 2;
		u32 post_cu                          : 2;
		u32 initialize                       : 1;
		u32 preset                           : 1;
		u32 rcvr_ready                       : 1;
		u32 reserved_9_31                    : 23;
	} s;
	/* struct rpm_spu_sds_cu_s_s cn; */
};

/**
 * Structure rpm_spu_sds_skew_status_s
 *
 * RPM Skew Status Structure Provides receive skew information detected
 * for a physical SerDes lane when it is assigned to a multilane
 * LMAC/LPCS. Contents are valid when RX deskew is done for the
 * associated LMAC/LPCS.
 */
union rpm_spu_sds_skew_status_s {
	u32 u;
	struct rpm_spu_sds_skew_status_s_s {
		u32 am_timestamp                     : 12;
		u32 reserved_12_15                   : 4;
		u32 am_lane_id                       : 5;
		u32 reserved_21_22                   : 2;
		u32 lane_skew                        : 7;
		u32 reserved_30_31                   : 2;
	} s;
	/* struct rpm_spu_sds_skew_status_s_s cn; */
};

/**
 * Structure rpm_spu_sds_sr_s
 *
 * INTERNAL: RPM Lane Training Coefficient Structure  Similar to
 * RPM_SPU_BR_TRAIN_REP_S format, but with reserved and RX ready fields
 * removed.
 */
union rpm_spu_sds_sr_s {
	u32 u;
	struct rpm_spu_sds_sr_s_s {
		u32 pre_status                       : 2;
		u32 main_status                      : 2;
		u32 post_status                      : 2;
		u32 reserved_6_31                    : 26;
	} s;
	/* struct rpm_spu_sds_sr_s_s cn; */
};

/**
 * Register (RSL) rpm#_active_pc
 *
 * RPM ACTIVE PC Register This register counts the conditional clocks for
 * power management.
 */
union rpmx_active_pc {
	u64 u;
	struct rpmx_active_pc_s {
		u64 cnt                              : 64;
	} s;
	/* struct rpmx_active_pc_s cn; */
};

static inline u64 RPMX_ACTIVE_PC(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ACTIVE_PC(void)
{
	return 0x2010;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_0
 *
 * RPM An  Aneg Lane 0 Control Register 0 Register
 */
union rpmx_anx_aneg_lane_0_control_register_0 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_0_s {
		u64 override_ctrl_s                  : 2;
		u64 ap_aneg_amdisam_s                : 1;
		u64 ap_aneg_bp_reached_s             : 1;
		u64 ap_aneg_state_s10_0              : 11;
		u64 rg_eee_xnp_sel_s                 : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_0_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_0(u64 a)
{
	return 0x49000 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_1
 *
 * RPM An  Aneg Lane 0 Control Register 1 Register
 */
union rpmx_anx_aneg_lane_0_control_register_1 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_1_s {
		u64 ag_lgth_match_cnt_s3_0           : 4;
		u64 cg_aneg_test0_s                  : 1;
		u64 cg_aneg_test3_s                  : 1;
		u64 rg_no_ringosc_s                  : 1;
		u64 ow_as_nonce_match_s              : 1;
		u64 rg_as_nonce_match_s              : 1;
		u64 rg_link_fail_timer_sel1500_s     : 2;
		u64 rg_link_fail_timer_sel500_s      : 2;
		u64 rg_link_fail_timer_sel50_s       : 2;
		u64 rg_link_fail_timer_off_s         : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_1_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_1(u64 a)
{
	return 0x49008 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_10
 *
 * RPM An  Aneg Lane 0 Control Register 10 Register
 */
union rpmx_anx_aneg_lane_0_control_register_10 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_10_s {
		u64 advertised_con25gkrcr_location   : 5;
		u64 advertised_con50gkrcr_location   : 5;
		u64 advertised_con400grcr8_location  : 3;
		u64 reg10_reserved13                 : 2;
		u64 rg_ap_local_reset_s              : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_10_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_10(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_10(u64 a)
{
	return 0x49050 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_11
 *
 * RPM An  Aneg Lane 0 Control Register 11 Register
 */
union rpmx_anx_aneg_lane_0_control_register_11 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_11_s {
		u64 reg11_reserved                   : 3;
		u64 symbol_lock_override_value       : 1;
		u64 reg11_reserved4                  : 3;
		u64 reg11_reserved7                  : 4;
		u64 chk_dsp_lock_s                   : 1;
		u64 reg11_reserved12                 : 3;
		u64 symbol_lock_select               : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_11_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_11(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_11(u64 a)
{
	return 0x49058 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_12
 *
 * RPM An  Aneg Lane 0 Control Register 12 Register
 */
union rpmx_anx_aneg_lane_0_control_register_12 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_12_s {
		u64 reg12_reserved                   : 3;
		u64 symbol_lock_override_enable      : 1;
		u64 reg12_my_reserved4               : 4;
		u64 n_ag_mode_s                      : 5;
		u64 adv_use_n_agmode_s               : 1;
		u64 re_agstart_use_n_link_s          : 1;
		u64 intel_fec_mode                   : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_12_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_12(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_12(u64 a)
{
	return 0x49060 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_13
 *
 * RPM An  Aneg Lane 0 Control Register 13 Register
 */
union rpmx_anx_aneg_lane_0_control_register_13 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_13_s {
		u64 ap_interrupt                     : 1;
		u64 reserved_1_63                    : 63;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_13_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_13(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_13(u64 a)
{
	return 0x49068 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_14
 *
 * RPM An  Aneg Lane 0 Control Register 14 Register
 */
union rpmx_anx_aneg_lane_0_control_register_14 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_14_s {
		u64 advertised_200g_r8_marvell_mode_a15_to_a22_bit_location_select : 2;
		u64 advertised_25r2_mode_a15_to_a22_bit_location_select : 2;
		u64 advertised_800g_r8_marvell_mode_a15_to_a22_bit_location_select : 2;
		u64 advertised_50g_r4_marvell_mode_a15_to_a22_bit_location_select : 2;
		u64 reserved_8                       : 1;
		u64 advertised_40g_r2_mode_a15_to_a22_bit_location_select : 3;
		u64 cfg_40gr2_prio_higher_than_40gr4_s : 1;
		u64 ap_interrupt_type                : 1;
		u64 interrupt_time_select            : 1;
		u64 interrupt_mask                   : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_14_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_14(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_14(u64 a)
{
	return 0x49070 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_15
 *
 * RPM An  Aneg Lane 0 Control Register 15 Register
 */
union rpmx_anx_aneg_lane_0_control_register_15 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_15_s {
		u64 pm_norm_x_state_s                : 1;
		u64 far_set_restart_all_s            : 1;
		u64 far_clear_reset_all_s            : 1;
		u64 reserved_3                       : 1;
		u64 pcs_link                         : 1;
		u64 reserved_5_14                    : 10;
		u64 symbol_lock                      : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_15_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_15(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_15(u64 a)
{
	return 0x49078 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_16_consortium_mp5_register_0
 *
 * RPM An  Aneg Lane 0 Control Register 16 Consortium Mp5 Register 0
 * Register
 */
union rpmx_anx_aneg_lane_0_control_register_16_consortium_mp5_register_0 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_16_consortium_mp5_register_0_s {
		u64 reg16_messageunformatted_field   : 11;
		u64 reg16_toggle                     : 1;
		u64 reg16_acknowledge2               : 1;
		u64 reg16_message_page               : 1;
		u64 reg16_acknowledge                : 1;
		u64 reg16_next_page                  : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_16_consortium_mp5_register_0_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_16_CONSORTIUM_MP5_REGISTER_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_16_CONSORTIUM_MP5_REGISTER_0(u64 a)
{
	return 0x49080 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_17_consortium_mp5_register_1
 *
 * RPM An  Aneg Lane 0 Control Register 17 Consortium Mp5 Register 1
 * Register
 */
union rpmx_anx_aneg_lane_0_control_register_17_consortium_mp5_register_1 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_17_consortium_mp5_register_1_s {
		u64 reg17_oui_23_13                  : 11;
		u64 reg17_reserved11                 : 5;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_17_consortium_mp5_register_1_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_17_CONSORTIUM_MP5_REGISTER_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_17_CONSORTIUM_MP5_REGISTER_1(u64 a)
{
	return 0x49088 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_18_consortium_mp5_register_2
 *
 * RPM An  Aneg Lane 0 Control Register 18 Consortium Mp5 Register 2
 * Register
 */
union rpmx_anx_aneg_lane_0_control_register_18_consortium_mp5_register_2 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_18_consortium_mp5_register_2_s {
		u64 reg18_oui_12_2                   : 11;
		u64 reg18_reserved11                 : 5;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_18_consortium_mp5_register_2_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_18_CONSORTIUM_MP5_REGISTER_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_18_CONSORTIUM_MP5_REGISTER_2(u64 a)
{
	return 0x49090 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_19_consortium_et_register_0
 *
 * RPM An  Aneg Lane 0 Control Register 19 Consortium Et Register 0
 * Register
 */
union rpmx_anx_aneg_lane_0_control_register_19_consortium_et_register_0 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_19_consortium_et_register_0_s {
		u64 reg19_messageunformatted_field   : 9;
		u64 reg19_oui_1_0                    : 2;
		u64 reg19_toggle                     : 1;
		u64 reg19_acknowledge2               : 1;
		u64 reg19_message_page               : 1;
		u64 reg19_acknowledge                : 1;
		u64 reg19_next_page                  : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_19_consortium_et_register_0_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_19_CONSORTIUM_ET_REGISTER_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_19_CONSORTIUM_ET_REGISTER_0(u64 a)
{
	return 0x49098 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_2
 *
 * RPM An  Aneg Lane 0 Control Register 2 Register
 */
union rpmx_anx_aneg_lane_0_control_register_2 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_2_s {
		u64 clear_hcd_ow_s                   : 1;
		u64 rg_clear_hcd_s                   : 1;
		u64 aneg_enable_ow_s                 : 1;
		u64 rg_aneg_enable_s                 : 1;
		u64 restart_aneg_ow_s                : 1;
		u64 rg_restart_aneg_s                : 1;
		u64 link_ow_s                        : 1;
		u64 rg_link_s                        : 1;
		u64 hcd_resolved_ow_s                : 1;
		u64 rg_hcd_resolved_s                : 1;
		u64 reg2_reserved10                  : 6;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_2_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_2(u64 a)
{
	return 0x49010 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_20_consortium_et_register_1
 *
 * RPM An  Aneg Lane 0 Control Register 20 Consortium Et Register 1
 * Register
 */
union rpmx_anx_aneg_lane_0_control_register_20_consortium_et_register_1 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_20_consortium_et_register_1_s {
		u64 reg20_reserved                   : 4;
		u64 reg20_consortium_25g_kr1         : 1;
		u64 reg20_consortium_25g_cr1         : 1;
		u64 reg20_consortium_reserved        : 2;
		u64 reg20_consortium_50g_kr2         : 1;
		u64 reg20_consortium_50g_cr2         : 1;
		u64 reg20_reserved10                 : 6;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_20_consortium_et_register_1_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_20_CONSORTIUM_ET_REGISTER_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_20_CONSORTIUM_ET_REGISTER_1(u64 a)
{
	return 0x490a0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_21_consortium_et_register_2
 *
 * RPM An  Aneg Lane 0 Control Register 21 Consortium Et Register 2
 * Register
 */
union rpmx_anx_aneg_lane_0_control_register_21_consortium_et_register_2 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_21_consortium_et_register_2_s {
		u64 reg21_marvell_consortium_40gr2   : 1;
		u64 reg21_reserved1                  : 1;
		u64 reg21_marvell_consortium_400gr8  : 1;
		u64 reg21_reserved3                  : 2;
		u64 reg21_consortium_lf1             : 1;
		u64 reg21_consortium_lf2             : 1;
		u64 reg21_consortium_lf3             : 1;
		u64 reg21_consortium_f1              : 1;
		u64 reg21_consortium_f2              : 1;
		u64 reg21_consortium_f3              : 1;
		u64 reg21_consortium_f4              : 1;
		u64 reg21_consortium_lfr             : 1;
		u64 reg21_reserved13                 : 3;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_21_consortium_et_register_2_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_21_CONSORTIUM_ET_REGISTER_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_21_CONSORTIUM_ET_REGISTER_2(u64 a)
{
	return 0x490a8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_22_link_partner_consortium_mp5_register_0
 *
 * RPM An  Aneg Lane 0 Control Register 22 Link Partner Consortium Mp5
 * Register 0 Register
 */
union rpmx_anx_aneg_lane_0_control_register_22_link_partner_consortium_mp5_register_0 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_22_link_partner_consortium_mp5_register_0_s {
		u64 reg22_messageunformatted_field   : 11;
		u64 reg22_toggle                     : 1;
		u64 reg22_acknowledge2               : 1;
		u64 reg22_message_page               : 1;
		u64 reg22_acknowledge                : 1;
		u64 reg22_next_page                  : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_22_link_partner_consortium_mp5_register_0_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_22_LINK_PARTNER_CONSORTIUM_MP5_REGISTER_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_22_LINK_PARTNER_CONSORTIUM_MP5_REGISTER_0(u64 a)
{
	return 0x490b0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_23_link_partner_consortium_mp5_register_1
 *
 * RPM An  Aneg Lane 0 Control Register 23 Link Partner Consortium Mp5
 * Register 1 Register
 */
union rpmx_anx_aneg_lane_0_control_register_23_link_partner_consortium_mp5_register_1 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_23_link_partner_consortium_mp5_register_1_s {
		u64 reg23_oui_23_13                  : 11;
		u64 reserved_11_63                   : 53;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_23_link_partner_consortium_mp5_register_1_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_23_LINK_PARTNER_CONSORTIUM_MP5_REGISTER_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_23_LINK_PARTNER_CONSORTIUM_MP5_REGISTER_1(u64 a)
{
	return 0x490b8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_24_link_partner_consortium_mp5_register_2
 *
 * RPM An  Aneg Lane 0 Control Register 24 Link Partner Consortium Mp5
 * Register 2 Register
 */
union rpmx_anx_aneg_lane_0_control_register_24_link_partner_consortium_mp5_register_2 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_24_link_partner_consortium_mp5_register_2_s {
		u64 reg24_oui_12_2                   : 11;
		u64 reg24_reserved11                 : 5;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_24_link_partner_consortium_mp5_register_2_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_24_LINK_PARTNER_CONSORTIUM_MP5_REGISTER_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_24_LINK_PARTNER_CONSORTIUM_MP5_REGISTER_2(u64 a)
{
	return 0x490c0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_25_link_partner_consortium_et_register_0
 *
 * RPM An  Aneg Lane 0 Control Register 25 Link Partner Consortium Et
 * Register 0 Register
 */
union rpmx_anx_aneg_lane_0_control_register_25_link_partner_consortium_et_register_0 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_25_link_partner_consortium_et_register_0_s {
		u64 reg25_messageunformatted_field   : 9;
		u64 reg25_oui_1_0                    : 2;
		u64 reg25_toggle                     : 1;
		u64 reg25_acknowledge2               : 1;
		u64 reg25_message_page               : 1;
		u64 reg25_acknowledge                : 1;
		u64 reg25_next_page                  : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_25_link_partner_consortium_et_register_0_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_25_LINK_PARTNER_CONSORTIUM_ET_REGISTER_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_25_LINK_PARTNER_CONSORTIUM_ET_REGISTER_0(u64 a)
{
	return 0x490c8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_26_link_partern_consortium_et_register_1
 *
 * RPM An  Aneg Lane 0 Control Register 26 Link Partern Consortium Et
 * Register 1 Register
 */
union rpmx_anx_aneg_lane_0_control_register_26_link_partern_consortium_et_register_1 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_26_link_partern_consortium_et_register_1_s {
		u64 reg26_reserved                   : 4;
		u64 reg26_consortium_25g_kr1         : 1;
		u64 reg26_consortium_25g_cr1         : 1;
		u64 reg26_consortium_reserved        : 2;
		u64 reg26_consortium_50g_kr2         : 1;
		u64 reg26_consortium_50g_cr2         : 1;
		u64 reg26_reserved10                 : 6;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_26_link_partern_consortium_et_register_1_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_26_LINK_PARTERN_CONSORTIUM_ET_REGISTER_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_26_LINK_PARTERN_CONSORTIUM_ET_REGISTER_1(u64 a)
{
	return 0x490d0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_27_link_partner_consortium_et_register_2
 *
 * RPM An  Aneg Lane 0 Control Register 27 Link Partner Consortium Et
 * Register 2 Register
 */
union rpmx_anx_aneg_lane_0_control_register_27_link_partner_consortium_et_register_2 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_27_link_partner_consortium_et_register_2_s {
		u64 reg27_marvell_consortium_40gr2   : 1;
		u64 reg27_reserved1                  : 1;
		u64 reg27_marvell_consortium_400gr8  : 1;
		u64 reg27_reserved3                  : 2;
		u64 reg27_consortium_lf1             : 1;
		u64 reg27_consortium_lf2             : 1;
		u64 reg27_consortium_lf3             : 1;
		u64 reg27_consortium_f1              : 1;
		u64 reg27_consortium_f2              : 1;
		u64 reg27_consortium_f3              : 1;
		u64 reg27_consortium_f4              : 1;
		u64 reg27_consortium_lfr             : 1;
		u64 reg27_reserved13                 : 3;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_27_link_partner_consortium_et_register_2_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_27_LINK_PARTNER_CONSORTIUM_ET_REGISTER_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_27_LINK_PARTNER_CONSORTIUM_ET_REGISTER_2(u64 a)
{
	return 0x490d8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_3
 *
 * RPM An  Aneg Lane 0 Control Register 3 Register
 */
union rpmx_anx_aneg_lane_0_control_register_3 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_3_s {
		u64 reg3_reserved                    : 2;
		u64 aa_sfec_enable_s                 : 1;
		u64 aa_fec_enable_s                  : 1;
		u64 aa_rsfec_enable_overwrite_value  : 1;
		u64 ap_rsfec_enable_overwrite        : 1;
		u64 aa_fec_enable_overwrite_value    : 1;
		u64 ap_fec_enable_overwrite          : 1;
		u64 tx_pause_enable_ow_s             : 1;
		u64 rg_tx_pause_enable_s             : 1;
		u64 rx_pause_enable_ow_s             : 1;
		u64 rg_rx_pause_enable_s             : 1;
		u64 pcs_lock_ow_s                    : 1;
		u64 rg_pcs_lock_s                    : 1;
		u64 dsp_lock_ow_s                    : 1;
		u64 rg_dsp_lock_s                    : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_3_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_3(u64 a)
{
	return 0x49018 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_4
 *
 * RPM An  Aneg Lane 0 Control Register 4 Register
 */
union rpmx_anx_aneg_lane_0_control_register_4 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_4_s {
		u64 aa_pwrup_g_s                     : 1;
		u64 aa_pwrup_2p5g_s                  : 1;
		u64 aa_pwrup_5gr_s                   : 1;
		u64 aa_pwrup_10gkr_s                 : 1;
		u64 aa_pwrup_con25gkr_s              : 1;
		u64 aa_pwrup_con25gcr_s              : 1;
		u64 aa_pwrup_ieee25gs_s              : 1;
		u64 aa_pwrup_ieee25grcr_s            : 1;
		u64 aa_pwrup_40gkr4_s                : 1;
		u64 aa_pwrup_40gcr4_s                : 1;
		u64 aa_pwrup_con50gkr2_s             : 1;
		u64 aa_pwrup_con50gcr2_s             : 1;
		u64 aa_pwrup_100gkr4_s               : 1;
		u64 aa_pwrup_100gcr4_s               : 1;
		u64 aa_pwrup_50grcr_s                : 1;
		u64 aa_pwrup_100grcr2_s              : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_4_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_4(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_4(u64 a)
{
	return 0x49020 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_5
 *
 * RPM An  Aneg Lane 0 Control Register 5 Register
 */
union rpmx_anx_aneg_lane_0_control_register_5 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_5_s {
		u64 aa_pwrup_200grcr4_s              : 1;
		u64 aa_pwrup_25gkr4_s                : 1;
		u64 aa_pwrup_50gkr4_s                : 1;
		u64 aa_pwrup_25gkr2_s                : 1;
		u64 aa_pwrup_40gr2                   : 1;
		u64 my_reserved5                     : 5;
		u64 selected_modes_to_overwrite      : 5;
		u64 pwrup_modes_ow_s                 : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_5_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_5(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_5(u64 a)
{
	return 0x49028 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_6
 *
 * RPM An  Aneg Lane 0 Control Register 6 Register
 */
union rpmx_anx_aneg_lane_0_control_register_6 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_6_s {
		u64 ow_pm_norm_x_state_s             : 1;
		u64 rg_pm_norm_x_state_s             : 1;
		u64 ow_pm_loopback_s                 : 1;
		u64 rg_pm_loopback_s                 : 1;
		u64 ow_far_clear_reset_all_s         : 1;
		u64 rg_far_clear_reset_all_s         : 1;
		u64 ow_far_set_restart_all_s         : 1;
		u64 rg_far_set_restart_all_s         : 1;
		u64 ow_ring_osc_s                    : 1;
		u64 rg_ring_osc_s                    : 1;
		u64 reg6_reserved10                  : 6;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_6_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_6(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_6(u64 a)
{
	return 0x49030 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_7
 *
 * RPM An  Aneg Lane 0 Control Register 7 Register
 */
union rpmx_anx_aneg_lane_0_control_register_7 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_7_s {
		u64 rg_lostlock_value_s              : 1;
		u64 rg_lostlock_ow_s                 : 1;
		u64 rg_lostlock_sel_s                : 1;
		u64 rg_timer2_length_s_ow_s          : 1;
		u64 rg_timer2_length_sel_s           : 2;
		u64 my_reserved6                     : 10;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_7_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_7(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_7(u64 a)
{
	return 0x49038 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_8
 *
 * RPM An  Aneg Lane 0 Control Register 8 Register
 */
union rpmx_anx_aneg_lane_0_control_register_8 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_8_s {
		u64 det_data_timer_window_select     : 3;
		u64 rg_det_dat_timer_ow_s            : 1;
		u64 det_clock_timer_window_select    : 3;
		u64 rg_det_clk_timer_ow_s            : 1;
		u64 rg_ignore_pulse_too_short_s      : 1;
		u64 break_link_length_override_value_select : 2;
		u64 rg_break_link_length_ow_s        : 1;
		u64 rg_disable_hw_np_s               : 1;
		u64 rg_chk_clk_tran_s                : 1;
		u64 my_reserved14                    : 2;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_8_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_8(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_8(u64 a)
{
	return 0x49040 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_lane_0_control_register_9
 *
 * RPM An  Aneg Lane 0 Control Register 9 Register
 */
union rpmx_anx_aneg_lane_0_control_register_9 {
	u64 u;
	struct rpmx_anx_aneg_lane_0_control_register_9_s {
		u64 sw_ag_mode                       : 5;
		u64 sw_fec_enable_s                  : 1;
		u64 sw_rsfec_enable_s                : 1;
		u64 sw_resolved_s                    : 1;
		u64 sw_llfec_enable_s                : 1;
		u64 my_reserved9                     : 4;
		u64 sw_lp_selector_enable_s          : 1;
		u64 z80_io_resolved                  : 1;
		u64 sw_ap_enable                     : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_lane_0_control_register_9_s cn; */
};

static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_9(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_LANE_0_CONTROL_REGISTER_9(u64 a)
{
	return 0x49048 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_z80_message_reg_0
 *
 * RPM An  Aneg Z80 Message Reg 0 Register
 */
union rpmx_anx_aneg_z80_message_reg_0 {
	u64 u;
	struct rpmx_anx_aneg_z80_message_reg_0_s {
		u64 aneg_z80_message0                : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_z80_message_reg_0_s cn; */
};

static inline u64 RPMX_ANX_ANEG_Z80_MESSAGE_REG_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_Z80_MESSAGE_REG_0(u64 a)
{
	return 0x49100 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_z80_message_reg_1
 *
 * RPM An  Aneg Z80 Message Reg 1 Register
 */
union rpmx_anx_aneg_z80_message_reg_1 {
	u64 u;
	struct rpmx_anx_aneg_z80_message_reg_1_s {
		u64 aneg_z80_message1                : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_z80_message_reg_1_s cn; */
};

static inline u64 RPMX_ANX_ANEG_Z80_MESSAGE_REG_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_Z80_MESSAGE_REG_1(u64 a)
{
	return 0x49108 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_z80_message_reg_2
 *
 * RPM An  Aneg Z80 Message Reg 2 Register
 */
union rpmx_anx_aneg_z80_message_reg_2 {
	u64 u;
	struct rpmx_anx_aneg_z80_message_reg_2_s {
		u64 aneg_z80_message2                : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_z80_message_reg_2_s cn; */
};

static inline u64 RPMX_ANX_ANEG_Z80_MESSAGE_REG_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_Z80_MESSAGE_REG_2(u64 a)
{
	return 0x49110 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_aneg_z80_message_reg_3
 *
 * RPM An  Aneg Z80 Message Reg 3 Register
 */
union rpmx_anx_aneg_z80_message_reg_3 {
	u64 u;
	struct rpmx_anx_aneg_z80_message_reg_3_s {
		u64 aneg_z80_message3                : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_aneg_z80_message_reg_3_s cn; */
};

static inline u64 RPMX_ANX_ANEG_Z80_MESSAGE_REG_3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_ANEG_Z80_MESSAGE_REG_3(u64 a)
{
	return 0x49118 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_auto_negotiation_device_identifier_1
 *
 * RPM An  Auto Negotiation Device Identifier 1 Register
 */
union rpmx_anx_auto_negotiation_device_identifier_1 {
	u64 u;
	struct rpmx_anx_auto_negotiation_device_identifier_1_s {
		u64 organizationally_unique_identifier_bit_318 : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_auto_negotiation_device_identifier_1_s cn; */
};

static inline u64 RPMX_ANX_AUTO_NEGOTIATION_DEVICE_IDENTIFIER_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_AUTO_NEGOTIATION_DEVICE_IDENTIFIER_1(u64 a)
{
	return 0x48010 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_auto_negotiation_device_identifier_2
 *
 * RPM An  Auto Negotiation Device Identifier 2 Register
 */
union rpmx_anx_auto_negotiation_device_identifier_2 {
	u64 u;
	struct rpmx_anx_auto_negotiation_device_identifier_2_s {
		u64 revision_number                  : 4;
		u64 model_number                     : 6;
		u64 organizationally_unique_identifier_bit_1924 : 6;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_auto_negotiation_device_identifier_2_s cn; */
};

static inline u64 RPMX_ANX_AUTO_NEGOTIATION_DEVICE_IDENTIFIER_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_AUTO_NEGOTIATION_DEVICE_IDENTIFIER_2(u64 a)
{
	return 0x48018 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_auto_negotiation_devices_in_package_1
 *
 * RPM An  Auto Negotiation Devices In Package 1 Register
 */
union rpmx_anx_auto_negotiation_devices_in_package_1 {
	u64 u;
	struct rpmx_anx_auto_negotiation_devices_in_package_1_s {
		u64 clause_22_registers_present      : 1;
		u64 pmdpma_present                   : 1;
		u64 wis_present                      : 1;
		u64 pcs_present                      : 1;
		u64 phy_xs_present                   : 1;
		u64 dte_xs_present                   : 1;
		u64 tc_present                       : 1;
		u64 auto_negotiation_present         : 1;
		u64 separated_pma_1                  : 1;
		u64 separated_pma_2                  : 1;
		u64 separated_pma_3                  : 1;
		u64 separated_pma_4                  : 1;
		u64 reserved_12_63                   : 52;
	} s;
	/* struct rpmx_anx_auto_negotiation_devices_in_package_1_s cn; */
};

static inline u64 RPMX_ANX_AUTO_NEGOTIATION_DEVICES_IN_PACKAGE_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_AUTO_NEGOTIATION_DEVICES_IN_PACKAGE_1(u64 a)
{
	return 0x48028 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_auto_negotiation_devices_in_package_2
 *
 * RPM An  Auto Negotiation Devices In Package 2 Register
 */
union rpmx_anx_auto_negotiation_devices_in_package_2 {
	u64 u;
	struct rpmx_anx_auto_negotiation_devices_in_package_2_s {
		u64 reserved_0_12                    : 13;
		u64 clause_22_extension_present      : 1;
		u64 marvell_specific_device_1_present : 1;
		u64 marvell_specific_device_2_present : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_auto_negotiation_devices_in_package_2_s cn; */
};

static inline u64 RPMX_ANX_AUTO_NEGOTIATION_DEVICES_IN_PACKAGE_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_AUTO_NEGOTIATION_DEVICES_IN_PACKAGE_2(u64 a)
{
	return 0x48030 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_auto_negotiation_package_identifier_1
 *
 * RPM An  Auto Negotiation Package Identifier 1 Register
 */
union rpmx_anx_auto_negotiation_package_identifier_1 {
	u64 u;
	struct rpmx_anx_auto_negotiation_package_identifier_1_s {
		u64 organizationally_unique_pkg_identifier_bit_318 : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_auto_negotiation_package_identifier_1_s cn; */
};

static inline u64 RPMX_ANX_AUTO_NEGOTIATION_PACKAGE_IDENTIFIER_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_AUTO_NEGOTIATION_PACKAGE_IDENTIFIER_1(u64 a)
{
	return 0x48070 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_auto_negotiation_package_identifier_2
 *
 * RPM An  Auto Negotiation Package Identifier 2 Register
 */
union rpmx_anx_auto_negotiation_package_identifier_2 {
	u64 u;
	struct rpmx_anx_auto_negotiation_package_identifier_2_s {
		u64 pkg_revision_number              : 4;
		u64 pkg_model_number                 : 6;
		u64 pkg_organizationally_unique_identifier_bit_1924 : 6;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_auto_negotiation_package_identifier_2_s cn; */
};

static inline u64 RPMX_ANX_AUTO_NEGOTIATION_PACKAGE_IDENTIFIER_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_AUTO_NEGOTIATION_PACKAGE_IDENTIFIER_2(u64 a)
{
	return 0x48078 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_backplane_ethernet_status2_register
 *
 * RPM An  Backplane Ethernet Status2 Register Register
 */
union rpmx_anx_backplane_ethernet_status2_register {
	u64 u;
	struct rpmx_anx_backplane_ethernet_status2_register_s {
		u64 field_50gbase_krcr               : 1;
		u64 field_100gbase_krcr2             : 1;
		u64 field_200gbase_krcr4             : 1;
		u64 field_100gbase_krcr              : 1;
		u64 field_200gbase_krcr2             : 1;
		u64 field_400gbase_krcr4             : 1;
		u64 reserved_6_63                    : 58;
	} s;
	/* struct rpmx_anx_backplane_ethernet_status2_register_s cn; */
};

static inline u64 RPMX_ANX_BACKPLANE_ETHERNET_STATUS2_REGISTER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_BACKPLANE_ETHERNET_STATUS2_REGISTER(u64 a)
{
	return 0x48188 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_backplane_ethernet_status_register
 *
 * RPM An  Backplane Ethernet Status Register Register
 */
union rpmx_anx_backplane_ethernet_status_register {
	u64 u;
	struct rpmx_anx_backplane_ethernet_status_register_s {
		u64 stat_bp_an_ability               : 1;
		u64 stat_field_1000base_kx           : 1;
		u64 stat_field_10gbase_kx4           : 1;
		u64 stat_field_10gbase_kr            : 1;
		u64 stat_base_r_fec_negotiated       : 1;
		u64 stat_field_40gbase_kr4           : 1;
		u64 stat_field_40gbase_cr4           : 1;
		u64 stat_rs_fec_negotiated           : 1;
		u64 stat_field_100gbase_cr10         : 1;
		u64 stat_field_100gbase_kp4          : 1;
		u64 stat_field_100gbase_kr4          : 1;
		u64 stat_field_100gbase_cr4          : 1;
		u64 stat_field_25gbase_kr_s_or_25gbase_cr_s : 1;
		u64 stat_field_25gbase_kr_or_25gbase_cr : 1;
		u64 stat_field_2_5gbase_kx           : 1;
		u64 stat_field_5gbase_kr             : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_backplane_ethernet_status_register_s cn; */
};

static inline u64 RPMX_ANX_BACKPLANE_ETHERNET_STATUS_REGISTER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_BACKPLANE_ETHERNET_STATUS_REGISTER(u64 a)
{
	return 0x48180 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_auto_negotiation_control
 *
 * RPM An  Reg 802 3ap Auto Negotiation Control Register
 */
union rpmx_anx_reg_802_3ap_auto_negotiation_control {
	u64 u;
	struct rpmx_anx_reg_802_3ap_auto_negotiation_control_s {
		u64 control_reserved                 : 9;
		u64 restart_802_3ap_auto_negotiation : 1;
		u64 reserved_10_11                   : 2;
		u64 field_802_3ap_auto_negotiation_enable : 1;
		u64 extended_next_page_control       : 1;
		u64 reserved_14                      : 1;
		u64 my_reset                         : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_auto_negotiation_control_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_AUTO_NEGOTIATION_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_AUTO_NEGOTIATION_CONTROL(u64 a)
{
	return 0x48000 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_auto_negotiation_status
 *
 * RPM An  Reg 802 3ap Auto Negotiation Status Register
 */
union rpmx_anx_reg_802_3ap_auto_negotiation_status {
	u64 u;
	struct rpmx_anx_reg_802_3ap_auto_negotiation_status_s {
		u64 link_partner_auto_negotiation_ability : 1;
		u64 reserved_1                       : 1;
		u64 field_802_3ap_link_status        : 1;
		u64 field_802_3ap_auto_negotiation_ability : 1;
		u64 stat_remote_fault                : 1;
		u64 field_802_3ap_auto_negotiation_complete : 1;
		u64 page_received                    : 1;
		u64 extended_next_page_status        : 1;
		u64 reserved_8                       : 1;
		u64 field_802_3ap_parallel_detection_fault : 1;
		u64 reserved_10_63                   : 54;
	} s;
	/* struct rpmx_anx_reg_802_3ap_auto_negotiation_status_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_AUTO_NEGOTIATION_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_AUTO_NEGOTIATION_STATUS(u64 a)
{
	return 0x48008 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_extended_next_page_transmit_register_unformatted_code_field_u0_to_u15
 *
 * RPM An  Reg 802 3ap Extended Next Page Transmit Register Unformatted
 * Code Field U0 To U15 Register
 */
union rpmx_anx_reg_802_3ap_extended_next_page_transmit_register_unformatted_code_field_u0_to_u15 {
	u64 u;
	struct rpmx_anx_reg_802_3ap_extended_next_page_transmit_register_unformatted_code_field_u0_to_u15_s {
		u64 ld_unformatted_field0_15         : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_extended_next_page_transmit_register_unformatted_code_field_u0_to_u15_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_EXTENDED_NEXT_PAGE_TRANSMIT_REGISTER_UNFORMATTED_CODE_FIELD_U0_TO_U15(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_EXTENDED_NEXT_PAGE_TRANSMIT_REGISTER_UNFORMATTED_CODE_FIELD_U0_TO_U15(u64 a)
{
	return 0x480b8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_extended_next_page_transmit_register_unformatted_code_field_u16_to_u31
 *
 * RPM An  Reg 802 3ap Extended Next Page Transmit Register Unformatted
 * Code Field U16 To U31 Register
 */
union rpmx_anx_reg_802_3ap_extended_next_page_transmit_register_unformatted_code_field_u16_to_u31 {
	u64 u;
	struct rpmx_anx_reg_802_3ap_extended_next_page_transmit_register_unformatted_code_field_u16_to_u31_s {
		u64 ld_unformatted_field16_31        : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_extended_next_page_transmit_register_unformatted_code_field_u16_to_u31_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_EXTENDED_NEXT_PAGE_TRANSMIT_REGISTER_UNFORMATTED_CODE_FIELD_U16_TO_U31(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_EXTENDED_NEXT_PAGE_TRANSMIT_REGISTER_UNFORMATTED_CODE_FIELD_U16_TO_U31(u64 a)
{
	return 0x480c0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_link_partner_base_page_ability_register_1
 *
 * RPM An  Reg 802 3ap Link Partner Base Page Ability Register 1 Register
 */
union rpmx_anx_reg_802_3ap_link_partner_base_page_ability_register_1 {
	u64 u;
	struct rpmx_anx_reg_802_3ap_link_partner_base_page_ability_register_1_s {
		u64 lp_selector_field                : 5;
		u64 lp_echoed_nonce_field            : 5;
		u64 lp_pause_capable                 : 1;
		u64 lp_asymmetric_pause              : 1;
		u64 reserved_12                      : 1;
		u64 lp_remote_fault                  : 1;
		u64 lp_acknowledge                   : 1;
		u64 lp_next_page                     : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_link_partner_base_page_ability_register_1_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_BASE_PAGE_ABILITY_REGISTER_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_BASE_PAGE_ABILITY_REGISTER_1(u64 a)
{
	return 0x48098 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_link_partner_base_page_ability_register_2
 *
 * RPM An  Reg 802 3ap Link Partner Base Page Ability Register 2 Register
 */
union rpmx_anx_reg_802_3ap_link_partner_base_page_ability_register_2 {
	u64 u;
	struct rpmx_anx_reg_802_3ap_link_partner_base_page_ability_register_2_s {
		u64 lp_link_partner_transmitted_nonce_field : 5;
		u64 lp_1000base_kx                   : 1;
		u64 lp_10gbase_kx4                   : 1;
		u64 lp_10gbase_kr                    : 1;
		u64 lp_40gbase_kr4                   : 1;
		u64 lp_40gbase_cr4                   : 1;
		u64 lp_100gbase_cr10                 : 1;
		u64 lp_100gbase_kp4                  : 1;
		u64 lp_100gbase_kr4                  : 1;
		u64 lp_100gbase_cr4                  : 1;
		u64 lp_25gbase_kr_s_or_25gbase_cr_s  : 1;
		u64 lp_25gbase_kr_or_25gbase_cr      : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_link_partner_base_page_ability_register_2_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_BASE_PAGE_ABILITY_REGISTER_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_BASE_PAGE_ABILITY_REGISTER_2(u64 a)
{
	return 0x480a0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_link_partner_base_page_ability_register_3
 *
 * RPM An  Reg 802 3ap Link Partner Base Page Ability Register 3 Register
 */
union rpmx_anx_reg_802_3ap_link_partner_base_page_ability_register_3 {
	u64 u;
	struct rpmx_anx_reg_802_3ap_link_partner_base_page_ability_register_3_s {
		u64 lp_2_5g_kx                       : 1;
		u64 lp_5g_kr                         : 1;
		u64 lp_50g_krcr                      : 1;
		u64 lp_100g_krcr2                    : 1;
		u64 lp_200g_krcr4                    : 1;
		u64 reserved_5_8                     : 4;
		u64 lp_25g_r2_marvell_mode           : 1;
		u64 lp_200g_krcr8                    : 1;
		u64 lp_50g_r4_marvell_mode           : 1;
		u64 lp_25g_rs_fec_requestedf2        : 1;
		u64 lp_25g_base_r_requestedf3        : 1;
		u64 lp_link_partner_fec_ability      : 1;
		u64 lp_link_partner_requesting_fec_enable : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_link_partner_base_page_ability_register_3_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_BASE_PAGE_ABILITY_REGISTER_3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_BASE_PAGE_ABILITY_REGISTER_3(u64 a)
{
	return 0x480a8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_link_partner_extended_next_page_ability_register_unformatted_code_field_u0_to_u15
 *
 * RPM An  Reg 802 3ap Link Partner Extended Next Page Ability Register
 * Unformatted Code Field U0 To U15 Register
 */
union rpmx_anx_reg_802_3ap_link_partner_extended_next_page_ability_register_unformatted_code_field_u0_to_u15 {
	u64 u;
	struct rpmx_anx_reg_802_3ap_link_partner_extended_next_page_ability_register_unformatted_code_field_u0_to_u15_s {
		u64 lp_unformatted_field0_15         : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_link_partner_extended_next_page_ability_register_unformatted_code_field_u0_to_u15_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_EXTENDED_NEXT_PAGE_ABILITY_REGISTER_UNFORMATTED_CODE_FIELD_U0_TO_U15(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_EXTENDED_NEXT_PAGE_ABILITY_REGISTER_UNFORMATTED_CODE_FIELD_U0_TO_U15(u64 a)
{
	return 0x480d0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_link_partner_extended_next_page_ability_register_unformatted_code_field_u16_to_u31
 *
 * RPM An  Reg 802 3ap Link Partner Extended Next Page Ability Register
 * Unformatted Code Field U16 To U31 Register
 */
union rpmx_anx_reg_802_3ap_link_partner_extended_next_page_ability_register_unformatted_code_field_u16_to_u31 {
	u64 u;
	struct rpmx_anx_reg_802_3ap_link_partner_extended_next_page_ability_register_unformatted_code_field_u16_to_u31_s {
		u64 lp_unformatted_field16_31        : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_link_partner_extended_next_page_ability_register_unformatted_code_field_u16_to_u31_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_EXTENDED_NEXT_PAGE_ABILITY_REGISTER_UNFORMATTED_CODE_FIELD_U16_TO_U31(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_EXTENDED_NEXT_PAGE_ABILITY_REGISTER_UNFORMATTED_CODE_FIELD_U16_TO_U31(u64 a)
{
	return 0x480d8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_link_partner_next_page_register_link_partner_extended_next_page_ability_register
 *
 * RPM An  Reg 802 3ap Link Partner Next Page Register Link Partner
 * Extended Next Page Ability Register Register
 */
union rpmx_anx_reg_802_3ap_link_partner_next_page_register_link_partner_extended_next_page_ability_register {
	u64 u;
	struct rpmx_anx_reg_802_3ap_link_partner_next_page_register_link_partner_extended_next_page_ability_register_s {
		u64 lp_np_messageunformatted_field   : 11;
		u64 lp_np_toggle                     : 1;
		u64 lp_np_acknowledge2               : 1;
		u64 lp_np_message_page               : 1;
		u64 lp_np_acknowledge                : 1;
		u64 lp_np_next_page                  : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_link_partner_next_page_register_link_partner_extended_next_page_ability_register_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_NEXT_PAGE_REGISTER_LINK_PARTNER_EXTENDED_NEXT_PAGE_ABILITY_REGISTER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_LINK_PARTNER_NEXT_PAGE_REGISTER_LINK_PARTNER_EXTENDED_NEXT_PAGE_ABILITY_REGISTER(u64 a)
{
	return 0x480c8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_local_device_base_page_ability_register_1
 *
 * RPM An  Reg 802 3ap Local Device Base Page Ability Register 1 Register
 */
union rpmx_anx_reg_802_3ap_local_device_base_page_ability_register_1 {
	u64 u;
	struct rpmx_anx_reg_802_3ap_local_device_base_page_ability_register_1_s {
		u64 ld_selector_field                : 5;
		u64 ld_echoed_nonce_field            : 5;
		u64 ld_pause_capable                 : 1;
		u64 ld_asymmetric_pause              : 1;
		u64 reserved_12                      : 1;
		u64 ld_remote_fault                  : 1;
		u64 ld_acknowledge                   : 1;
		u64 ld_next_page                     : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_local_device_base_page_ability_register_1_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_LOCAL_DEVICE_BASE_PAGE_ABILITY_REGISTER_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_LOCAL_DEVICE_BASE_PAGE_ABILITY_REGISTER_1(u64 a)
{
	return 0x48080 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_local_device_base_page_ability_register_2
 *
 * RPM An  Reg 802 3ap Local Device Base Page Ability Register 2 Register
 */
union rpmx_anx_reg_802_3ap_local_device_base_page_ability_register_2 {
	u64 u;
	struct rpmx_anx_reg_802_3ap_local_device_base_page_ability_register_2_s {
		u64 ld_link_partner_transmitted_nonce_field : 5;
		u64 ld_1000base_kx                   : 1;
		u64 ld_10gbase_kx4                   : 1;
		u64 ld_10gbase_kr                    : 1;
		u64 ld_40gbase_kr4                   : 1;
		u64 ld_40gbase_cr4                   : 1;
		u64 ld_100gbase_cr10                 : 1;
		u64 ld_100gbase_kp4                  : 1;
		u64 ld_100gbase_kr4                  : 1;
		u64 ld_100gbase_cr4                  : 1;
		u64 ld_25gbase_kr_s_or_25gbase_cr_s  : 1;
		u64 ld_25gbase_kr_or_25gbase_cr      : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_local_device_base_page_ability_register_2_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_LOCAL_DEVICE_BASE_PAGE_ABILITY_REGISTER_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_LOCAL_DEVICE_BASE_PAGE_ABILITY_REGISTER_2(u64 a)
{
	return 0x48088 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_local_device_base_page_ability_register_3
 *
 * RPM An  Reg 802 3ap Local Device Base Page Ability Register 3 Register
 */
union rpmx_anx_reg_802_3ap_local_device_base_page_ability_register_3 {
	u64 u;
	struct rpmx_anx_reg_802_3ap_local_device_base_page_ability_register_3_s {
		u64 ld_2_5g_kx                       : 1;
		u64 ld_5g_kr                         : 1;
		u64 ld_50g_krcr                      : 1;
		u64 ld_100g_krcr2                    : 1;
		u64 ld_200g_krcr4                    : 1;
		u64 reserved_5_8                     : 4;
		u64 ld_25g_r2_marvell_mode           : 1;
		u64 ld_200g_krcr8                    : 1;
		u64 ld_50g_r4_marvell_mode           : 1;
		u64 ld_25g_rs_fec_requestedf2        : 1;
		u64 ld_25g_base_r_requestedf3        : 1;
		u64 ld_link_partner_fec_ability      : 1;
		u64 ld_link_partner_requesting_fec_enable : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_local_device_base_page_ability_register_3_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_LOCAL_DEVICE_BASE_PAGE_ABILITY_REGISTER_3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_LOCAL_DEVICE_BASE_PAGE_ABILITY_REGISTER_3(u64 a)
{
	return 0x48090 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_an#_reg_802_3ap_next_page_transmit_register_extended_next_page_transmit_register
 *
 * RPM An  Reg 802 3ap Next Page Transmit Register Extended Next Page
 * Transmit Register Register
 */
union rpmx_anx_reg_802_3ap_next_page_transmit_register_extended_next_page_transmit_register {
	u64 u;
	struct rpmx_anx_reg_802_3ap_next_page_transmit_register_extended_next_page_transmit_register_s {
		u64 ld_np_messageunformatted_field   : 11;
		u64 ld_np_toggle                     : 1;
		u64 ld_np_acknowledge_2              : 1;
		u64 ld_np_message_page_mode          : 1;
		u64 reserved_14                      : 1;
		u64 ld_np_next_page                  : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anx_reg_802_3ap_next_page_transmit_register_extended_next_page_transmit_register_s cn; */
};

static inline u64 RPMX_ANX_REG_802_3AP_NEXT_PAGE_TRANSMIT_REGISTER_EXTENDED_NEXT_PAGE_TRANSMIT_REGISTER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANX_REG_802_3AP_NEXT_PAGE_TRANSMIT_REGISTER_EXTENDED_NEXT_PAGE_TRANSMIT_REGISTER(u64 a)
{
	return 0x480b0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_anp_global_an_abilities
 *
 * RPM Anp Global An Abilities Register Set default values of internal
 * PACKAGE registers.
 */
union rpmx_anp_global_an_abilities {
	u64 u;
	struct rpmx_anp_global_an_abilities_s {
		u64 runi_cfg_ability1_a              : 16;
		u64 runi_cfg_ability2_a              : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_an_abilities_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_AN_ABILITIES(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_AN_ABILITIES(void)
{
	return 0x580e0;
}

/**
 * Register (RSL) rpm#_anp_global_an_revision
 *
 * RPM Anp Global An Revision Register Set default value for internal
 * IDENTIFIER registers.
 */
union rpmx_anp_global_an_revision {
	u64 u;
	struct rpmx_anp_global_an_revision_s {
		u64 runi_oui_num_3_18_a              : 16;
		u64 runi_cfg_rev_id_a                : 4;
		u64 runi_cfg_model_num_a             : 6;
		u64 runi_oui_num_19_24_a             : 6;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_an_revision_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_AN_REVISION(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_AN_REVISION(void)
{
	return 0x580e8;
}

/**
 * Register (RSL) rpm#_anp_global_an_train_type
 *
 * RPM Anp Global An Train Type Register Set in advance the train type
 * for any possible resolved speed. train_type can be: TX train (KR/TRX)
 * , RX train and no train.
 */
union rpmx_anp_global_an_train_type {
	u64 u;
	struct rpmx_anp_global_an_train_type_s {
		u64 an_train_type_mode_1g            : 2;
		u64 an_train_type_mode_2p5g          : 2;
		u64 an_train_type_mode_5g            : 2;
		u64 an_train_type_mode_10g           : 2;
		u64 an_train_type_mode_25g           : 2;
		u64 an_train_type_mode_40gr4         : 2;
		u64 an_train_type_mode_40gr2         : 2;
		u64 an_train_type_mode_50gr2         : 2;
		u64 an_train_type_mode_50g           : 2;
		u64 an_train_type_mode_100gr4        : 2;
		u64 an_train_type_mode_100gr2        : 2;
		u64 an_train_type_mode_200gr4        : 2;
		u64 an_train_type_mode_400gr8        : 2;
		u64 an_train_type_mode_200gr8        : 2;
		u64 reserved_28_63                   : 36;
	} s;
	/* struct rpmx_anp_global_an_train_type_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_AN_TRAIN_TYPE(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_AN_TRAIN_TYPE(void)
{
	return 0x580f0;
}

/**
 * Register (RSL) rpm#_anp_global_clock_and_reset
 *
 * RPM Anp Global Clock And Reset Register Global clock and reset
 * control. Controls PWM clock & reset. Controls HW/SW reset for each AN
 * in the ANP.
 */
union rpmx_anp_global_clock_and_reset {
	u64 u;
	struct rpmx_anp_global_clock_and_reset_s {
		u64 pwm_soft_reset_n                 : 1;
		u64 pwm_clk_en                       : 1;
		u64 an_hw_soft_reset_n               : 1;
		u64 an_sw_soft_reset_n               : 1;
		u64 p1_an_hw_soft_reset_n            : 1;
		u64 p1_an_sw_soft_reset_n            : 1;
		u64 p2_an_hw_soft_reset_n            : 1;
		u64 p2_an_sw_soft_reset_n            : 1;
		u64 p3_an_hw_soft_reset_n            : 1;
		u64 p3_an_sw_soft_reset_n            : 1;
		u64 p4_an_hw_soft_reset_n            : 1;
		u64 p4_an_sw_soft_reset_n            : 1;
		u64 p5_an_hw_soft_reset_n            : 1;
		u64 p5_an_sw_soft_reset_n            : 1;
		u64 p6_an_hw_soft_reset_n            : 1;
		u64 p6_an_sw_soft_reset_n            : 1;
		u64 p7_an_hw_soft_reset_n            : 1;
		u64 p7_an_sw_soft_reset_n            : 1;
		u64 reserved_18_63                   : 46;
	} s;
	/* struct rpmx_anp_global_clock_and_reset_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_CLOCK_AND_RESET(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_CLOCK_AND_RESET(void)
{
	return 0x58008;
}

/**
 * Register (RSL) rpm#_anp_global_control
 *
 * RPM Anp Global Control Register Global configurations which are common
 * to all ports in single ANP.
 */
union rpmx_anp_global_control {
	u64 u;
	struct rpmx_anp_global_control_s {
		u64 an_ap_train_type                 : 2;
		u64 reg_txclk_sync_en_width_s        : 8;
		u64 reg_train_type_samp              : 2;
		u64 reg_train_type_mx_samp           : 2;
		u64 reg_reset_pulse_conf_delay       : 10;
		u64 reg_ch_sm_cnt_saturate           : 1;
		u64 reg_txstr_rxsd_clear_cnt         : 1;
		u64 reg_tx_sm_cnt_saturate           : 1;
		u64 reg_rx_sm_cnt_saturate           : 1;
		u64 reg_tx_ready_loss_latch_en       : 1;
		u64 reg_dsp_sigdet_loss_latch_en     : 1;
		u64 reg_dsp_lock_loss_latch_en       : 1;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_global_control_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_CONTROL(void)
{
	return 0x58000;
}

/**
 * Register (RSL) rpm#_anp_global_control2
 *
 * RPM Anp Global Control2 Register Global configurations which are
 * common to all ports in single ANP.
 */
union rpmx_anp_global_control2 {
	u64 u;
	struct rpmx_anp_global_control2_s {
		u64 reg_pu_tx_conf_delay             : 4;
		u64 reg_pu_rx_conf_delay             : 4;
		u64 reg_dsp_on_conf_delay            : 5;
		u64 reg_tx_idle_conf_dly             : 9;
		u64 reg_pcs_tx_on_conf_delay         : 5;
		u64 reg_pcs_rx_on_conf_delay         : 5;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_control2_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_CONTROL2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_CONTROL2(void)
{
	return 0x58140;
}

/**
 * Register (RSL) rpm#_anp_global_control3
 *
 * RPM Anp Global Control3 Register Global configurations which are
 * common to all ports in single ANP.
 */
union rpmx_anp_global_control3 {
	u64 u;
	struct rpmx_anp_global_control3_s {
		u64 reg_no_pre_selector              : 1;
		u64 reg_hcd_resolved_clean           : 1;
		u64 reg_reset_to_clock_cnt           : 6;
		u64 reg_clock_to_reset_cnt           : 6;
		u64 reg_start_ap_mode_cnt            : 6;
		u64 reg_start_selector_cnt           : 6;
		u64 reg_start_ungate_cnt             : 6;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_control3_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_CONTROL3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_CONTROL3(void)
{
	return 0x58148;
}

/**
 * Register (RSL) rpm#_anp_global_control4
 *
 * RPM Anp Global Control4 Register Global configurations which are
 * common to all ports in single ANP.
 */
union rpmx_anp_global_control4 {
	u64 u;
	struct rpmx_anp_global_control4_s {
		u64 reg_sd_dfe_update_dis_samp       : 2;
		u64 reg_sd_dfe_pat_dis_samp          : 2;
		u64 reg_sd_dfe_en_samp               : 2;
		u64 reg_sd_tx_idle_samp              : 2;
		u64 reg_tx_train_enable_samp         : 2;
		u64 reg_rx_train_enable_samp         : 2;
		u64 reg_rx_init_samp                 : 2;
		u64 reg_rx_idle_count                : 1;
		u64 reg_tx_idle_count                : 1;
		u64 phy_gen_tx_done_ow               : 1;
		u64 phy_gen_tx_done_ow_val           : 1;
		u64 phy_gen_tx_load_ow               : 1;
		u64 phy_gen_tx_load_ow_val           : 1;
		u64 phy_gen_tx_start_cnt_done_ow     : 1;
		u64 phy_gen_tx_start_cnt_done_ow_val : 1;
		u64 phy_gen_rx_done_ow               : 1;
		u64 phy_gen_rx_done_ow_val           : 1;
		u64 phy_gen_rx_load_ow               : 1;
		u64 phy_gen_rx_load_ow_val           : 1;
		u64 phy_gen_rx_start_cnt_done_ow     : 1;
		u64 phy_gen_rx_start_cnt_done_ow_val : 1;
		u64 reg_phy_gen_dn_frc               : 1;
		u64 reg_txstr_regret_enable          : 1;
		u64 reg_rxstr_regret_enable          : 1;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_global_control4_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_CONTROL4(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_CONTROL4(void)
{
	return 0x58150;
}

/**
 * Register (RSL) rpm#_anp_global_interrupt_cause
 *
 * RPM Anp Global Interrupt Cause Register Global Interrupts register.
 */
union rpmx_anp_global_interrupt_cause {
	u64 u;
	struct rpmx_anp_global_interrupt_cause_s {
		u64 global_int_sum                   : 1;
		u64 illegal_address_access           : 1;
		u64 reserved_2_63                    : 62;
	} s;
	/* struct rpmx_anp_global_interrupt_cause_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_INTERRUPT_CAUSE(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_INTERRUPT_CAUSE(void)
{
	return 0x58110;
}

/**
 * Register (RSL) rpm#_anp_global_interrupt_mask
 *
 * RPM Anp Global Interrupt Mask Register Mask for Global interrupts
 * register.
 */
union rpmx_anp_global_interrupt_mask {
	u64 u;
	struct rpmx_anp_global_interrupt_mask_s {
		u64 reserved_0                       : 1;
		u64 global_int_mask                  : 1;
		u64 reserved_2_63                    : 62;
	} s;
	/* struct rpmx_anp_global_interrupt_mask_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_INTERRUPT_MASK(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_INTERRUPT_MASK(void)
{
	return 0x58118;
}

/**
 * Register (RSL) rpm#_anp_global_lane_delay
 *
 * RPM Anp Global Lane Delay Register Provides delay between the power up
 * of the different COMPHYs in a single QUAD.
 */
union rpmx_anp_global_lane_delay {
	u64 u;
	struct rpmx_anp_global_lane_delay_s {
		u64 lane_delay                       : 25;
		u64 reserved_25_63                   : 39;
	} s;
	/* struct rpmx_anp_global_lane_delay_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_LANE_DELAY(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_LANE_DELAY(void)
{
	return 0x58020;
}

/**
 * Register (RSL) rpm#_anp_global_phy_gen_load_to_pup_timer
 *
 * RPM Anp Global Phy Gen Load To Pup Timer Register Global
 * configurations which are common to all ports in single ANP.
 */
union rpmx_anp_global_phy_gen_load_to_pup_timer {
	u64 u;
	struct rpmx_anp_global_phy_gen_load_to_pup_timer_s {
		u64 reg_phy_gen_load_to_pup_timer    : 24;
		u64 reserved_24_63                   : 40;
	} s;
	/* struct rpmx_anp_global_phy_gen_load_to_pup_timer_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_PHY_GEN_LOAD_TO_PUP_TIMER(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_PHY_GEN_LOAD_TO_PUP_TIMER(void)
{
	return 0x58170;
}

/**
 * Register (RSL) rpm#_anp_global_phy_gen_pdn_to_load_timer
 *
 * RPM Anp Global Phy Gen Pdn To Load Timer Register Global
 * configurations which are common to all ports in single ANP.
 */
union rpmx_anp_global_phy_gen_pdn_to_load_timer {
	u64 u;
	struct rpmx_anp_global_phy_gen_pdn_to_load_timer_s {
		u64 reg_phy_gen_pdn_to_load_timer    : 24;
		u64 reserved_24_63                   : 40;
	} s;
	/* struct rpmx_anp_global_phy_gen_pdn_to_load_timer_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_PHY_GEN_PDN_TO_LOAD_TIMER(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_PHY_GEN_PDN_TO_LOAD_TIMER(void)
{
	return 0x58168;
}

/**
 * Register (RSL) rpm#_anp_global_sd_mux_control
 *
 * RPM Anp Global Sd Mux Control Register When SD_MUX is present, this
 * register provides the control for the port-sd muxing.
 */
union rpmx_anp_global_sd_mux_control {
	u64 u;
	struct rpmx_anp_global_sd_mux_control_s {
		u64 sd_mux_control0                  : 8;
		u64 sd_mux_control1                  : 8;
		u64 sd_mux_control2                  : 8;
		u64 sd_mux_control3                  : 8;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_sd_mux_control_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_SD_MUX_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_SD_MUX_CONTROL(void)
{
	return 0x58010;
}

/**
 * Register (RSL) rpm#_anp_global_sd_mux_control1
 *
 * RPM Anp Global Sd Mux Control1 Register When SD_MUX is present, this
 * register provides the control for the port-sd muxing.
 */
union rpmx_anp_global_sd_mux_control1 {
	u64 u;
	struct rpmx_anp_global_sd_mux_control1_s {
		u64 sd_mux_control4                  : 8;
		u64 sd_mux_control5                  : 8;
		u64 sd_mux_control6                  : 8;
		u64 sd_mux_control7                  : 8;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_sd_mux_control1_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_SD_MUX_CONTROL1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_SD_MUX_CONTROL1(void)
{
	return 0x58018;
}

/**
 * Register (RSL) rpm#_anp_global_sd_rx_idle_min_wait
 *
 * RPM Anp Global Sd Rx Idle Min Wait Register Global configurations
 * which are common to all ports in single ANP.
 */
union rpmx_anp_global_sd_rx_idle_min_wait {
	u64 u;
	struct rpmx_anp_global_sd_rx_idle_min_wait_s {
		u64 reg_rx_idle_min_wait_s           : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_global_sd_rx_idle_min_wait_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_SD_RX_IDLE_MIN_WAIT(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_SD_RX_IDLE_MIN_WAIT(void)
{
	return 0x58160;
}

/**
 * Register (RSL) rpm#_anp_global_sd_tx_idle_min_wait
 *
 * RPM Anp Global Sd Tx Idle Min Wait Register Global configurations
 * which are common to all ports in single ANP.
 */
union rpmx_anp_global_sd_tx_idle_min_wait {
	u64 u;
	struct rpmx_anp_global_sd_tx_idle_min_wait_s {
		u64 reg_tx_idle_min_wait_s           : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_global_sd_tx_idle_min_wait_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_SD_TX_IDLE_MIN_WAIT(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_SD_TX_IDLE_MIN_WAIT(void)
{
	return 0x58158;
}

/**
 * Register (RSL) rpm#_anp_global_speed_table
 *
 * RPM Anp Global Speed Table Register Speed table values per frequency.
 */
union rpmx_anp_global_speed_table {
	u64 u;
	struct rpmx_anp_global_speed_table_s {
		u64 pm_speed_table_1p25g             : 5;
		u64 pm_speed_table_2p578125g         : 5;
		u64 pm_speed_table_3p125g            : 5;
		u64 pm_speed_table_5g                : 5;
		u64 pm_speed_table_5p15625g          : 5;
		u64 pm_speed_table_10g               : 5;
		u64 reserved_30_63                   : 34;
	} s;
	/* struct rpmx_anp_global_speed_table_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_SPEED_TABLE(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_SPEED_TABLE(void)
{
	return 0x580f8;
}

/**
 * Register (RSL) rpm#_anp_global_speed_table1
 *
 * RPM Anp Global Speed Table1 Register Speed table values per frequency.
 */
union rpmx_anp_global_speed_table1 {
	u64 u;
	struct rpmx_anp_global_speed_table1_s {
		u64 pm_speed_table_10p3125g          : 5;
		u64 pm_speed_table_20p625g           : 5;
		u64 pm_speed_table_25p78125g         : 5;
		u64 pm_speed_table_26p5625g          : 5;
		u64 pm_speed_table_27p5g             : 5;
		u64 pm_speed_table_53p125g           : 5;
		u64 reserved_30_63                   : 34;
	} s;
	/* struct rpmx_anp_global_speed_table1_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_SPEED_TABLE1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_SPEED_TABLE1(void)
{
	return 0x58100;
}

/**
 * Register (RSL) rpm#_anp_global_speed_table2
 *
 * RPM Anp Global Speed Table2 Register Speed table values per frequency.
 */
union rpmx_anp_global_speed_table2 {
	u64 u;
	struct rpmx_anp_global_speed_table2_s {
		u64 pm_speed_table_56g               : 5;
		u64 reserved_5_63                    : 59;
	} s;
	/* struct rpmx_anp_global_speed_table2_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_SPEED_TABLE2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_SPEED_TABLE2(void)
{
	return 0x58108;
}

/**
 * Register (RSL) rpm#_anp_global_timer_100ms
 *
 * RPM Anp Global Timer 100ms Register Value for 100ms timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_100ms {
	u64 u;
	struct rpmx_anp_global_timer_100ms_s {
		u64 rg_an_timers_100ms               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_100ms_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_100MS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_100MS(void)
{
	return 0x58050;
}

/**
 * Register (RSL) rpm#_anp_global_timer_10ms
 *
 * RPM Anp Global Timer 10ms Register Value for 10ms timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_10ms {
	u64 u;
	struct rpmx_anp_global_timer_10ms_s {
		u64 rg_an_timers_10ms                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_10ms_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_10MS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_10MS(void)
{
	return 0x58028;
}

/**
 * Register (RSL) rpm#_anp_global_timer_12600ms_20s_high
 *
 * RPM Anp Global Timer 12600ms 20s High Register Values for 12600ms/20s
 * timers according to system clock (allowed range 100-300Mhz). *16 high
 * bits of the value*
 */
union rpmx_anp_global_timer_12600ms_20s_high {
	u64 u;
	struct rpmx_anp_global_timer_12600ms_20s_high_s {
		u64 rg_an_timers_12600ms_high        : 16;
		u64 rg_an_timers_20s_high            : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_12600ms_20s_high_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_12600MS_20S_HIGH(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_12600MS_20S_HIGH(void)
{
	return 0x580d0;
}

/**
 * Register (RSL) rpm#_anp_global_timer_12600ms_low
 *
 * RPM Anp Global Timer 12600ms Low Register Value for 12600ms timer
 * according to system clock (allowed range 100-300Mhz). *32 low bits of
 * the value*
 */
union rpmx_anp_global_timer_12600ms_low {
	u64 u;
	struct rpmx_anp_global_timer_12600ms_low_s {
		u64 rg_an_timers_12600ms_low         : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_12600ms_low_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_12600MS_LOW(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_12600MS_LOW(void)
{
	return 0x580a8;
}

/**
 * Register (RSL) rpm#_anp_global_timer_12s_low
 *
 * RPM Anp Global Timer 12s Low Register Value for 12s timer according to
 * system clock (allowed range 100-300Mhz). *32 low bits of the value*
 */
union rpmx_anp_global_timer_12s_low {
	u64 u;
	struct rpmx_anp_global_timer_12s_low_s {
		u64 rg_an_timers_12s_low             : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_12s_low_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_12S_LOW(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_12S_LOW(void)
{
	return 0x580a0;
}

/**
 * Register (RSL) rpm#_anp_global_timer_1s
 *
 * RPM Anp Global Timer 1s Register Value for 1s timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_1s {
	u64 u;
	struct rpmx_anp_global_timer_1s_s {
		u64 rg_an_timers_1s                  : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_1s_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_1S(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_1S(void)
{
	return 0x58070;
}

/**
 * Register (RSL) rpm#_anp_global_timer_200ms
 *
 * RPM Anp Global Timer 200ms Register Value for 200ms timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_200ms {
	u64 u;
	struct rpmx_anp_global_timer_200ms_s {
		u64 rg_an_timers_200ms               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_200ms_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_200MS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_200MS(void)
{
	return 0x58058;
}

/**
 * Register (RSL) rpm#_anp_global_timer_20s_low
 *
 * RPM Anp Global Timer 20s Low Register Value for 20s timer according to
 * system clock (allowed range 100-300Mhz). *32 low bits of the value*
 */
union rpmx_anp_global_timer_20s_low {
	u64 u;
	struct rpmx_anp_global_timer_20s_low_s {
		u64 rg_an_timers_20s_low             : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_20s_low_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_20S_LOW(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_20S_LOW(void)
{
	return 0x580b0;
}

/**
 * Register (RSL) rpm#_anp_global_timer_2s
 *
 * RPM Anp Global Timer 2s Register Value for 2s timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_2s {
	u64 u;
	struct rpmx_anp_global_timer_2s_s {
		u64 rg_an_timers_2s                  : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_2s_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_2S(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_2S(void)
{
	return 0x58078;
}

/**
 * Register (RSL) rpm#_anp_global_timer_300ms
 *
 * RPM Anp Global Timer 300ms Register Value for 300ms timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_300ms {
	u64 u;
	struct rpmx_anp_global_timer_300ms_s {
		u64 rg_an_timers_300ms               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_300ms_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_300MS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_300MS(void)
{
	return 0x58060;
}

/**
 * Register (RSL) rpm#_anp_global_timer_30s_40s_high
 *
 * RPM Anp Global Timer 30s 40s High Register Values for 30s/40s timers
 * according to system clock (allowed range 100-300Mhz). *16 high bits of
 * the value*
 */
union rpmx_anp_global_timer_30s_40s_high {
	u64 u;
	struct rpmx_anp_global_timer_30s_40s_high_s {
		u64 rg_an_timers_30s_high            : 16;
		u64 rg_an_timers_40s_high            : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_30s_40s_high_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_30S_40S_HIGH(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_30S_40S_HIGH(void)
{
	return 0x580d8;
}

/**
 * Register (RSL) rpm#_anp_global_timer_30s_low
 *
 * RPM Anp Global Timer 30s Low Register Value for 30s timer according to
 * system clock (allowed range 100-300Mhz). *32 low bits of the value*
 */
union rpmx_anp_global_timer_30s_low {
	u64 u;
	struct rpmx_anp_global_timer_30s_low_s {
		u64 rg_an_timers_30s_low             : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_30s_low_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_30S_LOW(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_30S_LOW(void)
{
	return 0x580b8;
}

/**
 * Register (RSL) rpm#_anp_global_timer_3150ms
 *
 * RPM Anp Global Timer 3150ms Register Value for 3150ms timer according
 * to system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_3150ms {
	u64 u;
	struct rpmx_anp_global_timer_3150ms_s {
		u64 rg_an_timers_3150ms              : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_3150ms_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_3150MS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_3150MS(void)
{
	return 0x58080;
}

/**
 * Register (RSL) rpm#_anp_global_timer_35ms
 *
 * RPM Anp Global Timer 35ms Register Value for 35ms timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_35ms {
	u64 u;
	struct rpmx_anp_global_timer_35ms_s {
		u64 rg_an_timers_35ms                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_35ms_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_35MS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_35MS(void)
{
	return 0x58030;
}

/**
 * Register (RSL) rpm#_anp_global_timer_40ms
 *
 * RPM Anp Global Timer 40ms Register Value for 40ms timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_40ms {
	u64 u;
	struct rpmx_anp_global_timer_40ms_s {
		u64 rg_an_timers_40ms                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_40ms_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_40MS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_40MS(void)
{
	return 0x58038;
}

/**
 * Register (RSL) rpm#_anp_global_timer_40s_low
 *
 * RPM Anp Global Timer 40s Low Register Value for 40s timer according to
 * system clock (allowed range 100-300Mhz). *32 low bits of the value*
 */
union rpmx_anp_global_timer_40s_low {
	u64 u;
	struct rpmx_anp_global_timer_40s_low_s {
		u64 rg_an_timers_40s_low             : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_40s_low_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_40S_LOW(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_40S_LOW(void)
{
	return 0x580c0;
}

/**
 * Register (RSL) rpm#_anp_global_timer_500ms
 *
 * RPM Anp Global Timer 500ms Register Value for 500ms timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_500ms {
	u64 u;
	struct rpmx_anp_global_timer_500ms_s {
		u64 rg_an_timers_500ms               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_500ms_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_500MS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_500MS(void)
{
	return 0x58068;
}

/**
 * Register (RSL) rpm#_anp_global_timer_50ms
 *
 * RPM Anp Global Timer 50ms Register Value for 50ms timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_50ms {
	u64 u;
	struct rpmx_anp_global_timer_50ms_s {
		u64 rg_an_timers_50ms                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_50ms_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_50MS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_50MS(void)
{
	return 0x58040;
}

/**
 * Register (RSL) rpm#_anp_global_timer_5s
 *
 * RPM Anp Global Timer 5s Register Value for 5s timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_5s {
	u64 u;
	struct rpmx_anp_global_timer_5s_s {
		u64 rg_an_timers_5s                  : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_5s_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_5S(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_5S(void)
{
	return 0x58088;
}

/**
 * Register (RSL) rpm#_anp_global_timer_6s
 *
 * RPM Anp Global Timer 6s Register Value for 6s timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_6s {
	u64 u;
	struct rpmx_anp_global_timer_6s_s {
		u64 rg_an_timers_6s                  : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_6s_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_6S(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_6S(void)
{
	return 0x58090;
}

/**
 * Register (RSL) rpm#_anp_global_timer_70ms
 *
 * RPM Anp Global Timer 70ms Register Value for 70ms timer according to
 * system clock (allowed range 100-300Mhz).
 */
union rpmx_anp_global_timer_70ms {
	u64 u;
	struct rpmx_anp_global_timer_70ms_s {
		u64 rg_an_timers_70ms                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_70ms_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_70MS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_70MS(void)
{
	return 0x58048;
}

/**
 * Register (RSL) rpm#_anp_global_timer_9s_12s_high
 *
 * RPM Anp Global Timer 9s 12s High Register Values for 9s/12s timers
 * according to system clock (allowed range 100-300Mhz). *16 high bits of
 * the value*
 */
union rpmx_anp_global_timer_9s_12s_high {
	u64 u;
	struct rpmx_anp_global_timer_9s_12s_high_s {
		u64 rg_an_timers_9s_high             : 16;
		u64 rg_an_timers_12s_high            : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_9s_12s_high_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_9S_12S_HIGH(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_9S_12S_HIGH(void)
{
	return 0x580c8;
}

/**
 * Register (RSL) rpm#_anp_global_timer_9s_low
 *
 * RPM Anp Global Timer 9s Low Register Value for 9s timer according to
 * system clock (allowed range 100-300Mhz). *32 low bits of the value*
 */
union rpmx_anp_global_timer_9s_low {
	u64 u;
	struct rpmx_anp_global_timer_9s_low_s {
		u64 rg_an_timers_9s_low              : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_global_timer_9s_low_s cn; */
};

static inline u64 RPMX_ANP_GLOBAL_TIMER_9S_LOW(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_GLOBAL_TIMER_9S_LOW(void)
{
	return 0x58098;
}

/**
 * Register (RSL) rpm#_anp_interrupt_summary_cause
 *
 * RPM Anp Interrupt Summary Cause Register Final interrupts register
 * (top of the tree) - summary goes out.
 */
union rpmx_anp_interrupt_summary_cause {
	u64 u;
	struct rpmx_anp_interrupt_summary_cause_s {
		u64 interrupt_cause_int_sum          : 1;
		u64 global_int                       : 1;
		u64 int_sum                          : 1;
		u64 int2_sum                         : 1;
		u64 p1_int_sum                       : 1;
		u64 p1_int2_sum                      : 1;
		u64 p2_int_sum                       : 1;
		u64 p2_int2_sum                      : 1;
		u64 p3_int_sum                       : 1;
		u64 p3_int2_sum                      : 1;
		u64 p4_int_sum                       : 1;
		u64 p4_int2_sum                      : 1;
		u64 p5_int_sum                       : 1;
		u64 p5_int2_sum                      : 1;
		u64 p6_int_sum                       : 1;
		u64 p6_int2_sum                      : 1;
		u64 p7_int_sum                       : 1;
		u64 p7_int2_sum                      : 1;
		u64 reserved_18_63                   : 46;
	} s;
	/* struct rpmx_anp_interrupt_summary_cause_s cn; */
};

static inline u64 RPMX_ANP_INTERRUPT_SUMMARY_CAUSE(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_INTERRUPT_SUMMARY_CAUSE(void)
{
	return 0x58128;
}

/**
 * Register (RSL) rpm#_anp_interrupt_summary_mask
 *
 * RPM Anp Interrupt Summary Mask Register Mask for Final interrupts
 * register.
 */
union rpmx_anp_interrupt_summary_mask {
	u64 u;
	struct rpmx_anp_interrupt_summary_mask_s {
		u64 reserved_0                       : 1;
		u64 interrupt_mask                   : 1;
		u64 interrupt1_mask                  : 1;
		u64 interrupt2_mask                  : 1;
		u64 p1_interrupt1_mask               : 1;
		u64 p1_interrupt2_mask               : 1;
		u64 p2_interrupt1_mask               : 1;
		u64 p2_interrupt2_mask               : 1;
		u64 p3_interrupt1_mask               : 1;
		u64 p3_interrupt2_mask               : 1;
		u64 p4_interrupt1_mask               : 1;
		u64 p4_interrupt2_mask               : 1;
		u64 p5_interrupt1_mask               : 1;
		u64 p5_interrupt2_mask               : 1;
		u64 p6_interrupt1_mask               : 1;
		u64 p6_interrupt2_mask               : 1;
		u64 p7_interrupt1_mask               : 1;
		u64 p7_interrupt2_mask               : 1;
		u64 reserved_18_63                   : 46;
	} s;
	/* struct rpmx_anp_interrupt_summary_mask_s cn; */
};

static inline u64 RPMX_ANP_INTERRUPT_SUMMARY_MASK(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_INTERRUPT_SUMMARY_MASK(void)
{
	return 0x58130;
}

/**
 * Register (RSL) rpm#_anp_last_violation
 *
 * RPM Anp Last Violation Register In case of access violation, contains
 * the address of the last violating transaction.
 */
union rpmx_anp_last_violation {
	u64 u;
	struct rpmx_anp_last_violation_s {
		u64 last_violation                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_last_violation_s cn; */
};

static inline u64 RPMX_ANP_LAST_VIOLATION(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_LAST_VIOLATION(void)
{
	return 0x58138;
}

/**
 * Register (RSL) rpm#_anp_metal_fix
 *
 * RPM Anp Metal Fix Register Reserved register for metal fix.
 */
union rpmx_anp_metal_fix {
	u64 u;
	struct rpmx_anp_metal_fix_s {
		u64 metal_fix                        : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_metal_fix_s cn; */
};

static inline u64 RPMX_ANP_METAL_FIX(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_METAL_FIX(void)
{
	return 0x58120;
}

/**
 * Register (RSL) rpm#_anp_port#_an_control
 *
 * RPM Anp Port An Control Register Control and overrides for AN related.
 */
union rpmx_anp_portx_an_control {
	u64 u;
	struct rpmx_anp_portx_an_control_s {
		u64 phy_gen_ap_ow                    : 1;
		u64 phy_gen_ap_ow_val                : 5;
		u64 pm_ap_mode_ow                    : 1;
		u64 pm_ap_mode_ow_val                : 1;
		u64 pm_ap_reset_rx_ow                : 1;
		u64 pm_ap_reset_rx_ow_val            : 1;
		u64 pm_enclk_ap_fr_ow                : 1;
		u64 pm_enclk_ap_fr_ow_val            : 1;
		u64 pm_norm_x_state_ow               : 1;
		u64 pm_norm_x_state_ow_val           : 1;
		u64 pm_enclk_ap_sys_ow               : 1;
		u64 pm_enclk_ap_sys_ow_val           : 1;
		u64 pm_ap_reset_tx_ow                : 1;
		u64 pm_ap_reset_tx_ow_val            : 1;
		u64 pm_enclk_ap_ft_ow                : 1;
		u64 pm_enclk_ap_ft_ow_val            : 1;
		u64 an_pcs_sel_ow                    : 1;
		u64 an_pcs_sel_ow_val                : 1;
		u64 rg_st_pcslink_max_time_ap_inf_s  : 1;
		u64 reg_ap_force_mode_s              : 1;
		u64 rg_break_link_timer_fast         : 1;
		u64 rg_autoneg_wait_timer_fast       : 1;
		u64 rg_link_fail_inhibit_timer_fast  : 1;
		u64 rg_timer2_fast                   : 1;
		u64 reserved_28_63                   : 36;
	} s;
	/* struct rpmx_anp_portx_an_control_s cn; */
};

static inline u64 RPMX_ANP_PORTX_AN_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_AN_CONTROL(u64 a)
{
	return 0x5c038 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_an_hang_out
 *
 * RPM Anp Port An Hang Out Register AN unused outputs as status.
 */
union rpmx_anp_portx_an_hang_out {
	u64 u;
	struct rpmx_anp_portx_an_hang_out_s {
		u64 ap_irq_s                         : 1;
		u64 ap_rg_3_0_wr_s                   : 1;
		u64 ag_mode_s                        : 5;
		u64 aneg_int_s                       : 1;
		u64 ag_reg_1_6_s                     : 1;
		u64 ap_ag_link_down_pls_s            : 1;
		u64 ieee_ag_aneg_enable_pls_s        : 1;
		u64 reserved_11_63                   : 53;
	} s;
	/* struct rpmx_anp_portx_an_hang_out_s cn; */
};

static inline u64 RPMX_ANP_PORTX_AN_HANG_OUT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_AN_HANG_OUT(u64 a)
{
	return 0x5c058 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_an_restart_counter
 *
 * RPM Anp Port An Restart Counter Register AN restart counter.
 */
union rpmx_anp_portx_an_restart_counter {
	u64 u;
	struct rpmx_anp_portx_an_restart_counter_s {
		u64 an_restart_counter               : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anp_portx_an_restart_counter_s cn; */
};

static inline u64 RPMX_ANP_PORTX_AN_RESTART_COUNTER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_AN_RESTART_COUNTER(u64 a)
{
	return 0x5c070 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_an_status1
 *
 * RPM Anp Port An Status1 Register AN resolved speed.
 */
union rpmx_anp_portx_an_status1 {
	u64 u;
	struct rpmx_anp_portx_an_status1_s {
		u64 ap_pwrup_4x_s                    : 1;
		u64 ap_pwrup_g_s                     : 1;
		u64 ap_pwrup_2p5g_s                  : 1;
		u64 ap_pwrup_5gr_s                   : 1;
		u64 ap_pwrup_10gkr_s                 : 1;
		u64 ap_pwrup_ieee25gs_s              : 1;
		u64 ap_pwrup_ieee25gr_s              : 1;
		u64 ap_pwrup_con25gkr_s              : 1;
		u64 ap_pwrup_con25gcr_s              : 1;
		u64 ap_pwrup_40gkr4_s                : 1;
		u64 ap_pwrup_40gcr4_s                : 1;
		u64 ap_pwrup_50gkr2_s                : 1;
		u64 ap_pwrup_50gcr2_s                : 1;
		u64 ap_pwrup_100gcr4_s               : 1;
		u64 ap_pwrup_100gcr10_s              : 1;
		u64 ap_pwrup_100gkr4_s               : 1;
		u64 ap_pwrup_100gkp4_s               : 1;
		u64 ap_pwrup_200gkr8_s               : 1;
		u64 ap_pwrup_25gkr2_s                : 1;
		u64 ap_pwrup_con40gr2_s              : 1;
		u64 ap_pwrup_50gkr4_s                : 1;
		u64 ap_pwrup_50grcr_s                : 1;
		u64 ap_pwrup_100grcr2_s              : 1;
		u64 ap_pwrup_200grcr4_s              : 1;
		u64 ap_pwrup_100grcr_s               : 1;
		u64 ap_pwrup_200grcr2_s              : 1;
		u64 ap_pwrup_400grcr4_s              : 1;
		u64 ap_pwrup_800grcr8_s              : 1;
		u64 ap_pwrup_con400grcr8_s           : 1;
		u64 reserved_29_63                   : 35;
	} s;
	/* struct rpmx_anp_portx_an_status1_s cn; */
};

static inline u64 RPMX_ANP_PORTX_AN_STATUS1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_AN_STATUS1(u64 a)
{
	return 0x5c060 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_an_status2
 *
 * RPM Anp Port An Status2 Register AN resolved fec/pause + indications.
 */
union rpmx_anp_portx_an_status2 {
	u64 u;
	struct rpmx_anp_portx_an_status2_s {
		u64 ap_aa_clear_hcd_s                : 1;
		u64 ieee_ag_aneg_enable_s            : 1;
		u64 ap_ag_restart_aneg_s             : 1;
		u64 ap_ag_link_s                     : 1;
		u64 ap_ag_hcd_resolved_s             : 1;
		u64 aa_link_good_s                   : 1;
		u64 ap_ag_rx_pause_enable_s          : 1;
		u64 ap_ag_tx_pause_enable_s          : 1;
		u64 ap_fec_enable_s                  : 1;
		u64 ap_rsfec_enable_s                : 1;
		u64 ag_llfec_enable_s                : 1;
		u64 reserved_11_63                   : 53;
	} s;
	/* struct rpmx_anp_portx_an_status2_s cn; */
};

static inline u64 RPMX_ANP_PORTX_AN_STATUS2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_AN_STATUS2(u64 a)
{
	return 0x5c068 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_an_tied_in
 *
 * RPM Anp Port An Tied In Register General inputs towards AN. should
 * keep defaults in functional mode.
 */
union rpmx_anp_portx_an_tied_in {
	u64 u;
	struct rpmx_anp_portx_an_tied_in_s {
		u64 pm_ap_aneg_remote_ready_s        : 1;
		u64 pm_prbs_mode_s                   : 1;
		u64 grg_3_0_15_s                     : 1;
		u64 n_ag_mode_s                      : 5;
		u64 n_aa_link_good_s                 : 1;
		u64 far_clear_reset_all_s            : 1;
		u64 far_set_restart_all_s            : 1;
		u64 pm_kr_enable_s                   : 1;
		u64 pm_loopback_s                    : 1;
		u64 por_fec_adv_s                    : 1;
		u64 ring_osc_a                       : 1;
		u64 z80_ag_mode_s                    : 5;
		u64 z80_fec_enable_s                 : 1;
		u64 z80_llfec_enable_s               : 1;
		u64 z80_resolved_s                   : 1;
		u64 z80_rsfec_enable_s               : 1;
		u64 reserved_24_63                   : 40;
	} s;
	/* struct rpmx_anp_portx_an_tied_in_s cn; */
};

static inline u64 RPMX_ANP_PORTX_AN_TIED_IN(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_AN_TIED_IN(u64 a)
{
	return 0x5c050 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_an_timers
 *
 * RPM Anp Port An Timers Register Short AN timers for simulation
 * purpose.
 */
union rpmx_anp_portx_an_timers {
	u64 u;
	struct rpmx_anp_portx_an_timers_s {
		u64 rg_break_link_timer_fast_val     : 16;
		u64 rg_autoneg_wait_timer_fast_val   : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_an_timers_s cn; */
};

static inline u64 RPMX_ANP_PORTX_AN_TIMERS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_AN_TIMERS(u64 a)
{
	return 0x5c040 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_an_timers1
 *
 * RPM Anp Port An Timers1 Register Short AN timers for simulation
 * purpose.
 */
union rpmx_anp_portx_an_timers1 {
	u64 u;
	struct rpmx_anp_portx_an_timers1_s {
		u64 rg_link_fail_inhibit_timer_fast_val : 16;
		u64 rg_timer2_fast_val               : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_an_timers1_s cn; */
};

static inline u64 RPMX_ANP_PORTX_AN_TIMERS1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_AN_TIMERS1(u64 a)
{
	return 0x5c048 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_dsp_rxup_max_timer
 *
 * RPM Anp Port Ch Dsp Rxup Max Timer Register CH SM Max timer for states
 * RX_SD and TXRX_SD.
 */
union rpmx_anp_portx_ch_dsp_rxup_max_timer {
	u64 u;
	struct rpmx_anp_portx_ch_dsp_rxup_max_timer_s {
		u64 rg_dsp_rxup_max_timer            : 15;
		u64 rg_dsp_rxup_max_timer_inf        : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anp_portx_ch_dsp_rxup_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_DSP_RXUP_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_DSP_RXUP_MAX_TIMER(u64 a)
{
	return 0x5c0d8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_pcs_lost_min_timer
 *
 * RPM Anp Port Ch Pcs Lost Min Timer Register CH SM Min timer to be in
 * NORM state.
 */
union rpmx_anp_portx_ch_pcs_lost_min_timer {
	u64 u;
	struct rpmx_anp_portx_ch_pcs_lost_min_timer_s {
		u64 rg_pcs_lost_min_timer            : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_pcs_lost_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_PCS_LOST_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_PCS_LOST_MIN_TIMER(u64 a)
{
	return 0x5c0a0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_prog_max_timer
 *
 * RPM Anp Port Ch Prog Max Timer Register CH SM Max time to be in any
 * PROG state.
 */
union rpmx_anp_portx_ch_prog_max_timer {
	u64 u;
	struct rpmx_anp_portx_ch_prog_max_timer_s {
		u64 reg_prog_max_time_s              : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_prog_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_PROG_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_PROG_MAX_TIMER(u64 a)
{
	return 0x5c0e0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_pwrdn_min_timer
 *
 * RPM Anp Port Ch Pwrdn Min Timer Register Minimal timer for CH SM PWRDN
 * state.
 */
union rpmx_anp_portx_ch_pwrdn_min_timer {
	u64 u;
	struct rpmx_anp_portx_ch_pwrdn_min_timer_s {
		u64 reg_pwrdn_min_time_s             : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_pwrdn_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_PWRDN_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_PWRDN_MIN_TIMER(u64 a)
{
	return 0x5c078 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_rxon_max_timer
 *
 * RPM Anp Port Ch Rxon Max Timer Register CH SM Max time in RXON state.
 */
union rpmx_anp_portx_ch_rxon_max_timer {
	u64 u;
	struct rpmx_anp_portx_ch_rxon_max_timer_s {
		u64 rg_rxon_max_timer                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_rxon_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_RXON_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_RXON_MAX_TIMER(u64 a)
{
	return 0x5c0b8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_st_min_timer
 *
 * RPM Anp Port Ch St Min Timer Register CH SM Min timer for all states
 * the dont have dedicated timer.
 */
union rpmx_anp_portx_ch_st_min_timer {
	u64 u;
	struct rpmx_anp_portx_ch_st_min_timer_s {
		u64 rg_st_wait_min_s                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_st_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_ST_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_ST_MIN_TIMER(u64 a)
{
	return 0x5c0b0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_st_reset_min_timer
 *
 * RPM Anp Port Ch St Reset Min Timer Register CH SM Min timer in RESET
 * state.
 */
union rpmx_anp_portx_ch_st_reset_min_timer {
	u64 u;
	struct rpmx_anp_portx_ch_st_reset_min_timer_s {
		u64 rg_st_reset_min_time             : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_st_reset_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_ST_RESET_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_ST_RESET_MIN_TIMER(u64 a)
{
	return 0x5c0a8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_txon_max_timer
 *
 * RPM Anp Port Ch Txon Max Timer Register CH SM Max time in TXON state.
 */
union rpmx_anp_portx_ch_txon_max_timer {
	u64 u;
	struct rpmx_anp_portx_ch_txon_max_timer_s {
		u64 rg_txon_max_timer                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_txon_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_TXON_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_TXON_MAX_TIMER(u64 a)
{
	return 0x5c0c0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_txrx_max_timer
 *
 * RPM Anp Port Ch Txrx Max Timer Register CH SM Max timer to be in PWRUP
 * state.
 */
union rpmx_anp_portx_ch_txrx_max_timer {
	u64 u;
	struct rpmx_anp_portx_ch_txrx_max_timer_s {
		u64 rg_txrx_max_timer                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_txrx_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_TXRX_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_TXRX_MAX_TIMER(u64 a)
{
	return 0x5c098 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_txrx_min_timer
 *
 * RPM Anp Port Ch Txrx Min Timer Register CH SM Min timer in state
 * PWRUP.
 */
union rpmx_anp_portx_ch_txrx_min_timer {
	u64 u;
	struct rpmx_anp_portx_ch_txrx_min_timer_s {
		u64 rg_txrx_min_timer                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_txrx_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_TXRX_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_TXRX_MIN_TIMER(u64 a)
{
	return 0x5c090 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_wait_pwrdn_max_timer
 *
 * RPM Anp Port Ch Wait Pwrdn Max Timer Register CH SM Max timer to be in
 * WAIT_PWRDN state.
 */
union rpmx_anp_portx_ch_wait_pwrdn_max_timer {
	u64 u;
	struct rpmx_anp_portx_ch_wait_pwrdn_max_timer_s {
		u64 rg_wait_pwrdn_max_timer          : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_wait_pwrdn_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_WAIT_PWRDN_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_WAIT_PWRDN_MAX_TIMER(u64 a)
{
	return 0x5c080 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_ch_wait_pwrdn_min_timer
 *
 * RPM Anp Port Ch Wait Pwrdn Min Timer Register CH SM Min timer to be in
 * WAIT_PWRDN state.
 */
union rpmx_anp_portx_ch_wait_pwrdn_min_timer {
	u64 u;
	struct rpmx_anp_portx_ch_wait_pwrdn_min_timer_s {
		u64 rg_wait_pwrdn_min_timer          : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_ch_wait_pwrdn_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CH_WAIT_PWRDN_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CH_WAIT_PWRDN_MIN_TIMER(u64 a)
{
	return 0x5c088 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_channel_sm_control
 *
 * RPM Anp Port Channel Sm Control Register Control Register for CH SM.
 * provides - state status, ability to override state, ability to trap
 * state.
 */
union rpmx_anp_portx_channel_sm_control {
	u64 u;
	struct rpmx_anp_portx_channel_sm_control_s {
		u64 ch_sm_override_ctrl              : 2;
		u64 ch_sm_amdisam                    : 1;
		u64 ch_sm_bp_reached                 : 1;
		u64 ch_sm_state                      : 5;
		u64 reserved_9_63                    : 55;
	} s;
	/* struct rpmx_anp_portx_channel_sm_control_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CHANNEL_SM_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CHANNEL_SM_CONTROL(u64 a)
{
	return 0x5c020 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_cmd_line#_hi
 *
 * RPM Anp Port Cmd Line Hi Register 16MSB of command interface cmem in
 * line 2*b and 2*b+1
 */
union rpmx_anp_portx_cmd_linex_hi {
	u64 u;
	struct rpmx_anp_portx_cmd_linex_hi_s {
		u64 cmd_line_hi                      : 16;
		u64 cmd_line2_hi                     : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_cmd_linex_hi_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CMD_LINEX_HI(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CMD_LINEX_HI(u64 a, u64 b)
{
	return 0x5c600 + 0x800 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_anp_port#_cmd_line#_lo
 *
 * RPM Anp Port Cmd Line Lo Register 32LSB of command interface cmem in
 * line b
 */
union rpmx_anp_portx_cmd_linex_lo {
	u64 u;
	struct rpmx_anp_portx_cmd_linex_lo_s {
		u64 cmd_line_lo                      : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_cmd_linex_lo_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CMD_LINEX_LO(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CMD_LINEX_LO(u64 a, u64 b)
{
	return 0x5c400 + 0x800 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_anp_port#_control
 *
 * RPM Anp Port Control Register Set operating MODE + enable, when not
 * operating AN. +Overrides.
 */
union rpmx_anp_portx_control {
	u64 u;
	struct rpmx_anp_portx_control_s {
		u64 port_mode                        : 5;
		u64 pcs_en                           : 1;
		u64 power_down                       : 1;
		u64 custom_phy_gen                   : 5;
		u64 custom_num_of_lanes              : 2;
		u64 reg_pwrup_cnt_clear_s            : 1;
		u64 rg_st_pcslink_max_time_norm_inf_s : 1;
		u64 rg_mode_1g_ow                    : 1;
		u64 rg_mode_1g_ow_val                : 1;
		u64 rg_mode_2p5g_ow                  : 1;
		u64 rg_mode_2p5g_ow_val              : 1;
		u64 rg_mode_5g_ow                    : 1;
		u64 rg_mode_5g_ow_val                : 1;
		u64 rg_mode_10g_ow                   : 1;
		u64 rg_mode_10g_ow_val               : 1;
		u64 rg_mode_25g_ow                   : 1;
		u64 rg_mode_25g_ow_val               : 1;
		u64 rg_mode_40gr4_ow                 : 1;
		u64 rg_mode_40gr4_ow_val             : 1;
		u64 rg_mode_40gr2_ow                 : 1;
		u64 rg_mode_40gr2_ow_val             : 1;
		u64 rg_mode_50gr2_ow                 : 1;
		u64 rg_mode_50gr2_ow_val             : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL(u64 a)
{
	return 0x5c178 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control1
 *
 * RPM Anp Port Control1 Register Overrides.
 */
union rpmx_anp_portx_control1 {
	u64 u;
	struct rpmx_anp_portx_control1_s {
		u64 rg_mode_50gr_ow                  : 1;
		u64 rg_mode_50gr_ow_val              : 1;
		u64 rg_mode_100gr4_ow                : 1;
		u64 rg_mode_100gr4_ow_val            : 1;
		u64 rg_mode_110gr4_ow                : 1;
		u64 rg_mode_110gr4_ow_val            : 1;
		u64 rg_mode_100gr2_ow                : 1;
		u64 rg_mode_100gr2_ow_val            : 1;
		u64 rg_mode_107gr2_ow                : 1;
		u64 rg_mode_107gr2_ow_val            : 1;
		u64 rg_mode_200gr8_ow                : 1;
		u64 rg_mode_200gr8_ow_val            : 1;
		u64 rg_mode_200gr4_ow                : 1;
		u64 rg_mode_200gr4_ow_val            : 1;
		u64 rg_mode_400gr8_ow                : 1;
		u64 rg_mode_400gr8_ow_val            : 1;
		u64 rg_mode_428gr8_ow                : 1;
		u64 rg_mode_428gr8_ow_val            : 1;
		u64 rg_mode_qsgmii_ow                : 1;
		u64 rg_mode_qsgmii_ow_val            : 1;
		u64 rg_mode_usgmii_ow                : 1;
		u64 rg_mode_usgmii_ow_val            : 1;
		u64 rg_mode_usx2p5g_ow               : 1;
		u64 rg_mode_usx2p5g_ow_val           : 1;
		u64 rg_mode_usx5g_ow                 : 1;
		u64 rg_mode_usx5g_ow_val             : 1;
		u64 rg_mode_usx10g_ow                : 1;
		u64 rg_mode_usx10g_ow_val            : 1;
		u64 rg_mode_usx20g_ow                : 1;
		u64 rg_mode_usx20g_ow_val            : 1;
		u64 rg_mode_custom_ow                : 1;
		u64 rg_mode_custom_ow_val            : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control1_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL1(u64 a)
{
	return 0x5c180 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control10
 *
 * RPM Anp Port Control10 Register Overrides + configurations + CH SM
 * options.
 */
union rpmx_anp_portx_control10 {
	u64 u;
	struct rpmx_anp_portx_control10_s {
		u64 pm_pu_rx_req_s_ow                : 1;
		u64 pm_pu_rx_req_s_ow_val            : 1;
		u64 pm_pu_tx_req_s_ow                : 1;
		u64 pm_pu_tx_req_s_ow_val            : 1;
		u64 pm_st_normal_s_ow                : 1;
		u64 pm_st_normal_s_ow_val            : 1;
		u64 pm_st_pwrdn_s_ow                 : 1;
		u64 pm_st_pwrdn_s_ow_val             : 1;
		u64 pm_tx_idle_s_ow                  : 1;
		u64 pm_tx_idle_s_ow_val              : 1;
		u64 sd_phy_gen_ow                    : 1;
		u64 sd_phy_gen_ow_val                : 5;
		u64 txt_dsp_sigdet_loss_pwrdn_instead_rxstr : 1;
		u64 txt_ignore_dsp_lock_loss         : 1;
		u64 txt_ignore_dsp_sigdet_loss       : 1;
		u64 txt_ignore_tx_ready_loss         : 1;
		u64 txt_no_dsp_lock_pwrdn_instead_rxstr : 1;
		u64 txt_rxstr_pu_pll_rx_value        : 1;
		u64 txt_tx_ready_loss_pwrdn_instead_txstr : 1;
		u64 txt_txrx_on_timeout_pwrdn_instead_rxstr : 1;
		u64 txt_txrx_sd_timeout_pwrdn_instead_rxstr : 1;
		u64 txt_txstr_pu_pll_tx_value        : 1;
		u64 reg_tx_train_dsp_sigdet_sel_s    : 2;
		u64 reg_reset_en_tx_train_s          : 1;
		u64 reg_reset_en_rx_train_s          : 1;
		u64 reg_reset_en_rx_init_s           : 1;
		u64 reg_reset_en_pu_pll_s            : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control10_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL10(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL10(u64 a)
{
	return 0x5c1c8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control11
 *
 * RPM Anp Port Control11 Register Configurations + overrides.
 */
union rpmx_anp_portx_control11 {
	u64 u;
	struct rpmx_anp_portx_control11_s {
		u64 pu_rx_both_in_idle               : 1;
		u64 reg_pm_sd_pu_reset_on_sftrst_s   : 1;
		u64 reg_prog_mask_rx_init_s          : 1;
		u64 reg_prog_mask_rx_restr_s         : 1;
		u64 reg_prog_mask_rx_train_s         : 1;
		u64 reg_prog_mask_tx_restr_s         : 1;
		u64 reg_prog_mask_tx_train_s         : 1;
		u64 reg_prog_mask_norm_s             : 1;
		u64 reg_prog_mask_pwrup_s            : 1;
		u64 reg_prog_mask_rxon_s             : 1;
		u64 reg_prog_mask_rxsd_s             : 1;
		u64 reg_prog_mask_txon_s             : 1;
		u64 reg_prog_mask_txrxon_s           : 1;
		u64 reg_prog_mask_txrxsd_s           : 1;
		u64 pu_tx_both_in_idle               : 1;
		u64 sd_sw_resetn_ow                  : 1;
		u64 sd_sw_resetn_ow_val              : 1;
		u64 pm_clockout_gater_ow             : 1;
		u64 pm_clockout_gater_ow_val         : 1;
		u64 rg_wait_pwrdn_max_timer_inf      : 1;
		u64 reserved_20_63                   : 44;
	} s;
	/* struct rpmx_anp_portx_control11_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL11(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL11(u64 a)
{
	return 0x5c1d0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control2
 *
 * RPM Anp Port Control2 Register Overrides.
 */
union rpmx_anp_portx_control2 {
	u64 u;
	struct rpmx_anp_portx_control2_s {
		u64 pm_sd_phy_gen_tx_ow              : 1;
		u64 pm_sd_phy_gen_tx_ow_val          : 5;
		u64 pm_sd_phy_gen_rx_ow              : 1;
		u64 pm_sd_phy_gen_rx_ow_val          : 5;
		u64 pm_an_restart_ow                 : 1;
		u64 pm_an_restart_ow_val             : 1;
		u64 pm_nr_reset_ow                   : 1;
		u64 pm_nr_reset_ow_val               : 1;
		u64 pm_sd_pu_pll_ow                  : 1;
		u64 pm_sd_pu_pll_ow_val              : 1;
		u64 pm_sd_pu_tx_ow                   : 1;
		u64 pm_sd_pu_tx_ow_val               : 1;
		u64 pm_sd_pu_rx_ow                   : 1;
		u64 pm_sd_pu_rx_ow_val               : 1;
		u64 pm_sd_softrst_s_ow               : 1;
		u64 pm_sd_softrst_s_ow_val           : 1;
		u64 pm_sd_couple_mode_en_ow          : 1;
		u64 pm_sd_couple_mode_en_ow_val      : 1;
		u64 pm_sd_txclk_sync_en_pll_ow       : 1;
		u64 pm_sd_txclk_sync_en_pll_ow_val   : 1;
		u64 pm_st_en_ow                      : 1;
		u64 pm_st_en_ow_val                  : 1;
		u64 pm_softrst_s_ow                  : 1;
		u64 pm_softrst_s_ow_val              : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control2_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL2(u64 a)
{
	return 0x5c188 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control3
 *
 * RPM Anp Port Control3 Register Overrides + TFIFO pointers update +
 * timeout disable.
 */
union rpmx_anp_portx_control3 {
	u64 u;
	struct rpmx_anp_portx_control3_s {
		u64 pm_pcs_couple_ow                 : 1;
		u64 pm_pcs_couple_ow_val             : 1;
		u64 pm_train_type_mx_ow              : 1;
		u64 pm_train_type_mx_ow_val          : 2;
		u64 reg_prog_max_time_s_inf          : 1;
		u64 pm_pwrdn_ow                      : 1;
		u64 pm_pwrdn_ow_val                  : 1;
		u64 pm_an_hcd_clear_ow               : 1;
		u64 pm_an_hcd_clear_ow_val           : 1;
		u64 pm_sd_tx_idle_ow                 : 1;
		u64 pm_sd_tx_idle_ow_val             : 1;
		u64 pm_an_hcd_resolved_ow            : 1;
		u64 pm_an_hcd_resolved_ow_val        : 1;
		u64 pm_sd_dfe_update_dis_ow          : 1;
		u64 pm_sd_dfe_update_dis_ow_val      : 1;
		u64 pm_tx_train_poly_sel_ow          : 1;
		u64 pm_tx_train_poly_sel_ow_val      : 4;
		u64 pm_sd_dfe_pat_dis_ow             : 1;
		u64 pm_sd_dfe_pat_dis_ow_val         : 1;
		u64 pm_sd_dfe_en_ow                  : 1;
		u64 pm_sd_dfe_en_ow_val              : 1;
		u64 reg_tx_tfifo_w_upd               : 1;
		u64 reg_tx_tfifo_r_upd               : 1;
		u64 reg_rx_tfifo_w_upd               : 1;
		u64 reg_rx_tfifo_r_upd               : 1;
		u64 tx_train_error_s_ow              : 1;
		u64 tx_train_error_s_ow_val          : 2;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control3_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL3(u64 a)
{
	return 0x5c190 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control4
 *
 * RPM Anp Port Control4 Register Overrides.
 */
union rpmx_anp_portx_control4 {
	u64 u;
	struct rpmx_anp_portx_control4_s {
		u64 pm_ap_en_s_ow                    : 1;
		u64 pm_ap_en_s_ow_val                : 1;
		u64 pll_ready_tx_ow                  : 1;
		u64 pll_ready_tx_ow_val              : 1;
		u64 pll_ready_rx_ow                  : 1;
		u64 pll_ready_rx_ow_val              : 1;
		u64 sq_detected_lpf_ow               : 1;
		u64 sq_detected_lpf_ow_val           : 1;
		u64 rx_init_done_ow                  : 1;
		u64 rx_init_done_ow_val              : 1;
		u64 rx_train_complete_ow             : 1;
		u64 rx_train_complete_ow_val         : 1;
		u64 rx_train_failed_ow               : 1;
		u64 rx_train_failed_ow_val           : 1;
		u64 tx_train_complete_ow             : 1;
		u64 tx_train_complete_ow_val         : 1;
		u64 tx_train_failed_ow               : 1;
		u64 tx_train_failed_ow_val           : 1;
		u64 sd_txclk_sync_start_out_ow       : 1;
		u64 sd_txclk_sync_start_out_ow_val   : 1;
		u64 pm_dsp_txdn_ack_ow               : 1;
		u64 pm_dsp_txdn_ack_ow_val           : 1;
		u64 pm_dsp_rxdn_ack_ow               : 1;
		u64 pm_dsp_rxdn_ack_ow_val           : 1;
		u64 pm_dsp_tx_ready_ow               : 1;
		u64 pm_dsp_tx_ready_ow_val           : 1;
		u64 pm_dsp_sigdet_ow                 : 1;
		u64 pm_dsp_sigdet_ow_val             : 1;
		u64 pm_dsp_lock_ow                   : 1;
		u64 pm_dsp_lock_ow_val               : 1;
		u64 pm_rx_init_ow                    : 1;
		u64 pm_rx_init_ow_val                : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control4_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL4(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL4(u64 a)
{
	return 0x5c198 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control5
 *
 * RPM Anp Port Control5 Register Overrides + timeout disable + CH SM
 * options.
 */
union rpmx_anp_portx_control5 {
	u64 u;
	struct rpmx_anp_portx_control5_s {
		u64 pm_rx_train_enable_ow            : 1;
		u64 pm_rx_train_enable_ow_val        : 1;
		u64 pm_tx_train_enable_ow            : 1;
		u64 pm_tx_train_enable_ow_val        : 1;
		u64 rg_txrx_max_timer_inf            : 1;
		u64 reg_normal_state_lock            : 1;
		u64 sd_rx_dtl_clamp_s_ow             : 1;
		u64 sd_rx_dtl_clamp_s_ow_val         : 1;
		u64 dsp_sigdet_loss_pwrdn_instead_rxstr : 1;
		u64 ignore_dsp_lock_loss             : 1;
		u64 ignore_dsp_sigdet_loss           : 1;
		u64 ignore_tx_ready_loss             : 1;
		u64 no_dsp_lock_pwrdn_instead_rxstr  : 1;
		u64 rxstr_pu_pll_rx_value            : 1;
		u64 tx_ready_loss_pwrdn_instead_txstr : 1;
		u64 txrx_on_timeout_pwrdn_instead_rxstr : 1;
		u64 txrx_sd_timeout_pwrdn_instead_rxstr : 1;
		u64 txstr_pu_pll_tx_value            : 1;
		u64 pm_prog_pwm_norm_en_mx_s_ow      : 1;
		u64 pm_prog_pwm_norm_en_mx_s_ow_val  : 1;
		u64 pm_prog_pwm_pwrup_en_mx_s_ow     : 1;
		u64 pm_prog_pwm_pwrup_en_mx_s_ow_val : 1;
		u64 pm_prog_pwm_rxon_en_mx_s_ow      : 1;
		u64 pm_prog_pwm_rxon_en_mx_s_ow_val  : 1;
		u64 pm_prog_pwm_rxsd_en_mx_s_ow      : 1;
		u64 pm_prog_pwm_rxsd_en_mx_s_ow_val  : 1;
		u64 pm_prog_pwm_txon_en_mx_s_ow      : 1;
		u64 pm_prog_pwm_txon_en_mx_s_ow_val  : 1;
		u64 pm_prog_pwm_txrxon_en_mx_s_ow    : 1;
		u64 pm_prog_pwm_txrxon_en_mx_s_ow_val : 1;
		u64 pm_prog_pwm_txrxsd_en_mx_s_ow    : 1;
		u64 pm_prog_pwm_txrxsd_en_mx_s_ow_val : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control5_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL5(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL5(u64 a)
{
	return 0x5c1a0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control6
 *
 * RPM Anp Port Control6 Register Overrides + Configurations.
 */
union rpmx_anp_portx_control6 {
	u64 u;
	struct rpmx_anp_portx_control6_s {
		u64 pm_pcs_rx_clk_ena_ow             : 1;
		u64 pm_pcs_rx_clk_ena_ow_val         : 1;
		u64 pm_pcs_tx_clk_ena_ow             : 1;
		u64 pm_pcs_tx_clk_ena_ow_val         : 1;
		u64 pm_pcs_sd_rx_resetn_ow           : 1;
		u64 pm_pcs_sd_rx_resetn_ow_val       : 1;
		u64 pm_pcs_sd_tx_resetn_ow           : 1;
		u64 pm_pcs_sd_tx_resetn_ow_val       : 1;
		u64 pm_an_pcs_clkout_sel_ow          : 1;
		u64 pm_an_pcs_clkout_sel_ow_val      : 1;
		u64 link_status_ow                   : 1;
		u64 link_status_ow_val               : 1;
		u64 reg_invert_sd_rx_in_s            : 1;
		u64 reg_invert_sd_tx_out_s           : 1;
		u64 reg_sigdet_mode                  : 2;
		u64 rg_pwrdn_rdy_s                   : 1;
		u64 rg_sel_los_sig_s                 : 1;
		u64 tx_busy_no_pwrdn                 : 1;
		u64 pcs_cfg_done_hw_clr              : 1;
		u64 reg_tx_train_complete_latch      : 1;
		u64 reg_rx_train_complete_latch      : 1;
		u64 reg_tx_train_failed_latch        : 1;
		u64 reg_rx_train_failed_latch        : 1;
		u64 reg_an_restart_cnt_en            : 1;
		u64 reg_dsp_lock_fail_cnt_en         : 1;
		u64 reg_link_fail_cnt_en             : 1;
		u64 rg_prog_enable_s                 : 1;
		u64 rg_force_pg_start_s              : 1;
		u64 cmem_mask                        : 1;
		u64 int_valid_ow                     : 1;
		u64 int_enabled_ow                   : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control6_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL6(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL6(u64 a)
{
	return 0x5c1a8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control7
 *
 * RPM Anp Port Control7 Register Overrides + Configurations.
 */
union rpmx_anp_portx_control7 {
	u64 u;
	struct rpmx_anp_portx_control7_s {
		u64 pll_ready_rx_clean_ow            : 1;
		u64 pll_ready_rx_clean_ow_val        : 1;
		u64 pll_ready_tx_clean_ow            : 1;
		u64 pll_ready_tx_clean_ow_val        : 1;
		u64 reserved_4                       : 1;
		u64 reg_prog_norm_en_s               : 1;
		u64 reg_rxstr_abort_rx_train_s       : 1;
		u64 reg_prog_pwrup_en_s              : 1;
		u64 reg_prog_rxon_en_s               : 1;
		u64 reg_prog_rxsd_en_s               : 1;
		u64 reg_prog_txon_en_s               : 1;
		u64 reg_prog_txrxon_en_s             : 1;
		u64 reg_prog_txrxsd_en_s             : 1;
		u64 cnt_clear_on_sd_rst              : 1;
		u64 duration_clear_on_sd_rst         : 1;
		u64 rx_init_ok_cnt_clear             : 1;
		u64 rx_init_timeout_clear            : 1;
		u64 rx_train_failed_cnt_clear        : 1;
		u64 rx_train_ok_cnt_clear            : 1;
		u64 rx_train_timeout_cnt_clear       : 1;
		u64 tx_train_failed_cnt_clear        : 1;
		u64 tx_train_ok_cnt_clear            : 1;
		u64 tx_train_timeout_cnt_clear       : 1;
		u64 reg_rxstr_abort_rx_init_s        : 1;
		u64 dsp_rxstr_ack_s_ow               : 1;
		u64 dsp_rxstr_ack_s_ow_val           : 1;
		u64 dsp_txstr_ack_s_ow               : 1;
		u64 dsp_txstr_ack_s_ow_val           : 1;
		u64 reg_dfe_adaptation_en_s          : 1;
		u64 reg_dfe_auto_ctrl_s              : 1;
		u64 reg_dfe_frozen_s                 : 1;
		u64 reg_norm_st_sq_detected_mask_s   : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control7_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL7(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL7(u64 a)
{
	return 0x5c1b0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control8
 *
 * RPM Anp Port Control8 Register Overrides + timeout disable +
 * configurations.
 */
union rpmx_anp_portx_control8 {
	u64 u;
	struct rpmx_anp_portx_control8_s {
		u64 pm_train_type_ow                 : 1;
		u64 pm_train_type_ow_val             : 2;
		u64 rg_txon_max_timer_inf            : 1;
		u64 rg_rxon_max_timer_inf            : 1;
		u64 reg_prog_en_rx_init_s            : 1;
		u64 reg_prog_en_rx_train_s           : 1;
		u64 rx_busy_no_pwrdn                 : 1;
		u64 reg_prog_en_tx_train_s           : 1;
		u64 pcs_cfg_done_fin_s_ow            : 1;
		u64 pcs_cfg_done_fin_s_ow_val        : 1;
		u64 reg_rx_auto_re_train_s           : 1;
		u64 reg_sel_init_done_s              : 1;
		u64 reg_tx_auto_re_train_s           : 1;
		u64 comphy_int_ack_mx_s_ow           : 1;
		u64 comphy_int_ack_mx_s_ow_val       : 1;
		u64 prog_pwm_done_s_ow               : 1;
		u64 prog_pwm_done_s_ow_val           : 1;
		u64 prog_rx_done_s_ow                : 1;
		u64 prog_rx_done_s_ow_val            : 1;
		u64 prog_tx_done_s_ow                : 1;
		u64 prog_tx_done_s_ow_val            : 1;
		u64 opmode_match_mx_s_ow             : 1;
		u64 opmode_match_mx_s_ow_val         : 1;
		u64 couple_match_mx_s_ow             : 1;
		u64 couple_match_mx_s_ow_val         : 1;
		u64 ap_match_mx_s_ow                 : 1;
		u64 ap_match_mx_s_ow_val             : 1;
		u64 pg_en_mx_s_ow                    : 1;
		u64 pg_en_mx_s_ow_val                : 1;
		u64 int_data_ow                      : 1;
		u64 int_code_ow                      : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control8_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL8(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL8(u64 a)
{
	return 0x5c1b8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_control9
 *
 * RPM Anp Port Control9 Register Overrides.
 */
union rpmx_anp_portx_control9 {
	u64 u;
	struct rpmx_anp_portx_control9_s {
		u64 int_enabled_ow_val               : 1;
		u64 int_valid_ow_val                 : 1;
		u64 int_code_ow_val                  : 8;
		u64 int_data_ow_val                  : 22;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_control9_s cn; */
};

static inline u64 RPMX_ANP_PORTX_CONTROL9(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_CONTROL9(u64 a)
{
	return 0x5c1c0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_counter
 *
 * RPM Anp Port Counter Register Last rx_init duration.
 */
union rpmx_anp_portx_counter {
	u64 u;
	struct rpmx_anp_portx_counter_s {
		u64 stat_rx_init_duration_l          : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_counter_s cn; */
};

static inline u64 RPMX_ANP_PORTX_COUNTER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_COUNTER(u64 a)
{
	return 0x5c208 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_counter1
 *
 * RPM Anp Port Counter1 Register Last rx training duration.
 */
union rpmx_anp_portx_counter1 {
	u64 u;
	struct rpmx_anp_portx_counter1_s {
		u64 stat_rx_train_duration_l         : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_counter1_s cn; */
};

static inline u64 RPMX_ANP_PORTX_COUNTER1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_COUNTER1(u64 a)
{
	return 0x5c210 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_counter2
 *
 * RPM Anp Port Counter2 Register Last tx training duration.
 */
union rpmx_anp_portx_counter2 {
	u64 u;
	struct rpmx_anp_portx_counter2_s {
		u64 stat_tx_train_duration_l         : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_counter2_s cn; */
};

static inline u64 RPMX_ANP_PORTX_COUNTER2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_COUNTER2(u64 a)
{
	return 0x5c218 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_counter3
 *
 * RPM Anp Port Counter3 Register RX train/init counter + rx train
 * duration.
 */
union rpmx_anp_portx_counter3 {
	u64 u;
	struct rpmx_anp_portx_counter3_s {
		u64 stat_rx_init_ok_cnt              : 10;
		u64 stat_rx_init_timeout_cnt         : 10;
		u64 stat_rx_train_failed_cnt         : 10;
		u64 stat_rx_train_duration_l_hi      : 2;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_counter3_s cn; */
};

static inline u64 RPMX_ANP_PORTX_COUNTER3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_COUNTER3(u64 a)
{
	return 0x5c220 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_counter4
 *
 * RPM Anp Port Counter4 Register RX + TX Counters + Last tx train
 * duration.
 */
union rpmx_anp_portx_counter4 {
	u64 u;
	struct rpmx_anp_portx_counter4_s {
		u64 stat_rx_train_ok_cnt             : 10;
		u64 stat_rx_train_timeout_cnt        : 10;
		u64 stat_tx_train_failed_cnt         : 10;
		u64 stat_tx_train_duration_l_hi      : 2;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_counter4_s cn; */
};

static inline u64 RPMX_ANP_PORTX_COUNTER4(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_COUNTER4(u64 a)
{
	return 0x5c228 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_counter5
 *
 * RPM Anp Port Counter5 Register TX counters + CH SM PWRUP counter.
 */
union rpmx_anp_portx_counter5 {
	u64 u;
	struct rpmx_anp_portx_counter5_s {
		u64 stat_tx_train_ok_cnt             : 10;
		u64 stat_tx_train_timeout_cnt        : 10;
		u64 pwrup_cnt_s                      : 10;
		u64 reserved_30_63                   : 34;
	} s;
	/* struct rpmx_anp_portx_counter5_s cn; */
};

static inline u64 RPMX_ANP_PORTX_COUNTER5(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_COUNTER5(u64 a)
{
	return 0x5c230 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_dsp_lock_fail_counter
 *
 * RPM Anp Port Dsp Lock Fail Counter Register Count dsp_lock restart.
 */
union rpmx_anp_portx_dsp_lock_fail_counter {
	u64 u;
	struct rpmx_anp_portx_dsp_lock_fail_counter_s {
		u64 dsp_lock_fail_coutner            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anp_portx_dsp_lock_fail_counter_s cn; */
};

static inline u64 RPMX_ANP_PORTX_DSP_LOCK_FAIL_COUNTER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_DSP_LOCK_FAIL_COUNTER(u64 a)
{
	return 0x5c238 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_interrupt2_cause
 *
 * RPM Anp Port Interrupt2 Cause Register Per port interrupt register.
 * TFIFO errors.
 */
union rpmx_anp_portx_interrupt2_cause {
	u64 u;
	struct rpmx_anp_portx_interrupt2_cause_s {
		u64 port_int2_sum                    : 1;
		u64 tx_tfifo_w_err                   : 1;
		u64 tx_tfifo_r_err                   : 1;
		u64 rx_tfifo_w_err                   : 1;
		u64 rx_tfifo_r_err                   : 1;
		u64 tx_tfifo_full                    : 1;
		u64 tx_tfifo_empty                   : 1;
		u64 rx_tfifo_full                    : 1;
		u64 rx_tfifo_empty                   : 1;
		u64 reserved_9_63                    : 55;
	} s;
	/* struct rpmx_anp_portx_interrupt2_cause_s cn; */
};

static inline u64 RPMX_ANP_PORTX_INTERRUPT2_CAUSE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_INTERRUPT2_CAUSE(u64 a)
{
	return 0x5c010 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_interrupt2_mask
 *
 * RPM Anp Port Interrupt2 Mask Register Mask for interrupt2 register.
 */
union rpmx_anp_portx_interrupt2_mask {
	u64 u;
	struct rpmx_anp_portx_interrupt2_mask_s {
		u64 reserved_0                       : 1;
		u64 port_int2_mask                   : 8;
		u64 reserved_9_63                    : 55;
	} s;
	/* struct rpmx_anp_portx_interrupt2_mask_s cn; */
};

static inline u64 RPMX_ANP_PORTX_INTERRUPT2_MASK(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_INTERRUPT2_MASK(u64 a)
{
	return 0x5c018 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_interrupt_cause
 *
 * RPM Anp Port Interrupt Cause Register Per port interrupt register. an
 * - restart/hcd_found. SMs - break point reached. timeouts and reaching
 * prog states.
 */
union rpmx_anp_portx_interrupt_cause {
	u64 u;
	struct rpmx_anp_portx_interrupt_cause_s {
		u64 port_int_sum                     : 1;
		u64 an_restart                       : 1;
		u64 hcd_found                        : 1;
		u64 an_good_ck                       : 1;
		u64 ch_sm_bp_reached_int             : 1;
		u64 sd_tx_sm_bp_reached_int          : 1;
		u64 sd_rx_sm_bp_reached_int          : 1;
		u64 int_prog_pwm_norm_en_mx_s        : 1;
		u64 int_prog_pwm_pwrup_en_mx_s       : 1;
		u64 int_prog_pwm_rxon_en_mx_s        : 1;
		u64 int_prog_pwm_rxsd_en_mx_s        : 1;
		u64 int_prog_pwm_txon_en_mx_s        : 1;
		u64 int_prog_pwm_txrxon_en_mx_s      : 1;
		u64 int_prog_pwm_txrxsd_en_mx_s      : 1;
		u64 int_pm_dsp_rxup_time_out         : 1;
		u64 int_pm_pcs_link_timer_out        : 1;
		u64 int_rxon_wait_time_out           : 1;
		u64 int_txon_wait_time_out           : 1;
		u64 int_txrx_start_wait_time_out     : 1;
		u64 int_pm_prog_rx_init_s            : 1;
		u64 int_pm_prog_rx_train_s           : 1;
		u64 int_pm_prog_tx_train_s           : 1;
		u64 int_prog_rx_time_out             : 1;
		u64 int_prog_tx_time_out             : 1;
		u64 int_rx_init_time_out             : 1;
		u64 int_rx_pll_up_time_out           : 1;
		u64 int_rx_train_time_out            : 1;
		u64 int_tx_pll_up_time_out           : 1;
		u64 int_tx_train_time_out            : 1;
		u64 int_prog_time_out                : 1;
		u64 int_wait_pwrdn_time_out          : 1;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_interrupt_cause_s cn; */
};

static inline u64 RPMX_ANP_PORTX_INTERRUPT_CAUSE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_INTERRUPT_CAUSE(u64 a)
{
	return 0x5c000 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_interrupt_mask
 *
 * RPM Anp Port Interrupt Mask Register Mask for port interrupt register.
 */
union rpmx_anp_portx_interrupt_mask {
	u64 u;
	struct rpmx_anp_portx_interrupt_mask_s {
		u64 reserved_0                       : 1;
		u64 port_int_mask                    : 30;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_interrupt_mask_s cn; */
};

static inline u64 RPMX_ANP_PORTX_INTERRUPT_MASK(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_INTERRUPT_MASK(u64 a)
{
	return 0x5c008 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_link_fail_counter
 *
 * RPM Anp Port Link Fail Counter Register Count restarts due to link
 * fail.
 */
union rpmx_anp_portx_link_fail_counter {
	u64 u;
	struct rpmx_anp_portx_link_fail_counter_s {
		u64 link_fail_counter                : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_anp_portx_link_fail_counter_s cn; */
};

static inline u64 RPMX_ANP_PORTX_LINK_FAIL_COUNTER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_LINK_FAIL_COUNTER(u64 a)
{
	return 0x5c240 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_pcs_link_max_timer_ap
 *
 * RPM Anp Port Pcs Link Max Timer Ap Register CH SM Max time in TXRX_ON
 * state when operating AN.
 */
union rpmx_anp_portx_pcs_link_max_timer_ap {
	u64 u;
	struct rpmx_anp_portx_pcs_link_max_timer_ap_s {
		u64 pcs_link_max_timer_ap            : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_pcs_link_max_timer_ap_s cn; */
};

static inline u64 RPMX_ANP_PORTX_PCS_LINK_MAX_TIMER_AP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_PCS_LINK_MAX_TIMER_AP(u64 a)
{
	return 0x5c0d0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_pcs_link_max_timer_norm
 *
 * RPM Anp Port Pcs Link Max Timer Norm Register CH SM Max time in
 * TXRX_ON state when NOT operating AN.
 */
union rpmx_anp_portx_pcs_link_max_timer_norm {
	u64 u;
	struct rpmx_anp_portx_pcs_link_max_timer_norm_s {
		u64 pcs_link_max_timer_norm          : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_pcs_link_max_timer_norm_s cn; */
};

static inline u64 RPMX_ANP_PORTX_PCS_LINK_MAX_TIMER_NORM(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_PCS_LINK_MAX_TIMER_NORM(u64 a)
{
	return 0x5c0c8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_idle_ap_min_timer
 *
 * RPM Anp Port Sd Idle Ap Min Timer Register COMPHY TX SM - Min timer to
 * be in state tx_idle_remove_nokr when operating AN.
 */
union rpmx_anp_portx_sd_idle_ap_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_idle_ap_min_timer_s {
		u64 reg_tx_idle_wait_time_ap_s       : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_idle_ap_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_IDLE_AP_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_IDLE_AP_MIN_TIMER(u64 a)
{
	return 0x5c130 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_idle_nap_min_timer
 *
 * RPM Anp Port Sd Idle Nap Min Timer Register COMPHY TX SM - Min timer
 * to be in state tx_idle_remove_nokr when NOT operating AN.
 */
union rpmx_anp_portx_sd_idle_nap_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_idle_nap_min_timer_s {
		u64 reg_tx_idle_wait_time_other_s    : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_idle_nap_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_IDLE_NAP_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_IDLE_NAP_MIN_TIMER(u64 a)
{
	return 0x5c138 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_idle_rm_min_timer
 *
 * RPM Anp Port Sd Idle Rm Min Timer Register COMPHY TX SM - Min timer
 * for state tx_idle_remove_2kr.
 */
union rpmx_anp_portx_sd_idle_rm_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_idle_rm_min_timer_s {
		u64 reg_tx_comphy_idle_remove_min_wait_s : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_idle_rm_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_IDLE_RM_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_IDLE_RM_MIN_TIMER(u64 a)
{
	return 0x5c128 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_pll_up_max_timer
 *
 * RPM Anp Port Sd Pll Up Max Timer Register Max time for COMPHY TX SM -
 * tx_pll_up state. COMPHY RX SM - rx_pll_up state.
 */
union rpmx_anp_portx_sd_pll_up_max_timer {
	u64 u;
	struct rpmx_anp_portx_sd_pll_up_max_timer_s {
		u64 reg_pll_up_time_out_s            : 31;
		u64 reg_pll_up_time_out_s_inf        : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_sd_pll_up_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_PLL_UP_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_PLL_UP_MAX_TIMER(u64 a)
{
	return 0x5c0e8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_prog_max_timer
 *
 * RPM Anp Port Sd Prog Max Timer Register COMPHY SMs (TX & RX) Max timer
 * for any PROG state.
 */
union rpmx_anp_portx_sd_prog_max_timer {
	u64 u;
	struct rpmx_anp_portx_sd_prog_max_timer_s {
		u64 reg_sd_prog_max_time_s           : 31;
		u64 reg_sd_prog_max_time_s_inf       : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_sd_prog_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_PROG_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_PROG_MAX_TIMER(u64 a)
{
	return 0x5c0f0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_rx_init_max_timer
 *
 * RPM Anp Port Sd Rx Init Max Timer Register COMPHY RX SM - Max timer
 * for rx_init state.
 */
union rpmx_anp_portx_sd_rx_init_max_timer {
	u64 u;
	struct rpmx_anp_portx_sd_rx_init_max_timer_s {
		u64 reg_rx_init_time_out_s           : 31;
		u64 reg_rx_init_time_out_s_inf       : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_sd_rx_init_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_RX_INIT_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_RX_INIT_MAX_TIMER(u64 a)
{
	return 0x5c100 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_rx_init_min_timer
 *
 * RPM Anp Port Sd Rx Init Min Timer Register COMPHY RX SM - Min time for
 * rx_init state.
 */
union rpmx_anp_portx_sd_rx_init_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_rx_init_min_timer_s {
		u64 reg_rx_init_min_wait_s           : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_rx_init_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_RX_INIT_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_RX_INIT_MIN_TIMER(u64 a)
{
	return 0x5c0f8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_rx_pll_up_min_timer
 *
 * RPM Anp Port Sd Rx Pll Up Min Timer Register COMPHY RX SM - Min timer
 * for rx_pll_up state.
 */
union rpmx_anp_portx_sd_rx_pll_up_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_rx_pll_up_min_timer_s {
		u64 reg_rx_pll_up_min_wait_s         : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_rx_pll_up_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_RX_PLL_UP_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_RX_PLL_UP_MIN_TIMER(u64 a)
{
	return 0x5c108 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_rx_retrain_min_timer
 *
 * RPM Anp Port Sd Rx Retrain Min Timer Register When RX retrain is set.
 * this is the COMPHY RX SM wait time after rx_train failed, before
 * moving to rx_wait_plug.
 */
union rpmx_anp_portx_sd_rx_retrain_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_rx_retrain_min_timer_s {
		u64 reg_rx_retrain_time_s            : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_rx_retrain_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_RX_RETRAIN_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_RX_RETRAIN_MIN_TIMER(u64 a)
{
	return 0x5c160 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_rx_wait_plug_min_timer
 *
 * RPM Anp Port Sd Rx Wait Plug Min Timer Register COMPHY RX SM - Min
 * timer for rx_wait_plug state.
 */
union rpmx_anp_portx_sd_rx_wait_plug_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_rx_wait_plug_min_timer_s {
		u64 reg_rx_wait_plug_min_wait_s      : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_rx_wait_plug_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_RX_WAIT_PLUG_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_RX_WAIT_PLUG_MIN_TIMER(u64 a)
{
	return 0x5c168 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_rx_wait_sq_det_min_timer
 *
 * RPM Anp Port Sd Rx Wait Sq Det Min Timer Register COMPHY RX SM - Min
 * timer for rx_wait_sq_det state.
 */
union rpmx_anp_portx_sd_rx_wait_sq_det_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_rx_wait_sq_det_min_timer_s {
		u64 reg_rx_wait_sq_det_min_wait_s    : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_rx_wait_sq_det_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_RX_WAIT_SQ_DET_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_RX_WAIT_SQ_DET_MIN_TIMER(u64 a)
{
	return 0x5c170 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_rxt_max_timer
 *
 * RPM Anp Port Sd Rxt Max Timer Register COMPHY RX SM - Max timer for
 * rx_train state.
 */
union rpmx_anp_portx_sd_rxt_max_timer {
	u64 u;
	struct rpmx_anp_portx_sd_rxt_max_timer_s {
		u64 reg_rx_train_time_out_s          : 31;
		u64 reg_rx_train_time_out_s_inf      : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_sd_rxt_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_RXT_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_RXT_MAX_TIMER(u64 a)
{
	return 0x5c120 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_rxt_min_timer
 *
 * RPM Anp Port Sd Rxt Min Timer Register COMPHY RX SM - Min timer for
 * rx_train state.
 */
union rpmx_anp_portx_sd_rxt_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_rxt_min_timer_s {
		u64 reg_rx_train_min_wait_s          : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_rxt_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_RXT_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_RXT_MIN_TIMER(u64 a)
{
	return 0x5c110 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_rxt_ok_max_timer
 *
 * RPM Anp Port Sd Rxt Ok Max Timer Register COMPHY RX SM - time to wait
 * after RX train complete successful, before moving to rx_done_ok state.
 */
union rpmx_anp_portx_sd_rxt_ok_max_timer {
	u64 u;
	struct rpmx_anp_portx_sd_rxt_ok_max_timer_s {
		u64 reg_rx_train_ok_wait_timeout_s   : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_rxt_ok_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_RXT_OK_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_RXT_OK_MAX_TIMER(u64 a)
{
	return 0x5c118 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_tx_pll_up_min_timer
 *
 * RPM Anp Port Sd Tx Pll Up Min Timer Register COMPHY TX SM - Min timer
 * for state tx_pll_up.
 */
union rpmx_anp_portx_sd_tx_pll_up_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_tx_pll_up_min_timer_s {
		u64 reg_tx_pll_up_min_wait_s         : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_tx_pll_up_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_TX_PLL_UP_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_TX_PLL_UP_MIN_TIMER(u64 a)
{
	return 0x5c140 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_txt_max_timer
 *
 * RPM Anp Port Sd Txt Max Timer Register COMPHY TX SM - Max timer for
 * tx_train state.
 */
union rpmx_anp_portx_sd_txt_max_timer {
	u64 u;
	struct rpmx_anp_portx_sd_txt_max_timer_s {
		u64 reg_tx_train_time_out_s          : 31;
		u64 reg_tx_train_time_out_s_inf      : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_sd_txt_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_TXT_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_TXT_MAX_TIMER(u64 a)
{
	return 0x5c158 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_txt_min_timer
 *
 * RPM Anp Port Sd Txt Min Timer Register COMPHY TX SM - Min timer for
 * tx_train state.
 */
union rpmx_anp_portx_sd_txt_min_timer {
	u64 u;
	struct rpmx_anp_portx_sd_txt_min_timer_s {
		u64 reg_tx_train_min_wait_s          : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_txt_min_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_TXT_MIN_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_TXT_MIN_TIMER(u64 a)
{
	return 0x5c148 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_sd_txt_ok_max_timer
 *
 * RPM Anp Port Sd Txt Ok Max Timer Register COMPHY TX SM - time to wait
 * after tx_train complete successfully, before moving to tx_done_ok.
 */
union rpmx_anp_portx_sd_txt_ok_max_timer {
	u64 u;
	struct rpmx_anp_portx_sd_txt_ok_max_timer_s {
		u64 reg_tx_train_ok_wait_timeout_s   : 31;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_sd_txt_ok_max_timer_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SD_TXT_OK_MAX_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SD_TXT_OK_MAX_TIMER(u64 a)
{
	return 0x5c150 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_serdes_rx_sm_control
 *
 * RPM Anp Port Serdes Rx Sm Control Register Control Register for COMPHY
 * RX SM. provides - state status, ability to override state, ability to
 * trap state.
 */
union rpmx_anp_portx_serdes_rx_sm_control {
	u64 u;
	struct rpmx_anp_portx_serdes_rx_sm_control_s {
		u64 sd_rx_sm_override_ctrl           : 2;
		u64 sd_rx_sm_amdisam                 : 1;
		u64 sd_rx_sm_bp_reached              : 1;
		u64 sd_rx_sm_state                   : 4;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_anp_portx_serdes_rx_sm_control_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SERDES_RX_SM_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SERDES_RX_SM_CONTROL(u64 a)
{
	return 0x5c030 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_serdes_tx_sm_control
 *
 * RPM Anp Port Serdes Tx Sm Control Register Control Register for COMPHY
 * TX SM. provides - state status, ability to override state, ability to
 * trap state.
 */
union rpmx_anp_portx_serdes_tx_sm_control {
	u64 u;
	struct rpmx_anp_portx_serdes_tx_sm_control_s {
		u64 sd_tx_sm_override_ctrl           : 2;
		u64 sd_tx_sm_amdisam                 : 1;
		u64 sd_tx_sm_bp_reached              : 1;
		u64 sd_tx_sm_state                   : 4;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_anp_portx_serdes_tx_sm_control_s cn; */
};

static inline u64 RPMX_ANP_PORTX_SERDES_TX_SM_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_SERDES_TX_SM_CONTROL(u64 a)
{
	return 0x5c028 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_status
 *
 * RPM Anp Port Status Register Status + Self-Clear configurations.
 */
union rpmx_anp_portx_status {
	u64 u;
	struct rpmx_anp_portx_status_s {
		u64 reg_tx_train_complete            : 1;
		u64 reg_rx_train_complete            : 1;
		u64 reg_tx_train_failed              : 1;
		u64 reg_rx_train_failed              : 1;
		u64 pcs_cfg_done                     : 1;
		u64 reg_prog_pwm_done_s              : 1;
		u64 reg_prog_rx_done_s               : 1;
		u64 reg_prog_tx_done_s               : 1;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_anp_portx_status_s cn; */
};

static inline u64 RPMX_ANP_PORTX_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_STATUS(u64 a)
{
	return 0x5c1d8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_status1
 *
 * RPM Anp Port Status1 Register Status of internal signals.
 */
union rpmx_anp_portx_status1 {
	u64 u;
	struct rpmx_anp_portx_status1_s {
		u64 stat_dsp_rxstr_req_mx_s          : 1;
		u64 stat_dsp_rxstr_req_s             : 1;
		u64 stat_dsp_txstr_req_mx_s          : 1;
		u64 stat_dsp_txstr_req_s             : 1;
		u64 stat_pm_an_en_hcd_resolved       : 1;
		u64 stat_pm_an_hcd_clear             : 1;
		u64 stat_pm_an_hcd_resolved          : 1;
		u64 stat_pm_an_pcs_clkout_sel        : 1;
		u64 stat_pm_an_pcs_sel               : 1;
		u64 stat_pm_an_restart               : 1;
		u64 stat_pm_ap_en_s                  : 1;
		u64 stat_pm_ap_mode_s                : 1;
		u64 stat_pm_ap_reset_rx_s            : 1;
		u64 stat_pm_ap_reset_tx_s            : 1;
		u64 stat_pm_enclk_ap_fr_s            : 1;
		u64 stat_pm_enclk_ap_ft_s            : 1;
		u64 stat_pm_enclk_ap_sys_s           : 1;
		u64 stat_pm_mode_100gr2              : 1;
		u64 stat_pm_mode_100gr4              : 1;
		u64 stat_pm_mode_107gr2              : 1;
		u64 stat_pm_mode_10g                 : 1;
		u64 stat_pm_mode_110gr4              : 1;
		u64 stat_pm_mode_1g                  : 1;
		u64 stat_pm_mode_200gr4              : 1;
		u64 stat_pm_mode_200gr8              : 1;
		u64 stat_pm_mode_25g                 : 1;
		u64 stat_pm_mode_2p5g                : 1;
		u64 stat_pm_mode_400gr8              : 1;
		u64 stat_pm_mode_40gr2               : 1;
		u64 stat_pm_mode_40gr4               : 1;
		u64 stat_pm_mode_428gr8              : 1;
		u64 stat_pm_mode_50gr                : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_status1_s cn; */
};

static inline u64 RPMX_ANP_PORTX_STATUS1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_STATUS1(u64 a)
{
	return 0x5c1e0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_status2
 *
 * RPM Anp Port Status2 Register Internal signals status.
 */
union rpmx_anp_portx_status2 {
	u64 u;
	struct rpmx_anp_portx_status2_s {
		u64 stat_pm_mode_50gr2               : 1;
		u64 stat_pm_mode_5g                  : 1;
		u64 stat_pm_mode_custom              : 1;
		u64 stat_pm_mode_qsgmii              : 1;
		u64 stat_pm_mode_usgmii              : 1;
		u64 stat_pm_mode_usx10g              : 1;
		u64 stat_pm_mode_usx20g              : 1;
		u64 stat_pm_mode_usx2p5g             : 1;
		u64 stat_pm_mode_usx5g               : 1;
		u64 stat_pm_norm_x_state_s           : 1;
		u64 stat_pm_nr_reset_s               : 1;
		u64 stat_pm_pcs_couple_s             : 1;
		u64 stat_pm_pcs_rx_clk_ena           : 1;
		u64 stat_pm_pcs_sd_rx_reset_n        : 1;
		u64 stat_pm_pcs_sd_tx_reset_n        : 1;
		u64 stat_pm_pcs_tx_clk_ena           : 1;
		u64 stat_pm_pwrdn_s                  : 1;
		u64 stat_pm_sd_couple_mode_en_s      : 1;
		u64 stat_pm_sd_phy_gen_rx_s          : 5;
		u64 stat_pm_sd_phy_gen_tx_s          : 5;
		u64 stat_pm_sd_pu_pll_s              : 1;
		u64 stat_pm_sd_pu_rx_s               : 1;
		u64 reserved_30_63                   : 34;
	} s;
	/* struct rpmx_anp_portx_status2_s cn; */
};

static inline u64 RPMX_ANP_PORTX_STATUS2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_STATUS2(u64 a)
{
	return 0x5c1e8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_status3
 *
 * RPM Anp Port Status3 Register Internal signals status.
 */
union rpmx_anp_portx_status3 {
	u64 u;
	struct rpmx_anp_portx_status3_s {
		u64 stat_pm_sd_pu_tx_s               : 1;
		u64 stat_pm_sd_softrst_s             : 1;
		u64 stat_pm_sd_txclk_sync_en_pll_s   : 1;
		u64 stat_pm_softrst_s                : 1;
		u64 stat_pm_st_en_s                  : 1;
		u64 stat_pm_tx_train_poly_sel_s      : 4;
		u64 stat_dsp_lock_s                  : 1;
		u64 stat_dsp_pwrdn_ack_s             : 1;
		u64 stat_dsp_rxdn_ack_s              : 1;
		u64 stat_dsp_sigdet_s                : 1;
		u64 stat_dsp_txdn_ack_s              : 1;
		u64 stat_pcs_lock_s                  : 1;
		u64 stat_tx_ready_s                  : 1;
		u64 stat_pll_ready_rx_clean_s        : 1;
		u64 stat_pll_ready_rx_s              : 1;
		u64 stat_pll_ready_tx_clean_s        : 1;
		u64 stat_pll_ready_tx_s              : 1;
		u64 stat_pm_dsp_lock_s               : 1;
		u64 stat_pm_dsp_rxdn_ack_s           : 1;
		u64 stat_pm_dsp_rxstr_ack_s          : 1;
		u64 stat_pm_dsp_sigdet_s             : 1;
		u64 stat_pm_dsp_txdn_ack_s           : 1;
		u64 stat_pm_dsp_tx_ready_s           : 1;
		u64 stat_pm_dsp_txstr_ack_s          : 1;
		u64 stat_pm_rx_init_s                : 1;
		u64 stat_pm_rx_train_enable_s        : 1;
		u64 stat_pm_sd_dfe_en_s              : 1;
		u64 stat_pm_sd_dfe_pat_dis_s         : 1;
		u64 stat_pm_sd_dfe_update_dis_s      : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_anp_portx_status3_s cn; */
};

static inline u64 RPMX_ANP_PORTX_STATUS3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_STATUS3(u64 a)
{
	return 0x5c1f0 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_status4
 *
 * RPM Anp Port Status4 Register Internal signals status.
 */
union rpmx_anp_portx_status4 {
	u64 u;
	struct rpmx_anp_portx_status4_s {
		u64 stat_pm_sd_tx_idle_s             : 1;
		u64 stat_pm_train_type_s             : 2;
		u64 stat_pm_tx_train_enable_s        : 1;
		u64 stat_rx_init_done_s              : 1;
		u64 stat_rx_train_complete_s         : 1;
		u64 stat_rx_train_failed_s           : 1;
		u64 stat_sd_rx_dtl_clamp_s           : 1;
		u64 stat_sd_txclk_sync_start_out_s   : 1;
		u64 stat_sq_detected_lpf_s           : 1;
		u64 stat_tx_train_complete_s         : 1;
		u64 stat_tx_train_error_l            : 2;
		u64 stat_tx_train_error_s            : 2;
		u64 stat_tx_train_failed_s           : 1;
		u64 stat_pcs_cfg_done_fin_s          : 1;
		u64 stat_sd_busy_rx_s                : 1;
		u64 stat_sd_busy_tx_s                : 1;
		u64 stat_pm_pu_rx_req_s              : 1;
		u64 stat_pm_pu_tx_req_s              : 1;
		u64 stat_pm_st_normal_s              : 1;
		u64 stat_pm_st_pwrdn_s               : 1;
		u64 stat_pm_tx_idle_s                : 1;
		u64 stat_sd_phy_gen_s                : 5;
		u64 stat_cmem_state                  : 2;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_status4_s cn; */
};

static inline u64 RPMX_ANP_PORTX_STATUS4(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_STATUS4(u64 a)
{
	return 0x5c1f8 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_anp_port#_status5
 *
 * RPM Anp Port Status5 Register Command interface SMs states + TFIFOs
 * fill levels.
 */
union rpmx_anp_portx_status5 {
	u64 u;
	struct rpmx_anp_portx_status5_s {
		u64 stat_pm_cmem_addr_s              : 6;
		u64 stat_int_state                   : 2;
		u64 stat_prog_state_s                : 3;
		u64 reg_tx_tfifo_uw_w                : 5;
		u64 reg_tx_tfifo_uw_r                : 5;
		u64 reg_rx_tfifo_uw_w                : 5;
		u64 reg_rx_tfifo_uw_r                : 5;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_anp_portx_status5_s cn; */
};

static inline u64 RPMX_ANP_PORTX_STATUS5(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_ANP_PORTX_STATUS5(u64 a)
{
	return 0x5c200 + 0x800 * a;
}

/**
 * Register (RSL) rpm#_cmr#_activity
 *
 * RPM CMR Activity Registers
 */
union rpmx_cmrx_activity {
	u64 u;
	struct rpmx_cmrx_activity_s {
		u64 pause_tx_lat                     : 1;
		u64 pause_rx_lat                     : 1;
		u64 stop_tx_lat                      : 1;
		u64 reserved_3_63                    : 61;
	} s;
	/* struct rpmx_cmrx_activity_s cn; */
};

static inline u64 RPMX_CMRX_ACTIVITY(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_ACTIVITY(u64 a)
{
	return 0x45f8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_config
 *
 * RPM CMR Configuration Registers Logical MAC/PCS configuration
 * registers; one per LMAC. The maximum number of LMACs (and maximum LMAC
 * ID) that can be enabled by these registers is limited by
 * RPM()_CMR_RX_LMACS[LMACS] and RPM()_CMR_TX_LMACS[LMACS].  Internal:
 * \<pre\> Example configurations:   ------------------------------------
 * ---------------------------------------   Configuration
 * LMACS  Register             [ENABLE]    [LMAC_TYPE]   ----------------
 * -----------------------------------------------------------
 * 1x50G+1x25G+1xSGMII     4      RPMn_CMR0_CONFIG     1           8
 * RPMn_CMR1_CONFIG     0           --
 * RPMn_CMR2_CONFIG     1           7
 * RPMn_CMR3_CONFIG     1           0   ---------------------------------
 * ------------------------------------------   USXGMII
 * 1-4    RPMn_CMR0_CONFIG     1           a
 * RPMn_CMR1_CONFIG     1           a
 * RPMn_CMR2_CONFIG     1           a
 * RPMn_CMR3_CONFIG     1           a   ---------------------------------
 * ------------------------------------------   1x100GBASE-R4           1
 * RPMn_CMR0_CONFIG     1           9
 * RPMn_CMR1_CONFIG     0           --
 * RPMn_CMR2_CONFIG     0           --
 * RPMn_CMR3_CONFIG     0           --   --------------------------------
 * -------------------------------------------   2x50GBASE-R2
 * 2      RPMn_CMR0_CONFIG     1           8
 * RPMn_CMR1_CONFIG     1           8
 * RPMn_CMR2_CONFIG     0           --
 * RPMn_CMR3_CONFIG     0           --   --------------------------------
 * -------------------------------------------   4x25GBASE-R
 * 4      RPMn_CMR0_CONFIG     1           7
 * RPMn_CMR1_CONFIG     1           7
 * RPMn_CMR2_CONFIG     1           7
 * RPMn_CMR3_CONFIG     1           7   ---------------------------------
 * ------------------------------------------   QSGMII                  4
 * RPMn_CMR0_CONFIG     1           6
 * RPMn_CMR1_CONFIG     1           6
 * RPMn_CMR2_CONFIG     1           6
 * RPMn_CMR3_CONFIG     1           6   ---------------------------------
 * ------------------------------------------   1x40GBASE-R4            1
 * RPMn_CMR0_CONFIG     1           4
 * RPMn_CMR1_CONFIG     0           --
 * RPMn_CMR2_CONFIG     0           --
 * RPMn_CMR3_CONFIG     0           --   --------------------------------
 * -------------------------------------------   4x10GBASE-R
 * 4      RPMn_CMR0_CONFIG     1           3
 * RPMn_CMR1_CONFIG     1           3
 * RPMn_CMR2_CONFIG     1           3
 * RPMn_CMR3_CONFIG     1           3   ---------------------------------
 * ------------------------------------------   2xRXAUI                 2
 * RPMn_CMR0_CONFIG     1           2
 * RPMn_CMR1_CONFIG     1           2
 * RPMn_CMR2_CONFIG     0           --
 * RPMn_CMR3_CONFIG     0           --   --------------------------------
 * -------------------------------------------   1x10GBASE-X/XAUI/DXAUI
 * 1      RPMn_CMR0_CONFIG     1           1
 * RPMn_CMR1_CONFIG     0           --
 * RPMn_CMR2_CONFIG     0           --
 * RPMn_CMR3_CONFIG     0           --   --------------------------------
 * -------------------------------------------   4xSGMII/1000BASE-X
 * 4      RPMn_CMR0_CONFIG     1           0
 * RPMn_CMR1_CONFIG     1           0
 * RPMn_CMR2_CONFIG     1           0
 * RPMn_CMR3_CONFIG     1           0   ---------------------------------
 * ------------------------------------------ \</pre\>
 */
union rpmx_cmrx_config {
	u64 u;
	struct rpmx_cmrx_config_s {
		u64 lane_to_sds                      : 8;
		u64 reserved_8_15                    : 8;
		u64 rx_byte_flip                     : 1;
		u64 tx_byte_flip                     : 1;
		u64 reserved_18_39                   : 22;
		u64 lmac_type                        : 4;
		u64 unused                           : 8;
		u64 int_beat_gen                     : 1;
		u64 data_pkt_tx_en                   : 1;
		u64 data_pkt_rx_en                   : 1;
		u64 enable                           : 1;
		u64 x2p_select                       : 3;
		u64 p2x_select                       : 3;
		u64 reserved_62_63                   : 2;
	} s;
	/* struct rpmx_cmrx_config_s cn; */
};

static inline u64 RPMX_CMRX_CONFIG(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_CONFIG(u64 a)
{
	return 0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_fc_status
 *
 * RPM CMR Activity Registers
 */
union rpmx_cmrx_fc_status {
	u64 u;
	struct rpmx_cmrx_fc_status_s {
		u64 pause_tx_stat                    : 16;
		u64 pause_rx_stat                    : 16;
		u64 stop_tx_stat                     : 1;
		u64 reserved_33_63                   : 31;
	} s;
	/* struct rpmx_cmrx_fc_status_s cn; */
};

static inline u64 RPMX_CMRX_FC_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_FC_STATUS(u64 a)
{
	return 0x4610 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_int
 *
 * RPM CMR Interrupt Register
 */
union rpmx_cmrx_int {
	u64 u;
	struct rpmx_cmrx_int_s {
		u64 reserved_0                       : 1;
		u64 overflw                          : 1;
		u64 nic_nxc                          : 1;
		u64 nix0_nxc                         : 1;
		u64 nix1_nxc                         : 1;
		u64 reserved_5_63                    : 59;
	} s;
	/* struct rpmx_cmrx_int_s cn; */
};

static inline u64 RPMX_CMRX_INT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_INT(u64 a)
{
	return 0x40 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_int_ena_w1c
 *
 * RPM CMR Interrupt Enable Clear Register This register clears interrupt
 * enable bits.
 */
union rpmx_cmrx_int_ena_w1c {
	u64 u;
	struct rpmx_cmrx_int_ena_w1c_s {
		u64 reserved_0                       : 1;
		u64 overflw                          : 1;
		u64 nic_nxc                          : 1;
		u64 nix0_nxc                         : 1;
		u64 nix1_nxc                         : 1;
		u64 reserved_5_63                    : 59;
	} s;
	/* struct rpmx_cmrx_int_ena_w1c_s cn; */
};

static inline u64 RPMX_CMRX_INT_ENA_W1C(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_INT_ENA_W1C(u64 a)
{
	return 0x50 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_int_ena_w1s
 *
 * RPM CMR Interrupt Enable Set Register This register sets interrupt
 * enable bits.
 */
union rpmx_cmrx_int_ena_w1s {
	u64 u;
	struct rpmx_cmrx_int_ena_w1s_s {
		u64 reserved_0                       : 1;
		u64 overflw                          : 1;
		u64 nic_nxc                          : 1;
		u64 nix0_nxc                         : 1;
		u64 nix1_nxc                         : 1;
		u64 reserved_5_63                    : 59;
	} s;
	/* struct rpmx_cmrx_int_ena_w1s_s cn; */
};

static inline u64 RPMX_CMRX_INT_ENA_W1S(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_INT_ENA_W1S(u64 a)
{
	return 0x58 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_int_w1s
 *
 * RPM CMR Interrupt Set Register This register sets interrupt bits.
 */
union rpmx_cmrx_int_w1s {
	u64 u;
	struct rpmx_cmrx_int_w1s_s {
		u64 reserved_0                       : 1;
		u64 overflw                          : 1;
		u64 nic_nxc                          : 1;
		u64 nix0_nxc                         : 1;
		u64 nix1_nxc                         : 1;
		u64 reserved_5_63                    : 59;
	} s;
	/* struct rpmx_cmrx_int_w1s_s cn; */
};

static inline u64 RPMX_CMRX_INT_W1S(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_INT_W1S(u64 a)
{
	return 0x48 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_link_cfg
 *
 * Programmable Link Channel Register Each register specifies the base
 * channel (start channel) number and the range of channels associated
 * with the link.
 */
union rpmx_cmrx_link_cfg {
	u64 u;
	struct rpmx_cmrx_link_cfg_s {
		u64 base_chan                        : 12;
		u64 reserved_12_15                   : 4;
		u64 log2_range                       : 4;
		u64 reserved_20_63                   : 44;
	} s;
	/* struct rpmx_cmrx_link_cfg_s cn; */
};

static inline u64 RPMX_CMRX_LINK_CFG(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_LINK_CFG(u64 a)
{
	return 0x1070 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_prt_cbfc_ctl
 *
 * RPM CMR LMAC PFC Control Registers Controls for masking the effect of
 * specific classes and channels on FC logic
 */
union rpmx_cmrx_prt_cbfc_ctl {
	u64 u;
	struct rpmx_cmrx_prt_cbfc_ctl_s {
		u64 logl_en_tx                       : 16;
		u64 phys_bp                          : 16;
		u64 pause_mode_stop_tx_en            : 1;
		u64 logl_en_rx                       : 16;
		u64 reserved_49_63                   : 15;
	} s;
	/* struct rpmx_cmrx_prt_cbfc_ctl_s cn; */
};

static inline u64 RPMX_CMRX_PRT_CBFC_CTL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_PRT_CBFC_CTL(u64 a)
{
	return 0x4608 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_bp_drop
 *
 * RPM Receive Backpressure Drop Register
 */
union rpmx_cmrx_rx_bp_drop {
	u64 u;
	struct rpmx_cmrx_rx_bp_drop_s {
		u64 mark                             : 7;
		u64 reserved_7_63                    : 57;
	} s;
	/* struct rpmx_cmrx_rx_bp_drop_s cn; */
};

static inline u64 RPMX_CMRX_RX_BP_DROP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_BP_DROP(u64 a)
{
	return 0x40d8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_bp_off
 *
 * RPM Receive Backpressure Off Register
 */
union rpmx_cmrx_rx_bp_off {
	u64 u;
	struct rpmx_cmrx_rx_bp_off_s {
		u64 mark                             : 7;
		u64 reserved_7_63                    : 57;
	} s;
	/* struct rpmx_cmrx_rx_bp_off_s cn; */
};

static inline u64 RPMX_CMRX_RX_BP_OFF(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_BP_OFF(u64 a)
{
	return 0x40e8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_bp_on
 *
 * RPM Receive Backpressure On Register
 */
union rpmx_cmrx_rx_bp_on {
	u64 u;
	struct rpmx_cmrx_rx_bp_on_s {
		u64 mark                             : 14;
		u64 reserved_14_63                   : 50;
	} s;
	/* struct rpmx_cmrx_rx_bp_on_s cn; */
};

static inline u64 RPMX_CMRX_RX_BP_ON(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_BP_ON(u64 a)
{
	return 0x40e0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_dmac_ctl0
 *
 * RPM CMR Receive DMAC Address-Control0 Register DMAC CAM control
 * register for use by X2P/NIX bound traffic. Received packets are only
 * passed to X2P/NIX when the DMAC0 filter result is ACCEPT and STEERING0
 * filter result is PASS. See also RPM()_CMR_RX_DMAC()_CAM0 and
 * RPM()_CMR_RX_STEERING0().  Internal: "* ALGORITHM Here is some pseudo
 * code that represents the address filter behavior. \<pre\>
 * dmac_addr_filter(uint8 prt, uint48 dmac) { for (lmac=0, lmac\<4,
 * lmac++) {   if (is_bcst(dmac))                               //
 * broadcast accept     return (RPM()_CMR(lmac)_RX_DMAC_CTL0[BCST_ACCEPT]
 * ? ACCEPT : REJECT);   if (is_mcst(dmac) &&
 * RPM()_CMR(lmac)_RX_DMAC_CTL0[MCST_MODE] == 0)   // multicast reject
 * return REJECT;   if (is_mcst(dmac) &&
 * RPM()_CMR(lmac)_RX_DMAC_CTL0[MCST_MODE] == 1)   // multicast accept
 * return ACCEPT;   else        // DMAC CAM filter     cam_hit = 0;   for
 * (i=0; i\<32; i++) {     cam = RPM()_CMR_RX_DMAC(i)_CAM0;     if
 * (cam[EN] && cam[ID] == lmac && cam[ADR] == dmac) {       cam_hit = 1;
 * break;     }   }   if (cam_hit) {     return
 * (RPM()_CMR(lmac)_RX_DMAC_CTL0[CAM_ACCEPT] ? ACCEPT : REJECT);   else
 * return (RPM()_CMR(lmac)_RX_DMAC_CTL0[CAM_ACCEPT] ? REJECT : ACCEPT);
 * } } \</pre\>"
 */
union rpmx_cmrx_rx_dmac_ctl0 {
	u64 u;
	struct rpmx_cmrx_rx_dmac_ctl0_s {
		u64 bcst_accept                      : 1;
		u64 mcst_mode                        : 2;
		u64 cam_accept                       : 1;
		u64 reserved_4_63                    : 60;
	} s;
	/* struct rpmx_cmrx_rx_dmac_ctl0_s cn; */
};

static inline u64 RPMX_CMRX_RX_DMAC_CTL0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_DMAC_CTL0(u64 a)
{
	return 0x41f8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_fifo_len
 *
 * RPM CMR Receive Fifo Length Registers
 */
union rpmx_cmrx_rx_fifo_len {
	u64 u;
	struct rpmx_cmrx_rx_fifo_len_s {
		u64 fifo_len                         : 14;
		u64 busy                             : 1;
		u64 reserved_15_63                   : 49;
	} s;
	/* struct rpmx_cmrx_rx_fifo_len_s cn; */
};

static inline u64 RPMX_CMRX_RX_FIFO_LEN(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_FIFO_LEN(u64 a)
{
	return 0x4108 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_id_map
 *
 * RPM CMR Receive ID Map Register These registers set the RX LMAC ID
 * mapping for X2P/NIX.
 */
union rpmx_cmrx_rx_id_map {
	u64 u;
	struct rpmx_cmrx_rx_id_map_s {
		u64 pknd                             : 6;
		u64 unused                           : 2;
		u64 rid                              : 7;
		u64 reserved_15_63                   : 49;
	} s;
	/* struct rpmx_cmrx_rx_id_map_s cn; */
};

static inline u64 RPMX_CMRX_RX_ID_MAP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_ID_MAP(u64 a)
{
	return 0x60 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_logl_xoff
 *
 * RPM CMR Receive Logical XOFF Registers
 */
union rpmx_cmrx_rx_logl_xoff {
	u64 u;
	struct rpmx_cmrx_rx_logl_xoff_s {
		u64 xoff                             : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_cmrx_rx_logl_xoff_s cn; */
};

static inline u64 RPMX_CMRX_RX_LOGL_XOFF(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_LOGL_XOFF(u64 a)
{
	return 0x40f8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_logl_xon
 *
 * RPM CMR Receive Logical XON Registers
 */
union rpmx_cmrx_rx_logl_xon {
	u64 u;
	struct rpmx_cmrx_rx_logl_xon_s {
		u64 xon                              : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_cmrx_rx_logl_xon_s cn; */
};

static inline u64 RPMX_CMRX_RX_LOGL_XON(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_LOGL_XON(u64 a)
{
	return 0x4100 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat0
 *
 * RPM Receive Status Register 0 These registers provide a count of
 * received packets that meet the following conditions: * are not
 * recognized as ERROR packets(any OPCODE). * are not recognized as PAUSE
 * packets. * are not dropped due FIFO full status. * are not dropped due
 * DMAC0 or STEERING0 filtering.  Internal: "This pseudo code represents
 * the RX STAT0 through STAT8 accounting: \<pre\> If (errored)   incr
 * RX_STAT8 else if (ctrl packet, i.e. Pause/PFC)   incr RX_STAT2,3 else
 * if (fifo full drop)   incr RX_STAT6,7 else if (DMAC0/VLAN0 filter
 * drop)   incr RX_STAT4,5 if not a filter+decision else   incr
 * RX_STAT0,1 end \</pre\>"
 */
union rpmx_cmrx_rx_stat0 {
	u64 u;
	struct rpmx_cmrx_rx_stat0_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat0_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT0(u64 a)
{
	return 0x4070 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat1
 *
 * RPM Receive Status Register 1 These registers provide a count of
 * octets of received packets.
 */
union rpmx_cmrx_rx_stat1 {
	u64 u;
	struct rpmx_cmrx_rx_stat1_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat1_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT1(u64 a)
{
	return 0x4078 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat2
 *
 * RPM Receive Status Register 2 These registers provide a count of
 * received packets that meet the following conditions: * are not
 * recognized as ERROR packets(any OPCODE). * are recognized as PAUSE
 * packets.  Pause packets can be optionally dropped or forwarded based
 * on
 * RPM()_SMU()_RX_FRM_CTL[CTL_DRP]/RPM()_GMP_GMI_RX()_FRM_CTL[CTL_DRP].
 * This count increments regardless of whether the packet is dropped.
 */
union rpmx_cmrx_rx_stat2 {
	u64 u;
	struct rpmx_cmrx_rx_stat2_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat2_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT2(u64 a)
{
	return 0x4080 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat3
 *
 * RPM Receive Status Register 3 These registers provide a count of
 * octets of received PAUSE and control packets.
 */
union rpmx_cmrx_rx_stat3 {
	u64 u;
	struct rpmx_cmrx_rx_stat3_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat3_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT3(u64 a)
{
	return 0x4088 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat4
 *
 * RPM Receive Status Register 4 These registers provide a count of
 * received packets that meet the following conditions: * are not
 * recognized as ERROR packets(any OPCODE). * are not recognized as PAUSE
 * packets. * are not dropped due FIFO full status. * are dropped due
 * DMAC0 or STEERING0 filtering.  16B packets or smaller (20B in case of
 * FCS strip) as the result of truncation or other means are not dropped
 * by RPM (unless filter and decision is also asserted) and will never
 * appear in this count. Should the MAC signal to the CMR that the packet
 * be filtered upon decision before the end of packet, then STAT4 and
 * STAT5 will not be updated.
 */
union rpmx_cmrx_rx_stat4 {
	u64 u;
	struct rpmx_cmrx_rx_stat4_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat4_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT4(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT4(u64 a)
{
	return 0x4090 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat5
 *
 * RPM Receive Status Register 5 These registers provide a count of
 * octets of filtered DMAC0 or VLAN STEERING0 packets.
 */
union rpmx_cmrx_rx_stat5 {
	u64 u;
	struct rpmx_cmrx_rx_stat5_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat5_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT5(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT5(u64 a)
{
	return 0x4098 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat6
 *
 * RPM Receive Status Register 6 These registers provide a count of
 * received packets that meet the following conditions: * are not
 * recognized as ERROR packets(any OPCODE). * are not recognized as PAUSE
 * packets. * are dropped due FIFO full status.  They do not count any
 * packet that is truncated at the point of overflow and sent on to the
 * NIX. The truncated packet will be marked with error and increment
 * STAT8. These registers count all entire packets dropped by the FIFO
 * for a given LMAC.
 */
union rpmx_cmrx_rx_stat6 {
	u64 u;
	struct rpmx_cmrx_rx_stat6_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat6_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT6(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT6(u64 a)
{
	return 0x40a0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat7
 *
 * RPM Receive Status Register 7 These registers provide a count of
 * octets of received packets that were dropped due to a full receive
 * FIFO.
 */
union rpmx_cmrx_rx_stat7 {
	u64 u;
	struct rpmx_cmrx_rx_stat7_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat7_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT7(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT7(u64 a)
{
	return 0x40a8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat8
 *
 * RPM Receive Status Register 8 These registers provide a count of
 * received packets that meet the following conditions:  * are recognized
 * as ERROR packets(any OPCODE).
 */
union rpmx_cmrx_rx_stat8 {
	u64 u;
	struct rpmx_cmrx_rx_stat8_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat8_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT8(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT8(u64 a)
{
	return 0x40b0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat_defer_xoff
 *
 * RPM CMR Tx Defer XON to XOFF transition Registers
 */
union rpmx_cmrx_rx_stat_defer_xoff {
	u64 u;
	struct rpmx_cmrx_rx_stat_defer_xoff_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat_defer_xoff_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT_DEFER_XOFF(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT_DEFER_XOFF(u64 a)
{
	return 0x4880 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_rx_stat_pri#_xoff
 *
 * RPM CMR RX XON to XOFF transition Registers
 */
union rpmx_cmrx_rx_stat_prix_xoff {
	u64 u;
	struct rpmx_cmrx_rx_stat_prix_xoff_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_rx_stat_prix_xoff_s cn; */
};

static inline u64 RPMX_CMRX_RX_STAT_PRIX_XOFF(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_RX_STAT_PRIX_XOFF(u64 a, u64 b)
{
	return 0x4800 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_cmr#_scratch#
 *
 * RPM CMR Scratch Registers
 */
union rpmx_cmrx_scratchx {
	u64 u;
	struct rpmx_cmrx_scratchx_s {
		u64 scratch                          : 64;
	} s;
	/* struct rpmx_cmrx_scratchx_s cn; */
};

static inline u64 RPMX_CMRX_SCRATCHX(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_SCRATCHX(u64 a, u64 b)
{
	return 0x1050 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_cmr#_sw_int
 *
 * RPM CMR Interrupt Register
 */
union rpmx_cmrx_sw_int {
	u64 u;
	struct rpmx_cmrx_sw_int_s {
		u64 sw_set                           : 1;
		u64 reserved_1_63                    : 63;
	} s;
	/* struct rpmx_cmrx_sw_int_s cn; */
};

static inline u64 RPMX_CMRX_SW_INT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_SW_INT(u64 a)
{
	return 0x180 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_sw_int_ena_w1c
 *
 * RPM CMR Interrupt Enable Clear Register This register clears interrupt
 * enable bits.
 */
union rpmx_cmrx_sw_int_ena_w1c {
	u64 u;
	struct rpmx_cmrx_sw_int_ena_w1c_s {
		u64 sw_set                           : 1;
		u64 reserved_1_63                    : 63;
	} s;
	/* struct rpmx_cmrx_sw_int_ena_w1c_s cn; */
};

static inline u64 RPMX_CMRX_SW_INT_ENA_W1C(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_SW_INT_ENA_W1C(u64 a)
{
	return 0x190 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_sw_int_ena_w1s
 *
 * RPM CMR Interrupt Enable Set Register This register sets interrupt
 * enable bits.
 */
union rpmx_cmrx_sw_int_ena_w1s {
	u64 u;
	struct rpmx_cmrx_sw_int_ena_w1s_s {
		u64 sw_set                           : 1;
		u64 reserved_1_63                    : 63;
	} s;
	/* struct rpmx_cmrx_sw_int_ena_w1s_s cn; */
};

static inline u64 RPMX_CMRX_SW_INT_ENA_W1S(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_SW_INT_ENA_W1S(u64 a)
{
	return 0x198 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_sw_int_w1s
 *
 * RPM CMR Interrupt Set Register This register sets interrupt bits.
 */
union rpmx_cmrx_sw_int_w1s {
	u64 u;
	struct rpmx_cmrx_sw_int_w1s_s {
		u64 sw_set                           : 1;
		u64 reserved_1_63                    : 63;
	} s;
	/* struct rpmx_cmrx_sw_int_w1s_s cn; */
};

static inline u64 RPMX_CMRX_SW_INT_W1S(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_SW_INT_W1S(u64 a)
{
	return 0x188 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_channel
 *
 * RPM CMR Transmit-Channels Registers
 */
union rpmx_cmrx_tx_channel {
	u64 u;
	struct rpmx_cmrx_tx_channel_s {
		u64 msk                              : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_cmrx_tx_channel_s cn; */
};

static inline u64 RPMX_CMRX_TX_CHANNEL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_CHANNEL(u64 a)
{
	return 0x4600 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_fifo_len
 *
 * RPM CMR Transmit Fifo Length Registers
 */
union rpmx_cmrx_tx_fifo_len {
	u64 u;
	struct rpmx_cmrx_tx_fifo_len_s {
		u64 fifo_len                         : 14;
		u64 lmac_idle                        : 1;
		u64 reserved_15_63                   : 49;
	} s;
	/* struct rpmx_cmrx_tx_fifo_len_s cn; */
};

static inline u64 RPMX_CMRX_TX_FIFO_LEN(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_FIFO_LEN(u64 a)
{
	return 0x4618 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_ovr_bp
 *
 * RPM CMR Transmit-Channels Backpressure Override Registers
 */
union rpmx_cmrx_tx_ovr_bp {
	u64 u;
	struct rpmx_cmrx_tx_ovr_bp_s {
		u64 tx_chan_bp                       : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_cmrx_tx_ovr_bp_s cn; */
};

static inline u64 RPMX_CMRX_TX_OVR_BP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_OVR_BP(u64 a)
{
	return 0x4620 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat0
 *
 * RPM CMR Transmit Statistics Registers 0
 */
union rpmx_cmrx_tx_stat0 {
	u64 u;
	struct rpmx_cmrx_tx_stat0_s {
		u64 xscol                            : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat0_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT0(u64 a)
{
	return 0x4700 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat1
 *
 * RPM CMR Transmit Statistics Registers 1
 */
union rpmx_cmrx_tx_stat1 {
	u64 u;
	struct rpmx_cmrx_tx_stat1_s {
		u64 xsdef                            : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat1_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT1(u64 a)
{
	return 0x4708 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat10
 *
 * RPM CMR Transmit Statistics Registers 10
 */
union rpmx_cmrx_tx_stat10 {
	u64 u;
	struct rpmx_cmrx_tx_stat10_s {
		u64 hist4                            : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat10_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT10(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT10(u64 a)
{
	return 0x4750 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat11
 *
 * RPM CMR Transmit Statistics Registers 11
 */
union rpmx_cmrx_tx_stat11 {
	u64 u;
	struct rpmx_cmrx_tx_stat11_s {
		u64 hist5                            : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat11_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT11(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT11(u64 a)
{
	return 0x4758 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat12
 *
 * RPM CMR Transmit Statistics Registers 12
 */
union rpmx_cmrx_tx_stat12 {
	u64 u;
	struct rpmx_cmrx_tx_stat12_s {
		u64 hist6                            : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat12_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT12(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT12(u64 a)
{
	return 0x4760 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat13
 *
 * RPM CMR Transmit Statistics Registers 13
 */
union rpmx_cmrx_tx_stat13 {
	u64 u;
	struct rpmx_cmrx_tx_stat13_s {
		u64 hist7                            : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat13_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT13(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT13(u64 a)
{
	return 0x4768 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat14
 *
 * RPM CMR Transmit Statistics Registers 14
 */
union rpmx_cmrx_tx_stat14 {
	u64 u;
	struct rpmx_cmrx_tx_stat14_s {
		u64 bcst                             : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat14_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT14(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT14(u64 a)
{
	return 0x4770 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat15
 *
 * RPM CMR Transmit Statistics Registers 15
 */
union rpmx_cmrx_tx_stat15 {
	u64 u;
	struct rpmx_cmrx_tx_stat15_s {
		u64 mcst                             : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat15_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT15(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT15(u64 a)
{
	return 0x4778 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat16
 *
 * RPM CMR Transmit Statistics Registers 16
 */
union rpmx_cmrx_tx_stat16 {
	u64 u;
	struct rpmx_cmrx_tx_stat16_s {
		u64 undflw                           : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat16_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT16(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT16(u64 a)
{
	return 0x4780 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat17
 *
 * RPM CMR Transmit Statistics Registers 17
 */
union rpmx_cmrx_tx_stat17 {
	u64 u;
	struct rpmx_cmrx_tx_stat17_s {
		u64 ctl                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat17_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT17(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT17(u64 a)
{
	return 0x4788 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat2
 *
 * RPM CMR Transmit Statistics Registers 2
 */
union rpmx_cmrx_tx_stat2 {
	u64 u;
	struct rpmx_cmrx_tx_stat2_s {
		u64 mcol                             : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat2_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT2(u64 a)
{
	return 0x4710 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat3
 *
 * RPM CMR Transmit Statistics Registers 3
 */
union rpmx_cmrx_tx_stat3 {
	u64 u;
	struct rpmx_cmrx_tx_stat3_s {
		u64 scol                             : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat3_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT3(u64 a)
{
	return 0x4718 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat4
 *
 * RPM CMR Transmit Statistics Registers 4
 */
union rpmx_cmrx_tx_stat4 {
	u64 u;
	struct rpmx_cmrx_tx_stat4_s {
		u64 octs                             : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat4_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT4(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT4(u64 a)
{
	return 0x4720 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat5
 *
 * RPM CMR Transmit Statistics Registers 5
 */
union rpmx_cmrx_tx_stat5 {
	u64 u;
	struct rpmx_cmrx_tx_stat5_s {
		u64 pkts                             : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat5_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT5(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT5(u64 a)
{
	return 0x4728 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat6
 *
 * RPM CMR Transmit Statistics Registers 6
 */
union rpmx_cmrx_tx_stat6 {
	u64 u;
	struct rpmx_cmrx_tx_stat6_s {
		u64 hist0                            : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat6_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT6(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT6(u64 a)
{
	return 0x4730 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat7
 *
 * RPM CMR Transmit Statistics Registers 7
 */
union rpmx_cmrx_tx_stat7 {
	u64 u;
	struct rpmx_cmrx_tx_stat7_s {
		u64 hist1                            : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat7_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT7(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT7(u64 a)
{
	return 0x4738 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat8
 *
 * RPM CMR Transmit Statistics Registers 8
 */
union rpmx_cmrx_tx_stat8 {
	u64 u;
	struct rpmx_cmrx_tx_stat8_s {
		u64 hist2                            : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat8_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT8(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT8(u64 a)
{
	return 0x4740 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat9
 *
 * RPM CMR Transmit Statistics Registers 9
 */
union rpmx_cmrx_tx_stat9 {
	u64 u;
	struct rpmx_cmrx_tx_stat9_s {
		u64 hist3                            : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat9_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT9(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT9(u64 a)
{
	return 0x4748 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr#_tx_stat_pri#_xoff
 *
 * RPM CMR TX XON to XOFF transition Registers
 */
union rpmx_cmrx_tx_stat_prix_xoff {
	u64 u;
	struct rpmx_cmrx_tx_stat_prix_xoff_s {
		u64 cnt                              : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_cmrx_tx_stat_prix_xoff_s cn; */
};

static inline u64 RPMX_CMRX_TX_STAT_PRIX_XOFF(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_STAT_PRIX_XOFF(u64 a, u64 b)
{
	return 0x4900 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_cmr#_tx_thresh
 *
 * RPM CMR TX Threshold Registers
 */
union rpmx_cmrx_tx_thresh {
	u64 u;
	struct rpmx_cmrx_tx_thresh_s {
		u64 bulk_thresh                      : 13;
		u64 macout_thresh                    : 3;
		u64 ch_cred_only_after_mac_tx        : 1;
		u64 reserved_17_63                   : 47;
	} s;
	/* struct rpmx_cmrx_tx_thresh_s cn; */
};

static inline u64 RPMX_CMRX_TX_THRESH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMRX_TX_THRESH(u64 a)
{
	return 0x5070 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_cmr_bad
 *
 * RPM CMR Bad Registers
 */
union rpmx_cmr_bad {
	u64 u;
	struct rpmx_cmr_bad_s {
		u64 rxb_nxl_0                        : 1;
		u64 rxb_nxl_1                        : 1;
		u64 rxb_nxl_2                        : 1;
		u64 rxb_nxl_3                        : 1;
		u64 reserved_4_63                    : 60;
	} s;
	/* struct rpmx_cmr_bad_s cn; */
};

static inline u64 RPMX_CMR_BAD(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_BAD(void)
{
	return 0x5020;
}

/**
 * Register (RSL) rpm#_cmr_chan_msk_and
 *
 * RPM CMR Backpressure Channel Mask AND Registers
 */
union rpmx_cmr_chan_msk_and {
	u64 u;
	struct rpmx_cmr_chan_msk_and_s {
		u64 msk_and                          : 64;
	} s;
	/* struct rpmx_cmr_chan_msk_and_s cn; */
};

static inline u64 RPMX_CMR_CHAN_MSK_AND(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_CHAN_MSK_AND(void)
{
	return 0x4110;
}

/**
 * Register (RSL) rpm#_cmr_chan_msk_or
 *
 * RPM Backpressure Channel Mask OR Registers
 */
union rpmx_cmr_chan_msk_or {
	u64 u;
	struct rpmx_cmr_chan_msk_or_s {
		u64 msk_or                           : 64;
	} s;
	/* struct rpmx_cmr_chan_msk_or_s cn; */
};

static inline u64 RPMX_CMR_CHAN_MSK_OR(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_CHAN_MSK_OR(void)
{
	return 0x4118;
}

/**
 * Register (RSL) rpm#_cmr_eco_nck
 *
 * INTERNAL: RPM ECO netclk Registers
 */
union rpmx_cmr_eco_nck {
	u64 u;
	struct rpmx_cmr_eco_nck_s {
		u64 eco_rw                           : 32;
		u64 eco_ro                           : 32;
	} s;
	/* struct rpmx_cmr_eco_nck_s cn; */
};

static inline u64 RPMX_CMR_ECO_NCK(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_ECO_NCK(void)
{
	return 0x5088;
}

/**
 * Register (RSL) rpm#_cmr_eco_sck
 *
 * INTERNAL: RPM ECO sclk Registers
 */
union rpmx_cmr_eco_sck {
	u64 u;
	struct rpmx_cmr_eco_sck_s {
		u64 eco_rw                           : 32;
		u64 eco_ro                           : 32;
	} s;
	/* struct rpmx_cmr_eco_sck_s cn; */
};

static inline u64 RPMX_CMR_ECO_SCK(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_ECO_SCK(void)
{
	return 0x1080;
}

/**
 * Register (RSL) rpm#_cmr_global_config
 *
 * RPM CMR Global Configuration Register These registers configure the
 * global CMR, PCS, and MAC.
 */
union rpmx_cmr_global_config {
	u64 u;
	struct rpmx_cmr_global_config_s {
		u64 pmux_sds_sel                     : 1;
		u64 rpm_clk_enable                   : 1;
		u64 cmr_x2p_reset                    : 3;
		u64 interleave_mode                  : 1;
		u64 fcs_strip                        : 1;
		u64 cmr_clken_ovrd                   : 1;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_cmr_global_config_s cn; */
};

static inline u64 RPMX_CMR_GLOBAL_CONFIG(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_GLOBAL_CONFIG(void)
{
	return 8;
}

/**
 * Register (RSL) rpm#_cmr_mem_int
 *
 * RPM CMR Memory Interrupt Register
 */
union rpmx_cmr_mem_int {
	u64 u;
	struct rpmx_cmr_mem_int_s {
		u64 infifo_0_overfl                  : 1;
		u64 infifo_1_overfl                  : 1;
		u64 infifo_2_overfl                  : 1;
		u64 infifo_3_overfl                  : 1;
		u64 reserved_4_63                    : 60;
	} s;
	/* struct rpmx_cmr_mem_int_s cn; */
};

static inline u64 RPMX_CMR_MEM_INT(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_MEM_INT(void)
{
	return 0x10;
}

/**
 * Register (RSL) rpm#_cmr_mem_int_ena_w1c
 *
 * RPM CMR Memory Interrupt Enable Clear Register This register clears
 * interrupt enable bits.
 */
union rpmx_cmr_mem_int_ena_w1c {
	u64 u;
	struct rpmx_cmr_mem_int_ena_w1c_s {
		u64 infifo_0_overfl                  : 1;
		u64 infifo_1_overfl                  : 1;
		u64 infifo_2_overfl                  : 1;
		u64 infifo_3_overfl                  : 1;
		u64 reserved_4_63                    : 60;
	} s;
	/* struct rpmx_cmr_mem_int_ena_w1c_s cn; */
};

static inline u64 RPMX_CMR_MEM_INT_ENA_W1C(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_MEM_INT_ENA_W1C(void)
{
	return 0x20;
}

/**
 * Register (RSL) rpm#_cmr_mem_int_ena_w1s
 *
 * RPM CMR Memory Interrupt Enable Set Register This register sets
 * interrupt enable bits.
 */
union rpmx_cmr_mem_int_ena_w1s {
	u64 u;
	struct rpmx_cmr_mem_int_ena_w1s_s {
		u64 infifo_0_overfl                  : 1;
		u64 infifo_1_overfl                  : 1;
		u64 infifo_2_overfl                  : 1;
		u64 infifo_3_overfl                  : 1;
		u64 reserved_4_63                    : 60;
	} s;
	/* struct rpmx_cmr_mem_int_ena_w1s_s cn; */
};

static inline u64 RPMX_CMR_MEM_INT_ENA_W1S(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_MEM_INT_ENA_W1S(void)
{
	return 0x28;
}

/**
 * Register (RSL) rpm#_cmr_mem_int_w1s
 *
 * RPM CMR Memory Interrupt Set Register This register sets interrupt
 * bits.
 */
union rpmx_cmr_mem_int_w1s {
	u64 u;
	struct rpmx_cmr_mem_int_w1s_s {
		u64 infifo_0_overfl                  : 1;
		u64 infifo_1_overfl                  : 1;
		u64 infifo_2_overfl                  : 1;
		u64 infifo_3_overfl                  : 1;
		u64 reserved_4_63                    : 60;
	} s;
	/* struct rpmx_cmr_mem_int_w1s_s cn; */
};

static inline u64 RPMX_CMR_MEM_INT_W1S(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_MEM_INT_W1S(void)
{
	return 0x18;
}

/**
 * Register (RSL) rpm#_cmr_nic_nxc_adr
 *
 * RPM CMR NIC NXC Exception Registers
 */
union rpmx_cmr_nic_nxc_adr {
	u64 u;
	struct rpmx_cmr_nic_nxc_adr_s {
		u64 channel                          : 12;
		u64 lmac_id                          : 4;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_cmr_nic_nxc_adr_s cn; */
};

static inline u64 RPMX_CMR_NIC_NXC_ADR(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_NIC_NXC_ADR(void)
{
	return 0x1030;
}

/**
 * Register (RSL) rpm#_cmr_nix0_nxc_adr
 *
 * RPM CMR NIX0 NXC Exception Registers
 */
union rpmx_cmr_nix0_nxc_adr {
	u64 u;
	struct rpmx_cmr_nix0_nxc_adr_s {
		u64 channel                          : 12;
		u64 lmac_id                          : 4;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_cmr_nix0_nxc_adr_s cn; */
};

static inline u64 RPMX_CMR_NIX0_NXC_ADR(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_NIX0_NXC_ADR(void)
{
	return 0x1038;
}

/**
 * Register (RSL) rpm#_cmr_nix1_nxc_adr
 *
 * RPM CMR NIX1 NXC Exception Registers
 */
union rpmx_cmr_nix1_nxc_adr {
	u64 u;
	struct rpmx_cmr_nix1_nxc_adr_s {
		u64 channel                          : 12;
		u64 lmac_id                          : 4;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_cmr_nix1_nxc_adr_s cn; */
};

static inline u64 RPMX_CMR_NIX1_NXC_ADR(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_NIX1_NXC_ADR(void)
{
	return 0x1040;
}

/**
 * Register (RSL) rpm#_cmr_p2x#_count
 *
 * RPM P2X Activity Register
 */
union rpmx_cmr_p2xx_count {
	u64 u;
	struct rpmx_cmr_p2xx_count_s {
		u64 p2x_cnt                          : 64;
	} s;
	/* struct rpmx_cmr_p2xx_count_s cn; */
};

static inline u64 RPMX_CMR_P2XX_COUNT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_P2XX_COUNT(u64 a)
{
	return 0x168 + 0x1000 * a;
}

/**
 * Register (RSL) rpm#_cmr_rx_dmac#_cam0
 *
 * RPM CMR Receive CAM Registers These registers provide access to the 32
 * DMAC CAM0 entries in RPM, for use by X2P/NIX bound traffic.
 */
union rpmx_cmr_rx_dmacx_cam0 {
	u64 u;
	struct rpmx_cmr_rx_dmacx_cam0_s {
		u64 adr                              : 48;
		u64 en                               : 1;
		u64 id                               : 2;
		u64 reserved_51_63                   : 13;
	} s;
	/* struct rpmx_cmr_rx_dmacx_cam0_s cn; */
};

static inline u64 RPMX_CMR_RX_DMACX_CAM0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_RX_DMACX_CAM0(u64 a)
{
	return 0x4200 + 8 * a;
}

/**
 * Register (RSL) rpm#_cmr_rx_lmacs
 *
 * RPM CMR Receive Logical MACs Registers
 */
union rpmx_cmr_rx_lmacs {
	u64 u;
	struct rpmx_cmr_rx_lmacs_s {
		u64 lmacs                            : 3;
		u64 reserved_3_63                    : 61;
	} s;
	/* struct rpmx_cmr_rx_lmacs_s cn; */
};

static inline u64 RPMX_CMR_RX_LMACS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_RX_LMACS(void)
{
	return 0x128;
}

/**
 * Register (RSL) rpm#_cmr_rx_ovr_bp
 *
 * RPM CMR Receive-Ports Backpressure Override Registers Per-LMAC
 * backpressure override register.
 */
union rpmx_cmr_rx_ovr_bp {
	u64 u;
	struct rpmx_cmr_rx_ovr_bp_s {
		u64 ign_fifo_bp                      : 4;
		u64 bp                               : 4;
		u64 en                               : 4;
		u64 reserved_12_63                   : 52;
	} s;
	/* struct rpmx_cmr_rx_ovr_bp_s cn; */
};

static inline u64 RPMX_CMR_RX_OVR_BP(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_RX_OVR_BP(void)
{
	return 0x4130;
}

/**
 * Register (RSL) rpm#_cmr_rx_steering0#
 *
 * RPM CMR Receive Steering0 Registers These registers, along with
 * RPM()_CMR_RX_STEERING_VETYPE0(), provide eight filters for identifying
 * and steering receive traffic to X2P/NIX. Received packets are only
 * passed to X2P/NIX when the DMAC0 filter result is ACCEPT and STEERING0
 * filter result is PASS. See also RPM()_CMR()_RX_DMAC_CTL0.  Internal:
 * "* ALGORITHM \<pre\> rx_steering(uint48 pkt_dmac, uint16 pkt_etype,
 * uint16 pkt_vlan_id) {    for (int i = 0; i \< 8; i++) {       steer =
 * RPM()_CMR_RX_STEERING0(i);       vetype =
 * RPM()_CMR_RX_STEERING_VETYPE0(i);       if (steer[MCST_EN] ||
 * steer[DMAC_EN] || vetype[VLAN_EN] || vetype[VLAN_TAG_EN]) {
 * // Filter is enabled.          if (   (!steer[MCST_EN] ||
 * is_mcst(pkt_dmac))              && (!steer[DMAC_EN] || pkt_dmac ==
 * steer[DMAC])              && (!vetype[VLAN_EN] || pkt_vlan_id ==
 * vetype[VLAN_ID])              && (!vetype[VLAN_TAG_EN] || pkt_etype ==
 * vetype[VLAN_ETYPE]) )          {             // Filter match (all
 * enabled matching criteria are met).             return steer[PASS];
 * }       }    }    return RPM()_CMR_RX_STEERING_DEFAULT0[PASS]; // No
 * match } \</pre\>"
 */
union rpmx_cmr_rx_steering0x {
	u64 u;
	struct rpmx_cmr_rx_steering0x_s {
		u64 dmac                             : 48;
		u64 dmac_en                          : 1;
		u64 mcst_en                          : 1;
		u64 pass                             : 1;
		u64 reserved_51_63                   : 13;
	} s;
	/* struct rpmx_cmr_rx_steering0x_s cn; */
};

static inline u64 RPMX_CMR_RX_STEERING0X(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_RX_STEERING0X(u64 a)
{
	return 0x4300 + 8 * a;
}

/**
 * Register (RSL) rpm#_cmr_rx_steering_default0
 *
 * RPM CMR Receive Steering Default0 Destination Register For determining
 * destination of traffic that does not meet matching algorithm described
 * in registers RPM()_CMR_RX_STEERING0() and
 * RPM()_CMR_RX_STEERING_VETYPE0(). All 16B packets or smaller (20B in
 * case of FCS strip) as the result of truncation will steer to default
 * destination
 */
union rpmx_cmr_rx_steering_default0 {
	u64 u;
	struct rpmx_cmr_rx_steering_default0_s {
		u64 pass                             : 1;
		u64 reserved_1_63                    : 63;
	} s;
	/* struct rpmx_cmr_rx_steering_default0_s cn; */
};

static inline u64 RPMX_CMR_RX_STEERING_DEFAULT0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_RX_STEERING_DEFAULT0(void)
{
	return 0x43f0;
}

/**
 * Register (RSL) rpm#_cmr_rx_steering_vetype0#
 *
 * RPM CMR Receive VLAN Ethertype1 Register These registers, along with
 * RPM()_CMR_RX_STEERING0(), provide eight filters for identifying and
 * steering X2P/NIX receive traffic.
 */
union rpmx_cmr_rx_steering_vetype0x {
	u64 u;
	struct rpmx_cmr_rx_steering_vetype0x_s {
		u64 vlan_etype                       : 16;
		u64 vlan_tag_en                      : 1;
		u64 vlan_id                          : 12;
		u64 vlan_en                          : 1;
		u64 reserved_30_63                   : 34;
	} s;
	/* struct rpmx_cmr_rx_steering_vetype0x_s cn; */
};

static inline u64 RPMX_CMR_RX_STEERING_VETYPE0X(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_RX_STEERING_VETYPE0X(u64 a)
{
	return 0x4380 + 8 * a;
}

/**
 * Register (RSL) rpm#_cmr_tx_lmacs
 *
 * RPM CMR Transmit Logical MACs Registers This register sets the number
 * of LMACs allowed on the TX interface. The value is important for
 * defining the partitioning of the transmit FIFO.
 */
union rpmx_cmr_tx_lmacs {
	u64 u;
	struct rpmx_cmr_tx_lmacs_s {
		u64 lmacs                            : 3;
		u64 reserved_3_63                    : 61;
	} s;
	/* struct rpmx_cmr_tx_lmacs_s cn; */
};

static inline u64 RPMX_CMR_TX_LMACS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_TX_LMACS(void)
{
	return 0x1000;
}

/**
 * Register (RSL) rpm#_cmr_x2p#_count
 *
 * RPM X2P Activity Register
 */
union rpmx_cmr_x2px_count {
	u64 u;
	struct rpmx_cmr_x2px_count_s {
		u64 x2p_cnt                          : 64;
	} s;
	/* struct rpmx_cmr_x2px_count_s cn; */
};

static inline u64 RPMX_CMR_X2PX_COUNT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CMR_X2PX_COUNT(u64 a)
{
	return 0x170 + 0x1000 * a;
}

/**
 * Register (RSL) rpm#_const
 *
 * RPM CONST Registers This register contains constants for software
 * discovery.
 */
union rpmx_const {
	u64 u;
	struct rpmx_const_s {
		u64 tx_fifosz                        : 24;
		u64 lmacs                            : 8;
		u64 rx_fifosz                        : 24;
		u64 ver                              : 8;
	} s;
	/* struct rpmx_const_s cn; */
};

static inline u64 RPMX_CONST(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CONST(void)
{
	return 0x2000;
}

/**
 * Register (RSL) rpm#_const1
 *
 * RPM CONST1 Registers This register contains constants for software
 * discovery.
 */
union rpmx_const1 {
	u64 u;
	struct rpmx_const1_s {
		u64 types                            : 11;
		u64 res_types                        : 21;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_const1_s cn; */
};

static inline u64 RPMX_CONST1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_CONST1(void)
{
	return 0x2008;
}

/**
 * Register (RSL) rpm#_ext_mti_global_amps_lock_status
 *
 * RPM Ext MTI Global AMPS Lock Status Register Amps lock status for RS-
 * FEC.
 */
union rpmx_ext_mti_global_amps_lock_status {
	u64 u;
	struct rpmx_ext_mti_global_amps_lock_status_s {
		u64 amps_lock                        : 4;
		u64 reserved_4_63                    : 60;
	} s;
	/* struct rpmx_ext_mti_global_amps_lock_status_s cn; */
};

static inline u64 RPMX_EXT_MTI_GLOBAL_AMPS_LOCK_STATUS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_GLOBAL_AMPS_LOCK_STATUS(void)
{
	return 0x50038;
}

/**
 * Register (RSL) rpm#_ext_mti_global_block_lock_status
 *
 * RPM Ext MTI Global Block Lock Status Register GearBox block lock per
 * virtual lane. Irrelevant when RS-FEC is enabled.
 */
union rpmx_ext_mti_global_block_lock_status {
	u64 u;
	struct rpmx_ext_mti_global_block_lock_status_s {
		u64 block_lock                       : 20;
		u64 reserved_20_63                   : 44;
	} s;
	/* struct rpmx_ext_mti_global_block_lock_status_s cn; */
};

static inline u64 RPMX_EXT_MTI_GLOBAL_BLOCK_LOCK_STATUS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_GLOBAL_BLOCK_LOCK_STATUS(void)
{
	return 0x50050;
}

/**
 * Register (RSL) rpm#_ext_mti_global_channel_control
 *
 * RPM Ext MTI Global Channel Control Register Controls port modes:
 * 10/25/40/50/100G Full definition of the port mode is combined with PMA
 * Width (define number of lanes), and values of the MTI registers for
 * modes 10M, 100M, 1000M, QSGMII, USGMII, USXGMII-S. Distinguishing
 * between 1G to 2.5G or 5G to 10G is done by serial line rate only.
 */
union rpmx_ext_mti_global_channel_control {
	u64 u;
	struct rpmx_ext_mti_global_channel_control_s {
		u64 gc_mode40_ena_in0                : 1;
		u64 reserved_1                       : 1;
		u64 gc_pcs100_ena_in0                : 1;
		u64 gc_pcs100_ena_in2                : 1;
		u64 reserved_4_7                     : 4;
		u64 gc_mlg_ena_in_0                  : 1;
		u64 reserved_9                       : 1;
		u64 gc_fec91_1lane_in0               : 1;
		u64 gc_fec91_1lane_in2               : 1;
		u64 reserved_12_13                   : 2;
		u64 gc_rxlaui_ena_in0                : 1;
		u64 gc_rxlaui_ena_in2                : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_ext_mti_global_channel_control_s cn; */
};

static inline u64 RPMX_EXT_MTI_GLOBAL_CHANNEL_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_GLOBAL_CHANNEL_CONTROL(void)
{
	return 0x50010;
}

/**
 * Register (RSL) rpm#_ext_mti_global_clock_control
 *
 * RPM Ext MTI Global Clock Control Register Special clock control. -
 * Mandatory setting for USXGMII - Lower jitter or low latency for 10G
 */
union rpmx_ext_mti_global_clock_control {
	u64 u;
	struct rpmx_ext_mti_global_clock_control_s {
		u64 gc_fast_1lane_mode               : 8;
		u64 gc_pacer_10g                     : 8;
		u64 gc_cfg_clock_rate                : 4;
		u64 reserved_20_63                   : 44;
	} s;
	/* struct rpmx_ext_mti_global_clock_control_s cn; */
};

static inline u64 RPMX_EXT_MTI_GLOBAL_CLOCK_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_GLOBAL_CLOCK_CONTROL(void)
{
	return 0x50018;
}

/**
 * Register (RSL) rpm#_ext_mti_global_clock_enable
 *
 * RPM Ext MTI Global Clock Enable Register Clock gating control for the
 * different domains.
 */
union rpmx_ext_mti_global_clock_enable {
	u64 u;
	struct rpmx_ext_mti_global_clock_enable_s {
		u64 mac_clken_ovrd                   : 4;
		u64 reserved_4_7                     : 4;
		u64 mac_pcs_cmn_clken_ovrd           : 1;
		u64 fec91_clken_ovrd                 : 1;
		u64 pcs_clken_ovrd                   : 1;
		u64 reserved_11_12                   : 2;
		u64 lpcs_clken_ovrd                  : 1;
		u64 reg_clken_ovrd                   : 1;
		u64 reserved_15_63                   : 49;
	} s;
	struct rpmx_ext_mti_global_clock_enable_cn {
		u64 mac_clken_ovrd                   : 4;
		u64 reserved_4_7                     : 4;
		u64 mac_pcs_cmn_clken_ovrd           : 1;
		u64 fec91_clken_ovrd                 : 1;
		u64 pcs_clken_ovrd                   : 1;
		u64 reserved_11                      : 1;
		u64 reserved_12                      : 1;
		u64 lpcs_clken_ovrd                  : 1;
		u64 reg_clken_ovrd                   : 1;
		u64 reserved_15_63                   : 49;
	} cn;
};

static inline u64 RPMX_EXT_MTI_GLOBAL_CLOCK_ENABLE(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_GLOBAL_CLOCK_ENABLE(void)
{
	return 0x50020;
}

/**
 * Register (RSL) rpm#_ext_mti_global_fec_control
 *
 * RPM Ext MTI Global FEC Control Register Control FEC Capabilities - No
 * FEC, Firecode FEC, RS-FEC (528,514), RS-FEC (544,514) In addition, in
 * case of Firecode FEC, it can control error signaling to PCS.
 */
union rpmx_ext_mti_global_fec_control {
	u64 u;
	struct rpmx_ext_mti_global_fec_control_s {
		u64 gc_fec_ena                       : 4;
		u64 reserved_4_7                     : 4;
		u64 gc_fec_err_ena                   : 4;
		u64 reserved_12_15                   : 4;
		u64 gc_fec91_ena_in                  : 4;
		u64 reserved_20_23                   : 4;
		u64 gc_kp_mode_in                    : 4;
		u64 reserved_28_63                   : 36;
	} s;
	/* struct rpmx_ext_mti_global_fec_control_s cn; */
};

static inline u64 RPMX_EXT_MTI_GLOBAL_FEC_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_GLOBAL_FEC_CONTROL(void)
{
	return 0x50008;
}

/**
 * Register (RSL) rpm#_ext_mti_global_fec_error_status
 *
 * RPM Ext MTI Global FEC Error Status Register Firecode FEC errors
 * status.
 */
union rpmx_ext_mti_global_fec_error_status {
	u64 u;
	struct rpmx_ext_mti_global_fec_error_status_s {
		u64 fec_cerr                         : 8;
		u64 reserved_8_15                    : 8;
		u64 fec_ncerr                        : 8;
		u64 reserved_24_63                   : 40;
	} s;
	/* struct rpmx_ext_mti_global_fec_error_status_s cn; */
};

static inline u64 RPMX_EXT_MTI_GLOBAL_FEC_ERROR_STATUS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_GLOBAL_FEC_ERROR_STATUS(void)
{
	return 0x50048;
}

/**
 * Register (RSL) rpm#_ext_mti_global_fec_status
 *
 * RPM Ext MTI Global FEC Status Register Firecode FEC lock status.
 */
union rpmx_ext_mti_global_fec_status {
	u64 u;
	struct rpmx_ext_mti_global_fec_status_s {
		u64 fec_locked                       : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_ext_mti_global_fec_status_s cn; */
};

static inline u64 RPMX_EXT_MTI_GLOBAL_FEC_STATUS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_GLOBAL_FEC_STATUS(void)
{
	return 0x50040;
}

/**
 * Register (RSL) rpm#_ext_mti_global_pma_control
 *
 * RPM Ext MTI Global PMA Control Register This register controls the PMA
 * Width (40/80)
 */
union rpmx_ext_mti_global_pma_control {
	u64 u;
	struct rpmx_ext_mti_global_pma_control_s {
		u64 gc_sd_n2                         : 4;
		u64 reserved_4_7                     : 4;
		u64 gc_sd_8x                         : 4;
		u64 reserved_12_63                   : 52;
	} s;
	/* struct rpmx_ext_mti_global_pma_control_s cn; */
};

static inline u64 RPMX_EXT_MTI_GLOBAL_PMA_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_GLOBAL_PMA_CONTROL(void)
{
	return 0x50000;
}

/**
 * Register (RSL) rpm#_ext_mti_global_reset_control
 *
 * RPM Ext MTI Global Reset Control Register Reset control for the
 * different domains.
 */
union rpmx_ext_mti_global_reset_control {
	u64 u;
	struct rpmx_ext_mti_global_reset_control_s {
		u64 mac_reset                        : 4;
		u64 reserved_4_7                     : 4;
		u64 mac_pcs_cmn_reset                : 1;
		u64 fec91_reset                      : 1;
		u64 pcs_reset                        : 1;
		u64 reserved_11_12                   : 2;
		u64 lpcs_reset                       : 1;
		u64 reg_reset                        : 1;
		u64 reserved_15                      : 1;
		u64 serdes_rx_reset                  : 4;
		u64 reserved_20_23                   : 4;
		u64 serdes_tx_reset                  : 4;
		u64 reserved_28_63                   : 36;
	} s;
	struct rpmx_ext_mti_global_reset_control_cn {
		u64 mac_reset                        : 4;
		u64 reserved_4_7                     : 4;
		u64 mac_pcs_cmn_reset                : 1;
		u64 fec91_reset                      : 1;
		u64 pcs_reset                        : 1;
		u64 reserved_11                      : 1;
		u64 reserved_12                      : 1;
		u64 lpcs_reset                       : 1;
		u64 reg_reset                        : 1;
		u64 reserved_15                      : 1;
		u64 serdes_rx_reset                  : 4;
		u64 reserved_20_23                   : 4;
		u64 serdes_tx_reset                  : 4;
		u64 reserved_28_31                   : 4;
		u64 reserved_32_63                   : 32;
	} cn;
};

static inline u64 RPMX_EXT_MTI_GLOBAL_RESET_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_GLOBAL_RESET_CONTROL(void)
{
	return 0x50028;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_control
 *
 * RPM Ext MTI Port Control Register General port configurations.
 */
union rpmx_ext_mti_portx_control {
	u64 u;
	struct rpmx_ext_mti_portx_control_s {
		u64 tx_loc_fault                     : 1;
		u64 tx_rem_fault                     : 1;
		u64 tx_li_fault                      : 1;
		u64 tod_select                       : 1;
		u64 rx_pause_control                 : 1;
		u64 rx_pause_ow_val                  : 1;
		u64 pause_802_3_reflect              : 1;
		u64 loop_ena                         : 1;
		u64 mask_sw_reset                    : 1;
		u64 led_port_num                     : 6;
		u64 led_port_en                      : 1;
		u64 ff_tx_crc                        : 1;
		u64 force_link_ok_en                 : 1;
		u64 force_link_ok_dis                : 1;
		u64 port_res_speed                   : 4;
		u64 port_res_speed_from_hw           : 1;
		u64 loop_rx_block_out                : 1;
		u64 loop_tx_rdy_out                  : 1;
		u64 reserved_26_63                   : 38;
	} s;
	/* struct rpmx_ext_mti_portx_control_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_CONTROL(u64 a)
{
	return 0x51000 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_frc_delta
 *
 * INTERNAL: RPM Ext MTI Port FRC Delta Register  FRC Delta Internal:
 * Used for 1-step PTP only, which RPM does not support.
 */
union rpmx_ext_mti_portx_frc_delta {
	u64 u;
	struct rpmx_ext_mti_portx_frc_delta_s {
		u64 frc_delta                        : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_ext_mti_portx_frc_delta_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_FRC_DELTA(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_FRC_DELTA(u64 a)
{
	return 0x510a8 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_interrupt_cause
 *
 * RPM Ext MTI Port Interrupt Cause Register Port Interrupts, such as
 * link/lock change, faults presence, FIFO over/underrun, etc.
 */
union rpmx_ext_mti_portx_interrupt_cause {
	u64 u;
	struct rpmx_ext_mti_portx_interrupt_cause_s {
		u64 port0_interrupt_cause_int_sum    : 1;
		u64 link_ok_change                   : 1;
		u64 link_status_change               : 1;
		u64 lpcs_link_status_change          : 1;
		u64 mac_loc_fault                    : 1;
		u64 mac_rem_fault                    : 1;
		u64 mac_li_fault                     : 1;
		u64 mac_tx_underflow                 : 1;
		u64 mac_tx_ovr_err                   : 1;
		u64 rx_overrun                       : 1;
		u64 hi_ber                           : 1;
		u64 ff_rx_rdy                        : 1;
		u64 tsu_rx_dff_err                   : 1;
		u64 tsu_rx_am_err                    : 1;
		u64 tsu_tx_sync_err                  : 1;
		u64 tsd_non_accurate_ptp             : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_ext_mti_portx_interrupt_cause_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_INTERRUPT_CAUSE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_INTERRUPT_CAUSE(u64 a)
{
	return 0x51028 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_interrupt_mask
 *
 * RPM Ext MTI Port Interrupt Mask Register Per port interrupt mask.
 */
union rpmx_ext_mti_portx_interrupt_mask {
	u64 u;
	struct rpmx_ext_mti_portx_interrupt_mask_s {
		u64 reserved_0                       : 1;
		u64 interrupt_mask                   : 15;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_ext_mti_portx_interrupt_mask_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_INTERRUPT_MASK(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_INTERRUPT_MASK(u64 a)
{
	return 0x51030 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_marker_status
 *
 * RPM Ext MTI Port Marker Status Register Marker insertion counters.
 * Counts till reaching the marker interval value and then performs
 * wraparound.
 */
union rpmx_ext_mti_portx_marker_status {
	u64 u;
	struct rpmx_ext_mti_portx_marker_status_s {
		u64 marker_ins_cnt                   : 16;
		u64 marker_ins_cnt_100               : 15;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_ext_mti_portx_marker_status_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_MARKER_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_MARKER_STATUS(u64 a)
{
	return 0x51020 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_pause_and_err_stat
 *
 * RPM Ext Port 0 Pause And Err Stat Register Port Status of RX flow
 * control per priority (in case of 802.3x, only bit[0] is meaningful);
 * Port status of the last received packet
 */
union rpmx_ext_mti_portx_pause_and_err_stat {
	u64 u;
	struct rpmx_ext_mti_portx_pause_and_err_stat_s {
		u64 pause_on                         : 16;
		u64 ff_rx_err_stat                   : 8;
		u64 reserved_24_63                   : 40;
	} s;
	/* struct rpmx_ext_mti_portx_pause_and_err_stat_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_PAUSE_AND_ERR_STAT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_PAUSE_AND_ERR_STAT(u64 a)
{
	return 0x51018 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_pause_override
 *
 * RPM Ext MTI Port Pause Override Register RX pause override. Can
 * override RX pause indications towards FCU RX (Non-segmented ports).
 */
union rpmx_ext_mti_portx_pause_override {
	u64 u;
	struct rpmx_ext_mti_portx_pause_override_s {
		u64 pause_override_ctrl              : 16;
		u64 pause_override_val               : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_ext_mti_portx_pause_override_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_PAUSE_OVERRIDE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_PAUSE_OVERRIDE(u64 a)
{
	return 0x51048 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_peer_delay
 *
 * RPM Ext MTI Port Peer Delay Register Set peer delay value for Time
 * Stamping purposes. Internal: Register relevant for 1-step PTP only.
 */
union rpmx_ext_mti_portx_peer_delay {
	u64 u;
	struct rpmx_ext_mti_portx_peer_delay_s {
		u64 peer_delay                       : 30;
		u64 reserved_30                      : 1;
		u64 peer_delay_valid                 : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_ext_mti_portx_peer_delay_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_PEER_DELAY(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_PEER_DELAY(u64 a)
{
	return 0x51038 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_status
 *
 * RPM Ext MTI Port Status Register Port status, such as link/lock
 * change, traffic, etc.
 */
union rpmx_ext_mti_portx_status {
	u64 u;
	struct rpmx_ext_mti_portx_status_s {
		u64 link_ok                          : 1;
		u64 link_status                      : 1;
		u64 lpcs_link_status                 : 1;
		u64 lpcs_rx_sync                     : 1;
		u64 lpcs_an_done                     : 1;
		u64 mac_res_speed                    : 8;
		u64 align_done                       : 1;
		u64 ber_timer_done                   : 1;
		u64 hi_ber                           : 1;
		u64 rsfec_aligned                    : 1;
		u64 rx_traffic_ind                   : 1;
		u64 tx_traffic_ind                   : 1;
		u64 mac_tx_empty                     : 1;
		u64 mac_tx_isidle                    : 1;
		u64 mac_tx_ts_frm_out                : 1;
		u64 pfc_mode                         : 1;
		u64 ff_tx_septy                      : 1;
		u64 ff_rx_empty                      : 1;
		u64 ff_rx_dsav                       : 1;
		u64 link_ok_clean                    : 1;
		u64 reserved_27_63                   : 37;
	} s;
	/* struct rpmx_ext_mti_portx_status_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_STATUS(u64 a)
{
	return 0x51008 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_status_2
 *
 * RPM Ext MTI Port Status 2 Register Port status2
 */
union rpmx_ext_mti_portx_status_2 {
	u64 u;
	struct rpmx_ext_mti_portx_status_2_s {
		u64 sd_bit_slip                      : 6;
		u64 reserved_6_63                    : 58;
	} s;
	/* struct rpmx_ext_mti_portx_status_2_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_STATUS_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_STATUS_2(u64 a)
{
	return 0x51010 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_time_stamp_dispatcher_control_0
 *
 * INTERNAL: RPM Ext MTI Port Time Stamp Dispatcher Control 0 Register
 * Time Stamp Dispatcher Control 0 Internal: Not used. Switches team use
 * this register for HW sending of PTP.
 */
union rpmx_ext_mti_portx_time_stamp_dispatcher_control_0 {
	u64 u;
	struct rpmx_ext_mti_portx_time_stamp_dispatcher_control_0_s {
		u64 fsu_enable                       : 1;
		u64 amd_enable                       : 1;
		u64 fsu_offset                       : 10;
		u64 fsu_rnd_delta                    : 8;
		u64 reserved_20_23                   : 4;
		u64 amd_cnt_type_sel                 : 1;
		u64 minimal_tx_stop_toggle           : 5;
		u64 reserved_30_63                   : 34;
	} s;
	/* struct rpmx_ext_mti_portx_time_stamp_dispatcher_control_0_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_TIME_STAMP_DISPATCHER_CONTROL_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_TIME_STAMP_DISPATCHER_CONTROL_0(u64 a)
{
	return 0x51058 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_time_stamp_dispatcher_control_1
 *
 * INTERNAL: RPM Ext MTI Port Time Stamp Dispatcher Control 1 Register
 * Time Stamp Dispatcher Control 1 Internal: Not used. Switches team use
 * this register for HW sending of PTP.
 */
union rpmx_ext_mti_portx_time_stamp_dispatcher_control_1 {
	u64 u;
	struct rpmx_ext_mti_portx_time_stamp_dispatcher_control_1_s {
		u64 amd_cnt_low                      : 16;
		u64 amd_cnt_high                     : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_ext_mti_portx_time_stamp_dispatcher_control_1_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_TIME_STAMP_DISPATCHER_CONTROL_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_TIME_STAMP_DISPATCHER_CONTROL_1(u64 a)
{
	return 0x51060 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_time_stamp_dispatcher_control_2
 *
 * INTERNAL: RPM Ext MTI Port 0 Time Stamp Dispatcher Control 2 Register
 * Internal: Not used. Switches team use this register for HW sending of
 * PTP.
 */
union rpmx_ext_mti_portx_time_stamp_dispatcher_control_2 {
	u64 u;
	struct rpmx_ext_mti_portx_time_stamp_dispatcher_control_2_s {
		u64 minimal_empty_for_stop_tx        : 6;
		u64 reserved_6_63                    : 58;
	} s;
	/* struct rpmx_ext_mti_portx_time_stamp_dispatcher_control_2_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_TIME_STAMP_DISPATCHER_CONTROL_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_TIME_STAMP_DISPATCHER_CONTROL_2(u64 a)
{
	return 0x51068 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_tsu_control_0
 *
 * RPM Ext MTI Port Tsu Control 0 Register TSU Control 0
 */
union rpmx_ext_mti_portx_tsu_control_0 {
	u64 u;
	struct rpmx_ext_mti_portx_tsu_control_0_s {
		u64 tsu_rx_mode                      : 3;
		u64 tsu_tx_mode                      : 3;
		u64 tsu_deskew                       : 10;
		u64 tsu_mii_mk_dly                   : 3;
		u64 tsu_mii_cw_dly                   : 3;
		u64 tsu_mii_tx_mk_cyc_dly            : 5;
		u64 tsu_mii_tx_cw_cyc_dly            : 5;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_ext_mti_portx_tsu_control_0_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_TSU_CONTROL_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_TSU_CONTROL_0(u64 a)
{
	return 0x51080 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_tsu_control_1
 *
 * RPM Ext MTI Port Tsu Control 1 Register TSU Control 2
 */
union rpmx_ext_mti_portx_tsu_control_1 {
	u64 u;
	struct rpmx_ext_mti_portx_tsu_control_1_s {
		u64 tsu_blocktime                    : 5;
		u64 tsu_blocktime_dec                : 9;
		u64 reserved_14_15                   : 2;
		u64 tsu_markertime                   : 5;
		u64 tsu_markertime_dec               : 9;
		u64 reserved_30                      : 1;
		u64 tsu_blks_per_clk                 : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_ext_mti_portx_tsu_control_1_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_TSU_CONTROL_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_TSU_CONTROL_1(u64 a)
{
	return 0x51088 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_tsu_control_2
 *
 * INTERNAL: RPM Ext MTI Port Tsu Control 2 Register  TSU Control 2
 * Internal: Not used. Switches team use this register for SW override of
 * HW-controlled TSU settings.
 */
union rpmx_ext_mti_portx_tsu_control_2 {
	u64 u;
	struct rpmx_ext_mti_portx_tsu_control_2_s {
		u64 c_rx_mode_ow                     : 1;
		u64 c_tx_mode_ow                     : 1;
		u64 c_blks_per_clk_ow                : 1;
		u64 c_mii_cw_dly_ow                  : 1;
		u64 c_mii_mk_dly_ow                  : 1;
		u64 c_deskew_ow                      : 1;
		u64 c_modulo_rx_ow                   : 1;
		u64 c_modulo_tx_ow                   : 1;
		u64 c_blocktime_int_ow               : 1;
		u64 c_blocktime_dec_ow               : 1;
		u64 c_markertime_int_ow              : 1;
		u64 c_markertime_dec_ow              : 1;
		u64 c_mii_tx_mk_cyc_dly_ow           : 1;
		u64 c_mii_tx_cw_cyc_dly_ow           : 1;
		u64 c_tsu_tx_sd_period_ow            : 1;
		u64 reserved_15_63                   : 49;
	} s;
	/* struct rpmx_ext_mti_portx_tsu_control_2_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_TSU_CONTROL_2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_TSU_CONTROL_2(u64 a)
{
	return 0x51090 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_tsu_control_3
 *
 * RPM Ext MTI Port Tsu Control 3 Register TSU Control 3
 */
union rpmx_ext_mti_portx_tsu_control_3 {
	u64 u;
	struct rpmx_ext_mti_portx_tsu_control_3_s {
		u64 tsu_tx_sd_period                 : 15;
		u64 reserved_15_63                   : 49;
	} s;
	/* struct rpmx_ext_mti_portx_tsu_control_3_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_TSU_CONTROL_3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_TSU_CONTROL_3(u64 a)
{
	return 0x51098 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_tsu_modulo_rx
 *
 * RPM Ext MTI Port Tsu Modulo Rx Register TSU Control 1
 */
union rpmx_ext_mti_portx_tsu_modulo_rx {
	u64 u;
	struct rpmx_ext_mti_portx_tsu_modulo_rx_s {
		u64 tsu_modulo_rx                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_ext_mti_portx_tsu_modulo_rx_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_TSU_MODULO_RX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_TSU_MODULO_RX(u64 a)
{
	return 0x51078 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_tsu_modulo_tx
 *
 * RPM Ext MTI Port Tsu Modulo Tx Register TSU Control 1
 */
union rpmx_ext_mti_portx_tsu_modulo_tx {
	u64 u;
	struct rpmx_ext_mti_portx_tsu_modulo_tx_s {
		u64 tsu_modulo_tx                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_ext_mti_portx_tsu_modulo_tx_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_TSU_MODULO_TX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_TSU_MODULO_TX(u64 a)
{
	return 0x51070 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_tsu_status
 *
 * RPM Ext MTI Port Tsu Status Register TSU Status
 */
union rpmx_ext_mti_portx_tsu_status {
	u64 u;
	struct rpmx_ext_mti_portx_tsu_status_s {
		u64 tsu_rx_ready                     : 1;
		u64 tsu_tx_ready                     : 1;
		u64 reserved_2_7                     : 6;
		u64 tsu_desk_rlevel                  : 7;
		u64 reserved_15_63                   : 49;
	} s;
	/* struct rpmx_ext_mti_portx_tsu_status_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_TSU_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_TSU_STATUS(u64 a)
{
	return 0x510a0 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_xoff_override
 *
 * RPM Ext MTI Port Xoff Override Register TX pause override. Can
 * override TX pause generation indications towards MAC (Non-segmented
 * ports).
 */
union rpmx_ext_mti_portx_xoff_override {
	u64 u;
	struct rpmx_ext_mti_portx_xoff_override_s {
		u64 xoff_override_ctrl               : 16;
		u64 xoff_override_val                : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_ext_mti_portx_xoff_override_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_XOFF_OVERRIDE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_XOFF_OVERRIDE(u64 a)
{
	return 0x51050 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_ext_mti_port#_xoff_status
 *
 * RPM Ext MTI Port Xoff Status Register Status of Pause frames triggers
 * towards the MAC. When pause mode is 802.3x, only bit[0] is meaningful
 * (Non-segmented ports).
 */
union rpmx_ext_mti_portx_xoff_status {
	u64 u;
	struct rpmx_ext_mti_portx_xoff_status_s {
		u64 xoff_status                      : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_ext_mti_portx_xoff_status_s cn; */
};

static inline u64 RPMX_EXT_MTI_PORTX_XOFF_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_EXT_MTI_PORTX_XOFF_STATUS(u64 a)
{
	return 0x51040 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_msix_pba#
 *
 * RPM MSI-X Pending Bit Array Registers This register is the MSI-X PBA
 * table, the bit number is indexed by the RPM_INT_VEC_E enumeration.
 */
union rpmx_msix_pbax {
	u64 u;
	struct rpmx_msix_pbax_s {
		u64 pend                             : 64;
	} s;
	/* struct rpmx_msix_pbax_s cn; */
};

static inline u64 RPMX_MSIX_PBAX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MSIX_PBAX(u64 a)
{
	return 0xf0000 + 8 * a;
}

/**
 * Register (RSL) rpm#_msix_vec#_addr
 *
 * RPM MSI-X Vector Table Address Registers This register is the MSI-X
 * vector table, indexed by the RPM_INT_VEC_E enumeration.
 */
union rpmx_msix_vecx_addr {
	u64 u;
	struct rpmx_msix_vecx_addr_s {
		u64 secvec                           : 1;
		u64 reserved_1                       : 1;
		u64 addr                             : 51;
		u64 reserved_53_63                   : 11;
	} s;
	/* struct rpmx_msix_vecx_addr_s cn; */
};

static inline u64 RPMX_MSIX_VECX_ADDR(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MSIX_VECX_ADDR(u64 a)
{
	return 0 + 0x10 * a;
}

/**
 * Register (RSL) rpm#_msix_vec#_ctl
 *
 * RPM MSI-X Vector Table Control and Data Registers This register is the
 * MSI-X vector table, indexed by the RPM_INT_VEC_E enumeration.
 */
union rpmx_msix_vecx_ctl {
	u64 u;
	struct rpmx_msix_vecx_ctl_s {
		u64 data                             : 32;
		u64 mask                             : 1;
		u64 reserved_33_63                   : 31;
	} s;
	/* struct rpmx_msix_vecx_ctl_s cn; */
};

static inline u64 RPMX_MSIX_VECX_CTL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MSIX_VECX_CTL(u64 a)
{
	return 8 + 0x10 * a;
}

/**
 * Register (RSL) rpm#_mti_fcfec#_cw_hi
 *
 * RPM MTI FC-FEC Codewords Count High Register
 */
union rpmx_mti_fcfecx_cw_hi {
	u64 u;
	struct rpmx_mti_fcfecx_cw_hi_s {
		u64 cw_hi                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_fcfecx_cw_hi_s cn; */
};

static inline u64 RPMX_MTI_FCFECX_CW_HI(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_FCFECX_CW_HI(u64 a)
{
	return 0x38638 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_fcfec#_fec_ability
 *
 * RPM MTI FC-FEC Ability Register
 */
union rpmx_mti_fcfecx_fec_ability {
	u64 u;
	struct rpmx_mti_fcfecx_fec_ability_s {
		u64 fec_ability                      : 1;
		u64 fec_error_indication_ability     : 1;
		u64 reserved_2_63                    : 62;
	} s;
	/* struct rpmx_mti_fcfecx_fec_ability_s cn; */
};

static inline u64 RPMX_MTI_FCFECX_FEC_ABILITY(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_FCFECX_FEC_ABILITY(u64 a)
{
	return 0x38600 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_fcfec#_fec_control
 *
 * RPM MTI FC-FEC Control Register
 */
union rpmx_mti_fcfecx_fec_control {
	u64 u;
	struct rpmx_mti_fcfecx_fec_control_s {
		u64 fec_enable                       : 1;
		u64 fec_enable_error_indication      : 1;
		u64 reserved_2_63                    : 62;
	} s;
	/* struct rpmx_mti_fcfecx_fec_control_s cn; */
};

static inline u64 RPMX_MTI_FCFECX_FEC_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_FCFECX_FEC_CONTROL(u64 a)
{
	return 0x38608 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_fcfec#_fec_status
 *
 * RPM MTI FC-FEC Status Register
 */
union rpmx_mti_fcfecx_fec_status {
	u64 u;
	struct rpmx_mti_fcfecx_fec_status_s {
		u64 fec_locked_vl0                   : 1;
		u64 fec_locked_vl1                   : 1;
		u64 reserved_2_63                    : 62;
	} s;
	/* struct rpmx_mti_fcfecx_fec_status_s cn; */
};

static inline u64 RPMX_MTI_FCFECX_FEC_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_FCFECX_FEC_STATUS(u64 a)
{
	return 0x38610 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_fcfec#_vl0_ccw_lo
 *
 * RPM MTI FC-FEC VL0 Corrected Codewords Count Low Register
 */
union rpmx_mti_fcfecx_vl0_ccw_lo {
	u64 u;
	struct rpmx_mti_fcfecx_vl0_ccw_lo_s {
		u64 ccw_lo                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_fcfecx_vl0_ccw_lo_s cn; */
};

static inline u64 RPMX_MTI_FCFECX_VL0_CCW_LO(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_FCFECX_VL0_CCW_LO(u64 a)
{
	return 0x38618 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_fcfec#_vl0_nccw_lo
 *
 * RPM MTI FC-FEC VL0 Uncorrected Codewords Count Low Register
 */
union rpmx_mti_fcfecx_vl0_nccw_lo {
	u64 u;
	struct rpmx_mti_fcfecx_vl0_nccw_lo_s {
		u64 nccw_lo                          : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_fcfecx_vl0_nccw_lo_s cn; */
};

static inline u64 RPMX_MTI_FCFECX_VL0_NCCW_LO(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_FCFECX_VL0_NCCW_LO(u64 a)
{
	return 0x38620 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_fcfec#_vl1_ccw_lo
 *
 * RPM MTI FC-FEC VL1 Corrected Codewords Count Low Register
 */
union rpmx_mti_fcfecx_vl1_ccw_lo {
	u64 u;
	struct rpmx_mti_fcfecx_vl1_ccw_lo_s {
		u64 ccw_lo                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_fcfecx_vl1_ccw_lo_s cn; */
};

static inline u64 RPMX_MTI_FCFECX_VL1_CCW_LO(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_FCFECX_VL1_CCW_LO(u64 a)
{
	return 0x38628 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_fcfec#_vl1_nccw_lo
 *
 * RPM MTI FC-FEC VL1 Uncorrected Codewords Count Low Register
 */
union rpmx_mti_fcfecx_vl1_nccw_lo {
	u64 u;
	struct rpmx_mti_fcfecx_vl1_nccw_lo_s {
		u64 nccw_lo                          : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_fcfecx_vl1_nccw_lo_s cn; */
};

static inline u64 RPMX_MTI_FCFECX_VL1_NCCW_LO(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_FCFECX_VL1_NCCW_LO(u64 a)
{
	return 0x38630 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_an_expansion
 *
 * RPM MTI LPCS Autonegotiation Expansion Register Autonegotiation
 * Expansion Register
 */
union rpmx_mti_lpcsx_an_expansion {
	u64 u;
	struct rpmx_mti_lpcsx_an_expansion_s {
		u64 page_received_realtime           : 1;
		u64 page_received                    : 1;
		u64 next_page_able                   : 1;
		u64 reserved_3_63                    : 61;
	} s;
	/* struct rpmx_mti_lpcsx_an_expansion_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_AN_EXPANSION(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_AN_EXPANSION(u64 a)
{
	return 0x30030 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_control
 *
 * RPM MTI LPCS Control Register Control register
 */
union rpmx_mti_lpcsx_control {
	u64 u;
	struct rpmx_mti_lpcsx_control_s {
		u64 reserved_0_5                     : 6;
		u64 speed_6                          : 1;
		u64 reserved_7                       : 1;
		u64 duplex                           : 1;
		u64 an_restart                       : 1;
		u64 isolate                          : 1;
		u64 powerdown                        : 1;
		u64 an_enable                        : 1;
		u64 speed_13                         : 1;
		u64 loopback                         : 1;
		u64 reset                            : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_control_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_CONTROL(u64 a)
{
	return 0x30000 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_decode_errors
 *
 * RPM MTI LPCS Decode Errors Register 10B decoder error counter for
 * test/debug; May not exist in all Core Variants;
 */
union rpmx_mti_lpcsx_decode_errors {
	u64 u;
	struct rpmx_mti_lpcsx_decode_errors_s {
		u64 decode_errors_counter            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_decode_errors_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_DECODE_ERRORS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_DECODE_ERRORS(u64 a)
{
	return 0x300a8 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_dev_ability
 *
 * RPM MTI LPCS Device Ability Register Local Device Abilities for
 * Autonegotiation. Contents differs for 1000Base-X or SGMII mode.
 */
union rpmx_mti_lpcsx_dev_ability {
	u64 u;
	struct rpmx_mti_lpcsx_dev_ability_s {
		u64 ability_rsv05                    : 5;
		u64 fd                               : 1;
		u64 hd                               : 1;
		u64 ps1                              : 1;
		u64 ps2                              : 1;
		u64 ability_rsv9                     : 3;
		u64 rf1                              : 1;
		u64 rf2                              : 1;
		u64 ack                              : 1;
		u64 np                               : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_dev_ability_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_DEV_ABILITY(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_DEV_ABILITY(u64 a)
{
	return 0x30020 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_if_mode
 *
 * RPM MTI LPCS Interface Mode Register SGMII Mode Control
 */
union rpmx_mti_lpcsx_if_mode {
	u64 u;
	struct rpmx_mti_lpcsx_if_mode_s {
		u64 sgmii_ena                        : 1;
		u64 use_sgmii_an                     : 1;
		u64 sgmii_speed                      : 2;
		u64 sgmii_duplex                     : 1;
		u64 rsv5                             : 1;
		u64 tx_preamble_sync                 : 1;
		u64 rx_preamble_sync                 : 1;
		u64 mode_xgmii_basex                 : 1;
		u64 seq_ena                          : 1;
		u64 reserved_10_63                   : 54;
	} s;
	/* struct rpmx_mti_lpcsx_if_mode_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_IF_MODE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_IF_MODE(u64 a)
{
	return 0x300a0 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_link_timer_hi
 *
 * RPM MTI LPCS Link Timer High Register Autonegotiation link timer upper
 * 5 bits
 */
union rpmx_mti_lpcsx_link_timer_hi {
	u64 u;
	struct rpmx_mti_lpcsx_link_timer_hi_s {
		u64 timer20_16                       : 5;
		u64 reserved_5_63                    : 59;
	} s;
	/* struct rpmx_mti_lpcsx_link_timer_hi_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_LINK_TIMER_HI(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_LINK_TIMER_HI(u64 a)
{
	return 0x30098 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_link_timer_lo
 *
 * RPM MTI LPCS Link Timer Low Register Autonegotiation link timer lower
 * 16 bits
 */
union rpmx_mti_lpcsx_link_timer_lo {
	u64 u;
	struct rpmx_mti_lpcsx_link_timer_lo_s {
		u64 timer0                           : 1;
		u64 timer15_1                        : 15;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_link_timer_lo_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_LINK_TIMER_LO(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_LINK_TIMER_LO(u64 a)
{
	return 0x30090 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_lp_np_rx
 *
 * RPM MTI LPCS Link Partner Next Page RX Register Received Next Page
 * data from link partner
 */
union rpmx_mti_lpcsx_lp_np_rx {
	u64 u;
	struct rpmx_mti_lpcsx_lp_np_rx_s {
		u64 data                             : 11;
		u64 toggle                           : 1;
		u64 ack2                             : 1;
		u64 mp                               : 1;
		u64 ack                              : 1;
		u64 np                               : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_lp_np_rx_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_LP_NP_RX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_LP_NP_RX(u64 a)
{
	return 0x30040 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_np_tx
 *
 * RPM MTI LPCS Next Page TX Register Next Page data to transmit
 */
union rpmx_mti_lpcsx_np_tx {
	u64 u;
	struct rpmx_mti_lpcsx_np_tx_s {
		u64 data                             : 11;
		u64 toggle                           : 1;
		u64 ack2                             : 1;
		u64 mp                               : 1;
		u64 ack                              : 1;
		u64 np                               : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_np_tx_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_NP_TX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_NP_TX(u64 a)
{
	return 0x30038 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_partner_ability
 *
 * RPM MTI LPCS Partner Ability Register Received Abilities during
 * Autonegotiation. Contents differ depending on 1000Base-X or SGMII
 * mode.
 */
union rpmx_mti_lpcsx_partner_ability {
	u64 u;
	struct rpmx_mti_lpcsx_partner_ability_s {
		u64 pability_rsv05                   : 5;
		u64 fd                               : 1;
		u64 hd                               : 1;
		u64 ps1                              : 1;
		u64 ps2                              : 1;
		u64 pability_rsv9                    : 1;
		u64 pability_rsv10                   : 2;
		u64 rf1                              : 1;
		u64 rf2                              : 1;
		u64 ack                              : 1;
		u64 np                               : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_partner_ability_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_PARTNER_ABILITY(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_PARTNER_ABILITY(u64 a)
{
	return 0x30028 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_phy_id_hi
 *
 * RPM MTI LPCS PHY Identifier High Register PHY Identifier upper 16 bits
 */
union rpmx_mti_lpcsx_phy_id_hi {
	u64 u;
	struct rpmx_mti_lpcsx_phy_id_hi_s {
		u64 phyid1                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_phy_id_hi_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_PHY_ID_HI(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_PHY_ID_HI(u64 a)
{
	return 0x30018 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_phy_id_lo
 *
 * RPM MTI LPCS PHY Identifier Low Register PHY Identifier lower 16 bits
 */
union rpmx_mti_lpcsx_phy_id_lo {
	u64 u;
	struct rpmx_mti_lpcsx_phy_id_lo_s {
		u64 phyid0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_phy_id_lo_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_PHY_ID_LO(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_PHY_ID_LO(u64 a)
{
	return 0x30010 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_rev
 *
 * RPM MTI LPCS Rev Register Core Revision
 */
union rpmx_mti_lpcsx_rev {
	u64 u;
	struct rpmx_mti_lpcsx_rev_s {
		u64 revision                         : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_rev_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_REV(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_REV(u64 a)
{
	return 0x30088 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_scratch
 *
 * RPM MTI LPCS Scratch Register General Purpose Test register
 */
union rpmx_mti_lpcsx_scratch {
	u64 u;
	struct rpmx_mti_lpcsx_scratch_s {
		u64 scratch                          : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcsx_scratch_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_SCRATCH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_SCRATCH(u64 a)
{
	return 0x30080 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_status
 *
 * RPM MTI LPCS Status Register Status indications
 */
union rpmx_mti_lpcsx_status {
	u64 u;
	struct rpmx_mti_lpcsx_status_s {
		u64 extd_capability                  : 1;
		u64 reserved_1                       : 1;
		u64 link_status                      : 1;
		u64 an_ability                       : 1;
		u64 reserved_4                       : 1;
		u64 an_complete                      : 1;
		u64 reserved_6_63                    : 58;
	} s;
	/* struct rpmx_mti_lpcsx_status_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_STATUS(u64 a)
{
	return 0x30008 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs#_usxgmii_rep
 *
 * RPM MTI LPCS USXGMII Replication Register 10G USXGMII replicator
 * setting
 */
union rpmx_mti_lpcsx_usxgmii_rep {
	u64 u;
	struct rpmx_mti_lpcsx_usxgmii_rep_s {
		u64 usx_replication                  : 10;
		u64 reserved_10_11                   : 2;
		u64 usx_rep_2_5                      : 1;
		u64 reserved_13_63                   : 51;
	} s;
	/* struct rpmx_mti_lpcsx_usxgmii_rep_s cn; */
};

static inline u64 RPMX_MTI_LPCSX_USXGMII_REP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCSX_USXGMII_REP(u64 a)
{
	return 0x300b0 + 0x100 * a;
}

/**
 * Register (RSL) rpm#_mti_lpcs_cfg_clock_rate
 *
 * RPM MTI LPCS Config Clock Rate Register Global LPCS clock rate
 * setting.
 */
union rpmx_mti_lpcs_cfg_clock_rate {
	u64 u;
	struct rpmx_mti_lpcs_cfg_clock_rate_s {
		u64 cfgclockrate                     : 4;
		u64 reserved_4_63                    : 60;
	} s;
	/* struct rpmx_mti_lpcs_cfg_clock_rate_s cn; */
};

static inline u64 RPMX_MTI_LPCS_CFG_CLOCK_RATE(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCS_CFG_CLOCK_RATE(void)
{
	return 0x307e0;
}

/**
 * Register (RSL) rpm#_mti_lpcs_gmode
 *
 * RPM MTI LPCS Global Mode Register Global LPCS Mode Configuration.
 */
union rpmx_mti_lpcs_gmode {
	u64 u;
	struct rpmx_mti_lpcs_gmode_s {
		u64 lpcs_enable                      : 4;
		u64 reserved_4_7                     : 4;
		u64 qsgmii_0_enable                  : 1;
		u64 reserved_9_10                    : 2;
		u64 usgmii_scramble_enable           : 1;
		u64 reserved_12_63                   : 52;
	} s;
	/* struct rpmx_mti_lpcs_gmode_s cn; */
};

static inline u64 RPMX_MTI_LPCS_GMODE(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCS_GMODE(void)
{
	return 0x307c0;
}

/**
 * Register (RSL) rpm#_mti_lpcs_gstatus
 *
 * RPM MTI LPCS Global Status Register Global LPCS status.
 */
union rpmx_mti_lpcs_gstatus {
	u64 u;
	struct rpmx_mti_lpcs_gstatus_s {
		u64 gsync_status                     : 8;
		u64 gan_done_status                  : 8;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_lpcs_gstatus_s cn; */
};

static inline u64 RPMX_MTI_LPCS_GSTATUS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCS_GSTATUS(void)
{
	return 0x307c8;
}

/**
 * Register (RSL) rpm#_mti_lpcs_usxgmii_enable_indication
 *
 * RPM MTI LPCS USXGMII Enable Indication Register Informal indication of
 * USXGMII channel replicator being enabled.
 */
union rpmx_mti_lpcs_usxgmii_enable_indication {
	u64 u;
	struct rpmx_mti_lpcs_usxgmii_enable_indication_s {
		u64 usxgmii_enable_ind               : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_lpcs_usxgmii_enable_indication_s cn; */
};

static inline u64 RPMX_MTI_LPCS_USXGMII_ENABLE_INDICATION(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_LPCS_USXGMII_ENABLE_INDICATION(void)
{
	return 0x307e8;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cf_gen_status
 *
 * RPM Mti Mac100  Cf Gen Status Register
 */
union rpmx_mti_mac100x_cf_gen_status {
	u64 u;
	struct rpmx_mti_mac100x_cf_gen_status_s {
		u64 cf_gen_ack_seen                  : 1;
		u64 reserved_1_63                    : 63;
	} s;
	/* struct rpmx_mti_mac100x_cf_gen_status_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CF_GEN_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CF_GEN_STATUS(u64 a)
{
	return 0x80f0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl01_pause_quanta
 *
 * RPM Mti Mac100  Cl01 Pause Quanta Register Class 0 and 1 pause quanta.
 * When link pause mode is enabled, CL0_PAUSE_QUANTA is used.
 */
union rpmx_mti_mac100x_cl01_pause_quanta {
	u64 u;
	struct rpmx_mti_mac100x_cl01_pause_quanta_s {
		u64 cl0_pause_quanta                 : 16;
		u64 cl1_pause_quanta                 : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl01_pause_quanta_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL01_PAUSE_QUANTA(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL01_PAUSE_QUANTA(u64 a)
{
	return 0x80a8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl01_quanta_thresh
 *
 * RPM Mti Mac100  Cl01 Quanta Thresh Register Class 0 and 1 refresh
 * threshold. When link pause mode is enabled, CL0_QUANTA_THRESH is used
 * for refreshing pause frames.
 */
union rpmx_mti_mac100x_cl01_quanta_thresh {
	u64 u;
	struct rpmx_mti_mac100x_cl01_quanta_thresh_s {
		u64 cl0_quanta_thresh                : 16;
		u64 cl1_quanta_thresh                : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl01_quanta_thresh_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL01_QUANTA_THRESH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL01_QUANTA_THRESH(u64 a)
{
	return 0x80c8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl1011_pause_quanta
 *
 * RPM Mti Mac100  Cl1011 Pause Quanta Register PFC Class 10 and Class 11
 * quanta value.
 */
union rpmx_mti_mac100x_cl1011_pause_quanta {
	u64 u;
	struct rpmx_mti_mac100x_cl1011_pause_quanta_s {
		u64 cl10_pause_quanta                : 16;
		u64 cl11_pause_quanta                : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl1011_pause_quanta_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL1011_PAUSE_QUANTA(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL1011_PAUSE_QUANTA(u64 a)
{
	return 0x8110 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl1011_quanta_thresh
 *
 * RPM Mti Mac100  Cl1011 Quanta Thresh Register PFC Class 10 and Class
 * 11 refresh threshold.
 */
union rpmx_mti_mac100x_cl1011_quanta_thresh {
	u64 u;
	struct rpmx_mti_mac100x_cl1011_quanta_thresh_s {
		u64 cl10_quanta_thresh               : 16;
		u64 cl11_quanta_thresh               : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl1011_quanta_thresh_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL1011_QUANTA_THRESH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL1011_QUANTA_THRESH(u64 a)
{
	return 0x8130 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl1213_pause_quanta
 *
 * RPM Mti Mac100  Cl1213 Pause Quanta Register PFC Class 12 and Class 13
 * quanta value.
 */
union rpmx_mti_mac100x_cl1213_pause_quanta {
	u64 u;
	struct rpmx_mti_mac100x_cl1213_pause_quanta_s {
		u64 cl12_pause_quanta                : 16;
		u64 cl13_pause_quanta                : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl1213_pause_quanta_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL1213_PAUSE_QUANTA(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL1213_PAUSE_QUANTA(u64 a)
{
	return 0x8118 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl1213_quanta_thresh
 *
 * RPM Mti Mac100  Cl1213 Quanta Thresh Register PFC Class 12 and Class
 * 13 refresh threshold.
 */
union rpmx_mti_mac100x_cl1213_quanta_thresh {
	u64 u;
	struct rpmx_mti_mac100x_cl1213_quanta_thresh_s {
		u64 cl12_quanta_thresh               : 16;
		u64 cl13_quanta_thresh               : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl1213_quanta_thresh_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL1213_QUANTA_THRESH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL1213_QUANTA_THRESH(u64 a)
{
	return 0x8138 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl1415_pause_quanta
 *
 * RPM Mti Mac100  Cl1415 Pause Quanta Register PFC Class 14 and Class 15
 * quanta value.
 */
union rpmx_mti_mac100x_cl1415_pause_quanta {
	u64 u;
	struct rpmx_mti_mac100x_cl1415_pause_quanta_s {
		u64 cl14_pause_quanta                : 16;
		u64 cl15_pause_quanta                : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl1415_pause_quanta_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL1415_PAUSE_QUANTA(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL1415_PAUSE_QUANTA(u64 a)
{
	return 0x8120 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl1415_quanta_thresh
 *
 * RPM Mti Mac100  Cl1415 Quanta Thresh Register PFC Class 14 and Class
 * 15 refresh threshold.
 */
union rpmx_mti_mac100x_cl1415_quanta_thresh {
	u64 u;
	struct rpmx_mti_mac100x_cl1415_quanta_thresh_s {
		u64 cl14_quanta_thresh               : 16;
		u64 cl15_quanta_thresh               : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl1415_quanta_thresh_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL1415_QUANTA_THRESH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL1415_QUANTA_THRESH(u64 a)
{
	return 0x8140 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl23_pause_quanta
 *
 * RPM Mti Mac100  Cl23 Pause Quanta Register Class 2 and 3 pause quanta
 */
union rpmx_mti_mac100x_cl23_pause_quanta {
	u64 u;
	struct rpmx_mti_mac100x_cl23_pause_quanta_s {
		u64 cl2_pause_quanta                 : 16;
		u64 cl3_pause_quanta                 : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl23_pause_quanta_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL23_PAUSE_QUANTA(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL23_PAUSE_QUANTA(u64 a)
{
	return 0x80b0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl23_quanta_thresh
 *
 * RPM Mti Mac100  Cl23 Quanta Thresh Register Class 2 and 3 refresh
 * threshold
 */
union rpmx_mti_mac100x_cl23_quanta_thresh {
	u64 u;
	struct rpmx_mti_mac100x_cl23_quanta_thresh_s {
		u64 cl2_quanta_thresh                : 16;
		u64 cl3_quanta_thresh                : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl23_quanta_thresh_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL23_QUANTA_THRESH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL23_QUANTA_THRESH(u64 a)
{
	return 0x80d0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl45_pause_quanta
 *
 * RPM Mti Mac100  Cl45 Pause Quanta Register Class 4 and 5 pause quanta
 */
union rpmx_mti_mac100x_cl45_pause_quanta {
	u64 u;
	struct rpmx_mti_mac100x_cl45_pause_quanta_s {
		u64 cl4_pause_quanta                 : 16;
		u64 cl5_pause_quanta                 : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl45_pause_quanta_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL45_PAUSE_QUANTA(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL45_PAUSE_QUANTA(u64 a)
{
	return 0x80b8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl45_quanta_thresh
 *
 * RPM Mti Mac100  Cl45 Quanta Thresh Register Class 4 and 5 refresh
 * threshold
 */
union rpmx_mti_mac100x_cl45_quanta_thresh {
	u64 u;
	struct rpmx_mti_mac100x_cl45_quanta_thresh_s {
		u64 cl4_quanta_thresh                : 16;
		u64 cl5_quanta_thresh                : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl45_quanta_thresh_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL45_QUANTA_THRESH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL45_QUANTA_THRESH(u64 a)
{
	return 0x80d8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl67_pause_quanta
 *
 * RPM Mti Mac100  Cl67 Pause Quanta Register Class 6 and 7 pause quanta
 */
union rpmx_mti_mac100x_cl67_pause_quanta {
	u64 u;
	struct rpmx_mti_mac100x_cl67_pause_quanta_s {
		u64 cl6_pause_quanta                 : 16;
		u64 cl7_pause_quanta                 : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl67_pause_quanta_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL67_PAUSE_QUANTA(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL67_PAUSE_QUANTA(u64 a)
{
	return 0x80c0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl67_quanta_thresh
 *
 * RPM Mti Mac100  Cl67 Quanta Thresh Register Class 6 and 7 refresh
 * threshold
 */
union rpmx_mti_mac100x_cl67_quanta_thresh {
	u64 u;
	struct rpmx_mti_mac100x_cl67_quanta_thresh_s {
		u64 cl6_quanta_thresh                : 16;
		u64 cl7_quanta_thresh                : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl67_quanta_thresh_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL67_QUANTA_THRESH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL67_QUANTA_THRESH(u64 a)
{
	return 0x80e0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl89_pause_quanta
 *
 * RPM Mti Mac100  Cl89 Pause Quanta Register Class 8 and 9 pause quanta
 */
union rpmx_mti_mac100x_cl89_pause_quanta {
	u64 u;
	struct rpmx_mti_mac100x_cl89_pause_quanta_s {
		u64 cl8_pause_quanta                 : 16;
		u64 cl9_pause_quanta                 : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl89_pause_quanta_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL89_PAUSE_QUANTA(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL89_PAUSE_QUANTA(u64 a)
{
	return 0x8108 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_cl89_quanta_thresh
 *
 * RPM Mti Mac100  Cl89 Quanta Thresh Register Class 8 and 9 refresh
 * threshold
 */
union rpmx_mti_mac100x_cl89_quanta_thresh {
	u64 u;
	struct rpmx_mti_mac100x_cl89_quanta_thresh_s {
		u64 cl8_quanta_thresh                : 16;
		u64 cl9_quanta_thresh                : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_cl89_quanta_thresh_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CL89_QUANTA_THRESH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CL89_QUANTA_THRESH(u64 a)
{
	return 0x8128 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_command_config
 *
 * RPM Mti Mac100  Command Config Register Control and Configuration
 */
union rpmx_mti_mac100x_command_config {
	u64 u;
	struct rpmx_mti_mac100x_command_config_s {
		u64 tx_ena                           : 1;
		u64 rx_ena                           : 1;
		u64 reserved_2_3                     : 2;
		u64 promis_en                        : 1;
		u64 pad_en                           : 1;
		u64 crc_fwd                          : 1;
		u64 pause_fwd                        : 1;
		u64 pause_ignore                     : 1;
		u64 tx_addr_ins                      : 1;
		u64 loopback_en                      : 1;
		u64 tx_pad_en                        : 1;
		u64 sw_reset                         : 1;
		u64 cntl_frame_ena                   : 1;
		u64 reserved_14                      : 1;
		u64 phy_txena                        : 1;
		u64 send_idle                        : 1;
		u64 reserved_17_18                   : 2;
		u64 pfc_mode                         : 1;
		u64 pause_pfc_comp                   : 1;
		u64 rx_sfd_any                       : 1;
		u64 tx_flush                         : 1;
		u64 tx_lowp_ena                      : 1;
		u64 reserved_24_25                   : 2;
		u64 tx_fifo_reset                    : 1;
		u64 flt_hdl_dis                      : 1;
		u64 tx_p_disable                     : 1;
		u64 rx_p_disable                     : 1;
		u64 short_preamble                   : 1;
		u64 no_preamble                      : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_command_config_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_COMMAND_CONFIG(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_COMMAND_CONFIG(u64 a)
{
	return 0x8010 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_crc_mode
 *
 * RPM Mti Mac100  Crc Mode Register Reserved.
 */
union rpmx_mti_mac100x_crc_mode {
	u64 u;
	struct rpmx_mti_mac100x_crc_mode_s {
		u64 reserved_0_15                    : 16;
		u64 disable_rx_crc_check             : 1;
		u64 reserved_17                      : 1;
		u64 crc_1byte                        : 1;
		u64 crc_2byte                        : 1;
		u64 crc_0byte                        : 1;
		u64 reserved_21_63                   : 43;
	} s;
	/* struct rpmx_mti_mac100x_crc_mode_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_CRC_MODE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_CRC_MODE(u64 a)
{
	return 0x8090 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_frm_length
 *
 * RPM Mti Mac100  Frm Length Register Maximum Frame Size
 */
union rpmx_mti_mac100x_frm_length {
	u64 u;
	struct rpmx_mti_mac100x_frm_length_s {
		u64 frm_length                       : 16;
		u64 tx_mtu                           : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_frm_length_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_FRM_LENGTH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_FRM_LENGTH(u64 a)
{
	return 0x8028 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_mac_addr_0
 *
 * RPM Mti Mac100  Mac Addr 0 Register First 4 bytes of MAC address
 */
union rpmx_mti_mac100x_mac_addr_0 {
	u64 u;
	struct rpmx_mti_mac100x_mac_addr_0_s {
		u64 mac_address_0                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_mac_addr_0_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_MAC_ADDR_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_MAC_ADDR_0(u64 a)
{
	return 0x8018 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_mac_addr_1
 *
 * RPM Mti Mac100  Mac Addr 1 Register Last 2 bytes of MAC address
 */
union rpmx_mti_mac100x_mac_addr_1 {
	u64 u;
	struct rpmx_mti_mac100x_mac_addr_1_s {
		u64 mac_address_1                    : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_mac100x_mac_addr_1_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_MAC_ADDR_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_MAC_ADDR_1(u64 a)
{
	return 0x8020 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_mdio_cfg_status
 *
 * INTERNAL: RPM Mti Mac100  Mdio Cfg Status Register  MDIO Configuration
 * and Status
 */
union rpmx_mti_mac100x_mdio_cfg_status {
	u64 u;
	struct rpmx_mti_mac100x_mdio_cfg_status_s {
		u64 mdio_busy                        : 1;
		u64 mdio_read_error                  : 1;
		u64 mdio_hold_time_setting           : 3;
		u64 mdio_disable_preamble            : 1;
		u64 mdio_clause45                    : 1;
		u64 mdio_clock_divisor               : 9;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_mac100x_mdio_cfg_status_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_MDIO_CFG_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_MDIO_CFG_STATUS(u64 a)
{
	return 0x8060 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_mdio_command
 *
 * INTERNAL: RPM Mti Mac100  Mdio Command Register  MDIO Command (PHY and
 * Port Address)
 */
union rpmx_mti_mac100x_mdio_command {
	u64 u;
	struct rpmx_mti_mac100x_mdio_command_s {
		u64 device_address                   : 5;
		u64 port_address                     : 5;
		u64 reserved_10_13                   : 4;
		u64 read_address_post_increment      : 1;
		u64 normal_read_transaction          : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_mac100x_mdio_command_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_MDIO_COMMAND(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_MDIO_COMMAND(u64 a)
{
	return 0x8068 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_mdio_data
 *
 * INTERNAL: RPM Mti Mac100  Mdio Data Register  MDIO Data to write and
 * last Data read
 */
union rpmx_mti_mac100x_mdio_data {
	u64 u;
	struct rpmx_mti_mac100x_mdio_data_s {
		u64 mdio_data                        : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_mac100x_mdio_data_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_MDIO_DATA(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_MDIO_DATA(u64 a)
{
	return 0x8070 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_mdio_regaddr
 *
 * INTERNAL: RPM Mti Mac100  Mdio Regaddr Register  MDIO Register
 * Address. Address of register within the PHY device to read from or
 * write to. After writing this register, an address-write transaction
 * will be initiated to set the PHY internal address register to the
 * value given.
 */
union rpmx_mti_mac100x_mdio_regaddr {
	u64 u;
	struct rpmx_mti_mac100x_mdio_regaddr_s {
		u64 mdio_regaddr                     : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_mac100x_mdio_regaddr_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_MDIO_REGADDR(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_MDIO_REGADDR(u64 a)
{
	return 0x8078 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_revision
 *
 * RPM Mti Mac100  Revision Register Package defined constants
 */
union rpmx_mti_mac100x_revision {
	u64 u;
	struct rpmx_mti_mac100x_revision_s {
		u64 core_revision                    : 8;
		u64 core_version                     : 8;
		u64 customer_revision                : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_revision_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_REVISION(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_REVISION(u64 a)
{
	return 0x8000 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_rx_fifo_sections
 *
 * RPM Mti Mac100  Rx Fifo Sections Register RX FIFO thresholds
 */
union rpmx_mti_mac100x_rx_fifo_sections {
	u64 u;
	struct rpmx_mti_mac100x_rx_fifo_sections_s {
		u64 rx_section_full                  : 16;
		u64 rx_section_empty                 : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_rx_fifo_sections_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_RX_FIFO_SECTIONS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_RX_FIFO_SECTIONS(u64 a)
{
	return 0x8038 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_rx_pause_status
 *
 * RPM Mti Mac100  Rx Pause Status Register Current per class received
 * pause status. 0 is used for link pause also.
 */
union rpmx_mti_mac100x_rx_pause_status {
	u64 u;
	struct rpmx_mti_mac100x_rx_pause_status_s {
		u64 pausestatus                      : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_mac100x_rx_pause_status_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_RX_PAUSE_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_RX_PAUSE_STATUS(u64 a)
{
	return 0x80e8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_scf_config0
 *
 * RPM MTI MAC100 Specific Control Frame Configuration 0 Register TBD
 */
union rpmx_mti_mac100x_scf_config0 {
	u64 u;
	struct rpmx_mti_mac100x_scf_config0_s {
		u64 mac_da                           : 48;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_mti_mac100x_scf_config0_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_SCF_CONFIG0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_SCF_CONFIG0(u64 a)
{
	return 0x9000 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_scf_config1
 *
 * RPM MTI MAC100 Specific Control Frame Configuration 1 Register TBD
 */
union rpmx_mti_mac100x_scf_config1 {
	u64 u;
	struct rpmx_mti_mac100x_scf_config1_s {
		u64 opcode                           : 16;
		u64 ether_type                       : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_scf_config1_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_SCF_CONFIG1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_SCF_CONFIG1(u64 a)
{
	return 0x9008 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_scf_config2
 *
 * RPM MTI MAC100 Specific Control Frame Configuration 2 Register TBD
 */
union rpmx_mti_mac100x_scf_config2 {
	u64 u;
	struct rpmx_mti_mac100x_scf_config2_s {
		u64 data                             : 64;
	} s;
	/* struct rpmx_mti_mac100x_scf_config2_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_SCF_CONFIG2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_SCF_CONFIG2(u64 a)
{
	return 0x9010 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_scf_control
 *
 * RPM MTI MAC100 Specific Control Frame Control Register TBD
 */
union rpmx_mti_mac100x_scf_control {
	u64 u;
	struct rpmx_mti_mac100x_scf_control_s {
		u64 send                             : 1;
		u64 reserved_1_63                    : 63;
	} s;
	/* struct rpmx_mti_mac100x_scf_control_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_SCF_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_SCF_CONTROL(u64 a)
{
	return 0x9018 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_scratch
 *
 * RPM Mti Mac100  Scratch Register General Purpose
 */
union rpmx_mti_mac100x_scratch {
	u64 u;
	struct rpmx_mti_mac100x_scratch_s {
		u64 scratch                          : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_scratch_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_SCRATCH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_SCRATCH(u64 a)
{
	return 0x8008 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_status
 *
 * RPM Mti Mac100  Status Register General Purpose Status
 */
union rpmx_mti_mac100x_status {
	u64 u;
	struct rpmx_mti_mac100x_status_s {
		u64 rx_loc_fault                     : 1;
		u64 rx_rem_fault                     : 1;
		u64 phy_los                          : 1;
		u64 ts_avail                         : 1;
		u64 rx_lowp                          : 1;
		u64 tx_empty                         : 1;
		u64 rx_empty                         : 1;
		u64 rx_lint_fault                    : 1;
		u64 tx_is_idle                       : 1;
		u64 reserved_9_63                    : 55;
	} s;
	/* struct rpmx_mti_mac100x_status_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_STATUS(u64 a)
{
	return 0x8080 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_ts_timestamp
 *
 * RPM Mti Mac100  Ts Timestamp Register Transmit Timestamp
 */
union rpmx_mti_mac100x_ts_timestamp {
	u64 u;
	struct rpmx_mti_mac100x_ts_timestamp_s {
		u64 ts_timestamp                     : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_ts_timestamp_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_TS_TIMESTAMP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_TS_TIMESTAMP(u64 a)
{
	return 0x80f8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_tx_fifo_sections
 *
 * RPM Mti Mac100  Tx Fifo Sections Register TX FIFO thresholds
 */
union rpmx_mti_mac100x_tx_fifo_sections {
	u64 u;
	struct rpmx_mti_mac100x_tx_fifo_sections_s {
		u64 tx_section_full                  : 16;
		u64 tx_section_empty                 : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_tx_fifo_sections_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_TX_FIFO_SECTIONS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_TX_FIFO_SECTIONS(u64 a)
{
	return 0x8040 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_tx_ipg_length
 *
 * RPM Mti Mac100  Tx Ipg Length Register TX InterPacketGap configuration
 */
union rpmx_mti_mac100x_tx_ipg_length {
	u64 u;
	struct rpmx_mti_mac100x_tx_ipg_length_s {
		u64 txipg                            : 7;
		u64 reserved_7                       : 1;
		u64 compensation_msb                 : 8;
		u64 compensation                     : 16;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_mac100x_tx_ipg_length_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_TX_IPG_LENGTH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_TX_IPG_LENGTH(u64 a)
{
	return 0x8088 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_mac100#_xif_mode
 *
 * RPM Mti Mac100  Xif Mode Register Interface Mode Configuration
 */
union rpmx_mti_mac100x_xif_mode {
	u64 u;
	struct rpmx_mti_mac100x_xif_mode_s {
		u64 xgmii                            : 1;
		u64 reserved_1_3                     : 3;
		u64 pausetimerx8                     : 1;
		u64 onestepena                       : 1;
		u64 rx_pause_bypass                  : 1;
		u64 reserved_7                       : 1;
		u64 tx_mac_rs_err                    : 1;
		u64 ts_delta_mode                    : 1;
		u64 ts_delay_mode                    : 1;
		u64 ts_binary_mode                   : 1;
		u64 ts_64upd_mode                    : 1;
		u64 reserved_13_15                   : 3;
		u64 rx_cnt_mode                      : 1;
		u64 pfc_pulse_mode                   : 1;
		u64 pfc_lp_mode                      : 1;
		u64 pfc_lp_16pri                     : 1;
		u64 reserved_20_63                   : 44;
	} s;
	/* struct rpmx_mti_mac100x_xif_mode_s cn; */
};

static inline u64 RPMX_MTI_MAC100X_XIF_MODE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_MAC100X_XIF_MODE(u64 a)
{
	return 0x8100 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_baser_status1
 *
 * RPM MTI PCS Baser Status1 Register Link Status Information.
 */
union rpmx_mti_pcs100x_baser_status1 {
	u64 u;
	struct rpmx_mti_pcs100x_baser_status1_s {
		u64 block_lock                       : 1;
		u64 high_ber                         : 1;
		u64 reserved_2_11                    : 10;
		u64 receive_link                     : 1;
		u64 reserved_13_63                   : 51;
	} s;
	/* struct rpmx_mti_pcs100x_baser_status1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_BASER_STATUS1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_BASER_STATUS1(u64 a)
{
	return 0x20100 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_baser_status2
 *
 * RPM MTI PCS Baser Status2 Register Link Status latches and error
 * counters.
 */
union rpmx_mti_pcs100x_baser_status2 {
	u64 u;
	struct rpmx_mti_pcs100x_baser_status2_s {
		u64 errored_cnt                      : 8;
		u64 ber_counter                      : 6;
		u64 high_ber                         : 1;
		u64 block_lock                       : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_baser_status2_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_BASER_STATUS2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_BASER_STATUS2(u64 a)
{
	return 0x20108 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_baser_test_control
 *
 * RPM MTI PCS Baser Test Control Register Test Pattern Generator and
 * Checker controls.
 */
union rpmx_mti_pcs100x_baser_test_control {
	u64 u;
	struct rpmx_mti_pcs100x_baser_test_control_s {
		u64 data_pattern_sel                 : 1;
		u64 select_square                    : 1;
		u64 rx_testpattern                   : 1;
		u64 tx_testpattern                   : 1;
		u64 reserved_4_6                     : 3;
		u64 select_random                    : 1;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_baser_test_control_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_BASER_TEST_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_BASER_TEST_CONTROL(u64 a)
{
	return 0x20150 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_baser_test_err_cnt
 *
 * RPM MTI PCS Baser Test Err Cnt Register Test Pattern Error Counter;
 * Clears on read; None roll-over.
 */
union rpmx_mti_pcs100x_baser_test_err_cnt {
	u64 u;
	struct rpmx_mti_pcs100x_baser_test_err_cnt_s {
		u64 counter                          : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_baser_test_err_cnt_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_BASER_TEST_ERR_CNT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_BASER_TEST_ERR_CNT(u64 a)
{
	return 0x20158 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_ber_high_order_cnt
 *
 * RPM MTI PCS Ber High Order Cnt Register BER High Order Counter of BER
 * bits 21:6; None roll-over.
 */
union rpmx_mti_pcs100x_ber_high_order_cnt {
	u64 u;
	struct rpmx_mti_pcs100x_ber_high_order_cnt_s {
		u64 ber_counter                      : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_ber_high_order_cnt_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_BER_HIGH_ORDER_CNT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_BER_HIGH_ORDER_CNT(u64 a)
{
	return 0x20160 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_bip_err_cnt_lane#
 *
 * RPM MTI PCS Bip Err Cnt Lane Register BIP Error Counter Lane \<b\>;
 * Clears on read; None roll-over. Internal: This group of registers
 * should be duplicated 0..19, but CSR3 does not support extern regs to
 * be non-aligned. Instead of duplicating 20 times, I prefer to create
 * several (aligned) groups.
 */
union rpmx_mti_pcs100x_bip_err_cnt_lanex {
	u64 u;
	struct rpmx_mti_pcs100x_bip_err_cnt_lanex_s {
		u64 error_counter                    : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_bip_err_cnt_lanex_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_BIP_ERR_CNT_LANEX(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_BIP_ERR_CNT_LANEX(u64 a, u64 b)
{
	return 0x20640 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_bip_err_cnt_lane2#
 *
 * RPM MTI PCS Bip Err Cnt Lane Register BIP Error Counter Lane \<b+8\>;
 * Clears on read; None roll-over. Internal: This group of registers
 * should be duplicated 0..19, but CSR3 does not support extern regs to
 * be non-aligned. Instead of duplicating 20 times, I prefer to create
 * several (aligned) groups.
 */
union rpmx_mti_pcs100x_bip_err_cnt_lane2x {
	u64 u;
	struct rpmx_mti_pcs100x_bip_err_cnt_lane2x_s {
		u64 error_counter                    : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_bip_err_cnt_lane2x_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_BIP_ERR_CNT_LANE2X(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_BIP_ERR_CNT_LANE2X(u64 a, u64 b)
{
	return 0x20680 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_bip_err_cnt_lane3#
 *
 * RPM MTI PCS Bip Err Cnt Lane Register BIP Error Counter Lane \<b+16\>;
 * Clears on read; None roll-over. Internal: This group of registers
 * should be duplicated 0..19, but CSR3 does not support extern regs to
 * be non-aligned. Instead of duplicating 20 times, I prefer to create
 * several (aligned) groups.
 */
union rpmx_mti_pcs100x_bip_err_cnt_lane3x {
	u64 u;
	struct rpmx_mti_pcs100x_bip_err_cnt_lane3x_s {
		u64 error_counter                    : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_bip_err_cnt_lane3x_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_BIP_ERR_CNT_LANE3X(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_BIP_ERR_CNT_LANE3X(u64 a, u64 b)
{
	return 0x206c0 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_control1
 *
 * RPM MTI PCS Control1 Register PCS Control.
 */
union rpmx_mti_pcs100x_control1 {
	u64 u;
	struct rpmx_mti_pcs100x_control1_s {
		u64 reserved_0_1                     : 2;
		u64 speed_selection                  : 4;
		u64 speed_always1                    : 1;
		u64 reserved_7_10                    : 4;
		u64 low_power                        : 1;
		u64 reserved_12                      : 1;
		u64 speed_select_always1             : 1;
		u64 loopback                         : 1;
		u64 reset                            : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_control1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_CONTROL1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_CONTROL1(u64 a)
{
	return 0x20000 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_control2
 *
 * RPM MTI PCS Control2 Register Operating speed indication.
 */
union rpmx_mti_pcs100x_control2 {
	u64 u;
	struct rpmx_mti_pcs100x_control2_s {
		u64 pcs_type                         : 4;
		u64 reserved_4_63                    : 60;
	} s;
	/* struct rpmx_mti_pcs100x_control2_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_CONTROL2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_CONTROL2(u64 a)
{
	return 0x20038 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_device_id0
 *
 * RPM MTI PCS Device Id0 Register PHY Identifier constant from package
 * parameter PHY_IDENTIFIER bits 15:4. Bits 3:0 always 0.
 */
union rpmx_mti_pcs100x_device_id0 {
	u64 u;
	struct rpmx_mti_pcs100x_device_id0_s {
		u64 identifier0                      : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_device_id0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_DEVICE_ID0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_DEVICE_ID0(u64 a)
{
	return 0x20010 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_device_id1
 *
 * RPM MTI PCS Device Id1 Register PHY Identifier constant from package
 * parameter PHY_IDENTIFIER bits 31:16.
 */
union rpmx_mti_pcs100x_device_id1 {
	u64 u;
	struct rpmx_mti_pcs100x_device_id1_s {
		u64 identifier1                      : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_device_id1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_DEVICE_ID1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_DEVICE_ID1(u64 a)
{
	return 0x20018 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_devices_in_pkg1
 *
 * RPM MTI PCS Devices In Pkg1 Register Constant indicating PCS presence.
 */
union rpmx_mti_pcs100x_devices_in_pkg1 {
	u64 u;
	struct rpmx_mti_pcs100x_devices_in_pkg1_s {
		u64 clause22                         : 1;
		u64 pmd_pma                          : 1;
		u64 wis_pres                         : 1;
		u64 pcs_pres                         : 1;
		u64 phy_xs                           : 1;
		u64 dte_xs                           : 1;
		u64 tc_pres                          : 1;
		u64 reserved_7_63                    : 57;
	} s;
	/* struct rpmx_mti_pcs100x_devices_in_pkg1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_DEVICES_IN_PKG1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_DEVICES_IN_PKG1(u64 a)
{
	return 0x20028 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_devices_in_pkg2
 *
 * RPM MTI PCS Devices In Pkg2 Register Vendor specific presence.
 */
union rpmx_mti_pcs100x_devices_in_pkg2 {
	u64 u;
	struct rpmx_mti_pcs100x_devices_in_pkg2_s {
		u64 reserved_0_12                    : 13;
		u64 clause22                         : 1;
		u64 device1                          : 1;
		u64 device2                          : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_devices_in_pkg2_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_DEVICES_IN_PKG2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_DEVICES_IN_PKG2(u64 a)
{
	return 0x20030 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_err_blk_high_order_cnt
 *
 * RPM MTI PCS Err Blk High Order Cnt Register Error Blocks High Order
 * Counter bits 21:8; None roll-over.
 */
union rpmx_mti_pcs100x_err_blk_high_order_cnt {
	u64 u;
	struct rpmx_mti_pcs100x_err_blk_high_order_cnt_s {
		u64 errored_blocks_counter           : 14;
		u64 reserved_14                      : 1;
		u64 high_order_present               : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_err_blk_high_order_cnt_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_ERR_BLK_HIGH_ORDER_CNT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_ERR_BLK_HIGH_ORDER_CNT(u64 a)
{
	return 0x20168 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_lane_mapping#
 *
 * RPM MTI PCS Lane Mapping Register Lane \<b\> mapping. Internal: This
 * group of registers should be duplicated 0..19, but CSR3 does not
 * support extern regs to be non-aligned. Instead of duplicating 20
 * times, I prefer to create several (aligned) groups.
 */
union rpmx_mti_pcs100x_lane_mappingx {
	u64 u;
	struct rpmx_mti_pcs100x_lane_mappingx_s {
		u64 mapping                          : 5;
		u64 reserved_5_63                    : 59;
	} s;
	/* struct rpmx_mti_pcs100x_lane_mappingx_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_LANE_MAPPINGX(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_LANE_MAPPINGX(u64 a, u64 b)
{
	return 0x20c80 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_lane_mapping2#
 *
 * RPM MTI PCS Lane Mapping Register Lane \<b+16\> mapping. Internal:
 * This group of registers should be duplicated 0..19, but CSR3 does not
 * support extern regs to be non-aligned. Instead of duplicating 20
 * times, I prefer to create several (aligned) groups.
 */
union rpmx_mti_pcs100x_lane_mapping2x {
	u64 u;
	struct rpmx_mti_pcs100x_lane_mapping2x_s {
		u64 mapping                          : 5;
		u64 reserved_5_63                    : 59;
	} s;
	/* struct rpmx_mti_pcs100x_lane_mapping2x_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_LANE_MAPPING2X(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_LANE_MAPPING2X(u64 a, u64 b)
{
	return 0x20d00 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_multilane_align_stat1
 *
 * RPM MTI PCS Multilane Align Stat1 Register Lane Alignment Status Bits
 * and Block Lock.
 */
union rpmx_mti_pcs100x_multilane_align_stat1 {
	u64 u;
	struct rpmx_mti_pcs100x_multilane_align_stat1_s {
		u64 lane_block_lock                  : 8;
		u64 reserved_8_11                    : 4;
		u64 lane_align_status                : 1;
		u64 reserved_13_63                   : 51;
	} s;
	/* struct rpmx_mti_pcs100x_multilane_align_stat1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_MULTILANE_ALIGN_STAT1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_MULTILANE_ALIGN_STAT1(u64 a)
{
	return 0x20190 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_multilane_align_stat2
 *
 * RPM MTI PCS Multilane Align Stat2 Register Lane Alignment Status Bits
 * and Block Lock.
 */
union rpmx_mti_pcs100x_multilane_align_stat2 {
	u64 u;
	struct rpmx_mti_pcs100x_multilane_align_stat2_s {
		u64 lane_block_lock                  : 12;
		u64 reserved_12_63                   : 52;
	} s;
	/* struct rpmx_mti_pcs100x_multilane_align_stat2_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_MULTILANE_ALIGN_STAT2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_MULTILANE_ALIGN_STAT2(u64 a)
{
	return 0x20198 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_multilane_align_stat3
 *
 * RPM MTI PCS Multilane Align Stat3 Register Lane Alignment Marker Lock
 * Status bits.
 */
union rpmx_mti_pcs100x_multilane_align_stat3 {
	u64 u;
	struct rpmx_mti_pcs100x_multilane_align_stat3_s {
		u64 lane_align_mlock                 : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_multilane_align_stat3_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_MULTILANE_ALIGN_STAT3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_MULTILANE_ALIGN_STAT3(u64 a)
{
	return 0x201a0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_multilane_align_stat4
 *
 * RPM MTI PCS Multilane Align Stat4 Register Lane Alignment Marker Lock
 * lane 8 to 19.
 */
union rpmx_mti_pcs100x_multilane_align_stat4 {
	u64 u;
	struct rpmx_mti_pcs100x_multilane_align_stat4_s {
		u64 lane_align_mlock                 : 12;
		u64 reserved_12_63                   : 52;
	} s;
	/* struct rpmx_mti_pcs100x_multilane_align_stat4_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_MULTILANE_ALIGN_STAT4(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_MULTILANE_ALIGN_STAT4(u64 a)
{
	return 0x201a8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_pkg_id0
 *
 * RPM MTI PCS Pkg Id0 Register Constant from package parameter
 * PACK_IDENTIFIER bits 15:0.
 */
union rpmx_mti_pcs100x_pkg_id0 {
	u64 u;
	struct rpmx_mti_pcs100x_pkg_id0_s {
		u64 identifier                       : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_pkg_id0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_PKG_ID0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_PKG_ID0(u64 a)
{
	return 0x20070 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_pkg_id1
 *
 * RPM MTI PCS Pkg Id1 Register Constant from package parameter
 * PACK_IDENTIFIER bits 31:16.
 */
union rpmx_mti_pcs100x_pkg_id1 {
	u64 u;
	struct rpmx_mti_pcs100x_pkg_id1_s {
		u64 identifier                       : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_pkg_id1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_PKG_ID1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_PKG_ID1(u64 a)
{
	return 0x20078 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_seed_a0
 *
 * RPM MTI PCS Seed A0 Register 10G Base-R Test Pattern Seed A bits 15:0.
 */
union rpmx_mti_pcs100x_seed_a0 {
	u64 u;
	struct rpmx_mti_pcs100x_seed_a0_s {
		u64 seed                             : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_seed_a0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_SEED_A0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_SEED_A0(u64 a)
{
	return 0x20110 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_seed_a1
 *
 * RPM MTI PCS Seed A1 Register 10G Base-R Test Pattern Seed A bits
 * 31:16.
 */
union rpmx_mti_pcs100x_seed_a1 {
	u64 u;
	struct rpmx_mti_pcs100x_seed_a1_s {
		u64 seed                             : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_seed_a1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_SEED_A1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_SEED_A1(u64 a)
{
	return 0x20118 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_seed_a2
 *
 * RPM MTI PCS Seed A2 Register 10G Base-R Test Pattern Seed A bits
 * 47:32.
 */
union rpmx_mti_pcs100x_seed_a2 {
	u64 u;
	struct rpmx_mti_pcs100x_seed_a2_s {
		u64 seed                             : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_seed_a2_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_SEED_A2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_SEED_A2(u64 a)
{
	return 0x20120 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_seed_a3
 *
 * RPM MTI PCS Seed A3 Register 10G Base-R Test Pattern Seed A bits
 * 57:48.
 */
union rpmx_mti_pcs100x_seed_a3 {
	u64 u;
	struct rpmx_mti_pcs100x_seed_a3_s {
		u64 seed                             : 10;
		u64 reserved_10_63                   : 54;
	} s;
	/* struct rpmx_mti_pcs100x_seed_a3_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_SEED_A3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_SEED_A3(u64 a)
{
	return 0x20128 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_seed_b0
 *
 * RPM MTI PCS Seed B0 Register 10G Base-R Test Pattern Seed B bits 15:0.
 */
union rpmx_mti_pcs100x_seed_b0 {
	u64 u;
	struct rpmx_mti_pcs100x_seed_b0_s {
		u64 seed                             : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_seed_b0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_SEED_B0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_SEED_B0(u64 a)
{
	return 0x20130 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_seed_b1
 *
 * RPM MTI PCS Seed B1 Register 10G Base-R Test Pattern Seed B bits
 * 31:16.
 */
union rpmx_mti_pcs100x_seed_b1 {
	u64 u;
	struct rpmx_mti_pcs100x_seed_b1_s {
		u64 seed                             : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_seed_b1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_SEED_B1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_SEED_B1(u64 a)
{
	return 0x20138 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_seed_b2
 *
 * RPM MTI PCS Seed B2 Register 10G Base-R Test Pattern Seed B bits
 * 47:32.
 */
union rpmx_mti_pcs100x_seed_b2 {
	u64 u;
	struct rpmx_mti_pcs100x_seed_b2_s {
		u64 seed                             : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_seed_b2_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_SEED_B2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_SEED_B2(u64 a)
{
	return 0x20140 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_seed_b3
 *
 * RPM MTI PCS Seed B3 Register 10G Base-R Test Pattern Seed B bits
 * 57:48.
 */
union rpmx_mti_pcs100x_seed_b3 {
	u64 u;
	struct rpmx_mti_pcs100x_seed_b3_s {
		u64 seed                             : 10;
		u64 reserved_10_63                   : 54;
	} s;
	/* struct rpmx_mti_pcs100x_seed_b3_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_SEED_B3(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_SEED_B3(u64 a)
{
	return 0x20148 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_speed_ability
 *
 * RPM MTI PCS Speed Ability Register PCS supported speeds (values as
 * defined by standard only, no proprietary speeds).
 */
union rpmx_mti_pcs100x_speed_ability {
	u64 u;
	struct rpmx_mti_pcs100x_speed_ability_s {
		u64 c10geth                          : 1;
		u64 c10pass_ts                       : 1;
		u64 c40g                             : 1;
		u64 c100g                            : 1;
		u64 c25g                             : 1;
		u64 c50g                             : 1;
		u64 reserved_6_63                    : 58;
	} s;
	/* struct rpmx_mti_pcs100x_speed_ability_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_SPEED_ABILITY(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_SPEED_ABILITY(u64 a)
{
	return 0x20020 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_status1
 *
 * RPM MTI PCS Status1 Register PCS Status.
 */
union rpmx_mti_pcs100x_status1 {
	u64 u;
	struct rpmx_mti_pcs100x_status1_s {
		u64 reserved_0                       : 1;
		u64 low_power_ability                : 1;
		u64 pcs_receive_link                 : 1;
		u64 reserved_3_6                     : 4;
		u64 fault                            : 1;
		u64 rx_lpi_active                    : 1;
		u64 tx_lpi_active                    : 1;
		u64 rx_lpi                           : 1;
		u64 tx_lpi                           : 1;
		u64 reserved_12_63                   : 52;
	} s;
	/* struct rpmx_mti_pcs100x_status1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_STATUS1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_STATUS1(u64 a)
{
	return 0x20008 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_status2
 *
 * RPM MTI PCS Status2 Register Fault status; Device capabilities
 */
union rpmx_mti_pcs100x_status2 {
	u64 u;
	struct rpmx_mti_pcs100x_status2_s {
		u64 c10gbase_r                       : 1;
		u64 c10gbase_x                       : 1;
		u64 c10gbase_w                       : 1;
		u64 c10gbase_t                       : 1;
		u64 c40gbase_r                       : 1;
		u64 c100gbase_r                      : 1;
		u64 reserved_6                       : 1;
		u64 c25gbase_r                       : 1;
		u64 c50gbase_r                       : 1;
		u64 reserved_9                       : 1;
		u64 receive_fault                    : 1;
		u64 transmit_fault                   : 1;
		u64 reserved_12_13                   : 2;
		u64 device_present                   : 2;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_status2_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_STATUS2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_STATUS2(u64 a)
{
	return 0x20040 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_core_rev
 *
 * RPM MTI PCS Vendor Core Rev Register Vendor Specific Reg; Core
 * Revision as defined by CORE_REVISION package parameter.
 */
union rpmx_mti_pcs100x_vendor_core_rev {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_core_rev_s {
		u64 revision                         : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_core_rev_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_CORE_REV(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_CORE_REV(u64 a)
{
	return 0x21008 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_mirror_vl0_0
 *
 * INTERNAL: RPM Mti Pcs100  Vendor Mirror Vl0 0 Register  Vendor
 * Specific Reg; Marker pattern for PCS Virtual Lane 0.  This register
 * exists due to standard backward compatibility of original 40G PCS
 * definition This register and VLx_y register access a same shared
 * register. Thus, writing this register will affect VLx_y register as
 * well. It is highly recommended NOT TO WRITE to this register.
 */
union rpmx_mti_pcs100x_vendor_mirror_vl0_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_mirror_vl0_0_s {
		u64 mirror_m0                        : 8;
		u64 mirror_m1                        : 8;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_mirror_vl0_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL0_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL0_0(u64 a)
{
	return 0x21040 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_mirror_vl0_1
 *
 * INTERNAL: RPM Mti Pcs100  Vendor Mirror Vl0 1 Register  Vendor
 * Specific Reg; Last byte of PCS Virtual Lane 0 marker pattern.  This
 * register exists due to standard backward compatibility of original 40G
 * PCS definition This register and VLx_y register access a same shared
 * register. Thus, writing this register will affect VLx_y register as
 * well. It is highly recommended NOT TO WRITE to this register.
 */
union rpmx_mti_pcs100x_vendor_mirror_vl0_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_mirror_vl0_1_s {
		u64 mirror_m2                        : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_mirror_vl0_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL0_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL0_1(u64 a)
{
	return 0x21048 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_mirror_vl1_0
 *
 * INTERNAL: RPM Mti Pcs100  Vendor Mirror Vl1 0 Register  Vendor
 * Specific Reg; Marker pattern for PCS Virtual Lane 1.  This register
 * exists due to standard backward compatibility of original 40G PCS
 * definition This register and VLx_y register access a same shared
 * register. Thus, writing this register will affect VLx_y register as
 * well. It is highly recommended NOT TO WRITE to this register.
 */
union rpmx_mti_pcs100x_vendor_mirror_vl1_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_mirror_vl1_0_s {
		u64 mirror_m0                        : 8;
		u64 mirror_m1                        : 8;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_mirror_vl1_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL1_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL1_0(u64 a)
{
	return 0x21050 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_mirror_vl1_1
 *
 * INTERNAL: RPM Mti Pcs100  Vendor Mirror Vl1 1 Register  Vendor
 * Specific Reg; Last byte of PCS Virtual Lane 1 marker pattern.  This
 * register exists due to standard backward compatibility of original 40G
 * PCS definition This register and VLx_y register access a same shared
 * register. Thus, writing this register will affect VLx_y register as
 * well. It is highly recommended NOT TO WRITE to this register.
 */
union rpmx_mti_pcs100x_vendor_mirror_vl1_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_mirror_vl1_1_s {
		u64 mirror_m2                        : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_mirror_vl1_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL1_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL1_1(u64 a)
{
	return 0x21058 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_mirror_vl2_0
 *
 * INTERNAL: RPM Mti Pcs100  Vendor Mirror Vl2 0 Register  Vendor
 * Specific Reg; Marker pattern for PCS Virtual Lane 2.  This register
 * exists due to standard backward compatibility of original 40G PCS
 * definition This register and VLx_y register access a same shared
 * register. Thus, writing this register will affect VLx_y register as
 * well. It is highly recommended NOT TO WRITE to this register.
 */
union rpmx_mti_pcs100x_vendor_mirror_vl2_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_mirror_vl2_0_s {
		u64 mirror_m0                        : 8;
		u64 mirror_m1                        : 8;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_mirror_vl2_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL2_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL2_0(u64 a)
{
	return 0x21060 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_mirror_vl2_1
 *
 * INTERNAL: RPM Mti Pcs100  Vendor Mirror Vl2 1 Register  Vendor
 * Specific Reg; Last byte of PCS Virtual Lane 2 marker pattern.  This
 * register exists due to standard backward compatibility of original 40G
 * PCS definition This register and VLx_y register access a same shared
 * register. Thus, writing this register will affect VLx_y register as
 * well. It is highly recommended NOT TO WRITE to this register.
 */
union rpmx_mti_pcs100x_vendor_mirror_vl2_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_mirror_vl2_1_s {
		u64 mirror_m2                        : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_mirror_vl2_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL2_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL2_1(u64 a)
{
	return 0x21068 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_mirror_vl3_0
 *
 * INTERNAL: RPM Mti Pcs100  Vendor Mirror Vl3 0 Register  Vendor
 * Specific Reg; Marker pattern for PCS Virtual Lane 3.  This register
 * exists due to standard backward compatibility of original 40G PCS
 * definition This register and VLx_y register access a same shared
 * register. Thus, writing this register will affect VLx_y register as
 * well. It is highly recommended NOT TO WRITE to this register.
 */
union rpmx_mti_pcs100x_vendor_mirror_vl3_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_mirror_vl3_0_s {
		u64 mirror_m0                        : 8;
		u64 mirror_m1                        : 8;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_mirror_vl3_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL3_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL3_0(u64 a)
{
	return 0x21070 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_mirror_vl3_1
 *
 * INTERNAL: RPM Mti Pcs100  Vendor Mirror Vl3 1 Register  Vendor
 * Specific Reg; Last byte of PCS Virtual Lane 3 marker pattern.  This
 * register exists due to standard backward compatibility of original 40G
 * PCS definition This register and VLx_y register access a same shared
 * register. Thus, writing this register will affect VLx_y register as
 * well. It is highly recommended NOT TO WRITE to this register.
 */
union rpmx_mti_pcs100x_vendor_mirror_vl3_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_mirror_vl3_1_s {
		u64 mirror_m2                        : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_mirror_vl3_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL3_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_MIRROR_VL3_1(u64 a)
{
	return 0x21078 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_pcs_mode
 *
 * RPM MTI PCS Vendor Pcs Mode Register Vendor Specific Reg; Configure
 * PCS supporting Clause 49 or 82 Encoder/Decoder, MLD.
 */
union rpmx_mti_pcs100x_vendor_pcs_mode {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_pcs_mode_s {
		u64 ena_clause49                     : 1;
		u64 disable_mld                      : 1;
		u64 hi_ber25                         : 1;
		u64 hi_ber5                          : 1;
		u64 reserved_4_7                     : 4;
		u64 st_ena_clause49                  : 1;
		u64 st_disable_mld                   : 1;
		u64 st_hi_ber25                      : 1;
		u64 st_hi_ber5                       : 1;
		u64 reserved_12_63                   : 52;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_pcs_mode_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_PCS_MODE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_PCS_MODE(u64 a)
{
	return 0x21080 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_scratch
 *
 * RPM MTI PCS Vendor Scratch Register Vendor Specific Reg; Scratch
 * Register.
 */
union rpmx_mti_pcs100x_vendor_scratch {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_scratch_s {
		u64 scratch                          : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_scratch_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_SCRATCH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_SCRATCH(u64 a)
{
	return 0x21000 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_txlane_thresh
 *
 * RPM MTI PCS Vendor Txlane Thresh Register Vendor Specific Reg; Defines
 * the transmit line decoupling FIFOs almost full threshold.
 */
union rpmx_mti_pcs100x_vendor_txlane_thresh {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_txlane_thresh_s {
		u64 threshold0                       : 4;
		u64 threshold1                       : 4;
		u64 threshold2                       : 4;
		u64 threshold3                       : 4;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_txlane_thresh_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_TXLANE_THRESH(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_TXLANE_THRESH(u64 a)
{
	return 0x21018 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vendor_vl_intvl
 *
 * RPM MTI PCS Vendor Vl Intvl Register Vendor Specific Reg; Set the
 * amount of data between markers. (I.e. distance of markers-1).
 */
union rpmx_mti_pcs100x_vendor_vl_intvl {
	u64 u;
	struct rpmx_mti_pcs100x_vendor_vl_intvl_s {
		u64 marker_counter                   : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vendor_vl_intvl_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VENDOR_VL_INTVL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VENDOR_VL_INTVL(u64 a)
{
	return 0x21010 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl0_0
 *
 * RPM MTI PCS Vl0 0 Register Vendor Specific Reg; Marker pattern for PCS
 * Virtual Lane 0.
 */
union rpmx_mti_pcs100x_vl0_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl0_0_s {
		u64 vl0_0                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl0_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL0_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL0_0(u64 a)
{
	return 0x21200 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl0_1
 *
 * RPM MTI PCS Vl0 1 Register Vendor Specific Reg; Last byte of PCS
 * Virtual Lane 0 marker pattern.
 */
union rpmx_mti_pcs100x_vl0_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl0_1_s {
		u64 vl0_1                            : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl0_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL0_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL0_1(u64 a)
{
	return 0x21208 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl10_0
 *
 * RPM MTI PCS Vl10 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl10_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl10_0_s {
		u64 vl10_0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl10_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL10_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL10_0(u64 a)
{
	return 0x212a0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl10_1
 *
 * RPM MTI PCS Vl10 1 Register Marker pattern m2 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl10_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl10_1_s {
		u64 vl10_1                           : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl10_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL10_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL10_1(u64 a)
{
	return 0x212a8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl11_0
 *
 * RPM MTI PCS Vl11 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl11_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl11_0_s {
		u64 vl11_0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl11_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL11_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL11_0(u64 a)
{
	return 0x212b0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl11_1
 *
 * RPM MTI PCS Vl11 1 Register Marker pattern m2 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl11_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl11_1_s {
		u64 vl11_1                           : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl11_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL11_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL11_1(u64 a)
{
	return 0x212b8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl12_0
 *
 * RPM MTI PCS Vl12 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl12_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl12_0_s {
		u64 vl12_0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl12_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL12_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL12_0(u64 a)
{
	return 0x212c0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl12_1
 *
 * RPM MTI PCS Vl12 1 Register Marker pattern m2 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl12_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl12_1_s {
		u64 vl12_1                           : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl12_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL12_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL12_1(u64 a)
{
	return 0x212c8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl13_0
 *
 * RPM MTI PCS Vl13 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl13_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl13_0_s {
		u64 vl13_0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl13_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL13_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL13_0(u64 a)
{
	return 0x212d0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl13_1
 *
 * RPM MTI PCS Vl13 1 Register Marker pattern m2 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl13_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl13_1_s {
		u64 vl13_1                           : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl13_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL13_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL13_1(u64 a)
{
	return 0x212d8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl14_0
 *
 * RPM MTI PCS Vl14 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl14_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl14_0_s {
		u64 vl14_0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl14_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL14_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL14_0(u64 a)
{
	return 0x212e0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl14_1
 *
 * RPM MTI PCS Vl14 1 Register Marker pattern m2 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl14_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl14_1_s {
		u64 vl14_1                           : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl14_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL14_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL14_1(u64 a)
{
	return 0x212e8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl15_0
 *
 * RPM MTI PCS Vl15 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl15_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl15_0_s {
		u64 vl15_0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl15_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL15_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL15_0(u64 a)
{
	return 0x212f0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl15_1
 *
 * RPM MTI PCS Vl15 1 Register Marker pattern m2 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl15_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl15_1_s {
		u64 vl15_1                           : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl15_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL15_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL15_1(u64 a)
{
	return 0x212f8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl16_0
 *
 * RPM MTI PCS Vl16 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl16_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl16_0_s {
		u64 vl16_0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl16_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL16_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL16_0(u64 a)
{
	return 0x21300 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl16_1
 *
 * RPM MTI PCS Vl16 1 Register Marker pattern m2 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl16_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl16_1_s {
		u64 vl16_1                           : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl16_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL16_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL16_1(u64 a)
{
	return 0x21308 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl17_0
 *
 * RPM MTI PCS Vl17 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl17_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl17_0_s {
		u64 vl17_0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl17_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL17_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL17_0(u64 a)
{
	return 0x21310 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl17_1
 *
 * RPM MTI PCS Vl17 1 Register Marker pattern m2 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl17_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl17_1_s {
		u64 vl17_1                           : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl17_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL17_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL17_1(u64 a)
{
	return 0x21318 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl18_0
 *
 * RPM MTI PCS Vl18 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl18_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl18_0_s {
		u64 vl18_0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl18_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL18_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL18_0(u64 a)
{
	return 0x21320 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl18_1
 *
 * RPM MTI PCS Vl18 1 Register Marker pattern m2 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl18_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl18_1_s {
		u64 vl18_1                           : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl18_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL18_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL18_1(u64 a)
{
	return 0x21328 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl19_0
 *
 * RPM MTI PCS Vl19 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl19_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl19_0_s {
		u64 vl19_0                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl19_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL19_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL19_0(u64 a)
{
	return 0x21330 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl19_1
 *
 * RPM MTI PCS Vl19 1 Register Marker pattern m2 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl19_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl19_1_s {
		u64 vl19_1                           : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl19_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL19_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL19_1(u64 a)
{
	return 0x21338 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl1_0
 *
 * RPM MTI PCS Vl1 0 Register Vendor Specific Reg; Marker pattern for PCS
 * Virtual Lane 1.
 */
union rpmx_mti_pcs100x_vl1_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl1_0_s {
		u64 vl1_0                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl1_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL1_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL1_0(u64 a)
{
	return 0x21210 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl1_1
 *
 * RPM MTI PCS Vl1 1 Register Vendor Specific Reg; Last byte of PCS
 * Virtual Lane 1 marker pattern.
 */
union rpmx_mti_pcs100x_vl1_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl1_1_s {
		u64 vl1_1                            : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl1_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL1_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL1_1(u64 a)
{
	return 0x21218 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl2_0
 *
 * RPM MTI PCS Vl2 0 Register Vendor Specific Reg; Marker pattern for PCS
 * Virtual Lane 2.
 */
union rpmx_mti_pcs100x_vl2_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl2_0_s {
		u64 vl2_0                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl2_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL2_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL2_0(u64 a)
{
	return 0x21220 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl2_1
 *
 * RPM MTI PCS Vl2 1 Register Vendor Specific Reg; Last byte of PCS
 * Virtual Lane 2 marker pattern.
 */
union rpmx_mti_pcs100x_vl2_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl2_1_s {
		u64 vl2_1                            : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl2_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL2_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL2_1(u64 a)
{
	return 0x21228 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl3_0
 *
 * RPM MTI PCS Vl3 0 Register Vendor Specific Reg; Marker pattern for PCS
 * Virtual Lane 3.
 */
union rpmx_mti_pcs100x_vl3_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl3_0_s {
		u64 vl3_0                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl3_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL3_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL3_0(u64 a)
{
	return 0x21230 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl3_1
 *
 * RPM MTI PCS Vl3 1 Register Vendor Specific Reg; Last byte of PCS
 * Virtual Lane 3 marker pattern.
 */
union rpmx_mti_pcs100x_vl3_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl3_1_s {
		u64 vl3_1                            : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl3_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL3_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL3_1(u64 a)
{
	return 0x21238 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl4_0
 *
 * RPM MTI PCS Vl4 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl4_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl4_0_s {
		u64 vl4_0                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl4_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL4_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL4_0(u64 a)
{
	return 0x21240 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl4_1
 *
 * RPM MTI PCS Vl4 1 Register Marker pattern m2 for PCS Virtual Lane. RAZ
 * for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl4_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl4_1_s {
		u64 vl4_1                            : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl4_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL4_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL4_1(u64 a)
{
	return 0x21248 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl5_0
 *
 * RPM MTI PCS Vl5 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl5_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl5_0_s {
		u64 vl5_0                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl5_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL5_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL5_0(u64 a)
{
	return 0x21250 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl5_1
 *
 * RPM MTI PCS Vl5 1 Register Marker pattern m2 for PCS Virtual Lane. RAZ
 * for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl5_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl5_1_s {
		u64 vl5_1                            : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl5_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL5_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL5_1(u64 a)
{
	return 0x21258 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl6_0
 *
 * RPM MTI PCS Vl6 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl6_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl6_0_s {
		u64 vl6_0                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl6_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL6_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL6_0(u64 a)
{
	return 0x21260 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl6_1
 *
 * RPM MTI PCS Vl6 1 Register Marker pattern m2 for PCS Virtual Lane. RAZ
 * for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl6_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl6_1_s {
		u64 vl6_1                            : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl6_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL6_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL6_1(u64 a)
{
	return 0x21268 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl7_0
 *
 * RPM MTI PCS Vl7 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl7_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl7_0_s {
		u64 vl7_0                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl7_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL7_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL7_0(u64 a)
{
	return 0x21270 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl7_1
 *
 * RPM MTI PCS Vl7 1 Register Marker pattern m2 for PCS Virtual Lane. RAZ
 * for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl7_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl7_1_s {
		u64 vl7_1                            : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl7_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL7_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL7_1(u64 a)
{
	return 0x21278 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl8_0
 *
 * RPM MTI PCS Vl8 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl8_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl8_0_s {
		u64 vl8_0                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl8_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL8_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL8_0(u64 a)
{
	return 0x21280 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl8_1
 *
 * RPM MTI PCS Vl8 1 Register Marker pattern m2 for PCS Virtual Lane. RAZ
 * for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl8_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl8_1_s {
		u64 vl8_1                            : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl8_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL8_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL8_1(u64 a)
{
	return 0x21288 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl9_0
 *
 * RPM MTI PCS Vl9 0 Register Marker pattern m1,m0 for PCS Virtual Lane.
 * RAZ for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl9_0 {
	u64 u;
	struct rpmx_mti_pcs100x_vl9_0_s {
		u64 vl9_0                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_pcs100x_vl9_0_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL9_0(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL9_0(u64 a)
{
	return 0x21290 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_pcs100#_vl9_1
 *
 * RPM MTI PCS Vl9 1 Register Marker pattern m2 for PCS Virtual Lane. RAZ
 * for odd PCS channels.
 */
union rpmx_mti_pcs100x_vl9_1 {
	u64 u;
	struct rpmx_mti_pcs100x_vl9_1_s {
		u64 vl9_1                            : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_pcs100x_vl9_1_s cn; */
};

static inline u64 RPMX_MTI_PCS100X_VL9_1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_PCS100X_VL9_1(u64 a)
{
	return 0x21298 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_ccw_hi#
 *
 * RPM MTI RSFEC Corrected Codewords High Register Counts number of
 * corrected RSFEC codewords. Upper 16 bits. Clears on read. Non roll-
 * over.
 */
union rpmx_mti_rsfec_ccw_hix {
	u64 u;
	struct rpmx_mti_rsfec_ccw_hix_s {
		u64 ccw_hi                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_rsfec_ccw_hix_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_CCW_HIX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_CCW_HIX(u64 a)
{
	return 0x38018 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_ccw_lo#
 *
 * RPM MTI RSFEC Corrected Codewords Low Register Counts number of
 * corrected RSFEC codewords. Lower 16 bits. Must be read first. Non
 * roll-over when upper 16-bits are 0xffff.
 */
union rpmx_mti_rsfec_ccw_lox {
	u64 u;
	struct rpmx_mti_rsfec_ccw_lox_s {
		u64 ccw_lo                           : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_rsfec_ccw_lox_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_CCW_LOX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_CCW_LOX(u64 a)
{
	return 0x38010 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_control#
 *
 * RPM MTI RSFEC Control Register Control register for enabling RS-FEC
 * functions.
 */
union rpmx_mti_rsfec_controlx {
	u64 u;
	struct rpmx_mti_rsfec_controlx_s {
		u64 bypass_correction                : 1;
		u64 bypass_error_indication          : 1;
		u64 degrade_enable                   : 1;
		u64 am16_copy_dis                    : 1;
		u64 reserved_4_7                     : 4;
		u64 kp_enable                        : 1;
		u64 tc_pad_value                     : 1;
		u64 tc_pad_alter                     : 1;
		u64 reserved_11_63                   : 53;
	} s;
	/* struct rpmx_mti_rsfec_controlx_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_CONTROLX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_CONTROLX(u64 a)
{
	return 0x38000 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_dec_thresh#
 *
 * RPM MTI RSFEC Decoder Channel Threshold Register RSFEC decoder channel
 * threshold.
 */
union rpmx_mti_rsfec_dec_threshx {
	u64 u;
	struct rpmx_mti_rsfec_dec_threshx_s {
		u64 dec_thresh                       : 6;
		u64 reserved_6_63                    : 58;
	} s;
	/* struct rpmx_mti_rsfec_dec_threshx_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_DEC_THRESHX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_DEC_THRESHX(u64 a)
{
	return 0x38038 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_lane_map#
 *
 * RPM MTI RSFEC Lane Map Register RSFEC lane mappings.
 */
union rpmx_mti_rsfec_lane_mapx {
	u64 u;
	struct rpmx_mti_rsfec_lane_mapx_s {
		u64 rsfec_lane_map                   : 8;
		u64 reserved_8_63                    : 56;
	} s;
	/* struct rpmx_mti_rsfec_lane_mapx_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_LANE_MAPX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_LANE_MAPX(u64 a)
{
	return 0x38030 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_nccw_hi#
 *
 * RPM MTI RSFEC Uncorrected Codewords High Register Counts number of
 * uncorrected RSFEC codewords. Upper 16 bits. Clears on read. Non roll-
 * over.
 */
union rpmx_mti_rsfec_nccw_hix {
	u64 u;
	struct rpmx_mti_rsfec_nccw_hix_s {
		u64 nccw_hi                          : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_rsfec_nccw_hix_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_NCCW_HIX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_NCCW_HIX(u64 a)
{
	return 0x38028 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_nccw_lo#
 *
 * RPM MTI RSFEC Uncorrected Codewords Low Register Counts number of
 * uncorrected RSFEC codewords. Lower 16 bits. Non roll-over when upper
 * 16-bits are 0xffff.
 */
union rpmx_mti_rsfec_nccw_lox {
	u64 u;
	struct rpmx_mti_rsfec_nccw_lox_s {
		u64 nccw_lo                          : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_rsfec_nccw_lox_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_NCCW_LOX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_NCCW_LOX(u64 a)
{
	return 0x38020 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_10se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 10se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_10se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_10se_0_s {
		u64 cw_corrected_10se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_10se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_10SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_10SE_0(void)
{
	return 0x401a8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_10se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 10se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_10se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_10se_1_s {
		u64 cw_corrected_10se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_10se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_10SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_10SE_1(void)
{
	return 0x40268;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_10se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 10se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_10se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_10se_2_s {
		u64 cw_corrected_10se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_10se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_10SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_10SE_2(void)
{
	return 0x40328;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_10se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 10se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_10se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_10se_3_s {
		u64 cw_corrected_10se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_10se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_10SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_10SE_3(void)
{
	return 0x403e8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_11se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 11se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_11se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_11se_0_s {
		u64 cw_corrected_11se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_11se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_11SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_11SE_0(void)
{
	return 0x401b0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_11se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 11se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_11se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_11se_1_s {
		u64 cw_corrected_11se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_11se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_11SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_11SE_1(void)
{
	return 0x40270;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_11se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 11se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_11se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_11se_2_s {
		u64 cw_corrected_11se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_11se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_11SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_11SE_2(void)
{
	return 0x40330;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_11se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 11se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_11se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_11se_3_s {
		u64 cw_corrected_11se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_11se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_11SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_11SE_3(void)
{
	return 0x403f0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_12se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 12se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_12se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_12se_0_s {
		u64 cw_corrected_12se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_12se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_12SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_12SE_0(void)
{
	return 0x401b8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_12se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 12se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_12se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_12se_1_s {
		u64 cw_corrected_12se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_12se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_12SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_12SE_1(void)
{
	return 0x40278;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_12se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 12se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_12se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_12se_2_s {
		u64 cw_corrected_12se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_12se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_12SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_12SE_2(void)
{
	return 0x40338;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_12se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 12se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_12se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_12se_3_s {
		u64 cw_corrected_12se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_12se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_12SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_12SE_3(void)
{
	return 0x403f8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_13se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 13se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_13se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_13se_0_s {
		u64 cw_corrected_13se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_13se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_13SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_13SE_0(void)
{
	return 0x401c0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_13se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 13se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_13se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_13se_1_s {
		u64 cw_corrected_13se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_13se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_13SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_13SE_1(void)
{
	return 0x40280;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_13se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 13se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_13se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_13se_2_s {
		u64 cw_corrected_13se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_13se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_13SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_13SE_2(void)
{
	return 0x40340;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_13se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 13se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_13se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_13se_3_s {
		u64 cw_corrected_13se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_13se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_13SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_13SE_3(void)
{
	return 0x40400;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_14se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 14se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_14se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_14se_0_s {
		u64 cw_corrected_14se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_14se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_14SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_14SE_0(void)
{
	return 0x401c8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_14se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 14se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_14se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_14se_1_s {
		u64 cw_corrected_14se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_14se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_14SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_14SE_1(void)
{
	return 0x40288;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_14se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 14se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_14se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_14se_2_s {
		u64 cw_corrected_14se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_14se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_14SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_14SE_2(void)
{
	return 0x40348;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_14se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 14se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_14se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_14se_3_s {
		u64 cw_corrected_14se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_14se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_14SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_14SE_3(void)
{
	return 0x40408;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_15se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 15se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_15se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_15se_0_s {
		u64 cw_corrected_15se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_15se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_15SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_15SE_0(void)
{
	return 0x401d0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_15se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 15se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_15se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_15se_1_s {
		u64 cw_corrected_15se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_15se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_15SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_15SE_1(void)
{
	return 0x40290;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_15se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 15se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_15se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_15se_2_s {
		u64 cw_corrected_15se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_15se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_15SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_15SE_2(void)
{
	return 0x40350;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_15se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 15se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_15se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_15se_3_s {
		u64 cw_corrected_15se                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_15se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_15SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_15SE_3(void)
{
	return 0x40410;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_1se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 1se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_1se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_1se_0_s {
		u64 cw_corrected_1se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_1se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_1SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_1SE_0(void)
{
	return 0x40160;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_1se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 1se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_1se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_1se_1_s {
		u64 cw_corrected_1se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_1se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_1SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_1SE_1(void)
{
	return 0x40220;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_1se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 1se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_1se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_1se_2_s {
		u64 cw_corrected_1se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_1se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_1SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_1SE_2(void)
{
	return 0x402e0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_1se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 1se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_1se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_1se_3_s {
		u64 cw_corrected_1se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_1se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_1SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_1SE_3(void)
{
	return 0x403a0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_2se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 2se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_2se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_2se_0_s {
		u64 cw_corrected_2se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_2se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_2SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_2SE_0(void)
{
	return 0x40168;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_2se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 2se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_2se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_2se_1_s {
		u64 cw_corrected_2se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_2se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_2SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_2SE_1(void)
{
	return 0x40228;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_2se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 2se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_2se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_2se_2_s {
		u64 cw_corrected_2se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_2se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_2SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_2SE_2(void)
{
	return 0x402e8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_2se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 2se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_2se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_2se_3_s {
		u64 cw_corrected_2se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_2se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_2SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_2SE_3(void)
{
	return 0x403a8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_3se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 3se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_3se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_3se_0_s {
		u64 cw_corrected_3se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_3se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_3SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_3SE_0(void)
{
	return 0x40170;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_3se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 3se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_3se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_3se_1_s {
		u64 cw_corrected_3se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_3se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_3SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_3SE_1(void)
{
	return 0x40230;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_3se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 3se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_3se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_3se_2_s {
		u64 cw_corrected_3se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_3se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_3SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_3SE_2(void)
{
	return 0x402f0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_3se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 3se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_3se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_3se_3_s {
		u64 cw_corrected_3se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_3se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_3SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_3SE_3(void)
{
	return 0x403b0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_4se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 4se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_4se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_4se_0_s {
		u64 cw_corrected_4se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_4se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_4SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_4SE_0(void)
{
	return 0x40178;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_4se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 4se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_4se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_4se_1_s {
		u64 cw_corrected_4se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_4se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_4SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_4SE_1(void)
{
	return 0x40238;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_4se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 4se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_4se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_4se_2_s {
		u64 cw_corrected_4se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_4se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_4SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_4SE_2(void)
{
	return 0x402f8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_4se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 4se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_4se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_4se_3_s {
		u64 cw_corrected_4se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_4se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_4SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_4SE_3(void)
{
	return 0x403b8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_5se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 5se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_5se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_5se_0_s {
		u64 cw_corrected_5se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_5se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_5SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_5SE_0(void)
{
	return 0x40180;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_5se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 5se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_5se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_5se_1_s {
		u64 cw_corrected_5se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_5se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_5SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_5SE_1(void)
{
	return 0x40240;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_5se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 5se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_5se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_5se_2_s {
		u64 cw_corrected_5se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_5se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_5SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_5SE_2(void)
{
	return 0x40300;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_5se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 5se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_5se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_5se_3_s {
		u64 cw_corrected_5se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_5se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_5SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_5SE_3(void)
{
	return 0x403c0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_6se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 6se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_6se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_6se_0_s {
		u64 cw_corrected_6se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_6se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_6SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_6SE_0(void)
{
	return 0x40188;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_6se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 6se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_6se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_6se_1_s {
		u64 cw_corrected_6se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_6se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_6SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_6SE_1(void)
{
	return 0x40248;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_6se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 6se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_6se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_6se_2_s {
		u64 cw_corrected_6se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_6se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_6SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_6SE_2(void)
{
	return 0x40308;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_6se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 6se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_6se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_6se_3_s {
		u64 cw_corrected_6se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_6se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_6SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_6SE_3(void)
{
	return 0x403c8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_7se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 7se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_7se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_7se_0_s {
		u64 cw_corrected_7se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_7se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_7SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_7SE_0(void)
{
	return 0x40190;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_7se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 7se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_7se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_7se_1_s {
		u64 cw_corrected_7se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_7se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_7SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_7SE_1(void)
{
	return 0x40250;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_7se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 7se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_7se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_7se_2_s {
		u64 cw_corrected_7se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_7se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_7SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_7SE_2(void)
{
	return 0x40310;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_7se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 7se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_7se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_7se_3_s {
		u64 cw_corrected_7se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_7se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_7SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_7SE_3(void)
{
	return 0x403d0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_8se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 8se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_8se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_8se_0_s {
		u64 cw_corrected_8se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_8se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_8SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_8SE_0(void)
{
	return 0x40198;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_8se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 8se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_8se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_8se_1_s {
		u64 cw_corrected_8se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_8se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_8SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_8SE_1(void)
{
	return 0x40258;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_8se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 8se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_8se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_8se_2_s {
		u64 cw_corrected_8se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_8se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_8SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_8SE_2(void)
{
	return 0x40318;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_8se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 8se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_8se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_8se_3_s {
		u64 cw_corrected_8se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_8se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_8SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_8SE_3(void)
{
	return 0x403d8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_9se_0
 *
 * RPM Mti Rsfec Stat Codewords Corrected 9se 0 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_9se_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_9se_0_s {
		u64 cw_corrected_9se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_9se_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_9SE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_9SE_0(void)
{
	return 0x401a0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_9se_1
 *
 * RPM Mti Rsfec Stat Codewords Corrected 9se 1 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_9se_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_9se_1_s {
		u64 cw_corrected_9se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_9se_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_9SE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_9SE_1(void)
{
	return 0x40260;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_9se_2
 *
 * RPM Mti Rsfec Stat Codewords Corrected 9se 2 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_9se_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_9se_2_s {
		u64 cw_corrected_9se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_9se_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_9SE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_9SE_2(void)
{
	return 0x40320;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_codewords_corrected_9se_3
 *
 * RPM Mti Rsfec Stat Codewords Corrected 9se 3 Register
 */
union rpmx_mti_rsfec_stat_codewords_corrected_9se_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_codewords_corrected_9se_3_s {
		u64 cw_corrected_9se                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_codewords_corrected_9se_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_9SE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_CODEWORDS_CORRECTED_9SE_3(void)
{
	return 0x403e0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_0
 *
 * RPM Mti Rsfec Stat Counter Capture 0 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_0_s {
		u64 counter_low_0                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_0(void)
{
	return 0x40040;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_1
 *
 * RPM Mti Rsfec Stat Counter Capture 1 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_1_s {
		u64 counter_low_1                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_1(void)
{
	return 0x40048;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_10
 *
 * RPM Mti Rsfec Stat Counter Capture 10 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_10 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_10_s {
		u64 counter_low_10                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_10_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_10(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_10(void)
{
	return 0x40090;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_11
 *
 * RPM Mti Rsfec Stat Counter Capture 11 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_11 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_11_s {
		u64 counter_low_11                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_11_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_11(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_11(void)
{
	return 0x40098;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_12
 *
 * RPM Mti Rsfec Stat Counter Capture 12 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_12 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_12_s {
		u64 counter_low_12                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_12_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_12(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_12(void)
{
	return 0x400a0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_13
 *
 * RPM Mti Rsfec Stat Counter Capture 13 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_13 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_13_s {
		u64 counter_low_13                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_13_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_13(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_13(void)
{
	return 0x400a8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_14
 *
 * RPM Mti Rsfec Stat Counter Capture 14 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_14 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_14_s {
		u64 counter_low_14                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_14_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_14(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_14(void)
{
	return 0x400b0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_15
 *
 * RPM Mti Rsfec Stat Counter Capture 15 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_15 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_15_s {
		u64 counter_low_15                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_15_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_15(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_15(void)
{
	return 0x400b8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_16
 *
 * RPM Mti Rsfec Stat Counter Capture 16 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_16 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_16_s {
		u64 counter_low_16                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_16_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_16(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_16(void)
{
	return 0x400c0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_17
 *
 * RPM Mti Rsfec Stat Counter Capture 17 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_17 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_17_s {
		u64 counter_low_17                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_17_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_17(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_17(void)
{
	return 0x400c8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_18
 *
 * RPM Mti Rsfec Stat Counter Capture 18 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_18 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_18_s {
		u64 counter_low_18                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_18_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_18(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_18(void)
{
	return 0x400d0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_19
 *
 * RPM Mti Rsfec Stat Counter Capture 19 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_19 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_19_s {
		u64 counter_low_19                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_19_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_19(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_19(void)
{
	return 0x400d8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_2
 *
 * RPM Mti Rsfec Stat Counter Capture 2 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_2_s {
		u64 counter_low_2                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_2(void)
{
	return 0x40050;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_3
 *
 * RPM Mti Rsfec Stat Counter Capture 3 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_3_s {
		u64 counter_low_3                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_3(void)
{
	return 0x40058;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_4
 *
 * RPM Mti Rsfec Stat Counter Capture 4 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_4 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_4_s {
		u64 counter_low_4                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_4_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_4(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_4(void)
{
	return 0x40060;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_5
 *
 * RPM Mti Rsfec Stat Counter Capture 5 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_5 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_5_s {
		u64 counter_low_5                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_5_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_5(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_5(void)
{
	return 0x40068;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_6
 *
 * RPM Mti Rsfec Stat Counter Capture 6 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_6 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_6_s {
		u64 counter_low_6                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_6_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_6(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_6(void)
{
	return 0x40070;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_7
 *
 * RPM Mti Rsfec Stat Counter Capture 7 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_7 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_7_s {
		u64 counter_low_7                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_7_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_7(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_7(void)
{
	return 0x40078;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_8
 *
 * RPM Mti Rsfec Stat Counter Capture 8 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_8 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_8_s {
		u64 counter_low_8                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_8_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_8(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_8(void)
{
	return 0x40080;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_counter_capture_9
 *
 * RPM Mti Rsfec Stat Counter Capture 9 Register
 */
union rpmx_mti_rsfec_stat_counter_capture_9 {
	u64 u;
	struct rpmx_mti_rsfec_stat_counter_capture_9_s {
		u64 counter_low_9                    : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_counter_capture_9_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_9(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_COUNTER_CAPTURE_9(void)
{
	return 0x40088;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_fast_data_hi
 *
 * RPM Mti Rsfec Stat Fast Data Hi Register
 */
union rpmx_mti_rsfec_stat_fast_data_hi {
	u64 u;
	struct rpmx_mti_rsfec_stat_fast_data_hi_s {
		u64 latched_counter_hi_fast          : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_fast_data_hi_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_FAST_DATA_HI(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_FAST_DATA_HI(void)
{
	return 0x40000;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_slow_data_hi
 *
 * RPM Mti Rsfec Stat Slow Data Hi Register
 */
union rpmx_mti_rsfec_stat_slow_data_hi {
	u64 u;
	struct rpmx_mti_rsfec_stat_slow_data_hi_s {
		u64 latched_data_hi_slow             : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_slow_data_hi_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SLOW_DATA_HI(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SLOW_DATA_HI(void)
{
	return 0x40038;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_statn_clearvalue_hi
 *
 * RPM Mti Rsfec Stat Statn Clearvalue Hi Register
 */
union rpmx_mti_rsfec_stat_statn_clearvalue_hi {
	u64 u;
	struct rpmx_mti_rsfec_stat_statn_clearvalue_hi_s {
		u64 statm_high_32b                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_statn_clearvalue_hi_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_STATN_CLEARVALUE_HI(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_STATN_CLEARVALUE_HI(void)
{
	return 0x40028;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_statn_clearvalue_lo
 *
 * RPM Mti Rsfec Stat Statn Clearvalue Lo Register
 */
union rpmx_mti_rsfec_stat_statn_clearvalue_lo {
	u64 u;
	struct rpmx_mti_rsfec_stat_statn_clearvalue_lo_s {
		u64 statm_lower_32b                  : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_statn_clearvalue_lo_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_STATN_CLEARVALUE_LO(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_STATN_CLEARVALUE_LO(void)
{
	return 0x40020;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_statn_config
 *
 * RPM Mti Rsfec Stat Statn Config Register
 */
union rpmx_mti_rsfec_stat_statn_config {
	u64 u;
	struct rpmx_mti_rsfec_stat_statn_config_s {
		u64 saturate                         : 1;
		u64 clear_on_read                    : 1;
		u64 reserved_2_30                    : 29;
		u64 soft_reset                       : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_statn_config_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_STATN_CONFIG(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_STATN_CONFIG(void)
{
	return 0x40010;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_statn_control
 *
 * RPM Mti Rsfec Stat Statn Control Register
 */
union rpmx_mti_rsfec_stat_statn_control {
	u64 u;
	struct rpmx_mti_rsfec_stat_statn_control_s {
		u64 chanmask                         : 8;
		u64 reserved_8_27                    : 20;
		u64 cmd_capture                      : 1;
		u64 clear_pre                        : 1;
		u64 reserved_30                      : 1;
		u64 cmd_clear                        : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_statn_control_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_STATN_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_STATN_CONTROL(void)
{
	return 0x40018;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_statn_status
 *
 * RPM Mti Rsfec Stat Statn Status Register
 */
union rpmx_mti_rsfec_stat_statn_status {
	u64 u;
	struct rpmx_mti_rsfec_stat_statn_status_s {
		u64 stat_module_busy                 : 1;
		u64 reserved_1_63                    : 63;
	} s;
	/* struct rpmx_mti_rsfec_stat_statn_status_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_STATN_STATUS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_STATN_STATUS(void)
{
	return 0x40008;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane0_0
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane0 0 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_0_s {
		u64 se_corrected_lane0               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE0_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE0_0(void)
{
	return 0x401d8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane0_1
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane0 1 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_1_s {
		u64 se_corrected_lane0               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE0_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE0_1(void)
{
	return 0x40298;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane0_2
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane0 2 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_2_s {
		u64 se_corrected_lane0               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE0_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE0_2(void)
{
	return 0x40358;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane0_3
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane0 3 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_3_s {
		u64 se_corrected_lane0               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane0_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE0_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE0_3(void)
{
	return 0x40418;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane1_0
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane1 0 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_0_s {
		u64 se_corrected_lane1               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE1_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE1_0(void)
{
	return 0x401e0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane1_1
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane1 1 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_1_s {
		u64 se_corrected_lane1               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE1_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE1_1(void)
{
	return 0x402a0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane1_2
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane1 2 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_2_s {
		u64 se_corrected_lane1               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE1_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE1_2(void)
{
	return 0x40360;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane1_3
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane1 3 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_3_s {
		u64 se_corrected_lane1               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane1_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE1_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE1_3(void)
{
	return 0x40420;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane2_0
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane2 0 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_0_s {
		u64 se_corrected_lane2               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE2_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE2_0(void)
{
	return 0x401e8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane2_1
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane2 1 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_1_s {
		u64 se_corrected_lane2               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE2_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE2_1(void)
{
	return 0x402a8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane2_2
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane2 2 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_2_s {
		u64 se_corrected_lane2               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE2_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE2_2(void)
{
	return 0x40368;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane2_3
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane2 3 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_3_s {
		u64 se_corrected_lane2               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane2_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE2_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE2_3(void)
{
	return 0x40428;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane3_0
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane3 0 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_0_s {
		u64 se_corrected_lane3               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE3_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE3_0(void)
{
	return 0x401f0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane3_1
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane3 1 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_1_s {
		u64 se_corrected_lane3               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE3_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE3_1(void)
{
	return 0x402b0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane3_2
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane3 2 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_2_s {
		u64 se_corrected_lane3               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE3_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE3_2(void)
{
	return 0x40370;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_symbol_error_corrected_lane3_3
 *
 * RPM Mti Rsfec Stat Symbol Error Corrected Lane3 3 Register
 */
union rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_3_s {
		u64 se_corrected_lane3               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_symbol_error_corrected_lane3_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE3_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_SYMBOL_ERROR_CORRECTED_LANE3_3(void)
{
	return 0x40430;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_correct_0
 *
 * RPM Mti Rsfec Stat Total Codewords Correct 0 Register .
 */
union rpmx_mti_rsfec_stat_total_codewords_correct_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_correct_0_s {
		u64 total_cw_correct                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_correct_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECT_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECT_0(void)
{
	return 0x40148;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_correct_1
 *
 * RPM Mti Rsfec Stat Total Codewords Correct 1 Register .
 */
union rpmx_mti_rsfec_stat_total_codewords_correct_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_correct_1_s {
		u64 total_cw_correct                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_correct_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECT_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECT_1(void)
{
	return 0x40208;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_correct_2
 *
 * RPM Mti Rsfec Stat Total Codewords Correct 2 Register .
 */
union rpmx_mti_rsfec_stat_total_codewords_correct_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_correct_2_s {
		u64 total_cw_correct                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_correct_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECT_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECT_2(void)
{
	return 0x402c8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_correct_3
 *
 * RPM Mti Rsfec Stat Total Codewords Correct 3 Register .
 */
union rpmx_mti_rsfec_stat_total_codewords_correct_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_correct_3_s {
		u64 total_cw_correct                 : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_correct_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECT_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECT_3(void)
{
	return 0x40388;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_corrected_0
 *
 * RPM Mti Rsfec Stat Total Codewords Corrected 0 Register
 */
union rpmx_mti_rsfec_stat_total_codewords_corrected_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_corrected_0_s {
		u64 total_cw_corrected               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_corrected_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECTED_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECTED_0(void)
{
	return 0x40150;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_corrected_1
 *
 * RPM Mti Rsfec Stat Total Codewords Corrected 1 Register
 */
union rpmx_mti_rsfec_stat_total_codewords_corrected_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_corrected_1_s {
		u64 total_cw_corrected               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_corrected_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECTED_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECTED_1(void)
{
	return 0x40210;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_corrected_2
 *
 * RPM Mti Rsfec Stat Total Codewords Corrected 2 Register
 */
union rpmx_mti_rsfec_stat_total_codewords_corrected_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_corrected_2_s {
		u64 total_cw_corrected               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_corrected_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECTED_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECTED_2(void)
{
	return 0x402d0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_corrected_3
 *
 * RPM Mti Rsfec Stat Total Codewords Corrected 3 Register
 */
union rpmx_mti_rsfec_stat_total_codewords_corrected_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_corrected_3_s {
		u64 total_cw_corrected               : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_corrected_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECTED_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_CORRECTED_3(void)
{
	return 0x40390;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_received_0
 *
 * RPM Mti Rsfec Stat Total Codewords Received 0 Register .
 */
union rpmx_mti_rsfec_stat_total_codewords_received_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_received_0_s {
		u64 total_cw_received                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_received_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_RECEIVED_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_RECEIVED_0(void)
{
	return 0x40140;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_received_1
 *
 * RPM Mti Rsfec Stat Total Codewords Received 1 Register .
 */
union rpmx_mti_rsfec_stat_total_codewords_received_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_received_1_s {
		u64 total_cw_received                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_received_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_RECEIVED_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_RECEIVED_1(void)
{
	return 0x40200;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_received_2
 *
 * RPM Mti Rsfec Stat Total Codewords Received 2 Register .
 */
union rpmx_mti_rsfec_stat_total_codewords_received_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_received_2_s {
		u64 total_cw_received                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_received_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_RECEIVED_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_RECEIVED_2(void)
{
	return 0x402c0;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_received_3
 *
 * RPM Mti Rsfec Stat Total Codewords Received 3 Register .
 */
union rpmx_mti_rsfec_stat_total_codewords_received_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_received_3_s {
		u64 total_cw_received                : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_received_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_RECEIVED_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_RECEIVED_3(void)
{
	return 0x40380;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_uncorrectable_0
 *
 * RPM Mti Rsfec Stat Total Codewords Uncorrectable 0 Register
 */
union rpmx_mti_rsfec_stat_total_codewords_uncorrectable_0 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_uncorrectable_0_s {
		u64 total_cw_uncorrectable           : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_uncorrectable_0_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_UNCORRECTABLE_0(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_UNCORRECTABLE_0(void)
{
	return 0x40158;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_uncorrectable_1
 *
 * RPM Mti Rsfec Stat Total Codewords Uncorrectable 1 Register
 */
union rpmx_mti_rsfec_stat_total_codewords_uncorrectable_1 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_uncorrectable_1_s {
		u64 total_cw_uncorrectable           : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_uncorrectable_1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_UNCORRECTABLE_1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_UNCORRECTABLE_1(void)
{
	return 0x40218;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_uncorrectable_2
 *
 * RPM Mti Rsfec Stat Total Codewords Uncorrectable 2 Register
 */
union rpmx_mti_rsfec_stat_total_codewords_uncorrectable_2 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_uncorrectable_2_s {
		u64 total_cw_uncorrectable           : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_uncorrectable_2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_UNCORRECTABLE_2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_UNCORRECTABLE_2(void)
{
	return 0x402d8;
}

/**
 * Register (RSL) rpm#_mti_rsfec_stat_total_codewords_uncorrectable_3
 *
 * RPM Mti Rsfec Stat Total Codewords Uncorrectable 3 Register
 */
union rpmx_mti_rsfec_stat_total_codewords_uncorrectable_3 {
	u64 u;
	struct rpmx_mti_rsfec_stat_total_codewords_uncorrectable_3_s {
		u64 total_cw_uncorrectable           : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_rsfec_stat_total_codewords_uncorrectable_3_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_UNCORRECTABLE_3(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STAT_TOTAL_CODEWORDS_UNCORRECTABLE_3(void)
{
	return 0x40398;
}

/**
 * Register (RSL) rpm#_mti_rsfec_status#
 *
 * RPM MTI RSFEC Status Register RS-FEC Status register.
 */
union rpmx_mti_rsfec_statusx {
	u64 u;
	struct rpmx_mti_rsfec_statusx_s {
		u64 bypass_correction_ability        : 1;
		u64 bypass_indication_ability        : 1;
		u64 high_ser                         : 1;
		u64 degrade_ser_ability              : 1;
		u64 degrade_ser                      : 1;
		u64 rx_am_sf2                        : 1;
		u64 rx_am_sf1                        : 1;
		u64 rx_am_sf0                        : 1;
		u64 amps_lock                        : 4;
		u64 reserved_12_13                   : 2;
		u64 fec_align_status                 : 1;
		u64 reserved_15_63                   : 49;
	} s;
	/* struct rpmx_mti_rsfec_statusx_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_STATUSX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_STATUSX(u64 a)
{
	return 0x38008 + 0x40 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_symblerr_hi#
 *
 * RPM MTI RSFEC Symbol Error High Register Upper 16 bits of counter
 * (latched value after read of _LO).
 */
union rpmx_mti_rsfec_symblerr_hix {
	u64 u;
	struct rpmx_mti_rsfec_symblerr_hix_s {
		u64 symblerr_hi                      : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_rsfec_symblerr_hix_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_SYMBLERR_HIX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_SYMBLERR_HIX(u64 a)
{
	return 0x38408 + 0x10 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_symblerr_lo#
 *
 * RPM MTI RSFEC Symbol Error Low Register Sum of (corrected) 10-bit
 * symbol errors for a lane. Increments only for correctable codewords.
 * Lower 16 bit of counter. Must be read first.
 */
union rpmx_mti_rsfec_symblerr_lox {
	u64 u;
	struct rpmx_mti_rsfec_symblerr_lox_s {
		u64 symblerr_lo                      : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_rsfec_symblerr_lox_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_SYMBLERR_LOX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_SYMBLERR_LOX(u64 a)
{
	return 0x38400 + 0x10 * a;
}

/**
 * Register (RSL) rpm#_mti_rsfec_vendor_align_status
 *
 * RPM MTI RSFEC Vendor Align Status Register Convenience register
 * providing the status bit from each channel's RSFEC Status register bit
 * 14.
 */
union rpmx_mti_rsfec_vendor_align_status {
	u64 u;
	struct rpmx_mti_rsfec_vendor_align_status_s {
		u64 rsfec_vendor_align_status        : 10;
		u64 reserved_10_63                   : 54;
	} s;
	/* struct rpmx_mti_rsfec_vendor_align_status_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_VENDOR_ALIGN_STATUS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_VENDOR_ALIGN_STATUS(void)
{
	return 0x38520;
}

/**
 * Register (RSL) rpm#_mti_rsfec_vendor_control
 *
 * INTERNAL: Not Implemented. RPM MTI RSFEC Vendor Control Register
 * Implementation specific core configuration.
 */
union rpmx_mti_rsfec_vendor_control {
	u64 u;
	struct rpmx_mti_rsfec_vendor_control_s {
		u64 reserved_0_63                    : 64;
	} s;
	/* struct rpmx_mti_rsfec_vendor_control_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_VENDOR_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_VENDOR_CONTROL(void)
{
	return 0x38500;
}

/**
 * Register (RSL) rpm#_mti_rsfec_vendor_info1
 *
 * INTERNAL: Relevant for 200/400G only. RPM MTI RSFEC Vendor Info 1
 * Register  Implementation specific information that may be useful for
 * debugging link problems; Clears on read.
 */
union rpmx_mti_rsfec_vendor_info1 {
	u64 u;
	struct rpmx_mti_rsfec_vendor_info1_s {
		u64 vendor_amps_lock                 : 1;
		u64 reserved_1_3                     : 3;
		u64 vendor_align_status_lh           : 1;
		u64 vendor_marker_check_restart      : 1;
		u64 reserved_6_9                     : 4;
		u64 vendor_align_status_ll           : 1;
		u64 reserved_11_63                   : 53;
	} s;
	/* struct rpmx_mti_rsfec_vendor_info1_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_VENDOR_INFO1(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_VENDOR_INFO1(void)
{
	return 0x38508;
}

/**
 * Register (RSL) rpm#_mti_rsfec_vendor_info2
 *
 * RPM MTI RSFEC Vendor Info 2 Register Implementation specific status
 * information; Clears on read.
 */
union rpmx_mti_rsfec_vendor_info2 {
	u64 u;
	struct rpmx_mti_rsfec_vendor_info2_s {
		u64 amps_lock_lanes                  : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_rsfec_vendor_info2_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_VENDOR_INFO2(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_VENDOR_INFO2(void)
{
	return 0x38510;
}

/**
 * Register (RSL) rpm#_mti_rsfec_vendor_revision
 *
 * RPM MTI RSFEC Vendor Revision Register A version information taken
 * from package file parameter FEC91_DEV_VERSION.
 */
union rpmx_mti_rsfec_vendor_revision {
	u64 u;
	struct rpmx_mti_rsfec_vendor_revision_s {
		u64 revision                         : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_mti_rsfec_vendor_revision_s cn; */
};

static inline u64 RPMX_MTI_RSFEC_VENDOR_REVISION(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_RSFEC_VENDOR_REVISION(void)
{
	return 0x38518;
}

/**
 * Register (RSL) rpm#_mti_stat_captured_page_counter#
 *
 * RPM MTI Statistics Captured Page Counter Register Set of registers
 * that contain all the RX or TX counters of a specific port, upon
 * capture command. When TX statistics are captured only 34 entries
 * contain the snapshot values. When RX statistics are captured only 43
 * entries contain the snapshot values. These registers contain only the
 * 32 lower bits of each counter. Reading a captured counter will latch
 * its 32 upper bits in RPM_MTI_STAT_DATA_HI_CDC register.
 */
union rpmx_mti_stat_captured_page_counterx {
	u64 u;
	struct rpmx_mti_stat_captured_page_counterx_s {
		u64 counter_low                      : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_stat_captured_page_counterx_s cn; */
};

static inline u64 RPMX_MTI_STAT_CAPTURED_PAGE_COUNTERX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_STAT_CAPTURED_PAGE_COUNTERX(u64 a)
{
	return 0x11000 + 8 * a;
}

/**
 * Register (RSL) rpm#_mti_stat_data_hi
 *
 * INTERNAL: RPM MTI Statistics Data HI Register  Not functional.
 */
union rpmx_mti_stat_data_hi {
	u64 u;
	struct rpmx_mti_stat_data_hi_s {
		u64 data_hi                          : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_stat_data_hi_s cn; */
};

static inline u64 RPMX_MTI_STAT_DATA_HI(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_STAT_DATA_HI(void)
{
	return 0x10000;
}

/**
 * Register (RSL) rpm#_mti_stat_data_hi_cdc
 *
 * RPM MTI Statistics Data HI Register The upper 32-bit of the 64-Bit
 * counter value latched from last counter read.
 */
union rpmx_mti_stat_data_hi_cdc {
	u64 u;
	struct rpmx_mti_stat_data_hi_cdc_s {
		u64 data_hi_cdc                      : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_stat_data_hi_cdc_s cn; */
};

static inline u64 RPMX_MTI_STAT_DATA_HI_CDC(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_STAT_DATA_HI_CDC(void)
{
	return 0x10038;
}

/**
 * Register (RSL) rpm#_mti_stat_rx_stat_pages_counter#
 *
 * RPM MTI Statistics RX Statistics Pages Register Per-port RX statistics
 * pages. 43 counters per page.
 */
union rpmx_mti_stat_rx_stat_pages_counterx {
	u64 u;
	struct rpmx_mti_stat_rx_stat_pages_counterx_s {
		u64 counter_low                      : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_stat_rx_stat_pages_counterx_s cn; */
};

static inline u64 RPMX_MTI_STAT_RX_STAT_PAGES_COUNTERX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_STAT_RX_STAT_PAGES_COUNTERX(u64 a)
{
	return 0x12000 + 8 * a;
}

/**
 * Register (RSL) rpm#_mti_stat_statn_clearvalue_hi
 *
 * RPM MTI Statistics Clear Value HI Register Upper 32bit of 64bit value
 * written into statistics memory when a clear command is triggered, or
 * clear-after-read is used.
 */
union rpmx_mti_stat_statn_clearvalue_hi {
	u64 u;
	struct rpmx_mti_stat_statn_clearvalue_hi_s {
		u64 statn_clearvalue_hi              : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_stat_statn_clearvalue_hi_s cn; */
};

static inline u64 RPMX_MTI_STAT_STATN_CLEARVALUE_HI(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_STAT_STATN_CLEARVALUE_HI(void)
{
	return 0x10028;
}

/**
 * Register (RSL) rpm#_mti_stat_statn_clearvalue_lo
 *
 * RPM MTI Statistics Clear Value LO Register Lower 32bit of 64bit value
 * written into statistics memory when a clear command is triggered, or
 * clear-after-read is used.
 */
union rpmx_mti_stat_statn_clearvalue_lo {
	u64 u;
	struct rpmx_mti_stat_statn_clearvalue_lo_s {
		u64 statn_clearvalue_lo              : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_stat_statn_clearvalue_lo_s cn; */
};

static inline u64 RPMX_MTI_STAT_STATN_CLEARVALUE_LO(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_STAT_STATN_CLEARVALUE_LO(void)
{
	return 0x10020;
}

/**
 * Register (RSL) rpm#_mti_stat_statn_config
 *
 * RPM MTI Statistics Config Register Configure Statistics Module
 * Functions (saturate/wraparound, ...)
 */
union rpmx_mti_stat_statn_config {
	u64 u;
	struct rpmx_mti_stat_statn_config_s {
		u64 saturate                         : 1;
		u64 clear_on_read                    : 1;
		u64 reserved_2_30                    : 29;
		u64 reset                            : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_stat_statn_config_s cn; */
};

static inline u64 RPMX_MTI_STAT_STATN_CONFIG(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_STAT_STATN_CONFIG(void)
{
	return 0x10010;
}

/**
 * Register (RSL) rpm#_mti_stat_statn_control
 *
 * RPM MTI Statistics Control Register Control commands to the module for
 * clearing, latching statistics
 */
union rpmx_mti_stat_statn_control {
	u64 u;
	struct rpmx_mti_stat_statn_control_s {
		u64 portmask                         : 8;
		u64 reserved_8_26                    : 19;
		u64 cmd_capture_rx                   : 1;
		u64 cmd_capture_tx                   : 1;
		u64 clear_pre                        : 1;
		u64 cmd_clear_rx                     : 1;
		u64 cmd_clear_tx                     : 1;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_stat_statn_control_s cn; */
};

static inline u64 RPMX_MTI_STAT_STATN_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_STAT_STATN_CONTROL(void)
{
	return 0x10018;
}

/**
 * Register (RSL) rpm#_mti_stat_statn_status
 *
 * RPM MTI Statistics Status Register Informal busy indicators providing
 * a real-time indication that there is ongoing activity in a module.
 * When a bit is 0 no counting activities do currently happen in that
 * module.
 */
union rpmx_mti_stat_statn_status {
	u64 u;
	struct rpmx_mti_stat_statn_status_s {
		u64 rx_statistics_busy               : 1;
		u64 tx_statistics_busy               : 1;
		u64 reserved_2_63                    : 62;
	} s;
	/* struct rpmx_mti_stat_statn_status_s cn; */
};

static inline u64 RPMX_MTI_STAT_STATN_STATUS(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_STAT_STATN_STATUS(void)
{
	return 0x10008;
}

/**
 * Register (RSL) rpm#_mti_stat_tx_stat_pages_counter#
 *
 * RPM MTI Statistics TX Statistics Pages Register Per-port TX statistics
 * pages. 34 counters per page.
 */
union rpmx_mti_stat_tx_stat_pages_counterx {
	u64 u;
	struct rpmx_mti_stat_tx_stat_pages_counterx_s {
		u64 counter_low                      : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_mti_stat_tx_stat_pages_counterx_s cn; */
};

static inline u64 RPMX_MTI_STAT_TX_STAT_PAGES_COUNTERX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_MTI_STAT_TX_STAT_PAGES_COUNTERX(u64 a)
{
	return 0x13000 + 8 * a;
}

/**
 * Register (RSL) rpm#_spu#_an_adv
 *
 * RPM SPU Autonegotiation Advertisement Registers Software programs this
 * register with the contents of the AN-link code word base page to be
 * transmitted during autonegotiation. (See IEEE 802.3 section 73.6 for
 * details.) Any write operations to this register prior to completion of
 * autonegotiation, as indicated by RPM()_SPU()_AN_STATUS[AN_COMPLETE],
 * should be followed by a renegotiation in order for the new values to
 * take effect. Renegotiation is initiated by setting
 * RPM()_SPU()_AN_CONTROL[AN_RESTART]. Once autonegotiation has
 * completed, software can examine this register along with
 * RPM()_SPU()_AN_LP_BASE to determine the highest common denominator
 * technology.
 */
union rpmx_spux_an_adv {
	u64 u;
	struct rpmx_spux_an_adv_s {
		u64 s                                : 5;
		u64 e                                : 5;
		u64 pause                            : 1;
		u64 asm_dir                          : 1;
		u64 xnp_able                         : 1;
		u64 rf                               : 1;
		u64 ack                              : 1;
		u64 np                               : 1;
		u64 t                                : 5;
		u64 a1g_kx                           : 1;
		u64 a10g_kx4                         : 1;
		u64 a10g_kr                          : 1;
		u64 a40g_kr4                         : 1;
		u64 a40g_cr4                         : 1;
		u64 a100g_cr10                       : 1;
		u64 a100g_kp4                        : 1;
		u64 a100g_kr4                        : 1;
		u64 a100g_cr4                        : 1;
		u64 a25g_krs_crs                     : 1;
		u64 a25g_kr_cr                       : 1;
		u64 arsv                             : 12;
		u64 a25g_rs_fec_req                  : 1;
		u64 a25g_br_fec_req                  : 1;
		u64 fec_able                         : 1;
		u64 fec_req                          : 1;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_spux_an_adv_s cn; */
};

static inline u64 RPMX_SPUX_AN_ADV(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_AN_ADV(u64 a)
{
	return 0xd0198 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_an_bp_status
 *
 * RPM SPU Autonegotiation Backplane Ethernet & BASE-R Copper Status
 * Registers The contents of this register are updated during
 * autonegotiation and are valid when RPM()_SPU()_AN_STATUS[AN_COMPLETE]
 * is set. At that time, one of the port type bits will be set depending
 * on the AN priority resolution. The port types are listed in order of
 * decreasing priority. If a BASE-R type is negotiated then [FEC] or
 * [RS_FEC] will be set to indicate whether/which FEC operation has been
 * negotiated and will be clear otherwise.
 */
union rpmx_spux_an_bp_status {
	u64 u;
	struct rpmx_spux_an_bp_status_s {
		u64 bp_an_able                       : 1;
		u64 n1g_kx                           : 1;
		u64 n10g_kx4                         : 1;
		u64 n10g_kr                          : 1;
		u64 n25g_kr1                         : 1;
		u64 n25g_cr1                         : 1;
		u64 n25g_krs_crs                     : 1;
		u64 n25g_kr_cr                       : 1;
		u64 n40g_kr4                         : 1;
		u64 n40g_cr4                         : 1;
		u64 n50g_kr2                         : 1;
		u64 n50g_cr2                         : 1;
		u64 n100g_cr10                       : 1;
		u64 n100g_kp4                        : 1;
		u64 n100g_kr4                        : 1;
		u64 n100g_cr4                        : 1;
		u64 fec                              : 1;
		u64 rs_fec                           : 1;
		u64 reserved_18_63                   : 46;
	} s;
	/* struct rpmx_spux_an_bp_status_s cn; */
};

static inline u64 RPMX_SPUX_AN_BP_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_AN_BP_STATUS(u64 a)
{
	return 0xd01b8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_an_control
 *
 * RPM SPU Autonegotiation Control Registers
 */
union rpmx_spux_an_control {
	u64 u;
	struct rpmx_spux_an_control_s {
		u64 reserved_0_8                     : 9;
		u64 an_restart                       : 1;
		u64 reserved_10_11                   : 2;
		u64 an_en                            : 1;
		u64 xnp_en                           : 1;
		u64 reserved_14                      : 1;
		u64 an_reset                         : 1;
		u64 an_arb_link_chk_en               : 1;
		u64 usx_an_arb_link_chk_en           : 1;
		u64 reserved_18_63                   : 46;
	} s;
	/* struct rpmx_spux_an_control_s cn; */
};

static inline u64 RPMX_SPUX_AN_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_AN_CONTROL(u64 a)
{
	return 0xd0188 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_an_lp_base
 *
 * RPM SPU Autonegotiation Link-Partner Base-Page Ability Registers This
 * register captures the contents of the latest AN link code word base
 * page received from the link partner during autonegotiation. (See IEEE
 * 802.3 section 73.6 for details.) RPM()_SPU()_AN_STATUS[PAGE_RX] is set
 * when this register is updated by hardware.
 */
union rpmx_spux_an_lp_base {
	u64 u;
	struct rpmx_spux_an_lp_base_s {
		u64 s                                : 5;
		u64 e                                : 5;
		u64 pause                            : 1;
		u64 asm_dir                          : 1;
		u64 xnp_able                         : 1;
		u64 rf                               : 1;
		u64 ack                              : 1;
		u64 np                               : 1;
		u64 t                                : 5;
		u64 a1g_kx                           : 1;
		u64 a10g_kx4                         : 1;
		u64 a10g_kr                          : 1;
		u64 a40g_kr4                         : 1;
		u64 a40g_cr4                         : 1;
		u64 a100g_cr10                       : 1;
		u64 a100g_kp4                        : 1;
		u64 a100g_kr4                        : 1;
		u64 a100g_cr4                        : 1;
		u64 a25g_krs_crs                     : 1;
		u64 a25g_kr_cr                       : 1;
		u64 arsv                             : 12;
		u64 a25g_rs_fec_req                  : 1;
		u64 a25g_br_fec_req                  : 1;
		u64 fec_able                         : 1;
		u64 fec_req                          : 1;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_spux_an_lp_base_s cn; */
};

static inline u64 RPMX_SPUX_AN_LP_BASE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_AN_LP_BASE(u64 a)
{
	return 0xd01a0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_an_lp_xnp
 *
 * RPM SPU Autonegotiation Link Partner Extended Next Page Ability
 * Registers This register captures the contents of the latest next page
 * code word received from the link partner during autonegotiation, if
 * any. See IEEE 802.3 section 73.7.7 for details.
 */
union rpmx_spux_an_lp_xnp {
	u64 u;
	struct rpmx_spux_an_lp_xnp_s {
		u64 m_u                              : 11;
		u64 toggle                           : 1;
		u64 ack2                             : 1;
		u64 mp                               : 1;
		u64 ack                              : 1;
		u64 np                               : 1;
		u64 u                                : 32;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_spux_an_lp_xnp_s cn; */
};

static inline u64 RPMX_SPUX_AN_LP_XNP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_AN_LP_XNP(u64 a)
{
	return 0xd01b0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_an_status
 *
 * RPM SPU Autonegotiation Status Registers
 */
union rpmx_spux_an_status {
	u64 u;
	struct rpmx_spux_an_status_s {
		u64 lp_an_able                       : 1;
		u64 reserved_1                       : 1;
		u64 link_status                      : 1;
		u64 an_able                          : 1;
		u64 rmt_flt                          : 1;
		u64 an_complete                      : 1;
		u64 page_rx                          : 1;
		u64 xnp_stat                         : 1;
		u64 reserved_8                       : 1;
		u64 prl_flt                          : 1;
		u64 reserved_10_63                   : 54;
	} s;
	/* struct rpmx_spux_an_status_s cn; */
};

static inline u64 RPMX_SPUX_AN_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_AN_STATUS(u64 a)
{
	return 0xd0190 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_an_xnp_tx
 *
 * RPM SPU Autonegotiation Extended Next Page Transmit Registers Software
 * programs this register with the contents of the AN message next page
 * or unformatted next page link code word to be transmitted during
 * autonegotiation. Next page exchange occurs after the base link code
 * words have been exchanged if either end of the link segment sets the
 * NP bit to 1, indicating that it has at least one next page to send.
 * Once initiated, next page exchange continues until both ends of the
 * link segment set their NP bits to 0. See IEEE 802.3 section 73.7.7 for
 * details.
 */
union rpmx_spux_an_xnp_tx {
	u64 u;
	struct rpmx_spux_an_xnp_tx_s {
		u64 m_u                              : 11;
		u64 toggle                           : 1;
		u64 ack2                             : 1;
		u64 mp                               : 1;
		u64 ack                              : 1;
		u64 np                               : 1;
		u64 u                                : 32;
		u64 reserved_48_63                   : 16;
	} s;
	/* struct rpmx_spux_an_xnp_tx_s cn; */
};

static inline u64 RPMX_SPUX_AN_XNP_TX(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_AN_XNP_TX(u64 a)
{
	return 0xd01a8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_algn_status
 *
 * RPM SPU Multilane BASE-R PCS Alignment-Status Registers This register
 * implements the IEEE 802.3 multilane BASE-R PCS alignment status 1-4
 * registers (3.50-3.53). It is valid only when the LPCS type is
 * 40GBASE-R, 50GBASE-R, 100GBASE-R, (RPM()_CMR()_CONFIG[LMAC_TYPE] =
 * RPM_LMAC_TYPES_E::FORTYG_R,FIFTYG_R,HUNDREDG_R), and always returns
 * 0x0 for all other LPCS types. Service interfaces (lanes) 19-0 (100G)
 * and 3-0 (all others) are mapped to PCS lanes 19-0 or 3-0 via
 * RPM()_SPU()_BR_LANE_MAP()[LN_MAPPING]. For 100G, logical lane 0 fans
 * out to service interfaces 0-4, logical lane 1 fans out to service
 * interfaces 5-9, ... etc. For all other modes, logical lanes and
 * service interfaces are identical. Logical interfaces (lanes) map to
 * SerDes lanes via RPM()_CMR()_CONFIG[LANE_TO_SDS] (programmable).
 */
union rpmx_spux_br_algn_status {
	u64 u;
	struct rpmx_spux_br_algn_status_s {
		u64 block_lock                       : 20;
		u64 reserved_20_29                   : 10;
		u64 alignd                           : 1;
		u64 reserved_31_40                   : 10;
		u64 marker_lock                      : 20;
		u64 reserved_61_63                   : 3;
	} s;
	/* struct rpmx_spux_br_algn_status_s cn; */
};

static inline u64 RPMX_SPUX_BR_ALGN_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_ALGN_STATUS(u64 a)
{
	return 0xd0050 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_lane_map#
 *
 * RPM SPU 40,50,100GBASE-R Lane-Mapping Registers This register
 * implements the IEEE 802.3 lane 0-19 mapping registers (3.400-3.403).
 * It is valid only when the LPCS type is 40GBASE-R, 50GBASE-R,
 * 100GBASE-R, USXGMII (RPM()_CMR()_CONFIG[LMAC_TYPE]), and always
 * returns 0x0 for all other LPCS types. The LNx_MAPPING field for each
 * programmed PCS lane (called service interface in 802.3) is valid when
 * that lane has achieved alignment marker lock on the receive side (i.e.
 * the associated RPM()_SPU()_BR_ALGN_STATUS[MARKER_LOCK] = 1), and is
 * invalid otherwise. When valid, it returns the actual detected receive
 * PCS lane number based on the received alignment marker contents
 * received on that service interface.  In RS-FEC mode the LNx_MAPPING
 * field is valid when that lane has achieved alignment marker lock on
 * the receive side (i.e. the associated
 * RPM_SPU(0..3)_RSFEC_STATUS[AMPS_LOCK] = 1), and is invalid otherwise.
 * When valid, it returns the actual detected receive FEC lane number
 * based on the received alignment marker contents received on that
 * logical lane therefore expect for RS-FEC that LNx_MAPPING = x.  The
 * mapping is flexible because IEEE 802.3 allows multilane BASE-R receive
 * lanes to be re-ordered. Note that for the transmit side, each logical
 * lane is mapped to a physical SerDes lane based on the programming of
 * RPM()_CMR()_CONFIG[LANE_TO_SDS]. For the receive side,
 * RPM()_CMR()_CONFIG[LANE_TO_SDS] specifies the logical lane to physical
 * SerDes lane mapping, and this register specifies the service interface
 * (or lane) to PCS lane mapping.
 */
union rpmx_spux_br_lane_mapx {
	u64 u;
	struct rpmx_spux_br_lane_mapx_s {
		u64 ln_mapping                       : 6;
		u64 reserved_6_63                    : 58;
	} s;
	/* struct rpmx_spux_br_lane_mapx_s cn; */
};

static inline u64 RPMX_SPUX_BR_LANE_MAPX(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_LANE_MAPX(u64 a, u64 b)
{
	return 0xd0600 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_spu#_br_pmd_control
 *
 * RPM SPU BASE-R PMD Control Registers
 */
union rpmx_spux_br_pmd_control {
	u64 u;
	struct rpmx_spux_br_pmd_control_s {
		u64 train_restart                    : 1;
		u64 train_en                         : 1;
		u64 use_lane_poly                    : 1;
		u64 reserved_3_63                    : 61;
	} s;
	/* struct rpmx_spux_br_pmd_control_s cn; */
};

static inline u64 RPMX_SPUX_BR_PMD_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_PMD_CONTROL(u64 a)
{
	return 0xd00a8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_pmd_ld_cup
 *
 * INTERNAL:RPM SPU BASE-R PMD Local Device Coefficient Update Registers
 * This register implements MDIO register 1.154 of 802.3-2012 Section 5
 * CL45 for 10GBASE-R and and of 802.3by-2016 CL45 for 25GBASE-R. Note
 * that for 10G, 25G LN0_ only is used.  It implements  MDIO registers
 * 1.1300-1.1303 for all other BASE-R modes (40G, 50G, 100G) per
 * 802.3bj-2014 CL45. Note that for 50G LN0_ and LN1_ only are used.  The
 * fields in this register are read/write even though they are specified
 * as read-only in 802.3.  The register is automatically cleared at the
 * start of training. When link training is in progress, each field
 * reflects the contents of the coefficient update field in the
 * associated lane's outgoing training frame.  If
 * RPM()_SPU_DBG_CONTROL[BR_PMD_TRAIN_SOFT_EN] is set, then this register
 * must be updated by software during link training and hardware updates
 * are disabled. If RPM()_SPU_DBG_CONTROL[BR_PMD_TRAIN_SOFT_EN] is clear,
 * this register is automatically updated by hardware, and it should not
 * be written by software. The lane fields in this register are indexed
 * by logical PCS lane ID.
 */
union rpmx_spux_br_pmd_ld_cup {
	u64 u;
	struct rpmx_spux_br_pmd_ld_cup_s {
		u64 ln0_cup                          : 16;
		u64 ln1_cup                          : 16;
		u64 ln2_cup                          : 16;
		u64 ln3_cup                          : 16;
	} s;
	/* struct rpmx_spux_br_pmd_ld_cup_s cn; */
};

static inline u64 RPMX_SPUX_BR_PMD_LD_CUP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_PMD_LD_CUP(u64 a)
{
	return 0xd00c8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_pmd_ld_rep
 *
 * INTERNAL:RPM SPU BASE-R PMD Local Device Status Report Registers  This
 * register implements MDIO register 1.155 of 802.3-2012 Section 5 CL45
 * for 10GBASE-R and and of 802.3by-2016 CL45 for 25GBASE-R. Note that
 * for 10G, 25G LN0_ only is used.  It implements  MDIO registers
 * 1.1400-1.1403 for all other BASE-R modes (40G, 50G, 100G) per
 * 802.3bj-2014 CL45. Note that for 50G LN0_ and LN1_ only are used.  The
 * fields in this register are read/write even though they are specified
 * as read-only in 802.3.  The register is automatically cleared at the
 * start of training. Each field reflects the contents of the status
 * report field in the associated lane's outgoing training frame.  If
 * RPM()_SPU_DBG_CONTROL[BR_PMD_TRAIN_SOFT_EN] is set, then this register
 * must be updated by software during link training and hardware updates
 * are disabled. If RPM()_SPU_DBG_CONTROL[BR_PMD_TRAIN_SOFT_EN] is clear,
 * this register is automatically updated by hardware, and it should not
 * be written by software. The lane fields in this register are indexed
 * by logical PCS lane ID.
 */
union rpmx_spux_br_pmd_ld_rep {
	u64 u;
	struct rpmx_spux_br_pmd_ld_rep_s {
		u64 ln0_rep                          : 16;
		u64 ln1_rep                          : 16;
		u64 ln2_rep                          : 16;
		u64 ln3_rep                          : 16;
	} s;
	/* struct rpmx_spux_br_pmd_ld_rep_s cn; */
};

static inline u64 RPMX_SPUX_BR_PMD_LD_REP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_PMD_LD_REP(u64 a)
{
	return 0xd00d0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_pmd_lp_cup
 *
 * INTERNAL:RPM SPU BASE-R PMD Link Partner Coefficient Update Registers
 * This register implements MDIO register 1.152 of 802.3-2012 Section 5
 * CL45 for 10GBASE-R and and of 802.3by-2016 CL45 for 25GBASE-R. Note
 * that for 10G, 25G LN0_ only is used.  It implements  MDIO registers
 * 1.1100-1.1103 for all other BASE-R modes (40G, 50G, 100G) per
 * 802.3bj-2014 CL45. Note that for 50G LN0_ and LN1_ only are used.  The
 * register is automatically cleared at the start of training. Each field
 * reflects the contents of the coefficient update field in the lane's
 * most recently received training frame. This register should not be
 * written when link training is enabled, i.e. when
 * RPM()_SPU()_BR_PMD_CONTROL[TRAIN_EN] is set. The lane fields in this
 * register are indexed by logical PCS lane ID.
 */
union rpmx_spux_br_pmd_lp_cup {
	u64 u;
	struct rpmx_spux_br_pmd_lp_cup_s {
		u64 ln0_cup                          : 16;
		u64 ln1_cup                          : 16;
		u64 ln2_cup                          : 16;
		u64 ln3_cup                          : 16;
	} s;
	/* struct rpmx_spux_br_pmd_lp_cup_s cn; */
};

static inline u64 RPMX_SPUX_BR_PMD_LP_CUP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_PMD_LP_CUP(u64 a)
{
	return 0xd00b8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_pmd_lp_rep
 *
 * INTERNAL:RPM SPU BASE-R PMD Link Partner Status Report Registers  This
 * register implements MDIO register 1.153 of 802.3-2012 Section 5 CL45
 * for 10GBASE-R and and of 802.3by-2016 CL45 for 25GBASE-R. Note that
 * for 10G, 25G LN0_ only is used.  It implements  MDIO registers
 * 1.1200-1.1203 for all other BASE-R modes (40G, 50G, 100G) per
 * 802.3bj-2014 CL45. Note that for 50G LN0_ and LN1_ only are used.  The
 * register is automatically cleared at the start of training. Each field
 * reflects the contents of the coefficient update field in the lane's
 * most recently received training frame. This register should not be
 * written when link training is enabled, i.e. when
 * RPM()_SPU()_BR_PMD_CONTROL[TRAIN_EN] is set. The lane fields in this
 * register are indexed by logical PCS lane ID.
 */
union rpmx_spux_br_pmd_lp_rep {
	u64 u;
	struct rpmx_spux_br_pmd_lp_rep_s {
		u64 ln0_rep                          : 16;
		u64 ln1_rep                          : 16;
		u64 ln2_rep                          : 16;
		u64 ln3_rep                          : 16;
	} s;
	/* struct rpmx_spux_br_pmd_lp_rep_s cn; */
};

static inline u64 RPMX_SPUX_BR_PMD_LP_REP(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_PMD_LP_REP(u64 a)
{
	return 0xd00c0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_pmd_status
 *
 * INTERNAL:RPM SPU BASE-R PMD Status Registers  The lane fields in this
 * register are indexed by logical PCS lane ID. The lane 0 field (LN0_*)
 * is valid for 10GBASE-R, 25GBASE-R, 40GBASE-R, 50GBASE-R and
 * 100GBASE-R. The lane 1 field (LN1_*) is valid for 40GBASE-R, 50GBASE-R
 * and 100GBASE-R. The remaining fields (LN2_*, LN3_*) are only valid for
 * 40GBASE-R and 100GBASE-R.
 */
union rpmx_spux_br_pmd_status {
	u64 u;
	struct rpmx_spux_br_pmd_status_s {
		u64 ln0_train_status                 : 4;
		u64 ln1_train_status                 : 4;
		u64 ln2_train_status                 : 4;
		u64 ln3_train_status                 : 4;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_spux_br_pmd_status_s cn; */
};

static inline u64 RPMX_SPUX_BR_PMD_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_PMD_STATUS(u64 a)
{
	return 0xd00b0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_status1
 *
 * RPM SPU BASE-R Status 1 Registers
 */
union rpmx_spux_br_status1 {
	u64 u;
	struct rpmx_spux_br_status1_s {
		u64 blk_lock                         : 1;
		u64 hi_ber                           : 1;
		u64 prbs31                           : 1;
		u64 prbs9                            : 1;
		u64 reserved_4_11                    : 8;
		u64 rcv_lnk                          : 1;
		u64 reserved_13_63                   : 51;
	} s;
	/* struct rpmx_spux_br_status1_s cn; */
};

static inline u64 RPMX_SPUX_BR_STATUS1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_STATUS1(u64 a)
{
	return 0xd0030 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_status2
 *
 * RPM SPU BASE-R Status 2 Registers This register implements a
 * combination of the following IEEE 802.3 registers: * BASE-R PCS status
 * 2 (MDIO address 3.33). * BASE-R BER high-order counter (MDIO address
 * 3.44). * Errored-blocks high-order counter (MDIO address 3.45).  Note
 * that the relative locations of some fields have been moved from IEEE
 * 802.3 in order to make the register layout more software friendly: the
 * BER counter high-order and low-order bits from sections 3.44 and 3.33
 * have been combined into the contiguous, 22-bit [BER_CNT] field;
 * likewise, the errored-blocks counter high-order and low-order bits
 * from section 3.45 have been combined into the contiguous, 22-bit
 * [ERR_BLKS] field.
 */
union rpmx_spux_br_status2 {
	u64 u;
	struct rpmx_spux_br_status2_s {
		u64 reserved_0_13                    : 14;
		u64 latched_ber                      : 1;
		u64 latched_lock                     : 1;
		u64 ber_cnt                          : 22;
		u64 reserved_38_39                   : 2;
		u64 err_blks                         : 22;
		u64 reserved_62_63                   : 2;
	} s;
	/* struct rpmx_spux_br_status2_s cn; */
};

static inline u64 RPMX_SPUX_BR_STATUS2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_STATUS2(u64 a)
{
	return 0xd0038 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_tp_control
 *
 * RPM SPU BASE-R Test-Pattern Control Registers Refer to the test
 * pattern methodology described in 802.3 sections 49.2.8 and 82.2.10.
 */
union rpmx_spux_br_tp_control {
	u64 u;
	struct rpmx_spux_br_tp_control_s {
		u64 dp_sel                           : 1;
		u64 tp_sel                           : 1;
		u64 rx_tp_en                         : 1;
		u64 tx_tp_en                         : 1;
		u64 prbs31_tx                        : 1;
		u64 prbs31_rx                        : 1;
		u64 prbs9_tx                         : 1;
		u64 scramble_tp                      : 2;
		u64 pr_tp_data_type                  : 1;
		u64 reserved_10_63                   : 54;
	} s;
	/* struct rpmx_spux_br_tp_control_s cn; */
};

static inline u64 RPMX_SPUX_BR_TP_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_TP_CONTROL(u64 a)
{
	return 0xd0040 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_tp_err_cnt
 *
 * RPM SPU BASE-R Test-Pattern Error-Count Registers This register
 * provides the BASE-R PCS test-pattern error counter.
 */
union rpmx_spux_br_tp_err_cnt {
	u64 u;
	struct rpmx_spux_br_tp_err_cnt_s {
		u64 err_cnt                          : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_spux_br_tp_err_cnt_s cn; */
};

static inline u64 RPMX_SPUX_BR_TP_ERR_CNT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_TP_ERR_CNT(u64 a)
{
	return 0xd0048 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_tp_seed_a
 *
 * RPM SPU BASE-R Test-Pattern Seed A Registers Refer to the test pattern
 * methodology described in 802.3 sections 49.2.8 and 82.2.10.
 */
union rpmx_spux_br_tp_seed_a {
	u64 u;
	struct rpmx_spux_br_tp_seed_a_s {
		u64 tp_seed_a                        : 58;
		u64 reserved_58_63                   : 6;
	} s;
	/* struct rpmx_spux_br_tp_seed_a_s cn; */
};

static inline u64 RPMX_SPUX_BR_TP_SEED_A(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_TP_SEED_A(u64 a)
{
	return 0xd0060 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_br_tp_seed_b
 *
 * RPM SPU BASE-R Test-Pattern Seed B Registers Refer to the test pattern
 * methodology described in 802.3 sections 49.2.8 and 82.2.10.
 */
union rpmx_spux_br_tp_seed_b {
	u64 u;
	struct rpmx_spux_br_tp_seed_b_s {
		u64 tp_seed_b                        : 58;
		u64 reserved_58_63                   : 6;
	} s;
	/* struct rpmx_spux_br_tp_seed_b_s cn; */
};

static inline u64 RPMX_SPUX_BR_TP_SEED_B(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BR_TP_SEED_B(u64 a)
{
	return 0xd0068 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_bx_status
 *
 * RPM SPU BASE-X Status Registers
 */
union rpmx_spux_bx_status {
	u64 u;
	struct rpmx_spux_bx_status_s {
		u64 lsync                            : 4;
		u64 reserved_4_10                    : 7;
		u64 pattst                           : 1;
		u64 alignd                           : 1;
		u64 reserved_13_63                   : 51;
	} s;
	/* struct rpmx_spux_bx_status_s cn; */
};

static inline u64 RPMX_SPUX_BX_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_BX_STATUS(u64 a)
{
	return 0xd0028 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_control1
 *
 * RPM SPU Control 1 Registers
 */
union rpmx_spux_control1 {
	u64 u;
	struct rpmx_spux_control1_s {
		u64 reserved_0_1                     : 2;
		u64 spd                              : 4;
		u64 spdsel0                          : 1;
		u64 reserved_7_10                    : 4;
		u64 lo_pwr                           : 1;
		u64 reserved_12                      : 1;
		u64 spdsel1                          : 1;
		u64 loopbck                          : 1;
		u64 reset                            : 1;
		u64 usxgmii_type                     : 3;
		u64 usxgmii_rate                     : 3;
		u64 disable_am                       : 1;
		u64 reserved_23_63                   : 41;
	} s;
	/* struct rpmx_spux_control1_s cn; */
};

static inline u64 RPMX_SPUX_CONTROL1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_CONTROL1(u64 a)
{
	return 0xd0000 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_control2
 *
 * RPM SPU Control 2 Registers
 */
union rpmx_spux_control2 {
	u64 u;
	struct rpmx_spux_control2_s {
		u64 pcs_type                         : 4;
		u64 reserved_4_63                    : 60;
	} s;
	/* struct rpmx_spux_control2_s cn; */
};

static inline u64 RPMX_SPUX_CONTROL2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_CONTROL2(u64 a)
{
	return 0xd0018 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_fec_abil
 *
 * RPM SPU Forward Error Correction Ability Registers
 */
union rpmx_spux_fec_abil {
	u64 u;
	struct rpmx_spux_fec_abil_s {
		u64 fec_abil                         : 1;
		u64 err_abil                         : 1;
		u64 reserved_2_63                    : 62;
	} s;
	/* struct rpmx_spux_fec_abil_s cn; */
};

static inline u64 RPMX_SPUX_FEC_ABIL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_FEC_ABIL(u64 a)
{
	return 0xd00d8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_fec_control
 *
 * RPM SPU Forward Error Correction Control Registers
 */
union rpmx_spux_fec_control {
	u64 u;
	struct rpmx_spux_fec_control_s {
		u64 fec_en                           : 2;
		u64 err_en                           : 1;
		u64 fec_byp_ind_en                   : 1;
		u64 fec_byp_cor_en                   : 1;
		u64 reserved_5_63                    : 59;
	} s;
	/* struct rpmx_spux_fec_control_s cn; */
};

static inline u64 RPMX_SPUX_FEC_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_FEC_CONTROL(u64 a)
{
	return 0xd00e0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_fec_ln#_rsfec_err
 *
 * RPM SPU Reed-Solomon FEC Symbol Error Counter for FEC Lanes 0-3
 * Registers This register is valid only when Reed-Solomon FEC is
 * enabled. The symbol error counters are defined in 802.3 section
 * 91.6.11 (for 100G and extended to 50G) and 802.3by-2016 section
 * 108.6.9 (for 25G and extended to USXGMII). The counter is reset to all
 * zeros when the register is read, and held at all ones in case of
 * overflow.  The reset operation takes precedence over the increment
 * operation; if the register is read on the same clock cycle as an
 * increment operation, the counter is reset to all zeros and the
 * increment operation is lost. The counters are writable for test
 * purposes, rather than read-only as specified in IEEE 802.3.
 */
union rpmx_spux_fec_lnx_rsfec_err {
	u64 u;
	struct rpmx_spux_fec_lnx_rsfec_err_s {
		u64 symb_err_cnt                     : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_spux_fec_lnx_rsfec_err_s cn; */
};

static inline u64 RPMX_SPUX_FEC_LNX_RSFEC_ERR(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_FEC_LNX_RSFEC_ERR(u64 a, u64 b)
{
	return 0xd0900 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_spu#_int
 *
 * RPM SPU Interrupt Registers
 */
union rpmx_spux_int {
	u64 u;
	struct rpmx_spux_int_s {
		u64 rx_link_up                       : 1;
		u64 rx_link_down                     : 1;
		u64 err_blk                          : 1;
		u64 bitlckls                         : 1;
		u64 synlos                           : 1;
		u64 algnlos                          : 1;
		u64 dbg_sync                         : 1;
		u64 bip_err                          : 1;
		u64 fec_corr                         : 1;
		u64 fec_uncorr                       : 1;
		u64 an_page_rx                       : 1;
		u64 an_link_good                     : 1;
		u64 an_complete                      : 1;
		u64 training_done                    : 1;
		u64 training_failure                 : 1;
		u64 fec_align_status                 : 1;
		u64 rsfec_corr                       : 1;
		u64 rsfec_uncorr                     : 1;
		u64 hi_ser                           : 1;
		u64 usx_an_lnk_st                    : 1;
		u64 usx_an_cpt                       : 1;
		u64 reserved_21_63                   : 43;
	} s;
	/* struct rpmx_spux_int_s cn; */
};

static inline u64 RPMX_SPUX_INT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_INT(u64 a)
{
	return 0xd0220 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_int_ena_w1c
 *
 * RPM SPU Interrupt Enable Clear Registers This register clears
 * interrupt enable bits.
 */
union rpmx_spux_int_ena_w1c {
	u64 u;
	struct rpmx_spux_int_ena_w1c_s {
		u64 rx_link_up                       : 1;
		u64 rx_link_down                     : 1;
		u64 err_blk                          : 1;
		u64 bitlckls                         : 1;
		u64 synlos                           : 1;
		u64 algnlos                          : 1;
		u64 dbg_sync                         : 1;
		u64 bip_err                          : 1;
		u64 fec_corr                         : 1;
		u64 fec_uncorr                       : 1;
		u64 an_page_rx                       : 1;
		u64 an_link_good                     : 1;
		u64 an_complete                      : 1;
		u64 training_done                    : 1;
		u64 training_failure                 : 1;
		u64 fec_align_status                 : 1;
		u64 rsfec_corr                       : 1;
		u64 rsfec_uncorr                     : 1;
		u64 hi_ser                           : 1;
		u64 usx_an_lnk_st                    : 1;
		u64 usx_an_cpt                       : 1;
		u64 reserved_21_63                   : 43;
	} s;
	/* struct rpmx_spux_int_ena_w1c_s cn; */
};

static inline u64 RPMX_SPUX_INT_ENA_W1C(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_INT_ENA_W1C(u64 a)
{
	return 0xd0230 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_int_ena_w1s
 *
 * RPM SPU Interrupt Enable Set Registers This register sets interrupt
 * enable bits.
 */
union rpmx_spux_int_ena_w1s {
	u64 u;
	struct rpmx_spux_int_ena_w1s_s {
		u64 rx_link_up                       : 1;
		u64 rx_link_down                     : 1;
		u64 err_blk                          : 1;
		u64 bitlckls                         : 1;
		u64 synlos                           : 1;
		u64 algnlos                          : 1;
		u64 dbg_sync                         : 1;
		u64 bip_err                          : 1;
		u64 fec_corr                         : 1;
		u64 fec_uncorr                       : 1;
		u64 an_page_rx                       : 1;
		u64 an_link_good                     : 1;
		u64 an_complete                      : 1;
		u64 training_done                    : 1;
		u64 training_failure                 : 1;
		u64 fec_align_status                 : 1;
		u64 rsfec_corr                       : 1;
		u64 rsfec_uncorr                     : 1;
		u64 hi_ser                           : 1;
		u64 usx_an_lnk_st                    : 1;
		u64 usx_an_cpt                       : 1;
		u64 reserved_21_63                   : 43;
	} s;
	/* struct rpmx_spux_int_ena_w1s_s cn; */
};

static inline u64 RPMX_SPUX_INT_ENA_W1S(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_INT_ENA_W1S(u64 a)
{
	return 0xd0238 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_int_w1s
 *
 * RPM SPU Interrupt Set Registers This register sets interrupt bits.
 */
union rpmx_spux_int_w1s {
	u64 u;
	struct rpmx_spux_int_w1s_s {
		u64 rx_link_up                       : 1;
		u64 rx_link_down                     : 1;
		u64 err_blk                          : 1;
		u64 bitlckls                         : 1;
		u64 synlos                           : 1;
		u64 algnlos                          : 1;
		u64 dbg_sync                         : 1;
		u64 bip_err                          : 1;
		u64 fec_corr                         : 1;
		u64 fec_uncorr                       : 1;
		u64 an_page_rx                       : 1;
		u64 an_link_good                     : 1;
		u64 an_complete                      : 1;
		u64 training_done                    : 1;
		u64 training_failure                 : 1;
		u64 fec_align_status                 : 1;
		u64 rsfec_corr                       : 1;
		u64 rsfec_uncorr                     : 1;
		u64 hi_ser                           : 1;
		u64 usx_an_lnk_st                    : 1;
		u64 usx_an_cpt                       : 1;
		u64 reserved_21_63                   : 43;
	} s;
	/* struct rpmx_spux_int_w1s_s cn; */
};

static inline u64 RPMX_SPUX_INT_W1S(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_INT_W1S(u64 a)
{
	return 0xd0228 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_ln#_br_bip_err_cnt
 *
 * RPM SPU 40,50,100GBASE-R BIP Error-Counter Registers This register
 * implements the IEEE 802.3 BIP error-counter registers for PCS lanes
 * 0-19 (3.200-3.203). It is valid only when the LPCS type is 40GBASE-R,
 * 50GBASE-R, 100GBASE-R, (RPM()_CMR()_CONFIG[LMAC_TYPE]), and always
 * returns 0x0 for all other LPCS types. The counters are indexed by the
 * RX PCS lane number based on the alignment marker detected on each lane
 * and captured in RPM()_SPU()_BR_LANE_MAP(). Each counter counts the BIP
 * errors for its PCS lane, and is held at all ones in case of overflow.
 * The counters are reset to all zeros when this register is read by
 * software.  The reset operation takes precedence over the increment
 * operation; if the register is read on the same clock cycle as an
 * increment operation, the counter is reset to all zeros and the
 * increment operation is lost. The counters are writable for test
 * purposes, rather than read-only as specified in IEEE 802.3.
 */
union rpmx_spux_lnx_br_bip_err_cnt {
	u64 u;
	struct rpmx_spux_lnx_br_bip_err_cnt_s {
		u64 bip_err_cnt                      : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_spux_lnx_br_bip_err_cnt_s cn; */
};

static inline u64 RPMX_SPUX_LNX_BR_BIP_ERR_CNT(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_LNX_BR_BIP_ERR_CNT(u64 a, u64 b)
{
	return 0xd0500 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_spu#_ln#_fec_corr_blks
 *
 * RPM SPU FEC Corrected-Blocks Counters 0-19 Registers This register is
 * valid only when the LPCS type is BASE-R
 * (RPM()_CMR()_CONFIG[LMAC_TYPE]) and applies to BASE-R FEC and Reed-
 * Solomon FEC (RS-FEC). When BASE-R FEC is enabled, the FEC corrected-
 * block counters are defined in IEEE 802.3 section 74.8.4.1. Each
 * corrected-blocks counter increments by one for a corrected FEC block,
 * i.e. an FEC block that has been received with invalid parity on the
 * associated PCS lane and has been corrected by the FEC decoder. The
 * counter is reset to all zeros when the register is read, and held at
 * all ones in case of overflow.  The reset operation takes precedence
 * over the increment operation; if the register is read on the same
 * clock cycle as an increment operation, the counter is reset to all
 * zeros and the increment operation is lost. The counters are writable
 * for test purposes, rather than read-only as specified in IEEE 802.3.
 */
union rpmx_spux_lnx_fec_corr_blks {
	u64 u;
	struct rpmx_spux_lnx_fec_corr_blks_s {
		u64 ln_corr_blks                     : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_spux_lnx_fec_corr_blks_s cn; */
};

static inline u64 RPMX_SPUX_LNX_FEC_CORR_BLKS(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_LNX_FEC_CORR_BLKS(u64 a, u64 b)
{
	return 0xd0700 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_spu#_ln#_fec_uncorr_blks
 *
 * RPM SPU FEC Uncorrected-Blocks Counters 0-19 Registers This register
 * is valid only when the LPCS type is BASE-R
 * (RPM()_CMR()_CONFIG[LMAC_TYPE]) and applies to BASE-R FEC and Reed-
 * Solomon FEC (RS-FEC). When BASE-R FEC is enabled, the FEC corrected-
 * block counters are defined in IEEE 802.3 section 74.8.4.2. Each
 * uncorrected-blocks counter increments by one for an uncorrected FEC
 * block, i.e. an FEC block that has been received with invalid parity on
 * the associated PCS lane and has not been corrected by the FEC decoder.
 * The counter is reset to all zeros when the register is read, and held
 * at all ones in case of overflow.  The reset operation takes precedence
 * over the increment operation; if the register is read on the same
 * clock cycle as an increment operation, the counter is reset to all
 * zeros and the increment operation is lost. The counters are writable
 * for test purposes, rather than read-only as specified in IEEE 802.3.
 */
union rpmx_spux_lnx_fec_uncorr_blks {
	u64 u;
	struct rpmx_spux_lnx_fec_uncorr_blks_s {
		u64 ln_uncorr_blks                   : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_spux_lnx_fec_uncorr_blks_s cn; */
};

static inline u64 RPMX_SPUX_LNX_FEC_UNCORR_BLKS(u64 a, u64 b)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_LNX_FEC_UNCORR_BLKS(u64 a, u64 b)
{
	return 0xd0800 + 0x100000 * a + 8 * b;
}

/**
 * Register (RSL) rpm#_spu#_lpcs_states
 *
 * RPM SPU BASE-X Transmit/Receive States Registers
 */
union rpmx_spux_lpcs_states {
	u64 u;
	struct rpmx_spux_lpcs_states_s {
		u64 deskew_sm                        : 3;
		u64 reserved_3                       : 1;
		u64 deskew_am_found                  : 20;
		u64 bx_rx_sm                         : 2;
		u64 reserved_26_27                   : 2;
		u64 br_rx_sm                         : 3;
		u64 reserved_31_63                   : 33;
	} s;
	/* struct rpmx_spux_lpcs_states_s cn; */
};

static inline u64 RPMX_SPUX_LPCS_STATES(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_LPCS_STATES(u64 a)
{
	return 0xd0208 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_misc_control
 *
 * RPM SPU Miscellaneous Control Registers "* RX logical PCS lane
 * polarity vector \<3:0\> = [XOR_RXPLRT]\<3:0\> ^ {4{[RXPLRT]}}. * TX
 * logical PCS lane polarity vector \<3:0\> = [XOR_TXPLRT]\<3:0\> ^
 * {4{[TXPLRT]}}.  In short, keep [RXPLRT] and [TXPLRT] cleared, and use
 * [XOR_RXPLRT] and [XOR_TXPLRT] fields to define the polarity per
 * logical PCS lane. Only bit 0 of vector is used for 10GBASE-R, and only
 * bits 1:0 of vector are used for RXAUI."
 */
union rpmx_spux_misc_control {
	u64 u;
	struct rpmx_spux_misc_control_s {
		u64 txplrt                           : 1;
		u64 rxplrt                           : 1;
		u64 xor_txplrt                       : 4;
		u64 xor_rxplrt                       : 4;
		u64 intlv_rdisp                      : 1;
		u64 skip_after_term                  : 1;
		u64 rx_packet_dis                    : 1;
		u64 rx_edet_signal_ok                : 1;
		u64 reserved_14_63                   : 50;
	} s;
	/* struct rpmx_spux_misc_control_s cn; */
};

static inline u64 RPMX_SPUX_MISC_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_MISC_CONTROL(u64 a)
{
	return 0xd0218 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_rsfec_corr
 *
 * RPM SPU Reed-Solomon FEC Corrected Codeword Counter Register This
 * register implements the IEEE 802.3 RS-FEC corrected codewords counter
 * described in 802.3 section 91.6.8 (for 100G and extended to 50G) and
 * 802.3by-2016 section 108.6.7 (for 25G and extended to USXGMII).
 */
union rpmx_spux_rsfec_corr {
	u64 u;
	struct rpmx_spux_rsfec_corr_s {
		u64 cw_cnt                           : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_spux_rsfec_corr_s cn; */
};

static inline u64 RPMX_SPUX_RSFEC_CORR(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_RSFEC_CORR(u64 a)
{
	return 0xd0088 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_rsfec_status
 *
 * RPM SPU Reed-Solomon FEC Status Registers This register implements the
 * IEEE 802.3 RS-FEC status and lane mapping registers as described in
 * 802.3 section 91.6 (for 100G and extended to 50G) and 802.3by-2016
 * section 108-6 (for 25G and extended to USXGMII).
 */
union rpmx_spux_rsfec_status {
	u64 u;
	struct rpmx_spux_rsfec_status_s {
		u64 fec_lane_mapping                 : 8;
		u64 fec_align_status                 : 1;
		u64 amps_lock                        : 4;
		u64 hi_ser                           : 1;
		u64 fec_byp_ind_abil                 : 1;
		u64 fec_byp_cor_abil                 : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_spux_rsfec_status_s cn; */
};

static inline u64 RPMX_SPUX_RSFEC_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_RSFEC_STATUS(u64 a)
{
	return 0xd0080 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_rsfec_uncorr
 *
 * RPM SPU Reed-Solomon FEC Uncorrected Codeword Counter Register This
 * register implements the IEEE 802.3 RS-FEC uncorrected codewords
 * counter described in 802.3 section 91.6.9 (for 100G and extended to
 * 50G) and 802.3by-2016 section 108.6.8 (for 25G and extended to
 * USXGMII).
 */
union rpmx_spux_rsfec_uncorr {
	u64 u;
	struct rpmx_spux_rsfec_uncorr_s {
		u64 cw_cnt                           : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_spux_rsfec_uncorr_s cn; */
};

static inline u64 RPMX_SPUX_RSFEC_UNCORR(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_RSFEC_UNCORR(u64 a)
{
	return 0xd0090 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_rx_eee_wake
 *
 * INTERNAL: RPM SPU  RX EEE Wake Error Counter  Registers  Reserved.
 * Internal: A counter that is incremented each time that the LPI receive
 * state diagram enters the RX_WTF state indicating that a wake time
 * fault has been detected.
 */
union rpmx_spux_rx_eee_wake {
	u64 u;
	struct rpmx_spux_rx_eee_wake_s {
		u64 wtf_error_counter                : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_spux_rx_eee_wake_s cn; */
};

static inline u64 RPMX_SPUX_RX_EEE_WAKE(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_RX_EEE_WAKE(u64 a)
{
	return 0xd03e0 + 8 * a;
}

/**
 * Register (RSL) rpm#_spu#_rx_lpi_timing
 *
 * INTERNAL: RPM SPU RX EEE LPI Timing Parameters Registers  Reserved.
 * Internal: This register specifies receiver LPI timing parameters Tqr,
 * Twr and Twtf.
 */
union rpmx_spux_rx_lpi_timing {
	u64 u;
	struct rpmx_spux_rx_lpi_timing_s {
		u64 twtf                             : 20;
		u64 twr                              : 20;
		u64 tqr                              : 20;
		u64 reserved_60_61                   : 2;
		u64 rx_lpi_fw                        : 1;
		u64 rx_lpi_en                        : 1;
	} s;
	/* struct rpmx_spux_rx_lpi_timing_s cn; */
};

static inline u64 RPMX_SPUX_RX_LPI_TIMING(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_RX_LPI_TIMING(u64 a)
{
	return 0xd03c0 + 8 * a;
}

/**
 * Register (RSL) rpm#_spu#_rx_lpi_timing2
 *
 * INTERNAL: RPM SPU RX EEE LPI Timing2 Parameters Registers  Reserved.
 * Internal: This register specifies receiver LPI timing parameters
 * hold_off_timer.
 */
union rpmx_spux_rx_lpi_timing2 {
	u64 u;
	struct rpmx_spux_rx_lpi_timing2_s {
		u64 hold_off_timer                   : 20;
		u64 reserved_20_63                   : 44;
	} s;
	/* struct rpmx_spux_rx_lpi_timing2_s cn; */
};

static inline u64 RPMX_SPUX_RX_LPI_TIMING2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_RX_LPI_TIMING2(u64 a)
{
	return 0xd0420 + 8 * a;
}

/**
 * Register (RSL) rpm#_spu#_rx_mrk_cnt
 *
 * RPM SPU Receiver Marker Interval Count Control Registers
 */
union rpmx_spux_rx_mrk_cnt {
	u64 u;
	struct rpmx_spux_rx_mrk_cnt_s {
		u64 mrk_cnt                          : 20;
		u64 reserved_20_43                   : 24;
		u64 by_mrk_100g                      : 1;
		u64 reserved_45_47                   : 3;
		u64 ram_mrk_cnt                      : 8;
		u64 reserved_56_63                   : 8;
	} s;
	/* struct rpmx_spux_rx_mrk_cnt_s cn; */
};

static inline u64 RPMX_SPUX_RX_MRK_CNT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_RX_MRK_CNT(u64 a)
{
	return 0xd03a0 + 8 * a;
}

/**
 * Register (RSL) rpm#_spu#_spd_abil
 *
 * RPM SPU PCS Speed Ability Registers
 */
union rpmx_spux_spd_abil {
	u64 u;
	struct rpmx_spux_spd_abil_s {
		u64 tengb                            : 1;
		u64 tenpasst                         : 1;
		u64 usxgmii                          : 1;
		u64 twentyfivegb                     : 1;
		u64 fortygb                          : 1;
		u64 fiftygb                          : 1;
		u64 hundredgb                        : 1;
		u64 reserved_7_63                    : 57;
	} s;
	/* struct rpmx_spux_spd_abil_s cn; */
};

static inline u64 RPMX_SPUX_SPD_ABIL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_SPD_ABIL(u64 a)
{
	return 0xd0010 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_status1
 *
 * RPM SPU Status 1 Registers
 */
union rpmx_spux_status1 {
	u64 u;
	struct rpmx_spux_status1_s {
		u64 reserved_0                       : 1;
		u64 lpable                           : 1;
		u64 rcv_lnk                          : 1;
		u64 reserved_3_6                     : 4;
		u64 flt                              : 1;
		u64 rx_lpi_indication                : 1;
		u64 tx_lpi_indication                : 1;
		u64 rx_lpi_received                  : 1;
		u64 tx_lpi_received                  : 1;
		u64 reserved_12_63                   : 52;
	} s;
	/* struct rpmx_spux_status1_s cn; */
};

static inline u64 RPMX_SPUX_STATUS1(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_STATUS1(u64 a)
{
	return 0xd0008 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_status2
 *
 * RPM SPU Status 2 Registers
 */
union rpmx_spux_status2 {
	u64 u;
	struct rpmx_spux_status2_s {
		u64 tengb_r                          : 1;
		u64 tengb_x                          : 1;
		u64 tengb_w                          : 1;
		u64 tengb_t                          : 1;
		u64 usxgmii_r                        : 1;
		u64 twentyfivegb_r                   : 1;
		u64 fortygb_r                        : 1;
		u64 fiftygb_r                        : 1;
		u64 hundredgb_r                      : 1;
		u64 reserved_9                       : 1;
		u64 rcvflt                           : 1;
		u64 xmtflt                           : 1;
		u64 reserved_12_13                   : 2;
		u64 dev                              : 2;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_spux_status2_s cn; */
};

static inline u64 RPMX_SPUX_STATUS2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_STATUS2(u64 a)
{
	return 0xd0020 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_tx_lpi_timing
 *
 * INTERNAL: RPM SPU TX EEE LPI Timing Parameters Registers  Reserved.
 * Internal: Transmit LPI timing parameters Tsl, Tql and Tul
 */
union rpmx_spux_tx_lpi_timing {
	u64 u;
	struct rpmx_spux_tx_lpi_timing_s {
		u64 tql                              : 19;
		u64 reserved_19_31                   : 13;
		u64 tul                              : 12;
		u64 reserved_44_47                   : 4;
		u64 tsl                              : 12;
		u64 reserved_60                      : 1;
		u64 tx_lpi_ignore_twl                : 1;
		u64 tx_lpi_fw                        : 1;
		u64 tx_lpi_en                        : 1;
	} s;
	/* struct rpmx_spux_tx_lpi_timing_s cn; */
};

static inline u64 RPMX_SPUX_TX_LPI_TIMING(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_TX_LPI_TIMING(u64 a)
{
	return 0xd0400 + 8 * a;
}

/**
 * Register (RSL) rpm#_spu#_tx_lpi_timing2
 *
 * INTERNAL: RPM SPU TX EEE LPI Timing2 Parameters Registers  Reserved.
 * Internal: This register specifies transmit LPI timer parameters.
 */
union rpmx_spux_tx_lpi_timing2 {
	u64 u;
	struct rpmx_spux_tx_lpi_timing2_s {
		u64 t1u                              : 8;
		u64 reserved_8_11                    : 4;
		u64 twl                              : 12;
		u64 reserved_24_31                   : 8;
		u64 twl2                             : 12;
		u64 reserved_44_47                   : 4;
		u64 tbyp                             : 12;
		u64 reserved_60_63                   : 4;
	} s;
	/* struct rpmx_spux_tx_lpi_timing2_s cn; */
};

static inline u64 RPMX_SPUX_TX_LPI_TIMING2(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_TX_LPI_TIMING2(u64 a)
{
	return 0xd0440 + 8 * a;
}

/**
 * Register (RSL) rpm#_spu#_tx_mrk_cnt
 *
 * RPM SPU Transmitter Marker Interval Count Control Registers
 */
union rpmx_spux_tx_mrk_cnt {
	u64 u;
	struct rpmx_spux_tx_mrk_cnt_s {
		u64 mrk_cnt                          : 20;
		u64 reserved_20_43                   : 24;
		u64 by_mrk_100g                      : 1;
		u64 reserved_45_47                   : 3;
		u64 ram_mrk_cnt                      : 8;
		u64 reserved_56_63                   : 8;
	} s;
	/* struct rpmx_spux_tx_mrk_cnt_s cn; */
};

static inline u64 RPMX_SPUX_TX_MRK_CNT(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_TX_MRK_CNT(u64 a)
{
	return 0xd0380 + 8 * a;
}

/**
 * Register (RSL) rpm#_spu#_usx_an_adv
 *
 * RPM SPU USXGMII Autonegotiation Advertisement Registers Software
 * programs this register with the contents of the AN-link code word base
 * page to be transmitted during autonegotiation. Any write operations to
 * this register prior to completion of autonegotiation should be
 * followed by a renegotiation in order for the new values to take
 * effect. Once autonegotiation has completed, software can examine this
 * register along with RPM()_SPU()_USX_AN_ADV to determine the highest
 * common denominator technology. The format for this register is from
 * USXGMII Multiport specification section 1.1.2 Table 2.
 */
union rpmx_spux_usx_an_adv {
	u64 u;
	struct rpmx_spux_usx_an_adv_s {
		u64 set                              : 1;
		u64 reserved_1_6                     : 6;
		u64 eee_clk_stop_abil                : 1;
		u64 eee_abil                         : 1;
		u64 spd                              : 3;
		u64 dplx                             : 1;
		u64 reserved_13_14                   : 2;
		u64 lnk_st                           : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_spux_usx_an_adv_s cn; */
};

static inline u64 RPMX_SPUX_USX_AN_ADV(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_USX_AN_ADV(u64 a)
{
	return 0xd01d0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_usx_an_control
 *
 * RPM SPU USXGMII Autonegotiation Control Register
 */
union rpmx_spux_usx_an_control {
	u64 u;
	struct rpmx_spux_usx_an_control_s {
		u64 reserved_0_8                     : 9;
		u64 rst_an                           : 1;
		u64 reserved_10_11                   : 2;
		u64 an_en                            : 1;
		u64 reserved_13_14                   : 2;
		u64 an_reset                         : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_spux_usx_an_control_s cn; */
};

static inline u64 RPMX_SPUX_USX_AN_CONTROL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_USX_AN_CONTROL(u64 a)
{
	return 0xd01c0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_usx_an_expansion
 *
 * RPM SPU USXGMII Autonegotiation Expansion Register This register is
 * only used to signal page reception.
 */
union rpmx_spux_usx_an_expansion {
	u64 u;
	struct rpmx_spux_usx_an_expansion_s {
		u64 reserved_0                       : 1;
		u64 an_page_received                 : 1;
		u64 next_page_able                   : 1;
		u64 reserved_3_63                    : 61;
	} s;
	/* struct rpmx_spux_usx_an_expansion_s cn; */
};

static inline u64 RPMX_SPUX_USX_AN_EXPANSION(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_USX_AN_EXPANSION(u64 a)
{
	return 0xd01e0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_usx_an_flow_ctrl
 *
 * RPM SPU USXGMII Flow Control Registers This register is used by
 * software to affect USXGMII AN hardware behavior.
 */
union rpmx_spux_usx_an_flow_ctrl {
	u64 u;
	struct rpmx_spux_usx_an_flow_ctrl_s {
		u64 start_idle_detect                : 1;
		u64 reserved_1_63                    : 63;
	} s;
	/* struct rpmx_spux_usx_an_flow_ctrl_s cn; */
};

static inline u64 RPMX_SPUX_USX_AN_FLOW_CTRL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_USX_AN_FLOW_CTRL(u64 a)
{
	return 0xd01e8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_usx_an_link_timer
 *
 * RPM SPU USXGMII Link Timer Registers This is the link timer register.
 */
union rpmx_spux_usx_an_link_timer {
	u64 u;
	struct rpmx_spux_usx_an_link_timer_s {
		u64 count                            : 16;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_spux_usx_an_link_timer_s cn; */
};

static inline u64 RPMX_SPUX_USX_AN_LINK_TIMER(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_USX_AN_LINK_TIMER(u64 a)
{
	return 0xd01f0 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_usx_an_lp_abil
 *
 * RPM SPU USXGMII Autonegotiation Link-Partner Advertisement Registers
 * This register captures the contents of the latest AN link code word
 * base page received from the link partner during autonegotiation. This
 * is register 5 per IEEE 802.3, Clause 37.
 * RPM()_SPU()_USX_AN_EXPANSION[AN_PAGE_RECEIVED] is set when this
 * register is updated by hardware.
 */
union rpmx_spux_usx_an_lp_abil {
	u64 u;
	struct rpmx_spux_usx_an_lp_abil_s {
		u64 set                              : 1;
		u64 reserved_1_6                     : 6;
		u64 eee_clk_stop_abil                : 1;
		u64 eee_abil                         : 1;
		u64 spd                              : 3;
		u64 dplx                             : 1;
		u64 reserved_13_14                   : 2;
		u64 lnk_st                           : 1;
		u64 reserved_16_63                   : 48;
	} s;
	/* struct rpmx_spux_usx_an_lp_abil_s cn; */
};

static inline u64 RPMX_SPUX_USX_AN_LP_ABIL(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_USX_AN_LP_ABIL(u64 a)
{
	return 0xd01d8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu#_usx_an_status
 *
 * RPM SPU USXGMII Autonegotiation Status Register
 */
union rpmx_spux_usx_an_status {
	u64 u;
	struct rpmx_spux_usx_an_status_s {
		u64 extnd                            : 1;
		u64 reserved_1                       : 1;
		u64 lnk_st                           : 1;
		u64 an_abil                          : 1;
		u64 rmt_flt                          : 1;
		u64 an_cpt                           : 1;
		u64 reserved_6_63                    : 58;
	} s;
	/* struct rpmx_spux_usx_an_status_s cn; */
};

static inline u64 RPMX_SPUX_USX_AN_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPUX_USX_AN_STATUS(u64 a)
{
	return 0xd01c8 + 0x100000 * a;
}

/**
 * Register (RSL) rpm#_spu_dbg_control
 *
 * RPM SPU Debug Control Registers
 */
union rpmx_spu_dbg_control {
	u64 u;
	struct rpmx_spu_dbg_control_s {
		u64 marker_rxp                       : 15;
		u64 reserved_15                      : 1;
		u64 scramble_dis                     : 1;
		u64 reserved_17_18                   : 2;
		u64 br_pmd_train_soft_en             : 1;
		u64 reserved_20_27                   : 8;
		u64 timestamp_norm_dis               : 1;
		u64 an_nonce_match_dis               : 1;
		u64 br_ber_mon_dis                   : 1;
		u64 rf_cw_mon_erly_restart_dis       : 1;
		u64 us_clk_period                    : 12;
		u64 ms_clk_period                    : 12;
		u64 reserved_56_63                   : 8;
	} s;
	/* struct rpmx_spu_dbg_control_s cn; */
};

static inline u64 RPMX_SPU_DBG_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPU_DBG_CONTROL(void)
{
	return 0xd0300;
}

/**
 * Register (RSL) rpm#_spu_sds#_skew_status
 *
 * RPM SPU SerDes Lane Skew Status Registers This register provides
 * SerDes lane skew status. One register per physical SerDes lane.
 */
union rpmx_spu_sdsx_skew_status {
	u64 u;
	struct rpmx_spu_sdsx_skew_status_s {
		u64 skew_status                      : 32;
		u64 reserved_32_63                   : 32;
	} s;
	/* struct rpmx_spu_sdsx_skew_status_s cn; */
};

static inline u64 RPMX_SPU_SDSX_SKEW_STATUS(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPU_SDSX_SKEW_STATUS(u64 a)
{
	return 0xd0340 + 8 * a;
}

/**
 * Register (RSL) rpm#_spu_sds#_states
 *
 * RPM SPU SerDes States Registers This register provides SerDes lane
 * states. One register per physical SerDes lane.
 */
union rpmx_spu_sdsx_states {
	u64 u;
	struct rpmx_spu_sdsx_states_s {
		u64 bx_sync_sm                       : 4;
		u64 br_sh_cnt                        : 11;
		u64 br_block_lock                    : 1;
		u64 br_sh_invld_cnt                  : 7;
		u64 reserved_23                      : 1;
		u64 fec_sync_cnt                     : 4;
		u64 fec_block_sync                   : 1;
		u64 reserved_29                      : 1;
		u64 an_rx_sm                         : 2;
		u64 an_arb_sm                        : 3;
		u64 reserved_35                      : 1;
		u64 train_lock_bad_markers           : 3;
		u64 train_lock_found_1st_marker      : 1;
		u64 train_frame_lock                 : 1;
		u64 train_code_viol                  : 1;
		u64 train_sm                         : 3;
		u64 reserved_45_47                   : 3;
		u64 am_lock_sm                       : 2;
		u64 am_lock_invld_cnt                : 2;
		u64 reserved_52_63                   : 12;
	} s;
	/* struct rpmx_spu_sdsx_states_s cn; */
};

static inline u64 RPMX_SPU_SDSX_STATES(u64 a)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPU_SDSX_STATES(u64 a)
{
	return 0xd0360 + 8 * a;
}

/**
 * Register (RSL) rpm#_spu_usxgmii_control
 *
 * RPM SPU Common USXGMII Control Register This register is the common
 * control register that enables USXGMII Mode. The fields in this
 * register are preserved across any LMAC soft-resets. For an LMAC in
 * soft- reset state in USXGMII mode, the RPM will transmit Remote Fault
 * BASE-R blocks.
 */
union rpmx_spu_usxgmii_control {
	u64 u;
	struct rpmx_spu_usxgmii_control_s {
		u64 enable                           : 1;
		u64 usxgmii_type                     : 3;
		u64 sds_id                           : 2;
		u64 reserved_6_63                    : 58;
	} s;
	/* struct rpmx_spu_usxgmii_control_s cn; */
};

static inline u64 RPMX_SPU_USXGMII_CONTROL(void)
	__attribute__ ((pure, always_inline));
static inline u64 RPMX_SPU_USXGMII_CONTROL(void)
{
	return 0xd0920;
}

#endif /* __CSRS_RPM_H__ */
