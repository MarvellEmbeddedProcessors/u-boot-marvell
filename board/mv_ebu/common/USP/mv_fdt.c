/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

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

*******************************************************************************/

#include <common.h>
#include <command.h>
#include "mvOs.h"
#include "mv_amp.h"
#include "mvCpuIfRegs.h"
#include "uart/mvUart.h"
#include "libfdt.h"
#include "fdt_support.h"
#include "mvBoardEnvLib.h"

/*******************************************************************************
* fdt_env_setup
*
* DESCRIPTION:
*
* INPUT:
*	fdtfile.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
void fdt_env_setup(char *fdtfile)
{
#if CONFIG_OF_LIBFDT
	char *env;

	env = getenv("fdt_skip_update"); /* if set to yes, automatic board setup will be skipped */
	if (!env)
		setenv("fdt_skip_update", "no");

	env = getenv("fdtaddr");
	if (!env)
		setenv("fdtaddr", "0x1000000");

	env = getenv("fdtfile");
	if (!env)
		setenv("fdtfile", fdtfile);

	/* boot command to fetch DT file, update DT (if fdt_skip_update=no) and bootz LSP zImage */
	char bootcmd_fdt[] = "tftpboot 0x2000000 $image_name;tftpboot $fdtaddr $fdtfile;"
		"setenv bootargs $console $nandEcc $mtdparts $bootargs_root nfsroot=$serverip:$rootpath "
		"ip=$ipaddr:$serverip$bootargs_end $mvNetConfig video=dovefb:lcd0:$lcd0_params "
		"clcd.lcd0_enable=$lcd0_enable clcd.lcd_panel=$lcd_panel; bootz 0x2000000 - $fdtaddr;";

	/* boot command to and bootz LSP zImage (after DT already fetch and set) */
	env = getenv("bootcmd_fdt_boot");
	if (!env)
		setenv("bootcmd_fdt_boot", "tftpboot 0x2000000 $image_name; setenv bootargs $console $nandEcc "
			"$mtdparts $bootargs_root nfsroot=$serverip:$rootpath ip=$ipaddr:$serverip$bootargs_end "
			"$mvNetConfig video=dovefb:lcd0:$lcd0_params clcd.lcd0_enable=$lcd0_enable "
			"clcd.lcd_panel=$lcd_panel; bootz 0x2000000 - $fdtaddr;");

	/* boot command to fetch DT file, and set bootcmd_fdt_boot as new bootcmd (manually edit DT & run 'boot') */
	env = getenv("bootcmd_fdt_edit");
	if (!env)
		setenv("bootcmd_fdt_edit", "tftpboot $fdtaddr $fdtfile; fdt addr $fdtaddr; setenv bootcmd $bootcmd_fdt_boot");

	env = getenv("bootcmd_fdt");
	if (!env)
		setenv("bootcmd_fdt",bootcmd_fdt);
#endif
}


#ifdef CONFIG_OF_BOARD_SETUP

#define MV_FDT_MAXDEPTH 32

static int mv_fdt_find_node(void *fdt, const char *name);
static int mv_fdt_board_compatible_name_update(void *fdt);
static int mv_fdt_update_serial(void *fdt);
static int mv_fdt_update_cpus(void *fdt);
static int mv_fdt_update_pex(void *fdt);
static int mv_fdt_update_sata(void *fdt);
static int mv_fdt_update_usb(void *fdt, MV_UNIT_ID unitType);
static int mv_fdt_update_ethnum(void *fdt);
static int mv_fdt_update_flash(void *fdt);
static int mv_fdt_set_node_prop(void *fdt, const char *node, const char *prop, const char *prop_val);
static int mv_fdt_remove_node(void *fdt, const char *path);
static int mv_fdt_scan_and_set_alias(void *fdt, const char *name, const char *alias);
static int mv_fdt_nand_mode_fixup(void *fdt);
static int mv_fdt_update_pinctrl(void *fdt);
static int mv_fdt_debug;
#ifdef CONFIG_MV_SDHCI
static int mv_fdt_update_sdhci(void *fdt);
#endif

#if 0 /* not compiled, since this routine is currently not in use  */
static int mv_fdt_remove_prop(void *fdt, const char *path,
				const char *name, int nodeoff);
#endif

enum nfc_driver_type {
	MV_FDT_NFC_PXA3XX,	/* mainline pxa3xx-nand */
	MV_FDT_NFC_HAL,		/* mvebu HAL-based NFC */
	MV_FDT_NFC_HAL_A375,	/* mvebu HAL-based NFC for A375 */
	MV_FDT_NFC_NONE,
};

#define mv_fdt_dprintf(...)		\
	if (mv_fdt_debug)		\
		printf(__VA_ARGS__);

#define mv_fdt_modify(fdt, err, function)				\
	do {								\
		err = function; /* Try requested routine */		\
		while (err == -FDT_ERR_NOSPACE) {			\
			mv_fdt_dprintf("Resize fdt...\n");		\
			fdt_resize(fdt);				\
			err = function; /* Retry requested routine*/	\
		}							\
	} while (0)

