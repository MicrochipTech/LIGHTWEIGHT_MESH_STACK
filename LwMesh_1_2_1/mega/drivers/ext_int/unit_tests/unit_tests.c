/**
 * \file
 *
 * \brief Unit tests for MegaRF Ext Interrupt driver
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

#include <asf.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test for the MEGARF Ext Interrupt driver. Ext interrupt is 
 * getting trigged by changing the pin status on external pin change interrupt pins.
 * This is being simulated by connecting one of the GPIO port pin configured as output
 * to external pin change interrupt pin using jumper wire. By outputing high or low 
 * on GPIO pin the status of external pin change interrupt pin connected to that GPIO
 * pin will be changing and hence interrupt will get triggered.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * This example has been tested with the following setup(s):
 * - atmega128rfa1_stk600 and atmega256rfr2_xplained_pro
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions
 * \ref unit_tests.c "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref megarf_usart_group
 * - \ref megarf_interrupt_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */

/* IOPort pin configured as output */
#define CONF_OUT_PIN   IOPORT_CREATE_PIN(PORTE,1)

/* Definition for Level High */
#define HIGH   0x01

/* Definition for Level High */
#define LOW    0x00

/* PC_INT8_PIN Port Pin position */
#define PC_INT8_PIN_PORT_POS    0x00

/* Variable for trigger counter **/
static volatile uint8_t trigger_count=0;

/**
 * \brief Interrupt callback for External PC interrupt
 *  Increments the trigger counter when the interrupt is triggered
 *  \param none
 */
static void ext_pc_int_callback(void)
{
	/* Increment the trigger count */
	trigger_count++;
	
}

/**
 * \brief Test External interrupt is getting triggered.
 *
 * This function enables the pin change interrupt(PE0) and changes the status of the
 * pin by outputting high/low on GPIO pin (PE1) which in turn connected to PC 
 * interrupt pin
 *
 * \param test Current test case.
 */
static void run_interrupt_trigger_test(const struct test_case *test)
{
	
	/* Disable Global interrupt */
	cpu_irq_disable();
	
	/* Configure I/O Port pin as output */
	ioport_configure_pin(CONF_OUT_PIN, IOPORT_DIR_OUTPUT);

	/* Set Interrupt CallBack Function **/
	ext_int_set_interrupt_callback(CONF_PC_INT,ext_pc_int_callback);
	
	/* Enable the Ext PC Int */
	ext_int_pcint_init(CONF_PC_INT);
	
	/* Output Low on the IOPORT pin during the init */
	ioport_set_pin_level(CONF_OUT_PIN,LOW);

	/* Enable Global interrupt */
	cpu_irq_enable();
	
	/* Clear the counter value */
	trigger_count =0;
	
	/* Output High on the IOPORT pin */
	ioport_set_pin_level(CONF_OUT_PIN,HIGH);
	
	/* Wait for the ISR to get serviced */
	for(volatile uint16_t delay=0;delay<10000;delay++);
	
	/* Output Low on the IOPORT pin */
	ioport_set_pin_level(CONF_OUT_PIN,LOW);
	
	/* Wait for the ISR to get serviced */
	for(volatile uint16_t delay=0;delay<10000;delay++);
	
	/* Output Low on the IOPORT pin */
	ioport_set_pin_level(CONF_OUT_PIN,LOW);
		
	/* Wait for the ISR to get serviced */
	for(volatile uint16_t delay=0;delay<10000;delay++);
	
	/* Disable Global interrupt */
	cpu_irq_disable();

	test_assert_true(test, trigger_count == 2,
			"Ext Pin change interrupt trigger failed.");		
}

/**
 * \brief Test setting different parameters of the Ext_Int module
 *
 * This function calls the different set functions, and verifies that the
 * correct values are being set.
 *
 * \param test Current test case.
 */
static void run_set_functions_test(const struct test_case *test)
{
	bool success;
     
	/* Test enabling and disabling external interrupt */
	ext_int_enable(CONF_EXT_INT);
	success = (EIMSK & INT_PIN_MASK(CONF_EXT_INT));
	test_assert_true(test, success, "Trying to enable Ext interrupt failed.");

	ext_int_disable(CONF_EXT_INT);
	success = !(EIMSK & INT_PIN_MASK(CONF_EXT_INT));
	test_assert_true(test, success, "Trying to disable Ext interrupt failed.");

	/* Test enabling and disabling ext pin change interrupt */
	ext_int_pcint_enable(CONF_PC_INT);
	success = (PCMSK1 & INT_PIN_MASK(PC_INT8_PIN_PORT_POS));
	test_assert_true(test, success, "Trying to enable Ext PC interrupt failed.");

	ext_int_pcint_disable(CONF_PC_INT);
	success = !(PCMSK1 & INT_PIN_MASK(PC_INT8_PIN_PORT_POS));
	test_assert_true(test, success, "Trying to disable Ext PC interrupt failed.");
	
	/* Test clear ext interrupt flag */
	ext_int_clear_flag(CONF_EXT_INT);
	success = !(EIFR & (1 << (CONF_EXT_INT & EXT_INT_PORT_MASK)));
	test_assert_true(test, success, "Trying to clear Ext interrupt flag failed.");

	/* Test clear ext pin change interrupt flag */
	ext_int_pcint_clear_flag(CONF_PC_INT);
	success = !(PCIFR & (1 << PCIF1));
	test_assert_true(test, success, "Trying to clear Ext PC interrupt flag failed.");
	
}

/**
 * \brief Run Ext Interrupt unit tests
 *
 * Initializes the clock system, board and serial output, then sets up the
 * Ext Interrupt unit test suite and runs it.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};

	board_init();
	sysclk_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	DEFINE_TEST_CASE(interrupt_trigger_test, NULL, run_interrupt_trigger_test, NULL,
			"Test external interrupt is getting triggered");
	DEFINE_TEST_CASE(set_functions_test, NULL, run_set_functions_test, NULL,
			"Test setting of various properties");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(ext_int_tests) = {
		&interrupt_trigger_test,
		&set_functions_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(ext_int_suite, ext_int_tests,
			"MEGARF External Interrupt driver test suite");

	/* Run all tests in the suite */
	test_suite_run(&ext_int_suite);

	while (1) {
		/* Intentionally left empty. */
	}
}
