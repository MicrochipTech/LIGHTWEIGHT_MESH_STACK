/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter Driver Example 2
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This example demonstrates a method for offset and gain calibration for
 * single-ended measurements with the \ref adc_group.
 *
 * \section files Main files:
 * - adc_example2_gfx.c: ADC driver example application
 * - conf_adc.h: configuration of the ADC driver
 * - conf_example.h: configuration of example I/O pins and ADC input
 *
 * \section apiinfo ADC driver API
 * The ADC driver API can be found \ref adc_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with an ADC can be used.
 *
 * \section exampledescription Description of the example
 * ADC B is configured for unsigned, 12-bit conversions using the internal 1 V
 * voltage reference. The conversions are configured for manual triggering.
 * Channel 0 of the ADC is configured for single-ended measurements from the
 * configured input pin (\ref INPUT_PIN). An external voltage supply, capable of
 * giving 0 to 1.0 V, must be connected to this input pin.
 *
 * In addition to the ADC input pin, three I/O pins are used for LED control
 * (\ref LED1_PIN and \ref LED2_PIN) and button input (\ref BUTTON_PIN, active
 * low).
 *
 * When the example starts, the first LED lights up. This indicates that the
 * example is waiting for the external voltage supply to be set to 0 Volts.
 * Once this has been done, the button must be pressed to trigger measurement
 * of the total \a offset error.
 *
 * After this, the second LED lights up. This indicates that the example is
 * waiting for the external voltage supply to be set to 0.9 Volts.
 * Once this has been done, the button must once again be pressed to trigger
 * measurements for \a gain compensation.
 *
 * Oversampling is used to improve the accuracy of the calibration measurements.
 * This means that a \a mean sample value is computed. The amount of
 * oversampling is configurable (see \ref OVERSAMPLING_FACTOR).
 *
 * Once the calibration has been done, both LEDs are turned off and the device
 * continuously samples from the input pin. The calibration data are then used
 * to compute a calibrated sample value according to the following equation
 * (U = voltage, R = ADC result):
 * \f[ U_\textnormal{now} = \frac{(R_\textnormal{now} - R_\textnormal{offset})
 * \cdot R_\textnormal{gain,ideal}}{R_\textnormal{gain,calibration} -
 * R_\textnormal{offset}}
 * \f]
 *
 * The calibrated sample value is stored in the global variable
 * \ref last_sample, which in turn is output to the A3BU Xplained display.
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
 * \def INPUT_PIN
 * \brief ID of ADC input pin
 *
 * This pin is the one used for all measurements with the ADC.
 */

/**
 * \def LED1_PIN
 * \brief ID of LED 1 control I/O pin
 *
 * This pin is driven low when the example is waiting for the external voltage
 * supply to be adjusted to 0 Volts. It is driven high once calibration is done.
 */

/**
 * \def LED2_PIN
 * \brief ID of LED 2 control I/O pin
 *
 * This pin is driven low when the example is waiting for the external voltage
 * supply to be adjusted to 0.9 Volts. It is driven high once calibration is
 * done.
 */

/**
 * \def BUTTON_PIN
 * \brief ID of button input I/O pin
 *
 * This pin is checked for active low status when the example is waiting for
 * user input, i.e., after the external voltage supply has been adjusted.
 */

/**
 * \def OVERSAMPLING_FACTOR
 * \brief Number of calibration samples, as exponent of 2
 *
 * This determines the number of samples used for the oversampling. It is given
 * as an exponent of 2, i.e., setting this factor to 3 gives 2^3 = 8 samples.
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

//@}
#include <conf_example.h>
#include <stdio.h>
#include <asf.h>

/**
 * \brief Ideal ADC result at 0.9 V
 *
 * This factor is given by the following equation:
 * \f[
 * R_\textnormal{ideal}(U) = 2^{12} \cdot \frac{U_\textnormal{measured}}
 * {U_\textnormal{reference}}
 * \f]
 *
 * For a 1 V reference and 0.9 V input, the expected result from an unsigned,
 * 12-bit conversion is 3686.
 *
 * \note To make the example compute the \a voltage in milliVolts, change this
 * we set the factor to 900.
 */
#define IDEAL_GAIN_RESULT    900

//! ADC calibration data.
static struct calibration_data {
	//! Input offset error.
	uint16_t offset;
	//! Gain error.
	uint16_t gain;
} cal_data;

//! Offset- and gain-compensated sample value.
static volatile uint16_t last_sample;

/**
 * \brief Interrupt Service Routine for channel 0 on ADC B
 *
 * Upon completed AD conversion, this ISR computes a calibrated sample from the
 * ADC result, then stores it in a global variable before triggering a new
 * conversion.
 */
