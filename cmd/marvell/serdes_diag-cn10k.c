// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2021 Marvell
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <command.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <linux/arm-smccc.h>
#include <linux/delay.h>
#include <asm/arch/smc.h>
#include <net.h>

#define DEFINE_STR_2_ENUM_FUNC(_conv_arr)				\
static inline int _conv_arr ## _str2enum(const char *str)		\
{									\
	size_t idx;							\
	size_t len = ARRAY_SIZE(_conv_arr);				\
									\
	if (!str)							\
		return -1;						\
									\
	for (idx = 0; idx < len; idx++) {				\
		if (!strcmp(_conv_arr[idx].s, str))			\
			return _conv_arr[idx].e;			\
	}								\
									\
	return -1;							\
}

static struct {
	enum prbs_subcmd e;
	const char *s;
} prbs_subcmd[] = {
	{PRBS_START, "start"},
	{PRBS_SHOW, "show"},
	{PRBS_CLEAR, "clear"},
	{PRBS_STOP, "stop"},
	{PRBS_INJECT, "inject"},
};

DEFINE_STR_2_ENUM_FUNC(prbs_subcmd)

enum prbs_optcmd {
	PRBS_CHECKER,
	PRBS_GENERATOR,
	PRBS_BOTH,
};

static struct {
	enum prbs_optcmd e;
	const char *s;
} prbs_optcmd[] = {
	{PRBS_CHECKER, "check"},
	{PRBS_GENERATOR, "gen"},
	{PRBS_BOTH, "both"},
};

DEFINE_STR_2_ENUM_FUNC(prbs_optcmd)

enum prbs_pattern {
	PRBS_7 = 7,
	PRBS_9 = 9,
	PRBS_11 = 11,
	PRBS_15 = 15,
	PRBS_16 = 16,
	PRBS_23 = 23,
	PRBS_31 = 31,
	PRBS_32 = 32
};

struct prbs_error_stats {
	u64 total_bits;
	u64 error_bits;
	int locked;
};

enum lpbk_type {
	LPBK_TYPE_NONE = 0,
	LPBK_TYPE_NEA,
	LPBK_TYPE_NED,
	LPBK_TYPE_FED
};

static const char *const lpbk_type[] = {
	"No Loopback",
	"NEA",
	"NED",
	"FED"
};

enum tx_param {
	TX_PARAM_PRE2,
	TX_PARAM_PRE1,
	TX_PARAM_POST,
	TX_PARAM_MAIN,
};

struct tx_eq_params {
	u16 pre2;
	u16 pre1;
	u16 post;
	u16 main;
};

static struct {
	enum tx_param e;
	const char *s;
} tx_param[] = {
	{TX_PARAM_PRE2, "pre2"},
	{TX_PARAM_PRE1, "pre1"},
	{TX_PARAM_MAIN, "main"},
	{TX_PARAM_POST, "post"},
};

DEFINE_STR_2_ENUM_FUNC(tx_param)

#define DFE_TAPS_NUM 24
#define CTLE_PARAMS_NUM 13

const char *dfe_taps_names[] = {
	"dfe_dc:\t\t",
	"dfe_vre:\t",
	"dfe_f0:\t\t",
	"dfe_f1:\t\t",
	"dfe_f2:\t\t",
	"dfe_f3:\t\t",
	"dfe_f4:\t\t",
	"dfe_f5:\t\t",
	"dfe_f6:\t\t",
	"dfe_f7:\t\t",
	"dfe_f8:\t\t",
	"dfe_f9:\t\t",
	"dfe_f10:\t",
	"dfe_f11:\t",
	"dfe_f12:\t",
	"dfe_f13:\t",
	"dfe_f14:\t",
	"dfe_f15:\t",
	"dfe_ff0:\t",
	"dfe_ff1:\t",
	"dfe_ff2:\t",
	"dfe_ff3:\t",
	"dfe_ff4:\t",
	"dfe_ff5:\t",
};

const char *ctle_params_names[] = {
	"ctle_current1_sel:\t",
	"ctle_rl1_sel:\t\t",
	"ctle_rl1_extra:\t\t",
	"ctle_res1_sel:\t\t",
	"ctle_cap1_sel:\t\t",
	"ctle_en_mid_freq:\t",
	"ctle_cs1_mid:\t\t",
	"ctle_rs1_mid:\t\t",
	"ctle_current2_sel:\t",
	"ctle_rl2_sel:\t\t",
	"ctle_rl2_tune_g:\t",
	"ctle_res2_sel:\t\t",
	"ctle_cap2_sel:\t\t",
};

