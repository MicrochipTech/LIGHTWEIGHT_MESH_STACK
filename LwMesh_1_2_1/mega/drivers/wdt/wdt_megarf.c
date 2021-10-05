/**
 * \file
 *
 * \brief AVR MEGARF WatchDog Timer driver.
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

#include "wdt_megarf.h"

/* ! \internal Local storage of WDT timer interrupt callback function */
static wdt_callback_t wdt_timer_callback;

/**
 * \internal
 * \brief Interrupt handler for WDT timer overflow
 *
 * This function will handle interrupt on WDT Timer overflow and
 * call the callback function.
 */

ISR(WDT_vect)
{
	if (wdt_timer_callback) {
		wdt_timer_callback();
	}
}

/**
 * \brief Disable Watchdog.
 *
 *  This function disables the WDT without changing period settings.
 *  This function is written in asm where ever the time is critical
 */
void wdt_disable(void)
{
	/* Disable Global interrupt */
	uint8_t sreg = cpu_irq_save();

	/* Reset Watchdog timer */
	wdt_reset();

	/* Clear WDRF flag in MCUSR */
	wdt_reset_flag_clear();

	/* Write logical one to WDCE and WDE to keep old prescale setting */
	asm ("LDS R17,0x60");   /* WDTCSR Address = 0x60 */
	asm ("ORI R17,0x18");
	asm ("LDI R18,0x00");
	asm ("STS 0x60,R17");   /* WDTCSR Address = 0x60 */
	/* Disable WDT */
	asm ("STS 0x60,R18");   /* WDTCSR Address = 0x60 */

	/* Restore Global interrupt */
	cpu_irq_restore(sreg);
}

/**
 * \brief Set Watchdog timeout period.
 *
 *  This function sets the coded field of the WDT timeout period.
 *  This function is written in asm where ever the time is critical
 *
 *  \param  to_period  WDT timeout period
 */
void wdt_set_timeout_period(enum wdt_timeout_period to_period)
{
	/* Store the prescale value to temp register */
#if defined (__GNUC__)
	asm ("MOV R19,R24");
#elif defined(__ICCAVR__)
	asm ("MOV R19,R16");
#else
#error Unsupported compiler.
#endif

	/* Mask for WDP3 */
	if (to_period & MASK_PRESCALE_WPD3) {
		asm ("LDI R21,0x20");
	} else {
		asm ("LDI R21,0x00");
	}

	/* Disable Global interrupt */
	uint8_t sreg = cpu_irq_save();

	/* Reset Watchdog timer */
	wdt_reset();

	asm ("LDI R17,0xD8");
	asm ("LDS R18,0x60");   /* WDTCSR Address = 0x60 */
	asm ("AND R17,R18");
	asm ("STS 0x60,R17");   /* WDTCSR Address = 0x60 */
	/* Load the new prescale value */
	asm ("LDI R20,0x18");
	asm ("LDI R18,0x07");
	asm ("AND R19,R18");
	asm ("OR R19,R21");
	asm ("OR R19,R17");
	/* Write logical one to WDCE and WDE */
	asm ("STS 0x60,R20");   /* WDTCSR Address = 0x60 */
	/* Write new prescale setting */
	asm ("STS 0x60,R19");   /* WDTCSR Address = 0x60 */

	/* Restore Global interrupt */
	cpu_irq_restore(sreg);
}

/**
 * \brief Enable WDT at different modes
 *
 * This function allows WDT to enable. Using this function we can
 * select either Interrupt Mode or System reset mode or Interrupt
 * and System reset mode.
 * This function is written in asm where ever the time is critical
 *
 * \param mode WDT timer mode selection
 */
void wdt_enable(enum wdt_mode_select mode)
{
	/* Disable Global interrupt */
	uint8_t sreg = cpu_irq_save();

	/* Reset Watchdog timer */
	wdt_reset();

	/* Clear WDRF flag in MCUSR */
	wdt_reset_flag_clear();

	/*System reset mode */
	if (mode == SYSTEM_RESET_MODE) {
		/* Write logical zero to WDIE */
		asm ("LDI R17,0xBF");
		asm ("LDS R18,0x60"); /* WDTCSR Address = 0x60 */
		asm ("AND R17,R18");
		asm ("STS 0x60,R17"); /* WDTCSR Address = 0x60 */
		/* Write WDIF,WDE and WDCE to logical one */
		asm ("LDI R18,0x98");
		asm ("OR R18,R17");
		asm ("STS 0x60,R18"); /* WDTCSR Address = 0x60 */
	}
	/* Interrupt mode */
	else if (mode == INTERRUPT_MODE) {
		/* Write logical zero to WDE */
		asm ("LDI R17,0xF7");
		asm ("LDS R18,0x60"); /* WDTCSR Address = 0x60 */
		asm ("AND R17,R18");
		asm ("STS 0x60,R17"); /* WDTCSR Address = 0x60 */
		asm ("LDI R20,0x18");
		asm ("LDI R19,0xD0");
		asm ("OR R19,R17");
		/* Write logical one to WDCE and WDE */
		asm ("STS 0x60,R20"); /* WDTCSR Address = 0x60 */
		/* Write WDIF,WDIE and WDCE to logical one */
		asm ("STS 0x60,R19"); /* WDTCSR Address = 0x60 */
	}
	/* Interrupt and System reset mode */
	else if (mode == INTERRUPT_SYSTEM_RESET_MODE) {
		/* Write logical zero to WDE */
		asm ("LDI R17,0xF7");
		asm ("LDS R18,0x60"); /* WDTCSR Address = 0x60 */
		asm ("AND R17,R18");
		asm ("STS 0x60,R17"); /* WDTCSR Address = 0x60 */
		/* Write logical one to WDCE, WDIE,WDIF and WDE */
		asm ("LDI R18,0xD8");
		asm ("OR R18,R17");
		asm ("STS 0x60,R18"); /* WDTCSR Address = 0x60 */
	}

	/* Restore Global interrupt */
	cpu_irq_restore(sreg);
}

/**
 * \brief Set WDT timer overflow interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param callback Reference to a callback function
 */
void wdt_set_interrupt_callback(wdt_callback_t callback)
{
	wdt_timer_callback = callback;
}

/**
 * *\brief Reset MCU via Watchdog.
 *
 *  This function generates an hardware microcontroller reset using the WDT.
 *
 *  The function loads enables the WDT in system reset mode.
 */
void wdt_reset_mcu(void)
{
	/*
	 * Set minimum timeout period
	 */
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_2KCLK);

	/*
	 * WDT enabled
	 */
	wdt_enable(SYSTEM_RESET_MODE);

	/*
	 * WDT Reset
	 */
	wdt_reset();

	/*
	 * No exit to prevent the execution of the following instructions.
	 */
	while (true) {
		/* Wait for Watchdog reset. */
	}
}
