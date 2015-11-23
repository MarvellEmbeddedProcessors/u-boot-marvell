#!/bin/bash

if [ -z "$MARVELL_XDB_HOME" ]; then
	#	MARVELL_XDB_HOME="/home/user/.wine/drive_c/Marvell/XDB5.7"
	echo "Please set the XDB location by setting MARVELL_XDB_HOME variable"
	exit 1
fi
echo "Add Hyperion to XDB in $MARVELL_XDB_HOME"

THOME=$MARVELL_XDB_HOME/xdb/tci/cs/jtag

if [ -z `grep -m 1 hyperion "$THOME/config1/jtag.ini"` ]; then
	cat jtag1.ini >> "$THOME/config1/jtag.ini"
	echo "Adding Hyperion settings to $THOME/config1/jtag.ini file"
else
	echo "Hyperion settings already present in $THOME/config1/jtag.ini file"
fi
if [ -z `grep -m 1 hyperion "$THOME/config2/jtag.ini"` ]; then
	cat jtag2.ini >> "$THOME/config2/jtag.ini"
	echo "Adding Hyperion settings to $THOME/config2/jtag.ini file"
else
	echo "Hyperion settings already present in $THOME/config2/jtag.ini file"
fi
if [ -z `grep -m 1 hyperion "$THOME/config3/jtag.ini"` ]; then
	cat jtag3.ini >> "$THOME/config3/jtag.ini"
	echo "Adding Hyperion settings to $THOME/config3/jtag.ini file"
else
	echo "Hyperion settings already present in $THOME/config3/jtag.ini file"
fi

THOME=$MARVELL_XDB_HOME/xdb/configurations/jtag/SoC/hyperion

if  ! [ -d "$THOME" ]; then
	mkdir "$THOME"
	echo "Creating new folder $THOME"
fi

cp ../hyperion_3core.xsf "$THOME"


