// SPDX-License-Identifier: GPL-2.0+
/*
 *  Copyright (c) 2020 Marvell
 *
 *  EFI SPI-NOR flash protocol
 */

#include <common.h>
#include <efi_loader.h>
#include <spi_flash.h>
#include <linux/sizes.h>
#include <dm/uclass.h>
#ifdef CONFIG_ARCH_CN10K
#include <asm/arch/smc.h>
#include <asm/arch/board.h>
#endif

const efi_guid_t efi_guid_spi_nor_flash_protocol =
		EFI_SPI_NOR_FLASH_PROTOCOL_GUID;

struct efi_spi_nor_flash_protocol_obj {
	struct efi_object header;
	struct efi_spi_nor_flash_protocol efi_spi_nor_flash_protocol;
	int bus;
	int cs;
};

/*
 * Read the 3 byte manufacture and device ID from the SPI flash.
 *
 * This routine reads the 3 byte manufacture and device ID from the flash part
 * filling the buffer provided.
 *
 * @this		Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
 *                        structure.
 * @buffer	Pointer to a 3 byte buffer to receive the manufacture and
 *                     device ID.
 *
 *
 * @return EFI_SUCCESS            The manufacture and device ID was read
 *                                successfully.
 * @return EFI_INVALID_PARAMETER  Buffer is NULL
 * @return EFI_DEVICE_ERROR       Invalid data received from SPI flash part.

 */
static efi_status_t EFIAPI get_flash_id(struct efi_spi_nor_flash_protocol *this, u8 *buffer)
{
	int ret = 0;

	if (!buffer)
		return EFI_INVALID_PARAMETER;

	EFI_ENTRY("%p %p", this, buffer);
	ret = this->spi_peripheral->read_reg(this->spi_peripheral, SPINOR_OP_RDID, buffer, 3);
	if (ret < 0) {
		debug("%s(%s) error %d reading JEDEC ID\n", __func__,
		      this->spi_peripheral->dev->name, ret);
		return EFI_EXIT(EFI_DEVICE_ERROR);
	}
	return EFI_EXIT(ret);
}

static efi_status_t EFIAPI get_flash_id_secure(struct efi_spi_nor_flash_protocol *this, u8 *buffer)
{
	if (!buffer)
		return EFI_INVALID_PARAMETER;

	/* For secure SPI-NOR return 3 byte id as zero */
	memset(buffer, 0, 3);

	return EFI_SUCCESS;
}

/*
 * Read data from the SPI flash.
 *
 * This routine reads data from the SPI part in the buffer provided.
 *
 * @this	Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
 *                        structure.
 * @offset	Address in the flash to start reading
 * @len		Read length in bytes
 * @data	Address of a buffer to receive the data
 *
 * @return EFI_SUCCESS            The data was read successfully.
 * @return EFI_INVALID_PARAMETER  Buffer is NULL, or
 *                                Offset >= FlashSize, or
 *                                len > FlashSize - offset
 *
 */
static efi_status_t EFIAPI read_data(struct efi_spi_nor_flash_protocol *this,
				     u32 offset, u32 len, u8 *data)
{
	int ret = 0;
	struct dm_spi_flash_ops *ops = sf_get_ops(this->spi_peripheral->dev);

	if (!data || offset > this->spi_peripheral->size ||
	    (len > (this->spi_peripheral->size - offset)))
		return EFI_INVALID_PARAMETER;

	EFI_ENTRY("%p %d %d", this, offset, len);
	ret = ops->read(this->spi_peripheral->dev, offset, len, data);
	debug("%s(%s) len:%u status:%d\n\n", __func__,
	      this->spi_peripheral->dev->name, len, ret);
	return EFI_EXIT(ret);
}

#ifdef CONFIG_ARCH_CN10K
static efi_status_t EFIAPI read_data_secure(struct efi_spi_nor_flash_protocol *this,
					    u32 offset, u32 len, u8 *data)
{
	struct efi_spi_nor_flash_protocol_obj *proto;

	if (!data)
		return EFI_INVALID_PARAMETER;

	proto = container_of(this, struct efi_spi_nor_flash_protocol_obj,
			     efi_spi_nor_flash_protocol);
	if (!proto || (proto->bus == -1) || (proto->cs == -1))
		return EFI_DEVICE_ERROR;

	if (smc_read_secure_flash((u64)data, len, offset, proto->bus, proto->cs))
		return EFI_SECURITY_VIOLATION;

	return EFI_SUCCESS;
}
#else
static efi_status_t EFIAPI read_data_secure(struct efi_spi_nor_flash_protocol *this,
					    u32 offset, u32 len, u8 *data)
{
	return EFI_UNSUPPORTED;
}
#endif

