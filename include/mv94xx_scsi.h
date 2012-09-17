/*
 * Copyright (C) Freescale Semiconductor, Inc. 2006.
 * Author: Jason Jin<Jason.jin@freescale.com>
 *         Zhang Wei<wei.zhang@freescale.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */
#ifndef _MV94XX_H_
#define _MV94XX_H_

#include <pci.h>


/***************************************************************************/
#define SAS_REG_BASE	0x20000L

enum common_regs {
	/* SATA/SAS port common registers */
	COMMON_PORT_IMPLEMENT        = 0x9C,  /* port implement register */
	COMMON_PORT_TYPE        = 0xa0,  /* port type register */
	COMMON_CONFIG           = 0x100, /* configuration register */
	COMMON_CONTROL          = 0x104, /* control register */
	COMMON_LST_ADDR         = 0x108, /* command list DMA addr */
	COMMON_LST_ADDR_HI      = 0x10c, /* command list DMA addr hi */
	COMMON_FIS_ADDR         = 0x110, /* FIS rx buf addr */
	COMMON_FIS_ADDR_HI      = 0x114, /* FIS rx buf addr hi */

	COMMON_SATA_REG_SET0    = 0x0118, /* SATA/STP Register Set 0 */
	COMMON_SATA_REG_SET1    = 0x011c, /* SATA/STP Register Set 1 */

	COMMON_DELV_Q_CONFIG    = 0x120, /* delivery queue configuration */
	COMMON_DELV_Q_ADDR      = 0x124, /* delivery queue base address */
	COMMON_DELV_Q_ADDR_HI   = 0x128, /* delivery queue base address hi */
	COMMON_DELV_Q_WR_PTR    = 0x12c, /* delivery queue write pointer */
	COMMON_DELV_Q_RD_PTR    = 0x130, /* delivery queue read pointer */
	COMMON_CMPL_Q_CONFIG    = 0x134, /* completion queue configuration */
	COMMON_CMPL_Q_ADDR      = 0x138, /* completion queue base address */
	COMMON_CMPL_Q_ADDR_HI   = 0x13c, /* completion queue base address hi */
	COMMON_CMPL_Q_WR_PTR    = 0x140, /* completion queue write pointer */
	COMMON_CMPL_Q_RD_PTR    = 0x144, /* completion queue read pointer */

	COMMON_COAL_CONFIG      = 0x148, /* interrupt coalescing config */
	COMMON_COAL_TIMEOUT     = 0x14c, /* interrupt coalescing time wait */
	COMMON_IRQ_STAT         = 0x150, /* interrupt status */
	COMMON_IRQ_MASK         = 0x154, /* interrupt enable/disable mask */

	COMMON_SRS_IRQ_STAT0    = 0x0158, /* SRS interrupt status 0 */
	COMMON_SRS_IRQ_MASK0    = 0x015c, /* SRS intr enable/disable mask 0 */
	COMMON_SRS_IRQ_STAT1    = 0x0160, /* SRS interrupt status 1*/
	COMMON_SRS_IRQ_MASK1    = 0x0164, /* SRS intr enable/disable mask 1*/

	COMMON_CMD_ADDR         = 0x0170, /* Command Address Port */
	COMMON_CMD_DATA         = 0x0174, /* Command Data Port */

	/* Port interrupt status/mask register set $i (0x180/0x184-0x1b8/0x1bc) */
	COMMON_PORT_IRQ_STAT0   = 0x0180,
	COMMON_PORT_IRQ_MASK0   = 0x0184,

	/* port serial control/status register set $i (0x1d0-0x1ec) */
	COMMON_PORT_PHY_CONTROL0  = 0x01d0,
	COMMON_PORT_ALL_PHY_CONTROL=0x01f0, /* All port serial status/control */

	/* port config address/data regsiter set $i (0x200/0x204 - 0x238/0x23c) */
	COMMON_PORT_CONFIG_ADDR0  = 0x0200,
	COMMON_PORT_CONFIG_DATA0  = 0x0204,

