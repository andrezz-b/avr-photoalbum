/**
 * @file config.h
 * @brief Configuration file for the project.
 *
 * This file contains various configuration options and macros used in the project.
 */

#ifndef CONFIG_H
#define CONFIG_H


#include <avr/io.h>

/**
 * @def DEBUG_SERIAL
 * @brief Macro for enabling debug output via serial communication.
 *
 * Uncomment this line to enable debug output via serial communication.
 * When enabled, the DEBUG macro will print debug messages using printf.
 */
// #define DEBUG_SERIAL

#if defined(DEBUG_SERIAL)
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

/**
 * @defgroup DISPLAY_CONFIG Display pins, registers and configuration
 * @brief Definition for the display pins, registers and configurations
 *
 * @{
 */
#define WR_PORT     PORTA /**< Write pin port register */
#define WR_PIN      4     /**< Write pin number */
#define WR_PORT_DDR DDRA  /**< Write pin ddr register */

#define DATA_PORT_LOW      PORTD /**< Data port low register */
#define DATA_PORT_HIGH     PORTC /**< Data port high register */
#define DATA_PORT_LOW_DDR  DDRD  /**< Data port low ddr register */
#define DATA_PORT_HIGH_DDR DDRC  /**< Data port high ddr register */

#define RS_PORT     PORTA /**< Register select port register */
#define RS_PIN      2     /**< Register select pin number */
#define RS_PORT_DDR DDRA  /**< Register select ddr register */

#define CS_PORT     PORTA /**< Chip select port register */
#define CS_PIN      5     /**< Chip select pin number */
#define CS_PORT_DDR DDRA  /**< Chip select ddr register */

#define RD_PORT     PORTA /**< Read pin port register */
#define RD_PIN      3     /**< Read pin number */
#define RD_PORT_DDR DDRA  /**< Read pin ddr register */

#define RESET_PORT     PORTB /**< Reset pin port register */
#define RESET_PIN      1     /**< Reset pin number */
#define RESET_PORT_DDR DDRB  /**< Reset pin ddr register */

#define BACK_LIGHT_PORT PORTB /**< Back light port register */
#define BACK_LIGHT_PIN  3     /**< Back light pin number */
#define BACK_LIGHT_DDR  DDRB  /**< Back light ddr register */

#define TFT_WIDTH  240   /**< Display width */
#define TFT_HEIGHT 320UL /**< Display height */
#define BUFFPIXEL  240   /**< Buffer size for reading image */
/** @}*/

/**
 * @defgroup SPI_PIN SPI pins and registers
 * @brief SPI pins and registers used for connecting with the SD card
 *
 * @{
 */
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_SS   PB4
#define SPI_MOSI PB5
#define SPI_MISO PB6
#define SPI_SCK  PB7
/** @}*/

/**
 * @defgroup JOYSTICK_PIN Joystick controls
 * @brief Joystick control pins and registers
 *
 * @{
 */
#define IMG_CTRL_PORT PORTA /**< Joystick port register */
#define IMG_CTRL_PIN  PINA  /**< Joystick pin register */
#define IMG_CTRL_DDR  DDRA  /**< Joystick ddr register */
#define IMG_NEXT      0     /**< Joystick pin for the next image */
#define IMG_PREV      1     /**< Joystick pin for the previous image */
/** @}*/

/* No touch zone follows */
#define DPLIO _SFR_IO_ADDR(DATA_PORT_LOW)
#define DPHIO _SFR_IO_ADDR(DATA_PORT_HIGH)

/**
 * @defgroup STATUS_LED Status LED
 * @brief Pins, macros and registers for the display's status LED
 *
 * @{
 */
/**
 * @def HAVE_STATUS_LED
 * @brief Definition for enabling the status LED
 */
// #define HAVE_STATUS_LED
#define STATUS_LED_PORT PORTB  /**< Status LED port register */
#define STATUS_LED_PIN  PORTB0 /**< Status LED pin number */
#define STATUS_LED_DDR  DDRB   /**< Status LED ddr register */

#ifdef HAVE_STATUS_LED

#define status_led_on() STATUS_LED_PORT &= ~_BV(STATUS_LED_PIN);

#define status_led_off() STATUS_LED_PORT |= _BV(STATUS_LED_PIN);

#define status_led_toggle() STATUS_LED_PORT ^= _BV(STATUS_LED_PIN);

#define status_led_init() STATUS_LED_DDR |= _BV(STATUS_LED_PIN);

#else

#define status_led_on() \
    {                   \
    }
#define status_led_off() \
    {                    \
    }
#define status_led_toggle() \
    {                       \
    }
#define status_led_init() \
    {                     \
    }

#endif
/** @}*/

#endif // CONFIG_H