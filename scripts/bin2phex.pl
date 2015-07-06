#!/usr/bin/perl

sub bin2hex
{
	my ($in_file, $out_file, $base_addr, $width) = @_;

	# Convert base address to 16 byte resolution
	my $addr = sprintf("%d", hex($base_addr));
	$addr = $addr / $width;

	unless(open ($src, "<$in_file"))
	{
		print ("Failed to open input file $in_file\n");
		goto ERROR;
	}
	binmode($src);

	unless(open $dst, ">$out_file")
	{
		print ("Failed to open output file $out_file\n");
		goto ERROR;
	}

	my @bin;
	while (<$src>)
	{
		my @bytes = split /..\K/, unpack "H*", $_;
		push(@bin, @bytes);
	}

	my $size = scalar(@bin);
	my $idx = 0;
	while ($size > 0) {
		# Print address line
		my $addr_str = sprintf("@%07x",$addr);
		print $dst "$addr_str\n";

		$byte = 0;
		while ($byte < $width) {
			print $dst @bin[$idx + ($width - 1) - $byte];
			$byte++;
		}

		print $dst "\n";
		$addr++;
		$idx += $width;
		$size -= $width;
	}

	close($dst);
	close($src);

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
	print "\t-w\tByte width of output file upto 16.\n";
	print "\t  \tMust be power of 2 (1,2,4,8...). Default = 16\n";
	print "\n";
}

# Main
use strict;
use warnings;
use Getopt::Std;

use vars qw($opt_o $opt_b $opt_h $opt_i $opt_w);

getopt('o:i:b:w');

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

# default width is 16 bytes
if ($opt_w) {
	unless (($opt_w == 1) or ($opt_w == 2) or
		($opt_w == 4) or ($opt_w == 8) or
		($opt_w == 16)) {
		printf "\nError: Width \"$opt_w\" is not a be power of 2\n\n";
		usage();
		exit 1;
	}
} else {
	$opt_w = 16;
}

if(bin2hex($opt_i, $opt_o, $opt_b, $opt_w)) {
	printf "\nError: Failed converting image\n\n";
	exit 1;
}

exit 0;

