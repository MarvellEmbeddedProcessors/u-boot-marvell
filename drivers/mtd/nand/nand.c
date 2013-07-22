/*
 * (C) Copyright 2005
 * 2N Telekomunikace, a.s. <www.2n.cz>
 * Ladislav Michl <michl@2n.cz>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
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
 */
#include <config.h>
#include <common.h>
#include <nand.h>
#include <errno.h>

#ifndef CONFIG_SYS_NAND_BASE_LIST
#define CONFIG_SYS_NAND_BASE_LIST { CONFIG_SYS_NAND_BASE }
#endif

DECLARE_GLOBAL_DATA_PTR;

int nand_curr_device = -1;


nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];

#ifndef CONFIG_SYS_NAND_SELF_INIT
static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];
static ulong base_address[CONFIG_SYS_MAX_NAND_DEVICE] = CONFIG_SYS_NAND_BASE_LIST;
#endif

static char dev_name[CONFIG_SYS_MAX_NAND_DEVICE][8];

static unsigned long total_nand_size; /* in kiB */

/* Register an initialized NAND mtd device with the U-Boot NAND command. */
int nand_register(int devnum)
{
	struct mtd_info *mtd;

	if (devnum >= CONFIG_SYS_MAX_NAND_DEVICE)
		return -EINVAL;

	mtd = &nand_info[devnum];

	sprintf(dev_name[devnum], "nand%d", devnum);
	mtd->name = dev_name[devnum];

#ifdef CONFIG_MTD_DEVICE
	/*
	 * Add MTD device so that we can reference it later
	 * via the mtdcore infrastructure (e.g. ubi).
	 */
	add_mtd_device(mtd);
#endif

	total_nand_size += mtd->size / 1024;

	if (nand_curr_device == -1)
		nand_curr_device = devnum;

	return 0;
}

#ifndef CONFIG_SYS_NAND_SELF_INIT
static void nand_init_chip(int i)
{
	struct mtd_info *mtd = &nand_info[i];
	struct nand_chip *nand = &nand_chip[i];
	ulong base_addr = base_address[i];
	int maxchips = CONFIG_SYS_NAND_MAX_CHIPS;

	if (maxchips < 1)
		maxchips = 1;

	mtd->priv = nand;
	nand->IO_ADDR_R = nand->IO_ADDR_W = (void  __iomem *)base_addr;

	if (board_nand_init(nand))
		return;

	if (nand_scan(mtd, maxchips))
		return;

	nand_register(i);
}
#endif

void nand_init(void)
{
#ifdef CONFIG_SYS_NAND_SELF_INIT
	board_nand_init();
#else
	int i;

	for (i = 0; i < CONFIG_SYS_MAX_NAND_DEVICE; i++)
		nand_init_chip(i);
#endif
#if defined(MV_NAND_GANG_MODE)
	printf("%u MiB - Gang\n", total_nand_size / 1024);
#else
	printf("%lu MiB\n", total_nand_size / 1024);
#endif

#ifdef CONFIG_SYS_NAND_SELECT_DEVICE
	/*
	 * Select the chip in the board/cpu specific driver
	 */
	board_nand_select_device(nand_info[nand_curr_device].priv, nand_curr_device);
#endif
}
#if defined(CONFIG_ENV_IS_IN_NAND)
int nand_get_env_offs(void) 
{
	size_t offset = 0;

#if defined(CONFIG_SKIP_BAD_BLOCK)
	int i = 0;
	int sum = 0;
	size_t blocksize;
	blocksize = nand_info[0].erasesize;

	/* Find U-Boot start */
	while(i * blocksize < nand_info[0].size) {
		if (!nand_block_isbad(&nand_info[0], (i * blocksize)))
			sum += blocksize;
		else {
			sum = 0;
			offset = (i + 1) * blocksize;
		}
		i++;
		if (sum >= CONFIG_UBOOT_SIZE)
			break;
	}

	offset += CONFIG_UBOOT_SIZE;

	/* Find Env start */
        sum = 0;
        while(i * blocksize < nand_info[0].size) {
                if (!nand_block_isbad(&nand_info[0], (i * blocksize)))
                        sum += blocksize;
                else {
                        sum = 0;
                        offset = (i + 1) * blocksize;
                }
                i++;
                if (sum >= CONFIG_ENV_RANGE)
                        break;

        }
#else
	offset = CONFIG_UBOOT_SIZE;
#endif

	/* Align U-Boot size to currently used blocksize */
	offset = ( (offset + (blocksize - 1)) & (~(blocksize-1)) );
	return offset;
}
#endif

