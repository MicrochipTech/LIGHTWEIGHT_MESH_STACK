/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter Driver Example 1
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
 * This simple example shows how to use the \ref adc_group for temperature
 * measurement with the internal sensor.
 *
 * \section files Main files:
 * - adc_example1.c: ADC driver example application
 * - conf_adc.h: configuration of the ADC driver
 *
 * \section apiinfo ADC driver API
 * The ADC driver API can be found \ref adc_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with an ADC can be used.
 *
 * \section exampledescription Description of the example
 * ADC A is configured for unsigned, 12-bit conversions using the internal 1 V
 * voltage reference. The conversions are configured for manual triggering.
 * Channel 0 of the ADC is configured for single-ended measurements from the
 * internal temperature sensor.
 *
 * An ADC interrupt callback function is used to handle the conversion results,
 * from which the current temperature, in kelvin, is computed. The computation
 * requires the factory calibration measurement, and is done according to the
 * following formula (T = temperature, R = ADC result):
 * \f[ T_\textnormal{now} = \frac{(R_\textnormal{now} \cdot
 * T_\textnormal{calibration})}{R_\textnormal{calibration}}
 * \f]
 *
 * The computed temperature is stored in the global variable
 * \ref last_temperature, which can, e.g., be watched during debugging.
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
#include <asf.h>

//! Temperature sensor calibration data.
static uint16_t tempsense;

//! The latest, computed temperature.
static volatile uint16_t last_temperature;

/**
 * \brief Callback function for ADC interrupts
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask ADC channel mask.
 * \param result Conversion result from ADC channel.
 */
static void adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result)
{
	uint32_t temperature;

	/* Compute current temperature in kelvin, based on the factory
	 * calibration measurement of the temperature sensor. The calibration
	 * has been done at 85 degrees Celsius, which corresponds to 358 kelvin.
	 */
	temperature = (uint32_t)result * 358;
	temperature /= tempsense;

	// Store temperature in global variable.
	last_temperature = temperature & 0xffff;

	// Start next conversion.
	adc_start_conversion(adc, ch_mask);
}

int main(void)
{
	struct adc_config         adc_conf;
	struct adc_channel_config adcch_conf;

	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize configuration structures.
	adc_read_configuration(&ADCA, &adc_conf);
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);

	/* Configure the ADC module:
	 * - unsigned, 12-bit results
	 * - bandgap (1 V) voltage reference
	 * - 200 kHz maximum clock rate
	 * - manual conversion triggering
	 * - temperature sensor enabled
	 * - callback function
	 */
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_enable_internal_input(&adc_conf, ADC_INT_TEMPSENSE);

	adc_write_configuration(&ADCA, &adc_conf);
	adc_set_callback(&ADCA, &adc_handler);

	/* Configure ADC channel 0:
	 * - single-ended measurement from temperature sensor
	 * - interrupt flag set on completed conversion
	 * - interrupts disabled
	 */
	adcch_set_input(&adcch_conf, ADCCH_POS_TEMPSENSE, ADCCH_NEG_NONE,
			1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_enable_interrupt(&adcch_conf);

	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	// Get measurement for 85 degrees C (358 kelvin) from calibration data.
	tempsense = adc_get_calibration_data(ADC_CAL_TEMPSENSE);

	// Enable the ADC and start the first conversion.
	adc_enable(&ADCA);
	adc_start_conversion(&ADCA, ADC_CH0);

	do {
		// Sleep until ADC interrupt triggers.
		sleepmgr_enter_sleep();
	} while (1);
}
