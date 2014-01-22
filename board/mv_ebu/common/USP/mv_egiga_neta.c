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
disclaimer/

*******************************************************************************/

#include <common.h>
#include <command.h>
#include <net.h>
#include <malloc.h>

#if defined(MV_INCLUDE_GIG_ETH)
#include "mvOs.h"
#include "mvSysHwConfig.h"


#include "neta/gbe/mvNeta.h"

#include "gpp/mvGppRegs.h"
#include "eth-phy/mvEthPhy.h"
#include "ethSwitch/mvSwitch.h"
#include "mvBoardEnvLib.h"
#include "mvSysNetaApi.h"

/******************************************************
 * driver internal definitions --                     *
 ******************************************************/
/* use only tx-queue0 and rx-queue0 */
#define EGIGA_DEF_TXQ 0
#define EGIGA_DEF_TXP 0
#define EGIGA_DEF_RXQ 0

/* rx buffer size */
#define ETH_HLEN       14
#define WRAP           (2 + ETH_HLEN + 4 + 32)  /* 2(HW hdr) 14(MAC hdr) 4(CRC) 32(extra for cache prefetch)*/
#define MTU            1500
#define RX_BUFFER_SIZE (MTU + WRAP)

/* rings length */
#define EGIGA_TXQ_LEN   20
#define EGIGA_RXQ_LEN   20

#define NAMESIZE 16


typedef struct _egigaPriv {
	int port;
	MV_VOID *halPriv;
	MV_BOOL devInit;
	MV_BOOL devEnable;
} egigaPriv;

typedef struct _packetInfo {
	MV_U8 *bufVirtPtr;
	MV_ULONG bufPhysAddr;
	MV_U32 dataSize;
} pktInfo;


/******************************************************
 * functions prototype --                             *
 ******************************************************/
static int mvEgigaLoad(int port, char *name, char *enet_addr);
static int mvEgigaInit(struct eth_device *dev, bd_t *p);
static int mvEgigaHalt(struct eth_device *dev);
static int mvEgigaTx(struct eth_device *dev, volatile MV_VOID *packet, int len);
static int mvEgigaRx(struct eth_device *dev);
static pktInfo *mvCreatePacket(MV_VOID);

/***********************************************************
 * mv_eth_initialize --                                    *
 *   main driver initialization. loading the interfaces.   *
 ***********************************************************/
int mv_eth_initialize(bd_t *bis)
{
	int port;
	MV_8 *enet_addr;
	MV_8 name[NAMESIZE+1];
	MV_8 enetvar[9];
	MV_U32 portMask = 0;

	for (port = 0; port < mvCtrlEthMaxPortGet(); port++) {
		if (MV_FALSE ==  mvBoardIsGbEPortConnected(port))
			continue;

		if (MV_FALSE == mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, port))
			continue;
		MV_BIT_SET(portMask, port);
	}

	/* HAL init + port power up + port win init */
	mvSysNetaInit(portMask,0xff); /* TODO: do i need to return status? what to do if failed? in Linux - void function */
	for (port = 0; port < mvCtrlEthMaxPortGet(); port++) {

		if (MV_FALSE ==  mvBoardIsGbEPortConnected(port))
			continue;

		if (MV_FALSE == mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, port))
			continue;

		/* interface name */
		sprintf(name, "egiga%d", port);
		/* interface MAC addr extract */
		sprintf(enetvar, port ? "eth%daddr" : "ethaddr", port);
		enet_addr = getenv(enetvar);

		mvEgigaLoad(port, name, enet_addr);
	}

	return 0;
}

/***********************************************************
 * mvEgigaLoad --                                          *
 *   load a network interface into uboot network core.     *
 *   initialize sw structures e.g. private, rings, etc.    *
 ***********************************************************/
