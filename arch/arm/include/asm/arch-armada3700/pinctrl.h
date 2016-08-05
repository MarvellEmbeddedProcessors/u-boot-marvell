/*
 * ***************************************************************************
 * Copyright (C) 2016 Marvell International Ltd.
 * ***************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of Marvell nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************
 */

#ifndef _ARMADA3700_PINCTRL_H_
#define _ARMADA3700_PINCTRL_H_

#define MAX_PINCTL_BANKS	2
#define MAX_PINS_PER_BANK	36
#define MAX_FUNC_PER_PIN	0x3

#ifdef CONFIG_A3700_Z_SUPPORT
#define PINCTL_BANK_CONF { \
	/* north bridge */ \
	{	/* bank name,		reg_base,	pin_cnt */ \
		"armada-3700-nb",	0,		36, \
		{ \
			/* mask		func_num	reg_values */ \
			{BIT10,		2,		{0, BIT10}		},	/* pin 0 */ \
			{BIT10,		2,		{0, BIT10}		},	/* pin 1 */ \
			{BIT9,		2,		{0, BIT9}		},	/* pin 2 */ \
			{BIT9,		2,		{0, BIT9}		},	/* pin 3 */ \
			{BIT16,		2,		{0, BIT16}		},	/* pin 4 */ \
			{0,		1,		{0}			},	/* pin 5 */ \
			{BIT8,		2,		{0, BIT8}		},	/* pin 6 */ \
			{BIT7,		2,		{0, BIT7}		},	/* pin 7 */ \
			{BIT1,		2,		{0, BIT1}		},	/* pin 8 */ \
			{BIT1 | BIT19,	3,		{0, BIT1, BIT1 | BIT19}	},	/* pin 9 */ \
			{BIT1 | BIT19,	3,		{0, BIT1, BIT1 | BIT19}	},	/* pin 10 */ \
			{BIT3 | BIT20,	3,		{0, BIT3, BIT3 | BIT20}	},	/* pin 11 */ \
			{BIT4 | BIT21,	3,		{0, BIT4, BIT4 | BIT21}	},	/* pin 12 */ \
			{BIT5 | BIT22,	3,		{0, BIT5, BIT5 | BIT22}	},	/* pin 13 */ \
			{BIT6 | BIT23,	3,		{0, BIT6, BIT6 | BIT23}	},	/* pin 14 */ \
			{BIT18,		2,		{0, BIT18}		},	/* pin 15 */ \
			{BIT18,		2,		{0, BIT18}		},	/* pin 16 */ \
			{BIT12,		2,		{0, BIT12}		},	/* pin 17 */ \
			{BIT13 | BIT19,	3,		{0, BIT13, BIT19}	},	/* pin 18 */ \
			{BIT14 | BIT19,	3,		{0, BIT14, BIT19}	},	/* pin 19 */ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 20 */ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 21 */ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 22 */ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 23 */ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 24 */ \
			{BIT17,		2,		{0, BIT17}		},	/* pin 25 */ \
			{BIT17,		2,		{0, BIT17}		},	/* pin 26 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 27 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 28 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 29 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 30 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 31 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 32 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 33 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 34 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 35 */ \
		}, \
	}, \
	/* South bridge */ \
	{	/* bank name,		reg_base,	pin_cnt */ \
		"armada-3700-sb",	0,		30, \
		{ \
			/*mask		config_num	configs*/ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 0 */ \
			{BIT1,		2,		{0, BIT1}		},	/* pin 1 */ \
			{0,		1,		{0}			},	/* pin 2 */ \
			{BIT4,		2,		{0, BIT4}		},	/* pin 3 */ \
			{BIT4,		2,		{0, BIT4}		},	/* pin 4 */ \
			{0,		1,		{0}			},	/* pin 5 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 6 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 7 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 8 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 9 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 10 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 11 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 12 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 13 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 14 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 15 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 16 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 17 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 18 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 19 */ \
			{BIT5,		2,		{0, BIT5}		},	/* pin 20 */ \
			{BIT5 | BIT6,	3,		{0, BIT5, BIT6}		},	/* pin 21 */ \
			{BIT5 | BIT7,	3,		{0, BIT5, BIT7}		},	/* pin 22 */ \
			{BIT3 | BIT8,	3,		{0, BIT3, BIT8}		},	/* pin 23 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 24 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 25 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 26 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 27 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 28 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 29 */ \
		}, \
	}, \
}
#else
/* Settings for A0 soc */
#define PINCTL_BANK_CONF { \
	/* north bridge */ \
	{	/* bank name,		reg_base,	pin_cnt */ \
		"armada-3700-nb",	0,		36, \
		{ \
			/* mask		func_num	reg_values */ \
			{BIT10,		2,		{0, BIT10}		},	/* pin 0 */ \
			{BIT10,		2,		{0, BIT10}		},	/* pin 1 */ \
			{BIT9,		2,		{0, BIT9}		},	/* pin 2 */ \
			{BIT9,		2,		{0, BIT9}		},	/* pin 3 */ \
			{BIT16,		2,		{0, BIT16}		},	/* pin 4 */ \
			{0,		1,		{0}			},	/* pin 5 */ \
			{BIT8,		2,		{0, BIT8}		},	/* pin 6 */ \
			{BIT7,		2,		{0, BIT7}		},	/* pin 7 */ \
			{BIT1,		2,		{0, BIT1}		},	/* pin 8 */ \
			{BIT1 | BIT19,	3,		{0, BIT1, BIT1 | BIT19}	},	/* pin 9 */ \
			{BIT1 | BIT19,	3,		{0, BIT1, BIT1 | BIT19}	},	/* pin 10 */ \
			{BIT3 | BIT20,	3,		{0, BIT3, BIT3 | BIT20}	},	/* pin 11 */ \
			{BIT4 | BIT21,	3,		{0, BIT4, BIT4 | BIT21}	},	/* pin 12 */ \
			{BIT5 | BIT22,	3,		{0, BIT5, BIT5 | BIT22}	},	/* pin 13 */ \
			{BIT6 | BIT23,	3,		{0, BIT6, BIT6 | BIT23}	},	/* pin 14 */ \
			{BIT18,		2,		{0, BIT18}		},	/* pin 15 */ \
			{BIT18,		2,		{0, BIT18}		},	/* pin 16 */ \
			{BIT12,		2,		{0, BIT12}		},	/* pin 17 */ \
			{BIT13 | BIT19,	3,		{0, BIT13, BIT19}	},	/* pin 18 */ \
			{BIT14 | BIT19,	3,		{0, BIT14, BIT19}	},	/* pin 19 */ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 20 */ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 21 */ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 22 */ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 23 */ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 24 */ \
			{BIT17,		2,		{0, BIT17}		},	/* pin 25 */ \
			{BIT17,		2,		{0, BIT17}		},	/* pin 26 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 27 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 28 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 29 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 30 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 31 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 32 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 33 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 34 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 35 */ \
		}, \
	}, \
	/* South bridge */ \
	{	/* bank name,		reg_base,	pin_cnt */ \
		"armada-3700-sb",	0,		30, \
		{ \
			/*mask		config_num	configs*/ \
			{BIT0,		2,		{0, BIT0}		},	/* pin 0 */ \
			{BIT1,		2,		{0, BIT1}		},	/* pin 1 */ \
			{0,		1,		{0}			},	/* pin 2 */ \
			{BIT5,		2,		{0, BIT5}		},	/* pin 3 */ \
			{BIT9,		2,		{0, BIT9}		},	/* pin 4 */ \
			{BIT10,		2,		{0, BIT10}		},	/* pin 5 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 6 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 7 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 8 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 9 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 10 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 11 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 12 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 13 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 14 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 15 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 16 */ \
			{BIT3,		2,		{0, BIT3}		},	/* pin 17 */ \
			{BIT4,		2,		{0, BIT4}		},	/* pin 18 */ \
			{BIT4,		2,		{0, BIT4}		},	/* pin 19 */ \
			{BIT11,		2,		{0, BIT11}		},	/* pin 20 */ \
			{BIT12 | BIT6,	3,		{0, BIT12, BIT6}	},	/* pin 21 */ \
			{BIT13 | BIT7,	3,		{0, BIT13, BIT7}	},	/* pin 22 */ \
			{BIT14 | BIT8,	3,		{0, BIT14, BIT8}	},	/* pin 23 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 24 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 25 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 26 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 27 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 28 */ \
			{BIT2,		2,		{0, BIT2}		},	/* pin 29 */ \
		}, \
	}, \
}
#endif

#endif /* _ARMADA3700_PINCTRL_H_ */
