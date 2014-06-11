
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
sinclude ../../../../include/autoconf.mk

ifeq "$(CONFIG_CUSTOMER_BOARD_SUPPORT)" "y"
  CFLAGS += -DCONFIG_CUSTOMER_BOARD_SUPPORT
endif
ifeq "$(CONFIG_CUSTOMER_BOARD_0)"  "y"
  CFLAGS += -DCONFIG_CUSTOMER_BOARD_0
endif
ifeq "$(CONFIG_CUSTOMER_BOARD_1)"  "y"
  CFLAGS += -DCONFIG_CUSTOMER_BOARD_1
endif

ifeq "$(CONFIG_DB_78x60_BP_REV2)"  "y"
  BOARDFLAG += -DCONFIG_DB_78x60_BP_REV2
endif
ifeq "$(CONFIG_DB_78X60_PCAC_REV2)"  "y"
  BOARDFLAG += -DCONFIG_DB_78X60_PCAC_REV2
endif
ifeq "$(CONFIG_RD_78460_SERVER_REV2)"  "y"
  BOARDFLAG += -DCONFIG_RD_78460_SERVER_REV2
endif
ifeq "$(CONFIG_DB_784MP_GP)"  "y"
  BOARDFLAG += -DCONFIG_DB_784MP_GP
endif
ifeq "$(CONFIG_DB_78X60_AMC)"  "y"
  BOARDFLAG += -DCONFIG_DB_78X60_AMC
endif
ifeq "$(CONFIG_RD_78460_CUSTOMER)"  "y"
  BOARDFLAG += -DCONFIG_RD_78460_CUSTOMER
endif

ifeq "$(CONFIG_DB_88F6710_BP)"  "y"
  BOARDFLAG += -DCONFIG_DB_88F6710_BP
endif
ifeq "$(CONFIG_RD_88F6710)"  "y"
  BOARDFLAG += -DCONFIG_RD_88F6710
endif

ifeq "$(CONFIG_ARMADA_375)"  "y"
  BOARDFLAG += -DCONFIG_ARMADA_375
  ifeq "$(CONFIG_ALP_A375_ZX_REV)" "y"
    BOARDFLAG += -DCONFIG_ALP_A375_ZX_REV
  endif
endif
ifeq "$(CONFIG_AVANTA_LP)"  "y"
  BOARDFLAG += -DCONFIG_AVANTA_LP
  ifeq "$(CONFIG_ALP_A375_ZX_REV)" "y"
    BOARDFLAG += -DCONFIG_ALP_A375_ZX_REV
  endif
endif
ifeq "$(CONFIG_ARMADA_38X)"  "y"
  BOARDFLAG += -DCONFIG_ARMADA_38X
endif
ifeq "$(CONFIG_BOBCAT2)"  "y"
  CFLAGS += -DCONFIG_BOBCAT2
endif
ifeq "$(CONFIG_DB_BOBCAT2)"  "y"
  CFLAGS += -DCONFIG_DB_BOBCAT2
endif
ifeq "$(CONFIG_RD_BOBCAT2)"  "y"
  CFLAGS += -DCONFIG_RD_BOBCAT2
endif
ifeq "$(CONFIG_RD_MTL_BOBCAT2)"  "y"
  CFLAGS += -DCONFIG_RD_BOBCAT2
endif
ifeq "$(CONFIG_ALLEYCAT3)"  "y"
  CFLAGS += -DCONFIG_ALLEYCAT3
endif
ifeq "$(CONFIG_DB_AC3)"  "y"
  CFLAGS += -DCONFIG_DB_AC3
endif

CFLAGS += $(BOARDFLAG)
INCLUDE     = -I./ -I../inc/common -I../inc/ddr3_soc/$(BOARD) -I../src_phy/$(BOARD) -I./sysEnv/$(BOARD) -I../../../../include

LDFLAGSBIN  = -T $(LDSCRIPT) $(LDFLAGS)

HOSTCPPFLAGS = $(CPPFLAGS)

TGT = plat.a
TGT_UART = plat.uart.a

DRV = drivers
UTILS = utils
SYS_ENV = sysEnv/$(BOARD)

CSRC_DRV = $(DRV)/mv_uart.c $(DRV)/mv_twsi.c $(DRV)/mv_time.c
CSRC_UTILS = $(UTILS)/utils.c $(UTILS)/printf.c $(UTILS)/mvHwsSiliconIf.c
CSRC_SYS_ENV = $(SYS_ENV)/mvBHboardEnvSpec.c

ifeq "$(CONFIG_ALLEYCAT3)"  "y"
  CSRC_UTILS += $(UTILS)/mv_seq_exec_ext.c
else ifeq "$(CONFIG_BOBCAT2)"  "y"
  CSRC_UTILS += $(UTILS)/mv_seq_exec_ext.c
else
  CSRC_UTILS += $(UTILS)/mv_seq_exec.c
endif

CSRC = $(CSRC_DRV) $(CSRC_UTILS) $(CSRC_SYS_ENV)

COBJ = $(subst .c,.o,$(CSRC))
CUART_OBJ = $(subst .c,.uart.o,$(CSRC))

all:   $(TGT) $(TGT_UART)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

%.uart.o: %.c
	$(CC) $(CFLAGS) -DNOT_USE_UART -DMV_NO_INPUT -DMV_NO_PRINT  $(CPPFLAGS) -c -o  $@ $<

$(TGT): $(COBJ)
	ar rcs $(TGT)  $(COBJ)
	$(CP) ./$@ ../

$(TGT_UART): $(CUART_OBJ)
	ar rcs $(TGT_UART)  $(CUART_OBJ)
	$(CP) ./$@ ../

setparams:
	$(HOSTCC) $(BOARDFLAG) -Wall $(INCLUDE) $(HOSTCPPFLAGS) -o $@  $(addsuffix .c, $(UTILS)/$@)
	$(CP) $@ ../

clean:
	$(RM) setparams *.raw  ./*.o  ./*.a
