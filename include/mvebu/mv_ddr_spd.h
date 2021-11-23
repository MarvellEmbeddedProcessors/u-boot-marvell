/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2020, Sujeet Baranwal <sbaranwal@marvell.com>
 *
 */

#ifndef _MV_DDR_SPD_H
#define _MV_DDR_SPD_H

/*
 * Based on JEDEC Standard No. 21-C, 4.1.2.L-4:
 * Serial Presence Detect (SPD) for DDR4 SDRAM Modules
 */

/* block 0: base configuration and dram parameters */
#define MV_DDR_SPD_DATA_BLOCK0_SIZE		128
/* block 1: module specific parameters sub-block */
#define MV_DDR_SPD_DATA_BLOCK1M_SIZE		64
/* block 1: hybrid memory parameters sub-block */
#define MV_DDR_SPD_DATA_BLOCK1H_SIZE		64
/* block 2: extended function parameter block */
#define MV_DDR_SPD_DATA_BLOCK2E_SIZE		64
/* block 2: manufacturing information */
#define MV_DDR_SPD_DATA_BLOCK2M_SIZE		64
/* block 3: end user programmable */
#define MV_DDR_SPD_DATA_BLOCK3_SIZE		128

#define MV_DDR_SPD_DEV_TYPE_DDR4		0xc
#define MV_DDR_SPD_MODULE_TYPE_UDIMM		0x2
#define MV_DDR_SPD_MODULE_TYPE_SO_DIMM		0x3
#define MV_DDR_SPD_MODULE_TYPE_MINI_UDIMM	0x6
#define MV_DDR_SPD_MODULE_TYPE_72BIT_SO_UDIMM	0x9
#define MV_DDR_SPD_MODULE_TYPE_16BIT_SO_DIMM	0xc
#define MV_DDR_SPD_MODULE_TYPE_32BIT_SO_DIMM	0xd

