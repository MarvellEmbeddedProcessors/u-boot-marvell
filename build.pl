#!/usr/bin/perl

use Cwd qw();

sub HELP_MESSAGE
{
	print "\nUsage  : build -f \"Flash type\" -v X.X.X -b \"board name\" [-c] [-o \"Output file\"]\n";
	print "Example: ./build -f spi -v 13T3 -b avanta_lp -c\n";
	print "\n";
	print "Options:\n";
	print "\t-f\tBoot device. Accepts spi, nor, nand\n";
	print "\t-b\tBoard type. Accepts:\tavanta_lp , avanta_lp_customer0 , avanta_lp_customer1\n";
	print "\t\t\t\t\tarmada_38x, armada_38x_customer0, armada_38x_customer1\n";
	print "\t\t\t\t\tarmada_375, armada_375_customer0, armada_375_customer1\n";
	print "\t\t\t\t\tbobcat2_db, bobcat2_rd, bobcat2_customer0, bobcat2_customer1\n";
	print "\t-c\tClean build. calls make mrproper\n";
	print "\t-o\tOutput dir/file. The image will be copied into this dir/file\n";
	print "\t-e\tBig Endian. If not specified Little endian is used\n";
	print "\t-i\tSupported interfaces, seperated by \":\" -  Accepts [spi:nor:nand]\n";
	print "\t-v\tSW version (add to binary file name u-boot-alp-X.X.X-spi.bin)\n";
	print "\t\tinterfaces. Supports spi, nor, nand. the boot \n";
	print "\t\tinterface will always be suppored\n";
	print "\n";
	print "Environment Variables:\n";
	print "\tCROSS_COMPILE     Cross compiler to build U-BOOT\n";
	print "\tCROSS_COMPILE_BH  Cross compiler to build bin hdr\n";
	print "\n";
}

# Main
use Getopt::Std;

getopt('f:b:o:i:v:d:');

if((!defined $opt_b) or
	(!defined $opt_f) or
	(!defined $opt_v)) {
	printf "\n *** Error: Please set required compilation options\n";
	HELP_MESSAGE();
	exit 1;
}

$cross    = $ENV{'CROSS_COMPILE'};
$cross_bh = $ENV{'CROSS_COMPILE_BH'};

if(!defined $cross){
	printf " *** Error: Please set environment variables CROSS_COMPILE\n";
	HELP_MESSAGE();
	exit 1;
}
if(!defined $cross_bh){
	printf " *** Error: Please set environment variables CROSS_COMPILE_BH\n";
	HELP_MESSAGE();
	exit 1;
}

