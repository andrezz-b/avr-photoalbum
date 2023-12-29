#include "SD.h"
#include "SPI.h"
#include "USART.h"
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

SD_Error_t Error;
USART_Config_t USART_Config = {
    .DeviceMode = USART_MODE_ASYNCH,
    .Direction = USART_DIRECTION_BOTH,
    .Baudrate = 9600,
    .Parity = USART_PARITY_NONE,
    .Stop = USART_STOP_1,
    .Size = USART_SIZE_8,
    .ClockPolarity = 0,
    .EnableDoubleSpeed = false,
};

uint8_t is_newline(const char);
void sd_usart_read_write();
void write_to_sd_card();

int main(void)
{
    USART_Init(&USART_Config);
    SPI_init();

    sd_usart_read_write();

    return 0;
}

void sd_usart_read_write()
{
    Error = SD_Init();

    if (Error != SD_SUCCESSFULL)
    {
        USART_WriteLine("SD card initialization failed!");
        while (1)
            ;
    }
    else
    {
        USART_WriteLine("SD card initialization successful!");
    }
    DDRD &= ~_BV(PD3);
    PORTD |= _BV(PD3);
    GICR |= _BV(INT1);
    sei();

    USART_Write("Press 1 to write to SD card, Press INT1 to read from SD card ");
    char input;
    while (1)
    {
        input = USART_GetChar();
        if (input == '1')
        {
            USART_SendChar(input);
            USART_WriteLine("");
            write_to_sd_card();
        }
    }
}

uint8_t is_newline(const char input)
{
    return input == '\n' || input == '\r';
}

void write_to_sd_card()
{
    cli();
    USART_WriteLine("Enter text to write to SD card (press ENTER to finish):");
    unsigned char buffer[512] = {0};
    uint16_t len = 0;
    char input = ' ';
    while (!is_newline(input) || !len)
    {
        input = USART_GetChar();
        USART_SendChar(input);
        buffer[len++] = input;
        if (len == 511)
        {
            break;
        }
    }
    buffer[len] = '\0';
    SD_WriteDataBlock(0, buffer);
    USART_WriteLine("");
    USART_WriteLine("Data written to SD card");

    sei();
}

ISR(INT1_vect)
{
    uint8_t Data[512];
    Error = SD_ReadDataBlock(0, Data);
    USART_WriteLine("Data read from SD card:");
    USART_WriteLine((char *) Data);
    _delay_ms(5);
    GIFR |= _BV(INTF1);
}