/* SPDX-License-Identifier:    GPL-2.0
 *
 * Copyright (C) 2020 Marvell.
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <dm.h>
#include <cpu.h>

/* SMBIOS spec proc family for ARM :Surce https://www.dmtf.org */
#define DMTF_BIT_64			BIT(2)
#define DMTF_MULTI_CORE			BIT(3)
#define DMTF_EXECUTE_PROTECTION		BIT(5)
#define DMTF_ENHANCED_VIRTUALIZATION	BIT(6)
#define DMTF_POWER_PERFORMANCE_CONTROL	BIT(7)

#define DMTF_PROC_VOLT_3_3V		(0x2)
#define DMTF_PROC_TYPE_CENTRAL_PROC	(0x3)
#define	DMTF_CPU_SOCKET_POPULATED	BIT(6)
#define	DMTF_CPU_ENABLED		(0x1)
#define	MAX_CPU_SPEED_2200_MHZ		(2200)

#define DMTF_PROC_CHAR (DMTF_BIT_64 | DMTF_MULTI_CORE | \
			DMTF_EXECUTE_PROTECTION | \
			DMTF_ENHANCED_VIRTUALIZATION | \
			DMTF_POWER_PERFORMANCE_CONTROL)

static inline u64 read_midr(void)
{
	u64 result;

	asm ("mrs %[rd],MIDR_EL1" : [rd] "=r" (result));
	return result;
}

int cpu_armada_bind(struct udevice *dev)
{
	u64 midr;
	u32 *p_midr = (u32 *)&midr;

	char tbd[] = "To Be Filled By O.E.M.";
	char proc_v[] = "Armada CN9130 SMP arm,cortex-a72";

	struct cpu_platdata *plat = dev_get_parent_platdata(dev);

	midr = read_midr();
	plat->id[0] = p_midr[0];
	plat->id[1] = p_midr[1];

	plat->voltage = DMTF_PROC_VOLT_3_3V;
	plat->proc_type = DMTF_PROC_TYPE_CENTRAL_PROC;
	plat->core_count = 4;
	plat->core_enabled = 4;
	plat->thread_count = 4;
	plat->proc_char = DMTF_PROC_CHAR;
	plat->max_speed = MAX_CPU_SPEED_2200_MHZ;
	plat->status = DMTF_CPU_SOCKET_POPULATED | DMTF_CPU_ENABLED;

	strncpy(plat->socket_designation, "CPU 1", 5);
	strncpy(plat->processor_version, proc_v, sizeof(proc_v));
	strncpy(plat->serial_number, tbd, sizeof(tbd));
	strncpy(plat->asset_tag, tbd, sizeof(tbd));
	strncpy(plat->part_number, tbd, sizeof(tbd));

	return 0;
}

int cpu_armada_get_desc(struct udevice *dev, char *buf, int size)
{
	struct cpu_platdata *plat = dev_get_parent_platdata(dev);

	snprintf(buf, size, plat->processor_version);
	return 0;
}

int cpu_armada_get_info(struct udevice *dev, struct cpu_info *info)
{
	struct cpu_platdata *plat = dev_get_parent_platdata(dev);

	info->cpu_freq = plat->max_speed;
	strncpy(info->cpu_type, "ARMv8", 5);
	return 0;
}

int cpu_armada_get_count(struct udevice *dev)
{
	struct cpu_platdata *plat = dev_get_parent_platdata(dev);

	/* No of cores */
	return plat->core_count;
}

int cpu_armada_get_vendor(struct udevice *dev, char *buf, int size)
{
	snprintf(buf, size, "Marvell");

	return 0;
}

static const struct cpu_ops cpu_armada_ops = {
	.get_desc = cpu_armada_get_desc,
	.get_info = cpu_armada_get_info,
	.get_count = cpu_armada_get_count,
	.get_vendor = cpu_armada_get_vendor,
};

int cpu_armada_probe(struct udevice *dev)
{
	return 0;
}

static const struct udevice_id cpu_armada_ids[] = {
	{ .compatible = "arm,cortex-a72" },
	{ }
};

U_BOOT_DRIVER(cpu_armada) = {
	.name           = "cpu_armada",
	.id             = UCLASS_CPU,
	.bind		= cpu_armada_bind,
	.ops		= &cpu_armada_ops,
	.of_match       = cpu_armada_ids,
	.probe          = cpu_armada_probe,
};
