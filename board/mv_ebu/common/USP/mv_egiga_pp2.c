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

#include "pp2/gbe/mvPp2Gbe.h"
#include "pp2/bm/mvBm.h"
#include "pp2/prs/mvPp2Prs.h"
#include "pp2/prs/mvPp2PrsHw.h"
#include "pp2/cls/mvPp2Classifier.h"

#include "gpp/mvGppRegs.h"
#include "eth-phy/mvEthPhy.h"
#include "ethSwitch/mvSwitch.h"
#include "mvBoardEnvLib.h"
#include "mvSysPp2Api.h"

/******************************************************
 * driver internal definitions --                     *
 ******************************************************/
/* use only tx-queue0 and rx-queue0 */
#define EGIGA_DEF_TXQ 0
#define EGIGA_DEF_TXP 0
#define EGIGA_DEF_RXQ 0

/* rx buffer size */
#define BUFF_HDR_OFFS  32
#define BM_ALIGN       32
#define ETH_HLEN       14
#define WRAP           (2 + ETH_HLEN + 4)  /* 2(HW hdr) 14(MAC hdr) 4(CRC) 32(buffer header) */
#define MTU            1500
#define RX_PKT_SIZE    (MTU + WRAP)
#define RX_BUFFER_SIZE (RX_PKT_SIZE + BUFF_HDR_OFFS + BM_ALIGN)

/* rings length */
#define EGIGA_TXQ_HWF_LEN   16
#define EGIGA_TXQ_LEN       32
#define EGIGA_RXQ_LEN       32
#define EGIGA_TX_DESC_ALIGN 0x1F

/* BM configuration */
#define EGIGA_BM_POOL	    0
#define EGIGA_BM_SIZE	    128

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
static int mv_eth_bm_init(MV_VOID);
static int mv_eth_bm_start(MV_VOID);
static void mv_eth_bm_stop(MV_VOID);
static int mvEgigaLoad(int port, char *name, char *enet_addr);
static int mvEgigaInit(struct eth_device *dev, bd_t *p);
static int mvEgigaHalt(struct eth_device *dev);
static int mvEgigaTx(struct eth_device *dev, volatile MV_VOID *packet, int len);
static int mvEgigaRx(struct eth_device *dev);

/***********************************************************
 * mv_eth_bm_init --                                       *
 *   initialize BM pool to bu used by all ports            *
 ***********************************************************/
static int mv_eth_bm_init(MV_VOID)
{
	MV_STATUS status;
	unsigned char *pool_addr, *pool_addr_phys;

	mvBmInit();

	pool_addr = mvOsIoUncachedMalloc(NULL,
		(sizeof(MV_U32) * EGIGA_BM_SIZE) + MV_BM_POOL_PTR_ALIGN, (MV_ULONG *)&pool_addr_phys, NULL);
	if (!pool_addr) {
		printf("Can't alloc %d bytes for pool #%d\n", sizeof(MV_U32) * EGIGA_BM_SIZE, EGIGA_BM_POOL);
		return -1;
	}
	if (MV_IS_NOT_ALIGN((MV_ULONG)pool_addr_phys, MV_BM_POOL_PTR_ALIGN))
		pool_addr_phys = (unsigned char *)MV_ALIGN_UP((MV_ULONG)pool_addr_phys, MV_BM_POOL_PTR_ALIGN);

	status = mvBmPoolInit(EGIGA_BM_POOL, (MV_U32 *)pool_addr, (MV_ULONG)pool_addr_phys, EGIGA_BM_SIZE);
	if (status != MV_OK) {
		printf("Can't init #%d BM pool. status=%d\n", EGIGA_BM_POOL, status);
		return -1;
	}

#ifdef CONFIG_MV_ETH_PP2_1
	/* Disable BM priority */
	mvPp2WrReg(MV_BM_PRIO_CTRL_REG, 0);
#endif

	mvBmPoolControl(EGIGA_BM_POOL, MV_START);
	mvPp2BmPoolBufSizeSet(EGIGA_BM_POOL, RX_BUFFER_SIZE);

	return 0;
}

/***********************************************************
 * mv_eth_bm_start --                                      *
 *   enable and fill BM pool                               *
 ***********************************************************/