	/* port vendor specific address/data register set $i (0x250/0x254-0x268/0x26c) */
	COMMON_PORT_VSR_ADDR0      = 0x0250,
	COMMON_PORT_VSR_DATA0      = 0x0254,	
	COMMON_PORT_ALL_VSR_ADDR   = 0x0290, /* All port Vendor Specific Register addr */
	COMMON_PORT_ALL_VSR_DATA   = 0x0294, /* All port Vendor Specific Register Data */
};

enum common_regs_bits {
	/* COMMON_CONFIG register bits */
	CONFIG_CMD_TBL_BE       = (1U << 0),
	CONFIG_OPEN_ADDR_BE     = (1U << 1),
	CONFIG_RSPNS_FRAME_BE   = (1U << 2),
	CONFIG_DATA_BE          = (1U << 3),
	CONFIG_SAS_SATA_RST     = (1U << 5),
	CONFIG_STP_STOP_ON_ERR	= (1U << 25),
	
	/* COMMON_CONTROL : port control/status bits (R104h) */
	CONTROL_EN_CMD_ISSUE        = (1U << 0),
	CONTROL_RESET_CMD_ISSUE     = (1U << 1),
	CONTROL_ERR_STOP_CMD_ISSUE  = (1U << 3),
	CONTROL_FIS_RCV_EN          = (1U << 4),
	CONTROL_CMD_CMPL_SELF_CLEAR = (1U << 5),
	CONTROL_EN_SATA_RETRY       = (1U << 6),
	CONTROL_RSPNS_RCV_EN        = (1U << 7),

	/* COMMON_DELV_Q_CONFIG (R120h) bits */
	DELV_QUEUE_SIZE_MASK        = (0xFFFU << 0),
	DELV_QUEUE_ENABLE           = (1U << 16),

	/* COMMON_CMPL_Q_CONFIG (R134h) bits */
	CMPL_QUEUE_SIZE_MASK        = (0xFFFU << 0),
	CMPL_QUEUE_ENABLE           = (1U << 16),

	/* COMMON_COAL_CONFIG (R148h) bits */
	INT_COAL_COUNT_MASK      = (0x1FFU << 0),
	INT_COAL_ENABLE          = (1U << 16),

	/* COMMON_COAL_TIMEOUT (R14Ch) bits */
	COAL_TIMER_MASK          = (0xFFFFU << 0),
	COAL_TIMER_UNIT_1MS      = (1U << 16),   /* 6.67 ns if set to 0 */

	/* COMMON_IRQ_STAT/MASK (R150h) bits */
	INT_CMD_CMPL               = (1U << 0),
	INT_CMD_CMPL_MASK          = (1U << 0),

	INT_PORT_MASK_OFFSET       = 8,
	INT_PORT_MASK              = (0xFF << INT_PORT_MASK_OFFSET),
	INT_PHY_MASK_OFFSET        = 4,
	INT_PHY_MASK               = (0x0F << INT_PHY_MASK_OFFSET),
	INT_PORT_STOP_MASK_OFFSET  = 16,
	INT_PORT_STOP_MASK         = (0xFF << INT_PORT_STOP_MASK_OFFSET),

	INT_NON_SPCFC_NCQ_ERR	   = (1U << 25),

	INT_MEM_PAR_ERR            = (1U << 26),
	INT_DMA_PEX_TO			= (1U << 27),
	INT_PRD_BC_ERR			= (1U << 28),
	INT_DP_PAR_ERR			= (1U << 29),