/*
 * TODO: For now, the struct contains block 0 & block 1 with module specific
 * parameters for unbuffered memory module types only.
 */
	union mv_ddr_spd_data {
			unsigned char all_bytes[MV_DDR_SPD_DATA_BLOCK0_SIZE +
									MV_DDR_SPD_DATA_BLOCK1M_SIZE +
									MV_DDR_SPD_DATA_BLOCK1H_SIZE +
									MV_DDR_SPD_DATA_BLOCK2E_SIZE +
									MV_DDR_SPD_DATA_BLOCK2M_SIZE];
	struct {
		/* block 0 */
		union { /* num of bytes used/num of bytes in spd device/crc coverage */
			unsigned char all_bits;
			struct {
				unsigned char spd_bytes_used:4,
					spd_bytes_total:3,
					reserved:1;
			} bit_fields;
		} byte_0;
		union { /* spd revision */
			unsigned char all_bits;
			struct {
				unsigned char addtions_level:4,
					encoding_level:4;
			} bit_fields;
		} byte_1;
		unsigned char  byte_2; /* key_byte/dram device type */
		union { /* key byte/module type */
			unsigned char all_bits;
			struct {
				unsigned char module_type:4,
					hybrid_media:3,
					hybrid:1;
			} bit_fields;
		} byte_3;
		union { /* sdram density & banks */
			unsigned char all_bits;
			struct {
				unsigned char die_capacity:4,
					bank_address:2,
					bank_group:2;
			} bit_fields;
		} byte_4;
		union { /* sdram addressing */
			unsigned char all_bits;
			struct {
				unsigned char col_address:3,
					row_address:3,
					reserved:2;
			} bit_fields;
		} byte_5;
		union { /* sdram package type */
			unsigned char all_bits;
			struct {
				unsigned char signal_loading:2,
					reserved:2,
					die_count:3,
					sdram_package_type:1;
			} bit_fields;
		} byte_6;
		union { /* sdram optional features */
			unsigned char all_bits;
			struct {
				unsigned char mac:4, /* max activate count */
					t_maw:2, /* max activate window */
					reserved:2; /* all 0s */
			} bit_fields;
		} byte_7;
		unsigned char byte_8; /* sdram thermal & refresh options; reserved; 0x00 */
		union { /* other sdram optional features */
			unsigned char all_bits;
			struct {
				unsigned char reserved:5, /* all 0s */
					soft_ppr:1,
					ppr:2; /* post package repair */
			} bit_fields;
		} byte_9;
		union { /* secondary sdram package type */
			unsigned char all_bits;
			struct {
				unsigned char signal_loading:2,
					density_ratio:2, /* dram density ratio */
					die_count:3,
					sdram_package_type:1;
			} bit_fields;
		} byte_10;
		union { /* module nominal voltage, vdd */
			unsigned char all_bits;
			struct {
		unsigned char operable : 1,
			      endurant : 1,
			      reserved : 6;         /* all 0s */
			} bit_fields;
		} byte_11;
		union { /* module organization*/
			unsigned char all_bits;
			struct {
				unsigned char device_width:3,
					dimm_pkg_ranks_num:3, /* package ranks per dimm number */
					rank_mix:1,
					reserved:1; /* 0 */
			} bit_fields;
		} byte_12;
		union { /* module memory bus width */
			unsigned char all_bits;
			struct {
				unsigned char primary_bus_width:3, /* in bits */
					bus_width_ext:2, /* in bits */
					reserved:3; /* all 0s */
			} bit_fields;
		} byte_13;
		union { /* module thernal sensor */
			unsigned char all_bits;
			struct {
				unsigned char reserved:7,
					thermal_sensor:1;
			} bit_fields;
		} byte_14;
		union { /* extended module type */
			unsigned char all_bits;
			struct {
				unsigned char ext_base_module_type:4,
					reserved:4; /* all 0s */
			} bit_fields;
		} byte_15;
		unsigned char byte_16; /* reserved; 0x00 */
		union { /* timebases */
			unsigned char all_bits;
			struct {
				unsigned char ftb:2, /* fine timebase */
					mtb:2, /* medium timebase */
					reserved:4; /* all 0s */
			} bit_fields;
		} byte_17;
		unsigned char byte_18; /* sdram min cycle time (t ck avg min), mtb */
		unsigned char byte_19; /* sdram max cycle time (t ck avg max), mtb */
		unsigned char byte_20; /* cas latencies supported, first byte */
		unsigned char byte_21; /* cas latencies supported, second byte */
		unsigned char byte_22; /* cas latencies supported, third byte */
		unsigned char byte_23; /* cas latencies supported, fourth byte */
		unsigned char byte_24; /* min cas latency time (t aa min), mtb */
		unsigned char byte_25; /* min ras to cas delay time (t rcd min), mtb */
		unsigned char byte_26; /* min row precharge delay time (t rp min), mtb */
		union { /* upper nibbles for t ras min & t rc min */
			unsigned char all_bits;
			struct {
				unsigned char t_ras_min_msn:4, /* t ras min most significant nibble */
					t_rc_min_msn:4; /* t rc min most significant nibble */
			} bit_fields;
		} byte_27;
		unsigned char byte_28; /* min active to precharge delay time (t ras min), l-s-byte, mtb */
		unsigned char byte_29; /* min active to active/refresh delay time (t rc min), l-s-byte, mtb */
		unsigned char byte_30; /* min refresh recovery delay time (t rfc1 min), l-s-byte, mtb */
		unsigned char byte_31; /* min refresh recovery delay time (t rfc1 min), m-s-byte, mtb */
		unsigned char byte_32; /* min refresh recovery delay time (t rfc2 min), l-s-byte, mtb */
		unsigned char byte_33; /* min refresh recovery delay time (t rfc2 min), m-s-byte, mtb */
		unsigned char byte_34; /* min refresh recovery delay time (t rfc4 min), l-s-byte, mtb */
		unsigned char byte_35; /* min refresh recovery delay time (t rfc4 min), m-s-byte, mtb */
		union { /* upper nibble for t faw */
			unsigned char all_bits;
			struct {
				unsigned char t_faw_min_msn:4, /* t faw min most significant nibble */
					reserved:4;
			} bit_fields;
		} byte_36;
		unsigned char byte_37; /* min four activate window delay time (t faw min), l-s-byte, mtb */
		/* byte 38: min activate to activate delay time (t rrd_s min), diff bank group, mtb */
		unsigned char byte_38;
		/* byte 39: min activate to activate delay time (t rrd_l min), same bank group, mtb */
		unsigned char byte_39;
		unsigned char byte_40; /* min cas to cas delay time (t ccd_l min), same bank group, mtb */
		union { /* upper nibble for t wr min */
			unsigned char all_bits;
			struct {
				unsigned char t_wr_min_msn:4, /* t wr min most significant nibble */
					reserved:4;
			} bit_fields;
		} byte_41;
		unsigned char byte_42; /* min write recovery time (t wr min) */
		union { /* upper nibbles for t wtr min */
			unsigned char all_bits;
			struct {
				unsigned char t_wtr_s_min_msn:4, /* t wtr s min most significant nibble */
					t_wtr_l_min_msn:4; /* t wtr l min most significant nibble */
			} bit_fields;
		} byte_43;
		unsigned char byte_44; /* min write to read time (t wtr s min), diff bank group, mtb */
		unsigned char byte_45; /* min write to read time (t wtr l min), same bank group, mtb */
		unsigned char bytes_46_59[14]; /* reserved; all 0s */
		unsigned char bytes_60_77[18]; /* TODO: connector to sdram bit mapping */
		unsigned char bytes_78_116[39]; /* reserved; all 0s */
		/* fine offset for min cas to cas delay time (t ccd_l min), same bank group, ftb */
		unsigned char byte_117;
		/* fine offset for min activate to activate delay time (t rrd_l min), same bank group, ftb */
		unsigned char byte_118;
		/* fine offset for min activate to activate delay time (t rrd_s min), diff bank group, ftb */
		unsigned char byte_119;
		/* fine offset for min active to active/refresh delay time (t rc min), ftb */
		unsigned char byte_120;
		unsigned char byte_121; /* fine offset for min row precharge delay time (t rp min), ftb */
		unsigned char byte_122; /* fine offset for min ras to cas delay time (t rcd min), ftb */
		unsigned char byte_123; /* fine offset for min cas latency time (t aa min), ftb */
		unsigned char byte_124; /* fine offset for sdram max cycle time (t ck avg max), ftb */
		unsigned char byte_125; /* fine offset for sdram min cycle time (t ck avg min), ftb */
		unsigned char byte_126; /* crc for base configuration section, l-s-byte */
		unsigned char byte_127; /* crc for base configuration section, m-s-byte */
		/*
		 * block 1: module specific parameters for unbuffered memory module types only
		 */
		union { /* (unbuffered) raw card extension, module nominal height */
			unsigned char all_bits;
			struct {
				unsigned char nom_height_max:5, /* in mm */
					raw_cad_ext:3;
			} bit_fields;
		} byte_128;
		union { /* (unbuffered) module maximum thickness */
			unsigned char all_bits;
			struct {
				unsigned char front_thickness_max:4, /* in mm */
					back_thickness_max:4; /* in mm */
			} bit_fields;
		} byte_129;
		union { /* (unbuffered) reference raw card used */
			unsigned char all_bits;
			struct {
				unsigned char ref_raw_card:5,
					ref_raw_card_rev:2,
					ref_raw_card_ext:1;
			} bit_fields;
		} byte_130;
		union { /* (unbuffered) address mapping from edge connector to dram */
			unsigned char all_bits;
			struct {
				unsigned char rank_1_mapping:1,
					reserved:7;
			} bit_fields;
		} byte_131;
		unsigned char bytes_132_191[60]; /* reserved; all 0s */
		unsigned char bytes_192_255[MV_DDR_SPD_DATA_BLOCK1H_SIZE];  /* reserved; all 0s */
		unsigned char bytes_256_319[MV_DDR_SPD_DATA_BLOCK2E_SIZE];  /* reserved; all 0s */
		unsigned char bytes_320_383[MV_DDR_SPD_DATA_BLOCK2M_SIZE];  /* reserved; all 0s */
	} byte_fields;
};

