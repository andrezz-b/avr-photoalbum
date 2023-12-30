#include "USART.h"
#include <stdarg.h>

void USART_Init(USART_Config_t* Config)
{
    USART_SetBaudrate(Config->Baudrate, F_CPU, Config->EnableDoubleSpeed);
    USART_SetDirection(Config->Direction);

    // Configure the USART interface
    UCSRC = _BV(URSEL) | Config->DeviceMode | Config->Stop | Config->Parity | Config->Size |
            Config->ClockPolarity;
}

void USART_Write(char* Data)
{
    while (*Data)
    {
        USART_SendChar(*Data++);
    }
}

void USART_WriteLine(char* Data)
{
    USART_Write(Data);
    USART_SendChar(LF);
    USART_SendChar(CR);
}

void USART_WriteDecimal(const uint32_t Number)
{
    uint8_t Temp = Number / 10;
    char    Buffer[2];

    if (Temp)
    {
        USART_WriteDecimal(Temp);
    }

    Buffer[0] = 0x30 + (Number % 10);
    Buffer[1] = '\0';

    USART_Write(Buffer);
}

void USART_WriteHex(uint8_t val)
{
    // extract upper and lower nibbles from input value
    uint8_t upperNibble = (val & 0xF0) >> 4;
    uint8_t lowerNibble = val & 0x0F;

    // convert nibble to its ASCII hex equivalent
    upperNibble += upperNibble > 9 ? 'A' - 10 : '0';
    lowerNibble += lowerNibble > 9 ? 'A' - 10 : '0';

    char buffer[3];
    buffer[0] = upperNibble;
    buffer[1] = lowerNibble;
    buffer[2] = '\0';
    USART_Write(buffer);
}

void USART_printf(const char* format, ...)
{
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    USART_Write(buffer);
}

void USART_SetBaudrate(const uint32_t Baudrate, const uint32_t Clock, const bool DoubleSpeed)
{
    /*  - Clock << DoubleSpeed - When using DobuleSpeed we need to divide by 8,
        but (Clock * 2) / 16 = Clock / 8
        - Baudrate << 0x04 = 16 * Baudrate
    */
    uint16_t BaudValue = ((Clock << DoubleSpeed) / (Baudrate << 0x04)) - 0x01;

    // URSEL must be set to 0 when writing to UBRRH
    UBRRH &= ~_BV(URSEL);
    UBRRH = (BaudValue >> 0x08);
    UBRRL = (BaudValue & 0xFF);
    UCSRA = (UCSRA & ~_BV(U2X)) | (DoubleSpeed << U2X);
}