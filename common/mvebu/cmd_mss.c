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
#include <asm/io.h>
#include <command.h>

#define MSS_DMA_SRCBR(base)	(base + 0xC0)
#define MSS_DMA_DSTBR(base)	(base + 0xC4)
#define MSS_DMA_CTRLR(base)	(base + 0xC8)
#define MSS_M3_RSTCR(base)	(base + 0xFC)
#define MSS_AEBR(base)		(base + 0x160)

#define MSS_DMA_CTRLR_SIZE_OFFSET		(0)
#define MSS_DMA_CTRLR_SIZE_MASK		    (0xFF)
#define MSS_DMA_CTRLR_REQ_OFFSET		(15)
#define MSS_DMA_CTRLR_REQ_SET		    (1)
#define MSS_DMA_CTRLR_ACK_OFFSET		(12)
#define MSS_DMA_CTRLR_ACK_MASK		    (0x1)
#define MSS_DMA_CTRLR_ACK_BUSY		    (0)
#define MSS_DMA_CTRLR_ACK_READY		    (1)
#define MSS_M3_RSTCR_RST_OFFSET     (0)
#define MSS_M3_RSTCR_RST_ON         (0)
#define MSS_M3_RSTCR_RST_OFF        (1)
#define MSS_AEBR_MASK				0xFFF

#define MSS_DMA_TIMEOUT             1000
#define MSS_EXTERNAL_SPACE          0x50000000
#define MSS_EXTERNAL_ACCESS_BIT     28

int mss_boot(u32 size, u32 srcAddr, uintptr_t  mss_regs)
{
	u32 i, loop_num, timeout;

	printf("Loading MSS image from address 0x%08X size 0x%x to MSS at 0x%lx...",
		(u32)srcAddr, size, mss_regs);

	/* load image to MSS RAM using DMA */
	loop_num = (size / 128) + (((size & 127) == 0) ? 0 : 1);

	/* set AXI External Address Bus extension */
	writel(((srcAddr >> MSS_EXTERNAL_ACCESS_BIT) & MSS_AEBR_MASK), MSS_AEBR(mss_regs));

	for (i = 0; i < loop_num; i++) {
		/* write destination and source addresses */
		writel(MSS_EXTERNAL_SPACE | (srcAddr + (i * 128)), MSS_DMA_SRCBR(mss_regs));
		writel((i * 128), MSS_DMA_DSTBR(mss_regs));

		/* set the DMA control register */
		writel(((MSS_DMA_CTRLR_REQ_SET << MSS_DMA_CTRLR_REQ_OFFSET) |
				(128 << MSS_DMA_CTRLR_SIZE_OFFSET)), MSS_DMA_CTRLR(mss_regs));

		/* Poll DMA_ACK at MSS_DMACTLR until it is ready */
		timeout = MSS_DMA_TIMEOUT;
		while (timeout) {
			if ((readl(MSS_DMA_CTRLR(mss_regs)) >> MSS_DMA_CTRLR_ACK_OFFSET & MSS_DMA_CTRLR_ACK_MASK)
				== MSS_DMA_CTRLR_ACK_READY) {
				break;
			}
			udelay(1);
			timeout--;
		}
		if (timeout == 0) {
			printf("\nDMA failed to load MSS image\n");
			return 1;
		}
	}

	/* Release M3 from reset */
	writel((MSS_M3_RSTCR_RST_OFF << MSS_M3_RSTCR_RST_OFFSET), MSS_M3_RSTCR(mss_regs));

	return 0;
}


int do_mss_boot(cmd_tbl_t *cmdtp, int flag, int argc,
						char *const argv[])
{
	u32 srcAddr, size, mss_regs;

	if (argc != 4) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	/* Get source address*/
	srcAddr = simple_strtoul(argv[1], NULL, 16);

	/* Get image size */
	size = simple_strtoul(argv[2], NULL, 16);

	/* Get mss base address */
	mss_regs = simple_strtoul(argv[3], NULL, 16);

	return mss_boot(size, srcAddr, (uintptr_t)mss_regs);
}

U_BOOT_CMD(
	mss_boot,     4,      1,       do_mss_boot,
	"mss_boot - Loads MSS code into CM3 ID RAM and release from reset\n",
	"<image_src_address> <image_size> <mss_reg_base>\n"
	"	- Loads MSS code into CM3 ID RAM and release CM3 from reset\n"
	"\n"
	"Parameters:\n"
	"image_src_address	memory address of image to load to MSS\n"
	"image_size		image size\n"
	"mss_reg_base		base address of MSS registers\n"
);

