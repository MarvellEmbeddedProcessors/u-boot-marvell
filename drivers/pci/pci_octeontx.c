// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <dm.h>
#include <errno.h>
#include <fdtdec.h>
#include <log.h>
#include <malloc.h>
#include <pci.h>

#include <asm/io.h>

#include <linux/ioport.h>

#include <asm/arch/soc.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * This driver supports multiple types of operations / host bridges / busses:
 *
 * OTX_ECAM: Octeon TX & TX2 ECAM (Enhanced Configuration Access Mechanism)
 *	     Used to access the internal on-chip devices which are connected
 *	     to internal buses
 * OTX_PEM:  Octeon TX PEM (PCI Express MAC)
 *	     Used to access the external (off-chip) PCI devices
 * OTX2_PEM: Octeon TX2 PEM (PCI Express MAC)
 *	     Used to access the external (off-chip) PCI devices
 */
enum {
	OTX_ECAM,
	OTX_PEM,
	OTX2_PEM,
};

/**
 * struct octeontx_pci - Driver private data
 * @type:	Device type matched via compatible (e.g. OTX_ECAM etc)
 * @cfg:	Config resource
 * @bus:	Bus resource
 */
struct octeontx_pci {
	unsigned int type;

	struct resource cfg;
	struct resource bus;
	struct resource pem;
};

static inline bool is_otx2_B0(void)
{
	u64 var;

	asm ("mrs %[rd],MIDR_EL1" : [rd] "=r" (var));
	var = (var >> 20) & 0xF;

	if (otx_is_soc(CN96XX) && var <= 1)
		return true;

	return false;
}

static uintptr_t octeontx_cfg_addr(struct octeontx_pci *pcie,
				   int bus_offs, int shift_offs,
				   pci_dev_t bdf, uint offset)
{
	u32 bus, dev, func;
	uintptr_t address;

	bus = PCI_BUS(bdf) + bus_offs;
	dev = PCI_DEV(bdf);
	func = PCI_FUNC(bdf);

	address = (bus << (20 + shift_offs)) |
		(dev << (15 + shift_offs)) |
		(func << (12 + shift_offs)) | offset;
	address += pcie->cfg.start;

	return address;
}

static ulong readl_size(uintptr_t addr, enum pci_size_t size)
{
	ulong val;

	switch (size) {
	case PCI_SIZE_8:
		val = readb(addr);
		break;
	case PCI_SIZE_16:
		val = readw(addr);
		break;
	case PCI_SIZE_32:
		val = readl(addr);
		break;
	default:
		printf("Invalid size\n");
		return -EINVAL;
	};

	return val;
}

static void writel_size(uintptr_t addr, enum pci_size_t size, ulong valuep)
{
	switch (size) {
	case PCI_SIZE_8:
		writeb(valuep, addr);
		break;
	case PCI_SIZE_16:
		writew(valuep, addr);
		break;
	case PCI_SIZE_32:
		writel(valuep, addr);
		break;
	default:
		printf("Invalid size\n");
	};
}

static bool octeontx_bdf_invalid(struct octeontx_pci *pcie, pci_dev_t bdf,
				 struct pci_controller *ctlr)
{
	if (pcie->type == OTX_PEM)
		if (((PCI_BUS(bdf) + 1 - ctlr->first_busno) == 1) && PCI_DEV(bdf) > 0)
			return true;
	if (pcie->type == OTX2_PEM)
		if (((PCI_BUS(bdf) - ctlr->first_busno) == 1) && PCI_DEV(bdf) > 0)
			return true;
	return false;
}

static int octeontx_ecam_read_config(const struct udevice *bus, pci_dev_t bdf,
				     uint offset, ulong *valuep,
				     enum pci_size_t size)
{
	struct octeontx_pci *pcie = (struct octeontx_pci *)dev_get_priv(bus);
	struct pci_controller *hose = dev_get_uclass_priv(bus);
	uintptr_t address;

	address = octeontx_cfg_addr(pcie, pcie->bus.start - hose->first_busno,
				    0, bdf, offset);
	*valuep = readl_size(address, size);

	debug("%02x.%02x.%02x: u%d %x -> %lx\n",
	      PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), size, offset, *valuep);

	return 0;
}

