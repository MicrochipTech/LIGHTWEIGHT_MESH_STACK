/**
 * \file
 *
 * \brief AVR MEGARF MAC Symbol Counter Driver 
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
#include <stdint.h>

#include "compiler.h"
#include "macsc_megarf.h"

/* ! \internal Local storage of Timer Counter TCC0 interrupt callback function */
static macsc_callback_t macsc_ovf_cb;
static macsc_callback_t macsc_cmp1_cb;
static macsc_callback_t macsc_cmp2_cb;
static macsc_callback_t macsc_cmp3_cb;
static macsc_callback_t macsc_slotcnt_cb;

/**
 * \internal
 * \brief Interrupt handler for MAC Symbol counter overflow
 *
 * This function will handle interrupt on MAC Symbol counter overflow and
 * call the callback function.
 */
ISR(SCNT_OVFL_vect)
{
	if (macsc_ovf_cb) {
		macsc_ovf_cb();
	}
}

/**
 * \internal
 * \brief Interrupt handler for MAC Symbol counter Compare 1 Match
 *
 * This function will handle interrupt on MAC Symbol counter Compare 1 Match and
 * call the callback function.
 */
ISR(SCNT_CMP1_vect)
{
	if (macsc_cmp1_cb) {
		macsc_cmp1_cb();
	}
}

/**
 * \internal
 * \brief Interrupt handler for MAC Symbol counter Compare 2 Match
 *
 * This function will handle interrupt on MAC Symbol counter Compare 2 Match and
 * call the callback function.
 */
ISR(SCNT_CMP2_vect)
{
	if (macsc_cmp2_cb) {
		macsc_cmp2_cb();
	}
}

/**
 * \internal
 * \brief Interrupt handler for MAC Symbol counter Compare 3 Match
 *
 * This function will handle interrupt on MAC Symbol counter Compare 3 Match and
 * call the callback function.
 */
ISR(SCNT_CMP3_vect)
{
	if (macsc_cmp3_cb) {
		macsc_cmp3_cb();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Back-off Slot counter
 *
 * This function will handle interrupt of the Back-off Slot counter and
 * call the callback function.
 */
ISR(SCNT_BACKOFF_vect)
{
	if (macsc_slotcnt_cb) {
		macsc_slotcnt_cb();
	}
}

/**
 * \brief Enable MAC SC
 *
 * Enables the SC
 *
 * \param none
 *
 */
void macsc_enable(void)
{
	irqflags_t iflags = cpu_irq_save();

	SCCR0 |= (1 << SCEN);

	cpu_irq_restore(iflags);
}

/**
 * \brief Check if MACSC is enabled
 *
 * check if the MACSC is enabled.
 *
 * \param none
 *
 */
bool is_macsc_enable(void)
{	
   	return(SCCR0&&(1<<SCEN));	
}

/**
 * \brief Disable MAC SC
 *
 * Disables the MAC SC
 *
 * \param none
 *
 */
void macsc_disable(void)
{
	SCCR0 &= ~(1<<SCEN);
}

/**
 * \brief Check if back-off slot counter is enabled
 *
 * check if the back-off slot counter is enabled.
 *
 * \param none
 *
 */
bool is_macsc_backoff_enable(void)
{	
   	return(SCCR1&&(1<<SCENBO));	
}

/**
 * \brief Enables compare interrupts of the MACSC 
 * \param channel Compare channel
 */
void macsc_enable_cmp_int(enum macsc_cc_channel channel)
{
	switch (channel) {
	case MACSC_CC1:
	{
		if ((SCIRQM & (1 << IRQMCP1)) == 0) {
			SCIRQS |= (1 << IRQSCP1);
			SCIRQM |= (1 << IRQMCP1);
		}
	}
	break;

	case MACSC_CC2:
	{
		if ((SCIRQM & (1 << IRQMCP2)) == 0) {
			SCIRQS |= (1 << IRQSCP2);
			SCIRQM |= (1 << IRQMCP2);
		}
	}
	break;

	case MACSC_CC3:
	{
		if ((SCIRQM & (1 << IRQMCP3)) == 0) {
			SCIRQS |= (1 << IRQSCP3);
			SCIRQM |= (1 << IRQMCP3);
		}
	}
	break;

	default:
		break;
	}
}

/**
 * \brief Absolute compare usage
 *
 * \param abs_rel  0 for absoulte cmp;1 for relative cmp
 * \param cmp compare value for SCOCRx register
 * \param channel Compare channel
 */
void macsc_use_cmp(bool abs_rel, uint32_t cmp,
		enum macsc_cc_channel channel)
{
	switch (channel) {
	case MACSC_CC1:
	{
		if (abs_rel) {
			SCCR0 |= (1 << SCCMP1);
		} else {
			SCCR0 &= ~(1 << SCCMP1);
		}

		MACSC_WRITE32(SCOCR1, cmp);
	}
	break;

	case MACSC_CC2:
	{
		if (abs_rel) {
			SCCR0 |= (1 << SCCMP2);
		} else {
			SCCR0 &= ~(1 << SCCMP2);
		}

		MACSC_WRITE32(SCOCR2, cmp);
	}
	break;

	case MACSC_CC3:
	{
		if (abs_rel) {
			SCCR0 |= (1 << SCCMP3);
		} else {
			SCCR0 &= ~(1 << SCCMP3);
		}

		MACSC_WRITE32(SCOCR3, cmp);
	}
	break;

	default:
		break;
	}
}

void macsc_set_ovf_int_cb(macsc_callback_t callback)
{
	macsc_ovf_cb = callback;
}

void macsc_set_cmp1_int_cb(macsc_callback_t callback)
{
	macsc_cmp1_cb = callback;
}

void macsc_set_cmp2_int_cb(macsc_callback_t callback)
{
	macsc_cmp2_cb = callback;
}

void macsc_set_cmp3_int_cb(macsc_callback_t callback)
{
	macsc_cmp3_cb = callback;
}

void macsc_set_backoff_slot_cntr_int_cb(macsc_callback_t callback)
{
	macsc_slotcnt_cb = callback;
}
