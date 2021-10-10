// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2021 Marvell
 *
 * https://spdx.org/licenses
 */

#include <common.h>
#include <command.h>
#include <dm.h>
#include <asm/arch/smc.h>
#include <linux/arm-smccc.h>

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

enum phy_sgmii_vod {
	PHY_SGMII_VOD_14mV = 0,
	PHY_SGMII_VOD_112mV,
	PHY_SGMII_VOD_210mV,
	PHY_SGMII_VOD_308mV,
	PHY_SGMII_VOD_406mV,
	PHY_SGMII_VOD_504mV,
	PHY_SGMII_VOD_602mV,
	PHY_SGMII_VOD_700mV,

	PHY_SGMII_VOD_MAX
};

#define VOD(_val) {PHY_SGMII_VOD_ ## _val, #_val}
static struct {
	enum phy_sgmii_vod e;
	const char *s;
} sgmii_vod_values[] = {
	VOD(14mV),
	VOD(112mV),
	VOD(210mV),
	VOD(308mV),
	VOD(406mV),
	VOD(504mV),
	VOD(602mV),
	VOD(700mV),
};

DEFINE_STR_2_ENUM_FUNC(sgmii_vod_values)

enum phy_lpbk_type {
	PHY_LPBK_TYPE_NONE,
	PHY_LPBK_TYPE_LINE,
};

enum phy_prbs_cmd {
	PHY_PRBS_START_CMD = 1,
	PHY_PRBS_STOP_CMD,
	PHY_PRBS_GET_DATA_CMD,
};

static struct {
	enum phy_prbs_cmd e;
	const char *s;
} prbs_cmds[] = {
	{PHY_PRBS_START_CMD, "start"},
	{PHY_PRBS_STOP_CMD, "stop"},
	{PHY_PRBS_GET_DATA_CMD, "show"},
};

DEFINE_STR_2_ENUM_FUNC(prbs_cmds)

enum prbs_side {
	PRBS_SIDE_LINE = 0,
	PRBS_SIDE_HOST,
};

static struct {
	enum prbs_side e;
	const char *s;
} prbs_sides[] = {
	{PRBS_SIDE_LINE, "line"},
	{PRBS_SIDE_HOST, "host"},
};

DEFINE_STR_2_ENUM_FUNC(prbs_sides)

enum prbs_type {
	PRBS_7 = 0,
	PRBS_23,
	PRBS_31,
	PRBS_1010,
};

static struct {
	enum prbs_type e;
	const char *s;
} prbs_types[] = {
	{PRBS_7, "7"},
	{PRBS_23, "23"},
	{PRBS_31, "31"},
	{PRBS_1010, "1010"},
};

DEFINE_STR_2_ENUM_FUNC(prbs_types)

enum phy_sdes_cmd {
	PHY_SDES_GET_CFG,
	PHY_SDES_SET_CFG,
};

enum phy_mdio_optype {
	CLAUSE_22 = 0,
	CLAUSE_45,
};

static struct {
	enum phy_mdio_optype e;
	const char *s;
} mdio_optype[] = {
	{CLAUSE_22, "c22"},
	{CLAUSE_45, "c45"},
};

DEFINE_STR_2_ENUM_FUNC(mdio_optype)

