/*
 * (C) Copyright 2000
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

/* PCI.c - PCI functions */
#include "mvCommon.h"
#include <common.h>
#include <config.h>
#include <command.h>
#if defined(CONFIG_CMD_PCI)


#include <pci.h>
#include "mvSysPexApi.h"
#include "pex/mvPexRegs.h"
#include "gpp/mvGpp.h"
//#include "pci-if/mvPciIf.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "pci-if/pci_util/mvPciUtils.h"

#undef DEBUG
#ifdef DEBUG
#define DB(x) x
#define DB1(x) 
#else
#define DB(x)
#define DB1(x) 
#endif /* DEBUG */

/* global definetion */
#define REG_NUM_MASK	(0x3FF << 2)
/* global indicate wether we are in the scan process */
unsigned int bus_scan = 0;
extern unsigned int whoAmI(void);


#if defined(CONFIG_CMD_BSP)

/******************************************************************************
* Category     - PCI0
* Functionality- Scans PCI0 for devices and prints relevant information
* Need modifications (Yes/No) - No
*****************************************************************************/
MV_BOOL scanPci(MV_U32 host)
{
	MV_U32    index,numOfElements=4*8,barIndex;
	MV_PCI_DEVICE      pciDevices[4*8]; //3 slots and us,Max 8 functions per slot

	memset (&pciDevices,0,12*sizeof(MV_PCI_DEVICE));


	if (mvPciScan(host, pciDevices , &numOfElements) != MV_OK ) {
		DB(printf("scanPci:mvPciScan failed for host %d \n",host));
		return MV_FALSE;
	}

	for(index = 0; index < numOfElements ; index++)
	{
	    printf("\nBus: %x Device: %x Func: %x Vendor ID: %x Device ID: %x\n",
			pciDevices[index].busNumber,
			pciDevices[index].deviceNum,
			pciDevices[index].function,
			pciDevices[index].venID,
			pciDevices[index].deviceID);

		printf("-------------------------------------------------------------------\n");

		printf("Class: %s\n",pciDevices[index].type);

		/* check if we are bridge*/
		if ((pciDevices[index].baseClassCode == PCI_BRIDGE_CLASS)&&
			(pciDevices[index].subClassCode == P2P_BRIDGE_SUB_CLASS_CODE))
		{
			printf("Primary Bus:0x%x \tSecondary Bus:0x%x \tSubordinate Bus:0x%x\n",
							pciDevices[index].p2pPrimBusNum,
							pciDevices[index].p2pSecBusNum,
							pciDevices[index].p2pSubBusNum);

			printf("IO Base:0x%x \t\tIO Limit:0x%x",pciDevices[index].p2pIObase,
							pciDevices[index].p2pIOLimit);

			(pciDevices[index].bIO32)? (printf(" (32Bit IO)\n")):
								(printf(" (16Bit IO)\n"));

			printf("Memory Base:0x%x \tMemory Limit:0x%x\n",pciDevices[index].p2pMemBase,
							pciDevices[index].p2pMemLimit);

			printf("Pref Memory Base:0x%x \tPref Memory Limit:0x%x",
				   pciDevices[index].p2pPrefMemBase,
							pciDevices[index].p2pPrefMemLimit);

			(pciDevices[index].bPrefMem64)? (printf(" (64Bit PrefMem)\n")):
								(printf(" (32Bit PrefMem)\n"));
			if (pciDevices[index].bPrefMem64)
			{
				printf("Pref Base Upper 32bit:0x%x \tPref Limit Base Upper32 bit:0x%x\n",
								pciDevices[index].p2pPrefBaseUpper32Bits,
								pciDevices[index].p2pPrefLimitUpper32Bits);
			}
		}

	for (barIndex = 0 ; barIndex < pciDevices[index].barsNum ; barIndex++)
        {

	   if (pciDevices[index].pciBar[barIndex].barType == PCI_64BIT_BAR)
	   {
           printf("PCI_BAR%d (%s-%s) base: %x%08x%s",barIndex,
                  (pciDevices[index].pciBar[barIndex].barMapping == PCI_MEMORY_BAR)?"Mem":"I/O",
                  "64bit",
                  pciDevices[index].pciBar[barIndex].barBaseHigh,
                  pciDevices[index].pciBar[barIndex].barBaseLow,
                  (pciDevices[index].pciBar[barIndex].barBaseLow == 0)?"\t\t":"\t");
	   }
	   else if (pciDevices[index].pciBar[barIndex].barType == PCI_32BIT_BAR)
	   {
             printf("PCI_BAR%d (%s-%s) base: %x%s",barIndex,
                (pciDevices[index].pciBar[barIndex].barMapping == PCI_MEMORY_BAR)?"Mem":"I/O",
                "32bit",
                pciDevices[index].pciBar[barIndex].barBaseLow,
                (pciDevices[index].pciBar[barIndex].barBaseLow == 0)?"\t\t\t":"\t\t");
           }

         if(pciDevices[index].pciBar[barIndex].barSizeHigh != 0)
            printf("size: %d%08d bytes\n",pciDevices[index].pciBar[barIndex].barSizeHigh,
                                         pciDevices[index].pciBar[barIndex].barSizeLow);
         else
            printf("size: %dMb\n", pciDevices[index].pciBar[barIndex].barSizeLow / 1024 / 1024);
        }
    }
    return MV_TRUE;
}


