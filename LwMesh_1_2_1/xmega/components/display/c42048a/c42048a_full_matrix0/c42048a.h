/**
 * \file
 *
 * \brief Management of C42048A LCD Glass component.
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

#ifndef C42048A_H
#define C42048A_H


#include "compiler.h"
#include "lcd.h"
#include "conf_board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup c42048a_group Atmel YMCC42048AAAYDCL LCD Glass component
 *
 * This is a driver for Atmel YMCC42048AAAYDCL LCD component.
 * This component is the custom LCD used for XMEGA-B1 Xplained.
 * This driver provides functions for initialization and control of the
 * LCD segments.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref lcd_group for Liquid Crystal Display (LCD) module.
 * - \ref interrupt_group for ISR definition and disabling interrupts during
 * critical code sections.
 * @{
 */

/*! \name LCD Common
 * @{
 */
#define NB_OF_COM 4
#define NB_OF_SEG 40
//! @}

/*! \name LCD Connection Initialization
 * @{
 */
#define COM_SWP   false
#define SEG_SWP   false
#define PORT_MASK NB_OF_SEG
#define X_BIAS    false
#define LCD_DUTY  (enum LCD_DUTY_enum)(NB_OF_COM % 4)
//! @}

/*! \name Digit Decoder
 * @{
 */
//! 7-segment with 3 commons terminals
#define FIRST_7SEG_3C  255 // 255 = unavailable
#define WIDTH_7SEG_3C  255 // 255 = unavailable
#define DIR_7SEG_3C    0 //0 = from left to right

//! 7-segment with 4 commons terminals
#ifdef CONFIG_XMEGA_128B1_REVA
#  define FIRST_7SEG_4C 2
#else
#  define FIRST_7SEG_4C 10
#endif
#define WIDTH_7SEG_4C 5
#define DIR_7SEG_4C   1 // 1 = from right to left

//! 14-segment with 4 commons terminals
#define FIRST_14SEG_4C 12
#define WIDTH_14SEG_4C 7
#define DIR_14SEG_4C   0 // 0 = from left to right

//! 16-segment with 3 commons terminals
#define FIRST_16SEG_3C 255 // 255 = unavailable
#define WIDTH_16SEG_3C 255 // 255 = unavailable
#define DIR_16SEG_3C   0
//! @}


 /*! \name Pixel With Blinking feature
 * @{
 */
#define ICON_LEVELBAR 0,0
#define ICON_USB      1,0
#define ICON_AVR      2,0
#define ICON_COLON    3,0
#define ICON_WARNING  0,1
#define ICON_IN       1,1
#define ICON_OUT      2,1
#define ICON_ERROR    3,1
//! @}

 /*! \name Pixel With No-Blinking feature
 * @{
 */
#define ICON_AM         3,30
#define ICON_PM         0,29
#define ICON_DEGREE_C   3,11
#define ICON_DEGREE_F   2,11
#define ICON_VOLT       1,11
#define ICON_MILLI_VOLT 0,11

#define ICON_LEVEL_1    0,13
#define ICON_LEVEL_2    3,14
#define ICON_LEVEL_3    0,17
#define ICON_LEVEL_4    3,18
#define ICON_LEVEL_5    0,21
#define ICON_LEVEL_6    3,22
#define ICON_LEVEL_7    0,25
#define ICON_LEVEL_8    3,26

#define ICON_DOT_1      3,4
#define ICON_DOT_2      3,6
#define ICON_DOT_3      3,8
#define ICON_DOT_4      3,10

#define ICON_MINUS      0,10
//! @}


/*! \name Macro Functions
 * @{
 */

/**
 * \brief Write string to c42048a lcd glass alphanumeric field.
 *
 * This function will write the input string to the alphanumeric field of the
 * lcd glass.
 *
 * \param data Pointer to the data input string
 */
static inline void c42048a_write_alpha_packet(const uint8_t *data)
{
	lcd_write_packet(LCD_TDG_14S_4C_gc, FIRST_14SEG_4C, data, \
			WIDTH_14SEG_4C, DIR_14SEG_4C);
}

/**
 * \brief Write string to c42048a lcd glass numeric field.
 *
 * This function will write the input string to the numeric field of the
 * lcd glass.
 *
 * \param  data Pointer to the data input string
 */
