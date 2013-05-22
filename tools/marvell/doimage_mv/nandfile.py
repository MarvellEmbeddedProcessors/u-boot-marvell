#!/usr/bin/python

import os
import sys
import select
import optparse
import time
import binascii
import socket
import xml.dom.minidom
import datetime




def printframe(packet,f = sys.stdout):
    '''
    Print the frame nicely formatted.
    '''
    s = binascii.b2a_hex(packet)
    l = len(s)
    i = 0
    while i < l:
        if i % 32 == 0:
            f.write('\n%04x: ' % i)
        if i < l - 4:   
            f.write('%4s ' % s[i:i+4])
        else:
            f.write('%s\n'  % s[i:])         
        i = i + 4
    f.write('\n')      





    

if __name__ == '__main__':
    file_name = 'test_nand_exec_on_ddr_RTL.hex'
    output_file_name = '%s.out' % file_name
    page_size = 512
    op=optparse.OptionParser()	
    op.add_option("-p","--page_size",dest="page_size",default=page_size,type="int",help="page size")			
    op.add_option("-f","--file_name",dest="file_name",default=file_name,type='string',help="file_name")
    op.add_option("-o","--output_file_name",dest="output_file_name",default=output_file_name,type='string',help="output_file_name")	
    (options,args) = op.parse_args()

    spare_area_size = (options.page_size * 16) / 512    

    f = open(options.file_name)
    s = f.read()
    f.close()

    output = open(options.output_file_name,'wb')
    offset = 0
    count = 0
    print 'source file legth is %d bytes' % len(s)

    while offset < len(s):
        chunk_size = options.page_size*3
        chunk = s[offset:offset+chunk_size] # 2 characters + LF

        # fill remainder of last memory page with 2-byte rising counter
        while len(chunk) < chunk_size: # last chunk is smaller than page
            if (chunk_size - len(chunk)) == 1:
                chunk = chunk + "%02x\n" % count&0xFF 
            else:
                chunk = chunk + "%02x\n%02x\n" % ((count&0xFF00)>>8, count&0xFF) 
            count = count + 1
      
        chunk = chunk + options.page_size * 'FF\n' # add page with spare area and memory hole
        offset = offset + chunk_size

        output.write(chunk)
         
    output.close()

    
    




