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
#include <malloc.h>
#include <spi.h>
#include <asm-generic/errno.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/mvebu.h>

DECLARE_GLOBAL_DATA_PTR;

/* to use global variables before u-boot relocation, initialize it to something !=0 */
unsigned long mvebu_spi_reg_base = 100;
#define MVEBU_SPI_BASE         mvebu_spi_reg_base

unsigned int mvebu_spi_input_clock;
unsigned int mvebu_spi_max_freq;
static bool mvebu_spi_fifo_enabled;

#define CONFIG_MAX_SPI_NUM	8
#define CONFIG_MAX_SPI_CS_NUM	4

#define SPI_TIMEOUT		10000

#define MVEBU_SPI_A3700_CTRL_OFFSET         0x00
#define MVEBU_SPI_A3700_CONF_OFFSET         0x04
#define MVEBU_SPI_A3700_DOUT_OFFSET         0x08
#define MVEBU_SPI_A3700_DIN_OFFSET          0x0c
#define MVEBU_SPI_A3700_IF_INST_REG         0x10
#define MVEBU_SPI_A3700_IF_ADDR_REG         0x14
#define MVEBU_SPI_A3700_IF_RMODE_REG        0x18
#define MVEBU_SPI_A3700_IF_HDR_CNT_REG      0x1C
#define MVEBU_SPI_A3700_IF_DIN_CNT_REG      0x20
#define MVEBU_SPI_A3700_IF_TIME_REG         0x24

#define MVEBU_SPI_A3700_CTRL_ADDR           (MVEBU_SPI_BASE + MVEBU_SPI_A3700_CTRL_OFFSET)
#define MVEBU_SPI_A3700_CONF_ADDR           (MVEBU_SPI_BASE + MVEBU_SPI_A3700_CONF_OFFSET)
#define MVEBU_SPI_A3700_DOUT_ADDR           (MVEBU_SPI_BASE + MVEBU_SPI_A3700_DOUT_OFFSET)
#define MVEBU_SPI_A3700_DIN_ADDR            (MVEBU_SPI_BASE + MVEBU_SPI_A3700_DIN_OFFSET)
#define MVEBU_SPI_A3700_IF_INST_ADDR        (MVEBU_SPI_BASE + MVEBU_SPI_A3700_IF_INST_REG)
#define MVEBU_SPI_A3700_IF_ADDR	            (MVEBU_SPI_BASE + MVEBU_SPI_A3700_IF_ADDR_REG)
#define MVEBU_SPI_A3700_IF_RMODE            (MVEBU_SPI_BASE + MVEBU_SPI_A3700_IF_RMODE_REG)
#define MVEBU_SPI_A3700_IF_HDR_CNT_ADDR     (MVEBU_SPI_BASE + MVEBU_SPI_A3700_IF_HDR_CNT_REG)
#define MVEBU_SPI_A3700_IF_DIN_CNT_ADDR     (MVEBU_SPI_BASE + MVEBU_SPI_A3700_IF_DIN_CNT_REG)
#define MVEBU_SPI_A3700_IF_TIME_ADDR        (MVEBU_SPI_BASE + MVEBU_SPI_A3700_IF_TIME_REG)

#define MVEBU_SPI_A3700_XFER_RDY				BIT1
#define MVEBU_SPI_A3700_FIFO_FLUSH			BIT9
#define MVEBU_SPI_A3700_BYTE_LEN				BIT5
#define MVEBU_SPI_A3700_BYTE_CLK_PHA			BIT6
#define MVEBU_SPI_A3700_CLK_POL				BIT7
#define MVEBU_SPI_A3700_FIFO_EN				BIT17
#define MVEBU_SPI_A3700_SPI_EN_0				BIT16
#define MVEBU_SPI_A3700_CLK_PRESCALE_BIT			0
#define MVEBU_SPI_A3700_CLK_PRESCALE_MASK		(0x1F << MVEBU_SPI_A3700_CLK_PRESCALE_BIT)

