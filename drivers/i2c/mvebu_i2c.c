/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., on the worldwide web at
 * http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <common.h>
#include <i2c.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/arch-mvebu/clock.h>
#include <asm/arch-mvebu/mvebu.h>

#define I2C_TIMEOUT_VALUE			0x500
#define I2C_MAX_RETRY_CNT			1000
#define I2C_CMD_WRITE				0x0
#define I2C_CMD_READ				0x1

#define I2C_DATA_ADDR_7BIT_OFFS			0x1
#define I2C_DATA_ADDR_7BIT_MASK			(0xFF << I2C_DATA_ADDR_7BIT_OFFS)

#define	I2C_CONTROL_ACK				0x00000004
#define	I2C_CONTROL_IFLG			0x00000008
#define	I2C_CONTROL_STOP			0x00000010
#define	I2C_CONTROL_START			0x00000020
#define	I2C_CONTROL_TWSIEN			0x00000040
#define	I2C_CONTROL_INTEN			0x00000080

#define	I2C_STATUS_START			0x08
#define	I2C_STATUS_REPEATED_START		0x10
#define	I2C_STATUS_ADDR_W_ACK			0x18
#define	I2C_STATUS_DATA_W_ACK			0x28
#define I2C_STATUS_LOST_ARB_DATA_ADDR_TRANSFER	0x38
#define	I2C_STATUS_ADDR_R_ACK			0x40
#define	I2C_STATUS_DATA_R_ACK			0x50
#define	I2C_STATUS_DATA_R_NAK			0x58
#define I2C_STATUS_LOST_ARB_GENERAL_CALL	0x78
#define	I2C_STATUS_IDLE				0xF8

DECLARE_GLOBAL_DATA_PTR;

struct  mvebu_i2c_regs {
	u32 slave_address;
	u32 data;
	u32 control;
	union {
		u32 status;	/* when reading */
		u32 baudrate;	/* when writing */
	};
	u32 xtnd_slave_addr;
	u32 reserved[2];
	u32 soft_reset;
};

static struct  mvebu_i2c_regs *i2c_reg;

static int mvebu_i2c_lost_arbitration(u32 *status)
{
	*status = readl(&i2c_reg->status);
	if ((I2C_STATUS_LOST_ARB_DATA_ADDR_TRANSFER == *status) || (I2C_STATUS_LOST_ARB_GENERAL_CALL == *status))
		return -EAGAIN;
	return 0;
}

static void mvebu_i2c_interrupt_clear(void)
{
	u32 reg;

	/* Wait for 1 ms to prevent I2C register write after write issues */
	udelay(1000);
	reg = readl(&i2c_reg->control);
	reg &= ~(I2C_CONTROL_IFLG);
	writel(reg, &i2c_reg->control);
	/* Wait for 1 ms for the clear to take effect */
	udelay(1000);

	return;
}

static int mvebu_i2c_interrupt_get(void)
{
	u32 reg;

	/* get the interrupt flag bit */
	reg = readl(&i2c_reg->control);
	reg &= I2C_CONTROL_IFLG;
	return reg && I2C_CONTROL_IFLG;
}

static int mvebu_i2c_wait_interrupt(void)
{
	u32 timeout = 0;
	while (!mvebu_i2c_interrupt_get() && (timeout++ < I2C_TIMEOUT_VALUE))
		;
	if (timeout >= I2C_TIMEOUT_VALUE)
		return -ETIMEDOUT;
	return 0;
}

static int mvebu_i2c_start_bit_set(void)
{
	int is_int_flag = 0;
	u32 status;

	if (mvebu_i2c_interrupt_get())
		is_int_flag = 1;

	/* set start bit */
	writel(readl(&i2c_reg->control) | I2C_CONTROL_START, &i2c_reg->control);

	/* in case that the int flag was set before i.e. repeated start bit */
	if (is_int_flag) {
		debug("%s: repeated start Bit\n", __func__);
		mvebu_i2c_interrupt_clear();
	}

	if (mvebu_i2c_wait_interrupt()) {
		error("Start clear bit timeout\n");
		return -ETIMEDOUT;
	}

	/* check that start bit went down */
	if ((readl(&i2c_reg->control) & I2C_CONTROL_START) != 0) {
		error("Start bit didn't went down\n");
		return -EPERM;
	}

	/* check the status */
	if (mvebu_i2c_lost_arbitration(&status)) {
		debug("%s - %d: Lost arbitration, got status %x\n", __func__, __LINE__, status);
		return -EAGAIN;
	}
	if ((status != I2C_STATUS_START) && (status != I2C_STATUS_REPEATED_START)) {
		error("Got status %x after enable start bit.\n", status);
		return -EPERM;
	}

	return 0;
}

