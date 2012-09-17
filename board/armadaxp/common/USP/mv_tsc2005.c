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

#include "mvCommon.h"
#include "mvOs.h"
#include "mvSysSpiApi.h"
#include "spi/mvSpi.h"
#include "spi/mvSpiCmnd.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "gpp/mvGppRegs.h"

#define TSC2005_CMD	(0x80)
#define TSC2005_REG	(0x00)

#define TSC2005_CMD_STOP	(1)
#define TSC2005_CMD_10BIT	(0 << 2)
#define TSC2005_CMD_12BIT	(1 << 2)

#define TSC2005_CMD_SCAN_XYZZ	(0 << 3)
#define TSC2005_CMD_SCAN_XY	(1 << 3)
#define TSC2005_CMD_SCAN_X	(2 << 3)
#define TSC2005_CMD_SCAN_Y	(3 << 3)
#define TSC2005_CMD_SCAN_ZZ	(4 << 3)
#define TSC2005_CMD_AUX_SINGLE	(5 << 3)
#define TSC2005_CMD_TEMP1	(6 << 3)
#define TSC2005_CMD_TEMP2	(7 << 3)
#define TSC2005_CMD_AUX_CONT	(8 << 3)
#define TSC2005_CMD_TEST_X_CONN	(9 << 3)
#define TSC2005_CMD_TEST_Y_CONN	(10 << 3)
/* command 11 reserved */
#define TSC2005_CMD_TEST_SHORT	(12 << 3)
#define TSC2005_CMD_DRIVE_XX	(13 << 3)
#define TSC2005_CMD_DRIVE_YY	(14 << 3)
#define TSC2005_CMD_DRIVE_YX	(15 << 3)

#define TSC2005_REG_X		(0 << 3)
#define TSC2005_REG_Y		(1 << 3)
#define TSC2005_REG_Z1		(2 << 3)
#define TSC2005_REG_Z2		(3 << 3)
#define TSC2005_REG_AUX		(4 << 3)
#define TSC2005_REG_TEMP1	(5 << 3)
#define TSC2005_REG_TEMP2	(6 << 3)
#define TSC2005_REG_STATUS	(7 << 3)
#define TSC2005_REG_AUX_HIGH	(8 << 3)
#define TSC2005_REG_AUX_LOW	(9 << 3)
#define TSC2005_REG_TEMP_HIGH	(10 << 3)
#define TSC2005_REG_TEMP_LOW	(11 << 3)
#define TSC2005_REG_CFR0	(12 << 3)
#define TSC2005_REG_CFR1	(13 << 3)
#define TSC2005_REG_CFR2	(14 << 3)
#define TSC2005_REG_FUNCTION	(15 << 3)

#define TSC2005_REG_PND0	(1 << 1)
#define TSC2005_REG_READ	(0x01)
#define TSC2005_REG_WRITE	(0x00)


#define TSC2005_CFR0_LONGSAMPLING	(1)
#define TSC2005_CFR0_DETECTINWAIT	(1 << 1)
#define TSC2005_CFR0_SENSETIME_32US	(0)
#define TSC2005_CFR0_SENSETIME_96US	(1 << 2)
#define TSC2005_CFR0_SENSETIME_544US	(1 << 3)
#define TSC2005_CFR0_SENSETIME_2080US	(1 << 4)
#define TSC2005_CFR0_SENSETIME_2656US	(0x001C)
#define TSC2005_CFR0_PRECHARGE_20US	(0x0000)
#define TSC2005_CFR0_PRECHARGE_84US	(0x0020)
#define TSC2005_CFR0_PRECHARGE_276US	(0x0040)
#define TSC2005_CFR0_PRECHARGE_1044US	(0x0080)
#define TSC2005_CFR0_PRECHARGE_1364US	(0x00E0)
#define TSC2005_CFR0_STABTIME_0US	(0x0000)
#define TSC2005_CFR0_STABTIME_100US	(0x0100)
#define TSC2005_CFR0_STABTIME_500US	(0x0200)
#define TSC2005_CFR0_STABTIME_1MS	(0x0300)
#define TSC2005_CFR0_STABTIME_5MS	(0x0400)
#define TSC2005_CFR0_STABTIME_100MS	(0x0700)
#define TSC2005_CFR0_CLOCK_4MHZ		(0x0000)
#define TSC2005_CFR0_CLOCK_2MHZ		(0x0800)
#define TSC2005_CFR0_CLOCK_1MHZ		(0x1000)
#define TSC2005_CFR0_RESOLUTION12	(0x2000)
#define TSC2005_CFR0_STATUS		(0x4000)
#define TSC2005_CFR0_PENMODE_INITIATE_BY_TSC2005	(0x8000)
#define TSC2005_CFR0_PENMODE_INITIATE_BY_HOST		(0x0000)

