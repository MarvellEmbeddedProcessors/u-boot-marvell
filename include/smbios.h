/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2015, Bin Meng <bmeng.cn@gmail.com>
 *
 * Adapted from coreboot src/include/smbios.h
 */

#ifndef _SMBIOS_H_
#define _SMBIOS_H_

/* SMBIOS spec version implemented */
#define SMBIOS_MAJOR_VER	3
#define SMBIOS_MINOR_VER	0

#define DMTF_PROC_ARMV7		0x100
#define DMTF_PROC_ARMV8		0x101
#define DMTF_CHECK_FAMILY2	0xFE

enum {
	L1_D_CACHE = 0,
	L1_I_CACHE,
	L2_CACHE,
	CACHE_MAX,
};

#define NO_OF_CACHE CACHE_MAX
#define MAX_PORTS 3
#define MAX_SLOTS 2
#define MAX_MEMORY_DEV 1
#define MAX_MEMORY_ARRAY 1
#define MAX_MEMORY_MAPPED_DEV 1

/* SMBIOS structure types */
enum {
	SMBIOS_BIOS_INFORMATION = 0,
	SMBIOS_SYSTEM_INFORMATION = 1,
	SMBIOS_BOARD_INFORMATION = 2,
	SMBIOS_SYSTEM_ENCLOSURE = 3,
	SMBIOS_PROCESSOR_INFORMATION = 4,
	SMBIOS_CACHE_INFORMATION = 7,
	SMBIOS_PORT_INFORMATION = 8,
	SMBIOS_SYSTEM_SLOTS = 9,
	SMBIOS_BIOS_LANGUAGE_INFORMATION = 13,
	SMBIOS_PHYS_MEMORY_ARRAY = 16,
	SMBIOS_MEMORY_DEVICE = 17,
	SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS = 19,
	SMBIOS_MEMORY_DEVICE_MAPPED_ADDRESS = 20,
	SMBIOS_SYSTEM_BOOT_INFORMATION = 32,
	SMBIOS_ONBOARD_EVICES_EXTENDED_INFORMATION = 41,
	SMBIOS_END_OF_TABLE = 127
};

#define SMBIOS_INTERMEDIATE_OFFSET	16
#define SMBIOS_STRUCT_EOS_BYTES		2

struct __packed smbios_entry {
	u8 anchor[4];
	u8 checksum;
	u8 length;
	u8 major_ver;
	u8 minor_ver;
	u16 max_struct_size;
	u8 entry_point_rev;
	u8 formatted_area[5];
	u8 intermediate_anchor[5];
	u8 intermediate_checksum;
	u16 struct_table_length;
	u32 struct_table_address;
	u16 struct_count;
	u8 bcd_rev;
};

/* BIOS characteristics */
#define BIOS_CHARACTERISTICS_PCI_SUPPORTED	(1 << 7)
#define BIOS_CHARACTERISTICS_UPGRADEABLE	(1 << 11)
#define BIOS_CHARACTERISTICS_SELECTABLE_BOOT	(1 << 16)

#define BIOS_CHARACTERISTICS_EXT1_ACPI		(1 << 0)
#define BIOS_CHARACTERISTICS_EXT1_UEFI		(1 << 3)
#define BIOS_CHARACTERISTICS_EXT2_TARGET	(1 << 2)