// ddr5_spd_data union copied 1:1 from firmware/ebf/cn10k/libebf-dram/ebf-ddr54-spd.h
// at 17.11.2021 from ebf-devel branch.
// just renamed union from original ddr5_spd_data to mv_ddr5_spd_data to follow naming convention

/* DDR5 SPD:
 * Based on JEDEC Standard No. JC-45-2260.01 JESD400-5 DDR5 SPD Contents Document Release 0.90b:
 * Serial Presence Detect (SPD) for DDR5 SDRAM Modules
 */

/* blocks 0-1: base configuration and dram parameters */
#define DDR5_BLOCK0_AND_1_SIZE          128
/* blocks 3-6: Standard Module Parameters - Annex A.x: Solder Down, Unbuffered ,and Registered DIMM */
#define DDR5_BLOCK3_TO_6_SIZE           256
/* block 2: manufacturing information */
#define DDR5_BLOCK8_AND_9               128
/* block 3: end user programmable */
#define DDR5_BLOCK10_TO_15              128

#define DDR5_SPD_BLOCK0_TO_9_SIZE       640

union mv_ddr5_spd_data {
	unsigned char all_bytes[DDR5_SPD_BLOCK0_TO_9_SIZE];
	struct {
	/* blocks 0 and 1 : Base Configuration and DRAM Parameters*/
	union {         /* Number of Bytes in SPD Device */
		unsigned char all_bits;
	    struct {
		unsigned char reserved0 : 4,
			      spd_bytes_total : 3,
			      reserved1 : 1;
	    } bit_fields;
	} byte_0;
	union {         /* spd revision */
		unsigned char all_bits;
		struct {
		unsigned char addtions_level : 4,
			      encoding_level : 4;
	    } bit_fields;
	} byte_1;
	unsigned char byte_2;          /* Key Byte / Host Bus Command Protocol Type */
	union {         /* key byte/module type */
		unsigned char all_bits;
		struct {
		unsigned char base_module_type : 4,
			      hybrid_media : 3,
			      hybrid : 1;
	    } bit_fields;
	} byte_3;
	union {         /* First SDRAM Density and Package */
		unsigned char all_bits;
		struct {
		unsigned char sdram_density_per_die : 5,
			      die_per_package : 3;
	    } bit_fields;
	} byte_4;
	union {         /* First SDRAM Addressing */
		unsigned char all_bits;
		struct {
		unsigned char first_sdram_row_address_bits : 5,
			      first_sdram_column_address_bits : 3;
	    } bit_fields;
	} byte_5;
	union {         /* First SDRAM I/O Width */
		unsigned char all_bits;
		struct {
		unsigned char reserved : 5,
			      sdram_io_width : 3;
	    } bit_fields;
	} byte_6;
	union {         /* First SDRAM Bank Groups & Banks Per Bank Group */
		unsigned char all_bits;
		struct {
		unsigned char first_sdram_bank_per_bank_group : 3,
			      reserved : 2,
			      first_sdram_bank_group : 3;
	    } bit_fields;
	} byte_7;
	union {/* Second SDRAM Density and Package */
		unsigned char all_bits;
		struct {
		unsigned char sdram_density_per_die : 5,
			      die_per_package : 3;
	    } bit_fields;
	} byte_8;
	union {         /* Second SDRAM Addressing */
		unsigned char all_bits;
		struct {
		unsigned char second_sdram_row_address_bits : 5,
			      second_sdram_column_address_bits : 3;
	    } bit_fields;
	} byte_9;
	union {         /* Secondary SDRAM I/O Width */
		unsigned char all_bits;
		struct {
		unsigned char reserved : 5,
			      sdram_io_width : 3;
	    } bit_fields;
	} byte_10;
	union {         /* Second SDRAM Bank Groups & Banks Per Bank Group */
		unsigned char all_bits;
		struct {
		unsigned char second_sdram_bank_per_bank_group : 3,
			      reserved : 2,
			      second_sdram_bank_broup : 3;         /* all 0s */
	    } bit_fields;
	} byte_11;
	union {         /* SDRAM Optional Features*/
		unsigned char all_bits;
		struct {
		unsigned char reserved : 4,
			      bl_32 : 1,
			      soft_ppr : 1,
			      post_package_repair : 2;
	    } bit_fields;
	} byte_12;
	unsigned char byte_13; /* Reserved; must be coded as 0x00.*/
	unsigned char byte_14;         /* Reserved; must be coded as 0x00.*/
	unsigned char byte_15;         /* Reserved; must be coded as 0x00.*/
	union {         /* SDRAM Nominal Voltage, VDD */
		unsigned char all_bits;
		struct {
		unsigned char endurant : 2,
			      operable : 2,
			      nominal : 4;
	    } bit_fields;
	} byte_16;
	union { /* SDRAM Nominal Voltage, VDDQ */
		unsigned char all_bits;
		struct {
		unsigned char endurant : 2,
			      operable : 2,
			      nominal : 4;
	    } bit_fields;
	} byte_17;
	union { /* SDRAM Nominal Voltage, VPP */
		unsigned char all_bits;
		struct {
		unsigned char endurant : 2,
			      operable : 2,
			      nominal : 4;
	    } bit_fields;
	} byte_18;
	unsigned char byte_19; /* Reserved; must be coded as 0x00.*/
	unsigned char byte_20; /* SDRAM Minimum Cycle Time (tCKAVGmin) Least Significant Byte*/
	unsigned char byte_21; /* SDRAM Minimum Cycle Time (tCKAVGmin) Most Significant Byte*/
	unsigned char byte_22; /* SDRAM Minimum Cycle Time (tCKAVGmax) Least Significant Byte*/
	unsigned char byte_23; /* SDRAM Minimum Cycle Time (tCKAVGmax) Most Significant Byte*/
	unsigned char byte_24;         /* cas latencies supported, first byte */
	unsigned char byte_25;         /* cas latencies supported, second byte */
	unsigned char byte_26;         /* cas latencies supported, third byte */
	unsigned char byte_27;         /* cas latencies supported, fourth byte */
	unsigned char byte_28; /* cas latencies supported, fifth byte */
	unsigned char byte_29; /* Reserved; must be coded as 0x00.*/
	unsigned char byte_30; /* SDRAM Minimum CAS Latency Time (tAAmin) Least Significant Byte*/
	unsigned char byte_31; /* SDRAM Minimum CAS Latency Time (tAAmin) Most Significant Byte*/
	unsigned char byte_32; /* SDRAM Minimum RAS to CAS Delay Time (tRCDmin) Least Significant Byte*/
	unsigned char byte_33; /* SDRAM Minimum RAS to CAS Delay Time (tRCDmin) Most Significant Byte*/
	unsigned char byte_34; /* SDRAM Minimum Row Precharge Delay Time (tRPmin) Least Significant Byte*/
	unsigned char byte_35; /* SDRAM Minimum Row Precharge Delay Time (tRPmin) Most Significant Byte */
	unsigned char byte_36; /* SDRAM Minimum Active to Precharge Delay Time (tRASmin) Least Significant Byte*/
	unsigned char byte_37; /* SDRAM Minimum Active to Precharge Delay Time (tRASmin) Most Significant Byte*/
	unsigned char byte_38; /* SDRAM Minimum Active to Active/Refresh Delay Time (tRCmin) Least Significant Byte*/
	unsigned char byte_39; /* SDRAM Minimum Active to Active/Refresh Delay Time (tRCmin) Most Significant Byte*/
	unsigned char byte_40; /* SDRAM Minimum Write Recovery Time (tWRmin) Least Significant Byte*/
	unsigned char byte_41; /* SDRAM Minimum Write Recovery Time (tWRmin) Most Significant Byte*/
	unsigned char byte_42; /* SDRAM Minimum Refresh Recovery Delay Time (tRFC1min, tRFC1_slr min) Least Significant Byte*/
	unsigned char byte_43; /* SDRAM Minimum Refresh Recovery Delay Time (tRFC1min, tRFC1_slr min) Most Significant Byte*/
	unsigned char byte_44; /* SDRAM Minimum Refresh Recovery Delay Time (tRFC2min, tRFC2_slr min) Least Significant Byte*/
	unsigned char byte_45; /* SDRAM Minimum Refresh Recovery Delay Time (tRFC2min, tRFC2_slr min) Most Significant Byte*/
	unsigned char byte_46; /* SDRAM Minimum Refresh Recovery Delay Time (tRFCsbmin, tRFCsb_slr min) Least Significant Byte*/
	unsigned char byte_47; /* SDRAM Minimum Refresh Recovery Delay Time (tRFCsbmin, tRFCsb_slr min) Most Significant Byte*/
	unsigned char byte_48; /* SDRAM Minimum Refresh Recovery Delay Time, 3DS Different Logical Rank(tRFC1_dlr min) Least Significant Byte*/
	unsigned char byte_49; /* SDRAM Minimum Refresh Recovery Delay Time, 3DS Different Logical Rank(tRFC1_dlr min) Most Significant Byte*/
	unsigned char byte_50; /* SDRAM Minimum Refresh Recovery Delay Time, 3DS Different Logical Rank(tRFC2_dlr min) Least Significant Byte*/
	unsigned char byte_51; /* SDRAM Minimum Refresh Recovery Delay Time, 3DS Different Logical Rank(tRFC2_dlr min) Most Significant Byte*/
	unsigned char byte_52; /* SDRAM Minimum Refresh Recovery Delay Time, 3DS Different Logical Rank(tRFCsb_dlr min) Least Significant Byte*/
	unsigned char byte_53; /* SDRAM Minimum Refresh Recovery Delay Time, 3DS Different Logical Rank(tRFCsb_dlr min) Most Significant Byte*/
	unsigned char byte_54; /* Primary SDRAM Refresh Management First Byte*/
	unsigned char byte_55; /* Primary SDRAM Refresh Management Second Byte*/
	unsigned char byte_56; /* Secondary SDRAM Refresh Management First Byte*/
	unsigned char byte_57; /* Secondary SDRAM Refresh Management Second Byte*/
	unsigned char bytes_58_125[68]; /* reserved; all 0s */
	unsigned char byte_126; /*Cyclical Redundancy Code (CRC) for Base Configuration Section, Least Significant Byte*/
	unsigned char byte_127; /*Cyclical Redundancy Code (CRC) for Base Configuration Section, Most Significant Byte*/
	unsigned char bytes_128_191[64]; /* reserved; all 0s */
	/* blocks 3 to 6 : Module-Specific Section : including Solder down, unbuffered and R-DIMM*/