struct rx_eq_params {
	s32 dfe_taps[DFE_TAPS_NUM];
	u32 ctle_params[CTLE_PARAMS_NUM];
};

static inline int _get_pattern(int argc, char *const argv[], int *arg_idx)
{
	unsigned long pattern;

	if (argc == *arg_idx || strict_strtoul(argv[*arg_idx], 10, &pattern))
		return -1;

	 (*arg_idx)++;

	switch (pattern) {
	/* Validate pattern against the list below */
	case PRBS_7:
	case PRBS_9:
	case PRBS_11:
	case PRBS_15:
	case PRBS_16:
	case PRBS_23:
	case PRBS_31:
	case PRBS_32:
		return (int)pattern;

	default:
		return -1;
	}
}

static int do_serdes_prbs(struct cmd_tbl *cmdtp, int flag, int argc,
			  char *const argv[])
{
	unsigned long port, inject_cnt;
	int gen_pattern = 0, check_pattern = 0;
	int subcmd, optcmd, ret, arg_idx, lanes_num;
	struct gserm_data gserm_data;
	char strbuf[32] = {0};

	if (argc < 3)
		return CMD_RET_USAGE;

	subcmd = prbs_subcmd_str2enum(argv[1]);
	if (subcmd == -1)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[2], 10, &port))
		return CMD_RET_USAGE;

	port &= 0xff;

	if (subcmd != PRBS_START && subcmd != PRBS_STOP) {
		if (subcmd == PRBS_INJECT) {
			if (argc == 3 || strict_strtoul(argv[3], 10, &inject_cnt))
				return CMD_RET_USAGE;
		}
		goto send_smc;
	}

	arg_idx = 3;
	while (arg_idx < argc) {
		optcmd = prbs_optcmd_str2enum(argv[arg_idx]);
		arg_idx++;

		switch (optcmd) {
		case PRBS_GENERATOR:
			gen_pattern = subcmd == PRBS_START ?
				_get_pattern(argc, argv, &arg_idx) : 1;
			break;

		case PRBS_CHECKER:
			check_pattern = subcmd == PRBS_START ?
				_get_pattern(argc, argv, &arg_idx) : 1;
			break;

		case PRBS_BOTH:
			gen_pattern = subcmd == PRBS_START ?
				_get_pattern(argc, argv, &arg_idx) : 1;
			check_pattern = gen_pattern;
			break;

		default:
			return CMD_RET_USAGE;
		}
	}

	if (gen_pattern == -1 || check_pattern == -1)
		return CMD_RET_USAGE;

