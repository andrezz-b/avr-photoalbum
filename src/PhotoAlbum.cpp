#include <PhotoAlbum.h>
#include <SPI.h>
#include <stdlib.h>
#if defined(DEBUG_SERIAL)
#include <serial.h>
#endif
PhotoAlbum::PhotoAlbum()
    : disk(&PORTB, &DDRB, PB4),
      fs(&disk),
      root_dir(&fs),
      current_file(&fs),
      image_changed(false),
      imgFolder(&fs)
{
}

PhotoAlbum::~PhotoAlbum()
{
}

void PhotoAlbum::init()
{
#if defined(DEBUG_SERIAL)
    uart_init();
#endif
    DEBUG("Initializing SD card...\n");
    if (disk.init())
    {
        DEBUG("Card connected!\n");
    }
    else
    {
        DEBUG("Card initialization failed.\n");
    }
    SPI::set_speed();

    /* Display specific code */
    ILI9341_Init();
    ILI9341_ClearScreen(ILI9341_BLACK);
    /* ********************* */

    DEBUG("\nMounting FAT Filesystem...\n");
    if (fs.mount())
    {
        DEBUG("Filesystem mounted!\n");
    }
    else
    {
        DEBUG("Mount error.\n");
    }

    DEBUG("\nOpening filesystem root...\n");
    if (root_dir.open_root())
    {
        DEBUG("Root is open\n");
    }
    else
    {
        DEBUG("Unable to open root\n");
    }

    IMG_CTRL_DDR &= ~(_BV(IMG_NEXT) | _BV(IMG_PREV));
    IMG_CTRL_PORT |= _BV(IMG_NEXT) | _BV(IMG_PREV);

    imgFolder.init(root_dir, "img");
    draw_title_screen();
}

void PhotoAlbum::listen_for_input()
{
    if (button_pressed(IMG_NEXT))
    {
        if (!imgFolder.next_file(current_file))
        {
            DEBUG("Unable to open next file\n");
        }
        else
        {
            image_changed = true;
        }
    }

    if (button_pressed(IMG_PREV))
    {
        if (!imgFolder.prev_file(current_file))
        {
            DEBUG("Unable to open prev file\n");
        }
        else
        {
            image_changed = true;
        }
    }
    if (image_changed)
    {
        draw_image();
        image_changed = false;
    }
}

void PhotoAlbum::draw_title_screen()
{
    ILI9341_SetPosition(55, 94);
    ILI9341_DrawString("URS Fotoalbum", ILI9341_WHITE, ILI9341_Sizes::X3);
    ILI9341_SetPosition(70, 134);
    ILI9341_DrawString("Images found: ", ILI9341_WHITE, ILI9341_Sizes::X1);
    char buffer[3];
    itoa(imgFolder.get_image_count(), buffer, 10);
    ILI9341_DrawString(buffer, ILI9341_WHITE, ILI9341_Sizes::X1);
    ILI9341_SetPosition(70, 154);
    ILI9341_DrawString("Controls: ", ILI9341_WHITE, ILI9341_Sizes::X1);
    ILI9341_SetPosition(75, 164);
    ILI9341_DrawString("--> Next", ILI9341_WHITE, ILI9341_Sizes::X1);
    ILI9341_SetPosition(75, 174);
    ILI9341_DrawString("<-- Prev", ILI9341_WHITE, ILI9341_Sizes::X1);
    ILI9341_SetPosition(70, 204);
    ILI9341_DrawString("Press --> to start", ILI9341_WHITE, ILI9341_Sizes::X1);
}

