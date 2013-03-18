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
#include <net.h>
#include <malloc.h>

#if defined (MV_INCLUDE_GIG_ETH)
#if defined(MV_ETH_LEGACY)
//#include "sys/mvSysGbe.h"
#include "mvOs.h"
#include "mvSysHwConfig.h"
#include "eth/mvEth.h"
#include "gpp/mvGppRegs.h"
#include "eth/gbe/mvEthGbe.h"
#include "eth-phy/mvEthPhy.h"
#include "ethSwitch/mvSwitch.h"
#include "mvBoardEnvLib.h"
#include "mvSysEthApi.h"

//#define MV_DEBUG
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/******************************************************
* driver internal definitions --                     *
******************************************************/
/* use only tx-queue0 and rx-queue0 */
#define EGIGA_DEF_TXQ 0
#define EGIGA_DEF_RXQ 0

/* rx buffer size */
#define ETH_HLEN       14
#define WRAP           (2 + ETH_HLEN + 4 + 32)  /* 2(HW hdr) 14(MAC hdr) 4(CRC) 32(extra for cache prefetch)*/
#define MTU            1500
#define RX_BUFFER_SIZE (MTU + WRAP)

/* rings length */
#define EGIGA_TXQ_LEN   20
#define EGIGA_RXQ_LEN   20

typedef struct _egigaPriv {
	int port;
	MV_VOID *halPriv;
	MV_U32 rxqCount;
	MV_U32 txqCount;
	MV_BOOL devInit;
} egigaPriv;

/******************************************************
* functions prototype --                             *
******************************************************/
static int mvEgigaLoad(int port, char *name, char *enet_addr);

static int mvEgigaInit(struct eth_device *dev, bd_t *p);
static int mvEgigaHalt(struct eth_device *dev);
static int mvEgigaTx(struct eth_device *dev, volatile MV_VOID *packet, int len);
static int mvEgigaRx(struct eth_device *dev);

static MV_PKT_INFO* mvEgigaRxFill(MV_VOID);

/***********************************************************
* mv_eth_initialize --                                    *
*   main driver initialization. loading the interfaces.   *
***********************************************************/
int mv_eth_initialize(bd_t *bis)
{
	int port;
	MV_8 *enet_addr;
	MV_8 name[NAMESIZE + 1];
	MV_8 enetvar[9];

	mvSysEthInit();

	for (port = 0; port < mvCtrlEthMaxPortGet(); port++) {
		if (MV_FALSE ==  mvBoardIsGbEPortConnected(port)) continue;

		if (MV_FALSE == mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, port)) continue;

		/* interface name */
		sprintf(name, "egiga%d", port);
		/* interface MAC addr extract */
		sprintf(enetvar, port ? "eth%daddr" : "ethaddr", port);
		enet_addr = getenv(enetvar);

		mvEthPortPowerUp(port);

		MV_REG_WRITE(ETH_TX_QUEUE_COMMAND1_REG(port), 0x8);
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
	ETH_PORT_CTRL dummy_port_handle;

	DB(printf("%s: %s load - ", __FUNCTION__, name) );

	dev = malloc(sizeof(struct eth_device) );
	priv = malloc(sizeof(egigaPriv) );

	if ( !dev) {
		DB(printf("%s: %s falied to alloc eth_device (error)\n", __FUNCTION__, name) );
		goto error;
	}

	if ( !priv) {
		DB(printf("%s: %s falied to alloc egiga_priv (error)\n", __FUNCTION__, name) );
		goto error;
	}

	memset(priv, 0, sizeof(egigaPriv) );

	/* init device methods */
	memcpy(dev->name, name, NAMESIZE);
	mvMacStrToHex(enet_addr, (MV_U8*)(dev->enetaddr));

	/* set MAC addres even if port was not used yet. */
	dummy_port_handle.portNo = port;

	mvEthMacAddrSet(&dummy_port_handle, dev->enetaddr, EGIGA_DEF_RXQ);

	dev->init = (void*)mvEgigaInit;
	dev->halt = (void*)mvEgigaHalt;
	dev->send = (void*)mvEgigaTx;
	dev->recv = (void*)mvEgigaRx;
	dev->priv = priv;
	dev->iobase = 0;
	dev->port = port;
	priv->port = port;

	/* register the interface */
	eth_register(dev);

	DB(printf("%s: %s load ok\n", __FUNCTION__, name) );
	return 0;

error:
	printf("%s: %s load failed\n", __FUNCTION__, name);
	if ( priv) free(dev->priv);
	if ( dev) free(dev);
	return -1;
}

