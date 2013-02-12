# General definitions
CPU_ARCH    = ARM
ENDIAN      = LE
LD_ENDIAN   = -EL

ifeq ($(BIG_ENDIAN),y)
ENDIAN      = BE
LD_ENDIAN   = -EB
endif

# Main directory structure
SRC_PATH           = $(TOPDIR)/board/mv_ebu
AXP_CMN_DIR		   = $(SRC_PATH)/common
HAL_DIR            = $(AXP_CMN_DIR)/mv_hal
COMMON_DIR         = $(AXP_CMN_DIR)/common
USP_DIR            = $(AXP_CMN_DIR)/USP

HAL_IF_DIR	   = $(AXP_CMN_DIR)/mv_hal_if
CONFIG_DIR         = $(AXP_CMN_DIR)/config

ifeq ($(BOARD),avanta)
SOC_DIR            = $(SRC_PATH)/avanta
FAM_DIR            = $(SOC_DIR)/avanta_lp_family
endif

ifeq ($(BOARD),a370)
SOC_DIR            = $(SRC_PATH)/a370
FAM_DIR            = $(SOC_DIR)/armada_370_family
endif

ifeq ($(BOARD),axp)	
SOC_DIR            = $(SRC_PATH)/axp
FAM_DIR            = $(SOC_DIR)/armada_xp_family
		
endif

SOC_ENV_DIR        = $(FAM_DIR)/ctrlEnv
SOC_SYS_DIR        = $(FAM_DIR)/ctrlEnv/sys
SOC_CPU_DIR        = $(FAM_DIR)/cpu
SOC_DEVICE_DIR     = $(FAM_DIR)/device
BOARD_ENV_DIR      = $(FAM_DIR)/boardEnv
USP_ETH_SWITCH_DIR = $(USP_DIR)/ethSwitch
OSSERVICES_DIR     = $(AXP_CMN_DIR)/uboot_oss

# HAL components
HAL_DRAM_DIR       = $(HAL_DIR)/ddr2_3
HAL_ETHPHY_DIR     = $(HAL_DIR)/eth-phy
HAL_FLASH_DIR      = $(HAL_DIR)/norflash
HAL_PCI_DIR        = $(HAL_DIR)/pci
HAL_PCIIF_DIR      = $(HAL_DIR)/pci-if
HAL_PCIIF_UTIL_DIR = $(HAL_DIR)/pci-if/pci_util
HAL_RTC_DIR        = $(HAL_DIR)/rtc/integ_rtc
HAL_SFLASH_DIR     = $(HAL_DIR)/sflash
HAL_SATA_CORE_DIR  = $(HAL_DIR)/sata/CoreDriver/
HAL_CNTMR_DIR      = $(HAL_DIR)/cntmr
HAL_GPP_DIR        = $(HAL_DIR)/gpp
HAL_IDMA_DIR       = $(HAL_DIR)/idma
HAL_PEX_DIR        = $(HAL_DIR)/pex
HAL_TWSI_DIR       = $(HAL_DIR)/twsi
HAL_TWSI_ARCH_DIR  = $(HAL_TWSI_DIR)/Arch$(CPU_ARCH)
HAL_NETA_DIR	   = $(HAL_DIR)/neta
HAL_NETA_GBE_DIR   = $(HAL_NETA_DIR)/gbe
HAL_UART_DIR       = $(HAL_DIR)/uart
HAL_XOR_DIR        = $(HAL_DIR)/xor
HAL_USB_DIR        = $(HAL_DIR)/usb
HAL_SATA_DIR       = $(HAL_DIR)/sata
HAL_MFLASH_DIR     = $(HAL_DIR)/mflash
HAL_SPI_DIR        = $(HAL_DIR)/spi
HAL_TS_DIR	       = $(HAL_DIR)/ts
HAL_CESA_DIR	   = $(HAL_DIR)/cesa
HAL_NFC_DIR	       = $(HAL_DIR)/nfc
HAL_MMC_DIR	       = $(HAL_DIR)/sdmmc

# Internal compile definitions
MV_DEFINE = -DMV_UBOOT -DMV_CPU_$(ENDIAN) -DMV_$(CPU_ARCH)

# Internal include path
HAL_PATH           = -I$(HAL_DIR) -I$(HAL_IF_DIR) -I$(HAL_SATA_CORE_DIR)
COMMON_PATH        = -I$(COMMON_DIR) -I$(AXP_CMN_DIR)
OSSERVICES_PATH    = -I$(OSSERVICES_DIR)
USP_PATH           = -I$(USP_DIR) -I$(TOPDIR) -I$(USP_ETH_SWITCH_DIR) -I$(HAL_IF_DIR)
SOC_PATH	       = -I$(FAM_DIR) -I$(SOC_DIR) -I$(SOC_SYS_DIR) -I$(SOC_ENV_DIR) -I$(SOC_CPU_DIR) -I$(SOC_DEVICE_DIR) -I$(CONFIG_DIR) -I$(HAL_IF_DIR) -I$(HAL_NETA_DIR)
BOARD_PATH	       = -I$(BOARD_ENV_DIR)

# Compiler and linker flags
CFLAGS   += $(MV_DEFINE) $(OSSERVICES_PATH) -I$(TOPDIR) $(HAL_PATH) $(COMMON_PATH) \
            $(USP_PATH) $(SOC_PATH) $(BOARD_PATH) $(SYS_PATH)

CPPFLAGS += $(CFLAGS)

AFLAGS  += $(MV_DEFINE) $(OSSERVICES_PATH) $(HAL_PATH) $(COMMON_PATH) \
           $(USP_PATH) $(SOC_PATH) $(BOARD_PATH) $(SYS_PATH)
