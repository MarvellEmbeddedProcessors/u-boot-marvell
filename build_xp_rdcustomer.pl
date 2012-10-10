#!/usr/bin/perl

# Main
use Getopt::Std;
 
getopt('v');


$fail = system("./build.pl  -f spi -v $opt_v -b armada_xp_rdcustomer -i spi -c -o ");
if($fail){
	print  "\n *** Error: Build u-boot for armada_xp_rdcustomer boot from SPI failed\n\n";
	exit;
}

