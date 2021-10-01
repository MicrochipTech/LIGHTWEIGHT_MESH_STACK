/**
 * \file
 *
 * \brief AVR XMEGA WatchDog Timer driver.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include "compiler.h"
#include "ccp.h"
#include "wdt.h"

/*! \brief Set Watchdog timeout period.
 *
 *  This function sets the coded field of the WDT timeout period.
 *
 *  The function writes the correct signature to the Configuration
 *  Change Protection register before writing the CTRL register. Interrupts are
 *  automatically ignored during the change enable period. The function will
 *  wait for the WDT to be synchronized to the WDT clock domain before
 *  proceeding
 *
 *  \param  to_period  WDT timeout coded period
 */
void wdt_set_timeout_period(enum wdt_timeout_period_t to_period)
{
	uint8_t temp = (WDT_PER_gm & (to_period << WDT_PER_gp)) |
			(WDT.CTRL & WDT_ENABLE_bm) | (1 << WDT_CEN_bp);
	ccp_write_io((void *)&WDT.CTRL, temp);
	wdt_wait_while_busy();
}


/*! \brief Set Watchdog window period.
 *
 *  This function sets the coded field of the WDT closed window period.
 *  Note that this setting is available only if the WDT is enabled (hardware
 *  behaviour relayed by software).
 *
 *  The function writes the correct signature to the Configuration
 *  Change Protection register before writing the WINCTRL register. Interrupts
 *  are automatically ignored during the change enable period. The function will
 *  wait for the WDT to be synchronized to the WDT clock domain before
 *  proceeding
 *
 *  \param  win_period  Window coded period
 *
 *  \retval  true   The WDT was enabled and the setting is done.
 *           false  The WDT is disabled and the setting is discarded.
 */
bool wdt_set_window_period(enum wdt_window_period_t win_period)
{
	if (!(wdt_is_enabled())) {
		return false;
	}
	uint8_t temp = (WDT_WPER_gm & (win_period << WDT_WPER_gp)) |
			(WDT.WINCTRL & WDT_WEN_bm) | (1 << WDT_WCEN_bp);
	ccp_write_io((void *)&WDT.WINCTRL, temp);
	wdt_wait_while_busy();
	return true;
}


/*! \brief Disable Watchdog.
 *
 *  This function disables the WDT without changing period settings.
 *
 *  The function writes the correct signature to the Configuration
 *  Change Protection register before writing the CTRL register. Interrupts are
 *  automatically ignored during the change enable period. Disable functions
 *  operate asynchronously with immediate effect.
 */
void wdt_disable(void)
{
	uint8_t temp = (WDT.CTRL & ~WDT_ENABLE_bm) | (1 << WDT_CEN_bp);
	ccp_write_io((void *)&WDT.CTRL, temp);
}


/*! \brief Enable Watchdog.
 *
 *  This function enables the WDT without changing period settings.
 *
 *  The function writes the correct signature to the Configuration
 *  Change Protection register before writing the CTRL register. Interrupts are
 *  automatically ignored during the change enable period. The function will
 *  wait for the WDT to be synchronized to the WDT clock domain before
 *  proceeding
 */
void wdt_enable(void)
{
	uint8_t temp = (WDT.CTRL & WDT_PER_gm) |
			(1 << WDT_ENABLE_bp) | (1 << WDT_CEN_bp);
	ccp_write_io((void *)&WDT.CTRL, temp);
	wdt_wait_while_busy();
}


/*! \brief Disable Watchdog window mode without changing period settings.
 *
 *  This function disables the WDT window mode without changing period settings.
 *
 *  The function writes the correct signature to the Configuration
 *  Change Protection register before writing the WINCTRL register. Interrupts
 *  are automatically ignored during the change enable period. Disable functions
 *  operate asynchronously with immediate effect.
 *
 *  \retval  true   The WDT was enabled and the window mode is disabled.
 *           false  The WDT (& the window mode) is already disabled.
 */
bool wdt_disable_window_mode(void)
{
	if (!(wdt_is_enabled())) {
		return false;
	}
	uint8_t temp = (WDT.WINCTRL & ~WDT_WEN_bm) | (1 << WDT_WCEN_bp);
	ccp_write_io((void *)&WDT.WINCTRL, temp);

	return true;
}


/*! \brief Enable Watchdog window mode.
 *
 *  This function enables the WDT window mode without changing period settings.
 *
 *  The function writes the correct signature to the Configuration
 *  Change Protection register before writing the WINCTRL register. Interrupts
 *  are automatically ignored during the change enable period. The function will
 *  wait for the WDT to be synchronized to the WDT clock domain before
 *  proceeding
 *
 *  \retval  true   The WDT was enabled and the setting is done.
 *           false  The WDT is disabled and the setting is discarded.
 */
bool wdt_enable_window_mode(void)
{
	if (!(wdt_is_enabled())) {
		return false;
	}
	uint8_t temp = (WDT.WINCTRL & WDT_WPER_gm) |
			(1 << WDT_WEN_bp) | (1 << WDT_WCEN_bp);
	ccp_write_io((void *)&WDT.WINCTRL, temp);
	wdt_wait_while_busy();
	return true;
}


/*! \brief Reset MCU via Watchdog.
 *
 *  This function generates an hardware microcontroller reset using the WDT.
 *
 *  The function loads enables the WDT in window mode. Executing a "wdr" asm
 *  instruction when the windows is closed, provides a quick mcu reset.
 *
 */
void wdt_reset_mcu(void)
{
uint8_t temp;
	/*
	 * WDT enabled (minimum timeout period for max. security)
	 */
	temp = WDT_PER_8CLK_gc | (1 << WDT_ENABLE_bp) | (1 << WDT_CEN_bp);
	ccp_write_io((void *)&WDT.CTRL, temp);
	wdt_wait_while_busy();
	/*
	 * WDT enabled (maximum window period for max. security)
	 */
	temp = WDT_WPER_8KCLK_gc | (1 << WDT_WEN_bp) | (1 << WDT_WCEN_bp);
	ccp_write_io((void *)&WDT.WINCTRL, temp);
	wdt_wait_while_busy();
	/*
	 * WDT Reset during window => WDT generates an Hard Reset.
	 */
	wdt_reset();
	/*
	 * No exit to prevent the execution of the following instructions.
	 */
	while (true) {
		/* Wait for Watchdog reset. */
	}
}
