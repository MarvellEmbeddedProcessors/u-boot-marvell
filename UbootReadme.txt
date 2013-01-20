Read me for Armada XP/370 U-Boot
================================

1. How to upgrade to a new version of U-Boot
============================================

	Assuming you have a U-Boot running, follow these instructions:
	--------------------------------------------------------------
	From U-Boot prompt:
	1. setenv serverip <IP address of your TFTP server>
	2. setenv ipaddr <IP address for the board>
	3. bubt <u-boot binary image name> - You can find compiled U-Boot images in the U-Boot release under the images folder.
	   Select the appropriate image based on your configuration.
	   Example: bubt u-boot.bin
	4. If you want to override the environment, Answer y when asked.

	Note:
	-----
	You must use a compatible U-Boot image according to your boot device and sample at reset configuration.
	If you are currently running U-Boot from SPI, then bubt an appropriate U-Boot image from SPI.
	If you are currently running U-Boot from NAND, then bubt an appropriate U-Boot image from NAND.
	If you are currently running U-Boot from NOR, then bubt an appropriate U-Boot image from NOR.

	Burning new U-Boot using Lauterbach debugger:
	---------------------------------------------
	1. Connect the JTAG cable to the board and open Lauterbach Trace32 program.
	2. Turn on the board.
	3. Run Batchfile -> select the appropriate cmm configuration file for your DDR DIMM.
		The cmm files can be found as part of the U-Boot release directory "/tools/debugger_scripts".
	4. Load U-Boot elf file. This file can be found under the release directory:
		"Images\<board name>\le\<spi/nand/nor>\" folder.
		Select the file with no extension, called "u-boot-<board name>".
	5. Run->Go (F7).
	6. When U-Boot loads you can use the instructions for updating an existing U-Boot above.

		* NOTE: need to place per88sv581xv7.per/per88sv581xv6.per under T32 ( Windows ).

	Burning new U-Boot using NAND commands (For boards support NAND):
	---------------------------------------
	1. run the following commands:
	2. setenv serverip <IP address of your TFTP server>
	3. setenv ipaddr <IP address for the board>
	4. tftp 2000000 <image_image>
		* NOTE: You can find compiled U-Boot images in the U-Boot release under the images folder.
		       Select the appropriate image based on your configuration.
		       Example: bubt u-boot-axp-XXXX-nand.bin (XXXX is u-boot version)
	5. nand erase 0 100000
	6. nand write 2000000 0 xxxxx (when xxxxx is if u-boot size - is printed by tftp)
	7. Set the following S@R value to switch to boot from NAND
		7.1. SatR write bootsrc 1
		7.2. SatR write bootwidth 0
	8. reset
		* NOTE: all environment variables will be reseted!

	Burning new U-Boot using NOR commands (For boards support NOR):
	---------------------------------------
	1. run the following commands:
	2. setenv serverip <IP address of your TFTP server>
	3. setenv ipaddr <IP address for the board>
	4. tftp 2000000 <image_image>
		* NOTE: You can find compiled U-Boot images in the U-Boot release under the images folder.
		       Select the appropriate image based on your configuration.
		       Example: bubt u-boot-axp-XXXX-nor.bin (XXXX is u-boot version)
	5. protect off all
	6. erase bank 1
	7. cp.b 2000000 fd000000 xxxxx (when xxxxx is if u-boot size - is printed by tftp)
	8. Set the following S@R value to switch to boot from NAND
		7.1. SatR write bootsrc 0
		7.2. SatR write bootwidth 1
	9. reset

