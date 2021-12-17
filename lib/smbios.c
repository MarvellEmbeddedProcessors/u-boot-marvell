// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2015, Bin Meng <bmeng.cn@gmail.com>
 *
 * Adapted from coreboot src/arch/x86/smbios.c
 */

#include <common.h>
#include <env.h>
#include <mapmem.h>
#include <smbios.h>
#include <tables_csum.h>
#include <version.h>
#ifdef CONFIG_CPU
#include <cpu.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#endif
#include <linux/bitops.h>
#include <mvebu/smbios_ddr_info.h>
#include <dm/ofnode.h>

static u32 smbios_struct_count;
static ulong offset_type4;

/**
 * smbios_add_string() - add a string to the string area
 *
 * This adds a string to the string area which is appended directly after
 * the formatted portion of an SMBIOS structure.
 *
 * @start:	string area start address
 * @str:	string to add
 * @return:	string number in the string area
 */
static int smbios_add_string(char *start, const char *str)
{
	int i = 1;
	char *p = start;

	if (str == NULL)
		return 0;

	if (!*str)
		str = "Unknown";

	for (;;) {
		if (!*p) {
			strcpy(p, str);
			p += strlen(str);
			*p++ = '\0';
			*p++ = '\0';

			return i;
		}

		if (!strcmp(p, str))
			return i;

		p += strlen(p) + 1;
		i++;
	}
}

/**
 * smbios_string_table_len() - compute the string area size
 *
 * This computes the size of the string area including the string terminator.
 *
 * @start:	string area start address
 * @return:	string area size
 */
static int smbios_string_table_len(char *start)
{
	char *p = start;
	int i, len = 0;

	while (*p) {
		i = strlen(p) + 1;
		p += i;
		len += i;
	}

	/* if there is no strings, return soze of eos which is 2 */
	if (!len)
		return 2;

	return len + 1;
}

static int smbios_write_type0(ulong *current, int handle)
{
	struct smbios_type0 *t;
	int len = sizeof(struct smbios_type0);

	t = map_sysmem(*current, len);
	memset(t, 0, sizeof(struct smbios_type0));
	fill_smbios_header(t, SMBIOS_BIOS_INFORMATION, len, handle);
	t->vendor = smbios_add_string(t->eos, "U-Boot");
	t->bios_ver = smbios_add_string(t->eos, PLAIN_VERSION);
	t->bios_release_date = smbios_add_string(t->eos, U_BOOT_DMI_DATE);
#ifdef CONFIG_ROM_SIZE
	t->bios_rom_size = (CONFIG_ROM_SIZE / 65536) - 1;
#endif
	t->bios_characteristics = BIOS_CHARACTERISTICS_PCI_SUPPORTED |
				  BIOS_CHARACTERISTICS_SELECTABLE_BOOT |
				  BIOS_CHARACTERISTICS_UPGRADEABLE;
#ifdef CONFIG_GENERATE_ACPI_TABLE
	t->bios_characteristics_ext1 = BIOS_CHARACTERISTICS_EXT1_ACPI;
#endif
#ifdef CONFIG_EFI_LOADER
	t->bios_characteristics_ext2 |= BIOS_CHARACTERISTICS_EXT1_UEFI;
#endif
	t->bios_characteristics_ext2 = BIOS_CHARACTERISTICS_EXT2_TARGET;

	t->bios_major_release = 0xff;
	t->bios_minor_release = 0xff;
	t->ec_major_release = 0xff;
	t->ec_minor_release = 0xff;

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	unmap_sysmem(t);

	smbios_struct_count++;
	return len;
}

