/*
 * ***************************************************************************
 * Copyright (C) 2015 Marvell International Ltd.
 * ***************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************
 */

#include <common.h>
#include <i2c.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/clock.h>
#include <asm/arch-mvebu/fdt.h>

#define CONFIG_MAX_I2C_NUM			5
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
#define MAX_CHIPS_PER_BUS			10

#define I2C_BRIDGE_PAGE_READ_SIZE		8
#define I2C_BRIDGE_PAGE_WRITE_SIZE		4
#define I2C_BRIDGE_TIMEOUT_VALUE		100
#define I2C_BRIDGE_REG_OFFSET			0xC0

/* I2C Bridge Control */
#define I2C_BC_WRITE_OFFSET			0
#define I2C_BC_READ_OFFSET			1
#define I2C_BC_SLAVE_ADDR_OFFSET		2
#define I2C_BC_EXTAND_ADDR_OFFSET		12
#define I2C_BC_TX_SIZE_OFFSET			13
#define I2C_BC_RX_SIZE_OFFSET			16
#define I2C_BC_BRIDGE_EN_OFFSET			19
#define I2C_BC_REPEATED_START_OFFSET		20

/* I2C Bridge Status */
#define I2C_BS_ERROR_STATE_OFFSET		11
#define I2C_BS_ERROR_STATE_MASK			0X7 << I2C_BS_ERROR_STATE_OFFSET

#define reverse_2_bytes(v) ((v) & 0xff) << 8 | ((v) & 0xff00) >> 8;

DECLARE_GLOBAL_DATA_PTR;

struct  mvebu_i2c_bridge_regs {
	u32 transmit_low;
	u32 transmit_high;
	u32 receive_low;
	u32 receive_high;
	u32 control;
	u32 status;
	u32 interrupt;
	u32 interrupt_mask;
	u32 timing_gaps;
};

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

struct mvebu_i2c_bus {
	struct  mvebu_i2c_regs *i2c_reg;
	struct  mvebu_i2c_bridge_regs *i2c_bridge_reg;
	u32 clock;
	int two_bytes_addr[MAX_CHIPS_PER_BUS];
	bool status;
};

enum i2c_bridge_error {
	I2C_BRIDGE_NO_ERROR,
	I2C_BRIDGE_START_ERROR,
	I2C_BRIDGE_ADDR_ERROR,
	I2C_BRIDGE_EXTENDED_ADDR_ERROR,
	I2C_BRIDGE_TRANSMIT_ERROR,
	I2C_BRIDGE_RECEIVE_ERROR,
	I2C_BRIDGE_UNKNOWN_ERROR,
};

/* initialize i2c_bus to -1, because we use this struct before relocation */
static struct mvebu_i2c_bus i2c_bus[CONFIG_MAX_I2C_NUM] = { { .i2c_reg = NULL, .i2c_bridge_reg = NULL, .clock = -1,
					 .two_bytes_addr = {[0 ... MAX_CHIPS_PER_BUS - 1] = -1}, .status = false},
							    { .i2c_reg = NULL, .i2c_bridge_reg = NULL, .clock = -1,
					 .two_bytes_addr = {[0 ... MAX_CHIPS_PER_BUS - 1] = -1}, .status = false},
							    { .i2c_reg = NULL, .i2c_bridge_reg = NULL, .clock = -1,
					 .two_bytes_addr = {[0 ... MAX_CHIPS_PER_BUS - 1] = -1}, .status = false},
							    { .i2c_reg = NULL, .i2c_bridge_reg = NULL, .clock = -1,
					 .two_bytes_addr = {[0 ... MAX_CHIPS_PER_BUS - 1] = -1}, .status = false},
							    { .i2c_reg = NULL, .i2c_bridge_reg = NULL, .clock = -1,
					 .two_bytes_addr = {[0 ... MAX_CHIPS_PER_BUS - 1] = -1}, .status = false} };