#define MVEBU_SPI_A3700_WFIFO_FULL		BIT7
#define MVEBU_SPI_A3700_WFIFO_EMPTY		BIT6
#define MVEBU_SPI_A3700_RFIFO_EMPTY		BIT4
#define MVEBU_SPI_A3700_WFIFO_RDY		BIT3
#define MVEBU_SPI_A3700_RFIFO_RDY		BIT2

#define MVEBU_SPI_A3700_SRST			BIT16
#define MVEBU_SPI_A3700_XFER_START		BIT15
#define MVEBU_SPI_A3700_XFER_STOP		BIT14
#define MVEBU_SPI_A3700_RW_EN			BIT8

#define MVEBU_SPI_A3700_WFIFO_THRS_BIT		28
#define MVEBU_SPI_A3700_RFIFO_THRS_BIT		24
#define MVEBU_SPI_A3700_FIFO_THRS_MASK		0x7

#define MVEBU_SPI_A3700_DUMMY_CNT_BIT		12
#define MVEBU_SPI_A3700_DUMMY_CNT_MASK		0x7
#define MVEBU_SPI_A3700_ADDR_CNT_BIT		4
#define MVEBU_SPI_A3700_ADDR_CNT_MASK		0x7
#define MVEBU_SPI_A3700_INSTR_CNT_BIT		0
#define MVEBU_SPI_A3700_INSTR_CNT_MASK		0x3


/**
 * if_spi_flags_is_set() - poll a register to check if certain flags is set
 *
 * This function is almost simply checking the value of a register,
 * just more time are allowed. This is useful when you want to wait
 * and make sure that a transfer is done. However, this brings a new
 * problem that a misuse of this function can make it too long for
 * each transfer. So make sure to only invoke this routine one time,
 * do not add while(timeout--) around it, since there is already a
 * --timeout inside.
 *
 * Input:
 *	reg: register address that need to be checked.
 *	flags: bits that need to be checked if they are set to 1.
 *	timeout: how many times we need to check before return.
 * Return:	0 - the flags are not set
 *		1 - the flags are set
 */
int if_spi_flags_is_set(unsigned long reg, u16 flags, u32 timeout)
{
	while (!(readl(reg) & flags)) {
		if (--timeout == 0)
			return 0;
	}
	return 1;
}

/**
 * poll_spi_xfer_ready() - poll xfer ready flag
 *
 * When Ready bit on CONTROL register is set, it means one transfer is
 * done and SPI module (in legacy mode) is ready for the next
 * transfer.
 *
 * See also:	poll_spi_flag_set()
 *
 * Return:	0 - xfer flags is not set
 *		1 - xfer flags is set
 */
static inline int spi_poll_xfer_ready(void)
{
	unsigned int timeout = SPI_TIMEOUT;

	return if_spi_flags_is_set(MVEBU_SPI_A3700_CTRL_ADDR, MVEBU_SPI_A3700_XFER_RDY, timeout);
}

static inline int spi_poll_fifo_write_ready(void)
{
	unsigned int timeout = SPI_TIMEOUT;

	return if_spi_flags_is_set(MVEBU_SPI_A3700_CTRL_ADDR, MVEBU_SPI_A3700_WFIFO_RDY, timeout);
}

static inline int spi_poll_fifo_read_ready(void)
{
	unsigned int timeout = SPI_TIMEOUT;

	return if_spi_flags_is_set(MVEBU_SPI_A3700_CTRL_ADDR, MVEBU_SPI_A3700_RFIFO_RDY, timeout);
}

static inline int spi_poll_fifo_write_empty(void)
{
	unsigned int timeout = SPI_TIMEOUT;

	return if_spi_flags_is_set(MVEBU_SPI_A3700_CTRL_ADDR, MVEBU_SPI_A3700_WFIFO_EMPTY, timeout);
}

/**
 * spi_set_legacy() - set SPI unit to be working in legacy mode
 *
 * Legacy mode means use only DO pin (I/O 1) for Data Out, and
 * DI pin (I/O 0) for Data In.
 * Non-legacy mode means both DO and DI pin coud be used for
 * read or write.
 *
 * Return:	0 - configuration has been done.
 *		1 - error
 */
