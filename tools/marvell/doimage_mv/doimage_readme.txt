
Glossary
=============
Boot Image	The entire boot image including all headers and the binary image

Headers block	First part of boot image that contains all headers

Main header	Mandatory part of each boot image (32 bytes). If there are more than
		one header in the boot image, the main header always appears first

Secure header	Mandatory part of a headers block for secure mode boot images.
		Immediately follows the main header.

Register header Optional part of a headers block. Contains pairs of address-value
		used for HW registers initialization prior to loading the binary
		image. Each header block can contain several register headers.

Binary header	Optional part of a header block. Contains ARM executable in a binary
		form. Each headers block can contain several binary headers.
		The binary header is executed in place without copying into the system
		RAM (since the RAM can be not accessible at this stage). This limitation
		should be taken into consideration by the binary header preparation
		tools during the code linkage stage.

Binary image	Second part of a boot image that follows the headers block.
		The binary part is an executable code usually containing a secondary
		boot loader (like u-boot or UEFI) or an OS image.

Introduction
============
------------
doimage
------------
Doimage tool is used for preparation of specially-formatted image for usage with 
BootROM to bring up a secondary boot loader or OS from various types of supported
boot media.
The boot image contains one or more headers and binary (executable) part.
For more details of boot image format please refer to Functional Specification of
specific Marvell HW.

The tool is supplied in source code that is adopted for build on Linux systems.
Supported boot media types:
- SPI/NOR flash
- NAND flash
- SATA disk
- PCI-e (PEX)
- UART
- MMC (eMMC/SD/SDHC)

In addition to a regular boot mode, doimage supports image build for a special
secure boot mode.
In this mode the binary image and all its headers are signed by RSA signatures
that are created with a help of private RSA-2048 key.
All appropriate signatures and a public RSA key needed for image verification during
the boot time are embedded in a special secure image header and prevent unauthorized
boot image replacement or modification inside the boot device.
The RSA public key used for image verification is created by doimage from private
key during the boot image build.
The validity of this RSA public key is verified at boot time against its SHA-256 digest
stored in eFuse.
The binary part of a boot image can be encrypted using AES-128-CBC
algorithm in order to prevent its reverse code engineering.
The BootROM is able to decrypt such image on the fly using symmetric key
that is stored in hidden eFuse.
The boot time AES decription is only supported for images that are copied to DRAM and
not supported for images that are executed direcly from flash device (SPI or NOR).
Please refer to Marvell HW Function Specification document for more information
about eFuse locations and fields description.

------------
hdparser
------------
The "hdparser" utility allows certain level of visualization and full verification of
a boot image created with "doimage" utility.
The "hdparser" utility accepts the boot image file name as input, prints basic headers
and binary image information and do the same verification check as the BootROM during
bootstrap process. If requested, the security segnatures for headers block and binary
image are also verified.

======================
Parameters description
======================
------------
hdparser
------------

Hdparset has only one mandatory parameter - the boot image file name.

	hdparser -[t] <boot_image_file_name>
	
-t
	Verify headers block and binary image RSA signatures,
	print RSA public key SHA-256 digest for eFuse usage

-any_other_switch
	Print command usage information

boot_image_file_name
	The boot image file name for verification

------------
doimage
------------

Doimage uses list of parameters supplied in command line.
Some parameters are mandatory and some are optional.
Below is the command line format used by doimage utility:

	doimage <mandatory parameters> [optional parameters] <image_in> <image_out> [hdr_out]

---------------------
Mandatory parameters
---------------------

-T image_type
	One of the following types: sata\uart\flash\bootrom\nand\hex\pex
	According to image type the following output formats are supported:
		sata - image for booting from a serial ATA device.
		uart - image for loading through serial port using Xmodem protocol
		flash - boot image for storage in SPI or NOR flash.
		nand - boot image for a NAND flash.
		pex - boot image for booting over PCI-e bus in EP mode.
		hex - special target for creating HEX images (one symbol per nibble).
		bin - special target for creating ROM images (one symbol per bit).
		bootrom - special output image format used only for creating BootROM test images.
	Note that hex, bin and bootrom image formats should not be used for a normal operation.

