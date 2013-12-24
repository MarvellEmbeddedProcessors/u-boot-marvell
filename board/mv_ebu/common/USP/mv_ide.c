/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */
/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.

*******************************************************************************/

/*
 * IDE support
 */
#include <common.h>
#include <config.h>
#include <watchdog.h>
#include <command.h>
#include <image.h>
#include <asm/byteorder.h>
#include <ide.h>
#include <ata.h>
#include <pci.h>
#include "mvCtrlEnvLib.h"

#if defined(CONFIG_CMD_IDE)

#include "sata/CoreDriver/mvOsS.h"
#include "sata/CoreDriver/mvSata.h"
#include "sata/CoreDriver/mvStorageDev.h"
#include "mvSysSataApi.h"

#ifdef MV_LOGGER
char *szModules[] = {"Core Driver",
"BIOS IAL"
};
#endif

#undef	IDE_DEBUG

#ifdef	IDE_DEBUG
#define	DP(fmt,args...)	printf (fmt ,##args)
#else
#define DP(fmt,args...)
#endif

#define SHOW_BOOT_PROGRESS(arg)

#ifndef MRVL_SATA_BUFF_BOUNDARY
#define MRVL_SATA_BUFF_BOUNDARY (1 << 24)
#endif /* MRVL_SATA_BUFF_BOUNDARY */

#define MRVL_SATA_BOUNDARY_MASK (MRVL_SATA_BUFF_BOUNDARY - 1)

#if defined(__BE)
void swapATABuffer(unsigned short *buffer, ulong count);
#endif
/* ------------------------------------------------------------------------- */

/* Current I/O Device	*/
static int curr_device = -1;
static int use_dma = 1;
static int use_sata_delay_reset = 0;

#ifdef	ATA_CURR_BASE
#undef  ATA_CURR_BASE
#endif
#define	ATA_CURR_BASE(dev)	(CFG_ATA_BASE_ADDR+ide_bus_offset[IDE_BUS(dev)])

#define MV_PM 0xF

typedef struct mvSataPMDeviceInfo
{
	MV_U16      vendorId;
	MV_U16      deviceId;
	MV_U8       productRevision;
	MV_U8       PMSpecRevision:4;
	MV_U8       numberOfPorts:4;
} MV_SATA_PM_DEVICE_INFO;

typedef struct _MV_CHANNEL_INFO
{
	MV_SATA_DEVICE_TYPE	deviceType;
	MV_U8			numberOfPorts;
	MV_U16			connected;
}MV_CHANNEL_INFO;
typedef struct _HW_ADAPTER_DESCRIPTION
{
    MV_BOOLEAN          valid;
    int		 	devno;
    MV_SATA_ADAPTER     mvSataAdapter;  /* CoreDriver Adapter data structure*/
    MV_SATA_CHANNEL	mvSataChannels[MV_SATA_CHANNELS_NUM];
    MV_CHANNEL_INFO	channelInfo[MV_SATA_PM_MAX_PORTS];
} HW_ADAPTER_DESCRIPTION, *PHW_ADAPTER_DESCRIPTION;

typedef struct
{
	MV_U8 adapterId;
	MV_U8 channelIndex;
	MV_U8 port;
	MV_BOOLEAN dmaSupported;
} MV_SATA_DISK_PRIV;
/* Data structure describing mvSata adapter and channels */
/* The data structure describes 4 adapters */
#define MAX_NUM_OF_ADAPTERS 4
HW_ADAPTER_DESCRIPTION sataAdapters[CONFIG_SYS_IDE_MAXBUS];
static  block_dev_desc_t ide_dev_desc[CONFIG_SYS_IDE_MAXDEVICE];
static  unsigned int ide_initiated=0,ide_detected=0;

MV_SATA_EDMA_PRD_ENTRY prd_table[2]  __attribute__ ((aligned(32)));
unsigned char request_q[MV_EDMA_REQUEST_ENTRY_SIZE] __attribute__ ((aligned(1024)));
unsigned char response_q[MV_EDMA_RESPONSE_ENTRY_SIZE] __attribute__ ((aligned(256)));

/* ------------------------------------------------------------------------- */

static MV_BOOLEAN StartPM(HW_ADAPTER_DESCRIPTION *sataAdapter, MV_U8 channelIndex);

static MV_BOOLEAN initDisk(MV_SATA_ADAPTER *pSataAdapter, MV_U8 channelIndex,
			   MV_U8 port, block_dev_desc_t *dev_desc);

static void ident_cpy (unsigned char *dest, unsigned char *src, unsigned int len);

static ulong pio_read_write (MV_SATA_ADAPTER *pSataAdapter,
			     unsigned int channelIndex,
			     MV_U8 port,
			     lbaint_t blknr, lbaint_t blkcnt, ulong *buffer, int read);
static
ulong dma_read_write (MV_SATA_ADAPTER *pSataAdapter,
		      unsigned int channelIndex,
		      MV_U8 port,
		      lbaint_t blknr, lbaint_t blkcnt, ulong *buffer, int read);
static
ulong dma_read_write_cmd (MV_SATA_ADAPTER *pSataAdapter,
			  unsigned int channelIndex,
			  MV_U8 port,
			  lbaint_t blknr, lbaint_t blkcnt, ulong *buffer, int read);
/* ------------------------------------------------------------------------- */

#ifdef CONFIG_PCI

static struct pci_device_id supported[] = {
        {MV_SATA_VENDOR_ID, MV_SATA_DEVICE_ID_5080},
        {MV_SATA_VENDOR_ID, MV_SATA_DEVICE_ID_5081},
        {MV_SATA_VENDOR_ID, MV_SATA_DEVICE_ID_5040},
        {MV_SATA_VENDOR_ID, MV_SATA_DEVICE_ID_5041},
        {MV_SATA_VENDOR_ID, MV_SATA_DEVICE_ID_6041},
        {MV_SATA_VENDOR_ID, MV_SATA_DEVICE_ID_6081},
        {MV_SATA_VENDOR_ID, MV_SATA_DEVICE_ID_6042},
        {MV_SATA_VENDOR_ID, MV_SATA_DEVICE_ID_7042},
};

#endif /* CONFIG_PCI */