static int smbios_write_type1(ulong *current, int handle)
{
	struct smbios_type1 *t;
	ofnode node_type1;
	u32 tmp;
	int len = sizeof(struct smbios_type1);
	char *serial_str = env_get("serial#");

	t = map_sysmem(*current, len);
	memset(t, 0, sizeof(struct smbios_type1));
	fill_smbios_header(t, SMBIOS_SYSTEM_INFORMATION, len, handle);
	t->manufacturer = smbios_add_string(t->eos, CONFIG_SMBIOS_MANUFACTURER);
	t->product_name = smbios_add_string(t->eos, CONFIG_SMBIOS_PRODUCT_NAME);
	if (serial_str) {
		strncpy((char *)t->uuid, serial_str, sizeof(t->uuid));
		t->serial_number = smbios_add_string(t->eos, serial_str);
	}

	node_type1 = ofnode_path("/uboot-smbios/type1");
	if (ofnode_valid(node_type1)) {
		tmp = 0x01;
		ofnode_read_u32(node_type1, "wakeup-type", &tmp);
		t->wakeup_type = (u8)tmp;
	}
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	unmap_sysmem(t);

	smbios_struct_count++;
	return len;
}

static int smbios_write_type2(ulong *current, int handle)
{
	struct smbios_type2 *t;
	int len = sizeof(struct smbios_type2);

	t = map_sysmem(*current, len);
	memset(t, 0, sizeof(struct smbios_type2));
	fill_smbios_header(t, SMBIOS_BOARD_INFORMATION, len, handle);
	t->manufacturer = smbios_add_string(t->eos, CONFIG_SMBIOS_MANUFACTURER);
	t->product_name = smbios_add_string(t->eos, CONFIG_SMBIOS_PRODUCT_NAME);
	t->feature_flags = SMBIOS_BOARD_FEATURE_HOSTING;
	t->board_type = SMBIOS_BOARD_MOTHERBOARD;

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	unmap_sysmem(t);

	smbios_struct_count++;
	return len;
}

static int smbios_write_type3(ulong *current, int handle)
{
	struct smbios_type3 *t;
	ofnode node_type3;
	u32 tmp;
	int len = sizeof(struct smbios_type3);

	t = map_sysmem(*current, len);
	memset(t, 0, sizeof(struct smbios_type3));
	fill_smbios_header(t, SMBIOS_SYSTEM_ENCLOSURE, len, handle);
	t->manufacturer = smbios_add_string(t->eos, CONFIG_SMBIOS_MANUFACTURER);
	t->chassis_type = SMBIOS_ENCLOSURE_DESKTOP;
	t->bootup_state = SMBIOS_STATE_SAFE;
	t->power_supply_state = SMBIOS_STATE_SAFE;
	t->thermal_state = SMBIOS_STATE_SAFE;
	t->security_status = SMBIOS_SECURITY_NONE;

	node_type3 = ofnode_path("/uboot-smbios/type3");
	if (ofnode_valid(node_type3)) {
		ofnode_read_u32(node_type3, "type", &tmp);
		t->chassis_type = (u8)tmp;
	}

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	unmap_sysmem(t);

	smbios_struct_count++;
	return len;
}

static void set_cache_handle(struct smbios_type7 *type7)
{
	struct smbios_type4 *type4;

	type4 = (struct smbios_type4 *)map_sysmem(offset_type4, sizeof(struct smbios_type4));
	while ((type4) && (type4->type == SMBIOS_PROCESSOR_INFORMATION)) {
		switch (type7->cache_configuration & 0x3) {
		case 0:
			if (type4->l1_cache_handle == 0xFFFF)
				type4->l1_cache_handle = type7->handle;
			break;
		case 1:
			if (type4->l2_cache_handle == 0xFFFF)
				type4->l2_cache_handle = type7->handle;
			break;
		case 2:
			if (type4->l3_cache_handle == 0xFFFF)
				type4->l3_cache_handle = type7->handle;
			break;
		}
		type4 += sizeof(struct smbios_type4);
	}
	unmap_sysmem(type4);
}

