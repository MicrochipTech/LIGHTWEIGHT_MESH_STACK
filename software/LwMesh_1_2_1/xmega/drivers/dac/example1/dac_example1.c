/**
 * \file
 *
 * \brief AVR XMEGA Digital to Analog Converter Driver Example 1
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 * This example shows one way to use the \ref dac_group for outputting a sample
 * buffer at a constant conversion rate.
 *
 * \section files Main files:
 * - dac_example1.c: DAC driver example application
 * - conf_board.h: board initialization configuration
 * - conf_example.h: example application configuration
 * - conf_clock.h: system clock configuration
 *
 * \section apiinfo DAC driver API
 * The DAC driver API can be found \ref dac_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with a DAC can be used.
 *
 * \section exampledescription Description of the example
 * This example sets up the configured DAC (\ref SPEAKER_DAC) to perform DA
 * conversions on one channel (\ref SPEAKER_DAC_CHANNEL), with output to a pin,
 * every time it receives events in event channel 0. Timer/counter 0 on port C
 * is used to generate the events on overflow. This setup gives a constant
 * conversion rate.
 *
 * The main loop in the example waits for the DAC channel to get ready for new
 * data, then feeds it with a new sample from a sample buffer that contains one
 * period of a sine wave. The DAC channel is not flagged as ready for new data
 * until a conversion has completed, i.e., after the DAC has received an event.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 */

//! \name Example configuration
//@{

/**
 * \def SPEAKER_DAC
 * \brief DAC to use in example
 */

/**
 * \def SPEAKER_DAC_CHANNEL
 * \brief DAC channel to use in example
 *
 * The DAC channel is output on its corresponding pin. This pin may, e.g., be
 * connected to an oscilloscope or an amplifier and speaker.
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

//@}

#include <conf_example.h>
#include <asf.h>

//! Rate at which DA conversions should be done.
#define RATE_OF_CONVERSION    22050

//! Number of samples in sample buffer.
#define NR_OF_SAMPLES         32

//! Sample buffer with one period of a sine wave.
static const uint16_t sine[NR_OF_SAMPLES] = {
	32768, 35325, 37784, 40050, 42036, 43666, 44877, 45623,
	45875, 45623, 44877, 43666, 42036, 40050, 37784, 35325,
	32768, 30211, 27752, 25486, 23500, 21870, 20659, 19913,
	19661, 19913, 20659, 21870, 23500, 25486, 27752, 30211,
};

int main(void)
{
	struct dac_config conf;
	uint8_t           i = 0;

	board_init();
	sysclk_init();

	// Initialize the dac configuration.
	dac_read_configuration(&SPEAKER_DAC, &conf);

	/* Create configuration:
	 * - 1V from bandgap as reference, left adjusted channel value
	 * - one active DAC channel, no internal output
	 * - conversions triggered by event channel 0
	 * - 1 us conversion intervals
	 */
	dac_set_conversion_parameters(&conf, DAC_REF_BANDGAP, DAC_ADJ_LEFT);
	dac_set_active_channel(&conf, SPEAKER_DAC_CHANNEL, 0);
	dac_set_conversion_trigger(&conf, SPEAKER_DAC_CHANNEL, 0);
#if XMEGA_DAC_VERSION_1
	dac_set_conversion_interval(&conf, 1);
#endif
	dac_write_configuration(&SPEAKER_DAC, &conf);
	dac_enable(&SPEAKER_DAC);
	
#if XMEGA_E
	// Configure timer/counter to generate events at sample rate.
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC4);
	TCC4.PER = (sysclk_get_per_hz() / RATE_OF_CONVERSION) - 1;

	// Configure event channel 0 to generate events upon T/C overflow.
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC4_OVF_gc;

	// Start the timer/counter.
	TCC4.CTRLA = TC45_CLKSEL_DIV1_gc;
#else
	// Configure timer/counter to generate events at sample rate.
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC0);
	TCC0.PER = (sysclk_get_per_hz() / RATE_OF_CONVERSION) - 1;

	// Configure event channel 0 to generate events upon T/C overflow.
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;

	// Start the timer/counter.
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;
#endif

	/* Write samples to the DAC channel every time it is ready for new
	 * data, i.e., when it is done converting. Conversions are triggered by
	 * the timer/counter.
	 */
	do {
		dac_wait_for_channel_ready(&SPEAKER_DAC, SPEAKER_DAC_CHANNEL);

		dac_set_channel_value(&SPEAKER_DAC, SPEAKER_DAC_CHANNEL, sine[i]);

		i++;
		i %= NR_OF_SAMPLES;
	} while (1);
}
