#include "SPI.h"

void SPI_init()
{
    // set CS, MOSI and SCK to output
    SPI_DDR |= (1 << CS) | (1 << MOSI) | (1 << SCK);

    // enable pull up resistor in MISO
    SPI_DDR |= (1 << MISO);

    // enable SPI, set as master, and clock to fosc/128
    SPCR = SPI_ENABLE | SPI_MASTER | SPI_FOSC_128 | SPI_MODE_0;

    // 2X disabled
    SPSR = 0;
}

uint8_t SPI_transmit(const uint8_t data)
{
    SPDR = data;

    while (!(SPSR & _BV(SPIF)))
        ;

    return SPDR;
}

uint8_t SPI_receive(void)
{
    SPDR = 0xff;

    while (!(SPSR & _BV(SPIF)))
        ;

    return SPDR;
}
