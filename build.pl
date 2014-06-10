#!/usr/bin/perl

use Cwd qw();

our @supported_boards = ("armada8k", "armada8k-pxp", "armada38x");
our @supported_flash = ("spi", "nand", "nor");

sub bin2hex
{
	system ("\${CROSS_COMPILE}objcopy -O verilog -I binary u-boot.bin u-boot.tmp");

	my $addr=-1;

	unless(open $src, "<./u-boot.tmp")
	{
		print ("Failed to open u-boot.tmp\n");
		goto ERROR;
	}
	unless(open $dst, ">./u-boot.hex")
	{
		print ("Failed to open u-boot.hex\n");
		goto ERROR;
	}

	foreach my $line (<$src>)
	{
		if ($addr eq -1){
			goto SKIP;
		}
		
		# Create address line
		my $addr_str = sprintf("@%07x",$addr);
	
		# Remove spaces line endings and ^M
		$line =~ s/\r//;
		$line =~ s/ //g;
		chomp($line);
		
		print $dst "$addr_str\n";
		print $dst reverse split /(..)/, $line;
		print $dst "\n";

	SKIP:
		$addr++;
	}

	close($dst);
	close($src);

	`rm -rf u-boot.tmp`;

	return 0;

ERROR:
	return 1;
}

sub usage
{
	print "\nUsage  : build.pl -f \"Flash type\" -b \"board name\" [-c] [-o \"Output file\"]\n";
	print "Example: build.pl -f spi -b armada8k -c\n";
	print "\n";
	print "Options:\n";
	print "\t-f\tBoot device. Accepts spi, nor, nand\n";
	print "\t-b\tBoard type. Accepts: armada8k, armada8k-pxp, armada38x\n";
	print "\t-c\tClean build. calls \"make mrproper\"\n";
	print "\t-o\tOutput directory. Build products will be copied to here\n";
	print "\t-h\tPrints this help message\n";
	print "\n";
	print "Environment Variables:\n";
	print "\tCROSS_COMPILE     Cross compiler to build U-BOOT\n";
	print "\tCROSS_COMPILE_BH  Cross compiler to build bin hdr\n";
	print "\n";
}

# Main
use Getopt::Std;

getopt('f:b:o:i:v:d:');

if ($opt_h)
{
	usage();
	exit 0;
}

unless ($opt_b) {
	printf "\nError: Please specify board type\n\n";
	usage();
	exit 1;
}

unless ($opt_f) {
	printf "\nError: Please specify flash type\n\n";
	usage();
	exit 1;
}

$cross    = $ENV{'CROSS_COMPILE'};
$cross_bh = $ENV{'CROSS_COMPILE_BH'};

if(!defined $cross){
	printf "\nError: Please set environment variable CROSS_COMPILE\n\n";
	usage();
	exit 1;
}

my $board = $opt_b;
my $flash = $opt_f;

#if(!defined $cross_bh){
#	printf " *** Error: Please set environment variables CROSS_COMPILE_BH\n";
#	usage();
#	exit 1;
#}

# Handle clean build
if($opt_c eq 1)
{
	unless($board ~~ @supported_boards) {
		print "\nError: Unsupported board \"$opt_b\"\n\n";
		usage();
		exit 1;
	}

	unless($flash ~~ @supported_flash) {
		print "\nError: Unsupported flash \"$flash\"\n\n";
		usage();
		exit 1;
	}

	# Clean U-Boot
	print "\n**** [Cleaning U-Boot]\t*****\n\n";
	if (system("make mrproper")) {
		print "\nError: Failed calling make mrporer\n\n";
		exit 1;
	}

	# Clean tools folder
	#my $path = Cwd::cwd();
	#chdir  ("./tools/marvell");
	#system("make clean BOARD=$boardID -s");
	#chdir  ("$path");
	
	print "\n**** [Configuring U-boot]\t*****\n\n";
	if (system("make ${board}_config")) {
		print "\nError: Failed configuring u-boot to board $board\n\n";
		exit 1;
	}

	if($flash eq "spi")      {
		$img_opts   = "";
		$flash_name = "spi";
		$img_type   = "flash";
	}
	elsif ($flash eq "nor")  {
		$img_opts   = "";
		$flash_name = "nor";
		$img_type   = "flash";
	}
	elsif  ($flash eq "nand"){
		$img_opts   = "-P 4096 -L 128 -N MLC";
		$flash_name = "nand";
		$img_type   = "nand";
	}
}

#if(defined $opt_d)
#{
#	system("echo \"DDR3LIB = $opt_d\" >> include/config.mk");
#	print "\n *** DDR3LIB = v$opt_d *********************************\n\n";
#}

# Build !
print "\n**** [Building U-BOOT]\t*****\n\n";
if (system("make -j6 -s")) {
	print "\nError: Build failed\n\n";
	exit 1;
}

unless ($board eq "armada8k-pxp")
{
	print "\n**** [Creating flash Image]\t*****\n\n";
	if (system("./tools/marvell/doimage -T uart -D 0 -E 0 -G ./tools/marvell/bin_hdr.uart u-boot.bin u-boot-$flash_name-uart.bin")) {
		print "\nError: doimage failed creating UART image \n\n";
		exit 1;
	}
	if(system("./tools/marvell/doimage -T $img_type -D 0x0 -E 0x0 $img_opts -G ./tools/marvell/bin_hdr u-boot.bin u-boot-$flash_name.bin")) {
		print "\nError: doimage failed creating image\n\n";
		exit 1;
	}	
} else {

	print "\n**** [Creating HEX Image]\t*****\n\n";
	# Create palladium compatible hex file #
	if(bin2hex()) {
		print "\nError: failed creating palladium image\n\n";
		exit 1;
	}
}

if(defined $opt_o)
{
	print "\n**** [Copying output] *****\n\n";

	system("mkdir -p $opt_o/");
	system("mkdir -p $opt_o/bin_hdr");

	# Copy images
	system("cp u-boot-$flash_name.bin $opt_o/u-boot.bin");
	system("cp u-boot-$flash_name.bin      $opt_o/");
	system("cp u-boot-$flash_name-uart.bin $opt_o/");

	# Copy meta data
	system("cp u-boot $opt_o/u-boot-$flash_name");
	system("cp u-boot.srec $opt_o/u-boot-$flash_name.srec");

	# Copy bin header images 
	system("cp tools/marvell/bin_hdr/bin_hdr.bin $opt_o/bin_hdr/");
	system("cp tools/marvell/bin_hdr/bin_hdr.elf $opt_o/bin_hdr/");
	system("cp tools/marvell/bin_hdr/bin_hdr.dis $opt_o/bin_hdr/");
	system("cp tools/marvell/bin_hdr/bin_hdr.srec $opt_o/bin_hdr/");
}

exit 0;
