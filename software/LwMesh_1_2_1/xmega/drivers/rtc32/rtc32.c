/**
 * \file
 *
 * \brief AVR XMEGA 32-bit Real Time Counter driver
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <compiler.h>
#include <parts.h>
#include <sysclk.h>
#include <delay.h>
#include "rtc32.h"

#ifdef __ICCAVR__
# define _DWORDREGISTER DWORDREGISTER
#endif

/**
 * \internal
 * Workaround for missing CNT, PER and COMP in WinAVR header files
 * \todo Remove when header files are fixed if WinAVR release
 */
typedef struct RTC32_struct2 {
	register8_t CTRL;
	register8_t SYNCCTRL;
	register8_t INTCTRL;
	register8_t INTFLAGS;
	_DWORDREGISTER(CNT);
	_DWORDREGISTER(PER);
	_DWORDREGISTER(COMP);
} RTC32_t2;

#undef RTC32

/**
 * \internal
 * Workaround for missing CNT, PER and COMP in WinAVR header files
 * \todo Remove when header files are fixed if WinAVR release
 */
#define RTC32 (*(RTC32_t2 *)0x0420)

#ifdef CONFIG_RTC32_COMPARE_INT_LEVEL
# define RTC32_COMPARE_INT_LEVEL CONFIG_RTC32_COMPARE_INT_LEVEL
#else
# define RTC32_COMPARE_INT_LEVEL RTC32_COMPINTLVL_LO_gc
#endif

#ifdef CONFIG_RTC32_CLOCK_1024HZ
# define RTC32_CLOCK VBAT_XOSCSEL_bm
#else
# define RTC32_CLOCK 0
#endif

/**
 * \internal
 * \brief Driver private struct
 */
struct rtc_data_struct {
	//! Callback function to use on alarm
	rtc_callback_t callback;
};

/**
 * \internal
 * \brief Driver private data
 */
struct rtc_data_struct rtc_data;

/**
 * \internal
 * \brief Check if RTC32 is busy synchronizing
 *
 * \retval true  Is busy
 * \retval false Is ready
 */
static __always_inline bool rtc_is_busy(void)
{
	return RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm;
}

/**
 * \internal
 * \brief Get counter
 *
 * \return Counter value
 */
static inline uint32_t rtc_get_counter(void)
{
	RTC32.SYNCCTRL = RTC32_SYNCCNT_bm;
	while (RTC32.SYNCCTRL & RTC32_SYNCCNT_bm);
	return RTC32.CNT;
}

/**
 * \brief Set current time
 *
 * \param time Time value to set
 */
void rtc_set_time(uint32_t time)
{
	RTC32.CTRL = 0;

	while (rtc_is_busy());

	RTC32.CNT = time;
	RTC32.CTRL = RTC32_ENABLE_bm;
}

/**
 * \brief Get current time
 *
 * \return Current time value
 */
uint32_t rtc_get_time(void)
{
	return rtc_get_counter();
}

/**
 * \brief Set alarm time
 *
 * Will set absolute time of the alarm that will call the callback function
 * specified by \ref rtc_set_callback on expiration. Alternatively, you may
 * use \ref rtc_alarm_has_triggered to check if the alarm has expired.
 *
 * Any pending alarm will be overwritten with this function.
 *
 * \param time Absolute time value. See also \ref rtc32_min_alarm_time
 * \pre Needs interrupts disabled if used from several contexts
 */
void rtc_set_alarm(uint32_t time)
{
	RTC32.INTCTRL = RTC32_COMPARE_INT_LEVEL;
	RTC32.COMP = time;
	RTC32.INTFLAGS = RTC32_COMPIF_bm;
}

/**
 * \brief Check if pending alarm has triggered
 *
 * \retval true  Alarm has triggered
 * \retval false Alarm is pending
 */
bool rtc_alarm_has_triggered(void)
{
	// Interrupt enable is used on pending alarm
	return !(RTC32.INTCTRL & RTC32_COMPARE_INT_LEVEL);
}

/**
 * \brief Set callback to call on alarm
 *
 * \param callback Callback function pointer
 */
void rtc_set_callback(rtc_callback_t callback)
{
	rtc_data.callback = callback;
}

