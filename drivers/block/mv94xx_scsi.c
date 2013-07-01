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
/*
 * Copyright (C) Freescale Semiconductor, Inc. 2006.
 * Author: Jason Jin<Jason.jin@freescale.com>
 *         Zhang Wei<wei.zhang@freescale.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * 
 *
 */
#include <common.h>

#include <command.h>
#include <pci.h>
#include <asm/processor.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <malloc.h>
#include <scsi.h>
#include <ata.h>
#include <linux/ctype.h>
#include <mv94xx_scsi.h>

struct host_info *mv_host = NULL;
hd_driveid_t *ataid[MV_MAX_DEVICES];
static unsigned long scsi_mem_addr; 
#define msleep(a) udelay(a * 1000)
#define ssleep(a) msleep(a * 1000)

#define bus_to_phys(a)	pci_mem_to_phys(busdevfunc, (unsigned long) (a))
#define phys_to_bus(a)	pci_phys_to_mem(busdevfunc, (unsigned long) (a))

void scsi_write_dword(ulong offset,ulong val)
{
	writel(val, scsi_mem_addr+offset);
}

ulong scsi_read_dword(ulong offset)
{
	return(readl(scsi_mem_addr+offset));
}

static int waiting_for_cmd_completed(u8 core_id,
				     int timeout_msec)
{
	int i;
	u32 status;
	u32 offset1 = mv_host->mmio; /*main irq cause*/
	u32 offset2 = mv_host->mmio + mv_host->core[core_id].mmio_off; 
	u32 sign = (core_id == 0) ? (1<<18) : (1<<19);

	for (i = 0; (!((status = readl(offset1+0x10200)) & sign)) && (!((status = readl(offset2+0x0150)) & (1<<0))) && (i < timeout_msec); i++)
		msleep(1);

	return (i < timeout_msec) ? 0 : -1;
}

#define MV_MAX_DATA_BYTE_COUNT  (128 *1024)

static int issue_sata_cmd(struct dev_info *dev, u8 *fis, int fis_len, u8 *buf,
				int buf_len)
{
	struct mv_cmd_tbl *cmd_tbl = NULL;
	struct mv_cmd_hdr *cmd_hdr = NULL;
	u32 status_buf_off;
	u32 prd_tbl_off;
	u32 cmd_tbl_off;
	struct mv_sg *sg = NULL;
	int i;
	ulong tmp;
	struct port_info *dev_port = (struct port_info *)dev->port;
	struct sas_core *core = (struct sas_core *)dev_port->core;
	u8 phy_id;
	struct dlvq_entry *dlvq_entry;
	struct dlvq_entry *cur_entry = NULL;
	struct cmplq_entry *cplq_entry;
	u16 j;
	u16 slot_nm;
	u32 err_info0 = 0;
	u32 err_info1 = 0;
	

	cmd_tbl = (struct mv_cmd_tbl *)mv_host->cmd_table;
	memset((unsigned char *)mv_host->cmd_table, 0, sizeof(struct mv_cmd_tbl));
	cmd_hdr = (struct mv_cmd_hdr *)mv_host->cmd_list;
	memset((unsigned char *)cmd_hdr, 0, sizeof(struct mv_cmd_hdr));

	status_buf_off = (unsigned char *)&cmd_tbl->status_buf - (unsigned char *)cmd_tbl;
	cmd_hdr->sbf_addr = cpu_to_le32(mv_host->cmd_table) + status_buf_off;
	cmd_hdr->sbf_addr_hi = 0;

	prd_tbl_off = (unsigned char *)&cmd_tbl->prd_entry - (unsigned char *)cmd_tbl;
	cmd_hdr->prdt_addr = cpu_to_le32(mv_host->cmd_table) + prd_tbl_off;

	cmd_hdr->intf_select = 0;
	cmd_hdr->ssp_retry = 1;

	memset((unsigned char *)&cmd_tbl->status_buf.err_info, 0, 8);

	if(dev->dev_type  & DEVICE_TYPE_SATA) {
		cmd_tbl_off = (unsigned char *)&cmd_tbl->stp_cmd_table - (unsigned char *)cmd_tbl;
		cmd_hdr->tbl_addr = cpu_to_le32(mv_host->cmd_table) + cmd_tbl_off;
		cmd_hdr->tbl_addr_hi = 0;
		
	} else {
		printf("other device type %x shouldn't go here\n", dev->dev_type);
		return -1;
	}
	
	if (dev_port->id > mv_host->port_num) {
		printf("Invaild port number %d\n", dev_port->id);
		return -1;
	}

	cmd_hdr->frm_len = 5;
	cmd_hdr->atapi = 0;
	cmd_hdr->reset =0; 
	cmd_hdr->tag = 0;
	cmd_hdr->fst_dma = 0;
	
	if (dev_port->type & PORT_TYPE_PM)
		cmd_hdr->pm_port = dev->pm_num;
	else
	    cmd_hdr->pm_port = 0;
	
	memcpy((unsigned char *)cmd_tbl->stp_cmd_table.fis, fis, fis_len);

	cmd_hdr->prd_entry_cnt = 1;
	if (buf_len) {
		if (buf_len > MV_MAX_DATA_BYTE_COUNT) {
			printf("buf len %x is larger then HW size:%x exit!\n", buf_len, MV_MAX_DATA_BYTE_COUNT);
			return -1;
		}			
		sg = &cmd_tbl->prd_entry;
		sg->addr = cpu_to_le32((u32) buf);
		sg->addr_hi = 0;
		sg->flags_size = cpu_to_le32(0x3fffff & buf_len);
		//printf("sg size:%x\n", sg->flags_size);
	}  
	cmd_hdr->data_len = buf_len;

	/*assign register set*/
	if (dev->register_set != 0xff && core->used_register_set > 64) {
		printf("no register set, exit!\n");
		return -1;
	}
	for( i=0; i<64; i++ )
	{		
		tmp = scsi_read_dword(core->mmio_off + (i<32 ? COMMON_SATA_REG_SET0 : COMMON_SATA_REG_SET1));
		if( !(tmp & (1 <<(i%32)) ))
		{
			dev->register_set = i;
			tmp |= (1<<(i%32));
			scsi_write_dword(core->mmio_off + (i<32 ? COMMON_SATA_REG_SET0 : COMMON_SATA_REG_SET1), tmp);
			break;
		}
	}
	//printf("use register set:%d\n", dev->register_set);
	core->used_register_set++;

	/*deliver queue entry*/
	dlvq_entry = (struct dlvq_entry *)mv_host->dlv_q;
	core->lst_dlvq++;
	if (core->lst_dlvq >= MAX_DELV_QUEUE_SIZE)
		core->lst_dlvq = 0;
	tmp = (u32)core->lst_dlvq;
	cur_entry = &dlvq_entry[tmp];
	memset((unsigned char *)cur_entry, 0, sizeof(struct dlvq_entry));
	cur_entry->slot_nm = 0;
	cur_entry->phy = dev_port->phy_map;
	cur_entry->cmd = 0x03;  
	cur_entry->mode = 1;
	cur_entry->prio = 0;
	cur_entry->sata_reg_set = dev->register_set;
	scsi_write_dword(core->mmio_off + COMMON_DELV_Q_WR_PTR, tmp);
	

	if (waiting_for_cmd_completed(core->id, 10000)) {
		//tmp = scsi_read_dword(0x10200L);
		//printf("after clear main cause:%lx\n", tmp);
		
		//tmp = scsi_read_dword(core->mmio_off + COMMON_IRQ_STAT);
		//printf("after clear irq status:%lx\n", tmp);
		
		printf("issue_sata_cmd: cmd %x to phy_map:%x timeout exit!\n", fis[2], cur_entry->phy);
		return -1;
	}

	tmp = scsi_read_dword(0x10200L);
	scsi_write_dword(0x10200L, tmp);
	tmp = scsi_read_dword(0x10200L);
	//printf("after clear main cause:%lx\n", tmp);

	tmp = scsi_read_dword(core->mmio_off + COMMON_IRQ_STAT);
	scsi_write_dword(core->mmio_off + COMMON_IRQ_STAT, tmp);
	tmp = scsi_read_dword(core->mmio_off + COMMON_IRQ_STAT);
	//printf("after clear irq status:%lx\n", tmp);

	/*clear port irq status*/
	phy_id = 0;
	while (0 == (dev_port->phy_map & (1 <<phy_id)))
			phy_id++;
	tmp = scsi_read_dword(core->mmio_off+COMMON_PORT_IRQ_STAT0+(phy_id * 8));
	scsi_write_dword((core->mmio_off+COMMON_PORT_IRQ_STAT0+(phy_id * 8)), tmp);
	
	/*if support pm, need enhance*/
	
	scsi_write_dword(core->mmio_off + COMMON_IRQ_STAT, INT_CMD_CMPL_MASK); /*clear cmpl int*/

	/*handle cmpl queue*/
	cplq_entry = (struct cmplq_entry *)mv_host->cmpl_q;
	j = core->lst_cmplq;
	core->lst_cmplq = (u16)(*(u32 *)&cplq_entry[0])&0xfff;
	/*wait cmplq update*/
	i = 1000;
	while((j==core->lst_cmplq) && i)
	{
	    	core->lst_cmplq =(u16) le32_to_cpu(scsi_read_dword(core->mmio_off + COMMON_CMPL_Q_WR_PTR))&0xfff;
		i--;
	}
	if(i == 0)
	{
		i=300;
		while((j==core->lst_cmplq) && i)
		{
			udelay(10);		//sometimes main interrupt is abnormal
			core->lst_cmplq =(u16) le32_to_cpu(scsi_read_dword(core->mmio_off + COMMON_CMPL_Q_WR_PTR))&0xfff;
			i--;
		}
	}
	if(j==core->lst_cmplq){
		printf("complete queue isn't updated exit!\n");
		return -1;
	}

	if (core->lst_cmplq!= 0xfff) {
		while (j != core->lst_cmplq) {
			j++;
			if (j > MAX_CMPL_QUEUE_SIZE)
				j = 0;
			
			if (cplq_entry[j + 1].attention) //RXQ_ATTN
			{
				printf("RXQ_ATTN\n");				
				continue;
			}
		
			slot_nm = (u16)cplq_entry[j+1].slot_num;
			if (slot_nm != 0) {
				printf("wrong slot num:%d, only use 0\n", slot_nm);
				continue;
			}
			/*read error info, to be done*/
			err_info0 = cmd_tbl[slot_nm].status_buf.err_info.record1;
			err_info1 = cmd_tbl[slot_nm].status_buf.err_info.record0;
			//printf("error info 0x%08x 0x%08x\n", err_info0, err_info1);

			if (((err_info0 != 0x0L)||(err_info1 != 0x0L)) && (cplq_entry[j + 1].err_rcrd_xfrd))
			{
				printf("has error info, error info 0x%08x 0x%08x, cplq_entry[%x].err_rcrd_xfrd %x\n", err_info0, err_info1, j+1, cplq_entry[j + 1].err_rcrd_xfrd);//cplq_entry %x,  ((u32)(cplq_entry[j + 1])),
				return -1;
			}
			else
			{
				memset((void *)&err_info0, 0, sizeof(u32));
				memset((void *)&err_info1, 0, sizeof(u32));
			}				
			/*check sata r-busy, to be done*/			
		}
	}
	
	/*free register set*/
	if (dev->register_set != 0xff && core->used_register_set > 64) {
		printf("no register set exit!\n");
		return -1;
	}
	tmp = scsi_read_dword(core->mmio_off + (dev->register_set < 32 ? COMMON_SATA_REG_SET0 : COMMON_SATA_REG_SET1));
	tmp &= ~(1 << (dev->register_set%32));
	scsi_write_dword(core->mmio_off + (dev->register_set<32 ? COMMON_SATA_REG_SET0 : COMMON_SATA_REG_SET1), tmp);

	tmp = scsi_read_dword(core->mmio_off + (dev->register_set < 32 ? COMMON_SRS_IRQ_STAT0 : COMMON_SRS_IRQ_STAT1));
	if(tmp & (1 <<(dev->register_set%32)) ) {
		printf("register set %d is stopped !\n", dev->register_set);
		scsi_write_dword(core->mmio_off + (dev->register_set<32 ? COMMON_SRS_IRQ_STAT0 : COMMON_SRS_IRQ_STAT1), tmp);
	}
	dev->register_set = 0xff;
	core->used_register_set--;

	if ((err_info0 != 0) || (err_info1 != 0)) {
		printf("has error info");
		return -1;
	}
	return 0;
}

