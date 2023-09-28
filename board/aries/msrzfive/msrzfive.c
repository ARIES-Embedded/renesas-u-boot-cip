// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 */

#include <common.h>
#include <spl.h>
#include <init.h>
#include <dm.h>
#include <asm/sections.h>
#include <asm/arch/sh_sdhi.h>
#include <mmc.h>
#include <i2c.h>
#include <hang.h>
#include <wdt.h>
#include <rzg2l_wdt.h>
#include <renesas/rzf-dev/mmio.h>
#include <renesas/rzf-dev/rzf-dev_def.h>
#include <renesas/rzf-dev/rzf-dev_sys.h>
#include <renesas/rzf-dev/rzf-dev_pfc_regs.h>
#include <renesas/rzf-dev/rzf-dev_cpg_regs.h>
#include "../../renesas/rzf-dev/rzf-dev_spi_multi.h"

#define USBPHY_BASE		0x11c40000
#define USB0_BASE		0x11c50000
#define USB1_BASE		0x11c70000
#define USBF_BASE		0x11c60000
#define USBPHY_RESET		(USBPHY_BASE + 0x000u)
#define COMMCTRL		0x800
#define HcRhDescriptorA		0x048
#define LPSTS			0x102

#define RPC_CMNCR		0x10060000

/* WDT */
#define WDT_INDEX		0

extern void cpg_setup(void);
extern void pfc_setup(void);
extern void ddr_setup(void);
extern int spi_multi_setup(uint32_t addr_width, uint32_t dq_width, uint32_t dummy_cycle);

void cpu_cpg_setup(void);

DECLARE_GLOBAL_DATA_PTR;

void spl_early_board_init_f(void);
extern phys_addr_t prior_stage_fdt_address;
/*
 * Miscellaneous platform dependent initializations
 */
#ifdef CONFIG_V5L2_CACHE
static void v5l2_init(void)
{
	struct udevice *dev;

	uclass_get_device(UCLASS_CACHE, 0, &dev);
}
#endif

#ifdef CONFIG_BOARD_EARLY_INIT_F
int board_early_init_f(void)
{
#ifdef CONFIG_V5L2_CACHE
	v5l2_init();
#endif
	/* can go in board_eth_init() once enabled */
	*(volatile u32 *)(PFC_ETH_ch0) = (*(volatile u32 *)(PFC_ETH_ch0) & 0xFFFFFFFC) | ETH_ch0_1_8;
	*(volatile u32 *)(PFC_ETH_ch1) = (*(volatile u32 *)(PFC_ETH_ch1) & 0xFFFFFFFC) | ETH_ch1_1_8;

	/* Enable RGMII for both ETH{0,1} */
	*(volatile u32 *)(PFC_ETH_MII) = (*(volatile u32 *)(PFC_ETH_MII) & 0xFFFFFFFC);

	/* ETH CLK */
	*(volatile u32 *)(CPG_RST_ETH) = 0x30003;

	/* I2C CLK */
	*(volatile u32 *)(CPG_RST_I2C) = 0xF000F;

	/* I2C pin non GPIO enable */
	*(volatile u32 *)(PFC_IEN0E) = 0x01010101;

	/* SD CLK */
	*(volatile u32 *)(CPG_PL2SDHI_DSEL) = 0x00110011;
	while (*(volatile u32 *)(CPG_CLKSTATUS) != 0)
		;

	*(volatile u32 *)(RPC_CMNCR) = 0x01FFF300;
	return 0;
}
#endif

static void board_usb_init(void)
{
	/*Enable USB*/
	(*(volatile u32 *)CPG_RST_USB) = 0x000f000f;
	(*(volatile u32 *)CPG_CLKON_USB) = 0x000f000f;

	/* Setup  */
	/* Disable GPIO Write Protect */
	(*(volatile u32 *)PFC_PWPR) &= ~(0x1u << 7);    /* PWPR.BOWI = 0 */
	(*(volatile u32 *)PFC_PWPR) |= (0x1u << 6);     /* PWPR.PFCWE = 1 */

	/* set P5_0 as Func.1 for VBUSEN */
	(*(volatile u8 *)PFC_PMC15) |= (0x1u << 0);
	(*(volatile u8 *)PFC_PFC15) &= ~(0x7u << 0);
	(*(volatile u8 *)PFC_PFC15) |= (0x1u << 0);

	/* set P5_2 as Func.1 for OVERCUR */
	(*(volatile u8 *)PFC_PMC15) |= 0x4;
	(*(volatile u8 *)PFC_PFC15) &= ~(0x7u << 8);
	(*(volatile u8 *)PFC_PFC15) |= (0x1u << 8);

	/* set P6_0 as Func.1 for VBUSEN */
	(*(volatile u8 *)PFC_PMC16) |= (0x1u << 0);
	(*(volatile u8 *)PFC_PFC16) &= ~(0x7u << 0);
	(*(volatile u8 *)PFC_PFC16) |= (0x1u << 0);

	/* set P5_4 as Func.5 for OVERCUR */
	(*(volatile u8 *)PFC_PMC15) = (*(volatile u8 *)PFC_PMC15 & 0xEF) | 0x10;
	(*(volatile u32 *)PFC_PFC15) &= ~(0x7u << 16);
	(*(volatile u32 *)PFC_PFC15) |= (0x5u << 16);

	/* Enable write protect */
	(*(volatile u32 *)PFC_PWPR) &= ~(0x1u << 6);    /* PWPR.PFCWE = 0 */
	(*(volatile u32 *)PFC_PWPR) |= (0x1u << 7);     /* PWPR.BOWI = 1 */

	/*Enable 2 USB ports*/
	(*(volatile u32 *)USBPHY_RESET) = 0x00001000u;
	/*USB0 is HOST*/
	(*(volatile u32 *)(USB0_BASE + COMMCTRL)) = 0;
	/*USB1 is HOST*/
	(*(volatile u32 *)(USB1_BASE + COMMCTRL)) = 0;
	/* Set USBPHY normal operation (Function only) */
	(*(volatile u16 *)(USBF_BASE + LPSTS)) |= (0x1u << 14);		/* USBPHY.SUSPM = 1 (func only) */
	/* Overcurrent is not supported */
	(*(volatile u32 *)(USB0_BASE + HcRhDescriptorA)) |= (0x1u << 12);       /* NOCP = 1 */
	(*(volatile u32 *)(USB1_BASE + HcRhDescriptorA)) |= (0x1u << 12);       /* NOCP = 1 */
}