/*******************************************************************************
* ft_board_setup
*
* DESCRIPTION:
*
* INPUT:
*	blob.
*	bd.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
void ft_board_setup(void *blob, bd_t *bd)
{
	int err;		/* error number */
	char *env;		/* env value */

	/* Debug information will be printed if variable enaFDTdebug=yes */
	env = getenv("enaFDTdebug");
	if (env && ((strncmp(env, "yes", 3) == 0)))
		mv_fdt_debug = 1;
	else
		mv_fdt_debug = 0;

	/* Update dt information for all SoCs */
	/* Update dt bootargs from commandline */
	fdt_resize(blob);
	mv_fdt_modify(blob, err, fdt_chosen(blob, 1));
	if (err < 0) {
		mv_fdt_dprintf("Updating DT bootargs failed\n");
		goto bs_fail;
	}
	mv_fdt_dprintf("DT bootargs updated from commandline\n");

	/* Update ethernet aliases with nodes' names and update mac addresses */
	err = mv_fdt_scan_and_set_alias(blob, "ethernet@", "ethernet");
	if (err < 0)
		goto bs_fail;
	fdt_fixup_ethernet(blob);
	mv_fdt_dprintf("DT ethernet MAC addresses updated from environment\n");

	/* Update memory node */
	fixup_memory_node(blob);
	mv_fdt_dprintf("Memory node updated\n");

	/* Get number of CPUs and update dt */
	err = mv_fdt_update_cpus(blob);
	if (err < 0)
		goto bs_fail;

	/* Get number of active PEX port and update DT */
	err = mv_fdt_update_pex(blob);
	if (err < 0)
		goto bs_fail;

	/* Get number of active SATA units and update DT */
	err = mv_fdt_update_sata(blob);
	if (err < 0)
		goto bs_fail;

	/* Get number of active USB2.0 units and update DT */
	err = mv_fdt_update_usb(blob, USB_UNIT_ID);
	if (err < 0)
		goto bs_fail;

	/* Get number of active USB3.0 units and update DT */
	err = mv_fdt_update_usb(blob, USB3_UNIT_ID);
	if (err < 0)
		goto bs_fail;

	/* Get number of active ETH port and update DT */
	err = mv_fdt_update_ethnum(blob);
	if (err < 0)
		goto bs_fail;

#ifdef CONFIG_MV_SDHCI
	/* Update SDHCI driver settings in DT */
	err = mv_fdt_update_sdhci(blob);
	if (err < 0)
		goto bs_fail;
#endif

	/* Get number of active flash devices and update DT */
	err = mv_fdt_update_flash(blob);
	if (err < 0)
		goto bs_fail;

	/* Update NAND controller ECC settings in DT */
	err = mv_fdt_nand_mode_fixup(blob);
	if (err < 0)
		goto bs_fail;

	/* Update pinctrl driver settings in DT */
	err = mv_fdt_update_pinctrl(blob);
	if (err < 0)
		goto bs_fail;

	/* Update compatible (board name) in DT */
	err = mv_fdt_board_compatible_name_update(blob);
	if (err < 0)
		goto bs_fail;

	/* Update serial/UART nodes in DT */
	err = mv_fdt_update_serial(blob);
	if (err < 0)
		goto bs_fail;

	printf("Updating device tree successful\n");
	return;

bs_fail:
	printf("Updating device tree failed\n");
	return;
}

/*******************************************************************************
* mv_fdt_find_node
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*	name.
*
* OUTPUT:
*	None.
*
* RETURN:
*	node offset or -1 if node not found.
*
*******************************************************************************/
static int mv_fdt_find_node(void *fdt, const char *name)
{
	int nodeoffset;		/* current node's offset from libfdt */
	int nextoffset;		/* next node's offset from libfdt */
	int level = 0;		/* current fdt scanning depth */
	uint32_t tag;		/* device tree tag at given offset */
	const char *node;	/* node name */

	/* Set scanning starting point to '/' */
	nodeoffset = fdt_path_offset(fdt, "/");
	if (nodeoffset < 0) {
		mv_fdt_dprintf("%s: failed to get '/' nodeoffset\n",
			       __func__);
		return -1;
	}
	/* Scan device tree for node = 'name' and return its offset */
	while (level >= 0) {
		tag = fdt_next_tag(fdt, nodeoffset, &nextoffset);
		switch (tag) {
		case FDT_BEGIN_NODE:
			node = fdt_get_name(fdt, nodeoffset, NULL);
			if (strncmp(node, name, strlen(name)+1) == 0)
				return nodeoffset;
			level++;
			if (level >= MV_FDT_MAXDEPTH) {
				mv_fdt_dprintf("%s: Nested too deep, "
					       "aborting.\n", __func__);
				return -1;
			}
			break;
		case FDT_END_NODE:
			level--;
			if (level == 0)
				level = -1;		/* exit the loop */
			break;
		case FDT_PROP:
		case FDT_NOP:
			break;
		case FDT_END:
			mv_fdt_dprintf("Device tree scanning failed - end of "
				       "tree tag\n");
			return -1;
		default:
			mv_fdt_dprintf("Device tree scanning failed - unknown "
				       "tag\n");
			return -1;
		}
		nodeoffset = nextoffset;
	}

	/* Node not found in the device tree */
	return -1;
}