send_smc:
	printf("SerDes PRBS:\n");

	if (subcmd == PRBS_SHOW) {
		void *stats;
		struct prbs_error_stats *error_stats;

		printf("port#:\tlane#:\tgserm#:\tg-lane#:"
				"\tlocked:\ttotal_bits:\terror_bits:\n");
		ret = smc_serdes_prbs_show(port, &gserm_data,
					   &stats);
		if (ret)
			return CMD_RET_FAILURE;

		lanes_num = gserm_data.lanes_num;

		error_stats = (struct prbs_error_stats *)stats;

		for (int lane_idx = 0; lane_idx < lanes_num; lane_idx++) {
			int glane = (gserm_data.mapping >> 4 * lane_idx) & 0xf;

			printf("%d\t%d\t%d\t%d\t\t%d\t%llu\t\t%llu\n",
			       (int)port,
			       lane_idx,
			       (int)gserm_data.gserm_idx,
			       glane,
			       error_stats[lane_idx].locked,
			       error_stats[lane_idx].total_bits,
			       error_stats[lane_idx].error_bits);
		}
		return CMD_RET_SUCCESS;
	}

	printf("port#:\tlane#:\tgserm#:\tg-lane#:\tcmd:\n");

	switch (subcmd) {
	case PRBS_START:
		ret = smc_serdes_prbs_start(port, &gserm_data,
					    gen_pattern, check_pattern);

		if (gen_pattern || check_pattern) {
			char cbuf[16] = {0};
			char gbuf[16] = {0};

			if (gen_pattern)
				snprintf(gbuf, 16, " gen=%d", gen_pattern);

			if (check_pattern)
				snprintf(cbuf, 16, " check=%d", check_pattern);

			snprintf(strbuf, 32, "(patterns:%s%s)", gbuf, cbuf);
		}
		break;
	case PRBS_CLEAR:
		ret = smc_serdes_prbs_clear(port, &gserm_data);
		snprintf(strbuf, 32, "counters");
		break;
	case PRBS_STOP:
		/* if both gen and check not provided, then stop both */
		if (!gen_pattern && !check_pattern) {
			gen_pattern = 1;
			check_pattern = 1;
		}

		ret = smc_serdes_prbs_stop(port, &gserm_data,
					   gen_pattern, check_pattern);

		snprintf(strbuf, 32, "%s%s%s",
			 gen_pattern ? " generator" : "",
			 gen_pattern && check_pattern ? "," : "",
			 check_pattern ? " checker" : "");
		break;
	case PRBS_INJECT:
		ret = smc_serdes_prbs_inject(port, &gserm_data, inject_cnt);
		snprintf(strbuf, 32, "%ld errors", inject_cnt);
		break;
	default:
		return CMD_RET_FAILURE;
	}

	if (ret)
		return CMD_RET_FAILURE;

	lanes_num = gserm_data.lanes_num;

	for (int lane_idx = 0; lane_idx < lanes_num; lane_idx++) {
		int glane = (gserm_data.mapping >> 4 * lane_idx) & 0xf;

		printf("%d\t%d\t%d\t%d\t\t%s %s\n",
		       (int)port,
		       lane_idx,
		       (int)gserm_data.gserm_idx,
		       glane,
		       prbs_subcmd[subcmd].s,
		       strbuf);
	}
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	sdes_prbs, 7, 1, do_serdes_prbs, "perform SerDes PRBS",
	"start <port#> [gen <pattern>] [check <pattern>] [both <pattern>]\n"
	"sdes_prbs show <port#>\n"
	"sdes_prbs stop <port#> [gen|check|both]\n"
	"sdes_prbs clear <port#>\n"
	"sdes_prbs inject <port#> <count>\n\n"
	"Parameters:\n"
	"\t <port#>: Port number from the DTS\n"
	"\t gen,check,both: generator, checker or both\n"
	"\t <pattern>: The pattern. Options are: 7 9 11 15 16 23 31 32\n"
	"\t <count>: Inject <count> of errors (accepted values: 1..8)\n"
);

static int do_serdes_lpbk(struct cmd_tbl *cmdtp, int flag, int argc,
			  char *const argv[])
{
	unsigned long port, type;
	struct gserm_data gserm_data;
	int lanes_num;
	ssize_t ret;

	if (argc < 3)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[1], 10, &port))
		return CMD_RET_USAGE;

	port &= 0xff;

	if (strict_strtoul(argv[2], 10, &type))
		return CMD_RET_USAGE;

	/* Validate looback type against the list below */
	switch (type) {
	case LPBK_TYPE_NONE:
	case LPBK_TYPE_NEA:
	case LPBK_TYPE_NED:
	case LPBK_TYPE_FED:
		break;

	default:
		return CMD_RET_USAGE;
	}

	printf("Set SerDes Loopback:\n");
	printf("port#:\tlane#:\tgserm#:\tg-lane#:\ttype:\n");

	ret =  smc_serdes_lpbk(port, &gserm_data, type);
	if (ret)
		return CMD_RET_FAILURE;

	lanes_num = gserm_data.lanes_num;

	for (int lane_idx = 0; lane_idx < lanes_num; lane_idx++) {
		int glane = (gserm_data.mapping >> 4 * lane_idx) & 0xf;

		printf("%d\t%d\t%d\t%d\t\t%s\n",
			(int)port,
			lane_idx,
			(int)gserm_data.gserm_idx,
			glane,
			lpbk_type[type]);
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	sdes_lpbk, 3, 1, do_serdes_lpbk, "set SerDes Loopback",
	"<port#> <type>\n\n"
	"Parameters:\n"
	"\t <port#>: Port number from the DTS\n"
	"\t <type>: Loopback type:\n"
	"\t\t 0: No Loopback\n"
	"\t\t 1: NEA (Near End Analog)\n"
	"\t\t 2: NED (Near End Digital)\n"
	"\t\t 3: FED (Far End Digital)\n"
);