struct __packed smbios_type0 {
	u8 type;
	u8 length;
	u16 handle;
	u8 vendor;
	u8 bios_ver;
	u16 bios_start_segment;
	u8 bios_release_date;
	u8 bios_rom_size;
	u64 bios_characteristics;
	u8 bios_characteristics_ext1;
	u8 bios_characteristics_ext2;
	u8 bios_major_release;
	u8 bios_minor_release;
	u8 ec_major_release;
	u8 ec_minor_release;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

struct __packed smbios_type1 {
	u8 type;
	u8 length;
	u16 handle;
	u8 manufacturer;
	u8 product_name;
	u8 version;
	u8 serial_number;
	u8 uuid[16];
	u8 wakeup_type;
	u8 sku_number;
	u8 family;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

#define SMBIOS_BOARD_FEATURE_HOSTING	(1 << 0)
#define SMBIOS_BOARD_MOTHERBOARD	10

struct __packed smbios_type2 {
	u8 type;
	u8 length;
	u16 handle;
	u8 manufacturer;
	u8 product_name;
	u8 version;
	u8 serial_number;
	u8 asset_tag_number;
	u8 feature_flags;
	u8 chassis_location;
	u16 chassis_handle;
	u8 board_type;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

#define SMBIOS_ENCLOSURE_DESKTOP	3
#define SMBIOS_STATE_SAFE		3
#define SMBIOS_SECURITY_NONE		3

struct __packed smbios_type3 {
	u8 type;
	u8 length;
	u16 handle;
	u8 manufacturer;
	u8 chassis_type;
	u8 version;
	u8 serial_number;
	u8 asset_tag_number;
	u8 bootup_state;
	u8 power_supply_state;
	u8 thermal_state;
	u8 security_status;
	u32 oem_defined;
	u8 height;
	u8 number_of_power_cords;
	u8 element_count;
	u8 element_record_length;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

#define SMBIOS_PROCESSOR_TYPE_CENTRAL	3
#define SMBIOS_PROCESSOR_STATUS_ENABLED	1
#define SMBIOS_PROCESSOR_UPGRADE_NONE	6

#define SMBIOS_PROCESSOR_FAMILY_OTHER	1
#define SMBIOS_PROCESSOR_FAMILY_UNKNOWN	2

struct __packed smbios_type4 {
	u8 type;
	u8 length;
	u16 handle;
	u8 socket_designation;
	u8 processor_type;
	u8 processor_family;
	u8 processor_manufacturer;
	u32 processor_id[2];
	u8 processor_version;
	u8 voltage;
	u16 external_clock;
	u16 max_speed;
	u16 current_speed;
	u8 status;
	u8 processor_upgrade;
	u16 l1_cache_handle;
	u16 l2_cache_handle;
	u16 l3_cache_handle;
	u8 serial_number;
	u8 asset_tag;
	u8 part_number;
	u8 core_count;
	u8 core_enabled;
	u8 thread_count;
	u16 processor_characteristics;
	u16 processor_family2;
	u16 core_count2;
	u16 core_enabled2;
	u16 thread_count2;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

/* Type 7 */
#define DMTF_TYPE7_SOCKETED					BIT(3)
#define DMTF_TYPE7_CACHE_ENABLED			BIT(7)
#define DMTF_TYPE7_CACHE_L1					(0)
#define DMTF_TYPE7_CACHE_L2					(1)
#define DMTF_TYPE7_OP_MODE					BIT(8)
#define DMTF_TYPE7_1K_GRANULARITY			BIT(15)
#define DMTF_TYPE7_32K						(32)
#define DMTF_TYPE7_48K						(48)
#define DMTF_TYPE7_512K						(512)
#define DMTF_TYPE7_SRAM_TYPE_UNKNOWN		BIT(1)
#define DMTF_TYPE7_PARITY					(0x04)
#define DMTF_TYPE7_SINGLE_BIT_ECC			(0x05)
#define DMTF_TYPE7_MULTI_BIT_ECC			(0x06)
#define DMTF_TYPE7_INST_CACHE				(0x03)
#define DMTF_TYPE7_INSTRUCTION_CACHE		(0x03)
#define DMTF_TYPE7_DATA_CACHE				(0x04)
#define DMTF_TYPE7_DATA_UNIFIED				(0x05)
#define DMTF_TYPE7_2_WAY_SET_ASSOCIATIVE	(0x04)
#define DMTF_TYPE7_16_WAY_SET_ASSOCIATIVE	(0x08)

struct cache_details {
	char name[16];
	u8 level;
	u8 error_control;
	u8 associativity;
	u8 type;
	u32 size;
};

struct __packed smbios_type7 {
	u8 type;
	u8 length;
	u16 handle;
	u8 socket_designation;
	u16 cache_configuration;
	u16 maximum_cache_size;
	u16 installed_size;
	u16 supported_sram_type;
	u16 current_sram_type;
	u8 cache_speed;
	u8 error_correction_type;
	u8 system_cache_type;
	u8 associativity;
	u32 maximum_cache_size2;
	u32 installed_cache_size2;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

/* Type 8 */
#define DMTF_TYPE8_OTHER			0xFF
#define DMTF_TYPE8_RJ45				0x0B
#define DMTF_TYPE8_NETWORK_PORT		0x1F
#define DMTF_TYPE8_USB				0x10
#define DMTF_TYPE8_ACCESS_BUS		0x12

struct __packed smbios_type8 {
	u8 type;
	u8 length;
	u16 handle;
	u8 internal_reference_designator;
	u8 internal_connector_type;
	u8 external_reference_designator;
	u8 external_connector_type;
	u8 port_type;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

/* Type 9 */
#define DMTF_TYPE9_SLOT_TYPE_PCI			0x06
#define DMTF_TYPE9_SLOT_TYPE_PCIE			0xA8
#define DMTF_TYPE9_CURRENT_USAGE_AVAILABLE	0x03
#define DMTF_TYPE9_SLOT_LENGTH_LONG			0x04
#define DMTF_TYPE9_SLOT_CHAR_1_3_3V			BIT(2)
#define DMTF_TYPE9_SLOT_DATA_BUS_WIDTH_X4	0x0A

struct type9_data {
	char slot_designation[16];
	u8 slot_type;
	u8 slot_data_bus_width;
	u8 current_usage;
	u8 slot_length;
	u16 slot_id;
	u8 slot_characteristics_1;
};

struct __packed smbios_type9 {
	u8 type;
	u8 length;
	u16 handle;
	u8 slot_designation;
	u8 slot_type;
	u8 slot_data_bus_width;
	u8 current_usage;
	u8 slot_length;
	u16 slot_id;
	u8 slot_characteristics_1;
	u8 slot_characteristics_2;
	u16 segment_group_number;
	u8 bus_number;
	u8 device_function_number_base;
	u8 data_bus_width_base;
	u8 peer_grouping_count;
	u8 peer_groups;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

struct __packed smbios_type13 {
	u8 type;
	u8 length;
	u16 handle;
	u8 installable_languages;
	u8 flags;
	u8 reserved[15];
	u8 current_language;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

/* Type 16 */
#define DMTF_TYPE16_LOCATION_SYSTEM_BOARD_OR_MOTHERBOARD 0x03
#define	DMTF_TYPE16_USE_SYSTEM_MEMORY 0x03
#define	DMTF_TYPE16_ERROR_CORRECTION_SINGLE_BIT_ECC 0x05
#define	DMTF_TYPE16_MAXIMUM_CAPACITY_64GB BIT(26)

struct __packed smbios_type16 {
	u8 type;
	u8 length;
	u16 handle;
	u8 location;
	u8 use;
	u8 memory_error_correction;
	u32 maximum_capacity;
	u16 memory_error_information_handle;
	u16 number_of_memory_devices;
	u64 extended_maximum_capacity;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

/* Type 17 */
#define DMTF_TYPE17_DIMM	0x09
#define DMTF_TYPE17_4GB		BIT(12)

struct __packed smbios_type17 {
	u8 type;
	u8 length;
	u16 handle;
	u16 physical_memory_array_handle;
	u16 memory_error_information_handle;
	u16 total_width;
	u16 data_width;
	u16 size;
	u8 form_factor;
	u8 device_set;
	u8 device_locator;
	u8 bank_locator;
	u8 memory_type;
	u16 type_detail;
	u16 speed;
	u8 manufacturer;
	u8 serial_number;
	u8 asset_tag;
	u8 part_number;
	u8 attributes;
	u32 extended_size;
	u16 configured_memory_speed;
	u16 minimum_voltage;
	u16 maximum_voltage;
	u16 configured_voltage;
	u8 memory_technology;
	u16 memory_operating_mode_capability;
	u8 firmware_version;
	u16 module_manufacturer_id;
	u16 module_product_id;
	u16 memory_subsystem_controller_manufacturer_id;
	u16 memory_subsystem_controller_product_id;
	u32 non_volatile_size;
	u32 volatile_size;
	u32 cache_size;
	u32 logical_size;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

struct __packed smbios_type19 {
	u8 type;
	u8 length;
	u16 handle;
	u32 starting_address;
	u32 ending_address;
	u16 memory_array_handle;
	u8 partition_width;
	u64 extended_starting_address;
	u64 extended_ending_address;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

struct __packed smbios_type20 {
	u8 type;
	u8 length;
	u16 handle;
	u32 starting_address;
	u32 ending_address;
	u16 memory_device_handle;
	u16 memory_array_mapped_address_handle;
	u8 partition_row_position;
	u8 interleave_position;
	u8 interleaved_data_depth;
	u64 extended_starting_address;
	u64 extended_ending_address;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

struct __packed smbios_type41 {
	u8 type;
	u8 length;
	u16 handle;
	u8 reference_designation;
	u8 device_type;
	u8 device_type_instance;
	u16 segment_group_number;
	u8 bus_number;
	u8 device_function_number;
	char eos[SMBIOS_STRUCT_EOS_BYTES];
};

struct __packed smbios_type32 {
	u8 type;
	u8 length;
	u16 handle;
	u8 reserved[6];
	u8 boot_status;
	u8 eos[SMBIOS_STRUCT_EOS_BYTES];
};

struct __packed smbios_type127 {
	u8 type;
	u8 length;
	u16 handle;
	u8 eos[SMBIOS_STRUCT_EOS_BYTES];
};

struct __packed smbios_header {
	u8 type;
	u8 length;
	u16 handle;
};

/*
 * is_proc_arm() - Findout if cpu is arm
 *
 * Findout if cpu is arm.
 *
 * @cpu:	cpu name
 */
static inline u16 is_proc_arm(char *cpu)
{
	if (!strcmp(cpu, "ARMv7"))
		return DMTF_PROC_ARMV7;

	if (!strcmp(cpu, "ARMv8"))
		return DMTF_PROC_ARMV8;

	return 0;
}

/**
 * fill_smbios_header() - Fill the header of an SMBIOS table
 *
 * This fills the header of an SMBIOS table structure.
 *
 * @table:	start address of the structure
 * @type:	the type of structure
 * @length:	the length of the formatted area of the structure
 * @handle:	the structure's handle, a unique 16-bit number
 */
static inline void fill_smbios_header(void *table, int type,
				      int length, int handle)
{
	struct smbios_header *header = table;

	header->type = type;
	header->length = length - SMBIOS_STRUCT_EOS_BYTES;
	header->handle = handle;
}

/**
 * Function prototype to write a specific type of SMBIOS structure
 *
 * @addr:	start address to write the structure
 * @handle:	the structure's handle, a unique 16-bit number
 * @return:	size of the structure
 */
typedef int (*smbios_write_type)(ulong *addr, int handle);

/**
 * write_smbios_table() - Write SMBIOS table
 *
 * This writes SMBIOS table at a given address.
 *
 * @addr:	start address to write SMBIOS table. If this is not
 *	16-byte-aligned then it will be aligned before the table is written
 * @return:	end address of SMBIOS table (and start address for next entry)
 */
ulong write_smbios_table(ulong addr);

#endif /* _SMBIOS_H_ */
