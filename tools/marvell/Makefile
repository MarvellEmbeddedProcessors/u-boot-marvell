#
# (C) Copyright 2000-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#
sinclude $(TOPDIR)/config.mk

MARVELL_SUBDIRS += doimage_mv
MARVELL_SUBDIRS += bin_hdr

clean:
		@for dir in $(MARVELL_SUBDIRS) ; do \
		    $(MAKE) clean BOARD="$(BOARD)" -C $$dir || exit 1 ; \
		done




all:	subdirs

subdirs:
	@for dir in $(MARVELL_SUBDIRS) ; do \
	    $(MAKE) \
		HOSTOS=$(HOSTOS) \
		HOSTARCH=$(HOSTARCH) \
		HOSTCFLAGS="$(HOSTCFLAGS)" \
		HOST_LDFLAGS="$(HOST_LDFLAGS)" \
		BOARD="$(BOARD)" \
		-C $$dir || exit 1 ; \
	done

#########################################################################

# defines $(obj).depend target
sinclude $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

