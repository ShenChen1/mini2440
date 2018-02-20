/*
 * (C) Copyright 2006 OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

#include <nand.h>
#include <asm/arch/s3c24x0_cpu.h>
#include <asm/io.h>

#define S3C2440_NFCONT_EN			(1<<0)
#define S3C2440_NFCONT_nFCE			(1<<1)
#define S3C2440_NFCONT_INITECC		(1<<4)
#define S3C2440_NFCONT_MAINECCLOCK	(1<<5)
#define S3C2440_NFCONT_SPAREECCLOCK	(1<<6)
#define S3C2440_NFCONF_TACLS(x)    ((x)<<12)
#define S3C2440_NFCONF_TWRPH0(x)   ((x)<<8)
#define S3C2440_NFCONF_TWRPH1(x)   ((x)<<4)

#define S3C2440_ADDR_NCLE 0x08
#define S3C2440_ADDR_NALE 0x0c
#define S3C2440_ADDR_NDLE 0x10

#ifdef CONFIG_NAND_SPL

/* in the early stage of NAND flash booting, printf() is not available */
#define printf(fmt, args...)

#ifndef __UBOOT__
static void nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
#else
void nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
#endif
{
	int i;
	struct nand_chip *this = mtd->priv;

	for (i = 0; i < len; i++)
		buf[i] = readb(this->IO_ADDR_R);
}
#endif

static void s3c2440_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	struct s3c2440_nand *nand = s3c2440_get_base_nand();

	debug("hwcontrol(): 0x%02x 0x%02x\n", cmd, ctrl);

	if (ctrl & NAND_CTRL_CHANGE) {
		ulong IO_ADDR_W = (ulong)nand;

		if (ctrl & NAND_CLE)
			IO_ADDR_W |= S3C2440_ADDR_NCLE;
		else if (ctrl & NAND_ALE)
			IO_ADDR_W |= S3C2440_ADDR_NALE;
		else
			IO_ADDR_W |= S3C2440_ADDR_NDLE;

		chip->IO_ADDR_W = (void *)IO_ADDR_W;

		if (ctrl & NAND_NCE)
			writel(readl(&nand->nfcont) & ~S3C2440_NFCONT_nFCE,
				&nand->nfcont);
		else
			writel(readl(&nand->nfcont) | S3C2440_NFCONT_nFCE,
				&nand->nfcont);
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, chip->IO_ADDR_W);
}

static int s3c2440_dev_ready(struct mtd_info *mtd)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	debug("dev_ready\n");
	return readl(&nand->nfstat) & 0x01;
}

#ifdef CONFIG_S3C2440_NAND_HWECC
void s3c2440_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	debug("s3c2440_nand_enable_hwecc(%p, %d)\n", mtd, mode);
/*
	1. In software mode, ECC module generates ECC parity code for all read / 
	write data. So you have to reset ECC value by writing the InitECC(NFCONT[4]) 
	bit as ¡®1¡¯ and have to clear theMainECCLock(NFCONT[5]) bit to ¡®0¡¯(Unlock) 
	before read or write data.MainECCLock(NFCONT[5]) and SpareECCLock(NFCONT[6]) 
	control whether ECC Parity code is generated or not.
*/
	writel(readl(&nand->nfcont) | (S3C2440_NFCONT_INITECC 
		& (~S3C2440_NFCONT_MAINECCLOCK)), &nand->nfcont);
}

static int s3c2440_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat,
					u_char *ecc_code)
{
	ulong ecc;
	struct s3c2440_nand *nand = s3c2440_get_base_nand();	
/*
	2. Whenever data is read or written, the ECC module generates ECC 
	parity code on register NFMECC0/1
*/

/*
	3. After you completely read or write one page (not include spare 
	area data), Set the MainECCLock bit to ¡®1¡¯(Lock).ECC Parity code 
	is locked and the value of the ECC status register will not be changed.
*/
	writel(readl(&nand->nfcont) | S3C2440_NFCONT_MAINECCLOCK, &nand->nfcont);

	ecc = readl(&nand->nfeccd0);
	ecc_code[0] = ecc & 0xff;
	ecc_code[1] = (ecc >> 8) & 0xff;
	ecc_code[2] = (ecc >> 16) & 0xff;
	ecc_code[3] = (ecc >> 24) & 0xff;
	debug("s3c2440_nand_calculate_hwecc(%p,): 0x%02x 0x%02x 0x%02x 0x%02x\n",
		mtd , ecc_code[0], ecc_code[1], ecc_code[2], ecc_code[3]);

	return 0;
}