static int mvEgigaLoad(int port, char *name, char *enet_addr)
{
	struct eth_device *dev = NULL;
	egigaPriv *priv = NULL;

	dev = malloc(sizeof(struct eth_device));
	if (!dev) {
		printf("%s: %s falied to alloc eth_device (error)\n", __func__, name);
		goto error;
	}

	priv = malloc(sizeof(egigaPriv));
	if (!priv) {
		printf("%s: %s falied to alloc egiga_priv (error)\n", __func__, name);
		goto error;
	}
	memset(priv, 0, sizeof(egigaPriv));

	/* init device methods */
	memcpy(dev->name, name, NAMESIZE);
	mvMacStrToHex(enet_addr, (MV_U8 *)(dev->enetaddr));

	dev->init = (void *)mvEgigaInit;
	dev->halt = (void *)mvEgigaHalt;
	dev->send = (void *)mvEgigaTx;
	dev->recv = (void *)mvEgigaRx;
	dev->priv = priv;
	dev->iobase = 0;
	dev->write_hwaddr = 0;
	priv->port = port;
	priv->devInit = MV_FALSE;
	priv->devEnable = MV_FALSE;

	/* register the interface */
	eth_register(dev);
	return 0;
error:
	printf("%s: %s load failed\n", __func__, name);
	if (priv)
		free(dev->priv);
	if (dev)
		free(dev);
	return -1;
}


static pktInfo *mvCreatePacket(MV_VOID)
{
	pktInfo *pkt;
	MV_U8 *buf = (MV_U8 *)memalign(32, RX_BUFFER_SIZE); /* align on 32B */
	if (!buf) {
		printf("failed to alloc buffer\n");
		return NULL;
	}
	if (((MV_U32)buf) & 0xf)
		printf("un-align rx buffer %x\n", (MV_U32)buf);

	pkt = malloc(sizeof(pktInfo));
	if (pkt == NULL) {
		printf("Error: cannot allocate memory for pktInfo\n");
		free(buf);
		return NULL;
	}
	pkt->bufPhysAddr = mvOsIoVirtToPhy(NULL, buf);
	pkt->bufVirtPtr = buf;
	pkt->dataSize = 0;
	return pkt;
}