-D image_dest
	Hexadecimal destination address in system RAM used for copying the binary
	image from a boot media during the boot stage. Should be in sync with a value
	provided to linker during binary image build.
	The destination address can have a value of 0xFFFFFFFF. In this case the boot
	image is not copied into the system RAM, but executed in-place, directly from
	the boot media. Only SPI and NOR boot devices support this boot mode.

-E image_exec
	Hexadecimal address in system RAM used for image execution (entry point).
	If selected image type is "flash" and the image destination address is 0xFFFFFFFF,
	the image is executed directly from flash media bypassing the copy to RAM stage.
	In this case the execution address should be locates in SPI or NOR flash memory space.

-S image_source
	Hexadecimal byte offset of binary image part from the start of the boot image.
	For SATA devices the offset is counted in 512b sectors.
	This parameter is ignored for pex boot device. If omitted, the 0 is assumed and 
	the binary image is located next to the boot image headers.
	The offset must be aligned to 512 bytes boundary for nand boot devices.

-W hex_width
	Mandatory only for "hex" and "bin" image types and defines number of bits per row in output file.
	Supported values are 8,16,32 and 64 bits.

-P nand_pg_sz
	Decimal number defining the NAND page size. Supported values are 512, 2048, 4096 and 8192.
	This parameter is mandatory for NAND image type only.

-L nand_blk_size
	NAND block size in KBytes (decimal int in range 64-16320).
	This parameter is ignored for flashes with  512B pages since all small page flashes always use 16K block size.
	This parameter is mandatory for NAND image type only.

-N nand_cell_typ
	NAND cell technology type (char: M for MLC, S for SLC).
	This parameter defines location of bad block markers - first block page for SLC and
	last block page for MLC flash types and it is  mandatory for NAND image type only.

----------------------------------------------------------------------------------------------------------------------
NOTE:
----------------------------------------------------------------------------------------------------------------------
The NAND flash parameters from the main header are taken into account only when the main header itself is successfully
read and verified. Until then all NAND settings are either default or defined by Sample at Reset Register (SAR).
For more details about SAR fields please refer to Marvell documentation for the specific device.
For information about default values please refer to BootROM section of the device Functional Specification.
----------------------------------------------------------------------------------------------------------------------

---------------------
Optional parameters
---------------------

-G exec_file
	ASCII file name of single binary header with raw ARM code with stripped ELF header.
	The header must include array of all its parameters placed before the entry point and 
	an appropriate ARM code for saving all affected CPU registers to stack at procedure start
	and restoring them before return from the routine (similar to a regular function).
	The code should be linked to a right SRAM address and take into account the amount of
	input parameters and memory alignment.
	Single binary header will always follow register header in the boot image.
	For including more than one binary header or a special headers order please use "-C" command option.
	The example of such binary header creation can be found in u-boot sources under "tools"
	directory.

-R regs_file
	ASCII file name that contains single list of address-value pairs used for configuring 
	the device registers before the binary image execution (Register header).
	For including more than one register header or a special headers order please use "-C" command option.

-C hdrs_file
	ASCII file name that defines order and source filenames for multiple binary and 
	register headers that should be embedded into the boot image.

-X pre_pad
	Hexadecimal number of bytes to be placed before the binary image.

-Y post_pad
	Hexadecimal number of bytes to be placed after the binary image.

-H header_mode
	The header mode. The default mode is "-H 1". Supported modes:
      -H 1 : creates one file (image_out) for header and image
	-H 2 : creates two files - (image_out) for image and (hdr_out) for header
      -H 3 : creates one file (image_out) for header only
      -H 4 : creates one file (image_out) for image only

----------------------
Secure mode parameters
----------------------

-Z prv_key
	ASCII file name containing RSA private key of 2048 bit length.
	The private RSA key is used for creating signatures for the image header and binary image.
	Additionally it allows generation of public RSA key, which is embedded in the image header.
	The key file must be in TXT format similar to one generated by rsa_genkey utility supplied with
	PolarSSL library as application example.
	This parameter activates secure boot mode and enables all other secure mode parameters usage.

