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
#include <asm/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

/* to use global variables before u-boot relocation, initialize it to something !=0 */
unsigned long mvebu_spi_reg_base = 100;
#define MVEBU_SPI_BASE         mvebu_spi_reg_base

unsigned int mvebu_spi_input_clock;
unsigned int mvebu_spi_max_freq;

#define CONFIG_MAX_SPI_NUM	8
#define CONFIG_MAX_SPI_CS_NUM	4

#define SPI_TIMEOUT		10000

#define MVEBU_SPI_A3700_CTRL_OFFSET         0x00
#define MVEBU_SPI_A3700_CONF_OFFSET         0x04
#define MVEBU_SPI_A3700_DOUT_OFFSET         0x08
#define MVEBU_SPI_A3700_DIN_OFFSET          0x0c

#define MVEBU_SPI_A3700_CTRL_ADDR           (MVEBU_SPI_BASE + MVEBU_SPI_A3700_CTRL_OFFSET)
#define MVEBU_SPI_A3700_CONF_ADDR           (MVEBU_SPI_BASE + MVEBU_SPI_A3700_CONF_OFFSET)
#define MVEBU_SPI_A3700_DOUT_ADDR           (MVEBU_SPI_BASE + MVEBU_SPI_A3700_DOUT_OFFSET)
#define MVEBU_SPI_A3700_DIN_ADDR            (MVEBU_SPI_BASE + MVEBU_SPI_A3700_DIN_OFFSET)

#define MVEBU_SPI_A3700_XFER_RDY				BIT1
#define MVEBU_SPI_A3700_FIFO_FLUSH			BIT9
#define MVEBU_SPI_A3700_BYTE_LEN				BIT5
#define MVEBU_SPI_A3700_BYTE_CLK_PHA			BIT6
#define MVEBU_SPI_A3700_CLK_POL				BIT7
#define MVEBU_SPI_A3700_FIFO_EN				BIT17
#define MVEBU_SPI_A3700_SPI_EN_0				BIT16
#define MVEBU_SPI_A3700_CLK_PRESCALE_BIT			0
#define MVEBU_SPI_A3700_CLK_PRESCALE_MASK		(0x1F << MVEBU_SPI_A3700_CLK_PRESCALE_BIT)

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
static inline int poll_spi_xfer_ready(void)
{
	unsigned int timeout = SPI_TIMEOUT;

	return if_spi_flags_is_set(MVEBU_SPI_A3700_CTRL_ADDR, MVEBU_SPI_A3700_XFER_RDY, timeout);
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

	/* Set legacy mode */
	val_conf = val_conf & (~MVEBU_SPI_A3700_BYTE_CLK_PHA);

	/* Set CPOL = 0 */
	val_conf = val_conf & (~MVEBU_SPI_A3700_CLK_POL);

	/* Set Prescaler */
	val_conf = val_conf & (~MVEBU_SPI_A3700_CLK_PRESCALE_MASK);

	/* calculate Prescaler = (spi_input_freq / spi_max_freq) */
	val_conf = val_conf | (mvebu_spi_input_clock / mvebu_spi_max_freq);

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
		if (!poll_spi_xfer_ready())
			return -ETIMEDOUT;

		if (dout)
			pending_dout = (u32)*dout_8;
		else
			pending_dout = (u32)dummy_dout;

		/* Trigger the xfer */
		writel(pending_dout, MVEBU_SPI_A3700_DOUT_ADDR);

		if (din) {
			if (!poll_spi_xfer_ready())
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
	u32 timeout = SPI_TIMEOUT;
	u32 data;
#ifdef CONFIG_MVEBU_GPIO
	struct fdt_gpio_state output_enable_gpio;
#endif
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

#ifdef CONFIG_MVEBU_GPIO
		/* set hiden GPIO setting for SPI
		 * in north_bridge_test_pin_out_en register 13804(high GPIO register),
		 * bit 28(since it's in high regsiter, so the local pin number in bank is 60)
		 * which is hidden bit which is reserved in function spec
		 * (the corresponding gpio is ARMADA_3700_GPIO(BANK_0, 60) in a3700 spi dts node)
		 * is the one which enables CS, CLK pin to be
		 * output, need to set it to 1.
		 * normally, it is needed only in UART boot mode,
		 * but after trying all other modes, it is OK to set it.
		 * later, we could read the SAR register, and do not
		 * set it in other boot mode.
		 */
		fdtdec_decode_gpio(gd->fdt_blob, node, "output-enable-gpio", &output_enable_gpio);
		fdtdec_setup_gpio(&output_enable_gpio);
		if (fdt_gpio_isvalid(&output_enable_gpio)) {
			int val;

			/* Set to output enbale GPIO in output mode with low level by default */
			val = output_enable_gpio.flags & FDT_GPIO_ACTIVE_LOW ? 1 : 0;
			gpio_direction_output(output_enable_gpio.gpio, val);
		}
#else
		printf("ERROR: CS, CLK pins are not enabled in output mode, need to implement gpio in SOC code\n");
#endif
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

	/* flush read/write FIFO */
	data = readl(MVEBU_SPI_A3700_CONF_ADDR);
	writel((data | MVEBU_SPI_A3700_FIFO_FLUSH), MVEBU_SPI_A3700_CONF_ADDR);

	do {
		data = readl(MVEBU_SPI_A3700_CONF_ADDR);
		if (timeout-- == 0)
			return NULL;
	} while (data & MVEBU_SPI_A3700_FIFO_FLUSH);

	/* set SPI polarity
	 * 0: Serial interface clock is low when inactive
	 * 1: Serial interface clock is high when inactive
	 */
	if (mode & SPI_CPOL)
		data |= MVEBU_SPI_A3700_CLK_POL;

	/* disable FIFO mode */
	data &= ~MVEBU_SPI_A3700_FIFO_EN;
	writel(data, MVEBU_SPI_A3700_CONF_ADDR);

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

	/* Settings SPI controller to be working in legacy mode, which
	  * means use only DO pin (I/O 1) for Data Out, and DI pin (I/O 0) for Data In.
	  */
	spi_set_legacy();

	/* Send and/or receive */
	if (dout || din) {
		ret = spi_legacy_shift_byte(bytelen, dout, din);
		if (ret)
			return ret;
	}

	/* Deactivate CS */
	if (flags & SPI_XFER_END) {
		if (!poll_spi_xfer_ready())
			return -ETIMEDOUT;

		debug("SPI: deactivate cs.\n");
		spi_cs_deactivate(slave);
	}

	return 0;
}