	/* COMMON_PORT_IRQ_STAT/MASK (R160h) bits */
	IRQ_PHY_RDY_CHNG_MASK         = (1U << 0),
	IRQ_HRD_RES_DONE_MASK         = (1U << 1),
	IRQ_PHY_ID_DONE_MASK          = (1U << 2),
	IRQ_PHY_ID_FAIL_MASK          = (1U << 3),
	IRQ_PHY_ID_TIMEOUT            = (1U << 4),
	IRQ_HARD_RESET_RCVD_MASK      = (1U << 5),
	IRQ_PORT_SEL_PRESENT_MASK     = (1U << 6),
	IRQ_COMWAKE_RCVD_MASK         = (1U << 7),
	IRQ_BRDCST_CHNG_RCVD_MASK     = (1U << 8),
	IRQ_UNKNOWN_TAG_ERR           = (1U << 9),
	IRQ_IU_TOO_SHRT_ERR           = (1U << 10),
	IRQ_IU_TOO_LNG_ERR            = (1U << 11),
	IRQ_PHY_RDY_CHNG_1_TO_0       = (1U << 12),
	IRQ_SIG_FIS_RCVD_MASK         = (1U << 16),
	IRQ_BIST_ACTVT_FIS_RCVD_MASK  = (1U << 17),
	IRQ_ASYNC_NTFCN_RCVD_MASK     = (1U << 18),
	IRQ_UNASSOC_FIS_RCVD_MASK     = (1U << 19),
	IRQ_STP_SATA_RX_ERR_MASK      = (1U << 20),
	IRQ_STP_SATA_TX_ERR_MASK      = (1U << 21),
	IRQ_STP_SATA_CRC_ERR_MASK     = (1U << 22),
	IRQ_STP_SATA_DCDR_ERR_MASK    = (1U << 23),
	IRQ_STP_SATA_PHY_DEC_ERR_MASK = (1U << 24),
	IRQ_STP_SATA_SYNC_ERR_MASK    = (1U << 25),

	/* common port serial control/status (R180h) bits */
	SCTRL_STP_LINK_LAYER_RESET        = (1 << 0),
	SCTRL_PHY_HARD_RESET_SEQ          = (1 << 1),
	SCTRL_PHY_BRDCST_CHNG_NOTIFY      = (1 << 2),
	SCTRL_SSP_LINK_LAYER_RESET        = (1 << 3),
	SCTRL_MIN_SPP_PHYS_LINK_RATE_MASK = (0xF << 8),
	SCTRL_MAX_SPP_PHYS_LINK_RATE_MASK = (0xF << 12),
	SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK_OFFSET = 16,
	SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK = (0xF << SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK_OFFSET),
	SCTRL_PHY_READY_MASK              = (1 << 20),
};

/* sas/sata configuration port registers */
enum config_regs {
	CONFIG_SATA_CONTROL    = 0x18, /* port SATA control register */
	CONFIG_PHY_CONTROL     = 0x1c, /* port phy control register */

	CONFIG_SATA_SIG0       = 0x20, /* port SATA signature FIS(Byte 0-3) */
	CONFIG_SATA_SIG1       = 0x24, /* port SATA signature FIS(Byte 4-7) */
	CONFIG_SATA_SIG2       = 0x28, /* port SATA signature FIS(Byte 8-11) */
	CONFIG_SATA_SIG3       = 0x2c, /* port SATA signature FIS(Byte 12-15)*/
	CONFIG_R_ERR_COUNT     = 0x30, /* port R_ERR count register */
	CONFIG_CRC_ERR_COUNT   = 0x34, /* port CRC error count register */
	CONFIG_WIDE_PORT       = 0x38, /* port wide participating register */

	CONFIG_CRN_CNT_INFO0   = 0x80, /* port current connection info register 0*/
	CONFIG_CRN_CNT_INFO1   = 0x84, /* port current connection info register 1*/
	CONFIG_CRN_CNT_INFO2   = 0x88, /* port current connection info register 2*/
	CONFIG_ID_FRAME0       = 0x100, /* Port device ID frame register 0, DEV Info*/
	CONFIG_ID_FRAME1       = 0x104, /* Port device ID frame register 1*/
	CONFIG_ID_FRAME2       = 0x108, /* Port device ID frame register 2*/
	CONFIG_ID_FRAME3       = 0x10c, /* Port device ID frame register 3, SAS Address lo*/
	CONFIG_ID_FRAME4       = 0x110, /* Port device ID frame register 4, SAS Address hi*/
	CONFIG_ID_FRAME5       = 0x114, /* Port device ID frame register 5, Phy Id*/
	CONFIG_ID_FRAME6       = 0x118, /* Port device ID frame register 6*/
	CONFIG_ATT_ID_FRAME0   = 0x11c, /* attached device ID frame register 0*/
	CONFIG_ATT_ID_FRAME1   = 0x120, /* attached device ID frame register 1*/
	CONFIG_ATT_ID_FRAME2   = 0x124, /* attached device ID frame register 2*/
	CONFIG_ATT_ID_FRAME3   = 0x128, /* attached device ID frame register 3*/
	CONFIG_ATT_ID_FRAME4   = 0x12c, /* attached device ID frame register 4*/
	CONFIG_ATT_ID_FRAME5   = 0x130, /* attached device ID frame register 5*/
	CONFIG_ATT_ID_FRAME6   = 0x134, /* attached device ID frame register 6*/
};

