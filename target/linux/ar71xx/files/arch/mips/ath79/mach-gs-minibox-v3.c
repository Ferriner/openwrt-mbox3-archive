/*
 *  Gainstrong MiniBox V3 board support
 *
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-gpio-buttons.h"
#include "dev-eth.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define GS_MINIBOX_V3_GPIO_BTN_RESET	17
#define GS_MINIBOX_V3_GPIO_LED_SYSTEM	14

#define GS_MINIBOX_V3_KEYS_POLL_INTERVAL	20	/* msecs */
#define GS_MINIBOX_V3_KEYS_DEBOUNCE_INTERVAL (3 * GS_MINIBOX_V3_KEYS_POLL_INTERVAL)

#define GS_MINIBOX_V3_WMAC_CALDATA_OFFSET	0x1000

static const char *gs_minibox_v3_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data gs_minibox_v3_flash_data = {
	.part_probes	= gs_minibox_v3_part_probes,
};

static struct gpio_led gs_minibox_v3_leds_gpio[] __initdata = {
	{
		.name		= "minibox-v3:green:system",
		.gpio		= GS_MINIBOX_V3_GPIO_LED_SYSTEM,
		.active_low	= 1,
	},
};

static struct gpio_keys_button gs_minibox_v3_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = GS_MINIBOX_V3_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= GS_MINIBOX_V3_GPIO_BTN_RESET,
		.active_low	= 0,
	},
};

static void __init gs_minibox_v3_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_setup_ar933x_phy4_switch(false, false);
	
	ath79_register_leds_gpio(-1, ARRAY_SIZE(gs_minibox_v3_leds_gpio),
				 gs_minibox_v3_leds_gpio);

	ath79_register_gpio_keys_polled(-1, GS_MINIBOX_V3_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(gs_minibox_v3_gpio_keys),
					gs_minibox_v3_gpio_keys);
	
	ath79_register_m25p80(&gs_minibox_v3_flash_data);
	ath79_register_usb();

	ath79_register_mdio(0, 0x0);
	
	/* WAN */
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 1);
	ath79_register_eth(0);

	/* LAN */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, -1);
	ath79_register_eth(1);

	ath79_register_wmac(art + GS_MINIBOX_V3_WMAC_CALDATA_OFFSET, mac);

}

MIPS_MACHINE(ATH79_MACH_GS_MINIBOX_V3, "MINIBOX-V3",
	     "MiniBox V3", gs_minibox_v3_setup);
