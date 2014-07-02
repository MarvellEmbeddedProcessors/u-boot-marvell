#;/*******************************************************************************
#;Copyright (C) Marvell International Ltd. and its affiliates
#;
#;This software file (the "File") is owned and distributed by Marvell
#;International Ltd. and/or its affiliates ("Marvell") under the following
#;alternative licensing terms.  Once you have made an election to distribute the
#;File under one of the following license alternatives, please (i) delete this
#;introductory statement regarding license alternatives, (ii) delete the two
#;license alternatives that you have not elected to use and (iii) preserve the
#;Marvell copyright notice above.
#;
#;********************************************************************************
#;Marvell Commercial License Option
#;
#;If you received this File from Marvell and you have entered into a commercial
#;license agreement (a "Commercial License") with Marvell, the File is licensed
#;to you under the terms of the applicable Commercial License.
#;
#;********************************************************************************
#;Marvell GPL License Option
#;
#;If you received this File from Marvell, you may opt to use, redistribute and/or
#;modify this File in accordance with the terms and conditions of the General
#;Public License Version 2, June 1991 (the "GPL License"), a copy of which is
#;available along with the File in the license.txt file or by writing to the Free
#;Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
#;on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
#;
#;THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
#;WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
#;DISCLAIMED.  The GPL License provides additional details about this warranty
#;disclaimer.
#;********************************************************************************
#;Marvell BSD License Option
#;
#;If you received this File from Marvell, you may opt to use, redistribute and/or
#;modify this File under the following licensing terms.
#;Redistribution and use in source and binary forms, with or without modification,
#;are permitted provided that the following conditions are met:
#;
#;   *   Redistributions of source code must retain the above copyright notice,
#;	    this list of conditions and the following disclaimer.
#;
#;    *   Redistributions in binary form must reproduce the above copyright
#;        notice, this list of conditions and the following disclaimer in the
#;        documentation and/or other materials provided with the distribution.
#;
#;    ;*   Neither the name of Marvell nor the names of its contributors may be
#;        used to endorse or promote products derived from this software without
#;        specific prior written permission.
#;
#;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#;ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#;WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#;DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
#;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#;(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
#;ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#;(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#;SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#;
#;*******************************************************************************/

sinclude $(TOPDIR)/include/autoconf.mk

CFLAGS =

# CUSTOMER
ifeq "$(CONFIG_CUSTOMER_BOARD_SUPPORT)" "y"
  CFLAGS += -DCONFIG_CUSTOMER_BOARD_SUPPORT
endif
ifeq "$(CONFIG_CUSTOMER_BOARD_0)"  "y"
  CFLAGS += -DCONFIG_CUSTOMER_BOARD_0
endif
ifeq "$(CONFIG_CUSTOMER_BOARD_1)"  "y"
  CFLAGS += -DCONFIG_CUSTOMER_BOARD_1
endif

# AXP
ifeq "$(CONFIG_DB_78x60_BP_REV2)"  "y"
  CFLAGS += -DCONFIG_DB_78x60_BP_REV2
endif
ifeq "$(CONFIG_DB_78X60_PCAC_REV2)"  "y"
  CFLAGS += -DCONFIG_DB_78X60_PCAC_REV2
endif
ifeq "$(CONFIG_RD_78460_SERVER_REV2)"  "y"
  CFLAGS += -DCONFIG_RD_78460_SERVER_REV2
endif
ifeq "$(CONFIG_DB_784MP_GP)"  "y"
  CFLAGS += -DCONFIG_DB_784MP_GP
endif
ifeq "$(CONFIG_DB_78X60_AMC)"  "y"
  CFLAGS += -DCONFIG_DB_78X60_AMC
endif
ifeq "$(CONFIG_RD_78460_CUSTOMER)"  "y"
  CFLAGS += -DCONFIG_RD_78460_CUSTOMER
endif

# A370
ifeq "$(CONFIG_DB_88F6710_BP)"  "y"
  CFLAGS += -DCONFIG_DB_88F6710_BP
endif
ifeq "$(CONFIG_RD_88F6710)"  "y"
  CFLAGS += -DCONFIG_RD_88F6710
endif
ifeq "$(CONFIG_DB_88F6710_PCAC)"  "y"
  CFLAGS += -DCONFIG_DB_88F6710_PCAC
endif

# A375
ifeq "$(CONFIG_ARMADA_375)"  "y"
  CFLAGS += -DCONFIG_ARMADA_375
  ifeq "$(CONFIG_ALP_A375_ZX_REV)" "y"
    CFLAGS += -DCONFIG_ALP_A375_ZX_REV
  endif