/*******************************************************************************
* mv_fdt_update_cpus
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_update_cpus(void *fdt)
{
	int cpusnodes = 0;	/* number of cpu nodes */
	int nodeoffset;		/* node offset from libfdt */
	int err;		/* error number */
	const char *node;	/* node name */
	char *p;		/* auxiliary pointer */
	char *lastcpu;		/* pointer to last valid cpu */
	MV_U8 cpusnum;		/* number of cpus */
	int depth = 2;

	/* Get CPUs number and remove unnecessary nodes */
	cpusnum = mvCtrlGetCpuNum() + 1;
	mv_fdt_dprintf("Number of CPUs detected: %d\n", cpusnum);
	/* Find cpus node */
	node = "cpus";
	nodeoffset = mv_fdt_find_node(fdt, node);
	if (nodeoffset < 0) {
		mv_fdt_dprintf("Lack of '%s' node in device tree\n", node);
		return -1;
	}
	p = malloc(strlen("cpu@x")+1);
	lastcpu = malloc(strlen("cpu@x")+1);
	while (strncmp(node, "cpu", 3) == 0) {
		cpusnodes++;
		/* Remove excessive cpu nodes */
		if (cpusnodes > cpusnum + 1) {
			strcpy(p, node);
			err = mv_fdt_remove_node(fdt, (const char *)p);
				if (err < 0) {
					mv_fdt_dprintf("Failed to remove %s\n",
						       node);
					free(p);
					free(lastcpu);
					return -1;
				}
			node = lastcpu;
			nodeoffset = mv_fdt_find_node(fdt, node);
		} else {
			strcpy(lastcpu, node);
		}
		nodeoffset = fdt_next_node(fdt, nodeoffset, &depth);
		node = fdt_get_name(fdt, nodeoffset, NULL);
	}
	free(p);
	free(lastcpu);

	return 0;
}

/*******************************************************************************
* mv_fdt_update_sata
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_update_sata(void *fdt)
{
	int i;
	char propval[10];				/* property value */
	const char *prop = "status";			/* property name */
	char node[64];					/* node name */
	MV_U32 sataUnitNum = mvCtrlSataMaxUnitGet();	/* number of interfaces */

	mv_fdt_dprintf("Maximum SATA Units = %d\n, Active SATA Units:", sataUnitNum);
	for (i = 0; i < sataUnitNum; i++) {
		if (mvCtrlIsActiveSataUnit(i))
			sprintf(propval, "okay"); /* Enable active SATA interfaces */
		else
			sprintf(propval, "disabled"); /* disable NON active SATA units */

		sprintf(node, "sata@%x", mvCtrlSataRegBaseGet(i));
		if (mv_fdt_set_node_prop(fdt, node, prop, propval) < 0) {
			mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n", prop, node);
			return -1;
		}
	}

	return 0;
}

#ifdef CONFIG_MV_SDHCI
/*******************************************************************************
* mv_fdt_update_sdhci
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_update_sdhci(void *fdt)
{
	int err, nodeoffset;				/* nodeoffset: node offset from libfdt */
	char propval[10];				/* property value */
	const char *prop = "status";			/* property name */
	char node[64];					/* node name */

	if (mvBoardisSdioConnected())
		sprintf(propval, "okay");
	else
		sprintf(propval, "disabled");

	sprintf(node, "sdhci@%x", MV_SDMMC_REGS_OFFSET);
	nodeoffset = mv_fdt_find_node(fdt, node);

	if (nodeoffset < 0) {
		mv_fdt_dprintf("Lack of '%s' node in device tree\n", node);
		return -1;
	}

	if (strncmp(fdt_get_name(fdt, nodeoffset, NULL), node, strlen(node)) == 0) {
		mv_fdt_modify(fdt, err, fdt_setprop(fdt, nodeoffset, prop,
						propval, strlen(propval)+1));
		if (err < 0) {
			mv_fdt_dprintf("Modifying '%s' in '%s' node failed\n", prop, node);
			return -1;
		}
		mv_fdt_dprintf("Set '%s' property to '%s' in '%s' node\n", prop, propval, node);
	}

	return 0;
}
#endif