static inline void c42048a_write_num_packet(const uint8_t *data)
{
	lcd_write_packet(LCD_TDG_7S_4C_gc, FIRST_7SEG_4C, data, \
			WIDTH_7SEG_4C, DIR_7SEG_4C);
}
/**
 * \brief Clear a specific LCD pixel on the LCD glass.
 *
 * This function will clear a specific LCD pixel.
 *
 *  \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 *  \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
static inline void c42048a_clear_pixel(uint8_t pix_com, uint8_t pix_seg)
{
		lcd_clear_pixel(pix_com,pix_seg);
}

/**
 * \brief Light on a specific LCD pixel on the LCD glass.
 *
 * This function will set a specific LCD pixel.
 *
 *  \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 *  \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
static inline void c42048a_set_pixel(uint8_t pix_com, uint8_t pix_seg)
{
	if (pix_seg<2) {
		lcd_clear_blink_pixel(pix_com,pix_seg);
	}
	lcd_set_pixel(pix_com,pix_seg);
}

/**
 * \brief Blink on a specific LCD pixel on the LCD glass.
 *
 * This function will blink a specific LCD pixel.
 *
 *  \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 *  \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
static inline void c42048a_blink_pixel(uint8_t pix_com, uint8_t pix_seg)
{
	// Be sure to blink only pixel that can do that by hardware
	if (pix_seg<2) {
		lcd_set_blink_pixel(pix_com,pix_seg);
	}
	lcd_blink_enable();
	lcd_set_pixel(pix_com,pix_seg);
}

/**
 * \brief Blink the entire screen.
 *
 * This function will make the entire screen blink.
 *
 */
static inline void c42048a_set_blink_screen(void)
{
	lcd_clear_blink_all_pixel();
	lcd_blink_enable();
}

/**
 * \brief Blink the entire screen.
 *
 * This function will make the entire screen blink.
 *
 */
static inline void c42048a_clear_blink_screen(void)
{
	lcd_blink_disable();
}

/*! \brief Let the LCD blinking n times.
 *
 *  This function counts the number of interrupt periods (note: 0=256).
 *
 *  \param  n_blink  Number of of interrupt periods.
 */
static inline void c42048a_wait_int_period(uint8_t n_int)
{
	lcd_wait_n_int(n_int);
}

/*! \brief Set the LCD contrast.
 *
 *  This function allows to adjust the LCD contrast of the glass.
 *
 *  \param  fcont  -32 <= signed contrast value <= 31.
 */
static inline void c42048a_set_contrast(int8_t fcont)
{
	lcd_set_contrast(fcont);
}

/*! \brief LCD blinking initialization.
 *
 *  This function sets the frequency of the hardware display blinking.
 *
 *  The function defines the blinking rate. In the same time, the hardware
 *  display blinking is disabled.
 *
 *  \param  lcd_blink_rate  Hardware display blinking frequency.
 */
static inline void c42048a_blinkrate_init(
		enum LCD_BLINKRATE_enum lcd_blink_rate)
{
	lcd_blinkrate_init(lcd_blink_rate);
}
//! @}

/*! \name Function Prototypes
 * @{
 */

/**
 * \brief Initialize the LCD glass control.
 *
 * This function initializes the LCD driver to control the LCD glass.
 * It perform LCD module intialization according to the C42048 characteristics.
 *
 */
void c42048a_init(void);

/**
 * \brief Display a value on the LCD bar graph.
 *
 * This function allows to display a uint8_t value using the
 * the bargraph section of the LCD graph.
 *
 * \param val The uint8_t value to be displayed
 */
void c42048a_bar_graph(uint8_t val);

/**
 * \brief Write a decimal numeric value to LCD glass.
 *
 * This function displays an uint16 value to the LCD numeric field of the glass.
 *
 * \param val The uint16_t value to be displayed
 */
void c42048a_set_numeric_dec(uint16_t val);

/**
 * \brief Write an alphanumeric string to the alphanumeric field of the glass.
 *
 * This function displays an input text string to the alphanumeric field of LCD.
 *
 * \param data The uint16_t value to be displayed
 */
#ifdef CONFIG_XMEGA_128B1_REVA
void c42048a_set_text(const uint8_t *data);

#else // XMEGA_B rev. higher than revA
static inline void c42048a_set_text(const uint8_t *data) {
	c42048a_write_alpha_packet(data);
}
#endif
//! @}



/** @} */

#ifdef __cplusplus
}
#endif



#endif  // C42048A_H