static int do_phy_lpbk(struct cmd_tbl *cmdtp, int flag, int argc,
		       char *const argv[])
{
	unsigned long eth_idx, lmac_idx, type;
	int ret;

	if (argc < 4)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[1], 10, &eth_idx))
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[2], 10, &lmac_idx))
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[3], 10, &type))
		return CMD_RET_USAGE;

	/* Validate looback type against the list below */
	switch (type) {
	case PHY_LPBK_TYPE_NONE:
	case PHY_LPBK_TYPE_LINE:
		break;

	default:
		printf("ERROR: Unsupported loopback type\n");
		return CMD_RET_USAGE;
	}

	printf("Enable PHY Loopback:\n");
	printf("eth#:\tlmac#:\ttype:\tstatus:\n");

	ret = smc_phy_dbg_loopback_write(eth_idx, lmac_idx, type);
	printf("%ld\t%ld\t%ld\t%s\n",
	       eth_idx, lmac_idx, type, ret ? "FAILED" : "OK");

	return ret ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	phy_lpbk, 4, 1, do_phy_lpbk, "enable PHY Loopback",
	"<eth#> <lmac#> <type>\n\n"
	"Parameters:\n"
	"\t <eth#>: RPM/CGX index\n"
	"\t <lmac#>: lmac index\n"
	"\t <enable>: Loopback type:\n"
	"\t\t 0: No Loopback\n"
	"\t\t 1: Line loopback\n"
);

static int do_phy_temp(struct cmd_tbl *cmdtp, int flag, int argc,
		       char *const argv[])
{
	unsigned long eth_idx, lmac_idx;
	int ret, temp = 0;
	char temp_str[8];

	if (argc < 3)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[1], 10, &eth_idx))
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[2], 10, &lmac_idx))
		return CMD_RET_USAGE;

	printf("Read PHY temperature:\n");
	printf("eth#:\tlmac#:\ttemp:\n");

	ret = smc_phy_dbg_temp_read(eth_idx, lmac_idx, &temp);
	snprintf(temp_str, ARRAY_SIZE(temp_str), "%dC", temp);

	printf("%ld\t%ld\t%s\n",
	       eth_idx, lmac_idx, ret ? "FAILED" : temp_str);

	return ret ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	phy_temp, 3, 1, do_phy_temp, "get PHY temperature",
	"<eth#> <lmac#>\n\n"
	"Parameters:\n"
	"\t <eth#>: RPM/CGX index\n"
	"\t <lmac#>: lmac index\n"
);

static int do_phy_prbs(struct cmd_tbl *cmdtp, int flag, int argc,
		       char *const argv[])
{
	unsigned long eth_idx, lmac_idx;
	int ret, cmd, host, type = 0;
	int host_errors, line_errors;

	if (argc < 4)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[1], 10, &eth_idx))
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[2], 10, &lmac_idx))
		return CMD_RET_USAGE;

	cmd = prbs_cmds_str2enum(argv[3]);
	if (cmd == -1)
		return CMD_RET_USAGE;

	if (cmd != PHY_PRBS_GET_DATA_CMD) {
		if (argc < 5)
			return CMD_RET_USAGE;

		host = prbs_sides_str2enum(argv[4]);
	}

	if (cmd == -1 || host == -1)
		return CMD_RET_USAGE;

	switch (cmd) {
	case PHY_PRBS_START_CMD:
		if (argc < 6)
			return CMD_RET_USAGE;

		type = prbs_types_str2enum(argv[5]);
		if (type == -1)
			return CMD_RET_USAGE;

		ret = smc_phy_dbg_prbs_write(eth_idx, lmac_idx,
					     PHY_PRBS_START_CMD, host, type);

		printf("Start PHY PRBS:\n");
		printf("eth#:\tlmac#:\thost:\ttype:\tstatus:\n");
		printf("%ld\t%ld\t%d\t%d\t%s\n",
		       eth_idx, lmac_idx, host, type, ret ? "FAILED" : "OK");
		break;

	case PHY_PRBS_STOP_CMD:
		ret = smc_phy_dbg_prbs_write(eth_idx, lmac_idx,
					     PHY_PRBS_STOP_CMD, host, type);
		printf("Stop PHY PRBS:\n");
		printf("eth#:\tlmac#:\thost:\tstatus:\n");
		printf("%ld\t%ld\t%d\t%s\n",
		       eth_idx, lmac_idx, host, ret ? "FAILED" : "OK");

		break;

	case PHY_PRBS_GET_DATA_CMD:
		host_errors = smc_phy_dbg_prbs_read(eth_idx, lmac_idx, 1);
		line_errors = smc_phy_dbg_prbs_read(eth_idx, lmac_idx, 0);
		printf("PRBS error counters:\n");
		printf("eth#:\tlmac#:\thost errors:\tline errors:\n");
		printf("%ld\t%ld\t%d\t\t%d\n",
		       eth_idx, lmac_idx, host_errors, line_errors);
		break;

	default:
		return CMD_RET_USAGE;
	}

	return ret ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	phy_prbs, 6, 1, do_phy_prbs, "start/stop/show PHY PRBS",
	"<eth#> <lmac#> start <side> <type>\n"
	"phy_prbs <eth#> <lmac#> stop <side>\n"
	"phy_prbs <eth#> <lmac#> show\n\n"
	"Parameters:\n"
	"\t <eth#>: RPM/CGX index\n"
	"\t <lmac#>: lmac index\n"
	"\t <side>:\n"
	"\t\t host: host side PRBS\n"
	"\t\t line: line side PRBS\n"
	"\t <type>:\n"
	"\t\t 7\n"
	"\t\t 23\n"
	"\t\t 31\n"
	"\t\t 1010\n"
);