# Handle clean build
if($opt_c eq 1)
{
	if(($opt_b eq "armada_xp_dbgp") or
		($opt_b eq "avanta_lp_fpga") or
		($opt_b eq "avanta_lp") or
		($opt_b eq "avanta_lp_customer0") or
		($opt_b eq "avanta_lp_customer1") or
		($opt_b eq "armada_375") or
		($opt_b eq "armada_375_customer0") or
		($opt_b eq "armada_375_customer1") or
		($opt_b eq "armada_38x") or
		($opt_b eq "armada_38x_customer0") or
		($opt_b eq "armada_38x_customer1") or
		($opt_b eq "bobcat2_db") or
		($opt_b eq "bobcat2_rd_mtl") or
		($opt_b eq "bobcat2_rd") or
		($opt_b eq "bobcat2_customer0") or
		($opt_b eq "bobcat2_customer1") )
	{
		$board = $opt_b;
		if( (substr $board,7 , 3) eq "370" ) {
			$boardID="a370";
			$targetBoard = substr $board, 11;
		}
		elsif ( (substr $board,7 , 2) eq "xp" ) {
			$boardID="axp";
			$targetBoard = substr $board, 10;
		}
		elsif ( (substr $board,7 , 2) eq "lp" ) {
			$boardID="alp";
		}
		elsif ( (substr $board,7 , 3) eq "375" ) {
			$boardID="a375";
		}
		elsif ( (substr $board,7 , 3) eq "38x" ) {
			$boardID="a38x";
		}
		elsif ( (substr $board,0 , 7) eq "bobcat2" ) {
			$boardID="msys";
			$targetBoard = substr $board, 8;
		}

		# if board string contains "customer", use customer define for binary_header
		if (index($board, "customer") != -1){
			system("echo \"#define CONFIG_CUSTOMER_BOARD_SUPPORT 1\" >> include/config.h");
		}
	}
	else
	{
		if (defined) {
			print "\n *** Error: Bad board type $opt_b specified\n\n";
		}
		else {
			print "\n *** Error: Board type unspecified\n\n";
		}
		HELP_MESSAGE();
		exit 1;
	}

	# Configure Make
	system("make mrproper");
	print "\n**** [Cleaning Make]\t*****\n\n";

	my $path = Cwd::cwd();
	chdir  ("./tools/marvell");
	system("make clean BOARD=$boardID -s");
	chdir  ("$path");
	system("make ${board}_config");

	# Set pre processors
	print "\n**** [Setting Macros]\t*****\n\n";
	if($opt_f eq "spi")      {
		system("echo \"#define MV_SPI_BOOT\" >> include/config.h");
		system("echo \"#define MV_INCLUDE_SPI\" >> include/config.h");
		print "Boot from SPI\n";
		$img_opts   = "";
		$flash_name = "spi";
		$img_type   = "flash";
	}
	elsif ($opt_f eq "nor")  {
		system("echo \"#define MV_NOR_BOOT\" >> include/config.h");
                print "Boot from NOR\n";
		$img_opts   = "";
		$flash_name = "nor";
		$img_type   = "flash";
	}
	elsif  ($opt_f eq "nand"){
		system("echo \"#define MV_NAND_BOOT\" >> include/config.h");
		print "Boot from NAND\n";
		$flash_name = "nand";
		$img_type   = "nand";
		if( ($boardID eq "axp") or
			($boardID eq "msys") or
                        ($boardID eq "a38x")) {
			$img_opts   = "-P 4096 -L 128 -N MLC";
		}
		elsif($boardID eq "a375") {
			$img_opts   = "-P 4096 -L 256 -N MLC";
		}
		elsif($boardID eq "alp") {
			$img_opts   = "-P 2048 -L 128 -N SLC";
		}
		print "Image options =  $img_opts\n\n";
	}
	else
	{
		if (defined $opt_f) {
			print "\n *** Error: Bad flash type $opt_f specified\n\n";
		}
		else {
			print "\n *** Error: Flash type unspecified\n\n";
		}
		HELP_MESSAGE();
		exit 1;
	}

	# Big endian place holder
	if(defined $opt_e) {

	$endian = "be";
	system("echo \"#define __BE\" >> include/config.h");
	system("echo \"BIG_ENDIAN = y\" >> include/config.mk");
	system("echo \"LDFLAGS += -EB  \" >> include/config.mk");
	system("echo \"LDFLAGS_FINAL += -be8  \" >> include/config.mk");
	system("echo \"  * Big Endian byte ordering \"");
	system("echo \"PLATFORM_CPPFLAGS += -march=armv7-a \" >>  arch/arm/cpu/armv7/config.mk");
	system("echo \"#define CPU_ARMARCH7 \" >> include/config.h");
	system("echo \"  * ARM Architecture 7 - Using be8 compile flag\"");
	system("echo \"CPPFLAGS += -falign-labels=4\" >> include/config.mk");
	system("echo \"CFLAGS += -mno-tune-ldrd\" >> include/config.mk");
	print "** BIG ENDIAN ** \n";
	}
	else {
		$endian = "le";
		print "** Little ENDIAN ** \n";
	}

	#Interface support
	if(defined $opt_i)
	{
		@interfaces = split(':', $opt_i);

		if (($boardID eq "a370") and
			(grep{$_ eq 'nor'} @interfaces)  and
			(grep{$_ eq 'nand'} @interfaces))
		{
			print"\n *** Error: Armada-370 does not Support nand and nor interfaces together\n";
			exit 1;
		}

		print "Support flash: ";
		foreach $if (@interfaces)
		{
			if($if eq "spi"){
				system("echo \"#define MV_INCLUDE_SPI\" >> include/config.h");
				print "SPI ";
			}
			elsif($if eq "nor"){
				system("echo \"#define MV_INCLUDE_NOR\" >> include/config.h");
				print "NOR ";
			}
			elsif($if eq "nand"){
				system("echo \"#define MV_NAND\" >> include/config.h");
				print "NAND ";
			}
			else {
				print " *** Warning: Ignoring unrecognized interface - $if";
			}
		}
		print "\n";
	}
}

