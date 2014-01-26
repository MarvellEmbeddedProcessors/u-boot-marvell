include $(TOPDIR)/config.mk
include paths.mk


ifneq ($(BOARD_TYPE),mv_fpga_board)
MAKE_PEX = yes
MAKE_SDMMC = yes
MAKE_CNTMR = yes
MAKE_SATA = yes
MAKE_RTC = yes
else
MAKE_PCI = yes
endif

ifeq ($(BOARD),alp)
MAKE_USB = yes
MAKE_PPV2 = yes
MAKE_GENERAL_U_BOOT_TOOLS = yes
endif

ifeq ($(BOARD),a375)
MAKE_USB = yes
MAKE_PPV2 = yes
MAKE_GENERAL_U_BOOT_TOOLS = yes
endif

ifeq ($(BOARD),a38x)
MAKE_USB = yes
MAKE_SATA3 = yes
MAKE_NETA = yes
MAKE_GENERAL_U_BOOT_TOOLS = yes
endif

ifeq ($(BOARD),msys)
MAKE_NETA = yes
endif

ifeq ($(BOARD_TYPE),mv_fpga_board)
MAKE_NETA = yes
endif


COMMON_OBJS     = $(COMMON_DIR)/mvCommon.o
OSSERVICES_OBJS = $(OSSERVICES_DIR)/mvOs.o
ifdef MAKE_SATA
OSSERVICES_OBJS+= $(OSSERVICES_DIR)/mvOsSata.o
endif
BOARD_OBJS      = $(BOARD_ENV_DIR)/mvBoardEnvLib.o $(BOARD_ENV_DIR)/mvBoardEnvSpec.o

HAL_IF_OBJS =	$(HAL_IF_DIR)/mvSysSpi.o	\
		$(HAL_IF_DIR)/mvSysNfc.o	\
		$(HAL_IF_DIR)/mvSysXor.o	\
		$(HAL_IF_DIR)/mvSysEthPhy.o



HAL_OBJS =	$(HAL_SPI_DIR)/mvSpi.o		\
		$(HAL_SPI_DIR)/mvSpiCmnd.o	\
		$(HAL_NFC_DIR)/mvNfc.o		\
		$(HAL_XOR_DIR)/mvXor.o		\
		$(HAL_XOR_DIR)/mvXorAddrDec.o	\
		$(HAL_DRAM_DIR)/mvDramIf.o	\
		$(HAL_ETHPHY_DIR)/mvEthPhy.o	\
		$(HAL_GPP_DIR)/mvGpp.o		\
		$(HAL_TWSI_DIR)/mvTwsi.o	\
		$(HAL_UART_DIR)/mvUart.o	\
		$(HAL_PCIIF_DIR)/mvPciIf.o	\
		$(HAL_PCIIF_UTIL_DIR)/mvPciUtils.o


USP_OBJS =	$(USP_DIR)/mv_spi.o		\
		$(USP_DIR)/nand_nfc.o		\
		$(USP_DIR)/mv_flash.o		\
		$(USP_DIR)/mv_serial.o 		\
		$(USP_DIR)/mv_i2c.o		\
		$(USP_DIR)/mv_tsc2005.o		\
		$(USP_DIR)/cmd_rcvr.o		\
		$(USP_DIR)/mv_cmd.o		\
		$(USP_DIR)/mv_dram.o		\
		$(USP_DIR)/cmd_bubt.o		\
		$(USP_DIR)/cmd_resetenv.o	\
		$(USP_DIR)/mvDramScrubbing.o	\
		$(USP_DIR)/mv_fdt.o		\
		$(USP_DIR)/cmd_ddr.o		\
		$(USP_DIR)/mv_phy.o


ifdef MAKE_PPV2
ETH_OBJS =	$(HAL_IF_DIR)/mvSysPp2.o 		\
		$(HAL_PP2_GMAC_DIR)/mvEthGmacApi.o	\
		$(HAL_PP2_GBE_DIR)/mvPp2Gbe.o 		\
		$(HAL_PP2_GBE_DIR)/mvPp2AddrDec.o 	\
		$(HAL_PP2_BM_DIR)/mvBm.o		\
		$(HAL_PP2_PRS_DIR)/mvPp2PrsHw.o 	\
		$(HAL_PP2_PRS_DIR)/mvPp2Prs.o		\
		$(HAL_PP2_CLS_DIR)/mvPp2ClsHw.o 	\
		$(HAL_PP2_CLS_DIR)/mvPp2Classifier.o	\
		$(HAL_PP2_COMMON_DIR)/mvPp2Common.o	\
		$(USP_DIR)/mv_egiga_pp2.o

USP_OBJS +=	$(USP_ETH_SWITCH_DIR)/mvSwitch.o
endif


