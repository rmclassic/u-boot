/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  (C) Copyright 2010,2012
 *  NVIDIA Corporation <www.nvidia.com>
 *
 *  (C) Copyright 2022
 *  Svyatoslav Ryhel <clamor95@gmail.com>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>

#include "tegra30-common.h"

/* High-level configuration options */
#define CONFIG_TEGRA_BOARD_STRING	"ASUS Transformer"

/* VDD core PMIC */
#define CONFIG_TEGRA_VDD_CORE_TPS62361B_SET3

#ifdef CONFIG_TRANSFORMER_SPI_BOOT
#define TRANSFORMER_VOLDOWN_ACTION \
	"then echo Starting SPI flash update ...;" \
		"run update_spi;"
/* SPI */
#define CONFIG_TEGRA_SLINK_CTRLS       6
#define CONFIG_SPI_FLASH_SIZE          (4 << 20)
#else
#define TRANSFORMER_VOLDOWN_ACTION \
	"then echo Starting U-Boot update ...;" \
		"run update_ebt;"
#endif

#define TRANSFORMER_FLASH_SPI \
	"update_spi=sf probe 0:1;" \
		"echo Dumping current SPI flash content ...;" \
		"sf read ${kernel_addr_r} 0x0 0x400000;" \
		"if fatwrite mmc 1:1 ${kernel_addr_r} spi-flash-backup.bin 0x400000;" \
		"then echo SPI flash content was successfully written into spi-flash-backup.bin;" \
			"echo Reading bootloader binary;" \
			"if load mmc 1:1 ${kernel_addr_r} bootloader-update.bin;" \
			"then echo Writing bootloader into SPI flash;" \
  				"sf probe 0:1;" \
  				"sf update ${kernel_addr_r} 0x0 0x400000;" \
				"poweroff;" \
			"else echo Reading bootloader failed;" \
				"poweroff; fi;" \
		"else echo SPI flash backup FAILED! Aborting ...;" \
			"poweroff; fi\0"

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
	TRANSFORMER_FLASH_SPI \
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
	TRANSFORMER_VOLDOWN_ACTION \
	"else echo Loading from uSD...;" \
		"setenv bootdev 1;" \
		TRANSFORMER_LOAD_KERNEL \
		"else echo Loading from uSD failed!;" \
			"echo Loading from eMMC...;" \
			"setenv bootdev 0;" \
			TRANSFORMER_LOAD_KERNEL \
			"else echo Loading Kernel FAILED! Turning power off;" \
				"poweroff; fi;" \
		"fi;" \
	"fi;"

/* Board-specific serial config */
#define CONFIG_TEGRA_ENABLE_UARTA
#define CONFIG_SYS_NS16550_COM1		NV_PA_APB_UARTA_BASE

#include "tegra-common-post.h"
#include "tegra-common-usb-gadget.h"

#endif /* __CONFIG_H */
