// SPDX-License-Identifier: GPL-2.0+
/*
 * board/renesas/ek874/ek874.c
 *     This file is ek874 board support.
 *
 * Copyright (C) 2019 Marek Vasut <marek.vasut+renesas@gmail.com>
 */

#include <common.h>
#include <malloc.h>
#include <netdev.h>
#include <dm.h>
#include <dm/platform_data/serial_sh.h>
#include <asm/processor.h>
#include <asm/mach-types.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/arch/gpio.h>
#include <asm/arch/rmobile.h>
#include <asm/arch/rcar-mstp.h>
#include <asm/arch/sh_sdhi.h>
#include <i2c.h>
#include <mmc.h>
#include <renesas_wdt.h>

#include "../rzg-common/common.h"

DECLARE_GLOBAL_DATA_PTR;

#define	PFC_PMMR		0xE6060000
#define	PFC_PUEN4		0xE6060410
#define	PFC_PUD4		0xE6060450
#define	PFC_GPSR5		0xE6060114
#define	GPIO_IOINTSEL5		0xE6055000
#define	GPIO_INOUTSEL5		0xE6055004
#define	GPIO_INDT5		0xE605500C
#define	MSTP907_GPIO5		BIT(7)

static char board_rev;

char check_rev(void)
{
	u32 reg;
	int ret;

	/* Check GPIO/Peripheral Function mode */
	reg = readl(PFC_GPSR5) & BIT(19);
	if (reg) {
		reg = readl(PFC_GPSR5) & ~BIT(19);
		writel(~reg, PFC_PMMR);
		writel(reg, PFC_GPSR5);
	}

	/* Check Clock support to Module GPIO5 */
	reg = readl(MSTPSR9) & MSTP907_GPIO5;
	if (reg)
		writel(readl(SMSTPCR9) & ~MSTP907_GPIO5, SMSTPCR9);

	/* Enable Pull up/down function */
	writel(readl(PFC_PUEN4) | BIT(17), PFC_PUEN4);
	/* Enable Pull up */
	writel(readl(PFC_PUD4) | BIT(17), PFC_PUD4);

	/* Set General input/output mode for GP5_19 */
	writel(readl(GPIO_IOINTSEL5) & ~BIT(19), GPIO_IOINTSEL5);
	/* Set General input mode for GP5_19 */
	writel(readl(GPIO_INOUTSEL5) & ~BIT(19), GPIO_INOUTSEL5);
	/* Read the value received through GP5_19 */
	ret = readl(GPIO_INDT5) & BIT(19);

	/* Enable Pull down */
	writel(readl(PFC_PUD4) & BIT(17), PFC_PUD4);
	/* Disable Pull up/down function */
	writel(readl(PFC_PUEN4) & BIT(17), PFC_PUEN4);

	return ret ? 'C' : 'E';
}

void s_init(void)
{
}

int board_early_init_f(void)
{
	return 0;
}

int board_init(void)
{
	board_rev = check_rev();
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_TEXT_BASE + 0x50000;

	return 0;
}

int dram_init(void)
{
	if (fdtdec_setup_mem_size_base() != 0)
		return -EINVAL;

	return 0;
}

int dram_init_banksize(void)
{
	fdtdec_setup_memory_banksize();

	return 0;
}

#define RST_BASE	0xE6160000
#define RST_CA57RESCNT	(RST_BASE + 0x40)
#define RST_CA53RESCNT	(RST_BASE + 0x44)
#define RST_RSTOUTCR	(RST_BASE + 0x58)
#define RST_CA57_CODE	0xA5A5000F
#define RST_CA53_CODE	0x5A5A000F

void reset_cpu(ulong addr)
{
	unsigned long midr, cputype;

	asm volatile("mrs %0, midr_el1" : "=r" (midr));
	cputype = (midr >> 4) & 0xfff;

	if (cputype == 0xd03)
		writel(RST_CA53_CODE, RST_CA53RESCNT);
	else if (cputype == 0xd07)
		writel(RST_CA57_CODE, RST_CA57RESCNT);
	else
		hang();
}

void board_add_ram_info(int use_default)
{
	int i;

	printf("\n");
	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		if (!gd->bd->bi_dram[i].size)
			break;
		printf("Bank #%d: 0x%09llx - 0x%09llx, ", i,
		      (unsigned long long)(gd->bd->bi_dram[i].start),
		      (unsigned long long)(gd->bd->bi_dram[i].start
		      + gd->bd->bi_dram[i].size - 1));
		print_size(gd->bd->bi_dram[i].size, "\n");
	};
}

int board_late_init(void)
{
#ifdef CONFIG_WDT_RENESAS
	reinitr_wdt();
#endif

	env_set("board_rev", &board_rev);

	return 0;
}

#if defined(CONFIG_MULTI_DTB_FIT)
int board_fit_config_name_match(const char *name)
{
	char rev;

	rev = check_rev();

	if (!strcmp(name, "r8a774c0-ek874-revc-u-boot") && (rev == 'C'))
		return 0;

	if (!strcmp(name, "r8a774c0-ek874-u-boot") && (rev == 'E'))
		return 0;

	return -1;
}
#endif

static const char * const dt_non_ecc[] = {
	"/memory@48000000", "reg", "<0x0 0x48000000 0x0 0x78000000>",
};

static const char * const dt_ecc_full_single[] = {
	"/memory@48000000", "reg", "<0x0 0x48000000 0x0 0x3c000000>",
};

int ft_verify_fdt(void *fdt)
{
	const char **fdt_dt = NULL;
	int use_ecc, ecc_mode, size;
	struct pt_regs regs;

	size = 0;
	/* Setting SiP Service GET_ECC_MODE command*/
	regs.regs[0] = RZG_SIP_SVC_GET_ECC_MODE;
	smc_call(&regs);
	/* First result is USE ECC or not, Second result is ECC MODE*/
	use_ecc = regs.regs[0];
	ecc_mode = regs.regs[1];

	if (!use_ecc) {
		fdt_dt = (const char **)dt_non_ecc;
		size = ARRAY_SIZE(dt_non_ecc);
	} else if (use_ecc == 1) {
		switch (ecc_mode) {
		case 0:
			/* The memory map of partial ECC same as non-ECC mode*/
			fdt_dt = (const char **)dt_non_ecc;
			size = ARRAY_SIZE(dt_non_ecc);
			break;
		case 2:
			fdt_dt = (const char **)dt_ecc_full_single;
			size = ARRAY_SIZE(dt_ecc_full_single);
			break;
		default:
			printf("Not support changing device-tree to ");
			printf("compatible with ECC_MODE = %d\n", ecc_mode);
			return 1;
		};
	}
	return update_fdt(fdt, fdt_dt, size);
}
