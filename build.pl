#!/usr/bin/perl

use Cwd qw();

sub HELP_MESSAGE
{
	print "\nUsage  : build -f \"Flash type\" -v X.X.X [-b \"board name\"] [-c] [-o \"Output file\"]\n";
	print "Example: build -f nor -b pcac -c\n";
	print "\n";
	print "Options:\n";
	print "\t-f\tBoot device. Accepts spi, nor, nand\n";
        print "\t-b\tBoard type. Accepts:\n";
        print "\t\tarmada_xp_db, armada_xp_rdnas, armada_xp_pcac armada_xp_rdserver armada_xp_dbgp armada_xp_rdcustomer\n";
        print "\t\tarmada_370_db armada_370_rd, armada_375\n";
	print "\t-c\tClean build. calls make mrproper\n";
	print "\t-o\tOutput dir/file. The image will be copied into this dir/file\n";
	print "\t-e\tBig Endian. If not specified Little endian is used\n";
	print "\t-i\tSupported interfaces. A \":\" seperated list of\n";
	print "\t-v\tSW version (add to binary file name u-boot--axp-X.X.X-spi.bin)\n";
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
 
getopt('f:b:o:i:v:');

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
# Validate flash type
if($opt_f eq "spi")
{
	$img_opts   = "";
	$flash_name = "spi";
	$img_type   = "flash";
}
elsif ($opt_f eq "nor")
{
	$img_opts   = "";
	$flash_name = "nor";
	$img_type   = "flash";
}
elsif  ($opt_f eq "nand")
{
	$img_opts   = "-P 4096 -L 128 -N MLC";
	$flash_name = "nand";
	$img_type   = "nand";
}
else
{
	if (defined) {
		print "\n *** Error: Bad flash type $opt_f specified\n\n";
	}
	else {
		print "\n *** Error: Flash type unspecified\n\n";
	}
	HELP_MESSAGE();
	exit 1;
}

# Handle clean build
if($opt_c eq 1)
{
	if(($opt_b eq "armada_xp_db") or
           ($opt_b eq "armada_xp_rdnas") or
           ($opt_b eq "armada_xp_pcac") or
           ($opt_b eq "armada_xp_amc") or
           ($opt_b eq "armada_xp_rdserver") or
           ($opt_b eq "armada_xp_dbgp") or
		   ($opt_b eq "armada_370_db") or
		   ($opt_b eq "armada_370_rd") or
   		   ($opt_b eq "avanta_lp_fpga") or		   		   
		   ($opt_b eq "avanta_lp") or
		   ($opt_b eq "armada_375") or
           ($opt_b eq "armada_xp_rdcustomer") )
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
				$boardID="avanta";
                                $targetBoard = substr $board, 10;
		}
		elsif ( (substr $board,7 , 3) eq "375" ) {
				$boardID="a375";
                                $targetBoard = substr $board, 7;
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
	print " clean tools\n";
	chdir  ("./tools/marvell/doimage_mv");
        system("make clean");
	chdir  ("../bin_hdr");
        system("make clean");
	chdir  ("$path");
	print "\n**** [Configuring Make] version $opt_v\t to board $targetBoard *****\n\n";
	system("make ${board}_config");

	# Set pre processors
	print "\n**** [Setting Macros]\t*****\n\n";
	if($opt_f eq "spi")      {
		system("echo \"#define MV_SPI_BOOT\" >> include/config.h");
		system("echo \"#define MV_INCLUDE_SPI\" >> include/config.h");
                print "Boot from SPI\n";

	}
	elsif ($opt_f eq "nor")  {
		system("echo \"#define MV_NOR_BOOT\" >> include/config.h");
                print "Boot from NOR\n";
	}
	elsif  ($opt_f eq "nand"){
		system("echo \"#define MV_NAND_BOOT\" >> include/config.h");
                print "Boot from NAND\n";
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

		print " Support flash: ";
		foreach $if (@interfaces)
		{
			if   ($if eq "spi"){
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

# Build !
print "\n**** [Building U-BOOT]\t*****\n\n";
$fail = system("make -j5 -s");

if($fail){
	print "\n *** Error: Build failed\n\n";
	exit 1;
}

#Create Image and Uart Image
print "\n**** [Creating Image for $boardID]\t*****\n\n";
if($boardID eq "axp") {
        $failUart = system("./tools/marvell/doimage -T uart -D 0 -E 0 -C ./tools/marvell/bin_hdr/bin_hdr.uart.bin u-boot.bin u-boot-axp-$opt_v-$flash_name-$targetBoard-uart.bin");
        $fail = system("./tools/marvell/doimage -T $img_type -D 0x0 -E 0x0 $img_opts -G ./tools/marvell/bin_hdr/bin_hdr.bin u-boot.bin u-boot-axp-$opt_v-$flash_name-$targetBoard.bin");

}
elsif($boardID eq "a370"){
	$failUart=system("./tools/marvell/doimage -T uart -D 0 -E 0  -G ./tools/marvell/bin_hdr/bin_hdr.bin u-boot.bin u-boot-$boardID-$opt_v-$flash_name-$targetBoard-uart.bin");
	$fail = system("./tools/marvell/doimage -T $img_type -D 0x0 -E 0x0 $img_opts -G ./tools/marvell/bin_hdr/bin_hdr.bin u-boot.bin u-boot-a370-$opt_v-$flash_name-$targetBoard.bin");
}
elsif($boardID eq "avanta"){
	$failUart=system("./tools/marvell/doimage -T uart -D 0 -E 0  -G ./tools/marvell/bin_hdr/bin_hdr.bin u-boot.bin u-boot-$boardID-$opt_v-$flash_name-$targetBoard-uart.bin");
	$fail = system("./tools/marvell/doimage -T $img_type -D 0x0 -E 0x0 $img_opts -G ./tools/marvell/bin_hdr/bin_hdr.bin u-boot.bin u-boot-$boardID-$opt_v-$flash_name-$targetBoard.bin");
}
elsif($boardID eq "a375"){
	$failUart=system("./tools/marvell/doimage -T uart -D 0 -E 0  -G ./tools/marvell/bin_hdr/bin_hdr.bin u-boot.bin u-boot-$boardID-$opt_v-$flash_name-$targetBoard-uart.bin");
	$fail = system("./tools/marvell/doimage -T $img_type -D 0x0 -E 0x0 $img_opts -G ./tools/marvell/bin_hdr/bin_hdr.bin u-boot.bin u-boot-$boardID-$opt_v-$flash_name-$targetBoard.bin");
}

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

	if($boardID eq "avanta") {
		system("cp u-boot.bin $opt_o/u-boot.bin");
	}
	else {
		system("mkdir -p $opt_o/$endian/$opt_f");
		system("mkdir -p $opt_o/bin_hdr");
		system("cp u-boot-$boardID-$opt_v-$flash_name-$targetBoard.bin $opt_o/u-boot.bin");
		system("cp u-boot-$boardID-$opt_v-$flash_name-$targetBoard.bin $opt_o/$endian/$opt_f/ ");
		system("cp u-boot $opt_o/$endian/$opt_f/u-boot-$boardID-$opt_v-$flash_name-$targetBoard");
		system("cp u-boot.srec $opt_o/$endian/$opt_f/u-boot-$boardID-$opt_v-$flash_name-$targetBoard.srec");
		system("cp u-boot-$boardID-$opt_v-$flash_name-$targetBoard-uart.bin $opt_o/$endian/$opt_f/");

		system("cp tools/marvell/bin_hdr/bin_hdr.bin $opt_o/bin_hdr/");
		system("cp tools/marvell/bin_hdr/bin_hdr.elf $opt_o/bin_hdr/");
		system("cp tools/marvell/bin_hdr/bin_hdr.dis $opt_o/bin_hdr/");
		system("cp tools/marvell/bin_hdr/bin_hdr.srec $opt_o/bin_hdr/");
	}
}

exit 0;