static int octeontx_ecam_write_config(struct udevice *bus, pci_dev_t bdf,
				      uint offset, ulong value,
				      enum pci_size_t size)
{
	struct octeontx_pci *pcie = (struct octeontx_pci *)dev_get_priv(bus);
	struct pci_controller *hose = dev_get_uclass_priv(bus);
	uintptr_t address;

	address = octeontx_cfg_addr(pcie, pcie->bus.start - hose->first_busno,
				    0, bdf, offset);
	writel_size(address, size, value);

	debug("%02x.%02x.%02x: u%d %x <- %lx\n",
	      PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), size, offset, value);

	return 0;
}

static int octeontx_pem_read_config(const struct udevice *bus, pci_dev_t bdf,
				    uint offset, ulong *valuep,
				    enum pci_size_t size)
{
	struct octeontx_pci *pcie = (struct octeontx_pci *)dev_get_priv(bus);
	struct pci_controller *hose = dev_get_uclass_priv(bus);
	uintptr_t address;
	u8 hdrtype;
	u8 pri_bus = pcie->bus.start + 1 - hose->first_busno;
	u32 bus_offs = (pri_bus << 16) | (pri_bus << 8) | (pri_bus << 0);

	address = octeontx_cfg_addr(pcie, 1 - hose->first_busno, 4,
				    bdf, 0);

	*valuep = pci_conv_32_to_size(~0UL, offset, size);

	if (octeontx_bdf_invalid(pcie, bdf, hose))
		return 0;

	*valuep = readl_size(address + offset, size);

	hdrtype = readb(address + PCI_HEADER_TYPE);
	if (hdrtype == PCI_HEADER_TYPE_BRIDGE &&
	    offset >= PCI_PRIMARY_BUS &&
	    offset <= PCI_SUBORDINATE_BUS &&
	    *valuep != pci_conv_32_to_size(~0UL, offset, size))
		*valuep -= pci_conv_32_to_size(bus_offs, offset, size);

	return 0;
}

static int octeontx_pem_write_config(struct udevice *bus, pci_dev_t bdf,
				     uint offset, ulong value,
				     enum pci_size_t size)
{
	struct octeontx_pci *pcie = (struct octeontx_pci *)dev_get_priv(bus);
	struct pci_controller *hose = dev_get_uclass_priv(bus);
	uintptr_t address;
	u8 hdrtype;
	u8 pri_bus = pcie->bus.start + 1 - hose->first_busno;
	u32 bus_offs = (pri_bus << 16) | (pri_bus << 8) | (pri_bus << 0);

	address = octeontx_cfg_addr(pcie, 1 - hose->first_busno, 4, bdf, 0);

	hdrtype = readb(address + PCI_HEADER_TYPE);
	if (hdrtype == PCI_HEADER_TYPE_BRIDGE &&
	    offset >= PCI_PRIMARY_BUS &&
	    offset <= PCI_SUBORDINATE_BUS &&
	    value != pci_conv_32_to_size(~0UL, offset, size))
		value +=  pci_conv_32_to_size(bus_offs, offset, size);

	if (octeontx_bdf_invalid(pcie, bdf, hose))
		return -EPERM;

	writel_size(address + offset, size, value);

	debug("%02x.%02x.%02x: u%d %x (%lx) <- %lx\n",
	      PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), size, offset,
	      address, value);

	return 0;
}

#define PEM_CFG_WR 0x18
#define PEM_CFG_RD 0x20
static int pci_octeontx2_pem_bridge_read(const struct udevice *bus,
					 pci_dev_t bdf,
					 uint offset, ulong *valuep,
					 enum pci_size_t size)
{
	struct octeontx_pci *pcie = (void *)dev_get_priv(bus);
	struct pci_controller *hose = dev_get_uclass_priv(bus);
	uintptr_t address;
	u32 b, d, f;
	u64 raddr, rval;
	uint where;

	b = PCI_BUS(bdf) - hose->first_busno;
	d = PCI_DEV(bdf);
	f = PCI_FUNC(bdf);
	address = (b << 20) | (d << 15) | (f << 12);
	address += pcie->cfg.start;

