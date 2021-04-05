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
#include <dm/uclass-internal.h>
#include <spi.h>
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
	struct efi_spi_nor_flash_protocol_obj *parent;
	struct udevice *bus, *ctrl;
	struct spi_flash *flash;
	int ret = 0;

	parent = container_of(this, struct efi_spi_nor_flash_protocol_obj,
			      efi_spi_nor_flash_protocol);

	EFI_ENTRY("%p %p", this, buffer);

	if (!buffer)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	ret = spi_find_bus_and_cs(parent->bus, parent->cs, &bus, &ctrl);
	if (ret)
		return EFI_EXIT(EFI_DEVICE_ERROR);

	flash = dev_get_uclass_priv(ctrl);

	ret = flash->read_reg(flash, SPINOR_OP_RDID,
					     buffer, 3);
	if (ret < 0) {
		debug("%s(%d:%d) error %d reading JEDEC ID\n", __func__,
		      parent->bus, parent->cs, ret);
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
	struct efi_spi_nor_flash_protocol_obj *parent;
	struct udevice *bus, *ctrl;
	struct dm_spi_flash_ops *ops;
	int ret = 0;

	parent = container_of(this, struct efi_spi_nor_flash_protocol_obj,
			      efi_spi_nor_flash_protocol);

	EFI_ENTRY("%p %d %d", this, offset, len);

	if (!data || offset > this->flash_size ||
	    (len > (this->flash_size - offset)))
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	ret = spi_find_bus_and_cs(parent->bus, parent->cs, &bus, &ctrl);
	if (ret)
		return EFI_EXIT(EFI_DEVICE_ERROR);

	ops = sf_get_ops(ctrl);
	ret = ops->read(ctrl, offset, len, data);
	if (ret < 0) {
		debug("%s(%d:%d) error %d\n", __func__,
		      parent->bus, parent->cs, ret);
		return EFI_EXIT(EFI_DEVICE_ERROR);
	}
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
	struct efi_spi_nor_flash_protocol_obj *parent;
	struct udevice *bus, *ctrl;
	struct dm_spi_flash_ops *ops;
	int ret = 0;

	parent = container_of(this, struct efi_spi_nor_flash_protocol_obj,
			      efi_spi_nor_flash_protocol);

	EFI_ENTRY("%p %d %d", this, offset, len);

	if (!data || offset > this->flash_size ||
	    (len > (this->flash_size - offset)))
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	ret = spi_find_bus_and_cs(parent->bus, parent->cs, &bus, &ctrl);
	if (ret)
		return EFI_EXIT(EFI_DEVICE_ERROR);

	ops = sf_get_ops(ctrl);
	ret = ops->write(ctrl, offset, len, data);
	if (ret < 0) {
		debug("%s(%d:%d) error %d\n", __func__,
		      parent->bus, parent->cs, ret);
		return EFI_EXIT(EFI_DEVICE_ERROR);
	}
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
	struct efi_spi_nor_flash_protocol_obj *parent;
	struct udevice *bus, *ctrl;
	struct spi_flash *flash;
	int ret = 0;

	parent = container_of(this, struct efi_spi_nor_flash_protocol_obj,
			      efi_spi_nor_flash_protocol);

	EFI_ENTRY("%p %d %p", this, num_bytes, status);

	if (!status)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	ret = spi_find_bus_and_cs(parent->bus, parent->cs, &bus, &ctrl);
	if (ret)
		return EFI_EXIT(EFI_DEVICE_ERROR);

	flash = dev_get_uclass_priv(ctrl);

	ret = flash->read_reg(flash, SPINOR_OP_RDSR, status, num_bytes);
	if (ret < 0) {
		debug("%s(%d:%d) error %d\n", __func__,
		      parent->bus, parent->cs, ret);
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
	struct efi_spi_nor_flash_protocol_obj *parent;
	struct udevice *bus, *ctrl;
	struct spi_flash *flash;
	int ret = 0;

	parent = container_of(this, struct efi_spi_nor_flash_protocol_obj,
			      efi_spi_nor_flash_protocol);

	EFI_ENTRY("%p %d %p", this, num_bytes, status);

	if (!status)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	ret = spi_find_bus_and_cs(parent->bus, parent->cs, &bus, &ctrl);
	if (ret)
		return EFI_EXIT(EFI_DEVICE_ERROR);

	flash = dev_get_uclass_priv(ctrl);

	ret = flash->write_reg(flash, SPINOR_OP_WRSR, status, num_bytes);
	if (ret < 0) {
		debug("%s(%d:%d) error %d\n", __func__,
		      parent->bus, parent->cs, ret);
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
	struct efi_spi_nor_flash_protocol_obj *parent;
	struct udevice *bus, *ctrl;
	struct dm_spi_flash_ops *ops;
	u32 offset_aligned = offset & ~(SZ_4K - 1);
	int ret = 0;

	parent = container_of(this, struct efi_spi_nor_flash_protocol_obj,
			      efi_spi_nor_flash_protocol);

	EFI_ENTRY("%p %d %d", this, offset, blk_count);

	if (offset > this->flash_size ||
	    (blk_count * SZ_4K) > (this->flash_size - offset))
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	ret = spi_find_bus_and_cs(parent->bus, parent->cs, &bus, &ctrl);
	if (ret)
		return EFI_EXIT(EFI_DEVICE_ERROR);

	ops = sf_get_ops(ctrl);
	ret = ops->erase(ctrl, offset_aligned, (blk_count * SZ_4K));
	if (ret < 0) {
		debug("%s(%d:%d) error %d\n", __func__,
		      parent->bus, parent->cs, ret);
		return EFI_EXIT(EFI_DEVICE_ERROR);
	}
	return EFI_EXIT(ret);
}

static efi_status_t install_spi_nor_flash_protocol(struct udevice *bus_dev)
{
	efi_status_t r;
	struct udevice *spi_dev;
	struct spi_flash *flash_dev;
	struct efi_spi_nor_flash_protocol_obj *proto_obj = NULL;
	struct udevice dummy_dev;
	const struct driver dummy_drv = {.id = UCLASS_SPI_FLASH};
	int bus, cs;

	/* Create SpiBus */
	struct efi_spi_bus *spi_bus = calloc(1, sizeof(struct efi_spi_bus));

	if (!spi_bus) {
		debug("%s:%d ERROR: Out of memory\n", __func__, __LINE__);
		return EFI_OUT_OF_RESOURCES;
	}
	spi_bus->friendly_name = NULL;
	spi_bus->peripheral_list = NULL;
	spi_bus->controller_path =
		efi_dp_create_device_node(DEVICE_PATH_TYPE_END,
					  DEVICE_PATH_SUB_TYPE_END, sizeof(struct efi_device_path));
	spi_bus->clock = NULL;
	spi_bus->clock_parameter = NULL;

	device_foreach_child(spi_dev, bus_dev) {
		if (spi_dev)
			flash_dev = dev_get_uclass_priv(spi_dev);
		bus = -1;
		cs = -1;
#ifdef CONFIG_ARCH_CN10K
		board_get_spi_bus_cs(spi_dev, &bus, &cs);
#endif
		if ((bus != -1) && (cs != -1)) {
			/* Create SpiPart */
			struct efi_spi_part *spi_part = calloc(1, sizeof(struct efi_spi_part));

			if (!spi_part) {
				debug("%s:%d ERROR: Out of memory\n", __func__, __LINE__);
				return EFI_OUT_OF_RESOURCES;
			}
			spi_part->max_clk_hz = flash_dev->spi->max_hz;

			/* Create spi peripheral object */
			const struct efi_spi_peripheral *spi_peripheral =
				calloc(1, sizeof(struct efi_spi_peripheral));

			if (!spi_peripheral) {
				debug("%s:%d ERROR: Out of memory\n", __func__, __LINE__);
				return EFI_OUT_OF_RESOURCES;
			}

			/* First child of spi_bus */
			if (!spi_bus->peripheral_list) {
				memcpy((void *)&spi_bus->peripheral_list, (void *)&spi_peripheral,
				       sizeof(void *));
			} else {
				struct efi_spi_peripheral *tmp;

				tmp = (struct efi_spi_peripheral *)spi_bus->peripheral_list;

				while (tmp->next_spi_peripheral)
					tmp = (struct efi_spi_peripheral *)tmp->next_spi_peripheral;
				memcpy((void *)&tmp->next_spi_peripheral,
				       (void *)&spi_peripheral, sizeof(void *));
			}

			memcpy((void *)&spi_peripheral->spi_part,
			       (void *)&spi_part, sizeof(struct spi_part *));
			memcpy((void *)&spi_peripheral->spi_bus,
			       (void *)&spi_bus, sizeof(struct spi_bus *));

			/* Create protocol object */
			proto_obj = calloc(1, sizeof(*proto_obj));
			if (!proto_obj) {
				debug("%s:%d ERROR: Out of memory\n", __func__, __LINE__);
				return EFI_OUT_OF_RESOURCES;
			}
			proto_obj->efi_spi_nor_flash_protocol.spi_peripheral = spi_peripheral;

			/* Hook up to the device list */
			efi_add_handle(&proto_obj->header);

			/* Fill in object data */
			dummy_dev.driver = &dummy_drv;
			dummy_dev.parent = NULL;
			proto_obj->dp = (struct efi_spi_nor_flash_path *)
				efi_dp_from_spi(&dummy_dev, bus, cs);

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

			r = efi_add_protocol(&proto_obj->header,
					     &efi_guid_device_path, proto_obj->dp);
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

			proto_obj->efi_spi_nor_flash_protocol.flash_size = flash_dev->size;
			proto_obj->efi_spi_nor_flash_protocol.erase_block_size =
									flash_dev->erase_size;

			proto_obj->efi_spi_nor_flash_protocol.device_id[0] = 0;
			r = flash_dev->read_reg(flash_dev, SPINOR_OP_RDID,
						proto_obj->efi_spi_nor_flash_protocol.device_id, 3);
		}
	}

	return EFI_SUCCESS;
}

efi_status_t efi_spinor_protocol_register(void)
{
	efi_status_t r;
	struct udevice *bus_dev;

	bus_dev = NULL;
	uclass_find_first_device(UCLASS_SPI, &bus_dev);
	while (bus_dev) {
		r = install_spi_nor_flash_protocol(bus_dev);
		if (r)
			debug("%s Unable to attach SPI NOR FLASH PROTOCOL to SPI",
			      __func__);

		uclass_find_next_device(&bus_dev);
	}

	return EFI_SUCCESS;
}
