/**
 * \file
 *
 * \brief USART driver for AVR MEGA.
 *
 * This file contains basic functions for the AVR MEGA USART, with support for
 * all modes, settings and clock speeds.
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
#ifndef _USART_MEGA_H_
#define _USART_MEGA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"
#include "status_codes.h"

/**
 * \defgroup mega_usart_group USART module (USART)
 *
 * See \ref mega_usart_quickstart.
 *
 * This is a driver for configuring, enabling, disabling and use of the on-chip
 * USART.
 *
 * \section dependencies Dependencies
 *
 * The USART module depends on the following modules:
 *  - \ref sysclk_group for peripheral clock control.
 *  - \ref port_driver_group for peripheral io port control.
 *
 * @{
 */

/* ! Offset in lookup table for baudrate 1200 */
#define USART_BAUD_1200      0x00
/* ! Offset in lookup table for baudrate 2400 */
#define USART_BAUD_2400      0x01
/* ! Offset in lookup table for baudrate 4800 */
#define USART_BAUD_4800      0x02
/* ! Offset in lookup table for baudrate 9600 */
#define USART_BAUD_9600      0x03
/* ! Offset in lookup table for baudrate 19200 */
#define USART_BAUD_19200     0x04
/* ! Offset in lookup table for baudrate 38400 */
#define USART_BAUD_38400     0x05
/* ! Offset in lookup table for baudrate 57600 */
#define USART_BAUD_57600     0x06
/* ! Baudrate not in lookup table */
#define USART_BAUD_UNDEFINED 0xFF

typedef volatile uint8_t register8_t;
typedef volatile uint16_t register16_t;
/* Universal Synchronous/Asynchronous Receiver/Transmitter */
typedef struct USART_struct {
	register8_t UCSRnA;  /* Control Register A */
	register8_t UCSRnB;  /* Control Register B */
	register8_t UCSRnC;  /* Control Register C */
#if AVR8_PART_IS_DEFINED(ATmega324PB)
	register8_t UCSRnD;  /* Control Register D */
#else
	register8_t reserved;
#endif
	register16_t UBRR;  /* Baud Rate Register Value */
	register8_t UDR;  /* I/O Data Register */
} USART_t;

#define USART0    (*(USART_t *)0xC0)   /* Universal Asynchronous
	                                 * Receiver-Transmitter A0 */
#if AVR8_PART_IS_DEFINED(ATmega324PB)
#define USART1    (*(USART_t *)0xC8)   /* Universal Asynchronous
	                                 * Receiver-Transmitter A1 */
#endif

/* USART.UCSRxA  bit masks and bit positions */
#define USART_RXC_bm  0x80 /* RX complete bit mask.*/
#define USART_RXC_bp  7 /* RX complete bit position.*/

#define USART_TXC_bm  0x40 /* TX complete bit mask.*/
#define USART_TXC_bp  6 /* TX complete bit position.*/

#define USART_DRE_bm  0x20 /* DATA Register Empty Bit mask.*/
#define USART_DRE_bp  5 /*  DATA Register Empty bit position.*/

#define USART_FE_bm   0x10 /* Frame Error bit mask.*/
#define USART_FE_bp   4 /*Frame error bit position.*/

#define USART_DOR_bm  0x08 /* Data Over Run bit mask.*/
#define USART_DOR_bp  3 /* Data Over Run bit position.*/

#define USART_UPE_bm  0x04 /* Parity error bit mask.*/
#define USART_UPE_bp  2 /* Parity error bit position.*/

#define USART_U2X_bm  0x02 /* Double TX speed bit mask.*/
#define USART_U2X_bp  1 /* Double TX speed bit position.*/

#define USART_MPCM_bm  0x01 /* Multi Processor bit mask.*/
#define USART_MPMC_bp  0 /* Multi processor bit position.*/

/* USART.UCSRxB  bit masks and bit positions */
#define USART_RXCIE_bm  0x80 /* RX complete interrupt Enable bit mask.*/
#define USART_RXCIE_bp  7 /* RX complete interrupt Enable bit position.*/

#define USART_TXCIE_bm  0x40 /* TX complete interrupt Enable bit mask.*/
#define USART_TXCIE_bp  6 /* TX complete interrupt Enable bit position.*/

#define USART_DRIE_bm  0x20 /* Data register empty interrupt Enable bit mask.*/
#define USART_DRIE_bp  5 /* Data register empty interrupt Enable bit position.*/

#define USART_RXEN_bm  0x10  /* Receiver Enable bit mask. */
#define USART_RXEN_bp  4  /* Receiver Enable bit position. */

#define USART_TXEN_bm  0x08  /* Transmitter Enable bit mask. */
#define USART_TXEN_bp  3  /* Transmitter Enable bit position. */