int sp_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 host = 0;
#if defined(MV88F78X60)
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
#endif
	MV_U32 pexHWInf = 0;

	if (argc > 1)
		host = simple_strtoul (argv[1], NULL, 10);

	if(host >= mvCtrlPexMaxIfGet()){
		printf("PCI %d doesn't exist\n",host);
		return 1;
	}

#if defined(MV88F78X60)
	pexHWInf = boardPexInfo->pexMapping[host];
#else
	pexHWInf = host;
#endif

	printf("scanning pex number: %d\n", pexHWInf);
	if( scanPci(pexHWInf) == MV_FALSE)
		printf("PCI %d Scan - FAILED!!.\n",host);
	return 1;
}

U_BOOT_CMD(
	sp,      2,     1,      sp_cmd,
	"sp	- Scan PCI Interface [bus].\n",
	"\tScan and detect all devices on mvPCI Interface \n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

int me_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 host = 0;
	if (argc > 1)
		host = simple_strtoul (argv[1], NULL, 10);


	if(host >= mvCtrlPexMaxIfGet()) {
		printf("Master %d doesn't exist\n",host);
		return 1;
	}

	if(mvPexMasterEnable(host,MV_TRUE)  == MV_OK)
		printf("PCI %d Master enabled.\n",host);
	else
		printf("PCI %d Master enabled -FAILED!!\n",host);

	return 1;
}

U_BOOT_CMD(
        me,      2,      1,      me_cmd,
        "me	- PCI master enable\n",
        " [0/1] \n"
        "\tEnable the MV device as Master on PCI 0/1. \n"
);

int se_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 host=0,dev = 0,bus=0;
#if defined(MV88F78X60)
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
#endif
	MV_U32 pexHWInf = 0;


	if(argc != 4) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	host = simple_strtoul (argv[1], NULL, 10);
	bus = simple_strtoul (argv[2], NULL, 16);
	dev = simple_strtoul (argv[3], NULL, 16);

	if(host >= mvCtrlPexMaxIfGet()) {
		printf("PCI %d doesn't exist\n",host);
		return 1;
	}
	
#if defined(MV88F78X60)
	pexHWInf = boardPexInfo->pexMapping[host];
#else
	pexHWInf = host;
#endif

	if(mvPexSlaveEnable(pexHWInf,bus,dev,MV_TRUE) == MV_OK )
			printf("PCI %d Bus %d Slave 0x%x enabled.\n",host,bus,dev);
	else
		printf("PCI %d Bus %d Slave 0x%x enabled - FAILED!!.\n",host,bus,dev);
	return 1;
}

U_BOOT_CMD(
        se,      4,     1,      se_cmd,
        "se	- PCI Slave enable\n",
        " [0/1] bus dev \n"
        "\tEnable the PCI device as Slave on PCI 0/1. \n"
);

/******************************************************************************
* Functionality- The commands changes the pci remap register and displays the
*                address to be used in order to access PCI 0.
*****************************************************************************/
int mapPci_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_ADDR_WIN pciWin;
	MV_TARGET target=0;
	MV_U32 host=0,effectiveBaseAddress=0;
