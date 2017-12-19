/*******************************************************************************
   Copyright (C) Marvell International Ltd. and its affiliates

   This software file (the "File") is owned and distributed by Marvell
   International Ltd. and/or its affiliates ("Marvell") under the following
   alternative licensing terms.  Once you have made an election to distribute the
   File under one of the following license alternatives, please (i) delete this
   introductory statement regarding license alternatives, (ii) delete the two
   license alternatives that you have not elected to use and (iii) preserve the
   Marvell copyright notice above.

********************************************************************************
   Marvell Commercial License Option

   If you received this File from Marvell and you have entered into a commercial
   license agreement (a "Commercial License") with Marvell, the File is licensed
   to you under the terms of the applicable Commercial License.

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
********************************************************************************
   Marvell BSD License Option

   If you received this File from Marvell, you may opt to use, redistribute and/or
   modify this File under the following licensing terms.
   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

*   Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

*   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

*   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "config_marvell.h"     /* Required to identify SOC and Board */
#include "mv_os.h"
#include "mvSysEnvLib.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"
#include "mv_seq_exec.h"
#include "printf.h"
#include "generalInit.h"

#ifdef MV88F69XX
       #include "ddr3_a39x.h"
#else
       #include "ddr3_a38x.h"
#endif

#ifdef WIN32
#define mvPrintf    printf
#endif

#ifdef CONFIG_ARMADA_38X
MV_UNIT_ID mvSysEnvSocUnitNums[MAX_UNITS_ID][MAX_DEV_ID_NUM] = {
/*                     6820    6810     6811     6828   6W22    6W23 	6825 */
/*                     A385    A380     A381/2   A388   A383    A384 	A385 */
/*                     ========= HW Flavors =========   == Virtual ==========*/
/* PEX_UNIT_ID      */ { 4,     3,       3,       4,	2,	2,	4},
/* SGMII_UNIT_ID*/     { 3,	2,       3,       3,	2,	2,	3},
/* USB3H_UNIT_ID    */ { 2,     2,       2,       2,	1,	1,	2},
/* USB3D_UNIT_ID    */ { 1,     1,       1,       1,	0,	0,	1},
/* SATA_UNIT_ID     */ { 2,     2,       2,       4,	1,	1,	2},
/* QSGMII_UNIT_ID   */ { 1,     0,       0,       1,	0,	0,	1},
/* XAUI_UNIT_ID     */ { 0,     0,       0,       0,	0,	0,	0},
/* RXAUI_UNIT_ID    */ { 0,     0,       0,       0,	0,	0,	0}
};
#else  /* if (CONFIG_ARMADA_39X) */
MV_UNIT_ID mvSysEnvSocUnitNums[MAX_UNITS_ID][MAX_DEV_ID_NUM] = {
/*                             6920    6925    6928     */
/* PEX_UNIT_ID      */         { 4,    4,      4},
/* SGMII_UNIT_ID    */         { 4,    4,      4},
/* USB3H_UNIT_ID    */         { 1,    2,      2},
/* USB3D_UNIT_ID    */         { 0,    1,      1},
/* SATA_UNIT_ID     */         { 0,    4,      4},
/* QSGMII_UNIT_ID   */         { 0,    1,      1},
/* XAUI_UNIT_ID     */         { 1,    1,      1},
/* RXAUI_UNIT_ID    */         { 1,    1,      1}
};
#endif

static MV_VOID gppRegSet(MV_U32 group, MV_U32 regOffs, MV_U32 mask, MV_U32 value)
{
	MV_U32 gppData;

	gppData = MV_REG_READ(regOffs);

	gppData &= ~mask;

	gppData |= (value & mask);

	MV_REG_WRITE(regOffs, gppData);
}
/*******************************************************************************
* mvSysEnvIoExpValSet - Set USB VBUS signal via GPIO
*
** INPUT:	gppNo - GPIO pin number.
** OUTPUT:	None.
** RETURN:	None.
*******************************************************************************/
MV_VOID mvSysEnvUsbVbusGppReset(int gppNo)
{
	MV_U32 regVal;

	/* MPP Control Register - set mpp as GPP (value = 0)*/
	regVal = MV_REG_READ(MPP_CONTROL_REG((unsigned int)(gppNo / 8)));
	regVal &= ~(0xf << ((gppNo % 8) * 4));
	MV_REG_WRITE(MPP_CONTROL_REG((unsigned int)(gppNo / 8)), regVal);

	if (gppNo < 32) {
		/* GPIO Data Out Enable Control Register - set to output */
		MV_REG_BIT_RESET(GPP_DATA_OUT_EN_REG(0), (1 << gppNo));
		/* GPIO output Data Value Register - set as low */
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(0), (1 << gppNo));
	} else {
		/* GPIO Data Out Enable Control Register - set to output */
		MV_REG_BIT_RESET(GPP_DATA_OUT_EN_REG(1), (1 << gppNo));
		/* GPIO output Data Value Register - set as low */
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(1), (1 << gppNo));
	}
}
/*******************************************************************************
* mvSysEnvIoExpValSet - Set USB VBUS signal for DB-GP board via IO expander
*
** INPUT:	None.
** OUTPUT:	None.
** RETURN:	None.
*******************************************************************************/
MV_STATUS mvSysEnvIoExpUsbVbusSet(MV_U8 value)
{
	MV_U8 readVal, configVal, offset = 7; /* Io Expander#1 (0x21), register#1, bit 7 */
	MV_TWSI_SLAVE twsiSlave;

	/* Read bit[4] in BC2 bios0 SW SatR (register 1) */
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.slaveAddr.address = MV_BOARD_IO_EXPANDER1_ADDR;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_FALSE;

	twsiSlave.offset = 7;	/* direction reg #1 (register 7) for output/input setting */
	if (MV_OK != mvTwsiRead(TWSI_CHANNEL_A3XX, &twsiSlave, &configVal, 1)) {
		mvPrintf("%s: Error: Read Configuration from IO Expander failed\n", __func__);
		return MV_ERROR;
	}

	/* Modify direction (output/input) value of requested pin */
	configVal &= ~(1 << offset);	/* output marked as 0: clean bit of old value  */

	if (mvTwsiWrite(TWSI_CHANNEL_A3XX, &twsiSlave, &configVal, 1) != MV_OK) {
		/* Write again in case the controller is busy */
		if (mvTwsiWrite(TWSI_CHANNEL_A3XX, &twsiSlave, &configVal, 1) != MV_OK) {
			mvPrintf("%s: Error: direction Write to IO Expander at 0x%x failed\n", __func__
			   , MV_BOARD_IO_EXPANDER1_ADDR);
			return MV_ERROR;
		}
	}

	twsiSlave.offset = 3; 	/* Read Output Value */
	if (MV_OK != mvTwsiRead(TWSI_CHANNEL_A3XX, &twsiSlave, &readVal, 1)) {
		mvPrintf("%s: Error: Read Configuration from IO Expander failed\n", __func__);
		return MV_ERROR;
	}

	/* Modify */
	readVal &= ~(1 << offset);	/* clean bit of old value  */
	readVal |= (value << offset);

	if (mvTwsiWrite(TWSI_CHANNEL_A3XX, &twsiSlave, &readVal, 1) != MV_OK) {
		/* Write again in case the controller is busy */
		if (mvTwsiWrite(TWSI_CHANNEL_A3XX, &twsiSlave, &readVal, 1) != MV_OK) {
		mvPrintf("%s: Error: direction Write to IO Expander at 0x%x failed\n", __func__
			   , MV_BOARD_IO_EXPANDER1_ADDR);
			return MV_ERROR;
		}
	}

	return MV_OK;
}