#define TSC2005_CFR0_INITVALUE	(TSC2005_CFR0_STABTIME_1MS  |	\
				 TSC2005_CFR0_CLOCK_1MHZ    |	\
				 TSC2005_CFR0_RESOLUTION12  |	\
				 TSC2005_CFR0_PRECHARGE_276US | \
				 TSC2005_CFR0_PENMODE_INITIATE_BY_TSC2005)

#define TSC2005_CFR1_BATCHDELAY_0MS	(0x0000)
#define TSC2005_CFR1_BATCHDELAY_1MS	(0x0001)
#define TSC2005_CFR1_BATCHDELAY_2MS	(0x0002)
#define TSC2005_CFR1_BATCHDELAY_4MS	(0x0003)
#define TSC2005_CFR1_BATCHDELAY_10MS	(0x0004)
#define TSC2005_CFR1_BATCHDELAY_20MS	(0x0005)
#define TSC2005_CFR1_BATCHDELAY_40MS	(0x0006)
#define TSC2005_CFR1_BATCHDELAY_100MS	(0x0007)

#define TSC2005_CFR1_INITVALUE	(TSC2005_CFR1_BATCHDELAY_2MS)

#define TSC2005_CFR2_MAVE_TEMP	(0x0001)
#define TSC2005_CFR2_MAVE_AUX	(0x0002)
#define TSC2005_CFR2_MAVE_Z	(0x0004)
#define TSC2005_CFR2_MAVE_Y	(0x0008)
#define TSC2005_CFR2_MAVE_X	(0x0010)
#define TSC2005_CFR2_AVG_1	(0x0000)
#define TSC2005_CFR2_AVG_3	(0x0400)
#define TSC2005_CFR2_AVG_7	(0x0800)
#define TSC2005_CFR2_MEDIUM_1	(0x0000)
#define TSC2005_CFR2_MEDIUM_3	(0x1000)
#define TSC2005_CFR2_MEDIUM_7	(0x2000)
#define TSC2005_CFR2_MEDIUM_15	(0x3000)

#define TSC2005_CFR2_IRQ_DAV	(0x4000)
#define TSC2005_CFR2_IRQ_PEN	(0x8000)
#define TSC2005_CFR2_IRQ_PENDAV	(0x0000)

#define TSC2005_CFR2_INITVALUE	(TSC2005_CFR2_IRQ_DAV   |	\
				 TSC2005_CFR2_MAVE_X    |	\
				 TSC2005_CFR2_MAVE_Y    |	\
				 TSC2005_CFR2_MAVE_Z    |	\
				 TSC2005_CFR2_MEDIUM_15 |	\
				 TSC2005_CFR2_AVG_7)


#define LOBYTE(x) ((unsigned char) (x))
#define HIBYTE(x) ((unsigned char) ((x) >> 8))


#define TIMEOUT_3_SECONDS	3000	//unit msec
#define TIMEOUT_10_SECONDS	10000	//unit msec

#define DELAYS		10000	//10msec

//UPPER_RIGHT
//must x < UPPER_RIGHT_X_RANGE && y < UPPER_RIGHT_Y_RANGE
#define UPPER_RIGHT_X_RANGE	1000
#define UPPER_RIGHT_Y_RANGE	1000

//LOWER_LEFT
//must x > LOWER_LEFT_X_RANGE && y < LOWER_LEFT_Y_RANGE
#define LOWER_LEFT_X_RANGE	3000
#define LOWER_LEFT_Y_RANGE	3000


#define DEFAULT_SPI_BAUDRATE	10*1000*1000 //10M

#define MPP_RESET	18
#define MPP_DAVPEN 	19
#define MPP_SPI_MISO	20
#define MPP_SPI_CS 	21
#define MPP_SPI_MOSI 	22
#define MPP_SPI_CLK 	23

//static int	tsc2005_initialized;

static
int tsc2005_cmd(unsigned char cmd)
{
	unsigned char data;
	int status;

	data = TSC2005_CMD | TSC2005_CMD_12BIT | cmd;
	status = mvSpiWriteThenRead(1,&data,1,NULL,0,0);
	return status;
}