static char *ata_id_strcpy(u16 *target, u16 *src, int len)
{
	int i;
	for (i = 0; i < len / 2; i++)
		target[i] = swab16(src[i]);
	return (char *)target;
}


static void dump_ataid(u8 dev_id, u8 force)
{
	hd_driveid_t *ata_id = ataid[dev_id]; 
	if (force) {
		int i;
		printf("dev:%d size of identify data:\n", dev_id);
		for (i=0; i<256; i++) { 
			printf("word %d~%d: %4x  %4x	 %4x  %4x  %4x  %4x  %4x  %4x\n", i, i+7, ((u16 *)ata_id)[i], ((u16 *)ata_id)[i+1], ((u16 *)ata_id)[i+2], ((u16 *)ata_id)[i+3], ((u16 *)ata_id)[i+4], ((u16 *)ata_id)[i+5], ((u16 *)ata_id)[i+6], ((u16 *)ata_id)[i+7]);
			i += 7;
		}
	} else {
		debug("(49)ataid->capability = 0x%x\n", ata_id->capability);
		debug("(53)ataid->field_valid =0x%x\n", ata_id->field_valid);
		debug("(63)ataid->dma_mword = 0x%x\n", ata_id->dma_mword);
		debug("(64)ataid->eide_pio_modes = 0x%x\n", ata_id->eide_pio_modes);
		debug("(75)ataid->queue_depth = 0x%x\n", ata_id->queue_depth);
		debug("(80)ataid->major_rev_num = 0x%x\n", ata_id->major_rev_num);
		debug("(81)ataid->minor_rev_num = 0x%x\n", ata_id->minor_rev_num);
		debug("(82)ataid->command_set_1 = 0x%x\n", ata_id->command_set_1);
		debug("(83)ataid->command_set_2 = 0x%x\n", ata_id->command_set_2);
		debug("(84)ataid->cfsse = 0x%x\n", ata_id->cfsse);
		debug("(85)ataid->cfs_enable_1 = 0x%x\n", ata_id->cfs_enable_1);
		debug("(86)ataid->cfs_enable_2 = 0x%x\n", ata_id->cfs_enable_2);
		debug("(87)ataid->csf_default = 0x%x\n", ata_id->csf_default);
		debug("(88)ataid->dma_ultra = 0x%x\n", ata_id->dma_ultra);
	}
}

/*need consider to share code with issue_sata_cmd()*/
static int issue_sata_cmd_nodata(struct dev_info *dev, u8 *fis, int fis_len, u8 is_reset)
{
    struct mv_cmd_tbl *cmd_tbl = NULL;
	struct mv_cmd_hdr *cmd_hdr = NULL;
	u32 status_buf_off;
	u32 prd_tbl_off;
	u32 cmd_tbl_off;
	int i;
	u32 tmp;
	struct port_info *dev_port = (struct port_info *)dev->port;
	struct sas_core *core = (struct sas_core *)dev_port->core;
	u8 phy_id;
	struct dlvq_entry *dlvq_entry;
	struct dlvq_entry *cur_entry = NULL;
	struct cmplq_entry *cplq_entry;
	u16 j;
	u16 slot_nm;
	u32 err_info0 = 0;
	u32 err_info1 = 0;
	
	cmd_tbl = (struct mv_cmd_tbl *)mv_host->cmd_table;
	memset((unsigned char *)mv_host->cmd_table, 0, sizeof(struct mv_cmd_tbl));
	cmd_hdr = (struct mv_cmd_hdr *)mv_host->cmd_list;
	memset((unsigned char *)cmd_hdr, 0, sizeof(struct mv_cmd_hdr));

	status_buf_off = (unsigned char *)&cmd_tbl->status_buf - (unsigned char *)cmd_tbl;
	cmd_hdr->sbf_addr = cpu_to_le32(mv_host->cmd_table) + status_buf_off;
	cmd_hdr->sbf_addr_hi = 0;

	prd_tbl_off = (unsigned char *)&cmd_tbl->prd_entry - (unsigned char *)cmd_tbl;
	cmd_hdr->prdt_addr = cpu_to_le32(mv_host->cmd_table) + prd_tbl_off;

	cmd_hdr->intf_select = 0;
	cmd_hdr->ssp_retry = 1;

	memset((unsigned char *)&cmd_tbl->status_buf.err_info, 0, 8);
	
	cmd_tbl_off = (unsigned char *)&cmd_tbl->stp_cmd_table - (unsigned char *)cmd_tbl;
	cmd_hdr->tbl_addr = cpu_to_le32(mv_host->cmd_table) + cmd_tbl_off;
	cmd_hdr->tbl_addr_hi = 0;	
	
	if (dev_port->id > mv_host->port_num) {
		printf("Invaild port number %d\n", dev_port->id);
		return -1;
	}

	cmd_hdr->frm_len = 5;
	cmd_hdr->atapi = 0;
	cmd_hdr->reset = is_reset; 
	cmd_hdr->tag = 0;
	cmd_hdr->fst_dma = 0; 
	cmd_hdr->pm_port = fis[1];
	
	memcpy((unsigned char *)cmd_tbl->stp_cmd_table.fis, fis, fis_len);

	cmd_hdr->prd_entry_cnt = 1;
	memset(&cmd_tbl->prd_entry, 0, sizeof(struct mv_sg));  
	cmd_hdr->data_len = 0;

	/*assign register set*/
	if (dev->register_set != 0xff && core->used_register_set > 64) {
		printf("no register set exit!\n");
		return -1;
	}
	for( i=0; i<64; i++ )
	{		
		tmp = scsi_read_dword(core->mmio_off + (i<32 ? COMMON_SATA_REG_SET0 : COMMON_SATA_REG_SET1));
		if( !(tmp & (1 <<(i%32)) ))
		{
			dev->register_set = i;
			tmp |= (1<<(i%32));
			scsi_write_dword(core->mmio_off + (i<32 ? COMMON_SATA_REG_SET0 : COMMON_SATA_REG_SET1), tmp);
			break;
		}
	}
	core->used_register_set++;

	/*deliver queue entry*/
	dlvq_entry = (struct dlvq_entry *)mv_host->dlv_q;
	core->lst_dlvq++;
	if (core->lst_dlvq >= MAX_DELV_QUEUE_SIZE)
		core->lst_dlvq = 0;
	tmp = (u32)core->lst_dlvq;	
	cur_entry = &dlvq_entry[tmp];
	memset((unsigned char *)cur_entry, 0, sizeof(struct dlvq_entry));
	cur_entry->slot_nm = 0;
	cur_entry->phy = dev_port->phy_map;
	cur_entry->cmd = 0x03;	/*CMD_STP*/
	cur_entry->mode = 1;
	cur_entry->prio = 0;
	cur_entry->sata_reg_set = dev->register_set;
	scsi_write_dword(core->mmio_off + COMMON_DELV_Q_WR_PTR, tmp);	

	if (waiting_for_cmd_completed(core->id, 10000)) {
		//tmp = scsi_read_dword(0x10200L);
		//printf("errrafter clear main cause:%lx\n", tmp);
		
		//tmp = scsi_read_dword(core->mmio_off + COMMON_IRQ_STAT);
		//printf("errrafter clear irq status:%lx\n", tmp);
		
		printf("issue_sata_cmd_nodata: cmd %x to phy_map:%x timeout exit!\n", fis[2], cur_entry->phy);
		return -1;
	}

	if (is_reset)
		msleep(10);

	tmp = scsi_read_dword(0x10200L);
	//printf("before clear main cause:%lx\n", tmp);
	scsi_write_dword(0x10200L, tmp);
	tmp = scsi_read_dword(0x10200L);
	//printf("after clear main cause:%lx\n", tmp);

	tmp = scsi_read_dword(core->mmio_off + COMMON_IRQ_STAT);
	//printf("before clear irq status:%lx\n", tmp);
	scsi_write_dword(core->mmio_off + COMMON_IRQ_STAT, tmp);
	tmp = scsi_read_dword(core->mmio_off + COMMON_IRQ_STAT);
	//printf("after clear irq status:%lx\n", tmp);
	
	/*clear port irq status*/
	phy_id = 0;
	while (0 == (dev_port->phy_map & (1 <<phy_id)))
			phy_id++;
	tmp = scsi_read_dword(core->mmio_off+COMMON_PORT_IRQ_STAT0+(phy_id * 8));
	//printf("before clear port irq status: %lx\n", tmp);
	scsi_write_dword((core->mmio_off+COMMON_PORT_IRQ_STAT0+(phy_id * 8)), tmp);
	tmp = scsi_read_dword(core->mmio_off+COMMON_PORT_IRQ_STAT0+(phy_id * 8));
	//printf("After clear port irq status: %lx\n", tmp);

	/*if support pm, need enhance*/
	
	scsi_write_dword(core->mmio_off + COMMON_IRQ_STAT, INT_CMD_CMPL_MASK); /*clear cmpl int*/

	/*handle cmpl queue*/
	cplq_entry = (struct cmplq_entry *)mv_host->cmpl_q;
	j = core->lst_cmplq;
	core->lst_cmplq = (u16)(*(u32 *)&cplq_entry[0])&0xfff;
	//printf("j= %x, core->lst_cmplq = %x \n", j, core->lst_cmplq );
	/*wait cmplq update*/
	i = 3000;
	while((j==core->lst_cmplq) && i)
	{
		core->lst_cmplq =(u16) le32_to_cpu(scsi_read_dword(core->mmio_off + COMMON_CMPL_Q_WR_PTR))&0xfff;
		i--;
	}
	if(i == 0)
	{
		i=300;
		while((j==core->lst_cmplq) && i)
		{
			msleep(10); 	//sometimes main interrupt is abnormal
			core->lst_cmplq =(u16) le32_to_cpu(scsi_read_dword(core->mmio_off + COMMON_CMPL_Q_WR_PTR))&0xfff;
			i--;
		}
	}
	if(j==core->lst_cmplq){
		printf("complete queue isn't update exit!\n");
		return -1;
	}
	else{
		tmp = scsi_read_dword(core->mmio_off + 0x10200L);
		if (0 != tmp)
			printf("main irq == %x \n", tmp);
	}

	if (core->lst_cmplq!= 0xfff) {
		while (j != core->lst_cmplq) {
			j++;

			if (j >= MAX_CMPL_QUEUE_SIZE)
				j = 0;
			
			if (cplq_entry[j + 1].attention) //RXQ_ATTN
			{ 
				printf("RXQ_ATTN\n");				
				continue;
			}
			
			slot_nm = (u16)cplq_entry[j+1].slot_num;


			if (slot_nm != 0) {
				printf("wrong slot num:%d, only use 0\n", slot_nm);
				continue;
			}
			/*read error info, to be done*/
			err_info0 = cmd_tbl[slot_nm].status_buf.err_info.record1; 
			err_info1 = cmd_tbl[slot_nm].status_buf.err_info.record0;
		//	printf("error info 0x%08x 0x%08x\n", err_info0, err_info1);

			if (((err_info0 != 0x0L)||(err_info1 != 0x0L)) && (cplq_entry[j + 1].err_rcrd_xfrd))
			{
				printf("has error info, error info 0x%08x 0x%08x, cplq_entry[%x].err_rcrd_xfrd %x\n", err_info0, err_info1, j+1, cplq_entry[j + 1].err_rcrd_xfrd);
				return -1;
			}
			else
			{
				memset((void *)&err_info0, 0, sizeof(u32));
				memset((void *)&err_info1, 0, sizeof(u32));
			}				
				
			/*check sata r-busy, to be done*/			
		}
	}
	
	/*free register set*/
	if (dev->register_set != 0xff && core->used_register_set > 64) {
		printf("no register set exit!\n");
		return -1;
	}
	tmp = scsi_read_dword(core->mmio_off + (dev->register_set < 32 ? COMMON_SATA_REG_SET0 : COMMON_SATA_REG_SET1));
	tmp &= ~(1 << (dev->register_set%32));
	scsi_write_dword(core->mmio_off + (dev->register_set<32 ? COMMON_SATA_REG_SET0 : COMMON_SATA_REG_SET1), tmp);

	tmp = scsi_read_dword(core->mmio_off + (dev->register_set < 32 ? COMMON_SRS_IRQ_STAT0 : COMMON_SRS_IRQ_STAT1));
	if(tmp & (1 <<(dev->register_set%32)) ) {
		printf("register set %d is stopped !\n", dev->register_set);
		scsi_write_dword(core->mmio_off + (dev->register_set<32 ? COMMON_SRS_IRQ_STAT0 : COMMON_SRS_IRQ_STAT1), tmp);
	}
	dev->register_set = 0xff;
	core->used_register_set--;

	if ((err_info0 != 0) || (err_info1 != 0)) {
		printf("has error info\n");
		return -1;
	}
	return 0;
}


