/**
 * \file
 *
 * \brief XMEGA-B1-XPLAINED board header file.
 *
 * This file contains definitions and services related to the features of the
 * XMEGA-B1-XPLAINED board.
 *
 * To use this board, define BOARD=XMEGA_B1_XPLAINED.
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

#ifndef _XMEGA_B1_XPLAINED_H_
#define _XMEGA_B1_XPLAINED_H_

#include "compiler.h"
#include "conf_board.h"
#include "gpio.h"


/*! \name Miscellaneous data
 */
//! @{

//! Validate board support for the common sensor service.

#define COMMON_SENSOR_PLATFORM

/*! \name Voltages
 */
//! @{
#define	VCC_TARGET      3.30
#define	VCC_ANA         3.30
#define	VCC_USB         5.00
//! @}

/*! \name ADC Connection of the Temperature Sensor
 */
//! @{
#define TEMPERATURE_SENSOR_ADC_MODULE    ADCB
#define TEMPERATURE_SENSOR_ADC_INPUT     ADCCH_POS_PIN0
#define TEMPERATURE_SENSOR_SIGNAL_PIN    IOPORT_CREATE_PIN(PORTB,0)
//! @}

/*! \name ADC Connection of the Light Sensor
 */
//! @{
#define LIGHT_SENSOR_ADC_MODULE         ADCB
#define LIGHT_SENSOR_ADC_INPUT          ADCCH_POS_PIN2
#define LIGHT_SENSOR_SIGNAL_PIN         IOPORT_CREATE_PIN(PORTB,2)
//! @}

/*! \name ADC Connection of the Potentiometer
 */
//! @{
#define POTENTIOMETER_ADC_MODULE         ADCB
#define POTENTIOMETER_ADC_INPUT          ADCCH_POS_PIN1
#define POTENTIOMETER_SIGNAL_PIN         IOPORT_CREATE_PIN(PORTB,1)
//! @}

/*! \name ADC Connection of the External Voltage Input
 */
//! @{
#define EXT_VIN_ADC_MODULE         ADCB
#define EXT_VIN_ADC_INPUT          ADCCH_POS_PIN3
#define EXT_VIN_SIGNAL_PIN         IOPORT_CREATE_PIN(PORTB,3)
//! @}

/*! \name Common ADC module used for on-board sensor.
 */
//! @{
#define BOARD_SENSOR_ADC_MODULE         ADCB
//! @}

/*! \name USER LEDs
 */
//! @{
#define	USERLEDPORT     PORTB
#define	USERLEDMASK     (0xF0)
#define	USERLED0POS     PIN4_bp

static inline void user_led_set(uint8_t b)
{
	USERLEDPORT.OUT = b << USERLED0POS;
}

static inline void user_led_toggle(void)
{
	USERLEDPORT.OUTTGL = USERLEDMASK;
}
//! @}


/*! \name Touch
 */
//! @{
#define	TOUCHPORT       PORTE
#define	TOUCHMASK       (0x0F)
#define	TOUCH0POS       PIN0_bp


static inline bool is_touch0(void)
{
	if (TOUCHPORT.IN & (1<<(TOUCH0POS  ))) return false;
	else return true;
}

static inline bool is_touch1(void)
{
	if (TOUCHPORT.IN & (1<<(TOUCH0POS+1))) return false;
	else return true;
}

/* If USARTE0 is enabled, touch buttons CS2 and CS3 will not work, as they
 * share pins with the USART. These functions are then not defined if the user
 * tries to use them, generating an error rather than
 * the buttons not working. */
#ifndef CONF_BOARD_ENABLE_USARTE0
static inline bool is_touch2(void)
{
	if (TOUCHPORT.IN & (1<<(TOUCH0POS+2))) return false;
	else return true;
}

static inline bool is_touch3(void)
{
	if (TOUCHPORT.IN & (1<<(TOUCH0POS+3))) return false;
	else return true;
}
#endif

static inline uint8_t touch_get(void)
{
	return (((~TOUCHPORT.IN) & TOUCHMASK) << TOUCH0POS);
}
//! @}


/*! \name LCD BACKLIGHT
 */
//! @{
#define	BACKLIGHTPORT	PORTE
#define BACKLIGHT_GPIO	IOPORT_CREATE_PIN(PORTE,5)
#define BACKLIGHT_TIMER TCE0

#ifndef BACKLIGHT_TIMER_FREQUENCY
#define BACKLIGHT_TIMER_FREQUENCY	2000000
#endif