2. How to use UBIFS
===================
	1. UBIFS is supported in u-boot.
	2. To create a UBI partition and volume and creating UBIFS please refer to the UBI documentation.
	3. after setting the mtdparts & mtdids variables as needed. (SEE paragraph 5), you can scan the mtd
	   partition for the UBI device.
	4. use the command: ubi part, to instruct u-boot on which mtd partition the UBI is on.
		>>ubi part <partition_name> <offset>
			<partition_name> - the mtd partition name, for example: rootfs.
			<offset>         - the offset to the UBI VID header, is should be the sector size of the NAND.
			                   for example: when using GANG mode the value will be 4096, when not the value is 2048.
	5. for example:
		>> ubi part rootfs 4096
		UBI: mtd1 is detached from ubi0
		Creating 1 MTD partitions on "nand0":
		0x000000400000-0x000200000000 : "mtd=1"
		UBI: attaching mtd1 to ubi0
		UBI: physical eraseblock size:   524288 bytes (512 KiB)
		UBI: logical eraseblock size:    516096 bytes
		UBI: smallest flash I/O unit:    4096
		UBI: VID header offset:          4096 (aligned 4096)
		UBI: data offset:                8192
		UBI: attached mtd1 to ubi0
		UBI: MTD device name:            "mtd=1"
		UBI: MTD device size:            8188 MiB
		UBI: number of good PEBs:        16366
		UBI: number of bad PEBs:         10
		UBI: max. allowed volumes:       16
		UBI: wear-leveling threshold:    4096
		UBI: number of internal volumes: 1
		UBI: number of user volumes:     1
		UBI: available PEBs:             0
		UBI: total number of reserved PEBs: 16366
		UBI: number of PEBs reserved for bad PEB handling: 163
		UBI: max/mean erase counter: 7/1
	6. after u-boot scan finishes with no errors, you can now use the UBIFS on the UBI device.
	7. to mount a UBIFS use the command:
		>>ubifsmount <volume_name>
		<volume_name> - the name that the volume created with.
	8. for example:
		>> ubifsmount lzo
		UBIFS: mounted UBI device 0, volume 0, name "lzo"
		UBIFS: mounted read-only
		UBIFS: file system size:   8355078144 bytes (8159256 KiB, 7968 MiB, 16189 LEBs)
		UBIFS: journal size:       10452992 bytes (10208 KiB, 9 MiB, 21 LEBs)
		UBIFS: media format:       w4/r0 (latest is w4/r0)
		UBIFS: default compressor: zlib
		UBIFS: reserved for root:  0 bytes (0 KiB)
	9. now you can ls and load the filesystem on the UBIFS with the commands:
		ubifsls [directory] - list files
			[directory] - optional, files in a 'directory' (default '/' or any other directory, for ex: '/boot')

		ubifsload <addr> <filename> [bytes] - load file 'filename' to address 'addr'
			<addr>     - address to load the file to.
			<filename> - the file name to load. for example: /boot/uImage.
			[bytes]    - optional, bytes to load, when left blank the whole file will be loaded.

3. How to use MMC 
=================
	1. in order to use MMC use the following commands:
	2. to scan for MMC use:
		>> mmc rescan
	3. to see MMC detected use:
		>> mmcinfo <port>
		<port> - the port the MMC is connected to.
		         on 510-BP-B board:
		           U17 is port 0.
		           U28 is port 1.
	4. for example:
		>> mmcinfo 0
		Device: MV_SDHCI
		Manufacturer ID: 1
		OEM: 5041
		Name: S032B
		Tran Speed: 25000000
		Rd Block Len: 512
		SD version 1.0
		High Capacity: No
		Capacity: 31129600
		Bus Width: 1-bit
	5. to list the files on a MMC use:
		fatls/ext2ls <interface> <dev[:part]> [directory] - list files from 'dev' on 'interface' in a 'directory'
			<interface> = mmc
			<dev> = 0
			[part] - the partition number that used.
			[directory] - the directory to list (default '/' or any other directory, for ex: '/boot')
	6. to load files from MMC to memory use:
		ext2ls/fatload <interface> <dev[:part]>  <addr> <filename> [bytes] - load binary file 'filename' from 'dev' on 'interface'
			<interface> = mmc
			<dev> = 0
			[part] - the partition number that used.
			<addr> - target address in memory.
			<filename> - which file to load.
			[bytes] - size to load or file size when left blank.
	* Note: use fat or ext2 commands according to the filesystem created on the USB.

4. How to use USB storage device
================================
	1. to descover the USB devices connected use the command: usb start.
		>> usb start
		(Re)start USB...
		USB:   Register 10011 NbrPorts 1
		USB EHCI 1.00
		scanning bus for devices... 2 USB Device(s) found
		Waiting for storage device(s) to settle before scanning...
			scanning bus for storage devices... 1 Storage Device(s) found
	2. to list the files on a USB use:
		fatls/ext2ls <interface> <dev[:part]> [directory] - list files from 'dev' on 'interface' in a 'directory'
			<interface> = usb
			<dev> = 0
			[part] - the partition number that used.
			[directory] - the directory to list (default '/' or any other directory, for ex: '/boot')
	3. to load files from USB to memory use:
		ext2ls/fatload <interface> <dev[:part]>  <addr> <filename> [bytes] - load binary file 'filename' from 'dev' on 'interface'
			<interface> = usb
			<dev> = 0
			[part] - the partition number that used.
			<addr> - target address in memory.
			<filename> - which file to load.
			[bytes] - size to load or file size when left blank.
	* Note: 
		- use fat or ext2 commands according to the filesystem created on the USB.
		- On board supported more than 1 USB device use  usbActive environment parameter
		  for using other USB storage device.

