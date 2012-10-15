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

#include <config.h>
#include <common.h>
#include <command.h>
#include <pci.h>
#include <net.h>

#include "mvCommon.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "cpu/mvCpu.h"

#if defined(MV_INC_BOARD_NOR_FLASH)
#include "norflash/mvFlash.h"
#endif

#if defined(MV_INCLUDE_GIG_ETH)
#include "eth-phy/mvEthPhy.h"
#endif

#if defined(MV_INCLUDE_PEX)
#include "pex/mvPex.h"
#endif

#if defined(MV_INCLUDE_PDMA)
#include "pdma/mvPdma.h"
#include "mvSysPdmaApi.h"
#endif

#if defined(MV_INCLUDE_XOR)
#include "xor/mvXorRegs.h"
#include "xor/mvXor.h"
#endif

#if defined(MV_INCLUDE_PMU)
#include "pmu/mvPmuRegs.h"
#endif

#include "cntmr/mvCntmrRegs.h"

#if defined(CONFIG_CMD_BSP)

/******************************************************************************
* Category     - General
* Functionality- The commands allows the user to view the contents of the MV
*                internal registers and modify them.
* Need modifications (Yes/No) - no
*****************************************************************************/
int ir_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[] )
{
	MV_U32 regNum = 0x0, regVal, regValTmp, res;
	MV_8 regValBin[40];
	MV_8 cmd[40];
	int i,j = 0, flagm = 0;
	extern MV_8 console_buffer[];

	if( argc == 2 ) {
		regNum = simple_strtoul( argv[1], NULL, 16 );
	}
	else {
		printf( "Usage:\n%s\n", cmdtp->usage );
		return 0;
	}

	regVal = MV_REG_READ( regNum + INTER_REGS_BASE);
	regValTmp = regVal;
	printf( "Internal register 0x%x value : 0x%x\n ",regNum, regVal );
	printf( "\n    31      24        16         8         0" );
	printf( "\n     |       |         |         |         |\nOLD: " );

	for( i = 31 ; i >= 0 ; i-- ) {
		if( regValTmp > 0 ) {
			res = regValTmp % 2;
			regValTmp = (regValTmp - res) / 2;
			if( res == 0 )
				regValBin[i] = '0';
			else
				regValBin[i] = '1';
		}
		else
			regValBin[i] = '0';
	}

	for( i = 0 ; i < 32 ; i++ ) {
		printf( "%c", regValBin[i] );
		if( (((i+1) % 4) == 0) && (i > 1) && (i < 31) )
			printf( "-" );
	}

	readline( "\nNEW: " );
	strcpy(cmd, console_buffer);
	if( (cmd[0] == '0') && (cmd[1] == 'x') ) {
		regVal = simple_strtoul( cmd, NULL, 16 );
		flagm=1;
	}
	else {
		for( i = 0 ; i < 40 ; i++ ) {
			if(cmd[i] == '\0')
				break;
			if( i == 4 || i == 9 || i == 14 || i == 19 || i == 24 || i == 29 || i == 34 )
				continue;
			if( cmd[i] == '1' ) {
				regVal = regVal | (0x80000000 >> j);
				flagm = 1;
			}
			else if( cmd[i] == '0' ) {
				regVal = regVal & (~(0x80000000 >> j));
				flagm = 1;
			}
			j++;
		}
	}

	if( flagm == 1 ) {
		MV_REG_WRITE( regNum + INTER_REGS_BASE, regVal );
		printf( "\nNew value = 0x%x\n\n", MV_REG_READ(regNum +
					INTER_REGS_BASE) );
	}
	return 1;
}

U_BOOT_CMD(
	ir,      2,     1,      ir_cmd,
	"ir	- reading and changing MV internal register values.\n",
	" address\n"
	"\tDisplays the contents of the internal register in 2 forms, hex and binary.\n"
	"\tIt's possible to change the value by writing a hex value beginning with \n"
	"\t0x or by writing 0 or 1 in the required place. \n"
	"\tPressing enter without any value keeps the value unchanged.\n"
);

