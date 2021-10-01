/**
 * \file
 *
 * \brief Unit tests for XMEGA timeout service
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
#include <asf.h>
#include <compiler.h>
#include <stdint.h>
#include <stdbool.h>
#include <board.h>
#include <sysclk.h>
#include <string.h>
#include <unit_test/suite.h>
#include <stdio_serial.h>
#include <conf_test.h>
#include <conf_timeout.h>
#include <timeout.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the \ref timeout_group. It will
 * test both singleshot and periodic timeouts.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_rtc.h
 * - \ref conf_timeout.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * All AVR XMEGA devices with RTC can be used
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions
 * \ref unit_tests.c "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref timeout_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 * Support and FAQ: https://www.microchip.com/support/
 */

//! \name Unit test configuration
//@{
/**
 * \def CONF_TEST_USART
 * \brief USART to redirect STDIO to
 */
/**
 * \def CONF_TEST_BAUDRATE
 * \brief Baudrate of USART
 */
/**
 * \def CONF_TEST_CHARLENGTH
 * \brief Character length (bits) of USART
 */
/**
 * \def CONF_TEST_PARITY
 * \brief Parity mode of USART
 */
/**
 * \def CONF_TEST_STOPBITS
 * \brief Stop bit configuration of USART
 */
//@}

//! Accuracy of timeout clock source (due to 32kHz ULP oscillator)
const float TIMEOUT_ACCURACY = 1.3;

//! Enum with index ID for all timeout channels
enum {
	TEST_TIMEOUT_0,
	TEST_TIMEOUT_1,
	TEST_TIMEOUT_2,
};

//! Variable to keep track of test results
static bool success = false;

static void mdelay(uint16_t ms)
{
	uint32_t count;

	// Approximate the number of loop iterations needed.
	count = sysclk_get_cpu_hz() / 6;
	count *= ms;
	count /= 1000;

	do {
		asm("");
	} while (--count);
}

/**
 * \brief Run timeout initialization test
 *
 * This test verifies that no timeouts are running after initialization
 *
 * \param test Current test case.
 */
static void test_initialization(const struct test_case *test)
{
	success = true;
	uint8_t i;

	// Check if the timeout channels are initialized (not running)
	for (i = 0; i < TIMEOUT_COUNT; i++) {
		if(timeout_test_and_clear_expired(i))
			success = false;
	}

	test_assert_true(test, success, "Timeout initialization failed");
}

/**
 * \brief Run timeout singleshot test
 *
 * This test start three different singleshot timeouts and verify that
 * they timeout when expected.
 *
 * \param test Current test case.
 */
static void test_singleshot_timeout
		(const struct test_case *test)
{
	success = true;

	// Start three timeouts (100ms, 200ms, 400ms - 10ms pr tick)
	timeout_start_singleshot(TEST_TIMEOUT_0, 10);
	timeout_start_singleshot(TEST_TIMEOUT_1, 20);
	timeout_start_singleshot(TEST_TIMEOUT_2, 40);

	// Verify all timeouts are running
	for (uint8_t i = 0; i < TIMEOUT_COUNT; i++ ) {
		if (timeout_test_and_clear_expired(i))
			success = false;
	}

	// Delay 100ms
	mdelay((int)(100 * TIMEOUT_ACCURACY));

	// Verify that only timeout_0 has expired
	if (!timeout_test_and_clear_expired(TEST_TIMEOUT_0) ||
			timeout_test_and_clear_expired(TEST_TIMEOUT_1) ||
			timeout_test_and_clear_expired(TEST_TIMEOUT_2)) {
		success = false;
	}

	// Delay 100ms
	mdelay((int)(100 * TIMEOUT_ACCURACY));

	// Verify that timeout_1 has expired and timeout_2 is still running
	if (!timeout_test_and_clear_expired(TEST_TIMEOUT_1) ||
			 timeout_test_and_clear_expired(TEST_TIMEOUT_2)) {
		success = false;
	}

	// Delay 200ms
	mdelay((int)(200 * TIMEOUT_ACCURACY));

	// Verify that timeout_2 has expired
	if (!timeout_test_and_clear_expired(TEST_TIMEOUT_2))
		success = false;

	test_assert_true(test, success,"Singleshot test failed");
}

/**
 * \brief Periodic timeout test
 *
 * This test start one periodic timeout and verifies that the timeout
 * occurs as expected 10 times.
 *
 * \param test Current test case.
 */
static void test_periodic_timeout(const struct test_case *test)
{
	success = true;

	//Start a timeout with 20Hz period
	timeout_start_periodic(TEST_TIMEOUT_0, 5);

	// Test 10 timeout periods
	for (uint8_t i = 0; i < 10; i++ ) {

		// Verify that timeout has not expired
		if(timeout_test_and_clear_expired(TEST_TIMEOUT_0))
			success = false;

		// Delay 50ms
		mdelay((int)(50 * TIMEOUT_ACCURACY));

		// Verify that timeout has expired
		if(!timeout_test_and_clear_expired(TEST_TIMEOUT_0))
			success = false;
	}
	test_assert_true(test, success,"Periodic test failed");
}

/**
 * \brief Run XMEGA timeout service unit tests
 */
int main (void)
{
	const usart_serial_options_t usart_serial_options =
	{
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};

	sysclk_init();
	board_init();
	pmic_init();
	timeout_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);
	cpu_irq_enable();

	// Define all the test cases
	DEFINE_TEST_CASE(timeout_test1, NULL, test_initialization, NULL,
			"Testing timeout initialization");

	DEFINE_TEST_CASE(timeout_test2, NULL, test_singleshot_timeout, NULL,
			"Testing singleshot timeout");


	DEFINE_TEST_CASE(timeout_test3, NULL, test_periodic_timeout, NULL,
			"Testing periodic timeout");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(timeout_tests) = {
		&timeout_test1,
		&timeout_test2,
		&timeout_test3,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(timeout_suite, timeout_tests,
			"XMEGA timeout service test suite");

	// Run all tests in the test suite
	test_suite_run(&timeout_suite);

	while (1) {
		// Loop for infinity
	}
}