#if defined(MV88F78X60)
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
#endif
	MV_U32 pexHWInf = 0;

	pciWin.baseLow=0;
	pciWin.baseHigh=0;

	if (argc > 1) {
		host = simple_strtoul(argv[1], NULL, 10);
	}

    if(argc > 2) {
		pciWin.baseLow = simple_strtoul(argv[2], NULL, 16);
	}

	if(host >= mvCtrlPexMaxIfGet()){
		printf("PCI %d doesn't exist\n",host);
		return 1;
	}

#if defined(MV88F78X60)
	pexHWInf = boardPexInfo->pexMapping[host];
#else
	pexHWInf = host;
#endif
	
	target = PCI0_MEM0 + (2 * pexHWInf);

	printf("mapping pci %x to address 0x%x\n",host,pciWin.baseLow);

#if defined(MV_INCLUDE_PEX) || defined(MV_INCLUDE_PCI)
	effectiveBaseAddress = mvCpuIfPexRemap(target, &pciWin);
#endif

	if ( effectiveBaseAddress == 0xffffffff) {
		printf("Error remapping\n");
		return 1;
	}

	printf("PCI %x Access base address : %x\n",host,effectiveBaseAddress);
	return 1;
}

U_BOOT_CMD(
        mp,      3,     1,      mapPci_cmd,
        "mp	- map PCI BAR\n",
        " [0/1] address \n"
        "\tChange the remap of PCI 0/1 window 0 to address 'addrress'.\n"
        "\tIt also displays the new access address, since the remap is not always\n"
        "\tthe same as requested. \n"
);

#endif

MV_U32 mv_mem_ctrl_dev(MV_U32 pexIf, MV_U32 bus,MV_U32 dev)
{
	MV_U32 ven, class;

	ven =    mvPexConfigRead(pexIf,bus,dev,0,PCI_VENDOR_ID) & 0xffff;
	class = (mvPexConfigRead(pexIf,bus,dev,0,PCI_REVISION_ID) >> 16 ) & 0xffff;
	/* if we got any other Marvell PCI cards ignore it. */
	if(((ven == 0x11ab) && (class == PCI_CLASS_MEMORY_OTHER))||
	  ((ven == 0x11ab) && (class == PCI_CLASS_BRIDGE_HOST)))
		return 1;
	return 0;
}

static int mv_read_config_dword(struct pci_controller *hose,
				pci_dev_t dev,
				int offset, u32* value)
{
	MV_U32 bus,func,regOff,dev_no;
	char *env;

	bus = PCI_BUS(dev);
	dev_no = PCI_DEV(dev);

	func = (MV_U32)PCI_FUNC(dev);
	regOff = (MV_U32)offset & REG_NUM_MASK;

	/*  We will scan only ourselves and the PCI slots that exist on the
		board, because we may have a case that we have one slot that has
		a Cardbus connector, and because CardBus answers all IDsels we want
		to scan only this slot and ourseleves.
	*/

	if( bus_scan == 1 ) {
		env = getenv("disaMvPnp");
		if(env && ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) ) {
			if( mv_mem_ctrl_dev((MV_U32)hose->cfg_addr, bus, dev_no) ) {
				*value = 0xffffffff;
				return 0;
			}
		}
	}
	DB(printf("mv_read_config_dword hose->cfg_addr %x\n",hose->cfg_addr);)
	DB(printf("mv_read_config_dword bus %x\n",bus);)
	DB(printf("mv_read_config_dword dev_no %x\n",dev_no);)
	DB(printf("mv_read_config_dword func %x\n",func);)
	DB(printf("mv_read_config_dword regOff %x\n",regOff);)

	*value = (u32) mvPexConfigRead((MV_U32)hose->cfg_addr,bus,dev_no,func,regOff);
	DB(printf("mv_read_config_dword value %x\n",*value);)

	return 0;
}

static int mv_write_config_dword(struct pci_controller *hose,
				 pci_dev_t dev,
				 int offset, u32 value)
{
	MV_U32 bus,func,regOff,dev_no;

	bus = PCI_BUS(dev);
	dev_no = PCI_DEV(dev);
	func = (MV_U32)PCI_FUNC(dev);
	regOff = offset & REG_NUM_MASK;
	mvPexConfigWrite((MV_U32)hose->cfg_addr,bus,dev_no,func,regOff,value);

	return 0;
}