static int mvebu_i2c_stop_bit_set(void)
{
	int timeout;
	u32 status;

	/* Generate stop bit */
	writel(readl(&i2c_reg->control) | I2C_CONTROL_STOP, &i2c_reg->control);
	mvebu_i2c_interrupt_clear();

	timeout = 0;
	/* Read control register, check the control stop bit */
	while ((readl(&i2c_reg->control) & I2C_CONTROL_STOP) && (timeout++ < I2C_TIMEOUT_VALUE))
		;
	if (timeout >= I2C_TIMEOUT_VALUE) {
		error("Stop bit didn't went down\n");
		return -ETIMEDOUT;
	}

	/* check that stop bit went down */
	if ((readl(&i2c_reg->control) & I2C_CONTROL_STOP) != 0) {
		error("Stop bit didn't went down\n");
		return -EPERM;
	}

	/* check the status */
	if (mvebu_i2c_lost_arbitration(&status)) {
		debug("%s - %d: Lost arbitration, got status %x\n", __func__, __LINE__, status);
		return -EAGAIN;
	}
	if (status != I2C_STATUS_IDLE) {
		error("Got status %x after enable stop bit.\n", status);
		return -EPERM;
	}

	return 0;
}

static int mvebu_i2c_address_set(u8 chain, int command)
{
	u32 reg, status;

	reg = (chain << I2C_DATA_ADDR_7BIT_OFFS) & I2C_DATA_ADDR_7BIT_MASK;
	reg |= command;
	writel(reg, &i2c_reg->data);
	udelay(1000);

	mvebu_i2c_interrupt_clear();

	if (mvebu_i2c_wait_interrupt()) {
		error("Start clear bit timeout\n");
		return -ETIMEDOUT;
	}

	/* check the status */
	if (mvebu_i2c_lost_arbitration(&status)) {
		debug("%s - %d: Lost arbitration, got status %x\n", __func__, __LINE__, status);
		return -EAGAIN;
	}
	if (((status != I2C_STATUS_ADDR_R_ACK) && (command == I2C_CMD_READ)) ||
	   ((status != I2C_STATUS_ADDR_W_ACK) && (command == I2C_CMD_WRITE))) {
		/* only in debug, since in boot we try to read the SPD of both DRAM, and we don't
		   want error messages in case DIMM doesn't exist. */
		debug("%s: ERROR - status %x addr in %s mode.\n", __func__, status, (command == I2C_CMD_WRITE) ?
				"Write" : "Read");
		return -EPERM;
	}

	return 0;
}

static int mvebu_i2c_data_receive(u8 *p_block, u32 block_size)
{
	u32 reg, status, block_size_read = block_size;

	/* Wait for cause interrupt */
	if (mvebu_i2c_wait_interrupt()) {
		error("Start clear bit timeout\n");
		return -ETIMEDOUT;
	}
	while (block_size_read) {
		if (block_size_read == 1) {
			reg = readl(&i2c_reg->control);
			reg &= ~(I2C_CONTROL_ACK);
			writel(reg, &i2c_reg->control);
		}
		mvebu_i2c_interrupt_clear();

		if (mvebu_i2c_wait_interrupt()) {
			error("Start clear bit timeout\n");
			return -ETIMEDOUT;
		}
		/* check the status */
		if (mvebu_i2c_lost_arbitration(&status)) {
			debug("%s - %d: Lost arbitration, got status %x\n", __func__, __LINE__, status);
			return -EAGAIN;
		}
		if ((status != I2C_STATUS_DATA_R_ACK) && (block_size_read != 1)) {
			error("Status %x in read transaction\n", status);
			return -EPERM;
		}
		if ((status != I2C_STATUS_DATA_R_NAK) && (block_size_read == 1)) {
			error("Status %x in Rd Terminate\n", status);
			return -EPERM;
		}

		/* read the data */
		*p_block = (u8) readl(&i2c_reg->data);
		debug("%s: place %d read %x\n", __func__, block_size - block_size_read, *p_block);
		p_block++;
		block_size_read--;
	}

	return 0;
}