int spi_set_legacy(void)
{
	unsigned int val_conf;

	/* Always shift 1 byte at a time */
	val_conf = readl(MVEBU_SPI_A3700_CONF_ADDR);
	val_conf = val_conf & (~MVEBU_SPI_A3700_BYTE_LEN);

	/* Set legacy mode - mode 0: CPHA = 0 and  CPOL = 0 */
	val_conf = val_conf & (~MVEBU_SPI_A3700_BYTE_CLK_PHA);

	val_conf = val_conf & (~MVEBU_SPI_A3700_CLK_POL);

	/* Disable FIFO mode */
	val_conf &= ~MVEBU_SPI_A3700_FIFO_EN;

	writel(val_conf, MVEBU_SPI_A3700_CONF_ADDR);

	return 0;
}

/**
 * spi_legacy_shift_byte() - triggers the real SPI transfer
 * @bytelen:	Indicate how many bytes to transfer.
 * @dout:	Buffer address of what to send.
 * @din:	Buffer address of where to receive.
 *
 * This function triggers the real SPI transfer in legacy mode. It
 * will shift out char buffer from @dout, and shift in char buffer to
 * @din, if necessary.
 *
 * This function assumes that only one byte is shifted at one time.
 * However, it is not its responisbility to set the transfer type to
 * one-byte. Also, it does not guarantee that it will work if transfer
 * type becomes two-byte. See spi_set_legacy() for details.
 *
 * In legacy mode, simply write to the SPI_DOUT register will trigger
 * the transfer.
 *
 * If @dout == NULL, which means no actual data needs to be sent out,
 * then the function will shift out 0x00 in order to shift in data.
 * The XFER_RDY flag is checked every time before accessing SPI_DOUT
 * and SPI_DIN register.
 *
 * The number of transfers to be triggerred is decided by @bytelen.
 *
 * Return:	0 - cool
 *		-ETIMEDOUT - XFER_RDY flag timeout
 */
int spi_legacy_shift_byte(unsigned int bytelen, const void *dout, void *din)
{
	const u8 *dout_8;
	u8 *din_8;

	/* Use 0x00 as dummy dout */
	const u8 dummy_dout = 0x0;
	u32 pending_dout = 0x0;

	/* dout_8: pointer of current dout */
	dout_8 = dout;
	/* din_8: pointer of current din */
	din_8 = din;

	while (bytelen) {
		if (!spi_poll_xfer_ready())
			return -ETIMEDOUT;

		if (dout)
			pending_dout = (u32)*dout_8;
		else
			pending_dout = (u32)dummy_dout;

		/* Trigger the xfer */
		writel(pending_dout, MVEBU_SPI_A3700_DOUT_ADDR);

		if (din) {
			if (!spi_poll_xfer_ready())
				return -ETIMEDOUT;

			/* Read what is transferred in */
			*din_8 = (u8)readl(MVEBU_SPI_A3700_DIN_ADDR);
		}

		/* Don't increment the current pointer if NULL */
		if (dout)
			dout_8++;
		if (din)
			din_8++;

		bytelen--;
	}

	return 0;
}

static int spi_xfer_non_fifo(struct spi_slave *slave, unsigned int bitlen, const void *dout,
	     void *din, unsigned long flags)
{
	register unsigned int bytelen;
	int ret;

	/* bytelen = bitlen / 8 */
	bytelen = bitlen >> 3;

	if (dout && din)
		debug("This is a duplex transfer.\n");

	/* Activate CS */
	if (flags & SPI_XFER_BEGIN) {
		debug("SPI: activate cs.\n");
		spi_cs_activate(slave);
	}

	/* Send and/or receive */
	if (dout || din) {
		ret = spi_legacy_shift_byte(bytelen, dout, din);
		if (ret)
			return ret;
	}

	/* Deactivate CS */
	if (flags & SPI_XFER_END) {
		if (!spi_poll_xfer_ready())
			return -ETIMEDOUT;

		debug("SPI: deactivate cs.\n");
		spi_cs_deactivate(slave);
	}

	return 0;
}

