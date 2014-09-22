#!/usr/bin/perl

sub bin2hex
{
	my ($in_file, $out_file, $base_addr) = @_;

	# Generate a HEX dump from the binary using GCC
	system ("\${CROSS_COMPILE}objcopy -O verilog -I binary $in_file /tmp/image.tmp");

	# Convert base address to 16 byte resolution
	my $addr = sprintf("%d", hex($base_addr));
	$addr = $addr / 16;

	unless(open $src, "</tmp/image.tmp")
	{
		print ("Failed to open input file $in_file\n");
		goto ERROR;
	}
	unless(open $dst, ">$out_file")
	{
		print ("Failed to open output file $out_file\n");
		goto ERROR;
	}

	# Move away the first line which containes an address
	my $tmp = <$src>;

	# Add address to each line
	foreach my $line (<$src>)
	{
		# Create address line
		my $addr_str = sprintf("@%07x",$addr);

		# Remove spaces line endings and ^M
		$line =~ s/\r//;
		$line =~ s/ //g;
		chomp($line);

		print $dst "$addr_str\n";
		print $dst reverse split /(..)/, $line;
		print $dst "\n";

		$addr++;
	}

	close($dst);
	close($src);

	`rm -rf /tmp/image.tmp`;

	return 0;

ERROR:
	return 1;
}

sub usage
{
	print "\nConvert binary file to Palladium HEX format\n";
	print "\nExample  : bin2phex.pl -i Image -o Image.hex -b 0x0\n";
	print "\n";
	print "Options:\n";
	print "\t-i\tInput file in binary format\n";
	print "\t-o\tOutput file in Palladium HEX format\n";
	print "\t-b\tBase address of the image in hex\n";
	print "\n";
	print "Environment Variables:\n";
	print "\tCROSS_COMPILE     Cross compiler to build U-BOOT\n";
	print "\n";
}

# Main
use strict;
use warnings;
use Getopt::Std;

use vars qw($opt_o $opt_b $opt_h $opt_i);

getopt('o:i:b');

if ($opt_h)
{
	usage();
	exit 0;
}

unless ($opt_i) {
	printf "\nError: Please specify input file\n\n";
	usage();
	exit 1;
}
unless (-e $opt_i) {
	printf "\nError: Input file $opt_i doesn't exist\n\n";
	exit 1;
}

unless ($opt_o) {
	printf "\nError: Please specify output file\n\n";
	usage();
	exit 1;
}

unless ($opt_b) {
	printf "\nError: Please specify the base address of the image\n\n";
	usage();
	exit 1;
}

my $cross = $ENV{'CROSS_COMPILE'};
if(!defined $cross){
	printf "\nError: Please set environment variable CROSS_COMPILE\n\n";
	usage();
	exit 1;
}

if(bin2hex($opt_i, $opt_o, $opt_b)) {
	printf "\nError: Failed converting image\n\n";
	exit 1;
}

exit 0;

