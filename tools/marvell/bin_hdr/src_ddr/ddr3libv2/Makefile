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

include ../../base.mk

TIP_INC = $(BH_ROOT_DIR)/src_ddr/ddr3libv2/h

INCLUDE = -I$(TIP_INC)/Os -I$(TIP_INC)/Os/gtOs -I$(TIP_INC)/Os/common/siliconIf \
	  -I$(TIP_INC)/SoC -I$(TIP_INC)/Silicon -I$(TIP_INC)/Os/common/configElementDb \
	  -I$(TIP_INC)/Driver -I$(TIP_INC)/Driver/ddr3  -I$(BH_ROOT_DIR)/inc/common -I$(BH_ROOT_DIR)/inc/common/gtOs\
	  -I$(BH_ROOT_DIR)/inc/ddr3_soc/$(BOARD) -I$(BH_ROOT_DIR)/inc/ddr3_soc/$(INCNAME) \
	  -I$(BH_ROOT_DIR)/src_ddr  -I$(BH_ROOT_DIR)/platform/sysEnv/$(BOARD)

ifeq ($(DDRTYPE),ddr4)
	INCLUDE += -I$(BH_ROOT_DIR)/src_ddr/ddr3libv2/src/Driver/ddr4/h
endif

TGT = ddr_$(LIBNAME).a
TGT_UART = ddr_$(LIBNAME).uart.a

TLIB = ./$(DDRTYPE)_training_$(LIBNAME).lib

TDDR4SUBLIB = ./$(DDRTYPE)_training_$(LIBNAME)sub.lib

TSRC = $(wildcard ./src/Driver/ddr3/*.c)
TSRC += ./src/Silicon/mvHwsDdr3$(SILNAME).c
ifeq "$(BOARD)"  "msys"
	TSRC += ./src/Silicon/mvHwsDdr3Msys.c
endif
TSRC += ./src/Soc/ddr3_$(BOARDNAME)_training.c
TSRC += ./src/Soc/ddr3_hws_hw_training.c
TSRC += ./src/Os/gtOs/mvXor.c


TSRCDDR4 = $(wildcard ./src/Driver/ddr4/src/*.c)

TOBJ = $(TSRC:.c=.o)
TOBJDDR4 = $(TSRCDDR4:.c=.o)

ifeq ($(DDR4SUBLIB),yes)
	TARGETS = $(TLIB) $(TDDR4SUBLIB)
else
	TARGETS = $(TLIB)
endif


#############global flags enable/disable features to save footprint###########
# exclude debug function not relevent for SoC( set by default)
CFLAGS += -DEXCLUDE_SWITCH_DEBUG -DDDR_VIEWER_TOOL
ifeq "$(CONFIG_BOBCAT2)"  "y"
CFLAGS += -DMV_HWS_EXCLUDE_DEBUG_PRINTS
endif
#remove all debug prints from training( unset by default)
#CFLAGS += -DSILENT_LIB
CFLAGS += -DLIB_FUNCTIONAL_DEBUG_ONLY

#Flag to support static training algo functions( unset by default)
ifeq ($(BOARD),a38x)
 #CFLAGS += -DSTATIC_ALGO_SUPPORT
else
CFLAGS += -DSTATIC_ALGO_SUPPORT
endif
#flag to support ODT test debug function( unset by default)
#CFLAGS += -DODT_TEST_SUPPORT

# Flag to enable RX IO BIST Test
# CFLAGS += -DMV_HWS_RX_IO_BIST
# Flag enable IO BIST of CMD/ADDR Test (in addition to MV_HWS_RX_IO_BIST)
# CFLAGS += -DMV_HWS_RX_IO_BIST_ETP

#############end of global flags #############################################


all:   $(TARGETS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

%.uart.o: %.c
	$(CC) $(CFLAGS) -DNOT_USE_UART -DMV_NO_INPUT -DMV_NO_PRINT  $(CPPFLAGS) -c -o  $@ $<

$(TDDR4SUBLIB): $(TOBJDDR4)
	$(RM) ./$@
	ar rcs $(TDDR4SUBLIB) $(TOBJDDR4)
	$(CP) ./$@ ../lib

$(TLIB): $(TOBJ)
	$(RM) ./$@
	ar rcs $(TLIB) $(TOBJ)
	$(CP) ./$@ ../lib

clean:
	$(RM) ./*.o  ./*.a
