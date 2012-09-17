/*
 * Driver for Marvell SDIO/MMC controller
 */

#include <common.h>
#include <malloc.h>
#include <part.h>
#include <mmc.h>

#include "mrvl_mmc.h"
#define MRVL_MMC_MAKE_CMD(c, f) (((c & 0xff) << 8) | (f & 0xff))


static int mrvl_mmc_setup_data(struct mmc_data *data)
{
	if (data->flags & MMC_DATA_READ) {
		SDIO_REG_WRITE16(SDIO_SYS_ADDR_LOW,((ulong)(data->dest)) & 0xffff);
		SDIO_REG_WRITE16(SDIO_SYS_ADDR_HI,(((ulong)data->dest) >> 16) & 0xffff);
	} else {
		SDIO_REG_WRITE16(SDIO_SYS_ADDR_LOW,((ulong)(data->src)) & 0xffff);
		SDIO_REG_WRITE16(SDIO_SYS_ADDR_HI,(((ulong)data->src) >> 16) & 0xffff);
	}

	SDIO_REG_WRITE16(SDIO_BLK_SIZE, data->blocksize);
	SDIO_REG_WRITE16(SDIO_BLK_COUNT, data->blocks);

	return 0;
}

static int mrvl_mmc_send_cmd (struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	int	timeout = 10;
	ushort waittype = 0;
	int err = 0 ;
	ushort response[8], resp_indx = 0;
	ushort resptype = 0;
	ushort xfertype = 0;

#ifdef DEBUG
	printf("cmd [0x%x] resp_type[0x%x] arg[0x%x]\n", cmd->cmdidx, cmd->resp_type, cmd->cmdarg);
#endif

	/* clear status */
	SDIO_REG_WRITE16(SDIO_NOR_INTR_STATUS, 0xffff);
	SDIO_REG_WRITE16(SDIO_ERR_INTR_STATUS, 0xffff);

	/* Checking if card is busy */
	while ((SDIO_REG_READ16(SDIO_PRESENT_STATE0) & CARD_BUSY)) {
		if (timeout == 0) {
			printf("MRVL MMC: card busy!\n");
			return -1;
		}
		timeout--;
		udelay(1000);
	}

	/* Set up for a data transfer if we have one */
	if (data) {
		err = mrvl_mmc_setup_data(data);
		if(err)
			return err;
	}

	/* Analyzing resptype/xfertype/waittype for the command */
	if (cmd->resp_type & MMC_RSP_BUSY)
		resptype |= SDIO_CMD_RSP_48BUSY;
	else if (cmd->resp_type & MMC_RSP_136)
		resptype |= SDIO_CMD_RSP_136;
	else if (cmd->resp_type & MMC_RSP_PRESENT)
		resptype |= SDIO_CMD_RSP_48;
	else
		resptype |= SDIO_CMD_RSP_NONE;

	if (cmd->resp_type & MMC_RSP_CRC)
		resptype |= SDIO_CMD_CHECK_CMDCRC;

	if (cmd->resp_type & MMC_RSP_OPCODE)
		resptype |= SDIO_CMD_INDX_CHECK;

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		resptype |= SDIO_UNEXPECTED_RESP;
		waittype |= SDIO_NOR_UNEXP_RSP;
	}

	if (data) {
		resptype |= SDIO_CMD_DATA_PRESENT | SDIO_CMD_CHECK_DATACRC16;
		xfertype |= SDIO_XFER_MODE_HW_WR_DATA_EN;
		if (data->flags & MMC_DATA_READ) {
			xfertype |= SDIO_XFER_MODE_TO_HOST;
			waittype = SDIO_NOR_DMA_INI;
		} else
			waittype |= SDIO_NOR_XFER_DONE;
	} else {
		waittype |= SDIO_NOR_CMD_DONE;
	}

	/* Setting cmd arguments */
	SDIO_REG_WRITE16(SDIO_ARG_LOW, (ushort)(cmd->cmdarg & 0xffff));
	SDIO_REG_WRITE16(SDIO_ARG_HI,  (ushort)(cmd->cmdarg >> 16) );

	/* Setting Xfer mode */
	SDIO_REG_WRITE16(SDIO_XFER_MODE, xfertype);

	/* Sending command */
	SDIO_REG_WRITE16(SDIO_CMD, MRVL_MMC_MAKE_CMD(cmd->cmdidx, resptype));

	/* Waiting for completion */
	timeout = 1000000;

	while (!((SDIO_REG_READ16(SDIO_NOR_INTR_STATUS)) & waittype)) {
		udelay(10);
		if (SDIO_REG_READ16(SDIO_NOR_INTR_STATUS) & SDIO_NOR_ERROR) {
#ifdef DEBUG
			printf("mrvl_mmc_send_cmd: error! cmd : %d, err reg: %04x\n", cmd->cmdidx, SDIO_REG_READ16(SDIO_ERR_INTR_STATUS));
#endif
			if (SDIO_REG_READ16(SDIO_ERR_INTR_STATUS) & (SDIO_ERR_CMD_TIMEOUT | SDIO_ERR_DATA_TIMEOUT))
				return TIMEOUT;
			return COMM_ERR;
		}
		timeout--;
		if (timeout <= 0) {
			printf("MRVL MMC: command timed out\n");
			return TIMEOUT;
		}
	}

	/* Handling response */
	for (resp_indx = 0 ; resp_indx < 8; resp_indx++)
		response[resp_indx] = SDIO_REG_READ16(SDIO_RSP(resp_indx));

	/* Copy the response to the response buffer */
	if (cmd->resp_type & MMC_RSP_PRESENT) {
		cmd->response[0] = ((response[2] & 0x3f) << (8 - 8)) |
				((response[1] & 0xffff) << (14 - 8)) |
				((response[0] & 0x3ff) << (30 - 8));
		if (cmd->resp_type & MMC_RSP_136) {
			cmd->response[3] = ((response[7] & 0x3fff) << 8)	|
					((response[6] & 0x3ff) << 22);
			cmd->response[2] = ((response[6] & 0xfc00) >> 10)	|
					((response[5] & 0xffff) << 6)	|
					((response[4] & 0x3ff) << 22);
			cmd->response[1] = ((response[4] & 0xfc00) >> 10)	|
					((response[3] & 0xffff) << 6)	|
					((response[2] & 0x3ff) << 22);
			cmd->response[0] = ((response[2] & 0xfc00) >> 10)	|
					((response[1] & 0xffff) << 6)	|
					((response[0] & 0x3ff) << 22);
		}
	}