static int mvebu_i2c_data_transmit(u8 *p_block, u32 block_size)
{
	u32 status, block_size_write = block_size;

	if (mvebu_i2c_wait_interrupt()) {
		error("Start clear bit timeout\n");
		return -ETIMEDOUT;
	}

	while (block_size_write) {
		/* write the data */
		writel((u32) *p_block, &i2c_reg->data);
		debug("%s: index = %d, data = %x\n", __func__, block_size - block_size_write, *p_block);
		p_block++;
		block_size_write--;

		mvebu_i2c_interrupt_clear();

		if (mvebu_i2c_wait_interrupt()) {
			error("Start clear bit timeout\n");
			return -ETIMEDOUT;
		}

		/* check the status */
		if (mvebu_i2c_lost_arbitration(&status)) {
			debug("%s - %d: Lost arbitration, got status %x\n", __func__, __LINE__, status);
			return -EAGAIN;
		}
		if (status != I2C_STATUS_DATA_W_ACK) {
			error("Status %x in write transaction\n", status);
			return -EPERM;
		}
	}

	return 0;
}

static int mvebu_i2c_target_offset_set(int alen, uint addr)
{
	u8 off_block[2];
	u32 off_size;

	if (alen == 2) {
		off_block[0] = (addr >> 8) & 0xff;
		off_block[1] = addr & 0xff;
		off_size = 2;
	} else {
		off_block[0] = addr & 0xff;
		off_size = 1;
	}
	debug("%s: off_size = %x addr1 = %x addr2 = %x\n", __func__, off_size, off_block[0], off_block[1]);
	return mvebu_i2c_data_transmit(off_block, off_size);
}

static unsigned int mvebu_i2c_bus_speed_set(struct i2c_adapter *adap, unsigned int requested_speed)
{
	unsigned int n, m, freq, margin, min_margin = 0xffffffff;
	unsigned int actual_freq = 0, actual_n = 0, actual_m = 0;

	debug("%s: Tclock = 0x%x, freq = 0x%x\n", __func__, soc_tclk_get(), requested_speed);
	/* Calucalte N and M for the TWSI clock baud rate */
	for (n = 0; n < 8; n++) {
		for (m = 0; m < 16; m++) {
			freq = soc_tclk_get() / (10 * (m + 1) * (2 << n));
			margin = abs(requested_speed - freq);

			if ((freq <= requested_speed) && (margin < min_margin)) {
				min_margin = margin;
				actual_freq = freq;
				actual_n = n;
				actual_m = m;
			}
		}
	}
	debug("%s: actual_n = 0x%x, actual_m = 0x%x, actual_freq = 0x%x\n", __func__, actual_n, actual_m, actual_freq);
	/* Set the baud rate */
	writel((actual_m << 3) | actual_n, &i2c_reg->baudrate);

	return 0;
}

static void mvebu_i2c_init(struct i2c_adapter *adap, int speed, int slaveaddr)
{
	i2c_reg = (struct  mvebu_i2c_regs *)MVEBU_I2C_BASE(gd->cur_i2c_bus);
	/* Reset the I2C logic */
	writel(0, &i2c_reg->soft_reset);

	udelay(2000);

	mvebu_i2c_bus_speed_set(adap, speed);

	/* Enable the I2C and slave */
	writel(I2C_CONTROL_TWSIEN | I2C_CONTROL_ACK, &i2c_reg->control);

	/* set the I2C slave address */
	writel(0, &i2c_reg->xtnd_slave_addr);
	writel(slaveaddr, &i2c_reg->slave_address);

	/* unmask I2C interrupt */
	writel(readl(&i2c_reg->control) | I2C_CONTROL_INTEN, &i2c_reg->control);

	udelay(1000);
}