static int mvEgigaInit(struct eth_device *dev, bd_t *p)
{
	egigaPriv *priv = dev->priv;
	pktInfo *pkt;
	MV_STATUS status;
	int i;
	MV_BOARD_MAC_SPEED mac_speed;
	MV_ETH_PORT_SPEED speed = 0;
	NETA_RX_DESC *pDesc;

	/* init each port only once */
	if (priv->devInit != MV_TRUE) {
		/* init the hal -- create internal port control structure and descriptor rings, */
		/* open address decode windows, disable rx and tx operations. mask interrupts.  */
		priv->halPriv =	mvNetaPortInit(priv->port, NULL);

		if (!priv->halPriv) {
			printf("falied to init eth port (error)\n");
			goto error;
		}

		/* after init port - init rx & tx */
		MV_NETA_RXQ_CTRL *rxqCtrl = mvNetaRxqInit(priv->port, EGIGA_DEF_RXQ, EGIGA_RXQ_LEN);
		if (!rxqCtrl) {
			printf("Rxq Init Failed\n");
			goto error;
		}
		MV_NETA_TXQ_CTRL *txqCtrl = mvNetaTxqInit(priv->port, EGIGA_DEF_TXQ, EGIGA_DEF_TXP, EGIGA_TXQ_LEN);
		if (!txqCtrl) {
			printf("Txq Init Failed\n"); /* TODO: free Rxq? in Linux - no free */
			goto error;
		}
		mvNetaRxqBufSizeSet(priv->port, 0, RX_BUFFER_SIZE);

		/* fill the rxq descriptors */
		for (i = 0; i < EGIGA_RXQ_LEN; i++) {
			pkt = mvCreatePacket();
			if (!pkt)
				goto error;
			pDesc = mvNetaRxqNextDescGet(rxqCtrl);
#if defined(MV_CPU_BE)
			mvNetaRxqDescSwap(pDesc);
#endif /* MV_CPU_BE */
			mvNetaRxDescFill(pDesc, pkt->bufPhysAddr, (MV_U32)pkt);
			mvOsCacheLineFlush(NULL, pDesc);
		}
		/* update number of available descriptors */
		mvNetaRxqNonOccupDescAdd(priv->port, EGIGA_DEF_RXQ, i);

		/* Set  Phy & MAC addr */ /* Copy to AXP as is */
		mvNetaPhyAddrSet(priv->port, mvBoardPhyAddrGet(priv->port));

		/* set new addr in hw */
		if (mvNetaMacAddrSet(priv->port,  dev->enetaddr, EGIGA_DEF_RXQ) != MV_OK) {
				printf("%s: NetaSetMacAddr failed\n", dev->name);
				goto error;
		}
		priv->devInit = MV_TRUE;
	}

	mac_speed = mvBoardMacSpeedGet(priv->port);
	if (mac_speed != BOARD_MAC_SPEED_AUTO) {
		switch (mac_speed) {
		case BOARD_MAC_SPEED_10M:
			speed = MV_ETH_SPEED_10;
			break;
		case BOARD_MAC_SPEED_100M:
			speed = MV_ETH_SPEED_100;
			break;
		case BOARD_MAC_SPEED_1000M:
			speed = MV_ETH_SPEED_1000;
			break;
		default:
			/* do nothing */
			break;
		}
		if (mvNetaForceLinkModeSet(priv->port, MV_TRUE, MV_FALSE)) {
			printf("mvNetaForceLinkModeSet failed\n");
			goto error;
		}
		if (mvNetaSpeedDuplexSet(priv->port, speed, MV_ETH_DUPLEX_FULL)) {
			printf("mvNetaSpeedDuplexSet failed\n");
			goto error;
		}
	}

	/* start the hal - rx/tx activity */
	/* Check if link is up for 2 Sec */
	for (i = 1; i < 100 ; i++) {
		status = mvNetaPortEnable(priv->port);
		if (status == MV_OK) {
			priv->devEnable = MV_TRUE;
			break;
		}
		mvOsDelay(20);
	}
	if (status != MV_OK) {
		printf("%s: %s mvNetaPortEnable failed (error)\n", __func__, dev->name);
		goto error;
	}
	return 1;
error:
	if (priv->devInit)
		mvEgigaHalt(dev);

	printf("%s: %s failed\n", __func__, dev->name);
	return -1;
}


static int mvEgigaHalt(struct eth_device *dev)
{
	egigaPriv *priv = dev->priv;
	if (priv->devInit == MV_TRUE && priv->devEnable == MV_TRUE) {
		/* stop the port activity, mask all interrupts */
		if (mvNetaPortDisable(priv->port) != MV_OK)
			printf("mvNetaPortDisable failed (error)\n");
		priv->devEnable = MV_FALSE;
	}

	return 0;
}


static int mvEgigaTx(struct eth_device *dev, volatile void *buf, int len)
{
	egigaPriv *priv = dev->priv;
	MV_U32 timeout = 0;
	int txDone;
	MV_NETA_TXQ_CTRL *pTxq;
	NETA_TX_DESC *pDesc;

	if (priv->devInit != MV_TRUE || priv->devEnable != MV_TRUE)
		return 0; /* port is not initialized or not enabled */

	pTxq = mvNetaTxqHndlGet(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ);
	/* get next descriptor */
	pDesc = mvNetaTxqNextDescGet(pTxq);
	if (pDesc == NULL) {
		printf("No available descriptors\n");
		goto error;
	}

	/* set descriptor fields */
	pDesc->command = 0 | NETA_TX_L4_CSUM_NOT | NETA_TX_FLZ_DESC_MASK; /* indicates that 1st desc is also last */
	pDesc->dataSize = len;
	pDesc->bufPhysAddr = mvOsIoVirtToPhy(NULL, (void *)buf);

	mvOsCacheFlush(NULL, (void *)buf, len);
#if defined(MV_CPU_BE)
	mvNetaTxqDescSwap(pDesc);
#endif /* MV_CPU_BE */
	mvOsCacheLineFlush(NULL, (void *)pDesc);

	/* send */
	mvNetaTxqPendDescAdd(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ, 1);

	/* Tx done processing */
	txDone = mvNetaTxqSentDescNumGet(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ);
	while (!txDone) { 		/* wait for packet to be transmitted */
		if (timeout++ > 10000) {
			printf("timeout: packet not sent\n");
			goto error;
		}
		txDone = mvNetaTxqSentDescNumGet(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ);
	}
	/* txDone has increased - hw sent packet */
	mvNetaTxqSentDescDec(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ, txDone);
	return 0;

error:
	printf("%s: %s failed\n", __func__, dev->name);

	/* mvNetaTxpReset(priv->port, EGIGA_DEF_TXP); */
	return 1;
}


