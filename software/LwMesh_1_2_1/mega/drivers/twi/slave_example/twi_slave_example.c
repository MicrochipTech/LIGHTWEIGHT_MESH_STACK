/**
 *
 * \file
 *
 * \brief megaRF example for twi as slave
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
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the TWI slave mode example.
 *
 * \section files Main Files
 * - twi_slave_example.c: example application.
 * - conf_board.h: board configuration
 * - conf_twi.h: TWI slave configuration used in this example
 *
 * \section driverinfo twi slave Driver
 * The twi driver can be found \ref group_megarf_drivers_twi "here".
 *
 * \section deviceinfo Device Info
 * AVR MEGARF device can be used.
 *
 * \section exampledescription Description of the example
 * The given example uses one kit as a TWI slave:
 *    - the TWI slave gets data for the a write request from TWI master,
 *    - the TWI slave send data for the a read request from TWI master,
 *
 * Yellow LED , Green LED gives the result of the test:
 * Green LED is ON when transmission is success.
 * Yellow LED is ON when reception is success.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/8-bit">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include "asf.h"
#include <conf_twi.h>

/** \name Slave memory Test Pattern Constants */
/** @{*/

#define PATTERN_TEST_LENGTH     sizeof(test_pattern)
const uint8_t test_pattern[] = {
	0x05,
	0x15,
	0x25,
	0x35,
	0x45,
	0x55,
	0x65,
	0x75,
	0x85
};

uint8_t data_received[PATTERN_TEST_LENGTH] = {0};

/** @}*/


#define SLAVE_BUS_ADDR       0xB0        /* !< TWI slave bus address */
#define TWI_GCE              0x01        /* !< TWI slave general call enable */

static void twi_Transmission_Failure(void);
static void twi_Transmit_Success(void);
static void twi_Receive_Success(void);


/**
 * \brief TWI Slave Example Main
 */
int main(void)
{
	/* Initialize the common clock service, board-specific initialization,
	 * and
	 * interrupt vector support prior to using the TWI master interfaces.
	 */
	sysclk_init();

	board_init();

	/* Enable the peripheral clock for TWI module */
	sysclk_enable_peripheral_clock(&TWBR);

	/* Initialize the TWI Slave driver. */
	twi_slave_init(SLAVE_BUS_ADDR | TWI_GCE);

	/* Enable global interrupt */
	cpu_irq_enable();

	slave_data_buffer_t data_buffer = {
		.rx_buffer = (void *)data_received, /* transfer data source
		                                     * buffer */
		.tx_buffer = (void *)test_pattern	
	};

	/* Start the TWI slave Transceiver */
	twi_slave_start(&data_buffer);
	
	/* Wait in infinite loop and check for TWI status */
	while (1) {
		if (twi_slave_state_get() == TWI_IDLE) {
			switch (twi_slave_status_get()) {
			case TWI_STATUS_TX_COMPLETE:
				twi_Transmit_Success();
				break;

			case TWI_STATUS_RX_COMPLETE:
				twi_Receive_Success();
				break;

			case TWI_STATUS_NO_STATE:
				break;

			default:
				twi_Transmission_Failure();
				break;
			}
		}
	}
}

/**
 * \brief TWI transmission failure indication through red LED On
 */
static void twi_Transmission_Failure(void)
{
	/* Red LED ON */
	LED_On(LED_RED_GPIO);

	twi_slave_status_reset();
}

/**
 * \brief TWI transmit success indication through green LED On
 */
static void twi_Transmit_Success(void)
{
	/* Green LED ON */
	LED_On(LED_GREEN_GPIO);

	twi_slave_status_reset();
}

/**
 * \brief TWI receive success indication through yellow LED On
 */
static void twi_Receive_Success(void)
{
	/* Yellow LED ON */
	LED_On(LED_YELLOW_GPIO);

	twi_slave_status_reset();
}
