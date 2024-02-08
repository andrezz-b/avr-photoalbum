/**
 * @file main.cpp
 *
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * GEPID - Grupo de Pesquisa em Cultura Digital (http://gepid.upf.br/)
 * Universidade de Passo Fundo (http://www.upf.br/)
 *
 * @copyright
 * Copyright (C) 2018 by Angelo Elias Dalzotto
 *
 * @brief This is a pure AVR port of the Arduino Sd2Card Library.
 * This file has the example for the library.
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino Sd2Card Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <FAT.h>
#include <File.h>
#include <ImgFolder.h>
#include <SDCard.h>
#include <SPI.h>
#include <avr/io.h>
#include <serial.h>
#include <util/delay.h>
extern "C"
{
#include <ili9341.h>
}

#define TFT_WIDTH  ILI9341_MAX_X
#define TFT_HEIGHT ILI9341_MAX_Y

#define IMG_CTRL_PORT PORTA
#define IMG_CTRL_PIN PINA
#define IMG_CTRL_DDR DDRA
#define IMG_NEXT 0
#define IMG_PREV 1

SDCard disk(&PORTB, &DDRB, PB4);
FAT fs(&disk);
File root(&fs);
File file(&fs);

void handle_error()
{
    switch (disk.get_error())
    {
        case SDCard::Error::CMD0:
            printf("timeout error for command CMD0\n");
            break;
        case SDCard::Error::CMD8:
            printf("CMD8 was not accepted - not a valid SD card\n");
            break;
        case SDCard::Error::ACMD41:
            printf("card's ACMD41 initialization process timeout\n");
            break;
        case SDCard::Error::CMD58:
            printf("card returned an error response for CMD58 (read OCR)\n");
            break;
        case SDCard::Error::CMD24:
            printf("card returned an error response for CMD24 (write block)\n");
            break;
        default:
            printf("Unknown error. Code %x\n", (uint8_t) disk.get_error());
            break;
    }
}
void bmpDraw(File& dir, char* filename, uint8_t x, uint16_t y);
void bmpDraw(File& bmpFile, uint8_t x, uint8_t y);
uint32_t read32(File& f);
uint16_t read16(File& f);

int main()
{
    uart_init();
    printf("Initializing SD card...\n");
    if (disk.init())
    {
        printf("Card connected!\n");
    }
    else
    {
        printf("Card initialization failed.\n");
        handle_error();
    }
    SPI::set_speed();

    ILI9341_Init();
    ILI9341_ClearScreen(ILI9341_BLACK);

    printf("\nMounting FAT Filesystem...\n");
    if (fs.mount())
    {
        printf("Filesystem mounted!\n");
    }
    else
    {
        printf("Mount error.\n");
        handle_error();
    }

    printf("\nOpening filesystem root...\n");
    if (root.open_root())
    {
        printf("Root is open\n");
    }
    else
    {
        printf("Unable to open root\n");
        handle_error();
    }
    File imgDir(&fs);
    if (!imgDir.open(root, "img", File::O_RDONLY))
    {
        printf("Unable to open img folder\n");
        handle_error();
    }

    ImgFolder imgFolder(imgDir);
    imgFolder.first_file(file);
    bool imgChanged = true;
    // PA0 = next image, PA1 = prev image, set as input with pullup
    IMG_CTRL_DDR &= ~(_BV(IMG_NEXT) | _BV(IMG_PREV));
    IMG_CTRL_PORT |= _BV(IMG_NEXT) | _BV(IMG_PREV);
    while (1)
    {
        if (!(IMG_CTRL_PIN & _BV(IMG_NEXT)))
        {
            if (!imgFolder.next_file(file))
            {
                printf("Unable to open next file\n");
            }
            else
            {
                imgChanged = true;
            }
        }

        if (!(IMG_CTRL_PIN & _BV(IMG_PREV)))
        {
            if (!imgFolder.prev_file(file))
            {
                printf("Unable to open prev file\n");
            }
            else
            {
                imgChanged = true;
            }
        }
        if (imgChanged)
        {
            ILI9341_ClearScreen(ILI9341_BLACK);
            bmpDraw(file, 0, 0);
            imgChanged = false;
        }
        _delay_ms(50);
    }

    return 0;
}

#define BUFFPIXEL 60

void bmpDraw(File& dir, char* filename, uint8_t x, uint16_t y)
{
    File bmpFile(&fs);
    if ((x >= TFT_WIDTH) || (y >= TFT_HEIGHT))
        return;

    printf("Loading image %s\n", filename);

    // Open requested file on SD card
    if (!bmpFile.open(dir, filename, File::O_RDONLY))
    {
        printf("File not found\n");
        return;
    }
}

void bmpDraw(File& bmpFile, uint8_t x, uint8_t y)
{
    if ((x >= TFT_WIDTH) || (y >= TFT_HEIGHT))
        return;

    int bmpWidth, bmpHeight;            // W+H in pixels
    uint8_t bmpDepth;                   // Bit depth (currently must be 24)
    uint32_t bmpImageoffset;            // Start of image data in file
    uint32_t rowSize;                   // Not always = bmpWidth; may have padding
    uint8_t sdbuffer[3 * BUFFPIXEL];    // pixel buffer (R+G+B per pixel)
    uint8_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
    bool goodBmp = false;               // Set to true on valid header parse
    bool flip = true;                   // BMP is stored bottom-to-top
    int w, h, row, col;
    uint8_t r, g, b;
    uint32_t pos = 0;

    // Parse BMP header
    if (read16(bmpFile) == 0x4D42)
    { // BMP signature
        printf("File size: %d\n", read32(bmpFile));
        (void) read32(bmpFile);           // Read & ignore creator bytes
        bmpImageoffset = read32(bmpFile); // Start of image data
        printf("Image Offset: %d\n", bmpImageoffset);
        // Read DIB header
        printf("Header size: %d\n", read32(bmpFile));
        bmpWidth = read32(bmpFile);
        bmpHeight = read32(bmpFile);
        if (read16(bmpFile) == 1)
        {                               // # planes -- must be '1'
            bmpDepth = read16(bmpFile); // bits per pixel
            printf("Bit depth: %d\n", bmpDepth);
            if ((bmpDepth == 24) && (read32(bmpFile) == 0))
            { // 0 = uncompressed

                goodBmp = true; // Supported BMP format -- proceed!
                printf("Image size: %d x %d\n", bmpWidth, bmpHeight);

                // BMP rows are padded (if needed) to 4-byte boundary
                rowSize = (bmpWidth * 3 + 3) & ~3;

                // If bmpHeight is negative, image is in top-down order.
                // This is not canon but has been observed in the wild.
                if (bmpHeight < 0)
                {
                    bmpHeight = -bmpHeight;
                    flip = false;
                }

                // Crop area to be loaded
                w = bmpWidth;
                h = bmpHeight;
                if ((x + w - 1) >= TFT_WIDTH)
                    w = TFT_WIDTH - x;
                if ((y + h - 1) >= TFT_HEIGHT)
                    h = TFT_HEIGHT - y;

                // Set TFT address window to clipped image bounds

                for (row = 0; row < h; row++)
                { // For each scanline...

                    // Seek to start of scan line.  It might seem labor-
                    // intensive to be doing this on every line, but this
                    // method covers a lot of gritty details like cropping
                    // and scanline padding.  Also, the seek only takes
                    // place if the file position actually needs to change
                    // (avoids a lot of cluster math in SD library).
                    if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
                        pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
                    else // Bitmap is stored top-to-bottom
                        pos = bmpImageoffset + row * rowSize;
                    if (bmpFile.get_current_position() != pos)
                    { // Need seek?
                        bmpFile.seek(pos);
                        buffidx = sizeof(sdbuffer); // Force buffer reload
                    }

                    for (col = 0; col < w; col++)
                    { // For each pixel...
                        // Time to read more pixel data?
                        if (buffidx >= sizeof(sdbuffer))
                        { // Indeed
                            bmpFile.read(sdbuffer, sizeof(sdbuffer));
                            buffidx = 0; // Set index to beginning
                        }

                        // Convert pixel from BMP to TFT format, push to display
                        b = sdbuffer[buffidx++];
                        g = sdbuffer[buffidx++];
                        r = sdbuffer[buffidx++];
                        // printf("c1: %d, c2: %d, c3: %d\n", r, g, b);
                        uint16_t color565 = (r & 0xF8) << 8 | (g & 0xFC) << 3 | b >> 3;
                        // printf("color: %02X\n", color565);
                        ILI9341_DrawPixel(col, row, color565);
                    } // end pixel
                }     // end scanline
            }         // end goodBmp
        }
    }

    bmpFile.close();
    if (!goodBmp)
        printf("BMP format not recognized.\n");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File& f)
{
    uint16_t result;
    ((uint8_t*) &result)[0] = f.read(); // LSB
    ((uint8_t*) &result)[1] = f.read(); // MSB
    return result;
}

uint32_t read32(File& f)
{
    uint32_t result;
    ((uint8_t*) &result)[0] = f.read(); // LSB
    ((uint8_t*) &result)[1] = f.read();
    ((uint8_t*) &result)[2] = f.read();
    ((uint8_t*) &result)[3] = f.read(); // MSB
    return result;
}