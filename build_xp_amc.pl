#!/usr/bin/perl

# Main
use Getopt::Std;
 
getopt('v');


$fail = system("./build.pl  -f spi -v $opt_v -b armada_xp_amc -i spi -c -o ");
if($fail){
	print  "\n *** Error: Build amc u-boot boot from SPI failed\n\n";
	exit;
}

$fail = system("cp u-boot-axp-$opt_v-spi-amc.bin /tftpboot/u-boot.bin");
if($fail){
	print  "\n *** Error: copy u-boot-amc boot from SPI failed\n\n";
	exit;
}

print  "====> copied cp u-boot-axp-$opt_v-spi-amc.bin /tftpboot/u-boot.bin \n\n";

