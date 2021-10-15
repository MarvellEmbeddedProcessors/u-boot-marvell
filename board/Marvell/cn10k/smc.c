// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/ptrace.h>
#include <asm/arch/smc.h>
#include <asm/psci.h>
#include <efi_loader.h>
#include <linux/bitops.h>
#include <asm/arch/update.h>

DECLARE_GLOBAL_DATA_PTR;

ssize_t smc_dram_size(unsigned int node)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_DRAM_SIZE;
	regs.regs[1] = node;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_disable_rvu_lfs(unsigned int node)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_DISABLE_RVU_LFS;
	regs.regs[1] = node;
	smc_call(&regs);

	return regs.regs[0];
}

/*
 * Get RVU Reserved Memory Region Info
 *
 * Return:
 *	x0:
 *		0 -- Success
 *	x1 - region start address
 *	x2 - region size
 */
int smc_rvu_rsvd_reg_info(u64 *reg_addr, u64 *reg_size)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_RVU_RSVD_REG_INFO;
	smc_call(&regs);

	*reg_addr = regs.regs[1];
	*reg_size = regs.regs[2];
	return regs.regs[0];
}

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
int smc_load_efi_img(u64 img_addr, u64 *img_size)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_LOAD_EFI_APP;
	regs.regs[1] = img_addr;
	smc_call(&regs);

	*img_size = regs.regs[1];
	return regs.regs[0];
}

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
int smc_efi_var_shared_memory(u64 *mem_addr, u64 *mem_size)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_GET_EFI_SHARED_MEM;

	smc_call(&regs);

	*mem_addr = regs.regs[1];
	*mem_size = regs.regs[2];
	return regs.regs[0];
}

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
__efi_runtime int smc_write_efi_var(u64 var_addr, u64 var_size, u32 bus, u32 cs)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_WRITE_EFI_VAR;
	regs.regs[1] = var_addr;
	regs.regs[2] = var_size;
	regs.regs[3] = bus;
	regs.regs[4] = cs;

	smc_call(&regs);

	return regs.regs[0];
}

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
unsigned long smc_sec_spi_op(u64 offset, u64 buffer, u64 size, u32 bus, u32 cs, u32 op)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SPI_SECURE_OP;
	regs.regs[1] = offset;
	regs.regs[2] = buffer;
	regs.regs[3] = size;
	regs.regs[4] = (bus << 4) | (cs & 0xF) | (op << 8);

	smc_call(&regs);

	return regs.regs[0];
}

/*
 * Perform Switch Firmware load to DRAM in ATF
 *
 * x1 - super image location
 * x2 - cm3 image location
 * x3 - ptr to store cm3 size
 *
 * Return:
 *	x0:
 *		0 -- Success
 *		-1 -- Invalid Arguments
 *		-2 -- SPI_CONFIG_ERR
 *		-3 -- SPI_MMAP_ERR
 *		-5 -- EIO
 */
int smc_load_switch_fw(u64 super_img_addr, u64 cm3_img_addr, u64 *cm3_img_size)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_LOAD_SWITCH_FW;
	regs.regs[1] = super_img_addr;
	regs.regs[2] = cm3_img_addr;
	smc_call(&regs);

	*cm3_img_size = regs.regs[1];
	return regs.regs[0];
}

