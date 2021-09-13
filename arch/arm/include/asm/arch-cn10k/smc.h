/* SPDX-License-Identifier:    GPL-2.0
 *
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#ifndef __SMC_H__
#define __SMC_H__

#include <asm/arch/smc-id.h>
#include <asm/arch/update.h>

ssize_t smc_dram_size(unsigned int node);
ssize_t	smc_disable_rvu_lfs(unsigned int node);

/*
 * Get RVU Reserved Memory Region Info
 *
 * Return:
 *	x0:
 *		0 -- Success
 *	x1 - region start address
 *	x2 - region size
 */
int smc_rvu_rsvd_reg_info(u64 *reg_addr, u64 *reg_size);

/*
 * x1 - descriptor address
 * x2 - descriptor size
 * x3 - 0
 * x4 - 0
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *		-2 -- SPI_CONFIG_ERR
 *		-3 -- SPI_MMAP_ERR
 *		-4 -- SPI_IMG_VALIDATE_ERR
 *		-5 -- SPI_IMG_UPDATE_ERR
 */
int smc_spi_update(const struct smc_update_descriptor *desc);

/*
 * Perform Switch Firmware load to DRAM in ATF
 *
 * x1 - super image location
 * x2 - cm3 image location
 * x3 - ptr to cm3 image size
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *		-2 -- SPI_CONFIG_ERR
 *		-3 -- SPI_MMAP_ERR
 *		-5 -- EIO
 */
int smc_load_switch_fw(u64 super_img_addr, u64 cm3_img_addr,
		       u64 *cm3_img_size);

/*
 * Perform EFI Application Image load to DRAM in ATF
 *
 * x1 - Image location
 * x2 - Pointer to store image size
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *		-2 -- SPI_CONFIG_ERR
 *		-3 -- SPI_MMAP_ERR
 *		-5 -- EIO
 */
int smc_load_efi_img(u64 img_addr, u64 *img_size);

/*
 * Get EFI variabled shared memory info
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *	x1:
 *		Physical address of the shared memory
 *	x2:
 *		Size in bytes of this shared memory
 */
int smc_efi_var_shared_memory(u64 *mem_addr, u64 *mem_size);

/*
 * Perform EFI variable store write to flash in ATF
 *
 * x1 - Variable store location
 * x2 - Variable store size
 * x3 - Flash device bus number
 * X4 - Flash device chip select
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 */
int smc_write_efi_var(u64 var_addr, u64 var_size, u32 bus, u32 cs);

/*
 * Perform secure SPI flash operation
 *
 * x1 - Offset in flash
 * x2 - Buffer pointer
 * x3 - Size
 * X4 - x3[3:0] - Bus, x3[7:4] - Chipselecti, [15:8] - Operation
 *	Operation: 1 - Read, 4 - Info
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 */
unsigned long smc_sec_spi_op(u64 offset, u64 buffer, u64 size, u32 bus,
			     u32 cs, u32 op);

/**
 * Verify objects in flash
 *
 * x1 - address of descriptor
 * x2 - size of descriptor
 *
 * returns:
 *	x0:
 *		0 -- success
 *		negative - error
 */
int smc_spi_verify(struct smc_version_info *desc);

struct gserm_data {
	u8 gserm_idx;
	u8 lanes_num;
	u16 mapping;
};

/**
 * Get PHY's temperature
 *
 * x1 - eth
 * x2 - lmac
 *
 * Return:
 *	x0: 0 (Success) or -1 (Fail)
 *	x1: Avaraged temperature sensor reading of phy @eth(x1), lmac(x2)
 */
int smc_phy_dbg_temp_read(int eth, int lmac, int *temp);

/**
 * Set PHY's loopback
 *
 * x1 - cmd
 *	0 - disable line loopback for phy @eth(x2),lmac(x3)
 *	1 - enable line loopback for phy @eth(x2),lmac(x3)
 *
 * x2 - eth
 * x3 - lmac
 * Return:
 *	x0: 0 (Success) or -1 (Fail)
 *
 */
int smc_phy_dbg_loopback_write(int eth, int lmac, int enable);