/*
 * soft-reset.
 */
static int ata_issue_softreset(struct dev_info *dev, u8 srst, u8 port_reset)
{
	u8 fis[20];

	memset(fis, 0, 20);
	/* Construct the FIS */
	fis[0] = 0x27;		/* Host to device FIS. */
	fis[1] = port_reset ? 0x0f : dev->pm_num;	/* Command FIS. */
	fis[2] = 0;	/* Command byte. */

	fis[15] = srst ? (1<<2) : 0; /*Control */
		
	if (issue_sata_cmd_nodata(dev, (u8 *) &fis, 20, 1)) {
		debug("softreset failure.\n");
		return -EIO;
	}

	return 0;
}

/*
 * SCSI INQUIRY command operation.
 */
static int ata_scsiop_inquiry(ccb *pccb)
{
	struct dev_info *dev = NULL;
	struct port_info *port = NULL;
	u8 hdr[] = {
		0,
		0,
		0x5,		/* claim SPC-3 version compatibility */
		2,
		95 - 4,
	};
	u8 fis[20];
	u8 *tmpid;
	//u8 port;

	/* Clean ccb data buffer */
	memset(pccb->pdata, 0, pccb->datalen);

	memcpy(pccb->pdata, hdr, sizeof(hdr));

	if (pccb->datalen <= 35)
		return 0;

	memset(fis, 0, 20);
	/* Construct the FIS */
	fis[0] = 0x27;		/* Host to device FIS. */
	fis[1] = 1 << 7;	/* Command FIS. */
	fis[2] = ATA_CMD_IDENT;	/* Command byte. */
	fis[3] = 1 << 0;
	fis[7] = 0x40; /*device */

	/* Read id from sata */
	dev = &mv_host->devices[pccb->target];
	port = dev->port;

	if (port->type & PORT_TYPE_PM)
		fis[1] |= dev->pm_num;
	
	if (!(tmpid = malloc(sizeof(hd_driveid_t))))
		return -ENOMEM;
	
	memset(tmpid, 0, sizeof(hd_driveid_t));

	if (issue_sata_cmd(dev, (u8 *) &fis, 20,
				 tmpid, sizeof(hd_driveid_t))) {
		debug("scsi_mv94xx: SCSI inquiry command failure.\n");
		return -EIO;
	}

	if (ataid[dev->id]) {
		free(ataid[dev->id]);
		ataid[dev->id] = NULL;
	}
	ataid[dev->id] = (hd_driveid_t *) tmpid;

	memcpy(&pccb->pdata[8], "ATA     ", 8);
	ata_id_strcpy((u16 *) &pccb->pdata[16], (u16 *)ataid[dev->id]->model, 16);
	ata_id_strcpy((u16 *) &pccb->pdata[32], (u16 *)ataid[dev->id]->fw_rev, 4);

	dump_ataid(dev->id, 0);
	return 0;
}


/*
 * SCSI READ10 command operation.
 */
static int ata_scsiop_read10(ccb * pccb)
{
	struct dev_info *dev = NULL;
	struct port_info *port = NULL;
	u32 len = 0;
	u8 fis[20];
	u8 lba48 = 0;

	if (ataid[pccb->target]->command_set_2 & 0x0400) {
		/*48bit lba*/
		lba48 = 1;
	}

	len = (((u32) pccb->cmd[7]) << 8) | ((u32) pccb->cmd[8]);

	/* For 10-byte and 16-byte SCSI R/W commands, transfer
	 * length 0 means transfer 0 block of data.
	 * However, for ATA R/W commands, sector count 0 means
	 * 256 or 65536 sectors, not 0 sectors as in SCSI.
	 *
	 * WARNING: one or two older ATA drives treat 0 as 0...
	 */
	if (!len)
		return 0;
	memset(fis, 0, 20);

	/* Construct the FIS */
	fis[0] = 0x27;		/* Host to device FIS. */
	fis[1] = 1 << 7;	/* Command FIS. */
	if (lba48)
		fis[2] = 0x25;	/* Command byte. */
	else
		fis[2] = 0xC8;	/* Command byte. */
	fis[3] = 1<<0;

	/* LBA address, only support LBA28 in this driver */
	fis[4] = pccb->cmd[5];
	fis[5] = pccb->cmd[4];
	fis[6] = pccb->cmd[3];

	if(lba48) {
		fis[7] = 0x40;
		fis[8] = pccb->cmd[2];
		
		/* Sector Count */
		fis[12] = pccb->cmd[8];
		fis[13] = pccb->cmd[7];
	}
	else {
		fis[7] = (pccb->cmd[2] & 0x0f) | 0x40; //0xe0;
				/* Sector Count */
		fis[12] = pccb->cmd[8];
		fis[13] = 0; // pccb->cmd[7];
	}

	/* Read */
	dev = &mv_host->devices[pccb->target];
	port = dev->port;
	if (port->type & PORT_TYPE_PM)
		fis[1] |= dev->pm_num;
	
	if (issue_sata_cmd(dev, (u8 *) &fis, 20,
				 pccb->pdata, pccb->datalen)) {
		debug("mv94xx: SCSI READ10 command failure.\n");
		return -EIO;
	}

	return 0;
}


/*
 * SCSI READ CAPACITY10 command operation.
 */
static int ata_scsiop_read_capacity10(ccb *pccb)
{
	//ulong cap;
	u32 cap_low, cap_high=0x0;
	u32 *tmp32 = (u32 *)pccb->pdata;
	u32 *tmp_cap = NULL;

	if (!ataid[pccb->target]) {
		printf("scsi_mv94xx: SCSI READ CAPACITY10 command failure. "
		       "\tNo ATA info!\n"
		       "\tPlease run SCSI commmand INQUIRY firstly!\n");
		return -EPERM;
	}

	cap_low = le32_to_cpu((u32)ataid[pccb->target]->lba_capacity);
	cap_high = 0;

#ifdef CONFIG_LBA48
	if (ataid[pccb->target]->command_set_2 & 0x0400) {	/* LBA 48 support */
		tmp_cap = (u32 *)&ataid[pccb->target]->lba48_capacity[0];
		cap_low = le32_to_cpu(*tmp_cap);
		tmp_cap = (u32 *)&ataid[pccb->target]->lba48_capacity[2];
		cap_high = le32_to_cpu(*tmp_cap);
	} 
	//printf("ata_scsiop_read_capacity10:lba:%lx\n", cap);
#endif /* CONFIG_LBA48 */


	if (pccb->datalen >= 8) {
		if (cap_high != 0)
			tmp32[0] = cpu_to_be32(0xffffffff);
		else
			tmp32[0] = cpu_to_be32(cap_low);
		tmp32[1] = cpu_to_be32(512); /*Sector Size default 512byte*/
	}
	
	return 0;
}


/*
 * SCSI TEST UNIT READY command operation.
 */
static int ata_scsiop_test_unit_ready(ccb *pccb)
{
	return (ataid[pccb->target]) ? 0 : -EPERM;
}

static u8 ssp_normalize_sense(u8* sense_buffer, u32 sb_len, struct sas_sense_hdr* sshdr)
{
	if (!sense_buffer || !sb_len)
		return 0;

    memset(sshdr, 0, sizeof(*sshdr));	
	sshdr->response_code = (sense_buffer[0] & 0x7f);

	if ((sshdr->response_code & 0x70)!= 0x70)
		return 0;

	if (sshdr->response_code >= 0x72) {
        if (sb_len > 1)
            sshdr->sense_key = (sense_buffer[1] & 0xf);
        if (sb_len > 2)
            sshdr->asc = sense_buffer[2];
        if (sb_len > 3)
            sshdr->ascq = sense_buffer[3];
    } else {
        if (sb_len > 2)
            sshdr->sense_key = (sense_buffer[2] & 0xf);
        if (sb_len > 7) {
            sb_len = (sb_len < (u32)(sense_buffer[7] + 8)) ?
                   sb_len : (u32)(sense_buffer[7] + 8);
            if (sb_len > 12)
                sshdr->asc = sense_buffer[12];
            if (sb_len > 13)
                sshdr->ascq = sense_buffer[13];
        }
    }
	return 1;
}