	debug("%s %02x.%02x.%02x: u%d %x (%lx) %lx\n",
	      __func__, b, d, f, size, offset, address, *valuep);
	raddr = pcie->pem.start + PEM_CFG_RD;

	if (PCI_DEVFN(d, f) != 0ul) {
		*valuep = pci_conv_32_to_size(~0UL, offset, size);
		return 0;
	}
	where = offset & ~3ull;
	writeq(where, raddr);
	rval = readq(raddr);
	rval >>= 32;
	where = offset;

	/* HW reset value at few config space locations are
	 * garbage, fix them.
	 */
	switch (where & ~3) {
	case 0x00: /* DevID & VenID */
		rval = 0xA02D177D;
		break;
	case 0x04:
		rval = 0x00100006;
		break;
	case 0x08:
		rval = 0x06040100;
		break;
	case 0x0c:
		rval = 0x00010000;
		break;
	case 0x18:
		rval = 0x00010100;
		break;
	default:
		break;
	}

	rval >>= (8 * (where & 3));
	switch (size) {
	case PCI_SIZE_8:
		rval &= 0xff;
		break;
	case PCI_SIZE_16:
		rval &= 0xffff;
		break;
	default:
		break;
	}
	debug("%s u%d %x %llx\n", __func__, size, offset, rval);
	*valuep = rval;

	debug("%s %02x.%02x.%02x: u%d %x (%lx) -> %lx\n",
	      __func__, b, d, f, size, offset, address + offset, *valuep);
	return 0;
}

static int octeontx2_pem_read_config(const struct udevice *bus, pci_dev_t bdf,
				     uint offset, ulong *valuep,
				     enum pci_size_t size)
{
	struct octeontx_pci *pcie = (struct octeontx_pci *)dev_get_priv(bus);
	struct pci_controller *hose = dev_get_uclass_priv(bus);
	uintptr_t address;

	address = octeontx_cfg_addr(pcie, 0 - hose->first_busno, 0,
				    bdf, 0);

	*valuep = pci_conv_32_to_size(~0UL, offset, size);

	if (octeontx_bdf_invalid(pcie, bdf, hose))
		return 0;

	*valuep = readl_size(address + offset, size);

	if (!(PCI_BUS(bdf) - hose->first_busno))
		return pci_octeontx2_pem_bridge_read(bus, bdf, offset, valuep,
						     size);

	switch (size) {
	case PCI_SIZE_8:
		debug("byte %lx\n", address + offset);
		*valuep = readb(address + offset);
		break;
	case PCI_SIZE_16:
		debug("word %lx\n", address + offset);
		*valuep = readw(address + offset);
		break;
	case PCI_SIZE_32:
		debug("long %lx\n", address + offset);
		*valuep = readl(address + offset);
		break;
	default:
		printf("Invalid size\n");
	}

	debug("%02x.%02x.%02x: u%d %x (%lx) -> %lx\n",
	      PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), size, offset,
	      address + offset, *valuep);

	return 0;
}

