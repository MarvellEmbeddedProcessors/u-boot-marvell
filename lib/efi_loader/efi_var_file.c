// SPDX-License-Identifier: GPL-2.0+
/*
 * File interface for UEFI variables
 *
 * Copyright (c) 2020, Heinrich Schuchardt
 */

#define LOG_CATEGORY LOGC_EFI

#include <common.h>
#include <charset.h>
#include <fs.h>
#include <log.h>
#include <malloc.h>
#include <mapmem.h>
#include <efi_loader.h>
#include <efi_variable.h>
#include <u-boot/crc.h>
#include <spi_flash.h>
#if defined(CONFIG_ARCH_CN10K) || defined(CONFIG_ARCH_OCTEONTX2)
#include <asm/arch/board.h>
#include <asm/arch/smc.h>
#endif

#define PART_STR_LEN 10
__efi_runtime_data struct efi_var_file *efi_var_mem_base = (struct efi_var_file *)EFI_VAR_MEM_BASE;
__efi_runtime_data struct efi_var_file *efi_var_mem_base_phy = (struct efi_var_file *)EFI_VAR_MEM_BASE;

#if IS_ENABLED(CONFIG_EFI_VARIABLE_IN_SPI_FLASH)
#include <spi_flash.h>
#include <asm/arch/board.h>

static struct spi_flash *flash;
__efi_runtime_data int efi_var_offset;
__efi_runtime_data int bus;
__efi_runtime_data int cs;

static efi_status_t __maybe_unused efi_init_spi_flash(void)
{
	unsigned int speed = CONFIG_SF_DEFAULT_SPEED;
	unsigned int mode = CONFIG_SF_DEFAULT_MODE;
	struct udevice *new;
	int ret;

	/* Get env variable storage device */
	board_get_env_spi_bus_cs(&bus, &cs);
	if ((bus == -1) || (cs == -1)) {
		bus = 1; /* Data flash on SPI 1:0 */
		cs = 0;
	}

	ret = spi_flash_probe_bus_cs(bus, cs, speed, mode, &new);
	if (ret) {
		printf("Failed to initialize SPI flash at %u:%u (error %d)\n",
		       bus, cs, ret);
	} else {
		flash = dev_get_uclass_priv(new);
	}

	/* Get EFI variable offset */
	board_get_env_offset(&efi_var_offset, "u-boot,efivar-offset");
	if (efi_var_offset == -1)
		efi_var_offset = CONFIG_EFI_VARIABLE_IN_SPI_FLASH_AT_OFFSET;

	return flash ? EFI_SUCCESS : ret;
}
#endif

/**
 * efi_set_blk_dev_to_system_partition() - select EFI system partition
 *
 * Set the EFI system partition as current block device.
 *
 * Return:	status code
 */
static efi_status_t __maybe_unused efi_set_blk_dev_to_system_partition(void)
{
	char part_str[PART_STR_LEN];
	int r;

	if (!efi_system_partition.if_type) {
		log_err("No EFI system partition\n");
		return EFI_DEVICE_ERROR;
	}
	snprintf(part_str, PART_STR_LEN, "%u:%u",
		 efi_system_partition.devnum, efi_system_partition.part);
	r = fs_set_blk_dev(blk_get_if_type_name(efi_system_partition.if_type),
			   part_str, FS_TYPE_ANY);
	if (r) {
		log_err("Cannot read EFI system partition\n");
		return EFI_DEVICE_ERROR;
	}
	return EFI_SUCCESS;
}

efi_status_t __efi_runtime efi_var_collect(struct efi_var_file **bufp, loff_t *lenp,
					   u32 check_attr_mask)
{
	size_t len = EFI_VAR_BUF_SIZE;
	struct efi_var_file *buf;
	struct efi_var_entry *var, *old_var;
	size_t old_var_name_length = 2;

	buf = (struct efi_var_file *)efi_var_mem_base;
	if (!buf)
		return EFI_OUT_OF_RESOURCES;
	efi_memset_runtime(buf, 0, EFI_VAR_BUF_SIZE);
	var = buf->var;
	old_var = var;
	for (;;) {
		efi_uintn_t data_length, var_name_length;
		u8 *data;
		efi_status_t ret;

		if ((uintptr_t)buf + len <=
		    (uintptr_t)var->name + old_var_name_length)
			return EFI_BUFFER_TOO_SMALL;

		var_name_length = (uintptr_t)buf + len - (uintptr_t)var->name;
		efi_memcpy_runtime(var->name, old_var->name, old_var_name_length);
		efi_memcpy_runtime(&var->guid, &old_var->guid, sizeof(efi_guid_t));
		ret = efi_get_next_variable_name_int(&var_name_length, var->name, &var->guid);
		if (ret == EFI_NOT_FOUND)
			break;

		if (ret != EFI_SUCCESS)
			return ret;

		old_var_name_length = var_name_length;
		old_var = var;

		data = (u8 *)var->name + old_var_name_length;
		data_length = (uintptr_t)buf + len - (uintptr_t)data;
		ret = efi_get_variable_int(var->name, &var->guid,
					   &var->attr, &data_length, data,
					   &var->time);
		if (ret != EFI_SUCCESS)
			return ret;

		if ((var->attr & check_attr_mask) == check_attr_mask) {
			var->length = data_length;
			var = (struct efi_var_entry *)ALIGN((uintptr_t)data + data_length, 8);
		}
	}

	buf->reserved = 0;
	buf->magic = EFI_VAR_FILE_MAGIC;
	len = (uintptr_t)var - (uintptr_t)buf;
	buf->crc32 = crc32(0, (u8 *)buf->var,
			   len - sizeof(struct efi_var_file));
	buf->length = len;
	*bufp = buf;
	*lenp = len;

	return EFI_SUCCESS;
}