ISR(ADCB_CH0_vect)
{
	int32_t sample;
	char out_str[OUTPUT_STR_SIZE];

	sample = adc_get_result(&ADCB, ADC_CH0);

	// Compensate for offset error.
	sample -= cal_data.offset;

	/* Some samples may become negative after compensation due to noise.
	 * Clamp these to 0 since we are doing unsigned conversions.
	 */
	if (sample > 0) {
		// Compensate for gain error.
		sample *= IDEAL_GAIN_RESULT;
		sample /= cal_data.gain;
	} else {
		sample = 0;
	}

	// Store the calibrated sample value.
	last_sample = sample;

	snprintf(out_str, OUTPUT_STR_SIZE, "Voltage: %4d mV", last_sample);

	gfx_mono_draw_string(out_str, 0, 0, &sysfont);

	// Manually start the next conversion.
	adc_start_conversion(&ADCB, ADC_CH0);
}

/**
 * \brief Get mean sample value
 *
 * Performs 2 to the power of \ref OVERSAMPLING_FACTOR successive conversions,
 * and computes the mean value of the resulting sample values.
 *
 * \return Mean sample value.
 */
static uint16_t get_mean_sample_value(void)
{
	uint32_t sum = 0;
	uint16_t i;

	// Sum the configured number of samples.
	for (i = 0; i < (1 << OVERSAMPLING_FACTOR); i++) {
		adc_start_conversion(&ADCB, ADC_CH0);
		adc_wait_for_interrupt_flag(&ADCB, ADC_CH0);

		sum += adc_get_result(&ADCB, ADC_CH0);
	}

	// Compute sample mean by scaling down according to oversampling factor.
	sum >>= OVERSAMPLING_FACTOR;

	return sum;
}

/**
 * \brief Delay for \a ms milliseconds
 *
 * \param ms number of milliseconds to busy wait.
 */
static void mdelay(uint16_t ms)
{
	uint32_t count;

	// Approximate the number of loop iterations needed.
	count = sysclk_get_cpu_hz() / 1000;
	count *= ms;
	count /= 6;

	do {
		asm("");
	} while (--count);
}

/**
 * \brief Wait for button press and release
 *
 * Waits for the button connected to \ref BUTTON_PIN to be pressed and released.
 * Debouncing is done with an approximate 20 ms delay after both the press and
 * the release.
 */
static void wait_for_button(void)
{
	do {} while (ioport_pin_is_high(BUTTON_PIN));
	mdelay(20);
	do {} while (ioport_pin_is_low(BUTTON_PIN));
	mdelay(20);
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
	gfx_mono_init();

	// Enable the back light of the LCD
	ioport_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);

	// Initialize configuration structures.
	adc_read_configuration(&ADCB, &adc_conf);
	adcch_read_configuration(&ADCB, ADC_CH0, &adcch_conf);

	/* Configure the ADC module:
	 * - unsigned, 12-bit results
	 * - bandgap (1 V) voltage reference
	 * - 200 kHz maximum clock rate
	 * - manual conversion triggering
	 */
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);

	adc_write_configuration(&ADCB, &adc_conf);

	/* Configure ADC channel 0:
	 * - single-ended measurement from configured input pin
	 * - interrupt flag set on completed conversion
	 */
	adcch_set_input(&adcch_conf, INPUT_PIN, ADCCH_NEG_NONE,
			1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_interrupt(&adcch_conf);

	adcch_write_configuration(&ADCB, ADC_CH0, &adcch_conf);

	// Enable the ADC and do one dummy conversion.
	adc_enable(&ADCB);
	adc_start_conversion(&ADCB, ADC_CH0);
	adc_wait_for_interrupt_flag(&ADCB, ADC_CH0);

	// Light up LED 1, wait for button press.
	ioport_set_pin_low(LED1_PIN);
	wait_for_button();

	// Perform oversampling of offset.
	cal_data.offset = get_mean_sample_value();

	// Light up LED 2, wait for button press.
	ioport_set_pin_low(LED2_PIN);
	wait_for_button();

	// Perform oversampling of 0.9 V for gain calibration.
	cal_data.gain = get_mean_sample_value() - cal_data.offset;

	// Turn off LEDs.
	ioport_set_pin_high(LED1_PIN);
	ioport_set_pin_high(LED2_PIN);

	// Enable interrupts on ADC channel, then trigger first conversion.
	adcch_enable_interrupt(&adcch_conf);
	adcch_write_configuration(&ADCB, ADC_CH0, &adcch_conf);
	adc_start_conversion(&ADCB, ADC_CH0);

	do {
		// Sleep until ADC interrupt triggers.
		sleepmgr_enter_sleep();
	} while (1);
}