static void spi_set_fifo(void)
{
	unsigned int val_conf;

	val_conf = readl(MVEBU_SPI_A3700_CONF_ADDR);

	/* Always shift 4 byte at a time */
	val_conf = val_conf | MVEBU_SPI_A3700_BYTE_LEN;

	/* Set fifo mode -mode 3: CPHA = 1 and  CPOL = 1 */
	val_conf = val_conf | MVEBU_SPI_A3700_BYTE_CLK_PHA;

	val_conf = val_conf | MVEBU_SPI_A3700_CLK_POL;

	/* Enabel FIFO mode */
	val_conf |= MVEBU_SPI_A3700_FIFO_EN;

	/*
	 * Set FIFO threshold
	 * For read FIFO threshold, value 0 presents 1 data entry, which means
	 * when data in the read FIFO is equal to or greater than 1 entry,
	 * flag RFIFO_RDY_IS will be set;
	 * For write FIFO threshold, value 7 presents 7 data entry, which means
	 * when data in the write FIFO is less than or equal to 7 entry,
	 * flag WFIFO_RDY_IS will be set;
	 */
	val_conf |= 0 << MVEBU_SPI_A3700_RFIFO_THRS_BIT;
	val_conf |= 7 << MVEBU_SPI_A3700_WFIFO_THRS_BIT;

	writel(val_conf, MVEBU_SPI_A3700_CONF_ADDR);
}

static int spi_fifo_flush(void)
{
	int timeout = SPI_TIMEOUT;
	unsigned int val;

	val = readl(MVEBU_SPI_A3700_CONF_ADDR);
	val |= MVEBU_SPI_A3700_FIFO_FLUSH;
	writel(val, MVEBU_SPI_A3700_CONF_ADDR);

	while (--timeout) {
		val = readl(MVEBU_SPI_A3700_CONF_ADDR);
		if (!(val & MVEBU_SPI_A3700_FIFO_FLUSH))
			return 0;
		udelay(1);
	}
	error("spi_fifo_flush timeout\n");
	return 1;
}

static unsigned int spi_fifo_header_set(unsigned int bytelen, const void *dout, unsigned long flags)
{
	unsigned int max_instr_cnt = 1, max_addr_cnt = 3, max_dummy_cnt = 1;
	unsigned int instr_cnt = 0, addr_cnt = 0, dummy_cnt = 0, val = 0, done_len;
	unsigned char *dout_ptr = (unsigned char *)dout;

	writel(0, MVEBU_SPI_A3700_IF_INST_ADDR);
	writel(0, MVEBU_SPI_A3700_IF_ADDR);
	writel(0, MVEBU_SPI_A3700_IF_RMODE);

	if (flags & SPI_XFER_BEGIN) {
		if (bytelen <= max_instr_cnt) {
			instr_cnt = 1;
			addr_cnt = 0;
			dummy_cnt = 0;
		} else if (bytelen <= max_instr_cnt + max_addr_cnt) {
			instr_cnt = 1;
			addr_cnt = bytelen - instr_cnt;
			dummy_cnt = 0;
		} else if (bytelen <= max_instr_cnt + max_addr_cnt + max_dummy_cnt) {
			instr_cnt = 1;
			addr_cnt = 3;
			dummy_cnt = bytelen - instr_cnt - addr_cnt;
		}
		val = 0;
		val |= ((instr_cnt & MVEBU_SPI_A3700_INSTR_CNT_MASK)
			<< MVEBU_SPI_A3700_INSTR_CNT_BIT);
		val |= ((addr_cnt & MVEBU_SPI_A3700_ADDR_CNT_MASK)
			<< MVEBU_SPI_A3700_ADDR_CNT_BIT);
		val |= ((dummy_cnt & MVEBU_SPI_A3700_DUMMY_CNT_MASK)
			<< MVEBU_SPI_A3700_DUMMY_CNT_BIT);
	}

	writel(val, MVEBU_SPI_A3700_IF_HDR_CNT_ADDR);
	done_len = instr_cnt + addr_cnt + dummy_cnt;

	/* Set Instruction */
	val = 0;
	while (instr_cnt--) {
		val = (val << 8) | dout_ptr[0];
		dout_ptr++;
	}
	writel(val, MVEBU_SPI_A3700_IF_INST_ADDR);

	/* Set Address */
	val = 0;
	while (addr_cnt--) {
		val = (val << 8) | dout_ptr[0];
		dout_ptr++;
	}
	writel(val, MVEBU_SPI_A3700_IF_ADDR);

	return done_len;
}