static void smbios_write_type4_dm(struct smbios_type4 *t, ofnode node_type4)
{
	u8 proc_type = 0;
	u8 voltage = 0;
	u8 core_count = 0;
	u8 core_enabled = 0;
	u8 thread_count = 0;
	u16 max_speed = 0;
	u16 current_speed = 0;
	u16 proc_char = 0;
	u8 status = 0;
	u16 processor_family = SMBIOS_PROCESSOR_FAMILY_UNKNOWN;
	const char *vendor = "Unknown";
	const char *name = "Unknown";
	const char *serial_number = "Unknown";
	const char *asset_tag = "Unknown";
	const char *part_number = "Unknown";
	const char *socket_designation = "Unknown";

#ifdef CONFIG_CPU
	char processor_name[49];
	char vendor_name[49];
	struct udevice *dev = NULL;
	struct cpu_info info;

	uclass_find_first_device(UCLASS_CPU, &dev);
	if (dev) {
		struct cpu_platdata *plat = dev_get_parent_platdata(dev);

		cpu_get_info(dev, &info);
		processor_family = is_proc_arm(info.cpu_type);
		if (processor_family) {
			t->processor_family = DMTF_CHECK_FAMILY2;
			t->processor_family2 = processor_family;
		} else {
			t->processor_family = processor_family;
		}

		t->processor_id[0] = plat->id[0];
		t->processor_id[1] = plat->id[1];

		if (!cpu_get_vendor(dev, vendor_name, sizeof(vendor_name)))
			vendor = vendor_name;
		if (!cpu_get_desc(dev, processor_name, sizeof(processor_name)))
			name = processor_name;

		core_count = cpu_get_count(dev);
		status = plat->status;
		proc_type = plat->proc_type;
		voltage = plat->voltage;
		core_enabled = plat->core_enabled;
		thread_count = plat->thread_count;
		max_speed = plat->max_speed;
		proc_char = plat->proc_char;
		serial_number = plat->serial_number;
		asset_tag = plat->asset_tag;
		part_number = plat->part_number;
		socket_designation = plat->socket_designation;
	}
#else
	u32 tmp = 0;

	socket_designation = ofnode_read_string(node_type4, "socket");

	tmp = 0;
	ofnode_read_u32(node_type4, "processor-type", &tmp);
	proc_type = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type4, "processor-family", &tmp);
	processor_family = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type4, "processor-family2", &tmp);
	t->processor_family2 = (u16)tmp;
	vendor = ofnode_read_string(node_type4, "processor-manufacturer");

	tmp = 0;
	ofnode_read_u32(node_type4, "processor-id", &tmp);
	t->processor_id[0] = tmp;
	t->processor_id[1] = 0;

	name = ofnode_read_string(node_type4, "processor-version");

	tmp = 0;
	ofnode_read_u32(node_type4, "voltage", &tmp);
	voltage = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type4, "external-clock", &tmp);
	t->external_clock = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type4, "maxspeed", &tmp);
	max_speed = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type4, "curspeed", &tmp);
	current_speed = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type4, "cpu-status", &tmp);
	status = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type4, "processor-upgrade", &tmp);
	t->processor_upgrade = (u8)tmp;

	serial_number	= ofnode_read_string(node_type4, "serial-number");
	asset_tag		= ofnode_read_string(node_type4, "asset-tag");
	part_number		= ofnode_read_string(node_type4, "part-number");

	tmp = 0;
	ofnode_read_u32(node_type4, "core-count", &tmp);
	core_count = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type4, "core-enabled", &tmp);
	core_enabled = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type4, "thread-count", &tmp);
	thread_count = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type4, "processor-characteristics", &tmp);
	proc_char = (u16)tmp;

#endif

	t->processor_family = processor_family;
	t->processor_manufacturer = smbios_add_string(t->eos, vendor);
	t->processor_version = smbios_add_string(t->eos, name);
	t->status =  status;
	t->processor_type = proc_type;
	t->voltage = voltage;
	t->core_count = core_count;
	t->core_enabled = core_enabled;
	t->thread_count = thread_count;
	t->max_speed = max_speed;
	t->current_speed = current_speed;
	t->processor_characteristics = proc_char;
	t->serial_number = smbios_add_string(t->eos, serial_number);
	t->asset_tag = smbios_add_string(t->eos, asset_tag);
	t->part_number = smbios_add_string(t->eos, part_number);
	t->socket_designation = smbios_add_string(t->eos, socket_designation);
}