int do_ide (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int rcode = 0;

	/* Check if we have already called reset or not */
	if (ide_initiated == 0)
	{
		/* If this is not a call to reset !!!! */
		if (!((argc==2)&&(strncmp(argv[1],"res",3) == 0)))
		{
			puts ("\nWarning: Please run 'ide reset' before running other ide commands \n\n");
			return 1;
		}
	}
	else /* We have already called reset */
	{
		if (ide_detected == 0)
		{
			puts ("\nno IDE devices available\n");
			return 1;
		}
	}

    switch (argc) {
    case 0:
    case 1:
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    case 2:
	if (strncmp(argv[1],"res",3) == 0) {
		puts ("\nReset IDE"
#ifdef CONFIG_IDE_8xx_DIRECT
			" on PCMCIA " PCMCIA_SLOT_MSG
#endif
			": ");

		ide_init ();
		return 0;
	} else if (strncmp(argv[1],"inf",3) == 0) {
		int i;

		putc ('\n');

		for (i=0; i<CONFIG_SYS_IDE_MAXDEVICE; ++i) {
			if (ide_dev_desc[i].type==DEV_TYPE_UNKNOWN)
				continue; /* list only known devices */
			printf ("IDE device %x: ", i);
			dev_print(&ide_dev_desc[i]);
		}
		return 0;

	} else if (strncmp(argv[1],"dev",3) == 0) {
		if ((curr_device < 0) || (curr_device >= CONFIG_SYS_IDE_MAXDEVICE)) {
			puts ("\nno IDE devices available\n");
			return 1;
		}
		printf ("\nIDE device %x: ", curr_device);
		dev_print(&ide_dev_desc[curr_device]);
		return 0;
	} else if (strncmp(argv[1],"part",4) == 0) {
		int dev, ok;

		for (ok=0, dev=0; dev<CONFIG_SYS_IDE_MAXDEVICE; ++dev) {
			if (ide_dev_desc[dev].part_type!=PART_TYPE_UNKNOWN) {
				++ok;
				if (dev)
					putc ('\n');
				print_part(&ide_dev_desc[dev]);
			}
		}
		if (!ok) {
			puts ("\nno IDE devices available\n");
			rcode ++;
		}
		return rcode;
	}
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    case 3:
	if (strncmp(argv[1],"dev",3) == 0) {
		int dev = (int)simple_strtoul(argv[2], NULL, 10);

		printf ("\nIDE device %x: ", dev);
		if (dev >= CONFIG_SYS_IDE_MAXDEVICE) {
			puts ("unknown device\n");
			return 1;
		}
		dev_print(&ide_dev_desc[dev]);
		/*ide_print (dev);*/

		if (ide_dev_desc[dev].type == DEV_TYPE_UNKNOWN) {
			return 1;
		}

		curr_device = dev;

		puts ("... is now current device\n");

		return 0;
	} else if (strncmp(argv[1],"part",4) == 0) {
		int dev = (int)simple_strtoul(argv[2], NULL, 10);

		if (ide_dev_desc[dev].part_type!=PART_TYPE_UNKNOWN) {
				print_part(&ide_dev_desc[dev]);
		} else {
			printf ("\nIDE device %x not available\n", dev);
			rcode = 1;
		}
		return rcode;
	}

	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    default:
	/* at least 4 args */

	if (strcmp(argv[1],"read") == 0) {
		ulong addr = simple_strtoul(argv[2], NULL, 16);
		ulong cnt  = simple_strtoul(argv[4], NULL, 16);
		ulong n;

		if (curr_device == -1)
		{
			printf ("\n\nIDE device not available\n");
			return 1;
		}
#ifdef CONFIG_SYS_64BIT_LBA
		lbaint_t blk  = simple_strtoull(argv[3], NULL, 16);

		printf ("\nIDE read: device %x block # %qd, count %ld ... ",
			curr_device, blk, cnt);
#else
		lbaint_t blk  = simple_strtoul(argv[3], NULL, 16);

		printf ("\nIDE read: device %x block # %ld, count %ld ... ",
			curr_device, blk, cnt);
#endif

		n = ide_dev_desc[curr_device].block_read (curr_device,
							  blk, cnt,
							  (ulong *)addr);
		/* flush cache after read */
		flush_cache (addr, cnt*ide_dev_desc[curr_device].blksz);

		printf ("%ld blocks read: %s\n",
			n, (n==cnt) ? "OK" : "ERROR");
		if (n==cnt) {
			return 0;
		} else {
			return 1;
		}
	} else if (strcmp(argv[1],"write") == 0) {
		ulong addr = simple_strtoul(argv[2], NULL, 16);
		ulong cnt  = simple_strtoul(argv[4], NULL, 16);
		ulong n;

		if (curr_device == -1)
		{
			printf ("\n\nIDE device not available\n");
			return 1;
		}
#ifdef CONFIG_SYS_64BIT_LBA
		lbaint_t blk  = simple_strtoull(argv[3], NULL, 16);

		printf ("\nIDE write: device %x block # %qd, count %ld ... ",
			curr_device, blk, cnt);
#else
		lbaint_t blk  = simple_strtoul(argv[3], NULL, 16);

		printf ("\nIDE write: device %x block # %ld, count %ld ... ",
			curr_device, blk, cnt);
#endif

		n = ide_write (curr_device, blk, cnt, (ulong *)addr);

		printf ("%ld blocks written: %s\n",
			n, (n==cnt) ? "OK" : "ERROR");
		if (n==cnt) {
			return 0;
		} else {
			return 1;
		}
	} else {
		printf ("Usage:\n%s\n", cmdtp->usage);
		rcode = 1;
	}

	return rcode;
    }
}

int do_diskboot (cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	char *boot_device = NULL;
	char *ep;
	int dev, part = 0;
	ulong cnt;
	ulong addr;
	disk_partition_t info;
	image_header_t *hdr;
	int rcode = 0;

	switch (argc) {
	case 1:
		addr = CONFIG_SYS_LOAD_ADDR;
		boot_device = getenv ("bootdevice");
		break;
	case 2:
		addr = simple_strtoul(argv[1], NULL, 16);
		boot_device = getenv ("bootdevice");
		break;
	case 3:
		addr = simple_strtoul(argv[1], NULL, 16);
		boot_device = argv[2];
		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		SHOW_BOOT_PROGRESS (-1);
		return 1;
	}

	if (!boot_device) {
		puts ("\n** No boot device **\n");
		SHOW_BOOT_PROGRESS (-1);
		return 1;
	}

	dev = simple_strtoul(boot_device, &ep, 16);

	if (ide_dev_desc[dev].type==DEV_TYPE_UNKNOWN) {
		printf ("\n** Device %x not available\n", dev);
		SHOW_BOOT_PROGRESS (-1);
		return 1;
	}

	if (*ep) {
		if (*ep != ':') {
			puts ("\n** Invalid boot device, use `dev[:part]' **\n");
			SHOW_BOOT_PROGRESS (-1);
			return 1;
		}
		part = simple_strtoul(++ep, NULL, 16);
	}
	if (get_partition_info (&ide_dev_desc[dev], part, &info)) {
		SHOW_BOOT_PROGRESS (-1);
		return 1;
	}
	if ((strncmp((const char *)info.type, BOOT_PART_TYPE, sizeof(info.type)) != 0) &&
	    (strncmp((const char *)info.type, BOOT_PART_COMP, sizeof(info.type)) != 0)) {
		printf ("\n** Invalid partition type \"%.32s\""
			" (expect \"" BOOT_PART_TYPE "\")\n",
			info.type);
		SHOW_BOOT_PROGRESS (-1);
		return 1;
	}

	printf ("\nLoading from IDE device %x, partition %d: "
		"Name: %.32s  Type: %.32s\n",
		dev, part, info.name, info.type);

	DP ("First Block: %ld,  # of blocks: %ld, Block Size: %ld\n",
		info.start, info.size, info.blksz);

	if (ide_dev_desc[dev].block_read (dev, info.start, 1, (ulong *)addr) != 1) {
		printf ("** Read error on %x:%d\n", dev, part);
		SHOW_BOOT_PROGRESS (-1);
		return 1;
	}

	hdr = (image_header_t *)addr;

	if (ntohl(hdr->ih_magic) == IH_MAGIC) {

		image_print_contents (hdr);

		cnt = (ntohl(hdr->ih_size) + sizeof(image_header_t));
		cnt += info.blksz - 1;
		cnt /= info.blksz;
		cnt -= 1;
	} else {
		printf("\n** Bad Magic Number **\n");
		SHOW_BOOT_PROGRESS (-1);
		return 1;
	}

	if (ide_dev_desc[dev].block_read (dev, info.start+1, cnt,
		      (ulong *)(addr+info.blksz)) != cnt) {
		printf ("** Read error on %x:%d\n", dev, part);
		SHOW_BOOT_PROGRESS (-1);
		return 1;
	}


	/* Loading ok, update default load address */

	load_addr = addr;

	/* Check if we should attempt an auto-start */
	if (((ep = getenv("autostart")) != NULL) && (strcmp(ep,"yes") == 0)) {
		char *local_args[2];
		extern int do_bootm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

		local_args[0] = argv[0];
		local_args[1] = NULL;

		printf ("Automatic boot of image at addr 0x%08lX ...\n", addr);

		do_bootm (cmdtp, 0, 1, local_args);
		rcode = 1;
	}
	return rcode;
}

