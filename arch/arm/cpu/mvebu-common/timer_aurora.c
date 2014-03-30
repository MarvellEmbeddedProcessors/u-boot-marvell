/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/regs-base.h>
#include <asm/arch-mvebu/clock.h>



/* Global timer source clock is the Punit clock (which is L2 clock divided by 2) */
#ifdef MV88F68XX
#define MV_BOARD_REFCLK MV_BOARD_REFCLK_25MHZ
#else
#define MV_BOARD_REFCLK (soc_l2_clk_get() / 2)
#endif

#define TIMER_BASE(x)			MVEBU_GLOBAL_TIMER_BASE
#define TIMER_CTRL_REG(x)		TIMER_BASE(x)
#define TIMER_RELOAD_REG(x)		(TIMER_BASE(x) + 0x10 + (x * 8))
#define TIMER_VAL_REG(x)		(TIMER_BASE(x) + 0x14 + (x * 8))

#define TIMER_ENABLE(x)			(1 << (0 + (2 * x)))
#define TIMER_AUTO(x)			(1 << (1 + (2 * x)))
#define TIMER_25MHZ(x)			(1 << (11 +  x))

#define TIMER_LOAD_VAL 0xFFFFFFFF

static ulong timestamp;
static ulong lastdec;

int timer_init_done;

int read_timer(void)
{
	return readl(TIMER_VAL_REG(0));
}

int timer_init(void)
{
	unsigned int ctrl;

	if (timer_init_done)
		return 0;

	/* init the counter */
	writel(TIMER_LOAD_VAL, TIMER_RELOAD_REG(0));
	writel(TIMER_LOAD_VAL, TIMER_VAL_REG(0));

	/* set control for timer cunter and enable */
	ctrl = readl(TIMER_CTRL_REG(0));
	ctrl |= TIMER_ENABLE(0);
	ctrl |= TIMER_AUTO(0);
#ifdef MV88F68XX
	ctrl |= TIMER_25MHZ(0);
#else
	ctrl &= ~TIMER_25MHZ(0);
#endif
	writel(ctrl, TIMER_CTRL_REG(0));

	/* init the timestamp and lastdec value */
	reset_timer_masked();

	timer_init_done = 1;

	return 0;
}

void reset_timer(void)
{
	reset_timer_masked();
}

ulong get_timer(ulong base)
{
	return get_timer_masked() - base;
}

void set_timer(ulong t)
{
	timestamp = t;
}

/* delay x useconds AND perserve advance timstamp value */
#ifndef CONFIG_MARVELL
void udelay(unsigned long usec)
{
	ulong tmo, tmp;

	if (usec >= 1000) {             /* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;      /* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ;   /* find number of "ticks" to wait to achieve target */
		tmo /= 1000;            /* finish normalize. */
	} else {                          /* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
		tmo /= (1000 * 1000);
	}

	tmp = get_timer(0);                     /* get current timestamp */
	if ((tmo + tmp + 1) < tmp)             /* if setting this fordward will roll time stamp */
		reset_timer_masked();           /* reset "advancing" timestamp to 0, set lastdec value */
	else
		tmo += tmp;                     /* else, set advancing stamp wake up time */

	while (get_timer_masked() < tmo)        /* loop till event */
		/*NOP*/;
}

#else
/* FIXME: udelay supports only the maximum time needed for one round of the counter */
void __udelay(unsigned long usec)
{
	uint current;
	ulong delayticks;

	/* In case udelay is called before timier was initialized */
	if (!timer_init_done)
		timer_init();

	delayticks = (usec * (MV_BOARD_REFCLK / 1000000));

	current = read_timer();
	if (current < delayticks) {
		delayticks -= current;
		while (read_timer() < current)
			/*NOP*/;
		while ((TIMER_LOAD_VAL - delayticks) < read_timer())
			/*NOP*/;
	} else {
		while (read_timer() > (current - delayticks))
			/*NOP*/;
	}
}

#endif

void reset_timer_masked(void)
{
	/* reset time */
	lastdec = read_timer() / (MV_BOARD_REFCLK / 1000);
	timestamp = 0;
}

ulong get_timer_masked(void)
{
	ulong now = read_timer() / (MV_BOARD_REFCLK / 1000);

	if (lastdec >= now) {                   /* normal mode (non roll) */
		/* normal mode */
		timestamp += lastdec - now;     /* move stamp fordward with absoulte diff ticks */
	} else {                                /* we have overflow of the count down timer */
		/* nts = ts + ld + (TLV - now)
		 * ts=old stamp, ld=time that passed before passing through -1
		 * (TLV-now) amount of time after passing though -1
		 * nts = new "advancing time stamp"...it could also roll and cause problems.
		 */
		timestamp += lastdec + (TIMER_LOAD_VAL / (MV_BOARD_REFCLK / 1000)) - now;
	}
	lastdec = now;

	return timestamp;
}

/* waits specified delay value and resets timestamp */
void udelay_masked(unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

	if (usec >= 1000) {
		/* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;
		/* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ; /* find number of "ticks" to wait to achieve target */
		tmo /= 1000;          /* finish normalize. */
	} else {
		/* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
		tmo /= (1000 * 1000);
	}

	endtime = get_timer_masked() + tmo;

	do {
		ulong now = get_timer_masked();
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
ulong get_tbclk(void)
{
	ulong tbclk;

	tbclk = CONFIG_SYS_HZ;
	return tbclk;
}