-A aes_key
	ASCII file name containing AES-128 symmetric key.
	The binary part of the boot image will be encrypted using AES-128-CBC algorithm.
	The key file must be in binary format (array of 16 bytes).
	See the example key file supplied with doimage utility.

-J jtag_delay
	Enable JTAG and delay the boot sequence execution by <jtag_delay> * 10 seconds.
	Used for overriding the eFuse value for devices with disabled JTAG functionality.

-B hex_box_id
	Hexadecimal Box ID in range of 0x0 to 0xFFFFFFFF

-F hex_flash_id
	Hexadecimal Flash ID in range of 0x0 to 0xFFFF

=============
File formats
=============
--------------------
RSA private key file
--------------------

RSA-2048 private key file is ACII text file in the same format as produced by rsa_genkey utility
supplied as example with PolarSSL library.

Example of a valid RSA-2048 key file content:

===== Start of file =====
N = 849D09BA6ADF22115D7354774503EDC908687290C9A66E2CE09BCDD39D5815BA6751C07F03EC4584EA8569AE18AA30BF0280CE54DCB687D50250A0FA8D645BC92BB38F59F3182B62AB40B2826F786B77CA78C8541AFED52D7EE8D0B87EBBC0D4A5FBA5B4A8F24A98C90B0C873E1E02E1F9987B10B0F275DA668AD31E4798744888A96AB218A4CD0832FCBE906B5D2A6DF3AABC19A2C2D10DAEC1FE6B004DE7BEA8358515927631724B3C2B6FE9915FBAF612DF3287BD44E0D615D615E04C5FF0A67908E5B7066432C0CE6C012FBA50B15F9DD8943279C9AF83E65A0261EEA9385031B3FB3440B7747905A4327F8E8E1CE96DDC03B70332A4DDAA17D7D8039153
E = 010001
D = 1461F2B1D565505E2CE7E20EA6EBF0AB5A807B4367CB0DDA15F7BC8E0EFC0FF2E3987C26C99A9220D8CF02AC409C3FFBCBCDB268F6B7936F18F8FD9525104CCF41C94A7A3FD3F22F2B35397AB4D96463381A9267E2F2521FB906DDB5CDBCDBE9934D776DAA1DFB7B3A43597C9BFE8EE0DEB5220D31B54289FC02D06A3204EBB253690AA130A19F45D580AC32153FA050359AB54CC0C475B2C9A9EE2479B3A6961A571C60BB5881C166DBDFBDCFD681C4B8997B5B6F72DFA34319BFC2566F61C5BD0DFEC2577871125AF2BEEC95D43E8081336E0DF5570B0951384DF313A579F447F199DFC8584EBA801FCD382BECFF35F8721390B2876C0A00AC49A13194AFB9
P = F49E06036D3D5124BF54AE02F93A7B5D804566E1DF114639CC3C4528BE8A2B8AAFBD180169F0519C890DEC92CF743F0E61FB805805BF417548EEBF0F4DF405FA514DAC3E8B772620CBFB00DC3C084751890FFD012BA12394EA58D1BB608A6090B99EE1B6AFA9CDD04EE08B14B550729F9F0FA565CA37907F13FCD0BB12B2BB47
Q = 8AC8C7EB8152987E5BD48C333D13D3D7DAD5B7D6F5801FED98583C972BAD1BE50F7D91255872D61A83B471B35B51B85E09D4D626B48F2739C56507513C04EF92FDEDA288C4C37496DFA0AB483544E600F9321ADF35955DFE6F1ECC12D07AF03169D8D7C5D8BC0A95538994057D76110B7E4360195C56273B7FC3A28419C56795
DP = B61FB9666FBCC95F4CBCA55A3D691754841E27FD5F71A3118148164DC19F69CFB60C598523F4F32E52B47540D10841B259AD40DF48581F4A4D64C40787087A32E6B9A7D6614BF2AD5471AB304DD9F40F76B6DAAE3667A0F04C929884A3DA230F5DC4375526F08BF5DDE43C68CC9DFAD97139DAF1305FB24B8AFC1E044A659C1D
DQ = 88D011B52A11A05EC06C31A1B94E38820C2AAF466B51D927C20D09FE04826C834CD7FE2410A374494318B91594AA64D3DF613F937C5FBACD8AB863944354A250079F21A12E477841DB466A538E9FD31596933C465A6E5AF09DDE8052F3CE9DC638901975E288CCEE14220617BB6C8EB503CDAB8894AC2194D808A5C71018B859
QP = 6B2700710DCD6A4503A0569F81448D146C4BF1B63D2FDC902C3A801D040AC0DCEF4376C881261DECA3EB5CC64E07B58019789AB02E2A0FC3E2B112161F735267B41127D3BF3C115AE577F27319AB40BB4364C2C86D598D943B07E08BF206D2EC3C1F4DCCB8613025259F45F09E5338E3C8B7B7E92934E2A43F257505D0B48FB7
===== End of file =====