static int mv_eth_bm_start(MV_VOID)
{
	unsigned char *buff, *buff_phys;
	int i;

	mvBmPoolControl(EGIGA_BM_POOL, MV_START);
	mvPp2BmPoolBufSizeSet(EGIGA_BM_POOL, RX_BUFFER_SIZE);

	/* fill BM pool with buffers */
	for (i = 0; i < EGIGA_BM_SIZE; i++) {
		buff = (unsigned char *)malloc(RX_BUFFER_SIZE);
		if (!buff)
			return -1;

		buff_phys = (unsigned char *)MV_ALIGN_UP((MV_ULONG)buff, BM_ALIGN);

		mvBmPoolPut(EGIGA_BM_POOL, (MV_ULONG)buff_phys, (MV_ULONG)buff);
	}

	return 0;
}

/***********************************************************
 * mv_eth_bm_stop --                                       *
 *   empty BM pool and stop its activity                   *
 ***********************************************************/
static void mv_eth_bm_stop(MV_VOID)
{
	int i;
	unsigned char *buff;

	for (i = 0; i < EGIGA_BM_SIZE; i++) {
		buff = (unsigned char *)mvBmPoolGet(EGIGA_BM_POOL, NULL);
		free(buff);
	}

	mvBmPoolControl(EGIGA_BM_POOL, MV_STOP);
}

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

	/* HAL init + port power up + port win init */
	mvSysPp2Init();
	mv_eth_bm_init();

	/* Parser default initialization */
	if (mvPrsDefaultInit())
		printf("Warning PARSER default init failed\n");

	if (mvPp2ClassifierDefInit())
		printf("Warning Classifier defauld init failed\n");

	if (mvPp2AggrTxqInit(0/*cpu*/, EGIGA_TXQ_LEN) == NULL) {
		printf("Error failed to init aggr TXQ\n");
		return -1;
	}

	for (port = 0; port < mvCtrlEthMaxPortGet(); port++) {
		if (MV_PON_PORT(port) || mvBoardIsPortLoopback(port))
			continue;

		if (mvBoardIsEthConnected(port) == MV_FALSE)
			continue;

		if (mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, port) == MV_FALSE)
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

	/* First disable GMAC */
	mvGmacPortDisable(priv->port);

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

