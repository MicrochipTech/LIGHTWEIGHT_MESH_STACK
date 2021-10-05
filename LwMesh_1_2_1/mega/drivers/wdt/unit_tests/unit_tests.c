/**
 * \file
 *
 * \brief Unit tests for Watchdog timer module
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

#include <conf_test.h>
#include <asf.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the \ref wdt_group.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * All MEGARF devices can be used.
 * This example has been tested with the following setup:
 * - atmega128rfa1_stk600-rc128x_rfx
 * - atmega256rfr2_atmega256rfr2_xplained_pro
 *
 * \section description Description of the unit tests
 * The application will set up a watchdog timer in different modes
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref megarf_wdt_group
 * - \ref reset_cause_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other
 * compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information,
 * visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */

/* Macros for success/fail indication */
#define SUCCESS    1
#define FAIL       0

/* ! \brief Flag to indicate watchdog timeout */
static volatile bool wdt_rc, wdt_int;

/**
 * \brief Interrupt callback for wdt timer overflow interrupt
 *  Change the state when the interrupt is triggered
 *  \param none
 */
static void wdt_timer_callback(void)
{
	wdt_int = SUCCESS;
}

/**
 * \brief Configure WDT in reset mode
 *
 * \param none.
 */
static void wdt_reset_mode_enable(void)
{
	/* Check if we are reset by WDT. If not, do a WDT reset */
	if (!wdt_rc) {
		/* Disable WDT */
		wdt_disable();

		/* Enable WDT 250 ms. */
		wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_32KCLK);

		/* Enable wdt in reset mode */
		wdt_enable(SYSTEM_RESET_MODE);

		while (1) {
		}
	}
}

/**
 * \brief Run test to check last reset cause
 *
 * This test will pass if last reset was caused by a watchdog timeout.
 *
 * \param test Current test case.
 */
static void run_reset_cause_test(const struct test_case *test)
{
	test_assert_true(test, wdt_rc, "Watchdog reset failed");
}

/**
 * \brief Run test to check interrupt is getting triggered when WDT timout
 * ouccured and system reset will not happen since refresh wdt is done.
 *
 * \param test Current test case.
 */
static void run_interrupt_reset_test(const struct test_case *test)
{
	volatile bool status;

	status = SUCCESS;

	/* Disable WDT */
	wdt_disable();

	/* Enable WDT 250 ms. */
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_32KCLK);

	/* Check for timeout period is set properly */
	if (wdt_get_timeout_period() != WDT_TIMEOUT_PERIOD_32KCLK) {
		status = FAIL;
	}

	test_assert_true(test, status, "Watchdog period setting failed");

	/* Put the interrupt callback */
	wdt_set_interrupt_callback(wdt_timer_callback);

	/* Enable WDT in interrupt reset mode */
	wdt_enable(INTERRUPT_SYSTEM_RESET_MODE);

	/* Check for wdt interrupt mode is enabled */
	if (wdt_reset_interrupt_mode_enabled() == false) {
		status = FAIL;
	}

	test_assert_true(test, status, "Watchdog interrupt enable failed");

	/* Clear the status flag */
	wdt_int = FAIL;

	cpu_irq_disable();

	/* Check the interrupt flag is triggered after the timeout */
	while (!wdt_timeout_interrupt_flag_is_set()) {
	}

	wdt_int = SUCCESS;

	/* Clear the interrupt flag */
	wdt_timeout_interrupt_flag_clear();

	/* Refresh watchdog before reset happens */
	wdt_reset();

	wdt_disable();

	test_assert_true(test, wdt_int, "Watchdog interrupt reset mode failed");
}

/**
 * \brief Run test to check interrupt is getting triggered when WDT timout
 * ouccured.
 *
 * This test will pass if ISR is serviced which is caused by a watchdog timeout.
 *
 * \param test Current test case.
 */
static void run_wdt_interrupt_test(const struct test_case *test)
{
	volatile bool status = SUCCESS;

	/* Enable WDT 125 ms. */
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_16KCLK);

	/* Put the interrupt callback */
	wdt_set_interrupt_callback(wdt_timer_callback);

	/* Enable WDT in interrupt mode */
	wdt_enable(INTERRUPT_MODE);

	/* Check for timeout period is set properly */
	if (wdt_get_timeout_period() != WDT_TIMEOUT_PERIOD_16KCLK) {
		status = FAIL;
	}

	test_assert_true(test, status, "Watchdog period setting failed");

	/* Check for wdt interrupt mode is enabled */
	if (wdt_interrupt_mode_enabled() == false) {
		status = FAIL;
	}

	test_assert_true(test, status, "Watchdog interrupt enable failed");

	/* Clear the status flag */
	wdt_int = FAIL;

	cpu_irq_enable();

	/* Wait for WDT Timeout interrupt to get triggered */
	delay_ms(175);

	/* Disable WDT */
	wdt_disable();

	test_assert_true(test, wdt_int, "Watchdog interrupt timeout failed");
}

/* ! \brief Set up and run test suite */
int main(void)
{
	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */

	sysclk_init();
	board_init();

	/* USART init values */
	const usart_serial_options_t usart_serial_options = {
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};

	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Detection of all RESET except WDT RESET. */
	if ((reset_cause_get_causes() & CHIP_RESET_CAUSE_WDT)
			!= CHIP_RESET_CAUSE_WDT) {
		reset_cause_clear_causes(CHIP_RESET_CAUSE_POR |
				CHIP_RESET_CAUSE_EXTRST |
				CHIP_RESET_CAUSE_BOD_CPU);

		wdt_rc = false;
	} else {
		reset_cause_clear_causes(CHIP_RESET_CAUSE_WDT);
		wdt_rc = true;
	}

	wdt_disable();

	/* Enbale WDT in reset mode */
	wdt_reset_mode_enable();

	/* Define all the test cases */
	DEFINE_TEST_CASE(rc_test, NULL, run_reset_cause_test, NULL,
			"Confirming watchdog reset");
	DEFINE_TEST_CASE(interrupt_mode_test, NULL, run_wdt_interrupt_test,
			NULL,
			"Running WDT Interrupt mode test");
	DEFINE_TEST_CASE(int_rst_mode_test, NULL, run_interrupt_reset_test,
			NULL,
			"Running WDT Interrupt Reset mode test");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(wdt_tests) = {
		&rc_test,
		&interrupt_mode_test,
		&int_rst_mode_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(wdt_suite, wdt_tests, "MEGARF WDT driver test suite");

	/* Run all tests in the test suite */
	test_suite_run(&wdt_suite);

	while (1) {
		/* Intentionally left empty. */
	}
}