static int smbios_write_type4(ulong *current, int handle)
{
	struct smbios_type4 *t;
	char node_path[30];
	u32 i;
	ofnode node_type4;
	int len;

	offset_type4 = *current;
	i = 0;
	len = 0;
	do {
		sprintf(node_path, "/uboot-smbios/type4@%d", i);
		node_type4 = ofnode_path(node_path);
		if (ofnode_valid(node_type4)) {
			len = sizeof(struct smbios_type4);
			t = map_sysmem(*current, len);
			memset(t, 0, sizeof(struct smbios_type4));
			fill_smbios_header(t, SMBIOS_PROCESSOR_INFORMATION, len, handle);
			t->processor_type = SMBIOS_PROCESSOR_TYPE_CENTRAL;
			t->processor_upgrade = SMBIOS_PROCESSOR_UPGRADE_NONE;
			smbios_write_type4_dm(t, node_type4);

			t->l1_cache_handle = 0xFFFF;
			t->l2_cache_handle = 0xFFFF;
			t->l3_cache_handle = 0xFFFF;

			len = t->length + smbios_string_table_len(t->eos);
			*current += len;
			unmap_sysmem(t);

			smbios_struct_count++;
			handle++;
			i++;
		}
	} while (ofnode_valid(node_type4));

	return len * i;
}

static void smbios_write_type7_dm(struct smbios_type7 *t, ofnode node_type7)
{
	u32 tmp;

	t->socket_designation = smbios_add_string(t->eos, ofnode_read_string(node_type7, "socket"));
	tmp = 0;
	ofnode_read_u32(node_type7, "ecorr-type", &tmp);
	t->error_correction_type = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type7, "cache-type", &tmp);
	t->system_cache_type = (u8)tmp;
	tmp = 0;
	ofnode_read_u32(node_type7, "maxsize", &tmp);
	t->maximum_cache_size = (u16)tmp;
	t->maximum_cache_size2 = tmp;

	tmp = 0;
	if (!ofnode_read_u32(node_type7, "maxsize2", &tmp)) // Update field only when corresponding node found
		t->maximum_cache_size2 = tmp;

	tmp = 0;
	ofnode_read_u32(node_type7, "installed-size", &tmp);
	t->installed_size = (u16)tmp;
	t->installed_cache_size2 = tmp;

	tmp = 0;
	if (!ofnode_read_u32(node_type7, "installed-size2", &tmp)) // Update field only when corresponding node found
		t->installed_cache_size2 = tmp;

	tmp = 0;
	ofnode_read_u32(node_type7, "cache-config", &tmp);
	t->cache_configuration = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type7, "associativity", &tmp);
	t->associativity = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type7, "supported-sram", &tmp);
	t->supported_sram_type = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type7, "current-sram", &tmp);
	t->current_sram_type = (u16)tmp;
}

static int smbios_write_type7(ulong *current, int handle)
{
	struct smbios_type7 *t;
	char node_path[30];
	ofnode node_type7;
	u32 i = 0, len = 0, total_len = 0;

	do {
		sprintf(node_path, "/uboot-smbios/type7@%d", i);
		node_type7 = ofnode_path(node_path);
		if (ofnode_valid(node_type7)) {
			len = sizeof(struct smbios_type7);
			t = map_sysmem(*current, len);
			memset(t, 0, sizeof(struct smbios_type7));
			fill_smbios_header(t, SMBIOS_CACHE_INFORMATION, len, handle);
			smbios_write_type7_dm(t, node_type7);
			set_cache_handle(t);
			len = t->length + smbios_string_table_len(t->eos);
			total_len += len;
			*current += len;
			unmap_sysmem(t);
			smbios_struct_count++;
			handle++;
			i++;
		}
	} while (ofnode_valid(node_type7));

	return total_len;
}

static void smbios_write_type8_dm(struct smbios_type8 *t, ofnode node_type8)
{
	u32 tmp;

	tmp = 0;
	ofnode_read_u32(node_type8, "internal-reference-designator", &tmp);
	t->internal_reference_designator = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type8, "internal-connector-type", &tmp);
	t->internal_connector_type = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type8, "external-reference-designator", &tmp);
	t->external_reference_designator = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type8, "external-connector-type", &tmp);
	t->external_connector_type = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type8, "port-type", &tmp);
	t->port_type = (u8)tmp;
}