enum vsr_regs {
	VSR_IRQ_STATUS      = 0x00,
	VSR_IRQ_MASK        = 0x04,
	VSR_PHY_CONFIG      = 0x08,
	VSR_PHY_STATUS      = 0x0c,

	VSR_PHY_MODE_REG_1	= 0x064,
	VSR_PHY_FFE_CONTROL	= 0x10C,
	VSR_PHY_DFE_UPDATE_CRTL	= 0x110,
	VSR_REF_CLOCK_CRTL	= 0x1A0,
};
enum vsr_reg_bits {
	/* VSR_IRQ_STATUS bits */
	VSR_IRQ_PHY_TIMEOUT 	= (1U << 10),
	
	/* VSR_PHY_STATUS bits */
	VSR_PHY_STATUS_MASK 	= 0x3f0000,
	VSR_PHY_STATUS_IDLE 	= 0x00,
	VSR_PHY_STATUS_SAS_RDY	= 0x10,
	VSR_PHY_STATUS_HR_RDY	= 0x1d,
};

#define MV_MAX_CORES	2
#define MV_MAX_CORE_PHY	4
#define MV_MAX_PORTS	(MV_MAX_CORES*MV_MAX_CORE_PHY)
#define MV_MAX_PM_NUM	8
#define MV_MAX_DEV_PER_PM		5
#define MV_MAX_DEVICES	(MV_MAX_PM_NUM*MV_MAX_DEV_PER_PM)

struct sas_sense_hdr {
        u8 response_code;       /* permit: 0x0, 0x70, 0x71, 0x72, 0x73 */
        u8 sense_key;
        u8 asc;
        u8 ascq;
};

struct mv_sg {
	u32	addr;
	u32	addr_hi;
	u32	flags_size;
};

struct mv_cmd_hdr {
	/*DWORD0*/
	u32	pm_port : 4;			/* Port Multiplier field in command FIS */
	u32	bist : 1;				/* SATA only, set if it is a BIST FIS */
	u32	atapi : 1;				/* SATA only, set if it is a ATAPI PIO */
	u32	fst_dma : 1;			/* SATA only, set if it is a first party DMA command */
	u32	reset: 1;				/* SATA only, set if it is for device reset */
	u32	pi : 1;	/* SSP only, set if protection information record present */
	u32	ssp_retry : 1;			/* SSP only, set if enabling SSP transport layer retry */
	u32	ssp_verify : 1;	/* SSP only, verify Data length */
	u32	ssp_fb : 1;		/* SSP only, generate Burst without waiting for XFER-RDY */
	u32	ssp_pt : 1;	/* SSP only, 0-frame type set by HW, 1-frame type given by SSP_SSPFrameType */
	u32	ssp_ft : 3;
	u32	prd_entry_cnt : 8;
	u32	rsvd : 8;
/* DWORD 1 */
	u32	frm_len : 9;		/* command frame length in DW, including frame length, excluding CRC */
	u32	rsvd1 : 7;
	u32	max_rsp_len : 9;	/* max response frame length in DW, HW will put in status buffer structure */
	u32	rsvd2 : 7;
/* DWORD 2 */
	u32	tag: 16;				/* command tag */
	u32	tgt_tag : 16;	
	