/*need consider to share complete queue entry check with issue_sata_cmd()*/
static int issue_sas_cmd(struct dev_info *dev, u8 *cdb, u8 *buf,
				int buf_len)
{
	struct mv_cmd_tbl *cmd_tbl = NULL;
	struct mv_cmd_hdr *cmd_hdr = NULL;
	u32 status_buf_off;
	u32 prd_tbl_off;
	u32 tmp_tbl_off;
	struct mv_sg *sg = NULL;
	int i;
	ulong tmp;
	struct port_info *dev_port = (struct port_info *)dev->port;
	struct sas_core *core = (struct sas_core *)dev_port->core;
	u8 phy_id;
	struct dlvq_entry *dlvq_entry;
	struct dlvq_entry *cur_entry = NULL;
	struct cmplq_entry *cplq_entry;
	u16 j;
	u16 slot_nm;
	u16 *tmp16 = NULL;
	u32 err_info0 = 0;
	u32 err_info1 = 0;
	u32 sense_len;
	struct sas_sense_hdr sshdr;	

	cmd_tbl = (struct mv_cmd_tbl *)mv_host->cmd_table;
	memset((unsigned char *)mv_host->cmd_table, 0, sizeof(struct mv_cmd_tbl));
	cmd_hdr = (struct mv_cmd_hdr *)mv_host->cmd_list;
	memset((unsigned char *)cmd_hdr, 0, sizeof(struct mv_cmd_hdr));

	status_buf_off = (unsigned char *)&cmd_tbl->status_buf - (unsigned char *)cmd_tbl;
	cmd_hdr->sbf_addr = cpu_to_le32(mv_host->cmd_table) + status_buf_off;
	cmd_hdr->sbf_addr_hi = 0;

	prd_tbl_off = (unsigned char *)&cmd_tbl->prd_entry - (unsigned char *)cmd_tbl;
	cmd_hdr->prdt_addr = cpu_to_le32(mv_host->cmd_table) + prd_tbl_off;

	cmd_hdr->intf_select = 0;
	cmd_hdr->ssp_retry = 1;

	memset((unsigned char *)&cmd_tbl->status_buf.err_info, 0, 8);

	if(dev->dev_type  & DEVICE_TYPE_SATA) {
		tmp_tbl_off = (unsigned char *)&cmd_tbl->stp_cmd_table - (unsigned char *)cmd_tbl;
		cmd_hdr->tbl_addr = cpu_to_le32(mv_host->cmd_table) + tmp_tbl_off;
		cmd_hdr->tbl_addr_hi = 0;
		
	} else if(dev->dev_type  & DEVICE_TYPE_SSP) {
		tmp_tbl_off = (unsigned char *)&cmd_tbl->open_addr_frame - (unsigned char *)cmd_tbl;
		cmd_hdr->opf_addr = cpu_to_le32(mv_host->cmd_table) + tmp_tbl_off;
		cmd_hdr->opf_addr_hi = 0;
	
		tmp_tbl_off = (unsigned char *)&cmd_tbl->ssp_cmd_table - (unsigned char *)cmd_tbl;
		cmd_hdr->tbl_addr = cpu_to_le32(mv_host->cmd_table) + tmp_tbl_off;
		cmd_hdr->tbl_addr_hi = 0;
	
	} else {
		printf("other device type %x shouldn't go here\n", dev->dev_type);
		return -1;
	}
	
	if (dev_port->id > mv_host->port_num) {
		printf("Invaild port number %d\n", dev_port->id);
		return -1;
	}

	cmd_hdr->max_rsp_len = (sizeof(struct ssp_rsp_iu) > 0x200 ? sizeof(struct ssp_rsp_iu) : 0x200)/4;
	cmd_hdr->frm_len = (sizeof(struct ssp_cmd_iu) + sizeof(struct ssp_frame_hdr))/4;
	cmd_hdr->ssp_ft = 0x00; //frame_type_cmd
	
	cmd_hdr->tag |= 0xab << 9; //internal used tag

	memcpy(&cmd_tbl->ssp_cmd_table.cmd_iu.cdb[0], cdb, 16);

	/*assert only support single lun*/
	cmd_tbl->open_addr_frame.protocol = 0x1; //protocol SSP
	cmd_tbl->open_addr_frame.frame_type = 0x1; //open frame
	cmd_tbl->open_addr_frame.initiator = 1;
	tmp16 = (u16 *)&(cmd_tbl->open_addr_frame.connect_tag[0]);
	*tmp16 = cpu_to_be16(dev->id + 1);
	cmd_tbl->open_addr_frame.connect_rate = dev->link_rate;
	cmd_tbl->open_addr_frame.dst_sas_addr_low = be32_to_cpu(dev->sas_addr_low);
	cmd_tbl->open_addr_frame.dst_sas_addr_high = be32_to_cpu(dev->sas_addr_high);
	
	cmd_hdr->prd_entry_cnt = 1;
	if (buf_len) {
		if (buf_len > MV_MAX_DATA_BYTE_COUNT) {
			printf("buf len %x is larger then HW size:%x exit!\n", buf_len, MV_MAX_DATA_BYTE_COUNT);
			return -1;
		}			
		sg = &cmd_tbl->prd_entry;
		sg->addr = cpu_to_le32((u32) buf);
		sg->addr_hi = 0;
		sg->flags_size = cpu_to_le32(0x3fffff & buf_len);
		//printf("sg size:%x\n", sg->flags_size);
	}  
	cmd_hdr->data_len = cpu_to_le32(buf_len);

	/*deliver queue entry*/
	dlvq_entry = (struct dlvq_entry *)mv_host->dlv_q;
	core->lst_dlvq++;
	if (core->lst_dlvq >= MAX_DELV_QUEUE_SIZE)
		core->lst_dlvq = 0;
	tmp = (u32)core->lst_dlvq;
	cur_entry = &dlvq_entry[tmp];
	memset((unsigned char *)cur_entry, 0, sizeof(struct dlvq_entry));
	cur_entry->slot_nm = 0;
	cur_entry->phy = dev_port->phy_map;
	cur_entry->cmd = 0x01; //cmd_ssp  
	cur_entry->mode = 1;
	cur_entry->prio = 0;
	scsi_write_dword(core->mmio_off + COMMON_DELV_Q_WR_PTR, tmp);
	

	if (waiting_for_cmd_completed(core->id, 10000)) {
		//tmp = scsi_read_dword(0x10200L);
		//printf("errrafter clear main cause:%lx\n", tmp);
		
		//tmp = scsi_read_dword(core->mmio_off + COMMON_IRQ_STAT);
		//printf("errrafter clear irq status:%lx\n", tmp);
		
		return -1;
	}

	tmp = scsi_read_dword(0x10200L);
	scsi_write_dword(0x10200L, tmp);
	tmp = scsi_read_dword(0x10200L);
	//printf("after clear main cause:%lx\n", tmp);

	tmp = scsi_read_dword(core->mmio_off + COMMON_IRQ_STAT);
	scsi_write_dword(core->mmio_off + COMMON_IRQ_STAT, tmp);
	tmp = scsi_read_dword(core->mmio_off + COMMON_IRQ_STAT);
	//printf("after clear irq status:%lx\n", tmp);

	/*clear port irq status*/
	phy_id = 0;
	while (0 == (dev_port->phy_map & (1 <<phy_id)))
			phy_id++;
	tmp = scsi_read_dword(core->mmio_off+COMMON_PORT_IRQ_STAT0+(phy_id * 8));
	scsi_write_dword((core->mmio_off+COMMON_PORT_IRQ_STAT0+(phy_id * 8)), tmp);
	
	/*if support pm, need enhance*/
	
	scsi_write_dword(core->mmio_off + COMMON_IRQ_STAT, INT_CMD_CMPL_MASK); /*clear cmpl int*/

	/*handle cmpl queue*/
	cplq_entry = (struct cmplq_entry *)mv_host->cmpl_q;
	j = core->lst_cmplq;
	core->lst_cmplq = (u16)(*(u32 *)&cplq_entry[0])&0xfff;
	/*wait cmplq update*/
	i = 1000;
	while((j==core->lst_cmplq) && i)
	{
	    	core->lst_cmplq =(u16) le32_to_cpu(scsi_read_dword(core->mmio_off + COMMON_CMPL_Q_WR_PTR))&0xfff;
		i--;
	}
	if(i == 0)
	{
		i=300;
		while((j==core->lst_cmplq) && i)
		{
			udelay(10);		//sometimes main interrupt is abnormal
			core->lst_cmplq =(u16) le32_to_cpu(scsi_read_dword(core->mmio_off + COMMON_CMPL_Q_WR_PTR))&0xfff;
			i--;
		}
	}
	if(j==core->lst_cmplq){
		printf("complete queue isn't update exit!\n");
		return -1;
	}

	if (core->lst_cmplq!= 0xfff) {
		while (j != core->lst_cmplq) {
			j++;
			if (j > MAX_CMPL_QUEUE_SIZE)
				j = 0;
			slot_nm = (u16)cplq_entry[j+1].slot_num;
			if (slot_nm != 0) {
				printf("wrong slot num:%d, only use 0\n", slot_nm);
				continue;
			}
			/*read error info, to be done*/
			err_info0 = cmd_tbl[slot_nm].status_buf.err_info.record1;
			err_info1 = cmd_tbl[slot_nm].status_buf.err_info.record0;
			//printf("error info 0x%08x 0x%08x\n", err_info0, err_info1);
			if (cmd_tbl[slot_nm].status_buf.rsp_iu.status != 0x00) /*SCSI_STATUS_GOOD*/	 {
				printf("ssp response failed status :%x\n", cmd_tbl[slot_nm].status_buf.rsp_iu.status);

				sense_len = be32_to_cpu(cmd_tbl[slot_nm].status_buf.rsp_iu.sd_len);

                if (sense_len && (cmd_tbl[slot_nm].status_buf.rsp_iu.data_pres & 0x2) &&
					ssp_normalize_sense(cmd_tbl[slot_nm].status_buf.rsp_iu.sense_data, sense_len, &sshdr))
                {
                    printf("dev %d req 0x%x status 0x%x sense length %d sense(key 0x%x, asc 0x%x ascq 0x%x).\n", \
			        dev->id, cdb[0], cmd_tbl[slot_nm].status_buf.rsp_iu.status, sense_len, \
			        sshdr.sense_key, sshdr.asc, sshdr.ascq);
                }
				
				return -1;
			}
			/*check sata r-busy, to be done*/			
		}
	}

	if ((err_info0 != 0) || (err_info1 != 0)) {
		printf("has error info");
		return -1;
	}

	return 0;
}
static int sas_scsiop_inquiry(ccb *pccb)
{
	struct dev_info *dev = NULL;
	u8 cdb[16];
	u8 hdr[] = {
		0,
		0,
		0x5,		/* claim SPC-3 version compatibility */
		2,
		95 - 4,
	};
	u8 *tmpid;

	/* Clean ccb data buffer */
	memset(pccb->pdata, 0, pccb->datalen);
	memcpy(pccb->pdata, hdr, sizeof(hdr));

	if (pccb->datalen <= 35)
		return 0;

	/* Read id from sata */
	dev = &mv_host->devices[pccb->target];
	
	if (!(tmpid = malloc(96)))
		return -ENOMEM;	
	memset(tmpid, 0, 0x60);

	memset(cdb, 0, 16);
	cdb[0] = pccb->cmd[0];
	cdb[4] = 0x60; //standard inquiry
	//printf("receive SAS inquiry cmd!!\n");
	if (issue_sas_cmd(dev, (u8 *) &cdb, tmpid, 0x60)) {
		debug("scsi_mv94xx: SCSI inquiry command failure.\n");
		return -EIO;
	}
	memcpy(&pccb->pdata[8], "SAS     ", 8);
	memcpy((u8 *) &pccb->pdata[16], (u8 *)&tmpid[16], 16);
	memcpy((u8 *) &pccb->pdata[32], (u8 *)&tmpid[32], 4);
	return 0;
}