static MV_PKT_INFO* mvEgigaRxFill(MV_VOID)
{
	MV_BUF_INFO *pBufInfo;
	MV_PKT_INFO *pPktInfo;
	MV_U8 *buf = (MV_U8*)memalign(32, RX_BUFFER_SIZE);    /* align on 32B */

	if ( !buf) {
		DB(printf("failed to alloc buffer\n"));
		return NULL;
	}

	if ( ((MV_U32)buf) & 0xf)
		printf("un-align rx buffer %x\n", (MV_U32)buf);

	pPktInfo = malloc(sizeof(MV_PKT_INFO));
	if (pPktInfo == NULL) {
		printf("Error: cannot allocate memory for pktInfo\n");
		free(buf);
		return NULL;
	}

	pBufInfo = malloc(sizeof(MV_BUF_INFO));
	if (pBufInfo == NULL) {
		printf("Error: cannot allocate memory for bufInfo\n");
		free(buf);
		free(pPktInfo);
		return NULL;
	}
	pBufInfo->bufPhysAddr = mvOsIoVirtToPhy(NULL, buf);
	pBufInfo->bufVirtPtr = buf;
	pBufInfo->bufSize = RX_BUFFER_SIZE;
	pBufInfo->dataSize = 0;
	pPktInfo->osInfo = (MV_ULONG)buf;
	pPktInfo->pFrags = pBufInfo;
	pPktInfo->pktSize = RX_BUFFER_SIZE; /* how much to invalidate */
	pPktInfo->numFrags = 1;
	pPktInfo->status = 0;
	pPktInfo->ownerId = -1;
	return pPktInfo;
}

unsigned int egiga_init = 0;

static int mvEgigaInit(struct eth_device *dev, bd_t *p)
{
	egigaPriv *priv = dev->priv;
	MV_ETH_PORT_INIT halInitStruct;
	MV_PKT_INFO *pktInfo;
	MV_STATUS status;
	int i;

	DB(printf("%s: %s init - ", __FUNCTION__, dev->name) );

	/* egiga not ready */
	DB(printf("mvBoardPhyAddrGet()=0x%x , priv->port =0x%x\n", mvBoardPhyAddrGet(priv->port), priv->port));

	/* If speed is not auto then link is force */
	if (BOARD_MAC_SPEED_AUTO == mvBoardMacSpeedGet(priv->port)) {
		/* Check Link status on phy */
		if ( mvEthPhyCheckLink(mvBoardPhyAddrGet(priv->port) ) == MV_FALSE) {
			printf("%s no link\n", dev->name);
			return 0;
		}else
			DB(printf("link up\n") );
	}

	egiga_init = 1;

	/* init the hal -- create internal port control structure and descriptor rings, */
	/* open address decode windows, disable rx and tx operations. mask interrupts.  */
	halInitStruct.maxRxPktSize = RX_BUFFER_SIZE;
	halInitStruct.rxDefQ = EGIGA_DEF_RXQ;

	halInitStruct.txDescrNum[0] = EGIGA_TXQ_LEN;
	halInitStruct.rxDescrNum[0] = EGIGA_RXQ_LEN;
	halInitStruct.osHandle = NULL;

	priv->halPriv = mvEthPortInit(priv->port, &halInitStruct);

	if ( !priv->halPriv) {
		DB(printf("falied to init eth port (error)\n") );
		goto error;
	}

	/* set new addr in hw */
	if ( mvEthMacAddrSet(priv->halPriv, dev->enetaddr, EGIGA_DEF_RXQ) != MV_OK) {
		printf("%s: ethSetMacAddr failed\n", dev->name);
		goto error;
	}

	priv->devInit = MV_TRUE;

	/* fill rx ring with buffers */
	for ( i = 0; i < EGIGA_RXQ_LEN; i++) {
		pktInfo = mvEgigaRxFill();
		if (pktInfo == NULL)
			goto error;

		/* give the buffer to hal */
		status = mvEthPortRxDone(priv->halPriv, EGIGA_DEF_RXQ, pktInfo);
		if ( status == MV_OK)
			priv->rxqCount++;
		else if ( status == MV_FULL) {
			/* the ring is full */
			priv->rxqCount++;
			DB(printf("ring full\n") );
			break;
		}else  {
			printf("error\n");
			goto error;
		}
	}

#ifdef MV_DEBUG
	ethPortQueues(priv->port, EGIGA_DEF_RXQ, EGIGA_DEF_TXQ, 1);

	printf("%s : after calling ethPortQueues\n", __FUNCTION__);

#endif

	/* start the hal - rx/tx activity */
	/* Check if link is up for 2 Sec */
	for (i = 1; i < 100; i++) {
		status = mvEthPortEnable(priv->halPriv);
		if (status == MV_OK)
			break;
		mvOsDelay(20);
	}

	if ( status != MV_OK) {
		printf("%s: %s mvEthPortEnable failed (error)\n", __FUNCTION__, dev->name);
		goto error;
	}

#ifdef MV_DEBUG
	ethRegs(priv->port);
	ethPortRegs(priv->port);
	ethPortStatus(priv->port);

	ethPortQueues(priv->port, EGIGA_DEF_RXQ, -1 /*EGIGA_DEF_TXQ*/, 0);
#endif

	DB(printf("%s: %s complete ok\n", __FUNCTION__, dev->name) );
	return 1;

error:
	if ( priv->devInit)
		mvEgigaHalt(dev);
	printf("%s: %s failed\n", __FUNCTION__, dev->name);
	return 0;
}

