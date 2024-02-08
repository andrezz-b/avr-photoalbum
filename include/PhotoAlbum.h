#ifndef PHOTO_ALBUM_H
#define PHOTO_ALBUM_H

#include <FAT.h>
#include <File.h>
#include <ImgFolder.h>
#include <SDCard.h>
#include <avr/io.h>
extern "C"
{
#include <ili9341.h>
}

#define TFT_WIDTH  ILI9341_MAX_X
#define TFT_HEIGHT ILI9341_MAX_Y
#define BUFFPIXEL  240

#define IMG_CTRL_PORT PORTA
#define IMG_CTRL_PIN  PINA
#define IMG_CTRL_DDR  DDRA
#define IMG_NEXT      0
#define IMG_PREV      1

#define DEBUG_SERIAL

#if defined(DEBUG_SERIAL)
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

class PhotoAlbum
{
public:
    PhotoAlbum();
    ~PhotoAlbum();

    void init();
    void listen_for_input();

private:
    bool button_pressed(uint8_t button_pin);
    void draw_image();

    static void bmp_draw(File& bmpFile, uint8_t x, uint8_t y);

    SDCard disk;
    FAT fs;
    File root_dir;
    File current_file;
    bool image_changed;
    ImgFolder imgFolder;
};

#endif // PHOTO_ALBUM_H