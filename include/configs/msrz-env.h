#ifndef __MSRZ_ENV_H
#define __MSRZ_ENV_H

#include <linux/stringify.h>

#ifndef MSRZ_FIRST_LOADER
#error "MSRZ_FIRST_LOADER is not defined"
#endif
#ifndef MSRZ_SECOND_LOADER
#error "MSRZ_SECOND_LOADER is not defined"
#endif
#ifndef MSRZ_SECOND_LOADER_OFFSET
#error "MSRZ_SECOND_LOADER_OFFSET is not defined"
#endif
#ifndef MSRZ_DEFAULT_DEVICE_TREE
#error "MSRZ_DEFAULT_DEVICE_TREE is not defined"
#endif

#ifdef CONFIG_SUPPORT_EMMC_BOOT
#define MSRZ_EMMC_ENV_SETTINGS \
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
		"tftpboot ${serverip}:${fip_file};" \
		"run divup_filesize;mmc write ${fileaddr} 0x100 ${filesize}\0"
#define MSRZ_MMC_DEV "1:1"
#define MSRZ_MMC_ROOTFS "/dev/mmcblk1p1"
#else
#define MSRZ_EMMC_ENV_SETTINGS
#define MSRZ_MMC_DEV "0:1"
#define MSRZ_MMC_ROOTFS "/dev/mmcblk0p1"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	"bl2_file=" MSRZ_FIRST_LOADER "\0" \
	"bootargs=rw rootwait earlycon\0" \
	"bootm_size=0x10000000\0" \
	MSRZ_EMMC_ENV_SETTINGS \
	"eth1addr=32:eb:f5:29:04:30\0" \
	"ethaddr=d6:8f:16:4f:c3:c7\0" \
	"fdt_addr_r=0x58000000\0" \
	"fdt_file=" MSRZ_DEFAULT_DEVICE_TREE "\0" \
	"fdt_size=20000\0" \
	"fip_file=" MSRZ_SECOND_LOADER "\0" \
	"image_file=" MSRZ_DEFAULT_IMAGE "\0" \
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
	"ramdisk_addr_r=0x58020000\0" \
	"ramdisk_file=" MSRZ_DEFAULT_RAMDISK "\0" \
	"ramdisk_size=8c0000\0" \
	"mmc_bootargs=setenv bootargs rw rootwait earlycon root=" MSRZ_MMC_ROOTFS "\0" \
	"mmc_boot=echo Booting from MMC...;" \
		"ext4load mmc " MSRZ_MMC_DEV " ${kernel_addr_r} ${image_file} && " \
		"setenv kernel_comp_size ${filesize} && " \
		"ext4load mmc " MSRZ_MMC_DEV " ${fdt_addr_r} ${fdt_file} && " \
		"run mmc_bootargs && booti ${kernel_addr_r} - ${fdt_addr_r}\0" \
	"usb_bootargs=setenv bootargs rw rootwait earlycon root=/dev/sda1\0" \
	"usb_boot=echo Booting from USB...;" \
		"usb start; ext4load usb 0:1 ${kernel_addr_r} ${image_file} && " \
		"setenv kernel_comp_size ${filesize} && " \
		"ext4load usb 0:1 ${fdt_addr_r} ${fdt_file} && " \
		"run usb_bootargs && booti ${kernel_addr_r} - ${fdt_addr_r}\0" \
	"serverip=192.168.1.1\0" \
	"spi_bootargs=setenv bootargs earlycon\0" \
	"spi_boot=echo Booting from SPI...;" \
		"sf probe;sf read ${kernel_addr_r} 0x140000 ${kernel_comp_size};" \
		"sf read ${ramdisk_addr_r} 0x740000 ${ramdisk_size};" \
		"sf read ${fdt_addr_r} 0x120000 ${fdt_size};run spi_bootargs;" \
		"booti ${kernel_addr_r} ${ramdisk_addr_r}:${ramdisk_size} ${fdt_addr_r}\0" \
	"spi_update=sf probe;sf protect unlock 0 0x100000;sf erase 0 +0x100000;" \
		"tftpboot ${serverip}:${bl2_file} && sf write ${fileaddr} 0 ${filesize};" \
		"tftpboot ${serverip}:${fip_file} && sf write ${fileaddr} " \
		__stringify(MSRZ_SECOND_LOADER_OFFSET) " ${filesize}\0" \
	"spi_update_all=run spi_update && " \
		"sf protect unlock 0x120000 0x20000; sf erase 0x120000 +0x20000; " \
		"tftpboot ${serverip}:${fdt_file} && sf write ${fileaddr} 0x120000 ${filesize};" \
		"sf protect unlock 0x140000 0x600000; sf erase 0x140000 +0x600000; " \
		"tftpboot ${serverip}:${image_file} && sf write ${fileaddr} 0x140000 ${filesize};" \
		"sf protect unlock 0x740000 0x8c0000; sf erase 0x740000 +0x8c0000; " \
		"tftpboot ${serverip}:${ramdisk_file} && sf write ${fileaddr} 0x740000 ${filesize}\0" \
	"autoboot=run mmc_boot || run usb_boot || run spi_boot\0" \
	"boot_mode=auto\0" \
	"boot_select=" \
		"if test ${boot_mode} = auto; then run autoboot; fi; " \
		"if test ${boot_mode} = mmc; then run mmc_boot;fi; " \
		"if test ${boot_mode} = usb; then run usb_boot; fi;" \
		"if test ${boot_mode} = spi; then run spi_boot; fi\0"

#define CONFIG_BOOTCOMMAND	"run boot_select"

#endif /* __MSRZ_ENV_H */