static int s3c2440_nand_correct_data(struct mtd_info *mtd, u_char *dat,
					u_char *read_ecc, u_char *calc_ecc)
{
	if (read_ecc[0] == calc_ecc[0] &&
		read_ecc[1] == calc_ecc[1] &&
		read_ecc[2] == calc_ecc[2] &&
		read_ecc[3] == calc_ecc[3])
		return 0;

#if 1
	struct s3c2440_nand *nand = s3c2440_get_base_nand();	

	ulong meccdata0= (read_ecc[1] << 16) | read_ecc[0];
	ulong meccdata1= (read_ecc[3] << 16) | read_ecc[2];

	writel(meccdata0, &nand->nfeccd0);
	writel(meccdata1, &nand->nfeccd1);

	/*Read ecc status */
	ulong estat0 = readl(&nand->nfstat0);  
	switch(estat0 & 0x3) {
		case 0: /* No error */
			return 0;
		case 1:
		{
			/*
				1 bit error (Correctable)
				(nfstat0 >> 7) & 0x7ff    :error byte number
				(nfstat0 >> 4) & 0x7      :error bit number
			*/
			ulong err_byte_num = (estat0 >> 7) & 0x7ff;
			ulong err_bit_num = (estat0 >> 4) & 0x7;

			uchar repaired = dat[err_byte_num] ^ (1 << err_bit_num);

			printf("S3C NAND: 1 bit error detected at %ld byte.\n", err_byte_num);
			printf("\tCorrecting from 0x%02x to 0x%02x...OK\n", dat[err_byte_num], repaired);

			dat[err_byte_num] = repaired;

			return 1;
		}
		case 2: /* Multiple error */
		case 3: /* ECC area error */
			printf("S3C NAND: ECC uncorrectable errordetected. Not correctable.\n");
			return -1;
	}
#endif

	return 0;
}
#endif

int board_nand_init(struct nand_chip *nand)
{
	u_int32_t cfg;
	u_int8_t tacls, twrph0, twrph1;
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	struct s3c2440_nand *nand_reg = s3c2440_get_base_nand();

	debug("board_nand_init()\n");

	writel(readl(&clk_power->clkcon) | (1 << 4), &clk_power->clkcon);

	/* initialize hardware */
#if defined(CONFIG_S3C24XX_CUSTOM_NAND_TIMING)
	tacls  = CONFIG_S3C24XX_TACLS;
	twrph0 = CONFIG_S3C24XX_TWRPH0;
	twrph1 =  CONFIG_S3C24XX_TWRPH1;
#else
	tacls = 4;
	twrph0 = 8;
	twrph1 = 8;
#endif

	cfg = 0;
	cfg |= S3C2440_NFCONF_TACLS(tacls - 1);
	cfg |= S3C2440_NFCONF_TWRPH0(twrph0 - 1);
	cfg |= S3C2440_NFCONF_TWRPH1(twrph1 - 1);
	writel(cfg, &nand_reg->nfconf);

	cfg = 0;
	cfg |= S3C2440_NFCONT_EN;
	cfg |= S3C2440_NFCONT_INITECC;
	cfg |= S3C2440_NFCONT_nFCE;
	writel(cfg, &nand_reg->nfcont);

	/* initialize nand_chip data structure */
	nand->IO_ADDR_R = (void *)&nand_reg->nfdata;
	nand->IO_ADDR_W = (void *)&nand_reg->nfdata;

	nand->select_chip = NULL;

	/* read_buf and write_buf are default */
	/* read_byte and write_byte are default */
#ifdef CONFIG_NAND_SPL
	nand->read_buf = nand_read_buf;
#endif

	/* hwcontrol always must be implemented */
	nand->cmd_ctrl = s3c2440_hwcontrol;

	nand->dev_ready = s3c2440_dev_ready;

#ifdef CONFIG_S3C2440_NAND_HWECC
	nand->ecc.hwctl = s3c2440_nand_enable_hwecc;
	nand->ecc.calculate = s3c2440_nand_calculate_ecc;
	nand->ecc.correct = s3c2440_nand_correct_data;
	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.size = CONFIG_SYS_NAND_ECCSIZE;
	nand->ecc.bytes = CONFIG_SYS_NAND_ECCBYTES;
	nand->ecc.strength = 1;
#else
	nand->ecc.mode = NAND_ECC_SOFT;
#endif

#ifdef CONFIG_NAND_SPL
	nand->ecc.mode = NAND_ECC_NONE;
#endif

#ifdef CONFIG_S3C2440_NAND_BBT
	nand->bbt_options |= NAND_BBT_USE_FLASH;
#endif

	debug("end of nand_init\n");

	return 0;
}