static int mvEgigaRx(struct eth_device *dev)
{
	egigaPriv *priv = dev->priv;
	pktInfo *pkt;
	int packets_done = 0;
	int num_recieved_packets;
	MV_U32 status;
	MV_NETA_RXQ_CTRL *pRxq;
	NETA_RX_DESC *pDesc;

	if (priv->devInit != MV_TRUE || priv->devEnable != MV_TRUE)
		return 0; /* port is not initialized or not enabled */

	pRxq = mvNetaRxqHndlGet(priv->port, EGIGA_DEF_RXQ);
	num_recieved_packets = mvNetaRxqBusyDescNumGet(priv->port, EGIGA_DEF_RXQ);
	packets_done = num_recieved_packets;
	while (num_recieved_packets--) {
		pDesc = mvNetaRxqNextDescGet(pRxq);
		/* cache invalidate - descriptor */
		mvOsCacheLineInv(NULL, pDesc);
#if defined(MV_CPU_BE)
		mvNetaRxqDescSwap(pDesc);
#endif /* MV_CPU_BE */
		status = pDesc->status;
		/* check that this buffer is 1st and last */
		if ((status & (NETA_RX_L_DESC_MASK | NETA_RX_F_DESC_MASK)) != (NETA_RX_L_DESC_MASK | NETA_RX_F_DESC_MASK)) {
#if defined(MV_CPU_BE)
			mvNetaRxqDescSwap(pDesc);
#endif /* MV_CPU_BE */
			mvOsCacheLineFlushInv(NULL, pDesc);
			continue;
		}
		/* check for errors */
		if (status & NETA_RX_ES_MASK) {
#if defined(MV_CPU_BE)
			mvNetaRxqDescSwap(pDesc);
#endif /* MV_CPU_BE */
			mvOsCacheLineFlushInv(NULL, pDesc);
			continue;
		}
		pkt = (pktInfo *)(pDesc->bufCookie);
		pkt->dataSize = pDesc->dataSize - 6; /* 2 bytes for marvell header. 4 bytes for crc */
		/* cache invalidate - packet */
		/* pPktInfo->pktSize = RX_BUFFER_SIZE; */
		mvOsCacheInvalidate(NULL, pkt->bufVirtPtr, RX_BUFFER_SIZE);
		/* give packet to stack - skip on first 2 bytes */
		NetReceive(((MV_U8 *)pkt->bufVirtPtr)+2, (int)pkt->dataSize);

		/* cache invalidate - packet */
		/* pPktInfo->pktSize = RX_BUFFER_SIZE; */
#if defined(MV_CPU_BE)
		mvNetaRxqDescSwap(pDesc);
#endif /* MV_CPU_BE */
		mvOsCacheInvalidate(NULL, pkt->bufVirtPtr, RX_BUFFER_SIZE);

	}
	/* cache invalidate - descriptor */
	mvOsCacheLineInv(NULL, pDesc);
	/* packets done == packets filled (use the same buffers so no need to refill) */
	mvNetaRxqDescNumUpdate(priv->port, EGIGA_DEF_RXQ, packets_done, packets_done);

	return 0;
}
#endif /* #if defined (MV_INCLUDE_GIG_ETH) */