static inline int spi_is_wfifo_full(void)
{
	u32 val;

	val = readl(MVEBU_SPI_A3700_CTRL_ADDR);
	return val & MVEBU_SPI_A3700_WFIFO_FULL;
}

static int spi_fifo_write(unsigned int buf_len, unsigned char *tx_buf)
{
	unsigned int val = 0;

	while (!spi_is_wfifo_full()) {
		/*
		 * In FIFO mode, the SPI controller is forced to work in 4-bytes mode.
		 * It always shifts 4-bytes on each write.
		 */
		if (buf_len > 4) {
			val = (tx_buf[3] << 24) | (tx_buf[2] << 16) | (tx_buf[1] << 8) | tx_buf[0];
			buf_len -= 4;
			tx_buf += 4;
		} else {
			/*
			 * If the remained buffer length is less than 4-bytes, we should pad the write buffer with
			 * all ones. So that it avoids overwrite the unexpected bytes following the last one;
			 */
			int i = 0;

			val = 0xffffffff;
			while (buf_len) {
				val &= ~(0xff << (8 * i));
				val |= *tx_buf++ << (8 * i);
				i++;
				buf_len--;
			}

			break;
		}

		writel(val, MVEBU_SPI_A3700_DOUT_ADDR);
	}

	writel(val, MVEBU_SPI_A3700_DOUT_ADDR);

	/* Return the unwritten bytes number */
	return buf_len;
}

static inline int spi_is_rfifo_empty(void)
{
	u32 val;

	val = readl(MVEBU_SPI_A3700_CTRL_ADDR);
	return val & MVEBU_SPI_A3700_RFIFO_EMPTY;
}

static int spi_fifo_read(unsigned int buf_len, unsigned char *rx_buf)
{
	unsigned int val;

	while (!spi_is_rfifo_empty()) {
		/*
		 * In FIFO mode, the SPI controller is forced to work in 4-bytes mode.
		 * It always shifts 4-bytes on each read.
		 */
		val = readl(MVEBU_SPI_A3700_DIN_ADDR);
		if (buf_len > 4) {
			rx_buf[0] = val & 0xff;
			rx_buf[1] = (val >> 8) & 0xff;
			rx_buf[2] = (val >> 16) & 0xff;
			rx_buf[3] = (val >> 24) & 0xff;
			buf_len -= 4;
			rx_buf += 4;
		} else {
			/*
			 * When remain bytes is not larger than 4, we should avoid memory overwriting
			 * and and just write the left rx buffer bytes.
			 */
			while (buf_len) {
				*rx_buf++ = val & 0xff;
				val >>= 8;
				buf_len--;
			}
			break;
		}
	}

	/* Return the unread bytes number */
	return buf_len;
}

static int spi_fifo_abort_xfer(bool force_stop)
{
	int timeout = SPI_TIMEOUT;
	unsigned int val;
	int ret = 0;

	val = readl(MVEBU_SPI_A3700_CONF_ADDR);
	if (force_stop) {
		val |= MVEBU_SPI_A3700_XFER_STOP;
		writel(val, MVEBU_SPI_A3700_CONF_ADDR);
	}

	while (--timeout) {
		val = readl(MVEBU_SPI_A3700_CONF_ADDR);
		if (!(val & MVEBU_SPI_A3700_XFER_START))
			break;
		udelay(1);
	}
	if (timeout == 0) {
		printf("spi_fifo_abort_xfer timeout\n");
		ret = 1;
	}

	spi_fifo_flush();

	if (force_stop) {
		val &= ~MVEBU_SPI_A3700_XFER_STOP;
		writel(val, MVEBU_SPI_A3700_CONF_ADDR);
	}

	return ret;
}