static int sas_scsiop_read10(ccb *pccb)
{
	struct dev_info *dev = NULL;
	u8 cdb[16];
	u32 len=0;

	len = (((u32) pccb->cmd[7]) << 8) | ((u32) pccb->cmd[8]);
	if (!len)
		return 0;

	memset(cdb, 0, 16);
	cdb[0] = pccb->cmd[0];
	cdb[2] = pccb->cmd[2];
	cdb[3] = pccb->cmd[3];
	cdb[4] = pccb->cmd[4];
	cdb[5] = pccb->cmd[5];
	cdb[7] = pccb->cmd[7];
	cdb[8] = pccb->cmd[8];
	dev = &mv_host->devices[pccb->target];

	if (issue_sas_cmd(dev, (u8 *) &cdb, pccb->pdata, pccb->datalen)) {
		debug("scsi_mv94xx: SCSI read10 command failure.\n");
		return -EIO;
	}

	return 0;
}

static int sas_scsiop_common_cmds(ccb *pccb)
{
	struct dev_info *dev = NULL;
	u8 cdb[16];
	struct mv_cmd_tbl *cmd_tbl = NULL;
	u8 sense_key;
	u8 retry_cnt = 0;
	
	/* Read id from sata */
	dev = &mv_host->devices[pccb->target];
	
	memset(cdb, 0, 16);
	cdb[0] = pccb->cmd[0];	

retry:
	if (issue_sas_cmd(dev, (u8 *) &cdb, pccb->pdata, pccb->datalen)) {
		cmd_tbl = (struct mv_cmd_tbl *)mv_host->cmd_table;
		if (cmd_tbl[0].status_buf.rsp_iu.status == 0x02) /*Check condition*/ {
			sense_key = cmd_tbl[0].status_buf.rsp_iu.sense_data[2] & 0x0f;
			/*SCSI_SK_UNIT_ATTENTION & SCSI_SK_NOT_READY */
		    if ((cdb[0] == 0x0) && ((sense_key == 0x06)|| (sense_key == 0x02))) { 
		       msleep(100);
			   retry_cnt++;

			   if (retry_cnt <= 10)
			   {
			       //printf("retry cmd %x , retry count is %x\n", cdb[0], retry_cnt);
			       goto retry;
			   }
			   else
			       return -EIO;
		    }
		}
		debug("scsi_mv94xx: SCSI command %x failure.\n", pccb->cmd[0]);
		return -EIO;
	}
	return 0;
}

int scsi_exec(ccb *pccb)
{
	int ret;

	if (mv_host->devices[pccb->target].dev_type & DEVICE_TYPE_SATA) {
		switch (pccb->cmd[0]) {
		case SCSI_READ10:
			ret = ata_scsiop_read10(pccb);
			break;
		case SCSI_RD_CAPAC:
			ret = ata_scsiop_read_capacity10(pccb);
			break;
		case SCSI_TST_U_RDY:
			ret = ata_scsiop_test_unit_ready(pccb);
			break;
		case SCSI_INQUIRY:
			ret = ata_scsiop_inquiry(pccb);
			break;
		default:
			printf("Unsupport SCSI command 0x%02x\n", pccb->cmd[0]);
			return FALSE;
		}
	} else if (mv_host->devices[pccb->target].dev_type & DEVICE_TYPE_SSP) {
		switch (pccb->cmd[0]) {
		case SCSI_READ10:
			ret = sas_scsiop_read10(pccb);
			break;
		case SCSI_RD_CAPAC:
		case SCSI_TST_U_RDY:
			ret = sas_scsiop_common_cmds(pccb);
			break;
		case SCSI_INQUIRY:
			ret = sas_scsiop_inquiry(pccb);
			break;
		default:
			//printf("temp not support SAS device command 0x%02x\n", pccb->cmd[0]);
			return FALSE;
		}
	} else {
		/*other type device*/
		return FALSE;
	}

	if (ret) {
		debug("SCSI command 0x%02x ret errno %d\n", pccb->cmd[0], ret);
		return FALSE;
	} 
	
	return TRUE;

}

void scsi_write_phy_vsr_addr(ulong offset, u8 phy_id, ulong val)
{
	writel(val, scsi_mem_addr+offset+0x0250+(phy_id*8));
}

void scsi_write_phy_vsr_data(ulong offset, u8 phy_id, ulong val)
{
	writel(val, scsi_mem_addr+offset+0x0254+(phy_id*8));
}

ulong scsi_read_vsr_data(ulong offset, u8 phy_id)
{
	return(readl(scsi_mem_addr+offset+0x0254+(phy_id*8)));
}

static void mv_phy_config(struct sas_core *core, u8 phy_id) {
	ulong tmp;
	ulong off = core->mmio_off;
	int loop=0;

	//printf("spin up phy:%d of core %d %p off:%lx\n", phy_id, core->id, core, off);

	/*set dev info*/
	scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_ID_FRAME5);
	tmp = scsi_read_dword(off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8));
	tmp &= 0xffffff00;
	tmp |= phy_id;
	scsi_write_dword((off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8)), tmp);

	tmp = (1<<4) + (((1<<9)|(1<<10)|(1<<11))<<8);
	scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_ID_FRAME0);
	scsi_write_dword((off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8)), tmp);	

	/*set sas addr*/
	scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_ID_FRAME4);
	tmp = 0x50050430;
	scsi_write_dword((off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8)), tmp);
	scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_ID_FRAME3);
	tmp = 0x11ab0000;
	tmp |= phy_id;
	scsi_write_dword((off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8)), tmp);

	/*enable phy*/
	scsi_write_dword((off+COMMON_PORT_VSR_ADDR0+(phy_id * 8)), VSR_PHY_CONFIG);
	tmp = scsi_read_dword(off+COMMON_PORT_VSR_DATA0+(phy_id * 8));
	tmp |= (1<<0);
	scsi_write_dword((off+COMMON_PORT_VSR_DATA0+(phy_id * 8)), tmp&0xfd7fffffL);

#if 1
	scsi_write_dword((off+COMMON_PORT_VSR_ADDR0+(phy_id * 8)), 0x144);
	scsi_write_dword((off+COMMON_PORT_VSR_DATA0+(phy_id * 8)), 0x08001006L);
	scsi_write_dword((off+COMMON_PORT_VSR_ADDR0+(phy_id * 8)), 0x1b4);
	scsi_write_dword((off+COMMON_PORT_VSR_DATA0+(phy_id * 8)), 0x0000705fL);

#endif
	/* Should clean the unassociated FIS interrupt before issuing
	 * reset phy. Saw a case with a special WD hard drive that
	 * sends two signature FIS's. For this case, if we do not clean
	 * the status, hardware could get stuck not responding to X_RDY
	 * with R_RDY
	 */ 	
	tmp = scsi_read_dword(off+COMMON_PORT_IRQ_STAT0+(phy_id * 8));
	if (tmp & IRQ_UNASSOC_FIS_RCVD_MASK) {
		scsi_write_dword((off+COMMON_PORT_IRQ_STAT0+(phy_id * 8)), IRQ_UNASSOC_FIS_RCVD_MASK);
	} 
	
	tmp = scsi_read_dword(off+COMMON_PORT_PHY_CONTROL0+(phy_id * 4));
	tmp |= (1<<0);
	scsi_write_dword((off+COMMON_PORT_PHY_CONTROL0+(phy_id * 4)), tmp);	

	msleep(100);
	tmp = scsi_read_dword(off+COMMON_PORT_PHY_CONTROL0+(phy_id * 4));
	//printf("after write %lx\n", tmp);
	while((tmp & SCTRL_STP_LINK_LAYER_RESET) && (loop++<100)){
		msleep(10);
		tmp = scsi_read_dword(off+COMMON_PORT_PHY_CONTROL0+(phy_id * 4));
		//printf("after write loop tmp %lx loop:%d\n", tmp, loop);
	}
	//printf("loop :%d\n", loop);
	
	/*reset irq*/
	tmp = scsi_read_dword(off+COMMON_PORT_IRQ_STAT0+(phy_id * 8));
	scsi_write_dword((off+COMMON_PORT_IRQ_STAT0+(phy_id * 8)), tmp);
	
	tmp = scsi_read_dword(off+COMMON_PORT_IRQ_STAT0+(phy_id * 8));
	tmp &= ~(1<<16);
	scsi_write_dword((off+COMMON_PORT_IRQ_STAT0+(phy_id * 8)), tmp);
	
	/* enable phy change interrupt and broadcast change */
	tmp = IRQ_UNASSOC_FIS_RCVD_MASK | IRQ_ASYNC_NTFCN_RCVD_MASK;
	scsi_write_dword((off+COMMON_PORT_IRQ_MASK0+(phy_id * 8)), tmp);

	
	msleep(100);

	/*detect port type*/
	scsi_write_dword((off+COMMON_PORT_VSR_ADDR0+(phy_id * 8)), VSR_PHY_STATUS);
	tmp = scsi_read_dword(off+COMMON_PORT_VSR_DATA0+(phy_id * 8));
	//printf("phy status R0Ch:%lx\n", tmp);

	//msleep(600);

	switch((u8)(((tmp&VSR_PHY_STATUS_MASK)>>16)&0xff))
	{
	case VSR_PHY_STATUS_SAS_RDY:
		core->phy[phy_id].type = PORT_TYPE_SAS;
		break;
	case VSR_PHY_STATUS_HR_RDY:
	default:
		core->phy[phy_id].type = PORT_TYPE_SATA;
		break;
	}
	//printf("core %d phy %d type:%x\n", core->id, phy_id, core->phy[phy_id].type);

	/*read devinfo and sas address info*/

	/*read phy status*/ 
	 tmp = scsi_read_dword(off+COMMON_PORT_PHY_CONTROL0+(phy_id * 4))+0x80000L;
	//printf("phy status 201d0h:%lx\n", tmp);
	core->phy[phy_id].status = tmp;
	if (tmp & SCTRL_PHY_READY_MASK) {
		if (core->phy[phy_id].type & PORT_TYPE_SAS) {
			scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_ATT_ID_FRAME5);
			tmp = scsi_read_dword(off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8));
			core->phy[phy_id].attach_info = (tmp&0xffL)<<24;
			scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_ATT_ID_FRAME0);
			tmp = scsi_read_dword(off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8));
			core->phy[phy_id].attach_info |= ((tmp&0x70L)>>4) + 
				((tmp&0x0f000000L)>>8) + 
				((tmp&0x0f0000L)>>8);

			scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_ATT_ID_FRAME4);
			tmp = scsi_read_dword(off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8));
			core->phy[phy_id].attach_sas_addr_high = cpu_to_be32(tmp);
			scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_ATT_ID_FRAME3);
			tmp = scsi_read_dword(off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8));
			core->phy[phy_id].attach_sas_addr_low = cpu_to_be32(tmp);
			printf("SAS Core %d PHY %d detect SAS Type attach_info:%x!\n", core->id, phy_id, core->phy[phy_id].attach_info);
		} else 
			printf("SAS Core %d PHY %d detect SATA Type!\n", core->id, phy_id);
		
		scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_PHY_CONTROL);
		scsi_write_dword((off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8)), 0x04);
	}
}