static
int tsc2005_write(u16 reg,u16 value)
{
	unsigned char cmd;
	unsigned char data[2] __attribute__((aligned(16)));
	int status;

	cmd = (TSC2005_REG | reg | TSC2005_REG_PND0 | TSC2005_REG_WRITE);
	data[0] = HIBYTE(value);
	data[1] = LOBYTE(value);

	status = mvSpiWriteThenWrite(1,&cmd,1,&data[0],2);
	return status;
}

static
int ts2005_read(u16 reg,unsigned char *data)
{
	int status;
	unsigned char cmd;

	cmd = (TSC2005_REG | reg | TSC2005_REG_PND0 | TSC2005_REG_READ);
	status = mvSpiWriteThenRead(1,&cmd,1,data,2,0);
	return status;

}


int tsc2005_configuration(void)
{
	int status;

	printf("wr cfr0 = %x\n",TSC2005_CFR0_INITVALUE);
	status = tsc2005_write(TSC2005_REG_CFR0,TSC2005_CFR0_INITVALUE);
	if(status != 0)
	{
		return -1;
	}
	printf("wr cfr1 = %x\n",TSC2005_CFR1_INITVALUE);
	status = tsc2005_write(TSC2005_REG_CFR1,TSC2005_CFR1_INITVALUE);
	if(status != 0)
	{
		return -1;
	}

	printf("wr cfr2 = %x\n",TSC2005_CFR2_INITVALUE);
	status = tsc2005_write(TSC2005_REG_CFR2,TSC2005_CFR2_INITVALUE);
	if(status != 0)
	{
		return -1;
	}

	status = tsc2005_cmd(TSC2005_CMD_SCAN_XYZZ);

	return status;
}

static int tsc2005_check_reg(void)
{
	u8 rxdata[2];
	u16 value;

	ts2005_read(TSC2005_REG_CFR0,&rxdata[0]);
	value = (rxdata[0] << 8)+ rxdata[1];
	printf("rd cfr0 = %x\n",value);
	if((value & 0x3fff) != (TSC2005_CFR0_INITVALUE & 0x3fff))
	{
//		return -1;
	}

	ts2005_read(TSC2005_REG_CFR1,&rxdata[0]);
	value = (rxdata[0] << 8)+ rxdata[1];
	printf("rd cfr1 = %x\n",value);
	if(value != (TSC2005_CFR1_INITVALUE))
	{
//		return -1;
	}

	ts2005_read(TSC2005_REG_CFR2,&rxdata[0]);
	value = (rxdata[0] << 8)+ rxdata[1];
	printf("rd cfr2 = %x\n",value);
	if(value != TSC2005_CFR2_INITVALUE)
	{
//		return -1;
	}
	return 0;

}

static int tsc2005_start_scan(void)
{
	return tsc2005_configuration();
}

static int tsc2005_stop_scan(void)
{
	return tsc2005_cmd(TSC2005_CMD_STOP);
}



static void mv_mpp_set_mode(int mpp,int mode)
{
	u32 temp_reg;
	u32 mpp_control_reg = MPP_CONTROL_REG(0) + (mpp >> 3) *4;
	u8 bit_shift = (mpp % 8)*4;

	temp_reg = MV_REG_READ(mpp_control_reg);
	temp_reg &= ~(0xf << bit_shift);
	temp_reg |= mode << bit_shift;
	MV_REG_WRITE(mpp_control_reg,temp_reg);
}

static void mv_mpp_set_output(int mpp)
{
	if(mpp < 32)
	{
		MV_REG_BIT_RESET(GPP_DATA_OUT_EN_REG(0), (1 << mpp));
	}else{
		MV_REG_BIT_RESET(GPP_DATA_OUT_EN_REG(1), (1 << (mpp-32)));
	}

}

static void mv_mpp_set_input(int mpp)
{
	if(mpp < 32)
	{
		MV_REG_BIT_SET(GPP_DATA_OUT_EN_REG(0), (1 << mpp));
	}else{
		MV_REG_BIT_SET(GPP_DATA_OUT_EN_REG(1), (1 << (mpp-32)));
	}

}

static void mv_wr_mpp(int mpp, int value)
{
	if(mpp < 32)
	{
		if(value){
			MV_REG_BIT_SET(GPP_DATA_OUT_REG(0), 1 << mpp);
		}else{
			MV_REG_BIT_RESET(GPP_DATA_OUT_REG(0), 1 << mpp);
		}
	}else{
		if(value){
			MV_REG_BIT_SET(GPP_DATA_OUT_REG(1), 1 << (mpp-32));
		}else{
			MV_REG_BIT_RESET(GPP_DATA_OUT_REG(1), 1 << (mpp-32));
		}
	}

}