static void mv_setup_ide(struct pci_controller *hose,
			 pci_dev_t dev, struct pci_config_table *entry)
{
    static const int ide_bar[]={8,4,8,4,16,1024};
    u32 bar_response, bar_value;
    int bar;

    for (bar=0; bar<6; bar++) {
	/*ronen different function for 3rd bank.*/
	unsigned int offset = (bar < 2)? bar*8: 0x100 + (bar-2)*8;

	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0 + offset, 0x0);
	pci_read_config_dword(dev, PCI_BASE_ADDRESS_0 + offset, &bar_response);

	pciauto_region_allocate(bar_response & PCI_BASE_ADDRESS_SPACE_IO ?
				hose->pci_io : hose->pci_mem, ide_bar[bar], &bar_value);

	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0 + bar*4, bar_value);
    }
}

static void mv_setup_host(struct pci_controller *hose,
			 pci_dev_t dev, struct pci_config_table *entry)
{
	//skip our host
	DB(printf("skipping :bus=%x dev=%x fun=%x\n",
			  (unsigned int)PCI_BUS(dev),
			  (unsigned int)PCI_DEV(dev),
			  (unsigned int)PCI_FUNC(dev)));
	return;
}


static void mv_pci_bus_mode_display(MV_U32 host)
{
#if defined(MV_INCLUDE_PEX)

	MV_PEX_MODE			pexMode;
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
	MV_U32				pexHWInf;
	MV_32				linkDelayCount;

	if (boardPexInfo == NULL) {
		printf("mv_pci_bus_mode_display: mvBoardPexInfoGet failed\n");
		return;
	}
	
#if defined(MV88F78X60)
	pexHWInf = boardPexInfo->pexMapping[host];
#else
	pexHWInf = host;
#endif
	
	if (mvPexModeGet(pexHWInf, &pexMode) != MV_OK)
		printf("mv_pci_bus_mode_display: mvPexModeGet failed\n");

#if defined(MV88F78X60)
	printf("PEX %d.%d(%d): ",(pexHWInf<8)?(pexHWInf/4):(pexHWInf-6) , (pexHWInf<8)?(pexHWInf%4):0, host);
#else
	printf("PEX %d: ",host);
#endif

	switch (pexMode.pexType)
	{
		case MV_PEX_ROOT_COMPLEX:
			printf("Root Complex Interface");
			break;
		case MV_PEX_END_POINT:
			printf("End Point Interface");
			break;
	}
	linkDelayCount = 2000;
	while (!(pexMode.pexLinkUp) && (linkDelayCount))  {
		mvOsDelay(1);
		linkDelayCount--;
		if (mvPexModeGet(pexHWInf, &pexMode) != MV_OK){
			printf("mv_pci_bus_mode_display: mvPexModeGet failed\n");
			break;
			}
	}

	/* Check if we have link */
	if (!(pexMode.pexLinkUp))
	{
		printf(", no Link.\n");
	}
	else
	{
		if (MV_PEX_WITDH_X1 ==  pexMode.pexWidth)
		{
			printf(", Detected Link X1");
		}
		else if (MV_PEX_WITDH_X4 ==  pexMode.pexWidth)
		{
			printf(", Detected Link X4");
		}
		if (MV_PEX_GEN2_0 ==  pexMode.pexGen)
		{
			printf(", GEN 2.0\n");
		}
		else if (MV_PEX_GEN1_1 ==  pexMode.pexGen)
		{
			printf(", GEN 1.1\n");
		}

	}

	return ;

	#endif /* MV_INCLUDE_PEX */
}

struct pci_config_table mv_config_table[] = {
    { PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_STORAGE_IDE,
      PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, mv_setup_ide},

    { PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID,
      PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, mv_setup_host}, //PCI host

    {}

};


/* Defines for more modularity of the pci_init_board function */

struct pci_controller  pci_hose[8];


//#if (MV_PEX_MAX_IF == 2)