/* ------------------------------------------------------------------------- */
MV_SATA_DEVICE_TYPE mvGetSataDeviceType(
					MV_STORAGE_DEVICE_REGISTERS *mvStorageDevRegisters)
{
	if (((mvStorageDevRegisters->sectorCountRegister & 0xff) != 1) ||
	    ((mvStorageDevRegisters->lbaLowRegister & 0xff) != 1))
		{
			return MV_SATA_DEVICE_TYPE_UNKNOWN;
		}
	if (((mvStorageDevRegisters->lbaMidRegister & 0xff) == 0) &&
	    ((mvStorageDevRegisters->lbaHighRegister & 0xff) == 0))
		{
			return MV_SATA_DEVICE_TYPE_ATA_DISK;
		}
	if ((((mvStorageDevRegisters->lbaMidRegister & 0xff) == 0x14) &&
	     ((mvStorageDevRegisters->lbaHighRegister & 0xff) == 0xEB))/* ||
         (((mvStorageDevRegisters->lbaMidRegister & 0xff) == 0x69) &&
         ((mvStorageDevRegisters->lbaHighRegister & 0xff) == 0x96))*/)
		{
			return MV_SATA_DEVICE_TYPE_ATAPI_DEVICE;
		}
	if (((mvStorageDevRegisters->lbaMidRegister & 0xff) == 0x69) &&
	    ((mvStorageDevRegisters->lbaHighRegister & 0xff) == 0x96))
		{
			return MV_SATA_DEVICE_TYPE_PM;
		}
	return MV_SATA_DEVICE_TYPE_UNKNOWN;
}

MV_BOOLEAN  mvGetPMDeviceInfo(MV_SATA_ADAPTER   *pSataAdapter,
                              MV_U8 channelIndex,
                              MV_SATA_PM_DEVICE_INFO *pPMDeviceInfo)
{
	MV_U32  regVal;

	if (mvPMDevReadReg(pSataAdapter, channelIndex, MV_SATA_PM_CONTROL_PORT,
			   MV_SATA_GSCR_ID_REG_NUM, &regVal, NULL) == MV_FALSE)
		{
			printf("Error [%d %d]: Failed to read PM GSCR ID register\n",
			       pSataAdapter->adapterId, channelIndex);
			return MV_FALSE;
		}
	pPMDeviceInfo->vendorId = (MV_U16)(regVal & 0xffff);
	pPMDeviceInfo->deviceId = (MV_U16)((regVal & 0xffff0000) >> 16);

	if (mvPMDevReadReg(pSataAdapter, channelIndex, MV_SATA_PM_CONTROL_PORT,
			   MV_SATA_GSCR_REVISION_REG_NUM, &regVal, NULL)== MV_FALSE)
		{
			printf("Error:[%d %d]: Failed to read PM GSCR REVISION register\n", pSataAdapter->adapterId,
			       channelIndex);
			return MV_FALSE;
		}
	pPMDeviceInfo->PMSpecRevision = (MV_U8)(regVal & 0xff);
	pPMDeviceInfo->productRevision = (MV_U8)((regVal & 0xff00) >> 8);

	if (mvPMDevReadReg(pSataAdapter, channelIndex, MV_SATA_PM_CONTROL_PORT,
			   MV_SATA_GSCR_INFO_REG_NUM, &regVal, NULL)== MV_FALSE)
		{
			printf("Error: [%d %d]: Failed to read PM GSCR INFO\n", pSataAdapter->adapterId,
				 channelIndex);
			return MV_FALSE;
		}
	pPMDeviceInfo->numberOfPorts = (MV_U8)(regVal & 0xf);
	return MV_TRUE;
}

static MV_BOOLEAN
StartPM(HW_ADAPTER_DESCRIPTION *sataAdapter,
	MV_U8 channelIndex)
{
	MV_SATA_ADAPTER *pSataAdapter = &sataAdapter->mvSataAdapter;
	MV_U8               PMPort;
	MV_SATA_PM_DEVICE_INFO PMInfo;

	if (mvGetPMDeviceInfo(pSataAdapter, channelIndex, &PMInfo) == MV_FALSE)	{
		printf("Error: Failed to get PortMultiplier Info\n");
		return MV_FALSE;
	}
	printf("Port Multiplier found @ %d %d. Vendor: %04x ports: %d\n",
	       pSataAdapter->adapterId, channelIndex, PMInfo.vendorId,
	       PMInfo.numberOfPorts);

	for (PMPort = 0; PMPort < PMInfo.numberOfPorts; PMPort++){
		MV_U32  SStatus;
		/*
		 * Skip PMPort #0 - this should be already enabled due to legacy mode
		 * transition of PM
		 */
		if (PMPort > 0)	{
			if (mvPMDevEnableStaggeredSpinUp(pSataAdapter, channelIndex,
							 PMPort) == MV_FALSE){
				printf("Error:[%d %d %d]: EnableStaggeredSpinUp Failed\n",
				       pSataAdapter->adapterId, channelIndex, PMPort);
				if (mvStorageDevATASoftResetDevice(pSataAdapter, channelIndex,
								   MV_SATA_PM_CONTROL_PORT, NULL) == MV_FALSE)
					{
						printf("Error: [%d %d]: failed to Soft Reset PM control port\n"
						       , pSataAdapter->adapterId, channelIndex);
					}
				continue;
			}
		}
		if (mvPMDevReadReg(pSataAdapter, channelIndex, PMPort,
				   MV_SATA_PSCR_SSTATUS_REG_NUM, &SStatus, NULL) ==
		    MV_FALSE)
			{
				printf("Error:[%d %d %d]: mvPMDevReadReg Failed\n",
				       pSataAdapter->adapterId, channelIndex, PMPort);
				if (mvStorageDevATASoftResetDevice(pSataAdapter, channelIndex,
								   MV_SATA_PM_CONTROL_PORT, NULL) == MV_FALSE)
					{
						printf("Error: [%d %d]: failed to Soft Reset PM control port\n"
						       , pSataAdapter->adapterId, channelIndex);
					}
				continue;
			}
		DP("[%d %d %x]: S-Status: 0x%x\n", pSataAdapter->adapterId,
				   channelIndex, PMPort, SStatus);
		if ((SStatus & 0xf) == 3){
			if (mvPMDevWriteReg(pSataAdapter, channelIndex, PMPort,
					    MV_SATA_PSCR_SERROR_REG_NUM, 0xffffffff, NULL) ==
			    MV_FALSE)
				{
					printf("Error [%d %d %d]: PM Write SERROR Failed\n",
					       pSataAdapter->adapterId, channelIndex, PMPort);
					continue;
				}

			sataAdapter->channelInfo[channelIndex].connected |= 1 << PMPort;
		}
	}
	sataAdapter->channelInfo[channelIndex].numberOfPorts = PMInfo.numberOfPorts;
	return MV_TRUE;
}