static int mv_rd_mpp(int mpp)
{
	int reg = 0;
	if(mpp < 32)
	{
		reg = MV_REG_READ(GPP_DATA_IN_REG(0));
		reg &= (1 << mpp);
	}else{
		reg = MV_REG_READ(GPP_DATA_IN_REG(1));
		reg &= (1 << (mpp-32));
	}
	return reg ? 1 : 0;
}


void mv_ts_init(void)
{
	/************************************************************/
	/*SPI GPIO (SPI_CS, SPI_CLK, SPI_MISO, SPI_MOSI*/
	/************************************************************/
	//SET SPI MODE (2)
	mv_mpp_set_mode(MPP_SPI_CS, 2);
	mv_mpp_set_mode(MPP_SPI_MOSI, 2);
	mv_mpp_set_mode(MPP_SPI_CLK, 2);
	mv_mpp_set_mode(MPP_SPI_MISO, 2);

	/************************************************************/
	/*RESET PIN*/
	/************************************************************/

	//SET GPIO MODE (0)
	mv_mpp_set_mode(MPP_RESET, 0);

	//SET OUPUT
	mv_mpp_set_output(MPP_RESET);

	//RESET ACTION
	mv_wr_mpp(MPP_RESET, 0);
	udelay(DELAYS);
	mv_wr_mpp(MPP_RESET, 1);

	/************************************************************/
	/*DAVPEN PIN*/
	/************************************************************/

	//set GPIO mode (0)
	mv_mpp_set_mode(MPP_DAVPEN, 0);

	//SET input
	mv_mpp_set_input(MPP_DAVPEN);

	//init baudrate
	mvSysSpiInit(1, DEFAULT_SPI_BAUDRATE);


}

int tsc2005_touched(void)
{

	return mv_rd_mpp(MPP_DAVPEN) ? 0 : 1;
}

static
int ts_report_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	  ulong start;
	  ulong timeout;
	  int x,y;
	  unsigned char rxdata[2] __attribute__((aligned(16)));
	  int touched = 0;


	  if(argc == 1){
		  //use default timeout
		  timeout = TIMEOUT_10_SECONDS;
	  }else{
		  timeout = simple_strtoul (argv[1], NULL, 10); //second
		  timeout = timeout * 1000; //to msecond
	  }

	  mv_ts_init();

	  tsc2005_start_scan();

	  tsc2005_check_reg();

//	  printf("start scan\n");

	  start = get_timer(0);

	  do{
		  if(tsc2005_touched())
		  {
			  touched = 1;

//			  ts2005_read(TSC2005_REG_CFR0,rxdata);
// 			  x = (rxdata[0] << 8) + rxdata[1];
//			  printf("cfr0 = %x\n",x);

			  ts2005_read(TSC2005_REG_X,rxdata);
			  x = (rxdata[0] << 8) + rxdata[1];

			  ts2005_read(TSC2005_REG_Y,rxdata);
			  y = (rxdata[0] << 8) + rxdata[1];

			  printf("touched [%d : %d]\n",x,y);
			  break;
		  }
		  udelay(DELAYS);	//delay 20msecs

	  }while(get_timer(start) < timeout);

	  if(!touched)
	  {
		  printf("timeout\n");
	  }
	  tsc2005_stop_scan();

	  return 0;
}

static
int ts_test_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	unsigned char rxdata[2];
	int x,y;

	if(argc!=1)
	{
		printf("invalid argument\n");
		return -1;
	}

	mv_ts_init();

	tsc2005_start_scan();

	if(tsc2005_check_reg() !=0)
	{
		printf("check reg err\n");
		while(1);
	}
	printf("Please touch the screen\n");
	while(1)
	{
		if(tsc2005_touched()){
			ts2005_read(TSC2005_REG_X,rxdata);
			x = (rxdata[0] << 8) + rxdata[1];

			ts2005_read(TSC2005_REG_Y,rxdata);
			y = (rxdata[0] << 8) + rxdata[1];

			printf("[%d : %d]\n",x,y);

			break;
		}
		udelay(DELAYS);	//delay 20msecs
	}
	printf("Pass\n");

	tsc2005_stop_scan();
	return 0;
}






U_BOOT_CMD(
        ts_report,      2,     0,      ts_report_cmd,
        "ts_report	- report touch screen cooridate\n",
        "timeout (seconds),if you don't specify,the default is 5 seconds\n"
);

U_BOOT_CMD(
        ts_test,      1,     0,      ts_test_cmd,
        "ts_test	- test touch screen\n",
        "You have to touch the screen according to the indication\n"
);
