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
$fail = system("./build.pl  -f spi -v $opt_v -b armada_xp_rdserver -i spi -c -o $output/armada_xp_rdserver");
if($fail){
	print  "\n *** Error: Build u-boot bot from SPI failed\n\n";
	exit;
}
$fail = system("./build.pl  -f spi -v $opt_v -b armada_xp_amc -i spi -c -o $output/armada_xp_amc");
if($fail){
	print  "\n *** Error: Build amc u-boot boot from SPI failed\n\n";
	exit;
}
$fail = system("./build.pl  -f spi -v $opt_v -b armada_xp_pcac -i spi -c -o $output/armada_xp_pcac");
if($fail){
	print  "\n *** Error: Build pcac u-boot \n\n";
	exit;
}
$fail = system("./build.pl  -f spi -v $opt_v -b armada_370_db -i spi -c -o $output/armada_370_db");
if($fail){
	print  "\n *** Error: Build armada_370_db u-boot \n\n";
	exit;
}
$fail = system("./build.pl  -f spi -v $opt_v -b armada_370_rd -i spi -c -o $output/armada_370_rd");
if($fail){
	print  "\n *** Error: Build armada_370_rd u-boot \n\n";
	exit;
}