void PhotoAlbum::draw_image()
{
    ILI9341_ClearScreen(ILI9341_BLACK);
    // Top UI bar - Image name and size
    // Name
    char buffer[16];
    imgFolder.get_current_file_name(buffer);
    strcat(buffer, "  ");
    ILI9341_SetPosition(10, 1);
    ILI9341_DrawString(buffer, ILI9341_WHITE, ILI9341_Sizes::X1);
    // Images
    ILI9341_SetPosition(110, 1);
    itoa(imgFolder.get_index() + 1, buffer, 10);
    strcat(buffer, "/");
    itoa(imgFolder.get_image_count(), buffer + strlen(buffer), 10);
    ILI9341_DrawString(buffer, ILI9341_WHITE, ILI9341_Sizes::X1);
    // Size
    ILI9341_SetPosition(180, 1);
    itoa(current_file.get_file_size() >> 10, buffer, 10);
    strcat(buffer, " KiB");
    ILI9341_DrawString(buffer, ILI9341_WHITE, ILI9341_Sizes::X1);
    // Bottom UI bar - Controls
    // Prev
    if (imgFolder.prev_available())
    {
        ILI9341_SetPosition(50, 311);
        ILI9341_DrawString("<-- Prev", ILI9341_WHITE, ILI9341_Sizes::X1);
    }
    // Split
    ILI9341_SetPosition(95, 311);
    ILI9341_DrawString("   |   ", ILI9341_WHITE, ILI9341_Sizes::X1);
    // Next
    if (imgFolder.next_available())
    {
        ILI9341_DrawString("Next -->", ILI9341_WHITE, ILI9341_Sizes::X1);
    }

    bmp_draw(current_file, 0, 10);
}

bool PhotoAlbum::button_pressed(uint8_t button_pin)
{
    return !(IMG_CTRL_PIN & _BV(button_pin));
}

typedef struct
{
    int32_t width;
    int32_t height;
    uint32_t data_offset;
} BMPHeader;

bool parse_bmp_header(File& bmp_file, BMPHeader& header)
{
    // BMP Signature
    if (File::read16(bmp_file) != 0x4D42)
    {
        return false;
    }

    // File size
    File::read32(bmp_file);

    // Read & ignore creator bytes
    File::read32(bmp_file);

    // Offset to start of image data
    header.data_offset = File::read32(bmp_file);

    // Header Size
    File::read32(bmp_file);

    // Width
    header.width = File::read32(bmp_file);

    // Height
    header.height = File::read32(bmp_file);

    // Number of planes - must be 1
    if (File::read16(bmp_file) != 1)
    {
        return false;
    }

    // Depth - Bits per pixel - only 24 supported
    if (File::read16(bmp_file) != 24)
    {
        return false;
    }

    // Compression - must be uncompressed (0)
    if (File::read32(bmp_file) != 0)
    {
        return false;
    }

    return true;
}

void PhotoAlbum::bmp_draw(File& bmpFile, uint8_t x, uint8_t y)
{
    if ((x >= TFT_WIDTH) || (y >= TFT_HEIGHT))
        return;

    BMPHeader header;
    uint32_t rowSize;                    // Not always = bmpWidth; may have padding
    uint8_t sdbuffer[3 * BUFFPIXEL];     // pixel buffer (R+G+B per pixel)
    uint16_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
    bool flip = true;                    // BMP is stored bottom-to-top
    int w, h, row, col;
    uint8_t r, g, b;
    uint32_t pos = 0;

    if (!parse_bmp_header(bmpFile, header))
    {
        DEBUG("Invalid BMP file\n");
        return;
    }

    // BMP rows are padded (if needed) to 4-byte boundary
    rowSize = (header.width * 3 + 3) & ~3;

    // If bmpHeight is negative, image is in top-down order.
    // This is not canon but has been observed in the wild.
    if (header.height < 0)
    {
        header.height = -header.height;
        flip = false;
    }

    // Crop area to be loaded
    w = header.width;
    h = header.height;
    if ((x + w - 1) >= TFT_WIDTH)
        w = TFT_WIDTH - x;
    if ((y + h - 1) >= TFT_HEIGHT - 20)
        h = TFT_HEIGHT - 10 - y;

    for (row = 0; row < h; row++)
    { // For each scanline...

        // Seek to start of scan line.  It might seem labor-
        // intensive to be doing this on every line, but this
        // method covers a lot of gritty details like cropping
        // and scanline padding.  Also, the seek only takes
        // place if the file position actually needs to change
        // (avoids a lot of cluster math in SD library).
        if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = header.data_offset + (header.height - 1 - row) * rowSize;
        else // Bitmap is stored top-to-bottom
            pos = header.data_offset + row * rowSize;
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
            uint16_t color565 = (r & 0xF8) << 8 | (g & 0xFC) << 3 | b >> 3;
            ILI9341_DrawPixel(col + x, row + y, color565);
        } // end pixel
    }     // end scanline
    // } // end goodBmp
    //     }
    // }

    bmpFile.close();
}
