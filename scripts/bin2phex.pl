#!/usr/bin/perl

# This table describes the total size read by the NFC
# controller depending on the page size and ecc strength
# The input image must contain ecc information based on
# marvell NFC data layout
our %chunk_info = (
	# page size
	2048 => {
		# ecc size
		4 =>  { data_size => 2112},
	},
	4096 => {
		# ecc size
		4 =>  { data_size => 4222},
	},
);

sub bin2hex
{
	my ($in_file, $out_file, $base_addr, $width, $page_size, $ecc_size) = @_;
	my $ecc_info;

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

	# Convert base address to 16 byte resolution
	my $addr = sprintf("%d", hex($base_addr));
	my $data_size = undef;
	if ($page_size) {
		# For Double the base address to acount
		# for the spare area page
		$addr = $addr * 2;
		$data_size = $page_size;
	}

	my @bin;
	while (<$src>)
	{
		my @bytes = split /..\K/, unpack "H*", $_;
		push(@bin, @bytes);
	}


	# extract the actual data size based on page and ecc size
	if ($ecc_size) {
		unless (exists $chunk_info{$page_size}{$ecc_size}) {
			print ("Failed to find ecc info in table\n");
			goto ERROR;
		}
		$data_size = $chunk_info{$page_size}{$ecc_size}{data_size};
	}

	my $size = scalar(@bin);
	my $in_idx = 0;
	my $out_idx = $addr;

	# Put the base address
	my $addr_str = sprintf("@%07x",$addr / $width);
	print $dst "$addr_str\n";

	while ($size > 0) {

		$byte = 0;
		while ($byte < $width) {
			print $dst @bin[$in_idx + ($width - 1) - $byte];
			$byte++;
		}

		print $dst "\n";
		$in_idx += $width;
		$out_idx += $width;
		$size -= $width;

		# PD model allocates a full page for the spare are while
		# only some if it used. Find where the real data ends
		# and increment the address to the next page.
		if(defined($data_size) and (($in_idx % $data_size) == 0)) {
			# Round up address to next page
			$out_idx = int(($out_idx + $page_size - 1) / $page_size) * $page_size ;
			my $addr_str = sprintf("@%07x",$out_idx	/ $width);
			print $dst "$addr_str\n";
		}
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
	print "\t-p\tNand page size. Can be 2048, 4096, or 8192\n";
	print "\t-e\tECC strength. Can be 4, 8, 12, or 16\n";
	print "\n";
}

# Main
use strict;
use warnings;
use Getopt::Std;

use vars qw($opt_o $opt_b $opt_h $opt_i $opt_w $opt_p $opt_e);

getopt('o:i:b:w:p:e');

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

if ($opt_p) {
	unless (($opt_p == 2048) or ($opt_p == 4096) or ($opt_w == 8192)) {
		printf "\nError: Bad page size $opt_p\n";
		usage();
		exit 1;
	}
}

if(bin2hex($opt_i, $opt_o, $opt_b, $opt_w, $opt_p, $opt_e)) {
	printf "\nError: Failed converting image\n\n";
	exit 1;
}

exit 0;