/*******************************************************************************
* mv_fdt_update_pex
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_update_pex(void *fdt)
{
	MV_U32 pexnum;				/* number of interfaces */
	MV_BOARD_PEX_INFO *boardPexInfo;	/* pex info */
	int err;				/* error number */
	int nodeoffset;				/* node offset from libfdt */
	char propval[10];			/* property value */
	const char *prop = "status";		/* property name */
	const char *node = "pcie-controller";	/* node name */
	int i = 0;
	int k = 0;
	int depth = 1;

	/* Get number of active pex interfaces */
	boardPexInfo = mvBoardPexInfoGet();
	pexnum = boardPexInfo->boardPexIfNum;
	mv_fdt_dprintf("Number of active PEX ports detected = %d\n", pexnum);
	mv_fdt_dprintf("Active PEX HW interfaces: ");
	for (k = 0; k < pexnum; k++)
		mv_fdt_dprintf("%d, ", boardPexInfo->pexMapping[k]);
	mv_fdt_dprintf("\n");
	/* Set controller and 'pexnum' number of interfaces' status to 'okay'.
	 * Rest of them are disabled */
	nodeoffset = mv_fdt_find_node(fdt, node);
	if (nodeoffset < 0) {
		mv_fdt_dprintf("Lack of '%s' node in device tree\n", node);
		return -1;
	}
	while (strncmp(node, "pcie", 4) == 0) {
		for (k = 0; k <= pexnum; k++)
			if (i == boardPexInfo->pexMapping[k]) {
				sprintf(propval, "okay");
				goto pex_ok;
			}
		sprintf(propval, "disabled");
pex_ok:
		if (strncmp(node, "pcie-controller", 15) != 0)
			i++;
		mv_fdt_modify(fdt, err, fdt_setprop(fdt, nodeoffset, prop,
						propval, strlen(propval)+1));
		if (err < 0) {
			mv_fdt_dprintf("Modifying '%s' in '%s' node failed\n",
				       prop, node);
			return -1;
		}
		mv_fdt_dprintf("Set '%s' property to '%s' in '%s' node\n", prop,
			       propval, node);
		nodeoffset = fdt_next_node(fdt, nodeoffset, &depth);
		if (nodeoffset < 0) {
			mv_fdt_dprintf("Modifying '%s' in '%s' node failed\n",
				       prop, node);
			return -1;
		}
		node = fdt_get_name(fdt, nodeoffset, NULL);
	}
	return 0;
}

/*******************************************************************************
* mv_fdt_update_usb
*
* DESCRIPTION: update USB2.0(eHCI) & USB3.0(xHCI) status
*
* INPUT: fdt.
* OUTPUT: None.
* RETURN: -1 on error os 0 otherwise.
*******************************************************************************/
static int mv_fdt_update_usb(void *fdt, MV_UNIT_ID unitType)
{
	char propval[10];				/* property value */
	const char *prop = "status";			/* property name */
	char node[64];					/* node name */
	int i, maxUsbPorts = unitType == USB_UNIT_ID ? MV_USB_MAX_PORTS : MV_USB3_MAX_HOST_PORTS;

	/* update USB2.0 ports status */
	for (i = 0; i < maxUsbPorts; i++) {
		if (mvBoardIsUsbPortConnected(unitType, i))
			sprintf(propval, "okay"); /* Enable active SATA interfaces */
		else
			sprintf(propval, "disabled"); /* disable NON active SATA units */

		sprintf(node, "usb%s@%x", unitType == USB_UNIT_ID ? "" : "3", MV_USB2_USB3_REGS_OFFSET(unitType, i));
		if (mv_fdt_set_node_prop(fdt, node, prop, propval) < 0) {
			mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n", prop, node);
			return -1;
		}
	}

	return 0;
}

/*******************************************************************************
* mv_fdt_update_pinctrl
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_update_pinctrl(void *fdt)
{
	char propval[10];				/* property value */
	const char *prop = "compatible";		/* property name */
	const char *node = "pinctrl";			/* node name */

	/* update pinctrl driver 'compatible' propert, according to device model type */
	sprintf(propval, "marvell,mv88f%x-pinctrl", mvCtrlModelGet());
	if (mv_fdt_set_node_prop(fdt, node, prop, propval) < 0) {
		mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n", prop, node);
		return -1;
	}

	return 0;
}

