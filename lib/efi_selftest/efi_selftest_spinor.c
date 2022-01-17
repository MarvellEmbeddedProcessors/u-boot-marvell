// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2020 Marvell
 *
 * efi_selftest_spinor
 *
 */

#include <efi_selftest.h>
#include <spi_flash.h>

static struct efi_boot_services *boottime;
extern const efi_guid_t efi_guid_spi_nor_flash_protocol;

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
	struct efi_spi_nor_flash_protocol *spinor;
	u8 flash_status[10];
	u8 flash_id[3];

	/* Get the handle for the partition */
	ret = boottime->locate_handle_buffer(BY_PROTOCOL,
				&efi_guid_spi_nor_flash_protocol, NULL,
				&no_handles, &handles);
	if (ret != EFI_SUCCESS) {
		efi_st_error("Failed to locate handles\n");
		return EFI_ST_FAILURE;
	}
	efi_st_printf("Detected %d devices\n", (int)no_handles);
	for (i = 0; i < no_handles; ++i) {
		ret = boottime->open_protocol(handles[i],
					      &efi_guid_spi_nor_flash_protocol,
					      (void **)&spinor, NULL, NULL,
					      EFI_OPEN_PROTOCOL_GET_PROTOCOL);
		if (ret != EFI_SUCCESS) {
			efi_st_error("[%d]Failed to open device path protocol\n", (int)i);
			continue;
		}

		if (spinor->spi_peripheral->friendly_name)
			printf("Device Name: %ls\n", spinor->spi_peripheral->friendly_name);

		ret = spinor->get_flash_id(spinor, flash_id);
		if (ret != EFI_SUCCESS && ret != EFI_UNSUPPORTED) {
			efi_st_error("[%d]Failed to read flash id[%u]\n", (int)i, (int)ret);
			continue;
		}

		ret = spinor->read_status(spinor, 1, flash_status);
		if (ret != EFI_SUCCESS && ret != EFI_UNSUPPORTED) {
			efi_st_error("[%d]Failed to read status[%u]\n", (int)i, (int)ret);
			continue;
		}

		u8 dataw[4096], datar[4096];
		memset(dataw, 0, sizeof(dataw));
		memset(datar, 0, sizeof(datar));

		ret = spinor->read_data(spinor, 0, sizeof(dataw), dataw);
		if (ret != EFI_SUCCESS) {
			efi_st_error("[%d]Failed to read data[%u]\n", (int)i, (int)ret);
			continue;
		}

		ret = spinor->erase_blocks(spinor, 0, 1);
		if (ret != EFI_SUCCESS && ret != EFI_UNSUPPORTED) {
			efi_st_error("[%d]Failed to erase data[%u]\n", (int)i, (int)ret);
			continue;
		}

		ret = spinor->write_data(spinor, 0, sizeof(dataw), dataw);
		if (ret != EFI_SUCCESS && ret != EFI_UNSUPPORTED) {
			efi_st_error("[%d]Failed to write data[%u]\n", (int)i, (int)ret);
			continue;
		}

		ret = spinor->read_data(spinor, 0, sizeof(datar), datar);
		if (ret != EFI_SUCCESS) {
			efi_st_error("[%d]Failed to read data[%u]\n", (int)i, (int)ret);
			continue;
		}

		if (memcmp(datar, dataw, sizeof(datar)))
			efi_st_printf("Test Fail for device %d\n", (int)i);
		else
			efi_st_printf("Test Pass for device %d\n", (int)i);

		ret = boottime->close_protocol(handles[i],
					      &efi_guid_spi_nor_flash_protocol,
					      NULL, NULL);
	}

	ret = boottime->free_pool(handles);
	if (ret != EFI_SUCCESS) {
		efi_st_error("Failed to free pool memory\n");
		return EFI_ST_FAILURE;
	}

	return EFI_ST_SUCCESS;
}

EFI_UNIT_TEST(flashdev) = {
	.name = "spinor device",
	.phase = EFI_EXECUTE_BEFORE_BOOTTIME_EXIT,
	.setup = setup,
	.execute = execute,
};