/******************************************************************************
* Category     - General
* Functionality- Display temperature from sensor.
* Need modifications (Yes/No) - no
*****************************************************************************/
int temperature_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 reg = 0;
	MV_U32 value = 0;

	/* init the TSEN sensor once */
	if ((MV_REG_READ(TSEN_CONF_REG) & TSEN_CONF_OTF_CALIB_MASK) == 0) {
		MV_REG_BIT_SET(TSEN_CONF_REG, TSEN_CONF_OTF_CALIB_MASK);

		reg = MV_REG_READ(TSEN_CONF_REG);
		reg &= ~(TSEN_CONF_REF_CAL_MASK);
		reg |= (0xf1 << 11);
		MV_REG_WRITE(TSEN_CONF_REG, reg);

        /* Do not start calibration sequence */
		MV_REG_BIT_RESET(TSEN_CONF_REG, TSEN_CONF_START_CALIB_MASK);

		/* Initiate Soft Reset*/
		MV_REG_BIT_SET(TSEN_CONF_REG, TSEN_CONF_SOFT_RESET_MASK);
		udelay(1000);

		/* Exit from Soft Reset*/
        MV_REG_BIT_RESET(TSEN_CONF_REG, TSEN_CONF_SOFT_RESET_MASK);
		udelay(10000);
	}

	reg = MV_REG_READ(TSEN_STATUS_REG);
	reg = (reg & TSEN_STATUS_TEMP_OUT_MASK) >> TSEN_STATUS_TEMP_OUT_OFFSET;
	value = ((3153000 - (10000 * reg)) / 13825);
	printf("Junction Temprature (Tj) = %d\n", value);

	return 1;
}

U_BOOT_CMD(
	temp,      1,     1,      temperature_cmd,
	"temp	- Display the device temprature.\n",
	" \n"
	"\tDisplay the device temprature as read from the internal sensor.\n"
);

#if defined(MV_INCLUDE_PMU)
/******************************************************************************
* Category     - General
* Functionality- Display temperature from sensor.
* Need modifications (Yes/No) - no
*****************************************************************************/
int volt_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 reg = 0, reg1 = 0;
	MV_U32 value = 0;
	int i = 0;
	char *cmd, *s;

	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strncmp(cmd, "cpu", 3) != 0 && strncmp(cmd, "core", 4) != 0)
		goto usage;

	reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL0_REG);
	reg |= PMU_TDC0_SEL_IP_MODE_MASK;
	MV_REG_WRITE(PMU_TEMP_DIOD_CTRL0_REG, PMU_TDC0_SEL_IP_MODE_MASK);

	if (strncmp(cmd, "cpu", 3) == 0) {
		reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL0_REG);
		reg &= ~(PMU_TDC0_SEL_VSEN_MASK);
		MV_REG_WRITE(PMU_TEMP_DIOD_CTRL0_REG, reg);
	}
	if (strncmp(cmd, "core", 3) == 0) {
		reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL0_REG);
		reg |= PMU_TDC0_SEL_VSEN_MASK;
		MV_REG_WRITE(PMU_TEMP_DIOD_CTRL0_REG, reg);
	}
	udelay(1000);
	/* Verify that the temperature is valid */
	reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL1_REG);
	if ((reg & PMU_TDC1_TEMP_VLID_MASK) == 0x0)
	{
		printf("Error reading voltage\n");
	}
	else
	{
		reg1 = MV_REG_READ(PMU_THERMAL_MNGR_REG);
		reg1 = ((reg1 >> 1) & 0x1FF);
		for (i = 0; i < 16; i++)
		{
			/* Read the thermal sensor looking for two successive readings that differ in LSB only */
			reg = MV_REG_READ(PMU_THERMAL_MNGR_REG);
			reg = ((reg >> 1) & 0x1FF);
			if (((reg ^ reg1) & 0x1FE) == 0x0)
				break;
			/* save the current reading for the next iteration */
			reg1 = reg;
		}
		value = ((reg + 241)*10000/39619);
		if (i == 16)
			printf("Voltage sensor is unstable: could not get two identical successive readings\n");
		else
			printf("Voltage (V) = %d.%d, Register Value = 0x%08X\n", value/100, value%100, reg);
	}
	return 0;
usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

U_BOOT_CMD(
	volt,      2,     1,      volt_cmd,
	"volt	- Display the cpu / core voltage.\n",
	"volt cpu	- display the cpu voltage\n"
	"volt core	- display the core voltage\n"
);
#endif /* #if defined(MV_INCLUDE_PMU) */

/******************************************************************************
* Functional only when using Lauterbach to load image into DRAM
* Category     - DEBUG
* Functionality- Display the array of registers the u-boot write to.
*
*****************************************************************************/
#if defined(REG_DEBUG)
int reg_arry[4096][2];
int reg_arry_index = 0;
int print_registers( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	printf("Register display\n");

	for (i=0; i < reg_arry_index; i++)
		printf("Reg no %d addr 0x%x = 0x%08x\n", i, reg_arry[i][0], reg_arry[i][1]);

	return 1;
}

U_BOOT_CMD(
	printreg,      1,     1,      print_registers,
	"printreg	- Display the register array the u-boot write to.\n",
	" \n"
	"\tDisplay the register array the u-boot write to.\n"
);
#endif

#if defined(MV_INCLUDE_GIG_ETH)
/******************************************************************************
* Category     - Etherent
* Functionality- Display PHY ports status (using SMI access).
* Need modifications (Yes/No) - No
*****************************************************************************/
int sg_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 port;
	for( port = 0 ; port < mvCtrlEthMaxPortGet(); port++ ) {

		printf( "PHY %d :\n", port );
		printf( "---------\n" );

		mvEthPhyPrintStatus((MV_U32)mvBoardPhyAddrGet(port) );

		printf("\n");
	}
	return 1;
}

