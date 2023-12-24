#include <avr/io.h>

#ifndef _SPI_H_
#define _SPI_H_

// PIN and PORT definitions
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define CS       PB4
#define MOSI     PB5
#define MISO     PB6
#define SCK      PB7

// Macros for setting or clearing the chip select pin
#define CS_CLEAR() SPI_PORT &= ~_BV(CS)
#define CS_SET()   SPI_PORT |= _BV(CS)

// initialization definitions
#define SPI_ENABLE     _BV(SPE)
#define SPI_MASTER     _BV(MSTR)
#define SPI_SLAVE      ~_BV(MSTR)
#define SPI_FOSC_4     0
#define SPI_FOSC_16    _BV(SPR0)
#define SPI_FOSC_64    _BV(SPR1)
#define SPI_FOSC_128   _BV(SPR1) | _BV(SPR0)
#define SPI_2X_FOSC_2  _BV(SPI2X)
#define SPI_2X_FOSC_8  _BV(SPI2X) | _BV(SPR0)
#define SPI_2X_FOSC_32 _BV(SPI2X) | _BV(SPR1)
#define SPI_2X_FOSC_64 _BV(SPI2X) | _BV(SPR1) | _BV(SPR0)
#define SPI_INTERRUPTS _BV(SPIE)
#define SPI_MODE_0     0
#define SPI_MODE_1     _BV(CPHA)
#define SPI_MODE_2     _BV(CPOL)
#define SPI_MODE_3     _BV(CPOL) | _BV(CPHA)
#define SPI_DEFAULT    SPI_MASTER | SPI_FOSC_128 | SPI_MODE_0

/**
 * @brief Initialize the SPI interface.
 *
 * @details This function initializes the SPI interface. It sets the CS, MOSI,
 * and SCK pins to output, and the MISO pin to input. It also enables the pull
 * up resistor on the MISO pin. The SPI interface is set to master mode, with
 * the clock set to fosc/128.
 */
void SPI_init(void);
/**
 * @brief Function to transmit a byte of data over the SPI interface.
 * @param data The data byte to be transmitted.
 * @return The status of the transmission. Typically, this is the data byte that
 * was received while the transmission was taking place.
 */
uint8_t SPI_transmit(const uint8_t data);
/**
 * @brief Function which receives a byte of data over the SPI interface.
 * @return The data byte that was received.
 */
uint8_t SPI_receive(void);

#endif