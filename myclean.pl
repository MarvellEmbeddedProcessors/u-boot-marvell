#!/usr/bin/perl


#use Getopt::Std;
 
#getopt('b:v:');

#  Make clean
system("make mrproper");
if(defined $opt_b) {
$boardID=$opt_b;
}
else {
   $boardID="alp";
}


print "\n**** [Cleaning Make]\t*****\n\n";
#-------------------------------------------------------------
        $fail = chdir './tools/marvell/bin_hdr';
         system("pwd"); 
         print " clean ./tools/marvell/bin_hdr\n";
        $fail = system("make clean BOARD=alp");
        if($fail){
        	print "\n *** Error: make clean\n\n";
        	exit 1;
        }
#-------------------------------------------------------------
        $fail = chdir ('../doimage_mv');
        system("pwd"); 
	print " clean tools/marvell/doimage_mv\n";
        $fail = system("make clean"); 
        if($fail){
        	print "\n *** Error: make clean\n\n";
        	exit 1;
        }
        $fail = chdir ('../../../');
        system("pwd"); 

exit 0;