ifdef MAKE_NETA
ETH_OBJS =	$(HAL_IF_DIR)/mvSysNeta.o		\
		$(HAL_NETA_GBE_DIR)/mvNeta.o		\
		$(HAL_NETA_GBE_DIR)/mvNetaAddrDec.o	\
		$(HAL_NETA_GBE_DIR)/mvNetaDebug.o	\
		$(USP_DIR)/mv_egiga_neta.o
endif


ifdef MAKE_USB
HAL_IF_OBJS += 	$(HAL_IF_DIR)/mvSysUsb.o
HAL_OBJS +=	$(HAL_USB_DIR)/mvUsb.o		\
		$(HAL_USB_DIR)/mvUsbAddrDec.o
USP_OBJS +=	$(USP_DIR)/mv_usb.o
endif

ifdef MAKE_PCI
HAL_IF_OBJS +=	$(HAL_IF_DIR)/mvSysPci.o
HAL_OBJS +=	$(HAL_PCI_DIR)/mvPci.o
USP_OBJS +=	$(USP_DIR)/mv_pci.o
endif


ifdef MAKE_PEX
HAL_IF_OBJS +=	$(HAL_IF_DIR)/mvSysPex.o
HAL_OBJS +=	$(HAL_PEX_DIR)/mvPex.o		\
		$(HAL_PEX_DIR)/mvPexAddrDec.o
USP_OBJS +=	$(USP_DIR)/cmd_pcie.o		\
		$(USP_DIR)/mv_pex.o
endif


ifdef MAKE_SDMMC
HAL_IF_OBJS +=	$(HAL_IF_DIR)/mvSysSdmmc.o
HAL_OBJS +=	$(HAL_MMC_DIR)/mvSdmmcAddrDec.o
endif


ifdef MAKE_CNTMR
HAL_IF_OBJS +=	$(HAL_IF_DIR)/mvSysCntmr.o
HAL_OBJS +=	$(HAL_CNTMR_DIR)/mvCntmr.o
endif


ifdef MAKE_SATA
HAL_IF_OBJS +=	$(HAL_IF_DIR)/mvSysSata.o
HAL_OBJS +=	$(HAL_SATA_CORE_DIR)/mvSata.o		\
		$(HAL_SATA_CORE_DIR)/mvSataAddrDec.o	\
		$(HAL_SATA_CORE_DIR)/mvStorageDev.o	\
		$(HAL_SATA_CORE_DIR)/mvLog.o
USP_OBJS +=	$(USP_DIR)/mv_ide.o
ifdef MAKE_SATA3
SOC_OBJS    +=	$(HAL_SATA3_DIR)/mvSata3AddrDec.o
HAL_IF_OBJS +=	$(HAL_IF_DIR)/mvSysSata3.o
endif
endif


ifdef MAKE_RTC
HAL_OBJS +=	$(HAL_RTC_DIR)/mvRtc.o
USP_OBJS +=	$(USP_DIR)/mv_rtc.o
endif


ifdef MAKE_GENERAL_U_BOOT_TOOLS
USP_OBJS +=	$(USP_DIR)/cmd_sys_restore.o	\
		$(USP_DIR)/cmd_unitInfo.o
endif

SOC_OBJS+=	$(SOC_ENV_DIR)/mvCtrlEnvAddrDec.o 	\
		$(SOC_ENV_DIR)/mvCtrlEnvLib.o		\
		$(SOC_SYS_DIR)/mvAhbToMbus.o 		\
		$(SOC_SYS_DIR)/mvCpuIf.o		\
		$(SOC_CPU_DIR)/mvCpu.o 			\
		$(SOC_DEVICE_DIR)/mvDevice.o

ifdef MAKE_PPV2
SOC_OBJS +=	$(SOC_ENV_DIR)/mvCtrlEthCompLib.o
endif

ifneq ($(BOARD),alp)
ifneq ($(BOARD),a375)
ifneq ($(BOARD),a38x)
USP_OBJS +=	$(USP_DIR)/mv_main.o
endif
endif
endif

ifneq ($(BOARD),msys)
ifneq ($(BOARD),axp)
SOBJS +=	$(USP_DIR)/jump.o 		\
		$(HAL_DRAM_DIR)/mvDramIfBasicInit.o
endif
endif

SOBJS = $(USP_DIR)/platform.o

ifdef CONFIG_AMP_SUPPORT
USP_OBJS +=	$(USP_DIR)/mv_amp.o
SOBJS +=	$(USP_DIR)/mv_micro_loader.o
endif

COBJS = $(COMMON_OBJS)		\
	$(OSSERVICES_OBJS)	\
	$(HAL_OBJS) 		\
	$(HAL_IF_OBJS)		\
	$(SOC_OBJS) 		\
	$(BOARD_OBJS) 		\
	$(USP_OBJS) 		\
	$(ETH_OBJS)

LIB  = lib$(VENDOR).o

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))
SOBJS	:= $(addprefix $(obj),$(SOBJS))

$(LIB):	$(obj).depend $(OBJS) $(SOBJS)
	$(call cmd_link_o_target, $(OBJS) $(SOBJS))

#########################################################################
# This is for $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