#define USART_CHSIZE2_bm  0x04  /* Character Size bit mask. */
#define USART_CHSIZE2_bp  2 /* Character Size bit position. */

#define USART_RXB8_bm  0x02  /* Transmit bit 8 bit mask. */
#define USART_RXB8_bp  1  /* Transmit bit 8 bit position. */

#define USART_TXB8_bm  0x01  /* Transmit bit 8 bit mask. */
#define USART_TXB8_bp  0  /* Transmit bit 8 bit position. */

/* USART.UCSRxC  bit masks and bit positions */
#define USART_UMSEL01_gm  0xC0 /* USART Mode Select 01 grp mask.*/
#define USART_UMSEL01_gp  6 /* USART Mode Select 01 grp position.*/

#define USART_UMSEL01_bm  0x80 /* USART Mode Select 01 bit mask.*/
#define USART_UMSEL01_bp  7 /* USART Mode Select 01 bit position.*/

#define USART_UMSEL00_bm  0x40 /* USART Mode Select 00 bit mask.*/
#define USART_UMSEL00_bp  6 /* USART Mode Select 00 bit position.*/

#define USART_PMODE_gm  0x30 /* USART Parity Mode Select grp mask.*/
#define USART_PMODE_gp  4 /* USART Parity Mode Select grp position.*/

#define USART_PMODE01_bm  0x20 /* USART Parity Mode Select 01 bit mask.*/
#define USART_PMODE01_bp  5 /* USART Parity Mode Select 01 bit position.*/

#define USART_PMODE00_bm  0x10 /* USART Parity Mode Select 00 bit mask.*/
#define USART_PMODE00_bp  4 /* USART Parity Mode Select 00 bit position.*/

#define USART_STOPB_bm  0x08 /* USART stop bit mask.*/
#define USART_STOPB_bp  3 /* USART stop bit Position.*/

#define USART_CHSIZE01C_gm  0x06  /* Character Size 10 bit 1 mask. */
#define USART_CHSIZE01C_gp  1 /* Character Size 10 bit position. */

#define USART_CHSIZE01_bm  0x04  /* Character Size 10 bit 1 mask. */
#define USART_CHSIZE01_bp  2 /* Character Size 10 bit position. */

#define USART_CHSIZE00_bm  0x02  /* Character Size 00 bit 1 mask. */
#define USART_CHSIZE00_bp  1 /* Character Size bit 00 position. */

#define USART_UCPOL_bm  0x01  /* Sync mode Pol bit 1 mask. */
#define USART_UCPOL_bp  0 /*Sync mode Pol bit 0 position */

#define USART_UCPHA_bm 0x02 /* Clock Phase bit mask. */
#define USART_UCPHA_bp 1 /* Clock Phase bit position. */

#define USART_DORD_bm 0x04 /* Data order bit mask. */
#define USART_DORD_bp 2 /* Data order bit position. */

/* Character Size */
typedef enum USART_CHSIZE_enum {
	USART_CHSIZE_5BIT_gc = (0x00),  /* Character size: 5 bit */
	USART_CHSIZE_6BIT_gc = (0x01),  /* Character size: 6 bit */
	USART_CHSIZE_7BIT_gc = (0x02),  /* Character size: 7 bit */
	USART_CHSIZE_8BIT_gc = (0x03),  /* Character size: 8 bit */
	USART_CHSIZE_9BIT_gc = (0x07),  /* Character size: 9 bit */
} USART_CHSIZE_t;

/* Communication Mode */
typedef enum USART_CMODE_enum {
	USART_CMODE_ASYNCHRONOUS_gc = (0x00 << USART_UMSEL01_gp),  /*
	                                                            *
	                                                            *
	                                                            *Asynchronous
	                                                            * Mode */
	USART_CMODE_SYNCHRONOUS_gc = (0x01 << USART_UMSEL01_gp),  /* Synchronous
	                                                           * Mode */
	USART_CMODE_IRDA_gc = (0x02 << USART_UMSEL01_gp),  /* IrDA Mode */
	USART_CMODE_MSPI_gc = (0x03 << USART_UMSEL01_gp),  /* Master SPI Mode */
} USART_CMODE_t;

/* Parity Mode */
typedef enum USART_PMODE_enum {
	USART_PMODE_DISABLED_gc = (0x00 << USART_PMODE_gp),  /* No Parity */
	USART_PMODE_EVEN_gc = (0x02 << USART_PMODE_gp),  /* Even Parity */
	USART_PMODE_ODD_gc = (0x03 << USART_PMODE_gp),  /* Odd Parity */
} USART_PMODE_t;

