#!/bin/bash
#
# Create TIM/NTIM descriptor
#
# $1 - trusted/non trusted (1/0)
# $2 - Boot device (SPINOR/SPINAND/EMMCNORM/EMMCALT/SATA/UART)
# $3 - Path to image text files
# $4 - Clocks and DDR init data path
# $5 - Clocks preset
# $6 - Partition number
# $7 - Output TIM/NTIM file name
# $8 - Output TIMN file name (valid for trusted mode only)
# $9 - TIMN CSK sign key file name (valid for trusted mode only)
#

DATE=`date +%d%m%Y`
IMGPATH=$3
CLOCKSPATH=$4
PRESET=$5
BOOTPART=$6
OUTFILE=$7


# All file names extention
FILEEXT="txt"
# Image definition files name prefix
IMGPREF="img"
# Number of images excepting TIM
IMGSNUM=`ls -l $IMGPATH/$IMGPREF*.$FILEEXT | wc -l`
# Reserved area definition for TIM/NTIM - file name prefix
RSRVDPREF="rsrvd"
RSRVDLEN=`wc -l < $IMGPATH/$RSRVDPREF.$FILEEXT`
# DLL tuning - same for all DDR frequencies
# Located in the same folder as DDR init file
DLLTUNFILE=$CLOCKSPATH/dll_tune.$FILEEXT

# TIM/NTIM image definition file name prefix
TIMPREF="tim"
# CSK keys file name prefix
CSKPREF="csk"
# KAK key file name prefix (TIM signature)
KAKPREF="kak"

# Below values used only by TIMN
TIMNOUTFILE=$8
SIGNFILE=$9
# TIMN image definition file name prefix
TIMNPREF="timn"
# Reserved area definition for TIMN - file name prefix
RSRVD2PREF="rsrvdtimn"

usage () {
	echo ""
	echo "$0 - script for creating TIM/NTIM/TIMN image descriptors"
	echo ""
	echo "$0 <trusted> <boot_device> <files_path> <clocks_path> <clocks_preset> <output> [timN_out] [timN_key]"
	echo " <trusted>     - trusted/non trusted (supported values 0 and 1)"
	echo " <boot_device> - Boot device (Supported values SPINOR/SPINAND/EMMCNORM/EMMCALT/SATA/UART)"
	echo " <files_path>  - Path to image and keys descriptors text files"
	echo " <clocks_path> - Path to clocks and DDR initialization files"
	echo " <clocks_preset> - Name of clocks preset to use - see \"freq\" parameter in DTS file for details"
	echo " <output>      - Output TIM/NTIM file name"
	echo " [timN_out]    - Output TIMN file name (required for trusted boot only)"
	echo " [timN_key]    - TIMN CSK sign key file name (required for trusted boot only)"
	echo ""
	exit 1
}

# Parameters check
case "$1" in
[0-1])
	TRUSTED="0x0000000"$1
	if [ "$TRUSTED" = "0x00000001" ]; then
		if [ "$TIMNOUTFILE" = "" ]; then
			echo "Empty TIMN output file name!"
			usage
		fi

		# Values required for trusted boot mode
		KEYSNUM=`ls -l $IMGPATH/$CSKPREF*.$FILEEXT | wc -l`
		RSRVD2LEN=`wc -l < $IMGPATH/$RSRVD2PREF.$FILEEXT`
	fi
	;;
*)
	echo "Only 0/1 is supported as first parameter (trusted/untrusted)!"
	usage
esac

case "$2" in
SPINOR)
	FLASH="0x5350490A		; Select SPI'0A"
	;;
SPINAND)
	FLASH="0x5350491A		; Select SPI'1A"
	;;
EMMCNORM)
	FLASH="0x454D4D08		; Select MMC'08"
	;;
EMMCALT)
	FLASH="0x454D4D0B		; Select MMC'0B"
	;;
SATA)
	FLASH="0x53415432		; Select SAT'32"
	;;
UART)
	FLASH="0x55415223		; Select UAR'23"
	;;
*)
	echo "Unsupported boot device $2!"
	usage
esac

case "$PRESET" in
PRESET_CPU_600_DDR_600)
	CLOCKSFILE=$CLOCKSPATH/clocks-600-600.$FILEEXT
	DDRFILE=$CLOCKSPATH/ddr-600.$FILEEXT
	;;
PRESET_CPU_800_DDR_800)
	CLOCKSFILE=$CLOCKSPATH/clocks-800-800.$FILEEXT
	DDRFILE=$CLOCKSPATH/ddr-800.$FILEEXT
	;;
