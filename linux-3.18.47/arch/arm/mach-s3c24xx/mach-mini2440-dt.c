/* linux/arch/arm/mach-s3c2440/mach-mini2440.c
 *
 * Copyright (c) 2008 Ramax Lo <ramaxlo@gmail.com>
 *      Based on mach-anubis.c by Ben Dooks <ben@simtec.co.uk>
 *      and modifications by SBZ <sbz@spgui.org> and
 *      Weibing <http://weibing.blogbus.com> and
 *      Michel Pollet <buserror@gmail.com>
 *
 * For product information, visit http://code.google.com/p/mini2440/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#include <linux/clocksource.h>
#include <linux/irqchip.h>
#include <linux/of_platform.h>
#include <linux/serial_s3c.h>

#include <asm/mach/arch.h>
#include <mach/map.h>
#include <mach/regs-mem.h>

#include <plat/cpu.h>
#include <plat/pm.h>

#include "common.h"

static void __init mini2440_dt_map_io(void)
{
    s3c24xx_init_io(NULL, 0);
}

static void __init mini2440_dt_machine_init(void)
{
	of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);
    s3c_pm_init();

	//for dm9000:loss packet
	*((volatile unsigned int *)S3C2410_BWSCON) = 
		(*(volatile unsigned int *)S3C2410_BWSCON & ~(3<<16)) | 
		S3C2410_BWSCON_DW4_16 | S3C2410_BWSCON_WS4 | S3C2410_BWSCON_ST4;
	*((volatile unsigned int *)S3C2410_BANKCON4) = 0x1f7c;
}

static const char *const mini2440_dt_compat[] __initconst = {
    "samsung,s3c2440",
    "samsung,mini2440",
    NULL
};

DT_MACHINE_START(MINI2440_DT, "Samsung S3C2440 (Flattened Device Tree)")
    .dt_compat    = mini2440_dt_compat,
    .map_io        = mini2440_dt_map_io,
    .init_irq    = irqchip_init,
    .init_machine    = mini2440_dt_machine_init,
MACHINE_END