static int smbios_write_type8(ulong *current, int handle)
{
	struct smbios_type8 *t;
	char node_path[30];
	ofnode node_type8;
	u32 i = 0, len = 0, total_len = 0;

	do {
		sprintf(node_path, "/uboot-smbios/type8@%d", i);
		node_type8 = ofnode_path(node_path);
		if (ofnode_valid(node_type8)) {
			len = sizeof(struct smbios_type8);
			t = map_sysmem(*current, len);
			memset(t, 0, sizeof(struct smbios_type8));
			fill_smbios_header(t, SMBIOS_PORT_INFORMATION, len, handle);
			smbios_write_type8_dm(t, node_type8);
			len = t->length + smbios_string_table_len(t->eos);
			total_len += len;
			*current += len;
			unmap_sysmem(t);
			smbios_struct_count++;
			handle++;
			i++;
		}
	} while (ofnode_valid(node_type8));

	return total_len;
}

static void smbios_write_type9_dm(struct smbios_type9 *t, ofnode node_type9)
{
	u32 tmp;

	t->slot_designation = smbios_add_string(t->eos, ofnode_read_string(node_type9,
									   "slot-designation"));

	tmp = 0;
	ofnode_read_u32(node_type9, "slot-id", &tmp);
	t->slot_id = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type9, "slot-length", &tmp);
	t->slot_length = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type9, "slot-type", &tmp);
	t->slot_type = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type9, "slot-data-bus-width", &tmp);
	t->slot_data_bus_width =  (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type9, "current-usage", &tmp);
	t->current_usage =  (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type9, "slot-characteristics-1", &tmp);
	t->slot_characteristics_1 =  (u8)tmp;
}

static int smbios_write_type9(ulong *current, int handle)
{
	struct smbios_type9 *t;
	char node_path[30];
	ofnode node_type9;
	u32 i = 0, len = 0, total_len = 0;

	do {
		sprintf(node_path, "/uboot-smbios/type9@%d", i);
		node_type9 = ofnode_path(node_path);
		if (ofnode_valid(node_type9)) {
			len = sizeof(struct smbios_type9);
			t = map_sysmem(*current, len);
			memset(t, 0, sizeof(struct smbios_type9));
			fill_smbios_header(t, SMBIOS_SYSTEM_SLOTS, len, handle);
			smbios_write_type9_dm(t, node_type9);
			len = t->length + smbios_string_table_len(t->eos);
			total_len += len;
			*current += len;
			unmap_sysmem(t);
			smbios_struct_count++;
			handle++;
			i++;
		}
	} while (ofnode_valid(node_type9));

	return total_len;
}

static int smbios_write_type13(ulong *current, int handle)
{
	u32 tmp;

	ofnode node_type13;
	char node_path[30];
	struct smbios_type13 *t;
	int len = 0;

	sprintf(node_path, "/uboot-smbios/type13");
	node_type13 = ofnode_path(node_path);

	if (ofnode_valid(node_type13)) {
		len = sizeof(struct smbios_type13);
		t = map_sysmem(*current, len);
		memset(t, 0, sizeof(struct smbios_type13));
		fill_smbios_header(t, SMBIOS_BIOS_LANGUAGE_INFORMATION, len, handle);

		ofnode_read_u32(node_type13, "installable-languages", &tmp);
		t->installable_languages = (u8)tmp;

		ofnode_read_u32(node_type13, "flags", &tmp);
		t->flags = (u8)tmp;

		t->current_language = smbios_add_string(t->eos,
							ofnode_read_string(node_type13,
									   "current-language"));
		len = t->length + smbios_string_table_len(t->eos);
		*current += len;
		unmap_sysmem(t);

		smbios_struct_count++;
	}
	return len;
}

