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

	unless(open $zero, "< /dev/zero") {
		print ("Failed to open output file /dev/zero\n");
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

	#--------------------------------------------------------------------------

	for ($index = 0; $index < $img_num; $index++) {

		unless(open ($src, "<$img_names[$index]")) {
			print ("Failed to open input file $img_names[$index]\n");
			goto ERROR;
		}
		binmode($src);

		my $addr = $img_offset[$index];
		my $byte;
		my $next_addr = 0;

		# offset of the next image if any
		if ($index < ($img_num - 1)) {
			$next_addr = $img_offset[$index + 1];
		}

		# Copy source image data
		while (read($src, $byte, 1)) {

			print $dst $byte;
			$addr++;
		}

		close($src);

		# Pad the destination image
		while ($addr < $next_addr) {

			read($zero, $byte, 1);
			print $dst $byte;
			$addr++;
		}
	}

	close($dst);
	close($zero);

	return 0;
ERROR:
	return 1;
}

sub usage
{
	print "\nConvert TIM/NTIM binary files to flash IMG format\n";
	print "\nAccording to TIM/NTIM configuration file\n";
	print "\nExample  : tim2img.pl -i tim.txt -o Image.img\n";
	print "\n";
	print "Options:\n";
	print "\t-i\tInput TIM/NTIM configuration file in text format\n";
	print "\t-o\tOutput file in flash IMG format\n";
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