U_BOOT_CMD(
	sg,      1,     1,      sg_cmd,
	"sg	- scanning the PHYs status\n",
	" \n"
	"\tScan all the Gig port PHYs and display their Duplex, Link, Speed and AN status.\n"
);
#endif /* #if defined(MV_INCLUDE_GIG_ETH) */

#if defined(MV_INCLUDE_PDMA)

/******************************************************************************
* Category     - PDMA
* Functionality- Perform a PDMA transaction
* Need modifications (Yes/No) - No
*****************************************************************************/
int mvPdma_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[] )
{
	MV_8 cmd[20];
	extern MV_8 console_buffer[];
	MV_U32 src, dst, byteCount;
	MV_PDMA_CHANNEL chan;

	/* get PDMA channel */

	/* source address */
	while(1) {
		readline( "Source Address: " );
		strcpy( cmd, console_buffer );
		src = simple_strtoul( cmd, NULL, 16 );
		if( src == 0xffffffff ) printf( "Bad address !!!\n" );
		else break;
	}

	/* desctination address */
	while(1) {
		readline( "Destination Address: " );
		strcpy(cmd, console_buffer);
		dst = simple_strtoul( cmd, NULL, 16 );
		if( dst == 0xffffffff ) printf("Bad address !!!\n");
		else break;
	}

	/* byte count */
	while(1) {
		readline( "Byte Count (up to 8KByte (0 - 0x1FFF)): " );
		strcpy( cmd, console_buffer );
		byteCount = simple_strtoul( cmd, NULL, 16 );
		if( (byteCount >= 0x2000) || (byteCount == 0) ) printf("Bad value !!!\n");
		else break;
	}

	if (mvPdmaChanAlloc(MV_PDMA_MEMORY, 0, &chan) != MV_OK) {
		printf("Error allocating PDMA channel\n");
		return 0;
	}
	/* wait for previous transfer completion */
	while(mvPdmaChannelStateGet(&chan) == MV_PDMA_CHANNEL_RUNNING);
	/* issue the transfer */
	if (mvPdmaChanTransfer(&chan, MV_PDMA_MEM_TO_MEM, src, dst, byteCount, 0) != MV_OK) {
		printf("Error with PDMA transfer\n");
	}
	/* wait for completion */
	while(mvPdmaChannelStateGet(&chan) == MV_PDMA_CHANNEL_RUNNING);
	if (mvPdmaChanFree(&chan) != MV_OK) {
		printf("Error freeing PDMA channel\n");
		return 0;
	}

	printf( "Done...\n" );
	return 1;
}

U_BOOT_CMD(
	pdma,      1,     1,      mvPdma_cmd,
	"pdma	- Perform PDMA\n",
	" \n"
	"\tPerform PDMA memory to memory transaction with the parameters given by the user.\n"
);

#endif /* #if defined(MV_INCLUDE_PDMA) */

#if defined(MV_INCLUDE_XOR)

/******************************************************************************
* Category     - DMA
* Functionality- Perform a DMA transaction using the XOR engine
* Need modifications (Yes/No) - No
*****************************************************************************/
#define XOR_TIMEOUT 0x8000000

struct xor_channel_t
{
	MV_CRC_DMA_DESC *pDescriptor;
	MV_ULONG	descPhyAddr;
};