/**
 * efi_var_to_file() - save non-volatile variables as file
 *
 * File ubootefi.var is created on the EFI system partion.
 *
 * Return:	status code
 */
efi_status_t __efi_runtime efi_var_to_file(void)
{
#if IS_ENABLED(CONFIG_EFI_VARIABLE_FILE_STORE) || IS_ENABLED(CONFIG_EFI_VARIABLE_IN_SPI_FLASH)
	efi_status_t ret;
	struct efi_var_file *buf;
	loff_t len;
	int r;

	ret = efi_var_collect(&buf, &len, EFI_VARIABLE_NON_VOLATILE);
	if (ret != EFI_SUCCESS)
		goto error;

#if IS_ENABLED(CONFIG_EFI_VARIABLE_IN_SPI_FLASH)
	if (systab.boottime) {
		ret = efi_init_spi_flash();
		if (ret != EFI_SUCCESS)
			goto error;

		/*
		 * VAR Buffer size is fixed for 16K so assume the file is stored
		 * at configured offset in data flash.
		 * Erase sector - 64KB usually.
		 */
		r = spi_flash_erase(flash, efi_var_offset,
				    flash->erase_size);
		if (ret != EFI_SUCCESS)
			goto error;

		r = spi_flash_write(flash, efi_var_offset,
				    len, (void *)map_to_sysmem((void *)buf));
		if (r)
			ret = EFI_DEVICE_ERROR;

	} else {
		/* SMC call to write variable store to flash device */
		ret = smc_write_efi_var((u64)efi_var_mem_base_phy,
					EFI_VAR_BUF_SIZE);
	}
#else
	loff_t actlen;

	ret = efi_set_blk_dev_to_system_partition();
	if (ret != EFI_SUCCESS)
		goto error;

	r = fs_write(EFI_VAR_FILE_NAME, map_to_sysmem(buf), 0, len, &actlen);
	if (r || len != actlen)
		ret = EFI_DEVICE_ERROR;
#endif
error:
	if (ret != EFI_SUCCESS)
		log_err("Failed to persist EFI variables\n");
	return ret;
#else
	return EFI_SUCCESS;
#endif
}

efi_status_t efi_var_restore(struct efi_var_file *buf)
{
	struct efi_var_entry *var, *last_var;
	efi_status_t ret;

	if (buf->reserved || buf->magic != EFI_VAR_FILE_MAGIC ||
	    buf->crc32 != crc32(0, (u8 *)buf->var,
				buf->length - sizeof(struct efi_var_file))) {
		log_debug("Invalid EFI variables file\n");
		return EFI_INVALID_PARAMETER;
	}

	var = buf->var;
	last_var = (struct efi_var_entry *)((u8 *)buf + buf->length);
	while (var < last_var) {
		u16 *data = var->name + u16_strlen(var->name) + 1;

		if (var->attr & EFI_VARIABLE_NON_VOLATILE && var->length) {
			ret = efi_var_mem_ins(var->name, &var->guid, var->attr,
					      var->length, data, 0, NULL,
					      var->time);
			if (ret != EFI_SUCCESS)
				log_err("Failed to set EFI variable %ls\n",
					var->name);
		}
		var = (struct efi_var_entry *)
		      ALIGN((uintptr_t)data + var->length, 8);
	}
	return EFI_SUCCESS;
}

/**
 * efi_var_from_file() - read variables from file
 *
 * File ubootefi.var is read from the EFI system partitions and the variables
 * stored in the file are created.
 *
 * In case the file does not exist yet or a variable cannot be set EFI_SUCCESS
 * is returned.
 *
 * Return:	status code
 */
efi_status_t efi_var_from_file(void)
{
#if IS_ENABLED(CONFIG_EFI_VARIABLE_FILE_STORE) || IS_ENABLED(CONFIG_EFI_VARIABLE_IN_SPI_FLASH)
	struct efi_var_file *buf;
	loff_t len;
	efi_status_t ret;
	int r;

	buf = calloc(1, EFI_VAR_BUF_SIZE);
	if (!buf) {
		log_err("Out of memory\n");
		return EFI_OUT_OF_RESOURCES;
	}

#if IS_ENABLED(CONFIG_EFI_VARIABLE_IN_SPI_FLASH)
		ret = efi_init_spi_flash();
		if (ret != EFI_SUCCESS)
			goto error;
		/*
		 * VAR Buffer size is fixed for 16K so assume the file is stored
		 * at offset configured.
		 */
		r = spi_flash_read(flash, efi_var_offset,
				   EFI_VAR_BUF_SIZE, (void *)map_to_sysmem((void *)buf));
		len = buf->length;
#else
		ret = efi_set_blk_dev_to_system_partition();
		if (ret != EFI_SUCCESS)
			goto error;
		r = fs_read(EFI_VAR_FILE_NAME, map_to_sysmem(buf), 0, EFI_VAR_BUF_SIZE,
			    &len);
#endif

	if (r || len < sizeof(struct efi_var_file)) {
		log_err("Failed to load EFI variables\n");
		goto error;
	}
	if (buf->length != len || efi_var_restore(buf) != EFI_SUCCESS)
		log_err("EFI variable store file not found\n");
error:
	free(buf);
#endif
	return EFI_SUCCESS;
}
