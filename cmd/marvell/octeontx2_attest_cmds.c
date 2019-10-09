/*
 * Copyright (C) 2019 Marvell International Ltd.
 *
 * SPDX-License-Identifier:    GPL-2.0
 * https://spdx.org/licenses
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <asm/arch/smc.h>
#include <asm/arch/board.h>

static void hexdump(const char *prefix, unsigned char *buf, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		if ((i % 16) == 0)
			printf("%s%s%08lx: ", i ? "\n" : "",
			       prefix ? prefix : "",
			       (unsigned long)i /* i.e. offset */);
		printf("%02x ", buf[i]);
	}
	printf("\n");
}

static const char *tlv_type_name(enum sw_attestation_tlv_type type)
{
	static const char *tlv_types[ATT_TLV_TYPE_COUNT] = {
		[ATT_IMG_INIT_BIN] = "ATT_IMG_INIT_BIN",
		[ATT_IMG_ATF_BL1] = "ATT_IMG_ATF_BL1",
		[ATT_IMG_BOARD_DT] = "ATT_IMG_BOARD_DT",
		[ATT_IMG_LINUX_DT] = "ATT_IMG_LINUX_DT",
		[ATT_IMG_SCP_TBL1FW] = "ATT_IMG_SCP_TBL1FW",
		[ATT_IMG_MCP_TBL1FW] = "ATT_IMG_MCP_TBL1FW",
		[ATT_IMG_AP_TBL1FW] = "ATT_IMG_AP_TBL1FW",
		[ATT_IMG_ATF_BL2] = "ATT_IMG_ATF_BL2",
		[ATT_IMG_ATF_BL31] = "ATT_IMG_ATF_BL31",
		[ATT_IMG_ATF_BL33] = "ATT_IMG_ATF_BL33",
		[ATT_SIG_NONCE] = "ATT_SIG_NONCE",
	};
	if (type < 0 || type >= ATT_TLV_TYPE_COUNT)
		return "Unknown";
	else
		return tlv_types[type];
}

static int do_attest(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	ssize_t len;
	uintptr_t attest_ret;
	sw_attestation_info_hdr_t *att_info;
	sw_attestation_tlv_t *tlv;
	void *tlv_limit, *next_tlv, *cert, *sig;
	unsigned long nonce_len;

	if (argc < 2) {
		printf("Please specify a nonce string.\n");
		return CMD_RET_USAGE;
	}

	nonce_len = strlen(argv[1]);
	if (nonce_len > SW_ATT_INFO_NONCE_MAX_LEN) {
		printf("Input nonce too large (%lu vs %u)\n",
		       nonce_len, SW_ATT_INFO_NONCE_MAX_LEN);
		return CMD_RET_USAGE;
	}

	/* query for buffer address */
	attest_ret = smc_attest(0);

	if ((ssize_t)attest_ret <= 0) {
		printf("Error: unable to obtain buffer address.\n");
		return CMD_RET_FAILURE;
	}

	att_info = (void *)attest_ret;

	/* Pass nonce data to service in buffer */
	memcpy(att_info->input_nonce, argv[1], nonce_len);

	attest_ret = smc_attest(nonce_len);

	if ((ssize_t)attest_ret != 0) {
		printf("Error %ld from attest command\n", attest_ret);
		return CMD_RET_FAILURE;
	}

	printf("Attestation decode:\n");
	printf("\tMagic: %08x\n", ntohl(att_info->magic_be));
	tlv = att_info->tlv_list;
	tlv_limit = (void *)tlv + (long)(ntohs(att_info->tlv_len_be));
	while ((uintptr_t)tlv < (uintptr_t)tlv_limit) {
		next_tlv = (void *)tlv + ntohs(tlv->length_be) +
			   sizeof(*tlv);
		if ((uintptr_t)next_tlv > (uintptr_t)tlv_limit) {
			printf("Error: invalid TLV construct\n");
			break;
		}
		printf("\t%s:\n", tlv_type_name(ntohs(tlv->type_be)));
		hexdump("\t   ", tlv->value, ntohs(tlv->length_be));
		tlv = (sw_attestation_tlv_t *)next_tlv;
	}

	/* command returned successfully, but with malformed data */
	if (tlv != tlv_limit)
		return CMD_RET_SUCCESS;

	/* Certificate is contiguous to TLV date */
	cert = tlv_limit;
	printf("\tCertificate:\n");
	hexdump("\t   ", (unsigned char *)cert,
		ntohs(att_info->certificate_len_be));

	len = ntohs(att_info->total_len_be);
	len -= ntohs(att_info->signature_len_be);
	printf("\tAttestation [Signed] Image:\n");
	hexdump("\t   ", (unsigned char *)att_info, len);

	/* Authentication signature is contiguous to certificate */
	sig = (void *)cert + ntohs(att_info->certificate_len_be);
	printf("\tAuthentication signature:\n");
	hexdump("\t   ", sig, htons(att_info->signature_len_be));

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(attest, 2, 0, do_attest,
	   "Retrieve attestation information",
	   "<nonce_string>\n"
	   "    - where <nonce_string> consists of ASCII characters\n"
	   "\nExample: attest ABCDEF0123\n"
);