5. BIOS and SatR commands
=========================
	BIOS: (relevant for Armada XP)
	1. BIOS command is used to switch between the varios supported modes.
	2. to see all the supported modes please usethe command: bios list.
	3. use the command: bios write <mode> to switch between the different modes.
	4. after setting the relevant mode, reset the board.
	SatR:
	1. SatR command is used to toggle the different Sample At Reset avaliable modes.
	2. Use the commnad: SatR to see all the possibilities:
		Example from Armada XP
		$ SatR
			Usage:
		SatR list cpufreq       - prints the S@R modes list
		SatR list fabfreq       - prints the S@R modes list
		SatR list l2size        - prints the S@R modes list
		SatR list bootsrc       - prints the S@R modes list
		SatR list bootwidth     - prints the S@R modes list
		SatR list cpu0core      - prints the S@R modes list
		SatR list cpusnum       - prints the S@R modes list
		SatR list freq          - prints the S@R modes list
		SatR read cpufreq       - read and print the CPU frequency S@R value
		SatR read fabfreq       - read and print the Fabric frequency S@R value
		SatR read l2size        - read and print the L2 cache size S@R value
		SatR read bootsrc       - read and print the Boot source S@R value
		SatR read bootwidthc    - read and print the Boot device width S@R value
		SatR read cpu0core      - read and print the CPU0 core mode S@R value
		SatR read cpusnum       - read and print the number of CPU cores S@R value
		SatR read freq          - read and print the mode of cpu/ddr freq S@R value
		SatR write cpufreq <val>        - write the S@R with CPU frequency value
		SatR write fabfreq <val>        - write the S@R with Fabric frequency value
		SatR write l2size <val> - write the S@R with L2 cache size value
		SatR write bootsrc <val>        - write the S@R with Boot source value
		SatR write bootwidth <val>      - write the S@R with Boot device width value
		SatR write cpu0core <val>       - write the S@R with CPU0 core mode value
		SatR write cpusnum <val>        - write the S@R with the number of CPU cores
		SatR write freq <val>           - write the S@R with the cpu/ddr freq mode
	3. use SatR list <mode> to get the list of possible values for each <mode>.
	4. use SatR write <mode> <val> to set the needed value, reset is requaired.
	5. use SatR read <mode> to get the current value.
	6. In order to change frequancies please use the following command:
		SatR write freq <val>
		where <val> is one of the option in SatR list freq 
		below is the example for Armada XP:
	val| Frequency Configuration   | CPU Freq| Fabric Freq| 
	 00|  800 / 400 / 200 / 400 Mhz|     0x0a|        0x05|
	 01| 1066 / 533 / 266 / 533 Mhz|     0x01|        0x05|
	 02| 1200 / 600 / 300 / 600 Mhz|     0x02|        0x05|
	 03| 1200 / 600 / 200 / 400 Mhz|     0x02|        0x09|
	 04| 1333 / 667 / 333 / 667 Mhz|     0x03|        0x05|
	 05| 1500 / 750 / 375 / 750 Mhz|     0x04|        0x05|
	 06| 1500 / 750 / 250 / 500 Mhz|     0x04|        0x09|
	 07| 1600 / 800 / 266 / 533 Mhz|     0x0b|        0x09|
	 08| 1600 / 800 / 400 / 800 Mhz|     0x0b|        0x05|
	 09| 1600 / 640 / 320 / 640 Mhz|     0x0b|        0x0a|


	7. reset the system for the changes to take effect.

	*Note: These command are relevant only for Marvell DB boards, 
	       and that there are jumpers settings that configure the SatR back to default settings. 
	       More details in the specific Board User Manual


6. Boot options
=================

    How to boot from NOR (For boards support NOR)
    =============================================
	1. Set the following S@R value to switch to boot from NOR
		1.1. SatR write bootsrc 0
		1.2. SatR write bootwidth 1
	3. Burn u-boot on nor flash (see chapter 4) or burning new U-Boot using Lauterbach debugger

    How to boot from NAND (For boards support NAND)
    ===============================================
	1. Set the following S@R value to switch to boot from NAND
		2.1. SatR write bootsrc 1
		2.2. SatR write bootwidth 0
	3. Burn u-boot on nand (see chapter 4) or burning new U-Boot using Lauterbach debugger

    How to boot from SPI (For boards support SPI)
    =============================================
	1. Set the following S@R value to switch to boot from NAND
		2.1. SatR write bootsrc 3
		2.2. SatR write bootwidth 1
	2. Burn u-boot using the method discribed above: Burning new U-Boot using Lauterbach debugger

    How to boot from UART
    ====================
	1. Set the following S@R value to switch to boot from UART
		2.1. SatR write bootsrc 2
    2. reset the board.
    3. Send New U-boot using XMODEM.

