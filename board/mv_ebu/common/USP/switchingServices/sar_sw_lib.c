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

#include "sar_sw_lib.h"

/* Offset between PCA_9555 output port reg and configuration reg */
#define PCA_9555_CONF_OFFSET	4

static int do_sar_list(struct satr_info *s_cmd_i)
{
	int valid_bits_nr = hweight32(s_cmd_i->bit_mask);

	printf("%s (0x%x) [%d:%d]: determines the %s\n", s_cmd_i->name,
	       s_cmd_i->twsi_addr, s_cmd_i->field_offs,
	       s_cmd_i->field_offs + valid_bits_nr - 1, *s_cmd_i->help_info);

	return 0;
}

static int get_satr_cmd_id(struct satr_info *sar_info, const char *cmd, uint satr_max)
{
	int i;

	for (i = 0; i < satr_max; i++) {
		if (strcmp(cmd, sar_info[i].name) == 0)
			return i;
	}

	return -1;
}

static int do_sar_read(struct satr_info *s_cmd_i)
{
	uint satr_reg;

	satr_reg = tread_msys(s_cmd_i->twsi_addr, s_cmd_i->twsi_reg,
			      s_cmd_i->moreThan256);
	if (satr_reg == MV_ERROR)
		return -1;

	satr_reg = (satr_reg >> s_cmd_i->field_offs) & s_cmd_i->bit_mask;

	printf("Remote switch S@R %s = 0x%02x\n", s_cmd_i->name, satr_reg);

	return 0;
}

static int do_sar_write(struct satr_info *s_cmd_i, uint data)
{
	int err;
	uint satr_reg;

	satr_reg = tread_msys(s_cmd_i->twsi_addr, s_cmd_i->twsi_reg,
			      s_cmd_i->moreThan256);
	if (satr_reg == MV_ERROR)
		return -1;

	satr_reg &= (~(s_cmd_i->bit_mask << s_cmd_i->field_offs));
	satr_reg |= ((data & s_cmd_i->bit_mask) << s_cmd_i->field_offs);

	err = twrite_msys(s_cmd_i->twsi_addr, s_cmd_i->twsi_reg,
			  (MV_U8)satr_reg, s_cmd_i->moreThan256);
	if (err == MV_ERROR)
		return -1;

	/* For SERDES configuration which is done through PCA9555, setting the
	 * configuration port is required, since at reset all PCA's pins are
	 * inputs
	 */
	if (s_cmd_i->pca9555)
		err = twrite_msys(s_cmd_i->twsi_addr, s_cmd_i->twsi_reg +
				  PCA_9555_CONF_OFFSET, 0x0, MV_FALSE);

	if (err == MV_ERROR)
		return -1;

	return 0;
}

static int do_sar_write_default(struct satr_info *sar_info, uint satr_max)
{
	int i, err = 0;

	for (i = 0; i < satr_max; i++) {
		printf("writing def val 0x%x to %s\n",
			sar_info[i].default_val,
			sar_info[i].name);

		err = do_sar_write(&sar_info[i],
				  sar_info[i].default_val);
		if (err == MV_ERROR)
			return -1;
	}

	return err;
}

static void print_list_help(struct satr_info *sar_info, uint satr_max)
{
	int i;

	for (i = 0; i < satr_max; i++)
		printf("SatR list %s - print list of \'%s\' available options\n",
			sar_info[i].name, sar_info[i].name);

	printf("\n");
}

static void print_read_help(struct satr_info *sar_info, uint satr_max)
{
	int i;

	for (i = 0; i < satr_max; i++)
		printf("SatR read %s - read %s\n", sar_info[i].name,
			sar_info[i].name);

	printf("SatR read - dump all available SatR\n");
	printf("\n");
}

static void print_write_help(struct satr_info *sar_info, uint satr_max)
{
	int i;

	for (i = 0; i < satr_max; i++)
		printf("SatR write %s <val> - write <val> to %s\n",
			sar_info[i].name, sar_info[i].name);

	printf("SatR write default - restore default switch SatR settings\n");

	printf("\n");
}

static int do_sar_dump(struct satr_info *sar_info, uint satr_max)
{
	int i;

	for (i = 0; i < satr_max; i++)
		do_sar_read(&sar_info[i]);

	return 0;
}

static int get_max_sar_options(struct satr_info *sar_info)
{
	int i;

	for (i = 0; i < 100; i++) {
		if (strcmp("LAST", sar_info[i].name) == 0)
			return i;
	}

	return -1;
}

int do_sar_switch(int argc, char *const argv[], struct satr_info *sar_info)
{
	uint data;
	const char *cmd;
	uint satr_id, sar_max;
	struct satr_info *satr_cmd_i;

	sar_max = get_max_sar_options(sar_info);
	if (sar_max == -1)
		printf("%s: wrong satr info\n", __func__);


	/* need at least tree arguments for all cmds except read */
	if ((argc < 2) || (argc < 3 && strcmp(argv[1], "read")))
		goto usage;

	cmd = argv[1];

	if (mvBoardEnableExternalI2C() == MV_ERROR) {
		printf("AMC external twsi channel not enabled\n");
		return 1;
	}

	if (check_twsi_msys() == MV_ERROR) {
		printf("AMC external twsi channel not connected\n");
		return 1;
	}

	/* get SatR_cmd_id */
	satr_id = get_satr_cmd_id(sar_info, argv[2], sar_max);
	if (satr_id == -1) {
		if ((strcmp(cmd, "read") == 0) && (argc < 3))
			return do_sar_dump(sar_info, sar_max);
		else if ((strcmp(cmd, "write") == 0) && !strcmp(argv[2], "default"))
			return do_sar_write_default(sar_info, sar_max);
		else {
			printf("unknown SatR cmd %s\n", argv[2]);
			return 1;
		}
	}

	satr_cmd_i = &sar_info[satr_id];

	if (strcmp(cmd, "list") == 0)
		return do_sar_list(satr_cmd_i);

	else if (strcmp(cmd, "read") == 0)
		return do_sar_read(satr_cmd_i);

	else if (strcmp(cmd, "write") == 0) {
		data = simple_strtoul(argv[3], NULL, 16);
		if (do_sar_write(satr_cmd_i, data) == 0)
			return do_sar_read(satr_cmd_i);

		return -1;
	}

usage:
	printf("\n");
	printf("Remote Switch Sample At Reset sub-system\n\n");

	print_list_help(sar_info, sar_max);
	print_read_help(sar_info, sar_max);
	print_write_help(sar_info, sar_max);

	return 1;
}
