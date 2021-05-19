// SPDX-License-Identifier: GPL-2.0+
/*
 *  Copyright (c) 2021 Marvell
 *
 *  EFI SWITCH CONFIGURATION protocol
 */

#include <common.h>
#include <malloc.h>
#include <efi_loader.h>
#include <linux/sizes.h>
#include <dm/device.h>
//#include <dm/read.h>
//#include <dm/uclass.h>
//#include <asm/io.h>
#include <asm/arch-cn10k/switch.h>

const efi_guid_t efi_guid_switch_config_protocol = EFI_SWITCH_CONFIG_PROTOCOL_GUID;

efi_status_t EFIAPI efi_set_switch_profile(const struct efi_switch_config_protocol *this,
					   u8 profile_num)
{
	u8 result;
	efi_status_t ret;

	EFI_ENTRY("%d", profile_num);
	if (!this)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	result = switch_cmd_opcode4(profile_num);
	switch (result) {
	case 0:
		ret = EFI_SUCCESS;
		break;
	case 16:
		debug("Invalid profile number\n");
		ret = EFI_INVALID_PARAMETER;
		break;
	case 17:
		debug("Cannot find configuration file\n");
		ret = EFI_NOT_FOUND;
		break;
	default:
		debug("Invalid return code [0x%x]\n", result);
		ret = EFI_PROTOCOL_ERROR;
		break;
	}
	return EFI_EXIT(ret);
}

efi_status_t EFIAPI efi_get_port_status(const struct efi_switch_config_protocol *this,
					u32 dev_num, u32 port_num, u16 *status)
{
	int ret;
	u32 result;
	efi_status_t efi_stat;

	EFI_ENTRY("%d %d", dev_num, port_num);

	if (!this || (port_num % 2))
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	result = 0;
	ret = switch_cmd_opcode6(dev_num, port_num, &result);
	*status = (u16)result;
	if (!ret)
		efi_stat = EFI_SUCCESS;
	else
		efi_stat = EFI_PROTOCOL_ERROR;

	return EFI_EXIT(efi_stat);
}

efi_status_t EFIAPI efi_get_mi_version(const struct efi_switch_config_protocol *this,
				       char *buffer)
{
	int ret;
	efi_status_t efi_stat;

	EFI_ENTRY("%p %p", this, buffer);

	if (!this || !buffer)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	ret = switch_cmd_opcode5(buffer);
	if (!ret)
		efi_stat = EFI_SUCCESS;
	else
		efi_stat = EFI_PROTOCOL_ERROR;

	return EFI_EXIT(efi_stat);
}

efi_status_t EFIAPI efi_get_boot_status(const struct efi_switch_config_protocol *this,
					u32 *status)
{
	int ret;
	efi_status_t efi_stat;

	EFI_ENTRY("%p %p", this, status);

	if (!this || !status)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	ret = read_virtual_reg_offset(4, status);
	if (!ret)
		efi_stat = EFI_SUCCESS;
	else
		efi_stat = EFI_PROTOCOL_ERROR;

	return EFI_EXIT(efi_stat);
}

efi_status_t EFIAPI efi_get_boot_error(const struct efi_switch_config_protocol *this,
				       u32 *error)
{
	int ret;
	efi_status_t efi_stat;

	EFI_ENTRY("%p %p", this, error);

	if (!this || !error)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	ret = read_virtual_reg_offset(8, error);
	if (!ret)
		efi_stat = EFI_SUCCESS;
	else
		efi_stat = EFI_PROTOCOL_ERROR;

	return EFI_EXIT(efi_stat);
}

efi_status_t EFIAPI efi_get_general_error(const struct efi_switch_config_protocol *this,
					  u32 *error)
{
	int ret;
	efi_status_t efi_stat;

	EFI_ENTRY("%p %p", this, error);

	if (!this || !error)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	ret = read_virtual_reg_offset(12, error);
	if (!ret)
		efi_stat = EFI_SUCCESS;
	else
		efi_stat = EFI_PROTOCOL_ERROR;

	return EFI_EXIT(efi_stat);
}

struct efi_switch_config_protocol swcfg_proto = {
	.set_config_profile = efi_set_switch_profile,
	.get_port_status = efi_get_port_status,
	.get_mi_version = efi_get_mi_version,
	.get_boot_status = efi_get_boot_status,
	.get_boot_error = efi_get_boot_error,
	.get_general_error = efi_get_general_error
};

efi_handle_t switch_handle;

efi_status_t efi_switch_config_protocol_register(void)
{
	efi_status_t r;
	/* Check if switch on PCIe bus */
	if (!get_switch_dev()) {
		debug("No switch device detected\n");
		return EFI_SUCCESS;
	}

	/* Create handles */
	r = efi_create_handle(&switch_handle);
	if (r != EFI_SUCCESS) {
		debug("%s:%d ERROR: Out of memory\n", __func__, __LINE__);
		return EFI_OUT_OF_RESOURCES;
	}

	/* Register protocol handle */
	r = efi_add_protocol(switch_handle,
			     &efi_guid_switch_config_protocol, &swcfg_proto);
	if (r != EFI_SUCCESS) {
		debug("%s ERROR: Failure to add protocol\n", __func__);
		return r;
	}

	return EFI_SUCCESS;
}