#define XOR_CAUSE_DONE_MASK(chan) ((BIT0|BIT1) << (chan * 16) )
void xor_waiton_eng(int chan)
{
    int timeout = 0;

    while(!(MV_REG_READ(XOR_CAUSE_REG(XOR_UNIT(chan))) & XOR_CAUSE_DONE_MASK(XOR_CHAN(chan))))
    {
	if(timeout > XOR_TIMEOUT)
	    goto timeout;
	timeout++;
    }

    timeout = 0;
    while(mvXorStateGet(chan) != MV_IDLE)
    {
	if(timeout > XOR_TIMEOUT)
	    goto timeout;
	timeout++;
    }
    /* Clear int */
    MV_REG_WRITE(XOR_CAUSE_REG(XOR_UNIT(chan)), ~(XOR_CAUSE_DONE_MASK(XOR_CHAN(chan))));

timeout:
    if(timeout > XOR_TIMEOUT)
    {
	printf("ERR: XOR eng got timedout!!\n");
    }
    return;
}
int mvDma_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[] )
{
	MV_8 cmd[20], c;
	extern MV_8 console_buffer[];
	MV_U32 chan, src, dst, byteCount, ctrlLo, ctrlOvrrid;
	MV_UNIT_WIN_INFO win;
    struct xor_channel_t channel;
    MV_U8	*pVirt = (MV_U8*)mvOsIoUncachedAlignedMalloc(NULL, 32, sizeof(MV_XOR_DESC),
					    &(channel.descPhyAddr), NULL);

	MV_BOOL err;
	/* XOR channel */
	if( argc == 2 )
		chan = simple_strtoul( argv[1], NULL, 16 );
	else
		chan = 0;

	/* source address */
	while(1) {
		readline( "Physical Source Address (must be cache-line aligned): " );
		strcpy( cmd, console_buffer );
		src = simple_strtoul( cmd, NULL, 16 );
		if ((src == 0xffffffff) || (src & 0x1F)) printf( "Bad address !!!\n" );
		else break;
	}

	/* desctination address */
	while(1) {
		readline( "Physical Destination Address (must be cache-line aligned): " );
		strcpy(cmd, console_buffer);
		dst = simple_strtoul( cmd, NULL, 16 );
		if ((dst == 0xffffffff) || (dst & 0x1F)) printf("Bad address !!!\n");
		else break;
	}

	/* byte count */
	while(1) {
		readline( "Byte Count (up to (16M-1), must be a multiple of the cache-line): " );
		strcpy( cmd, console_buffer );
		byteCount = simple_strtoul( cmd, NULL, 16 );
		if( (byteCount > 0xffffff) || (byteCount == 0) ) printf("Bad value !!!\n");
		else break;
	}
	/* compose the command */
	ctrlLo = (1 << XEXCR_REG_ACC_PROTECT_OFFS);
#if defined(MV_CPU_BE)
	ctrlLo |= (XEXCR_DES_SWP_MASK);
/* 				| (1 << XEXCR_DRD_RES_SWP_OFFS)
				| (1 << XEXCR_DWR_REQ_SWP_OFFS);
*/
#endif


	/* set data transfer limit */
	while(1) {
		printf( "Source Data transfer limit(DTL):\n" );
		printf( "(2) 32  bytes at a time.\n" );
		printf( "(3) 64  bytes at a time.\n" );
		printf( "(4) 128 bytes at a time.\n" );

		c = getc();
		printf( "%c\n", c );

		err = MV_FALSE;

		switch( c ) {
			case 13: /* Enter */
				ctrlLo |= (2 << XEXCR_SRC_BURST_LIMIT_OFFS);
				printf( "32 bytes at a time.\n" );
				break;
			case '2':
				ctrlLo |= (2 << XEXCR_SRC_BURST_LIMIT_OFFS);
				break;
			case '3':
				ctrlLo |= (3 << XEXCR_SRC_BURST_LIMIT_OFFS);
				break;
			case '4':
				ctrlLo |= (4 << XEXCR_SRC_BURST_LIMIT_OFFS);
				break;
			default:
				printf( "Bad value !!!\n" );
				err = MV_TRUE;
		}

		if( !err ) break;
	}
	while(1) {
		printf( "Destination Data transfer limit(DTL):\n" );
		printf( "(2) 32  bytes at a time.\n" );
		printf( "(3) 64  bytes at a time.\n" );
		printf( "(4) 128 bytes at a time.\n" );

		c = getc();
		printf( "%c\n", c );

		err = MV_FALSE;

		switch( c ) {
			case 13: /* Enter */
				ctrlLo |= (2 << XEXCR_DST_BURST_LIMIT_OFFS);
				printf( "32 bytes at a time.\n" );
				break;
			case '2':
				ctrlLo |= (2 << XEXCR_DST_BURST_LIMIT_OFFS);
				break;
			case '3':
				ctrlLo |= (3 << XEXCR_DST_BURST_LIMIT_OFFS);
				break;
			case '4':
				ctrlLo |= (4 << XEXCR_DST_BURST_LIMIT_OFFS);
				break;
			default:
				printf( "Bad value !!!\n" );
				err = MV_TRUE;
		}

		if( !err ) break;
	}

	/* set ovveride source option */

	/* read the override control register */
	ctrlOvrrid =  MV_REG_READ(XOR_OVERRIDE_CTRL_REG(chan));
	ctrlOvrrid &= ~((1 << XEAOCR_SA0OVR_EN_OFFS) |
					(3 << XEAOCR_SA0OVRPTR_OFFS) |
					(1 << XEAOCR_DA0OVR_EN_OFFS) |
					(3 << XEAOCR_DA0OVRPTR_OFFS));

	while(1) {
		printf( "Override Source:\n" );
		printf( "(9) - no override (default)\n" );
		mvXorTargetWinRead( chan,0, &win );
		printf( "Win0 base=%08x, size=%llx, Attrib=0x%x, ID=%d\n",
					win.addrWin.baseLow, win.addrWin.size,win.attrib,   win.targetId);

		printf( "(0) - use Win0 (%s)\n",mvCtrlTargetNameGet(win.targetId));
		mvXorTargetWinRead( chan,1, &win );
		printf( "Win1 base=%08x, size=%llx, Attrib=0x%x, ID=%d\n",
					win.addrWin.baseLow, win.addrWin.size,win.attrib,   win.targetId);

		printf( "(1) - use Win1 (%s)\n",mvCtrlTargetNameGet(win.targetId));
		mvXorTargetWinRead( chan,2, &win );
		printf( "(2) - use Win2 (%s)\n",mvCtrlTargetNameGet(win.targetId));
		mvXorTargetWinRead( chan,3, &win );
		printf( "(3) - use Win3 (%s)\n",mvCtrlTargetNameGet(win.targetId));

		c = getc();
		printf( "%c\n", c );

		err = MV_FALSE;

		switch( c ) {
			case 13: /* Enter */
			case '9':
				printf( "No override\n" );
				break;
			case '0':
			case '1':
			case '2':
			case '3':
				ctrlOvrrid |= (1 << XEAOCR_SA0OVR_EN_OFFS) |
							 ((c - '0') << XEAOCR_SA0OVRPTR_OFFS);
				break;
			default:
				printf("Bad value !!!\n");
				err = MV_TRUE;
		}

		if( !err ) break;
	}

	/* set override destination option */
	while(1) {
		printf( "Override Destination:\n" );
		printf( "(9) - no override (default)\n" );
		mvXorTargetWinRead( chan,0, &win );
		printf( "(0) - use Win0 (%s)\n",mvCtrlTargetNameGet(win.targetId));
		mvXorTargetWinRead( chan,1, &win );
		printf( "(1) - use Win1 (%s)\n",mvCtrlTargetNameGet(win.targetId));
		mvXorTargetWinRead( chan,2, &win );
		printf( "(2) - use Win2 (%s)\n",mvCtrlTargetNameGet(win.targetId));
		mvXorTargetWinRead( chan,3, &win );
		printf( "(3) - use Win3 (%s)\n",mvCtrlTargetNameGet(win.targetId));

		c = getc();
		printf( "%c\n", c );

		err = MV_FALSE;

	        switch( c ) {
			case 13: /* Enter */
			case '9':
				printf( "No override\n" );
				break;
			case '0':
			case '1':
			case '2':
			case '3':
				ctrlOvrrid |= (1 << XEAOCR_DA0OVR_EN_OFFS) |
							 ((c - '0') << XEAOCR_DA0OVRPTR_OFFS);
				break;
			default:
				printf("Bad value !!!\n");
				err = MV_TRUE;
		}

		if( !err ) break;
	}


	/* wait for previous transfer completion */
	while (mvXorStateGet(chan) != MV_IDLE);

	mvXorCtrlSet( chan, ctrlLo );

	/* save the override control register */
	MV_REG_WRITE(XOR_OVERRIDE_CTRL_REG(chan), ctrlOvrrid);

	/* build the channel descriptor */
	channel.pDescriptor = (MV_CRC_DMA_DESC *)pVirt;
        channel.pDescriptor->srcAdd0 = src;
        channel.pDescriptor->srcAdd1 = 0;
	channel.pDescriptor->destAdd = dst;
        channel.pDescriptor->byteCnt = byteCount;
        channel.pDescriptor->nextDescPtr = 0;
        channel.pDescriptor->status = BIT31;
        channel.pDescriptor->descCommand = 0x0;

	/* issue the transfer */
	if (mvXorTransfer(chan, MV_DMA, channel.descPhyAddr) != MV_OK)
		printf("Error in DMA(XOR) Operation\n");

	/* wait for completion */
	xor_waiton_eng(chan);

	mvOsIoUncachedAlignedFree(NULL, sizeof(MV_XOR_DESC), channel.descPhyAddr, pVirt, 0);

	printf( "Done...\n" );
	return 1;
}

