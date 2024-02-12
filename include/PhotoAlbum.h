#ifndef PHOTO_ALBUM_H
#define PHOTO_ALBUM_H

#include <FAT.h>
#include <File.h>
#include <ImgFolder.h>
#include <SDCard.h>
#include <avr/io.h>
#include <config.h>

typedef struct
{
    int32_t width;
    int32_t height;
    uint32_t data_offset;
} BMPHeader;

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
    void draw_ui();
    void draw_title_screen();

    static void bmp_draw(File& bmpFile, uint8_t x, uint8_t y);
    static bool parse_bmp_header(File&, BMPHeader&);

    SDCard disk;
    FAT fs;
    File root_dir;
    File current_file;
    bool image_changed;
    ImgFolder imgFolder;
};

#endif // PHOTO_ALBUM_H