#!/usr/bin/perl

sub tim2hex
{
	my ($in_file, $out_file) = @_;
	my $row;
	my $index;
	my $img_num = 0;
	my @img_names = ();
	my @img_offset = ();

	unless(open ($cfg, "<$in_file")) {
		print ("Failed to open input file $in_file\n");
		goto ERROR;
	}

	unless(open $dst, ">$out_file") {
		print ("Failed to open output file $out_file\n");
		goto ERROR;
	}

	# Parse configuration file
	# Find a number of boot images and their names
	while ($row = <$cfg>) {
		chomp $row;
		($name, $value) = split(":", $row);
		# trim spaces from both ends
		$name =~ s/^\s+|\s+$//g;
		$value =~ s/^\s+|\s+$//g;

		if ($img_num == 0) {
			if ($name eq "Number of Images") {
				$img_num = $value;
				$index = 0;
				print ("Total number of images to process - $img_num\n");
			}
		} else {
			if ($name eq "Image Filename") {
				$img_names[$index] = $value;
				print ("is $img_names[$index]\n");
				$index++;
			} elsif ($name eq "Flash Entry Address") {
				$img_offset[$index] = hex($value);
				$offs_str = sprintf("%08x", $img_offset[$index]);
				print ("$index Image at offset $offs_str ");
			}
		}
	}

	close($cfg);

	if ($img_num < 1) {
		print ("The \"Number of Images\" field was not found in $in_file\n");
		goto ERROR;
	}

	#	Example of the required file format (each address gets data in 8 bit chunks):
	#--------------------------------------------------------------------------
	#	$ADDRESSFMT H
	#	$DATAFMT H
	#	$DEFAULTVALUE 0

	#	1/03;48;49;53
	#	6/80
	#	150/aa;bb;cc
	#	163/ad
	#--------------------------------------------------------------------------
	#	What is in the example:
	#	Initialize address 1 to 4 with values 03, 48, 49, 53 respectively
	#	Address 4 & 5 are filled with 0
	#	Address 6 with value 80
	#	Address 7 to 149 are filled with 0
	#	Address 150-152 with values aa; bb; cc respectively
	#	Address 163 with value ad

	# Print the PHEX header
	print $dst "\$ADDRESSFMT H\n";
	print $dst "\$DATAFMT H\n";
	print $dst "\$DEFAULTVALUE 0\n\n";

	for ($index = 0; $index < $img_num; $index++) {

		unless(open ($src, "<$img_names[$index]")) {
			print ("Failed to open input file $img_names[$index]\n");
			goto ERROR;
		}
		binmode($src);

		# Print image data
		my $addr = $img_offset[$index];
		my $byte;
		my $byte_idx = 0; # bytes in row

		while (read($src, $byte, 1)) {
			if ($byte_idx == 0) {
				# Print image address @ row start position
				print $dst sprintf("\n%08x/", $addr);
			} else {
				print $dst ";";
			}

			print $dst sprintf("%02x", unpack('C', $byte));
			$byte_idx++;

			# end of row
			if ($byte_idx == 16) {
				#print $dst "\n";
				$byte_idx = 0;
				$addr += 16;
			}
		}

		close($src);
	}

	close($dst);

	return 0;
ERROR:
	return 1;
}

sub usage
{
	print "\nConvert TIM/NTIM binary files to Palladium HEX format\n";
	print "\nAccording to TIM/NTIM configuration file\n";
	print "\nExample  : tim2phex.pl -i tim.txt -o Image.hex\n";
	print "\n";
	print "Options:\n";
	print "\t-i\tInput TIM/NTIM configuration file in text format\n";
	print "\t-o\tOutput file in Palladium HEX format\n";
	print "\n";
}

# Main
use strict;
use warnings;
use Getopt::Std;

use vars qw($opt_o $opt_h $opt_i $opt_s);

getopt('o:i:s:h');

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


if(tim2hex($opt_i, $opt_o)) {
	printf "\nError: Failed converting image\n\n";
	exit 1;
}

exit 0;

