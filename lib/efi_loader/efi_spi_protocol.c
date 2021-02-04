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
#include <asm/arch/board.h>
#endif

const efi_guid_t efi_guid_spi_nor_flash_protocol =
		EFI_SPI_NOR_FLASH_PROTOCOL_GUID;

struct efi_spi_nor_flash_path {
	struct efi_device_path dp;
	efi_guid_t guid;
	u8 vendor_data[2];
	struct efi_device_path end;
} __packed;

struct efi_spi_nor_flash_protocol_obj {
	struct efi_object header;
	struct efi_spi_nor_flash_protocol efi_spi_nor_flash_protocol;
	struct efi_spi_nor_flash_path *dp;
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
static efi_status_t EFIAPI get_flash_id(const struct efi_spi_nor_flash_protocol *this, u8 *buffer)
{
	int ret = 0;

	if (!buffer)
		return EFI_INVALID_PARAMETER;

	EFI_ENTRY("%p %p", this, buffer);
	ret = this->spi_peripheral->read_reg((struct spi_nor *)this->spi_peripheral, SPINOR_OP_RDID,
					     buffer, 3);
	if (ret < 0) {
		debug("%s(%s) error %d reading JEDEC ID\n", __func__,
		      this->spi_peripheral->dev->name, ret);
		return EFI_EXIT(EFI_DEVICE_ERROR);
	}
	return EFI_EXIT(ret);
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
static efi_status_t EFIAPI read_data(const struct efi_spi_nor_flash_protocol *this,
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
static efi_status_t EFIAPI write_data(const struct efi_spi_nor_flash_protocol *this,
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
static efi_status_t EFIAPI read_status(const struct efi_spi_nor_flash_protocol *this,
				       u32 num_bytes, u8 *status)
{
	int ret = 0;

	if (!status)
		return EFI_INVALID_PARAMETER;

	EFI_ENTRY("%p %d %p", this, num_bytes, status);
	ret = this->spi_peripheral->read_reg((struct spi_nor *)this->spi_peripheral,
					     SPINOR_OP_RDSR, status, num_bytes);
	if (ret < 0) {
		debug("%s(%s) error %d reading Status Register\n", __func__,
		      this->spi_peripheral->dev->name, ret);
		return EFI_EXIT(EFI_DEVICE_ERROR);
	}
	return EFI_EXIT(ret);
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
static efi_status_t EFIAPI write_status(const struct efi_spi_nor_flash_protocol *this,
					u32 num_bytes, u8 *status)
{
	int ret = 0;

	if (!status)
		return EFI_INVALID_PARAMETER;

	EFI_ENTRY("%p %d %p", this, num_bytes, status);
	ret = this->spi_peripheral->write_reg((struct spi_nor *)this->spi_peripheral,
					      SPINOR_OP_WRSR, status, num_bytes);
	if (ret < 0) {
		debug("%s(%s) error %d writing Status Register\n", __func__,
		      this->spi_peripheral->dev->name, ret);
		return EFI_EXIT(EFI_DEVICE_ERROR);
	}
	return EFI_EXIT(ret);
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
static efi_status_t EFIAPI erase_blocks(const struct efi_spi_nor_flash_protocol *this,
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

static efi_status_t install_spi_nor_flash_protocol(struct spi_flash *flash_dev,
						   int bus, int cs)
{
	struct efi_spi_nor_flash_protocol_obj *proto_obj = NULL;
	efi_status_t r;
	struct udevice dummy_dev;
	const struct driver dummy_drv = {.id = UCLASS_SPI_FLASH};

	proto_obj = calloc(1, sizeof(*proto_obj));
	if (!proto_obj) {
		debug("%s ERROR: Out of memory\n", __func__);
		return EFI_OUT_OF_RESOURCES;
	}

	/* Hook up to the device list */
	efi_add_handle(&proto_obj->header);

	/* Fill in object data */
	dummy_dev.driver = &dummy_drv;
	dummy_dev.parent = NULL;
	proto_obj->dp = (struct efi_spi_nor_flash_path *)efi_dp_from_spi(&dummy_dev, bus, cs);

	proto_obj->dp->vendor_data[0] = bus;
	proto_obj->dp->vendor_data[1] = cs;

	proto_obj->dp->dp.length = sizeof(struct efi_spi_nor_flash_path) -
						sizeof(struct efi_device_path);
	proto_obj->dp->end.type = DEVICE_PATH_TYPE_END;
	proto_obj->dp->end.sub_type = DEVICE_PATH_SUB_TYPE_END;
	proto_obj->dp->end.length = sizeof(struct efi_device_path);

	r = efi_add_protocol(&proto_obj->header, &efi_guid_spi_nor_flash_protocol,
			     &proto_obj->efi_spi_nor_flash_protocol);
	if (r != EFI_SUCCESS) {
		debug("%s ERROR: Failure to add protocol\n", __func__);
		return r;
	}

	r = efi_add_protocol(&proto_obj->header, &efi_guid_device_path, proto_obj->dp);
	if (r != EFI_SUCCESS) {
		debug("%s ERROR: Failure to add protocol\n", __func__);
		return r;
	}

	proto_obj->bus = bus;
	proto_obj->cs = cs;
	proto_obj->efi_spi_nor_flash_protocol.get_flash_id = get_flash_id;
	proto_obj->efi_spi_nor_flash_protocol.lf_read_data = read_data;
	proto_obj->efi_spi_nor_flash_protocol.read_data = read_data;
	proto_obj->efi_spi_nor_flash_protocol.write_data = write_data;
	proto_obj->efi_spi_nor_flash_protocol.read_status = read_status;
	proto_obj->efi_spi_nor_flash_protocol.write_status = write_status;
	proto_obj->efi_spi_nor_flash_protocol.erase_blocks = erase_blocks;

	proto_obj->efi_spi_nor_flash_protocol.spi_peripheral = flash_dev;
	proto_obj->efi_spi_nor_flash_protocol.flash_size = flash_dev->size;
	proto_obj->efi_spi_nor_flash_protocol.erase_block_size =
							flash_dev->erase_size;

	proto_obj->efi_spi_nor_flash_protocol.device_id[0] = 0;
	r = proto_obj->efi_spi_nor_flash_protocol.spi_peripheral->read_reg
		((struct spi_nor *)proto_obj->efi_spi_nor_flash_protocol.spi_peripheral,
		  SPINOR_OP_RDID, proto_obj->efi_spi_nor_flash_protocol.device_id, 3);

	return r;
}

efi_status_t efi_spinor_protocol_register(void)
{
	efi_status_t r;
	struct udevice *dev;
	struct spi_flash *flash_dev;
	int index = 0, ret = 0;
	int bus, cs;

	do {
		dev = NULL;
		ret = uclass_get_device(UCLASS_SPI_FLASH, index++, &dev);
		if (ret == -ENOENT)
			continue;

		flash_dev = NULL;
		if (dev)
			flash_dev = dev_get_uclass_priv(dev);
		if (!flash_dev)
			break;

		bus = -1;
		cs = -1;
#ifdef CONFIG_ARCH_CN10K
		board_get_spi_bus_cs(dev, &bus, &cs);
#endif
		if ((bus != -1) && (cs != -1)) {
			r = install_spi_nor_flash_protocol(flash_dev, bus, cs);
			if (r)
				debug("%s Unable to attach SPI NOR FLASH PROTOCOL to SPI-%d",
				      __func__, index);
		}
	} while (index);

	return EFI_SUCCESS;
}