static int mvEgigaHalt(struct eth_device *dev)
{
	egigaPriv *priv = dev->priv;
	MV_PKT_INFO *pktInfo;

	DB(printf("%s: %s halt - ", __FUNCTION__, dev->name) );
	if ( priv->devInit == MV_TRUE) {
		/* stop the port activity, mask all interrupts */
		if ( mvEthPortDisable(priv->halPriv) != MV_OK)
			printf("mvEthPortDisable failed (error)\n");

		/* free the buffs in the rx ring */
		while ( (pktInfo = mvEthPortForceRx(priv->halPriv, EGIGA_DEF_RXQ)) != NULL) {
			priv->rxqCount--;
			if ( pktInfo->osInfo)
				free( (void*)pktInfo->osInfo);
			else
				printf("mvEthPortForceRx failed (error)\n");
			if ( pktInfo->pFrags)
				free( (void*)pktInfo->pFrags);
			else
				printf("mvEthPortForceRx failed (error)\n");
			free( (void*)pktInfo);
		}

		/* Clear Cause registers (must come before mvEthPortFinish) */
		MV_REG_WRITE(ETH_INTR_CAUSE_REG(((ETH_PORT_CTRL*)(priv->halPriv))->portNo), 0);
		MV_REG_WRITE(ETH_INTR_CAUSE_EXT_REG(((ETH_PORT_CTRL*)(priv->halPriv))->portNo), 0);

		/* Clear Cause registers */
		MV_REG_WRITE(ETH_INTR_CAUSE_REG(((ETH_PORT_CTRL*)(priv->halPriv))->portNo), 0);
		MV_REG_WRITE(ETH_INTR_CAUSE_EXT_REG(((ETH_PORT_CTRL*)(priv->halPriv))->portNo), 0);

		mvEthPortFinish(priv->halPriv);
		priv->devInit = MV_FALSE;

	}
	egiga_init = 0;

	DB(printf("%s: %s complete\n", __FUNCTION__, dev->name) );
	return 0;
}