/*******************************************************************************
* mv_fdt_update_ethnum
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_update_ethnum(void *fdt)
{
	MV_32 phyAddr;
	int port;
	int len = 0;			/* property length */
	int ethPortsNum;		/* nodes' counter */
	int nodeoffset;			/* node offset from libfdt */
	int aliasesoffset;		/* aliases node offset from libfdt */
	int phyoffset;
	const uint32_t *phandle;
	char prop[20];			/* property name */
	char propval[20];		/* property value */
	const char *node;		/* node name */
	int depth = 1;
	int err;

	/* Get number of ethernet nodes */
	aliasesoffset = mv_fdt_find_node(fdt, "aliases");
	if (aliasesoffset < 0) {
		mv_fdt_dprintf("Lack of 'aliases' node in device tree\n");
		return -1;
	}

	/* Walk trough all aliases and count Ethernet ports entries */
	ethPortsNum = 0;
	nodeoffset = aliasesoffset;
	do {
		nodeoffset = fdt_next_node(fdt, nodeoffset, &depth);
		node = fdt_get_name(fdt, nodeoffset, NULL);
		if (strncmp(node, "ethernet@", 9) == 0) /* Node name can be NULL, but it's OK */
			ethPortsNum++;
	} while (node != NULL);
	mv_fdt_dprintf("Number of ethernet nodes in DT = %d\n", ethPortsNum);

		/* Get path to ethernet node from property value */
	for (port = 0; port < ethPortsNum; port++) {

		/* Get path to ethernet node from property value */
		sprintf(prop, "ethernet%d", port);
		node = fdt_getprop(fdt, aliasesoffset, prop, &len);
		if (node == NULL) {
			mv_fdt_dprintf("Lack of '%s' property in 'aliases' node\n", prop);
			return -1;
		}
		if (len == 0) {
			mv_fdt_dprintf("Empty property value\n");
			return -1;
		}
		/* Alias points to the ETH port node using full DT path */
		nodeoffset = fdt_path_offset(fdt, node);

		/* Set ethernet port status to 'disabled' */
		/* Enable valid ports and configure their parametrs, disable non valid ones */
		if (mvBoardIsEthConnected(port) != MV_TRUE)
			sprintf(propval, "disabled");
		else {

			/* Configure PHY address */
			phyAddr = mvBoardPhyAddrGet(port);
			if (phyAddr == -1)
				phyAddr = 999; /* Inband management - see mv_netdev.c for details */

			/* The ETH node we found contains a pointer (phandle) to its PHY
			   The phandle is a unique numeric identifier of a specific node */
			phandle = fdt_getprop(fdt, nodeoffset, "phy", &len);
			if (len == 0) {
				mv_fdt_dprintf("Empty \"phy\" property value\n");
				return -1;
			}
			phyoffset = fdt_node_offset_by_phandle(fdt, ntohl(*phandle));
			if (phyoffset < 0) {
				mv_fdt_dprintf("Failed to get PHY node by phandle %x\n", ntohl(*phandle));
				return -1;
			}

			/* Setup PHY address in DT in "reg" property of an appropriate PHY node.
			   This value is HEX number, not a string, and uses network byte order */
			phyAddr = htonl(phyAddr);
			sprintf(prop, "reg");
			mv_fdt_modify(fdt, err, fdt_setprop(fdt, phyoffset, prop, &phyAddr, sizeof(phyAddr)));
			if (err < 0) {
				mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n",
							   prop, fdt_get_name(fdt, phyoffset, NULL));
				return -1;
			}
			mv_fdt_dprintf("Set property '%s' of node '%s' to %#010x\n",
						   prop, fdt_get_name(fdt, phyoffset, NULL), ntohl(phyAddr));

			/* Configure PHY mode */
			switch (mvBoardPortTypeGet(port)) {
			case MV_PORT_TYPE_SGMII: /* regardless the fact that qsgmii is supported by kernel DT,*/
			case MV_PORT_TYPE_QSGMII:/* the ETH driver does not make use of this connection mode */
				sprintf(propval, "sgmii");
				break;
			case MV_PORT_TYPE_RGMII:
				sprintf(propval, "rgmii");
				break;
			default:
				mv_fdt_dprintf("Bad port type received for interface %d\n", port);
				return -1;
			}

			/* Setup PHY connection type in DT */
			sprintf(prop, "phy-mode");
			mv_fdt_modify(fdt, err, fdt_setprop(fdt, nodeoffset, prop, propval, strlen(propval)+1));
			if (err < 0) {
				mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n", prop, node);
				return -1;
			}
			mv_fdt_dprintf("Set '%s' property to '%s' in '%s' node\n", prop, propval, node);

			/* Last property to set is the "status" - common for valid and non-valid ports */
			sprintf(propval, "okay");
		}

		sprintf(prop, "status");
		mv_fdt_modify(fdt, err, fdt_setprop(fdt, nodeoffset, prop, propval, strlen(propval)+1));
		if (err < 0) {
			mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n", prop, node);
			return -1;
		}
		mv_fdt_dprintf("Set '%s' property to '%s' in '%s' node\n", prop, propval, node);

	} /* For all ports in board sctructure */

	return 0;
}