static MV_BOOLEAN StartChannel(HW_ADAPTER_DESCRIPTION *sataAdapter,
			       MV_U8 channelIndex)
{
	MV_SATA_ADAPTER *pSataAdapter = &sataAdapter->mvSataAdapter;
	MV_STORAGE_DEVICE_REGISTERS ATARegs;
	MV_SATA_CHANNEL     *pSataChannel;
	MV_SATA_DEVICE_TYPE deviceType;

	pSataChannel = pSataAdapter->sataChannel[channelIndex];
	if (!pSataChannel) {
		printf ("Error in StartChannel - Channel data structure is null\n");
		return MV_FALSE;
	}

	if (use_sata_delay_reset > 0) {
		printf("Delay [%d] useconds\n", use_sata_delay_reset);
		udelay(use_sata_delay_reset);
	}

	if (mvStorageDevATASoftResetDevice(pSataAdapter, channelIndex, MV_PM, &ATARegs) == MV_FALSE) {
		printf("Error - Failed initializing(SRST) drive on channel %d\n", channelIndex);
		return MV_FALSE;
	}
	deviceType = mvGetSataDeviceType(&ATARegs);
	mvStorageDevSetDeviceType(pSataAdapter, channelIndex,deviceType);
	sataAdapter->channelInfo[channelIndex].deviceType = deviceType;
	sataAdapter->channelInfo[channelIndex].numberOfPorts = 0;
	sataAdapter->channelInfo[channelIndex].connected = 0;
	switch(deviceType) {
		case MV_SATA_DEVICE_TYPE_UNKNOWN:
			printf("Error - unknown device at channel %d\n", channelIndex);
			return MV_FALSE;
		case MV_SATA_DEVICE_TYPE_ATAPI_DEVICE:
			printf("Error - unknown device at channel %d\n", channelIndex);
			return MV_FALSE;
		case MV_SATA_DEVICE_TYPE_ATA_DISK:
			sataAdapter->channelInfo[channelIndex].numberOfPorts = 1;
			sataAdapter->channelInfo[channelIndex].connected = 1;
			return MV_TRUE;
		case MV_SATA_DEVICE_TYPE_PM:
			return StartPM(sataAdapter, channelIndex);
	}
	return MV_TRUE;
}

#if defined(CONFIG_MV_SCATTERED_SPINUP)
static MV_BOOLEAN SpinupGetEnv(MV_U8 *disks, MV_U8 *timeout)
{
	char *env = getenv("spinup_config");
	if (!env)
		return MV_FALSE;

	*disks = env[0] - '0';
	*timeout = env[2] - '0';

	if (env[1] != ',' || *disks == 0 || *disks > 8 || *timeout == 0 || *timeout > 6)
		return MV_FALSE;

	return MV_TRUE;
}

static MV_BOOLEAN DoStaggeredSpinUp(void)
{
	static MV_U32 modulo = 0;
	MV_U8 disks, timeout;

	/* If SpinupGetEnv() fail don't stagger spin-up */
	if (SpinupGetEnv(&disks, &timeout) == MV_FALSE)
		return MV_FALSE;

	if (modulo % disks == 0) {
		/* Delay disk spin-up */
		printf("\nWaiting for %d second(s) to spin-up %d disk(s)... ", timeout, disks);
		udelay(1000000 * timeout);
		printf("done\n\n");
	}

	modulo++;

	return MV_TRUE;
}
#endif /* CONFIG_MV_SCATTERED_SPINUP */

