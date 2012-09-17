#!/usr/bin/perl

# Main
use Getopt::Std;

getopt('v');

$output="/tftpboot/u-boot-2011.12/$opt_v";

$fail = system("./build.pl  -f nor -v $opt_v -b armada_xp_db -i nand:spi:nor -c -o $output/armada_xp_db/");
if($fail){
	print "\n *** Error: Build u-boot boot from NOR failed\n\n";
	exit;
}

$fail = system("./build.pl  -f spi -v $opt_v -b armada_xp_db -i nand:spi:nor -c -o $output/armada_xp_db/");
if($fail){
	print  "\n *** Error: Build u-boot boot from SPI failed\n\n";
	exit;
}

$fail = system("./build.pl  -f nand -v $opt_v  -b armada_xp_db -i nand:spi:nor -c -o $output/armada_xp_db/");
if($fail){
	print "\n *** Error: Build u-boot boot from NAND failed\n\n";
	exit;
}
$fail = system("./build.pl  -f spi -v $opt_v -b armada_xp_dbgp -i nand:spi:nor -c -o $output/armada_xp_dbgp/");
if($fail){
	print  "\n *** Error: Build u-boot boot from SPI failed\n\n";
	exit;
}
$fail = system("./build.pl  -f nor -v $opt_v -b armada_xp_dbgp -i nand:spi:nor -c -o $output/armada_xp_dbgp/");
if($fail){
	print  "\n *** Error: Build u-boot boot from SPI failed\n\n";
	exit;
}
$fail = system("./build.pl  -f nand -v $opt_v -b armada_xp_dbgp -i nand:spi:nor -c -o $output/armada_xp_dbgp/");
if($fail){
	print  "\n *** Error: Build u-boot boot from SPI failed\n\n";
	exit;
}
$fail = system("./build.pl  -f spi -v $opt_v -b armada_xp_rdserver -i spi -c -o $output//armada_xp_rdserver");
if($fail){
	print  "\n *** Error: Build u-boot bot from SPI failed\n\n";
	exit;
}