U_BOOT_CMD(
	dma,      1,     1,      mvDma_cmd,
	"dma	- Perform DMA using the XOR engine\n",
	" \n"
	"\tPerform DMA transaction with the parameters given by the user.\n"
);
#endif /* #if defined(MV_INCLUDE_XOR) */

/******************************************************************************
* Category     - Memory
* Functionality- Displays the MV's Memory map
* Need modifications (Yes/No) - Yes
*****************************************************************************/
int displayMemoryMap_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	mvCtrlAddrDecShow();
	return 1;
}

U_BOOT_CMD(
	map,      1,     1,      displayMemoryMap_cmd,
	"map	- Diasplay address decode windows\n",
	" \n"
	"\tDisplay controller address decode windows: CPU, PCI, Gig, DMA, XOR and COMM\n"
);



#if defined(MV_INC_BOARD_DDIM)

/******************************************************************************
* Category     - Memory
* Functionality- Displays the SPD information for a givven dimm
* Need modifications (Yes/No) -
*****************************************************************************/

int dimminfo_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
        int num = 0;

        if (argc > 1) {
                num = simple_strtoul (argv[1], NULL, 10);
        }

        printf("*********************** DIMM%d *****************************\n",num);

        dimmSpdPrint(num);

        printf("************************************************************\n");

        return 1;
}

