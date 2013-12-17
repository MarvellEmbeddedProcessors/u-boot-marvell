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
#.SECONDARY:

include ./base.mk
sinclude ../../../include/autoconf.mk

ifeq "$(MAKECMDGOALS)" "rtl"
  MVFLAGS += MV_RTL_PLATFORM=1 MV_NO_PRINT=1
endif
ifeq "$(MAKECMDGOALS)" "test"
  MVFLAGS += MV_TEST_PLATFORM=1 MV_NO_REAL_HW=1
endif
ifeq "$(MAKECMDGOALS)" "fpga"
  MVFLAGS += MV_FPGA_HW=1 MV_NO_REAL_HW=1
endif

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

ifeq "$(CONFIG_DB_88F6710_BP)"  "y"
  CFLAGS += -DCONFIG_DB_88F6710_BP
endif
ifeq "$(CONFIG_RD_88F6710)"  "y"
  CFLAGS += -DCONFIG_RD_88F6710
endif

ifeq "$(CONFIG_ARMADA_375)"  "y"
  CFLAGS += -DCONFIG_ARMADA_375
endif
ifeq "$(CONFIG_AVANTA_LP)"  "y"
  CFLAGS += -DCONFIG_AVANTA_LP
endif

ifeq "$(CONFIG_ARMADA_38X)"  "y"
  CFLAGS += -DCONFIG_ARMADA_38X
endif
SETPARAMS  = setparams
PARAMSFILE = params
LDSCRIPT   = linker.scr
DRAMREGS   = ./dramregs.txt

INCLUDE     = -I./ -I./../ -I./src_phy_$(BOARD)/ -I./src_ddr/  -I./inc/ -I../../../include

ifneq ($(BOARD),alp)
	ifneq ($(BOARD),a375)
		ifneq ($(BOARD),a38x)
			LDFLAGSBIN  = -T linker.scr $(LDFLAGS)
		else
			LDFLAGSBIN  = -T linker_a38x.scr $(LDFLAGS)
		endif
	else
		LDFLAGSBIN  = -T linker_alp_a375.scr $(LDFLAGS)
	endif
else
	LDFLAGSBIN  = -T linker_alp_a375.scr $(LDFLAGS)
endif



HOSTCFLAGS   = -Wall $(INCLUDE)
HOSTCPPFLAGS = $(CPPFLAGS)

TGT = bin_hdr

ASRC = bin_entry.s
CSRC = mvdispacher.c

AOBJ = $(subst .s,.o,$(ASRC))
COBJ = $(subst .c,.o,$(CSRC))
CUART_OBJ = $(subst .c,.uart.o,$(CSRC))

LIB_PHY = phy_$(BOARD).a
LIB_DDR = ddr_$(BOARD).a
LIB_UTILS = utils.a
LIB_SWUP = suspendWUP.a
LIB_GENERAL_INIT = generalInit.a

LIB_PHY_UART = phy_$(BOARD).uart.a
LIB_DDR_UART = ddr_$(BOARD).uart.a
LIB_UTIL_UART = utils.uart.a
LIB_SWUP_UART = suspendWUP.uart.a
LIB_GENERAL_INIT_UART = generalInit.uart.a

TLIB = ./src_ddr/ddr3_training_$(BOARD).lib

COMPONENT_SUBDIRS = src_util src_phy_$(BOARD) src_ddr src_suspendWUP src_generalInit
HDR_COMPONENTS = $(LIB_GENERAL_INIT) $(LIB_PHY) $(LIB_DDR) $(TLIB) $(LIB_SWUP)
HDR_COMPONENTS_UART = $(LIB_GENERAL_INIT_UART) $(LIB_PHY_UART) $(LIB_DDR_UART) $(TLIB) $(LIB_SWUP_UART)

#how to add new component to binary header:
#EXAMPLE_COMPONENT=no
#ifeq ($(EXAMPLE_COMPONENT),yes)
#COMPONENT_SUBDIRS += src_example
#HDR_COMPONENTS += component_example.a
#HDR_COMPONENTS_UART += component_example.uart.a
#CFLAGS += -DCONFIG_EXAMPLE_COMPONENT
#endif

all:   subdirs $(TGT).bin $(TGT).uart.bin  $(TGT).dis $(TGT).uart.dis $(TGT).srec $(TGT).uart.srec

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

%.uart.o: %.c
	$(CC) $(CFLAGS) -DNOT_USE_UART -DMV_NO_INPUT -DMV_NO_PRINT  $(CPPFLAGS) -c -o  $@ $<

$(TGT).bin: $(TGT).elf
	$(OBJCOPY) -S -O binary $< $(addsuffix .tmp, $@)
	$(CAT) $(PARAMSFILE).raw $(addsuffix .tmp, $@) > $@
	$(RM) $(addsuffix .tmp, $@)

$(TGT).uart.bin: $(TGT).uart.elf
	$(OBJCOPY) -S -O binary $< $(addsuffix .tmp, $@)
	$(CAT) $(PARAMSFILE).raw $(addsuffix .tmp, $@) > $@
	$(RM) $(addsuffix .tmp, $@)

$(TGT).srec: $(TGT).elf
	$(OBJCOPY) -O srec $< $@

$(TGT).dis:  $(TGT).elf
	$(OBJDUMP) -lhaD -S $<  > $@

$(TGT).uart.srec: $(TGT).uart.elf
	$(OBJCOPY) -O srec $< $@

$(TGT).uart.dis:  $(TGT).uart.elf
	$(OBJDUMP) -lhaD -S $<  > $@

$(TGT).elf: subdirs  $(AOBJ) $(COBJ) $(SETPARAM) $(LDSCRIPT)
	$(CC) $(LDFLAGSBIN) $(AOBJ) $(COBJ) $(HDR_COMPONENTS) $(LIB_UTILS) libgcc.lib -o $@

$(TGT).uart.elf: $(AOBJ) $(CUART_OBJ) subdirs
	$(CC) $(LDFLAGSBIN) $(AOBJ) $(CUART_OBJ) $(HDR_COMPONENTS_UART) $(LIB_UTIL_UART) libgcc.lib -o $@

$(LDSCRIPT): $(SETPARAMS)
	./$(SETPARAMS) -P $(PARAMSFILE).txt -R $(DRAMREGS) $(PARAMSFILE).raw $@

$(SETPARAMS):
	$(MAKE) $(SETPARAMS) BOARD="$(BOARD)"  HOSTOS=$(HOSTOS) HOSTARCH=$(HOSTARCH) HOSTCFLAGS="$(HOSTCFLAGS)" HOST_LDFLAGS="$(HOST_LDFLAGS)"  -C ./src_util

subdirs:
	@for dir in $(COMPONENT_SUBDIRS) ; do \
	    $(MAKE) all \
		HOSTOS=$(HOSTOS) \
		HOSTARCH=$(HOSTARCH) \
		HOSTCFLAGS="$(HOSTCFLAGS)" \
		HOST_LDFLAGS="$(HOST_LDFLAGS)" \
		BOARD="$(BOARD)" \
		-C $$dir || exit 1 ; \
	done

clean:
	$(RM) *.raw *.elf *.srec *.dis *.bin *.a *.uart.bin *.uart.elf ./src/*.o  $(LDSCRIPT) $(SETPARAMS)
	@for dir in $(COMPONENT_SUBDIRS) ; do \
	    $(MAKE) clean -s -C $$dir || exit 1 ; \
	done


