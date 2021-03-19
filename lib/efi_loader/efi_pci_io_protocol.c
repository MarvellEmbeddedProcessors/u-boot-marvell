// SPDX-License-Identifier: GPL-2.0+
/*
 *  Copyright (c) 2021 Marvell
 *
 *  EFI PCI IO protocol
 */

#include <common.h>
#include <malloc.h>
#include <efi_loader.h>
#include <linux/sizes.h>
#include <dm/device.h>
#include <dm/read.h>
#include <dm/uclass.h>
#include <asm/io.h>

const efi_guid_t efi_guid_pci_io_protocol = EFI_PCI_IO_PROTOCOL_GUID;

struct efi_pci_io_path {
	struct efi_device_path_acpi_path adp;
	struct efi_device_path end;
} __packed;

struct efi_pci_io_protocol_obj {
	struct efi_object header;
	struct efi_pci_io_protocol efi_pci_io_protocol;
	struct efi_pci_io_path *dp;
	struct udevice *pci_dev;
	u32 dbdf;
};

/*
 * Reads from the memory space of a PCI controller. Returns either when
 * the polling exit criteria is satisfied or after a defined duration.
 * @this                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @width                 Signifies the width of the memory or I/O operations.
 * @barIndex              The BAR index of the standard PCI Configuration
 *			  header to use as the base address for the memory
 *			  operation to perform.
 * @Offset                The offset within the selected BAR to start the
 *			  memory operation.
 * @Mask                  Mask used for the polling criteria.
 * @Value                 The comparison value used for polling exit criteria.
 * @Delay                 The number of 100 ns units to poll.
 * @Result                Pointer to the last value read from memory location.
 *
 * @return EFI_SUCCESS           The last data returned from the access matched
 *				 the poll exit criteria.
 * @return EFI_UNSUPPORTED       BarIndex not valid for this PCI controller.
 * @return EFI_UNSUPPORTED       Offset is not valid for the BarIndex of this
 *				 PCI controller.
 * @return EFI_TIMEOUT           Delay expired before a match occurred.
 * @return EFI_OUT_OF_RESOURCES  The request could not be completed due to a
 *				 lack of resources.
 * @return EFI_INVALID_PARAMETER One or more parameters are invalid.
 */
