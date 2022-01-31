// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Svyatoslav Ryhel <clamor95@gmail.com>
 */

#include <common.h>
#include <clk.h>
#include <dm.h>
#include <i2c.h>
#include <log.h>
#include <video_bridge.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <power/regulator.h>

#include <asm/gpio.h>

/* TF700T uses Toshiba TC358768 MIPI DSI bridge */

#define DISPLAY_MAX_RETRIES 3 /* max counter for retry I2C access */

#define usleep_range(a, b) udelay((b))
#define msleep(a) udelay(a * 1000)

struct tc358768_priv {
	struct udevice *vddc;
	struct udevice *vddmipi;
	struct udevice *vddio;

	struct clk *refclk;

	struct gpio_desc reset;
};

static int tc358768_set_backlight(struct udevice *dev, int percent)
{
	return -ENOSYS;
}

static int tc358768_attach(struct udevice *dev)
{
	return 0;
}

static int tc358768_setup(struct udevice *dev)
{
	struct tc358768_priv *uc_priv = dev_get_uclass_priv(dev);
	int ret;

	/* get regulators */
	ret = device_get_supply_regulator(dev, "vddc-supply", &uc_priv->vddc);
	if (ret && ret != -ENOENT) {
		debug("%s: vddc regulator error:%d\n", __func__, ret);
		return ret;
	}

	ret = device_get_supply_regulator(dev, "vddmipi-supply", &uc_priv->vddmipi);
	if (ret && ret != -ENOENT) {
		debug("%s: vddmipi regulator error:%d\n", __func__, ret);
		return ret;
	}

	ret = device_get_supply_regulator(dev, "vddio-supply", &uc_priv->vddio);
	if (ret && ret != -ENOENT) {
		debug("%s: vddio regulator error:%d\n", __func__, ret);
		return ret;
	}

	/* get clk */
//	uc_priv->refclk = devm_clk_get_optional(dev, "refclk");
//	if (IS_ERR(uc_priv->refclk))
//		debug("%s: Could not get refclk\n", __func__);

	/* get gpios */
	ret = gpio_request_by_name(dev, "reset-gpios", 1,
				   &uc_priv->reset, GPIOD_IS_OUT);
	if (ret) {
		debug("%s: Could not decode reset-gpios (%d)\n", __func__, ret);
		if (ret != -ENOENT)
			return ret;
	}

	return 0;
}

static int tc358768_probe(struct udevice *dev)
{
	if (device_get_uclass_id(dev->parent) != UCLASS_I2C)
		return -EPROTONOSUPPORT;

	return tc358768_setup(dev);
}

struct video_bridge_ops tc358768_ops = {
	.attach = tc358768_attach,
	.set_backlight = tc358768_set_backlight,
};

static const struct udevice_id tc358768_ids[] = {
	{ .compatible = "toshiba,tc358768", },
	{ }
};

U_BOOT_DRIVER(tc358768) = {
	.name	= "tc358768",
	.id	= UCLASS_VIDEO_BRIDGE,
	.of_match = tc358768_ids,
	.probe	= tc358768_probe,
	.ops	= &tc358768_ops,
	.priv_auto	= sizeof(struct tc358768_priv),
};