/*
 * Write data to the SPI flash.
 *
 * This routine breaks up the write operation as necessary to write the data to
 * the SPI part.
 *
 * @this		Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
 *                        structure.
 * @offset		Address in the flash to start writing
 * @len			Write length in bytes
 * @data			Address of a buffer containing the data
 *
 * @return EFI_SUCCESS            The data was written successfully.
 * @return EFI_INVALID_PARAMETER  Buffer is NULL, or
 *                              offset >= FlashSize, or
 *                               len > FlashSize - offset
 * @return EFI_OUT_OF_RESOURCES   Insufficient memory to copy buffer.
 *
 */
static efi_status_t EFIAPI write_data(struct efi_spi_nor_flash_protocol *this,
				      u32 offset, u32 len, u8 *data)
{
	int ret = 0;
	struct dm_spi_flash_ops *ops = sf_get_ops(this->spi_peripheral->dev);

	if (!data || offset > this->spi_peripheral->size ||
	    (len > (this->spi_peripheral->size - offset)))
		return EFI_INVALID_PARAMETER;

	EFI_ENTRY("%p %d %d", this, offset, len);
	ret = ops->write(this->spi_peripheral->dev, offset, len, data);
	debug("%s(%s) len:%u status:%d\n\n", __func__,
	      this->spi_peripheral->dev->name, len, ret);
	return EFI_EXIT(ret);
}

#ifdef CONFIG_ARCH_CN10K
static efi_status_t EFIAPI write_data_secure(struct efi_spi_nor_flash_protocol *this,
					     u32 offset, u32 len, u8 *data)
{
	struct efi_spi_nor_flash_protocol_obj *proto;

	if (!data)
		return EFI_INVALID_PARAMETER;

	proto = container_of(this, struct efi_spi_nor_flash_protocol_obj,
			     efi_spi_nor_flash_protocol);
	if (!proto || (proto->bus == -1) || (proto->cs == -1))
		return EFI_DEVICE_ERROR;

	if (smc_write_secure_flash((u64)data, len, offset, proto->bus, proto->cs))
		return EFI_SECURITY_VIOLATION;

	return EFI_SUCCESS;
}
#else
static efi_status_t EFIAPI write_data_secure(struct efi_spi_nor_flash_protocol *this,
					     u32 offset, u32 len, u8 *data)
{
	return EFI_UNSUPPORTED;
}
#endif

/*
 * Read the flash status register.
 *
 * This routine reads the flash part status register.
 *
 * @this		Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
 *                        structure.
 * @num_bytes		Number of status bytes to read.
 * @status			Pointer to a buffer to receive the flash status.
 *
 * @return EFI_SUCCESS  The status register was read successfully.
 *
 */
static efi_status_t EFIAPI read_status(struct efi_spi_nor_flash_protocol *this,
				       u32 num_bytes, u8 *status)
{
	int ret = 0;

	if (!status)
		return EFI_INVALID_PARAMETER;

	EFI_ENTRY("%p %d %p", this, num_bytes, status);
	ret = this->spi_peripheral->read_reg(this->spi_peripheral, SPINOR_OP_RDSR,
					     status, num_bytes);
	if (ret < 0) {
		debug("%s(%s) error %d reading Status Register\n", __func__,
		      this->spi_peripheral->dev->name, ret);
		return EFI_EXIT(EFI_DEVICE_ERROR);
	}
	return EFI_EXIT(ret);
}

static efi_status_t EFIAPI read_status_secure(struct efi_spi_nor_flash_protocol *this,
					      u32 num_bytes, u8 *status)
{
	if (!status)
		return EFI_INVALID_PARAMETER;

	/* For secure SPI-NOR return status as zero */
	memset(status, 0, num_bytes);

	return EFI_SUCCESS;
}