static int spi_xfer_fifo_read(struct spi_slave *slave, unsigned int bytelen, void *din, unsigned long flags)
{
	unsigned int val;
	int ret = 0;
	unsigned char *char_p;
	int remain_len;

	/* Clean number of bytes for instruction, address, dummy field and read mode */
	writel(0, MVEBU_SPI_A3700_IF_INST_ADDR);
	writel(0, MVEBU_SPI_A3700_IF_ADDR);
	writel(0, MVEBU_SPI_A3700_IF_RMODE);
	writel(0, MVEBU_SPI_A3700_IF_HDR_CNT_ADDR);

	/* Set read data length */
	writel(bytelen, MVEBU_SPI_A3700_IF_DIN_CNT_ADDR);
	/* Start READ transfer */
	val = readl(MVEBU_SPI_A3700_CONF_ADDR);
	val &= ~MVEBU_SPI_A3700_RW_EN;
	val |= MVEBU_SPI_A3700_XFER_START;
	writel(val, MVEBU_SPI_A3700_CONF_ADDR);

	/* Read data from spi */
	char_p = (unsigned char *)din;
	while (bytelen) {
		if (!spi_poll_fifo_read_ready()) {
			printf("spi_poll_fifo_read_ready timeout\n");
			spi_fifo_abort_xfer(true);
			return 1;
		}
		remain_len = spi_fifo_read(bytelen, char_p);
		char_p += bytelen - remain_len;
		bytelen = remain_len;
	}

	/* When read xfer finishes, force stop is not needed */
	ret = spi_fifo_abort_xfer(false);
	return ret;
}

static int spi_xfer_fifo_write(struct spi_slave *slave, unsigned int bytelen, const void *dout, unsigned long flags)
{
	unsigned int val;
	int ret = 0;
	unsigned char *char_p;
	int remain_len;
	unsigned int len_done;
	bool write_data;

	/* Set number of bytes for instruction, address, dummy field and read mode */
	len_done = spi_fifo_header_set(bytelen, dout, flags);
	bytelen -= len_done;
	write_data = (bytelen != 0);

	/* Start Write transfer */
	val = readl(MVEBU_SPI_A3700_CONF_ADDR);
	val |= (MVEBU_SPI_A3700_XFER_START | MVEBU_SPI_A3700_RW_EN);
	writel(val, MVEBU_SPI_A3700_CONF_ADDR);

	/* Write data to spi */
	char_p = (unsigned char *)dout;
	while (bytelen) {
		if (!spi_poll_fifo_write_ready()) {
			printf("spi_poll_fifo_write_ready timeout\n");
			ret = 1;
			goto error;
		}

		remain_len = spi_fifo_write(bytelen, char_p);
		char_p += bytelen - remain_len;
		bytelen = remain_len;
	}

	if (write_data) {
		/*
		 * If there are data written to the SPI device, wait until SPI_WFIFO_EMPTY is 1
		 * to wait for all data to transfer out of write FIFO.
		 */
		if (!spi_poll_fifo_write_empty()) {
			printf("spi_poll_fifo_write_empty timeout\n");
			ret = 1;
			goto error;
		}
		/*
		 * wait for spi ready for spi interface to be idle (spi is
		 * ready for a new transfer)
		 */
		if (!spi_poll_xfer_ready()) {
			printf("spi_poll_xfer_ready timeout\n");
			ret = 1;
			goto error;
		}
	}

	/* When read xfer finishes, force stop is needed */
	ret = spi_fifo_abort_xfer(true);

out:
	return ret;

error:
	spi_fifo_abort_xfer(true);
	goto out;
}

