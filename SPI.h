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

// macros
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

void    SPI_init(void);
uint8_t SPI_transmit(const uint8_t data);
uint8_t SPI_receive(void);

#endif