#define PCI_IF_MEM(pexIf)	((pexIf==0)?PCI_IF0_MEM0:PCI_IF1_MEM0)
#define PCI_IF_REMAPED_MEM_BASE(pexIf) ((pexIf==0)?PCI_IF0_REMAPED_MEM_BASE:PCI_IF1_REMAPED_MEM_BASE)
#define PCI_IF_MEM_BASE(pexIf)	((pexIf==0)?PCI_IF0_MEM0_BASE:PCI_IF1_MEM0_BASE)
#define PCI_IF_MEM_SIZE(pexIf)	((pexIf==0)?PCI_IF0_MEM0_SIZE:PCI_IF1_MEM0_SIZE)
#define PCI_IF_IO_BASE(pexIf)	((pexIf==0)?PCI_IF0_IO_BASE:PCI_IF1_IO_BASE)
#define PCI_IF_IO_SIZE(pexIf)	((pexIf==0)?PCI_IF0_IO_SIZE:PCI_IF1_IO_SIZE)
/*
#else

#define PCI_IF_MEM(pexIf)	(PCI_IF0_MEM0)
#define PCI_IF_REMAPED_MEM_BASE(pexIf) (PCI_IF0_REMAPED_MEM_BASE)
#define PCI_IF_MEM_BASE(pexIf)	(PCI_IF0_MEM0_BASE)
#define PCI_IF_MEM_SIZE(pexIf)	(PCI_IF0_MEM0_SIZE)
#define PCI_IF_IO_BASE(pexIf)	(PCI_IF0_IO_BASE)
#define PCI_IF_IO_SIZE(pexIf)	(PCI_IF0_IO_SIZE)

#endif*/

/* because of CIV team needs we are gonna do a remap to PCI memory */
#define PCI_IF0_REMAPED_MEM_BASE	0x40000000
#define PCI_IF1_REMAPED_MEM_BASE	0x40000000