void pem_write_fixup(struct udevice *bus, uint offset, enum pci_size_t size)
{

#define PCIERC_RASDP_DE_ME		0x440
#define PCIERC_RASDP_EP_CTL		0x420
#define PCIERC_RAS_EINJ_EN		0x348
#define PCIERC_RAS_EINJ_CTL6PE		0x3A4
#define PCIERC_RAS_EINJ_CTL6_CMPP0	0x364
#define PCIERC_RAS_EINJ_CTL6_CMPV0	0x374
#define PCIERC_RAS_EINJ_CTL6_CHGP1	0x388
#define PCIERC_RAS_EINJ_CTL6_CHGV1	0x398

	struct octeontx_pci *pcie = (void *)dev_get_priv(bus);
	u64 rval, wval;
	u32 cfg_off, data;
	u64 raddr, waddr;
	u8 shift;

	raddr = pcie->pem.start + PEM_CFG_RD;
	waddr = pcie->pem.start + PEM_CFG_WR;

	debug("%s raddr %llx waddr %llx\n", __func__, raddr, waddr);
		cfg_off = PCIERC_RASDP_DE_ME;
		wval = cfg_off;
	debug("%s DE_ME raddr %llx wval %llx\n", __func__, raddr, wval);
		writeq(wval, raddr);
		rval = readq(raddr);
	debug("%s DE_ME raddr %llx rval %llx\n", __func__, raddr, rval);
		data = rval >> 32;
		if (data & 0x1) {
			data = (data & (~0x1));
			wval |= ((u64)data << 32);
	debug("%s DE_ME waddr %llx wval %llx\n", __func__, waddr, wval);
			writeq(wval, waddr);
		}

		cfg_off = PCIERC_RAS_EINJ_CTL6_CMPP0;
		wval = cfg_off;
		data = 0xFE000000;
		wval |= ((u64)data << 32);
	debug("%s CMPP0 waddr %llx wval %llx\n", __func__, waddr, wval);
		writeq(wval, waddr);

		cfg_off = PCIERC_RAS_EINJ_CTL6_CMPV0;
		wval = cfg_off;
		data = 0x44000000;
		wval |= ((u64)data << 32);
	debug("%s CMPV0 waddr %llx wval %llx\n", __func__, waddr, wval);
		writeq(wval, waddr);

		cfg_off = PCIERC_RAS_EINJ_CTL6_CHGP1;
		wval = cfg_off;
		data = 0xFF;
		wval |= ((u64)data << 32);
	debug("%s CHGP1 waddr %llx wval %llx\n", __func__, waddr, wval);
		writeq(wval, waddr);

	cfg_off = PCIERC_RAS_EINJ_EN;
	wval = cfg_off;
	data = 0x40;
	wval |= ((u64)data << 32);
	debug("%s EINJ_EN waddr %llx wval %llx\n", __func__, waddr, wval);
	writeq(wval, waddr);

	cfg_off = PCIERC_RAS_EINJ_CTL6PE;
	wval = cfg_off;
	data = 0x1;
	wval |= ((u64)data << 32);
	debug("%s EINJ_CTL6PE waddr %llx wval %llx\n", __func__, waddr, wval);
	writeq(wval, waddr);

	switch (size) {
	case PCI_SIZE_8:
		shift = offset % 4;
		data = (0x1 << shift);
		break;
	case PCI_SIZE_16:
		shift = (offset % 4) ? 2 : 0;
		data = (0x3 << shift);
		break;
	default:
	case PCI_SIZE_32:
		data = 0xF;
		break;
	}

	cfg_off = PCIERC_RAS_EINJ_CTL6_CHGV1;
	wval = cfg_off;
	wval |= ((u64)data << 32);
	debug("%s EINJ_CHGV1 waddr %llx <= wval %llx\n", __func__, waddr,
	      wval);
	writeq(wval, waddr);

	cfg_off = PCIERC_RASDP_EP_CTL;
	wval = cfg_off;
	wval |= ((u64)0x1 << 32);
	debug("%s EP_CTL waddr %llx <= wval %llx\n", __func__, waddr, wval);
	writeq(wval, waddr);

	wval = readq(waddr);
	debug("%s EP_CTL waddr %llx => wval %llx\n", __func__, waddr, wval);
}

static int octeontx2_pem_write_config(struct udevice *bus, pci_dev_t bdf,
				      uint offset, ulong value,
				      enum pci_size_t size)
{
	struct octeontx_pci *pcie = (struct octeontx_pci *)dev_get_priv(bus);
	struct pci_controller *hose = dev_get_uclass_priv(bus);
	uintptr_t address;

	address = octeontx_cfg_addr(pcie, 0 - hose->first_busno, 0,
				    bdf, 0);

	if (octeontx_bdf_invalid(pcie, bdf, hose))
		return -EPERM;

	if (is_otx2_B0() ||
	    (!(PCI_BUS(bdf) - hose->first_busno))) {
		/* HW issue workaround only to older silicon */
		uintptr_t addr = (address + offset) & ~0x3UL;
		u32 data;
		int tmp;

		switch (size) {
		case PCI_SIZE_8:
			tmp = (address + offset) & 0x3;
			size = PCI_SIZE_32;
			data = readl(addr);
			debug("tmp 8 long %lx %x\n", addr, data);
			tmp *= 8;
			value = (data & ~(0xFFUL << tmp)) | ((value & 0xFF) << tmp);
			break;
		case PCI_SIZE_16:
			tmp = (address + offset) & 0x3;
			size = PCI_SIZE_32;
			data = readl(addr);
			debug("tmp 16 long %lx %x\n", addr, data);
			tmp *= 8;
			value = (data & 0xFFFF) | (value << tmp);
			break;
		case PCI_SIZE_32:
			break;
		}
		debug("tmp long %lx %lx\n", addr, value);

		if (is_otx2_B0())
			pem_write_fixup(bus, offset, size);
		writel_size(addr, size, value);
	} else {
		writel_size(address + offset, size, value);
	}