	unsigned char byte_192; /* SPD Revision for Module Information */
	unsigned char byte_193; /* Reserved */
	unsigned char byte_194; /* SPD Manufacturer ID Code, First Byte */
	unsigned char byte_195; /* SPD Manufacturer ID Code, Second Byte */
	union { /* SPD Device Type */
		unsigned char all_bits;
		struct {
		unsigned char device_type : 4,
			      reserved : 2,
			      devices_installed : 2;
	    } bit_fields;
	} byte_196;
	unsigned char byte_197; /* SPD Device Revision */
	unsigned char bytes_198_229[32]; /* reserved; all 0s */
	union { /* Module Nominal Height */
		unsigned char all_bits;
		struct {
		unsigned char nominal_height_max : 5,
			      reserved : 3;
	    } bit_fields;
	} byte_230;
	union {  /* Module Maximum Thickness*/
		unsigned char all_bits;
		struct {
		unsigned char thickness_max_front : 4,
			      thickness_max_back : 4;
	    } bit_fields;
	} byte_231;
	union { /* Reference Raw Card Used*/
		unsigned char all_bits;
		struct {
		unsigned char reference_design : 5,
			      design_revision : 3;
	    } bit_fields;
	} byte_232;
	union { /* DIMM Attributes*/
		unsigned char all_bits;
		struct {
		unsigned char num_rows_on_module : 2,
			      heat_spreader : 1,
			      reserved : 1,
			      temperature_grade : 4;
	    } bit_fields;
	} byte_233;
	union { /* Module Organization*/
		unsigned char all_bits;
		struct {
		unsigned char reserved0 : 3,
			      package_rank_per_channel : 3,
			      rank_mix : 1,
			      reserved1 : 1;
	    } bit_fields;
	} byte_234;
	union { /* Memory Channel Bus Width*/
		unsigned char all_bits;
		struct {
		unsigned char primary_bus_width_per_channel : 3,
			      bus_width_ext_per_channel : 2,
			      channel_per_dimm : 2,
			      reserved : 1;
	    } bit_fields;
	} byte_235;
	unsigned char bytes_236_239[4]; /* reserved; all 0s */
	unsigned char bytes_240; /* RDIMM only: Registering Clock Driver Manufacturer ID Code, First Byte */
	unsigned char bytes_241; /* RDIMM only: Registering Clock Driver Manufacturer ID Code, Second Byte*/
	unsigned char bytes_242; /* RDIMM only: Register Device Type */
	unsigned char bytes_243; /* RDIMM only: Register Revision Number */
	unsigned char bytes_246_445[200];

	/* blocks 8 to 9 : Manufacturing Information*/

	unsigned char bytes_512; /* Module Manufacturer ID Code, First Byte */
	unsigned char bytes_513; /* Module Manufacturer ID Code, Second Byte */
	unsigned char bytes_514; /* Module Manufacturing Location */
	unsigned char bytes_515_516[2]; /* Module Manufacturing Date */
	unsigned char bytes_517_520[4]; /* Module Serial Number*/
	unsigned char bytes_521_550[30]; /* Module Part Number*/
	unsigned char bytes_552; /* DRAM Manufacturer ID Code, First Byte */
	unsigned char bytes_553; /* DRAM Manufacturer ID Code, Second Byte */
	unsigned char bytes_554; /* DRAM Stepping */
	unsigned char bytes_638_639[2]; /* reserved; all 0s */

	/* blocks 8 to 9 : Manufacturing Information*/

	unsigned char bytes_640_1023[384];/* End User Programmable */

	} byte_fields;
};

union mv_spd_data {
	union mv_ddr_spd_data   ddr4_spd_data;	// DDR4
	union mv_ddr5_spd_data  ddr5_spd_data;	// DDR5
};

#endif /* _MV_DDR_SPD_H */