--------------------
AES-128  key file
--------------------

Symmetrical key AES-128 file is a binary file 16 bytes long.
Example of valid AES-128 file content as it shown in HEX editor (first column displays offset):

===== Start of file =====
00000000: 2B 7E 15 16 28 AE D2 A6 | AB F7 15 88 09 CF 4F 3C
===== End of file =====

-----------------------
Headers definition file
-----------------------

Headers list file is a text file that lists multiple binary and register headers and their order.
Each row in this file defines single header type and the header file name.
First 3 symbols in each row defines the header type - BIN or REG. The rest of row contains the filename
used as source for the header creation. Each name is separated from the header type with one or more spaces.
Headers are included in final boot image in the same order as they defined in the headers definition file
and always follows the main header and secure header (in included).
Example:

===== Start of file =====
REG /somewhere/regfile1.txt
REG /somewhere/regfile2.txt
BIN /somewhere/binfile1.bin
REG /somewhere/regfile3.txt
BIN /somewhere/binfile2.bin
===== End of file =====

===============
Usage examples
===============
---------------------
SPI/NOR flash images
---------------------

1. Secure mode is enabled. JTAG support is enabled. Delay boot execution by 10 sec for JTAG probe connection.
Run DRAM registers configuration (REG header) procedure defined in dramregs.txt file followed by single BIN header
execution taken from binhdr.bin file before running U-boot.
Encode binary image using AES-128 key taken from file aes128.bin. Assign BoxID of 0xDEADBEF and FlashID of 0xBABE 
to the boot image. Sign headers block and encoded binary image by RSA signatures created with help of RSA private key
defined in file rsa_priv.txt.
Load U-boot into DRAM address 0x600000 and start execution at address 0x6A0000.

	doimage -T flash -D 0x600000 -E 0x6A0000 -Z rsa_priv.txt -A aes128.bin \
		-J 1 -B 0xDEADBEEF -F 0xBABE -R dramregs.txt -G binhdr.bin u-boot.bin u-boot_flash_dram_secure.bin

2. Non-secure boot mode. Add headers listed in headers_def.txt file to the boot image.
Load U-boot into DRAM address 0x600000 and start execution at address 0x6A0000.

	doimage -T flash -D 0x600000 -E 0x6A0000 -C headers_def.txt u-boot.bin u-boot_flash_dram.bin

3. Same as above, but binary image is located at offset 0x10000 from the image location in flash
and not immediately follows the headers block.

	doimage -T flash -D 0x600000 -E 0x6A0000 -S 0x10000 -C headers_def.txt u-boot.bin u-boot_flash_dram.bin

4. Non-secure boot mode. Add headers defined in headers_def.txt file to the boot image.
Run U-boot directly from flash without loading it into DRAM at address 0xD4000020.
The entry point is located in SPI address space at offset 0x20, right after main header.
Consult Marvell documentation for address map of specific device.

	doimage -T flash -D 0xFFFFFFFF -E 0xD4000020 u-boot.bin u-boot_spi_dram.bin