static efi_status_t EFIAPI poll_mem(const struct efi_pci_io_protocol *this,
				    efi_pci_io_protocol_width proto_width,
				    u8 bar_index, u64 offset, u64 mask,
				    u64 value, u64 delay, u64 *result)
{
	EFI_ENTRY("%p, %x, %x, %llx, %llx, %llx, %llx, %p", this, proto_width,
		  bar_index, offset, mask, value, delay, result);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Reads from the memory space of a PCI controller. Returns either when the
 * polling exit criteria is satisfied or after a defined duration.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Width                 Signifies the width of the memory or I/O operations.
 * @BarIndex              The BAR index of the standard PCI Configuration
 *			  header to use as the base address for the memory
 *			  operation to perform.
 * @Offset                The offset within the selected BAR to start the
 *			  memory operation.
 * @Mask                  Mask used for the polling criteria.
 * @Value                 The comparison value used for the polling exit
 *			  criteria.
 * @Delay                 The number of 100 ns units to poll.
 * @Result                Pointer to the last value read from memory location.
 *
 * @return EFI_SUCCESS           The last data returned from the access
 *				 matched the poll exit criteria.
 * @return EFI_UNSUPPORTED       BarIndex not valid for this PCI controller.
 * @return EFI_UNSUPPORTED       Offset is not valid for the BarIndex of
 *				 this PCI controller.
 * @return EFI_TIMEOUT           Delay expired before a match occurred.
 * @return EFI_OUT_OF_RESOURCES  The request could not be completed due to
 *				 a lack of resources.
 * @return EFI_INVALID_PARAMETER One or more parameters are invalid.
 */
efi_status_t EFIAPI poll_io(const struct efi_pci_io_protocol *this,
			    efi_pci_io_protocol_width proto_width,
			    u8 bar_index, u64 offset, u64 mask,
			    u64 value, u64 delay, u64 *result)
{
	EFI_ENTRY("%p, %x, %x, %llx, %llx, %llx, %llx, %p", this, proto_width,
		  bar_index, offset, mask, value, delay, result);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Enable a PCI driver to access PCI controller registers in the PCI memory
 * or I/O space.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Width                 Signifies the width of the memory or I/O operations.
 * @BarIndex              The BAR index of the standard PCI Configuration
 *			  header to use as the base address for the memory
 *			  or I/O operation to perform.
 * @Offset                The offset within the selected BAR to start the
 *			  memory or I/O operation.
 * @Count                 The number of memory or I/O operations to perform.
 * @Buffer                For read operations, the destination buffer to
 *			  store the results. For write operations, the
 *			  source buffer to write data from.
 * @return EFI_SUCCESS           The data was read from or written to the
 *				 PCI controller.
 * @return EFI_UNSUPPORTED       BarIndex not valid for this PCI controller.
 * @return EFI_UNSUPPORTED       The address range specified by Offset, Width,
 *				 and Count is not valid for the PCI BAR
 *				 specified by BarIndex.
 * @return EFI_OUT_OF_RESOURCES  The request could not be completed due to a
 *				 lack of resources.
 * @return EFI_INVALID_PARAMETER One or more parameters are invalid.
 */
efi_status_t EFIAPI mem_read(const struct efi_pci_io_protocol *this,
			     efi_pci_io_protocol_width proto_width,
			     u8 bar_index, u64 offset, u32 count,
			     void *buffer)
{
	struct efi_pci_io_protocol_obj *p_obj;
	struct udevice *pdev;
	void *addr;
	int bar;
	u64 cpu_addr;

	EFI_ENTRY("%p, %x, %x, %llx, %x, %p", this, proto_width,
		  bar_index, offset, count, buffer);

	if (!buffer || !count || proto_width >= EFIPCIIOWIDTHMAXIMUM)
		return EFI_EXIT(EFI_INVALID_PARAMETER);
	if (bar_index < 0 || bar_index > 5)
		return EFI_EXIT(EFI_UNSUPPORTED);

	p_obj = container_of(this, struct efi_pci_io_protocol_obj,
			     efi_pci_io_protocol);
	pdev = p_obj->pci_dev;

	switch (bar_index) {
	case 0:
		bar = PCI_BASE_ADDRESS_0;
		break;
	case 1:
		bar = PCI_BASE_ADDRESS_1;
		break;
	case 2:
		bar = PCI_BASE_ADDRESS_2;
		break;
	case 3:
		bar = PCI_BASE_ADDRESS_3;
		break;
	case 4:
		bar = PCI_BASE_ADDRESS_4;
		break;
	case 5:
		bar = PCI_BASE_ADDRESS_5;
		break;
	}

	addr = dm_pci_map_bar(pdev, bar, PCI_REGION_MEM);
	if (!addr)
		return EFI_EXIT(EFI_UNSUPPORTED);
	cpu_addr = (u64)addr;

	for (int i = 0; i < count; i++) {
		switch (proto_width) {
		case EFIPCIIOWIDTHUINT8:
			*(u8 *)(buffer + i) = readb(cpu_addr + offset + i);
			break;
		case EFIPCIIOWIDTHUINT16:
			*(u16 *)(buffer + 2 * i) =
					readw(cpu_addr + offset + 2 * i);
			break;
		case EFIPCIIOWIDTHUINT32:
			*(u32 *)(buffer + 4 * i) =
					readl(cpu_addr + offset + 4 * i);
			break;
		case EFIPCIIOWIDTHUINT64:
			*(u64 *)(buffer + 8 * i) =
					readq(cpu_addr + offset + 8 * i);
			break;
		case EFIPCIIOWIDTHFIFOUINT8:
			*(u8 *)(buffer + i) = readb(cpu_addr + offset);
			break;
		case EFIPCIIOWIDTHFIFOUINT16:
			*(u16 *)(buffer + 2 * i) = readw(cpu_addr + offset);
			break;
		case EFIPCIIOWIDTHFIFOUINT32:
			*(u32 *)(buffer + 4 * i) = readl(cpu_addr + offset);
			break;
		case EFIPCIIOWIDTHFIFOUINT64:
			*(u64 *)(buffer + 8 * i) = readq(cpu_addr + offset);
			break;
		case EFIPCIIOWIDTHFILLUINT8:
			*(u8 *)buffer = readb(cpu_addr + offset + i);
			break;
		case EFIPCIIOWIDTHFILLUINT16:
			*(u16 *)buffer = readw(cpu_addr + offset + 2 * i);
			break;
		case EFIPCIIOWIDTHFILLUINT32:
			*(u32 *)buffer = readl(cpu_addr + offset + 4 * i);
			break;
		case EFIPCIIOWIDTHFILLUINT64:
			*(u64 *)buffer = readq(cpu_addr + offset + 8 * i);
			break;
		default:
			break;
		}
	}

	return EFI_EXIT(EFI_SUCCESS);
}

/*
 * Enable a PCI driver to access PCI controller registers in the PCI memory or I/O space.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Width                 Signifies the width of the memory or I/O operations.
 * @BarIndex              The BAR index of the standard PCI Configuration header to use as the
 *                               base address for the memory or I/O operation to perform.
 * @Offset                The offset within the selected BAR to start the memory or I/O operation.
 * @Count                 The number of memory or I/O operations to perform.
 * @Buffer                For read operations, the destination buffer to store the results. For
 *                        write operations, the source buffer to write data from.
 * @return EFI_SUCCESS           The data was read from or written to the PCI controller.
 * @return EFI_UNSUPPORTED       BarIndex not valid for this PCI controller.
 * @return EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
 *                               valid for the PCI BAR specified by BarIndex.
 * @return EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
 * @return EFI_INVALID_PARAMETER One or more parameters are invalid.
 */
efi_status_t EFIAPI mem_write(const struct efi_pci_io_protocol *this,
			      efi_pci_io_protocol_width proto_width,
			      u8 bar_index, u64 offset, u32 count,
			      void *buffer)
{
	struct efi_pci_io_protocol_obj *p_obj;
	struct udevice *pdev;
	void *addr;
	int bar;
	u64 cpu_addr;

	EFI_ENTRY("%p, %x, %x, %llx, %x, %p", this, proto_width,
		  bar_index, offset, count, buffer);

	if (!buffer || !count || proto_width >= EFIPCIIOWIDTHMAXIMUM)
		return EFI_EXIT(EFI_INVALID_PARAMETER);
	if (bar_index < 0 || bar_index > 5)
		return EFI_EXIT(EFI_UNSUPPORTED);

	p_obj = container_of(this, struct efi_pci_io_protocol_obj,
			     efi_pci_io_protocol);
	pdev = p_obj->pci_dev;

	switch (bar_index) {
	case 0:
		bar = PCI_BASE_ADDRESS_0;
		break;
	case 1:
		bar = PCI_BASE_ADDRESS_1;
		break;
	case 2:
		bar = PCI_BASE_ADDRESS_2;
		break;
	case 3:
		bar = PCI_BASE_ADDRESS_3;
		break;
	case 4:
		bar = PCI_BASE_ADDRESS_4;
		break;
	case 5:
		bar = PCI_BASE_ADDRESS_5;
		break;
	}

	addr = dm_pci_map_bar(pdev, bar, PCI_REGION_MEM);
	if (!addr)
		return EFI_EXIT(EFI_UNSUPPORTED);
	cpu_addr = (u64)addr;

	for (int i = 0; i < count; i++) {
		switch (proto_width) {
		case EFIPCIIOWIDTHUINT8:
			writeb(*(u8 *)(buffer + i),
			       cpu_addr + offset + i);
			break;
		case EFIPCIIOWIDTHUINT16:
			writew(*(u16 *)(buffer + 2 * i),
			       cpu_addr + offset + 2 * i);
			break;
		case EFIPCIIOWIDTHUINT32:
			writel(*(u32 *)(buffer + 4 * i),
			       cpu_addr + offset + 4 * i);
			break;
		case EFIPCIIOWIDTHUINT64:
			writeq(*(u64 *)(buffer + 8 * i),
			       cpu_addr + offset + 8 * i);
			break;
		case EFIPCIIOWIDTHFIFOUINT8:
			writeb(*(u8 *)(buffer + i),
			       cpu_addr + offset);
			break;
		case EFIPCIIOWIDTHFIFOUINT16:
			writew(*(u16 *)(buffer + 2 * i),
			       cpu_addr + offset);
			break;
		case EFIPCIIOWIDTHFIFOUINT32:
			writel(*(u32 *)(buffer + 4 * i),
			       cpu_addr + offset);
			break;
		case EFIPCIIOWIDTHFIFOUINT64:
			writeq(*(u64 *)(buffer + 8 * i),
			       cpu_addr + offset);
			break;
		case EFIPCIIOWIDTHFILLUINT8:
			writeb(*(u8 *)buffer,
			       cpu_addr + offset + i);
			break;
		case EFIPCIIOWIDTHFILLUINT16:
			writew(*(u16 *)buffer,
			       cpu_addr + offset + 2 * i);
			break;
		case EFIPCIIOWIDTHFILLUINT32:
			writel(*(u32 *)buffer,
			       cpu_addr + offset + 4 * i);
			break;
		case EFIPCIIOWIDTHFILLUINT64:
			writeq(*(u64 *)buffer,
			       cpu_addr + offset + 8 * i);
			break;
		default:
			break;
		}
	}

	return EFI_EXIT(EFI_SUCCESS);
}

/*
 * Enable a PCI driver to access PCI controller registers in the PCI memory or I/O space.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Width                 Signifies the width of the memory or I/O operations.
 * @BarIndex              The BAR index of the standard PCI Configuration header to use as the
 *                               base address for the memory or I/O operation to perform.
 * @Offset                The offset within the selected BAR to start the memory or I/O operation.
 * @Count                 The number of memory or I/O operations to perform.
 * @Buffer                For read operations, the destination buffer to store the results. For
 *                        write operations, the source buffer to write data from.
 * @return EFI_SUCCESS           The data was read from or written to the PCI controller.
 * @return EFI_UNSUPPORTED       BarIndex not valid for this PCI controller.
 * @return EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
 *                               valid for the PCI BAR specified by BarIndex.
 * @return EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
 * @return EFI_INVALID_PARAMETER One or more parameters are invalid.
 */
efi_status_t EFIAPI io_read(const struct efi_pci_io_protocol *this,
			    efi_pci_io_protocol_width proto_width,
			    u8 bar_index, u64 offset, u32 count,
			    void *buffer)
{
	EFI_ENTRY("%p, %x, %x, %llx, %x, %p", this, proto_width,
		  bar_index, offset, count, buffer);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Enable a PCI driver to access PCI controller registers in the PCI memory or I/O space.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Width                 Signifies the width of the memory or I/O operations.
 * @BarIndex              The BAR index of the standard PCI Configuration header to use as the
 *                               base address for the memory or I/O operation to perform.
 * @Offset                The offset within the selected BAR to start the memory or I/O operation.
 * @Count                 The number of memory or I/O operations to perform.
 * @Buffer                For read operations, the destination buffer to store the results. For
 *                        write operations, the source buffer to write data from.
 * @return EFI_SUCCESS           The data was read from or written to the PCI controller.
 * @return EFI_UNSUPPORTED       BarIndex not valid for this PCI controller.
 * @return EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
 *                               valid for the PCI BAR specified by BarIndex.
 * @return EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
 * @return EFI_INVALID_PARAMETER One or more parameters are invalid.
 */
efi_status_t EFIAPI io_write(const struct efi_pci_io_protocol *this,
			     efi_pci_io_protocol_width proto_width,
			     u8 bar_index, u64 offset, u32 count,
			     void *buffer)
{
	EFI_ENTRY("%p, %x, %x, %llx, %x, %p", this, proto_width,
		  bar_index, offset, count, buffer);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Enable a PCI driver to access PCI controller registers in PCI configuration space.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Width                 Signifies the width of the memory operations.
 * @Offset                The offset within the PCI configuration space for the PCI controller.
 * @Count                 The number of PCI configuration operations to perform.
 * @Buffer                For read operations, the destination buffer to store the results. For
 *                        write operations, the source buffer to write data from.
 * @return EFI_SUCCESS           The data was read from or written to the PCI controller.
 * @return EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
 *                               valid for the PCI configuration header of the PCI controller.
 * @return EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
 * @return EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.
 */
efi_status_t EFIAPI cfg_read(const struct efi_pci_io_protocol *this,
			     efi_pci_io_protocol_width proto_width,
			     u32 offset, u32 count, void *buffer)
{
	struct efi_pci_io_protocol_obj *p_obj;
	struct udevice *pdev;
	u8 val8;
	u16 val16;
	u32 val32;

	EFI_ENTRY("%p, %x, %x, %x, %p", this, proto_width,
		  offset, count, buffer);

	if (proto_width == EFIPCIIOWIDTHUINT64 ||
	    proto_width == EFIPCIIOWIDTHFIFOUINT64 ||
	    proto_width == EFIPCIIOWIDTHFILLUINT64) {
		return EFI_EXIT(EFI_INVALID_PARAMETER);
	}
	if (!buffer || proto_width >= EFIPCIIOWIDTHMAXIMUM)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	p_obj = container_of(this, struct efi_pci_io_protocol_obj,
			     efi_pci_io_protocol);
	pdev = p_obj->pci_dev;

	for (int i = 0; i < count; i++) {
		switch (proto_width) {
		case EFIPCIIOWIDTHUINT8:
			dm_pci_read_config8(pdev, offset + i, &val8);
			*(u8 *)(buffer + i) = val8;
			break;
		case EFIPCIIOWIDTHUINT16:
			dm_pci_read_config16(pdev, offset + 2 * i, &val16);
			*(u16 *)(buffer + 2 * i) = val16;
			break;
		case EFIPCIIOWIDTHUINT32:
			dm_pci_read_config32(pdev, offset + 4 * i, &val32);
			*(u32 *)(buffer + 4 * i) = val32;
			break;
		case EFIPCIIOWIDTHFIFOUINT8:
			dm_pci_read_config8(pdev, offset, &val8);
			*(u8 *)(buffer + i) = val8;
			break;
		case EFIPCIIOWIDTHFIFOUINT16:
			dm_pci_read_config16(pdev, offset, &val16);
			*(u16 *)(buffer + 2 * i) = val16;
			break;
		case EFIPCIIOWIDTHFIFOUINT32:
			dm_pci_read_config32(pdev, offset, &val32);
			*(u32 *)(buffer + 4 * i) = val32;
			break;
		case EFIPCIIOWIDTHFILLUINT8:
			dm_pci_read_config8(pdev, offset + i, &val8);
			*(u8 *)(buffer) = val8;
			break;
		case EFIPCIIOWIDTHFILLUINT16:
			dm_pci_read_config16(pdev, offset + 2 * i, &val16);
			*(u16 *)(buffer) = val16;
			break;
		case EFIPCIIOWIDTHFILLUINT32:
			dm_pci_read_config32(pdev, offset + 4 * i, &val32);
			*(u32 *)(buffer) = val32;
			break;
		default:
			return EFI_EXIT(EFI_INVALID_PARAMETER);
		}
	}

	return EFI_EXIT(EFI_SUCCESS);
}

/*
 * Enable a PCI driver to access PCI controller registers in PCI configuration space.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Width                 Signifies the width of the memory operations.
 * @Offset                The offset within the PCI configuration space for the PCI controller.
 * @Count                 The number of PCI configuration operations to perform.
 * @Buffer                For read operations, the destination buffer to store the results. For
 *                        write operations, the source buffer to write data from.
 * @return EFI_SUCCESS           The data was read from or written to the PCI controller.
 * @return EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
 *                               valid for the PCI configuration header of the PCI controller.
 * @return EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
 * @return EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.
 */
efi_status_t EFIAPI cfg_write(const struct efi_pci_io_protocol *this,
			      efi_pci_io_protocol_width proto_width,
			      u32 offset, u32 count, void *buffer)
{
	struct efi_pci_io_protocol_obj *p_obj;
	struct udevice *pdev;
	unsigned long value;

	EFI_ENTRY("%p, %x, %x, %x, %p", this, proto_width,
		  offset, count, buffer);

	if (proto_width == EFIPCIIOWIDTHUINT64 ||
	    proto_width == EFIPCIIOWIDTHFIFOUINT64 ||
	    proto_width == EFIPCIIOWIDTHFILLUINT64) {
		return EFI_EXIT(EFI_INVALID_PARAMETER);
	}
	if (!buffer || proto_width >= EFIPCIIOWIDTHMAXIMUM)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	p_obj = container_of(this, struct efi_pci_io_protocol_obj,
			     efi_pci_io_protocol);
	pdev = p_obj->pci_dev;

	for (int i = 0; i < count; i++) {
		switch (proto_width) {
		case EFIPCIIOWIDTHUINT8:
			value = *(u8 *)(buffer + i);
			dm_pci_write_config8(pdev, offset + i, value);
			break;
		case EFIPCIIOWIDTHUINT16:
			value = *(u16 *)(buffer + i * 2);
			dm_pci_write_config16(pdev, offset + 2 * i, value);
			break;
		case EFIPCIIOWIDTHUINT32:
			value = *(u32 *)(buffer + 4 * i);
			dm_pci_write_config32(pdev, offset + 4 * i, value);
			break;
		case EFIPCIIOWIDTHFIFOUINT8:
			value = *(u8 *)(buffer + i);
			dm_pci_write_config8(pdev, offset, value);
			break;
		case EFIPCIIOWIDTHFIFOUINT16:
			value = *(u16 *)(buffer + i * 2);
			dm_pci_write_config16(pdev, offset, value);
			break;
		case EFIPCIIOWIDTHFIFOUINT32:
			value = *(u32 *)(buffer + 4 * i);
			dm_pci_write_config32(pdev, offset, value);
			break;
		case EFIPCIIOWIDTHFILLUINT8:
			value = *(u8 *)buffer;
			dm_pci_write_config8(pdev, offset + i, value);
			break;
		case EFIPCIIOWIDTHFILLUINT16:
			value = *(u16 *)buffer;
			dm_pci_write_config16(pdev, offset + 2 * i, value);
			break;
		case EFIPCIIOWIDTHFILLUINT32:
			value = *(u32 *)buffer;
			dm_pci_write_config32(pdev, offset + 4 * i, value);
			break;
		default:
			return EFI_EXIT(EFI_INVALID_PARAMETER);
		}
	}

	return EFI_EXIT(EFI_SUCCESS);
}

/*
 * Enables a PCI driver to copy one region of PCI memory space to another region of PCI
 * memory space.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Width                 Signifies the width of the memory operations.
 * @DestBarIndex          The BAR index in the standard PCI Configuration header to use as the
 *                               base address for the memory operation to perform.
 * @DestOffset            The destination offset within the BAR specified by DestBarIndex to
 *                               start the memory writes for the copy operation.
 * @SrcBarIndex           The BAR index in the standard PCI Configuration header to use as the
 *                               base address for the memory operation to perform.
 * @SrcOffset             The source offset within the BAR specified by SrcBarIndex to start
 *                               the memory reads for the copy operation.
 * @Count                 The number of memory operations to perform. Bytes moved is Width
 *                               size * Count, starting at DestOffset and SrcOffset.
 *
 * @return EFI_SUCCESS           The data was copied from one memory region to another memory region
 * @return EFI_UNSUPPORTED       DestBarIndex not valid for this PCI controller.
 * @return EFI_UNSUPPORTED       SrcBarIndex not valid for this PCI controller.
 * @return EFI_UNSUPPORTED       The address range specified by DestOffset, Width, and Count
 *                               is not valid for the PCI BAR specified by DestBarIndex.
 * @return EFI_UNSUPPORTED       The address range specified by SrcOffset, Width, and Count is
 *                               not valid for the PCI BAR specified by SrcBarIndex.
 * @return EFI_INVALID_PARAMETER Width is invalid.
 * @return EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
 */
efi_status_t EFIAPI copy_mem(const struct efi_pci_io_protocol *this,
			     efi_pci_io_protocol_width proto_width,
			     u8 dst_bar_index, u64 dst_offset,
			     u8 src_bar_index, u64 src_offset,
			     u32 count)
{
	EFI_ENTRY("%p, %x, %x, %llx, %x, %llx, %x", this, proto_width,
		  dst_bar_index, dst_offset, src_bar_index, src_offset, count);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Provides the PCI controller-specific addresses needed to access system memory.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Operation             Indicates if the bus master is going to read or write to system memory.
 * @HostAddress           The system memory address to map to the PCI controller.
 * @NumberOfBytes         On input the number of bytes to map. On output the number of bytes
 *                               that were mapped.
 * @DeviceAddress         The resulting map address for the bus master PCI controller to use to
 *                               access the hosts HostAddress.
 * @Mapping               A resulting value to pass to Unmap().
 *
 * @return EFI_SUCCESS           The range was mapped for the returned NumberOfBytes.
 * @return EFI_UNSUPPORTED       The HostAddress cannot be mapped as a common buffer.
 * @return EFI_INVALID_PARAMETER One or more parameters are invalid.
 * @return EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
 * @return EFI_DEVICE_ERROR      The system hardware could not map the requested address.
 */
efi_status_t EFIAPI map(const struct efi_pci_io_protocol *this,
			efi_pci_io_protocol_op op,
			void *host_address, u32 *num_bytes,
			u64 *phys_addr, void **mapping)
{
	EFI_ENTRY("%p, %x, %p, %p, %p, %p", this, op,
		  host_address, num_bytes, phys_addr, mapping);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Completes the Map() operation and releases any corresponding resources.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Mapping               The mapping value returned from Map().
 *
 * @return EFI_SUCCESS           The range was unmapped.
 * @return EFI_DEVICE_ERROR      The data was not committed to the target system memory.
 */
efi_status_t EFIAPI unmap(const struct efi_pci_io_protocol *this,
			  void *mapping)
{
	EFI_ENTRY("%p, %p", this, mapping);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Allocates pages that are suitable for an EfiPciIoOperationBusMasterCommonBuffer
 * or EfiPciOperationBusMasterCommonBuffer64 mapping.
 * @ This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @ Type                  This parameter is not used and must be ignored.
 * @ MemoryType            The type of memory to allocate, EfiBootServicesData or
 *                               EfiRuntimeServicesData.
 * @ Pages                 The number of pages to allocate.
 * @ HostAddress           A pointer to store the base system memory address of the
 *                               allocated range.
 * @ Attributes            The requested bit mask of attributes for the allocated range.
 *
 * @return EFI_SUCCESS           The requested memory pages were allocated.
 * @return EFI_UNSUPPORTED       Attributes is unsupported. The only legal attribute bits are
 *                               MEMORY_WRITE_COMBINE, MEMORY_CACHED and DUAL_ADDRESS_CYCLE.
 * @return EFI_INVALID_PARAMETER One or more parameters are invalid.
 * @return EFI_OUT_OF_RESOURCES  The memory pages could not be allocated.
 */
efi_status_t EFIAPI alloc_buffer(const struct efi_pci_io_protocol *this,
				 efi_pci_io_protocol_width proto_width,
				 int alloc_type, enum efi_mem_type mem_type,
				 u32 pages, void **host_address,
				 u64 attributes)
{
	EFI_ENTRY("%p, %x, %x, %x, %x, %p, %llx", this, proto_width,
		  alloc_type, mem_type, pages, host_address, attributes);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Frees memory that was allocated with AllocateBuffer().
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Pages                 The number of pages to free.
 * @HostAddress           The base system memory address of the allocated range.
 *
 * @return EFI_SUCCESS           The requested memory pages were freed.
 * @return EFI_INVALID_PARAMETER The memory range specified by HostAddress and Pages
 *                               was not allocated with AllocateBuffer().
 */
efi_status_t EFIAPI free_buffer(const struct efi_pci_io_protocol *this,
				u32 pages, void *host_address)
{
	EFI_ENTRY("%p, %x, %p", this, pages, host_address);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Flushes all PCI posted write transactions from a PCI host bridge to system memory.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 *
 * @return EFI_SUCCESS           The PCI posted write transactions were flushed from the PCI host
 *                               bridge to system memory.
 * @return EFI_DEVICE_ERROR      The PCI posted write transactions were not flushed from the PCI
 *                               host bridge due to a hardware error.
 */
efi_status_t EFIAPI flush(const struct efi_pci_io_protocol *this)
{
	EFI_ENTRY("%p", this);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Retrieves this PCI controller's current PCI bus number, device number
 * and function number.
 * @ This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @ SegmentNumber         The PCI controller's current PCI segment number.
 * @ BusNumber             The PCI controller's current PCI bus number.
 * @ DeviceNumber          The PCI controller's current PCI device number.
 * @ FunctionNumber        The PCI controller's current PCI function number.
 *
 * @return EFI_SUCCESS           The PCI controller location was returned.
 * @return EFI_INVALID_PARAMETER One or more parameters are invalid.
 */
efi_status_t EFIAPI get_location(const struct efi_pci_io_protocol *this,
				 u32 *segment_number, u32 *bus_number,
				 u32 *device_number, u32 *function_number)
{
	struct efi_pci_io_protocol_obj *p_obj;

	EFI_ENTRY("%p, %p, %p, %p, %p", this, segment_number,
		  bus_number, device_number, function_number);

	p_obj = container_of(this, struct efi_pci_io_protocol_obj,
			     efi_pci_io_protocol);
	if (!segment_number || !bus_number || !device_number ||
	    !function_number)
		return EFI_EXIT(EFI_INVALID_PARAMETER);

	*segment_number = (p_obj->dbdf >> 16) & 0xFF;
	*bus_number = (p_obj->dbdf >> 8) & 0xFF;
	*device_number = (p_obj->dbdf >> 3) & 0x1F;
	*function_number = p_obj->dbdf & 0x7;

	return EFI_EXIT(EFI_SUCCESS);
}

/*
 * Performs an operation on the attributes that this PCI controller supports. The operations include
 * getting the set of supported attributes, retrieving the current attributes, setting the current
 * attributes, enabling attributes, and disabling attributes.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Operation             The operation to perform on the attributes for this PCI controller.
 * @Attributes            The mask of attributes that are used for Set, Enable, and Disable
 *                               operations.
 * @Result                A pointer to the result mask of attributes that are returned for the Get
 *                               and Supported operations.
 *
 * @return EFI_SUCCESS           The operation on the PCI controller's attributes was completed.
 * @return EFI_INVALID_PARAMETER One or more parameters are invalid.
 * @return EFI_UNSUPPORTED       one or more of the bits set in
 *                               Attributes are not supported by this PCI controller or one of
 *                               its parent bridges when Operation is Set, Enable or Disable.
 */
efi_status_t EFIAPI attributes(const struct efi_pci_io_protocol *this,
			       efi_pci_io_protocol_attr_op attr_op,
			       u64 attributes, u64 *result)
{
	EFI_ENTRY("%p, %x, %llx, %p", this, attr_op, attributes, result);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Gets the attributes that this PCI controller supports setting on a BAR using
 * SetBarAttributes(), and retrieves the list of resource descriptors for a BAR.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @BarIndex              The BAR index of the standard PCI Configuration header to use as the
 *                        base address for resource range. The legal range for this field is 0..5.
 * @Supports              A pointer to the mask of attributes that this PCI controller supports
 *                               setting for this BAR with SetBarAttributes().
 * @Resources             A pointer to the resource descriptors that describe the current
 *                               configuration of this BAR of the PCI controller.
 *
 * @return EFI_SUCCESS           If Supports is not NULL, then the attributes that the PCI
 *                               controller supports are returned in Supports. If Resources
 *                               is not NULL, then the resource descriptors that the PCI
 *                               controller is currently using are returned in Resources.
 * @return EFI_INVALID_PARAMETER Both Supports and Attributes are NULL.
 * @return EFI_UNSUPPORTED       BarIndex not valid for this PCI controller.
 * @return EFI_OUT_OF_RESOURCES  There are not enough resources available to allocate
 *                               Resources.
 */
efi_status_t EFIAPI get_bar_attributes(const struct efi_pci_io_protocol *this,
				       u8 bar_index, u64 *supports, void **resources)
{
	EFI_ENTRY("%p, %x, %p, %p", this, bar_index, supports, resources);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

/*
 * Sets the attributes for a range of a BAR on a PCI controller.
 * @This                  A pointer to the EFI_PCI_IO_PROTOCOL instance.
 * @Attributes            The mask of attributes to set for the resource range specified by
 *                               BarIndex, Offset, and Length.
 * @BarIndex              The BAR index of the standard PCI Configuration header to use as the
 *                        base address for resource range. The legal range for this field is 0..5.
 * @Offset                A pointer to the BAR relative base address of the resource range to be
 *                               modified by the attributes specified by Attributes.
 * @Length                A pointer to the length of the resource range to be modified by the
 *                               attributes specified by Attributes.
 *
 * @return EFI_SUCCESS           The set of attributes specified by Attributes for the resource
 *                               range specified by BarIndex, Offset, and Length were
 *                               set on the PCI controller, and actual resource range is returned
 *                               in Offset and Length.
 * @return EFI_INVALID_PARAMETER Offset or Length is NULL.
 * @return EFI_UNSUPPORTED       BarIndex not valid for this PCI controller.
 * @return EFI_OUT_OF_RESOURCES  There are not enough resources to set the attributes on the
 *                               resource range specified by BarIndex, Offset, and
 *                               Length.
 */
efi_status_t EFIAPI set_bar_attributes(const struct efi_pci_io_protocol *this,
				       u64 attributes, u8 bar_index,
				       u64 *offset, u64 *length)
{
	EFI_ENTRY("%p, %llx, %x, %p, %p", this, attributes, bar_index,
		  offset, length);

	return EFI_EXIT(EFI_UNSUPPORTED);
}

static efi_status_t install_pci_io_protocol(struct udevice *dev, u8 domain)
{
	struct efi_pci_io_protocol_obj *proto_obj = NULL;
	efi_status_t r;

	proto_obj = calloc(1, sizeof(*proto_obj));
	if (!proto_obj) {
		debug("%s ERROR: Out of memory\n", __func__);
		return EFI_OUT_OF_RESOURCES;
	}

	/* Hook up to the device list */
	efi_add_handle(&proto_obj->header);

#if 0
	/* Fill in object data */
	proto_obj->dp = (struct efi_pci_io_path *)efi_dp_from_pci(dev);
	proto_obj->dp->segment_number = 0;//dm_pci_get_bdf(dev);
	proto_obj->dp->bus_number = PCI_BUS(dm_pci_get_bdf(dev));
	proto_obj->dp->device_number = PCI_DEV(dm_pci_get_bdf(dev));
	proto_obj->dp->function_number = PCI_FUNC(dm_pci_get_bdf(dev));

	proto_obj->dp->end.type = DEVICE_PATH_TYPE_END;
	proto_obj->dp->end.sub_type = DEVICE_PATH_SUB_TYPE_END;
	proto_obj->dp->end.length = sizeof(struct efi_device_path);

	r = efi_add_protocol(&proto_obj->header, &efi_guid_device_path, proto_obj->dp);
	if (r != EFI_SUCCESS) {
		debug("%s ERROR: Failure to add protocol\n", __func__);
		return r;
	}
#endif
	r = efi_add_protocol(&proto_obj->header, &efi_guid_pci_io_protocol,
			     &proto_obj->efi_pci_io_protocol);
	if (r != EFI_SUCCESS) {
		debug("%s ERROR: Failure to add protocol\n", __func__);
		return r;
	}

	proto_obj->efi_pci_io_protocol.poll_mem = poll_mem;
	proto_obj->efi_pci_io_protocol.poll_io = poll_io;
	proto_obj->efi_pci_io_protocol.mem.read = mem_read;
	proto_obj->efi_pci_io_protocol.mem.write = mem_write;
	proto_obj->efi_pci_io_protocol.io.read = io_read;
	proto_obj->efi_pci_io_protocol.io.write = io_write;
	proto_obj->efi_pci_io_protocol.config.read = cfg_read;
	proto_obj->efi_pci_io_protocol.config.write = cfg_write;
	proto_obj->efi_pci_io_protocol.copy_mem = copy_mem;
	proto_obj->efi_pci_io_protocol.map = map;
	proto_obj->efi_pci_io_protocol.unmap = unmap;
	proto_obj->efi_pci_io_protocol.alloc_buffer = alloc_buffer;
	proto_obj->efi_pci_io_protocol.free_buffer = free_buffer;
	proto_obj->efi_pci_io_protocol.flush = flush;
	proto_obj->efi_pci_io_protocol.get_location = get_location;
	proto_obj->efi_pci_io_protocol.attributes = attributes;
	proto_obj->efi_pci_io_protocol.get_bar_attributes = get_bar_attributes;
	proto_obj->efi_pci_io_protocol.set_bar_attributes = set_bar_attributes;

	proto_obj->efi_pci_io_protocol.rom_size = 0;
	proto_obj->efi_pci_io_protocol.rom_image = NULL;

	proto_obj->pci_dev = dev;
	proto_obj->dbdf = (domain << 16) | dm_pci_get_bdf(dev);

	return r;
}

void efi_pci_io_probe_dev(struct udevice *udev, u32 domain)
{
	efi_status_t r;
	struct udevice *dev;

	device_foreach_child(dev, udev) {
		if (device_is_on_pci_bus(dev)) {
			r = install_pci_io_protocol(dev, domain);
			if (r)
				debug("%s Unable to setup protocol for dev %p",
				      __func__, dev);
		}
		if (device_has_children(dev))
			efi_pci_io_probe_dev(dev, domain);
	}
}

void efi_pci_io_probe_domain(struct udevice *bus)
{
	u32 domain;

	dev_read_u32(bus, "linux,pci-domain", &domain);
	efi_pci_io_probe_dev(bus, domain);
}

efi_status_t efi_pci_io_protocol_register(void)
{
	struct udevice *bus;
	struct uclass *uc;

	uclass_id_foreach_dev(UCLASS_PCI, bus, uc) {
		if (dev_of_valid(bus) &&
		    device_is_compatible(bus, "pci-host-ecam-generic")) {
			if (device_has_children(bus))
				efi_pci_io_probe_domain(bus);
		}
	}
	return EFI_SUCCESS;
}
