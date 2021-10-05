/**
 * \file
 *
 * \brief USART driver for AVR MEGA.
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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

#include "compiler.h"
#include "usart_mega.h"
#include "sysclk.h"
#include "ioport.h"
#include "status_codes.h"

/**
 * \brief Enable the system clock to an USART module.
 *
 * This function will enable the system clock to the provided \arg usart
 * module.
 *
 * \brief usart Pointer to an USART module.
 */
static inline void usart_enable_module_clock(USART_t *usart)
{
#ifdef USART0
	if ((uintptr_t)usart == (uintptr_t)&UCSR0A) {
		sysclk_enable_module(POWER_RED_REG0, PRUSART0_bm);
	}
#endif
#ifdef USART1
	if ((uintptr_t)usart == (uintptr_t)&UCSR1A) {
		sysclk_enable_module(POWER_RED_REG1, PRUSART1_bm);
	}
#endif
}

/**
 * \brief Initialize USART in RS232 mode.
 *
 * This function initializes the USART module in RS232 mode using the
 * usart_rs232_options_t configuration structure and CPU frequency.
 *
 * \param usart The USART module.
 * \param opt The RS232 configuration option.
 *
 * \retval true if the initialization was successful
 * \retval false if the initialization failed (error in baud rate calculation)
 */
bool usart_init_rs232(USART_t *usart, const usart_rs232_options_t *opt)
{
	bool result;
	usart_enable_module_clock(usart);
	usart_set_mode(usart, USART_CMODE_ASYNCHRONOUS_gc);
	usart_format_set(usart, opt->charlength, opt->paritytype,
			opt->stopbits);
	result = usart_set_baudrate(usart, opt->baudrate,
			sysclk_get_source_clock_hz());
	usart_tx_enable(usart);
	usart_rx_enable(usart);
	return result;
}

/**
 * \brief Initialize USART in SPI master mode.
 *
 * This function initializes the USART module in SPI master mode using the
 * usart_spi_options_t configuration structure and CPU frequency.
 *
 * \param usart The USART module.
 * \param opt The RS232 configuration option.
 */
void usart_init_spi(USART_t *usart, const usart_spi_options_t *opt)
{
	usart->UBRR = 0;

	usart_enable_module_clock(usart);
	usart_set_mode(usart, USART_CMODE_MSPI_gc);

	if (opt->spimode == 1 || opt->spimode == 3) {
		usart->UCSRnC |= USART_UCPHA_bm;
	} else {
		usart->UCSRnC &= ~USART_UCPHA_bm;
	}
	if (opt->spimode == 2 || opt->spimode == 3) {
		usart->UCSRnC |= USART_UCPOL_bm;
	} else {
		usart->UCSRnC &= ~USART_UCPOL_bm;
	}	
	
	if (opt->data_order) {
		usart->UCSRnC |= USART_DORD_bm;
	} else {
		usart->UCSRnC &= ~USART_DORD_bm;
	}
	
	
	usart_spi_set_baudrate(usart, opt->baudrate,
			sysclk_get_source_clock_hz());
	usart_tx_enable(usart);
	usart_rx_enable(usart);
}

/**
 * \brief Send a data with the USART module
 *
 * This function outputs a data using the USART module.
 *
 * \param usart The USART module.
 * \param c The data to send.
 *
 * \return STATUS_OK
 */
status_code_t usart_putchar(USART_t *usart, uint8_t c)
{
	while (usart_data_register_is_empty(usart) == false) {
	}

	usart->UDR = c;
	return STATUS_OK;
}

/**
 * \brief Receive a data with the USART module
 *
 * This function returns the received data from the USART module.
 *
 * \param usart The USART module.
 *
 * \return The received data.
 */
uint8_t usart_getchar(USART_t *usart)
{
	while (usart_rx_is_complete(usart) == false) {
	}

	return ((uint8_t)usart->UDR);
}

/**
 * \brief Get the offset for lookup in the baudrate table
 *
 * \param baud The requested baudrate
 *
 * \return The baudrate offset in PROGMEM table
 * \retval USART_BAUD_UNDEFINED for baudrates not in lookup table
 */