static inline int parse_mdio_op_data(char *const argv[], int argc,
				     int write, int *clause, int *dev_page,
				     int *reg, int *val)
{
	int optype, min_args;
	unsigned long arg;
	const char *dp;
	bool neg = false;

	min_args = write ? 4 : 3;

	if (argc < min_args)
		return CMD_RET_USAGE;

	optype = mdio_optype_str2enum(argv[0]);
	if (optype == -1)
		return -1;

	*clause = optype;

	dp = argv[1];
	if (*dp == '-') {
		dp++;
		neg = true;
	}

	if (strict_strtoul(dp, 0, &arg))
		return -1;

	if (neg)
		arg = -arg;

	if (arg == -1) {
		/* Cannot ignore devad when using clause 45 */
		if (optype == CLAUSE_45)
			return -1;
	} else if (arg >= 0) {
		/* device addr or page nr are 5 bits */
		arg &= 0x1f;
	} else {
		return -1;
	}

	*dev_page = arg;

	if (strict_strtoul(argv[2], 0, &arg))
		return -1;

	*reg = arg;

	if (write) {
		if (!strict_strtoul(argv[3], 0, &arg))
			*val = arg;
		else
			return -1;
	}

	return 0;
}

static int do_phy_read(struct cmd_tbl *cmdtp, int flag, int argc,
		       char *const argv[])
{
	unsigned long eth_idx, lmac_idx;
	int ret, clause, dev_page, reg, val;
	char val_str[8];

	if (argc < 3)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[1], 10, &eth_idx))
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[2], 10, &lmac_idx))
		return CMD_RET_USAGE;

	if (parse_mdio_op_data(argv + 3, argc - 3, 0,
			       &clause, &dev_page, &reg, NULL)) {
		return CMD_RET_USAGE;
	}

	ret = smc_phy_dbg_reg_read(eth_idx, lmac_idx, clause,
				   dev_page == -1 ? (1 << 5) : dev_page,
				   reg, &val);

	snprintf(val_str, ARRAY_SIZE(val_str), "0x%x", val);

	printf("Read PHY register:\n");
	printf("eth#:\tlmac#:\ttype:\tdev:\treg:\tval:\n");
	printf("%ld\t%ld\t%d\t%d\t0x%x\t%s\n",
	       eth_idx, lmac_idx, clause, dev_page,
	       reg, ret ? "FAILED" : val_str);

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	phy_read, 6, 1, do_phy_read, "read PHY register",
	"<eth#> <lmac#> <mode> <dev_page> <reg>\n\n"
	"Parameters:\n"
	"\t <eth#>: RPM/CGX index\n"
	"\t <lmac#>: lmac index\n"
	"\t <mode>: MDIO mode:\n"
	"\t\t c22: use Clause 22\n"
	"\t\t c45: use Clause 45\n"
	"\t <dev_page>: device addr or page num (clause 22)\n"
	"\t <reg>: register addr\n"
);