static void mv_port_config (struct sas_core *core) {
	ulong off = core->mmio_off;
	ulong tmp;
	u8 i;
	
	for (i=0; i<core->phy_num; i++) {
		struct port_info *port = NULL;

		if (0 == (core->phy[i].status & SCTRL_PHY_READY_MASK)) {
			continue; 
		}
		
		core->phy[i].wp_phymap |= (1<<i);
		
		port = core->phy[i].port;
		if (port == NULL) {
			/*new port*/
			u8 port_id = mv_host->port_num;
			if (port_id >= mv_host->max_port_num)
				return;
			mv_host->ports[port_id].id = port_id;
			mv_host->port_num++;
			port = &mv_host->ports[port_id];
			core->phy[i].port = port;
			port->type = core->phy[i].type;
			port->host = mv_host;
			port->core = core;
			port->dev_num = 0;
			port->phy_map = core->phy[i].wp_phymap;
		}

		scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(i * 8)), CONFIG_WIDE_PORT);
		tmp = (ulong)core->phy[i].wp_phymap;
		scsi_write_dword((off+COMMON_PORT_CONFIG_DATA0+(i * 8)), tmp);
		
	}
}

static int pm_issue_read_reg(struct dev_info *dev, u8 is_control, u8 reg_num){
	u8 fis[20];
    //printf("pm issue read reg ,reg num %d\n", reg_num);
	
	memset(fis, 0, 20);
	/* Construct the FIS */
	fis[0] = 0x27;		/* Host to device FIS. */
	//fis[1] = dev->pm_num | 0x80; /* cmd pm */
	fis[1] = 0x0F | 0x80;
	fis[2] = ATA_CMD_PM_READ;	/* Command byte. */
	fis[3] = reg_num;  /*features */
	fis[7] = is_control? 0x0f : dev->pm_num; /*device*/
		
	if (issue_sata_cmd_nodata(dev, (u8 *) &fis, 20, 0)) {
		debug("pm read reg failed.\n");
		return -EIO;
	}

	return 0;
}

static int pm_issue_write_reg(struct dev_info *dev, u8 is_control,
	                           u8 reg_num, ulong reg_value){
	u8 fis[20];
	//printf("pm issue write reg, reg num %d, reg value %x.\n", reg_num, reg_value);
	
	memset(fis, 0, 20);

	/* Construct the FIS */
	fis[0] = 0x27;		/* Host to device FIS. */
	fis[1] = 0x0F | 0x80; /* cmd pm */
	fis[2] = ATA_CMD_PM_WRITE;	/* Command byte. */
	fis[3] = reg_num;  /*features */
	fis[4] = (u8)((reg_value & 0xff00) >> 8);/*lba_low */
	fis[5] = (u8)((reg_value & 0xff0000) >> 16);/*lba_mid */
	fis[6] = (u8)((reg_value & 0xff000000) >> 24);/*lba_high */
	fis[7] = is_control? 0x0f : dev->pm_num; /*device*/
	fis[12] = (u8)(reg_value & 0xff);/*sector count */
			
	if (issue_sata_cmd_nodata(dev, (u8 *) &fis, 20, 0)) {
		debug("pm write reg failed.\n");
		return -EIO;
	}

	return 0;
}

static u8 sata_init_pm(struct sas_core * core, struct dev_info *dev){
	ulong tmp, try_cnt = 20;	
	ulong rx_fis = cpu_to_le32(mv_host->rx_fis & 0xffffffff);
	u32	unassoc_off = mv_host->unassociate_fis_off;
    u8 tmp_set = 0;
	ulong serror = 0, sstatus;
	//printf("sata init pm \n");

	memset((u32 *)rx_fis, 0, unassoc_off + 40 + 32);  //only one register set, 0x800+D2H FIS off + 4 Dword.

	//memset(mv_host->rx_fis, 0, (0x100 * core->phy_num) + 40 + 32);
	pm_issue_write_reg(dev, 0, PM_PSCR_SCONTROL, 0x01);
	msleep(5);
	pm_issue_write_reg(dev, 0, PM_PSCR_SCONTROL, 0x00);
	msleep(5);

	pm_issue_read_reg(dev, 0, PM_PSCR_SERROR);
	tmp = readl(rx_fis + SATA_RECEIVED_D2H_FIS(unassoc_off, tmp_set) + 0xC);
	serror = (u8)tmp;
	tmp = readl(rx_fis + SATA_RECEIVED_D2H_FIS(unassoc_off, tmp_set) + 0x4);
	serror |= tmp << 8;
	pm_issue_write_reg(dev, 0, PM_PSCR_SERROR, serror);

	do{ 
		pm_issue_read_reg(dev, 0, PM_PSCR_SSTATUS);
		tmp = readl(rx_fis + SATA_RECEIVED_D2H_FIS(unassoc_off, tmp_set) + 0xC);
		sstatus = (u8)tmp;
		tmp = readl(rx_fis + SATA_RECEIVED_D2H_FIS(unassoc_off, tmp_set) + 0x4);
		sstatus |= ((u8)tmp) <<8;
		msleep(10);
		try_cnt--;
	}while(((sstatus & 0xFFF) != 0x113) && ((sstatus & 0xFFF) != 0x123) && ((sstatus & 0xFFF) != 0x133) && try_cnt > 0);

   // printf("Line %d: sstatus %x\n", __LINE__, sstatus);
	pm_issue_read_reg(dev, 0, PM_PSCR_SERROR);
	tmp = readl(rx_fis + SATA_RECEIVED_D2H_FIS(unassoc_off, tmp_set) + 0xC);

	switch(sstatus & 0xfff)
	{
	case 0x113:
		dev->link_rate = 0x08;
		printf("Find SATA Dev %d Link Rate:1.5 Gbps!\n", dev->id);
		break;
	case 0x123:
		dev->link_rate = 0x09;
		printf("Find SATA Dev %d Link Rate:3.0 Gbps!\n", dev->id);
		break;
	case 0x133:
		dev->link_rate = 0x0A;
		printf("Find SATA Dev %d Link Rate:6.0 Gbps!\n", dev->id);
		break;
	default:
		//printf("Find SATA Dev %d, sstatus %x \n", dev->id, sstatus);		
		return 0;
	}

	return 1;
}

static void sata_pm_device_detect(struct sas_core * core, struct dev_info *dev){
	ulong tmp, off;
	struct port_info *port = dev->port;
	ulong rx_fis = cpu_to_le32(mv_host->rx_fis & 0xffffffff);
	u32 unassoc_off = mv_host->unassociate_fis_off;
    u8 tmp_set = 0, pm_num_ports, pm_feature_enable;
	u8 i, dev_id;	

	//printf("sata_pm_device_detect.rx_fis %x, core->phy_num %x\n", rx_fis, core->phy_num);
	
	off = core->mmio_off;
	tmp = scsi_read_dword(off + COMMON_CONTROL);
	scsi_write_dword(off + COMMON_CONTROL, tmp | CONTROL_FIS_RCV_EN | CONTROL_EN_SATA_RETRY);

	pm_issue_read_reg(dev, 1, PM_GSCR_INFO);
	tmp = readl(rx_fis + SATA_RECEIVED_D2H_FIS(unassoc_off, tmp_set) + 0xC);
    pm_num_ports = (u8)tmp;
	//printf("line %d: rx_fis 0x%x, pm_num_ports %x \n", __LINE__, rx_fis, pm_num_ports);

	if (pm_num_ports > MV_MAX_DEV_PER_PM)
		pm_num_ports = MV_MAX_DEV_PER_PM;

	/*Get PM device ID and vendor ID*/
	pm_issue_read_reg(dev, 1, PM_GSCR_ID);
	tmp = readl(rx_fis + SATA_RECEIVED_D2H_FIS(unassoc_off, tmp_set) + 0xC);
	port->pm_vendor = (u8)tmp;
	tmp = readl(rx_fis + SATA_RECEIVED_D2H_FIS(unassoc_off, tmp_set) + 0x4);
    port->pm_id = (u16)(tmp >> 8);
	port->pm_vendor |= ((u8)tmp) << 8;
	//printf("port->pm_id = %x, port->pm_vendor = %x \n", port->pm_id, port->pm_vendor);

    if ((port->pm_vendor == 0x11ab) && (port->pm_id == 0x4140))
		pm_issue_write_reg(dev, 1, 0x9B, 0xF0);

	/*set PM feature*/
	pm_issue_read_reg(dev, 1, PM_GSCR_FEATURES_ENABLE);
	tmp = readl(rx_fis + SATA_RECEIVED_D2H_FIS(unassoc_off, tmp_set) + 0xC);
	
    pm_feature_enable = (u8)(tmp | 0x00000008);
	pm_issue_write_reg(dev, 1, PM_GSCR_FEATURES_ENABLE, pm_feature_enable);

	pm_issue_read_reg(dev, 1, PM_GSCR_ERROR_ENABLE);
	tmp = readl(rx_fis + SATA_RECEIVED_D2H_FIS(unassoc_off, tmp_set) + 0xC);
	tmp = tmp | 0x00010000 | 0x04000000;
	pm_issue_write_reg(dev, 1, PM_GSCR_ERROR_ENABLE, tmp);

	//memset(dev, 0, sizeof(struct dev_info));
	dev->dev_type &= ~DEVICE_TYPE_SATA;
	dev->id = 0;
	dev->pm_num = 0;
    port->dev_num--;
	mv_host->dev_num--;
	
	for (i = 0; i < pm_num_ports; i++)
	{
	    dev_id = mv_host->dev_num;
		dev = &mv_host->devices[dev_id];
		dev->id = dev_id;
		dev->port = port;
		dev->pm_num = i;
		dev->dev_type |= DEVICE_TYPE_SATA;
		dev->register_set = 0xff;
		mv_host->dev_num++;		
		port->dev_num++; 

		if (!sata_init_pm(core, dev))
		{
		   // if ((dev->link_rate != 0x08)
			//	&& (dev->link_rate != 0x09)
			//	&& (dev->link_rate != 0x0A))
				port->dev_num--;

			//memset(dev, 0, sizeof(struct dev_info));	
			dev->dev_type &= ~DEVICE_TYPE_SATA;
			dev->id = 0;
	        dev->pm_num = 0;
			mv_host->dev_num--;
			continue;
		}

		printf("dev_id %x, Find Device on PM Port-> %x \n",dev_id, dev->pm_num);
	}  
}