static int smbios_write_type16(ulong *current, int handle)
{
	struct smbios_type16 *t;
	ofnode node_type16;
	u32 tmp;
	u64 tmp64;
	int len = sizeof(struct smbios_type16);

	t = map_sysmem(*current, len);
	memset(t, 0, sizeof(struct smbios_type16));
	fill_smbios_header(t, SMBIOS_PHYS_MEMORY_ARRAY, len, handle);

	t->location = DMTF_TYPE16_LOCATION_SYSTEM_BOARD_OR_MOTHERBOARD;
	t->use = DMTF_TYPE16_USE_SYSTEM_MEMORY;
	t->memory_error_correction = DMTF_TYPE16_ERROR_CORRECTION_SINGLE_BIT_ECC;
	t->maximum_capacity = DMTF_TYPE16_MAXIMUM_CAPACITY_64GB;
	t->number_of_memory_devices = 1;
	t->memory_error_information_handle = 0xFFFE;

	node_type16 = ofnode_path("/uboot-smbios/type16");
	if (ofnode_valid(node_type16)) {
		tmp = 0;
		ofnode_read_u32(node_type16, "location", &tmp);
		t->location = (u8)tmp;
		tmp = 0;
		ofnode_read_u32(node_type16, "use", &tmp);
		t->use = (u8)tmp;
		tmp = 0;
		ofnode_read_u32(node_type16, "err-corr", &tmp);
		t->memory_error_correction = (u8)tmp;
		tmp = 0;
		ofnode_read_u32(node_type16, "max-capacity", &tmp);
		t->maximum_capacity = tmp;
		tmp = 0xFFFE;
		ofnode_read_u32(node_type16, "memerr-info", &tmp);
		t->memory_error_information_handle = (u16)tmp;
		tmp = 0;
		ofnode_read_u32(node_type16, "mem-slots", &tmp);
		t->number_of_memory_devices = (u16)tmp;
		tmp64 = 0;
		ofnode_read_u64(node_type16, "ext-max-capacitys", &tmp64);
		t->extended_maximum_capacity = tmp64;
	}

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	unmap_sysmem(t);

	smbios_struct_count++;
	return len;
}

static void smbios_write_type17_dm(struct smbios_type17 *t, ofnode node_type17,
				   ulong *current, int handle, int index)
{
	u32 tmp;
	u64 tmp64;

	tmp = 0;
	ofnode_read_u32(node_type17, "array-handle", &tmp);
	t->physical_memory_array_handle = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "size", &tmp);
	t->size = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "ext-size", &tmp);
	t->extended_size = tmp;

	tmp64 = 0;
	ofnode_read_u64(node_type17, "vol-sizes", &tmp64);
	t->volatile_size = tmp64;

	tmp64 = 0;
	ofnode_read_u64(node_type17, "log-sizes", &tmp64);
	t->logical_size = tmp64;

	tmp = 0;
	ofnode_read_u32(node_type17, "form-factor", &tmp);
	t->form_factor = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "total-width", &tmp);
	t->total_width = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "data-width", &tmp);
	t->data_width = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "device-set", &tmp);
	t->device_set = (u8)tmp;

	t->device_locator = smbios_add_string(t->eos,
					      ofnode_read_string(node_type17, "device-loc"));

	tmp = 0;
	ofnode_read_u32(node_type17, "mem-type", &tmp);
	t->memory_type = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "minimum_voltage", &tmp);
	t->minimum_voltage = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "maximum_voltage", &tmp);
	t->maximum_voltage = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "configured_voltage", &tmp);
	t->configured_voltage = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "speed", &tmp);
	t->speed = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "configured-memory-speed", &tmp);
	t->configured_memory_speed = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "attributes", &tmp);
	t->attributes = (u8)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "module-product-id", &tmp);
	t->module_product_id = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type17, "module-manufacturer-id", &tmp);
	t->module_manufacturer_id = (u16)tmp;

	t->serial_number = smbios_add_string(t->eos,
					      ofnode_read_string(node_type17, "serial-number"));

	t->manufacturer = smbios_add_string(t->eos,
					      ofnode_read_string(node_type17, "manufacturer"));

	t->part_number = smbios_add_string(t->eos,
					      ofnode_read_string(node_type17, "part-number"));

	t->memory_technology = 0x03; // DRAM
}

