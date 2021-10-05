/**
 * \file
 *
 * \brief AVR XMEGA TC Driver Example 4
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This example shows how to use the Capture feature of the \ref tc_group
 * module.
 *
 * \section files Main files:
 *  - tc.c Timer XMEGA Timer Counter driver implementation
 *  - tc.h Timer XMEGA Timer Counter driver definitions
 *  - tc_example4.c example application
 *  - conf_example.h: configuration of the example
 *
 * \section driverinfo TC Driver
 * The XMEGA TC driver can be found \ref tc_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with an tc can be used.
 *
 * \section exampledescription Description of the example
 * The example will configure one of the device TC modules in frequency capture
 * mode. The event system is configured to trigger a capture when switch
 * 0 is pressed and released. The resulting value (the interval between press
 * and release) is displayed on the leds. Only the lower 8 bits are displayed.
 *
 * The TC is setup to use a 1000Hz resolution clock and a 32767 period value.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <conf_example.h>
#include <asf.h>

/* Interrupt callback function declaration */
void cca_callback(void);

/**
 * \brief Callback function for TC_CCA interrupt
 *
 * This function reads the result from tc frequency capture and displays the
 * lower 8 bits of the result on leds.
 */
void cca_callback(void)
{
	/* Store the compare channel result */
	uint16_t cca_capture = tc_read_cc(&EXAMPLE_TC, TC_CCA);

	/* The high 8-bits of the result are masked away while the lower 8-bits
	are displayed on the LEDs. */
	LEDPORT.OUT = ~((uint8_t) (cca_capture & 0xff));
}

/**
 * \brief Main application routine
 */
int main(void)
{
	/* Enable all leds */
	LEDPORT.DIRSET = 0xff;

	/* Enable trigger on both edges */
	ioport_configure_port_pin(ioport_pin_to_port(GPIO_PUSH_BUTTON_0),
			ioport_pin_to_mask(GPIO_PUSH_BUTTON_0),
			PORT_ISC_BOTHEDGES_gc | PORT_OPC_PULLUP_gc);

	/* Setup pin0 (sw0) as input to event system channel 0 */
	EVSYS_CH0MUX = EVSYS_CHMUX_EXAMPLEPORT_PIN0_gc;

	/* Enable clock to event sys */
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);

	/* Set resolution */
	tc_set_resolution(&EXAMPLE_TC, TIMER_EXAMPLE_RESOLUTION);

	/* Set period */
	tc_write_period(&EXAMPLE_TC, TIMER_EXAMPLE_PERIOD);

	/* Enable capture channels */
	tc_enable_cc_channels(&EXAMPLE_TC, TC0_CCAEN_bm);

	/* Clear timer interrupts */
	tc_clear_cc_interrupt(&EXAMPLE_TC, TC_CCA);

	/* Do capture on event channel 0 (sw0) */
	tc_set_input_capture(&EXAMPLE_TC, TC_EVSEL_CH0_gc, TC_EVACT_FRQ_gc);

	/* Register callback for interrupt */
	tc_set_cca_interrupt_callback(&EXAMPLE_TC, &cca_callback);

	/* Enable pmic module */
	pmic_init();

	/* Enable CCA interrupt at level low */
	tc_set_cca_interrupt_level(&EXAMPLE_TC, TC_CCAINTLVL_LO_gc);
	pmic_enable_level(PMIC_LOLVLEN_bm);

	/* Enable clock to timer */
	tc_enable(&EXAMPLE_TC);

	/* Enable global interrupts */
	cpu_irq_enable();

	while (1) {
		/* Do nothing */
	}
}