static int do_serdes_rx(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	unsigned long port, lane;
	int lanes_cnt, max_idx, lane_idx = 0xff;
	struct gserm_data gserm_data;
	ssize_t ret;
	void *params;
	struct rx_eq_params *rx_eq_params;

	if (argc < 2)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[1], 10, &port))
		return CMD_RET_USAGE;

	port &= 0xff;

	printf("SerDes Rx Tuning Parameters:\n");
	printf("port#:\tlane#:\tgserm#:\tg-lane#:\n");
	if (argc > 2) {
		if (strict_strtoul(argv[2], 10, &lane))
			return CMD_RET_FAILURE;

		lane_idx = lane & 0xff;
		max_idx = lane_idx + 1;
	}

	ret = smc_serdes_get_rx_tuning((int)port, lane_idx,
				       &params, &gserm_data);
	if (ret)
		return CMD_RET_FAILURE;

	lanes_cnt = gserm_data.lanes_num;

	rx_eq_params = (struct rx_eq_params *)params;

	if (lane_idx == 0xff) {
		lane_idx = 0;
		max_idx = lanes_cnt;
	}

	for (; lane_idx < max_idx; lane_idx++) {
		int glane = (gserm_data.mapping >> 4 * lane_idx) & 0xf;

		printf("%d\t%d\t%d\t%d\n",
			(int)port, lane_idx,
			(int)gserm_data.gserm_idx,
			glane);

		for (int idx = 0; idx < DFE_TAPS_NUM; idx++)
			printf("\t\t%s%d\n", dfe_taps_names[idx],
			       rx_eq_params[lane_idx].dfe_taps[idx]);

		printf("\n");
		for (int idx = 0; idx < CTLE_PARAMS_NUM; idx++)
			printf("\t\t%s%d\n", ctle_params_names[idx],
			       rx_eq_params[lane_idx].ctle_params[idx]);
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	sdes_rx, 3, 1, do_serdes_rx, "read serdes Rx tuning parameters",
	"<port#> [<lane#>]\n\n"
	"parameters:\n"
	"\t <port#>, <lane#>: Port & lane pair denoting serdes lane.\n"
);

static int do_serdes_rx_training(struct cmd_tbl *cmdtp, int flag, int argc,
				 char *const argv[])
{
	unsigned long port, lane;
	int port_idx, lane_idx, glane, completed = 0, failed, tries = 30;
	struct gserm_data gserm_data;
	ssize_t ret;

	if (argc < 3)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[1], 10, &port))
		return CMD_RET_USAGE;

	port_idx = port & 0xff;

	if (strict_strtoul(argv[2], 10, &lane))
		return CMD_RET_FAILURE;

	lane_idx = lane & 0xff;

	printf("SerDes Rx Training:\n");
	printf("port#:\tlane#:\tgserm#:\tg-lane#:\tstatus:\n");

	ret = smc_serdes_start_rx_training(port_idx, lane_idx,
					   &gserm_data);
	if (ret)
		return CMD_RET_FAILURE;

	while (!completed && tries--) {
		mdelay(100);
		smc_serdes_check_rx_training(port_idx, lane_idx,
					     &completed, &failed);
	}

	if (!completed) {
		failed = 1;
		smc_serdes_stop_rx_training(port_idx, lane_idx);
	}

	glane = (gserm_data.mapping >> 4 * lane_idx) & 0xf;

	printf("%d\t%d\t%d\t%d\t\t%s\n",
	       port_idx, lane_idx,
	       (int)gserm_data.gserm_idx, glane,
	       failed ? "FAILED" : "OK");

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	sdes_rx_tr, 3, 1, do_serdes_rx_training, "perform serdes Rx training",
	"<port#> <lane#>\n"
	"parameters:\n"
	"\t <port#>, <lane#>: Port & lane pair denoting serdes lane.\n"
);

