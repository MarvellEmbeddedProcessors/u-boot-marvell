#!/bin/bash

# This script automates sending the magic pattern to halt normal boot of Armada
# 38x processor and redirects it to boot the uart version of u-boot (u-boot-uart.mmc).

set -e

if [ "x$1" == "x" ] || [ "x$2" == "x" ]; then
	echo "$0 <serial interface - like /dev/ttyUSB1> <binary file to send via xmodem>"
	exit -1
fi

# Build the helper C program. Notic that curses should be installed
cat > send-stop-pattern.c << EOF
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>
#include <curses.h>

#define COUNT 6000
#define DELAY 1100
void main(void) {
	int i;
	initscr();
	timeout (DELAY);
	while ( 1 ) {
		/* Send the stop pattern COUNT times before checking for 0x15 */
		for (i = 0 ; i < COUNT ; i ++)
			printf ("%c%c%c%c%c%c%c%c",0xbb,0x11,0x22,0x33,0x44,0x55,0x66,0x77);
		fflush(stdout);
		fprintf (stderr, "Sleeping\n");
		fprintf (stderr, "Out of sleep\n");
		i=getch();
		fprintf (stderr, "Got something %d\n",i);
		if (i == 0x15) break;
	}
	fprintf	(stderr, "Got NACK (0x15) character\n");
	sleep(2);
}
EOF
gcc -O2 send-stop-pattern.c -o send-stop-pattern -lcurses
 
echo "Now reset or power cycle your Armada 38x board and then press enter"
./send-stop-pattern < $1 > $1
echo "The board should be in serial downloader now. Sending the binary file "
sx $2 < $1 > $1