static int smbios_write_type17(ulong *current, int handle)
{
	struct smbios_type17 *t;
	char node_path[30];
	ofnode node_type17;
	u32 i = 0, len = 0, total_len = 0;

	do {
		sprintf(node_path, "/uboot-smbios/type17@%d", i);
		node_type17 = ofnode_path(node_path);
		if (ofnode_valid(node_type17)) {
			len = sizeof(struct smbios_type17);
			t = map_sysmem(*current, len);
			memset(t, 0, sizeof(struct smbios_type17));
			fill_smbios_header(t, SMBIOS_MEMORY_DEVICE, len, handle);
			smbios_write_type17_dm(t, node_type17,  current, handle, i);
			len = t->length + smbios_string_table_len(t->eos);
			total_len += len;
			*current += len;
			unmap_sysmem(t);
			smbios_struct_count++;
			handle++;
			i++;
		}
	} while (ofnode_valid(node_type17));

	return total_len;
}

static void smbios_write_type19_dm(struct smbios_type19 *t, ofnode node_type19,
				   ulong *current, int handle, int index)
{
	u32 tmp;
	u64 tmp64;

	tmp = 0;
	ofnode_read_u32(node_type19, "start-addr", &tmp);
	t->starting_address = tmp;

	tmp = 0;
	ofnode_read_u32(node_type19, "end-addr", &tmp);
	t->ending_address = tmp;

	tmp = 0;
	ofnode_read_u32(node_type19, "array-handle", &tmp);
	t->memory_array_handle = (u16)tmp;

	tmp = 0;
	ofnode_read_u32(node_type19, "part-width", &tmp);
	t->partition_width = (u8)tmp;

	tmp64 = 0;
	ofnode_read_u64(node_type19, "ext-start-addrs", &tmp64);
	t->extended_starting_address = tmp64;

	tmp64 = 0;
	ofnode_read_u64(node_type19, "ext-end-addrs", &tmp64);
	t->extended_ending_address = tmp64;
}

static int smbios_write_type19(ulong *current, int handle)
{
	struct smbios_type19 *t;
	char node_path[30];
	ofnode node_type19;
	u32 i = 0, len = 0, total_len = 0;

	do {
		sprintf(node_path, "/uboot-smbios/type19@%d", i);
		node_type19 = ofnode_path(node_path);
		if (ofnode_valid(node_type19)) {
			len = sizeof(struct smbios_type19);
			t = map_sysmem(*current, len);
			memset(t, 0, sizeof(struct smbios_type19));
			fill_smbios_header(t, SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS, len, handle);
			smbios_write_type19_dm(t, node_type19,  current, handle, i);
			len = t->length + smbios_string_table_len(t->eos);
			total_len += len;
			*current += len;
			unmap_sysmem(t);
			smbios_struct_count++;
			handle++;
			i++;
		}
	} while (ofnode_valid(node_type19));

	return total_len;
}

static int smbios_write_type20_dm(ulong *current, int handle, int index)
{
	struct smbios_type20 *t;
	int len = sizeof(struct smbios_type20);

	t = map_sysmem(*current, len);
	memset(t, 0, sizeof(struct smbios_type20));
	fill_smbios_header(t, SMBIOS_MEMORY_DEVICE_MAPPED_ADDRESS, len, handle + index);

	/* Data to be filled by OEM */
	/* ...
	 * ...
	 * ...
	 */

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	unmap_sysmem(t);

	smbios_struct_count++;
	return len;
}

static int smbios_write_type20(ulong *current, int handle)
{
	u32 no_of_handles = MAX_MEMORY_MAPPED_DEV, i = 0, len = 0;

	for (; i < no_of_handles; i++)
		len += smbios_write_type20_dm(current, handle, i);

	return len;
}

static int smbios_write_type32(ulong *current, int handle)
{
	struct smbios_type32 *t;
	int len = sizeof(struct smbios_type32);

	t = map_sysmem(*current, len);
	memset(t, 0, sizeof(struct smbios_type32));
	fill_smbios_header(t, SMBIOS_SYSTEM_BOOT_INFORMATION, len, handle);

	*current += len;
	unmap_sysmem(t);

	smbios_struct_count++;
	return len;
}

