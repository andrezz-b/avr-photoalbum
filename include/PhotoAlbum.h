/**
 * @file PhotoAlbum.h
 * @brief The main class that runs the project.
 */
#ifndef PHOTO_ALBUM_H
#define PHOTO_ALBUM_H

#include <FAT.h>
#include <File.h>
#include <ImgFolder.h>
#include <SDCard.h>
#include <avr/io.h>
#include <config.h>

/**
 * @struct BMPHeader
 * @brief Structure representing the header of a BMP image file.
 * @details The BMP header contains the width and height of the image, as well as the offset to the
 * image data. This is not the full BMP header, but it is enough to read the image data from the
 * file.
 */
typedef struct
{
    int32_t width;        /**< The width of the image. */
    int32_t height;       /**< The height of the image. */
    uint32_t data_offset; /**< The offset to the image data. */
} BMPHeader;

/**
 * @class PhotoAlbum
 * @brief The main class that runs the project.
 */
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

    static bool parse_bmp_header(File& file, BMPHeader& header);

    SDCard disk;         /// The SD card object. 
    FAT fs;              /// The FAT file system object. 
    File root_dir;       /// The root directory of the photo album. 
    File current_file;   /// The currently displayed file. 
    bool image_changed;  /// Flag indicating if the image has changed. 
    ImgFolder imgFolder; /// The image folder object. 
};

#endif // PHOTO_ALBUM_H