5. Same as above, but run image directly from NOR flash (see address space map for specific device)

	doimage -T flash -D 0xFFFFFFFF -E 0xD2000020 u-boot.bin u-boot_nor_dram.bin

---------------------
NAND flash images
---------------------

1. Flash page size is 2K, block size is 128K, SLC-type. Secure mode is enabled. JTAG support is enabled. 
Delay boot execution by 10 sec for JTAG probe connection. Run DRAM registers configuration (REG header) 
procedure defined in dramregs.txt file followed by single BIN header execution taken from binhdr.bin file
before running U-boot. Encode binary image using AES-128 key taken from file aes128.bin. 
Assign BoxID of 0xDEADBEF and FlashID of 0xBABE to the boot image. Sign headers block and encoded binary
image by RSA signatures created with help of RSA private key defined in file rsa_priv.txt.
Load U-boot into DRAM address 0x600000 and start execution at address 0x6A0000.

	doimage -T nand -D 0x600000 -E 0x6A0000 -P 2048 -L 128 -N S -Z rsa_priv.txt -A aes128.bin \
		-J 1 -B 0xDEADBEEF -F 0xBABE -R dramregs.txt -G binhdr.bin u-boot.bin u-boot_nand_dram_secure.bin

2. Flash page size is 4K, block size is 256K, MLC-type. Non-secure boot mode. 
Add headers listed in headers_def.txt file to the boot image.
Load U-boot into DRAM address 0x600000 and start execution at address 0x6A0000.

	doimage -T nand -D 0x600000 -E 0x6A0000 -P 4096 -L 256 -N M -C headers_def.txt u-boot.bin u-boot_nand_dram.bin

-------------
UART image
-------------

Image for UART. Secure mode is disabled. 
Run DRAM registers configuration (REG header) procedure defined in dramregs.txt file followed by single BIN header
execution taken from binhdr.bin file before running U-boot.
Load U-boot into DRAM address 0x600000 and start execution at address 0x6A0000.

	doimage -T uart -D 0x600000 -E 0x6A0000 -R dramregs.txt -G binhdr.bin u-boot.bin u-boot_uart.bin

---------------------
SATA disk image
---------------------

Secure mode is enabled. JTAG support is enabled. Delay boot execution by 10 sec for JTAG probe connection.
Run DRAM registers configuration (REG header) procedure defined in dramregs.txt file followed by single BIN header
execution taken from binhdr.bin file before running U-boot.
Encode binary image using AES-128 key taken from file aes128.bin. Assign BoxID of 0xDEADBEF and FlashID of 0xBABE
to the boot image. Sign headers block and encoded binary image by RSA signatures created with help of RSA private key
defined in file rsa_priv.txt.
Load U-boot into DRAM address 0x600000 and start execution at address 0x6A0000.

	doimage -T sata -D 0x600000 -E 0x6A0000 -Z rsa_priv.txt -A aes128.bin \
		-J 1 -B 0xDEADBEEF -F 0xBABE -G bin_hdr.bin u-boot.bin u-boot_sata_img_secure.bin


-----------------------------------------------
hdparser output example for regular boot image
-----------------------------------------------

hdrparser test_image.bin 
#####################################################
Marvell Boot Image checker version 1.0 
-----------------------------------------------------
Image file:
test_image.bin
File size = 765308 bytes
-----------------------------------------------------
                     MAIN HEADER
-----------------------------------------------------
[00:00]Image type:                SPI/NOR
[01:01]Reserved:                  0x00
[03:02]NAND page size:            0 (0x0000)
[07:04]Boot image size:           665500 (0x000A279C)
[08:08]Header version:            1 (0x01)
[11:09]Headers block size:        99808 (0x0185E0)
[15:12]Source address:            0x000185E0
[19:16]Destination address:       0x00600000
[23:20]Execution address:         0x006A0000
[24:24]Reserved:                  0x00
[25:25]NAND Block size:           0 (0KB)
[26:26]NAND Technology:           0 (N/A)
[27:27]Reserved:                  0x00
[29:28]Reserved:                  0x0000
[30:30]Num of ext. headers:       5 (0x05)
[31:31]Header block checksum:     0xCF (GOOD)
-----------------------------------------------------
             REGISTER HEADER @ 0x20
