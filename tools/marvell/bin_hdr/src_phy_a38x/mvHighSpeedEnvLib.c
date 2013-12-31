/*******************************************************************************
   Copyright (C) Marvell International Ltd. and its affiliates

   This software file (the "File") is owned and distributed by Marvell
   International Ltd. and/or its affiliates ("Marvell") under the following
   alternative licensing terms.  Once you have made an election to distribute the
   File under one of the following license alternatives, please (i) delete this
   introductory statement regarding license alternatives, (ii) delete the two
   license alternatives that you have not elected to use and (iii) preserve the
   Marvell copyright notice above.

********************************************************************************
   Marvell Commercial License Option

   If you received this File from Marvell and you have entered into a commercial
   license agreement (a "Commercial License") with Marvell, the File is licensed
   to you under the terms of the applicable Commercial License.

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
********************************************************************************
   Marvell BSD License Option

   If you received this File from Marvell, you may opt to use, redistribute and/or
   modify this File under the following licensing terms.
   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

*   Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

*   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

*   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "mvHighSpeedEnvSpec.h"
#include "mvBHboardEnvSpec.h"
#include "mvCtrlPex.h"
#include "mv_seq_exec.h"

#define SLOWDOWN  mvOsUDelay(50);
extern loadTopologyFuncPtr loadTopologyFuncArr[];
extern REF_CLOCK serdesTypeToRefClockMap[LAST_SERDES_TYPE];

#ifdef REGISTER_TRACE_DEBUG
static MV_U32 _MV_REG_READ(MV_U32 regAddr)
{
	putstring("\n >>>       MV_REG_READ.  regAddr=0x");
	putdata(INTER_REGS_BASE | (regAddr), 8);
	putstring(" regData=0x");
	MV_U32 regData = MV_MEMIO_LE32_READ((INTER_REGS_BASE | (regAddr)));
	putdata(regData, 8);

	return regData;
}

static MV_VOID _MV_REG_WRITE(MV_U32 regAddr, MV_U32 regData)
{
	putstring("\n >>>       MV_REG_WRITE. regAddr=0x");
	putdata(INTER_REGS_BASE | (regAddr), 8);
	putstring(" regData=0x");
	putdata(regData, 8);
	MV_MEMIO_LE32_WRITE((INTER_REGS_BASE | (regAddr)), (regData));
}
#undef MV_REG_WRITE
#undef MV_REG_READ

#define MV_REG_WRITE    _MV_REG_WRITE
#define MV_REG_READ     _MV_REG_READ
#endif /*REGISTER_TRACE_DEBUG*/

MV_U8 commonPhysSelectorsMap[LAST_SERDES_TYPE][MAX_SERDES_LANES] =
{
	/* 0      1       2       3       4       5 */
	{ 0x1, 0x1,    NA,	NA,	 NA,	  NA	  },    /* PEX0 */
	{ NA,  0x2,    0x1,	NA,	 0x1,	  NA	  },    /* PEX1 */
	{ NA,  NA,     0x2,	NA,	 NA,	  0x1	  },    /* PEX2 */
	{ NA,  NA,     NA,	0x1,	 NA,	  NA	  },    /* PEX3 */
	{ 0x2, 0x3,    NA,	NA,	 NA,	  NA	  },    /* SATA0 */
	{ NA,  NA,     0x3,	NA,	 0x2,	  NA	  },    /* SATA1 */
	{ NA,  NA,     NA,	NA,	 0x6,	  0x2	  },    /* SATA2 */
	{ NA,  NA,     NA,	0x3,	 NA,	  NA	  },    /* SATA3 */
	{ 0x3, 0x4,    NA,	NA,	 NA,	  NA	  },    /* SGMII0 */
	{ NA,  0x5,    0x4,	NA,	 0x3,	  NA	  },    /* SGMII1 */
	{ NA,  NA,     NA,	0x4,	 NA,	  0x3	  },    /* SGMII2 */
	{ NA,  0X7,    NA,	NA,	 NA,	  NA	  },    /* QSGMII */
	{ NA,  0x6,    NA,	NA,	 0x4,	  NA	  },    /* USB3_HOST0 */
	{ NA,  NA,     NA,	0x5,	 NA,	  0x4	  },    /* USB3_HOST1 */
	{ NA,  NA,     NA,	0x6,	 0x5,	  0x5	  },    /* USB3_DEVICE */
	{ 0x0, 0x0,    0x0,	0x0,	 0x0,	  0x0	  } /* DEFAULT_SERDES */
};

/* Serdes type to ref clock map */
REF_CLOCK serdesTypeToRefClockMap[LAST_SERDES_TYPE] =
{
	REF_CLOCK__100MHz,      /* PEX0 */
	REF_CLOCK__100MHz,      /* PEX1 */
	REF_CLOCK__100MHz,      /* PEX2 */
	REF_CLOCK__100MHz,      /* PEX3 */
	REF_CLOCK__25MHz,       /* SATA0 */
	REF_CLOCK__25MHz,       /* SATA1 */
	REF_CLOCK__25MHz,       /* SATA2 */
	REF_CLOCK__25MHz,       /* SATA3 */
	REF_CLOCK__25MHz,       /* SGMII0 */
	REF_CLOCK__25MHz,       /* SGMII1 */
	REF_CLOCK__25MHz,       /* SGMII2 */
	REF_CLOCK__25MHz,       /* QSGMII */
	REF_CLOCK__100MHz,      /* USB3_HOST0 */
	REF_CLOCK__100MHz,      /* USB3_HOST1 */
	REF_CLOCK__25MHz,       /* USB3_DEVICE */
	REF_CLOCK_UNSUPPORTED   /* DEFAULT_SERDES */
};

/******************************** Sequences DB ********************************/

/******************/
/* SATA and SGMII */
/******************/

MV_OP_PARAMS sataPowerUpParams[] =
{
	/* unitunitBaseReg                          unitOffset   mask        SATA data       SGMII data      waitTime    numOfLoops */
	{ SATA_CTRL_REG_INDIRECT, 0x38000,				       0xFFFFFFFF,	  { 0x0,      NO_DATA,	     }, 0,	0		 }, /* Power Down Sata addr*/
	{ SATA_CTRL_REG,	  0x38000,				       0xFFFFFFFF,	  { 0xC44040, NO_DATA,	     }, 0,	0		 } /* Power Down Sata */
};