/*
 * Perform SPI Update from ATF
 *
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
int smc_spi_update(const struct smc_update_descriptor *desc)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SPI_SECURE_UPDATE;
	regs.regs[1] = (uint64_t)desc;
	regs.regs[2] = sizeof(*desc);
	regs.regs[3] = 0;
	regs.regs[4] = 0;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_flsf_fw_booted(void)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_FSAFE_PR_BOOT_SUCCESS;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_flsf_clr_force_2ndry(void)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_FSAFE_CLR_FORCE_SEC;
	smc_call(&regs);

	return regs.regs[0];
}

int smc_spi_verify(struct smc_version_info *desc)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_VERIFY_FIRMWARE;
	regs.regs[1] = (u64)desc;
	regs.regs[2] = sizeof(*desc);
	regs.regs[3] = 0;
	regs.regs[4] = 0;
	smc_call(&regs);

	return regs.regs[0];
}

int smc_phy_dbg_temp_read(int eth, int lmac, int *temp)
{
	struct pt_regs regs;

	if (!temp)
		return -1;

	regs.regs[0] = PLAT_OCTEONTX_PHY_GET_TEMP;
	regs.regs[1] = eth;
	regs.regs[2] = lmac;
	smc_call(&regs);

	if (!regs.regs[0])
		*temp = regs.regs[1];

	return regs.regs[0];
}

int smc_phy_dbg_loopback_write(int eth, int lmac, int enable)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_PHY_LOOPBACK;
	regs.regs[1] = enable;
	regs.regs[2] = eth;
	regs.regs[3] = lmac;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_phy_dbg_prbs_read(int eth, int lmac, int host)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_PHY_DBG_PRBS;
	regs.regs[1] = 3;
	regs.regs[2] = host;
	regs.regs[3] = eth;
	regs.regs[4] = lmac;
	smc_call(&regs);

	return regs.regs[0];
}

int smc_phy_dbg_prbs_write(int eth, int lmac, int cmd, int host, int type)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_PHY_DBG_PRBS;
	regs.regs[1] = cmd;
	regs.regs[2] = type << 2 | 1 << 1 | host;
	regs.regs[3] = eth;
	regs.regs[4] = lmac;
	smc_call(&regs);

	return regs.regs[0];
}

int smc_phy_dbg_get_serdes_cfg(int eth, int lmac, u32 *cfg)
{
	struct pt_regs regs;

	if (!cfg)
		return -1;

	regs.regs[0] = PLAT_OCTEONTX_PHY_SERDES_CFG;
	regs.regs[1] = 0;
	regs.regs[3] = eth;
	regs.regs[4] = lmac;
	smc_call(&regs);

	if (!regs.regs[0])
		*cfg = regs.regs[1];

	return regs.regs[0];
}

int smc_phy_dbg_set_serdes_cfg(int eth, int lmac, u32 cfg)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_PHY_SERDES_CFG;
	regs.regs[1] = 1;
	regs.regs[2] = cfg;
	regs.regs[3] = eth;
	regs.regs[4] = lmac;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_phy_dbg_reg_read(int eth, int lmac, int mode,
			     int dev_page, int reg, int *val)
{
	struct pt_regs regs;

	if (!val)
		return -1;

	regs.regs[0] = PLAT_OCTEONTX_PHY_MDIO;
	regs.regs[1] = (dev_page << 2) | (mode << 1);
	regs.regs[2] = reg;
	regs.regs[3] = eth;
	regs.regs[4] = lmac;
	smc_call(&regs);

	if (!regs.regs[0])
		*val = regs.regs[1];

	return regs.regs[0];
}

int smc_phy_dbg_reg_write(int eth, int lmac, int mode,
			  int dev_page, int reg, int val)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_PHY_MDIO;
	regs.regs[1] = (dev_page << 2) | (mode << 1) | 1;
	regs.regs[2] = (val << 16) | reg;
	regs.regs[3] = eth;
	regs.regs[4] = lmac;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_serdes_prbs_start(int port,
			      struct gserm_data *gserm,
			      int gen_pattern, int check_pattern)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_PRBS;
	regs.regs[1] = (PRBS_START << 16) | (0xff << 8) | port;
	regs.regs[2] = gen_pattern;
	regs.regs[3] = check_pattern;
	regs.regs[4] = 0;
	smc_call(&regs);

	if (gserm) {
		gserm->gserm_idx = (regs.regs[2] >> 24) & 0xff;
		gserm->lanes_num = regs.regs[2] & 0xff;
		gserm->mapping = (regs.regs[2] >> 8) & 0xffff;
	}

	return regs.regs[0];
}

ssize_t smc_serdes_prbs_stop(int port, struct gserm_data *gserm,
			     int gen, int check)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_PRBS;
	regs.regs[1] = (PRBS_STOP << 16) | (0xff << 8) | port;
	regs.regs[2] = gen;
	regs.regs[3] = check;
	regs.regs[4] = 0;
	smc_call(&regs);

	if (gserm) {
		gserm->gserm_idx = (regs.regs[2] >> 24) & 0xff;
		gserm->lanes_num = regs.regs[2] & 0xff;
		gserm->mapping = (regs.regs[2] >> 8) & 0xffff;
	}

	return regs.regs[0];
}

ssize_t smc_serdes_prbs_clear(int port, struct gserm_data *gserm)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_PRBS;
	regs.regs[1] = (PRBS_CLEAR << 16) | (0xff << 8) | port;
	regs.regs[2] = 0;
	regs.regs[3] = 0;
	regs.regs[4] = 0;
	smc_call(&regs);

	if (gserm) {
		gserm->gserm_idx = (regs.regs[2] >> 24) & 0xff;
		gserm->lanes_num = regs.regs[2] & 0xff;
		gserm->mapping = (regs.regs[2] >> 8) & 0xffff;
	}

	return regs.regs[0];
}

ssize_t smc_serdes_prbs_show(int port,
			     struct gserm_data *gserm,
			     void **error_stats)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_PRBS;
	regs.regs[1] = (PRBS_SHOW << 16) | (0xff << 8) | port;
	regs.regs[2] = 0;
	regs.regs[3] = 0;
	regs.regs[4] = 0;
	smc_call(&regs);

	if (error_stats)
		*error_stats = (void *)regs.regs[1];

	if (gserm) {
		gserm->gserm_idx = (regs.regs[2] >> 24) & 0xff;
		gserm->lanes_num = regs.regs[2] & 0xff;
		gserm->mapping = (regs.regs[2] >> 8) & 0xffff;
	}

	return regs.regs[0];
}

ssize_t smc_serdes_prbs_inject(int port,
			       struct gserm_data *gserm,
			       int errors_cnt)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_PRBS;
	regs.regs[1] = (PRBS_INJECT << 16) | (0xff << 8) | port;
	regs.regs[2] = 0;
	regs.regs[3] = 0;
	regs.regs[4] = errors_cnt;
	smc_call(&regs);

	if (gserm) {
		gserm->gserm_idx = (regs.regs[2] >> 24) & 0xff;
		gserm->lanes_num = regs.regs[2] & 0xff;
		gserm->mapping = (regs.regs[2] >> 8) & 0xffff;
	}

	return regs.regs[0];
}

ssize_t smc_serdes_lpbk(int port, struct gserm_data *gserm, int type)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_LOOPBACK;
	regs.regs[1] = (0xff << 8) | port;
	regs.regs[2] = type;
	smc_call(&regs);

	if (gserm) {
		gserm->gserm_idx = (regs.regs[1] >> 24) & 0xff;
		gserm->lanes_num = regs.regs[1] & 0xff;
		gserm->mapping = (regs.regs[1] >> 8) & 0xffff;
	}

	return regs.regs[0];
}

ssize_t smc_serdes_start_rx_training(int port, int lane,
				     struct gserm_data *gserm)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_RX_TRAINING;
	regs.regs[1] = (lane << 8) | port;
	regs.regs[2] = 0;
	regs.regs[3] = 0;
	regs.regs[4] = 0;
	smc_call(&regs);

	if (gserm) {
		gserm->gserm_idx = (regs.regs[1] >> 24) & 0xff;
		gserm->lanes_num = regs.regs[1] & 0xff;
		gserm->mapping = (regs.regs[1] >> 8) & 0xffff;
	}

	return regs.regs[0];
}

ssize_t smc_serdes_check_rx_training(int port, int lane,
				     int *completed, int *res)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_RX_TRAINING;
	regs.regs[1] = (lane << 8) | port;
	regs.regs[2] = 1;
	regs.regs[3] = 0;
	regs.regs[4] = 0;
	smc_call(&regs);

	if (completed && res) {
		*completed = regs.regs[2] & 1;
		*res = (regs.regs[2] >> 1) & 1;
	}

	return regs.regs[0];
}

ssize_t smc_serdes_stop_rx_training(int port, int lane)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_RX_TRAINING;
	regs.regs[1] = (lane << 8) | port;
	regs.regs[2] = 2;
	regs.regs[3] = 0;
	regs.regs[4] = 0;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_serdes_get_rx_tuning(int port, int lane,
				 void **params,
				 struct gserm_data *gserm)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_RX_TUNING;
	regs.regs[1] = (lane << 8) | port;
	regs.regs[2] = 0;
	regs.regs[3] = 0;
	regs.regs[4] = 0;
	smc_call(&regs);

	if (params)
		*params = (void *)regs.regs[1];

	if (gserm) {
		gserm->gserm_idx = (regs.regs[2] >> 24) & 0xff;
		gserm->lanes_num = regs.regs[2] & 0xff;
		gserm->mapping = (regs.regs[2] >> 8) & 0xffff;
	}

	return regs.regs[0];
}

ssize_t smc_serdes_get_tx_tuning(int port, int lane,
				 void **params,
				 struct gserm_data *gserm)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_TX_TUNING;
	regs.regs[1] = (lane << 8) | port;
	regs.regs[2] = 0;
	regs.regs[3] = 0;
	regs.regs[4] = 0;
	smc_call(&regs);

	if (params)
		*params = (void *)regs.regs[1];

	if (gserm) {
		gserm->gserm_idx = (regs.regs[2] >> 24) & 0xff;
		gserm->lanes_num = regs.regs[2] & 0xff;
		gserm->mapping = (regs.regs[2] >> 8) & 0xffff;
	}

	return regs.regs[0];
}

ssize_t smc_serdes_set_tx_tuning(int port, int lane,
				 u32 pre2_pre1,
				 u32 post_main,
				 u32 flags,
				 struct gserm_data *gserm)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_SERDES_DBG_TX_TUNING;
	regs.regs[1] = (lane << 8) | port;
	regs.regs[2] = pre2_pre1;
	regs.regs[3] = post_main;
	regs.regs[4] = flags;
	smc_call(&regs);

	if (gserm) {
		gserm->gserm_idx = (regs.regs[2] >> 24) & 0xff;
		gserm->lanes_num = regs.regs[2] & 0xff;
		gserm->mapping = (regs.regs[2] >> 8) & 0xffff;
	}

	return regs.regs[0];
}