void pci_init_board(void)
{
	/* removed PEX init from all other cores except 0 as asked from CV */
	if (whoAmI() != 0)
		return;
	MV_U16 ctrlModel;
	MV_U32 pexIfNum = mvCtrlPexMaxIfGet();
	MV_U32 pexIf=0;
	MV_U32 pexLane = 0;
	MV_U32 pexUnit = 0;
	MV_U32 maxLinkWidth = 0;
	MV_U32 negLinkWidth = 0;
	MV_U32 tmp = 0;

	MV_ADDR_WIN rempWin;
	char *env;
	int status;
	MV_CPU_DEC_WIN  cpuAddrDecWin;
	PCI_IF_MODE	pexIfMode = PCI_IF_MODE_HOST;
	int pexIfStart = 0;
	MV_U32 addr;
	MV_U32 tempReg;
	MV_U32 tempPexReg;
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
	MV_U32 pexHWInf = 0;
    if(pexIfNum == 0)
        return;
	env = getenv("disaMvPnp");

	if(env && ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
		printf("Warning: skip configuration of Marvell devices!!!\n");


	pexIfNum = boardPexInfo->boardPexIfNum;


	/* start pci scan */
	for (pexIf = pexIfStart; pexIf < pexIfNum; pexIf++) {

#if defined(MV88F78X60)
		pexHWInf = boardPexInfo->pexMapping[pexIf];
#else
		pexHWInf = pexIf;
#endif
		if (pexIf == 0) {
			pci_hose[pexIf].first_busno = 0;
			pci_hose[pexIf].last_busno = 0;
		} else {
			pci_hose[pexIf].first_busno = pci_hose[pexIf-1].last_busno + 1;
			pci_hose[pexIf].last_busno = pci_hose[pexIf].first_busno;
                  }

		pci_hose[pexIf].config_table = mv_config_table;
#if !defined(MV88F78X60)
		if (MV_FALSE == mvCtrlPwrClckGet(PEX_UNIT_ID, pexHWInf)) {
			continue;
		}
#endif
		/* device or host ? */
#if defined(MV_INCLUDE_PEX)

		/* Set pex mode incase S@R not exist */
		env = getenv("pexMode");
		if (env && (((strcmp(env,"EP") == 0) || (strcmp(env,"ep") == 0) )))
		    pexIfMode = MV_PEX_END_POINT;
		else
		    pexIfMode = MV_PEX_ROOT_COMPLEX;

#endif

#if defined(DB_78X60_PCAC) || defined(DB_78X60_PCAC_REV2) || defined(DB_88F6710_PCAC)
		pexIfMode = MV_PEX_END_POINT;
#endif

#if defined(DB_78X60_AMC)
		pexIfMode = MV_PEX_ROOT_COMPLEX;
#endif

		/* PEX capability */
		ctrlModel=mvCtrlModelGet();
		if ((ctrlModel == MV_6710_DEV_ID) ||
			(ctrlModel == MV_6W11_DEV_ID) ||
			(ctrlModel == MV_6707_DEV_ID)) {
			tempPexReg = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexHWInf, PEX_LINK_CAPABILITY_REG));
			tempPexReg &= ~(0xF);

			#if defined(DB_88F6710)
				switch (mvBoardPexCapabilityGet()) {
					case 0x2:
						tempPexReg |= 0x2;
						break;
					default:
						tempPexReg |= 0x1;
						break;
				}
		
			#elif defined(DB_88F6710_PCAC)
				if (mvGppValueGet(1, (1 << 25))) {
					tempPexReg |= 0x1; 	/* Set pex GEN1 capability */
					printf("PEX - GEN1 \n");
				} else {
					tempPexReg |= 0x2; 	/* Set pex GEN2 capability */
					printf("PEX - GEN2 \n");
				}
			#elif defined(RD_88F6710_ID)
				tempPexReg |= 0x2; 	/* Set pex GEN2 capability */
			#else
				tempPexReg |= 0x1; 	/* Set pex GEN1 capability */
			#endif

			MV_REG_WRITE(PEX_CFG_DIRECT_ACCESS(pexHWInf, PEX_LINK_CAPABILITY_REG), tempPexReg);
		}



		status = mvSysPexInit(pexHWInf, pexIfMode);
		if (status == MV_ERROR) {
			printf("pci_init_board:Error calling mvPexIfInit for PEX%d.%d(%d)\n",pexHWInf/4, pexHWInf%4, pexIf);
		} else {
			if (status == MV_OK) {
#if defined(DB_PRPMC)
				mvPexLocalDevNumSet(pexIf, 0x1f);
#endif
				/* start Uboot PCI scan */
				pci_hose[pexIf].current_busno = pci_hose[pexIf].first_busno;
				pci_hose[pexIf].last_busno = 0xff;

				if (mvPexLocalBusNumSet(pexHWInf,pci_hose[pexIf].first_busno) != MV_OK) {
					printf("pci_init_board:Error calling mvPexLocalBusNumSet for pexIf %d\n",pexIf);
				}
#if defined(MV88F78X60) /* DSMP-A0 */
				/* Step 17: Speed change to target speed*/
				tempPexReg = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexHWInf, PEX_LINK_CAPABILITY_REG));
				tempPexReg &= (0xF);
				if (tempPexReg == 0x2) {
					tempReg = (MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexHWInf, PEX_LINK_CTRL_STAT_REG)) & 0xF0000) >> 16;
					/* check if the link established is GEN1 */
					if (tempReg == 0x1) {
						/* link is Gen1, check the EP capability */
						DB(printf("0x34 = 0x%x -> ", mvPexConfigRead(pexHWInf, pci_hose[pexIf].first_busno, 1, 0, 0x34)));
						addr = mvPexConfigRead(pexHWInf, pci_hose[pexIf].first_busno, 1, 0, 0x34) & 0xFF;
						if (addr == 0xff) {
							printf("PEX %d.%d(%d): Detected No Link.\n", (pexHWInf<8)?(pexHWInf/4):(pexHWInf-6) , (pexHWInf<8)?(pexHWInf%4):0, pexIf);
							continue;
						}
						while ((mvPexConfigRead(pexHWInf, pci_hose[pexIf].first_busno, 1, 0, addr) & 0xFF) != 0x10) {
							DB(printf("[0x%x] = 0x%x -> \n", addr, mvPexConfigRead(pexHWInf, pci_hose[pexIf].first_busno, 1, 0, addr)));
							addr = (mvPexConfigRead(pexHWInf, pci_hose[pexIf].first_busno, 1, 0, addr) & 0xFF00) >> 8;
						}
						DB(printf("[0x%x] = 0x%x -> ",addr +0xc, mvPexConfigRead(pexHWInf, pci_hose[pexIf].first_busno, 1, 0, addr + 0xC)));
						if ((mvPexConfigRead(pexHWInf, pci_hose[pexIf].first_busno, 1, 0, addr + 0xC) & 0xF) == 0x2) {
							/*MV_REG_BIT_SET(PEX_CFG_DIRECT_ACCESS(pexHWInf, PEX_LINK_CTRL_STAT_REG), BIT5);*/
							MV_REG_BIT_RESET(PEX_CFG_DIRECT_ACCESS(pexHWInf, PEX_LINK_CTRL_STAT_2_REG), (BIT0 | BIT1));
							MV_REG_BIT_SET(PEX_CFG_DIRECT_ACCESS(pexHWInf, PEX_LINK_CTRL_STAT_2_REG), BIT1);
							MV_REG_BIT_SET(PEX_CTRL_REG(pexHWInf), BIT10);
							udelay(10000);/* We need to wait 10ms before reading the PEX_DBG_STATUS_REG in order not to read the status of the former state*/
							DB(printf("Gen2 client!\n"));			
						} else 
							DB(printf("GEN1 client!\n"));
					}
				}