/* SATA and SGMII - power up seq */
MV_OP_PARAMS sataAndSgmiiPowerUpParams[] =
{
	/* unitunitBaseReg                          unitOffset   mask        SATA data       SGMII data      waitTime    numOfLoops */
	{ COMMON_PHY_CONFIGURATION1_REG, 0x28,		   0x90006,		   { 0x80002,	0x80002	       }, 0,		   0			},      /* Power Up */
	{ COMMON_PHY_CONFIGURATION1_REG, 0x28,		   0x3FC00000,		   { 0x8800000, 0x19800000     }, 0,		   0			},      /* Baud Rate */
	{ COMMON_PHY_CONFIGURATION1_REG, 0x28,		   0x7800,		   { 0x6000,	0x6000	       }, 0,		   0			},      /* Unreset */
	{ POWER_AND_PLL_CTRL_REG,	 0x800,		   0x0E0,		   { 0x0,	0x80	       }, 0,		   0			} /* Phy Selector */
};

/* SATA and SGMII - speed config seq */
MV_OP_PARAMS sataAndSgmiiSpeedConfigParams[] =
{
	/* unitunitBaseReg                  unitOffset   mask     SATA data  SGMII data   waitTime    numOfLoops */
	{ INTERFACE_REG, 0x800,		       0xC00,	       { 0x800,	  NO_DATA }, 0,	    0		     }, /* Select Baud Rate for SATA only*/
	{ ISOLATE_REG,	 0x800,		       0xFF,	       { NO_DATA, 0x66	  }, 0,	    0		     }, /* Phy Gen RX and TX */
	{ LOOPBACK_REG,	 0x800,		       0xE,	       { 0x4,	  0x2	  }, 0,	    0		     } /* Bus Width */
};

/* SATA and SGMII - TX config seq */
MV_OP_PARAMS sataAndSgmiiTxConfigParams1[] =
{
	/* unitunitBaseReg                      unitOffset   mask        SATA data       SGMII data      waitTime    numOfLoops */
	{ MISC_REG,			 0x800,				 0x440,		     { 0x440,	0x400	      }, 0,	    0		   },
	{ GLUE_REG,			 0x800,				 0x1800,	     { NO_DATA, 0x800	      }, 0,	    0		   },
	{ RESET_DFE_REG,		 0x800,				 0x401,		     { 0x401,	0x401	      }, 0,	    0		   },   /* Sft Reset pulse */
	{ RESET_DFE_REG,		 0x800,				 0x401,		     { 0x0,	0x0	      }, 0,	    0		   },   /* Sft Reset pulse */
	{ COMMON_PHY_CONFIGURATION1_REG, 0x28,				 0xF0000,	     { 0x70000, 0x70000	      }, 0,	    0		   } /* Power up PLL, RX and TX */
};

MV_OP_PARAMS sataTxConfigParams[] =
{
	/* unitunitBaseReg                      unitOffset   mask        SATA data       SGMII data      waitTime    numOfLoops */
	{ SATA_CTRL_REG_INDIRECT, 0x38000,			       0xFFFFFFFF,     { 0x0,	   NO_DATA,	      }, 0,  0		    },  /* Power Down Sata addr*/
	{ SATA_CTRL_REG,	  0x38000,			       0xFFFFFFFF,     { 0xC40000, NO_DATA,	      }, 0,  0		    },  /* Power Down Sata */
	{ SATA_CTRL_REG_INDIRECT, 0x38000,			       0xFFFFFFFF,     { 0x4,	   NO_DATA,	      }, 0,  0		    },  /* Power Down Sata addr*/
	{ SATA_CTRL_REG,	  0x38000,			       0xFFFFFFFF,     { 0x0,	   NO_DATA,	      }, 0,  0		    } /* Power Down Sata */
};

MV_OP_PARAMS sataAndSgmiiTxConfigParams2[] =
{
	/* unitunitBaseReg                      unitOffset   mask        SATA data       SGMII data      waitTime    numOfLoops */
	{ COMMON_PHY_STATUS1_REG, 0x28,	       0xC,		  { 0xC, 0xC	       }, 10,	       1000	      },        /* Wait for PHY power up sequence to finish */
	{ COMMON_PHY_STATUS1_REG, 0x28,	       0x1,		  { 0x1, 0x1	       }, 1,	       1000	      },        /* Wait for PHY power up sequence to finish */
};

/****************/
/* PEX and USB3 */
/****************/

/* PEX and USB3 - power up seq */
MV_OP_PARAMS pexAndUsb3PowerUpParams[] =
{
	/* unitunitBaseReg                          unitOffset   mask        PEX data        USB3 data       waitTime    numOfLoops */
	{ COMMON_PHY_CONFIGURATION1_REG, 0x28,							  0x3FC7F806,			   { 0x4471804, 0x4479804 },	0,          0	 },
	{ COMMON_PHY_CONFIGURATION2_REG, 0x28,							  0x5C,				   { 0x58,	0x58 },		0,          0	 },
	{ COMMON_PHY_CONFIGURATION4_REG, 0x28,							  0x3,				   { 0x1,	0x1 },		0,          0	 },
	{ COMMON_PHY_CONFIGURATION1_REG, 0x28,							  0x7800,			   { 0x6000,	0xE000 },	0,          0	 },
	{ GLOBAL_CLK_CTRL,		 0x800,							  0xD,				   { 0x5,	0x1 },		0,          0	 },
	{ POWER_AND_PLL_CTRL_REG,	 0x800,							  0x0E0,			   { 0x60,	0xA0 },		0,          0	 } /* Phy Selector */

};

/* PEX and USB3 - speed config seq */
MV_OP_PARAMS pexAndUsb3SpeedConfigParams[] =
{
	/* unitunitBaseReg         unitOffset   mask            PEX data                    USB3 data                           waitTime    numOfLoops
	                                                                2.5         5               HOST            DEVICE                                      */
	{ INTERFACE_REG, 0x800,	   0xC00,	  { 0x400, 0x400,   0x400,   0x400,   0x400   }, 0,		  0		  }, /* Maximal PHY Generation Setting */
};