/*******************************************************************************
* mvSysEnvUsbVbusReset - Set USB VBUS signal before detection
*
* DESCRIPTION:
* this routine sets VBUS signal via GPIO or via I2C IO expander
*
** INPUT:	int  dev - USB Host number
** OUTPUT:	None.
** RETURN:	None.
*******************************************************************************/
MV_VOID mvSysEnvUsbVbusReset(MV_VOID)
{
	MV_32 i, gppNo;
	MV_BOARD_USB_VBUS_GPIO boardUsbVbusGpio[] = MV_BOARD_USB_VBUS_GPIO_INFO;
	MV_U32 boardIdIndex;

	/* Some of Marvell boards control VBUS signal via I2C IO expander unit */
#ifndef CONFIG_CUSTOMER_BOARD_SUPPORT
	/* if VBUS signal is Controlled via I2C IO Expander on board*/
	if (mvBoardIdGet() == DB_GP_68XX_ID) {
		mvSysEnvIoExpUsbVbusSet(0);
		return;
	}
#endif

	boardIdIndex = mvBoardIdIndexGet(mvBoardIdGet());
	if (!(sizeof(boardUsbVbusGpio)/sizeof(MV_BOARD_USB_VBUS_GPIO) > boardIdIndex)) {
		mvPrintf("\nFailed loading USB VBUS GPIO information (invalid board ID)\n");
		return;
	}

	for (i = 0; i < mvSysEnvUnitMaxNumGet(USB3H_UNIT_ID); i++) {
		gppNo = boardUsbVbusGpio[boardIdIndex].usbVbusGpio[i];
		/* if VBUS signal is Controlled via GPIO on board */
		if (gppNo != MV_ERROR) {
			mvSysEnvUsbVbusGppReset(gppNo);
			continue;
		}
	}
}

/* mvBoardDb6820AmcTwsiConfig:
 * for AMC board, to allow detection of remote PCIe GEN1/GEN2 enforcement settings:
 * 1. Enable external i2c channel for via GPIO
 * 2. Disable I2C Slave Port0 of local AMC device, to avoid bus address contention
 */
static MV_VOID mvBoardDb6820AmcTwsiConfig(MV_VOID)
{
	/* Disable I2C Slave Ports of local AMC device, to avoid bus address contention*/
	MV_REG_BIT_RESET(TWSI_CONFIG_DEBUG_REG, TWSI_DEBUG_SLAVE_PORT0_EN);
	MV_REG_BIT_RESET(TWSI_CONFIG_DEBUG_REG, TWSI_DEBUG_SLAVE_PORT1_EN);

	/* GPP configuration is required for disabling access to i2c channel 1
	   Output from GPP-44 should set to be HIGH for disabling external
	   i2c channel 1 buffer circuit
	   The entire GPPs configuration is the same as in u-boot */
	/* Set GPP Out value */
	MV_REG_WRITE(GPP_DATA_OUT_REG(0), BIT29); /* GPIO29: QS_SMI_ENA = OUT VAL High */
	MV_REG_WRITE(GPP_DATA_OUT_REG(1), 0); /* GPIO44 (BIT12) : I2C_EXT_EN = FALSE (False = OUT VAL Low) */
	MV_REG_WRITE(GPP_DATA_OUT_REG(2), 0);

	/* set GPP polarity */
	gppRegSet(0, GPP_DATA_IN_POL_REG(0), 0xFFFFFFFF, 0x0);
	gppRegSet(1, GPP_DATA_IN_POL_REG(1), 0xFFFFFFFF, 0x0);
	gppRegSet(2, GPP_DATA_IN_POL_REG(2), 0xFFFFFFFF, 0x0);

	/* Set GPP Out Enable */
	/* GPIO29: QS_SMI_ENA */
	gppRegSet(0, GPP_DATA_OUT_EN_REG(0), 0xFFFFFFFF, ~(BIT29));
	/* 44:I2C_EXT_EN, 49,50,52,53:Leds*/
	gppRegSet(1, GPP_DATA_OUT_EN_REG(1), 0xFFFFFFFF, ~(BIT12 | BIT17 | BIT18 | BIT20 | BIT21));
	gppRegSet(2, GPP_DATA_OUT_EN_REG(2), 0xFFFFFFFF, ~(0x0));
}

/*******************************************************************************
* mvBoardForcePcieGen1Get - read MSYS BC2/AC3 SatR bios0 bit[4] for PCIe GEN1/GEN2 mode
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN: TRUE if GEN1 connection is enforced
*******************************************************************************/
MV_BOOL mvBoardForcePcieGen1Get(MV_VOID)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
	MV_U8 data;

	/* TWSI init */
	slave.address = MV_BOARD_CTRL_I2C_ADDR_BC2;
	slave.type    = ADDR7_BIT;
	mvTwsiInit(TWSI_CHANNEL_A3XX, TWSI_SPEED_BC2, mvBoardTclkGet(), &slave, 0);

	/* Read bit[4] in BC2 bios0 SW SatR (register 1) */
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.slaveAddr.address = MV_BOARD_BIOS0_ADDR;
	twsiSlave.offset = 1; /* register 1, SW SatR fields */
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_FALSE;

	if (MV_OK == mvTwsiRead(TWSI_CHANNEL_A3XX, &twsiSlave, &data, 1)) {
		if ((data >> 4) & 0x1)
			return MV_TRUE;
	}

	return MV_FALSE;
}

MV_U32 gBoardId = -1;
MV_U32 mvBoardIdGet(MV_VOID)
{
	if (gBoardId != -1)
		return gBoardId;

#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
	#ifdef CONFIG_CUSTOMER_BOARD_0
		gBoardId = CUSTOMER_BOARD_ID0;
	#elif CONFIG_CUSTOMER_BOARD_1
		gBoardId = CUSTOMER_BOARD_ID1;
	#elif CONFIG_CLEARFOG_BOARD
		gBoardId = A38X_CLEARFOG_BOARD_ID;
	#endif
#else
	/* For Marvell Boards: read board ID from TWSI*/
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 boardId;

	DEBUG_INIT_FULL_S("\n### mvBoardIdGet ###\n");

	twsiSlave.slaveAddr.address = mvSysEnvi2cAddrGet();
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;

	/* Reading board id */
	DEBUG_INIT_FULL_S("mvBoardIdGet: getting board id\n");
	if (mvTwsiRead(0, &twsiSlave, &boardId, 1) != MV_OK) {
#ifdef MV88F69XX
		/* In case of A395 BOARD_DEV_TWSI_SATR is located at 0x57 and not 0x50.
		 * First check if address 0x57 (in DB board it's not valid, and RD board the read will success)
		 * If the read from 0x57 fail, will try to read from EEPROM again with address 0x50 */
		twsiSlave.slaveAddr.address = 0x50;
		if(mvTwsiRead(0, &twsiSlave, &boardId, 1) != MV_OK) {
#endif
			mvPrintf("\n\n%s: TWSI Read for Marvell Board ID failed (%x) \n", __func__, mvSysEnvi2cAddrGet());
			mvPrintf("\tUsing default board ID\n\n");
			gBoardId = MV_DEFAULT_BOARD_ID;
			return gBoardId;
#ifdef MV88F69XX
		}
#endif
	}

	DEBUG_INIT_FULL_S("boardId from HW = 0x");
	DEBUG_INIT_FULL_D(boardId, 2);
	DEBUG_INIT_FULL_S("\n");

	boardId &= 0x7; /* bits 0-2 */

	if (boardId < MV_MARVELL_BOARD_NUM && boardId >= 0)
		gBoardId = MARVELL_BOARD_ID_BASE + boardId;
	else {
		DEBUG_INIT_S("mvBoardIdGet: board id 0x");
		DEBUG_INIT_FULL_D(boardId, 8);
		DEBUG_INIT_S("is out of range. Using default board ID\n");
		gBoardId = MV_DEFAULT_BOARD_ID;
	}
#endif
	return gBoardId;
}