#endif
			} else {
				/* Interface with no link */
				printf("PEX %d.%d(%d): Detected No Link.\n", (pexHWInf<8)?(pexHWInf/4):(pexHWInf-6) , (pexHWInf<8)?(pexHWInf%4):0, pexIf);
				continue;
			}
		}
		
		mv_pci_bus_mode_display(pexIf);

		/* If no link on the interface it will not be scan */
		if (status == MV_NO_SUCH) {
			pci_hose[pexIf].last_busno =pci_hose[pexIf].first_busno;
			continue;
		}

#ifdef PCI_DIS_INTERFACE
		/* The disable interface will not be scan */
		if (pexIf == PCI_DIS_INTERFACE) {
			printf("***Interface is disable***\n");
			pci_hose[pexIf].last_busno =pci_hose[pexIf].first_busno;
			continue;
		}
#endif
		if (MV_OK != mvCpuIfTargetWinGet(PCI_MEM(pexHWInf, 0), &cpuAddrDecWin)) {
			printf("%s: ERR. mvCpuIfTargetWinGet failed\n", __FUNCTION__);
			return ;
		}

		rempWin.baseLow = ((cpuAddrDecWin.addrWin.baseLow & 0x0fffffff) | PCI_IF_REMAPED_MEM_BASE(pexHWInf));
		rempWin.baseHigh = 0;

		/* perform a remap for the PEX0 interface*/
		if (0xffffffff == mvCpuIfPexRemap(PCI_MEM(pexHWInf, 0),&rempWin)) {
			printf("%s:mvCpuIfPexRemap failed, %d\n",__FUNCTION__, pexHWInf);
			return;
		}

		/* PCI memory space */
		pci_set_region(pci_hose[pexIf].regions + 0,
				rempWin.baseLow, /* bus address */
				cpuAddrDecWin.addrWin.baseLow,
				cpuAddrDecWin.addrWin.size,
				PCI_REGION_MEM);

		if (MV_OK != mvCpuIfTargetWinGet(PCI_IO(pexHWInf), &cpuAddrDecWin)) {
			/* No I/O space */
			pci_hose[pexIf].region_count = 1;
		} else {
			/* PCI I/O space */
			pci_set_region(pci_hose[pexIf].regions + 1,
				    cpuAddrDecWin.addrWin.baseLow,
				    cpuAddrDecWin.addrWin.baseLow,
				    cpuAddrDecWin.addrWin.size,
				    PCI_REGION_IO);
			pci_hose[pexIf].region_count = 2;
		}
		pci_set_ops(&pci_hose[pexIf],
				pci_hose_read_config_byte_via_dword,
				pci_hose_read_config_word_via_dword,
				mv_read_config_dword,
				pci_hose_write_config_byte_via_dword,
				pci_hose_write_config_word_via_dword,
				mv_write_config_dword);

		pci_hose[pexIf].cfg_addr = (unsigned int*) pexHWInf;

		pci_hose[pexIf].config_table[1].bus = mvPexLocalBusNumGet(pexHWInf);
		pci_hose[pexIf].config_table[1].dev = mvPexLocalDevNumGet(pexHWInf);

		pci_register_hose(&pci_hose[pexIf]);

		if (pexIfMode == PCI_IF_MODE_HOST) {
			MV_U32 pciData=0,baseClassCode=0,subClassCode;

			bus_scan = 1;

			pci_hose[pexIf].last_busno = pci_hose_scan(&pci_hose[pexIf]);
			bus_scan = 0;

			pciData = mvPexConfigRead(pexHWInf,pci_hose[pexIf].first_busno,1,0, PCI_CLASS_CODE_AND_REVISION_ID);
	
			baseClassCode = (pciData & PCCRIR_BASE_CLASS_MASK) >> PCCRIR_BASE_CLASS_OFFS;
			subClassCode = 	(pciData & PCCRIR_SUB_CLASS_MASK) >> PCCRIR_SUB_CLASS_OFFS;

			if ((baseClassCode == PCI_BRIDGE_CLASS) &&
				(subClassCode == P2P_BRIDGE_SUB_CLASS_CODE)) {
				mvPexConfigWrite(pexHWInf,pci_hose[pexIf].first_busno,1,0,P2P_MEM_BASE_LIMIT,pciData);
				/* In rthe bridge : We want to open its memory and
				IO to the maximum ! after the u-boot Scan */
				/* first the IO */
				pciData  = mvPexConfigRead(pexHWInf,pci_hose[pexIf].first_busno,1,0,P2P_IO_BASE_LIMIT_SEC_STATUS);
				/* keep the secondary status */
				pciData  &= PIBLSS_SEC_STATUS_MASK;
				/* set to the maximum - 0 - 0xffff */
				pciData  |= 0xff00;
				mvPexConfigWrite(pexHWInf,pci_hose[pexIf].first_busno,1,0,P2P_IO_BASE_LIMIT_SEC_STATUS,pciData);

				/* the the Memory */
				pciData  = mvPexConfigRead(pexHWInf,pci_hose[pexIf].first_busno,1,0,P2P_MEM_BASE_LIMIT);
				/* set to the maximum - PCI_IF_REMAPED_MEM_BASE(pexIf) - 0xf000000 */
				pciData  = 0xEFF00000 | (PCI_IF_REMAPED_MEM_BASE(pexIf) >> 16);
				mvPexConfigWrite(pexHWInf,pci_hose[pexIf].first_busno,1,0,P2P_MEM_BASE_LIMIT,pciData);
			}
		} else { /* PCI_IF_MODE_HOST */
			pci_hose[pexIf].last_busno = pci_hose[pexIf].first_busno;
		}
	}
	