int board_init(void)
{
	board_usb_init();
	return 0;
}


int dram_init(void)
{
#ifdef CONFIG_DEBUG_RZF_FPGA
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;
	return 0;
#endif
	return fdtdec_setup_mem_size_base();
}


#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
	/* boot using first FIT config */
	return 0;
}
#endif


#ifdef CONFIG_SPL
u32 spl_boot_device(void)
{
#ifdef CONFIG_DEBUG_RZF_FPGA
	return BOOT_DEVICE_NOR;
#else
	uint16_t boot_dev;

	boot_dev = *((uint16_t *)RZF_BOOTINFO_BASE) & MASK_BOOTM_DEVICE;
	switch (boot_dev)
	{
		case BOOT_MODE_SPI_1_8:
		case BOOT_MODE_SPI_3_3:
			return BOOT_DEVICE_NOR;

		case BOOT_MODE_ESD:
		case BOOT_MODE_EMMC_1_8:
		case BOOT_MODE_EMMC_3_3:
			return BOOT_DEVICE_MMC1;

		case BOOT_MODE_SCIF:
		default:
			return BOOT_DEVICE_NONE;
	}
#endif
}
#endif


#ifdef CONFIG_SPL_BUILD
void spl_early_board_init_f(void)
{
	/* setup PFC */
	pfc_setup();

	/* set QSPI 1.8V voltage level */
	mmio_write_32(PFC_QSPI, 1);

	/* setup Clock and Reset */
	cpg_setup();
}

int spl_board_init_f(void)
{
	uint16_t boot_dev;

#ifndef CONFIG_DEBUG_RZF_FPGA
	/* initialize DDR */
	ddr_setup();
#endif
	/* initisalize SPI Multi when SPI BOOT */
	boot_dev = *((uint16_t *)RZF_BOOTINFO_BASE) & MASK_BOOTM_DEVICE;
	if (boot_dev == BOOT_MODE_SPI_1_8 ||
		boot_dev == BOOT_MODE_SPI_3_3) {
		spi_multi_setup(SPI_MULTI_ADDR_WIDES_24, SPI_MULTI_DQ_WIDES_1_1_1, SPI_MULTI_DUMMY_8CYCLE);
	}

	return 0;
}

void board_init_f(ulong dummy)
{
	int ret;

	cpu_cpg_setup();

	/* Initialize SPL*/
	ret = spl_early_init();
	if (ret)
		panic("spl_early_init() failed: %d\n", ret);

	/* Initialize CPU Architecure */
	arch_cpu_init_dm();

	/* Initialixe Bord part */
	spl_early_board_init_f();

	/* Initialize console */
	preloader_console_init();

	/* Initialize Board part */
	ret = spl_board_init_f();
	if (ret)
		panic("spl_board_init_f() failed: %d\n", ret);
}
#endif

void reset_cpu(void)
{
#if defined(CONFIG_RENESAS_RZG2LWDT) && !defined(CONFIG_SPL_BUILD)
	struct udevice *wdt_dev;
	if (uclass_get_device(UCLASS_WDT, WDT_INDEX, &wdt_dev) < 0) {
		printf("failed to get wdt device. cannot reset\n");
		return;
	}
	if (wdt_expire_now(wdt_dev, 0) < 0) {
		printf("failed to expire_now wdt\n");
	}
#endif // CONFIG_RENESAS_RZG2LWDT
}

int board_late_init(void)
{
#ifdef CONFIG_RENESAS_RZG2LWDT
	rzg2l_reinitr_wdt();
#endif // CONFIG_RENESAS_RZG2LWDT

	return 0;
}
