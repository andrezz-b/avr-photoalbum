#include "SD.h"
#include "SPI.h"
#include "USART.h"

SD_Error_t     Error;
USART_Config_t USART_Config = {
    .Baudrate          = 9600,
    .ClockPolarity     = 0,
    .DeviceMode        = USART_MODE_ASYNCH,
    .Direction         = USART_DIRECTION_BOTH,
    .Size              = USART_SIZE_8,
    .Stop              = USART_STOP_1,
    .Parity            = USART_PARITY_NONE,
    .EnableDoubleSpeed = false,
};

void print_boot_sector_first_last_byte();

int main(void)
{
    USART_Init(&USART_Config);
    SPI_init();

    Error = SD_Init();

    if (Error != SD_SUCCESSFULL)
    {
        USART_WriteLine("SD card initialization failed!");
        return -1;
    }
    else
    {
        USART_WriteLine("SD card initialization successful!");
    }

    print_boot_sector_first_last_byte();

    while (1)
        ;
    return 0;
}

void print_boot_sector_first_last_byte()
{
    uint8_t FirstSector[512];
    Error = SD_ReadDataBlock(0x00, FirstSector);

    USART_WriteLine("First sector:");
    USART_WriteHex(FirstSector[0]);
    USART_WriteLine("");
    USART_WriteHex(FirstSector[511]);
}