static MV_BOOLEAN initDisk(MV_SATA_ADAPTER *pSataAdapter, MV_U8 channelIndex,
			   MV_U8 port, block_dev_desc_t *dev_desc)
{
	MV_STORAGE_DEVICE_REGISTERS ATARegs;
	unsigned short identifyBuffer[(sizeof(unsigned long)/sizeof(unsigned short)) * ATA_SECTORWORDS];
	hd_driveid_t *iop = (hd_driveid_t *)identifyBuffer;
	MV_U16_PTR	iden = (MV_U16_PTR) identifyBuffer;
	MV_U8 PIOMode;
	MV_U8 UDMAMode;
	MV_SATA_DEVICE_TYPE deviceType = mvStorageDevGetDeviceType(pSataAdapter,channelIndex);
	MV_SATA_DISK_PRIV *priv = malloc(sizeof(MV_SATA_DISK_PRIV));

	if (deviceType == MV_SATA_DEVICE_TYPE_PM)
	{
		if (mvStorageDevATASoftResetDevice(pSataAdapter,
						   channelIndex, port, &ATARegs) == MV_FALSE)
		{
			printf("Error - SRST for port %d on channel %d failed\n", port, channelIndex);
			return MV_FALSE;
		}
		if(mvGetSataDeviceType(&ATARegs) != MV_SATA_DEVICE_TYPE_ATA_DISK)
		{
			//					printf("Error: device at %d %d %d not supported\n", pSataAdapter->adapterId, channelIndex, port);

			return MV_FALSE;
		}
	}

	/* identify device*/
	memset(&ATARegs, 0, sizeof(ATARegs));
	ATARegs.commandRegister = MV_ATA_COMMAND_IDENTIFY;
	if (mvStorageDevATAIdentifyDevice(pSataAdapter, channelIndex, port,
		(unsigned short *)identifyBuffer) == MV_FALSE) {
		printf("[%d %d %d]: failed to perform ATA Identify command\n", pSataAdapter->adapterId, channelIndex, port);
		return MV_FALSE;
	}
 
#if defined(CONFIG_MV_SCATTERED_SPINUP)

	/* Call routine that will delay the start of hard drives */
	if (DoStaggeredSpinUp() == MV_FALSE)
		printf("\nStaggered Spin-up feature disabled\n\n");

#endif /* CONFIG_MV_SCATTERED_SPINUP */

	/*
	 * Device requires SET FEATURES subcommand to spin-up after power-up
	 * and IDENTIFY DEVICE response is incomplete
	 */
	if (iden[IDEN_UNIQUE_CONFIG] == IDEN_UNIQUE_INCOMPLETE) {
		if (mvStorageDevATASetFeatures(pSataAdapter, channelIndex, port,
					       MV_ATA_SET_FEATURES_SPIN_UP, 0, 0, 0, 0) == MV_FALSE) {
			printf("[%d %d %d]: failed to perform ATA device spin-up command\n", pSataAdapter->adapterId, channelIndex, port);
			return MV_FALSE;
		}

		/*
		 * Wait 10ms after issue Set Features command -
		 * MV_ATA_SET_FEATURES_SPIN_UP in this case
		 */
		udelay(10000);

		/* Identify device once again */
		if (mvStorageDevATAIdentifyDevice(pSataAdapter, channelIndex, port,
						  (unsigned short *)identifyBuffer) == MV_FALSE) {
			printf("[%d %d %d]: failed to perform ATA Identify command\n", pSataAdapter->adapterId, channelIndex, port);
			return MV_FALSE;
		}
		/*
		 * Device requires SET FEATURES subcommand to spin-up after
		 * power-up and IDENTIFY DEVICE response is complete
		 */
	} else if (iden[IDEN_UNIQUE_CONFIG] == IDEN_UNIQUE_COMPLETE) {
		if (mvStorageDevATASetFeatures(pSataAdapter, channelIndex, port,
					       MV_ATA_SET_FEATURES_SPIN_UP, 0, 0, 0, 0) == MV_FALSE) {
			printf("[%d %d %d]: failed to perform ATA device spin-up command\n", pSataAdapter->adapterId, channelIndex, port);
			return MV_FALSE;
		}

		/*
		 * Wait 10ms after issue Set Features command -
		 * MV_ATA_SET_FEATURES_SPIN_UP in this case
		 */
		udelay(10000);
	}

	/* Check if read look ahead is supported. If so enable it */
	if (iden[IDEN_SUPPORTED_COMMANDS1] & MV_BIT6) {
		if (mvStorageDevATASetFeatures(pSataAdapter, channelIndex, port,
						MV_ATA_SET_FEATURES_ENABLE_RLA,0,0,0, 0) == MV_FALSE) {
			printf("[%d %d %d]: failed to perform Enable RLA command\n", pSataAdapter->adapterId, channelIndex, port);
			return MV_FALSE;
		}
	}

	/* Usual case. Word 53 indicates word 64 is valid */
	if (iden[IDEN_VALID] & MV_BIT1) {
		if (iden[IDEN_PIO_MODE_SPPORTED] & MV_BIT0) {
			PIOMode = MV_ATA_TRANSFER_PIO_3;
		} else if (iden[IDEN_PIO_MODE_SPPORTED] & MV_BIT1) {
			PIOMode = MV_ATA_TRANSFER_PIO_4;
		} else {
			PIOMode = MV_ATA_TRANSFER_PIO_SLOW;
		}
	} else {
		/* If word 64 isn't valid then Word 51 high byte holds
			* the PIO timing number for the maximum. Turn it into
			* a mask.
			* The value returned in Bits 15-8 shall be 00h for mode 0, 01h for
			* mode 1, or 02h for mode 2. Values 03h through FFh are reserved.
		*/
		MV_U8 mode = (iden[IDEN_OLD_PIO_MODES] >> 8) & 0xFF;
		if (mode == 0)
			PIOMode = MV_ATA_TRANSFER_PIO_0;
		if (mode == 1)
			PIOMode = MV_ATA_TRANSFER_PIO_1;
		if (mode == 2)
			PIOMode = MV_ATA_TRANSFER_PIO_2;
	}

	if ((iden[IDEN_ID_CONFIG] == 0x848A) ||	/* Traditional CF */
		(iden[IDEN_ID_CONFIG] == 0x844A)) {	/* Delkin Devices CF */
		/*
		 *	Process compact flash extended modes
		 */
		int pio = (iden[IDEN_CFA_MODES] & 0x7);
		if (pio == 1)
			PIOMode = MV_ATA_TRANSFER_PIO_5;
		if (pio == 2)
			PIOMode = MV_ATA_TRANSFER_PIO_6;

		/* leave the default PIO mode */
		goto skip_pio;
	}

	if (mvStorageDevATASetFeatures(pSataAdapter, channelIndex, port,
		MV_ATA_SET_FEATURES_TRANSFER, PIOMode, 0, 0, 0) == MV_FALSE) {
		printf("[%d %d %d]: failed to enable PIO mode [%d]\n", pSataAdapter->adapterId, channelIndex, port, PIOMode);
		return MV_FALSE;
	}

skip_pio:
	if (iden[IDEN_VALID] & MV_BIT2) {
		if (iden[IDEN_UDMA_MODE] & MV_BIT6) {
			UDMAMode = MV_ATA_TRANSFER_UDMA_6;
		} else if (iden[IDEN_UDMA_MODE] & MV_BIT5) {
			UDMAMode = MV_ATA_TRANSFER_UDMA_5;
		} else if (iden[IDEN_UDMA_MODE] & MV_BIT4) {
			UDMAMode = MV_ATA_TRANSFER_UDMA_4;
		} else {
			UDMAMode = MV_ATA_TRANSFER_UDMA_4;
		}

		priv->dmaSupported = MV_TRUE;
		if (mvStorageDevATASetFeatures(pSataAdapter, channelIndex, port,
			MV_ATA_SET_FEATURES_TRANSFER, UDMAMode, 0,0, 0) == MV_FALSE) {
			printf("[%d %d %d]: failed to perform Enable DMA mode\n", pSataAdapter->adapterId, channelIndex, port);
			return MV_FALSE;
		}
	} else {
		/* ignore the use_dma - disk support only PIO */
		priv->dmaSupported = MV_FALSE;
	}

	priv->adapterId = pSataAdapter->adapterId;
	priv->channelIndex = channelIndex;
	priv->port = port;
	/* Continue parsing identify buffer*/
	int device;
	device = dev_desc->dev;
	printf ("  Device %d @ %d %d", device, pSataAdapter->adapterId, channelIndex);
	if(deviceType == MV_SATA_DEVICE_TYPE_PM)
		printf(" %d:\n", port);
	else
		printf(":\n");

	ident_cpy ((unsigned char *)(dev_desc->revision), iop->fw_rev, sizeof(dev_desc->revision));
	ident_cpy ((unsigned char *)(dev_desc->vendor), iop->model, sizeof(dev_desc->vendor));
	ident_cpy ((unsigned char *)(dev_desc->product), iop->serial_no, sizeof(dev_desc->product));

	if ((iop->config & 0x0080)==0x0080)
		dev_desc->removable = 1;
	else
		dev_desc->removable = 0;

#ifdef __BIG_ENDIAN
	/* swap shorts */
	dev_desc->lba = (iop->lba_capacity << 16) | (iop->lba_capacity >> 16);
#else   /* ! __BIG_ENDIAN */
	dev_desc->lba = iop->lba_capacity;
#endif  /* __BIG_ENDIAN */

#ifdef CONFIG_LBA48
	if (iop->command_set_2 & 0x0400) { /* LBA 48 support */
		dev_desc->lba48 = 1;
		dev_desc->lba = (unsigned long long)iop->lba48_capacity[0] |
						((unsigned long long)iop->lba48_capacity[1] << 16) |
						((unsigned long long)iop->lba48_capacity[2] << 32) |
						((unsigned long long)iop->lba48_capacity[3] << 48);
	} else {
		dev_desc->lba48 = 0;
	}
#endif /* CONFIG_LBA48 */

	/* assuming HD */
	dev_desc->type = DEV_TYPE_HARDDISK;
	dev_desc->blksz = ATA_BLOCKSIZE;
	dev_desc->lun = 0; /* just to fill something in... */
	dev_desc->priv = priv; /* save info on disk in the priv pointer */

	return MV_TRUE;
}

void
InitChannel(
           PHW_ADAPTER_DESCRIPTION HwDeviceExtension,
           MV_U8 channelIndex)
{
	MV_SATA_ADAPTER     *pSataAdapter = &(HwDeviceExtension->mvSataAdapter);
	MV_SATA_CHANNEL     *pSataChannel;

	pSataChannel = &HwDeviceExtension->mvSataChannels[channelIndex];
	pSataAdapter->sataChannel[channelIndex] = pSataChannel;
	pSataChannel->channelNumber = channelIndex;
	pSataChannel->requestQueue = (struct mvDmaRequestQueueEntry  *)request_q;
	pSataChannel->requestQueuePciLowAddress = (MV_U32)request_q;
	pSataChannel->requestQueuePciHiAddress = 0;
	pSataChannel->responseQueue = (struct mvDmaResponseQueueEntry *) response_q;
	pSataChannel->responseQueuePciLowAddress = (MV_U32)response_q;
	pSataChannel->responseQueuePciHiAddress = 0;
}

MV_BOOLEAN
mvSataEventNotify(MV_SATA_ADAPTER *pSataAdapter, MV_EVENT_TYPE eventType,
                  MV_U32 param1, MV_U32 param2)
{

    switch (eventType)
    {
    case MV_EVENT_TYPE_SATA_CABLE:
	    switch(param1)
	    {
	    case MV_SATA_CABLE_EVENT_CONNECT:
		    printf("[%d,%d]: device connected event received\n",
			   pSataAdapter->adapterId, param2);
		    break;
	    case MV_SATA_CABLE_EVENT_DISCONNECT:
		    printf("[%d,%d]: device disconnected event received \n",
			   pSataAdapter->adapterId, param2);
		    break;
	    case MV_SATA_CABLE_EVENT_PM_HOT_PLUG:
		    printf("[%d,%d]: Port Multiplier hotplug event received \n",
			   pSataAdapter->adapterId, param2);
		    break;
	    default:
		    printf( "illegal value for param1(%d) at "
			    "connect/disconect event, host=%d\n", param1,
			    pSataAdapter->adapterId );
	    }
	    break;
    case MV_EVENT_TYPE_ADAPTER_ERROR:
	    printf("DEVICE error event received, pci cause "
		   "reg=%x, don't know how to handle this\n", param1);
	    return MV_TRUE;
    case MV_EVENT_TYPE_SATA_ERROR:
	    switch (param1)
		{
		case MV_SATA_RECOVERABLE_COMMUNICATION_ERROR:
			printf(" [%d %d] sata recoverable error occured\n",
			       pSataAdapter->adapterId, param2);
			break;
		case MV_SATA_UNRECOVERABLE_COMMUNICATION_ERROR:
			printf(" [%d %d] sata unrecoverable error occured, restart channel\n",
			       pSataAdapter->adapterId, param2);
			break;
		case MV_SATA_DEVICE_ERROR:
			printf( " [%d %d] device error occured\n",
				pSataAdapter->adapterId, param2);
			break;
		}
	    break;
    default:
	    printf(" adapter %d unknown event %d"
		   " param1= %x param2 = %x\n", pSataAdapter->adapterId,
		   eventType - MV_EVENT_TYPE_ADAPTER_ERROR, param1, param2);
	    return MV_FALSE;

    }/*switch*/

    return MV_TRUE;
}

