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

#ifdef CONFIG_OF_BOARD_SETUP

static int mv_fdt_update_l2(void *fdt);
static int mv_fdt_update_cpus(void *fdt);
static int mv_fdt_update_pex(void *fdt);
static int mv_fdt_update_ethnum(void *fdt);
static int mv_fdt_remove_prop(void *fdt, const char *path,
				const char *name, int nodeoff);
static int mv_fdt_remove_node(void *fdt, const char *path);
static int mv_fdt_scan_and_set_alias(void *fdt, const char *startnode,
					const char *name, const char *alias);
static int mv_fdt_modify(void *fdt, int function);
static int mv_fdt_debug;

#define mv_fdt_dprintf(...)		\
	if (mv_fdt_debug)		\
		printf(__VA_ARGS__);

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
	err = mv_fdt_modify(blob, fdt_chosen(blob, 1));
	if (err < 0) {
		mv_fdt_dprintf("Updating DT bootargs failed\n");
		goto bs_fail;
	}
	mv_fdt_dprintf("DT bootargs updated from commandline\n");

	/* Update dt L2 information */
	err = mv_fdt_update_l2(blob);
	if (err < 0)
		goto bs_fail;

	/* Update ethernet aliases with nodes' names and update mac addresses */
	err = mv_fdt_scan_and_set_alias(blob, "/soc", "ethernet", "ethernet");
	if (err < 0)
		goto bs_fail;
	fdt_fixup_ethernet(blob);
	mv_fdt_dprintf("DT ethernet MAC addresses updated from environment\n");

	/* Get number of CPUs and update dt */
	err = mv_fdt_update_cpus(blob);
	if (err < 0)
		goto bs_fail;

	/* Get number of active PEX port and update DT */
	err = mv_fdt_update_pex(blob);
	if (err < 0)
		goto bs_fail;

	/* Get number of active ETH port and update DT */
	err = mv_fdt_update_ethnum(blob);
	if (err < 0)
		goto bs_fail;

	printf("Updating device tree successful\n");
	return;

bs_fail:
	printf("Updating device tree failed\n");
	return;
}

static int mv_fdt_update_l2(void *fdt)
{
	int err;			/* error number */
	int nodeoffset;			/* node offset from libfdt */
	char *env;			/* env value */
	char *prop;			/* property name */
	const char *node;		/* node name */

	/* If /l2-cache node doesn't exist, omit whole updating */
	node = "/l2-cache";
	nodeoffset = fdt_path_offset(fdt, node);
	if (nodeoffset < 0) {
		mv_fdt_dprintf("Omit L2 properties update - /l2-cache node "
			       "not present in fdt\n");
		goto no_l2_node;
	}
	/* Enable/disable L2 cache */
	env = getenv("disL2Cache");
	if (env && ((strncmp(env, "yes", 3) == 0))) {
		err = mv_fdt_remove_node(fdt, node);
		if (err < 0) {
			mv_fdt_dprintf("Updating fdt failed - L2 disabling\n");
			return -1;
		}
		mv_fdt_dprintf("Remove '%s' node - disL2Cache=yes\n", node);
	} else {
		/* Enable/disable L2 cache WT mode */
		/* Check setting of L2forceWrPolicy variable. If it doesn't
		 * exist examine setL2CacheWT variable*/
		env = getenv("L2forceWrPolicy");
		prop = "wt-override";
		nodeoffset = fdt_path_offset(fdt, node);
		if (env) {
			if ((strncmp(env, "wt", 2) == 0) ||
			    (strncmp(env, "WT", 2) == 0)) {
				err = mv_fdt_modify(fdt, fdt_setprop(fdt,
						nodeoffset, prop, NULL, 0));
				if (err < 0) {
					mv_fdt_dprintf("Adding '%s' to '%s' "
						       "node failed\n", prop,
						       node);
					return -1;
				} else
					mv_fdt_dprintf("Adding '%s' to '%s' "
						       "node succeeded\n", prop,
						       node);
			} else {
				err = mv_fdt_remove_prop(fdt, node, prop,
								nodeoffset);
				if (err < 0) {
					mv_fdt_dprintf("Removing '%s' from "
						       "'%s' node failed\n",
						       prop, node);
					return -1;
				}
			}
		} else {
			env = getenv("setL2CacheWT");
			if (env && ((strncmp(env, "yes", 3) == 0))) {
				err = mv_fdt_modify(fdt, fdt_setprop(fdt,
						nodeoffset, prop, NULL, 0));
				if (err < 0) {
					mv_fdt_dprintf("Adding '%s' to '%s' "
						       "node failed\n", prop,
						       node);
					return -1;
				} else
					mv_fdt_dprintf("Adding '%s' to '%s' "
						       "node succeeded\n", prop,
						       node);
			} else if (env && ((strncmp(env, "no", 2) == 0))) {
				err = mv_fdt_remove_prop(fdt, node, prop,
								nodeoffset);
				if (err < 0) {
					mv_fdt_dprintf("Removing '%s' from "
						       "'%s' node failed\n",
						       prop, node);
					return -1;
				}
			}
		}
	}

no_l2_node:
	return 0;
}