endif

# ALP
ifeq "$(CONFIG_AVANTA_LP)"  "y"
  CFLAGS += -DCONFIG_AVANTA_LP
  ifeq "$(CONFIG_ALP_A375_ZX_REV)" "y"
    CFLAGS += -DCONFIG_ALP_A375_ZX_REV
  endif
endif

# A38x
ifeq "$(CONFIG_ARMADA_38X)"  "y"
  CFLAGS += -DCONFIG_ARMADA_38X
endif

# BOBCAT2
ifeq "$(CONFIG_BOBCAT2)"  "y"
  CFLAGS += -DCONFIG_BOBCAT2
  BC2_SOC = yes
endif
ifeq "$(CONFIG_DB_BOBCAT2)"  "y"
  CFLAGS += -DCONFIG_DB_BOBCAT2
  BC2_SOC = yes
endif
ifeq "$(CONFIG_RD_BOBCAT2)"  "y"
  CFLAGS += -DCONFIG_RD_BOBCAT2
  BC2_SOC = yes
endif
ifeq "$(CONFIG_RD_MTL_BOBCAT2)"  "y"
  CFLAGS += -DCONFIG_RD_BOBCAT2
  BC2_SOC = yes
endif

# ALLEYCAT3
ifeq "$(CONFIG_ALLEYCAT3)"  "y"
  CFLAGS += -DCONFIG_ALLEYCAT3
  AC3_SOC = yes
endif
ifeq "$(CONFIG_DB_AC3)"  "y"
  CFLAGS += -DCONFIG_DB_AC3
  AC3_SOC = yes
endif
ifeq "$(CONFIG_RD_AC3)"  "y"
  CFLAGS += -DCONFIG_RD_AC3
  AC3_SOC = yes
endif

CROSS    = $(CROSS_COMPILE_BH)
LD       = $(CROSS)ld
CC       = $(CROSS)gcc
AS       = $(CROSS)as
OBJCOPY  = $(CROSS)objcopy
OBJDUMP  = $(CROSS)objdump
HOSTCC   = gcc

RM       = @rm -rf
CP       = @cp
MKDIR    = @mkdir -p
CD       = @cd
MV       = @mv
CAT      = @cat
PWD      = @pwd
ECHO     = @echo

MVFLAGS = MV_CPU_LE=1
ifeq ($(ARMARCH),7)
CFLAGS += -mno-tune-ldrd -nostdlib -mabi=aapcs
endif

CPUOPTS  = -mthumb -mthumb-interwork -march=armv7 -mlittle-endian

BH_ROOT_DIR  = $(TOPDIR)/tools/marvell/bin_hdr
INCLUDE      = -I$(BH_ROOT_DIR)/src_ddr -I$(BH_ROOT_DIR)/src_phy/$(BOARD) -I$(BH_ROOT_DIR)/inc/common \
	       -I$(BH_ROOT_DIR)/inc/ddr3_soc/$(BOARD) -I$(BH_ROOT_DIR)/platform/sysEnv/$(BOARD) -I$(TOPDIR)/include
HOSTCFLAGS   = -Wall $(INCLUDE)

ifeq ($(BIN_HDR_DEBUG),1)
DEBUG_FLAGS += -g -O0
DEBUG_MODE_FLAG = yes
else
ifeq ($(BOARD),a38x)
DEBUG_FLAGS += -Os
DEBUG_MODE_FLAG = no
else
ifeq ($(BOARD),msys)
DEBUG_FLAGS += -Os
DEBUG_MODE_FLAG = no
else
DEBUG_FLAGS += -g -O0
DEBUG_MODE_FLAG = no
endif
endif
endif

CFLAGS   += -Wall $(INCLUDE) $(DEBUG_FLAGS) $(CPUOPTS) -msoft-float -mabi=aapcs
ifeq ($(BOARD),a38x)
CFLAGS   += -fdata-sections -ffunction-sections
else
CFLAGS   += -fPIE -fno-zero-initialized-in-bss -fno-unwind-tables
endif

CPPFLAGS = $(foreach FLAG, $(MVFLAGS), $(addprefix -D, $(FLAG)))
ASFLAGS  = $(foreach FLAG, $(MVFLAGS), $(addprefix --defsym , $(FLAG)))
LDFLAGS  = -static -nostartfiles -unwind-tables  -nostdlib -Wl,--build-id=none