MV_U32 mvBoardTclkGet(MV_VOID)
{
	MV_U32 value, devId;

	value = (MV_MEMIO_LE32_READ(INTER_REGS_BASE | DEVICE_SAMPLE_AT_RESET1_REG) >> 15) & 0x1;

	switch (value) {
	case (0x0):
		/* Read HW device ID from S@R:
		 * note: avoiding reading device ID here with mvSysEnvDeviceIdGet(), since it include
		 * prints, and TWSI reads, which require Tclk value--> reading Tclk value straight from register */
		devId = MV_REG_READ(DEVICE_SAMPLE_AT_RESET1_REG);
		devId = devId >> SAR_DEV_ID_OFFS & SAR_DEV_ID_MASK;
		if (devId == MV_6811)
			return MV_BOARD_TCLK_166MHZ;	/* device 381/2 (6811/21) use 166MHz instead of 250MHz */
		else
			return MV_BOARD_TCLK_250MHZ;
	case (0x1):
		return MV_BOARD_TCLK_200MHZ;
	default:
		return MV_BOARD_TCLK_ERROR;
	}
}

MV_U32 mvBoardIdIndexGet(MV_U32 boardId)
{
/* Marvell Boards use 0x10 as base for Board ID: mask MSB to receive index for board ID*/
	return boardId & (MARVELL_BOARD_ID_MASK - 1);
}

/* Use flagTwsiInit global flag to init the Twsi once */
static int flagTwsiInit = -1;
MV_STATUS mvHwsTwsiInitWrapper(MV_VOID)
{
	MV_TWSI_ADDR slave;
	MV_U32 tClock;

	if (flagTwsiInit == -1) {
		DEBUG_INIT_FULL_S("\n### mvHwsTwsiInitWrapper ###\n");
		slave.type = ADDR7_BIT;
		slave.address = 0;
		tClock = mvBoardTclkGet();
		if (tClock == MV_BOARD_TCLK_ERROR) {
			DEBUG_INIT_FULL_S("mvHwsTwsiInitWrapper: TClk read from the board is not supported\n");
			return MV_NOT_SUPPORTED;
		}

		mvTwsiInit(0, TWSI_SPEED, tClock, &slave, 0);
		flagTwsiInit = 1;

		/* for AMC board, to allow detection of remote PCIe GEN1/GEN2 enforcement settings:
		   1. Enable external i2c channel for via GPIO
		   2. Disable I2C Slave Port0 of local AMC device, to avoid bus address contention */
		if (mvBoardIdGet() == DB_AMC_6820_ID)
			mvBoardDb6820AmcTwsiConfig();
	}
	return MV_OK;
}

MV_U32 mvSysEnvi2cAddrGet(MV_VOID)
{
#ifdef MV88F69XX
	/* In case of A395 BOARD_DEV_TWSI_SATR is located at 0x57 and not 0x50.
	 * First check if address 0x57 (in DB board it's not valid, and RD board the read will success)
	 * If the read from 0x57 fail, will try to read from EEPROM again with address 0x50
	 * gBoardId is not initialize at the first read */
	if (gBoardId == -1)
		return 0x57;
	return (mvBoardIdGet() == A39X_DB_69XX_ID ? 0x50 : 0x57);
#else
	return (mvSysEnvDeviceRevGet() == MV_88F68XX_Z1_ID ? 0x50 : 0x57);
#endif
}

/*******************************************************************************
* mvSysEnvSuspendWakeupCheck
* DESCRIPTION:	 	Reads GPIO input for suspend-wakeup indication.
* INPUT:	 	None.
* OUTPUT:
* RRETURNS:	MV_U32 indicating suspend wakeup status:
* 0 - Not supported, 1 - supported: read magic word detect wakeup, 2 - detected wakeup from GPIO.
 ***************************************************************************/
MV_SUSPEND_WAKEUP_STATUS mvSysEnvSuspendWakeupCheck(void)
{
	MV_U32 reg, boardIdIndex, gpio;
	MV_BOARD_WAKEUP_GPIO boardGpio[] = MV_BOARD_WAKEUP_GPIO_INFO;

	boardIdIndex = mvBoardIdIndexGet(mvBoardIdGet());
	if (!(sizeof(boardGpio)/sizeof(MV_BOARD_WAKEUP_GPIO) > boardIdIndex)) {
		mvPrintf("\nFailed loading Suspend-Wakeup information (invalid board ID)\n");
		return MV_SUSPEND_WAKEUP_DISABLED;
	}

	/* - Detect if Suspend-Wakeup is supported on current board
	 * - Fetch the GPIO number for wakeup status input indication */
	if (boardGpio[boardIdIndex].gpioNum == -1)
		return MV_SUSPEND_WAKEUP_DISABLED; /* suspend to RAM is not supported */
	else if (boardGpio[boardIdIndex].gpioNum == -2)
		return MV_SUSPEND_WAKEUP_ENABLED; /* suspend to RAM is supported but GPIO indication is not implemented - Skip */
	else
		gpio = boardGpio[boardIdIndex].gpioNum;

	/* Initialize MPP for GPIO (set MPP = 0x0) */
	reg = MV_REG_READ(MPP_CONTROL_REG(MPP_REG_NUM(gpio)));
	reg &= ~MPP_MASK(gpio);		/* reset MPP21 to 0x0, keep rest of MPP settings*/
	MV_REG_WRITE(MPP_CONTROL_REG(MPP_REG_NUM(gpio)), reg);

	/* Initialize GPIO as input */
	reg = MV_REG_READ(GPP_DATA_OUT_EN_REG(GPP_REG_NUM(gpio)));
	reg |= GPP_MASK(gpio);
	MV_REG_WRITE(GPP_DATA_OUT_EN_REG(GPP_REG_NUM(gpio)), reg);

	/* Check GPP for input status from PIC: 0 - regular init, 1 - suspend wakeup */
	reg = MV_REG_READ(GPP_DATA_IN_REG(GPP_REG_NUM(gpio)));

	/* if GPIO is ON: wakeup from S2RAM indication detected */
	return (reg & GPP_MASK(gpio)) ? MV_SUSPEND_WAKEUP_ENABLED_GPIO_DETECTED: MV_SUSPEND_WAKEUP_DISABLED;
}

