// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2020 Marvell
 *
 * efi_selftest_pci_io
 *
 */

#include <efi_selftest.h>

static struct efi_boot_services *boottime;
extern const efi_guid_t efi_guid_pci_io_protocol;

static int setup(const efi_handle_t handle,
		 const struct efi_system_table *systable)
{
	boottime = systable->boottime;
	return EFI_ST_SUCCESS;
}

/*
 * Execute unit test.
 *
 * @return:	EFI_ST_SUCCESS for success
 */
static int execute(void)
{
	efi_status_t ret;
	efi_uintn_t no_handles, i;
	efi_handle_t *handles;
	struct efi_pci_io_protocol *pci_io;
	u32 vend_dev = 0, cmd_sts, mem32;

	/* Get the handle for the partition */
	ret = boottime->locate_handle_buffer(BY_PROTOCOL,
				&efi_guid_pci_io_protocol, NULL,
				&no_handles, &handles);
	if (ret != EFI_SUCCESS) {
		efi_st_error("Failed to locate handles\n");
		return EFI_ST_FAILURE;
	}
	efi_st_printf("Detected %d devices\n", (int)no_handles);
	for (i = 0; i < no_handles; ++i) {
		ret = boottime->open_protocol(handles[i],
					      &efi_guid_pci_io_protocol,
					      (void **)&pci_io, NULL, NULL,
					      EFI_OPEN_PROTOCOL_GET_PROTOCOL);
		if (ret != EFI_SUCCESS) {
			efi_st_error("[%d]Failed to open device path protocol\n", (int)i);
			continue;
		}

		ret = pci_io->config.read(pci_io, EFIPCIIOWIDTHUINT32, 0x0, 1,
					  (void *)&vend_dev);
		if (ret != EFI_SUCCESS) {
			efi_st_error("Failed to read device ID\n");
			return EFI_ST_FAILURE;
		}
		printf("PCI IO Ven:Dev %x\n", vend_dev);
		efi_st_printf("Config Space Read Success\n");

		ret = pci_io->config.read(pci_io, EFIPCIIOWIDTHUINT32, 0x4, 1,
					  (void *)&cmd_sts);
		if (ret != EFI_SUCCESS) {
			efi_st_error("Failed to read command register\n");
			return EFI_ST_FAILURE;
		}
		debug("PCI IO command regval %x\n", cmd_sts);
		cmd_sts ^= (1 << 2);
		ret = pci_io->config.write(pci_io, EFIPCIIOWIDTHUINT32, 0x4, 1,
					   (void *)&cmd_sts);
		if (ret != EFI_SUCCESS) {
			efi_st_error("Failed to write command register\n");
			return EFI_ST_FAILURE;
		}
		ret = pci_io->config.read(pci_io, EFIPCIIOWIDTHUINT32, 0x4, 1,
					  (void *)&cmd_sts);
		if (ret != EFI_SUCCESS) {
			efi_st_error("Failed to read command register\n");
			return EFI_ST_FAILURE;
		}
		debug("PCI IO command regval %x\n", cmd_sts);
		cmd_sts ^= (1 << 2);
		ret = pci_io->config.write(pci_io, EFIPCIIOWIDTHUINT32, 0x4, 1,
					   (void *)&cmd_sts);
		if (ret != EFI_SUCCESS) {
			efi_st_error("Failed to write command register\n");
			return EFI_ST_FAILURE;
		}
		ret = pci_io->config.read(pci_io, EFIPCIIOWIDTHUINT32, 0x4, 1,
					  (void *)&cmd_sts);
		if (ret != EFI_SUCCESS) {
			efi_st_error("Failed to read command register\n");
			return EFI_ST_FAILURE;
		}
		debug("PCI IO command regval %x\n", cmd_sts);

		efi_st_printf("Config Space Write Success\n");

		/* Mem space test */
		ret = pci_io->config.read(pci_io, EFIPCIIOWIDTHUINT32, 0x0, 1,
					  (void *)&vend_dev);
		if (ret != EFI_SUCCESS) {
			efi_st_error("Failed to read device ID\n");
			return EFI_ST_FAILURE;
		}

		if (vend_dev == 0x210011ab) {
			ret = pci_io->mem.read(pci_io, EFIPCIIOWIDTHUINT32,
					       2, 0x50, 1, (void *)&mem32);
			if (ret != EFI_SUCCESS) {
				efi_st_error("Failed to read BAR2\n");
				return EFI_ST_FAILURE;
			}
			if (mem32 == 0x11ab)
				efi_st_printf("Memory Space Read Success\n");
			else
				efi_st_printf("Memory Space Read Failure\n");

			ret = pci_io->mem.read(pci_io, EFIPCIIOWIDTHUINT32,
					       0, 0x2c, 1, (void *)&mem32);
			if (ret != EFI_SUCCESS) {
				efi_st_error("Failed to read BAR0\n");
				return EFI_ST_FAILURE;
			}
			if (mem32 == 0x11ab11ab)
				efi_st_printf("Memory Space Read Success\n");
			else
				efi_st_printf("Memory Space Read Failure\n");

			mem32 = 0x0;
			ret = pci_io->mem.read(pci_io, EFIPCIIOWIDTHUINT32,
					       0, 0x4, 1, (void *)&mem32);
			if (ret != EFI_SUCCESS) {
				efi_st_error("Failed to read BAR0\n");
				return EFI_ST_FAILURE;
			}
			printf("mem32 BAR0 0x4 %x\n", mem32);
			mem32 ^= (1 << 2);
			ret = pci_io->mem.write(pci_io, EFIPCIIOWIDTHUINT32,
					       0, 0x4, 1, (void *)&mem32);
			if (ret != EFI_SUCCESS) {
				efi_st_error("Failed to write BAR0\n");
				return EFI_ST_FAILURE;
			}
			mem32 = 0x0;
			ret = pci_io->mem.read(pci_io, EFIPCIIOWIDTHUINT32,
					       0, 0x4, 1, (void *)&mem32);
			if (ret != EFI_SUCCESS) {
				efi_st_error("Failed to read BAR2\n");
				return EFI_ST_FAILURE;
			}
			printf("mem32 BAR0 0x4 %x\n", mem32);
		}

		efi_st_printf("Test Pass for device %d\n", (int)i);
		ret = boottime->close_protocol(handles[i],
					      &efi_guid_pci_io_protocol,
					      NULL, NULL);
	}

	ret = boottime->free_pool(handles);
	if (ret != EFI_SUCCESS) {
		efi_st_error("Failed to free pool memory\n");
		return EFI_ST_FAILURE;
	}

	return EFI_ST_SUCCESS;
}

EFI_UNIT_TEST(pcidev) = {
	.name = "pci_io device",
	.phase = EFI_EXECUTE_BEFORE_BOOTTIME_EXIT,
	.setup = setup,
	.execute = execute,
};