/*******************************************************************************
* mv_fdt_update_flash
*
* DESCRIPTION:
*   Update FDT entires related to board flash devices according to board sctructures.
*   The board sctructures are scanned and initialized at uboot startup using board
*   boot source and bord type. This function obtails flash entries marked as active
*   during the board initalization process and activates appropriate nodes in FDT.
*   Nodes corresponding to non-active flash devices are disabled in FDT.
*
* INPUT:
*	fdt		- FDT
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_update_flash(void *fdt)
{
	MV_U32 device;
	char propval[10];				/* property value */
	const char *prop = "status";	/* property name */
	char node[64];					/* node name */
	MV_U32 flashNum;				/* number of flashes defined for beard */
	MV_U32 unit, maxUnits;			/* number of interface controller units */
	MV_U32 chipSel;
	MV_BOOL interfaceIsActive;

	/* start with SPI flashes */
	flashNum = mvBoardGetDevicesNumber(BOARD_DEV_SPI_FLASH);
	maxUnits = mvCtrlSocUnitInfoNumGet(SPI_UNIT_ID);
	for (unit = 0; unit < maxUnits; unit++) {
		interfaceIsActive = MV_FALSE;
		for (device = 0; device < flashNum; device++) {
			/* Only devices related to current bus/unit */
			if (mvBoardGetDevBusNum(device, BOARD_DEV_SPI_FLASH) != unit)
				continue;

			if (mvBoardGetDevState(device, BOARD_DEV_SPI_FLASH) == MV_TRUE) {
				interfaceIsActive = MV_TRUE; /* One active device is enough */
				break;
			}
		} /* SPI devices */

		if (interfaceIsActive == MV_TRUE)
			sprintf(propval, "okay"); /* Enable active SPI unit/bus */
		else
			sprintf(propval, "disabled"); /* disable NON active SPI unit/bus */

		sprintf(node, "spi@%x", MV_SPI_REGS_OFFSET(unit));
		if (mv_fdt_set_node_prop(fdt, node, prop, propval) < 0) {
			mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n", prop, node);
			return -1;
		}
	} /* SPI units/buses */

	/* handle NAND flashes - there is only one NAND unit, but different CS are possible */
	flashNum = mvBoardGetDevicesNumber(BOARD_DEV_NAND_FLASH);
	interfaceIsActive = MV_FALSE;
	for (device = 0; device < flashNum; device++) {
		if (mvBoardGetDevState(device, BOARD_DEV_NAND_FLASH) == MV_TRUE) {
			interfaceIsActive = MV_TRUE; /* One active device is enough */
			/* Once a NAND node updated, there is no reason to search for other devices */
			break;
		}
	} /* NAND devices */

	if (interfaceIsActive == MV_TRUE)
		sprintf(propval, "okay"); /* Enable NAND interface if found active device */
	else
		sprintf(propval, "disabled"); /* disable NAND interface if NOT found active devices */

	sprintf(node, "nand@%x", MV_NFC_REGS_OFFSET);
	if (mv_fdt_set_node_prop(fdt, node, prop, propval) < 0) {
		mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n", prop, node);
		return -1;
	}

	/* handle NOR flashes - there is only one NOR unit, but different CS are possible */
	flashNum = mvBoardGetDevicesNumber(BOARD_DEV_NOR_FLASH);
	chipSel = 0xFFFF;
	for (device = 0;  device < flashNum; device++) {
		if (mvBoardGetDevState(device, BOARD_DEV_NOR_FLASH) != MV_TRUE)
			continue;

		chipSel = mvBoardGetDevCSNum(device, BOARD_DEV_NOR_FLASH);
		if (chipSel == DEV_BOOCS)	/* Special case - this value is not close to the rest of Device Bus */
			chipSel = 4;			/* Chip Selects (0 to 3) in target definitiond enum, */
		else						/* so it will be hadled just as a 4th chip select */
			chipSel -= DEVICE_CS0;	/* Substract the base value for getting 0-3 CS range */

		/* Once an active NOR flash entry found, there is no reason to search for others */
		break;
	} /* NOR devices */

	/* Walk through Device Bus entries and update them */
	for (device = 0; device < 5; device++) {
		if (device == chipSel)
			sprintf(propval, "okay"); /* Enable NOR interface if found active device */
		else
			sprintf(propval, "disabled"); /* disable NOR interface if NOT found active devices */

		if (device == 4)	/* Fourth CS is a Boot CS - see the comment above */
			sprintf(node, "devbus-bootcs");
		else
			sprintf(node, "devbus-cs%d", device);

		if (mv_fdt_set_node_prop(fdt, node, prop, propval) < 0) {
			mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n", prop, node);
			return -1;
		}
	}

	return 0;
}

/*******************************************************************************
* mv_fdt_set_node_prop
*
* DESCRIPTION:
* 	Set a named node property to a specific value
*
* INPUT:
*	fdt		- FDT
*	node	- FDT node name
*	prop	- FDT node property name
*	propval	- property value to set
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_set_node_prop(void *fdt, const char *node, const char *prop, const char *propval)
{
	int err, nodeoffset = 0; /* nodeoffset: node offset from libfdt */

	if (node != NULL) { /* node == NULL --> search property in root node */
		nodeoffset = mv_fdt_find_node(fdt, node);
		if (nodeoffset < 0) {
			mv_fdt_dprintf("Lack of '%s' node in device tree\n", node);
			return -1;
		}
	}

	if (strncmp(fdt_get_name(fdt, nodeoffset, NULL), node, strlen(node)) == 0) {
		mv_fdt_modify(fdt, err, fdt_setprop(fdt, nodeoffset, prop, propval, strlen(propval)+1));
		if (err < 0) {
			mv_fdt_dprintf("Modifying '%s' in '%s' node failed\n", prop, node);
			return -1;
		}
	}
	mv_fdt_dprintf("Set '%s' property to '%s' in '%s' node\n", prop, propval, node);

	return 0;
}