	debug("%02x.%02x.%02x: u%d %x (%lx) <- %lx\n",
	      PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), size, offset,
	      address + offset, value);

	return 0;
}

int pci_octeontx_read_config(const struct udevice *bus, pci_dev_t bdf,
			     uint offset, ulong *valuep,
			     enum pci_size_t size)
{
	struct octeontx_pci *pcie = (struct octeontx_pci *)dev_get_priv(bus);
	int ret = -EIO;

	switch (pcie->type) {
	case OTX_ECAM:
		ret = octeontx_ecam_read_config(bus, bdf, offset, valuep,
						size);
		break;
	case OTX_PEM:
		ret = octeontx_pem_read_config(bus, bdf, offset, valuep,
					       size);
		break;
	case OTX2_PEM:
		ret = octeontx2_pem_read_config(bus, bdf, offset, valuep,
						size);
		break;
	}

	return ret;
}

int pci_octeontx_write_config(struct udevice *bus, pci_dev_t bdf,
			      uint offset, ulong value,
			      enum pci_size_t size)
{
	struct octeontx_pci *pcie = (struct octeontx_pci *)dev_get_priv(bus);
	int ret = -EIO;

	switch (pcie->type) {
	case OTX_ECAM:
		ret = octeontx_ecam_write_config(bus, bdf, offset, value,
						 size);
		break;
	case OTX_PEM:
		ret = octeontx_pem_write_config(bus, bdf, offset, value,
						size);
		break;
	case OTX2_PEM:
		ret = octeontx2_pem_write_config(bus, bdf, offset, value,
						 size);
		break;
	}

	return ret;
}

static int pci_octeontx_ofdata_to_platdata(struct udevice *dev)
{
	return 0;
}

static int pci_octeontx_probe(struct udevice *dev)
{
	struct octeontx_pci *pcie = (struct octeontx_pci *)dev_get_priv(dev);
	int err;

	pcie->type = dev_get_driver_data(dev);

	err = dev_read_resource(dev, 0, &pcie->cfg);
	if (err) {
		debug("Error reading resource: %s\n", fdt_strerror(err));
		return err;
	}

	if (pcie->type == OTX2_PEM) {
		err = dev_read_resource(dev, 1, &pcie->pem);
		if (err) {
			debug("Error reading resource: %s\n",
			      fdt_strerror(err));
			return err;
		}
	}

	err = dev_read_pci_bus_range(dev, &pcie->bus);
	if (err) {
		debug("Error reading resource: %x\n", err);
		return err;
	}

	return 0;
}

static const struct dm_pci_ops pci_octeontx_ops = {
	.read_config	= pci_octeontx_read_config,
	.write_config	= pci_octeontx_write_config,
};

static const struct udevice_id pci_octeontx_ids[] = {
	{ .compatible = "cavium,pci-host-thunder-ecam", .data = OTX_ECAM },
	{ .compatible = "cavium,pci-host-octeontx-ecam", .data = OTX_ECAM },
	{ .compatible = "pci-host-ecam-generic", .data = OTX_ECAM },
	{ .compatible = "cavium,pci-host-thunder-pem", .data = OTX_PEM },
	{ .compatible = "marvell,pci-host-octeontx2-pem", .data = OTX2_PEM },
	{ }
};

U_BOOT_DRIVER(pci_octeontx) = {
	.name	= "pci_octeontx",
	.id	= UCLASS_PCI,
	.of_match = pci_octeontx_ids,
	.ops	= &pci_octeontx_ops,
	.ofdata_to_platdata = pci_octeontx_ofdata_to_platdata,
	.probe	= pci_octeontx_probe,
	.priv_auto_alloc_size = sizeof(struct octeontx_pci),
	.flags = DM_FLAG_PRE_RELOC,
};
