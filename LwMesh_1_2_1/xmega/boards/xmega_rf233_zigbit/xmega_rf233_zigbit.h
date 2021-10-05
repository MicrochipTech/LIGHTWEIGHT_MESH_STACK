/**
 * \file
 *
 * \brief XMEGA_RF233_ZIGBIT board header file.
 *
 * This file contains definitions and services related to the features of the
 * XMEGA_RF233_ZIGBIT Xplained board.
 *
 * To use this board define BOARD=XMEGA_RF233_ZIGBIT
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
#ifndef _XMEGA_ZIGBIT_RF233_H_
#define _XMEGA_ZIGBIT_RF233_H_

#include <compiler.h>
# include "conf_board.h"
/**
 * \defgroup xmega_rf233_zigbit_group XMEGA-A3BU Xplained board
 * @{
 */

/**
 * \defgroup xmega_rf233_zigbit_feature_group Feature definitions
 * @{
 */

//! \name Miscellaneous data
//@{
//! Validate board support for the common sensor service.
#define COMMON_SENSOR_PLATFORM
//@}

/**
 * \name LEDs
 *
 * LED0 and LED1 are single yellow LEDs that are active low..
 * LED2 and LED3 are inside one package (Led red and green close
 * to USB connector) but can be controlled individually.
 * LED2 has a red color and is active low. This LED can be
 * used for general purposes.
 * LED3 has a green color and is active high. By default this
 * LED is on since it shall indicate that power is applied to the
 * board. By pulling the gate of a N-FET low it is possible to
 * turn off the LED if needed.
 */
//@{
#define MCU_SOC_NAME        "ATxmega256A3U"
#ifdef ZIGBIT_USB
#define BOARD_NAME          "ATZB-X-233-USB"
#define LED0_GPIO                       IOPORT_CREATE_PIN(PORTA, 4)
#define LED1_GPIO                       IOPORT_CREATE_PIN(PORTA, 5)

#define LED0                            LED0_GPIO
#define LED1                            LED1_GPIO

//! Number of LEDs.
#define LED_COUNT                       2

#endif

#ifdef ZIGBIT_EXT

#define BOARD_NAME          "ATZB-X-233-XPRO"
#define LED0_GPIO                       IOPORT_CREATE_PIN(PORTA, 6)
#define LED1_GPIO                       IOPORT_CREATE_PIN(PORTA, 7)
#define LED2_GPIO                       IOPORT_CREATE_PIN(PORTA, 5)

#define LED0                            LED0_GPIO
#define LED1                            LED1_GPIO
#define LED2                            LED2_GPIO

//! Number of LEDs.
#define LED_COUNT                       3

#define GPIO_PUSH_BUTTON_0              IOPORT_CREATE_PIN(PORTF, 2)

//! \name Communication interfaces on header J1
//@{
#define TWID_SDA                        IOPORT_CREATE_PIN(PORTD, 0)
#define TWID_SCL                        IOPORT_CREATE_PIN(PORTD, 1)
#define USARTD0_RXD                     IOPORT_CREATE_PIN(PORTD, 2)
#define USARTD0_TXD                     IOPORT_CREATE_PIN(PORTD, 3)
#define SPID_SS                         IOPORT_CREATE_PIN(PORTD, 4)
#define SPID_MOSI                       IOPORT_CREATE_PIN(PORTD, 5)
#define SPID_MISO                       IOPORT_CREATE_PIN(PORTD, 6)
#define SPID_SCK                        IOPORT_CREATE_PIN(PORTD, 7)
//@}


#endif

//@}






/*! \name Connections of the AT86RFX transceiver
 */
//! @{
#define AT86RFX_SPI                  &SPIC
#define AT86RFX_RST_PIN              IOPORT_CREATE_PIN(PORTC, 0)
#define AT86RFX_MISC_PIN             IOPORT_CREATE_PIN(PORTC, 1)
#define AT86RFX_IRQ_PIN              IOPORT_CREATE_PIN(PORTC, 2)
#define AT86RFX_SLP_PIN              IOPORT_CREATE_PIN(PORTC, 3)
#define AT86RFX_SPI_CS               IOPORT_CREATE_PIN(PORTC, 4)
#define AT86RFX_SPI_MOSI             IOPORT_CREATE_PIN(PORTC, 5)
#define AT86RFX_SPI_MISO             IOPORT_CREATE_PIN(PORTC, 6)
#define AT86RFX_SPI_SCK              IOPORT_CREATE_PIN(PORTC, 7)

#define AT86RFX_INTC_INIT()          ioport_configure_pin(AT86RFX_IRQ_PIN, IOPORT_DIR_INPUT); \
									 PORTC.PIN2CTRL = PORT_ISC0_bm; \
									 PORTC.INT0MASK = PIN2_bm; \
									 PORTC.INTFLAGS = PORT_INT0IF_bm;

#define AT86RFX_ISR()                ISR(PORTC_INT0_vect)

/** Enables the transceiver main interrupt. */
#define ENABLE_TRX_IRQ()                (PORTC.INTCTRL |= PORT_INT0LVL_gm)

/** Disables the transceiver main interrupt. */
#define DISABLE_TRX_IRQ()               (PORTC.INTCTRL &= ~PORT_INT0LVL_gm)

/** Clears the transceiver main interrupt. */
#define CLEAR_TRX_IRQ()                 (PORTC.INTFLAGS = PORT_INT0IF_bm)

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()   { uint8_t irq_mask = PORTC.INTCTRL; PORTC.INTCTRL &= ~PORT_INT0LVL_gm

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()   PORTC.INTCTRL = irq_mask; }

//! @}



/**
 * @}
 */

/**
 * \defgroup xmega_rf233_zigbit_config_group Configuration options
 * @{
 */

#if defined(__DOXYGEN__)

/**
 * \name Initialization
 * \note Define these symbols in \ref conf_board.h to enable the corresponding
 * features.
 */
//@{


/**
 * \def CONF_BOARD_ENABLE_USARTD0
 * \brief Initialize IO pins for USART 0 on port D
 */
#  if !defined(CONF_BOARD_ENABLE_USARTD0)
#    define CONF_BOARD_ENABLE_USARTD0
#  endif


//@}

#endif // __DOXYGEN__

/**
 * @}
 */

/**
 * @}
 */

#endif /* _XMEGA_ZIGBIT_RF233_H_ */