/*
 * Write the flash status register.
 *
 * This routine writes the flash part status register.
 *
 * @this		Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
 *                        structure.
 * @num_bytes		Number of status bytes to write.
 * @status			Pointer to a buffer containing the new status.
 *
 * @return EFI_SUCCESS           The status write was successful.
 * @return EFI_OUT_OF_RESOURCES  Failed to allocate the write buffer.
 *
 */
static efi_status_t EFIAPI write_status(struct efi_spi_nor_flash_protocol *this,
					u32 num_bytes, u8 *status)
{
	int ret = 0;

	if (!status)
		return EFI_INVALID_PARAMETER;

	EFI_ENTRY("%p %d %p", this, num_bytes, status);
	ret = this->spi_peripheral->write_reg(this->spi_peripheral, SPINOR_OP_WRSR,
					      status, num_bytes);
	if (ret < 0) {
		debug("%s(%s) error %d writing Status Register\n", __func__,
		      this->spi_peripheral->dev->name, ret);
		return EFI_EXIT(EFI_DEVICE_ERROR);
	}
	return EFI_EXIT(ret);
}

static efi_status_t EFIAPI write_status_secure(struct efi_spi_nor_flash_protocol *this,
					       u32 num_bytes, u8 *status)
{
	if (!status)
		return EFI_INVALID_PARAMETER;

	return EFI_SUCCESS;
}

/*
 * Efficiently erases one or more blocks in the SPI flash.
 *
 * This routine uses a combination of 4 KiB and larger blocks to erase the
 * specified area.
 *
 * @this		Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
 *                        structure.
 * @offset		Address within a 4 KiB block to start erasing
 * @blk_count    Number of 4 KiB blocks to erase
 *
 * @return EFI_SUCCESS            The erase was completed successfully.
 * @return EFI_INVALID_PARAMETER  offset >= FlashSize, or
 *					blk_count * 4 KiB
 *					> FlashSize - offset
 *
 */
static efi_status_t EFIAPI erase_blocks(struct efi_spi_nor_flash_protocol *this,
					u32 offset, u32 blk_count)
{
	int ret = 0;
	u32 offset_aligned = offset & ~(SZ_4K - 1);
	struct dm_spi_flash_ops *ops = sf_get_ops(this->spi_peripheral->dev);

	if (offset > this->spi_peripheral->size ||
	    (blk_count * SZ_4K) > (this->spi_peripheral->size - offset))
		return EFI_INVALID_PARAMETER;

	EFI_ENTRY("%p %d %d", this, offset, blk_count);
	ret = ops->erase(this->spi_peripheral->dev, offset_aligned, blk_count * SZ_4K);
	debug("%s(%s) blk_count:%u status:%d\n\n", __func__,
	      this->spi_peripheral->dev->name, blk_count, ret);
	return EFI_EXIT(ret);
}

#ifdef CONFIG_ARCH_CN10K
static efi_status_t EFIAPI erase_blocks_secure(struct efi_spi_nor_flash_protocol *this,
					       u32 offset, u32 blk_count)
{
	struct efi_spi_nor_flash_protocol_obj *proto = NULL;

	proto = container_of(this, struct efi_spi_nor_flash_protocol_obj,
			     efi_spi_nor_flash_protocol);
	if (!proto || (proto->bus == -1) || (proto->cs == -1))
		return EFI_DEVICE_ERROR;

	if (smc_erase_secure_flash(blk_count * SZ_4K, offset, proto->bus, proto->cs))
		return EFI_SECURITY_VIOLATION;

	return EFI_SUCCESS;
}
#else
static efi_status_t EFIAPI erase_blocks_secure(struct efi_spi_nor_flash_protocol *this,
					       u32 offset, u32 blk_count)
{
	return EFI_UNSUPPORTED;
}
#endif