static int mvEgigaTx(struct eth_device *dev, volatile void *buf, int len)
{
	egigaPriv *priv = dev->priv;
	MV_BUF_INFO bufInfo;
	MV_PKT_INFO pktInfo;
	MV_PKT_INFO *pPktInfo;
	MV_STATUS status;
	MV_U32 timeout = 0;

	DB(printf("mvEgigaTx start\n") );
	/* if no link exist */
	if (!egiga_init)
		return 0;

	pktInfo.osInfo = (MV_ULONG)0x44CAFE44;
	pktInfo.pktSize = len;
	pktInfo.pFrags = &bufInfo;
	pktInfo.status = 0;
	pktInfo.numFrags = 1;
	bufInfo.bufVirtPtr = (MV_U8*)buf;
	bufInfo.bufPhysAddr = mvOsIoVirtToPhy(NULL, buf);
	bufInfo.dataSize = len;

	/* send the packet */
	status = mvEthPortTx(priv->halPriv, EGIGA_DEF_TXQ, &pktInfo);

	if ( status != MV_OK) {
		if ( status == MV_NO_RESOURCE)
			DB(printf("ring is full (error)\n") );
		else if ( status == MV_ERROR)
			printf("error\n");
		else
			printf("unrecognize status (error) ethPortSend\n");
		goto error;
	}else DB(printf("ok\n") );

	priv->txqCount++;

	/* release the transmitted packet(s) */
	while ( 1) {
		if (timeout++ > 100)
			break;
		DB(printf("%s: %s tx-done - ", __FUNCTION__, dev->name) );

		/* get a packet */
		pPktInfo = mvEthPortTxDone(priv->halPriv, EGIGA_DEF_TXQ);

		if ( pPktInfo != NULL) {
			priv->txqCount--;

			/* validate skb */
			if ( (pPktInfo != &pktInfo) || (pPktInfo->osInfo != 0x44CAFE44 ) ) {
				printf("error\n");
				goto error;
			}

			/* handle tx error */
			if ( pPktInfo->status & (ETH_ERROR_SUMMARY_BIT) ) {
				printf("bad status (error)\n");
				goto error;
			}
			DB(printf("ok\n") );
			break;
		}else
			DB(printf("NULL pPktInfo\n"));
	}

	DB(printf("%s: %s complete ok\n", __FUNCTION__, dev->name) );
	return 0;

error:
	printf("%s: %s failed\n", __FUNCTION__, dev->name);
	return 1;
}

static int mvEgigaRx(struct eth_device *dev)
{
	egigaPriv*  priv = dev->priv;
	MV_PKT_INFO *pktInfo;
	MV_STATUS status;

	/* if no link exist */
	if (!egiga_init) return 0;

	while ( 1) {
		/* get rx packet from hal */
		pktInfo = mvEthPortRx(priv->halPriv, EGIGA_DEF_RXQ);

		if ( pktInfo != NULL) {
			/*DB( printf( "good rx\n" ) );*/
			priv->rxqCount--;

			/* check rx error status */
			if ( pktInfo->status & (ETH_ERROR_SUMMARY_MASK) ) {
				MV_U32 err = pktInfo->status & ETH_RX_ERROR_CODE_MASK;
				/*DB( printf( "bad rx status %08x, ", (MV_U32)pktInfo->status ) );*/
				if ( err == ETH_RX_RESOURCE_ERROR)
					DB(printf("(resource error)") );
				else if ( err == ETH_RX_MAX_FRAME_LEN_ERROR)
					DB(printf("(max frame length error)") );
				else if ( err == ETH_RX_OVERRUN_ERROR)
					DB(printf("(overrun error)") );
				else if ( err == ETH_RX_CRC_ERROR)
					DB(printf("(crc error)") );
				else {
					DB(printf("(unknown error)") );
					goto error;
				}
				DB(printf("\n") );
			}else  {
				DB(printf("%s: %s calling NetRecieve ", __FUNCTION__, dev->name) );
				DB(printf("%s: calling NetRecieve pkInfo = 0x%x\n", __FUNCTION__, pktInfo) );
				DB(printf("%s: calling NetRecieve osInfo = 0x%x\n", __FUNCTION__, pktInfo->osInfo) );
				DB(printf("%s: calling NetRecieve pktSize = 0x%x\n", __FUNCTION__, pktInfo->pFrags->dataSize) );
				/* good rx - push the packet up (skip on two first empty bytes) */
				NetReceive( ((MV_U8*)pktInfo->osInfo) + 2, (int)pktInfo->pFrags->dataSize);
			}

			DB(printf("%s: %s refill rx buffer - ", __FUNCTION__, dev->name) );

			/* give the buffer back to hal (re-init the buffer address) */
			pktInfo->pktSize = RX_BUFFER_SIZE; /* how much to invalidate */
			status = mvEthPortRxDone(priv->halPriv, EGIGA_DEF_RXQ, pktInfo);

			if ( status == MV_OK)
				priv->rxqCount++;
			else if ( status == MV_FULL) {
				/* this buffer made the ring full */
				priv->rxqCount++;
				DB(printf("ring full\n") );
				break;
			}else  {
				printf("error\n");
				goto error;
			}

		} else {
			/* no more rx packets ready */
			/*DB( printf( "no more work\n" ) );*/
			break;
		}
	}

	/*DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );*/
	return 0;

error:
	DB(printf("%s: %s failed\n", __FUNCTION__, dev->name) );
	return 1;
}

#endif  /* legacy */
#endif  /* #if defined (MV_INCLUDE_GIG_ETH) */