/* ! Lookup table containing baudctrl values for CPU frequency 1 Mhz */
static PROGMEM_DECLARE(uint16_t, baudctrl_1mhz[]) = {
	0x0033, /* Baud: 1200 */
	0x0019, /* Baud: 2400 */
	0x000C, /* Baud: 4800 */
	0x0006, /* Baud: 9600 */
	0x0003, /* Baud: 19200 */
	0x0002, /* Baud: 38400 */
	0x0001, /* Baud: 57600 */
};

/* ! Lookup table containing baudctrl values for CPU frequency 8 Mhz */
static PROGMEM_DECLARE(uint16_t, baudctrl_8mhz[]) = {
	0x01A0, /* Baud: 1200 */
	0x00CF, /* Baud: 2400 */
	0x0067, /* Baud: 4800 */
	0x0033, /* Baud: 9600 */
	0x0019, /* Baud: 19200 */
	0x000C, /* Baud: 38400 */
	0x0007, /* Baud: 57600 */
};

/* ! Lookup table containing baudctrl values for CPU frequency 16 Mhz */
static PROGMEM_DECLARE(uint16_t, baudctrl_16mhz[]) = {
	0x0340, /* Baud: 1200 */
	0x01A0, /* Baud: 2400 */
	0x00CF, /* Baud: 4800 */
	0x0067, /* Baud: 9600 */
	0x0033, /* Baud: 19200 */
	0x0019, /* Baud: 38400 */
	0x0010, /* Baud: 57600 */
};


/* ! Input parameters when initializing RS232 and similar modes. */
typedef struct usart_rs232_options {
	/* ! Set baud rate of the USART (unused in slave modes). */
	uint32_t baudrate;

	/* ! Number of bits to transmit as a character (5 to 9). */
	USART_CHSIZE_t charlength;

	/* ! Parity type: USART_PMODE_DISABLED_gc, USART_PMODE_EVEN_gc, */
	/* ! USART_PMODE_ODD_gc. */
	USART_PMODE_t paritytype;

	/* ! Number of stop bits between two characters: */
	/* ! true: 2 stop bits */
	/* ! false: 1 stop bit */
	bool stopbits;
} usart_rs232_options_t;

/* ! Input parameters when initializing SPI master mode. */
typedef struct usart_spi_options {
	/* ! Set baud rate of the USART in SPI mode. */
	uint32_t baudrate;

	/* ! SPI transmission mode. */
	uint8_t spimode;

	uint8_t data_order;
} usart_spi_options_t;


/**
 * \brief Enable USART receiver.
 *
 * \param usart Pointer to the USART module
 */
static inline void usart_rx_enable(USART_t *usart)
{
	(usart)->UCSRnB |= USART_RXEN_bm;
}

/**
 * \brief Disable USART receiver.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_rx_disable(USART_t *usart)
{
	(usart)->UCSRnB &= ~USART_RXEN_bm;
}

/**
 * \brief  Configure the USART frame format.
 *
 *  Sets the frame format, Frame Size, parity mode and number of stop bits.
 *
 *  \param usart Pointer to the USART module
 *  \param charSize The character size. Use USART_CHSIZE_t type.
 *  \param parityMode The parity Mode. Use USART_PMODE_t type.
 *  \param twoStopBits Enable two stop bit mode. Use bool type.
 */
static inline void usart_format_set(USART_t *usart, USART_CHSIZE_t charSize,
		USART_PMODE_t parityMode, bool twoStopBits)
{
	(usart)->UCSRnC
		= ((usart)->UCSRnC &
			(~USART_CHSIZE01C_gm)) | ((charSize & 0x03)
			<< USART_CHSIZE01C_gp);
	(usart)->UCSRnB
		= ((usart)->UCSRnB &
			(~USART_CHSIZE2_bm)) | ((charSize & 0x04)
			<< USART_CHSIZE2_bp);

	(usart)->UCSRnC = ((usart)->UCSRnC & (~USART_PMODE_gm)) | parityMode;

	(usart)->UCSRnC = ((usart)->UCSRnC & (~USART_STOPB_bm)) | (twoStopBits
			<< USART_STOPB_bp);
}

