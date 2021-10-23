// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/psci.h>
#include <asm/ptrace.h>
#include <asm/system.h>
#include <asm/arch/smc.h>
#include <efi_loader.h>

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

ssize_t smc_configure_ooo(unsigned int val)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_CONFIG_OOO;
	regs.regs[1] = val;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_configure_ooo_mask(u64 val)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_CONFIG_OOO_MASK;
	regs.regs[1] = val;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_configure_wfe_mask(u64 val)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_CONFIG_WFE_MASK;
	regs.regs[1] = val;
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

ssize_t smc_mdio_dbg_read(int cgx_lmac, int mode, int phyaddr, int devad,
			  int reg)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_MDIO_DBG_READ;
	regs.regs[1] = cgx_lmac;
	regs.regs[2] = mode;
	regs.regs[3] = phyaddr;
	regs.regs[4] = devad;
	regs.regs[5] = reg;
	smc_call(&regs);

	return regs.regs[0];
}

ssize_t smc_mdio_dbg_write(int cgx_lmac, int mode, int phyaddr, int devad,
			   int reg, int val)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX2_MDIO_DBG_WRITE;
	regs.regs[1] = cgx_lmac;
	regs.regs[2] = mode;
	regs.regs[3] = phyaddr;
	regs.regs[4] = devad;
	regs.regs[5] = reg;
	regs.regs[6] = val;
	smc_call(&regs);

	return regs.regs[0];
}

/*
 * on entry,
 *   subcmd:  one of OCTEONTX_ATTESTATION_QUERY_SUBCMD_xxx
 *   ctx_arg: subcmd-specific argument
 *
 * returns,
 *   signed value: <0 - error code
 *                  0 - success
 */
#ifdef CONFIG_CMD_ATTEST
ssize_t smc_attest(long subcmd, long ctx_arg)
{
	struct pt_regs regs;

	regs.regs[0] = OCTEONTX_ATTESTATION_QUERY;
	regs.regs[1] = ctx_arg;
	regs.regs[2] = subcmd;

	switch (subcmd) {
	/* deprecated sub-command */
	case OCTEONTX_ATTESTATION_QUERY_SUBCMD_LEGACY:
		return -1;

	case OCTEONTX_ATTESTATION_QUERY_SUBCMD_BUFFER:
	case OCTEONTX_ATTESTATION_QUERY_SUBCMD_INFO:
		/* no args */
		break;

	default:
		return -1;
	}

	smc_call(&regs);

	return regs.regs[0];
}
#endif // CONFIG_CMD_ATTEST

/*
 * Get EFI variabled shared memory info
 *
 * Return:
 *      x0:
 *              0 -- Success
 *              -1 -- Invalid Arguments
 *      x1:
 *              Physical address of the shared memory
 *      x2:
 *              Size in bytes of this shared memory
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
 *      x0:
 *              0 -- Success
 *              -1 -- Invalid Arguments
 */
__efi_runtime int smc_write_efi_var(u64 var_addr, u64 var_size)
{
	struct pt_regs regs;

	regs.regs[0] = PLAT_OCTEONTX_WRITE_EFI_VAR;
	regs.regs[1] = var_addr;
	regs.regs[2] = var_size;

	smc_call(&regs);

	return regs.regs[0];
}