7.  Stage Boot Howto
    ================
	This command used to run over different devices (installed system, PXE server, TFTP server), 
	and try to run script or boot image from the device, in different orders.

	UBOOT version 2012_Q4.0 (2012.11) or later.

	Use >>help stage_boot to see all the possibilities Usage:

	stage_boot usage:
		stage_boot hd_img - load and boot image from installed system.
		stage_boot hd_scr - load and run script from installed system.
		stage_boot pxe - boot from network (PXE-boot).
		stage_boot net_img - load and boot image from tftp server.
		stage_boot net_scr - load and run script from tftp server.

Environment Setup: 
	Environment Name	default value			describtion  							example  
	kernel_addr_r  		2080000  			kernel load address  						2080000  
	script_addr_r  		3000000  			script load addres  						3000000  
	device_partition	0:1  				search image/script in ide partitions, the command run over 
								the partitions and search the necessary file  			0:1 0:2 1:1  
	boot_order  		hd_scr hd_img			the order search of the command  				pxe ide_img hd_scr    
				pxe net_img     		
				net_scr  set    		
	script_name 		boot.scr  			set the script name to load and run  				script_name.scr  
	image_name  		uImage  			set the kernel image name to load  				uImage-v7axp-smp  
	ide_path  		/  				set the path to the script/image in ide device  		/home/filename/  
	pxe_files_load  	:default.arm-armadaxp-db:	necessary if you use PXE and you want to load spicific 		:default.arm-armadaxp-db     
				default.arm-armadaxp:		files from the PXE server  
				default.arm  	
	initrd_name  		uInitrd  			ram disk image name  						uInitrd_name  
	bootargs_dflt  		$console $mtdparts 		the default environment variables that sould pass to the kernel $console $mtdparts 
				$bootargs_root                                                                                  $bootargs_root    
				nfsroot=$serverip:$rootpath 
				ip=$ipaddr:$serverip$bootargs_end 
				$mvNetConfig video=dovefb:lcd0:
				$lcd0_params clc d.lcd0_enable=$lcd0_enable 
				clcd.lcd_panel=$lcd_panel  
	bootcmd_auto  		stage_boot $boot_order  contain the command to stage boot    
	bootcmd_lgcy  		tftpboot 0x2000000 $image_name; contain the tftp command boot       
				setenv bootargs $bootargs_dflt; 
				bootm 0x2000000;  
	serverip  		10.4.50.5  			TFTP server ip  						10.4.50.10  
	pxefile_addr_r  	3100000  			pxe boot file load addres    



	How it Works?
	The Stage boot command goes by the order in the environment variable boot_order (e.g. : boot_order = hd_scr hd_img pxe net_img net_scr):
	
	Note: The command start with reset and mount the ide device.
	
	1. hd_scr: Loading and run script from IDE device: 
	   a. For every partion by the environment (device_partition) the command will follow this steps: 
		1. Searching script (script_name) file in the directory (ide_path) 
		2. If the script file exsit in the ide device, it will be loaded to (script_addr_r), if not the command will skip to the next option. 
		3. Runs the script. 
	2. hd_img: Loading and boot image form IDE device  
	   a. For every partion by the environment (device_partition) the command will follow this steps: 
		1. searching kernel image (image_name) file in the directory (ide_path) 
		2. if the image exsit in the ide device, 
			a. The image be loaded to (kernel_add_r) and then the command search for ram disk image (initrd_name) file in the same directory. 
			b. if the initrd exsit, it will be loaded to (ramdisk_addr_r) and boot with the two images. 
			c. if the (initrd_name) don't exsit the command load only the kernel image.  
			d. If the kernel image don't exsit in the ide device the command will pass to the next device. 
	3. pxe: Boot from PXE server: 
	   a. Searching for DHCP server. 
	      1. Get IP from the dhcp server. 
	      2. Get PXE server IP, if not founded the (sereverip) get default value. 
	   b. Searching in the PXE server for boot file: 
	      1. Searching files with this order: physical address, IP address, files in environment (pxe_files_load) and the latest is default. 
	      2. If one of this files is founded, it will be loaded to (pxefile_addr_r) and stop searching for the other files 
	  c. PXE boot file that loaded in (pxefile_addr_r) 
	4. net_img: Loading and boot image from TFTP server: 
	  a. seaching kernel image (image_name) in tftp server. 
	  b. if the image exsit, will be loaded to (kernel_addr_r) 
	  c. boot the image. 
	5. net_scr: loading and runs script from TFTP server: 
	  a. seaching script (script_name) in tftp server. 
	  b. if the script exsit, will be loaded to (kernel_addr_r) 
	  c. run the script. 

	Make sure the u-boot is compiled with CONFIG_CMD_STAGE_BOOT, if not please add #define CONFIG_CMD_STAGE_BOOT into file include/configs/armada_xp.h then compile the u-boot. 
	to boot by the stage boot command use: 
	       $ setenv bootcmd $bootcmd_auto 
	       $ saveenv 
	to boot by the old way use: 
	       $ setenv bootcmd $bootcmd_lgcy 
	       $ saveenv 


