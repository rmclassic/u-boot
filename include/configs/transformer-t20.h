/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  (C) Copyright 2010,2011
 *  NVIDIA Corporation <www.nvidia.com>
 *
 *  (C) Copyright 2022
 *  Svyatoslav Ryhel <clamor95@gmail.com>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>

#include "tegra20-common.h"

/* High-level configuration options */
#define CONFIG_TEGRA_BOARD_STRING	"ASUS Transformer"

#define TRANSFORMER_FLASH_EBT \
	"update_ebt=echo Reading U-Boot binary;" \
		"if load mmc 1:1 ${kernel_addr_r} ${bootloader_file};" \
		"then echo Writing U-Boot into EBT;" \
			"mmc dev 0 1;" \
			"mmc write ${kernel_addr_r} 0x0 0x1000;" \
		"else echo Reading U-Boot failed; fi\0"

#define BOARD_EXTRA_ENV_SETTINGS \
	"kernel_file=vmlinuz\0" \
	"ramdisk_file=uInitrd\0" \
	"bootloader_file=u-boot-dtb-tegra.bin\0" \
	"bootkernel=bootz ${kernel_addr_r} - ${fdt_addr_r}\0" \
	"bootrdkernel=bootz ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}\0" \
	TRANSFORMER_FLASH_EBT

#define TRANSFORMER_LOAD_KERNEL \
	"echo Loading Kernel;" \
	"if load mmc ${bootdev}:1 ${kernel_addr_r} ${kernel_file};" \
	"then echo Loading DTB;" \
		"load mmc ${bootdev}:1 ${fdt_addr_r} ${fdtfile};" \
		"setenv bootargs console=ttyS0,115200n8 root=/dev/mmcblk${bootdev}p2 rw gpt;" \
		"echo Loading Initramfs;" \
		"if load mmc ${bootdev}:1 ${ramdisk_addr_r} ${ramdisk_file};" \
		"then echo Booting Kernel;" \
			"run bootrdkernel;" \
		"else echo Booting Kernel;" \
			"run bootkernel; fi;"

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND \
	"if button VolumeDown;" \
	"then echo Starting U-Boot update ...;" \
		"run update_ebt;" \
	"else echo Loading from uSD...;" \
		"setenv bootdev 1;" \
		TRANSFORMER_LOAD_KERNEL \
		"else echo Loading from uSD failed!;" \
			"echo Loading from eMMC...;" \
			"setenv bootdev 0;" \
			TRANSFORMER_LOAD_KERNEL \
			"fi;" \
		"fi;" \
	"fi;"

/* Board-specific serial config */
#define CONFIG_TEGRA_ENABLE_UARTD
#define CONFIG_SYS_NS16550_COM1		NV_PA_APB_UARTD_BASE

#include "tegra-common-post.h"

#endif /* __CONFIG_H */