static void mv_sata_port_reset(struct port_info *port) {
	ulong tmp, off;
	struct sas_core *core;
	struct dev_info *dev = NULL;
	u8 phy_id;
	u8 dev_id;
	if (mv_host->dev_num >= MV_MAX_DEVICES)
		return;

	//printf("enter mv_sata_port_reset!!\n");
	if (port == NULL) {
		return;
	}
	
	port->dev_num = 0;

	/*sata port detect*/
	/*only support narrow port, use port->phy_map as phy id*/
	core = (struct sas_core *)port->core;
	off = core->mmio_off;

	phy_id = 0;
	while (0 == (port->phy_map & (1 <<phy_id)))
			phy_id++;
	//printf("reset sata port, core:%p off:%lx phy id:%d\n", core, off, phy_id);
	
	tmp = scsi_read_dword(off+COMMON_PORT_PHY_CONTROL0+(phy_id * 4));
	if ((tmp & SCTRL_PHY_READY_MASK) != SCTRL_PHY_READY_MASK)
		printf("detect sata port %d failed phy status(0x01d0h):%lx\n", phy_id, tmp);

	/*assign device*/
	{
		dev_id = mv_host->dev_num;
		dev = &mv_host->devices[dev_id];
		dev->id = dev_id;
		dev->port = port;
		dev->pm_num = 0;
		dev->register_set = 0xff;
		mv_host->dev_num++;
		
		port->dev_num++;
	}

	/*sata port detect*/
#if 1
	/*if support PM, need issue softreset first*/
	ata_issue_softreset(dev, 1, 1);
	msleep(5);
	ata_issue_softreset(dev, 0, 1);
	msleep(50);
#endif
	
	/*check device ready*/
	{
		u32 loop=1000;
		//u8	phy_id = port->phy_map;
		u32	sig = 0x0;
		while (loop > 0) {
			sig = 0;
			scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_SATA_SIG3);
			tmp = scsi_read_dword(off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8)) & 0xFF;
			sig |= tmp;
			scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_SATA_SIG1);
			tmp = scsi_read_dword(off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8)) & 0xFFFFFF;
			sig |= (tmp << 8);
			scsi_write_dword((off+COMMON_PORT_CONFIG_ADDR0+(phy_id * 8)), CONFIG_SATA_SIG0);
			tmp = scsi_read_dword(off+COMMON_PORT_CONFIG_DATA0+(phy_id * 8)) & 0xFF;
			if (!(((tmp>>16) & 0xFF) & 0x80)) {
				switch(sig)
				{
					case 0x96690101:	/*PM signature*/
						port->type = PORT_TYPE_SATA | PORT_TYPE_PM;
						if ((u8)(tmp >> 16) == 0x50 )
							goto end;
						break;
					case 0x00000101: 	/*ATA signature*/
						//printf("find sata signature!!\n");
						dev->dev_type |= DEVICE_TYPE_SATA;
						if((u8)((tmp >> 16)& 0xc0) == 0x40 )
							goto end;
						break;
					default:
						break;
				}
				break;

			} else 
				printf("signature busy!!!\n");
			msleep(1);
			loop--;
		}
end:
		if (loop == 0) {
			printf("device not ready!!!\n");
			return;
		}

		if (port->type & PORT_TYPE_PM) {
			/*device detection behind PM*/
			dev->pm_num = 0xF;
			sata_pm_device_detect(core, dev);
		} else {
			/*sata device ready*/
			
			if ((core->phy[phy_id].status & SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK) >>
				SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK_OFFSET < 0x0A) 
				dev->link_rate = (core->phy[phy_id].status & SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK) >>
				SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK_OFFSET;
			else
				dev->link_rate = 0x0A;

			switch (dev->link_rate) {
			case 0x08:
				printf("Find SATA Dev %d Link Rate:1.5 Gbps!\n", dev->id);
				break;
			case 0x09:
				printf("Find SATA Dev %d Link Rate:3.0 Gbps!\n", dev->id);
				break;
			case 0x0A:
			default:
				printf("Find SATA Dev %d Link Rate:6.0 Gbps!\n", dev->id);
				break;			
			}
		}
		return;
	}
}

static void mv_sas_port_reset(struct port_info *port) {
	//ulong off;
	struct sas_core *core;
	struct dev_info *dev = NULL;
	u8 phy_id;
	u8 dev_id;
	if (mv_host->dev_num >= MV_MAX_DEVICES)
		return;

	//printf("enter mv_sata_port_reset!!\n");
	if (port == NULL) {
		return;
	}
	
	port->dev_num = 0;

	/*sata port detect*/
	/*only support narrow port, use port->phy_map as phy id*/
	core = (struct sas_core *)port->core;
	//off = core->mmio_off;

	phy_id = 0;
	while (0 == (port->phy_map & (1 <<phy_id)))
			phy_id++;
	//printf("reset sata port, core:%p off:%lx phy id:%d\n", core, off, phy_id);

	if (core->phy[phy_id].attach_info & (1<<19)){
	/*assign device*/
		dev_id = mv_host->dev_num;
		dev = &mv_host->devices[dev_id];
		dev->id = dev_id;
		dev->port = port;
		dev->pm_num = 0;
		dev->register_set = 0xff;
		
		dev->dev_type = DEVICE_TYPE_SSP;
		dev->sas_addr_low = core->phy[phy_id].attach_sas_addr_low;
		dev->sas_addr_high = core->phy[phy_id].attach_sas_addr_high;
		if ((core->phy[phy_id].status & SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK) >>
			SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK_OFFSET < 0x0A) 
			dev->link_rate = (core->phy[phy_id].status & SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK) >>
			SCTRL_NEG_SPP_PHYS_LINK_RATE_MASK_OFFSET;
		else
			dev->link_rate = 0x0A;
		mv_host->dev_num++;
		port->dev_num++;
			
		switch (dev->link_rate) {
		case 0x08:
			printf("Find SAS Dev %d Link Rate:1.5 Gbps!\n", dev->id);
			break;
		case 0x09:
			printf("Find SAS Dev %d Link Rate:3.0 Gbps!\n", dev->id);
			break;
		case 0x0A:
		default:
			printf("Find SAS Dev %d Link Rate:6.0 Gbps!\n", dev->id);
			break;			
		}		
	} else 
		printf("detect sas phy %d failed attach info:%x\n", phy_id, core->phy[phy_id].attach_info);

	return;	
}

static void mv_port_start (struct host_info *host) {
	u8 i;
	struct port_info *port;

	for (i=0; i<host->port_num; i++) {
		port = &mv_host->ports[i];
		if (port->type == PORT_TYPE_SAS) {
			/*reset sas port*/
			//to be done 
			mv_sas_port_reset(port);
		} else
			mv_sata_port_reset(port);
			/*reset sata port*/
	}
}

