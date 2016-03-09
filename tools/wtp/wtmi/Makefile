#/*
# * ***************************************************************************
# * Copyright (C) 2015 Marvell International Ltd.
# * ***************************************************************************
# * This program is free software: you can redistribute it and/or modify it
# * under the terms of the GNU General Public License as published by the Free
# * Software Foundation, either version 2 of the License, or any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# * ***************************************************************************
#*/

CROSS_CM3 ?= arm-linux-gnueabi-

LD       = $(CROSS_CM3)ld
CC       = $(CROSS_CM3)gcc
AS       = $(CROSS_CM3)as
OBJCOPY  = $(CROSS_CM3)objcopy
OBJDUMP  = $(CROSS_CM3)objdump
HOSTCC   = gcc

RM       = @rm -rf
MKDIR    = @mkdir -p
CD       = @cd
MV       = @mv
CAT      = @cat
PWD      = @pwd
ECHO     = @echo

CPUOPTS  = -mthumb -mcpu=cortex-m3 -mlittle-endian

BINPATH  = build
LDSCRIPT = wtmi.ld
INCLUDE  = -I.

CFLAGS   = -g -gdwarf-2 -Wall -fno-stack-protector $(INCLUDE) -Os $(CPUOPTS)
CPPFLAGS =
ASFLAGS  = -g --gdwarf-2 --warn $(INCLUDE) $(CPUOPTS)
LDFLAGS  = -nostdlib -T $(LDSCRIPT) -Xlinker "--build-id=none"


CSRC = $(wildcard *.c)
ASRC = $(wildcard *.S)

COBJ   = $(CSRC:.c=.o)
AOBJ   = $(ASRC:.S=.o)

COMPILER_FOUND  = $(shell which $(CC))
TOOLCHAIN_CHECK = \
	if [ -z "$(COMPILER_FOUND)" ]; then \
		echo "***************************************************" >&2; \
		echo "ARM GCC toolchain installation is missing!" >&2; \
		echo "Please install GNU ARM toolchain suitable for" >&2; \
		echo "compiling code for Cortex M3 targets" >&2; \
		echo "On Debian/Ubuntu run the following command for" >&2; \
		echo "the default toolchain installation:" >&2; \
		echo " sudo apt-get install gcc-arm-linux-gnueabi" >&2; \
		echo "If non-default toolchain is used, please export" >&2; \
		echo "the CROSS_CM3 environment variable pointin to it." >&2; \
		echo "For instance, with BASH shell:" >&2; \
		echo " export CROSS_CM3=/opt/arm-cross/bin/arm-linux-gnueabi" >&2; \
		echo "***************************************************" >&2; \
		exit 1; \
	fi

.SILENT:

all: clean tools $(COBJ) $(AOBJ) $(BINPATH)
	$(ECHO) "  LD    $(MAKECMDGOALS)"
	$(RM) $(BINPATH)/*
	$(CC) $(LDFLAGS)  $(AOBJ)  $(COBJ) -o $(BINPATH)/wtmi.elf
	$(OBJCOPY) -S -O binary $(BINPATH)/wtmi.elf $(BINPATH)/wtmi.bin
	if [ `uname -m` = "x86_64" ]; then $(OBJDUMP) -D -S $(BINPATH)/wtmi.elf > $(BINPATH)/wtmi.dis; fi

%.o: %.c
	$(ECHO) "  CC    $<"
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $(subst .o,.c,$@)

%.o: %.S
	$(ECHO) "  AS    $<"
	$(AS) $(ASFLAGS) -o $@ $(subst .o,.S,$@)

$(BINPATH):
	@$(MKDIR) $(BINPATH)

clean:
	$(ECHO) "  CLEAN"
	@$(RM) *.o

tools:
	@$(TOOLCHAIN_CHECK)

distclean: clean
	@$(RM) $(BINPATH)
