/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <garyj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Configuation settings for the SAMSUNG SMDK2410 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARM920T		/* This is an ARM920T Core */
#define CONFIG_S3C24X0		/* in a SAMSUNG S3C24x0-type SoC */
#define CONFIG_S3C2440		/* specifically a SAMSUNG S3C2440 SoC */
#define CONFIG_MINI2440		/* on a MINI2440 Board */

#define CONFIG_SYS_TEXT_BASE	0x33E00000

#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH

/* input clock of PLL (the MINI2440 has 12MHz input clock) */
#define CONFIG_SYS_CLK_FREQ		12000000

#define CONFIG_CMDLINE_TAG	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/*
 * Hardware drivers
 */
/* Ethernet */
#define CONFIG_DRIVER_DM9000
#define CONFIG_DM9000_BASE		0x20000300
#define DM9000_IO				CONFIG_DM9000_BASE
#define DM9000_DATA				(CONFIG_DM9000_BASE + 4)
#define CONFIG_DM9000_USE_16BIT
#define CONFIG_DM9000_NO_SROM
#define CONFIG_NET_RETRY_COUNT	20
/*
 * select serial console configuration
 */
#define CONFIG_S3C24X0_SERIAL
#define CONFIG_SERIAL1			1	/* we use SERIAL 1 on MINI2440 */
#define CONFIG_BAUDRATE			115200
#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200	/* speed to run kgdb serial port */
#endif

/************************************************************
 * USB support (currently only works with D-cache off)
 ************************************************************/
#define CONFIG_USB_OHCI
#define CONFIG_USB_OHCI_S3C24XX
#define CONFIG_USB_STORAGE
#define CONFIG_DOS_PARTITION

/************************************************************
 * RTC
 ************************************************************/
#define CONFIG_RTC_S3C24X0

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_BSP
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DATE
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ELF
#define CONFIG_CMD_NAND
#define CONFIG_CMD_PING
#define CONFIG_CMD_REGINFO
#define CONFIG_CMD_USB
#define CONFIG_CMD_UNZIP

#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#define CONFIG_COMMAND_HISTORY

/* autoboot */
#define CONFIG_BOOTDELAY		5
#define CONFIG_BOOT_RETRY_TIME	-1
#define CONFIG_RESET_TO_RETRY
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_ETHADDR			08:00:3a:26:05:6b
#define CONFIG_GATEWAYIP		192.168.0.1
#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_IPADDR			192.168.0.123
#define CONFIG_SERVERIP			192.168.0.105

#define CONFIG_BOOTFILE			"uImage"		/* Boot file name */
#if 0
#define CONFIG_BOOTARGS    		"console=ttySAC0,115200n8 "		\
								"root=/dev/nfs "			\
								"nfsroot="__stringify(CONFIG_SERVERIP)	\
								":/opt/FriendlyARM/mini2440/rootfs ip="__stringify(CONFIG_IPADDR)		\
								":"__stringify(CONFIG_SERVERIP)			\
								":"__stringify(CONFIG_GATEWAYIP)			\
								":"__stringify(CONFIG_NETMASK)			\
								":mini2440.arm9.net:eth0:off"
#define CONFIG_BOOTCOMMAND		"nfs "__stringify(CONFIG_SYS_LOAD_ADDR)		\
								" "__stringify(CONFIG_SERVERIP)			\
								":/opt/development/mini2440/linux-3.18.47/arch/arm/boot/${CONFIG_BOOTFILE};bootm"
#else
#define CONFIG_BOOTARGS			"noinitrd "	\
								"root=/dev/mtdblock4 "	\
								"init=/linuxrc "	\
								"console=ttySAC0,115200n8" \
								"earlyprintk ignore_loglevel initcall_debug"
#define CONFIG_BOOTCOMMAND		"setenv bootargs ${bootargs} ${mtdparts};"	\
								"nand read "__stringify(CONFIG_SYS_LOAD_ADDR)" kernel;bootm"
#endif


/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_PROMPT		"MINI2440 # "
#define CONFIG_SYS_CBSIZE		256
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define CONFIG_DISPLAY_CPUINFO	/* Display cpu info */

#define CONFIG_SYS_MEMTEST_START	0x30000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x33F00000	/* 63 MB in DRAM */

#define CONFIG_SYS_LOAD_ADDR		0x30008000

/* support additional compression methods */
#define CONFIG_BZIP2
#define CONFIG_LZO
#define CONFIG_LZMA

/* Device Tree support */
#define CONFIG_OF_LIBFDT 
#define CONFIG_CMD_FDT

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS		1          /* we have 1 bank of DRAM */
#define PHYS_SDRAM_1				0x30000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE			0x04000000 /* 64 MB */

#define PHYS_FLASH_1				0x00000000 /* Flash Bank #0 */
#define CONFIG_SYS_FLASH_BASE		PHYS_FLASH_1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_FLASH_CFI_LEGACY
#define CONFIG_SYS_FLASH_LEGACY_1Mx16
#define CONFIG_FLASH_SHOW_PROGRESS		45

#define CONFIG_SYS_MAX_FLASH_BANKS		1
#define CONFIG_SYS_FLASH_BANKS_LIST     { CONFIG_SYS_FLASH_BASE }
#define CONFIG_SYS_MAX_FLASH_SECT		(512)

/*
 * Size of malloc() pool
 * BZIP2 / LZO / LZMA need a lot of RAM
 */
