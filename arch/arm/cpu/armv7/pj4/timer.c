/*
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002-2004
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * (C) Copyright 2004
 * Philippe Robin, ARM Ltd. <philippe.robin@arm.com>
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

#define TIMER_LOAD_VAL 0xFFFFFFFF


/* macro to read the 32 bit timer */
#include "mvOs.h"
#include "cntmr/mvCntmr.h"
#include "cntmr/mvCntmrRegs.h"
#include "cpu/mvCpu.h"
#if defined(MV88F78X60)
#define MV_BOARD_REFCLK MV_BOARD_REFCLK_25MHZ
#define CTCR_ARM_TIMER_FRQ_SEL(cntr) CTCR_ARM_TIMER_25MhzFRQ_EN(cntr)	 
#else
#define MV_BOARD_REFCLK mvCpuL2ClkGet()
#define CTCR_ARM_TIMER_FRQ_SEL(cntr) 0	
#endif
#define READ_TIMER (mvCntmrRead(UBOOT_CNTR)/(MV_BOARD_REFCLK/1000))

static ulong timestamp;
static ulong lastdec;

int timer_init_done=0;

/* nothing really to do with interrupts, just starts up a counter. */
int timer_init (void)
{
	unsigned int cntmrCtrl;

	if (timer_init_done)
		return 0;

	/* init the counter */
	MV_REG_WRITE(CNTMR_RELOAD_REG(UBOOT_CNTR),TIMER_LOAD_VAL);
	MV_REG_WRITE(CNTMR_VAL_REG(UBOOT_CNTR),TIMER_LOAD_VAL);

	/* set control for timer \ cunter and enable */
	/* read control register */
	cntmrCtrl = MV_REG_READ(CNTMR_CTRL_REG(UBOOT_CNTR));
	cntmrCtrl |= CTCR_ARM_TIMER_EN(UBOOT_CNTR);
	cntmrCtrl |= CTCR_ARM_TIMER_AUTO_EN(UBOOT_CNTR);
#if !defined(CONFIG_MACH_AVANTA_LP_FPGA)
	cntmrCtrl |= CTCR_ARM_TIMER_FRQ_SEL(UBOOT_CNTR);
#endif

	MV_REG_WRITE(CNTMR_CTRL_REG(UBOOT_CNTR),cntmrCtrl);
	/* init the timestamp and lastdec value */
	reset_timer_masked();

	timer_init_done = 1;

	return 0;
}

/*
 * timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

/* delay x useconds AND perserve advance timstamp value */
#ifndef CONFIG_MARVELL
void udelay (unsigned long usec)
{
	ulong tmo, tmp;

	if(usec >= 1000){		/* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;	/* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ;		/* find number of "ticks" to wait to achieve target */
		tmo /= 1000;		/* finish normalize. */
	}else{				/* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
		tmo /= (1000*1000);
	}

	tmp = get_timer (0);		/* get current timestamp */
	if( (tmo + tmp + 1) < tmp )	/* if setting this fordward will roll time stamp */
		reset_timer_masked ();	/* reset "advancing" timestamp to 0, set lastdec value */
	else
		tmo += tmp;		/* else, set advancing stamp wake up time */

	while (get_timer_masked () < tmo)/* loop till event */
		/*NOP*/;
}
#else
/* FIXME: udelay supports only the maximum time needed for one round of the
		counter */
void __udelay (unsigned long usec)
{
    uint current;
    ulong delayticks;

	/* In case udelay is called before timier was initialized */
	if (!timer_init_done)
		timer_init();
	
	delayticks = (usec * (MV_BOARD_REFCLK/ 1000000));
	
	current = mvCntmrRead(UBOOT_CNTR);
	if(current < delayticks)
	{
		delayticks -= current;
		while(mvCntmrRead(UBOOT_CNTR) < current);
		while((TIMER_LOAD_VAL - delayticks) < mvCntmrRead(UBOOT_CNTR));
	}
	else
	{
		while(mvCntmrRead(UBOOT_CNTR) > (current-delayticks));
	}
}
#endif


void reset_timer_masked (void)
{
	/* reset time */
	lastdec = READ_TIMER;  /* capure current decrementer value time */
	timestamp = 0;	       /* start "advancing" time stamp from 0 */
}

ulong get_timer_masked (void)
{
	ulong now = READ_TIMER;		/* current tick value */

	if (lastdec >= now) {		/* normal mode (non roll) */

		/* normal mode */
		timestamp += lastdec - now; /* move stamp fordward with absoulte diff ticks */
	} else {			/* we have overflow of the count down timer */
		/* nts = ts + ld + (TLV - now)
		 * ts=old stamp, ld=time that passed before passing through -1
		 * (TLV-now) amount of time after passing though -1
		 * nts = new "advancing time stamp"...it could also roll and cause problems.
		 */
		timestamp += lastdec + (TIMER_LOAD_VAL/(MV_BOARD_REFCLK/1000))-now;

	}
	lastdec = now;

	return timestamp;
}

/* waits specified delay value and resets timestamp */
void udelay_masked (unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

	if (usec >= 1000) {		/* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;	/* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ;		/* find number of "ticks" to wait to achieve target */ tmo /= 1000;		/* finish normalize. */
	} else {			/* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
		tmo /= (1000*1000);
	}

	endtime = get_timer_masked () + tmo;

	do {
		ulong now = get_timer_masked ();
		diff = endtime - now;
	} while (diff >= 0);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

	tbclk = CONFIG_SYS_HZ;
	return tbclk;
}
