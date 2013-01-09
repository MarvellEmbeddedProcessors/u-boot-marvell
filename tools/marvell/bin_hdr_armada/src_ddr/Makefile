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

include ../base.mk
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

SETPARAMS  = ../src_util/setparams     
PARAMSFILE = ../src_util/params
LDSCRIPT   = ./linker_ddr.scr
LDSCRIPT_uart_ddr = ./linker_uart_ddr.scr
LDSCRIPT_phy = ../src_phy/linker_phy.scr

INCLUDE     += -I./ -I../inc/ -I../../../include

HOSTCFLAGS   = -Wall $(INCLUDE)
HOSTCPPFLAGS = $(CPPFLAGS)

TGT_ddr = bin_hdr_ddr
TGT_phy = bin_hdr_phy
ULIB = ../utils.lib
UART_ULIB = ../uart_utils.lib

ASRC_ddr = ./ddr_entry_axp.s
CSRC_ddr = ./ddr3_init.c ./bin_sramcfg.c ./ddr3_spd.c ../src_suspendWUP/suspendWakeup.c

AOBJ_ddr = $(subst .s,.o,$(ASRC_ddr))
COBJ_ddr = $(subst .c,.o,$(CSRC_ddr))
CUART_ddr_OBJ = $(subst .c,.uart.o,$(CSRC_ddr))

TLIB = ./ddr3_training.lib

all: $(TGT_ddr).elf $(TGT_ddr).bin $(TGT_ddr).dis $(TGT_ddr).srec $(TGT_ddr).uart.elf  $(TGT_ddr).uart.bin

rtl: clean all
fpga: clean all
test: clean all

$(AOBJ_ddr): $(ASRC_ddr)
	$(AS) $(ASFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

%.uart.o: %.c
	$(CC) $(CFLAGS) -DNOT_USE_UART -DMV_NO_INPUT -DMV_NO_PRINT  $(CPPFLAGS) $(INCLUDE) -c -o  $@ $<

#############################################################################################
$(TGT_ddr).elf: $(AOBJ_ddr) $(COBJ_ddr) $(ULIB)  $(TLIB) $(LDSCRIPT)
	$(CC) -T $(LDSCRIPT) $(LDFLAGS) $(AOBJ_ddr) $(COBJ_ddr) $(ULIB) $(TLIB)  ../libgcc.lib -o $@
	$(CP) $@ ../

#############################################################################################
$(TGT_ddr).uart.elf: $(AOBJ_ddr) $(CUART_ddr_OBJ) $(UART_ULIB)  $(TLIB) $(LDSCRIPT_uart_ddr)
	$(CC) -T $(LDSCRIPT_uart_ddr) $(LDFLAGS) $(AOBJ_ddr) $(CUART_ddr_OBJ) $(UART_ULIB) $(TLIB) ../libgcc.lib -o $@

#############################################################################################
%.uart.bin: %.uart_elf
	$(OBJCOPY) -S -O binary $< $(addsuffix .tmp, $@)
	$(CAT) $(PARAMSFILE).raw $(addsuffix .tmp, $@) > $@
	$(RM) $(addsuffix .tmp, $@)
	$(CP) $@ ../

%.bin: %.elf
	$(OBJCOPY) -S -O binary $< $(addsuffix .tmp, $@)
	$(CAT) $(PARAMSFILE).raw $(addsuffix .tmp, $@) > $@
	$(RM) $(addsuffix .tmp, $@)
	$(CP) $@ ../

%.srec: %.elf
	$(OBJCOPY) -O srec $< $@

%.dis:  %.elf
	$(OBJDUMP) -lhaD -S $<  > $@

$(LDSCRIPT): $(SETPARAMS) ../$(TGT_phy).bin  $(LDSCRIPT_phy)
	$(SETPARAMS) -P $(PARAMSFILE).txt -B ../$(TGT_phy).bin   $(PARAMSFILE).raw $@

$(LDSCRIPT_uart_ddr): $(SETPARAMS) ../$(TGT_phy).uart.bin  $(LDSCRIPT_phy)
	$(SETPARAMS) -P $(PARAMSFILE).txt -B ../$(TGT_phy).uart.bin   $(PARAMSFILE).raw $@

.PHONY: clean

clean:
	$(RM) *.raw *.elf *.srec *.dis *.bin *.scr *.uart.bin *.uart.elf $(LDSCRIPT) 