PRESET_CPU_1000_DDR_800)
	CLOCKSFILE=$CLOCKSPATH/clocks-1000-800.$FILEEXT
	DDRFILE=$CLOCKSPATH/ddr-800.$FILEEXT
	;;
*)
	echo "Unsupported clock preset $PRESET!"
	usage
esac



if [ ! -e "$DDRFILE" ]; then
	echo "Cannot find DDR init file!"
	usage
fi

if [ "$OUTFILE" = "" ]; then
	echo "Empty TIM/NTIM output file name!"
	usage
fi

# Build TIM/NTIM descriptor header

# Add TIM/NTIM to the images number, we count it too
let IMGSNUM=IMGSNUM+1
# Reserved section is in rows (one row per word), we need it in bytes
let RSRVDLEN=RSRVDLEN*4

echo "Version:                        0x00030600" > $OUTFILE
echo "Trusted:                        $TRUSTED" >> $OUTFILE
echo "Issue Date:                     0x$DATE		; today's date" >> $OUTFILE
echo "OEM UniqueID:                   0x4D52564C		; MRVL" >> $OUTFILE
echo "Stepping:                       0x00000000" >> $OUTFILE
echo "Processor Type:                 <undefined>		; ARMADALP" >> $OUTFILE
echo "Boot Flash Signature:           $FLASH" >> $OUTFILE
if [ "$TRUSTED" = "0x00000000" ]; then
	echo "Number of Images:               $IMGSNUM" >> $OUTFILE
else
	# Trusted TIM has only TIMH image
	echo "Number of Images:                1" >> $OUTFILE
	echo "Number of Keys:                  $KEYSNUM		; DSIG KAK key is not counted" >> $OUTFILE
fi
echo "Size of Reserved in bytes:      $RSRVDLEN" >> $OUTFILE
echo ""  >> $OUTFILE

# First image is always TIMH

TIMHFILE="$IMGPATH/$TIMPREF.$FILEEXT"
if [ ! -e "$TIMHFILE" ]; then
	echo "Cannot find $TIMHFILE file!"
	exit 1
else
	cat $TIMHFILE >> $OUTFILE
	echo "" >> $OUTFILE
fi

# Images or CSK keys block

if [ "$TRUSTED" = "0x00000000" ]; then
	# Untrusted (NTIM) lists images before reserved area
	i=1
	while [ "$i" -lt "$IMGSNUM" ]; do
		IMAGE="$IMGPATH/$IMGPREF-$i.$FILEEXT"
		if [ ! -e "$IMAGE" ]; then
			echo "Cannot find $IMAGE file!"
			exit 1
		fi
		cat $IMAGE >> $OUTFILE
		echo "" >> $OUTFILE
		let i=i+1
	done
else
	# Trusted (TIM) has keys block before reserved area
	i=1
	while [ "$i" -le "$KEYSNUM" ]; do
		IMAGE="$IMGPATH/$CSKPREF-$i.$FILEEXT"
		if [ ! -e "$IMAGE" ]; then
			echo "Cannot find $IMAGE file!"
			exit 1
		fi
		cat $IMAGE >> $OUTFILE
		echo "" >> $OUTFILE
		let i=i+1
	done
fi

# Reserved area

RSRVDFILE="$IMGPATH/$RSRVDPREF.$FILEEXT"
if [ ! -e "$RSRVDFILE" ]; then
	echo "Cannot find $RSRVDFILE file!"
	exit 1
else
	echo "Reserved Data:" >> $OUTFILE
	cat $RSRVDFILE >> $OUTFILE
	echo "" >> $OUTFILE
fi

# DDR init
if [ ! -e "$DDRFILE" ]; then
	echo "Cannot find $DDRFILE file!"
	exit 1
else
	echo "Extended Reserved Data:" >> $OUTFILE
	echo "Consumer ID:" >> $OUTFILE
	echo "CID: TBRI" >> $OUTFILE
	echo "PID: DDR3" >> $OUTFILE
	echo "End Consumer ID:" >> $OUTFILE
	echo "DDR Initialization:" >> $OUTFILE
	echo "DDR_PID: DDR3" >> $OUTFILE
	echo "Operations:" >> $OUTFILE
	echo "DDR_INIT_ENABLE: 0x00000001" >> $OUTFILE
	echo "End Operations:" >> $OUTFILE
	echo "Instructions:" >> $OUTFILE
	cat $CLOCKSFILE >> $OUTFILE
	cat $DDRFILE >> $OUTFILE
	cat $DLLTUNFILE >> $OUTFILE
	echo "End Instructions:" >> $OUTFILE
	echo "End DDR Initialization:" >> $OUTFILE
	echo "End Extended Reserved Data:" >> $OUTFILE
	echo "" >> $OUTFILE