/**
 * \brief Checks battery backup system status.
 *
 * This function should be called once after each reset of the device in order
 * to determine the current battery backup status.
 * This function can not be used to continuously poll the status of the backup
 * system during normal operation since most status flags are only latched
 * during the power up sequence of the device.
 *
 * \param first_time_startup Indicates whether or not the VBAT system has been
 * started previously. This should be set to \c true upon the first call to this
 * function, and \c false upon later calls. Typically, the value for this
 * parameter should be stored in, e.g., EEPROM, in order to preserve the value
 * when main system power is lost.
 *
 * \returns Battery backup system status.
 */
enum vbat_status_code rtc_vbat_system_check(bool first_time_startup)
{
	enum vbat_status_code vbat_status;
	uint8_t flags = VBAT.STATUS;

	/* Ensure the module is clocked to be able to check the registers */
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);

	/*
	 * Check if a sufficient voltage was detected on the VBAT input.
	 * The brown-out detector (BBBOD) will be sampled once when the
	 * device starts up and the result is visible as the BBPWR flag.
	 */
	if (flags & VBAT_BBPWR_bm) {
		vbat_status = VBAT_STATUS_NO_POWER;
	} else {
		/*
		 * We have sufficient power, now we check if a power-on-reset
		 * (BBPOR) was detected on VBAT. This is visible from the BBPORF
		 * flag which is also only updated once when the device starts.
		 */
		if (flags & VBAT_BBPORF_bm) {
			if (first_time_startup) {
				vbat_status = VBAT_STATUS_INIT;
			} else {
				vbat_status = VBAT_STATUS_BBPOR;
			}
		} else if (flags & VBAT_BBBORF_bm) {
			vbat_status = VBAT_STATUS_BBBOD;
		} else {
			VBAT.CTRL = VBAT_ACCEN_bm;
			if (flags & VBAT_XOSCFAIL_bm) {
				vbat_status = VBAT_STATUS_XOSCFAIL;
			} else {
				vbat_status = VBAT_STATUS_OK;
			}
		}
	}
	return vbat_status;
}

/**
 * \internal
 * \brief Initialize VBAT and start 32kHz oscillator
 *
 * Enables access to the VBAT system, performs a reset, enables the failure
 * detection, and starts the oscillator.
 *
 * The default clock rate to the RTC32 is 1Hz but this can be changed to 1024Hz
 * by the user in the module configuration by defining
 * \ref CONFIG_RTC32_CLOCK_1024HZ.
 */
static void vbat_init(void)
{
	// Enable access to VBAT
	VBAT.CTRL |= VBAT_ACCEN_bm;

	ccp_write_io((void *)&VBAT.CTRL, VBAT_RESET_bm);

	VBAT.CTRL |= VBAT_XOSCFDEN_bm;
	/* This delay is needed to give the voltage in the backup system some
	* time to stabilize before we turn on the oscillator. If we do not
	* have this delay we may get a failure detection.
	*/
	delay_us(200);
	VBAT.CTRL |= VBAT_XOSCEN_bm | RTC32_CLOCK;
	while (!(VBAT.STATUS & VBAT_XOSCRDY_bm));
}

/**
 * \brief Initialize the 32kHz oscillator and RTC32
 *
 * Starts up the 32kHz oscillator in the backup system and initializes the
 * RTC32.
 *
 * \note When the backup system is used, the function \ref
 * rtc_vbat_system_check should be called to determine if a re-initialization
 * must be done.
 */
void rtc_init(void)
{
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	// Set up VBAT system and start oscillator
	vbat_init();

	// Disable the RTC32 module before setting it up
	RTC32.CTRL = 0;

	while (rtc_is_busy());

	// Set up maximum period and start at 0
	RTC32.PER = 0xffffffff;
	RTC32.CNT = 0;

	while (rtc_is_busy());

	RTC32.INTCTRL = 0;
	RTC32.CTRL = RTC32_ENABLE_bm;

	// Make sure it's sync'ed before return
	while (rtc_is_busy());
}

/**
 * \internal
 * \brief Compare interrupt used for alarm
 *
 * Disables the RTC32 interrupts, then calls the alarm callback function if one
 * has been set.
 */
ISR(RTC32_COMP_vect)
{
	RTC32.INTCTRL = 0;
	if (rtc_data.callback)
		rtc_data.callback(rtc_get_time());
}