/* PEX and USB3 - TX config seq */
MV_OP_PARAMS pexAndUsb3TxConfigParams[] =
{
	/* unitunitBaseReg                      unitOffset   mask    PEX data  USB3 data   waitTime    numOfLoops */
	{ MISC_REG,	   0x800,					    0x4C0,	     { 0x80,  0xC0  }, 0,	   0			  },
	{ RESET_DFE_REG,   0x800,					    0x401,	     { 0x401, 0x401 }, 0,	   0			  },    /* Sft Reset pulse */
	{ RESET_DFE_REG,   0x800,					    0x401,	     { 0x0,   0x0   }, 0,	   0			  },    /* Sft Reset pulse */
	{ GLOBAL_CLK_CTRL, 0x800,					    0x1,	     { 0x0,   0x0   }, 0,	   0			  },
	{ 0x0,		   0x0,						    0x0,	     { 0x0,   0x0   }, 10,	   0			  } /* 10ms delay */
};

/* USB3 device donfig seq */
MV_OP_PARAMS usb3DeviceConfigParams[] =
{
	/* unitunitBaseReg  unitOffset   mask    data        waitTime   numOfLoops */
	{ 0xa0620,	    0x800,       0x200,	 { 0x200 },  0,	        0}
};


/*****************/
/*    USB2       */
/*****************/

MV_OP_PARAMS usb2PowerUpParams[] =
{
	/* unitunitBaseReg unitOffset   mask       USB2 data  waitTime  numOfLoops */
	{ 0x18440 ,        0x0 /*NA*/,	0xffffffff, {0x62},       0,        0}, /* Init phy 0 */
	{ 0x18444 ,        0x0 /*NA*/,	0xffffffff, {0x62},       0,        0}, /* Init phy 1 */
	{ 0x18448 ,        0x0 /*NA*/,	0xffffffff, {0x62},       0,        0}, /* Init phy 2 */
	{ 0xC0000 ,        0x0 /*NA*/,	0xffffffff, {0x40605205}, 0,        0}, /* Phy offset 0x0 - PLL_CONTROL0  */
	{ 0xC0004 ,        0x0 /*NA*/,	0x1,        {0x1},        0,        0}, /* Phy offset 0x1 - PLL_CONTROL1 */
	{ 0xC000C ,        0x0 /*NA*/,	0x1000000,  {0x1000000},  0,        0}, /* Phy0 register 3  - TX Channel control 0 */
	{ 0xC200C ,        0x0 /*NA*/,	0x1000000,  {0x1000000},  0,        0}, /* Phy0 register 3  - TX Channel control 0 */
	{ 0xC400C ,        0x0 /*NA*/,	0x1000000,  {0x1000000},  0,        0}, /* Phy0 register 3  - TX Channel control 0 */
	{ 0xC0008 ,        0x0 /*NA*/,	0x80800000, {0x80800000}, 1,     1000}, /* check PLLCAL_DONE is set and IMPCAL_DONE is set*/
	{ 0xC0018 ,        0x0 /*NA*/,	0x80000000, {0x80000000}, 1,     1000}, /* check REG_SQCAL_DONE  is set*/
	{ 0xC0000 ,        0x0 /*NA*/,	0x80000000, {0x80000000}, 1,     1000} /* check PLL_READY  is set*/
};


/* SERDES_POWER_DOWN */
MV_OP_PARAMS serdesPowerDownParams[] =
{
	{ COMMON_PHY_CONFIGURATION1_REG, 0x28, (0xF << 11), { (0x3 << 11) }, 0,	     0	},
	{ COMMON_PHY_CONFIGURATION1_REG, 0x28, (0x7 << 16), { 0		  }, 0,	     0	}
};

/************************* Local functions declarations ***********************/

