#ifndef __FIVEBERRY_ENV_H
#define __FIVEBERRY_ENV_H

#include <linux/stringify.h>

#ifndef FIVEBERRY_FIRST_LOADER
#error "FIVEBERRY_FIRST_LOADER is not defined"
#endif
#ifndef FIVEBERRY_SECOND_LOADER
#error "FIVEBERRY_SECOND_LOADER is not defined"
#endif
#ifndef FIVEBERRY_SECOND_LOADER_OFFSET
#error "FIVEBERRY_SECOND_LOADER_OFFSET is not defined"
#endif
#ifndef FIVEBERRY_DEFAULT_DEVICE_TREE
#error "FIVEBERRY_DEFAULT_DEVICE_TREE is not defined"
#endif

#ifdef CONFIG_SUPPORT_EMMC_BOOT
#define FIVEBERRY_EMMC_ENV_SETTINGS \
	"divup_filesize=setexpr filesize ${filesize} / 200 && " \
		"setexpr filesize ${filesize} + 1\0" \
	"emmc_bootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk0p1\0" \
	"emmc_boot=ext4load mmc 0:1 ${kernel_addr_r} ${image_file} && " \
		"setenv kernel_comp_size ${filesize} && " \
		"ext4load mmc 0:1 ${fdt_addr_r} ${fdt_file} && " \
		"run emmc_bootargs && booti ${kernel_addr_r} - ${fdt_addr_r}\0" \
	"emmc_update=mmc bootbus 0 2 0 0;mmc partconf 0 0 1 1;" \
		"tftpboot ${serverip}:${bl2_file};" \
		"run divup_filesize;mmc write ${fileaddr} 1 ${filesize};" \
		"tftpboot ${serverip}:${fipfile};" \
		"run divup_filesize;mmc write ${fileaddr} 0x100 ${filesize}\0" \
	"emmc_update=run emmc_bl2_update emmc_fip_update\0"
#define FIVEBERRY_SD_DEV "mmc 1:1"
#else
#define FIVEBERRY_EMMC_ENV_SETTINGS
#define FIVEBERRY_SD_DEV "mmc 0:1"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	"bl2_file=" FIVEBERRY_FIRST_LOADER "\0" \
	"bootargs=rw rootwait earlycon\0" \
	"bootm_size=0x10000000\0" \
	FIVEBERRY_EMMC_ENV_SETTINGS \
	"eth1addr=32:eb:f5:29:04:30\0" \
	"ethaddr=d6:8f:16:4f:c3:c7\0" \
	"fdt_addr_r=0x60000000\0" \
	"fdt_file=" FIVEBERRY_DEFAULT_DEVICE_TREE "\0" \
	"fdt_size=20000\0" \
	"fip_file=" FIVEBERRY_SECOND_LOADER "\0" \
	"image_file=" FIVEBERRY_DEFAULT_IMAGE "\0" \
	"ipaddr=192.168.1.2\0" \
	"kernel_addr_r=0x48000000\0" \
	"kernel_comp_addr_r=0x4c000000\0" \
	"kernel_comp_size=600000\0" \
	"net_args=setenv bootargs earlycon\0" \
	"net_boot=tftpboot ${kernel_addr_r} ${serverip}:${image_file} && " \
		"setenv kernel_comp_size ${filesize} && " \
		"tftpboot ${ramdisk_addr_r} ${serverip}:${ramdisk_file} && " \
		"setenv ramdisk_size ${filesize} && " \
		"tftpboot ${fdt_addr_r} ${serverip}:${fdt_file} && " \
		"run net_args && " \
		"booti ${kernel_addr_r} ${ramdisk_addr_r}:${ramdisk_size} ${fdt_addr_r}\0" \
	"ramdisk_addr_r=0x61000000\0" \
	"ramdisk_file=" FIVEBERRY_DEFAULT_RAMDISK "\0" \
	"ramdisk_size=8c0000\0" \
	"sd_bootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk1p1\0" \
	"sd_boot=ext4load " FIVEBERRY_SD_DEV " ${kernel_addr_r} ${image_file} && " \
		"setenv kernel_comp_size ${filesize} && " \
		"ext4load mmc " ":1 ${fdt_addr_r} ${fdt_file} && " \
		"run sd_bootargs && booti ${kernel_addr_r} - ${fdt_addr_r}\0" \
	"serverip=192.168.1.1\0" \
	"spi_bootargs=setenv bootargs earlycon\0" \
	"spi_boot=sf probe;sf read ${kernel_addr_r} 0x140000 ${kernel_comp_size};" \
		"sf read ${ramdisk_addr_r} 0x740000 ${ramdisk_size};" \
		"sf read ${fdt_addr_r} 0x120000 ${fdt_size};run spi_bootargs;" \
		"booti ${kernel_addr_r} ${ramdisk_addr_r}:${ramdisk_size} ${fdt_addr_r}\0" \
	"spi_update=sf probe;sf protect unlock 0 0x100000;sf erase 0 +0x100000;" \
		"tftpboot ${serverip}:${bl2_file} && sf write ${fileaddr} 0 ${filesize};" \
		"tftpboot ${serverip}:${fip_file} && sf write ${fileaddr} " \
		__stringify(FIVEBERRY_SECOND_LOADER_OFFSET) " ${filesize}\0"

#define CONFIG_BOOTCOMMAND	"run sd_boot || run spi_boot"

#endif /* __FIVEBERRY_ENV_H */