static int spi_xfer_fifo(struct spi_slave *slave, unsigned int bitlen, const void *dout,
	     void *din, unsigned long flags)
{
	int ret = 0;
	unsigned int bytelen = bitlen >> 3;

	/* Activate CS */
	if (flags & SPI_XFER_BEGIN)
		spi_cs_activate(slave);

	spi_fifo_flush();

	if (dout)
		ret = spi_xfer_fifo_write(slave, bytelen, dout, flags);
	else if (din)
		ret = spi_xfer_fifo_read(slave, bytelen, din, flags);

		/* Deactivate CS */
	if (flags & SPI_XFER_END)
		spi_cs_deactivate(slave);

	return ret;
}

/**
 * spi_init() - API implementation
 *
 * This function does nothing, all the hardware configuration
 * will be done in spi_setup_slave, but it is required by SPI skeleton.
 *
 */
void spi_init(void)
{
	/* do nothing */
}

/**
 * spi_setup_slave() - API implementation
 *
 * This function does not really touch the hardware.
 *
 * See also:	doc/driver-model/UDM-spi.txt
 *
 * Return:	0 - cool
 */
struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				  unsigned int max_hz, unsigned int mode)
{
	struct spi_slave *slave;
	u32 data;
	int node_list[CONFIG_MAX_SPI_NUM], node;
	u32 i, count;

	/* get reg_base from FDT */
	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "spi",
					   COMPAT_MVEBU_A3700_SPI, node_list, CONFIG_MAX_SPI_NUM);

	/* in FDT file, there should be only one "spi" node that are enabled,
	* which has the 'reg' attribute for register base of SPI unit */
	for (i = 0; i < count ; i++) {
		node = node_list[i];

		if (node <= 0)
			continue;

		mvebu_spi_reg_base = (unsigned long)fdt_get_regs_offs(gd->fdt_blob, node, "reg");
		mvebu_spi_max_freq = fdtdec_get_int(gd->fdt_blob, node, "spi-max-frequency", 0);
		mvebu_spi_input_clock = fdtdec_get_int(gd->fdt_blob, node, "clock-frequency", 0);
		if (fdtdec_get_bool(gd->fdt_blob, node, "fifo-mode"))
			mvebu_spi_fifo_enabled = true;
		else
			mvebu_spi_fifo_enabled = false;

		break;
	}

	if (mvebu_spi_reg_base == 100) {
		error("spi_reg_base: %lx still has Init value 100, failed to get right value\n", mvebu_spi_reg_base);
		return NULL;
	}

	if (mvebu_spi_max_freq == 0 || mvebu_spi_input_clock == 0) {
		error("%max_freq: %x, input_clock: %x, failed to get right SPI clock configuration\n",
		      mvebu_spi_max_freq, mvebu_spi_input_clock);
		return NULL;
	}

	if (!spi_cs_is_valid(bus, cs)) {
		error("%s: (bus %i, cs %i) not valid\n", __func__, bus, cs);
		return NULL;
	}

	slave = calloc(1, sizeof(struct spi_slave));
	if (!slave)
		return NULL;

	slave->bus = bus;
	slave->cs = cs;

	/* Reset SPI unit */
	data = readl(MVEBU_SPI_A3700_CONF_ADDR);
	data |= MVEBU_SPI_A3700_SRST;
	writel(data, MVEBU_SPI_A3700_CONF_ADDR);

	udelay(SPI_TIMEOUT);

	data = readl(MVEBU_SPI_A3700_CONF_ADDR);
	data &= ~MVEBU_SPI_A3700_SRST;
	writel(data, MVEBU_SPI_A3700_CONF_ADDR);

	/* flush read/write FIFO */
	if (spi_fifo_flush())
		return NULL;

	data = readl(MVEBU_SPI_A3700_CONF_ADDR);

	/* Set Prescaler */
	data = data & (~MVEBU_SPI_A3700_CLK_PRESCALE_MASK);
	/* calculate Prescaler = (spi_input_freq / spi_max_freq) */
	data = data | (mvebu_spi_input_clock / mvebu_spi_max_freq);

	writel(data, MVEBU_SPI_A3700_CONF_ADDR);

	if (!mvebu_spi_fifo_enabled)
		spi_set_legacy();
	else
		spi_set_fifo();

	return slave;
}