-----------------------------------------------------
[0000:0000]Header type:           0x03
[0003:0001]Header length:         144 (0x000090)
[0139:0004]Num of ADDR-VAL pairs: 17 (0x11)
[0140:0140]More headers follow:   YES
[0141:0141]Delay(mS):             0x00
[0143:0142]Reserved:              0x0000
-----------------------------------------------------
             REGISTER HEADER @ 0xb0
-----------------------------------------------------
[0000:0000]Header type:           0x03
[0003:0001]Header length:         288 (0x000120)
[0283:0004]Num of ADDR-VAL pairs: 35 (0x23)
[0284:0284]More headers follow:   YES
[0285:0285]Delay(mS):             0x00
[0287:0286]Reserved:              0x0000
-----------------------------------------------------
             BINARY HEADER @ 0x1d0
-----------------------------------------------------
[0000:0000]Header type:           0x02
[0003:0001]Header length:         49572 (0x00C1A4)
[0004:0004]Number of parameters:  2 (0x000002)
[0005:0005]Reserved:              0x00
[0007:0005]Reserved:              0x0000
[0015:0008]Parameters
[0016:49568]ARM Code link offset:  0x1e0
[49568:49568]More headers follow:   YES
[49569:49569]Reserved:              0x00
[49570:49571]Reserved:              0x0000
-----------------------------------------------------
             REGISTER HEADER @ 0xc374
-----------------------------------------------------
[0000:0000]Header type:           0x03
[0003:0001]Header length:         200 (0x0000C8)
[0195:0004]Num of ADDR-VAL pairs: 24 (0x18)
[0196:0196]More headers follow:   YES
[0197:0197]Delay(mS):             0x00
[0199:0198]Reserved:              0x0000
-----------------------------------------------------
             BINARY HEADER @ 0xc43c
-----------------------------------------------------
[0000:0000]Header type:           0x02
[0003:0001]Header length:         49572 (0x00C1A4)
[0004:0004]Number of parameters:  2 (0x000002)
[0005:0005]Reserved:              0x00
[0007:0005]Reserved:              0x0000
[0015:0008]Parameters
[0016:49568]ARM Code link offset:  0xc44c
[49568:49568]More headers follow:   NOT
[49569:49569]Reserved:              0x00
[49570:49571]Reserved:              0x0000
-----------------------------------------------------
             BOOT IMAGE @ 0x185e0
-----------------------------------------------------
Binary image checksum = 0x536C7D26 (GOOD)
************ T E S T   S U C C E E D E D ************

-----------------------------------------------
hdparser output example for secure boot image
-----------------------------------------------

./hdrparser -t test_image_secure.bin
#####################################################
Marvell Boot Image checker version 1.0 
-----------------------------------------------------
Image file:
test_image_secure.bin
File size = 766420 bytes
-----------------------------------------------------
                     MAIN HEADER
-----------------------------------------------------
[00:00]Image type:                SPI/NOR
[01:01]Reserved:                  0x00
[03:02]NAND page size:            0 (0x0000)
[07:04]Boot image size:           665524 (0x000A27B4)
[08:08]Header version:            1 (0x01)
[11:09]Headers block size:        100868 (0x018A04)
[15:12]Source address:            0x00018A20
[19:16]Destination address:       0x00600000
[23:20]Execution address:         0x006A0000
[24:24]Reserved:                  0x00
[25:25]NAND Block size:           0 (0KB)
[26:26]NAND Technology:           0 (N/A)
[27:27]Reserved:                  0x00
[29:28]Reserved:                  0x0000
[30:30]Num of ext. headers:       6 (0x06)
[31:31]Header block checksum:     0xB9 (GOOD)
-----------------------------------------------------
             SECURITY HEADER @ 0x20