/**
 * \brief Enable USART transmitter.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_tx_enable(USART_t *usart)
{
	(usart)->UCSRnB |= USART_TXEN_bm;
}

/**
 * \brief Disable USART transmitter.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_tx_disable(USART_t *usart)
{
	(usart)->UCSRnB &= ~USART_TXEN_bm;
}

/**
 * \brief Enable USART transmit complete interrupt.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_tx_complete_interrupt_enable(USART_t *usart)
{
	(usart)->UCSRnB |= USART_TXC_bm;
}

/**
 * \brief Enable USART receive complete interrupt.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_rx_complete_interrupt_enable(USART_t *usart)
{
	(usart)->UCSRnB |= USART_RXC_bm;
}

/**
 * \brief Enable USART data register empty interrupt.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_data_empty_interrupt_enable(USART_t *usart)
{
	(usart)->UCSRnB |= USART_DRIE_bm;
}

/**
 * \brief Disable USART transmit complete interrupt.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_tx_complete_interrupt_disable(USART_t *usart)
{
	(usart)->UCSRnB &= ~USART_TXC_bm;
}

/**
 * \brief Disable USART receive complete interrupt.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_rx_complete_interrupt_disable(USART_t *usart)
{
	(usart)->UCSRnB &= ~USART_RXC_bm;
}

/**
 * \brief Disable USART data register empty interrupt.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_data_empty_interrupt_disable(USART_t *usart)
{
	(usart)->UCSRnB &= ~USART_DRIE_bm;
}

/**
 * \brief Set the mode the USART run in.
 *
 * Set the mode the USART run in. The default mode is asynchronous mode.
 *
 * \param usart Pointer to the USART module register section.
 * \param usartmode Selects the USART mode. Use USART_CMODE_t type.
 *
 * USART modes:
 * - 0x0        : Asynchronous mode.
 * - 0x1        : Synchronous mode.
 * - 0x2        : IrDA mode.
 * - 0x3        : Master SPI mode.
 */
static inline void usart_set_mode(USART_t *usart, USART_CMODE_t usartmode)
{
	(usart)->UCSRnC = ((usart)->UCSRnC & (~USART_UMSEL01_gm)) | usartmode;
}

/**
 * \brief Check if data register empty flag is set.
 *
 * \param usart The USART module.
 */
static inline bool usart_data_register_is_empty(USART_t *usart)
{
	return (usart)->UCSRnA & USART_DRE_bm;
}

/**
 * \brief Checks if the RX complete interrupt flag is set.
 *
 * Checks if the RX complete interrupt flag is set.
 *
 * \param usart The USART module.
 */
static inline bool usart_rx_is_complete(USART_t *usart)
{
	return (usart)->UCSRnA & USART_RXC_bm;
}

/**
 * \brief Checks if the TX complete interrupt flag is set.
 *
 * Checks if the TX complete interrupt flag is set.
 *
 * \param usart The USART module.
 */
static inline bool usart_tx_is_complete(USART_t *usart)
{
	return (usart)->UCSRnA & USART_TXC_bm;
}

/**
 * \brief Clear TX complete interrupt flag.
 * TX flag is clear after complete transmission, automatically.
 * \param usart The USART module.
 */
static inline void usart_clear_tx_complete(USART_t *usart)
{
         usart->UCSRnA |=  USART_TXC_bm;
}

/**
 * \brief Write a data to the USART data register.
 *
 * \param usart The USART module.
 * \param txdata The data to be transmitted.
 */
static inline void usart_put(USART_t *usart, uint8_t txdata)
{
	(usart)->UDR = txdata;
}

/**
 * \brief  Read a data to the USART data register.
 *
 * \param usart The USART module.
 *
 * \return The received data
 */
static inline uint8_t usart_get(USART_t *usart)
{
	return (usart)->UDR;
}

/**
 * \brief Double the USART transmission speed.
 *
 * \param usart The USART module.
 */
static inline void usart_double_baud_enable(USART_t *usart)
{
	usart->UCSRnA |=  USART_U2X_bm;
}

/**
 * \brief Disable the Doubling of USART transmission speed.
 *
 * \param usart The USART module.
 */
static inline void usart_double_baud_disable(USART_t *usart)
{
	usart->UCSRnA &= ~USART_U2X_bm;
}

/**
 * \brief Performs a data transfer on the USART in SPI mode.
 *
 * \param usart The USART module.
 * \param txdata The data to be transmitted.
 *
 * \return The received data
 */
static inline uint8_t usart_spi_transmit(USART_t *usart,
		uint8_t txdata)
{
	while (usart_data_register_is_empty(usart) == false) {
	}
	usart_put(usart, txdata);
	while (!usart_tx_is_complete(usart)) {
	}
	usart_clear_tx_complete(usart);
	return usart_get(usart);
}

bool usart_init_rs232(USART_t *usart, const usart_rs232_options_t *opt);
void usart_init_spi(USART_t *usart, const usart_spi_options_t *opt);

status_code_t usart_putchar(USART_t *usart, uint8_t c);
uint8_t usart_getchar(USART_t *usart);

void usart_set_baudrate_precalculated(USART_t *usart, uint32_t baud,
		uint32_t cpu_hz);
bool usart_set_baudrate(USART_t *usart, uint32_t baud, uint32_t cpu_hz);
void usart_spi_set_baudrate(USART_t *usart, uint32_t baud, uint32_t cpu_hz);

/* ! @} */

#ifdef __cplusplus
}
#endif

#endif /* _USART_MEGA_H_ */