if(defined $opt_d)
{
	system("echo \"DDR3LIB = $opt_d\" >> include/config.mk");
	print "\n *** DDR3LIB = v$opt_d *********************************\n\n";
}
if($opt_z eq 1)
{
	if ($boardID eq "alp" or $boardID eq "a375"){
		print "\n\nBuild U-Boot $boardID for Zx revision\n\n";
		system("echo \"#define CONFIG_ALP_A375_ZX_REV 1\" >> include/config.h");
	}
}
# Build !
print "\n**** [Building U-BOOT]\t*****\n\n";
$fail = system("make -j6 -s");

if($fail){
	print "\n *** Error: Build failed\n\n";
	exit 1;
}


if( ($boardID eq "alp") or
    ($boardID eq "a375") or
    ($boardID eq "a38x") ) {
	$targetBoard = "";
	if (($boardID eq "alp" or $boardID eq "a375") and $opt_z eq 1){
		$targetBoard = "-Z";
	}
}
else {
	$targetBoard = "-$targetBoard";
}


#Create Image and Uart Image
print "\n**** [Creating Image]\t*****\n\n";

$failUart = system("./tools/marvell/doimage -T uart -D 0 -E 0 -G ./tools/marvell/bin_hdr/bin_hdr.uart.bin u-boot.bin u-boot-$boardID-$opt_v-$flash_name$targetBoard-uart.bin");
$fail = system("./tools/marvell/doimage -T $img_type -D 0x0 -E 0x0 $img_opts -G ./tools/marvell/bin_hdr/bin_hdr.bin u-boot.bin u-boot-$boardID-$opt_v-$flash_name$targetBoard.bin");

if($fail){
	print "\n *** Error: Doimage failed\n\n";
	exit 1;
}
if($failUart){
	print "\n *** Error: Doimage for uart image failed\n\n";
	exit 1;
}

if(defined $opt_o)
{
	print "\n**** [Copying Image]\tto ",$opt_o,"  *****\n\n";

	system("mkdir -p $opt_o/$endian/$opt_f");
	system("mkdir -p $opt_o/bin_hdr");
	system("cp u-boot-$boardID-$opt_v-$flash_name$targetBoard.bin $opt_o/u-boot.bin");
	system("cp u-boot-$boardID-$opt_v-$flash_name$targetBoard.bin $opt_o/$endian/$opt_f/ ");
	system("cp u-boot $opt_o/$endian/$opt_f/u-boot-$boardID-$opt_v-$flash_name$targetBoard");
	system("cp u-boot.srec $opt_o/$endian/$opt_f/u-boot-$boardID-$opt_v-$flash_name$targetBoard.srec");
	system("cp u-boot-$boardID-$opt_v-$flash_name$targetBoard-uart.bin $opt_o/$endian/$opt_f/");

	system("cp tools/marvell/bin_hdr/bin_hdr.bin $opt_o/bin_hdr/");
	system("cp tools/marvell/bin_hdr/bin_hdr.elf $opt_o/bin_hdr/");
	system("cp tools/marvell/bin_hdr/bin_hdr.dis $opt_o/bin_hdr/");
	system("cp tools/marvell/bin_hdr/bin_hdr.srec $opt_o/bin_hdr/");
}

exit 0;