U_BOOT_CMD(
        ddimm,      2,     1,      dimminfo_cmd,
        "ddimm  - Display SPD Dimm Info\n",
        " [0/1]\n"
        "\tDisplay Dimm 0/1 SPD information.\n"
);

/******************************************************************************
* Category     - Memory
* Functionality- Copy the SPD information of dimm 0 to dimm 1
* Need modifications (Yes/No) -
*****************************************************************************/

int spdcpy_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

        printf("Copy DIMM 0 SPD data into DIMM 1 SPD...");

        if (MV_OK != dimmSpdCpy())
		printf("\nDIMM SPD copy fail!\n");
	else
		printf("Done\n");

        return 1;
}

U_BOOT_CMD(
        spdcpy,      2,     1,      spdcpy_cmd,
        "spdcpy  - Copy Dimm 0 SPD to Dimm 1 SPD \n",
        ""
        ""
);
#endif /* #if defined(MV_INC_BOARD_DDIM) */

/******************************************************************************
* Functionality- Go to an address and execute the code there and return,
*    defualt address is 0x40004
*****************************************************************************/
extern void cpu_dcache_flush_all(void);
extern void cpu_icache_flush_invalidate_all(void);

void mv_go(unsigned long addr,int argc, char * const argv[])
{
	addr = MV_CACHEABLE(addr);
	char* envCacheMode = getenv("cacheMode");

	/*
	 * pass address parameter as argv[0] (aka command name),
	 * and all remaining args
	 */

    if(envCacheMode && (strcmp(envCacheMode,"write-through") == 0))
	{
		int i=0;

		/* Flush Invalidate I-Cache */
		cpu_icache_flush_invalidate_all();

		/* Drain write buffer */
		asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i));


	}
	else /*"write-back"*/
	{
		int i=0;

		/* Flush Invalidate I-Cache */
		cpu_icache_flush_invalidate_all();

		/* Drain write buffer */
		asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i));


		/* Flush invalidate D-cache */
		cpu_dcache_flush_all();
    }
	((ulong (*)(int, char *const[]))addr) (--argc, &argv[1]);

	return;
}

int g_cmd (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        ulong   addr;

	if(!enaMonExt()){
		printf("This command can be used only if enaMonExt is set!\n");
		return 0;
	}

	addr = 0x40000;

        if (argc > 1) {
		addr = simple_strtoul(argv[1], NULL, 16);
        }
	mv_go(addr,argc, argv);
	return 1;
}