static void scsi_chip_init(void)
{
	ulong tmp, off;
	u8 i, j;
	u8 k;

	for (i=0; i<mv_host->core_num; i++) {
		off = mv_host->core[i].mmio_off;
		tmp = scsi_read_dword(off+COMMON_CONFIG);
		//printf("common config:%lx core %d off:%lx\n", tmp, i, off);
		tmp |= CONFIG_SAS_SATA_RST;
		scsi_write_dword(off, tmp);
		msleep(100);

		scsi_write_dword((off+COMMON_PORT_ALL_VSR_ADDR), VSR_PHY_CONFIG);
		tmp = scsi_read_dword(off+COMMON_PORT_ALL_VSR_DATA);
		//printf("phy config:%lx\n", tmp);

		scsi_write_dword((off+COMMON_PORT_ALL_VSR_ADDR), VSR_PHY_CONFIG);
		scsi_write_dword((off+COMMON_PORT_ALL_VSR_DATA), 0x0084fffeL);

#if 0
		/*bios*/
		scsi_write_dword((off+COMMON_PORT_ALL_VSR_ADDR), 0x144);
		scsi_write_dword((off+COMMON_PORT_ALL_VSR_DATA), 0x08001006L);
		scsi_write_dword((off+COMMON_PORT_ALL_VSR_ADDR), 0x1b4);
		scsi_write_dword((off+COMMON_PORT_ALL_VSR_DATA), 0x0000705fL);
#endif
		
		scsi_write_dword((off+COMMON_CONTROL), 0);

		scsi_write_dword((off+COMMON_CMD_ADDR), 0x128);
		tmp = scsi_read_dword(off+COMMON_CMD_DATA);
		tmp &=~ 0xffff;
		tmp |= 0x7f7f;
		scsi_write_dword((off+COMMON_CMD_DATA), tmp);
		
		scsi_write_dword((off+COMMON_CMD_ADDR), 0x124);
		tmp = scsi_read_dword(off+COMMON_CMD_DATA);
		tmp &= ~0xffffL;
		tmp |= 0x3fffL;
		scsi_write_dword((off+COMMON_CMD_DATA), tmp);
		
		scsi_write_dword((off+COMMON_CMD_ADDR), 0x134);
		tmp = scsi_read_dword(off+COMMON_CMD_DATA);
		tmp &=0xFFFF00FF;
		tmp |=0x00028200;
		scsi_write_dword((off+COMMON_CMD_DATA), tmp);
		
		scsi_write_dword((off+COMMON_CMD_ADDR), 0x138);
		scsi_write_dword((off+COMMON_CMD_DATA), 0x003f003fL);
		
		scsi_write_dword((off+COMMON_CMD_ADDR), 0x13c);
		scsi_write_dword((off+COMMON_CMD_DATA), 0x7a0000L);
		
		scsi_write_dword((off+COMMON_CMD_ADDR), 0x1a4);
		scsi_write_dword((off+COMMON_CMD_DATA), 0xffefbf7dL);

#if 0
		/*bios*/
		scsi_write_dword((off+COMMON_CMD_ADDR), 0x1b8);
		tmp = scsi_read_dword(off+COMMON_CMD_DATA);
		tmp &=0x0000ffffL;
		tmp |=0x00fa0000L;
		scsi_write_dword((off+COMMON_CMD_DATA), tmp);
		
#endif

#if 1
		/*set phy ffe*/
		for (j=0; j<MV_MAX_CORE_PHY; j++) {
			scsi_write_phy_vsr_addr(off, j, 0x10c);
			tmp = scsi_read_vsr_data(off, j);
			tmp &= 0xFFFFFF80L;
			tmp |= 0x7C;
			scsi_write_phy_vsr_data(off, j, tmp);
		}
		
		/*set phy tuning*/
		for (j=0; j<MV_MAX_CORE_PHY; j++) {
			ulong value1, value2;				
			for (k=0; k<3; k++) {
				switch (k) {
				case 0:
					value1 = 0x118;
					value2 = 0x11C;
					break;
				case 1:
					value1 = 0x11C;
					value2 = 0x120;
					break;
				case 2:
					value1 = 0x120;
					value2 = 0x124;
					break;
				}
				
				scsi_write_phy_vsr_addr(off, j, value1);
				tmp = scsi_read_vsr_data(off, j);
				tmp &= ~(0xFBE << 16);
				tmp |= (((0x1 << 11) | (0x1A << 7) | (0x6 << 1)) << 16);
				scsi_write_phy_vsr_data(off, j, tmp);
				
				scsi_write_phy_vsr_addr(off, j, value2);
				tmp = scsi_read_vsr_data(off, j);
				tmp &= ~(0xC000);
				tmp |= (0x3 << 14);
				scsi_write_phy_vsr_data(off, j, tmp);
			}
		}
			/*set phy rate*/
		for (j=0; j<MV_MAX_CORE_PHY; j++) {
			scsi_write_phy_vsr_addr(off, j, VSR_PHY_CONFIG);
			tmp = scsi_read_vsr_data(off, j);
			//printf("phy rate:%lx\n", tmp);
			tmp &= 0x80000FL;
			tmp |= 0x0004fffeL; //6G  //0x0004783eL; // 3G
			scsi_write_phy_vsr_addr(off, j, VSR_PHY_CONFIG);
			scsi_write_phy_vsr_data(off, j, tmp);	
		}
#endif
		//msleep(500); /*wait phy ready*/
			
		/*spin up device*/
		//To be done
		for (j=0; j<MV_MAX_CORE_PHY; j++) {
			mv_phy_config(&mv_host->core[i], j);			
		}		

		/*reset CMD queue*/
		tmp = scsi_read_dword(off+COMMON_CONTROL);
		tmp |= CONTROL_RESET_CMD_ISSUE;
		scsi_write_dword((off+COMMON_CONTROL), tmp);
		
		tmp = scsi_read_dword(off+COMMON_CONFIG);
		tmp |= CONFIG_CMD_TBL_BE;
		tmp |= CONFIG_DATA_BE;
		tmp &= ~CONFIG_OPEN_ADDR_BE;
		tmp |= CONFIG_RSPNS_FRAME_BE;
		scsi_write_dword((off+COMMON_CONFIG), tmp);
#if 0/*bios*/
		tmp = scsi_read_dword(off+COMMON_CONFIG);
		tmp = scsi_read_dword(off+COMMON_CONFIG);
#endif	
		tmp = CONTROL_EN_CMD_ISSUE | CONTROL_RSPNS_RCV_EN;
		scsi_write_dword((off+COMMON_CONTROL), tmp);

		tmp = cpu_to_le32(mv_host->cmd_list & 0xffffffff);
		scsi_write_dword((off+COMMON_LST_ADDR), tmp);
		scsi_write_dword((off+COMMON_LST_ADDR_HI), 0);
		
		tmp = cpu_to_le32(mv_host->rx_fis & 0xffffffff);
		scsi_write_dword((off+COMMON_FIS_ADDR), tmp);
		scsi_write_dword((off+COMMON_FIS_ADDR_HI), 0);

		tmp = 0;
		scsi_write_dword((off+COMMON_DELV_Q_CONFIG), tmp);
		tmp = DELV_QUEUE_SIZE_MASK & MAX_DELV_QUEUE_SIZE;
		tmp |= DELV_QUEUE_ENABLE;
		scsi_write_dword((off+COMMON_DELV_Q_CONFIG), tmp);
		tmp = cpu_to_le32(mv_host->dlv_q & 0xffffffff);
		scsi_write_dword((off+COMMON_DELV_Q_ADDR), tmp);
		scsi_write_dword((off+COMMON_DELV_Q_ADDR_HI), 0);
		
		tmp = 0;
		scsi_write_dword((off+COMMON_CMPL_Q_CONFIG), tmp);
		tmp = CMPL_QUEUE_SIZE_MASK & MAX_CMPL_QUEUE_SIZE;
		tmp |= CMPL_QUEUE_ENABLE;
		scsi_write_dword((off+COMMON_CMPL_Q_CONFIG), tmp);
		tmp = cpu_to_le32(mv_host->cmpl_q & 0xffffffff);
		scsi_write_dword((off+COMMON_CMPL_Q_ADDR), tmp);
		scsi_write_dword((off+COMMON_CMPL_Q_ADDR_HI), 0);
		
		tmp = 0;
		tmp =  INT_COAL_COUNT_MASK & MAX_DELV_QUEUE_SIZE; //one slot support
		tmp |= INT_COAL_ENABLE;
		scsi_write_dword((off+COMMON_COAL_CONFIG), tmp);
		//tmp = 0x10400;	//1ms
		tmp = COAL_TIMER_MASK;
		scsi_write_dword((off+COMMON_COAL_TIMEOUT), tmp);
		
		/* enable CMD/CMPL_Q/RESP mode */
		tmp = scsi_read_dword(off+COMMON_CONTROL);
		tmp |= CONTROL_EN_CMD_ISSUE; /* for performance */
		tmp |= CONTROL_FIS_RCV_EN;
		scsi_write_dword((off+COMMON_CONTROL), tmp);
		
		/* enable completion queue interrupt */
		tmp = (INT_PORT_MASK | INT_CMD_CMPL | INT_PHY_MASK);
		scsi_write_dword((off+COMMON_IRQ_MASK), tmp);

		mv_port_config(&mv_host->core[i]);		
	}

	//msleep(9500);

	mv_port_start(mv_host);
	
}

static int mv_host_data_init(void)
{
	u8 i;
	u16 device;
	struct sas_core *core = NULL; 
	struct port_info *port = NULL;
	struct dev_info *dev = NULL;
	
	pci_read_config_word(mv_host->dev, PCI_DEVICE_ID, &device);
	switch (device) {
	case 0x9445:
		mv_host->core_num = 1;
		mv_host->max_port_num = 4;
		break;
	case 0x9485:
		mv_host->core_num = 2;
		mv_host->max_port_num = 8;
		break;		
	default:
		printf("unsupported device ID %x\n", device);
		return -1;		
	}

	for (i=0; i<mv_host->core_num; i++) {
		core = &mv_host->core[i];
		core->host = mv_host;
		core->mmio_off = SAS_REG_BASE + 0x4000*i;
		core->lst_dlvq = 0xfff;
		core->lst_cmplq = 0xfff;
		core->id = i;
		core->start_phy_id = MV_MAX_CORE_PHY * i;
		core->phy_num = MV_MAX_CORE_PHY;
		printf("PCI Bar Address:0x%lx SAS Core %d Register Base:0x%lx!\n", mv_host->mmio, core->id, core->mmio_off);
	}
	
	for (i=0; i<mv_host->max_port_num; i++) {
		port = &mv_host->ports[i];
		port->id = i;
		port->host = mv_host;
		port->core = (i < MV_MAX_CORE_PHY) ? (&mv_host->core[0]) : (&mv_host->core[1]);
		port->type = PORT_TYPE_SAS;
	}

	for (i=0; i<MV_MAX_DEVICES; i++) {
		dev = &mv_host->devices[i];
		dev->register_set = 0xff;
	}
	
	mv_host->unassociate_fis_off = 0x800;

	return 0;	
}

static int mv_host_init(int busdevfunc)
{
	u32 mem;
		
	/*init memory*/
	mv_host = malloc(sizeof(struct host_info));
	if (!mv_host) {
		printf("No mem for host!\n");
		return -ENOMEM;
	}
	memset(mv_host, 0, sizeof(struct host_info));

	mv_host->dev = busdevfunc;
	mv_host->mmio = scsi_mem_addr;	

	if (mv_host_data_init())
		return -1;
	
	mem = (u32) malloc(CORE_BUFFLEN);
	if (!mem) {
		free(mv_host);
		printf("No mem for DMA!\n");
		return -ENOMEM;
	}
	memset((u8 *) mem, 0, CORE_BUFFLEN);

	mem = (mem + 0x40 - 1) & (~0x3f); /*align to 64bytes*/
	mv_host->cmd_list = mem;
	mem += CMD_LIST_POOL_SIZE;

	mem = (mem + 0x100 - 1) & (~0xff); /*align to 256bytes*/
	mv_host->rx_fis = mem;
	mem += RX_FIS_POOL_SIZE;
	
	mem = (mem + 0x80 - 1) & (~0x7f); /*align to 128bytes*/
	mv_host->cmd_table = mem;
	//mv_host->cmd_table_dma = 
	mem += CMD_TABLE_POOL_SIZE;
	
	mem = (mem + 0x40 - 1) & (~0x3f); /*align to 64bytes*/
	mv_host->dlv_q = mem;
	mem += DELV_Q_POOL_SIZE;
	
	mem = (mem + 0x40 - 1) & (~0x3f); /*align to 64bytes*/
	mv_host->cmpl_q = mem;
	mem += CMPL_Q_POOL_SIZE;		

	return 0;
}

static int mv_host_reinit(void)
{
	struct host_info tmp_host;

	memcpy(&tmp_host, mv_host, sizeof(struct host_info));
	memset(mv_host, 0, sizeof(struct host_info));

	mv_host->dev = tmp_host.dev;
	mv_host->mmio = scsi_mem_addr;	
	mv_host->cmd_list = tmp_host.cmd_list;
	mv_host->rx_fis = tmp_host.rx_fis;
	mv_host->cmd_table = tmp_host.cmd_table;
	mv_host->dlv_q = tmp_host.dlv_q;
	mv_host->cmpl_q = tmp_host.cmpl_q;

	if (mv_host_data_init())
		return -1;

	return 0;
}

void scsi_low_level_init(int busdevfunc)
{
#if 1
	u32 cmd;
	ulong addr;

	//pci_read_config_byte(busdevfunc, PCI_INTERRUPT_LINE, &vec);
	addr = (ulong)pci_map_bar(busdevfunc,
			PCI_BASE_ADDRESS_2, PCI_REGION_MEM);
	scsi_mem_addr = addr;

	if (mv_host_init(busdevfunc)) {
		printf("init scsi host failed!!!\n");
		return;
	}
	/*
	 * Enable bus mastering in case this has not been done, yet.
	 */
	pci_read_config_dword(busdevfunc, PCI_COMMAND, &cmd);
	cmd |= PCI_COMMAND_MASTER | PCI_COMMAND_IO | PCI_COMMAND_MEMORY;
	pci_write_config_dword(busdevfunc, PCI_COMMAND, cmd);

	pci_read_config_dword(busdevfunc, 0x78, &cmd);
	//printf("PCI dev ctl 0x%x, it should be smaller than 0x2000\n", (cmd & 0x00007000));

	scsi_chip_init();

#endif
}

void scsi_bus_reset(void)
{
	/*Not implement*/
	mv_host_reinit();
	scsi_chip_init();
}

void scsi_print_error(ccb * pccb)
{
	//printf("scsi_print_error: command 0x%02x error\n", pccb->cmd[0]);
}