/*******************************************************************************
* mvSysEnvCheckWakeupDramEnable
*
* DESCRIPTION: Check the magic wakeup enabled
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*       MV_SUSPEND_WAKEUP_ENABLED_MEM_DETECTED or MV_SUSPEND_WAKEUP_DISABLED
*
*******************************************************************************/
MV_SUSPEND_WAKEUP_STATUS mvSysEnvCheckWakeupDramEnable(void)
{
	int *boot_info = (int*)(BOOT_INFO_ADDR);
	int  magic_word;

	magic_word =  *(boot_info);

	if(magic_word == SUSPEND_MAGIC_WORD) {
		return MV_SUSPEND_WAKEUP_ENABLED_MEM_DETECTED;
	}
	else{
		return MV_SUSPEND_WAKEUP_DISABLED;
	}
}

/*******************************************************************************
* mvCtrlDevIdIndexGet
*
* DESCRIPTION: return SOC device index
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*        return SOC device index
*
*******************************************************************************/
MV_U32 mvSysEnvIdIndexGet(MV_U32 ctrlModel)
{
	switch (ctrlModel) {
	/* HW flavors - represented by devid bits in S@R @ 0x18600 */
	case MV_6820_DEV_ID:
		return MV_6820_INDEX;
	case MV_6810_DEV_ID:
		return MV_6810_INDEX;
	case MV_6811_DEV_ID:
		return MV_6811_INDEX;
	case MV_6828_DEV_ID:
		return MV_6828_INDEX;
	case MV_6920_DEV_ID:
		return MV_6920_INDEX;
	case MV_6925_DEV_ID:
		return MV_6925_INDEX;
	case MV_6928_DEV_ID:
		return MV_6928_INDEX;
	/* Virtual flavors - not represented by dev ID bits in S@R @ 0x18600 */
	case MV_6W22_DEV_ID: /* 6W22=A383 */
		return MV_6W22_INDEX;
	case MV_6W23_DEV_ID: /* 6W23=A384 */
		return MV_6W23_INDEX;
	default:
		return MV_6820_INDEX;
	}
}

MV_U32 mvSysEnvUnitMaxNumGet(MV_UNIT_ID unit)
{
	MV_U32 devIdIndex;

	if (unit >= MAX_UNITS_ID) {
		mvPrintf("%s: Error: Wrong unit type (%u)\n", __func__, unit);
		return 0;
	}

	devIdIndex = mvSysEnvIdIndexGet(mvSysEnvModelGet());
	return mvSysEnvSocUnitNums[unit][devIdIndex];
}

/************************************************************************************
* mvSysEnvModelGet
* DESCRIPTION:	 	Returns 16bit describing the device model (ID) as defined
*       		in Vendor ID configuration register
 ***************************************************************************/
MV_U16 mvSysEnvModelGet(MV_VOID)
{
	MV_U32	defaultCtrlId, ctrlId = MV_REG_READ(DEV_ID_REG);
	ctrlId = (ctrlId & (DEV_ID_REG_DEVICE_ID_MASK)) >> DEV_ID_REG_DEVICE_ID_OFFS;

	switch (ctrlId) {
	/* HW flavors - represented by devid bits in S@R @ 0x18600 */
	case MV_6820_DEV_ID:
	case MV_6810_DEV_ID:
	case MV_6811_DEV_ID:
	case MV_6828_DEV_ID:
	case MV_6920_DEV_ID:
	case MV_6925_DEV_ID:
	case MV_6928_DEV_ID:
	/* Virtual flavors - not represented by dev ID bits in S@R @ 0x18600 */
	case MV_6W22_DEV_ID: /* 6W22=A383 */
	case MV_6W23_DEV_ID: /* 6W23=A384 */
	case MV_6825_DEV_ID:
		return ctrlId;
	default: /*Device ID Default for A38x: 6820 , for A39x: 6920 */
	#ifdef MV88F68XX
		defaultCtrlId =  MV_6820_DEV_ID;
	#else
		defaultCtrlId = MV_6920_DEV_ID;
	#endif
		mvPrintf("%s:Error retrieving device ID (%x), using default ID = %x \n", __func__, ctrlId, defaultCtrlId);
		return defaultCtrlId;
	}
}
#ifndef CONFIG_CUSTOMER_BOARD_SUPPORT
/************************************************************************************
* mvSysEnvIsFlavourReduced
* DESCRIPTION:		Reduced Flavor A383/A384 is simulated on
*			DB-GP/DB-381/2. configured by SatR field 'flavor'
 ***************************************************************************/
MV_16 isFlavorReduced = -1;
static MV_16 mvSysEnvIsFlavourReduced(MV_VOID)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 reducedVal;

	if (isFlavorReduced != -1)
		return isFlavorReduced; /* read last value if already read from EEPROM */

	twsiSlave.slaveAddr.address = mvSysEnvi2cAddrGet();
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_TRUE;
	twsiSlave.offset = 2;		/* SW EEPROM, register 2, bit 4 */

	if (mvTwsiRead(0, &twsiSlave, &reducedVal, 1) != MV_OK) {
		mvPrintf("%s: TWSI Read of 'flavor' failed\n", __func__);
		return 0;
	}
	isFlavorReduced = !((reducedVal & SATR_DEVICE_FLAVOR_MASK) >> SATR_DEVICE_FLAVOR_OFFSET);


	return isFlavorReduced;
}
#endif
/************************************************************************************
* mvSysEnvDeviceIdGet
* DESCRIPTION:	Returns enum:
*			(0..7) index of the device model (ID)
*			(8..) index of the virtual device model (ID)
 ***************************************************************************/
MV_U32 gDevId = -1;
MV_U32 mvSysEnvDeviceIdGet(MV_VOID)
{
	char *deviceIdStr[MV_MAX_DEV_ID] = { "6810", "6820", "6811", "6828",
				"NONE", "6920", "6928", "6925", "MAX_HW_DEV_ID",
				"6W22", "6W23", "6825"}; /* 6W22=A383, 6W23=A384 */
#ifndef CONFIG_CUSTOMER_BOARD_SUPPORT
	MV_U32 boardId = mvBoardIdGet();
#endif

	if (gDevId != -1)
		return gDevId;

	/* read HW device ID value */
	gDevId = MV_REG_READ(DEVICE_SAMPLE_AT_RESET1_REG);
	gDevId = gDevId >> SAR_DEV_ID_OFFS & SAR_DEV_ID_MASK;
	/* Virtual Reduced flavor Device IDs :
		- Hardware Device ID's represented by index 0..7
		- Virtual Device ID's represented by index 8..
		- Customer boards: reduced flavor ID defined by REDUCED_FLAVOR value.
		- Marvell boards: Reduced Flavor A383/A384 is simulated on DB-GP/DB-381/2
		  only (configured by SatR field 'flavor').
	 */
#if defined(CONFIG_CUSTOMER_BOARD_SUPPORT)
#ifdef REDUCED_FLAVOR
	if (REDUCED_FLAVOR == 0x383)
		gDevId = MV_6W22;
	else if (REDUCED_FLAVOR == 0x384)
		gDevId = MV_6W23;
#endif
#else
	if (mvSysEnvIsFlavourReduced() == 1) {
		if (boardId == DB_GP_68XX_ID || boardId == DB_68XX_ID
				|| boardId == DB_AP_68XX_ID) {
			if (gDevId != MV_6810) /* simulate 6W23(A384) only on 6820(A385) or 6828(A388) */
				gDevId = MV_6W23;
			else {
				mvPrintf("%s: Error: A384 (6W23) can not run with device ", __func__);
				mvPrintf("id A380 (6810)\nto set A384,run 'SatR ");
				mvPrintf("write devid 3' OR 'SatR write devid 1'\n");
			}
		}
		else if (boardId == DB_BP_6821_ID)
			gDevId = MV_6W22;
	}
#endif
	/* Check is CESA disabled on */
	if (gDevId < MV_NONE) {
		MV_U32 cesa_en = MV_REG_READ(DEVICE_CONFIGURATION_REG0);

		cesa_en = (cesa_en >> DEV_CFG0_CESA_OFFSET) & DEV_CFG0_CESA_MASK;
		if (!cesa_en)
			gDevId = MV_6825;
	}

	mvPrintf("Detected Device ID %s\n" ,deviceIdStr[gDevId]);
	return gDevId;
}