/**
 * Configure PRBS for PHY / Get PRBS counters
 *
 * x1 - cmd
 *	1 - start phy prbs with config (x2) for phy @eth(x3),lmac(x4)
 *	2 - stop phy prbs for phy @eth(x3),lmac(x4)
 *	3 - get prbs error counters for phy @eth(x3),lmac(x4)
 *
 * x2 - config, fields are:
 *	- x2[3:2] is pattern selector, options are:
 *		0x00 - PRBS_7
 *		0x01 - PRBS_23
 *		0x10 - PRBS_31
 *		0x11 - PRBS_1010
 *	- x2[1] is the direction
 *	- x2[0] is denoting host or line side:
 *		1 - host side
 *		0 - line side
 * x3 - eth
 * x4 - lmac
 * Return:
 *	x0: 0 (Success) or -1 (Fail)
 *			or
 *		prbs error count for host/line side
 *			(PHY_PRBS_GET_DATA_CMD only)
 */
ssize_t smc_phy_dbg_prbs_read(int eth, int lmac, int host);
int smc_phy_dbg_prbs_write(int eth, int lmac,
			   int cmd, int host, int type);

/**
 * Read / Write PHY registers
 *
 * x1 - cmd, clause and device addr:
 *	x1[0]: command
 *		0 - Read PHY register
 *		1 - Write PHY register
 *	x1[1]: MDIO clause
 *		0 - clause22
 *		1 - clause45
 *
 *	x1[6:2]: device addr (clause45)
 *			or
 *		page nr (paged access in clause22)
 *
 *
 * x2 - register address and/or value
 *	x2[15:0]: register address
 *	x2[31:16]: register value (only in case of Write cmd)
 *
 * x3 - eth
 * x4 - lmac
 *
 * Return:
 *	x0: 0 (Success) or -1 (Fail)
 *	x1: register value (only in case of Read cmd)
 *
 */
ssize_t smc_phy_dbg_reg_read(int eth, int lmac,
			     int mode, int dev_page, int reg, int *val);
int smc_phy_dbg_reg_write(int eth, int lmac,
			  int mode, int dev_page, int reg, int val);

/**
 * Set / Get PHY's SerDes config (VoD)
 *
 * x1 - cmd
 *	0 - Get SerDes config
 *	1 - Set SerDes config
 * x2 - phy's SERDES config (only Tx amplitude supported):
 *	x2[2:0] SGMII VOD level (Tx ampl.), one of:
 *		000 = 14mV
 *		001 = 112mV
 *		010 = 210mV
 *		011 = 308mV
 *		100 = 406mV
 *		101 = 504mV
 *		110 = 602mV
 *		111 = 700mV
 * x3 - eth
 * x4 - lmac
 * Return:
 *	x0: 0 (Success) or -1 (Fail)
 *	x1: config in the same format as in x2
 *		(cmd == 0 only)
 *
 */
int smc_phy_dbg_get_serdes_cfg(int eth, int lmac, u32 *cfg);
int smc_phy_dbg_set_serdes_cfg(int eth, int lmac, u32 cfg);

/**
 * Set SerDes PRBS
 *
 * x1[19]: enable/disable generator (enabled by default)
 * x1[18]: enable/disable checker (enabled by default)
 * x1[17:16]: subcommand:
 *	0 - start prbs
 *	1 - show prbs
 *	2 - clear prbs
 *	3 - stop prbs
 *
 * x1[15:8]:	lane# or 0xff if no lane provided
 *		in which case it will be executed for
 *		all the lanes assigned to the given port
 * x1[7:0]:	port#
 *
 * x2: prbs pattern (valid only for start command)
 * x3: inject error count (valid only for start command)
 *
 * returns:
 *	x0:
 *		0 -- success
 *		negative - error
 *
 *	Show command only:
 *	x1:
 *		SERDES_PRBS_DATA_BASE address, where the following
 *		structure is stored to return prbs error statistics
 *		data for maximum of 4 lanes:
 *
 *		struct prbs_error_stats {
 *			uint64_t total_bits;
 *			uint64_t error_bits;
 *		} stats[4];
 *
 *	x2[31:24]: gserm number
 *	x2[23:8] : port lane# to gserm lane# mapping
 *	x2[7:0]  : Number of lanes assigned to the given port
 *
 */
