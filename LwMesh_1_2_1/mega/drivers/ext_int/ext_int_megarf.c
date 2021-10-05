/**
 * \file
 *
 * \brief External interrupt driver for megaRF
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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

#include <compiler.h>
#include "ioport.h"
#include <ext_int_megarf.h>

//! \internal Local storage of EXT_INT0 interrupt callback function
static ext_int_callback_t ext_int_int0_callback;
//! \internal Local storage of EXT_INT1 interrupt callback function
static ext_int_callback_t ext_int_int1_callback;
//! \internal Local storage of EXT_INT2 interrupt callback function
static ext_int_callback_t ext_int_int2_callback;
//! \internal Local storage of EXT_INT3 interrupt callback function
static ext_int_callback_t ext_int_int3_callback;
//! \internal Local storage of EXT_INT4 interrupt callback function
static ext_int_callback_t ext_int_int4_callback;
//! \internal Local storage of EXT_INT0 interrupt callback function
static ext_int_callback_t ext_int_int5_callback;
//! \internal Local storage of EXT_INT6 interrupt callback function
static ext_int_callback_t ext_int_int6_callback;
//! \internal Local storage of EXT_INT7 interrupt callback function
static ext_int_callback_t ext_int_int7_callback;
//! \internal Local storage of PC_EXT_INT0 to PC_EXT_INT7 interrupt callback function
static ext_int_callback_t ext_int_pcint0_callback;
//! \internal Local storage of PC_EXT_INT8 interrupt callback function
static ext_int_callback_t ext_int_pcint1_callback;

/**
 * \internal
 * \brief Interrupt handler for External Interrupt INT0
 *
 * This function will handle interrupt on External Interrupt INT0 and
 * call the callback function.
 */
ISR(INT0_vect)
{
	if (ext_int_int0_callback) {
		ext_int_int0_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for External Interrupt INT1
 *
 * This function will handle interrupt on External Interrupt INT1 and
 * call the callback function.
 */
ISR(INT1_vect)
{
	if (ext_int_int1_callback) {
		ext_int_int1_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for External Interrupt INT2
 *
 * This function will handle interrupt on External Interrupt INT2 and
 * call the callback function.
 */
ISR(INT2_vect)
{
	if (ext_int_int2_callback) {
		ext_int_int2_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for External Interrupt INT3
 *
 * This function will handle interrupt on External Interrupt INT3 and
 * call the callback function.
 */
ISR(INT3_vect)
{
	if (ext_int_int3_callback) {
		ext_int_int3_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for External Interrupt INT4
 *
 * This function will handle interrupt on External Interrupt INT4 and
 * call the callback function.
 */
ISR(INT4_vect)
{
	if (ext_int_int4_callback) {
		ext_int_int4_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for External Interrupt INT5
 *
 * This function will handle interrupt on External Interrupt INT5 and
 * call the callback function.
 */
ISR(INT5_vect)
{
	if (ext_int_int5_callback) {
		ext_int_int5_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for External Interrupt INT6
 *
 * This function will handle interrupt on External Interrupt INT6 and
 * call the callback function.
 */
ISR(INT6_vect)
{
	if (ext_int_int6_callback) {
		ext_int_int6_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for External Interrupt INT7
 *
 * This function will handle interrupt on External Interrupt INT7 and
 * call the callback function.
 */
ISR(INT7_vect)
{
	if (ext_int_int7_callback) {
		ext_int_int7_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for PC External Interrupt INT0 - INT7
 *
 * This function will handle interrupt on PC External Interrupt INT0-INT7 and
 * call the callback function.
 */
ISR(PCINT0_vect)
{
	if (ext_int_pcint0_callback) {
		ext_int_pcint0_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for PC External Interrupt INT8
 *
 * This function will handle interrupt on PC External Interrupt INT8 and
 * call the callback function.
 */
ISR(PCINT1_vect)
{
	if (ext_int_pcint1_callback) {
		ext_int_pcint1_callback();
	}
}


void ext_int_pcint_clear_flag(ioport_pin_t pin)
{
	if (pin == PC_INT8_PIN) {
		CLEAR_PCINT_FLAG1();
	} else {
		CLEAR_PCINT_FLAG0();
	}
}

void ext_int_pcint_enable(ioport_pin_t pin)
{
	if (pin == PC_INT8_PIN) {
		PCICR |= (1 << PCIE1);

		PCMSK1 |= INT_PIN_MASK(0);
	} else {
		PCICR |= (1 << PCIE0);

		PCMSK0 |= INT_PIN_MASK(pin);
	}
}

void ext_int_pcint_disable(ioport_pin_t pin)
{
	if (pin == PC_INT8_PIN) {
		PCMSK1 &= ~INT_PIN_MASK(0);
	} else {
		PCMSK0 &= ~INT_PIN_MASK(pin);
	}
}

void ext_int_clear_flag(ioport_pin_t pin)
{
	CLEAR_INT_FLAG(pin);
}

void ext_int_disable(ioport_pin_t pin)
{
	EIMSK &= ~INT_PIN_MASK(pin);
}

void ext_int_enable(ioport_pin_t pin)
{
	EIMSK |= INT_PIN_MASK(pin);
}

void ext_int_init(ioport_pin_t pin, enum ioport_sense trigmode)
{
	/* Disable the interrupt */
	ext_int_disable(pin);
	
	/* Enable IRQ pin as input */
	/* Enable the pullup for the IRQ pin */
	ioport_configure_pin(pin, IOPORT_DIR_INPUT | IOPORT_PULL_UP);

        /* Setup interrupt sence control */
	ioport_set_pin_sense_mode(pin,trigmode);
	
	/* Clear the INTn interrupt flag */
	ext_int_clear_flag(pin);

	/* Enable the interrupt */
	ext_int_enable(pin);
}

void ext_int_pcint_init(ioport_pin_t pin)
{
	/* Disable the interrupt */
	ext_int_pcint_disable(pin);

	/* Enable IRQ pin as input */
	/* Enable the pullup for the IRQ pin */
	ioport_configure_pin(pin, IOPORT_DIR_INPUT | IOPORT_PULL_UP);

	/* Clear the PCINTn interrupt flag */
	ext_int_pcint_clear_flag(pin);

	/* Enable the interrupt */
	ext_int_pcint_enable(pin);
}

void ext_int_set_interrupt_callback(ioport_pin_t ext_int, ext_int_callback_t callback)
{
	if (ext_int == EXT_INT0_PIN) {
		ext_int_int0_callback = callback;
	} else
	if (ext_int == EXT_INT1_PIN) {
		ext_int_int1_callback = callback;
	} else
	if (ext_int == EXT_INT2_PIN) {
		ext_int_int2_callback = callback;
	} else
	if (ext_int == EXT_INT3_PIN) {
		ext_int_int3_callback = callback;
	} else
	if (ext_int == EXT_INT4_PIN) {
		ext_int_int4_callback = callback;
	} else
	if (ext_int == EXT_INT5_PIN) {
		ext_int_int5_callback = callback;
	} else
	if (ext_int == EXT_INT6_PIN) {
		ext_int_int6_callback = callback;
	} else
	if (ext_int == EXT_INT7_PIN) {
		ext_int_int7_callback = callback;
	} else
	if (ext_int >= PC_INT0_PIN && ext_int <=PC_INT7_PIN) {
		ext_int_pcint0_callback = callback;
	} else
	if (ext_int == PC_INT8_PIN) {
		ext_int_pcint1_callback = callback;
	} else
	{
		
	}
}