/*******************************************************************************
* mvSysEnvDeviceRevGet - Get Marvell controller device revision number
*
* DESCRIPTION:
*       This function returns 8bit describing the device revision as defined
*       Revision ID Register.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       8bit desscribing Marvell controller revision number
*
*******************************************************************************/
MV_U8 mvSysEnvDeviceRevGet(MV_VOID)
{
    MV_U32 value;

    value = MV_REG_READ(DEV_VERSION_ID_REG);
    return ((value & (REVISON_ID_MASK)) >> REVISON_ID_OFFS);
}
/*******************************************************************************
* mvSysEnvDlbConfigPtrGet
*
* DESCRIPTION: defines pointer to to DLB COnfiguration table
*
* INPUT: none
*
* OUTPUT: pointer to DLB COnfiguration table
*
* RETURN:
*       returns pointer to DLB COnfiguration table
*
*******************************************************************************/
MV_DRAM_DLB_CONFIG  *mvSysEnvDlbConfigPtrGet(MV_VOID)
{
	return (&ddr3DlbConfigTable[0]);
}

#ifdef CONFIG_CMD_BOARDCFG
MV_BOARD_CONFIG_TYPE_INFO boardConfigTypesInfo[] = MV_EEPROM_CONFIG_INFO;
MV_U8 boardOptionsConfig[MV_CONFIG_TYPE_MAX_OPTION];

/*******************************************************************************
* mvSysEnvConfigTypeGet
*
* DESCRIPTION:
*       Return the Config type fields information for a given Config type class.
*
* INPUT:
*       configClass - The Config type field to return the information for.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOARD_CONFIG_TYPE_INFO struct with mask, offset and register number.
*
*******************************************************************************/
static MV_BOOL mvSysEnvConfigTypeGet(MV_CONFIG_TYPE_ID configClass, MV_BOARD_CONFIG_TYPE_INFO *configInfo)
{
	int i;
	MV_U32 boardId = mvBoardIdIndexGet(mvBoardIdGet());

	/* verify existence of requested config type, pull its data,
	 * and check if field is relevant to current running board */
	for (i = 0; i < MV_CONFIG_TYPE_MAX_OPTION ; i++)
		if (boardConfigTypesInfo[i].configId == configClass) {
			*configInfo = boardConfigTypesInfo[i];
			if (boardConfigTypesInfo[i].isActiveForBoard[boardId])
				return MV_TRUE;
			else
				return MV_FALSE;
		}

	DEBUG_INIT_FULL_S("mvSysEnvConfigTypeGet: Error: requested MV_CONFIG_TYPE_ID was not found\n");
	return MV_FALSE;
}

/*******************************************************************************
* mvSysEnvEpromRead -
*
* DESCRIPTION:
*	routine to read more that 1 byte from EEPROM
* INPUT:
*	byteNum - byteNumber to read
*	byteCnt - how many bytes to read/write
*	pData - pointer for 32bit
*
* OUTPUT:
*               None.
*
* RETURN:
*               reg value
*
*******************************************************************************/
static MV_STATUS mvSysEnvEpromRead(MV_U8 byteNum, MV_U8 *pData, MV_U32 byteCnt, MV_U8 addr)
{
	MV_TWSI_SLAVE twsiSlave;

	twsiSlave.slaveAddr.address = addr;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_TRUE;
	twsiSlave.offset = byteNum;

	if (mvTwsiRead(MV_BOARD_CONFIG_EEPROM_OFFSET, &twsiSlave,
		pData, byteCnt) != MV_OK) {
		/* Read again in case the controler is busy */
		if (mvTwsiRead(MV_BOARD_CONFIG_EEPROM_OFFSET, &twsiSlave,
			pData, byteCnt) != MV_OK) {
			DEBUG_INIT_S("mvSysEnvEepromRead: Error: Read from EEPROM failed\n");
			return MV_ERROR;
		}
	}
	return MV_OK;
}

/*******************************************************************************
* mvSysEnvEpromWrite -
*
* DESCRIPTION:
*       routine to write more that 1 byte to EEPROM
* INPUT:
*       byteNum - byteNumber to write
*       byteCnt - how many bytes to read/write
*       pData - pointer for 32bit
*
* OUTPUT:
*               None.
*
* RETURN:
*               returns MV_OK on success, MV_ERROR on failure.
*
*******************************************************************************/
static MV_STATUS mvSysEnvEpromWrite(MV_U8 byteNum, MV_U8 *pData, MV_U32 byteCnt, MV_U8 addr)
{
	MV_TWSI_SLAVE twsiSlave;

	twsiSlave.slaveAddr.address = addr;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.moreThen256 = MV_TRUE;
	twsiSlave.offset = byteNum;

	if (mvTwsiWrite(MV_BOARD_CONFIG_EEPROM_OFFSET, &twsiSlave,
		pData, byteCnt) != MV_OK) {
		/* Write again in case the controler is busy */
		if (mvTwsiWrite(MV_BOARD_CONFIG_EEPROM_OFFSET, &twsiSlave,
			pData, byteCnt) != MV_OK) {
			DEBUG_INIT_S("mvSysEnvEpromWrite: Error: write to EEPROM failed\n");
			return MV_ERROR;
		}
	}

	return MV_OK;
}

/*******************************************************************************
* mvSysEnvTwsiProbe - Probe the given I2C chip address
*
* DESCRIPTION:
*
* INPUT:
*       chip - i2c chip address to probe
*
* OUTPUT:
*       None.
*
* RETURN:
*       Returns MV_TRUE if a chip responded, MV_FALSE on failure
*
*******************************************************************************/
static MV_STATUS mvSysEnvTwsiProbe(MV_U32 chip)
{
	MV_TWSI_ADDR eepromAddress;

	/* TWSI init */
	mvHwsTwsiInitWrapper();

	if (mvTwsiStartBitSet(0)) {
		DEBUG_INIT_S("mvSysEnvTwsiProbe: Transaction start failed\n");
		mvTwsiStopBitSet(0);
		return MV_FALSE;
	}

	eepromAddress.type = ADDR7_BIT;
	eepromAddress.address = chip;

	if (mvTwsiAddrSet(0, &eepromAddress, MV_TWSI_WRITE)) {
		DEBUG_INIT_S("mvSysEnvTwsiProbe: Failed to set slave address\n");
		mvTwsiStopBitSet(0);
		return MV_FALSE;
	}
	DEBUG_INIT_FULL_S("address 0x");
	DEBUG_INIT_FULL_D(chip, 2);
	DEBUG_INIT_FULL_S(" returned 0x");
	DEBUG_INIT_FULL_D(MV_REG_READ(TWSI_STATUS_BAUDE_RATE_REG(0)), 8);
	DEBUG_INIT_FULL_S("\n");

	/* issue a stop bit */
	mvTwsiStopBitSet(0);

	DEBUG_INIT_FULL_S("mvSysEnvTwsiProbe: successful I2C probe\n");
	return MV_TRUE; /* successful completion */
}