fi

# Set correct partition number in the output
mv $OUTFILE $OUTFILE.temp
while IFS='' read -r line; do
	if [[ "$line" == *"Partition Number:"* ]]; then
		echo "Partition Number:               $BOOTPART" >> $OUTFILE
	else
		echo "$line" >> $OUTFILE
	fi
done < $OUTFILE.temp
rm $OUTFILE.temp

# Untrusted NTIM does not require more operations, continue with trusted stuff
if [ "$TRUSTED" = "0x00000001" ]; then
	# Trusted TIM needs a KAK key for header signature
	KAKFILE="$IMGPATH/$KAKPREF.$FILEEXT"
	if [ ! -e "$KAKFILE" ]; then
		echo "Cannot find $KAKFILE file!"
		exit 1
	else
		cat $KAKFILE >> $OUTFILE
		echo "" >> $OUTFILE
	fi
	# No more operations for TIM file

	# Now the TIMN file has to be created
	# Reserved section is in rows (one row per word), we need it in bytes
	let RSRVD2LEN=RSRVD2LEN*4

	echo "Version:                        0x00030600" > $TIMNOUTFILE
	echo "Trusted:                        $TRUSTED" >> $TIMNOUTFILE
	echo "Issue Date:                     0x$DATE		; today's date" >> $TIMNOUTFILE
	echo "OEM UniqueID:                   0x4D52564C		; MRVL" >> $TIMNOUTFILE
	echo "Stepping:                       0x00000000" >> $TIMNOUTFILE
	echo "Processor Type:                 <undefined>		; ARMADALP" >> $TIMNOUTFILE
	echo "Boot Flash Signature:           $FLASH" >> $TIMNOUTFILE
	echo "Number of Images:               $IMGSNUM" >> $TIMNOUTFILE
	echo "Number of Keys:                 0			; DSIG key is not counted" >> $TIMNOUTFILE
	echo "Size of Reserved in bytes:      $RSRVD2LEN" >> $TIMNOUTFILE
	echo ""

	# TIMN header

	TIMNHFILE="$IMGPATH/$TIMNPREF.$FILEEXT"
	if [ ! -e "$TIMNHFILE" ]; then
		echo "Cannot find $TIMNFILE file!"
		exit 1
	else
		cat $TIMNHFILE >> $TIMNOUTFILE
		echo "" >> $TIMNOUTFILE
	fi

	# TIMN images

	i=1
	while [ "$i" -lt "$IMGSNUM" ]; do
		IMAGE="$IMGPATH/$IMGPREF-$i.$FILEEXT"
		if [ ! -e "$IMAGE" ]; then
			echo "Cannot find $IMAGE file!"
			exit 1
		fi
		cat $IMAGE >> $TIMNOUTFILE
		echo "" >> $TIMNOUTFILE
		let i=i+1
	done

	# Second reserved area

	RSRVD2FILE="$IMGPATH/$RSRVD2PREF.$FILEEXT"
	if [ ! -e "$RSRVD2FILE" ]; then
		echo "Cannot find $RSRVD2FILE file!"
		exit 1
	else
		echo "Reserved Data:" >> $TIMNOUTFILE
		cat $RSRVD2FILE >> $TIMNOUTFILE
		echo "" >> $TIMNOUTFILE
	fi

	# Last TIMN component is the CSK key fir signature creation

	if [ ! -e "$SIGNFILE" ]; then
		echo "Cannot find $SIGNFILE file!"
		exit 1
	else
		cat $SIGNFILE >> $TIMNOUTFILE
		echo "" >> $TIMNOUTFILE
		exit 0
	fi
fi

# Replace partition number in the output for EMMC
if [[ "$2" = "EMMCNORM" || "$2" = "EMMCALT" ]]; then
	mv $TIMNOUTFILE $TIMNOUTFILE.temp
	while IFS='' read -r line; do
		if [[ "$line" == *"Partition Number:"* ]]; then
			echo "Partition Number:               $EMMCPART" >> $TIMNOUTFILE
		else
			echo "$line" >> $TIMNOUTFILE
		fi
	done < $TIMNOUTFILE.temp
	#rm $TIMNOUTFILE.temp
fi


exit 0
