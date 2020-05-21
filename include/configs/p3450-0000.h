/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2018-2019 NVIDIA Corporation.
 */

#ifndef _P3450_0000_H
#define _P3450_0000_H

#include <linux/sizes.h>

#include "tegra210-common.h"

/* High-level configuration options */
#define CONFIG_TEGRA_BOARD_STRING	"NVIDIA P3450-0000"

/* Board-specific serial config */
#define CONFIG_TEGRA_ENABLE_UARTA

/* Only MMC/PXE/DHCP for now, add USB back in later when supported */
#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 1) \
	func(MMC, mmc, 0) \
	func(PXE, pxe, na) \
	func(DHCP, dhcp, na)

/* Environment at end of QSPI, in the VER partition */
#define CONFIG_ENV_SPI_MAX_HZ		48000000
#define CONFIG_ENV_SPI_MODE		SPI_MODE_0
#define CONFIG_SPI_FLASH_SIZE		(4 << 20)

#define CONFIG_PREBOOT

#define CONTROL_PARTITION "misc"

#define FASTBOOT_CMD \
	"echo Booting into fastboot ...; " \
	"fastboot " __stringify(CONFIG_FASTBOOT_USB_DEV) "; "

#define BOARD_EXTRA_ENV_SETTINGS \
	"preboot=if test -e mmc 1:1 /u-boot-preboot.scr; then " \
		"load mmc 1:1 ${scriptaddr} /u-boot-preboot.scr; " \
		"source ${scriptaddr}; " \
	"fi\0" \
	"tegra_set_values=" \
		"fdt addr 83000000; " \
		"fdt get value serial# / serial-number; " \
		"setexpr ver_simple gsub \"U-Boot (.*) \\\\(.*\\\\)\" \"\\\\1\" \"$ver\"; " \
		"\0" \
	"tegra_android_boot=" \
		"run tegra_set_values; " \
		"setenv bootimg_staging 0x98000000; " \
		"abootimg addr $bootimg_staging; " \
		"mmc dev 1; " \
		"part start mmc 1 boot boot_start; " \
		"part size mmc 1 boot boot_size; " \
		"mmc read $bootimg_staging $boot_start $boot_size; " \
		"abootimg get dtb --index=0 dtb0_start dtb0_size; " \
		"cp.b $dtb0_start $fdt_addr_r $dtb0_size; " \
		"setenv normalboot androidboot.force_normal_boot=1; " \
		"if bcb load " __stringify(CONFIG_FASTBOOT_FLASH_MMC_DEV) " " \
		CONTROL_PARTITION "; then " \
			"if bcb test command = bootonce-bootloader; then " \
				"echo Android: Bootloader boot...; " \
				"bcb clear command; bcb store; " \
				FASTBOOT_CMD \
			"elif bcb test command = boot-recovery; then " \
				"echo Android: Recovery boot...; " \
				"setenv normalboot androidboot.force_normal_boot=0; " \
			"else " \
				"echo Android: Normal boot...; " \
			"fi; " \
		"else " \
			"echo Warning: BCB is corrupted or does not exist; " \
			"echo Android: Normal boot...; " \
		"fi; " \
		"setenv eval_bootargs setenv bootargs '$bootargs $cbootargs $normalboot androidboot.serialno=${serial#} androidboot.bootloader=${ver_simple}'; " \
		"run eval_bootargs; " \
		"bootm $bootimg_staging $bootimg_staging $fdt_addr_r; " \
		"\0"

/* General networking support */
#include "tegra-common-usb-gadget.h"
#include "tegra-common-post.h"

/* Crystal is 38.4MHz. clk_m runs at half that rate */
#define COUNTER_FREQUENCY	19200000

/* Increase kernel image max size to 64M */
#define CONFIG_SYS_BOOTM_LEN	SZ_64M

#endif /* _P3450_0000_H */