#if 0 /* not compiled, since this routine is currently not in use  */
/*******************************************************************************
* mv_fdt_remove_prop
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*	path.
*	name.
*	nodeoff.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_remove_prop(void *fdt, const char *path,
				const char *name, int nodeoff)
{
	int error;

	error = fdt_delprop(fdt, nodeoff, name);
	if (error == -FDT_ERR_NOTFOUND) {
		mv_fdt_dprintf("'%s' already removed from '%s' node\n",
			       name, path);
		return 0;
	} else if (error < 0) {
		mv_fdt_dprintf("Removing '%s' from '%s' node failed\n",
			       name, path);
		return error;
	} else {
		mv_fdt_dprintf("Removing '%s' from '%s' node succeeded\n",
			       name, path);
		return 0;
	}
}
#endif

/*******************************************************************************
* mv_fdt_remove_node
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*	path.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_remove_node(void *fdt, const char *path)
{
	int error;
	int nodeoff;

	nodeoff = mv_fdt_find_node(fdt, path);
	error = fdt_del_node(fdt, nodeoff);
	if (error == -FDT_ERR_NOTFOUND) {
		mv_fdt_dprintf("'%s' node already removed from device tree\n",
			       path);
		return 0;
	} else if (error < 0) {
		mv_fdt_dprintf("Removing '%s' node from device tree failed\n",
			       path);
		return error;
	} else {
		mv_fdt_dprintf("Removing '%s' node from device tree "
			       "succeeded\n", path);
		return 0;
	}
}

/*******************************************************************************
* mv_fdt_scan_and_set_alias
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*	name.
*	alias.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_scan_and_set_alias(void *fdt,
					const char *name, const char *alias)
{
	int i = 0;		/* alias index */
	int nodeoffset;		/* current node's offset from libfdt */
	int nextoffset;		/* next node's offset from libfdt */
	int delta;		/* difference between next and current offset */
	int err = 0;		/* error number */
	int level = 0;		/* current fdt scanning depth */
	char aliasname[16];	/* alias name to be stored in '/aliases' node */
	char path[128] = "";	/* full path to current node */
	char tmp[24];		/* auxiliary char array for extended node name*/
	char *cut;		/* auxiliary char pointer */
	const char *node;	/* node name */
	uint32_t tag;		/* device tree tag at given offset */
	const struct fdt_property *prop;

	/* Check if '/aliases' node exist. Otherwise create it */
	nodeoffset = mv_fdt_find_node(fdt, "aliases");
	if (nodeoffset < 0) {
		mv_fdt_modify(fdt, err, fdt_add_subnode(fdt, 0, "aliases"));
		if (err < 0) {
			mv_fdt_dprintf("Creating '/aliases' node failed\n");
			return -1;
		}
		nodeoffset = fdt_path_offset(fdt, "/aliases");
	}

	/* Check if there are pre-defined aliases and rely on them
	 * instead of scanning the Device Tree
	 */
	for (nextoffset = fdt_first_property_offset(fdt, nodeoffset);
	     (nextoffset >= 0);
	     (nextoffset = fdt_next_property_offset(fdt, nextoffset))) {

		prop = fdt_get_property_by_offset(fdt, nextoffset, NULL);
		if (!prop) {
			nextoffset = -FDT_ERR_INTERNAL;
			break;
		}
		if (strncmp(fdt_string(fdt, fdt32_to_cpu(prop->nameoff)),
			    alias, strlen(alias)) == 0) {
			mv_fdt_dprintf("'%s' aliases exist. Skip scanning DT for '%s' nodes\n",
				       alias, name);
			return 0;
		}
	}

	/* Scan device tree for nodes that that contain 'name' substring and
	 * create their 'alias' with respective number */

	nodeoffset = 0;
	while (level >= 0) {
		tag = fdt_next_tag(fdt, nodeoffset, &nextoffset);
		switch (tag) {
		case FDT_BEGIN_NODE:
			node = fdt_get_name(fdt, nodeoffset, NULL);
			sprintf(tmp, "/%s", node);
			if (nodeoffset != 0)
				strcat(path, tmp);
			if (strstr(node, name) != NULL) {
				delta = nextoffset - nodeoffset;
				sprintf(aliasname, "%s%d", alias, i);
				nodeoffset = mv_fdt_find_node(fdt, "aliases");
				if (nodeoffset < 0)
					goto alias_fail;
				mv_fdt_modify(fdt, err, fdt_setprop(fdt,
							nodeoffset, aliasname,
							path, strlen(path)+1));
				if (err < 0)
					goto alias_fail;
				nodeoffset = fdt_path_offset(fdt, path);
				if (nodeoffset < 0)
					goto alias_fail;
				nextoffset = nodeoffset + delta;
				mv_fdt_dprintf("Set alias %s=%s\n", aliasname,
					       path);
				i++;
			}
			level++;
			if (level >= MV_FDT_MAXDEPTH) {
				mv_fdt_dprintf("%s: Nested too deep, "
					       "aborting.\n", __func__);
				return -1;
			}
			break;
alias_fail:
			mv_fdt_dprintf("Setting alias %s=%s failed\n",
				       aliasname, path);
			return -1;
		case FDT_END_NODE:
			level--;
			cut = strrchr(path, '/');
			*cut = '\0';
			if (level == 0)
				level = -1;		/* exit the loop */
			break;
		case FDT_PROP:
		case FDT_NOP:
			break;
		case FDT_END:
			mv_fdt_dprintf("Device tree scanning failed - end of "
				       "tree tag\n");
			return -1;
		default:
			mv_fdt_dprintf("Device tree scanning failed - unknown "
				       "tag\n");
			return -1;
		}
		nodeoffset = nextoffset;
	}

	return 0;
}