static int mv_fdt_update_cpus(void *fdt)
{
	MV_U8 cpusnum;		/* number of cpus */
	int err;		/* error number */

	/* Get CPUs number and remove unnecessary nodes */
	cpusnum = mvCtrlGetCpuNum() + 1;
	mv_fdt_dprintf("Number of CPUs detected: %d\n", cpusnum);
	if (cpusnum <= 2) {
		err = mv_fdt_remove_node(fdt, "/cpus/cpu@3");
		if (err < 0) {
			mv_fdt_dprintf("Failed to remove cpu@3\n");
			return -1;
		}
		err = mv_fdt_remove_node(fdt, "/cpus/cpu@2");
		if (err < 0) {
			mv_fdt_dprintf("Failed to remove cpu@2\n");
			return -1;
		}
		if (cpusnum == 1)
			err = mv_fdt_remove_node(fdt, "/cpus/cpu@1");
		if (err < 0) {
			mv_fdt_dprintf("Failed to remove cpu@1\n");
			return -1;
		}
	}
	return 0;
}

static int mv_fdt_update_pex(void *fdt)
{
	MV_U32 pexnum;				/* number of interfaces */
	MV_BOARD_PEX_INFO *boardPexInfo;	/* pex info */
	int err;				/* error number */
	int nodeoffset;				/* node offset from libfdt */
	char *prop;				/* property name */
	char *propval;				/* property value */
	const char *node;			/* node name */
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
	prop = "status";
	node = "/soc/pcie-controller";
	nodeoffset = fdt_path_offset(fdt, node);
	if (nodeoffset < 0) {
		mv_fdt_dprintf("Lack of '%s' node in device tree\n", node);
		return -1;
	}
	while (strncmp(node, "pcie", 4) == 0) {
		for (k = 0; k <= pexnum; k++)
			if (i == boardPexInfo->pexMapping[k]) {
				propval = "okay";
				goto pex_ok;
			}
		propval = "disabled";
pex_ok:
		if (strncmp(node, "pcie-controller", 15) != 0)
			i++;
		err = mv_fdt_modify(fdt, fdt_setprop(fdt, nodeoffset, prop,
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

static int mv_fdt_update_ethnum(void *fdt)
{
	MV_U32 ethnum = 0;		/* number of interfaces */
	int i, err;			/* error number */
	int len = 0;			/* property length */
	int nodeoffset;			/* node offset from libfdt */
	char *prop;			/* property name */
	char *propval;			/* property value */
	const char *node;		/* node name */

	for (i = 0; i < mvCtrlEthMaxPortGet(); i++) {
		if (mvBoardIsEthConnected(i) == MV_TRUE)
			ethnum++;
	}

	mv_fdt_dprintf("Number of ethernet ports detected = %d\n", ethnum);
	if (ethnum == 3) {
		/* Get ethernet3 node path from /aliases */
		nodeoffset = fdt_path_offset(fdt, "/aliases");
		if (nodeoffset < 0) {
			mv_fdt_dprintf("Lack of /aliases node in device "
				       "tree\n");
			return -1;
		}
		prop = "ethernet3";
		node = fdt_getprop(fdt, nodeoffset, prop, &len);
		if (node == NULL) {
			mv_fdt_dprintf("Lack of '%s' property in /aliases "
				       "node\n", prop);
			return -1;
		}
		if (len == 0) {
			mv_fdt_dprintf("Empty property value\n");
			return -1;
		}
		/* Set ethernet3 port status to 'disabled' */
		nodeoffset = fdt_path_offset(fdt, node);
		if (nodeoffset < 0) {
			mv_fdt_dprintf("Lack of '%s' node in device tree\n",
				       node);
			return -1;
		}
		prop = "status";
		propval = "disabled";
		err = mv_fdt_modify(fdt, fdt_setprop(fdt, nodeoffset, prop,
						propval, strlen(propval)+1));
		if (err < 0) {
			mv_fdt_dprintf("Modifying '%s' in '%s' node failed\n",
				       prop, node);
			return -1;
		}
		mv_fdt_dprintf("Set '%s' property to '%s' in '%s' node\n",
			       prop, propval, node);
	}
	return 0;
}

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

static int mv_fdt_remove_node(void *fdt, const char *path)
{
	int error;
	int nodeoff;

	nodeoff = fdt_path_offset(fdt, path);
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

static int mv_fdt_modify(void *fdt, int function)
{
	int err;		/* error number */

	err = function;
	while (err == -FDT_ERR_NOSPACE) {
		mv_fdt_dprintf("Resize fdt...\n");
		fdt_resize(fdt);
		err = function;
	}

	return err;
}

static int mv_fdt_scan_and_set_alias(void *fdt, const char *startnode,
					const char *name, const char *alias)
{
	int i = 0;		/* alias index */
	int nodeoffset;		/* current node's offset from libfdt */
	int nextoffset;		/* next node's offset from libfdt */
	int delta;		/* difference between next and current offset */
	int err = 0;		/* error number */
	int level = 0;		/* current fdt scanning depth */
	char aliasname[16];	/* alias name to be stored in '/aliases' node */
	char path[128];		/* path to be saved under 'aliasname' */
	const char *node;	/* node name */
	uint32_t tag;		/* device tree tag at given offset */

	/* Check if '/aliases' node exist. Otherwise create it */
	nodeoffset = fdt_path_offset(fdt, "/aliases");
	if (nodeoffset < 0) {
		err = mv_fdt_modify(fdt, fdt_add_subnode(fdt, 0, "aliases"));
		if (err < 0) {
			mv_fdt_dprintf("Creating '/aliases' node failed\n");
			return -1;
		}
	}

	/* Set scanning starting point to 'startnode' */
	nodeoffset = fdt_path_offset(fdt, startnode);
	if (nodeoffset < 0) {
		mv_fdt_dprintf("%s: failed to find '%s' node\n", __func__,
			       startnode);
		return -1;
	}
	/* Scan device tree for nodes that that contain 'name' substring and
	 * create their 'alias' with respective number */
	while (level >= 0) {
		tag = fdt_next_tag(fdt, nodeoffset, &nextoffset);
		switch (tag) {
		case FDT_BEGIN_NODE:
			/* Don't scan deeper than the nodes comprised directly
			 * in 'startnode' */
			if (level > 1) {
				level++;
				break;
			}
			node = fdt_get_name(fdt, nodeoffset, NULL);
			if (strstr(node, name) != NULL) {
				delta = nextoffset - nodeoffset;
				sprintf(aliasname, "%s%d", alias, i);
				sprintf(path, "%s/%s", startnode, node);
				nodeoffset = fdt_path_offset(fdt, "/aliases");
				if (nodeoffset < 0)
					goto alias_fail;
				err = mv_fdt_modify(fdt, fdt_setprop(fdt,
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
			break;
alias_fail:
			mv_fdt_dprintf("Setting alias %s=%s failed\n",
				       aliasname, path);
			return -1;
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

	return 0;
}
#endif
