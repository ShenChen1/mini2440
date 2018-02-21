/*
 * vivi/s3c2410/nand_read.c: Simple NAND read functions for booting from NAND
 *
 * Copyright (C) 2002 MIZI Research, Inc.
 *
 * Author: Hwang, Chideok <hwang@mizi.com>
 * Date  : $Date: 2004/02/04 06:22:24 $
 *
 * $Revision: 1.1.1.1 $
 * $Id: param.c,v 1.9 2002/07/11 06:17:20 nandy Exp
 *
 * History
 *
 * 2002-05-xx: Hwang, Chideok <hwang@mizi.com>
 *
 * 2002-05-xx: Chan Gyun Jeong <cgjeong@mizi.com>
 *
 * 2002-08-10: Yong-iL Joh <tolkien@mizi.com>
 *
 */
#include <common.h>

/* NAND FLASH控制器 */
#define NFCONF	(*((volatile unsigned long *)0x4E000000))
#define NFCONT	(*((volatile unsigned long *)0x4E000004))
#define NFCMMD	(*((volatile unsigned char *)0x4E000008))
#define NFADDR	(*((volatile unsigned char *)0x4E00000C))
#define NFDATA	(*((volatile unsigned char *)0x4E000010))
#define NFSTAT	(*((volatile unsigned char *)0x4E000020))

/* GPIO */
#define GPHCON		(*(volatile unsigned long*)0x56000070)
#define GPHUP		(*(volatile unsigned long*)0x56000078)

/* UART registers*/
#define ULCON0		(*(volatile unsigned long*)0x50000000)
#define UCON0		(*(volatile unsigned long*)0x50000004)
#define UFCON0		(*(volatile unsigned long*)0x50000008)
#define UMCON0		(*(volatile unsigned long*)0x5000000c)
#define UTRSTAT0	(*(volatile unsigned long*)0x50000010)
#define UTXH0		(*(volatile unsigned char*)0x50000020)
#define URXH0		(*(volatile unsigned char*)0x50000024)
#define UBRDIV0		(*(volatile unsigned long *)0x50000028)

#define TXD0READY	(1<<2)

void nand_init_ll(void)
{
#define TACLS   0
#define TWRPH0  1
#define TWRPH1  0

		/* 设置时序 */
		NFCONF =(TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);

		/* 使能NAND Flash控制器,初始化ECC,禁止片选 */
		NFCONT =(1<<4)|(1<<1)|(1<<0);
}

static void nand_select(void)
{
		NFCONT &= ~(1<<1);
}

static void nand_deselect_ll(void)
{
		NFCONT |= (1<<1);
}

static void nand_cmd(unsigned char cmd)
{
		volatile int i;
		NFCMMD = cmd;

		for (i = 0; i < 10; i++);
}

static void nand_addr(unsigned int addr)
{
		unsigned int col  = addr % 2048;
		unsigned int page = addr / 2048;
		volatile int i;

		NFADDR = col & 0xff;
		for (i = 0; i < 10; i++);

		NFADDR = (col >> 8) & 0xff;
		for (i = 0; i < 10; i++);

		NFADDR = page & 0xff;
		for (i = 0; i < 10; i++);

		NFADDR = (page >> 8) & 0xff;
		for (i = 0; i < 10; i++);

		NFADDR = (page >> 16) & 0xff;
		for (i = 0; i < 10; i++);
}

static void nand_wait_ready_ll(void)
{
		while (!(NFSTAT & 1));
}

static unsigned char nand_data(void)
{
		return NFDATA;
}

/*
*src=0;     dest=0x33f80000;len=0x73250
*/
void nand_read_ll(unsigned int addr, unsigned char *buf, unsigned int len)
{
		int col = addr % 2048;
		int i = 0;

		/* 1. 选中 */
		nand_select();

		while (i < len)
		{
				/* 2. 发出读命令00h */
				nand_cmd(0x00);

				/* 3. 发出地址(分5步发出) */
				nand_addr(addr);

				/* 4. 发出读命令30h */
				nand_cmd(0x30);

				/* 5. 判断状态 */
				nand_wait_ready_ll();

				/* 6. 读数据 */
				for (; (col < 2048)&& (i < len); col++)
				{
							buf[i] =nand_data();
							i++;
							addr++;
				}
				col = 0;
		}
		/* 7. 取消选中 */
		nand_deselect_ll();
}

int nand_spl_load_image(uint32_t offs, unsigned int size, void *dst)
{
	nand_init_ll();
	nand_read_ll(offs, dst, size);

	return size;
}