MV_VOID serdesSeqInit(MV_VOID)
{
	DEBUG_INIT_FULL_S("\n### serdesSeqInit ###\n");

	/* SATA_ONLY_POWER_UP_SEQ sequence init */
	serdesSeqDb[SATA_ONLY_POWER_UP_SEQ].opParamsPtr = sataPowerUpParams;
	serdesSeqDb[SATA_ONLY_POWER_UP_SEQ].cfgSeqSize =  sizeof(sataPowerUpParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SATA_ONLY_POWER_UP_SEQ].dataArrIdx = SATA;

	/* SATA_POWER_UP_SEQ sequence init */
	serdesSeqDb[SATA_POWER_UP_SEQ].opParamsPtr = sataAndSgmiiPowerUpParams;
	serdesSeqDb[SATA_POWER_UP_SEQ].cfgSeqSize =  sizeof(sataAndSgmiiPowerUpParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SATA_POWER_UP_SEQ].dataArrIdx = SATA;

	/* SATA__1_5_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[SATA__1_5_SPEED_CONFIG_SEQ].opParamsPtr = sataAndSgmiiSpeedConfigParams;
	serdesSeqDb[SATA__1_5_SPEED_CONFIG_SEQ].cfgSeqSize =  sizeof(sataAndSgmiiSpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SATA__1_5_SPEED_CONFIG_SEQ].dataArrIdx = SATA;

	/* SATA__3_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[SATA__3_SPEED_CONFIG_SEQ].opParamsPtr = sataAndSgmiiSpeedConfigParams;
	serdesSeqDb[SATA__3_SPEED_CONFIG_SEQ].cfgSeqSize =  sizeof(sataAndSgmiiSpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SATA__3_SPEED_CONFIG_SEQ].dataArrIdx = SATA;

	/* SATA__6_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[SATA__6_SPEED_CONFIG_SEQ].opParamsPtr = sataAndSgmiiSpeedConfigParams;
	serdesSeqDb[SATA__6_SPEED_CONFIG_SEQ].cfgSeqSize =  sizeof(sataAndSgmiiSpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SATA__6_SPEED_CONFIG_SEQ].dataArrIdx = SATA;

	/* SATA_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[SATA_TX_CONFIG_SEQ1].opParamsPtr = sataAndSgmiiTxConfigParams1;
	serdesSeqDb[SATA_TX_CONFIG_SEQ1].cfgSeqSize =  sizeof(sataAndSgmiiTxConfigParams1) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SATA_TX_CONFIG_SEQ1].dataArrIdx = SATA;

	/* SATA_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[SATA_ONLY_TX_CONFIG_SEQ].opParamsPtr = sataTxConfigParams;
	serdesSeqDb[SATA_ONLY_TX_CONFIG_SEQ].cfgSeqSize =  sizeof(sataTxConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SATA_ONLY_TX_CONFIG_SEQ].dataArrIdx = SATA;

	/* SATA_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[SATA_TX_CONFIG_SEQ2].opParamsPtr = sataAndSgmiiTxConfigParams2;
	serdesSeqDb[SATA_TX_CONFIG_SEQ2].cfgSeqSize =  sizeof(sataAndSgmiiTxConfigParams2) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SATA_TX_CONFIG_SEQ2].dataArrIdx = SATA;

	/* SGMII_POWER_UP_SEQ sequence init */
	serdesSeqDb[SGMII_POWER_UP_SEQ].opParamsPtr = sataAndSgmiiPowerUpParams;
	serdesSeqDb[SGMII_POWER_UP_SEQ].cfgSeqSize =  sizeof(sataAndSgmiiPowerUpParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SGMII_POWER_UP_SEQ].dataArrIdx = SGMII;

	/* SGMII__1_25_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[SGMII__1_25_SPEED_CONFIG_SEQ].opParamsPtr = sataAndSgmiiSpeedConfigParams;
	serdesSeqDb[SGMII__1_25_SPEED_CONFIG_SEQ].cfgSeqSize =  sizeof(sataAndSgmiiSpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SGMII__1_25_SPEED_CONFIG_SEQ].dataArrIdx = SGMII;

	/* SGMII__3_125_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[SGMII__3_125_SPEED_CONFIG_SEQ].opParamsPtr = sataAndSgmiiSpeedConfigParams;
	serdesSeqDb[SGMII__3_125_SPEED_CONFIG_SEQ].cfgSeqSize =  sizeof(sataAndSgmiiSpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SGMII__3_125_SPEED_CONFIG_SEQ].dataArrIdx = SGMII;

	/* SGMII_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[SGMII_TX_CONFIG_SEQ1].opParamsPtr = sataAndSgmiiTxConfigParams1;
	serdesSeqDb[SGMII_TX_CONFIG_SEQ1].cfgSeqSize =  sizeof(sataAndSgmiiTxConfigParams1) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SGMII_TX_CONFIG_SEQ1].dataArrIdx = SGMII;

	/* SGMII_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[SGMII_TX_CONFIG_SEQ2].opParamsPtr = sataAndSgmiiTxConfigParams2;
	serdesSeqDb[SGMII_TX_CONFIG_SEQ2].cfgSeqSize =  sizeof(sataAndSgmiiTxConfigParams2) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SGMII_TX_CONFIG_SEQ2].dataArrIdx = SGMII;

	/* PEX_POWER_UP_SEQ sequence init */
	serdesSeqDb[PEX_POWER_UP_SEQ].opParamsPtr = pexAndUsb3PowerUpParams;
	serdesSeqDb[PEX_POWER_UP_SEQ].cfgSeqSize =  sizeof(pexAndUsb3PowerUpParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[PEX_POWER_UP_SEQ].dataArrIdx = PEX;

	/* PEX__2_5_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[PEX__2_5_SPEED_CONFIG_SEQ].opParamsPtr = pexAndUsb3SpeedConfigParams;
	serdesSeqDb[PEX__2_5_SPEED_CONFIG_SEQ].cfgSeqSize =  sizeof(pexAndUsb3SpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[PEX__2_5_SPEED_CONFIG_SEQ].dataArrIdx = PEX__2_5Gbps;

	/* PEX__5_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[PEX__5_SPEED_CONFIG_SEQ].opParamsPtr = pexAndUsb3SpeedConfigParams;
	serdesSeqDb[PEX__5_SPEED_CONFIG_SEQ].cfgSeqSize =  sizeof(pexAndUsb3SpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[PEX__5_SPEED_CONFIG_SEQ].dataArrIdx = PEX__5Gbps;

	/* PEX_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[PEX_TX_CONFIG_SEQ].opParamsPtr = pexAndUsb3TxConfigParams;
	serdesSeqDb[PEX_TX_CONFIG_SEQ].cfgSeqSize =  sizeof(pexAndUsb3TxConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[PEX_TX_CONFIG_SEQ].dataArrIdx = PEX;

	/* USB3_POWER_UP_SEQ sequence init */
	serdesSeqDb[USB3_POWER_UP_SEQ].opParamsPtr = pexAndUsb3PowerUpParams;
	serdesSeqDb[USB3_POWER_UP_SEQ].cfgSeqSize =  sizeof(pexAndUsb3PowerUpParams) / sizeof(MV_OP_PARAMS);

	serdesSeqDb[USB3_POWER_UP_SEQ].dataArrIdx = USB3;

	/* USB3__HOST_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[USB3__HOST_SPEED_CONFIG_SEQ].opParamsPtr = pexAndUsb3SpeedConfigParams;
	serdesSeqDb[USB3__HOST_SPEED_CONFIG_SEQ].cfgSeqSize =  sizeof(pexAndUsb3SpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[USB3__HOST_SPEED_CONFIG_SEQ].dataArrIdx = USB3__5Gbps_HOST;

	/* USB3__DEVICE_SPEED_CONFIG_SEQ sequence init */
	serdesSeqDb[USB3__DEVICE_SPEED_CONFIG_SEQ].opParamsPtr = pexAndUsb3SpeedConfigParams;
	serdesSeqDb[USB3__DEVICE_SPEED_CONFIG_SEQ].cfgSeqSize =  sizeof(pexAndUsb3SpeedConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[USB3__DEVICE_SPEED_CONFIG_SEQ].dataArrIdx = USB3__5Gbps_DEVICE;

	/* USB3_TX_CONFIG_SEQ sequence init */
	serdesSeqDb[USB3_TX_CONFIG_SEQ].opParamsPtr = pexAndUsb3TxConfigParams;
	serdesSeqDb[USB3_TX_CONFIG_SEQ].cfgSeqSize =  sizeof(pexAndUsb3TxConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[USB3_TX_CONFIG_SEQ].dataArrIdx = USB3;

	/* USB2_POWER_UP_SEQ sequence init */
	serdesSeqDb[USB2_POWER_UP_SEQ].opParamsPtr = usb2PowerUpParams;
	serdesSeqDb[USB2_POWER_UP_SEQ].cfgSeqSize =  sizeof(usb2PowerUpParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[USB2_POWER_UP_SEQ].dataArrIdx = 0;

	/* USB3_DEVICE_CONFIG_SEQ sequence init */
	serdesSeqDb[USB3_DEVICE_CONFIG_SEQ].opParamsPtr = usb3DeviceConfigParams;
	serdesSeqDb[USB3_DEVICE_CONFIG_SEQ].cfgSeqSize =  sizeof(usb3DeviceConfigParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[USB3_DEVICE_CONFIG_SEQ].dataArrIdx = 0; /* Not relevant */

	/* SERDES_POWER_DOWN_SEQ sequence init */
	serdesSeqDb[SERDES_POWER_DOWN_SEQ].opParamsPtr = serdesPowerDownParams;
	serdesSeqDb[SERDES_POWER_DOWN_SEQ].cfgSeqSize =  sizeof(serdesPowerDownParams) / sizeof(MV_OP_PARAMS);
	serdesSeqDb[SERDES_POWER_DOWN_SEQ].dataArrIdx = FIRST_CELL;
}

/***************************************************************************/
SERDES_SEQ serdesTypeAndSpeedToSpeedSeq
(
	SERDES_TYPE serdesType,
	SERDES_SPEED baudRate
)
{
	SERDES_SEQ seqId = SERDES_LAST_SEQ;

	DEBUG_INIT_FULL_S("\n### serdesTypeAndSpeedToSpeedSeq ###\n");
	switch (serdesType) {
	case PEX0:
	case PEX1:
	case PEX2:
	case PEX3:
		if (baudRate == __2_5Gbps)
			seqId = PEX__2_5_SPEED_CONFIG_SEQ;
		else if (baudRate == __5Gbps)
			seqId = PEX__5_SPEED_CONFIG_SEQ;
		break;
	case USB3_HOST0:
	case USB3_HOST1:
		if (baudRate == __5Gbps)
			seqId = USB3__HOST_SPEED_CONFIG_SEQ;
		break;
	case USB3_DEVICE:
		if (baudRate == __5Gbps)
			seqId = USB3__DEVICE_SPEED_CONFIG_SEQ;
		break;
	case SATA0:
	case SATA1:
	case SATA2:
	case SATA3:
		if (baudRate == __1_5Gbps)
			seqId = SATA__1_5_SPEED_CONFIG_SEQ;
		else if (baudRate == __3Gbps)
			seqId = SATA__3_SPEED_CONFIG_SEQ;
		else if (baudRate == __6Gbps)
			seqId = SATA__6_SPEED_CONFIG_SEQ;
		break;
	case SGMII0:
	case SGMII1:
	case SGMII2:
		if (baudRate == __1_25Gbps)
			seqId = SGMII__1_25_SPEED_CONFIG_SEQ;
		else if (baudRate == __3_125Gbps)
			seqId = SGMII__3_125_SPEED_CONFIG_SEQ;
		break;
	default:
		return SERDES_LAST_SEQ;
	}

	return seqId;
}

/***************************************************************************/
MV_STATUS mvHwsTwsiInitWrapper(MV_VOID)
{
	MV_TWSI_ADDR slave;
	MV_U32 tClock;

	DEBUG_INIT_FULL_S("\n### mvHwsTwsiInitWrapper ###\n");
	slave.type = ADDR7_BIT;
	slave.address = 0;
	tClock = mvBoardTclkGet();
	if (tClock == MV_BOARD_TCLK_ERROR) {
		DEBUG_INIT_FULL_S("mvHwsTwsiInitWrapper: TClk read from the board is not supported\n");
		return MV_NOT_SUPPORTED;
	}

	mvTwsiInit(0, TWSI_SPEED, tClock, &slave, 0);
	return MV_OK;
}


/***************************************************************************/
#ifdef MV_DEBUG_INIT
static const char *serdesTypeToString[] = {
	"PEX0",
	"PEX1",
	"PEX2",
	"PEX3",
	"SATA0",
	"SATA1",
	"SATA2",
	"SATA3",
	"SGMII0",
	"SGMII1",
	"SGMII2",
	"QSGMII",
	"USB3_HOST0",
	"USB3_HOST1",
	"USB3_DEVICE",
	"DEFAULT_SERDES",
	"LAST_SERDES_TYPE"
};

MV_VOID printTopologyDetails(SERDES_MAP  *serdesMapArray)
{
	MV_U32 laneNum;

	DEBUG_INIT_S("board topology details:\n");

	for (laneNum = 0; laneNum < MAX_SERDES_LANES; laneNum++) {
		if (serdesMapArray[laneNum].serdesType == DEFAULT_SERDES)
		{
			continue;
		}
		DEBUG_INIT_C("serdes num: ", laneNum, 1);
		DEBUG_INIT_C("serdes speed: ", serdesMapArray[laneNum].serdesSpeed, 2);
		DEBUG_INIT_S("serdes type: ");
		DEBUG_INIT_S((char *)serdesTypeToString[serdesMapArray[laneNum].serdesType]);
		DEBUG_INIT_S("\n");
	}
}
#endif

/***************************************************************************/
MV_STATUS mvHwsCtrlHighSpeedSerdesPhyConfig(MV_VOID)
{
	DEBUG_INIT_FULL_S("\n### mvCtrlHighSpeedSerdesPhyConfig ###\n");

	/* Init serdes sequences DB */
	serdesSeqInit();

	/* TWSI init */
	DEBUG_INIT_FULL_S("mvHwsTwsiInitWrapper: Init TWSI interface.\n");
	CHECK_STATUS(mvHwsTwsiInitWrapper());

	/* Board topology load */
	DEBUG_INIT_FULL_S("mvCtrlHighSpeedSerdesPhyConfig: Loading board topology..\n");
	CHECK_STATUS(mvHwsBoardTopologyLoad(serdesConfigurationMap));

#ifdef MV_DEBUG_INIT
	/* print topology */
	printTopologyDetails(serdesConfigurationMap);
#endif

	/* Power-Up sequence */
	DEBUG_INIT_FULL_S("mvCtrlHighSpeedSerdesPhyConfig: Starting serdes power up sequence\n");

	CHECK_STATUS(powerUpSerdesLanes(serdesConfigurationMap));

	DEBUG_INIT_FULL_S("\n### mvCtrlHighSpeedSerdesPhyConfig ended successfully ###\n");
	return MV_OK;
}

/***************************************************************************/
MV_STATUS powerUpSerdesLanes(SERDES_MAP  *serdesConfigMap)
{
	MV_U32 serdesLaneNum;
	REF_CLOCK refClock;
	SERDES_TYPE serdesType;
	SERDES_SPEED serdesSpeed;

	DEBUG_INIT_FULL_S("\n### powerUpSerdesLanes ###\n");

	/* COMMON PHYS SELECTORS register configuration */
	DEBUG_INIT_FULL_S("powerUpSerdesLanes: Updating COMMON PHYS SELECTORS reg\n");
	CHECK_STATUS(mvHwsUpdateSerdesPhySelectors(serdesConfigurationMap));

	/* per Serdes Power Up */
	for (serdesLaneNum = 0; serdesLaneNum < MAX_SERDES_LANES; serdesLaneNum++) {
		DEBUG_INIT_FULL_S("calling serdesPowerUpCtrl: serdes lane number ");
		DEBUG_INIT_FULL_D_10(serdesLaneNum, 1);
		DEBUG_INIT_FULL_S("\n");

		serdesType = serdesConfigMap[serdesLaneNum].serdesType;
		serdesSpeed = serdesConfigMap[serdesLaneNum].serdesSpeed;

		/* serdes lane is not in use */
		if (serdesType == DEFAULT_SERDES)
			continue;

		refClock = serdesTypeToRefClockMap[serdesType];
		if (refClock == REF_CLOCK_UNSUPPORTED) {
			DEBUG_INIT_S("powerUpSerdesLanes: unsupported ref clock\n");
			return MV_NOT_SUPPORTED;
		}
		CHECK_STATUS(mvSerdesPowerUpCtrl(serdesLaneNum,
						 MV_TRUE,
						 serdesType,
						 serdesSpeed,
						 refClock));
	}

	/* PEX configuration*/
	CHECK_STATUS(mvHwsPexConfig(serdesConfigMap));

	/* USB2 configuration */
	DEBUG_INIT_FULL_S("powerUpSerdesLanes: init USB2 Phys\n");
	CHECK_STATUS(mvSeqExec(0 /* not relevant */, USB2_POWER_UP_SEQ));

	DEBUG_INIT_FULL_S("### powerUpSerdesLanes ended successfully ###\n");
	return MV_OK;
}

/***************************************************************************/

MV_STATUS mvCtrlHighSpeedSerdesPhyConfig(MV_VOID)
{
	return mvHwsCtrlHighSpeedSerdesPhyConfig(); // stub
}

#define LINK_WAIT_CNTR  100
#define LINK_WAIT_SLEEP 100
/***************************************************************************/

MV_STATUS mvSerdesPowerUpCtrl
(
	MV_U32 serdesNum,
	MV_BOOL serdesPowerUp,
	SERDES_TYPE serdesType,
	SERDES_SPEED baudRate,
	REF_CLOCK refClock
)
{
#ifdef DB_LINK_CHECK
	int i;
#endif
	int sataIdx, pexIdx;
	SERDES_SEQ speedSeqId;
	MV_U32 regData;

	DEBUG_INIT_FULL_S("\n### mvSerdesPowerUpCtrl ###\n");

	if (serdesPowerUp == MV_TRUE) { /* Serdes power up */
		DEBUG_INIT_FULL_S("mvSerdesPowerUpCtrl: executing power up.. ");
		DEBUG_INIT_FULL_C("serdes num = ", serdesNum, 2);
		DEBUG_INIT_FULL_C("serdes type = ", serdesType, 2);

		DEBUG_INIT_FULL_S("Going access 1");

		/* Getting the Speed Select sequence id */
		speedSeqId = serdesTypeAndSpeedToSpeedSeq(serdesType, baudRate);
		if (speedSeqId == SERDES_LAST_SEQ) {
			DEBUG_INIT_S("mvSerdesPowerUpCtrl: serdes type and speed aren't supported together\n");
			return MV_BAD_PARAM;
		}

		/* Executing power up, ref clock set, speed config and TX config */
		switch (serdesType) {
		case PEX0:
		case PEX1:
		case PEX2:
		case PEX3:
			regData = MV_REG_READ(SOC_CONTROL_REG1);
			regData |= 0x4000;
			MV_REG_WRITE(SOC_CONTROL_REG1, regData);

			pexIdx = serdesType - PEX0;

			regData = MV_REG_READ(((MV_PEX_IF_REGS_BASE(pexIdx)) + 0x6c));
			regData &= ~0x3F0;
			regData |= 0x10;
			MV_REG_WRITE(((MV_PEX_IF_REGS_BASE(pexIdx)) + 0x6c), regData);

			regData = MV_REG_READ(((MV_PEX_IF_REGS_BASE(pexIdx)) + 0x6c));
			regData &= ~0xF;
			regData |= 0x2;
			MV_REG_WRITE(((MV_PEX_IF_REGS_BASE(pexIdx)) + 0x6c), regData);

			regData = MV_REG_READ(((MV_PEX_IF_REGS_BASE(pexIdx)) + 0x70));
			regData &= ~0x40;
			regData |= 0x40;
			MV_REG_WRITE(((MV_PEX_IF_REGS_BASE(pexIdx)) + 0x70), regData);

			CHECK_STATUS(mvSeqExec(serdesNum, PEX_POWER_UP_SEQ));
			CHECK_STATUS(mvHwsRefClockSet(serdesNum, serdesType, refClock));
			CHECK_STATUS(mvSeqExec(serdesNum, speedSeqId));
			CHECK_STATUS(mvSeqExec(serdesNum, PEX_TX_CONFIG_SEQ));

			mvOsUDelay(20);

#ifdef DB_LINK_CHECK
			if (serdesNum == 1) {
				MV_REG_WRITE(SOC_CONTROL_REG1, 0x0707C0F1);
				for (i = 0; i < LINK_WAIT_CNTR; i++) {
					if ((MV_REG_READ(0x81a64) & 0xFF) == 0x7E) {
						DEBUG_INIT_S("PEX0 LINK UP ;-)\n");
						break;
					}
					mvOsUDelay(LINK_WAIT_SLEEP);
				}

				if (i == LINK_WAIT_CNTR)
					DEBUG_INIT_S("PEX0 NO LINK ;-|\n");
			}else{
				MV_REG_WRITE(SOC_CONTROL_REG1, 0x0707C0F3);
				for (i = 0; i < LINK_WAIT_CNTR; i++) {
					if ((MV_REG_READ(0x41a64) & 0xFF) == 0x7E) {
						DEBUG_INIT_S("PEX1 LINK UP ;-)\n");
						break;
					}
					mvOsUDelay(LINK_WAIT_SLEEP);
				}

				if (i == LINK_WAIT_CNTR)
					DEBUG_INIT_S("PEX1 NO LINK ;-|\n");
			}
#endif                  /* DB_LINK_CHECK */

			break;
		case USB3_HOST0:
		case USB3_HOST1:
			CHECK_STATUS(mvSeqExec(serdesNum, USB3_POWER_UP_SEQ));
			CHECK_STATUS(mvHwsRefClockSet(serdesNum, serdesType, refClock));
			CHECK_STATUS(mvSeqExec(serdesNum, speedSeqId));
			CHECK_STATUS(mvSeqExec(serdesNum, USB3_TX_CONFIG_SEQ));

			mvOsUDelay(10000);

#ifdef DB_LINK_CHECK
			if ( serdesNum == 4) {
				if ( 0 != (MV_REG_READ(0xA2004) & 0x100))
					DEBUG_INIT_S("USB PLL0 UNLOCK ;-)\n");
				else
					DEBUG_INIT_S("USB PLL0 LOCKED ;-|\n");

				if ( 0xD == (MV_REG_READ(0x183B8) & 0xD))
					DEBUG_INIT_S("USB PLL0 READY ;-)\n");
				else
					DEBUG_INIT_S("USB PLL0 NOT READY ;-|\n");
			}else{
				if ( 0 != (MV_REG_READ(0xA2804) & 0x100))
					DEBUG_INIT_S("USB PLL1 UNLOCK ;-)\n");
				else
					DEBUG_INIT_S("USB PLL1 LOCKED ;-|\n");

				if ( 0xD == (MV_REG_READ(0x183E0) & 0xD))
					DEBUG_INIT_S("USB PLL1 READY ;-)\n");
				else
					DEBUG_INIT_S("USB PLL1 NOT READY ;-|\n");

			}
#endif                  /* DB_LINK_CHECK */

			break;
		case USB3_DEVICE:
			CHECK_STATUS(mvSeqExec(serdesNum, USB3_POWER_UP_SEQ));
			CHECK_STATUS(mvHwsRefClockSet(serdesNum, serdesType, refClock));
			CHECK_STATUS(mvSeqExec(serdesNum, speedSeqId));
			CHECK_STATUS(mvSeqExec(serdesNum, USB3_DEVICE_CONFIG_SEQ));
			CHECK_STATUS(mvSeqExec(serdesNum, USB3_TX_CONFIG_SEQ));
			break;

		case SATA0:
		case SATA1:
		case SATA2:
		case SATA3:

			sataIdx =  ((serdesType == SATA0) || (serdesType == SATA1)) ? 0 : 1;
			CHECK_STATUS(mvSeqExec(sataIdx, SATA_ONLY_POWER_UP_SEQ));
			CHECK_STATUS(mvSeqExec(serdesNum, SATA_POWER_UP_SEQ));
			CHECK_STATUS(mvHwsRefClockSet(serdesNum, serdesType, refClock));
			CHECK_STATUS(mvSeqExec(serdesNum, speedSeqId));

			CHECK_STATUS(mvSeqExec(serdesNum, SATA_TX_CONFIG_SEQ1));
			CHECK_STATUS(mvSeqExec(sataIdx, SATA_ONLY_TX_CONFIG_SEQ));
			CHECK_STATUS(mvSeqExec(serdesNum, SATA_TX_CONFIG_SEQ2));

			mvOsUDelay(10000);
#ifdef DB_LINK_CHECK
			if ( serdesNum == 0) {
				for (i = 0; i < LINK_WAIT_CNTR; i++) {
					if ((MV_REG_READ(0x18318) & 0xD) == 0xD) {
						DEBUG_INIT_S("SATA0 PLL READY ;-)\n");
						break;
					}
					mvOsUDelay(LINK_WAIT_SLEEP);
				}

				if (i == LINK_WAIT_CNTR)
					DEBUG_INIT_S("SATA0 PLL NOT READY\n");
			}else{
				for (i = 0; i < LINK_WAIT_CNTR; i++) {
					if ((MV_REG_READ(0x18390) & 0xD) == 0xD) {
						DEBUG_INIT_S("SATA1 PLL READY ;-)\n");
						break;
					}
					mvOsUDelay(LINK_WAIT_SLEEP);
				}

				if (i == LINK_WAIT_CNTR)
					DEBUG_INIT_S("SATA1 PLL NOT READY\n");
			}
#endif                  /* DB_LINK_CHECK */
			break;
		case SGMII0:
		case SGMII1:
		case SGMII2:
			CHECK_STATUS(mvSeqExec(serdesNum, SGMII_POWER_UP_SEQ));
			CHECK_STATUS(mvHwsRefClockSet(serdesNum, serdesType, refClock));
			CHECK_STATUS(mvSeqExec(serdesNum, speedSeqId));
			CHECK_STATUS(mvSeqExec(serdesNum, SGMII_TX_CONFIG_SEQ1));
			CHECK_STATUS(mvSeqExec(serdesNum, SGMII_TX_CONFIG_SEQ2));

			/* GBE configuration */
			regData = MV_REG_READ(GBE_CONFIGURATION_REG);
			regData |= 0x1 << (serdesType - SGMII0); /* write the SGMII index */
			MV_REG_WRITE(GBE_CONFIGURATION_REG, regData);

			break;
		default:
			DEBUG_INIT_S("mvSerdesPowerUpCtrl: bad serdesType parameter\n");
			return MV_BAD_PARAM;
		}
	}else { /* Serdes power down */
		DEBUG_INIT_FULL_S("mvSerdesPowerUp: executing power down.. ");
		DEBUG_INIT_FULL_C("serdes num = ", serdesNum, 1);

		CHECK_STATUS(mvSeqExec(serdesNum, SERDES_POWER_DOWN_SEQ));
	}

	DEBUG_INIT_FULL_C("mvSerdesPowerUpCtrl ended successfully for serdes ", serdesNum, 2);

	return MV_OK;
}

/***************************************************************************/
MV_STATUS mvHwsUpdateSerdesPhySelectors(SERDES_MAP* serdesConfigMap)
{
	MV_U32 laneData, serdesIdx, regData = 0;
	SERDES_TYPE serdesType;
	SERDES_MODE serdesMode;
	MV_BOOL isPEXx4 = MV_FALSE;

	DEBUG_INIT_FULL_S("\n### mvHwsUpdateSerdesPhySelectors ###\n");
	DEBUG_INIT_FULL_S("Updating the COMMON PHYS SELECTORS register with the serdes types\n");

	/* Updating bits 0-17 in the COMMON PHYS SELECTORS register according to the serdes types */
	for (serdesIdx = 0; serdesIdx < MAX_SERDES_LANES; serdesIdx++) {
		serdesType = serdesConfigMap[serdesIdx].serdesType;
		serdesMode = serdesConfigMap[serdesIdx].serdesMode;
		laneData = commonPhysSelectorsMap[serdesType][serdesIdx];

		if (laneData == NA) {
			DEBUG_INIT_S("mvUpdateSerdesSelectPhyModeSeq: serdes number and type are not supported together\n");
			return MV_BAD_PARAM;
		}

		/* Updating the data that will be written to COMMON_PHYS_SELECTORS_REG */
		regData |= (laneData << (3 * serdesIdx));

		/* Checking if the board topology configuration includes PEXx4 - for the next step */
		if ((serdesMode == PEX_END_POINT_x4) || (serdesMode == PEX_ROOT_COMPLEX_x4))
			isPEXx4 = MV_TRUE;
	}

	/* Updating the 18th bit in the COMMON PHYS SELECTORS register in case there is PEXx4 */
	regData |= (isPEXx4 == MV_TRUE) ? (0x1 << 18) : 0;

	/* Updating the COMMON PHYS SELECTORS register */
	MV_REG_WRITE(COMMON_PHYS_SELECTORS_REG, regData);

	return MV_OK;
}

/***************************************************************************/
MV_STATUS mvHwsRefClockSet
(
	MV_U32 serdesNum,
	SERDES_TYPE serdesType,
	REF_CLOCK refClock
)
{
	MV_U32 data, regData;

	DEBUG_INIT_FULL_S("\n### mvHwsRefClockSet ###\n");

	if (serdesNum >= MAX_SERDES_LANES) {
		DEBUG_INIT_S("mvHwsRefClockSet: bad serdes number\n");
		return MV_BAD_PARAM;
	}

	switch (serdesType) {
	case PEX0:
	case PEX1:
	case PEX2:
	case PEX3:
	case USB3_HOST0:
	case USB3_HOST1:
	case USB3_DEVICE:
		if (refClock == REF_CLOCK__100MHz)
			data = 0x0;
		else if (refClock == REF_CLOCK__25MHz)
			data = 0x2;
		else{
			DEBUG_INIT_S("mvHwsRefClockSet: bad ref clock\n");
			return MV_BAD_PARAM;
		}
		break;
	case SATA0:
	case SATA1:
	case SATA2:
	case SATA3:
		if (refClock == REF_CLOCK__100MHz)
			data = 0x7;
		else if (refClock == REF_CLOCK__25MHz)
			data = 0x1;
		else{
			DEBUG_INIT_S("mvHwsRefClockSet: bad ref clock\n");
			return MV_BAD_PARAM;
		}
		break;
	case SGMII0:
	case SGMII1:
	case SGMII2:
		if (refClock == REF_CLOCK__100MHz)
			data = 0x8;
		else if (refClock == REF_CLOCK__25MHz)
			data = 0x1;
		else{
			DEBUG_INIT_S("mvHwsRefClockSet: bad ref clock\n");
			return MV_BAD_PARAM;
		}
		break;
	default:
		DEBUG_INIT_S("mvHwsRefClockSet: bad serdes type\n");
		return MV_BAD_PARAM;
	}

	/* Write the refClock to relevant SELECT_REF_CLOCK_REG bits and offset */
	regData = MV_REG_READ(POWER_AND_PLL_CTRL_REG + (0x800 * serdesNum));
	regData &= ~(0x1F);
	regData |= data;
	MV_REG_WRITE(POWER_AND_PLL_CTRL_REG + (0x800 * serdesNum), regData);

	return MV_OK;
}

/***************************************************************************/
MV_STATUS mvHwsBoardTopologyLoad(SERDES_MAP  *serdesMapArray)
{
	MV_U8 boardId;

	//DEBUG_INIT_FULL_S("\n### mvHwsBoardTopologyLoad ###\n");
	DEBUG_INIT_FULL_S("\n### mvHwsBoardTopologyLoad ###\n");

	boardId = mvHwsBoardIdGet();

	if (boardId >= MV_MAX_BOARD_ID) {
		DEBUG_INIT_S("mvHwsBoardTopologyLoad: board id 0x");
		DEBUG_INIT_FULL_D(boardId, 8);
		DEBUG_INIT_S("out of range. Use default board ID - DB\n");
		boardId = DB_A38X_BP_ID;
	}
	/* getting board topology according to the board id */
	DEBUG_INIT_FULL_S("mvHwsBoardTopologyLoad: getting board topology according to the board id\n");
	CHECK_STATUS(loadTopologyFuncArr[boardId](serdesMapArray));

	return MV_OK;
}