static int smbios_write_type41_dm(ulong *current, int handle, int index)
{
	struct smbios_type41 *t;
	int len = sizeof(struct smbios_type41);

	t = map_sysmem(*current, len);
	memset(t, 0, sizeof(struct smbios_type41));
	fill_smbios_header(t, SMBIOS_ONBOARD_EVICES_EXTENDED_INFORMATION, len, handle + index);

	/* Data to be filled by OEM */
	/* ...
	 * ...
	 * ...
	 */

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	unmap_sysmem(t);

	smbios_struct_count++;
	return len;
}

static int smbios_write_type41(ulong *current, int handle)
{
	u32 no_of_handles = MAX_MEMORY_MAPPED_DEV, i = 0, len = 0;

	for (; i < no_of_handles; i++)
		len += smbios_write_type41_dm(current, handle, i);

	return len;
}

static int smbios_write_type127(ulong *current, int handle)
{
	struct smbios_type127 *t;
	int len = sizeof(struct smbios_type127);

	t = map_sysmem(*current, len);
	memset(t, 0, sizeof(struct smbios_type127));
	fill_smbios_header(t, SMBIOS_END_OF_TABLE, len, handle);

	*current += len;
	unmap_sysmem(t);

	smbios_struct_count++;
	return len;
}

static smbios_write_type smbios_write_funcs[] = {
	smbios_write_type0,
	smbios_write_type1,
	smbios_write_type2,
	smbios_write_type3,
	smbios_write_type4,
	smbios_write_type7,
	smbios_write_type8,
	smbios_write_type9,
	smbios_write_type13,
	smbios_write_type16,
	smbios_write_type17,
	smbios_write_type19,
	smbios_write_type20,
	smbios_write_type41,
	smbios_write_type32,
	smbios_write_type127
};

ulong write_smbios_table(ulong addr)
{
	struct smbios_entry *se;
	ulong table_addr;
	ulong tables;
	int len = 0;
	int max_struct_size = 0;
	int handle = 0;
	char *istart;
	int isize;
	int i;

	/* Init Type 4 pointer */
	offset_type4 = 0;

	/* 16 byte align the table address */
	addr = ALIGN(addr, 16);

	se = map_sysmem(addr, sizeof(struct smbios_entry));
	memset(se, 0, sizeof(struct smbios_entry));

	addr += sizeof(struct smbios_entry);
	addr = ALIGN(addr, 16);
	tables = addr;

	/* populate minimum required tables */
	for (i = 0; i < ARRAY_SIZE(smbios_write_funcs); i++) {
		int tmp = smbios_write_funcs[i]((ulong *)&addr, handle);

		max_struct_size = max(max_struct_size, tmp);
		len += tmp;
		handle += 0x100;
	}

	memcpy(se->anchor, "_SM_", 4);
	se->length = sizeof(struct smbios_entry);
	se->major_ver = SMBIOS_MAJOR_VER;
	se->minor_ver = SMBIOS_MINOR_VER;
	se->max_struct_size = max_struct_size;
	memcpy(se->intermediate_anchor, "_DMI_", 5);
	se->struct_table_length = len;

	/*
	 * We must use a pointer here so things work correctly on sandbox. The
	 * user of this table is not aware of the mapping of addresses to
	 * sandbox's DRAM buffer.
	 */
	table_addr = (ulong)map_sysmem(tables, 0);
	if (sizeof(table_addr) > sizeof(u32) && table_addr > (ulong)UINT_MAX) {
		/*
		 * We need to put this >32-bit pointer into the table but the
		 * field is only 32 bits wide.
		 */
		printf("WARNING: SMBIOS table_address overflow %llx\n",
		       (unsigned long long)table_addr);
		table_addr = 0;
	}
	se->struct_table_address = table_addr;

	se->struct_count = smbios_struct_count;

	/* calculate checksums */
	istart = (char *)se + SMBIOS_INTERMEDIATE_OFFSET;
	isize = sizeof(struct smbios_entry) - SMBIOS_INTERMEDIATE_OFFSET;
	se->intermediate_checksum = table_compute_checksum(istart, isize);
	se->checksum = table_compute_checksum(se, sizeof(struct smbios_entry));
	unmap_sysmem(se);

	return addr;
}