#define CONFIG_SYS_MALLOC_LEN			(4 * 1024 * 1024)

#define CONFIG_SYS_MONITOR_LEN			(448 * 1024)
#define CONFIG_SYS_MONITOR_BASE			CONFIG_SYS_FLASH_BASE

/*
 * NAND configuration
 */
#define CONFIG_NAND_S3C2440
#define CONFIG_S3C2440_NAND_HWECC
#define CONFIG_MTD_NAND_VERIFY_WRITE
#define CONFIG_SYS_MAX_NAND_DEVICE		1
#define CONFIG_SYS_NAND_MAX_CHIPS		1
#define CONFIG_SYS_NAND_BASE			0x4E000000
#define CONFIG_SYS_NAND_5_ADDR_CYCLE

#define CONFIG_SYS_NAND_PAGE_SIZE		2048
#define CONFIG_SYS_NAND_BLOCK_SIZE		(128 * 1024)
#define CONFIG_SYS_NAND_PAGE_COUNT		64
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0
#define CONFIG_SYS_NAND_ECCSIZE			CONFIG_SYS_NAND_PAGE_SIZE
#define CONFIG_SYS_NAND_ECCBYTES		4
#define CONFIG_SYS_NAND_ECCSTEPS		(CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_ECCSIZE)
#define CONFIG_SYS_NAND_OOBSIZE			64
#define CONFIG_SYS_NAND_ECCTOTAL		(CONFIG_SYS_NAND_BYTES / CONFIG_STS_NAND_ECCSTEPS)
#define CONFIG_SYS_NAND_ECCPOS			{40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63}

/*
 * File system
 */
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_YAFFS2
#define CONFIG_RBTREE

#define MTDIDS_DEFAULT "nand0=nandflash0"

/* bootstrap + u-boot + env + linux in nandflash */
#define CONFIG_ENV_IS_IN_NAND
#ifdef CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET 		0x100000
#define CONFIG_ENV_SIZE 		0x10000
#endif

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define MTDPARTS_DEFAULT 						\
	"mtdparts=nandflash0:1m(u-boot),256k(env),"		\
	"256k(spare),10m(kernel),15m(cramfs),20m(ubifs),"	\
	"20m(squashfs),20m(jffs2),20m(yaffs)"


#define	CONFIG_EXTRA_ENV_SETTINGS			\
	"db=tftp "__stringify(CONFIG_SYS_LOAD_ADDR)" "CONFIG_SPL_TARGET	\
		";nand erase.part u-boot"			\
		";nand write "__stringify(CONFIG_SYS_LOAD_ADDR)" u-boot\0"	\
	"dk=tftp "__stringify(CONFIG_SYS_LOAD_ADDR)" "CONFIG_BOOTFILE	\
		";nand erase.part kernel"			\
		";nand write "__stringify(CONFIG_SYS_LOAD_ADDR)" kernel\0"	\
	"dc=tftp "__stringify(CONFIG_SYS_LOAD_ADDR)" cramfs.img"			\
		";nand erase.part cramfs"			\
		";nand write "__stringify(CONFIG_SYS_LOAD_ADDR)" cramfs\0"	\
	"du=tftp "__stringify(CONFIG_SYS_LOAD_ADDR)" ubifs.img"				\
		";nand erase.part ubifs"			\
		";nand write "__stringify(CONFIG_SYS_LOAD_ADDR)" ubifs\0"	\
	"ds=tftp "__stringify(CONFIG_SYS_LOAD_ADDR)" squashfs.img"			\
		";nand erase.part squashfs"			\
		";nand write "__stringify(CONFIG_SYS_LOAD_ADDR)" squashfs\0"\
	"dj=tftp "__stringify(CONFIG_SYS_LOAD_ADDR)" jffs2.img"				\
		";nand erase.part jffs2"			\
		";nand write "__stringify(CONFIG_SYS_LOAD_ADDR)" jffs2\0"	\
	"dy=tftp "__stringify(CONFIG_SYS_LOAD_ADDR)" yaffs.img"				\
		";nand erase.part yaffs"			\
		";nand write "__stringify(CONFIG_SYS_LOAD_ADDR)" yaffs\0"

/* additions for new relocation code, must be added to all boards */
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x1000 - GENERATED_GBL_DATA_SIZE)

#define CONFIG_BOARD_EARLY_INIT_F

#ifndef CONFIG_SPL_BUILD
#define CONFIG_SKIP_LOWLEVEL_INIT
#else
#define CONFIG_NAND_SPL
#endif

/* SPL */
#define CONFIG_SPL_TARGET			"u-boot-with-spl.bin"
#define CONFIG_SPL_LDSCRIPT			"arch/$(ARCH)/cpu/u-boot-spl.lds"
#define CONFIG_SPL_MAX_SIZE			0x1000
#define CONFIG_SPL_TEXT_BASE    	0x000000000
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_NAND_LOAD
#define CONFIG_SPL_NAND_SIMPLE
#define CONFIG_SPL_NAND_DRIVERS

/* NAND boot config */
#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_NAND_LOAD
#define CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_NAND_U_BOOT_OFFS		(CONFIG_SPL_TEXT_BASE + CONFIG_SPL_MAX_SIZE)
#define CONFIG_SYS_NAND_U_BOOT_DST		CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_NAND_U_BOOT_SIZE		(0x100000 - CONFIG_SPL_MAX_SIZE)

#endif /* __CONFIG_H */
