#include "SD.h"
#include "SPI.h"
#include "USART.h"
#include "ff.h"

#include "diskio.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static DWORD pn(          /* Pseudo random number generator */
                DWORD pns /* 0:Initialize, !0:Read */
)
{
    static DWORD lfsr;
    UINT n;

    if (pns)
    {
        lfsr = pns;
        for (n = 0; n < 32; n++)
            pn(0);
    }
    if (lfsr & 1)
    {
        lfsr >>= 1;
        lfsr ^= 0x80200003;
    }
    else
    {
        lfsr >>= 1;
    }
    return lfsr;
}

void USART_printf(const char* format, ...)
{
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    USART_Write(buffer);
}

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
uint8_t disk_test(BYTE disk_id);

int main(void)
{
    USART_Init(&USART_Config);
    SPI_init();

    uint8_t result = disk_test(0);

    while (1)
        ;

    return 0;
}

uint8_t disk_test(BYTE disk_id)
{
    DSTATUS disk_status;
    DRESULT disk_result;
    uint8_t buffer[530];
    uint32_t sector = 0;
    uint32_t pns = 1;
    uint16_t disk_size, buffer_size = sizeof(buffer), n = 0, sector_size = 512, ns;

    USART_printf(" disk_initalize(%u)", disk_id);
    disk_status = disk_initialize(disk_id);
    if (disk_status & STA_NOINIT)
    {
        USART_printf(" - failed.\n\r");
        return 2;
    }
    else
    {
        USART_printf(" - ok.\n\r");
    }

    USART_printf("**** Get drive size ****\n\r");
    USART_printf(" disk_ioctl(%u, GET_SECTOR_COUNT, 0x%08X)", disk_id, (UINT) &disk_size);
    disk_size = 0;
    disk_result = disk_ioctl(disk_id, GET_SECTOR_COUNT, &disk_size);
    if (disk_result == RES_OK)
    {
        USART_printf(" - ok.\n\r");
    }
    else
    {
        USART_printf(" - failed.\n\r");
        return 3;
    }
    if (disk_size < 128)
    {
        USART_printf("Failed: Insufficient drive size to test.\n\r");
        return 4;
    }
    USART_printf(" Number of sectors on the drive %u is %lu.\n\r", disk_id, disk_size);

    USART_printf("**** Single sector write test ****\n\r");
    for (n = 0, pn(pns); n < sector_size; n++)
    {
        buffer[n] = (BYTE) pn(0);
    }
    USART_printf(" disk_write(%u, 0x%X, %lu, 1)", disk_id, (UINT) buffer, sector);

    disk_result = disk_write(disk_id, buffer, sector, 1);
    if (disk_result == RES_OK)
    {
        USART_printf(" - ok.\n\r");
    }
    else
    {
        USART_printf(" - failed.\n\r");
        return 6;
    }

    USART_printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", disk_id);
    disk_result = disk_ioctl(disk_id, CTRL_SYNC, 0);
    if (disk_result == RES_OK)
    {
        USART_printf(" - ok.\n\r");
    }
    else
    {
        USART_printf(" - failed.\n\r");
        return 7;
    }

    memset(buffer, 0, sector_size);
    USART_printf(" disk_read(%u, 0x%X, %lu, 1)", disk_id, (UINT) buffer, sector);
    disk_result = disk_read(disk_id, buffer, sector, 1);
    if (disk_result == RES_OK)
    {
        USART_printf(" - ok.\n\r");
    }
    else
    {
        USART_printf(" - failed.\n\r");
        return 8;
    }

    for (n = 0, pn(pns); n < sector_size && buffer[n] == (BYTE) pn(0); n++)
        ;
    if (n == sector_size)
    {
        USART_printf(" Read data matched.\n\r");
    }
    else
    {
        USART_printf(" Read data differs from the data written.\n\r");
    }
    pns++;

    USART_printf("**** Multiple sector write test ****\n\r");
    USART_printf("Test skipped.\n\r");
    USART_printf("**** Single sector write test (unaligned buffer address) ****\n\r");
    sector = 5;
    for (n = 0, pn(pns); n < sector_size; n++)
        buffer[n + 3] = (BYTE) pn(0);
    USART_printf(" disk_write(%u, 0x%X, %lu, 1)", disk_id, (UINT) (buffer + 3), sector);
    disk_result = disk_write(disk_id, buffer + 3, sector, 1);
    if (disk_result == RES_OK)
    {
        USART_printf(" - ok.\n\r");
    }
    else
    {
        USART_printf(" - failed.\n\r");
        return 15;
    }
    USART_printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", disk_id);
    disk_result = disk_ioctl(disk_id, CTRL_SYNC, 0);
    if (disk_result == RES_OK)
    {
        USART_printf(" - ok.\n\r");
    }
    else
    {
        USART_printf(" - failed.\n\r");
        return 16;
    }
    memset(buffer + 5, 0, sector_size);
    USART_printf(" disk_read(%u, 0x%X, %lu, 1)", disk_id, (UINT) (buffer + 5), sector);
    disk_result = disk_read(disk_id, buffer + 5, sector, 1);
    if (disk_result == RES_OK)
    {
        USART_printf(" - ok.\n\r");
    }
    else
    {
        USART_printf(" - failed.\n\r");
        return 17;
    }
    for (n = 0, pn(pns); n < sector_size && buffer[n + 5] == (BYTE) pn(0); n++)
        ;
    if (n == sector_size)
    {
        USART_printf(" Read data matched.\n\r");
    }
    else
    {
        USART_printf(" Read data differs from the data written.\n\r");
        return 18;
    }
    pns++;
    uint32_t sector2;

    USART_printf("**** 4GB barrier test ****\n\r");
    if (disk_size >= 128 + 0x80000000 / (sector_size / 2))
    {
        sector = 6;
        sector2 = sector + 0x80000000 / (sector_size / 2);
        for (n = 0, pn(pns); n < (UINT) (sector_size * 2); n++)
            buffer[n] = (BYTE) pn(0);
        USART_printf(" disk_write(%u, 0x%X, %lu, 1)", disk_id, (UINT) buffer, sector);
        disk_result = disk_write(disk_id, buffer, sector, 1);
        if (disk_result == RES_OK)
        {
            USART_printf(" - ok.\n\r");
        }
        else
        {
            USART_printf(" - failed.\n\r");
            return 19;
        }
        USART_printf(" disk_write(%u, 0x%X, %lu, 1)", disk_id, (UINT) (buffer + sector_size), sector2);
        disk_result = disk_write(disk_id, buffer + sector_size, sector2, 1);
        if (disk_result == RES_OK)
        {
            USART_printf(" - ok.\n\r");
        }
        else
        {
            USART_printf(" - failed.\n\r");
            return 20;
        }
        USART_printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", disk_id);
        disk_result = disk_ioctl(disk_id, CTRL_SYNC, 0);
        if (disk_result == RES_OK)
        {
            USART_printf(" - ok.\n\r");
        }
        else
        {
            USART_printf(" - failed.\n\r");
            return 21;
        }
        memset(buffer, 0, sector_size * 2);
        USART_printf(" disk_read(%u, 0x%X, %lu, 1)", disk_id, (UINT) buffer, sector);
        disk_result = disk_read(disk_id, buffer, sector, 1);
        if (disk_result == RES_OK)
        {
            USART_printf(" - ok.\n\r");
        }
        else
        {
            USART_printf(" - failed.\n\r");
            return 22;
        }
        USART_printf(" disk_read(%u, 0x%X, %lu, 1)", disk_id, (UINT) (buffer + sector_size), sector2);
        disk_result = disk_read(disk_id, buffer + sector_size, sector2, 1);
        if (disk_result == RES_OK)
        {
            USART_printf(" - ok.\n\r");
        }
        else
        {
            USART_printf(" - failed.\n\r");
            return 23;
        }
        for (n = 0, pn(pns); buffer[n] == (BYTE) pn(0) && n < (UINT) (sector_size * 2); n++)
            ;
        if (n == (UINT) (sector_size * 2))
        {
            USART_printf(" Read data matched.\n\r");
        }
        else
        {
            USART_printf(" Read data differs from the data written.\n\r");
            return 24;
        }
    }
    else
    {
        USART_printf(" Test skipped.\n\r");
    }

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
    USART_WriteLine((char*) Data);
    _delay_ms(5);
    GIFR |= _BV(INTF1);
}