static efi_status_t install_spi_nor_flash_protocol(struct spi_flash *flash_dev,
						   int bus, int cs, int secure)
{
	struct efi_spi_nor_flash_protocol_obj *proto_obj = NULL;
	efi_status_t r;

	proto_obj = calloc(1, sizeof(*proto_obj));
	if (!proto_obj) {
		debug("%s ERROR: Out of memory\n", __func__);
		return EFI_OUT_OF_RESOURCES;
	}

	/* Hook up to the device list */
	efi_add_handle(&proto_obj->header);

	/* Fill in object data */
	r = efi_add_protocol(&proto_obj->header, &efi_guid_spi_nor_flash_protocol,
			     &proto_obj->efi_spi_nor_flash_protocol);
	if (r != EFI_SUCCESS) {
		debug("%s ERROR: Failure to add protocol\n", __func__);
		return r;
	}

	proto_obj->bus = bus;
	proto_obj->cs = cs;
	if (secure) {
		/* Secure SPI-NOR uses different set of interface calls */
		proto_obj->efi_spi_nor_flash_protocol.get_flash_id = get_flash_id_secure;
		proto_obj->efi_spi_nor_flash_protocol.lf_read_data = read_data_secure;
		proto_obj->efi_spi_nor_flash_protocol.read_data = read_data_secure;
		proto_obj->efi_spi_nor_flash_protocol.write_data = write_data_secure;
		proto_obj->efi_spi_nor_flash_protocol.read_status = read_status_secure;
		proto_obj->efi_spi_nor_flash_protocol.write_status = write_status_secure;
		proto_obj->efi_spi_nor_flash_protocol.erase_blocks = erase_blocks_secure;

		proto_obj->efi_spi_nor_flash_protocol.spi_peripheral = flash_dev;
		proto_obj->efi_spi_nor_flash_protocol.device_id[0] = 0;
		proto_obj->efi_spi_nor_flash_protocol.flash_size = 0;
		proto_obj->efi_spi_nor_flash_protocol.erase_block_size = 0;

	} else {
		proto_obj->efi_spi_nor_flash_protocol.get_flash_id = get_flash_id;
		proto_obj->efi_spi_nor_flash_protocol.lf_read_data = read_data;
		proto_obj->efi_spi_nor_flash_protocol.read_data = read_data;
		proto_obj->efi_spi_nor_flash_protocol.write_data = write_data;
		proto_obj->efi_spi_nor_flash_protocol.read_status = read_status;
		proto_obj->efi_spi_nor_flash_protocol.write_status = write_status;
		proto_obj->efi_spi_nor_flash_protocol.erase_blocks = erase_blocks;

		proto_obj->efi_spi_nor_flash_protocol.spi_peripheral = flash_dev;
		proto_obj->efi_spi_nor_flash_protocol.device_id[0] = 0;
		proto_obj->efi_spi_nor_flash_protocol.flash_size = flash_dev->size;
		proto_obj->efi_spi_nor_flash_protocol.erase_block_size =
								flash_dev->erase_size;
	}

	return r;
}

efi_status_t efi_spinor_protocol_register(void)
{
	efi_status_t r;
	struct udevice *dev = NULL;
	struct spi_flash *flash_dev;
	int index = 0, ret = 0;
	int bus[8], cs[8], num;

	num = 0;
#ifdef CONFIG_ARCH_CN10K
	/* Add secure SPI-NOR to the protocol */
	board_get_secure_spi_bus_cs(bus, cs, &num);
#endif

	debug("%s - %d Secure SPI-NOR devices detected\n", __func__, num);
	for (int i = 0; i < num; i++) {
		if ((bus[i] != -1) && (cs[i] != -1)) {
			r = install_spi_nor_flash_protocol(NULL, bus[i], cs[i], 1);
			if (r)
				debug("%s Unable to attach SPI NOR FLASH PROTOCOL to secure SPI- bus:%d cs:%d",
				      __func__, bus[i], cs[i]);
		}
	}

	do {
		ret = uclass_get_device(UCLASS_SPI_FLASH, index, &dev);
		if (ret == -ENOENT)
			continue;

		flash_dev = NULL;
		if (dev)
			flash_dev = dev_get_uclass_priv(dev);
		if (!flash_dev)
			break;

		bus[0] = -1;
		cs[0] = -1;
#ifdef CONFIG_ARCH_CN10K
		board_get_spi_bus_cs(dev, bus, cs);
#endif
		if ((bus[0] != -1) && (cs[0] != -1)) {
			r = install_spi_nor_flash_protocol(flash_dev, bus[0], cs[0], 0);
			if (r)
				debug("%s Unable to attach SPI NOR FLASH PROTOCOL to SPI-%d",
				      __func__, index);
		}
	} while (++index);

	return EFI_SUCCESS;
}
