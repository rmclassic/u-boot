// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  (C) Copyright 2010-2013
 *  NVIDIA Corporation <www.nvidia.com>
 *
 *  (C) Copyright 2021
 *  Svyatoslav Ryhel <clamor95@gmail.com>
 */

/* T30 Transformers derive from Cardhu board */

#include <common.h>
#include <dm.h>
#include <log.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/gp_padctrl.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <linux/delay.h>
#include "pinmux-config-transformer.h"
#include <i2c.h>

#define PMU_I2C_ADDRESS		0x2D
#define MAX_I2C_RETRY		3

#define TPS65911_LDO1		0x30
#define TPS65911_LDO2		0x31
#define TPS65911_LDO5		0x32
#define TPS65911_LDO3		0x37

#define TPS65911_GPIO0		0x60
#define TPS65911_GPIO1		0x61
#define TPS65911_GPIO2		0x62
#define TPS65911_GPIO3		0x63
#define TPS65911_GPIO4		0x64
#define TPS65911_GPIO5		0x65
#define TPS65911_GPIO6		0x66
#define TPS65911_GPIO7		0x67
#define TPS65911_GPIO8		0x68

#define TPS65911_DEVCTRL	0x3F
#define DEVCTRL_PWR_OFF_MASK	0x80
#define DEVCTRL_DEV_OFF_MASK	0x01
#define DEVCTRL_DEV_ON_MASK	0x04

#define TEGRA_CLK_RESET_BASE	0x60006000
#define TEGRA_FUSE_CLK_ENB	0x14
#define TEGRA_FUSE_CLK_PREP	0x48

#define FUSE_PRIVATE_KEY0	0x7000F9A4
#define FUSE_PRIVATE_KEY1	0x7000F9A8
#define FUSE_PRIVATE_KEY2	0x7000F9AC
#define FUSE_PRIVATE_KEY3	0x7000F9B0
#define FUSE_PRIVATE_KEY4	0x7000F9B4

#ifdef CONFIG_CMD_POWEROFF
int do_poweroff(struct cmd_tbl *cmdtp,
		       int flag, int argc, char *const argv[])
{
	u32 key;

	key = readl_relaxed(TEGRA_CLK_RESET_BASE + TEGRA_FUSE_CLK_PREP);
	key |= 1 << 28;
	writel(key, TEGRA_CLK_RESET_BASE + TEGRA_FUSE_CLK_PREP);

	/*
	 * Enable FUSE clock. This needs to be hardcoded because the clock
	 * subsystem is not active during early boot.
	 */
	key = readl(TEGRA_CLK_RESET_BASE + TEGRA_FUSE_CLK_ENB);
	key |= 1 << 7;
	writel(key, TEGRA_CLK_RESET_BASE + TEGRA_FUSE_CLK_ENB);

	key = readl(FUSE_PRIVATE_KEY0);
	printf("Fuse private key 0 is %x\n", key);

	key = readl(FUSE_PRIVATE_KEY1);
	printf("Fuse private key 1 is %x\n", key);

	key = readl(FUSE_PRIVATE_KEY2);
	printf("Fuse private key 2 is %x\n", key);

	key = readl(FUSE_PRIVATE_KEY3);
	printf("Fuse private key 3 is %x\n", key);

	key = readl(FUSE_PRIVATE_KEY4);
	printf("Fuse private key 4 is %x\n", key);

	return 1;
}
#endif

/*
 * Routine: pinmux_init
 * Description: Do individual peripheral pinmux configs
 */
void pinmux_init(void)
{
	pinmux_config_pingrp_table(tegra3_pinmux_common,
		ARRAY_SIZE(tegra3_pinmux_common));

	pinmux_config_pingrp_table(unused_pins_lowpower,
		ARRAY_SIZE(unused_pins_lowpower));

	/* Initialize any non-default pad configs (APB_MISC_GP regs) */
	pinmux_config_drvgrp_table(cardhu_padctrl, ARRAY_SIZE(cardhu_padctrl));

#ifdef CONFIG_TRANSFORMER_TF700T_MIPI
	pinmux_config_pingrp_table(tf700t_mipi_pinmux,
		ARRAY_SIZE(tf700t_mipi_pinmux));
#endif
}

#ifdef CONFIG_MMC_SDHCI_TEGRA
/*
 * Do I2C/PMU writes to bring up SD card and eMMC bus power
 */
void board_sdmmc_voltage_init(void)
{
	struct udevice *dev;
	uchar data_buffer[1];
	int ret;
	int i;

	ret = i2c_get_chip_for_busnum(0, PMU_I2C_ADDRESS, 1, &dev);
	if (ret) {
		debug("%s: Cannot find PMIC I2C chip\n", __func__);
		return;
	}

	/* TPS659110: LDO1_REG = 3.3v, ACTIVE to SDMMC4 */
	data_buffer[0] = 0xc9;

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_LDO1, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: LDO2_REG = 3.1v, ACTIVE to SDMMC1 */
	data_buffer[0] = 0xb9;

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_LDO2, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: LDO3_REG = 3.1v, ACTIVE to SDMMC1 VIO */
	data_buffer[0] = 0x5d;

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_LDO3, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: LDO5_REG = 3.3v, ACTIVE to SDMMC1 VIO */
	data_buffer[0] = 0x65;

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_LDO5, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: GPIO0_REG output high to VDD_5V0_SBY */
	data_buffer[0] = 0x07;

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_GPIO0, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: GPIO6_REG output high to VDD_3V3_SYS */
	data_buffer[0] = 0x07;

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_GPIO6, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: GPIO7_REG output high to VDD_1V5_DDR */
	data_buffer[0] = 0x07;

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_GPIO7, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: GPIO8_REG pull_down output high to VDD_5V0_SYS */
	data_buffer[0] = 0x0f;

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_GPIO8, data_buffer, 1))
			udelay(100);
	}
}

/*
 * Routine: pin_mux_mmc
 * Description: setup the MMC muxes, power rails, etc.
 */
void pin_mux_mmc(void)
{
	/*
	 * NOTE: We don't do mmc-specific pin muxes here.
	 * They were done globally in pinmux_init().
	 */

	/* Bring up uSD and eMMC power */
	board_sdmmc_voltage_init();
}
#endif	/* MMC */