#if defined(MV88F78X60) 
	if ((ctrlModel == MV_78130_DEV_ID) ||
		(ctrlModel == MV_78160_DEV_ID) ||
		(ctrlModel == MV_78230_DEV_ID) ||
		(ctrlModel == MV_78260_DEV_ID) ||
		(ctrlModel == MV_78460_DEV_ID) ||
		(ctrlModel == MV_78000_DEV_ID)) {			

	/* WA for AXP-A0 shuld be removed on B0 */
	/* in case the maxLink width is greater than the Negotioated Link then the max link should be equal for the negotioated link */
		for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
			if (boardPexInfo->pexUnitCfg[pexUnit].pexCfg != PEX_BUS_DISABLED) {
				if (pexUnit < 3) /* ports 0, 1, 2 */
					pexLane = pexUnit*4;
				else /*Port 3*/
					pexLane = pexUnit*3;
				maxLinkWidth = ((MV_REG_READ(PEX_LINK_CAPABILITIES_REG(pexLane)) >> 4) & 0x3F);
				negLinkWidth = ((MV_REG_READ(PEX_LINK_CTRL_STATUS_REG(pexLane)) >> 20) & 0x3F);
				if (maxLinkWidth >  negLinkWidth) {
					tmp = MV_REG_READ(PEX_LINK_CAPABILITIES_REG(pexLane));
					tmp &= ~(0x3F << 4);
					tmp |= (negLinkWidth << 4);
					MV_REG_WRITE(PEX_LINK_CAPABILITIES_REG(pexLane),tmp);
				}

			}
		}
	}
#endif
#ifdef DB_FPGA
	MV_REG_BIT_RESET(PCI_BASE_ADDR_ENABLE_REG(0), BIT10);
#endif
}

#endif /* CONFIG_PCI */