#ifdef DEBUG
	printf("resp index[0x%x] ", response[0] >> 10);
	printf("[0x%x] ", cmd->response[0]);
	printf("[0x%x] ", cmd->response[1]);
	printf("[0x%x] ", cmd->response[2]);
	printf("[0x%x] ", cmd->response[3]);
	printf("\n");
#endif

	return 0;
}

static void mrvl_mmc_set_clk(unsigned long clk)
{
	unsigned int m;

	m = MRVL_MMC_BASE_FAST_CLOCK/(2*clk) - 1;
#ifdef DEBUG
	printf("mrvl_mmc_set_clk: dividor = 0x%x clock=%d\n", m, clk);
#endif
	SDIO_REG_WRITE32(SDIO_CLK_DIV, m & 0x7ff);
	udelay(10*1000);
}

static void mrvl_mmc_set_bus(unsigned int bus)
{
	ushort reg;

#ifdef DEBUG
	printf("mrvl_mmc_set_bus: bus = 0x%d\n", bus);
#endif
	reg = SDIO_REG_READ16(SDIO_HOST_CTRL);
	reg &= ~(1 << 9);
	switch (bus) {
		case 4:
			reg |= SDIO_HOST_CTRL_DATA_WIDTH_4_BITS;
			break;
		case 1:
		default:
			reg |= SDIO_HOST_CTRL_DATA_WIDTH_1_BIT;
	}
	SDIO_REG_WRITE16(SDIO_HOST_CTRL, reg);
	udelay(10*1000);
}
static void mrvl_mmc_set_ios(struct mmc *mmc)
{
#ifdef DEBUG
	printf("bus[%d] clock[%d]\n", mmc->bus_width, mmc->clock);
#endif
	mrvl_mmc_set_bus(mmc->bus_width);
	mrvl_mmc_set_clk(mmc->clock);
}

static int mrvl_mmc_init(struct mmc *mmc)
{
#ifdef DEBUG
	printf("mrvl_mmc_init\n");
#endif

	/* Setting host parameters */
	SDIO_REG_WRITE16(SDIO_HOST_CTRL, SDIO_HOST_CTRL_TMOUT_EN |
					SDIO_HOST_CTRL_TMOUT(0xf) |
					SDIO_HOST_CTRL_DATA_WIDTH_1_BIT |
					SDIO_HOST_CTRL_BIG_ENDIAN |
					SDIO_HOST_CTRL_PUSH_PULL_EN |
					SDIO_HOST_CTRL_CARD_TYPE_MEM_ONLY );
	SDIO_REG_WRITE16(SDIO_CLK_CTRL, 0);

	/* enable status */
	SDIO_REG_WRITE16(SDIO_NOR_STATUS_EN, 0xffff);
	SDIO_REG_WRITE16(SDIO_ERR_STATUS_EN, 0xffff);

	/* disable interrupts */
	SDIO_REG_WRITE16(SDIO_NOR_INTR_EN, 0);
	SDIO_REG_WRITE16(SDIO_ERR_INTR_EN, 0);

	/* SW reset */
	SDIO_REG_WRITE16(SDIO_SW_RESET,0x100);
	udelay(100000);
	return 0;
}

int mrvl_mmc_initialize(bd_t *bis)
{
	struct mmc *mmc = NULL;

	mmc = malloc(sizeof(struct mmc));
	if (!mmc)
		return -1;

	sprintf(mmc->name, "MRVL_MMC");
	mmc->send_cmd = mrvl_mmc_send_cmd;
	mmc->set_ios = mrvl_mmc_set_ios;
	mmc->init = mrvl_mmc_init;

	mmc->host_caps = MMC_MODE_4BIT | MMC_MODE_HS;
	mmc->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->f_max = 50000000;
	mmc->f_min = 250000;
	mmc->block_dev.part_type = PART_TYPE_DOS;

	mmc_register(mmc);

	return 0;
}
