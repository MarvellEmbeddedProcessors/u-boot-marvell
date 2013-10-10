#include <command.h>

#define MV_SW_RST_CONTROL_REG_CORE0     0x0020800
#define MV_FABRIC_RST_CONTROL_REG       0x0020840
#define MV_FABRIC_CONTROL_REG           0x0020200
#define MV_FABRIC_CONFIG_REG            0x0020204
#define MV_CLOCK_GATING_CONTROL         0x0018220

/*Gbe0, Gbe1, Gbe2, Gbe3 Port RX Queue Configuration*/
unsigned int MV_GBEx_PORT_RXQ_CONFIG_REG[4] = {0x0071400, 0x0075400, 0x0031400, 0x0035400 };

//#define MV_MEMIO32_WRITE(addr, data) printf("mw 0x%X 0x%X\n", (addr), (data))
#define MV_MEMIO32_WRITE(addr, data) ((*((volatile unsigned int*)(addr))) = ((unsigned int)(data)))
#define MV_MEMIO32_READ(addr)        ((*((volatile unsigned int*)(addr))))

int do_warmRst(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int i, gbe, gatingReg;
	unsigned int base = simple_strtoul( argv[1], NULL, 16 );
    /*Set all 4 cores in reset*/
    for ( i=0; i<=3; i++){
        MV_MEMIO32_WRITE((base | (MV_SW_RST_CONTROL_REG_CORE0 + 8*i)), (0x101));
    }

    /*Fix only used GBE default value in RX queue control registers*/
    gatingReg = MV_MEMIO32_READ(MV_CLOCK_GATING_CONTROL);
    for( gbe=0; gbe <=3; gbe++){
        /*If Gbe powered down(bits 1,2,3,4) - skip.*/
        if( (gatingReg>>(gbe+1) & 0x1) ){
            continue;
        }
        for ( i=0; i<=7; i++){
            MV_MEMIO32_WRITE((base | ( MV_GBEx_PORT_RXQ_CONFIG_REG[gbe] + 4*i)), (0x40));
        }
    }
    /*Reset all units in Fabric*/
    MV_MEMIO32_WRITE((base | (MV_FABRIC_RST_CONTROL_REG)), (0xFFFFFFFF));

    /*Set Fabric control and config to defaults*/
    MV_MEMIO32_WRITE((base | (MV_FABRIC_CONTROL_REG)), (0x2));
    MV_MEMIO32_WRITE((base | (MV_FABRIC_CONFIG_REG)), (0x3));

    /*Kick in Fabric units*/
    MV_MEMIO32_WRITE((base | (MV_FABRIC_RST_CONTROL_REG)), (0x0));

    /*Kick in Core0 to start boot process*/
    MV_MEMIO32_WRITE((base | (MV_SW_RST_CONTROL_REG_CORE0)), (0x0));

    return 1;
}

U_BOOT_CMD(
		   warmRst,      2,     1,      do_warmRst,
	 "warmRst	- Warn Reset AXP card over PEX.\n",
  " PEX base address of AXP internal registers \n"
		  "\tWarm reset of AXP card connected via PEX, requeres previously configured PEX base address.\n"
		  );
