/**
 * \file
 *
 * \brief Unit tests for SPI driver
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
#include <stdint.h>
#include <stdbool.h>
#include <board.h>
#include <sysclk.h>
#include <spi.h>
#include <string.h>
#include <unit_test/suite.h>
#include <stdio_serial.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the SPI driver.
 * It consists of test cases for the following functionality:
 * - Setting baudrate divisor
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * All AVR XMEGA devices can be used.
 * This example has been tested with the following setup:
 * - Xplain
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions \ref unit_tests.c
 * "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */

//! \name Unit test configuration
//@{
/**
 * \def CONF_TEST_SPI
 * \brief SPI instance to perform unit tests on
 */
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
 * \brief Stopbit configuration of USART
 */
//@}

/**
 * \brief Get SPI module baudrate divisor value
 *
 * \return The divisor value the SPI module is configured to
 */
static uint8_t get_spi_divisor(void)
{
	uint8_t divisor;

	switch (CONF_TEST_SPI.CTRL & SPI_PRESCALER_gm) {
	case SPI_PRESCALER_DIV4_gc:
		divisor = 4;
		break;

	case SPI_PRESCALER_DIV16_gc:
		divisor = 16;
		break;

	case SPI_PRESCALER_DIV64_gc:
		divisor = 64;
		break;

	case SPI_PRESCALER_DIV128_gc:
		divisor = 128;
		break;
	}

	if (CONF_TEST_SPI.CTRL & SPI_CLK2X_bm) {
		divisor >>= 1;
	}

	return divisor;
}

/**
 * \brief Test baudrate setting
 *
 * This test sets different baudrates and test if the divisor is set as
 * expected.
 *
 * \param test Current test case.
 */
static void run_baudrate_set_test(const struct test_case *test)
{
	// A struct list of test sets to perform
	struct test_set {
		uint32_t baudrate;
		uint8_t divisor;
		int8_t ret;
	} test_set[] = {
		// First we test the possible divisors
		{16000000,   2,  1},
		{ 8000000,   4,  1},
		{ 4000000,   8,  1},
		{ 2000000,  16,  1},
		{ 1000000,  32,  1},
		{  500000,  64,  1},
		{  250000, 128,  1},
		// Since SPI drivers implements Assert(clkper_hz > baudrate);
		// UnitTest for very high baudrate removed
		// {32000000,   2,  -1}

		// Test baudrate very close to one higher divisor
		{15999999,   4,  1},
		// Test that too low speed requested fails
		{  100000, 128, -1}
	};
	int8_t ret;
	uint8_t i;

	// Enable SPI clock and module
	sysclk_enable_peripheral_clock(&CONF_TEST_SPI);
	spi_enable(&CONF_TEST_SPI);

	// Loop through the test set and test each case
	for (i = 0; i < (sizeof(test_set) / sizeof(test_set[0])); i++) {
		ret = spi_xmega_set_baud_div(&CONF_TEST_SPI,
				test_set[i].baudrate, 32000000);
		test_assert_true(test, ret == test_set[i].ret,
				"For baudrate %ld got unexpected return value "
				"%d, expected %d",
				test_set[i].baudrate, ret, test_set[i].ret);

		// No need to test divisor value when it fails
		if (ret <= 0) {
			continue;
		}

		test_assert_true(test, get_spi_divisor() == test_set[i].divisor,
				"For baudrate %ld read divisor %d, expected %d",
				test_set[i].baudrate, get_spi_divisor(),
				test_set[i].divisor);
	}
}

/**
 * \brief Run SPI driver unit tests
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};

	sysclk_init();
	board_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	// Define all the test cases
	DEFINE_TEST_CASE(baudrate_set_test, NULL, run_baudrate_set_test, NULL,
			"Baudrate set test");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(spi_tests) = {
		&baudrate_set_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(spi_suite, spi_tests, "XMEGA SPI driver test suite");

	// Run all tests in the test suite
	test_suite_run(&spi_suite);

	while (1) {
		/* Busy-wait forever. */
	}
}
