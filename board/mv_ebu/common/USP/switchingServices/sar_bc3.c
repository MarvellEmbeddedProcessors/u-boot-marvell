/*
* ***************************************************************************
* Copyright (C) 2016 Marvell International Ltd.
* ***************************************************************************
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 2 of the License, or any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ***************************************************************************
*/

#include "sar_sw_lib.h"

/* help descriptions */
static char *h_devid = "Device_ID\n",
	    *h_pll = "Core clock frequency [MHz]\n"
		     "\t0 = 450(internal)\n"
		     "\t1 = 487.5 (internal)\n"
		     "\t2 = 525 (default)\n"
		     "\t3 = 583.334 (internal)\n"
		     "\t4 = 600\n"
		     "\t5 = 625 (internal)\n"
		     "\t6 = 556.250 (internal)\n"
		     "\t7 = 250 (internal)\n"
		     "\t8 = 572.917\n"
		     "\t9 = 577.083 (internal)\n"
		     "\t10 = 585.417 (internal)\n"
		     "\t11 = 400 (internal)\n"
		     "\t12 = 535.416 (internal)\n"
		     "\t13 = 545.8335 (internal)\n"
		     "\t14 = 593.75 (internal)\n"
		     "\t15 = PLL bypass Reserved\n",
	    *h_pllmppm = "MPPM clock frequency [MHz]\n"
			 "\t0 = 650 (internal)\n"
			 "\t1 = 700 (internal)\n"
			 "\t2 = 750 (internal)\n"
			 "\t3 = 800 (internal)\n"
			 "\t4 = 850 (default)\n"
			 "\t5 = 875 (internal)\n"
			 "\t6 = 900 (internal)\n"
			 "\t7 = PLL bypass. Reserved\n",
	    *h_pllclk = "PTP clock frequency\n"
			"\t0 = PTP Clock = 500MHz (default)\n"
			"\t1 = PLL Bypass. Reserved.\n",
	    *h_port_cg_clk = "Port (CG MAC) clock frequency [MHz]\n"
			"\t0 = 800 (default)\n"
			"\t1 = 725 (internal)\n"
			"\t2 = 700 (internal)r\n"
			"\t3 = PLL bypass. Reserved.\n",
	    *h_boardid = "BoardID\n"
			 "\t0 - BC3 DB Board\n"
			 "\t1 - BC3 ETP Board\n";

/* PCA9560PW	is used for all SatRs configurations (0x4c, 0x4d, 0x4f, 0x4e)
 * PCA9555	is used for all Serdes configurations (0x20)
 */
struct satr_info bc3_satr_info[] = {
/*	name	twsi_addr  twsi_reg  field_of bit_mask moreThen256  default	help		pca9555*/
	{"devid",	0x4c,	0,	0,	0xf,	MV_FALSE,	0xf,	&h_devid,	MV_FALSE},
	{"corepll",	0x4d,	0,	0,	0xf,	MV_FALSE,	0x2,	&h_pll,		MV_FALSE},
	{"pll-mppm-cnf", 0x4e,	0,	0,	0x7,	MV_FALSE,	0x4,	&h_pllmppm,	MV_FALSE},
	{"ptp-pll-frq", 0x4e,	0,	3,	0x1,	MV_FALSE,	0x0,	&h_pllclk,	MV_FALSE},
	{"port-CG-frq",	0x4f,	0,	0,	0x3,	MV_FALSE,	0x0,	&h_port_cg_clk,	MV_FALSE},
	{"boardid",	0x53,	7,	0,	0x7,	MV_TRUE,	0x0,	&h_boardid,	MV_FALSE},
	/* the "LAST entry should be always last - it is used for SatR max options calculation */
	{"LAST",	0x0,	0,	0,	0x0,	MV_FALSE,	0x0,	NULL,		MV_FALSE},
};
