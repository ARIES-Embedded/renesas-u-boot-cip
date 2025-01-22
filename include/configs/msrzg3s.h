/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ifndef __MSRZG3S_H
#define __MSRZG3S_H

#include <asm/arch/rmobile.h>

#define CONFIG_REMAKE_ELF

#ifdef CONFIG_SPL
#define CONFIG_SPL_TARGET	"spl/u-boot-spl.scif"
#endif

/* boot option */

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/* Generic Interrupt Controller Definitions */
/* RZ/G3S use GIC-v3 */
#define CONFIG_GICV3
#define GICD_BASE	0x12400000
#define GICR_BASE	0x12440000

/* console */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS		64
#define CONFIG_SYS_BAUDRATE_TABLE	{ 115200, 38400 }

/* PHY needs a longer autoneg timeout */
#define PHY_ANEG_TIMEOUT		20000

/* MEMORY */
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE

/* SDHI clock freq */
#define CONFIG_SH_SDHI_FREQ		133000000

#ifdef CONFIG_SUPPORT_EMMC_BOOT
#define DRAM_SIZE			0x40000000u /* 1 GB */
#else
#define DRAM_SIZE			0x20000000u /* 512 MB */
#endif
#define DRAM_RSV_SIZE			0x08000000
#define CONFIG_SYS_SDRAM_BASE		(0x40000000 + DRAM_RSV_SIZE)
#define CONFIG_SYS_SDRAM_SIZE		(DRAM_SIZE - DRAM_RSV_SIZE)
#define CONFIG_SYS_LOAD_ADDR		0x58000000
/* Default load address for tfpt,bootp... */
#define CONFIG_LOADADDR			CONFIG_SYS_LOAD_ADDR
#define CONFIG_VERY_BIG_RAM
#define CONFIG_MAX_MEM_MAPPED		(DRAM_SIZE - DRAM_RSV_SIZE)

#define CONFIG_SYS_MONITOR_BASE		0x00000000
#define CONFIG_SYS_MONITOR_LEN		(1 * 1024 * 1024)
#define CONFIG_SYS_MALLOC_LEN		(64 * 1024 * 1024)
#define CONFIG_SYS_BOOTM_LEN		(64 << 20)

/* The HF/QSPI layout permits up to 1 MiB large bootloader blob */
#define CONFIG_BOARD_SIZE_LIMIT		1048576

/* ENV setting */

#ifdef CONFIG_SUPPORT_EMMC_BOOT
#define MSRZ_SD_DEV "1:1"
#else
#define MSRZ_SD_DEV "0:1"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS		\
	"usb_pgood_delay=2000\0" \
	"bootm_size=0x10000000\0" \
	"sdbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk1p1\0" \
	"bootimage=booti 0x48080000 - 0x48000000\0" \
	"image_file=boot/Image\0" \
	"fdt_file=boot/msrzg3s-a0a.dtb\0" \
	"sdload=ext4load mmc " MSRZ_SD_DEV " 0x48080000 ${image_file};ext4load mmc " MSRZ_SD_DEV " 0x48000000 ${fdt_file};run sdbootargs\0" \

#define CONFIG_BOOTCOMMAND	"run sdload bootimage"

/* For board */
/* Ethernet RAVB */
#define CONFIG_BITBANGMII_MULTI

#endif /* __RZG3S_DEV_H */