/*******************************************************************************
* mvSysEnvIsEepromEnabled - read EEPROM and verify if EEPROM exists
*
* DESCRIPTION:
*       This function returns MV_TRUE if board configuration EEPROM exists on board.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOOL :  MV_TRUE if EEPROM exists, else return MV_FALSE.
*
*******************************************************************************/
static MV_BOOL mvSysEnvIsEepromEnabled(void)
{
	MV_U8 addr = mvSysEnvi2cAddrGet();

	DEBUG_INIT_FULL_S("mvSysEnvIsEepromEnabled probing for i2c chip 0x");
	DEBUG_INIT_FULL_D(addr, 2);
	DEBUG_INIT_FULL_S("\n");

	if (mvSysEnvTwsiProbe((MV_U32)addr) == MV_TRUE)
		return MV_TRUE;  /* EEPROM enabled */
	else
		return MV_FALSE; /* EEPROM disabled */
}

/*******************************************************************************
* mvSysEnvEpromReset - reset EEPROM to default content.
*
* DESCRIPTION:
*       This function resets the EEPROM content. used for first board boot.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK if EEPROM is reset, else return MV_FAIL.
*
*******************************************************************************/
static MV_STATUS mvSysEnvEpromReset(void)
{
	MV_U8 data[MV_BOARD_CONFIG_MAX_BYTE_COUNT] = MV_BOARD_CONFIG_DEFAULT_VALUE;
	MV_U8 pattern[MV_BOARD_CONFIG_PATTERN_BYTES_NUM] = EEPROM_VERIFICATION_PATTERN;

#ifdef MV_BOARD_CONFIG_SKIP_FIRST_BYTE
	/* Start from the second element in data (first element is the boardID, and we should not change it) */
	if (mvSysEnvEpromWrite(MV_BOARD_CONFIG_EEPROM_OFFSET + 1, data + 1, MV_BOARD_CONFIG_MAX_BYTE_COUNT - 1,
#else
	if (mvSysEnvEpromWrite(MV_BOARD_CONFIG_EEPROM_OFFSET, data, MV_BOARD_CONFIG_MAX_BYTE_COUNT,
#endif
		mvSysEnvi2cAddrGet()) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEpromReset: Error: Write default configuration to EEPROM failed\n");
		return MV_FAIL;
	}

	/* write magic pattern to the EEPROM */
	if (mvSysEnvEpromWrite(MV_BOARD_CONFIG_PATTERN_OFFSET, pattern, MV_BOARD_CONFIG_PATTERN_BYTES_NUM,
		mvSysEnvi2cAddrGet()) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEpromReset: failed to write magic pattern to EEPROM\n");
		return MV_FAIL;
	}

	return MV_OK;
}

/*******************************************************************************
* mvSysEnvEepromInit - Verify if the EEPROM have been initialized
*
* DESCRIPTION:
*       Verify if the EEPROM have been initialized:
*       EEPROM expected mapping:
*       [0x0-0x7](64bits) - board configuration section 1
*       [0x8-0xF](64bits) - board configuration section 2
* INPUT:
*       None
*
* OUTPUT:
*       None.
*
* RETURN:
*       Returns MV_TRUE if a chip responded, MV_FALSE on failure
*
*******************************************************************************/
static MV_STATUS mvSysEnvEepromInit(void)
{
	MV_U8 data[MV_BOARD_CONFIG_MAX_BYTE_COUNT] = {0};
	MV_U8 pattern[MV_BOARD_CONFIG_PATTERN_BYTES_NUM] = EEPROM_VERIFICATION_PATTERN;
	MV_U32 validCount = 0;
	MV_U32 i;
	MV_BOARD_CONFIG_TYPE_INFO enableConfigInfo, validConfigInfo;

	/* check if EEPROM is enabled */
	if (mvSysEnvIsEepromEnabled() != MV_TRUE) {
		DEBUG_INIT_S("mvSysEnvEepromInit: EEPROM doesn't exists on board\n");
		return MV_FAIL;
	}

	/* check for EEPROM pattern to see if this is the board's first boot */
	if (mvSysEnvEpromRead(MV_BOARD_CONFIG_PATTERN_OFFSET, data,
			MV_BOARD_CONFIG_PATTERN_BYTES_NUM, mvSysEnvi2cAddrGet()) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEepromInit: Error: read pattern from EEPROM failed.\n");
		return MV_FAIL;
	}

	for (i = 0; i < MV_BOARD_CONFIG_PATTERN_BYTES_NUM; i++) {
		/* if the magic pattern was not found, reset the EEPROM to default configuration. */
		if (data[i] != pattern[i]) {
			DEBUG_INIT_S("mvSysEnvEepromInit: First init of the board. loadind default configuration\n");
			if (mvSysEnvEpromReset() != MV_OK) {
				DEBUG_INIT_S("mvSysEnvEepromInit: Error: failed resetting EEPROM\n");
				return MV_FAIL;
			}
			return MV_OK;
		}
	}

	if (mvSysEnvConfigTypeGet(MV_CONFIG_BOARDCFG_EN, &enableConfigInfo) != MV_TRUE)
		return MV_FAIL;

	/* check if board auto configuration is enabled */
	if (mvSysEnvEpromRead(enableConfigInfo.byteNum, data, 1, mvSysEnvi2cAddrGet()) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEepromInit: Error: read data from EEPROM failed.\n");
		return MV_FAIL;
	}

	/* if board auto configuration is disabled, return MV_ERROR to load default configuration */
	if ((data[0] & enableConfigInfo.mask) >> enableConfigInfo.offset == 0x0) {
		DEBUG_INIT_S("mvSysEnvEepromInit: board auto configuration is not enabled.\n");
		return MV_FAIL;
	}

	if (mvSysEnvConfigTypeGet(MV_CONFIG_BOARDCFG_VALID, &validConfigInfo) != MV_TRUE)
		return MV_FAIL;

	/* board configuration is enabled.*/
	/* bits 0-4 in offset 13 in the EEPROM are used as counters for board configuration validation.
	   each load of the board by EEPROM configuration, the counter is incremented, and when
	   it reaches 10 times, the configuration is set to default */
	if (mvSysEnvEpromRead(validConfigInfo.byteNum, data, 1, mvSysEnvi2cAddrGet()) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEepromInit: Error: read data from EEPROM failed.");
		return MV_FAIL;
	}

	validCount = (data[0] & validConfigInfo.mask) >> validConfigInfo.offset;

	/* if the valid counter has reached 10, reset the counter, disable the board auto configuration
	   enable bit, and return MV_FAIL to load default configuration. */
	if (validCount == 10) {
		DEBUG_INIT_S("mvSysEnvEepromInit: board configuration from the EEPROM is not valid\n");

		/* reset the valid counter to 0 */
		data[0] &= ~validConfigInfo.mask;
		if (mvSysEnvEpromWrite(validConfigInfo.byteNum, data, 1, mvSysEnvi2cAddrGet()) != MV_OK)
			DEBUG_INIT_S("mvSysEnvEepromInit: write data to EEPROM failed.\n");

		if (mvSysEnvEpromRead(enableConfigInfo.byteNum, data, 1, mvSysEnvi2cAddrGet()) != MV_OK) {
			DEBUG_INIT_S("mvSysEnvEepromInit: Error: read data from EEPROM failed.");
			return MV_FAIL;
		}

		/* disable the board auto config */
		data[0] &= ~enableConfigInfo.mask;
		if (mvSysEnvEpromWrite(enableConfigInfo.byteNum, data, 1, mvSysEnvi2cAddrGet()) != MV_OK)
			DEBUG_INIT_S("mvSysEnvEepromInit: write data to EEPROM failed.\n");

		return MV_FAIL;
	}

	/* incremeant the valid counter by 1, and return MV_OK to load configuration from the EEPROM */
	validCount++;
	data[0] &= ~validConfigInfo.mask;
	data[0] |= (validCount) << validConfigInfo.offset;

	if (mvSysEnvEpromWrite(validConfigInfo.byteNum, data, 1, mvSysEnvi2cAddrGet()) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvEepromInit: write data to EEPROM failed.\n");
		return MV_FAIL;
	}

	return MV_OK;
}

/*******************************************************************************
* mvSysEnvConfigInit
*
* DESCRIPTION: Initialize EEPROM configuration
*       1. initialize all board configuration fields
*       3. read relevant board configuration (using TWSI/EEPROM access)
*
* INPUT:  None
*
* OUTPUT: None
*
* RETURN:
*       MV_OK if initialize pass, MV_INIT_ERROR if read board topology fail,
*
*******************************************************************************/
static int flagConfigInit = -1;
MV_STATUS mvSysEnvConfigInit(void)
{
	MV_U8 i, readValue;
	MV_U8 defaultVal[MV_BOARD_CONFIG_MAX_BYTE_COUNT] = MV_BOARD_CONFIG_DEFAULT_VALUE;
	MV_U8 configVal[MV_BOARD_CONFIG_MAX_BYTE_COUNT] = MV_BOARD_CONFIG_DEFAULT_VALUE;
	MV_BOARD_CONFIG_TYPE_INFO configInfo;
	MV_BOOL readSuccess = MV_FALSE, readFlagError = MV_TRUE;
	MV_STATUS res;

	if (flagConfigInit != -1)
		return MV_OK;

	flagConfigInit = 1;

	/* Read Board Configuration*/
	res = mvSysEnvEepromInit();

	if (res == MV_OK) {
		/* Read configuration data: 1st 8 bytes in  EEPROM */
		if (mvSysEnvEpromRead(MV_BOARD_CONFIG_EEPROM_OFFSET, configVal,
				MV_BOARD_CONFIG_MAX_BYTE_COUNT, mvSysEnvi2cAddrGet()) != MV_OK) {
			DEBUG_INIT_S("mvSysEnvConfigInit: Error: Read board configuration from EEPROM failed\n");
			readFlagError = MV_FALSE;
		}
	}


	/* if mvSysEnvEepromInit or the EEPROM reading fails, load default configuration. */
	if ((res != MV_OK) || (readFlagError == MV_FALSE)) {
		DEBUG_INIT_S("mvSysEnvConfigInit: Setting default configurations\n");
		for (i = 0; i < MV_BOARD_CONFIG_MAX_BYTE_COUNT; i++)
			configVal[i] = defaultVal[i];
	}

	/* Save values Locally in configVal[] */
	for (i = 0; i < MV_CONFIG_TYPE_MAX_OPTION; i++) {
		/* Get board configuration field information (Mask, offset, etc..) */
		if (mvSysEnvConfigTypeGet(i, &configInfo) != MV_TRUE)
			continue;

		readValue = (configVal[configInfo.byteNum] & configInfo.mask) >> configInfo.offset;

		boardOptionsConfig[configInfo.configId] = readValue;
		readSuccess = MV_TRUE;
	}

	if (readSuccess == MV_FALSE) {
		DEBUG_INIT_FULL_S("mvSysEnvConfigInit: Error: Read board configuration from EEPROM failed\n");
		return MV_INIT_ERROR;
	}

	return MV_OK;
}

/*******************************************************************************
* mvSysEnvConfigGet
*
* DESCRIPTION: Read Board configuration Field
*
* INPUT: configField - Field description enum
*
* OUTPUT: None
*
* RETURN:
*       if field is valid - returns requested Board configuration field value
*
*******************************************************************************/
MV_U32 mvSysEnvConfigGet(MV_CONFIG_TYPE_ID configField)
{
	MV_BOARD_CONFIG_TYPE_INFO configInfo;

	if (configField >=MV_CONFIG_TYPE_MAX_OPTION ||
		mvSysEnvConfigTypeGet(configField, &configInfo) != MV_TRUE) {
		DEBUG_INIT_S("mvSysEnvConfigGet: Error: Requested board config is invalid for this board\n");
		return MV_ERROR;
	}

	return boardOptionsConfig[configField];
}

#endif /* CONFIG_CMD_BOARDCFG */

#ifdef MV_DDR_TOPOLOGY_UPDATE_FROM_TWSI
/*******************************************************************************
* mvSysEnvGetTopologyUpdateInfo
*
* DESCRIPTION: Read TWSI fields to update DDR topology structure
*
* INPUT: None
*
* OUTPUT: None, 0 means no topology update
*
* RETURN:
*       Bit mask of changes topology features
*
*******************************************************************************/
#ifdef MV88F69XX
MV_U32 mvSysEnvGetTopologyUpdateInfo(MV_TOPOLOGY_UPDATE_INFO *topologyUpdateInfo)
{
	/*Set 16/32 bit configuration*/
	topologyUpdateInfo->mvUpdateWidth = MV_TRUE;
	topologyUpdateInfo->mvWidth = MV_TOPOLOGY_UPDATE_WIDTH_32BIT;

	if( 1 == mvSysEnvConfigGet(MV_CONFIG_DDR_BUSWIDTH) ){
		/*16bit*/
		topologyUpdateInfo->mvWidth = MV_TOPOLOGY_UPDATE_WIDTH_16BIT;
	}
	else{
		/*32bit*/
		topologyUpdateInfo->mvWidth = MV_TOPOLOGY_UPDATE_WIDTH_32BIT;
	}

	/*Set ECC/no ECC bit configuration*/
	topologyUpdateInfo->mvUpdateECC = MV_TRUE;
	if( 0 == mvSysEnvConfigGet(MV_CONFIG_DDR_ECC_EN) ){
		/*NO ECC*/
		topologyUpdateInfo->mvECC = MV_TOPOLOGY_UPDATE_ECC_OFF;
	}
	else{
		/*ECC*/
		topologyUpdateInfo->mvECC = MV_TOPOLOGY_UPDATE_ECC_ON;
	}

	topologyUpdateInfo->mvUpdateECCPup3Mode = MV_TRUE;
	topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP4;

	return MV_OK;
}
#else /*MV88F68XX*/
MV_U32 mvSysEnvGetTopologyUpdateInfo(MV_TOPOLOGY_UPDATE_INFO *topologyUpdateInfo)
{
	MV_U8	configVal;
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 boardEccModeArray[A38X_MV_MAX_MARVELL_BOARD_ID-A38X_MARVELL_BOARD_ID_BASE][5] = MV_TOPOLOGY_UPDATE;
	MV_U8 boardId = mvBoardIdGet();

	boardId = mvBoardIdIndexGet(boardId);

	/*Fix the topology for A380 by SatR values*/
	twsiSlave.slaveAddr.address = mvSysEnvi2cAddrGet();
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;

	/* Reading DDR topology configuration from EEPROM */
	if (mvTwsiRead(0, &twsiSlave, &configVal, 1) != MV_OK) {
		DEBUG_INIT_S("mvSysEnvGetTopologyUpdateInfo: TWSI Read failed\n");
		return 0;
	}

	/*Set 16/32 bit configuration*/
	if( ( 0 == (configVal & DDR_SATR_CONFIG_MASK_WIDTH) ) ||
		(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_32BIT] == 0) ){
		/*16bit by SatR of 32bit mode not supported for the board*/
		if( (boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT] != 0)){
			topologyUpdateInfo->mvUpdateWidth = MV_TRUE;
			topologyUpdateInfo->mvWidth = MV_TOPOLOGY_UPDATE_WIDTH_16BIT;
		}
	}
	else{
		/*32bit*/
		if( (boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_32BIT] !=0 )){
			topologyUpdateInfo->mvUpdateWidth = MV_TRUE;
			topologyUpdateInfo->mvWidth = MV_TOPOLOGY_UPDATE_WIDTH_32BIT;
		}
	}

	/*Set ECC/no ECC bit configuration*/
	if( 0 == (configVal & DDR_SATR_CONFIG_MASK_ECC) ){
		/*NO ECC*/
		topologyUpdateInfo->mvUpdateECC = MV_TRUE;
		topologyUpdateInfo->mvECC = MV_TOPOLOGY_UPDATE_ECC_OFF;
	}
	else{
		/*ECC*/
		if( (boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_32BIT_ECC] !=0) ||
			(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT_ECC] !=0) ||
			(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT_ECC_PUP3] !=0) ){
				topologyUpdateInfo->mvUpdateECC = MV_TRUE;
				topologyUpdateInfo->mvECC = MV_TOPOLOGY_UPDATE_ECC_ON;
		}
	}

	/*Set ECC pup bit configuration*/
	if( 0 == (configVal & DDR_SATR_CONFIG_MASK_ECC_PUP) ){
		/*PUP3*/
		/*Check if PUP3 configuration allowed, if not - force Pup4 with warning message*/
		if(	(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT_ECC_PUP3] != 0)){
			if(topologyUpdateInfo->mvWidth == MV_TOPOLOGY_UPDATE_WIDTH_16BIT){
				topologyUpdateInfo->mvUpdateECCPup3Mode = MV_TRUE;
				topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP3;
			}
			else{
				if( (boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_32BIT_ECC] !=0 ) ){
					mvPrintf("DDR Topology Update: ECC PUP3 not valid for 32bit mode, force ECC in PUP4\n");
					topologyUpdateInfo->mvUpdateECCPup3Mode = MV_TRUE;
					topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP4;
				}
			}
		}
		else{
			if(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT_ECC] != 0 ){
				mvPrintf("DDR Topology Update: ECC on PUP3 not supported, force ECC on PUP4\n");
				topologyUpdateInfo->mvUpdateECCPup3Mode = MV_TRUE;
				topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP4;
			}
		}
	}
	else{
		/*PUP4*/
		if( (boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_32BIT_ECC] !=0 ) ||
			(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_16BIT_ECC] !=0 )){
			topologyUpdateInfo->mvUpdateECCPup3Mode = MV_TRUE;
			topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP4;
		}
	}

	/*Check for forbidden ECC mode,
		if by default width and pup selection set 32bit ECC mode and this mode not supported for the board - config 16bit with ECC on PUP3*/
	if( (topologyUpdateInfo->mvECC == MV_TOPOLOGY_UPDATE_ECC_ON) &&(topologyUpdateInfo->mvWidth == MV_TOPOLOGY_UPDATE_WIDTH_32BIT)){
		if(boardEccModeArray[boardId][MV_TOPOLOGY_UPDATE_32BIT_ECC] == 0){
			mvPrintf("DDR Topology Update: 32bit mode with ECC not allowed on this board, forced  16bit with ECC on PUP3\n");
			topologyUpdateInfo->mvWidth = MV_TOPOLOGY_UPDATE_WIDTH_16BIT;
			topologyUpdateInfo->mvUpdateECCPup3Mode = MV_TRUE;
			topologyUpdateInfo->mvECCPupModeOffset = MV_TOPOLOGY_UPDATE_ECC_OFFSET_PUP3;
		}
	}

	return MV_OK;
}
#endif/*MV88F68XX*/
#endif /*MV_DDR_TOPOLOGY_UPDATE_FROM_TWSI*/