/**
 * spi_free_slave() - API implementation
 *
 * This function does not really touch the hardware.
 * only free the memory of slave structure.
 *
 * See also:	doc/driver-model/UDM-spi.txt
 *
 * Return:	0 - cool
 */
void spi_free_slave(struct spi_slave *slave)
{
	free(slave);
}

/**
 * spi_claim_bus() - API implementation
 *
 * According to `doc/driver-model/UDM-spi.txt`, this should do `all
 * the hardware settings'.
 *
 * This function should setup speed, mode, etc. In newer API beginning
 * Oct 2014, these setups will be converted to separate functions.
 * There is only one master on the bus, so do not need to claim or release.
 * And all the hardware configurations have been done in setup_slave routine,
 * so it is nothing here. But skeleton needs it, so have to keep it alive.
 *
 * Return:	0 - cool
 */
int spi_claim_bus(struct spi_slave *slave)
{
	return 0;
}

/**
 * spi_release_bus() - API implementation
 *
 * There is only one master on the bus, so do not need to claim or release.
 * And all the hardware configurations have been done in setup_slave routine,
 * so it is nothing here. But skeleton needs it, so have to keep it alive.
 *
 */
void spi_release_bus(struct spi_slave *slave)
{
}

/**
 * spi_cs_is_valid() - API implementation
 *
 * check if the bus and CS number is valid
 *
 * Input:
 *	bus: bus number
 *	cs: CS number
 */
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	/* we only have one bus, and 4 CS */
	if (bus == 0 && cs < CONFIG_MAX_SPI_CS_NUM)
		return 1;
	else
		return 0;
}

/**
 * spi_cs_activate() - API implementation
 *
 * Active input CS number
 *
 * Input:
 *	slave: structure for slave
 */
void spi_cs_activate(struct spi_slave *slave)
{
	u32 val_conf;

	/* enable cs */
	val_conf = readl(MVEBU_SPI_A3700_CTRL_ADDR);
	val_conf = val_conf | (MVEBU_SPI_A3700_SPI_EN_0 << slave->cs);
	writel(val_conf, MVEBU_SPI_A3700_CTRL_ADDR);
}

/**
 * spi_cs_activate() - API implementation
 *
 * Deactive input CS number
 *
 * Input:
 *	slave: structure for slave
 */
void spi_cs_deactivate(struct spi_slave *slave)
{
	u32 val_conf;

	/* disable cs */
	val_conf = readl(MVEBU_SPI_A3700_CTRL_ADDR);
	val_conf = val_conf & (~(MVEBU_SPI_A3700_SPI_EN_0 << slave->cs));
	writel(val_conf, MVEBU_SPI_A3700_CTRL_ADDR);
}

/**
 * spi_xfer() - API function to start SPI transfer
 *
 * This interface is designed by U-Boot. Upper level drivers, such as
 * sf.c use this API to do the real transfer work. The command sspi
 * also uses this API directly.
 *
 * This function should send the content in *dout, write incoming data
 * in *din, and return 0 if no issues.
 *
 * Armada3700 SPI module supports two modes: legacy mode and non-legacy
 * mode. You can choose to use Legacy mode or Non-Legacy mode at
 * compilation. For details, please see the sub-routines.
 *
 * For either mode, the driver does not need to tweak the SPI clock
 * one by one. Data is transferred either by one byte or two bytes.
 * That is, the SPI module will generate 8 cycles or 16 cycles at a
 * time. The driver should, however, guarantee that the SPI clock freq
 * should not exceed the max value from slave device's datasheet. In
 * this driver, this job is done in spi_setup_slave().
 *
 * Legacy mode is closer to the original way that SPI works. The SPI
 * module will start transfer upon writing to Data Out register. To
 * receive data, simply send out dummy bytes 0x00 and incoming data
 * will be shifted in Data In register.
 *
 * Return:	0 if no issues
 */
int spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
	     void *din, unsigned long flags)
{
	if (mvebu_spi_fifo_enabled)
		return spi_xfer_fifo(slave, bitlen, dout, din, flags);
	else
		return spi_xfer_non_fifo(slave, bitlen, dout, din, flags);
}