-----------------------------------------------------
[0000:0000]Header type:           0x01
[0003:0001]Header length:         1060 (0x000424)
[0004:0004]Image is encrypted:    YES
[0005:0005]Reserved:              0x00
[0007:0006]Reserved:              0x0000
[0531:0008]Public Key:            30 82 01 0B ...
[0532:0532]JTAG enabled:          YES [1]
[0533:0533]Reserved:              0x00
[0535:0534]Reserved:              0x0000
[0539:0536]Box ID:                0xDEADBEEF
[0541:0540]Flash ID:              0xBABE
[0543:0542]Reserved:              0x0000
[0799:0544]Header signature:      3E 09 AB A9 ...
[1055:0800]Image signature:       73 6D D6 EF ...
[1057:1056]More headers follow:   YES
[1058:1058]Reserved:              0x00
[1060:1059]Reserved:              0x0000
-----------------------------------------------------
             REGISTER HEADER @ 0x444
-----------------------------------------------------
[0000:0000]Header type:           0x03
[0003:0001]Header length:         144 (0x000090)
[0139:0004]Num of ADDR-VAL pairs: 17 (0x11)
[0140:0140]More headers follow:   YES
[0141:0141]Delay(mS):             0x00
[0143:0142]Reserved:              0x0000
-----------------------------------------------------
             REGISTER HEADER @ 0x4d4
-----------------------------------------------------
[0000:0000]Header type:           0x03
[0003:0001]Header length:         288 (0x000120)
[0283:0004]Num of ADDR-VAL pairs: 35 (0x23)
[0284:0284]More headers follow:   YES
[0285:0285]Delay(mS):             0x00
[0287:0286]Reserved:              0x0000
-----------------------------------------------------
             BINARY HEADER @ 0x5f4
-----------------------------------------------------
[0000:0000]Header type:           0x02
[0003:0001]Header length:         49572 (0x00C1A4)
[0004:0004]Number of parameters:  2 (0x000002)
[0005:0005]Reserved:              0x00
[0007:0005]Reserved:              0x0000
[0015:0008]Parameters
[0016:49568]ARM Code link offset:  0x604
[49568:49568]More headers follow:   YES
[49569:49569]Reserved:              0x00
[49570:49571]Reserved:              0x0000
-----------------------------------------------------
             REGISTER HEADER @ 0xc798
-----------------------------------------------------
[0000:0000]Header type:           0x03
[0003:0001]Header length:         200 (0x0000C8)
[0195:0004]Num of ADDR-VAL pairs: 24 (0x18)
[0196:0196]More headers follow:   YES
[0197:0197]Delay(mS):             0x00
[0199:0198]Reserved:              0x0000
-----------------------------------------------------
             BINARY HEADER @ 0xc860
-----------------------------------------------------
[0000:0000]Header type:           0x02
[0003:0001]Header length:         49572 (0x00C1A4)
[0004:0004]Number of parameters:  2 (0x000002)
[0005:0005]Reserved:              0x00
[0007:0005]Reserved:              0x0000
[0015:0008]Parameters
[0016:49568]ARM Code link offset:  0xc870
[49568:49568]More headers follow:   NOT
[49569:49569]Reserved:              0x00
[49570:49571]Reserved:              0x0000
-----------------------------------------------------
             BOOT IMAGE @ 0x18a20
-----------------------------------------------------
Binary image checksum = 0x4F5141E2 (GOOD)
RSA key SHA256 digest for eFuse:
  BE FULL: c1 9e fc 0a 99 2c 9d a0 be 8a 9c b5 e0 ac d3 1d 
           de 67 67 fd cf 09 90 3a b8 bc a5 01 8e 99 a0 fb 
  LE REGS: [0]0afc9ec1 [1]a09d2c99 [2]b59c8abe [3]1dd3ace0 
           [4]fd6767de [5]3a9009cf [6]01a5bcb8 [7]fba0998e 
Header RSA signature verification - PASSED
Image  RSA signature verification - PASSED
************ T E S T   S U C C E E D E D ************

