/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <mach/fw_info.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * Not all memory is mapped in the MMU. So we need to restrict the
 * memory size so that U-Boot does not try to access it. Also, the
 * internal registers are located at 0xf000.0000 - 0xffff.ffff.
 * Currently only 2GiB are mapped for system memory. This is what
 * we pass to the U-Boot subsystem here.
 */
#define USABLE_RAM_SIZE		0x80000000

ulong board_get_usable_ram_top(ulong total_size)
{
	if (gd->ram_size > USABLE_RAM_SIZE)
		return USABLE_RAM_SIZE;

	return gd->ram_size;
}

/*
 * On ARMv8, MBus is not configured in U-Boot. To enable compilation
 * of the already implemented drivers, lets add a dummy version of
 * this function so that linking does not fail.
 */
const struct mbus_dram_target_info *mvebu_mbus_dram_info(void)
{
	return NULL;
}

/* DRAM init code ... */

static const void *get_memory_reg_prop(const void *fdt, int *lenp)
{
	int offset;

	offset = fdt_path_offset(fdt, "/memory");
	if (offset < 0)
		return NULL;

	return fdt_getprop(fdt, offset, "reg", lenp);
}

__weak int mvebu_dram_init(void)
{
	const void *fdt = gd->fdt_blob;
	const fdt32_t *val;
	int ac, sc, len;

	ac = fdt_address_cells(fdt, 0);
	sc = fdt_size_cells(fdt, 0);
	if (ac < 0 || sc < 1 || sc > 2) {
		printf("invalid address/size cells\n");
		return -EINVAL;
	}

	val = get_memory_reg_prop(fdt, &len);
	if (len / sizeof(*val) < ac + sc)
		return -EINVAL;

	val += ac;

	gd->ram_size = fdtdec_get_number(val, sc);

	debug("DRAM size = %08lx\n", (unsigned long)gd->ram_size);

	return 0;
}

__weak void mvebu_dram_init_banksize(void)
{
	const void *fdt = gd->fdt_blob;
	const fdt32_t *val;
	int ac, sc, cells, len, i;

	val = get_memory_reg_prop(fdt, &len);
	if (len < 0)
		return;

	ac = fdt_address_cells(fdt, 0);
	sc = fdt_size_cells(fdt, 0);
	if (ac < 1 || sc > 2 || sc < 1 || sc > 2) {
		printf("invalid address/size cells\n");
		return;
	}

	cells = ac + sc;

	len /= sizeof(*val);

	for (i = 0; i < CONFIG_NR_DRAM_BANKS && len >= cells;
	     i++, len -= cells) {
		gd->bd->bi_dram[i].start = fdtdec_get_number(val, ac);
		val += ac;
		gd->bd->bi_dram[i].size = fdtdec_get_number(val, sc);
		val += sc;

		debug("DRAM bank %d: start = %08lx, size = %08lx\n",
		      i, (unsigned long)gd->bd->bi_dram[i].start,
		      (unsigned long)gd->bd->bi_dram[i].size);
	}
}

int dram_init(void)
{
	int ret;
	ret = mvebu_dram_init();
	return ret;
}

void dram_init_banksize(void)
{
	mvebu_dram_init_banksize();
}

int arch_cpu_init(void)
{
	/* Nothing to do (yet) */
	return 0;
}

int arch_early_init_r(void)
{
	struct udevice *dev;
	int ret;
	int i;

	/*
	 * Loop over all MISC uclass drivers to call the comphy code
	 * and init all CP110 devices enabled in the DT
	 */
	i = 0;
	while (1) {
		/* Call the comphy code via the MISC uclass driver */
		ret = uclass_get_device(UCLASS_MISC, i++, &dev);

		/* We're done, once no further CP110 device is found */
		if (ret)
			break;
	}

	i = 0;
	while (1) {
		/* Call the pinctrl code via the PINCTRL uclass driver */
		ret = uclass_get_device(UCLASS_PINCTRL, i++, &dev);

		/* We're done, once no further CP110 device is found */
		if (ret)
			break;
	}

	/* Cause the SATA devices to do their early init */
	for (uclass_first_device(UCLASS_AHCI, &dev);
	     dev;
	     uclass_next_device(&dev))
		;

#ifdef CONFIG_DM_PCI
	/* Set the top of region accessible by PCI to 2GB */
	gd->pci_ram_top = board_get_usable_ram_top(gd->ram_top);

	/* Trigger PCIe devices detection */
	pci_init();
#endif

	return 0;
}

void plat_do_sync(void)
{
	uint32_t far, el;

	el = current_el();

	if (el == 1)
		asm volatile("mrs %0, far_el1" : "=r" (far));
	else if (el == 2)
		asm volatile("mrs %0, far_el2" : "=r" (far));
	else
		asm volatile("mrs %0, far_el3" : "=r" (far));

	if (far >= ATF_REGION_START && far <= ATF_REGION_END) {
		printf("\n\tAttemp to access RT service or TEE region (addr: 0x%x, el%d)\n",
		       far, el);
		printf("\tDo not use address range 0x%x-0x%x\n\n",
		       ATF_REGION_START, ATF_REGION_END);
	}
}