static int do_serdes_tx(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	unsigned long port, lane;
	u32 pre2_pre1 = 0, post_main = 0, flags = 0;
	int lanes_cnt, max_idx, lane_idx = 0xff;
	struct gserm_data gserm_data;
	int arg_idx;
	ssize_t ret;
	void *params;
	struct tx_eq_params *tx_eq_params;

	if (argc < 2)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[1], 10, &port))
		return CMD_RET_USAGE;

	port &= 0xff;

	if (argc > 2) {
		if (!strict_strtoul(argv[2], 10, &lane)) {
			arg_idx = 3;
			lane_idx = lane & 0xff;
			max_idx = lane_idx + 1;
		} else {
			arg_idx = 2;
		}
	} else {
		arg_idx = 2;
	}

	/* If there are no parameters left, then it means
	 * a Tx read command.
	 */
	if (arg_idx == argc)
		goto read_tx_tuning;

	/* Next parameters are optional and they should come in pairs
	 * [name <value>], like: [pre1 <pre1>].
	 * The loop below is to parse each such pair.
	 */
	while (arg_idx < argc) {
		int param;
		unsigned long value;

		param = tx_param_str2enum(argv[arg_idx]);
		if (param == -1)
			return CMD_RET_USAGE;

		arg_idx++;
		if (arg_idx == argc || strict_strtoul(argv[arg_idx], 0, &value))
			return CMD_RET_USAGE;

		value &= 0xffff;
		arg_idx++;

		switch (param) {
		case TX_PARAM_PRE2:
			pre2_pre1 |= value << 16;
			flags |= 0x01;
			break;

		case TX_PARAM_PRE1:
			pre2_pre1 |= value;
			flags |= 0x02;
			break;

		case TX_PARAM_POST:
			post_main |= value << 16;
			flags |= 0x04;
			break;

		case TX_PARAM_MAIN:
			post_main |= value;
			flags |= 0x08;
			break;

		default:
			return CMD_RET_USAGE;
		}
	}

	printf("SerDes Tx Tuning Parameters:\n");
	printf("port#:\tlane#:\tgserm#:\tg-lane#:\tstatus:\n");
	ret = smc_serdes_set_tx_tuning(port, lane_idx,
				       pre2_pre1,
				       post_main,
				       flags,
				       &gserm_data);
	if (ret)
		return CMD_RET_FAILURE;

	lanes_cnt = gserm_data.lanes_num;

	if (lane_idx == 0xff) {
		lane_idx = 0;
		max_idx = lanes_cnt;
	}

	for (; lane_idx < max_idx; lane_idx++) {
		int glane = (gserm_data.mapping >> 4 * lane_idx) & 0xf;

		printf("%d\t%d\t%d\t%d\t\tUpdated\n",
		(int)port, lane_idx,
		(int)gserm_data.gserm_idx,
		glane);
	}

	return CMD_RET_SUCCESS;

read_tx_tuning:
	printf("SerDes Tx Tuning Parameters:\n");
	printf("port#:\tlane#:\tgserm#:\tg-lane#:"
		"\tpre2:\tpre1:\tmain:\tpost:\n");
	ret = smc_serdes_get_tx_tuning((int)port, lane_idx,
				       &params, &gserm_data);
	if (ret)
		return CMD_RET_FAILURE;

	lanes_cnt = gserm_data.lanes_num;

	tx_eq_params = (struct tx_eq_params *)params;

	if (lane_idx == 0xff) {
		lane_idx = 0;
		max_idx = lanes_cnt;
	}

	for (; lane_idx < max_idx; lane_idx++) {
		int glane = (gserm_data.mapping >> 4 * lane_idx) & 0xf;

		printf("%d\t%d\t%d\t%d\t\t0x%x\t0x%x\t0x%x\t0x%x\n",
		       (int)port, lane_idx,
		       (int)gserm_data.gserm_idx,
		       glane,
		       tx_eq_params[lane_idx].pre2,
		       tx_eq_params[lane_idx].pre1,
		       tx_eq_params[lane_idx].main,
		       tx_eq_params[lane_idx].post);
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	sdes_tx, 11, 1, do_serdes_tx, "read/write serdes Tx tuning parameters",
	"<port#> [<lane#>] [pre2 <pre2>] [pre1 <pre1>]\n"
	"\t\t[main <main>] [post <post>]\n"
	"sdes_tx <port#> [<lane#>]\n\n"
	"parameters:\n"
	"\t <port#>, <lane#>: Port & lane pair denoting serdes lane.\n"
	"\t <pre2>, <pre1>, <main>, <post>:\n"
	"\t\t Transmitter’s tuning parameters.\n"
);