static int mvEgigaInit(struct eth_device *dev, bd_t *p)
{
	egigaPriv *priv = dev->priv;
	MV_STATUS status;
	int i, phys_queue, phys_port;
	MV_BOARD_MAC_SPEED mac_speed;
	MV_ETH_PORT_SPEED speed = 0;
	MV_32 phy_addr;
	static MV_U8 mac_bcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	mv_eth_bm_start();

	/* init each port only once */
	if (priv->devInit != MV_TRUE) {
		/* port power up - release from reset */
		if (!MV_PON_PORT(priv->port)) {
			phy_addr = mvBoardPhyAddrGet(priv->port);
			if (phy_addr != -1) {
				mvGmacPhyAddrSet(priv->port, phy_addr);
				mvEthPhyInit(priv->port, MV_FALSE);
			}

			mvGmacPortPowerUp(priv->port,
				MV_FALSE/*mvBoardIsPortInSgmii(priv->port)*/,
				MV_FALSE/*mvBoardIsPortInRgmii(priv->port)*/);
		}

		/* init the hal -- create internal port control structure and descriptor rings, */
		/* open address decode windows, disable rx and tx operations. mask interrupts.  */
		priv->halPriv =	mvPp2PortInit(priv->port, priv->port * CONFIG_MV_ETH_RXQ, CONFIG_MV_ETH_RXQ, NULL);

		if (!priv->halPriv) {
			printf("falied to init eth port (error)\n");
			goto error;
		}

		/* after init port - init rx & tx */
		MV_PP2_PHYS_RXQ_CTRL *rxqCtrl = mvPp2RxqInit(priv->port, EGIGA_DEF_RXQ, EGIGA_RXQ_LEN);
		if (!rxqCtrl) {
			printf("Rxq Init Failed\n");
			goto error;
		}
		MV_PP2_PHYS_TXQ_CTRL *txqCtrl =
			mvPp2TxqInit(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ, EGIGA_TXQ_LEN, EGIGA_TXQ_HWF_LEN);
		if (!txqCtrl) {
			printf("Txq Init Failed\n");
			goto error;
		}
		mvPp2RxqBmLongPoolSet(priv->port, EGIGA_DEF_RXQ, EGIGA_BM_POOL);
		mvPp2RxqBmShortPoolSet(priv->port, EGIGA_DEF_RXQ, EGIGA_BM_POOL);

		mvPp2RxqNonOccupDescAdd(priv->port, EGIGA_DEF_RXQ, EGIGA_RXQ_LEN);

		mvPp2TxpMaxTxSizeSet(priv->port, EGIGA_DEF_TXP, RX_PKT_SIZE);

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
		if (mvGmacForceLinkModeSet(priv->port, MV_TRUE, MV_FALSE)) {
			printf("mvEthForceLinkModeSet failed\n");
			goto error;
		}
		if (mvGmacSpeedDuplexSet(priv->port, speed, MV_ETH_DUPLEX_FULL)) {
			printf("mvEthSpeedDuplexSet failed\n");
			goto error;
		}
		if (mvGmacFlowCtrlSet(priv->port, MV_ETH_FC_DISABLE)) {
			printf("mvEthFlowCtrlSet failed\n");
			goto error;
		}
	}

	/* allow new packets to RXQs */
	mvPp2PortIngressEnable(priv->port, 1);

	/* classifier port default config */
	phys_queue = mvPp2LogicRxqToPhysRxq(priv->port, EGIGA_DEF_RXQ);
	phys_port = MV_PPV2_PORT_PHYS(priv->port);

	mvPp2ClsHwPortDefConfig(phys_port, 0, FLOWID_DEF(phys_port), phys_queue);

	if (mvPrsMacDaAccept(phys_port, mac_bcast, 1 /*add*/)) {
		printf("%s: mvPrsMacDaAccept failed\n", dev->name);
		return -1;
	}
	if (mvPrsMacDaAccept(phys_port, dev->enetaddr, 1 /*add*/)) {
		printf("%s: mvPrsMacDaAccept failed\n", dev->name);
		return -1;
	}
	if (mvPrsDefFlow(phys_port)) {
		printf("%s: mvPp2PrsDefFlow failed\n", dev->name);
		return -1;
	}

	/* start the hal - rx/tx activity */
	/* Check if link is up for 2 Sec */
	for (i = 1; i < 100; i++) {
		status = mvPp2PortEnable(priv->port, 1);
		if (status == MV_OK) {
			priv->devEnable = MV_TRUE;
			break;
		}
		mvOsDelay(20);
	}
	if (status != MV_OK) {
		printf("%s: %s mvPp2PortEnable failed (error)\n", __func__, dev->name);
		goto error;
	}

	mvPp2PortEgressEnable(priv->port, 1);

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
		/* stop new packets from arriving to RXQs */

		mvPp2PortIngressEnable(priv->port, 0);

		/* stop the port activity, mask all interrupts */
		if (mvPp2PortEnable(priv->port, 0) != MV_OK)
			printf("mvPp2PortDisable failed (error)\n");
		priv->devEnable = MV_FALSE;
	}

	mv_eth_bm_stop();

	mvGmacPortDisable(priv->port);

	return 0;
}


static int mvEgigaTx(struct eth_device *dev, volatile void *buf, int len)
{
	egigaPriv *priv = dev->priv;
	MV_U32 timeout = 0;
	int txDone;
	MV_PP2_AGGR_TXQ_CTRL *pAggrTxq;
	PP2_TX_DESC *pDesc;

	if (priv->devInit != MV_TRUE || priv->devEnable != MV_TRUE)
		return 0; /* port is not initialized or not enabled */

	pAggrTxq = mvPp2AggrTxqHndlGet(0);

	/* get next descriptor */
	pDesc = mvPp2AggrTxqNextDescGet(pAggrTxq);
	if (pDesc == NULL) {
		printf("No available descriptors\n");
		goto error;
	}

	/* set descriptor fields */
	pDesc->command = 0 | PP2_TX_L4_CSUM_NOT | PP2_TX_F_DESC_MASK | PP2_TX_L_DESC_MASK;
	pDesc->dataSize = len;
	pDesc->pktOffset = (MV_U32)buf & EGIGA_TX_DESC_ALIGN;
	pDesc->bufPhysAddr = (MV_U32)buf & (~EGIGA_TX_DESC_ALIGN);
	pDesc->bufCookie = (MV_U32)buf;
	pDesc->physTxq = MV_PPV2_TXQ_PHYS(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ);

	mvOsCacheFlush(NULL, (void *)buf, len);
#if defined(MV_CPU_BE)
	mvNetaTxqDescSwap(pDesc);//TODO
#endif /* MV_CPU_BE */
	mvOsCacheLineFlush(NULL, (void *)pDesc);

	/* send */
	mvPp2AggrTxqPendDescAdd(1);

	/* Enable TXQ drain */
	mvPp2TxqDrainSet(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ, MV_TRUE);

	/* Tx done processing */
	/* wait for agrregated to physical TXQ transfer */
	txDone = mvPp2AggrTxqPendDescNumGet(0);
	while (txDone) {
		if (timeout++ > 10000) {
			printf("timeout: packet not sent from aggregated to phys TXQ\n");
			goto error;
		}
		txDone = mvPp2AggrTxqPendDescNumGet(0);
	}

	/* Disable TXQ drain */
	mvPp2TxqDrainSet(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ, MV_FALSE);

	timeout = 0;
	txDone = mvPp2TxqSentDescProc(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ);
	/* wait for packet to be transmitted */
	while (!txDone) { 
		if (timeout++ > 10000) {
			printf("timeout: packet not sent\n");
			goto error;
		}
		txDone = mvPp2TxqSentDescProc(priv->port, EGIGA_DEF_TXP, EGIGA_DEF_TXQ);
	}
	/* txDone has increased - hw sent packet */

	return 0;

error:
	printf("%s: %s failed\n", __func__, dev->name);

	/* mvNetaTxpReset(priv->port, EGIGA_DEF_TXP); */
	return 1;
}