U_BOOT_CMD(
	g,      CONFIG_SYS_MAXARGS,     1,      g_cmd,
        "g	- start application at cached address 'addr'(default addr 0x40000)\n",
        " addr [arg ...] \n"
	"\tStart application at address 'addr' cachable!!!(default addr 0x40004/0x240004)\n"
	"\tpassing 'arg' as arguments\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

/******************************************************************************
* Functionality- Searches for a value
*****************************************************************************/
int fi_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    MV_U32 s_address,e_address,value,i,tempData;
    MV_BOOL  error = MV_FALSE;

    if(!enaMonExt()){
	printf("This command can be used only if enaMonExt is set!\n");
	return 0;}

    if(argc == 4){
	value = simple_strtoul(argv[1], NULL, 16);
	s_address = simple_strtoul(argv[2], NULL, 16);
	e_address = simple_strtoul(argv[3], NULL, 16);
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
	return 0;
    }

    if(s_address == 0xffffffff || e_address == 0xffffffff) error = MV_TRUE;
    if(s_address%4 != 0 || e_address%4 != 0) error = MV_TRUE;
    if(s_address > e_address) error = MV_TRUE;
    if(error)
    {
	printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    for(i = s_address; i < e_address ; i+=4)
    {
        tempData = (*((volatile unsigned int *)i));
        if(tempData == value)
        {
            printf("Value: %x found at ",value);
            printf("address: %x\n",i);
            return 1;
        }
    }
    printf("Value not found!!\n");
    return 1;
}

U_BOOT_CMD(
	fi,      4,     1,      fi_cmd,
	"fi	- Find value in the memory.\n",
	" value start_address end_address\n"
	"\tSearch for a value 'value' in the memory from address 'start_address to\n"
	"\taddress 'end_address'.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

/******************************************************************************
* Functionality- Compare the memory with Value.
*****************************************************************************/
int cmpm_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    MV_U32 s_address,e_address,value,i,tempData;
    MV_BOOL  error = MV_FALSE;

    if(!enaMonExt()){
	printf("This command can be used only if enaMonExt is set!\n");
	return 0;}

    if(argc == 4){
	value = simple_strtoul(argv[1], NULL, 16);
	s_address = simple_strtoul(argv[2], NULL, 16);
	e_address = simple_strtoul(argv[3], NULL, 16);
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
	return 0;
    }

    if(s_address == 0xffffffff || e_address == 0xffffffff) error = MV_TRUE;
    if(s_address%4 != 0 || e_address%4 != 0) error = MV_TRUE;
    if(s_address > e_address) error = MV_TRUE;
    if(error)
    {
	printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    for(i = s_address; i < e_address ; i+=4)
    {
        tempData = (*((volatile unsigned int *)i));
        if(tempData != value)
        {
            printf("Value: %x found at address: %x\n",tempData,i);
        }
    }
    return 1;
}

U_BOOT_CMD(
	cmpm,      4,     1,      cmpm_cmd,
	"cmpm	- Compare Memory\n",
	" value start_address end_address.\n"
	"\tCompare the memory from address 'start_address to address 'end_address'.\n"
	"\twith value 'value'\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

#if defined(MV_INCLUDE_GIG_ETH)

#include "eth-phy/mvEthPhy.h"

int phy_read_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U16 phyReg;

	mvEthPhyRegRead(simple_strtoul( argv[1], NULL, 16 ),
	                simple_strtoul( argv[2], NULL, 16), &phyReg);

	printf ("0x%x\n", phyReg);

	return 1;
}

U_BOOT_CMD(
	phyRead,      3,     3,      phy_read_cmd,
	"phyRead	- Read Phy register\n",
	" Phy_address Phy_offset. \n"
	"\tRead the Phy register. \n"
);


int phy_write_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	mvEthPhyRegWrite(simple_strtoul( argv[1], NULL, 16 ),
					 simple_strtoul( argv[2], NULL, 16 ),
					 simple_strtoul( argv[3], NULL, 16 ));

	return 1;
}

U_BOOT_CMD(
	phyWrite,      4,     4,      phy_write_cmd,
	"phyWrite	- Write Phy register\n",
	" Phy_address Phy_offset value.\n"
	"\tWrite to the Phy register.\n"
);

#endif /* #if defined(MV_INCLUDE_GIG_ETH) */

/******************************************************************************
* Category     - DDR3 Training
* Functionality- The commands prints the results of the DDR3 Training
* Need modifications (Yes/No) - no
*****************************************************************************/
int training_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[] )
{
#define REG_SDRAM_CONFIG_ADDR					0x1400
#define REG_DDR3_RANK_CTRL_ADDR					0x15E0
#define REG_READ_DATA_SAMPLE_DELAYS_ADDR		0x1538
#define REG_READ_DATA_READY_DELAYS_ADDR			0x153C
#define REG_PHY_REGISTRY_FILE_ACCESS_ADDR		0x16A0
#define REG_PHY_OP_OFFS							31
#define REG_PHY_CS_OFFS							16
#define REG_PHY_PUP_OFFS						22

	MV_U32 uiCsEna,uiCs,uiReg,uiPup,uiPhase,uiDelay,uiDQS,uiRdRdyDly,uiRdSmplDly,uiDq;
	MV_U32 uiPupNum;

	uiCsEna = MV_REG_READ(REG_DDR3_RANK_CTRL_ADDR) & 0xF;
	printf("DDR3 Training results: \n");

	if (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & (1 << 18))
		uiPupNum = 9;
	else
		uiPupNum = 8;

	for (uiCs = 0; uiCs < 4; uiCs++) {
		if (uiCsEna & (1 << uiCs)) {
			printf("CS: %d \n", uiCs);
			for(uiPup = 0; uiPup < uiPupNum; uiPup++) {
				uiReg = (1 << REG_PHY_OP_OFFS) | (uiPup << REG_PHY_PUP_OFFS) | ((0x4*uiCs) << REG_PHY_CS_OFFS);
				MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR,uiReg);  /* 0x16A0 */

				do {
					uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) & (1 << REG_PHY_OP_OFFS));
				} while (uiReg);				/* Wait for '0' to mark the end of the transaction */

				uiReg = MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR);  /* 0x16A0 */
				uiPhase = (uiReg >> 8) & 0x7;
				uiDelay = uiReg & 0x1F;

				uiReg = (1 << REG_PHY_OP_OFFS) | (uiPup << REG_PHY_PUP_OFFS) | ((0x4*uiCs+0x1) << REG_PHY_CS_OFFS);
				MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR,uiReg);  /* 0x16A0 */

				do {
					uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) & (1 << REG_PHY_OP_OFFS)); 
				} while (uiReg);				/* Wait for '0' to mark the end of the transaction */

				uiReg = MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR);  /* 0x16A0 */
				uiDQS = (uiReg & 0x3F) - uiDelay;

				printf("Write Leveling: PUP: %d, Phase: %d, Delay: %d, DQS: %d \n",uiPup,uiPhase,uiDelay,uiDQS);
			}

			for (uiPup = 0; uiPup < uiPupNum; uiPup++) {
				uiReg = (1 << REG_PHY_OP_OFFS) | (uiPup << REG_PHY_PUP_OFFS) | ((0x4*uiCs+0x2) << REG_PHY_CS_OFFS);
				MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR,uiReg);  /* 0x16A0 */

				do {
					uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) & (1 << REG_PHY_OP_OFFS));
				} while (uiReg);				/* Wait for '0' to mark the end of the transaction */

				uiReg = MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR);  /* 0x16A0 */
				uiPhase = (uiReg >> 8) & 0x7;
				uiDelay = uiReg & 0x1F;

				uiReg = (1 << REG_PHY_OP_OFFS) | (uiPup << REG_PHY_PUP_OFFS) | ((0x4*uiCs+0x3) << REG_PHY_CS_OFFS);
				MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR,uiReg);  /* 0x16A0 */

				do {
					uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) & (1 << REG_PHY_OP_OFFS)); 
				} while (uiReg);				/* Wait for '0' to mark the end of the transaction */

				uiReg = MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR);  /* 0x16A0 */
				uiDQS = uiReg & 0x3F;

				printf("Read Leveling: PUP: %d, Phase: %d, Delay: %d, DQS: %d \n",uiPup, uiPhase, uiDelay, uiDQS);
			}

			uiRdRdyDly = ((MV_REG_READ(REG_READ_DATA_READY_DELAYS_ADDR) >> (8*uiCs)) & 0x1F);
			uiRdSmplDly = ((MV_REG_READ(REG_READ_DATA_SAMPLE_DELAYS_ADDR) >> (8*uiCs)) & 0x1F);
			printf("Read Sample Delay:\t%d\n",uiRdSmplDly);
			printf("Read Ready Delay:\t%d\n",uiRdRdyDly);

			/* PBS */
			if (uiCs == 0) {
				for (uiPup=0;uiPup<uiPupNum;uiPup++) {
					for (uiDq = 0; uiDq < 10 ;uiDq++) {
						if (uiDq == 9)
							uiDq++;
						uiReg = (1 << REG_PHY_OP_OFFS) | (uiPup << REG_PHY_PUP_OFFS) | ((0x10+uiDq) << REG_PHY_CS_OFFS);
						MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR,uiReg);  /* 0x16A0 */

						do {
							uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) & (1 << REG_PHY_OP_OFFS));
						} while (uiReg);				/* Wait for '0' to mark the end of the transaction */

						uiReg = MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR);  /* 0x16A0 */
						uiDelay = uiReg & 0x1F;

						if (uiDq == 0)
							printf("PBS TX: PUP: %d: ", uiPup);
 						if (uiDq < 8)
							printf("DQ:%d-%d,", uiDq, uiDelay);
						else if (uiDq == 8)
							printf("\nPBS TX: PUP: %d, DQS-%d \n", uiPup, uiDelay);
						else
							printf("PBS TX: PUP: %d, DM-%d \n", uiPup, uiDelay);
					}
				}
				for(uiPup=0; uiPup < uiPupNum; uiPup++) {
					for(uiDq = 0; uiDq < 9; uiDq++) {
						uiReg = (1 << REG_PHY_OP_OFFS) | (uiPup << REG_PHY_PUP_OFFS) | ((0x30+uiDq) << REG_PHY_CS_OFFS);
						MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR,uiReg);  /* 0x16A0 */

						do {
							uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) & (1 << REG_PHY_OP_OFFS));
						} while (uiReg);				/* Wait for '0' to mark the end of the transaction */

						uiReg = MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR);  /* 0x16A0 */
						uiDelay = uiReg & 0x1F;

						if (uiDq == 0)
							printf("PBS RX: PUP: %d: ", uiPup);
 						if (uiDq < 8)
							printf("DQ:%d-%d,", uiDq, uiDelay);
						if (uiDq == 8)
							printf("\nPBS RX: PUP: %d, DQS-%d \n", uiPup, uiDelay);
					}
				}
			}
		}
	}
	return 1;
}

U_BOOT_CMD(
		   training,      1,     1,      training_cmd,
	 "training	- prints the results of the DDR3 Training.\n",""
 );

#endif /* MV_TINY */

int whoAmI_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[] )
{
	printf("cpu #: %d", whoAmI());
	return 1;
}

U_BOOT_CMD(
		   whoAmI,      2,     1,      whoAmI_cmd,
	 "- reading CPU ID\n",
	""
		  );