static uint8_t usart_get_baud_offset(uint32_t baud)
{
	switch (baud) {
	case 1200:
		return (uint8_t)USART_BAUD_1200;

	case 2400:
		return (uint8_t)USART_BAUD_2400;

	case 4800:
		return (uint8_t)USART_BAUD_4800;

	case 9600:
		return (uint8_t)USART_BAUD_9600;

	case 19200:
		return (uint8_t)USART_BAUD_19200;

	case 38400:
		return (uint8_t)USART_BAUD_38400;

	case 57600:
		return (uint8_t)USART_BAUD_57600;

	default:
		return (uint8_t)USART_BAUD_UNDEFINED;
	}
}

/**
 * \brief Set the baudrate using pre calculated BAUDCTRL values stored in program 
 * memory
 *
 * \note This function only works for cpu_hz 1Mhz, 8Mhz or 16Mhz and baudrate values
 * 1200, 2400, 4800, 9600, 19200, 38400 and 57600.
 *
 * \param usart  The USART module.
 * \param baud   The baudrate.
 * \param cpu_hz The CPU frequency.
 *
 */
void usart_set_baudrate_precalculated(USART_t *usart, uint32_t baud,
		uint32_t cpu_hz)
{
	uint8_t baud_offset;
	uint16_t baudctrl = 0;

	baud_offset = usart_get_baud_offset(baud);

	if (cpu_hz == 1000000UL) {
		baudctrl = PROGMEM_READ_WORD(baudctrl_1mhz + baud_offset);
	} else if (cpu_hz == 8000000UL) {
		baudctrl = PROGMEM_READ_WORD(baudctrl_8mhz + baud_offset);
	} else if (cpu_hz == 16000000UL) {
		baudctrl = PROGMEM_READ_WORD(baudctrl_16mhz + baud_offset);
	} else {
		/* Error, system clock speed or USART baud rate is not supported
		 * by the look-up table */
		Assert(false);
	}

	if (baud_offset != USART_BAUD_UNDEFINED) {
		usart->UBRR = baudctrl;
	}
}

/**
 * \brief Set the baudrate value in the USART module
 *
 * This function sets the baudrate register with scaling regarding the CPU
 * frequency and makes sure the baud rate is supported by the hardware.
 * The function can be used if you don't want to calculate the settings
 * yourself or changes to baudrate at runtime is required.
 *
 * \param usart The USART module.
 * \param baud The baudrate.
 * \param cpu_hz The CPU frequency.
 *
 * \retval true if the hardware supports the baud rate
 * \retval false if the hardware does not support the baud rate (i.e. it's
 *               either too high or too low.)
 */
bool usart_set_baudrate(USART_t *usart, uint32_t baud, uint32_t cpu_hz)
{
	uint32_t min_rate;
	uint32_t max_rate;
	uint32_t ubrr;

	/*
	 * Check if the hardware supports the given baud rate
	 */
	/* 8 =  8 * (0 + 1) = (UBRRn_min + 1) */
	max_rate = cpu_hz / 8;
	/* 524288 =  8 * (65535 + 1)  =  8 *(UBRRn_max+1) */
	min_rate = cpu_hz / 524288;

	if (usart->UCSRnA & USART_U2X_bm) {
		max_rate /= 2;
		min_rate /= 2;
	}

	if ((baud > max_rate) || (baud < min_rate)) {
		return false;
	}

	/* Check if double speed is enabled. */
	if (usart->UCSRnA & USART_U2X_bm) {
		baud *= 2;
		ubrr = (uint32_t)(cpu_hz / 8 / baud) - 1;
	} else {
		ubrr = (uint32_t)(cpu_hz / 16 / baud) - 1;
	}

	usart->UBRR = ubrr;
	return true;
}

/**
 * \brief Set the baudrate value in the USART_SPI module
 *
 * This function sets the baudrate register regarding the CPU frequency.
 *
 * \param usart The USART(SPI) module.
 * \param baud The baudrate.
 * \param cpu_hz The CPU frequency.
 */
void usart_spi_set_baudrate(USART_t *usart, uint32_t baud, uint32_t cpu_hz)
{
	uint32_t ubrr;

	/* Check if baudrate is less than the maximim limit specified in
	 * datasheet */
	if (baud < (cpu_hz / 2)) {
		ubrr = (cpu_hz / (2 * baud) - 1);
	} else {
		/* If baudrate is not within the specfication in datasheet,
		 * assign maximum baudrate possible for the current CPU frequency */
		ubrr = 0;
	}
	
	usart->UBRR  = ubrr;
}
