/**
 * @file config.h
 * @brief Configuration file for the project.
 *
 * This file contains various configuration options and macros used in the project.
 */

#ifndef CONFIG_H
#define CONFIG_H

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

#define TFT_WIDTH  240
#define TFT_HEIGHT 320UL
#define BUFFPIXEL  240

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

#endif // CONFIG_H