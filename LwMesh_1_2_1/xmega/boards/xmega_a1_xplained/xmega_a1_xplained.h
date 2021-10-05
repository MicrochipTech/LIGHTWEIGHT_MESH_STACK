/*****************************************************************************
 *
 * \file
 *
 * \brief XMEGA-A1-XPLAINED board header file.
 *
 * This file contains definitions and services related to the features of the
 * XMEGA-A1-XPLAINED board.
 *
 * To use this board, define BOARD=XMEGA_A1_XPLAINED.
 *
 ******************************************************************************/

/**
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#ifndef _XMEGA_A1_XPLAINED_H_
#define _XMEGA_A1_XPLAINED_H_

#include <compiler.h>

/**
 * @defgroup atxmega128a1_xpld_config XMEGA-A1 Xplained Board Configuration
 * @{
 */

//! \name Miscellaneous data
//@{
//@}

//! Number of LEDs.
#define LED_COUNT                        8

//! \name GPIO Connections of LEDs
//@{
#define LED0_GPIO                        IOPORT_CREATE_PIN(PORTE, 0)
#define LED1_GPIO                        IOPORT_CREATE_PIN(PORTE, 1)
#define LED2_GPIO                        IOPORT_CREATE_PIN(PORTE, 2)
#define LED3_GPIO                        IOPORT_CREATE_PIN(PORTE, 3)
#define LED4_GPIO                        IOPORT_CREATE_PIN(PORTE, 4)
#define LED5_GPIO                        IOPORT_CREATE_PIN(PORTE, 5)
#define LED6_GPIO                        IOPORT_CREATE_PIN(PORTE, 6)
#define LED7_GPIO                        IOPORT_CREATE_PIN(PORTE, 7)

#define LED0                             LED0_GPIO
#define LED1                             LED1_GPIO
#define LED2                             LED2_GPIO
#define LED3                             LED3_GPIO
#define LED4                             LED4_GPIO
#define LED5                             LED5_GPIO
#define LED6                             LED6_GPIO
#define LED7                             LED7_GPIO
//@}

//! \name GPIO Connections of Push Buttons
//@{
#define GPIO_PUSH_BUTTON_0               IOPORT_CREATE_PIN(PORTD, 0)
#define GPIO_PUSH_BUTTON_1               IOPORT_CREATE_PIN(PORTD, 1)
#define GPIO_PUSH_BUTTON_2               IOPORT_CREATE_PIN(PORTD, 2)
#define GPIO_PUSH_BUTTON_3               IOPORT_CREATE_PIN(PORTD, 3)
#define GPIO_PUSH_BUTTON_4               IOPORT_CREATE_PIN(PORTD, 4)
#define GPIO_PUSH_BUTTON_5               IOPORT_CREATE_PIN(PORTD, 5)
// NB! PORTR pin 0
#define GPIO_PUSH_BUTTON_6               IOPORT_CREATE_PIN(PORTR, 0)
// NB! PORTR pin 1
#define GPIO_PUSH_BUTTON_7               IOPORT_CREATE_PIN(PORTR, 1)
//@}

//! \name ADC Connection of the Light Sensor
//@{
#define LIGHT_SENSOR_ADC_MODULE          ADCB
#define LIGHT_SENSOR_ADC_INPUT           ADCCH_POS_PIN1
#define LIGHT_SENSOR_SIGNAL_PIN          IOPORT_CREATE_PIN(PORTB, 1)
//@}

//! \name ADC Connection of the Temperature Sensor
//@{
#define TEMPERATURE_SENSOR_ADC_MODULE    ADCB
#define TEMPERATURE_SENSOR_ADC_INPUT     ADCCH_POS_PIN0
#define TEMPERATURE_SENSOR_SIGNAL_PIN    IOPORT_CREATE_PIN(PORTB, 0)
#define TEMPERATURE_SENSOR_ENABLE_PIN    IOPORT_CREATE_PIN(PORTB, 3)
#define TEMPERATURE_SENSOR_ENABLE_LEVEL  false
//@}

//! \name DAC connection and control of the Speaker
//@{
#define SPEAKER_DAC_MODULE               DACB
#define SPEAKER_DAC_CHANNEL              DAC_CH0
#define SPEAKER_SIGNAL_PIN               IOPORT_CREATE_PIN(PORTB, 2)
#define SPEAKER_ENABLE_PIN               IOPORT_CREATE_PIN(PORTQ, 3)
#define SPEAKER_ENABLE_LEVEL             true
//@}

/*! \name SPI Connections of the AT45DBX Data Flash Memory
 */
//! @{
#define AT45DBX_SPI           &USARTC0
#define AT45DBX_CS            IOPORT_CREATE_PIN(PORTQ,3)
#define AT45DBX_MASTER_SCK    IOPORT_CREATE_PIN(PORTC,7)  // SCK as output
#define AT45DBX_MASTER_MOSI   IOPORT_CREATE_PIN(PORTC,5)  // MOSI as output
#define AT45DBX_MASTER_MISO   IOPORT_CREATE_PIN(PORTC,6)  // MISO as input
#define AT45DBX_SPI_CLK_MASK  SYSCLK_PORT_C
//! @}

//! \name MXT143E Xplained top module
//@{
#define MXT143E_XPLAINED_TWI           &TWIF
#define MXT143E_XPLAINED_USART_SPI     &USARTF1
#define MXT143E_XPLAINED_CS            IOPORT_CREATE_PIN(PORTF, 4)
#define MXT143E_XPLAINED_SCK           IOPORT_CREATE_PIN(PORTF, 7)
#define MXT143E_XPLAINED_MOSI          IOPORT_CREATE_PIN(PORTF, 5)
#define MXT143E_XPLAINED_MISO          IOPORT_CREATE_PIN(PORTF, 6)
#define MXT143E_XPLAINED_CHG           IOPORT_CREATE_PIN(PORTF, 2)
#define MXT143E_XPLAINED_DC            IOPORT_CREATE_PIN(PORTF, 3)
#define MXT143E_XPLAINED_BACKLIGHT     IOPORT_CREATE_PIN(PORTA, 4)
#define MXT143E_XPLAINED_LCD_RESET     IOPORT_CREATE_PIN(PORTA, 6)
//@}

//! \name External oscillator
//@{
#define BOARD_XOSC_HZ                    8000000
#define BOARD_XOSC_TYPE                  XOSC_TYPE_XTAL
#define BOARD_XOSC_STARTUP_US            2000
//@}

//!< Validate board support for the common sensor service.

#define                             COMMON_SENSOR_PLATFORM

/** @} */ // atxmega128a1_xpld_config group

#endif  // _XMEGA_A1_XPLAINED_H_