ssize_t smc_serdes_prbs_start(int port,
			      struct gserm_data *gserm,
			      int pattern, int gen_check,
			      int err_inject_cnt);

ssize_t smc_serdes_prbs_stop(int port, struct gserm_data *gserm);
ssize_t smc_serdes_prbs_clear(int port, struct gserm_data *gserm);
ssize_t smc_serdes_prbs_show(int port, struct gserm_data *gserm,
			     void **error_stats);

enum prbs_subcmd {
	PRBS_START,
	PRBS_SHOW,
	PRBS_CLEAR,
	PRBS_STOP
};

/**
 * Set SerDes Loopback
 *
 * x1[15:8]:	lane# or 0xff if no lane provided
 *		in which case it will be executed for
 *		all the lanes assigned to the given port
 * x1[7:0]:	port#
 *
 * x2: type of loopback:
 *	0: No Loopback
 *	1: Near End Analog
 *	2: Near End Digital
 *	3: Far End Digital
 *
 * returns:
 *	x0:
 *		0 -- success
 *		negative - error
 *
 *	x1[31:24]: gserm number
 *	x1[23:8] : port lane# to gserm lane# mapping
 *	x1[7:0]  : Number of lanes assigned to the given port
 */
ssize_t smc_serdes_lpbk(int port, struct gserm_data *gserm, int type);

/**
 * Read SerDes Rx tuning parameters
 *
 * x1[15:8]:	lane# or 0xff if no lane provided
 *		in which case it will be executed for
 *		all the lanes assigned to the given port
 * x1[7:0]:	port#
 *
 * returns:
 *	x0:
 *		0 -- success
 *		negative - error
 *	x1:
 *		SERDES_SETTINGS_DATA_BASE address, where
 *		the following structure is stored:
 *		struct rx_eq_params {
 *			s32 dfe_taps[24];
 *			u32 ctle_params[13];
 *		} params[4];
 *
 *	x2[31:24]: gserm number
 *	x2[23:8] : port lane# to gserm lane# mapping
 *	x2[7:0]  : Number of lanes assigned to the given port
 *
 */
ssize_t smc_serdes_get_rx_tuning(int port, int lane,
				 void **params,
				 struct gserm_data *gserm);

/**
 * Read SerDes Tx tuning parameters
 *
 * x1[15:8]:	lane# or 0xff if no lane provided
 *		in which case it will be executed for
 *		all the lanes assigned to the given port
 * x1[7:0]:	port#
 *
 * returns:
 *	x0:
 *		0 -- success
 *		negative - error
 *
 *	x1:
 *		SERDES_SETTINGS_DATA_BASE address, where
 *		the following structure is stored:
 *		struct tx_eq_params {
 *			u16 pre2;
 *			u16 pre1;
 *			u16 post;
 *			u16 main;
 *		} params[4];
 *
 *	x2[31:24]: gserm number
 *	x2[23:8] : port lane# to gserm lane# mapping
 *	x2[7:0]  : Number of lanes assigned to the given port
 */
ssize_t smc_serdes_get_tx_tuning(int port, int lane,
				 void **params,
				 struct gserm_data *gserm);

/**
 * Write SerDes Tx tuning parameters
 *
 * x1[15:8]:	lane# or 0xff if no lane provided
 *		in which case it will be executed for
 *		all the lanes assigned to the given port
 * x1[7:0]:	port#
 *
 * x2[31:16]: pre2 parameter
 * x2[15:0]: pre1 parameter
 *
 * x3[31:16]: post parameter
 * x3[15:0]: main parameter
 *
 * x4[3]: '1' means main provided
 * x4[2]: '1' means post provided
 * x4[1]: '1' means pre1 provided
 * x4[0]: '1' means pre2 provided
 *
 * returns:
 *	x0:
 *		0 -- success
 *		negative - error
 *
 *	x2[31:24]: gserm number
 *	x2[23:8] : port lane# to gserm lane# mapping
 *	x2[7:0]  : Number of lanes assigned to the given port
 */
ssize_t smc_serdes_set_tx_tuning(int port, int lane,
				 u32 pre2_pre1,
				 u32 post_main,
				 u32 flags,
				 struct gserm_data *gserm);

#endif