/* ------------------------------------------------------------------------- */
#define bus_to_phys(devno, a)	pci_mem_to_phys(devno, a)

void ide_init (void)
{
	int j, numOfDrives;
	int idx = 0;
	unsigned int temp, initAdapterResult, pciCommand;
	unsigned short stemp;
	MV_SATA_ADAPTER *pMvSataAdapter;
	int devno = 0;
	unsigned char channelIndex;
	unsigned int numOfIdeDev = 0;
	unsigned int numOfAdapters = 0;
	MV_BOOLEAN integratedSataDetected = MV_FALSE;
	MV_BOOLEAN integratedSataInitialized = MV_FALSE;
	char *env;
	char * param;

#ifdef MV_LOGGER
#if defined (MV_LOG_DEBUG)
	mvLogRegisterModule(MV_CORE_DRIVER_LOG_ID, 0x1FF,
                        szModules[MV_CORE_DRIVER_LOG_ID]);
#elif defined (MV_LOG_ERROR)
	mvLogRegisterModule(MV_CORE_DRIVER_LOG_ID, MV_DEBUG_ERROR,
                        szModules[MV_CORE_DRIVER_LOG_ID]);
#endif
#endif
	printf("\nMarvell Serial ATA Adapter\n");
	curr_device = -1;

	env = getenv("sata_dma_mode");
	if(( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ))
	  use_dma = 1;
	else
	  use_dma = 0;

	param = getenv("sata_delay_reset");
	use_sata_delay_reset = param ? simple_strtol(param, NULL, 10) : 0;

	memset(sataAdapters,0, sizeof(HW_ADAPTER_DESCRIPTION)*CONFIG_SYS_IDE_MAXBUS);
	/* Init ide structure */
	for(j = 0; j < CONFIG_SYS_IDE_MAXDEVICE; j++)
	{
		ide_dev_desc[j].type=DEV_TYPE_UNKNOWN;
		ide_dev_desc[j].if_type=IF_TYPE_IDE;
		ide_dev_desc[j].dev=j;
		ide_dev_desc[j].part_type=PART_TYPE_UNKNOWN;
		ide_dev_desc[j].blksz=0;
		ide_dev_desc[j].lba=0;
		ide_dev_desc[j].block_read=ide_read;
	}

        while ((numOfIdeDev < CONFIG_SYS_IDE_MAXDEVICE) && (numOfAdapters < CONFIG_SYS_IDE_MAXBUS)) {
            MV_BOOLEAN integratedSataDevice = MV_FALSE;
		numOfDrives = 0;

#if defined(MV_INCLUDE_INTEG_SATA)

		if (integratedSataInitialized != MV_TRUE) {
			if (MV_FALSE == mvCtrlPwrClckGet(SATA_UNIT_ID, 0)) {
				printf("Warning Integrated SATA is Powered Off\n");
			} else {
				integratedSataDetected = MV_TRUE;
				integratedSataDevice = MV_TRUE;

				mvSysSataWinInit();
					printf("Integrated Sata device found\n");
			}
		}

#endif

#ifdef CONFIG_PCI
	if ((integratedSataDetected == MV_FALSE) || (integratedSataInitialized == MV_TRUE)) {
		/* Find PCI device(s) */
		if ((devno = pci_find_devices(supported, idx++)) < 0) {
			if(!numOfAdapters) printf("no device found \n");
			break;
		}
	}
#endif /* CONFIG_PCI */

	integratedSataInitialized = MV_TRUE;
	sataAdapters[numOfAdapters].devno = devno;
	sataAdapters[numOfAdapters].valid = MV_TRUE;

	memset (&sataAdapters[numOfAdapters].mvSataAdapter, 0, sizeof(MV_SATA_ADAPTER));
	pMvSataAdapter = &sataAdapters[numOfAdapters].mvSataAdapter;

#if defined(MV_INCLUDE_INTEG_SATA)
	if (integratedSataDevice == MV_TRUE) {
		pMvSataAdapter->adapterIoBaseAddress = INTER_REGS_BASE + MV_SATA_REGS_OFFSET - 0x20000;
		pMvSataAdapter->pciConfigDeviceId = mvCtrlModelGet();
		pMvSataAdapter->pciConfigRevisionId = 0;
	}
	else
#endif
#ifdef CONFIG_PCI
	{
		pci_read_config_dword(devno, PCI_BASE_ADDRESS_0 ,&temp);
		pMvSataAdapter->adapterIoBaseAddress = bus_to_phys(devno, (temp & 0xfffffff0));
		pci_read_config_word(devno, PCI_DEVICE_ID, &stemp);
		pMvSataAdapter->pciConfigDeviceId = stemp;
		pci_read_config_word(devno, PCI_REVISION_ID, &stemp);
		pMvSataAdapter->pciConfigRevisionId = stemp & 0xff;
	}
#endif /*CONFIG_PCI */

	pMvSataAdapter->adapterId = numOfAdapters;
	pMvSataAdapter->intCoalThre[0]= 4;
	pMvSataAdapter->intCoalThre[1]= 4;
	pMvSataAdapter->intTimeThre[0] = 150*50 ;
	pMvSataAdapter->intTimeThre[1] = 150*50;
	pMvSataAdapter->mvSataEventNotify = mvSataEventNotify;
	pMvSataAdapter->IALData = (void *)&sataAdapters[numOfAdapters];
	pMvSataAdapter->pciSerrMask = MV_PCI_SERR_MASK_REG_ENABLE_ALL;
	pMvSataAdapter->pciInterruptMask = 0;
	pMvSataAdapter->pciCommand = MV_PCI_COMMAND_REG_DEFAULT;

#ifdef CONFIG_PCI
	if (integratedSataDevice != MV_TRUE) {
		/* Enable master / IO / Memory accesses */
		pci_read_config_dword(devno, PCI_COMMAND ,&pciCommand);
		pci_write_config_dword(devno, PCI_COMMAND, pciCommand | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

		printf("Found adapter at bus %d, device %d ... Scanning channels\n",PCI_BUS(devno), PCI_DEV(devno));
	}
#endif /*CONFIG_PCI */
	/* Init adapter */
	initAdapterResult = mvSataInitAdapter(pMvSataAdapter);
	if (initAdapterResult == MV_FALSE){
		printf("Error Initializing SATA Adapter\n");
		sataAdapters[numOfAdapters].valid = MV_FALSE;
		numOfAdapters ++;
		continue;
	}

	/* Enable staggered spin-up of all SATA channels */
	if (mvSataEnableStaggeredSpinUpAll(pMvSataAdapter) == MV_FALSE) {
		printf("Error in mvSataEnableStaggeredSpinUpAll\n");
		sataAdapters[numOfAdapters].valid = MV_FALSE;
		numOfAdapters ++;
		continue;
	}
	/* when sending DMA command we poll interrupt for completion*/
	mvSataUnmaskAdapterInterrupt(pMvSataAdapter);

	/* find and init channels */
	for (channelIndex = 0 ; channelIndex < pMvSataAdapter->numberOfChannels ; channelIndex++) {
		if (mvSataIsStorageDeviceConnected(pMvSataAdapter, channelIndex, NULL) == MV_TRUE) {
			DP("Channel %x is connected ... ", channelIndex);
			InitChannel(&sataAdapters[numOfAdapters], channelIndex);
			if (mvSataConfigureChannel(pMvSataAdapter, channelIndex) == MV_FALSE) {
				printf("Error in mvSataConfigureChannel on channel %d\n",channelIndex);
				pMvSataAdapter->sataChannel[channelIndex] = NULL;
				numOfIdeDev++;
			} else if (StartChannel(&sataAdapters[numOfAdapters], channelIndex) == MV_FALSE) {
				printf ("Failed initializing storage deivce connected "
					"to SATA channel %d\n",channelIndex);
				pMvSataAdapter->sataChannel[channelIndex] = NULL;
				numOfIdeDev++;
			} else {
				int port;
				MV_CHANNEL_INFO *channelInfo=&sataAdapters[numOfAdapters].channelInfo[channelIndex];
				for (port = 0; port < channelInfo->numberOfPorts; port++){
					if (!(channelInfo->connected & (1 << port))) {
						numOfIdeDev++;
						continue;
					}

					if (initDisk(pMvSataAdapter, channelIndex, port, &ide_dev_desc[numOfIdeDev]) == MV_FALSE) {
						/* mark it as not connected */
						sataAdapters[numOfAdapters].channelInfo[channelIndex].connected &= ~(1 << port);
						numOfIdeDev++;
						continue;
					}
					//					printf("OK\n");
					dev_print(&ide_dev_desc[numOfIdeDev]);
					if ((ide_dev_desc[numOfIdeDev].lba > 0) &&
					    (ide_dev_desc[numOfIdeDev].blksz > 0))
						{
							init_part (&ide_dev_desc[numOfIdeDev]);			/* initialize partition type */
							if (curr_device < 0)
								curr_device = numOfIdeDev;
						}
					numOfDrives++;
					numOfIdeDev++;
				}
			}
		}else{
			pMvSataAdapter->sataChannel[channelIndex] = NULL;
			sataAdapters[numOfAdapters].channelInfo[channelIndex].numberOfPorts = 1;
			numOfIdeDev++;
		}

	}

	numOfAdapters++;
	}

	ide_detected = numOfIdeDev;
	ide_initiated=1;

	putc ('\n');
}

/* ------------------------------------------------------------------------- */

block_dev_desc_t * ide_get_dev(int dev)
{
	return ((block_dev_desc_t *)&ide_dev_desc[dev]);
}

/* ------------------------------------------------------------------------- */

ulong ide_read_write (int device, lbaint_t blknr, lbaint_t blkcnt, ulong *buffer, int read)
{
	MV_SATA_ADAPTER *pSataAdapter = NULL;
	MV_SATA_DISK_PRIV *priv = ide_dev_desc[device].priv;

	/* read count is 0 no read is needed */
	if(blkcnt == 0) {
		return 0;
	}

	/* data buffer must be 2 bytes aligned */
	if((long)buffer & 1) {
		printf("\nide error: address (0x%p) is not 2 bytes aligned!\n",buffer);
		return 0;
	}

	DP("ide %s: device %x, blknr %x blkcnt %lx, buffer %x\n",
		((read)?"read":"write"),device, blknr, blkcnt, (unsigned int)buffer);

	pSataAdapter = &(sataAdapters[priv->adapterId].mvSataAdapter);

	if ((priv->dmaSupported) && (use_dma))
		return dma_read_write(pSataAdapter, priv->channelIndex, priv->port,
				blknr, blkcnt, buffer, read);
	else
		return pio_read_write(pSataAdapter, priv->channelIndex, priv->port,
				blknr, blkcnt, buffer, read);
}

static
ulong pio_read_write (MV_SATA_ADAPTER *pSataAdapter,
		      unsigned int channelIndex,
		      MV_U8 port,
		      lbaint_t blknr, lbaint_t blkcnt, ulong *buffer, int read)
{
	MV_STORAGE_DEVICE_REGISTERS inATARegs;
	MV_STORAGE_DEVICE_REGISTERS outATARegs;
	MV_BOOLEAN isExt = MV_FALSE;
	unsigned int command;
	ulong blk_transferred = 0;


#if defined(__BE)
	if(read == 0)
		swapATABuffer((unsigned short *)buffer, blkcnt * ATA_SECTOR_SIZE);
#endif

	/* read all blocks */
	while(blkcnt > 0) {
		ulong cmd_blkcnt = (blkcnt > 128 ) ? 128 : blkcnt;

		/* Set transfer mode */
		memset(&inATARegs, 0, sizeof(inATARegs));
		memset(&outATARegs, 0, sizeof(outATARegs));
		inATARegs.commandRegister = (read)? MV_ATA_COMMAND_READ_SECTORS: MV_ATA_COMMAND_WRITE_SECTORS;
		inATARegs.sectorCountRegister = cmd_blkcnt;
		inATARegs.lbaLowRegister = ((blknr >> 0) & 0xFF);
		inATARegs.lbaMidRegister = ((blknr >> 8) & 0xFF);
		inATARegs.lbaHighRegister = ((blknr >> 16) & 0xFF);
		inATARegs.deviceRegister = BIT6 | ((blknr >> 24) & 0xF);

#ifdef CONFIG_LBA48
		/* more than 28 bits used, use 48bit mode */
		if (blknr & IDE_BLOCK_NUMBER_MASK) {
			inATARegs.commandRegister = (read)? MV_ATA_COMMAND_READ_SECTORS_EXT : MV_ATA_COMMAND_WRITE_SECTORS_EXT;
			inATARegs.lbaLowRegister |=  ((blknr >> LBA_LOW_REG_SHIFT) & 0xFF) << 8;
			inATARegs.lbaMidRegister |=  ((blknr >> LBA_MID_REG_SHIFT) & 0xFF) << 8;
			inATARegs.lbaHighRegister |= ((blknr >> LBA_HIGH_REG_SHIFT) & 0xFF) << 8;
			inATARegs.deviceRegister = BIT6;
			isExt = MV_TRUE;
		}
#endif
		command = (read)? MV_NON_UDMA_PROTOCOL_PIO_DATA_IN: MV_NON_UDMA_PROTOCOL_PIO_DATA_OUT;
		if (mvStorageDevExecutePIO(pSataAdapter, channelIndex, port, command,
                               isExt, (MV_U16 *)buffer, (cmd_blkcnt * 512)/2 , &inATARegs, &outATARegs) == MV_FALSE)
		{
			printf("[%d %d]: %s Fail for PIO\n",
				pSataAdapter->adapterId, channelIndex, ((read)?"Read":"Write"));
			return 0;
		}
		blknr += cmd_blkcnt;
		blkcnt = blkcnt - cmd_blkcnt;
		buffer += (cmd_blkcnt * ATA_SECTORWORDS);
		blk_transferred += cmd_blkcnt;
	}
#if defined(__BE)
		swapATABuffer((unsigned short *)buffer, blkcnt * ATA_SECTOR_SIZE);
#endif

	return blk_transferred;
}
static int command_completed;
static MV_COMPLETION_TYPE completion_type;
static
MV_BOOLEAN cmd_callback(struct mvSataAdapter *pSataAdapter,
			MV_U8 channel_index,
			MV_COMPLETION_TYPE type,
			MV_VOID_PTR command_id, MV_U16 edma_error_cause,
			MV_U32 time_stamp,
			struct mvStorageDevRegisters *deviceRegs){
	command_completed = 1;
	completion_type = type;
	return MV_TRUE;
}
ulong dma_read_write (MV_SATA_ADAPTER *pSataAdapter,
		      unsigned int channelIndex,
		      MV_U8 port,
		      lbaint_t blknr, lbaint_t blkcnt, ulong *buffer, int read)
{
	int res = 0;
	int transfered_blks = 0;

	/* we use single PRD table entry (limited to 64KB - 128 sector) */
	while(blkcnt){
		int chunk = (blkcnt > 128) ? 128: blkcnt;

		res = dma_read_write_cmd(pSataAdapter, channelIndex, port, blknr, chunk,
					 buffer, read) ;

		if(res != chunk)
			return transfered_blks;

		transfered_blks += res;
		buffer += res << 7; //buffer is in long
		blknr += res;
		blkcnt -= res;
	}
	return transfered_blks;
}

static
ulong dma_read_write_cmd (MV_SATA_ADAPTER *pSataAdapter,
			  unsigned int channelIndex,
			  MV_U8 port,
			  lbaint_t blknr, lbaint_t blkcnt, ulong *buffer, int read)
{
	MV_QUEUE_COMMAND_INFO q_cmd_info;
	MV_UDMA_COMMAND_PARAMS      *udmaCommand = &q_cmd_info.commandParams.udmaCommand;
	MV_SATA_DEVICE_TYPE	deviceType;
	MV_U32			byte_count = blkcnt << 9;
	MV_U32 buffer_addr = (MV_U32) buffer;

	if ( blkcnt > 128){
		printf("error in %s: blk count %llx exceeded max limit\n",
		       __func__, blkcnt);
	}

	//	mvSataDisableChannelDma(pSataAdapter, channelIndex);
	/* reset the request/response pointers as we allcated one entry*/
	/* but some info need to be preserved */
	deviceType = mvStorageDevGetDeviceType(pSataAdapter, channelIndex);
	mvSataConfigureChannel(pSataAdapter, channelIndex);
	mvStorageDevSetDeviceType(pSataAdapter, channelIndex,deviceType);
	mvSataConfigEdmaMode(pSataAdapter, channelIndex,
			     MV_EDMA_MODE_NOT_QUEUED, 2);
	mvSataEnableChannelDma(pSataAdapter, channelIndex);


	memset(&q_cmd_info, 0, sizeof(MV_QUEUE_COMMAND_INFO));
	memset(prd_table, 0, 2 * sizeof(MV_SATA_EDMA_PRD_ENTRY));

	/* buffer must not cross address decode window */
	if(((buffer_addr & MRVL_SATA_BOUNDARY_MASK) + byte_count) > MRVL_SATA_BUFF_BOUNDARY)
	{
	     MV_U32 chunk_len = MRVL_SATA_BUFF_BOUNDARY -
		  (buffer_addr & MRVL_SATA_BOUNDARY_MASK);

	     memset(prd_table + 1, 0, sizeof(MV_SATA_EDMA_PRD_ENTRY));
	     prd_table[0].lowBaseAddr = cpu_to_le32(buffer_addr);
	     prd_table[0].byteCount = cpu_to_le16(chunk_len & 0xFFFF);

	     byte_count -= chunk_len;
	     buffer_addr += chunk_len;
	     prd_table[1].lowBaseAddr = cpu_to_le32(buffer_addr);
	     prd_table[1].byteCount = cpu_to_le16(byte_count & 0xFFFF);
	     prd_table[1].flags = cpu_to_le16(MV_EDMA_PRD_EOT_FLAG);

	}
	else
	{
	     prd_table[0].lowBaseAddr = cpu_to_le32(buffer_addr);
	     prd_table[0].byteCount = cpu_to_le16(byte_count & 0xFFFF);
	     prd_table[0].flags = cpu_to_le16(MV_EDMA_PRD_EOT_FLAG);
	}

	q_cmd_info.type = MV_QUEUED_COMMAND_TYPE_UDMA;
	q_cmd_info.PMPort = port;
	udmaCommand->readWrite = (read)? MV_UDMA_TYPE_READ : MV_UDMA_TYPE_WRITE;
	udmaCommand->isEXT = MV_FALSE;

#ifdef CONFIG_LBA48
	/* more than 28 bits used, use 48bit mode */
	if (blknr & IDE_BLOCK_NUMBER_MASK) {
	  udmaCommand->isEXT = MV_TRUE;
	}
#endif

	udmaCommand->FUA = MV_FALSE;
	udmaCommand->lowLBAAddress = blknr & 0xFFFFFFFF;
#ifdef CONFIG_SYS_64BIT_LBA
	udmaCommand->highLBAAddress = (blknr >> 32 )& 0xFFFFFFFF;
#else
	udmaCommand->highLBAAddress = 0;
#endif
	udmaCommand->numOfSectors = blkcnt;
	udmaCommand->prdLowAddr = (MV_U32)prd_table;
	udmaCommand->callBack = cmd_callback;

	command_completed = 0;
	if(mvSataQueueCommand(pSataAdapter,
			      channelIndex,
			      &q_cmd_info) != MV_QUEUE_COMMAND_RESULT_OK){
		printf("error in mv_ide: queue command failed\n");
		mvSataDisableChannelDma(pSataAdapter, channelIndex);
		return 0;
	}

	do{
		mvSataInterruptServiceRoutine(pSataAdapter);
	}while(!command_completed);

	mvSataDisableChannelDma(pSataAdapter, channelIndex);
	if(completion_type == MV_COMPLETION_TYPE_NORMAL)
		return blkcnt;
	else
		return 0;
}

/* ------------------------------------------------------------------------- */
#if defined(__BE)
void swapATABuffer(unsigned short *buffer, ulong count)
{
	count >>= 1;
	while(count--)
	{
		buffer[count] = MV_CPU_TO_LE16(buffer[count]);
	}
}
#endif

unsigned long ide_read (int device, unsigned long blknr, lbaint_t blkcnt, void *buffer)
{
	return ide_read_write (device, (lbaint_t)blknr, blkcnt, buffer, MV_TRUE);
}
unsigned long ide_write(int device, unsigned long blknr, lbaint_t blkcnt, const void *buffer)
{
	return ide_read_write (device, (lbaint_t)blknr, blkcnt, (ulong *)buffer, MV_FALSE);
}

/* ------------------------------------------------------------------------- */

/*
 * copy src to dest, skipping leading and trailing blanks and null
 * terminate the string
 * "len" is the size of available memory including the terminating '\0'
 */
static void ident_cpy (unsigned char *dst, unsigned char *src, unsigned int len)
{
	unsigned char *end, *last;

	last = dst;

	/* Make sure lenis multiple of 2 + 1 for '\0'*/
	if( !(len % 2) || (len == 0))
		goto OUT;

	end  = src + len - 1;

	while ((*src) && (src<end))  {
		if(((unsigned int)src % 2) == 0)
		{
			#if defined(__BE)
			*dst = *src;
			#else
			*(dst+1) = *src;
			#endif
		}
		else
		{
			#if defined(__BE)
			*(dst+1) = *src;
			#else
			*dst = *src;
			#endif

			dst+=2;
			last = dst;

		}
		src++;
	}
OUT:
	*last = '\0';
}


/* ------------------------------------------------------------------------- */


U_BOOT_CMD(
	ide,  5,  1,  do_ide,
	"ide     - IDE sub-system\n",
	"reset - reset IDE controller\n"
	"ide info  - show available IDE devices\n"
	"ide device [dev] - show or set current device\n"
	"ide part [dev] - print partition table of one or all IDE devices\n"
	"ide read  addr blk# cnt\n"
	"ide write addr blk# cnt - read/write `cnt'"
	" blocks starting at block `blk#'\n"
	"    to/from memory address `addr'\n"
);

U_BOOT_CMD(
	diskboot,	3,	1,	do_diskboot,
	"diskboot- boot from IDE device\n",
	"loadAddr dev:part\n"
);

#endif