8. Disclaimer
==============
This document provides preliminary information about the products described, and such information should not be used for
purpose of final design. Visit the Marvell  web site at www.marvell.com for the latest information on Marvell products.

No part of this document may be reproduced or transmitted in any form or by any means, electronic or mechanical, including
photocopying and recording, for any purpose, without the express written permission of Marvell. Marvell retains the right
to make changes to this document at any time, without notice. Marvell makes no warranty of any kind, expressed or implied,
with regard to any information contained in this document, including, but not limited to, the implied warranties of
merchantability or fitness for any particular purpose. Further, Marvell does not warrant the accuracy or completeness of
the information, text, graphics, or other items contained within this document. Marvell makes no commitment either to
update or to keep current the information contained in this document. Marvell products are not designed for use in
life-support equipment or applications that would cause a life-threatening situation if any such products failed.
Do not use Marvell products in these types of equipment or applications. The user should contact Marvell to obtain the
latest specifications before finalizing a product design. Marvell assumes no responsibility, either for use of these
products or for any infringements of patents and trademarks, or other rights of third parties resulting from its use.
No license is granted under any patents, patent rights, or trademarks of Marvell. These products may include one or more
optional functions. The user has the choice of implementing any particular optional function. Should the user choose to
implement any of these optional functions, it is possible that the use could be subject to third party intellectual
property rights. Marvell recommends that the user investigate whether third party intellectual property rights are
relevant to the intended use of these products and obtain licenses as appropriate under relevant intellectual property
rights.
Marvell comprises Marvell Technology Group Ltd. (MTGL) and its subsidiaries, Marvell International Ltd. (MIL),
Marvell Semiconductor, Inc. (MSI), Marvell Asia Pte Ltd. (MAPL), Marvell Japan K.K. (MJKK),
Marvell Semiconductor Israel Ltd. (MSIL), SysKonnect GmbH, and Radlan Computer Communications, Ltd.
Export Controls.  With respect to any of Marvell s Information, the user or recipient, in the absence of appropriate U.S.
government authorization, agrees: 1) not to re-export or release any such information consisting of technology, software
or source code controlled for national security reasons by the U.S. Export Control Regulations ("EAR"), to a national of
EAR Country Groups D:1 or E:2; 2) not to export the direct product of such technology or such software, to EAR Country
Groups D:1 or E:2, if such technology or software and direct products thereof are controlled for national security reasons
by the EAR; and, 3) in the case of technology controlled for national security reasons under the EAR where the direct
product of the technology is a complete plant or component of a plant, not to export to EAR Country Groups D:1 or E:2 the
direct product of the plant or major component thereof, if such direct product is controlled for national security reasons
by the EAR, or is subject to controls under the U.S. Munitions List  ("USML"). At all times hereunder, the recipient of
any such information agrees that they shall be deemed to have manually signed this document in connection with their
receipt of any such information.
Copyright   2004.  Marvell.  All rights reserved.  Marvell, the Marvell logo, Moving Forward Faster, Alaska, and GalNet
are registered trademarks of Marvell.  Discovery, Fastwriter, GalTis, Horizon, Libertas, Link Street, NetGX,
PHY Advantage, Prestera, Raising The Technology Bar, UniMAC, Virtual Cable Tester, and Yukon are trademarks of Marvell.
All other trademarks are the property of their respective owners.
