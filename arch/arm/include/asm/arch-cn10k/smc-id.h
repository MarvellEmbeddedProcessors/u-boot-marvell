/* SPDX-License-Identifier:    GPL-2.0
 *
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#ifndef __SMC_ID_H__
#define __SMC_ID_H__

/* SMC function IDs for general purpose queries */

#define OCTEONTX2_SVC_CALL_COUNT	0xc200ff00
#define OCTEONTX2_SVC_UID		0xc200ff01

#define OCTEONTX2_SVC_VERSION		0xc200ff03

/* OcteonTX Service Calls version numbers */
#define OCTEONTX2_VERSION_MAJOR	0x1
#define OCTEONTX2_VERSION_MINOR	0x0

/* x1 - node number */
#define OCTEONTX2_DRAM_SIZE		0xc2000301
#define OCTEONTX2_DISABLE_RVU_LFS	0xc2000b01

/*
 * x1 - user_buffer
 * x2 - size
 * x3 - bus
 * x4 - chip select
 *
 * Return:
 *	x0:
 *		0x0 -- Success
 *		0x2 -- Fail
 */
#define PLAT_OCTEONTX_SPI_SECURE_UPDATE		0xc2000b05

#define PLAT_OCTEONTX_LOAD_SWITCH_FW		0xc2000b06
#define PLAT_OCTEONTX_RVU_RSVD_REG_INFO		0xc2000b07
#define PLAT_OCTEONTX_LOAD_EFI_APP		0xc2000b08
#define PLAT_OCTEONTX_GET_EFI_SHARED_MEM	0xc2000b09
#define PLAT_OCTEONTX_WRITE_EFI_VAR		0xc2000b0a
#define PLAT_OCTEONTX_SPI_SECURE_OP		0xc2000b0b
#define PLAT_OCTEONTX_VERIFY_FIRMWARE		0xc2000b0c


/* fail safe */
#define OCTEONTX2_FSAFE_PR_BOOT_SUCCESS		0xc2000b02
#define OCTEONTX2_FSAFE_CLR_FORCE_SEC		0xc2000b03

#endif /* __SMC_ID_H__ */