	u32	data_len;
	u32	tbl_addr;
	u32	tbl_addr_hi;
	u32	opf_addr;
	u32	opf_addr_hi;
	u32	sbf_addr;
	u32	sbf_addr_hi;
	u32	prdt_addr;
	u32	prdt_addr_hi;
	u32	intf_select;
	u32	reserved[3];
};

struct ssp_frame_hdr {
	u8	frame_type;
	u8	dest_sas_addr[3];
	u8	reserved1;
	u8	src_sas_addr[3];
	u8	reserved2[2];
	
	u8	cdp:1;
	u8	retxed:1;
	u8	retry_data:1;
	u8	reserved3:5;

	u8	num_of_bytes:2;
	u8	reserved4:6;

	u8	reserved5[4];
	u16	tag;
	u16	target_tag;
	u32	data_off;
};

struct ssp_cmd_iu {
	u8	lun[8];
	u8	reserved1;
	u8	task_attrib:3;
	u8	reserved2:4;
	u8	first_burst:1;
	u8	reserved3;
	u8	reserved4:2;
	u8	add_cdb_len:6;
	u8	cdb[16];
};

struct ssp_rsp_iu {
	u8	reserved1[10];
	u8  data_pres;
	u8	status;
	u32	reserved2;
	u32	sd_len;
	u32	rsp_len;
	u8	sense_data[18];
};

struct mv_ssp_cmd_tbl {
	struct ssp_frame_hdr	frm_hdr;
	struct ssp_cmd_iu	cmd_iu;
	u32	reserved[3];
};

/* SATA STP Command Table */
struct mv_stp_cmd_tbl {
	u8	fis[64]; /* Command FIS */
	u8	atapi_cdb[32]; /* ATAPI CDB */
};

struct op_frm {
	u8	frame_type:4;
	u8	protocol:3;
	u8	initiator:1;

	u8	connect_rate:4;
	u8	feature:4;
	u8	connect_tag[2];
	u32	dst_sas_addr_low; //u8	dst_sas_addr[8];
	u32	dst_sas_addr_high;	
	u8	src_sas_addr[8];
	u32	reserved[3];
};

struct err_info {
	u32	record0;
	u32	record1;
};

struct status_info {
	struct	err_info	err_info;
	struct	ssp_rsp_iu rsp_iu;
};

struct mv_cmd_tbl {
	struct op_frm open_addr_frame;
	struct status_info	status_buf;
	struct mv_sg	prd_entry;
	union
	{
		struct mv_ssp_cmd_tbl ssp_cmd_table;
		struct mv_stp_cmd_tbl stp_cmd_table;	
	};
};

struct dlvq_entry {
	u32	slot_nm:12;
	u32	phy:8;
	u32	sata_reg_set:7;
	u32	prio:1;
	u32	mode:1;
	u32	cmd:3;
};

struct cmplq_entry {
	u32	slot_num:12;
	u32	reserved1:4;
	u32	cmd_cmpl:1;
	u32	err_rcrd_xfrd:1;
	u32	rspns_xfrd:1;
	u32	attention:1;
	u32	cmd_rcvd:1; /* target mode */
	u32	slot_rst_cmpl:1;
	u32	rspns_good:1;
	u32	reserved2:9;
};
struct dev_info {
	void *port;
	u16	features;
	u8	state;
	u8	status;

	u16	setting;
	u8	register_set;
	u8	link_rate;
	u32	sector_size;

	u32	sas_addr_low;
	u32	sas_addr_high;

	u8	id;
	u8	dev_type;
	u8	pm_num;
	u8	reserved[5];
};

struct pm_info {
	void	*port;
	struct dev_info *dev[MV_MAX_DEV_PER_PM];
	u8 register_set;
	u8 state;
	u8 status;
	u8 num_ports;
	u8 active_port;
	u8 feature_enabled;
	u8 rsvd[2];
	
	u32	sstatus;
	u32	global_serror;
};

struct port_info {	
	void *host;
	void *core;
	u8	id;
	u8	type;
	u8	phy_map;
	u8	dev_num;
	u16	pm_vendor;
	u16	pm_id;
};

