#!/usr/bin/perl


#use Getopt::Std;
 
#getopt('f:b:o:i:v:');

#  Make clean
system("make mrproper");
print "\n**** [Cleaning Make]\t*****\n\n";
#-------------------------------------------------------------
        $fail = chdir './tools/bin_hdr_armada';
         system("pwd"); 
         print " clean tools/bin_hdr_armada\n";
        $fail = system("make clean"); 
        if($fail){
        	print "\n *** Error: make clean\n\n";
        	exit 1;
        }
#-------------------------------------------------------------
        $fail = chdir ('../doimage_armada');
        system("pwd"); 
	print " clean tools/doimage_armada\n";
        $fail = system("make clean"); 
        if($fail){
        	print "\n *** Error: make clean\n\n";
        	exit 1;
        }
        $fail = chdir ('../../');
        system("pwd"); 

exit 0;