static int mvebu_i2c_probe(struct i2c_adapter *adap, uchar chip)
{
	int ret = 0;

	ret = mvebu_i2c_start_bit_set();
	if (ret != 0) {
		mvebu_i2c_stop_bit_set();
		error("%s - %d: %s", __func__, __LINE__, "mvebu_i2c_start_bit_set failed\n");
		return -EPERM;
	}

	ret = mvebu_i2c_address_set(chip, I2C_CMD_WRITE);
	if (ret != 0) {
		mvebu_i2c_stop_bit_set();
		debug("%s - %d: %s", __func__, __LINE__, "mvebu_i2c_address_set failed\n");
		return -EPERM;
	}

	mvebu_i2c_stop_bit_set();

	debug("%s: successful I2C probe\n", __func__);

	return ret;
}

static int mvebu_i2c_read(struct i2c_adapter *adap, uchar chip, uint addr,
			int alen, uchar *data, int length)
{
	int ret = 0;
	u32 counter = 0;

	do	{
		if (ret != -EAGAIN && ret) {
			error("i2c transaction failed, after %d retries\n", counter);
			mvebu_i2c_stop_bit_set();
			return ret;
		}

		/* wait for 1 ms for the interrupt clear to take effect */
		if (counter > 0)
			udelay(1000);
		counter++;

		ret = mvebu_i2c_start_bit_set();
		if (ret)
			continue;

		/* if EEPROM device */
		if (alen != 0) {
			ret = mvebu_i2c_address_set(chip, I2C_CMD_WRITE);
			if (ret)
				continue;

			ret = mvebu_i2c_target_offset_set(alen, addr);
			if (ret)
				continue;
			ret = mvebu_i2c_start_bit_set();
			if (ret)
				continue;
		}

		ret =  mvebu_i2c_address_set(chip, I2C_CMD_READ);
		if (ret)
			continue;

		ret = mvebu_i2c_data_receive(data, length);
		if (ret)
			continue;

		ret =  mvebu_i2c_stop_bit_set();
	} while ((ret == -EAGAIN) && (counter < I2C_MAX_RETRY_CNT));

	if (counter == I2C_MAX_RETRY_CNT)
		error("I2C transactions failed, got EAGAIN %d times\n", I2C_MAX_RETRY_CNT);

	writel(readl(&i2c_reg->control) | I2C_CONTROL_ACK, &i2c_reg->control);

	udelay(1000);

	return 0;
}

static int mvebu_i2c_write(struct i2c_adapter *adap, uchar chip, uint addr,
			int alen, uchar *data, int length)
{
	int ret = 0;
	u32 counter = 0;

	do	{
		if (ret != -EAGAIN && ret) {
			error("i2c transaction failed\n");
			mvebu_i2c_stop_bit_set();
			return ret;
		}
		/* wait for 1 ms for the interrupt clear to take effect */
		if (counter > 0)
			udelay(1000);
		counter++;

		ret = mvebu_i2c_start_bit_set();
		if (ret)
			continue;

		ret = mvebu_i2c_address_set(chip, I2C_CMD_WRITE);
		if (ret)
			continue;

		/* if EEPROM device */
		if (alen != 0) {
			ret = mvebu_i2c_target_offset_set(alen, addr);
			if (ret)
				continue;
		}

		ret = mvebu_i2c_data_transmit(data, length);
		if (ret)
			continue;

		ret = mvebu_i2c_stop_bit_set();
	} while ((ret == -EAGAIN) && (counter < I2C_MAX_RETRY_CNT));

	if (counter == I2C_MAX_RETRY_CNT)
		error("I2C transactions failed, got EAGAIN %d times\n", I2C_MAX_RETRY_CNT);

	udelay(1000);

	return 0;
}

U_BOOT_I2C_ADAP_COMPLETE(mvebu0, mvebu_i2c_init, mvebu_i2c_probe,
			 mvebu_i2c_read, mvebu_i2c_write,
			 mvebu_i2c_bus_speed_set,
			 CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE, 0)

U_BOOT_I2C_ADAP_COMPLETE(mvebu1, mvebu_i2c_init, mvebu_i2c_probe,
			 mvebu_i2c_read, mvebu_i2c_write,
			 mvebu_i2c_bus_speed_set,
			 CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE, 0)