static int do_phy_write(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	unsigned long eth_idx, lmac_idx;
	int ret, clause, dev_page, reg, val;

	if (argc < 3)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[1], 10, &eth_idx))
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[2], 10, &lmac_idx))
		return CMD_RET_USAGE;

	if (parse_mdio_op_data(argv + 3, argc - 3, 1,
			       &clause, &dev_page, &reg, &val)) {
		return CMD_RET_USAGE;
	}

	ret = smc_phy_dbg_reg_write(eth_idx, lmac_idx, clause,
				    dev_page == -1 ? (1 << 5) : dev_page,
				    reg, val);

	printf("Write PHY register:\n");
	printf("eth#:\tlmac#:\ttype:\tdev:\treg:\tval:\tstatus:\n");
	printf("%ld\t%ld\t%d\t%d\t0x%x\t0x%x\t%s\n",
	       eth_idx, lmac_idx, clause, dev_page,
	       reg, val, ret ? "FAILED" : "OK");

	return ret ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	phy_write, 7, 1, do_phy_write, "write PHY register",
	"<eth#> <lmac#> <mode> <dev_page> <reg> <val>\n\n"
	"Parameters:\n"
	"\t <eth#>: RPM/CGX index\n"
	"\t <lmac#>: lmac index\n"
	"\t <mode>: MDIO mode:\n"
	"\t\t c22: use Clause 22\n"
	"\t\t c45: use Clause 45\n"
	"\t <dev_page>: device addr or page num (clause 22)\n"
	"\t <reg>: register addr\n"
	"\t <val>: register value\n"
);

static int do_phy_sdes(struct cmd_tbl *cmdtp, int flag, int argc,
		       char *const argv[])
{
	unsigned long eth_idx, lmac_idx;
	int ret;
	u32 vod;

	if (argc < 3)
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[1], 10, &eth_idx))
		return CMD_RET_USAGE;

	if (strict_strtoul(argv[2], 10, &lmac_idx))
		return CMD_RET_USAGE;

	if (argc < 4) {
		const char *res;

		printf("Get PHY SerDes VOD:\n");
		printf("eth#:\tlmac#:\tvod:\n");
		ret = smc_phy_dbg_get_serdes_cfg(eth_idx, lmac_idx, &vod);
		res = ret ? "FAILED" : sgmii_vod_values[vod & 0x7].s;

		printf("%ld\t%ld\t%s\n", eth_idx, lmac_idx, res);
	} else {
		printf("Set PHY SerDes VOD:\n");
		printf("eth#:\tlmac#:\tvod:\tstatus:\n");
		vod = sgmii_vod_values_str2enum(argv[3]);
		if (vod == -1)
			return CMD_RET_USAGE;

		ret = smc_phy_dbg_set_serdes_cfg(eth_idx, lmac_idx, vod);
		printf("%ld\t%ld\t%s\t%s\n",
		       eth_idx, lmac_idx,
		       sgmii_vod_values[vod & 0x7].s,
		       ret ? "FAILED" : "OK");
	}

	return ret ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	phy_sdes, 4, 1, do_phy_sdes, "read/write PHY Tx SerDes config (VOD)",
	"<eth#> <lmac#> [<vod>]\n\n"
	"Parameters:\n"
	"\t <eth#>: RPM/CGX index\n"
	"\t <lmac#>: lmac index\n"
	"\t <vod>:\n"
	"\t\t 14mV\n"
	"\t\t 112mV\n"
	"\t\t 210mV\n"
	"\t\t 308mV\n"
	"\t\t 406mV\n"
	"\t\t 504mV\n"
	"\t\t 602mV\n"
	"\t\t 700mV\n"
);