static int mvEgigaRx(struct eth_device *dev)
{
	egigaPriv *priv = dev->priv;
	MV_U8 *pkt;
	int packets_done = 0;
	int num_recieved_packets, pool_id;
	MV_U32 status;
	MV_PP2_PHYS_RXQ_CTRL *pRxq;
	PP2_RX_DESC *pDesc;

	if (priv->devInit != MV_TRUE || priv->devEnable != MV_TRUE)
		return 0; /* port is not initialized or not enabled */

	pRxq = mvPp2RxqHndlGet(priv->port, EGIGA_DEF_RXQ);
	num_recieved_packets = mvPp2RxqBusyDescNumGet(priv->port, EGIGA_DEF_RXQ);
	packets_done = num_recieved_packets;

	while (num_recieved_packets--) {
		pDesc = mvPp2RxqNextDescGet(pRxq);
		/* cache invalidate - descriptor */
		mvOsCacheLineInv(NULL, pDesc);
#if defined(MV_CPU_BE)
		mvNetaRxqDescSwap(pDesc);//TODO
#endif /* MV_CPU_BE */
		status = pDesc->status;

		/* drop packets with error or with buffer header (MC, SG) */
		if ((status & PP2_RX_BUF_HDR_MASK) || (status & PP2_RX_ES_MASK)) {
#if defined(MV_CPU_BE)
			mvNetaRxqDescSwap(pDesc);//TODO
#endif /* MV_CPU_BE */
			mvOsCacheLineFlushInv(NULL, pDesc);
			continue;
		}
		/* TODO: drop fragmented packets */

		/* cache invalidate - packet */
		mvOsCacheInvalidate(NULL, (void *)pDesc->bufPhysAddr, RX_BUFFER_SIZE);

		/* give packet to stack - skip on first 2 bytes + buffer header */
		pkt = ((MV_U8 *)pDesc->bufPhysAddr) + 2 + BUFF_HDR_OFFS;
		NetReceive(pkt, (int)pDesc->dataSize - 2);

		/* refill: pass packet back to BM */
		pool_id = (status & PP2_RX_BM_POOL_ALL_MASK) >> PP2_RX_BM_POOL_ID_OFFS;
		mvBmPoolPut(pool_id, (MV_ULONG) pDesc->bufPhysAddr, (MV_ULONG) pDesc->bufCookie);

		/* cache invalidate - packet */
#if defined(MV_CPU_BE)
		mvNetaRxqDescSwap(pDesc);//TODO
#endif /* MV_CPU_BE */
		mvOsCacheInvalidate(NULL, (void *)pDesc->bufPhysAddr, RX_BUFFER_SIZE);

	}
	/* cache invalidate - descriptor */
	mvOsCacheLineInv(NULL, pDesc);

	mvPp2RxqDescNumUpdate(priv->port, EGIGA_DEF_RXQ, packets_done, packets_done);

	return 0;
}
#endif /* #if defined (MV_INCLUDE_GIG_ETH) */
