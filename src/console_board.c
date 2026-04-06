// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file console_board.c
 * @brief AkiraConsole product-level board initialisation
 *
 * Runs via Zephyr's SYS_INIT mechanism at APPLICATION level — after the
 * AkiraOS platform HAL, filesystems, and runtime are already up.  This is
 * the right place for anything that is specific to the AkiraConsole *product*
 * rather than the AkiraOS platform:
 *
 *   - Boot-time button sampling (e.g. WiFi provisioning shortcut)
 *   - Product-specific log banner
 *   - Future: battery / charging status, LED patterns, etc.
 *
 * Add new console-specific initialisation here without modifying AkiraOS.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>

LOG_MODULE_REGISTER(console_board, CONFIG_AKIRA_LOG_LEVEL);

/* ── Boot button (GPIO_0 pin 0, active-low with internal pull-up) ─────── */
#define BOOT_BUTTON_NODE DT_ALIAS(sw0)

#if DT_NODE_HAS_STATUS(BOOT_BUTTON_NODE, okay)
static const struct gpio_dt_spec boot_btn =
	GPIO_DT_SPEC_GET(BOOT_BUTTON_NODE, gpios);
#endif

/* Set to true by early boot sampling when the user holds the BOOT button. */
static bool s_wifi_prov_requested;

bool akira_console_wifi_prov_requested(void)
{
	return s_wifi_prov_requested;
}

/* ── Initialisation ────────────────────────────────────────────────────── */

static int console_board_init(void)
{
	LOG_INF("AkiraConsole board: " CONFIG_AKIRA_CONSOLE_PRODUCT_NAME);

#if defined(CONFIG_AKIRA_CONSOLE_BOOT_BUTTON_WIFI_PROV) && \
    DT_NODE_HAS_STATUS(BOOT_BUTTON_NODE, okay)
	if (!device_is_ready(boot_btn.port)) {
		LOG_WRN("Boot button GPIO not ready — provisioning shortcut disabled");
		return 0;
	}

	if (gpio_pin_configure_dt(&boot_btn, GPIO_INPUT) < 0) {
		LOG_WRN("Failed to configure boot button");
		return 0;
	}

	/* Button is active-low: reading 0 means pressed */
	if (gpio_pin_get_dt(&boot_btn) == 0) {
		s_wifi_prov_requested = true;
		LOG_WRN("BOOT button held at power-on — WiFi provisioning requested");
	}
#endif /* CONFIG_AKIRA_CONSOLE_BOOT_BUTTON_WIFI_PROV */

	return 0;
}

/*
 * APPLICATION priority runs after Zephyr drivers and kernel services are
 * ready, but the prio offset (99) keeps this after AkiraOS platform init
 * (which uses lower numbers).
 */
SYS_INIT(console_board_init, APPLICATION, 99);