struct phy_info {
	struct port_info *port;
	u32	status;
	u32	attach_sas_addr_low;
	u32	attach_sas_addr_high;
	u32	attach_info;
	u8	type;
	u8	wp_phymap;
	u8	reserved[2];
};

struct sas_core {
	void *host;
	ulong mmio_off;
	
	u16	lst_dlvq;
	u16	lst_cmplq;
	u8	id;
	u8	used_register_set;
	u8	start_phy_id;
	u8	phy_num;
	
	struct phy_info phy[MV_MAX_CORE_PHY];
};

struct host_info {
	pci_dev_t	dev;
	struct sas_core core[MV_MAX_CORES];
	struct port_info ports[MV_MAX_PORTS];
	struct dev_info devices[MV_MAX_DEVICES];
	ulong mmio;
	u32	cmd_list;
	u32	rx_fis;
	u32 cmd_table;
	//phys_addr_t cmd_table_dma;
	u32	dlv_q;
	u32	cmpl_q;
	u32 unassociate_fis_off;
	u8	core_num;
	u8	max_port_num;
	u8	port_num;
	u8	dev_num;
};

#define MAX_SLOT_NUMBER	1
#define MAX_RX_FIS_NUMBER	1
#define MAX_DELV_QUEUE_SIZE	4
#define MAX_CMPL_QUEUE_SIZE	4

#define RX_FIS_SIZE			256
#define CMD_LIST_POOL_SIZE		sizeof(struct mv_cmd_hdr)*MAX_SLOT_NUMBER
#define UNASSOCIATED_FIS_POOL_SIZE      2048
#define RX_FIS_POOL_SIZE		RX_FIS_SIZE*MAX_RX_FIS_NUMBER + UNASSOCIATED_FIS_POOL_SIZE
#define CMD_TABLE_POOL_SIZE		(sizeof(struct mv_cmd_tbl)*MAX_SLOT_NUMBER)
#define DELV_Q_POOL_SIZE			sizeof(struct dlvq_entry)*MAX_DELV_QUEUE_SIZE
#define CMPL_Q_POOL_SIZE			sizeof(struct cmplq_entry)*(MAX_CMPL_QUEUE_SIZE+1)

#define	CORE_BUFFLEN	(64+CMD_LIST_POOL_SIZE	 	\
						+256+RX_FIS_POOL_SIZE		\
						+128+CMD_TABLE_POOL_SIZE 	\
						+64+DELV_Q_POOL_SIZE			\
						+64+CMPL_Q_POOL_SIZE+0x60			\
						) 	
#define PORT_TYPE_SAS					(1 << 0)
#define PORT_TYPE_SATA					(1 << 1)
#define PORT_TYPE_PM					(1 << 2)

#define DEVICE_TYPE_ATAPI						(1 << 0)
#define DEVICE_TYPE_SSP							(1 << 1)
#define DEVICE_TYPE_STP							(1 << 2)
#define DEVICE_TYPE_SATA						(1 << 3)
#define DEVICE_TYPE_SES							(1 << 4)
#define DEVICE_TYPE_TAPE						(1 << 5)
#define DEVICE_TYPE_SSD							(1 << 6) //for blank ssd

#define PM_GSCR_ID					0
#define PM_GSCR_REVISION			1
#define PM_GSCR_INFO				2
#define PM_GSCR_ERROR				32
#define PM_GSCR_ERROR_ENABLE		33
#define PM_GSCR_FEATURES			64
#define PM_GSCR_FEATURES_ENABLE		96

#define PM_PSCR_SSTATUS				0
#define PM_PSCR_SERROR				1
#define PM_PSCR_SCONTROL			2
#define PM_PSCR_SACTIVE				3

#define SATA_RECEIVED_D2H_FIS(unassoc_off,reg_set)       (unassoc_off + 0x100 * reg_set + 0x40)

#define ATA_CMD_PM_READ				0xE4
#define ATA_CMD_PM_WRITE			0xE8

#endif