/*******************************************************************************
* mvSysEnvGetCSEnaFromReg
*
* DESCRIPTION: Get bit mask of enabled CS
*
* INPUT: None
*
* OUTPUT: None
*
* RETURN:
*       Bit mask of enabled CS, 1 if only CS0 enabled, 3 if both CS0 and CS1 enabled
*
*******************************************************************************/
MV_U32 mvSysEnvGetCSEnaFromReg(void)
{
	return MV_REG_READ(REG_DDR3_RANK_CTRL_ADDR) & REG_DDR3_RANK_CTRL_CS_ENA_MASK;
}

/* mvSysEnvTimerIsRefClk25Mhz:
 * A38x/A39x support 25Mhz as ref.clock for timer
 */
MV_BOOL mvSysEnvTimerIsRefClk25Mhz(void)
{
	return MV_TRUE;
}
/*******************************************************************************
* Description: Read GEN1 Force Settings from SW EEPROM
* Bit mapping for field from SatR ForceGen1: SW EEPROM (0x57), reg#3, bit2
********************************************************************************/
MV_STATUS mvSysEnvReadPcieGenSetting(MV_BOOL *isForceGen1)
{
#ifdef MV88F68XX
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 data = 0;
	/* Read SatR  */
	twsiSlave.slaveAddr.address = mvSysEnvi2cAddrGet();
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0x3;
	twsiSlave.moreThen256 = MV_TRUE;
	if (mvTwsiRead(0, &twsiSlave, &data, 1) == MV_OK) {
		data = data & 0x4;
		if (!data)
			*isForceGen1 = MV_TRUE;
		else
			*isForceGen1 = MV_FALSE;
		return MV_OK;
	}
	mvPrintf("%s: TWSI Read of SatR field 'isForceGen1 failed\n", __func__);
	return MV_ERROR;
#else
	*isForceGen1 = MV_FALSE;
	return MV_OK;
#endif
}