#ifndef	BACKLIGHT_TIMER_COUNT
#define BACKLIGHT_TIMER_COUNT		0x7FFF
#endif

static inline void backlight_on(void)
{
	gpio_set_pin_high(BACKLIGHT_GPIO);
}

static inline void backlight_off(void)
{
	gpio_set_pin_low(BACKLIGHT_GPIO);
}

static inline void backlight_toggle(void)
{
	gpio_toggle_pin(BACKLIGHT_GPIO);
}


//! @}

/*! \name Number of LEDs.
 */
//! @{
#define LED_COUNT   4
//! @}

/*! \name GPIO Connections of LEDs
 */
//! @{
#define LED0_GPIO   IOPORT_CREATE_PIN(PORTB,4)
#define LED1_GPIO   IOPORT_CREATE_PIN(PORTB,5)
#define LED2_GPIO   IOPORT_CREATE_PIN(PORTB,6)
#define LED3_GPIO   IOPORT_CREATE_PIN(PORTB,7)
#define LED_POWER   IOPORT_CREATE_PIN(PORTE,4)
#define LED0        LED0_GPIO
#define LED1        LED1_GPIO
#define LED2        LED2_GPIO
#define LED3        LED3_GPIO
//! @}


/*! \name GPIO Connections of Push Buttons
 * Warning, PB4 to PB7 are available only if JTAG is disabled.
 */
//! @{
#define GPIO_PUSH_BUTTON_0	IOPORT_CREATE_PIN(PORTE,0)
#define GPIO_PUSH_BUTTON_1	IOPORT_CREATE_PIN(PORTE,1)
#define GPIO_PUSH_BUTTON_2	IOPORT_CREATE_PIN(PORTE,2)
#define GPIO_PUSH_BUTTON_3	IOPORT_CREATE_PIN(PORTE,3)
//! @}

/*! \name SPI Connections of the AT45DBX Data Flash Memory
 */
//! @{
#define AT45DBX_SPI           &USARTC0
#define AT45DBX_CS            IOPORT_CREATE_PIN(PORTD,2)
#define AT45DBX_MASTER_SCK    IOPORT_CREATE_PIN(PORTC,5)  // SCK as output
#define AT45DBX_MASTER_MOSI   IOPORT_CREATE_PIN(PORTC,7)  // MOSI as output
#define AT45DBX_MASTER_MISO   IOPORT_CREATE_PIN(PORTC,6)  // MISO as input
#define AT45DBX_SPI_CLK_MASK  SYSCLK_PORT_C
//! @}

//! \name MXT143E Xplained top module
//@{
#define MXT143E_XPLAINED_TWI           &TWIC
#define MXT143E_XPLAINED_USART_SPI     &USARTC0
#define MXT143E_XPLAINED_CS            IOPORT_CREATE_PIN(PORTC, 4)
#define MXT143E_XPLAINED_SCK           IOPORT_CREATE_PIN(PORTC, 7)
#define MXT143E_XPLAINED_MOSI          IOPORT_CREATE_PIN(PORTC, 5)
#define MXT143E_XPLAINED_MISO          IOPORT_CREATE_PIN(PORTC, 6)
#define MXT143E_XPLAINED_CHG           IOPORT_CREATE_PIN(PORTC, 2)
#define MXT143E_XPLAINED_DC            IOPORT_CREATE_PIN(PORTC, 3)
#define MXT143E_XPLAINED_BACKLIGHT     IOPORT_CREATE_PIN(PORTA, 4)
#define MXT143E_XPLAINED_LCD_RESET     IOPORT_CREATE_PIN(PORTA, 6)
//@}

/*! \name Power ON LED management
 *
 */
//! @{
static inline void led_power_on(void)
{
	gpio_set_pin_high(LED_POWER);
}

static inline void led_power_off(void)
{
	gpio_set_pin_low(LED_POWER);
}

/*! \name External oscillator
 */
//@{
#define BOARD_XOSC_HZ          8000000
#define BOARD_XOSC_TYPE        XOSC_TYPE_XTAL
#define BOARD_XOSC_STARTUP_US  2000
//@}

#ifdef CONF_BOARD_LCD_BACKLIGHT_PWM
	void backlight_set_pwm(uint16_t pwm);
	uint16_t backlight_get_pwm(void);
	void backlight_stop_pwm(void);
	void backlight_start_pwm(void);
#endif

#endif  // _XMEGA_B1_XPLAINED_H_
