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
#include <common.h>
#if defined(CONFIG_CMD_I2C)
//#include <mpc8xx.h>
#include <malloc.h>
#include <config.h>
#include "twsi/mvTwsi.h"
#include "mvCtrlEnvSpec.h"

//#define MAX_I2C_RETRYS	    10
#define I2C_DELAY	    300  /* Should be at least the # of MHz of Tclk */

#undef	DEBUG_I2C
#ifdef DEBUG_I2C
#define DP(x) x
#else
#define DP(x)
#endif

MV_U8 i2c_current_bus = 0;

/*
 * Initialization, must be called once on start up, may be called
 * repeatedly to change the speed and slave addresses.
 */
void i2c_init(int speed, int slaveaddr)
{
	MV_TWSI_ADDR slave;
	slave.type = ADDR7_BIT;
	slave.address = slaveaddr;
	mvTwsiInit(i2c_current_bus, speed, CONFIG_SYS_TCLK, &slave, 0);
}
/*
 * Read/Write interface:
 *   chip:    I2C chip address, range 0..127
 *   addr:    Memory (register) address within the chip
 *   alen:    Number of bytes to use for addr (typically 1, 2 for larger
 *              memories, 0 for register type devices with only one
 *              register)
 *   buffer:  Where to read/write the data
 *   len:     How many bytes to read/write
 *
 *   Returns: 0 on success, not 0 on failure
 */
int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	MV_TWSI_SLAVE twsiSlave;
	DP(puts("i2c_read\n"));

	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.slaveAddr.address = chip;
	if(alen != 0){
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = addr;
		if(alen == 2)
		{
			twsiSlave.moreThen256 = MV_TRUE;
		}
		else
		{
			twsiSlave.moreThen256 = MV_FALSE;
		}
	}
	i2c_init(CONFIG_SYS_I2C_SPEED,0); /* set the i2c frequency */
	return mvTwsiRead (i2c_current_bus, &twsiSlave, buffer, len);
}
int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	MV_TWSI_SLAVE twsiSlave;
	DP(puts("i2c_write\n"));

	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.slaveAddr.address = chip;
	if(alen != 0){
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = addr;
		if(alen == 2)
		{
			twsiSlave.moreThen256 = MV_TRUE;
		}
		else
		{
			twsiSlave.moreThen256 = MV_FALSE;
		}
	}
	i2c_init(CONFIG_SYS_I2C_SPEED,0); /* set the i2c frequency */
	return mvTwsiWrite (i2c_current_bus, &twsiSlave, buffer, len);
}
/*
 * Probe the given I2C chip address.  Returns 0 if a chip responded,
 * not 0 on failure.
 */
int i2c_probe(uchar chip)
{
	/* We are just looking for an <ACK> back. */
	/* To see if the device/chip is there */

	MV_TWSI_ADDR eepromAddress;

	unsigned int status = 0;
	//unsigned int i2cFreq = CONFIG_SYS_I2C_SPEED;
	//MV_U8 i2c_current_bus;

	DP(puts("i2c_probe\n"));

	//for (i2c_current_bus = 0; i2c_current_bus < MV_TWSI_MAX_CHAN; i2c_current_bus++) {

	i2c_init(CONFIG_SYS_I2C_SPEED,0); /* set the i2c frequency */

	status = mvTwsiStartBitSet(i2c_current_bus);

	if (status) {
		DP(printf("Transaction start failed: 0x%02x\n", status));
		mvTwsiStopBitSet(i2c_current_bus);
		return (int)status;
	}

	eepromAddress.type = ADDR7_BIT;
	eepromAddress.address = chip;

	status = mvTwsiAddrSet(i2c_current_bus, &eepromAddress, MV_TWSI_WRITE); /* send the slave address */
	if (status) {
		DP(printf("Failed to set slave address: 0x%02x\n", status));
		mvTwsiStopBitSet(i2c_current_bus);
		return (int)status;
	}
	DP(printf("address %#x returned %#x\n",chip,MV_REG_READ(TWSI_STATUS_BAUDE_RATE_REG(i2c_current_bus))));

	/* issue a stop bit */
	mvTwsiStopBitSet(i2c_current_bus);
	//}

	DP(printf("*** successful completion \n"));
	return 0; /* successful completion */
}
/*
 * i2c_set_bus_num:
 *
 *  Change the active I2C bus.  Subsequent read/write calls will
 *  go to this one.
 *
 *	bus - bus index, zero based
 *
 *	Returns: 0 on success, not 0 on failure
 *
 */
int i2c_set_bus_num(unsigned int bus)
{
	if (bus >= CONFIG_SYS_MAX_I2C_BUS)
		return -1;

	i2c_current_bus = bus;
	return 0;
}
/*
 * i2c_get_bus_num:
 *
 *  Returns index of currently active I2C bus.  Zero-based.
 */
unsigned int i2c_get_bus_num(void)
{
	return i2c_current_bus;
}
/*
 * i2c_set_bus_speed:
 *
 *  Change the speed of the active I2C bus
 *
 *	speed - bus speed in Hz
 *
 *	Returns: 0 on success, not 0 on failure
 *
 */
int i2c_set_bus_speed(unsigned int speed)
{
	printf("Speed change not supported\n");
	return 0;
}
/*
 * i2c_get_bus_speed:
 *
 *  Returns speed of currently active I2C bus in Hz
 */
unsigned int i2c_get_bus_speed(void)
{
	return CONFIG_SYS_I2C_SPEED;
}
#endif