/*******************************************************************************
* mv_fdt_nfc_driver_type
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*	offset.
*	check_status.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_nfc_driver_type(void *fdt, int *offset,
				  int check_status)
{
	int nodeoffset, type;
	const void *status;
	const char *compat[3] = {"marvell,armada370-nand",
				 "marvell,armada-nand",
				 "marvell,armada-375-nand"};

	for (type = 0; type < 3; type++) {
		nodeoffset = fdt_node_offset_by_compatible(fdt, -1,
							   compat[type]);
		if (nodeoffset < 0)
			continue;

		if (!check_status) {
			mv_fdt_dprintf("Detected NFC driver - %s\n",
				       compat[type]);
			break;
		}

		status = fdt_getprop(fdt, nodeoffset, "status", NULL);
		if (!status || strncmp(status, "okay", 4) == 0) {
			mv_fdt_dprintf("Enabled NFC driver - %s\n",
				       compat[type]);
			break;
		}
	}

	*offset = nodeoffset;
	return type;
}

/*******************************************************************************
* mv_fdt_nand_mode_fixup
*
* DESCRIPTION:
*   Obtain the NAND ECC value from u-boot environment and upodate FDT NAND node
*   accordingly.
*
* INPUT:
*	fdt.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_nand_mode_fixup(void *fdt)
{
	u32 ecc_val;
	int nfcoffset, nfc_driver_hal, err;
	char *nfc_config;
	char prop[20];
	char propval[7];

	/* Search for enabled NFC driver in DT */
	nfc_driver_hal = mv_fdt_nfc_driver_type(fdt, &nfcoffset, 1);
	if (nfc_driver_hal == MV_FDT_NFC_NONE) {
		mv_fdt_dprintf("No NFC driver enabled in device tree\n");
		return 0;
	}

	/* Search for 'nandEcc' parameter in the environment */
	nfc_config = getenv("nandEcc");
	if (!nfc_config) {
		mv_fdt_dprintf("Keep default NFC configuration\n");
		return 0;
	}

	nfc_config += strlen("nfcConfig") + 1;

	/* Check for ganged mode */
	if (strncmp(nfc_config, "ganged", 6) == 0) {
		nfc_config += 7;

		if (!nfc_driver_hal) {
			mv_fdt_dprintf("NFC update: pxa3xx-nand driver "
				       "doesn't support ganged mode\n");
			goto check_ecc;
		}

		sprintf(prop, "%s", "nfc,nfc-mode");
		sprintf(propval, "%s", "ganged");
		mv_fdt_modify(fdt, err, fdt_setprop(fdt, nfcoffset,
						    prop, propval,
						    strlen(propval) + 1));
		if (err < 0) {
			mv_fdt_dprintf("NFC update: fail to modify '%s'\n",
				       prop);
			return -1;
		}
		mv_fdt_dprintf("NFC update: set '%s' property to '%s'\n",
			       prop, propval);
	}

check_ecc:
	/* Check for ECC type directive */
	if (strcmp(nfc_config, "1bitecc") == 0)
		ecc_val = nfc_driver_hal ? MV_NFC_ECC_HAMMING : 1;
	else if (strcmp(nfc_config, "4bitecc") == 0)
		ecc_val = nfc_driver_hal ? MV_NFC_ECC_BCH_2K : 4;
	else if (strcmp(nfc_config, "8bitecc") == 0)
		ecc_val = nfc_driver_hal ? MV_NFC_ECC_BCH_1K : 8;
	else if (strcmp(nfc_config, "12bitecc") == 0)
		ecc_val = nfc_driver_hal ? MV_NFC_ECC_BCH_704B : 12;
	else if (strcmp(nfc_config, "16bitecc") == 0)
		ecc_val = nfc_driver_hal ? MV_NFC_ECC_BCH_512B : 16;
	else {
		mv_fdt_dprintf("NFC update: invalid nfcConfig ECC parameter - \"%s\"\n", nfc_config);
		return 0;
	}

	if (!nfc_driver_hal)
		sprintf(prop, "%s", "nand-ecc-strength");
	else
		sprintf(prop, "%s", "nfc,ecc-type");

	mv_fdt_modify(fdt, err, fdt_setprop_u32(fdt, nfcoffset, prop,
						ecc_val));
	if (err < 0) {
		mv_fdt_dprintf("NFC update: fail to modify'%s'\n", prop);
		return -1;
	}
	mv_fdt_dprintf("NFC update: set '%s' property to %d\n", prop, ecc_val);

	return 0;
}

static int mv_fdt_board_compatible_name_update(void *fdt)
{
	char propval[128];				/* property value */
	const char *prop = "compatible";		/* property name */
	char node[64];					/* node name */

	mvBoardCompatibleNameGet(propval);

	mvOsPrintf("\nskipping update of 'compatible' property in root node of device tree\n");
	return 0;

	if (mv_fdt_set_node_prop(fdt, NULL, prop, propval) < 0)
		mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n", prop, node);

	return 0;
}

/*******************************************************************************
* mv_fdt_update_serial
*
* DESCRIPTION:
*
* INPUT:
*	fdt.
*
* OUTPUT:
*	None.
*
* RETURN:
*	-1 on error os 0 otherwise.
*
*******************************************************************************/
static int mv_fdt_update_serial(void *fdt)
{
	char propval[10];				/* property value */
	const char *prop = "status";	/* property name */
	char node[64];					/* node name */
	int i, defaultSerialPort = mvBoardUartPortGet();

	for (i = 0; i < MV_UART_MAX_CHAN; i++) {
		if (i == defaultSerialPort)
			sprintf(propval, "okay"); /* Enable active Serial port node */
		else
			sprintf(propval, "disabled"); /* Disable non-active Serial port node */

		sprintf(node, "serial@%x", MV_UART_REGS_OFFSET(i));
		if (mv_fdt_set_node_prop(fdt, node, prop, propval) < 0) {
			mv_fdt_dprintf("Failed to set property '%s' of node '%s' in device tree\n", prop, node);
			return -1;
		}
	}

	return 0;
}

#endif
