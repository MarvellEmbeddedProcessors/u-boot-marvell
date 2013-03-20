#!/usr/bin/python

# Boot command
string = '\xBB\x11\x22\x33\x44\x55\x66\x77' * 1024
open('Boot_pattern.txt','wb').write(string)
# Wrong command
string = '\xBB\x11\x11\x11\x44\x55\x66\x77' * 10 * 1024
open('Wrong_pattern.txt','wb').write(string)
# Debug command
string = '\xDD\x11\x22\x33\x44\x55\x66\x77' * 1024
open('Debug_pattern.txt','wb').write(string)
