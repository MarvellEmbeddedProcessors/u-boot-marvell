/*
 * Driver for SysKonnect Gigabit Ethernet Server Adapters.
 *
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#if defined(CONFIG_CMD_NET) && defined(CONFIG_NET_MULTI) && defined(CONFIG_SK98)

#include "h/skdrv1st.h"
#include "h/skdrv2nd.h"
#include "u-boot_compat.h"


#define SKGE_MAX_CARDS	4


extern int skge_probe(struct eth_device **);
extern void SkGeIsr(int irq, void *dev_id, struct pt_regs *ptregs);
extern void SkGeIsrOnePort(int irq, void *dev_id, struct pt_regs *ptregs);
/* Marvell - Yukon2 support*/
extern void SkY2Isr(int irq, void *dev_id, struct pt_regs *ptregs);
extern int SkGeOpen(struct eth_device *);
extern int SkGeClose(struct eth_device *);
extern int SkGeXmit(struct sk_buff *skb, struct eth_device *dev);
/* Marvell - Yukon2 support*/
extern int SkY2Xmit(struct sk_buff *skb, struct eth_device *dev);
extern void ReceiveIrq(SK_AC *pAC, RX_PORT *pRxPort, SK_BOOL SlowPathLock);

static int skge_init(struct eth_device *dev, bd_t * bis);
static int skge_send(struct eth_device *dev, volatile void *packet, int length);
static int skge_recv(struct eth_device *dev);
static void skge_halt(struct eth_device *dev);

int skge_initialize(bd_t * bis)
{
	int numdev,i ;
	struct eth_device *dev[SKGE_MAX_CARDS];

	numdev = skge_probe(&dev[0]);

	if (numdev > SKGE_MAX_CARDS)
	{
		printf("ERROR: numdev > SKGE_MAX_CARDS\n");
	}

	for (i = 0; i < numdev; i++)
	{
		sprintf (dev[i]->name, "SK98#%d", i);

		dev[i]->init = skge_init;
		dev[i]->halt = skge_halt;
		dev[i]->send = skge_send;
		dev[i]->recv = skge_recv;

		eth_register(dev[i]);
	}
	return numdev;
}

/* ronen - since if we try to halt the dev after it was halted it cause a stuck I add this
 global variable */
int sk98_inited = 0;

static int skge_init(struct eth_device *dev, bd_t * bis)
{
	int ret;
	SK_AC * pAC = ((DEV_NET*)dev->priv)->pAC;
	int i = 0;

	/* ronen */
	if(sk98_inited == 1)
		skge_halt(dev);

	ret = SkGeOpen(dev);

	while (pAC->Rlmt.Port[0].PortState != SK_RLMT_PS_GOING_UP)
	{
		/* ronen - since if the link on the sk98 is down we don't want to wait here for ever */
		/* so I adde a counter */
		i++;

		/* Marvell - Yukon2 support*/
		if (CHIP_ID_YUKON_2(pAC)) {
		  SkY2Isr (0, pAC->dev[0], 0);
		  if(i > 1500000){
			  ret = 1;
			  break;
		  }

			ret = 0;
		}
		else {
		  SkGeIsrOnePort (0, pAC->dev[0], 0);
		  if(i > 1500000){
			  ret = 1;
			  break;
		  }

		}
	}

//	for (i = 0; i < 100; i ++)
//	{
//		udelay(1000);
//	}

	/* ronen - for the SkGeOpen 0 is success and 1 is fail */
	/* the return except 1 for success */
	if(ret == 0){
		sk98_inited = 1;
		return 1;
	}
	skge_halt(dev);
	return 0;
}

static void skge_halt(struct eth_device *dev)
{
	DEV_NET         *pNet    = (DEV_NET*) dev->priv;
	SK_AC           *pAC     = pNet->pAC;


	/* ronen */
	if(sk98_inited == 0)
		return;


	/* Marvell - Yukon2 support*/
	/* to be in the safe side we call SkY2Isr to treat any
	pending rx or tx complete */
	if (CHIP_ID_YUKON_2(pAC))
	{
		  SkY2Isr (0, pAC->dev[0], 0);
	}

	SkGeClose(dev);


	sk98_inited = 0;
}


static int skge_send(struct eth_device *dev, volatile void *packet,
						  int length)
{
	int ret = -1;
	struct sk_buff * skb = alloc_skb(length,0);
	DEV_NET         *pNet    = (DEV_NET*) dev->priv;
	SK_AC           *pAC     = pNet->pAC;

	if (! skb)
	{
		printf("skge_send: failed to alloc skb\n");
		goto Done;
	}

	memcpy(skb->data, (void*)packet, length);

	/* Marvell - Yukon2 support*/
	if (CHIP_ID_YUKON_2(pAC)) {
	  ret = SkY2Xmit(skb,dev);
	}
	else {
	  ret = SkGeXmit(skb, dev);
	}
Done:
	return ret;
}


static int skge_recv(struct eth_device *dev)
{
	DEV_NET		*pNet;
	SK_AC		*pAC;
	int		FromPort = 0;

	pNet = (DEV_NET*) dev->priv;
	pAC = pNet->pAC;

	/* Marvell - Yukon2 support*/
	if (CHIP_ID_YUKON_2(pAC)) {

	  SkY2Isr (0, pAC->dev[0], 0);
	}
	else {

	  ReceiveIrq(pAC, &pAC->RxPort[FromPort], SK_FALSE);
	}

	return 0;
}


#endif	/* CONFIG_SK98 */