#define i2c_reg(x) (&i2c_bus[gd->cur_i2c_bus].i2c_reg->x)
#define i2c_bridge_reg(x) (&i2c_bus[gd->cur_i2c_bus].i2c_bridge_reg->x)

static int mvebu_i2c_lost_arbitration(u32 *status)
{
	*status = readl(i2c_reg(status));
	if ((I2C_STATUS_LOST_ARB_DATA_ADDR_TRANSFER == *status) || (I2C_STATUS_LOST_ARB_GENERAL_CALL == *status))
		return -EAGAIN;
	return 0;
}

static void mvebu_i2c_interrupt_clear(void)
{
	u32 reg;

	/* Wait for 1 ms to prevent I2C register write after write issues */
	udelay(1000);
	reg = readl(i2c_reg(control));
	reg &= ~(I2C_CONTROL_IFLG);
	writel(reg, i2c_reg(control));
	/* Wait for 1 ms for the clear to take effect */
	udelay(1000);

	return;
}

static int mvebu_i2c_interrupt_get(void)
{
	u32 reg;

	/* get the interrupt flag bit */
	reg = readl(i2c_reg(control));
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
	writel(readl(i2c_reg(control)) | I2C_CONTROL_START, i2c_reg(control));

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
	if ((readl(i2c_reg(control)) & I2C_CONTROL_START) != 0) {
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
	writel(readl(i2c_reg(control)) | I2C_CONTROL_STOP, i2c_reg(control));
	mvebu_i2c_interrupt_clear();

	timeout = 0;
	/* Read control register, check the control stop bit */
	while ((readl(i2c_reg(control)) & I2C_CONTROL_STOP) && (timeout++ < I2C_TIMEOUT_VALUE))
		;
	if (timeout >= I2C_TIMEOUT_VALUE) {
		error("Stop bit didn't went down\n");
		return -ETIMEDOUT;
	}

	/* check that stop bit went down */
	if ((readl(i2c_reg(control)) & I2C_CONTROL_STOP) != 0) {
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
	writel(reg, i2c_reg(data));
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

static unsigned int mvebu_i2c_bus_speed_set(struct i2c_adapter *adap, unsigned int requested_speed)
{
	unsigned int n, m, freq, margin, min_margin = 0xffffffff;
	unsigned int actual_freq = 0, actual_n = 0, actual_m = 0;

	debug("%s: clock = 0x%x, freq = 0x%x\n", __func__, i2c_bus[gd->cur_i2c_bus].clock, requested_speed);
	/* Calucalte N and M for the TWSI clock baud rate */
	for (n = 0; n < 8; n++) {
		for (m = 0; m < 16; m++) {
			freq = i2c_bus[gd->cur_i2c_bus].clock / (10 * (m + 1) * (2 << n));
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
	writel((actual_m << 3) | actual_n, i2c_reg(baudrate));

	return 0;
}

static void mvebu_i2c_init(struct i2c_adapter *adap, int speed, int slaveaddr)
{
	int node_list[CONFIG_MAX_I2C_NUM], node, var, j = 0;
	u32 i;

	if (i2c_bus[gd->cur_i2c_bus].status)
		return;

	/* update the node_list with the active I2C nodes */
	fdtdec_find_aliases_for_id(gd->fdt_blob, "i2c", COMPAT_MVEBU_I2C, node_list, CONFIG_MAX_I2C_NUM);

	for (i = 0; i < CONFIG_MAX_I2C_NUM ; i++) {
		node = node_list[i];

		if (node <= 0)
			continue;

		if (gd->cur_i2c_bus == i) {
			i2c_bus[gd->cur_i2c_bus].i2c_reg =
				(struct  mvebu_i2c_regs *)fdt_get_regs_offs(gd->fdt_blob, node, "reg");
			i2c_bus[gd->cur_i2c_bus].i2c_bridge_reg =
				(struct  mvebu_i2c_bridge_regs *)(fdt_get_regs_offs(gd->fdt_blob, node, "reg")
											+ I2C_BRIDGE_REG_OFFSET);
			i2c_bus[gd->cur_i2c_bus].clock = soc_clock_get(gd->fdt_blob, node);
			/* Get the fisrt variable in i2c (if exist) */
			var = fdt_first_subnode(gd->fdt_blob, node);
			/* Find the variables under i2c node */
			while (var > 0) {
				if (fdtdec_get_bool(gd->fdt_blob, var, "two_bytes_addr")) {
					i2c_bus[gd->cur_i2c_bus].two_bytes_addr[j] =
						fdtdec_get_int(gd->fdt_blob, var, "address", 0);
					j++;
				}
				/* Get the offset of the next subnode */
				var = fdt_next_subnode(gd->fdt_blob, var);
			};
			i2c_bus[gd->cur_i2c_bus].status = true;
		}
	}
	if (!i2c_bus[gd->cur_i2c_bus].status)
		error("i2c %d device not found in device tree blob\n", gd->cur_i2c_bus);

	/* Reset the I2C logic */
	writel(0, i2c_reg(soft_reset));

	udelay(2000);

	mvebu_i2c_bus_speed_set(adap, speed);

	/* Enable the I2C and slave */
	writel(I2C_CONTROL_TWSIEN | I2C_CONTROL_ACK, i2c_reg(control));

	/* set the I2C slave address */
	writel(0, i2c_reg(xtnd_slave_addr));
	writel(slaveaddr, i2c_reg(slave_address));

	/* unmask I2C interrupt */
	writel(readl(i2c_reg(control)) | I2C_CONTROL_INTEN, i2c_reg(control));

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

#ifdef CONFIG_MVEBU_I2C_BRIDGE
static int i2c_bridge_detect_error(int status)
{
	status = (status & I2C_BS_ERROR_STATE_MASK) >> I2C_BS_ERROR_STATE_OFFSET;
	if (status < I2C_BRIDGE_UNKNOWN_ERROR)
		return status;
	return I2C_BRIDGE_UNKNOWN_ERROR;
}

static bool i2c_offload_write(u16 dev_addr, u16 offset, u8 offset_length, u8 *data, u8 size, u8 extend_addr)
{
	u32 control_reg, status_reg;
	u32 low_trans, high_trans;
	u32 i, timeout = 0;

	/* verify data size */
	if (size > I2C_BRIDGE_PAGE_WRITE_SIZE || size < 1) {
		error("bytes to transmit should be betweeen 1 and 6\n");
		return false;
	}

	/* verify device address according to the extended address,
	   extend address: 0x0 = 7-bit address, 0x1 = 10-bit address */
	if (extend_addr && (dev_addr > 0x3ff)) {
			error("address is more than 10bits\n");
			return false;
	} else if (dev_addr > 0x7f) {
		error("address is more than 7bits\n");
		return false;
	}

	/* verify offset length */
	if (offset_length < 1 || offset_length > 2) {
		error("i2c bridge support only 1 or 2 bytes address\n");
		return false;
	}

	/* write offset and data to transmit registers */
	if (offset_length == 2) {
		control_reg = ((size + 1) << I2C_BC_TX_SIZE_OFFSET);
		/* reverse the offset address */
		offset = reverse_2_bytes(offset);
		/* write data starting from the third byte in low transmit register
		   until the fourth byte in high transmitregister */
		for (i = 0, low_trans = 0x0, high_trans = 0x0; i < size; i++) {
			if (i > 1)
				high_trans |= (data[i] << (i - 2)*8);
			else
				low_trans |= (data[i] << (i + 2)*8);
		}
	} else if (offset_length == 1) {
		control_reg = (size << I2C_BC_TX_SIZE_OFFSET);
		offset &= 0xff;
		/* write data starting from the second byte in low transmit register
		   until the fourth byte in high transmitregister */
		for (i = 0, low_trans = 0x0, high_trans = 0x0; i < size; i++) {
			if (i > 2)
				high_trans |= (data[i] << (i - 3)*8);
			else
				low_trans |= (data[i] << (i + 1)*8);
		}
	}
	low_trans |= offset;
	writel(low_trans, i2c_bridge_reg(transmit_low));
	writel(high_trans, i2c_bridge_reg(transmit_high));

	/* extend address: 0x0 = 7-bit address, 0x1 = 10-bit address */
	if (extend_addr)
		control_reg |= (0x1 << I2C_BC_EXTAND_ADDR_OFFSET);

	/* write device address */
	control_reg |= (dev_addr << I2C_BC_SLAVE_ADDR_OFFSET);

	/* prepare to write transaction  */
	control_reg |= (0x1 << I2C_BC_WRITE_OFFSET);
	writel(control_reg, i2c_bridge_reg(control));

	/* start transaction */
	control_reg |= (0x1 << I2C_BC_BRIDGE_EN_OFFSET);
	writel(control_reg, i2c_bridge_reg(control));

	/* wait to interrupt until transaction is ending */
	while (!readl(i2c_bridge_reg(interrupt)) && (timeout++ < I2C_BRIDGE_TIMEOUT_VALUE))
		udelay(100);
	if (timeout >= I2C_BRIDGE_TIMEOUT_VALUE)
		return -ETIMEDOUT;

	/* back to back write transaction delay */
	udelay(10000);

	/* read status register to verify no error */
	status_reg = readl(i2c_bridge_reg(status));
	debug("register status write = %x\n", status_reg);

	/* reset control register */
	writel(0x0, i2c_bridge_reg(control));

	/* reset interrupt register */
	writel(0x0, i2c_bridge_reg(interrupt));

	if (status_reg & 0x1) {
		/* reset status register */
		writel(0x0, i2c_bridge_reg(status));
		error("status register = %d\n", i2c_bridge_detect_error(status_reg));
		return false;
	}
	return true;
}

static bool i2c_offload_read(u16 dev_addr, u16 offset, u8 offset_length, u8 *data, u8 size, u8 extend_addr)
{
	u32 control_reg, status_reg;
	u32 low_trans, high_rec, low_rec;
	u32 i, timeout = 0;

	/* verify data size */
	if (size > I2C_BRIDGE_PAGE_READ_SIZE || size < 1) {
		error("bytes to transmit should be betweeen 1 and 8\n");
		return false;
	}

	/* verify device address according to the extended address,
	   extend address: 0x0 = 7-bit address, 0x1 = 10-bit address */
	if (extend_addr && (dev_addr > 0x3ff)) {
			error("address is more than 10bits\n");
			return false;
	} else if (dev_addr > 0x7f) {
		error("address is more than 7bits\n");
		return false;
	}

	/* verify offset length */
	if (offset_length < 1 || offset_length > 2) {
		error("i2c bridge support only 1 or 2 bytes address\n");
		return false;
	}

	/* write offset size and offset address */
	if (offset_length == 2) {
		control_reg = (0x1 << I2C_BC_TX_SIZE_OFFSET);
		/* reverse the offset address */
		offset = reverse_2_bytes(offset);
	} else if (offset_length == 1) {
		offset &= 0xff;
		control_reg = (0x0 << I2C_BC_TX_SIZE_OFFSET);
	}
	low_trans = offset;
	writel(low_trans, i2c_bridge_reg(transmit_low));

	/* extend address: 0x0 = 7-bit address, 0x1 = 10-bit address */
	if (extend_addr)
		control_reg |= (0x1 << I2C_BC_EXTAND_ADDR_OFFSET);

	/* write device address */
	control_reg |= (dev_addr << I2C_BC_SLAVE_ADDR_OFFSET);

	/* write size of receive data */
	control_reg |= ((size - 1) << I2C_BC_RX_SIZE_OFFSET);

	/* prepare to read after write transaction */
	control_reg |= ((0x1 << I2C_BC_WRITE_OFFSET) | (0x1 << I2C_BC_READ_OFFSET));
	writel(control_reg , i2c_bridge_reg(control));

	/* start transaction */
	control_reg |= (0x1 << I2C_BC_BRIDGE_EN_OFFSET) | (0x1 << I2C_BC_REPEATED_START_OFFSET);
	writel(control_reg, i2c_bridge_reg(control));

	/* wait to interrupt until transaction is ending */
	while (!readl(i2c_bridge_reg(interrupt)) && (timeout++ < I2C_BRIDGE_TIMEOUT_VALUE))
		udelay(100);
	if (timeout >= I2C_BRIDGE_TIMEOUT_VALUE)
		return -ETIMEDOUT;

	/* back to back write transaction delay */
	udelay(1000);

	/* read status register to verify no error */
	status_reg = readl(i2c_bridge_reg(status));
	debug("register status read = %x\n", status_reg);

	/* reset control register */
	writel(0x0, i2c_bridge_reg(control));

	/* reset interrupt register */
	writel(0x0, i2c_bridge_reg(interrupt));

	if (status_reg & 0x1) {
		/* reset status register */
		writel(0x0, i2c_bridge_reg(status));
		error("status register = %d\n", i2c_bridge_detect_error(status_reg));
		return false;
	}

	/* copy received data from registers */
	low_rec = readl(i2c_bridge_reg(receive_low));
	high_rec = readl(i2c_bridge_reg(receive_high));
	for (i = 0; i < size; i++) {
		if (i < 4)
			data[i] = (low_rec >> i*8) & 0xff;
		else
			data[i] = (high_rec >> (i-4)*8) & 0xff;
	}
	return true;
}

static int mvebu_i2c_read(struct i2c_adapter *adap, uchar chip, uint addr,
			int alen, uchar *data, int length)
{
	int reserve_length, size_of_loop, i;

	/* i2c bridge support on read transaction with 8-bytes */
	size_of_loop = length / I2C_BRIDGE_PAGE_READ_SIZE;
	reserve_length = length % I2C_BRIDGE_PAGE_READ_SIZE;

	/* read from EEPROM in sections of 8 bytes */
	for (i = 0; i < size_of_loop; i++) {
		i2c_offload_read(chip, addr + (i*I2C_BRIDGE_PAGE_READ_SIZE), alen,
				 (uint8_t *)data + i*I2C_BRIDGE_PAGE_READ_SIZE, I2C_BRIDGE_PAGE_READ_SIZE, 0);
	}
	if (reserve_length)
		i2c_offload_read(chip, addr + (i*I2C_BRIDGE_PAGE_READ_SIZE), alen,
				 (uint8_t *)data + i*I2C_BRIDGE_PAGE_READ_SIZE, reserve_length, 0);
	return 0;
}

static int mvebu_i2c_write(struct i2c_adapter *adap, uchar chip, uint addr,
			int alen, uchar *data, int length)
{
	int reserve_length = 0, size_of_loop = 0, i;

	/* writing the first bytes until align to 32, because i2c page write align to 32 bytes */
	if ((addr % 32 + length) > 31) {
		for (i = addr%32; (i + I2C_BRIDGE_PAGE_WRITE_SIZE) < 32 && length >= I2C_BRIDGE_PAGE_WRITE_SIZE;
										     i += I2C_BRIDGE_PAGE_WRITE_SIZE) {
			i2c_offload_write(chip, addr, alen, (uint8_t *)data, I2C_BRIDGE_PAGE_WRITE_SIZE, 0);
			length -= I2C_BRIDGE_PAGE_WRITE_SIZE;
			addr += I2C_BRIDGE_PAGE_WRITE_SIZE;
			data += I2C_BRIDGE_PAGE_WRITE_SIZE;
		}

		reserve_length = 32 - i;
		if (reserve_length) {
			i2c_offload_write(chip, addr, alen, (uint8_t *)data, reserve_length, 0);
			addr += reserve_length;
			data += reserve_length;
			length -= reserve_length;
		}
	}
	/* i2c bridge support on write transaction with 4-bytes */
	size_of_loop = length / I2C_BRIDGE_PAGE_WRITE_SIZE;
	reserve_length = length % I2C_BRIDGE_PAGE_WRITE_SIZE;

	/* write to EEPROM in sections of 4 bytes */
	for (i = 0; i < size_of_loop;  i++) {
		i2c_offload_write(chip, addr + i*I2C_BRIDGE_PAGE_WRITE_SIZE, alen,
				  (uint8_t *)data + i*I2C_BRIDGE_PAGE_WRITE_SIZE, I2C_BRIDGE_PAGE_WRITE_SIZE, 0);
	}

	if (reserve_length)
		i2c_offload_write(chip, addr + i*I2C_BRIDGE_PAGE_WRITE_SIZE, alen,
				  (uint8_t *)data + i*I2C_BRIDGE_PAGE_WRITE_SIZE, reserve_length, 0);
	return 0;
}
#else
/* regular i2c transaction */
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
			reg = readl(i2c_reg(control));
			reg &= ~(I2C_CONTROL_ACK);
			writel(reg, i2c_reg(control));
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
		*p_block = (u8) readl(i2c_reg(data));
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
		writel((u32) *p_block, i2c_reg(data));
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

static int mvebu_i2c_target_offset_set(int chip, uint addr)
{
	u8 off_block[2], support_two_bytes = 0;
	u32 off_size, i;

	for (i = 0; (i < MAX_CHIPS_PER_BUS) && (i2c_bus[gd->cur_i2c_bus].two_bytes_addr[i] != -1) ; i++) {
		if (i2c_bus[gd->cur_i2c_bus].two_bytes_addr[i] == chip) {
			support_two_bytes = 1;
			break;
		}
	}
	if (support_two_bytes) {
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

			ret = mvebu_i2c_target_offset_set(chip, addr);
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

	writel(readl(i2c_reg(control)) | I2C_CONTROL_ACK, i2c_reg(control));

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
			ret = mvebu_i2c_target_offset_set(chip, addr);
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
#endif  /* CONFIG_MVEBU_I2C_BRIDGE */

U_BOOT_I2C_ADAP_COMPLETE(mvebu0, mvebu_i2c_init, mvebu_i2c_probe,
			 mvebu_i2c_read, mvebu_i2c_write,
			 mvebu_i2c_bus_speed_set,
			 CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE, 0)

U_BOOT_I2C_ADAP_COMPLETE(mvebu1, mvebu_i2c_init, mvebu_i2c_probe,
			 mvebu_i2c_read, mvebu_i2c_write,
			 mvebu_i2c_bus_speed_set,
			 CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE, 1)

U_BOOT_I2C_ADAP_COMPLETE(mvebu2, mvebu_i2c_init, mvebu_i2c_probe,
			 mvebu_i2c_read, mvebu_i2c_write,
			 mvebu_i2c_bus_speed_set,
			 CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE, 2)

U_BOOT_I2C_ADAP_COMPLETE(mvebu3, mvebu_i2c_init, mvebu_i2c_probe,
			 mvebu_i2c_read, mvebu_i2c_write,
			 mvebu_i2c_bus_speed_set,
			 CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE, 3)

U_BOOT_I2C_ADAP_COMPLETE(mvebu4, mvebu_i2c_init, mvebu_i2c_probe,
			 mvebu_i2c_read, mvebu_i2c_write,
			 mvebu_i2c_bus_speed_set,
			 CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE, 4)
